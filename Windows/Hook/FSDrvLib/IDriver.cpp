//IDriver

#include "..\hook\IDriver.h"
#include <tchar.h>
//#include <AVPRegID.h>
#include "..\hook\HookSpec.h"
#include "..\hook\Funcs.h"
#include "service/sa.h"

#define _BUF_FILTER_SIZE	4096
// -----------------------------------------------------------------------------------------

char _APPNAME[MAX_PATH] = AVPGNAME;

#define _SysExt TEXT(".SYS")
#define _VxdExt TEXT(".VXD")

DWORD _DebugInfoLevel = 3;

typedef DWORD (__stdcall *_SysCall0)(enum AVPG_W32_Services ServiceNum);
typedef DWORD (__stdcall *_SysCall1)(enum AVPG_W32_Services ServiceNum, DWORD arg1);
typedef DWORD (__stdcall *_SysCall2)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2);
typedef DWORD (__stdcall *_SysCall3)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3);
typedef DWORD (__stdcall *_SysCall4)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4);

_SysCall0 gpSysCall0 = NULL;
_SysCall1 gpSysCall1 = NULL;
_SysCall2 gpSysCall2 = NULL;
_SysCall3 gpSysCall3 = NULL;
_SysCall4 gpSysCall4 = NULL;

extern OSVERSIONINFO g_FSDrvLibOSVer;

#ifndef STATUS_SUCCESS
	#define STATUS_SUCCESS	0L
#endif

class CAutoHSA
{
public:
	CAutoHSA()
	{
		m_pHSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
	}
	~CAutoHSA()
	{
		SA_Destroy(m_pHSA);
		m_pHSA = NULL;
	}

	PSECURITY_ATTRIBUTES Get()
	{
		return SA_Get(m_pHSA);
	}
	
	HSA m_pHSA;
};

BOOL IDriver_AddProtectionFilterNT(HANDLE hDevice, ULONG AppID, DWORD Flags);
BOOL IDriver_AddProtectionFilter9x(HANDLE hDevice, ULONG AppID, DWORD Flags);

HANDLE GetDriverHandle(char *pFileName);

#ifndef NTSTATUS
	#define NTSTATUS DWORD
#endif
#pragma warning( disable: 4035)
NTSTATUS __stdcall NTSysCall0(enum AVPG_W32_Services ServiceNum)
{
	_asm {
		mov eax, ServiceNum;
		int 2eh;
  }
}

NTSTATUS __stdcall NTSysCall1(enum AVPG_W32_Services ServiceNum, DWORD arg1)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}

NTSTATUS __stdcall NTSysCall2(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}

NTSTATUS __stdcall NTSysCall3(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}

NTSTATUS __stdcall NTSysCall4(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}
#pragma warning( default: 4035)

typedef BOOL (WINAPI *_tlstrlenW)(PWCHAR);
_tlstrlenW _plstrlenW = NULL;
typedef BOOL (WINAPI *_tlstrcpyW)(PWCHAR, PWCHAR);
_tlstrcpyW _plstrcpyW = NULL;

typedef int (WINAPI *_tlstrcmpW)(PWCHAR, PWCHAR);
_tlstrcmpW _plstrcmpW = NULL;

typedef int (WINAPI *_tlstrcmpiW)(PWCHAR, PWCHAR);
_tlstrcmpW _plstrcmpiW = NULL;

typedef BOOL (__stdcall *_tpLookupAccountSid)(LPCSTR, PSID, LPSTR, LPDWORD, LPSTR, LPDWORD, PSID_NAME_USE);
static _tpLookupAccountSid _gLookupAccountSid = NULL;

typedef BOOL (__stdcall *_tpLookupAccountSidW)(LPCWSTR, PSID, LPWSTR, LPDWORD, LPWSTR, LPDWORD, PSID_NAME_USE);
static _tpLookupAccountSidW _gLookupAccountSidW = NULL;

typedef DWORD (__stdcall *_tpExpandEnvironmentStrings)(LPCTSTR, LPTSTR, DWORD);
static _tpExpandEnvironmentStrings _gExpandEnvironmentStrings = NULL;


class IDriver_ExtCalls
{
public:
	IDriver_ExtCalls()
	{
		HINSTANCE hAdvapi = GetModuleHandle("advapi32.dll");
		if (hAdvapi != NULL)
		{
			if (_gLookupAccountSid == NULL)
				_gLookupAccountSid = (_tpLookupAccountSid) GetProcAddress(hAdvapi, "LookupAccountSidA");
			if (_gLookupAccountSidW == NULL)
				_gLookupAccountSidW = (_tpLookupAccountSidW) GetProcAddress(hAdvapi, "LookupAccountSidW");
		}

		HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");
		if (hKernel != NULL)
		{
			if (_plstrlenW == NULL)
				_plstrlenW = (_tlstrlenW) GetProcAddress(hKernel, "lstrlenW");
			if (_plstrcpyW == NULL)
				_plstrcpyW = (_tlstrcpyW) GetProcAddress(hKernel, "lstrcpyW");
			if (_plstrcmpW == NULL)
				_plstrcmpW = (_tlstrcmpW) GetProcAddress(hKernel, "lstrcmpW");
			if (_plstrcmpiW == NULL)
				_plstrcmpiW = (_tlstrcmpiW) GetProcAddress(hKernel, "lstrcmpiW");
			if (_gExpandEnvironmentStrings == NULL)
				_gExpandEnvironmentStrings = (_tpExpandEnvironmentStrings) GetProcAddress(hKernel, "ExpandEnvironmentStringsA");

		}
	}
};

IDriver_ExtCalls gExternCalls;

//+ ------------------------------------------------------------------------------------------


class IDirver_DirectCall
{
public:
	ULONG m_DrvFlags;
	ULONG m_DirectSysCallOffset;
	
	void Reset()
	{
		m_DrvFlags = _DRV_FLAG_NONE;
		m_DirectSysCallOffset = 0;
	}
	IDirver_DirectCall()
	{
		Reset();
	}
	
	~IDirver_DirectCall()
	{
		Reset();
	}
};

IDirver_DirectCall gIDriverDirectCall;

BOOL IDriverInitDirectCalls(OSVERSIONINFO *pOSVer)
{
	if (pOSVer->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (gpSysCall0 == 0)
		{
			gIDriverDirectCall.m_DrvFlags &= (~_DRV_FLAG_DIRECTSYSCALL);
			return FALSE;
		}
		else
		{
			gpSysCall1 = (_SysCall1) gpSysCall0;
			gpSysCall2 = (_SysCall2) gpSysCall0;
			gpSysCall3 = (_SysCall3) gpSysCall0;
			gpSysCall4 = (_SysCall4) gpSysCall0;
		}
	}
	else
	{
		//VER_PLATFORM_WIN32_NT
		gpSysCall0 = NTSysCall0;
		gpSysCall1 = NTSysCall1;
		gpSysCall2 = NTSysCall2;
		gpSysCall3 = NTSysCall3;
		gpSysCall4 = NTSysCall4;
	}

	return TRUE;
}

void ReparseDrvPath(char* path, DWORD pathlen)
{
	char szPath[MAX_PATH*2];
	typedef DWORD (WINAPI *_QUERYDOSDEVICE)(LPSTR, LPSTR, DWORD);
	_QUERYDOSDEVICE _QueryDosDevice = NULL;

	if (pathlen < 3 || path[1] != ':' || path[2] != '\\') // no drive letter
		return;

	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");
	if (hKernel != NULL)
	{
		_QueryDosDevice = (_QUERYDOSDEVICE) GetProcAddress(hKernel, "QueryDosDeviceA");
		if (_QueryDosDevice)
		{
			DWORD dwDeviceNameLen;
			DWORD dwPathLen;

			path[2] = 0;
			dwDeviceNameLen = _QueryDosDevice(path, szPath, sizeof(szPath)/sizeof(szPath[0]));
			path[2] = '\\';
			
			if (memcmp(szPath, "\\Device", sizeof("\\Device") - 1) == 0)
				return;


			if (dwDeviceNameLen)
			{
				char* newpath = szPath;
				dwPathLen = lstrlen(path+3);
				
				if(dwDeviceNameLen + dwPathLen + 1 <= sizeof(szPath)/sizeof(szPath[0]))
				{
					lstrcat(newpath, "\\");
					lstrcat(newpath, path+3);
				}
				
				if (memcmp(newpath, "\\??\\", 4) == 0)
				{
					newpath += 4;
					dwDeviceNameLen -= 4;
				}
				
				if(dwDeviceNameLen + dwPathLen + 1 <= pathlen)
				{
					lstrcpy(path, newpath);
				}
			}
		}
	}
}

BOOL EnableLoadDriverPrivilege(VOID)
{
	HANDLE hToken;
	LUID DebugValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
	{
		DbgPrint(2, "Can't open process token");
		return FALSE;
	}
	
	if (!LookupPrivilegeValue((LPSTR) NULL, SE_LOAD_DRIVER_NAME, &DebugValue))
	{
		DbgPrint(2, "Can't LookupPrivilegeValue - SE_LOAD_DRIVER_NAME");
		CloseHandle(hToken);
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = DebugValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE,&tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL);
	if (GetLastError() != ERROR_SUCCESS)
	{
		DbgPrint(2, "AdjustTokenPrivileges failed");
		CloseHandle(hToken);
		return FALSE;
	}
	
	CloseHandle(hToken);
	return TRUE;
}

// Все лентяям Лаборатории Касперского посвящается ;-)
#ifdef _DEBUG
VOID DbgPrint(DWORD level, LPCTSTR lpFmt,...)
{
	__try {
		va_list ArgPtr;
		int i;
		static char ModuleName[0x200] = {0};
		char Buff[2048];
		if(_DebugInfoLevel < level)
			return;
		Buff[0]=0;
		if ( !*ModuleName ) {
			if(GetModuleFileName(NULL,ModuleName,sizeof(ModuleName))) {
				for(i=strlen(ModuleName);i>=0;i--) {
					if(ModuleName[i]=='.')
						ModuleName[i]=0;
					if(ModuleName[i]=='\\') {
						lstrcpy(ModuleName,ModuleName+i+1);
						break;
					}
				}
			}
		}
		lstrcpy( Buff, ModuleName );
		wsprintf(Buff+strlen(Buff),"(%x)%d: ",GetCurrentThreadId(), level);
		va_start(ArgPtr,lpFmt);
		wvsprintf(Buff+strlen(Buff),lpFmt,ArgPtr);
		va_end(ArgPtr);
		OutputDebugString(Buff);
	}
	__except(1)	{
	}
}
#else 
#define DbgPrint
#endif // _DEBUG

