#include "stdafx.h"
#include <io.h>
#include "Services.h"
#include "avpsrvex.h"
#include "UnicodeConv.h"
#include "../../windows/Hook/kl1lib/kl1lib.h"

int g_bUseKL1 = 1;

/*
static tstring CKAHUM_GetSvcName (LPCWSTR szwFullPath)
{
	tstring sFullPath (__LPCTSTR (szwFullPath));
	LPCTSTR szFullPath = sFullPath.c_str ();

	LPCTSTR pLastSlash = _tcsrchr (szFullPath, _T('\\'));
	LPCTSTR pLastDot = _tcsrchr (szFullPath, _T('.'));

	LPCTSTR pName = pLastSlash ? (pLastSlash + 1) : szFullPath;

	if (*pName)
	{
		if (pLastDot)
			return tstring (pName, pLastDot);
		else
			return tstring (pName);
	}
	else
		return tstring ();
}
*/

bool CKAHUM_RegisterPlugin (LPCWSTR szwPluginServiceName, LPCWSTR szwPluginFileName, DWORD dwStartType)
{
	log.WriteFormat (_T("[REGPLUGIN] Trying to register plugin '%ls'"), PEL_INFO, szwPluginFileName);

    if (g_bUseKL1)
    {
		log.WriteFormat (_T("[REGPLUGIN] Not necessary to register plugins using KL1"), PEL_INFO);
        return true;
    }

	if (!g_bIsNT)
	{
		log.WriteFormat (_T("[REGPLUGIN] Not necessary to register plugins under Win 9x"), PEL_INFO);
		return true;
	}

	if (IOSGetFileAttributesW (szwPluginFileName) == 0xffffffff)
	{
		log.WriteFormat (_T("[REGPLUGIN] File '%ls' doesn't exist"), PEL_ERROR, szwPluginFileName);
		return false;
	}

	//tstring sSvcName = CKAHUM_GetSvcName (szwPluginFileName);
    LPCTSTR szSvcName = __LPCTSTR (szwPluginServiceName);

	HSRV hSrv = SRV_InitEx (NULL, szSvcName);

	if (hSrv == NULL)
	{
		log.WriteFormat (_T("[REGPLUGIN] Unable to initialize service for '%ls'"), PEL_ERROR, szwPluginFileName);
		return false;
	}

	bool bRetValue;
	
	bRetValue = SRV_InstallExW (hSrv, (LPWSTR)szwPluginServiceName, SERVICE_KERNEL_DRIVER, 
										dwStartType, NULL, (LPWSTR)szwPluginFileName, NULL, NULL);

	if (bRetValue)
		log.WriteFormat (_T("[REGPLUGIN] Service '%s' for plugin '%ls' successfully created"), PEL_INFO, szSvcName, szwPluginFileName);
	else
		log.WriteFormat (_T("[REGPLUGIN] Service '%s' for plugin '%ls' not created (0x%08x)"), PEL_ERROR, szSvcName, szwPluginFileName, GetLastError ());

	SRV_Done (hSrv);

	return bRetValue;
}

bool CKAHUM_UnregisterPlugin (LPCWSTR szPluginServiceName, bool bStopBeforeUnregister)
{
	bool bRetValue = false;

	log.WriteFormat (_T("[UNREGPLUGIN] Trying to unregister plugin '%ls'"), PEL_INFO, szPluginServiceName);

	if (g_bUseKL1)
	{
		log.WriteFormat (_T("[UNREGPLUGIN] Not necessary to unregister plugins using KL1"), PEL_INFO);
		return true;
	}

	if (!g_bIsNT)
	{
		log.WriteFormat (_T("[UNREGPLUGIN] Not necessary to unregister plugins under Win 9x"), PEL_INFO);
		return true;
	}

	HSRV hSrv = SRV_InitEx (NULL, __LPCTSTR (szPluginServiceName));

	if (hSrv == NULL)
	{
		log.WriteFormat (_T("[UNREGPLUGIN] Unable to initialize service for '%ls' (0x%08x)"), PEL_ERROR, szPluginServiceName, GetLastError ());
		return false;
	}

	if (!SRV_IsInstalled (hSrv))
	{
		log.WriteFormat (_T("[UNREGPLUGIN] Service '%ls' not installed"), PEL_WARNING, szPluginServiceName);
		bRetValue = true;
	}
	else
	{
		bRetValue = bStopBeforeUnregister ? SRV_Remove (hSrv) : SRV_RemoveNoStop (hSrv);

		if (bRetValue)
			log.WriteFormat (_T("[UNREGPLUGIN] Service '%ls' successfully removed"), PEL_INFO, szPluginServiceName);
		else
			log.WriteFormat (_T("[UNREGPLUGIN] Service '%ls' not removed (0x%08x)"), PEL_ERROR, szPluginServiceName, GetLastError ());
	}

	SRV_Done (hSrv);

	return bRetValue;
}

