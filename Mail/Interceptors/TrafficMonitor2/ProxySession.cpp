// ProxySession.cpp: implementation of the CProxySession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProxySession.h"
#include <mail/common/GetAppInfoByPID.h>
#include "CKAH/ckahiptoa.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////
////////  CProxySession::send_buf_t /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

CProxySession::send_buf_t::send_buf_t() : buf(0), fullsize(0)
{
    alloc_more(2048);
}

CProxySession::send_buf_t::~send_buf_t()
{
    delete[] buf;
};

tBOOL 
CProxySession::send_buf_t::ready()
{
    return fullsize != 0;
};

tBOOL 
CProxySession::send_buf_t::alloc_more(hIO hData)
{
    // Пытаемся выделить буфер размером с hData->GetSize()
    tQWORD qwSize = 0;
    return PR_SUCC(hData->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT)) ? alloc_more((tDWORD)qwSize) : ready();
};

tBOOL 
CProxySession::send_buf_t::alloc_more(tSIZE_T size)
{
    if ( size <= fullsize )
        return ready();				// Не уменьшаем буфер

    if ( size > 64 * 1024 )
        return ready();				// Не более 64Кб

    if ( fullsize != 0 )
        size = (size / 1024 + 1) * 1024;	// Приращиваем по 1Кб

    delete[] buf;
    buf         = new tBYTE[ size + 1 ];
    fullsize    = buf ? ( size + 1 ) : 0;
    return clear();
};

