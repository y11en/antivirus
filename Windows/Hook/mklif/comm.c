#include "pch.h"
#include "inc/commdata.h"

#include "comm.tmh"

#define _COMM_MAX_CONNECTION	1024

extern void
ResolveAlign (
	ULONG block_size, 
	LONGLONG offset_low, 
	ULONG reqsize, 
	ULONG* poffset_align,
	ULONG* pnew_size
	);

typedef struct _EV_LISTITEM
{
    LIST_ENTRY					m_List;
	PMKLIF_EVENT_TRANSMIT		m_pEventTr;
	PEVENT_OBJ					m_pEventObj;
	PWCHAR						m_pwchProcessPath;
	ULONG						m_ProcessPathLenB;
	KEVENT						m_KEvent;
	KEVENT						m_KEventPending;
	PMKLIF_REPLY_EVENT			m_pEvVerdict;
	SECURITY_CLIENT_CONTEXT*	m_pClientContext;
} EV_LISTITEM, *PEV_LISTITEM;

// predefines
NTSTATUS
Comm_EventGetPart (
	PCOMM_CONTEXT pPortContext, 
	PMKLIF_GET_EVENT_PART pEvGetPart,
	PVOID pBuffer,
	ULONG BufferSize
	);

PEV_LISTITEM
Comm_FindEvent (
	PCOMM_CONTEXT pPortContext,
	ULONG DrvMark
	);
//

PWCHAR
Comm_GetCommandInStr (
	__in MKLIF_COMMAND Command)
{
	switch (Command)
	{
	case mkc_Undefined:				return L"<undefined>";
	case mkc_GetVersion:			return L"mkc_GetVersion";
	case mkc_RegisterClient:		return L"mkc_RegisterClient";
	case mkc_ClientStateRequest:	return L"mkc_ClientStateRequest";
	case mkc_AddFilter:				return L"mkc_AddFilter";
	case mkc_GetEventPart:			return L"mkc_GetEventPart";
	case mkc_ObjectRequest:			return L"mkc_ObjectRequest";
	case mkc_QueryDrvFlags:			return L"mkc_QueryDrvFlags";
	case mkc_AddInvThread:			return L"mkc_AddInvThread";
	case mkc_DelInvThread:			return L"mkc_DelInvThread";
	case mkc_IsInvisibleThread:		return L"mkc_IsInvisibleThread";
	case mkc_DelFilter:				return L"mkc_DelFilter";
	case mkc_DelAllFilters:			return L"mkc_DelAllFilters";
	case mkc_QueryProcessesInfo:	return L"mkc_QueryProcessesInfo";
	case mkc_QueryModulesInfo:		return L"mkc_QueryModulesInfo";
	case mkc_ReadProcessMemory:		return L"mkc_ReadProcessMemory";
	case mkc_SaveFilters:			return L"mkc_SaveFilters";
	case mkc_AddInvProcess:			return L"mkc_AddInvProcess";
	case mkc_AddInvProcessRecursive:return L"mkc_AddInvProcessRecursive";
	case mkc_ReleaseDrvFiles:		return L"mkc_ReleaseDrvFiles";
	case mkc_AddInvThreadByHandle:	return L"mkc_AddInvThreadByHandle";
	case mkc_DelInvThreadByHandle:	return L"mkc_DelInvThreadByHandle";
	case mkc_IsImageActive:			return L"mkc_IsImageActive";
	case mkc_QueryActiveImages:		return L"mkc_QueryActiveImages";
	case mkc_ClientResetCache:		return L"mkc_ClientResetCache";
	case mkc_AllowUnload:			return L"mkc_AllowUnload";
	case mkc_DetachDrvFiles:		return L"mkc_DetachDrvFiles";
	case mkc_QueryProcessHash:		return L"mkc_QueryProcessHash";
	case mkc_BreakConnection:		return L"mkc_BreakConnection";
	case mkc_ClientSync:			return L"mkc_ClientSync";
	case mkc_QueryFiltersCount:		return L"mkc_QueryFiltersCount";
	case mkc_QueryClientCounter:	return L"mkc_QueryClientCounter";
	case mkc_FidBoxGet_ByHandle:	return L"mkc_FidBoxGet_ByHandle";
	case mkc_FidBoxSet_ByHandle:	return L"mkc_FidBoxSet_ByHandle";
	case mkc_LLD_QueryNames:		return L"mkc_LLD_QueryNames";
	case mkc_LLD_GetInfo:			return L"mkc_LLD_GetInfo";
	case mkc_LLD_GetGeometry:		return L"mkc_LLD_GetGeometry";
	case mkc_LLD_Read:				return L"mkc_LLD_Read";
	case mkc_LLD_Write:				return L"mkc_LLD_Write";
	case mkc_LLD_GetDiskId:			return L"mkc_LLD_GetDiskId";
	case mkc_AddApplRule:			return L"mkc_AddApplRule";
	case mkc_ResetClientRules:		return L"mkc_ResetClientRules";
	case mkc_ApplyRules:			return L"mkc_ApplyRules";
	case mkc_QueryFileNativePath:	return L"mkc_QueryFileNativePath";
	case mkc_QueryFileHash:			return L"mkc_QueryFileHash";
	case mkc_SpecialFileRequest:	return L"mkc_SpecialFileRequest";
	case mkc_FilterGetSize:			return L"mkc_FilterGetSize";
	case mkc_FilterGet:				return L"mkc_FilterGet";
	case mkc_FilterGetNextId:		return L"mkc_FilterGetNextId";
	case mkc_GetHashVersion:		return L"mkc_GetHashVersion";
	case mkc_PreCreateDisable:		return L"mkc_PreCreateDisable";
	case mkc_PreCreateEnable:		return L"mkc_PreCreateEnable";
	case mkc_GetHashSize:			return L"mkc_GetHashSize";
	case mkc_QueryFileHandleHash:	return L"mkc_QueryFileHandleHash";
	case mkc_FilterEvent:			return L"mkc_FilterEvent";
	case mkc_FidBox2Get_ByHandle:	return L"mkc_FidBox2Get_ByHandle";
	case mkc_KLFltDev_SetRule:		return L"mkc_KLFltDev_SetRule";
	case mkc_KLFltDev_GetRulesSize:	return L"mkc_KLFltDev_GetRulesSize";
	case mkc_KLFltDev_GetRules:		return L"mkc_KLFltDev_GetRules";
	case mkc_KLFltDev_ApplyRules:	return L"mkc_KLFltDev_ApplyRules";
	case mkc_AddInvProcessByPid:	return L"mkc_AddInvProcessByPid";
	case mkc_Virt_AddApplToSB:		return L"mkc_Virt_AddApplToSB";
	case mkc_Virt_Reset:			return L"mkc_Virt_Reset";
	case mkc_Virt_Apply:			return L"mkc_Virt_Apply";
	case mkc_TerminateProcess:		return L"mkc_TerminateProcess";
	case mkc_IsInvisibleProcess:	return L"mkc_IsInvisibleProcess";
	case mkc_DisconnectAllClients:	return L"mkc_DisconnectAllClients";
	case mkc_GetStatCounter:		return L"mkc_GetStatCounter";
	case mkc_GetFidBox2:			return L"mkc_GetFidBox2";
	case mkc_CreateFile:			return L"mkc_CreateFile";
	case mkc_ReadFile:				return L"mkc_ReadFile";
	case mkc_CloseFile:				return L"mkc_CloseFile";
	case mkc_TimingGet:				return L"mkc_TimingGet";
	}

	return L"<wrong request>";
}

NTSTATUS
TerminateProcessImp (
	HANDLE ProcessId
	)
{
	NTSTATUS status;
	HANDLE hProcess = NULL;

	OBJECT_ATTRIBUTES oa;
	CLIENT_ID ClientId;

	InitializeObjectAttributes( &oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
	ClientId.UniqueProcess = ProcessId;
	ClientId.UniqueThread = NULL;

	status = ZwOpenProcess( &hProcess, PROCESS_TERMINATE, &oa, &ClientId );
	if (NT_SUCCESS( status ))
	{
		status = ZwTerminateProcess( hProcess, STATUS_FATAL_APP_EXIT );
		DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Terminate process %p - status %!STATUS!",
			hProcess,
			status );

		ZwClose( hProcess );
	}
	else
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Open process for terminate process %p - status %!STATUS!",
			hProcess,
			status );
	}

	return status;
}

NTSTATUS
Comm_CheckParams (
	__in PMKLIF_EVENT_TRANSMIT pEventTr
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER( pEventTr );
#if DBG != 0
	try {
		if (pEventTr->m_EventHdr.m_ParamsCount)
		{
			ULONG ParamsCount = 0;
			PSINGLE_PARAM pSingleParam = NULL;
			PUCHAR pSingleParamPtr = pEventTr->m_SingleParams;
			do
			{
				pSingleParam = (PSINGLE_PARAM) pSingleParamPtr;

				DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Check eventtr: param %d flags 0x%x, size %d",
					ParamsCount,
					pSingleParam->ParamFlags,
					pSingleParam->ParamSize
					);
				
				if (FlagOn( pSingleParam->ParamFlags, _SINGLE_PARAM_FLAG_POINTER) )
					pSingleParamPtr += sizeof(SINGLE_PARAM) + sizeof(DWORD);
				else
					pSingleParamPtr += sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;

				ParamsCount++;
			} while (ParamsCount < pEventTr->m_EventHdr.m_ParamsCount);
		}
	} except( CheckException() ) {
		status = GetExceptionCode();
		_dbg_break_;
	}

	DoTraceEx( TRACE_LEVEL_INFORMATION, DC_COMM, "Check eventtr result %!STATUS! (params count %d)",
		status,
		pEventTr->m_EventHdr.m_ParamsCount
		);

#endif // DBG

	return status;
}

NTSTATUS
Comm_CheckOutputBuffer (
    __in PVOID OutputBuffer,
    __in ULONG OutputBufferSize,
	__in ULONG MinSize
	)
{
	if (!OutputBuffer)
		return STATUS_INVALID_PARAMETER;

#if defined(_WIN64)
	if (IoIs32bitProcess( NULL ))
	{
		if (!IS_ALIGNED(OutputBuffer, sizeof(ULONG)))
		{
			//_dbg_break_;
			//return STATUS_DATATYPE_MISALIGNMENT;
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "OutputBuffer not aligned %p",  OutputBuffer );
		}
	}
	else
	{
#endif
		if (!IS_ALIGNED(OutputBuffer, sizeof(PVOID)))
		{
			//_dbg_break_;
			//return STATUS_DATATYPE_MISALIGNMENT;
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "OutputBuffer not aligned %p",  OutputBuffer );
		}
#if defined(_WIN64)
	}
#endif

	if (OutputBufferSize < MinSize)
		return STATUS_BUFFER_TOO_SMALL;

	return STATUS_SUCCESS;
}

NTSTATUS
Comm_CopyDataToUserBuffer (
	__in PVOID OutputBuffer,
	__in ULONG OutputBufferSize,
	__in PVOID pSource,
	__in ULONG SourceLen,
	__inout PULONG ReturnOutputBufferLength
	)
{
	NTSTATUS status = STATUS_BUFFER_TOO_SMALL;

	*ReturnOutputBufferLength = 0;
	if (!OutputBuffer)
	{
		_dbg_break_;
		return STATUS_INVALID_PARAMETER;
	}

	if (OutputBufferSize < SourceLen)
		return STATUS_BUFFER_TOO_SMALL;

	try {
		memcpy( OutputBuffer, pSource, SourceLen );
		*ReturnOutputBufferLength = SourceLen;
		status = STATUS_SUCCESS;
	} except( CheckException() ) {
		status = GetExceptionCode();
		_dbg_break_;
	}

	return status;
}

NTSTATUS
Comm_CopyUserBufferToInternal (
	__in PVOID pInternalPtr,
	__in PVOID pUserBuffer,
	__in ULONG Size
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	try {
		memcpy( pInternalPtr, pUserBuffer, Size );
	} except( CheckException() ) {
		status = GetExceptionCode();
		_dbg_break_;
	}
	return status;
}

NTSTATUS
Comm_GetIncomeHandlePart (
	__in PMKLIF_COMMAND_MESSAGE pCommand,
	__in ULONG InputBufferSize,
	__out PHANDLE phHandle,
	__out PULONG pHandleSize
	)
{
	*phHandle = NULL;
	*pHandleSize = sizeof(HANDLE);

#if defined(_WIN64)
	if (IoIs32bitProcess( NULL ))
		*pHandleSize = sizeof(ULONG);
#endif

	if (InputBufferSize < (sizeof(MKLIF_COMMAND_MESSAGE) + *pHandleSize))
	{
		_dbg_break_;
		return STATUS_INVALID_PARAMETER;
	}
	try {
#if defined(_WIN64)
		if (IoIs32bitProcess( NULL ))
			*phHandle = UlongToHandle(*(ULONG*) pCommand->m_Data);
		else
			*phHandle = *(HANDLE*) pCommand->m_Data;
#else
		*phHandle = *(HANDLE*) pCommand->m_Data;
#endif
	} except( CheckException() ) {
		NTSTATUS status = GetExceptionCode();
		_dbg_break_;
		return status;
	}

	if (((HANDLE)-1 == *phHandle) || (!*phHandle))
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

NTSTATUS
Comm_GetIncomeHandle (
	__in PMKLIF_COMMAND_MESSAGE pCommand,
	__in ULONG InputBufferSize,
	__out PHANDLE phHandle
	)
{
	*phHandle = NULL;
	try {
		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE)))
			*phHandle = *(HANDLE*) pCommand->m_Data;
		else if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)))
			*phHandle = UlongToHandle(*(ULONG*) pCommand->m_Data);
		else
		{
			_dbg_break_;
			return STATUS_INVALID_PARAMETER; // unknown handle size
		}
	} except( CheckException() ) {
		NTSTATUS status = GetExceptionCode();
		_dbg_break_;
		return status;
	}

	if (((HANDLE)-1 == *phHandle) || (!*phHandle))
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

