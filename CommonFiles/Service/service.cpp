// service.cpp ///////////////////////////////////

// Service
// [Version 1.04.003]
// Copyright (C) 1999 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include <winsvc.h>
#include "service.h"

//////////////////////////////////////////////////

LPCTSTR					g_pszServiceInternalName;
SERVICE_PSTART_HANDLER	g_pServiceStart;
DWORD					g_dwServiceStartingTime;
SERVICE_PSTOP_HANDLER	g_pServiceStop;
DWORD					g_dwServiceStoppingTime;

SERVICE_STATUS			g_ssStatus;
SERVICE_STATUS_HANDLE	g_sshStatusHandle;

//////////////////////////////////////////////////

						void
SERVICE_ReportStatus(	DWORD	dwCurrentState,
						DWORD	dwWin32ExitCode,
						DWORD	dwWaitHint) {

	static DWORD dwCheckPoint = 1;

    if(dwCurrentState == SERVICE_START_PENDING)	g_ssStatus.dwControlsAccepted = 0;
	else g_ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    g_ssStatus.dwCurrentState  = dwCurrentState;
    g_ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    g_ssStatus.dwWaitHint      = dwWaitHint;

	if(dwCurrentState == SERVICE_RUNNING
	|| dwCurrentState == SERVICE_STOPPED) g_ssStatus.dwCheckPoint = 0;
	else g_ssStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus(g_sshStatusHandle, &g_ssStatus);
}

//////////////////////////////////////////////////

				void
					WINAPI
SERVICE_Control(DWORD	dwCtrlCode) {

	switch(dwCtrlCode) {
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			SERVICE_ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, g_dwServiceStoppingTime);
			g_pServiceStop();
			break;
		default:
			SERVICE_ReportStatus(g_ssStatus.dwCurrentState, 0, 0);
    }
}

//////////////////////////////////////////////////

				void
					WINAPI
SERVICE_Main(	DWORD		dwArgc,
				LPTSTR *	pszArgv) {

	g_sshStatusHandle = RegisterServiceCtrlHandler(g_pszServiceInternalName, SERVICE_Control);
    if(!g_sshStatusHandle) {
		SERVICE_ReportStatus(SERVICE_STOPPED, 0, 0);
		return;
	}

    g_ssStatus.dwServiceType				= SERVICE_WIN32_OWN_PROCESS;
    g_ssStatus.dwServiceSpecificExitCode	= 0;

	SERVICE_ReportStatus(SERVICE_START_PENDING, NO_ERROR, g_dwServiceStartingTime);
	g_pServiceStart(dwArgc, pszArgv);
	SERVICE_ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

//////////////////////////////////////////////////

				BOOL
SERVICE_Init(	LPTSTR					pszServiceInternalName,
				SERVICE_PSTART_HANDLER	pServiceStart,
				DWORD					dwServiceStartingTime,
				SERVICE_PSTOP_HANDLER	pServiceStop,
				DWORD					dwServiceStoppingTime) {

	if(	pszServiceInternalName == NULL ||
		*pszServiceInternalName == 0 ||
		pServiceStart == NULL ||
		pServiceStop == NULL)
		return FALSE;

	if(dwServiceStartingTime == 0) dwServiceStartingTime = SERVICE_STARTING_DEFAULT_TIME;
	if(dwServiceStoppingTime == 0) dwServiceStoppingTime = SERVICE_STOPPING_DEFAULT_TIME;

	g_pszServiceInternalName	= pszServiceInternalName;
	g_pServiceStart				= pServiceStart;
	g_dwServiceStartingTime		= dwServiceStartingTime;
	g_pServiceStop				= pServiceStop;
	g_dwServiceStoppingTime		= dwServiceStoppingTime;

	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{pszServiceInternalName,	SERVICE_Main},
		{NULL,						NULL}
	};

	return StartServiceCtrlDispatcher(DispatchTable);
}

//////////////////////////////////////////////////

				void
SERVICE_Running() {

	SERVICE_ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
}

// EOF ///////////////////////////////////////////