tBOOL 
CProxySession::send_buf_t::clear()
{
    if ( buf )
        memset(buf, 0, fullsize);
    return ready();
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/*
int
AssociateCR( SOCKET client, SOCKET server )
{
    CKAHUM::OpResult opRes;

    int RetVal;
    ULONG lIp1, lIp2, rIp1, rIp2;
    USHORT lPort1, lPort2, rPort1, rPort2;

    RetVal = GetConnInfo( client, &lIp1, &lPort1, &rIp1, &rPort1 );

    if ( RetVal )
        return RetVal;

    RetVal = GetConnInfo( server, &lIp2, &lPort2, &rIp2, &rPort2 );

    if ( RetVal )
        return RetVal;

    opRes = CKAHCR::Associate_Conn( 
        rIp1, rPort1, lIp1, lPort1,
        lIp2, lPort2, rIp2, rPort2 );

    return ( opRes == CKAHUM::srOK ) ? 0 : -1;
}
*/

tBOOL GetParam(cStringObj& sBuf, cStringObj& sRes, tINT& nStart)
{
    PR_TRACE( (NULL, prtNOT_IMPORTANT, "GetParam: Enter...") );

    unsigned int i = nStart;
    while( i < sBuf.length() && sBuf.at(i) != '<' ) 
        i++;

    i++; 
    sRes = "";
    while( i < sBuf.length() && sBuf.at(i) != '>' )
        sRes += sBuf.at( i++ ); 

    i++;
    nStart = i;

    PR_TRACE( (NULL, prtNOT_IMPORTANT, "GetParam: Leave...") );

    return !sRes.empty();	
}

tBOOL ParseKAVSVC(
                  IN tCHAR* Buf,
                  IN tDWORD dwSize,
                  OUT cStringObj& szServerName, 
                  OUT tINT& nPort,
                  OUT tDWORD& dwClientPID)
{
    tBOOL bIsKAVSVC = (  
        Buf[0] == 'K' &&
        Buf[1] == 'A' &&
        Buf[2] == 'V' &&
        Buf[3] == 'S' &&
        Buf[4] == 'V' &&
        Buf[5] == 'C' ) ? cTRUE : cFALSE;

    PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\tParseKAVSVC: %.*s", dwSize, Buf) );

    if ( bIsKAVSVC )
    {
        CKAHCR::AskConnection* Ask = (CKAHCR::AskConnection*) ( Buf + 6 );

        nPort = Ask->m_rPort;
        dwClientPID = Ask->m_PID;

        CKAHUM::IP rIp;
        if ( Ask->m_rIp.IpVer == 6 )
            rIp.Setv6( *(CKAHUM::OWord *)Ask->m_rIp.Ipv6_uchar, Ask->m_rIp.Zone );
        else
            rIp.Setv4( Ask->m_rIp.Ipv4 );

        szServerName = CKAHUM_IPTOA(rIp);

        return cTRUE;
    }

    return cFALSE;
}

tBOOL ParseKAVSEND( 
                   IN tCHAR* Buf, 
                   IN tDWORD dwSize, 
                   OUT cStringObj& szServerName, 
                   OUT tINT& nPort,
                   OUT tINT& nMaskedPort,
                   OUT tDWORD& dwClientPID
                   )
{	
    tBOOL bIsKAVSEND = (  
        Buf[0] == 'K' &&
        Buf[1] == 'A' &&
        Buf[2] == 'V' &&
        Buf[3] == 'S' &&
        Buf[4] == 'E' &&
        Buf[5] == 'N' &&
        Buf[6] == 'D' ) ? cTRUE : cFALSE;

    PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\tParseKAVSEND: %.*s", dwSize, Buf) );

    if ( bIsKAVSEND ) 
    {
        cStringObj sBuf; 
        cStringObj sRes;
        tINT nStart = 0;
        sBuf.append(Buf, cCP_ANSI, 0);

        if( GetParam(sBuf, sRes, nStart) )
            szServerName = sRes;
        else
            PR_TRACE( (NULL, MC_TRACE_LEVEL, "ParseKAVSEND: GetParam(_server_name_) returned false") );

        // Узнаем порт сервера
        if( GetParam(sBuf, sRes, nStart) )
            nPort = atoi(sRes.c_str(cCP_ANSI));
        else
            PR_TRACE( (NULL, MC_TRACE_LEVEL, "ParseKAVSEND: GetParam(_server_port_) returned false") );	

        // Узнаем PID локального процесса, инициализировавшего соединение
        if( GetParam(sBuf, sRes, nStart) )
            dwClientPID = atoi(sRes.c_str(cCP_ANSI));
        else
            PR_TRACE( (NULL, MC_TRACE_LEVEL, "ParseKAVSEND: GetParam(_pid_) returned false") );

        // Узнаем маскирующий порт сервера
        if( GetParam(sBuf, sRes, nStart) )
            nMaskedPort = atoi(sRes.c_str(cCP_ANSI));
        else
        {
            nMaskedPort = nPort;
            PR_TRACE( (NULL, MC_TRACE_LEVEL, "ParseKAVSEND: GetParam(_masked_port_) returned false") );	
        }

        return cTRUE;
    }
    else
        return cFALSE;
}

tDWORD GetSessionIDForProcess(ULONG ProcessId)
{
    DWORD dwSessionId = (DWORD) -1;
    HMODULE hModule = GetModuleHandle("Kernel32.dll");
    typedef BOOL (WINAPI *tProcessIdToSessionId)(DWORD dwProcessId,DWORD* pSessionId);
    tProcessIdToSessionId fnProcessToSession = hModule ? (tProcessIdToSessionId)::GetProcAddress(hModule, "ProcessIdToSessionId") : NULL;
    return fnProcessToSession && fnProcessToSession(ProcessId, &dwSessionId) ? (tDWORD)dwSessionId : (tDWORD)-1;
}

LPCSTR GetDetectCodeString(tDWORD code)
{
    switch(code)
    {
    case dcProtocolUnknown:
        return "dcProtocolUnknown";
    case dcProtocolDetected:
        return "dcProtocolDetected";
    case dcNeedMoreInfo:
        return "dcNeedMoreInfo";
    case dcDataDetected:
        return "dcDataDetected";
    case dcProtocolLikeDetected:
        return "dcProtocolLikeDetected";
    }
    return "<unknown>";
}

void CProxySession::ClearCallback(CSimpleBufferWindow* _this, tDWORD context, tVOID* data)
{
    // Отослать данные на детектирование
    tBYTE* pData = _this->View();
    tDWORD dwDataSize = _this->GetSize();

    if ( dwDataSize )
    {
        KLSTD_TRACE2(KLMC_TRACELEVEL, "ProxySession: Sending data '%s' (length = %d) to protocollers for detection...\n",
            GetBinBufferTraceStringShort(pData, dwDataSize).c_str(),
            dwDataSize);
        tERROR err = ((CProxySession*)context)->SendProtocollerDetect(
            ((ClearCallbackData*)data)->data_source, 
            pData, 
            dwDataSize);
        KLSTD_TRACE2(KLMC_TRACELEVEL, "ProxySession: Protocoller detector returns %terr, DC is %s\n", err, GetDetectCodeString(((CProxySession*)context)->m_dcDetectCode));
    }
}

CProxySession::CProxySession(hOBJECT hParent /* = NULL  */, CProxySessionManager* manager /* = NULL */, HANDLE StopEvent /* = NULL  */) :
	m_hProtocoller(NULL),
	m_dcDetectCode(dcProtocolLikeDetected),
	m_hCurrentUser(NULL),
    m_manager( manager ),
    m_hParent( hParent ),
    m_hStopEvent( StopEvent ),    
	m_ThreadHandle ( NULL ),
    m_Flags( FPS_FIRST_SELECT ),
    m_dwCDCount(10),
	m_socketFromSave(0),
    m_bInWaitSessionList(false)
{
	m_hSessionCtx.pdwSessionCtx = new tPTR[po_last];

	if ( m_hSessionCtx.pdwSessionCtx )
	{
		m_hSessionCtx.dwSessionCtxCount = po_last;
		memset(m_hSessionCtx.pdwSessionCtx, 0, po_last*sizeof(tDWORD));
		m_hSessionCtx.pdwSessionCtx[po_ProxySession] = this;
        m_Flags |= FPS_CTX_ALLOCATED;
	}
    
    m_ccdClient = new ClearCallbackData(dsClient);
    m_Buf_Client = new CSimpleBufferWindow(SOCKETBUFSIZE * 2, ClearCallback, (tDWORD)this, m_ccdClient); // буфер-окно исходящих данных клиента. Размер окна > размера рабочего буфера
    
    m_ccdServer = new ClearCallbackData(dsServer);
    m_Buf_Server = new CSimpleBufferWindow(SOCKETBUFSIZE * 2, ClearCallback, (tDWORD)this, m_ccdServer); // буфер-окно исходящих данных сервера. Размер окна > размера рабочего буфера
}

CProxySession::~CProxySession()
{
	if ( m_hSessionCtx.pdwSessionCtx )
	{
		delete [] m_hSessionCtx.pdwSessionCtx;
		m_hSessionCtx.pdwSessionCtx = NULL;
		m_hSessionCtx.dwSessionCtxCount = 0;
	}
	CLOSEHANDLE(m_hCurrentUser);
	CLOSEHANDLE(m_ThreadHandle);

    delete m_ccdClient;
    delete m_Buf_Client;

    delete m_ccdServer;
    delete m_Buf_Server;
}

PRLocker ssl_cs;

tBOOL CProxySession::NeedToSwitchToSSL(cTrafficSelfAskAction& AskAction)
{
    tERROR err = errOK;

    if ( !m_hSessionCtx.bSSL && m_client.HasSSLHandshake() )
    {
        // Все вопросы по переходу на SSL следует производить под одной критической секцией.
        
        PRAutoLocker _cs_(ssl_cs);

        AskAction.m_nSessionId = GetSessionIDForProcess(AskAction.m_dwClientPID);

        // Спросим сначала у собственных настроек, надо ли переключаться в SSL
        err = m_hParent->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmSESSION_SWITCH_TO_SSL, 0, &AskAction, SER_SENDMSG_PSIZE );

        if ( errOK_DECIDED == err )
        {
            if ( AskAction.m_bDecodeSSL )
            {
                if ( AskAction.m_bNeedToInform )
                    // Сообщим в GUI о переходе на SSL
                    err = m_hParent->sysSendMsg(pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE);
                return cTRUE;
            }
            else 
                return cFALSE;
        }

        // Теперь спросим у GUI, надо ли переключаться в SSL
        err = m_hParent->sysSendMsg( pmcASK_ACTION, pmSESSION_SWITCH_TO_SSL, 0, &AskAction, SER_SENDMSG_PSIZE );

        if ( errOK_DECIDED == err )
        {
            if ( AskAction.m_nUseRule == usr_NO )
                AskAction.m_nUseRule = usr_Temporary;

            // Добавим правило в настройки трафикМонитора
            m_hParent->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmSESSION_SWITCH_TO_SSL, 0, &AskAction, SER_SENDMSG_PSIZE );

            if ( AskAction.m_bDecodeSSL )
            {
                if ( AskAction.m_bNeedToInform )
                    // Сообщим в GUI о переходе на SSL
                    err = m_hParent->sysSendMsg(pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE);

                return cTRUE;
            }
        }
    }
    return cFALSE;
}

