// SelfDefence.cpp : Defines the entry point for the console application.
//

#include <windows.h>

#pragma comment(lib, "mklapi.lib")

#include <version/ver_product.h>

#include <shtypes.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tlhelp32.h>

#include "../mklif/mklapi/mklapi.h"

typedef struct _sDrvContext
{
	PVOID	m_pClientContext;
	HANDLE	m_hDevice;
}DrvContext, *PDrvContext;

#ifdef _self_defence
#ifndef VER_PRODUCT_APPDATA_NAME_W
#define VER_PRODUCT_APPDATA_NAME_W L"KIS"
#endif
#endif

WCHAR wsPSDllPath[MAX_PATH * 2] = L"psapi.dll";

typedef
HANDLE
(WINAPI* _tpCreateToolhelp32Snapshot) (
  DWORD dwFlags, 
  DWORD th32ProcessID 
);

typedef
BOOL
(WINAPI* _tpProcess32First) (
  HANDLE hSnapshot, 
  LPPROCESSENTRY32W lppe 
);

typedef
BOOL
(WINAPI* _tpProcess32Next) (
  HANDLE hSnapshot, 
  LPPROCESSENTRY32W lppe 
);

typedef
DWORD
(WINAPI *_tpZwQuerySystemInformation) (
	ULONG InfoClass,
	PVOID SysInfo,
	ULONG SysInfoLen,
	PULONG LenReturned
	);

typedef
BOOL
(WINAPI* _tpPS_EnumProcesses) (
  DWORD* pProcessIds,
  DWORD cb,
  DWORD* pBytesReturned
);

typedef
DWORD
(WINAPI* _tpPS_GetModuleBaseName) (
  HANDLE hProcess,
  HMODULE hModule,
  LPWSTR lpBaseName,
  DWORD nSize
);

_tpCreateToolhelp32Snapshot pfCreateToolhelp32Snapshot = NULL;
_tpProcess32First pfProcess32First = NULL;
_tpProcess32Next pfProcess32Next = NULL;
_tpZwQuerySystemInformation pfZwQuerySystemInformation = NULL;

_tpPS_EnumProcesses pfPS_EnumProcesses = NULL;
_tpPS_GetModuleBaseName pfPS_GetModuleBaseName = NULL;

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


typedef enum _eThreeState
{
	_eTS_None = 0,
	_eTS_On = 1,
	_eTS_Off = 2
}eThreeState;

// the functions below are implemented in mklif/fssync.lib

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );

	return ptr;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;
};

// Old driver

HANDLE
SD_OLD_GetDriverHandle	(
	)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwErr;

	hDevice = CreateFileW( L"\\\\.\\KLIF", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (INVALID_HANDLE_VALUE == hDevice)
	{
		dwErr = GetLastError();
		if (!dwErr || dwErr == ERROR_ACCESS_DENIED)
		{
			for (int idx = 0; (idx < 3) && (hDevice == INVALID_HANDLE_VALUE); idx++)
			{
				hDevice = CreateFileW( L"\\\\.\\KLIF", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if (INVALID_HANDLE_VALUE == hDevice)
					Sleep( 100 );
			}
		}
	}
	
	return hDevice;
}

HANDLE
SD_OLD_ClientRegisterImp (
	APP_REGISTRATION* pAppReg,
	HANDLE* phWhistleup,
	HANDLE* phWFChanged,
	char* pWhistleupName,
	char* pChangedName,
	PCLIENT_INFO pClientInfo
	)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	pAppReg->m_InterfaceVersion = HOOK_INTERFACE_NUM;
	pAppReg->m_SyncAddr = GetCurrentProcessId();
	
	*phWhistleup = NULL;
	*phWFChanged = NULL;

	hDevice = SD_OLD_GetDriverHandle();

	if (INVALID_HANDLE_VALUE == hDevice)
		return INVALID_HANDLE_VALUE;

	if(!(*phWhistleup = CreateEventA(NULL, FALSE, FALSE, pWhistleupName)) || 
		(!(*phWFChanged = CreateEventA(NULL, FALSE, FALSE, pChangedName)))) 
	{
		if (*phWhistleup != NULL)
			CloseHandle( *phWhistleup );
		if (*phWFChanged != NULL)
			CloseHandle( *phWFChanged );
		*phWhistleup = NULL;
		*phWFChanged = NULL;

		return INVALID_HANDLE_VALUE;
	}

	lstrcpyA( pAppReg->m_WhistleUp, pWhistleupName );
	lstrcpyA( pAppReg->m_WFChanged, pChangedName );

	if (INVALID_HANDLE_VALUE != hDevice)
	{
		DWORD dwRet;
		BOOL bRet = DeviceIoControl (
			hDevice,
			IOCTLHOOK_RegistrApp,
			pAppReg,
			sizeof(APP_REGISTRATION),
			pClientInfo,
			sizeof(CLIENT_INFO),
			&dwRet,
			NULL
			);

		if(!bRet)
		{
			CloseHandle( hDevice );
			hDevice = INVALID_HANDLE_VALUE;
		}
	}

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		CloseHandle( *phWhistleup );
		CloseHandle( *phWFChanged );
		*phWhistleup = NULL;
		*phWFChanged = NULL;
	}
	
	return hDevice;
}

