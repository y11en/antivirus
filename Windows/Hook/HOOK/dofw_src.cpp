#include <windows.h>
#include <shlobj.h>

#include <version/ver_product.h>

#include "../../../windows/Hook/hook/avpgcom.h"
#include "../../../windows/Hook/hook/IDriver.h"
#include "../../../windows/Hook/hook/HookSpec.h"
#include "../../../windows/Hook/hook/Funcs.h"

typedef enum _eThreeState
{
	_eTS_None = 0,
	_eTS_On = 1,
	_eTS_Off = 2
}eThreeState;

void AddSecurityFilters(HANDLE hDevice, ULONG AppID, PWCHAR pwchService)
{
	ULONG len;
	BYTE buf_cont[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM ParamContext = (PFILTER_PARAM) buf_cont;
	PFILTER_PARAM ParamUrl;

	len = (lstrlenW(pwchService) + 1) * sizeof(WCHAR);

	ULONG fltsize = sizeof(FILTER_PARAM) + len;

    ParamUrl = (PFILTER_PARAM) HeapAlloc(GetProcessHeap(), 0, fltsize + 0x100);
	if (ParamUrl)
	{
		ZeroMemory(ParamUrl, fltsize);

		ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
		ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
		memcpy((PWCHAR) (ParamUrl->m_ParamValue), pwchService, len);
		ParamUrl->m_ParamSize = len;

		AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, 
			FLTTYPE_SYSTEM, MJ_SYSTEM_SETREGSECURITY, 0, 0, PreProcessing, 0, ParamUrl, 0);

	}
}

void AddFiltersForPath(HANDLE hDevice, ULONG AppID, PWCHAR pwchCurrentFN)
{
	ULONG len;
	BYTE buf_cont[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM ParamContext = (PFILTER_PARAM) buf_cont;
	PFILTER_PARAM ParamUrl;

	len = (lstrlenW(pwchCurrentFN) + 1) * sizeof(WCHAR);

	ULONG fltsize = sizeof(FILTER_PARAM) + len;

    ParamUrl = (PFILTER_PARAM) HeapAlloc(GetProcessHeap(), 0, fltsize + 0x100);
	if (ParamUrl)
	{
		ZeroMemory(ParamUrl, fltsize);

		ParamContext->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		ParamContext->m_ParamFlt = FLT_PARAM_AND;
		ParamContext->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
		ParamContext->m_ParamSize = sizeof(ULONG);
		*(ULONG*)ParamContext->m_ParamValue = _CONTEXT_OBJECT_FLAG_FORWRITE | _CONTEXT_OBJECT_FLAG_FORDELETE;


		ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
		ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
		memcpy((PWCHAR) (ParamUrl->m_ParamValue), pwchCurrentFN, len);
		ParamUrl->m_ParamSize = len;

	//+ ------------------------------------------------------------------------------------------
	/*
	 *	ps. check only lfn names
	 *	open for write in {this} folder
	 *	rename from {this}
	 *	delete in {this folder}
	 */
	//- ------------------------------------------------------------------------------------------
		
		AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, 
			FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, 0, ParamContext, ParamUrl, 0);
	}
}

void AddRenameFilter(HANDLE hDevice, ULONG AppID, PWCHAR pwchCurrentFN)
{
	ULONG len;
	PFILTER_PARAM ParamUrl;

	len = (lstrlenW(pwchCurrentFN) + 1) * sizeof(WCHAR);

	ULONG fltsize = sizeof(FILTER_PARAM) + len;

    ParamUrl = (PFILTER_PARAM) HeapAlloc(GetProcessHeap(), 0, fltsize + 0x100);
	if (ParamUrl)
	{
		ZeroMemory(ParamUrl, fltsize);

		ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
		ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
		memcpy((PWCHAR) (ParamUrl->m_ParamValue), pwchCurrentFN, len);
		ParamUrl->m_ParamSize = len;

		AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, 
			FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, FileRenameInformation, 0, PreProcessing, 0, ParamUrl, 0);
	}
}

