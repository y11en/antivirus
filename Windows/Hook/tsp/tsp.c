
//#undef DBG		//Don't forget to Undef when release compile!!!
#ifdef DBG
	#pragma message ("Debug defined!")
	#define __DBG__
#endif

#include "ntifs.h"
#include "ntdddisk.h"


// -----------------------------------------------------------------------------------------
#ifdef EHB
	#pragma message("hardware break points enabled!")
//	#define BreakPoint DbgBreakPoint();
#else
	#pragma message("hardware break points disabled!")
//	#define BreakPoint 
#endif
// -----------------------------------------------------------------------------------------


//_________________  MS MD  _______________________ 
// These defines don't doubled in ntifs.h

#ifndef IoSetNextIrpStackLocation
#define IoSetNextIrpStackLocation( Irp ) {      \
    (Irp)->CurrentLocation--;                   \
    (Irp)->Tail.Overlay.CurrentStackLocation--; }
#endif

#ifndef IoCopyCurrentIrpStackLocationToNext
#define IoCopyCurrentIrpStackLocationToNext( Irp ) { \
    PIO_STACK_LOCATION irpSp; \
    PIO_STACK_LOCATION nextIrpSp; \
    irpSp = IoGetCurrentIrpStackLocation( (Irp) ); \
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) ); \
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine)); \
    nextIrpSp->Control = 0; }
#endif

#ifndef	IoSkipCurrentIrpStackLocation
#define IoSkipCurrentIrpStackLocation( Irp ) {\
	(Irp)->CurrentLocation++; \
	(Irp)->Tail.Overlay.CurrentStackLocation++;}
#endif

ULONG KeSetAffinityThread(PETHREAD Thread,ULONG AffinityMask);
NTSYSAPI NTSTATUS NTAPI ZwOpenSymbolicLinkObject(OUT PHANDLE LinkHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes);
NTSYSAPI NTSTATUS NTAPI ZwQuerySymbolicLinkObject(IN HANDLE LinkHandle,IN OUT PUNICODE_STRING LinkTarget,OUT PULONG ReturnedLength OPTIONAL);

//_________________________________________________

//NTSTATUS (__stdcall *NtCreateSemaphore)(OUT PHANDLE Handle, IN ACCESS_MASK Access, IN POBJECT_ATTRIBUTES ObjAttr, IN LONG InitCount, IN LONG MaxCount);
//NTSTATUS (__stdcall *NtOpenSemaphore) (OUT PHANDLE SemaphoreHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes);
NTSTATUS (__stdcall *NtTerminateProcess)(IN HANDLE ProcessHandle, IN ULONG ProcessExitCode);
//NTSTATUS (__stdcall *NtTerminateThread)(IN HANDLE ThreadHandle, IN ULONG ProcessExitCode);

//BOOLEAN ObFindHandleForObject(IN PVOID Object,IN ACCESS_MASK DesiredAccess,IN POBJECT_TYPE ObjectType,IN KPROCESSOR_MODE AccessMode,OUT PHANDLE pHandle);
//NTSTATUS ObOpenObjectByPointer(IN PVOID Object,IN ULONG HandleAttributes,IN PACCESS_STATE PassedAccessState,IN ACCESS_MASK DesiredAccess,IN POBJECT_TYPE ObjectType,IN KPROCESSOR_MODE AccessMode,OUT PHANDLE Handle);

//NTSTATUS ObOpenObjectByName(IN POBJECT_ATTRIBUTES ObjAttr,IN POBJECT_TYPE ObjectType,ULONG U_0,ULONG pAccessState,IN ACCESS_MASK DesiredAccess,ULONG U1_0,OUT PHANDLE pHandle);
//NTSTATUS ObQueryNameString(PVOID Object,PUNICODE_STRING Name,ULONG MaximumLength,PULONG ActualLength);

NTSTATUS ObReferenceObjectByName(IN PUNICODE_STRING ObjectPath,IN ULONG Attributes,
																 IN PACCESS_STATE PassedAccessState OPTIONAL,IN ACCESS_MASK DesiredAccess OPTIONAL,
																 IN POBJECT_TYPE ObjectType,IN KPROCESSOR_MODE AccessMode,
																 IN OUT PVOID ParseContext OPTIONAL,OUT PVOID *ObjectPtr);  
