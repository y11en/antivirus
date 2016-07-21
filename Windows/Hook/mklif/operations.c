#include "pch.h"

#include "..\hook\avpgcom.h"

#include "operations.tmh"


//
//  Assign text sections for each routine.
//

#ifdef  ALLOC_PRAGMA
    #pragma alloc_text(PAGE,     CreateControlDeviceObject)
    #pragma alloc_text(PAGE,     DeleteControlDeviceObject)
    #pragma alloc_text(PAGE,     MajorFunction)
    #pragma alloc_text(PAGE,     HandlePrivateOpen)
    #pragma alloc_text(PAGE,     HandlePrivateCleanup)
    #pragma alloc_text(PAGE,     HandlePrivateClose)
    #pragma alloc_text(PAGE,     HandlePrivateFsControl)
    #pragma alloc_text(PAGE,     FastIoCheckIfPossible)
    #pragma alloc_text(PAGE,     FastIoRead)
    #pragma alloc_text(PAGE,     FastIoWrite)
    #pragma alloc_text(PAGE,     FastIoQueryBasicInfo)
    #pragma alloc_text(PAGE,     FastIoQueryStandardInfo)
    #pragma alloc_text(PAGE,     FastIoLock)
    #pragma alloc_text(PAGE,     FastIoUnlockSingle)
    #pragma alloc_text(PAGE,     FastIoUnlockAll)
    #pragma alloc_text(PAGE,     FastIoUnlockAllByKey)
    #pragma alloc_text(PAGE,     FastIoDeviceControl)
    #pragma alloc_text(PAGE,     FastIoQueryNetworkOpenInfo)
    #pragma alloc_text(PAGE,     FastIoMdlRead)
    #pragma alloc_text(NONPAGED, FastIoMdlReadComplete)
    #pragma alloc_text(PAGE,     FastIoPrepareMdlWrite)
    #pragma alloc_text(NONPAGED, FastIoMdlWriteComplete)
    #pragma alloc_text(PAGE,     FastIoReadCompressed)
    #pragma alloc_text(PAGE,     FastIoWriteCompressed)
    #pragma alloc_text(NONPAGED, FastIoMdlReadCompleteCompressed)
    #pragma alloc_text(NONPAGED, FastIoMdlWriteCompleteCompressed)
    #pragma alloc_text(PAGE,     FastIoQueryOpen)
#endif

//
//  Fast IO dispatch routines
//

FAST_IO_DISPATCH FastIoDispatch =
{
    sizeof(FAST_IO_DISPATCH),
    FastIoCheckIfPossible,           //  CheckForFastIo
    FastIoRead,                      //  FastIoRead
    FastIoWrite,                     //  FastIoWrite
    FastIoQueryBasicInfo,            //  FastIoQueryBasicInfo
    FastIoQueryStandardInfo,         //  FastIoQueryStandardInfo
    FastIoLock,                      //  FastIoLock
    FastIoUnlockSingle,              //  FastIoUnlockSingle
    FastIoUnlockAll,                 //  FastIoUnlockAll
    FastIoUnlockAllByKey,            //  FastIoUnlockAllByKey
    FastIoDeviceControl,             //  FastIoDeviceControl
    NULL,                               //  AcquireFileForNtCreateSection
    NULL,                               //  ReleaseFileForNtCreateSection
    NULL,                               //  FastIoDetachDevice
    FastIoQueryNetworkOpenInfo,      //  FastIoQueryNetworkOpenInfo
    NULL,                               //  AcquireForModWrite
    FastIoMdlRead,                   //  MdlRead
    FastIoMdlReadComplete,           //  MdlReadComplete
    FastIoPrepareMdlWrite,           //  PrepareMdlWrite
    FastIoMdlWriteComplete,          //  MdlWriteComplete
    FastIoReadCompressed,            //  FastIoReadCompressed
    FastIoWriteCompressed,           //  FastIoWriteCompressed
    FastIoMdlReadCompleteCompressed, //  MdlReadCompleteCompressed
    FastIoMdlWriteCompleteCompressed, //  MdlWriteCompleteCompressed
    FastIoQueryOpen,                 //  FastIoQueryOpen
    NULL,                               //  ReleaseForModWrite
    NULL,                               //  AcquireForCcFlush
    NULL,                               //  ReleaseForCcFlush
};