// -----------------------------------------------------------------------------------------
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))
PVOID _IDriver_GetProcAddress(PVOID hModule, DWORD Ordinal)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_EXPORT_DIRECTORY exportDir;
	DWORD *functions;
	WORD *ordinals;
	PVOID NativeEP=0;
	pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		DbgPrint(1, "HOOK: MZ header not found\n");
		return 0;
	}
	pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
		DbgPrint(1, "HOOK: PE header not found\n");
		return 0;
	}
	exportDir=MakePtr(PIMAGE_EXPORT_DIRECTORY,hModule,
	pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	functions=MakePtr(DWORD*,hModule,exportDir->AddressOfFunctions);
	ordinals=MakePtr(WORD*,hModule,exportDir->AddressOfNameOrdinals);
	
	if (ordinals[Ordinal] > exportDir->NumberOfFunctions) 
	{
		DbgPrint(1, "HOOK: ExportAddr for %d not found\n", Ordinal);
		return 0;
	}
	
	return (PVOID)(functions[Ordinal - exportDir->Base] + (DWORD) hModule);
	//return (PVOID)(functions[exportDir->Base + ordinals[Ordinal]] + (DWORD) hModule);
}

// -----------------------------------------------------------------------------------------
#ifndef _UNICODE_STRING
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
#endif

typedef DWORD (__stdcall *_tpZwLoadDriver)(PUNICODE_STRING);	
//NTSYSAPI DWORD NTAPI pZwLoadDriver(IN PUNICODE_STRING /*DriverServiceName*/);
static _tpZwLoadDriver pZwLoadDriver = 0;
WCHAR _drvpath_avpg[] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\"AVPGNAME_W;
WCHAR _drvpath_tsp[] = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\tsp";

WCHAR* pDrvPath = _drvpath_avpg;

#define STATUS_IMAGE_ALREADY_LOADED      (0xC000010EL)
// -----------------------------------------------------------------------------------------

DWORD LoadDeviceDriver(const char *Name, const char *Path)
{
	SC_HANDLE	SchSCManager = 0;
	SC_HANDLE	schService;
	DWORD	dwErr = -1;
	
	UNICODE_STRING Uni;
	Uni.Buffer = (PWSTR) pDrvPath;
	if (pDrvPath == _drvpath_avpg)
		Uni.Length = sizeof(_drvpath_avpg) - 2;
	else
		Uni.Length = sizeof(_drvpath_tsp) - 2;
	Uni.MaximumLength = Uni.Length;
	
	if (pZwLoadDriver == 0)
		pZwLoadDriver = (_tpZwLoadDriver) GetProcAddress(GetModuleHandle("NTDLL.DLL"), "ZwLoadDriver");

	if (pZwLoadDriver != 0)
	{
		DWORD st = pZwLoadDriver(&Uni);
		DbgPrint(1, "FSDrvLib: ZwLoadDriver status - 0x%x (str len %d)\n", st, Uni.Length);
		if (st == 0 || st == STATUS_IMAGE_ALREADY_LOADED)
		{
			DbgPrint(1, "FSDrvLib: ZwLoadDriver ok\n");
			return 0;
		}
	}

	HANDLE hSyncMutex = CreateMutex(NULL, FALSE, "FSDrvLibSyncService");

	if (hSyncMutex != NULL)
		WaitForSingleObject(hSyncMutex, INFINITE);

	DbgPrint(1, "FSDrvLib: OpenSCManager\n");
	if (SchSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))
	{
		DbgPrint(1, "FSDrvLib: OpenService\n");
		if (schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS))
		{
			DbgPrint(1, "FSDrvLib: StartService\n");
			if ((StartService(schService, 0, NULL)) || ((dwErr = GetLastError()) == ERROR_SERVICE_ALREADY_RUNNING))
				dwErr = 0;
			else
				DbgPrint(1, "FSDrvLib: start service failed!\n");
			CloseServiceHandle(schService);
		}

		if (dwErr != 0)
		{
			DbgPrint(1, "FSDrvLib: Start existing service failed... try register (%s From %s)...\n", Name, Path);
			if ((schService = CreateService(SchSCManager, Name, Name, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, Path, NULL, NULL, NULL, NULL, NULL))
				|| ((dwErr = GetLastError()) == ERROR_SERVICE_EXISTS))
			{
				if (schService != NULL)
					CloseServiceHandle(schService);
				if (schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS))
				{
					if ((StartService(schService, 0, NULL)) || ((dwErr = GetLastError()) == ERROR_SERVICE_ALREADY_RUNNING))
						dwErr = 0;
					else
					{
						DbgPrint(1, "FSDrvLib: start service failed!\n");
					}
					CloseServiceHandle(schService);
				}
				else
				{
					DbgPrint(1, "FSDrvLib: open service failed!\n");
				}
			}
			else
			{
				DbgPrint(1, "FSDrvLib: register service failed!\n");
			}
		}
	}
	else
		dwErr = GetLastError();

	if (SchSCManager)
	{
		DbgPrint(1, "FSDrvLib: CloseSCManager\n");
		CloseServiceHandle(SchSCManager);
	}
	
	if (hSyncMutex != NULL)
	{
		ReleaseMutex(hSyncMutex);
	}
	return dwErr;
}

DWORD UnloadDeviceDriver(const char * Name)
{
	SC_HANDLE	SchSCManager;
	SC_HANDLE	schService;
	SERVICE_STATUS  serviceStatus;
	DWORD	dwErr;
	if (SchSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))
	{
		if (schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS))
		{
			dwErr = ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);
			CloseServiceHandle(schService);
			if (dwErr)
			{
#ifdef UNREGISTER_DRIVER
				if (schService = OpenService(SchSCManager, Name, SERVICE_ALL_ACCESS))
				{
					dwErr = DeleteService(schService);
					CloseServiceHandle(schService);
					if (dwErr)
					{
#endif //UNREGISTER_DRIVER
						CloseServiceHandle(SchSCManager);
						return 0;
#ifdef UNREGISTER_DRIVER
					}
				}
#endif //UNREGISTER_DRIVER
			}
		}
		CloseServiceHandle(SchSCManager);
	}
	else 	
	{
		if (GetLastError() == ERROR_ACCESS_DENIED)
			return 0;
	}
	dwErr = GetLastError();
	
	return dwErr;
}

//+ ----------------------------------------------------------------------------------------
BOOL IDriverInitDirectCallIO(HANDLE hDevice, OSVERSIONINFO* pOSVer)
{
	ADDITIONALSTATE AddState;
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
		return TRUE;

	if (IDriverGetAdditionalState(hDevice, &AddState) == FALSE)
	{
		DbgPrint(2, "IDriver: InitDirectCallIO failed - can't do IDriverGetAdditionalState\n");
		return FALSE;
	}
	
	gIDriverDirectCall.m_DrvFlags = AddState.DrvFlags;
	gIDriverDirectCall.m_DirectSysCallOffset = AddState.DirectSysCallStartId;
	
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (IDriverInitDirectCalls(pOSVer))
		{
			DbgPrint(2, "IDriver: InitDirectCallIO successfully inited. DirectSysCallOffset %#x (%u)\n", gIDriverDirectCall.m_DirectSysCallOffset, gIDriverDirectCall.m_DirectSysCallOffset);
			return TRUE;
		}
	}

	DbgPrint(2, "IDriver: InitDirectCallIO failed - not supported\n");
	return FALSE;
}

BOOL IDriverGetVersion(HANDLE hDevice, DWORD* pDwBuildNum)
{
	DWORD BytesRet = 0;
	if (pDwBuildNum == NULL)
		return FALSE;
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		*pDwBuildNum = gpSysCall0((AVPG_W32_Services)(_W32ServGetVersion + gIDriverDirectCall.m_DirectSysCallOffset));
		return TRUE;
	}

	return DeviceIoControl(hDevice, IOCTLHOOK_GetVersion, NULL, 0, pDwBuildNum, sizeof(DWORD), &BytesRet, NULL);
}

BOOL IDriverClientActivity(HANDLE hDevice, PHDSTATE pActivityIn, PHDSTATE pActivityOut)
{
	DWORD BytesRet = 0;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (gpSysCall2((AVPG_W32_Services)(_W32ServActivity + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) pActivityIn, (DWORD) pActivityOut) == STATUS_SUCCESS)
			return TRUE;

		return FALSE;
	}

	if (!DeviceIoControl(hDevice, IOCTLHOOK_Activity, pActivityIn, sizeof(HDSTATE), pActivityOut, sizeof(HDSTATE), &BytesRet, NULL))
		return FALSE;

	return TRUE;
}

//+ ----------------------------------------------------------------------------------------


