
typedef struct _DISK_DEV_LIST {
	struct _DISK_DEV_LIST *Next;
	PDRIVER_OBJECT	Drv;		// Hooked driver
	PDRIVER_DISPATCH *DiskDevDispArray;
} DISK_DEV_LIST, *PDISK_DEV_LIST;

PDISK_DEV_LIST DiskDevList=NULL;
FAST_MUTEX	DiskDevListMutex;

NTSTATUS DiskDevDispatch(IN PDEVICE_OBJECT HookDevice,IN PIRP Irp)
{
PIO_STACK_LOCATION		currentIrpStack=IoGetCurrentIrpStackLocation(Irp);
ULONG						ioControlCode;
VERDICT						Verdict;
NTSTATUS					ntStatus = STATUS_SUCCESS;
PDISK_DEV_LIST				DevEntry;
ULONG						Function;
ANSI_STRING					fullPathName;
BOOLEAN						FPNAllocated=FALSE;
ULONG						actualLen;
PCHAR						Action;
ULONG						ParamVal = 0;
LARGE_INTEGER				ByteOffset;
POBJECT_NAME_INFORMATION	DevName=NULL;
KIRQL						CurrIrql;

BOOLEAN bInvisible;

ULONG SectorSize = 0;
ULONG DataLength = 0;

ULONG ReqDataSize;
PVOID RequestData;
PSINGLE_PARAM pSingleParam;
PFILTER_EVENT_PARAM pParam;

	ByteOffset.LowPart = 0;
	ByteOffset.HighPart = 0;

	fullPathName.Buffer=NULL;
	CurrIrql = KeGetCurrentIrql();
	if (CurrIrql < DISPATCH_LEVEL) {
			
		ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN + 1 + sizeof(SINGLE_PARAM) + 
			sizeof(ULONG) + sizeof(SINGLE_PARAM) + sizeof(LARGE_INTEGER);
		
		ReqDataSize += sizeof (SINGLE_PARAM) + __SID_LENGTH;
		RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
		
		pParam = (PFILTER_EVENT_PARAM)RequestData;
		if (pParam == NULL) {
			DbPrint(DC_DISK,DL_ERROR, ("Allocate buffer in DiskDevDispatch for RequestData failed\n"));
		} else {

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_DISK, 0, 0, PreProcessing, 3);

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			
		//	pParam->ProcName[0] = 0;
		//	GetProcName(pParam->ProcName,NULL);//Irp????

			if(CurrIrql<DISPATCH_LEVEL) {
				if(HookDevice->Flags & DO_DEVICE_HAS_NAME)
				{
					if(DevName = ExAllocatePoolWithTag(NonPagedPool,MAXPATHLEN*sizeof(WCHAR),'DSeB'))
					{
						if(NT_SUCCESS(ObQueryNameString(HookDevice,DevName,MAXPATHLEN*sizeof(WCHAR),&actualLen)))
						{
							if(fullPathName.Buffer = ExAllocatePoolWithTag(NonPagedPool,MAXPATHLEN,'DSeB'))
							{
								FPNAllocated=TRUE;
								fullPathName.Length=0;
								fullPathName.MaximumLength=MAXPATHLEN;
								RtlUnicodeStringToAnsiString(&fullPathName,&DevName->Name,FALSE);
							}
						}
					}
				}
				else 
				{
					if(fullPathName.Buffer=ExAllocatePoolWithTag(NonPagedPool,MAXPATHLEN,'DSeB'))
					{
						FPNAllocated=TRUE;
						fullPathName.Length=10;
						fullPathName.MaximumLength=MAXPATHLEN;
						sprintf(fullPathName.Buffer,">%08x<",HookDevice);
					}
				}
			}

			pParam->FunctionMj = currentIrpStack->MajorFunction;
			if(currentIrpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL || 
				currentIrpStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL)
			{
				ioControlCode = IoGetFunctionCodeFromCtlCode(currentIrpStack->Parameters.DeviceIoControl.IoControlCode);//(currentIrpStack->Parameters.DeviceIoControl.IoControlCode & 0x1ffc)>>2;
				if(ioControlCode > sizeof(NTDiskDeviceIOCTLName)/sizeof(PVOID))
				{
					DbPrint(DC_DISK,DL_SPAM, ("!!! Disk_IOCTL 0x%x(0x%x) %s\n",
						currentIrpStack->Parameters.DeviceIoControl.IoControlCode, ioControlCode, 
						fullPathName.Buffer == NULL ? UnknownStr : fullPathName.Buffer));
					ioControlCode=0;
				}
				pParam->FunctionMi = ioControlCode;
			}
			else if(currentIrpStack->MajorFunction==IRP_MJ_READ || currentIrpStack->MajorFunction==IRP_MJ_WRITE)
			{
				SectorSize = currentIrpStack->DeviceObject->SectorSize == 0 ? 512 : currentIrpStack->DeviceObject->SectorSize;

				ParamVal=(ULONG)(*(__int64*)(&currentIrpStack->Parameters.Read.ByteOffset)/SectorSize);
				ByteOffset = currentIrpStack->Parameters.Read.ByteOffset;
				DataLength = currentIrpStack->Parameters.Read.Length;
				DbPrint(DC_DISK,DL_SPAM, ("%s %s Sec=%u Len=%u\n", NTMajorFunctionName[currentIrpStack->MajorFunction], 
					fullPathName.Buffer == NULL ? UnknownStr : fullPathName.Buffer, ParamVal, DataLength));
				
				pParam->FunctionMi = currentIrpStack->MinorFunction;
			} else 
				pParam->FunctionMi = currentIrpStack->MinorFunction;
		#ifdef __DBG__
				Action=ExAllocatePoolWithTag(NonPagedPool,MY_PAGE_SIZE,'DSeB');
				if(Action) {
					DbPrint(DC_DISK,DL_INFO, ("%s %s (IRQL=%d)\n",
						NTGetFunctionStr(Action, pParam->HookID, pParam->FunctionMj, pParam->FunctionMi), 
						fullPathName.Buffer == NULL ? UnknownStr : fullPathName.Buffer, CurrIrql));
					ExFreePool(Action);
				}
		#endif //__DBG__

		#ifdef __DBG__
		//		if(IoIsOperationSynchronous(Irp)==FALSE)
		//			DbPrint(DC_DISK,DL_NOTIFY,("^^^^^^^^^  Async operation !!!!!!!!!\n"));
		#endif //__DBG__

			if (IsNeedFilterEventAsyncChk(pParam->HookID, pParam->FunctionMj, pParam->FunctionMi,Irp, &bInvisible) == TRUE) {
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
				sprintf(pSingleParam->ParamValue, "%s", fullPathName.Buffer);	//сюда имя девайса
				pSingleParam->ParamSize = strlen(pSingleParam->ParamValue) + 1;
				
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_LARGEINTEGER(pSingleParam, _PARAM_OBJECT_BYTEOFFSET, ByteOffset);

				//+ ----------------------------------------------------------------------------------------
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DATALEN, DataLength);
				
				// -----------------------------------------------------------------------------------------
				// reserve place for sid
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_SID(pSingleParam);
				pParam->ParamsCount++;
				// end reserve place for sid
				// -----------------------------------------------------------------------------------------
				
				Verdict = FilterEvent(pParam, NULL);
				if(!_PASS_VERDICT(Verdict)) {
					if(Verdict == Verdict_Kill) {
						DbPrint(DC_DISK,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
						KillCurrentProcess();
					} else if(Verdict == Verdict_Discard) {
						DbPrint(DC_DISK,DL_NOTIFY, ("Discard disk operation\n"/*, pSingleParam->ParamValue*/));
					}
					ntStatus = STATUS_ACCESS_DENIED;
				}
			}
			ExFreePool(RequestData);
		}
	}

	if (ntStatus != STATUS_ACCESS_DENIED)
	{
		DevEntry=DiskDevList;
		while(DevEntry) {
			if(DevEntry->Drv==HookDevice->DriverObject)
				break;
			DevEntry=DevEntry->Next;
		}
		
		if(DevEntry)
			ntStatus=(DevEntry->DiskDevDispArray[currentIrpStack->MajorFunction])(HookDevice,Irp);
		else {
			DbPrint(DC_DISK,DL_WARNING, ("!!! DiskDevDispatch. Device 0x%x (Drv=0x%x) not found in hooked list\n",HookDevice,HookDevice->DriverObject));
			ntStatus=Irp->IoStatus.Status=STATUS_NO_SUCH_DEVICE;
			Irp->IoStatus.Information=0;
			IoCompleteRequest(Irp,IO_NO_INCREMENT);
		}
	}
	if (DevName != NULL)
		ExFreePool(DevName);
	if (fullPathName.Buffer != NULL)
		ExFreePool(fullPathName.Buffer);
	
	return ntStatus;
}