NTSTATUS
CreateControlDeviceObject(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING pusDrvName
)
{
    NTSTATUS status;
    UNICODE_STRING nameString;
	UNICODE_STRING usDefDrvName;
	ULONG strSize;
	PWCHAR pstr;
    ULONG i;

	PAGED_CODE();
	
	if( !pusDrvName )
		return STATUS_INVALID_PARAMETER;

	RtlInitUnicodeString (&usDefDrvName, DEFAULT_DRV_NAME );

	if ( !RtlCompareUnicodeString(&usDefDrvName, pusDrvName, TRUE) )
	{
		strSize = sizeof (CONTROL_DEVICE_OBJECT_PREFIX) + 
				  sizeof (CONTROL_DEVICE_OBJECT_NAME) + sizeof(WCHAR);
		
		pstr = ExAllocatePoolWithTag( PagedPool, strSize, tag_operat1 );
		if (!pstr)
			return STATUS_NO_MEMORY;

		RtlZeroMemory ( pstr, strSize );
		RtlInitEmptyUnicodeString(&nameString,pstr, (USHORT)strSize);
		status = RtlAppendUnicodeToString ( &nameString, CONTROL_DEVICE_OBJECT_PREFIX );
		if (NT_SUCCESS(status))
			status = RtlAppendUnicodeToString ( &nameString, CONTROL_DEVICE_OBJECT_NAME );

		if (!NT_SUCCESS( status ))
		{
			ExFreePool( pstr );
			return status;
		}
	}
	else
	{
		strSize = sizeof (CONTROL_DEVICE_OBJECT_PREFIX) + 
			pusDrvName->Length + sizeof(WCHAR);

		pstr = ExAllocatePoolWithTag( PagedPool, strSize, tag_operat2 );
		if (!pstr)
			return STATUS_NO_MEMORY;

		memset ( pstr, 0, strSize );
		RtlInitEmptyUnicodeString(&nameString,pstr, (USHORT)strSize);
		
		status = RtlAppendUnicodeToString ( &nameString, CONTROL_DEVICE_OBJECT_PREFIX );
		if (NT_SUCCESS(status))
			status = RtlAppendUnicodeStringToString( &nameString, pusDrvName );

		if (!NT_SUCCESS(status))
		{
			ExFreePool( pstr );
			return status;
		}
	}

	status = IoCreateDevice(DriverObject,
		0,
		&nameString,
		FILE_DEVICE_DISK_FILE_SYSTEM,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&Globals.m_FilterControlDeviceObject
		);

	if (!NT_SUCCESS(status))
	{
		ExFreePool(nameString.Buffer);
		return status;
	}

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = MajorFunction;

	DriverObject->FastIoDispatch = &FastIoDispatch;

	{
		UNICODE_STRING LinkUnicodeString;
		//RtlInitUnicodeString(&LinkUnicodeString, LINK_DEVICE_OBJECT_NAME);
		
		if (!RtlCompareUnicodeString( &usDefDrvName, pusDrvName, TRUE ))
		{
			strSize = sizeof (LINK_DEVICE_OBJECT_PREFIX) +
				sizeof (LINK_DEVICE_OBJECT_NAME) + sizeof(WCHAR);

			pstr = ExAllocatePoolWithTag( PagedPool, strSize, tag_operat3 );
			if (!pstr)
			{
				ExFreePool(nameString.Buffer);
				return STATUS_NO_MEMORY;
			}

			RtlZeroMemory( pstr, strSize );
			RtlInitEmptyUnicodeString(&LinkUnicodeString,pstr, (USHORT)strSize);
			status = RtlAppendUnicodeToString ( &LinkUnicodeString, LINK_DEVICE_OBJECT_PREFIX );
			if (NT_SUCCESS(status))
				status = RtlAppendUnicodeToString ( &LinkUnicodeString, LINK_DEVICE_OBJECT_NAME );
			if (!NT_SUCCESS(status))
			{
				ExFreePool( pstr );
				ExFreePool(nameString.Buffer);
				return status;
			}
		}
		else
		{
			strSize = sizeof (LINK_DEVICE_OBJECT_PREFIX) +
				pusDrvName->Length + sizeof(WCHAR);

			pstr = ExAllocatePoolWithTag( NonPagedPool, strSize, tag_operat4 );
			if (!pstr)
			{
				ExFreePool( nameString.Buffer );
				return STATUS_NO_MEMORY;
			}

			memset( pstr, 0, strSize );
			RtlInitEmptyUnicodeString( &LinkUnicodeString, pstr, (USHORT)strSize );

			status = RtlAppendUnicodeToString ( &LinkUnicodeString, LINK_DEVICE_OBJECT_PREFIX );
			if (!NT_SUCCESS(status))
			{
				ExFreePool( pstr );
				ExFreePool(nameString.Buffer);
				return status;
			}

			status = RtlAppendUnicodeStringToString( &LinkUnicodeString, pusDrvName );
			if (!NT_SUCCESS(status))
			{
				ExFreePool( pstr );
				ExFreePool( nameString.Buffer );
				return status;
			}
		}

		status = IoCreateSymbolicLink( &LinkUnicodeString, &nameString );
		
		ExFreePool( nameString.Buffer );
		
		if(!NT_SUCCESS(status))
		{
			IoDeleteDevice(Globals.m_FilterControlDeviceObject);
			Globals.m_FilterControlDeviceObject = NULL;

			return status;
		}
		
		memcpy ( &Globals.m_ControlDeviceObjectSymLink, &LinkUnicodeString, sizeof( UNICODE_STRING ) );
	}
	
	IoRegisterShutdownNotification( Globals.m_FilterControlDeviceObject );

	return STATUS_SUCCESS;
}



