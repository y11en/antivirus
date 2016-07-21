#include "pch.h"
#include "ntdddisk.h"

#include "../hook/avpgcom.h"

extern POBJECT_TYPE* IoDeviceObjectType;

#define _geom_size		0x1000
#define _vol_info		0x1000
#define _media_types	0x1000
#define _layout_size	0x1000 * 8

//////////////////////////////////////////////////////////////////////////

USHORT
LLD_GetSectorSize (
	PDEVICE_OBJECT pDevice
	)
{
	if (!pDevice->SectorSize)
	{
		if ((FILE_DEVICE_CD_ROM == pDevice->DeviceType) || (FILE_DEVICE_DVD == pDevice->DeviceType))
			return 2048;
		else
			return 512;
	}
	return pDevice->SectorSize;
}

VOID
ChangeByteOrder (
	PCHAR szString,
	USHORT uscStrSize
	)
{
	USHORT  i;
	CHAR    temp;
	
	for (i = 0; i < uscStrSize; i+=2)
	{
		temp = szString[i];
		szString[i] = szString[i+1];
		szString[i+1] = temp;
	}
}

NTSTATUS
IdentifyDevice (
	PDEVICE_OBJECT pDevice,
	PDISK_ID_INFO pOutInfo
	)
{
	UNREFERENCED_PARAMETER( pDevice );
	UNREFERENCED_PARAMETER( pOutInfo );

     return STATUS_UNSUCCESSFUL;
}

NTSTATUS
Disk_GetGeometryImp (
	PDEVICE_OBJECT pDevice,
	PDISK_GEOMETRY_EX* ppGeometry
	)
{
	PIRP Irp;
	PIO_STACK_LOCATION		irpStack;
	KEVENT Event;
    NTSTATUS status;
    IO_STATUS_BLOCK Iosb;
	
	*ppGeometry = ExAllocatePoolWithTag(NonPagedPool, _geom_size, '1BOS');
	if (!*ppGeometry)
		return STATUS_NO_MEMORY;
	
	memset(*ppGeometry, 0, _geom_size);
	
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, pDevice, 
		NULL, 0, *ppGeometry, _geom_size, FALSE, &Event, &Iosb);
	
    if (!Irp)
	{
		ExFreePool(*ppGeometry);
		return STATUS_UNSUCCESSFUL;
	}
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
	
    status = IoCallDriver(pDevice, Irp);
	
    if (STATUS_PENDING == status)
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		status = Iosb.Status;
	}
	
	if (NT_SUCCESS(status) && !Iosb.Information)
	{
		_dbg_break_;
		status = STATUS_UNSUCCESSFUL;
	}
	
	if (!NT_SUCCESS(status))
		ExFreePool(*ppGeometry);
	
    return status;
}

NTSTATUS
Disk_GetGeometrySimpleImp(
					PDEVICE_OBJECT pDevice,
					PDISK_GEOMETRY pGeometry
					)
{
	PIRP Irp;
	PIO_STACK_LOCATION		irpStack;
	KEVENT Event;
    NTSTATUS status;
    IO_STATUS_BLOCK Iosb;

	memset(pGeometry, 0, sizeof(DISK_GEOMETRY));
	
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY, pDevice, 
		NULL, 0, pGeometry, sizeof(DISK_GEOMETRY), FALSE, &Event, &Iosb);
	
    if (!Irp)
		return STATUS_UNSUCCESSFUL;
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
	
    status = IoCallDriver(pDevice, Irp);
	
    if (STATUS_PENDING == status)
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		status = Iosb.Status;
	}
	
	if (NT_SUCCESS( status ) && !Iosb.Information)
	{
		_dbg_break_;
		status = STATUS_UNSUCCESSFUL;
	}

	if (STATUS_NOT_IMPLEMENTED == status)
	{
		pGeometry->BytesPerSector = LLD_GetSectorSize( pDevice );
		pGeometry->Cylinders.QuadPart = 1;
		pGeometry->SectorsPerTrack = 1;
		pGeometry->TracksPerCylinder = 1;
		pGeometry->MediaType = Unknown;

		switch (pDevice->DeviceType)
		{
		case FILE_DEVICE_CD_ROM:
		case FILE_DEVICE_DVD:
			pGeometry->MediaType = RemovableMedia;
			break;

		default:
			_dbg_break_;
			break;
		}

		status = STATUS_SUCCESS;
	}
	
    return status;
}