HANDLE RegisterAppInDrv(APP_REGISTRATION* pAppReg, OSVERSIONINFO* pOSVer, HANDLE* phWhistleup, HANDLE* phWFChanged, char* pWhistleupName, char* pChangedName, PCHAR pDrvName, PCLIENT_INFO pClientInfo)
{
	HANDLE			hDevice = INVALID_HANDLE_VALUE;
	HKEY			hKey	= NULL;
	char*			DriverExt;
	char			DrvStr[] = "\\\\.\\%s";
	char			Tmp[256];
	char			Tmp1[1024];
	char			pDeviceName[1024];
	char*			pTmp;
	DWORD			dwErr = 0;
	DWORD			BuildNum, BytesRet;

	DWORD KeyLenght;
	DWORD TypeStr;
	
	pAppReg->m_InterfaceVersion = HOOK_INTERFACE_NUM;
	
	switch (pOSVer->dwPlatformId)
	{
		case VER_PLATFORM_WIN32s:
			DbgPrint(1, "FSDrvLib: Wrong OS platform!\n");
			return hDevice;
		case VER_PLATFORM_WIN32_WINDOWS:
			DriverExt = _VxdExt;
			break;
		case VER_PLATFORM_WIN32_NT:
			EnableLoadDriverPrivilege();
			DriverExt = _SysExt;
			break;
	}

	wsprintf(pDeviceName, DrvStr, pDrvName);

	CAutoHSA _hsa;

	if(!(*phWhistleup = CreateEvent(_hsa.Get(), FALSE, FALSE, pWhistleupName)) || 
		(!(*phWFChanged = CreateEvent(_hsa.Get(), FALSE, FALSE, pChangedName)))) 
	{
		if (*phWhistleup != NULL)
			CloseHandle(*phWhistleup);
		if (*phWFChanged != NULL)
			CloseHandle(*phWFChanged);
		*phWhistleup = NULL;
		*phWFChanged = NULL;
		DbgPrint(1, "FSDrvLib: Create events for transmit data failed!\n");
		return hDevice;
	}

	pAppReg->m_SyncAddr = 0;
	if(pOSVer->dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		lstrcpy(pAppReg->m_WhistleUp, pWhistleupName);
		lstrcpy(pAppReg->m_WFChanged, pChangedName);
	} 
	else
	{
		HINSTANCE hK32 = NULL;
		static HANDLE (WINAPI *pOpenVxDHandle)(HANDLE) = 0;

		if(!(hK32=LoadLibrary("kernel32.dll")) || 
			!(pOpenVxDHandle=(HANDLE(WINAPI *)(HANDLE))GetProcAddress(hK32,"OpenVxDHandle")) ||
			!(*(VOID**)pAppReg->m_WhistleUp=pOpenVxDHandle(*phWhistleup)) ||
			!(*(VOID**)pAppReg->m_WFChanged=pOpenVxDHandle(*phWFChanged)))
		{
			DbgPrint(1, "FSDrvLib: Create events for transmit data failed!\n");
			CloseHandle(*phWhistleup);
			CloseHandle(*phWFChanged);			
			*phWhistleup = NULL;
			*phWFChanged = NULL;
			if (hK32)
				FreeLibrary(hK32);
			return hDevice; 
		} 
		else
		{
			static void (WINAPI *pGetWin16Mutex)(DWORD*) = (void (WINAPI *)(DWORD*)) _IDriver_GetProcAddress(hK32, 93);
			if (pGetWin16Mutex != NULL)
				pGetWin16Mutex(&pAppReg->m_SyncAddr);
			else 
			{
				DbgPrint(1, "FSDrvLib: Getting Win16 mutex failed!\n");
			}

			gpSysCall0 = (_SysCall0) _IDriver_GetProcAddress(hK32, 1);
		}
		if (hK32)
			FreeLibrary(hK32);
	}

	if (pOSVer->dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		hDevice = GetDriverHandle(pDeviceName);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			if (!SearchPath(NULL, pDrvName, DriverExt, sizeof(Tmp1), Tmp1, &pTmp))
			{
				DbgPrint(1, "FSDrvLib: SearchPath failed. try to get path form drvreg record\n");

				Tmp1[0] = 0;
				if (_gExpandEnvironmentStrings != NULL)
				{
					if (_gExpandEnvironmentStrings("%SystemRoot%\\system32\\drivers\\klif.sys", Tmp1, sizeof(Tmp1)))
					{
						if (-1 == GetFileAttributes(Tmp1))
						{
							DbgPrint(1, "FSDrvLib: _gExpandEnvironmentStrings ... file not found %s\n", Tmp1);
							Tmp1[0] = 0;
						}
					}
					else
					{
						DbgPrint(1, "FSDrvLib: _gExpandEnvironmentStrings failed\n");
					}
				}
				else
				{
					DbgPrint(1, "FSDrvLib: _gExpandEnvironmentStrings is null\n");
				}

				if (Tmp1[0] == 0)
				{
					DbgPrint(1, "FSDrvLib: open key for get path\n");
					if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\"AVPGNAME, 0, KEY_READ, &hKey))
					{
						KeyLenght = sizeof(Tmp1);
						TypeStr = REG_SZ;
						
						if (RegQueryValueEx(hKey, "ImagePath", 0, &TypeStr, (LPBYTE) Tmp1, &KeyLenght) == ERROR_SUCCESS)
						{
							if (lstrlen(Tmp1) > 4)
							{
								if (Tmp1[1] == '?')
									lstrcpy(Tmp1, Tmp1 + 4);
							}
						}
						RegCloseKey(hKey);
					}
					else
					{
						DbgPrint(1, "FSDrvLib: Driver not registered and not found!\n", Tmp1);
					}
				}
			}
			else
			{
				ReparseDrvPath(Tmp1, sizeof(Tmp1));
				DbgPrint(1, "FSDrvLib: SearchPath '%s'\n", Tmp1);
			}
			if (Tmp1[0] != 0)
			{
				if (GetShortPathName(Tmp1, Tmp, sizeof(Tmp)) == 0)
				{
					DbgPrint(1, "FSDrvLib: get short name failed for '%s'! ignoring...\n", Tmp1);
					lstrcpy(Tmp, Tmp1);
				}
				
				{
					CharToOem(Tmp, Tmp);
					wsprintf(Tmp1, DrvStr, pDrvName);

					LoadDeviceDriver(pDrvName, Tmp);
					hDevice = GetDriverHandle(pDeviceName);
				}
			}
		}
	}
	else //9x
	{
		wsprintf(Tmp1, DrvStr, pDrvName);
		if ((hDevice = CreateFile(Tmp1, 0, 0, NULL, 0, 0, NULL)) == INVALID_HANDLE_VALUE)
		{
			DbgPrint(1, "FSDrvLib: open hDevice on %s failed!\n", Tmp1);
			dwErr = GetLastError();
		}
	}

	if (hDevice != INVALID_HANDLE_VALUE)
	{
		IDriverInitDirectCallIO(hDevice, pOSVer);

		if ((IDriverGetVersion(hDevice, &BuildNum) == FALSE) || (BuildNum < HOOK_INTERFACE_NUM))
		{
			// Error - get version failed
			dwErr = ERROR_BAD_ENVIRONMENT;
			DbgPrint(1, "FSDrvLib: The driver has wrong version! Current %d Required %d\n", BuildNum, HOOK_INTERFACE_NUM );
			CloseHandle(hDevice);
			hDevice = INVALID_HANDLE_VALUE;
		}
		else
		{
			if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_MINIFILTER)
			{
				if (pOSVer->dwPlatformId == VER_PLATFORM_WIN32_NT)
				{
					pAppReg->m_WhistleUp[0] = 0;
					pAppReg->m_WhistleUp[5] = 0;
					*((ULONG*)(pAppReg->m_WhistleUp + 1)) = (ULONG) *phWhistleup;

					pAppReg->m_WFChanged[0] = 0;
					pAppReg->m_WFChanged[5] = 0;
					*((ULONG*)(pAppReg->m_WFChanged + 1)) = (ULONG) *phWFChanged;
				}
			}

			if(!DeviceIoControl(hDevice, IOCTLHOOK_RegistrApp, pAppReg, sizeof(APP_REGISTRATION), pClientInfo, sizeof(CLIENT_INFO), &BytesRet, NULL))
			{
				// Error - not registered
				dwErr = ERROR_INVALID_ACCESS;
				DbgPrint(1, "FSDrvLib: Error - client not registered!\n");
				CloseHandle(hDevice);
				hDevice = INVALID_HANDLE_VALUE;
			}
			else
			{
				DbgPrint(1, "FSDrvLib: Client %d registered!\n", pClientInfo->m_AppID);
			}
		}
	}

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		CloseHandle(*phWhistleup);
		CloseHandle(*phWFChanged);
		*phWhistleup = NULL;
		*phWFChanged = NULL;
	}
	return hDevice;
}

HANDLE RegisterApp(APP_REGISTRATION* pAppReg, OSVERSIONINFO* pOSVer, HANDLE* phWhistleup, HANDLE* phWFChanged, char* pWhistleupName, char* pChangedName)
{
	CLIENT_INFO ClientInfo;
	HANDLE hDevice = RegisterAppInDrv(pAppReg, pOSVer, phWhistleup, phWFChanged, pWhistleupName, pChangedName, _APPNAME, &ClientInfo);
	pAppReg->m_AppID = ClientInfo.m_AppID;

	return hDevice;
}

HANDLE RegisterAppEx(APP_REGISTRATION* pAppReg, OSVERSIONINFO* pOSVer, HANDLE* phWhistleup, HANDLE* phWFChanged, char* pWhistleupName, char* pChangedName, PCLIENT_INFO pClientInfo)
{
	return RegisterAppInDrv(pAppReg, pOSVer, phWhistleup, phWFChanged, pWhistleupName, pChangedName, _APPNAME, pClientInfo);
}

