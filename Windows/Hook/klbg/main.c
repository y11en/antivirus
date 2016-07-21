#include <fltKernel.h>
#include <stdlib.h>
#include "main.h"
#include "avzbc.h"
#include "watcher.h"
#include "ctlcodes.h"
#include "avz.h"
#include "threadcontext.h"

extern PULONG InitSafeBootMode;
extern PUSHORT NtBuildNumber;

typedef struct _ZBD_INSTANCE_CONTEXT
{
	KSPIN_LOCK	m_slRanOnceLock;
	BOOLEAN		m_bRanOnce;
} ZBD_INSTANCE_CONTEXT, *PZBD_INSTANCE_CONTEXT;

#define INSTANCE_CONTEXT_SIZE		sizeof(ZBD_INSTANCE_CONTEXT)

typedef struct _ZBD_VOLUME_CONTEXT
{
	PFLT_INSTANCE	m_pMyInstance;
} ZBD_VOLUME_CONTEXT, *PZBD_VOLUME_CONTEXT;

typedef struct _ZBD_THREAD_CONTEXT
{
	BOOLEAN		m_bRecursive;
} ZBD_THREAD_CONTEXT, *PZBD_THREAD_CONTEXT;


#define VOLUME_CONTEXT_SIZE		sizeof(ZBD_VOLUME_CONTEXT)

static PDEVICE_OBJECT g_pCtlDevObj = NULL;

static PSERVICE_ENUM_ENTRY g_pCurrEntry = NULL;

FLT_PREOP_CALLBACK_STATUS
ZbdPreCreate(	__in OUT PFLT_CALLBACK_DATA Data,
			 __in PCFLT_RELATED_OBJECTS FltObjects,
			 __out PVOID *CompletionContext
			 );

static FLT_CONTEXT_REGISTRATION fltContextRegs[] = {	{FLT_INSTANCE_CONTEXT, 0, NULL, INSTANCE_CONTEXT_SIZE, 'CIbz', NULL, NULL, NULL},
														{FLT_VOLUME_CONTEXT, 0, NULL, VOLUME_CONTEXT_SIZE, 'CVbz', NULL, NULL, NULL},
														{FLT_CONTEXT_END}
													};

static FLT_OPERATION_REGISTRATION fltOperationRegs[] = {	{IRP_MJ_CREATE, 0, ZbdPreCreate, NULL, NULL},
															{IRP_MJ_OPERATION_END}
														};

PFLT_FILTER g_pFilterObj = NULL;

static UNICODE_STRING g_usRegistryPath;

static volatile ULONG g_CreateEnters = 0;
static KSPIN_LOCK g_slCreateLock; // create count lock

static ERESOURCE g_erJobLock;

// get this filter instance(referenced) for volume on which this file resides
PFLT_INSTANCE GetInstanceForFile(
									  __in PUNICODE_STRING p_usFileName
									  )
{
	NTSTATUS status;
	OBJECT_ATTRIBUTES fileOA;
	HANDLE hFile;
	IO_STATUS_BLOCK ioSB;
	PFLT_INSTANCE pInstance = NULL;
	PWCHAR wcPathName;
	PWCHAR RevSlashPos;
	UNICODE_STRING usPathName;

// extract directory path
	wcPathName = (PWCHAR) ExAllocatePoolWithTag(PagedPool, p_usFileName->Length + sizeof(WCHAR), 'NPbz' );
	if ( !wcPathName )
		return NULL;

	memcpy( wcPathName, p_usFileName->Buffer, p_usFileName->Length );
	wcPathName[p_usFileName->Length / sizeof(WCHAR)] = 0;

	RevSlashPos = wcsrchr( wcPathName, L'\\' );
	if ( RevSlashPos )
		RevSlashPos[1] = 0;

	RtlInitUnicodeString( &usPathName, wcPathName );

// open directory containing file
	InitializeObjectAttributes(
		&fileOA,
		&usPathName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
		);

// Microsoft bug: I cannot use FltCreateFile here with NULL instance pointer
// because it always failes on Windows 2000 while working well on xp+
	status = ZwCreateFile(
		&hFile,
		SYNCHRONIZE,
		&fileOA,
		&ioSB,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN,
		0,
		NULL,
		0
		);
	if ( NT_SUCCESS(status) )
	{
		PFILE_OBJECT pFileObject;

		status = ObReferenceObjectByHandle(
			hFile,
			0,
			*IoFileObjectType,
			KernelMode,
			(PVOID *) &pFileObject,
			NULL
			);
		if ( NT_SUCCESS(status) )
		{
			PFLT_VOLUME pVolume;

			status = FltGetVolumeFromFileObject(
				g_pFilterObj,
				pFileObject,
				&pVolume
				);
			if ( !NT_SUCCESS(status) )
			{
				KdPrint( ("zbd.sys: GetInstanceForFile(): FltGetVolumeFromFileObject( 0x%08x ).\n", pFileObject) );
				_dbg_break_;
			}
			else
			{
				PZBD_VOLUME_CONTEXT pVolumeContext;

				status = FltGetVolumeContext(
					g_pFilterObj,
					pVolume,
					(PFLT_CONTEXT *) &pVolumeContext
					);
				if ( NT_SUCCESS(status) )
				{
					pInstance = pVolumeContext->m_pMyInstance;
					FltObjectReference(pInstance);
					FltReleaseContext( pVolumeContext );
				}


				FltObjectDereference( pVolume );
			}

			ObDereferenceObject( pFileObject );
		}

		ZwClose( hFile );
	}

	ExFreePool( wcPathName );

	return pInstance;
}

