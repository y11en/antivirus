#include "prepfltd.h"
#include "..\kldefs.h"

#include "prepfltd.tmh"

#ifdef WIN2K
#pragma message ("----------------------- w2k compatibility: PsDereferenceImpersonationToken")
#define PsDereferenceImpersonationToken(T)                                          \
            {if (ARGUMENT_PRESENT(T)) {                                       \
                (ObDereferenceObject((T)));                                   \
             } else {                                                         \
                ;                                                             \
             }                                                                \
            }
#endif // WIN2K

#define _add_param( _total_size, _count, _size) if (_size) { _count++; _total_size +=sizeof(SINGLE_PARAM) + _size;}

void
PlaceParam (
	PSINGLE_PARAM* ppSingleParam,
	ULONG ParamId,
	PVOID pParamValue,
	ULONG ParamValueSize,
	ULONG Flags
	)
{
	// укладывает потом сдвигает!
	PSINGLE_PARAM pSingleParam = *ppSingleParam;
	if (!pParamValue || !ParamValueSize)
		return;

	pSingleParam->ParamID = ParamId;
	pSingleParam->ParamFlags = Flags;
	pSingleParam->ParamSize = ParamValueSize;
	RtlCopyMemory( pSingleParam->ParamValue, pParamValue, ParamValueSize);
	
	SINGLE_PARAM_SHIFT( pSingleParam );
	
	*ppSingleParam = pSingleParam;
}

void
PlaceParamWstr (
	PSINGLE_PARAM* ppSingleParam,
	ULONG ParamId,
	PUNICODE_STRING pus,
	ULONG Flags
	)
{
	// укладывает потом сдвигает!
	PSINGLE_PARAM pSingleParam = *ppSingleParam;
	if (!pus->Length)
		return;

	pSingleParam->ParamID = ParamId;
	pSingleParam->ParamFlags = Flags;
	pSingleParam->ParamSize = pus->Length + sizeof(WCHAR);
	RtlCopyMemory( pSingleParam->ParamValue, pus->Buffer, pus->Length);
	((PCHAR)pSingleParam->ParamValue)[pus->Length] = 0;
	((PCHAR)pSingleParam->ParamValue)[pus->Length + 1] = 0;

	SINGLE_PARAM_SHIFT( pSingleParam );

	*ppSingleParam = pSingleParam;
}


#define STR_RESULT_TO_GENTABLE(__r__)	(	((LONG)__r__) < 0L ? GenericLessThan : ( ((LONG)__r__) == 0L ? GenericEqual : GenericGreaterThan)	);

RTL_GENERIC_COMPARE_RESULTS
TransTableCompare_Files (
	PRTL_GENERIC_TABLE  Table,
	PVOID  FirstStruct,
	PVOID  SecondStruct
	)
{
	PMKAV_ENLISTED_FILE pFileEntry1 = (PMKAV_ENLISTED_FILE)FirstStruct;
	PMKAV_ENLISTED_FILE pFileEntry2 = (PMKAV_ENLISTED_FILE)SecondStruct;
	LONG lResult;

	UNREFERENCED_PARAMETER( Table );

	lResult = _wcsicmp(pFileEntry1->m_FileName, pFileEntry2->m_FileName);
	
	return STR_RESULT_TO_GENTABLE(lResult);
}

RTL_GENERIC_COMPARE_RESULTS
TransTableCompare_Keys (
	PRTL_GENERIC_TABLE  Table,
	PVOID  FirstStruct,
	PVOID  SecondStruct
	)
{
	PMKAV_ENLISTED_KEY pKeyEntry1 = (PMKAV_ENLISTED_KEY)FirstStruct;
	PMKAV_ENLISTED_KEY pKeyEntry2 = (PMKAV_ENLISTED_KEY)SecondStruct;
	LONG lResult;

	UNREFERENCED_PARAMETER(Table);

	lResult = _wcsicmp(pKeyEntry1->m_KeyPath, pKeyEntry2->m_KeyPath);
	
	return STR_RESULT_TO_GENTABLE(lResult);
}

PVOID
TransTableAlloc_Common(
	PRTL_GENERIC_TABLE  Table,
	CLONG  ByteSize
	)
{
	UNREFERENCED_PARAMETER(Table);

	return ExAllocatePoolWithTag( PagedPool, ByteSize, tag_table_ev );
}

VOID
TransTableFree_Common(
	PRTL_GENERIC_TABLE  Table,
	PVOID  Buffer
)
{
	UNREFERENCED_PARAMETER(Table);

	ExFreePool(Buffer);
}

//

PMKAV_STREAM_CONTEXT
GetStreamContext (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT FileObject
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PMKAV_STREAM_CONTEXT pContext = NULL;

	status = FltGetStreamContext( pInstance, FileObject, &pContext );

	if (!NT_SUCCESS( status ))
	{
		if (pContext)
		{
			_dbg_break_;
		}

		pContext = NULL;
	}

	return pContext;
}

PMKAV_STREAM_HANDLE_CONTEXT
GetStreamHandleContext (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT FileObject
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext = NULL;

	status = FltGetStreamHandleContext( pInstance, FileObject, &pStreamHandleContext );

	if (!NT_SUCCESS( status ))
	{
		if (pStreamHandleContext)
		{
			_dbg_break_;
		}

		pStreamHandleContext = NULL;
	}

	return pStreamHandleContext;
}

PMKAV_VOLUME_CONTEXT
GetVolumeContextFromFO (
	__in PFILE_OBJECT pFileObject
	)
{
	NTSTATUS status;
	PMKAV_VOLUME_CONTEXT pContext = NULL;
	PFLT_VOLUME pVolume = NULL;
	
	status = FltGetVolumeFromFileObject( Globals.m_Filter, pFileObject, &pVolume );
	if (NT_SUCCESS( status ))
	{
		status = FltGetVolumeContext( Globals.m_Filter, pVolume, &pContext );
		if (!NT_SUCCESS( status ))
			pContext = NULL;

		FltObjectDereference(pVolume);
	}

	return pContext;
}

VOID
ReleaseFltContext (
	PFLT_CONTEXT* ppContext
	)
{
	PFLT_CONTEXT pContext = *ppContext;
	if (pContext)
	{
		FltReleaseContext( pContext );
		*ppContext = NULL;
	}
}


ULONG
GetMiFunction (
	PFLT_CALLBACK_DATA Data
	)
{
	ULONG FuncMi = Data->Iopb->MinorFunction;

	switch(Data->Iopb->MajorFunction)
	{
	case IRP_MJ_SET_INFORMATION:
		FuncMi = Data->Iopb->Parameters.SetFileInformation.FileInformationClass;
		break;

	case IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION:
		FuncMi = 0;
		break;

	default:
		break;
	}

	return FuncMi;
}


PFLT_FILE_NAME_INFORMATION
GetFileNameInfo (
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PMKAV_INSTANCE_CONTEXT pInstanceContext
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;
	Timing_Def;

	Timing_In;
	
	if (IsInvisible( PsGetCurrentThreadId(), 0 ))
		return NULL;

	if (NT_SUCCESS( AddInvisibleThread( PsGetCurrentThreadId() ) ))
	{
		if (!FltObjects)
			status = STATUS_SUCCESS;
		else if (FlagOn( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE ))
			status = STATUS_SUCCESS;
		else
		{
			ULONG len;
			FILE_INTERNAL_INFORMATION file_id;
			
			status = MySynchronousQueryInformationFile (
				FltObjects->Instance,
				FltObjects->FileObject,
				&file_id, sizeof(file_id),
				FileInternalInformation,
				&len
				);
			
			if (NT_SUCCESS( status ))
			{
				file_id.IndexNumber.HighPart &= 0xFFFF;
				if (file_id.IndexNumber.QuadPart < 0x16)
					status = STATUS_OBJECT_NAME_NOT_FOUND; // this is system file dont check it!
			}
			else
			{
				// no id???
				status = STATUS_SUCCESS;
			}
		}
	
		if (NT_SUCCESS( status ))
		{
			status = FltGetFileNameInformation( Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &pNameInfo );
			if (NT_SUCCESS(status))
			{
				NTSTATUS status_tmp = STATUS_UNSUCCESSFUL;

				status_tmp = FltParseFileNameInformation( pNameInfo );
				if (!NT_SUCCESS( status_tmp ))
				{
					DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "reparse name failed %!STATUS!",
									status_tmp
									);
					_dbg_break_;
				}
			}
			else
				pNameInfo = NULL; //todo: no name, skip file?
		}
		
		DelInvisibleThread( PsGetCurrentThreadId(), FALSE );
	}

	Timing_Out( _timing_filter, 1, PostProcessing );

	return pNameInfo;
}

NTSTATUS
SeGetUserLuid (
	PLUID pLuid
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PACCESS_TOKEN  pToken = 0;

	SECURITY_SUBJECT_CONTEXT SubjectContext;
	
	SeCaptureSubjectContext( &SubjectContext );

	if (SubjectContext.ClientToken)
		pToken = SubjectContext.ClientToken;
	else if (SubjectContext.PrimaryToken)
		pToken = SubjectContext.PrimaryToken;

	if (pToken)
		status = SeQueryAuthenticationIdToken( pToken, pLuid );

	SeReleaseSubjectContext( &SubjectContext );

	if (!NT_SUCCESS( status ))
		memset( pLuid, 0, sizeof(LUID) );

	return status;
}

