#include <service/avpsrv.cpp>
#include "UnicodeConv.h"

	HSRV
SRV_InitEx (
	const TCHAR *	pszMachineName,
	const TCHAR *	pszServiceName,
	DWORD dwSCMAccess,
	DWORD dwServiceAccess
)
{
	if(	pszServiceName == NULL ||
		*pszServiceName == 0 ||
		lstrlen(pszServiceName) >= _MAX_PATH)
		return NULL;

	PHSERVICE hSrv = (PHSERVICE)new BYTE[sizeof(HSERVICE) + SRV_CONFIG_SIZE];
	if(hSrv == NULL)
		return NULL;

	lstrcpy(hSrv->pszServiceName, pszServiceName);
	hSrv->pServiceConfig = (LPQUERY_SERVICE_CONFIG)(hSrv + 1);

	hSrv->schSCManager = OpenSCManager(pszMachineName, NULL, dwSCMAccess);
    if(hSrv->schSCManager == NULL) {	
		DWORD dwError = GetLastError();
		delete[] (BYTE *)hSrv;
		SetLastError(dwError);
		return NULL;
	}

	hSrv->schService = OpenService(hSrv->schSCManager, hSrv->pszServiceName, dwServiceAccess);
	if(hSrv->schService == NULL) {
		DWORD dwError = GetLastError();
		SetLastError(dwError);
		if(dwError == ERROR_SERVICE_DOES_NOT_EXIST)
			return hSrv;
		CloseServiceHandle(hSrv->schSCManager);
		delete[] (BYTE *)hSrv;
		SetLastError(dwError);
		return NULL;
	}

	return hSrv;
}

	bool
SRV_InstallExW (
	HSRV	hSrv,
	WCHAR *	pszDisplayName,
	DWORD	dwServiceType,
	DWORD	dwStartType,
	WCHAR *	pszDependencies,
	WCHAR *	pszPathName,
	WCHAR *	pszAccountName,
	WCHAR *	pszPassword
)
{
    PHSERVICE h = (PHSERVICE)hSrv;
	if(h == NULL)
		return false;

	if(	pszDependencies && *pszDependencies == 0)
		pszDependencies = NULL;

	if(	pszAccountName && *pszAccountName == 0)
		pszAccountName = NULL;

	if(	pszPassword && *pszPassword == 0)
		pszPassword = NULL;

	h->schService = CreateServiceW (h->schSCManager,
								  __LPWSTR (h->pszServiceName),
								  pszDisplayName,
								  SERVICE_ALL_ACCESS,
								  dwServiceType,
								  dwStartType,
								  SERVICE_ERROR_NORMAL,
								  pszPathName,
								  NULL,
								  NULL,
								  pszDependencies,
								  pszAccountName,
								  pszPassword);
	if(h->schService == NULL)
		return false;

	return true;
}

	bool
SRV_RemoveNoStop(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;

	if(	h == NULL ||
		h->schService == NULL)
		return false;

	if(	!DeleteService(h->schService) ||
		!CloseServiceHandle(h->schService))
		return false;
	
	h->schService = NULL;

	return true;
}
