// ipc.cpp ///////////////////////////////////////////////////////////////////

// Interprocess Communication (IPC)
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#define	IPC_API	__declspec(dllexport)
#include "ipc.h"
#include "ipc32.h"

//////////////////////////////////////////////////////////////////////////////

static C_IPC32 g_ipc32;

//////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain( HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved )
{	if ( fdwReason == DLL_PROCESS_ATTACH )
	{	if ( ! DisableThreadLibraryCalls( hinstDll ) || ! g_ipc32.IsValid() )
			return FALSE;
	}
	return TRUE;
}

IPC_API DWORD __stdcall IPC_GetVersion()
{ return g_ipc32.GetVersion(); }

IPC_API HIPCSERVER __stdcall IPC_ServerStart( char *pszServerName )
{ return g_ipc32.ServerStart( pszServerName ); }

IPC_API BOOL __stdcall IPC_ServerStop( HIPCSERVER hServer )
{ return g_ipc32.ServerStop( hServer ); }

IPC_API HIPCCONNECTION __stdcall IPC_ServerWaitForConnection( HIPCSERVER hServer, DWORD	dwTimeout )
{ return g_ipc32.ServerWaitForConnection( hServer, dwTimeout ); }

IPC_API HIPCCONNECTION __stdcall IPC_Connect( char *pszServerName, DWORD dwTimeout )
{ return g_ipc32.Connect( pszServerName, dwTimeout ); }

IPC_API BOOL __stdcall IPC_CloseConnection( HIPCCONNECTION hConnection )
{ return g_ipc32.CloseConnection( hConnection ); }

IPC_API DWORD __stdcall IPC_Recv( HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
{ return g_ipc32.Recv( hConnection, pvBuf, dwBufSize, dwTimeout ); }

IPC_API DWORD __stdcall IPC_Send( HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
{ return g_ipc32.Send( hConnection, pvBuf, dwBufSize, dwTimeout ); }

IPC_API BOOL __stdcall IPC_SetEvents( HIPCCONNECTION hConnection, HANDLE *pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents )
{ return g_ipc32.SetUserEvent( hConnection, pUserEvents ); }

IPC_API BOOL __stdcall IPC_GetEvents( HIPCCONNECTION hConnection, DWORD *pdwUserEvents, DWORD *pdwIPCEvents )
{ return g_ipc32.GetUserEvent( hConnection, pdwUserEvents ); }

IPC_API BOOL __stdcall IPC_ResetEvents( HIPCCONNECTION hConnection )
{ return g_ipc32.ResetUserEvent( hConnection ); }

//////////////////////////////////////////////////////////////////////////////

IPC_API DWORD __stdcall IPC_GetConnectionLastErr( HIPCCONNECTION hConnection )
{ return g_ipc32.GetConnectionLastErr( hConnection ); }

//////////////////////////////////////////////////////////////////////////////

// not implemented functions
IPC_API HIPCSERVER __stdcall IPC_ServerDgStart( char *pszDgServerName )
{ return IPC_RC_ERROR; }

IPC_API BOOL __stdcall IPC_ServerDgStop( HIPCSERVER hServer )
{ return FALSE; }

IPC_API DWORD __stdcall IPC_DgRecv( char *pszDgServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
{ return IPC_RC_ERROR; }

IPC_API DWORD __stdcall IPC_DgSend( char *pszDgServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
{ return IPC_RC_ERROR; }

IPC_API void __stdcall IPC_Init()
{}

IPC_API void __stdcall IPC_Done()
{}

// eof ///////////////////////////////////////////////////////////////////////