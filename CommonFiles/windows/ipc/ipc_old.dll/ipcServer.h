// ipcServer.h ///////////////////////////////////////////////////////////////

// Interprocess Communication Server
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_SERVER_H
#define IPC_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#include "IPCBuffer.h"
#include "IPCConnection.h"

//////////////////////////////////////////////////////////////////////////////

class C_IPC_SERVER : public C_IPC_BUFFER
{
public:
	C_IPC_SERVER();

	bool Create( char *pszServerName, SECURITY_ATTRIBUTES *pSA );
	bool Close();

	C_IPC_CONNECTION * WaitForConnection( DWORD dwTimeout, SECURITY_ATTRIBUTES *pSA );
	C_IPC_CONNECTION * Connect( char *pszServerName, DWORD dwTimeout, SECURITY_ATTRIBUTES *pSA );

private:
	bool IsValidServerName( char *pszServerName ) const;
	bool Open( char *pszServerName );

	//////////////////////////////////////////////

	// objects add names

	static const char	*const IPC_ADD_NAME_MAPFILE_BUFFER;

	static const char	*const IPC_ADD_NAME_BREAK;

	static const char	*const IPC_ADD_NAME_WAIT_BUSY;
	static const char	*const IPC_ADD_NAME_CONNECT_BUSY;

	static const char	*const IPC_ADD_NAME_CONNECT_NEW;

	static const char	*const IPC_ADD_NAME_SEND;
	static const char	*const IPC_ADD_NAME_RECV;

	//////////////////////////////////////////////

	bool	m_boInited;

	HANDLE	m_hBreak;

	HANDLE	m_hWaitBusy;
	HANDLE	m_hConnectBusy;

	HANDLE	m_hConnectNew;

	HANDLE	m_hSend;
	HANDLE	m_hRecv;
};

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_SERVER_H

// eof ///////////////////////////////////////////////////////////////////////