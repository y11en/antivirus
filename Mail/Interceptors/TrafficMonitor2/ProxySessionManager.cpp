// ProxySessionManager.cpp: implementation of the CProxySessionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProxySessionManager.h"
#include "util.h"

#define SESSION_LIST_REFRESH_TIMEOUT	20000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static tERROR pr_call fMsgReceive( hOBJECT _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

CProxySessionManager::CProxySessionManager(hOBJECT hParent /* = NULL  */) :
	m_hMsgReceiver(NULL),
	m_sync(NULL),
    m_ListenPort( 0 ),
    m_hParent( hParent ),
	m_ThreadHandle( NULL )
{
	m_hRequestSessionStop   = CreateEvent(NULL,FALSE,FALSE,NULL);	
    m_hStopEvent            = CreateEvent(NULL, TRUE, FALSE, NULL );

    InitializeCriticalSection( &m_WaitSessionListLock );
    m_ClientsSemaphore = CreateSemaphore( NULL, 0, 1000, NULL );

    m_SessionCount = 0;
}

CProxySessionManager::~CProxySessionManager()
{
	if ( m_hMsgReceiver )
	{
		m_hMsgReceiver->sysUnregisterMsgHandler( pmcTRAFFIC_PROTOCOLLER, m_hParent );
		m_hMsgReceiver->sysCloseObject();
		m_hMsgReceiver = NULL;
	}
	
	CloseHandle(m_hRequestSessionStop);    

    DeleteCriticalSection( &m_WaitSessionListLock );
    CloseHandle( m_ClientsSemaphore );    
    CloseHandle( m_hStopEvent );
	if(m_ThreadHandle)
		CloseHandle(m_ThreadHandle);
}

void
CProxySessionManager::FinishActiveSessions()
{
    // Останавливаем запущенные сессии. 
    // Нужно порвать соединения, дождаться выхода из потоков и удалить их
    CProxySession* Session;
    std::list<CProxySession*>::iterator ii;    

    KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::ProxyManagerThread => Stopping running sessions...");	

    for ( ii = m_SessionList.begin(); ii != m_SessionList.end(); ii++ )
    {   
        if ( (Session = *ii) != NULL )
        {            
            Session->BreakKavsvcConnect();
            Session->WaitExitThread();
            Session->StopSession();
            delete Session;
        }
    }

    m_SessionList.clear();
}

tERROR CProxySessionManager::ProxyManagerThread()
{
    SOCKET server, server_v6, CrService, CrService_v6;

	cERROR err = errOK;

	KLSTD_TRACE1 (KLMC_TRACELEVEL, "CProxySessionManager::ProxyManagerThread => begin on port %d...", m_ListenPort );

	server = ServerSocket( m_ListenPort, PF_INET  );
    server_v6 = ServerSocket( m_ListenPort, PF_INET6  );

    CrService = ServerSocket( m_ServPort, PF_INET  );
    CrService_v6 = ServerSocket( m_ServPort, PF_INET6  );

    KLSTD_TRACE4 (KLMC_TRACELEVEL, "CProxySessionManager::ProxyManagerThread => Server(0x%x) Server6(0x%x) Service(0x%x) Service6(0x%x)", 
        server, server_v6, CrService, CrService_v6);

    ProxyMain( server, server_v6, CrService, CrService_v6 );

	FinishActiveSessions();

    // Вообще лочить имхо необязательно, т.к. к данному моменту все клиенты 
    // избавились от своих потоков, т.е. не может быть одновременного доступа к списку 
    // m_WaitSessionList. 
    // т.е. просто "на всякий случай" ...
    EnterCriticalSection( &m_WaitSessionListLock );
    m_WaitSessionList.clear();
    LeaveCriticalSection( &m_WaitSessionListLock );

    /*
    CLOSESOCKET( server );
	CLOSESOCKET( server_v6 );
	CLOSESOCKET( CrService );
	CLOSESOCKET( CrService_v6 );
    */

	KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::ProxyManagerThread => finish...");
	return err;
}

