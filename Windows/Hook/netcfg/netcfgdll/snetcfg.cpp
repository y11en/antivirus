//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997.
//
//  File:       S N E T C F G . C P P
//
//  Contents:   Sample code that demonstrates how to:
//              - find out if a component is installed
//              - install a net component
//              - install an OEM net component
//              - uninstall a net component
//              - enumerate net components
//              - enumerate net adapters using Setup API
//              - enumerate binding paths of a component
//
//  Notes:
//
//----------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop
#include "snetcfg.h"

typedef HANDLE          HCATADMIN;
typedef HANDLE          HCATINFO;

typedef BOOL (WINAPI *tpfCryptCATAdminAcquireContext) (
	OUT HCATADMIN *phCatAdmin, 
	IN const GUID *pgSubsystem, 
	IN DWORD dwFlags
	);

typedef BOOL (WINAPI *tpfCryptCATAdminReleaseContext) (
	IN HCATADMIN hCatAdmin,
	IN DWORD dwFlags
	);

typedef HCATINFO (WINAPI *tpfCryptCATAdminAddCatalog)(
	HCATADMIN hCatAdmin,
	WCHAR* pwszCatalogFile,
	WCHAR* pwszSelectBaseName,
	DWORD dwFlags
	);

typedef BOOL (WINAPI *tpfCryptCATAdminReleaseCatalogContext)(
	HCATADMIN hCatAdmin,
	HCATINFO hCatInfo,
	DWORD dwFlags
	);

typedef BOOL (WINAPI *tpfCryptCATAdminRemoveCatalog)(
	HCATADMIN hCatAdmin,
	WCHAR* pwszCatalogFile,
	DWORD dwFlags
	);


void reg_cat(LPWSTR szwCatPath, LPWSTR szwCatName, bool bAdd)
{
	HMODULE hWinTrust = LoadLibrary( L"Wintrust.dll");
	if (!hWinTrust)
	{
		return;
	}

	tpfCryptCATAdminAcquireContext pfCryptCATAdminAcquireContext = (tpfCryptCATAdminAcquireContext) GetProcAddress (
		hWinTrust, "CryptCATAdminAcquireContext" );

	tpfCryptCATAdminReleaseContext pfCryptCATAdminReleaseContext = (tpfCryptCATAdminReleaseContext) GetProcAddress (
		hWinTrust, "CryptCATAdminReleaseContext" );

	tpfCryptCATAdminAddCatalog pfCryptCATAdminAddCatalog = (tpfCryptCATAdminAddCatalog) GetProcAddress (
		hWinTrust, "CryptCATAdminAddCatalog" );

	tpfCryptCATAdminReleaseCatalogContext fCryptCATAdminReleaseCatalogContext = (tpfCryptCATAdminReleaseCatalogContext) GetProcAddress (
		hWinTrust, "CryptCATAdminReleaseCatalogContext" );

	tpfCryptCATAdminRemoveCatalog pfCryptCATAdminRemoveCatalog = (tpfCryptCATAdminRemoveCatalog) GetProcAddress (
		hWinTrust, "CryptCATAdminRemoveCatalog" );


	if (pfCryptCATAdminAcquireContext
		&& pfCryptCATAdminReleaseContext
		&& pfCryptCATAdminAddCatalog
		&& fCryptCATAdminReleaseCatalogContext
		&& pfCryptCATAdminRemoveCatalog
		)
	{
		HCATADMIN hCatAdmin = NULL;
		if (pfCryptCATAdminAcquireContext( &hCatAdmin, NULL, 0 ))
		{

			HCATINFO hInfo = NULL;
			if (bAdd)
			{
				hInfo = pfCryptCATAdminAddCatalog( hCatAdmin, szwCatPath, szwCatName, 0 );
				if (hInfo)
				{
					fCryptCATAdminReleaseCatalogContext( hCatAdmin, hInfo, 0 );
				}
			}
			else
			{
				BOOL bResult = pfCryptCATAdminRemoveCatalog( hCatAdmin, szwCatName, 0 );
			}

			pfCryptCATAdminReleaseContext( hCatAdmin, 0 );
		}
	}

	FreeLibrary( hWinTrust );

}


void DeleteService(LPCWSTR szServiceName)
{
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager)
    {
	    SC_HANDLE schService = ::OpenService(schSCManager, szServiceName, SERVICE_ALL_ACCESS);
	    if (schService)
	    {
		    ::DeleteService(schService);
        }
    }
}


