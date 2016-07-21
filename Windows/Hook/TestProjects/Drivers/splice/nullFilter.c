/*++

Copyright (c) 1999 - 2002  Microsoft Corporation

Module Name:

    nullFilter.c

Abstract:

    This is the main module of the nullFilter mini filter driver.
    It is a simple minifilter that registers itself with the main filter
    for no callback operations.

Environment:

    Kernel mode

--*/

//#include <fltKernel.h>
//#include <dontuse.h>
//#include <suppress.h>

#include <ntifs.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

//---------------------------------------------------------------------------
//      Global variables
//---------------------------------------------------------------------------

#define NULL_FILTER_FILTER_NAME     L"NullFilter"

//typedef struct _NULL_FILTER_DATA {
//
//    //
//    //  The filter handle that results from a call to
//    //  FltRegisterFilter.
//    //
//
//    PFLT_FILTER FilterHandle;
//
//} NULL_FILTER_DATA, *PNULL_FILTER_DATA;
//

/*************************************************************************
    Prototypes for the startup and unload routines used for
    this Filter.

    Implementation in nullFilter.c
*************************************************************************/

NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

VOID
DriverUnload (
	__in PDRIVER_OBJECT DriverObject
			 );

extern void Cr0off();
extern void Cr0on();

//
//  Structure that contains all the global data structures
//  used throughout NullFilter.
//

//NULL_FILTER_DATA NullFilterData;

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif


//
//  This defines what we want to filter with FltMgr
//

//CONST FLT_REGISTRATION FilterRegistration = {
//
//    sizeof( FLT_REGISTRATION ),         //  Size
//    FLT_REGISTRATION_VERSION,           //  Version
//    0,                                  //  Flags
//
//    NULL,                               //  Context
//    NULL,                               //  Operation callbacks
//
//    NullUnload,                         //  FilterUnload
//
//    NULL,                               //  InstanceSetup
//    NullQueryTeardown,                  //  InstanceQueryTeardown
//    NULL,                               //  InstanceTeardownStart
//    NULL,                               //  InstanceTeardownComplete
//
//    NULL,                               //  GenerateFileName
//    NULL,                               //  GenerateDestinationFileName
//    NULL                                //  NormalizeNameComponent
//
//};


/*************************************************************************
    Filter initialization and unload routines.
*************************************************************************/


/*void InsertByte(ULONG Addr, unsigned char Byte)
{
	*((unsigned char*)Addr) = Byte;
}*/
//--------------------------------------------------------------------------------------
/*void InsertDword(ULONG Addr, ULONG dWord)
{
	*((PULONG)Addr) = dWord;
}*/

void InsertQword(ULONGLONG Addr, ULONGLONG dQord)
{
	*((PULONGLONG)Addr) = dQord;
}

//--------------------------------------------------------------------------------------
#define SIZEOFJUMP	14
#define ASMNOP		0x90
#define ASMJMP		0xe9

//
//	mov eax, ptr
// jmp eax
//
//CHAR HookStub[] =
//"\x48\xb8\x41\x41\x41\x41\x41\x41\x41\x41\xff\xe0";

//
//	eee dq 0x1122334455667788
// jmp eee
//
//CHAR HookStub[] =
//"\x88\x77\x66\x55\x44\x33\x22\x11\xeb\xf6";

CHAR HookStub[] =
"\xff\x25\x00\x00\x00\x00\x88\x77\x66\x55\x44\x33\x22\x11";


void GenJmp(ULONGLONG To, ULONGLONG From)
{	
	
	memcpy ((PVOID*)From,HookStub,SIZEOFJUMP);
	InsertQword(From+6, To);	// dst - src - 5
	
}
//--------------------------------------------------------------------------------------