NTSTATUS
SeGetUserSid (
	__in_opt PFLT_CALLBACK_DATA Data,
	__out PSID* ppSid
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PACCESS_TOKEN pAccessToken = NULL;

	BOOLEAN CopyOnOpen;
	BOOLEAN EffectiveOnly;
	SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;

	PTOKEN_USER pTokenUser = NULL;

	if (PsIsThreadTerminating( PsGetCurrentThread() ))
		return STATUS_UNSUCCESSFUL;

	if (Data)
		pAccessToken = PsReferenceImpersonationToken( Data->Thread, &CopyOnOpen, &EffectiveOnly, &ImpersonationLevel );
	
	if (!pAccessToken)
	{
		//why current - not from irp: iogetrequestorprocess?
		pAccessToken = PsReferencePrimaryToken( PsGetCurrentProcess() );
	}

	if (!pAccessToken)
		return STATUS_UNSUCCESSFUL;

	status = SeQueryInformationToken( pAccessToken, TokenUser, &pTokenUser );
	if(NT_SUCCESS( status ))
	{
		ULONG SidLength = RtlLengthSid( pTokenUser->User.Sid );
		if (!SidLength)
			status = STATUS_NOT_SUPPORTED;
		else
		{
			*ppSid = ExAllocatePoolWithTag( PagedPool, SidLength, tag_prepfltd1 );
			if (*ppSid)
			{
				status = RtlCopySid( SidLength, *ppSid, pTokenUser->User.Sid );
				if (!NT_SUCCESS( status ))
				{
					ExFreePool( *ppSid );
					*ppSid = NULL;
				}
			}
		}

		ExFreePool( pTokenUser );
	}

	PsDereferenceImpersonationToken( pAccessToken );

	if (!NT_SUCCESS(status))
	{
		_dbg_break_;
	}

	return status;
}

VOID
ReleaseFileNameInfo (
	PFLT_FILE_NAME_INFORMATION pNameInfo
	)
{
	if (pNameInfo)
		FltReleaseFileNameInformation( pNameInfo );
}

PFLT_FILE_NAME_INFORMATION
LockFileNameInfo (
	PMKAV_STREAM_CONTEXT pContext
	)
{
	PFLT_FILE_NAME_INFORMATION pFileNameInfoTmp = NULL;

	AcquireResourceShared( &Globals.m_ResourceFileNameInfo );

	if (pContext->m_pFileNameInfo)
	{
		FltReferenceFileNameInformation( pContext->m_pFileNameInfo );

		pFileNameInfoTmp = pContext->m_pFileNameInfo;
	}

	ReleaseResource( &Globals.m_ResourceFileNameInfo );

	return pFileNameInfoTmp;
}

VOID
ReplaceFileNameInfo (
	PMKAV_STREAM_CONTEXT pContext,
	PFLT_FILE_NAME_INFORMATION pNewFileNameInfo
	)
{
	PFLT_FILE_NAME_INFORMATION pFileNameInfoTmp = NULL;

	AcquireResourceExclusive( &Globals.m_ResourceFileNameInfo );

	pFileNameInfoTmp = pContext->m_pFileNameInfo;
	pContext->m_pFileNameInfo = pNewFileNameInfo;

	ReleaseResource( &Globals.m_ResourceFileNameInfo );

	ReleaseFileNameInfo( pFileNameInfoTmp );
}

VOID
MapStreamHandleFlags (
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext,
	PULONG pFlags
	)
{
	if (!pStreamHandleContext)
		return;

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_FOR_BACKUP ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_BACKUP);

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_FOR_READ ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_FORREAD );

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_FOR_WRITE ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_FORWRITE );

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_FOR_EXECUTE ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_EXECUTE );

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_FOR_DELETE ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_FORDELETE );

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_NEW_OBJECT ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT );

	if (FlagOn( pStreamHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_DELETE_PENDING ))
		SetFlag( *pFlags, _CONTEXT_OBJECT_FLAG_DELETEPENDING );
}

PFLT_FILE_NAME_INFORMATION
BuildName (
	PFLT_CALLBACK_DATA Data,
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects
	)
{
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = NULL;

	UNICODE_STRING usRelatedFilePath;
	UNICODE_STRING usFN;
	Timing_Def;
	PFILE_OBJECT pFileObject = Data->Iopb->TargetFileObject;

	if (!FltObjects->Instance || !pFileObject || !pInstanceContext || !FltObjects)
		return NULL;
		
	if ( !pFileObject->FileName.Length)
		return NULL;

	Timing_In;

	RtlInitEmptyUnicodeString ( &usRelatedFilePath, NULL, 0 );
	usFN = pFileObject->FileName;
	while(TRUE)
	{
		if (usFN.Length > 4)
		{
			if (L'\\' == usFN.Buffer[0] && L';' == usFN.Buffer[1])
			{
				while (usFN.Length)
				{
					usFN.Length -= sizeof(WCHAR);
					usFN.Buffer++;
					if (!usFN.Length)
						break;
					if (L'\\' == usFN.Buffer[0])
						break;
				}
				continue;
			}
		}
		break;
	}

	if (!usFN.Length)
	{
		_dbg_break_;
		return NULL;
	}

	if ( pFileObject->RelatedFileObject )
	{
		PMKAV_STREAM_CONTEXT pContext = GetStreamContext( FltObjects->Instance, pFileObject->RelatedFileObject );
		if (pContext)
		{
			PFLT_FILE_NAME_INFORMATION pFileNameInfo = LockFileNameInfo( pContext );
			
			if (!pFileNameInfo)
			{
				_dbg_break_;
			}
			else
			{
				usRelatedFilePath.Buffer = ExAllocatePoolWithTag( PagedPool, pFileNameInfo->Name.Length, tag_build_name );
				if ( usRelatedFilePath.Buffer )
				{
					memcpy ( usRelatedFilePath.Buffer, pFileNameInfo->Name.Buffer, pFileNameInfo->Name.Length );
					usRelatedFilePath.Length = pFileNameInfo->Name.Length;
					usRelatedFilePath.MaximumLength = pFileNameInfo->Name.Length;
				}
													
				ReleaseFileNameInfo( pFileNameInfo );
			}
							
			ReleaseFltContext( &pContext );
		}
	}

	if ( pFileObject->RelatedFileObject && !usRelatedFilePath.Buffer )
	{
		Timing_Out( _timing_filter, 1, PreProcessing );
		return NULL;
	}
	
	pFileNameInfo = ExAllocatePoolWithTag( PagedPool, sizeof(FLT_FILE_NAME_INFORMATION), tag_build_name );
	if (pFileNameInfo)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		USHORT Len;
		RtlZeroMemory( pFileNameInfo, sizeof(FLT_FILE_NAME_INFORMATION) );
		if (pFileObject->RelatedFileObject)
		{
			UNICODE_STRING usSlash;
			RtlInitUnicodeString( &usSlash, L"\\" );
			Len = usRelatedFilePath.Length + usFN.Length + sizeof(WCHAR);
			pFileNameInfo->Name.MaximumLength = Len;
			pFileNameInfo->Name.Buffer = ExAllocatePoolWithTag( PagedPool, Len, tag_build_name );
			if (pFileNameInfo->Name.Buffer)
			{
				status = RtlAppendUnicodeStringToString( &pFileNameInfo->Name, &usRelatedFilePath );
				if (NT_SUCCESS( status ))
					status = RtlAppendUnicodeStringToString( &pFileNameInfo->Name, &usSlash );
				if (NT_SUCCESS( status ))
					status = RtlAppendUnicodeStringToString( &pFileNameInfo->Name, &usFN );
			}
		}
		else
		{
			Len = pInstanceContext->m_VolumeName.Length + usFN.Length;
			pFileNameInfo->Name.MaximumLength = Len;
			pFileNameInfo->Name.Buffer = ExAllocatePoolWithTag( PagedPool, Len, tag_build_name );
			if (pFileNameInfo->Name.Buffer)
			{
				status = RtlAppendUnicodeStringToString( &pFileNameInfo->Name, &pInstanceContext->m_VolumeName );
				if (NT_SUCCESS( status ))
					status = RtlAppendUnicodeStringToString( &pFileNameInfo->Name, &usFN );
			}
		}

		if (NT_SUCCESS( status ))
		{
			pFileNameInfo->Volume.Length = pInstanceContext->m_VolumeName.Length;
			pFileNameInfo->Volume.MaximumLength = pInstanceContext->m_VolumeName.Length;
			pFileNameInfo->Volume.Buffer = pFileNameInfo->Name.Buffer; 

			status = FltParseFileNameInformation( pFileNameInfo );
			if (!NT_SUCCESS( status ))
			{
				_dbg_break_;
			}
		}
		else
		{
			_dbg_break_;
			if (pFileNameInfo->Name.Buffer)
				ExFreePool( pFileNameInfo->Name.Buffer );
			ExFreePool( pFileNameInfo );
			pFileNameInfo = NULL;
		}
	}

	if ( usRelatedFilePath.Buffer)
		ExFreePool( usRelatedFilePath.Buffer );

	Timing_Out( _timing_filter, 1, PreProcessing );
	
	return pFileNameInfo;
}