void
CProxySessionManager::ProcessPseudoStop()
{
    while ( true )
    {
        hOBJECT hStoppingProtocoller;  
        
        {
            cAutoCS _lock(m_sync, true);
            if(!m_hStoppingProtocollers.size())
                break;
            hStoppingProtocoller = m_hStoppingProtocollers[0];
            m_hStoppingProtocollers.remove(0);
        }

        std::list<CProxySession*>::iterator ii;
        for ( ii = m_SessionList.begin(); ii != m_SessionList.end(); ii++ )
        {
            CProxySession* pSession = *ii;

            if ( pSession->PseudoStop(hStoppingProtocoller) == errOK_DECIDED )
            {
                break;
            }
            
        }
    }
}

CProxySession*
CProxySessionManager::FindSessionByID(__int64& ID )
{   
    std::list<CProxySession*>::iterator ii;

    for ( ii = m_SessionList.begin(); ii != m_SessionList.end(); ii++ )
    {
        if ( (*ii)->m_ID == ID )
        {
            return *ii;            
        }
    }

    return NULL;
}

void
CProxySessionManager::ProcessNewConnection( SOCKET Server )
{
    CProxySession* pSession = NULL;
    SOCKET Client;
    
    // SOCKADDR_STORAGE from;
    // на Vista изменили размер SOCKADDR_STORAGE. теперь 128 байт.
    // на всякий случай делаю 256 ( вдруг еще раз изменят !? )
    // P.S. pigs !!!
    union _MY_SOCKADDR_STORAGE {
        SOCKADDR_STORAGE ss;
        char reserved[256];
    } from;

    int fromlen = sizeof ( from );

    char    DrvBuffer[512];
    int     RetVal;

    Client = accept(Server, (struct sockaddr *)&from, &fromlen);

    if ( Client == INVALID_SOCKET )
    {
        KLSTD_TRACE1( KLMC_TRACELEVEL, "ProcessNewConnection: accept error = %d", WSAGetLastError() );        
        return;    
    }
    
    KLSTD_TRACE0( KLMC_TRACELEVEL, "ProcessNewConnection: connection accepted. create Session" );

    SetBlocking( Client, true );

    RetVal = recv ( Client, DrvBuffer, 500, 0 );

	char Prefix[16];
	memset( Prefix, '.', sizeof(Prefix) );
    memcpy( Prefix, DrvBuffer, 10 );
    Prefix[15] = 0;

    KLSTD_TRACE3 (KLMC_TRACELEVEL, "CProxySessionManager::Run => recv %d bytes from server [%s]. err = %d", 
        RetVal, GetBinBufferTraceStringShort((const BYTE *)Prefix, sizeof(Prefix)).c_str(), WSAGetLastError() );

	if (RetVal == 0 || RetVal == INVALID_SOCKET)
	{
		// CLOSESOCKET(Client);
        closesocket( Client );
		return;
	}
    
    // обычный редирект через KAVSEND. Оставлено на всяких случай, если не смогли переключиться на новую схему.
    if ( 0 == memcmp( DrvBuffer, KAVSEND_COMMAND, strlen( KAVSEND_COMMAND ) ) ) 
    {   
        KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::Run => KAVSEND" );

        pSession = new CProxySession(m_hParent, this, m_hStopEvent );

        if ( pSession )
        {
            KLSTD_TRACE1 (KLMC_TRACELEVEL, "CProxySessionManager::Run => KAVSEND create session(%p)", pSession );
            // тут клиентский сокет
            pSession->m_Flags |= FPS_GOT_KAVSEND;
            pSession->m_AcceptedSock = Client;
            pSession->ParseKavsendBuffer( DrvBuffer, RetVal );

            pSession->StartProtocollers();

            m_SessionList.push_back(pSession);  //Добавим в список запущенную сессию            
            if ( !pSession->StartClient() )
            {
                KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::Run => Unable to start thread after KAVSEND");
                AddClient( pSession );
            }
        }
        return;
    }

    // редирект по новой схеме KAVSVC
    if ( 0 == memcmp( DrvBuffer, KAVSVC_COMMAND, strlen( KAVSVC_COMMAND ) ))
    {
        // сервисное соединение. Тут должны указываться параметры сервера,
        // куда выполняем соединение. Прокси должна соединиться туда и 
        // создать ID сессии.
        pSession = new CProxySession(m_hParent, this, m_hStopEvent );

        KLSTD_TRACE1 (KLMC_TRACELEVEL, "CProxySessionManager::Run => KAVSVC. session = %p", pSession );

        if ( pSession )
        {
            pSession->m_AcceptedSock = Client; 
            pSession->m_Flags |= FPS_GOT_KAVSVC;
            pSession->ParseKavsvcBuffer( DrvBuffer, RetVal );
            
            m_SessionList.push_back(pSession); //Добавим в список запущенную сессию            
            if ( !pSession->StartClient() )
            {
                KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::Run => Unable to start thread after KAVSVC");
                AddClient( pSession );
            }
        }

        return;
    }

    // продолжение редиректа по новой схеме. Мы законнектились на сервер.
    // Теперь нужно найти сессию с установленным серверным соединением по указанному ID
    // !! TODO : можно избежать лишнего запуска потока. для этого нужно засинхронизировать
    //           уже работающий поток с серверным соединением с текущим потоком.
    if ( 0 == memcmp( DrvBuffer, KAVCONN_ID, strlen( KAVCONN_ID ) ) )
    {
        __int64 ID = *(__int64*) ( (PCHAR)DrvBuffer + 10 );

        KLSTD_TRACE1 (KLMC_TRACELEVEL, "CProxySessionManager::Run => KAVCONN_ID 0x%I64x", ID );

        if ( pSession  = FindSessionByID( ID ) )
        {
            KLSTD_TRACE1 (KLMC_TRACELEVEL, "CProxySessionManager::Run => Session %p found by KAVCONN_ID", pSession );
            // тут клиентский сокет
            
            pSession->m_client.AssignSocket( Client );
            pSession->m_client.PrepareSocket();

            GetLocalName( pSession->m_client.Socket(), pSession->m_Localhost, sizeof ( pSession->m_Localhost ) );

            pSession->m_Flags |= FPS_GOT_KAVCONN_ID | FPS_ESTABLISHED | FPS_CLIENT_CONNECTED;
            
            pSession->StartProtocollers();

            // запускаем поток сессии
            if ( !pSession->StartClient() )
            {
                KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::Run => Unable to start thread after KAVCONN_ID");
                AddClient( pSession );
            }
        }
        else
        {
            KLSTD_TRACE0 (KLMC_TRACELEVEL, "CProxySessionManager::Run => Session NOT found by KAVCONN_ID" );
        }

        return;
    }
}

