
#include <windows.h>
#include <tchar.h>
#include <OLECTL.h>

#include "ShellExtClassFactory.h"

extern UINT g_cRefThisDll ;

HINSTANCE     g_hmodThisDll = NULL ; // Handle to this DLL itself.
OSVERSIONINFO OS ;

//--------------------------- DllMain -------------------------------

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		OS.dwOSVersionInfoSize = sizeof(OS) ;
		::GetVersionEx(&OS) ;

       // Extension DLL one-time initialization
        g_hmodThisDll = hInstance ;
    }

    return 1 ; // ok
}

//--------------------------- DllCanUnloadNow -----------------------

STDAPI DllCanUnloadNow()
{
    return g_cRefThisDll ? S_FALSE : S_OK ;
}

//--------------------------- DllGetClassObject ---------------------

STDAPI DllGetClassObject (REFCLSID rclsid, REFIID riid, LPVOID * ppvOut)
{
    *ppvOut = NULL ;
    
    if  (::IsEqualIID (rclsid, CLSID_ShellExtension))
    {
        CShellExtClassFactory * pcf = new CShellExtClassFactory ;
        return pcf->QueryInterface (riid, ppvOut) ;
    }

    return CLASS_E_CLASSNOTAVAILABLE ;
}

//--------------------------- DllRegisterServer ---------------------
STDAPI DllRegisterServer()
{
    HKEY hKey ;
    
    TCHAR szModuleName [ _MAX_PATH ] ;
    ::GetModuleFileName (g_hmodThisDll, szModuleName, _MAX_PATH) ;
    
    if (::RegCreateKey(HKEY_CLASSES_ROOT, _T("*\\shellex\\ContextMenuHandlers\\Kaspersky Anti-Virus"), &hKey) != ERROR_SUCCESS)
    {
        return SELFREG_E_CLASS ;
    }

    ::RegSetValueEx(hKey, _T(""), 0, REG_SZ, (BYTE*)_T(SHLEX_CLASSID), (_tcslen(_T(SHLEX_CLASSID)) + 1) * sizeof(TCHAR)) ;
    ::RegCloseKey(hKey) ;
    
    if (::RegCreateKey(HKEY_CLASSES_ROOT, _T("CLSID\\{dd230880-495a-11d1-b064-008048ec2fc5}\\InProcServer32"), &hKey) != ERROR_SUCCESS)
    {
        return SELFREG_E_CLASS ;
    }

    ::RegSetValueEx(hKey, _T(""), 0, REG_SZ, (BYTE*)szModuleName, (_tcslen(szModuleName) + 1) * sizeof(TCHAR)) ;
    ::RegSetValueEx(hKey, _T("ThreadingModel"), 0, REG_SZ, (BYTE*) _T("Apartment"), (_tcslen(_T("Apartment")) + 1) * sizeof(TCHAR)) ;
    ::RegCloseKey(hKey) ;
    
    if (::RegCreateKey(HKEY_CLASSES_ROOT, _T("Drive\\shellex\\ContextMenuHandlers\\Kaspersky Anti-Virus"), &hKey) != ERROR_SUCCESS)
    {
        return SELFREG_E_CLASS ;
    }

    ::RegSetValueEx(hKey, _T(""), 0, REG_SZ, (BYTE*)_T(SHLEX_CLASSID), (_tcslen(_T(SHLEX_CLASSID)) + 1) * sizeof(TCHAR)) ;
    ::RegCloseKey(hKey) ;
    
    if (::RegCreateKey(HKEY_CLASSES_ROOT, _T("Folder\\shellex\\ContextMenuHandlers\\Kaspersky Anti-Virus"), &hKey) != ERROR_SUCCESS)
    {
        return SELFREG_E_CLASS ;
    }

    ::RegSetValueEx(hKey, _T(""), 0, REG_SZ, (BYTE*)_T(SHLEX_CLASSID), (_tcslen(_T(SHLEX_CLASSID)) + 1) * sizeof(TCHAR)) ;
    ::RegCloseKey(hKey) ;
    
    // NT needs to have shell extensions "approved".
    OS.dwOSVersionInfoSize = sizeof(OS) ;
    ::GetVersionEx(&OS) ;

    if (OS.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if (::RegCreateKey(HKEY_CLASSES_ROOT, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), &hKey) != ERROR_SUCCESS)
        {
            return SELFREG_E_CLASS ;
        }

        TCHAR szValue[_MAX_PATH] ;
        _tcscpy(szValue, _T("Scan with ")) ;
        _tcscat(szValue, _T("Kaspersky Anti-Virus")) ;
        
        ::RegSetValueEx(hKey, _T(SHLEX_CLASSID), 0, REG_SZ, (BYTE*)LPCTSTR(szValue), (_tcslen(szValue) + 1) * sizeof(TCHAR)) ;
        ::RegCloseKey(hKey) ;
    }

    return S_OK ;
}

//--------------------------- DllUnregisterServer -------------------

STDAPI DllUnregisterServer()
{
    if  ( ::RegDeleteKey ( HKEY_CLASSES_ROOT, _T("*\\shellex\\ContextMenuHandlers\\Kaspersky Anti-Virus") ) != ERROR_SUCCESS )
    {
        return SELFREG_E_CLASS;
    }
    
    if  ( ::RegDeleteKey ( HKEY_CLASSES_ROOT, _T("CLSID\\{dd230880-495a-11d1-b064-008048ec2fc5}\\InProcServer32") ) != ERROR_SUCCESS )
    {
        return SELFREG_E_CLASS;
    }
    
    if  ( ::RegDeleteKey ( HKEY_CLASSES_ROOT, _T("Drive\\shellex\\ContextMenuHandlers\\Kaspersky Anti-Virus") ) != ERROR_SUCCESS )
    {
        return SELFREG_E_CLASS;
    }
    
    if  ( ::RegDeleteKey ( HKEY_CLASSES_ROOT, _T("Folder\\shellex\\ContextMenuHandlers\\Kaspersky Anti-Virus") ) != ERROR_SUCCESS )
    {
        return SELFREG_E_CLASS;
    }

    // NT needs to have shell extensions "approved".
    OS.dwOSVersionInfoSize = sizeof(OS) ;
    ::GetVersionEx(&OS) ;

    if (OS.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        HKEY hKey = NULL ;
        if (::RegOpenKey(HKEY_CLASSES_ROOT, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), &hKey) == ERROR_SUCCESS)
        {
            ::RegDeleteValue(hKey, _T(SHLEX_CLASSID)) ;
            ::RegCloseKey(hKey) ;
        }
    }

    return S_OK ;
}

//-------------------------------------------------------------------