void
CProxySession::MakeAlert()
{
    //Выдаем алерт только если нет АХ(fw)
    cProfileBase hAH;
    hTASKMANAGER hTM = (hTASKMANAGER)(m_hParent->propGetObj(pgTASK_TM));
    if( hTM && PR_FAIL(hTM->GetProfileInfo(AVP_PROFILE_FIREWALL, &hAH)) )
    {
        cTrafficAskAction AskAction;
        AskAction.m_strProfile   = AVP_SERVICE_TRAFFICMONITOR;
        AskAction.m_strTaskType  = AVP_TASK_TRAFFICMONITOR;
        AskAction.m_nAction      = pmSESSION_UNACCESS;
        AskAction.m_dwClientPID  = m_hSessionCtx.dwClientPID;
        AskAction.m_szServerName = m_hSessionCtx.szServerName;
        AskAction.m_nServerPort  = m_hSessionCtx.nServerPort;
        GetAppInfoByPid(AskAction.m_dwClientPID, AskAction.m_szImagePath);
        AskAction.m_nSessionId = GetSessionIDForProcess(AskAction.m_dwClientPID);

        KLSTD_TRACE2(KLMC_TRACELEVEL, "ProxySession: Got ImagePath by PID %d, path is '%s'\n", AskAction.m_dwClientPID, (tCHAR*)AskAction.m_szImagePath.c_str(cCP_ANSI));
        m_hParent->sysSendMsg(pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE);
    }
}