NTSTATUS
Comm_GetIncomeString (
	__in PVOID pStartPtr,
	__in USHORT StrLen,
	__out PWCHAR* ppwchStr
	)
{
	NTSTATUS status = STATUS_NO_MEMORY;
	*ppwchStr = NULL;

	*ppwchStr = ExAllocatePoolWithTag( PagedPool, StrLen, tag_lldisk );
	if (*ppwchStr)
	{
		status = Comm_CopyUserBufferToInternal( *ppwchStr, pStartPtr, StrLen );
		if (!NT_SUCCESS( status ))
		{
			ExFreePool( *ppwchStr );
			*ppwchStr=NULL;
		}
	}

	return status;
}

NTSTATUS Comm_ObjectRequest (
	__in PCOMM_CONTEXT pPortContext,
	__in PMKLIF_OBJECT_REQUEST pRequest,
    __out_bcount_part_opt(OutputBufferSize,*ReturnOutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferSize,
    __out PULONG ReturnOutputBufferLength
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	PEV_LISTITEM pListItem = NULL;

	PFILE_OBJECT pFO = NULL;
	PFLT_INSTANCE pInstance = NULL;

	PMKAV_TRANSACTION_CONTEXT pTransactionContext = NULL;
	ULONG SectorSize = 0;
	BOOLEAN bCached = FALSE;

	Timing_Def;
	Timing_In;

	AcquireResourceExclusive( &pPortContext->m_SyncLock );

	pListItem = Comm_FindEvent( pPortContext, pRequest->m_DrvMark );
	if (!pListItem)
	{
		status = STATUS_NOT_FOUND;

		DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "event request for mark 0x%x not found", pRequest->m_DrvMark );
		
		ReleaseResource( &pPortContext->m_SyncLock );

		return status;
	}
	else
	{
		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "event request for mark 0x%x found", pRequest->m_DrvMark );
	}
	
	switch(pRequest->m_RequestType)
	{
	case _object_request_get_proc_path:
	case _object_request_get_proc_name:
	case _object_request_set_verdict:
	case _object_request_set_preverdict:
	case _object_request_impersonate_thread:
		// critical section will be free after processing
		break;

	default:
		if (pListItem->m_pEventObj)
		{
			PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
			PCFLT_RELATED_OBJECTS FltObjects = NULL;

			PEVENT_OBJ pEvInfo = pListItem->m_pEventObj;

			if (FlagOn( pEvInfo->m_Flags, _EVOF_CACHE_AVAIL ))
				bCached = TRUE;

			pInstanceContext = (PMKAV_INSTANCE_CONTEXT) pEvInfo->m_pInstanceContext;
			FltObjects = (PCFLT_RELATED_OBJECTS) pEvInfo->m_pRelatedObject;

			if (FltObjects)
			{
				status = ObReferenceObjectByPointer (
					FltObjects->FileObject,
					STANDARD_RIGHTS_REQUIRED,
					*IoFileObjectType,
					KernelMode
					);
				
				if (NT_SUCCESS( status ))
					pFO = FltObjects->FileObject;

				status = FltObjectReference( FltObjects->Instance );
				if (NT_SUCCESS( status ))
					pInstance = FltObjects->Instance;

				if (_object_request_get_enlisted_objects == pRequest->m_RequestType && FltObjects->Transaction)
					pTransactionContext = LookupTransactionContext( FltObjects->Transaction );
			}
			
			if (pInstanceContext)
				SectorSize = pInstanceContext->m_SectorSize;
		}

		ReleaseResource( &pPortContext->m_SyncLock );
		break;
	}

	switch (pRequest->m_RequestType)
	{
	case _object_request_impersonate_thread:
		{
			status = STATUS_NOT_SUPPORTED;
			if (pListItem->m_pClientContext)
				status = Security_ImpersonateClient( pListItem->m_pClientContext, PsGetCurrentThread() );

			if (!NT_SUCCESS( status ))
			{
				DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "impersonate status %!STATUS!", status );
			}

			ReleaseResource( &pPortContext->m_SyncLock );
		}
		break;

	case _object_request_get_size:
		status = STATUS_NOT_SUPPORTED;
		if (pInstance && pFO)
		{
			FILE_STANDARD_INFORMATION stdinfo;
			status = MySynchronousQueryInformationFile (
				pInstance,
				pFO,
				&stdinfo,
				sizeof(stdinfo),
				FileStandardInformation,
				NULL
				);

			if (NT_SUCCESS(status))
			{
				if (OutputBufferSize < sizeof(LARGE_INTEGER) || !OutputBuffer)
					status = STATUS_INVALID_PARAMETER;
				else
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&stdinfo.EndOfFile,
						sizeof(stdinfo.EndOfFile),
						ReturnOutputBufferLength
						);
				}
			}
		}

		break;

	case _object_request_read:
		if (!OutputBufferSize || !OutputBuffer)
			status = STATUS_INVALID_PARAMETER;
		if (!pInstance || !pFO || !SectorSize)
			status = STATUS_NOT_SUPPORTED;
		else
		{
			PVOID buffer = NULL;
			ULONG new_size;
			ULONG offset_allign;
			ULONG BytesRead = 0;

			ResolveAlign (
				SectorSize,
				pRequest->m_Offset.LowPart,
				OutputBufferSize,
				&offset_allign,
				&new_size
				);

			pRequest->m_Offset.LowPart -= offset_allign;

			buffer = FltAllocatePoolAlignedWithTag( pInstance, PagedPool, new_size, tag_read_buf );
			if (!buffer)
				status = STATUS_NO_MEMORY;
			else
			{
				// todo: cached when possible
				ULONG ioflags = FLTFL_IO_OPERATION_DO_NOT_UPDATE_BYTE_OFFSET;
				if (!bCached)
					ioflags |= FLTFL_IO_OPERATION_NON_CACHED;
				
				status = FltReadFile( pInstance, pFO, 
					&pRequest->m_Offset, new_size, buffer, ioflags, &BytesRead, NULL, NULL); 

				if (NT_SUCCESS(status))
				{
					if (offset_allign >= BytesRead)
					{
						// read but data not exist
					}
					else
					{
						ULONG read_len;
						BytesRead -= offset_allign;
						read_len = BytesRead > OutputBufferSize ? OutputBufferSize : BytesRead;

						if ( read_len > OutputBufferSize)
						{
							_dbg_break_;
							read_len = 0;
						}

						try {
							memcpy(OutputBuffer, (PCHAR) buffer + offset_allign, read_len);
							*ReturnOutputBufferLength = read_len;
						} except( CheckException() ) {
							status = GetExceptionCode();
							_dbg_break_;
						}
					}
				}
				else
				{
					DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "read error %!STATUS!", status );
				}
							
				FltFreePoolAlignedWithTag( pInstance, buffer, tag_read_buf );

				DoTraceEx( TRACE_LEVEL_INFORMATION, DC_COMM, "read result %!STATUS!, retlen %d (requested %d)",
					status,
					*ReturnOutputBufferLength,
					OutputBufferSize
					);
			}
		}
		break;

	case _object_request_get_proc_name:
		status = STATUS_NOT_SUPPORTED;
		if (pListItem->m_pwchProcessPath)
		{
			ULONG pos = pListItem->m_ProcessPathLenB / sizeof(WCHAR);
			if (pos)
				pos--;
			while(pos)
			{
				if (L'\\' == pListItem->m_pwchProcessPath[pos])
				{
					pos++;
					break;
				}

				pos--;
			}

			status = STATUS_NOT_FOUND;
			if (pos)
			{
				ULONG len = pListItem->m_ProcessPathLenB - pos * sizeof(WCHAR);
				if (!OutputBuffer || len > OutputBufferSize)
					status = STATUS_BUFFER_TOO_SMALL;
				else
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&pListItem->m_pwchProcessPath[pos],
						len,
						ReturnOutputBufferLength
						);
				}
			}
		}
		ReleaseResource( &pPortContext->m_SyncLock );
		break;

	case _object_request_get_proc_path:
		status = STATUS_NOT_FOUND;
		
		if (pListItem->m_pwchProcessPath)
		{
			if (!OutputBuffer || pListItem->m_ProcessPathLenB > OutputBufferSize)
				status = STATUS_BUFFER_TOO_SMALL;
			else
			{
				try {
					memcpy( OutputBuffer, pListItem->m_pwchProcessPath, pListItem->m_ProcessPathLenB );
					*ReturnOutputBufferLength = pListItem->m_ProcessPathLenB;
					status = STATUS_SUCCESS;
				} except( CheckException() ) {
					status = GetExceptionCode();
					_dbg_break_;
				}
			}
		}
		
		ReleaseResource( &pPortContext->m_SyncLock );
		break;

	case _object_request_set_verdict:
		{
			PMKLIF_REPLY_EVENT pVerdict = (PMKLIF_REPLY_EVENT) pRequest->m_Buffer;
			HANDLE ProcessId = UlongToHandle(pListItem->m_pEventTr->m_EventHdr.m_ProcessId);

			MKLIF_SETVERDICT_RESULT VerdictResult;

			RtlZeroMemory( &VerdictResult, sizeof(VerdictResult) );
			VerdictResult.m_Status = STATUS_SUCCESS;

			*pListItem->m_pEvVerdict = *pVerdict;
			if (FlagOn(pListItem->m_pEvVerdict->m_VerdictFlags, efVerdict_TerminateProcess ))
			{
				// teminate on this thread
				pListItem->m_pEvVerdict->m_VerdictFlags &= ~efVerdict_TerminateProcess;
			}
			
			if (FlagOn(pListItem->m_pEvVerdict->m_VerdictFlags, efVerdict_Pending ))
			{
				DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Set event (pending) mark 0x%x",
					pListItem->m_pEventTr->m_EventHdr.m_DrvMark
					);

				KeSetEvent( &pListItem->m_KEventPending, 0, FALSE );
			}
			else
			{
				DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Set event mark 0x%x",
					pListItem->m_pEventTr->m_EventHdr.m_DrvMark
					);

				KeSetEvent( &pListItem->m_KEvent, 0, FALSE );
			}

			ReleaseResource( &pPortContext->m_SyncLock );

			if (FlagOn( pVerdict->m_VerdictFlags, efVerdict_TerminateProcess ))
			{
				VerdictResult.m_Status = TerminateProcessImp( ProcessId );

				if (OutputBuffer && (OutputBufferSize >= sizeof(VerdictResult)))
				{
					try {
						*(PMKLIF_SETVERDICT_RESULT)OutputBuffer = VerdictResult;
						*ReturnOutputBufferLength = sizeof(VerdictResult);
					} except( CheckException() ) {
						GetExceptionCode();
						_dbg_break_;
					}
				}
			}
		}
		break;

	case _object_request_set_preverdict:
		{
			PMKLIF_REPLY_EVENT pVerdict = (PMKLIF_REPLY_EVENT) pRequest->m_Buffer;
			ULONG BlockedFlags = efVerdict_Pending
				| efVerdict_TerminateProcess
				| efVerdict_TerminateThread
				| efVerdict_Quarantine;

			if (BlockedFlags & pVerdict->m_VerdictFlags)
				status = STATUS_NOT_SUPPORTED;
			else
				pListItem->m_pEvVerdict->m_VerdictFlags = pVerdict->m_VerdictFlags;
			
			ReleaseResource( &pPortContext->m_SyncLock );
		}
		break;

	case _object_request_get_enlisted_objects:
		if (!OutputBufferSize || !OutputBuffer)
			status = STATUS_INVALID_PARAMETER;
		
		if (!pInstance || !pFO)
			status = STATUS_NOT_SUPPORTED;
		else
		{
			status = STATUS_TRANSACTION_INVALID_ID;
			if (pTransactionContext)
			{
				PVOID pEnlistedObjects = NULL;
				ULONG EnlistedObjectsLen = 0;

				status = Sys_QueryEnlistedObjects (
					pTransactionContext,
					&pEnlistedObjects,
					&EnlistedObjectsLen
					);

				if (NT_SUCCESS( status ))
				{
					if (sizeof(ULONG) == OutputBufferSize)
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&EnlistedObjectsLen,
							sizeof(EnlistedObjectsLen),
							ReturnOutputBufferLength
							);
					}
					else
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							pEnlistedObjects,
							EnlistedObjectsLen,
							ReturnOutputBufferLength
							);
					}

					ExFreePool( pEnlistedObjects );
				}
			}
		}
		break;

	case _object_request_get_basicinfo:
		status = STATUS_NOT_SUPPORTED;
		if (OutputBufferSize < sizeof(FILE_BASIC_INFORMATION))
			status = STATUS_INVALID_PARAMETER;
		else
		{
			if (!pInstance || !pFO )
				status = STATUS_NOT_SUPPORTED;
			else
			{
				FILE_BASIC_INFORMATION BasicInfo;
				status = FileObject_GetBasicInfo( pInstance, pFO, &BasicInfo );
				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&BasicInfo,
						sizeof(BasicInfo),
						ReturnOutputBufferLength
						);
				}
				else
				{
					DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Get basic inof failed %!STATUS!", status );
				}
			}
		}
		break;
		
	case _object_request_get_fidbox:
		if (!pFO)
			status = STATUS_NOT_SUPPORTED;
		else if (!OutputBuffer || OutputBufferSize <= sizeof(FIDBOX_REQUEST_DATA_EX))
			status = STATUS_INVALID_PARAMETER;
		else if (!Fid_GetItemSize1())
			status = STATUS_REINITIALIZATION_NEEDED;
		else
		{
			PVOID pFidData = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_fid_tmp );
			if (!pFidData)
				status = STATUS_NO_MEMORY;
			else
			{
				ULONG RetSize;
				status = FidBox_GetDataByFO (
					pFO,
					FALSE,
					pFidData,
					OutputBufferSize - sizeof(FIDBOX_REQUEST_DATA_EX),
					&RetSize,
					FALSE
					);

				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						pFidData,
						RetSize,
						ReturnOutputBufferLength
						);
				}

				ExFreePool( pFidData );
			}
		}
		break;
	
	case _object_request_set_fidbox:
		if (!pFO)
			status = STATUS_NOT_SUPPORTED;
		else if (!pRequest->m_Buffer || !pRequest->m_RequestBufferSize)
			status = STATUS_INVALID_PARAMETER;
		else
		{
			PFIDBOX_REQUEST_DATA_EX pFidRequest = ExAllocatePoolWithTag (
				PagedPool,
				sizeof(FIDBOX_REQUEST_DATA_EX) + pRequest->m_RequestBufferSize,
				tag_fid_tmp
				);

			if (!pFidRequest)
			{
				status = STATUS_NO_MEMORY;
				_dbg_break_;
			}
			else
			{
				ULONG FidDataSize = pRequest->m_RequestBufferSize;
				RtlCopyMemory( pFidRequest->m_Buffer, pRequest->m_Buffer, FidDataSize );
				pFidRequest->m_DataFormatId = 0;

				status = FidBox_SetDataByFO( pFO, FALSE, pFidRequest, FidDataSize );

				ExFreePool( pFidRequest );
			}
		}
		break;

	default:
		status = STATUS_NOT_SUPPORTED;
		_dbg_break_;
		break;
	}

	if (pTransactionContext)
		ReleaseFltContext( &pTransactionContext );

	if (pFO)
		ObDereferenceObject( pFO );

	if (pInstance)
		FltObjectDereference( pInstance );

	Timing_Out( _timing_filter, 3, PreProcessing );

	return status;
}

