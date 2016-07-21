#include "commhdr.h"
#pragma hdrstop

// KAVLicensing.cpp : Defines the entry point for the DLL application.
//


extern HINSTANCE g_hinst=NULL;


BOOL APIENTRY DllMain( HINSTANCE hinst, 
                       DWORD dwReason, 
                       LPVOID
                     )
{
    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
            g_hinst=hinst;
            DisableThreadLibraryCalls(hinst);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#define LICENSING_CHECK			Check
#define LICENSING_ADD_KEY		AddLicenseKey

// return values of function LPFNCHECK
#define CHECK_ERROR				0
#define CHECK_FULL_FEATURES		0x2
#define CHECK_NO_UPDATES		0x4
#define CHECK_NO_FEATURES		0x8 // updater is updates only key black list


#include "lic.h"

extern "C" __declspec(dllexport) 
LONG APIENTRY MyFunction (HWND hwnd, LPLONG lpIValue, LPSTR lpszValue)
{
    return MessageBox(hwnd,lpszValue,"DEBUG", MB_OK);
}


extern "C" __declspec(dllexport) DWORD /*__stdcall*/ fn03 /*LICENSING_CHECK*/(LPSTR szKey, LPSTR, LPSTR, LPVOID, DWORD* pdwWarning)
{
MessageBox(NULL,"LICENSING_CHECK","DEBUG", MB_OK);
DebugBreak();
    initLic(NULL);

    funcLevel level;
    *pdwWarning=checkKeyFile(szKey, &level);
    if (FAILED (*pdwWarning) || (S_FALSE == *pdwWarning)) return CHECK_NO_FEATURES;

    switch(level)
    {
        case ::flFullFunctionality:
            return CHECK_FULL_FEATURES;
        case ::flNoUpdates:
            return CHECK_NO_UPDATES;
        default:
            return CHECK_NO_FEATURES;
    }
}

extern "C" __declspec(dllexport) DWORD __stdcall LICENSING_ADD_KEY(LPSTR pszKeyName)
{
MessageBox(NULL,"LICENSING_ADD_KEY","DEBUG", MB_OK);
DebugBreak();
    HRESULT hr=0;

    if (!pszKeyName) {
        hr=deleteActiveKey();
        if (FAILED (hr) || (S_FALSE == hr)) return FALSE;
    }
    
    if(pszKeyName) {
        hr=addKeyFile(pszKeyName);
        if (FAILED (hr) || (S_FALSE == hr)) return FALSE;
    }

    return TRUE;
}