NTSTATUS SWGetCurrent( __in PIRP pIrp )
{
	NTSTATUS status;
	PIO_STACK_LOCATION p_ioLoc;
	ULONG RequiredBufLen;
	ULONG Information = 0;

	AcquireResourceShared( &g_erServiceListLock );

	do
	{
		PSERVICE_ENUM_ENTRY_OUT pOutEntry;

		if ( !g_pCurrEntry || (&g_ServiceListHead == (PLIST_ENTRY) g_pCurrEntry) )
		{
			status = STATUS_NO_MORE_ENTRIES;
			break;
		}

		RequiredBufLen = FIELD_OFFSET( SERVICE_ENUM_ENTRY_OUT, m_ServiceName ) + g_pCurrEntry->m_cbServiceName;

		p_ioLoc = IoGetCurrentIrpStackLocation( pIrp );

		if ( p_ioLoc->Parameters.DeviceIoControl.OutputBufferLength < RequiredBufLen )
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		pOutEntry = (PSERVICE_ENUM_ENTRY_OUT) pIrp->AssociatedIrp.SystemBuffer;

		pOutEntry->m_Type = g_pCurrEntry->m_Type;
		pOutEntry->m_Start = g_pCurrEntry->m_Start;
		pOutEntry->m_Md5Computed = g_pCurrEntry->m_Md5Computed;
		memcpy( pOutEntry->m_Md5Hash, g_pCurrEntry->m_Md5Hash, sizeof(pOutEntry->m_Md5Hash) );
		pOutEntry->m_cbServiceName = g_pCurrEntry->m_cbServiceName;
		pOutEntry->m_ImagePathOffset = g_pCurrEntry->m_ImagePathOffset;
		memcpy( pOutEntry->m_ServiceName, g_pCurrEntry->m_ServiceName, g_pCurrEntry->m_cbServiceName );

		Information = RequiredBufLen;
		status = STATUS_SUCCESS;

		g_pCurrEntry = (PSERVICE_ENUM_ENTRY) g_pCurrEntry->m_List.Flink;
	} while (FALSE);

	ReleaseResource( &g_erServiceListLock );

	pIrp->IoStatus.Information = Information;
	pIrp->IoStatus.Status = status;

	return status;
}

NTSTATUS SWGetFirst( __in PIRP pIrp )
{
	NTSTATUS status;

	g_pCurrEntry = (PSERVICE_ENUM_ENTRY) g_ServiceListHead.Flink;
	status = SWGetCurrent( pIrp );

	return status;
}

NTSTATUS SWGetNext( __in PIRP pIrp )
{
	return SWGetCurrent( pIrp );
}

NTSTATUS DispatchSuccess(
						 __in PDEVICE_OBJECT DeviceObject,
						 __in PIRP Irp
						 )
{
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return STATUS_SUCCESS;
}

