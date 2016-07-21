#include "transparentsocket.h"

#define SSL_CONNECT_TRIES 5

CTransparentSocket::CTransparentSocket(): 
	m_pSSLCtx(NULL),
	m_hConnection(NULL), 
	m_sock(INVALID_SOCKET), 
	m_ConnectionState(0), 
	m_bInRecv(false),
	m_nSelectRes(SOCKET_ERROR),
	m_nSystemError(0),
	m_ip(0),
	m_alive(true)
{    
};

int CTransparentSocket::Connect(const char* servername,const unsigned short port, bool bUseSSL) 
{
	m_nSystemError = 0;
	int i_connect = -1;

	if( !g_bSSLLoaded )
		return ESSL_NOTLOADED;

	if( m_sock == INVALID_SOCKET )
	{
		struct hostent *m_server = NULL;
		struct sockaddr_in remote;

		ZeroMemory( &remote, sizeof( struct sockaddr_in ) );
		        
        m_server = gethostbyname(servername);
		
        if( NULL == m_server )
		{
			m_nSystemError = WSAGetLastError();
			return ESSL_GETHOSTBYNAME;
		}
		
        remote.sin_family   = AF_INET;
        remote.sin_port     = htons( port );
		memmove((char*)&(remote.sin_addr.s_addr),m_server->h_addr,m_server->h_length);		

        m_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

		if( INVALID_SOCKET == m_sock )
		{
			m_nSystemError = WSAGetLastError();
			return ESSL_CREATESOCKET;
		}

		i_connect = connect( m_sock, (struct sockaddr*)&remote, sizeof(remote) );
		
        if ( SOCKET_ERROR == i_connect )
		{
			m_nSystemError = WSAGetLastError();
			return ESSL_SOCKETCONNECT;
		}
		else
		{
			m_name = servername;
			m_ip = *((unsigned long*)m_server->h_addr);
		}
	}

	m_ConnectionState = CONN_ESTABLISHED;

	if ( bUseSSL )
		SwitchToSSL();

	PrepareSocket();

	return i_connect;
}

bool CTransparentSocket::PrepareSocket()
{
	//////////////////////////////////////////////////////////////////////////
	//
	// Set blocking socket
	//
	//
	if ( m_sock == INVALID_SOCKET )
        return false;
	
	int iMode = 0;

	if ( 0 != ioctlsocket(m_sock, FIONBIO, (u_long FAR*) &iMode) )
	{
		m_nSystemError = WSAGetLastError();
		return false;
	}

	if ( m_ConnectionState == CONN_SSL_ESTABLISHED )
	{
		SSL_set_read_ahead(m_hConnection, 1);
	}
	return true;
	
	//
	// Set blocking socket
	//
	//
	//////////////////////////////////////////////////////////////////////////
	
}


bool CTransparentSocket::AssignSocket(int sock, char* name, unsigned long ip)
{
	if ( m_sock == INVALID_SOCKET && m_ConnectionState == CONN_NO )
	{
		if ( name ) 
            m_name = name;

		m_ip                = ip;
		m_sock              = sock;
		m_ConnectionState   = CONN_ESTABLISHED;

		return true;
	}
	return false;
}

int __cdecl sslContext_password_cb( char *buf, int num, int, void * )
{
	int size = 7;
	if ( num < size+1 )
        return 0;
	strcpy(buf,"123456");
	return size;
}

#define ISCLIENTHELLO_v2()	\
    ( (nRecv >= 2) && (buf[0] & 0x80) && (buf[2] == 1) )
#define ISCLIENTHELLO_v3(x0,x1,x2)	\
    (	\
    (nRecv >= 5) &&	\
    buf[0] == (char) 0x##x0 &&	\
    buf[1] == (char) 0x##x1	&&	\
    buf[2] == (char) 0x##x2	&&	\
    buf[5] == 1					\
    )
bool CTransparentSocket::HasSSLHandshake()
{
	if ( m_ConnectionState == CONN_ESTABLISHED )
	{
		char buf[256] = {0};
		int buflen = 255;
		int nRecv = recv(m_sock, buf, buflen, MSG_PEEK);

		if ( nRecv )
		{			
			if ( 
				ISCLIENTHELLO_v2()			||	// SSLv2 - check 1st bit from 1st bite and 3d bite
				ISCLIENTHELLO_v3(16,03,00)	||	// SSLv3 - check 1-3 bytes
				ISCLIENTHELLO_v3(16,03,01)	||	// TLSv1 - check 1-3 bytes
				ISCLIENTHELLO_v3(16,03,02)		// TLSv2 - check 1-3 bytes
				)
				return true;
		}
	}
	return false;
}

int SSL_CONNECT(SSL *ssl)
{
	for ( int i=0; i < SSL_CONNECT_TRIES; i++ )
	{
		__try { return SSL_connect(ssl); }
		__except (1) {};
	}
	return -1;
}

