// load_ipc.cpp //////////////////////////////////////////////////////////////

// InterProcess Communication Loader
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "load_ipc.h"

//////////////////////////////////////////////////////////////////////////////

DWORD			__stdcall IPC_StubGetVersion				() {return -1;}

HIPCSERVER		__stdcall IPC_StubServerStart				(char *pszServerName) {return 0;}
BOOL			__stdcall IPC_StubServerStop				(HIPCSERVER hServer) {return FALSE;}
HIPCCONNECTION	__stdcall IPC_StubServerWaitForConnection	(HIPCSERVER hServer, DWORD dwTimeout) {return 0;}
HIPCCONNECTION	__stdcall IPC_StubConnect					(char *pszServerName, DWORD dwTimeout) {return 0;}
BOOL			__stdcall IPC_StubCloseConnection			(HIPCCONNECTION hConnection) {return FALSE;}
DWORD			__stdcall IPC_StubRecv						(HIPCCONNECTION	hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout) {return IPC_RC_ERROR;}
DWORD			__stdcall IPC_StubSend						(HIPCCONNECTION	hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout) {return IPC_RC_ERROR;}

BOOL			__stdcall IPC_StubSetEvents					(HIPCCONNECTION	hConnection, HANDLE *pUserEvents, DWORD dwUserEventsResCount, HANDLE *pIPCEvents) {return FALSE;}
BOOL			__stdcall IPC_StubGetEvents					(HIPCCONNECTION	hConnection, DWORD *pdwUserEventsRes, DWORD *pdwIPCEventsRes) {return FALSE;}
BOOL			__stdcall IPC_StubResetEvents				(HIPCCONNECTION hConnection) {return FALSE;}

DWORD			__stdcall IPC_StubGetConnectionLastErr		(HIPCCONNECTION hConnection) {return IPC_RC_ERROR;}

HIPCSERVER		__stdcall IPC_StubServerDgStart				(char *pszServerName) {return 0;}
BOOL			__stdcall IPC_StubServerDgStop				(HIPCSERVER	hServer) {return FALSE;}
DWORD			__stdcall IPC_StubDgRecv					(char *pszServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout) {return IPC_RC_ERROR;}
DWORD			__stdcall IPC_StubDgSend					(char *pszServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout) {return IPC_RC_ERROR;}
void			__stdcall IPC_StubInit						() {}
void			__stdcall IPC_StubDone						() {}

//////////////////////////////////////////////////////////////////////////////

IPC_GET_VERSION					IPC_GetVersion				= IPC_StubGetVersion;

IPC_SERVER_START				IPC_ServerStart				= IPC_StubServerStart;
IPC_SERVER_STOP					IPC_ServerStop				= IPC_StubServerStop;
IPC_SERVER_WAIT_FOR_CONNECTION	IPC_ServerWaitForConnection	= IPC_StubServerWaitForConnection;
IPC_CONNECT						IPC_Connect					= IPC_StubConnect;
IPC_CLOSE_CONNECTION			IPC_CloseConnection			= IPC_StubCloseConnection;
IPC_RECV						IPC_Recv					= IPC_StubRecv;
IPC_SEND						IPC_Send					= IPC_StubSend;

IPC_SET_EVENTS					IPC_SetEvents				= IPC_StubSetEvents;
IPC_GET_EVENTS					IPC_GetEvents				= IPC_StubGetEvents;
IPC_RESET_EVENTS				IPC_ResetEvents				= IPC_StubResetEvents;

IPC_GET_CONNECTION_LAST_ERR		IPC_GetConnectionLastErr	= IPC_StubGetConnectionLastErr;

IPC_SERVER_DG_START				IPC_ServerDgStart			= IPC_StubServerDgStart;
IPC_SERVER_DG_STOP				IPC_ServerDgStop			= IPC_StubServerDgStop;
IPC_DG_RECV						IPC_DgRecv					= IPC_StubDgRecv;
IPC_DG_SEND						IPC_DgSend					= IPC_StubDgSend;
IPC_INIT						IPC_Init					= IPC_StubInit;
IPC_DONE						IPC_Done					= IPC_StubDone;

//////////////////////////////////////////////////////////////////////////////

#define	REG_AVPSHARED __TEXT( "Software\\KasperskyLab\\SharedFiles" )	// registry entry

