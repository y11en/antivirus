
/************************************* IFS ********************************************************/
ppIFSFileHookFunc			PrevIFSHookProc;

void ClearDriveFlags();
/*
// Converts an unparsed unicode path to ANSI. This is only used for
// UNC paths except for the special cases of renames and findopens.
VOID ConvertUnparsedPath(pioreq pir,PCHAR fullpathname)
{
_QWORD  result;
    UniToBCS(fullpathname, pir->ir_upath,wstrlen( pir->ir_upath ),MAXPATHLEN,BCS_WANSI,&result);
    fullpathname[result.ddLower]=0;
}

// Converts a parsed unicode path to ANSI.
VOID FilemonConvertParsedPath(int drive,path_t ppath,PCHAR fullpathname)
{
int  i = 0;
_QWORD  result;
    if(drive != 0xFF) {
        // Its a volume-based path
        fullpathname[0] = drive+'A'-1;
        fullpathname[1] = ':';
        i = 2;
    }
    fullpathname[i]=0;
    UniToBCSPath( &fullpathname[i], ppath->pp_elements, MAXPATHLEN-1, BCS_WANSI, &result );
    fullpathname[ i + result.ddLower ] = 0;
}

// This converts a mix of unparsed and parsed paths to ANSI. The 
// unparsed path is used for server/share, whereas the parsed
// path is used for the directory/file. Only UNC rename and findopen 
// use this.
VOID FilemonConvertMixedPath(pioreq pir,path_t ppath,PCHAR fullpathname)
{
int     i, slashes;
_QWORD  result;
    UniToBCS( fullpathname, pir->ir_upath, wstrlen( pir->ir_upath ),MAXPATHLEN, BCS_WANSI, &result );
    fullpathname[ result.ddLower ] = 0;
    slashes = 0;
    for( i = 0; i < result.ddLower; i++ ) {
        // We find the 4th slash: \\Server\share\...
        if( fullpathname[i] == '\\' && ++slashes == 4 ) break;
    }
    if( slashes == 4 ) {
        FilemonConvertParsedPath( 0xFF, ppath, &fullpathname[i]);
    }
}

PCHAR FilemonConvertPath(CONVERT_TYPE converttype,int drive, pioreq pir,PCHAR fullpathname)
{
	if( drive != 0xFF ) {
        // Its a volume-based path
        FilemonConvertParsedPath( drive, pir->ir_ppath, fullpathname );
    } else {
        // Its a UNC path. The parsed path doesn't include the
        // server/share, so we get that from the unparsed path.
        switch( converttype ) {
        case CONVERT_STANDARD:
            FilemonConvertUnparsedPath( pir, fullpathname );
            break;
        case CONVERT_FINDOPEN:
        case CONVERT_RENAME_SOURCE:
            FilemonConvertMixedPath( pir, pir->ir_ppath, fullpathname );
            break;
        case CONVERT_RENAME_TARGET:
            FilemonConvertMixedPath( pir, pir->ir_ppath2, fullpathname );
            break;
        }
    }
    return fullpathname;
}
*/

PCHAR ConvertPath(PCHAR fullname,int Drive,path_t ppath)
{
_QWORD			result;
	if((BYTE)Drive==(BYTE)-1) {
		//UNC resource
		*fullname=0;
		DbPrint(DC_FILE,DL_INFO,("ConvertPath. Drive = UNC resource\n"));
	} else if(Drive<'Z'-'A') {
		*fullname++=Drive+'A'-1;
		*(WORD*)fullname++=':';
	} else {
		*fullname=0;
		DbPrint(DC_FILE,DL_IMPORTANT,("ConvertPath. Wrong Drive value %d\n",Drive));
		DbgBreakPoint();
	}
	if(ppath==(void*)0xFFFFFBBB) {
		DbPrint(DC_FILE,DL_INFO,("ppath==0xFFFFFBBB !!!!!!!!!!!!!!!!!!\n"));
		;
	} else if(ppath)
	{
		UniToBCSPath(fullname, ppath->pp_elements, MAXPATHLEN, BCS_WANSI, &result);
		fullname[result.ddLower] = 0;
#ifdef __DBG__
		if(!result.ddLower) {
			DbPrint(DC_FILE,DL_INFO,("ConvertPath. UniToBCSPath convert zero bytes, flag=%d\n",result.ddUpper));
		}
#endif
	} else {
//		DbPrint(DC_FILE,DL_SPAM,("ppath==NULL. See DDK for ir_uFName\n"));
		;
	}
	return fullname;
}

