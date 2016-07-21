// DisableProtection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

#include "../../Hook/avpgcom.h"
#include "../../Hook/FSDrvLib.h"
#include "../../Hook/idriver.h"
#include "../../Hook/hookspec.h"
#include "../../Hook/Funcs.h"

BOOL AddFilters(HANDLE hDevice, ULONG AppID)
{
	BOOL bFilterAdded = TRUE;
	
	DWORD DeadlockTimeout = DEADLOCKWDOG_TIMEOUT;
	
	DWORD dwFlags;
	
	DWORD dwSite = 0;
	
	dwFlags = FLT_A_DEFAULT | FLT_A_DENYPASS2LOWPR;

	bFilterAdded &= AddFSFilter(hDevice, AppID, "*", "*", DeadlockTimeout, dwFlags, FLTTYPE_SYSTEM, MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, &dwSite);
	bFilterAdded &= AddFSFilter(hDevice, AppID, "*", "*", DeadlockTimeout, dwFlags, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, &dwSite);
	
	return bFilterAdded;
}

int main(int argc, char* argv[])
{
	if (!FSDrvInterceptorInitEx(_CLIENT_FLAG_WITHOUTWATCHDOG, AVPG_Driver_Specific, AVPG_INVISIBLEPRIORITY + 1, 0))
		return 1;

	if (!AddFilters(FSDrvGetDeviceHandle(), FSDrvGetAppID()))
		return 2;

	FSDrvInterceptorSetActive(TRUE);

	::MessageBox(NULL, "Protection disabled. (for terminate)", "DisableProtection", MB_ICONINFORMATION);

	FSDrvInterceptorDone();	


	if (FSDrvInterceptorInitEx(_CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_SAVE_FILTERS | _CLIENT_FLAG_PARALLEL, InstallerProtection, AVPG_INVISIBLEPRIORITY, 0))
	{
		FSDrvInterceptorSetActive(FALSE);

		::MessageBox(NULL, "Protection disabled. (folder & registry)", "DisableProtection", MB_ICONINFORMATION);

		FSDrvInterceptorDone();
	}

	return 0;
}
