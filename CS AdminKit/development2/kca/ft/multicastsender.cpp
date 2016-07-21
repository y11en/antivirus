/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file multicastsender.cpp
 * \author Дамир Шияфетдинов
 * \date 2006 
 * 
 */

#include <map>
#include <sstream>

#include <std/base/klbase.h>
#include <std/thr/sync.h>
#include <std/err/klerrors.h>
#include <std/trc/trace.h>
#include <std/par/par_gsoap.h>

#include <std/tmstg/timeoutstore.h>

#include <common/measurer.h>

#include "boost/crc.hpp"

#include <transport/tr/transportproxy.h>

#include <kca/ft/filetransfer.h>
#include <kca/ft/errors.h>

#include "multicastsender.h"

#define KLCS_MODULENAME L"KLFTMC"

namespace KLFT {

	class MulticastSenderImp : public KLSTD::KLBaseImpl<MulticastSender> 
	{	
	public:

		MulticastSenderImp();

		~MulticastSenderImp();
	
		//!\brief Инициализация подсистемы
		virtual void Init( const std::wstring &strMulticastIP, 
			KLPAR::BinaryValue *pPrivateSSLKey,
			FileProvider *pFileProvider,
			int chunkSize = c_MulticastSendChunkSize,
			int chunkTimeout = c_MulticastSendChunkTimeout );

		//!\brief Остановка работы отправки файлов
		virtual void Stop();

		//!\brief Остановка работы отправки файлов
		virtual void SendFile( const std::wstring &fileId, 
			unsigned long fullFileSize );

		//!\brief Иницирования отправки файлов по multicast
		virtual void SendSyncFolderFiles( const std::wstring &syncFolderId, 
			const std::vector<std::wstring> &filesNames );

		//!\brief Производит поиск указанного файла во внтуреннем списке рассылки
		virtual bool IsFileSending( const std::wstring &fileId );

		virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId );

	private:

		struct SendFileDesc
		{
			SendFileDesc()
			{
				fullFileSize = 0;
				currentSendPos = 0;
			}

			SendFileDesc( const SendFileDesc &desc )
			{
				fileId = desc.fileId;
				fullFileSize = desc.fullFileSize;
				currentSendPos = desc.currentSendPos;
				fileName = desc.fileName;
			}

			std::wstring	fileId;
			std::wstring	fileName;
			std::wstring	folderName;
			unsigned long	fullFileSize;
			unsigned long	currentSendPos;
		};

		typedef std::map<std::wstring,SendFileDesc> SendFileMap; 

		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCricSec;

		KLSTD::CAutoPtr<FileProvider>	m_pFileProvider;

		std::wstring					m_multicastIp;

		SendFileMap						m_sendFileMap;

		int								m_chunkSize;
		int								m_chunkTimeout;

		bool							m_workerStopFlag;
		bool							m_workerAdded;
		KLTP::ThreadsPool::WorkerId		m_workerId;

	protected:

		void StopSenderWorker( KLTP::ThreadsPool::WorkerId wId );

	};

	MulticastSenderImp::MulticastSenderImp()
	{
		KLSTD_CreateCriticalSection( &m_pCricSec );
		m_workerAdded = false;
		m_workerStopFlag = false;
	}

	MulticastSenderImp::~MulticastSenderImp()
	{
	}

	//!\brief Инициализация подсистемы
	void MulticastSenderImp::Init( const std::wstring &strMulticastIP, 
			KLPAR::BinaryValue *pPrivateSSLKey,
			FileProvider *pFileProvider,
			int chunkSize,
			int chunkTimeout )
	{
		if ( strMulticastIP.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );
		if ( chunkSize<=0 || chunkTimeout<=0 )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );
		if ( pFileProvider==NULL )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
		m_pFileProvider = pFileProvider;

		m_chunkSize = chunkSize;
		m_chunkTimeout = chunkTimeout;
		m_multicastIp = strMulticastIP;

		KLTR_GetTransport()->CreateMulticastGroupSender( m_multicastIp.c_str(),
			pPrivateSSLKey );
	}

	//!\brief Остановка работы отправки файлов
	void MulticastSenderImp::Stop()
	{
		KLTP::ThreadsPool::WorkerId wId;
		bool needDeleteWorker = false;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_workerAdded )
			{
				wId = m_workerId;
				m_workerAdded = false;
				needDeleteWorker = true;
				m_workerStopFlag = true;
			}
		}

		if ( needDeleteWorker ) 
		{
			KLTP_GetThreadsPool()->DeleteWorker( wId );
		}

		std::wstring multicastIp;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			m_pFileProvider = NULL;
			multicastIp = m_multicastIp;
			m_multicastIp.clear();
		}
		
		if ( !multicastIp.empty() )
		{
			KLTR_GetTransport()->DeleteMulticastConnection( m_multicastIp.c_str() );
		}
	}

	//!\brief Остановка работы отправки файлов
	void MulticastSenderImp::SendFile( const std::wstring &fileId, 
			unsigned long fullFileSize )
	{
		SendFileDesc fDesc;

		fDesc.fileId = fileId;
		fDesc.fullFileSize = fullFileSize;

		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( IsFileSending( fileId ) )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );
		
		m_sendFileMap[fileId] = fDesc;

		KLSTD_TRACE2( 1, L"MulticastSenderImp::SendFile. File id - '%ls' fullFileSize - %d.\n",
				fileId.c_str(), fullFileSize );

		if ( !m_workerAdded )
		{
			m_workerAdded = true;

			KLTP_GetThreadsPool()->AddWorker( &m_workerId, L"MulticastSenderWorker", 
				this, KLTP::ThreadsPool::RunNormal, true, m_chunkTimeout );
		}
	}

	//!\brief Иницирования отправки файлов по multicast
	void MulticastSenderImp::SendSyncFolderFiles( const std::wstring &syncFolderId, 
			const std::vector<std::wstring> &filesNames )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( IsFileSending( syncFolderId ) )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, syncFolderId.c_str() );

		std::vector<std::wstring>::const_iterator it = filesNames.begin();
		for( ; it != filesNames.end(); ++it )
		{
			SendFileDesc fDesc;

			fDesc.fileId = syncFolderId;
			fDesc.fileName = *it;

			m_sendFileMap[syncFolderId+(*it)] = fDesc;
		}

		KLSTD_TRACE2( 1, L"MulticastSenderImp::SendSyncFolderFiles. syncFolderId - '%ls' filesNames.size() - %d.\n",
				syncFolderId.c_str(), filesNames.size() );

		if ( !m_workerAdded )
		{
			m_workerAdded = true;

			KLTP_GetThreadsPool()->AddWorker( &m_workerId, L"MulticastSenderWorker", 
				this, KLTP::ThreadsPool::RunNormal, true, m_chunkTimeout );
		}
	}

	//!\brief Производит поиск указанного файла во внтуреннем списке рассылки
	bool MulticastSenderImp::IsFileSending( const std::wstring &fileId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_sendFileMap.find( fileId )!=m_sendFileMap.end() ) return true;
		else return false;
	}

	void MulticastSenderImp::StopSenderWorker( KLTP::ThreadsPool::WorkerId wId )
	{		
		KLTP_GetThreadsPool()->DeleteWorker( wId );
	}

	int MulticastSenderImp::RunWorker( KLTP::ThreadsPool::WorkerId wId )
	{
		std::vector<std::wstring> mapFilesIds;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( wId!=m_workerId ) return 0;

			SendFileMap::iterator it = m_sendFileMap.begin();
			for( ; it != m_sendFileMap.end(); ++it )
			{
				mapFilesIds.push_back( it->first );
			}
		}

		std::wstringstream s;
        s << L"MulticastSender multicast file. mapFilesIds.size() - " << mapFilesIds.size() << ".";
		KL_TMEASURE_BEGIN( s.str().c_str(), 2 );

		std::vector<std::wstring>::iterator it = mapFilesIds.begin();
		while( it != mapFilesIds.end() )
		{
			bool endOfFile = false;

			while( !endOfFile )
			{
				KLERR_BEGIN
				{
					SendFileDesc fDesc;
					{
						KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
										
						if ( m_workerStopFlag ) break;
						if ( m_sendFileMap.find( *it )==m_sendFileMap.end() )  break;

						if ( !m_sendFileMap[*it].fileName.empty() && m_sendFileMap[*it].fullFileSize==0 )
						{
							m_pFileProvider->GetFileInfo( m_sendFileMap[*it].fileId,
								m_sendFileMap[*it].fileName, m_sendFileMap[*it].fullFileSize,
								m_sendFileMap[*it].folderName );
						}
						
						fDesc = m_sendFileMap[*it];
					}

					KLSTD::CArrayPointer2<char> pFileChunk;			
					int chunkSize = m_chunkSize;
					if ( fDesc.currentSendPos + chunkSize > fDesc.fullFileSize  )
					{
						chunkSize = fDesc.fullFileSize - fDesc.currentSendPos;
					}

					pFileChunk = new char[chunkSize];
					int ns = m_pFileProvider->GetFileChunk( fDesc.fileId, fDesc.fileName, 
						fDesc.currentSendPos,
						pFileChunk.get(), chunkSize, endOfFile );
					if ( ns!=chunkSize )
					{
						KLSTD_TRACE4( 1, L"MulticastSender error ( can't get next file chunk ). File id - '%ls' chunk size - %d pos - %d full size - %d\n",
									fDesc.fileId.c_str(), chunkSize, fDesc.currentSendPos, fDesc.fullFileSize );

						{
							KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

							m_sendFileMap.erase( fDesc.fileId );							
							it = mapFilesIds.begin();
						}

						break;
					}

					KLSTD::CPointer<boost::crc_32_type> crc32;
					crc32=new boost::crc_32_type;
					KLSTD_CHKMEM(crc32);
					crc32->process_bytes( pFileChunk.get(), chunkSize );
					unsigned long CRC = crc32->checksum();

					KLPAR::param_entry_t chunkpar[]=
					{
						KLPAR::param_entry_t( SER_MC_FILE_ID, fDesc.fileId.c_str() ),
						KLPAR::param_entry_t( SER_MC_FILE_NAME, fDesc.fileName.c_str() ),
						KLPAR::param_entry_t( SER_MC_FOLDER_NAME, fDesc.folderName.c_str() ),
						KLPAR::param_entry_t( SER_MC_FULL_SIZE, (long)fDesc.fullFileSize ),
						KLPAR::param_entry_t( SER_MC_CHUNK_POS, (long)fDesc.currentSendPos ),
						KLPAR::param_entry_t( SER_MC_CHUNK_SIZE, (long)chunkSize ),
						KLPAR::param_entry_t( SER_MC_CHUNK_CRC, (long )CRC )
					};			
					KLSTD::CAutoPtr<KLPAR::Params> pChunkParams;
					KLPAR::CreateParamsBody(chunkpar, KLSTD_COUNTOF(chunkpar), &pChunkParams);

					KLSTD::CAutoPtr<KLPAR::ValuesFactory> pFactory;

					KLSTD::CAutoPtr<KLPAR::BinaryValue> pBinData;
					KLPAR_CreateValuesFactory(&pFactory);			
					pFactory->CreateBinaryValue(&pBinData);
					pBinData->SetValue(pFileChunk.get(), chunkSize );
					pChunkParams->AddValue( SER_MC_CHUNK_DATA, pBinData );

					KLSTD::CAutoPtr<KLSTD::MemoryChunk> pParMemChunk;
					KLPAR_SerializeToMemory2( KLPAR_SF_BINARY, pChunkParams, &pParMemChunk );

					pBinData->SetValue( pParMemChunk->GetDataPtr(), pParMemChunk->GetDataSize() );
					KLTR_GetTransport()->SendMulticastPacket( m_multicastIp.c_str(), pBinData );

					fDesc.currentSendPos += chunkSize;

					{
						KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
										
						if ( m_workerStopFlag ) break;
						if ( m_sendFileMap.find( *it )==m_sendFileMap.end() )  break;
						
						if ( endOfFile )
						{
							m_sendFileMap.erase( *it );
							it = mapFilesIds.begin();

							KLSTD_TRACE1( 3, L"MulticastSender sender ( file was multicasted ). File id - '%ls'\n",
									fDesc.fileId.c_str() );
						}
						else
						{
							m_sendFileMap[*it] = fDesc;
						}
					}

					if ( endOfFile )
					{
						m_pFileProvider->OnFileSent( fDesc.fileId, fDesc.fileName, fDesc.currentSendPos );
					}
					else
					{
						KLSTD_Sleep( 30 );
					}
				}
				KLERR_ENDT(1);
			}

			if ( !endOfFile ) ++it;
		}

		KL_TMEASURE_END();

		if ( m_sendFileMap.empty() )
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_workerAdded )
			{
				m_workerAdded = false;
				KLTP::ThreadsPool::WorkerId wId = m_workerId;
				m_workerId = (-1);
				
				KLTMSG::AsyncCall1T<MulticastSenderImp, KLTP::ThreadsPool::WorkerId>
						::Start(this, &MulticastSenderImp::StopSenderWorker, wId);
			}
		}

		return 0;
	}
	
} // end namespace KLFT


KLCSKCA_DECL void KLFT_CreateMulticastSender( KLFT::MulticastSender **ppMCSender )
{
	KLSTD_CHKOUTPTR(ppMCSender);
	*ppMCSender = new KLFT::MulticastSenderImp();
	KLSTD_CHKMEM(*ppMCSender);
}