//The first parameter, ObjectPath, points to a counted unicode string that holds the path of the object, e.g. \Driver\Serial
//Attributes may be zero or OBJ_CASE_INSENSITIVE, which indicates that the name lookup should be performed ignoring the case of the ObjectName.
//Set parameter AccessMode to KernelMode.
//Parameter ObjectType optionally points to a 'type' object, which can be used to limit the search for the object to a particular type.
//ParseContext is an optional pointer that is passed uninterpreted to any parse procedure that is called during the course of performing the name lookup. 
//Parameter ObjectPtr is the address of the variable that receives a pointer to the object if the object is found 
//This service is only callable at PASSIVE_LEVEL.

NTSYSAPI NTSTATUS NTAPI ZwQueryInformationProcess(IN HANDLE ProcessHandle, IN ULONG ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);

NTSTATUS ZwQuerySystemInformation(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);
NTSYSAPI NTSTATUS NTAPI ZwOpenDirectoryObject(OUT PHANDLE DirectoryHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes);
NTSTATUS (__stdcall *NtQueryDirectoryObject)(IN HANDLE DirectoryHandle,OUT PVOID Buffer,IN ULONG BuffLength,IN BOOLEAN ReturnSingleEntry,IN BOOLEAN RestartScan,IN OUT PULONG Context,OUT PULONG ReturnLength OPTIONAL);

typedef struct _DRIVER_INFO {
	ULONG Unknown1;
	PVOID	BaseAddress;
	ULONG	Size;
	ULONG Unknown3;
	ULONG	Index;
	ULONG Unknown4;
	CHAR	PathName[0x104];
}DRIVER_INFO,*PDRIVER_INFO;

typedef struct _SYSTEM_INFO_DRIVERS {
	ULONG	NumberOfDrivers;
	ULONG	Reserved;
	DRIVER_INFO Drivers[0x100];
}SYSTEM_INFO_DRIVERS,*PSYSTEM_INFO_DRIVERS;

#define QUERY_DIRECTORY_BUFF_SIZE 0x200

#ifndef FILE_DEVICE_DFS_FILE_SYSTEM
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#endif

extern PSHORT NtBuildNumber;

//extern ULONG **KeServiceDescriptorTable;
//extern POBJECT_TYPE PsProcessType;
//extern POBJECT_TYPE IoDriverObjectType;
//extern POBJECT_TYPE IoDeviceObjectType;

//#define SEMAPHORE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)
//#define SEMAPHORE_MODIFY_STATE      0x0002  // winnt

typedef struct {
   PDEVICE_OBJECT	FileSystem;
   ULONG				LogicalDrive;
} FSHOOK_EXTENSION, *PFSHOOK_EXTENSION;


#include "..\defs.h"
#include "../nt/glbenvir.h"
#include "stdio.h"
#include "stdarg.h"


PDRIVER_OBJECT	HookerDriverObject=NULL;
PDEVICE_OBJECT	HookerDeviceObject=NULL;



#define _TSP_DEDICATED_DRIVER_
#include "..\osspec.h"
#define DBG_PREFIX "TSP"
#include "..\debug.h"
#include "..\hook\avpgcom.h"
#include "..\Hook\HookSpec.h"
#include "..\osspec.c"

#include "..\TSPv2.c"


/******************************** ioctl stubs ***************************************************/
BOOLEAN RegisterApplication(PAPP_REGISTRATION pReg, PCLIENT_INFO pClientInfo)
{
	return TRUE;
}

BOOLEAN ClientActivity (PHDSTATE In, PHDSTATE Out) 
{
	return TRUE;
}

//!!!!!!!!!!! надо бы возвращать boolean
VERDICT __stdcall FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject)
{
	//return FALSE;
	return Verdict_NotFiltered;
}

void ClientSetFlowTimeout(PFLOWTIMEOUT pFlow)
{
}

void GetEventSizeForClient (ULONG AppID, DWORD *pSize) 
{
}

NTSTATUS SkipEventForClient (ULONG AppID) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS YeldEventForClient (PYELD_EVENT pYeldEvent, ULONG InYeldSize, PCHAR OutputBuffer, DWORD* pOutDataLen) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS ClientResetCache (ULONG AppID) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EventSetVerdict (PSET_VERDICT pSetVerdict) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS InvisibleTransmit (PINVISIBLE_TRANSMIT pInvTransmit, PINVISIBLE_TRANSMIT pInvTransmitOut) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS FilterTransmit(PFILTER_TRANSMIT pFilterTransmitIn, PFILTER_TRANSMIT pFilterTransmitOut, ULONG OutputBufferLength)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DoClientCommonMemoryOp(PCOMMON_MEMORY pInMem, PCOMMON_MEMORY pOutMem)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS ImpersonateThreadForClient(PIMPERSONATE_REQUEST pImperonate)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DoExternalDrvRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DoEventRequest(PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, DWORD* pOutDataLen)
{
	return STATUS_NOT_IMPLEMENTED;
}