void CheckParentIsKaspTooAndProtect(HANDLE hDevice, ULONG AppID, PWCHAR pwchCurrentFN)
{
	DWORD len = lstrlenW(pwchCurrentFN);

	if (!len)
		return;

	DWORD alloclen = (len + 1 ) * sizeof(WCHAR);;
	PWCHAR pwchTmp = (PWCHAR) HeapAlloc(GetProcessHeap(), 0, alloclen);

	if (!pwchTmp)
		return;

	lstrcpyW(pwchTmp, pwchCurrentFN);

	while (len)
	{
		WCHAR c = pwchTmp[len];
		pwchTmp[len] = 0;
		len--;
		if (c == L'\\')
			break;
	}
		
	if (len)
	{
		while (len)
		{
			if (pwchTmp[len] == L'\\')
			{
				len++;
				break;
			}

			len--;
		}

	}

	if (len)
	{
		if (!lstrcmpiW(pwchTmp + len, L"Kaspersky Lab"))
		{
			AddRenameFilter(hDevice, AppID, pwchTmp);
			
			WCHAR ShortCommonAppPath[MAX_PATH + 1];

			if (GetShortPathNameW(pwchTmp, ShortCommonAppPath, sizeof(ShortCommonAppPath) / sizeof(WCHAR)))
				AddRenameFilter(hDevice, AppID, ShortCommonAppPath);
		}
	}

	HeapFree(GetProcessHeap(), 0, pwchTmp);
}

bool GetSpecialLocation(DWORD ShellId, PWCHAR pwchStr)
{
	bool bRet = false;
	LPITEMIDLIST pListId = NULL;
	SHGetSpecialFolderLocation(NULL, ShellId, &pListId);

	if(pListId)
	{
		if( SHGetPathFromIDListW(pListId, pwchStr))
			bRet = true;
	}

	CoTaskMemFree((PVOID)pListId);

	return bRet;
}

void ProtectAllUserAppData(HANDLE hDevice, ULONG AppID)
{
	WCHAR CommonAppPath[MAX_PATH * 2];
	WCHAR ShortCommonAppPath[MAX_PATH + 1];
	ZeroMemory(CommonAppPath, sizeof(CommonAppPath));
	ZeroMemory(ShortCommonAppPath, sizeof(ShortCommonAppPath));

	if (GetSpecialLocation(CSIDL_COMMON_APPDATA, CommonAppPath))
	{
		DWORD shortlen;
		if (CommonAppPath[lstrlenW(CommonAppPath) - 1] != L'\\')
			lstrcatW(CommonAppPath, L"\\");

		lstrcatW(CommonAppPath, L"Kaspersky Lab\\" VER_PRODUCT_APPDATA_NAME_W);
		shortlen = GetShortPathNameW(CommonAppPath, ShortCommonAppPath, sizeof(ShortCommonAppPath) / sizeof(WCHAR));

		lstrcatW(CommonAppPath, L"\\*");
		AddFiltersForPath(hDevice, AppID, CommonAppPath);

		if (shortlen)
		{
			lstrcatW(ShortCommonAppPath, L"\\*");
			AddFiltersForPath(hDevice, AppID, ShortCommonAppPath);
		}
	}
}

void ProtectKey(HANDLE hDevice, ULONG AppID, PFILTER_PARAM ParamUrl)
{
	ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;

	ParamUrl->m_ParamSize = (lstrlenW((PWCHAR) (ParamUrl->m_ParamValue)) + 1) * sizeof(WCHAR);

	AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY,
		FLTTYPE_REGS, Interceptor_SetValueKey, 0, 0, PreProcessing, NULL, ParamUrl, 0);

	AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY,
		FLTTYPE_REGS, Interceptor_DeleteKey, 0, 0, PreProcessing, NULL, ParamUrl, 0);

	AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY,
		FLTTYPE_REGS, Interceptor_DeleteValueKey, 0, 0, PreProcessing, NULL, ParamUrl, 0);
}

