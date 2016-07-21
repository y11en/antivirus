
#include <windows.h>
#include "vkbd.h"

HINSTANCE g_hInstance = NULL;

//--------------------------- DllMain -------------------------------

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if( dwReason == DLL_PROCESS_ATTACH )
    {
        g_hInstance = hInstance;
    }
    return 1 ; // ok
}

//--------------------------- DllCanUnloadNow -----------------------

STDAPI DllCanUnloadNow()
{
    return S_OK;
}

//--------------------------- DllGetClassObject ---------------------

STDAPI DllGetClassObject (REFCLSID rclsid, REFIID riid, LPVOID * ppvOut)
{
    *ppvOut = NULL;
    return CLASS_E_CLASSNOTAVAILABLE;
}

//--------------------------- DllRegisterServer ---------------------

STDAPI DllRegisterServer()
{
	return S_OK;//RegisterSputnikDllByHandle("user32.dll", g_hInstance);
}

//--------------------------- DllUnregisterServer -------------------

STDAPI DllUnregisterServer()
{
	return S_OK;//UnregisterSputnikDllByHandle("user32.dll", g_hInstance);
}

//-------------------------------------------------------------------