//----------------------------------------------------------------------------
// Globals
//
static const GUID* c_aguidClass[] =
{
    &GUID_DEVCLASS_NET,
    &GUID_DEVCLASS_NETTRANS,
    &GUID_DEVCLASS_NETSERVICE,
    &GUID_DEVCLASS_NETCLIENT
};

//----------------------------------------------------------------------------
// Prototypes of helper functions
//
HRESULT HrInstallNetComponent(IN INetCfg* pnc, IN PCWSTR szComponentId,
                              IN const GUID* pguidClass);
HRESULT HrUninstallNetComponent(IN INetCfg* pnc, IN PCWSTR szComponentId);
HRESULT HrGetINetCfg(IN BOOL fGetWriteLock, INetCfg** ppnc);
HRESULT HrReleaseINetCfg(BOOL fHasWriteLock, INetCfg* pnc);
void ShowMessage(IN PCWSTR szMsg, ...);
void ShowHrMessage(IN HRESULT hr);
inline ULONG ReleaseObj(IUnknown* punk)
{
    return (punk) ? punk->Release () : 0;
}


//+---------------------------------------------------------------------------
//
// Function:  HrIsComponentInstalled
//
// Purpose:   Find out if a component is installed
//
// Arguments:
//    szComponentId [in]  id of component to search
//
// Returns:   S_OK    if installed,
//            S_FALSE if not installed,
//            otherwise an error code
//
// Author:    kumarp 11-February-99
//
// Notes:
//
HRESULT HrIsComponentInstalled(IN PCWSTR szComponentId)
{
    HRESULT hr=S_OK;
    INetCfg* pnc;
    INetCfgComponent* pncc;

    hr = HrGetINetCfg(FALSE, &pnc);
    if (S_OK == hr)
    {
        hr = pnc->FindComponent(szComponentId, &pncc);
        if (S_OK == hr)
        {
            ReleaseObj(pncc);
        }
        (void) HrReleaseINetCfg(FALSE, pnc);
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrInstallNetComponent
//
// Purpose:   Install the specified net component
//
// Arguments:
//    szComponentId [in]  component to install
//    nc            [in]  class of the component
//    szInfFullPath [in]  full path to primary INF file
//                        required if the primary INF and other
//                        associated files are not pre-copied to
//                        the right destination dirs.
//                        Not required when installing MS components
//                        since the files are pre-copied by
//                        Windows NT Setup.
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT NETCFGEXPORT HrInstallNetComponent(IN PCWSTR szComponentId,
                                           IN enum NetClass nc,
                                           IN PCWSTR szInfFullPath,
										   IN PWSTR szCatFullPath,
										   IN PWSTR szCatName, 
										   IN PCWSTR szwUniqueID)
{
    HRESULT hr=S_OK;
    INetCfg* pnc;

    if (szCatFullPath && szCatFullPath[0] && 
        szCatName && szCatName[0])
    {
	    reg_cat(szCatFullPath, szCatName, true);
    }


    if (szInfFullPath && lstrlen(szInfFullPath))
    {

		//MessageBoxW(NULL, szInfFullPath, L"netcfg", MB_OK);
		WCHAR szInfNameAfterCopy[MAX_PATH+1];
		szInfNameAfterCopy[0]=L'\0';
		WCHAR* szwOemName = NULL;
        if (SetupCopyOEMInfW(
                szInfFullPath,
                NULL,               // other files are in the
                                    // same dir. as primary INF
                SPOST_PATH,         // first param. contains path to INF
                0,                  // default copy style
                szInfNameAfterCopy, // receives the name of the INF
                                    // after it is copied to %windir%\inf
                MAX_PATH,           // max buf. size for the above
                NULL,               // receives required size if non-null
                &szwOemName))              // optionally retrieves filename
                                    // component of szInfNameAfterCopy
        {
            ShowMessage(L"...%s was copied to %s",
                        szInfFullPath,
                        szInfNameAfterCopy);

			DWORD dwDisposition = 0;
			HKEY hKey = NULL;
			if (szwUniqueID && lstrlenW(szwUniqueID) && szwOemName && lstrlenW(szwOemName) &&
				::RegCreateKeyExW (HKEY_CLASSES_ROOT, szwUniqueID, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
			{
				RegSetValueExW(hKey, szwUniqueID, 0, REG_SZ, (LPBYTE)szwOemName, (lstrlenW(szwOemName)+1)*sizeof(WCHAR));
				RegCloseKey( hKey );
			}

			//MessageBoxA(NULL, "HrInstallNetComponent copy inf - ok", "netcfg", MB_OK);
        }
        else
        {
            char str[256];
            wsprintfA(str, "HrInstallNetComponent copy inf failed: %d", GetLastError());
            //MessageBoxA(NULL, str, "netcfg", MB_OK);
            DWORD dwError = GetLastError();
            hr = HRESULT_FROM_WIN32(dwError);
        }
//			MessageBoxW(NULL, szInfNameAfterCopy, L"netcfg", MB_OK);
    }

    // cannot install net adapters this way. they have to be
    // enumerated/detected and installed by PnP
//	MessageBoxA(NULL, "HrInstallNetComponent begin", "netcfg", MB_OK);
    if (((nc == NC_NetProtocol) ||
         (nc == NC_NetService) ||
         (nc == NC_NetClient)) &&
        szComponentId &&
        szComponentId[0])
    {
        ShowMessage(L"Trying to install '%s'...", szComponentId);

        // if full path to INF has been specified, the INF
        // needs to be copied using Setup API to ensure that any other files
        // that the primary INF copies will be correctly found by Setup API
        //

//			MessageBoxA(NULL, "HrInstallNetComponent try get INetCfg...", "netcfg", MB_OK);
        // get INetCfg interface
        hr = HrGetINetCfg(TRUE, &pnc);

        if (SUCCEEDED(hr))
        {
//			MessageBoxA(NULL, "HrInstallNetComponent try get INetCfg - ok", "netcfg", MB_OK);
            // install szComponentId
            hr = HrInstallNetComponent(pnc, szComponentId,
                                       c_aguidClass[nc]);
            if (SUCCEEDED(hr))
            {
                // Apply the changes
                hr = pnc->Apply();
            }

            // release INetCfg
            (void) HrReleaseINetCfg(TRUE, pnc);
        }
        // show success/failure message
        ShowHrMessage(hr);
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrInstallNetComponent
//
// Purpose:   Install the specified net component
//
// Arguments:
//    pnc           [in]  pointer to INetCfg object
//    szComponentId [in]  component to install
//    pguidClass    [in]  class guid of the component
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT HrInstallNetComponent(IN INetCfg* pnc,
                              IN PCWSTR szComponentId,
                              IN const GUID* pguidClass)
{
    HRESULT hr=S_OK;
	OBO_TOKEN OboToken = {OBO_USER, NULL, NULL, NULL, NULL, false};
    INetCfgClassSetup* pncClassSetup;
    INetCfgComponent* pncc;

    // OBO_TOKEN specifies the entity on whose behalf this
    // component is being installed

    // set it to OBO_USER so that szComponentId will be installed
    // On-Behalf-Of "user"
//    MessageBoxA(NULL, "HrInstallNetComponent body", "netcfg", MB_OK);
    hr = pnc->QueryNetCfgClass (pguidClass, IID_INetCfgClassSetup,
                                (void**)&pncClassSetup);
    if (SUCCEEDED(hr))
    {
//		MessageBoxA(NULL, "QueryNetCfgClass success", "netcfg", MB_OK);
        hr = pncClassSetup->Install(szComponentId,
                                    &OboToken,
                                    NSF_POSTSYSINSTALL,
                                    0,       // <upgrade-from-build-num>
                                    NULL,    // answerfile name
                                    NULL,    // answerfile section name
                                    &pncc);
        if (S_OK == hr)
        {
//			MessageBoxA(NULL, "Install success", "netcfg", MB_OK);
            // we dont want to use pncc (INetCfgComponent), release it
            ReleaseObj(pncc);
        }
	else
	{
//			MessageBoxA(NULL, "Install failure", "netcfg", MB_OK);
	}

        ReleaseObj(pncClassSetup);
    }

    return hr;
}


static bool RemoveInfByAPI(const WCHAR* szInfNameAfterCopy)
{
	bool bRet = false;
	HMODULE hMod = LoadLibraryW(L"setupapi.dll");
	if (hMod)
	{
		BOOL (WINAPI *fnSetupUninstallOEMInfW)(
			PCWSTR InfFileName,
			DWORD Flags,
			PVOID Reserved
			);
		*(void**)&fnSetupUninstallOEMInfW = GetProcAddress(hMod, "SetupUninstallOEMInfW");

		if (fnSetupUninstallOEMInfW)
		{
			fnSetupUninstallOEMInfW(szInfNameAfterCopy, 1, NULL);
			bRet = true;
		}

		FreeLibrary(hMod);
		hMod = NULL;
	}

	return bRet;
}
//+---------------------------------------------------------------------------
//
// Function:  HrUninstallNetComponent
//
// Purpose:   Initialize INetCfg and uninstall a component
//
// Arguments:
//    szComponentId [in]  InfId of component to uninstall (e.g. MS_TCPIP)
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT NETCFGEXPORT HrUninstallNetComponent(IN PCWSTR szComponentId, 
											 IN PWSTR szCatFullPath,
											 IN PWSTR szCatName,
											 IN PCWSTR szwUniqueID,
                                             IN PCWSTR szServiceName)
{
    HRESULT hr=S_OK;
    INetCfg* pnc;

    ShowMessage(L"Trying to uninstall '%s'...", szComponentId);

    if (szComponentId && szComponentId[0])
    {
        // get INetCfg interface
        hr = HrGetINetCfg(TRUE, &pnc);

        if (SUCCEEDED(hr))
        {
            // uninstall szComponentId
            hr = HrUninstallNetComponent(pnc, szComponentId);

            if (S_OK == hr)
            {
                // Apply the changes
                hr = pnc->Apply();
            }
            else if (S_FALSE == hr)
            {
                ShowMessage(L"...'%s' is not installed");
            }
            // release INetCfg
            (void) HrReleaseINetCfg(TRUE, pnc);
        }
    }

    if (szwUniqueID && szwUniqueID[0]) 
    {
		HKEY hKey = NULL;
		if (::RegOpenKeyExW (HKEY_CLASSES_ROOT, szwUniqueID, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			WCHAR szInfNameAfterCopy[MAX_PATH+1];
			szInfNameAfterCopy[0]=L'\0';
			DWORD dwType, dwSize=sizeof(szInfNameAfterCopy);

			if (::RegQueryValueExW (hKey, szwUniqueID, NULL, &dwType, (LPBYTE)szInfNameAfterCopy, &dwSize) == ERROR_SUCCESS)
			{
				if (!RemoveInfByAPI(szInfNameAfterCopy))
				{	//manual remove if API unreachable (NT 4.0, 2K)
					
					//break extension
					for(int i = 0; szInfNameAfterCopy[i] != L'\0'; ++i)
					{
						if(szInfNameAfterCopy[i] == L'.')
						{
							szInfNameAfterCopy[i] = L'\0';
							break;
						}
					}
					
					WCHAR szInfFolder[MAX_PATH+1];
					ExpandEnvironmentStrings(L"%Windir%\\Inf", szInfFolder, sizeof(szInfFolder)/sizeof(szInfFolder[0]));

					WCHAR szDeletePath[MAX_PATH+1];
					wsprintfW(szDeletePath, L"%s\\%s.pnf", szInfFolder, szInfNameAfterCopy);
					DeleteFile(szDeletePath);

					wsprintfW(szDeletePath, L"%s\\%s.inf", szInfFolder, szInfNameAfterCopy);
					DeleteFile(szDeletePath);
				}
			}

			::RegCloseKey( hKey );
		}
		
		::RegDeleteKeyW(HKEY_CLASSES_ROOT, szwUniqueID);
    }

    if (szCatFullPath && szCatFullPath[0] &&
        szCatName && szCatName[0])
    {
	    reg_cat(szCatFullPath, szCatName, false);
    }

    if (szServiceName && szServiceName[0])
    {
        DeleteService(szServiceName);
    }

    // show success/failure message
    ShowHrMessage(hr);

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrUninstallNetComponent
//
// Purpose:   Uninstall the specified component.
//
// Arguments:
//    pnc           [in]  pointer to INetCfg object
//    szComponentId [in]  component to uninstall
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT HrUninstallNetComponent(IN INetCfg* pnc, IN PCWSTR szComponentId)
{
    HRESULT hr=S_OK;
    OBO_TOKEN OboToken = {OBO_USER, NULL, NULL, NULL, NULL, false};
    INetCfgComponent* pncc;
    GUID guidClass;
    INetCfgClass* pncClass;
    INetCfgClassSetup* pncClassSetup;

    // OBO_TOKEN specifies the entity on whose behalf this
    // component is being uninstalld

    // set it to OBO_USER so that szComponentId will be uninstalld
    // On-Behalf-Of "user"
 
    // see if the component is really installed
    hr = pnc->FindComponent(szComponentId, &pncc);

    if (S_OK == hr)
    {
        // yes, it is installed. obtain INetCfgClassSetup and DeInstall

        hr = pncc->GetClassGuid(&guidClass);

        if (S_OK == hr)
        {
            hr = pnc->QueryNetCfgClass(&guidClass, IID_INetCfgClass,
                                       (void**)&pncClass);
            if (SUCCEEDED(hr))
            {
                hr = pncClass->QueryInterface(IID_INetCfgClassSetup,
                                              (void**)&pncClassSetup);
                    if (SUCCEEDED(hr))
                    {
                        hr = pncClassSetup->DeInstall (pncc, &OboToken, NULL);

                        ReleaseObj (pncClassSetup);
                    }
                ReleaseObj(pncClass);
            }
        }
        ReleaseObj(pncc);
    }

    return hr;
}




//+---------------------------------------------------------------------------
//
// Function:  HrGetNextBindingInterface
//
// Purpose:   Enumerate over binding interfaces that constitute
//            the given binding path
//
// Arguments:
//    pncbp  [in]  pointer to INetCfgBindingPath object
//    ppncbi [out] pointer to pointer to INetCfgBindingInterface object
//
// Returns:   S_OK on success, otherwise an error code
//
// Notes:
//
/*
HRESULT HrGetNextBindingInterface(IN  INetCfgBindingPath* pncbp,
                                  OUT INetCfgBindingInterface** ppncbi)
{
    HRESULT hr=S_OK;
    INetCfgBindingInterface* pncbi=NULL;

    static IEnumNetCfgBindingInterface* pencbi=NULL;

    *ppncbi = NULL;

    // if this is the first call in the enumeration, obtain
    // the IEnumNetCfgBindingInterface interface
    //
    if (!pencbi)
    {
        hr = pncbp->EnumBindingInterfaces(&pencbi);
    }

    if (S_OK == hr)
    {
        ULONG celtFetched;

        // get next binding interface
        hr = pencbi->Next(1, &pncbi, &celtFetched);
    }

    // on the last call (hr == S_FALSE) or on error, release resources

    if (S_OK == hr)
    {
        *ppncbi = pncbi;
    }
    else
    {
        ReleaseObj(pencbi);
        pencbi = NULL;
    }

    return hr;
}
*/
//+---------------------------------------------------------------------------
//
// Function:  HrGetNextBindingPath
//
// Purpose:   Enumerate over binding paths that start with
//            the specified component
//
// Arguments:
//    pncc              [in]  pointer to INetCfgComponent object
//    dwBindingPathType [in]  type of binding path to retrieve
//    ppncbp            [out] pointer to INetCfgBindingPath interface
//
// Returns:   S_OK on success, otherwise an error code
//
// Notes:
//
/*
HRESULT HrGetNextBindingPath(IN  INetCfgComponent* pncc,
                             IN  DWORD  dwBindingPathType,
                             OUT INetCfgBindingPath** ppncbp)
{
    HRESULT hr=S_OK;
    INetCfgBindingPath* pncbp=NULL;

    static IEnumNetCfgBindingPath* pebp=NULL;

    *ppncbp = NULL;

    // if this is the first call in the enumeration, obtain
    // the IEnumNetCfgBindingPath interface
    if (!pebp)
    {
        INetCfgComponentBindings* pnccb=NULL;

        hr = pncc->QueryInterface(IID_INetCfgComponentBindings,
                                  (void**) &pnccb);
        if (S_OK == hr)
        {
            hr = pnccb->EnumBindingPaths(dwBindingPathType, &pebp);
            ReleaseObj(pnccb);
        }
    }

    if (S_OK == hr)
    {
        ULONG celtFetched;

        // get next binding path
        hr = pebp->Next(1, &pncbp, &celtFetched);
    }

    // on the last call (hr == S_FALSE) or on error, release resources

    if (S_OK == hr)
    {
        *ppncbp = pncbp;
    }
    else
    {
        ReleaseObj(pebp);
        pebp = NULL;
    }

    return hr;
}
*/


//+---------------------------------------------------------------------------
//
// Function:  HrGetINetCfg
//
// Purpose:   Initialize COM, create and initialize INetCfg.
//            Obtain write lock if indicated.
//
// Arguments:
//    fGetWriteLock [in]  whether to get write lock
//    ppnc          [in]  pointer to pointer to INetCfg object
//
// Returns:   S_OK on success, otherwise an error code
//
// Notes:
//
HRESULT HrGetINetCfg(IN BOOL fGetWriteLock,
                     INetCfg** ppnc)
{
    HRESULT hr=S_OK;

    // Initialize the output parameters.
    *ppnc = NULL;

    // initialize COM
    hr = CoInitializeEx(NULL,
                        COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED );

    if (SUCCEEDED(hr))
    {
        // Create the object implementing INetCfg.
        //
        INetCfg* pnc;
        hr = CoCreateInstance(CLSID_CNetCfg, NULL, CLSCTX_INPROC_SERVER,
                              IID_INetCfg, (void**)&pnc);
        if (SUCCEEDED(hr))
        {
            INetCfgLock * pncLock = NULL;
            if (fGetWriteLock)
            {
                // Get the locking interface
                hr = pnc->QueryInterface(IID_INetCfgLock,
                                         (LPVOID *)&pncLock);
                if (SUCCEEDED(hr))
                {
                    // Attempt to lock the INetCfg for read/write
                    static const ULONG c_cmsTimeout = 15000;
                    static const WCHAR c_szSampleNetcfgApp[] =
                        L"Sample Netcfg Application (netcfg.exe)";
                    PWSTR szLockedBy;

                    hr = pncLock->AcquireWriteLock(c_cmsTimeout,
                                                   c_szSampleNetcfgApp,
                                                   &szLockedBy);
                    if (S_FALSE == hr)
                    {
                        hr = NETCFG_E_NO_WRITE_LOCK;
                    }
                }
            }

            if (SUCCEEDED(hr))
            {
                // Initialize the INetCfg object.
                //
                hr = pnc->Initialize(NULL);
                if (SUCCEEDED(hr))
                {
                    *ppnc = pnc;
                    pnc->AddRef();
                }
                else
                {
                    // initialize failed, if obtained lock, release it
                    if (pncLock)
                    {
                        pncLock->ReleaseWriteLock();
                    }
                }
            }
            ReleaseObj(pncLock);
            ReleaseObj(pnc);
        }

        if (FAILED(hr))
        {
            CoUninitialize();
        }
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrReleaseINetCfg
//
// Purpose:   Uninitialize INetCfg, release write lock (if present)
//            and uninitialize COM.
//
// Arguments:
//    fHasWriteLock [in]  whether write lock needs to be released.
//    pnc           [in]  pointer to INetCfg object
//
// Returns:   S_OK on success, otherwise an error code
//
// Notes:
//
HRESULT HrReleaseINetCfg(BOOL fHasWriteLock, INetCfg* pnc)
{
    HRESULT hr = S_OK;

    // uninitialize INetCfg
    hr = pnc->Uninitialize();

    // if write lock is present, unlock it
    if (SUCCEEDED(hr) && fHasWriteLock)
    {
        INetCfgLock* pncLock;

        // Get the locking interface
        hr = pnc->QueryInterface(IID_INetCfgLock,
                                 (LPVOID *)&pncLock);
        if (SUCCEEDED(hr))
        {
            hr = pncLock->ReleaseWriteLock();
            ReleaseObj(pncLock);
        }
    }

    ReleaseObj(pnc);

    CoUninitialize();

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  ShowMessage
//
// Purpose:   Helper function to display a message in verbose mode.
//            If not in verbose mode, do nothing.
//
// Arguments:
//    szMsg [in]  message to display
//
// Returns:   None
//
// Notes:
//
void ShowMessage(IN PCWSTR szMsg, ...)
{
}

//+---------------------------------------------------------------------------
//
// Function:  ShowHrMessage
//
// Purpose:   Helper function to display the status of the last action
//            as indicated by the given HRESULT
//
// Arguments:
//    hr [in]  status code
//
// Returns:   None
//
// Notes:
//
void ShowHrMessage(IN HRESULT hr)
{
    if (SUCCEEDED(hr))
    {
        ShowMessage(L"...done");
        if (NETCFG_S_REBOOT == hr)
        {
            ShowMessage(L"*** You need to reboot your computer for this change to take effect ***");
        }
    }
    else
    {
        ShowMessage(L"..failed. Error code: 0x%lx", hr);
    }
}

