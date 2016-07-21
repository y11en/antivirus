// low level disk io for 9x


DCB* GetDCB4LogDrive(BYTE letter)
{
	ISP_dcb_get GetDcbIsp;
	DCB* pDcb;
	
	memset(&GetDcbIsp, 0, sizeof(ISP_dcb_get));

	GetDcbIsp.ISP_g_d_hdr.ISP_func = ISP_GET_DCB;	
	GetDcbIsp.ISP_g_d_drive = letter;

	if(!iosRegister)
	{
		iosRegister = 1;
		IOS_Register(&DRIVER_Drp);
	}

	KLIF_Ilb.ILB_service_rtn((PISP)&GetDcbIsp);

	if (GetDcbIsp.ISP_g_d_hdr.ISP_result != 0)
		return NULL;

	pDcb = (DCB*) GetDcbIsp.ISP_g_d_dcb->DCB_physical_dcb;

	return pDcb;
}
//+ ------------------------------------------------------------------------------------------

__declspec(naked) void Ring0GetPortVal()
{
  _asm
  {
    Cmp CL, 1
    Je ByteVal
    Cmp CL, 2
    Je WordVal
    Cmp CL, 4
    Je DWordVal

ByteVal:

    In AL, DX
    Mov [EBX], AL
    ret

WordVal:

    In AX, DX
    Mov [EBX], AX
    ret

DWordVal:

    In EAX, DX
    Mov [EBX], EAX
	ret
  }
}

__declspec(naked) void Ring0SetPortVal()
{
  _asm
  {
    Cmp CL, 1
    Je ByteVal
    Cmp CL, 2
    Je WordVal
    Cmp CL, 4
    Je DWordVal

ByteVal:

    Mov AL, [EBX]
    Out DX, AL
    ret
	

WordVal:

    Mov AX, [EBX]
    Out DX, AX
    ret

DWordVal:

    Mov EAX, [EBX]
    Out DX, EAX
	ret
  }
}


BOOLEAN GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize)
{
	DWORD bRet;

	_asm Mov DX, [wPortAddr]
    _asm Mov EBX, [pdwPortVal]
    _asm Mov CL, [bSize]

	Ring0GetPortVal();

	_asm mov bRet, eax

	return (BOOLEAN) bRet;
}

BOOLEAN SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize)
{
	DWORD bRet;
	DWORD *pdwPortVal = &dwPortVal;

	_asm Mov DX, [wPortAddr]
    _asm Mov EBX, [pdwPortVal]
    _asm Mov CL, [bSize]

	Ring0SetPortVal();

	_asm mov bRet, eax
	
	return (BOOLEAN) bRet;
}

char *ConvertToString (DWORD diskdata [256], int firstIndex, int lastIndex)
{
   static char string [1024];
   int index = 0;
   int position = 0;

      //  each integer has two characters stored in it backwards
   for (index = firstIndex; index <= lastIndex; index++)
   {
         //  get high byte for 1st character
      string [position] = (char) (diskdata [index] / 256);
      position++;

         //  get low byte for 2nd character
      string [position] = (char) (diskdata [index] % 256);
      position++;
   }

      //  end the string
   string [position] = '\0';

      //  cut off the trailing blanks
   for (index = position - 1; index > 0 && ' ' == string [index]; index--)
      string [index] = '\0';

   return string;
}

