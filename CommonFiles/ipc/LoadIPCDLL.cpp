//////////////////////////////////////////////////////////////////////////
//
// CCClient DLL loader
//
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "LoadIPCDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REG_AVPSHARED	_T("Software\\KasperskyLab\\SharedFiles")	// registry entry

static HINSTANCE hLib = NULL;

//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// DLL Exported general functions (Initialization / Deinitialization)
/////////////////////////////////////////////////////////////////////////////

IPC_GETVERSION IPC_GetVersion = NULL;

IPC_INIT IPC_Init = NULL;
IPC_DONE IPC_Done = NULL;

IPC_SERVERSTART IPC_ServerStart = NULL;
IPC_SERVERSTOP IPC_ServerStop = NULL;

IPC_SERVERWAITFORCONNECTION IPC_ServerWaitForConnection = NULL;
IPC_CONNECT IPC_Connect = NULL;
IPC_CLOSECONNECTION IPC_CloseConnection = NULL;

IPC_RECV IPC_Recv = NULL;
IPC_SEND IPC_Send = NULL;

IPC_SERVERDGSTART IPC_ServerDgStart = NULL;
IPC_SERVERDGSTOP IPC_ServerDgStop = NULL;

IPC_DGRECV IPC_DgRecv = NULL;
IPC_DGSEND IPC_DgSend = NULL;

IPC_SETEVENTS IPC_SetEvents = NULL;
IPC_GETEVENTS IPC_GetEvents = NULL;
IPC_RESETEVENTS IPC_ResetEvents = NULL;

IPC_GET_CONNECTION_LAST_ERR IPC_GetConnectionLastErr = NULL;
//___________________________________________________________________________


/////////////////////////////////////////////////////////////////////////////
// IPCLoadDLL - Function loads DLL from specified folder or (if there is not DLL)
// from default folders and imports all functions
// --------------------------------------------------------------------------
// LPCTSTR pszDllPath		The DLL Folder. If pszDllPath == NULL or pszDllPath == ""
//							function searches DLL in folder stored in AVP Shared Files Registry Entry
// return:		TRUE - if successfull, FALSE - otherwise.
BOOL IPCLoadDLL(LPCTSTR pszDllPath)
{
	try
	{
		if (!hLib)
		{
			TCHAR szFullDLLPath[MAX_PATH];

			// Try to load DLL from specified folder

			if (!pszDllPath || *pszDllPath == 0)
			{
				// Load CCClient DLL
				TCHAR szSharedFolder[MAX_PATH];
				DWORD n = MAX_PATH;

				szSharedFolder[0] = 0;
			
				HKEY hKey;

				if (RegOpenKey(HKEY_LOCAL_MACHINE, REG_AVPSHARED, &hKey) == ERROR_SUCCESS)
				{
					RegQueryValueEx(hKey, _T("Folder"), NULL, NULL, (LPBYTE)szSharedFolder, &n);
					RegCloseKey(hKey);
				}

				_tcscpy(szFullDLLPath, szSharedFolder);
			}
			else
				_tcscpy(szFullDLLPath, pszDllPath);

			// Close path
			if (szFullDLLPath[_tcslen(szFullDLLPath) - 1] != L'\\') _tcscat(szFullDLLPath, _T("\\"));
			_tcscat(szFullDLLPath, _T("KLIPC.dll"));

			hLib = LoadLibrary(szFullDLLPath);
			if (!hLib)
			{
				// If error - Try to load DLL from default folders
				hLib = LoadLibrary(_T("KLIPC.dll"));
			}

			if (hLib)
			{
				IPC_GetConnectionLastErr = (IPC_GET_CONNECTION_LAST_ERR)::GetProcAddress(hLib, "IPC_GetConnectionLastErr");

				if ((IPC_GetVersion = (IPC_GETVERSION)::GetProcAddress(hLib, "IPC_GetVersion")) &&

					(IPC_Init = (IPC_INIT)::GetProcAddress(hLib, "IPC_Init")) &&
					(IPC_Done = (IPC_DONE)::GetProcAddress(hLib, "IPC_Done")) &&

					(IPC_ServerStart = (IPC_SERVERSTART)::GetProcAddress(hLib, "IPC_ServerStart")) &&
					(IPC_ServerStop = (IPC_SERVERSTOP)::GetProcAddress(hLib, "IPC_ServerStop")) &&

					(IPC_ServerWaitForConnection = (IPC_SERVERWAITFORCONNECTION)::GetProcAddress(hLib, "IPC_ServerWaitForConnection")) &&
					(IPC_Connect = (IPC_CONNECT)::GetProcAddress(hLib, "IPC_Connect")) &&
					(IPC_CloseConnection = (IPC_CLOSECONNECTION)::GetProcAddress(hLib, "IPC_CloseConnection")) &&

					(IPC_Recv = (IPC_RECV)::GetProcAddress(hLib, "IPC_Recv")) &&
					(IPC_Send = (IPC_SEND)::GetProcAddress(hLib, "IPC_Send")) &&

					(IPC_ServerDgStart = (IPC_SERVERDGSTART)::GetProcAddress(hLib, "IPC_ServerDgStart")) &&
					(IPC_ServerDgStop = (IPC_SERVERDGSTOP)::GetProcAddress(hLib, "IPC_ServerDgStop")) &&

					(IPC_DgRecv = (IPC_DGRECV)::GetProcAddress(hLib, "IPC_DgRecv")) &&
					(IPC_DgSend = (IPC_DGSEND)::GetProcAddress(hLib, "IPC_DgSend"))	&&

					(IPC_SetEvents = (IPC_SETEVENTS )::GetProcAddress(hLib, "IPC_SetEvents")) &&
					(IPC_GetEvents = (IPC_GETEVENTS )::GetProcAddress(hLib, "IPC_GetEvents")) &&
					(IPC_ResetEvents = (IPC_RESETEVENTS )::GetProcAddress(hLib, "IPC_ResetEvents")) 
					) return TRUE;

			}
		}
		else
			return TRUE;
	}
	catch(...)
	{
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IPCFreeDLL - Function unloads DLL from memory
void IPCFreeDLL()
{
	try
	{
		if (hLib) 
		{
			IPC_GetVersion = NULL;
			IPC_Init = NULL;
			IPC_Done = NULL;
			IPC_ServerStart = NULL;
			IPC_ServerStop = NULL;
			IPC_ServerWaitForConnection = NULL;
			IPC_Connect = NULL;
			IPC_CloseConnection = NULL;
			IPC_Recv = NULL;
			IPC_Send = NULL;
			IPC_ServerDgStart = NULL;
			IPC_ServerDgStop = NULL;
			IPC_DgRecv = NULL;
			IPC_DgSend = NULL;
			IPC_SetEvents = NULL;
			IPC_GetEvents = NULL;
			IPC_ResetEvents = NULL;
			IPC_GetConnectionLastErr = NULL;
			
			FreeLibrary(hLib);
			hLib = NULL;
		}
	}
	catch(...)
	{
	}
}

