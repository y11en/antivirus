// ipc.h /////////////////////////////////////////////////////////////////////

// Interprocess Communication (IPC)
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_H
#define IPC_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#include "ipc_def.h"

//////////////////////////////////////////////////////////////////////////////

	IPC_API DWORD __stdcall				// [ 1, 2, ... IPC_RC_ERROR ]
IPC_GetVersion();

//////////////////////////////////////////////////////////////////////////////

	IPC_API HIPCSERVER __stdcall		// [ 1, 2, ... , IPC_RC_INVALID_HANDLE ]
IPC_ServerStart(
	char			*pszServerName );

	IPC_API BOOL __stdcall
IPC_ServerStop(
	HIPCSERVER		hServer );

	IPC_API HIPCCONNECTION __stdcall	// [ 1, 2, ... , IPC_RC_TIMEOUT, IPC_RC_INVALID_HANDLE ]
IPC_ServerWaitForConnection(
	HIPCSERVER		hServer,
	DWORD			dwTimeout );		// [ 0, 1, ... , IPC_TIMEOUT_INFINITE ]

	IPC_API HIPCCONNECTION __stdcall	// [ 1, 2, ... , IPC_RC_TIMEOUT, IPC_RC_INVALID_HANDLE ]
IPC_Connect(
	char			*pszServerName,
	DWORD			dwTimeout );		// [ 0, 1, ... , IPC_TIMEOUT_INFINITE ]

	IPC_API BOOL __stdcall
IPC_CloseConnection(
	HIPCCONNECTION	hConnection );

	IPC_API DWORD __stdcall				// [ 0, 1, ... , IPC_RC_TIMEOUT, IPC_RC_ERROR ]
IPC_Recv(
	HIPCCONNECTION	hConnection,
	void			*pvBuf,
	DWORD			dwBufSize,
	DWORD			dwTimeout );		// [ 0, 1, ... , IPC_TIMEOUT_INFINITE ]

	IPC_API DWORD __stdcall				// [ 0, 1, ... , IPC_RC_TIMEOUT, IPC_RC_ERROR ]
IPC_Send(
	HIPCCONNECTION	hConnection,
	void			*pvBuf,
	DWORD			dwBufSize,
	DWORD			dwTimeout );		// [ 0, 1, ... , IPC_TIMEOUT_INFINITE ]

//////////////////////////////////////////////////////////////////////////////

	IPC_API BOOL __stdcall
IPC_SetEvents(
	HIPCCONNECTION	hConnection,
	HANDLE			*pUserEvents,
	DWORD			dwUserEventsCount,
	HANDLE			*pIPCEvents );

	IPC_API BOOL __stdcall
IPC_GetEvents(
	HIPCCONNECTION	hConnection,
	DWORD			*pdwUserEvents,
	DWORD			*pdwIPCEvents );

	IPC_API BOOL __stdcall
IPC_ResetEvents(
	HIPCCONNECTION	hConnection );

//////////////////////////////////////////////////////////////////////////////

	IPC_API DWORD __stdcall
IPC_GetConnectionLastErr(
	HIPCCONNECTION	hConnection );

//////////////////////////////////////////////////////////////////////////////

// NOT IMPLEMENTED FUNCTIONS
IPC_API HIPCSERVER	__stdcall IPC_ServerDgStart( char * pszDgServerName );
IPC_API BOOL		__stdcall IPC_ServerDgStop( HIPCSERVER hServer );
IPC_API DWORD		__stdcall IPC_DgRecv( char *pszDgServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout );
IPC_API DWORD		__stdcall IPC_DgSend( char *pszDgServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout );
IPC_API void		__stdcall IPC_Init();
IPC_API void		__stdcall IPC_Done();

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_H

// eof ///////////////////////////////////////////////////////////////////////