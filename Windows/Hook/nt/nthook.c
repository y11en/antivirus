
/* 
Known bugz and incompleteness:
-Накладки с именами файлов и ключей. (Проверить! FastIOWrite & CreateKey)
-Kill. Убитый процесс наверху помнят и любят, Копать сервер win32.
-У имен могут быть алиасы. (макроподстановки? Хитрые wildcards?)
-OSR guys say that IRP remains in the system for about 5 minutes waiting to be handled. ???
-Просмотреть Registry Hookers на предмет неполной информации и возращаемых в случае отказа значений.QueryMultipleValueKey
-----------------
-Under veryfier Caller has manually copied the stack and has inadvertantly copied the upper layer's completion routine. Please use IoCopyCurrentIrpStackLocationToNext. (Whata hella??!!)
-IsInvisibleThread() fail on referenced memory which was earlier freed. in line "if (pInvQueue->ThreadID == ThreadID)"
-maybe turn whole processing only on passive level??? (and change memory allocation to paged pool)
-----------------
*/

/*#ifdef __DBG__
//#define CHECK_WHAT_AVPM_DO !!! place this define into workspace settings
#endif*/


#ifdef DBG
#pragma message ("----------------------- Debug defined!")
#define __DBG__
#endif

#include "ntifs.h"
#include "ntdddisk.h"

//+ ------------------------------------------------------------------------------------------

typedef struct {
	PDEVICE_OBJECT	FileSystem;
	ULONG					LogicalDrive;
	ULONG					ContextFlags;  // see hookspec.h _CONTEXT_FLAG_???
} FSHOOK_EXTENSION, *PFSHOOK_EXTENSION;

#include "..\defs.h"
//#include "stdio.h"
_CRTIMP int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
_CRTIMP int __cdecl sprintf(char *, const char *, ...);

#include "stdarg.h"

#define DBG_PREFIX "HOOK"

#include "glbenvir.h"

BOOLEAN DrvRequestHook(ULONG HookID);
BOOLEAN DrvRequestUnhook(ULONG HookID);

BOOLEAN RegHook(VOID);
BOOLEAN RegUnHook(VOID);


NTSTATUS ReloadSettings(ULONG SettingsMask);

PDRIVER_OBJECT	HookerDriverObject = NULL;
PDEVICE_OBJECT	HookerDeviceObject = NULL;

PDEVICE_OBJECT	HookerDeviceObjectAlternate = NULL;

UNICODE_STRING		usDriverImagePath;

#include "../osspec.h"
#include "../debug.h"

#include "../hook\avpgcom.h"
#include "../Hook\HookSpec.h"

#include "../nt/specfunc.h"

#include "fidbox.h"

#include "../client.h"
#include "../osspec.h"
#include "../kldefs.h"
#include "../klsecur.h"
#include "../InvThread.h"
#include "../namecache.h"
#include "../evqueue.h"
#include "../filter.h"
#include "../TSPv2.h"
#include "SysIO.h"
#include "useklin.h"

#include "runproc.h"

#define AVPGNAME_ALTERNATE_W		L"AVPG"

WCHAR deviceNameBuffer[]  = L"\\Device\\"AVPGNAME_W;
WCHAR deviceNameBufferAlternate[]  = L"\\Device\\"AVPGNAME_ALTERNATE_W;

WCHAR deviceLinkBuffer[] = L"\\DosDevices\\"AVPGNAME_W;
WCHAR deviceLinkBufferAlternate[]=L"\\DosDevices\\"AVPGNAME_ALTERNATE_W;

//------------------ Declaration for INIT section -----------------------------------------------------------------
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath);

NTSTATUS CommonInit(VOID);
NTSTATUS StartLogging(VOID);

// located in "INIT" section, look for #pragma alloc_text("INIT",...)
#pragma alloc_text("INIT",DriverEntry,GetNativeBase,GetNativeAddresses,CommonInit,SetSystemNotifiers, PatchSwapC)
//#pragma alloc_text("INIT",FilterEvent, IsNeedFilterEvent, IsNeedFilterEventEx, FilterTransmit, LoadFilters, SaveFilters, MatchWithPatternW)
#pragma alloc_text("INIT",TSPInit,InitImpersonate,InitSysCalls,GetAddrOfShadowTable,GlobalCacheInit,HookSetProcessAffinity)
#pragma alloc_text("INIT",GlobalInvThreadInit,GlobalFiltersInit,GlobalClientsInit,StartAWDog,StartLogging,TSPIsAllowed,R3HookInit)