HANDLE
SD_OLD_ClientRegister (
	PAPP_REGISTRATION pAppReg,
	PHANDLE phWhistleup,
	PHANDLE phWFChanged)
{
	pAppReg->m_CurProcId = GetCurrentProcessId();
	pAppReg->m_AppID = InstallerProtection;
	pAppReg->m_CacheSize = 0;
	pAppReg->m_Priority = AVPG_INVISIBLEPRIORITY;
	pAppReg->m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_SAVE_FILTERS | _CLIENT_FLAG_PARALLEL;
	pAppReg->m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter( (LARGE_INTEGER*) &qPerfomance );

	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];

	wsprintfA( WhistleupName, "IPU%08Xd%08X%08X", qPerfomance.HighPart, qPerfomance.LowPart, GetCurrentThreadId() );
	wsprintfA( WFChangedName, "IPC%08Xd%08X%08X", qPerfomance.HighPart, qPerfomance.LowPart, GetCurrentThreadId() );
	
	CLIENT_INFO ClientInfo;

	return SD_OLD_ClientRegisterImp( pAppReg, phWhistleup, phWFChanged, WhistleupName, WFChangedName, &ClientInfo );
}

void
SD_OLD_ClientUnregister (
	HANDLE hDevice,
	BOOL bStayResident
	)
{
	HDSTATE Activity;

	Activity.Activity = _AS_Unload;

	if (bStayResident)
		Activity.Activity = _AS_UnRegisterStayResident;
	
	Activity.AppID = InstallerProtection;

	DWORD dwRet;

	DeviceIoControl (
		hDevice,
		IOCTLHOOK_Activity,
		&Activity,
		sizeof(Activity),
		&Activity,
		sizeof(Activity),
		&dwRet,
		NULL
		);
}

void
SD_OLD_ReleaseDrvFile (
	HANDLE hDevice
	)
{
	EXTERNAL_DRV_REQUEST Request;
	memset( &Request, 0, sizeof(Request) );
	
	Request.m_DrvID = FLTTYPE_SYSTEM;
	Request.m_IOCTL = _AVPG_IOCTL_FIDBOX_DISABLE;
	Request.m_AppID = InstallerProtection;

	DWORD dwRet = 0;

	DeviceIoControl (
		hDevice,
		IOCTLHOOK_External_Drv_Request,
		&Request,
		sizeof(Request),
		NULL,
		0,
		&dwRet,
		NULL
		);
}

void
SD_OLD_DelAllFilters (
	HANDLE hDevice
	)
{
	FILTER_TRANSMIT Filter;
	Filter.m_AppID = InstallerProtection;
	Filter.m_FltCtl = FLTCTL_RESET_FILTERS;

	DWORD dwRet;

	DeviceIoControl (
		hDevice,
		IOCTLHOOK_FiltersCtl,
		&Filter,
		sizeof(Filter),
		&Filter,
		sizeof(Filter),
		&dwRet,
		NULL
		);
}

DWORD
SD_OLD_GetParentPidCheckMsi (
	DWORD CheckProcessId
	)
{
	DWORD RetParentPid = 0;
	DWORD ParentPid = 0;

	HMODULE hKernel = GetModuleHandleW( L"kernel32" );
	HMODULE hNtDll = GetModuleHandleW( L"ntdll" );

	pfCreateToolhelp32Snapshot = (_tpCreateToolhelp32Snapshot) GetProcAddress( hKernel ,"CreateToolhelp32Snapshot" );
	pfProcess32First = (_tpProcess32First) GetProcAddress( hKernel, "Process32FirstW" );
	pfProcess32Next = (_tpProcess32Next) GetProcAddress( hKernel, "Process32NextW" );

	if (!pfCreateToolhelp32Snapshot
		|| !pfProcess32First
		|| !pfProcess32Next
		)
	{
		if (!hNtDll)
			return 0;

		if (!wcslen(wsPSDllPath))
			return 0;

		HMODULE hPsApi = LoadLibraryW( wsPSDllPath );

		if (!hPsApi)
			return 0;

		pfPS_EnumProcesses = (_tpPS_EnumProcesses) GetProcAddress( hPsApi, "EnumProcesses" );
		pfPS_GetModuleBaseName = (_tpPS_GetModuleBaseName) GetProcAddress( hPsApi, "GetModuleBaseNameW" );

		pfZwQuerySystemInformation = (_tpZwQuerySystemInformation) GetProcAddress( hNtDll, "ZwQuerySystemInformation" );

		if (pfPS_EnumProcesses && pfPS_GetModuleBaseName && pfZwQuerySystemInformation)
		{
			PVOID pInfoBuff;
			DWORD dwResult;
			DWORD dwBuffSize = 1024 * 64;
			pInfoBuff = (CHAR*) pfMemAlloc( NULL, dwBuffSize, 0 );
			if (pInfoBuff)
			{
				dwResult = pfZwQuerySystemInformation (
					(DWORD)5,
					pInfoBuff,
					dwBuffSize,
					NULL
					);
					
				if(!dwResult)
				{
					PROCESS_INFO* pNextProcess = (PROCESS_INFO*) pInfoBuff;

					while (TRUE)
					{
						if (CheckProcessId == pNextProcess->PID)
						{
							ParentPid = pNextProcess->ParentPID;
							break;
						}
						
						if (!pNextProcess->Len)
							break;

						pNextProcess = (PROCESS_INFO*)((CHAR*) pNextProcess + pNextProcess->Len );
					}
				}

				pfMemFree( NULL, &pInfoBuff );
			}

			if (ParentPid)
			{
				HANDLE hProcess = OpenProcess (
					PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
					FALSE,
					ParentPid
				);

				if (hProcess)
				{
					WCHAR module[MAX_PATH * 2];

					if (pfPS_GetModuleBaseName( hProcess, NULL, module, sizeof(module) / sizeof(WCHAR) ))
					{
						if( !lstrcmpiW( module, L"msiexec.exe" ))
							RetParentPid = ParentPid;
					}

					CloseHandle( hProcess );
				}
			}
		}

		FreeLibrary( hPsApi );
		
		return RetParentPid;
	}
	
	HANDLE hProcessSnap = pfCreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if (INVALID_HANDLE_VALUE == hProcessSnap)
		return 0;

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32W );

	if( !pfProcess32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );

		return 0;
	}

	do
	{
		if (pe32.th32ProcessID == CheckProcessId)
		{
			ParentPid = pe32.th32ParentProcessID;

			break;
		}
	} while( pfProcess32Next( hProcessSnap, &pe32 ) );

	pe32.dwSize = sizeof( PROCESSENTRY32W );

	if( pfProcess32First( hProcessSnap, &pe32 ) )
	{ 
		do
		{
			if (pe32.th32ProcessID == ParentPid)
			{
				if( !lstrcmpiW( pe32.szExeFile, L"msiexec.exe" ))
					RetParentPid = ParentPid;

				break;
			}
		} while( pfProcess32Next( hProcessSnap, &pe32 ) );
	}

	CloseHandle( hProcessSnap );

	return RetParentPid;
}