void
CProxySession::ProcessFirstSelect()
{
    cTrafficSelfAskAction AskAction;
    AskAction.m_nAction      = pmSESSION_SWITCH_TO_SSL;
    AskAction.m_dwClientPID  = m_hSessionCtx.dwClientPID;
    AskAction.m_szServerName = m_hSessionCtx.szServerName;
    AskAction.m_nServerPort  = m_hSessionCtx.nServerPort;

    if ( NeedToSwitchToSSL( AskAction ) )
    {
        // Переключаемся на SSL, если надо
        certificate_info_t certificate_info;
        tBOOL bSwitched = m_server.SwitchToSSL(false, &certificate_info, m_hStopEvent, (char*)AskAction.m_szSSLCacheFolder.c_str(cCP_ANSI));

        if ( !bSwitched )
        {
			KLSTD_TRACE4(KLMC_TRACELEVEL, "ProxySession: Failed to switch server to SSL 0x%08x (%s, %s, %s)\n", 
                m_server.GetLastError(), certificate_info.szName.c_str(), certificate_info.szFileName.c_str(), certificate_info.szOrgName.c_str());
        }

        if ( bSwitched )
        {
            AskAction.m_szCertName      = certificate_info.szName.c_str();
            AskAction.m_szOrigFileName  = certificate_info.szFileName.c_str();
            certificate_info.szFileName.erase();
            bSwitched = m_client.SwitchToSSL(true,  &certificate_info, m_hStopEvent, (char*)AskAction.m_szSSLCacheFolder.c_str(cCP_ANSI));

            if ( !bSwitched )
            {
				KLSTD_TRACE4(KLMC_TRACELEVEL, "ProxySession: Failed to switch client to SSL 0x%08x (%s, %s, %s)\n", 
                    m_client.GetLastError(), certificate_info.szName.c_str(), certificate_info.szFileName.c_str(), certificate_info.szOrgName.c_str());
            }
        }

        AskAction.m_szFakeFileName = certificate_info.szFileName.c_str();
        AskAction.m_bValid = certificate_info.valid;

        if ( bSwitched )
        {
            // Сообщаем об успешном переходе на SSL
            m_hParent->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmSESSION_SWITCH_TO_SSL, 0, &AskAction, SER_SENDMSG_PSIZE );

            m_hSessionCtx.bSSL = cTRUE;
            m_Flags |= FPS_SWITCHED_SSL;
        }
        else
        {
            AskAction.m_bSSLEstablished = cFALSE;
            m_hParent->sysSendMsg(pmc_EVENTS_IMPORTANT, 0, 0, &AskAction, SER_SENDMSG_PSIZE);            
        }

        KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession: SwitchToSSL %s\n", bSwitched ? "succeeded" : "failed" );

        if ( !bSwitched )
        {
            KLSTD_TRACE1(KLMC_TRACELEVEL, "ProcessFirstSelect : (%p) Reconnect server!\n", this ); 
            
            BreakKavsvcConnect();
            
            if ( (m_Flags & FPS_SERVER_CONNECTED) && !(m_Flags & FPS_SERVER_DISCONNECTED) )
            {
                m_server.Disconnect();
                m_Flags |= FPS_SERVER_DISCONNECTED;
                m_Flags &= ~FPS_SERVER_CONNECTED;
            }

            DoConnect();
            m_Flags &= ~FPS_FIRST_SELECT;
        }
    }
}