BOOLEAN IsNeedFilterEvent(ULONG HookID, ULONG FuncMj, ULONG FuncMi)
{
	return FALSE;
}

BOOLEAN GetEventForClient(PGET_EVENT pGetEvent, PEVENT_TRANSMIT pEventTransmit, DWORD* pOutputBufferLength)
{
	return FALSE;
}


#include "..\ioctl.c"

BOOLEAN GetDriverInfo(PADDITIONALSTATE pAddState)
{
	pAddState->DeadlockCount = 0;
	pAddState->FilterEventEntersCount = 0;
	pAddState->ReplyWaiterEnters = 0;
	pAddState->DrvFlags = _DRV_FLAG_NONE;
#ifdef __DBG__
	pAddState->DrvFlags |= _DRV_FLAG_DEBUG;
#endif
	if (TSPInitedOk == TRUE)
		pAddState->DrvFlags |= _DRV_FLAG_TSP_OK;
//	if(SysCallsInited==TRUE)
//		pAddState->DrvFlags |= _DRV_FLAG_DIRECTSYSCALL;
	pAddState->DirectSysCallStartId=0;//DirectSysCallStartId;
	return TRUE;
}


/******************************** MyDev Dispatch routines ***************************************************/
NTSTATUS MyDevDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
NTSTATUS					ntStatus;
PIO_STACK_LOCATION	irpStack			= IoGetCurrentIrpStackLocation(Irp);
PVOID						InBuffer			= Irp->AssociatedIrp.SystemBuffer;
ULONG						InBufferLen		= irpStack->Parameters.DeviceIoControl.InputBufferLength;
PVOID						OutBuffer		= Irp->AssociatedIrp.SystemBuffer;
ULONG						OutBufferLen	= irpStack->Parameters.DeviceIoControl.OutputBufferLength;
ULONG						ioControlCode	= irpStack->Parameters.DeviceIoControl.IoControlCode;
//Проверить "подпись"!
	Irp->IoStatus.Status      = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	switch (irpStack->MajorFunction) {
		case IRP_MJ_CREATE:
			DbPrint(DC_SYS,DL_NOTIFY, ("IRP_MJ_CREATE\n"));
//			MmResetDriverPaging(DriverEntry);
			break;
		case IRP_MJ_CLEANUP:
//Never acquire one of the driver's own spin locks (if you have any) in your DispatchCleanup routine,
//before acquiring the system-wide cancel spin lock (IoAcquireCancelSpinLock). Following a consistant 
//lock acquisition hierarchy throughout your driver is essential to avoiding potential deadlocks
			DbPrint(DC_SYS,DL_NOTIFY, ("IRP_MJ_CLEANUP\n"));
			//Здесь типа ето IRP нехило бы зафайлить (как советует МС в доке на IFS)
			break;
		case IRP_MJ_CLOSE:
			DbPrint(DC_SYS,DL_NOTIFY, ("IRP_MJ_CLOSE.\n"));
			break;
		case IRP_MJ_SHUTDOWN:
			DbPrint(DC_SYS,DL_NOTIFY, ("IRP_MJ_SHUTDOWN\n"));
//			DoneSysCalls();
			TSPDone();
			break;
		case IRP_MJ_DEVICE_CONTROL:
			DbPrint(DC_SYS,DL_SPAM, ("IRP_MJ_DEVICE_CONTROL\n"));
			if(Irp->MdlAddress)
				OutBuffer = MmGetSystemAddressForMdl(Irp->MdlAddress);
			HOOKDeviceControl(InBuffer,InBufferLen, OutBuffer,OutBufferLen,
				ioControlCode,&Irp->IoStatus.Information,&Irp->IoStatus.Status, TRUE);
			break;
		default:
			DbPrint(DC_SYS,DL_FATAL_ERROR, ("Unprocessed IRP_MJ_???????\n"));
			Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			break;
		}
	ntStatus = Irp->IoStatus.Status;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return ntStatus;
}

VOID ProcessCrNotify(IN HANDLE  ParentId,IN HANDLE  ProcessId,IN BOOLEAN  Create)
{
	DbPrint(DC_SYS,DL_SPAM, ("ProcessCrNotify ParentId=%x ProcessId=%x Create=%x\n",ParentId,ProcessId,Create));
	if(!Create) {
		TSPProcCrash(ProcessId);
	}
}

