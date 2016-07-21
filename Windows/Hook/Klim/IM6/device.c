/*++
 *
 * The file contains the routines to create a device and handle ioctls
 *
-- */

#include "precomp.h"

NDIS_STATUS
FilterRegisterDevice(
    VOID
    )
{
    NDIS_STATUS            Status = NDIS_STATUS_SUCCESS;
    UNICODE_STRING         DeviceName;
    UNICODE_STRING         DeviceLinkUnicodeString;
    PDRIVER_DISPATCH       DispatchTable[IRP_MJ_MAXIMUM_FUNCTION+1];
    NDIS_DEVICE_OBJECT_ATTRIBUTES   DeviceAttribute;
    PFILTER_DEVICE_EXTENSION        FilterDeviceExtension;
    PDRIVER_OBJECT                  DriverObject;
   
    DEBUGP(DL_TRACE, ("==>FilterRegisterDevice\n"));
   
    
    NdisZeroMemory(DispatchTable, (IRP_MJ_MAXIMUM_FUNCTION+1) * sizeof(PDRIVER_DISPATCH));
    
    DispatchTable[IRP_MJ_CREATE] = FilterDispatch;
    DispatchTable[IRP_MJ_CLEANUP] = FilterDispatch;
    DispatchTable[IRP_MJ_CLOSE] = FilterDispatch;
    DispatchTable[IRP_MJ_DEVICE_CONTROL] = FilterDeviceIoControl;
	DispatchTable[IRP_MJ_INTERNAL_DEVICE_CONTROL] = FilterDeviceIoControl;
    
    
    NdisInitUnicodeString(&DeviceName, NTDEVICE_STRING);
    NdisInitUnicodeString(&DeviceLinkUnicodeString, LINKNAME_STRING);
    
    //
    // Create a device object and register our dispatch handlers
    //
    NdisZeroMemory(&DeviceAttribute, sizeof(NDIS_DEVICE_OBJECT_ATTRIBUTES));
    
    DeviceAttribute.Header.Type = NDIS_OBJECT_TYPE_DEVICE_OBJECT_ATTRIBUTES;
    DeviceAttribute.Header.Revision = NDIS_DEVICE_OBJECT_ATTRIBUTES_REVISION_1;
    DeviceAttribute.Header.Size = sizeof(NDIS_DEVICE_OBJECT_ATTRIBUTES);
    
    DeviceAttribute.DeviceName = &DeviceName;
    DeviceAttribute.SymbolicName = &DeviceLinkUnicodeString;
    DeviceAttribute.MajorFunctions = &DispatchTable[0];
    DeviceAttribute.ExtensionSize = sizeof(FILTER_DEVICE_EXTENSION);
    
    Status = NdisRegisterDeviceEx(
                FilterDriverHandle,
                &DeviceAttribute,
                &DeviceObject,
                &NdisFilterDeviceHandle
                );
   
   
    if (Status == NDIS_STATUS_SUCCESS)
    {
        FilterDeviceExtension = NdisGetDeviceReservedExtension(DeviceObject);
   
        FilterDeviceExtension->Signature = 'FTDR';
        FilterDeviceExtension->Handle = FilterDriverHandle;

        //
        // Workaround NDIS bug
        //
        DriverObject = (PDRIVER_OBJECT)FilterDriverObject;
    }
              
        
    DEBUGP(DL_TRACE, ("<==PtRegisterDevice: %x\n", Status));
        
    return (Status);
        
}

VOID
FilterDeregisterDevice(
    IN VOID
    )

{
    if (NdisFilterDeviceHandle != NULL)
    {
        NdisDeregisterDeviceEx(NdisFilterDeviceHandle);
    }

    NdisFilterDeviceHandle = NULL;

}

NTSTATUS
FilterDispatch(
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp
    )
{
    PIO_STACK_LOCATION       IrpStack;
    NTSTATUS                 Status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(DeviceObject);

    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    
    switch (IrpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
            break;

        case IRP_MJ_CLEANUP:
            break;

        case IRP_MJ_CLOSE:
            break;

        default:
            break;
    }

    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Status;
}
                
NTSTATUS
FilterDeviceIoControl(
    IN PDEVICE_OBJECT        DeviceObject,
    IN PIRP                  Irp
    )
{
    PIO_STACK_LOCATION          IrpSp;
    NTSTATUS                    Status = STATUS_SUCCESS;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

	return KlimDispatcher( DeviceObject, Irp );
}



