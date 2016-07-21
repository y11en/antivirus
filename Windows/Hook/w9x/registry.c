/************************************* Registry ********************************************************/

struct _KEYNODE;
typedef struct _KEYDR{
	struct _KEYDR *Next;
	struct _KEYNODE *KeyNode;
	DWORD NameLen;
	BYTE	field_c;
	BYTE	Name;
}KEYDR,*PKEYDR;

typedef struct _KEYNODE{
	DWORD	field_0;
	PKEYDR KeyDR;
} KEYNODE,*PKEYNODE;

typedef struct _HKeyS{
	CHAR		Sig[2];
	WORD		Flg;
	DWORD		InUseCnt;
	PKEYNODE	KeyNode;
	DWORD		field_C;
	DWORD		field_10;
	WORD		field_14;
	WORD		field_16;
	BYTE		field_18;
	BYTE		field_19;
	struct _HKeyS	*Prev;
	struct _HKeyS	*Next;
	DWORD		field_22;
	DWORD		field_26;
	PVOID		SomeMemPtr;
} HKeyStruct,*PHKeyStruct;

void GetKeyName(HKEY hKey, PCHAR lpszSubKey, PCHAR lpszValue, PCHAR fullname)
{
	BOOLEAN bOverBuf;
PCHAR	pStr=NULL;
	switch(hKey) {
	case HKEY_CLASSES_ROOT:
		pStr="HKEY_CLASSES_ROOT";
		break;
	case HKEY_CURRENT_USER:
		pStr="HKEY_CURRENT_USER";
		break;
	case HKEY_LOCAL_MACHINE:
		pStr="HKEY_LOCAL_MACHINE";
		break;
	case HKEY_USERS:
		pStr="HKEY_USERS";
		break;
	case HKEY_PERFORMANCE_DATA:
		pStr="HKEY_PERFORMANCE_DATA";
		break;
	case HKEY_CURRENT_CONFIG:
		pStr="HKEY_CURRENT_CONFIG";
		break;
	case HKEY_DYN_DATA:
		pStr="HKEY_DYN_DATA";
		break;
	default:
		bOverBuf = FALSE;
		pStr=NameCacheGet(g_pCommon_NameCache, (ULONG)hKey,NULL, MAXPATHLEN, &bOverBuf);//fullname);
	}
	strcpy(fullname,pStr);
	if( lpszSubKey ) {
		if(*lpszSubKey) {
			strcat(fullname, "\\" );
			strcat(fullname, lpszSubKey);
		}
	}
	if(lpszValue) {
		if(*lpszValue) {
			strcat(fullname,"\\");
			strcat(fullname, lpszValue);
		}
	}
}


LONG HRegOpenKey(HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult);
LONG HRegCloseKey(HKEY hkey);
LONG HRegCreateKey(HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult);
LONG HRegDeleteKey(HKEY hkey, PCHAR lpszSubKey);
LONG HRegEnumKey(HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName);
LONG HRegQueryValue(HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue, PLONG pcbValue);
LONG HRegSetValue(HKEY hkey, PCHAR lpszSubKey, DWORD fdwType, PCHAR lpszData, DWORD cbData);
LONG HRegDeleteValue(HKEY hkey, PCHAR lpszValue);
LONG HRegEnumValue(HKEY hkey, DWORD iValue, PCHAR lpszValue, PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData);
LONG HRegQueryValueEx(HKEY hkey, PCHAR lpszValueName, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData);
LONG HRegSetValueEx(HKEY hkey, PCHAR lpszValueName, DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData);
LONG HRegFlushKey(HKEY hkey);
DWORD HRegQueryInfoKey(HKEY hKey, PCHAR lpszClass, PDWORD lpcchClass,  DWORD lpdwReserved, PDWORD lpcSubKeys, PDWORD lpcchMaxSubKey, PDWORD  pcchMaxClass, PDWORD lpcValues, PDWORD lpcchMaxValueName, PDWORD lpcbMaxValueData, PDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime);
DWORD HRegRemapPreDefKey(HKEY hKey, HKEY hRootKey);
DWORD HRegCreateDynKey(PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle);
DWORD HRegQueryMultipleValues(HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize);
LONG __stdcall HQueryInfoKey16(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PDWORD lpcSubKeys,PDWORD lpcchMaxSubKey,PDWORD lpcValues, PDWORD lpcchMaxValueName,PDWORD lpcbMaxValueData);
LONG __stdcall HLoadKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpSubKey,PCHAR lpFileName);
LONG __stdcall HUnLoadKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpSubKey);
LONG __stdcall HSaveKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpFileName,PVOID SecurAttr);
LONG __stdcall HReplaceKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpSubKey,PCHAR lpNewFileName,PCHAR OldFileName);
LONG __stdcall HNotifyChangeKeyValue(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,BOOL bWatchSubtree,DWORD dwNotifyFilter,HANDLE hEvent);


