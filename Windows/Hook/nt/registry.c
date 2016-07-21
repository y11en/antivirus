#define _REG_MAXPATHLEN	MAXPATHLEN * 2

ULONG gRegUnsafe = 0;

PVOID
CopyUserBuffer (
	PVOID pContext,
	ULONG Size
	);

PUNICODE_STRING
GetUserUniStr (
	PUNICODE_STRING pUserUniStr
	);

typedef struct _treg_val
{
	ULONG	m_DataType;
	ULONG	m_DataSize;
	PVOID	m_pData;
	HANDLE	m_Handle;
}reg_val;

POBJECT_NAME_INFORMATION GetKeyName(HANDLE hKey)
{
	PVOID                   pKey = NULL;
	ANSI_STRING             keyname;
	ULONG                   actualLen;
	NTSTATUS				 ntStatus;
	POBJECT_NAME_INFORMATION fullUniName = NULL;

	if(!hKey)
		return NULL;

	ntStatus = ObReferenceObjectByHandle(hKey, 0, NULL, KernelMode, &pKey, NULL);
	if(NT_SUCCESS(ntStatus))
	{
		fullUniName = ExAllocatePoolWithTag(NonPagedPool,_REG_MAXPATHLEN * sizeof(WCHAR),'pBoS');
		if(NULL == fullUniName)
		{
			DbPrint(DC_REG,DL_ERROR, ("GetKeyName failed: low memory\n"));
		}
		else
		{
			ntStatus = ObQueryNameString(pKey, fullUniName, _REG_MAXPATHLEN * sizeof(WCHAR), &actualLen);
			if(!NT_SUCCESS(ntStatus) || (0 == fullUniName->Name.Length))
			{
				ExFreePool(fullUniName);
				fullUniName = NULL;
			}
		}
		ObDereferenceObject(pKey);
	}
	else
	{
		DbPrint(DC_REG,DL_ERROR, ("ObReferenceObjectByHandle failed status=%x hKey=%x\n",ntStatus,hKey));
	}

	return fullUniName;
}

NTSTATUS HCreateKey(OUT PHANDLE pHandle, IN ACCESS_MASK ReqAccess,IN POBJECT_ATTRIBUTES pOpenInfo, IN ULONG TitleIndex,IN PUNICODE_STRING Class, IN ULONG CreateOptions, OUT PULONG Disposition);
NTSTATUS HDeleteKey(IN HANDLE Handle);
NTSTATUS HDeleteValueKey(IN HANDLE Handle, PUNICODE_STRING Name);
NTSTATUS HEnumerateKey(IN HANDLE Handle, IN ULONG Index,IN KEY_INFORMATION_CLASS KeyInformationClass,OUT PVOID KeyInformation, IN ULONG Length, OUT PULONG pResultLength);
NTSTATUS HEnumerateValueKey(IN HANDLE Handle, IN ULONG Index,IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,OUT PVOID KeyValueInformation, IN ULONG Length,OUT PULONG  pResultLength);
NTSTATUS HFlushKey(IN HANDLE Handle);
NTSTATUS HInitializeRegistry(ULONG State);
NTSTATUS HLoadKey2(IN POBJECT_ATTRIBUTES SubKey,IN POBJECT_ATTRIBUTES File,ULONG arg_C);
NTSTATUS HNotifyChangeKey(IN HANDLE Handle,IN HANDLE hEvent,ULONG arg_C,ULONG arg_10,ULONG arg_14,ULONG NotifyFilter,ULONG bWatchSubtree,ULONG arg_20,ULONG arg_24,ULONG arg_28);
NTSTATUS HOpenKey(IN OUT PHANDLE pHandle, IN ACCESS_MASK ReqAccess, IN POBJECT_ATTRIBUTES pOpenInfo);
NTSTATUS HQueryKey(IN HANDLE Handle,IN KEY_INFORMATION_CLASS  KeyInformationClass,OUT PVOID  KeyInformation, IN ULONG  Length, OUT PULONG  pResultLength);
NTSTATUS HQueryMultipleValueKey(IN HANDLE Handle,ULONG arg_8,ULONG arg_C,PVOID pValBuff,ULONG arg_14,ULONG arg_18);
NTSTATUS HQueryValueKey(IN HANDLE Handle,IN PUNICODE_STRING ValueName,IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,OUT PVOID KeyValueInformation, IN ULONG Length,OUT PULONG  pResultLength);
NTSTATUS HReplaceKey(IN POBJECT_ATTRIBUTES NewFile,IN HANDLE Handle,IN POBJECT_ATTRIBUTES OldFile);
NTSTATUS HRestoreKey(IN HANDLE Handle,IN HANDLE FileHandle,ULONG dwFlags);
NTSTATUS HSaveKey(IN HANDLE Handle,IN HANDLE FileHandle);
NTSTATUS HSetInformationKey(IN HANDLE Handle,ULONG arg_8,ULONG arg_C,ULONG arg_10);
NTSTATUS HSetValueKey(IN HANDLE Handle, IN PUNICODE_STRING ValueName,IN ULONG TitleIndex, IN ULONG Type, IN PVOID Data, IN ULONG DataSize);
NTSTATUS HUnloadKey(IN POBJECT_ATTRIBUTES SubKey);
NTSTATUS HLoadKey(IN POBJECT_ATTRIBUTES KeyToLoad, IN POBJECT_ATTRIBUTES File);


