/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filereceiverbridge.cpp
* \author Шияфетдинов Дамир
* \date 2005
* \brief Класс для локального межпроцесного взаимодействия с FileReceiver'ом.
*
*/

#include <soapH.h>

#include <std/par/par_gsoap.h>
#include <std/stress/st_prefix.h>

#include <transport/tr/transport.h>
#include <avp/klids.h>

#include <kca/ft/errors.h>
#include <kca/ft/filetransfer.h>

#include "filetransferimp.h"
#include "filereceiverbridge.h"

#include "modulestub/ftbridgesoapstub.h"

namespace KLFT {

	#define KLCS_MODULENAME L"KLFTBRIDGE"

	KLSTD::CAutoPtr<FileReceiverBridge>	g_FileReciverBridge;
	KLSTD::CAutoPtr<KLSTD::Semaphore>	g_FileReciverBridgeDestroySem;

	FileReceiverBridge::FileReceiverBridge()
	{
		KLSTD_CreateCriticalSection( &m_pCricSec );
	}

	FileReceiverBridge::~FileReceiverBridge()
	{
		if ( g_FileReciverBridgeDestroySem!=NULL ) g_FileReciverBridgeDestroySem->Post();

		if ( !m_socketListenerName.empty() )
		{
			KLERR_IGNORE( KLTR_GetTransport()->DeleteListenLocation( m_socketListenerName.c_str(), m_simpleSocketListenerLoc.c_str(), 0 ) );
		}

		if ( m_pContie )
		{
			m_pContie->Close();
			m_pContie = NULL;
		}
	}

	void FileReceiverBridge::Init()
	{
		KLERR_BEGIN
		{			
			std::wstring listName = KLSTD_CreateLocallyUniqueString();
			int serverLocalUsedPort;
			m_simpleSocketListenerLoc = L"http://127.0.0.1:30523";		
			KLTR_GetTransport()->AddSimpleSocketListenLocation( listName.c_str(),
					m_simpleSocketListenerLoc.c_str(), serverLocalUsedPort );

			m_socketListenerName = listName;
			
			m_DisconnectSink.Init( this );

			CONTIE_CreateConnectionTiedObjectsHelper( &m_pContie );
		}
		KLERR_ENDT(2);
	}

	void FileReceiverBridge::CreateReceiver( std::wstring& receiverId,
		const std::wstring& localConnectionName,
		const std::wstring& remoteConnectionName )
	{
		KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

		receiverId = KLSTD_CreateLocallyUniqueString();

		KLSTD_TRACE1( 3, L"FileReceiverBridge::CreateReceiver receiverId - '%ls'\n", receiverId.c_str() );

		ReceiverBridgeUnit unit;
		unit.m_receiverId = receiverId;

		bool bUseReserveUpdate = false;
		std::wstring wstrNagentHostId;
		((KLFT::FileTransferImp*)KLFT_GetFileTransfer())->IsReserveUpdaterFolderActivated(
			bUseReserveUpdate, wstrNagentHostId );

		if ( bUseReserveUpdate ) unit.m_wstrHostIdForReserveUpdates = wstrNagentHostId;

		if ( !localConnectionName.empty() )
		{
			if( KLTR_GetTransport()->IsConnectionActive( localConnectionName.c_str(), remoteConnectionName.c_str()) )
			{				
				m_pContie->AddObject( receiverId, &m_DisconnectSink, localConnectionName, remoteConnectionName );				
			}	
			else
			{
				KLFT_THROW_ERROR( ERR_CONNECTION_BROKEN );
			}
		}

		m_ReceiversUnits[receiverId] = unit;
	}

	void FileReceiverBridge::CreateReceiver( std::wstring& receiverId )
	{
		CreateReceiver( receiverId, L"", L"" );
	}
	

	ConnectResult FileReceiverBridge::ConnectToServer( const std::wstring& receiverId )
	{
		KLSTD_TRACE1( 3, L"FileReceiverBridge::ConnectToServer receiverId - '%ls'\n", receiverId.c_str() );


		std::wstring localName, serverRemoteName;
		bool bUseUAFolderSync = false;
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				return CR_WrongReceiverId;
			}