PCHAR GetFullName(PCHAR fullname,int Drive,int ResType,int CodePage,pioreq pir, ULONG uNameSize)
{
pIFSFunc			enumFunc;
ifsreq			ifsr;
path_t			uniFullName;
int				retval;
BOOLEAN bOverBuf;
	*fullname=0;
	bOverBuf = FALSE;
	if(NameCacheGet(g_pCommon_NameCache, (ULONG)pir->ir_fh,fullname, uNameSize, &bOverBuf))
	{
		if (bOverBuf == TRUE)
		{
			DbPrint(DC_SYS, DL_WARNING, ("NameCacheTruncating - buffer too small in GetFullName (9x)\n"));
			DbgBreakPoint();
		}
		return fullname;
	}
	strcpy(fullname,UnknownStr);
	uniFullName=IFSMgr_GetHeap(MAXPATHLEN*sizeof(WCHAR));
	if(uniFullName) {
		memcpy(&ifsr,pir,sizeof(ifsreq));
		ifsr.ifsir.ir_flags=ENUMH_GETFILENAME;
		ifsr.ifsir.ir_ppath=uniFullName;
		enumFunc=ifsr.ifs_hndl->hf_misc->hm_func[HM_ENUMHANDLE];
		DbPrint(DC_FILE,DL_INFO, ("IFSFN_ENUMHANDLE call (addr %x) enumFunc=%x\n", *PrevIFSHookProc,enumFunc));
		retval=(*PrevIFSHookProc)(enumFunc,IFSFN_ENUMHANDLE,Drive,ResType,CodePage,(pioreq)&ifsr);
		if(retval==ERROR_SUCCESS) {
			ConvertPath(fullname,Drive,uniFullName);
			NameCacheStore(g_pCommon_NameCache, (ULONG)pir->ir_fh,fullname,strlen(fullname)+1, FALSE, '8boS');
		} else {
			DbPrint(DC_FILE,DL_WARNING, ("PrevIFSHookProc (addr %x) IFSFN_ENUMHANDLE failed, enumFunc=%x, retval=%x\n", *PrevIFSHookProc,enumFunc,retval));
		}
		IFSMgr_RetHeap((void*)uniFullName);
	} else {
		DbPrint(DC_FILE,DL_WARNING, ("IFSMgr_GetHeap failed\n"));
	}
	return fullname;
}

//+ ------------------------------------------------------------------------------------------
typedef struct _UNRECOGNIZED_DEV_LIST
{
	struct _UNRECOGNIZED_DEV_LIST *m_pNext;
	__int64 m_Expiration;
	int	m_Drive;	
} UNRECOGNIZED_DEV_LIST, *PUNRECOGNIZED_DEV_LIST;

PUNRECOGNIZED_DEV_LIST gpUnrecognizedDevices = NULL;
FAST_MUTEX	FsUnrecognizedDevListMutex;

BOOLEAN
IsInvisibleThread(ULONG ThreadID);

VOID
PlaceToDelayedQueue(int Drive)
{
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceTmp;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceEnum;
	
	if (IsInvisibleThread((ULONG)PsGetCurrentThreadId()))
		return;
	
	ExAcquireFastMutex(&FsUnrecognizedDevListMutex);

	pUnrecognizedDeviceEnum = gpUnrecognizedDevices;
	while (pUnrecognizedDeviceEnum != NULL)
	{
		if (pUnrecognizedDeviceEnum->m_Drive == Drive)
		{
			ExReleaseFastMutex(&FsUnrecognizedDevListMutex);
			return;
		}
		pUnrecognizedDeviceEnum = pUnrecognizedDeviceEnum->m_pNext;
	}

	pUnrecognizedDeviceTmp = ExAllocatePoolWithTag(NonPagedPool, sizeof(UNRECOGNIZED_DEV_LIST),'UBOS');
	if (pUnrecognizedDeviceTmp == NULL)
	{
		DbPrint(DC_FILE, DL_WARNING, ("PlaceToDelayedQueue - low resource\n"));
	}
	else
	{
		pUnrecognizedDeviceTmp->m_Drive = Drive;
		pUnrecognizedDeviceTmp->m_pNext = gpUnrecognizedDevices;
		pUnrecognizedDeviceTmp->m_Expiration = GetTimeShift(15 * 1000);
		gpUnrecognizedDevices = pUnrecognizedDeviceTmp;

		DbPrint(DC_FILE, DL_WARNING, ("PlaceToDelayedQueue - object placed to queue %d\n", Drive));
	}

	ExReleaseFastMutex(&FsUnrecognizedDevListMutex);
}