PFILTER_EVENT_PARAM
EvContext_Create (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext,
	BOOLEAN bPreOp
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;

	if (bPreOp)
		pNameInfo = BuildName( Data, pInstanceContext, FltObjects );
	else
		pNameInfo = GetFileNameInfo( Data, FltObjects, pInstanceContext );

	if (g_bTransactionOperationsSupported && FltObjects->Transaction)
		SetFlag( pContext->m_Flags, _STREAM_FLAGS_TRANSACTED );

	if (pNameInfo)
	{
		ULONG ParamsCount = 0;
		ULONG params_size = sizeof(FILTER_EVENT_PARAM);
		PSID pSid = NULL;
		ULONG SidLength = 0;
		PLUID pLuid = NULL;

		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_DRIVER, "%S-create '%wZ",
			bPreOp ? L"pre"  : L"post",
			&pNameInfo->Name
			);

		ParamsCount++;											//1: _PARAM_OBJECT_URL_W + \0
		params_size += sizeof(SINGLE_PARAM) + pNameInfo->Name.Length + sizeof(WCHAR);

		ParamsCount++;
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);		//2: _PARAM_OBJECT_ACCESSATTR

		ParamsCount++;
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);		//3: _PARAM_OBJECT_CONTEXT_FLAGS

		if (pStreamHandleContext)
		{
			if (pStreamHandleContext->m_pSid)						//4: _PARAM_OBJECT_SID 
			{
				pSid = pStreamHandleContext->m_pSid;
				SidLength = RtlLengthSid( pSid );

				ParamsCount++;
				params_size += sizeof(SINGLE_PARAM) + SidLength;
			}

			if (pStreamHandleContext->m_Luid.LowPart || pStreamHandleContext->m_Luid.HighPart)
			{
				pLuid = &pStreamHandleContext->m_Luid;
				ParamsCount++;										//5: _PARAM_OBJECT_LUID
				params_size += sizeof(SINGLE_PARAM) + sizeof(LUID);
			}
		}

		ParamsCount++;												// 6: _PARAM_OBJECT_VOLUME_NAME_W + \0
		params_size += sizeof(SINGLE_PARAM) + pNameInfo->Volume.Length + sizeof(WCHAR);

		if (pNameInfo->Stream.Length)
		{
			ParamsCount++;												// 7: _PARAM_STREAM_W + \0
			params_size += sizeof(SINGLE_PARAM) + pNameInfo->Stream.Length + sizeof(WCHAR);
		}

		ParamsCount++;												// 8: _PARAM_HARDLINK_COUNT + \0
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

		pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;
			FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_NFIOR, IRP_MJ_CREATE, GetMiFunction( Data ), PreProcessing, ParamsCount );
			
			pParam->ProcName[0] = 0;
			//GetProcName( pParam->ProcName, IoThreadToProcess( Data->Thread ) );

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );
			if (pNameInfo->Name.Length)
			{
				memcpy( pSingleParam->ParamValue, pNameInfo->Name.Buffer, pNameInfo->Name.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Name.Length + sizeof(WCHAR);
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, (ULONG) Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess );

			{
				ULONG tmpFlags = _CONTEXT_OBJECT_FLAG_NONE;
				BOOLEAN bIsDirectory = FALSE;

				ULONG disposition = (Data->Iopb->Parameters.Create.Options >> 24) & 0xff; 
				SINGLE_PARAM_SHIFT( pSingleParam );
				if (FILE_CREATE_TREE_CONNECTION & Data->Iopb->Parameters.Create.Options)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_NETWORKTREE;

				if (FILE_OPEN_FOR_BACKUP_INTENT & Data->Iopb->Parameters.Create.Options)
				{
					tmpFlags |= _CONTEXT_OBJECT_FLAG_BACKUP;
					if (pStreamHandleContext)
						pStreamHandleContext->m_Flags |= _STREAM_HANDLE_FLAGS_FOR_BACKUP;
				}

				// if request form network IoIsFileOriginRemote( FltObjects->FileObject )
				if (!bPreOp)
				{
					if (NT_SUCCESS( FltIsDirectory( FltObjects->FileObject, FltObjects->Instance, &bIsDirectory )) )
					{
						if (bIsDirectory)
						{
							SetFlag( pContext->m_Flags, _STREAM_FLAGS_DIRECTORY );
							
							DoTraceEx( TRACE_LEVEL_VERBOSE, DC_DRIVER, "folder '%wZ'",
								&pNameInfo->Name
								);
						}
					}
				}

				if (_STREAM_FLAGS_DIRECTORY & pContext->m_Flags)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_DIRECTORY;
				else
				{
					if (Data->Iopb->Parameters.Create.Options & FILE_DIRECTORY_FILE)
						tmpFlags |= _CONTEXT_OBJECT_FLAG_DIRECTORY;
				}
				
				if ((FILE_OVERWRITE_IF == disposition)
					|| (FILE_OVERWRITE == disposition)
					|| (FILE_CREATE == disposition)
					|| (FILE_SUPERSEDE == disposition))
				{
					tmpFlags |= _CONTEXT_OBJECT_FLAG_CREATENEWOBJECT;
				}
				
				if (Data->Iopb->Parameters.Create.ShareAccess & FILE_SHARE_READ)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_SHAREREAD;
				
				if (Data->Iopb->Parameters.Create.ShareAccess & FILE_SHARE_WRITE)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_SHAREWRITE;
				
				if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess & (FILE_WRITE_DATA | FILE_APPEND_DATA))
				{
					tmpFlags |= _CONTEXT_OBJECT_FLAG_FORWRITE;
					if (pStreamHandleContext)
						pStreamHandleContext->m_Flags |= _STREAM_HANDLE_FLAGS_FOR_WRITE;
				}
				
				if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess & FILE_READ_DATA)
				{
					tmpFlags |= _CONTEXT_OBJECT_FLAG_FORREAD;
					if (pStreamHandleContext)
						pStreamHandleContext->m_Flags |= _STREAM_HANDLE_FLAGS_FOR_READ;
				}
				
				if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess & DELETE)
				{
					tmpFlags |= _CONTEXT_OBJECT_FLAG_FORDELETE | _CONTEXT_OBJECT_FLAG_DELETEPENDING;
					if (pStreamHandleContext)
						pStreamHandleContext->m_Flags |= _STREAM_HANDLE_FLAGS_FOR_DELETE | _STREAM_HANDLE_FLAGS_DELETE_PENDING;
				}

				if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess & FILE_EXECUTE)
				{
					tmpFlags |= _CONTEXT_OBJECT_FLAG_EXECUTE;
					if (pStreamHandleContext)
						pStreamHandleContext->m_Flags |= _STREAM_HANDLE_FLAGS_FOR_EXECUTE;
				}
				
				if (Data->Iopb->OperationFlags & SL_OPEN_TARGET_DIRECTORY)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_SL_OPENTAR_DIR;

				if (Data->Iopb->Parameters.Create.Options & FILE_NO_INTERMEDIATE_BUFFERING)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_BUFFER_ALIGN;

				tmpFlags |= pInstanceContext->m_ContextFlags;
				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, tmpFlags );
			}

			if (SidLength)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_SID );
				pSingleParam->ParamSize = SidLength;
				RtlCopySid( SidLength, pSingleParam->ParamValue, pStreamHandleContext->m_pSid );
			}

			if (pLuid)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_LUID );
				pSingleParam->ParamSize = sizeof(LUID);
				memcpy( pSingleParam->ParamValue, pLuid, sizeof(LUID) );

				DoTraceEx( TRACE_LEVEL_VERBOSE, DC_DRIVER, "CreateAccess luid: 0x%x:%x to '%wZ'",
					pLuid->HighPart,
					pLuid->LowPart,
					&pNameInfo->Name
					);
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W );
			if (pNameInfo->Volume.Length)
			{
				memcpy( pSingleParam->ParamValue, pNameInfo->Volume.Buffer, pNameInfo->Volume.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Volume.Length + sizeof(WCHAR);
			}

			if (pNameInfo->Stream.Length)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_STREAM_W );
				memcpy( pSingleParam->ParamValue, pNameInfo->Stream.Buffer, pNameInfo->Stream.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Stream.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Stream.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Stream.Length + sizeof(WCHAR);
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_HARDLINK_COUNT, pContext->m_NumberOfLinks );
		}

		if (bPreOp)
		{
			ExFreePool( pNameInfo->Name.Buffer );
			ExFreePool( pNameInfo );
		}
		else
			pContext->m_pFileNameInfo = pNameInfo; // bc. pContext on stack (ReplaceFileNameInfo( pContext, pNameInfo ))
	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_Write (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;

	UNREFERENCED_PARAMETER ( FltObjects );
	UNREFERENCED_PARAMETER ( pStreamHandleContext );

	if (pContext)
	{
		pNameInfo = LockFileNameInfo( pContext );
		if (pNameInfo)
		{
			DoTraceEx( TRACE_LEVEL_VERBOSE, DC_DRIVER, "write to '%wZ size %d, status %!STATUS!",
				&pNameInfo->Name,
				(ULONG) Data->IoStatus.Information,
				Data->IoStatus.Status
				);
		}
	}
	
	if (pNameInfo)
	{

		ULONG ParamsCount = 0;
		ULONG params_size = sizeof(FILTER_EVENT_PARAM);
		
		ParamsCount++;											//1: _PARAM_OBJECT_URL_W + \0
		params_size += sizeof(SINGLE_PARAM) + pNameInfo->Name.Length + sizeof(WCHAR);
		
		ParamsCount++;											//2: _PARAM_OBJECT_CONTEXT_FLAGS
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);	

		ParamsCount++;											//3: _PARAM_OBJECT_VOLUME_NAME_W + \0
		params_size += sizeof(SINGLE_PARAM) + pNameInfo->Volume.Length + sizeof(WCHAR);

		ParamsCount++;													//4: _PARAM_HARDLINK_COUNT
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

		pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;
			FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_NFIOR, IRP_MJ_WRITE, GetMiFunction( Data ), PostProcessing, ParamsCount );
			
			pParam->ProcName[0] = 0;
			//GetProcName( pParam->ProcName, IoThreadToProcess( Data->Thread ) );

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );
			if (pNameInfo->Name.Length)
			{
				memcpy( pSingleParam->ParamValue, pNameInfo->Name.Buffer, pNameInfo->Name.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Name.Length + sizeof(WCHAR);
			}

			{
				ULONG tmpFlags = _CONTEXT_OBJECT_FLAG_MODIFIED;
				SINGLE_PARAM_SHIFT( pSingleParam );

				tmpFlags |= pInstanceContext->m_ContextFlags;
				if (_STREAM_FLAGS_DIRECTORY & pContext->m_Flags)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_DIRECTORY;

				MapStreamHandleFlags( pStreamHandleContext, &tmpFlags );

				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, tmpFlags );
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W );
			if (pNameInfo->Volume.Length)
			{
				memcpy( pSingleParam->ParamValue, pNameInfo->Volume.Buffer, pNameInfo->Volume.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Volume.Length + sizeof(WCHAR);
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_HARDLINK_COUNT, pContext->m_NumberOfLinks );
		}

		ReleaseFileNameInfo( pNameInfo );
	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_Common (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext,
	PROCESSING_TYPE ProcessingType
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;

	UNREFERENCED_PARAMETER ( FltObjects );

	if (pContext)
		pNameInfo = LockFileNameInfo( pContext );
	
	if (pNameInfo)
	{
		ULONG ParamsCount = 0;
		ULONG params_size;
		ULONG SidLength = 0;
		PLUID pLuid = NULL;

		ULONG HookId = FLTTYPE_NFIOR;
		ULONG FuncMj = Data->Iopb->MajorFunction;
		ULONG FuncMi = GetMiFunction( Data );

		params_size = sizeof(FILTER_EVENT_PARAM);
		
		ParamsCount++;													//1: _PARAM_OBJECT_URL_W + \0
		params_size += sizeof(SINGLE_PARAM) + pNameInfo->Name.Length + sizeof(WCHAR);

		ParamsCount++;													//2: _PARAM_OBJECT_CONTEXT_FLAGS
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

		if (pStreamHandleContext)
		{
			if(pStreamHandleContext->m_pSid)
			{
				ParamsCount++;											//3: _PARAM_OBJECT_SID 
				SidLength = RtlLengthSid( pStreamHandleContext->m_pSid );
				params_size += sizeof(SINGLE_PARAM) + SidLength;
			}
			
			if (pStreamHandleContext->m_Luid.LowPart || pStreamHandleContext->m_Luid.HighPart)
			{
				pLuid = &pStreamHandleContext->m_Luid;
				ParamsCount++;											//4: _PARAM_OBJECT_LUID
				params_size += sizeof(SINGLE_PARAM) + sizeof(LUID);
			}
		}
		
		ParamsCount++;													// 5: _PARAM_OBJECT_VOLUME_NAME_W + \0
		params_size += sizeof(SINGLE_PARAM) + pNameInfo->Volume.Length + sizeof(WCHAR);

		if (pContext)
		{
			ParamsCount++;													 //6: _PARAM_OBJECT_INTERNALID
			params_size += sizeof(SINGLE_PARAM) + sizeof(LARGE_INTEGER);
		}

		if (IRP_MJ_SET_INFORMATION == FuncMj && FileDispositionInformation == FuncMi)
		{
			ParamsCount++;													//7: delete value
			params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);
		}

		ParamsCount++;													//8: _PARAM_HARDLINK_COUNT
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

		pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;
			
			if (IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION == Data->Iopb->MajorFunction)
			{
				HookId = FLTTYPE_SYSTEM;
				FuncMj = 0x04/*MJ_SYSTEM_CREATE_SECTION*/;
				FuncMi = 0;
			}

			FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, FuncMi, ProcessingType, ParamsCount );
			
			pParam->ProcName[0] = 0;
			//GetProcName(pParam->ProcName, IoThreadToProcess( Data->Thread) );

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
			if (pNameInfo->Name.Length)
			{
				memcpy( pSingleParam->ParamValue, pNameInfo->Name.Buffer, pNameInfo->Name.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Name.Length + sizeof(WCHAR);
			}

			{
				ULONG tmpFlags = _CONTEXT_OBJECT_FLAG_NONE;
				SINGLE_PARAM_SHIFT( pSingleParam );

				if (FlagOn( pContext->m_Flags, _STREAM_FLAGS_MODIFIED ))
					tmpFlags |= _CONTEXT_OBJECT_FLAG_MODIFIED;

				if (FlagOn( pContext->m_Flags, _STREAM_FLAGS_DIRECTORY ))
					tmpFlags |= _CONTEXT_OBJECT_FLAG_DIRECTORY;

				tmpFlags |= pInstanceContext->m_ContextFlags;
				
				MapStreamHandleFlags( pStreamHandleContext, &tmpFlags );

				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, tmpFlags );

				DoTraceEx( TRACE_LEVEL_VERBOSE, DC_FILTER, "CtxFlags 0x%x", tmpFlags );
			}

			if (SidLength)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_SID );
				pSingleParam->ParamSize = SidLength;
				RtlCopySid( SidLength, pSingleParam->ParamValue, pStreamHandleContext->m_pSid );
			}

			if (pLuid)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_LUID );
				pSingleParam->ParamSize = sizeof(LUID);
				memcpy( pSingleParam->ParamValue, pLuid, sizeof(LUID) );
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W );
			if (pNameInfo->Volume.Length)
			{
				memcpy( pSingleParam->ParamValue, pNameInfo->Volume.Buffer, pNameInfo->Volume.Length );
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Volume.Length + sizeof(WCHAR);
			}

			if (pContext)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );				
				SINGLE_PARAM_INIT_LARGEINTEGER( pSingleParam, _PARAM_OBJECT_INTERNALID, pContext->m_InternalId );
			}

			if (IRP_MJ_SET_INFORMATION == FuncMj && FileDispositionInformation == FuncMi)
			{
				PFILE_DISPOSITION_INFORMATION pfdi;
				pfdi = (PFILE_DISPOSITION_INFORMATION) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
				SINGLE_PARAM_SHIFT( pSingleParam );				
				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_BINARYDATA, (ULONG) pfdi->DeleteFile );
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_HARDLINK_COUNT, pContext->m_NumberOfLinks );
		}

		ReleaseFileNameInfo( pNameInfo );
	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_Rename (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext
	)
{
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;
	PFLT_FILE_NAME_INFORMATION pNameInfoDest = NULL;

	PFILTER_EVENT_PARAM pParam = NULL;
	PFILE_RENAME_INFORMATION pRenameInfo = (PFILE_RENAME_INFORMATION) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;

	if (Data->Iopb->Parameters.SetFileInformation.Length < sizeof(FILE_RENAME_INFORMATION))
		return NULL;
	
	if (pContext)
		pNameInfo = LockFileNameInfo( pContext );

	if (pNameInfo)
	{
		status = FltGetDestinationFileNameInformation (
			FltObjects->Instance,
			FltObjects->FileObject,
			pRenameInfo->RootDirectory,
			pRenameInfo->FileName,
			pRenameInfo->FileNameLength,
			FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT/*fixed debug assertion on chk build*/,
			&pNameInfoDest
			);

		if (NT_SUCCESS( status ))
			FltParseFileNameInformation( pNameInfoDest );
		else
			pNameInfoDest = NULL;
	}

	if (pNameInfo && pNameInfoDest)
	{
		ULONG params_size = sizeof(FILTER_EVENT_PARAM);
		ULONG ParamsCount = 0;

		PSID pSid = NULL;
		ULONG SidLength = 0;
		PLUID pLuid = NULL;

		if (pNameInfo->Name.Length)			// _PARAM_OBJECT_URL_W + \0
		{
			ParamsCount++;
			params_size += pNameInfo->Name.Length + sizeof(WCHAR);
		}
		
		ParamsCount++;
		params_size += sizeof(ULONG);		// _PARAM_OBJECT_CONTEXT_FLAGS

		if (pNameInfo->Volume.Length)		// _PARAM_OBJECT_VOLUME_NAME_W + \0
		{
			ParamsCount++;
			params_size += pNameInfo->Volume.Length + sizeof(WCHAR);
		}
		
		ParamsCount++;
		params_size += pNameInfoDest->Name.Length + sizeof(WCHAR);			//_PARAM_OBJECT_URL_DEST_W
		
		if (pNameInfoDest->Volume.Length)		// _PARAM_OBJECT_VOLUME_NAME_DEST_W + \0
		{
			ParamsCount++;
			params_size += pNameInfoDest->Volume.Length + sizeof(WCHAR);
		}

		if (pStreamHandleContext)
		{
			if (pStreamHandleContext->m_pSid)						//4: _PARAM_OBJECT_SID 
			{
				pSid = pStreamHandleContext->m_pSid;
				SidLength = RtlLengthSid( pSid );

				ParamsCount++;
				params_size += SidLength;
			}

			if (pStreamHandleContext->m_Luid.LowPart || pStreamHandleContext->m_Luid.HighPart)
			{
				pLuid = &pStreamHandleContext->m_Luid;
				ParamsCount++;										//5: _PARAM_OBJECT_LUID
				params_size += sizeof(LUID);
			}
		}
		
		params_size += sizeof(SINGLE_PARAM) * ParamsCount;

		pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;
			FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_NFIOR, IRP_MJ_SET_INFORMATION, GetMiFunction( Data ), PreProcessing, ParamsCount );
			
			pParam->ProcName[0] = 0;
			//GetProcName( pParam->ProcName, IoThreadToProcess( Data->Thread ) );

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
			if (pNameInfo->Name.Length)
			{
				memcpy(pSingleParam->ParamValue, pNameInfo->Name.Buffer, pNameInfo->Name.Length);
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Name.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Name.Length + sizeof(WCHAR);
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			{
				ULONG tmpFlags = _CONTEXT_OBJECT_FLAG_NONE;

				if (_STREAM_FLAGS_DIRECTORY & pContext->m_Flags)
					tmpFlags |= _CONTEXT_OBJECT_FLAG_DIRECTORY;

				tmpFlags |= pInstanceContext->m_ContextFlags;

				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, tmpFlags );
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W);
			if (pNameInfo->Volume.Length)
			{
				memcpy(pSingleParam->ParamValue, pNameInfo->Volume.Buffer, pNameInfo->Volume.Length);
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfo->Volume.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfo->Volume.Length + sizeof(WCHAR);
			}

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_DEST_W);

			memcpy(pSingleParam->ParamValue, pNameInfoDest->Name.Buffer, pNameInfoDest->Name.Length);
			((PCHAR)pSingleParam->ParamValue)[pNameInfoDest->Name.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[pNameInfoDest->Name.Length + 1] = 0;
			pSingleParam->ParamSize = pNameInfoDest->Name.Length + sizeof(WCHAR);

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_VOLUME_NAME_DEST_W);
			if (pNameInfoDest->Volume.Length)
			{
				memcpy(pSingleParam->ParamValue, pNameInfoDest->Volume.Buffer, pNameInfoDest->Volume.Length);
				((PCHAR)pSingleParam->ParamValue)[pNameInfoDest->Volume.Length] = 0;
				((PCHAR)pSingleParam->ParamValue)[pNameInfoDest->Volume.Length + 1] = 0;
				pSingleParam->ParamSize = pNameInfoDest->Volume.Length + sizeof(WCHAR);
			}

			if (SidLength)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_SID );
				pSingleParam->ParamSize = SidLength;
				RtlCopySid( SidLength, pSingleParam->ParamValue, pStreamHandleContext->m_pSid );
			}

			if (pLuid)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_LUID );
				pSingleParam->ParamSize = sizeof(LUID);
				memcpy( pSingleParam->ParamValue, pLuid, sizeof(LUID) );
			}
		}
	}

	ReleaseFileNameInfo( pNameInfo );
	ReleaseFileNameInfo( pNameInfoDest );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_CreateLink (
	PMKAV_INSTANCE_CONTEXT pInstanceContext,
	PCFLT_RELATED_OBJECTS FltObjects,
	PFLT_CALLBACK_DATA Data,
	PMKAV_STREAM_CONTEXT pContext,
	PMKAV_STREAM_HANDLE_CONTEXT pStreamHandleContext
	)
{
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;
	PFLT_FILE_NAME_INFORMATION pNameInfoDest = NULL;

	PFILTER_EVENT_PARAM pParam = NULL;
	PFILE_LINK_INFORMATION pli = (PFILE_LINK_INFORMATION) (Data->Iopb->Parameters.SetFileInformation.InfoBuffer);

	UNREFERENCED_PARAMETER( pInstanceContext );

	if (Data->Iopb->Parameters.SetFileInformation.Length < sizeof(FILE_LINK_INFORMATION))
		return NULL;
	
	if (pContext)
		pNameInfo = LockFileNameInfo( pContext );

	if (pNameInfo)
	{
		status = FltGetDestinationFileNameInformation (
			FltObjects->Instance,
			FltObjects->FileObject,
			pli->RootDirectory,
			pli->FileName,
			pli->FileNameLength,
			FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT/*fixed debug assertion on chk build*/,
			&pNameInfoDest
			);

		if (NT_SUCCESS( status ))
			FltParseFileNameInformation( pNameInfoDest );
		else
			pNameInfoDest = NULL;
	}
	
	if (pNameInfo && pNameInfoDest)
	{
		ULONG ParamsCount = 0;
		ULONG params_size;
		ULONG HookId = FLTTYPE_NFIOR;
		ULONG FuncMj = IRP_MJ_SET_INFORMATION;
		ULONG FuncMi = GetMiFunction( Data );

		PSID pSid = NULL;
		ULONG SidLength = 0;
		PLUID pLuid = NULL;

		if (pStreamHandleContext)
		{
			if(pStreamHandleContext->m_pSid)
			{
				pSid = pStreamHandleContext->m_pSid;
				SidLength = RtlLengthSid( pSid );
			}
			pLuid = &pStreamHandleContext->m_Luid;
		}

		params_size = sizeof(FILTER_EVENT_PARAM);
		_add_param( params_size, ParamsCount, pNameInfo->Name.Length + sizeof(WCHAR) );			//1: _PARAM_OBJECT_URL_W
		_add_param( params_size, ParamsCount, pNameInfo->Volume.Length + sizeof(WCHAR) );		//2: _PARAM_OBJECT_VOLUME_NAME_W
		_add_param( params_size, ParamsCount, pNameInfoDest->Name.Length + sizeof(WCHAR) );		//3: _PARAM_OBJECT_URL_DEST_W
		_add_param( params_size, ParamsCount, pNameInfoDest->Volume.Length + sizeof(WCHAR) );	//4: _PARAM_OBJECT_VOLUME_NAME_DEST_W
		_add_param( params_size, ParamsCount, SidLength );										//5: _PARAM_OBJECT_SID
		_add_param( params_size, ParamsCount, sizeof(LUID) );									//6: _PARAM_OBJECT_LUID

		pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;
		
			FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, FuncMi, PreProcessing, ParamsCount );
				
			pParam->ProcName[0] = 0;
			//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

			pSingleParam = (PSINGLE_PARAM) pParam->Params;

			PlaceParam( &pSingleParam, _PARAM_OBJECT_LUID, pLuid, sizeof(LUID), _SINGLE_PARAM_FLAG_NONE );
			PlaceParam( &pSingleParam, _PARAM_OBJECT_SID, pLuid, SidLength, _SINGLE_PARAM_FLAG_NONE );
			PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, &pNameInfo->Name, _SINGLE_PARAM_FLAG_NONE );
			PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W, &pNameInfo->Volume, _SINGLE_PARAM_FLAG_NONE );
			PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_DEST_W, &pNameInfoDest->Name, _SINGLE_PARAM_FLAG_NONE );
			PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_VOLUME_NAME_DEST_W, &pNameInfoDest->Volume, _SINGLE_PARAM_FLAG_NONE );
		}
	}

	ReleaseFileNameInfo( pNameInfo );
	ReleaseFileNameInfo( pNameInfoDest );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_MountVolume (
	PUNICODE_STRING pVolumeName
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;
	ULONG params_size;

	params_size = sizeof(FILTER_EVENT_PARAM);
	params_size += sizeof(SINGLE_PARAM) * 1;
	if (pVolumeName->Length)		// _PARAM_OBJECT_VOLUME_NAME_W + \0
		params_size += pVolumeName->Length + sizeof(WCHAR);
	
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_NFIOR, IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, PostProcessing, 1);
		
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess());

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W);
		if (pVolumeName->Length)
		{
			memcpy(pSingleParam->ParamValue, pVolumeName->Buffer, pVolumeName->Length);
			((PCHAR)pSingleParam->ParamValue)[pVolumeName->Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[pVolumeName->Length + 1] = 0;
			pSingleParam->ParamSize = pVolumeName->Length + sizeof(WCHAR);
		}
		else
			pSingleParam->ParamSize = 0;
	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_UnMountVolume (
	PUNICODE_STRING pVolumeName
	)
{
	PFILTER_EVENT_PARAM pParam = EvContext_MountVolume( pVolumeName );
	if (pParam)
		pParam->FunctionMi = 0xf1; //IRP_MN_UNMOUNT_VOLUME

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_OpenProcess (
	__in ACCESS_MASK DesiredAccess,
	__in_opt PCLIENT_ID pClientId
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 3; //MJ_SYSTEM_OPEN_PROCESS;
	ULONG FuncMi = 0;

	HANDLE CurrentProcess = PsGetCurrentProcess();
	ULONG ContextFlags = _CONTEXT_OBJECT_FLAG_NONE;

	PSID pSid = NULL;
	ULONG SidLength = 0;

	NTSTATUS status = SeGetUserSid( NULL, &pSid );
	if (NT_SUCCESS( status ))
		SidLength = RtlLengthSid( pSid );

	params_size = sizeof(FILTER_EVENT_PARAM);
		
	ParamsCount++;													//1: _PARAM_OBJECT_ACCESSATTR
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	ParamsCount++;													//2: _PARAM_OBJECT_CONTEXT_FLAGS
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	if (pClientId)
	{
		ParamsCount++;													//3: _PARAM_OBJECT_DEST_ID
		params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

		if (CurrentProcess == pClientId->UniqueProcess)
			ContextFlags = _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF;
	}

	if(SidLength)
	{
		ParamsCount++;												//4: _PARAM_OBJECT_SID 
		params_size += sizeof(SINGLE_PARAM) + SidLength;
	}
			
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
			
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, FuncMi, PreProcessing, ParamsCount );
			
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, CurrentProcess );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_ULONG( pSingleParam, _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF, ContextFlags );

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, DesiredAccess );

		if (SidLength)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_SID );
			pSingleParam->ParamSize = SidLength;
			RtlCopySid( SidLength, pSingleParam->ParamValue, pSid );
		}

		if (pClientId)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_DEST_ID, HandleToUlong( pClientId->UniqueProcess) );
		}
	}

	if (pSid)
		ExFreePool( pSid );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_TerminateProcess (
	__in HANDLE hProcess
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0; //MJ_SYSTEM_TERMINATE_PROCESS;
	ULONG FuncMi = 0;

	PSID pSid = NULL;
	ULONG SidLength = 0;

	NTSTATUS status = SeGetUserSid( NULL, &pSid );
	if (NT_SUCCESS( status ))
		SidLength = RtlLengthSid( pSid );

	params_size = sizeof(FILTER_EVENT_PARAM);
		
	ParamsCount++;													//1: _PARAM_OBJECT_DEST_ID
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	if(SidLength)
	{
		ParamsCount++;												//2: _PARAM_OBJECT_SID 
		params_size += sizeof(SINGLE_PARAM) + SidLength;
	}
			
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
			
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, FuncMi, PreProcessing, ParamsCount );
			
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_DEST_ID, HandleToUlong(hProcess) );

		if (SidLength)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_SID );
			pSingleParam->ParamSize = SidLength;
			RtlCopySid( SidLength, pSingleParam->ParamValue, pSid );
		}
	}

	if (pSid)
		ExFreePool( pSid );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_WriteProcessMemory (
	__in HANDLE hProcess,
	__in PVOID StartAddress,
	__in PVOID pBuffer,
	__in ULONG BytesToWrite
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0x0b; //MJ_SYSTEM_WRITE_PROCESS_MEMORY
	ULONG FuncMi = 0;

	PSID pSid = NULL;
	ULONG SidLength = 0;
	LUID Luid;

	NTSTATUS status = SeGetUserSid( NULL, &pSid );
	if (NT_SUCCESS( status ))
		SidLength = RtlLengthSid( pSid );

	SeGetUserLuid( &Luid );

	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//1: _PARAM_OBJECT_DEST_ID
	_add_param( params_size, ParamsCount, sizeof(PVOID) );			//2: _PARAM_OBJECT_BYTEOFFSET
	_add_param( params_size, ParamsCount, BytesToWrite );			//3: _PARAM_OBJECT_BINARYDATA
	_add_param( params_size, ParamsCount, SidLength );				//4: _PARAM_OBJECT_SID
	_add_param( params_size, ParamsCount, sizeof(LUID) );			//5: _PARAM_OBJECT_LUID

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
			
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, FuncMi, PreProcessing, ParamsCount );
			
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_DEST_ID, HandleToUlong(hProcess) );

		SINGLE_PARAM_SHIFT( pSingleParam );
		SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_BYTEOFFSET );
		pSingleParam->ParamSize = sizeof(PVOID);
		RtlCopyMemory( pSingleParam->ParamValue, &StartAddress, sizeof(PVOID) );

		SINGLE_PARAM_SHIFT( pSingleParam );
		SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_BINARYDATA );
		pSingleParam->ParamSize = BytesToWrite;
		RtlCopyMemory( pSingleParam->ParamValue, pBuffer, BytesToWrite );

		if (SidLength)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_SID );
			pSingleParam->ParamSize = SidLength;
			RtlCopySid( SidLength, pSingleParam->ParamValue, pSid );
		}

		SINGLE_PARAM_SHIFT( pSingleParam );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_LUID, &Luid, sizeof(Luid), _SINGLE_PARAM_FLAG_NONE );
	}

	if (pSid)
		ExFreePool( pSid );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_CreateThread (
	__in HANDLE hDestProcessId,
	__in ACCESS_MASK DesiredAccess,
	__in PCONTEXT pContext,
	__in BOOLEAN bCreateSuspended
	)
{
#ifdef _WIN64
	UNREFERENCED_PARAMETER( hDestProcessId );
	UNREFERENCED_PARAMETER( DesiredAccess );
	UNREFERENCED_PARAMETER( pContext );
	UNREFERENCED_PARAMETER( bCreateSuspended );

	return NULL;
#else
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0x08; //MJ_SYSTEM_CREATE_REMOTE_THREAD

	PSID pSid = NULL;
	ULONG SidLength = 0;

	NTSTATUS status = SeGetUserSid( NULL, &pSid );
	if (NT_SUCCESS( status ))
		SidLength = RtlLengthSid( pSid );

	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//1: _PARAM_OBJECT_DEST_ID
	_add_param( params_size, ParamsCount, sizeof(DesiredAccess) );	//2: _PARAM_OBJECT_ACCESSATTR
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//3: _PARAM_OBJECT_ADDR1
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//3: _PARAM_OBJECT_ADDR2
	_add_param( params_size, ParamsCount, sizeof(BOOLEAN) );		//4: _PARAM_OBJECT_TYPE
	_add_param( params_size, ParamsCount, SidLength );				//5: _PARAM_OBJECT_SID

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		ULONG DestId = HandleToUlong( hDestProcessId );
		ULONG addr1 = PtrToUlong( pContext->Eip );
		ULONG addr2 = PtrToUlong( pContext->Eax );

		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );
			
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParam( &pSingleParam, _PARAM_OBJECT_DEST_ID, &DestId, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ACCESSATTR, &DesiredAccess, sizeof(DesiredAccess), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ADDR1, &addr1, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ADDR2, &addr2, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_TYPE, &bCreateSuspended, sizeof(BOOLEAN), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_SID, pSid, SidLength, _SINGLE_PARAM_FLAG_NONE );
	}

	if (pSid)
		ExFreePool( pSid );

	return pParam;
