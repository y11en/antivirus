// avpsrv.cpp ////////////////////////////////////

// Service Control Manager
// [Version 1.18.000]
// Copyright (C) 1999-2001 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <winsvc.h>
#include "avpsrv.h"

//////////////////////////////////////////////////

#define SRV_CONFIG_SIZE		4096

#define SRV_STOP_TRY_COUNT	5
#define SRV_STOP_SLEEP_TIME	1000 // msec

//////////////////////////////////////////////////

typedef struct _HSERVICE {// Handle Of Service

	TCHAR					pszServiceName[_MAX_PATH];
	SC_HANDLE				schSCManager;
	SC_HANDLE				schService;
	LPQUERY_SERVICE_CONFIG	pServiceConfig;

} HSERVICE, * PHSERVICE;

//////////////////////////////////////////////////

#define	SRV_COUNT_STARTUP_TYPE	5

DWORD SRV_g_dwSrvStartupType[5] = {
	SRV_STARTUP_TYPE_BOOT_START,
	SRV_STARTUP_TYPE_SYSTEM_START,
	SRV_STARTUP_TYPE_AUTO_START,
	SRV_STARTUP_TYPE_DEMAND_START,
	SRV_STARTUP_TYPE_DISABLED,
};

//////////////////////////////////////////////////

	HSRV
SRV_Init(
	const TCHAR *	pszMachineName,
	const TCHAR *	pszServiceName
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

	hSrv->schSCManager = OpenSCManager(pszMachineName, NULL, SC_MANAGER_ALL_ACCESS);
    if(hSrv->schSCManager == NULL) {	
		delete[] (BYTE *)hSrv;
		return NULL;
	}

	hSrv->schService = OpenService(hSrv->schSCManager, hSrv->pszServiceName, SERVICE_ALL_ACCESS);
	if(hSrv->schService == NULL) {
		DWORD dwError = GetLastError();
		SetLastError(dwError);
		if(dwError == ERROR_SERVICE_DOES_NOT_EXIST)
			return hSrv;

		CloseServiceHandle(hSrv->schSCManager);
		delete[] (BYTE *)hSrv;
		return NULL;
	}

	return hSrv;
}

//////////////////////////////////////////////////

	void
SRV_Done(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;
	if(h == NULL)
		return;

	if(h->schService)
		CloseServiceHandle(h->schService);
	CloseServiceHandle(h->schSCManager);
	delete[] (BYTE *)h;
}

//////////////////////////////////////////////////

	bool
SRV_IsInstalled(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;
	if(	h == NULL ||
		h->schService == NULL)
		return false;

	return true;
}

//////////////////////////////////////////////////

	bool
SRV_Install(
	HSRV	hSrv,
	TCHAR *	pszDisplayName,
	TCHAR *	pszPathName,
	TCHAR *	pszAccountName,
	TCHAR *	pszPassword
)
{
    PHSERVICE h = (PHSERVICE)hSrv;
	if(h == NULL)
		return false;

	if(	pszAccountName &&
		*pszAccountName == 0)
		pszAccountName = NULL;
	if(	pszPassword &&
		*pszPassword == 0)
		pszPassword = NULL;

	h->schService = CreateService(h->schSCManager,
								  h->pszServiceName,
								  pszDisplayName,
								  SERVICE_ALL_ACCESS,
								  SERVICE_WIN32_OWN_PROCESS,
								  SERVICE_DEMAND_START,
								  SERVICE_ERROR_NORMAL,
								  pszPathName,
								  NULL,
								  NULL,
								  NULL,
								  pszAccountName,
								  pszPassword);
	if(h->schService == NULL)
		return false;

	return true;
}

//////////////////////////////////////////////////

	bool
