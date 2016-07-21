#include "stdafx.h"
#include "sock_util.h"
#include "util.h"

int
SockInitialize()
{
    WORD wVersionRequested = MAKEWORD( 2, 2 );
    WSADATA wsaData;
    int nRet;

    nRet = WSAStartup( wVersionRequested, &wsaData );

    if ( nRet != 0 )
    {
        return -1 ;
    }

    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
    {        
        WSACleanup( );
        return -1; 
    }
    return 0;
}

void
SockDone()
{
    WSACleanup();
}

void
SetBlocking( SOCKET sock, bool block )
{
    u_long   on = !block;
    ioctlsocket( sock, FIONBIO, &on );
}

bool 
CheckPort(unsigned short nPort)
{   
    SOCKET sock = ServerSocket( nPort, PF_INET );

    if ( sock != INVALID_SOCKET )
    {
        closesocket( sock );
        return true;
    }
    
    return false;	
}


bool 
ScanPorts( USHORT nStartPort, USHORT nRange, USHORT &nResultPort)
{
    for( USHORT i = nStartPort; i <= nStartPort + nRange; i++ )	
    {
        if( CheckPort(i) )
        {
            nResultPort = i;            
            return true;
        }
    }

    return false;
}

cIP 
GetServerIP(const char* szServerName)
{
    cIP ip;
    addrinfo* res;

    int nres = getaddrinfo(szServerName, NULL, NULL, &res);
    if ((WSATRY_AGAIN == nres) || (WSA_NOT_ENOUGH_MEMORY == nres))
        nres = getaddrinfo(szServerName, NULL, NULL, &res);
    if (!nres)
    {
        addrinfo* restmp = res;
        while (restmp)
        {
            if (restmp->ai_family == PF_INET)
            {
                sockaddr_in *paddr;
                paddr = (sockaddr_in*) restmp->ai_addr;
                
                ip.SetV4(htonl(paddr->sin_addr.S_un.S_addr));                
                break;
            }
            else if (restmp->ai_family == PF_INET6)
            {
                sockaddr_in6 *paddr;
                paddr = (sockaddr_in6*) restmp->ai_addr;

                cIPv6 ipv6;
                for (int i = 0; i < 16; ++i)
                    ipv6.m_Bytes[0] = paddr->sin6_addr.u.Byte[16-i];
                
                ip.SetV6(ipv6);    
                break;
            }

            restmp = restmp->ai_next;
        }

        freeaddrinfo(res);
    }

    return ip;
}

SOCKET
ServerSocket( unsigned short Port, int Family )
{
    SOCKET sock = INVALID_SOCKET;
    int RetVal;
    ADDRINFO Hints, *AddrInfo;

    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family = Family;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
    u_long on = 1;

    char PortVal[20];    

    do 
    {
        RetVal = getaddrinfo( NULL, _itoa( Port, PortVal, 10 ), &Hints, &AddrInfo);

        if ( RetVal == ERROR_SUCCESS )
        {
            sock = socket( AddrInfo->ai_family, AddrInfo->ai_socktype, AddrInfo->ai_protocol );

            if ( sock == INVALID_SOCKET )
                break;

            RetVal = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof (on));
            if ( RetVal != ERROR_SUCCESS)
            {
                KLSTD_TRACE1(KLMC_TRACELEVEL, "ServerSocket => unable to setsockopt err = %d\n", WSAGetLastError() );
                closesocket( sock );
                sock = INVALID_SOCKET;
                break;
            }

            RetVal = bind( sock, AddrInfo->ai_addr, AddrInfo->ai_addrlen );
            if ( RetVal != ERROR_SUCCESS )
            {
                KLSTD_TRACE2(KLMC_TRACELEVEL, "ServerSocket => unable to bind to %d port. err = %d\n", Port, WSAGetLastError() );
                closesocket( sock );
                sock = INVALID_SOCKET;
                break;
            }

            SetBlocking( sock, true );

            RetVal = listen ( sock, SOMAXCONN );

            if ( RetVal != ERROR_SUCCESS )
            {
                KLSTD_TRACE2(KLMC_TRACELEVEL, "ServerSocket => unable to listen to %d port. err = %d\n", Port, WSAGetLastError() );
                closesocket( sock );
                sock = INVALID_SOCKET;
                break;
            }
        }
        else
        {
            KLSTD_TRACE2(KLMC_TRACELEVEL, "ServerSocket => unable to getaddrinfo for %d port. err = %d\n", 
                Port, WSAGetLastError() );
        }
    } while (FALSE);

    freeaddrinfo( AddrInfo );
    

    return sock;
}