tERROR
CProxySessionManager::ProxyMain(SOCKET Server, SOCKET Server_v6, SOCKET CrService, SOCKET CrService_v6 )
{
    int RetVal = 0;
    DWORD dwCount;    
    CProxySession* Session;
    fd_set fds;
    TIMEVAL Timeout = { 1, 0 }; // 1 second Timeout

    SOCKET MySock = INVALID_SOCKET;

    while(true)
    {
        FD_ZERO(&fds);
        
        if ( Server != INVALID_SOCKET )
            FD_SET(Server, &fds);
        if ( Server_v6 != INVALID_SOCKET )
            FD_SET(Server_v6, &fds);
        if ( CrService != INVALID_SOCKET )
            FD_SET(CrService, &fds);
        if ( CrService_v6 != INVALID_SOCKET )
            FD_SET(CrService_v6, &fds);        

        // Получаем набор дескрипторов, у которых мы ожидаем какой-либо активности.
        dwCount = GetDescriptors( &fds );

        CKAHCR::SetWatchdogTimeout( ( SESSION_LIST_REFRESH_TIMEOUT / 1000 ) * 2);
        
        // выгрызаем из набора дескриптор с событием.
        RetVal = select( 0, &fds, NULL, NULL, &Timeout );

        // KLSTD_TRACE1( KLMC_TRACELEVEL, "CProxySessionManager::ProxyMain : select return %d\n", RetVal );

        CKAHCR::SetWatchdogTimeout( ( SESSION_LIST_REFRESH_TIMEOUT / 1000 ) * 2);

        // проверка на выгрузку
        if ( WAIT_OBJECT_0 == WaitForSingleObject( m_hStopEvent, 0 ))
        {
            // global stop event
            KLSTD_TRACE0( KLMC_TRACELEVEL, "CProxySessionManager::ProxyMain => Received m_hStopEvent\n" );
            break;
        }

        if ( WAIT_OBJECT_0 == WaitForSingleObject( m_hRequestSessionStop, 0 ))
        {
            KLSTD_TRACE0( KLMC_TRACELEVEL, "CProxySessionManager::ProxyMain => Received m_hRequestSessionStop\n" );

            ProcessPseudoStop();
            continue;
        }

        if ( RetVal == SOCKET_ERROR )
        {
            KLSTD_TRACE2( KLMC_TRACELEVEL, "CProxySessionManager::ProxyMain => select returned %d. err = %d\n", RetVal, WSAGetLastError() );

            break;
        }

        // обработка новых подключений к слушающим сокетам
        if ( FD_ISSET(Server, &fds) )
        {
            FD_CLR( Server, &fds );
            ProcessNewConnection( Server );
            continue;
        }

        if ( FD_ISSET(Server_v6, &fds) )
        {
            FD_CLR( Server_v6, &fds );
            ProcessNewConnection( Server_v6 );
            continue;
        }

        if ( FD_ISSET(CrService, &fds) )
        {
            FD_CLR( CrService, &fds );
            ProcessNewConnection( CrService );
            continue;
        }

        if ( FD_ISSET(CrService_v6, &fds) )
        {
            FD_CLR( CrService_v6, &fds );
            ProcessNewConnection( CrService_v6 );
            continue;
        }
        
        // обработка "спящих" клиентов.
        Session = GetClient( &fds );

        if ( Session )
        {            
            if ( !Session->StartClient() )
            {
                KLSTD_TRACE1( KLMC_TRACELEVEL, "CProxySessionManager::ProxyMain: Unable To restart Client %x \n", Session );
                Sleep( 1000 );
                AddClient( Session );
            }
        }
        else
        {
            // timeout 
            m_SessionList.erase( remove_if( m_SessionList.begin(), m_SessionList.end(),CDoneSession()), m_SessionList.end());
        }
    }

    if ( Server != INVALID_SOCKET )
        closesocket( Server );

    if ( Server_v6 != INVALID_SOCKET )
        closesocket( Server_v6 );

    if ( CrService != INVALID_SOCKET )
        closesocket( CrService );

    if ( CrService_v6 != INVALID_SOCKET )
        closesocket( CrService_v6 );

    return errOK;
}

