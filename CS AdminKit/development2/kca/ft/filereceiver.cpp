/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file FileReceiver.cpp
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Реализация класса для закачки файлов
 * 
 */
#include <std/base/klbase.h>
#include <std/thr/sync.h>
#include <std/err/klerrors.h>

#include <std/par/paramsi.h>
#include <std/par/helpers.h>

#include <transport/tr/errors.h>
#include <transport/tr/transportproxy.h>

#include <kca/prss/settingsstorage.h>
#include <nagent/naginst/nagent_const.h>
#include <nagent/naginst/cert_load.h>
#include <kca/prcp/proxybase.h>

#include <kca/srvhrch/srvhrch_common.h>

#include <server/srvinst/server_constants.h>

#include <kca/ft/filetransfer.h>
#include <kca/ft/errors.h>

#include <transport/tr/transport.h>
#include <transport/tr/errors.h>
#include <transport/tr/common.h>

#include "filereceiver.h"
#include "filetransferimp.h"

DECLARE_MODULE_INIT_DEINIT2(KLCSKCA_DECL,KLFT);

#define KLCS_MODULENAME L"KLFT"

namespace KLFT {
	
	class FileReceiverImp : public KLSTD::KLBaseImpl<FileReceiver> 
	{	
	public:

		virtual FileReceiverImp::~FileReceiverImp()
		{
			CALL_MODULE_DEINIT(KLFT);
		}

		//!\brief Функция производит подключение к серверу администрирования
		virtual ConnectResult Connect()
		{
			CALL_MODULE_INIT(KLFT);
			
			KLERR_TRY
			{
				KLPRCI::ComponentId idLocal(
					KLCS_PRODUCT_ADMSERVER,
					KLCS_VERSION_ADMSERVER,
					c_FileTransferComponentName,
					KLSTD_CreateLocallyUniqueString());
				KLTRAP::ConvertComponentIdToTransportName( m_localName, idLocal );

				KLFT::ConnectToServer( m_localName, m_serverConnName );
			}
			KLERR_CATCH( pError )
			{
				m_serverConnName.clear();// = L"";
				KLERR_SAY_FAILURE( 2, pError );
				if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
				{
					if ( pError->GetId()==KLTRAP::TRERR_TIMEOUT || 
						pError->GetId()==KLTRAP::TRERR_REMOTE_SERVER_IS_BUSY )
					{
						return CR_ServerBusy;
					}					
				}
				
				CALL_MODULE_DEINIT(KLFT);
				
				return CR_ConnectionError;
			}
			KLERR_ENDTRY	

			return CR_Ok;
		}

		//\brief Функция производит подключение от подчиненного сервера к главному
				// серверу администрирования
		virtual ConnectResult ConnectToMasterServer()
		{
			CALL_MODULE_INIT(KLFT);
			
			KLERR_TRY
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
					return CR_ConnectionError;
				}

				KLPRCI::ComponentId idLocal(
					KLCS_PRODUCT_ADMSERVER,
					KLCS_VERSION_ADMSERVER,
					c_FileTransferComponentName,
					KLSTD_CreateLocallyUniqueString());				

				std::wstring wstrRemoteName = idMasterSrv.PutToString();
				std::wstring wstrLocalName = idLocal.PutToString();				
				
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
					
				for( std::wstring wstrAddress; !bAdded && addresses.GetNext(wstrAddress); )
				{
					KLTRAP::Transport::ConnectionInfo connInf;
					connInf.localName = wstrLocalName;			//!< Локальное имя компоненты
					connInf.remoteName = wstrRemoteName;		//!< Удаленное имя компоненты
					connInf.remoteLocation = wstrAddress;		//!< Адрес удаленной компоненты
					connInf.pCustomAuthClientCtx = NULL;	//!< Сохраняется локально для данного соединения
					connInf.pRemotePublicSSLKey = pMasterSrvCert;	//!< Публичный ключ SSL удаленной компоненты
					connInf.bCreateSSLConnection = true;
					connInf.pLocalPublicSSLKey = pCert;		//!< Публичный ключ SSL локальной компоненты компоненты 
					connInf.bConnForGateway = false;		//!< Флаг использования данного соединения для gateway
					connInf.bMakeTwoWayAuth = false;			//!< Для данного соединения будет проводиться двухстороная аутонтификация
					if( !proxyHost.location.empty() )
					{
						connInf.pProxyInfo = &proxyHost;	//!< Информация для создания соединения через прокси сервер
					}
					
					KLSTD_TRACE1(4, L"AddSSLClientConnection('%ls')\n",
						wstrAddress.c_str() );
					
					bAdded = KLTR_GetTransport()->AddClientConnection2(connInf);
					KLSTD_TRACE1(4, L"AddSSLClientConnection returned %u\n", bAdded);

					m_localName = wstrLocalName;
					m_serverConnName = wstrRemoteName; 
				}
    
