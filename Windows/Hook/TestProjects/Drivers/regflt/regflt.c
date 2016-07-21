/*
1. I would like to receive callback for ZwRestoreKey on Vista x86/64. XP/W2003 server will be welcome too
2. This sample make BSOD on XP sp2

STACK_TEXT:  
f7a33c28 805a21db 806ce000 e1225960 86425838 nt!MiFindExportedRoutineByName+0x51
f7a33c64 b7c0e057 f7a33c70 002a0028 b7c0e0e0 nt!MmGetSystemRoutineAddress+0xab
f7a33c7c 80575550 86425838 86687000 00000000 regflt!DriverEntry+0x47 [k:\!test\!drivers\regflt\regflt.c @ 86]
f7a33d4c 8057565f 000011c8 00000001 00000000 nt!IopLoadDriver+0x66c
f7a33d74 80533dd0 000011c8 00000000 867c3020 nt!IopLoadUnloadDriver+0x45
f7a33dac 805c4a28 f6c8acf4 00000000 00000000 nt!ExpWorkerThread+0x100
f7a33ddc 80540fa2 80533cd0 00000001 00000000 nt!PspSystemThreadStartup+0x34
00000000 00000000 00000000 00000000 00000000 nt!KiThreadStartup+0x16


FOLLOWUP_IP: 
regflt!DriverEntry+47 [k:\!test\!drivers\regflt\regflt.c @ 86]
b7c0e057 a310d0c0b7      mov     dword ptr [regflt!pfCmGetCallbackVersion (b7c0d010)],eax

FAULTING_SOURCE_CODE:  
    82: 	{
    83: 		// crash on xp
    84: 		UNICODE_STRING usRoutineName;
    85: 		RtlInitUnicodeString( &usRoutineName, L"CmGetCallbackVersion" );
>   86: 		pfCmGetCallbackVersion = MmGetSystemRoutineAddress( &usRoutineName );
    87: 		if (pfCmGetCallbackVersion)
    88: 			pfCmGetCallbackVersion( &Globals.m_RegHookMajorVersion, NULL );
    89: 	}
    90: 
    91: 	status = CmRegisterCallback( RegOpNotify, NULL, &Globals.m_RegCallback_Cookie );

*/


#pragma warning(error:4100)     //  Enable-Unreferenced formal parameter
#pragma warning(error:4101)     //  Enable-Unreferenced local variable
#pragma warning(error:4061)     //  Eenable-missing enumeration in switch statement
#pragma warning(error:4505)     //  Enable-identify dead functions

#include <fltKernel.h>

typedef
VOID
(NTAPI* _tpfCmGetCallbackVersion) (
	__out_opt   PULONG  Major,
	__out_opt   PULONG  Minor
	);

_tpfCmGetCallbackVersion pfCmGetCallbackVersion = NULL;

NTSTATUS
DriverEntry (
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
	);


typedef struct _GLOBAL_DATA {
	PDRIVER_OBJECT		m_FilterDriverObject;
	
	ULONG				m_RegHookMajorVersion;
	LARGE_INTEGER		m_RegCallback_Cookie;
} GLOBAL_DATA, *PGLOBAL_DATA;

GLOBAL_DATA Globals;

#ifdef ALLOC_PRAGMA

#pragma message ("----------------- ALLOC_PRAGMA defined")
	#pragma alloc_text(INIT, DriverEntry)
#endif //ALLOC_PRAGMA

NTSTATUS RegOpNotify(
					 __in PVOID  CallbackContext,
					 __in PVOID  Argument1,
					 __in PVOID  Argument2 
					 )
{
	NTSTATUS			RetStatus = STATUS_SUCCESS;

	REG_NOTIFY_CLASS	RegNotifyClass = (REG_NOTIFY_CLASS)Argument1;

	UNREFERENCED_PARAMETER( CallbackContext );

	switch (RegNotifyClass)
	{
	case RegNtPreCreateKeyEx:
		if (Argument2)
		{
			PREG_CREATE_KEY_INFORMATION pRegInfo = (PREG_CREATE_KEY_INFORMATION) Argument2;
		}
		break;

	default:
		break;
	}

	return RetStatus;
}


NTSTATUS
DriverEntry (
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER( RegistryPath );

	RtlZeroMemory( &Globals, sizeof(Globals) );

	Globals.m_FilterDriverObject = DriverObject;

	{
		// crash on xp
		UNICODE_STRING usRoutineName;
		RtlInitUnicodeString( &usRoutineName, L"CmGetCallbackVersion" );
		pfCmGetCallbackVersion = MmGetSystemRoutineAddress( &usRoutineName );
		if (pfCmGetCallbackVersion)
			pfCmGetCallbackVersion( &Globals.m_RegHookMajorVersion, NULL );
	}

	status = CmRegisterCallback( RegOpNotify, NULL, &Globals.m_RegCallback_Cookie );

	return status;
}