VOID
DeleteFromDelayedQueue(int Drive)
{
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceEnum;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDevicePrev = NULL;

	ExAcquireFastMutex(&FsUnrecognizedDevListMutex);

	pUnrecognizedDeviceEnum = gpUnrecognizedDevices;
	while (pUnrecognizedDeviceEnum != NULL)
	{
		if (pUnrecognizedDeviceEnum->m_Drive == Drive)
		{
			if (pUnrecognizedDevicePrev == NULL)
				gpUnrecognizedDevices = pUnrecognizedDeviceEnum->m_pNext;
			else
				pUnrecognizedDevicePrev->m_pNext = pUnrecognizedDeviceEnum->m_pNext;
			ExFreePool(pUnrecognizedDeviceEnum);
			break;
		}
		pUnrecognizedDevicePrev = pUnrecognizedDeviceEnum;
		pUnrecognizedDeviceEnum = pUnrecognizedDeviceEnum->m_pNext;
	}

	ExReleaseFastMutex(&FsUnrecognizedDevListMutex);
}

VOID
ProcessDelayedQueue()
{
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDeviceEnum;
	PUNRECOGNIZED_DEV_LIST pUnrecognizedDevicePrev = NULL;

	int DriveToCheck = -1;

	ExAcquireFastMutex(&FsUnrecognizedDevListMutex);

	pUnrecognizedDeviceEnum = gpUnrecognizedDevices;
	while (pUnrecognizedDeviceEnum != NULL)
	{
		if (IsTimeExpired(pUnrecognizedDeviceEnum->m_Expiration))
		{
			DriveToCheck = pUnrecognizedDeviceEnum->m_Drive;

			if (pUnrecognizedDevicePrev == NULL)
				gpUnrecognizedDevices = pUnrecognizedDeviceEnum->m_pNext;
			else
				pUnrecognizedDevicePrev->m_pNext = pUnrecognizedDeviceEnum->m_pNext;

			ExFreePool(pUnrecognizedDeviceEnum);
			break;
		}
		
		pUnrecognizedDevicePrev = pUnrecognizedDeviceEnum;
		pUnrecognizedDeviceEnum = pUnrecognizedDeviceEnum->m_pNext;
	}

	ExReleaseFastMutex(&FsUnrecognizedDevListMutex);

	if (DriveToCheck != -1)
	{
		PSINGLE_PARAM pSingleParam;
		PFILTER_EVENT_PARAM pParam;
		PVOID RequestData;
		ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN * 2 + 1;

		RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
		if (RequestData != NULL)
		{
			PCHAR fullPathName;
			pParam = (PFILTER_EVENT_PARAM) RequestData;

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_IFS, IFSFN_CONNECT, 0, PostProcessing, 0);
			if(IsNeedFilterEvent(pParam->HookID, pParam->FunctionMj, pParam->FunctionMi))
			{
				pParam->ParamsCount = 1;
				pSingleParam = (PSINGLE_PARAM) pParam->Params;
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
				fullPathName = pSingleParam->ParamValue;
				
				ConvertPath(fullPathName, DriveToCheck, NULL);

				pSingleParam->ParamSize = strlen(pSingleParam->ParamValue) + 1;

				DbgBreakPoint();
				FilterEvent(pParam, NULL);
			}
				
			ExFreePool(RequestData);
		}
	}
}