bool CKAHUM_IsPluginRegistered (LPCWSTR szPluginServiceName, bool &bIsRegistered)
{
    return CKAHUM_IsPluginRegistered(szPluginServiceName, NULL, bIsRegistered);
}

bool CKAHUM_IsPluginRegistered (LPCWSTR szPluginServiceName, LPCWSTR szPluginFileName, bool &bIsRegistered)
{
	bIsRegistered = false;

	log.WriteFormat (_T("[ISREGPLUGIN] Is plugin '%ls' registered?"), PEL_INFO, szPluginServiceName);

	if (g_bUseKL1)
	{
		log.WriteFormat (_T("[ISREGPLUGIN] Using KL1 plugins don't require registration"), PEL_INFO);
		bIsRegistered = true;
		return true;
	}

	if (!g_bIsNT)
	{
		log.WriteFormat (_T("[ISREGPLUGIN] Under Win 9x plugins don't require registration"), PEL_INFO);
		bIsRegistered = true;
		return true;
	}

	HSRV hSrv = SRV_InitEx (NULL, __LPCTSTR (szPluginServiceName));

	if (hSrv == NULL)
	{
		log.WriteFormat (_T("[ISREGPLUGIN] Unable to initialize service for '%ls' (0x%08x)"), PEL_ERROR, szPluginServiceName, GetLastError ());
		return false;
	}

	bIsRegistered = SRV_IsInstalled (hSrv);

	if (bIsRegistered)
		log.WriteFormat (_T("[ISREGPLUGIN] Plugin '%ls' is registered"), PEL_INFO, szPluginServiceName);
	else
		log.WriteFormat (_T("[ISREGPLUGIN] Plugin '%ls' is not registered"), PEL_INFO, szPluginServiceName);

	SRV_Done (hSrv);

	return true;
}

bool CKAHUM_StartPlugin (LPCWSTR szwPluginServiceName, LPCWSTR szwPluginFileName)
{
	bool bRetValue = false;

	log.WriteFormat (_T("[STARTPLUGIN] Trying to start plugin '%ls' (file '%ls')"), PEL_INFO, szwPluginServiceName, szwPluginFileName);

    if (g_bUseKL1)
    {
        int err = KL1_LoadModule(szwPluginServiceName, szwPluginFileName);
            
        if (err != KL1_SUCC)
        {
			log.WriteFormat (_T("[STARTPLUGIN] Failed to start module '%ls': %d"), PEL_ERROR, szwPluginFileName, err);
            return false;
        }
		log.WriteFormat (_T("[STARTPLUGIN] Module '%ls' successfully started"), PEL_INFO, szwPluginFileName);
        return true;
    }

	if (g_bIsNT)
	{
		//tstring sSvcName = CKAHUM_GetSvcName (szPluginFileName);
        LPCTSTR szPluginServiceName = __LPCTSTR (szwPluginServiceName);

		HSRV hSrv = SRV_InitEx (NULL, szPluginServiceName);

		if (hSrv == NULL)
		{
			log.WriteFormat (_T("[STARTPLUGIN] Unable to initialize service for '%ls' (0x%08x)"), PEL_ERROR, szwPluginFileName, GetLastError ());
			return false;
		}

		if (!SRV_IsInstalled (hSrv))
		{
			log.WriteFormat (_T("[STARTPLUGIN] Service '%ls' not installed"), PEL_ERROR, szwPluginFileName);
		}
		else
		{
			bRetValue = SRV_Start (hSrv);

			if (bRetValue)
				log.WriteFormat (_T("[STARTPLUGIN] Service '%ls' successfully started"), PEL_INFO, szwPluginServiceName);
			else
				log.WriteFormat (_T("[STARTPLUGIN] Service '%ls' not started (0x%08x)"), PEL_ERROR, szwPluginServiceName, GetLastError ());
		}

		SRV_Done (hSrv);
	}
	else
	{
		TCHAR szName [MAX_PATH + 5] = _T("\\\\.\\");
		CHAR szOemName [MAX_PATH + 5];
		IOSGetShortPathName (__LPCTSTR (szwPluginFileName), szName + 4, MAX_PATH);
		CharToOem(szName, szOemName);

		bRetValue = CreateFile (szOemName, 0, 0, NULL, 0, 0, NULL) != INVALID_HANDLE_VALUE;

		if (bRetValue)
			log.WriteFormat (_T("[STARTPLUGIN] Service '%s' successfully started"), PEL_INFO, szName);
		else
			log.WriteFormat (_T("[STARTPLUGIN] Service '%s' not started (0x%08x)"), PEL_ERROR, szName, GetLastError ());
	}

	return bRetValue;
}