void
SD_OLD_AddInvisibleProcess (
	HANDLE hDevice
	)
{
	INVISIBLE_TRANSMIT InvTransmit;

	InvTransmit.m_AppID = InstallerProtection;
	InvTransmit.m_InvCtl = INVCTL_PROCADD;
	InvTransmit.m_ThreadID = SD_OLD_GetParentPidCheckMsi(GetCurrentProcessId());

	DWORD BytesRet;

	BOOL bRet = DeviceIoControl (
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
void
SD_OLD_ChangeClientActiveStatus (
	HANDLE hDevice,
	BOOL bActivate
	)
{
	HDSTATE Activity;
	
	Activity.AppID = InstallerProtection;

	if (bActivate)
		Activity.Activity = _AS_GoActive;
	else
		Activity.Activity = _AS_GoSleep;
		
	DWORD dwRet;
	DeviceIoControl (
		hDevice,
		IOCTLHOOK_Activity,
		&Activity,
		sizeof(Activity),
		&Activity,
		sizeof(Activity),
		&dwRet,
		NULL
		);
}

void
SD_OLD_SaveFilters (
	HANDLE hDevice
	)
{
	HDSTATE Activity;
	
	Activity.Activity = _AS_SaveFilters;
	Activity.AppID = InstallerProtection;
	
	DWORD dwRet;

	DeviceIoControl (
		hDevice,
		IOCTLHOOK_Activity,
		&Activity,
		sizeof(Activity),
		&Activity,
		sizeof(Activity),
		&dwRet,
		NULL
		);
}

void
SD_OLD_AddFilter (
	HANDLE hDevice,
	DWORD dwFlags, 
	DWORD HookID,
	DWORD FunctionMj,
	DWORD FunctionMi,
	PROCESSING_TYPE ProcessingType,
	PFILTER_PARAM pFirstParam,
	...
	)
{
	ULONG FilterID = 0;

	va_list ArgPtr;
	BYTE* pFilterData;
	PFILTER_PARAM pNextParam = pFirstParam;
	DWORD PackeSize = sizeof(FILTER_TRANSMIT);
	int ParamCount = 0;

	DWORD BytesRet;

	va_start( ArgPtr, pFirstParam );
	while(pNextParam != NULL)
	{
		PackeSize += sizeof(FILTER_PARAM) + pNextParam->m_ParamSize;
		pNextParam = va_arg( ArgPtr, PFILTER_PARAM );
		ParamCount++;
	}
	va_end( ArgPtr );

	pFilterData = (BYTE*) pfMemAlloc( NULL, PackeSize, 0 );
	if (!pFilterData)
		return;

	PFILTER_TRANSMIT pFilter = (PFILTER_TRANSMIT) pFilterData;
	PFILTER_PARAM pParam = (PFILTER_PARAM) pFilter->m_Params;
	pNextParam = pFirstParam;

	va_start( ArgPtr, pFirstParam );
	while(pNextParam != NULL)
	{
		memcpy( pParam, pNextParam, sizeof(FILTER_PARAM) + pNextParam->m_ParamSize );
		pParam = (PFILTER_PARAM)((BYTE*)pParam + sizeof(FILTER_PARAM) + pNextParam->m_ParamSize);

		pNextParam = va_arg( ArgPtr, PFILTER_PARAM );
	}
	va_end( ArgPtr );

	pFilter->m_AppID = InstallerProtection;
	lstrcpyA( pFilter->m_ProcName, "*" );
	pFilter->m_Timeout = DEADLOCKWDOG_TIMEOUT;
	pFilter->m_Flags = dwFlags;
	pFilter->m_HookID = HookID;
	pFilter->m_FunctionMj = FunctionMj;
	pFilter->m_FunctionMi = FunctionMi;
	pFilter->m_ProcessingType = ProcessingType;
	pFilter->m_Expiration = 0;

	pFilter->m_ParamsCount = ParamCount;

	pFilter->m_FltCtl = FLTCTL_ADD;
	
	DeviceIoControl (
		hDevice,
		IOCTLHOOK_FiltersCtl,
		pFilter,
		PackeSize,
		pFilter,
		PackeSize,
		&BytesRet,
		NULL
		);
	
	pfMemFree( NULL, (void**) &pFilterData );
}

//- old driver

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) ( HANDLE hProcess,PBOOL Wow64Process );
LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
 
BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandleW(L"kernel32"),"IsWow64Process" );
 
    if (fnIsWow64Process)
    {
        if (!fnIsWow64Process( GetCurrentProcess(),&bIsWow64 ))
        {
            // handle error
        }
    }
    return bIsWow64;
}