typedef struct _REGHOOK {
	PVOID Hook;
	PVOID Real;
	ULONG ID;
	PCHAR FuncName;
} REGHOOK;

REGHOOK RHTable[]= {
	{HCreateKey,				NULL,0,"NtCreateKey"},
	{HDeleteKey,				NULL,0,"NtDeleteKey"},
	{HDeleteValueKey,			NULL,0,"NtDeleteValueKey"},
	{HEnumerateKey,			NULL,0,"NtEnumerateKey"},
	{HEnumerateValueKey,		NULL,0,"NtEnumerateValueKey"},
	{HFlushKey,					NULL,0,"NtFlushKey"},
	{HInitializeRegistry,	NULL,0,"NtInitializeRegistry"},
	{HLoadKey2,					NULL,0,"NtLoadKey2"},
	{HNotifyChangeKey,		NULL,0,"NtNotifyChangeKey"},
	{HOpenKey,					NULL,0,"NtOpenKey"},
	{HQueryKey,					NULL,0,"NtQueryKey"},
	{HQueryMultipleValueKey,NULL,0,"NtQueryMultipleValueKey"},
	{HQueryValueKey,			NULL,0,"NtQueryValueKey"},
	{HReplaceKey,				NULL,0,"NtReplaceKey"},
	{HRestoreKey,				NULL,0,"NtRestoreKey"},
	{HSaveKey,					NULL,0,"NtSaveKey"},
	{HSetInformationKey,		NULL,0,"NtSetInformationKey"},
	{HSetValueKey,				NULL,0,"NtSetValueKey"},
	{HUnloadKey,				NULL,0,"NtUnloadKey"},
	{HLoadKey,					NULL,0,"NtLoadKey"}
};

volatile BOOLEAN bRegHooked = FALSE;
BOOLEAN RegHook(VOID)
{
ULONG	NativeID;
ULONG i;
BOOLEAN	OldProtect;

	if (bRegHooked)
		return TRUE;
	bRegHooked = TRUE;

	if(UnloadInProgress) {
		DbPrint(DC_REG,DL_WARNING, ("RegHook when UnloadInProgress!\n"));
		return FALSE;
	}
	for(i=0;i<sizeof(RHTable)/sizeof(REGHOOK);i++) {
		if(NativeID=GetNativeID(BaseOfNtDllDll,RHTable[i].FuncName)) {
			RHTable[i].ID=NativeID;
			RHTable[i].Real=(PVOID)SYSTEMSERVICE_BY_FUNC_ID(NativeID);
			MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase+NativeID,TRUE,&OldProtect);
			__try {
				SYSTEMSERVICE_BY_FUNC_ID(NativeID)=(ULONG)RHTable[i].Hook;
			} __except (EXCEPTION_EXECUTE_HANDLER) {
				DbPrint(DC_REG,DL_ERROR, ("Exception on setting reg hooker! %s VAddr 0x%x\n",RHTable[i].FuncName, KeServiceDescriptorTable.ServiceTableBase +NativeID));
				return FALSE;
			}
			DbPrint(DC_REG,DL_INFO, ("%s (%x/%x) hooked\n",RHTable[i].FuncName,RHTable[i].ID,RHTable[i].Real));
		} else {
			DbPrint(DC_REG,DL_WARNING, ("ID for %s not found\n",RHTable[i].FuncName));
		}
	}
	return TRUE;
}