bool CKAHUM_StopPlugin (LPCWSTR szPluginServiceName)
{
	bool bRetValue = false;

    if (g_bUseKL1)
    {
		log.WriteFormat (_T("[STOPPLUGIN] Trying to stop module '%ls'"), PEL_INFO, szPluginServiceName);
        
        int err = KL1_UnloadModule(szPluginServiceName);

        if (err != KL1_SUCC)
        {
			log.WriteFormat (_T("[STOPPLUGIN] Failed to stop module '%ls': %d"), PEL_ERROR, szPluginServiceName, err);
            return false;
        }
		log.WriteFormat (_T("[STOPPLUGIN] Module '%ls' successfully stopped"), PEL_INFO, szPluginServiceName);

        return true;
    }

	if (g_bIsNT)
	{
		log.WriteFormat (_T("[STOPPLUGIN] Trying to stop plugin '%ls'"), PEL_INFO, szPluginServiceName);

		HSRV hSrv = SRV_InitEx (NULL, __LPCTSTR (szPluginServiceName));

		if (hSrv == NULL)
		{
			log.WriteFormat (_T("[STOPPLUGIN] Unable to initialize service for '%ls' (0x%08x)"), PEL_ERROR, szPluginServiceName, GetLastError ());
			return false;
		}

		if (!SRV_IsInstalled (hSrv))
		{
			log.WriteFormat (_T("[STOPPLUGIN] Service '%ls' not installed"), PEL_ERROR, szPluginServiceName);
		}
		else
		{
			bRetValue = true;

			SRV_Stop (hSrv);

			log.WriteFormat (_T("[STOPPLUGIN] Service '%ls' successfully stopped"), PEL_INFO, szPluginServiceName);
		}

		SRV_Done (hSrv);
	}
	else
	{
		TCHAR szName [MAX_PATH + 9];
		wsprintf (szName, _T("\\\\.\\%s.VXD"), _tcsupr (__LPCTSTR (szPluginServiceName)));

		log.WriteFormat (_T("[STOPPLUGIN] Trying to stop service '%s'"), PEL_INFO, szName);

		bRetValue = (DeleteFile (szName) == 0); // Q139999

		if (bRetValue)
			log.WriteFormat (_T("[STOPPLUGIN] Service '%ls' successfully stopped"), PEL_INFO, szPluginServiceName);
		else
			log.WriteFormat (_T("[STOPPLUGIN] Service '%ls' not stopped (0x%08x)"), PEL_ERROR, szPluginServiceName, GetLastError ());
	}

	return bRetValue;
}

static const TCHAR szWin9xVxdRegPath[] = _T("System\\CurrentControlSet\\Services\\VxD\\");

