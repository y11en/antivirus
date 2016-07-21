#define WIN32_LEAN_AND_MEAN
#define WINVER			0x0400
#define _WIN32_WINNT	0x0400
#include <windows.h>

char exename[MAX_PATH];

int Win_Main()
{
	// ƒадим агенту 2 минуты, чтобы отослать сообщение о завершении инстал€ции в јдмин ит
	Sleep(2 * 60 * 1000);

	SC_HANDLE hSCM = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM
		&& NULL == (hSCM = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT)))
		return -1;

	SC_HANDLE hNag = OpenService(hSCM, "klnagent", SERVICE_QUERY_STATUS|SERVICE_START|SERVICE_STOP);
	if (hNag)
	{
		SERVICE_STATUS status;
		if (QueryServiceStatus(hNag, &status)
			&& (status.dwCurrentState == SERVICE_RUNNING
				|| status.dwCurrentState == SERVICE_START_PENDING))
		{
			SERVICE_STATUS status2;
			ControlService(hNag, SERVICE_CONTROL_STOP, &status2);

			for (int i = 0; i < 180; ++i)
			{
				if (QueryServiceStatus(hNag, &status2)
					&& status2.dwCurrentState == SERVICE_STOPPED)
					break;
				Sleep(1000);
			}

			StartService(hNag, 0, NULL);
		}

		CloseServiceHandle(hNag);
	}

	CloseServiceHandle(hSCM);

    // delete itself on reboot
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule)
    {
        if (GetModuleFileName(hModule, exename, MAX_PATH) != 0)
        {
            MoveFileEx(exename, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
        }
    }

	return 0;
}