VOID
DeleteControlDeviceObject(
    VOID
   )
{
	PAGED_CODE();

	IoUnregisterShutdownNotification( Globals.m_FilterControlDeviceObject );

	IoDeleteSymbolicLink(&Globals.m_ControlDeviceObjectSymLink);

    IoDeleteDevice( Globals.m_FilterControlDeviceObject );
	Globals.m_FilterControlDeviceObject = NULL;
	
	ExFreePool( Globals.m_ControlDeviceObjectSymLink.Buffer );
	Globals.m_ControlDeviceObjectSymLink.Buffer=NULL;
	Globals.m_ControlDeviceObjectSymLink.Length = 0;
	Globals.m_ControlDeviceObjectSymLink.MaximumLength = 0;
}


NTSTATUS
MajorFunction(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
   )
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpSp;

    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    status = STATUS_SUCCESS;
    
    irpSp = IoGetCurrentIrpStackLocation(Irp);

    switch (irpSp->MajorFunction)
	{
        case IRP_MJ_CREATE:
        {
            status = HandlePrivateOpen(Irp);

            Irp->IoStatus.Status = status;

            if(NT_SUCCESS(status))
                Irp->IoStatus.Information = FILE_OPENED;
            else
                Irp->IoStatus.Information = 0;

            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            
			break;
        }

        case IRP_MJ_CLOSE:
        {
            HandlePrivateClose(Irp);

            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;

            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            break;
        }

		case IRP_MJ_FILE_SYSTEM_CONTROL:
        {
            ULONG Operation;
            ULONG OutputBufferLength;
            ULONG InputBufferLength;
            PVOID InputBuffer;
            PVOID OutputBuffer;

            Operation = irpSp->Parameters.FileSystemControl.FsControlCode;
            InputBufferLength = irpSp->Parameters.FileSystemControl.InputBufferLength;
            OutputBufferLength = irpSp->Parameters.FileSystemControl.OutputBufferLength;        

            InputBuffer = Irp->AssociatedIrp.SystemBuffer;
            OutputBuffer = Irp->AssociatedIrp.SystemBuffer;

            //  The caller will update the IO status block 
            status = HandlePrivateFsControl (DeviceObject,
                                                Operation,
                                                InputBuffer,
                                                InputBufferLength,
                                                OutputBuffer,
                                                OutputBufferLength,
                                                &Irp->IoStatus,
                                                Irp);

			IoCompleteRequest(Irp, IO_NO_INCREMENT);

            break;
        }

		case IRP_MJ_DEVICE_CONTROL:
			{
				ULONG Operation;
				ULONG OutputBufferLength;
				ULONG InputBufferLength;
				PVOID InputBuffer;
				PVOID OutputBuffer;

				Operation = irpSp->Parameters.DeviceIoControl.IoControlCode;
				InputBufferLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
				OutputBufferLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;

				InputBuffer = Irp->AssociatedIrp.SystemBuffer;
				OutputBuffer = Irp->AssociatedIrp.SystemBuffer;

				/*if(Irp->MdlAddress)
				{
					if (_pfMmMapLockedPagesSpecifyCache)
						OutBuffer = __MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
					else
						OutBuffer = __MmGetSystemAddressForMdl(Irp->MdlAddress);
				}*/
				
				status = HandlePrivateFsControl(
					DeviceObject,
					Operation,
					InputBuffer,
					InputBufferLength,
					OutputBuffer,
					OutputBufferLength,
					&Irp->IoStatus,
					Irp
					);
			}

			IoCompleteRequest(Irp, IO_NO_INCREMENT);
		break;


        case IRP_MJ_CLEANUP:
        {
            HandlePrivateCleanup(Irp);

            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;

            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            break;
        }

		case IRP_MJ_SHUTDOWN:
		{
			GlobalFidBoxDone( FALSE );

            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;

            IoCompleteRequest(Irp, IO_NO_INCREMENT);
		}
		break;
    
        default:
        {
            // unsupported!
       
            Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;

            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            status = STATUS_INVALID_DEVICE_REQUEST;

			_dbg_break_;
        }
    }

    return status;
}