BOOL DosPathToNtVolumePath(PWCHAR DosPath, PWCHAR NtVolumePath, size_t NtVolumePathSize)
{
	DWORD DosPathLen = (DWORD)wcslen( DosPath );
	if (DosPathLen >= 2 && DosPath[1] == L':')
	{
		WCHAR DosVolumeName[8];
		DosVolumeName[0] = DosPath[0];
		DosVolumeName[1] = L':';
		DosVolumeName[2] = 0;

		PWCHAR StrippedVolumePath = DosPath+2;
		
		WCHAR NtVolumeName[128];
		DWORD dwResult = QueryDosDeviceW( DosVolumeName, NtVolumeName, sizeof(NtVolumeName) / sizeof(NtVolumeName[0]) );
		if (dwResult && NtVolumeName[0] == L'\\')
		{
			wcscpy_s( NtVolumePath, NtVolumePathSize, NtVolumeName );
			wcscat_s( NtVolumePath, NtVolumePathSize, StrippedVolumePath );

			return TRUE;
		}
	}

	return FALSE;
}

// write/rename/delete filter
void
AddFiltersForPath (
	PDrvContext pDrvContext,
	PWCHAR Path
	)
{
	WCHAR NtPathName[MAX_PATH*2];

	BOOL bResult = TRUE;
	
	if (pDrvContext->m_pClientContext)
		bResult = DosPathToNtVolumePath( Path, NtPathName, sizeof(NtPathName) / sizeof(NtPathName[0]) );
	else
		lstrcpyW( NtPathName, Path );
	
	if (!bResult)
		return;

	CHAR Buf1[sizeof(FILTER_PARAM)+sizeof(DWORD)];
	PFILTER_PARAM pParamFlags = (PFILTER_PARAM)Buf1;

	pParamFlags->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_SINGLEID;
	pParamFlags->m_ParamFlt = FLT_PARAM_AND;
	pParamFlags->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
	pParamFlags->m_ParamSize = sizeof(DWORD);
	*(DWORD *)pParamFlags->m_ParamValue = _CONTEXT_OBJECT_FLAG_FORWRITE | _CONTEXT_OBJECT_FLAG_FORDELETE;

	LUID SystemLuid = SYSTEM_LUID;
	CHAR Buf2[sizeof(FILTER_PARAM)+sizeof(LUID)];
	PFILTER_PARAM pParamLuid = (PFILTER_PARAM)Buf2;

	pParamLuid->m_ParamFlags = _FILTER_PARAM_FLAG_SINGLEID | _FILTER_PARAM_FLAG_INVERS_OP;
	pParamLuid->m_ParamFlt = FLT_PARAM_EUA;
	pParamLuid->m_ParamID = _PARAM_OBJECT_LUID;
	pParamLuid->m_ParamSize = sizeof(LUID);
	*(PLUID)pParamLuid->m_ParamValue = SystemLuid;

	PFILTER_PARAM pParamURL = (PFILTER_PARAM)pfMemAlloc( NULL, sizeof(FILTER_PARAM)+sizeof(WCHAR)*MAX_PATH*2, 0 );
	if (pParamURL)
	{
		pParamURL->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_SINGLEID;
		pParamURL->m_ParamFlt = FLT_PARAM_WILDCARD;
		pParamURL->m_ParamID = _PARAM_OBJECT_URL_W;
		pParamURL->m_ParamSize = (ULONG)(sizeof(WCHAR)*(wcslen( NtPathName) + 1 ));
		memcpy( pParamURL->m_ParamValue, NtPathName, pParamURL->m_ParamSize );

		DWORD FilterID;
		if (pDrvContext->m_pClientContext)
		{
			HRESULT hResult = MKL_AddFilter (
				&FilterID,
				pDrvContext->m_pClientContext,
				"*",
				DEADLOCKWDOG_TIMEOUT,
				FLT_A_DENY,
				FLTTYPE_NFIOR,
				IRP_MJ_CREATE,
				0,
				0,
				PreProcessing,
				NULL,
				pParamFlags,
				pParamLuid,
				pParamURL,
				0
				);
		}
		else
		{
			SD_OLD_AddFilter( 
				pDrvContext->m_hDevice,
				FLT_A_DENY, 
				FLTTYPE_NFIOR,
				IRP_MJ_CREATE,
				0,
				PreProcessing,
				pParamFlags,
				pParamLuid,
				pParamURL,
				0
				);
		}

		pfMemFree( NULL, (PVOID *)&pParamURL );
	}
}

// rename op filter (MJ_SET_INFO with FileRenameInfo)
void
AddRenameFilter (
	PDrvContext pDrvContext,
	PWCHAR RenameSourcePath
	)
{
	WCHAR NtPathName[MAX_PATH*2];
	BOOL bResult = TRUE;

	if (pDrvContext->m_pClientContext)
		bResult = DosPathToNtVolumePath( RenameSourcePath, NtPathName, sizeof(NtPathName) / sizeof(NtPathName[0]) );
	else
		lstrcpyW( NtPathName, RenameSourcePath );

	if (!bResult)
		return;

	PFILTER_PARAM pParamUrl = (PFILTER_PARAM)pfMemAlloc( NULL, sizeof(WCHAR) * MAX_PATH * 2, 0 );
	if (pParamUrl)
	{
		HRESULT hResult;

		pParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		pParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
		pParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
		pParamUrl->m_ParamSize = (ULONG)(sizeof(WCHAR)*(wcslen( NtPathName ) + 1));
		memcpy( pParamUrl->m_ParamValue, NtPathName, pParamUrl->m_ParamSize );

		DWORD FilterID;

		if (pDrvContext->m_pClientContext)
		{
			hResult = MKL_AddFilter (
				&FilterID,
				pDrvContext->m_pClientContext,
				"*",
				DEADLOCKWDOG_TIMEOUT,
				FLT_A_DENY,
				FLTTYPE_NFIOR,
				IRP_MJ_SET_INFORMATION,
				FileRenameInformation,
				0,
				PreProcessing,
				NULL,
				pParamUrl,
				NULL
				);
		}
		else
		{
			SD_OLD_AddFilter (
				pDrvContext->m_hDevice,
				FLT_A_DENY, 
				FLTTYPE_NFIOR,
				IRP_MJ_SET_INFORMATION,
				FileRenameInformation,
				PreProcessing,
				pParamUrl,
				NULL
				);
		}

		pfMemFree( NULL, (PVOID *)&pParamUrl );
	}
}

