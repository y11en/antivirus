// clldr.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "hooks.h"
#include "debug.h"
volatile DWORD g_SysEnters = 0;

BOOL InitHooks()
{
	BOOL bResult = FALSE;

	bResult = HookBaseInit();
	if ( !bResult )
		return FALSE;

	bResult = SetDispatchHooks();
	if ( !bResult )
	{
		HookBaseDone();
		return FALSE;
	}
	else
		return TRUE;

	return FALSE;
}

VOID UnInitHooks()
{
	DWORD SleepCount = 0;
	while (g_SysEnters && SleepCount < 200)
	{
		Sleep(15);
		SleepCount++;
	}

	RemoveDispatchHooks();
	HookBaseDone();
}

VOID CheckDedicatedProcess()
{
	TCHAR tcProcessBaseName[0x200];

	DWORD dwResult = GetModuleBaseName(
		GetCurrentProcess(),
		NULL,
		tcProcessBaseName,
		_countof(tcProcessBaseName)
		);
	if ( dwResult )
	{
		if ( 0 == _tcsicmp( _T("taskmgr.exe"), tcProcessBaseName)
//			|| 0 == _tcsicmp( _T("avp.exe"), tcProcessBaseName) // we call SetClientVerdict from avp.exe by ourselves
			)
		{
			SetClientVerdict(Verdict_Discard);		// protect Task manager
		}
	}
}

// hook proc 
extern "C" __declspec(dllexport) LRESULT CALLBACK hc(int code, WPARAM wParam, LPARAM lParam)
{
	if ( code < 0 )
		return CallNextHookEx(NULL, code, wParam, lParam);

	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, 
					DWORD  ul_reason_for_call, 
					LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);

			BOOL bResult = InitHooks();
			if ( bResult )
				CheckDedicatedProcess();

			return bResult;
		}
	case DLL_PROCESS_DETACH:
		UnInitHooks();
		return TRUE;
	}

	return TRUE;
}
