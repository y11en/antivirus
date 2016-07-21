#define PR_IMPEX_DEF

#include "stdafx.h"
#include "ProxySession.h"
#include "util.h"

tERROR
CProxySession::DoConnect()
{    
    tBOOL   bNeedToAlert = cFALSE;    

    m_ConnectSocket = Connect( m_ConnectSocket, m_hSessionCtx.szServerName.c_str(cCP_ANSI), m_ServerPort, bNeedToAlert );

    if ( m_ConnectSocket == INVALID_SOCKET )
    {
        if ( bNeedToAlert )
            MakeAlert();

        // cannot connect server;
        KLSTD_TRACE3 (KLMC_TRACELEVEL, "ProxySession (%p): Failed to connect to %S:%d\n", this, m_hSessionCtx.szServerName.data(), m_ServerPort );

        m_Flags |= FPS_FINISH;
        return errNOT_OK;
    }

    KLSTD_TRACE3 (KLMC_TRACELEVEL, "ProxySession (%p): Successfully connected to %S:%d\n", this, m_hSessionCtx.szServerName.data(), m_ServerPort );

    if(m_server.AssignSocket(m_ConnectSocket, 0 /*, ulServerIP*/ ))
        m_ConnectSocket = INVALID_SOCKET;
    m_server.PrepareSocket();

    GetLocalName( m_server.Socket(), m_lIp, sizeof ( m_lIp ) );

    return errOK;
}