//!!  IFSMgr_GetLockState
int _cdecl IFSHookProc(pIFSFunc pfn, int fn, int Drive, int ResType, int CodePage, pioreq pir)
{
int				retval;
VERDICT			Verdict;
BOOLEAN			isneedflt;
#ifdef __DBG__
PCHAR		Action;
#endif

//!!CodePage?

	BOOLEAN bTime = FALSE;

	PCHAR fullPathName;
	
	PSINGLE_PARAM pSingleParam;
	PSINGLE_PARAM pSingleParamUrl;
	PFILTER_EVENT_PARAM pParam;
	PVOID RequestData;

	//! sdesa remote flag attach on url
	ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM) + (sizeof(SINGLE_PARAM) + MAXPATHLEN + 1) * 2;
	ReqDataSize += sizeof(SINGLE_PARAM) + sizeof(ULONG);	// drive flags

	RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
	if (RequestData != NULL) {
		pParam = (PFILTER_EVENT_PARAM) RequestData;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_IFS, fn, 0, PreProcessing, 2);
		switch(fn) {
		case IFSFN_CLOSE:
		case IFSFN_DIR:
		case IFSFN_DASDIO:
		case IFSFN_FILEATTRIB:
		case IFSFN_FILETIMES:
		case IFSFN_FILELOCKS:
		case IFSFN_SEARCH:
		case IFSFN_DISCONNECT:
			pParam->FunctionMi = (pir->ir_flags & 0xFF);
			break;
		case IFSFN_OPEN:
			pParam->FunctionMi =(pir->ir_flags & ACCESS_MODE_MASK);
			//ir_options ACTION_OPENEXISTING / ACTION_REPLACEEXISTING / ACTION_CREATENEW / ACTION_OPENALWAYS / ACTION_CREATEALWAYS
			break;
		default:
			pParam->FunctionMi = 0;
		}
		
		if((isneedflt=IsNeedFilterEvent(pParam->HookID,pParam->FunctionMj,pParam->FunctionMi))==FALSE) {
			if(pParam->FunctionMj==IFSFN_CLOSE && pir->ir_flags==CLOSE_FINAL)
				NameCacheFree(g_pCommon_NameCache, (ULONG)pir->ir_fh);
		} else {
			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
			fullPathName = pSingleParam->ParamValue;

			pSingleParamUrl = pSingleParam;
			
			switch(fn) {
			case IFSFN_CLOSE:
				GetFullName(fullPathName,Drive,ResType,CodePage,pir, MAXPATHLEN);
				if(pir->ir_flags==CLOSE_FINAL)
					NameCacheFree(g_pCommon_NameCache, (ULONG)pir->ir_fh);
				break;
			case IFSFN_FILETIMES:
			case IFSFN_FILELOCKS:
			case IFSFN_SEEK:
			case IFSFN_COMMIT:
			case IFSFN_HANDLEINFO:
				GetFullName(fullPathName,Drive,ResType,CodePage,pir, MAXPATHLEN);
				break;
			case IFSFN_FINDNEXT:
			case IFSFN_FCNNEXT:
				//		strcpy(fullPathName,CacheGet(pir->ir_sfn,fullPathName));
				ConvertPath(fullPathName,Drive,NULL);
				break;
			case IFSFN_FINDCLOSE:
			case IFSFN_FCNCLOSE:
				//		strcpy(fullPathName,CacheGet(pir->ir_sfn,fullPathName));
				//		CacheFree(pir->ir_sfn);
				ConvertPath(fullPathName,Drive,NULL);
				break;
			case IFSFN_DIR:
			case IFSFN_FILEATTRIB:
			case IFSFN_SEARCH:
			case IFSFN_DELETE:
			case IFSFN_FINDOPEN:
				ConvertPath(fullPathName,Drive,pir->ir_ppath);
				break;
			case IFSFN_READ:
			case IFSFN_WRITE:
				if(pir->ir_options & (R0_SWAPPER_CALL | R0_MM_READ_WRITE)) {
					DbPrint(DC_FILE,DL_INFO,("R0_SWAPPER_CALL | R0_MM_READ_WRITE Waiting unsafely.\n"));
					pParam->UnsafeValue++;
				}
				GetFullName(fullPathName,Drive,ResType,CodePage,pir, MAXPATHLEN);
				break;
			case IFSFN_OPEN:
				if(pir->ir_options & R0_SWAPPER_CALL) {
					DbPrint(DC_FILE,DL_INFO,("R0_SWAPPER_CALL Waiting unsafely.\n"));
					pParam->UnsafeValue++;
				}
				/*if (pir->ir_upath != 0) esli unrem to ne lovit command.com + copy otnositelnii put
				{
					_QWORD result;
					UniToBCS(fullPathName, pir->ir_upath, wcslen(pir->ir_upath) * sizeof(WCHAR), MAXPATHLEN, BCS_WANSI, &result);
					fullPathName[result.ddLower] = 0;
				}
				else
				{
					if (pir->ir_ppath)
						ConvertPath(fullPathName,Drive,pir->ir_ppath);
					else
					{
						// a gde vziat imia tuta?
						DbgBreakPoint();
					}
				}*/

				if (pir->ir_ppath)
					ConvertPath(fullPathName,Drive,pir->ir_ppath);
				else
				{
					if (pir->ir_upath != 0)
					{
						_QWORD result;
						UniToBCS(fullPathName, pir->ir_upath, wcslen(pir->ir_upath) * sizeof(WCHAR), MAXPATHLEN, BCS_WANSI, &result);
						fullPathName[result.ddLower] = 0;
					}
					else
					{
						// a gde vziat imia tuta?
						DbgBreakPoint();
					}
				}
				break;
			case IFSFN_DASDIO:
			case IFSFN_DISCONNECT:
			case IFSFN_FLUSH:
			case IFSFN_GETDISKINFO:
			case IFSFN_GETDISKPARMS:
			case IFSFN_QUERY:
			case IFSFN_CONNECT:
			case IFSFN_ENUMHANDLE:  // потому чтоооо!
				ConvertPath(fullPathName,Drive,NULL);
				break;
			case IFSFN_IOCTL16DRIVE:
				ConvertPath(fullPathName, Drive, NULL);
				sprintf(fullPathName+strlen(fullPathName)," SubFunc=%02xh",pir->ir_flags);
				break;
			case IFSFN_RENAME:
				ConvertPath(fullPathName,Drive,pir->ir_ppath);
				pSingleParam->ParamSize = strlen(pSingleParam->ParamValue) + 1;
				SINGLE_PARAM_SHIFT(pSingleParam);

				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_DEST);
				ConvertPath(pSingleParam->ParamValue, Drive,pir->ir_ppath2);
				pSingleParam->ParamSize = strlen(pSingleParam->ParamValue) + 1;
				pParam->ParamsCount++;
				break;
			case IFSFN_PIPEREQUEST: //???????????
			case IFSFN_UNCPIPEREQ:
				*fullPathName=0;
				break;
			default:
				ConvertPath(fullPathName,Drive,NULL);
				strcat(fullPathName," Unrecognized command!!!");
			}