NTSTATUS
Comm_MessageNotify (
    __in PVOID ConnectionCookie,
    __in_bcount_opt(InputBufferSize) PVOID InputBuffer,
    __in ULONG InputBufferSize,
    __out_bcount_part_opt(OutputBufferSize,*ReturnOutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferSize,
    __out PULONG ReturnOutputBufferLength
	)
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;
	PCOMM_CONTEXT pPortContext = (PCOMM_CONTEXT) ConnectionCookie;
	ULONG RequestedApiVersion = 0;
	PMKLIF_COMMAND_MESSAGE pCommand = (PMKLIF_COMMAND_MESSAGE) InputBuffer;
	MKLIF_COMMAND command = mkc_Undefined;

	Timing_Def;
	Timing_In;

	*ReturnOutputBufferLength = 0;

	if (!pPortContext)
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Income message with no context" );
		return STATUS_INVALID_PARAMETER;
	}

	if ((InputBuffer != NULL) &&
		(InputBufferSize >= (FIELD_OFFSET( MKLIF_COMMAND_MESSAGE, m_Command ) +
		sizeof(MKLIF_COMMAND))))
	{
		try {
			RequestedApiVersion = pCommand->m_ApiVersion;
			command = pCommand->m_Command;
		} except( CheckException() ) {
			status = GetExceptionCode();
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Exception 0x%x: Invalid income buffer", status );
			_dbg_break_;
			
			return status;
		}
	}

	if (MKLIF_API_VERSION < RequestedApiVersion)
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Unsupported request version 0x%x", RequestedApiVersion );
		return STATUS_NOT_SUPPORTED;
	}

	if (mkc_ObjectRequest == command)
		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Command %S (0x%x)", Comm_GetCommandInStr( command ), command );
	else
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Command %S (0x%x)", Comm_GetCommandInStr( command ), command );

	if (_SYSTEM_APPLICATION == pPortContext->m_ClientId)
	{
		if ((mkc_GetVersion != command) && (mkc_RegisterClient != command))
		{

			_dbg_break_;
		}
	}

	switch (command)
	{
		case mkc_GetVersion:
		case mkc_RegisterClient:
			break;
		
		default:
		if (_SYSTEM_APPLICATION == pPortContext->m_ClientId)
		{
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "ClientId (%x) is invalid. ignoring command %S", 
				pPortContext->m_ClientId,
				Comm_GetCommandInStr( command )
				);

			command = mkc_Undefined;
		}
		break;
	}

	switch (command)
	{
	case mkc_GetVersion:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (NT_SUCCESS( status ))
		{
			try {
				*(ULONG*) OutputBuffer = MKLIF_API_VERSION;
				*ReturnOutputBufferLength = sizeof(ULONG);
				status = STATUS_SUCCESS;
			} except( CheckException() ) {
		        status = GetExceptionCode();
				_dbg_break_;
			}
		}
		break;
	
	case mkc_RegisterClient:
		if (sizeof(CLIENT_REGISTRATION) + sizeof(MKLIF_COMMAND_MESSAGE) !=  InputBufferSize)
		{
			_dbg_break_;
		}
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(CLIENT_INFO) );

		if (NT_SUCCESS( status ))
		{
			CLIENT_REGISTRATION ClientTmp;
			CLIENT_INFO ClientInfoTmp;
			PCLIENT_REGISTRATION pClient = (PCLIENT_REGISTRATION) pCommand->m_Data;
			try {
				// make client
				DoTraceEx( TRACE_LEVEL_INFORMATION, DC_COMM, "Connect request AppID 0x%x priority %d ClientFlags 0x%x CacheSize %d BSTm %x", 
					pClient->m_AppID,
					pClient->m_Priority,
					pClient->m_ClientFlags,
					pClient->m_CacheSize,
					pClient->m_BlueScreenTimeout );

				memcpy(&ClientTmp, pClient, sizeof(CLIENT_REGISTRATION) );

				status = STATUS_NOT_SUPPORTED;
			} except( CheckException() ) {
		        status = GetExceptionCode();
				_dbg_break_;
			}

			status = STATUS_NOT_SUPPORTED;
			if (RegisterApplication( &ClientTmp, &ClientInfoTmp, pPortContext ))
			{
				status = STATUS_SUCCESS;
				pPortContext->m_ClientId = ClientInfoTmp.m_AppID;

				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					&ClientInfoTmp,
					sizeof(CLIENT_INFO),
					ReturnOutputBufferLength
					);

				if (NT_SUCCESS( status ))
					DoTraceEx( TRACE_LEVEL_INFORMATION, DC_COMM, "Client 0x%x registered", ClientInfoTmp.m_AppID );
			}
		}
		break;
	
	case mkc_ClientStateRequest:
		if(OutputBuffer && (sizeof(APPSTATE) != OutputBufferSize))
		{
			_dbg_break_;
			status = STATUS_INVALID_PARAMETER;
		}
		else if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(APPSTATE_REQUEST)))
		{
			HDSTATE stin, stout;
			APPSTATE_REQUEST StateRequset;

			status = Comm_CopyUserBufferToInternal( &StateRequset, pCommand->m_Data, sizeof(APPSTATE_REQUEST) );

			if (NT_SUCCESS( status ))
			{
				stin.AppID = pPortContext->m_ClientId;
				stin.Activity = StateRequset;
				if (!ClientActivity( &stin, &stout ))
					status = STATUS_UNSUCCESSFUL;
				else
				{
					status = STATUS_SUCCESS;
					if (OutputBuffer)
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&stout.Activity,
							sizeof(APPSTATE),
							ReturnOutputBufferLength
							);
					}
				}
			}
		}
		else if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE)))
		{
			// client yeild
			status = Client_Yeild( pPortContext->m_ClientId );
		}
		break;
	
	case mkc_AddFilter:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (!NT_SUCCESS( status ))
		{
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "mkc_AddFilter - invalid output buffer" );
			break;
		}

		if (InputBufferSize >= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(FILTER_TRANSMIT))
		{
			ULONG FltTransmitSize = 0;
			PFILTER_TRANSMIT pFltIn = NULL;
			try {
				FltTransmitSize = CalcFilterTransmitSize( (PFILTER_TRANSMIT) pCommand->m_Data );
				if (FltTransmitSize)
				{
					pFltIn = ExAllocatePoolWithTag( PagedPool, FltTransmitSize, tag_filter_tr );
					if (pFltIn)
					{
						memcpy( pFltIn, pCommand->m_Data, FltTransmitSize );
						status = STATUS_SUCCESS;
					}
				}
				
			} except( CheckException() ) {
				status = GetExceptionCode();
				_dbg_break_;
			}

			if (NT_SUCCESS( status ) && pFltIn)
			{
				status = FilterTransmit( pFltIn, NULL, 0 );

				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					&pFltIn->m_FilterID,
					sizeof(pFltIn->m_FilterID),
					ReturnOutputBufferLength
					);
			}

			if (pFltIn)
				ExFreePool( pFltIn );
		}

		break;

	case mkc_DelFilter:
		if (InputBufferSize >= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG))
		{
			ULONG FilterId = 0;
			
			status = Comm_CopyUserBufferToInternal( &FilterId, pCommand->m_Data, sizeof(ULONG) );

			if (NT_SUCCESS( status ) && FilterId)
			{
				FILTER_TRANSMIT FltIn;

				memset( &FltIn, 0, sizeof(FltIn) );

				FltIn.m_AppID = pPortContext->m_ClientId;
				FltIn.m_FltCtl = FLTCTL_DEL;
				FltIn.m_FilterID = FilterId;

				status = FilterTransmit( &FltIn, NULL, 0 );
			}
		}

		break;

	case mkc_DelAllFilters:
		{
			FILTER_TRANSMIT FltIn;
			
			memset( &FltIn, 0, sizeof(FltIn) );
			
			FltIn.m_AppID = pPortContext->m_ClientId;
			FltIn.m_FltCtl = FLTCTL_RESET_FILTERS;

			status = FilterTransmit( &FltIn, NULL, 0 );
		}
		
		break;
	
	case mkc_GetEventPart:
		if (!OutputBuffer || !OutputBufferSize)
			break;

		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_GET_EVENT_PART)))
		{
			MKLIF_GET_EVENT_PART EvGetPart;

			status = Comm_CopyUserBufferToInternal( &EvGetPart, pCommand->m_Data, sizeof(MKLIF_GET_EVENT_PART) );

			if (NT_SUCCESS( status ))
			{
				PVOID pBuffer = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_event_part );
				if (!pBuffer)
				{
					status = STATUS_NO_MEMORY;
					break;
				}
				
				status = Comm_EventGetPart( pPortContext, &EvGetPart, pBuffer, OutputBufferSize );
				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						pBuffer,
						OutputBufferSize,
						ReturnOutputBufferLength
						);
				}

				ExFreePool( pBuffer );
			}
		}

		break;
	
	case mkc_AddInvThread:
		status = AddInvisibleThread( PsGetCurrentThreadId() );
		break;

	case mkc_AddInvThreadByHandle:
		{
			HANDLE hThread = NULL;
			status = Comm_GetIncomeHandle( pCommand, InputBufferSize, &hThread );
			if (NT_SUCCESS( status ))
				status = AddInvisibleThread( hThread );
		}
		break;

	case mkc_DelInvThreadByHandle:
		{
			HANDLE hThread = NULL;
			status = Comm_GetIncomeHandle( pCommand, InputBufferSize, &hThread );
			if (NT_SUCCESS( status ))
				status = DelInvisibleThread( hThread, FALSE );
		}
		break;

	case mkc_DelInvThread:
		status = DelInvisibleThread( PsGetCurrentThreadId(), FALSE );
		break;

	case mkc_QueryDrvFlags:
		status = Comm_CopyDataToUserBuffer (
			OutputBuffer,
			OutputBufferSize,
			&Globals.m_DrvFlags,
			sizeof(Globals.m_DrvFlags),
			ReturnOutputBufferLength
			);

		break;

	case mkc_ObjectRequest:
		if (InputBufferSize >= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_OBJECT_REQUEST))
		{
			PMKLIF_OBJECT_REQUEST pRequest;
			ULONG inSize = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);

			pRequest = ExAllocatePoolWithTag( PagedPool, inSize, tag_object_request );
			if (!pRequest)
			{
				status = STATUS_NO_MEMORY;
				break;
			}
			
			status = Comm_CopyUserBufferToInternal( pRequest, pCommand->m_Data, inSize );

			if (NT_SUCCESS( status ))
			{
				status = Comm_ObjectRequest (
					pPortContext,
					pRequest,
					OutputBuffer,
					OutputBufferSize,
					ReturnOutputBufferLength
					);
			}

			ExFreePool( pRequest );
		}

		break;
		
	case mkc_QueryProcessesInfo:
		if (OutputBuffer && OutputBufferSize)
		{
			PVOID pProcessInfo = NULL;
			ULONG ProcessInfoLen = 0;
			status = Sys_GetProcessesInfo( &pProcessInfo, &ProcessInfoLen );
			if (NT_SUCCESS( status ))
			{
				if (sizeof(ULONG) == OutputBufferSize)
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&ProcessInfoLen,
						sizeof(ProcessInfoLen),
						ReturnOutputBufferLength
						);
				}
				else
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						pProcessInfo,
						ProcessInfoLen,
						ReturnOutputBufferLength
						);
				}

				ExFreePool( pProcessInfo );
			}
		}
		break;

	case mkc_QueryModulesInfo:
		if (InputBufferSize >= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)
			&& OutputBuffer
			&& OutputBufferSize
			)
		{
			ULONG ProcessId = 0;

			status = Comm_CopyUserBufferToInternal( &ProcessId, pCommand->m_Data, sizeof(ULONG) );
			
			if (NT_SUCCESS(status) && ProcessId)
			{
				PVOID pModulesInfo = NULL;
				ULONG ModulesInfoLen = 0;
				status = Sys_GetModulesInfo( UlongToHandle( ProcessId ), &pModulesInfo, &ModulesInfoLen );
				if (NT_SUCCESS( status ))
				{
					if (sizeof(ULONG) == OutputBufferSize)
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&ModulesInfoLen,
							sizeof(ModulesInfoLen),
							ReturnOutputBufferLength
							);
					}
					else
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							pModulesInfo,
							ModulesInfoLen,
							ReturnOutputBufferLength
							);
					}

					ExFreePool( pModulesInfo );
				}
			}
		}

		break;

	case mkc_ReadProcessMemory:
		if (InputBufferSize >= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG) + sizeof(LARGE_INTEGER))
		{
			ULONG ProcessId = 0;
			LARGE_INTEGER Offset;

			try {
				ProcessId = *(ULONG*) pCommand->m_Data;
				Offset = *(LARGE_INTEGER*) (pCommand->m_Data + sizeof(ULONG));
			} except( CheckException() ) {
				status = GetExceptionCode();
				ProcessId = 0;
				_dbg_break_;
			}
			
			if (OutputBuffer && OutputBufferSize && ProcessId)
			{
				PVOID pBuffer = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_read_buf );
				ULONG ReadBytes = 0;
				if (!pBuffer)
					status = STATUS_NO_MEMORY;
				else
				{
					status = Sys_ReadProcessMemory( UlongToHandle( ProcessId ), Offset, pBuffer, OutputBufferSize, &ReadBytes );
					if (NT_SUCCESS( status ))
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							pBuffer,
							ReadBytes,
							ReturnOutputBufferLength
							);
					}
					
					ExFreePool( pBuffer );
				}
			}
		}
		break;

	case mkc_SaveFilters:
		status = SaveFiltersForApp( pPortContext->m_ClientId );
		break;

	case mkc_AddInvProcess:
		status = AddInvisibleProcess( PsGetCurrentProcessId(), FALSE );
		break;

	case mkc_AddInvProcessRecursive:
		status = AddInvisibleProcess( PsGetCurrentProcessId(), TRUE );
		break;
	
	case mkc_ReleaseDrvFiles:
		GlobalFidBoxDone( TRUE );
		status = STATUS_SUCCESS;
		break;
	
	case mkc_IsImageActive:
		{
			HANDLE hFile = NULL;
			status = Comm_GetIncomeHandle( pCommand, InputBufferSize, &hFile );

			if (NT_SUCCESS( status ))
				status = Sys_IsImageActive( hFile );
		}
		break;

	case mkc_QueryActiveImages:
		if (OutputBuffer && OutputBufferSize)
		{
			PVOID pActiveImages = NULL;
			ULONG ActiveImagesLen = 0;
			status = Sys_GetActiveImages( &pActiveImages, &ActiveImagesLen );
			if (NT_SUCCESS( status ))
			{
				if (sizeof(ULONG) == OutputBufferSize)
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&ActiveImagesLen,
						sizeof(ActiveImagesLen),
						ReturnOutputBufferLength
						);
				}
				else
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						pActiveImages,
						ActiveImagesLen,
						ReturnOutputBufferLength
						);
				}

				ExFreePool( pActiveImages );
			}
		}
		break;

	case mkc_ClientResetCache:
		status = ClientResetCache( pPortContext->m_ClientId );
		break;

	case mkc_AllowUnload:
		if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_SUPPORT_UNLOAD ))
			status = STATUS_SUCCESS;
		else
			status = STATUS_ACCESS_DENIED;
		break;

	case mkc_DetachDrvFiles:
		GlobalFidBoxDone( FALSE );
		status = STATUS_SUCCESS;
		break;
	
	case mkc_QueryProcessHash:
		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)))
		{
			ULONG ProcessId = 0;
			md5_byte_t digest[16];

			status = Comm_CopyUserBufferToInternal( &ProcessId, pCommand->m_Data, sizeof(ULONG) );
			
			if (NT_SUCCESS( status ))
				status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(digest) );

			if (NT_SUCCESS( status ))
			{
				HANDLE hProcess = UlongToHandle( ProcessId );
				status = Sys_QueryProcessHash( hProcess, digest, sizeof(digest) );
			}

			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					digest,
					sizeof(digest),
					ReturnOutputBufferLength
					);
			}
		}
		break;


	case mkc_QueryFileHash:
		if (InputBufferSize <= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR)))
			status = STATUS_INVALID_PARAMETER;
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(md5_byte_t)*16 );

		if (NT_SUCCESS( status ))
		{
			PWCHAR pwchName = NULL;
			md5_byte_t digest[16];
			ULONG digest_size=sizeof(digest);
			USHORT NameLen = (USHORT) InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);

			status = Comm_GetIncomeString( pCommand->m_Data, NameLen, &pwchName );
			if (!NT_SUCCESS(status))
				pwchName = NULL;
			
			if (NT_SUCCESS( status ))
				status = Sys_CalcFileHash(pwchName, NameLen, digest, &digest_size );

			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					digest,
					digest_size,
					ReturnOutputBufferLength
					);
			}

			if (pwchName)
				ExFreePool(pwchName);
		}
		break;

	case mkc_BreakConnection:
		DisconnectClientByPort( pPortContext );
		ResetClientRulesFromList( pPortContext->m_ClientId );
		RebuildPidTree();
		
		SleepImpM( 100 );
		FltCloseClientPort( Globals.m_Filter, &pPortContext->m_ClientPort );
		
		status = STATUS_SUCCESS; 

		break;

	case mkc_ClientSync:
		{
			BOOLEAN bReset = FALSE;

			AcquireResourceExclusive( &pPortContext->m_SyncLock );
			if (FlagOn( pPortContext->m_Flags, _PORT_FLAG_CLIENT_SYNC ))
				bReset = TRUE;
			ReleaseResource( &pPortContext->m_SyncLock );

			status = ClientSync( pPortContext->m_ClientId, bReset );

			if (NT_SUCCESS( status ))
			{
				AcquireResourceExclusive( &pPortContext->m_SyncLock );

				if (bReset)
					ClearFlag( pPortContext->m_Flags, _PORT_FLAG_CLIENT_SYNC );
				else
					SetFlag( pPortContext->m_Flags, _PORT_FLAG_CLIENT_SYNC );

				ReleaseResource( &pPortContext->m_SyncLock );
			}
		}
		break;

	case mkc_QueryFiltersCount:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (NT_SUCCESS( status ))
		{
			ULONG fc = 0;
			status = Filters_GetCountFor( pPortContext->m_ClientId, &fc );
			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					&fc,
					sizeof(fc),
					ReturnOutputBufferLength
					);
			}

			DoTraceEx( TRACE_LEVEL_INFORMATION, DC_COMM, "QueryFiltersCount %!STATUS! (%d)", 
				status, fc
				);
		}

		break;
	
	case mkc_QueryClientCounter:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(LONG) );
		if (NT_SUCCESS( status ))
		{
			MKLIF_CLIENT_COUNTERS Counter = mkcc_None;

			status = Comm_CopyUserBufferToInternal( &Counter, pCommand->m_Data, sizeof(Counter) );

			if (NT_SUCCESS( status ))
			{
				LONG CounterValue = 0;
				status = Client_GetCounter( pPortContext->m_ClientId, Counter, &CounterValue );
				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&CounterValue,
						sizeof(CounterValue),
						ReturnOutputBufferLength
						);
				}
			}
		}
		break;

	case mkc_FidBoxGet_ByHandle:
		if (!Fid_GetItemSize1())
			status = STATUS_REINITIALIZATION_NEEDED;
		else if (OutputBuffer && OutputBufferSize > sizeof(FIDBOX_REQUEST_DATA_EX))
		{
			HANDLE hFile = NULL;
			status = Comm_GetIncomeHandle( pCommand, InputBufferSize, &hFile );
			if (NT_SUCCESS( status ))
			{
				PFILE_OBJECT pFileObject = NULL;

				status = ObReferenceObjectByHandle (
					hFile,
					0,
					*IoFileObjectType,
					KernelMode,
					&pFileObject,
					NULL
					);
				
				if(NT_SUCCESS( status ))
				{
					PVOID pFidData = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_fid_tmp );
					if (!pFidData)
						status = STATUS_NO_MEMORY;
					else
					{
						ULONG RetSize;
						status = FidBox_GetDataByFO (
							pFileObject,
							TRUE,
							pFidData,
							OutputBufferSize - sizeof(FIDBOX_REQUEST_DATA_EX),
							&RetSize,
							FALSE
							);

						if (NT_SUCCESS( status ))
						{
							status = Comm_CopyDataToUserBuffer (
								OutputBuffer,
								OutputBufferSize,
								pFidData,
								RetSize,
								ReturnOutputBufferLength
								);
						}

						ExFreePool( pFidData );
					}
				
					ObDereferenceObject( pFileObject );
				}
			}
		}
		break;

	case mkc_FidBoxSet_ByHandle:
		{
			HANDLE hFile = NULL;
			ULONG HandleSize = 0;
			
			status = Comm_GetIncomeHandlePart( pCommand, InputBufferSize, &hFile, &HandleSize );
			if (NT_SUCCESS( status ))
			{
				ULONG InFidDataLen = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE) - HandleSize;
				if (InputBufferSize <= sizeof(MKLIF_COMMAND_MESSAGE) + HandleSize )
				{
					status = STATUS_INVALID_PARAMETER;
					_dbg_break_;
				}
				else
				{
					PFILE_OBJECT pFileObject = NULL;
					
					status = ObReferenceObjectByHandle (
						hFile,
						0,
						*IoFileObjectType,
						KernelMode,
						&pFileObject,
						NULL
						);
				
					if(NT_SUCCESS( status ))
					{
						PFIDBOX_REQUEST_DATA_EX pFidRequest = ExAllocatePoolWithTag (
							PagedPool,
							sizeof(FIDBOX_REQUEST_DATA_EX) + InFidDataLen,
							tag_fid_tmp
							);

						if (!pFidRequest)
						{
							status = STATUS_NO_MEMORY;
							_dbg_break_;
						}
						else
						{
							status = Comm_CopyUserBufferToInternal (
									pFidRequest->m_Buffer,
									(PBYTE)(pCommand->m_Data) + HandleSize,
									InFidDataLen
									);
							if(NT_SUCCESS( status ))
							{
								pFidRequest->m_DataFormatId = 0;
								status = FidBox_SetDataByFO (
									pFileObject,
									TRUE,
									pFidRequest,
									InFidDataLen
									);

							}

							ExFreePool( pFidRequest );
						}

						ObDereferenceObject( pFileObject );
					}
				}
			}
		}
		break;

	case mkc_LLD_QueryNames:
		if (OutputBuffer)
		{
			ULONG uNeedSize = 0;
			if (sizeof(ULONG) == OutputBufferSize)
			{
				status = LLD_QueryNames( NULL, 0, &uNeedSize );
				if (NT_SUCCESS(status))
				{
					ULONG NeedSize = (ULONG) uNeedSize;
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&NeedSize,
						sizeof(NeedSize),
						ReturnOutputBufferLength
						);
				}
			}
			else
			{
				PVOID pBuffer = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_lldisk );
				if (!pBuffer)
				{
					status = STATUS_NO_MEMORY;
					break;
				}

				RtlZeroMemory( pBuffer, OutputBufferSize );

				status = LLD_QueryNames( pBuffer, OutputBufferSize, &uNeedSize );
				if (NT_SUCCESS( status ))
				{
					ULONG NeedSize = (ULONG) uNeedSize;
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						pBuffer,
						NeedSize,
						ReturnOutputBufferLength
						);
				}

				ExFreePool( pBuffer );
			}
		}
		break;

	case mkc_LLD_GetInfo:
		if (InputBufferSize <= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR)))
			status = STATUS_INVALID_PARAMETER;
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(MKLIF_LLD_INFO) );

		if (NT_SUCCESS( status ))
		{
			PWCHAR pwchName = NULL;
			USHORT NameLen = (USHORT) InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);

			status = Comm_GetIncomeString( pCommand->m_Data, NameLen, &pwchName );

			if (NT_SUCCESS( status ))
			{
				UNICODE_STRING usName;
				MKLIF_LLD_INFO Info;
				RtlZeroMemory( &Info, sizeof(MKLIF_LLD_INFO) );

				usName.Buffer = pwchName;
				usName.Length = NameLen;
				usName.MaximumLength = NameLen;

				status = LLD_GetInfo( &usName, &Info );
				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&Info,
						sizeof(Info),
						ReturnOutputBufferLength
						);
				}
				
				ExFreePool( pwchName );
			}
		}
		break;

	case mkc_LLD_GetGeometry:
		if (InputBufferSize <= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR)))
			status = STATUS_INVALID_PARAMETER;
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(MKLIF_LLD_GEOMETRY) );

		if (NT_SUCCESS( status ))
		{
			PWCHAR pwchName = NULL;
			USHORT NameLen = (USHORT) InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);

			status = Comm_GetIncomeString( pCommand->m_Data, NameLen, &pwchName );

			if (NT_SUCCESS( status ))
			{
				UNICODE_STRING usName;
				MKLIF_LLD_GEOMETRY Geometry;
				RtlZeroMemory( &Geometry, sizeof(Geometry) );

				usName.Buffer = pwchName;
				usName.Length = NameLen;
				usName.MaximumLength = NameLen;

				status = LLD_GetGeometry( &usName, &Geometry );
				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&Geometry,
						sizeof(Geometry),
						ReturnOutputBufferLength
						);
				}
				
				ExFreePool( pwchName );
			}
		}
		break;

	case mkc_LLD_Read:
		// offset:large_integer
		// volume name - last bytes
		if (InputBufferSize <= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR) + sizeof(__int64))
			|| !OutputBuffer
			|| !OutputBufferSize
			)
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			__int64 Offset = 0L;
			PWCHAR pwchName = NULL;
			USHORT NameLen = (USHORT) InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE) - sizeof(__int64);

			status = Comm_CopyUserBufferToInternal( &Offset, pCommand->m_Data, sizeof(Offset) );

			if (NT_SUCCESS( status ))
				status = Comm_GetIncomeString( (BYTE*)pCommand->m_Data + sizeof(__int64), NameLen, &pwchName );

			if (NT_SUCCESS( status ))
			{
				UNICODE_STRING usName;
				PVOID pBuffer = NULL;

				pBuffer = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_lldisk );
				if (!pBuffer)
					status = STATUS_NO_MEMORY;
				else
				{
					ULONG Bytes = 0;
					usName.Buffer = pwchName;
					usName.Length = NameLen;
					usName.MaximumLength = NameLen;

					status = LLD_Read( &usName, Offset, pBuffer, OutputBufferSize, &Bytes );
					if (NT_SUCCESS( status ))
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							pBuffer,
							Bytes,
							ReturnOutputBufferLength
							);
					}

					ExFreePool( pBuffer );
				}
				
				ExFreePool( pwchName );
			}
		}
		break;

	case mkc_LLD_Write:
		// offset:large_integer
		// size: ulong
		// data - 
		// volume name - last bytes
		if (InputBufferSize <= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR) + sizeof(__int64) + sizeof(ULONG))
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			BYTE* pIncome = pCommand->m_Data;

			PVOID pBuffer = NULL;
			__int64 Offset = 0L;
			ULONG DataSize = 0L;
			PWCHAR pwchName = NULL;
			USHORT NameLen = 0;

			status = Comm_CopyUserBufferToInternal( &Offset, pIncome, sizeof(Offset) );
			pIncome += sizeof(__int64);

			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyUserBufferToInternal ( &DataSize, pIncome, sizeof(DataSize) );
				pIncome += sizeof(ULONG);
			}

			if (!DataSize)
				status = STATUS_INVALID_PARAMETER;

			if (NT_SUCCESS( status ))
			{
				if (InputBufferSize <= sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR) + sizeof(__int64)
					+ sizeof(ULONG) + DataSize)
				{
					_dbg_break_;
					status = STATUS_INVALID_PARAMETER;
				}
			}

			if (NT_SUCCESS( status ))
			{
				pBuffer = ExAllocatePoolWithTag( PagedPool, DataSize, tag_lldisk );
				if (pBuffer)
				{
					status = Comm_CopyUserBufferToInternal( pBuffer, pIncome, DataSize );
					pIncome += DataSize;
				}
				else
					status = STATUS_NO_MEMORY;
			}

			if (NT_SUCCESS( status ))
			{
				NameLen = (USHORT) (InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE) - sizeof(__int64) - sizeof(ULONG) - DataSize);
				status = Comm_GetIncomeString( pIncome, NameLen, &pwchName );
			}

			if (NT_SUCCESS( status ))
			{
				ULONG Bytes = 0;

				UNICODE_STRING usName;
				usName.Buffer = pwchName;
				usName.Length = NameLen;
				usName.MaximumLength = NameLen;

				status = LLD_Write( &usName, Offset, pBuffer, DataSize, &Bytes );

				ExFreePool( pBuffer );

				if (NT_SUCCESS( status ))
				{
					Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&Bytes,
							sizeof(Bytes),
							ReturnOutputBufferLength
							);
				}
			}
		}
		break;

	case mkc_LLD_GetDiskId:
		if (InputBufferSize <= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR)))
			status = STATUS_INVALID_PARAMETER;
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(DISK_ID_INFO) );

		if (NT_SUCCESS( status ))
		{
			PWCHAR pwchName = NULL;
			USHORT NameLen = (USHORT) InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);

			status = Comm_GetIncomeString( pCommand->m_Data, NameLen, &pwchName );

			if (NT_SUCCESS( status ))
			{
				UNICODE_STRING usName;
				DISK_ID_INFO DiskIdInfo;
				RtlZeroMemory( &DiskIdInfo, sizeof(DiskIdInfo) );

				usName.Buffer = pwchName;
				usName.Length = NameLen;
				usName.MaximumLength = NameLen;

				status = LLD_GetDiskId( &usName, &DiskIdInfo );
				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&DiskIdInfo,
						sizeof(DiskIdInfo),
						ReturnOutputBufferLength
						);
				}
				
				ExFreePool( pwchName );
			}
		}
		break;
	
	case mkc_AddApplRule:
		_asm
		{
			int 3
		}

		if ( InputBufferSize < ( sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_APPL_RULE) + sizeof(ULONG) + sizeof(ULONG) ) )
			status = STATUS_INVALID_PARAMETER;
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(LONGLONG) );
		
		if (NT_SUCCESS( status ))
		{
			PMKLIF_APPL_RULE pmklif_appl_rule;
			ULONG	mklif_appl_rule_size;
			
			mklif_appl_rule_size = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);
			pmklif_appl_rule = ExAllocatePoolWithTag( PagedPool, mklif_appl_rule_size, tag_addrule );

			if (!pmklif_appl_rule)
				status = STATUS_NO_MEMORY;

			if (NT_SUCCESS( status ))
				status = Comm_CopyUserBufferToInternal (
									pmklif_appl_rule,
									pCommand->m_Data,
									mklif_appl_rule_size
									);
			
			if (NT_SUCCESS( status ))
			{
				LONGLONG RulID = 0;
				UNICODE_STRING usNativeAppPath;
				UNICODE_STRING usNativeFilePath;
				UNICODE_STRING usValueName;
				PULONG	pStrSize;
				PWCHAR	pStr;
				
				RtlInitEmptyUnicodeString ( &usNativeAppPath, NULL, 0 );
				RtlInitEmptyUnicodeString ( &usNativeFilePath, NULL, 0 );
				RtlInitEmptyUnicodeString ( &usValueName, NULL, 0 );

				pStrSize = (ULONG*)( (char*)pmklif_appl_rule + sizeof(MKLIF_APPL_RULE) + pmklif_appl_rule->m_HashSize );
				pStr = (PWCHAR)(pStrSize + 1);
				
				if (*pStrSize && (((char*)pStr + (*pStrSize)) <= (char*)pmklif_appl_rule + mklif_appl_rule_size))
				{
					RtlInitEmptyUnicodeString( &usNativeAppPath, pStr, (USHORT) *pStrSize );
					usNativeAppPath.Length =(USHORT) *pStrSize;
				}

				pStrSize = (ULONG*) ((char*) pStr + *pStrSize);
				pStr = (PWCHAR) (pStrSize + 1);
				if (*pStrSize && (((char*)pStr + *pStrSize) <= (char*) pmklif_appl_rule + mklif_appl_rule_size))
				{
					RtlInitEmptyUnicodeString( &usNativeFilePath, pStr, (USHORT)*pStrSize );
					usNativeFilePath.Length = (USHORT)*pStrSize;
				}

				pStrSize = (ULONG*) ((char*) pStr + *pStrSize);
				pStr = (PWCHAR) (pStrSize + 1);
				if (*pStrSize && (((char*)pStr + *pStrSize) <= (char*) pmklif_appl_rule + mklif_appl_rule_size))
				{
					RtlInitEmptyUnicodeString( &usValueName, pStr, (USHORT)*pStrSize );
					usValueName.Length = (USHORT)*pStrSize;
				}

				status = InsertAppRuleToList (
							pPortContext->m_ClientId,
							usNativeAppPath,
							usNativeFilePath,
							usValueName,
							pmklif_appl_rule->m_Hash,
							pmklif_appl_rule->m_HashSize,
							pmklif_appl_rule->m_AccessMask,
							&RulID,
							pmklif_appl_rule->m_blockID
							);

				if (NT_SUCCESS( status ))
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&RulID,
						sizeof(LONGLONG),
						ReturnOutputBufferLength
						);
				}
			}

			if (pmklif_appl_rule)
				ExFreePool( pmklif_appl_rule );
		}


		break;

	case mkc_ResetClientRules:
		_asm
		{
			int 3
		}

		if ( InputBufferSize < (sizeof(MKLIF_COMMAND_MESSAGE) ) )
			status = STATUS_INVALID_PARAMETER;
		else
			status = STATUS_SUCCESS;

		if (NT_SUCCESS( status ))
			status = ResetClientRulesFromList( pPortContext->m_ClientId );

		break;
	
	case mkc_ApplyRules:
		_asm
		{
			int 3
		}

		if ( InputBufferSize < (sizeof(MKLIF_COMMAND_MESSAGE) ) )
			status = STATUS_INVALID_PARAMETER;
		else
			status = STATUS_SUCCESS;

		if (NT_SUCCESS( status ))
		{
			status = RebuildPidTree();
		}


		break;

	case mkc_QueryFileNativePath:
		if (InputBufferSize <= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(WCHAR)))
			status = STATUS_INVALID_PARAMETER;
		else
			status = STATUS_SUCCESS;
			//status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize,  );

		if (NT_SUCCESS( status ))
		{
			PWCHAR pwchName = NULL;
			UNICODE_STRING usNativePath;
			USHORT NameLen = (USHORT) InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);

			status = Comm_GetIncomeString( pCommand->m_Data, NameLen, &pwchName );
			if (!NT_SUCCESS( status ))
				pwchName = NULL;
					
			if (NT_SUCCESS( status ))
				status = Sys_BuildNativePath( pwchName, NameLen, &usNativePath );
				
			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					usNativePath.Buffer,
					usNativePath.Length,
					ReturnOutputBufferLength
					);
				
				ExFreePool( usNativePath.Buffer );
			}
			
			if (pwchName)
				ExFreePool(pwchName);
		}
		break;

	case mkc_SpecialFileRequest:
		if (InputBufferSize < (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(SPECFILEFUNC_REQUEST)))
			status = STATUS_INVALID_PARAMETER;
		else
			status = STATUS_SUCCESS;

		if (NT_SUCCESS( status ))
		{
			SPECFILEFUNC_REQUEST SpecialFileRequest;
			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyUserBufferToInternal (
					&SpecialFileRequest,
					pCommand->m_Data,
					sizeof(SPECFILEFUNC_REQUEST)
					);
			}

			if (NT_SUCCESS( status ))
			{
				if (_sfr_get != SpecialFileRequest.m_Request || !OutputBufferSize)
					status = STATUS_INVALID_PARAMETER;
				else
				{
					status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, OutputBufferSize );
					if (NT_SUCCESS( status ))
					{
						PVOID pBuffer = ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_read_buf );
						if (!pBuffer)
							status = STATUS_NO_MEMORY;
						else
						{
							ULONG_PTR RetSize;
							status = SpecFileRequest (
								&SpecialFileRequest,
								sizeof(SpecialFileRequest),
								pBuffer,
								OutputBufferSize,
								&RetSize
								);

							if (NT_SUCCESS( status ))
							{
								if (!RetSize)
									status = STATUS_NOT_FOUND;
								else
								{
									status = Comm_CopyDataToUserBuffer (
										OutputBuffer,
										OutputBufferSize,
										pBuffer,
										(ULONG) RetSize,
										ReturnOutputBufferLength
										);
								}
							}

							ExFreePool( pBuffer );
						}
					}
				}
			}
		}
		break;

	case mkc_FilterGetSize:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (NT_SUCCESS( status ))
		{
			ULONG FilterId;
			status = Comm_CopyUserBufferToInternal( &FilterId, pCommand->m_Data, sizeof(FilterId) );
			if (NT_SUCCESS( status ))
			{
				PQFLT pQFlt = Filters_Find( pPortContext->m_ClientId, FilterId );
				if (!pQFlt)
					status = STATUS_NOT_FOUND;
				else
				{
					ULONG ParamsSize = 0;
					Filters_CalcParamSize( pQFlt, &ParamsSize );
					ParamsSize += sizeof(FILTER_TRANSMIT);

					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&ParamsSize,
						sizeof(ULONG),
						ReturnOutputBufferLength
						);

					Filters_Release( pQFlt );
				}
			}
		}
		break;

	case mkc_FilterGet:
		{
			ULONG FilterId;
			status = Comm_CopyUserBufferToInternal( &FilterId, pCommand->m_Data, sizeof(FilterId) );
			if (NT_SUCCESS( status ))
			{
				PQFLT pQFlt = Filters_Find( pPortContext->m_ClientId, FilterId );
				if (!pQFlt)
					status = STATUS_NOT_FOUND;
				else
				{
					ULONG FilterSize = 0;
					ULONG ParamsSize = 0;
					Filters_CalcParamSize( pQFlt, &ParamsSize );
					FilterSize = ParamsSize + sizeof(FILTER_TRANSMIT);

					status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, FilterSize );
					if (NT_SUCCESS( status ))
					{
						PFILTER_TRANSMIT pFilter = ExAllocatePoolWithTag( PagedPool, FilterSize, tag_filter_tr );
						if (!pFilter)
							status = STATUS_NO_MEMORY;
						else
						{
							Filters_FillOutData( pFilter, pQFlt, ParamsSize );

							status = Comm_CopyDataToUserBuffer (
								OutputBuffer,
								OutputBufferSize,
								pFilter,
								FilterSize,
								ReturnOutputBufferLength
								);

							ExFreePool( pFilter );
						}
					}
					
					Filters_Release( pQFlt );
				}
			}
		}
		break;

	case mkc_FilterGetNextId:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (NT_SUCCESS( status ))
		{
			ULONG FilterId;
			status = Comm_CopyUserBufferToInternal( &FilterId, pCommand->m_Data, sizeof(FilterId) );
			if (NT_SUCCESS( status ))
			{
				PQFLT pQFlt = Filters_FindNext( pPortContext->m_ClientId, FilterId );
				if (!pQFlt)
					status = STATUS_NOT_FOUND;
				else
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&pQFlt->m_FilterID,
						sizeof(ULONG),
						ReturnOutputBufferLength
						);

					Filters_Release( pQFlt );
				}
			}
		}
		break;

	case mkc_GetHashVersion:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(gHashFuncVersion) );
		if (NT_SUCCESS( status ))
		{
			status = Comm_CopyDataToUserBuffer (
				OutputBuffer,
				OutputBufferSize,
				&gHashFuncVersion,
				sizeof(gHashFuncVersion),
				ReturnOutputBufferLength
				);
		}
		break;

	case mkc_PreCreateDisable:
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_PRECREATE_DISABLED );
		status = STATUS_SUCCESS;
		break;

	case mkc_PreCreateEnable:
		ClearFlag( Globals.m_DrvFlags, _DRV_FLAG_PRECREATE_DISABLED );
		status = STATUS_SUCCESS;
		break;

	case mkc_GetHashSize:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (NT_SUCCESS( status ))
		{
			ULONG HashSize = sizeof(md5_byte_t) * 16;
			
			status = Comm_CopyDataToUserBuffer (
				OutputBuffer,
				OutputBufferSize,
				&HashSize,
				sizeof(HashSize),
				ReturnOutputBufferLength
				);
		}
		break;
	
	case mkc_QueryFileHandleHash:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(md5_byte_t) * 16 );
	
		if (NT_SUCCESS( status ))
		{
			HANDLE hFile = NULL;
			ULONG HandleSize = 0;
			
			status = Comm_GetIncomeHandlePart( pCommand, InputBufferSize, &hFile, &HandleSize );
			if (NT_SUCCESS( status ))
			{
				PFILE_OBJECT pFileObject = NULL;
				
				status = ObReferenceObjectByHandle (
					hFile,
					0,
					*IoFileObjectType,
					KernelMode,
					&pFileObject,
					NULL
					);

				if(NT_SUCCESS( status ))
				{
					md5_byte_t Hash[16];
					status = Sys_CalcHash( pFileObject, &Hash, sizeof(Hash) );
					if (NT_SUCCESS( status ))
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&Hash,
							sizeof(Hash),
							ReturnOutputBufferLength
							);
					}
					
					ObDereferenceObject( pFileObject );
				}
			}
		}
		break;

	case mkc_FilterEvent:
		status = STATUS_INVALID_PARAMETER;
		if (InputBufferSize >= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(FILTER_EVENT_PARAM)))
		{
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
			if (NT_SUCCESS( status ))
			{
				ULONG FltEventSize = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);
				PFILTER_EVENT_PARAM pParam = ExAllocatePoolWithTag( PagedPool, FltEventSize, tag_flt_event );
				if (pParam)
				{
					status = Comm_CopyUserBufferToInternal( pParam, pCommand->m_Data, FltEventSize );
					if (NT_SUCCESS( status ))
					{
						if (CheckEventParams( pParam, FltEventSize))
						{
							tefVerdict VerdictFlags = FilterEventImp( pParam, NULL, 0 );
							
							status = Comm_CopyDataToUserBuffer (
								OutputBuffer,
								OutputBufferSize,
								&VerdictFlags,
								sizeof(VerdictFlags),
								ReturnOutputBufferLength
								);
						}
					}

					ExFreePool( pParam );
				}
			}
		}
		break;

	case mkc_ChangeFilterParam:
		status = STATUS_INVALID_PARAMETER;
		if (InputBufferSize >= (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG) + sizeof(FILTER_PARAM)))
		{
			ULONG FilterId = 0;
			ULONG ParamSize = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE) - sizeof(ULONG);

			PFILTER_PARAM pParam = ExAllocatePoolWithTag( PagedPool, ParamSize, tag_flt_event );
			if (pParam)
			{
				status = Comm_CopyUserBufferToInternal( &FilterId, pCommand->m_Data, sizeof(ULONG) );
				if (NT_SUCCESS( status ))
					status = Comm_CopyUserBufferToInternal( pParam, pCommand->m_Data + sizeof(ULONG), ParamSize );
				if (NT_SUCCESS( status ))
				{
					status = Filters_ChangeParams (
						pPortContext->m_ClientId,
						FilterId,
						1,
						pParam
						);
				}

				ExFreePool( pParam );
			}
		}
		break;

	case mkc_FidBox2Get_ByHandle:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(FIDBOX2_REQUEST_DATA) );
		if (NT_SUCCESS( status ))
		{
			HANDLE hFile = NULL;
			status = Comm_GetIncomeHandle( pCommand, InputBufferSize, &hFile );
			if (NT_SUCCESS( status ))
			{
				PFILE_OBJECT pFileObject = NULL;

				status = ObReferenceObjectByHandle (
					hFile,
					0,
					*IoFileObjectType,
					KernelMode,
					&pFileObject,
					NULL
					);
				
				if(NT_SUCCESS( status ))
				{
					FIDBOX2_REQUEST_DATA FB2;
					status = Sys_GetFB2ByFO( pFileObject, &FB2 );

					if (NT_SUCCESS( status ))
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&FB2,
							sizeof(FB2),
							ReturnOutputBufferLength
							);
					}

					ObDereferenceObject( pFileObject );
				}
			}
		}
		break;
	
	case mkc_KLFltDev_SetRule:		
		if ( InputBufferSize < sizeof(MKLIF_COMMAND_MESSAGE) || !OutputBufferSize )
			status = STATUS_INVALID_PARAMETER;
		else 
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, OutputBufferSize );

		if (NT_SUCCESS( status ))
		{
			char *tmpInputBuffer;
			tmpInputBuffer = (char*)ExAllocatePoolWithTag(PagedPool, InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE), tag_comm1 );
			if (!tmpInputBuffer)
				status = STATUS_NO_MEMORY;

			if (NT_SUCCESS( status ))
			{
				
				status = Comm_CopyUserBufferToInternal(
												tmpInputBuffer, 
												pCommand->m_Data, 
												InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE) 
												);
				if (NT_SUCCESS( status ))
				{
					ULONG RetSize;
					char *tmpOutputBuffer;
					tmpOutputBuffer = NULL;
						
					tmpOutputBuffer = (char*)ExAllocatePoolWithTag(PagedPool, OutputBufferSize, tag_comm2 );
					if (!tmpOutputBuffer)
						status = STATUS_NO_MEMORY;

					if ( NT_SUCCESS( status ) )
					{

						status = KLFltDev_SetRule(
							tmpInputBuffer,
							InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE),
							tmpOutputBuffer,
							OutputBufferSize, 
							&RetSize
							);

						if ( NT_SUCCESS( status ) )
						{
							status = Comm_CopyDataToUserBuffer (
								OutputBuffer,
								OutputBufferSize,
								tmpOutputBuffer,
								RetSize,
								ReturnOutputBufferLength
								);	
						}
						ExFreePool(tmpOutputBuffer);
					}
				}
				
				ExFreePool(tmpInputBuffer);
			}
		}
		break;
	
	case mkc_KLFltDev_GetRulesSize:	
		if ( !OutputBufferSize )
			status = STATUS_INVALID_PARAMETER;
		else 
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, OutputBufferSize );

			if ( NT_SUCCESS( status ) )
			{
				ULONG RetSize;
				char *tmpOutputBuffer;
				tmpOutputBuffer = NULL;

				tmpOutputBuffer = (char*)ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_comm3 );
				if (!tmpOutputBuffer)
					status = STATUS_NO_MEMORY;

				if ( NT_SUCCESS( status ) )
				{
					status = KLFltDev_GetRulesSize(
						tmpOutputBuffer,
						OutputBufferSize, 
						&RetSize
						);

					if ( NT_SUCCESS( status ) )
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							tmpOutputBuffer,
							RetSize,
							ReturnOutputBufferLength
							);	
					}
					ExFreePool( tmpOutputBuffer );
				}
			}
		break;
	
	case mkc_KLFltDev_GetRules:		
		if ( !OutputBufferSize )
			status = STATUS_INVALID_PARAMETER;
		else 
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, OutputBufferSize );

		if ( NT_SUCCESS( status ) )
		{
			ULONG RetSize;
			char *tmpOutputBuffer;
			tmpOutputBuffer = NULL;

			tmpOutputBuffer = (char*)ExAllocatePoolWithTag( PagedPool, OutputBufferSize, tag_comm3 );
			if (!tmpOutputBuffer)
				status = STATUS_NO_MEMORY;

			if ( NT_SUCCESS( status ) )
			{
				status = KLFltDev_GetRules(
					tmpOutputBuffer,
					OutputBufferSize, 
					&RetSize
					);

				if ( NT_SUCCESS( status ) )
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						tmpOutputBuffer,
						RetSize,
						ReturnOutputBufferLength
						);	
				}
				ExFreePool( tmpOutputBuffer );
			}
		}
		break;
	
	case mkc_KLFltDev_ApplyRules:
		status = KLFltDev_ApplyRules();
		break;

	case mkc_AddInvProcessByPid:
		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)))
		{
			ULONG ProcessId = 0;
			status = Comm_CopyUserBufferToInternal( &ProcessId, pCommand->m_Data, sizeof(ULONG) );
			
			if (NT_SUCCESS( status ))
			{
				HANDLE hProcess = UlongToHandle( ProcessId );
				status = AddInvisibleProcess( hProcess, FALSE );
			}
		}
		break;

	case mkc_Virt_AddApplToSB:
		//виртуализация