tINT
CProxySession::ProcessClient( )
{
    int                 nRecv;    
    data_source_t       source_t;
    CSimpleBufferWindow *from_buf, *to_buf;

    DWORD               dwWaitResult = 0;

    tCHAR Buf[SOCKETBUFSIZE];	// рабочий буфер, в который идет прием данных из сокета
    tCHAR* Buf2Send = Buf;      // указатель на буфер для отправки

    // WSANETWORKEVENTS NetEvents;
    CTransparentSocket* from = &m_server;	// Сокет для чтения
    CTransparentSocket* to   = &m_client;	// Сокет для записи
    u_long arg = 0;    

    fd_set  fds;
    TIMEVAL Timeout = { 1, 0 }; // 1 second Timeout
    int     TimeCount;

    KLSTD_TRACE1 (KLMC_TRACELEVEL, "ProxySession (%p): Start Processing\n", this );

    while(true)
    {
        FD_ZERO(&fds);
		if(m_client.is_alive())
			FD_SET(m_client.Socket(), &fds);
		if(m_server.is_alive())
			FD_SET(m_server.Socket(), &fds);

        TimeCount = 0;

        do 
        {
            int RetVal = select( 0, &fds, NULL, NULL, &Timeout );
			// проверим event на выход
            dwWaitResult = WaitForSingleObject( m_hStopEvent, 0 );
            if ( WAIT_OBJECT_0 == dwWaitResult || WAIT_FAILED == dwWaitResult )
            {
                KLSTD_TRACE1 (KLMC_TRACELEVEL, "ProxySession (%p): StopEvent signalled \n", this);

                m_Flags |= FPS_FINISH;
                break;
            }
			// сработал select?
			if(RetVal != 0)
				break;
        }
        while(++TimeCount < 10);

        if ( (m_Flags & FPS_FINISH) || (m_Flags & FPS_DONE) )
            break;

        if ( m_client.is_alive() && FD_ISSET( m_client.Socket(), &fds ))
        {
            // Произошло событие на сокете клиента   
            KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession (%p): Client data received", this);

            source_t = dsClient;
            from     = &m_client;
            to       = &m_server;

            from_buf = m_Buf_Client;
            to_buf   = m_Buf_Server;
            
            // Если мы еще не переключились в режим SSL, то 10 получений данных от клиента
            // будем выяснять : ssl или не ssl...
            if ( (m_Flags & FPS_FIRST_SELECT) && !(m_Flags & FPS_SWITCHED_SSL) )
            {
                ProcessFirstSelect();
                if ( !m_dwCDCount-- )
                    m_Flags &= ~FPS_FIRST_SELECT;

                if ( (m_Flags & FPS_SWITCHED_SSL) == FPS_SWITCHED_SSL )
                    continue;
            }
        }
        else if ( m_server.is_alive() && FD_ISSET( m_server.Socket(), &fds ) )
        {
            // Произошло событие на сокете сервера
            KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession (%p): Server data received", this);

            source_t = dsServer;
            from     = &m_server;
            to       = &m_client;

            from_buf = m_Buf_Server;
            to_buf   = m_Buf_Client;

            // m_Flags &= ~FPS_FIRST_SELECT;
        }
        else
        {
            // Истек таймаут. клиент добавляется в общую очередь.
            m_manager->AddClient( this );
            return 0;
        }

        memset(Buf, 0, SOCKETBUFSIZE);

        Buf2Send = Buf;

        nRecv = from->Recv(Buf, SOCKETBUFSIZE);

		if(nRecv == 0)
		{
			KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession (%p): Recv returns 0\n", this);

//#pragma PR_BUILD_MSG("Graceful shutdown temporary disabled")

			if(to->is_alive())
			{
				StopProtocollers();
				
				KLSTD_TRACE2(KLMC_TRACELEVEL, "ProxySession (%p): Shutting down %s socket\n", this, source_t == dsServer ? "client" : "server");
				shutdown(to->Socket(), SD_SEND);
			}
			from->set_alive(false);
			
            if(!from->is_alive() && !to->is_alive())
			{
				KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession (%p): Connection seems to be closed\n", this);
				m_Flags |= FPS_FINISH;
				break;
			}
			continue;
//
//			m_Flags |= FPS_FINISH;
//			break;
		}

		if ( nRecv == SOCKET_ERROR )
        {
			KLSTD_TRACE2(KLMC_TRACELEVEL, "ProxySession (%p): Recv error %d\n", this, from->GetLastError());
			m_Flags |= FPS_FINISH;
            break;
        }

        KLSTD_TRACE5( KLMC_TRACELEVEL, "mc\tProxySession (%p): Received %d bytes from %s (0x%x):\r\n%s\n", 
            this, nRecv, source_t == dsServer ? "server" : "client",
            from->Socket(), GetBinBufferTraceString((const BYTE *)Buf, nRecv).c_str() );

        tINT nSend = 0;

        //////////////////////////////////////////////////////////////////////////
        //
        // Смена контекста данных
        //
        tBOOL bContextChanged = (from->Socket() != m_socketFromSave);
        m_socketFromSave = from->Socket();

        from_buf->Insert((tBYTE*)Buf, nRecv, cFALSE);

        // Распечатываем содержимое противоположного окна, и обнуляем его
        if ( bContextChanged )            
            to_buf->Clear();

        // Смена контекста данных
        //
        //////////////////////////////////////////////////////////////////////////


        //////////////////////////////////////////////////////////////////////////
        //
        // Отдадим буфер-окно на расширенное детектирование
        //

        if ( m_dcDetectCode == dcProtocolLikeDetected )
        {            
            tBYTE *pData = from_buf->View();
            tULONG dwDataSize = from_buf->GetSize();

            KLSTD_TRACE3(KLMC_TRACELEVEL, "ProxySession (%p): Sending data '%s' (length = %d) to protocollers for detection...\n",
                this,
                GetBinBufferTraceStringShort(pData, dwDataSize).c_str(),
                dwDataSize);

            tERROR e;

            if ( PR_FAIL( e = SendProtocollerDetectEx(
                source_t,
                pData,
                dwDataSize,
                pData + dwDataSize - nRecv,
                nRecv
                )))
            {
                m_dcDetectCode = dcNeedMoreInfo;
                m_hProtocoller = NULL;
            }

            Buf2Send = (tCHAR *)pData;
            nRecv    = dwDataSize;
            KLSTD_TRACE3(KLMC_TRACELEVEL, "ProxySession (%p): Protocoller detectors return %terr, DC is %s\n", this, e, GetDetectCodeString(m_dcDetectCode));
        }
        //
        // Отдадим буфер-окно на расширенное детектирование
        //
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //
        // Отдадим буфер на процессирование
        //
        if ( m_dcDetectCode == dcDataDetected )
        {
            // Посылаем данные протоколлеру
            KLSTD_TRACE3(KLMC_TRACELEVEL, "ProxySession (%p): Data detected, sending them '%s' (length = %d) for processing\n",
                this, GetBinBufferTraceStringShort((const BYTE *)Buf2Send, nRecv).c_str(), nRecv);

            tERROR e;
            if ( PR_FAIL(e = SendProtocollerProcess(source_t, Buf2Send, nRecv)) )
            {
                m_dcDetectCode  = dcNeedMoreInfo;
                m_hProtocoller  = NULL;
            }
            else
            {
                if ( m_dcDetectCode == dcProtocolLikeDetected )
                    from_buf->Clear(cFALSE, nRecv);

                Buf2Send = NULL; // Этот буфер не надо отправлять клиенту
            }
            KLSTD_TRACE3(KLMC_TRACELEVEL, "ProxySession (%p): Protocoller processor returns %terr, DC is %s\n", this, e, GetDetectCodeString(m_dcDetectCode));
        }
        //
        // Отдадим буфер на процессирование
        //
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //
        // Посылаем полученные данные напрямую клиенту
        //
        if ( m_dcDetectCode != dcDataDetected && 
             m_dcDetectCode != dcProtocolLikeDetected )
        {   
			KLSTD_TRACE0(KLMC_TRACELEVEL, "ReadWriteSocket: Sending data directly to client\n");
            tBOOL bCorrected = cFALSE;
            data_target_t data_target = to == &m_server ? dsServer : dsClient;
            //////////////////////////////////////////////////////////////////////////
            //
            //	Подправляем локальный IP в известных протоколах
            //
            if ( Buf2Send && bContextChanged && from->Socket() == m_client.Socket() )
            {
                if ( nRecv >= 14 )
                {
                    if ( 0 == memcmp("PORT 127,0,0,1", Buf2Send, 14) )
                    {
                        // FTP
                        cStringObj szIP = m_lIp;// inet_ntoa(saddr_info.sin_addr);
                        szIP.replace_all(".", 0, ",");
                        cStringObj szBuf2Send;
                        szBuf2Send.append( Buf2Send, cCP_ANSI, 5 );
                        szBuf2Send += (tCHAR*)szIP.c_str(cCP_ANSI);
                        szBuf2Send.append( Buf2Send + 14, cCP_ANSI, nRecv - 14);
                        // nSend = _send( *to, (tCHAR*)szBuf2Send.c_str(cCP_ANSI), szBuf2Send.length(), 0, m_hStopEvent, data_target);
                        nSend = to->Send( (tCHAR*)szBuf2Send.c_str(cCP_ANSI), szBuf2Send.length(), data_target == dsClient );
                        bCorrected = cTRUE;
                    }
                    else if ( 0 == memcmp("HELO 127.0.0.1", Buf2Send, 14 ) ||
                              0 == memcmp("EHLO 127.0.0.1", Buf2Send, 14 ) ||
                              0 == memcmp("EHLO localhost", Buf2Send, 14 )) 
                    {
                        // SMTP
                        cStringObj szIP = m_lIp;//inet_ntoa(saddr_info.sin_addr);
                        cStringObj szBuf2Send;
                        szBuf2Send.append( Buf2Send, cCP_ANSI, 5 );
                        szBuf2Send += (tCHAR*)szIP.c_str(cCP_ANSI);
                        szBuf2Send.append( Buf2Send + 14, cCP_ANSI, nRecv - 14);
                        // nSend = _send( *to, (tCHAR*)szBuf2Send.c_str(cCP_ANSI), szBuf2Send.length(), 0, m_hStopEvent, data_target);

                        nSend = to->Send( (tCHAR*)szBuf2Send.c_str(cCP_ANSI), szBuf2Send.length(), data_target == dsClient );

                        bCorrected = cTRUE;
                    }
                }
            }

            //
            //	Подправляем локальный IP в известных протоколах
            //
            //////////////////////////////////////////////////////////////////////////
            if ( !bCorrected )
                // nSend = _send(*to, Buf2Send, nRecv, 0, m_hStopEvent, data_target);
                nSend = to->Send( Buf2Send, nRecv, data_target == dsClient );
        }
        //
        // Посылаем полученные данные напрямую клиенту
        //
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //
        // Очистка буферов, ушедших на процессирование
        //
        if ( m_dcDetectCode == dcDataDetected )
        {
            KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession (%p): Clearing buffers\n", this );

            m_Buf_Server->Clear(cFALSE);
            m_Buf_Client->Clear(cFALSE);
        }
        //
        // Очистка буферов, ушедших на процессирование
        //
        //////////////////////////////////////////////////////////////////////////

        if ( nSend == SOCKET_ERROR )
        {
            KLSTD_TRACE2(KLMC_TRACELEVEL, "ProxySession (%p): Send error = %d\n", this, WSAGetLastError());

            m_Flags |= FPS_FINISH;
            break;
        }

        KLSTD_TRACE1(KLMC_TRACELEVEL, "ProxySession (%p): Send ok\n", this);
    }

    return 0;
}