BOOLEAN RegUnHook(VOID)
{
/*ULONG i;
	for(i=0;i<sizeof(RHTable)/sizeof(REGHOOK);i++) 
		if(RHTable[i].Real) {
			SYSTEMSERVICE_BY_FUNC_ID(RHTable[i].ID)=(ULONG)RHTable[i].Real;
// при снятии хука при нагрузке есть шанс получить в перехватчике для следующей функции
//!			RHTable[i].Real=NULL;
			DbPrint(DC_REG,DL_INFO, ("Registry %s (%x/%x) unhooked\n",NTRegistryFunctionName[i],RHTable[i].ID,RHTable[i].Real));
		}*/
	return TRUE;
}

ULONG RegGetIdex(PVOID HookFunc)
{
	ULONG i;
	for(i=0;i<sizeof(RHTable)/sizeof(REGHOOK);i++) 
		if(RHTable[i].Hook==HookFunc)
			break;
		return i;
}

NTSTATUS HRegCommon(ULONG index, PUNICODE_STRING KeyName, PUNICODE_STRING ValueName, PFILTER_EVENT_PARAM* ppEvent, reg_val* prval)
{
	NTSTATUS retStatus = STATUS_SUCCESS;
	VERDICT	Verdict = Verdict_Default;
	KIRQL	CurrIrql;
	
	ULONG ReqDataSize;
	PSINGLE_PARAM pSingleParam;
	PVOID RequestData;

	PSINGLE_PARAM pParamKeyName = NULL;
	PSINGLE_PARAM pParamValueName = NULL;

	CurrIrql = KeGetCurrentIrql();
	if (CurrIrql >= DISPATCH_LEVEL)
		return retStatus;
	
	if (IsNeedFilterEvent(FLTTYPE_REGS, index, 0) == TRUE)
	{
		PFILTER_EVENT_PARAM pParam;

		if (prval && prval->m_pData)
		{
			try
			{
				TouchPage(prval->m_pData);
			}
			except(EXCEPTION_EXECUTE_HANDLER)
			{
				DbgBreakPoint();
				prval = 0;
			}
		}
		
		if (ValueName)
		{
			try
			{
				USHORT len = ValueName->Length;
				if (len)
				{
					TouchPage(ValueName->Buffer);
				}
				else
				{
					ValueName = NULL;
				}
			}
			except(EXCEPTION_EXECUTE_HANDLER)
			{
				DbgBreakPoint();
				ValueName = NULL;
			}
		}
		if (*ppEvent)
		{
			pParam = *ppEvent;
			pParam->ProcessingType = PostProcessing;
		}
		else
		{
			ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) * 2;
			ReqDataSize += KeyName->Length + sizeof(WCHAR);
			if (ValueName)
				ReqDataSize += ValueName->Length + sizeof(WCHAR);
			ReqDataSize += sizeof (SINGLE_PARAM) + __SID_LENGTH;

			if (prval)
			{
				// type + val
				ReqDataSize += sizeof(SINGLE_PARAM) * 2 + sizeof(ULONG) + prval->m_DataSize;
			}
			
			*ppEvent = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
			if (0 == *ppEvent)
			{
				DbPrint(DC_REG,DL_ERROR, ("Allocate buffer in HRegCommon for RequestData failed. Size %d\n", ReqDataSize));
				return retStatus;
			}

			pParam = *ppEvent;

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_REGS, index, 0, PreProcessing, 2);
			pParam->UnsafeValue = gRegUnsafe;

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
			pParamKeyName = pSingleParam;

			pSingleParam->ParamSize = KeyName->Length;
			memcpy(pSingleParam->ParamValue, KeyName->Buffer, pSingleParam->ParamSize);
			((PWCHAR)pSingleParam->ParamValue)[pSingleParam->ParamSize / sizeof(WCHAR)] = 0;
			pSingleParam->ParamSize += sizeof(WCHAR);
			
			if (ValueName)
			{
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_PARAM_W);

				pParamValueName = pSingleParam;

				try
				{
					memcpy(pSingleParam->ParamValue, ValueName->Buffer, ValueName->Length);
					pSingleParam->ParamSize = ValueName->Length;
					((PWCHAR)pSingleParam->ParamValue)[pSingleParam->ParamSize / sizeof(WCHAR)] = 0;
					pSingleParam->ParamSize += sizeof(WCHAR);
				}
				except(EXCEPTION_EXECUTE_HANDLER)
				{
					DbgBreakPoint();
					pSingleParam->ParamSize = 0;
				}

				pParam->ParamsCount++;
			}
			// -----------------------------------------------------------------------------------------
			// reserve place for sid
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_SID(pSingleParam);
			// end reserve place for sid

			if (prval)
			{
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, prval->m_DataType);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_BINARYDATA);
				pSingleParam->ParamSize = prval->m_DataSize;
				try
				{
					memcpy(pSingleParam->ParamValue, prval->m_pData, prval->m_DataSize);
				}
				except(EXCEPTION_EXECUTE_HANDLER)
				{
					DbgBreakPoint();
					pSingleParam->ParamSize = 0;
				}

				pParam->ParamsCount += 2;
			}
			// -----------------------------------------------------------------------------------------
		}

		DbPrint(DC_REG,DL_SPAM, ("%s %s %s\n", pParam->ProcName, NTRegistryFunctionName[index],KeyName));
		
		if (pParamKeyName && pParamValueName && pParamValueName->ParamSize == sizeof(L"Seed") &&
			pParamKeyName->ParamSize == sizeof(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\RNG"))
		{
			if (!memcmp(pParamValueName->ParamValue, L"Seed", sizeof(L"Seed")) &&
				!memcmp(pParamKeyName->ParamValue, L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\RNG", sizeof(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\RNG")))
			{
				Verdict = Verdict_Pass; // skip this event
			}
		}
			
		if (Verdict_Default == Verdict)
			Verdict = FilterEvent(pParam, NULL);

		if(!_PASS_VERDICT(Verdict))
		{
			retStatus = STATUS_ACCESS_DENIED;
			if(Verdict == Verdict_Kill)
			{
				DbPrint(DC_REG,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
				KillCurrentProcess();
			}
			else if(Verdict == Verdict_Discard)
			{
				DbPrint(DC_REG,DL_NOTIFY, ("Discard %s\n", KeyName));

				if (prval && prval->m_DataSize == sizeof(ULONG))
				{
					NTSTATUS stattmp;
					PVOID LoadBuff = ExAllocatePoolWithTag(PagedPool, PAGE_SIZE, 'fSeB');
					if (LoadBuff)
					{
						KEY_VALUE_PARTIAL_INFORMATION* pInfo = (KEY_VALUE_PARTIAL_INFORMATION*) LoadBuff;
						ULONG ResultLength = 0;
						stattmp = ZwQueryValueKey(prval->m_Handle, ValueName, KeyValuePartialInformation, LoadBuff, PAGE_SIZE, &ResultLength);
						if (NT_SUCCESS(stattmp) && pInfo->DataLength == sizeof(ULONG))
						{
							if (*(ULONG*) pInfo->Data == *(ULONG*)prval->m_pData)
							{
								retStatus = STATUS_SUCCESS;
							}
						}
						ExFreePool(LoadBuff);
					}
				}
			}
		}
	}
	return retStatus;
}

