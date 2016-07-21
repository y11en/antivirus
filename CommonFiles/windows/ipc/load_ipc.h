// load_ipc.h ////////////////////////////////////////////////////////////////

// InterProcess Communication Library Loader
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_IPC_H
#define LOAD_IPC_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#include "ipc_def.h"

//////////////////////////////////////////////////////////////////////////////

typedef IPC_API	DWORD			(__stdcall * IPC_GET_VERSION)				();

typedef IPC_API HIPCSERVER		(__stdcall * IPC_SERVER_START)				(char *pszServerName);
typedef IPC_API BOOL			(__stdcall * IPC_SERVER_STOP)				(HIPCSERVER hServer);
typedef IPC_API HIPCCONNECTION	(__stdcall * IPC_SERVER_WAIT_FOR_CONNECTION)(HIPCSERVER hServer, DWORD dwTimeout);
typedef	IPC_API HIPCCONNECTION	(__stdcall * IPC_CONNECT)					(char *pszServerName, DWORD dwTimeout);
typedef	IPC_API BOOL			(__stdcall * IPC_CLOSE_CONNECTION)			(HIPCCONNECTION hConnection);
typedef IPC_API DWORD			(__stdcall * IPC_RECV)						(HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout);
typedef IPC_API DWORD			(__stdcall * IPC_SEND)						(HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout);

typedef	IPC_API BOOL			(__stdcall * IPC_SET_EVENTS)				(HIPCCONNECTION	hConnection, HANDLE * pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents);
typedef	IPC_API BOOL			(__stdcall * IPC_GET_EVENTS)				(HIPCCONNECTION	hConnection, DWORD * pdwUserEvents, DWORD *pdwIPCEvents);
typedef	IPC_API BOOL			(__stdcall * IPC_RESET_EVENTS)				(HIPCCONNECTION hConnection);

typedef IPC_API	DWORD			(__stdcall * IPC_GET_CONNECTION_LAST_ERR)	(HIPCCONNECTION hConnection);

typedef	IPC_API	HIPCSERVER		(__stdcall * IPC_SERVER_DG_START)			(char *pszServerName);
typedef	IPC_API	BOOL			(__stdcall * IPC_SERVER_DG_STOP)			(HIPCSERVER hServer);
typedef	IPC_API DWORD			(__stdcall * IPC_DG_RECV)					(char *pszServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout);
typedef	IPC_API DWORD			(__stdcall * IPC_DG_SEND)					(char *pszServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout);
typedef IPC_API void			(__stdcall * IPC_INIT)						();
typedef IPC_API void			(__stdcall * IPC_DONE)						();

//////////////////////////////////////////////////////////////////////////////

extern IPC_GET_VERSION					IPC_GetVersion;

extern IPC_SERVER_START					IPC_ServerStart;
extern IPC_SERVER_STOP					IPC_ServerStop;
extern IPC_SERVER_WAIT_FOR_CONNECTION	IPC_ServerWaitForConnection;
extern IPC_CONNECT						IPC_Connect;
extern IPC_CLOSE_CONNECTION				IPC_CloseConnection;
extern IPC_RECV							IPC_Recv;
extern IPC_SEND							IPC_Send;

extern IPC_SET_EVENTS					IPC_SetEvents;
extern IPC_GET_EVENTS					IPC_GetEvents;
extern IPC_RESET_EVENTS					IPC_ResetEvents;

extern IPC_GET_CONNECTION_LAST_ERR		IPC_GetConnectionLastErr;

extern IPC_SERVER_DG_START				IPC_ServerDgStart;
extern IPC_SERVER_DG_STOP				IPC_ServerDgStop;
extern IPC_DG_RECV						IPC_DgRecv;
extern IPC_DG_SEND						IPC_DgSend;
extern IPC_INIT							IPC_Init;
extern IPC_DONE							IPC_Done;

//////////////////////////////////////////////////////////////////////////////

bool IPC_LoadDLL();
bool IPC_FreeDLL();

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // LOAD_IPC_H

// eof ///////////////////////////////////////////////////////////////////////
