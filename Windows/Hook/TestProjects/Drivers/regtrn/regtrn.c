#pragma warning(error:4100)     //  Enable-Unreferenced formal parameter
#pragma warning(error:4101)     //  Enable-Unreferenced local variable
#pragma warning(error:4061)     //  Eenable-missing enumeration in switch statement
#pragma warning(error:4505)     //  Enable-identify dead functions

#include <fltKernel.h>

NTSTATUS
DriverEntry (
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
	);

NTSTATUS
Unload (
	FLT_FILTER_UNLOAD_FLAGS Flags
	);

NTSTATUS
MiniKavInstanceSetup (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in FLT_INSTANCE_SETUP_FLAGS Flags,
	__in DEVICE_TYPE VolumeDeviceType,
	__in FLT_FILESYSTEM_TYPE VolumeFilesystemType
	);

NTSTATUS
MiniKavTransactionNotify (
	__in PCFLT_RELATED_OBJECTS  FltObjects,
	__in PFLT_CONTEXT  TransactionContext,
	__in ULONG  NotificationMask
	);


//+ global data
typedef struct _GLOBAL_DATA {
	PDRIVER_OBJECT		m_FilterDriverObject;
	PFLT_FILTER			m_Filter;
	PFLT_INSTANCE		m_SystemVolumeInstance;
	LARGE_INTEGER		m_RegCallback_Cookie;
	ERESOURCE			m_Resource;
} GLOBAL_DATA, *PGLOBAL_DATA;

GLOBAL_DATA Globals;
//- global data

//+ transaction context
typedef struct _MKAV_TRANSACTION_CONTEXT
{
	PVOID				m_pTransObj;
} MKAV_TRANSACTION_CONTEXT, *PMKAV_TRANSACTION_CONTEXT;

//- transaction context

#ifdef ALLOC_PRAGMA

#pragma message ("----------------- ALLOC_PRAGMA defined")
	#pragma alloc_text(INIT, DriverEntry)
	#pragma alloc_text(PAGE, Unload)
	#pragma alloc_text(PAGE, MiniKavInstanceSetup)
	#pragma alloc_text(PAGE, MiniKavTransactionNotify)
#endif //ALLOC_PRAGMA

NTSTATUS
MiniKavInstanceSetup (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in FLT_INSTANCE_SETUP_FLAGS Flags,
	__in DEVICE_TYPE VolumeDeviceType,
	__in FLT_FILESYSTEM_TYPE VolumeFilesystemType
	)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER( Flags );
	UNREFERENCED_PARAMETER( VolumeFilesystemType );

	PAGED_CODE();

	ASSERT( FltObjects->Filter == Globals.m_Filter );

	if (FILE_DEVICE_NETWORK_FILE_SYSTEM == VolumeDeviceType)
		return STATUS_FLT_DO_NOT_ATTACH;

	if (FLT_FSTYPE_RAW == VolumeFilesystemType)
		return STATUS_FLT_DO_NOT_ATTACH;

	status = FltObjectReference( FltObjects->Instance );

	if (NT_SUCCESS( status ))
	{
		// save first (!) instance for transaction
		FltAcquireResourceExclusive( &Globals.m_Resource );
		
		if (!Globals.m_SystemVolumeInstance)
			Globals.m_SystemVolumeInstance = FltObjects->Instance;
		
		FltReleaseResource( &Globals.m_Resource );

		if (Globals.m_SystemVolumeInstance != FltObjects->Instance)
			FltObjectDereference( FltObjects->Instance );
	}

	return STATUS_SUCCESS;
}

VOID
TransEnlistKey (
	__in PVOID pKeyObject,
	__in PMKAV_TRANSACTION_CONTEXT pTransactionContext
	)
{
	POBJECT_NAME_INFORMATION pKeyInfo;

	UNREFERENCED_PARAMETER( pTransactionContext );

	pKeyInfo = (POBJECT_NAME_INFORMATION) ExAllocatePoolWithTag( PagedPool, 0x300, 'KBos' );
	if (pKeyInfo)
	{
		ULONG RetLen = 0;
		NTSTATUS status;

		status = ObQueryNameString(pKeyObject, pKeyInfo, 0x300, &RetLen);
		if (NT_SUCCESS(status) && pKeyInfo->Name.Buffer)
		{
		}

		ExFreePool( pKeyInfo );
	}
}