std::vector<tINT> accepted_ports; // список удачных соединений
PRLocker accepted_ports_cs;

SOCKET Connect( SOCKET& conn, IN tCHAR* lpszServerName, IN tINT nPort, OUT tBOOL& bNeedToAlert )
{	
    tCHAR  PortBuffer[20];
    tCHAR* lpszPort = _itoa( nPort, PortBuffer, 10 );

    KLSTD_TRACE2( prtNOTIFY, "Connect : Connect function... [%s]:%d", lpszServerName, nPort );

    ADDRINFO Hints, *AddrInfo, *AI;
    conn = INVALID_SOCKET;
    int RetVal;

    // By not setting the AI_PASSIVE flag in the hints to getaddrinfo, we're
    // indicating that we intend to use the resulting address(es) to connect
    // to a service.
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family     = PF_UNSPEC;
    Hints.ai_socktype   = SOCK_STREAM;
    Hints.ai_flags      = AI_NUMERICHOST;
    Hints.ai_protocol   = 6;
    RetVal = getaddrinfo( lpszServerName, lpszPort, &Hints, &AddrInfo); // сюда выполняется коннект

    for ( AI = AddrInfo; AI && AI->ai_protocol == 0 ; AI = AI->ai_next )
    {
        KLSTD_TRACE3(KLMC_TRACELEVEL, "Connect : (fam = %d) ( proto = %d ) ( type = %d )  \n", 
            AI->ai_family,    
            AI->ai_protocol,
            AI->ai_socktype);
    }

    if ( AI == NULL )
    {
        KLSTD_TRACE0(KLMC_TRACELEVEL, "Connect : ( AI == NULL )  \n");
        freeaddrinfo(AddrInfo);
        return INVALID_SOCKET;
    }

    KLSTD_TRACE4(KLMC_TRACELEVEL, "Connect : getaddrinfo : RetVal = %d, fam=%d, st=%d, proto=%d\n",
        RetVal, AddrInfo->ai_family, AddrInfo->ai_socktype, AddrInfo->ai_protocol );

    // создаем локальный адрес такого же типа
    conn = socket( AddrInfo->ai_family, AddrInfo->ai_socktype, AddrInfo->ai_protocol );

    if ( conn == INVALID_SOCKET )
    {
        KLSTD_TRACE1(KLMC_TRACELEVEL, "Connect : Unable to create socket (%d)\n", WSAGetLastError());

        freeaddrinfo(AddrInfo);
        return conn;
    }

    const int on = 1;
    setsockopt(conn, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof (on));

    int wsize1, wsize2, windowSize;
    int len;

    len = sizeof ( windowSize );
    getsockopt( conn, SOL_SOCKET, SO_RCVBUF, (char*) &wsize1, &len);
    len = sizeof ( windowSize );
    getsockopt( conn, SOL_SOCKET, SO_SNDBUF, (char*) &wsize2, &len);

    windowSize = 32768;
    setsockopt( conn, SOL_SOCKET, SO_RCVBUF, (char*) &windowSize, sizeof( windowSize ));
    setsockopt( conn, SOL_SOCKET, SO_SNDBUF, (char*) &windowSize, sizeof( windowSize ));

    WSA_COMPATIBILITY_MODE mode;
    char dummy[4];
    DWORD ret_dummy;

    WSAIoctl(conn, SIO_SET_COMPATIBILITY_MODE,  
        (char *)&mode, sizeof(mode),
        dummy, 4, &ret_dummy, 0, NULL);

    KLSTD_TRACE3(KLMC_TRACELEVEL, "Connect : old SO_RCVBUF(%d), old SO_SNDBUF(%d) new windows(%d) \n",
        wsize1, wsize2, windowSize );

    RetVal = connect( conn, AddrInfo->ai_addr, AddrInfo->ai_addrlen );

    freeaddrinfo(AddrInfo);

    if( RetVal )
    {		
        KLSTD_TRACE1(KLMC_TRACELEVEL, "Connect : Unable to connect (%d)\n", WSAGetLastError());

        // Ищем номер порта в списке удачных соединений
        PRAutoLocker _cs_(accepted_ports_cs);

        bool bFound = false;
        for ( size_t i = 0; i < accepted_ports.size(); i++ )
        {
            if ( nPort == accepted_ports.at(i) )
            {
                bFound = true;
                break;
            }
        }

        // Запоминаем этот порт в списке, чтобы не анноить пользователя
        if ( !bFound ) 
            accepted_ports.push_back(nPort); 

        bNeedToAlert = !bFound;

        KLSTD_TRACE1(KLMC_TRACELEVEL, "Connect : Close conn\n", conn);

        // соединение не установилось. просто closesocket
        closesocket(conn);		

        return conn = INVALID_SOCKET;
    }

    KLSTD_TRACE0(KLMC_TRACELEVEL, "Connect : Connected ok \n");
    
    // Запоминаем номер порта удачного соединения
    PRAutoLocker _cs_(accepted_ports_cs);
    bool bFound = false;

    for ( size_t i = 0; i < accepted_ports.size(); i++ )
    {
        if ( nPort == accepted_ports.at(i) )
        {
            bFound = true;
            break;
        }
    }

    if ( !bFound ) 
        accepted_ports.push_back(nPort);
    

    return conn;
}

