#include "..\HOOK\PIDExternalAPI.H"
#include "..\HOOK\klpf_api.h"
#include "..\HOOK\klmc_api.h"
#include "..\HOOK\minikav_api.h"

ULONG gHashFuncVersion = 2;	// 1- md5, 2 - smart fidbox2

#define bool char
#include "..\hook\klick_api.h"

PWCHAR pwcKLPID_NAME= L"\\Device\\"KLPID_NAME;
PWCHAR pwcKLPF_NAME= L"\\Device\\"KLPF_NAME;
PWCHAR pwcKLMC_NAME= L"\\Device\\"KLMC_NAME;

PWCHAR pwcMINIKAV_NAME= L"\\Device\\"MINIKAV_NAME;

PWCHAR pwcKLICK_NAME = L"\\Device\\"KLICK_NAME;

typedef struct _tSEARCH_DEV
{
	NTSTATUS	m_RetStatus;
	PCWSTR		m_pwchName;
	BOOLEAN		m_bSuccess;
	
	ULONG		IoControlCode;
	PVOID		InputBuffer;
	ULONG		InputBufferLength;
	PVOID		OutputBuffer;
	ULONG		OutputBufferLength;
	PULONG		pRetSize;
} SEARCH_DEV, *PSEARCH_DEV;

NTSTATUS DrvDeviceIoControl(PDEVICE_OBJECT Device, ULONG IoControlCode, PVOID InData, ULONG InDataSize, PVOID OutData, ULONG OutDataSize, PULONG Returned)
{
	PIRP				Irp;
	KEVENT				Event;
	IO_STATUS_BLOCK		ioStatus;
	NTSTATUS			ntStatus;

	PIO_STACK_LOCATION IrpS;                                           
	
	KeInitializeEvent(&Event, NotificationEvent, FALSE);
	Irp = IoBuildDeviceIoControlRequest(IoControlCode, Device, InData, InDataSize, OutData, OutDataSize, FALSE, &Event, &ioStatus);	
	if (Irp)
	{
		ntStatus = IoCallDriver (Device, Irp);
		if(ntStatus == STATUS_PENDING)
		{
			KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
			ntStatus = ioStatus.Status;
		}
	}
	else
	{
		ntStatus = STATUS_UNSUCCESSFUL;
		DbPrint(DC_FWALL, DL_ERROR, ("IoBuildDeviceIoControlRequest failed\n"));
	}
	
	return ntStatus;
}


VOID Alternate_FindkDev(PDRIVER_OBJECT pDrv, PVOID pUserValue)
{
	PDEVICE_OBJECT		pDev, tmp;
	NTSTATUS			refStatus;
	PWCHAR				pDeviceName;

	PSEARCH_DEV			pSearchDev = (PSEARCH_DEV) pUserValue;
	
	pDev = pDrv->DeviceObject;
	while(pDev && NT_SUCCESS(pSearchDev->m_RetStatus) && (pSearchDev->m_bSuccess == FALSE))
	{
		refStatus = ObReferenceObjectByPointer(pDev,STANDARD_RIGHTS_REQUIRED,NULL,KernelMode);
		if(!NT_SUCCESS(refStatus)){
			DbPrint(DC_DISK,DL_ERROR, ("FindDiskDev ObReferenceObjectByPointer 0x%x failed, status 0x%x\n",pDev, refStatus));
		}
		
		if (pDev->Flags & DO_DEVICE_HAS_NAME)
		{
			if ((pDev->AttachedDevice != NULL) && (pDev->AttachedDevice->Flags  & DO_DEVICE_HAS_NAME))
			{
				// skip this device because attached has name
			}
			else
			{
				pDeviceName = ExAllocatePoolWithTag(NonPagedPool, MAXPATHLEN * sizeof(WCHAR), 'WboS');
				if (pDeviceName != NULL)
				{
					if (GetDeviceName(pDev, pDeviceName, MAXPATHLEN * sizeof(WCHAR)) != NULL)
					{
						ULONG len = wcslen(pDeviceName);
						if (len == wcslen(pSearchDev->m_pwchName))
						{
							if (!wcsncmp(pDeviceName, pSearchDev->m_pwchName, len))
							{
								pSearchDev->m_bSuccess = TRUE;

								pSearchDev->m_RetStatus = DrvDeviceIoControl(pDev, pSearchDev->IoControlCode, 
									pSearchDev->InputBuffer, pSearchDev->InputBufferLength, 
									pSearchDev->OutputBuffer, pSearchDev->OutputBufferLength, pSearchDev->pRetSize);
								if (NT_ERROR(pSearchDev->m_RetStatus))
								{
									DbPrint(DC_FWALL,DL_ERROR, ("Io request failed. error %#x\n", pSearchDev->m_RetStatus));
								}
							}
						}
					}
					
					ExFreePool(pDeviceName);
				}
			}
		}

		tmp = pDev;
		pDev = pDev->NextDevice;
		if(NT_SUCCESS(refStatus))
		{
			ObDereferenceObject(tmp);
		}
	}
}