NTSTATUS 
HandlePrivateOpen(
    IN PIRP Irp
   )
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Irp);

    PAGED_CODE();

    return status;
}

NTSTATUS 
HandlePrivateCleanup(
    IN PIRP Irp
   )
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Irp);

    PAGED_CODE();

    return status;
}

NTSTATUS 
HandlePrivateClose(
    IN PIRP Irp
   )
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Irp);

    PAGED_CODE();

    return status;
}

NTSTATUS
HandlePrivateFsControl (
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG IoControlCode,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PIRP Irp
   )
/*++

Routine Description:

    This routine is invoked whenever an I/O Request Packet (IRP) w/a major
    function code of IRP_MJ_FILE_SYSTEM_CONTROL is encountered for the KLIF.

Arguments:

    DeviceObject        - Pointer to the device object for this driver.
    IoControlCode       - Control code for this IOCTL
    InputBuffer         - Input buffer
    InputBufferLength   - Input buffer length
    OutputBuffer        - Output buffer
    OutputBufferLength  - Output buffer length
    IoStatus            - IO status block for this request
    Irp - Pointer to the request packet representing the I/O request.

Return Value:

    The function value is the status of the operation.

--*/
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	KPROCESSOR_MODE PreviousMode = UserMode;
	ULONG Func = IoControlCode >> 2;

    UNREFERENCED_PARAMETER( DeviceObject );
	UNREFERENCED_PARAMETER( IoControlCode );
	UNREFERENCED_PARAMETER( InputBuffer );
	UNREFERENCED_PARAMETER( InputBufferLength );
	UNREFERENCED_PARAMETER( OutputBuffer );
	UNREFERENCED_PARAMETER( OutputBufferLength );
	UNREFERENCED_PARAMETER( Irp  );

    PAGED_CODE();

	IoStatus->Information = 0;

	if (Irp)
		PreviousMode = Irp->RequestorMode;
	else
		PreviousMode = ExGetPreviousMode();

	if (UserMode == PreviousMode)
	{
		_dbg_break_;

		IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	switch(IoControlCode)
	{
	case IOCTLHOOK_GetVersion:
		status = STATUS_BUFFER_TOO_SMALL;

		if(OutputBuffer && (OutputBufferLength >= sizeof(ULONG)))
		{
			*(ULONG*)OutputBuffer = HOOK_INTERFACE_NUM;
			
			IoStatus->Information = sizeof(ULONG);
			status = STATUS_SUCCESS;
		}
		break;

	case IOCTLHOOK_GetDriverInfo:
		status = STATUS_BUFFER_TOO_SMALL;
		if(OutputBuffer && (OutputBufferLength == sizeof(ADDITIONALSTATE)))
		{
			PADDITIONALSTATE pAddState = (PADDITIONALSTATE) OutputBuffer;
	
			pAddState->DeadlockCount = 0;
			pAddState->FilterEventEntersCount = Globals.m_FilterEventEntersCount;
			pAddState->ReplyWaiterEnters = Globals.m_ReplyWaiterEnters;
			pAddState->DrvFlags = _DRV_FLAG_MINIFILTER;
			pAddState->DrvFlags |= _DRV_FLAG_SUPPORT_NET_SAFESTREAMS;
			pAddState->DirectSysCallStartId = 0;
			pAddState->DrvFlags |= Globals.m_DrvFlags;

			IoStatus->Information = sizeof(ADDITIONALSTATE);
			status = STATUS_SUCCESS;
		}
		break;
	
	case IOCTLHOOK_FilterEvent:
		if(InputBuffer
			&& OutputBuffer
			&& InputBufferLength >= sizeof(FILTER_EVENT_PARAM)
			&& OutputBufferLength == sizeof(VERDICT))
		{
			if (CheckEventParams( (PFILTER_EVENT_PARAM) InputBuffer, InputBufferLength) )
			{
				*(VERDICT*)OutputBuffer = FilterEvent( (PFILTER_EVENT_PARAM) InputBuffer, NULL );
				IoStatus->Information = sizeof(VERDICT);
				status = STATUS_SUCCESS;
			}
			else
			{
				status = STATUS_INVALID_PARAMETER;
				IoStatus->Information = 0;
			}
		}
		break;

	case IOCTLHOOK_External_Drv_Request:
		status = STATUS_INVALID_PARAMETER;
		if(InputBuffer && InputBufferLength >= sizeof(EXTERNAL_DRV_REQUEST))
		{
			PEXTERNAL_DRV_REQUEST pIn = (PEXTERNAL_DRV_REQUEST) InputBuffer;
			ULONG RetBytes;
			status = DoExternalDrvRequest(pIn, InputBufferLength, OutputBuffer, OutputBufferLength, &RetBytes);
			IoStatus->Information = RetBytes;
			if (IoStatus->Information > OutputBufferLength)
			{
				status = STATUS_INVALID_BLOCK_LENGTH;
				IoStatus->Information = 0;
			}
		}
		break;

	case IOCTLHOOK_InvisibleThreadOperations:
		if(InputBuffer && OutputBuffer && 
			InputBufferLength == sizeof(INVISIBLE_TRANSMIT) && OutputBufferLength == sizeof(INVISIBLE_TRANSMIT))
		{
			status = InvisibleTransmit((PINVISIBLE_TRANSMIT)InputBuffer, (PINVISIBLE_TRANSMIT)OutputBuffer);
			if (NT_SUCCESS(status))
				IoStatus->Information = sizeof(INVISIBLE_TRANSMIT);
		}
		break;

	case IOCTLHOOK_RESTART_FWDRV:
		DrvRestartHook();
		status = STATUS_SUCCESS;
		IoStatus->Information = 0;
		break;

	default:
		_dbg_break_;
		break;
	}

	IoStatus->Status = status;

    return status;
}