//+ ------------------------------------------------------------------------------------------
#define _REG_FUNC_IMP(_call, _func_name, _need_post, _handle, _valname, _reg_val, _additional_ptr) {\
	NTSTATUS	ntstatus = STATUS_SUCCESS;\
	ULONG		index;\
	POBJECT_NAME_INFORMATION pInfo;\
	PFILTER_EVENT_PARAM pEvent = 0;\
	index = RegGetIdex((PVOID)_func_name);\
	pInfo = GetKeyName(_handle);\
	if (!pInfo)\
	{\
		ntstatus = _call;\
		if (_additional_ptr)\
			ExFreePool(_additional_ptr);\
		return ntstatus;\
	}\
	ntstatus = HRegCommon(index, &pInfo->Name, _valname, &pEvent, _reg_val);\
	if(NT_SUCCESS(ntstatus))\
	{\
		ntstatus = _call;\
		if (_need_post)\
		{\
			if (NT_SUCCESS(ntstatus))\
			{\
				if (pEvent)\
				{\
					pEvent->ProcessingType = PostProcessing;\
					FilterEvent(pEvent, NULL);\
				}\
			}\
		}\
	}\
	if (pInfo)\
		ExFreePool(pInfo);\
	if (pEvent)\
		ExFreePool(pEvent);\
	if (_additional_ptr)\
		ExFreePool(_additional_ptr);\
	return ntstatus;\
}
//+ ------------------------------------------------------------------------------------------

