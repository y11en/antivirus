/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file FileTransferImp.cpp
 * \author Дамир Шияфетдинов
 * \date 2004 
 * 
 */

#include <std/base/klbase.h>
#include <std/err/error.h>
#include <std/err/klerrors.h>
#include <std/trc/trace.h>
#include <std/par/paramsi.h>
#include <std/par/helpers.h>
#include <std/klcspwd/klcspwdr.h>
#include <std/klcspwd/prtstrg.h>

#include <common/measurer.h>
#include "boost/crc.hpp"

#include <std/tmstg/timeoutstore.h>

#include <transport/ev/eventsource.h>
#include <transport/tr/transport.h>
#include <transport/tr/transportproxy.h>
#include <transport/wat/authserver.h>

#include <kca/prss/settingsstorage.h>
#include <kca/prcp/proxybase.h>

#include <nagent/naginst/nagent_const.h>
#include <nagent/naginst/cert_load.h>

#include <server/srvinst/server_constants.h>
#include <kca/srvhrch/srvhrch_common.h>

#include <transport/avt/acedecl.h>
#include <transport/avt/accesscheck.h>

#include "filetransferimp.h"
#include <kca/ft/errors.h>
#include <kca/ft/foldersync.h>

#include "modulestub/ftsoapstub.h"
#include "fileuploader.h"

#ifdef N_PLAT_NLM
#include <nwconio.h>
#endif

#define KLCS_MODULENAME L"KLFT"

int klft_GetNextFileChunkUAStub(
						struct soap* soap,
						wchar_t*	remoteClientName,
						wchar_t*	fileName,
						wchar_t*	fileId,
						xsd__unsignedLong startFilePos,
						xsd__unsignedLong neededSize,
						xsd__boolean	  useUpdaterFolder,
						xsd__boolean	  forUpdateAgent,
						struct klft_GetNextFileChunkOptions *options,
						struct klft_GetNextFileChunkResponse &r );

const int c_CheckFilesListOnServer			= 1000 * 60 * 60;		// 1 hours
const int c_CheckFilesListOnServerSec		= c_CheckFilesListOnServer / 1000 - 60; // на минуту меньше, что бы попасть в период worker'а

namespace KLFT {

