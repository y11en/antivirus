#ifndef __mklapi
#define __mklapi

#include <windows.h>

#define MKL_PROC __cdecl

#include "../inc/commdata.h"
#include "../../hook/avpgcom.h"
#include "../../hook/hookspec.h"
#include "../../hook/Funcs.h"
#include "../../klfltdev/ioctl.h"

typedef void* ( MKL_PROC *mkl_pfMemAlloc )(PVOID pOpaquePtr, size_t, ULONG );
typedef void ( MKL_PROC *mkl_pfMemFree )( PVOID pOpaquePtr, void** );

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __out
#define __out
#define __in
#define __inout
#endif

HRESULT
MKL_PROC
MKL_ClientRegister (
	__out PVOID* ppClientContext,
	__in ULONG AppId,
	__in ULONG Priority,
	__in ULONG ClientFlags,
	__in ULONG CacheSize,
	__in ULONG BlueScreenTimeout,
	__in mkl_pfMemAlloc pfAlloc,
	__in mkl_pfMemFree pfFree,
	__in_opt PVOID pOpaquePtr
	);

HRESULT
MKL_PROC
MKL_ClientRegisterDummy (
	__out PVOID* ppClientContext,
	__in mkl_pfMemAlloc pfAlloc,
	__in mkl_pfMemFree pfFree,
	__in_opt PVOID pOpaquePtr
	);

HRESULT
MKL_PROC
MKL_Slot_Allocate (
	__in PVOID pClientContext,
	__out PULONG pSlot,
	__in ULONG DataSize,
	__out PVOID* ppData
	);

HRESULT
MKL_PROC
MKL_Slot_Get (
	__in PVOID pClientContext,
	__in ULONG Slot,
	__out PVOID* ppData
	);

HRESULT
MKL_PROC
MKL_Slot_Free (
	__in PVOID pClientContext,
	__in ULONG Slot
	);

HRESULT
MKL_PROC
MKL_ClientUnregister (
	__in PVOID* ppClientContext
	);

HRESULT
MKL_PROC
MKL_ClientUnregisterStayResident (
	__in PVOID* ppClientContext
	);

// only for single thread client!!!
HRESULT
MKL_PROC
MKL_ClientBreakConnection (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_GetDriverApiVersion (
	__in PVOID pClientContext,
	__out PULONG pApiVersion
	);


HRESULT
MKL_PROC
MKL_GetDriverFlags (
	__in PVOID pClientContext,
	__out PULONG pDriverFlags
	);

HRESULT
MKL_PROC
MKL_GetAppId (
	__in PVOID pClientContext,
	__out PULONG pAppId
	);

HRESULT
MKL_PROC
MKL_BuildMultipleWait (
	__in PVOID pClientContext,
	__in ULONG WaiterThreadMaxCount
	);

HRESULT
MKL_PROC
MKL_GetMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__out PULONG pMessageSize,
	__out PMKLIF_EVENT_HDR* ppEventHdr
	);

HRESULT
MKL_PROC
MKL_GetMultipleMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__in PULONG pMessageSize,
	__in PMKLIF_EVENT_HDR* ppEventHdr,
	__in DWORD dwMilliseconds
	);

// --- single API
HRESULT
MKL_PROC
MKL_GetSingleMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__in PULONG pMessageSize,
	__in PMKLIF_EVENT_HDR* ppEventHdr,
	__in DWORD dwMilliseconds,
	__in BOOL* pbStop
	);

HRESULT
MKL_PROC
MKL_FreeSingleMessage (
	__in PVOID pClientContext,
	__in PVOID* ppMessage
	);
//

HRESULT
MKL_PROC
MKL_ReplyMessage (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PMKLIF_REPLY_EVENT pVerdict
	);

HRESULT
MKL_PROC
MKL_ClientStateRequest (
	__in PVOID pClientContext,
	__in APPSTATE_REQUEST AppReqState,
	__in PAPPSTATE pCurrentState
	);

HRESULT
MKL_PROC
MKL_ChangeClientActiveStatus (
	__in PVOID pClientContext,
	__in BOOL bActive
	);

