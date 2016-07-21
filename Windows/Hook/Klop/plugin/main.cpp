#include "main.h"

#include "..\kl1\klload.h"
#include "..\g_dispatcher.h"
#include "..\hook\klim_api.h"
#include "kl_ioctl.h"
#include "CustomPacket.h"

extern "C" {
#include "klim_sync.h"
}

bool             SymLinkOld  = false;
PDEVICE_OBJECT   pMyDevObj    = NULL;

/*
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
}
*/

void
KlimCallback( PVOID Ctx )
{
    HANDLE Event = (HANDLE)Ctx;

    CKl_Device Klim(L"\\device\\klim6");

    if ( Klim.m_DevicePtr == NULL )
        Klim.m_DevicePtr = Klim.FindDeviceByName( L"\\device\\klim5" );

    if ( KL_SUCCESS == Klim.SendIoctl( KLIM_GET_TABLE, NULL, 0, &ext_KlimTable, sizeof ( PVOID ), NULL ) )
    {
        KLIM_REGISTER_CB InitBuffer;

        if  ( ext_KlimTable->Version < 3 )
            return;

        InitBuffer.InCallback.ProcessPkt = ProcessInPacket;
        InitBuffer.InCallback.Priority   = KLIM_PRIORITY_KLOP; 

        InitBuffer.OutCallback.ProcessPkt = ProcessOutPacket;
        InitBuffer.OutCallback.Priority   = KLIM_PRIORITY_KLOP; 

        Klim.SendIoctl( KLIM_REGISTER_CALLBACKS, &InitBuffer, sizeof ( InitBuffer ), NULL, 0, NULL );
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
            ClientPID = INVALID_HANDLE_VALUE;
            PCP_InterlockedDone();
            CP_Done();
        }
    }
}

extern "C"
NTSTATUS
DriverEntry(
				PDRIVER_OBJECT	DriverObject,
				PUNICODE_STRING	RegistryPath
)
{
	NTSTATUS					ntStatus;
	UNICODE_STRING				DeviceName, DeviceLinkName, DeviceLinkName2;	

	ULONG						i;

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

//	DriverObject->DriverUnload = MyUnload;

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

    PsSetCreateProcessNotifyRoutine(CreateProcessNotify, FALSE);
    
    g_KlimSyncEvent = KlimSyn_CreateEvent();

    if ( g_KlimSyncEvent )
    {
        KlinSyn_StartWaitThread( KlimCallback, g_KlimSyncEvent );
    }
	
    return ntStatus;
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

	if ( IrpSp->MajorFunction == IRP_MJ_DEVICE_CONTROL ||
         IrpSp->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL )
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


