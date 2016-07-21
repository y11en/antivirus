#include "kl_ioctl.h"

CKl_Device::CKl_Device() : m_DevicePtr( NULL )
{
}

CKl_Device::CKl_Device(PVOID DevicePtr) : m_DevicePtr(DevicePtr)
{
}

CKl_Device::CKl_Device(PCHAR DeviceName)
{
	m_DevicePtr = FindDeviceByName( DeviceName );
}

CKl_Device::CKl_Device(PWCHAR DeviceName)
{
	m_DevicePtr = FindDeviceByName( DeviceName );
}

#ifndef ISWIN9X
/*
 *	        ------- WINNT Section  ----------------
 */

PVOID
CKl_Device::FindDeviceByName( PCHAR Name )
{
	return NULL;
}

PVOID
CKl_Device::FindDeviceByName( PWCHAR Name )
{
	PVOID				pDevice = NULL;	
	NTSTATUS			ntStatus;
	HANDLE				hDriver;
	UNICODE_STRING		DeviceName;	
	OBJECT_ATTRIBUTES	ObjAttr;
	IO_STATUS_BLOCK		ioStatus;	
    
    RtlInitUnicodeString( &DeviceName, Name );
	
	InitializeObjectAttributes( &ObjAttr, &DeviceName, OBJ_CASE_INSENSITIVE, NULL, NULL);
	
	if ( NT_SUCCESS ( ntStatus = ZwCreateFile( 
		&hDriver, 
		SYNCHRONIZE | FILE_ANY_ACCESS, 
		&ObjAttr, 
		&ioStatus, 
		NULL, 
		0, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		FILE_OPEN,  
		FILE_SYNCHRONOUS_IO_NONALERT, 
		NULL, 
		0) ) )
	{	
		if ( NT_SUCCESS ( ntStatus = ObReferenceObjectByHandle( hDriver, FILE_READ_DATA, NULL, KernelMode, &m_DeviceFO, NULL ) ))
		{
			pDevice = IoGetRelatedDeviceObject( (PFILE_OBJECT)m_DeviceFO );

			ObDereferenceObject( ( PFILE_OBJECT)m_DeviceFO);
		}
		
		ZwClose( hDriver );
	}
	

	return pDevice;
}

KLSTATUS
CKl_Device::SendIoctl(ULONG ioctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, DWORD* dwRet )
{
	PIRP				Irp;
	KEVENT				Event;
	IO_STATUS_BLOCK		ioStatus;
	NTSTATUS			ntStatus = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION  irpSp;	
    
	PAGED_CODE();

	if ( m_DevicePtr == NULL )
		return KL_UNSUCCESSFUL;
	
	KeInitializeEvent( &Event, NotificationEvent, FALSE);
	
	Irp = IoBuildDeviceIoControlRequest( ioctl,
											(PDEVICE_OBJECT)m_DevicePtr,
											pInBuffer,
											InBufferSize,
											pOutBuffer,
											OutBufferSize,
											FALSE,
											&Event,
											&ioStatus);	
	
    if ( Irp ) 
    {
        irpSp               = IoGetNextIrpStackLocation( Irp );
        irpSp->FileObject   = (PFILE_OBJECT)m_DeviceFO;
		
        ntStatus            = IoCallDriver ( (PDEVICE_OBJECT)m_DevicePtr, Irp );
        
        if ( ntStatus  ==  STATUS_PENDING ) 
        {
            KeWaitForSingleObject( &Event, Executive, KernelMode, FALSE, NULL );
            
            ntStatus = ioStatus.Status;
        }
		
		if ( dwRet )
			*dwRet = (ULONG)ioStatus.Information;
    }

	switch( ntStatus ) 
	{
	case STATUS_SUCCESS :
		return KL_SUCCESS;
	case STATUS_BUFFER_TOO_SMALL:
		return KL_BUFFER_TOO_SMALL;
	default:
		return KL_UNSUCCESSFUL;
	}

	return KL_UNSUCCESSFUL;
}

#else // ISWIN9X
/*
 *	        ------- WIN9X Section  ----------------
 */

PVOID
CKl_Device::FindDeviceByName(PCHAR Name )
{
	return Get_DDB( 0, Name );
}

PVOID
CKl_Device::FindDeviceByName(PWCHAR Name )
{
	return NULL;
}

KLSTATUS
CKl_Device::SendIoctl(ULONG ioctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, DWORD* dwRet )
{
	KLSTATUS    klStatus = KL_SUCCESS;

    ALLREGS     regs;	// register struct passed to Directed_Sys_Ctrl
	IOCTLPARAMS io;	    // Ioctl param struct passed to target device
	
	if ( m_DevicePtr == NULL )
		return KL_UNSUCCESSFUL;
	
	// Set up the ioctl params 
	io.dioc_IOCtlCode   = ioctl;
	io.dioc_InBuf       = pInBuffer;
	io.dioc_cbInBuf     = InBufferSize;
	io.dioc_OutBuf      = pOutBuffer;
	io.dioc_cbOutBuf    = OutBufferSize;
	io.dioc_bytesret    = dwRet;
	
	// Clear the regs structure
	memset ( &regs, 0, sizeof (regs) );
	
	// Put a pointer to the ioctl param struct in the register struct
	regs.RESI = (DWORD)&io;
	
	// Send the message to the target device
	Directed_Sys_Control( (tagDDB*)m_DevicePtr, W32_DEVICEIOCONTROL, &regs );
	
	// Return the error code received back from the target device.
	if ( regs.REAX == 0 )
        return KL_SUCCESS;

    return KL_UNSUCCESSFUL;
}

#endif // ISWIN9X