//-----------------------------------------------------------------------------------------------------------------
#include "..\ntcommon.cpp"
#include "..\common.c"
#include "..\r3.h"

/************************************* Registry ********************************************************/
#include "registry.c"

/******************************** FS Dispatch routines ***************************************************/
#include "fileio.c"

/******************************** FAST I/O routines ****************************************************/
#include "fastio.c"

/******************************** DiskDev Dispatch routines ***************************************************/
#include "diskio.c"

/******************************** Low level disk IO ***************************************************/
#include "LLDiskIO.h"

/******************************** FireWall routines ***************************************************/
#include "firewall.c"

/************************************* Specials *********************************************************/

#include "specfunc.h"

//+ ------------------------------------------------------------------------------------------


BOOLEAN
DrvRequestHook(ULONG HookID)
{
	DbPrint(DC_SYS,DL_NOTIFY,("----------------------- DrvRequestHook - HookID 0x%x -----------------------\n", HookID));
	switch(HookID)
	{
	case FLTTYPE_REGS:
		return RegHook();
	case FLTTYPE_DISK:
		return DiskHook();
	case FLTTYPE_NFIOR:
	case FLTTYPE_FIOR:
		//return FIOHook();
		return TRUE;
	case FLTTYPE_R3:
		if (R3Hooked)
		{
			DbPrint(DC_R3,DL_IMPORTANT, ("R3HookUsed\n"));
			SetR3HookUsed(TRUE);
			return TRUE;//R3Hook();
		}
		return FALSE;
	case FLTTYPE_PID:
		return PIDStart();
	case FLTTYPE_IDS:
		return IDSStart();
	case FLTTYPE_KLPF:
		return KLPFStart();
	case FLTTYPE_SYSTEM:
		return TRUE;
	case FLTTYPE_MCHECK:
		return KLMCStart();
	case FLTTYPE_MINIKAV:
		return MiniStart();
	case FLTTYPE_FLT:
	case FLTTYPE_CKAH_IDS:
		return TRUE;
	case FLTTYPE_KLICK:
		return KlickStart();
	case FLTTYPE_KLIN:
		return KlinStart();
	default:
		DbPrint(DC_SYS,DL_WARNING,("DrvRequestHook - Unknown HookID 0x%x\n", HookID));
	}
	return FALSE;
}

BOOLEAN
DrvRequestUnhook(ULONG HookID)
{
	DbPrint(DC_SYS,DL_NOTIFY,("----------------------- DrvRequestUnHook - HookID 0x%x -----------------------\n", HookID));
	switch(HookID)
	{
	case FLTTYPE_REGS:
		return RegUnHook();
	case FLTTYPE_DISK:
		return DiskUnHook();
	case FLTTYPE_NFIOR:
	case FLTTYPE_FIOR:
		//return FIOUnHook();
		return TRUE;
	case FLTTYPE_R3:
		DbPrint(DC_R3,DL_IMPORTANT, ("R3Hook is not used\n"));
		SetR3HookUsed(FALSE);
		return TRUE;//R3UnHook();
	case FLTTYPE_PID:
		return PIDStop();
	case FLTTYPE_IDS:
		return IDSStop();
	case FLTTYPE_KLPF:
		return KLPFStop();
	case FLTTYPE_SYSTEM:
		return TRUE;
	case FLTTYPE_MCHECK:
		return KLMCStop();
	case FLTTYPE_MINIKAV:
		return MiniStop();
	case FLTTYPE_FLT:
	case FLTTYPE_CKAH_IDS:
		return TRUE;
	case FLTTYPE_KLICK:
		return KlickStop();
	case FLTTYPE_KLIN:
		return KlinStop();
	default:
		DbPrint(DC_SYS,DL_WARNING,("DrvRequestUnhook - Unknown HookID 0x%x\n", HookID));
	}
	return FALSE;
}

VOID
UnHookAll(VOID)
{
	RegUnHook();
	DiskUnHook();
	FIOUnHook();
	PIDStop();
	IDSStop();
	KLPFStop();
}

//+ ------------------------------------------------------------------------------------------
#define IRP_MJ_POWER                    0x16