#ifdef __DBG__
			Action=HeapAllocate(MAX_MSG_LEN,0);
			if(Action) {
				DbPrint(DC_FILE,DL_INFO, ("%s %s %s\n",GetProcName(pParam->ProcName, NULL),VxDGetFunctionStr(Action,pParam->HookID, pParam->FunctionMj, pParam->FunctionMi),fullPathName));
				HeapFree(Action,0);
			} else {
				DbPrint(DC_FILE,DL_INFO, ("%s %s %s\n",GetProcName(pParam->ProcName, NULL),"--NO-MEM--",fullPathName));
			}
#endif //__DBG__
			pSingleParam->ParamSize = strlen(pSingleParam->ParamValue) + 1;

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, ResType & IFSFH_RES_NETWORK ? _CONTEXT_OBJECT_FLAG_SRCREMOTE : 0);
			
			Verdict = FilterEvent(pParam, NULL);
			
			if(Verdict==Verdict_Discard &&
				(fn==IFSFN_CLOSE || fn==IFSFN_FINDCLOSE || fn==IFSFN_FCNCLOSE)) {
				DbPrint(DC_FILE,DL_INFO, ("Verdict changed to Pass for close operation\n"));
				Verdict=Verdict_Pass;
			} else if(!_PASS_VERDICT(Verdict)) {
				retval=pir->ir_error=ERROR_ACCESS_DENIED;
				if(Verdict == Verdict_Kill) {
					DbPrint(DC_FILE,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
					KillCurrentProcess();
				} else if(Verdict == Verdict_Discard) {
					DbPrint(DC_FILE,DL_NOTIFY, ("Discard %s\n",fullPathName));
				}
				ExFreePool(RequestData);
				return retval;
			}
		} //if(IsNeedFilterEvent())
	}

	if ((IFSFN_READ == fn) || (IFSFN_WRITE == fn))
	{
		if (!IsInvisibleThread((ULONG)PsGetCurrentThreadId()))
			bTime = TRUE;
	}

	if (bTime)
	{
		LONG curc = InterlockedIncrement(&g_C);
		InterlockedIncrement(&g_C);
		if (1 == curc)
			g_T = GetCurTimeFast(); // reset start period
	}

	retval=(*PrevIFSHookProc)(pfn,fn,Drive,ResType,CodePage,pir);

	if (bTime)
	{
		if (1 == InterlockedDecrement(&g_C)) // pre-exit
		{
			// lock not needed in this block
			// only one thread at a time may be here
			unsigned __int64 t = GetCurTimeFast();
			g_TW += (t - g_T); // increment wait time by delta
			g_T = t; // set new start period
		}
		InterlockedDecrement(&g_C);
	}

	if ((fn == IFSFN_DISCONNECT) || (fn == IFSFN_CONNECT))
		ClearDriveFlags();

	pParam->ProcessingType = PostProcessing;	
	if(isneedflt && RequestData)
	{
		if(retval == ERROR_SUCCESS)
		{
			if(fn==IFSFN_OPEN)
				NameCacheStore(g_pCommon_NameCache, (ULONG)pir->ir_fh,fullPathName,strlen(fullPathName)+1, FALSE, '9boS');
			
			switch(fn) {
			case IFSFN_CLOSE:
				DbPrint(DC_FILE,DL_INFO, ("IFSFN_CLOSE postproc: %s\n",fullPathName));
				Verdict = FilterEvent(pParam, NULL);
				break;
			case IFSFN_CONNECT:
				DeleteFromDelayedQueue(Drive);
				DbPrint(DC_FILE,DL_INFO, ("IFSFN_CONNECT for drive %c: status=%d\n",Drive+'A'-1,retval));
				
				Verdict = FilterEvent(pParam, NULL);
				break;
				//		case IFSFN_FINDOPEN:
				//			CacheStore((ULONG)pir->ir_sfn,fullPathName);
				//			break;
			}
			/*
			что бы нагрузку лишнюю не создавать... перенес в case...
			pParam->ProcessingType = PostProcessing;
			Verdict = FilterEvent(pParam);
			*/
		} //if(retval == ERROR_SUCCESS) 
		else
		{
			//! unremark this line to check unformatted volume
			if (fn == IFSFN_CONNECT)
			{
				if (ERROR_IFSVOL_EXISTS == retval)
				{
					//!Verdict = FilterEvent(pParam, NULL);
				}
				else if (retval != ERROR_NOT_READY)
				{
					PlaceToDelayedQueue(Drive);
				}
			}
#ifdef __DBG__
			if(fn==IFSFN_OPEN || fn==IFSFN_RENAME) {
				Action=HeapAllocate(MAX_MSG_LEN,0);
				if(Action) {
					DbPrint(DC_FILE,DL_INFO, ("%s compl status=%d %s\n",
						VxDGetFunctionStr(Action,pParam->HookID, pParam->FunctionMj, pParam->FunctionMi),retval,fullPathName));
					HeapFree(Action,0);
				}
			}
#endif
		}
	} //if(isneedflt)
	if(RequestData)
		ExFreePool(RequestData);
	return retval;
}