LPCTSTR GetProcessMsgStatusStr(process_status_t psStatus)
{
    switch(psStatus)
    {
    case psProcessFailed:
        return "psProcessFailed";
    case psUnknown:
        return "psUnknown";
    case psProcessFinished:
        return "psProcessFinished";
    case psKeepAlive:
        return "psKeepAlive";
    case psCloseConnection:
        return "psCloseConnection";
    }
    PR_ASSERT(0);
    return "<unknown>";
}



tERROR CProxySession::ReceiveProtocollerCommand(hOBJECT p_send_point, process_msg_t* ppmCommand)
{
	tERROR err = errOK;
	tBOOL bNeedToStop = cFALSE;

	if ( p_send_point != m_hParent )
		return errOBJECT_NOT_FOUND;

    KLSTD_TRACE2( KLMC_TRACELEVEL, "ProxySession (%p): Protocoller command received (status = %s)\n", this, GetProcessMsgStatusStr(ppmCommand->psStatus));
	
    if( ppmCommand->psStatus == psProcessFinished ||
        ppmCommand->psStatus == psProcessFailed ||
        ppmCommand->psStatus == psCloseConnection ||
        ppmCommand->psStatus == psUnknown )
    {
        //			PRAutoLocker cs(m_socket_cs); // синхронизируем работу с сокетами в нескольких потоках
        KLSTD_TRACE1( KLMC_TRACELEVEL, "ProxySession (%p): this is a last command from the protocoller\n", this);

        m_dcDetectCode = ppmCommand->psStatus == psUnknown ? dcProtocolLikeDetected : dcNeedMoreInfo;
        m_hProtocoller = NULL;
    }

    PRAutoLocker cs(m_send_buf_cs); // синхронизируем работу m_send_buf в нескольких потоках

    if ( m_send_buf.ready() && ( ppmCommand->psStatus == psProcessFinished  ||
                                 ppmCommand->psStatus == psKeepAlive        ||
                                 ppmCommand->psStatus == psCloseConnection  ||
                                 ppmCommand->psStatus == psUnknown) )
    {
        // А вот тут мы просто зальем данные в сокет. 
        // Причем, при таком исполнении, нам нет разницы между статусами psStatus
        // Посылаем полученные данные напрямую клиенту
        tDWORD dwRead = 0;
        tQWORD pos = 0;
        m_send_buf.clear();

        CTransparentSocket &to = (ppmCommand->dsDataTarget == dsClient) ? m_client : m_server;
        if ( to == INVALID_SOCKET )
        {
            err = errNOT_OK;
            KLSTD_TRACE1( prtERROR, "ProxySession (%p): no socket\n", this );
        }
        else
        {
            if ( ppmCommand->hData )
            {
                KLSTD_TRACE1( prtERROR, "ProxySession (%p): Sending data...\n", this);

                do
                {
                    m_send_buf.clear();
                    
                    // Реаллоцируем буфер, если нужно (не более 64K)
                    m_send_buf.alloc_more(ppmCommand->hData); 

                    err = ppmCommand->hData->SeekRead( &dwRead, pos, m_send_buf.buf,  m_send_buf.fullsize - 1 );

                    if ( PR_SUCC( err ) && dwRead )
                    {
                        pos += dwRead;

                        tINT nSend = to.Send( (char*)m_send_buf.buf, dwRead, ppmCommand->dsDataTarget == dsClient );

                        if( nSend == SOCKET_ERROR )
                        {
                            err = errNOT_OK;
                            KLSTD_TRACE2( prtERROR, "ProxySession (%p): 'send' failed with %d\n", this, WSAGetLastError());

                            bNeedToStop = cTRUE;
                            break;
                        }                        

                        if ( WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0 )	
                        {
                            err = errNOT_OK;
                            KLSTD_TRACE1( KLMC_TRACELEVEL, "ProxySession (%p): Stop event signalled\n", this);	

                            break;
                        }
                    }
                }
                while( PR_SUCC(err) && dwRead );
            }
        }
    }
    
    // Если протоколлер требует разорвать связь - исполним это пожелание
    if ( ppmCommand->psStatus == psCloseConnection )        
        bNeedToStop = cTRUE;
	
	if ( bNeedToStop )
    {
		// Stop(cFALSE);
        KLSTD_TRACE1( KLMC_TRACELEVEL, "ProxySession (%p): exiting. protocoller asked..\n", this);

        m_Flags |= FPS_FINISH;
    }

    KLSTD_TRACE1( KLMC_TRACELEVEL, "ProxySession (%p): Protocoller command processed\n", this);

	return err;
}

