// MSDevPass.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Windows.h"

#include "../../hook/avpgcom.h"
#include "..\..\Hook\FSDrvLib.h"
#include "..\..\Hook\IDriver.h"
#include "..\..\hook\HookSpec.h"
#include "..\..\hook\Funcs.h"

BOOL AddInvisibleProcess(HANDLE hDevice, ULONG AppID, OSVERSIONINFO* pOSVer, char* pcz)
{
	BOOL bFilterAdded = TRUE;
	
	DWORD DeadlockTimeout = DEADLOCKWDOG_TIMEOUT;
	
	DWORD dwFlags;
	
	DWORD dwSite = 0;
	
	dwFlags = FLT_A_DEFAULT | FLT_A_DENYPASS2LOWPR;
	if (pOSVer->dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_FIOR, FastIoWrite, 0, 0, PreProcessing, &dwSite);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PreProcessing, NULL);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_NORMAL, 0, PreProcessing, &dwSite);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL);
	}
	else
	{
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_IFS,IFSFN_OPEN, 0, 0, PreProcessing, &dwSite);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_IFS,IFSFN_OPEN, 1, 0, PreProcessing, &dwSite);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_IFS,IFSFN_OPEN, 2, 0, PreProcessing, &dwSite);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_IFS,IFSFN_OPEN, 3, 0, PreProcessing, &dwSite);
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_I21,0x3d, 0, 0, PreProcessing, &dwSite);	//open
		bFilterAdded &= AddFSFilter(hDevice, AppID, "*", pcz, DeadlockTimeout, dwFlags, FLTTYPE_IFS,IFSFN_CLOSE, 0, 0, PreProcessing, &dwSite);
	}
	
	return bFilterAdded;
}


int main(int argc, char* argv[])
{
	HANDLE				hDevice = NULL;
	
	APP_REGISTRATION	AppReg;
	
	HANDLE				hWhistleup = NULL;
	HANDLE				hWFChanged = NULL;
	
	char				WhistleupName[MAX_PATH]; 
	char				WFChangedName[MAX_PATH]; 
	
	IDriver_ReloadSettings(0);
	
	
	OSVERSIONINFO		OSVer;
	
	AppReg.m_AppID = AVPG_Debug;
	wsprintf(WhistleupName, "MSDEVPassWhistleup", AppReg.m_AppID);
	wsprintf(WFChangedName, "MSDEVPassChanged", AppReg.m_AppID);
	
	AppReg.m_Priority = AVPG_INVISIBLEPRIORITY;
	AppReg.m_CurProcId = GetCurrentProcessId();
	
	AppReg.m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG;
	AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
	AppReg.m_LogFileName[0] = 0;

	OSVer.dwOSVersionInfoSize = sizeof(OSVer);
	GetVersionEx(&OSVer);
	
	hDevice = RegisterApp(&AppReg, &OSVer, &hWhistleup, &hWFChanged, WhistleupName, WFChangedName);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		DbgPrint(1, "Register Undebugger failed!\n");
	}
	else
	{
		AddInvisibleProcess(hDevice, AppReg.m_AppID, &OSVer, "MSDEV*");
		AddInvisibleProcess(hDevice, AppReg.m_AppID, &OSVer, "SSEXP*");
		AddInvisibleProcess(hDevice, AppReg.m_AppID, &OSVer, "RATIONAL.EXE");
		AddInvisibleProcess(hDevice, AppReg.m_AppID, &OSVer, "WINDBG*");
		AddInvisibleProcess(hDevice, AppReg.m_AppID, &OSVer, "DEVENV*");

		UnRegisterApp(hDevice, &AppReg, TRUE, &OSVer);
	}

	return 0;
}