bool CKAHUM_RegisterBaseDrv (LPCWSTR szwPluginServiceName, LPCWSTR szwPluginFileName)
{
	log.WriteFormat (_T("[REGBD] Trying to register driver '%ls'('%ls')"), PEL_INFO, szwPluginServiceName, szwPluginFileName);

    if (g_bUseKL1)
    {
        int err = KL1_RegisterModule(szwPluginServiceName, szwPluginFileName);

        if (err != KL1_SUCC)
        {
	        log.WriteFormat (_T("[REGBD] Failed to register module '%ls': %d"), PEL_INFO, szwPluginServiceName, err);
            return false;
        }

	    log.WriteFormat (_T("[REGBD] Module '%ls' is registered"), PEL_INFO, szwPluginServiceName);
        return true;
    }

	if (g_bIsNT)
	{
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx (&osvi))
			return false;

		bool bNT4 = osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion <= 4;

		return CKAHUM_RegisterPlugin (szwPluginServiceName, szwPluginFileName, bNT4 ? SERVICE_SYSTEM_START : SERVICE_BOOT_START);
	}

	LPCTSTR szPluginFileName = __LPCTSTR (szwPluginFileName);

	// Win 9x
    LPCTSTR szSvcName = __LPCTSTR (szwPluginServiceName);
    /*
	TCHAR szSvcName[_MAX_FNAME];
	_tsplitpath (szPluginFileName, NULL, NULL, szSvcName, NULL);
    */

	TCHAR szRegPath[255];

	_tcscpy (szRegPath, szWin9xVxdRegPath);
	_tcscat (szRegPath, szSvcName);

	HKEY hValKey = NULL;
	LONG lRegResult;

	if ((lRegResult = RegCreateKeyEx (HKEY_LOCAL_MACHINE, szRegPath, 0, _T(""), 
							REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hValKey, NULL)) != ERROR_SUCCESS)
	{
		log.WriteFormat (_T("[REGBD] Failed to create registry key (0x%08x)"), PEL_ERROR, lRegResult);
		return false;
	}

	if ((lRegResult = RegSetValueEx (hValKey, _T("StaticVxD"), NULL, REG_SZ, (LPBYTE)szPluginFileName, (_tcslen (szPluginFileName) + 1) * sizeof (TCHAR))) != ERROR_SUCCESS)
		log.WriteFormat (_T("[REGBD] Failed to create StaticVxD registry key (0x%08x) for driver '%ls'"), PEL_ERROR, lRegResult, szPluginFileName);

	DWORD dwStart = 0;
	if ((lRegResult = RegSetValueEx (hValKey, _T("Start"), NULL, REG_DWORD, (LPBYTE)&dwStart, sizeof (dwStart))) != ERROR_SUCCESS)
		log.WriteFormat (_T("[REGBD] Failed to create Start registry key (0x%08x) for driver '%ls'"), PEL_ERROR, lRegResult, szPluginFileName);

	RegCloseKey (hValKey);

	log.WriteFormat (_T("[REGBD] Base driver '%ls' is registered"), PEL_INFO, szwPluginFileName);

	return true;
}

bool CKAHUM_UnregisterBaseDrv (LPCWSTR szwPluginServiceName, bool bStopBeforeUnregister)
{
	log.WriteFormat (_T("[UNREGBD] Trying to unregister driver '%ls'"), PEL_INFO, szwPluginServiceName);

    if (g_bUseKL1)
    {
        int err = KL1_UnregisterModule(szwPluginServiceName);
            
        if (err != KL1_SUCC)
        {
		    log.WriteFormat (_T("[UNREGBD] Failed to unregister module '%ls': %d"), PEL_ERROR, szwPluginServiceName, err);
            return false;
        }

		log.WriteFormat (_T("[UNREGBD] Module '%ls' successfully unregistered"), PEL_INFO, szwPluginServiceName);
        return true;
    }

	if (g_bIsNT)
    {
		return CKAHUM_UnregisterPlugin (szwPluginServiceName, bStopBeforeUnregister);
    }

	LPCTSTR szPluginServiceName = __LPCTSTR (szwPluginServiceName);

	// Win 9x
    /*
	TCHAR szSvcName[_MAX_FNAME];
	_tsplitpath (szPluginServiceName, NULL, NULL, szSvcName, NULL);
    */

	TCHAR szRegPath[255];

	_tcscpy (szRegPath, szWin9xVxdRegPath);
	_tcscat (szRegPath, szPluginServiceName);

	LONG lDelResult = RegDeleteKey (HKEY_LOCAL_MACHINE, szRegPath);
	
	bool bRetValue = (lDelResult == ERROR_SUCCESS);

	if (bRetValue)
		log.WriteFormat (_T("[UNREGBD] Driver '%s' successfully unregistered"), PEL_INFO, szPluginServiceName);
	else
		log.WriteFormat (_T("[UNREGBD] Failed to unregister driver '%s' (0x%08x)"), PEL_ERROR, szPluginServiceName, lDelResult);

	return bRetValue;
}