// 		if ( InputBufferSize < ( sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_VIRT_APPL) + sizeof(ULONG) + sizeof(WCHAR) ) )
// 			status = STATUS_INVALID_PARAMETER;
// 		else
// 			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(LONGLONG) );
// 
// 		if (NT_SUCCESS( status ))
// 		{
// 			PMKLIF_VIRT_APPL pmklif_virt_appl;
// 			ULONG	mklif_virt_appl_size;
// 
// 			mklif_virt_appl_size = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);
// 			pmklif_virt_appl = ExAllocatePoolWithTag( PagedPool, mklif_virt_appl_size, tag_virtB );
// 
// 			if (!pmklif_virt_appl)
// 				status = STATUS_NO_MEMORY;
// 
// 			if (NT_SUCCESS( status ))
// 				status = Comm_CopyUserBufferToInternal (
// 				pmklif_virt_appl,
// 				pCommand->m_Data,
// 				mklif_virt_appl_size
// 				);
// 
// 			if (NT_SUCCESS( status ))
// 			{
// 				UNICODE_STRING usNativeAppPath;
// 				UNICODE_STRING usNativeSBPath;
// 				UNICODE_STRING usNativeSBVolName;
// 				PULONG	pStrSize;
// 				PWCHAR	pStr;
// 
// 				RtlInitEmptyUnicodeString ( &usNativeAppPath, NULL, 0 );
// 				RtlInitEmptyUnicodeString ( &usNativeSBPath, NULL, 0 );
// 				
// 				if (pmklif_virt_appl->AppPathLen && (((char*)pmklif_virt_appl->AppPath + (pmklif_virt_appl->AppPathLen)) <= (char*)pmklif_virt_appl + mklif_virt_appl_size))
// 				{
// 					RtlInitEmptyUnicodeString( &usNativeAppPath, pmklif_virt_appl->AppPath, (USHORT) pmklif_virt_appl->AppPathLen );
// 					usNativeAppPath.Length =(USHORT)pmklif_virt_appl->AppPathLen;
// 				}
// 
// 				pStrSize = (ULONG*)( (char*)pmklif_virt_appl->AppPath + pmklif_virt_appl->AppPathLen );
// 				pStr = (PWCHAR)(pStrSize + 1);
// 
// 				if (*pStrSize && (((char*)pStr + *pStrSize) <= (char*) pmklif_virt_appl + mklif_virt_appl_size))
// 				{
// 					RtlInitEmptyUnicodeString( &usNativeSBPath, pStr, (USHORT)*pStrSize );
// 					usNativeSBPath.Length = (USHORT)*pStrSize;
// 				}
// 				
// 				pStrSize = (ULONG*)( (char*)pStr + *pStrSize );
// 				pStr = (PWCHAR)(pStrSize + 1);
// 
// 				if (*pStrSize && (((char*)pStr + *pStrSize) <= (char*) pmklif_virt_appl + mklif_virt_appl_size))
// 				{
// 					RtlInitEmptyUnicodeString( &usNativeSBVolName, pStr, (USHORT)*pStrSize );
// 					usNativeSBVolName.Length = (USHORT)*pStrSize;
// 				}
// 				
// 				status = Virt_InsertApplToList( 
// 					usNativeAppPath,
// 					usNativeSBPath,
// 					usNativeSBVolName,
// 					0
// 					);
// 			}
// 
// 			if (pmklif_virt_appl)
// 				ExFreePool( pmklif_virt_appl );
// 		}
		break;
	
	case mkc_Virt_Reset:
		//виртуализация
		//status = Virt_RemoveAllApp();
		break;
	
	case mkc_Virt_Apply:
	//виртуализация