#ifndef __SendDevIoCtl2DrvByName_Defined
#define __SendDevIoCtl2DrvByName_Defined

NTSTATUS SendDevIoCtl2DrvByName(ULONG IoControlCode, PVOID InputBuffer,ULONG InputBufferLength,PVOID OutputBuffer,ULONG OutputBufferLength, PCWSTR pDrvName, ULONG* pRetSize)
{
HANDLE						hExtDrv;
OBJECT_ATTRIBUTES	ObjAttr;
UNICODE_STRING		us;
IO_STATUS_BLOCK		ioStatus;
PDEVICE_OBJECT		DevObj;
PFILE_OBJECT			fileObject;
NTSTATUS					ntStatus;
ULONG							BuildNum;
KEVENT						Event;
PIRP							Irp;
PIO_STACK_LOCATION irpSp;
	RtlInitUnicodeString(&us, pDrvName);
	InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntStatus=ZwCreateFile(&hExtDrv,SYNCHRONIZE|FILE_ANY_ACCESS,&ObjAttr,&ioStatus,NULL,0,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_OPEN,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
	if(NT_SUCCESS(ntStatus)) {
		ntStatus=ObReferenceObjectByHandle(hExtDrv,FILE_READ_DATA,NULL,KernelMode, (PVOID*) &fileObject,NULL);
		if(NT_SUCCESS(ntStatus)) {
			if((DevObj=IoGetRelatedDeviceObject(fileObject))!=NULL) {
				KeInitializeEvent(&Event,NotificationEvent,FALSE);
				Irp=IoBuildDeviceIoControlRequest(IoControlCode,DevObj,InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,FALSE,&Event,&ioStatus);
				if(Irp!=NULL) {
					irpSp=IoGetNextIrpStackLocation(Irp);
					irpSp->FileObject = fileObject;
					ntStatus=IoCallDriver(DevObj,Irp);
					if(ntStatus==STATUS_PENDING) {
						KeWaitForSingleObject(&Event,Executive,KernelMode,FALSE,(PLARGE_INTEGER)NULL);
						ntStatus = ioStatus.Status;
					}
					if (pRetSize != NULL)
						*pRetSize = ioStatus.Information;
				} else {
					DbPrint(DC_FWALL,DL_ERROR, ("IoBuildDeviceIoControlRequest failed\n"));
					ntStatus=STATUS_UNSUCCESSFUL;
				}
			} else {
					DbPrint(DC_FWALL,DL_ERROR, ("IoGetRelatedDeviceObject %S failed \n",us.Buffer));
					ntStatus=STATUS_UNSUCCESSFUL;
			}
			ObDereferenceObject(fileObject);
		} else {
			DbPrint(DC_FWALL,DL_ERROR, ("ObReferenceObjectByHandle %S failed status=%x\n",us.Buffer,ntStatus));
		}
		ZwClose(hExtDrv);
	} else {
		SEARCH_DEV SearchDev;
		
		SearchDev.m_RetStatus = STATUS_SUCCESS;
		SearchDev.m_bSuccess = FALSE;
		SearchDev.m_pwchName = pDrvName;

		SearchDev.IoControlCode = IoControlCode;
		SearchDev.InputBuffer = InputBuffer;
		SearchDev.InputBufferLength = InputBufferLength;
		SearchDev.OutputBuffer = OutputBuffer;
		SearchDev.OutputBufferLength = OutputBufferLength;
		SearchDev.pRetSize = pRetSize;

		DbPrint(DC_FWALL,DL_ERROR, ("ZwCreateFile %S failed status=%x. Try alternate\n", us.Buffer, ntStatus));
		
		EnumDrivers(Alternate_FindkDev, &SearchDev);
		ntStatus = SearchDev.m_RetStatus;
		
		if (SearchDev.m_bSuccess == TRUE)
		{
			DbPrint(DC_FWALL, DL_IMPORTANT, ("Alternate search successfull\n"));
		}
		else
		{
			DbPrint(DC_FWALL, DL_IMPORTANT, ("Alternate search unsuccessfull\n"));
			ntStatus = STATUS_NOT_FOUND;
		}
	}
	return ntStatus;
}

#endif //__SendDevIoCtl2DrvByName_Defined

// -----------------------------------------------------------------------------------------
BOOLEAN GeneralStart_ExternalDrv(ULONG Ioctl_GetVersion, ULONG Ioctl_StartFiltering, PCWSTR pwcDrvNAME, ULONG ApiVersion, ULONG HookID, BOOLEAN *pbStarted)
{
	EXT_START_FILTERING	StartFiltering;
	ULONG ExtDrvApiVersion = 0;
	
	if(*pbStarted == TRUE)
	{
		DbPrint(DC_FWALL,DL_WARNING, ("%S is already started.\n", pwcDrvNAME));
		return TRUE;
	}
	
	if(NT_SUCCESS(SendDevIoCtl2DrvByName(Ioctl_GetVersion, NULL, 0, &ExtDrvApiVersion, sizeof(ExtDrvApiVersion), pwcDrvNAME, NULL)))
	{
		if(ExtDrvApiVersion == ApiVersion)
		{
			StartFiltering.m_HookID = HookID;
			InitStartFiltering(&StartFiltering);
			if(NT_SUCCESS(SendDevIoCtl2DrvByName(Ioctl_StartFiltering, &StartFiltering, sizeof(StartFiltering), NULL,0, pwcDrvNAME, NULL)))
			{
				DbPrint(DC_FWALL,DL_NOTIFY, ("%S Start succes.\n", pwcDrvNAME));
				*pbStarted = TRUE;
				return TRUE;
			}
		}
		else
		{
			DbPrint(DC_FWALL,DL_ERROR, ("%S/"AVPGNAME" API versions different. %d!=%d\n", pwcDrvNAME, ExtDrvApiVersion, ApiVersion));
		}
	}
	else
	{
		DbPrint(DC_FWALL,DL_ERROR, ("SendDevIoCtl2DrvByName failed.\n"));
	}
	
	return FALSE;
}

BOOLEAN GeneralStop_ExternalDrv(ULONG Ioctl_StopFiltering, PCWSTR pwcDrvNAME, ULONG HookID, BOOLEAN *pbStarted)
{
	if(*pbStarted == FALSE)
	{
		DbPrint(DC_FWALL,DL_WARNING, ("%S is not started.\n", pwcDrvNAME));
		return FALSE;
	}
	
	if(NT_SUCCESS(SendDevIoCtl2DrvByName(Ioctl_StopFiltering, &HookID, sizeof(HookID), NULL, 0, pwcDrvNAME, NULL)))
	{
		DbPrint(DC_FWALL,DL_NOTIFY, ("%S succes.\n", pwcDrvNAME));
		*pbStarted = FALSE;
		return TRUE;
	}

	return FALSE;
}
// -----------------------------------------------------------------------------------------
BOOLEAN KLMCStarted = FALSE;
BOOLEAN KLMCStart()
{
	return GeneralStart_ExternalDrv(KLMC_GET_VERSION, KLMC_START_FILTER, pwcKLMC_NAME, KLMC_BUILDNUM, FLTTYPE_MCHECK, &KLMCStarted);
}

BOOLEAN KLMCStop()
{
	return GeneralStop_ExternalDrv(KLMC_STOP_FILTER, pwcKLMC_NAME, FLTTYPE_MCHECK, &KLMCStarted);
}

// -----------------------------------------------------------------------------------------
BOOLEAN IDSStarted = FALSE;
BOOLEAN IDSStart()
{
	return GeneralStart_ExternalDrv(PID_IOCTL_GETVERSION, PID_IOCTL_START_FILTER, pwcKLPID_NAME, KLPID_BUILDNUM, FLTTYPE_IDS, &IDSStarted);
}

BOOLEAN IDSStop()
{
	return GeneralStop_ExternalDrv(PID_IOCTL_STOP_FILTER, pwcKLPID_NAME, FLTTYPE_IDS, &IDSStarted);
}

// -----------------------------------------------------------------------------------------

BOOLEAN PIDStarted=FALSE;
BOOLEAN PIDStart()
{
	return GeneralStart_ExternalDrv(PID_IOCTL_GETVERSION, PID_IOCTL_START_FILTER, pwcKLPID_NAME, KLPID_BUILDNUM, FLTTYPE_PID, &PIDStarted);
}

BOOLEAN PIDStop()
{
	return GeneralStop_ExternalDrv(PID_IOCTL_STOP_FILTER, pwcKLPID_NAME, FLTTYPE_PID, &PIDStarted);
}

// -----------------------------------------------------------------------------------------

BOOLEAN KLPFStarted=FALSE;
BOOLEAN KLPFStart(VOID)
{
	return GeneralStart_ExternalDrv(KLPF_GET_VERSION, KLPF_START_FILTER, pwcKLPF_NAME, KLPF_BUILDNUM, FLTTYPE_KLPF, &KLPFStarted);
}

BOOLEAN KLPFStop(VOID)
{
	return GeneralStop_ExternalDrv(KLPF_STOP_FILTER, pwcKLPF_NAME, FLTTYPE_KLPF, &KLPFStarted);	
}

//+ ------------------------------------------------------------------------------------------

BOOLEAN MiniKav = FALSE;
BOOLEAN MiniStart(VOID)
{
	return GeneralStart_ExternalDrv(MINIKAV_GET_VERSION, MINIKAV_START_FILTER, pwcMINIKAV_NAME, MINIKAV_BUILDNUM, FLTTYPE_MINIKAV, &MiniKav);
}

BOOLEAN MiniStop(VOID)
{
	return GeneralStop_ExternalDrv(MINIKAV_STOP_FILTER, pwcMINIKAV_NAME, FLTTYPE_MINIKAV, &MiniKav);	
}

//+ ------------------------------------------------------------------------------------------

BOOLEAN Klick = FALSE;
BOOLEAN KlickStart(VOID)
{
	return GeneralStart_ExternalDrv(KLICK_GET_VERSION, KLICK_START_FILTER, pwcKLICK_NAME, KLICK_API_VERSION, FLTTYPE_KLICK, &Klick);
}

BOOLEAN KlickStop(VOID)
{
	return GeneralStop_ExternalDrv(KLICK_STOP_FILTER, pwcKLICK_NAME, FLTTYPE_KLICK, &Klick);	
}

VOID
KlickRefreshData()
{
	ULONG HookID = FLTTYPE_KLICK;
	SendDevIoCtl2DrvByName(KLICK_RESET_TRUSTED_STREAMS, &HookID, sizeof(HookID), NULL, 0, pwcKLICK_NAME, NULL);
}


// -----------------------------------------------------------------------------------------

BOOLEAN Cache(PFILE_OBJECT Fo,PLARGE_INTEGER Offset,ULONG Size)
{
	BOOLEAN MapStat;
	PVOID Bcb;
	PVOID Buff;
	MapStat=CcMapData(Fo, Offset, Size,TRUE,&Bcb,&Buff);
	DbPrint(DC_SYS, DL_INFO, ("Cache regions offset %I64d size %u - %s\n",*(__int64*)Offset,Size,MapStat?"Ok":"Fail"));
	if(MapStat)
		CcUnpinData(Bcb);
	return MapStat;
/*
						DbPrint(DC_SYS, DL_INFO, ("SystemRequest - Cache regions %d offset %I64d size %d\n", cou, 
							pPrefetch->m_Regions[cou].m_RegionOffset, pPrefetch->m_Regions[cou].m_RegionSize));
						CcReadAhead(pFileObject, (PLARGE_INTEGER) &pPrefetch->m_Regions[cou].m_RegionOffset, pPrefetch->m_Regions[cou].m_RegionSize);						
						{
							LARGE_INTEGER Interval;
							*(__int64*)&Interval=-((__int64)1000000L);
							KeDelayExecutionThread(KernelMode,TRUE,&Interval);
						}
*/}

//+ ----------------------------------------------------------------------------------------
NTSTATUS FixBox_GetObjectId(HANDLE hFile, PFIDBOX_REQUEST_DATA_EX pFidData);

NTSTATUS DoSystemRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, 
						 ULONG *pRetSize, BOOLEAN bAppFound)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;
		
	switch (pInRequest->m_IOCTL)
	{
		// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_PROCESS_SUSPEND:
	case _AVPG_IOCTL_PROCESS_RESUME:
	case _AVPG_IOCTL_PROCESS_DISCARDOPS:
		if (pInRequest->m_BufferSize != sizeof(ULONG))
			ntStatus = STATUS_INVALID_PARAMETER;
		else
		{
			if (bAppFound)
				ntStatus = CheckProc_ProcessFunc(*(ULONG*)pInRequest->m_Buffer, pInRequest->m_IOCTL);
		}
		break;
	case _AVPG_IOCTL_STAT_DISK:
		if (OutRequestSize < (sizeof(unsigned __int64) * 2))
			ntStatus = STATUS_BUFFER_TOO_SMALL;
		else
		{
			unsigned __int64* pu = (unsigned __int64*) pOutRequest;
			*pRetSize = sizeof(unsigned __int64) * 2;
			*(pu + 1) = 0;
			while (*(pu + 1) != g_TW)
				*(pu + 1) = g_TW;
			*pu = GetCurTimeFast();
			
			ntStatus = STATUS_SUCCESS;
		}
		break;
	case _AVPG_IOCTL_FIDBOX_SET:
		if (bAppFound)
		{
			if (pInRequest->m_BufferSize > sizeof(FIDBOX_REQUEST_DATA_EX))
			{
				ULONG ReqSize = pInRequest->m_BufferSize - sizeof(FIDBOX_REQUEST_DATA_EX);
				if (ReqSize > 0x1000)
					ntStatus = STATUS_NOT_SUPPORTED;
				else
				{
					PFIDBOX_REQUEST_DATA_EX pFidData = (PFIDBOX_REQUEST_DATA_EX) pInRequest->m_Buffer;

					ntStatus = FidBox_SetData(pFidData, ReqSize);
				}
			}

			*pRetSize = 0;
		}
		break;
	case _AVPG_IOCTL_FIDBOX_SET_BH:
		if (bAppFound)
		{
			ntStatus = STATUS_UNSUCCESSFUL;
			if (pInRequest->m_BufferSize > sizeof(FIDBOX_REQUEST_SET))
			{
				PFIDBOX_REQUEST_SET pFidSet = (PFIDBOX_REQUEST_SET) pInRequest->m_Buffer;
				ULONG ReqSize = pInRequest->m_BufferSize - sizeof(FIDBOX_REQUEST_SET);

				PFIDBOX_REQUEST_DATA_EX pFidData = ExAllocatePoolWithTag (
					PagedPool,
					sizeof(FIDBOX_REQUEST_DATA_EX) + pFidSet->m_BufferSize,
					'dbOs'
					);
				if (pFidData)
				{
					ntStatus = FixBox_GetObjectId( (HANDLE) pFidSet->m_hFile, pFidData );
					if (NT_SUCCESS( ntStatus ))
					{
						memcpy( pFidData->m_Buffer, pFidSet->m_Buffer, pFidSet->m_BufferSize );
						ntStatus = FidBox_SetData( pFidData, pFidSet->m_BufferSize );
					}
					ExFreePool( pFidData );
				}
			}

			*pRetSize = 0;
		}
		break;
	case _AVPG_IOCTL_FIDBOX_DISABLE:
		ntStatus = FidBox_Disable();
		*pRetSize = 0;
		break;
	case _AVPG_IOCTL_FIDBOX_GET:
		if (bAppFound)
		{
			ULONG ReqSize;
			PFIDBOX_REQUEST_GET pFidGet = (PFIDBOX_REQUEST_GET) pInRequest->m_Buffer;
			PFIDBOX_REQUEST_DATA_EX pFidData = (PFIDBOX_REQUEST_DATA_EX) pOutRequest;

			if (pInRequest->m_BufferSize >= sizeof(FIDBOX_REQUEST_GET))
			{
				if (OutRequestSize >= sizeof(FIDBOX_REQUEST_DATA_EX))
				{
					ReqSize = OutRequestSize - sizeof(FIDBOX_REQUEST_DATA_EX);

					ntStatus = FidBox_GetData(pFidGet, pFidData, ReqSize, pRetSize);
					if (!NT_SUCCESS(ntStatus))
						*pRetSize = 0;
				}
			}
		}
		break;
		
	case _AVPG_IOCTL_FIDBOX2_GET:
		{
			PFIDBOX2_REQUEST_GET	pFidGet = (PFIDBOX2_REQUEST_GET) pInRequest->m_Buffer;
			PFIDBOX2_REQUEST_DATA	pFidData = (PFIDBOX2_REQUEST_DATA) pOutRequest;
			
			if (pInRequest->m_BufferSize >= sizeof(FIDBOX2_REQUEST_GET) &&
				OutRequestSize >= sizeof(FIDBOX2_REQUEST_DATA))
			{
				FIDBOX_REQUEST_GET	Tmp_FidboxRequest = {pFidGet->m_hFile};
				FIDBOX_REQUEST_DATA Tmp_FidboxResult;
				ULONG				Tmp_RetSize;

				memset(&Tmp_FidboxResult, 0, sizeof(Tmp_FidboxResult));
				
				ntStatus = FidBox2_GetData(&Tmp_FidboxRequest, &Tmp_FidboxResult, &Tmp_RetSize);
				*pRetSize = 0;
				if (NT_SUCCESS(ntStatus))
				{
					
					IO_STATUS_BLOCK			IoSB;
					FILE_BASIC_INFORMATION	BasicInfo;
					
					pFidData->m_DataFormatId = Tmp_FidboxResult.m_DataFormatId;
					memcpy(pFidData->m_VolumeID, Tmp_FidboxResult.m_VolumeID, sizeof(pFidData->m_VolumeID));
					memcpy(pFidData->m_FileID, Tmp_FidboxResult.m_FileID, sizeof(pFidData->m_FileID));
					pFidData->m_Counter = *((PULONG)Tmp_FidboxResult.m_Buffer);
					pFidData->m_ModificationTime.QuadPart = 0;
					
					// get last modification time
					if (NT_SUCCESS(ZwQueryInformationFile(
						(HANDLE)Tmp_FidboxRequest.m_hFile,
						&IoSB,
						&BasicInfo,
						sizeof(BasicInfo),
						FileBasicInformation
						)
						))
						pFidData->m_ModificationTime = BasicInfo.LastWriteTime;
					
					*pRetSize = sizeof(FIDBOX2_REQUEST_DATA);
					
					DbPrint(DC_SYS, DL_IMPORTANT, ("FIDBOX2: len: %d, fmtid: 0x%x, volid: 0x%x...0x%x, fid: 0x%x...0x%x, cnt: 0x%x, ft: 0x%x-0x%x\n", *pRetSize,
						pFidData->m_DataFormatId,
						pFidData->m_VolumeID[0], pFidData->m_VolumeID[sizeof(pFidData->m_VolumeID) - 1],
						pFidData->m_FileID[0], pFidData->m_FileID[sizeof(pFidData->m_FileID) - 1],
						pFidData->m_Counter,
						pFidData->m_ModificationTime.HighPart, pFidData->m_ModificationTime.LowPart));
				}
				
			}
		}
		break;

	case _AVPG_IOCTL_GET_PREFETCHINFO:
		ntStatus = SysGetPrefetch(pOutRequest, OutRequestSize, pRetSize);
		break;
	
	case _AVPG_IOCTL_KEYLOGGERS_CHECK:
		KeyLoggers_CommonCheck();
		ntStatus = STATUS_SUCCESS;
		break;

	case _AVPG_IOCTL_CHECK_ACTIVE_SECT:
		if (pInRequest->m_BufferSize != sizeof(LARGE_INTEGER))
			ntStatus = STATUS_INVALID_PARAMETER;
		else
			ntStatus = SysFindInSectionHashes(*(LARGE_INTEGER*)pInRequest->m_Buffer);
		break;
	case _AVPG_IOCTL_GET_PIDLIST:
		if (bAppFound)
			ntStatus = SysFillPIDList(pOutRequest, OutRequestSize, pRetSize);
		break;
	case _AVPG_IOCTL_GET_SECTIONLIST:
		if (bAppFound)
		{
			if (sizeof(ULONG) == pInRequest->m_BufferSize)
				ntStatus = SysFillSectionList(*(ULONG*) pInRequest->m_Buffer, pOutRequest, OutRequestSize, pRetSize);
			else
				ntStatus = SysFillSectionList(0, pOutRequest, OutRequestSize, pRetSize);
		}
		break;
	case _AVPG_IOCTL_GET_DRVLIST:
		if (bAppFound)
		{
			ntStatus = SysFillDrvList(pOutRequest, OutRequestSize, pRetSize);
		}
		break;

	case _AVPG_IOCTL_GET_APP_PATH:
		if (sizeof(ULONG) == pInRequest->m_BufferSize)
			ntStatus = SysGetAppPath(*(ULONG*) pInRequest->m_Buffer, pOutRequest, OutRequestSize, pRetSize);

		break;
		
// -----------------------------------------------------------------------------------------		
	default:
		DbPrint(DC_SYS, DL_WARNING, ("Unknown request\n"));
		DbgBreakPoint();
		break;
	}

	DbPrint(DC_SYS, DL_SPAM, ("DoSystemRequest - result %#x\n", ntStatus));	

	return ntStatus;
}
//////////////////////////////////////////////////////////////////////////
//+ ------------------------------------------------------------------------------------------
static NTSTATUS irpCompletionTmp(
    PDEVICE_OBJECT  deviceObject,
    PIRP            irp,
    PVOID           context
    )
{
    *irp->UserIosb = irp->IoStatus;         // Copy status information to
                                            // the user
    KeSetEvent(irp->UserEvent, 0, FALSE);   // Signal event
    IoFreeIrp(irp);                         // Free IRP
    return STATUS_MORE_PROCESSING_REQUIRED; // Tell the I/O manager to stop
}

