// test_dll.cpp : Defines the entry point for the DLL application.
//
//#pragma comment(linker, "/export:_SetHook")

#include "stdafx.h"
#include "test_export.h"
HHOOK g_hHook = NULL;
HHOOK g_hPersonalHook = NULL;
HINSTANCE g_hModule = NULL;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK KeyboardProcOld(int nCode, WPARAM wParam, LPARAM lParam) ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	//MessageBoxA(0, "main", "DllMain", MB_OK);
	OutputDebugStringA("DllMain::start");
	printf("DllMain:: ul_reason_for_call=%d\n", ul_reason_for_call);
	//if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	if( DLL_PROCESS_ATTACH == ul_reason_for_call ) {
		printf("DllMain:: do somethig\n");
        g_hModule = hModule;
	}
    return TRUE;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
DllExport BOOL SetHook()
{
	//MessageBoxA(0, "before SetWindowsHookEx", "SetHook", MB_OK);
    g_hHook = SetWindowsHookExA(WH_KEYBOARD, KeyboardProc, g_hModule, NULL);
	//MessageBoxA(0, "after SetWindowsHookEx", "SetHook", MB_OK);
    return (BOOL)g_hHook;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
DllExport BOOL DelHook()
{
	if (g_hHook)
		UnhookWindowsHookEx(g_hHook);
    return TRUE;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
DllExport BOOL SetOldHook()
{
    g_hHook = SetWindowsHook(WH_KEYBOARD_LL, KeyboardProcOld);
    return (BOOL)g_hHook;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
DllExport BOOL DelOldHook()
{
	if (g_hHook)
		UnhookWindowsHook(WH_KEYBOARD_LL, KeyboardProcOld);
    return TRUE;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
DllExport BOOL SetHookByThreadID(DWORD ThreadID)
{
	if (ThreadID == 0)
		return FALSE;
	g_hPersonalHook = SetWindowsHookExA(WH_KEYBOARD, KeyboardProc, g_hModule, ThreadID);
	return (BOOL)g_hPersonalHook;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
DllExport BOOL DelHookByThreadID()
{
	if (g_hPersonalHook) {
		UnhookWindowsHookEx(g_hPersonalHook);
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{ 
    CHAR szBuf[128]; 
    HDC hdc; 
    static int c = 0; 
    size_t cch; 
	size_t * pcch;
	HRESULT hResult;
 
    if (nCode < 0)  // do not process message 
        return CallNextHookEx(g_hHook, nCode, 
            wParam, lParam); 
 
	if (!(lParam>>31)) {
		char Temp[100];
		char tempCh[2];
		tempCh[0] = (char)wParam;
		tempCh[1] = 0;
		sprintf(Temp, "KeyEvent: %s\n", tempCh);
		OutputDebugStringA(Temp);
	}
    return CallNextHookEx(g_hHook, nCode, wParam, lParam); 
} 
///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
LRESULT CALLBACK KeyboardProcOld(int nCode, WPARAM wParam, LPARAM lParam) 
{ 
    CHAR szBuf[128]; 
    HDC hdc; 
    static int c = 0; 
    size_t cch; 
	size_t * pcch;
	HRESULT hResult;
 
    if (nCode < 0)  // do not process message 
        return DefHookProc(nCode, wParam, lParam, &g_hHook); 
 
	if (!(lParam>>31)) {
		char Temp[100];
		char tempCh[2];
		tempCh[0] = (char)wParam;
		tempCh[1] = 0;
		sprintf(Temp, "KeyEventOld: %s\n", tempCh);
		OutputDebugStringA(Temp);
	}
    return DefHookProc(nCode, wParam, lParam, &g_hHook); 
} 
