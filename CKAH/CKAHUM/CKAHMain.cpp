#include "stdafx.h"
#include <io.h>
//#include <time.h>
#include "PluginUtils.h"
#include "Unpacker.h"
#include "Services.h"
#include "UnicodeConv.h"
#include "Utils.h"
#include "CKAHStorage.h"
#include "CKAHMain.h"

extern LPCKAHSTORAGE g_lpStorage;

static CCKAHRegStorageImpl g_RegStorageImpl (HKEY_LOCAL_MACHINE, L"SOFTWARE\\KasperskyLab\\Components\\10a");

extern int g_bUseKL1;

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Initialize (CKAHUM::EXTERNALLOGGERPROC logger /* = NULL */)
{
    return Initialize(HKEY_LOCAL_MACHINE, L"SOFTWARE\\KasperskyLab\\Components\\10a", logger);
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Initialize (HKEY hKey, LPCWSTR szSubKey, CKAHUM::EXTERNALLOGGERPROC logger /* = NULL */)
{
    Guard::Lock lock(g_guardStorage);

	log.WriteFormat (_T("[CKAHUM::Initialize] Initializing..."), PEL_INFO);

    g_externalLogger.AddLogger (logger);

    if (!g_lpStorage)
    {
        g_RegStorageImpl.SetPath (hKey, szSubKey);

	    g_lpStorage = &g_RegStorageImpl;
    }

    //
	WCHAR szValue[0x1000];
	if (g_lpStorage->GetParam (L"\\UseKL1", sizeof (szValue), szValue))
		g_bUseKL1 = _wtoi (szValue);		

    if (g_bUseKL1)
	{
	    log.WriteFormat (_T("[CKAHUM::Initialize] Using KL1"), PEL_INFO);
		IsBaseDriverRunning(L"kl1"); // to trace kl1 state
	}
    else
	    log.WriteFormat (_T("[CKAHUM::Initialize] Using SCM"), PEL_INFO);
    //
	
	log.WriteFormat (_T("[CKAHUM::Initialize] Initializing done"), PEL_INFO);

    InterlockedIncrement(&g_refcnt);

	return CKAHUM::srOK;
}

extern "C" CKAHUMMETHOD CKAHUM_Initialize (HKEY hKey, LPCWSTR szSubKey, CKAHUM::EXTERNALLOGGERPROC logger)
{
	return CKAHUM::Initialize(hKey, szSubKey, logger);
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Deinitialize (CKAHUM::EXTERNALLOGGERPROC logger /* = NULL */)
{
    int refcnt = InterlockedDecrement(&g_refcnt);

    if (!refcnt)
    {
        CKAHIDS::Stop();
        CKAHSTM::Stop();
        CKAHFW::Stop();
        CKAHCR::Stop();
    }

    g_externalLogger.RemoveLogger (logger);

	return CKAHUM::srOK;
}

extern "C" CKAHUMMETHOD CKAHUM_Deinitialize (CKAHUM::EXTERNALLOGGERPROC logger)
{
	return CKAHUM::Deinitialize(logger);
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Reload (LPCWSTR szwManifestFilePath)
{
	//static time_t last_reload_time = 0;
	//static CKAHUM::OpResult last_reload_result;

    Guard::Lock lock(g_guardStorage);

	// !!! this code added to avoid multiple reload from several clients on product start
	// it must be removed after Reload optimization will be done, see email
	// From: Mikhail Pavlyushik 
    // Sent: Tuesday, August 09, 2005 1:02 PM
    // To: Andrey Rubin; Timur Amirkhanov
    // Subject: CKAHUM otimizations
    /*
	if ((unsigned)(time(0) - last_reload_time) < 3)
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Reloading again 3 sec, reload skipped..."), PEL_INFO, szwManifestFilePath);
		return last_reload_result;
	}
    */

	log.WriteFormat (_T("[CKAHUM::Reload] Reloading from '%ls'..."), PEL_INFO, szwManifestFilePath);

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::Reload] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	if (szwManifestFilePath == NULL)
	{
		log.Write (_T("[CKAHUM::Reload] No manifest path"), PEL_ERROR);
		return srInvalidArg;
	}

	if (IOSGetFileAttributesW (szwManifestFilePath) == 0xffffffff)
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Manifest at '%ls' not found"), PEL_ERROR, szwManifestFilePath);
		return srManifestNotFound;
	}

	CKAHUM_AfterRebootCheck (g_lpStorage);

	CCKAHManifest manifest;

	if (!manifest.ReadFromFile (szwManifestFilePath))
	{
		log.WriteFormat (_T("[CKAHUM::Reload] Failed to read manifest from '%ls'"), PEL_ERROR, szwManifestFilePath);
		return srWrongManifestFormat;
	}

    /*
	last_reload_result = ReloadFromManifest (manifest, g_lpStorage);
	last_reload_time = time(0);
	return last_reload_result;
    */
    return ReloadFromManifest (manifest, g_lpStorage);
}

extern "C" CKAHUMMETHOD CKAHUM_Reload (LPCWSTR szManifestFilePath)
{
	return CKAHUM::Reload (szManifestFilePath);
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Uninstall ()
{
    Guard::Lock lock(g_guardStorage);

	log.WriteFormat (_T("[CKAHUM::Uninstall] Uninstalling..."), PEL_INFO);

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::Uninstall] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

    CKAHUM::OpResult res = RemoveBaseDrivers(g_lpStorage);

    g_lpStorage->Cleanup ();

    return res;
}

extern "C" CKAHUMMETHOD CKAHUM_Uninstall ()
{
	return CKAHUM::Uninstall ();
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Pause ()
{
    Guard::Lock lock(g_guardStorage);

	log.Write (_T("[CKAHUM::Pause] Pausing..."));

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::Pause] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	CCKAHManifest manifest;

	if (!manifest.ReadFromStorage (g_lpStorage, szwPersistentManifestRoot))
	{
		log.Write (_T("[CKAHUM::Pause] Failed to read manifest from storage"), PEL_ERROR);
		return CKAHUM::srErrorReadingPersistentManifest;
	}

	unsigned i;
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		std::wstring szwBaseDrvName = RemoveExtension (manifest.m_BaseDrvs[i]->m_sName);
		
		log.WriteFormat (_T("[CKAHUM::Pause] Pausing base driver '%ls'"), PEL_INFO, szwBaseDrvName.c_str ());

		CKAHUM_PauseBaseDrv (szwBaseDrvName.c_str ());
	}

	log.Write (_T("[CKAHUM::Pause] Pausing done"));

	return srOK;
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::Resume ()
{
    Guard::Lock lock(g_guardStorage);

	log.Write (_T("[CKAHUM::Resume] Resuming..."));

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::Resume] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	CCKAHManifest manifest;

	if (!manifest.ReadFromStorage (g_lpStorage, szwPersistentManifestRoot))
	{
		log.Write (_T("[CKAHUM::Resume] Failed to read manifest from storage"), PEL_ERROR);
		return CKAHUM::srErrorReadingPersistentManifest;
	}

	unsigned i;
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		std::wstring szwBaseDrvName = RemoveExtension (manifest.m_BaseDrvs[i]->m_sName);
		
		log.WriteFormat (_T("[CKAHUM::Resume] Resuming base driver '%ls'"), PEL_INFO, szwBaseDrvName.c_str ());

		CKAHUM_ResumeBaseDrv (szwBaseDrvName.c_str ());
	}

	log.Write (_T("[CKAHUM::Resume] Resuming done"));

	return srOK;
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::StopAll ()
{
	return StopAllPlugins ();
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::UnregisterBaseDrivers ()
{
    Guard::Lock lock(g_guardStorage);

	log.Write (_T("[CKAHUM::UnregisterBaseDrivers] Unregistering..."));

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::UnregisterBaseDrivers] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	CCKAHManifest manifest;

	if (!manifest.ReadFromStorage (g_lpStorage, szwPersistentManifestRoot))
	{
		log.Write (_T("[CKAHUM::UnregisterBaseDrivers] Failed to read manifest from storage"), PEL_ERROR);
		return CKAHUM::srErrorReadingPersistentManifest;
	}

	bool bNeedReboot = false;

	TCHAR szDriverInstallPath[MAX_PATH + 1 + 9];
	GetDriverInstallPath (szDriverInstallPath, sizeof (szDriverInstallPath));
	std::wstring sDriverInstallPath (__LPWSTR (szDriverInstallPath));

    const WCHAR *szwBaseDrvExtension = g_bIsNT ? (g_bUseKL1 ? L".dat" : L".sys") : L".vxd";

	unsigned i;
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		std::wstring szwBaseDrvName = RemoveExtension (manifest.m_BaseDrvs[i]->m_sName);
		
		log.WriteFormat (_T("[CKAHUM::UnregisterBaseDrivers] Unregistering base driver '%ls'"), PEL_INFO, szwBaseDrvName.c_str ());

		if (CKAHUM_UnregisterBaseDrv (szwBaseDrvName.c_str (), false))
		{
			std::wstring szwDrvPath = sDriverInstallPath + szwBaseDrvName + szwBaseDrvExtension;
			RemoveAttribsDeleteFileW (szwDrvPath.c_str ());
			bNeedReboot = true;
		}		
	}

	log.Write (_T("[CKAHUM::UnregisterBaseDrivers] Unregistering done"));

	return srOK;
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::EnableBaseDrivers (bool bEnable)
{
    Guard::Lock lock(g_guardStorage);

	log.WriteStr (_T("[CKAHUM::EnableBaseDrivers] %s base drivers..."), bEnable ? _T("Enabling") : _T("Disabling"));

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::EnableBaseDrivers] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	CCKAHManifest manifest;

	if (!manifest.ReadFromStorage (g_lpStorage, szwPersistentManifestRoot))
	{
		log.Write (_T("[CKAHUM::EnableBaseDrivers] Failed to read manifest from storage"), PEL_ERROR);
		return srErrorReadingPersistentManifest;
	}

	WCHAR szDisabledParamBuffer[2] = { 0 };
	if(g_lpStorage->GetParam(L"\\Disabled", sizeof(szDisabledParamBuffer), szDisabledParamBuffer) == sizeof(szDisabledParamBuffer))
	{
		bool bNowEnabled = _wtoi(szDisabledParamBuffer) == 0;
		if(bNowEnabled == bEnable)
		{
			log.WriteFormat (_T("[CKAHUM::EnableBaseDrivers] Nothing to do"), PEL_INFO);
			return srOK;
		}
	}

	TCHAR szDriverInstallPath[MAX_PATH + 1 + 9];
	GetDriverInstallPath (szDriverInstallPath, sizeof (szDriverInstallPath));
	std::wstring sDriverInstallPath (__LPWSTR (szDriverInstallPath));

    const WCHAR *szwBaseDrvExtension = g_bIsNT ? (g_bUseKL1 ? L".dat" : L".sys") : L".vxd";

	bool bSucceeded = true, bNeedReboot = false;

	unsigned i;
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		std::wstring szwBaseDrvName = RemoveExtension (manifest.m_BaseDrvs[i]->m_sName);
		std::wstring szwBaseDrvPath = sDriverInstallPath + szwBaseDrvName + szwBaseDrvExtension;
		
		log.WriteFormat (_T("[CKAHUM::EnableBaseDrivers] %s base driver '%ls'"), PEL_INFO, bEnable ? _T("Enabling") : _T("Disabling"), szwBaseDrvName.c_str ());

		if(bEnable)
		{
			// проверим регистрацию
			bool bWasRegistered = true;
			CKAHUM_IsBaseDrvRegistered (szwBaseDrvName.c_str (), szwBaseDrvPath.c_str (), bWasRegistered);
			if(!bWasRegistered)
			{
				if(CKAHUM_RegisterBaseDrv (szwBaseDrvName.c_str (), szwBaseDrvPath.c_str ()))
				{
					bNeedReboot = true;
				}
				else
				{
					bSucceeded = false;
					continue;
				}
			}
		}

		if(CKAHUM_EnableBaseDriver (szwBaseDrvName.c_str (), szwBaseDrvPath.c_str (), bEnable))
		{
			bNeedReboot = IsBaseDriverRunning(szwBaseDrvName) != bEnable;
		}
		else
		{
			bSucceeded = false;
		}
	}

	g_lpStorage->SetParam(L"\\Disabled", bEnable ? L"0" : L"1");

	log.WriteFormat (_T("[CKAHUM::EnableBaseDrivers] %s done%s"), PEL_INFO,
					bEnable ? _T("Enabling") : _T("Disabling"),
					bNeedReboot ? _T(" (need reboot)") : _T(""));

	return bNeedReboot ? srNeedReboot : (bSucceeded ? srOK : srOpFailed);
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::IsBaseDriversRunning (bool *pbRunning)
{
    Guard::Lock lock(g_guardStorage);

	log.Write (_T("[CKAHUM::CHECKBDRUN] Check whether base drivers are running..."));

	if (!pbRunning)
	{
		log.Write (_T("[CKAHUM::CHECKBDRUN] Invalid argument"), PEL_ERROR);
		return srInvalidArg;
	}

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::CHECKBDRUN] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	CCKAHManifest manifest;

	if (!manifest.ReadFromStorage (g_lpStorage, szwPersistentManifestRoot))
	{
		log.Write (_T("[CKAHUM::CHECKBDRUN] Failed to read manifest from storage"), PEL_ERROR);
		return CKAHUM::srErrorReadingPersistentManifest;
	}
	
	CKAHUM::OpResult RetResult = srOK;

	bool bOneNotRunning = false;

	unsigned i;
	for (i = 0; i < manifest.m_BaseDrvs.size (); ++i)
	{
		std::wstring szwBaseDrvName = RemoveExtension (manifest.m_BaseDrvs[i]->m_sName);

		log.WriteFormat (_T("[CKAHUM::CHECKBDRUN] Checking '%ls'"), PEL_INFO, szwBaseDrvName.c_str ());

		bool bIsOK = true;
		
		CKAHUM_IsBaseDrvRegistered (szwBaseDrvName.c_str (), bIsOK);

		if (!bIsOK || !IsBaseDriverRunning(szwBaseDrvName))
		{
			bOneNotRunning = true;
			break;
		}
	}

	*pbRunning = !bOneNotRunning;
	if (manifest.m_BaseDrvs.size () == 0)
	{
		RetResult = srNotStarted;
		log.WriteFormat (_T("[CKAHUM::CHECKBDRUN] List of base drivers is empty"), PEL_WARNING);
	}

	log.WriteFormat (_T("[CKAHUM::CHECKBDRUN] Checking done: drivers are %srunning"), PEL_INFO, *pbRunning ? _T("") : _T("not "));

	return RetResult;
}