tERROR CProxySession::SendProtocollerDetectEx(
		IN data_source_t dsDataSource,   // Источник данных
		IN tVOID* pDataBuffer,           // Полный буфер с данными для оценки (все данные, переданные в направлении dsDataSource в состоянии dcProtocolLikeDetected)
		IN tULONG ulDataBuffer,          // Размер полного буфера
		IN tVOID* pLastReceivedData,     // Буфер с данными для оценки (расположен в конце полного буфера)
		IN tULONG ulLastReceivedData     // Размер буфера
		)
{
	tERROR err = errOK;	
	detect_ex_t hDetectParams;
	
    hDetectParams.assign(m_hSessionCtx, false);
	hDetectParams.dsDataSource       = dsDataSource;
	hDetectParams.pDataBuffer        = pDataBuffer;
	hDetectParams.ulDataBuffer       = ulDataBuffer;
	hDetectParams.pLastReceivedData  = pLastReceivedData;
	hDetectParams.ulLastReceivedData = ulLastReceivedData;
	hDetectParams.pdcDetectCode      = &m_dcDetectCode;
	hDetectParams.phDetectorHandle   = &m_hProtocoller;

	err = m_hParent->sysSendMsg( 
                                        pmcTRAFFIC_PROTOCOLLER, 
                                        pmDETECT_EX, 
                                        0, 
                                        &hDetectParams, 
                                        SER_SENDMSG_PSIZE );

	if ( PR_FAIL(err) || (err == errOK_NO_DECIDERS) ) //TODO: Если pmDETECT_EX будет обрабатываться более чем одним протоколлером - придется придумать, как обойтись без errOK_DECIDED
	{
		m_dcDetectCode = dcNeedMoreInfo;
		m_hProtocoller = NULL;
	}
	return err;
}