typedef struct _REGHOOK {
	PVOID Real;
	PVOID Hook;
	DWORD Service;
	HDSC_Thunk Thunk;
} REGHOOK;

REGHOOK RHTable[]= {
	{NULL,HRegOpenKey					,___RegOpenKey},
	{NULL,HRegCloseKey				,___RegCloseKey},
	{NULL,HRegCreateKey				,___RegCreateKey},
	{NULL,HRegDeleteKey				,___RegDeleteKey},
	{NULL,HRegEnumKey					,___RegEnumKey},
	{NULL,HRegQueryValue				,___RegQueryValue},
	{NULL,HRegSetValue				,___RegSetValue},
	{NULL,HRegDeleteValue			,___RegDeleteValue},
	{NULL,HRegEnumValue				,___RegEnumValue},
	{NULL,HRegQueryValueEx			,___RegQueryValueEx},
	{NULL,HRegSetValueEx				,___RegSetValueEx},
	{NULL,HRegFlushKey				,___RegFlushKey},
	{NULL,HRegQueryInfoKey			,___RegQueryInfoKey},
	{NULL,HRegRemapPreDefKey		,___RegRemapPreDefKey},
	{NULL,HRegCreateDynKey			,___RegCreateDynKey},
	{NULL,HRegQueryMultipleValues	,___RegQueryMultipleValues},
	{NULL,HQueryInfoKey16,			0x1D},
	{NULL,HLoadKey,					0x21},
	{NULL,HUnLoadKey,					0x22},
	{NULL,HSaveKey,					0x23},
	{NULL,HReplaceKey,				0x27},
	{NULL,HNotifyChangeKeyValue,	0x29}
};

PDWORD						VMMWin32ServiceTable;

BOOLEAN RegHook(VOID)
{
ULONG	i;
PDDB	vmmDDB;
REGHOOK *CurRH;
	vmmDDB=Get_DDB(VMM_DEVICE_ID,"");
	VMMWin32ServiceTable=(PDWORD)vmmDDB->DDB_Win32_Service_Table+2;
	for(i=0;i<sizeof(RHTable)/sizeof(REGHOOK);i++) {
		CurRH=&RHTable[i];
		if(CurRH->Service & 0xffff0000) {
			CurRH->Real=Hook_Device_Service_C(CurRH->Service,CurRH->Hook,&CurRH->Thunk);
		} else {
			CurRH->Real=(PVOID)VMMWin32ServiceTable[CurRH->Service*2];
			VMMWin32ServiceTable[CurRH->Service*2]=(DWORD)CurRH->Hook;
		}
#ifdef __DBG__
		if(CurRH->Real) {
			DbPrint(DC_REG,DL_NOTIFY, ("%s (%x/%x) hooked\n",VxDRegistryFunctionName[i],CurRH->Service,CurRH->Real));
		} else {
			DbPrint(DC_REG,DL_WARNING, ("!!! Hook %s (%x) failed\n",VxDRegistryFunctionName[i],CurRH->Service));
		}
#endif //__DBG__
	}
	return TRUE;
}