// Вызывается в контексте серверного потока
tERROR
CProxySession::StartProtocollers()
{
    tERROR err = errOK;

	KLSTD_TRACE3 (KLMC_TRACELEVEL, "ProxySession (%p, %p): StartProtocollers, flags = 0x%x\n", this, &m_hSessionCtx, m_Flags );

	err = m_hParent->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmSESSION_START, 0, &m_hSessionCtx, SER_SENDMSG_PSIZE );

    InterlockedIncrement ( &m_manager->m_SessionCount );
    
    m_Flags |= FPS_START_SENT;

    return err;
}

// вызывается в контексте серверного потока
tERROR
CProxySession::StopProtocollers()
{
    tERROR err = errOK;

    KLSTD_TRACE3 (KLMC_TRACELEVEL, "ProxySession (%p, %p): StopProtocollers, flags = 0x%x\n", this, &m_hSessionCtx, m_Flags );

    if ( m_Flags & FPS_STOP_SENT )
        return errOK;

    if ( m_Flags & FPS_START_SENT )
    {
        err = m_hParent->sysSendMsg( pmcTRAFFIC_PROTOCOLLER, pmSESSION_STOP, 0, &m_hSessionCtx, SER_SENDMSG_PSIZE );
        
        m_Flags |= FPS_STOP_SENT;

        InterlockedDecrement ( &m_manager->m_SessionCount );

        KLSTD_TRACE3 (KLMC_TRACELEVEL, "ProxySession (%p): message sent. flags = 0x%x session_count = %d\n", 
            this, m_Flags, m_manager->m_SessionCount );
    }

    return err;
}

void
CProxySession::RemoveFromWaitSessionList()
{
    EnterCriticalSection(&m_manager->m_WaitSessionListLock);

    m_manager->m_WaitSessionList.remove( this );

    LeaveCriticalSection( &m_manager->m_WaitSessionListLock );
}


