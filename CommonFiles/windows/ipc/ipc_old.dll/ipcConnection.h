// ipcConnection.h ///////////////////////////////////////////////////////////

// Interprocess Communication Connection
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_CONNECTION_H
#define IPC_CONNECTION_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#include "IPCBuffer.h"

//////////////////////////////////////////////////////////////////////////////

class C_IPC_CONNECTION : public C_IPC_BUFFER
{
public:
	C_IPC_CONNECTION();
	~C_IPC_CONNECTION();

	bool Create( char *pszConnectionName, char *pszLinkName, SECURITY_ATTRIBUTES *pSA );
	bool Open( char *pszConnectionName, char *pszLinkName, SECURITY_ATTRIBUTES *pSA );
	bool Close();

	DWORD Recv( BYTE *pbyBuf, DWORD dwBufSize, DWORD dwTimeout );
	DWORD Send( BYTE *pbyBuf, DWORD dwBufSize, DWORD dwTimeout );

	bool SetUserEvent( HANDLE *pUserEvent );
	bool GetUserEventRes( DWORD *pdwUserEvent ) const;
	bool ResetUserEvent() const;

	DWORD GetLastErr();
	void SetLastErr( DWORD dwError );

private:
	bool IsValidConnectionName( char * pszConnectionName ) const;
	HANDLE GetUserEventHandle() const;
	bool SetUserEventRes( DWORD dwUserEventRes );

	//////////////////////////////////////////////

	// objects add names

	static const char	*const IPC_ADD_NAME_MAPFILE_BUFFER;

	static const char	*const IPC_ADD_NAME_BREAK;

	static const char	*const IPC_ADD_NAME_SERVER_SEND_BUSY;
	static const char	*const IPC_ADD_NAME_SERVER_SEND_NEW;
	static const char	*const IPC_ADD_NAME_SERVER_SEND;
	static const char	*const IPC_ADD_NAME_SERVER_RECV_BUSY;
	static const char	*const IPC_ADD_NAME_SERVER_RECV;

	static const char	*const IPC_ADD_NAME_CLIENT_SEND_BUSY;
	static const char	*const IPC_ADD_NAME_CLIENT_SEND_NEW;
	static const char	*const IPC_ADD_NAME_CLIENT_SEND;
	static const char	*const IPC_ADD_NAME_CLIENT_RECV_BUSY;
	static const char	*const IPC_ADD_NAME_CLIENT_RECV;

	static const char	*const IPC_ADD_NAME_SERVER_LINK;
	static const char	*const IPC_ADD_NAME_CLIENT_LINK;

	//////////////////////////////////////////////

	static const DWORD	IPC_MAX_SENR_AND_RECV_BUF_SIZE;

	//////////////////////////////////////////////

	bool				m_boInited;

	char				m_szConnectionName[ _MAX_PATH ];

	HANDLE				m_hBreak;

	HANDLE				m_hServerSendBusy;
	HANDLE				m_hServerSendNew;
	HANDLE				m_hServerSend;
	HANDLE				m_hServerRecvBusy;
	HANDLE				m_hServerRecv;

	HANDLE				m_hClientSendBusy;
	HANDLE				m_hClientSendNew;
	HANDLE				m_hClientSend;
	HANDLE				m_hClientRecvBusy;
	HANDLE				m_hClientRecv;

	HANDLE				m_hUserEventBusy;
	HANDLE				m_hUserEvent;
	DWORD				m_dwUserEventRes;

	HANDLE				m_hLink;

	CRITICAL_SECTION	m_csLastError;
	DWORD				m_dwLastError;
};

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_CONNECTION_H

// eof ///////////////////////////////////////////////////////////////////////