NTSTATUS
HCreateKey (
	OUT PHANDLE pHandle,
	IN ACCESS_MASK ReqAccess,
	IN POBJECT_ATTRIBUTES pOpenInfo,
	IN ULONG TitleIndex,
	IN PUNICODE_STRING Class,
	IN ULONG CreateOptions,
	OUT PULONG Disposition
	)
{
	HANDLE hTmp = NULL;
	PUNICODE_STRING NameTmp = NULL;

	POBJECT_ATTRIBUTES pOpenInfoTmp = NULL;

	if (pOpenInfo)
		pOpenInfoTmp = CopyUserBuffer( pOpenInfo, sizeof(OBJECT_ATTRIBUTES));

	if (pOpenInfoTmp)
	{
		hTmp = pOpenInfoTmp->RootDirectory;

		NameTmp = GetUserUniStr( pOpenInfoTmp->ObjectName);

		ExFreePool( pOpenInfoTmp );
	}

	_REG_FUNC_IMP((((long (__stdcall *)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,ULONG,PUNICODE_STRING,ULONG,PULONG))(RHTable[index].Real))(pHandle,ReqAccess,pOpenInfo,TitleIndex,Class,CreateOptions,Disposition)),
		HCreateKey, FALSE, hTmp, NameTmp, NULL, NameTmp);
}

NTSTATUS HDeleteKey(IN HANDLE Handle)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE))(RHTable[index].Real))(Handle)),
		HDeleteKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HDeleteValueKey(IN HANDLE Handle, PUNICODE_STRING Name)
{
	PUNICODE_STRING NameTmp = GetUserUniStr( Name);

	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,PUNICODE_STRING))(RHTable[index].Real))(Handle,Name)),
		HDeleteValueKey, FALSE, Handle, NameTmp, NULL, NameTmp);
}

NTSTATUS HEnumerateKey(IN HANDLE Handle, IN ULONG Index,IN KEY_INFORMATION_CLASS KeyInformationClass,OUT PVOID KeyInformation, IN ULONG Length, OUT PULONG pResultLength)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,ULONG,KEY_INFORMATION_CLASS,PVOID,ULONG,PULONG))(RHTable[index].Real))(Handle,Index,KeyInformationClass,KeyInformation,Length,pResultLength)),
		HEnumerateKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HEnumerateValueKey(IN HANDLE Handle, IN ULONG Index,IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,OUT PVOID KeyValueInformation, IN ULONG Length,OUT PULONG  pResultLength)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,ULONG,KEY_VALUE_INFORMATION_CLASS,PVOID,ULONG,PULONG))(RHTable[index].Real))(Handle,Index,KeyValueInformationClass,KeyValueInformation,Length,pResultLength)),
		HEnumerateValueKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HFlushKey(IN HANDLE Handle)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE))(RHTable[index].Real))(Handle)),
		HFlushKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HInitializeRegistry(ULONG State)
{
	ULONG index = RegGetIdex((PVOID)HInitializeRegistry);
	return ((long (__stdcall *)(ULONG))(RHTable[index].Real))(State);
}

NTSTATUS HLoadKey2(IN POBJECT_ATTRIBUTES pOpenInfo,IN POBJECT_ATTRIBUTES File,ULONG arg_C)
{
	HANDLE hTmp = NULL;
	PUNICODE_STRING NameTmp = NULL;

	POBJECT_ATTRIBUTES pOpenInfoTmp = NULL;

	if (pOpenInfo)
		pOpenInfoTmp = CopyUserBuffer( pOpenInfo, sizeof(OBJECT_ATTRIBUTES));

	if (pOpenInfoTmp)
	{
		hTmp = pOpenInfoTmp->RootDirectory;

		NameTmp = GetUserUniStr( pOpenInfoTmp->ObjectName);

		ExFreePool( pOpenInfoTmp );
	}

	_REG_FUNC_IMP((((long (__stdcall *)(POBJECT_ATTRIBUTES,POBJECT_ATTRIBUTES,ULONG))(RHTable[index].Real))(pOpenInfo,File,arg_C)),
		HLoadKey2, FALSE, hTmp, NameTmp, NULL, NameTmp);
}

