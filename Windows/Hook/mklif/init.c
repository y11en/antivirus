#include "pch.h"

#include "init.tmh"

LONG gInternalId = 0;

NTSTATUS
DriverEntry (
	__in PDRIVER_OBJECT DriverObject,
	__in PUNICODE_STRING RegistryPath
	);

#ifdef WIN2K
#pragma message ("----------------------- w2k compatibility: No Unload")
#else
NTSTATUS
MiniKavUnload (
	FLT_FILTER_UNLOAD_FLAGS Flags
	);
#endif // WIN2K

NTSTATUS
MiniKavInstanceSetup (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in FLT_INSTANCE_SETUP_FLAGS Flags,
	__in DEVICE_TYPE VolumeDeviceType,
	__in FLT_FILESYSTEM_TYPE VolumeFilesystemType
	);

VOID
MiniKavContextCleanup (
	__in PFLT_CONTEXT Context, 
	__in FLT_CONTEXT_TYPE ContextType
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreFSCTRL (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostFSCTRL (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreNetQueryOpen (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID *CompletionContext
);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreCreate (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostCreate (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreRead (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);


FLT_POSTOP_CALLBACK_STATUS
MiniKavPostRead (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);


FLT_PREOP_CALLBACK_STATUS
MiniKavPreWrite (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);


FLT_POSTOP_CALLBACK_STATUS
MiniKavPostWrite (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreCleanup (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostCleanup (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreSyncSection (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostSyncSection (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreSetInformation (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	);

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostSetInformation (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

// net fidbox
FLT_POSTOP_CALLBACK_STATUS
MiniKavPostQueryInformation (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	);

FLT_PREOP_CALLBACK_STATUS
MiniKavPreQueryEA (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
    );

NTSTATUS
MiniKavTransactionNotify (
	__in PCFLT_RELATED_OBJECTS  FltObjects,
	__in PFLT_CONTEXT  TransactionContext,
	__in ULONG  NotificationMask
	);

VOID
PostCreateFileTransProcessing (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PMKAV_STREAM_CONTEXT pContext
	);

VOID
TransEnlistFile (
	__inout PMKAV_TRANSACTION_CONTEXT pTransactionContext,
	__in PMKAV_STREAM_CONTEXT pContext
	);

//
//  Global variables
//

GLOBAL_DATA Globals;


//
//  Pragma defintiion table
//

//
//  Assign text sections for each routine.
//


#ifdef ALLOC_PRAGMA

#pragma message ("----------------------- ALLOC_PRAGMA defined")

	#pragma alloc_text(INIT, DriverEntry)
	#pragma alloc_text(PAGE, MiniKavInstanceSetup)
#ifndef WIN2K
    #pragma alloc_text(PAGE, MiniKavUnload)
#endif // WIN2K
#endif //ALLOC_PRAGMA

BOOLEAN
IsThisKernelFileObject (
	PFILE_OBJECT FileObject
	)
{
	ULONG_PTR stLower;
	ULONG_PTR stHigh;

	IoGetStackLimits( &stLower, &stHigh );
	if ((stLower <= (ULONG_PTR) FileObject) && ((ULONG_PTR)FileObject <= stHigh))
		return TRUE;

	return FALSE;
}

BOOLEAN
IsPrefetchEcpPresent (
	__in PFLT_FILTER Filter,
	__in PFLT_CALLBACK_DATA Data
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PECP_LIST EcpList = NULL;
	PPREFETCH_OPEN_ECP_CONTEXT PrefetchEcp = NULL;

	if (!pfFltGetEcpListFromCallbackData
		|| !pfFltFindExtraCreateParameter
		|| !pfFltIsEcpFromUserMode)
		return FALSE;

	// Get the ECP List from the callback data, if present.

	status = pfFltGetEcpListFromCallbackData( Filter, Data, &EcpList );

	if (NT_SUCCESS(status) && EcpList)
	{
		// Check if the prefetch ECP is specified.

		status = pfFltFindExtraCreateParameter( Filter,
			EcpList,
			&GUID_ECP_PREFETCH_OPEN,
			&PrefetchEcp,
			NULL
			);

		if (NT_SUCCESS( status ))
		{
			if (!pfFltIsEcpFromUserMode( Filter, PrefetchEcp ))
				return TRUE;
		}
	}

	return FALSE;
}

BOOLEAN
IsSkipOp (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	if (KeGetCurrentIrql() >= DISPATCH_LEVEL)
		return TRUE;
	
	if (FLTFL_POST_OPERATION_DRAINING & Flags)
	{
		_dbg_break_;
		return TRUE;
	}

	if (!FltObjects->Instance)
	{
		_dbg_break_;
		return TRUE;
	}

	if (!FltObjects->FileObject)
	{
		_dbg_break_;
		return TRUE;
	}

	if (FlagOn( FltObjects->FileObject->Flags, FO_NAMED_PIPE ))
		return TRUE;

//	if (IsThisKernelFileObject( FltObjects->FileObject ))
//		return TRUE;

	return FALSE;
}

//-

IO_WORKITEM_ROUTINE Serv_AttachFidBoxImp;

VOID
Serv_AttachFidBoxImp (
	__in PDEVICE_OBJECT DeviceObject,
	__in PVOID pItem
	)
{
	int count = 0;

	UNREFERENCED_PARAMETER( DeviceObject );

	IoFreeWorkItem( pItem );

	while(TRUE)
	{
		if (Fid_GetItemSize1())
			break;

		if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED ))
			break;

		if (count > 20)
			break;

		SleepImp( 1 );
		count++;
	}

	GlobalFidBoxInit();

	InterlockedDecrement( &Globals.m_WorkItemsInWork );
}

void
Serv_AttachFidBox ()
{
	PIO_WORKITEM pItem = IoAllocateWorkItem( Globals.m_FilterControlDeviceObject );

	if (!pItem)
	{
		_dbg_break_;
		return;
	}

	InterlockedIncrement( &Globals.m_WorkItemsInWork );

	IoQueueWorkItem( pItem, Serv_AttachFidBoxImp, DelayedWorkQueue, pItem );
}

VOID
Serv_DetachFromSystemVolume (
	)
{
	PFLT_INSTANCE SystemVolumeInstance = NULL;

	AcquireResourceExclusive( &Globals.m_Resource );

	SystemVolumeInstance = Globals.m_SystemVolumeInstance;
	Globals.m_SystemVolumeInstance = NULL;

	ReleaseResource( &Globals.m_Resource );

	if (SystemVolumeInstance)
		FltObjectDereference( SystemVolumeInstance );	
}

VOID
Serv_StreamContextReset ( 
	PMKAV_STREAM_CONTEXT pContext
	)
{
	pContext->m_pFileNameInfo = NULL;
	pContext->m_pFileId = NULL;
	pContext->m_Flags = _STREAM_FLAGS_NONE;
	pContext->m_InternalId.HighPart = InterlockedIncrement ( &gInternalId );
	pContext->m_InternalId.LowPart  = 0;
	pContext->m_NumberOfLinks = 0;
}

VOID
Serv_ContextCleanup (
	__in PVOID Pool,
	__in FLT_CONTEXT_TYPE ContextType
	)
{
	switch (ContextType)
	{
	case FLT_STREAM_CONTEXT:
		{
			PMKAV_STREAM_CONTEXT pContext = Pool;
			if (!pContext)
			{
				_dbg_break_;
			}
			else
			{
				ReleaseFileNameInfo( pContext->m_pFileNameInfo );
				
				if (pContext->m_pFileId)
					ExFreePool( pContext->m_pFileId );
		
				Serv_StreamContextReset( pContext );
			}
		}
		break;

	case FLT_STREAMHANDLE_CONTEXT:
		{
			PMKAV_STREAM_HANDLE_CONTEXT pContext = Pool;
			if (!pContext)
			{
				_dbg_break_;
			}
			else
			{
				if (pContext->m_pSid)
					ExFreePool( pContext->m_pSid );
				
				RtlZeroMemory( pContext, sizeof(MKAV_STREAM_HANDLE_CONTEXT) );
			}
		}
		break;

	default:
		{
			_dbg_break_;
		}
		break;
	}
}

VOID
Serv_FixStoredFileName (
	__in PFLT_FILE_NAME_INFORMATION pNameInfoNew,
	__in PFLT_FILE_NAME_INFORMATION pNameInfoOld
	)
{
	Sys_FixStoredFileName( pNameInfoNew, pNameInfoOld );
}

NTSTATUS
MiniKavTransactionNotify(
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PFLT_CONTEXT TransactionContext,
	__in ULONG NotificationMask
	)
{
	PMKAV_TRANSACTION_CONTEXT pTransactionContext = (PMKAV_TRANSACTION_CONTEXT) TransactionContext;

	PAGED_CODE();

	UNREFERENCED_PARAMETER( FltObjects );

	if (TransactionContext)
	{
		PFLT_INSTANCE pInstance = GetSystemVolumeInstance();
		if (pInstance)
		{
			ASSERT(pTransactionContext->m_pTransObj == FltObjects->Transaction);

			if (NotificationMask & TRANSACTION_NOTIFY_PREPARE)
			{
// transaction is being prepared for commit
				DoTraceEx(TRACE_LEVEL_INFORMATION, DC_DRIVER, "preparing transaction %p for committing..",
					pTransactionContext->m_pTransObj );
			}

			if (NotificationMask & TRANSACTION_NOTIFY_COMMIT)
			{
// transaction is being committed
				DoTraceEx(TRACE_LEVEL_INFORMATION, DC_DRIVER, "committing transaction %p",
					pTransactionContext->m_pTransObj );
			}

			if (NotificationMask & TRANSACTION_NOTIFY_ROLLBACK)
			{
// transaction is being committed
				DoTraceEx(TRACE_LEVEL_INFORMATION, DC_DRIVER, "rolling back transaction %p",
					pTransactionContext->m_pTransObj );
			}
		}

		FltObjectDereference( pInstance );
	}

	return STATUS_SUCCESS;
}

VOID
TransactionContextCleanup (
	__in PVOID Pool,
	__in FLT_CONTEXT_TYPE ContextType
	)
{
	PMKAV_TRANSACTION_CONTEXT pTransactionContext = NULL;

	if (FLT_TRANSACTION_CONTEXT != ContextType)
		return;

	pTransactionContext = (PMKAV_TRANSACTION_CONTEXT) Pool;
	if (pTransactionContext)
	{
		PMKAV_ENLISTED_FILE pFileEntry;
		PMKAV_ENLISTED_KEY pKeyEntry;

		if (pTransactionContext->m_pTransObj)
		{
			pFileEntry = (PMKAV_ENLISTED_FILE)RtlGetElementGenericTable( &pTransactionContext->m_EnlistedFilesTable, 0 );
	// free enlisted file entries..
			while (pFileEntry)
			{
				RtlDeleteElementGenericTable( &pTransactionContext->m_EnlistedFilesTable, pFileEntry );
				pFileEntry = (PMKAV_ENLISTED_FILE)RtlGetElementGenericTable( &pTransactionContext->m_EnlistedFilesTable, 0 );
			};

	// now free registry key entries..
			pKeyEntry = (PMKAV_ENLISTED_KEY)RtlGetElementGenericTable( &pTransactionContext->m_EnlistedKeysTable, 0 );
			while (pKeyEntry)
			{
				RtlDeleteElementGenericTable( &pTransactionContext->m_EnlistedKeysTable, pKeyEntry );
				pKeyEntry = (PMKAV_ENLISTED_KEY)RtlGetElementGenericTable( &pTransactionContext->m_EnlistedKeysTable, 0 );
			};

			ExDeleteResourceLite( &pTransactionContext->m_RegLock );
			ExDeleteResourceLite( &pTransactionContext->m_FileLock );

			DoTraceEx(TRACE_LEVEL_INFORMATION, DC_DRIVER, "cleanup transaction context %p on transaction %p",
				pTransactionContext, pTransactionContext->m_pTransObj );

			pTransactionContext->m_pTransObj = NULL;
		}
	}
}

// support transactions on vista
const FLT_CONTEXT_REGISTRATION ContextRegistration[] = {
	{FLT_INSTANCE_CONTEXT,		0, MiniKavContextCleanup,		sizeof(MKAV_INSTANCE_CONTEXT),		tag_instance_ctx,	NULL, NULL, NULL},
	{FLT_STREAM_CONTEXT,		0, Serv_ContextCleanup,			sizeof(MKAV_STREAM_CONTEXT),		tag_stream_ctx,		NULL, NULL, NULL},
	{FLT_STREAMHANDLE_CONTEXT,	0, Serv_ContextCleanup,			sizeof(MKAV_STREAM_HANDLE_CONTEXT),	tag_streamh_ctx,	NULL, NULL, NULL},
	{FLT_VOLUME_CONTEXT,		0, NULL,						sizeof(MKAV_VOLUME_CONTEXT),		tag_volume_ctx,		NULL, NULL, NULL},
	{FLT_TRANSACTION_CONTEXT,	0, TransactionContextCleanup,	sizeof(MKAV_TRANSACTION_CONTEXT),	tag_tran_ctx,		NULL, NULL, NULL},
	{FLT_CONTEXT_END}
};

#define _skip_paging			FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO
#define _irp_execute_section	IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION

// dont remove MiniKavPreCreate - self defence (open+truncate will leave zero size file)
CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
	{IRP_MJ_CREATE,					0,				MiniKavPreCreate,			MiniKavPostCreate},
	{IRP_MJ_CLEANUP,				0,				MiniKavPreCleanup,			MiniKavPostCleanup},
	{IRP_MJ_READ,					_skip_paging,	MiniKavPreRead,				MiniKavPostRead},
	{IRP_MJ_WRITE,					_skip_paging,	MiniKavPreWrite,			MiniKavPostWrite},
	{IRP_MJ_SET_INFORMATION,		0,				MiniKavPreSetInformation,	MiniKavPostSetInformation},
	{IRP_MJ_FILE_SYSTEM_CONTROL,	0,				MiniKavPreFSCTRL,			MiniKavPostFSCTRL},
	{IRP_MJ_QUERY_INFORMATION,		0,				NULL,						MiniKavPostQueryInformation},
	{IRP_MJ_QUERY_EA,				0,				MiniKavPreQueryEA,			NULL},
	{_irp_execute_section,			0,				MiniKavPreSyncSection,		MiniKavPostSyncSection},
	{IRP_MJ_NETWORK_QUERY_OPEN,		0,				MiniKavPreNetQueryOpen,		NULL},
	{IRP_MJ_OPERATION_END}
};

VOID
GlobalDataDone (
	)
{
	if (Globals.m_RegParams.Buffer)
	{
		ExFreePool( Globals.m_RegParams.Buffer );
		Globals.m_RegParams.Buffer = NULL;
	}
	
	if (Globals.m_RegInstances.Buffer)
	{
		ExFreePool( Globals.m_RegInstances.Buffer );
		Globals.m_RegInstances.Buffer = NULL;
	}

	if (Globals.m_FilterControlDeviceObject)
		DeleteControlDeviceObject();

	if (Globals.m_ServerPort)
		Comm_ClosePort();

	if (Globals.m_Filter)
	{
		FltUnregisterFilter( Globals.m_Filter );
		Globals.m_Filter = NULL;
	}

	//todo: delete items
	Timing_Clear();
	ExDeleteResourceLite( &Globals.m_TimingLock );
	ExDeleteResourceLite( &Globals.m_FidBoxLock );
	ExDeleteResourceLite( &Globals.m_Resource );
	ExDeleteResourceLite( &Globals.m_ResourceFileNameInfo );
}

NTSTATUS
DriverEntry (
	__in PDRIVER_OBJECT DriverObject,
	__in PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING parameters;
	ULONG FileIdMode = 0;


#if DBG == 1
#pragma message ("--- debug driver ---")
#endif


	FLT_REGISTRATION_VISTA filterRegistration = {
		sizeof(FLT_REGISTRATION),		//  Size
		FLT_REGISTRATION_VERSION,		//  Version
		0,								//  Flags
		ContextRegistration,			//  Context
		Callbacks,						//  Operation callbacks
		NULL,							//  see later
		MiniKavInstanceSetup,			//  InstanceSetup
		NULL,							//  InstanceQueryTeardown
		NULL,					        //  InstanceTeardownStart
		NULL,						    //  InstanceTeardownComplete
		NULL, NULL,						//  NameProvider callbacks
		NULL,
		// vista/longhorn
		MiniKavTransactionNotify,		//  transaction callback for Vista
		NULL							// 
	};


	if (*InitSafeBootMode > 0)
	{
		_dbg_break_;
		return STATUS_UNSUCCESSFUL;
	}

	RtlZeroMemory( &Globals, sizeof( Globals) );

	Globals.m_DrvFlags |= _DRV_FLAG_MINIFILTER | _DRV_FLAG_HAS_DISPATCH_FLT;
#if DBG == 1
	Globals.m_DrvFlags |= _DRV_FLAG_DEBUG;
#endif

	//+ init registry key name
	RtlInitUnicodeString( &parameters, L"\\Parameters" );
	Globals.m_RegParams.Length = 0;
	Globals.m_RegParams.MaximumLength = RegistryPath->Length + parameters.Length + sizeof(WCHAR); 
	Globals.m_RegParams.Buffer = ExAllocatePoolWithTag (
		PagedPool,
		Globals.m_RegParams.MaximumLength,
		tag_env_param
		);

	if(!Globals.m_RegParams.Buffer)
	{ 
		status = STATUS_INSUFFICIENT_RESOURCES;
		return status;
	}

	RtlZeroMemory( Globals.m_RegParams.Buffer, Globals.m_RegParams.MaximumLength );
	RtlAppendUnicodeStringToString( &Globals.m_RegParams, RegistryPath );
	RtlAppendUnicodeStringToString( &Globals.m_RegParams, &parameters );
	//- init registry key name

	Globals.m_FilterDriverObject = DriverObject;

	ExInitializeResourceLite( &Globals.m_Resource );
	ExInitializeResourceLite( &Globals.m_ResourceFileNameInfo );
	ExInitializeResourceLite( &Globals.m_FidBoxLock );

	InitializeListHead( &Globals.m_Timing );
	ExInitializeResourceLite( &Globals.m_TimingLock );
	Timing_Init();

#ifndef WIN2K
	WPP_INIT_TRACING(DriverObject, RegistryPath );
#endif // WIN2K

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "DriverEntry" );

	Resolve_Init();
#ifdef WIN2K
	// no unload routine
	CheckSysPatch(); //- check crash
#else
	if (IsUnloadAllowed())
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Unload allowed" );
		filterRegistration.FilterUnloadCallback = MiniKavUnload;
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_SUPPORT_UNLOAD );
	}
	else
		CheckSysPatch();
#endif // WIN2K

	if (IsCalcHashAllowed())
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Calc hashes" );
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_SUPPORT_PROC_HASH );
	}

	if (IsISwiftDisabled())
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "ISwift disabled" );
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED );
	}

	if (IsINetSwiftDisabled())
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "INetSwift disabled" );
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_INETSWIFT_DISABLED );
	}

	if (IsTimingEnabled())
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Timing enabled" );
		SetFlag( Globals.m_DrvFlags, _DRV_FLAG_TIMING );
	}

	FileIdMode = GetFileIdMode();
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FileIdMode %d", FileIdMode );
	switch(FileIdMode)
	{
	case 2:
		{
			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "ISwift switched to index id" );
			SetFlag( Globals.m_DrvFlags, _DRV_FLAG_FILEID_INDEX );
			ClearFlag( Globals.m_DrvFlags, _DRV_FLAG_FILEID_ID );
		}
		break;

	default:
		{
			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "ISwift switched to ntfs fileid" );
			SetFlag( Globals.m_DrvFlags, _DRV_FLAG_FILEID_ID );
		}
	}