tERROR CProxySession::SendProtocollerDetect(
		IN data_source_t dsDataSource,   // Источник данных
		IN tVOID* pLastReceivedData,     // Буфер с данными для оценки
		IN tULONG ulLastReceivedData     // Размер буфера
		)
{
	tERROR err = errOK;	
	detect_t hDetectParams;

	hDetectParams.assign(m_hSessionCtx, false);
	hDetectParams.dsDataSource       = dsDataSource;
	hDetectParams.pLastReceivedData  = pLastReceivedData;
	hDetectParams.ulLastReceivedData = ulLastReceivedData;
	hDetectParams.pdcDetectCode      = &m_dcDetectCode;
	hDetectParams.phDetectorHandle  = &m_hProtocoller;
	
    err = m_hParent->sysSendMsg( 
                                       pmcTRAFFIC_PROTOCOLLER, 
                                       pmDETECT, 
                                       0, 
                                       &hDetectParams, 
                                       SER_SENDMSG_PSIZE );
	if ( PR_FAIL(err) ) 
	{
		m_dcDetectCode = dcNeedMoreInfo;
		m_hProtocoller = NULL;
	}
	return err;
}

tERROR CProxySession::SendProtocollerProcess(
		IN data_source_t dsDataSource,   // Источник данных
		IN tVOID* pLastReceivedData,     // Буфер с данными для обработки
		IN tULONG ulLastReceivedData     // Размер буфера
		)
{
	if ( !m_hProtocoller )
		return errOBJECT_NOT_FOUND;

	tERROR err = errOK;
	process_status_t psStatus;
	process_t hProcessParams;

	hProcessParams.assign(m_hSessionCtx, false);
	hProcessParams.dsDataSource     = dsDataSource;
	hProcessParams.pData            = pLastReceivedData;
	hProcessParams.ulDataSize       = ulLastReceivedData;
	hProcessParams.phDetectorHandle = &m_hProtocoller;
	hProcessParams.psStatus         = &psStatus;
	
    err = m_hParent->sysSendMsg( 
                                       pmcTRAFFIC_PROTOCOLLER, 
                                       pmPROCESS, 
                                       0, 
                                       &hProcessParams, 
                                       SER_SENDMSG_PSIZE );

	if ( psStatus == psProcessFailed ) 
	{
		m_dcDetectCode  = dcNeedMoreInfo;
		m_hProtocoller  = NULL;
		err = errOBJECT_NOT_FOUND;
	}

	return err;
}
