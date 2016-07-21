#include "pch.h"
#include "ntddcdrm.h"

NTSTATUS
QueryCdRomState (
	PDEVICE_OBJECT pDevice,
	PBOOLEAN bLockState
	)
{
	NTSTATUS Status = STATUS_INSUFFICIENT_RESOURCES;

	CDROM_EXCLUSIVE_ACCESS* pExclusiveCheck = NULL;
	CDROM_EXCLUSIVE_LOCK_STATE* pCurrentState = NULL;

	pExclusiveCheck = ExAllocatePoolWithTag( NonPagedPool, sizeof(CDROM_EXCLUSIVE_ACCESS), '.boS' );
	pCurrentState = ExAllocatePoolWithTag( NonPagedPool, sizeof(CDROM_EXCLUSIVE_LOCK_STATE), '.boS' );

	if (pExclusiveCheck && pCurrentState)
	{
		PIRP Irp;
		KEVENT Event;
		IO_STATUS_BLOCK Iosb;

		RtlZeroMemory( pExclusiveCheck, sizeof(CDROM_EXCLUSIVE_ACCESS) );
		RtlZeroMemory( pCurrentState, sizeof(CDROM_EXCLUSIVE_LOCK_STATE) );

		pExclusiveCheck->RequestType = ExclusiveAccessQueryState;

		KeInitializeEvent( &Event, NotificationEvent, FALSE );

		Irp = IoBuildDeviceIoControlRequest (
			IOCTL_CDROM_EXCLUSIVE_ACCESS,
			pDevice, 
			pExclusiveCheck,
			sizeof(CDROM_EXCLUSIVE_ACCESS),
			pCurrentState,
			sizeof(CDROM_EXCLUSIVE_LOCK_STATE),
			FALSE,
			&Event,
			&Iosb
			);

		if (Irp)
		{
			Status = IoCallDriver(pDevice, Irp);

			if ( Status == STATUS_PENDING )
			{
				KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
				Status = Iosb.Status;
			}

			if (NT_SUCCESS(Status) && Iosb.Information != sizeof(CDROM_EXCLUSIVE_LOCK_STATE))
			{
				Status = STATUS_UNSUCCESSFUL;
				_dbg_break_;
			}

			if (NT_SUCCESS(Status))
				*bLockState = pCurrentState->LockState;
		}
	}

	if (pExclusiveCheck)
		ExFreePool(pExclusiveCheck);

	if (pCurrentState)
		ExFreePool(pCurrentState);

	return Status;
}