//		status = Virt_RebuildPidTree();
		break;

	case mkc_TerminateProcess:
		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)))
		{
			ULONG ProcessId = 0;
			status = Comm_CopyUserBufferToInternal( &ProcessId, pCommand->m_Data, sizeof(ULONG) );
			
			if (NT_SUCCESS( status ))
			{
				HANDLE hProcess = UlongToHandle( ProcessId );
				DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Terminate process request %d", ProcessId );
				status = TerminateProcessImp( hProcess );

				DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Terminate process request %d result %!STATUS!",
					ProcessId, status );
			}
		}
		break;

	case mkc_IsInvisibleThread:
		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)))
		{
			ULONG ThreadId = 0;
			status = Comm_CopyUserBufferToInternal( &ThreadId, pCommand->m_Data, sizeof(ULONG) );
			
			if (NT_SUCCESS( status ))
			{
				HANDLE hThread = UlongToHandle( ThreadId );
				BOOLEAN bInvisible = InvThread_IsInvisible( hThread );

				if (!bInvisible)
					status = STATUS_NOT_FOUND;

				DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "invthread %p result %!STATUS!",
					hThread, status );
			}
		}
		break;

	case mkc_IsInvisibleProcess:
		if (InputBufferSize == (sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(ULONG)))
		{
			ULONG ProcessId = 0;
			status = Comm_CopyUserBufferToInternal( &ProcessId, pCommand->m_Data, sizeof(ULONG) );
			
			if (NT_SUCCESS( status ))
			{
				HANDLE hProcess = UlongToHandle( ProcessId );
				BOOLEAN bRecursive = FALSE;
				BOOLEAN bInvisible = InvProc_IsInvisible( hProcess, &bRecursive );

				if (!bInvisible)
					status = STATUS_NOT_FOUND;

				DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "invprocess %p result %!STATUS!",
					hProcess, status );
			}
		}
		break;

	case mkc_DisconnectAllClients:
		status = InvThread_IsInvisible( PsGetCurrentThreadId() );
		if (NT_SUCCESS( status ))
			DisconnectClientByProcID( PsGetCurrentProcessId() );
		break;

	case mkc_GetStatCounter:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(MKLIF_RW_STAT) );
		if (NT_SUCCESS( status ))
		{
			MKLIF_RW_STAT rw_stat;
			
			rw_stat.m_StatCounter = GetStatCounter();
			KeQuerySystemTime( (PLARGE_INTEGER)&rw_stat.m_CurrTime );
	
			status = Comm_CopyDataToUserBuffer (
				OutputBuffer,
				OutputBufferSize,
				&rw_stat,
				sizeof(MKLIF_RW_STAT),
				ReturnOutputBufferLength
				);
		}
		break;

	case mkc_GetFidBox2:
		status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(ULONG) );
		if (NT_SUCCESS( status ))
		{
			HANDLE hFile = NULL;
			ULONG HandleSize = 0;
			
			status = Comm_GetIncomeHandlePart( pCommand, InputBufferSize, &hFile, &HandleSize );
			if (NT_SUCCESS( status ))
			{
				PFILE_OBJECT pFileObject = NULL;
				status = ObReferenceObjectByHandle (
					hFile,
					0,
					*IoFileObjectType,
					KernelMode,
					&pFileObject,
					NULL
					);
				
				if(NT_SUCCESS( status ))
				{
					ULONG Data = 0;
					status = FidBox2_GetByFO (
						pFileObject,
						&Data
						);

					if (NT_SUCCESS( status ))
					{
						status = Comm_CopyDataToUserBuffer (
							OutputBuffer,
							OutputBufferSize,
							&Data,
							sizeof(ULONG),
							ReturnOutputBufferLength
							);
					}

					ObDereferenceObject( pFileObject );
				}
			}
		}
		break;
	
	case mkc_CreateFile:
		if ( InputBufferSize < ( sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_CREATE_FILE)  + sizeof(WCHAR) ) )
			status = STATUS_INVALID_PARAMETER;
		else
			status = Comm_CheckOutputBuffer( OutputBuffer, OutputBufferSize, sizeof(HANDLE) );

		if (NT_SUCCESS( status ))
		{
			PMKLIF_CREATE_FILE pmklif_create_file;
			ULONG	mklif_create_file_size;

			mklif_create_file_size = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);
			pmklif_create_file = ExAllocatePoolWithTag( PagedPool, mklif_create_file_size, tag_comm4 );

			if (!pmklif_create_file)
				status = STATUS_NO_MEMORY;

			if (NT_SUCCESS( status ))
				status = Comm_CopyUserBufferToInternal (
				pmklif_create_file,
				pCommand->m_Data,
				mklif_create_file_size
				);

			if ( NT_SUCCESS( status ) )
			{
 				HANDLE	FileHandle;
				
				if ( IsChLkOn() )
				{
					status = DoDirectOpen (
						&FileHandle,
						pmklif_create_file->NativeFileName,
						(USHORT)(pmklif_create_file->NativeFileNameSize / sizeof(WCHAR))
						);
				}
				else
				{
 					OBJECT_ATTRIBUTES	ObjectAttributes;
					IO_STATUS_BLOCK		IoStatus;
					UNICODE_STRING		FileName;

					RtlInitEmptyUnicodeString(
						&FileName, 
						pmklif_create_file->NativeFileName, 
						(USHORT)pmklif_create_file->NativeFileNameSize 
						);
					FileName.Length = (USHORT)pmklif_create_file->NativeFileNameSize;

					InitializeObjectAttributes( &ObjectAttributes, &FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );
// 					status = FltCreateFile(
// 						Globals.m_Filter,
// 						NULL,
// 						&FileHandle,
// 						pmklif_create_file->DesiredAccess,
// 						&ObjectAttributes,
// 						&IoStatus,
// 						NULL,
// 						pmklif_create_file->FileAttributes,
// 						pmklif_create_file->ShareAccess,
// 						pmklif_create_file->CreateDisposition,
// 						pmklif_create_file->CreateOptions | FILE_NO_INTERMEDIATE_BUFFERING,
// 						NULL,
// 						0,
// 						pmklif_create_file->Flags
// 						);

					status = FltCreateFile(
						Globals.m_Filter,
						NULL,
						&FileHandle,
						FILE_READ_DATA | SYNCHRONIZE,
						&ObjectAttributes,
						&IoStatus,
						NULL,
						0,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						FILE_OPEN,
						FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING,
						NULL,
						0,
						IO_IGNORE_SHARE_ACCESS_CHECK
						);
				}
				
					

				if ( NT_SUCCESS( status ) )
				{
					status = Comm_CopyDataToUserBuffer (
						OutputBuffer,
						OutputBufferSize,
						&FileHandle,
						sizeof(HANDLE),
						ReturnOutputBufferLength
						);

					if ( !NT_SUCCESS( status ) )
						FltClose(FileHandle);	
				}

			}

			if (pmklif_create_file)
				ExFreePool( pmklif_create_file );
		}
		break;
	
	case mkc_ReadFile:
		if ( InputBufferSize < ( sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(MKLIF_READ_FILE) + sizeof(HANDLE) ) )
			status = STATUS_INVALID_PARAMETER;
		else
			status = STATUS_SUCCESS;

		if (NT_SUCCESS( status ))
		{
			PMKLIF_READ_FILE pmklif_read_file;
			ULONG	mklif_read_file_size;

			mklif_read_file_size = InputBufferSize - sizeof(MKLIF_COMMAND_MESSAGE);
			pmklif_read_file = ExAllocatePoolWithTag( PagedPool, mklif_read_file_size, tag_comm5 );

			if (!pmklif_read_file)
				status = STATUS_NO_MEMORY;

			if (NT_SUCCESS( status ))
				status = Comm_CopyUserBufferToInternal (
				pmklif_read_file,
				pCommand->m_Data,
				mklif_read_file_size
				);

			if ( NT_SUCCESS( status ) )
			{
				if ( pmklif_read_file->FileHandleSize != sizeof(HANDLE) )
					status = STATUS_INVALID_PARAMETER;
				
				if ( NT_SUCCESS( status ) )
				{
					IO_STATUS_BLOCK		IoStatus;
					PVOID				buffer;

					ULONG new_size;
					ULONG offset_allign;
					ULONG BytesRead = 0;

					ResolveAlign (
						512,
						pmklif_read_file->ByteOffset,
						OutputBufferSize,
						&offset_allign,
						&new_size
						);

					pmklif_read_file->ByteOffset -= offset_allign;
					buffer = ExAllocatePoolWithTag( PagedPool, new_size, tag_comm6 );

					if (!buffer)
						status = STATUS_NO_MEMORY;

					if ( NT_SUCCESS( status ) )
					{
						if (IsChLkOn())
						{
							PFILE_OBJECT fileObject;

							HANDLE hFile = *(PHANDLE)pmklif_read_file->FileHandle;
							status = ObReferenceObjectByHandle(hFile, FILE_READ_DATA, NULL, KernelMode, (PVOID*) &fileObject, NULL);
							if (NT_SUCCESS(status))
							{
								AddSkipLockItem(fileObject);
								status = ZwReadFile(
									*(PHANDLE)pmklif_read_file->FileHandle,
									NULL,
									NULL,
									NULL,
									&IoStatus,
									buffer,
									new_size,
									(PLARGE_INTEGER)&pmklif_read_file->ByteOffset,
									NULL
									);
								DelSkipLockItem(fileObject);
								ObDereferenceObject( fileObject );
							}
						}
						else
						{
							status = ZwReadFile(
								*(PHANDLE)pmklif_read_file->FileHandle,
								NULL,
								NULL,
								NULL,
								&IoStatus,
								buffer,
								new_size,
								(PLARGE_INTEGER)&pmklif_read_file->ByteOffset,
								NULL
								);
						}

						if ( NT_SUCCESS( status ) )
						{
							ULONG 	BytesRead = (ULONG)IoStatus.Information;

							if (offset_allign >= BytesRead)
							{
								// read but data not exist
							}
							else
							{
								ULONG read_len;
								BytesRead -= offset_allign;
								read_len = BytesRead > OutputBufferSize ? OutputBufferSize : BytesRead;

								if ( read_len > OutputBufferSize)
								{
									_dbg_break_;
									read_len = 0;
								}

								status = Comm_CopyDataToUserBuffer (
									OutputBuffer,
									OutputBufferSize,
									(char *)buffer + offset_allign,
									read_len,
									ReturnOutputBufferLength
									);
							}

						}

						ExFreePool(buffer);
					}
				}
			}

			if (pmklif_read_file)
				ExFreePool( pmklif_read_file );
		}
		break;

	case mkc_CloseFile:
		if ( InputBufferSize < ( sizeof(MKLIF_COMMAND_MESSAGE) + sizeof(HANDLE) ) )
			status = STATUS_INVALID_PARAMETER;
		else
			status = STATUS_SUCCESS;

		if (NT_SUCCESS( status ))
		{
			HANDLE FileHandle;

			status = Comm_CopyUserBufferToInternal (
				&FileHandle,
				pCommand->m_Data,
				sizeof(HANDLE)
				);

			if ( NT_SUCCESS( status ) )
				status = FltClose(FileHandle);		
		}
		break;

	case mkc_TimingGet:
		if (!OutputBuffer || !OutputBufferSize)
			status = STATUS_INVALID_PARAMETER;
		else
		{
			PVOID pBuffer = NULL;
			ULONG BufferSize = 0;
			status = Timing_Get( &pBuffer, &BufferSize );
			if (NT_SUCCESS( status ))
			{
				status = Comm_CopyDataToUserBuffer (
					OutputBuffer,
					OutputBufferSize,
					pBuffer,
					BufferSize,
					ReturnOutputBufferLength
					);

				ExFreePool( pBuffer );
			}
		}
		break;
	
	case mkc_Undefined:
	default:
		break;
	}

	if (NT_SUCCESS(status))
	{
		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Command %S (0x%x) retsize %d", 
			Comm_GetCommandInStr( command ), command, *ReturnOutputBufferLength );
	}
	else
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Command %S (0x%x) result %!STATUS!, retsize %d", 
			Comm_GetCommandInStr( command ), command, status, *ReturnOutputBufferLength );
	}