//----------------------------------------------------------------------
	if (g_bIsVista && g_bTransactionOperationsSupported)
	{
		DoTraceEx(TRACE_LEVEL_INFORMATION, DC_DRIVER, "running on Vista, initializing transaction support..");

		// for Vista we support transactions - let it have context & callback
		filterRegistration.Size = sizeof(FLT_REGISTRATION_VISTA);
		filterRegistration.Version = FLT_REGISTRATION_VERSION_0202;
	}
//----------------------------------------------------------------------

	GlobalOsSpecInit();

	status = FltRegisterFilter( DriverObject,
		(PFLT_REGISTRATION) &filterRegistration,
		&Globals.m_Filter );

	if (!NT_SUCCESS( status ))
	{
		_dbg_break_;
		GlobalDataDone();
		return status;
	}
	
	{
		UNICODE_STRING usDrvName;
		UNICODE_STRING instances;
		USHORT i = 0;
		
		i = RegistryPath->Length/2 - 1;
		while ( i > 0 )
		{
			if ( RegistryPath->Buffer[ i ] == L'\\' )
				break;
			i--;
		}

		RtlInitEmptyUnicodeString( 
						&usDrvName, 
						&(RegistryPath->Buffer[ i ]), 
						( RegistryPath->Length/2 - i )*2 
						);

		usDrvName.Length = 	( RegistryPath->Length/2 - i )*2 ;
	
		status = Comm_CreatePort( &usDrvName );
		if (!NT_SUCCESS( status ))
		{
			_dbg_break_;
			GlobalDataDone();
			return status;
		}
	
		status = CreateControlDeviceObject( DriverObject,  &usDrvName );

		if (!NT_SUCCESS( status ))
		{
			_dbg_break_;
			GlobalDataDone();

			return status;
		}
		
		//+ init registry key name
		RtlInitUnicodeString( &instances, L"\\Instances" );
		Globals.m_RegInstances.Length = 0;
		Globals.m_RegInstances.MaximumLength = RegistryPath->Length + 
												instances.Length + 
												usDrvName.Length + sizeof(WCHAR); 
		
		Globals.m_RegInstances.Buffer = ExAllocatePoolWithTag (
			PagedPool,
			Globals.m_RegInstances.MaximumLength,
			tag_env_param2
			);

		if(!Globals.m_RegInstances.Buffer)
		{ 
			status = STATUS_INSUFFICIENT_RESOURCES;
			GlobalDataDone();
			return status;
		}

		RtlZeroMemory( Globals.m_RegInstances.Buffer, Globals.m_RegInstances.MaximumLength );
		
		status = RtlAppendUnicodeStringToString( &Globals.m_RegInstances, RegistryPath );
		if ( NT_SUCCESS(status) )
			status = RtlAppendUnicodeStringToString( &Globals.m_RegInstances, &instances );
		
		if ( NT_SUCCESS(status) )
			status = RtlAppendUnicodeStringToString( &Globals.m_RegInstances, &usDrvName );
		
		if ( !NT_SUCCESS(status) )
		{ 
			GlobalDataDone();
			return status;
		}
		//- init registry key name
	}
	
	if (NT_SUCCESS( status ))
	{
		BOOLEAN bServicesInited;

		GlobalSystemDataInit();
		Fid_GenericInit();

		bServicesInited = GlobalInvThreadInit();

		if (bServicesInited)
			bServicesInited = GlobalFiltersInit();

		if (bServicesInited)
			bServicesInited = GlobalClientsInit();

		GlobalProcCheckInit();

		if (bServicesInited)
		{
			SetSystemNotifiers();
			RegFltInit();

			status = FltStartFiltering( Globals.m_Filter );

			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "start filtering status %!STATUS!",
				status);
		}
		
		if (!NT_SUCCESS( status ))
		{
			status = STATUS_UNSUCCESSFUL;

			GlobalClientsDone();
			GlobalFiltersDone();
			GlobalProcCheckDone();
			GlobalInvThreadDone();
			GlobalSystemDataDone();
		}
	}

	if (!NT_SUCCESS( status ))
	{
		_dbg_break_;
		GlobalDataDone();
		
		return status;
	}

	SpecialFileProcessingInit();
	SpecialFileProcessingGo();
	LoadFilters();

	return status;
}

