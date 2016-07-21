#ifndef __AVPSRVEX_H__
#define __AVPSRVEX_H__

#include <service/avpsrv.h>

	HSRV
SRV_InitEx (
	const TCHAR *	pszMachineName,
	const TCHAR *	pszServiceName,
	DWORD dwSCMAccess = SC_MANAGER_ALL_ACCESS,
	DWORD dwServiceAccess = SERVICE_ALL_ACCESS
);

	bool
SRV_InstallExW (
	HSRV	hSrv,
	WCHAR *	pszDisplayName,
	DWORD	dwServiceType,		// SRV_TYPE_...
	DWORD	dwStartType,		// SRV_STARTUP_TYPE_...
	WCHAR *	pszDependencies,	// NULL
	WCHAR *	pszPathName,		// NULL
	WCHAR *	pszAccountName,		// NULL
	WCHAR *	pszPassword			// NULL
)
;

	bool
SRV_RemoveNoStop (
	HSRV	hSrv
);

#endif