#endif
}

PFILTER_EVENT_PARAM
EvContext_SetContextThread (
	__in HANDLE DestProcessId,
	__in HANDLE DestThreadID,
	__in PCONTEXT pContext
	)
{
#ifdef _WIN64
	UNREFERENCED_PARAMETER( DestProcessId );
	UNREFERENCED_PARAMETER( DestThreadID );
	UNREFERENCED_PARAMETER( pContext );

	return NULL;
#else
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0x0d; //MJ_SYSTEM_SET_THREAD_CONTEXT

	PSID pSid = NULL;
	ULONG SidLength = 0;

	NTSTATUS status = SeGetUserSid( NULL, &pSid );
	if (NT_SUCCESS( status ))
		SidLength = RtlLengthSid( pSid );

	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//1: _PARAM_OBJECT_CLIENTID1
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//3: _PARAM_OBJECT_CLIENTID2
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//3: _PARAM_OBJECT_ADDR1
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//3: _PARAM_OBJECT_ADDR2
	_add_param( params_size, ParamsCount, SidLength );				//5: _PARAM_OBJECT_SID

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		ULONG procid = HandleToUlong( DestProcessId );
		ULONG thread = HandleToUlong( DestThreadID );
		ULONG addr1 = PtrToUlong( pContext->Eip );
		ULONG addr2 = PtrToUlong( pContext->Eax );

		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );
			
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParam( &pSingleParam, _PARAM_OBJECT_CLIENTID1, &procid, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_CLIENTID2, &thread, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ADDR1, &addr1, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ADDR2, &addr2, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_SID, pSid, SidLength, _SINGLE_PARAM_FLAG_NONE );
	}

	if (pSid)
		ExFreePool( pSid );

	return pParam;