/////////////////////////////////////////////////////////////////////////////
//
//                      FastIO Handling routines
//
/////////////////////////////////////////////////////////////////////////////



BOOLEAN
FastIoCheckIfPossible (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN BOOLEAN CheckForReadOperation,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for checking to see
    whether fast I/O is possible for this file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be operated on.

    FileOffset - Byte offset in the file for the operation.

    Length - Length of the operation to be performed.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    LockKey - Provides the caller's key for file locks.

    CheckForReadOperation - Indicates whether the caller is checking for a
        read (TRUE) or a write operation.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(CheckForReadOperation);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}


BOOLEAN
FastIoRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for reading from a
    file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be read.

    FileOffset - Byte offset in the file of the read.

    Length - Length of the read operation to be performed.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    LockKey - Provides the caller's key for file locks.

    Buffer - Pointer to the caller's buffer to receive the data read.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}



BOOLEAN
FastIoWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for writing to a
    file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be written.

    FileOffset - Byte offset in the file of the write operation.

    Length - Length of the write operation to be performed.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    LockKey - Provides the caller's key for file locks.

    Buffer - Pointer to the caller's buffer that contains the data to be
        written.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}

BOOLEAN
FastIoQueryBasicInfo (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_BASIC_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for querying basic
    information about the file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be queried.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    Buffer - Pointer to the caller's buffer to receive the information about
        the file.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}