VOID
StopFiltering (
	)
{
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "StopFiltering..." );

	RegFltDone();

	Comm_ClosePort();

	GlobalFidBoxDone( FALSE );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Detaching..." );
	Serv_DetachFromSystemVolume();

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FltUnregisterFilter..." );
	FltUnregisterFilter( Globals.m_Filter );
	Globals.m_Filter = NULL;
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FltUnregisterFilter complete" );

	DeleteControlDeviceObject();

	RemoveSystemNotifiers();

	while (1 != InterlockedIncrement( &Globals.m_WorkItemsInWork ))
	{
		InterlockedDecrement( &Globals.m_WorkItemsInWork );

		SleepImp( 1 );
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "stopping clients" );
	GlobalClientsDone();
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "clients stopped" );	

	GlobalFiltersDone();
	GlobalProcCheckDone();
	SpecialFileProcessingDone();
	GlobalInvThreadDone();
	GlobalSystemDataDone();

	GlobalDataDone();
}
#ifndef WIN2K
NTSTATUS
MiniKavUnload (
	FLT_FILTER_UNLOAD_FLAGS Flags
	)
{
	PAGED_CODE();

	UNREFERENCED_PARAMETER( Flags );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Unload request" );

	if (!FlagOn(Flags, FLTFL_FILTER_UNLOAD_MANDATORY))
	{
		// необязательная выгрузка
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Unload (FLTFL_FILTER_UNLOAD_MANDATORY)" );
		return STATUS_FLT_DO_NOT_DETACH;
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Unloading..." );

	StopFiltering();

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Unloaded" );

	WPP_CLEANUP(Globals.m_FilterDriverObject);

	return STATUS_SUCCESS;
}

#endif // WIN2K

//
//  Instance setup routine
//

NTSTATUS InstanceSetup_QuerySysVol(
	__in PCFLT_RELATED_OBJECTS FltObjects
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PWCHAR wcWindowsDir = L"\\SystemRoot\\";
	UNICODE_STRING usWindowsDir;
	OBJECT_ATTRIBUTES WindowsDirOA;
	IO_STATUS_BLOCK	ioStatus;
	HANDLE hWindowsDir;

	if ( !(FltObjects && FltObjects->Volume) )
		return STATUS_INVALID_PARAMETER;

	RtlInitUnicodeString( &usWindowsDir, wcWindowsDir );
	InitializeObjectAttributes( &WindowsDirOA, &usWindowsDir, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

// get system volume
	status = FltCreateFile (
		Globals.m_Filter,
		NULL, 
		&hWindowsDir,
		FILE_LIST_DIRECTORY | SYNCHRONIZE,
		&WindowsDirOA,
		&ioStatus,
		NULL,
		0UL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		0
		); 
	
	if (NT_SUCCESS(status))
	{
		PFILE_OBJECT pWindowsDirFO = NULL;

		status = ObReferenceObjectByHandle (
			hWindowsDir,
			0,
			*IoFileObjectType,
			KernelMode,
			&pWindowsDirFO,
			NULL
			);
		
		if (NT_SUCCESS(status))
		{
			PFLT_VOLUME pSysVol = NULL;

			status = FltGetVolumeFromFileObject( Globals.m_Filter, pWindowsDirFO, &pSysVol );
			if (NT_SUCCESS(status))
			{
				status = STATUS_UNSUCCESSFUL;
				if (FltObjects->Volume == pSysVol)
					status = STATUS_SUCCESS;

				FltObjectDereference( pSysVol );
			}

			ObDereferenceObject(pWindowsDirFO);
		}

		FltClose( hWindowsDir );
	}

	return status;
}

DWORD NtfsStart = 0;
DWORD NtfsSize = 0;

NTSTATUS
MiniKavInstanceSetup (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in FLT_INSTANCE_SETUP_FLAGS Flags,
	__in DEVICE_TYPE VolumeDeviceType,
	__in FLT_FILESYSTEM_TYPE VolumeFilesystemType
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PFLT_VOLUME_PROPERTIES pvolprops = NULL;
	PMKAV_VOLUME_CONTEXT pVolumeContext = NULL;

	ULONG len_required;

	UNREFERENCED_PARAMETER( Flags );

	PAGED_CODE();

	ASSERT( FltObjects->Filter == Globals.m_Filter );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Attach request..." );

	if (FLT_FSTYPE_RAW == VolumeFilesystemType)
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "raw volume - skip" );
		
		return STATUS_FLT_DO_NOT_ATTACH;
	}

	pvolprops = ExAllocatePoolWithTag( PagedPool, 0x1000, 'Dbos' );
	if (!pvolprops)
		return STATUS_FLT_DO_NOT_ATTACH;

	status = FltGetVolumeProperties( FltObjects->Volume, pvolprops, 0x1000, &len_required );
	if (!NT_SUCCESS(status))
	{
		ExFreePool( pvolprops );

		return STATUS_FLT_DO_NOT_ATTACH;
	}

	//  Allocate context structure
	status = FltAllocateContext (
		Globals.m_Filter,
		FLT_INSTANCE_CONTEXT,
		sizeof(MKAV_INSTANCE_CONTEXT),
		NonPagedPool,
		&pInstanceContext
		);

	if (!NT_SUCCESS(status))
	{
		//ReleaseFltContext( &pInstanceContext );
		DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "Allocate instance context failed" );

		ExFreePool( pvolprops );

		return STATUS_FLT_DO_NOT_ATTACH;
	}
	
	RtlZeroMemory( pInstanceContext, sizeof(MKAV_INSTANCE_CONTEXT) );

	status = FltAllocateContext (
		Globals.m_Filter,
		FLT_VOLUME_CONTEXT,
		sizeof(MKAV_VOLUME_CONTEXT),
		NonPagedPool,
		&pVolumeContext
		);
	
	if (!NT_SUCCESS(status))
	{
		ReleaseFltContext( &pInstanceContext );
		ExFreePool( pvolprops );

		return STATUS_FLT_DO_NOT_ATTACH;
	}

	pInstanceContext->m_SectorSize = pvolprops->SectorSize;
	if (!pInstanceContext->m_SectorSize)
		pInstanceContext->m_SectorSize = 512;

	ExFreePool( pvolprops );

	if (FLT_FSTYPE_NTFS == VolumeFilesystemType)
	{
		int Object_Size1 = RTL_FIELD_SIZE(FILE_FS_OBJECTID_INFORMATION, ObjectId);
		int Object_Size2 = RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId);

		IO_STATUS_BLOCK	ioStatus;
		
		FILE_FS_OBJECTID_INFORMATION VolumeIdInformation;
		PDEVICE_OBJECT pDev = NULL; 

#ifndef _WIN64
		if (FltObjects)
			if (FltObjects->FileObject)
			if (FltObjects->FileObject->DeviceObject)
			{
				pDev = FltObjects->FileObject->DeviceObject;
				NtfsStart = (DWORD)(pDev->DriverObject->DriverStart); 
				NtfsSize =  (DWORD)(pDev->DriverObject->DriverSize);
			}
		