HRESULT
MKL_PROC
MKL_ClientResetCache (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_AddFilterEx (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in LPSTR szProcessName,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in_opt DWORD* pAdditionSite,
	__in_opt PFILTER_PARAM* pPA,
	__in ULONG ParamsCount
	);

HRESULT
MKL_PROC
MKL_AddFilter (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in LPSTR szProcessName,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in_opt DWORD* pAdditionSite,
	__in_opt PFILTER_PARAM pFirstParam, ...
	);

HRESULT
MKL_PROC
MKL_DelFilter (
	__in PVOID pClientContext,
	__in ULONG FilterId
	);

HRESULT
MKL_PROC
MKL_DelAllFilters (
	__in PVOID pClientContext
	);

PMKLIF_EVENT_HDR
MKL_PROC
MKL_GetEventHdr (
	__in PVOID pMessage,
	__in ULONG MessageSize
	);

PSINGLE_PARAM
MKL_PROC
MKL_GetEventParam (
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in ULONG ParamID,
	__in BOOLEAN bGetMapped
	);

HRESULT
MKL_PROC
MKL_GetProcessName (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__inout PWCHAR* ppwstrProcessName
	);

HRESULT
MKL_PROC
MKL_GetProcessPath (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__inout PWCHAR* ppwstrProcessPath
	);

HRESULT
MKL_PROC
MKL_QueryEnlistedObjects (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__out PVOID* ppEnlistedObjectsInfo,
	__out PULONG pEnlistedObjectsInfoLen
	);

HRESULT
MKL_PROC
MKL_EnumEnlistedObjects (
	__in PVOID pEnlistedObjectsInfo,
	__in ULONG EnlistedObjectsInfoLen,
	__inout PULONG pEnumContext,
	__out PULONG pProcessId,
	__out PWCHAR* ppwchObjectName,
	__out PMKLIF_INFO_TAGS pObjectTag
	);

HRESULT
MKL_PROC
MKL_ObjectRequest (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PMKLIF_OBJECT_REQUEST pRequest,
	__in ULONG EventRequestSize,
	__out_opt PVOID pOutBuffer,
	__out_opt ULONG* pOutBufferSize
	);

HRESULT
MKL_PROC
MKL_SetVerdict (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PMKLIF_REPLY_EVENT pReplyEvent
	);

HRESULT
MKL_PROC
MKL_AddInvisibleThread (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_AddInvisibleThreadByHandle (
	__in PVOID pClientContext,
	__in HANDLE hThread
	);

HRESULT
MKL_PROC
MKL_DelInvisibleThreadByHandle (
	__in PVOID pClientContext,
	__in HANDLE hThread
	);

HRESULT
MKL_PROC
MKL_AddInvisibleProcess (
	__in PVOID pClientContext,
	__in BOOL bRecursive
	);

HRESULT
MKL_PROC
MKL_AddInvisibleProcessEx (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	);

HRESULT
MKL_PROC
MKL_DelInvisibleThread (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_GetVolumeName (
	__in PVOID pClientContext,
	__in PWCHAR pwchNativeVolumeName,
	__inout PWCHAR pwchVolumeName,
	__in ULONG VolumeNameLen
	);

ULONG
MKL_PROC
MKL_GetLastNativeError (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_QueryProcessesInfo (
	__in PVOID pClientContext,
	__out PVOID* ppProcessesInfo,
	__out PULONG pProcessesInfoLen
	);

HRESULT
MKL_PROC
MKL_EnumProcessInfo (
	__in PVOID pProcessesInfo,
	__in ULONG ProcessesInfoLen,
	__inout PULONG pEnumContext,
	__out PULONG pProcessId,
	__out PULONG pParentProcessId,
	__out PLARGE_INTEGER pStartTime,
	__out PWCHAR* ppwchImagePath,
	__out PWCHAR* ppwchCurrDir,
	__out PWCHAR* ppwchCmdLine
	);

HRESULT
MKL_PROC
MKL_QueryProcessModules (
	__in PVOID pClientContext,
	__in ULONG ProcessId,
	__out PVOID* ppModulesInfo,
	__out PULONG pModulesInfoLen
	);

HRESULT
MKL_PROC
MKL_EnumModuleInfo (
	__in PVOID pModulesInfo,
	__in ULONG ModulesInfoLen,
	__inout PULONG pEnumContext,
	__out PWCHAR* ppwchImagePath,
	__out PLARGE_INTEGER pImageBase,
	__out PULONG pImageSize,
	__out PLARGE_INTEGER pEntryPoint
	);

HRESULT
MKL_PROC
MKL_ReadProcessMemory (
	__in PVOID pClientContext,
	__in ULONG ProcessId,
	__in LARGE_INTEGER Offset,
	__inout PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pNumberOfBytesRead
	);

HRESULT
MKL_PROC
MKL_SaveFilters (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_ReleaseDrvFile (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_ImpersonateThread (
	__in PVOID pClientContext,
	__in PVOID pMessage
	);

HRESULT
MKL_PROC
MKL_IsImageActive (
	__in PVOID pClientContext,
	__in HANDLE hFile
	);

HRESULT
MKL_PROC
MKL_QueryActiveImages (
	__in PVOID pClientContext,
	__out PVOID* ppActiveImages,
	__out PULONG pActiveImagesLen
	);

HRESULT
MKL_PROC
MKL_EnumActiveImages (
	__in PVOID pActiveImages,
	__in ULONG ActiveImagesLen,
	__inout PULONG pEnumContext,
	__out PWCHAR* ppwchImagePath,
	__out PWCHAR* ppwchImageVolume,
	__out PULONG pFlags,
	__out PLARGE_INTEGER pHash
	);

HRESULT
MKL_PROC
MKL_AllowUnload (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_ClientYeild (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_DetachDrvFiles (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_QueryProcessHash (
	__in PVOID pClientContext,
	__in ULONG ProcessId,
	__inout PVOID pBuffer,
	__in ULONG BufferSize
	);

HRESULT
MKL_PROC
MKL_QueryFileHash (
	__in PVOID pClientContext,
	__in PWCHAR FilePath,
	__inout PVOID pBuffer,
	__inout PULONG pBufferSize
	);

HRESULT
MKL_PROC
MKL_ClientSync (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_QueryFiltersCount (
	__in PVOID pClientContext,
	__out PULONG pFiltersCount
	);

HRESULT
MKL_PROC
MKL_GetClientCounter (
	__in PVOID pClientContext,
	__in MKLIF_CLIENT_COUNTERS Counter,
	__out PLONG pCounterValue
	);

HRESULT
MKL_PROC
MKL_FidBox_GetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PMKLIF_FIDBOX_DATA pFidData,
	__inout PULONG pFidDataLen
	);

HRESULT
MKL_PROC
MKL_FidBox_SetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__in PVOID pFidBuffer,
	__in ULONG FidBufferLen
	);

HRESULT
MKL_PROC
MKL_FidBox_Get (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__out PMKLIF_FIDBOX_DATA pFidData,
	__inout PULONG pFidDataLen
	);

HRESULT
MKL_PROC
MKL_FidBox_Set (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in PVOID pFidBuffer,
	__in ULONG FidBufferLen
	);

HRESULT
MKL_PROC
MKL_LLD_QueryNames (
	__in PVOID pClientContext,
	__out PVOID* ppNames,
	__out PULONG pNamesLength
	);
	
HRESULT
MKL_PROC
MKL_LLD_Enum (
	__in PVOID pNames,
	__in ULONG NamesLength,
	__inout PULONG pEnumContext,
	__out PWCHAR* ppwchName
	);

HRESULT
MKL_PROC
MKL_LLD_GetInfo (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__out PMKLIF_LLD_INFO pInfo
	);

HRESULT
MKL_PROC
MKL_LLD_GetGeometry (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__out PMKLIF_LLD_GEOMETRY pGeometry
	);

HRESULT
MKL_PROC
MKL_LLD_Read (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__in __int64 Offset,
	__out PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytes
	);

HRESULT
MKL_PROC
MKL_LLD_Write (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__in __int64 Offset,
	__out PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytes
	);

HRESULT
MKL_PROC
MKL_LLD_GetDiskId (
	__in PVOID pClientContext,
	__in PWCHAR pName,
	__out PDISK_ID_INFO pDiskId
	);
/*
AccessMask - битовая маска прав доступа к ресурсу
Маска содержит информацию о трех типах доступа: запись, чтение, просмотр содержимого (энумерация)
Каждому типу отводится 2 бита:
1,2 бит - доступ на запись
3,4 бит - доступ на чтение
5,6 бит - просмотр содержимого (энумерация )

Биты могут принимать следующие значения:
00 - доступ разрешен 
01 - необходимо спросить пользователя
11 - доступ запрещен.
*/
HRESULT
MKL_PROC
MKL_AddApplRule (
	__in PVOID pClientContext,
	__in_opt PWCHAR nativeAppPath,
	__in PWCHAR nativeFilePath,
	__in_opt PWCHAR ValueName,
	__in_opt PVOID	pHash,
	__in ULONG	HashSize,
	__in ULONG AccessMask,
	__out_opt PLONGLONG pRulID,
	__in ULONG blockID
	);

HRESULT
MKL_PROC
MKL_ResetClientRules (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_ApplyRules (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_QueryFileNativePath (
	__in PVOID pClientContext,
	__in PWCHAR FilePath,
	__out PWCHAR pBuffer,
	__inout PULONG pBufferSize
	);

HRESULT
MKL_PROC
MKL_SpecFileRequest (
	__in PVOID pClientContext,
	__in PSPECFILEFUNC_REQUEST pSpecRequest,
	__in ULONG SpecRequestSize,
	__out PVOID pBuffer,
	__inout PULONG pBufferSize
	);

HRESULT
MKL_PROC
MKL_GetFilter (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	);

HRESULT
MKL_PROC
MKL_GetFilterFirst (
	__in PVOID pClientContext,
	__out PVOID* ppFilter
	);

HRESULT
MKL_PROC
MKL_GetFilterNext (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	);

VOID
MKL_PROC
MKL_GetFilterInfo (
	__in PVOID pFilter,
	__out_opt PULONG pFilterId,
	__out_opt DWORD* pdwTimeout,
	__out_opt DWORD* pdwFlags,
	__out_opt DWORD* pHookID,
	__out_opt DWORD* pFunctionMj,
	__out_opt DWORD* pFunctionMi,
	__out_opt LONGLONG* pExpireTime,
	__out_opt PROCESSING_TYPE* pProcessingType
	);

HRESULT
MKL_PROC
MKL_GetHashVersion (
	__in PVOID pClientContext,
	__out PULONG pHashVersion
	);

HRESULT
MKL_PROC
MKL_QueryFileHandleHash (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PVOID* ppHash,
	__out PULONG pHashSize
	);

HRESULT
MKL_PROC
MKL_GetHashSize (
	__in PVOID pClientContext,
	__out PULONG pHashSize
	);

//+ special functions
HRESULT
MKL_PROC
MKL_PreCreateControl (
	__in PVOID pClientContext,
	__in BOOL bEnable
	);

HRESULT
MKL_PROC
MKL_FilterEvent (
	__in PVOID pClientContext,
	__in PFILTER_EVENT_PARAM pEventParam,
	__in BOOL bTry,
	__out PULONG pVerdictFlags
	);

HRESULT
MKL_PROC
MKL_ChangeFilterParam (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__in PFILTER_PARAM pParam
	);

HRESULT
MKL_PROC
MKL_GetFileFB2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PFIDBOX2_REQUEST_DATA pFidBox2
	);

HRESULT
MKL_PROC
MKL_KLFltDev_SetRule (
	__in PVOID pClientContext,
	__in PWCHAR wcGuid, 
	__in PWCHAR wcDevType, 
	__in ULONG mask, 
	__out PREPLUG_STATUS preplug_status
	);

HRESULT
MKL_PROC
MKL_KLFltDev_GetRulesSize (
	__in PVOID pClientContext,
	__out PULONG pRulesSize
	);

HRESULT
MKL_PROC
MKL_KLFltDev_GetRules (
	__in PVOID pClientContext,
	__out PKLFLTDEV_RULES pRules,
	__out ULONG RulesSize
	);

HRESULT
MKL_PROC
MKL_KLFltDev_ApplyRules (
	__in PVOID pClientContext
	);


HRESULT
MKL_PROC
MKL_Virt_AddApplToSB (
	__in PVOID pClientContext,
	__in PWCHAR nativeAppPath,
	__in PWCHAR nativeSBPath,
	__in PWCHAR nativeSBVolName
	);

HRESULT
MKL_PROC
MKL_Virt_Apply (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_Virt_Reset (
	__in PVOID pClientContext
);

HRESULT
MKL_PROC
MKL_TerminateProcess (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	);

HRESULT
MKL_PROC
MKL_IsInvisibleProcess (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	);

HRESULT
MKL_PROC
MKL_IsInvisibleThread (
	__in PVOID pClientContext,
	__in ULONG ThreadId
	);

HRESULT
MKL_PROC
MKL_DisconnectAllClients (
	__in PVOID pClientContext
	);

HRESULT
MKL_PROC
MKL_GetStatCounter (
	__in PVOID pClientContext,
	__out PULONGLONG pStatCounter,
	__out PULONGLONG pCurrTime
	);

HRESULT
MKL_PROC
MKL_GetFidbox2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PULONG pValue
);

HRESULT
MKL_PROC
MKL_CreateFile (
	__in PVOID pClientContext,
	__in PWCHAR NativeFileName,
	__out PVOID pFileHandle,
	__inout PULONG pFileHandleSize
	);

HRESULT
MKL_PROC
MKL_ReadFile (
	__in PVOID pClientContext,
	__in PVOID  pFileHandle,
	__in ULONG  FileHandleSize,
	__inout PVOID pBuffer,
	__inout PULONG pBufferSize,
	__in LONGLONG ByteOffset
	);

HRESULT
MKL_PROC
MKL_CloseFile (
	__in PVOID pClientContext,
	__in PVOID pFileHandle,
	__in ULONG FileHandleSize
	);

HRESULT
MKL_PROC
MKL_Timing_Get (
	__in PVOID pClientContext,
	__inout PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytesRet
	);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __mklapi