bool CKAHUM_IsBaseDrvRegistered (LPCWSTR szwPluginServiceName, bool &bIsRegistered)
{
    return CKAHUM_IsBaseDrvRegistered(szwPluginServiceName, NULL, bIsRegistered);
}

bool CKAHUM_IsBaseDrvRegistered (LPCWSTR szwPluginServiceName, LPCWSTR szBaseDrvFileName, bool &bIsRegistered)
{
	bIsRegistered = false;

	log.WriteFormat (_T("[ISREGBD] Is base driver '%ls' registered?"), PEL_INFO, szwPluginServiceName);

    if (g_bUseKL1)
    {
        bIsRegistered = (KL1_IsModuleRegistered(szwPluginServiceName, szBaseDrvFileName) == KL1_TRUE);
    }
    else
    {
	    if (g_bIsNT)
        {
		    return CKAHUM_IsPluginRegistered (szwPluginServiceName, szBaseDrvFileName, bIsRegistered);
        }

	    LPCTSTR szPluginServiceName = __LPCTSTR (szwPluginServiceName);

	    // Win 9x
        /*
	    TCHAR szSvcName[_MAX_FNAME];
	    _tsplitpath (szPluginServiceName, NULL, NULL, szSvcName, NULL);
        */

	    TCHAR szRegPath[255];

	    _tcscpy (szRegPath, szWin9xVxdRegPath);
	    _tcscat (szRegPath, szPluginServiceName);

	    HKEY hValKey = NULL;

	    bIsRegistered = RegOpenKeyEx (HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_READ, &hValKey) == ERROR_SUCCESS;
	    
	    if (bIsRegistered)
		    RegCloseKey (hValKey);
    }

	if (bIsRegistered)
		log.WriteFormat (_T("[ISREGBD] Base driver '%ls' is registered"), PEL_INFO, szwPluginServiceName);
	else
		log.WriteFormat (_T("[ISREGBD] Base driver '%ls' is not registered"), PEL_INFO, szwPluginServiceName);

	return true;
}

bool CKAHUM_SetBaseDrvStartType (LPCWSTR szBaseDrvServiceName, DWORD dwStartType)
{
	log.WriteFormat (_T("[SETSTARTTYPE] Setting start type of base driver '%ls' to %d"), PEL_INFO, szBaseDrvServiceName, dwStartType);

	if (!g_bIsNT)
	{
		log.WriteFormat (_T("[SETSTARTTYPE] Not necessary set start type under Win9x"), PEL_INFO);
		return true;
	}

	HSRV hSrv = SRV_InitEx (NULL, __LPCTSTR (szBaseDrvServiceName));

	if (hSrv == NULL)
	{
		log.WriteFormat (_T("[SETSTARTTYPE] Unable to initialize service for '%ls' (0x%08x)"), PEL_ERROR, szBaseDrvServiceName, GetLastError ());
		return false;
	}

	bool bRetValue;
	
	if ((bRetValue = SRV_SetStartupType (hSrv, dwStartType)) == true)
		log.WriteFormat (_T("[SETSTARTTYPE] Start type of '%ls' is set to %d"), PEL_INFO, szBaseDrvServiceName, dwStartType);
	else
		log.WriteFormat (_T("[SETSTARTTYPE] Failed to set start type of '%ls' to %d (0x%08x)"), PEL_ERROR, szBaseDrvServiceName, dwStartType, GetLastError ());

	SRV_Done (hSrv);

	return bRetValue;
}