			ReceiverBridgeUnit &unit = fit->second;
			if ( unit.m_bConnected!=false )
			{
				return CR_WrongReceiverId;
			}
		}

		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;

		KLERR_TRY
		{
			KLPRCI::ComponentId idLocal(
				KLCS_PRODUCT_ADMSERVER,
				KLCS_VERSION_ADMSERVER,
				L"FileReceiverBridge",
				KLSTD_CreateLocallyUniqueString());
			KLTRAP::ConvertComponentIdToTransportName( localName, idLocal );
			
			bool bAlreaydConnectedToUA = false;

			for( int attemps = 0; attemps < 2; attemps++ )
			{
				if ( CheckFolderSyncUpdateAgent( SYNC_FOLDER_NAME_UPDATE ) )
				{
					KLSTD_TRACE1( 3, L"FileReceiverBridge::ConnectToServer (ConnectToFolderSyncUpdateAgent) receiverId - '%ls'\n", receiverId.c_str() );

					ConnectResult connRes = ConnectToFolderSyncUpdateAgent( 
						SYNC_FOLDER_NAME_UPDATE, pFolderSync,
						localName, serverRemoteName );

					if ( connRes==CR_Ok ) bUseUAFolderSync = true;
					else pFolderSync = NULL;

					bAlreaydConnectedToUA = true;
				}
				if ( !bUseUAFolderSync )
				{
					KLSTD_TRACE1( 3, L"FileReceiverBridge::ConnectToServer (ConnectToServer) receiverId - '%ls'\n", receiverId.c_str() );

					KLFT::ConnectToServer( localName, serverRemoteName );

					if ( attemps==0 )
					{
						bool useUpdateAgent = false;
						bool bListChanged = ((KLFT::FileTransferImp*)KLFT_GetFileTransfer())->TryToGetUpdateAgentInfoFromServer(
							localName, serverRemoteName, useUpdateAgent );
						if ( useUpdateAgent && bListChanged ) 
						{
							KLERR_IGNORE( KLTR_GetTransport()->CloseClientConnection( localName.c_str(),
								serverRemoteName.c_str() ) );
							continue;
						}
					}
				}

				break;
			}
		}
		KLERR_CATCH( pError )
		{
			serverRemoteName.clear();// = L"";
			KLERR_SAY_FAILURE( 2, pError );
			if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
			{
				if ( pError->GetId()==KLTRAP::TRERR_TIMEOUT || 
					pError->GetId()==KLTRAP::TRERR_REMOTE_SERVER_IS_BUSY )
				{
					return CR_ServerBusy;
				}					
			}
			
			return CR_ConnectionError;
		}
		KLERR_ENDTRY	

		bool neeedClose = false;
		if ( !serverRemoteName.empty() )
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				neeedClose = true;				
			}
			else
			{
				ReceiverBridgeUnit &unit = fit->second;
				if ( unit.m_bConnected!=false )
				{
					neeedClose = true;					
				}

				unit.m_bConnected = true;
				unit.m_localName = localName;
				unit.m_serverConnName = serverRemoteName;
				unit.m_bUseUAFolderSync = bUseUAFolderSync;
				unit.m_folderSyncName = SYNC_FOLDER_NAME_UPDATE;
				unit.m_pFolderSync = pFolderSync;
			}
		}

		if ( neeedClose )
		{
			KLTR_GetTransport()->CloseClientConnection( localName.c_str(),
				serverRemoteName.c_str() );
			return CR_WrongReceiverId;
		}

		KLSTD_TRACE1( 3, L"FileReceiverBridge::ConnectToServer receiverId - '%ls'...OK\n", receiverId.c_str() );

		return CR_Ok;
	}
	
	ConnectResult FileReceiverBridge::ConnectToMasterServer( const std::wstring& receiverId )
	{
		KLSTD_TRACE1( 3, L"FileReceiverBridge::ConnectToMasterServer receiverId - '%ls'\n", receiverId.c_str() );

		std::wstring localName, serverRemoteName;
		bool bUseUAFolderSync = false;
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				return CR_WrongReceiverId;
			}

			ReceiverBridgeUnit &unit = fit->second;
			if ( unit.m_bConnected!=false )
			{
				return CR_WrongReceiverId;
			}
		}

		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;

		KLERR_TRY
		{
			KLPRCI::ComponentId idLocal(
				KLCS_PRODUCT_ADMSERVER,
				KLCS_VERSION_ADMSERVER,
				L"FileReceiverBridge",
				KLSTD_CreateLocallyUniqueString());
			KLTRAP::ConvertComponentIdToTransportName( localName, idLocal );

			KLFT::ConnectToMasterServer( localName, serverRemoteName );
		}
		KLERR_CATCH( pError )
		{
			serverRemoteName = L"";
			KLERR_SAY_FAILURE( 2, pError );
			if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
			{
				if ( pError->GetId()==KLTRAP::TRERR_TIMEOUT || 
					pError->GetId()==KLTRAP::TRERR_REMOTE_SERVER_IS_BUSY )
				{
					return CR_ServerBusy;
				}					
			}
			
			return CR_ConnectionError;
		}
		KLERR_ENDTRY	

		bool neeedClose = false;
		if ( !serverRemoteName.empty() )
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				neeedClose = true;				
			}
			else
			{
				ReceiverBridgeUnit &unit = fit->second;
				if ( unit.m_bConnected!=false )
				{
					neeedClose = true;					
				}

				unit.m_bConnected = true;
				unit.m_localName = localName;
				unit.m_serverConnName = serverRemoteName;
				unit.m_bUseUAFolderSync = bUseUAFolderSync;
				unit.m_folderSyncName = SYNC_FOLDER_NAME_UPDATE;
				unit.m_pFolderSync = pFolderSync;
			}
		}

		if ( neeedClose )
		{
			KLTR_GetTransport()->CloseClientConnection( localName.c_str(),
				serverRemoteName.c_str() );
			return CR_WrongReceiverId;
		}

		KLSTD_TRACE1( 3, L"FileReceiverBridge::ConnectToMasterServer receiverId - '%ls'...OK\n", receiverId.c_str() );

		return CR_Ok;
	}
	
	FileOpeartionResult FileReceiverBridge::GetFileChunk( const std::wstring& receiverId,
			const std::wstring& fileName, unsigned long startFilePos, 
			void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize )
	{
		std::wstring localName, serverRemoteName, folderSyncName, wstrHostIdForReserveUpdates;
		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
		
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				KLFT_THROW_ERROR( ERR_WRONG_RECEIVER_ID );
			}

			ReceiverBridgeUnit &unit = fit->second;
			if ( !unit.m_bConnected )
			{
				KLFT_THROW_ERROR( ERR_WRONG_RECEIVER_ID );
			}

			localName = unit.m_localName;
			serverRemoteName = unit.m_serverConnName;
			if ( unit.m_bUseUAFolderSync )
			{
				folderSyncName = unit.m_folderSyncName;
				pFolderSync = unit.m_pFolderSync;
			}
			wstrHostIdForReserveUpdates = unit.m_wstrHostIdForReserveUpdates;
		}

		FileOpeartionResult retCode = FR_Ok;

		KLERR_TRY
		{
			KLSTD_TRACE3( 4, L"FileReceiverBridge::GetFileChunk fileName - '%ls' startPos - %d chunkBuffSize - %d\n", 
								fileName.c_str(), startFilePos, chunkBuffSize  );

			if ( pFolderSync )
			{
				bool fileNotExists = false;
				if ( pFolderSync->IsNeedToUpdateFile( fileName, fileNotExists ) )
				{
					((KLFT::FileTransferImp*)KLFT_GetFileTransfer())->
							AddSubfoldersForFolderSync( folderSyncName, fileName );

					if ( fileNotExists )
					{
						KLSTD_TRACE2( 4, L"FileReceiverBridge::GetFileChunk fileName - '%ls' retCode - %d file not exists\n", 
								fileName.c_str(), retCode );

						retCode = FR_FileNotFound;

						return retCode;
					}

					KLSTD_TRACE2( 4, L"FileReceiverBridge Try to upload file fileName - '%ls'\n", 
								fileName.c_str(), retCode );

					UploadSyncFolderFile( localName, serverRemoteName, pFolderSync,
						fileName, receiverId );
				}

				unsigned long fullFileSize;
				bool endOfFile = false;

				int bs = (chunkBuffSize>c_MaxChunkSize)?c_MaxChunkSize:chunkBuffSize;
				receivedChunkSize = pFolderSync->GetFolderFileChunk( fileName, startFilePos, 
					bs, fullFileSize,
					&chunkBuff, bs, endOfFile );

				if ( endOfFile ) retCode = FR_OkDownloaded;
			}
			else
			{
				KLTRAP::TransportProxy trProxy;		
				trProxy.Initialize( localName.c_str(), serverRemoteName.c_str() );
				
				KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
				
				struct soap* pSoap = proxyLocker.Get();
				
				struct klft_GetNextFileChunkResponse res;
				struct klft_GetNextFileChunkOptions  opts;
				soap_default_klft_GetNextFileChunkOptions( pSoap, &opts );
				opts.wstrNagentHostId = (wchar_t *)wstrHostIdForReserveUpdates.c_str();
				
				soap_call_klft_GetNextFileChunk( pSoap, NULL, NULL,
					(wchar_t *)localName.c_str(), (wchar_t *)fileName.c_str(),
					(wchar_t *)fileName.c_str(), startFilePos,  
					(chunkBuffSize>c_MaxChunkSize)?c_MaxChunkSize:chunkBuffSize, 
					true, false, 
					wstrHostIdForReserveUpdates.empty()?NULL:&opts,
					res );	
				
				KLSTD_TRACE2( 4, L"FileReceiverBridge::GetFileChunk pSoap->error - %d res.error.code - %d\n", 
					pSoap->error, res.error.code );
				
				proxyLocker.CheckResult();
				
				if ( res.error.code )
				{	
					KLERR::Error *pError=NULL;
					KLPAR::ExceptionFromSoap( res.error, &pError );				
					throw pError;
				}
				
				KLSTD_TRACE2( 4, L"FileReceiverBridge::GetFileChunk call result buffSize - %d fullFileSize - %d\n", 
					res.chunk.buff.__size, res.chunk.fullFileSize );
				
				memcpy( chunkBuff, res.chunk.buff.__ptr, res.chunk.buff.__size );
				receivedChunkSize = res.chunk.buff.__size;
				if ( (startFilePos+res.chunk.buff.__size)>=res.chunk.fullFileSize )
					retCode = FR_OkDownloaded;
			}
		}
		KLERR_CATCH( pError )
		{
			if ( retCode==FR_Ok ) retCode = FR_ErrorInOperation;
			
			KLERR_SAY_FAILURE( 2, pError );
			
			switch( pError->GetId() )
			{
			case KLTRAP::TRERR_TIMEOUT:
				retCode = FR_Timeout;
				break;
			case KLFT::ERR_FILE_NOT_FOUND:
			case KLFT::ERR_WRONG_FILE_ID:
				retCode = FR_FileNotFound;
				break;
			case KLFT::ERR_BAD_FUNCTION_PARAMETERS:
				retCode = FR_WrongArguments;
				break;
			case KLTRAP::TRERR_NOT_FOUND_CONNECTION:
			default:
				retCode = FR_ErrorInOperation;
			}
		}
		KLERR_ENDTRY	
			
		KLSTD_TRACE1( 4, L"FileReceiverBridge::GetFileChunk result retCode - %d\n", retCode );
		
		return retCode;
	}

	FileOpeartionResult FileReceiverBridge::GetFileInfo( const std::wstring& receiverId, const std::wstring &fileName, 
			FileInfo &fileInfo )
	{
		std::wstring localName, serverRemoteName, folderSyncName, wstrHostIdForReserveUpdates;;
		KLSTD::CAutoPtr<KLFT::FolderSync> pFolderSync;
		
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				return FR_WrongReceiverId;
			}

			ReceiverBridgeUnit &unit = fit->second;
			if ( !unit.m_bConnected )
			{
				return FR_WrongReceiverId;
			}

			localName = unit.m_localName;
			serverRemoteName = unit.m_serverConnName;
			if ( unit.m_bUseUAFolderSync )
			{
				folderSyncName = unit.m_folderSyncName;
				pFolderSync = unit.m_pFolderSync;
			}
			wstrHostIdForReserveUpdates = unit.m_wstrHostIdForReserveUpdates;
		}

		KLERR_TRY
		{
			if ( pFolderSync )
			{
				KLSTD_TRACE2( 3, L"FileReceiverBridge::GetFileInfo fileName - '%ls' folderSyncFileId - '%ls'\n", 
								fileName.c_str(), pFolderSync->GetFileId().c_str() );

				bool fileNotExists = false;
				if ( pFolderSync->IsNeedToUpdateFile( fileName, fileNotExists ) )
				{
					if ( fileNotExists )
					{
						((KLFT::FileTransferImp*)KLFT_GetFileTransfer())->
							AddSubfoldersForFolderSync( folderSyncName, fileName );

						KLSTD_TRACE1( 4, L"FileReceiverBridge::GetFileInfo fileName - '%ls' file not exists\n", 
								fileName.c_str() );

						return FR_FileNotFound;
					}
					
					UploadSyncFolderFile( localName, serverRemoteName, pFolderSync,
						fileName, receiverId );
				}

				pFolderSync->GetFolderFileInfo( fileName, fileInfo );
				
				return FR_Ok;
			}
			else
			{
				KLTRAP::TransportProxy trProxy;		
				trProxy.Initialize( localName.c_str(), serverRemoteName.c_str() );
				KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );

				struct soap* pSoap = proxyLocker.Get();

				struct klft_GetUpdaterFolderFileInfoResponse	res;
				struct klft_GetUpdaterFolderFileInfoOptions		opts;
				opts.wstrNagentHostId = (wchar_t *)wstrHostIdForReserveUpdates.c_str();
				
				soap_call_klft_GetUpdaterFolderFileInfo( pSoap, NULL, NULL,
					(wchar_t *)fileName.c_str(), 
					wstrHostIdForReserveUpdates.empty()?NULL:&opts,
					res );
				
				proxyLocker.CheckResult();
				
				if ( res.error.code )
				{
					KLERR::Error *pError=NULL;
					KLPAR::ExceptionFromSoap( res.error, &pError );
					throw pError;
				}				

				fileInfo.m_fileName = fileName;
				fileInfo.m_isDirectory = res.info.isDir;
				fileInfo.m_fileSize = (long)res.info.fullFileSize;
				fileInfo.m_createTime = (time_t)res.info.createTime;
				fileInfo.m_readPermissionOk = res.info.readPermission;
			}
		}
		KLERR_CATCH( pError )
		{
			KLERR_SAY_FAILURE( 2, pError );			
			if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
			{
				return FR_ErrorInOperation;
			}
			else
				return FR_FileNotFound;
		}
		KLERR_ENDTRY

		return FR_Ok;
	}

	void FileReceiverBridge::ReleaseReceiver( const std::wstring& receiverId )
	{
		KLSTD_TRACE1( 3, L"FileReceiverBridge::ReleaseReceiver receiverId - '%ls'\n", receiverId.c_str() );

		std::wstring localName, serverRemoteName;
		{
			KLSTD::AutoCriticalSection	unlocker( m_pCricSec );

			ReceiverBridgeUnitMap::iterator fit = m_ReceiversUnits.find( receiverId );
			if ( fit==m_ReceiversUnits.end() )
			{
				KLFT_THROW_ERROR( ERR_WRONG_RECEIVER_ID );
			}

			ReceiverBridgeUnit &unit = fit->second;
			

			if ( unit.m_bConnected && !unit.m_serverConnName.empty() )
			{
				localName = unit.m_localName;
				serverRemoteName = unit.m_serverConnName;
			}
		}

		if ( !serverRemoteName.empty() )
		{
			KLTR_GetTransport()->CloseClientConnection( localName.c_str(),
				serverRemoteName.c_str() );
		}

		if ( m_pContie )
		{
			KLERR_IGNORE(m_pContie->RemoveObject( receiverId ));
		}
	}

	void FileReceiverBridge::OnDisconnectObjectID( const std::wstring& wstrObject )
	{
		const std::wstring &receiverId = wstrObject;
		KLERR_IGNORE( ReleaseReceiver( receiverId ) );
	}

	bool FileReceiverBridge::CheckFolderSyncUpdateAgent( const std::wstring &folderSyncName )
	{
		return ((KLFT::FileTransferImp*)KLFT_GetFileTransfer())->CanUseUpdateAgent();
	}

	ConnectResult FileReceiverBridge::ConnectToFolderSyncUpdateAgent( 
		const std::wstring &folderSyncName,
		KLSTD::CAutoPtr<KLFT::FolderSync> &pFolderSync,
		std::wstring &localName, std::wstring &remoteName )
	{
		bool res = ((KLFT::FileTransferImp*)KLFT_GetFileTransfer())->ConnectToFolderSyncUpdateAgent(
			folderSyncName,
			pFolderSync,
			localName, remoteName );

		return res?KLFT::CR_Ok:KLFT::CR_ConnectionError;
	}

	void FileReceiverBridge::UploadSyncFolderFile( const std::wstring &localName, 
			const std::wstring &remoteName, KLFT::FolderSync *pFolderSync,
			const std::wstring &fileName, 
			const std::wstring &receiverId )
	{
		std::wstring tempUploadFileName = pFolderSync->GetUploadingTemporyFileName( receiverId );
		
		KLSTD_TRACE4( 3, L"FileReceiverBridge::UploadSyncFolderFile fileName - '%ls' localName - '%ls' remoteName - '%ls' folderSyncId - '%ls'\n", 
					fileName.c_str(), localName.c_str(), remoteName.c_str(), pFolderSync->GetFileId().c_str() );

		KLSTD::CAutoPtr<KLSTD::File> pFile;
		KLSTD_CreateFile( tempUploadFileName, KLSTD::SF_READ | KLSTD::SF_WRITE,
					KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_READ | KLSTD::AF_WRITE, 0,
					&pFile );
		
		KLTRAP::TransportProxy trProxy;		
		trProxy.Initialize( localName.c_str(), remoteName.c_str() );
		
		int neededSize = c_MaxChunkSize;		
		unsigned long startFilePos = 0;
		bool endOfFile = false;

		while( !endOfFile )
		{				
			KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
			
			struct soap* pSoap = proxyLocker.Get();
					
			struct klft_GetSyncFolderFileChunkResponse res;

			soap_call_klft_GetSyncFolderFileChunk( pSoap, NULL, NULL,
						(wchar_t*)pFolderSync->GetFileId().c_str(),
						(wchar_t*)fileName.c_str(),
						startFilePos, neededSize,
						res );
			
			proxyLocker.CheckResult();
			
			if ( res.error.code )
			{	
				KLSTD_TRACE2( 3, L"FileReceiverBridge::UploadSyncFolderFile pSoap->error - %d res.error.code - %d\n", 
					pSoap->error, res.error.code );

				KLERR::Error *pError=NULL;
				KLPAR::ExceptionFromSoap( res.error, &pError );				
				throw pError;
			}

				
			KLSTD_TRACE2( 3, L"FileReceiverBridge::UploadSyncFolderFile call result buffSize - %d fullFileSize - %d\n", 
				res.chunk.buff.__size, res.chunk.fullFileSize );

			pFile->Write( res.chunk.buff.__ptr, res.chunk.buff.__size );

			startFilePos += res.chunk.buff.__size;
			if ( startFilePos>=res.chunk.fullFileSize ) endOfFile = true;
		}

		pFile = NULL;
		KLSTD_TRACE2( 3, L"FileReceiverBridge::UploadSyncFolderFile UpdateFile fileName - '%ls' tempName - '%ls'\n", 
					fileName.c_str(), tempUploadFileName.c_str() );

		pFolderSync->UpdateFile( tempUploadFileName, fileName );

		KLSTD_Unlink( tempUploadFileName.c_str(), false );
	}

	
} // end namespace KLFT

