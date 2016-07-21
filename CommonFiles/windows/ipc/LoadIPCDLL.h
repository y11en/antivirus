/////////////////////////////////////////////////////////////////////////////////////
//
// Load IPC DLL
//
/////////////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_IPC_H
#define LOAD_IPC_H

////////////////////////////////////////////////////////////////////////////////

#define		IPC_RC_ERROR			-1
#define		IPC_RC_TIMEOUT			-2

////////////////////////////////////////////////////////////////////////////////

typedef	void *	HIPCSERVER;
typedef	void *	HIPCCONNECTION;

////////////////////////////////////////////////////////////////////////////////

//#define	IPC_CALL_TYPE 
#define	IPC_CALL_TYPE __stdcall


//#define IPC_API 
#define IPC_API __declspec(dllimport)

////////////////////////////////////////////////////////////////////////////////

typedef IPC_API DWORD (IPC_CALL_TYPE *IPC_GETVERSION)();

typedef IPC_API void (IPC_CALL_TYPE *IPC_INIT)();
typedef IPC_API void (IPC_CALL_TYPE *IPC_DONE)();

typedef IPC_API HIPCSERVER (IPC_CALL_TYPE *IPC_SERVERSTART)(LPCTSTR pszAddress);
typedef IPC_API BOOL (IPC_CALL_TYPE *IPC_SERVERSTOP)(HIPCSERVER hServer);

typedef IPC_API HIPCCONNECTION (IPC_CALL_TYPE *IPC_SERVERWAITFORCONNECTION)(HIPCSERVER hServer, DWORD dwTimeout, HANDLE hBreakEvent = NULL);
typedef IPC_API HIPCCONNECTION (IPC_CALL_TYPE *IPC_CONNECT)(LPCTSTR pszAddress, DWORD dwTimeout);
typedef IPC_API BOOL (IPC_CALL_TYPE *IPC_CLOSECONNECTION)(HIPCCONNECTION hConnection);

typedef IPC_API DWORD (IPC_CALL_TYPE *IPC_RECV)(HIPCCONNECTION hConnection, void * pvBuf, DWORD dwBufSize, DWORD dwTimeout);
typedef IPC_API DWORD (IPC_CALL_TYPE *IPC_SEND)(HIPCCONNECTION hConnection, void * pvBuf, DWORD dwBufSize, DWORD dwTimeout);

typedef IPC_API HIPCSERVER (IPC_CALL_TYPE *IPC_SERVERDGSTART)(LPCTSTR pszAddress);
typedef IPC_API BOOL (IPC_CALL_TYPE *IPC_SERVERDGSTOP)(HIPCSERVER hServer);

typedef IPC_API DWORD (IPC_CALL_TYPE *IPC_DGRECV)(LPCTSTR pszAddress, void * pvBuf, DWORD dwBufSize, DWORD dwTimeout);
typedef IPC_API DWORD (IPC_CALL_TYPE *IPC_DGSEND)(LPCTSTR pszAddress, void * pvBuf, DWORD dwBufSize, DWORD dwTimeout);

typedef IPC_API BOOL (IPC_CALL_TYPE *IPC_SETEVENTS)( HIPCCONNECTION	hConnection, HANDLE *	pUserEvents, DWORD	dwUserEventsCount, HANDLE *	pIPCEvents );
typedef IPC_API BOOL (IPC_CALL_TYPE *IPC_GETEVENTS)( HIPCCONNECTION	hConnection, DWORD *	pdwUserEvents, DWORD *pdwIPCEvents );
typedef IPC_API BOOL (IPC_CALL_TYPE *IPC_RESETEVENTS)( HIPCCONNECTION	hConnection );
typedef IPC_API	DWORD (IPC_CALL_TYPE *IPC_GET_CONNECTION_LAST_ERR)(HIPCCONNECTION hConnection);

extern IPC_GETVERSION IPC_GetVersion;

extern IPC_INIT IPC_Init;
extern IPC_DONE IPC_Done;

extern IPC_SERVERSTART IPC_ServerStart;
extern IPC_SERVERSTOP IPC_ServerStop;

extern IPC_SERVERWAITFORCONNECTION IPC_ServerWaitForConnection;
extern IPC_CONNECT IPC_Connect;
extern IPC_CLOSECONNECTION IPC_CloseConnection;

extern IPC_RECV IPC_Recv;
extern IPC_SEND IPC_Send;

extern IPC_SERVERDGSTART IPC_ServerDgStart;
extern IPC_SERVERDGSTOP IPC_ServerDgStop;

extern IPC_DGRECV IPC_DgRecv;
extern IPC_DGSEND IPC_DgSend;

extern IPC_SETEVENTS IPC_SetEvents;
extern IPC_GETEVENTS IPC_GetEvents;
extern IPC_RESETEVENTS IPC_ResetEvents;

extern IPC_GET_CONNECTION_LAST_ERR IPC_GetConnectionLastErr;

//___________________________________________________________________________

/////////////////////////////////////////////////////////////////////////////
// IPCLoadDLL - Function loads DLL from specified folder or (if there is not DLL)
// from default folders and imports all functions
// --------------------------------------------------------------------------
// LPCTSTR pszDllPath		The DLL Folder. If pszDllPath == NULL or pszDllPath == ""
//							function searches DLL in folder stored in AVP Shared Files Registry Entry
// return:		TRUE - if successfull, FALSE - otherwise.
BOOL IPCLoadDLL(LPCTSTR pszDllPath);

/////////////////////////////////////////////////////////////////////////////
// IPCFreeDLL - Function unloads DLL from memory
void IPCFreeDLL();

__inline BOOL CHECK_IPC_HCONNECTION(PVOID hConnection)
{
	return (hConnection && ((int)hConnection != -2));
}

__inline BOOL CHECK_IPC_RESULT(int rc)
{
	return ((rc != 0) && (rc != -1) && (rc != -2));
}

#endif