BOOLEAN IFSHooked=FALSE;

BOOLEAN IFSHook(VOID)
{
	if(!IFSHooked)
	{
		if((PrevIFSHookProc=IFSMgr_InstallFileSystemApiHook(IFSHookProc))) {
#ifdef __DBG__
			{
			char vxdname[100];
			vxdname[0] = 0;
			GetVxDName((ULONG)*PrevIFSHookProc, vxdname);
			DbPrint(DC_FILE,DL_IMPORTANT, ("IFS hooked. Previos IFSHook is %x (%s)\n", *PrevIFSHookProc,vxdname));
			}
#endif
			IFSHooked=TRUE;
			return TRUE;
		} else 
			return FALSE;
	} else {
		DbPrint(DC_FILE,DL_WARNING, ("!!! IFSHook call when it already hooked\n"));
	}
	return TRUE;
}

BOOLEAN IFSUnHook(VOID)
{
	NTSTATUS Status;
	if(IFSHooked)
	{
		Status=IFSMgr_RemoveFileSystemApiHook(IFSHookProc);
		DbPrint(DC_FILE,DL_IMPORTANT, ("IFS unhooked. Status=%x\n",Status));
		IFSHooked=FALSE;
	}
	return TRUE;
}

VOID IFSInit()
{
	ExInitializeFastMutex(&FsUnrecognizedDevListMutex);
}

VOID IFSDone()
{
	
}