NTSTATUS IrpRead(PDEVICE_OBJECT pDevice, PFILE_OBJECT pFileObject, PVOID pBuffer, size_t nSize, LARGE_INTEGER Offset, size_t* pnBytesRead)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PMDL pMdl;
	PIRP                irp;
	KEVENT              event;
	IO_STATUS_BLOCK		UserIosb;

	DbPrint(DC_IOCTL, DL_IMPORTANT, ("DRead on fo 0x%x, size 0x%x, pos(l) 0x%x\n", pFileObject, nSize, Offset.LowPart));
	
	if (STATUS_WRONG_VOLUME == pFileObject->FinalStatus)
	{
		return STATUS_WRONG_VOLUME;
	}

	irp = IoAllocateIrp(pDevice->StackSize, FALSE);
	if (irp)
	{
		pMdl = IoAllocateMdl(pBuffer, nSize, FALSE, FALSE, irp);
		if (!pMdl)
			IoFreeIrp(irp); 
		else
		{
			PIO_STACK_LOCATION  irpSp;

			MmBuildMdlForNonPagedPool(pMdl);

			KeInitializeEvent(&event, NotificationEvent, FALSE);
			
			irp->Flags = IRP_READ_OPERATION/* | IRP_NOCACHE*/;	//will crash sometimes with no_cache
			irp->MdlAddress = pMdl;
			irp->Tail.Overlay.Thread = PsGetCurrentThread();
			irp->Tail.Overlay.OriginalFileObject = pFileObject;
			irp->RequestorMode = KernelMode;
			irp->UserEvent = &event;
			irp->UserIosb = &UserIosb;

			irpSp = IoGetNextIrpStackLocation(irp);
			irpSp->MajorFunction = IRP_MJ_READ;
			irpSp->MinorFunction = 0;
			irpSp->DeviceObject = pDevice;
			irpSp->FileObject = pFileObject;
			irpSp->Parameters.Read.Length = nSize;
			irpSp->Parameters.Read.Key = 0;
			irpSp->Parameters.Read.ByteOffset = Offset;

			IoSetCompletionRoutine(irp, irpCompletionTmp, 0, TRUE, TRUE, TRUE);

			IoCallDriver(pDevice, irp);
			KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

			ntStatus = UserIosb.Status;
			if (NT_SUCCESS(ntStatus))
				*pnBytesRead = UserIosb.Information;
			else
			{
				//DbgBreakPoint();
			}
	
			IoFreeMdl(pMdl);
		}
	}

	return ntStatus;
}

