#include <windows.h>
#include <shlwapi.h>

#include "..\hook\idriver.h"
#include "..\r3hook\hookbase64.h"
#include "..\r3hook\sputnik.h"
#include "common.h"
#include "hooks.h"
#include "..\r3hook\debug.h"

#pragma warning(disable:4390 4996)

CLIENT_CONTEXT g_ClientContext;

BOOL RegisterClient(PCLIENT_CONTEXT pClientContext)
{
	CHAR				WhistleupName[MAX_PATH];
	CHAR				WFChangedName[MAX_PATH];

	ZeroMemory(pClientContext, sizeof(*pClientContext));
	pClientContext->m_AppReg.m_CurProcId = GetCurrentProcessId();
	pClientContext->m_AppReg.m_AppID = 0;
	pClientContext->m_AppReg.m_Priority = AVPG_STANDARTRIORITY;
	pClientContext->m_AppReg.m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG;
	pClientContext->m_AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

	wsprintfA(WhistleupName, "KLG2U%d%d%d", GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount());
	wsprintfA(WFChangedName, "KLG2C%d%d%d", GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount());

	OSVERSIONINFO OsInfo;
	OsInfo.dwOSVersionInfoSize = sizeof(OsInfo);
	GetVersionEx(&OsInfo);

	pClientContext->m_hDevice = RegisterApp(&pClientContext->m_AppReg, &OsInfo, &pClientContext->m_hWhistleup, &pClientContext->m_hWFChanged, WhistleupName, WFChangedName);
	return pClientContext->m_hDevice != INVALID_HANDLE_VALUE?TRUE:FALSE;
}

VOID UnRegisterClient(PCLIENT_CONTEXT pClientContext)
{
	IDriverUnregisterApp(pClientContext->m_hDevice, pClientContext->m_AppReg.m_AppID, FALSE);
	if (pClientContext->m_hWhistleup)
		CloseHandle(pClientContext->m_hWhistleup);
	if (pClientContext->m_hWFChanged)
		CloseHandle(pClientContext->m_hWFChanged);

	ZeroMemory(pClientContext, sizeof(*pClientContext));
	pClientContext->m_hDevice = INVALID_HANDLE_VALUE;
}

volatile DWORD g_SysEnters = 0;
volatile BOOL g_bHookBaseInited = FALSE;
HANDLE g_hInitThread = NULL;

// асинхронная стадия инициализации - чтобы избавится от лочек
DWORD WINAPI wrInitHooks(PVOID pParameter)
{
	if (!RegisterClient(&g_ClientContext))
	{
		HookBaseDone();
		InterlockedDecrement((PLONG)&g_SysEnters);

		return -1L;
	}

	g_bHookBaseInited = TRUE;

	InterlockedDecrement((PLONG)&g_SysEnters);

	return 0;
}


BOOL InitHooks()
{
	BOOL bResult = FALSE;
	DWORD ThreadID;

	if (!HookBaseInit())
		return FALSE;

	bResult = HookExports();

	InterlockedIncrement((PLONG)&g_SysEnters);
	g_hInitThread = CreateThread(NULL, 0, wrInitHooks, NULL, 0, &ThreadID);
	if (!g_hInitThread)
	{
		ODS(("miscr3::InitHooks - failed to CreateThread!\n"));
		InterlockedDecrement((PLONG)&g_SysEnters);
	}

	return bResult;
}

VOID UnInitHooks()
{
	DWORD SleepCount = 0;

	if (g_hInitThread)
	{
		DWORD Result;

		Result = WaitForSingleObject(g_hInitThread, 3000/* fix: bug 22944 */);
		if (Result != WAIT_OBJECT_0)
		{
			ODS(("Achtung! wait for init thread returnes failure.\n"));
			TerminateThread(g_hInitThread, 0);	// fix: bug 23576 - forcibly terminated thread? which caused deadlock
		}

		CloseHandle(g_hInitThread);
	}

	while (g_SysEnters && SleepCount < 200)
	{
		Sleep(15);
		SleepCount++;
	}

	if (g_bHookBaseInited)
	{
		HookBaseDone();
		UnRegisterClient(&g_ClientContext);
	}
}

HMODULE g_hInstance = NULL;

BOOL CheckInstaller()
{
	WCHAR PathBuffer[512];
	DWORD Result;

	Result = GetModuleFileNameW(NULL, PathBuffer, sizeof(PathBuffer));
	if (Result)
	{
		PathStripPathW(PathBuffer);
		_wcslwr(PathBuffer);

		if ((wcscmp(L"msiexec.exe", PathBuffer) == 0) ||
			(wcscmp(L"regsvr32.exe", PathBuffer) == 0))
			return TRUE;
	}

	return FALSE;
}

BOOL WINAPI DllMain(HMODULE hModule, 
					DWORD  ul_reason_for_call, 
					LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);

		g_hInstance = hModule;

		if (CheckInstaller())
		{
			ODS(("r3hook - installer detected, bypassing patching.."));
			return TRUE;
		}

		return InitHooks();
	case DLL_PROCESS_DETACH:
		UnInitHooks();

		return TRUE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	RegisterSputnikDllByHandle("dnsapi.dll", g_hInstance);
	RegisterSputnikDllByHandle("dnsapi", g_hInstance);

    return S_OK;//_Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	UnregisterSputnikDllByHandle("dnsapi.dll", g_hInstance);
	UnregisterSputnikDllByHandle("dnsapi", g_hInstance);

    return S_OK;//_Module.UnregisterServer(TRUE);
}