NTSTATUS HNotifyChangeKey(IN HANDLE Handle,IN HANDLE hEvent,ULONG arg_C,ULONG arg_10,ULONG arg_14,ULONG NotifyFilter,ULONG bWatchSubtree,ULONG arg_20,ULONG arg_24,ULONG arg_28)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,HANDLE,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG))(RHTable[index].Real))(Handle,hEvent,arg_C,arg_10,arg_14,NotifyFilter,bWatchSubtree,arg_20,arg_24,arg_28)),
		HNotifyChangeKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HOpenKey(IN OUT PHANDLE pHandle, IN ACCESS_MASK ReqAccess, IN POBJECT_ATTRIBUTES pOpenInfo)
{
	HANDLE hTmp = NULL;
	PUNICODE_STRING NameTmp = NULL;

	POBJECT_ATTRIBUTES pOpenInfoTmp = NULL;

	if (pOpenInfo)
		pOpenInfoTmp = CopyUserBuffer( pOpenInfo, sizeof(OBJECT_ATTRIBUTES));

	if (pOpenInfoTmp)
	{
		hTmp = pOpenInfoTmp->RootDirectory;

		NameTmp = GetUserUniStr( pOpenInfoTmp->ObjectName);

		ExFreePool( pOpenInfoTmp );
	}

	_REG_FUNC_IMP((((long (__stdcall *)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES))(RHTable[index].Real))(pHandle, ReqAccess, pOpenInfo)),
		HOpenKey, FALSE, hTmp, NameTmp, NULL, NameTmp);
}

NTSTATUS HQueryKey(IN HANDLE Handle,IN KEY_INFORMATION_CLASS  KeyInformationClass,OUT PVOID  KeyInformation, IN ULONG  Length, OUT PULONG  pResultLength)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,KEY_INFORMATION_CLASS,PVOID,ULONG,PULONG))(RHTable[index].Real))(Handle,KeyInformationClass,KeyInformation,Length,pResultLength)),
		HQueryKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HQueryMultipleValueKey(IN HANDLE Handle,ULONG arg_8,ULONG arg_C,PVOID pValBuff,ULONG arg_14,ULONG arg_18)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,ULONG,ULONG,PVOID,ULONG,ULONG))(RHTable[index].Real))(Handle,arg_8,arg_C,pValBuff,arg_14,arg_18)),
		HQueryMultipleValueKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HQueryValueKey(IN HANDLE Handle,IN PUNICODE_STRING ValueName,IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,OUT PVOID KeyValueInformation, IN ULONG Length,OUT PULONG  pResultLength)
{
	PUNICODE_STRING NameTmp = GetUserUniStr( ValueName);
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,PUNICODE_STRING,KEY_VALUE_INFORMATION_CLASS,PVOID,ULONG,PULONG))(RHTable[index].Real))(Handle,ValueName,KeyValueInformationClass,KeyValueInformation,Length,pResultLength)),
		HQueryValueKey, FALSE, Handle, NameTmp, NULL, NameTmp);
}