void ResolveGranularity(DWORD block_size, LARGE_INTEGER* poffset, DWORD reqsize, DWORD* poffset_align, size_t* pnew_size)
{
	DWORD block_count;

	*poffset_align = ((DWORD)(poffset->QuadPart)) % block_size;
	block_count = (*poffset_align + reqsize + (block_size - 1)) / block_size;

	*pnew_size = block_count * block_size;	
}

NTSTATUS
DoExternalDrvRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutRequest,
	ULONG OutRequestSize,
	ULONG *pRetSize
	)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	BOOLEAN bAppFound = TRUE;

	PQCLIENT pClient = NULL;

	if (FLTTYPE_EMPTY == pInRequest->m_DrvID)
	{
		*pRetSize = 0;
		if (_AVPG_IOCTL_DISK_READ == pInRequest->m_IOCTL)
		{
			PDIRECT_FILE_REQUEST pReq = (PDIRECT_FILE_REQUEST) pInRequest->m_Buffer;
			if (
				InRequestSize >= sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DIRECT_FILE_REQUEST)
				&& pInRequest->m_Buffer
				&& pReq->m_RequestBufferSize
				&& pReq->m_hFile
				&& pOutRequest
				&& OutRequestSize
				&& pReq->m_RequestBufferSize <= OutRequestSize
				)
			{
				PFILE_OBJECT fileObject;

				HANDLE hFile = pReq->m_hFile;
				ntStatus = ObReferenceObjectByHandle(hFile, FILE_READ_DATA, NULL, KernelMode, (PVOID*) &fileObject, NULL);
				if (NT_SUCCESS(ntStatus))
				{
					PDEVICE_OBJECT pDevice;
					USHORT SectorSize;
					LARGE_INTEGER Offset = pReq->m_Offset;
					DWORD size = pReq->m_RequestBufferSize;
					DWORD offset_allign;
					size_t new_size;
					size_t retbytes;

					PVOID pBuf = NULL;
					if (Offset.HighPart != -1)
					{

						pDevice = fileObject->DeviceObject;
						if (fileObject->Vpb && fileObject->Vpb->DeviceObject)
							pDevice = fileObject->Vpb->DeviceObject;

						Disk_GetSectorSize(pDevice, &SectorSize);

						ResolveGranularity(SectorSize, &Offset, size, &offset_allign, &new_size);

						Offset.QuadPart -= offset_allign;

						pBuf = ExAllocatePoolWithTag(NonPagedPool, new_size, 'Dbos');
					}
					if (pBuf)
					{
						AddSkipLockItem(fileObject);
						ntStatus = IrpRead(pDevice, fileObject, pBuf, new_size, Offset, &retbytes);
						if (NT_SUCCESS(ntStatus))
						{
							if (retbytes > offset_allign)
							{
								size_t len = min(OutRequestSize, retbytes - offset_allign);
							
								memcpy(pOutRequest, (char*) pBuf + offset_allign, len);
								*pRetSize = len;
								ntStatus = STATUS_SUCCESS;
							}
							else
							{
								ntStatus = STATUS_UNSUCCESSFUL;
							}
						}
						else
						{
							DbgBreakPoint();
						}

						DelSkipLockItem(fileObject);

						ExFreePool(pBuf);
					}

					ObDereferenceObject(fileObject);
				}
			}
			else
			{
				ntStatus = STATUS_INVALID_PARAMETER;
				DbgBreakPoint();
			}
		}
		else if (_AVPG_IOCTL_FILE_OPEN == pInRequest->m_IOCTL)
		{
			*pRetSize = 0;
			if (*NtBuildNumber >= 2195)
			{
				if (!pOutRequest || (OutRequestSize != (sizeof(HANDLE))))
				{
					ntStatus = STATUS_INVALID_PARAMETER;
					DbgBreakPoint();
				}
				else
				{
					HANDLE hTmp;
					ntStatus = DoDirectOpen(&hTmp, (PWCHAR) pInRequest->m_Buffer, (USHORT) (pInRequest->m_BufferSize / sizeof(WCHAR)));
					if (NT_SUCCESS(ntStatus))
					{
						*(HANDLE*)pOutRequest = hTmp;
						*pRetSize = sizeof(HANDLE);
					}
				}
			}
			else
			{
				ntStatus = STATUS_NOT_SUPPORTED;
			}
		}
		else
		{
			DbgBreakPoint();
			ntStatus = STATUS_NOT_SUPPORTED;
		}

		return ntStatus;
	}
	
	pClient = AcquireClient(pInRequest->m_AppID);
	if (pClient)
		ReleaseClient(pClient);
	else
	{
		bAppFound = FALSE;
		if (pInRequest->m_DrvID != FLTTYPE_SYSTEM)
		{
			DbPrint(DC_IOCTL, DL_NOTIFY, ("Request for DrvId %d. Ioctl %#x (%d) failed - can't find AppID %d\n", pInRequest->m_DrvID, pInRequest->m_IOCTL, pInRequest->m_IOCTL, pInRequest->m_AppID));
			return STATUS_NOT_FOUND;
		}
	}

	DbPrint(DC_IOCTL, DL_NOTIFY, ("Request for DrvId %d. Ioctl %#x (%d)\n", pInRequest->m_DrvID, pInRequest->m_IOCTL, pInRequest->m_IOCTL));

	switch (pInRequest->m_DrvID)
	{
	case FLTTYPE_PID:
		ntStatus = SendDevIoCtl2DrvByName(pInRequest->m_IOCTL, pInRequest->m_Buffer, pInRequest->m_BufferSize, pOutRequest, OutRequestSize, pwcKLPID_NAME, pRetSize);
		break;

	case FLTTYPE_KLPF:
		ntStatus = SendDevIoCtl2DrvByName(pInRequest->m_IOCTL, pInRequest->m_Buffer, pInRequest->m_BufferSize, pOutRequest, OutRequestSize, pwcKLPF_NAME, pRetSize);
		break;

	case FLTTYPE_DISK:
		ntStatus = DoDiskRequest(pInRequest, pOutRequest, OutRequestSize, pRetSize);
		break;

	case FLTTYPE_MCHECK:
		ntStatus = SendDevIoCtl2DrvByName(pInRequest->m_IOCTL, pInRequest->m_Buffer, pInRequest->m_BufferSize, pOutRequest, OutRequestSize, pwcKLMC_NAME, pRetSize);
		break;

	case FLTTYPE_SYSTEM:
		ntStatus = DoSystemRequest(pInRequest, pOutRequest, OutRequestSize, pRetSize, bAppFound);
		break;

	default:
		DbPrint(DC_IOCTL, DL_ERROR, ("Unknown DrvId %d. Ioctl %#x (%d)\n", pInRequest->m_DrvID, pInRequest->m_IOCTL, pInRequest->m_IOCTL));
		break;
	}

	return ntStatus;
}