#if DBG == 0
	// skip check nonclosed critsec
#else
#ifdef WIN2K
	// skip w2k
#else //
	if (KeAreApcsDisabled())
	{
		_dbg_break_;
	}
#endif // WIN2K
#endif // DBG

	Timing_Out( _timing_filter, 2, PreProcessing );

	return status;
}

NTSTATUS
Comm_CreatePort (
	__in PUNICODE_STRING pusDrvName
)
{
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING uniString;
	UNICODE_STRING usDefDrvName;
	PSECURITY_DESCRIPTOR sd;
	NTSTATUS status;

	//  Create a communication port.
	const PWCHAR pwchCommPort = KLIF_COMM_PORT;

	if( !pusDrvName )
		return STATUS_INVALID_PARAMETER;
	
	RtlInitUnicodeString (&usDefDrvName, DEFAULT_DRV_NAME);
	
	if ( RtlCompareUnicodeString(&usDefDrvName, pusDrvName, TRUE) == 0 )
		RtlInitUnicodeString( &uniString,  pwchCommPort );
	else
	{
		RtlInitEmptyUnicodeString( &uniString, pusDrvName->Buffer, pusDrvName->Length );
		uniString.Length = pusDrvName->Length;
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "creating port..." );

    //  We secure the port so only ADMINs & SYSTEM can acecss it.
    status = FltBuildDefaultSecurityDescriptor( &sd, FLT_PORT_ALL_ACCESS );

    if (NT_SUCCESS( status ))
	{
        InitializeObjectAttributes( &oa,
                                    &uniString,
                                    OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                    NULL,
                                    sd );

        status = FltCreateCommunicationPort( Globals.m_Filter,
                                             &Globals.m_ServerPort,
                                             &oa,
                                             NULL,
                                             Comm_PortConnect,
                                             Comm_PortDisconnect,
                                             Comm_MessageNotify,
                                             _COMM_MAX_CONNECTION );

		FltFreeSecurityDescriptor( sd );
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "create port status 0x%x %wZ", status, &uniString );
	return status;
}