#endif
}

PFILTER_EVENT_PARAM
EvContext_CreateProcess (
	__in PFLT_FILE_NAME_INFORMATION pNameInfo,
	__in ACCESS_MASK DesiredAccess,
	__in HANDLE ProcessId
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0x01; //MJ_SYSTEM_CREATE_PROCESS

	PSID pSid = NULL;
	ULONG SidLength = 0;

	NTSTATUS status = SeGetUserSid( NULL, &pSid );
	if (NT_SUCCESS( status ))
		SidLength = RtlLengthSid( pSid );

	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, pNameInfo->Name.Length + sizeof(WCHAR) );			//1: _PARAM_OBJECT_URL_W
	_add_param( params_size, ParamsCount, pNameInfo->Volume.Length + sizeof(WCHAR) );		//2: _PARAM_OBJECT_VOLUME_NAME_W
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//3: _PARAM_OBJECT_ACCESSATTR
	_add_param( params_size, ParamsCount, SidLength );				//4: _PARAM_OBJECT_SID

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_event );
	if (pParam)
	{
		ULONG procid = HandleToUlong( ProcessId );

		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );
			
		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, &pNameInfo->Name, _SINGLE_PARAM_FLAG_NONE );
		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W, &pNameInfo->Volume, _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ACCESSATTR, &DesiredAccess, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_SID, pSid, SidLength, _SINGLE_PARAM_FLAG_NONE );
	}

	if (pSid)
		ExFreePool( pSid );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_SystemDebugControl (
	__in ULONG ControlCode
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0x1c; //MJ_SYSTEM_DEBUGCONTROL



	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, sizeof(ULONG) );			//1: _PARAM_OBJECT_INTERNALID

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepflt6 );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;
		//GetProcName(pParam->ProcName, PsGetCurrentProcess() );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParam( &pSingleParam, _PARAM_OBJECT_INTERNALID, &ControlCode, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_LoadDriver (
	__in PUNICODE_STRING pServiceName
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	ULONG FuncMj = 0x1a; //MJ_SYSTEM_LOADDRIVER



	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, pServiceName->Length + sizeof(WCHAR) );			//1: _PARAM_OBJECT_URL_W
	
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepflt7 );
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;
		
		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, pServiceName, _SINGLE_PARAM_FLAG_NONE );
	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_SetSecurityObject (
	__in PUNICODE_STRING pName,
	__in ULONG FuncMj
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;

	ULONG HookId = FLTTYPE_SYSTEM;
	
	params_size = sizeof(FILTER_EVENT_PARAM);
	_add_param( params_size, ParamsCount, sizeof(LUID) );					//1: _PARAM_OBJECT_LUID
	_add_param( params_size, ParamsCount, pName->Length + sizeof(WCHAR) );	//2: _PARAM_OBJECT_URL_W

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepflt8 );
	if ( pParam )
	{
		LUID luid;
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );
				
		RtlZeroMemory( &luid, sizeof(LUID) );
		pParam->ProcName[0] = 0;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		
		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, pName, _SINGLE_PARAM_FLAG_NONE );
		
		SeGetUserLuid ( &luid );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_LUID, &luid, sizeof(LUID), _SINGLE_PARAM_FLAG_NONE );

	}

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_CreateSection(
	__in PUNICODE_STRING pFileName,
	__in ACCESS_MASK DesiredAccess,
	__in ULONG ObjAttributes,
	__in ULONG SectionPageProtection
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;
	ULONG FuncMj = 0x4; //MJ_SYSTEM_CREATE_SECTION
	ULONG HookId = FLTTYPE_SYSTEM;
	
	params_size = sizeof(FILTER_EVENT_PARAM);
	
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//1: _PARAM_OBJECT_ACCESSATTR
	_add_param( params_size, ParamsCount, pFileName->Length + sizeof(WCHAR) );	//2: _PARAM_OBJECT_URL_W
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//3: _PARAM_OBJECT_ATTRIB
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//4: _PARAM_OBJECT_PAGEPROTECTION

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepflt9 );
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ACCESSATTR, &DesiredAccess, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, pFileName, _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_ATTRIB, &ObjAttributes, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_PAGEPROTECTION, &SectionPageProtection, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
	}

	return pParam;	
}