bool CKAHUM_GetBaseDrvStartType (LPCWSTR szBaseDrvServiceName, DWORD *pdwStartType)
{
	log.WriteFormat (_T("[GETSTARTTYPE] Getting start type of base driver '%ls'"), PEL_INFO, szBaseDrvServiceName);

	if (!g_bIsNT)
	{
		log.WriteFormat (_T("[GETSTARTTYPE] No start type under Win9x"), PEL_INFO);
		return true;
	}

	HSRV hSrv = SRV_InitEx (NULL, __LPCTSTR (szBaseDrvServiceName));

	if (hSrv == NULL)
	{
		log.WriteFormat (_T("[GETSTARTTYPE] Unable to initialize service for '%ls' (0x%08x)"), PEL_ERROR, szBaseDrvServiceName, GetLastError ());
		return false;
	}

	if ((*pdwStartType = SRV_GetStartupType (hSrv)) != SRV_RC_ERROR)
		log.WriteFormat (_T("[GETSTARTTYPE] Start type of '%ls' is %d"), PEL_INFO, szBaseDrvServiceName, *pdwStartType);
	else
		log.WriteFormat (_T("[GETSTARTTYPE] Failed to get start type of '%ls' (0x%08x)"), PEL_ERROR, szBaseDrvServiceName, GetLastError ());

	SRV_Done (hSrv);

	return *pdwStartType != SRV_RC_ERROR;
}

bool CKAHUM_EnableBaseDriver (LPCWSTR szBaseDrvServiceName, LPCWSTR szBaseDrvFileName, bool bEnable, bool *pbNeedReboot)
{
	log.WriteFormat (_T("[ENABLEBD] %s base driver '%ls'"), PEL_INFO, bEnable ? _T("Enabling") : _T("Disabling"), szBaseDrvServiceName);

	bool bRetValue = false;
	bool bNeedReboot = false;

    if (g_bUseKL1)
    {
		if (bEnable)
		{
			bool bRegistered = (KL1_IsModuleRegistered(szBaseDrvServiceName, szBaseDrvFileName) == KL1_TRUE);

			if (!bRegistered)
			{
				bRetValue = KL1_RegisterModule (szBaseDrvServiceName, szBaseDrvFileName) == KL1_SUCC;
				if(bRetValue)
					bNeedReboot = true;
			}
			else
			{
				bRetValue = true;
			}
		}
		else
		{
			bRetValue = KL1_UnregisterModule (szBaseDrvServiceName) == KL1_SUCC;
			if(bRetValue)
				bNeedReboot = true;
		}
    }
    else
    {
	    if (g_bIsNT)
	    {
		    OSVERSIONINFO osvi;
		    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		    bool bNT4 = GetVersionEx (&osvi) && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion <= 4;
			DWORD dwStartType = -1;
			DWORD dwRequiredStartType = bEnable ? (bNT4 ? SERVICE_SYSTEM_START : SERVICE_BOOT_START) : SERVICE_DISABLED;

			CKAHUM_GetBaseDrvStartType(szBaseDrvServiceName, &dwStartType);
			if(dwStartType != dwRequiredStartType)
			{
				bRetValue = CKAHUM_SetBaseDrvStartType (szBaseDrvServiceName, dwRequiredStartType);
				if(bRetValue)
					bNeedReboot = true;
			}
			else
			{
				bRetValue = true;
			}
	    }
	    else
	    {
		    if (bEnable)
		    {
			    bool bRegistered = false;
			    CKAHUM_IsBaseDrvRegistered (szBaseDrvServiceName, szBaseDrvFileName, bRegistered);

			    if (!bRegistered)
				{
				    bRetValue = CKAHUM_RegisterBaseDrv (szBaseDrvServiceName, szBaseDrvFileName);
					if(bRetValue)
						bNeedReboot = true;
				}
			    else
				{
				    bRetValue = true;
				}
		    }
		    else
			{
			    bRetValue = CKAHUM_UnregisterBaseDrv (szBaseDrvServiceName);
				if(bRetValue)
					bNeedReboot = true;
			}
	    }
    }

	if (bRetValue)
		log.WriteFormat (_T("[ENABLEBD] Base driver '%ls' %s"), PEL_INFO, szBaseDrvServiceName, bEnable ? _T("enabled") : _T("disabled"));
	else
		log.WriteFormat (_T("[ENABLEBD] Failed to %s base driver '%ls'"), PEL_ERROR, bEnable ? _T("enable") : _T("disable"), szBaseDrvServiceName);

	if(pbNeedReboot)
		*pbNeedReboot = bNeedReboot;

	return bRetValue;
}