CKAHUMEXPORT CKAHUM::OpResult CKAHUM::IsBaseDriversEnabled (bool *pbEnabled)
{
    Guard::Lock lock(g_guardStorage);

	log.Write (_T("[CKAHUM::CHECKBDEN] Check whether base drivers are enabled..."));

	if (!pbEnabled)
	{
		log.Write (_T("[CKAHUM::CHECKBDEN] Invalid argument"), PEL_ERROR);
		return srInvalidArg;
	}

	if (!g_lpStorage)
	{
		log.Write (_T("[CKAHUM::CHECKBDEN] No persistent manifest"), PEL_ERROR);
		return srNoPersistentManifest;
	}

	*pbEnabled = true;

	WCHAR szValue[0x1000];
	if (g_lpStorage->GetParam (L"\\Disabled", sizeof (szValue), szValue))
	{
		log.WriteFormat (_T("[CKAHUM::CHECKBDEN] 'Disabled' param is %ls"), PEL_INFO, szValue);
		*pbEnabled = _wtoi (szValue) == 0;
	}
	else
	{
		log.WriteFormat (_T("[CKAHUM::CHECKBDEN] 'Disabled' param is not present"), PEL_INFO);
	}

	log.WriteFormat (_T("[CKAHUM::CHECKBDEN] Checking done: drivers are %senabled"), PEL_INFO, *pbEnabled ? _T("") : _T("not "));

	return srOK;
}