#endif
		SetFlag( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_FLAG_NTFS );

		if (Object_Size1 == Object_Size2)
		{
			status = FltQueryVolumeInformation (
				FltObjects->Instance,
				&ioStatus,
				&VolumeIdInformation,
				sizeof(VolumeIdInformation),
				FileFsObjectIdInformation
				);

			if (NT_SUCCESS(status))
			{
				SetFlag( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_HAS_OBJID );
				RtlCopyMemory( pInstanceContext->m_VolumeId.m_ObjectId, VolumeIdInformation.ObjectId, Object_Size1 );
			}
		}
	}

	len_required = 0;

	status = FltGetVolumeName( FltObjects->Volume, NULL, &len_required );
	if (STATUS_BUFFER_TOO_SMALL == status && len_required)
	{
		RtlInitEmptyUnicodeString (
			&pInstanceContext->m_VolumeName,
			ExAllocatePoolWithTag(NonPagedPool, len_required, 'Vbos'),
			(USHORT) len_required
			);

		if (pInstanceContext->m_VolumeName.Buffer)
		{
			status = FltGetVolumeName( FltObjects->Volume, &pInstanceContext->m_VolumeName, NULL );
			if (NT_SUCCESS(status))
			{
				DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "InstanceSetup '%wZ', type 0x%x",
					&pInstanceContext->m_VolumeName,
					VolumeDeviceType );

				if (FILE_DEVICE_NETWORK_FILE_SYSTEM != VolumeDeviceType)
				{
					UNICODE_STRING usp;
					#define _disk_pattern	L"*\\DR?"
					PWCHAR pwchSysPattern = _disk_pattern;

					RtlInitEmptyUnicodeString( &usp, pwchSysPattern, sizeof(_disk_pattern) - sizeof(WCHAR) );
					usp.Length = usp.MaximumLength;

					if (FsRtlIsNameInExpression( &usp, &pInstanceContext->m_VolumeName, TRUE, NULL ))
					{
						_dbg_break_;
					}
					else
					{
						PFILTER_EVENT_PARAM pEvent = EvContext_MountVolume( &pInstanceContext->m_VolumeName );
						if (pEvent)
						{
							FilterEventImp( pEvent, NULL, 0 );
							EvContext_Done( pEvent );
						}
					}
				}
			}
			else
			{
				ExFreePool( pInstanceContext->m_VolumeName.Buffer );
				pInstanceContext->m_VolumeName.Buffer = 0;
			}
		}
	}

	if (FILE_DEVICE_NETWORK_FILE_SYSTEM == VolumeDeviceType)
		pInstanceContext->m_ContextFlags |= _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE;

	pInstanceContext->m_DeviceType = VolumeDeviceType;

	status = FltSetInstanceContext( FltObjects->Instance, FLT_SET_CONTEXT_KEEP_IF_EXISTS, pInstanceContext, NULL );

	if (STATUS_FLT_CONTEXT_ALREADY_DEFINED == status)
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "instance context already exist!" );
		_dbg_break_;
	}

	if (!NT_SUCCESS(status))
	{
		ASSERTMSG("Failed setting an instance context.", FALSE );
		_dbg_break_;
	}
	
	ReleaseFltContext( &pInstanceContext );

	pVolumeContext->m_pFltInstance = FltObjects->Instance;
	status = FltSetVolumeContext( FltObjects->Volume, FLT_SET_CONTEXT_KEEP_IF_EXISTS, pVolumeContext, NULL );
	if (STATUS_FLT_CONTEXT_ALREADY_DEFINED == status)
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "volume context already exist!" );
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "Attach request complete" );

	//SpecialFileProcessingGo();

	if (!Globals.m_SystemVolumeInstance)
	{
		status = InstanceSetup_QuerySysVol( FltObjects );
		if (NT_SUCCESS(status))
		{
			FltObjectReference( pVolumeContext->m_pFltInstance );

			AcquireResourceExclusive( &Globals.m_Resource );
			Globals.m_SystemVolumeInstance = pVolumeContext->m_pFltInstance;
			ReleaseResource( &Globals.m_Resource );

			if (!FlagOn( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED ))
				Serv_AttachFidBox();
		}
	}

	ReleaseFltContext( &pVolumeContext );

	return STATUS_SUCCESS;
}

VOID
MiniKavContextCleanup (
	__in PFLT_CONTEXT Context, __in FLT_CONTEXT_TYPE ContextType
	)
{
	if (ContextType == FLT_INSTANCE_CONTEXT)
	{
		PMKAV_INSTANCE_CONTEXT pInstanceContext = (PMKAV_INSTANCE_CONTEXT) Context;

		if (pInstanceContext->m_VolumeName.Buffer)
		{
			PFILTER_EVENT_PARAM pEvent = EvContext_UnMountVolume( &pInstanceContext->m_VolumeName );
			if (pEvent)
			{
				FilterEventImp( pEvent, NULL, 0 );
				EvContext_Done( pEvent );
			}

			ExFreePool( pInstanceContext->m_VolumeName.Buffer );
			pInstanceContext->m_VolumeName.Buffer = 0;
		}
	}
}

//+ ------------------------------------------------------------------------------------------
FLT_PREOP_CALLBACK_STATUS
MiniKavPreFSCTRL (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	)
{
	FLT_PREOP_CALLBACK_STATUS fltStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;
	tefVerdict Verdict = efVerdict_Default;

	PAGED_CODE();

	UNREFERENCED_PARAMETER( CompletionContext );

	status = FltGetInstanceContext( FltObjects->Instance, &pInstanceContext );
	
	if (!NT_SUCCESS(status))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	if (IRP_MN_MOUNT_VOLUME == Data->Iopb->MinorFunction || (IRP_MN_VERIFY_VOLUME == Data->Iopb->MinorFunction))
	{
		fltStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );

	if (pContext)
	{
		if ( IRP_MN_USER_FS_REQUEST == Data->Iopb->MinorFunction )
		if ( FSCTL_GET_RETRIEVAL_POINTERS == Data->Iopb->Parameters.FileSystemControl.Common.FsControlCode )
		{
			if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x27 /*MJ_SYSTEM_GET_FILESECTORS*/, 0))
			{
				PFILTER_EVENT_PARAM pEvent = EvContext_GET_RETRIEVAL_POINTERS(
					&pContext->m_pFileNameInfo->Name,
					&pContext->m_pFileNameInfo->Volume
					);

				if (pEvent)
				{
					Verdict = FilterEventImp( pEvent, NULL, 0 );
					ExFreePool(pEvent);
				}
			}
		}

		ReleaseFltContext( &pContext );
	}

	if ( efIsDeny( Verdict ) )
	{
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		Data->IoStatus.Information = 0;
		return FLT_PREOP_COMPLETE;
	}

	ReleaseFltContext( &pInstanceContext );

	return fltStatus;
}

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostFSCTRL (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	FLT_POSTOP_CALLBACK_STATUS fltStatus = FLT_POSTOP_FINISHED_PROCESSING;

	UNREFERENCED_PARAMETER( CompletionContext );
	
	PAGED_CODE();

	if (FLTFL_POST_OPERATION_DRAINING & Flags)
		return FLT_POSTOP_FINISHED_PROCESSING;

	if (NT_SUCCESS(Data->IoStatus.Status))
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
		
		status = FltGetInstanceContext( FltObjects->Instance, &pInstanceContext );
		
		if (NT_SUCCESS(status))
		{
			if ((IRP_MN_MOUNT_VOLUME == Data->Iopb->MinorFunction)
				|| (IRP_MN_VERIFY_VOLUME == Data->Iopb->MinorFunction))
			{
				PFILTER_EVENT_PARAM pEvent = EvContext_MountVolume( &pInstanceContext->m_VolumeName );
				if (pEvent)
				{
					//pEvent->FunctionMi = Data.Iopb->MinorFunction;
					FilterEventImp( pEvent, NULL, 0 );
					EvContext_Done( pEvent );
				}
			}
			
			ReleaseFltContext( &pInstanceContext );
		}
	}

	return fltStatus;
}

//MiniKavPreNetQueryOpen

FLT_PREOP_CALLBACK_STATUS
MiniKavPreNetQueryOpen (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER( CompletionContext );
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( Data );

	//виртуализация
	//Очень нужная обработка - без нее не возможно увидеть содержимое подкаталогов
// 	if (FLT_IS_FASTIO_OPERATION(Data))
// 	{
// 		if ( Virt_IsProcessInSB( PsGetCurrentProcessId() ) )
// 				return FLT_PREOP_DISALLOW_FASTIO;
// 	}
	
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

int
CompareUnicodeTail ( PUNICODE_STRING usString, PUNICODE_STRING usTail )
{
	int res = 0;
	res = usString->Length -  usTail->Length;

	if ( res < 0 )
		return res;

	// 	res = memcmp( 
	// 			(char*)usString->Buffer + usString->Length - (usTail->Length),
	// 			usTail->Buffer,
	// 			usTail->Length 
	// 			);

	res = _wcsnicmp(
		(PWCHAR)((char*)usString->Buffer + usString->Length - (usTail->Length)),
		usTail->Buffer,
		usTail->Length/sizeof(WCHAR) 
		);

	return res;
}

FLT_PREOP_CALLBACK_STATUS
MiniKavPreCreate (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	)
{
	FLT_PREOP_CALLBACK_STATUS fltStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	MKAV_STREAM_CONTEXT StreamContext;
	MKAV_STREAM_HANDLE_CONTEXT StreamHandleContext;

	PFILTER_EVENT_PARAM pEvent = NULL;
	PIRP pTopLevelIrp = NULL;

	tefVerdict Verdict = efVerdict_Default;
	Timing_Def;

	PAGED_CODE();

	UNREFERENCED_PARAMETER( CompletionContext );

	if (IsSkipOp( FltObjects, 0 ))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	if (KernelMode == Data->RequestorMode)
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;

	pTopLevelIrp = IoGetTopLevelIrp();
	if (pTopLevelIrp)
	{
		_dbg_break_;
	}

// 	if (FsRtlIsPagingFile( FltObjects->FileObject ))
// 	{
// 		_dbg_break_;
// 		return FLT_PREOP_SUCCESS_NO_CALLBACK;
// 	}

	if (FlagOn( Data->Iopb->OperationFlags, SL_OPEN_PAGING_FILE ))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_PRECREATE_DISABLED ))
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;

	Timing_In;

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	RtlZeroMemory( &StreamContext, sizeof(StreamContext) );
	RtlZeroMemory( &StreamHandleContext, sizeof(StreamHandleContext) );

	SeGetUserSid( Data, &StreamHandleContext.m_pSid);
	SeGetUserLuid( &StreamHandleContext.m_Luid );
	
	pEvent = EvContext_Create( pInstanceContext, Data, FltObjects, &StreamContext, &StreamHandleContext, TRUE );
	if (pEvent)
	{
		Verdict = FilterEventImp( pEvent, NULL, 0 );
		EvContext_Done( pEvent );
	}
	
	//виртуализация
	//сеть не виртуализируем