bool CKAHUM_PauseBaseDrv (LPCWSTR szBaseDrvServiceName)
{
	log.WriteFormat (_T("[PAUSEBD] Pausing base driver '%ls'"), PEL_INFO, szBaseDrvServiceName);

	bool bBaseDriverRegistered = false;
	CKAHUM_IsBaseDrvRegistered (szBaseDrvServiceName, bBaseDriverRegistered);

	if (!bBaseDriverRegistered)
		return false;

	HANDLE hBaseDrv = NULL;

	TCHAR szName [MAX_PATH + 5];
	wsprintf (szName, _T("\\\\.\\%s"), __LPCTSTR (szBaseDrvServiceName));

	if (g_bIsNT)
		hBaseDrv = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hBaseDrv = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

	if (hBaseDrv == INVALID_HANDLE_VALUE)
	{
		log.WriteFormat (_T("[PAUSEBD] Unable to get '%ls' base driver handle (0x%08x)"), PEL_ERROR, szBaseDrvServiceName, GetLastError ());
		return false;
	}

	DWORD dwBytesReturned = 0;
	bool bRetValue = DeviceIoControl (hBaseDrv, BASE_PAUSE_FILTER, NULL, 0, NULL, 0, &dwBytesReturned, NULL) == TRUE;

	if (!bRetValue)
		log.WriteFormat (_T("[PAUSEBD] DeviceIoControl failed (0x%08x)"), PEL_ERROR, GetLastError ());

	CloseHandle (hBaseDrv);

	log.WriteFormat (_T("[PAUSEBD] Pausing base driver '%ls' done"), PEL_INFO, szBaseDrvServiceName);

	return bRetValue;
}

bool CKAHUM_ResumeBaseDrv (LPCWSTR szBaseDrvServiceName)
{
	log.WriteFormat (_T("[RESUMEBD] Resuming base driver '%ls'"), PEL_INFO, szBaseDrvServiceName);

	bool bBaseDriverRegistered = false;
	CKAHUM_IsBaseDrvRegistered (szBaseDrvServiceName, bBaseDriverRegistered);

	if (!bBaseDriverRegistered)
		return false;

	HANDLE hBaseDrv = NULL;

	TCHAR szName [MAX_PATH + 5];
	wsprintf (szName, _T("\\\\.\\%s"), __LPCSTR (szBaseDrvServiceName));

	if (g_bIsNT)
		hBaseDrv = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hBaseDrv = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);

	if (hBaseDrv == INVALID_HANDLE_VALUE)
	{
		log.WriteFormat (_T("[RESUMEBD] Unable to get '%ls' base driver handle (0x%08x)"), PEL_ERROR, szBaseDrvServiceName, GetLastError ());
		return false;
	}

	DWORD dwBytesReturned = 0;
	bool bRetValue = DeviceIoControl (hBaseDrv, BASE_RESUME_FILTER, NULL, 0, NULL, 0, &dwBytesReturned, NULL) == TRUE;

	if (!bRetValue)
		log.WriteFormat (_T("[RESUMEBD] DeviceIoControl failed (0x%08x)"), PEL_ERROR, GetLastError ());

	CloseHandle (hBaseDrv);

	log.WriteFormat (_T("[RESUMEBD] Resuming base driver '%ls' done"), PEL_INFO, szBaseDrvServiceName);

	return bRetValue;
}
