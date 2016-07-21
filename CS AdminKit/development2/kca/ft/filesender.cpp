/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file filesender.h
 * \author Дамир Шияфетдинов
 * \date 2006
 * \brief Интерфейс объекта для передачи файлов при помощи File Transfer
 * 
 */

#include <std/base/klbase.h>
#include <std/thr/sync.h>
#include <std/err/klerrors.h>
#include <std/par/par_gsoap.h>

#include <transport/tr/errors.h>

#include "boost/crc.hpp"

#include <transport/tr/errors.h>
#include <transport/tr/transportproxy.h>

#include <kca/ft/filetransfer.h>
#include <kca/ft/errors.h>
#include <kca/ft/filesender.h>

#include "file.h"

#include "soapH.h"

#define KLCS_MODULENAME L"KLFT"

const int PERCENT_FOR_ARCHIVATE	= 10;

namespace KLFT {

	class FileSenderImp : public KLSTD::KLBaseImpl<FileSender> 
	{	
	public:

		// Worker 
		class FileSenderWorker : public KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
		{
		public:

			FileSenderWorker( FileSenderImp *fileSender )
			{
				m_fileSender = fileSender;
			}

			
			virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId )
			{
				return m_fileSender->RunWorker( wId );
			}

		public:
			
			FileSenderImp		*m_fileSender;
		};

	public:

		FileSenderImp();
		~FileSenderImp();

		//!\brief Функция назначает соединения для использования
		virtual void SetConnection( const std::wstring &localConnName,
			const std::wstring &remoteConnName );

		//!\brief Функция отправки файла на сервер FileTransfer
		virtual void SendFile( std::wstring &fileId,
			FileInfo *filesArray = NULL, int filesArraySize = 0,
			int lifeTime = c_DefaultSendFileTimeout );

		//!\brief Функция прерывает отправку файла 
		virtual void CancelSendFile( const std::wstring &fileId );

		//!\brief Функция ожидания окончания операции отправки файла на сервер
		virtual bool WaitFileSent( const std::wstring &fileId, int waitTimeout = KLSTD_INFINITE );

		//!\brief Получение текущего статуса отправляемого файла
		virtual void GetFileStatus( const std::wstring &fileId, 
			SendFileStatus &fileStatus );
		

		int RunWorker( KLTP::ThreadsPool::WorkerId wId );

	private:

		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCricSec;

		KLTRAP::Transport::ConnectionFullName	m_connName;

		KLSTD::CAutoPtr<File>					m_pSentFile;

		SendFileStatus							m_fileStatus;

		bool									m_stopSendFlag;

		KLSTD::CAutoPtr<KLSTD::Semaphore>		m_pWaitSem;

		KLTP::ThreadsPool::WorkerId				m_wId;

		std::wstring							m_fileId;