// 	if ( efIsAllow( Verdict ) && 
// 		!BooleanFlagOn( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE ) &&
// 		Virt_IsProcessInSB( PsGetCurrentProcessId() )
// 		)
// 	{
// 		UNICODE_STRING FilePath;
// 		UNICODE_STRING VolumeName;
// 		UNICODE_STRING usNewFilePath;
// 		BOOLEAN mustClear = FALSE;
// 		NTSTATUS status = STATUS_UNSUCCESSFUL;
// 		
// 		RtlZeroMemory( &FilePath, sizeof(UNICODE_STRING) );
// 		RtlZeroMemory( &FilePath, sizeof(VolumeName) );
// 		RtlZeroMemory( &usNewFilePath, sizeof(UNICODE_STRING) );
// 				
// 		if ( StreamContext.m_pFileNameInfo &&
// 			CompareUnicodeTail ( &StreamContext.m_pFileNameInfo->Name, &Data->Iopb->TargetFileObject->FileName ) == 0 
// 			)
// 		{
// 			memcpy(&FilePath, &StreamContext.m_pFileNameInfo->Name, sizeof(UNICODE_STRING) );
// 			memcpy(&VolumeName, &StreamContext.m_pFileNameInfo->Volume, sizeof(UNICODE_STRING) );
// 			status = STATUS_SUCCESS;
// 		}
// 		else
// 		{
// 			memcpy(&VolumeName, &pInstanceContext->m_VolumeName, sizeof(UNICODE_STRING) );
// 
// 			status = GetFullPath(
// 				&pInstanceContext->m_VolumeName,
// 				FltObjects->Instance,
// 				Data->Iopb->TargetFileObject,
// 				&FilePath
// 				);
// 			
// 			if ( NT_SUCCESS (status) )
// 				mustClear = TRUE;
// 		}
// 		
// 		if ( NT_SUCCESS (status) )
// 		{
// 			Verdict = GetVirtualPathFile(
// 						Data, 
// 						PsGetCurrentProcessId(),
// 						FltObjects->Instance,
// 						&FilePath,
// 						&VolumeName,
// 						Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess,
// 						(Data->Iopb->Parameters.Create.Options)&0xFFF,
// 						Data->Iopb->Parameters.Create.Options >> 24,
// 						&usNewFilePath	);
// 			
// 			if (mustClear)
// 			{
// 				ExFreePool(FilePath.Buffer);
// 				RtlZeroMemory( &FilePath, sizeof(UNICODE_STRING) );
// 			}
// 		}
// 	
// 		//если надо виртуализироваться
// 		if ( efIsAllow( Verdict ) &&  usNewFilePath.Length )
// 		{
// 			PFILE_OBJECT FileObject;
// 			FileObject=Data->Iopb->TargetFileObject;
// 
// 			if(FileObject->FileName.Buffer) {
// 				ExFreePool(FileObject->FileName.Buffer);
// 			}
// 
// 			FileObject->FileName.Length = usNewFilePath.Length;
// 			FileObject->FileName.MaximumLength = usNewFilePath.MaximumLength;
// 			FileObject->FileName.Buffer = usNewFilePath.Buffer;
// 
// 			//FileObject->RelatedFileObject=NULL;
// 
// 			Data->IoStatus.Status = STATUS_REPARSE;
// 			Data->IoStatus.Information = IO_REPARSE;
// 
// 			FltSetCallbackDataDirty( Data );
// 
// 			Serv_ContextCleanup( &StreamContext, FLT_STREAM_CONTEXT );
// 			Serv_ContextCleanup( &StreamHandleContext, FLT_STREAMHANDLE_CONTEXT );
// 
// 			ReleaseFltContext( &pInstanceContext );
// 
// 			return FLT_PREOP_COMPLETE;
// 		}
// 	}

	if (efIsAllow( Verdict ) &&  StreamContext.m_pFileNameInfo )
	{
		ULONG desiredMask = 0;
		PIO_SECURITY_CONTEXT pio_sc = Data->Iopb->Parameters.Create.SecurityContext;

		if ( FlagOn( pio_sc->DesiredAccess, FILE_WRITE_DATA) )
			SetFlag( desiredMask, FILE_OP_WRITE );
		
		if ( FlagOn(pio_sc->DesiredAccess,FILE_READ_DATA) )
			SetFlag( desiredMask, FILE_OP_READ );
		
		if ( FlagOn(Data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE) )
			SetFlag( desiredMask, FILE_OP_DELETE );

		Verdict = IsAllowAccessFile (
			Data, 
			PsGetCurrentProcessId(), 
			StreamContext.m_pFileNameInfo,
			desiredMask
			//FILE_OP_WRITE | FILE_OP_READ | FILE_OP_ENUM | FILE_OP_DELETE
			);
	}

	Serv_ContextCleanup( &StreamContext, FLT_STREAM_CONTEXT );
	Serv_ContextCleanup( &StreamHandleContext, FLT_STREAMHANDLE_CONTEXT );

	ReleaseFltContext( &pInstanceContext );
	Timing_Out( _timing_file, IRP_MJ_CREATE, PreProcessing );

	if (efIsAllow( Verdict ))
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "PreCreate op: STATUS_ACCESS_DENIED" );

	Data->IoStatus.Status = STATUS_ACCESS_DENIED;
	Data->IoStatus.Information = 0;

	return FLT_PREOP_COMPLETE;
}

// get & ref system volume instance atomicly

VOID
TransEnlistFile(
	__inout PMKAV_TRANSACTION_CONTEXT pTransactionContext,
	__in PMKAV_STREAM_CONTEXT pContext
	)
{
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = LockFileNameInfo( pContext );

	PAGED_CODE();

	if (pFileNameInfo)
	{
		PMKAV_ENLISTED_FILE pFileEntry = NULL;
		ULONG item_size = sizeof(MKAV_ENLISTED_FILE) + pFileNameInfo->Name.Length + sizeof(WCHAR);
		
		pFileEntry = (PMKAV_ENLISTED_FILE) ExAllocatePoolWithTag (
			NonPagedPool,
			item_size,
			'TbOs'
			);

		if (pFileEntry)
		{
			PMKAV_ENLISTED_FILE pResult = NULL;

			#pragma message ("----------------------- TODO: fix process id")
			pFileEntry->m_ProcessId = HandleToUlong( PsGetCurrentProcessId() );

			memcpy( pFileEntry->m_FileName, pFileNameInfo->Name.Buffer, pFileNameInfo->Name.Length );
			pFileEntry->m_FileName[pFileNameInfo->Name.Length/sizeof(WCHAR)] = 0;
			pFileEntry->m_NameLen = pFileNameInfo->Name.Length + sizeof(WCHAR);

			AcquireResourceExclusive( &pTransactionContext->m_FileLock );

			pResult = RtlInsertElementGenericTable (
				&pTransactionContext->m_EnlistedFilesTable,
				pFileEntry,
				item_size,
				NULL
				);
			
			ReleaseResource( &pTransactionContext->m_FileLock );

			DoTraceEx( TRACE_LEVEL_INFORMATION, DC_DRIVER, "enlisted file \'%S\' into transaction %p",
				pFileEntry->m_FileName, pTransactionContext->m_pTransObj );

			ExFreePool( pFileEntry );
		}
	}

	ReleaseFileNameInfo( pFileNameInfo );
}

VOID PostCreateFileTransProcessing (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PMKAV_STREAM_CONTEXT pContext
	)
{
	if (KeGetCurrentIrql() > APC_LEVEL)
	{
		_dbg_break_;
		return;
	}

	if (!g_bTransactionOperationsSupported)
		return;

	if (FltObjects->Transaction)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		PMKAV_TRANSACTION_CONTEXT  pTransactionContext = NULL;
		PVOID pSysVolInstance;

		pSysVolInstance = GetSystemVolumeInstance();
		if (pSysVolInstance)
		{
			status = g_pfFltGetTransactionContext( pSysVolInstance, FltObjects->Transaction, (PFLT_CONTEXT*) &pTransactionContext );
			if (!NT_SUCCESS( status ))
			{
				// no context allocated for this transaction, allocate one
				status = FltAllocateContext (
					FltObjects->Filter,
					FLT_TRANSACTION_CONTEXT,
					sizeof(MKAV_TRANSACTION_CONTEXT),
					NonPagedPool,
					(PFLT_CONTEXT*) &pTransactionContext
					);

				if (!NT_SUCCESS( status ))
					pTransactionContext = NULL;
				else
				{
					RtlZeroMemory( pTransactionContext, sizeof(MKAV_TRANSACTION_CONTEXT) );
					status = InitTransactionContext( pTransactionContext, FltObjects->Transaction );
					
					if (!NT_SUCCESS( status ))
						ReleaseFltContext( &pTransactionContext );
					else
					{
						status = g_pfFltSetTransactionContext (
							pSysVolInstance,
							FltObjects->Transaction,
							FLT_SET_CONTEXT_KEEP_IF_EXISTS,
							(PFLT_CONTEXT) pTransactionContext,
							NULL
							);

						if (!NT_SUCCESS( status ))
						{
							// strange..
							_dbg_break_;
							// deref & free context
							ReleaseFltContext( &pTransactionContext );
						}
					}
				}
			}

			if (pTransactionContext)
			{
				// over here we have context referenced 2 times - allocate & set/get
				// enlist(if it is not already enlisted) minifilter into transaction
				g_pfFltEnlistInTransaction (
					pSysVolInstance,
					FltObjects->Transaction,
					pTransactionContext,
					FLT_MAX_TRANSACTION_NOTIFICATIONS
					);

				// enlist file into transaction
				TransEnlistFile( pTransactionContext, pContext);

				ReleaseFltContext( &pTransactionContext );
			}

			FltObjectDereference( pSysVolInstance );
		}
	}
}