#define SSL_SHUTDOWN(_ssl_)	SSL_set_shutdown(_ssl_, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN)	// or SSL_shutdown(_ssl_)

#define CHECK_STOP_EVENT(_hStopEvent)	                            \
{	                                                                \
	if ( _hStopEvent && _hStopEvent != INVALID_HANDLE_VALUE )	    \
	{	                                                            \
		if ( WAIT_TIMEOUT != WaitForSingleObject(_hStopEvent, 10) )	\
			break;	                                                \
	}	                                                            \
	else	                                                        \
		Sleep(10);	                                                \
}


bool CTransparentSocket::SwitchToSSL(bool bAsServer, IN OUT certificate_info_t* certificate_info, HANDLE hStopEvent, const char* szCacheDir)
{
	if ( m_ConnectionState == CONN_ESTABLISHED && LoadSSLLibrary() )
	{
		if ( !bAsServer )
		{
			//	OpenSSL_add_all_algorithms();
			m_pSSLCtx = SSL_CTX_new(SSLv23_client_method());	//SSL3 & TLS1 used
			SSL_CTX_set_options(m_pSSLCtx,SSL_OP_NO_SSLv2);
		}
		else
		{
			m_pSSLCtx = SSL_CTX_new(SSLv23_server_method());	//SSL2,3 & TLS1 used
		}

		int i = 0;
		if ( bAsServer )
		{
			int i = 0;
			RSA* pRSA = get_RSA(szCacheDir);
			i = SSL_CTX_use_RSAPrivateKey(m_pSSLCtx, pRSA);
			X509* pX509 = create_certificate(
				pRSA, 
				certificate_info ? certificate_info->szName.c_str() : NULL, 
				certificate_info ? certificate_info->szOrgName.c_str() : NULL, 
				certificate_info ? certificate_info->start_time : NULL, 
				certificate_info ? certificate_info->end_time: NULL,
				szCacheDir
				);
			i = SSL_CTX_use_certificate(m_pSSLCtx, pX509);
			i = SSL_CTX_check_private_key(m_pSSLCtx);
			std::string szFileName;
			save_certificate(NULL, pX509, szCacheDir, certificate_info ? certificate_info->szFileName : szFileName);
		}
		if( NULL == (m_hConnection = SSL_new(m_pSSLCtx)) )
			return false;
		if( SSL_set_fd(m_hConnection,m_sock) < 1 )
			return false;
		int i_ssl_connect = 0;
		if ( !bAsServer )
		{
			SSL_set_connect_state(m_hConnection);
			while ( 
				0 > (i_ssl_connect = SSL_CONNECT(m_hConnection)) &&
				(
				SSL_ERROR_WANT_READ == (m_nSystemError = SSL_get_error(m_hConnection, i_ssl_connect)) ||
				SSL_ERROR_WANT_WRITE == m_nSystemError
				)) 
				CHECK_STOP_EVENT(hStopEvent);
			if ( i_ssl_connect == 1 )
			{
				// Надо проверить сертификат сервера
				std::string szFileName;
				save_certificate(m_hConnection, NULL, szCacheDir, certificate_info ? certificate_info->szFileName : szFileName);
				long lCertCheck = post_connection_check(
					m_hConnection, 
					m_name.length() ? m_name.c_str() : 0, 
					m_ip, 
					certificate_info ? certificate_info->szFileName.c_str() : szFileName.c_str()
					);
				if ( certificate_info )
					certificate_info->valid = (X509_V_OK == lCertCheck);
			}
		}
		else
		{
			SSL_set_accept_state(m_hConnection);
			while ( 
				0 > (i_ssl_connect = SSL_accept(m_hConnection)) &&
				(
				SSL_ERROR_WANT_READ == (m_nSystemError = SSL_get_error(m_hConnection, i_ssl_connect)) ||
				SSL_ERROR_WANT_WRITE == m_nSystemError
				)) 
				CHECK_STOP_EVENT(hStopEvent);
		}
		if( i_ssl_connect < 1 )
		{
			m_nSystemError = SSL_get_error(m_hConnection, i_ssl_connect);
			m_ConnectionState = CONN_ESTABLISHED;
			return false;
		}
		else
		{
			PrepareSocket();
			m_ConnectionState = CONN_SSL_ESTABLISHED;
			if ( !bAsServer && certificate_info ) GetCertificateInfo(*certificate_info);
			return true;
		}
	}
	else if ( m_ConnectionState == CONN_SSL_ESTABLISHED )
		return true;
	return false;
}