PFILTER_EVENT_PARAM
EvContext_CreateSymbolicLinkObject(
	__in ACCESS_MASK DesiredAccess
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;
	ULONG FuncMj = 0x13; //MJ_SYSTEM_ACCESS_PHYSICAL_MEM
	ULONG HookId = FLTTYPE_SYSTEM;

	params_size = sizeof(FILTER_EVENT_PARAM);

	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//1: _PARAM_OBJECT_ACCESSATTR
	
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepfltA );
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParam( &pSingleParam, _PARAM_OBJECT_ACCESSATTR, &DesiredAccess, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		
	}

	return pParam;	
}

PFILTER_EVENT_PARAM
EvContext_SetSystemInformation(
	__in PUNICODE_STRING pModuleName
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;
	ULONG FuncMj = 0x1f; //MJ_SYSTEM_LOADANDCALL
	ULONG HookId = FLTTYPE_SYSTEM;

	params_size = sizeof(FILTER_EVENT_PARAM);

	_add_param( params_size, ParamsCount, pModuleName->Length + sizeof(WCHAR) );	//1: _PARAM_OBJECT_URL_W
	
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepfltB );
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, pModuleName, _SINGLE_PARAM_FLAG_NONE );
	}

	return pParam;	
}

PFILTER_EVENT_PARAM
EvContext_DuplicateObject(
	ULONG SrcPrcID,		 // source process id
	ULONG TrgPrcID,		 // target process id
	ULONG SrcHandle,	 // source handle
	ULONG SrcHandleType, // source handle type
	ULONG SrcHandlePID 	 // source handle
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;
	ULONG FuncMj = 0xf; //MJ_SYSTEM_DUPLICATE_OBJECT
	ULONG HookId = FLTTYPE_SYSTEM;

	params_size = sizeof(FILTER_EVENT_PARAM);

	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//1: _PARAM_OBJECT_SOURCE_ID
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//2: _PARAM_OBJECT_DEST_ID
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//3: _PARAM_OBJECT_OBJECTHANDLE
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//4: _PARAM_OBJECT_TYPE
	_add_param( params_size, ParamsCount, sizeof(ULONG) );						//5: _PARAM_OBJECT_CLIENTID1

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepfltC );
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParam( &pSingleParam, _PARAM_OBJECT_SOURCE_ID, &SrcPrcID, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_DEST_ID, &TrgPrcID, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, &SrcHandle, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_TYPE, &SrcHandleType, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
		PlaceParam( &pSingleParam, _PARAM_OBJECT_CLIENTID1, &SrcHandlePID, sizeof(ULONG), _SINGLE_PARAM_FLAG_NONE );
	}

	return pParam;	
}