NTSTATUS
Disk_GetGeometry (
	PDEVICE_OBJECT pDevice,
	PDISK_GEOMETRY pGeom
	)
{
	NTSTATUS status;
	PDISK_GEOMETRY_EX pGeomEx = NULL;
	
	status = Disk_GetGeometryImp(pDevice, &pGeomEx);	
	
	if (STATUS_VERIFY_REQUIRED == status)
	{
		_dbg_break_;
		status = IoVerifyVolume(pDevice, TRUE);
		status = Disk_GetGeometryImp(pDevice, &pGeomEx);
	}
	
	if (NT_SUCCESS(status))
	{
		memcpy(pGeom, &pGeomEx->Geometry, sizeof(DISK_GEOMETRY));
		ExFreePool(pGeomEx);
	}
	else
	{
		if (STATUS_INVALID_DEVICE_REQUEST == status
			|| STATUS_NOT_SUPPORTED == status
			|| STATUS_NOT_IMPLEMENTED == status)
		{
			status = Disk_GetGeometrySimpleImp(pDevice, pGeom);
		}
	}
	
	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS
GetPartitionInfoImp (
	PDEVICE_OBJECT pDevice,
	IN PPARTITION_INFORMATION pPartitionInformation
	)
{
	PIRP Irp;
	PIO_STACK_LOCATION		irpStack;
	KEVENT Event;
	NTSTATUS status;
	IO_STATUS_BLOCK Iosb;

	PARTITION_INFORMATION* pPartInfo;
#define _part_al	0x10

	pPartInfo = ExAllocatePoolWithTag(NonPagedPool, sizeof(PARTITION_INFORMATION) + _part_al, '2BOS');
	if (!pPartInfo)
		return STATUS_NO_MEMORY;		

	memset(pPartInfo, 0, sizeof(PARTITION_INFORMATION));
	KeInitializeEvent(&Event, NotificationEvent, FALSE);

	Irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_PARTITION_INFO, pDevice, 
		NULL, 0, pPartInfo, sizeof(PARTITION_INFORMATION) + _part_al, FALSE, &Event, &Iosb);

	if (Irp == NULL)
	{
		ExFreePool(pPartInfo);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;

	status = IoCallDriver(pDevice, Irp);

	if ( status == STATUS_PENDING )
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		status = Iosb.Status;
	}

	if (NT_SUCCESS(status) && !Iosb.Information)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	if (NT_SUCCESS(status))
		memcpy(pPartitionInformation, pPartInfo, sizeof(PARTITION_INFORMATION));

	ExFreePool(pPartInfo);

	return status;
}

//////////////////////////////////////////////////////////////////////////

PDEVICE_OBJECT
LLD_GetDeviceByName (
	PUNICODE_STRING pusObjectName
	)
{
	NTSTATUS status;
	
	PDEVICE_OBJECT pDevice = NULL;

	HANDLE DeviceHandle = NULL;
	OBJECT_ATTRIBUTES ObjAttr;
	IO_STATUS_BLOCK ioStatus;
	
	InitializeObjectAttributes( &ObjAttr, pusObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL );
	
	status = ZwCreateFile (
		&DeviceHandle,
		SYNCHRONIZE | FILE_ANY_ACCESS,
		&ObjAttr,
		&ioStatus,
		NULL,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
		);

	if(NT_SUCCESS(status) && DeviceHandle)
	{
		PFILE_OBJECT pFileObject = NULL;
		status = ObReferenceObjectByHandle (
			DeviceHandle,
			STANDARD_RIGHTS_REQUIRED,
			NULL,
			KernelMode,
			(VOID**)&pFileObject,
			NULL
			);

		if(NT_SUCCESS( status ))
		{
			PDEVICE_OBJECT pDevTmp = IoGetRelatedDeviceObject( pFileObject );
			if (pDevTmp)
			{
				if (FlagOn( pDevTmp->Flags, DO_DEVICE_INITIALIZING ))
				{
					// skip
				}
				else if (pDevTmp->DeviceType == FILE_DEVICE_DISK
					|| pDevTmp->DeviceType == FILE_DEVICE_CD_ROM
					|| pDevTmp->DeviceType == FILE_DEVICE_DVD )
				{
					status = ObReferenceObjectByPointer (
						pDevTmp,
						STANDARD_RIGHTS_REQUIRED,
						*IoDeviceObjectType,
						KernelMode
						);
					
					if (NT_SUCCESS(status))
						pDevice = pDevTmp;
				}
			}
			
			ObDereferenceObject( pFileObject );
		}
		
		ZwClose(DeviceHandle);
	}

	return pDevice;
}