VOID ThreadCrNotify(IN HANDLE ProcessId,IN HANDLE ThreadId, IN BOOLEAN Create)
{
	DbPrint(DC_SYS,DL_SPAM, ("ThreadCrNotify ProcessId=%x ThreadId=%x Create=%x CurThID=%x\n",ProcessId,ThreadId,Create,PsGetCurrentThreadId()));
	// Удаляемая трида текущая, создаваемая нет
	if (!Create) {
		TSPThreadCrash((PVOID)ThreadId);
	}
}

VOID SetNotifiers(VOID)
{
	if(PsSetCreateProcessNotifyRoutine(ProcessCrNotify,FALSE)!=STATUS_SUCCESS) {
		DbPrint(DC_SYS,DL_ERROR, ("PsSetCreateProcessNotifyRoutine failed\n"));
	}
	if(PsSetCreateThreadNotifyRoutine(ThreadCrNotify)!=STATUS_SUCCESS) {
		DbPrint(DC_SYS,DL_ERROR, ("PsSetCreateThreadNotifyRoutine failed\n"));
	}
}

WCHAR	deviceLinkBuffer[]=L"\\DosDevices\\TSP";

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
NTSTATUS				ntStatus;
WCHAR					deviceNameBuffer[]  = L"\\Device\\TSP";
UNICODE_STRING		deviceNameUnicodeString;
UNICODE_STRING		deviceLinkUnicodeString;
UNICODE_STRING		parameters;
ULONG					i;
RTL_QUERY_REGISTRY_TABLE	paramTable[2];
	DbPrint(DC_SYS,DL_IMPORTANT, ("entering DriverEntry on NtBuildNumber %d\n",*NtBuildNumber));
// Fill Driver fields
	HookerDriverObject=DriverObject;
	for(i=0;i<=IRP_MJ_MAXIMUM_FUNCTION;i++)
		DriverObject->MajorFunction[i]=MyDevDispatch;
//	DriverObject->DriverUnload=HookUnload; // It sets by IRP_MJ_CLOSE request
	DriverObject->FastIoDispatch=NULL;//&FastIOHook;

	if(!InitGlobalEnvironment())
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR,("Incorrect ModBase ntdll=%x, ntoskrnl=%x\n",BaseOfNtDllDll,BaseOfNtOsKrnl));
		DbgBreakPoint();
		ntStatus=STATUS_ACCESS_VIOLATION;
		goto done_DriverEntry;
	}
	
// Create AVPG device and SymbolycLink
	RtlInitUnicodeString (&deviceNameUnicodeString,deviceNameBuffer);
	ntStatus=IoCreateDevice(DriverObject,0,&deviceNameUnicodeString, _KLG_HOOK, 0,FALSE,&HookerDeviceObject);
	if(!NT_SUCCESS(ntStatus)) {
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("IoCreateDevice failed\n"));
		goto done_DriverEntry;
	}
	DbPrint(DC_SYS,DL_NOTIFY, ("%S created\n",deviceNameUnicodeString.Buffer));
	RtlInitUnicodeString (&deviceLinkUnicodeString,deviceLinkBuffer);
	ntStatus=IoCreateSymbolicLink (&deviceLinkUnicodeString,&deviceNameUnicodeString);
	if(!NT_SUCCESS(ntStatus)) {
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("IoCreateSymbolicLink failed\n"));
		goto done_DriverEntry;
	}
	DbPrint(DC_SYS,DL_NOTIFY ,("%S linked\n",deviceLinkUnicodeString.Buffer));

	HookerDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	IoRegisterShutdownNotification(HookerDeviceObject);
	GetThreadIdOffsetInETHREAD();
	SetNotifiers();
	TSPInit();
//	InitSysCalls();
//	MmPageEntireDriver(DriverEntry);

	DbPrint(DC_SYS,DL_NOTIFY, ("==========================================================================\n"));
	DbPrint(DC_SYS,DL_NOTIFY ,("driver start success\n"));
	return STATUS_SUCCESS;
	
done_DriverEntry:
	DbgPrint("TSP: DriverEntry fail (NtStatus=%x)\n",ntStatus);
	IoDeleteSymbolicLink(&deviceLinkUnicodeString);
	if (HookerDeviceObject)
		IoDeleteDevice(HookerDeviceObject);
	return ntStatus;
//DbgBreakPoint();
}