bool CTransparentSocket::SwitchToClean()
{
	if ( m_ConnectionState == CONN_SSL_ESTABLISHED )
	{
		if( m_hConnection )
		{
			SSL_SHUTDOWN( m_hConnection );
			SSL_free( m_hConnection );
			m_hConnection = NULL;
		}
		if( m_pSSLCtx )
		{
			SSL_CTX_free( m_pSSLCtx );
			m_pSSLCtx = NULL;
		}
		m_ConnectionState = CONN_ESTABLISHED;
		return true;
	}
	else if ( m_ConnectionState == CONN_ESTABLISHED )
		return true;
	return false;
}


int CTransparentSocket::Send(const char *query, int len, bool to_client)
{
	int Sent = 0;
	if ( query == NULL )
		return ESSL_SEND;

	switch ( m_ConnectionState )
	{
	case CONN_SSL_ESTABLISHED:
		Sent = SSL_write(m_hConnection, (PVOID)query, len);

		if ( Sent != len )
		{
            m_nSystemError = SSL_get_error( m_hConnection, Sent );
			
            switch ( m_nSystemError ) 
			{
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				// m_nSystemError = WSAEWOULDBLOCK;
				return SOCKET_ERROR;
				break;
			};
		}
		break;
	case CONN_ESTABLISHED:

		Sent = SendAndTrace( m_sock, query, len, to_client );
        
		if ( Sent != len )
		{
			// m_nSystemError = WSAGetLastError();
			return SOCKET_ERROR;
		}
		break;
	default: // CONN_NO
		return ESSL_SEND;
	}
	return Sent;
}

int CTransparentSocket::Recv(char *buf,int buflen)
{
	int nRecv = 0;

	switch ( m_ConnectionState ) 
	{
	case CONN_SSL_ESTABLISHED:
		nRecv = SSL_read(m_hConnection,buf,buflen);
		m_bInRecv = (0 != SSL_pending(m_hConnection));

		if( nRecv < 1 )
		{
			switch (m_nSystemError = SSL_get_error(m_hConnection,nRecv)) 
			{
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				m_nSystemError = WSAEWOULDBLOCK;
				return SOCKET_ERROR;
				break;
			};
		}
		break;
	case CONN_ESTABLISHED:
		nRecv = recv(m_sock, buf, buflen, 0);
        if ( nRecv == SOCKET_ERROR  )
			m_nSystemError = WSAGetLastError();
		break;
	default: // CONN_NO
		return SOCKET_ERROR;
	}

	return nRecv;
}

int CTransparentSocket::Select(long sec, long msec )
{
	if ( m_ConnectionState == CONN_ESTABLISHED || !m_bInRecv )
	{
		struct timeval tval;
		tval.tv_sec = sec;     
		tval.tv_usec = msec;
		
		fd_set readfd;
		FD_ZERO(&readfd);
		FD_SET(m_sock, &readfd);
		
        m_nSelectRes = select( m_sock, &readfd, NULL, NULL, &tval );

		if ( m_nSelectRes == SOCKET_ERROR )
			m_nSystemError = WSAGetLastError();
	}
	else if ( m_ConnectionState == CONN_SSL_ESTABLISHED )
	{
		m_nSelectRes = SSL_pending(m_hConnection);
	}

	if ( m_nSelectRes == 0 )
		m_bInRecv = false;

	return m_nSelectRes;
}

CTransparentSocket::~CTransparentSocket()
{
	Disconnect();    
}

void CTransparentSocket::Disconnect()
{
	if ( INVALID_SOCKET != m_sock )
	{
        // shutdown( m_sock, 2 );
		CLOSESOCKET( m_sock );

		if( m_hConnection )
		{
			SSL_SHUTDOWN( m_hConnection );
			SSL_free( m_hConnection );
			m_hConnection = NULL;
		}
		if( m_pSSLCtx )
		{
			SSL_CTX_free( m_pSSLCtx );
			m_pSSLCtx = NULL;
		}
		m_sock = INVALID_SOCKET;
		m_ConnectionState = CONN_NO;
	}
}

int CTransparentSocket::GetLastError() const
{
	return m_nSystemError;
}

bool CTransparentSocket::GetCertificateInfo(IN OUT certificate_info_t& certificate_info)
{
	if ( ( m_ConnectionState != CONN_SSL_ESTABLISHED ) || !m_hConnection )
		return false;

	bool bRes = false;
	char data[256] = {0};
	X509* x509 = SSL_get_peer_certificate(m_hConnection);
	if ( get_certificate_org_name(x509, data, 256) )
	{
		certificate_info.szOrgName = data;
		bRes = true;
	}

	memset(data, 0, 256);
	if ( get_certificate_name(x509, data, 256) )
	{
		certificate_info.szName = data;
		bRes = true;
	}

	certificate_info.start_time = X509_get_notBefore(x509);
	certificate_info.end_time = X509_get_notAfter(x509);
	return bRes;
}