/*

The preferred method for hookers to perform other functions while on a hooked call is to usethe Ring 0 APIs. 
It is usually quite safe to issue a Ring 0 API call while on a file system API hook, the IFS manager is reentrant. 
There are, however, certain special cases in which the hook cannot make these calls. These restrictions are 
listed in the following. 

If the call being hooked is issued for a swap-file operation, that is, ir_options contains 
the R0_SWAPPER_CALL flag, then the call must be processed without reentering the IFS manager via the Ring 0 APIs. 
This can cause additional paging and can lead to a deadlock. Note that multiple calls read/write I/O calls 
can be made to the FSD, however, the calls must all specify the R0_SWAPPER_CALL flag so that the FSD does 
special handling for it. 

The same restrictions mentioned above also apply to memory-mapped files, that is, read/write I/O 
operations with the R0_MM_READ_WRITE flag specified in ir_options. 
Another important restriction on file system API hookers applies to paging I/O, that is, I/O with 
the R0_SWAPPER_CALL or R0_MM_READ_WRITE bits set in ir_options. Any code that is executed in the file system 
hook during paging I/O must be locked down. If the hook causes additional paging activity on a paging I/O, 
system deadlocks can result. This also means that the code executed during paging I/O cannot perform memory 
allocations or call any services that can cause paging. 

*/

/*
// read data from boot
// -----------------------------------------------------------------------------------------

			PFILTER_EVENT_PARAM pParam;
			PSINGLE_PARAM pSingleParam;
			WORD					Error=0;
			
			DWORD BufferLength = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) * 2 + MAXPATHLEN + 1 + 
				512 * 4;	// read at least 4 sectors;
			
			pParam = (PFILTER_EVENT_PARAM)ExAllocatePoolWithTag(NonPagedPool, BufferLength, 'TboS');
			if (pParam != NULL) {

				DWORD Sector;
				BYTE *pPointer;
	
				Sector = 0;
				
				pParam->HookID = FLTTYPE_IFS;
				pParam->FunctionMj = IFSFN_CONNECT;
				pParam->FunctionMi = 0;
				
				pParam->ProcessingType = PostProcessing;
				pParam->ParamsCount = 2;
				pParam->ProcName[0] = 0;
				pParam->ProcessID = 0;
				pParam->ThreadID = 0;
				pParam->UnsafeValue = 1;				// auto post processing
				
				BufferLength -= (sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM));
				pSingleParam = (PSINGLE_PARAM) pParam->Params;
				pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
				
				pSingleParam->ParamID = _PARAM_OBJECT_URL;
				sprintf(pSingleParam->ParamValue, "%s", Drive+'A' - 1);
				
				pSingleParam->ParamSize = strlen(pSingleParam->ParamValue) + 1;
				BufferLength -= pSingleParam->ParamSize;
				
				BufferLength -= sizeof(SINGLE_PARAM);
				SINGLE_PARAM_SHIFT(pSingleParam);
				pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
				pSingleParam->ParamID = _PARAM_OBJECT_BINARYDATA;
				pSingleParam->ParamSize = BufferLength - BufferLength % 512;
		{
		BYTE *buff;
		ifsreq	ifsr;
		pIFSFunc	enumFunc;
			if(buff=ExAllocatePoolWithTag(NonPagedPool, 1024, 'TboS')) {
				memcpy(&ifsr,pir,sizeof(ifsreq));
				ifsr.ifsir.ir_flags=DIO_ABS_READ_SECTORS;
				ifsr.ifsir.ir_pos=0;//sector
				ifsr.ifsir.ir_length=1;//num of sec
				ifsr.ifsir.ir_data=buff;
//				enumFunc=ifsr.ifs_vol->vfn_func[VFN_DASDIO];
//				DbPrint(DC_FILE,2, ("IFSFN_DASDIO call (addr %x) enumFunc=%x\n", *PrevIFSHookProc,enumFunc));
//				retval=(*PrevIFSHookProc)(enumFunc,IFSFN_DASDIO,Drive,ResType,CodePage,(pioreq)&ifsr);
//				if(retval==ERROR_SUCCESS) {
//				}
			}
			ExFreePool(buff);
		}

//				if(R0_ReadAbsoluteDisk((BYTE)Drive-1,BufferLength/512,Sector,pSingleParam->ParamValue,&Error)) {
//					FilterEvent(pParam);
//				} else {
//					DbPrint(DC_FILE,3, ("Read disk failed. Error=%d\n",Error));
//				}
//				ExFreePool(pParam);

			}
			// -----------------------------------------------------------------------------------------
*/





















