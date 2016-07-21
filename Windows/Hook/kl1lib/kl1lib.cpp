#include <windows.h>
#include <malloc.h>
#include "kl1lib.h"
#include "winioctl.h"
#include "..\hook\kl1_api.h"

#include <stddef.h>
#include <crtdbg.h>
#include <atlconv.h>

#define KL1_OS_UNKNOWN 0
#define KL1_OS_NT      1
#define KL1_OS_9X      2

static int g_os = KL1_OS_UNKNOWN;


// Unicode to ANSI, kinda, converation
char * _UtoA(const wchar_t *wstr, int wstrlen, char *cstr)
{
    for (int i = wstrlen; i >= 0; --i) cstr[i] = (char)wstr[i];
    return cstr;
}
wchar_t * _AtoU(const char *cstr, int cstrlen, wchar_t *wstr)
{
    for (int i = cstrlen; i >= 0; --i) wstr[i] = (wchar_t)(unsigned char)cstr[i];
    return wstr;
}

#define UtoA(wstr, wstrlen)  (_UtoA(wstr, wstrlen, (char*)_alloca(wstrlen + 1)))
#define AtoU(cstr, cstrlen)  (_AtoU(cstr, cstrlen, (wchar_t*)_alloca((cstrlen + 1)*sizeof(wchar_t))))

void KL1_CheckOS()
{
    if (g_os == KL1_OS_UNKNOWN)
    {
	    OSVERSIONINFO osvi;

	    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	    if (!GetVersionEx (&osvi))
        {
		    return;
        }

	    if (osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT)
        {
            g_os = KL1_OS_NT;
        }
        else
        {
            g_os = KL1_OS_9X;
        }
    }
}

int KL1_OpenHKey (HKEY& hkey)
{
    KL1_CheckOS();

    if (g_os == KL1_OS_9X)
    {
        char *key = "System\\CurrentControlSet\\Services\\VXD\\kl1\\Parameters";

        if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, key, 0, "", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0, &hkey, 0)!= ERROR_SUCCESS)
        {
            return KL1_ERR;
        }
    }
    else
    {
        WCHAR *key = L"System\\CurrentControlSet\\Services\\kl1\\Parameters";

        if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, key, 0, L"", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0, &hkey, 0)!= ERROR_SUCCESS)
        {
            return KL1_ERR;
        }
    }


    return KL1_SUCC;
}

int KL1_CloseHKey (HKEY hkey)
{
	return RegCloseKey(hkey) == ERROR_SUCCESS ? KL1_SUCC : KL1_ERR;
}

// registers module for start during system boot
int KL1_RegisterModule (const WCHAR *name, const WCHAR* path)
{
    HKEY hkey;

    int err = KL1_OpenHKey(hkey);

    if (err != KL1_SUCC)
    {
        return err;
    }

    if (g_os == KL1_OS_9X)
    {
		USES_CONVERSION;
        const char * sname = W2A(name);
        const char * spath = W2A(path);
        if (RegSetValueExA(hkey, sname, 0, REG_SZ, (CONST BYTE*)spath, (DWORD)strlen(spath) + 1) != ERROR_SUCCESS)
        {
			KL1_CloseHKey(hkey);
            return KL1_ERR;
        }
    }
    else
    {
        if (RegSetValueExW(hkey, name, 0, REG_SZ, (CONST BYTE*)path, (DWORD)((wcslen(path) + 1)*sizeof(WCHAR))) != ERROR_SUCCESS)
        {
			KL1_CloseHKey(hkey);
            return KL1_ERR;
        }
    }

	KL1_CloseHKey(hkey);

    return KL1_SUCC;
}

// unregisters previously registered module 
int KL1_UnregisterModule (const WCHAR *name)
{
    HKEY hkey;

    int err = KL1_OpenHKey(hkey);

    if (err != KL1_SUCC)
    {
        return err;
    }

    if (g_os == KL1_OS_9X)
    {
		USES_CONVERSION;
		LONG nDelResult = RegDeleteValueA(hkey, W2A(name));
        if (nDelResult != ERROR_SUCCESS && nDelResult != ERROR_FILE_NOT_FOUND)
        {
			KL1_CloseHKey(hkey);
            return KL1_ERR;
        }
    }
    else
    {
		LONG nDelResult = RegDeleteValueW(hkey, name);
        if (nDelResult != ERROR_SUCCESS && nDelResult != ERROR_FILE_NOT_FOUND)
        {
			KL1_CloseHKey(hkey);
            return KL1_ERR;
        }
    }

	KL1_CloseHKey(hkey);

    return KL1_SUCC;
}