void
ProtectKey (
	PDrvContext pDrvContext,
	PWCHAR KeyPath
	)
{
	PFILTER_PARAM pParamUrl;

	pParamUrl = (PFILTER_PARAM)pfMemAlloc( NULL, sizeof(WCHAR)*MAX_PATH*2, 0 );
	if (pParamUrl)
	{
		pParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_SINGLEID;
		pParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
		pParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
		pParamUrl->m_ParamSize = (ULONG)(sizeof(WCHAR)*(wcslen( KeyPath ) + 1));
		memcpy( pParamUrl->m_ParamValue, KeyPath, pParamUrl->m_ParamSize );

		HRESULT hResult;
		DWORD FilterID;

		if (pDrvContext->m_pClientContext)
		{
			hResult = MKL_AddFilter( &FilterID, pDrvContext->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT,
				FLT_A_DENY, FLTTYPE_REGS, Interceptor_SetValueKey, 0, 0, PreProcessing, NULL, pParamUrl, NULL );

			hResult = MKL_AddFilter( &FilterID, pDrvContext->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT,
				FLT_A_DENY, FLTTYPE_REGS, Interceptor_DeleteKey, 0, 0, PreProcessing, NULL, pParamUrl, NULL );

			hResult = MKL_AddFilter( &FilterID, pDrvContext->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT,
				FLT_A_DENY, FLTTYPE_REGS, Interceptor_DeleteValueKey, 0, 0, PreProcessing, NULL, pParamUrl, NULL );

			hResult = MKL_AddFilter( &FilterID, pDrvContext->m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT,
				FLT_A_DENY, FLTTYPE_REGS, Interceptor_RenameKey, 0, 0, PreProcessing, NULL, pParamUrl, NULL );
		}
		else
		{
			SD_OLD_AddFilter( pDrvContext->m_hDevice, FLT_A_DENY, FLTTYPE_REGS, Interceptor_SetValueKey, 0, PreProcessing, pParamUrl, NULL );

			SD_OLD_AddFilter( pDrvContext->m_hDevice, FLT_A_DENY, FLTTYPE_REGS, Interceptor_DeleteKey, 0, PreProcessing, pParamUrl, NULL );

			SD_OLD_AddFilter( pDrvContext->m_hDevice, FLT_A_DENY, FLTTYPE_REGS, Interceptor_DeleteValueKey, 0, PreProcessing, pParamUrl, NULL );

			SD_OLD_AddFilter( pDrvContext->m_hDevice, FLT_A_DENY, FLTTYPE_REGS, Interceptor_RenameKey, 0, PreProcessing, pParamUrl, NULL );
		}

		pfMemFree( NULL, (PVOID *)&pParamUrl );
	}
}
void
AddSecurityFiltersFileImp (
	PDrvContext pDrvContext,
	PWCHAR pwchService
	)
{
	WCHAR NtPathName[MAX_PATH*2];

	ULONG len;
	BYTE buf_cont[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM ParamContext = (PFILTER_PARAM) buf_cont;
	PFILTER_PARAM ParamUrl;

	LUID SystemLuid = SYSTEM_LUID;
	CHAR Buf2[sizeof(FILTER_PARAM)+sizeof(LUID)];
	PFILTER_PARAM pParamLuid = (PFILTER_PARAM)Buf2;

	if (!DosPathToNtVolumePath( pwchService, NtPathName, sizeof(NtPathName) / sizeof(NtPathName[0]) ))
		return;

	pParamLuid->m_ParamFlags = _FILTER_PARAM_FLAG_SINGLEID | _FILTER_PARAM_FLAG_INVERS_OP;
	pParamLuid->m_ParamFlt = FLT_PARAM_EUA;
	pParamLuid->m_ParamID = _PARAM_OBJECT_LUID;
	pParamLuid->m_ParamSize = sizeof(LUID);
	*(PLUID)pParamLuid->m_ParamValue = SystemLuid;

	len = (lstrlenW(NtPathName) + 1) * sizeof(WCHAR);

	ULONG fltsize = sizeof(FILTER_PARAM) + len;

    ParamUrl = (PFILTER_PARAM) HeapAlloc(GetProcessHeap(), 0, fltsize + 0x100);
	if (ParamUrl)
	{
		ZeroMemory(ParamUrl, fltsize);

		ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
		ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
		ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
		memcpy((PWCHAR) (ParamUrl->m_ParamValue), NtPathName, len);
		ParamUrl->m_ParamSize = len;

		SD_OLD_AddFilter (
			pDrvContext->m_hDevice,
			FLT_A_DENY, 
			FLTTYPE_SYSTEM,
			MJ_SYSTEM_SETFOSECURITY,
			0,
			PreProcessing,
			ParamUrl,
			pParamLuid,
			0
			);
	}
}

void
AddSecurityFiltersFile (
	PDrvContext pDrvContext,
	PWCHAR pwchPath
	)
{
	if (pDrvContext->m_pClientContext)
		return;

	AddSecurityFiltersFileImp( pDrvContext, pwchPath );

	DWORD Path_Len = (DWORD) wcslen( pwchPath );

	if (Path_Len)
	{
		AddSecurityFiltersFileImp( pDrvContext, pwchPath );

		size_t Path_DupSize = Path_Len + 3;
		PWCHAR Path_Dup = (PWCHAR) pfMemAlloc( NULL, sizeof(WCHAR) * Path_DupSize, 0 );
		if (Path_Dup)
		{
			memcpy( Path_Dup, pwchPath, sizeof(WCHAR)*(Path_Len + 1) );
			wcscat_s( Path_Dup, Path_DupSize, L"\\*" );
			AddSecurityFiltersFileImp( pDrvContext, Path_Dup );

			pfMemFree(NULL, (PVOID *)&(Path_Dup));
		}
	}
}


void
AddSecurityFiltersReg (
	PDrvContext pDrvContext,
	PWCHAR pwchService
	)
{
	if (pDrvContext->m_pClientContext)
		return;

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

		SD_OLD_AddFilter (
			pDrvContext->m_hDevice,
			FLT_A_DENY, 
			FLTTYPE_SYSTEM,
			MJ_SYSTEM_SETREGSECURITY,
			0,
			PreProcessing,
			ParamUrl,
			0
			);
	}
}

