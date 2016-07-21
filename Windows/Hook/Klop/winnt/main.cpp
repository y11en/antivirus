#include "main.h"
#include "..\..\kl1\klload.h"
#include "..\g_dispatcher.h"
#include "kl_registry.h"
#include "..\g_cp_proto.h"
#include "kl_hook.h"
#include "..\g_custom_packet.h"

extern REGISTER_PROTOCOL    rNdisRegisterProtocol;
extern OPEN_ADAPTER	        rNdisOpenAdapter;
extern DEREGISTER_PROTOCOL	rNdisDeregisterProtocol;
extern CLOSE_ADAPTER	    rNdisCloseAdapter;

#define MYDRV		    L"klop"
#define LINKNAME_STRING     L"\\DosDevices\\Global\\"MYDRV
#define LINKNAME_STRING2    L"\\DosDevices\\"MYDRV
#define NTDEVICE_STRING     L"\\Device\\"MYDRV

bool             SymLinkOld  = false;
PDEVICE_OBJECT   pMyDevObj    = NULL;

VOID
CreateProcessNotify (
    IN HANDLE  ParentId,
    IN HANDLE  ProcessId,
    IN BOOLEAN  Create
    );

void
HookNetwork();

NTSTATUS	
Dispatch(
                PDEVICE_OBJECT	DeviceObject,
                PIRP			Irp);


VOID	
MyUnload(PDRIVER_OBJECT	DriverObject)
{
	UNICODE_STRING	DeviceLinkUnicodeString;

	if ( SymLinkOld )
        RtlInitUnicodeString(&DeviceLinkUnicodeString, LINKNAME_STRING2);
    else
        RtlInitUnicodeString(&DeviceLinkUnicodeString, LINKNAME_STRING);

	IoDeleteSymbolicLink(&DeviceLinkUnicodeString);

    KlUnregisterDeviceObject( pMyDevObj, DriverObject );
    IoDeleteDevice ( pMyDevObj );

	PCP_InterlockedDone();
	CP_Done();

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify, TRUE);
}

extern "C"
NTSTATUS
DriverEntry(
				PDRIVER_OBJECT	DriverObject,
				PUNICODE_STRING	RegistryPath
)
{
	NTSTATUS					ntStatus;

	UNICODE_STRING				DeviceName, 
								DeviceLinkName, 
								DeviceLinkName2;	

	ULONG						i;	

	isWin98 = FALSE;

	RtlInitUnicodeString(&DeviceName,		NTDEVICE_STRING);
	RtlInitUnicodeString(&DeviceLinkName,	LINKNAME_STRING);
    RtlInitUnicodeString(&DeviceLinkName2,	LINKNAME_STRING2);	

	if ( !NT_SUCCESS( ntStatus = IoCreateDevice(
                        SafeDriverObject(DriverObject),
		                0,
		                &DeviceName,
		                FILE_DEVICE_UNKNOWN,
		                0,
		                FALSE,
		                &pMyDevObj)))
    {
        DbgPrint("Unable to create DevObj for %ls. err = %x\n", MYDRV, ntStatus );
        return STATUS_UNSUCCESSFUL;
    }    

    KlRegisterDeviceObject( pMyDevObj, DriverObject );

	for ( i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i )
	{
		DriverObject->MajorFunction[i]  =  Dispatch;
	}

	DriverObject->DriverUnload = NULL;//MyUnload;

	if ( !NT_SUCCESS(ntStatus = IoCreateSymbolicLink(&DeviceLinkName, &DeviceName)))
    {
        DbgPrint("Unable to create SymLink for %ls. err = %x\n", MYDRV, ntStatus );

        if ( !NT_SUCCESS( ntStatus = IoCreateSymbolicLink(&DeviceLinkName2, &DeviceName ) ) )
        {
            DbgPrint("Unable to create SymLink2 for %ls. err = %x\n", MYDRV, ntStatus );
            
            KlUnregisterDeviceObject( pMyDevObj, DriverObject );
            IoDeleteDevice( pMyDevObj );
            return STATUS_UNSUCCESSFUL;
        }
        
        SymLinkOld = true;
    }
	
	AllocateSpinLock( &g_PCP_Lock );
	
	HookNetwork();

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify, FALSE);

    return STATUS_SUCCESS;
}

NTSTATUS	
Dispatch(
		   IN PDEVICE_OBJECT	DeviceObject,
		   IN PIRP				Irp
)
{
    NTSTATUS			ntStatus	= STATUS_SUCCESS;
	PIO_STACK_LOCATION	IrpSp		= IoGetCurrentIrpStackLocation(Irp);
	PVOID				ioBuffer	= Irp->AssociatedIrp.SystemBuffer;
	ULONG               inBufLength = IrpSp->Parameters.DeviceIoControl.InputBufferLength;
    ULONG               outBufLength= IrpSp->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG               IOCTL_Code	= IrpSp->Parameters.DeviceIoControl.IoControlCode;

	if ( IrpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
	{
		
		switch ( KlopDispatch( IOCTL_Code,ioBuffer, inBufLength, ioBuffer, outBufLength, (ULONG*)&Irp->IoStatus.Information ) )
		{
		case KL_SUCCESS :
			ntStatus = STATUS_SUCCESS;
            break;
        case KL_BUFFER_TOO_SMALL:
            ntStatus = STATUS_BUFFER_TOO_SMALL;
            break;
		default:
			ntStatus = STATUS_UNSUCCESSFUL;
			break;
		}
		
	}


    Irp->IoStatus.Status = ntStatus;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
	return ntStatus;
}


void
HookNetwork()
{
	__asm {
		mov  ebx , cr0
		push ebx
		and  ebx , ~0x10000
		mov  cr0 , ebx
	}
	
	CKl_HookModule  hm("NDIS.SYS");        
	
	hm.HookFunction("NdisRegisterProtocol",     HNdisRegisterProtocol,   (PVOID*)&rNdisRegisterProtocol);
    hm.HookFunction("NdisDeregisterProtocol",   HNdisDeregisterProtocol, (PVOID*)&rNdisDeregisterProtocol);
    hm.HookFunction("NdisOpenAdapter",          HNdisOpenAdapter,        (PVOID*)&rNdisOpenAdapter);
    hm.HookFunction("NdisCloseAdapter",         HNdisCloseAdapter,       (PVOID*)&rNdisCloseAdapter);
	
	__asm {	
		pop   ebx 	
		mov   cr0 , ebx    
	}
}

VOID
CreateProcessNotify (
    IN HANDLE  ParentId,
    IN HANDLE  ProcessId,
    IN BOOLEAN  Create
    )
{
	if ( !Create )
	{
		if ( ProcessId == ClientPID )
		{
			CKl_FuncDebug dd("CreateProcessNotify");
			ClientPID = INVALID_HANDLE_VALUE;
			PCP_InterlockedDone();
			CP_Done();
		}
	}
}