HANDLE RegisterInDriver(POSVERSIONINFO pOSVer, PAPP_REGISTRATION pAppReg, PHANDLE phWhistleup, PHANDLE phWFChanged, bool bMiniCheck = true)
{
	pOSVer->dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(pOSVer);

	pAppReg->m_CurProcId = GetCurrentProcessId();
	pAppReg->m_AppID = InstallerProtection;
	pAppReg->m_CacheSize = 0;
	pAppReg->m_Priority = AVPG_INVISIBLEPRIORITY;
	pAppReg->m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_SAVE_FILTERS | _CLIENT_FLAG_PARALLEL;
	pAppReg->m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);

	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];

	wsprintfA(WhistleupName, "DWU%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
	wsprintfA(WFChangedName, "DWC%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
	
	CLIENT_INFO ClientInfo;

	HANDLE hDevice = RegisterAppEx(pAppReg, pOSVer, phWhistleup, phWFChanged, WhistleupName, WFChangedName, &ClientInfo);

	if (INVALID_HANDLE_VALUE != hDevice)
	{
		if (bMiniCheck)
		{
			ADDITIONALSTATE AddState;
			if (!IDriverGetAdditionalState(hDevice, &AddState))
				AddState.DrvFlags = _DRV_FLAG_NONE;

			if (AddState.DrvFlags & _DRV_FLAG_MINIFILTER)	// temporary not supported
			{
				UnRegisterApp(hDevice, pAppReg, FALSE, pOSVer);
				CloseHandle(*phWhistleup);
				*phWhistleup = NULL;

				CloseHandle(*phWFChanged);
				*phWFChanged = NULL;

				hDevice = INVALID_HANDLE_VALUE;
			}
		}
	}


	return hDevice;
}

//////////////////////////////////////////////////////////////////////////
typedef struct _THREAD_INFO {
	CHAR	  U1[0x10];
	DWORD    lowCreationTime;
	DWORD    hiCreationTime;
	DWORD    U2;
	DWORD    StartAddr;
	DWORD    OwningPID;
	DWORD	  TID;
	DWORD    CurrentPriority;
	DWORD    BasePriority;
	DWORD    ContextSwitches;
	DWORD    ThreadState;
	DWORD    WaitReason;
	DWORD    U3;
}THREAD_INFO,*PTHREAD_INFO;

#pragma warning (disable : 4200)
typedef struct _PROCESS_INFO {
	DWORD Len;
	DWORD ThreadsNum;			//threads number (need)
	DWORD U1[6];
	FILETIME CreationTime;		// creation time (need)
	LARGE_INTEGER liUserTime;	//?
	LARGE_INTEGER liKernelTime;	//?
	DWORD U2;
	WCHAR* ProcessName;			//In snapshoter addrspace - process name (need)
	DWORD BasePriority;			
	DWORD PID;					// process ID (need)
	DWORD ParentPID;			// parent process ID (need)
	DWORD HandleCount;
	DWORD U3[2];
	DWORD PeakVirtualSize;
	DWORD VirtualSize;
	DWORD WorkSetFaults;
	DWORD WorkSetPeak;
	DWORD WorkingSet;			//сколько памяти реально маппировано в физической памяти в данный момент
	DWORD PagedPoolPeak;
	DWORD PagedPool;
	DWORD NonPagedPoolPeak;
	DWORD NonPagedPool;
	DWORD PrivateBytes;
	DWORD PageFileBytesPeak;
	DWORD PageFileBytes;		
	THREAD_INFO	Ti[0];
}PROCESS_INFO,*PPROCESS_INFO;
#pragma warning (default : 4200)

typedef DWORD (FAR WINAPI *_pZwQuerySystemInformation)(IN ULONG /*InfoClass*/,OUT PVOID /*SysInfo*/,IN ULONG /*SysInfoLen*/,OUT PULONG /*LenReturned */OPTIONAL);

BOOL GetNTProcessInfo(CHAR** ppInfoBuff)
{
	DWORD dwBuffSize;
	DWORD dwResult;

	HMODULE hModNtDll;
	_pZwQuerySystemInformation pZwQuerySystemInformation = NULL; // pointer to kernel function

	hModNtDll = GetModuleHandleA("ntdll.dll");
	if (hModNtDll)
		pZwQuerySystemInformation = (_pZwQuerySystemInformation) GetProcAddress(hModNtDll, "ZwQuerySystemInformation");

	if (!pZwQuerySystemInformation)
		return 0;

	dwBuffSize = 1024 * 32;
	do
	{
		if(!(*ppInfoBuff = (CHAR*) HeapAlloc(GetProcessHeap(), 0, dwBuffSize)))
			break;

		dwResult = pZwQuerySystemInformation((DWORD)5, *ppInfoBuff, dwBuffSize, NULL);
		if(dwResult == 0)
			break;
		else
		{
			// STATUS_BUFFER_OVERFLOW || status == STATUS_INFO_LENGTH_MISMATCH
			if(dwResult == 0x80000005L || dwResult == 0xC0000004L)
			{
				HeapFree(GetProcessHeap(), 0, *ppInfoBuff);
				*ppInfoBuff = NULL;
				dwBuffSize += 0x2000;
			}
			else
			{
				HeapFree(GetProcessHeap(), 0, *ppInfoBuff);
				*ppInfoBuff = NULL;
				break;
			}
		}
	} while(TRUE);
	
	if (*ppInfoBuff == NULL)
		return FALSE;

	return TRUE;
}

DWORD GetParentPid()
{
	DWORD ParentPid = 0;
	DWORD dwCurrent = GetCurrentProcessId();

	CHAR* pProcList = NULL;

	if (!GetNTProcessInfo(&pProcList))
		return 0;

	PROCESS_INFO* pNextProcess;
	
	pNextProcess = (PROCESS_INFO*) pProcList;
	
	do
	{
		if (dwCurrent == pNextProcess->PID)
		{
			ParentPid = pNextProcess->ParentPID;
			break;
		}

	}	while(pNextProcess->Len && (pNextProcess = (PROCESS_INFO*)((CHAR*)pNextProcess + pNextProcess->Len)));
	

	HeapFree(GetProcessHeap(), 0, pProcList);

	return ParentPid;
}

DWORD GetParentPidCheckMsi(DWORD CheckProcessId)
{
	DWORD ParentPid = 0;

	CHAR* pProcList = NULL;

	if (!GetNTProcessInfo(&pProcList))
		return 0;

	PROCESS_INFO* pNextProcess;
	
	pNextProcess = (PROCESS_INFO*) pProcList;
	
	do
	{
		if (CheckProcessId == pNextProcess->PID)
		{
			if (!lstrcmpiW(L"msiexec.exe", pNextProcess->ProcessName))
				ParentPid = pNextProcess->ParentPID;
			break;
		}

	}	while(pNextProcess->Len && (pNextProcess = (PROCESS_INFO*)((CHAR*)pNextProcess + pNextProcess->Len)));

	HeapFree(GetProcessHeap(), 0, pProcList);

	return ParentPid;
}

DWORD GoProc(DWORD Param1, DWORD Param2)
{
	OSVERSIONINFO OSVer;
	APP_REGISTRATION AppReg;
	HANDLE hWhistleup;
	HANDLE hWFChanged;

	HANDLE hDevice = RegisterInDriver(&OSVer, &AppReg, &hWhistleup, &hWFChanged);
	if (INVALID_HANDLE_VALUE == hDevice)
		return 0;

	INVISIBLE_TRANSMIT InvTransmit;

	InvTransmit.m_AppID = AppReg.m_AppID;
	InvTransmit.m_InvCtl = INVCTL_PROCADD;
	InvTransmit.m_ThreadID = GetParentPid();

	DWORD BytesRet;
	DeviceIoControl (
		hDevice,
		IOCTLHOOK_InvisibleThreadOperations,
		&InvTransmit,
		sizeof(INVISIBLE_TRANSMIT),
		&InvTransmit,
		sizeof(INVISIBLE_TRANSMIT),
		&BytesRet,
		NULL
		);

	if (InvTransmit.m_ThreadID)
	{
		DWORD ParentId = InvTransmit.m_ThreadID;
		InvTransmit.m_ThreadID = GetParentPidCheckMsi(ParentId);
		
		if (InvTransmit.m_ThreadID)
		{
			DeviceIoControl (
				hDevice,
				IOCTLHOOK_InvisibleThreadOperations,
				&InvTransmit,
				sizeof(INVISIBLE_TRANSMIT),
				&InvTransmit,
				sizeof(INVISIBLE_TRANSMIT),
				&BytesRet,
				NULL
				);
		}
	}

	UnRegisterApp(hDevice, &AppReg, FALSE, &OSVer);
	CloseHandle(hWhistleup);
	CloseHandle(hWFChanged);

	return 1;
}

//////////////////////////////////////////////////////////////////////////
DWORD Protect(eThreeState eActivateStatus, BOOL bFile, PWCHAR pwchCurrentFN, BOOL bReg, BOOL bSecurity)
{
	OSVERSIONINFO OSVer;
	APP_REGISTRATION AppReg;
	HANDLE hWhistleup;
	HANDLE hWFChanged;

	HANDLE hDevice = RegisterInDriver(&OSVer, &AppReg, &hWhistleup, &hWFChanged);
	if (INVALID_HANDLE_VALUE == hDevice)
		return 1;

	IDriverResetFilters(hDevice, AppReg.m_AppID);

	BYTE buff[0x1000];

	if (bFile)
	{
		ProtectAllUserAppData(hDevice, AppReg.m_AppID);
	
		WCHAR ShortCommonAppPath[MAX_PATH + 1];
		DWORD shortlen = GetShortPathNameW(pwchCurrentFN, ShortCommonAppPath, sizeof(ShortCommonAppPath) / sizeof(WCHAR));

		AddRenameFilter(hDevice, AppReg.m_AppID, pwchCurrentFN);

		CheckParentIsKaspTooAndProtect(hDevice, AppReg.m_AppID, pwchCurrentFN);

		lstrcatW(pwchCurrentFN, L"\\*");
		AddFiltersForPath(hDevice, AppReg.m_AppID, pwchCurrentFN);

		if (shortlen)
		{
			AddRenameFilter(hDevice, AppReg.m_AppID, ShortCommonAppPath);
			lstrcatW(ShortCommonAppPath, L"\\*");
			AddFiltersForPath(hDevice, AppReg.m_AppID, ShortCommonAppPath);
		}

		if (ExpandEnvironmentStringsW(L"%windir%\\system32\\drivers\\klif.sys", (PWCHAR) buff, sizeof(buff) / sizeof(WCHAR)))
			AddFiltersForPath(hDevice, AppReg.m_AppID, (PWCHAR) buff);

		if (ExpandEnvironmentStringsW(L"%windir%\\system32\\drivers\\kl1.sys", (PWCHAR) buff, sizeof(buff) / sizeof(WCHAR)))
			AddFiltersForPath(hDevice, AppReg.m_AppID, (PWCHAR) buff);

		if (ExpandEnvironmentStringsW(L"%windir%\\system32\\drivers\\klop.dat", (PWCHAR) buff, sizeof(buff) / sizeof(WCHAR)))
			AddFiltersForPath(hDevice, AppReg.m_AppID, (PWCHAR) buff);


		bool bProtectKlim5 = true;
		bool bProtectKlim6 = true;
		OSVERSIONINFO OSVer;
		OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if(GetVersionEx(&OSVer))
		{
			if (6 == OSVer.dwMajorVersion) // longhorn
			{
				bProtectKlim6 = true;
				bProtectKlim5 = false;
			}
			else
			{
				bProtectKlim6 = false;
				bProtectKlim5 = true;
			}
		}
		//xp - klim5, vista klim6
		if (bProtectKlim5)
		{
			if (ExpandEnvironmentStringsW(L"%windir%\\system32\\drivers\\klim5.sys", (PWCHAR) buff, sizeof(buff) / sizeof(WCHAR)))
				AddFiltersForPath(hDevice, AppReg.m_AppID, (PWCHAR) buff);
		}

		if (bProtectKlim6)
		{
			if (ExpandEnvironmentStringsW(L"%windir%\\system32\\drivers\\klim6.sys", (PWCHAR) buff, sizeof(buff) / sizeof(WCHAR)))
				AddFiltersForPath(hDevice, AppReg.m_AppID, (PWCHAR) buff);
		}
	}

	if (bReg)
	{
		PFILTER_PARAM ParamUrl = (PFILTER_PARAM) buff;
		ZeroMemory(buff, sizeof(buff));

		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SOFTWARE\\KasperskyLab\\protected\\*");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);

		// drivers
		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\KLIF\\*");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);
		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\KLIF");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);

		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\AVP\\*");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);
		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\AVP");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);

		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET*\\SERVICES\\KL1");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);
		lstrcpyW((PWCHAR) (ParamUrl->m_ParamValue), L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET*\\SERVICES\\KL1\\*");
		ProtectKey(hDevice, AppReg.m_AppID, ParamUrl);
	}

	if (bSecurity)
	{
		AddSecurityFilters(hDevice, AppReg.m_AppID, L"*\\SERVICES\\AVP");
		AddSecurityFilters(hDevice, AppReg.m_AppID, L"*\\SERVICES\\AVP\\*");
	}

	IDriverSaveFilters(hDevice, AppReg.m_AppID);

	APPSTATE CurrentState;
	switch (eActivateStatus)
	{
	case _eTS_On:
		IDriverState(hDevice, AppReg.m_AppID, _AS_GoActive, &CurrentState);
		break;
	
	case _eTS_Off:
		IDriverState(hDevice, AppReg.m_AppID, _AS_GoSleep, &CurrentState);
		break;

	case _eTS_None:
	default:
		break;
	}

	UnRegisterApp(hDevice, &AppReg, FALSE, &OSVer);
	CloseHandle(hWhistleup);
	CloseHandle(hWFChanged);

	return 0;
}

DWORD GoThisImp(PWCHAR pwchCurrentFN, DWORD ParamState, BOOLEAN bRegThread)
{
	return Protect(_eTS_On, TRUE, pwchCurrentFN, ParamState != 3 ? TRUE : FALSE, TRUE);
}

VOID GoThisBackImp()
{
	//////////////////////////////////////////////////////////////////////////
	OSVERSIONINFO OSVer;
	APP_REGISTRATION AppReg;
	HANDLE hWhistleup;
	HANDLE hWFChanged;

	HANDLE hDevice = RegisterInDriver(&OSVer, &AppReg, &hWhistleup, &hWFChanged);
	if (INVALID_HANDLE_VALUE == hDevice)
		return;

	IDriverResetFilters(hDevice, AppReg.m_AppID);

	IDriverSaveFilters(hDevice, AppReg.m_AppID);

	UnRegisterApp(hDevice, &AppReg, FALSE, &OSVer);
	CloseHandle(hWhistleup);
	CloseHandle(hWFChanged);
}

void DisableFBImp()
{
	OSVERSIONINFO OSVer;
	APP_REGISTRATION AppReg;
	HANDLE hWhistleup;
	HANDLE hWFChanged;

	HANDLE hDevice = RegisterInDriver(&OSVer, &AppReg, &hWhistleup, &hWFChanged, false);
	if (INVALID_HANDLE_VALUE == hDevice)
		return;

	char Buffer[sizeof(EXTERNAL_DRV_REQUEST)];
	memset(Buffer, 0, sizeof(Buffer));
	
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
	pRequest->m_DrvID = FLTTYPE_SYSTEM;
	pRequest->m_IOCTL = _AVPG_IOCTL_FIDBOX_DISABLE;
	pRequest->m_AppID = AppReg.m_AppID;

	ULONG retsize = 0;
	IDriverExternalDrvRequest(hDevice, pRequest, NULL, &retsize);

	UnRegisterApp(hDevice, &AppReg, FALSE, &OSVer);
	CloseHandle(hWhistleup);
	CloseHandle(hWFChanged);
}