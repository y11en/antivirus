// CKAHComm.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "Stuff/RegStorage.h"

CProgramExecutionLog log;

HANDLE g_hStopEvent;

bool g_bIsNT = true;

void GetWindowsVersion ()
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (!GetVersionEx (&osvi))
		return;

	g_bIsNT = (osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetWindowsVersion ();
		DisableThreadLibraryCalls ((HMODULE)hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

    return TRUE;
}

CKAHCOMMEXPORT void CkahComm_SetExternalLogger (CProgramExecutionLog::EXTERNALLOGGERPROC logger)
{
	log.Initialize (logger);
}
