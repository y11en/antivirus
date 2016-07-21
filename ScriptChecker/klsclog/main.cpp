#include <windows.h>
#include <activscp.h>
#include "debug.h"

HINSTANCE g_hInstance = NULL;
HANDLE g_hMutex_KLSCLOG = NULL;

#include "..\sc_interceptor2\scplugin.h"
#define PLUGIN_NAME     "SC Logging Plugin"
#define PLUGIN_PRIORITY 1
#define PLUGIN_LOGFILE  "c:\\scripts.log"
#define	PLUGIN_REG_KEY  SC_REG_PLUGINS "\\" PLUGIN_NAME

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	static HANDLE g_hMutex_KavInProcessDllLoaded = NULL;
	BOOL bRes = TRUE;
	
	g_hInstance = hInstance;
    
	if (dwReason == DLL_PROCESS_ATTACH)
    {
		ODS(("DLL_PROCESS_ATTACH"));

#define KAV_INPROCESS_DLL_LOADED "KAV_INPROCESS_DLL_LOADED"
		g_hMutex_KavInProcessDllLoaded = CreateMutex(NULL, FALSE, KAV_INPROCESS_DLL_LOADED);

#define KLSCLOG_MUTEX "KLSCLOG_MUTEX"
		g_hMutex_KLSCLOG = CreateMutex(NULL, FALSE, KLSCLOG_MUTEX);
		if (g_hMutex_KLSCLOG == NULL)
			return FALSE;

		DisableThreadLibraryCalls(hInstance);
		ODS(("DLL_PROCESS_ATTACH finished"));
    }

    if (dwReason == DLL_PROCESS_DETACH)
	{
		ODS(("DLL_PROCESS_DETACH"));

		if (g_hMutex_KavInProcessDllLoaded)
			CloseHandle(g_hMutex_KavInProcessDllLoaded);

		if (g_hMutex_KLSCLOG)
			CloseHandle(g_hMutex_KLSCLOG);

		ODS(("DLL_PROCESS_DETACH finished"));
	}

    return bRes;    // ok
}

BOOL __stdcall IsEnabled()
{
	return TRUE;
}