BOOLEAN RegUnHook(VOID)
{
ULONG i;
REGHOOK *CurRH;
	for(i=0;i<sizeof(RHTable)/sizeof(REGHOOK);i++) {
		CurRH=&RHTable[i];
		if(CurRH->Real) {
			if(CurRH->Service & 0xffff0000) {
				Unhook_Device_Service_C(CurRH->Service,&CurRH->Thunk);
			} else {
				VMMWin32ServiceTable[CurRH->Service*2]=(DWORD)CurRH->Real;
			}
			DbPrint(DC_REG,DL_NOTIFY, ("%s (%x/%x) unhooked\n",VxDRegistryFunctionName[i],CurRH->Service,CurRH->Real));
		}
	}
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

NTSTATUS HRegCommon(ULONG index,PCHAR KeyName)
{
	NTSTATUS nResult = ERROR_SUCCESS;
	VERDICT	Verdict;

	PSINGLE_PARAM pSingleParam;
	ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN + 1;
	PVOID RequestData;
	PFILTER_EVENT_PARAM pParam;

	RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
	if (RequestData == NULL)
		return nResult;
	pParam = (PFILTER_EVENT_PARAM) RequestData;

	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_REGS, index, 0, PreProcessing, 1);
	
	pSingleParam = (PSINGLE_PARAM) pParam->Params;
	SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
	pSingleParam->ParamSize = strlen(KeyName) + 1;
	memcpy(pSingleParam->ParamValue, KeyName, pSingleParam->ParamSize);

	DbPrint(DC_REG,DL_INFO, ("%s %s %s\n",GetProcName(pParam->ProcName, NULL),VxDRegistryFunctionName[index], KeyName));
	Verdict = FilterEvent(pParam, NULL);
	if(!_PASS_VERDICT(Verdict)) {
		if(Verdict == Verdict_Kill) {
			DbPrint(DC_REG,DL_NOTIFY, ("Kill %s\n", pParam->ProcName));
			KillCurrentProcess();
		} else if(Verdict == Verdict_Discard) {
			DbPrint(DC_REG,DL_NOTIFY, ("Discard %s\n", KeyName));
		}
		nResult = ERROR_ACCESS_DENIED;
	}

	ExFreePool(RequestData);

	return nResult;
}

#pragma optimize("", off)
LONG HRegOpenKey(HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegOpenKey);
	GetKeyName(hkey,lpszSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,PHKEY))(RHTable[index].Real))(hkey,lpszSubKey,phkResult);
		if(ntstatus==ERROR_SUCCESS)
			NameCacheStore(g_pCommon_NameCache, (ULONG)*phkResult,fullname,strlen(fullname)+1, FALSE, '6boS');
	} else 
		*phkResult=0;
	return ntstatus;
}

LONG HRegCloseKey(HKEY hkey)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegCloseKey);
	GetKeyName(hkey,NULL,NULL,fullname);
	NameCacheFree(g_pCommon_NameCache, (ULONG)hkey);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY))(RHTable[index].Real))(hkey);
	}
	return ntstatus;
}

LONG HRegCreateKey(HKEY hkey, PCHAR lpszSubKey, PHKEY phkResult)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegCreateKey);
	GetKeyName(hkey,lpszSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,PHKEY))(RHTable[index].Real))(hkey,lpszSubKey,phkResult);
		if(ntstatus==ERROR_SUCCESS)
			NameCacheStore(g_pCommon_NameCache, (ULONG)*phkResult,fullname,strlen(fullname)+1, FALSE, '7boS');
	} else 
		*phkResult=0;
	return ntstatus;
}

LONG HRegDeleteKey(HKEY hkey, PCHAR lpszSubKey)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegDeleteKey);
	GetKeyName(hkey,lpszSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR))(RHTable[index].Real))(hkey,lpszSubKey);
	}
	return ntstatus;
}

LONG HRegEnumKey(HKEY hkey, DWORD iSubKey, PCHAR lpszName, DWORD cchName)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegEnumKey);
	GetKeyName(hkey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,DWORD,PCHAR,DWORD))(RHTable[index].Real))(hkey,iSubKey,lpszName,cchName);
	}
	return ntstatus;
}

LONG HRegQueryValue(HKEY hkey, PCHAR lpszSubKey, PCHAR lpszValue, PLONG pcbValue)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegQueryValue);
	GetKeyName(hkey,lpszSubKey,lpszValue,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,PCHAR,PLONG))(RHTable[index].Real))(hkey,lpszSubKey,lpszValue,pcbValue);
	}
	return ntstatus;
}