	#define ADD_PARAMS_VALUE(params, name, type, Var) { \
		KLSTD::CAutoPtr<KLPAR::type> pVal; \
		KLPAR::CreateValue(Var, &pVal); \
		(params)->AddValue(name, pVal); }

	#define GET_PARAMS_VALUE(params, name, type, typeType, Var) { \
		KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, true); \
		KLPAR_CHKTYPE(pVal, typeType, name); \
		Var = pVal->GetValue(); }

	#define GET_PARAMS_VALUE_NO_THROW(params, name, type, typeType, Var) { \
		KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, false); \
		if (pVal) { \
			KLPAR_CHKTYPE(pVal, typeType, name); \
			Var = pVal->GetValue(); \
		}}

	#define GET_PARAMS_VALUE_2TYPES_NO_THROW(params, name, type1, typeType1, type2, typeType2, Var){ \
		KLSTD::CAutoPtr<KLPAR::Value> pVal = params->GetValue2(name, false); \
		if (pVal) { \
			if( pVal->GetType()==KLPAR::Value::typeType1 )\
				Var = ((KLPAR::type1 *)(KLPAR::Value *)pVal)->GetValue();\
			if( pVal->GetType()==KLPAR::Value::typeType2 )\
				Var = ((KLPAR::type2 *)(KLPAR::Value *)pVal)->GetValue();\
		}}


	const wchar_t c_UAInfoFileName[]				= L"ua.inf";	

	const wchar_t c_serializeUAHostId[]				= L"UA_HostId";
	const wchar_t c_serializeUAGroupId[]			= L"UA_GroupId";
	const wchar_t c_serializeUAConnName[]			= L"UA_ConnName";
	const wchar_t c_serializeUAAddr[]				= L"UA_Addr";
	const wchar_t c_serializeUANOSSLAddr[]			= L"UA_NoSSLAddr";
	const wchar_t c_serializeUAMulticastIP[]		= L"UA_MulticastIP";
	const wchar_t c_serializeUACert[]				= L"UA_Cert";
	const wchar_t c_serializeUACertPswd[]			= L"UA_CertPswd";

	const wchar_t c_UAInfoLocFileName[]				= L"ualoc.inf";
	
	const wchar_t c_serializeParamsUAConnName[] = L"ftp_UAConnName";
	const wchar_t c_serializeParamsUALocation[] = L"ftp_UALocation";
	const wchar_t c_serializeParamsUASSLLocation[] = L"ftp_UASSLLocation";
	const wchar_t c_serializeParamsUAMulticastIP[] = L"ftp_UAMulticastIP";

	const wchar_t c_UAStatsFileName[]				= L"ua.sts";	

	const wchar_t c_serializeUAST_CREATETIME[]				= L"UA_ST_CreateTime";
	const wchar_t c_serializeUAST_NUMBEROFFILES[]			= L"UA_ST_NumberOfFiles";
	const wchar_t c_serializeUAST_SIZEOFFILES[]				= L"UA_ST_SizeOfFiles";
	const wchar_t c_serializeUAST_SUCCEDEDUPLOADS[]			= L"UA_ST_SuccededUploads";
	const wchar_t c_serializeUAST_SUCCEDEDUPLOADSSIZE[]		= L"UA_ST_SuccededUploadsSize";
	const wchar_t c_serializeUAST_TCPUPLOADEDSIZE[]			= L"UA_ST_TCPUploadedSize";
	const wchar_t c_serializeUAST_MULTICASTUPLOADEDSIZE[]	= L"UA_ST_MulticastUploadedSize";
	const wchar_t c_serializeUAST_MULTICASTSENTSIZE[]		= L"UA_ST_MulticastSentSize";
	const wchar_t c_serializeUAST_FOLDERSYNCLATSSERVERSYNCTIME[]= L"UA_ST_FolderSyncLastServerSyncTime";
	const wchar_t c_serializeUAST_FOLDERSYNCSERVERSYNCCOUNT[]	= L"UA_ST_FolderSyncServerSyncCount";
	const wchar_t c_serializeUAST_FOLDERSYNCMULTICASTSENTCOUNT[]= L"UA_ST_FolderSyncMulticastSentCount";
	const wchar_t c_serializeUAST_FOLDERSYNCMULTICASTSENTSIZE[]	= L"UA_ST_FolderSyncMulticastSentSize";
	const wchar_t c_serializeUAST_FOLDERSYNCTCPUPLOADEDSIZE[]	= L"UA_ST_FolderSyncTCPUploadedSize";
	const wchar_t c_serializeUAST_FOLDERSYNCTCPUPLOADEDCOUNT[]	= L"UA_ST_FolderSyncTCPUploadedCount";	


	void FillSOAPSyncFolderOptions( struct soap* pSoap,
		struct SOAPSyncFolderOptions *syncFolderOpts, 
		const KLFT::FolderSync::SubfoldersList &subFoldersList )
	{
		syncFolderOpts->subfoldersList.__size = subFoldersList.size();
		if ( syncFolderOpts->subfoldersList.__size>0 )
		{
			syncFolderOpts->subfoldersList.__ptr = (xsd__wstring *)soap_malloc( pSoap, syncFolderOpts->subfoldersList.__size * sizeof(xsd__wstring) );

			KLFT::FolderSync::SubfoldersList::const_iterator it = subFoldersList.begin();
			int c = 0;
			for( ; it != subFoldersList.end(); ++it, ++c )
			{				
				syncFolderOpts->subfoldersList.__ptr[c] = KLPAR::soap_strdup( pSoap, it->c_str() );
			}
		}
		else
		{
			syncFolderOpts->subfoldersList.__ptr = NULL;
		}		
	}

	KLSTD::CAutoPtr<FileTransferImp>	g_FileTransfer;
	KLSTD::Semaphore					*g_FileTransferDestroySem = NULL;

	FileTransferImp::FileTransferImp()
	{
		KLSTD_CreateCriticalSection( &m_pCricSec );		

		m_uploadDelay = c_UploadDelay;
		m_nextChunkTimeout = c_NextChunkTimeout;

		m_workerAdded = false;		
		m_checkFilesListWorkerAdded = false;
		m_checkFilesListWorkerFirstStart = true;
		m_lastLoadFilesInfo = 0;	
		m_shutdownFlag = false;
		m_updateAgentInfo.nAgentHostId = (-1);

		m_DisconnectSink.Init( this );
		CONTIE_CreateConnectionTiedObjectsHelper( &m_pContie );		
	}

	FileTransferImp::~FileTransferImp()
	{
		if ( g_FileTransferDestroySem!=NULL ) g_FileTransferDestroySem->Post();
	}

	
	//!\brief Инициализация подсистемы для работы в качестве сервера
	void FileTransferImp::InitServer( const std::wstring &workingFolder, 		
		const std::wstring &serverConnName )
	{
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( workingFolder[workingFolder.length()-1]==L'\\' ||
				workingFolder[workingFolder.length()-1]==L'/' )
				m_workingFolder.assign( workingFolder, 0, m_workingFolder.length()-1 );
			else
				m_workingFolder = workingFolder;
			
			KLPRCI::ComponentId processNameTmp;
			bool convRes = KLTRAP::ConvertTransportNameToComponentId( processNameTmp,
						serverConnName );

			if ( convRes )
			{
				processNameTmp.instanceId = KLSTD_CreateGUIDString();
				processNameTmp.componentName = c_FileTransferComponentName;

				KLTRAP::ConvertComponentIdToTransportName( m_localConnName, processNameTmp );
			}
		}

		LoadInfoFiles();
		
		KLTRACE3( KLCS_MODULENAME, L"FileTransfer::InitServer workingFolder - '%ls' localConnName - '%ls'\n",
			workingFolder.c_str(), m_localConnName.c_str() );
	}

	//!\brief Инициализация подсистемы для работы в качестве клиента
	void FileTransferImp::InitClient( const std::wstring &workingFolder,
			const std::wstring &serverRemoteConnName,
			const std::wstring &localConnName,
			const std::wstring &hostId,			
			KLPAR::BinaryValue *pSer,
			TransferConnectionInvoker *connectionInvoker )
	{
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			m_workingFolder = workingFolder;
			m_strHostId = hostId;
			m_strLocalHostConnName = localConnName;
			
			KLPRCI::ComponentId processNameTmp;
			bool convRes = KLTRAP::ConvertTransportNameToComponentId( processNameTmp,
						localConnName );

			if ( convRes )
			{
				processNameTmp.instanceId = KLSTD_CreateGUIDString();
				processNameTmp.componentName = c_FileTransferComponentName;

				KLTRAP::ConvertComponentIdToTransportName( m_localConnName, processNameTmp );
			}

			m_serverConnName = serverRemoteConnName;

			m_connectionInvoker = connectionInvoker;

			m_pSer = pSer;
		}

		if ( !workingFolder.empty() ) LoadInfoFiles();

		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			KLERR_BEGIN
			{
				ReadUpdateAgentInfo();

				ActivateUpdateAgent();

				ReadUpdateAgentLocsList();

				JoinUpdateAgentMulticastGroup();
			}
			KLERR_ENDT(2);		

			KLTR_GetTransport()->Notifier()->AddNotificationReceiver( this, KLTR::MulticastPacketArrived );

			FileMapType::iterator it = m_filesList.begin();
			for( ; it != m_filesList.end(); ++it ) 
			{
				if ( it->second.m_T->Type()==KLFT::FT_Folder )
				{
					ProcessSyncFolderFile( it->second.m_T->GetFileId() );
				}
			}

			KLTRACE3( KLCS_MODULENAME, L"FileTransfer::InitClient workingFolder - '%ls' localConnName - '%ls' serverRemoteConnName - '%ls' m_pSer - %d\n",
				workingFolder.c_str(), m_localConnName.c_str(), serverRemoteConnName.c_str(), pSer );
		}

		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			m_checkFilesListWorkerFirstStart = true;

			KLTP_GetThreadsPool()->AddWorker( &m_checkFilesListWorkerId,
				L"File transfer check server list worker", this, 
				KLTP::ThreadsPool::RunNormal,
				true, c_CheckFilesListOnServer );

			m_checkFilesListWorkerAdded = true;
		}
	}

	//!\brief Сохраняет клиентский сертификат для последующего использования		 	
	void FileTransferImp::SetClientInfo( KLPAR::BinaryValue *pSer, std::wstring &hostId )
	{
		bool bNeeedReactivateUA = false;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( pSer!=NULL )
			{
				if ( m_pSer==NULL && m_updateAgentInfo.nAgentHostId!=(-1) ) 
				{
					m_saveUpdateAgentInfo = m_updateAgentInfo;					
					bNeeedReactivateUA = true;
				}
				m_pSer = pSer;
			}

			m_strHostId = hostId;

			KLTRACE3( KLCS_MODULENAME, L"FileTransfer::SetClientInfo pSer - %d hostId - '%ls'\n", pSer, m_strHostId.c_str() );
		}

		if ( bNeeedReactivateUA )
		{
			KLERR_BEGIN
			{
				m_saveUpdateAgentInfo = m_updateAgentInfo;
				StopUpdateAgent();
				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
					m_saveUpdateAgentInfo.strAddress.clear();
					m_saveUpdateAgentInfo.strNonSSLAddress.clear();
				}
				ReadUpdateAgentInfo();
				ActivateUpdateAgent();
			}
			KLERR_ENDT(3)
		}
	}

	//!\brief Установка папки для файлов Updater'а
	void FileTransferImp::SetUpdaterFolder( const std::wstring &updaterFolderName,
			int maxNumberUploadingClients )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_updaterFolder = updaterFolderName;
		if( !m_updaterFolder.empty() && m_updaterFolder[m_updaterFolder.length()-1]!=L'/' &&
			m_updaterFolder[m_updaterFolder.length()-1]!=L'\\' )
		{
			m_updaterFolder.append(L"/");
		}
		m_maxNumberUploadingClientsForUpdaterFolder = maxNumberUploadingClients;

		if ( !KLSTD_IfExists( m_updaterFolder.c_str() ) )
		{
			KLSTD_CreatePath( m_updaterFolder.c_str() );
		}
	}

	//!\brief Прекрашает работу подсистемы
	void FileTransferImp::Shutdown()
	{	
		KLERR_BEGIN
		{
			m_saveUpdateAgentInfo = m_updateAgentInfo;			
			StopUpdateAgent();
		}
		KLERR_ENDT(2);		

		if ( m_workerAdded )
		{
			m_shutdownFlag = true;
			KLTP_GetThreadsPool()->DeleteWorker( m_transferWorkerId );		
		}

		if ( m_checkFilesListWorkerAdded )
		{
			m_checkFilesListWorkerAdded = false;
			KLTP_GetThreadsPool()->DeleteWorker( m_checkFilesListWorkerId );		
		}

		if ( m_pContie )
		{
			m_pContie->Close();
			m_pContie = NULL;
		}

		m_connectionInvoker = NULL;

		KLTR_GetTransport()->Notifier()->DeleteNotificationReceiver( this );
	}

	//!\brief Производить рассылку сигнала на клиенты о необходимости закачки указанного файла
	void FileTransferImp::SendFile( const std::wstring &filePath, 
			const KLTRAP::Transport::LocationExList &locsList,
			const std::wstring &fileDescription, std::wstring &fileId,
			int maxNumberUploadingClients )
	{	
		// создаем описатель файла
		KLSTD::CAutoPtr<File> newFile;
		
		newFile.Attach( new File() );
		KLSTD_CHKMEM(newFile);
		
		newFile->CreateServerFile( filePath, fileDescription, 
			m_workingFolder, locsList, c_UploadDelay, fileId,
			maxNumberUploadingClients );
		
		// добавляем файл в список
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
		m_filesList[fileId] = newFile;
		
		AddFileTransferWorker();
	}

	//\brief Установка максимального кол-ва клиентов для закачки для указанного файла
	void FileTransferImp::SetMaxNumberUploadingClients( const std::wstring &fileId, 
		int maxNumberUploadingClients )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->SetMaxNumberUploadingClients( maxNumberUploadingClients );
	}

	//!\brief Добавляет набор указанных клиентов в список рассылки для указанного файла
	void FileTransferImp::AddClientsToFile( const std::wstring &fileId, 
			const KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->AddClientsForSend( locsList );
		
		AddFileTransferWorker();
	}

	//!\brief Удаляет набор указанных клиентов из список рассылки для указанного файла
	void FileTransferImp::DeleteClientsFromFile( const std::wstring &fileId, 
			const std::list<std::wstring> &compsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->DeleteClients( compsList );
	}

	//!\brief Перезаписывает список клиентов для указанного файла
	void FileTransferImp::ReplaceClientsList( const std::wstring &fileId, 
			const KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->ResetClientsList( locsList );

		AddFileTransferWorker();
	}


	//!\brief Обнавляет список клиентов для указанного файла
	void FileTransferImp::UpdateClientsList( const std::wstring &fileId, 
			const KLTRAP::Transport::LocationExList &locsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->UpdateClientsList( locsList );

		AddFileTransferWorker();
	}

	//!\brief Возвращает список идентификаторов рабочих файлов
	void FileTransferImp::GetFilesList( std::vector<std::wstring> &filesIdsList )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		filesIdsList.clear();

		FileMapType::iterator it = m_filesList.begin();
		for( ; it != m_filesList.end(); ++it ) 
			filesIdsList.push_back( it->second.m_T->GetFileId() );
	}

	//!\brief Возвращает полную информацию об файле
	FileDescription FileTransferImp::GetFileInfo( const std::wstring &fileId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		FileDescription fDesc = fit->second.m_T->GetFileDescription();

		if ( fit->second.m_T->GetUpdateAgentFileStatus() & File::UAFS_Downloaded )
		{
			// для локальных клиентов возвращаем статус уже закачен
			fDesc.m_status = KLFT::FileDescription::Uploaded;
		}

		return fDesc;
	}

	//!\brief Удаляет описание данного файла и прекращает все незаконченные операции
	void FileTransferImp::DeleteFile( const std::wstring &fileId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->DeleteWorkingFiles();

		m_filesList.erase( fit );

		KLERR_IGNORE(m_pContie->RemoveObject( fileId ));
	}

	void FileTransferImp::DeleteUpdaterFile( const std::wstring &fileId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		std::wstring filePath =  m_updaterFolder + fileId;

		// ищем описатель данного файла в списке
		FileMapType::iterator it = m_filesList.begin();
		for( ; it!=m_filesList.end(); ++it )
		{
			if ( it->second.m_T->IsCustomModeFile() && it->second.m_T->GetFileId()==filePath )
			{
				m_filesList.erase( it );
				break;
			}
		}
	}

	void FileTransferImp::MulticastPacketArrived( const std::wstring &mcGroupName,
			const KLPAR::BinaryValue *pTransportPacket )
	{
		std::wstring fileId;
		std::wstring folderName, fileName;
		KLSTD::CAutoPtr<KLPAR::BinaryValue> pBinData;
		long chunkCRC, fullFileSize, posInFile;

		KLERR_TRY
		{
			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				if ( m_updateAgentInfo.nAgentHostId!=(-1) ) return; // UA не получает пакеты по multicast
			}

			KLSTD::CAutoPtr<KLPAR::Params> pChunkParams;

			KLPAR_DeserializeFromMemory( pTransportPacket->GetValue(), 
				pTransportPacket->GetSize(), &pChunkParams );
			
			GET_PARAMS_VALUE( pChunkParams, KLFT::SER_MC_FILE_ID, StringValue, STRING_T, fileId );

			GET_PARAMS_VALUE_NO_THROW( pChunkParams, KLFT::SER_MC_FILE_NAME, StringValue, STRING_T, fileName );
			GET_PARAMS_VALUE_NO_THROW( pChunkParams, KLFT::SER_MC_FOLDER_NAME, StringValue, STRING_T, folderName );

			long chunkSize;
			GET_PARAMS_VALUE( pChunkParams, KLFT::SER_MC_CHUNK_SIZE, IntValue, INT_T, chunkSize );			
			GET_PARAMS_VALUE( pChunkParams, KLFT::SER_MC_CHUNK_CRC, IntValue, INT_T, chunkCRC );

			GET_PARAMS_VALUE( pChunkParams, KLFT::SER_MC_FULL_SIZE, IntValue, INT_T, fullFileSize );
			GET_PARAMS_VALUE( pChunkParams, KLFT::SER_MC_CHUNK_POS, IntValue, INT_T, posInFile );
			
			pBinData = (KLPAR::BinaryValue*)pChunkParams->GetValue2( SER_MC_CHUNK_DATA, true);
			KLPAR_CHKTYPE(pBinData, BINARY_T, SER_MC_CHUNK_DATA);

			KLSTD::CPointer<boost::crc_32_type> crc32;
			crc32=new boost::crc_32_type;
			KLSTD_CHKMEM(crc32);
			crc32->process_bytes( pBinData->GetValue(), pBinData->GetSize() );
			unsigned long CRC = crc32->checksum();

			if ( CRC!=(unsigned)chunkCRC)
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, fileId.c_str() );

			if ( (unsigned)chunkSize!=pBinData->GetSize() )
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, fileId.c_str() );

			FileAutoUnlock  fileUnlocker;
			{		
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
				
				KLSTD::CAutoPtr<File> foundFile;
				
				FileMapType::iterator fit = m_filesList.find( fileId );
				if ( fit==m_filesList.end() )
				{
					if ( !folderName.empty() && !fileName.empty() )
					{
						KLSTD::CAutoPtr<File> newFile;

						newFile.Attach( new File() );
						KLSTD_CHKMEM(newFile);

						newFile->CreateEndPointFile( fileId, 
							folderName, m_workingFolder );

						m_filesList[fileId] = newFile;

						fileUnlocker.Init( newFile );
					}
					else 
					{
						KLSTD::CAutoPtr<File> newFile;

						newFile.Attach( new File() );
						KLSTD_CHKMEM(newFile);

						newFile->CreateClientFile( fileName, fileId, 
							L"", false, fullFileSize, FT_None,
							false, KLTRAP::Transport::ConnectionFullName(),
							c_UploadDelay, c_NextChunkTimeout, false, UpdateAgentLocationList(),
							m_workingFolder );

						m_filesList[fileId] = newFile;

						fileUnlocker.Init( newFile );

						// KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );
					}
				}
				else
				{
					foundFile = fit->second.m_T;

					fileUnlocker.Init( foundFile );
				}
			}

			bool endFlag = false;
			
			if ( !folderName.empty() && !fileName.empty() )
			{	
				KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
				fileUnlocker.Get()->GetFolderSync( pFolderSync );

				bool endOfFile = false;
				int wrote = pFolderSync->WriteFolderFileChunk( fileName, posInFile,
					fullFileSize, pBinData->GetValue(),  
					pBinData->GetSize(), endOfFile );

				if ( wrote<=0 )
				{
					KLTRACE3( KLCS_MODULENAME, L"FileTransfer::MulticastPacketArrived. Can't save multicast packet for folder sync file. Id - '%ls' Name - '%ls'.\n",
						fileId.c_str(), fileName.c_str() );
				}
			}
			else
			{
				fileUnlocker.Get()->AddChunk( pBinData->GetValue(),  
					pBinData->GetSize(), chunkCRC, posInFile, fullFileSize, true );
			}
								
			if ( endFlag )
			{
				// публикуем событие о закачки нового файла
				PublishEvent( EVENT_TYPE_UPLOADED, fileId );
			}

		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			return;
		}
		KLERR_ENDTRY
	}

	// KLFT::MulticastSender::FileProvider

	int FileTransferImp::GetFileInfo( const std::wstring &fileId, 
				const std::wstring &fileName,
				unsigned long &fullSize,
				std::wstring &folderName )
	{
		fullSize = 0;
		if ( fileName.empty() ) return 0;
		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;	

		FileAutoUnlock  fileUnlocker;
		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			KLSTD::CAutoPtr<File> foundFile;
			
			FileMapType::iterator fit = m_filesList.find( fileId );
			if ( fit==m_filesList.end() ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

			foundFile = fit->second.m_T;
			if ( foundFile->Type()!=KLFT::FT_Folder ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

			fileUnlocker.Init( foundFile );			
			fileUnlocker.Get()->GetFolderSync( pFolderSync );

			folderName = foundFile->GetFileDescription().m_description;
		}

		if ( pFolderSync==NULL )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		KLFT::FileInfo fi;
		pFolderSync->GetFolderFileInfo( fileName, fi );

		fullSize = fi.m_fileSize;		
		folderName = fileId;
		
		return fi.m_fileSize;
	}

	int FileTransferImp::GetFileChunk( const std::wstring &fileId, 
				const std::wstring &fileName,
				unsigned long pos,
				void *buff, unsigned long buffSize, 
				bool &endOfFile )
	{
		FileAutoUnlock  fileUnlocker;
		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;	
		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			KLSTD::CAutoPtr<File> foundFile;
			
			FileMapType::iterator fit = m_filesList.find( fileId );
			if ( fit==m_filesList.end() ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

			foundFile = fit->second.m_T;

			fileUnlocker.Init( foundFile );

			if ( foundFile->Type()==KLFT::FT_Folder ) 
			{
				fileUnlocker.Get()->GetFolderSync( pFolderSync );
			}				
		}

		if ( pFolderSync==NULL )
		{
			return fileUnlocker.Get()->GetChunk( L"", pos, buff, buffSize, endOfFile );
		}
		else
		{
			unsigned long fileFullSize = 0;
			return pFolderSync->GetFolderFileChunk( fileName, pos, buffSize, fileFullSize, &buff, buffSize, endOfFile );
		}
	}

	int FileTransferImp::OnFileSent( const std::wstring &fileId,
				const std::wstring &fileName, 
				const unsigned long sentSize )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
		KLSTD::CAutoPtr<File> foundFile;
		
		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		foundFile = fit->second.m_T;
		if ( foundFile->Type()==KLFT::FT_Folder ) 
		{
			foundFile->ProcessedLastSyncChangedFiles( fileName );

			if ( m_updateAgentInfo.nAgentHostId>=0 )
			{
				// update statistics			
				m_updateAgentStat.nFolderSyncMulticastSentSize += sentSize;
				SaveUpdateAgentStatistics();
			}
		}
		else
		{
			if ( m_updateAgentInfo.nAgentHostId>=0 )
			{
				// update statistics			
				m_updateAgentStat.nMulticastSentSize += foundFile->GetFileDescription().m_fileSize;
				SaveUpdateAgentStatistics();
			}
		}
		
		return 0;
	}

	bool FileTransferImp::GetSyncFolderDiff( const std::wstring &fileDescription, 
				const KLFT::FolderSync::FileInfoMap &clientFolderInfo,
				std::wstring &syncFolderFileId,
				KLFT::FolderSync::FolderFilesInfo &diffFolderFilesInfo,
				const FolderSync::SubfoldersList &subfoldersList,
				KLFT::FolderSync::SubfoldersHashList &subfoldersHashList )
	{
		FileAutoUnlock  fileUnlocker;
		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			KLSTD::CAutoPtr<File> foundFile;

			FileMapType::iterator fit = m_filesList.begin();
			for( ; fit != m_filesList.end(); ++fit )
			{
				if ( fit->second.m_T->GetFileDescription().m_description==fileDescription )				
					break;
			}
						
			if ( fit==m_filesList.end() ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileDescription.c_str() );

			foundFile = fit->second.m_T;
			syncFolderFileId = foundFile->GetFileId();

			if ( foundFile->Type()!=KLFT::FT_Folder )
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, (fileDescription + L":" + syncFolderFileId).c_str() );

			fileUnlocker.Init( foundFile );

			if ( m_updateAgentInfo.nAgentHostId>=0 )
			{
				// update statistics			
				m_updateAgentStat.nFolderSyncTCPUploadedCount++;
				SaveUpdateAgentStatistics();
			}
		}

		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
		fileUnlocker.Get()->GetFolderSync( pFolderSync );

		if ( pFolderSync==NULL )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, (fileDescription + L":" + syncFolderFileId).c_str() );

		return pFolderSync->GetFullSyncInfo( clientFolderInfo, diffFolderFilesInfo, 
			subfoldersList, subfoldersHashList );
	}

	void FileTransferImp::GetSyncFolderFileInfo( 
				const std::wstring &remoteClientName,
				const std::wstring &syncFolderFileId, 
				const std::wstring &folderFileName,
				KLFT::FileInfo &fi )
	{
		FileAutoUnlock  fileUnlocker;
		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;

		GetFileFolderSync( remoteClientName, syncFolderFileId, 
			fileUnlocker, pFolderSync );

		pFolderSync->GetFolderFileInfo( folderFileName, fi );
	}


	int FileTransferImp::GetSyncFolderFileChunk( 
				const std::wstring &remoteClientName,
				const std::wstring &syncFolderFileId, 
				const std::wstring &folderFileName,
				unsigned long pos, unsigned long neededSize,
				unsigned long &fullFileSize,
				void **buff, unsigned long &buffSize,
				bool &endOfFile )
	{
		FileAutoUnlock  fileUnlocker;
		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;

		GetFileFolderSync( remoteClientName, syncFolderFileId, 
			fileUnlocker, pFolderSync );
		
		unsigned long read = 0;
		bool bExists = false, bCanBeUploaded = false;
		
		bExists = pFolderSync->IsFileExists( folderFileName, bCanBeUploaded );

		if ( !bExists && bCanBeUploaded )
		{
			// закачиваем файл с сервера
			FileUploader fileUploader;

			KLTRAP::Transport::ConnectionFullName remoteConnection;
			remoteConnection.localComponentName = m_localConnName;
			remoteConnection.remoteComponentName = m_serverConnName;

			bool bActiveFlag = KLTR_GetTransport()->IsConnectionActive( remoteConnection.localComponentName.c_str(), 
									remoteConnection.remoteComponentName.c_str() );

			if ( !bActiveFlag )
			{
				// попробуем вызвать invoker для создания соединения
				if ( m_connectionInvoker!=NULL )
				{
					m_connectionInvoker->CreateConnection(
						remoteConnection.localComponentName.c_str(),
						remoteConnection.remoteComponentName.c_str() );
				}
			}

			KLPRCI::ComponentId remoteCompId;
			KLTRAP::ConvertTransportNameToComponentId( remoteCompId, remoteClientName );

			KLERR_TRY
			{
				fileUploader.GetSyncFolderFile( remoteConnection, 
					KLTRAP::Transport::ConnectionFullName(), pFolderSync, 
					folderFileName, remoteCompId.instanceId.empty()?remoteClientName:remoteCompId.instanceId );
			}
			KLERR_CATCH(pError)
			{
				KLERR_SAY_FAILURE( 2, pError );
				if ( !bActiveFlag )
				{
					KLTR_GetTransport()->CloseClientConnection( remoteConnection.localComponentName.c_str(),
						remoteConnection.remoteComponentName.c_str() );
				}
				KLERR_RETHROW();
			}
			KLERR_ENDTRY;

			fileUnlocker.Get()->AddSubfoldersForFolderSync( folderFileName );

			if ( !bActiveFlag )
			{
				KLTR_GetTransport()->CloseClientConnection( remoteConnection.localComponentName.c_str(),
					remoteConnection.remoteComponentName.c_str() );
			}
		}

		read = pFolderSync->GetFolderFileChunk( folderFileName,
			pos, neededSize, fullFileSize,
			buff, buffSize, endOfFile );
		
		fileUnlocker.Free();

		if ( endOfFile )
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_updateAgentInfo.nAgentHostId>=0 )
			{
				// update statistics			
				m_updateAgentStat.nFolderSyncTCPUploadedSize += fullFileSize;
					
				SaveUpdateAgentStatistics();
			}
		}

		buffSize = read;

		return read;
	}

	bool FileTransferImp::TryToGetUpdateAgentInfoFromServer(
				const std::wstring &localName, std::wstring &serverRemoteName,
				bool &useUpdateAgent )
	{
		bool bListChanged = false;

		KLERR_BEGIN
		{
			KLTRACE3( KLCS_MODULENAME, L"FileTransfer::TryToGetUpdateAgentInfoFromServer serverRemoteName - '%ls' m_strHostId - '%ls'....\n",
				serverRemoteName.c_str(), m_strHostId.c_str() );

			useUpdateAgent = false;

			if ( m_strHostId.empty() ) return bListChanged;

			KLTRAP::TransportProxy trProxy;		
			trProxy.Initialize( localName.c_str(), serverRemoteName.c_str() );
					
			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );

			struct soap* pSoap = proxyLocker.Get();
					
			struct klft_GetHostUpdateAgentInfoResponse res;

			soap_call_klft_GetHostUpdateAgentInfo( pSoap, NULL, NULL,
						(wchar_t*)m_strHostId.c_str(),						
						res );

			proxyLocker.CheckResult();
			
			if ( res.error.code )
			{	
				KLTRACE3( KLCS_MODULENAME, L"FileTransfer::TryToGetUpdateAgentInfoFromServer serverRemoteName - '%ls' error - %d\n",
					serverRemoteName.c_str(), res.error.code );

				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );				
				throw pError;
			}

			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				UpdateAgentLocationList	newUpdateAgentList;

				if ( res.updatesAgents.__size>0 ) 
				{
					useUpdateAgent = true;

					for( int i = 0; i < res.updatesAgents.__size; ++i )
					{
						KLFT::UpdateAgentLocation uaLoc;
						uaLoc.strConnectionName = res.updatesAgents.__ptr[i].connectionName;
						uaLoc.strLocation = res.updatesAgents.__ptr[i].location;
						uaLoc.strSSLLocation = res.updatesAgents.__ptr[i].sslLocation;
						uaLoc.strMulticastAddr = res.updatesAgents.__ptr[i].multicastAddr;

						newUpdateAgentList.push_back( uaLoc );
						
						bool bFoundLoc = false;
						UpdateAgentLocationList::iterator it = m_updateAgentLocsList.begin();
						for( ; it != m_updateAgentLocsList.end() && !bFoundLoc; ++it )
						{
							if ( it->strLocation==uaLoc.strLocation && 
								it->strConnectionName==uaLoc.strConnectionName )
							{
								bFoundLoc = true;
							}
						}

						if ( !bFoundLoc ) bListChanged = true;
					}
				}

				m_updateAgentLocsList.clear();
				m_updateAgentLocsList = newUpdateAgentList;

				KLTRACE3( KLCS_MODULENAME, L"FileTransfer::TryToGetUpdateAgentInfoFromServer serverRemoteName - '%ls' useUpdateAgent - %d...OK\n",
					serverRemoteName.c_str(), useUpdateAgent );

				SaveUpdateAgentLocsList();
				JoinUpdateAgentMulticastGroup();
			}
		}
		KLERR_ENDT(3);

		return bListChanged;
	}

	bool FileTransferImp::CanUseUpdateAgent()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		return !(m_updateAgentLocsList.empty());
	}

	bool FileTransferImp::ConnectToUpdateAgent( std::wstring &localName, 
		std::wstring &remoteName, UpdateAgentLocationList *locsList )
	{
		KLERR_BEGIN
		{
			unsigned int count = locsList==NULL?m_updateAgentLocsList.size():locsList->size();
			
			for( unsigned int i = 0; i < count; i++ )
			{
				UpdateAgentLocation uaLoc;
				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

					if ( locsList==NULL && !CanUseUpdateAgent() ) return false;

					if ( locsList!=NULL )
					{
						if ( locsList->empty() ) return false;
						if ( i>=locsList->size() ) return false;

						uaLoc = (*locsList)[i];					
					}
					else
					{
						if ( i>=m_updateAgentLocsList.size() ) return false;
						uaLoc = m_updateAgentLocsList[i];
					}
				}

				remoteName = uaLoc.strConnectionName;

				KLPRCI::ComponentId processNameTmp;
				bool convRes = KLTRAP::ConvertTransportNameToComponentId( processNameTmp,
					m_localConnName );
				if ( convRes )
				{
					processNameTmp.instanceId = uaLoc.strConnectionName;
					processNameTmp.componentName = c_FileTransferComponentName;
					KLTRAP::ConvertComponentIdToTransportName( remoteName, processNameTmp );
				}

				KLSTD::CAutoPtr<KLPAR::Params> pCustCred;
				KLPAR::param_entry_t pars[]=
				{					
					KLPAR::param_entry_t(KLNAG::c_spAuthNagentId, uaLoc.strConnectionName.c_str())
				};
				KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pCustCred);

				KLTRAP::Transport::ConnectionInfo connInfo;

				connInfo.localName = localName;
				connInfo.remoteName = remoteName;
				connInfo.remoteLocation = uaLoc.strSSLLocation;
				connInfo.bCreateSSLConnection = true;		
				connInfo.pRemotePublicSSLKey = m_pSer;
				connInfo.bCompressTraffic = true; // bCompressTraffic
				connInfo.pCustomCredentials = pCustCred;
				
				bool cRes = KLTR_GetTransport()->AddClientConnection2( connInfo );
				if ( !cRes ) 
				{
					KLTRACE3( KLCS_MODULENAME, L"FileTransfer::ConnectToUpdateAgent Can't connect to UpdateAgent. localName - '%ls' remoteName - '%ls' location - '%ls'.\n",
						localName.c_str(), remoteName.c_str(), uaLoc.strSSLLocation.c_str() );
					continue;
				}

				KLTRACE3( KLCS_MODULENAME, L"FileTransfer::ConnectToUpdateAgent. Successful in connect to UpdateAgent. localName - '%ls' remoteName - '%ls' location - '%ls'.\n",
						localName.c_str(), remoteName.c_str(), uaLoc.strSSLLocation.c_str() );

				return true;
			}
		}
		KLERR_ENDT(3)

		return false;		
	}

	bool FileTransferImp::ConnectToFolderSyncUpdateAgent(
				const std::wstring &folderSyncName,
				KLSTD::CAutoPtr<KLFT::FolderSync> &pOutFolderSync,
				std::wstring &localName, std::wstring &remoteName )
	{
		KLTRACE3( KLCS_MODULENAME, L"FileTransfer::ConnectToFolderSyncUpdateAgent... folderSyncName - '%ls'\n",
					folderSyncName.c_str() );

		KLERR_TRY
		{
			bool connRes = ConnectToUpdateAgent( localName, remoteName );
			if ( !connRes ) return false;

			std::wstring fileId;
			KLSTD::CAutoPtr<KLFT::FolderSync>	pFolderSync;
			KLFT::FolderSync::SubfoldersList	subFoldersList;

			{			
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				FileMapType::iterator fit = m_filesList.begin();
				for( ; fit != m_filesList.end(); ++fit )
				{
					if ( fit->second.m_T->GetFileDescription().m_description==folderSyncName )
					{
						fileId = fit->first;
						break;
					}
				}
				
				if ( !fileId.empty() && fit!=m_filesList.end() )
				{
					subFoldersList = fit->second.m_T->GetSubfoldersListForFolderSync();
					fit->second.m_T->GetFolderSync( pFolderSync );
				}

			}

			KLFT::FolderSync::FolderFilesInfo folderFilesInfo;

			if ( pFolderSync!=NULL )
				pFolderSync->GetFullSyncInfo( folderFilesInfo );

			KLTRAP::TransportProxy trProxy;		
			trProxy.Initialize( localName.c_str(), remoteName.c_str() );
			
			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
			
			struct soap* pSoap = proxyLocker.Get();

			struct SOAPFilesInfoList filesList;
			
			filesList.__size = folderFilesInfo.FilesList.size();
			if ( filesList.__size>0 )
			{
				filesList.__ptr = soap_new_SOAPFolderFileInfo( pSoap, filesList.__size );								
				KLFT::FolderSync::FileInfoList::iterator it = folderFilesInfo.FilesList.begin();
				int c = 0;
				for( ; it != folderFilesInfo.FilesList.end(); ++it, ++c )
				{
					filesList.__ptr[c].CRC = it->CRC;
					filesList.__ptr[c].FileNameHash = KLPAR::soap_strdup( pSoap, it->FileNameHash.c_str() );
				}
			}
			else
			{
				filesList.__ptr = NULL;
			}

			struct klft_GetSyncFolderDiffResponse	res;
			struct SOAPSyncFolderOptions syncFolderOpts;
			FillSOAPSyncFolderOptions( pSoap, &syncFolderOpts, subFoldersList );

			soap_call_klft_GetSyncFolderDiff( pSoap, NULL, NULL,
						(wchar_t*)folderSyncName.c_str(),
						&filesList, &syncFolderOpts,
						res );

			proxyLocker.CheckResult();
			
			if ( res.error.code )
			{	
				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );				
				throw pError;
			}

			KLFT::FolderSync::FolderFilesInfo badFolderFilesInfo;
			if ( res.diffFilesList!=NULL && res.diffFilesList->__size>0 )
			{
				for( int c = 0; c < res.diffFilesList->__size; c++ )
				{
					KLFT::FolderSync::FolderFileInfo fi( res.diffFilesList->__ptr[c].CRC, 
						res.diffFilesList->__ptr[c].FileNameHash );
					badFolderFilesInfo.FilesList.push_back( fi );
				}
			}
			
			KLFT::FolderSync::SubfoldersHashList serverSubfoldersHashList;
			if ( res.folderHashList!=NULL && res.folderHashList->__size>0 )
			{
				for( int c = 0; c < res.folderHashList->__size; c++ )
				{
					serverSubfoldersHashList.push_back( res.folderHashList->__ptr[c] );
				}
			}

			if ( !res.folderIsEqual )
			{
				if ( fileId.empty() ) fileId = res.syncFolderFileId;
				else 
				{
					if ( fileId!=res.syncFolderFileId ) KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, res.syncFolderFileId );
				}

				if ( pFolderSync!=NULL )
				{	
					pFolderSync->SetNeedToUpdateFilesInfo( badFolderFilesInfo, serverSubfoldersHashList );
				}
			}
			else
			{
				KLTRACE3( KLCS_MODULENAME, L"FileTransfer::ConnectToFolderSyncUpdateAgent folders are equal.\n" );
			}

			if ( pFolderSync==NULL )
			{					
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
				KLSTD::CAutoPtr<File> newFile;

				newFile.Attach( new File() );
				KLSTD_CHKMEM(newFile);
				
				newFile->CreatePackageFile( fileId, fileId, 
					folderSyncName, KLFT::FT_Folder, 
					false, false, KLTRAP::Transport::ConnectionFullName(),
					0, 0, m_workingFolder );

				m_filesList[fileId] = newFile;

				newFile->GetFolderSync( pOutFolderSync );

				pOutFolderSync->SetNeedToUpdateFilesInfo( badFolderFilesInfo, serverSubfoldersHashList );
			}
			else pOutFolderSync = pFolderSync;
		}
		KLERR_CATCH(pError)
		{
			KLERR_SAY_FAILURE( 2, pError );
			return false;
		}
		KLERR_ENDTRY;

		KLTRACE3( KLCS_MODULENAME, L"FileTransfer::ConnectToFolderSyncUpdateAgent...END folderSyncName - '%ls'\n",
					folderSyncName.c_str() );

		return true;
	}

	bool FileTransferImp::AddSubfoldersForFolderSync( 
				const std::wstring &folderSyncName,
				const std::wstring &requestedFilePath )	
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.begin();
		for( ; fit != m_filesList.end(); ++fit )
		{
			if ( fit->second.m_T->GetFileDescription().m_description==folderSyncName )
			{
				break;
			}
		}
		
		if ( fit!=m_filesList.end() )
		{
			return fit->second.m_T->AddSubfoldersForFolderSync( requestedFilePath );
		}
		else return false;
	}


	//!\brief Копирует закачанный файл по указанному пути			
	void FileTransferImp::SaveFileAs( const std::wstring &fileId,
			const std::wstring &copyFolderPath,
			bool overwrite )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		fit->second.m_T->SaveFileAs( copyFolderPath, overwrite );

		if ( fit->second.m_T->Type()==KLFT::FT_CustomServerUploaded )
		{
			fit->second.m_T->DeleteWorkingFiles();
			m_filesList.erase( fit );
		}
	}

	//!\brief Возвращает локальное транспортное имя FileTransfer'а
	void FileTransferImp::GetLocalConnectionName( std::wstring &localName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		localName = m_localConnName;
	}

	//!\brief Возвращает рабочую директорию FileTransfer'а
	void FileTransferImp::GetWorkingFolder( std::wstring &workingFolder )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		workingFolder = m_workingFolder;
	}

	//!\brief Определяет есть ли для данного соединения файлы для отправки
	bool FileTransferImp::HasNotSentFiles( const std::wstring &remoteName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
				
		FileMapType::iterator it = m_filesList.begin();
		for( ; it != m_filesList.end(); ++it )
		{		
			if ( it->second.m_T->IsNeedToBroadcast( false ) )
			{
				if ( it->second.m_T->GetBroadcastCount() >= 2 )
				{
					FileDescription fDesc = it->second.m_T->GetFileDescription();					
			
					KLTRAP::Transport::LocationExList::iterator it = fDesc.m_notSendLocsList.begin();
					for( ; it!=fDesc.m_notSendLocsList.end(); ++it )
					{
						KLPRCI::ComponentId remoteCompId;
						KLERR_IGNORE(remoteCompId.GetFromString( it->remoteComponentName ));
						if ( remoteCompId.instanceId==remoteName )
						{
							return true;
						}
					}
				}				
			}
		}

		return false;
	}

	/*!\brief Функция создает соединение для отсылки удаленному хосту информацию
		о необходиомости закачки файла
	*/
	void FileTransferImp::InitiateRemoteConnectionAndSendSignal( const std::wstring &localConnName, 
				const std::wstring &remoteConnName, 
				const std::wstring &remoteName )
	{
		KLTRAP::Transport::LocationEx sendLoc;
		std::wstring fileId;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
					
			FileMapType::iterator it = m_filesList.begin();
			for( ; it != m_filesList.end(); ++it )
			{		
				if ( it->second.m_T->IsNeedToBroadcast( false ) )
				{
					if ( it->second.m_T->GetBroadcastCount() >= 2 )
					{
						FileDescription fDesc = it->second.m_T->GetFileDescription();
				
						KLTRAP::Transport::LocationExList::iterator lit = fDesc.m_notSendLocsList.begin();
						for( ; lit!=fDesc.m_notSendLocsList.end(); ++lit )
						{
							if ( lit->remoteComponentName==remoteName )
							{
								sendLoc = *lit;
								fileId = it->second.m_T->GetFileId();
								it->second.m_T->ResetBroadcastCount();
							}
						}
					}				
				}
			}
		}

		if ( sendLoc.remoteComponentName==remoteName && !fileId.empty() )
		{
			KLERR_BEGIN

				SendUploadSignal( fileId, localConnName, remoteConnName );

			KLERR_ENDT(2);
		}
	}

	//!\brief Определяет начали ли указанных хост процесс загрузки файла
	bool FileTransferImp::IsFileUploadingByHost( const std::wstring &fileId, 
		const std::wstring &remoteName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit==m_filesList.end() ) 
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		FileDescription fDesc = fit->second.m_T->GetFileDescription();
		KLTRAP::Transport::LocationExList::iterator it = fDesc.m_sentLocsList.begin();
		for( ; it != fDesc.m_sentLocsList.end(); ++it )
		{	
			if ( it->remoteComponentName==remoteName )
			{
				return true;
			}
		}
		
		return false;
	}
	
	//!\brief Возвращает статтистику работы агента обновлений
	bool FileTransferImp::GetUpdateAgentStatistics( UpdateAgentStatistics &uaStat )
	{	
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_updateAgentInfo.nAgentHostId==(-1) ) return false;


		uaStat = m_updateAgentStat;

		if ( uaStat.nSuccededUploadsSize>0 )
			uaStat.nPercentUsageMulticast = (long)((double)(uaStat.nMulticastUploadedSize * 100) / 
				(double)uaStat.nSuccededUploadsSize);
		else 
			uaStat.nPercentUsageMulticast = 0;


		if ( uaStat.nFolderSyncMulticastSentSize>0 )
			uaStat.nFolderSyncPercentUsageMulticast = (long)(100.0 - 
				(double)((double)((uaStat.nFolderSyncTCPUploadedSize + uaStat.nFolderSyncMulticastSentSize) * 100) /
				(double)(uaStat.nFolderSyncMulticastSentSize * uaStat.nFolderSyncTCPUploadedCount)));
		else
			uaStat.nFolderSyncPercentUsageMulticast = 0;

		return true;
	}

	//\brief Возвращает список listener портов используемых UpdateAgent'ом
	void FileTransferImp::GetUsingListenersPostsList( std::vector<int> &vecTCPPortsList,
			std::vector<int> &vecUDPPortsList )
	{
		vecTCPPortsList.clear();
		vecUDPPortsList.clear();
		
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( m_updateAgentInfo.nAgentHostId==(-1) ) return;

		int port = (-1);
		std::wstring protocol, hostname;	

		port = (-1);
		KLERR_IGNORE(KLPRCP_SplitAddress( m_updateAgentInfo.strAddress.c_str(), &protocol, &port, &hostname));
		if ( port>0 ) vecTCPPortsList.push_back( port );

		port = (-1);
		KLERR_IGNORE(KLPRCP_SplitAddress( m_updateAgentInfo.strNonSSLAddress.c_str(), &protocol, &port, &hostname));
		if ( port>0 ) vecTCPPortsList.push_back( port );

		port = (-1);
		KLERR_IGNORE(KLPRCP_SplitAddress( m_updateAgentInfo.strMulticastAddr.c_str(), &protocol, &port, &hostname));
		if ( port>0 ) vecUDPPortsList.push_back( port );
	}

	//!\brief Посылает сигнал клиенту
	void FileTransferImp::SendUploadSignal( const std::wstring &fileId, 
		const std::wstring &localName, const std::wstring &remoteName )
	{
		FileDescription fDesc;
		int calculateUploadClientTimeout = 0;

		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			FileMapType::iterator fit = m_filesList.find( fileId );
			if ( fit==m_filesList.end() ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

			fDesc = fit->second.m_T->GetFileDescription();
			calculateUploadClientTimeout = fit->second.m_T->GetCalculateDelayTimeout();
		}

		{

			KLTRAP::TransportProxy trProxy;		
			trProxy.Initialize( localName.c_str(), remoteName.c_str() );

			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );

			struct soap* pSoap = proxyLocker.Get();

			struct klft_InitiateFileUploadResponse res;

			soap_call_klft_InitiateFileUpload( pSoap, NULL, NULL,
				(wchar_t *)fDesc.m_fileName.c_str(),
				(wchar_t *)fDesc.m_fileId.c_str(), (wchar_t *)fDesc.m_description.c_str(), 
				fDesc.m_isDirectory,
				fDesc.m_fileSize, L"", (wchar_t *)remoteName.c_str(),  
				calculateUploadClientTimeout, m_nextChunkTimeout, 
				(int)KLFT::FT_None, 0, NULL, res );

			proxyLocker.CheckResult();

			if ( res.error.code )
			{
				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );
				throw pError;
			}
		}
	}

	//!\brief Возвращает очередную порцию файла
	int FileTransferImp::GetNextFileChunk( 
			const std::wstring &remoteClientName,
			const std::wstring &fileName, const std::wstring &fileId,
			unsigned long pos, unsigned long neededSize,
			unsigned long &fullFileSize,
			bool useUpdaterFolder, unsigned long multicastRecived, 
			char **ppBuff, int &buffSize )
	{
		FileDescription fDesc;
		FileAutoUnlock  fileUnlocker;
		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			bool uploadingClientsExceeded = false;
			KLSTD::CAutoPtr<File> foundFile;
			
			if ( useUpdaterFolder )
			{
				foundFile = CreateUpdaterFolderFileDescription( fileName );
				
				if ( !foundFile->CheckUploadingClient( remoteClientName,
					uploadingClientsExceeded ) )
				{
					if ( uploadingClientsExceeded )
						KLFT_THROW_ERROR2( ERR_MAX_UPLOADING_CLIENTS_NUMBER_EXCEEDED,
							fileId.c_str(), remoteClientName.c_str() );
					else
						KLFT_THROW_ERROR2( ERR_FILE_UPLOADING_CANCELED, 
							fileId.c_str(), remoteClientName.c_str() );
				}
				
				foundFile->ReadFileSize();
			}
			else
			{			
				FileMapType::iterator fit = m_filesList.find( fileId );
				if ( fit==m_filesList.end() ) 
					KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

				// переносим данного клиента в список клиентов, которым уже отослан сигнал на выгрузку
				if ( fit->second.m_T->AddSentLocation( remoteClientName ) )
				{
					KLPAR::param_entry_t bodypars[]=
					{
						KLPAR::param_entry_t( EVENT_BODY_REMOTE_HOST_NAME, remoteClientName.c_str() )
					};
					KLSTD::CAutoPtr<KLPAR::Params> pEventBody;
					KLPAR::CreateParamsBody(bodypars, KLSTD_COUNTOF(bodypars), 
						&pEventBody );
					
					PublishEvent( EVENT_TYPE_START_HOST_FILE_UPLOAD, fileId, pEventBody );
				}

				if ( !(fit->second.m_T->GetUpdateAgentFileStatus() & KLFT::File::UAFS_Downloaded) )
				{
					KLFT_THROW_ERROR2( ERR_UA_UPLOADING_FILE_FROM_SERVER, 
							fileId.c_str(), remoteClientName.c_str() );
				}
				
				if ( !fit->second.m_T->CheckUploadingClient( remoteClientName,
					uploadingClientsExceeded ) )
				{
					if ( uploadingClientsExceeded )
						KLFT_THROW_ERROR2( ERR_MAX_UPLOADING_CLIENTS_NUMBER_EXCEEDED,
							fileId.c_str(), remoteClientName.c_str() );
					else
						KLFT_THROW_ERROR2( ERR_FILE_UPLOADING_CANCELED, 
							fileId.c_str(), remoteClientName.c_str() );
				}
				foundFile = fit->second.m_T;
			}

			fDesc = foundFile->GetFileDescription();

			fileUnlocker.Init( foundFile );
		}

		if ( !useUpdaterFolder && fDesc.m_fileName!=fileName )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

		if ( neededSize==0 )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		int bs = fDesc.m_fileSize - pos;
		if ( bs<0 || ( bs==0  && fDesc.m_fileSize!=0 ) )
			KLFT_THROW_ERROR( ERR_BAD_FUNCTION_PARAMETERS );

		if ( (unsigned)bs>neededSize ) bs = neededSize;

		bool endOfFile = false;
		int read = 0;		
		if ( bs!=0 )
		{
			*ppBuff = new char[bs];
			buffSize = bs;
			
			read = fileUnlocker.Get()->GetChunk( remoteClientName,
				pos, *ppBuff, buffSize, 
				endOfFile );
		}
		else 
		{
			*ppBuff = 0;
			buffSize = 0;

			endOfFile = true;
		}

		fullFileSize = fDesc.m_fileSize;

		if ( !useUpdaterFolder && bs!=0 )
		{
			int tenPercent = fullFileSize / 10;
			int percent = 0;			
			unsigned long uplimit = (pos / tenPercent) * tenPercent + tenPercent;
			if ( (pos + bs) > uplimit ) 
			{
				percent = (((uplimit * 100)/fullFileSize) / 10) * 10;
				if ( percent>0 )
				{					
					KLPAR::param_entry_t bodypars[]=
					{
						KLPAR::param_entry_t( EVENT_BODY_REMOTE_HOST_NAME, remoteClientName.c_str() ),
						KLPAR::param_entry_t( EVENT_BODY_UPLOAD_PERCENT, (long)percent )
					};
					KLSTD::CAutoPtr<KLPAR::Params> pEventBody;
					KLPAR::CreateParamsBody(bodypars, KLSTD_COUNTOF(bodypars), 
						&pEventBody );
							
					PublishEvent( EVENT_TYPE_HOST_FILE_PROGRESS, fileId, pEventBody );
				}
			}
		}

		if ( endOfFile && useUpdaterFolder==false )
		{			
			fileUnlocker.Get()->AddUploadedLocation( remoteClientName );

			// publish event
			KLPAR::param_entry_t bodypars[]=
			{
				KLPAR::param_entry_t( EVENT_BODY_REMOTE_HOST_NAME, remoteClientName.c_str() )
			};
			KLSTD::CAutoPtr<KLPAR::Params> pEventBody;
			KLPAR::CreateParamsBody(bodypars, KLSTD_COUNTOF(bodypars), 
				&pEventBody );
					
			PublishEvent( EVENT_TYPE_HOST_FILE_UPLOADED, fileId, pEventBody );

			unsigned long fullSize = fileUnlocker.Get()->GetFileDescription().m_fileSize;

			fileUnlocker.Free();
			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				if ( m_updateAgentInfo.nAgentHostId>=0 )
				{
					// update statistics				
					m_updateAgentStat.nSuccededUploads ++;
					m_updateAgentStat.nSuccededUploadsSize += fullSize;

					unsigned long tcpBytes = 0;
					if ( fullSize > multicastRecived ) tcpBytes = fullSize - multicastRecived;
					m_updateAgentStat.nTCPUploadedSize += tcpBytes;
					m_updateAgentStat.nMulticastUploadedSize += multicastRecived;
					
					SaveUpdateAgentStatistics();
				}
			}
		}

		return read;
	}

	//!\brief Записывает очередную порцию в файл закачки сервером
	void FileTransferImp::PutNextFileChunk( const std::wstring &fileId,			
			unsigned long pos, 
			unsigned long fullFileSize,
			unsigned long CRC,
			void *pBuff, int buffSize )
	{		
		FileAutoUnlock  fileUnlocker;
		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			bool uploadingClientsExceeded = false;
			KLSTD::CAutoPtr<File> foundFile;
			
			FileMapType::iterator fit = m_filesList.find( fileId );
			if ( fit==m_filesList.end() ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

			foundFile = fit->second.m_T;

			fileUnlocker.Init( foundFile );
		}

		bool endFlag = fileUnlocker.Get()->AddChunk( pBuff,  buffSize, CRC, pos,
					fullFileSize );
							
		if ( endFlag )
		{
			// публикуем событие о закачки нового файла
			PublishEvent( EVENT_TYPE_UPLOADED, fileId );
		}							
	}

	//!\brief Возвращает очередную порцию файла
	void FileTransferImp::InitiateFileUpload( const std::wstring &fileName, 
			const std::wstring &fileId,
			const std::wstring &description,
			const bool dirFlag,
			unsigned long fullFileSize,			
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName,
			KLFT::FileType fileType,
			KLFT::UploadingType updateAgentUploadingFlag,
			UpdateAgentLocationList &updateAgentsLocs,
			unsigned long uploadDelay, unsigned long nextChunkDelay )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		KLTRACE3( KLCS_MODULENAME, L"FileTransfer::InitiateFileUpload fileId - '%ls' fileType - %d UpdateAgentFlag - %d updateAgentsLocs.size - %d\n",
				fileId.c_str(), fileType, updateAgentUploadingFlag, updateAgentsLocs.size() );

		FileMapType::iterator fit = m_filesList.find( fileId );

		if ( fit!=m_filesList.end() )
		{
			bool needCheckStatus = true;

			if	 ( fit->second.m_T->GetStatus()==FileDescription::MulticastUploading ) 
				needCheckStatus = false; // файл уже начал качаться по multicast, необходимо только обновить информацию

			if ( needCheckStatus )
			{
				if ( fit->second.m_T->GetStatus()==FileDescription::Uploaded ||
					fit->second.m_T->GetUpdateAgentFileStatus() & File::UAFS_Downloaded )
				{
					// публикуем повторное событие о закачки нового файла
					PublishEvent( EVENT_TYPE_UPLOADED, fileId );
				}
				KLFT_THROW_ERROR1( ERR_FILE_IS_ALREADY_UPLOADING, fileId.c_str() );
			}
		}

		bool forUpdateAgentFlag = false;
		bool forHostFlag = false;

		if ( forUpdateAgentFlag ||m_updateAgentInfo.nAgentHostId!=(-1) ) updateAgentsLocs.clear();

		if ( fit!=m_filesList.end() )
		{
			fit->second.m_T->UpdateClientFile( fileName, fileId, 
				description, dirFlag, fullFileSize, fileType,
				useConnectionNames, connFullName,
				uploadDelay, nextChunkDelay, false,
				updateAgentsLocs, m_workingFolder );
		}
		else
		{
			// создаем описатель файла
			KLSTD::CAutoPtr<File> newFile;

			newFile.Attach( new File() );
			KLSTD_CHKMEM(newFile);

			if ( updateAgentUploadingFlag==UT_ForUpdateAgentOnly ) forUpdateAgentFlag = true;
			if ( updateAgentUploadingFlag==UT_ForHostUploadingOnly ) forHostFlag = true;
			if ( updateAgentUploadingFlag==UT_ForUpdateAgentAndHost ) forUpdateAgentFlag = forHostFlag = true;

			newFile->CreateClientFile( fileName, fileId, 
				description, dirFlag, fullFileSize, fileType,
				useConnectionNames, connFullName,
				uploadDelay, nextChunkDelay, forUpdateAgentFlag,
				updateAgentsLocs, m_workingFolder );

			if ( forUpdateAgentFlag && forHostFlag )
			{
				newFile->ResetUseOnlyForUpdateAgentFlag();
			}
			
			if ( !updateAgentsLocs.empty() && !forUpdateAgentFlag )
			{
				m_updateAgentLocsList = updateAgentsLocs;
				SaveUpdateAgentLocsList();

				bool mutlicastAdded = JoinUpdateAgentMulticastGroup();
				
				if ( uploadDelay<=c_MulticastStartUploadingDelay )
				{
					if ( mutlicastAdded )
					{
						// откладываем закачку по TCP. немного подождем рассылки по multicast
						newFile->IncreaseStartUploadingTimeout( c_MulticastStartUploadingDelay );
					}
					else
					{
						// подождем пока UA получит сигнал на выкачку файла
						newFile->IncreaseStartUploadingTimeout( c_UpdateAgentStartUploadingDelay );
					}
				}
			}

			m_filesList[fileId] = newFile;
		}
			
		AddFileTransferWorker();

		if ( forHostFlag ) PublishEvent( EVENT_TYPE_START_UPLOAD, fileId );

		KLTRACE3( KLCS_MODULENAME, L"FileTransfer::InitiateFileUpload fileId - '%ls' ..OK\n", fileId.c_str() );
	}

	//!\brief Иницирует загрузку package на клиенте
	void FileTransferImp::InitiatePackageUpload( const std::wstring &fileName, const std::wstring &fileId,
			const std::wstring &description,
			const bool dirFlag,			
			const bool useConnectionNames,
			const KLTRAP::Transport::ConnectionFullName &connFullName, 
			KLFT::FileType fileType,
			unsigned long uploadDelay, unsigned long nextChunkDelay )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit!=m_filesList.end() ) 
		{
			fit->second.m_T->UpdatePackageFile( fileType );
			PublishEvent( EVENT_TYPE_START_UPLOAD, fileId );
			AddFileTransferWorker();
			return;
		}

		// создаем описатель файла
		KLSTD::CAutoPtr<File> newFile;

		newFile.Attach( new File() );
		KLSTD_CHKMEM(newFile);

		newFile->CreatePackageFile( fileName, fileId, 
			description, 
			fileType,
			dirFlag, 			
			useConnectionNames, connFullName, 
			uploadDelay, nextChunkDelay, m_workingFolder );
		
		m_filesList[fileId] = newFile;
		
		PublishEvent( EVENT_TYPE_START_UPLOAD, fileId );

		AddFileTransferWorker();
	}

	//!\brief Иницирует загрузку файла на сервере
	void FileTransferImp::InitiateServerFileUpload( const std::wstring &fileId,
			const bool dirFlag,
			unsigned long fullFileSize,
			unsigned long lifeTime,
			const std::wstring &localConnName, 
			const std::wstring &remoteConnName )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit!=m_filesList.end() ) 
		{			
			KLFT_THROW_ERROR1( ERR_FILE_IS_ALREADY_UPLOADING, fileId.c_str() );
		}

		if( KLTR_GetTransport()->IsConnectionActive( localConnName.c_str(), remoteConnName.c_str()) )
		{				
			m_pContie->AddObject( fileId, &m_DisconnectSink, localConnName, remoteConnName );
		}	
		else
		{
			KLFT_THROW_ERROR( ERR_CONNECTION_BROKEN );
		}

		// создаем описатель файла
		KLSTD::CAutoPtr<File> newFile;

		newFile.Attach( new File() );
		KLSTD_CHKMEM(newFile);

		newFile->CreateServerUploadedClientFile( fileId, 
			dirFlag, fullFileSize, remoteConnName,
			m_workingFolder );
		
		m_filesList[fileId] = newFile;
		
		PublishEvent( EVENT_TYPE_START_UPLOAD, fileId );	
	}

	//!\brief Возвращает список идентификаторов файлов, предназначенных для данного клиента
	void FileTransferImp::GetFilesIdsList( const std::wstring &clientName, 
			KLSTD::CAutoPtr<KLPAR::Params> &pList )
	{
		KLPAR_CreateParams( &pList );

		// копируем список в params
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		FileMapType::iterator it = m_filesList.begin();
		for( ; it != m_filesList.end(); ++it )
		{
			FileDescription fDesc = it->second.m_T->GetFileDescription();
			bool found = false;
			for( int c = 0; c < 2 && !found ; c++ )
			{				
				KLTRAP::Transport::LocationExList *locsList = NULL;
				if ( c==0 ) locsList = &fDesc.m_sentLocsList;
				else locsList = &fDesc.m_notSendLocsList;
				KLTRAP::Transport::LocationExList::iterator lit = locsList->begin();
				for( ; lit != locsList->end() ; ++lit )
				{
					if ( clientName.empty() || lit->remoteComponentName==clientName )
					{
						ADD_PARAMS_VALUE( pList, it->second.m_T->GetFileId().c_str(), 
							StringValue, c_filesIdsListIdField );
						found = true;
						break;
					}
				}
			}
		}	

	}

	//!\brief Функция регестрирует агента обновлений
	void FileTransferImp::RegisterUpdateAgent( int nAgentHostId, 
			const std::wstring &strConnName, 
			const std::wstring &strAddress, const std::wstring &strNoSSLAddress, 
			const std::wstring &strMulticastAddr,
			int nGroupId, KLPAR::BinaryValue *pCert, const std::wstring &certPswd )
	{
		KLERR_BEGIN
		{
			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				if ( nGroupId<0 )
				{
					m_saveUpdateAgentInfo = m_updateAgentInfo;
					m_updateAgentInfo = UpdateAgentInfo();
				}
				else
				{
					m_saveUpdateAgentInfo = m_updateAgentInfo;

					m_updateAgentInfo.nAgentHostId = nAgentHostId;			
					m_updateAgentInfo.nGroupId = nGroupId;
					m_updateAgentInfo.strConnName = strConnName;
					m_updateAgentInfo.strAddress = strAddress;
					m_updateAgentInfo.strNonSSLAddress = strNoSSLAddress;
					m_updateAgentInfo.strMulticastAddr = strMulticastAddr;
					m_updateAgentInfo.pCertificate = NULL;
					if ( pCert!=NULL )
					{
						KLSTD_AllocMemoryChunk( pCert->GetSize(), &m_updateAgentInfo.pCertificate );
						memcpy( m_updateAgentInfo.pCertificate->GetDataPtr(), pCert->GetValue(), pCert->GetSize() );
					}
					m_updateAgentInfo.strCertPswd = certPswd;

					KLTRACE3( KLCS_MODULENAME, L"FileTransfer::RegisterUpdateAgent agentHostId - %d groupId - %d address - '%ls'.\n",
						nAgentHostId, nGroupId, strAddress.c_str() );
				}
			}

			StopUpdateAgent();
		}
		KLERR_ENDT(2)

		if ( nGroupId<0 )
		{
			DeleteUpdateAgentInfo();
			
			std::vector<std::wstring> fileIdsForDel;
			{
				// удаляем файлы синхронизации
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				FileMapType::iterator it = m_filesList.begin();
				for( ; it!=m_filesList.end(); ++it )
				{
					if ( it->second.m_T->Type()==KLFT::FT_Folder )
					{
						fileIdsForDel.push_back( it->second.m_T->GetFileId() );
					}
				}
			}

			for( unsigned int i = 0; i < fileIdsForDel.size(); i++ )
			{
				DeleteFile( fileIdsForDel[i] );
			}

			return;
		}

		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			m_updateAgentInfo.nAgentHostId = nAgentHostId;
		}

		SaveUpdateAgentInfo();
		
		KLERR_BEGIN
		{
			ReadUpdateAgentInfo();

			ActivateUpdateAgent();
		}
		KLERR_ENDT(2)

		PublishEvent( EVENT_TYPE_USING_LISTENERS_PORTS_LIST_CHANGED, L"", NULL );
	}
	
	//!\brief Функция проверки статуса агента обновления на сервере
	void FileTransferImp::CheckUpdateAgentStatus( const KLTRAP::Transport::ConnectionFullName &serverConnName )
	{
		KLERR_BEGIN
		{
			KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::CheckUpdateAgentStatus remote name - '%ls' local name - '%ls'\n",
				serverConnName.remoteComponentName.c_str(), serverConnName.localComponentName.c_str() );

			std::wstring strAgentHostId;
			int uaHostId;
			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				if ( m_updateAgentInfo.nAgentHostId<0 ) return;
				strAgentHostId = m_updateAgentInfo.strAgentHostId;
				uaHostId = m_updateAgentInfo.nAgentHostId;
			}

			KLTRAP::TransportProxy trProxy;		
			trProxy.Initialize( serverConnName.localComponentName.c_str(), serverConnName.remoteComponentName.c_str() );
					
			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );

			struct soap* pSoap = proxyLocker.Get();
					
			struct klft_GetHostUpdateAgentInfoResponse res;

			soap_call_klft_GetHostUpdateAgentInfo( pSoap, NULL, NULL,
						(wchar_t *)strAgentHostId.c_str(),
						res );

			proxyLocker.CheckResult();
			
			if ( res.error.code )
			{	
				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );				
				throw pError;
			}

			if ( res.updatesAgents.__size==0 )
			{
				// удаляем UA с данного хоста
				RegisterUpdateAgent( uaHostId, L"", L"", L"", L"",
					(-1), NULL, L"" );
			}
		}		
		KLERR_ENDT(2)

	}
	
	//\brief Фнукиця переводит работу FT на резервное хранилище обновлений на сервере
	void FileTransferImp::SwitchToReserveUpdaterFolder( const std::wstring &wstrNagentHostId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		m_wstrNagentHostId = wstrNagentHostId;
	}

	//\brief Возвращает параметры работы с резервным хранилищем обновлений
	void FileTransferImp::IsReserveUpdaterFolderActivated( bool &bUseReserveUpdate, 
			std::wstring &wstrNagentHostId )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		if ( !m_wstrNagentHostId.empty() )
		{
			bUseReserveUpdate = true;
			wstrNagentHostId = m_wstrNagentHostId;
		}
		else
		{
			bUseReserveUpdate = false;
		}
	}

	void FileTransferImp::DoBroadcastCall( 
			KLSTD::CAutoPtr<CBroadcastCallInfo> pInfo)
	{
		std::list<std::wstring>::iterator it = pInfo->m_filesIdsForBroadcasting.begin();		
		for( ; it != pInfo->m_filesIdsForBroadcasting.end(); 
			++it )
		{
			KLTRAP::Transport::LocationExList locsList;
			KLSTD::CAutoPtr<FileTransferBroadcaster> broadcaster;
			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
				FileMapType::iterator fit = m_filesList.find( (*it) );
				if ( fit!=m_filesList.end() ) 
				{
					KLSTD::CAutoPtr<File> f = fit->second.m_T;
					FileDescription fDesc = f->GetFileDescription();
			
					locsList = fDesc.m_notSendLocsList;
					
					
					// добавляем Broadcast для рассылки сигналов на клиентам		
					broadcaster.Attach( new FileTransferBroadcaster( this, (*it), locsList ) );				
					
					KLSTD_CHKMEM(broadcaster);
					
					std::wstring trComponentName = m_localConnName;

					broadcaster->broadcastId =
						KLTR_GetTransport()->CreateBroadcast( trComponentName.c_str(), 
						broadcaster->m_locsList, broadcaster );

					f->SetBroadcastId( broadcaster->broadcastId );
				}
			}

			if ( broadcaster!=NULL )				
				KLTR_GetTransport()->StartBroadcast( broadcaster->broadcastId );

			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
				FileMapType::iterator fit = m_filesList.find( (*it) );
				if ( fit!=m_filesList.end() ) 
				{
					KLSTD::CAutoPtr<File> f = fit->second.m_T;
					f->InBroadcastFlag( false );
				}
			}

		}
	}

	//!\brief Функция загрузки
	int FileTransferImp::RunWorker( KLTP::ThreadsPool::WorkerId wId )
	{
		std::list<std::wstring> filesIdsForUploading;		
		UsedConnectionInfo connectionInfo;
		std::wstring fileIdUAError;

		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::RunWorker(). File transfer working thread... wId - %d\n", 
												wId );		

		bool checkServerFilesListWorkerFlag = false;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_checkFilesListWorkerId==wId ) checkServerFilesListWorkerFlag = true;
		}
		if ( checkServerFilesListWorkerFlag )
		{
			DoCheckServerFilesList();
			return 0;
		}			

		do
		{
			filesIdsForUploading.clear();
			std::list<std::wstring> filesIdsForBroadcasting;
			{		
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
				
				FileMapType::iterator it = m_filesList.begin();
				for( ; it != m_filesList.end(); ++it )
				{
					if ( it->second.m_T->IsNeedToUpload() && filesIdsForUploading.empty() ) // за один раз закачиваем только один файл
					{
						if ( it->second.m_T->Type()==KLFT::FT_Folder && m_updateAgentInfo.nAgentHostId==(-1) )
							continue; // синхронизируем папку только на агентах обновлений

						filesIdsForUploading.push_back( it->second.m_T->GetFileId() );						
					}
					if ( it->second.m_T->IsNeedToBroadcast() )
					{
						it->second.m_T->InBroadcastFlag( true );
						filesIdsForBroadcasting.push_back( it->second.m_T->GetFileId() );
					}
				}
			}
			
			std::wstring clientName, saveClientName, fileLocalName, fileName, fileId; 
			unsigned long startFilePos = 0, neededSize = c_DefaultChunkSize, fullFileSize = 0,
				multicastRecvSize = 0;			
			
			std::list<std::wstring>::iterator fidit = filesIdsForUploading.begin();		
			for( ; fidit != filesIdsForUploading.end(); ++fidit )
			{
				bool found = false;
				bool forUpdateAgentFlag = false;
				KLFT::FileType fileType = FT_None;
				bool bPackageFileFlag = false, foldersIsEqual = false;				
				KLFT::FolderSync::FolderFilesInfo folderFileInfo;
				KLFT::FolderSync::FileInfoList	  deletedFilesList;
				KLFT::FolderSync::SubfoldersList	subFoldersList;
				KLFT::UpdateAgentLocationList		uaLocs;
				bool bLastPartlyChunk = false;

				{
					KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
					
					FileMapType::iterator fit = m_filesList.find( (*fidit) );
					if ( fit!=m_filesList.end() )
					{
						KLSTD::CAutoPtr<File> f = fit->second.m_T;
						FileDescription fDesc = f->GetFileDescription();
							
						fileType = f->Type();

						clientName = f->GetClientName();
						fileLocalName = f->GetLocalName();					
						if ( !fileLocalName.empty() )
						{
							saveClientName = clientName;
							clientName = fileLocalName;
						}
						else
						{
							if ( fileType==FT_Simple && !m_strHostId.empty() ) clientName = m_strHostId;
						}
						
						fileName = fDesc.m_fileName;
						fileId = fDesc.m_fileId;	
						fullFileSize = fDesc.m_fileSize;
						multicastRecvSize = fDesc.m_multicastReceived;
						forUpdateAgentFlag = f->IsForUpdateAgentFile();
						uaLocs = f->GetUpdateAgentList();						

						if ( f->Type()==KLFT::FT_Package || f->Type()==KLFT::FT_Folder )
						{
							f->GetFullSyncInfo( folderFileInfo );
							subFoldersList = f->GetSubfoldersListForFolderSync();
							bPackageFileFlag = true;
						}
						else
						{
							if ( f->PartlySavedFile() )
							{
								bool bfchunk = f->GetNextNeededChunk( startFilePos, neededSize, neededSize );
								if ( !bfchunk )
								{
									KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Whole file is uploaded. Can't get next chunk for file. File id - '%ls'\n", 
												fDesc.m_fileId.c_str() );
									// посылаем символический запрос для рассылки статистики и установки статуса
									// последний байт
									neededSize = 1;
									startFilePos = fDesc.m_fileSize - 1;
									bLastPartlyChunk = true;
								}
							}
							else
							{
								startFilePos = fDesc.m_currentUploadPos;
							}
							
							if ( startFilePos==fDesc.m_fileSize ) // в случае закачи файла но не пробивки корректного статуса ( сбой )
							{
								neededSize = 1;
								startFilePos = fDesc.m_fileSize - 1;
								bLastPartlyChunk = true;
							}
						}
						
						found = true;
					}
				}
				
				if ( found )
				{
					bool success = false;
					
					KLSTD::CArrayPointer2<char>	resBuff;
					int				resSize = 0;
					std::wstring	resFileName, resFileId;
					unsigned long	resCRC, resstartPosInFile, resfullFileSize;					
									
					
					// вызываем сервер
					KLERR_TRY
					{	
						if ( !uaLocs.empty() && fileIdUAError!=(*fidit) )
						{
							if ( connectionInfo.m_bNeedToClose==false )
							{
								connectionInfo.m_connType = UA_ConnType;

								KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Connect to update agent. name - '%ls' location - '%ls'.\n", 
												uaLocs[0].strConnectionName.c_str(), uaLocs[0].strSSLLocation.c_str() );
								
								connectionInfo.Init( m_localConnName, m_serverConnName );
								bool cRes = ConnectToUpdateAgent( connectionInfo.m_connLocalName, 
									connectionInfo.m_connRemoteName, &uaLocs );
								if ( cRes==false )
								{
									KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Can't create connection to update agent. name - '%ls' location - '%ls'.\n", 
												uaLocs[0].strConnectionName.c_str(), uaLocs[0].strSSLLocation.c_str() );									
									connectionInfo.m_connType = NO_ConnType;
								}
								else
								{
									connectionInfo.m_bNeedToClose = true;
								}
							}
						}
						
						if ( connectionInfo.m_connType==NO_ConnType )
						{
							bool activeFlag = false, useOnlyExistConn = false;

							uaLocs.clear();

							if ( !saveClientName.empty() )
							{
								KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Connect to server. name - '%ls' location - '%ls'.\n", 
												fileLocalName.c_str(), saveClientName.c_str() );

								connectionInfo.Init( fileLocalName, saveClientName );								
								useOnlyExistConn = true;
							}
							else
							{
								KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Connect to server. name - '%ls' location - '%ls'.\n", 
												m_localConnName.c_str(), m_serverConnName.c_str() );

								connectionInfo.Init( m_localConnName, m_serverConnName );
							}

							activeFlag = KLTR_GetTransport()->IsConnectionActive( connectionInfo.m_connLocalName.c_str(), 
									connectionInfo.m_connRemoteName.c_str() );
							
							if ( activeFlag==false )
							{
								if ( useOnlyExistConn )
								{
									KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Use only existing connection mode. Connection wasn't found. local - '%ls' remote - '%ls'.\n", 
												connectionInfo.m_connLocalName.c_str(), connectionInfo.m_connRemoteName.c_str() );
									
									{
										KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
					
										FileMapType::iterator fit = m_filesList.find( (*fidit) );
										if ( fit!=m_filesList.end() )
										{
											KLSTD::CAutoPtr<File> f = fit->second.m_T;
											f->ChangeUploadDelayTimeout( true ); // сбрасываем время послед. операции ( след. закачку через timeout )
										}
									}

									continue;
								}
								// попробуем вызвать invoker для создания соединения
								if ( m_connectionInvoker!=NULL )
								{
									m_connectionInvoker->CreateConnection(
										connectionInfo.m_connLocalName.c_str(),
										connectionInfo.m_connRemoteName.c_str() );
									connectionInfo.m_bNeedToClose = true;
								}
							}

							connectionInfo.m_connType = SRV_ConnType;
						}
						
						KLTRAP::TransportProxy trProxy;		
						trProxy.Initialize( connectionInfo.m_connLocalName.c_str(), connectionInfo.m_connRemoteName.c_str() );
						
						if ( connectionInfo.m_connType==SRV_ConnType )
						{
							LoadServerFileInfo( trProxy );
						}

						KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
						
						struct soap* pSoap = proxyLocker.Get();

						KLSTD::CAutoPtr<KLERR::Error> pError;						
						if ( bPackageFileFlag )
						{
							struct SOAPFilesInfoList		filesList;							
							
							filesList.__size = folderFileInfo.FilesList.size();
							if ( filesList.__size>0 )
							{
								filesList.__ptr = soap_new_SOAPFolderFileInfo( pSoap, filesList.__size );								
								KLFT::FolderSync::FileInfoList::iterator it = folderFileInfo.FilesList.begin();
								int c = 0;
								for( ; it != folderFileInfo.FilesList.end(); ++it, ++c )
								{
									filesList.__ptr[c].CRC = it->CRC;
									filesList.__ptr[c].FileNameHash = KLPAR::soap_strdup( pSoap, it->FileNameHash.c_str() );
								}
							}
							else
							{
								filesList.__ptr = NULL;
							}

							struct SOAPSyncFolderOptions syncFolderOpts;
							FillSOAPSyncFolderOptions( pSoap, &syncFolderOpts, subFoldersList );

							struct klft_SendToClientFullSyncArchiveResponse res;							
							
							soap_call_klft_SendToClientFullSyncArchive( pSoap, NULL, NULL,
								(wchar_t *)m_strLocalHostConnName.c_str(),
								(wchar_t *)fileId.c_str(), folderFileInfo.bFileNameHashed,
								&filesList, &syncFolderOpts,
								res );					
							
							proxyLocker.CheckResult();

							if ( res.folderIsEqual==true ) foldersIsEqual = true;
							if ( res.error.code )
							{
								KLPAR::ExceptionFromSoap( res.error, &pError );								
							}

							if ( res.deletedFilesList!=NULL && res.deletedFilesList->__size>0 )
							{
								for( int c = 0; c < res.deletedFilesList->__size; c++ )
								{
									KLFT::FolderSync::FolderFileInfo fi( res.deletedFilesList->__ptr[c].CRC, 
										res.deletedFilesList->__ptr[c].FileNameHash );
									deletedFilesList.push_back( fi );
								}
							}
						}
						else
						{						
							struct klft_GetNextFileChunkResponse res;

							struct klft_GetNextFileChunkOptions	 options;
							soap_default_klft_GetNextFileChunkOptions( pSoap, &options );
							options.nMulticastRecvSize = multicastRecvSize;
							bool needToSendOptions = (fullFileSize==0)?false:((startFilePos+neededSize)>=fullFileSize);
								
							soap_call_klft_GetNextFileChunk( pSoap, NULL, NULL,
								(wchar_t *)clientName.c_str(), (wchar_t *)fileName.c_str(),
								(wchar_t *)fileId.c_str(), startFilePos,  neededSize, 
								false, forUpdateAgentFlag, needToSendOptions?&options:NULL,
								res );					
							
							proxyLocker.CheckResult();

							if ( res.error.code )
							{
								KLPAR::ExceptionFromSoap( res.error, &pError );
							}
							
							if ( res.error.code==0 )
							{
								resBuff = new char[res.chunk.buff.__size]; 
								memcpy( resBuff.get(), res.chunk.buff.__ptr, res.chunk.buff.__size );
								resSize = res.chunk.buff.__size;
								resFileName = res.chunk.fileName;
								resFileId = res.chunk.fileId;
								resCRC = (unsigned long)res.chunk.CRC;
								resstartPosInFile = (unsigned long)res.chunk.startPosInFile;
								resfullFileSize = (unsigned long)res.chunk.fullFileSize;
							}
						}
						
						success = true;

						if ( pError!=NULL )
						{
							if ( pError->GetId()==ERR_FILE_UPLOADING_CANCELED ||
								 pError->GetId()==ERR_WRONG_FILE_ID ||
								 pError->GetId()==ERR_UA_UPLOADING_FILE_FROM_SERVER )
							{
								if ( connectionInfo.m_connType==UA_ConnType  )
								{
									long failCount = LONG_MAX;

									{
										KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
					
										FileMapType::iterator fit = m_filesList.find( (*fidit) );
										if ( fit!=m_filesList.end() )
										{
											KLSTD::CAutoPtr<File> f = fit->second.m_T;
											failCount = f->GetUploadChunkFailedCount();
										}

										KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Error working with UA. Error id - %d File id - '%ls' Failed count - %d\n", 
											pError->GetId(), (*fidit), failCount );

										if ( failCount>=4 )
										{
											KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Switch to server.\n" );

											// опускаем соединение для последующего закрытия
											KLERR_IGNORE( proxyLocker.Release() );

											// иницируем пересоздание соединения в след. итерации							
											connectionInfo.CloseConn();

											// сохраняем идентификатор файла, за которым необходимо ходить на сервер
											fileIdUAError = (*fidit);
										}
										else
										{
											// файла еще нет на UA подождем еще немного
											if ( fit!=m_filesList.end() ) fit->second.m_T->ChangeUploadDelayTimeout( true, true );
										}
									}
									
									continue;
								}
								else
								{
									if ( !forUpdateAgentFlag ) PublishEvent( EVENT_TYPE_UPLOADING_CANCELED, fileId );
									DeleteFile( fileId );
								}
							}
							
							KLERR::Error *pThrowError = NULL;
							pError.CopyTo(&pThrowError);
							throw pThrowError;
						}
					}
					KLERR_CATCH( pError )
					{
						if ( std::wstring(pError->GetModuleName())==KLTRAP::KLTRAP_ModuleName )
						{
							// иницируем пересоздание соединения в след. итерации							
							connectionInfo.CloseConn();							
						}

						KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
					
						FileMapType::iterator fit = m_filesList.find( (*fidit) );
						if ( fit!=m_filesList.end() )
						{
							KLSTD::CAutoPtr<File> f = fit->second.m_T;
							f->ChangeUploadDelayTimeout( true );
						}
					}
					KLERR_ENDTRY	
												
					if ( success ) 
					{
						bool endFlag = false;
						
						KLERR_TRY
						{					
							KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
							
							FileMapType::iterator fit = m_filesList.find( (*fidit) );
							if ( fit!=m_filesList.end() )
							{
								KLSTD::CAutoPtr<File> f = fit->second.m_T;
								FileDescription fDesc = f->GetFileDescription();

								if ( bPackageFileFlag )
								{
									KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
									f->GetFolderSync( pFolderSync );
									pFolderSync->SyncDeletedFiles( deletedFilesList );

									f->SetStatus( FileDescription::Uploaded );
									if ( foldersIsEqual )
									{
										PublishEvent( EVENT_TYPE_UPLOADED, f->GetFileId() );
										
										if ( m_updateAgentInfo.nAgentHostId>=0 )
										{
											// update statistics			
											time( &m_updateAgentStat.tFolderSyncLastServerSyncTime );
											m_updateAgentStat.nFolderSyncServerSyncCount++;
											
											SaveUpdateAgentStatistics();
										}
									}
								}
								else
								{
									if ( resFileName!=fDesc.m_fileName ||
										resFileId!=fDesc.m_fileId ||
										resfullFileSize!=fDesc.m_fileSize )
									{
										KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::TransferWorkerFunc(). Wrong file information. File id - '%ls'\n", 
											fDesc.m_fileId.c_str() );
										continue;
									}	

									if ( bLastPartlyChunk )
									{
										f->SetStatus( FileDescription::Uploaded );
										f->SaveInfoFile();
										endFlag = true;
									}
									else
									{									
										f->SetStatus( FileDescription::Uploading );
										
										endFlag = f->AddChunk( resBuff.get(), 
											resSize, resCRC, resstartPosInFile,
											resfullFileSize );
									}

									f->ChangeUploadDelayTimeout( false );
								}

								multicastRecvSize = fDesc.m_multicastReceived;
							}
							
						}
						KLERR_CATCH( pError )
						{
							KLERR_SAY_FAILURE( 1, pError );

							KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
							
							FileMapType::iterator fit = m_filesList.find( (*fidit) );
							if ( fit!=m_filesList.end() )
							{
								fit->second.m_T->ChangeUploadDelayTimeout( true );
							}
						}
						KLERR_ENDTRY
						
						if ( endFlag )
						{
							if ( fileType==KLFT::FT_SyncArchive )
							{
								ApplyPackageSyncArchive( (*fidit) );
							}
							else
							{									
								if ( forUpdateAgentFlag ) 
								{
									KLERR_BEGIN
									{
										CreateUpdateAgentServerFile( fileId );
									}
									KLERR_ENDT(2);
								}
								else
								{
									// публикуем событие о закачки нового файла
									PublishEvent( EVENT_TYPE_UPLOADED, fileId );
								}
							}
						}							
						
					}
				}
			}
		
			if ( !filesIdsForBroadcasting.empty() )
			{
				KLSTD::CAutoPtr<CBroadcastCallInfo> pInfo;
				pInfo.Attach(new CBroadcastCallInfo);
				KLSTD_CHKMEM(pInfo);
			
				pInfo->m_filesIdsForBroadcasting = filesIdsForBroadcasting;
			
				KLTMSG::AsyncCall1T<FileTransferImp, KLSTD::CAutoPtr<FileTransferImp::CBroadcastCallInfo> >
					::Start(this, &FileTransferImp::DoBroadcastCall, pInfo);
			}
		} while ( !filesIdsForUploading.empty() && !m_shutdownFlag );

		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::RunWorker(). File transfer working thread...END wId - %d\n", 
												wId );		
						
		return 0;
	}

	//!\brief Считывает info файлы в рабочей директории
	void FileTransferImp::LoadInfoFiles()
	{
	#ifdef WIN32

		WIN32_FIND_DATA FindFileData;

	#ifdef UNICODE
		std::wstring sFolder(m_workingFolder);		
		HANDLE hFind = ::FindFirstFile(std::wstring(sFolder + L"\\*" + c_InfoFileExt ).c_str(), &FindFileData);
		if (hFind == (void *)0xFFFFFFFF) return;
		while(true)
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::wstring fileName = std::wstring(sFolder + L"\\" + 
					std::wstring(FindFileData.cFileName)).c_str();

				LoadInfoFile( fileName );
			}
	#else		
		std::string sFolder( KLSTD_W2A2( m_workingFolder.c_str() ) );
		HANDLE hFind = ::FindFirstFile(std::string(sFolder + "\\*.*").c_str(), &FindFileData);
		while(true)
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string fileName = std::string(sFolder + "\\" + 
					std::string(FindFileData.cFileName)).c_str();				
				LoadInfoFile( (const wchar_t *)KLSTD_A2W2( fileName.c_str() ) );
			}
	#endif //  UNICODE
			if (!::FindNextFile(hFind, &FindFileData)) break;	
		}
	#endif // WIN32

		std::vector<std::wstring> syncPackageFileIds;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			FileMapType::iterator it = m_filesList.begin();
			while( it != m_filesList.end() )
			{
				if ( it->second.m_T->Type()==KLFT::FT_SyncArchive && it->second.m_T->GetStatus()==FileDescription::Uploaded )
				{
					syncPackageFileIds.push_back( it->second.m_T->GetFileId() );
				}
				if ( it->second.m_T->Type()==KLFT::FT_CustomServerUploaded )
				{
					it->second.m_T->DeleteWorkingFiles();
					m_filesList.erase( it );
					it = m_filesList.begin();
				}
				else ++it;
			}
		}

		std::vector<std::wstring>::iterator it = syncPackageFileIds.begin();
		for( ;it != syncPackageFileIds.begin(); ++it )
		{
			ApplyPackageSyncArchive( (*it) );
		}
	}

	//!\brief Инициализация info файла
	void FileTransferImp::LoadInfoFile( const std::wstring &infoFilePath )
	{		
		KLERR_BEGIN
		{
			// создаем описатель файла
			KLSTD::CAutoPtr<File> newFile;
			
			newFile.Attach( new File() );
			KLSTD_CHKMEM(newFile);
			
			newFile->CreateFromInfoFile( infoFilePath );
			
			// добавляем файл в список
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			m_filesList[newFile->GetFileId()] = newFile;

			if ( (newFile->GetStatus()==FileDescription::Sending ||
				newFile->GetStatus()==FileDescription::Uploading ||
				newFile->GetStatus()==FileDescription::MulticastUploading ) && 
				!m_workerAdded )
			{
				AddFileTransferWorker();
			}
			
		}
		KLERR_ENDT( 3 )
	}

	//!\brief Устанавливает соединение с сервером считывает информацию о файлах на сервере и удаляет устаревшие на клиенте
	void FileTransferImp::DoCheckServerFilesList()
	{
		KL_TMEASURE_BEGIN(L"FileTransferImp::DoCheckServerFilesList", 3)		
		KLERR_BEGIN
		{
			std::wstring localConnName, remoteConnName;
			{
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				if ( m_checkFilesListWorkerFirstStart )
				{
					// пропускаем первый запуск, дабы не загружать систему при старте
					m_checkFilesListWorkerFirstStart = false;
					return; 
				}

				if ( m_filesList.size()==0 ) return;
				if ( m_localConnName.empty() || m_localConnName.empty() ) return;
				if ( m_connectionInvoker==NULL ) return;

				localConnName = m_localConnName, remoteConnName = m_serverConnName;

				time_t curTime;
				time( &curTime );
				if ( m_lastLoadFilesInfo!=0 && (m_lastLoadFilesInfo + c_CheckFilesListOnServerSec)>curTime ) return;
			}


			bool activeFlag = KLTR_GetTransport()->IsConnectionActive( localConnName.c_str(), 
					remoteConnName.c_str() );
			
			if ( !activeFlag )
			{
				// попробуем вызвать invoker для создания соединения
				if ( m_connectionInvoker!=NULL )
				{
					m_connectionInvoker->CreateConnection(
						localConnName.c_str(),
						remoteConnName.c_str() );				
				}
			}

			{
				KLTRAP::TransportProxy trProxy;		
				trProxy.Initialize( localConnName.c_str(), remoteConnName.c_str() );

				LoadServerFileInfo( trProxy );
			}

			if ( !activeFlag )
			{
				KLTR_GetTransport()->CloseClientConnection(
						localConnName.c_str(),
						remoteConnName.c_str() );
			}
		}
		KLERR_ENDT(3)		
		KL_TMEASURE_END()
	}
	
	//!\brief Считывает информацию о файлах на сервере и удаляет устаревшие на клиенте
	void FileTransferImp::LoadServerFileInfo( KLTRAP::TransportProxy &trProxy )
	{
		std::wstring clientId;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			time_t curTime;
			time( &curTime );
			if ( m_lastLoadFilesInfo!=0 && (m_lastLoadFilesInfo + c_CheckFilesListOnServerSec)>curTime )
				return;

			m_lastLoadFilesInfo = curTime;
			clientId = m_strHostId;
			if ( m_updateAgentInfo.nAgentHostId!=(-1) ) clientId.erase(); // для UA файл удаляется только после физического удланиея на сервере ( не проверяем список клиентов )
		}

		KL_TMEASURE_BEGIN( L"FileTransferImp::LoadServerFileInfo", 3 );

		KLERR_BEGIN
		{
			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );

			struct soap* pSoap = proxyLocker.Get();
		
			struct klft_GetServerFilesListResponse res;
					
			soap_call_klft_GetServerFilesList( pSoap, NULL, NULL,
						(wchar_t *)clientId.c_str(), res );					

			proxyLocker.CheckResult();

			if ( res.error.code )
			{			
				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );							
				throw pError;
			}

			KLSTD::CAutoPtr<KLPAR::Params>	pParams;
			KLPAR::ParamsFromSoap( res.filesIdsList, &pParams );

			KLPAR::ParamsNames parNames;
			pParams->GetNames( parNames );

			{			
				KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

				FileMapType::iterator it = m_filesList.begin();
				while( it != m_filesList.end() )
				{
					if ( std::find( parNames.begin(), parNames.end(), it->second.m_T->GetFileId() )==
						parNames.end() )
					{
						KLSTD_TRACE1( 3, L"Delete working file ( this file doesn't exist in server ). File id - '%ls'\n",
							it->second.m_T->GetFileId().c_str() );

						it->second.m_T->DeleteWorkingFiles();
						m_filesList.erase( it );
						it = m_filesList.begin();
					}
					else  ++it;
				}
			}
		}
		KLERR_ENDT( 3 )

		KL_TMEASURE_END();
	}

	//!\brief Возвращает информацию о файлах в папке Updater
	void FileTransferImp::GetUpdaterFolderFileInfo( const std::wstring &fileName, 
			FileInfo &r )
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
				
		KLSTD::CAutoPtr<File> newFile = CreateUpdaterFolderFileDescription( fileName );

		newFile->ReadFileSize();

		FileDescription fd = newFile->GetFileDescription();
		r.m_fileName = fileName;
		r.m_isDirectory = fd.m_isDirectory;
		r.m_fileSize = fd.m_fileSize;
		//r.m_createTime = fd.
		r.m_readPermissionOk = true;		
	}

	void FileTransferImp::DeleteAlreadyUploadedClient( const std::wstring &fileId, 
		const std::wstring &remoteComponentName )
	{
		// переносим клиента в список уже отосланных
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
		FileMapType::iterator fit = m_filesList.find( fileId );
		if ( fit!=m_filesList.end() ) 
		{
			FileDescription fDesc = fit->second.m_T->GetFileDescription();			
			if ( fit->second.m_T->AddSentLocation( remoteComponentName ) )
			{
				KLPAR::param_entry_t bodypars[]=
				{
					KLPAR::param_entry_t( EVENT_BODY_REMOTE_HOST_NAME, remoteComponentName.c_str() )
				};
				KLSTD::CAutoPtr<KLPAR::Params> pEventBody;
				KLPAR::CreateParamsBody(bodypars, KLSTD_COUNTOF(bodypars), 
					&pEventBody );
				
				PublishEvent( EVENT_TYPE_START_HOST_FILE_UPLOAD, fileId, pEventBody );
			}

			KLTRAP::Transport::LocationExList::iterator cit = fDesc.m_uploadLocsList.begin();
			bool found = false;
			for( ; cit != fDesc.m_uploadLocsList.end() && !found; ++cit )
			{
				if ( cit->remoteComponentName==remoteComponentName) found = true;
			}
			if ( !found )
			{

				fit->second.m_T->AddUploadedLocation( remoteComponentName );
			}
		}
	}

	//!\brief Добавление рабочего потока
	void FileTransferImp::AddFileTransferWorker()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
		if ( !m_workerAdded )
		{
			m_workerAdded = true;
			KLTP_GetThreadsPool()->AddWorker( &m_transferWorkerId,
				L"File transfer worker", this, 
				KLTP::ThreadsPool::RunNormal,
				true, c_NextChunkTimeout );
		}		
	}

	//!\brief Публикация события
	void FileTransferImp::PublishEvent( const std::wstring &eventTypeName, 
			const std::wstring &fileId, KLPAR::Params *pEventBody )
	{
		KLPRCI::ComponentId localComponentName;

		bool convRes = KLTRAP::ConvertTransportNameToComponentId( localComponentName, m_localConnName );
		if ( !convRes ) return;

		
		KLSTD::CAutoPtr<KLPAR::Params> pParams;

		if ( !fileId.empty() )
		{
			KLSTD::CAutoPtr<KLPAR::StringValue> pStrValue;
			KLPAR::CreateValue( fileId.c_str(), &pStrValue );
			
			if ( pEventBody==NULL )
			{
				KLPAR_CreateParams( &pParams );
				pParams->AddValue( EVENT_BODY_FILE_ID, pStrValue );
			}
			else
			{
				pEventBody->ReplaceValue( EVENT_BODY_FILE_ID, pStrValue );
			}
		}
		else
		{
			KLPAR_CreateParams( &pParams );
		}

		KLSTD::CAutoPtr<KLEV::Event> pEvent;
		KLEV_CreateEvent(&pEvent, localComponentName, eventTypeName, 
			(pEventBody!=NULL)?pEventBody:(KLPAR::Params *)pParams );
		
		KLEV_GET_GLOBAL_EVENTSOURCE();
		if ( pGlobalEventSource!=NULL ) 
			pGlobalEventSource->PublishEvent(pEvent);
	}
	
	FileTransferImp::FileTransferBroadcaster::FileTransferBroadcaster( 
		FileTransferImp *fileTransfer, const std::wstring &fileId,
		const KLTRAP::Transport::LocationExList &locsList )
	{
		m_fileTransfer = fileTransfer;
		m_workingFileId = fileId;
		m_locsList = locsList;
	}

	FileTransferImp::FileTransferBroadcaster::~FileTransferBroadcaster()
	{
	}
			
	int FileTransferImp::FileTransferBroadcaster::BroadcastCallback( 
		KLTRAP::Transport::BroadcastId broadcastId, 
		KLTRAP::Transport::BroadcastState state, 
		const wchar_t *localComponentName, 
		const wchar_t *remoteComponentName,
		void *userData )
	{
		KLERR_TRY
		{
			if ( state==KLTRAP::Transport::BroadcastConnectionEstablished )
				m_fileTransfer->SendUploadSignal( m_workingFileId, 
					localComponentName, remoteComponentName );
		}
		KLERR_CATCH( pError )

			if ( std::wstring(pError->GetModuleName())==KLFT_ModuleName && 
				pError->GetId()==ERR_FILE_IS_ALREADY_UPLOADING )
			{
				m_fileTransfer->DeleteAlreadyUploadedClient( m_workingFileId, remoteComponentName );				
			}
			KLERR_SAY_FAILURE(3, pError);
		KLERR_ENDTRY

		return 0;
	}

	//!\brief Создает описатель файла для файла из папки updater
	File *FileTransferImp::CreateUpdaterFolderFileDescription( const std::wstring &fileName )
	{
		if ( m_updaterFolder.empty() ) 
			KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, fileName.c_str() );

		if ( fileName.find(L"../")!=std::wstring::npos || 
			 fileName.find(L"..\\")!=std::wstring::npos )
			KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, fileName.c_str() );

		std::wstring filePath =  m_updaterFolder + fileName;
		
		// ищем описатель данного файла в списке
		FileMapType::iterator it = m_filesList.begin();
		for( ; it!=m_filesList.end(); ++it )
		{
			if ( it->second.m_T->IsCustomModeFile() && it->second.m_T->GetFileId()==filePath )
			{
				return (it->second.m_T);
			}
		}

		// создаем описатель файла
		KLSTD::CAutoPtr<File> newFile;

		newFile.Attach( new File() );
		KLSTD_CHKMEM(newFile);

		KLERR_TRY
		{	
			newFile->CreateUpdaterFolderFile( filePath, m_maxNumberUploadingClientsForUpdaterFolder );
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );

			KLFT_THROW_ERROR1( ERR_FILE_NOT_FOUND, fileName.c_str() );
		}						
		KLERR_ENDTRY	

		m_filesList[KLSTD_CreateGUIDString()] = newFile;

		return newFile;
	}

	//!\brief Создает файл на UpdateAgent для дальнейшего распространения
	void FileTransferImp::CreateUpdateAgentServerFile( const std::wstring &fileId )
	{
		KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::CreateUpdateAgentServerFile fileId - '%ls'\n",
				fileId.c_str() );

		KLTRAP::TransportProxy trProxy;		
		trProxy.Initialize( m_localConnName.c_str(), m_serverConnName.c_str() );
		
		KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
		
		struct soap* pSoap = proxyLocker.Get();
		
		struct klft_GetFullFileInfoResponse res;
		
		soap_call_klft_GetFullFileInfo( pSoap, NULL, NULL,
			(wchar_t *)fileId.c_str(), res );					
		
		proxyLocker.CheckResult();
		
		if ( res.error.code )
		{
			KLERR::Error *pError=NULL;
			KLPAR::ExceptionFromSoap( res.error, &pError );							
			throw pError;
		}

				
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			FileMapType::iterator fit = m_filesList.find( fileId );
			if ( fit==m_filesList.end() ) KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, fileId.c_str() );

			if ( !fit->second.m_T->GetUseOnlyForUpdateAgentFlag() )
			{
				// публикуем событие о закачки нового файла
				PublishEvent( EVENT_TYPE_UPLOADED, fileId );
			}

			KLSTD::CAutoPtr<KLPAR::Params> pFullInfoPar;
			KLPAR::ParamsFromSoap( res.fileInfo, &pFullInfoPar );

			fit->second.m_T->ConvertToServerFile( pFullInfoPar );

			if ( m_updateAgentInfo.nAgentHostId>=0 )
			{
				// update statistics
				m_updateAgentStat.nNumberOfFiles++;
				m_updateAgentStat.nSizeOfFiles += fit->second.m_T->GetFileDescription().m_fileSize;
				SaveUpdateAgentStatistics();
			}

			if ( m_pMulticastSender )
			{
				m_pMulticastSender->SendFile( fileId, fit->second.m_T->GetFileDescription().m_fileSize );
			}
		}
	}

	//!\brief Сохраняет информацию агента обновления
	void FileTransferImp::SaveUpdateAgentInfo()
	{
		KLERR_TRY
		{
			KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
			KLPAR_CreateParams( &parSerFile );

			ADD_PARAMS_VALUE( parSerFile, c_serializeUAHostId, IntValue, m_updateAgentInfo.nAgentHostId );
			ADD_PARAMS_VALUE( parSerFile, c_serializeUAGroupId, IntValue, m_updateAgentInfo.nGroupId );
			ADD_PARAMS_VALUE( parSerFile, c_serializeUAConnName, StringValue, m_updateAgentInfo.strConnName.c_str() );
			ADD_PARAMS_VALUE( parSerFile, c_serializeUAAddr, StringValue, m_updateAgentInfo.strAddress.c_str() );
			ADD_PARAMS_VALUE( parSerFile, c_serializeUANOSSLAddr, StringValue, m_updateAgentInfo.strNonSSLAddress.c_str() );
			ADD_PARAMS_VALUE( parSerFile, c_serializeUAMulticastIP, StringValue, m_updateAgentInfo.strMulticastAddr.c_str() );
			

			if ( m_updateAgentInfo.pCertificate!=NULL )
			{
				KLSTD::CAutoPtr<KLPAR::ValuesFactory> pFactory;
				KLSTD::CAutoPtr<KLPAR::BinaryValue> pBinData;
				KLPAR_CreateValuesFactory(&pFactory);			
				pFactory->CreateBinaryValue(&pBinData);
				pBinData->SetValue( m_updateAgentInfo.pCertificate->GetDataPtr(), 
					m_updateAgentInfo.pCertificate->GetDataSize() );
				parSerFile->AddValue( c_serializeUACert, pBinData );

				ADD_PARAMS_VALUE( parSerFile, c_serializeUACertPswd, StringValue, m_updateAgentInfo.strCertPswd.c_str() );
			}			

			std::wstring uaInfoFileName = m_workingFolder + L"/" + c_UAInfoFileName;

			KLPAR_SerializeToFileName2( uaInfoFileName, KLPAR_SF_BINARY, parSerFile );
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::SaveUpdateAgentInfo error: can't save update agent info.\n" );
		}
		KLERR_ENDTRY

		SaveUpdateAgentStatistics();
	}

	//!\brief Производинт чтение настроек агента обновления
	void FileTransferImp::ReadUpdateAgentInfo()
	{
		std::wstring uaInfoFileName = m_workingFolder + L"/" + c_UAInfoFileName;
		if ( !KLSTD_IfExists( uaInfoFileName.c_str() ) ) return;

		KLERR_TRY
		{
			KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
			KLPAR_DeserializeFromFileName( uaInfoFileName, &parSerFile );

			GET_PARAMS_VALUE( parSerFile, c_serializeUAHostId, IntValue, INT_T, m_updateAgentInfo.nAgentHostId );
			GET_PARAMS_VALUE( parSerFile, c_serializeUAGroupId, IntValue, INT_T, m_updateAgentInfo.nGroupId );
			GET_PARAMS_VALUE( parSerFile, c_serializeUAConnName, StringValue, STRING_T, m_updateAgentInfo.strConnName );
			GET_PARAMS_VALUE( parSerFile, c_serializeUAAddr, StringValue, STRING_T, m_updateAgentInfo.strAddress );
			GET_PARAMS_VALUE_NO_THROW( parSerFile, c_serializeUANOSSLAddr, StringValue, STRING_T, m_updateAgentInfo.strNonSSLAddress );
			GET_PARAMS_VALUE( parSerFile, c_serializeUAMulticastIP, StringValue, STRING_T, m_updateAgentInfo.strMulticastAddr );

			KLSTD::CAutoPtr<KLPAR::BinaryValue> pVal = (KLPAR::BinaryValue*)parSerFile->GetValue2(c_serializeUACert, false); 
			if (pVal && pVal->GetType() == KLPAR::Value::BINARY_T) 
			{
				m_updateAgentInfo.pCertificate = NULL;
				KLSTD_AllocMemoryChunk( pVal->GetSize(), &m_updateAgentInfo.pCertificate );
				memcpy( m_updateAgentInfo.pCertificate->GetDataPtr(), pVal->GetValue(), pVal->GetSize() );

				GET_PARAMS_VALUE( parSerFile, c_serializeUACertPswd, StringValue, STRING_T, m_updateAgentInfo.strCertPswd );
			}
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::ReadUpdateAgentInfo error: can't read update agent info.\n" );
		}
		KLERR_ENDTRY

		ReadUpdateAgentStatistics();
	}

	//!\brief Удаляет информацию о агенте обновления
	void FileTransferImp::DeleteUpdateAgentInfo()
	{
		KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

		std::wstring uaInfoFileName = m_workingFolder + L"/" + c_UAInfoFileName;
		if ( KLSTD_IfExists( uaInfoFileName.c_str() ) ) 
		{
			KLERR_BEGIN
			{
				KLSTD_Unlink( uaInfoFileName.c_str(), true );
			}
			KLERR_ENDT(2)
		}
		std::wstring uaStatFileName = m_workingFolder + L"/" + c_UAStatsFileName;
		if ( KLSTD_IfExists( uaStatFileName.c_str() ) ) 
		{
			KLERR_BEGIN
			{
				KLSTD_Unlink( uaStatFileName.c_str(), true );
			}
			KLERR_ENDT(2)
		}

		m_updateAgentInfo = UpdateAgentInfo();
		m_updateAgentStat = UpdateAgentStatistics();

		PublishEvent( EVENT_TYPE_USING_LISTENERS_PORTS_LIST_CHANGED, L"", NULL );
	}

	//!\brief Сохраняет информацию об агентах обновлений
	void FileTransferImp::SaveUpdateAgentLocsList()
	{
		KLERR_TRY
		{
			KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
			KLPAR_CreateParams( &parSerFile );

			UpdateAgentLocationList::iterator it = m_updateAgentLocsList.begin();
			int counter = 0;
			for( ; it != m_updateAgentLocsList.end(); ++it, counter++ )
			{
				KLSTD::CAutoPtr<KLPAR::Params> parUALoc;
				KLPAR_CreateParams( &parUALoc );

				ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUAConnName, StringValue, it->strConnectionName.c_str() );
				ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUALocation, StringValue, it->strLocation.c_str() );
				ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUASSLLocation, StringValue, it->strSSLLocation.c_str() );
				ADD_PARAMS_VALUE( parUALoc, c_serializeParamsUAMulticastIP, StringValue, it->strMulticastAddr.c_str() );
				
				std::wostringstream ws;
				ws << counter;

				ADD_PARAMS_VALUE( parSerFile, ws.str().c_str(), ParamsValue, parUALoc );
			}

			std::wstring uaInfoLocFileName = m_workingFolder + L"/" + c_UAInfoLocFileName;

			KLPAR_SerializeToFileName2( uaInfoLocFileName, KLPAR_SF_BINARY, parSerFile );
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::SaveUpdateAgentLocsListerror: can't save update agent info.\n" );
		}
		KLERR_ENDTRY
	}

	//!\brief Производинт чтение информации об агентах обновлений
	void FileTransferImp::ReadUpdateAgentLocsList()
	{
		std::wstring uaInfoLocFileName = m_workingFolder + L"/" + c_UAInfoLocFileName;
		if ( !KLSTD_IfExists( uaInfoLocFileName .c_str() ) ) return;

		KLERR_TRY
		{
			KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
			KLPAR_DeserializeFromFileName( uaInfoLocFileName, &parSerFile );

			KLPAR::ParamsNames parNames;
			parSerFile->GetNames( parNames );
			KLPAR::ParamsNames::iterator it = parNames.begin();		
			for( ; it != parNames.end(); ++it )
			{
				KLSTD::CAutoPtr<KLPAR::Params> parUALoc;
				GET_PARAMS_VALUE( parSerFile, (*it).c_str(), ParamsValue, PARAMS_T, parUALoc );			

				UpdateAgentLocation uaLoc;
				GET_PARAMS_VALUE( parUALoc, c_serializeParamsUAConnName, StringValue, STRING_T, uaLoc.strConnectionName );
				GET_PARAMS_VALUE( parUALoc, c_serializeParamsUALocation, StringValue, STRING_T, uaLoc.strLocation );
				GET_PARAMS_VALUE( parUALoc, c_serializeParamsUASSLLocation, StringValue, STRING_T, uaLoc.strSSLLocation );
				GET_PARAMS_VALUE( parUALoc, c_serializeParamsUAMulticastIP, StringValue, STRING_T, uaLoc.strMulticastAddr );

				m_updateAgentLocsList.push_back( uaLoc );
			}
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::ReadUpdateAgentInfo error: can't read update agent info.\n" );
		}
		KLERR_ENDTRY
	}

	//!\brief Сохраняет статистику агента обновлений
	void FileTransferImp::SaveUpdateAgentStatistics()
	{
		KLERR_TRY
		{	
			KLPAR::param_entry_t uapars[]=
			{
				KLPAR::param_entry_t(c_serializeUAST_CREATETIME,				KLPAR::time_wrapper_t( m_updateAgentStat.tCreateTime ) ),
				KLPAR::param_entry_t(c_serializeUAST_NUMBEROFFILES,				m_updateAgentStat.nNumberOfFiles),
				KLPAR::param_entry_t(c_serializeUAST_SIZEOFFILES,				m_updateAgentStat.nSizeOfFiles),
				KLPAR::param_entry_t(c_serializeUAST_SUCCEDEDUPLOADS,			m_updateAgentStat.nSuccededUploads),
				KLPAR::param_entry_t(c_serializeUAST_SUCCEDEDUPLOADSSIZE,		m_updateAgentStat.nSuccededUploadsSize),
				KLPAR::param_entry_t(c_serializeUAST_TCPUPLOADEDSIZE,			m_updateAgentStat.nTCPUploadedSize),
				KLPAR::param_entry_t(c_serializeUAST_MULTICASTUPLOADEDSIZE,     m_updateAgentStat.nMulticastSentSize),
				KLPAR::param_entry_t(c_serializeUAST_MULTICASTSENTSIZE,			m_updateAgentStat.nMulticastUploadedSize),
				KLPAR::param_entry_t(c_serializeUAST_FOLDERSYNCLATSSERVERSYNCTIME,	KLPAR::time_wrapper_t( m_updateAgentStat.tFolderSyncLastServerSyncTime) ),
				KLPAR::param_entry_t(c_serializeUAST_FOLDERSYNCSERVERSYNCCOUNT,		m_updateAgentStat.nFolderSyncServerSyncCount),
				KLPAR::param_entry_t(c_serializeUAST_FOLDERSYNCMULTICASTSENTCOUNT,  m_updateAgentStat.nFolderSyncMulticastSentCount),
				KLPAR::param_entry_t(c_serializeUAST_FOLDERSYNCMULTICASTSENTSIZE,   m_updateAgentStat.nFolderSyncMulticastSentSize),
				KLPAR::param_entry_t(c_serializeUAST_FOLDERSYNCTCPUPLOADEDSIZE,		m_updateAgentStat.nFolderSyncTCPUploadedSize),
				KLPAR::param_entry_t(c_serializeUAST_FOLDERSYNCTCPUPLOADEDCOUNT,    m_updateAgentStat.nFolderSyncTCPUploadedCount)
			};

			KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
			KLPAR::CreateParamsBody(uapars, KLSTD_COUNTOF(uapars), &parSerFile);

			std::wstring uaStatFileName = m_workingFolder + L"/" + c_UAStatsFileName;

			KLPAR_SerializeToFileName2( uaStatFileName, KLPAR_SF_BINARY, parSerFile );
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::SaveUpdateAgentStatistics: can't save update agent info.\n" );
		}
		KLERR_ENDTRY
	}

	//!\brief Востанавливаем статистику агента обновлений
	void FileTransferImp::ReadUpdateAgentStatistics()
	{
		std::wstring uaStatFileName = m_workingFolder + L"/" + c_UAStatsFileName;
		if ( !KLSTD_IfExists( uaStatFileName .c_str() ) ) return;

		KLERR_TRY
		{
			KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
			KLPAR_DeserializeFromFileName( uaStatFileName, &parSerFile );

			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_CREATETIME, DateTimeValue, DATE_TIME_T, m_updateAgentStat.tCreateTime );
			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_NUMBEROFFILES, IntValue, INT_T, m_updateAgentStat.nNumberOfFiles);
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_SIZEOFFILES, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nSizeOfFiles);
			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_SUCCEDEDUPLOADS, IntValue, INT_T, m_updateAgentStat.nSuccededUploads);
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_SUCCEDEDUPLOADSSIZE, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nSuccededUploadsSize);						
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_TCPUPLOADEDSIZE, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nTCPUploadedSize);
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_MULTICASTUPLOADEDSIZE, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nMulticastSentSize);
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_MULTICASTSENTSIZE, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nMulticastUploadedSize);
			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_FOLDERSYNCLATSSERVERSYNCTIME, DateTimeValue, DATE_TIME_T, m_updateAgentStat.tFolderSyncLastServerSyncTime );
			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_FOLDERSYNCSERVERSYNCCOUNT, IntValue, INT_T, m_updateAgentStat.nFolderSyncServerSyncCount);
			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_FOLDERSYNCMULTICASTSENTCOUNT, IntValue, INT_T, m_updateAgentStat.nFolderSyncMulticastSentCount);
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_FOLDERSYNCMULTICASTSENTSIZE, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nFolderSyncMulticastSentSize);
			GET_PARAMS_VALUE_2TYPES_NO_THROW( parSerFile, c_serializeUAST_FOLDERSYNCTCPUPLOADEDSIZE, LongValue, LONG_T, IntValue, INT_T, m_updateAgentStat.nFolderSyncTCPUploadedSize);
			GET_PARAMS_VALUE( parSerFile, c_serializeUAST_FOLDERSYNCTCPUPLOADEDCOUNT, IntValue, INT_T, m_updateAgentStat.nFolderSyncTCPUploadedCount);
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );
			KLTRACE2( KLCS_MODULENAME, L"FileTransferImp::ReadUpdateAgentStatistics error: can't read update agent info.\n" );
		}
		KLERR_ENDTRY
	}

	//!\brief Активирует работы агента обновлений
	void FileTransferImp::ActivateUpdateAgent()
	{
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_updateAgentInfo.nAgentHostId==(-1) ) return;

			KLTRACE3( KLCS_MODULENAME, L"FileTransfer::ActivateUpdateAgent agentHostId - %d groupId - %d strConnName - '%ls' address - '%ls' mulitcastId - '%ls' m_pSer - %d.\n",
				m_updateAgentInfo.nAgentHostId, m_updateAgentInfo.nGroupId, 
				m_updateAgentInfo.strConnName.c_str(),
				m_updateAgentInfo.strAddress.c_str(), 
				m_updateAgentInfo.strMulticastAddr.c_str(), (m_pSer!=NULL) );

			int userPort = 0;
			KLSTD::CAutoPtr<KLPAR::BinaryValue> pCertBinValue;
			KLPAR::CreateValue(
						KLPAR::binary_wrapper_t(
									m_updateAgentInfo.pCertificate->GetDataPtr(),
									m_updateAgentInfo.pCertificate->GetDataSize()),
						&pCertBinValue);

			if ( !m_updateAgentInfo.strAddress.empty() )
			{
				{
					// добавляем функцию для приема вызовов ( по умолчанию данная функция регестрируется только на сервере )
					struct KLSTRT::SOAPStubFunc ft_funcs_ptrs[2]=
					{
						{ L"klft_GetNextFileChunk",		 (void*)klft_GetNextFileChunkUAStub },
						{ L"", NULL },
					};

					KLSTRT::RegisterGSOAPStubFunctions( ft_funcs_ptrs, (sizeof(ft_funcs_ptrs)/sizeof(ft_funcs_ptrs[0])) );
				}

				KLWAT_SetCustomCredentialsConverter( this );

				std::wstring locConnName = m_updateAgentInfo.strConnName;
				KLPRCI::ComponentId processNameTmp;
				bool convRes = KLTRAP::ConvertTransportNameToComponentId( processNameTmp,
					m_localConnName );

				if ( convRes )
				{
					processNameTmp.instanceId = m_updateAgentInfo.strConnName;
					processNameTmp.componentName = c_FileTransferComponentName;

					KLTRAP::ConvertComponentIdToTransportName( locConnName, processNameTmp );
				}

				if ( m_saveUpdateAgentInfo.strAddress!=m_updateAgentInfo.strAddress && m_pSer!=NULL )
				{
					KLTR_GetTransport()->AddSSLListenLocation( locConnName.c_str(),
						m_updateAgentInfo.strAddress.c_str(), userPort, pCertBinValue, m_pSer, 
						m_updateAgentInfo.strCertPswd.c_str() );
				}
				
				if ( !m_updateAgentInfo.strNonSSLAddress.empty() && m_saveUpdateAgentInfo.strNonSSLAddress!=m_updateAgentInfo.strNonSSLAddress )
				{
					KLTR_GetTransport()->AddListenLocation( locConnName.c_str(),
						m_updateAgentInfo.strNonSSLAddress.c_str(), userPort );
					
				}

			}

			if ( !m_updateAgentInfo.strMulticastAddr.empty() )
			{
				KLERR_BEGIN
				{
					KLSTD::CAutoPtr<KLPAR::BinaryValue> pBinKey;
					void*   pHostPrvKey = NULL;
					size_t  nHostPrvKey = 0;
					KLERR_TRY
						int nResult = KLCSPWD::GetData(
										KLCSPWD::c_szLocalPrvKey,
										pHostPrvKey,
										nHostPrvKey);
						if(nResult)
							KLSTD_THROW_LASTERROR_CODE2(nResult);
						;
						KLSTD::CAutoPtr<KLPAR::ValuesFactory> pFactory;
						KLPAR_CreateValuesFactory(&pFactory);
						pFactory->CreateBinaryValue(&pBinKey);
						pBinKey->SetValue(pHostPrvKey, nHostPrvKey);						
					KLERR_CATCH(pError)
						KLERR_SAY_FAILURE(1, pError);
					KLERR_FINALLY
						KLCSPWD::Free(pHostPrvKey, nHostPrvKey);
					KLERR_ENDTRY

					KLFT_CreateMulticastSender( &m_pMulticastSender );
					m_pMulticastSender->Init( m_updateAgentInfo.strMulticastAddr, 
						/*pBinKey*/ NULL, this );
				}
				KLERR_ENDT(2);
			}
		}
		
	}

	//!\brief Останавливает работу агента обновлений
	void FileTransferImp::StopUpdateAgent()
	{
		KLSTD::CAutoPtr<MulticastSender> pMulticastSender;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			if ( m_saveUpdateAgentInfo.nAgentHostId==(-1) ) return;

			KLTRACE3( KLCS_MODULENAME, L"FileTransfer::StopUpdateAgent agentHostId - %d groupId - %d address - '%ls' mulitcastId - '%ls'.\n",
				m_saveUpdateAgentInfo.nAgentHostId, m_saveUpdateAgentInfo.nGroupId, 
				m_saveUpdateAgentInfo.strAddress.c_str(), m_saveUpdateAgentInfo.strMulticastAddr.c_str() );

			if ( !m_saveUpdateAgentInfo.strAddress.empty() )
			{
				std::wstring locConnName = m_saveUpdateAgentInfo.strConnName;
				KLPRCI::ComponentId processNameTmp;
				bool convRes = KLTRAP::ConvertTransportNameToComponentId( processNameTmp,
					m_localConnName );

				if ( convRes )
				{
					processNameTmp.instanceId = m_saveUpdateAgentInfo.strConnName;
					processNameTmp.componentName = c_FileTransferComponentName;

					KLTRAP::ConvertComponentIdToTransportName( locConnName, processNameTmp );
				}

				if ( m_saveUpdateAgentInfo.strAddress!=m_updateAgentInfo.strAddress )
				{
					KLERR_BEGIN
					{
						KLTR_GetTransport()->DeleteListenerAndCloseConnections( locConnName.c_str(), 
							m_saveUpdateAgentInfo.strAddress.c_str(), 0 );
					}
					KLERR_ENDT(2);
				}

				if ( !m_saveUpdateAgentInfo.strNonSSLAddress.empty() &&
					m_saveUpdateAgentInfo.strNonSSLAddress!=m_updateAgentInfo.strNonSSLAddress )
				{
					KLERR_BEGIN
					{
						KLTR_GetTransport()->DeleteListenerAndCloseConnections( locConnName.c_str(), 
							m_saveUpdateAgentInfo.strNonSSLAddress.c_str(), 0 );
					}
					KLERR_ENDT(2);
				}

				KLERR_IGNORE( KLWAT_SetCustomCredentialsConverter( NULL ) );
			}

			if ( !m_saveUpdateAgentInfo.strMulticastAddr.empty() )
			{
				if ( m_pMulticastSender )
				{
					pMulticastSender = m_pMulticastSender;
					m_pMulticastSender = NULL;
				}
			}

		}
	
		KLERR_BEGIN
		{
			if ( pMulticastSender )
			{
				pMulticastSender->Stop();
			}
		}
		KLERR_ENDT(2);

		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );

			m_updateAgentInfo.nAgentHostId = (-1);
		}
	}

	//!\brief Подключает текущего агента к мультикаст группе рассылки
	bool FileTransferImp::JoinUpdateAgentMulticastGroup()
	{
		UpdateAgentLocationList::const_iterator it = m_updateAgentLocsList.begin();
		bool mutlicastAdded = false;
		for( ; it != m_updateAgentLocsList.end(); ++it )
		{
			if ( !it->strMulticastAddr.empty() )
			{
				mutlicastAdded = true;
				KLERR_TRY
				{
					KLTR_GetTransport()->JoinMulticastGroup( it->strMulticastAddr.c_str(), /*m_pSer*/ NULL );
				}
				KLERR_CATCH( pError )
				{
					KLERR_SAY_FAILURE( 2, pError );
					continue;
				}
				KLERR_ENDTRY;						
			}
		}

		return mutlicastAdded;
	}

	//!\brief Возвращает FolderSync для папки синхроонизации
	void FileTransferImp::GetFileFolderSync( const std::wstring &remoteClientName,
				const std::wstring &syncFolderFileId, FileAutoUnlock &fileUnlocker, 
				KLSTD::CAutoPtr<KLFT::FolderSync> &pFolderSync )
	{		
		std::wstring fileDescription;
		FileDescription fDesc;
		{		
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
			
			KLSTD::CAutoPtr<File> foundFile;

			FileMapType::iterator fit = m_filesList.find( syncFolderFileId );
			if ( fit==m_filesList.end() ) 
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_ID, syncFolderFileId.c_str() );

			foundFile = fit->second.m_T;			
			fDesc = foundFile->GetFileDescription();
			fileDescription = fDesc.m_description;

			if ( foundFile->Type()!=KLFT::FT_Folder )
				KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, (fileDescription + L":" + syncFolderFileId).c_str() );

			bool uploadingClientsExceeded = false;
			if ( !foundFile->CheckUploadingClient( remoteClientName,
					uploadingClientsExceeded ) )
			{
				if ( uploadingClientsExceeded )
					KLFT_THROW_ERROR2( ERR_MAX_UPLOADING_CLIENTS_NUMBER_EXCEEDED,
						(fileDescription + L":" + syncFolderFileId).c_str(), remoteClientName.c_str() );
				else
					KLFT_THROW_ERROR2( ERR_FILE_UPLOADING_CANCELED, 
						(fileDescription + L":" + syncFolderFileId).c_str(), remoteClientName.c_str() );
			}

			fileUnlocker.Init( foundFile );
		}
		
		fileUnlocker.Get()->GetFolderSync( pFolderSync );

		if ( pFolderSync==NULL )
			KLFT_THROW_ERROR1( ERR_WRONG_FILE_FORMAT, (fileDescription + L":" + syncFolderFileId).c_str() );
	}

	//!\brief Оброаботка файла после успешной синхронизации
	void FileTransferImp::ProcessSyncFolderFile( const std::wstring &folderFileId )
	{
		KLERR_BEGIN
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
			FileMapType::iterator fit = m_filesList.find( folderFileId );
			if ( fit!=m_filesList.end() ) 
			{
				KLSTD::CAutoPtr<File> f = fit->second.m_T;
				
				if ( f->Type()==KLFT::FT_Folder && f->IfExistsUnprocessedLastSyncChangedFiles() && m_pMulticastSender!=NULL )
				{
					std::vector<std::wstring> folderFiles;
					f->GetUnprocessedLastSyncChangedFiles( folderFiles );
					if ( !folderFiles.empty() )
					{
						m_pMulticastSender->SendSyncFolderFiles( folderFileId, folderFiles );

						if ( m_updateAgentInfo.nAgentHostId>=0 )
						{
							// update statistics			
							m_updateAgentStat.nFolderSyncMulticastSentCount++;
							SaveUpdateAgentStatistics();
						}
					}
				}
			}
		}
		KLERR_ENDT(2);
	}

	//!\brief Производит распаковку файла синхронизации пакета/папки
	void FileTransferImp::ApplyPackageSyncArchive( const std::wstring &fileId )
	{
		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::ApplyPackageSyncArchive... File id - '%ls'\n", 
							fileId.c_str() );
		
		FileAutoUnlock  fileUnlocker;
		std::wstring packageFileId;
		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
			KLERR_BEGIN
			{										
				FileMapType::iterator fit = m_filesList.find( fileId );
				if ( fit!=m_filesList.end() )
				{
					packageFileId = fit->second.m_T->GetFileDescription().m_description;

					KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::ApplyPackageSyncArchive. Found package Archive id - '%ls' Package id - '%ls'\n", 
							fileId.c_str(), packageFileId.c_str() );

					FileMapType::iterator packfit = m_filesList.find( packageFileId );
					if ( packfit!=m_filesList.end() )
					{	
						fileUnlocker.Init( packfit->second.m_T );						
					}
					else
					{
						KLTRACE3( KLCS_MODULENAME, L"FileTransferImp::ApplyPackageSyncArchive. Can't find package file. Archive id - '%ls' Package id - '%ls'\n", 
							fileId.c_str(), packageFileId.c_str() );

						packageFileId.clear();

						fit->second.m_T->DeleteWorkingFiles();
						m_filesList.erase( fit );
					}
				}
			}
			KLERR_ENDT(2);

			if ( m_updateAgentInfo.nAgentHostId>=0 )
			{
				// update statistics			
				time( &m_updateAgentStat.tFolderSyncLastServerSyncTime );
				m_updateAgentStat.nFolderSyncServerSyncCount++;
				
				SaveUpdateAgentStatistics();
			}	
		}

		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::ApplyPackageSyncArchive  fileUnlocker.Get()->ApplyPackageSyncArchive File id - '%ls'\n", 
							fileId.c_str() );

		if ( fileUnlocker.Get()!=NULL )
		{
			fileUnlocker.Get()->ApplyPackageSyncArchive( fileId );
			fileUnlocker.Free();
			PublishEvent( EVENT_TYPE_UPLOADED, packageFileId );
		}

		{
			KLSTD::AutoCriticalSection autoUnlock( m_pCricSec );
		
			KLERR_BEGIN
			{										
				FileMapType::iterator fit = m_filesList.find( fileId );
				if ( fit!=m_filesList.end() )
				{
					fit->second.m_T->DeleteWorkingFiles();						
					m_filesList.erase( fit );
				}
			}
			KLERR_ENDT(2);
		}
		
		if ( !packageFileId.empty() )
		{
			ProcessSyncFolderFile( packageFileId );
		}

		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::ApplyPackageSyncArchive...END File id - '%ls'\n", 
							fileId.c_str() );
	}

	void FileTransferImp::ConvertCustomCredentials(
            const KLPAR::Params*        pCustomCredetials,
            KLWAT::CustomAccessToken&   customAccessToken,
			std::wstring&               wstrUserName,
			std::wstring&               wstrUserDomain,
			std::wstring&               wstrUserPassword )
    {
		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::ConvertCustomCredentials\n" );

        KLSTD_CHKINPTR(pCustomCredetials);
        
        KLPAR::Params* pInfo = const_cast<KLPAR::Params*>(pCustomCredetials);
        if(pInfo->DoesExist(KLNAG::c_spAuthNagentId))
        {
            const std::wstring& wstrNagentId = KLPAR::GetStringValue(pInfo, KLNAG::c_spAuthNagentId);
            ;
            if( wstrNagentId.empty() || wstrNagentId != m_updateAgentInfo.strConnName)
            {
				KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::AdjustSSLCredentials wstrNagentId - '%ls' m_updateAgentInfo.strConnName - '%ls'\n",
					wstrNagentId.c_str(), m_updateAgentInfo.strConnName.c_str() );

                KLSTD_THROW(KLSTD::STDE_NOACCESS);
            };
            customAccessToken.vectGroups.resize(1);
            customAccessToken.vectGroups[0] = (int)KLAVT_BA_NAGENT;    
            customAccessToken.wstrLogin.clear();
        }        
        else
        {
            KLSTD_THROW(KLSTD::STDE_NOACCESS);
        };
		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::ConvertCustomCredentials..OK\n" );
    };

    void FileTransferImp::AdjustSSLCredentials(
		void* pPublicKey,
		size_t nPublicKeySize,
        KLWAT::CustomAccessToken& customAccessToken)
    {
		KLTRACE4( KLCS_MODULENAME, L"FileTransferImp::AdjustSSLCredentials\n" );
    };

	void FileTransferImp::OnDisconnectObjectID( const std::wstring& wstrObject )
	{
		const std::wstring &fileId = wstrObject;
		KLERR_IGNORE( DeleteFile( fileId ) );
	}

	static std::wstring NormalizeServerAddress(const std::wstring& wstrAddress)
	{
#ifdef _WIN32
		if(wstrAddress.size() > 2 && wstrAddress[0] == L'\\' && wstrAddress[1] == L'\\')
			return std::wstring(KLTRAP::c_TRLocationPrefic) + &wstrAddress[2];
#endif
		std::wstring wstrProto, wstrHost;
		KLPRCP_SplitAddress(wstrAddress.c_str(), &wstrProto, NULL, &wstrHost);
		if(wstrProto.empty())
		{
			return std::wstring(KLTRAP::c_TRLocationPrefic) + wstrHost;
		}
		return wstrAddress;
	};

	KLCSKCA_DECL void ConnectToServer( const std::wstring &localName, std::wstring &serverRemoteName )
	{
		KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pStorage;
		KLPRSS_OpenSettingsStorageR(
			KLPRSS_GetSettingsStorageLocation(KLPRSS::SS_SETTINGS),
			&pStorage);
		
		KLSTD::CAutoPtr<KLPAR::Params> pSecDynamicData;
		pStorage->Read(
			KLNAG_PRODUCT_DYNAMICDATA,
			KLNAG_VERSION_DYNAMICDATA,
			KLNAG_SECTION_DYNAMICDATA,
			&pSecDynamicData);
		;
		std::wstring wstrServerAddress;
		
		std::vector<int> vecServerPorts(
			KLPAR::MakeStdVector(
			KLSRV::c_pServerPortsDefault,
			KLSTD_COUNTOF(KLSRV::c_pServerPortsDefault)));
		
		std::vector<int> vecServerSslPorts(
			KLPAR::MakeStdVector(
			KLSRV::c_pServerSslPortsDefault,
			KLSTD_COUNTOF(KLSRV::c_pServerSslPortsDefault)));
		
		bool bUseSsl = KLNAG_USE_SSL_DEFAULT;
		
		if(pSecDynamicData->DoesExist(KLNAG::c_spServerAddress))
			wstrServerAddress = NormalizeServerAddress(
						KLPAR::GetStringValue(
						pSecDynamicData,
						KLNAG::c_spServerAddress));
		if(pSecDynamicData->DoesExist(KLNAG::c_spServerPorts))
		{
			KLSTD::CAutoPtr<KLPAR::ArrayValue> pServerPorts;
			KLPAR::GetValue(pSecDynamicData, KLNAG::c_spServerPorts, &pServerPorts);
			KLPAR::MakeStdVector(pServerPorts, vecServerPorts, KLPAR::CUShortConvertor());
			vecServerPorts.push_back(0);
		};
		if(pSecDynamicData->DoesExist(KLNAG::c_spServerSslPorts))
		{
			KLSTD::CAutoPtr<KLPAR::ArrayValue> pServerSslPorts;
			KLPAR::GetValue(pSecDynamicData, KLNAG::c_spServerSslPorts, &pServerSslPorts);
			KLPAR::MakeStdVector(pServerSslPorts, vecServerSslPorts, KLPAR::CUShortConvertor());
			vecServerSslPorts.push_back(0);
		};
#ifdef N_PLAT_NLM
		bUseSsl = false; 
#else
		if(pSecDynamicData->DoesExist(KLNAG_USE_SSL))
			bUseSsl=KLPAR::GetBoolValue(pSecDynamicData, KLNAG_USE_SSL);
#endif		
		std::wstring m_wstrAuthName, m_wstrAuthId;
		KLSTD::CAutoPtr<KLPAR::Params> pSection1;
		pStorage->Read(
			KLNAG_PRODUCT_NAME,
			KLNAG_PRODUCT_VERSION, 
			KLNAG_COMPONENT_NAME,
			&pSection1);
		
		const_cast<std::wstring&>(m_wstrAuthName) = KLPAR::GetStringValue(pSection1, KLNAG::c_spNagentConnName);
		const_cast<std::wstring&>(m_wstrAuthId) = KLPAR::GetStringValue(pSection1, KLNAG::c_spAuthNagentId);
		KLSTD::CAutoPtr<KLPAR::Params> pCustCred;			
		KLPAR::param_entry_t pars[]=
		{
			KLPAR::param_entry_t(KLNAG::c_spAuthNagentName, m_wstrAuthName.c_str()),
				KLPAR::param_entry_t(KLNAG::c_spAuthNagentId, m_wstrAuthId.c_str())
		};
		KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pCustCred);
		
		bool bLocalServer = false;
		if(pSection1->DoesExist(KLNAG_SERVER_INSTALLED))
            const_cast<bool&>(bLocalServer) = KLPAR::GetBoolValue(pSection1, KLNAG_SERVER_INSTALLED);

		
		KLSTD::CAutoPtr<KLPAR::BinaryValue> pCert;
			
		KLPRCI::ComponentId idServer(
			KLCS_PRODUCT_ADMSERVER,
			KLCS_VERSION_ADMSERVER,
			KLCS_COMPONENT_SERVER,
			KLCS_WELLKNOWN_SERVER);
		KLTRAP::ConvertComponentIdToTransportName( serverRemoteName, idServer );
			
		KLPRCP::CProxyBase ServerProxy;
		KLPRCP::conn_attr_t ca;
		ca.m_pszwProxyName = localName.c_str();

		if ( bLocalServer )
		{
			ServerProxy.InitializeLocal( idServer, &ca );
		}
		else
		{
			ca.m_pCredentials = pCustCred;
			ca.m_pCert = pCert;						
			
			ServerProxy.create_new_client_connection(
				idServer,
				wstrServerAddress.c_str(),
				bUseSsl ? &vecServerSslPorts[0] : &vecServerPorts[0],
				bUseSsl,
				&ca);			
		}

		ServerProxy.m_bDestroyConnection = false;
	}
	
	KLCSKCA_DECL void ConnectToMasterServer( const std::wstring &localName, std::wstring &serverRemoteName )
	{
		KLSTD::CAutoPtr<KLPAR::Params> pMasterSrvData;
		KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSS;
		KLPRSS_OpenSettingsStorageR(KLPRSS_GetSettingsStorageLocation(), &pSS);
		pSS->Read(
				KLCS_PRODUCT_ADMSERVER,
				KLCS_VERSION_ADMSERVER,
				KLSRV_MASTER_SRV_SECTION,
				&pMasterSrvData );

		std::wstring wstrMasterSrvAddress;
		std::vector<int> vecMasterSrvPorts;
		KLPRCI::ComponentId idMasterSrv;
		KLSTD::CAutoPtr<KLPAR::BinaryValue> pMasterSrvCert;
		KLPRCP::proxy_auth_t proxyHost;
		KLSTD::CAutoPtr<KLPAR::Params> pSettings;
		pSS->Read(KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KLSRV_COMMON_SETTINGS, &pSettings);

		KLSTD::CAutoPtr<KLPAR::BinaryValue> pCert;
		pCert = KLPAR::LoadCert(
                    pSettings,
                    KLSRV_SSL_SERVER_CERT,
                    KLSRV_SSL_SERVER_CERT_DEFAULT.c_str(),
                    true);
		bool bCompressTraffic(false);
		
		if( !KLSRVH_GetMasterSrvConnectionProp(
			pMasterSrvData,
			wstrMasterSrvAddress,
			vecMasterSrvPorts,
			&idMasterSrv,
			&pMasterSrvCert,
			proxyHost,
			bCompressTraffic) )
		{
			return;
		}

		std::wstring wstrRemoteName = idMasterSrv.PutToString();
		std::wstring wstrLocalName = localName;				
		
		std::wstring wstrMasterSrvAddressAdjusted(wstrMasterSrvAddress);
		std::wstring wstrMasterSrvAddressLoCase(wstrMasterSrvAddress);
		// @TODO: make low case
		if ( wstrMasterSrvAddressLoCase.empty() || wcsstr( wstrMasterSrvAddressLoCase.c_str(), KLTRAP::c_TRLocationPrefic ) != wstrMasterSrvAddressLoCase.c_str() ) 
		{		
			wstrMasterSrvAddressAdjusted = KLTRAP::c_TRLocationPrefic + wstrMasterSrvAddressAdjusted;
		}
		
		KLPRCP::CPortEnumerator addresses(                                    
			wstrMasterSrvAddressAdjusted.c_str(), 
			&vecMasterSrvPorts[0]);
		
		bool bAdded(false);

        KLSTD::CAutoPtr<KLWAT::ClientContext> pCustomAuthClientCtx;
        {
			KLSTD::CAutoPtr<KLWAT::AuthServer> pAuthServer;
            KLWAT_GetGlobalAuthServer(&pAuthServer);
            KLSTD_ASSERT_THROW(pAuthServer);
            pCustomAuthClientCtx = pAuthServer->GenClientContext(
				KLWAT::c_szwMasterAdmServerContext);
        };

		for( std::wstring wstrAddress; !bAdded && addresses.GetNext(wstrAddress); )
		{
			KLTRAP::Transport::ConnectionInfo connInf;
			connInf.localName = wstrLocalName;			//!< Локальное имя компоненты
			connInf.remoteName = wstrRemoteName;		//!< Удаленное имя компоненты
			connInf.remoteLocation = wstrAddress;		//!< Адрес удаленной компоненты
			connInf.pCustomAuthClientCtx = pCustomAuthClientCtx;	//!< Сохраняется локально для данного соединения
			connInf.pRemotePublicSSLKey = pMasterSrvCert;	//!< Публичный ключ SSL удаленной компоненты
			connInf.bCreateSSLConnection = true;
			connInf.pLocalPublicSSLKey = pCert;		//!< Публичный ключ SSL локальной компоненты компоненты 
			connInf.bConnForGateway = false;		//!< Флаг использования данного соединения для gateway
			connInf.bMakeTwoWayAuth = false;			//!< Для данного соединения будет проводиться двухстороная аутонтификация			
			if( !proxyHost.location.empty() )
			{
				connInf.pProxyInfo = &proxyHost;	//!< Информация для создания соединения через прокси сервер
			}
			
			KLSTD_TRACE1(4, L"ConnectToMasterServer AddSSLClientConnection('%ls')\n", wstrAddress.c_str() );
			
			bAdded = KLTR_GetTransport()->AddClientConnection2(connInf);

			KLSTD_TRACE1(4, L"ConnectToMasterServer AddSSLClientConnection returned %u\n", bAdded);

			if(bAdded)
			{				
				serverRemoteName = wstrRemoteName; 
			}
		}

		if(!bAdded)
		{
			KLSTD_TRACE1(3, L"Failed to add connection to server '%ls'\n", wstrRemoteName.c_str());
		}
	}

} // end namespace KLFT