DWORD UnRegisterApp(HANDLE hDevice, APP_REGISTRATION* pAppReg, BOOL bStayResident,  OSVERSIONINFO* pOSVer)
{
	DWORD			dwErr = ERROR_INVALID_HANDLE;
	HDSTATE			Activity;
	DWORD			BytesRet;
	
	if ((hDevice == INVALID_HANDLE_VALUE) || (pAppReg->m_AppID == _SYSTEM_APPLICATION))
	{
		DbgPrint(1, "invalid parameters\n");
		return dwErr; 
	}
	
	Activity.Activity = _AS_Unload;
	if ((!(pAppReg->m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE)) && bStayResident == TRUE)
		Activity.Activity = _AS_UnRegisterStayResident;
	
	Activity.AppID = pAppReg->m_AppID;
	
	if (!DeviceIoControl(hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
		dwErr = 0x80000046;//errPARAMETER_INVALID;
	else
		dwErr = 0x80000000;
	
	CloseHandle(hDevice);
	
	if(pOSVer->dwPlatformId == VER_PLATFORM_WIN32_NT)
		UnloadDeviceDriver(_APPNAME);
		
	return dwErr;
}

BOOL IDriverUnregisterApp(HANDLE hDevice, ULONG AppID, BOOL bStayResident)
{
	BOOL bRet = FALSE;
	HDSTATE			Activity;
	DWORD			BytesRet;
	
	if ((hDevice == INVALID_HANDLE_VALUE) || (AppID == _SYSTEM_APPLICATION))
	{
		DbgPrint(1, "invalid parameters\n");
		return bRet; 
	}
	
	Activity.Activity = _AS_Unload;
	if (bStayResident == TRUE)
		Activity.Activity = _AS_UnRegisterStayResident;
	
	Activity.AppID = AppID;
	
	if (DeviceIoControl(hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
		bRet = TRUE;
	
	CloseHandle(hDevice);
	
	return bRet;
}

BOOL IDriverChangeDebugInfoLevel(HANDLE hDevice, ULONG DebugInfo)
{
	DWORD BytesRet;
	if (!DeviceIoControl(hDevice, IOCTLHOOK_NewDebugInfoLevel, &DebugInfo, sizeof(DebugInfo), NULL, 0, &BytesRet, NULL))
		return FALSE;
	return TRUE;
}

BOOL IDriverChangeDebugInfoMask(HANDLE hDevice, ULONG DebugMask)
{
	DWORD BytesRet;
	if (!DeviceIoControl(hDevice, IOCTLHOOK_NewDebugCategoryMask, &DebugMask, sizeof(DebugMask), NULL, 0, &BytesRet, NULL))
		return FALSE;
	return TRUE;
}

BOOL IDriverGetDebugInfoLevel(HANDLE hDevice, ULONG* pdwDebugInfo)
{
	DWORD BytesRet;
	if (!DeviceIoControl(hDevice, IOCTLHOOK_NewDebugInfoLevel, NULL, 0, pdwDebugInfo, sizeof(*pdwDebugInfo), &BytesRet, NULL))
		return FALSE;
	return TRUE;
}

BOOL IDriverGetDebugInfoMask(HANDLE hDevice, ULONG* pdwDebugMask)
{
	DWORD BytesRet;
	if (!DeviceIoControl(hDevice, IOCTLHOOK_NewDebugCategoryMask, NULL, 0, pdwDebugMask, sizeof(*pdwDebugMask), &BytesRet, NULL))
		return FALSE;
	return TRUE;
}

BOOL IDriverSaveFilters(HANDLE hDevice, DWORD AppID)
{
	HDSTATE Activity;
	DWORD BytesRet;
	
	Activity.Activity = _AS_SaveFilters;
	Activity.AppID = AppID;
	if (!DeviceIoControl (hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
		return FALSE;
	
	return TRUE;
	
}

BOOL IDriverDeleteFilter(HANDLE hDevice, DWORD AppID, ULONG FilterId)
{
	FILTER_TRANSMIT _Filter;

	_Filter.m_AppID = AppID;
	_Filter.m_FltCtl = FLTCTL_DEL;
	_Filter.m_FilterID = FilterId;

	return IDriverFilterTransmit(hDevice, &_Filter, &_Filter, sizeof(_Filter), sizeof(_Filter));
}

BOOL IDriverResetFilters(HANDLE hDevice, DWORD AppID)
{
	FILTER_TRANSMIT Filter;
	Filter.m_AppID = AppID;
	Filter.m_FltCtl = FLTCTL_RESET_FILTERS;

	return IDriverFilterTransmit(hDevice, &Filter, &Filter, sizeof(Filter), sizeof(Filter));
}

BOOL AddFSFilter(HANDLE hDevice, DWORD AppID, LPSTR szFileName, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, 
				 DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite)
{
	BYTE FilterData[sizeof(FILTER_TRANSMIT) + sizeof(FILTER_PARAM) + MAXPATHLEN + 1];
	
	PFILTER_TRANSMIT pFilter = (PFILTER_TRANSMIT) FilterData;

	PFILTER_PARAM pParam = (PFILTER_PARAM) pFilter->m_Params;
	
	DWORD BytesRet = 0;

	pFilter->m_AppID = AppID;
	if (pAdditionSite == NULL)
		pFilter->m_FltCtl = FLTCTL_ADD;
	else
	{
		if (*pAdditionSite == 0)
			pFilter->m_FltCtl = FLTCTL_ADD2TOP;
		else
		{
			pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
			pFilter->m_FilterID = *pAdditionSite;
		}
	}

	lstrcpyn(pFilter->m_ProcName, szProcessName, PROCNAMELEN);
	pFilter->m_Timeout = dwTimeout;
	pFilter->m_Flags = dwFlags;
	pFilter->m_HookID = HookID;
	pFilter->m_FunctionMj = FunctionMj;
	pFilter->m_FunctionMi = FunctionMi;
	pFilter->m_ProcessingType = ProcessingType;
	pFilter->m_Expiration = ExpireTime;

	pFilter->m_ParamsCount = 1;

	pParam->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG;
	pParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	pParam->m_ParamID = _PARAM_OBJECT_URL;
	
	if (lstrlen(szFileName) == 0)
	{
		pParam->m_ParamSize = lstrlen("*") + 1;
		lstrcpyn((PCHAR) pParam->m_ParamValue, "*", MAXPATHLEN);
	}
	else
	{
		pParam->m_ParamSize = lstrlen(szFileName) + 1;
		lstrcpyn((PCHAR) pParam->m_ParamValue, szFileName, MAXPATHLEN);
	}

	if (DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pFilter, sizeof(FilterData), pFilter, sizeof(FilterData), &BytesRet, NULL))
	{
		DbgPrint(1, "Filter %d for app %d added \n", pFilter->m_FilterID, pFilter->m_AppID);
		return TRUE;
	}

	return FALSE;
}

BOOL AddFSFilterW(HANDLE hDevice, DWORD AppID, PWCHAR szFileName, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, 
					  DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite)
{
	BOOL bRet = FALSE;

	if (_plstrcpyW == NULL || _plstrlenW == NULL)
		return bRet;

	ULONG ReqBufferSize = sizeof(FILTER_TRANSMIT) + sizeof(FILTER_PARAM);
	BYTE* pFilterData;

	if (_plstrlenW(szFileName) == 0)
		return bRet;

	ReqBufferSize += (_plstrlenW(szFileName) + 1) * sizeof(WCHAR);
	pFilterData = (BYTE*) HeapAlloc(GetProcessHeap(), 0, ReqBufferSize);
	if (pFilterData == NULL)
		return bRet;

#ifdef _DEBUG
	ZeroMemory(pFilterData, ReqBufferSize);
#endif
	
	PFILTER_TRANSMIT pFilter = (PFILTER_TRANSMIT) pFilterData;
	
	PFILTER_PARAM pParam = (PFILTER_PARAM) pFilter->m_Params;
	
	DWORD BytesRet = 0;
	
	pFilter->m_AppID = AppID;
	if (pAdditionSite == NULL)
		pFilter->m_FltCtl = FLTCTL_ADD;
	else
	{
		if (*pAdditionSite == 0)
			pFilter->m_FltCtl = FLTCTL_ADD2TOP;
		else
		{
			pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
			pFilter->m_FilterID = *pAdditionSite;
		}
	}
	
	lstrcpyn(pFilter->m_ProcName, szProcessName, PROCNAMELEN);
	if (lstrcmp(pFilter->m_ProcName, "*") == 0)
		pFilter->m_ProcName[0] = 0;

	pFilter->m_Timeout = dwTimeout;
	pFilter->m_Flags = dwFlags;
	pFilter->m_HookID = HookID;
	pFilter->m_FunctionMj = FunctionMj;
	pFilter->m_FunctionMi = FunctionMi;
	pFilter->m_ProcessingType = ProcessingType;
	pFilter->m_Expiration = ExpireTime;
	
	pFilter->m_ParamsCount = 1;
	
	pParam->m_ParamFlags = _FILTER_PARAM_FLAG_CACHABLE | _FILTER_PARAM_FLAG_CACHEUPREG;
	pParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	pParam->m_ParamID = _PARAM_OBJECT_URL_W;
	
	if (_plstrlenW != NULL)
	{
		if (_plstrcmpW(szFileName, L"*") == 0)
			pParam->m_ParamFlt = FLT_PARAM_NOP;
	}

	pParam->m_ParamSize = (_plstrlenW(szFileName) + 1) * sizeof(WCHAR);
	_plstrcpyW((PWCHAR) pParam->m_ParamValue, szFileName);
	
	if (DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pFilter, ReqBufferSize, pFilter, ReqBufferSize, &BytesRet, NULL))
	{
		DbgPrint(1, "Filter %d for app %d added \n", pFilter->m_FilterID, pFilter->m_AppID);
		bRet = TRUE;
	}

	HeapFree(GetProcessHeap(), 0, pFilterData);
	
	return bRet;
}

ULONG AddFSFilter2(HANDLE hDevice, DWORD AppID, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite, PFILTER_PARAM pFirstParam, ...)
{
	ULONG FilterID = 0;

	va_list ArgPtr;
	BYTE* pFilterData;
	PFILTER_PARAM pNextParam = pFirstParam;
	DWORD PackeSize = sizeof(FILTER_TRANSMIT);
	int ParamCount = 0;

	DWORD BytesRet;

	va_start(ArgPtr, pFirstParam);
	while(pNextParam != NULL)
	{
		PackeSize += sizeof(FILTER_PARAM) + pNextParam->m_ParamSize;
		pNextParam = va_arg(ArgPtr, PFILTER_PARAM);
		ParamCount++;
	}
	va_end(ArgPtr);

	pFilterData = (BYTE*) HeapAlloc(GetProcessHeap(), 0, PackeSize);
	if (pFilterData != NULL)
	{
		PFILTER_TRANSMIT pFilter = (PFILTER_TRANSMIT) pFilterData;
		PFILTER_PARAM pParam = (PFILTER_PARAM) pFilter->m_Params;
		pNextParam = pFirstParam;

		va_start(ArgPtr, pFirstParam);
		while(pNextParam != NULL)
		{
			memcpy(pParam, pNextParam, sizeof(FILTER_PARAM) + pNextParam->m_ParamSize);
			pParam = (PFILTER_PARAM)((BYTE*)pParam + sizeof(FILTER_PARAM) + pNextParam->m_ParamSize);

			pNextParam = va_arg(ArgPtr, PFILTER_PARAM);
		}
		va_end(ArgPtr);

		pFilter->m_AppID = AppID;
		if (szProcessName)
			lstrcpyn(pFilter->m_ProcName, szProcessName, PROCNAMELEN);
		else
			pFilter->m_ProcName[0] = 0;

		pFilter->m_Timeout = dwTimeout;
		pFilter->m_Flags = dwFlags;
		pFilter->m_HookID = HookID;
		pFilter->m_FunctionMj = FunctionMj;
		pFilter->m_FunctionMi = FunctionMi;
		pFilter->m_ProcessingType = ProcessingType;
		pFilter->m_Expiration = ExpireTime;

		pFilter->m_ParamsCount = ParamCount;

		if (pAdditionSite == NULL)
			pFilter->m_FltCtl = FLTCTL_ADD;
		else
		{
			if (*pAdditionSite == 0)
				pFilter->m_FltCtl = FLTCTL_ADD2TOP;
			else
			{
				pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
				pFilter->m_FilterID = *pAdditionSite;
			}
			
		}
		
		if (DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pFilter, PackeSize, pFilter, PackeSize, &BytesRet, NULL))
		{
			DbgPrint(1, "Filter2 %d for app %d added \n", pFilter->m_FilterID, pFilter->m_AppID);
			FilterID = pFilter->m_FilterID;
		}
		HeapFree(GetProcessHeap(), 0, pFilterData);
	}
	
	return FilterID;
}

ULONG AddFSFilter3(HANDLE hDevice, DWORD AppID, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite, PFILTER_PARAM* pParamArray, ULONG ParamsCount)
{
	ULONG FilterID = 0;

	BYTE* pFilterData;
	DWORD PackeSize = sizeof(FILTER_TRANSMIT);

	DWORD BytesRet;

	for (ULONG cou = 0; cou < ParamsCount; cou++ )
		PackeSize += sizeof(FILTER_PARAM) + pParamArray[cou]->m_ParamSize;
	
	pFilterData = (BYTE*) HeapAlloc(GetProcessHeap(), 0, PackeSize);
	if (pFilterData != NULL)
	{
		PFILTER_TRANSMIT pFilter = (PFILTER_TRANSMIT) pFilterData;
		PFILTER_PARAM pParam = (PFILTER_PARAM) pFilter->m_Params;

		for (ULONG cou = 0; cou < ParamsCount; cou++ )
		{
			memcpy(pParam, pParamArray[cou], sizeof(FILTER_PARAM) + pParamArray[cou]->m_ParamSize);
			pParam = (PFILTER_PARAM)((BYTE*)pParam + sizeof(FILTER_PARAM) + pParamArray[cou]->m_ParamSize);
		}

		pFilter->m_AppID = AppID;
		if (szProcessName)
			lstrcpyn(pFilter->m_ProcName, szProcessName, PROCNAMELEN);
		else
			pFilter->m_ProcName[0] = 0;

		pFilter->m_Timeout = dwTimeout;
		pFilter->m_Flags = dwFlags;
		pFilter->m_HookID = HookID;
		pFilter->m_FunctionMj = FunctionMj;
		pFilter->m_FunctionMi = FunctionMi;
		pFilter->m_ProcessingType = ProcessingType;
		pFilter->m_Expiration = ExpireTime;

		pFilter->m_ParamsCount = ParamsCount;

		if (pAdditionSite == NULL)
			pFilter->m_FltCtl = FLTCTL_ADD;
		else
		{
			if (*pAdditionSite == 0)
				pFilter->m_FltCtl = FLTCTL_ADD2TOP;
			else
			{
				pFilter->m_FltCtl = FLTCTL_ADD2POSITION;
				pFilter->m_FilterID = *pAdditionSite;
			}
			
		}
		
		if (DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pFilter, PackeSize, pFilter, PackeSize, &BytesRet, NULL))
		{
			DbgPrint(1, "Filter2 %d for app %d added \n", pFilter->m_FilterID, pFilter->m_AppID);
			FilterID = pFilter->m_FilterID;
		}
		HeapFree(GetProcessHeap(), 0, pFilterData);
	}
	
	return FilterID;
}
// -----------------------------------------------------------------------------------------

DWORD IDriverInvisibleTransmit(HANDLE hDevice, PINVISIBLE_TRANSMIT pInvTransmitIn, PINVISIBLE_TRANSMIT pInvTransmitOut)
{
	DWORD BytesRet;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (gpSysCall2((AVPG_W32_Services)(_W32ServInvisibleTransmit + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) pInvTransmitIn, (DWORD) pInvTransmitOut) == STATUS_SUCCESS)
			return pInvTransmitOut->m_ThreadID;
	}
	else
	{
		if(DeviceIoControl(hDevice, IOCTLHOOK_InvisibleThreadOperations, pInvTransmitIn, sizeof(INVISIBLE_TRANSMIT), pInvTransmitOut, sizeof(INVISIBLE_TRANSMIT), &BytesRet, NULL))
			return pInvTransmitOut->m_ThreadID;
	}
	
	return 0;
}

DWORD IDriverRegisterInvisibleThread(HANDLE hDevice, ULONG uAppID)
{
	INVISIBLE_TRANSMIT InvTransmit;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	InvTransmit.m_AppID = uAppID;
	InvTransmit.m_InvCtl = INVCTL_ADD;

	return IDriverInvisibleTransmit(hDevice, &InvTransmit, &InvTransmit);
}

DWORD IDriverUnregisterInvisibleThread(HANDLE hDevice, ULONG uAppID)
{
	INVISIBLE_TRANSMIT InvTransmit;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	InvTransmit.m_AppID = uAppID;
	InvTransmit.m_InvCtl = INVCTL_DEL;

	return IDriverInvisibleTransmit(hDevice, &InvTransmit, &InvTransmit);
}

DWORD IDriverRegisterInvisibleThreadByID(HANDLE hDevice, ULONG uAppID, DWORD ThreadID)
{
	INVISIBLE_TRANSMIT InvTransmit;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	InvTransmit.m_AppID = uAppID;
	InvTransmit.m_InvCtl = INVCTL_ADDBYID;
	InvTransmit.m_ThreadID = ThreadID;

	return IDriverInvisibleTransmit(hDevice, &InvTransmit, &InvTransmit);
}


DWORD IDriverUnregisterInvisibleThreadByID(HANDLE hDevice, ULONG uAppID, DWORD ThreadID)
{
	INVISIBLE_TRANSMIT InvTransmit;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	InvTransmit.m_AppID = uAppID;
	InvTransmit.m_InvCtl = INVCTL_DELBYID;
	InvTransmit.m_ThreadID = ThreadID;

	return IDriverInvisibleTransmit(hDevice, &InvTransmit, &InvTransmit);
}

DWORD IDriverIsInvisibleThread(HANDLE hDevice, ULONG uAppID, DWORD ThreadID)
{
	INVISIBLE_TRANSMIT InvTransmit;

	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	InvTransmit.m_AppID = uAppID;
	InvTransmit.m_InvCtl = INVCTL_GETCOUNTER;
	InvTransmit.m_ThreadID = ThreadID;

	DWORD BytesRet;

	if(DeviceIoControl(hDevice, IOCTLHOOK_InvisibleThreadOperations, 
		&InvTransmit, sizeof(INVISIBLE_TRANSMIT),
		&InvTransmit, sizeof(INVISIBLE_TRANSMIT),
		&BytesRet, NULL))
	{
		return InvTransmit.m_InvCtl;
	}

	return 0;
}

BOOL IDriverState(HANDLE hDevice, ULONG uAppID, APPSTATE_REQUEST AppReqState, PAPPSTATE pCurrentState)
{
	HDSTATE Activity;

	Activity.Activity = AppReqState;	//_AS_DontChange
	Activity.AppID = uAppID;

	if (!IDriverClientActivity(hDevice, &Activity, &Activity))
		return FALSE;
	
	if (pCurrentState != NULL)
		*pCurrentState = (APPSTATE) Activity.Activity;
	return TRUE;
}

BOOL IDriverGetState(HANDLE hDevice, ULONG uAppID, PHDSTATE pActivity)
{
	pActivity->Activity = _AS_DontChange;
	pActivity->AppID = uAppID;
	
	return IDriverClientActivity(hDevice, pActivity, pActivity);
}

BOOL IDriverYieldEvent(HANDLE hDevice, ULONG AppID, ULONG Mark, ULONG Timeout)
{
	YELD_EVENT Yeld;
	DWORD BytesRet;
	
	Yeld.m_AppID = AppID;
	Yeld.Mark = Mark;
	Yeld.Timeout = Timeout;
	Yeld.YeldParameterSize = 0;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		 if (gpSysCall1((AVPG_W32_Services)(_W32ServYieldEvent + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) &Yeld) == STATUS_SUCCESS)
			 return TRUE;
		
		 return FALSE;
	}
	
	if (!DeviceIoControl(hDevice, IOCTLHOOK_YeldEvent, &Yeld, sizeof(YELD_EVENT), NULL, 0, &BytesRet, NULL))
		return FALSE;

	return TRUE;
}