void
CProxySession::WaitExitThread()
{
    KLSTD_TRACE2 (KLMC_TRACELEVEL, "ProxySession (%p): Waiting thread 0x%x...\n", this, m_ThreadHandle );

    WaitForSingleObject( m_ThreadHandle, INFINITE );

    KLSTD_TRACE1 (KLMC_TRACELEVEL, "ProxySession (%p): OK\n", this );
}

tERROR CProxySession::PseudoStop(IN hOBJECT hProtocoller)
{
    tERROR  err;
    pseudo_stop_t pseudo_stop;

	if ( !hProtocoller )
		return errNOT_OK;

    KLSTD_TRACE2 (KLMC_TRACELEVEL, "ProxySession (%p): PseudoStop for %p protocoller\n", this, hProtocoller );
	
    pseudo_stop.assign(m_hSessionCtx, false);
	pseudo_stop.phProtocollerHandle = &hProtocoller;
	
    err = m_hParent->sysSendMsg( 
                            pmcTRAFFIC_PROTOCOLLER, 
                            pmSESSION_PSEUDO_STOP, 
                            0, 
                            &pseudo_stop, 
                            SER_SENDMSG_PSIZE );

    KLSTD_TRACE2 (KLMC_TRACELEVEL, "ProxySession (%p): PseudoStop for %p protocoller Done.\n", this, hProtocoller );

    return err;
}

void
CProxySession::BreakKavsvcConnect()
{
    if ( m_ConnectSocket != INVALID_SOCKET )
    {   
        KLSTD_TRACE2 (KLMC_TRACELEVEL, "ProxySession (%p): shutdown1 socket 0x%x\n", this, m_ConnectSocket );
        shutdown( m_ConnectSocket, SD_BOTH );
        closesocket( m_ConnectSocket );
        m_ConnectSocket = INVALID_SOCKET;
        m_Flags |= FPS_SERVER_DISCONNECTED;
    }
}

void
CProxySession::ProcessExit()
{
	StopSession();

    KLSTD_TRACE1 (KLMC_TRACELEVEL, "ProxySession (%p): Stopped\n", this );

    m_Flags |= FPS_DONE;
}

void
CProxySession::StopSession()
{
    if ( (m_Flags & FPS_DONE) == 0 )
    {
        StopProtocollers();

        BreakKavsvcConnect();

        KLSTD_TRACE3 (KLMC_TRACELEVEL, "ProxySession (%p): Closing sockets. server = 0x%x, client = 0x%x\n", 
            this, m_server.Socket(), m_client.Socket() );

        KLSTD_TRACE2 (KLMC_TRACELEVEL, "ProxySession (%p): Close connecting socket 0x%x\n", this, m_ConnectSocket );

        if ( (m_Flags & FPS_SERVER_CONNECTED) && !(m_Flags & FPS_SERVER_DISCONNECTED) )
        {
            m_server.Disconnect();
            m_Flags |= FPS_SERVER_DISCONNECTED;
            m_Flags &= ~FPS_SERVER_CONNECTED;
        }

        if ( (m_Flags& FPS_CLIENT_CONNECTED) && !(m_Flags & FPS_CLIENT_DISCONNECTED) )
        {
            m_client.Disconnect();
            m_Flags |= FPS_CLIENT_DISCONNECTED;
            m_Flags &= ~FPS_CLIENT_CONNECTED;
        }

        m_Flags &= ~FPS_FINISH;
        m_Flags |= FPS_DONE;
    }
    else
    {
        KLSTD_TRACE0 (KLMC_TRACELEVEL, "!! CProxySession::StopSession called with FPS_FINISH cleared !!! ");    
    }
}