NTSTATUS ReadDrivePortsInWin9X (int drive, PDISK_ID_INFO pInfoRet)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	DWORD diskdata [256];
	WORD  baseAddress = 0;   //  Base address of drive controller 
	DWORD portValue = 0;
	int waitLoop = 0;
	int index = 0;
	
	switch (drive / 2)
	{
	case 0: baseAddress = 0x1f0; break;
	case 1: baseAddress = 0x170; break;
	case 2: baseAddress = 0x1e8; break;
	case 3: baseAddress = 0x168; break;
	}
	
	//  Wait for controller not busy 
	waitLoop = 100000;
	while (--waitLoop > 0)
	{
		GetPortVal ((WORD) (baseAddress + 7), &portValue, (BYTE) 1);
		//  drive is ready
		if ((portValue & 0x40) == 0x40) break;
		//  previous drive command ended in error
		if ((portValue & 0x01) == 0x01) break;
	}
	
	if (waitLoop < 1)
		return status;
	
	//  Set Master or Slave drive
	if ((drive % 2) == 0)
		SetPortVal ((WORD) (baseAddress + 6), 0xA0, 1);
	else
		SetPortVal ((WORD) (baseAddress + 6), 0xB0, 1);
	
	//  Get drive info data
	SetPortVal ((WORD) (baseAddress + 7), 0xEC, 1);
	
	// Wait for data ready 
	waitLoop = 100000;
	while (--waitLoop > 0)
	{
		GetPortVal ((WORD) (baseAddress + 7), &portValue, 1);
		//  see if the drive is ready and has it's info ready for us
		if ((portValue & 0x48) == 0x48) break;
		//  see if there is a drive error
		if ((portValue & 0x01) == 0x01) break;
	}
	
	//  check for time out or other error                                                    
	if (waitLoop < 1 || portValue & 0x01) // was continue;
		return status;
	
	//  read drive id information
	for (index = 0; index < 256; index++)
	{
		diskdata[index] = 0;   //  init the space
		GetPortVal (baseAddress, &(diskdata [index]), 2);
	}
	
	// disk data is IDINFO struct :)
	strcpy(pInfoRet->m_ModelNumber, ConvertToString(diskdata, 27, 46));
	strcpy(pInfoRet->m_SerialNumber, ConvertToString(diskdata, 10, 19));
	strcpy(pInfoRet->m_FirmwareRev, ConvertToString(diskdata, 23, 26));
	
	status = STATUS_SUCCESS;
	
	return status;
}
//+ ------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------
NTSTATUS DoDiskRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, ULONG *RetSize)
{
	NTSTATUS			ntStatus = STATUS_OBJECT_NAME_NOT_FOUND;

	// request by name pDiskRequest->m_DriveName
	// logical - A-Z (like "A:\")
	// phisical (like 80Disk)
	ULONG			DrvNameLen;
	WCHAR*			pDriveName;

	BYTE LogicalID = 0;
	BYTE PhisicalID = 0;
	
	PDISK_IO_REQUEST pDiskRequestTmp = (PDISK_IO_REQUEST) pInRequest->m_Buffer;
	
	if ((pInRequest->m_BufferSize < sizeof(DISK_IO_REQUEST)) || (pDiskRequestTmp->m_Tag != -1))
	{
		PDISK_IO_REQUEST_OLD pDiskRequestOld = NULL;
		if (pInRequest->m_BufferSize < sizeof(DISK_IO_REQUEST_OLD))
		{
			DbPrint(DC_LLDISKIO, DL_WARNING, ("DoDiskRequest - input buffer too small %d (required %d)\n", pInRequest->m_BufferSize, sizeof(DISK_IO_REQUEST)));
			return STATUS_INVALID_BLOCK_LENGTH;
		}

		pDiskRequestOld = (PDISK_IO_REQUEST_OLD) pInRequest->m_Buffer;

		DrvNameLen = DISKNAMEMAXLEN_OLD;
		pDriveName = pDiskRequestOld->m_DriveName;

		LogicalID = ((BYTE*)pDiskRequestOld->m_DriveName)[0];
		PhisicalID = ((BYTE*)pDiskRequestOld->m_DriveName)[1] - '0' + 0x80;
	}
	else
	{
		PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pInRequest->m_Buffer;

		DrvNameLen = DISKNAMEMAXLEN;
		pDriveName = pDiskRequest->m_DriveName;

		LogicalID = ((BYTE*)pDiskRequest->m_DriveName)[0];
		PhisicalID = ((BYTE*)pDiskRequest->m_DriveName)[1] - '0' + 0x80;
	}

	if ((LogicalID >= 'a') && (LogicalID <= 'z'))
		LogicalID &= (~0x20);

	DbPrint(DC_LLDISKIO, DL_SPAM, ("DoDiskRequest - request %d\n", pInRequest->m_IOCTL));

	if (pInRequest->m_IOCTL != _AVPG_IOCTL_DISK_QUERYNAMES)
	{
		memset(pDriveName, 0, DrvNameLen);
		
		DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - request for %c %c\n", LogicalID, PhisicalID));
	}
	
	*RetSize = 0;
		
	switch (pInRequest->m_IOCTL)
	{
	case _AVPG_IOCTL_DISK_QUERYHARDNAME:
		DbPrint(DC_LLDISKIO, DL_INFO, ("Query hard name\n"));
		if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
		{
			DCB* pDCB;
			
			pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));
			if (pDCB != 0)
			{
				CHAR DrvName[4];
			
				DrvName[0] = '8';
				DrvName[1] = 0;
				DrvName[2] = 'h';
				DrvName[3] = '\0';

				if (pDCB->DCB_bdd.DCB_BDD_Int_13h_Number >= 0x80)
				{
					DrvName[1] = '0' + (pDCB->DCB_bdd.DCB_BDD_Int_13h_Number - 0x80);
					strcpy(pOutRequest, DrvName);
					*RetSize = strlen(DrvName) + 1;
					ntStatus = STATUS_SUCCESS;
				}
			}
		}
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_GEOMETRY:
		if (pOutRequest == NULL || OutRequestSize < sizeof(DISK_GEOMETRY))
		{
			DbPrint(DC_LLDISKIO, DL_INFO, ("Get geometry failed. too small otuput buffer %d (required %d)\n", OutRequestSize, sizeof(DISK_GEOMETRY)));
			ntStatus = STATUS_BUFFER_TOO_SMALL;
		}
		else
		{
			PDISK_GEOMETRY pGeometry = (PDISK_GEOMETRY) pOutRequest;
			if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
			{
				DCB* pDCB;
				
				pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));
				if (pDCB != 0)
				{
					LARGE_INTEGER cyl;

					cyl.u.HighPart = 0;

					if (pDCB->DCB_actual_blk_size)
					{
						cyl.u.LowPart =  pDCB->DCB_actual_cyl_cnt;
						pGeometry->BytesPerSector = pDCB->DCB_actual_blk_size;
						pGeometry->SectorsPerTrack = pDCB->DCB_actual_spt;
						pGeometry->TracksPerCylinder = pDCB->DCB_actual_head_cnt;
					}
					else
					{
						cyl.u.LowPart =  pDCB->DCB_bdd.DCB_apparent_cyl_cnt;
						pGeometry->BytesPerSector = pDCB->DCB_bdd.DCB_apparent_blk_size;
						pGeometry->SectorsPerTrack = pDCB->DCB_bdd.DCB_apparent_spt;
						pGeometry->TracksPerCylinder = pDCB->DCB_bdd.DCB_apparent_head_cnt;
					}

					switch(pDCB->DCB_cmn.DCB_device_type)
					{
					case DCB_type_disk:
						pGeometry->MediaType = FixedMedia;
						break;
					case DCB_type_cdrom:
						pGeometry->MediaType = RemovableMedia;
						break;
					case DCB_type_floppy:
						pGeometry->MediaType = F3_1Pt44_512;	//wrong - parse format :(
						break;
					}

					pGeometry->Cylinders = cyl;
					*RetSize = sizeof(DISK_GEOMETRY);

					ntStatus = STATUS_SUCCESS;
				}
			}
			else
			{
				PBDD pbdd= BlockDev_Find_Int13_Drive((BYTE) PhisicalID);
				if (pbdd != NULL)
				{
					LARGE_INTEGER li;

					pGeometry->BytesPerSector = pbdd->BDD_Sector_Size;
					li.u.HighPart = 0;
					li.u.LowPart = pbdd->BDD_Num_Cylinders;
					pGeometry->Cylinders = li;
					pGeometry->MediaType = pbdd->BDD_Device_Type;
					pGeometry->SectorsPerTrack = pbdd->BDD_Num_Sec_Per_Track;
					pGeometry->TracksPerCylinder = pbdd->BDD_Num_Sec_Per_Track;
					pGeometry->MediaType = FixedMedia;

					*RetSize = sizeof(DISK_GEOMETRY);
					ntStatus = STATUS_SUCCESS;
				}
			}
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_GEOMETRY\n"));
		}
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_PARTITION_INFO:
		ntStatus = STATUS_NOT_IMPLEMENTED;
		break;
// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_DEVICE_TYPE:
		{
			if (pOutRequest == NULL || OutRequestSize < sizeof(DEVICE_TYPE))
			{
				ntStatus = STATUS_BUFFER_TOO_SMALL;
				DbPrint(DC_LLDISKIO, DL_INFO, ("Get device type failed. too small otuput buffer %d (required %d)\n", OutRequestSize, sizeof(DEVICE_TYPE)));
			}
			else
			{
				*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_UNKNOWN;

				DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_DEVICE_TYPE\n"));
				*RetSize = sizeof(DEVICE_TYPE);
				if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
				{
					//*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_UNKNOWN;

					DCB* pDCB;

					pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));
					if (pDCB)
					{
						switch(pDCB->DCB_cmn.DCB_device_type)
						{
						case DCB_type_disk:
							*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_DISK;
							break;
						case DCB_type_tape:
							*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_TAPE;
							break;
						case DCB_type_cdrom:
							*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_CD_ROM;
							break;
						case DCB_type_floppy:
							*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_DISK;
							break;
						}
					}
				}
				else
					*(DEVICE_TYPE*)pOutRequest = FILE_DEVICE_DISK;
				
				ntStatus = STATUS_SUCCESS;
			}
		}
		break;
		// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_DISK_GET_SECTOR_SIZE:
		{
			if (pOutRequest == NULL || OutRequestSize < sizeof(USHORT))
			{
				ntStatus = STATUS_BUFFER_TOO_SMALL;
				DbPrint(DC_LLDISKIO, DL_INFO, ("Get sector size failed. too small otuput buffer %d (required %d)\n", OutRequestSize, sizeof(USHORT)));
			}
			else
			{
				DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - GET_SECTOR_SIZE\n"));
				*RetSize = sizeof(USHORT);
				ntStatus = STATUS_SUCCESS;
				*(USHORT*)pOutRequest = 0;
				
				if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
				{
					DCB* pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));
					if (pDCB == NULL)
						ntStatus = STATUS_OBJECT_NAME_NOT_FOUND;
					else
						*(USHORT*)pOutRequest = (USHORT) pDCB->DCB_bdd.DCB_apparent_blk_size;
				}
				else
				{
					PBDD pbdd = BlockDev_Find_Int13_Drive(PhisicalID);
					if (pbdd != NULL)
						*(USHORT*)pOutRequest = (USHORT) pbdd->BDD_Sector_Size;	//? преобразование типа можно?
					else
						ntStatus = STATUS_OBJECT_NAME_NOT_FOUND;
				}
			}
		}
		break;