BOOL IDriverYieldEventForce(HANDLE hDevice, ULONG AppID, ULONG Mark, ULONG Timeout)
{
	char buffer[sizeof(YELD_EVENT) + sizeof(EXEC_ON_HOLD)];
	PYELD_EVENT pYeld = (PYELD_EVENT) buffer;
	PEXEC_ON_HOLD pHold = (PEXEC_ON_HOLD) pYeld->YeldParams;
	
	DWORD BytesRet;
	
	pYeld->m_AppID = AppID;
	pYeld->Mark = Mark;
	pYeld->Timeout = Timeout;
	pYeld->YeldParameterSize = sizeof(ULONG);
	pHold->m_FunctionID = __he_ForceYeld;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		 if (gpSysCall1((AVPG_W32_Services)(_W32ServYieldEvent + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) pYeld) == STATUS_SUCCESS)
			 return TRUE;
		
		 return FALSE;
	}
	
	if (!DeviceIoControl(hDevice, IOCTLHOOK_YeldEvent, pYeld, sizeof(buffer), NULL, 0, &BytesRet, NULL))
		return FALSE;

	return TRUE;
}
// -----------------------------------------------------------------------------------------
BOOL IDriverCacheClean(HANDLE hDevice, ULONG AppID)
{
	DWORD BytesRet;
	if (!DeviceIoControl(hDevice, IOCTLHOOK_ResetCache, &AppID, sizeof(AppID), NULL, 0, &BytesRet, NULL))
		return FALSE;
	
	return TRUE;
}

BOOL IDriverSetVerdict(HANDLE hDevice, ULONG AppID, VERDICT Verdict, ULONG Mark, ULONG ExpTime)
{
	DWORD BytesRet;
	SET_VERDICT SetVerdict;
	
	SetVerdict.m_AppID = AppID;
	SetVerdict.m_Mark = Mark;
	SetVerdict.m_ExpTime = ExpTime;
	SetVerdict.m_Verdict = Verdict;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (gpSysCall1((AVPG_W32_Services)(_W32ServSetVerdict + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) &SetVerdict) == STATUS_SUCCESS)
			return TRUE;

		return FALSE;
	}
	
	if (!DeviceIoControl(hDevice, IOCTLHOOK_SetVerdict, &SetVerdict, sizeof(SET_VERDICT), NULL, 0, &BytesRet, NULL))
		return FALSE;

	return TRUE;
}

BOOL IDriverSetVerdict2(HANDLE hDevice, PSET_VERDICT pVerdict)
{
	return IDriverSetVerdict(hDevice, pVerdict->m_AppID, pVerdict->m_Verdict, pVerdict->m_Mark, pVerdict->m_ExpTime);
}

void IDriverSkipEvent(HANDLE hDevice, ULONG AppID)
{
	DWORD BytesRet;
	DeviceIoControl(hDevice, IOCTLHOOK_SkipEvent, &AppID, sizeof(AppID), NULL, 0, &BytesRet, NULL);
}

BOOL IDriverIncreaseUnsafe(HANDLE hDevice, ULONG AppID, ULONG* puCurrentUnsafe)
{
	DWORD BytesRet;
	HDSTATE Activity;
	
	Activity.Activity = _AS_IncreaseUnsafe;
	Activity.AppID = AppID;
	if (!DeviceIoControl (hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
		return FALSE;
	
	if (puCurrentUnsafe != NULL)
		*puCurrentUnsafe = Activity.Activity;
	return TRUE;
}

BOOL IDriverDecreaseUnsafe(HANDLE hDevice, ULONG AppID, ULONG* puCurrentUnsafe)
{
	DWORD BytesRet;
	HDSTATE Activity;
	
	Activity.Activity = _AS_DecreaseUnsafe;
	Activity.AppID = AppID;
	if (!DeviceIoControl (hDevice, IOCTLHOOK_Activity, &Activity, sizeof(Activity), &Activity, sizeof(Activity), &BytesRet, NULL))
		return FALSE;
	
	if (puCurrentUnsafe != NULL)
		*puCurrentUnsafe = Activity.Activity;
	return TRUE;
}

// -----------------------------------------------------------------------------------------
// Execution on holded thread
DWORD _HE_GetDriveType(PDRV_EVENT_DESCRIBE pDrvDescr, BYTE drv, DWORD* pVal)
{
	DWORD BytesRet;

	char yeldbuf[64];
	PYELD_EVENT pYeld;
	PEXEC_ON_HOLD pexh;

	*pVal = DRIVE_UNKNOWN;

	pYeld = (PYELD_EVENT) yeldbuf;

	pYeld->m_AppID = pDrvDescr->m_AppID;
	pYeld->Mark = pDrvDescr->m_Mark;
	pYeld->Timeout = 30;
	pYeld->YeldParameterSize = sizeof(yeldbuf) - sizeof(YELD_EVENT);

	pexh = (PEXEC_ON_HOLD) pYeld->YeldParams;
	pexh->m_FunctionID = __he_GetDriveType;
	*((BYTE*)pexh->m_ExecParams) = drv;
	
	return DeviceIoControl(pDrvDescr->m_hDevice, IOCTLHOOK_YeldEvent, pYeld, 64, pVal, sizeof(DWORD), &BytesRet, NULL);
}

BOOL _HE_ImpersonateThread(PDRV_EVENT_DESCRIBE pDrvDescr)
{
	DWORD BytesRet;

	IMPERSONATE_REQUEST Impersonate;
	Impersonate.m_AppID = pDrvDescr->m_AppID;
	Impersonate.m_Mark = pDrvDescr->m_Mark;
	
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (gpSysCall1((AVPG_W32_Services)(_W32ServImpersonateThread + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) &Impersonate) == STATUS_SUCCESS)
			return TRUE;

		return FALSE;
	}

	return DeviceIoControl(pDrvDescr->m_hDevice, IOCTLHOOK_ImpersonateThread, &Impersonate, sizeof(Impersonate), NULL, 0, &BytesRet, NULL);
}

BOOL IDriverGetEvent(HANDLE hDevice, ULONG AppID, ULONG Mark, BYTE* pBuffer, ULONG BufferSize, ULONG* pBytesRet)
{
	BOOL bRet = FALSE;
	GET_EVENT GetEvent;
	ULONG BytesRet = 0;

	GetEvent.m_AppID = AppID;
	GetEvent.Mark = Mark;

	if ((pBuffer == NULL) || (BufferSize == 0) || (pBytesRet == NULL))
		return FALSE;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		BytesRet = BufferSize;
		if (gpSysCall3((AVPG_W32_Services)(_W32ServGetEvent + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) &GetEvent, (DWORD) pBuffer, (DWORD) &BytesRet) == STATUS_SUCCESS)
			bRet = TRUE;
	}
	else
		bRet = DeviceIoControl(hDevice, IOCTLHOOK_GetEvent, &GetEvent, sizeof(GET_EVENT), pBuffer, BufferSize, &BytesRet, NULL);

	if (pBytesRet != NULL)
		*pBytesRet = BytesRet;
	
	return bRet;
}