PDEVICE_OBJECT
GetDeviceByName(
				PWCHAR DriveName,
				USHORT DriveNameLen
				)
{
	PDEVICE_OBJECT pDevice = NULL;
	
	UNICODE_STRING ObjectName;
	PWCHAR InternalDriveName = NULL;

	if (!DriveNameLen)
		DriveNameLen = wcslen( DriveName ) * sizeof(WCHAR);

	if (!DriveNameLen)
	{
		_dbg_break_;
		return NULL;
	}

	InternalDriveName = (PWCHAR)ExAllocatePoolWithTag(PagedPool, DriveNameLen + sizeof(WCHAR), 'grDN');
	if (!InternalDriveName)
		return NULL;
	
	__try
	{
		if (DriveNameLen)
		{
			memcpy(InternalDriveName, DriveName, DriveNameLen);
			InternalDriveName[DriveNameLen/sizeof(WCHAR)] = 0;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		ExFreePool(InternalDriveName);
		return NULL;
	}

	if (DriveNameLen)
	{
		ObjectName.Buffer = InternalDriveName;
		ObjectName.Length = ObjectName.MaximumLength = DriveNameLen;
	}
	
	if (ObjectName.Length == sizeof(WCHAR))
	{
		ExFreePool(InternalDriveName);
		return NULL;/*Disk_GetDeviceObjectByLetter(*DriveName);*/
	}
	else
		pDevice = LLD_GetDeviceByName( &ObjectName );	

	ExFreePool(InternalDriveName);

	return pDevice;
}

NTSTATUS
LLD_QueryNames (
	PVOID pOutBuffer,
	ULONG OutBufferSize,
	PULONG pRetSize
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PDEVICE_OBJECT pDevice = NULL;

	PVOID pBuffer;
	ULONG vol_idx = 0;
	ULONG BytesReturned;
	ULONG BytesOut = 0;
	BYTE* ptr = pOutBuffer;

	FILTER_VOLUME_BASIC_INFORMATION* pVolumeInfo;

	UNICODE_STRING usFloppyMask;

	RtlInitUnicodeString( &usFloppyMask, L"\\DEVICE\\FLOPPY*" );

	pBuffer = ExAllocatePoolWithTag(NonPagedPool, _vol_info, 'Vbos');
	if (!pBuffer)
		return STATUS_NO_MEMORY;
	do // volumes
	{
		status = FltEnumerateVolumeInformation(Globals.m_Filter, vol_idx++, FilterVolumeBasicInformation,
			pBuffer, _vol_info, &BytesReturned);

		if (STATUS_NO_MORE_ENTRIES == status)
		{
			status = STATUS_SUCCESS;
			break;
		}

		if (NT_SUCCESS(status))
		{
			pVolumeInfo = (FILTER_VOLUME_BASIC_INFORMATION*) pBuffer;
			pDevice = GetDeviceByName(pVolumeInfo->FilterVolumeName, pVolumeInfo->FilterVolumeNameLength);
			if (pDevice)
			{
				// our volume
				UNICODE_STRING usVolume;
				RtlInitEmptyUnicodeString( &usVolume, pVolumeInfo->FilterVolumeName, pVolumeInfo->FilterVolumeNameLength );
				usVolume.Length = pVolumeInfo->FilterVolumeNameLength;

				if (FsRtlIsNameInExpression( &usFloppyMask, &usVolume, TRUE, NULL))
				{
					// skip
				}
				else
				{
					if (pOutBuffer)
					{
						if (pVolumeInfo->FilterVolumeNameLength < OutBufferSize - BytesOut + sizeof(WCHAR))
						{
							memcpy(ptr, pVolumeInfo->FilterVolumeName, pVolumeInfo->FilterVolumeNameLength);
							BytesOut += pVolumeInfo->FilterVolumeNameLength;
							BytesOut += sizeof(WCHAR);
							ptr += pVolumeInfo->FilterVolumeNameLength;
							ptr += sizeof(WCHAR);
						}
						else
							status = STATUS_NO_MEMORY;
					}
					else
					{
						BytesOut += pVolumeInfo->FilterVolumeNameLength;
						BytesOut += sizeof(WCHAR);
					}
				}
				
				ObDereferenceObject(pDevice);
			}
		}
		
	} while(NT_SUCCESS(status));

	if (NT_SUCCESS(status)) // scan hdd
	{
		ULONG drv_cou;
		ULONG disk_cou;
		WCHAR i2a[16];

		WCHAR dskbuff[64];

		UNICODE_STRING strVal;
		UNICODE_STRING strDiskName;

		UNICODE_STRING pref1;
		UNICODE_STRING pref2;
		
		strVal.Buffer = i2a;
		strVal.Length = 0;
		strVal.MaximumLength = sizeof(i2a);

		strDiskName.Buffer = dskbuff;
		strDiskName.Length = 0;
		strDiskName.MaximumLength = sizeof(dskbuff);

		pref1.Buffer = L"\\Device\\Harddisk";
		pref1.Length = pref1.MaximumLength = sizeof(L"\\Device\\Harddisk") - sizeof(WCHAR);

		pref2.Buffer = L"\\DR";
		pref2.Length = pref1.MaximumLength = sizeof(L"\\DR") - sizeof(WCHAR);

		for (drv_cou = 0; drv_cou < 64; drv_cou++)
		{
			for (disk_cou = 0; disk_cou < 64; disk_cou++)
			{
				if (!NT_SUCCESS(status))
					break;

				strDiskName.Length = 0;
				RtlAppendUnicodeStringToString(&strDiskName, &pref1);
				RtlIntegerToUnicodeString(drv_cou, 10, &strVal);
				RtlAppendUnicodeStringToString(&strDiskName, &strVal);

				RtlAppendUnicodeStringToString(&strDiskName, &pref2);
				RtlIntegerToUnicodeString(disk_cou, 10, &strVal);
				RtlAppendUnicodeStringToString(&strDiskName, &strVal);

				pDevice = GetDeviceByName(strDiskName.Buffer, strDiskName.Length);
				if (pDevice)
				{
					if (pOutBuffer)
					{
						if (strDiskName.Length < OutBufferSize - BytesOut + sizeof(WCHAR))
						{
							memcpy(ptr, strDiskName.Buffer, strDiskName.Length);
							BytesOut += strDiskName.Length;
							BytesOut += sizeof(WCHAR);
							ptr += strDiskName.Length;
							ptr += sizeof(WCHAR);
						}
						else
							status = STATUS_NO_MEMORY;
					}
					else 
					{
						BytesOut += strDiskName.Length;
						BytesOut += sizeof(WCHAR);
					}

					ObDereferenceObject(pDevice);
				}
			}
		}
	}

	if (NT_SUCCESS(status)) // scan Floppy
	{
		ULONG drv_cou;
		WCHAR i2a[16];

		WCHAR dskbuff[64];

		UNICODE_STRING strVal;
		UNICODE_STRING strDiskName;

		UNICODE_STRING pref1;
		
		strVal.Buffer = i2a;
		strVal.Length = 0;
		strVal.MaximumLength = sizeof(i2a);

		strDiskName.Buffer = dskbuff;
		strDiskName.Length = 0;
		strDiskName.MaximumLength = sizeof(dskbuff);

		pref1.Buffer = L"\\Device\\Floppy";
		pref1.Length = pref1.MaximumLength = sizeof(L"\\Device\\Floppy") - sizeof(WCHAR);

		for (drv_cou = 0; drv_cou < 4; drv_cou++)
		{
			if (!NT_SUCCESS(status))
				break;

			strDiskName.Length = 0;
			RtlAppendUnicodeStringToString(&strDiskName, &pref1);
			RtlIntegerToUnicodeString(drv_cou, 10, &strVal);
			RtlAppendUnicodeStringToString(&strDiskName, &strVal);

			pDevice = GetDeviceByName(strDiskName.Buffer, strDiskName.Length);
			if (pDevice)
			{
				if (pOutBuffer)
				{
					if (strDiskName.Length < OutBufferSize - BytesOut + sizeof(WCHAR))
					{
						memcpy(ptr, strDiskName.Buffer, strDiskName.Length);
						BytesOut += strDiskName.Length;
						BytesOut += sizeof(WCHAR);
						ptr += strDiskName.Length;
						ptr += sizeof(WCHAR);
					}
					else
						status = STATUS_NO_MEMORY;
				}
				else 
				{
					BytesOut += strDiskName.Length;
					BytesOut += sizeof(WCHAR);
				}

				ObDereferenceObject(pDevice);
			}
		}
	}

	ExFreePool(pBuffer);

	if (NT_SUCCESS(status))
		*pRetSize = BytesOut;
	else
		*pRetSize = 0;

	return status;
}

BOOLEAN
IsReadAllowed (
	PDEVICE_OBJECT pDevice )
{
	if ((FILE_DEVICE_CD_ROM == pDevice->DeviceType)
		|| (FILE_DEVICE_DVD == pDevice->DeviceType))
	{
		// DISK_PERFORMANCE start
		// sleep 2 sec
		// DISK_PERFORMANCE end
		//analyze!
	};
	return TRUE;
}

NTSTATUS
LLD_ReadImp (
	PDEVICE_OBJECT pDevice,
	__int64 Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytes
	)
{
	PIRP					Irp;
	PIO_STACK_LOCATION		irpStack;
	NTSTATUS				status;
	IO_STATUS_BLOCK			ioStatus;
	KEVENT					event;
	USHORT					kratnost;
	
	kratnost = LLD_GetSectorSize( pDevice );

	if(BufferSize % kratnost)
		BufferSize = (BufferSize) - (BufferSize) % kratnost;

	if (0 == BufferSize)
	{
		_dbg_break_;
		return STATUS_INVALID_PARAMETER;
	}

	if (!IsReadAllowed( pDevice ))
		return STATUS_WAS_LOCKED;

	KeInitializeEvent(&event, NotificationEvent, FALSE);
	
	if(!(Irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ, pDevice, pBuffer, (ULONG) BufferSize, (LARGE_INTEGER*) &Offset, &event, &ioStatus)))
		return STATUS_INSUFFICIENT_RESOURCES;
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
		
	status = IoCallDriver(pDevice, Irp);
	if(status == STATUS_PENDING)
	{
		KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
		status = ioStatus.Status;
	}
	
	*pBytes = (ULONG) ioStatus.Information;
	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS
LLD_WriteImp (
	PDEVICE_OBJECT pDevice,
	__int64 Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytes
	)
{
	PIRP					Irp;
	PIO_STACK_LOCATION		irpStack;
	NTSTATUS				status;
	IO_STATUS_BLOCK			ioStatus;
	KEVENT					event;
	USHORT					kratnost;
	
	kratnost = LLD_GetSectorSize( pDevice );

	if(BufferSize % kratnost)
		BufferSize = (BufferSize) - (BufferSize) % kratnost;

	if (0 == BufferSize)
		return STATUS_INVALID_PARAMETER;

	KeInitializeEvent(&event, NotificationEvent, FALSE);
	
	if(!(Irp = IoBuildSynchronousFsdRequest(IRP_MJ_WRITE, pDevice, pBuffer, BufferSize, (LARGE_INTEGER*) &Offset, &event, &ioStatus)))
		return STATUS_INSUFFICIENT_RESOURCES;
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
	
	status = IoCallDriver(pDevice, Irp);
	if(status == STATUS_PENDING)
	{
		KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
		status = ioStatus.Status;
	}
	
	*pBytes = (ULONG) ioStatus.Information;
	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS
GetDriveLyoutImp(
				 PDEVICE_OBJECT pDevice,
				 PDRIVE_LAYOUT_INFORMATION_EX* ppDrvLayout
				 )
{
    PIRP Irp;
	PIO_STACK_LOCATION		irpStack;
    KEVENT Event;
    NTSTATUS status;
    IO_STATUS_BLOCK Iosb;
	
	PDRIVE_LAYOUT_INFORMATION_EX pDrvLayout;
	
	pDrvLayout = ExAllocatePoolWithTag(NonPagedPool, _layout_size, '3BOS');
	if (!pDrvLayout)
		return STATUS_NO_MEMORY;

	*ppDrvLayout = pDrvLayout;
	
	memset(pDrvLayout, 0, _layout_size);
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_LAYOUT_EX, pDevice, 
		NULL, 0, pDrvLayout, _layout_size, FALSE, &Event, &Iosb);
	
    if (Irp == NULL)
	{
		ExFreePool(pDrvLayout);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	irpStack = IoGetNextIrpStackLocation(Irp);
	irpStack->Flags |= SL_OVERRIDE_VERIFY_VOLUME;
	
    status = IoCallDriver(pDevice, Irp);
	
    if ( status == STATUS_PENDING )
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, (PLARGE_INTEGER)NULL);
		status = Iosb.Status;
	}
	
	if (NT_SUCCESS(status) && !Iosb.Information)
	{
		status = STATUS_UNSUCCESSFUL;
		_dbg_break_;
	}
	
	if (NT_SUCCESS(status))
	{
		ULONG part_cou;
		PARTITION_INFORMATION_EX* pPartitionEntry;

		switch (pDrvLayout->PartitionStyle)
		{
		case PARTITION_STYLE_MBR:
			{
				DRIVE_LAYOUT_INFORMATION_MBR* pMbr = &pDrvLayout->Mbr;
			}
			break;
		case PARTITION_STYLE_GPT:
			{
				DRIVE_LAYOUT_INFORMATION_GPT* pGpt = &pDrvLayout->Gpt;
			}
			break;
		case PARTITION_STYLE_RAW:
			break;

		default:
			_dbg_break_;
		}
		
		for (part_cou = 0; part_cou < pDrvLayout->PartitionCount; part_cou++)
		{
			pPartitionEntry = &pDrvLayout->PartitionEntry[part_cou];

			if (PARTITION_STYLE_MBR == pPartitionEntry->PartitionStyle)
			{
				if (PARTITION_ENTRY_UNUSED == pPartitionEntry->Mbr.PartitionType)
					continue;
			}
			
			switch (pPartitionEntry->PartitionStyle)
			{
			case PARTITION_STYLE_MBR:
				{
					PARTITION_INFORMATION_MBR* pMbr = &pPartitionEntry->Mbr;
				}
				break;
			case PARTITION_STYLE_GPT:
				{
					PARTITION_INFORMATION_GPT* pGpt = &pPartitionEntry->Gpt;
				}
			case PARTITION_STYLE_RAW:
				break;
			default:
				break;
			}
		}
	}
	else
	{
		ExFreePool(pDrvLayout);
	}
	
    return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS
IsMediaProtected(
				 PDEVICE_OBJECT pDevice,
				 BOOLEAN *pbProtected
				 )
{
	PIRP Irp;
	KEVENT Event;
    NTSTATUS Status;
    IO_STATUS_BLOCK Iosb;
	
	BYTE* pBuffer = ExAllocatePoolWithTag(NonPagedPool, _media_types, '.boS');
	if (pBuffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;
	
    KeInitializeEvent( &Event, NotificationEvent, FALSE );
	
    Irp = IoBuildDeviceIoControlRequest(IOCTL_STORAGE_GET_MEDIA_TYPES_EX, pDevice, 
		NULL, 0, pBuffer, _media_types, FALSE, &Event, &Iosb);
	
    if (Irp == NULL)
	{
		ExFreePool(pBuffer);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
    Status = IoCallDriver(pDevice, Irp);
	
    if ( Status == STATUS_PENDING )
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
		Status = Iosb.Status;
	}

	if (NT_SUCCESS(Status) && Iosb.Information < sizeof(GET_MEDIA_TYPES))
	{
		Status = STATUS_UNSUCCESSFUL;
		_dbg_break_;
	}
	
	if (NT_SUCCESS(Status))
	{
		PGET_MEDIA_TYPES pGetTypes = (PGET_MEDIA_TYPES) pBuffer;
		PDEVICE_MEDIA_INFO pDevInfo = (PDEVICE_MEDIA_INFO) pGetTypes->MediaInfo;
		
		*pbProtected = FALSE;
		
		while (pGetTypes->MediaInfoCount != 0)
		{
			if ((pGetTypes->DeviceType == FILE_DEVICE_TAPE_FILE_SYSTEM) || (pGetTypes->DeviceType == FILE_DEVICE_TAPE))
			{
				if (pDevInfo->DeviceSpecific.TapeInfo.MediaCharacteristics & (MEDIA_READ_ONLY | MEDIA_WRITE_PROTECTED))
					*pbProtected = TRUE;
			}
			else
			{
				if (pDevInfo->DeviceSpecific.DiskInfo.MediaCharacteristics & (MEDIA_READ_ONLY | MEDIA_WRITE_PROTECTED))
					*pbProtected = TRUE;
			}
			
			pDevInfo = (PDEVICE_MEDIA_INFO) ((BYTE*)pDevInfo + sizeof(DEVICE_MEDIA_INFO));
			pGetTypes->MediaInfoCount--;
		}
	}
	
	ExFreePool(pBuffer);
	
    return Status;
}

//////////////////////////////////////////////////////////////////////////


NTSTATUS
DoDiskRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutBuffer,
	ULONG OutBufferSize,
	PULONG pRetSize
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	PDEVICE_OBJECT pDevice = NULL;
	
	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pInRequest->m_Buffer;
	
	if ((pInRequest->m_BufferSize < sizeof(DISK_IO_REQUEST)) || (pDiskRequest->m_Tag != -1))
	{
		_dbg_break_;
		return STATUS_NOT_SUPPORTED;
	}
	
	if (pInRequest->m_IOCTL == _AVPG_IOCTL_DISK_QUERYNAMES)
	{
		if (pOutBuffer && OutBufferSize >= sizeof(WCHAR) * 2)
		{
			memset( pOutBuffer, 0, OutBufferSize );
			return LLD_QueryNames( pOutBuffer, OutBufferSize, pRetSize );
		}
		
		*pRetSize = 0;
		return STATUS_INVALID_PARAMETER;
	}
	
	if (pInRequest->m_IOCTL == _AVPG_IOCTL_DISK_QUERYHARDNAME)
	{
		_dbg_break_;
		return STATUS_NOT_SUPPORTED;
	}
	
	pDevice = GetDeviceByName(pDiskRequest->m_DriveName, 0);
	
	if (pDevice == NULL)
		return STATUS_NOT_FOUND;
	
	switch (pInRequest->m_IOCTL)
	{
	case _AVPG_IOCTL_DISK_GET_GEOMETRY:
		{
			if (!pOutBuffer || OutBufferSize < sizeof(DISK_GEOMETRY))
				status = STATUS_INVALID_PARAMETER;
			else
			{
				*pRetSize = sizeof(DISK_GEOMETRY);
				status = Disk_GetGeometry(pDevice, (PDISK_GEOMETRY) pOutBuffer);
			}
		}
		break;
		
	case _AVPG_IOCTL_DISK_GET_PARTITION_INFO:
		{
			if (!pOutBuffer || OutBufferSize < sizeof(PARTITION_INFORMATION))
				status = STATUS_INVALID_PARAMETER;
			else
			{
				*pRetSize = sizeof(PARTITION_INFORMATION);
				status = GetPartitionInfoImp(pDevice, (PPARTITION_INFORMATION) pOutBuffer);
			}
		}
		break;
		
	case _AVPG_IOCTL_DISK_GET_DEVICE_TYPE:
		{
			if (!pOutBuffer || OutBufferSize < sizeof(DEVICE_TYPE))
				status = STATUS_INVALID_PARAMETER;
			else
			{
				*pRetSize = sizeof(DEVICE_TYPE);
				*(DEVICE_TYPE*)pOutBuffer = pDevice->DeviceType;
				status = STATUS_SUCCESS;
			}
		}
		break;
		
	case _AVPG_IOCTL_DISK_GET_SECTOR_SIZE:
		{
			if (!pOutBuffer || OutBufferSize < sizeof(USHORT))
				status = STATUS_INVALID_PARAMETER;
			else
			{
				*pRetSize = sizeof(USHORT);
				*(USHORT*) pOutBuffer = (USHORT) LLD_GetSectorSize( pDevice );
				status = STATUS_SUCCESS;
			}
		}
		break;
		
	case _AVPG_IOCTL_DISK_READ:
		{
			if (!pOutBuffer || !OutBufferSize)
				status = STATUS_INVALID_PARAMETER;
			else
			{
				ULONG Bytes = 0;
				status = LLD_ReadImp( pDevice, pDiskRequest->m_ByteOffset, pOutBuffer, OutBufferSize, &Bytes );
				if (NT_SUCCESS(status))
					*pRetSize = Bytes;
				else
					*pRetSize = 0;
			}
		}
		break;
		
	case _AVPG_IOCTL_DISK_WRITE:
		{
			*pRetSize = pDiskRequest->m_DataSize;

			if (InRequestSize < sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST) + pDiskRequest->m_DataSize)
			{
				_dbg_break_;
				status = STATUS_INVALID_PARAMETER;
			}
			else
			{
				status = LLD_WriteImp( pDevice, pDiskRequest->m_ByteOffset, pDiskRequest->m_Buffer, *pRetSize, pRetSize );
				if (!NT_SUCCESS(status))
					*pRetSize = 0;
				else
				{
					if (pOutBuffer && OutBufferSize >= sizeof(ULONG))
					{
						*((ULONG*) pOutBuffer) = (ULONG) *pRetSize;
						*pRetSize = sizeof(ULONG);
					}
				}
			}
		}
		break;
		
	case _AVPG_IOCTL_DISK_ISWRITEPROTECTED:
		{
			if (!pOutBuffer || OutBufferSize < sizeof(ULONG))
				status = STATUS_INVALID_PARAMETER;
			else
			{
				BOOLEAN bProtected;
				if (NT_SUCCESS(IsMediaProtected(pDevice, &bProtected)))
				{
					status = STATUS_SUCCESS;
					*(ULONG*) pOutBuffer = bProtected;
					*pRetSize = sizeof(ULONG);
				}
			}
	
		}
		break;
		
	case _AVPG_IOCTL_DISK_ISPARTITION:
		{
			PDRIVE_LAYOUT_INFORMATION_EX pDrvLayout = NULL;
			status = GetDriveLyoutImp(pDevice, &pDrvLayout);
			if (NT_SUCCESS(status))
				ExFreePool(pDrvLayout);
		}
		break;
		
	case _AVPG_IOCTL_DISK_GETID:
		{
			status = STATUS_NOT_SUPPORTED;
		}
		break;

	case _AVPG_IOCTL_DISK_ISLOCKED:
		if (!pOutBuffer || OutBufferSize < sizeof(ULONG))
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			BOOLEAN bLockState = FALSE;
			*pRetSize = 0;

			switch(pDevice->DeviceType)
			{
			case FILE_DEVICE_CD_ROM:
			case FILE_DEVICE_DVD:
				status = QueryCdRomState( pDevice,  &bLockState );
				if (NT_SUCCESS( status ))
				{
					*(ULONG*) pOutBuffer = bLockState;
					*pRetSize = sizeof(ULONG);
				}
				break;

			default:
				status = STATUS_NOT_SUPPORTED;
				break;
			}

			break;
		}
		break;

	case _AVPG_IOCTL_DISK_LOCK:
		status = STATUS_NOT_SUPPORTED;
		break;
	
	default:
		{
			_dbg_break_;
		}
		break;
	}
	
	ObDereferenceObject(pDevice);
	
	return status;
}

