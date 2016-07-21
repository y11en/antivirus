#include "gmutex.h"

// This function compares the passed in "suite name" string
// to the product suite information stored in the registry.
// This only works on the Terminal Server 4.0 platform.

BOOL ValidateProductSuite (LPSTR SuiteName)
{
    BOOL rVal = FALSE;
    LONG Rslt;
    HKEY hKey = NULL;
    DWORD Type = 0;
    DWORD Size = 0;
    LPSTR ProductSuite = NULL;
    LPSTR p;

    Rslt = RegOpenKeyA(
        HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Control\\ProductOptions",
        &hKey
        );

    if (Rslt != ERROR_SUCCESS)
        goto exit;

    Rslt = RegQueryValueExA( hKey, "ProductSuite", NULL, &Type, NULL, &Size );
    if (Rslt != ERROR_SUCCESS || !Size)
        goto exit;

    ProductSuite = (LPSTR) LocalAlloc( LPTR, Size );
    if (!ProductSuite)
        goto exit;

    Rslt = RegQueryValueExA( hKey, "ProductSuite", NULL, &Type,
        (LPBYTE) ProductSuite, &Size );
     if (Rslt != ERROR_SUCCESS || Type != REG_MULTI_SZ)
        goto exit;

    p = ProductSuite;
    while (*p)
    {
        if (lstrcmpA( p, SuiteName ) == 0)
        {
            rVal = TRUE;
            break;
        }
        p += (lstrlenA( p ) + 1);
    }

exit:
    if (ProductSuite)
        LocalFree( ProductSuite );

    if (hKey)
        RegCloseKey( hKey );

    return rVal;
}
// This function performs the basic check to see if
// the platform on which it is running is Terminal
// services enabled.  Note, this code is compatible on
// all Win32 platforms.  For the Windows 2000 platform
// we perform a "lazy" bind to the new product suite
// APIs that were first introduced on that platform.

BOOL IsTerminalServicesEnabled( VOID )
{
    BOOL    bResult = FALSE;    // assume Terminal Services is not enabled

    DWORD   dwVersion;
    OSVERSIONINFOEXA osVersionInfo;
    ULONGLONG dwlConditionMask = 0;
    HMODULE hmodK32 = NULL;
    HMODULE hmodNtDll = NULL;
    typedef ULONGLONG (__stdcall *PFnVerSetConditionMask)(ULONGLONG,DWORD,BYTE);
    typedef BOOL (__stdcall *PFnVerifyVersionInfoA) (POSVERSIONINFOEXA, DWORD, DWORDLONG);
    PFnVerSetConditionMask pfnVerSetConditionMask;
    PFnVerifyVersionInfoA pfnVerifyVersionInfoA;

    dwVersion = GetVersion();

    // are we running NT ?
    if (!(dwVersion & 0x80000000))
    {
        // Is it Windows 2000 (NT 5.0) or greater ?
        if (LOBYTE(LOWORD(dwVersion)) > 4)
        {
            // In Windows 2000 we need to use the Product Suite APIs
            // Don't static link because it won't load on non-Win2000 systems

            hmodNtDll = GetModuleHandleA( "ntdll.dll" );
            if (hmodNtDll != NULL)
            {
                pfnVerSetConditionMask = (PFnVerSetConditionMask )GetProcAddress( hmodNtDll, "VerSetConditionMask");
                if (pfnVerSetConditionMask != NULL)
                {
                    dwlConditionMask = (*pfnVerSetConditionMask)(dwlConditionMask, VER_SUITENAME, VER_OR);
                    hmodK32 = GetModuleHandleA( "KERNEL32.DLL" );
                    if (hmodK32 != NULL)
                    {
                        pfnVerifyVersionInfoA = (PFnVerifyVersionInfoA)GetProcAddress( hmodK32, "VerifyVersionInfoA");
                        if (pfnVerifyVersionInfoA != NULL)
                        {
                            ZeroMemory(&osVersionInfo, sizeof(osVersionInfo));
                            osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
                            osVersionInfo.wSuiteMask = VER_SUITE_TERMINAL | VER_SUITE_SINGLEUSERTS;
                            bResult = (*pfnVerifyVersionInfoA)(&osVersionInfo, VER_SUITENAME, dwlConditionMask);
                        }
                    }
                }
            }
        }
        else
        {
            // This is NT 4.0 or older
            bResult = ValidateProductSuite( "Terminal Server" );
        }
    }

    return bResult;
}


HANDLE OpenGlobalMutex(DWORD dwDesiredAccess,
                   BOOL bInheritHandle,
                   const CHAR* lpName)
{
	CHAR szMutexName[MAX_PATH];
	HANDLE hMutex;
	int len = 0;
	szMutexName[0] = 0;
	if (IsTerminalServicesEnabled())
	{
		strcpy(szMutexName, "Global\\");
		len = 7;
	}
	strncpy(szMutexName+len, lpName, MAX_PATH-len);
	szMutexName[MAX_PATH-1] = 0; // ensure zero-terminated
	hMutex = OpenMutex(dwDesiredAccess, bInheritHandle, szMutexName);
	//ODS(("Opening %s %s with err=%08X", szMutexName, hMutex ? "succeeded" : "failed", GetLastError()));
	return hMutex;
}

BOOL IsGlobalMutexExist(const TCHAR* pMutexName)
{
	HANDLE hMutex;
	hMutex = OpenGlobalMutex(MUTEX_ALL_ACCESS, FALSE, pMutexName);
	if(NULL == hMutex)
		return FALSE;
	CloseHandle(hMutex);
	return TRUE;
}