/**********************************************************************************************/

VOID FindDiskDev(PDRIVER_OBJECT pDrv, PVOID pUserValue)
{
PDEVICE_OBJECT		pDev,tmp;
NTSTATUS					refStatus;
PDISK_DEV_LIST		NewDevEntry;
ULONG							i;
BOOLEAN						Disk;
	pDev=pDrv->DeviceObject;
	Disk=FALSE;
	while(pDev) {
		refStatus=ObReferenceObjectByPointer(pDev,STANDARD_RIGHTS_REQUIRED,NULL,KernelMode);
		if(!NT_SUCCESS(refStatus)){
			DbPrint(DC_DISK,DL_ERROR, ("FindDiskDev ObReferenceObjectByPointer 0x%x failed, status 0x%x\n",pDev, refStatus));
		}
		if(pDev->DeviceType==FILE_DEVICE_DISK) {//!! probably FILE_DEVICE_CD_ROM && FILE_DEVICE_DVD & somthing else
			Disk=TRUE;
			DbPrint(DC_DISK,DL_INFO, ("Drv=0x%x Device=0x%x Type=%s \n",pDrv, pDev, DbgGetDevTypeName(pDev->DeviceType)));
		}
		tmp=pDev;
		pDev=pDev->NextDevice;
		if(NT_SUCCESS(refStatus)){
			ObDereferenceObject(tmp);
		}
	}
	if(Disk) {
		NewDevEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(DISK_DEV_LIST),'dSeB');
		if(NewDevEntry) {
			NewDevEntry->DiskDevDispArray=ExAllocatePoolWithTag(NonPagedPool,sizeof(PDRIVER_DISPATCH)*(IRP_MJ_MAXIMUM_FUNCTION+1),'dSeB');
			if(NewDevEntry->DiskDevDispArray) {
				memcpy(NewDevEntry->DiskDevDispArray,pDrv->MajorFunction,sizeof(PDRIVER_DISPATCH)*(IRP_MJ_MAXIMUM_FUNCTION+1));
				NewDevEntry->Drv=pDrv;
				for(i=0;i<=IRP_MJ_MAXIMUM_FUNCTION;i++) {
					if(pDrv->MajorFunction[i]) {
						pDrv->MajorFunction[i]=DiskDevDispatch;
					} else {
						DbPrint(DC_DISK,DL_WARNING, ("Disk Drv 0x%x Empty func %s\n",pDrv,NTMajorFunctionName[i]));
					}
				}
				DbPrint(DC_DISK,DL_INFO, ("Disk Drv 0x%x hooked\n",pDrv));
				ExAcquireFastMutex(&DiskDevListMutex);
				NewDevEntry->Next=DiskDevList;
				DiskDevList=NewDevEntry;
				ExReleaseFastMutex(&DiskDevListMutex);
			} else {
				ExFreePool(NewDevEntry);
				DbPrint(DC_DISK,DL_ERROR, ("No memory for IRP_MJ_ Table for disk device\n"));
			}
		} else {
			DbPrint(DC_DISK,DL_ERROR, ("No memory for NewDiskDevEntry\n"));
		}
	}
}