DWORD
CProxySessionManager::GetDescriptors(fd_set* set )
{
    u_int Count = 0;
    std::list<CProxySession*>::iterator ii;
    u_int SrvSockCount = set->fd_count;


    EnterCriticalSection( &m_WaitSessionListLock );

    for ( ii = m_WaitSessionList.begin(); ii != m_WaitSessionList.end() && Count < FD_SETSIZE - SrvSockCount; Count++, ii++ )
    {
        CProxySession* Session = *ii;

        if ( Session->m_server.is_alive() )
            FD_SET( Session->m_server.Socket(), set );
        if ( Session->m_client.is_alive() )
            FD_SET( Session->m_client.Socket(), set );
    }

    if ( Count >= FD_SETSIZE - SrvSockCount )
    {
        m_WaitSessionList.splice( m_WaitSessionList.end(), m_WaitSessionList, m_WaitSessionList.begin(), ii );
    }

    LeaveCriticalSection( &m_WaitSessionListLock );

    return Count;
}

// получает клиента из списка. клиент при этом из списка удаляется
CProxySession*
CProxySessionManager::GetClient( fd_set* set )
{
    CProxySession* Session = NULL;

    EnterCriticalSection( &m_WaitSessionListLock );

    std::list<CProxySession*>::iterator ii;

    for ( ii = m_WaitSessionList.begin(); ii != m_WaitSessionList.end(); ii++ )
    {
        Session = *ii;
        
        if ( ( Session->m_client.is_alive() && FD_ISSET( Session->m_client.Socket(), set ) ) || 
             ( Session->m_server.is_alive() && FD_ISSET( Session->m_server.Socket(), set ) ) )
        {
            if ( Session->m_client.is_alive() )
                FD_CLR( Session->m_client.Socket(), set );            
            if ( Session->m_server.is_alive() )
                FD_CLR( Session->m_server.Socket(), set );

            m_WaitSessionList.erase( ii );
            Session->m_bInWaitSessionList = false;

            LeaveCriticalSection( &m_WaitSessionListLock );
            return Session;
        }
    }

    LeaveCriticalSection( &m_WaitSessionListLock );

    return NULL;
}

