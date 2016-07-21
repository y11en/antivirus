#include <windows.h>
#include <string.h>
#include <shlwapi.h>

#define countof(array) (sizeof (array) / sizeof (*array))
#define LCASE(c) (c >= 'A' ? ( c <= 'Z' ? (c | 0x20) : c ) : c)

wchar_t * __cdecl __wcsistr (
        const wchar_t * wcs1,
        const wchar_t * wcs2
        )
{
        wchar_t *cp = (wchar_t *) wcs1;
        wchar_t *s1, *s2;

        while (*cp)
        {
                s1 = cp;
                s2 = (wchar_t *) wcs2;

                while ( *s1 && *s2 && !(LCASE(*s1)-LCASE(*s2)) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);
}

extern HANDLE gModuleHandle;

HRESULT RegisterAppInitDllsW(BOOL bRegister)
{
    HRESULT hRes;
    WCHAR sLongFileName[MAX_PATH*2];
    WCHAR sFileName[MAX_PATH*2];
    HKEY hKey;
    DWORD dwLen;
    BOOL bVista = (DWORD)(LOBYTE(LOWORD(GetVersion()))) > 5;
    dwLen = GetModuleFileNameW(gModuleHandle, sLongFileName, countof(sLongFileName));
    if (dwLen == 0 || dwLen >= countof(sLongFileName)-1)
        return E_UNEXPECTED;
	
	// с кавычками не работает
	dwLen = GetShortPathNameW(sLongFileName, sFileName, countof(sFileName)-1);
	if (dwLen == 0 || dwLen >= countof(sFileName)-1)
		return E_UNEXPECTED;
	sFileName[dwLen+1] = 0;

    if (ERROR_SUCCESS == (hRes = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows", &hKey)))
    {
        WCHAR sAppInit[MAX_PATH*2];
        DWORD dwType = REG_SZ;
        DWORD dwSize = sizeof(sAppInit);
        DWORD nAppInitLen = countof(sAppInit);
        hRes = RegQueryValueExW(hKey, L"AppInit_Dlls", NULL, &dwType, (LPBYTE)sAppInit, &dwSize);
        sAppInit[nAppInitLen-1] = 0;
        if (hRes == ERROR_FILE_NOT_FOUND)
        {
            hRes = ERROR_SUCCESS;
            sAppInit[0] = 0;
        }
        if (ERROR_SUCCESS == hRes)
        {
            WCHAR* pExist, *pNext;
            pExist = __wcsistr(sAppInit, sFileName);
            if (pExist)
            {
                if (bRegister)
                {
                    hRes = S_OK;
                }
                else
                {
                    size_t size;
                    pNext = pExist + wcslen(sFileName);
					// if was registered as quoted - include quotes
					if (pExist != sAppInit && pExist[-1]=='\"' && pNext[0]=='\"')
					{
						pExist--;
						pNext++;
					}
					// skip delimiters until next
					while (*pNext == ' ' || *pNext == ',')
						pNext++;
					// find prev, if any
					while (pExist != sAppInit)
					{	
						if (pExist[-1] == ' ' || (pExist[-1] == ',' && *pNext == 0))
							pExist--;
						else
							break;
					}
                    size = wcslen(pNext)+1;
                    memmove(pExist, pNext, size*sizeof(wchar_t));
                    dwType = REG_SZ;
                    dwSize = (DWORD)(wcslen(sAppInit)+1)*sizeof(wchar_t);
                    if (ERROR_SUCCESS == (hRes = RegSetValueExW(hKey, L"AppInit_Dlls", 0, dwType, (LPBYTE)sAppInit, dwSize)))
                    {
                        hRes = S_OK;
                    }
                    if (bVista && wcslen(sAppInit) == 0)
                    {
						DWORD dw0 = 0;
						RegSetValueExW(hKey, L"LoadAppInit_Dlls", 0, REG_DWORD, (BYTE*)&dw0, sizeof(dw0));
                    }
                }
            }
            else
            {
                if (bRegister)
                {
                    size_t size = wcslen(sAppInit);
                    size_t copy;
                    if (size && size < nAppInitLen)
                        sAppInit[size++] = ',';
                    copy = min(wcslen(sFileName)+1, nAppInitLen-size);
                    memcpy(sAppInit+size, sFileName, copy*sizeof(wchar_t));
                    sAppInit[nAppInitLen-1] = 0;
                    dwType = REG_SZ;
                    dwSize = (DWORD)(wcslen(sAppInit)+1)*sizeof(wchar_t);
                    // На Vista чтобы работал AppInit_Dlls, нужно выставить LoadAppInit_Dlls в 1
                    {
                        DWORD dw1 = 1;
                        if (ERROR_SUCCESS == (hRes = RegSetValueExW(hKey, L"AppInit_Dlls", 0, dwType, (LPBYTE)sAppInit, dwSize))
                            && (!bVista || ERROR_SUCCESS == (hRes = RegSetValueExW(hKey, L"LoadAppInit_Dlls", 0, REG_DWORD, (BYTE*)&dw1, sizeof(dw1)))))
                        {
                            hRes = S_OK;
                        }
                    }
                }
                else
                {
                    hRes = S_OK;
                }
            }
        }
        RegCloseKey(hKey);
    }

	if ( !bRegister )
	{
		// remove HKLM\SOFTWARE\KasperskyLab\Protected\R3H key from both wow6432 and native branches
		// (on a 64-bit machine)
		SHDeleteKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\KasperskyLab\\Protected\\R3H");
	}

    return hRes;
}

STDAPI DllRegisterServer(void)
{
    return RegisterAppInitDllsW(TRUE);
}

STDAPI DllUnregisterServer(void)
{
    return RegisterAppInitDllsW(FALSE);
}