NTSTATUS
LLD_GetInfo (
	PUNICODE_STRING pusName,
	PMKLIF_LLD_INFO pInfo
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	USHORT SectorSize = 0;
	
	PDRIVE_LAYOUT_INFORMATION_EX pDrvLayout = NULL;
	BOOLEAN bProtected = FALSE;
	BOOLEAN bLockState = FALSE;

	PARTITION_INFORMATION PartitionInformation;
	
	PDEVICE_OBJECT pDevice = LLD_GetDeviceByName( pusName );
	if (!pDevice)
	{
		_dbg_break_;
		return STATUS_NOT_FOUND;
	}

	if (FlagOn( pDevice->Characteristics, FILE_REMOVABLE_MEDIA ))
		SetFlag( pInfo->m_Flags, _LLD_FLAG_IS_REMOVABLE );

	pInfo->m_Type = pDevice->DeviceType;
	SetFlag( pInfo->m_Resolved, _LLD_RESOLVED_TYPE );

	pInfo->m_SectorSize = LLD_GetSectorSize( pDevice );
	SetFlag( pInfo->m_Resolved, _LLD_RESOLVED_SECTOR_SIZE );

	status = GetPartitionInfoImp( pDevice, &PartitionInformation );
	if (NT_SUCCESS(status))
	{
		SetFlag( pInfo->m_Resolved, _LLD_RESOLVED_PART_NUMBER );
		pInfo->m_PartitionNumber = PartitionInformation.PartitionNumber;

		SetFlag( pInfo->m_Resolved, _LLD_RESOLVED_PARTITION_TYPE );
		pInfo->m_PartitionalType = PartitionInformation.PartitionType;

		SetFlag( pInfo->m_Resolved, _LLD_RESOLVED_LENGTH );
		pInfo->m_Length = PartitionInformation.PartitionLength;

		if (PartitionInformation.RecognizedPartition)
			SetFlag( pInfo->m_Flags, _LLD_FLAG_IS_RECOGNIZED );
		if (PartitionInformation.BootIndicator)
			SetFlag( pInfo->m_Flags, _LLD_FLAG_IS_BOOTABLE );
	}

	status = GetDriveLyoutImp(pDevice, &pDrvLayout);
	if (NT_SUCCESS(status))
	{
		ExFreePool(pDrvLayout);
		SetFlag( pInfo->m_Flags, _LLD_FLAG_IS_PARTITIONAL );
	}

	switch(pDevice->DeviceType)
	{
	case FILE_DEVICE_CD_ROM:
	case FILE_DEVICE_DVD:
		status = QueryCdRomState( pDevice,  &bLockState );
		if (NT_SUCCESS( status ))
			SetFlag( pInfo->m_Flags, _LLD_FLAG_IS_LOCKED );
		break;
	default:
		break;
	}

	status = IsMediaProtected( pDevice, &bProtected );
	if (NT_SUCCESS( status ) && bProtected)
		SetFlag( pInfo->m_Flags, _LLD_FLAG_IS_WRITE_PROTECTED );

	ObDereferenceObject( pDevice );

	return STATUS_SUCCESS;
}