BOOLEAN
FastIoQueryStandardInfo (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_STANDARD_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for querying standard
    information about the file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be queried.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    Buffer - Pointer to the caller's buffer to receive the information about
        the file.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}


BOOLEAN
FastIoLock (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    BOOLEAN FailImmediately,
    BOOLEAN ExclusiveLock,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for locking a byte
    range within a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be locked.

    FileOffset - Starting byte offset from the base of the file to be locked.

    Length - Length of the byte range to be locked.

    ProcessId - ID of the process requesting the file lock.

    Key - Lock key to associate with the file lock.

    FailImmediately - Indicates whether or not the lock request is to fail
        if it cannot be immediately be granted.

    ExclusiveLock - Indicates whether the lock to be taken is exclusive (TRUE)
        or shared.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(Key);
    UNREFERENCED_PARAMETER(FailImmediately);
    UNREFERENCED_PARAMETER(ExclusiveLock);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}


BOOLEAN
FastIoUnlockSingle (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for unlocking a byte
    range within a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be unlocked.

    FileOffset - Starting byte offset from the base of the file to be
        unlocked.

    Length - Length of the byte range to be unlocked.

    ProcessId - ID of the process requesting the unlock operation.

    Key - Lock key associated with the file lock.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(Key);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}



BOOLEAN
FastIoUnlockAll (
    IN PFILE_OBJECT FileObject,
    PEPROCESS ProcessId,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for unlocking all
    locks within a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be unlocked.

    ProcessId - ID of the process requesting the unlock operation.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}


BOOLEAN
FastIoUnlockAllByKey (
    IN PFILE_OBJECT FileObject,
    PVOID ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for unlocking all
    locks within a file based on a specified key.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be unlocked.

    ProcessId - ID of the process requesting the unlock operation.

    Key - Lock key associated with the locks on the file to be released.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(Key);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}