SOCKET
ServerConnect( SOCKET& conn, const char* szHostAddr, const char* szPort )
{
    int RetVal;
    ADDRINFO Hints, *AddrInfo, *AI;
    conn = INVALID_SOCKET;    

    // By not setting the AI_PASSIVE flag in the hints to getaddrinfo, we're
    // indicating that we intend to use the resulting address(es) to connect
    // to a service.
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family     = PF_UNSPEC;
    Hints.ai_socktype   = SOCK_STREAM;
    Hints.ai_flags      = AI_NUMERICHOST;
    Hints.ai_protocol   = 6;
    RetVal = getaddrinfo( szHostAddr, szPort, &Hints, &AddrInfo); // сюда выполняется коннект

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

    RetVal = connect( conn, AddrInfo->ai_addr, AddrInfo->ai_addrlen );

    freeaddrinfo(AddrInfo);

    return conn;
}

bool
GetLocalName( SOCKET sock, char* AddrName, int AddrNameSize )
{
    SOCKADDR_STORAGE    Addr;
    int AddrLen = sizeof(Addr);
    if ( ERROR_SUCCESS != getsockname( sock, (LPSOCKADDR)&Addr, &AddrLen ) )
    {
        KLSTD_TRACE2 (KLMC_TRACELEVEL, "GetLocalName => unable to getpeername for sock(0x%x). err = %d\n", sock, WSAGetLastError() );
        return false;
    }
    else
    {
        if ( 0 != getnameinfo((LPSOCKADDR)&Addr, AddrLen, AddrName, AddrNameSize, NULL, 0, NI_NUMERICHOST ) )
        {
            KLSTD_TRACE2 (KLMC_TRACELEVEL, "GetLocalName => unable to getnameinfo for sock(0x%x). err = %d\n", sock, WSAGetLastError() );
            strcpy( AddrName, "0.0.0.0" );
        }
        else
        {
            KLSTD_TRACE1 (KLMC_TRACELEVEL, "GetLocalName => localname is %s\n", AddrName );
        }
    }

    return true;
}

int
SendAndTrace( SOCKET sock, const char* buf, int size, bool to_client )
{
	KLSTD_TRACE3(prtNOTIFY, "Sending data (length = %d) to %s:\r\n%s\n", 
		size,
		to_client ? "client" : "server",
		GetBinBufferTraceString((const BYTE *)buf, size).c_str());
	int res = ::send( sock, buf, size, 0 );
	if(res == SOCKET_ERROR)
	{
		KLSTD_TRACE1(prtNOTIFY, "Sending failed with %d", WSAGetLastError());
	}
    return res;
}


// Index - номер параметра по порядку
// DstStr - строка, куда копируется параметр
// SrcStr - HelloBuffer
char*
CR_GetParamStr( char* DstStr, char* SrcStr, ULONG Index)
{
    int i = 0;
    char* StartStr = SrcStr;

    while ( Index-- )
    {
        StartStr = strchr( StartStr, '<' ) + 1;
    }

    if ( !StartStr )
        return NULL;

    for ( i = 0; StartStr[i] != '>'; i++ )
    {
        DstStr[i] = StartStr[i];
    }

    DstStr[i] = 0;

    return DstStr;
}