LONG HRegSetValue(HKEY hkey, PCHAR lpszSubKey, DWORD fdwType, PCHAR lpszData, DWORD cbData)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegSetValue);
	GetKeyName(hkey,lpszSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,DWORD,PCHAR,DWORD))(RHTable[index].Real))(hkey,lpszSubKey,fdwType,lpszData,cbData);
	}
	return ntstatus;
}

LONG HRegDeleteValue(HKEY hkey, PCHAR lpszValue)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegDeleteValue);
	GetKeyName(hkey,NULL,lpszValue,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR))(RHTable[index].Real))(hkey,lpszValue);
	}
	return ntstatus;
}

LONG HRegEnumValue(HKEY hkey, DWORD iValue, PCHAR lpszValue, PDWORD lpcchValue, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegEnumValue);
	GetKeyName(hkey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,DWORD,PCHAR,PDWORD,PDWORD,PDWORD,PBYTE,PDWORD))(RHTable[index].Real))(hkey,iValue,lpszValue,lpcchValue,lpdwReserved,lpdwType,lpbData,lpcbData);
	}
	return ntstatus;
}

LONG HRegQueryValueEx(HKEY hkey, PCHAR lpszValueName, PDWORD lpdwReserved, PDWORD lpdwType, PBYTE lpbData, PDWORD lpcbData)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegQueryValueEx);
	GetKeyName(hkey,NULL,lpszValueName,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,PDWORD,PDWORD,PBYTE,PDWORD))(RHTable[index].Real))(hkey, lpszValueName, lpdwReserved, lpdwType, lpbData,lpcbData);
	}
	return ntstatus;
}

LONG HRegSetValueEx(HKEY hkey, PCHAR lpszValueName, DWORD dwReserved, DWORD fdwType, PBYTE lpbDta, DWORD cbData)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegSetValueEx);
	GetKeyName(hkey,NULL,lpszValueName,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,DWORD,DWORD,PBYTE,DWORD))(RHTable[index].Real))(hkey, lpszValueName, dwReserved, fdwType, lpbDta, cbData);
	}
	return ntstatus;
}

LONG HRegFlushKey(HKEY hkey)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegFlushKey);
	GetKeyName(hkey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY))(RHTable[index].Real))(hkey);
	}
	return ntstatus;
}

DWORD HRegQueryInfoKey(HKEY hKey, PCHAR lpszClass, PDWORD lpcchClass,  DWORD lpdwReserved, PDWORD lpcSubKeys, PDWORD lpcchMaxSubKey, PDWORD  pcchMaxClass, PDWORD lpcValues, PDWORD lpcchMaxValueName, PDWORD lpcbMaxValueData, PDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegQueryInfoKey);
	GetKeyName(hKey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PCHAR,PDWORD,DWORD,PDWORD,PDWORD,PDWORD,PDWORD,PDWORD,PDWORD,PDWORD,PFILETIME))(RHTable[index].Real))(hKey, lpszClass, lpcchClass,  lpdwReserved, lpcSubKeys, lpcchMaxSubKey, pcchMaxClass, lpcValues, lpcchMaxValueName, lpcbMaxValueData, lpcbSecurityDescriptor, lpftLastWriteTime);
	}
	return ntstatus;
}

DWORD HRegRemapPreDefKey(HKEY hKey, HKEY hRootKey)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegRemapPreDefKey);
	GetKeyName(hKey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,HKEY))(RHTable[index].Real))(hKey, hRootKey);
	}
	return ntstatus;
}

DWORD HRegCreateDynKey(PCHAR szName, PVOID KeyContext, PPROVIDER pInfo, PPVALUE pValList, DWORD dwNumVals, HKEY* pKeyHandle)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegCreateDynKey);
	GetKeyName(HKEY_DYN_DATA,szName,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(PCHAR , PVOID , PPROVIDER , PPVALUE , DWORD , HKEY* ))(RHTable[index].Real))(szName, KeyContext, pInfo, pValList, dwNumVals, pKeyHandle);
		if(ntstatus==ERROR_SUCCESS)
			NameCacheStore(g_pCommon_NameCache, (ULONG)*pKeyHandle,fullname,strlen(fullname)+1, FALSE, '7boS');
	} else
		*pKeyHandle=0;
	return ntstatus;
}