// checks if module is registered
int KL1_IsModuleRegistered (const WCHAR *name, const WCHAR* path)
{
    HKEY hkey;

    int err = KL1_OpenHKey(hkey);

    if (err != KL1_SUCC)
    {
        return err;
    }

    if (g_os == KL1_OS_9X)
    {
		USES_CONVERSION;
        const char * sname = W2A(name);

		char szPath[MAX_PATH];
        DWORD dwPathSize = sizeof(szPath);

        if (RegQueryValueExA(hkey, sname, 0, 0, (BYTE*)szPath, &dwPathSize) != ERROR_SUCCESS)
        {
			KL1_CloseHKey(hkey);
            return KL1_FALSE;
        }

        if (path)
        {
            const char * spath = W2A(path);
            if (strcmp(szPath, spath) != 0)
            {
                KL1_CloseHKey(hkey);
                return KL1_FALSE;
            }
        }
    }
    else
    {
		WCHAR wszPath[MAX_PATH];
        DWORD dwPathSize = sizeof(wszPath);

        if (RegQueryValueExW(hkey, name, 0, 0, (BYTE*)wszPath, &dwPathSize) != ERROR_SUCCESS)
        {
			KL1_CloseHKey(hkey);
            return KL1_FALSE;
        }

        if (path)
        {
            if (wcscmp(wszPath, path) != 0)
            {
                KL1_CloseHKey(hkey);
                return KL1_FALSE;
            }
        }
    }

	KL1_CloseHKey(hkey);

    return KL1_TRUE;
}

int KL1_SendIOCtrl(int ioctl, void *inbuf, int insize, void *outbuf, int outsize, int *retsize)
{
    KL1_CheckOS();

	HANDLE hloader;

    if (g_os == KL1_OS_9X)
		hloader = CreateFileA ("\\\\.\\KL1", 0, 0, NULL, 0, 0, NULL);
	else
        hloader = CreateFileW (L"\\\\.\\KL1", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hloader == INVALID_HANDLE_VALUE)
	{
		return KL1_NOLOADER;
	}

    int noretsize;
    if (!retsize) retsize = &noretsize;

    *retsize = 0;
	if (!DeviceIoControl (hloader, ioctl, inbuf, insize, outbuf, outsize, (DWORD*)retsize, NULL))
	{
        CloseHandle (hloader);
		return KL1_ERR;
	}

    CloseHandle (hloader);
    return KL1_SUCC;
}

// loads and starts module 
int KL1_LoadModule (const WCHAR *name, const WCHAR* path)
{
    KL1_CheckOS();
    KL1_MODULE_CONTEXT context;

	USES_CONVERSION;
    strcpy(context.ModuleName, W2A(name));

    if (g_os == KL1_OS_9X)
	{
		CHAR szOemPath[MAX_PATH];
		if(GetShortPathNameA(W2A(path), szOemPath, sizeof(szOemPath)) == 0)
			return KL1_ERR;
		CharToOem(szOemPath, szOemPath);
		wcscpy(context.ModulePath, AtoU(szOemPath, (int)strlen(szOemPath)));
	}
	else
	{
		wcscpy(context.ModulePath, path);
	}

    return KL1_SendIOCtrl(KL1_LOAD_MODULE, &context, sizeof(context), NULL, 0, NULL);
}

// stops and unloads module
int KL1_UnloadModule (const WCHAR *name)
{
    KL1_MODULE_CONTEXT context;

	USES_CONVERSION;
    strcpy(context.ModuleName, W2A(name));

    return KL1_SendIOCtrl(KL1_UNLOAD_MODULE, &context, sizeof(context), NULL, 0, NULL);
}

// checks if module is loaded
int KL1_IsModuleLoadedW (const WCHAR *name)
{
    /*
    KL1_MODULE_CONTEXT context;

    strcpy(context.ModuleName, UtoA(name, wcslen(name)));

    int loaded = 0;
    int ret = 0;

    int err = KL1_SendIOCtrl(KL1_IS_MODULE_LOADED, &context, sizeof(context), &loaded, sizeof(loaded), &ret);
    if (err != KL1_SUCC)
    {
        return err;
    }

    if (ret < sizeof(loaded))
    {
        return KL1_ERR;
    }

    if (!loaded)
    {
        return KL1_FALSE;
    }

    return KL1_TRUE;
    */

    return KL1_ERR;
}
