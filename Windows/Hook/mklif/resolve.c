#include "pch.h"

#include "resolve.tmh"

#ifdef ALLOC_PRAGMA
	#pragma alloc_text(INIT, IsUnloadAllowed)
	#pragma alloc_text(INIT, IsCalcHashAllowed)
	#pragma alloc_text(INIT, IsINetSwiftDisabled)
	#pragma alloc_text(INIT, IsISwiftDisabled)
	#pragma alloc_text(INIT, GetFileIdMode)
#endif //ALLOC_PRAGMA

#ifdef PECP_LIST_notdefined
	#pragma message ("----------------------- PECP_LIST not defined")
#endif //PECP_LIST_notdefined

#ifdef PPREFETCH_OPEN_ECP_CONTEXT_notdefined
	#pragma message ("----------------------- PPREFETCH_OPEN_ECP_CONTEXT not defined")
#endif // PPREFETCH_OPEN_ECP_CONTEXT_notdefined

#ifdef GUID_ECP_PREFETCH_OPEN_notdefined
	#pragma message ("----------------------- GUID_ECP_PREFETCH_OPEN not defined")
#endif // GUID_ECP_PREFETCH_OPEN_notdefined

_tpfFltGetEcpListFromCallbackData pfFltGetEcpListFromCallbackData = NULL;
_tpfFltFindExtraCreateParameter pfFltFindExtraCreateParameter = NULL;
_tpfFltIsEcpFromUserMode pfFltIsEcpFromUserMode = NULL;
_tpfFltQueryEaFile pfFltQueryEaFile = NULL;
_tpCmRegisterCallbackEx pfCmRegisterCallbackEx = NULL;
_tpfCmGetCallbackVersion pfCmGetCallbackVersion = NULL;
_tpfCmSetCallbackObjectContext pfCmSetCallbackObjectContext = NULL;

t_fIoGetLowerDeviceObject fIoGetLowerDeviceObject = NULL;

BOOLEAN g_bIsW2K = FALSE;
BOOLEAN g_bIsWXP = FALSE;
BOOLEAN g_bIsW2003 = FALSE;
BOOLEAN g_bIsVista = FALSE;

BOOLEAN g_bTransactionOperationsSupported = FALSE;

t_fFltGetTransactionContext		g_pfFltGetTransactionContext = NULL;
t_fFltSetTransactionContext		g_pfFltSetTransactionContext = NULL;
t_fFltEnlistInTransaction		g_pfFltEnlistInTransaction = NULL;
t_fCmGetBoundTransaction		g_pfCmGetBoundTransaction = NULL;
t_fFltRollbackEnlistment		g_pfFltRollbackEnlistment = NULL;
//- transaction operations for Vista -------------------------------------


//remark: may use FltGetRoutineAddress

VOID
ResolveVistaFunctions (
	)
{
	UNICODE_STRING usRoutineName;

	g_pfFltGetTransactionContext = (t_fFltGetTransactionContext) FltGetRoutineAddress( "FltGetTransactionContext" );
	g_pfFltSetTransactionContext = (t_fFltSetTransactionContext) FltGetRoutineAddress( "FltSetTransactionContext" );
	g_pfFltEnlistInTransaction = (t_fFltEnlistInTransaction) FltGetRoutineAddress( "FltEnlistInTransaction" );
	g_pfFltRollbackEnlistment = (t_fFltRollbackEnlistment) FltGetRoutineAddress("FltRollbackEnlistment");
	
	RtlInitUnicodeString( &usRoutineName, L"CmGetBoundTransaction" );
	g_pfCmGetBoundTransaction = (t_fCmGetBoundTransaction) MmGetSystemRoutineAddress( &usRoutineName );
	
	g_bTransactionOperationsSupported = g_pfFltGetTransactionContext
		&& g_pfFltSetTransactionContext
		&& g_pfFltEnlistInTransaction
		&& g_pfCmGetBoundTransaction
		&& g_pfFltRollbackEnlistment;

	if (!g_bTransactionOperationsSupported)
	{
		DoTraceEx(TRACE_LEVEL_ERROR, DC_DRIVER, "failed to init dynamic routines pointers on Vista!");
	}

	
	RtlInitUnicodeString( &usRoutineName, L"CmRegisterCallbackEx" );
	pfCmRegisterCallbackEx = MmGetSystemRoutineAddress( &usRoutineName );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "CmRegisterCallbackEx %S",
		pfCmRegisterCallbackEx ? L"resolved" : L"not resolved");

	RtlInitUnicodeString( &usRoutineName, L"CmGetCallbackVersion" );
	pfCmGetCallbackVersion = MmGetSystemRoutineAddress( &usRoutineName );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "CmGetCallbackVersion %S",
		pfCmGetCallbackVersion ? L"resolved" : L"not resolved");