NTSTATUS
FilterDispatchPower(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NTSTATUS    ntStatus;

	if((DeviceObject == HookerDeviceObject) || (DeviceObject == HookerDeviceObjectAlternate))
	{
		DbgBreakPoint();

		Irp->IoStatus.Status      = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;

		ntStatus = Irp->IoStatus.Status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		
		return ntStatus;
	}
	else
	{
  		PFSHOOK_EXTENSION hookExt = (PFSHOOK_EXTENSION)DeviceObject->DeviceExtension;

		DbgBreakPoint();

		if (_pfPoStartNextPowerIrp && _pfPoCallDriver)
		{
			_pfPoStartNextPowerIrp(Irp);
			/*ntStatus = Irp->IoStatus.Status;*/
			IoSkipCurrentIrpStackLocation(Irp);
			ntStatus = _pfPoCallDriver(hookExt->FileSystem, Irp);
		}
		else
		{
			ntStatus = FSDevDispatch(DeviceObject, Irp);
		}
	}

    return ntStatus;
}

/******************************** MyDev Dispatch routines ***************************************************/
NTSTATUS
MyDevDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
	NTSTATUS					ntStatus;
	PIO_STACK_LOCATION	irpStack			= IoGetCurrentIrpStackLocation(Irp);
	PVOID						InBuffer;
	ULONG						InBufferLen;		
	PVOID						OutBuffer;			
	ULONG						OutBufferLen;	
	ULONG						ioControlCode;	
	//Проверить "подпись"!
	Irp->IoStatus.Status      = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	
	switch (irpStack->MajorFunction)
	{
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
		DbPrint(DC_SYS,DL_NOTIFY, ("IRP_MJ_CLOSE. gActiveClientCounter=%d\n", gActiveClientCounter));
		{
			ULONG ThreadID = (ULONG)PsGetCurrentThreadId();
			HANDLE ProcessID = PsGetCurrentProcessId();
			NameCacheCleanupByProc(ProcessID);
		}
		FreeUnusedClients();
		break;
	case IRP_MJ_SHUTDOWN:
		DbPrint(DC_SYS,DL_NOTIFY, ("IRP_MJ_SHUTDOWN\n"));
		UnHookAll();
		UnloadInProgress = TRUE;
		FidBox_GlobalDone();
		
		//! нафига чистить если ещё используем в completition and other delayed functions?
		//! CommonDone();

		DbPrint(DC_SYS,DL_IMPORTANT, ("IRP_MJ_SHUTDOWN proceed\n"));
		break;
	case IRP_MJ_DEVICE_CONTROL:
		DbPrint(DC_SYS,DL_SPAM, ("IRP_MJ_DEVICE_CONTROL -- very strange, maybe here is intruder?\n"));
		InBuffer			= Irp->AssociatedIrp.SystemBuffer;
		InBufferLen		= irpStack->Parameters.DeviceIoControl.InputBufferLength;
		OutBuffer		= Irp->AssociatedIrp.SystemBuffer;
		OutBufferLen	= irpStack->Parameters.DeviceIoControl.OutputBufferLength;
		ioControlCode	= irpStack->Parameters.DeviceIoControl.IoControlCode;
		if(Irp->MdlAddress)
		{
			if (_pfMmMapLockedPagesSpecifyCache)
				OutBuffer = __MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
			else
				OutBuffer = __MmGetSystemAddressForMdl(Irp->MdlAddress);
		}
		if (DeviceObject == HookerDeviceObject)
		{
			BOOLEAN bCheck = Irp->RequestorMode == KernelMode ? FALSE : TRUE;
			HOOKDeviceControl(InBuffer,InBufferLen,OutBuffer,OutBufferLen,
				ioControlCode,&Irp->IoStatus.Information,&Irp->IoStatus.Status, TRUE, bCheck);
		}
		else
		{
			HFIODeviceControl(irpStack->FileObject,TRUE,InBuffer,InBufferLen, OutBuffer,OutBufferLen,
				ioControlCode, &Irp->IoStatus,DeviceObject);
		}
		break;
	default:
		//DbPrint(DC_SYS,DL_WARNING, ("Unprocessed IRP_MJ_???????\n"));
		Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
		break;
	}
	ntStatus = Irp->IoStatus.Status;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	
	return ntStatus;
}