VOID
PostCreateKeyTransProcessing (
	__in PREG_POST_OPERATION_INFORMATION pRegInfo
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PVOID pBoundTransaction = NULL;
	PFLT_INSTANCE SystemVolumeInstance = NULL;

	PMKAV_TRANSACTION_CONTEXT pTransactionContext = NULL;

	KIRQL kIrql = KeGetCurrentIrql();

	if (APC_LEVEL < kIrql)
	{
		DbgBreakPoint();
		return;
	}

	if (!pRegInfo || !NT_SUCCESS(pRegInfo->Status) || !pRegInfo->Object)
		return;

	pBoundTransaction = CmGetBoundTransaction( &Globals.m_RegCallback_Cookie, pRegInfo->Object );

	if (!pBoundTransaction)
		return;

	FltAcquireResourceExclusive( &Globals.m_Resource );

	if (Globals.m_SystemVolumeInstance)
	{
		status = FltObjectReference( Globals.m_SystemVolumeInstance );
		if (NT_SUCCESS( status ))
			SystemVolumeInstance = Globals.m_SystemVolumeInstance;
	}

	FltReleaseResource( &Globals.m_Resource );

	if (!SystemVolumeInstance)
		return;

	status = FltGetTransactionContext( SystemVolumeInstance, pBoundTransaction, (PFLT_CONTEXT) &pTransactionContext );
	
	if (!NT_SUCCESS(status))
		pTransactionContext = NULL;

	if (!pTransactionContext)
	{
		// no context allocated for this transaction, allocate one
		status = FltAllocateContext (
			Globals.m_Filter,
			FLT_TRANSACTION_CONTEXT,
			sizeof(MKAV_TRANSACTION_CONTEXT),
			NonPagedPool,
			(PFLT_CONTEXT*) &pTransactionContext
			);
		
		if (!NT_SUCCESS( status ))
			pTransactionContext = NULL;
		else
		{
			pTransactionContext->m_pTransObj = pBoundTransaction;

			status = FltSetTransactionContext (
					SystemVolumeInstance,
					pBoundTransaction,
					FLT_SET_CONTEXT_KEEP_IF_EXISTS,
					(PFLT_CONTEXT) pTransactionContext,
					NULL
					);

			if (!NT_SUCCESS(status))
			{
				FltReleaseContext( pTransactionContext );
				pTransactionContext = NULL;
			}
		}
	}

	if (pTransactionContext)
	{
		FltEnlistInTransaction (
			SystemVolumeInstance,
			pBoundTransaction,
			pTransactionContext,
			FLT_MAX_TRANSACTION_NOTIFICATIONS
			);

		TransEnlistKey( pRegInfo->Object, pTransactionContext );
		
		FltReleaseContext( pTransactionContext );
		pTransactionContext = NULL;
	}

	FltObjectDereference( SystemVolumeInstance );
}

NTSTATUS RegOpNotify(
	__in PVOID  CallbackContext,
	__in PVOID  Argument1,
	__in PVOID  Argument2 
	)
{
	NTSTATUS RetStatus = STATUS_SUCCESS;

	REG_NOTIFY_CLASS RegNotifyClass = (REG_NOTIFY_CLASS) Argument1;
	PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;


	UNREFERENCED_PARAMETER( CallbackContext );

	switch (RegNotifyClass)
	{
		case RegNtPostOpenKeyEx:
				PostCreateKeyTransProcessing( pRegInfo );
			break;

		case RegNtPostCreateKeyEx:
				PostCreateKeyTransProcessing( pRegInfo );
			break;

	default:
		break;
	}

	return RetStatus;
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
	UNREFERENCED_PARAMETER( NotificationMask );

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
		// cleanup transaction context
		pTransactionContext->m_pTransObj = NULL;
	}
}


// support transactions on vista
const FLT_CONTEXT_REGISTRATION ContextRegistration[] = {
	{FLT_TRANSACTION_CONTEXT, 0, TransactionContextCleanup, sizeof(MKAV_TRANSACTION_CONTEXT), 'TBos', NULL, NULL, NULL},
	{FLT_CONTEXT_END}
};

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
	{IRP_MJ_OPERATION_END}
};

NTSTATUS
DriverEntry (
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	FLT_REGISTRATION filterRegistration = {
		sizeof(FLT_REGISTRATION),		//  Size
		FLT_REGISTRATION_VERSION_0202,	//  Version
		0,								//  Flags
		ContextRegistration,			//  Context
		Callbacks,						//  Operation callbacks
		Unload,							//  unload routine
		MiniKavInstanceSetup,			//  InstanceSetup
		NULL,							//  InstanceQueryTeardown
		NULL,					        //  InstanceTeardownStart
		NULL,						    //  InstanceTeardownComplete
		NULL,							//  NameProvider callbacks
		NULL,							
		NULL,
		MiniKavTransactionNotify,		//  transaction callback
		NULL							// 
	};

	UNREFERENCED_PARAMETER( RegistryPath );

	RtlZeroMemory( &Globals, sizeof(Globals) );

	Globals.m_FilterDriverObject = DriverObject;
	ExInitializeResourceLite( &Globals.m_Resource );

	status = FltRegisterFilter (
		DriverObject,
		&filterRegistration,
		&Globals.m_Filter
		);

	if (!NT_SUCCESS( status ))
		return status;

	status = CmRegisterCallback( RegOpNotify, NULL, &Globals.m_RegCallback_Cookie );

	if (!NT_SUCCESS( status ))
	{
		FltUnregisterFilter( Globals.m_Filter );
		Globals.m_Filter = NULL;

		return status;
	}

	FltStartFiltering( Globals.m_Filter ); // dont check status - too lazy in this test

	return status;
}

NTSTATUS
Unload (
	FLT_FILTER_UNLOAD_FLAGS Flags
	)
{
	PFLT_INSTANCE SystemVolumeInstance = NULL;

	PAGED_CODE();

	UNREFERENCED_PARAMETER( Flags );

	FltAcquireResourceExclusive( &Globals.m_Resource );
	
	SystemVolumeInstance = Globals.m_SystemVolumeInstance;
	Globals.m_SystemVolumeInstance = NULL;
	
	FltReleaseResource( &Globals.m_Resource );

	CmUnRegisterCallback( Globals.m_RegCallback_Cookie );
	Globals.m_RegCallback_Cookie.QuadPart = 0L;

	if (SystemVolumeInstance)
		FltObjectDereference( SystemVolumeInstance );

	FltUnregisterFilter( Globals.m_Filter );
	Globals.m_Filter = NULL;

	ExDeleteResourceLite( &Globals.m_Resource );

	return STATUS_SUCCESS;
}