/*	RtlInitUnicodeString( &usRoutineName, L"CmSetCallbackObjectContext" );
	pfCmSetCallbackObjectContext = MmGetSystemRoutineAddress( &usRoutineName );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "CmSetCallbackObjectContext %S",
		pfCmSetCallbackObjectContext ? L"resolved" : L"not resolved");
*/
}

VOID
Resolve_Init (
	)
{
	UNICODE_STRING usSysRoutine;

	g_bIsW2K = NtBuildNumber == 2195 ? TRUE : FALSE;
	g_bIsWXP = NtBuildNumber == 2600 ? TRUE : FALSE;
	g_bIsW2003 = NtBuildNumber == 3790 ? TRUE : FALSE;
	g_bIsVista = NtBuildNumber > 5000;

	pfFltGetEcpListFromCallbackData = FltGetRoutineAddress( "FltGetEcpListFromCallbackData" );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FltGetEcpListFromCallbackData %S",
		pfFltGetEcpListFromCallbackData ? L"resolved" : L"not resolved");

	pfFltFindExtraCreateParameter = FltGetRoutineAddress( "FltFindExtraCreateParameter" );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FltFindExtraCreateParameter %S",
		pfFltGetEcpListFromCallbackData ? L"resolved" : L"not resolved");

	pfFltIsEcpFromUserMode = FltGetRoutineAddress( "FltIsEcpFromUserMode" );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FltIsEcpFromUserMode %S",
		pfFltGetEcpListFromCallbackData ? L"resolved" : L"not resolved");

	pfFltQueryEaFile  = FltGetRoutineAddress( "FltQueryEaFile" );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "FltQueryEaFile %S",
		pfFltQueryEaFile ? L"resolved" : L"not resolved");

	// for win2k compatibility
	RtlInitUnicodeString(
		&usSysRoutine,
		L"IoGetLowerDeviceObject"
		);
	fIoGetLowerDeviceObject = MmGetSystemRoutineAddress( &usSysRoutine );

	if (g_bIsVista)
		ResolveVistaFunctions();
}

// settings
DWORD
RegQueryValue (
	PWCHAR pwchValueName,
	DWORD* pdwValue
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	UNICODE_STRING usValueName;
	HANDLE hKey = NULL;
	OBJECT_ATTRIBUTES KeyOA;

	RtlInitUnicodeString( &usValueName, pwchValueName );

	InitializeObjectAttributes (
		&KeyOA,
		&Globals.m_RegParams,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
		);

	status = ZwOpenKey( &hKey, KEY_READ, &KeyOA );

	if (NT_SUCCESS( status ))
	{
		PCHAR buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD)];
		PKEY_VALUE_PARTIAL_INFORMATION pValueInfo = (PKEY_VALUE_PARTIAL_INFORMATION) buffer;
		ULONG size = sizeof(buffer);

		status = ZwQueryValueKey (hKey, &usValueName, KeyValuePartialInformation, pValueInfo, size, &size);
			
		if(NT_SUCCESS( status ) && pValueInfo->DataLength == sizeof(DWORD))
			*pdwValue = *(DWORD*) pValueInfo->Data;
		else
			status = STATUS_UNSUCCESSFUL;

		ZwClose( hKey );
	}

	return status;
}