PSINGLE_PARAM IDriverGetEventParam(PEVENT_TRANSMIT pEvent, DWORD ParamID)
{
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvent->m_Params;
	for (DWORD cou =  0; (cou < pEvent->m_ParamsCount); cou++)
	{
		if (pSingleParam->ParamID == ParamID)
			return pSingleParam;

		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
		else
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	return NULL;
}

PSINGLE_PARAM IDriverGetEventMappedParam(PEVENT_TRANSMIT pEvent, DWORD ParamID)
{
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvent->m_Params;
	for (DWORD cou =  0; (cou < pEvent->m_ParamsCount); cou++)
	{
		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM)
		{
			if (pSingleParam->ParamID == ParamID)
				return pSingleParam;
		}
		
		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
		else
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
	}
	return NULL;
}

DWORD IDriverGetEventSize(PEVENT_TRANSMIT pEvent)
{
	DWORD dwSize = sizeof(EVENT_TRANSMIT);
	PSINGLE_PARAM pSingleParam = (PSINGLE_PARAM) pEvent->m_Params;
	for (DWORD cou =  0; (cou < pEvent->m_ParamsCount); cou++)
	{
		if (pSingleParam->ParamFlags & _SINGLE_PARAM_FLAG_POINTER)
		{
			dwSize += sizeof(SINGLE_PARAM) + sizeof(DWORD);
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + sizeof(DWORD));
		}
		else
		{
			dwSize += sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;
			pSingleParam = (PSINGLE_PARAM)((BYTE*)pSingleParam + sizeof(SINGLE_PARAM) + pSingleParam->ParamSize);
		}
	}
	return dwSize;
}

// -----------------------------------------------------------------------------------------
BOOL IDriverGetUserFromSid(SID* pUserSid, char* pszDomain, DWORD* pdwDomainLen, char* pszUser, DWORD* pdwUserLen)
{
	SID_NAME_USE _SidNameUse;

	if (pUserSid == NULL)
		return FALSE;

	if (((BYTE*)pUserSid)[0] == 0)
		return FALSE;
	
	if (_gLookupAccountSid == NULL)
		return FALSE;
	
	return _gLookupAccountSid(NULL, pUserSid, pszUser, pdwUserLen, pszDomain, pdwDomainLen, &_SidNameUse);
}

BOOL IDriverGetUserFromSidW(SID* pUserSid, WCHAR* pszDomain, DWORD* pdwDomainLen, WCHAR* pszUser, DWORD* pdwUserLen)
{
	SID_NAME_USE _SidNameUse;

	if (pUserSid == NULL)
		return FALSE;

	if (_gLookupAccountSidW == NULL)
		return FALSE;
	
	return _gLookupAccountSidW(NULL, pUserSid, pszUser, pdwUserLen, pszDomain, pdwDomainLen, &_SidNameUse);
}

BOOL IDriverExternalDrvRequest(HANDLE hDevice, PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG* OutRequestSize)
{
	if ((pInRequest == NULL) || (OutRequestSize == NULL))
		return FALSE;

	ULONG Outtmp = 0;
	if (!pOutRequest)
	{
		if ((FLTTYPE_SYSTEM == pInRequest->m_DrvID) && 
			(
			(_AVPG_IOCTL_GET_PREFETCHINFO == pInRequest->m_IOCTL)
			|| (_AVPG_IOCTL_GET_SECTIONLIST == pInRequest->m_IOCTL)
			|| (_AVPG_IOCTL_GET_DRVLIST == pInRequest->m_IOCTL)
			))
		{
			if (!OutRequestSize)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}
			
			pOutRequest = &Outtmp;
			*OutRequestSize = sizeof(Outtmp);
		}
	}

	
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		DWORD err = gpSysCall3((AVPG_W32_Services)(_W32ServExternalDrvRequest + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) pInRequest, (DWORD) pOutRequest, (DWORD) OutRequestSize);
		if (SUCCEEDED(err))
		{
			if (pOutRequest == &Outtmp)
			{
				*OutRequestSize = Outtmp;
				return FALSE;
			}

			return TRUE;
		}

		if (pOutRequest == &Outtmp)
			*OutRequestSize = Outtmp;

		SetLastError(err);	// нужно перевести этот код ошибки в winerror
		
		return FALSE;
	}
	
	if (DeviceIoControl(hDevice, IOCTLHOOK_External_Drv_Request, pInRequest, sizeof(EXTERNAL_DRV_REQUEST) + pInRequest->m_BufferSize, 
		pOutRequest, *OutRequestSize, OutRequestSize, NULL))
	{
		if (pOutRequest == &Outtmp)
		{
			*OutRequestSize = Outtmp;
			return FALSE;
		}

		return TRUE;
	}

	if (pOutRequest == &Outtmp)
		*OutRequestSize = Outtmp;
	
	return FALSE;
}

// -----------------------------------------------------------------------------------------
BOOL IDriverEventObjectRequest(HANDLE hDevice, PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG* OutRequestSize)
{
	if ((pInRequest == NULL) || (OutRequestSize == NULL))
		return FALSE;

	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (gpSysCall3((AVPG_W32_Services)(_W32ServEventRequest + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) pInRequest, (DWORD) pOutRequest, (DWORD) OutRequestSize) == STATUS_SUCCESS)
			return TRUE;

		return FALSE;
	}
			
	
	if (DeviceIoControl(hDevice, IOCTLHOOK_EventObject_Request, pInRequest, sizeof(EVENT_OBJECT_REQUEST) + pInRequest->m_RequestBufferSize, 
		pOutRequest, *OutRequestSize, OutRequestSize, NULL))
		return TRUE;
	
	return FALSE;
}

BOOL IDriverFilterEvent(HANDLE hDevice, PFILTER_EVENT_PARAM pEventParam, BOOL bTry, PVERDICT pVerdict)
{
	if ((pEventParam == NULL) || (pVerdict == NULL))
		return FALSE;
	
	DWORD dwEventSize;
	DWORD dwOutSize = sizeof(VERDICT);
	
	if (bTry)
		pEventParam->ProcessingType = CheckProcessing;
	
	DWORD dwSingleSize;
	PSINGLE_PARAM pSingle = (PSINGLE_PARAM) pEventParam->Params;
	
	dwEventSize = sizeof(FILTER_EVENT_PARAM);
	for (ULONG cou = 0; cou < pEventParam->ParamsCount; cou++)
	{
		dwSingleSize = sizeof(SINGLE_PARAM) + pSingle->ParamSize;
		dwEventSize += dwSingleSize;
		pSingle = (PSINGLE_PARAM) ((BYTE*) pSingle + dwSingleSize);
	}
	
	if (DeviceIoControl(hDevice, IOCTLHOOK_FilterEvent, pEventParam, dwEventSize, pVerdict, dwOutSize, &dwOutSize, NULL))
		return TRUE;
	
	return FALSE;
}

BOOL IDriverGetAdditionalState(HANDLE hDevice, PADDITIONALSTATE pAddState)
{
	DWORD BytesRet = 0;
	if (DeviceIoControl(hDevice, IOCTLHOOK_GetDriverInfo, NULL, 0, pAddState, sizeof(ADDITIONALSTATE), &BytesRet, NULL))
	{
		if (BytesRet != NULL)
			return TRUE;
	}
	
	return FALSE;
}

HANDLE IDriverGetKlifNewDriver(OSVERSIONINFO* pOSVer)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	
	char DrvStr[] = "\\\\.\\klif";
	
	if (pOSVer->dwPlatformId != VER_PLATFORM_WIN32_NT)
		return hDevice;
	
	return GetDriverHandle(DrvStr);
	
}

BOOL IDriverFilterTransmit(HANDLE hDevice, FILTER_TRANSMIT* pInFilter, FILTER_TRANSMIT* pOutFilter, DWORD BufferSizeIn, DWORD BufferSizeOut)
{
	DWORD BytesRet = 0;
	
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		BytesRet = BufferSizeOut;
		if (gpSysCall3((AVPG_W32_Services)(_W32ServFilterTransmit + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) pInFilter, (DWORD) pOutFilter, (DWORD) &BytesRet) == STATUS_SUCCESS)
			return TRUE;
		
		return FALSE;
	}
	
	return DeviceIoControl(hDevice, IOCTLHOOK_FiltersCtl, pInFilter, BufferSizeIn, pOutFilter, BufferSizeOut, &BytesRet, NULL);
}


BOOL IDriverTSPRequest(HANDLE hDevice, PTSPRS_LOCK Req,ORIG_HANDLE *OutOrigHandle)
{
	DWORD BytesRet = 0;
	
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_DIRECTSYSCALL)
	{
		if (gpSysCall2((AVPG_W32_Services)(_W32ServTSPRequest + gIDriverDirectCall.m_DirectSysCallOffset), (DWORD) Req, (DWORD) OutOrigHandle) == STATUS_SUCCESS)
			return TRUE;
		
		return FALSE;
	}
	
	return DeviceIoControl(hDevice,IOCTLHOOK_TSPv2, Req,sizeof(TSPRS_LOCK), OutOrigHandle,sizeof(ORIG_HANDLE), &BytesRet, NULL);
}