DWORD 
WINAPI
CR_ClientThread( LPVOID pParam )
{
    tERROR  err = errOK;
    CProxySession* Client = (CProxySession*)pParam;

    if ( Client )    
    {
        if ( (Client->m_Flags & FPS_ESTABLISHED) == 0 ) 
        {
            if ( Client->m_Flags & FPS_GOT_KAVSEND )
            {
                // Клиентский сокет...
                Client->m_client.AssignSocket( Client->m_AcceptedSock );
                Client->m_Flags |= FPS_CLIENT_CONNECTED;

                if ( PR_SUCC(Client->DoConnect() ))
                {
                    GetLocalName( Client->m_client.Socket(), Client->m_Localhost, sizeof ( Client->m_Localhost ) );
                                        
                    Client->m_client.PrepareSocket();

                    Client->m_Flags |= FPS_SERVER_CONNECTED | FPS_ESTABLISHED;                    
                }
                else
                {
                    Client->m_Flags |= FPS_FINISH;
                }
            }
            else if ( Client->m_Flags & FPS_GOT_KAVSVC ) 
            {
                int RetVal;
                CKAHCR::ReplyConnection Reply;

                // соединяемся с сервером.
                err = Client->DoConnect();
                                
                Reply.m_Status   = PR_SUCC(err) ? 0 : -1;
                Client->m_ID = Reply.m_ID = (__int64)Client;

                Client->m_StartTime = GetTickCount();
                SOCKET s = Client->m_AcceptedSock;
                Client->m_AcceptedSock = INVALID_SOCKET;                
                
                Client->m_Flags |= PR_SUCC(err) ? FPS_SERVER_CONNECTED : FPS_FINISH;

                // отсылаем драйверу ответ о статусе серверного соединения и мочим сокет.
                RetVal = send( s, (const char*)&Reply, sizeof ( Reply ), 0 );

                // после вызова send обращение к Client запрещено.
                // т.к. на другом потоке может оказаться, что соединение не установилось и все почикали.

                KLSTD_TRACE3(KLMC_TRACELEVEL, "ProcessKavsvcCommand: sent %d bytes. err = %d. now close 0x%x\n", RetVal, WSAGetLastError(), s );
                KLSTD_TRACE1(KLMC_TRACELEVEL, "ProcessKavsvcCommand: now close m_AcceptedSock 0x%x\n", s );

                //CLOSESOCKET( s );
                shutdown ( s, SD_BOTH );
                closesocket( s );

                // поток завершится и запустится когда придет редирект из драйвера.
                // вообще можно избавиться от лишней операции создания потока.                
                return 0;
            }
        }
        
        if ( Client->m_Flags & FPS_ESTABLISHED )
        {
            bool bCOMInited = SUCCEEDED(::CoInitialize(NULL));
            
            Client->ProcessClient();
            
            if ( bCOMInited )
                ::CoUninitialize();
        }
    }

    return 0;
}

bool
CProxySession::StartClient()
{
	CLOSEHANDLE(m_ThreadHandle);

	m_ThreadHandle = CreateThread(NULL, 0, CR_ClientThread, this, 0, &m_ThreadID);

	return m_ThreadHandle != NULL;
}

tERROR
CProxySession::ParseKavsendBuffer(char* Buf, int nRecv )
{
    if ( ParseKAVSEND(Buf, nRecv, m_hSessionCtx.szServerName, m_hSessionCtx.nServerPort, m_ServerPort, m_hSessionCtx.dwClientPID) )
    {
        strcpy( m_rIp, (char*)m_hSessionCtx.szServerName.c_str(cCP_ANSI) );
        m_ServerPort = m_hSessionCtx.nServerPort;
    }

    return errOK;
}

tERROR
CProxySession::ParseKavsvcBuffer(char* Buf, int nRecv )
{
    if ( ParseKAVSVC(Buf, nRecv, m_hSessionCtx.szServerName, m_hSessionCtx.nServerPort, m_hSessionCtx.dwClientPID ) )
    {   
        strcpy( m_rIp, (char*)m_hSessionCtx.szServerName.c_str(cCP_ANSI) );
        m_ServerPort = m_hSessionCtx.nServerPort;
    }

    return errOK;
}