BOOL GetSpecialLocation(DWORD ShellId, PWCHAR ShellStr)
{
	BOOL bRet = FALSE;
	HRESULT hResult;
	LPITEMIDLIST pListId = NULL;

	hResult = SHGetSpecialFolderLocation( NULL, ShellId, &pListId );
	if(SUCCEEDED(hResult) && pListId)
	{
		bRet = SHGetPathFromIDListW( pListId, ShellStr );
		CoTaskMemFree( (PVOID) pListId );
	}

	return bRet;
};

void
ProtectAllUserAppData (
	PDrvContext pDrvContext,
	eThreeState ProtectbSecurity
	)
{
	size_t CommonAppPathSize = MAX_PATH*2;
	PWCHAR CommonAppPath = (PWCHAR)pfMemAlloc( NULL, sizeof(WCHAR)*CommonAppPathSize, 0 );

	if (CommonAppPath)
	{
		if (GetSpecialLocation( CSIDL_COMMON_APPDATA, CommonAppPath ))
		{
			DWORD ShortPathLen = 0;
			PWCHAR CommonAppPath_Short = NULL;

			if (CommonAppPath[wcslen( CommonAppPath ) - 1] != L'\\')
				wcscat_s( CommonAppPath, CommonAppPathSize, L"\\" );

			wcscat_s(CommonAppPath, CommonAppPathSize, L"Kaspersky Lab\\" VER_PRODUCT_APPDATA_NAME_W);

			if (_eTS_On == ProtectbSecurity)
				AddSecurityFiltersFile( pDrvContext, CommonAppPath ); 

			CommonAppPath_Short = (PWCHAR)pfMemAlloc(NULL, sizeof(WCHAR)*MAX_PATH, 0);
			if (CommonAppPath_Short)
				ShortPathLen = GetShortPathNameW(CommonAppPath, CommonAppPath_Short, MAX_PATH);

			wcscat_s(CommonAppPath, CommonAppPathSize, L"\\*");
			AddFiltersForPath( pDrvContext, CommonAppPath );

			if (ShortPathLen)
			{
				wcscat_s(CommonAppPath_Short, MAX_PATH, L"\\*");
				AddFiltersForPath( pDrvContext, CommonAppPath_Short );
			}

			if (CommonAppPath_Short)
				pfMemFree(NULL, (PVOID *)&CommonAppPath_Short);
		}

		pfMemFree(NULL, (PVOID *)&CommonAppPath);
	}

};

//
typedef BOOL (WINAPI *tCallbackLongShortPath)(LPVOID pContext, LPCWSTR wcsPath);
#define COMB_MAX_LEVELS 16

BOOL
WINAPI
CallbackLongShortPath (
	LPVOID pContext,
	LPCWSTR wcsPath
	)
{
	PDrvContext pDrvContext = (PDrvContext) pContext;

	AddRenameFilter( pDrvContext, (PWCHAR) wcsPath );

	size_t wcsPath_DupSize = wcslen( wcsPath ) + 3;
	PWCHAR wcsPath_Dup = (PWCHAR) pfMemAlloc( NULL, sizeof(WCHAR) * wcsPath_DupSize, 0 );
	if (wcsPath_Dup)
	{
		wcscpy_s( wcsPath_Dup, wcsPath_DupSize, wcsPath );
		wcscat_s( wcsPath_Dup, wcsPath_DupSize, L"\\*" );
		
		AddFiltersForPath( pDrvContext, wcsPath_Dup );
	}

	return TRUE;
}

BOOL
BuildLongShortPathCombinations (
	LPWSTR sPath,
	DWORD sPathSize,
	WIN32_FIND_DATAW fd[],
	DWORD dwLevel,
	tCallbackLongShortPath pfCallback,
	LPVOID pContext
	)
{
	if (!dwLevel)
		return pfCallback( pContext, sPath );

	SIZE_T len = wcslen(sPath);
	if (sPathSize - len < MAX_PATH+2)
		return FALSE;
	if (len && sPath[len-1]!='\\')
		sPath[len++] = '\\';
	dwLevel--;
	wcscpy_s(sPath+len, sPathSize-len, fd[dwLevel].cFileName);
	if (!BuildLongShortPathCombinations(sPath, sPathSize, fd, dwLevel, pfCallback, pContext))
		return FALSE;
	if (0==fd[dwLevel].cAlternateFileName[0] || 0==wcscmp(fd[dwLevel].cFileName, fd[dwLevel].cAlternateFileName))
		return TRUE;
	wcscpy_s(sPath+len, sPathSize-len, fd[dwLevel].cAlternateFileName);
	
	return BuildLongShortPathCombinations( sPath, sPathSize, fd, dwLevel, pfCallback, pContext );
}