PFILTER_PARAM IDriverGetFilterParam(PFILTER_TRANSMIT pFilter, DWORD ParamID)
{
	PFILTER_PARAM pFilterParam = (PFILTER_PARAM) pFilter->m_Params;
	for (DWORD cou =  0; (cou < pFilter->m_ParamsCount); cou++)
	{
		if (pFilterParam->m_ParamID == ParamID)
			return pFilterParam;

		pFilterParam = (PFILTER_PARAM)((BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize);
	}
	
	return NULL;
}

PFILTER_PARAM IDriverGetFilterNextParam(PFILTER_TRANSMIT pFilter, ULONG Index)
{
	PFILTER_PARAM pFilterParam = (PFILTER_PARAM) pFilter->m_Params;
	for (DWORD cou =  0; (cou < pFilter->m_ParamsCount); cou++)
	{
		if (Index == cou)
			return pFilterParam;

		pFilterParam = (PFILTER_PARAM)((BYTE*)pFilterParam + sizeof(FILTER_PARAM) + pFilterParam->m_ParamSize);
	}
	
	return NULL;
}

//+ ------------------------------------------------------------------------------------------
typedef DWORD (FAR WINAPI *_pZwQuerySystemInformation)(IN ULONG, OUT PVOID, IN ULONG, OUT PULONG OPTIONAL);
static _pZwQuerySystemInformation _gpZwQuerySystemInformation = NULL;
#include "../hook/MemManag.h"

BOOL IDriverInitEnumProcFuncs()
{
	BOOL bResult = FALSE;
	BOOL bErr = FALSE;

	HMODULE hModNtDll;
	HANDLE hToken;
	LUID DebugValue;
	TOKEN_PRIVILEGES tkp;

	if (_gpZwQuerySystemInformation != NULL)
		return TRUE;

	if (_plstrlenW == NULL)
		return FALSE;

	hModNtDll = GetModuleHandleA("ntdll.dll");
	if (hModNtDll != NULL)
	{
		if(!(_gpZwQuerySystemInformation = (_pZwQuerySystemInformation) GetProcAddress(hModNtDll, "ZwQuerySystemInformation")))
			bErr = TRUE;
		if (!LookupPrivilegeValueA("", "SeDebugPrivilege", &DebugValue))
			bErr = TRUE;
		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
			bErr = TRUE;
		
		if (bErr == FALSE)
		{
			BOOL bNeedPriv = TRUE;
			DWORD dwLength;
#define _TOKEN_BUFFER	0x500
			char TokenBuffer[_TOKEN_BUFFER];
			TOKEN_PRIVILEGES* ptkp;
			if (GetTokenInformation(hToken, TokenPrivileges, TokenBuffer, _TOKEN_BUFFER, &dwLength) != 0)
			{
				DWORD dwPrivCount = 0;
				ptkp = (TOKEN_PRIVILEGES*) TokenBuffer;
				for (; (dwPrivCount < ptkp->PrivilegeCount - 1) && (bNeedPriv == TRUE); dwPrivCount++)
				{
					if ((ptkp->Privileges[dwPrivCount].Luid.HighPart == DebugValue.HighPart) && (ptkp->Privileges[dwPrivCount].Luid.LowPart == DebugValue.LowPart))
						bNeedPriv = FALSE;
				}
			}
			if (bNeedPriv == TRUE)
			{
				tkp.PrivilegeCount = 1;
				tkp.Privileges[0].Luid = DebugValue;
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL))
					bResult = TRUE;
			}
			else
				bResult = TRUE;
			CloseHandle(hToken);
		}
	}

	return bResult;
}

ULONG __cdecl IDriver_GetAllowedPids(ULONG* pServicesExe, ULONG* pCsrss)
{
	ULONG ProcessId = 0;
	*pServicesExe = 0;
	*pCsrss = 0;
	if (!IDriverInitEnumProcFuncs())
		return ProcessId;

	DWORD dwBuffSize;
	DWORD dwResult;

	CHAR* pInfoBuff = NULL;
	dwBuffSize = 1024*32;
	do
	{
		if(!(pInfoBuff = (CHAR*) HeapAlloc(GetProcessHeap(), 0, dwBuffSize)))
			break;

		dwResult = _gpZwQuerySystemInformation((DWORD)5, pInfoBuff, dwBuffSize, NULL);
		if(dwResult == 0)
			break;
		else
		{
			// STATUS_BUFFER_OVERFLOW || status == STATUS_INFO_LENGTH_MISMATCH
			if(dwResult == 0x80000005L || dwResult == 0xC0000004L)
			{
				HeapFree(GetProcessHeap(), 0, pInfoBuff);
				pInfoBuff = NULL;
				dwBuffSize += 0x2000;
			}
			else
			{
				HeapFree(GetProcessHeap(), 0, pInfoBuff);
				pInfoBuff = NULL;
				break;
			}
		}
	} while(TRUE);
	
	if (pInfoBuff != NULL)
	{
		PROCESS_INFO* pNextProcess = (PROCESS_INFO*) pInfoBuff;
		while (pNextProcess->Len != -1)
		{
			if(pNextProcess->PID != 0)
			{
				if (!ProcessId && _plstrcmpiW(pNextProcess->ProcessName, L"lsass.exe") == 0)
					ProcessId = pNextProcess->PID;

				if (!*pServicesExe && _plstrcmpiW(pNextProcess->ProcessName, L"services.exe") == 0)
					*pServicesExe = pNextProcess->PID;

				if (!*pCsrss && _plstrcmpiW(pNextProcess->ProcessName, L"csrss.exe") == 0)
					*pCsrss = pNextProcess->PID;

				if (ProcessId && *pServicesExe && *pCsrss)
					break;
			}

			if (pNextProcess->Len == 0)
				pNextProcess->Len = -1;
			else
				pNextProcess = (PROCESS_INFO*)((CHAR*)pNextProcess + pNextProcess->Len);
		}

		HeapFree(GetProcessHeap(), 0, pInfoBuff);
	}

	return ProcessId;
}

BOOL __cdecl IDriver_AddAllowedPidFilter(ULONG AppID, HANDLE hDevice, ULONG ProcessId)
{
	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_SPECIAL_THISISPROCID;
	Param1->m_ParamFlt = FLT_PARAM_NOP;
	Param1->m_ParamID = _PARAM_OBJECT_SOURCE_ID;
	*(ULONG*) Param1->m_ParamValue = ProcessId;
	Param1->m_ParamSize = sizeof(ULONG);

	DWORD dwSite = 0;

	return !!AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, &dwSite, Param1, NULL);
}

//+ ------------------------------------------------------------------------------------------

BOOL IDriverProProtRequest(HANDLE hDevice, ULONG AppID, BOOL bSwitch, DWORD Flags)
{
	BOOL bStatus;
	char pFilterBuffer[_BUF_FILTER_SIZE];
	
	PFILTER_TRANSMIT _pFilter = (PFILTER_TRANSMIT) pFilterBuffer;

	_pFilter->m_AppID = AppID;
	_pFilter->m_FltCtl = FLTCTL_FIRST;

	//DbgPrint(2, "ProProRequest for AppID %", AppID);

	if (bSwitch == TRUE)
	{
		BOOL bRet = TRUE;
		BOOL bFound = FALSE;

		if (TRUE == (bStatus = IDriverFilterTransmit(hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE)))
		{	
			PFILTER_PARAM pParam;
			while (bStatus)
			{
				if (NULL != (pParam = IDriverGetFilterParam(_pFilter, _PARAM_USER_FSDRVLIB_PROPROT)))
				{
					bFound = TRUE;
					_pFilter->m_FltCtl = FLTCTL_ENABLE_FILTER;
					IDriverFilterTransmit(hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE);
				}
			
				_pFilter->m_FltCtl = FLTCTL_NEXT;
				bStatus = IDriverFilterTransmit(hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE);
			}
		}

		if (bFound == FALSE)
		{
			DWORD dwFlag = FLT_A_DENY;
			if (Flags & PROPROT_FLAG_NOTIFY)
				dwFlag |= FLT_A_NOTIFY;

			if (VER_PLATFORM_WIN32_NT == g_FSDrvLibOSVer.dwPlatformId)
				bRet = IDriver_AddProtectionFilterNT(hDevice, AppID, Flags);
			else
				bRet = IDriver_AddProtectionFilter9x(hDevice, AppID, Flags);
		}

		if (Flags & PROPROT_FLAG_AUTOACTIVATE)
			IDriverState(hDevice, AppID, _AS_GoActive, NULL);

		return bRet;
	}
	else
	{
		if (FALSE == (bStatus = IDriverFilterTransmit(hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE)))
			return TRUE;
		
		while (bStatus)
		{
			if (IDriverGetFilterParam(_pFilter, _PARAM_USER_FSDRVLIB_PROPROT) != NULL)
			{
				_pFilter->m_FltCtl = FLTCTL_DISABLE_FILTER;
				IDriverFilterTransmit(hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE);
				// disable filter
			}
		
			_pFilter->m_FltCtl = FLTCTL_NEXT;
			bStatus = IDriverFilterTransmit(hDevice, _pFilter, _pFilter, _BUF_FILTER_SIZE, _BUF_FILTER_SIZE);
		}

		return TRUE;
	}
	return FALSE;
}

BOOL IDriverIsTspOk()
{
	if (gIDriverDirectCall.m_DrvFlags & _DRV_FLAG_TSP_OK)
		return TRUE;

	return FALSE;
}

BOOL IDriver_AddProtectionFilterNT(HANDLE hDevice, ULONG AppID, DWORD Flags)
{
	BOOL bRet = TRUE;

	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
	
	BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;
	
	BYTE Buf3[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param3 = (PFILTER_PARAM) Buf3;

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param1->m_ParamFlt = FLT_PARAM_EUA;
	Param1->m_ParamID = _PARAM_OBJECT_DEST_ID;
	*(DWORD*)Param1->m_ParamValue = (DWORD) GetCurrentProcessId();
	Param1->m_ParamSize = sizeof(DWORD);

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MAP_TO_EVENT;
	Param2->m_ParamFlt = FLT_PARAM_NOP;
	Param2->m_ParamID = _PARAM_USER_FSDRVLIB_PROPROT;
	*(DWORD*)Param2->m_ParamValue = PROPROT_FLAG_TERMINATE;
	Param2->m_ParamSize = sizeof(DWORD);

	if (Flags & PROPROT_FLAG_TERMINATE)
	{
		Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
		Param3->m_ParamFlt = FLT_PARAM_EUA;
		Param3->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
		*(DWORD*)Param3->m_ParamValue = -1;
		Param3->m_ParamSize = sizeof(DWORD);
		
		if (!AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param3, Param2, NULL))
			bRet = FALSE;
	}
	else if (Flags & PROPROT_FLAG_OPEN)
	{
		BYTE Buf4[sizeof(FILTER_PARAM) + sizeof(DWORD)];
		PFILTER_PARAM Param4 = (PFILTER_PARAM) Buf4;
		
		ULONG ServicesExePid;
		ULONG CsrssPid;
		ULONG ProcessId = IDriver_GetAllowedPids(&ServicesExePid, &CsrssPid);
		if (ProcessId == 0)
			bRet = FALSE;
		else
		{
			IDriver_AddAllowedPidFilter(AppID, hDevice, ProcessId);
			if ((g_FSDrvLibOSVer.dwMajorVersion == 4) && (g_FSDrvLibOSVer.dwMinorVersion == 0)) // nt4 only
			{
				if (ServicesExePid)
					IDriver_AddAllowedPidFilter(AppID, hDevice, ServicesExePid);
				if (CsrssPid)
					IDriver_AddAllowedPidFilter(AppID, hDevice, CsrssPid);
			}
			
			Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
			Param3->m_ParamFlt = FLT_PARAM_AND;
			Param3->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
			*(DWORD*)Param3->m_ParamValue = _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF;
			Param3->m_ParamSize = sizeof(DWORD);
			
			Param4->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
			Param4->m_ParamFlt = FLT_PARAM_AND;
			Param4->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
			*(DWORD*)Param4->m_ParamValue = ~(SYNCHRONIZE | PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION);
			Param4->m_ParamSize = sizeof(DWORD);
			
			if (!AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
				MJ_SYSTEM_OPEN_PROCESS, 0, 0, PreProcessing, NULL, Param1, Param2, Param3, Param4, NULL))
			{
				bRet = FALSE;
			}
			
			/*
			// creating remote thread
			Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
			Param3->m_ParamFlt = FLT_PARAM_EUA;
			Param3->m_ParamID = _PARAM_OBJECT_CLIENTID2;
			*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
			Param3->m_ParamSize = sizeof(DWORD);
			
			if (!AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
				MJ_SYSTEM_CREATE_REMOTE_THREAD, 0, 0, PreProcessing, NULL, Param3, NULL))
			{
				bRet = FALSE;
			}*/
			
			// query info
			Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
			Param3->m_ParamFlt = FLT_PARAM_EUA;
			Param3->m_ParamID = _PARAM_OBJECT_CLIENTID1;
			*(DWORD*)Param3->m_ParamValue = (DWORD) GetCurrentProcessId();
			Param3->m_ParamSize = sizeof(DWORD);
			
			AddFSFilter2(hDevice, AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DENY, FLTTYPE_SYSTEM, 
				MJ_SYSTEM_QUERY_INFO, 0, 0, PreProcessing, NULL, Param3, NULL);
		}
	}

	return bRet;
}

