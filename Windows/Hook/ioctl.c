
// predefine
BOOLEAN GetDriverInfo(PADDITIONALSTATE pAddState);
extern ULONG gHashFuncVersion;

/********************************** DeviceControl ***************************************************/
#ifndef IoGetFunctionCodeFromCtlCode
#define IoGetFunctionCodeFromCtlCode( ControlCode ) (( ControlCode >> 2) & 0x00000FFF )
#endif

BOOLEAN
CheckEventParams (
	PFILTER_EVENT_PARAM pEvent,
	ULONG EventSize
	);

#ifndef _HOOK_VXD_
PVOID
CopyUserBuffer (
	PVOID pContext,
	ULONG Size
	);
#endif //_HOOK_VXD_

ULONG
HOOKDeviceControl (
	IN PVOID _InputBuffer,
	IN ULONG InputBufferLength,
	OUT PVOID _OutputBuffer,
	IN ULONG OutputBufferLength,
	IN ULONG IoControlCode,
	OUT PULONG IoStatusInfo,//Это то что вернет DeviceIOControl в параметре pBytesReturned
	OUT PULONG IoStatusStatus, //Это то что вернет DeviceIOControl
	BOOLEAN bNew,
	BOOLEAN bBufCheck
	)
{
#ifdef _HOOK_VXD_
	PVOID InputBuffer = _InputBuffer;
	PVOID OutputBuffer = _OutputBuffer;

	*IoStatusInfo=0;
	*IoStatusStatus = STATUS_SUCCESS;
#else
	PVOID InputBuffer = NULL;
	PVOID OutputBuffer = NULL;
	KPROCESSOR_MODE PreviousMode = ExGetPreviousMode();

	*IoStatusInfo=0;
	*IoStatusStatus = STATUS_SUCCESS;

	if (InputBufferLength > 0xffff)
		InputBufferLength = 0;

	if (!InputBufferLength && _InputBuffer)
		_InputBuffer = NULL;
	
	if (!OutputBufferLength && _OutputBuffer)
		_OutputBuffer = NULL;

	if(!bBufCheck)
		PreviousMode = KernelMode;

	if (UserMode == PreviousMode)
	{
		InputBuffer = CopyUserBuffer( _InputBuffer, InputBufferLength );
		if (_OutputBuffer && !MmIsAddressValid( _OutputBuffer ))
		{
			*IoStatusStatus = STATUS_INVALID_PARAMETER;
			return FALSE;
		}

		if (_OutputBuffer)
		{
			OutputBuffer = ExAllocatePoolWithTag( PagedPool, OutputBufferLength, 'zbos' );
			if (!OutputBuffer)
			{
				*IoStatusStatus = STATUS_UNSUCCESSFUL;
				DbgBreakPoint();

				if (InputBuffer && (InputBuffer != _InputBuffer))
					ExFreePool( InputBuffer );

				return FALSE;
			}
			
			__try
			{
				memcpy( OutputBuffer, _OutputBuffer, OutputBufferLength );
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				DbgBreakPoint();
				
				if (InputBuffer && (InputBuffer != _InputBuffer))
					ExFreePool( InputBuffer );

				ExFreePool( OutputBuffer );

				*IoStatusStatus = STATUS_INVALID_PARAMETER;
				return FALSE;
			}
		}
	}
	else
	{
		InputBuffer = _InputBuffer;
		OutputBuffer = _OutputBuffer;
	}
#endif // vxd

	switch(IoControlCode) {
	case IOCTLHOOK_GetVersion:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_GetVersion\n"));
		if(OutputBufferLength!=sizeof(ULONG) || OutputBuffer==NULL) {
			goto err;
		}
		if (bNew)
			*(ULONG*)OutputBuffer=HOOK_INTERFACE_NUM;
		else
			*(ULONG*)OutputBuffer = 11;
		*IoStatusInfo = sizeof(ULONG);
		break;
	case IOCTLHOOK_GetDriverInfo:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_GetDriverInfo\n"));
		if((OutputBuffer == NULL) || (OutputBufferLength!=sizeof(ADDITIONALSTATE)))
			goto err;
		if(!GetDriverInfo((PADDITIONALSTATE)OutputBuffer))
			goto err;
		*IoStatusInfo = sizeof(ADDITIONALSTATE);
		break;
	case IOCTLHOOK_NewDebugInfoLevel:
#ifdef __DBG__
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_NewDebugInfoLevel\n"));
		if(InputBufferLength==sizeof(ULONG) && InputBuffer!=NULL) {
			DebLev = *(ULONG*)InputBuffer;
		} else if(OutputBufferLength==sizeof(ULONG) && OutputBuffer!=NULL) {
			*(ULONG*)OutputBuffer = DebLev;
			*IoStatusInfo = sizeof(ULONG);
		} else
#endif
			goto err;
		break;
	case IOCTLHOOK_NewDebugCategoryMask:
#ifdef __DBG__
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_NewDebugCategoryMask\n"));
		if(InputBufferLength==sizeof(ULONG) && InputBuffer!=NULL) {
			DebCat = *(ULONG*)InputBuffer;
		} else if(OutputBufferLength==sizeof(ULONG) && OutputBuffer!=NULL) {
			*(ULONG*)OutputBuffer = DebCat;
		} else
#endif
			goto err;
		break;
	case IOCTLHOOK_ThreadSubProcesssing:
		DbPrint(DC_IOCTL,DL_SPAM, ("IOCTLHOOK_ThreadSubProcesssing\n"));
		*IoStatusStatus=STATUS_NOT_IMPLEMENTED;
/*
		if(InputBufferLength < sizeof(THREAD_PAGE_SET) || InputBuffer == NULL)
			goto err;
		{
			PTHREAD_PAGE_SET pPageSet = (PTHREAD_PAGE_SET) InputBuffer;
			if(pPageSet->m_PageCount) {
				if(InputBufferLength!=sizeof(THREAD_PAGE_SET)+sizeof(PAGE_REGION)*pPageSet->m_PageCount)
					goto err;
				if(!AddTSPRegion(pPageSet))
					goto err;
			} else {
				DelTSPOwner((PVOID)PsGetCurrentThreadId());
			}
		}
*/
		break;
	case IOCTLHOOK_TSPv2:
		DbPrint(DC_IOCTL,DL_SPAM, ("IOCTLHOOK_TSPv2\n"));
		if(InputBufferLength < sizeof(TSPRS_COMMON) || InputBuffer == NULL)
			goto err;
		switch(((PTSPRS_COMMON)InputBuffer)->ReqClass) {
		case TSPRC_Register:
			if(OutputBufferLength!=sizeof(ORIG_HANDLE) || OutputBuffer==NULL)
				goto err;
			*IoStatusStatus=TSPRegister(((PTSPRS_REGISTER)InputBuffer)->MaxSize,(ORIG_HANDLE*)OutputBuffer);
			*IoStatusInfo = sizeof(ORIG_HANDLE);
			break;
		case TSPRC_Unregister:
			*IoStatusStatus=TSPUnregister(((PTSPRS_COMMON)InputBuffer)->hOrig);
			break;
		case TSPRC_Lock:
			if(InputBufferLength!=sizeof(TSPRS_LOCK))
				goto err;
			*IoStatusStatus=TSPLock((PTSPRS_LOCK)InputBuffer);
			break;
		case TSPRC_ThreadEnter:
			*IoStatusStatus=TSPThreadEnter(((PTSPRS_COMMON)InputBuffer)->hOrig);
			break;
		case TSPRC_ThreadLeave:
			*IoStatusStatus=TSPThreadLeave(((PTSPRS_COMMON)InputBuffer)->hOrig);
			break;
		case TSPRC_SetMinShadow:
			TSPSetMinShadow(((PTSPRS_REGISTER)InputBuffer)->MaxSize);
			*IoStatusStatus = STATUS_SUCCESS;
		default:
			goto err;
		}
		break;
	case IOCTLHOOK_RegistrApp:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_RegistrApp\n"));
				if (bNew)
		{
			if(InputBufferLength!=sizeof(APP_REGISTRATION) || InputBuffer==NULL || 
				OutputBufferLength!=sizeof(CLIENT_INFO) || OutputBuffer==NULL || 
				!RegisterApplication((PAPP_REGISTRATION) InputBuffer, (PCLIENT_INFO) OutputBuffer)) {
				goto err;
			}
			
			((PAPP_REGISTRATION) _InputBuffer)->m_AppID = ((PCLIENT_INFO) OutputBuffer)->m_AppID;

			*IoStatusInfo = sizeof(CLIENT_INFO);
		}
		else
		{
			CLIENT_INFO ClientInfo;
			APP_REGISTRATION NewAppReg;

			ULONG AppRegSize = sizeof(APP_REGISTRATION) - sizeof(ULONG);

			if(InputBufferLength!=AppRegSize || InputBuffer==NULL || OutputBufferLength!=AppRegSize || OutputBuffer==NULL)
				goto err;

			memcpy(((BYTE*)&NewAppReg) + sizeof(ULONG), InputBuffer, AppRegSize);
			NewAppReg.m_InterfaceVersion = HOOK_INTERFACE_NUM;

			 if (!RegisterApplication(&NewAppReg, &ClientInfo))
				 goto err;
			
			memcpy(OutputBuffer, (((BYTE*)&NewAppReg)) + sizeof(ULONG), AppRegSize);
			*IoStatusInfo = AppRegSize;
		}
		break;
	case IOCTLHOOK_Activity:
		DbPrint(DC_IOCTL,DL_SPAM, ("IOCTLHOOK_SetActivity\n"));
		if(InputBufferLength != sizeof(HDSTATE) || InputBuffer == NULL ||
			OutputBufferLength != sizeof(HDSTATE) || OutputBuffer == NULL) {
			goto err;
		}
		if (!ClientActivity((PHDSTATE)InputBuffer,(PHDSTATE)OutputBuffer)) {
			DbPrint(DC_IOCTL,DL_WARNING, ("Client activity: status operation failed!\n"));
			*IoStatusStatus=STATUS_UNSUCCESSFUL;
			break;
		}
		
		*IoStatusInfo = sizeof(HDSTATE);
		break;
	case IOCTLHOOK_SetFlowTimeout:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_SetFlowTimeout\n"));
		if((InputBuffer == NULL) || (InputBufferLength != sizeof(FLOWTIMEOUT))) {
			goto err;
		}
		ClientSetFlowTimeout((PFLOWTIMEOUT)InputBuffer);
		break;
	case IOCTLHOOK_GetEvent:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_GetEvent\n"));
		if(InputBuffer==NULL || OutputBuffer==NULL || InputBufferLength != sizeof(GET_EVENT) || OutputBufferLength < sizeof(EVENT_TRANSMIT)) {
			goto err;
		}
		if (GetEventForClient((PGET_EVENT) InputBuffer, (PEVENT_TRANSMIT) OutputBuffer, &OutputBufferLength))
			*IoStatusInfo=OutputBufferLength;
		else
			*IoStatusInfo = 0; // no free event
		break;
	case IOCTLHOOK_SkipEvent:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_SkipEvent\n"));
		if(InputBuffer==NULL || InputBufferLength != sizeof(ULONG)) {
			goto err;
		}
		*IoStatusStatus = SkipEventForClient(*((ULONG*)InputBuffer));
		break;
	case IOCTLHOOK_ImpersonateThread:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_ImpersonateEvent\n"));
		if(InputBuffer==NULL || InputBufferLength < sizeof(IMPERSONATE_REQUEST)) {
			goto err;
		}
		*IoStatusStatus = ImpersonateThreadForClient((PIMPERSONATE_REQUEST)InputBuffer);
		break;
	case IOCTLHOOK_GetEventSize:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_GetEventSize\n"));
		if(InputBuffer==NULL || OutputBuffer==NULL || InputBufferLength != sizeof(ULONG) || OutputBufferLength < sizeof(DWORD)) {
			goto err;
		}
		GetEventSizeForClient(*((ULONG*)InputBuffer), (DWORD*) OutputBuffer);
		*IoStatusInfo = OutputBufferLength;
		break;
	case IOCTLHOOK_YeldEvent:
		DbPrint(DC_IOCTL,DL_SPAM, ("IOCTLHOOK_YieldEvent\n"));
		if(InputBuffer==NULL || InputBufferLength < sizeof(YELD_EVENT)) {
			goto err;
		}
		*IoStatusInfo = OutputBufferLength;
		*IoStatusStatus = YeldEventForClient((PYELD_EVENT)InputBuffer, InputBufferLength, OutputBuffer, IoStatusInfo);
		break;
	case IOCTLHOOK_ResetCache:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_ResetCache\n"));
		if(InputBufferLength!=sizeof(ULONG) || InputBuffer==NULL)  {
			goto err;
		}
//!!!!!!!!!!!!!!!!!
		*IoStatusStatus = ClientResetCache((*(ULONG*)InputBuffer));
		break;
	case IOCTLHOOK_SetVerdict:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_SetVerdict\n"));
		if(InputBufferLength!=sizeof(SET_VERDICT) || InputBuffer==NULL)  {
			goto err;
		}
		*IoStatusStatus = EventSetVerdict((PSET_VERDICT)InputBuffer);
		break;
	case IOCTLHOOK_FilterEvent:
		DbPrint(DC_IOCTL,DL_SPAM, ("IOCTLHOOK_FilterEvent\n"));
		*IoStatusInfo = STATUS_INVALID_PARAMETER;
		*IoStatusInfo = 0;

		if(InputBuffer
			&& OutputBuffer
			&& InputBufferLength >= sizeof(FILTER_EVENT_PARAM)
			&& OutputBufferLength == sizeof(VERDICT))
		{
			if (CheckEventParams( (PFILTER_EVENT_PARAM) InputBuffer, InputBufferLength) )
			{
				*(VERDICT*)OutputBuffer = FilterEvent((PFILTER_EVENT_PARAM) InputBuffer, NULL);
				*IoStatusInfo = OutputBufferLength;
			}
		}

		break;
	case IOCTLHOOK_FiltersCtl:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_FiltersCtl\n"));
			
		if(InputBuffer == NULL || OutputBuffer == NULL || 
			InputBufferLength < sizeof(FILTER_TRANSMIT) || OutputBufferLength < sizeof(FILTER_TRANSMIT)) {
			goto err;
		}

		*IoStatusStatus = FilterTransmit((PFILTER_TRANSMIT) InputBuffer, (PFILTER_TRANSMIT)OutputBuffer, OutputBufferLength);
		if (*IoStatusStatus == STATUS_SUCCESS)
			*IoStatusInfo = OutputBufferLength;

		break;
	case IOCTLHOOK_InvisibleThreadOperations:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_InvisibleThreadOperations\n"));
		
		if(InputBuffer==NULL || OutputBuffer==NULL || InputBufferLength != sizeof(INVISIBLE_TRANSMIT) || OutputBufferLength != sizeof(INVISIBLE_TRANSMIT)) {
			goto err;
		}
		*IoStatusStatus = InvisibleTransmit((PINVISIBLE_TRANSMIT)InputBuffer, (PINVISIBLE_TRANSMIT)OutputBuffer);
		if (*IoStatusStatus == STATUS_SUCCESS)
			*IoStatusInfo = sizeof(INVISIBLE_TRANSMIT);
		break;

		// -----------------------------------------------------------------------------------------
	case IOCTLHOOK_COMMONMEMORY:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_COMMONMEMORY\n"));
		*IoStatusStatus=STATUS_NOT_IMPLEMENTED;
/*
		if(InputBuffer==NULL || OutputBuffer==NULL || InputBufferLength != sizeof(COMMON_MEMORY) || OutputBufferLength != sizeof(COMMON_MEMORY))
			goto err;
		else {
			*IoStatusStatus = DoClientCommonMemoryOp((PCOMMON_MEMORY)InputBuffer, (PCOMMON_MEMORY)OutputBuffer);
			*IoStatusInfo = OutputBufferLength;
			goto err;
		}
*/
		break;
	case IOCTLHOOK_External_Drv_Request:
		if(InputBuffer==NULL || InputBufferLength < sizeof(EXTERNAL_DRV_REQUEST))
			goto err;
		else
		{
			*IoStatusStatus = DoExternalDrvRequest( 
				(PEXTERNAL_DRV_REQUEST)InputBuffer,
				InputBufferLength,
				OutputBuffer,
				OutputBufferLength,
				IoStatusInfo
				);
			if (*IoStatusInfo > OutputBufferLength)
			{
				DbPrint(DC_IOCTL,DL_WARNING, ("External request - return buffer to big! try to return - %d (allowed %d)\n", *IoStatusInfo, OutputBufferLength));
				*IoStatusStatus = STATUS_INVALID_BLOCK_LENGTH;
			}
		}
		break;
	case IOCTLHOOK_EventObject_Request:
		{
			if(InputBuffer==NULL || InputBufferLength < sizeof(EVENT_OBJECT_REQUEST))
				goto err;

			*IoStatusStatus = DoEventRequest((PEVENT_OBJECT_REQUEST) InputBuffer, OutputBuffer, OutputBufferLength, IoStatusInfo);
			if (*IoStatusStatus == STATUS_SUCCESS)
			{
				if (*IoStatusInfo > OutputBufferLength)
				{
					DbPrint(DC_IOCTL,DL_WARNING, ("Event request - return buffer to big! try to return - %d (allowed %d)\n", *IoStatusInfo, OutputBufferLength));
					*IoStatusInfo = OutputBufferLength;
				}
			}
		}
		break;
#ifdef _HOOK_VXD_
	case IOCTLHOOK_DiskIO:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_DiskIO\n"));
		if(InputBuffer==NULL || InputBufferLength!=sizeof(DISK_IO)) {
			goto err;
		}
		if(((PDISK_IO)InputBuffer)->Func==0x25 || ((PDISK_IO)InputBuffer)->Func==0x13) {
			if(OutputBuffer==NULL || OutputBufferLength!=((PDISK_IO)(InputBuffer))->SizeOfBuffer) {
				goto err;
			}
			if(!_DiskIO((PDISK_IO)InputBuffer,OutputBuffer))
				goto err;
			*IoStatusInfo=OutputBufferLength;
		} else //if((PDISK_IO)InputBuffer->Func==0x26 || (PDISK_IO)InputBuffer->Func==0x14)) 
		{
			*IoStatusInfo=OutputBufferLength;
			if(!_DiskIO((PDISK_IO)InputBuffer,(PBYTE)&(((PDISK_IO)InputBuffer)->Buffer)))
				goto err;
		} //else
//			goto err;
		break;
#endif //_HOOK_VXD_*/
	case IOCTLHOOK_LOCKMEMORY:
		DbPrint(DC_IOCTL,DL_SPAM, ("IOCTLHOOK_LOCKMEMORY\n"));
		*IoStatusStatus=STATUS_NOT_IMPLEMENTED;
/*
		if(InputBufferLength != sizeof(LOCKMEMREGION) || InputBuffer == NULL)
			goto err;
		{
			PLOCKMEMREGION reg;
			reg=(PLOCKMEMREGION)InputBuffer;
			if(reg->Lock) {
				if(!MyLockMemory(reg->m_Addr,reg->m_RegionSize))
					goto err;
			} else {
				if(!MyUnLockMemory(reg->m_Addr))
					goto err;
			}
		}
*/
		break;
	case IOCTLHOOK_RELOADSETTINGS:
		DbPrint(DC_IOCTL,DL_NOTIFY, ("IOCTLHOOK_RELOADSETTINGS\n"));
		if(InputBuffer==NULL || InputBufferLength!=sizeof(ULONG))
		{
			goto err;
		}

		*IoStatusInfo = 0;
		*IoStatusStatus = ReloadSettings(*(ULONG*)InputBuffer);
		break;
	case IOCTLHOOK_SPECFILEREQUEST:
		DbPrint(DC_IOCTL, DL_NOTIFY, ("IOCTLHOOK_SEPCFILEREQUEST\n"));
		if(InputBuffer==NULL || InputBufferLength < sizeof(SPECFILEFUNC_REQUEST))
			goto err;

		*IoStatusStatus = SpecFileRequest((PSPECFILEFUNC_REQUEST) InputBuffer, InputBufferLength, 
			OutputBuffer, OutputBufferLength, IoStatusInfo);
		break;

#ifdef _HOOK_VXD_
#else
	case IOCTLHOOK_GETHASHFUNCVER:
		*IoStatusStatus = STATUS_INVALID_PARAMETER;
		*IoStatusInfo = 0;

		if(OutputBuffer && (OutputBufferLength == sizeof(ULONG)))
		{
			__try
			{
				*(ULONG*)OutputBuffer = gHashFuncVersion;

				*IoStatusInfo = sizeof(ULONG);
				*IoStatusStatus = STATUS_SUCCESS;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				*IoStatusInfo = 0;
				*IoStatusStatus = STATUS_UNSUCCESSFUL;
			};
		}

		break;
#endif // _HOOK_VXD_
	default:
		{
			goto err;
		}
	}

#ifdef _HOOK_VXD_
#else
	if (*IoStatusStatus == STATUS_SUCCESS)
	{
		if (_OutputBuffer && (_OutputBuffer != OutputBuffer))
		{
			__try
			{
				memcpy( _OutputBuffer, OutputBuffer, OutputBufferLength );
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				DbgBreakPoint();
				*IoStatusStatus = STATUS_INVALID_PARAMETER;
				*IoStatusInfo = 0;
			}
		}
	}

	if (InputBuffer && (InputBuffer != _InputBuffer))
		ExFreePool( InputBuffer );

	if (OutputBuffer && (OutputBuffer != _OutputBuffer))
		ExFreePool( OutputBuffer );
#endif // _HOOK_VXD_
		
	if (*IoStatusStatus == STATUS_SUCCESS)
		return TRUE;
	else
		return FALSE;
err:
	DbPrint(DC_IOCTL ,DL_ERROR, ("IOCTLHOOK_0x%x (%d) - ERROR (ibs %d, obs %d)\n",IoControlCode,IoGetFunctionCodeFromCtlCode(IoControlCode), InputBufferLength, OutputBufferLength));
	*IoStatusStatus=STATUS_INVALID_PARAMETER;

	if (InputBuffer && (InputBuffer != _InputBuffer))
		ExFreePool( InputBuffer );

	if (OutputBuffer && (OutputBuffer != _OutputBuffer))
		ExFreePool( OutputBuffer );

	return FALSE;
}