NTSTATUS HReplaceKey(IN POBJECT_ATTRIBUTES NewFile,IN HANDLE Handle,IN POBJECT_ATTRIBUTES OldFile)
{
	_REG_FUNC_IMP((((long (__stdcall *)(POBJECT_ATTRIBUTES,HANDLE,POBJECT_ATTRIBUTES))(RHTable[index].Real))(NewFile,Handle,OldFile)),
		HReplaceKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HRestoreKey(IN HANDLE Handle,IN HANDLE FileHandle,ULONG dwFlags)
{
	ULONG NameSize = 0x1000;
	PFILE_NAME_INFORMATION pFileNameInformation = ExAllocatePoolWithTag(NonPagedPool, NameSize, ')bos' );
	
	if (pFileNameInformation)
	{
		PFILE_OBJECT pFileObject = NULL;

		NTSTATUS status = ObReferenceObjectByHandle(
					FileHandle,
					0,
					*IoFileObjectType,
					UserMode,
					&pFileObject,
					NULL);
		
		if (NT_SUCCESS( status ))
		{
			WCHAR DriveLetter;
			IO_STATUS_BLOCK IoStatusBlock;

			DriveLetter = GetDeviceLetter(pFileObject->DeviceObject);
			
			if (DriveLetter == 0)
			{
				DriveLetter = L'?';
				DbgBreakPoint();
			}

			status = ZwQueryInformationFile(FileHandle, &IoStatusBlock, pFileNameInformation, NameSize, FileNameInformation);
		
			ObDereferenceObject( pFileObject );

			if(NT_SUCCESS( status ))
			{
				if (pFileNameInformation->FileNameLength != 0)
				{
					UNICODE_STRING usFileName;

					PWCHAR pwchTmp = pFileNameInformation->FileName;
					
					usFileName.Length = (USHORT) pFileNameInformation->FileNameLength + sizeof(WCHAR);

					pwchTmp--;
					*pwchTmp = L':';

					pwchTmp--;
					*pwchTmp = DriveLetter;

					usFileName.Buffer = pwchTmp;
					usFileName.MaximumLength = usFileName.Length;

					_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,HANDLE,ULONG))(RHTable[index].Real))(Handle,FileHandle,dwFlags)),
						HRestoreKey, FALSE, Handle, &usFileName, NULL, pFileNameInformation);
				}
			}
		}

		ExFreePool( pFileNameInformation );
	}

	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,HANDLE,ULONG))(RHTable[index].Real))(Handle,FileHandle,dwFlags)),
		HRestoreKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HSaveKey(IN HANDLE Handle,IN HANDLE FileHandle)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,HANDLE))(RHTable[index].Real))(Handle,FileHandle)),
		HSaveKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HSetInformationKey(IN HANDLE Handle,ULONG arg_8,ULONG arg_C,ULONG arg_10)
{
	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,ULONG,ULONG,ULONG))(RHTable[index].Real))(Handle,arg_8,arg_C,arg_10)),
		HSetInformationKey, FALSE, Handle, NULL, NULL, NULL);
}

NTSTATUS HSetValueKey(IN HANDLE Handle, IN PUNICODE_STRING ValueName,IN ULONG TitleIndex, IN ULONG Type, IN PVOID Data, IN ULONG DataSize)
{
	reg_val rval;
	rval.m_DataType = Type;
	
	if (Data && MmIsAddressValid(Data))
	{
		rval.m_pData = Data;
		if (DataSize > 0xffff)
			rval.m_DataSize = 0xffff;
		else
			rval.m_DataSize = DataSize;
	}
	else
	{
		DbgBreakPoint();
		rval.m_pData = NULL;
		rval.m_DataSize = 0;
	}

	rval.m_Handle = Handle;

	_REG_FUNC_IMP((((long (__stdcall *)(HANDLE,PUNICODE_STRING,ULONG,ULONG,PVOID,ULONG))(RHTable[index].Real))(Handle,ValueName,TitleIndex,Type,Data,DataSize)), 
		HSetValueKey, TRUE, Handle, ValueName, &rval, NULL);
}

NTSTATUS HUnloadKey(IN POBJECT_ATTRIBUTES SubKey)
{
	NTSTATUS	ntstatus;
	ULONG		index;
	index = RegGetIdex(HUnloadKey);

	InterlockedIncrement(&gRegUnsafe);
	ntstatus = (((long (__stdcall *)(POBJECT_ATTRIBUTES))(RHTable[index].Real))(SubKey));
	InterlockedDecrement(&gRegUnsafe);

	return ntstatus;
}

NTSTATUS HLoadKey(IN POBJECT_ATTRIBUTES KeyToLoad, IN POBJECT_ATTRIBUTES File)
{
	NTSTATUS	ntstatus;
	ULONG		index;
	index = RegGetIdex(HLoadKey);

	InterlockedIncrement(&gRegUnsafe);
	ntstatus = (((long (__stdcall *)(POBJECT_ATTRIBUTES, POBJECT_ATTRIBUTES))(RHTable[index].Real))(KeyToLoad, File));
	InterlockedDecrement(&gRegUnsafe);

	return ntstatus;
}
