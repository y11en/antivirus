// kd1dll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
extern "C"
{
	#include "..\kdprint\user\server\kdprint.h"
}

HHOOK hHook = NULL;

LRESULT CALLBACK HookProc(int nCode,  WPARAM wParam, LPARAM lParam)
{
	char Buffer[32];
	if (GetKeyNameText(lParam, Buffer, 32))
		KDPrint("KD1: %s %s\n", Buffer, lParam & 0x80000000? "released": "pressed");

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------------------
extern "C"
{

__declspec(dllexport) BOOL __cdecl SetGlobalHook(HMODULE hModule)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD, HookProc, (HMODULE)hModule, 0);
	return !!hHook;
}

__declspec(dllexport) void __cdecl UnsetGlobalHook()
{
	if (hHook)
		UnhookWindowsHookEx(hHook);
}

}
//-------------------------------------------------------------------------------------------------------------

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:		
		if (!KDPrintInit())
			return FALSE;

		KDPrint("kd1dll.dll attached to PID:%d\n", GetCurrentProcessId());		
		break;
	case DLL_PROCESS_DETACH:
		KDPrintUnInit();

		KDPrint("kd1dll.dll detached from PID:%d\n", GetCurrentProcessId());		
	}

    return TRUE;
}

