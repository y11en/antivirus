// avpsrv.h //////////////////////////////////////

// Service Control Manager
// [Version 1.18.XXX]
// Copyright (C) 1999-2001 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#ifndef __AVPSRV_H
#define __AVPSRV_H

//////////////////////////////////////////////////

#define	SRV_RC_ERROR	-1

typedef	void *	HSRV;

//////////////////////////////////////////////////

// SRV_STATE_STOPPED				-	The service is not running.
// SRV_STATE_START_PENDING			-	The service is starting.
// SRV_STATE_STOP_PENDING			-	The service is stopping.
// SRV_STATE_RUNNING				-	The service is running.
// SRV_STATE_CONTINUE_PENDING		-	The service continue is pending.
// SRV_STATE_PAUSE_PENDING			-	The service pause is pending.
// SRV_STATE_PAUSED					-	The service is paused.

#define SRV_STATE_STOPPED				SERVICE_STOPPED
#define SRV_STATE_START_PENDING			SERVICE_START_PENDING
#define SRV_STATE_STOP_PENDING			SERVICE_STOP_PENDING
#define SRV_STATE_RUNNING				SERVICE_RUNNING
#define SRV_STATE_CONTINUE_PENDING		SERVICE_CONTINUE_PENDING
#define SRV_STATE_PAUSE_PENDING			SERVICE_PAUSE_PENDING
#define SRV_STATE_PAUSED				SERVICE_PAUSED

//////////////////////////////////////////////////

// SRV_TYPE_OWN_PROCESS				-	Specifies a service that runs in its own process.
// SRV_TYPE_SHARE_PROCESS			-	Specifies a service that shares a process with other services.
// SRV_TYPE_KERNEL_DRIVER			-	Specifies a driver service.
// SRV_TYPE_FILE_SYSTEM_DRIVER		-	Specifies a file system driver service.

// If you specify either SRV_TYPE_OWN_PROCESS or SRV_TYPE_SHARE_PROCESS,
// you can also specify the following type. 

// SRV_TYPE_INTERACTIVE_PROCESS		-	Enables a service to interact with the desktop.
// This value is valid only if lpServiceStartName is the LocalSystem account. 

#define SRV_TYPE_OWN_PROCESS			SERVICE_WIN32_OWN_PROCESS
#define SRV_TYPE_SHARE_PROCESS			SERVICE_WIN32_SHARE_PROCESS
#define SRV_TYPE_KERNEL_DRIVER			SERVICE_KERNEL_DRIVER
#define SRV_TYPE_FILE_SYSTEM_DRIVER		SERVICE_FILE_SYSTEM_DRIVER

#define SRV_TYPE_INTERACTIVE_PROCESS	SERVICE_INTERACTIVE_PROCESS 

//////////////////////////////////////////////////

// SRV_STARTUP_TYPE_BOOT_START		-	Specifies a device driver started by the operating system
//										loader. This value is valid only if the service type is
//										SERVICE_KERNEL_DRIVER or SERVICE_FILE_SYSTEM_DRIVER.
// SRV_STARTUP_TYPE_SYSTEM_START	-	Specifies a device driver started by the IoInitSystem
//										function. This value is valid only if the service type is
//										SERVICE_KERNEL_DRIVER or SERVICE_FILE_SYSTEM_DRIVER.
// SRV_STARTUP_TYPE_AUTO_START		-	Specifies a device driver or Win32 service started by
//										the service control manager automatically during system
//										startup.
// SRV_STARTUP_TYPE_DEMAND_START	-	Specifies a device driver or Win32 service started by the
//										service control manager when a process calls the
//										StartService function.
// SRV_STARTUP_TYPE_DISABLED		-	Specifies a device driver or Win32 service that can no
//										longer be started.

#define SRV_STARTUP_TYPE_BOOT_START		SERVICE_BOOT_START
#define SRV_STARTUP_TYPE_SYSTEM_START	SERVICE_SYSTEM_START
#define SRV_STARTUP_TYPE_AUTO_START		SERVICE_AUTO_START
#define SRV_STARTUP_TYPE_DEMAND_START	SERVICE_DEMAND_START
#define SRV_STARTUP_TYPE_DISABLED		SERVICE_DISABLED

//////////////////////////////////////////////////

	HSRV // NULL
SRV_Init(
	const TCHAR *	pszMachineName,
	const TCHAR *	pszServiceName
)
;

	void
SRV_Done(
	HSRV	hSrv
)
;

	bool
SRV_IsInstalled(
	HSRV	hSrv
)
;

	bool
SRV_Install(
	HSRV	hSrv,
	TCHAR *	pszDisplayName,
	TCHAR *	pszPathName,
	TCHAR *	pszAccountName,		// NULL
	TCHAR *	pszPassword			// NULL
)
;

	bool
SRV_InstallEx(
	HSRV	hSrv,
	TCHAR *	pszDisplayName,
	DWORD	dwServiceType,		// SRV_TYPE_...
	DWORD	dwStartType,		// SRV_STARTUP_TYPE_...
	TCHAR *	pszDependencies,	// NULL
	TCHAR *	pszPathName,		// NULL
	TCHAR *	pszAccountName,		// NULL
	TCHAR *	pszPassword			// NULL
)
;

	bool
SRV_Remove(
	HSRV	hSrv
)
;

	DWORD // SRV_RC_ERROR, SRV_STATE_...
SRV_GetCurrentState(
	HSRV	hSrv
)
;

	bool
SRV_Start(
	HSRV	hSrv
)
;

	void
SRV_Stop(
	HSRV	hSrv
)
;

	DWORD // SRV_RC_ERROR, SRV_STARTUP_TYPE_...
SRV_GetStartupType(
	HSRV	hSrv
)
;

	bool
SRV_SetStartupType(
	HSRV	hSrv,
	DWORD	dwServiceStartupType	// SRV_STARTUP_TYPE_...
)
;

	bool
SRV_IsSystemAccount(
	HSRV	hSrv
)
;

//////////////////////////////////////////////////

#endif // __AVPSRV_H

// EOF ///////////////////////////////////////////