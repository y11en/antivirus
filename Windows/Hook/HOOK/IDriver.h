//IDriver
#ifndef __IDRIVER_H
#define __IDRIVER_H

#include <windows.h>

#include <winioctl.h>
#include "..\hook\avpgcom.h"

#define AVPGSWhistleupName	"avpgWhistleup"
#define AVPGWFChangedName	"avpgWFChanged"

#ifdef _DEBUG
VOID DbgPrint(DWORD level, LPCTSTR lpFmt,...);
#else
#define DbgPrint
#endif

typedef struct _DRV_EVENT_DESCRIBE
{
	HANDLE		m_hDevice;
	ULONG		m_AppID;
	ULONG		m_Mark;
	ULONG		m_EventFlags;		// this is copy of event flags form driver
}DRV_EVENT_DESCRIBE, *PDRV_EVENT_DESCRIBE;

#if defined( __cplusplus )
extern "C" 
{
#endif

HANDLE __cdecl RegisterApp(APP_REGISTRATION* pAppReg, OSVERSIONINFO* pOSVer, HANDLE* phWhistleup, HANDLE* phWFChanged, char* pWhistleupName, char* pChangedName);
HANDLE __cdecl RegisterAppEx(APP_REGISTRATION* pAppReg, OSVERSIONINFO* pOSVer, HANDLE* phWhistleup, HANDLE* phWFChanged, char* pWhistleupName, char* pChangedName, PCLIENT_INFO pClientInfo);
HANDLE __cdecl RegisterAppInDrv(APP_REGISTRATION* pAppReg, OSVERSIONINFO* pOSVer, HANDLE* phWhistleup, HANDLE* phWFChanged, char* pWhistleupName, char* pChangedName, PCHAR pDrvName, PCLIENT_INFO pClientInfo);
DWORD __cdecl UnRegisterApp(HANDLE hDevice, APP_REGISTRATION* pAppReg, BOOL bStayResident, OSVERSIONINFO* pOSVer);
BOOL __cdecl IDriverUnregisterApp(HANDLE hDevice, ULONG AppID, BOOL bStayResident);

DWORD __cdecl IDriverRegisterInvisibleThread(HANDLE hDevice, ULONG uAppID);		// return ThreadID from driver if successful, zero if failed
DWORD __cdecl IDriverUnregisterInvisibleThread(HANDLE hDevice, ULONG uAppID);	// return ThreadID from driver if successful, zero if failed
DWORD __cdecl IDriverRegisterInvisibleThreadByID(HANDLE hDevice, ULONG uAppID, DWORD ThreadID);
DWORD __cdecl IDriverUnregisterInvisibleThreadByID(HANDLE hDevice, ULONG uAppID, DWORD ThreadID);
DWORD __cdecl IDriverIsInvisibleThread(HANDLE hDevice, ULONG uAppID, DWORD ThreadID);

BOOL __cdecl IDriverState(HANDLE hDevice, ULONG uAppID, APPSTATE_REQUEST AppReqState, PAPPSTATE pCurrentState);	// set state from AppReqState and return new state in *pCurrentState
BOOL __cdecl IDriverGetState(HANDLE hDevice, ULONG uAppID, PHDSTATE pActivity);   // set state from

BOOL __cdecl IDriverYieldEvent(HANDLE hDevice, ULONG AppID, ULONG Mark, ULONG Timeout);
BOOL __cdecl IDriverYieldEventForce(HANDLE hDevice, ULONG AppID, ULONG Mark, ULONG Timeout);

BOOL __cdecl IDriverSetVerdict(HANDLE hDevice, ULONG AppID, VERDICT Verdict, ULONG Mark, ULONG ExpTime);
BOOL __cdecl IDriverSetVerdict2(HANDLE hDevice, PSET_VERDICT pVerdict);
void __cdecl IDriverSkipEvent(HANDLE hDevice, ULONG AppID);

BOOL __cdecl IDriverGetEvent(HANDLE hDevice, ULONG AppID, ULONG Mark, BYTE* pBuffer, ULONG BufferSize, ULONG* pBytesRet);
DWORD __cdecl IDriverGetEventSize(PEVENT_TRANSMIT pEvent);
PSINGLE_PARAM __cdecl IDriverGetEventParam(PEVENT_TRANSMIT pEvent, DWORD ParamID);
PSINGLE_PARAM __cdecl IDriverGetEventMappedParam(PEVENT_TRANSMIT pEvent, DWORD ParamID);

BOOL __cdecl IDriverCacheClean(HANDLE hDevice, ULONG AppID);

BOOL __cdecl AddFSFilter(HANDLE hDevice, DWORD AppID, LPSTR szFileName, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite /*= NULL*/);
BOOL __cdecl AddFSFilterW(HANDLE hDevice, DWORD AppID, PWCHAR szFileName, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite/*= NULL*/);
BOOL __cdecl IDriverResetFilters(HANDLE hDevice, DWORD AppID);
						
ULONG __cdecl AddFSFilter2(HANDLE hDevice, DWORD AppID, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite, PFILTER_PARAM pFirstParam, ...);
ULONG __cdecl AddFSFilter3(HANDLE hDevice, DWORD AppID, LPSTR szProcessName, DWORD dwTimeout, DWORD dwFlags, DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, LONGLONG ExpireTime, PROCESSING_TYPE ProcessingType, DWORD* pAdditionSite, PFILTER_PARAM* pParamArray, ULONG ParamsCount);

BOOL __cdecl IDriverSaveFilters(HANDLE hDevice, DWORD AppID);
BOOL __cdecl IDriverDeleteFilter(HANDLE hDevice, DWORD AppID, ULONG FilterId);
PFILTER_PARAM __cdecl IDriverGetFilterParam(PFILTER_TRANSMIT pFilter, DWORD ParamID);
PFILTER_PARAM __cdecl IDriverGetFilterNextParam(PFILTER_TRANSMIT pFilter, ULONG Index);

// -----------------------------------------------------------------------------------------
BOOL __cdecl IDriverFilterEvent(HANDLE hDevice, PFILTER_EVENT_PARAM pEventParam, BOOL bTry, PVERDICT pVerdict);

// -----------------------------------------------------------------------------------------
BOOL __cdecl IDriverIncreaseUnsafe(HANDLE hDevice, ULONG AppID, ULONG* puCurrentUnsafe);
BOOL __cdecl IDriverDecreaseUnsafe(HANDLE hDevice, ULONG AppID, ULONG* puCurrentUnsafe);

// -----------------------------------------------------------------------------------------
//BOOL __cdecl IDriverThreadSubProcesssing(HANDLE hDevice, PTHREAD_PAGE_SET pPageSet, DWORD BlockSize);

// -----------------------------------------------------------------------------------------
// ret val 
//	= 0				(STATUS_SUCCESS)			if success
//  = 0xC0000001L	(STATUS_UNSUCCESSFUL)		if event not found
//	= 0xC0000225L	(STATUS_NOT_FOUND)			if thread already released
//  = 0xC000000DL	(STATUS_INVALID_PARAMETER)	if execution on holded thread has invalid parameters

DWORD __cdecl _HE_GetDriveType(PDRV_EVENT_DESCRIBE pDrvDescr, BYTE drv, DWORD* pVal);
BOOL __cdecl _HE_ImpersonateThread(PDRV_EVENT_DESCRIBE pDrvDescr);

BOOL __cdecl IDriverChangeDebugInfoLevel(HANDLE hDevice, ULONG DebugInfo);
BOOL __cdecl IDriverChangeDebugInfoMask(HANDLE hDevice, ULONG DebugMask);
BOOL __cdecl IDriverGetDebugInfoLevel(HANDLE hDevice, ULONG* pdwDebugInfo);
BOOL __cdecl IDriverGetDebugInfoMask(HANDLE hDevice, ULONG* pdwDebugMask);

BOOL __cdecl IDriverExternalDrvRequest(HANDLE hDevice, PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG* OutRequestSize);

// -----------------------------------------------------------------------------------------
BOOL __cdecl IDriverEventObjectRequest(HANDLE hDevice, PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG* OutRequestSize);

BOOL __cdecl IDriverProProtRequest(HANDLE hDevice, ULONG AppID, BOOL bSwitch, DWORD Flags);
// -----------------------------------------------------------------------------------------
// stuff
BOOL __cdecl IDriverGetUserFromSid(SID* pUserSid, char* pszDomain, DWORD* pdwDomainLen, char* pszUser, DWORD* pdwUserLen);
BOOL __cdecl IDriverGetUserFromSidW(SID* pUserSid, WCHAR* pszDomain, DWORD* pdwDomainLen, WCHAR* pszUser, DWORD* pdwUserLen);

//+ ------------------------------------------------------------------------------------------

BOOL __cdecl IDriver_ReloadSettings(ULONG SettingsMask);
BOOL __cdecl IDriver_SpecFileRequest(PSPECFILEFUNC_REQUEST pRequest, ULONG RequestSize, PVOID pBuffer, PULONG pBufferSize);

BOOL __cdecl IDriver_GetDiskStat(HANDLE hDevice, ULONG AppID, unsigned __int64* pTimeCurrent, unsigned __int64* pTotalWaitTime);

BOOL __cdecl IDriver_FidBox_Set(HANDLE hDevice, ULONG AppID, HANDLE hFile, PVOID pBuffer, ULONG BufferLen);
BOOL __cdecl IDriver_FidBox_Get(HANDLE hDevice, ULONG AppID, HANDLE hFile, PVOID pBuffer, ULONG* pBufferLen);

BOOL __cdecl IDriver_FidBox_SetByHandle(HANDLE hDevice, ULONG AppID, HANDLE hFile, PVOID pBuffer, ULONG BufferLen);
//+ ----------------------------------------------------------------------------------------
// special
HANDLE __cdecl IDriverGetKlifNewDriver(OSVERSIONINFO* pOSVer);
BOOL __cdecl IDriverGetAdditionalState(HANDLE hDevice, PADDITIONALSTATE pAddState);

BOOL __cdecl IDriverGetVersion(HANDLE hDevice, DWORD* pDwBuildNum);
BOOL __cdecl IDriverClientActivity(HANDLE hDevice, PHDSTATE pActivityIn, PHDSTATE pActivityOut);
BOOL __cdecl IDriverFilterTransmit(HANDLE hDevice, FILTER_TRANSMIT* pInFilter, FILTER_TRANSMIT* pOutFilter, DWORD BufferSizeIn, DWORD BufferSizeOut);
BOOL __cdecl IDriverTSPRequest(HANDLE hDevice, PTSPRS_LOCK Req,ORIG_HANDLE *OutOrigHandle);

BOOL __cdecl IDriverInitDirectCallIO(HANDLE hDevice, OSVERSIONINFO* pOSVer);

BOOL __cdecl IDriverIsTspOk();

HANDLE __cdecl IDriver_GetDriverHandleQuick();
BOOL __cdecl IDriverGetHashFuncVersion(HANDLE hDevice, ULONG* pVersion);

ULONG __cdecl IDriverGetFilterSize( PFILTER_TRANSMIT pFilter);
//////////////////////////////////////////////////////////////////////////

ULONG __cdecl IDriver_GetAllowedPids(ULONG* pServicesExe, ULONG* pCsrss);
BOOL __cdecl IDriver_AddAllowedPidFilter(ULONG AppID, HANDLE hDevice, ULONG ProcessId);

#if defined( __cplusplus )
}
#endif


#endif