VOID
Comm_ClosePort (
	)
{
	DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "closing port..." );
	FltCloseCommunicationPort( Globals.m_ServerPort );
	Globals.m_ServerPort = NULL;
	DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "port closed" );
}

NTSTATUS
Comm_PortConnect (
    __in PFLT_PORT ClientPort,
    __in_opt PVOID ServerPortCookie,
    __in_bcount_opt(SizeOfContext) PVOID ConnectionContext,
    __in ULONG SizeOfContext,
    __deref_out_opt PVOID *ConnectionCookie
    )
{
	PCOMM_CONTEXT pPortContext = NULL;

    PAGED_CODE();

    UNREFERENCED_PARAMETER( ServerPortCookie );
    UNREFERENCED_PARAMETER( ConnectionContext );
    UNREFERENCED_PARAMETER( SizeOfContext );

	pPortContext = ExAllocatePoolWithTag( NonPagedPool, sizeof(COMM_CONTEXT), tag_port );
	if (!pPortContext)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "PortConnect - no free memory. Connect failed" );
	
		return STATUS_NO_MEMORY;
	}

    pPortContext->m_UserProcessId = PsGetCurrentProcessId();
    pPortContext->m_ClientPort = ClientPort;
	pPortContext->m_ClientId = _SYSTEM_APPLICATION;
	pPortContext->m_RefCount = 0;
	pPortContext->m_Flags = _PORT_FLAG_NONE;

	ExInitializeResourceLite( &pPortContext->m_SyncLock );
	InitializeListHead( &pPortContext->m_EvList );

	pPortContext->m_CurrDrvMark = 0;
	
	DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "PortConnect: Context %p Process %p, Port %p",
		pPortContext,
		pPortContext->m_UserProcessId,
		ClientPort );

	*ConnectionCookie = pPortContext;

    return STATUS_SUCCESS;
}

VOID
Comm_PortDisconnect (
    __in PVOID ConnectionCookie
    )
{
	PCOMM_CONTEXT pPortContext = (PCOMM_CONTEXT) ConnectionCookie;
    PAGED_CODE();

	if (!pPortContext)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "PortDisconnect - no context" );
		return;

	}
	DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "PortDisconnect: Process %p, Port %p",  pPortContext->m_UserProcessId, pPortContext->m_ClientPort );

	if (pPortContext->m_ClientPort)
	{
		DisconnectClientByPort( pPortContext );
		ResetClientRulesFromList( pPortContext->m_ClientId );
		RebuildPidTree();
	}

	SetFlag( pPortContext->m_Flags, _PORT_FLAG_DISCONNECTED );

	if (FlagOn( pPortContext->m_Flags, _PORT_FLAG_CLIENT_SYNC ))
		ClientSync( pPortContext->m_ClientId, TRUE );

	//+ release events
	AcquireResourceExclusive( &pPortContext->m_SyncLock );

	if (!IsListEmpty( &pPortContext->m_EvList ))
	{
		PEV_LISTITEM pListItem;
		PLIST_ENTRY Flink = pPortContext->m_EvList.Flink;
		while(Flink != &pPortContext->m_EvList)
		{
			pListItem = CONTAINING_RECORD(Flink, EV_LISTITEM, m_List);

			DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "PortDisconnect: release event mark 0x%x",
				pListItem->m_pEventTr->m_EventHdr.m_DrvMark
				);

			KeSetEvent( &pListItem->m_KEvent, 0, FALSE );

			Flink = Flink->Flink;
		}
	}

	ReleaseResource( &pPortContext->m_SyncLock );
	//- release events

	while (1 != InterlockedIncrement( &pPortContext->m_RefCount ))
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "PortDisconnect: waiting for release..." );
		
		SleepImp( 1 );
		
		InterlockedDecrement( &pPortContext->m_RefCount );
	}

	ExDeleteResourceLite( &pPortContext->m_SyncLock );

	if (pPortContext->m_ClientPort)
		FltCloseClientPort( Globals.m_Filter, &pPortContext->m_ClientPort );

	ExFreePool( pPortContext );
}