		KLFT::FileSender::FileInfoList			m_filesArray;
	};

	FileSenderImp::FileSenderImp()
	{
		KLSTD_CreateCriticalSection( &m_pCricSec );
	}

	FileSenderImp::~FileSenderImp()
	{
	}

	//!\brief Функция назначает соединения для использования
	void FileSenderImp::SetConnection( const std::wstring &localConnName,
			const std::wstring &remoteConnName )
	{
		m_connName.localComponentName = localConnName;
		m_connName.remoteComponentName = remoteConnName;
	}

	//!\brief Функция отправки файла на сервер FileTransfer
	void FileSenderImp::SendFile( std::wstring &fileId,
			FileInfo *filesArray, int filesArraySize,
			int lifeTime )
	{	
		if ( m_connName.localComponentName.empty() || m_connName.remoteComponentName.empty() )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		if ( !KLTR_GetTransport()->IsConnectionActive( m_connName.localComponentName.c_str(),
			m_connName.remoteComponentName.c_str() ) )
			KLFT_THROW_ERROR( ERR_CONNECTION_BROKEN );

		if ( filesArray==NULL || filesArraySize==0 )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		if ( filesArray!=NULL && filesArraySize!=0 )
		{
			for( int i = 0; i < filesArraySize; i++ )
			{
				if( !KLSTD_IfExists( filesArray[i].sourceFilePath.c_str() ) )
				{
					KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, filesArray[i].sourceFilePath.c_str() );
				}
			}
		}

		if ( m_pSentFile!=NULL )
			KLFT_THROW_ERROR( ERR_FILE_IS_ALREADY_UPLOADING );

		KLERR_TRY
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			fileId = KLSTD_CreateGUIDString();
			
			m_fileId = fileId;

			m_filesArray.clear();
			for( int i = 0; i < filesArraySize; i++ )
			{
				m_filesArray.push_back( filesArray[i] );
			}			

			m_pWaitSem = NULL;
			KLSTD_CreateSemaphore( &m_pWaitSem, 0 );

			KLTP_GetThreadsPool()->AddWorker( &m_wId,
					L"File sender worker", new FileSenderWorker( this ), 
					KLTP::ThreadsPool::RunOnce );
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );

			m_pSentFile = NULL;

			KLERR_RETHROW();
		}
		KLERR_ENDTRY
	}

	//!\brief Функция прерывает отправку файла 
	void FileSenderImp::CancelSendFile( const std::wstring &fileId )
	{
		KLTP::ThreadsPool::WorkerId wId;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_pSentFile!=NULL && m_pSentFile->GetFileId()!=fileId )
				KLFT_THROW_ERROR( ERR_WRONG_FILE_ID );

			wId = m_wId;
			m_wId = (-1);

			m_stopSendFlag = true;

			m_pSentFile = NULL;

			KLERR_BEGIN
			{
				KLTRAP::TransportProxy trProxy;
				trProxy.Initialize( m_connName.localComponentName.c_str(), m_connName.remoteComponentName.c_str() );

				KLTRAP::TransportConnectionLocker trLocker( &trProxy );

				struct soap* pSoap = trLocker.Get();

				struct klft_CancelServerFileUploadResponse res;

				soap_call_klft_CancelServerFileUpload( pSoap, NULL, NULL,
					(wchar_t *)fileId.c_str(),					
					res );

				trLocker.CheckResult();

				if ( res.error.code )
				{
					KLERR::Error *pError=NULL;
					KLPAR::ExceptionFromSoap( res.error, &pError );
					throw pError;
				}

			}KLERR_ENDT(3);
		}

		KLTP_GetThreadsPool()->DeleteWorker( wId );
		
	}

	//!\brief Функция ожидания окончания операции отправки файла на сервер
	bool FileSenderImp::WaitFileSent( const std::wstring &fileId,
		int waitTimeout )
	{
		KLSTD::CAutoPtr<KLSTD::Semaphore>		pWaitSem;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			if ( m_fileId!=fileId ) KLFT_THROW_ERROR( ERR_WRONG_FILE_ID );

			pWaitSem = m_pWaitSem;

			if ( pWaitSem==NULL )
				KLFT_THROW_ERROR( ERR_WRONG_FILE_ID );

			if ( m_fileStatus.errorCode!=0 ) KLFT_THROW_ERROR_CODE( m_fileStatus.errorCode );
		}

		bool waitRes = pWaitSem->Wait( waitTimeout );
		if ( m_fileStatus.errorCode!=0 ) KLFT_THROW_ERROR_CODE( m_fileStatus.errorCode );

		return waitRes;
	}

	//!\brief Получение текущего статуса отправляемого файла
	void FileSenderImp::GetFileStatus( const std::wstring &fileId, 
			SendFileStatus &fileStatus )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_fileId!=fileId ) KLFT_THROW_ERROR( ERR_WRONG_FILE_ID );
		
		fileStatus = m_fileStatus;			

		if ( fileStatus.sentPercent < PERCENT_FOR_ARCHIVATE && m_pSentFile!=NULL )
		{
			int iArchPercent = m_pSentFile->GetArchivatePercent();
			fileStatus.sentPercent = (int)(((double)iArchPercent / 100.0) * (double)PERCENT_FOR_ARCHIVATE);
		}
	}
		

	int FileSenderImp::RunWorker( KLTP::ThreadsPool::WorkerId wId )
	{
		KLERR_TRY
		{	
			if ( m_pSentFile==NULL )
			{
				KLSTD::CAutoPtr<File> pFile;
				std::wstring fileId;
				std::vector<FileInfo> filesArray;
				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

					m_pSentFile.Attach( new File() );
					KLSTD_CHKMEM(m_pSentFile);
					m_fileStatus.Init();

					pFile = m_pSentFile;
					m_stopSendFlag = false;
					fileId = m_fileId;

					filesArray = m_filesArray;
				}
				
				pFile->CreateSendToFile( &filesArray[0], filesArray.size(), fileId );

				KLTRAP::TransportProxy trProxy;

				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

					if ( m_stopSendFlag || m_pSentFile==NULL ) return 0;
					
					m_fileStatus.fullFileSize = (m_pSentFile->GetFileDescription().m_fileSize);
				
					trProxy.Initialize( m_connName.localComponentName.c_str(), m_connName.remoteComponentName.c_str() );
				}

				KLTRAP::TransportConnectionLocker trLocker( &trProxy );

				struct soap* pSoap = trLocker.Get();

				struct klft_InitiateServerFileUploadResponse res;

				soap_call_klft_InitiateServerFileUpload( pSoap, NULL, NULL,
					(wchar_t *)fileId.c_str(),
					(pFile->GetFileDescription().m_isDirectory)?1L:0L,
					0L,
					(pFile->GetFileDescription().m_fileSize),
					res );

				trLocker.CheckResult();

				if ( res.error.code )
				{
					KLERR::Error *pError=NULL;
					KLPAR::ExceptionFromSoap( res.error, &pError );
					throw pError;
				}
			}

			std::wstring fileId;			
			bool endFile = false;
			
			
			KLTRAP::TransportProxy trProxy;

			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
				trProxy.Initialize( m_connName.localComponentName.c_str(), m_connName.remoteComponentName.c_str() );
				m_fileStatus.sentPercent = PERCENT_FOR_ARCHIVATE;
			}

			do 
			{
				int bs = c_DefaultChunkSize;
				struct klft_file_chunk_t			chunk;

				KLTRAP::TransportConnectionLocker trLocker( &trProxy );
				struct soap* pSoap = trLocker.Get();

				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

					if ( m_pSentFile==NULL || m_stopSendFlag ) return 0;

					unsigned long neededSize = c_DefaultChunkSize;
					FileDescription fDesc = m_pSentFile->GetFileDescription();

					fileId = m_pSentFile->GetFileId();

					bs = fDesc.m_fileSize - m_fileStatus.sendPos;
					if ( (unsigned)bs>c_DefaultChunkSize ) bs = c_DefaultChunkSize;
									
					chunk.buff.__ptr = ( unsigned char *) soap_malloc( pSoap, bs );				
					chunk.buff.__size = bs;
					
					m_pSentFile->GetChunk( L"", m_fileStatus.sendPos, chunk.buff.__ptr, chunk.buff.__size, endFile );

					KLSTD::CPointer<boost::crc_32_type> crc32;
					crc32=new boost::crc_32_type;
					KLSTD_CHKMEM(crc32);
					crc32->process_bytes( chunk.buff.__ptr, chunk.buff.__size );
					chunk.CRC = crc32->checksum();
				
					chunk.fileId = KLPAR::soap_strdup(pSoap, fileId.c_str());;
					chunk.fileName = NULL;
					chunk.startPosInFile = m_fileStatus.sendPos;
					chunk.fullFileSize = fDesc.m_fileSize;				
				}


				struct klft_PutNextFileChunkResponse	res;

				soap_call_klft_PutNextFileChunk( pSoap, NULL, NULL,
							(wchar_t *)fileId.c_str(),
							chunk,
							res );

				trLocker.CheckResult();

				if ( res.error.code )
				{
					KLERR::Error *pError=NULL;
					KLPAR::ExceptionFromSoap( res.error, &pError );
					throw pError;
				}

				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

					m_fileStatus.sendPos += bs;
					m_fileStatus.sentSize += bs;					
					m_fileStatus.sentPercent = (int)((double)((double)m_fileStatus.sentSize/(double)m_fileStatus.fullFileSize) * (100.0 - (double)PERCENT_FOR_ARCHIVATE));
					m_fileStatus.errorCode = 0;
				}
			} 
			while( !endFile );

			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				if ( m_pWaitSem!=NULL ) m_pWaitSem->Post();
			}
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );

			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			m_fileStatus.errorCode = pError->GetId();
			if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
			{
				m_fileStatus.errorCode = ERR_CONNECTION_BROKEN;
			}
		}
		KLERR_ENDTRY
		return 0;
	}

	

} // end namespace KLFT


/*\brief Создает объект класса FileSender */
KLCSKCA_DECL void KLFT_CreateFileSender( KLFT::FileSender **ppFileSender )
{
	KLSTD_CHKOUTPTR(ppFileSender);
	*ppFileSender = new KLFT::FileSenderImp();
	KLSTD_CHKMEM(*ppFileSender);
}