PFILTER_EVENT_PARAM
EvContext_GET_RETRIEVAL_POINTERS(
	__in PUNICODE_STRING pFileName,
	__in PUNICODE_STRING pVolumeName
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size;
	ULONG FuncMj = 0x27; //MJ_SYSTEM_GET_FILESECTORS			
	ULONG HookId = FLTTYPE_SYSTEM;

	params_size = sizeof(FILTER_EVENT_PARAM);

	_add_param( params_size, ParamsCount, pFileName->Length + sizeof(WCHAR) );	//1: _PARAM_OBJECT_URL_W
	_add_param( params_size, ParamsCount, pVolumeName->Length + sizeof(WCHAR) );	//2: _PARAM_OBJECT_VOLUME_NAME_W

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_prepfltD );
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, HookId, FuncMj, 0, PreProcessing, ParamsCount );

		pParam->ProcName[0] = 0;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_URL_W, pFileName, _SINGLE_PARAM_FLAG_NONE );
		PlaceParamWstr( &pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W, pVolumeName, _SINGLE_PARAM_FLAG_NONE );
	}

	return pParam;	
}

PFILTER_EVENT_PARAM
EvContext_Simple(
	ULONG FuncMj,
	__in ULONG Tag
	)
{
	PFILTER_EVENT_PARAM pParam = NULL;
	PSID psid = NULL;
	

	ULONG ParamsCount = 0;
	ULONG params_size = sizeof(FILTER_EVENT_PARAM);

	if ( !NT_SUCCESS( SeGetUserSid(NULL, &psid) ) )
		psid = NULL;

	if (psid )
	{
		ParamsCount++;										//1: SID
		params_size += sizeof(SINGLE_PARAM) + RtlLengthSid( psid );
	}

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, Tag );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_SYSTEM, FuncMj, 0, PreProcessing, ParamsCount );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		if (psid)
		{
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_SID);
			pSingleParam->ParamSize = RtlLengthSid( psid );
			memcpy( pSingleParam->ParamValue, psid, RtlLengthSid( psid ) );
		}
	}

	if (psid)
		ExFreePool( psid );

	return pParam;
}

PFILTER_EVENT_PARAM
EvContext_RasDial (
)
{

	return EvContext_Simple(
		0x22, //MJ_SYSTEM_RASDIAL
		tag_prepflt2
		);
}

PFILTER_EVENT_PARAM
EvContext_ProtectedStorage (
)
{
	return EvContext_Simple(
		0x23, //MJ_SYSTEM_PROTECTEDSTORAGE
		tag_prepflt3
		);
}

PFILTER_EVENT_PARAM
EvContext_RegRawInputDevices (
)
{
	return EvContext_Simple(
		0x24, //MJ_SYSTEM_RAWINPUTDEVICES
		tag_prepflt4
		);
}

PFILTER_EVENT_PARAM
EvContext_Shutdown (
)
{
	return EvContext_Simple(
		0x25, //MJ_SYSTEM_SHUTDOWN
		tag_prepflt5
		);
}