VOID
Comm_PortContextLock (
	__in PCOMM_CONTEXT pPortContext
	)
{
	PAGED_CODE();

	if (pPortContext)
		InterlockedIncrement( &pPortContext->m_RefCount );
}

VOID
Comm_PortContextUnLock (
	__in PCOMM_CONTEXT pPortContext
	)
{
	PAGED_CODE();

	if (pPortContext)
	{
		if (InterlockedDecrement( &pPortContext->m_RefCount ) < 0)
		{
			_dbg_break_;
		}
	}
}

PEV_LISTITEM
Comm_PortEventAdd (
	PCOMM_CONTEXT pPortContext,
	PMKLIF_EVENT_TRANSMIT pEventTr,
	PEVENT_OBJ pEventObj,
	PMKLIF_REPLY_EVENT pEvVerdict
	)
{
	PEV_LISTITEM pQueueItem = NULL;
	ULONG QueueItemSize = sizeof(EV_LISTITEM);
	
	pQueueItem = ExAllocatePoolWithTag( NonPagedPool, QueueItemSize, tag_event_part );
	
	if (!pQueueItem)
		return NULL;

	pQueueItem->m_pEventTr = pEventTr;
	pQueueItem->m_pEventObj = pEventObj;

	pQueueItem->m_ProcessPathLenB = 0;
	pQueueItem->m_pwchProcessPath = QueryProcessPath (
		UlongToHandle(pEventTr->m_EventHdr.m_ProcessId),
		&pQueueItem->m_ProcessPathLenB
		);

	pQueueItem->m_pEvVerdict = pEvVerdict;

	KeInitializeEvent( &pQueueItem->m_KEvent, SynchronizationEvent, FALSE );
	KeInitializeEvent( &pQueueItem->m_KEventPending, SynchronizationEvent, FALSE );
	
	pQueueItem->m_pClientContext = ExAllocatePoolWithTag( NonPagedPool, sizeof(SECURITY_CLIENT_CONTEXT), tag_event_part );
	if (pQueueItem->m_pClientContext)
	{
		if (!NT_SUCCESS( Security_CaptureContext( PsGetCurrentThread(), pQueueItem->m_pClientContext )))
		{
			ExFreePool( pQueueItem->m_pClientContext );
			pQueueItem->m_pClientContext = NULL;
		}
	}

	AcquireResourceExclusive( &pPortContext->m_SyncLock );

	if (!pPortContext->m_CurrDrvMark)
		pPortContext->m_CurrDrvMark++;

	pEventTr->m_EventHdr.m_DrvMark = pPortContext->m_CurrDrvMark;

	if (pEventObj)
		pEventTr->m_EventHdr.m_EventFlags |= _EVENT_FLAG_OBJECT_ATTENDED;
	
	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "add event 0x%x-%x-%x mark 0x%x", 
		pEventTr->m_EventHdr.m_HookID,
		pEventTr->m_EventHdr.m_FunctionMj,
		pEventTr->m_EventHdr.m_FunctionMi,
		pEventTr->m_EventHdr.m_DrvMark
		);

	pPortContext->m_CurrDrvMark++;
	
	InsertTailList( &pPortContext->m_EvList, &pQueueItem->m_List );

	ReleaseResource( &pPortContext->m_SyncLock );

	return pQueueItem;
}

VOID
Comm_PortEventRelease (
	PCOMM_CONTEXT pPortContext,
	PEV_LISTITEM pQueueItem
	)
{
	AcquireResourceExclusive( &pPortContext->m_SyncLock );

	RemoveEntryList( &pQueueItem->m_List );

	ReleaseResource( &pPortContext->m_SyncLock );

	ReleaseProcessPath( pQueueItem->m_pwchProcessPath );

	if (pQueueItem->m_pClientContext)
	{
		Security_ReleaseContext( pQueueItem->m_pClientContext );
		
		ExFreePool( pQueueItem->m_pClientContext );
		pQueueItem->m_pClientContext = NULL;
	}

	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "release event 0x%x-%x-%x mark 0x%x", 
		pQueueItem->m_pEventTr->m_EventHdr.m_HookID,
		pQueueItem->m_pEventTr->m_EventHdr.m_FunctionMj,
		pQueueItem->m_pEventTr->m_EventHdr.m_FunctionMi,
		pQueueItem->m_pEventTr->m_EventHdr.m_DrvMark
		);

	ExFreePool( pQueueItem );
}

PEV_LISTITEM
Comm_FindEvent (
	PCOMM_CONTEXT pPortContext,
	ULONG DrvMark
	)
{
	PEV_LISTITEM pListItem;
	PLIST_ENTRY Flink;

	if (IsListEmpty( &pPortContext->m_EvList ))
		return NULL;

	Flink = pPortContext->m_EvList.Flink;

	while(Flink != &pPortContext->m_EvList)
	{
		pListItem = CONTAINING_RECORD(Flink, EV_LISTITEM, m_List);
		Flink = Flink->Flink;

		if (pListItem->m_pEventTr->m_EventHdr.m_DrvMark == DrvMark)
			return pListItem;
	}

	return NULL;
}

NTSTATUS
Comm_EventGetPart (
	PCOMM_CONTEXT pPortContext, 
	PMKLIF_GET_EVENT_PART pEvGetPart,
	PVOID pBuffer,
	ULONG BufferSize
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	PEV_LISTITEM pListItem = NULL;

	AcquireResourceExclusive( &pPortContext->m_SyncLock );

	pListItem = Comm_FindEvent( pPortContext, pEvGetPart->m_DrvMark );
	if (!pListItem)
		status = STATUS_NOT_FOUND;
	else
	{
		if (pListItem->m_pEventTr->m_EventHdr.m_EventSize > BufferSize + pEvGetPart->m_Offset )
		{
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Comm_EventGetPart: invalid offset (0x%x) + size (%d)", pEvGetPart->m_Offset, BufferSize );
			status = STATUS_INVALID_PARAMETER;
			_dbg_break_;
		}
		else
			memcpy( pBuffer, (UCHAR*)(pListItem->m_pEventTr) + pEvGetPart->m_Offset, BufferSize );
	}
	
	ReleaseResource( &pPortContext->m_SyncLock );

	return status;
}

NTSTATUS
Comm_SingleEvent (
	__in PQCLIENT pClient,
	__in PCOMM_CONTEXT pPortContext,
	__in PMKLIF_EVENT_TRANSMIT pEventTr,
	__in PEVENT_OBJ pEventObject,
	__in PFILTER_SUB pFilterSub,
	__inout tefVerdict* pVerdict
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	MKLIF_REPLY_EVENT EvVerdict;
	BOOLEAN bInQueue = FALSE;
	PEV_LISTITEM pQueueItem = NULL;
	ULONG SendSize;
	ULONG ReplyLength = sizeof(EvVerdict);
	PLARGE_INTEGER  pTimeout = NULL;
	LARGE_INTEGER  Timeout;
	LONG tout;

	Timing_Def;

	*pVerdict = efVerdict_Default;

	if (FlagOn( pPortContext->m_Flags, _PORT_FLAG_DISCONNECTED ))
	{
		_dbg_break_;
		return STATUS_PORT_DISCONNECTED;
	}

	Timing_In;

	pQueueItem = Comm_PortEventAdd (  // also filled pEventTr->m_EventHdr.m_DrvMark
		pPortContext,
		pEventTr,
		pEventObject,
		&EvVerdict
		);
	
	if (!pQueueItem)
	{
		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Comm_PortEventAdd - cannot add item! Skip event" );
		InterlockedIncrement( &pClient->m_FaultsSend );
		return STATUS_NO_MEMORY;
	}

	SendSize = pEventTr->m_EventHdr.m_EventSize > sizeof(MKLIF_EVENT) ?
		sizeof(MKLIF_EVENT) : pEventTr->m_EventHdr.m_EventSize;

	if (pEventTr->m_EventHdr.m_Timeout)
	{
		tout = pEventTr->m_EventHdr.m_Timeout;
		*(__int64*)&Timeout=-( tout * 10000000L);

		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Comm_PortEventAdd - event with timeout %d sec", tout );

		pTimeout = &Timeout;
	}

	status = Comm_CheckParams( pEventTr );

	if (NT_SUCCESS( status ))
	{
		if (FlagOn( pFilterSub->m_Flags, FLT_A_SAVE2CACHE )
			&& pFilterSub->m_dwCacheValue)
		{
			DoTraceEx( TRACE_LEVEL_INFORMATION, DC_CACHE, "CacheValue: 0x%x add dirty verdict 0x%x",
				pFilterSub->m_dwCacheValue,
				pFilterSub->m_Verdict
				);

			Obj_CacheAdd( pClient, pFilterSub->m_dwCacheValue, TRUE, pFilterSub->m_Verdict, 0);
		}

		status = FltSendMessage (
			Globals.m_Filter,
			&pPortContext->m_ClientPort,
			pEventTr,
			SendSize,
			&EvVerdict,
			&ReplyLength,
			pTimeout
			);
	}

	Timing_Out( _timing_comm, 0, 0 );

	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "SendMessage mark 0x%x result %!STATUS!",
		pEventTr->m_EventHdr.m_DrvMark,
		status
		);

	if (STATUS_THREAD_IS_TERMINATING == status)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Comm_SingleEvent - thread is terminating, reschedule on other thread" );
		_dbg_break_;
	}

	if (STATUS_TIMEOUT == status)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Comm_SingleEvent - send timeout" );
		InterlockedIncrement( &pClient->m_FaultsSendTimeout );
		_dbg_break_;
	}
	else
	{
		if (NT_SUCCESS( status ))
		{
			if (FlagOn( EvVerdict.m_VerdictFlags,efVerdict_Pending ))
			{
				PVOID  SyncObjects[2];
				
				SyncObjects[0] = &pQueueItem->m_KEvent;
				SyncObjects[1] = &pQueueItem->m_KEventPending;

				while( TRUE )
				{
					pTimeout = NULL;

					if (EvVerdict.m_ExpTime)
					{
						tout = EvVerdict.m_ExpTime;
						*(__int64*)&Timeout=-( (__int64) tout * 10000000L);
						pTimeout = &Timeout;
					}

					DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "begin wait mark 0x%x (timeout %d)",
						pEventTr->m_EventHdr.m_DrvMark,
						tout
						);

					status = KeWaitForMultipleObjects (
						2,
						SyncObjects,
						WaitAny,
						Executive,
						KernelMode,
						FALSE,
						pTimeout,
						NULL
						);

					DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "wait loop mark 0x%x result %!STATUS! (timeout %d)",
						pEventTr->m_EventHdr.m_DrvMark,
						status,
						tout
						);

					if (STATUS_TIMEOUT == status)
					{
						_dbg_break_;
						break; // exit - no new pending request
					}

					if (STATUS_WAIT_0 == status)
					{
						status = STATUS_SUCCESS;
						break; // verdict exists
					}

					if (STATUS_WAIT_1 == status)
					{
						DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "wait loop mark 0x%x pending...",
							pEventTr->m_EventHdr.m_DrvMark
							);

						continue; // wait with new timeout
					}

					// other errors
					break;
				}
			}

			DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "mark 0x%x result %x",
				pEventTr->m_EventHdr.m_DrvMark,
				EvVerdict.m_VerdictFlags
				);

			*pVerdict = EvVerdict.m_VerdictFlags;

			if (FlagOn( EvVerdict.m_VerdictFlags, efVerdict_Cacheable ))
			{
				if (FlagOn( pFilterSub->m_Flags, FLT_A_RESETCACHE ))
				{
					Obj_CacheReset( pClient );
				}
				else if (FlagOn( pFilterSub->m_Flags, FLT_A_DROPCACHE ))
				{
					Obj_CacheDel( pClient, pFilterSub->m_dwCacheValue );
				}
				else if (FlagOn( pFilterSub->m_Flags, FLT_A_SAVE2CACHE ))
				{
					DoTraceEx( TRACE_LEVEL_INFORMATION, DC_CACHE, "CacheValue: 0x%x update verdict 0x%x, timeout %d",
						pFilterSub->m_dwCacheValue,
						*pVerdict,
						EvVerdict.m_ExpTime
						);

					Obj_CacheUpdate( pClient, pFilterSub->m_dwCacheValue, *pVerdict, EvVerdict.m_ExpTime * 1000 );
				}
			}
		}
		else
		{
			DoTraceEx( TRACE_LEVEL_ERROR, DC_COMM, "Comm_SingleEvent - send error %!STATUS!", status );
			_dbg_break_;
		}
	}

	Comm_PortEventRelease( pPortContext, pQueueItem );

	if (STATUS_TIMEOUT == status)
		InterlockedIncrement( &pClient->m_FaultsSubSend );
	
	if (NT_SUCCESS( status ))
	{
		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_COMM, "Comm_SingleEvent status success. Verdict 0x%x", *pVerdict );
	}
	else
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_COMM, "Comm_SingleEvent status: %!STATUS!. Verdict 0x%x",  status, *pVerdict );
	}

	Timing_Out( _timing_comm, 1, 0 );

	return status;
}
