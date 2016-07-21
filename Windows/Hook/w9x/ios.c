/************************************* IOS ********************************************************/
DeviceService_THUNK IOSThunk;
VOID (*PrevIOSHookProc)(VOID);

VOID __stdcall IOSHookProc(PDSFRAME pDS)
{
PCHAR			VolumeName;
#ifdef __DBG__
PCHAR			Action;
#endif
VERDICT			Verdict;
ULONG			ParamVal=0;
PIOR			pIOR=(PIOR)(pDS->RESI);
PDCB			pDCB=(PDCB)(pDS->REDI);

	BYTE		*RequestData;
	PSINGLE_PARAM pSingleParam;
	PFILTER_EVENT_PARAM pParam;
	ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN + 1;

	RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
	if (RequestData != NULL) {
		pParam = (PFILTER_EVENT_PARAM) RequestData;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_IOS, pIOR->IOR_func, 0, PreProcessing, 1);

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
		VolumeName = pSingleParam->ParamValue;

		if(pIOR->IOR_vol_designtr<='Z'-'A')
			sprintf(VolumeName,"%c:",pIOR->IOR_vol_designtr+'A');
		else
			sprintf(VolumeName,"HD%d",pIOR->IOR_vol_designtr-0x80);
		if(pIOR->IOR_func==IOR_READ || pIOR->IOR_func==IOR_WRITE || pIOR->IOR_func==IOR_WRITEV) {
			ParamVal=pIOR->IOR_start_addr[0];
		} else if(pIOR->IOR_func==IOR_GEN_IOCTL && pIOR->IOR_ioctl_function==0x440d)
			ParamVal=pIOR->IOR_ioctl_control_param & 0xff;
	#ifdef __DBG__
		Action=HeapAllocate(MAX_MSG_LEN,0);
		if(Action) {
			DbPrint(DC_DISK,DL_INFO, ("%s %s (%s) %s\n",GetProcName(pParam->ProcName, NULL),VxDGetFunctionStr(Action,pParam->HookID, pParam->FunctionMj, pParam->FunctionMi,ParamVal),pIOR->IOR_flags & IORF_SYNC_COMMAND?"Sync":"Async",VolumeName));
			HeapFree(Action,0);
		} else {
			DbPrint(DC_DISK,DL_INFO, ("%s %s %s\n",GetProcName(pParam->ProcName, NULL),"--NO-MEM--",VolumeName));
		}
	#endif //__DBG__
		pSingleParam->ParamSize = strlen(VolumeName) + 1;
		Verdict = FilterEvent(pParam, NULL);
		if(!_PASS_VERDICT(Verdict)) {
			pIOR->IOR_status=IORS_CANCELED;
			if(Verdict == Verdict_Kill) {
				DbPrint(DC_DISK,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
				KillCurrentProcess();
			} else if(Verdict == Verdict_Discard) {
				DbPrint(DC_DISK,DL_NOTIFY, ("Discard %s\n",VolumeName));
			}
			if(!(pIOR->IOR_flags & IORF_SYNC_COMMAND) && pIOR->IOR_callback) {
				DbPrint(DC_DISK,DL_INFO, ("Callback at %x\n",pIOR->IOR_callback));
				pIOR->IOR_callback();//pIOR->IOR_req_req_handle);
			}
			ExFreePool(RequestData);
			return;
		}
		ExFreePool(RequestData);
	}

	_asm mov esi, pIOR;
	_asm mov edi, pDCB;
	PrevIOSHookProc();
}

BOOLEAN IOSHooked=FALSE;

BOOLEAN IOSHook(VOID)
{
	if(!IOSHooked) {
		if((PrevIOSHookProc=Hook_Device_Service(__IOS_SendCommand,IOSHookProc, &IOSThunk))) {
			DbPrint(DC_DISK,DL_IMPORTANT, ("IOS hooked. Previos IOSHook is %x\n", PrevIOSHookProc));
			IOSHooked=TRUE;
			return TRUE;
		} else 
			return FALSE;
	} else {
		DbPrint(DC_DISK,DL_WARNING, ("!!! IOSHook call when it already hooked\n"));
	}
	return TRUE;
}

BOOLEAN IOSUnHook(VOID)
{
NTSTATUS Status;
	if(IOSHooked) {
		Status=Unhook_Device_Service(__IOS_SendCommand,IOSHookProc, &IOSThunk);
		DbPrint(DC_DISK, DL_IMPORTANT, ("IOS unhooked. Status=%x\n",Status));
		IOSHooked=FALSE;
	}
	return TRUE;
}