//+ ------------------------------------------------------------------------------------------
VOID
EvContext_Done (
	PFILTER_EVENT_PARAM pFilterEventParam
	)
{
	if (pFilterEventParam)
		ExFreePool(pFilterEventParam);	
}

//+ ------------------------------------------------------------------------------------------
VOID
EvObject_Fill (
	__in IN PEVENT_OBJ pEventObj,
	__in PCFLT_RELATED_OBJECTS pRelatedObject,
	__in PMKAV_INSTANCE_CONTEXT pInstanceContext
	)
{
	pEventObj->m_RefCount = 0; // if _EVEN_FILE_OBJECT - cached io allowed
	pEventObj->m_pRelatedObject = (PVOID) pRelatedObject;
	pEventObj->m_pInstanceContext = pInstanceContext;
	pEventObj->m_Flags = 0;
}

VOID
ResolveAlign (
	ULONG block_size, 
	LONGLONG offset_low, 
	ULONG reqsize, 
	ULONG* poffset_align,
	ULONG* pnew_size)
{
	ULONG block_count;

	block_count = reqsize / block_size;
	if (reqsize % block_size)
		block_count++;

	*poffset_align = (ULONG)(offset_low % block_size);
	if (*poffset_align)
		block_count++;

	*pnew_size = block_count * block_size;	
}

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
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PVOID buffer = NULL;
	ULONG BytesRead = 0;
	
	ULONG new_size;
	ULONG offset_allign;

	*pRetSize = 0;

	ResolveAlign( SectorSize, Offset.LowPart, OutputBufferLength, &offset_allign, &new_size);
	Offset.LowPart -= offset_allign;

	buffer = FltAllocatePoolAlignedWithTag( pInstance, PagedPool, new_size, 'Abos' );

	if (buffer)
	{
		ULONG ioflags = FLTFL_IO_OPERATION_DO_NOT_UPDATE_BYTE_OFFSET;
		if (!bCached)
			ioflags |= FLTFL_IO_OPERATION_NON_CACHED;
		
		status = FltReadFile( pInstance, pFileObject, 
			&Offset, new_size, buffer, ioflags, &BytesRead, NULL, NULL); 
		
		if (NT_SUCCESS( status ))
		{
			if (offset_allign >= BytesRead)
			{
				// read but data not exist
				_dbg_break_;
				status = STATUS_UNSUCCESSFUL;
			}
			else
			{
				ULONG read_len;
				BytesRead -= offset_allign;
				read_len = BytesRead > OutputBufferLength ? OutputBufferLength : BytesRead;
				memcpy(pOutputBuffer, (PCHAR) buffer + offset_allign, read_len);

				*pRetSize = read_len;
			}
		}
					
		FltFreePoolAlignedWithTag( pInstance, buffer, 'Abos');
	}

	return status;
}

// bug in RDPDR - will fixed in longhorn
// status = FltQueryInformationFile ( FltObjects->Instance, FltObjects->FileObject, &BasicInfo,
//	sizeof(BasicInfo), FileBasicInformation, &LengthReturned );

NTSTATUS
FileObject_GetBasicInfo  (
	PFLT_INSTANCE pInstance,
	PFILE_OBJECT pFileObject,
	PFILE_BASIC_INFORMATION pBasicInfo
	)
{
	ULONG RetBytes = 0;
	NTSTATUS status = MySynchronousQueryInformationFile (
		pInstance,
		pFileObject, 
		pBasicInfo,
		sizeof( FILE_BASIC_INFORMATION ),
		FileBasicInformation,
		&RetBytes
		);

	if (sizeof( FILE_BASIC_INFORMATION ) != RetBytes)
		status = STATUS_INVALID_BUFFER_SIZE;

	return status;
}

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
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	*pRetSize = 0;

	switch (Request)
	{
	case _event_request_get_fidbox:
		if (!pOutputBuffer || OutputBufferLength < sizeof(FIDBOX_REQUEST_DATA_EX))
		{
			_dbg_break_;
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			ULONG ReqSize = OutputBufferLength - sizeof(FIDBOX_REQUEST_DATA_EX);
			if (ReqSize)
				status = FidBox_GetDataByFO( pFileObject, FALSE, (PFIDBOX_REQUEST_DATA_EX) pOutputBuffer, ReqSize, pRetSize, TRUE );
			else
				status = STATUS_INVALID_PARAMETER;
		}
		break;

	case _event_request_set_fidbox:
		{
			PFIDBOX_REQUEST_DATA_EX pFidData = (PFIDBOX_REQUEST_DATA_EX) pBuffer;
			ULONG ReqSize = BufferLength;

			if (ReqSize <= sizeof(FIDBOX_REQUEST_DATA_EX))
				status = STATUS_NOT_SUPPORTED;
			else
				status = FidBox_SetDataByFO( pFileObject, FALSE, pFidData, ReqSize  - sizeof(FIDBOX_REQUEST_DATA_EX));
		}
		break;
	
	case _event_request_get_size:
		if (OutputBufferLength < sizeof(LARGE_INTEGER))
			status = STATUS_INVALID_PARAMETER;
		else
		{
			FILE_STANDARD_INFORMATION stdinfo;
			status = MySynchronousQueryInformationFile (
				pInstance,
				pFileObject, 
				&stdinfo,
				sizeof(stdinfo),
				FileStandardInformation,
				NULL
				);

			if (NT_SUCCESS(status))
			{
				*pRetSize = sizeof(LARGE_INTEGER);
				*(LARGE_INTEGER*)pOutputBuffer = stdinfo.EndOfFile;
			}
		}
		break;

	default:
		{
			_dbg_break_;
		}
		break;
	}

	return status;
}

// transaction support
PFLT_INSTANCE
GetSystemVolumeInstance (
	)
{
	PVOID pObject = NULL;

	PAGED_CODE();

	AcquireResourceExclusive( &Globals.m_Resource );

	if (Globals.m_SystemVolumeInstance)
	{
		pObject = Globals.m_SystemVolumeInstance;
		FltObjectReference(pObject);
	}

	ReleaseResource( &Globals.m_Resource );

	return pObject;
}

NTSTATUS
InitTransactionContext (
	__in PMKAV_TRANSACTION_CONTEXT pContext,
	__in PVOID pTransObj
	)
{
	NTSTATUS ntStatus;

	PAGED_CODE();

	pContext->m_pTransObj = pTransObj;

// store transaction object pointer
	RtlInitializeGenericTable (
		&pContext->m_EnlistedFilesTable,
		TransTableCompare_Files,
		TransTableAlloc_Common,
		TransTableFree_Common,
		NULL
		);

	RtlInitializeGenericTable (
		&pContext->m_EnlistedKeysTable,
		TransTableCompare_Keys,
		TransTableAlloc_Common,
		TransTableFree_Common,
		NULL
		);

	ntStatus = ExInitializeResourceLite( &pContext->m_FileLock );
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ExInitializeResourceLite( &pContext->m_RegLock );
		if (!NT_SUCCESS( ntStatus ))
			ExDeleteResourceLite( &pContext->m_FileLock );
	}

	if (!NT_SUCCESS( ntStatus ))
	{
		_dbg_break_;
		pContext->m_pTransObj = NULL;
	}

	return ntStatus;
}

PMKAV_TRANSACTION_CONTEXT
LookupTransactionContext (
	__in PKTRANSACTION pTransaction
	)
{
	PFLT_INSTANCE pInstance;
	PMKAV_TRANSACTION_CONTEXT pRetContext = NULL;

	PAGED_CODE();

	if (!g_bTransactionOperationsSupported)
		return NULL;

	pInstance = GetSystemVolumeInstance();
	if (pInstance)
	{
		NTSTATUS status;
		PFLT_CONTEXT pContext = NULL;

		status = g_pfFltGetTransactionContext (
			pInstance,
			pTransaction,
			&pContext
			);
		
		if (NT_SUCCESS( status ))
			pRetContext = (PMKAV_TRANSACTION_CONTEXT) pContext;

		FltObjectDereference( pInstance );
	}

	return pRetContext;
}


NTSTATUS
MySynchronousQueryInformationFile (
	IN PFLT_INSTANCE Instance,
	IN PFILE_OBJECT FileObject,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	OUT PULONG LengthReturned OPTIONAL
	)
{
	PFLT_CALLBACK_DATA data;
	NTSTATUS status;
	
	PAGED_CODE();
	
	status = FltAllocateCallbackData( Instance, FileObject, &data );
	
	if (!NT_SUCCESS( status ))
		return status;
	
	data->Iopb->MajorFunction = IRP_MJ_QUERY_INFORMATION;
	data->Iopb->Parameters.QueryFileInformation.FileInformationClass = FileInformationClass;
	data->Iopb->Parameters.QueryFileInformation.Length = Length;
	data->Iopb->Parameters.QueryFileInformation.InfoBuffer = FileInformation;
	data->Iopb->IrpFlags = IRP_SYNCHRONOUS_API;
	
	FltPerformSynchronousIo( data );

	status = data->IoStatus.Status;
	
	if (NT_SUCCESS( status ) &&  ARGUMENT_PRESENT( LengthReturned ))
		*LengthReturned = (ULONG) data->IoStatus.Information;

	FltFreeCallbackData( data );

	return status;
}