NTSTATUS
RegQueryStr (
	__in PUNICODE_STRING KeyPath,
	__in PWCHAR pwchValueName,
	__out PUNICODE_STRING pusStrValue
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	UNICODE_STRING usValueName;
	HANDLE hKey = NULL;
	OBJECT_ATTRIBUTES KeyOA;

	RtlZeroMemory( pusStrValue, sizeof(UNICODE_STRING) );

	RtlInitUnicodeString( &usValueName, pwchValueName );

	InitializeObjectAttributes (
		&KeyOA,
		KeyPath,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
		);

	status = ZwOpenKey( &hKey, KEY_READ, &KeyOA );

	if ( NT_SUCCESS( status ) )
	{
		PCHAR buffer;
		PKEY_VALUE_PARTIAL_INFORMATION pValueInfo;
		ULONG size = 0;

		status = ZwQueryValueKey (hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &size);
		if ( STATUS_BUFFER_TOO_SMALL == status )
			status = STATUS_SUCCESS;
		
		if ( NT_SUCCESS( status ) )
		{
			buffer = ExAllocatePoolWithTag( PagedPool, size, tag_resolve1 );
			if ( !buffer )
				status = STATUS_NO_MEMORY;
			
			if ( NT_SUCCESS( status ) )
			{
				pValueInfo = (PKEY_VALUE_PARTIAL_INFORMATION) buffer;

				status = ZwQueryValueKey (hKey, &usValueName, KeyValuePartialInformation, pValueInfo, size, &size);
				if( NT_SUCCESS( status ) )
				{
					PWCHAR wcStr;
					wcStr = ExAllocatePoolWithTag( PagedPool, pValueInfo->DataLength, tag_resolve2 );
					if ( !wcStr )
						status = STATUS_NO_MEMORY;
					
					if( NT_SUCCESS( status ) )
					{	
						RtlCopyMemory ( wcStr, pValueInfo->Data, pValueInfo->DataLength );
						
						RtlInitEmptyUnicodeString( pusStrValue, wcStr, (USHORT)pValueInfo->DataLength );
						pusStrValue->Length = (USHORT) pValueInfo->DataLength;
					}
				}
				ExFreePool( buffer) ;
			}
		}
		ZwClose( hKey );
	}

	return status;
}

void
CheckSysPatch (
	)
{
	DWORD dwValue = 0;
#if defined(_WIN64)
	// not implemented
#else
	if (g_bIsW2003 || g_bIsW2K || g_bIsWXP)
	{
		NTSTATUS status = RegQueryValue( L"SysPatch", &dwValue );
		if (NT_SUCCESS(status) )
		{
			if (1 == dwValue)
				SetFlag( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH );
		}
	}
#endif
}

BOOLEAN
IsUnloadAllowed (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD dwValue = 0;

	status = RegQueryValue( L"UA", &dwValue );
	
	if (NT_SUCCESS(status) )
	{
		if (1 == dwValue)
			return TRUE;
	}

	return FALSE;
}

BOOLEAN
IsCalcHashAllowed (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD dwValue;

	status = RegQueryValue( L"ProcHash", &dwValue );

	if (NT_SUCCESS(status) )
	{
		if (1 == dwValue)
			return TRUE;
	}

	return FALSE;
}

BOOLEAN
IsINetSwiftDisabled (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD dwValue;

	status = RegQueryValue( L"NoINetSwift", &dwValue );

	if (NT_SUCCESS(status) )
	{
		if (1 == dwValue)
			return TRUE;
	}

	return FALSE;
}

BOOLEAN
IsISwiftDisabled (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD dwValue;

	status = RegQueryValue( L"NoISwift", &dwValue );

	if (NT_SUCCESS(status) )
	{
		if (1 == dwValue)
			return TRUE;
	}

	return FALSE;
}

ULONG
GetFileIdMode (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD dwValue;

	status = RegQueryValue( L"ObjIdMode", &dwValue );

	if (NT_SUCCESS(status) )
		return dwValue;

	return 2;
}

NTSTATUS
GetAltitude (
	__out PUNICODE_STRING pusAltitude
)
{
	return RegQueryStr (
		&Globals.m_RegInstances,
		L"Altitude",
		pusAltitude
		);
}

BOOLEAN
IsTimingEnabled (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD dwValue;

	status = RegQueryValue( L"Timing", &dwValue );

	if (NT_SUCCESS(status) )
	{
		if (1 == dwValue)
			return TRUE;
	}

	return FALSE;
}