NTSTATUS DispatchControl(
						 __in PDEVICE_OBJECT DeviceObject,
						 __in PIRP Irp
						 )
{
	NTSTATUS status;
	PIO_STACK_LOCATION p_ioLoc;

	p_ioLoc = IoGetCurrentIrpStackLocation( Irp );
	switch ( p_ioLoc->Parameters.DeviceIoControl.IoControlCode )
	{
	case IOCTL_SW_GETFIRST:
		status = SWGetFirst( Irp );
		break;
	case IOCTL_SW_GETNEXT:
		status = SWGetNext( Irp );
		break;
	default:
#ifndef _WIN64
		status = AvzDispatchControl( Irp );
		if ( !NT_SUCCESS(status) )
			Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = status;
#else
		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
#endif
	}

	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return status;
}

BOOLEAN InitControl( __in PDRIVER_OBJECT pDriverObject )
{
	NTSTATUS status;
	UNICODE_STRING usCtlDeviceName;
	PDEVICE_OBJECT pCtlDevObj = NULL;
	UNICODE_STRING usSymlinkName;

	RtlInitUnicodeString( &usCtlDeviceName, CTL_DEVICE_NAME );
	status = IoCreateDevice(
		pDriverObject,
		0,
		&usCtlDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&pCtlDevObj);
	if ( !NT_SUCCESS(status) )
		return FALSE;

	RtlInitUnicodeString( &usSymlinkName, SYMLINK_NAME );
	status = IoCreateSymbolicLink(
		&usSymlinkName,
		&usCtlDeviceName);
	if ( !NT_SUCCESS( status) )
	{
		IoDeleteDevice(pCtlDevObj);
		return FALSE;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = 
		pDriverObject->MajorFunction[IRP_MJ_CLEANUP] =
		pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchSuccess;

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;

	g_pCtlDevObj = pCtlDevObj;

	return TRUE;
}

VOID UnInitControl()
{
	if ( g_pCtlDevObj )
	{
		UNICODE_STRING usSymlinkName;

		RtlInitUnicodeString( &usSymlinkName, SYMLINK_NAME );
		IoDeleteSymbolicLink( &usSymlinkName );
		IoDeleteDevice( g_pCtlDevObj );
	}
}

FLT_PREOP_CALLBACK_STATUS
ZbdPreCreate(	__in OUT PFLT_CALLBACK_DATA Data,
				__in PCFLT_RELATED_OBJECTS FltObjects,
				__out PVOID *CompletionContext
			)
{
	NTSTATUS status;
	PZBD_THREAD_CONTEXT pThreadContext = NULL;
	PZBD_INSTANCE_CONTEXT pInstanceContext = NULL;
	KIRQL OldIrql;

// recursive callback
	pThreadContext = (PZBD_THREAD_CONTEXT) GetThreadContext( PsGetCurrentThreadId() );
	if ( pThreadContext )
	{
		if ( pThreadContext->m_bRecursive )
		{
			ReleaseThreadContext( pThreadContext );
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		pThreadContext->m_bRecursive = TRUE;
	}
	else
	{
		pThreadContext = (PZBD_THREAD_CONTEXT) AllocateThreadContext(
			PagedPool,
			sizeof(ZBD_THREAD_CONTEXT)
			);
		if ( pThreadContext )
		{
			BOOLEAN bResult;

			pThreadContext->m_bRecursive = TRUE;

			bResult = SetThreadContext( 
				pThreadContext,
				PsGetCurrentThreadId()
				);
			if ( !bResult )
			{
				_dbg_break_;
				KdPrint( ("zbd.sys: failed to set thread context.\n") );
				ReleaseThreadContext( pThreadContext );
				pThreadContext = NULL;
			}
		}
	}

// bypass "\Windows\LastGood" file create which causes bugcheck
	KeAcquireSpinLock( &g_slCreateLock, &OldIrql);
	if ( g_CreateEnters < 1 )
	{
		InterlockedIncrement( &g_CreateEnters );
		KeReleaseSpinLock( &g_slCreateLock, OldIrql );

		if ( pThreadContext )
			pThreadContext->m_bRecursive = FALSE;

		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	KeReleaseSpinLock( &g_slCreateLock, OldIrql );

	status = FltGetInstanceContext(
		FltObjects->Instance,
		(PFLT_CONTEXT *) &pInstanceContext
		);
	if ( !NT_SUCCESS(status) )
	{
		if ( pThreadContext )
			pThreadContext->m_bRecursive = FALSE;

		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	KeAcquireSpinLock( &pInstanceContext->m_slRanOnceLock, &OldIrql );
	if ( !pInstanceContext->m_bRanOnce )
	{
		pInstanceContext->m_bRanOnce = TRUE;
		KeReleaseSpinLock( &pInstanceContext->m_slRanOnceLock, OldIrql );

		AcquireResourceExclusive( &g_erJobLock );

// run these routines only once per volume
		ExecuteQuarantine( &g_usRegistryPath, FALSE);
		ExecuteCopyFiles(&g_usRegistryPath, FALSE);
		ExecuteDeleteFiles(&g_usRegistryPath, FALSE);
		ExecuteDeleteDrivers(&g_usRegistryPath, FALSE);

		ReleaseResource( &g_erJobLock );

		ComputeMd5OnServiceImages();
	}
	else
		KeReleaseSpinLock( &pInstanceContext->m_slRanOnceLock, OldIrql );

	FltReleaseContext( (PFLT_CONTEXT) pInstanceContext );

	if ( pThreadContext )
		pThreadContext->m_bRecursive = FALSE;

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

VOID CreateThreadNotifyRoutine(
							   IN HANDLE  ProcessId,
							   IN HANDLE  ThreadId,
							   IN BOOLEAN  Create
								  )
{
	if ( !Create )
	{
		PTHREAD_CONTEXT pContext;

		pContext = GetThreadContext( ThreadId );
		if ( pContext )
		{
			ReleaseThreadContext( pContext );
			ReleaseThreadContext( pContext );
		}
	}
}

typedef
NTSTATUS
(NTAPI *t_fPsRemoveCreateThreadNotifyRoutine)(
								   IN PCREATE_THREAD_NOTIFY_ROUTINE  NotifyRoutine 
								   );

NTSTATUS ZbdFilterUnload( __in FLT_FILTER_UNLOAD_FLAGS Flags )
{
	UNICODE_STRING usRemoveStr;
	t_fPsRemoveCreateThreadNotifyRoutine fPsRemoveCreateThreadNotifyRoutine;

// dynamic linking - this function is not present on win2k
	RtlInitUnicodeString(
		&usRemoveStr,
		L"PsRemoveCreateThreadNotifyRoutine"
		);
	fPsRemoveCreateThreadNotifyRoutine = (t_fPsRemoveCreateThreadNotifyRoutine) MmGetSystemRoutineAddress( &usRemoveStr );
	if ( fPsRemoveCreateThreadNotifyRoutine )
		fPsRemoveCreateThreadNotifyRoutine( CreateThreadNotifyRoutine );
	else
		return STATUS_FLT_DO_NOT_DETACH;	// do not unload on win2k

	AVZBCUnInit();
	UnInitControl();
	ServiceEnumCleanup();
	FltUnregisterFilter( g_pFilterObj );
	ExFreePool( g_usRegistryPath.Buffer );
	UnInitThreadContexts();
	ExDeleteResourceLite( &g_erJobLock );

	return STATUS_SUCCESS;
}

NTSTATUS ZbdInstanceSetup(		 __in PCFLT_RELATED_OBJECTS  FltObjects,
								 __in FLT_INSTANCE_SETUP_FLAGS  Flags,
								 __in DEVICE_TYPE  VolumeDeviceType,
								 __in FLT_FILESYSTEM_TYPE  VolumeFilesystemType
								 )
{
	NTSTATUS status;
	PZBD_INSTANCE_CONTEXT pInstanceContext = NULL;
	PZBD_VOLUME_CONTEXT pVolumeContext = NULL;

	status = FltAllocateContext( 
				g_pFilterObj,
				FLT_INSTANCE_CONTEXT,
				INSTANCE_CONTEXT_SIZE,
				NonPagedPool,
				(PFLT_CONTEXT *) &pInstanceContext
				);
	if ( !NT_SUCCESS(status) )
		return STATUS_FLT_DO_NOT_ATTACH;

	KeInitializeSpinLock( &pInstanceContext->m_slRanOnceLock );
	pInstanceContext->m_bRanOnce = FALSE;

	status = FltSetInstanceContext(
		FltObjects->Instance,
		FLT_SET_CONTEXT_KEEP_IF_EXISTS,
		(PFLT_CONTEXT) pInstanceContext,
		NULL
		);
	if ( NT_SUCCESS(status) )
		FltReleaseContext( (PFLT_CONTEXT) pInstanceContext );
	else
		_dbg_break_;

	status = FltAllocateContext(
		g_pFilterObj,
		FLT_VOLUME_CONTEXT,
		VOLUME_CONTEXT_SIZE,
		NonPagedPool,
		(PFLT_CONTEXT *) &pVolumeContext
		);
	if ( !NT_SUCCESS(status) )
	{
		FltReleaseContext( (PFLT_CONTEXT) pInstanceContext);
		return STATUS_FLT_DO_NOT_ATTACH;
	}

	pVolumeContext->m_pMyInstance = FltObjects->Instance;

	status = FltSetVolumeContext(
		FltObjects->Volume,
		FLT_SET_CONTEXT_KEEP_IF_EXISTS,
		(PFLT_CONTEXT) pVolumeContext,
		NULL
		);
	if ( NT_SUCCESS(status) )
		FltReleaseContext( pVolumeContext );
	else
		_dbg_break_;

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry( __in PDRIVER_OBJECT pDriverObject, __in PUNICODE_STRING p_usRegistryPath )
{
	NTSTATUS status;
	BOOLEAN bResult;
	FLT_REGISTRATION fltRegistration;

	if ( *InitSafeBootMode > 0 )
		return STATUS_UNSUCCESSFUL;

	ServiceEnumInit();
	status = EnumServices();
	if ( !NT_SUCCESS( status ) )
	{
		ServiceEnumCleanup();
		return status;
	}

	RtlZeroMemory( &fltRegistration, sizeof(fltRegistration) );
	fltRegistration.Size = sizeof( fltRegistration );
	fltRegistration.Version = FLT_REGISTRATION_VERSION;
	fltRegistration.Flags = 0;
	fltRegistration.ContextRegistration = fltContextRegs;
	fltRegistration.OperationRegistration = fltOperationRegs;
	fltRegistration.FilterUnloadCallback = *NtBuildNumber > 2195 ? ZbdFilterUnload : NULL;
	fltRegistration.InstanceSetupCallback = ZbdInstanceSetup;

	status = FltRegisterFilter( pDriverObject, &fltRegistration, &g_pFilterObj );
	if ( !NT_SUCCESS(status) )
	{
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: failed to register filter.\n") );
		return status;
	}

	g_usRegistryPath.Buffer = (PWCHAR) ExAllocatePoolWithTag( PagedPool, 0x1000, 'PRbz' );
	if ( !g_usRegistryPath.Buffer )
	{
		FltUnregisterFilter( g_pFilterObj );
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: insufficient resources.\n") );
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	g_usRegistryPath.MaximumLength = 0x1000;

	RtlCopyUnicodeString( &g_usRegistryPath, p_usRegistryPath );

	bResult = InitControl( pDriverObject );
	if ( !bResult )
	{
		ExFreePool( g_usRegistryPath.Buffer );
		FltUnregisterFilter( g_pFilterObj );
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: failed to create control device.\n") );
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	bResult = InitThreadContexts();
	if ( !bResult )
	{
		UnInitControl();
		ExFreePool( g_usRegistryPath.Buffer );
		FltUnregisterFilter( g_pFilterObj );
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: failed to init thread context cache.\n") );
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	KeInitializeSpinLock( &g_slCreateLock ); // create count lock

	status = ExInitializeResourceLite( &g_erJobLock );
	if ( !NT_SUCCESS( status ) )
	{
		UnInitThreadContexts();
		UnInitControl();
		ExFreePool( g_usRegistryPath.Buffer );
		FltUnregisterFilter( g_pFilterObj );
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: failed to initialize job lock resource.\n") );
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	AVZBCInit( p_usRegistryPath );

	ExecuteREGOper(p_usRegistryPath, TRUE);		// execute registry cleaning synchronously in DriverEntry

	status = FltStartFiltering( g_pFilterObj );
	if ( !NT_SUCCESS(status) )
	{
		ExDeleteResourceLite( &g_erJobLock );
		UnInitThreadContexts();
		UnInitControl();
		ExFreePool( g_usRegistryPath.Buffer );
		FltUnregisterFilter( g_pFilterObj );
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: failed to start filtering.\n") );
		return status;
	}

	status = PsSetCreateThreadNotifyRoutine( CreateThreadNotifyRoutine );
	if ( !NT_SUCCESS( status ) )
	{
		ExDeleteResourceLite( &g_erJobLock );
		UnInitThreadContexts();
		UnInitControl();
		ExFreePool( g_usRegistryPath.Buffer );
		FltUnregisterFilter( g_pFilterObj );
		ServiceEnumCleanup();

		KdPrint( ("zbd.sys: failed to PsSetCreateThreadNotifyRoutine.\n") );
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	return STATUS_SUCCESS;
}