NTSTATUS
HookDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
	NTSTATUS ret;
	BOOLEAN bDispatched = FALSE;
	if(DeviceObject == HookerDeviceObject)
		ret = MyDevDispatch(DeviceObject, Irp);
	else
	{
		if ((HookerDeviceObjectAlternate != NULL) && (DeviceObject == HookerDeviceObjectAlternate))
			ret = MyDevDispatch(DeviceObject, Irp);
		else
			ret = FSDevDispatch(DeviceObject, Irp);
	}
	
	return ret;
}

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	
	UNICODE_STRING deviceNameUnicodeString;
	UNICODE_STRING deviceLinkUnicodeString;
	
	UNICODE_STRING deviceNameUnicodeStringAlternate;
	UNICODE_STRING deviceLinkUnicodeStringAlternate;
	
	UNICODE_STRING parameters;
	ULONG i;
	RTL_QUERY_REGISTRY_TABLE paramTable[2];
	
	DbPrint(DC_SYS,DL_IMPORTANT,("entering DriverEntry on NtBuildNumber %d\n",*NtBuildNumber));

	if (!InitGlobalEnvironment())
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("InitGlobalEnvironment failed\n"));
		DbgBreakPoint();
		return ntStatus;
	}

	if (!RecognizeFuncs((ULONG) DriverObject))
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("RecognizeFuncs failed\n"));
		DbgBreakPoint();
		return ntStatus;
	}

	// Init commons NTdata	
	if(!GetNativeAddresses())
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR,("GetNativeAddresses failed\n"));
		DbgBreakPoint();
		return ntStatus;
	}

	SpecialFileProcessing();

	GetThreadIdOffsetInETHREAD();

	// Fill Driver fields
	HookerDriverObject=DriverObject;
	if(*NtBuildNumber<1381)
		FastIOHook.SizeOfFastIoDispatch = (ULONG)&FastIOHook.FastIoQueryNetworkOpenInfo-(ULONG)&FastIOHook;
	
	for(i=0;i<=IRP_MJ_MAXIMUM_FUNCTION;i++)
		DriverObject->MajorFunction[i] = HookDispatch;

	DriverObject->MajorFunction[IRP_MJ_POWER] = FilterDispatchPower;

	DriverObject->FastIoDispatch=&FastIOHook;
	RtlInitUnicodeString (&deviceLinkUnicodeString,deviceLinkBuffer);
	RtlInitUnicodeString (&deviceLinkUnicodeStringAlternate, deviceLinkBufferAlternate);

	
	// Prepare RegParams str
	RtlInitUnicodeString(&parameters,L"\\Parameters");
	RegParams.Length=0;
	RegParams.MaximumLength=RegistryPath->Length+parameters.Length+sizeof(WCHAR); 
	if(!(RegParams.Buffer=ExAllocatePoolWithTag(PagedPool,RegParams.MaximumLength,'-SeB')))
	{ 
		ntStatus=STATUS_INSUFFICIENT_RESOURCES;
		goto done_DriverEntry;
	}
	RtlZeroMemory(RegParams.Buffer,RegParams.MaximumLength); 
	RtlAppendUnicodeStringToString(&RegParams, RegistryPath);
	RtlAppendUnicodeStringToString(&RegParams, &parameters);
	DbPrint(DC_SYS,DL_INFO,("RegParams=%S\n",RegParams.Buffer));
	
	// Prepare logFileName str
	usDriverImagePath.Length=0;
	usDriverImagePath.MaximumLength=0;
	usDriverImagePath.Buffer=NULL;
	RtlZeroMemory(&paramTable[0], sizeof(paramTable));
	paramTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_NOEXPAND;
	paramTable[0].Name = L"ImagePath";
	paramTable[0].EntryContext = &usDriverImagePath;
	paramTable[0].DefaultType = REG_EXPAND_SZ;
	paramTable[0].DefaultData = L"\\SystemRoot\\System32\\drivers\\";
	paramTable[0].DefaultLength = 0;
	ntStatus=RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL, RegistryPath->Buffer, &paramTable[0], 
		NULL, NULL);
	
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("RtlQueryRegistryValues failed. ntStatus=%x\n",ntStatus));
		goto done_DriverEntry;
	}
	
	for(i = wcslen(usDriverImagePath.Buffer);i!=0;i--)
	{
		if(usDriverImagePath.Buffer[i]=='\\')
			break;
	}
	usDriverImagePath.Buffer[i+1]='\0';
	usDriverImagePath.Length=(USHORT)i;
	DbPrint(DC_SYS,DL_INFO, ("DriverImagePath=%S status=%x\n",usDriverImagePath.Buffer,ntStatus));
	
	// Create AVPG device and SymbolycLink
	RtlInitUnicodeString (&deviceNameUnicodeString,deviceNameBuffer);
	ntStatus=IoCreateDevice(DriverObject,0,&deviceNameUnicodeString, _KLG_HOOK, 0,FALSE,&HookerDeviceObject);
	if(!NT_SUCCESS(ntStatus)) {
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("IoCreateDevice failed\n"));
		goto done_DriverEntry;
	}

	DbPrint(DC_SYS,DL_INFO,("%S created\n",deviceNameUnicodeString.Buffer));
	ntStatus=IoCreateSymbolicLink (&deviceLinkUnicodeString,&deviceNameUnicodeString);
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR,("IoCreateSymbolicLink failed\n"));
		goto done_DriverEntry;
	}
	DbPrint(DC_SYS,DL_INFO,("%S linked\n",deviceLinkUnicodeString.Buffer));
	
	RtlInitUnicodeString (&deviceNameUnicodeStringAlternate, deviceNameBufferAlternate);
	
	ntStatus = CommonInit();
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR,("CommonInit failed\n"));
		goto done_DriverEntry;
	}
	else
		DbPrint(DC_SYS,DL_INFO,("CommonInit ok\n"));

	if(!R3HookInit())
		DbPrint(DC_SYS,DL_ERROR, ("R3HookInit fail\n"));

	// Register this driver for watching file systems coming and going.
	if (*NtBuildNumber>1381)
	{
		ntStatus = IoRegisterFsRegistrationChange(DriverObject, FsNotification);
		if(!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_FILE,DL_ERROR,("Error registering FS change notification, status=%x\n",ntStatus));
		}
	}
	
	if (TSPInitedOk)
	{
		ntStatus = IoCreateDevice(DriverObject,0,&deviceNameUnicodeStringAlternate, _KLG_HOOK, 0,FALSE, &HookerDeviceObjectAlternate);
		if(!NT_SUCCESS(ntStatus))
		{
			DbPrint(DC_SYS, DL_FATAL_ERROR, ("IoCreateDevice failed (Alternate)\n"));
		}
		else
		{
			DbPrint(DC_SYS,DL_INFO,("%S created\n",deviceNameUnicodeStringAlternate.Buffer));
			ntStatus=IoCreateSymbolicLink (&deviceLinkUnicodeStringAlternate, &deviceNameUnicodeStringAlternate);
			if(!NT_SUCCESS(ntStatus))
			{
				DbPrint(DC_SYS,DL_FATAL_ERROR,("IoCreateSymbolicLink failed (Alternate)\n"));
			}
			else
			{
				DbPrint(DC_SYS,DL_INFO,("%S linked\n", deviceLinkUnicodeStringAlternate.Buffer));
			}
		}
	}
	
	IoRegisterShutdownNotification(HookerDeviceObject);
	SetSystemNotifiers();
	//	MmPageEntireDriver(DriverEntry);
	HookerDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	if (HookerDeviceObjectAlternate != NULL)
		HookerDeviceObjectAlternate->Flags &= ~DO_DEVICE_INITIALIZING;
	
	DbPrint(DC_SYS,DL_IMPORTANT,("==========================================================================\n"));
	DbPrint(DC_SYS,DL_IMPORTANT,("driver start success\n"));

	LoadDriverFlags();
	
	DbPrint(DC_SYS,DL_IMPORTANT,("loading filters\n"));
	LoadFilters();	
	DbPrint(DC_SYS,DL_IMPORTANT,("end loading filters\n"));


	FioInit();
	FIOHook();

	StartAWDog();
	
	return STATUS_SUCCESS;
	
done_DriverEntry:
	DbPrint(DC_SYS,DL_IMPORTANT, ("KLIF: DriverEntry fail (NtStatus=%x)\n",ntStatus));
	
	IoDeleteSymbolicLink(&deviceLinkUnicodeString);
	if (HookerDeviceObject)
		IoDeleteDevice(HookerDeviceObject);
	if (HookerDeviceObjectAlternate != NULL)
		IoDeleteDevice(HookerDeviceObjectAlternate);

	StopAWDog();

	if(RegParams.Buffer)
		ExFreePool(RegParams.Buffer);

	return ntStatus;
}

NTSTATUS ReloadSettings(ULONG SettingsMask)
{
	LoadSputniks();
	return STATUS_SUCCESS;
}