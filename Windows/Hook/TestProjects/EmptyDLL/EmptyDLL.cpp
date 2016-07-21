// EmptyDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// hook proc 
extern "C" __declspec(dllexport) LRESULT CALLBACK hc(int code, WPARAM wParam, LPARAM lParam)
{
	if ( code < 0 )
		return CallNextHookEx(NULL, code, wParam, lParam);

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		OutputDebugString(L"loaded empty dll!\n");

	if (ul_reason_for_call == DLL_PROCESS_DETACH)
		OutputDebugString(L"unloaded empty dll!\n");

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