// set hook
PVOID Hook(PVOID Addr, PVOID NewFunc)
{
	PVOID CallGate, Inst = Addr;
	ULONGLONG Size = 0, CollectedSpace = 0, CallGateSize = 0; 

	if (Addr == NULL)
		return 0;

	/*while (CollectedSpace < SIZEOFJUMP)
	{
		GetInstLenght(Inst, &Size);
		(ULONG)Inst += Size;
		CollectedSpace += Size;
	}*/
	
	Size=0xe;
	(ULONGLONG)Inst += Size;
	CollectedSpace += Size;


	CallGateSize = CollectedSpace + SIZEOFJUMP;
	CallGate = (PVOID)ExAllocatePool(NonPagedPool, CallGateSize);

	DbgPrint("Function addr : 0x%.8x\n", Addr);
	DbgPrint("Handler addr : 0x%.8x\n", NewFunc);
	DbgPrint("CallGate at : 0x%.8x; size : %d\n", CallGate, CallGateSize);

	memset(CallGate, ASMNOP, CallGateSize);

	memcpy(CallGate, Addr, CollectedSpace);

	memset(Addr, ASMNOP, CollectedSpace);

	GenJmp((ULONGLONG)Addr + 0xe, (ULONGLONG)CallGate + CollectedSpace);
	GenJmp((ULONGLONG)NewFunc, (ULONGLONG)Addr);

	return CallGate;
} 

typedef NTSTATUS (* funcZwOpenProcess)(
									   PHANDLE				ProcessHandle,
									   ACCESS_MASK			DesiredAccess,
									   POBJECT_ATTRIBUTES	ObjectAttributes,
									   PCLIENT_ID			ClientId
									   );


funcZwOpenProcess			OldZwOpenProcess;

NTSTATUS __stdcall NewZwOpenProcess(
									PHANDLE				ProcessHandle,
									ACCESS_MASK			DesiredAccess,
									POBJECT_ATTRIBUTES	ObjectAttributes,
									PCLIENT_ID			ClientId)
{
	NTSTATUS ns;
	
	DbgPrint("\n NtOpenProcess NewZwOpenProcess before hook! \n");

	ns = OldZwOpenProcess(
		ProcessHandle, 
		DesiredAccess, 
		ObjectAttributes, 
		ClientId
		);

	DbgPrint("\n NtOpenProcess  NewZwOpenProcess after hook! \n");

	return ns;
}


NTSTATUS
SetHooks(void)
{
	UNICODE_STRING SymbolName;
	NTSTATUS Status = STATUS_SUCCESS;
	PVOID KeBugCheckExSymbol;
	PUCHAR LockedAddress;
	PMDL Mdl = NULL;


	RtlInitUnicodeString(
		&SymbolName,
		L"NtOpenProcess");

	

	
	if (!(KeBugCheckExSymbol = MmGetSystemRoutineAddress(&SymbolName)))
	{
		Status = STATUS_PROCEDURE_NOT_FOUND;
		return Status;
	}


	if (!(Mdl = MmCreateMdl(
		NULL,
		(PVOID)KeBugCheckExSymbol,
		0xE)))
	{
		Status = STATUS_INSUFFICIENT_RESOURCES;
		return Status;
	}

	MmBuildMdlForNonPagedPool(
		Mdl);
	//
	// Probe & Lock.
	//
	if (!(LockedAddress = (PUCHAR)MmMapLockedPages(
		Mdl,
		KernelMode)))
	{
		IoFreeMdl(
			Mdl);
		Status = STATUS_ACCESS_VIOLATION;
		return Status;
	}

	OldZwOpenProcess= Hook (KeBugCheckExSymbol, NewZwOpenProcess);

	
	//
	// Cleanup the MDL.
	//
	MmUnmapLockedPages(
		LockedAddress,
		Mdl);
	IoFreeMdl(
		Mdl);



	return Status;

}



NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine for this miniFilter driver. This
    registers the miniFilter with FltMgr and initializes all
    its global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.
    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( RegistryPath );
	status=STATUS_SUCCESS;

    //
    //  Register with FltMgr
    //

   /* status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &NullFilterData.FilterHandle );*/

    ASSERT( NT_SUCCESS( status ) );

    //if (NT_SUCCESS( status )) {

    //    //
    //    //  Start filtering i/o
    //    //

    //    status = FltStartFiltering( NullFilterData.FilterHandle );

    //    if (!NT_SUCCESS( status )) {
    //        FltUnregisterFilter( NullFilterData.FilterHandle );
    //    }
    //}
	
	DriverObject->DriverUnload=DriverUnload;
	
	//DbgBreakPoint();
	Cr0off();
	status=SetHooks();
	Cr0on();
	

	
	DbgPrint("\n NullFilter loaded! \n");
    return status;
}

VOID
DriverUnload (
			  __in PDRIVER_OBJECT DriverObject
			  )
{

	DbgPrint("\nNullFilter unload! \n");

}