BOOLEAN
FastIoDeviceControl (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN ULONG IoControlCode,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for device I/O control
    operations on a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object representing the device to be
        serviced.

    Wait - Indicates whether or not the caller is willing to wait if the
        appropriate locks, etc. cannot be acquired

    InputBuffer - Optional pointer to a buffer to be passed into the driver.

    InputBufferLength - Length of the optional InputBuffer, if one was
        specified.

    OutputBuffer - Optional pointer to a buffer to receive data from the
        driver.

    OutputBufferLength - Length of the optional OutputBuffer, if one was
        specified.

    IoControlCode - I/O control code indicating the operation to be performed
        on the device.

    IoStatus - Pointer to a variable to receive the I/O status of the
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(Wait);

    //
    //  The caller will update the IO status block 
    //

	{
		PMDL pmdl = NULL;

		if (OutputBuffer && OutputBufferLength)
		{
			pmdl = IoAllocateMdl(OutputBuffer, OutputBufferLength, FALSE, FALSE, NULL);
			if (!pmdl)
			{
				_dbg_break_;
				IoStatus->Information = 0;
				IoStatus->Status = STATUS_UNSUCCESSFUL;

				return TRUE;
			}

			__try
			{
				MmProbeAndLockPages(pmdl, UserMode, IoWriteAccess);
			}
			__except(CheckException())
			{
				IoFreeMdl(pmdl);

				_dbg_break_;
				IoStatus->Information = 0;
				IoStatus->Status = STATUS_UNSUCCESSFUL;

				return TRUE;

			};
		}

	    HandlePrivateFsControl (DeviceObject,
                                IoControlCode,
                                InputBuffer,
                                InputBufferLength,
                                OutputBuffer,
                                OutputBufferLength,
                                IoStatus,
                                NULL);

		if (pmdl)
		{
			MmUnlockPages(pmdl);
			IoFreeMdl(pmdl);
		}
	}

    return TRUE;
}

BOOLEAN
FastIoQueryNetworkOpenInfo (
    IN PFILE_OBJECT FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_NETWORK_OPEN_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for querying network
    information about a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object to be queried.

    Wait - Indicates whether or not the caller can handle the file system
        having to wait and tie up the current thread.

    Buffer - Pointer to a buffer to receive the network information about the
        file.

    IoStatus - Pointer to a variable to receive the final status of the query
        operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}


BOOLEAN
FastIoMdlRead (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for reading a file
    using MDLs as buffers.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that is to be read.

    FileOffset - Supplies the offset into the file to begin the read operation.

    Length - Specifies the number of bytes to be read from the file.

    LockKey - The key to be used in byte range lock checks.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data read.

    IoStatus - Variable to receive the final status of the read operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //


    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}

