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

typedef struct _GLOBAL_DATA {
	PDRIVER_OBJECT		m_FilterDriverObject;
	PVOID				m_RegistrationHandle;
} GLOBAL_DATA, *PGLOBAL_DATA;

GLOBAL_DATA Globals;

#ifdef ALLOC_PRAGMA

#pragma message ("----------------- ALLOC_PRAGMA defined")
	#pragma alloc_text(INIT, DriverEntry)
#endif //ALLOC_PRAGMA

VOID
NTAPI
ObCallbackPreProcess (
    __in PVOID RegistrationContext,
    __inout POB_PRE_OPERATION_INFORMATION OperationInformation
    )
{
	UNREFERENCED_PARAMETER( RegistrationContext );
	
	if (OperationInformation)
	{
	}
}

NTSTATUS
NTAPI
ObCallbackPostProcess (
    __in PVOID RegistrationContext,
    __in POB_POST_OPERATION_INFORMATION OperationInformation
    )
{
	UNREFERENCED_PARAMETER( RegistrationContext );
	UNREFERENCED_PARAMETER( OperationInformation );

	return STATUS_SUCCESS;
}

VOID
NTAPI
ObCallbackPreThread (
    __in PVOID RegistrationContext,
    __inout POB_PRE_OPERATION_INFORMATION OperationInformation
    )
{
	UNREFERENCED_PARAMETER( RegistrationContext );
	
	if (OperationInformation->KernelHandle)
	{
		// kernel handle
	}
	else
	{
		// one of flag setted
		if (FlagOn( OperationInformation->Operation, OB_OPERATION_HANDLE_CREATE ))
		{
			// create handle
		}
		if (FlagOn( OperationInformation->Operation, OB_OPERATION_HANDLE_DUPLICATE ))
		{
			// Duplicate handle
		}
	}
}

NTSTATUS
NTAPI
ObCallbackPostThread (
    __in PVOID RegistrationContext,
    __in POB_POST_OPERATION_INFORMATION OperationInformation
    )
{
	UNREFERENCED_PARAMETER( RegistrationContext );
	UNREFERENCED_PARAMETER( OperationInformation );
	
	return STATUS_SUCCESS;
}

VOID
cbCreateNotifyEx (
	__inout PEPROCESS Process,
	__in HANDLE ProcessId,
	__in_opt PPS_CREATE_NOTIFY_INFO CreateInfo
	)
{
	UNREFERENCED_PARAMETER( Process );
	UNREFERENCED_PARAMETER( ProcessId );

	if (CreateInfo)
	{
		
	}
}

NTSTATUS
DriverEntry (
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	OB_OPERATION_REGISTRATION ObOperations[] = {
		{PsProcessType,		OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE,		ObCallbackPreProcess,	ObCallbackPostProcess},
		{PsThreadType,		OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE,		ObCallbackPreThread,	ObCallbackPostThread}
	};

	OB_CALLBACK_REGISTRATION ObRegistration = {
		OB_FLT_REGISTRATION_VERSION,
		sizeof(ObOperations) / sizeof(OB_OPERATION_REGISTRATION),
		{sizeof(L"320400") - sizeof(WCHAR), sizeof(L"320400"), L"320400"},
		&Globals,
		ObOperations
	};

	PVOID RegistrationHandle = NULL;

	UNREFERENCED_PARAMETER( RegistryPath );

	RtlZeroMemory( &Globals, sizeof(Globals) );

	Globals.m_FilterDriverObject = DriverObject;

	status = ObRegisterCallbacks( &ObRegistration, &RegistrationHandle );

	if (NT_SUCCESS( status ))
	{
		status = PsSetCreateProcessNotifyRoutineEx( cbCreateNotifyEx, FALSE );
		if (NT_SUCCESS( status ))
			Globals.m_RegistrationHandle = RegistrationHandle;
		else
		{
			ObUnRegisterCallbacks( RegistrationHandle );
			DbgBreakPoint();
		}
	}
	else
	{
		DbgBreakPoint();
	}

	return status;
}

// remove routines
//		PsSetCreateProcessNotifyRoutineEx( cbCreateNotifyEx, TRUE);*/
//		Globals.m_RegistrationHandle = NULL;
//		ObUnRegisterCallbacks( RegistrationHandle );