VOID
AttachStreamContext (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PMKAV_INSTANCE_CONTEXT pInstanceContext,
	__in PMKAV_STREAM_CONTEXT ptmpContext
	)
{
	NTSTATUS status;
	PMKAV_STREAM_CONTEXT pContext = NULL;

	if (!FsRtlSupportsPerStreamContexts( FltObjects->FileObject ))
		return;

	status = FltAllocateContext (
		Globals.m_Filter,
		FLT_STREAM_CONTEXT,
		sizeof(MKAV_STREAM_CONTEXT),
		PagedPool,
		&pContext
		);
	
	if (!NT_SUCCESS(status))
		return;

	// init stream context data
	*pContext = *ptmpContext;
	Serv_StreamContextReset( ptmpContext );

	if (_CONTEXT_OBJECT_FLAG_NETWORK_DEVICE & pInstanceContext->m_ContextFlags)
	{
		//skip IchDrv_AttachFileId - network file
	}
	else
	{
		if(FlagOn(ptmpContext->m_Flags, _STREAM_FLAGS_DIRECTORY ))
		{
			// some optimization (if disable - check vista + raid)
		}
		else
		{
			if (FlagOn( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_HAS_OBJID ))
				IchDrv_AttachFileId( FltObjects->Instance, FltObjects->FileObject, pContext );
		}
	}
	{
		PMKAV_STREAM_CONTEXT pOldContext = NULL;
		status = FltSetStreamContext ( 
			FltObjects->Instance,
			FltObjects->FileObject,
			FLT_SET_CONTEXT_REPLACE_IF_EXISTS,
			pContext,
			&pOldContext
			);
		
		//ASSERT( NT_SUCCESS(status) );

		if ((STATUS_FLT_CONTEXT_ALREADY_DEFINED == status) || NT_SUCCESS( status ))
		{
			if( pOldContext )
			{
				if (FlagOn( pOldContext->m_Flags, _STREAM_FLAGS_MODIFIED))
				{
					SetFlag( pContext->m_Flags, _STREAM_FLAGS_MODIFIED);
					
					if (pContext->m_pFileNameInfo)
					{
						DoTraceEx( TRACE_LEVEL_INFORMATION, DC_DRIVER, "reopen modified '%wZ",
							&pContext->m_pFileNameInfo->Name
							);
					}
				}

				 FltReleaseContext( pOldContext );
			}
		}
	}

	// transaction stuff
	PostCreateFileTransProcessing( FltObjects, pContext );

	ReleaseFltContext( &pContext );
}

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostCreate (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	FLT_POSTOP_CALLBACK_STATUS fltStatus = FLT_POSTOP_FINISHED_PROCESSING;
	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;

	PFILTER_EVENT_PARAM pEvent = NULL;

	MKAV_STREAM_CONTEXT tmpContext;

	PMKAV_STREAM_HANDLE_CONTEXT pHandleContext = NULL;

	BOOLEAN bSupportStreams = FALSE;
	BOOLEAN bPrefetch = FALSE;
	BOOLEAN bVolumeAccess = FALSE;
	BOOLEAN bNewObject = FALSE;

	Timing_Def;

	PIRP pTopLevelIrp = NULL;

	UNREFERENCED_PARAMETER( CompletionContext );
	
	PAGED_CODE();

	//  If this create was failing anyway, don't bother scanning now.
	if (!NT_SUCCESS(Data->IoStatus.Status) || (STATUS_REPARSE == Data->IoStatus.Status))
		return FLT_POSTOP_FINISHED_PROCESSING;

	if (IsSkipOp( FltObjects, Flags ))
		return FLT_POSTOP_FINISHED_PROCESSING;

	pTopLevelIrp = IoGetTopLevelIrp();
	if (pTopLevelIrp)
		return FLT_POSTOP_FINISHED_PROCESSING;

	Timing_In;

	bPrefetch = IsPrefetchEcpPresent(Globals.m_Filter, Data);

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
		return FLT_POSTOP_FINISHED_PROCESSING;

	Serv_StreamContextReset( &tmpContext );

	if (FlagOn( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_FLAG_NTFS ))
	{
		if (!FlagOn( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE ))
		{
			FILE_STANDARD_INFORMATION file_st;
			ULONG len;

			status = MySynchronousQueryInformationFile (
				FltObjects->Instance,
				FltObjects->FileObject,
				&file_st, sizeof(file_st),
				FileStandardInformation,
				&len
				);
			
			if (NT_SUCCESS( status ))
				tmpContext.m_NumberOfLinks = file_st.NumberOfLinks;
		}
	}


    switch(Data->IoStatus.Information)
	{
	case FILE_CREATED:
	case FILE_OVERWRITTEN:
	case FILE_SUPERSEDED:
		bNewObject = TRUE;
		break;

	default:
		break;
	}

	status = FltAllocateContext (
		Globals.m_Filter,
		FLT_STREAMHANDLE_CONTEXT,
		sizeof(MKAV_STREAM_HANDLE_CONTEXT),
		PagedPool,
		&pHandleContext
		);

	if (NT_SUCCESS( status ))
	{
		RtlZeroMemory( pHandleContext, sizeof(MKAV_STREAM_HANDLE_CONTEXT) );
		SeGetUserSid( Data, &pHandleContext->m_pSid);
		SeGetUserLuid( &pHandleContext->m_Luid );

		if (bPrefetch)
			pHandleContext->m_Flags |= _STREAM_HANDLE_FLAGS_PREFETCH;
		if (bNewObject)
			pHandleContext->m_Flags |= _STREAM_HANDLE_NEW_OBJECT;

		status = FltSetStreamHandleContext (
			FltObjects->Instance,
			FltObjects->FileObject,
			FLT_SET_CONTEXT_REPLACE_IF_EXISTS,
			pHandleContext,
			NULL
			);
		
		//ASSERT( NT_SUCCESS(status) );
	}

	pEvent = EvContext_Create( pInstanceContext, Data, FltObjects, &tmpContext, pHandleContext, FALSE );
	if (pEvent)
	{
		tefVerdict Verdict = efVerdict_Default;

		if (!RtlCompareUnicodeString( &tmpContext.m_pFileNameInfo->Name, &tmpContext.m_pFileNameInfo->Volume, FALSE ))
		{
			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "direct volume access %wZ", &tmpContext.m_pFileNameInfo->Volume);			
			bVolumeAccess = TRUE;
		}

		if (bPrefetch)
		{
			// skip request
		}
		else if (bVolumeAccess)
		{
			pEvent->HookID = FLTTYPE_SYSTEM;
			pEvent->FunctionMj = 0x25;//MJ_SYSTEM_VOLUME_ACCESS
			pEvent->FunctionMi = 0;

			/*if (UserMode == Data->RequestorMode)
				Verdict = FilterEventImp( pEvent, NULL, 0 );*/
		}
		else
		{
			EVENT_OBJ EventObject;
			EvObject_Fill( &EventObject, FltObjects, pInstanceContext );
			
			if (FlagOn( FltObjects->FileObject->Flags, FO_CACHE_SUPPORTED ))
				SetFlag( EventObject.m_Flags, _EVOF_CACHE_AVAIL );

			pEvent->ProcessingType = PostProcessing;
			
			AttachStreamContext( FltObjects, pInstanceContext, &tmpContext );

			if (UserMode == Data->RequestorMode)
				Verdict = FilterEventImp( pEvent, &EventObject, 0 );
		}

		if (efIsAllow( Verdict ))
		{
		}
		else
		{
			Data->IoStatus.Status = STATUS_ACCESS_DENIED;
			Data->IoStatus.Information = 0;

			if (FO_HANDLE_CREATED & FltObjects->FileObject->Flags)
			{
				_dbg_break_;
			}
			else
			{
				DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "PostCreate op: canceled" );

				FltCancelFileOpen( FltObjects->Instance, FltObjects->FileObject );
				fltStatus = FLT_POSTOP_FINISHED_PROCESSING;
			}
		}

		EvContext_Done( pEvent );
	}

	ReleaseFltContext( &pHandleContext );
	ReleaseFltContext( &pInstanceContext );

	ReleaseFileNameInfo( tmpContext.m_pFileNameInfo );

	Timing_Out( _timing_file, IRP_MJ_CREATE, PostProcessing );

	return fltStatus;
}

FLT_PREOP_CALLBACK_STATUS
MiniKavPreCleanup (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	)
{
	NTSTATUS fltStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;
	PMKAV_STREAM_HANDLE_CONTEXT pHandleContext = NULL;

	BOOLEAN bPrefetch = FALSE;
	BOOLEAN bVolumeAccess = FALSE;
	BOOLEAN bNewObject = FALSE;
	Timing_Def;

	PAGED_CODE();

	if (IsSkipOp( FltObjects, 0 ))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

// 	if (FsRtlIsPagingFile( FltObjects->FileObject ))
// 	{
// 		_dbg_break_;
// 		return FLT_PREOP_SUCCESS_NO_CALLBACK;
// 	}

	Timing_In;

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	pHandleContext = GetStreamHandleContext( FltObjects->Instance, FltObjects->FileObject );

	if (pHandleContext)
	{
		if (FlagOn( pHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_PREFETCH ))
			bPrefetch = TRUE;
		if (FlagOn( pHandleContext->m_Flags, _STREAM_HANDLE_NEW_OBJECT ))
			bNewObject = TRUE;
	}

	if (pContext)
	{
		if (!RtlCompareUnicodeString( &pContext->m_pFileNameInfo->Name, &pContext->m_pFileNameInfo->Volume, FALSE ))
		{
			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "direct volume access %wZ", &pContext->m_pFileNameInfo->Volume);
			bVolumeAccess = TRUE;
		}

		if (FlagOn( pContext->m_Flags, _STREAM_FLAGS_MODIFIED ))
		{
			DoTraceEx( TRACE_LEVEL_INFORMATION, DC_DRIVER, "precleanup modified %p %wZ", pContext, &pContext->m_pFileNameInfo->Name );
		}
		else
		{
			DoTraceEx( TRACE_LEVEL_VERBOSE, DC_DRIVER, "precleanup %wZ", &pContext->m_pFileNameInfo->Name );
		}

		if (bNewObject)
			DoTraceEx( TRACE_LEVEL_INFORMATION, DC_DRIVER, "cleanup new object %wZ", &pContext->m_pFileNameInfo->Name );
	}

	if (!bPrefetch && !bVolumeAccess)
	{
		tefVerdict Verdict = efVerdict_Default;

		PFILTER_EVENT_PARAM pEvent = EvContext_Common( pInstanceContext, FltObjects, Data, pContext, pHandleContext, PreProcessing );

		if (pEvent)
		{
			EVENT_OBJ EventObject;
			
			EvObject_Fill( &EventObject, FltObjects, pInstanceContext );

			if (FlagOn( FltObjects->FileObject->Flags, FO_CACHE_SUPPORTED ))
				SetFlag( EventObject.m_Flags, _EVOF_CACHE_AVAIL );

			Verdict = FilterEventImp( pEvent, &EventObject, 0 );

			*CompletionContext = pEvent;

			fltStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
		}
	}

	// force delete context
	//FltDeleteStreamHandleContext( FltObjects->Instance, FltObjects->FileObject, NULL );

	ReleaseFltContext( &pHandleContext );
	ReleaseFltContext( &pContext );
	ReleaseFltContext( &pInstanceContext );

	Timing_Out( _timing_file, IRP_MJ_CLEANUP, PreProcessing );
	
	return fltStatus;
}


FLT_POSTOP_CALLBACK_STATUS
MiniKavPostCleanup (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	FLT_POSTOP_CALLBACK_STATUS fltStatus = FLT_POSTOP_FINISHED_PROCESSING;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;
	Timing_Def;

	UNREFERENCED_PARAMETER( Data );
	UNREFERENCED_PARAMETER( Flags );

	PAGED_CODE();

	Timing_In;

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
	{
		EvContext_Done( CompletionContext );
		return fltStatus;
	}

	if (!CompletionContext)
	{
		//strange - check
		_dbg_break_;
	}
	else
	{
		tefVerdict Verdict = efVerdict_Default;
		PFILTER_EVENT_PARAM pEvent = CompletionContext;
		pEvent->ProcessingType = PostProcessing;

		Verdict = FilterEventImp( pEvent, NULL, 0 );
	}

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	if (pContext)
	{
		if (FlagOn( pContext->m_Flags, _STREAM_FLAGS_MODIFIED ))
		{
			IchDrv_ResetValid( FltObjects->Instance, Data, pInstanceContext, pContext );
			FidBox2_Update( &pInstanceContext->m_VolumeId, pContext->m_pFileId );
			ClearFlag( pContext->m_Flags, _STREAM_FLAGS_MODIFIED );
		}

		ReleaseFltContext( &pContext );
	}
	
	ReleaseFltContext( &pInstanceContext );

	EvContext_Done( CompletionContext );

	Timing_Out( _timing_file, IRP_MJ_CLEANUP, PostProcessing );

	return fltStatus;
}