// Добавляет клиента в список.
void
CProxySessionManager::AddClient(CProxySession* Client )
{
    LONG dwPrevCount;

    EnterCriticalSection( &m_WaitSessionListLock );    

    if ( (Client->m_Flags & FPS_FINISH) == 0 )
    {
        m_WaitSessionList.push_back( Client );
        Client->m_bInWaitSessionList = true;
    }

    LeaveCriticalSection( &m_WaitSessionListLock );

    ReleaseSemaphore( m_ClientsSemaphore, 1, &dwPrevCount );
}

tERROR CProxySessionManager::Init()
{
    tERROR  err;

	if (!m_hRequestSessionStop)
		return errNOT_ENOUGH_MEMORY;
			
    err = m_hParent->sysCreateObject( (hOBJECT*)&m_hMsgReceiver, IID_MSG_RECEIVER );
    if ( PR_SUCC(err) ) 
        err = m_hMsgReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)fMsgReceive);
    if ( PR_SUCC(err) ) 
        err = m_hMsgReceiver->set_pgRECEIVE_CLIENT_ID((tPTR)this);
    if ( PR_SUCC(err) ) 
        err = m_hMsgReceiver->sysCreateObjectDone();
    if ( PR_SUCC(err) ) 
    {
        err = m_hMsgReceiver->sysRegisterMsgHandler(
            pmcTRAFFIC_PROTOCOLLER, 
            rmhDECIDER, 
            m_hParent->sysGetParent(IID_TASKMANAGER), 
            IID_ANY, PID_ANY, IID_ANY, PID_ANY 
            );
    }

    if ( PR_SUCC(err) ) 
        err = m_hMsgReceiver->sysCreateObjectQuick((hOBJECT*)&m_sync, IID_CRITICAL_SECTION);
    if ( PR_SUCC(err)) 
        m_bInited = cTRUE;

	return err;
}