BOOL IDriver_AddProtectionFilter9x(HANDLE hDevice, ULONG AppID, DWORD Flags)
{
	BOOL bRet = FALSE;
	CHAR ModuleName[MAX_PATH + 1];
	CHAR ProcessName[MAX_PATH + 1];
	DWORD DeadlockTimeout = DEADLOCKWDOG_TIMEOUT;

	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
	
	BYTE Buf2[sizeof(FILTER_PARAM) + PROCNAMELEN + 1];
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;

	BYTE Buf3[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param3 = (PFILTER_PARAM) Buf3;

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	Param1->m_ParamFlt = FLT_PARAM_NOP;
	Param1->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
	*(DWORD*)Param1->m_ParamValue = (DWORD) -1;
	Param1->m_ParamSize = sizeof(DWORD);
	
	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param2->m_ParamFlt = FLT_PARAM_WILDCARD;
	Param2->m_ParamID = _PARAM_OBJECT_URL;	//!!

	Param3->m_ParamFlags = _FILTER_PARAM_FLAG_MAP_TO_EVENT;
	Param3->m_ParamFlt = FLT_PARAM_NOP;
	Param3->m_ParamID = _PARAM_USER_FSDRVLIB_PROPROT;
	*(DWORD*)Param3->m_ParamValue = PROPROT_FLAG_TERMINATE;
	Param3->m_ParamSize = sizeof(DWORD);

	if (GetModuleFileName(NULL, ModuleName, sizeof(ModuleName)) != 0)
	{
		int cou = lstrlen(ModuleName) - 1;
		while ((cou > 0) && (ModuleName[cou] != '\\'))
			cou--;

		if (cou > 0)
			lstrcpy(ProcessName, ModuleName + cou + 1);
		lstrcpy((PCHAR) Param2->m_ParamValue, "*");
		lstrcat((PCHAR) Param2->m_ParamValue, ProcessName);
		Param2->m_ParamSize = lstrlen((PCHAR) Param2->m_ParamValue) + 1;
		
		if (AddFSFilter2(hDevice, AppID, "*", 0, FLT_A_DENY, 
			FLTTYPE_R3, 28/*TerminateProcess*/, 0, 0, PreProcessing, NULL, Param1, Param2, Param3, NULL))
		{
			bRet = TRUE;
		}
	}
	
	return bRet;
}

//+ ------------------------------------------------------------------------------------------
BOOL IDriver_ReloadSettings(ULONG SettingsMask)
{
	BOOL bRet = FALSE;

	HANDLE hDevice = IDriver_GetDriverHandleQuick();
	
	if (INVALID_HANDLE_VALUE == hDevice)
		return FALSE;

	DWORD BytesRet = 0;
	
	bRet = DeviceIoControl(hDevice, IOCTLHOOK_RELOADSETTINGS, &SettingsMask, sizeof(SettingsMask), NULL, 0, &BytesRet, NULL);

	CloseHandle(hDevice);

	return bRet;
}

BOOL IDriver_GetDiskStat(HANDLE hDevice, ULONG AppID, unsigned __int64* pTimeCurrent, unsigned __int64* pTotalWaitTime)
{
	BOOL bRet = FALSE;
	
	BYTE buffer[sizeof(EXTERNAL_DRV_REQUEST)];
	PEXTERNAL_DRV_REQUEST pExtReq = (PEXTERNAL_DRV_REQUEST) buffer;
	unsigned __int64 disk_stat[2];

	pExtReq->m_DrvID = FLTTYPE_SYSTEM;
	pExtReq->m_IOCTL = _AVPG_IOCTL_STAT_DISK;
	pExtReq->m_AppID = AppID;
	pExtReq->m_BufferSize = 0;

	ULONG OutRequestSize = sizeof(disk_stat);

	bRet = IDriverExternalDrvRequest(hDevice, pExtReq, disk_stat, &OutRequestSize);

	if (bRet)
	{
		*pTimeCurrent = disk_stat[0];
		*pTotalWaitTime = disk_stat[1];
	}

	return bRet;
}

BOOL IDriver_FidBox_Set(HANDLE hDevice, ULONG AppID, HANDLE hFile, PVOID pBuffer, ULONG BufferLen)
{
	BOOL bRet = FALSE;
	
	BYTE buffer[0x1000];
	PEXTERNAL_DRV_REQUEST pExtReq = (PEXTERNAL_DRV_REQUEST) buffer;

	if (!pBuffer
		|| !BufferLen
		|| (BufferLen > sizeof(buffer) - (sizeof(EXTERNAL_DRV_REQUEST) + sizeof(FIDBOX_REQUEST_DATA))))
	{
		return FALSE;
	}

	pExtReq->m_DrvID = FLTTYPE_SYSTEM;
	pExtReq->m_IOCTL = _AVPG_IOCTL_FIDBOX_SET;
	pExtReq->m_AppID = AppID;
	pExtReq->m_BufferSize = BufferLen;
	
	// HANDLE + data
	PFIDBOX_REQUEST_DATA pFid = (PFIDBOX_REQUEST_DATA) pExtReq->m_Buffer;

	memcpy(pFid, pBuffer, BufferLen);

	ULONG OutRequestSize = 0;

	bRet = IDriverExternalDrvRequest(hDevice, pExtReq, NULL, &OutRequestSize);

	return bRet;
}

BOOL IDriver_FidBox_SetByHandle(HANDLE hDevice, ULONG AppID, HANDLE hFile, PVOID pBuffer, ULONG BufferLen)
{
	BOOL bRet = FALSE;
	
	BYTE buffer[0x1000];
	PEXTERNAL_DRV_REQUEST pExtReq = (PEXTERNAL_DRV_REQUEST) buffer;

	if (!pBuffer
		|| !BufferLen
		|| (BufferLen > sizeof(buffer) - 
		(sizeof(EXTERNAL_DRV_REQUEST) + sizeof(FIDBOX_REQUEST_SET) + BufferLen)))
	{
		return FALSE;
	}

	pExtReq->m_DrvID = FLTTYPE_SYSTEM;
	pExtReq->m_IOCTL = _AVPG_IOCTL_FIDBOX_SET_BH;
	pExtReq->m_AppID = AppID;
	pExtReq->m_BufferSize = sizeof(FIDBOX_REQUEST_SET) + BufferLen;
	
	// HANDLE + data
	PFIDBOX_REQUEST_SET pFid = (PFIDBOX_REQUEST_SET) pExtReq->m_Buffer;

	pFid->m_hFile = (ULONG) hFile;
	pFid->m_BufferSize = BufferLen;
	memcpy( pFid->m_Buffer, pBuffer, BufferLen );

	ULONG OutRequestSize = 0;

	bRet = IDriverExternalDrvRequest(hDevice, pExtReq, NULL, &OutRequestSize);

	return bRet;
}

BOOL IDriver_FidBox_Get(HANDLE hDevice, ULONG AppID, HANDLE hFile, PVOID pBuffer, ULONG* pBufferLen)
{
	BOOL bRet = FALSE;
	
	BYTE buffer[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(FIDBOX_REQUEST_GET)];
	PEXTERNAL_DRV_REQUEST pExtReq = (PEXTERNAL_DRV_REQUEST) buffer;

	if (!pBuffer || !pBufferLen)
		return FALSE;

	pExtReq->m_DrvID = FLTTYPE_SYSTEM;
	pExtReq->m_IOCTL = _AVPG_IOCTL_FIDBOX_GET;
	pExtReq->m_AppID = AppID;
	pExtReq->m_BufferSize = sizeof(FIDBOX_REQUEST_GET);
	
	// HANDLE
	PFIDBOX_REQUEST_GET pFid = (PFIDBOX_REQUEST_GET) pExtReq->m_Buffer;
	pFid->m_hFile = (ULONG) hFile;

	bRet = IDriverExternalDrvRequest(hDevice, pExtReq, pBuffer, pBufferLen);

	return bRet;
}


BOOL IDriverGetHashFuncVersion(HANDLE hDevice, ULONG* pVersion)
{
	BOOL bRet = FALSE;
	DWORD BytesRet = 0;
	ULONG Version = 0;
	
	if (!pVersion)
		return FALSE;

	bRet = DeviceIoControl(hDevice, IOCTLHOOK_GETHASHFUNCVER, NULL, 0, &Version, sizeof(Version), &BytesRet, NULL);

	if (bRet && (BytesRet == sizeof(ULONG)))
	{
		*pVersion = Version;
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	return bRet;
}


ULONG
IDriverGetFilterSize (
	PFILTER_TRANSMIT pFilter
	)
{
	ULONG FilterSize = sizeof(FILTER_TRANSMIT);

	PFILTER_PARAM pParam = (PFILTER_PARAM) pFilter->m_Params;
	for (ULONG cou = 0; cou < pFilter->m_ParamsCount; cou++)
	{
		ULONG tmp = sizeof(FILTER_PARAM) + pParam->m_ParamSize;
		FilterSize += tmp;

		BYTE* pPointer = (BYTE*)pParam + tmp;
		pParam = (PFILTER_PARAM) pPointer;
	}

	return FilterSize;
}