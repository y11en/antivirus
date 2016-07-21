// OETestHelper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "OETestHelper.h"

namespace
{
	const UINT ID_CONFIG = WM_APP + 4;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// This is an example of an exported function.
extern "C" OETESTHELPER_API void OpenConfigureDialog(HWND hMainOEWindow)
{
	::PostMessage(hMainOEWindow, WM_COMMAND, ID_CONFIG, 0);
}

