// regdef.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../../../../windows/Hook/hook/avpgcom.h"
#include "../../../../windows/Hook/hook/IDriver.h"
#include "../../../../windows/Hook/hook/HookSpec.h"
#include "../../../../windows/Hook/hook/Funcs.h"

typedef struct _FSDRV_CONTEXT
{
	HANDLE	m_hDevice;
	ULONG	m_AppID;
} FSDRV_CONTEXT, *PFSDRV_CONTEXT;

bool InitInstance(PFSDRV_CONTEXT pFSDrv_Context)
{
	OSVERSIONINFO OSVer;
	OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&OSVer);

	APP_REGISTRATION AppReg;
	AppReg.m_CurProcId = GetCurrentProcessId();
	AppReg.m_AppID = RebootDefender;
	AppReg.m_CacheSize = 0;
	AppReg.m_Priority = AVPG_STANDARTRIORITY;
	AppReg.m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG;
	AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);

	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];

	wsprintfA(WhistleupName, "DWU%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
	wsprintfA(WFChangedName, "DWC%08Xd%08X", qPerfomance.HighPart, qPerfomance.LowPart);
	
	HANDLE	hWhistleup,
			hWFChanged;
	HANDLE hDevice = RegisterApp(&AppReg, &OSVer, &hWhistleup, &hWFChanged, WhistleupName, WFChangedName);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		pFSDrv_Context->m_hDevice = hDevice;
		pFSDrv_Context->m_AppID = AppReg.m_AppID;

		APPSTATE CurrentState;
		IDriverState(pFSDrv_Context->m_hDevice, pFSDrv_Context->m_AppID, _AS_GoActive, &CurrentState);

		return true;
	}
	
	return false;
}

bool ProtectKey(PFSDRV_CONTEXT pFSDrv_Context, PWCHAR KeyPattern, PWCHAR ValuePattern)
{
	ULONG	SizeOf_Url = sizeof(WCHAR)*(lstrlenW(KeyPattern)+1);
	PFILTER_PARAM ParamUrl = (PFILTER_PARAM)HeapAlloc(GetProcessHeap(), 0, sizeof(FILTER_PARAM)+SizeOf_Url);
	if (!ParamUrl)
		return false;
	ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
	lstrcpyW((PWCHAR)ParamUrl->m_ParamValue, KeyPattern);
	ParamUrl->m_ParamSize = SizeOf_Url;

	ULONG	SizeOf_Param = sizeof(WCHAR)*(lstrlenW(ValuePattern)+1);
	PFILTER_PARAM ParamParam = (PFILTER_PARAM)HeapAlloc(GetProcessHeap(), 0, sizeof(FILTER_PARAM)+SizeOf_Param);
	if (!ParamParam)
	{
		HeapFree(GetProcessHeap(), 0 ,ParamUrl);
		return false;
	}
	ParamParam->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamParam->m_ParamID = _PARAM_OBJECT_URL_PARAM_W;
	lstrcpyW((PWCHAR)ParamParam->m_ParamValue, ValuePattern);
	ParamParam->m_ParamSize = SizeOf_Param;

	ULONG FilterID = AddFSFilter2(pFSDrv_Context->m_hDevice, pFSDrv_Context->m_AppID, "*", DEADLOCKWDOG_TIMEOUT,
		FLT_A_DENY, FLTTYPE_REGS, Interceptor_SetValueKey, 0, 0, PreProcessing, NULL, ParamUrl, ParamParam, 0);

	HeapFree(GetProcessHeap(), 0 ,ParamParam);
	HeapFree(GetProcessHeap(), 0 ,ParamUrl);

	return !!FilterID;
}

bool ProtectFileMask(PFSDRV_CONTEXT pFSDrv_Context, PWCHAR pwchFileMask)
{
	ULONG	SizeOf_Url = sizeof(WCHAR) * (lstrlenW(pwchFileMask) + 1);
	PFILTER_PARAM ParamUrl = (PFILTER_PARAM)HeapAlloc(GetProcessHeap(), 0, sizeof(FILTER_PARAM) + SizeOf_Url);
	if (!ParamUrl)
		return false;

	ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
	lstrcpyW((PWCHAR)ParamUrl->m_ParamValue, pwchFileMask);
	ParamUrl->m_ParamSize = SizeOf_Url;

	ULONG SizeOf_Param = sizeof(ULONG);
	PFILTER_PARAM ParamParam = (PFILTER_PARAM)HeapAlloc(GetProcessHeap(), 0, sizeof(FILTER_PARAM) + SizeOf_Param);
	if (!ParamParam)
	{
		HeapFree(GetProcessHeap(), 0, ParamUrl);
		return false;
	}
	
	ParamParam->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamParam->m_ParamFlt = FLT_PARAM_AND;
	ParamParam->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
	*(ULONG*)ParamParam->m_ParamValue = FILE_WRITE_DATA;
	ParamParam->m_ParamSize = SizeOf_Param;

	ULONG FilterID = AddFSFilter2(pFSDrv_Context->m_hDevice, pFSDrv_Context->m_AppID, "*", DEADLOCKWDOG_TIMEOUT,
		FLT_A_DENY, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL, ParamUrl, ParamParam, 0);

	HeapFree(GetProcessHeap(), 0, ParamParam);
	HeapFree(GetProcessHeap(), 0, ParamUrl);

	return !!FilterID;
}


bool Finish(PFSDRV_CONTEXT pFSDrv_Context)
{
	return !!IDriverUnregisterApp(pFSDrv_Context->m_hDevice, pFSDrv_Context->m_AppID, TRUE);
}

int main(int argc, char* argv[])
{
	FSDRV_CONTEXT FSDrv_Context;
	bool Result = InitInstance(&FSDrv_Context);
	
	//Result = ProtectKey(&FSDrv_Context, L"\\REGISTRY\\MACHINE\\SYSTEM\\TESTKEY\\*", L"*");
	//Result = ProtectKey(&FSDrv_Context, L"\\REGISTRY\\MACHINE\\SYSTEM\\TESTKEY", L"*");

	ProtectFileMask(&FSDrv_Context, L"*.zzz");

	getchar();
	getchar();

	Result = Finish(&FSDrv_Context);

	return 0;
}

