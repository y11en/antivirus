#include <windows.h>
#include <shlwapi.h>
#include <crtdbg.h>

#include "..\hook\fssync.h"
#include "..\r3hook\hookbase64.h"
#include "..\r3hook\sputnik.h"
#include "common.h"
#include "hooks.h"
#include "debug.h"

#pragma warning(disable:4390 4996)

void* __cdecl DRV_MemAlloc( PVOID pOpaquePtr, size_t Size, ULONG Tag)
{
	return HeapAlloc(
		GetProcessHeap(),
		0,
		Size
		);
}

void __cdecl DRV_MemFree( PVOID pOpaquePtr, void** p_pMem )
{
	_ASSERT( p_pMem );
	_ASSERT( *p_pMem );

	HeapFree(
		GetProcessHeap(),
		0,
		*p_pMem
		);
	*p_pMem = NULL;
}

HRESULT GetFSSyncContext( HMODULE hProductModule, PFSSYNC_CONTEXT *p_pContext )
{
	DWORD dwResult;
	HMODULE hFSSync = NULL;
	WCHAR ModulePathName[MAX_PATH*2];
	PFSSYNC_CONTEXT pContext = NULL;
	HRESULT hResult;
	PVOID pClientContext = NULL;

	_ASSERT(p_pContext);

	dwResult = GetModuleFileNameW(
		hProductModule,
		ModulePathName,
		_countof( ModulePathName )
		);
	if ( !dwResult )
		return E_INVALIDARG;

	if ( !PathRemoveFileSpecW( ModulePathName ) )
		return E_INVALIDARG;

	pContext = (PFSSYNC_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*pContext));
	if (!pContext)
		return E_OUTOFMEMORY;

	wcscat(ModulePathName, L"\\fssync.dll");
	hFSSync = LoadLibraryExW(ModulePathName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hFSSync)
	{
		HeapFree(GetProcessHeap(), 0, pContext);
		return E_FAIL;
	}

	pContext->m_DRV_Register = (t_fDRV_Register) GetProcAddress( hFSSync, "FSSync_DR" );
	pContext->m_DRV_UnRegister = (t_fDRV_UnRegister) GetProcAddress( hFSSync, "FSSync_DUR" );
	pContext->m_DRV_ChangeActiveStatus = (t_fDRV_ChangeActiveStatus) GetProcAddress( hFSSync, "FSSync_DCS" );
	pContext->m_DRV_FilterEvent = (t_fDRV_FilterEvent) GetProcAddress( hFSSync, "FSSYNC_FE" );
	pContext->m_DRV_VerdictFlags2Verdict = (t_fDRV_VerdictFlags2Verdict) GetProcAddress( hFSSync, "FSSYNC_EF2V" );

	if ( !pContext->m_DRV_Register ||
		!pContext->m_DRV_UnRegister ||
		!pContext->m_DRV_ChangeActiveStatus ||
		!pContext->m_DRV_FilterEvent ||
		!pContext->m_DRV_VerdictFlags2Verdict
		)
	{
		FreeLibrary(hFSSync);
		HeapFree(GetProcessHeap(), 0, pContext);

		return E_FAIL;
	}

	hResult = pContext->m_DRV_Register(
		&pClientContext,
		0,
		AVPG_STANDARTRIORITY,
		_CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		DRV_MemAlloc,
		DRV_MemFree,
		NULL
		);
	if ( IS_ERROR( hResult ) )
	{
		FreeLibrary( hFSSync );
		HeapFree(GetProcessHeap(), 0, pContext);

		return E_FAIL;
	}

	pContext->m_pClientContext = pClientContext;
	pContext->m_hFSSync = hFSSync;
	*p_pContext = pContext;

	return S_OK;
}

VOID ReleaseFSSyncContext(PFSSYNC_CONTEXT *p_pContext)
{
	_ASSERT(p_pContext);
	_ASSERT(*p_pContext);

	(*p_pContext)->m_DRV_UnRegister(
		&(*p_pContext)->m_pClientContext
		);
	FreeLibrary((*p_pContext)->m_hFSSync);
	HeapFree(GetProcessHeap(), 0, *p_pContext);
	*p_pContext = NULL;
}


HMODULE g_hInstance = NULL;
PFSSYNC_CONTEXT g_pFSSyncContext = NULL;
volatile DWORD g_SysEnters = 0;
volatile BOOL g_bHookBaseInited = FALSE;
volatile BOOL g_bHookBaseInitFailed = FALSE;
HANDLE g_hInitThread = NULL;

// асинхронная стадия инициализации - чтобы избавится от лочек
DWORD WINAPI wrInitHooks(PVOID pParameter)
{
	HRESULT hResult;

	hResult = GetFSSyncContext( g_hInstance, &g_pFSSyncContext );
	if ( IS_ERROR( hResult ) )
	{
		g_bHookBaseInitFailed = TRUE;

		HookBaseDone();
		InterlockedDecrement((PLONG)&g_SysEnters);

		return -1L;
	}

	g_bHookBaseInited = TRUE;

	InterlockedDecrement((PLONG)&g_SysEnters);

	return 0;
}

extern "C" VOID WINAPI hme()
{
	if ( !g_bHookBaseInited )
		return;

	UnHookAll();

	SetDllGetClassObjectHook("shell32.dll");
	SetDllGetClassObjectHook("qmgrprxy.dll");
	HookExports();
}

VOID InitHooks()
{
	DWORD ThreadID;

	if (!HookBaseInit())
		return;

	SetDllGetClassObjectHook("shell32.dll");
	SetDllGetClassObjectHook("qmgrprxy.dll");
	HookExports();

	InterlockedIncrement((PLONG)&g_SysEnters);
	g_hInitThread = CreateThread(NULL, 0, wrInitHooks, NULL, 0, &ThreadID);
	if (!g_hInitThread)
	{
		ODS(("miscr3::InitHooks - failed to CreateThread!\n"));
		InterlockedDecrement((PLONG)&g_SysEnters);
	}
}

VOID UnInitHooks()
{
	DWORD SleepCount = 0;

	if (g_hInitThread)
	{
		DWORD Result;

		Result = WaitForSingleObject(g_hInitThread, 3000/* possible deadlock: bug 22944 */);
		if (Result != WAIT_OBJECT_0)
			ODS(("Achtung! wait for init thread returnes failure.\n"));

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
		ReleaseFSSyncContext( &g_pFSSyncContext );
	}
}

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

		InitHooks();

		break;
	case DLL_PROCESS_DETACH:
		UnInitHooks();

		break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	RegisterSputnikDllByHandle("shell32.dll", g_hInstance);
	RegisterSputnikDllByHandle("qmgrprxy.dll", g_hInstance);
	RegisterSputnikDllByHandle("mshtml.dll", g_hInstance);
	RegisterSputnikDllByHandle("user32.dll", g_hInstance);

    return S_OK;//_Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	UnregisterSputnikDllByHandle("shell32.dll", g_hInstance);
	UnregisterSputnikDllByHandle("qmgrprxy.dll", g_hInstance);
	UnregisterSputnikDllByHandle("mshtml.dll", g_hInstance);
	UnregisterSputnikDllByHandle("user32.dll", g_hInstance);

    return S_OK;//_Module.UnregisterServer(TRUE);
}