NTSTATUS
LLD_GetGeometry (
	PUNICODE_STRING pusName,
	PMKLIF_LLD_GEOMETRY pGeometry
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	PDEVICE_OBJECT pDevice = LLD_GetDeviceByName( pusName );
	DISK_GEOMETRY Geometry;
	if (!pDevice)
	{
		_dbg_break_;
		return STATUS_NOT_FOUND;
	}

	status = Disk_GetGeometry( pDevice, &Geometry );
	if (NT_SUCCESS( status ))
	{
		pGeometry->m_MediaType = Geometry.MediaType;
		pGeometry->m_Cylinders = Geometry.Cylinders;
		pGeometry->m_TracksPerCylinder = Geometry.TracksPerCylinder;
		pGeometry->m_SectorsPerTrack = Geometry.SectorsPerTrack;
	}

	ObDereferenceObject( pDevice );

	return status;
}

NTSTATUS
LLD_GetDiskId (
	PUNICODE_STRING pusName,
	PDISK_ID_INFO pDiskIdInfo
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	PDEVICE_OBJECT pDevice = LLD_GetDeviceByName( pusName );
	if (!pDevice)
	{
		_dbg_break_;
		return STATUS_NOT_FOUND;
	}


	RtlZeroMemory( pDiskIdInfo, sizeof(DISK_ID_INFO) );

	status = IdentifyDevice( pDevice, pDiskIdInfo );
	if (NT_SUCCESS( status ))
	{
	}

	ObDereferenceObject( pDevice );

	return status;
}

NTSTATUS
LLD_Read (
	PUNICODE_STRING pusName,
	__int64 Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytes
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	PDEVICE_OBJECT pDevice = LLD_GetDeviceByName( pusName );

	if (!pDevice)
	{
		_dbg_break_;
		return STATUS_NOT_FOUND;
	}

	status = LLD_ReadImp( pDevice, Offset, pBuffer, BufferSize, pBytes );

	ObDereferenceObject( pDevice );

	return status;
}

NTSTATUS
LLD_Write (
	PUNICODE_STRING pusName,
	__int64 Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytes
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;
	PDEVICE_OBJECT pDevice = LLD_GetDeviceByName( pusName );

	if (!pDevice)
	{
		_dbg_break_;
		return STATUS_NOT_FOUND;
	}

	status = LLD_WriteImp( pDevice, Offset, pBuffer, BufferSize, pBytes );

	ObDereferenceObject( pDevice );

	return status;
}