BOOL
GetLongShortPathCombinations (
	LPCWSTR wcsPath,
	tCallbackLongShortPath pfCallback,
	LPVOID pContext
	)
{
	WIN32_FIND_DATAW fd[COMB_MAX_LEVELS];
	WCHAR sPath[0x1000];
	DWORD dwLevel = 0;
	if (!GetFullPathNameW(wcsPath, sizeof(sPath) / sizeof(sPath[0]), sPath, NULL))
		return FALSE;

	PWCHAR p;
	while (p = wcsrchr(sPath, '\\'))
	{
		if (p[1] == 0)
		{
			p[0] = 0;
			continue;
		}
		if (dwLevel == COMB_MAX_LEVELS)
			return FALSE;
		HANDLE hFind = FindFirstFileW(sPath, &fd[dwLevel]);
		if (INVALID_HANDLE_VALUE == hFind)
			return FALSE;
		FindClose(hFind);
		p[0] = 0;
		dwLevel++;
	}
	
	return BuildLongShortPathCombinations( sPath, sizeof(sPath) / sizeof(sPath[0]), fd, dwLevel, pfCallback, pContext );
}

//
void
ProtectMachine (
	PDrvContext pDrvContext,
	eThreeState ProtectFiles,
	PWCHAR AppPath,
	eThreeState ProtectRegistry,
	eThreeState ProtectbSecurity
	)
{
	if (_eTS_On == ProtectFiles)
	{
		ProtectAllUserAppData( pDrvContext, ProtectbSecurity );
		GetLongShortPathCombinations( AppPath, CallbackLongShortPath, pDrvContext );
		
		PWCHAR ExpandedPath = (PWCHAR) pfMemAlloc( NULL, sizeof(WCHAR)*MAX_PATH*2, 0 );
		if (ExpandedPath)
		{
			if (ExpandEnvironmentStringsW( L"%windir%\\system32\\drivers\\klif.sys", ExpandedPath, MAX_PATH*2) )
				AddFiltersForPath( pDrvContext, ExpandedPath );

			if (ExpandEnvironmentStringsW( L"%windir%\\system32\\drivers\\kl1.sys", ExpandedPath, MAX_PATH*2) )
				AddFiltersForPath( pDrvContext, ExpandedPath );

			if (ExpandEnvironmentStringsW( L"%windir%\\system32\\drivers\\klop.dat", ExpandedPath, MAX_PATH*2) )
				AddFiltersForPath( pDrvContext, ExpandedPath );

			//xp - klim5, vista klim6
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
			
			if (bProtectKlim5)
			{
				if (ExpandEnvironmentStringsW( L"%windir%\\system32\\drivers\\klim5.sys", ExpandedPath, MAX_PATH*2) )
					AddFiltersForPath( pDrvContext, ExpandedPath );
			}

			if (bProtectKlim6)
			{
				if (ExpandEnvironmentStringsW( L"%windir%\\system32\\drivers\\klim6.sys", ExpandedPath, MAX_PATH*2) )
					AddFiltersForPath( pDrvContext, ExpandedPath );
			}


			pfMemFree( NULL, (PVOID *)&ExpandedPath );
		}
	}

	if(_eTS_On == ProtectRegistry)
	{
		if (IsWow64())
			ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SOFTWARE\\WOW6432NODE\\" VER_PRODUCT_REGISTRY_PROT_W L"\\*" );
		else
			ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SOFTWARE\\" VER_PRODUCT_REGISTRY_PROT_W L"\\*" );

	// drivers & services
		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\KLIF" );
		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\KLIF\\*" );

		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\AVP" );
		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\AVP\\*" );

		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET*\\SERVICES\\KL1\\*" );
		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET*\\SERVICES\\KL1" );

		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\KLOP" );
		ProtectKey( pDrvContext, L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET001\\SERVICES\\KLOP\\*" );
	}

	if (_eTS_On == ProtectbSecurity)
	{
		AddSecurityFiltersReg( pDrvContext, L"*\\CONTROLSET*\\SERVICES\\AVP");
		AddSecurityFiltersReg( pDrvContext, L"*\\CONTROLSET*\\SERVICES\\AVP\\*");

		AddSecurityFiltersReg( pDrvContext, L"\\REGISTRY\\MACHINE\\SOFTWARE\\" VER_PRODUCT_REGISTRY_PROT_W );
		AddSecurityFiltersReg( pDrvContext, L"\\REGISTRY\\MACHINE\\SOFTWARE\\" VER_PRODUCT_REGISTRY_PROT_W L"\\*");
		
		if (AppPath)
			AddSecurityFiltersFile( pDrvContext, AppPath );
	}
}

BOOL _SD_PathRemoveFileSpecW(LPWSTR pszPath)
{
   wchar_t * pls = wcsrchr(pszPath, '\\');
   if (pls) 
   { 
      if (pls != pszPath)
      {  
         if (pls == &pszPath[2] && pszPath[1] == L':')
         {
            if (*(pls+1))
            {
               *(pls+1) = 0;
               return TRUE;
            }
         }
         else
         {
            *pls = 0;
            return TRUE;
         }
      }
      else if (*(pls+1))
      {
         *(pls+1) = 0;
         return TRUE;
      }
   }
   else
   {
      if (*pszPath)
      {
         *pszPath = 0;
         return TRUE;
      }
   }
   return FALSE;
}

BOOL
Protect (
	eThreeState eActivateStatus,
	eThreeState ProtectFile,
	PWCHAR pwchBasePath,
	eThreeState ProtectRegistry,
	eThreeState ProtectSecurity,
	BOOL bInstaller,
	BOOL bDelFidbox
	)
{
	PVOID pProContext = NULL;

	// old
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	APP_REGISTRATION AppReg;
	HANDLE hWhistleup;
	HANDLE hWFChanged;
    
	HRESULT hResult = MKL_ClientRegister (
		&pProContext, 
		InstallerProtection,
		AVPG_INVISIBLEPRIORITY,
		_CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_PARALLEL,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (!SUCCEEDED( hResult ) )
	{
		OSVERSIONINFO OSVer;
		OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if(GetVersionEx(&OSVer))
		{
			if (6 == OSVer.dwMajorVersion) // vista and higher
				return FALSE;
		}

		hDevice = SD_OLD_ClientRegister( &AppReg, &hWhistleup, &hWFChanged );
	
		if (INVALID_HANDLE_VALUE == hDevice)
			return FALSE;
	}

	if (bInstaller) // installer case
	{
		if (pProContext)
			MKL_AddInvisibleProcess( pProContext, TRUE );
		else
			SD_OLD_AddInvisibleProcess( hDevice );
	}

	PWCHAR pwchModuleFileName = NULL;

	if (_eTS_On == ProtectFile)
	{
		if (pwchBasePath)
			pwchModuleFileName = pwchBasePath;
		else
		{
			pwchModuleFileName = (PWCHAR) pfMemAlloc( NULL, MAX_PATH * sizeof(*pwchModuleFileName), 0 );

			DWORD dwResult = GetModuleFileNameW( NULL, pwchModuleFileName, MAX_PATH );
			DWORD err = GetLastError();

			if (!dwResult)
				pfMemFree( NULL, (PVOID *)&pwchModuleFileName );
			else if (ERROR_INSUFFICIENT_BUFFER == err)
			{
				#define UNI_MAX_LEN 0xfff0
				pfMemFree( NULL, (PVOID *)&pwchModuleFileName );
				
				pwchModuleFileName = (PWCHAR) pfMemAlloc( NULL, UNI_MAX_LEN, 0 );
				if (pwchModuleFileName)
					dwResult = GetModuleFileNameW( NULL, pwchModuleFileName, UNI_MAX_LEN );
				else
					dwResult = 0;
			}

		}

		if (pwchModuleFileName)
			_SD_PathRemoveFileSpecW( pwchModuleFileName );
		else
			ProtectFile = _eTS_Off;
	}

	if (_eTS_None != ProtectFile || _eTS_None != ProtectRegistry )
	{
		if (pProContext)
			MKL_DelAllFilters( pProContext );
		else
			SD_OLD_DelAllFilters( hDevice );

		DrvContext DrvContext;
		DrvContext.m_hDevice = hDevice;
		DrvContext.m_pClientContext = pProContext;
		ProtectMachine( &DrvContext, ProtectFile, pwchModuleFileName, ProtectRegistry, ProtectSecurity );
		
		if (pProContext)
			MKL_SaveFilters( pProContext );
		else
			SD_OLD_SaveFilters( hDevice );
	}

	switch (eActivateStatus)
	{
	case _eTS_On:
		if (pProContext)
			MKL_ChangeClientActiveStatus( pProContext, TRUE );
		else
			SD_OLD_ChangeClientActiveStatus( hDevice, TRUE );
		break;
	
	case _eTS_Off:
		if (pProContext)
			MKL_ChangeClientActiveStatus( pProContext, FALSE );
		else
			SD_OLD_ChangeClientActiveStatus( hDevice, FALSE );
		break;

	case _eTS_None:
	default:
		break;
	}

	if (bDelFidbox)
	{
		if (pProContext)
			MKL_ReleaseDrvFile( pProContext );
		else
			SD_OLD_ReleaseDrvFile( hDevice );
	}

	if (pProContext)
		MKL_ClientUnregisterStayResident( &pProContext );
	else
	{
		SD_OLD_ClientUnregister( hDevice, TRUE );

		CloseHandle( hWhistleup );
		CloseHandle( hWFChanged );
		CloseHandle( hDevice );
	}

	if (!pwchBasePath)
		pfMemFree( NULL, (PVOID *)&pwchModuleFileName );

	return TRUE;
}

BOOL
DelDrvFiles (
	)
{
	return Protect( _eTS_None, _eTS_None, NULL, _eTS_None, _eTS_None, FALSE, TRUE );
}

BOOL
DetachDrvFiles (
	)
{
	PVOID pProContext = NULL;

	HRESULT hResult = MKL_ClientRegister (
		&pProContext, 
		AVPG_Driver_Specific,
		AVPG_INVISIBLEPRIORITY,
		_CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (!SUCCEEDED( hResult ) )
		return FALSE;

	MKL_DetachDrvFiles( pProContext );
	MKL_ClientUnregisterStayResident( &pProContext );

	return TRUE;
}

BOOL
SetPSApiDllPath (
	PWCHAR pwchPath
	)
{
	size_t len = wcslen( pwchPath );
	if (len >= sizeof(wsPSDllPath) / sizeof(WCHAR))
		return FALSE;

	wcscpy_s( wsPSDllPath, sizeof(wsPSDllPath)/sizeof(WCHAR), pwchPath );
	return TRUE;
}

#ifdef _self_defence
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	eThreeState ActivateFileProtection = _eTS_Off;
	PWCHAR pwchFileProtectionLocation = NULL;
	eThreeState ActivateRegistryProtection = _eTS_On;
	eThreeState ProtectSecurity = _eTS_On;
	BOOL bInstaller = TRUE;
	BOOL bDelFidbox = TRUE;

	eThreeState eActivateStatus = _eTS_On;

	if (argc == 2)
	{
		ActivateFileProtection = _eTS_On;
		pwchFileProtectionLocation = argv[1];
	}

	Protect (
		eActivateStatus,
	    ActivateFileProtection,
	    pwchFileProtectionLocation,
	    ActivateRegistryProtection,
		ProtectSecurity,
	    bInstaller,
		bDelFidbox
	    );

	return 0;
}

#endif // _self_defence