// stat
unsigned __int64 g_T = 0;
unsigned __int64 g_TW = 0;
LONG g_C = 0;

ULONGLONG 
GetStatCounter(
	)
{
	return g_TW;
}

 VOID
 PreStatCounter()
 {
 	//if ( !IsInvisible ( PsGetCurrentThreadId(), PsGetCurrentProcessId() ) )
 	{
 
 		LONG curc = InterlockedExchangeAdd(&g_C, 2);
 		if (0 == curc)
 			KeQuerySystemTime((PLARGE_INTEGER)&g_T); // reset start period
 
 	}
 }
 
 VOID
 PostStatCounter()
 {
 	//if ( !IsInvisible ( PsGetCurrentThreadId(), PsGetCurrentProcessId() ) )
 	{
 		if (1 == InterlockedDecrement(&g_C)) // pre-exit
 		{
 			// lock not needed in this block
 			// only one thread at a time may be here
 			unsigned __int64 t;
 			KeQuerySystemTime( (PLARGE_INTEGER)&t );
			g_TW += (t - g_T); // increment wait time by delta
 			g_T = t; // set new start period
 		}
 		
 		if (1 == InterlockedDecrement(&g_C)) // pre-exit
		{
			// lock not needed in this block
			// only one thread at a time may be here
			unsigned __int64 t;
			KeQuerySystemTime( (PLARGE_INTEGER)&t );
			g_TW += (t - g_T); // increment wait time by delta
			g_T = t; // set new start period
		}
 		 		 		
 	}
 }

 FLT_PREOP_CALLBACK_STATUS
 MiniKavPreRead (
 	__inout PFLT_CALLBACK_DATA Data,
 	__in PCFLT_RELATED_OBJECTS FltObjects,
 	__out PVOID *CompletionContext
 )
 {
 	UNREFERENCED_PARAMETER( CompletionContext );
 	UNREFERENCED_PARAMETER( FltObjects );
 	UNREFERENCED_PARAMETER( Data );
 	
 	PreStatCounter();
 	
 	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
 }

 FLT_POSTOP_CALLBACK_STATUS
 MiniKavPostRead (
 	__inout PFLT_CALLBACK_DATA Data,
 	__in PCFLT_RELATED_OBJECTS FltObjects,
 	__in PVOID CompletionContext,
 	__in FLT_POST_OPERATION_FLAGS Flags
 )
 {
 	UNREFERENCED_PARAMETER( CompletionContext );
 	UNREFERENCED_PARAMETER( FltObjects );
 	UNREFERENCED_PARAMETER( Data );
 	UNREFERENCED_PARAMETER( Flags );
 
 	PostStatCounter();
 
 	return FLT_POSTOP_FINISHED_PROCESSING;
 }

FLT_PREOP_CALLBACK_STATUS
MiniKavPreWrite (
				  __inout PFLT_CALLBACK_DATA Data,
				  __in PCFLT_RELATED_OBJECTS FltObjects,
				  __out PVOID *CompletionContext
				  )
{
	
	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;

	UNREFERENCED_PARAMETER( CompletionContext );
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( Data );
	

	PreStatCounter();
	
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostWrite (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	FLT_POSTOP_CALLBACK_STATUS fltStatus = FLT_POSTOP_FINISHED_PROCESSING;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;

	PFILTER_EVENT_PARAM pEvent = NULL;

	Timing_Def;

	UNREFERENCED_PARAMETER( CompletionContext );
	
	PostStatCounter();

	if ( IsSkipOp( FltObjects, Flags ) )
		return FLT_POSTOP_FINISHED_PROCESSING;

	if ( !NT_SUCCESS(Data->IoStatus.Status) )
		return FLT_POSTOP_FINISHED_PROCESSING;

	if ( !Data->IoStatus.Information )
		return FLT_POSTOP_FINISHED_PROCESSING;

	Timing_In;

	if ( !NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )) )
		return FLT_POSTOP_FINISHED_PROCESSING;

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	if (pContext)
	{
		PFLT_FILE_NAME_INFORMATION pFileNameInfo = NULL;

		SetFlag( pContext->m_Flags, _STREAM_FLAGS_MODIFIED );
		InterlockedIncrement( &pContext->m_InternalId.LowPart );
	}

	pEvent = EvContext_Write( pInstanceContext, FltObjects, Data, pContext, NULL );
	if (pEvent)
	{
		tefVerdict Verdict = FilterEventImp( pEvent, NULL, 0 );

		EvContext_Done( pEvent );
	}

	ReleaseFltContext( &pContext );
	ReleaseFltContext( &pInstanceContext );

	Timing_Out( _timing_file, IRP_MJ_WRITE, PostProcessing );

	return fltStatus;
}