BOOL __stdcall ProcessScript(WCHAR* pwcsScript, WCHAR* pwcsLanguage, WCHAR* pwcsURL, IActiveScript* pActiveScript, DWORD* pdwFlags)
{
	BOOL bResult = TRUE;
	SYSTEMTIME systime;
	TCHAR ModuleName[MAX_PATH] = "";
	DWORD dwStrLen;
	HANDLE hLogFile = INVALID_HANDLE_VALUE;
	CHAR Caption[0x1000];
	char* pScriptText;
	char  szLogName[MAX_PATH];

	DWORD dwAllocSize = WideCharToMultiByte(CP_ACP, 0, pwcsScript, -1, NULL, 0, NULL, NULL);
	if (dwAllocSize == 0)
		return FALSE;

	pScriptText = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAllocSize);
	if (pScriptText == NULL)
	{
		ODS(("HeapAlloc failed\n"));
		return FALSE;
	}

	DWORD dwCurrentScriptLen = WideCharToMultiByte(CP_ACP, 0, pwcsScript, -1, pScriptText, dwAllocSize, NULL, NULL);
	if (dwCurrentScriptLen == 0)
	{
		if (pScriptText != NULL)
			HeapFree(GetProcessHeap(), 0, pScriptText);
		return FALSE;
	}

	GetLocalTime(&systime);

	GetModuleFileName(NULL, ModuleName, sizeof(ModuleName));
	if (lstrlen(ModuleName) == 0)
		lstrcpy(ModuleName, "Unknown");
	wsprintf(Caption, "\r\n---------------------\r\n%02d.%02d.%4d %02d:%02d:%02d\r\nApplication: '%s'\r\nLanguage: '%S'\r\nURL: %S\r\n\r\n", 
		systime.wDay, systime.wMonth, systime.wYear, 
		systime.wHour, systime.wMinute, systime.wSecond,
		ModuleName, pwcsLanguage, pwcsURL);

	if (WAIT_OBJECT_0 == WaitForSingleObject(g_hMutex_KLSCLOG, INFINITE))
	{
		DWORD dwSize = sizeof(szLogName);
		HKEY hPluginKey;
		if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\KasperskyLab\\Components\\SC\\Plugins\\" PLUGIN_NAME, &hPluginKey))
		{
			DWORD dwType = REG_SZ;
			if (ERROR_SUCCESS == RegQueryValueEx(hPluginKey, "LogFile", NULL, &dwType, (LPBYTE)&szLogName, &dwSize))
			{
				hLogFile = CreateFile(szLogName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hLogFile != INVALID_HANDLE_VALUE)
				{
					SetFilePointer(hLogFile, 0, NULL, FILE_END);
					WriteFile(hLogFile, Caption, lstrlen(Caption), &dwStrLen, NULL);
					OutputDebugString(Caption);
					OutputDebugString("\n-- Script ---------------------\n");
					WriteFile(hLogFile, pScriptText, dwCurrentScriptLen-1, &dwStrLen, NULL);
					OutputDebugString(pScriptText);
					OutputDebugString("\n-- End ---------------------\n");
					CloseHandle(hLogFile);
				}
				ReleaseMutex(g_hMutex_KLSCLOG);
			}
			RegCloseKey(hPluginKey);
		}
	}

	if (pScriptText != NULL)
		HeapFree(GetProcessHeap(), 0, pScriptText);


	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	if (ERROR_SUCCESS == RegDeleteKey(HKEY_LOCAL_MACHINE, PLUGIN_REG_KEY))
		return S_OK;
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	LONG nResult; 
	HKEY hPluginKey;

	DllUnregisterServer();

	nResult = RegCreateKey(HKEY_LOCAL_MACHINE, PLUGIN_REG_KEY, &hPluginKey);
	if (ERROR_SUCCESS == nResult)
	{
		CHAR szModule[_MAX_PATH];
		LPSTR pszModule;
		DWORD dwDword;

		GetModuleFileName(g_hInstance, szModule, _MAX_PATH);

		if ((g_hInstance == NULL) || (g_hInstance == GetModuleHandle(NULL))) // register as EXE
		{
			// Convert to short path to work around bug in NT4's CreateProcess
			CHAR szModuleShort[_MAX_PATH];
			int cbShortName = GetShortPathName(szModule, szModuleShort, _MAX_PATH);

			if (cbShortName == _MAX_PATH)
				return E_OUTOFMEMORY;

			pszModule = (cbShortName == 0 || cbShortName == ERROR_INVALID_PARAMETER) ? szModule : szModuleShort;
		}
		else
			pszModule = szModule;

		nResult = RegSetValue(hPluginKey, NULL, REG_SZ, (LPCSTR)pszModule, strlen(pszModule));
		if (ERROR_SUCCESS == nResult)
			nResult = RegSetValueEx(hPluginKey, "LogFile", NULL, REG_SZ, (LPBYTE)PLUGIN_LOGFILE, strlen(PLUGIN_LOGFILE));
		if (ERROR_SUCCESS == nResult)
		{
			dwDword = PLUGIN_PRIORITY;
			nResult = RegSetValueEx(hPluginKey, "Priority", NULL, REG_DWORD, (LPBYTE)&dwDword, sizeof(dwDword));
		}
		if (ERROR_SUCCESS == nResult)
		{
			dwDword = 1;
			nResult = RegSetValueEx(hPluginKey, "Enabled", NULL, REG_DWORD, (LPBYTE)&dwDword, sizeof(dwDword));
		}

		RegCloseKey(hPluginKey);
	}

	if (nResult != ERROR_SUCCESS)
		return E_FAIL;
	return S_OK;

}
