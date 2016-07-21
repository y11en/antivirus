#include "pch.h"

PMKAV_STREAM_CONTEXT
GetStreamContext (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT FileObject
	);

PMKAV_STREAM_HANDLE_CONTEXT
GetStreamHandleContext (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT FileObject
	);

PMKAV_VOLUME_CONTEXT
GetVolumeContextFromFO (
	__in PFILE_OBJECT pFileObject
	);

VOID
ReleaseFltContext (
	PFLT_CONTEXT*  ppContext
	);

PFLT_INSTANCE
GetSystemVolumeInstance (
	);


NTSTATUS
InitTransactionContext (
	__in PMKAV_TRANSACTION_CONTEXT pContext,
	__in PVOID pTransObj
	);

PMKAV_TRANSACTION_CONTEXT
LookupTransactionContext (
	__in PKTRANSACTION pTransaction
	);

//////////////////////////////////////////////////////////////////////////
 
PFILTER_EVENT_PARAM
EvContext_Create (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext,
	BOOLEAN bPreOp
	);

PFILTER_EVENT_PARAM
EvContext_Write (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext
	);

PFILTER_EVENT_PARAM
EvContext_Common (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext,
	PROCESSING_TYPE ProcessingType
	);

PFILTER_EVENT_PARAM
EvContext_Rename (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext
	);

PFILTER_EVENT_PARAM
EvContext_CreateLink (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext
	);

PFILTER_EVENT_PARAM
EvContext_MountVolume (
	PUNICODE_STRING pVolumeName
	);

PFILTER_EVENT_PARAM
EvContext_UnMountVolume (
	PUNICODE_STRING pVolumeName
	);

PFILTER_EVENT_PARAM
EvContext_OpenProcess (
	__in ACCESS_MASK DesiredAccess,
	__in_opt PCLIENT_ID pClientId
	);

PFILTER_EVENT_PARAM
EvContext_TerminateProcess (
	__in HANDLE hProcess
	);

PFILTER_EVENT_PARAM
EvContext_WriteProcessMemory (
	__in HANDLE hProcess,
	__in PVOID StartAddress,
	__in PVOID pBuffer,
	__in ULONG BytesToWrite
	);

PFILTER_EVENT_PARAM
EvContext_CreateThread (
	__in HANDLE hDestProcessId,
	__in ACCESS_MASK DesiredAccess,
	__in PCONTEXT pContext,
	__in BOOLEAN bCreateSuspended
	);

PFILTER_EVENT_PARAM
EvContext_SetContextThread (
	__in HANDLE DestProcessId,
	__in HANDLE DestThreadID,
	__in PCONTEXT pContext
	);

PFILTER_EVENT_PARAM
EvContext_CreateProcess (
	__in PFLT_FILE_NAME_INFORMATION pNameInfo,
	__in ACCESS_MASK DesiredAccess,
	__in HANDLE ProcessId
	);

PFILTER_EVENT_PARAM
EvContext_RasDial (
);

PFILTER_EVENT_PARAM
EvContext_ProtectedStorage (
);

PFILTER_EVENT_PARAM
EvContext_RegRawInputDevices (
);

PFILTER_EVENT_PARAM
EvContext_Shutdown (
);

PFILTER_EVENT_PARAM
EvContext_SystemDebugControl (
	__in ULONG ControlCode
);

PFILTER_EVENT_PARAM
EvContext_LoadDriver (
	__in PUNICODE_STRING pServiceName
);

PFILTER_EVENT_PARAM
EvContext_SetSecurityObject (
	__in PUNICODE_STRING pName,
	__in ULONG FuncMj
);

PFILTER_EVENT_PARAM
EvContext_CreateSection(
	__in PUNICODE_STRING pFileName,
	__in ACCESS_MASK DesiredAccess,
	__in ULONG ObjAttributes,
	__in ULONG SectionPageProtection
);

PFILTER_EVENT_PARAM
EvContext_CreateSymbolicLinkObject(
	__in ACCESS_MASK DesiredAccess
);

PFILTER_EVENT_PARAM
EvContext_SetSystemInformation(
	__in PUNICODE_STRING pModuleName
);

PFILTER_EVENT_PARAM
EvContext_DuplicateObject(
	__in ULONG SrcPrcID,		 // source process id
	__in ULONG TrgPrcID,		 // target process id
	__in ULONG SrcHandle,	 // source handle
	__in ULONG SrcHandleType, // source handle type
	__in ULONG SrcHandlePID 	 // source handle
);

PFILTER_EVENT_PARAM
EvContext_GET_RETRIEVAL_POINTERS(
	__in PUNICODE_STRING pFileName,
	__in PUNICODE_STRING pVolumeName
);
//+ ------------------------------------------------------------------------------------------

VOID
EvContext_Done (
	PFILTER_EVENT_PARAM pCreateContext
	);

//+ ------------------------------------------------------------------------------------------
VOID
EvObject_Fill (
	__in IN PEVENT_OBJ pEventObj,
	__in PCFLT_RELATED_OBJECTS pRelatedObject,
	__in PMKAV_INSTANCE_CONTEXT pInstanceContext
	);

ULONG __stdcall
EvObject_Proceed (
	IN PEVENT_OBJECT_REQUEST pInRequest,
	IN PEVENT_OBJECT_INFO pEventInfo, 
	PVOID pOutRequest,
	ULONG OutputBufferLength,
	PULONG pRetSize
	);

//+ ------------------------------------------------------------------------------------------
PFLT_FILE_NAME_INFORMATION
GetFileNameInfo (
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PMKAV_INSTANCE_CONTEXT pInstanceContext
	);

VOID
ReleaseFileNameInfo (
	PFLT_FILE_NAME_INFORMATION pNameInfo
	);

PFLT_FILE_NAME_INFORMATION
LockFileNameInfo (
	PMKAV_STREAM_CONTEXT pContext
	);

VOID
ReplaceFileNameInfo (
	PMKAV_STREAM_CONTEXT pContext,
	PFLT_FILE_NAME_INFORMATION pNewFileNameInfo
	);

//+ ------------------------------------------------------------------------------------------

NTSTATUS
SeGetUserSid (
	__in_opt PFLT_CALLBACK_DATA Data,
	__out PSID* ppSid
	);

NTSTATUS
SeGetUserLuid (
	PLUID pLuid
	);


NTSTATUS
MySynchronousQueryInformationFile (
	IN PFLT_INSTANCE Instance,
	IN PFILE_OBJECT FileObject,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	OUT PULONG LengthReturned OPTIONAL
	);

NTSTATUS
FileObject_Read (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT pFileObject,
	ULONG SectorSize,
	LARGE_INTEGER Offset,
	BOOLEAN bCached,
	PVOID pOutputBuffer,
	ULONG OutputBufferLength,
	PULONG pRetSize
	);

NTSTATUS
FileObject_Operations  (
	EVENT_OBJECT_REQUEST_TYPE Request,
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT pFileObject,
	PVOID pBuffer,
	ULONG BufferLength,
	PVOID pOutputBuffer,
	ULONG OutputBufferLength,
	PULONG pRetSize
	);

NTSTATUS
FileObject_GetBasicInfo  (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT pFileObject,
	PFILE_BASIC_INFORMATION pBasicInfo
	);