// -----------------------------------------------------------------------------------------		
	case _AVPG_IOCTL_DISK_QUERYNAMES:
		{
			/*
			данные положить в pOutRequest мультистрока = "имя_1\0имя_2\0...имя_н\0\0"
			размер выходного буфера OutRequestSize
			заполненный данные имеют размер *RetSize = ?
			*/
			PBDD pbdd;
			CHAR DrvName[4];
			BYTE idx = 0;
			DWORD len;
			
			DrvName[0] = '8';
			DrvName[1] = 0;
			DrvName[2] = 'h';
			DrvName[3] = '\0';
			((BYTE*)pOutRequest)[0] = 0;
			
			pbdd = BlockDev_Get_Device_List();
			while (pbdd != 0)
			{
				DrvName[1] = '0' + idx;
				strcat(pOutRequest, DrvName);
				len = strlen(DrvName) + 1;
				*RetSize += len;
				((BYTE*)pOutRequest) = ((BYTE*)pOutRequest) +  len;

				pbdd = (PBDD) pbdd->BDD_Next;
			}

			((BYTE*)pOutRequest)[0] = '\0';
			(*RetSize)++;
			ntStatus = STATUS_SUCCESS;
		}
		break;
// -----------------------------------------------------------------------------------------		
	case _AVPG_IOCTL_DISK_ISWRITEPROTECTED:
		DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - IsWriteProtected\n"));
		
		if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
		{
			DCB* pDCB;

			pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));
			if (pDCB != 0)
			{
				PVRP pVrp = (PVRP) pDCB->DCB_cmn.DCB_vrp_ptr;

				*RetSize = sizeof(ULONG);
				*(ULONG*) pOutRequest = FALSE;

				if (pVrp != NULL)
				{
					if (pVrp->VRP_event_flags & VRP_ef_write_protected)
						*(ULONG*) pOutRequest = TRUE;
				}

				ntStatus = STATUS_SUCCESS;
			}
		}
		break;
	case _AVPG_IOCTL_DISK_ISPARTITION:
		DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - Ispartition\n"));
		ntStatus = STATUS_SUCCESS;
		if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
		{
			DCB* pDCB;

			pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));
			if (pDCB != 0)
			{
				PVRP pVrp = (PVRP) pDCB->DCB_cmn.DCB_vrp_ptr;

				if (pVrp != NULL)
				{
					if (!pVrp->VRP_partition_offset)
						ntStatus = STATUS_UNSUCCESSFUL;
				}
			}
		}
		break;
	case _AVPG_IOCTL_DISK_GETID:
		{
			DbPrint(DC_LLDISKIO, DL_INFO, ("DoDiskRequest - Get disk ID\n"));
			if (pOutRequest == NULL || OutRequestSize < sizeof(DISK_ID_INFO))
			{
				ntStatus = STATUS_BUFFER_TOO_SMALL;
				DbPrint(DC_LLDISKIO, DL_INFO, ("Get disk ID -> too small output buffer %d (required %d)\n", OutRequestSize, sizeof(DISK_ID_INFO)));
			}
			else
			{
				BYTE disk_number = -1;
				PDISK_ID_INFO pInfoRet = (PDISK_ID_INFO) pOutRequest;
				
				memset(pInfoRet, 0, sizeof(DISK_ID_INFO));

				if ((LogicalID >= 'A') && (LogicalID <= 'Z'))
				{
					// this is logical drive
					DCB* pDCB;
					
					pDCB = GetDCB4LogDrive((BYTE)(LogicalID - 'A'));

					if (pDCB != 0)
					{
						disk_number = pDCB->DCB_bdd.DCB_BDD_Int_13h_Number - 0x80;
						pInfoRet->m_PartitionName[0] = (WCHAR) LogicalID;
					}
				}
				else
				{
					disk_number = PhisicalID - 0x80;
				}

				if (disk_number != -1)
					ntStatus = ReadDrivePortsInWin9X(disk_number, pInfoRet);
			}
		}
		break;
	default:
		DbPrint(DC_LLDISKIO, DL_WARNING, ("Unknown request\n"));
		DbgBreakPoint();
		break;
	}

	DbPrint(DC_LLDISKIO, DL_SPAM, ("DoDiskRequest - result %#x\n", ntStatus));	

	return ntStatus;
}

BOOLEAN
LL_IsLogicalMediaChanged(BYTE chDrive)
{
	DCB* pDCB;

	pDCB = GetDCB4LogDrive(chDrive);
	if (pDCB != 0)
	{
//		if (pDCB->DCB_cmn.DCB_device_flags & (/*DCB_DEV_UNCERTAIN_MEDIA | */DCB_DEV_MEDIA_CHANGED))
//			return TRUE;
		{
			PVRP pVrp = (PVRP) pDCB->DCB_cmn.DCB_vrp_ptr;
			if (pVrp != NULL)
			{
				if (pVrp->VRP_event_flags & (VRP_ef_media_uncertain | VRP_ef_media_changed))
					return TRUE;
			}
		}
	}
	else
	{
		DbgBreakPoint();
	}

	return FALSE;
}

ULONG LL_GetDeviceFlag(BYTE nDrive)
{
	DCB* pDCB;

	pDCB = GetDCB4LogDrive(nDrive);
	if (pDCB != 0)
		return pDCB->DCB_cmn.DCB_device_flags & 0xf;

	return 0;
}