VOID UnHookDisk(VOID)
{
PDISK_DEV_LIST	DevEntry;
	DbPrint(DC_DISK,DL_NOTIFY, ("UnHookDisk\n"));
	ExAcquireFastMutex(&DiskDevListMutex);
	while(DiskDevList) {
		DevEntry=DiskDevList;
		memcpy(DevEntry->Drv->MajorFunction,DevEntry->DiskDevDispArray,sizeof(PDRIVER_DISPATCH)*(IRP_MJ_MAXIMUM_FUNCTION+1));
		DbPrint(DC_DISK,DL_INFO, ("Disk Drv 0x%x unhooked\n",DevEntry->Drv));
		DiskDevList=DiskDevList->Next;
		ExFreePool(DevEntry->DiskDevDispArray);
		ExFreePool(DevEntry);
	}
	ExReleaseFastMutex(&DiskDevListMutex);
}

BOOLEAN DiskHooked=FALSE;

BOOLEAN DiskHook(VOID)
{
	if(UnloadInProgress) {
		DbPrint(DC_DISK,DL_WARNING, ("DiskHook when UnloadInProgress!\n"));
		return FALSE;
	}
	if(!DiskHooked) {
		DbPrint(DC_DISK,DL_NOTIFY, ("DiskHook!\n"));
		ExInitializeFastMutex(&DiskDevListMutex); 
		if(NT_SUCCESS(EnumDrivers(FindDiskDev, NULL))) {
			DiskHooked=TRUE;
		} else {
			ExDestroyFastMutex(&DiskDevListMutex);
			return FALSE;
		}
	}
	return TRUE;
}

BOOLEAN DiskUnHook(VOID)
{
	if(DiskHooked) {
		UnHookDisk();
		DiskHooked=FALSE;
		ExDestroyFastMutex(&DiskDevListMutex);
	}
	return TRUE;
}