static HINSTANCE IPC_g_hLib = 0;
static IPC_g_hLibLoadCount = 0;

//////////////////////////////////////////////////////////////////////////////

bool IPC_LoadDLL(const TCHAR * pszIpcDllPathName /* =0 */)
{//	__asm int 3
	if ( IPC_g_hLibLoadCount )
  {
		if ( IPC_g_hLibLoadCount == 0xffffffff ) return false;
		IPC_g_hLibLoadCount++;
		return true;
	}

	//////////////////////////////////////////////

	TCHAR szFullDLLPath[ _MAX_PATH ] = { 0 };

  if ( pszIpcDllPathName==0 || *pszIpcDllPathName==0 ||
    ( ( IPC_g_hLib = LoadLibraryEx( pszIpcDllPathName, 0, LOAD_WITH_ALTERED_SEARCH_PATH ) )==0 ) )
  {
	  if ( ! GetModuleFileName( 0, szFullDLLPath, _MAX_PATH - 1 ) ) return false;
	  *(_tcsrchr( szFullDLLPath, __TEXT('\\') ) + 1) = 0;
	  if ( lstrlen( szFullDLLPath ) + lstrlen( __TEXT( "ipc.dll" ) ) >= _MAX_PATH ) return false;
	  lstrcat( szFullDLLPath, __TEXT( "ipc.dll" ) );

	  IPC_g_hLib = LoadLibraryEx( szFullDLLPath, 0, LOAD_WITH_ALTERED_SEARCH_PATH );
	  if ( IPC_g_hLib == 0 )
	  {	
      HKEY hKey;
		  if ( RegOpenKey(HKEY_LOCAL_MACHINE, REG_AVPSHARED, &hKey) != ERROR_SUCCESS) return false;
		  DWORD dwSize = _MAX_PATH;
		  if ( RegQueryValueEx( hKey, __TEXT("Folder"), 0, 0, (LPBYTE)szFullDLLPath, &dwSize ) != ERROR_SUCCESS )
		  {
        RegCloseKey( hKey );
			  return false;
		  }
		  RegCloseKey( hKey );
		  DWORD dwLen = lstrlen( szFullDLLPath );
		  if ( dwLen >= _MAX_PATH ) return false;

		  //////////////////////////////////////////

		  if ( szFullDLLPath[ dwLen - 1 ] != __TEXT('\\') )
		  {
        if ( dwLen + lstrlen( __TEXT("\\ipc.dll")) < _MAX_PATH )
				  lstrcat( szFullDLLPath, __TEXT("\\ipc.dll") );
			  else return false;
		  }
		  else
		  {
        if ( dwLen + lstrlen( __TEXT("ipc.dll") ) < _MAX_PATH )
				  lstrcat( szFullDLLPath, __TEXT("ipc.dll") );
			  else return false;
		  }

		  IPC_g_hLib = LoadLibraryEx( szFullDLLPath, 0, LOAD_WITH_ALTERED_SEARCH_PATH );
		  if ( IPC_g_hLib == 0 ) return false;
	  }
  }

	//////////////////////////////////////////////

	if ( ! (IPC_GetVersion				= (IPC_GET_VERSION)					GetProcAddress(IPC_g_hLib, "IPC_GetVersion")))				IPC_GetVersion				= IPC_StubGetVersion;

	if ( ! (IPC_ServerStart				= (IPC_SERVER_START)				GetProcAddress(IPC_g_hLib, "IPC_ServerStart")))				IPC_ServerStart				= IPC_StubServerStart;
	if ( ! (IPC_ServerStop				= (IPC_SERVER_STOP)					GetProcAddress(IPC_g_hLib, "IPC_ServerStop")))				IPC_ServerStop				= IPC_StubServerStop;
	if ( ! (IPC_ServerWaitForConnection	= (IPC_SERVER_WAIT_FOR_CONNECTION)	GetProcAddress(IPC_g_hLib, "IPC_ServerWaitForConnection")))	IPC_ServerWaitForConnection	= IPC_StubServerWaitForConnection;
	if ( ! (IPC_Connect					= (IPC_CONNECT)						GetProcAddress(IPC_g_hLib, "IPC_Connect")))					IPC_Connect					= IPC_StubConnect;
	if ( ! (IPC_CloseConnection			= (IPC_CLOSE_CONNECTION)			GetProcAddress(IPC_g_hLib, "IPC_CloseConnection")))			IPC_CloseConnection			= IPC_StubCloseConnection;
	if ( ! (IPC_Recv					= (IPC_RECV)						GetProcAddress(IPC_g_hLib, "IPC_Recv")))					IPC_Recv					= IPC_StubRecv;
	if ( ! (IPC_Send					= (IPC_SEND)						GetProcAddress(IPC_g_hLib, "IPC_Send")))					IPC_Send					= IPC_StubSend;

	if ( ! (IPC_SetEvents				= (IPC_SET_EVENTS)					GetProcAddress(IPC_g_hLib, "IPC_SetEvents")))				IPC_SetEvents				= IPC_StubSetEvents;
	if ( ! (IPC_GetEvents				= (IPC_GET_EVENTS)					GetProcAddress(IPC_g_hLib, "IPC_GetEvents")))				IPC_GetEvents				= IPC_StubGetEvents;
	if ( ! (IPC_ResetEvents				= (IPC_RESET_EVENTS)				GetProcAddress(IPC_g_hLib, "IPC_ResetEvents")))				IPC_ResetEvents				= IPC_StubResetEvents;

	if ( ! (IPC_GetConnectionLastErr	= (IPC_GET_CONNECTION_LAST_ERR)		GetProcAddress(IPC_g_hLib, "IPC_GetConnectionLastErr")))	IPC_GetConnectionLastErr	= IPC_StubGetConnectionLastErr;

	if ( ! (IPC_ServerDgStart			= (IPC_SERVER_DG_START)				GetProcAddress(IPC_g_hLib, "IPC_ServerDgStart")))			IPC_ServerDgStart			= IPC_StubServerDgStart;
	if ( ! (IPC_ServerDgStop			= (IPC_SERVER_DG_STOP)				GetProcAddress(IPC_g_hLib, "IPC_ServerDgStop")))			IPC_ServerDgStop			= IPC_StubServerDgStop;
	if ( ! (IPC_DgRecv					= (IPC_DG_RECV)						GetProcAddress(IPC_g_hLib, "IPC_DgRecv")))					IPC_DgRecv					= IPC_StubDgRecv;
	if ( ! (IPC_DgSend					= (IPC_DG_SEND)						GetProcAddress(IPC_g_hLib, "IPC_DgSend")))					IPC_DgSend					= IPC_StubDgSend;
	if ( ! (IPC_Init					= (IPC_INIT)						GetProcAddress(IPC_g_hLib, "IPC_Init")))					IPC_Init					= IPC_StubInit;
	if ( ! (IPC_Done					= (IPC_DONE)						GetProcAddress(IPC_g_hLib, "IPC_Done")))					IPC_Done					= IPC_StubDone;

	IPC_g_hLibLoadCount++;
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool IPC_FreeDLL()
{	if ( ! IPC_g_hLibLoadCount ) return false;
	if ( IPC_g_hLibLoadCount > 1 ) {
		IPC_g_hLibLoadCount--;
		return true;
	}

	if ( ! IPC_g_hLib || ! FreeLibrary( IPC_g_hLib ) ) return false;
	IPC_g_hLib = 0;

	IPC_GetVersion				= IPC_StubGetVersion;

    IPC_ServerStart				= IPC_StubServerStart;
	IPC_ServerStop				= IPC_StubServerStop;
	IPC_ServerWaitForConnection	= IPC_StubServerWaitForConnection;
	IPC_Connect					= IPC_StubConnect;
	IPC_CloseConnection			= IPC_StubCloseConnection;
	IPC_Recv					= IPC_StubRecv;
	IPC_Send					= IPC_StubSend;

	IPC_SetEvents				= IPC_StubSetEvents;
	IPC_GetEvents				= IPC_StubGetEvents;
	IPC_ResetEvents				= IPC_StubResetEvents;

	IPC_GetConnectionLastErr	= IPC_StubGetConnectionLastErr;

	IPC_ServerDgStart			= IPC_StubServerDgStart;
	IPC_ServerDgStop			= IPC_StubServerDgStop;
	IPC_DgRecv					= IPC_StubDgRecv;
	IPC_DgSend					= IPC_StubDgSend;
	IPC_Init					= IPC_StubInit;
	IPC_Done					= IPC_StubDone;

	IPC_g_hLibLoadCount--;
	return true;
}

// eof ///////////////////////////////////////////////////////////////////////