IMPLEMENT_MODULE_INIT_DEINIT(KLFTBRIDGE)

//\brief функция инициализация библиотеки.
IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL,KLFTBRIDGE)

	if ( !KLSTD_StGetPrefixW().empty() ) return;

	if ( KLFT::g_FileReciverBridge!=NULL ) return;

	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLFT::c_errorDescriptions,
		KLSTD_COUNTOF(KLFT::c_errorDescriptions));

	KLFT::FileReceiverBridge *pFt = new KLFT::FileReceiverBridge();
	KLSTD_CHKMEM( pFt );
	KLFT::g_FileReciverBridge.Attach( pFt );
	KLSTD_CreateSemaphore( &KLFT::g_FileReciverBridgeDestroySem, 0 );

	KLFT::g_FileReciverBridge->Init();

	KLFTBRIDGE::RegisterGSOAPStubFunctions();	

IMPLEMENT_MODULE_INIT_END()

/*\brief Функция деинициализация библиотеки.*/
IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL,KLFTBRIDGE)

	if ( KLFT::g_FileReciverBridge==NULL ) return;

	KLFTBRIDGE::UnregisterGSOAPStubFunctions();
	
	KLSTD::CAutoPtr<KLFT::FileReceiverBridge> saveFt = KLFT::g_FileReciverBridge;	
	KLFT::g_FileReciverBridge = NULL;	
	saveFt.Release();
	KLFT::g_FileReciverBridgeDestroySem->Wait();
	KLFT::g_FileReciverBridgeDestroySem = NULL;

	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);

IMPLEMENT_MODULE_DEINIT_END() 

/*\brief Возвращает указатель на глобальный объект класса FileTransfer */
KLCSKCA_DECL KLFT::FileReceiverBridgeBase *KLFT_GetFileReceiverBridge()
{
	if ( ((KLFT::FileReceiverBridge *)KLFT::g_FileReciverBridge)==NULL )
		KLFT_THROW_ERROR( ERR_FILE_TRANSFER_NOT_INIT );

	return KLFT::g_FileReciverBridge;
}