				if(!bAdded)
				{
					KLSTD_TRACE1(3, L"Failed to add connection to server '%ls'\n", wstrRemoteName.c_str());					
					return CR_ConnectionError;
				}
			}
			KLERR_CATCH( pError )
			{
				m_serverConnName.clear();// = L"";
				KLERR_SAY_FAILURE( 2, pError );
				if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
				{
					if ( pError->GetId()==KLTRAP::TRERR_TIMEOUT || 
						pError->GetId()==KLTRAP::TRERR_REMOTE_SERVER_IS_BUSY )
					{
						return CR_ServerBusy;
					}					
				}
				
				CALL_MODULE_DEINIT(KLFT);
				
				return CR_ConnectionError;
			}
			KLERR_ENDTRY
				
			return CR_Ok;
		}

		/*!
			\brief Функция назначает соединения для использования
		*/
		virtual void SetConnectParam( const std::wstring &localConnName,
			const std::wstring &remoteConnName )
		{
			CALL_MODULE_INIT(KLFT);
			
			m_localName = localConnName;
			m_serverConnName = remoteConnName;
		}

		//!\brief Возвращает информацию о файле
		virtual bool GetFileInfo( const std::wstring &fileName,
			FileInfo &fileInfo )
		{
			bool retCode = true;
			KLERR_TRY
			{
				KLTRAP::TransportProxy trProxy;		
				trProxy.Initialize( m_localName.c_str(), m_serverConnName.c_str() );
				KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );

				struct soap* pSoap = proxyLocker.Get();

				struct klft_GetUpdaterFolderFileInfoResponse res;
				
				soap_call_klft_GetUpdaterFolderFileInfo( pSoap, NULL, NULL,
					(wchar_t *)fileName.c_str(), NULL, res );
				
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
			KLERR_CATCH( pError )
			{
				KLERR_SAY_FAILURE( 2, pError );
				retCode = false;
			}
			KLERR_ENDTRY	

			return retCode;
		}

		//!\brief Закачивает с сервера порцию файла
		virtual FileChunkResult GetFileChunk( const std::wstring &fileName,
			long startPos, void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize )
		{
			FileChunkResult retCode = FC_Ok;
			receivedChunkSize = 0;
			KLERR_TRY
			{
				KLSTD_TRACE3( 3, L"GetFileChunk fileName - '%ls' startPos - %d chunkBuffSize - %d\n", 
					fileName.c_str(), startPos, chunkBuffSize );
				
				KLTRAP::TransportProxy trProxy;		
				trProxy.Initialize( m_localName.c_str(), m_serverConnName.c_str() );
						
				KLTRAP::TransportConnectionLocker proxyLocker( &trProxy );
					
				struct soap* pSoap = proxyLocker.Get();
						
				struct klft_GetNextFileChunkResponse res;
						
				soap_call_klft_GetNextFileChunk( pSoap, NULL, NULL,
					(wchar_t *)m_localName.c_str(), (wchar_t *)fileName.c_str(),
					(wchar_t *)fileName.c_str(), startPos,  
					(chunkBuffSize>c_MaxChunkSize)?c_MaxChunkSize:chunkBuffSize, 
					true, false, NULL,
					res );	
				
				KLSTD_TRACE2( 3, L"GetFileChunk pSoap->error - %d res.error.code - %d\n", 
						pSoap->error, res.error.code );
						
				proxyLocker.CheckResult();
						
				if ( res.error.code )
				{	
					KLERR::Error *pError=NULL;
					KLPAR::ExceptionFromSoap( res.error, &pError );							
					if ( std::wstring(pError->GetModuleName())==KLFT::KLFT_ModuleName )
					{
						switch( pError->GetId() )
						{
						case KLFT::ERR_FILE_NOT_FOUND:
						case KLFT::ERR_WRONG_FILE_ID:
							retCode = FC_FileNotFound;
							break;
						case KLFT::ERR_BAD_FUNCTION_PARAMETERS:
							retCode = FC_WrongArguments;
							break;						
						default:
							retCode = FC_ErrorInOperation;
						}
					}
					throw pError;
				}

				KLSTD_TRACE2( 3, L"GetFileChunk call result buffSize - %d fullFileSize - %d\n", 
						res.chunk.buff.__size, res.chunk.fullFileSize );
				
				memcpy( chunkBuff, res.chunk.buff.__ptr, res.chunk.buff.__size );
				receivedChunkSize = res.chunk.buff.__size;
				if ( (startPos+res.chunk.buff.__size)>=res.chunk.fullFileSize )
					retCode = FC_OkDownloaded;



				
/*KLERR_BEGIN

std::wstring wtsrDir, wtsrName, wstrExt;
KLSTD_SplitPath( fileName, wtsrDir, wtsrName, wstrExt );
std::wstring testFName = std::wstring(L"c:\\");
testFName += wtsrName;
testFName += L"tmp";

KLSTD::CAutoPtr<KLSTD::File>				testFile;
KLSTD_CreateFile( testFName.c_str(), KLSTD::SF_READ | KLSTD::SF_WRITE,
				 KLSTD::CF_OPEN_ALWAYS, KLSTD::AF_READ | KLSTD::AF_WRITE, 0,
				&testFile, 10000 );
testFile->Write( chunkBuff, receivedChunkSize );

KLERR_END*/
			}
			KLERR_CATCH( pError )
			{
				if ( retCode==FC_Ok ) retCode = FC_ErrorInOperation;

				KLERR_SAY_FAILURE( 2, pError );
				if ( pError->GetModuleName()==KLTRAP::KLTRAP_ModuleName )
				{
					switch( pError->GetId() )
					{
					case KLTRAP::TRERR_TIMEOUT:
						retCode = FC_Timeout;
						break;
					case KLTRAP::TRERR_NOT_FOUND_CONNECTION:
					default:
						retCode = FC_ErrorInOperation;
					}
				}
			}
			KLERR_ENDTRY	

			KLSTD_TRACE1( 3, L"GetFileChunk result retCode - %d\n", retCode );
				
			return retCode;
		}

		//!\brief Стартует операции по закачке опеределенного файла
		virtual bool StartFileReceiving( const std::wstring &fileName )
		{
			m_currentFileName = fileName;
			m_currentFilePos = 0;
			return true;
		}

		//!\brief Закачивает с сервера порцию текущего файла 
		virtual FileChunkResult GetCurrentFileChunk( void *chunkBuff, int chunkBuffSize,  
			int &receivedChunkSize )
		{
			if ( m_currentFileName.empty() ) return FC_WrongArguments;
			
			FileChunkResult retCode = GetFileChunk( m_currentFileName,
					m_currentFilePos, chunkBuff, chunkBuffSize,  receivedChunkSize );
			if ( receivedChunkSize>0 ) m_currentFilePos += receivedChunkSize;

			return retCode;
		}

		//!\brief Разрывает соединение с сервером
		virtual void Disconnect()
		{
			if ( !m_serverConnName.empty() && !m_localName.empty() )
			{
				KLTR_GetTransport()->CloseClientConnection( m_localName.c_str(),
					m_serverConnName.c_str() );				
			}			

			CALL_MODULE_DEINIT(KLFT);
		}

	private:

		std::wstring	m_serverConnName;
		std::wstring	m_localName;

		std::wstring	m_currentFileName;
		unsigned long	m_currentFilePos;
	};

} // end namespace KLFT


/*\brief Создает объект класса FileReceiver */
KLCSNAGT_DECL void KLFT_CreateFileReceiver( KLFT::FileReceiver **ppFileReceiver )
{
	KLSTD_CHKOUTPTR(ppFileReceiver);
	*ppFileReceiver = new KLFT::FileReceiverImp();
	KLSTD_CHKMEM(*ppFileReceiver);
}