tERROR pr_call fMsgReceive( 
						   hOBJECT _this, 
						   tDWORD p_msg_cls_id, 
						   tDWORD p_msg_id, 
						   hOBJECT p_send_point, 
						   hOBJECT p_ctx, 
						   hOBJECT p_receive_point, 
						   tVOID* p_par_buf, 
						   tDWORD* p_par_buf_len 
						   )
{
	cERROR err = errOK;

    if ( p_msg_cls_id == pmcTRAFFIC_PROTOCOLLER )
    {
        //////////////////////////////////////////////////////////////////////////
        //
        // Остановка по требованию
        //
        if ( p_msg_id == pmSESSION_PSEUDO_STOP_REQUEST )
        {
            KLSTD_TRACE0 (KLMC_TRACELEVEL, "ProxySessionManager pmSESSION_PSEUDO_STOP_REQUEST\n" );

            CProxySessionManager* pSessionManager = (CProxySessionManager*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
            {
                cAutoCS _lock(pSessionManager->m_sync, true);
                pSessionManager->m_hStoppingProtocollers.push_back(p_send_point);
                KLSTD_TRACE1 (KLMC_TRACELEVEL, "ProxySessionManager pmSESSION_PSEUDO_STOP_REQUEST added to list (total %d items in list)\n", pSessionManager->m_hStoppingProtocollers.size() );
            }

            err = SetEvent(pSessionManager->m_hRequestSessionStop) ? errOK_DECIDED : errUNEXPECTED;

            if ( PR_FAIL(err))
                KLSTD_TRACE1 (KLMC_TRACELEVEL, "ProxySessionManager pmSESSION_PSEUDO_STOP_REQUEST error set event %d\n", GetLastError() );

            KLSTD_TRACE0 (KLMC_TRACELEVEL, "ProxySessionManager pmSESSION_PSEUDO_STOP_REQUEST event is set\n" );

            return err;
        }
        //
        // Остановка по требованию
        //
        //////////////////////////////////////////////////////////////////////////


        //////////////////////////////////////////////////////////////////////////
        //
        // Сообщения от протоколлеров
        //
        
        CProxySession* pProxySession;
        if((p_msg_id == pmPROCESS_FINISHED || p_msg_id == pmKEEP_ALIVE) &&
            p_par_buf_len == SER_SENDMSG_PSIZE &&
            ((cSerializable*)p_par_buf)->isBasedOn(process_msg_t::eIID) &&
            ((process_msg_t*)p_par_buf)->pdwSessionCtx &&
            ((process_msg_t*)p_par_buf)->dwSessionCtxCount > po_ProxySession &&
            (pProxySession = (CProxySession*)((process_msg_t*)p_par_buf)->pdwSessionCtx[po_ProxySession]) != NULL &&
            pProxySession->ReceiveProtocollerCommand(p_send_point, (process_msg_t*)p_par_buf) != errOBJECT_NOT_FOUND)
        {
            return errOK_DECIDED;
        }
        
        //
        // Сообщения от протоколлеров
        //
        //////////////////////////////////////////////////////////////////////////
    }
	
	return err;
}

DWORD 
WINAPI
CR_ManagerThread( LPVOID pParam )
{
    CProxySessionManager* Manager = (CProxySessionManager*)pParam;

    if ( Manager )
        Manager->ProxyManagerThread();

    return 0;
}

bool
CProxySessionManager::StartManager( USHORT  ListenPort, USHORT ServPort )
{
    m_ListenPort = ListenPort;
    m_ServPort = ServPort;

    Init();

    m_ThreadHandle = CreateThread(NULL, 0, CR_ManagerThread, this, 0, &m_ThreadID);

    return m_ThreadHandle != NULL;
}

void
CProxySessionManager::WaitExitThread()
{
    KLSTD_TRACE2 (KLMC_TRACELEVEL, "WaitExitThread %x => wait thread exit. ThreadHandle = 0x%x\n", this, m_ThreadHandle );

    WaitForSingleObject( m_ThreadHandle, INFINITE );

    KLSTD_TRACE1 (KLMC_TRACELEVEL, "WaitExitThread %x => wait OK \n", this );
}

tERROR
CProxySessionManager::StopManager()
{
    SetEvent( m_hStopEvent );
    WaitExitThread();

	return errOK;
}

tERROR
CProxySessionManager::PauseManager()
{
    return errOK;
}

