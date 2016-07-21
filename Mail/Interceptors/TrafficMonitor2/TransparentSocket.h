#ifndef _TRANSPARENTSOCKET_H
#define _TRANSPARENTSOCKET_H

#include <windows.h>
#include <string>
#include "ssl_util.h"
#include "sock_util.h"

#pragma warning( disable : 4290 )
#pragma warning( disable : 4002 )

struct certificate_info_t
{
	std::string szName;
	std::string szOrgName;
	ASN1_TIME* start_time;
	ASN1_TIME* end_time;
	bool valid;
	std::string szFileName;

	certificate_info_t(): start_time(NULL), end_time(NULL), valid(true) {};
};

class CTransparentSocket
{
public:
	CTransparentSocket();
	virtual ~CTransparentSocket();
	
	int Connect(const char* servername,const unsigned short port, bool bUseSSL = false);
	bool AssignSocket(int sock, char* name = 0, unsigned long ip = 0);
	bool HasSSLHandshake();
	bool SwitchToSSL(
		IN bool bAsServer = false, 
		IN OUT certificate_info_t* certificate_info = NULL,
		IN HANDLE hStopEvent = NULL, 
		IN const char* szCacheDir = NULL);
	bool SwitchToClean();
	bool PrepareSocket();

	int Send(const char *query, int len, bool to_client);
	int Select(long sec = 0, long msec = 50000);
	int Recv(char *buf, int buflen);
	void Disconnect();
	
	int GetLastError() const;
	inline int GetConnectionState() const { return m_ConnectionState; }
	inline bool GetInRecvState() const { return m_bInRecv; }
	inline int Socket() const { return m_sock; }
    
	bool GetCertificateInfo(IN OUT certificate_info_t& certificate_info);

	inline bool operator==(int sock){ return m_sock == sock; }
	inline bool operator!=(int sock){ return m_sock != sock; }
	inline bool operator==(CTransparentSocket& sock){ return m_sock == sock.Socket(); }
	inline bool operator!=(CTransparentSocket& sock){ return m_sock != sock.Socket(); }

	bool is_alive() const
	{
		return m_alive;
	}

	void set_alive(bool alive)
	{
		m_alive = alive;
	}

protected:
	SSL_CTX*        m_pSSLCtx;
	SSL*            m_hConnection;
	int             m_ConnectionState; // CONN_xxx
	bool            m_bInRecv;
	int             m_nSystemError;
	int             m_nSelectRes;
	
	unsigned long   m_ip;
	std::string     m_name;
	int             m_sock;    
	bool			m_alive;
};

enum 
{
	CONN_NO = 0,
	CONN_ESTABLISHED = 1,
	CONN_SSL_ESTABLISHED = 2,
};

enum
{
	ESSL_NOTLOADED = -100,	//OpenSSL is not loaded
	ESSL_WINSOCKINIT,		//WinSock 2.0 init failed
	ESSL_GETHOSTBYNAME,		//DNS error
	ESSL_CREATESOCKET,		//error creating socket
	ESSL_SOCKETCONNECT,		//error connecting with socket
	ESSL_CREATESSL,			//error creating SSL session structure
	ESSL_SETSOCKET,			//error connect socket with SSL session for bidirect I/O space
	ESSL_CONNECT,			//cannot connect to m_server
	ESSL_SEND,				//cannot send data
	ESSL_RECV,				//cannot receive data
	ESSL_RECVALLOC,			//cannot allocate memory for received data
};

#endif//_TRANSPARENTSOCKET_H