SRV_InstallEx(
	HSRV	hSrv,
	TCHAR *	pszDisplayName,
	DWORD	dwServiceType,
	DWORD	dwStartType,
	TCHAR *	pszDependencies,
	TCHAR *	pszPathName,
	TCHAR *	pszAccountName,
	TCHAR *	pszPassword
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

	h->schService = CreateService(h->schSCManager,
								  h->pszServiceName,
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

//////////////////////////////////////////////////

	bool
SRV_Remove(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;

	if(	h == NULL ||
		h->schService == NULL)
		return false;

	SRV_Stop(hSrv);
	for(DWORD i = 0; i < SRV_STOP_TRY_COUNT; i++) {
		Sleep(SRV_STOP_SLEEP_TIME);
		if(SRV_GetCurrentState(h) == SERVICE_STOPPED) {
			if(	!DeleteService(h->schService) ||
				!CloseServiceHandle(h->schService))
				return false;
			h->schService = NULL;
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////

	DWORD
SRV_GetCurrentState(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;
	if(h == NULL)
		return SRV_RC_ERROR;

	SERVICE_STATUS ssServiceStatus;
	if(	h->schService == NULL ||
		!QueryServiceStatus(h->schService, &ssServiceStatus))
		return SRV_RC_ERROR;

	return ssServiceStatus.dwCurrentState;
}

//////////////////////////////////////////////////

	bool
SRV_Start(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;
	if(	h == NULL ||
		h->schService == NULL)
		return false;

	DWORD dwState = SRV_GetCurrentState(hSrv);
	if(	dwState == SRV_STATE_START_PENDING ||
		dwState == SRV_STATE_RUNNING)
		return true;
	if(!StartService(h->schService, 0, NULL))
		return false;

	return true;
}

//////////////////////////////////////////////////

	void
SRV_Stop(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;
	if(h == NULL)
		return;

	DWORD dwState = SRV_GetCurrentState(hSrv);
	if(	dwState == SRV_STATE_STOP_PENDING ||
		dwState == SRV_STATE_STOPPED)
		return;
	SERVICE_STATUS ssServiceStatus;
	ControlService(h->schService, SERVICE_CONTROL_STOP, &ssServiceStatus);
}

//////////////////////////////////////////////////

	DWORD
SRV_GetStartupType(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;

	DWORD dwBufSize;
	if(	h == NULL ||
		h->schService == NULL ||
		h->pServiceConfig == NULL ||
		!QueryServiceConfig(h->schService, h->pServiceConfig, SRV_CONFIG_SIZE, &dwBufSize))
		return SRV_RC_ERROR;

	return h->pServiceConfig->dwStartType;
}

//////////////////////////////////////////////////

	bool
SRV_SetStartupType(
	HSRV	hSrv,
	DWORD	dwServiceStartupType
)
{
	DWORD i;
	for( i = 0; i < SRV_COUNT_STARTUP_TYPE; i++ )
		if( dwServiceStartupType == SRV_g_dwSrvStartupType[i] )
			break;
	if( i == SRV_COUNT_STARTUP_TYPE )
		return false;

	PHSERVICE h = (PHSERVICE)hSrv;

	DWORD dwBufSize;
	if(	h					== NULL				||
		h->schService		== NULL				||
		h->pServiceConfig	== NULL				||
		!QueryServiceConfig(h->schService,
							h->pServiceConfig,
							SRV_CONFIG_SIZE,
							&dwBufSize) )
		return false;

	if( h->pServiceConfig->dwStartType == dwServiceStartupType )
		return true;

	if( !ChangeServiceConfig(	h->schService,
								SERVICE_NO_CHANGE,
								dwServiceStartupType,
								SERVICE_NO_CHANGE,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								h->pServiceConfig->lpDisplayName) )
		return false;

	return true;
}

/////////////////////////////////////////////////

	bool
SRV_IsSystemAccount(
	HSRV	hSrv
)
{
	PHSERVICE h = (PHSERVICE)hSrv;
	DWORD dwBufSize;
	if(	h == NULL ||
		!QueryServiceConfig(h->schService,
							h->pServiceConfig,
							SRV_CONFIG_SIZE,
							&dwBufSize))
		return false;

	if(	h->pServiceConfig->lpServiceStartName == NULL ||
		lstrcmpi(h->pServiceConfig->lpServiceStartName, __TEXT("LocalSystem")) == 0)
		return true;

	return false;
}

// EOF ///////////////////////////////////////////