BOOLEAN
FastIoMdlReadComplete (
    IN PFILE_OBJECT FileObject,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing an
    MDL read operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the MdlRead function is supported by the underlying file system, and
    therefore this function will also be supported, but this is not assumed
    by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the MDL read upon.

    MdlChain - Pointer to the MDL chain used to perform the read operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/

{
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, return not supported
    //

    return FALSE;
}

BOOLEAN
FastIoPrepareMdlWrite (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for preparing for an
    MDL write operation.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that will be written.

    FileOffset - Supplies the offset into the file to begin the write operation.

    Length - Specifies the number of bytes to be write to the file.

    LockKey - The key to be used in byte range lock checks.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data written.

    IoStatus - Variable to receive the final status of the write operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}




BOOLEAN
FastIoMdlWriteComplete (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing an
    MDL write operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the PrepareMdlWrite function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not
    assumed by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the MDL write upon.

    FileOffset - Supplies the file offset at which the write took place.

    MdlChain - Pointer to the MDL chain used to perform the write operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/
{
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, return not supported
    //

    return FALSE;
}


/*********************************************************************************
        UNIMPLEMENTED FAST IO ROUTINES
        
        The following four Fast IO routines are for compression on the wire
        which is not yet implemented in NT.  
        
        NOTE:  It is highly recommended that you include these routines (which
               do a pass-through call) so your filter will not need to be
               modified in the future when this functionality is implemented in
               the OS.
        
        FastIoReadCompressed, FastIoWriteCompressed, 
        FastIoMdlReadCompleteCompressed, FastIoMdlWriteCompleteCompressed
**********************************************************************************/



BOOLEAN
FastIoReadCompressed (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    OUT struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
    IN ULONG CompressedDataInfoLength,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for reading compressed
    data from a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that will be read.

    FileOffset - Supplies the offset into the file to begin the read operation.

    Length - Specifies the number of bytes to be read from the file.

    LockKey - The key to be used in byte range lock checks.

    Buffer - Pointer to a buffer to receive the compressed data read.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data read.

    IoStatus - Variable to receive the final status of the read operation.

    CompressedDataInfo - A buffer to receive the description of the compressed
        data.

    CompressedDataInfoLength - Specifies the size of the buffer described by
        the CompressedDataInfo parameter.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(CompressedDataInfo);
    UNREFERENCED_PARAMETER(CompressedDataInfoLength);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}



BOOLEAN
FastIoWriteCompressed (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PMDL *MdlChain,
    OUT PIO_STATUS_BLOCK IoStatus,
    IN struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
    IN ULONG CompressedDataInfoLength,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for writing compressed
    data to a file.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    FileObject - Pointer to the file object that will be written.

    FileOffset - Supplies the offset into the file to begin the write operation.

    Length - Specifies the number of bytes to be write to the file.

    LockKey - The key to be used in byte range lock checks.

    Buffer - Pointer to the buffer containing the data to be written.

    MdlChain - A pointer to a variable to be filled in w/a pointer to the MDL
        chain built to describe the data written.

    IoStatus - Variable to receive the final status of the write operation.

    CompressedDataInfo - A buffer to containing the description of the
        compressed data.

    CompressedDataInfoLength - Specifies the size of the buffer described by
        the CompressedDataInfo parameter.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/

{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(Length);
    UNREFERENCED_PARAMETER(LockKey);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(CompressedDataInfo);
    UNREFERENCED_PARAMETER(CompressedDataInfoLength);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    IoStatus->Status = STATUS_INVALID_DEVICE_REQUEST;
    IoStatus->Information = 0;

    return TRUE;
}




BOOLEAN
FastIoMdlReadCompleteCompressed (
    IN PFILE_OBJECT FileObject,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing an
    MDL read compressed operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the read compressed function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not assumed
    by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the compressed read
        upon.

    MdlChain - Pointer to the MDL chain used to perform the read operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/
{
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, return not supported
    //

    return FALSE;
}



BOOLEAN
FastIoMdlWriteCompleteCompressed (
    IN PFILE_OBJECT FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PMDL MdlChain,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for completing a
    write compressed operation.

    This function simply invokes the file system's corresponding routine, if
    it has one.  It should be the case that this routine is invoked only if
    the write compressed function is supported by the underlying file system,
    and therefore this function will also be supported, but this is not assumed
    by this driver.

Arguments:

    FileObject - Pointer to the file object to complete the compressed write
        upon.

    FileOffset - Supplies the file offset at which the file write operation
        began.

    MdlChain - Pointer to the MDL chain used to perform the write operation.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE, depending on whether or not it is
    possible to invoke this function on the fast I/O path.

--*/
{
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(FileObject);
    UNREFERENCED_PARAMETER(FileOffset);
    UNREFERENCED_PARAMETER(MdlChain);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, return not supported
    //

    return FALSE;
}

BOOLEAN
FastIoQueryOpen (
    IN PIRP Irp,
    OUT PFILE_NETWORK_OPEN_INFORMATION NetworkInformation,
    IN PDEVICE_OBJECT DeviceObject)
/*++

Routine Description:

    This routine is the fast I/O "pass through" routine for opening a file
    and returning network information for it.

    This function simply invokes the file system's corresponding routine, or
    returns FALSE if the file system does not implement the function.

Arguments:

    Irp - Pointer to a create IRP that represents this open operation.  It is
        to be used by the file system for common open/create code, but not
        actually completed.

    NetworkInformation - A buffer to receive the information required by the
        network about the file being opened.

    DeviceObject - Pointer to this driver's device object, the device on
        which the operation is to occur.

Return Value:

    The function value is TRUE or FALSE based on whether or not fast I/O
    is possible for this file.

--*/
{
    PAGED_CODE();
    ASSERT(IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject));

    UNREFERENCED_PARAMETER(NetworkInformation);
    UNREFERENCED_PARAMETER(DeviceObject);

    //
    //  This is our KLIF, fail the operation
    //

    Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    Irp->IoStatus.Information = 0;

    return TRUE;
}