DWORD HRegQueryMultipleValues(HKEY hKey, PVALENT pValent, DWORD dwNumVals, PCHAR pValueBuf, PDWORD pTotSize)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HRegQueryMultipleValues);
	GetKeyName(hKey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		ntstatus=((long (__stdcall *)(HKEY,PVALENT,DWORD,PCHAR,PDWORD))(RHTable[index].Real))(hKey,pValent,dwNumVals,pValueBuf,pTotSize);
	}
	return ntstatus;
}

LONG __stdcall HQueryInfoKey16(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PDWORD lpcSubKeys,PDWORD lpcchMaxSubKey,PDWORD lpcValues, PDWORD lpcchMaxValueName,PDWORD lpcbMaxValueData)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HQueryInfoKey16);
	GetKeyName(hKey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		((long (__stdcall *)(PCLIENT_STRUCT ,VMHANDLE ,HKEY ,PDWORD ,PDWORD ,PDWORD , PDWORD ,PDWORD ))(RHTable[index].Real))(pClientRegs,hVM,hKey,lpcSubKeys,lpcchMaxSubKey,lpcValues,lpcchMaxValueName,lpcbMaxValueData);
		ntstatus=pClientRegs->CRS.Client_EAX;
	}
	return ntstatus;
}

LONG __stdcall HLoadKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpSubKey,PCHAR lpFileName)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HLoadKey);
	GetKeyName(hKey,lpSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		((long (__stdcall *)(PCLIENT_STRUCT ,VMHANDLE ,HKEY ,PCHAR ,PCHAR ))(RHTable[index].Real))(pClientRegs,hVM,hKey,lpSubKey,lpFileName);
		ntstatus=pClientRegs->CRS.Client_EAX;
	}
	return ntstatus;
}

LONG __stdcall HUnLoadKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpSubKey)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HUnLoadKey);
	GetKeyName(hKey,lpSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		((long (__stdcall *)(PCLIENT_STRUCT ,VMHANDLE ,HKEY ,PCHAR ))(RHTable[index].Real))(pClientRegs,hVM,hKey,lpSubKey);
		ntstatus=pClientRegs->CRS.Client_EAX;
	}
	return ntstatus;
}

LONG __stdcall HSaveKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpFileName,PVOID SecurAttr)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HSaveKey);
	GetKeyName(hKey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		((long (__stdcall *)(PCLIENT_STRUCT ,VMHANDLE,HKEY ,PCHAR ,PVOID ))(RHTable[index].Real))(pClientRegs,hVM,hKey,lpFileName,SecurAttr);
		ntstatus=pClientRegs->CRS.Client_EAX;
	}
	return ntstatus;
}

LONG __stdcall HReplaceKey(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,PCHAR lpSubKey,PCHAR lpNewFileName,PCHAR OldFileName)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HReplaceKey);
	GetKeyName(hKey,lpSubKey,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		((long (__stdcall *)(PCLIENT_STRUCT ,VMHANDLE ,HKEY ,PCHAR ,PCHAR ,PCHAR ))(RHTable[index].Real))(pClientRegs,hVM,hKey,lpSubKey,lpNewFileName,OldFileName);
		ntstatus=pClientRegs->CRS.Client_EAX;
	}
	return ntstatus;
}

LONG __stdcall HNotifyChangeKeyValue(volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,HKEY hKey,BOOL bWatchSubtree,DWORD dwNotifyFilter,HANDLE hEvent)
{
LONG		ntstatus;
CHAR		fullname[MAXPATHLEN];
ULONG		index;
	index=RegGetIdex((PVOID)HNotifyChangeKeyValue);
	GetKeyName(hKey,NULL,NULL,fullname);
	if((ntstatus=HRegCommon(index,fullname))==ERROR_SUCCESS) {
		((long (__stdcall *)(PCLIENT_STRUCT ,VMHANDLE ,HKEY ,BOOL ,DWORD ,HANDLE ))(RHTable[index].Real))(pClientRegs,hVM,hKey,bWatchSubtree,dwNotifyFilter,hEvent);
		ntstatus=pClientRegs->CRS.Client_EAX;
	}
	return ntstatus;
}

#pragma optimize("", on)