IMPLEMENT_MODULE_INIT_DEINIT(KLFT)

//\brief функция инициализация библиотеки.
IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL,KLFT)

	if ( KLFT::g_FileTransfer!=NULL ) return;

	KLERR_InitModuleDescriptions(
		KLFT::KLFT_ModuleName,
		KLFT::c_errorDescriptions,
		KLSTD_COUNTOF(KLFT::c_errorDescriptions));

	KLFT::FileTransferImp *pFt = new KLFT::FileTransferImp();
	KLSTD_CHKMEM( pFt );
	KLFT::g_FileTransfer.Attach( pFt );
	KLFT::g_FileTransferDestroySem = NULL;
	KLSTD_CreateSemaphore( &KLFT::g_FileTransferDestroySem, 0 );

	KLFT::RegisterGSOAPStubFunctions();	

IMPLEMENT_MODULE_INIT_END()

/*\brief Функция деинициализация библиотеки.*/
IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL,KLFT)

	if ( KLFT::g_FileTransfer==NULL ) return;

	KLFT::UnregisterGSOAPStubFunctions();
	
	KLSTD::CAutoPtr<KLFT::FileTransferImp> saveFt = KLFT::g_FileTransfer;	
	KLFT::g_FileTransfer = NULL;
	saveFt->Shutdown();
	saveFt.Release();
	if ( KLFT::g_FileTransferDestroySem!=NULL )
	{
		KLFT::g_FileTransferDestroySem->Wait();
		delete KLFT::g_FileTransferDestroySem;
		KLFT::g_FileTransferDestroySem = NULL;
	}

	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);

IMPLEMENT_MODULE_DEINIT_END()

/*\brief Возвращает указатель на глобальный объект класса FileTransfer */
KLFT::FileTransfer *KLFT_GetFileTransfer()
{
	if ( ((KLFT::FileTransfer *)KLFT::g_FileTransfer)==NULL )
		KLFT_THROW_ERROR( ERR_FILE_TRANSFER_NOT_INIT );

	return KLFT::g_FileTransfer;
}