FLT_PREOP_CALLBACK_STATUS
MiniKavPreSetInformation (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
	)
{
	NTSTATUS fltStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;
	PMKAV_STREAM_HANDLE_CONTEXT pHandleContext = NULL;

	PFILTER_EVENT_PARAM pEvent = NULL;
	tefVerdict Verdict = efVerdict_Default;
	BOOLEAN bCallUser = TRUE;

	PAGED_CODE();

	if (IsSkipOp( FltObjects, 0 ))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	pHandleContext = GetStreamHandleContext( FltObjects->Instance, FltObjects->FileObject );

	switch (Data->Iopb->Parameters.SetFileInformation.FileInformationClass)
	{
	case FileLinkInformation:
		if (Data->Iopb->Parameters.SetFileInformation.InfoBuffer)
			pEvent = EvContext_CreateLink( pInstanceContext, FltObjects, Data, pContext, pHandleContext );
		break;

	case FileRenameInformation:
		if ( pContext && pContext->m_pFileNameInfo )
		{
			Verdict = IsAllowAccessFile (
					Data, 
					PsGetCurrentProcessId(), 
					pContext->m_pFileNameInfo,
					FILE_OP_DELETE
					);
		}
		
		if (efIsAllow( Verdict ))
			pEvent = EvContext_Rename( pInstanceContext, FltObjects, Data, pContext, pHandleContext );
		break;
	
	case FileDispositionInformation:
		if ( pContext && pContext->m_pFileNameInfo )
		{
			PFILE_DISPOSITION_INFORMATION pfdi;
			pfdi = (PFILE_DISPOSITION_INFORMATION)(Data->Iopb->Parameters.SetFileInformation.InfoBuffer);
			if (pfdi)
			{
				if ( !pfdi->DeleteFile )
					bCallUser = FALSE;
				else
				{
					Verdict = IsAllowAccessFile (
						Data, 
						PsGetCurrentProcessId(), 
						pContext->m_pFileNameInfo,
						FILE_OP_DELETE
						);
				}

				if ( efIsAllow( Verdict ) )
					pEvent = EvContext_Common( pInstanceContext, FltObjects, Data, pContext, pHandleContext, PreProcessing );
			}
		}
		break;
	}

	if ( efIsDeny( Verdict ) )
	{
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		Data->IoStatus.Information = 0;

		fltStatus = FLT_PREOP_COMPLETE;
	}
	else if (pEvent)
	{
		if (bCallUser)
			Verdict = FilterEventImp( pEvent, NULL, 0 );

		if (efIsAllow( Verdict ))
		{
			// allow, nothing to do
			*CompletionContext = pEvent;
			fltStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;
		}
		else
		{
			_dbg_break_;
			
			Data->IoStatus.Status = STATUS_ACCESS_DENIED;
			Data->IoStatus.Information = 0;

			fltStatus = FLT_PREOP_COMPLETE;

			EvContext_Done( pEvent );
		}
	}

	ReleaseFltContext( &pHandleContext );
	ReleaseFltContext( &pContext );
	ReleaseFltContext( &pInstanceContext );

	return fltStatus;
}

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostSetInformation (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	FLT_POSTOP_CALLBACK_STATUS fltStatus = FLT_POSTOP_FINISHED_PROCESSING;
	NTSTATUS status;
	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;
	PMKAV_STREAM_HANDLE_CONTEXT pHandleContext = NULL;

	BOOLEAN bDelete = FALSE;
	ULONG NumberOfLinks = 0;

	PAGED_CODE();

	if (FLTFL_POST_OPERATION_DRAINING & Flags)
	{
		EvContext_Done( CompletionContext );
		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	if (!NT_SUCCESS(Data->IoStatus.Status))
	{
		EvContext_Done( CompletionContext );
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	
	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
	{
		EvContext_Done( CompletionContext );

		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	if (CompletionContext)
	{
		tefVerdict Verdict = efVerdict_Default;
		PFILTER_EVENT_PARAM pEvent = CompletionContext;
		PSINGLE_PARAM pBinaryData = NULL;

		pEvent->ProcessingType = PostProcessing;

		if (FileDispositionInformation == Data->Iopb->Parameters.SetFileInformation.FileInformationClass)
		{
			pBinaryData = GetSingleParamPtr( pEvent, _PARAM_OBJECT_BINARYDATA );
			if (pBinaryData && sizeof(ULONG) == pBinaryData->ParamSize)
				bDelete = (BOOLEAN) *(ULONG*) pBinaryData->ParamValue;
		}
		if (FileLinkInformation == Data->Iopb->Parameters.SetFileInformation.FileInformationClass)
		{
			ULONG len;
			FILE_STANDARD_INFORMATION file_st;

			status = MySynchronousQueryInformationFile (
				FltObjects->Instance,
				FltObjects->FileObject,
				&file_st, sizeof(file_st),
				FileStandardInformation,
				&len
				);
			
			if (NT_SUCCESS( status ))
				NumberOfLinks = file_st.NumberOfLinks;
		}

		Verdict = FilterEventImp( pEvent, NULL, 0 );

		EvContext_Done( CompletionContext );
	}

	pHandleContext = GetStreamHandleContext( FltObjects->Instance, FltObjects->FileObject );
	if (pHandleContext)
	{
		switch(Data->Iopb->Parameters.SetFileInformation.FileInformationClass)
		{
		case FileDispositionInformation:
			if (bDelete)
				SetFlag( pHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_DELETE_PENDING );
			else
				ClearFlag( pHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_DELETE_PENDING );
			break;

		case FileRenameInformation:
			ClearFlag( pHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_DELETE_PENDING );
			break;
		}

		ReleaseFltContext( &pHandleContext );
	}

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	if (pContext)
	{
		switch(Data->Iopb->Parameters.SetFileInformation.FileInformationClass)
		{
		case FileLinkInformation:
			pContext->m_NumberOfLinks = NumberOfLinks;
			break;

		case FileRenameInformation: 
			{
				PFLT_FILE_NAME_INFORMATION pNameInfoDest = NULL;
				PFILE_RENAME_INFORMATION pRenameInfo = (PFILE_RENAME_INFORMATION) Data->Iopb->Parameters.SetFileInformation.InfoBuffer;

				if (Data->Iopb->Parameters.SetFileInformation.Length < sizeof(FILE_RENAME_INFORMATION))
				{
					_dbg_break_;
				}
				else
				{
					if (!pRenameInfo->FileNameLength)
					{
						_dbg_break_;
					}
					else
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
						{
							PFLT_FILE_NAME_INFORMATION pFileNameInfo = LockFileNameInfo( pContext );
							if (pFileNameInfo)
							{
								Serv_FixStoredFileName( pNameInfoDest, pFileNameInfo  );

								ReleaseFileNameInfo( pFileNameInfo );
							}

							ReplaceFileNameInfo( pContext, pNameInfoDest );
						}
					}
				}
			}
			break;

		case FileDispositionInformation:
			{
				PFLT_FILE_NAME_INFORMATION pFileNameInfo = NULL;
				pFileNameInfo = LockFileNameInfo( pContext );
				if (pFileNameInfo)
				{
					DoTraceEx( TRACE_LEVEL_INFORMATION, DC_DRIVER, "delete %d pending '%wZ'",
						bDelete,
						&pFileNameInfo->Name
						);

					ReleaseFileNameInfo( pFileNameInfo );
				}
			}
			break;
		}
	}

	ReleaseFltContext( &pContext );
	ReleaseFltContext( &pInstanceContext );
	
	return fltStatus;
}

#define NETSWIFT_BUFFER_SIZE		0x60

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostQueryInformation (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	PAGED_CODE();

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	if (FLTFL_POST_OPERATION_DRAINING & Flags)
		return FLT_POSTOP_FINISHED_PROCESSING;

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_INETSWIFT_DISABLED ))
		return FLT_POSTOP_FINISHED_PROCESSING;

	if (NT_SUCCESS( Data->IoStatus.Status )
		&& Fid_GetItemSize1()
		&& Data->Iopb->Parameters.QueryFileInformation.FileInformationClass == FileEaInformation
		&& Data->Iopb->Parameters.QueryFileInformation.Length >= sizeof(FILE_EA_INFORMATION)
		)
	{
		ULONG RequiredSize = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])
			+ _INETSWIFT_PREFIX_FULL_BLEN + 1 + sizeof(INETSWIFT_DATA_EX)
			+ Fid_GetItemSize1();

		PULONG pEASize = &((PFILE_EA_INFORMATION) Data->Iopb->Parameters.QueryFileInformation.InfoBuffer)->EaSize;

		if (RequiredSize > *pEASize)		//
			*pEASize = RequiredSize;		// for netswift

		FltSetCallbackDataDirty(Data);
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
MiniKavPreQueryEA (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
    )
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PFILE_GET_EA_INFORMATION pEAQueryList;
	BOOLEAN bProcessed = FALSE;

	PAGED_CODE();

	UNREFERENCED_PARAMETER(CompletionContext);

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_INETSWIFT_DISABLED ))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	pEAQueryList = (PFILE_GET_EA_INFORMATION) Data->Iopb->Parameters.QueryEa.EaList;
	if (pEAQueryList)
	{
		if (!pEAQueryList->NextEntryOffset
			&& pEAQueryList->EaNameLength >= _INETSWIFT_PREFIX_FULL_BLEN
			&& !memcmp( pEAQueryList->EaName, _INETSWIFT_PREFIX_FULL, _INETSWIFT_PREFIX_FULL_BLEN - 1 )
			)
		{
			UCHAR Key = pEAQueryList->EaName[_INETSWIFT_PREFIX_FULL_BLEN - 1];
			PCHAR pEABuffer = (PCHAR) Data->Iopb->Parameters.QueryEa.EaBuffer;

			if (pEABuffer)
			{
				ULONG FidboxDataBufferSize = Data->Iopb->Parameters.QueryEa.Length
					- FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])
					- _INETSWIFT_PREFIX_FULL_BLEN - 1
					- sizeof(INETSWIFT_DATA_EX);

				if ((LONG) FidboxDataBufferSize >= (LONG) Fid_GetItemSize1())
				{
					PINETSWIFT_DATA_EX piSwiftData = (PINETSWIFT_DATA_EX)
						(pEABuffer+FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])+_INETSWIFT_PREFIX_FULL_BLEN + 1);

					PFIDBOX_REQUEST_DATA_EX pFidboxData = (PFIDBOX_REQUEST_DATA_EX) ((PCHAR)piSwiftData + FIELD_OFFSET( INETSWIFT_DATA_EX, m_Data ));

					ULONG RetSize;

					RtlZeroMemory( piSwiftData, sizeof(INETSWIFT_DATA_EX) );
					status = FidBox_GetDataByFO( FltObjects->FileObject, FALSE, pFidboxData, Fid_GetItemSize1(), &RetSize, TRUE );
					if (NT_SUCCESS( status ))
					{
						if (RetSize > 0)
						{
							PFILE_FULL_EA_INFORMATION pRetEAInfo = (PFILE_FULL_EA_INFORMATION) pEABuffer;

							pRetEAInfo->NextEntryOffset = 0;
							pRetEAInfo->EaNameLength = _INETSWIFT_PREFIX_FULL_BLEN;
							pRetEAInfo->EaValueLength = (USHORT) ((ULONG)RetSize + FIELD_OFFSET( INETSWIFT_DATA_EX, m_Data ));
							memcpy( pRetEAInfo->EaName, _INETSWIFT_PREFIX_FULL, _INETSWIFT_PREFIX_FULL_BLEN );
							pRetEAInfo->EaName[_INETSWIFT_PREFIX_FULL_BLEN] = 0;
							piSwiftData->m_Version = _INETSWIFT_VERSION_3;
							piSwiftData->m_Tag = _INETSWIFT_TAG;
							_xor_drv_func_ex( piSwiftData, pRetEAInfo->EaValueLength, Key, _xor_dir_go );

							Data->IoStatus.Information = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])
								+ pRetEAInfo->EaNameLength + 1 + pRetEAInfo->EaValueLength;

							Data->IoStatus.Status = STATUS_SUCCESS;
							
							FltSetCallbackDataDirty(Data);

							return FLT_PREOP_COMPLETE;
						}
					}

				}
			}

			Data->IoStatus.Information = 0;
			Data->IoStatus.Status = STATUS_INVALID_PARAMETER;

			return FLT_PREOP_COMPLETE;
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS
MiniKavPreSyncSection (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__out PVOID *CompletionContext
    )
{
	NTSTATUS fltStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;
	PMKAV_STREAM_HANDLE_CONTEXT pHandleContext = NULL;

	tefVerdict Verdict = efVerdict_Default;

	PAGED_CODE();

	UNREFERENCED_PARAMETER( CompletionContext );

	if (IsSkipOp( FltObjects, 0 ))
		return FLT_PREOP_SUCCESS_NO_CALLBACK;

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
		return FLT_PREOP_SUCCESS_NO_CALLBACK ;

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	if (pContext)
	{
		if (PAGE_EXECUTE & Data->Iopb->Parameters.AcquireForSectionSynchronization.PageProtection)
		{
			if (SyncTypeCreateSection == Data->Iopb->Parameters.AcquireForSectionSynchronization.SyncType)
			{
				PFLT_FILE_NAME_INFORMATION pFileNameInfo = NULL;
				// may be denied by returned STATUS_ACCESS_DENIED

				pHandleContext = GetStreamHandleContext( FltObjects->Instance, FltObjects->FileObject );
				if (pHandleContext)
				{
					PFILTER_EVENT_PARAM pEvent = NULL;

					SetFlag( pHandleContext->m_Flags, _STREAM_HANDLE_FLAGS_FOR_EXECUTE );

					fltStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;

					//if (UserMode == Data->RequestorMode) dont chekc из-за eicar.com
					{
						pEvent = EvContext_Common( pInstanceContext, FltObjects, Data, pContext, pHandleContext, PreProcessing );
						if (pEvent)
						{
							if (FlagOn( FltObjects->FileObject->Flags, FO_CACHE_SUPPORTED ))
							{
								EVENT_OBJ EventObject;
								EvObject_Fill( &EventObject, FltObjects, pInstanceContext );
								SetFlag( EventObject.m_Flags, _EVOF_CACHE_AVAIL );

								Verdict = FilterEventImp( pEvent, &EventObject, 0 );
							}
							else
								Verdict = FilterEventImp( pEvent, NULL, 0 );

							EvContext_Done( pEvent );
						}
					}
				}
			}
			else
			{
				// cannot be failed! (by help)
			}
		}

		ReleaseFltContext( &pHandleContext );
		ReleaseFltContext( &pContext );
	}

	ReleaseFltContext( &pInstanceContext );

	if (efIsDeny( Verdict ))
	{
		_dbg_break_;
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		Data->IoStatus.Information = 0;

		fltStatus = FLT_PREOP_COMPLETE;
	}

	return fltStatus;
}

FLT_POSTOP_CALLBACK_STATUS
MiniKavPostSyncSection (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
	)
{
	FLT_POSTOP_CALLBACK_STATUS fltStatus = FLT_POSTOP_FINISHED_PROCESSING;

	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;

	PFILTER_EVENT_PARAM pEvent = NULL;

	UNREFERENCED_PARAMETER( CompletionContext );

	if(IsSkipOp( FltObjects, Flags ))
		return FLT_POSTOP_FINISHED_PROCESSING;

	if (!NT_SUCCESS(Data->IoStatus.Status))
		return FLT_POSTOP_FINISHED_PROCESSING;

	if (!NT_SUCCESS(FltGetInstanceContext( FltObjects->Instance, &pInstanceContext )))
		return FLT_POSTOP_FINISHED_PROCESSING;

	pContext = GetStreamContext( FltObjects->Instance, FltObjects->FileObject );
	if (pContext)
	{
		PFLT_FILE_NAME_INFORMATION pFileNameInfo = LockFileNameInfo( pContext );

		if (pFileNameInfo)
		{
			FILE_STANDARD_INFORMATION stdinfo;
			NTSTATUS status = MySynchronousQueryInformationFile (
				FltObjects->Instance,
				FltObjects->FileObject,
				&stdinfo,
				sizeof(stdinfo),
				FileStandardInformation,
				NULL
				);

			if (NT_SUCCESS(status))
			{
				if (stdinfo.EndOfFile.LowPart || stdinfo.EndOfFile.HighPart )
					Sys_RegisterExecutedModule( &pFileNameInfo->Volume, &pFileNameInfo->Name );
			}
			ReleaseFileNameInfo( pFileNameInfo );
		}
		
		ReleaseFltContext( &pContext );
	}
	
	ReleaseFltContext( &pInstanceContext );

	return fltStatus;
}

