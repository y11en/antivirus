#include "osspec.h"
#include "debug.h"
#include "structures.h"
#include "drvtypes.h"

#include "defs.h"
#include "hook/HookSpec.h"

//+ ------------------------------------------------------------------------------------------
//+ for windows nt platform

_tpGetPageDescriptor GetPageDescriptor = NULL;
_tpGetPageDescriptorNativeGeneric GetPageDescriptorNative = NULL;
_tpGetPDEEntryNativeLite GetPDEEntryNativeLite = NULL;
_tpGetPageDescriptorNativeLite GetPageDescriptorNativeLite = NULL;

#ifdef _HOOK_NT_
#include "nt/glbenvir.h"

UNICODE_STRING	RegParams;
DWORD ThreadIdOffsetInETHREAD = 0x1e4;
ULONG ZwProtectVirtualMemoryNativeId = -1;
PVOID _MmHighestUserAddress = 0;
PVOID _MmUserProbeAddress = 0;

NTSTATUS (__stdcall *_pfPoCallDriver)(IN PDEVICE_OBJECT DeviceObject, IN OUT PIRP Irp) = 0;
VOID (__stdcall *_pfPoStartNextPowerIrp)(IN PIRP Irp) = 0;

//+ ------------------------------------------------------------------------------------------

NTSTATUS (__stdcall *_pfRtlGetAce)(IN PACL Acl, ULONG AceIndex, OUT PVOID *Ace) = 0;
NTSTATUS (__stdcall *_pfZwSetSecurityObject)(IN HANDLE  Handle, IN SECURITY_INFORMATION  SecurityInformation, 
						IN PSECURITY_DESCRIPTOR  SecurityDescriptor) = 0;

NTSTATUS (__stdcall *_pfZwFsControlFile)(IN HANDLE  FileHandle, IN HANDLE  Event OPTIONAL, 
										 IN PIO_APC_ROUTINE  ApcRoutine OPTIONAL, IN PVOID  ApcContext OPTIONAL, 
										 OUT PIO_STATUS_BLOCK  IoStatusBlock, IN ULONG  FsControlCode, 
										 IN PVOID  InputBuffer OPTIONAL, IN ULONG  InputBufferLength,
										 OUT PVOID  OutputBuffer OPTIONAL, IN ULONG  OutputBufferLength) = 0;

VOID (__stdcall *_pfKeStackAttachProcess)(PKPROCESS Process, OUT ULONG* /*PRKAPC_STATE*/ ApcState) = 0;

VOID (__stdcall *_pfKeUnstackDetachProcess)(/*PRKAPC_STATE*/ ULONG* ApcState) = 0;

HANDLE (__stdcall *_pfPsGetThreadId)(PKTHREAD pkTh) = 0; //ETHREAD*
HANDLE (__stdcall *_pfPsGetThreadProcessId)(PKTHREAD pkTh) = 0;	//ETHREAD*


//+ ------------------------------------------------------------------------------------------


PVOID (__stdcall *_pfMmMapLockedPages)(IN PMDL  MemoryDescriptorList, IN KPROCESSOR_MODE  AccessMode) = 0;

PVOID (__stdcall *_pfMmMapLockedPagesSpecifyCache)(
	IN PMDL  MemoryDescriptorList, IN KPROCESSOR_MODE  AccessMode, IN MEMORY_CACHING_TYPE  CacheType, 
	IN PVOID  BaseAddress, IN ULONG  BugCheckOnFailure, IN MM_PAGE_PRIORITY  Priority) = 0;

BOOLEAN (__stdcall* _pfKeInsertQueueApc)(PKAPC	Apc,PVOID	SystemArgument1,PVOID	SystemArgument2,UCHAR	Mode) = 0;

NTSTATUS (__stdcall* _pfZwFreeVirtualMemory)(IN PHANDLE ProcessHandle, IN OUT PVOID *BaseAddress, 
	 IN OUT PULONG RegionSize, IN ULONG FreeType) = 0;

//+ ------------------------------------------------------------------------------------------
NTSTATUS (__stdcall* _pfIoReadPartitionTable)(IN PDEVICE_OBJECT  DeviceObject,
							  IN ULONG  SectorSize,
							  IN BOOLEAN  ReturnRecognizedPartitions,
							  OUT struct _DRIVE_LAYOUT_INFORMATION  **PartitionBuffer) = 0;

NTSTATUS (__stdcall* _pfIoReadPartitionTableEx)(IN PDEVICE_OBJECT  DeviceObject,
    IN /*struct _DRIVE_LAYOUT_INFORMATION_EX*/VOID  **PartitionBuffer) = NULL; 


#define SizeOfStringBaseNamedObjects 18
WCHAR	WhistleupNameBuffer[PROCNAMELEN+SizeOfStringBaseNamedObjects+1]=L"\\BaseNamedObjects\\";
UNICODE_STRING	uWhistleupName=
	{SizeOfStringBaseNamedObjects*sizeof(WCHAR),
	(PROCNAMELEN+SizeOfStringBaseNamedObjects)*sizeof(WCHAR),
	WhistleupNameBuffer};

UCHAR GetCr4[]={0x0f,0x20,0xe0,0xc3}; //mov eax,cr4;ret

PWCHAR
Uwcsncpy(PWCHAR Dst, ULONG MaxDstLenInChars, PWCHAR Src, ULONG SrcLenInBytes)
{
	ULONG CopyLen;
	CopyLen = SrcLenInBytes/sizeof(WCHAR);
	if(CopyLen > MaxDstLenInChars)
		CopyLen = MaxDstLenInChars - 1;
	memcpy(Dst,Src,CopyLen*sizeof(WCHAR));
	Dst[CopyLen] = 0;
	return Dst;
}

ULONG Uwcslen(PWCHAR pwchName, ULONG BufferLengthInWChars)
{
	ULONG cou = 0;

	while (cou < BufferLengthInWChars)
	{
		if (pwchName[cou] == 0)
			return cou;

		cou++;
	}

	if (cou == 0)
		return 0;

	return BufferLengthInWChars;
}

NTSTATUS
EnumDrivers(PENUM_DRIVERS_FUNC EnumFunc, PVOID pUserValue)
{
	ULONG					i;
	OBJECT_ATTRIBUTES	ObjAttr;
	NTSTATUS				ntStatus;
	HANDLE				hDirectoryObject;
	BOOLEAN				First;
	POBJECT_NAMETYPE_INFO p;
	HANDLE				DrvHandle;
	PDRIVER_OBJECT		pDrv;
	UNICODE_STRING		Name;
	PWCHAR				DrvName;
	PWCHAR				DriverStr=L"Driver";
	WCHAR					Dir[128]=L"\\FileSystem";
	ULONG					RegionSize;
	PMY_QUERY_DIRECTORY_STRUCT qds;
	qds=NULL;
	RegionSize=sizeof(MY_QUERY_DIRECTORY_STRUCT);
	if(!NT_SUCCESS(ntStatus=ZwAllocateVirtualMemory(NtCurrentProcess(), (PVOID*) &qds,0,&RegionSize,MEM_COMMIT,PAGE_READWRITE))) {
		//                                                            ^ - Whistler beta 2 doesn't work with 8
		DbPrint(DC_SYS,DL_ERROR,("ZwAllocateVirtualMemory fail. Status=%x\n", ntStatus));
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	for(i=0;i<2;i++) {
		RtlInitUnicodeString(&Name,Dir);
		InitializeObjectAttributes(&ObjAttr,&Name,OBJ_CASE_INSENSITIVE,NULL,NULL);
		ntStatus=ZwOpenDirectoryObject(&hDirectoryObject,DIRECTORY_QUERY,&ObjAttr);
		if(!NT_SUCCESS(ntStatus)) {
			DbPrint(DC_SYS,DL_ERROR,("ZwOpenDirectoryObject %S failed, status %x\n",Dir, ntStatus));
			RegionSize=0;
			_pfZwFreeVirtualMemory(NtCurrentProcess(),(PVOID*) &qds,&RegionSize,MEM_RELEASE);
			return ntStatus;
		}
		First=TRUE;
		DrvName=Dir+wcslen(Dir);
		*DrvName++='\\';
		while(NtQueryDirectoryObject(hDirectoryObject,&qds->Buffer,QUERY_DIRECTORY_BUFF_SIZE,TRUE,First,&qds->Index,&qds->Retlen)>=0) {
			p=(POBJECT_NAMETYPE_INFO)&qds->Buffer;
			First=FALSE;
			*DrvName=0;
			if(wcsncmp(p->ObjectType.Buffer,DriverStr,6)) continue;
			wcscat(Dir,p->ObjectName.Buffer);
			RtlInitUnicodeString(&Name,Dir);
			InitializeObjectAttributes(&ObjAttr,&Name,OBJ_CASE_INSENSITIVE,NULL,NULL);
			ntStatus=ObOpenObjectByName(&ObjAttr,0,0,0,STANDARD_RIGHTS_REQUIRED,0,&DrvHandle);
			if(NT_SUCCESS(ntStatus)) {
				ntStatus=ObReferenceObjectByHandle(DrvHandle,STANDARD_RIGHTS_REQUIRED,NULL,KernelMode,(VOID**)&pDrv,NULL);
				DbPrint(DC_SYS,DL_INFO,("Open %S %S handle=%x Driver=%x\n",p->ObjectType.Buffer,Dir,DrvHandle,pDrv));
				ZwClose(DrvHandle);
				if(NT_SUCCESS(ntStatus)) {
					if(EnumFunc)
						EnumFunc(pDrv, pUserValue);
					ObDereferenceObject(pDrv);
				}
			}
		}
		ZwClose(hDirectoryObject);
		wcscpy(Dir+1,DriverStr);
	}
	RegionSize=0;
	_pfZwFreeVirtualMemory(NtCurrentProcess(),(PVOID*) &qds,&RegionSize,MEM_RELEASE);
	return STATUS_SUCCESS;
}

BOOLEAN OpenWhistleupper(PCHAR Name,HANDLE *phWhistleUp,PKEVENT *pWhistleUp)
{
BOOLEAN ret;
ANSI_STRING TmpAName;
UNICODE_STRING TmpUName;
	if(!Name)
		return TRUE;
	ret=FALSE;
	*pWhistleUp=NULL;
	*phWhistleUp=NULL;
	uWhistleupName.Length=SizeOfStringBaseNamedObjects*sizeof(WCHAR);
	uWhistleupName.Buffer[SizeOfStringBaseNamedObjects]=0;
	RtlInitAnsiString(&TmpAName,Name);
	if(RtlAnsiStringToUnicodeString(&TmpUName,&TmpAName,TRUE)==STATUS_SUCCESS) {
		if(RtlAppendUnicodeStringToString(&uWhistleupName,&TmpUName)==STATUS_SUCCESS) {
			if((*pWhistleUp=IoCreateSynchronizationEvent(&uWhistleupName,phWhistleUp))) {
				ret=TRUE;
			}
		}
		RtlFreeUnicodeString(&TmpUName);
	}
	return ret;
}

VOID CloseWhistleupper(HANDLE *hWhistleUp)
{
	DbPrint(DC_SYS,DL_NOTIFY, ("CloseWhistleupper called\n"));
	if(*hWhistleUp)
	{
		HANDLE hWhistleUpTmp = *hWhistleUp;
		*hWhistleUp = NULL;
		ZwClose(hWhistleUpTmp);
	}
}

PCHAR GetProcName(PCHAR ProcessName, PVOID pIrp)
{
	PEPROCESS curproc;
	PIRP Irp = (PIRP) pIrp;
	if(Irp)
		curproc=IoGetRequestorProcess(Irp);
	else
		curproc=IoGetCurrentProcess();
	__try {
		if(curproc && ProcessNameOffset) {
			strncpy(ProcessName,(PCHAR)curproc+ProcessNameOffset,PROCNAMELEN-1);
			return ProcessName;
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		DbPrint(DC_SYS,DL_ERROR, ("exeption on GetProcName\n"));
	}
	strcpy(ProcessName,UnknownStr);
	return ProcessName;
}

PVOID GetNativeBase(PCHAR DllName, ULONG ValidateAddress)
{
ULONG		NtStatus;
ULONG		BufLen;
ULONG		i;
PVOID		ret;
PULONG	qBuff;
PSYSTEM_MODULE_INFORMATION Mod;
	ret=NULL;
	NtStatus=ZwQuerySystemInformation(11,&BufLen,0,&BufLen);//STATUS_INFO_LENGTH_MISMATCH
	qBuff=ExAllocatePoolWithTag(PagedPool,BufLen,'-SeB');
	if(qBuff) {
		NtStatus=ZwQuerySystemInformation(11,qBuff,BufLen,NULL);
		if(NtStatus==STATUS_SUCCESS) {
			Mod=(PSYSTEM_MODULE_INFORMATION)(qBuff+1);
			for(i=0; i < *qBuff; i++) {
//				DbPrint(DC_SYS,DL_INFO,("(%d) Base of %s=0x%x Size=%d Flags=0x%x LoadCnt=%d FullName: %s\n",
//					Mod[i].Index, Mod[i].ImageName+Mod[i].ModNameOffset,Mod[i].Base,Mod[i].Size,
//					Mod[i].Flags, Mod[i].LoadCount,Mod[i].ImageName ));
				if(!_stricmp(Mod[i].ImageName+Mod[i].ModNameOffset,DllName))
				{
					if (ValidateAddress)
					{
						if (((ULONG) Mod[i].Base > ValidateAddress) || (ValidateAddress > ((ULONG) Mod[i].Base + Mod[i].Size)))
						{
							DbgBreakPoint();
							continue;
						}
					}
					
					DbPrint(DC_SYS,DL_IMPORTANT,("(%d) Native base of %s=%x Size=0x%x Flags=0x%x LoadCnt=%d FullName: %s\n",
						Mod[i].Index, Mod[i].ImageName+Mod[i].ModNameOffset,Mod[i].Base,Mod[i].Size,
						Mod[i].Flags, Mod[i].LoadCount,Mod[i].ImageName ));
					ret=Mod[i].Base;
					break;
				}
			}
		} else {
			DbPrint(DC_SYS,DL_IMPORTANT,("QuerySystemInformation for system modules fail with status %x\n",NtStatus))
		}
		ExFreePool(qBuff);
	}
	return ret;
}

BOOLEAN IsLargePageNative(PVOID VirtAddr)
{
	ULONG *tmp;
	ULONG *PageDir;
	ULONG myCr3;
	ULONG myCr4;
	ULONG resultaddr;
	ULONG PageTable;
	
	myCr4=((LONG(__stdcall *)(VOID))(GetCr4))();
	if(myCr4 & CR4_PAE)//4Kb & 2Mb - 36bit address
		return TRUE; 
	if(!(myCr4 & CR4_PSE))
		return FALSE;
	TouchPage(VirtAddr);
	PageTable = 0xC0300000 + ((ULONG)VirtAddr >> 22) * 4; 
	if(!(*(ULONG*)PageTable & PDE_P))
	{
		DbPrint(DC_SYS,DL_ERROR, ("IsLargePageNative (Va=%x) PageTable not present\n",VirtAddr));
		DbgBreakPoint();
		return FALSE; 
	}
	if(*(ULONG*)PageTable & PDE_PS)
		return TRUE;
	return FALSE;
}

BOOLEAN MyVirtualProtect(PVOID VAddr,BOOLEAN Protect, BOOLEAN *OldProtect)
{
	BOOLEAN		ret=FALSE;
	ULONG*		pPatchDataDesc;
	ULONG			OrigDescr;

	TouchPage(VAddr);
	if((pPatchDataDesc = (ULONG*)GetPageDescriptorNative(VAddr))!=BADP2LMAPPING)
	{
		__try {
			TouchPage(VAddr);
			if(*pPatchDataDesc & PDE_P) {
				OrigDescr=*pPatchDataDesc;
				if(Protect) {
					__asm {
						mov eax,[pPatchDataDesc];
						lock or dword ptr [eax],PDE_W
					}
					//					*pPatchDataDesc|=PDE_W;
				}	else {
					__asm {
						mov eax,[pPatchDataDesc];
						lock and dword ptr [eax],0xFFFFFFFD //~PDE_W;
					}
					// *pPatchDataDesc &= ~PDE_W;
				}
				if(OldProtect)
					*OldProtect=(BOOLEAN)(OrigDescr & PDE_W);
				ret=TRUE;
				//Flush TLB
				__asm {
					mov eax,VAddr;
					invlpg [eax];
				}
			}
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			DbPrint(DC_SYS,DL_ERROR, ("MyVirtualProtect exception! VAddr 0x%x, Protect %d\n", VAddr, Protect));
		}
	}
	return ret;
}


BOOLEAN DynamicLinkFunc(PVOID Base, ULONG* pFunc, PCHAR FuncName)
{
	if (!Base)
	{
		*pFunc = 0;
		DbPrint(DC_SYS, DL_ERROR,("%s not found - wrong pBase\n", FuncName));
		return FALSE;
	}

	*pFunc = (ULONG) GetExport(Base, FuncName, NULL);
	if (*pFunc)
		return TRUE;

	DbPrint(DC_SYS, DL_ERROR,("%s not found\n", FuncName));
	return FALSE;
}

BOOLEAN GetNativeAddresses(VOID)
{
	ULONG					NativeID;

	if (!DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfMmMapLockedPagesSpecifyCache, "MmMapLockedPagesSpecifyCache"))
	{
		if (!DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfMmMapLockedPages, "MmMapLockedPages"))
			return FALSE;
	}


	if (*NtBuildNumber > 1381)
	{
		if (!DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfRtlGetAce, "RtlGetAce"))
			return FALSE;

		if (!DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfZwSetSecurityObject, "ZwSetSecurityObject"))
			return FALSE;

		if (DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfKeUnstackDetachProcess, "KeUnstackDetachProcess"))
			DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfKeStackAttachProcess, "KeStackAttachProcess");
	}

	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfZwFsControlFile, "ZwFsControlFile");
	
	if (!DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfKeInsertQueueApc, "KeInsertQueueApc"))
		return FALSE;

	if (!DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfZwFreeVirtualMemory, "ZwFreeVirtualMemory"))
		return FALSE;

	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfIoReadPartitionTable, "IoReadPartitionTable");
	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfIoReadPartitionTableEx, "IoReadPartitionTableEx");

	if ((NULL == _pfIoReadPartitionTable) && (NULL == _pfIoReadPartitionTableEx))
	{
		DynamicLinkFunc(BaseOfHal, (ULONG*) &_pfIoReadPartitionTable, "IoReadPartitionTable");
		DynamicLinkFunc(BaseOfHal, (ULONG*) &_pfIoReadPartitionTableEx, "IoReadPartitionTableEx");

		if ((NULL == _pfIoReadPartitionTable) && (NULL == _pfIoReadPartitionTableEx))
			return FALSE;
	}

	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfPoCallDriver, "PoCallDriver");
	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfPoStartNextPowerIrp, "PoStartNextPowerIrp");
	
	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfPsGetThreadId, "PsGetThreadId");
	DynamicLinkFunc(BaseOfNtOsKrnl, (ULONG*) &_pfPsGetThreadProcessId, "PsGetThreadProcessId");


	// Find undocumented calls addresses
	__try
	{
		if(!(NativeID = GetNativeID((PVOID)BaseOfNtDllDll,"NtQueryDirectoryObject")))
		{
			DbPrint(DC_SYS, DL_ERROR,("GetNativeID for NtQueryDirectoryObject failed!\n"));
			return FALSE;
		}
		NtQueryDirectoryObject = (long (__stdcall *)(void *,void *,unsigned long ,unsigned char ,unsigned char ,unsigned long *,unsigned long *))SYSTEMSERVICE_BY_FUNC_ID(NativeID);
		DbPrint(DC_SYS, DL_INFO,("NtQueryDirectoryObject ID=%x Addr=%x\n", NativeID, NtQueryDirectoryObject));

		if(!(NativeID = GetNativeID((PVOID)BaseOfNtDllDll,"NtReadVirtualMemory")))
		{
			DbPrint(DC_SYS, DL_ERROR,("GetNativeID for NtReadVirtualMemory failed!\n"));
			return FALSE;
		}

		if(!(NativeID = GetNativeID((PVOID)BaseOfNtDllDll,"NtQueryInformationThread")))
		{
			DbPrint(DC_SYS, DL_ERROR,("GetNativeID for NtQueryInformationThread failed!\n"));
			return FALSE;
		}
		NtQueryInformationThread = (long (__stdcall *)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG))SYSTEMSERVICE_BY_FUNC_ID(NativeID);

		NtReadVirtualMemory = (long (__stdcall *)(HANDLE, PVOID, PVOID, ULONG, PULONG))SYSTEMSERVICE_BY_FUNC_ID(NativeID);
		DbPrint(DC_SYS,DL_INFO,("NtReadVirtualMemory ID=%x Addr=%x\n", NativeID, NtReadVirtualMemory));
		
		if(!(ZwProtectVirtualMemoryNativeId = GetNativeID((PVOID)BaseOfNtDllDll,"NtProtectVirtualMemory")))
		{
			DbPrint(DC_SYS, DL_ERROR,("GetNativeID for NtProtectVirtualMemory failed!\n"));
			return FALSE;
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		DbPrint(DC_SYS,DL_ERROR, ("exeption on GetNativeAddresses\n"));
		return FALSE;
	}

	return TRUE;
}

WCHAR
FindDrvLetter(PUNICODE_STRING pUniname)
{
	WCHAR wchRet = 0;
	static WCHAR        targetNameBuffer[260];
	UNICODE_STRING      targetNameUnicodeString;
	OBJECT_ATTRIBUTES	ObjAttr;
	UNICODE_STRING		us;
	HANDLE				hDir;
	HANDLE				hLink;
	NTSTATUS			ntStatus;
	WCHAR				cDrive[] = L"A:";
	IO_STATUS_BLOCK		ioStatus;
	PFILE_OBJECT			fileObject;
	PDEVICE_OBJECT		DevObj;
	
	RtlInitUnicodeString(&us, (PWCHAR)L"\\??");
	InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntStatus=ZwOpenDirectoryObject(&hDir,DIRECTORY_QUERY,&ObjAttr);
	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_FILE,DL_ERROR, ("ZwOpenDirectoryObject %S failed, status %x\n",us.Buffer, ntStatus));
		return wchRet;
	}
	
	for(cDrive[0] = L'A'; cDrive[0] <= L'Z'; cDrive[0]++)
	{
		DevObj = NULL;
		RtlInitUnicodeString(&us, cDrive);
		InitializeObjectAttributes(&ObjAttr, &us, OBJ_CASE_INSENSITIVE, hDir, NULL);
		ntStatus = ZwOpenSymbolicLinkObject(&hLink,SYMBOLIC_LINK_QUERY,&ObjAttr);
		if(NT_SUCCESS(ntStatus))
		{
			RtlZeroMemory(targetNameBuffer,sizeof(targetNameBuffer));
			targetNameUnicodeString.Buffer = targetNameBuffer;
			targetNameUnicodeString.MaximumLength = sizeof(targetNameBuffer);
			ntStatus = ZwQuerySymbolicLinkObject(hLink, &targetNameUnicodeString, NULL);
			ZwClose(hLink);
			if(NT_SUCCESS(ntStatus))
			{
				if (RtlCompareUnicodeString(pUniname, &targetNameUnicodeString, FALSE) == 0)
				{
					wchRet = cDrive[0];
					break;
				}
			}
			else
			{
				DbPrint(DC_FILE,DL_ERROR, ("ZwQuerySymbolicLinkObject %S failed status=%x\n",us.Buffer,ntStatus));
			}
		}
	}
	ZwClose(hDir);

	return wchRet;
}

ULONG
GetDeviceNameUnistring(PDEVICE_OBJECT pDevice, POBJECT_NAME_INFORMATION pObjectNameInfo, ULONG InfoBufferSize)
{
	ULONG PathLen = 0;
	if(NT_SUCCESS(ObQueryNameString(pDevice, pObjectNameInfo, InfoBufferSize, &PathLen)))
		return PathLen;

	return 0;
}

#include "namecache.h"

WCHAR
GetDeviceLetter(PDEVICE_OBJECT pDevice)
{
	WCHAR letter;
	BYTE DrvName[512];
	POBJECT_NAME_INFORMATION pObjectNameInfo = (POBJECT_NAME_INFORMATION) DrvName;
	ULONG NameSize;
	BOOLEAN bOver;
	
	if (NameCacheGet(g_pNameCache_DriveLetter, (ULONG) pDevice, DrvName, sizeof(DrvName), &bOver))
		return *((WCHAR*) DrvName);

	NameSize = GetDeviceNameUnistring(pDevice, pObjectNameInfo, sizeof(DrvName));
	if (!NameSize)
		return 0;

	letter = FindDrvLetter(&pObjectNameInfo->Name);
	if (!letter)
	{
		if (FILE_DEVICE_NETWORK_FILE_SYSTEM == pDevice->DeviceType)
		{
			letter = '\\';
		}
		else if (FILE_DEVICE_DFS == pDevice->DeviceType)
		{
			BOOL  found = FALSE;
			ULONG i, len = pObjectNameInfo->Name.Length / sizeof(WCHAR);
			PWCHAR pName = (PWCHAR)pObjectNameInfo->Name.Buffer;
			for (i=0; i<len; i++)
			{
				if (pName[i] == ':')
				{
					found = TRUE;
					break;
				}
			}
			if (!found) // there is no drive letter (\Device\WinDfs\Root) => was just direct access via \\avp.ru\dfs\blablabla\file
				letter = '\\';
		}

	}
	
	return letter;
}

//BuffLen in chars
PWCHAR
GetDeviceName(PDEVICE_OBJECT pDevice, PWCHAR pDeviceName,ULONG BuffLen)
{
	PWCHAR Ret = NULL;
	POBJECT_NAME_INFORMATION nameInfo;
	ULONG	pathLen = 0;
	ULONG qbuflen;
	NTSTATUS ntStatus;
	qbuflen=BuffLen * sizeof(WCHAR)+ sizeof(UNICODE_STRING);
	if((nameInfo = ExAllocatePoolWithTag(NonPagedPool,qbuflen,'FSeB')))
	{
		nameInfo->Name.Length=0;
		nameInfo->Name.Buffer=NULL;
		nameInfo->Name.MaximumLength = (USHORT)(BuffLen * sizeof(WCHAR));
		ntStatus=ObQueryNameString(pDevice, nameInfo, qbuflen, &pathLen);
		if(NT_SUCCESS(ntStatus) && nameInfo->Name.Buffer)
		{
			Uwcsncpy(pDeviceName, BuffLen, nameInfo->Name.Buffer, nameInfo->Name.Length);
			Ret = pDeviceName;
		}
		else
			DbPrint(DC_FILE,DL_SPAM, ("GetDeviceName fail for obj=%x Status=%x\n", pDevice,ntStatus));

		ExFreePool(nameInfo);
	}
	
	return Ret;
}

ULONG
GetProcessIDByHandle(HANDLE ProcessHandle, PPEB* ppPeb)
{
	NTSTATUS ntStatus;
	PROCESS_BASIC_INFORMATION ProcInfo;
	
	ntStatus = ZwQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), NULL);
	if (STATUS_SUCCESS == ntStatus)
	{
		if (ppPeb)
			*ppPeb = ProcInfo.PebBaseAddress;

		return ProcInfo.UniqueProcessId;
	}
	
	return 0;
}

BOOLEAN
HookNtFunc(ULONG* InterceptFunc, ULONG NewHook, PCHAR Description)
{
	BOOLEAN	bRet = FALSE;
	ULONG		NativeID;
	BOOLEAN		OldProtect;

	NativeID = GetNativeID(BaseOfNtDllDll, Description);
	if (NativeID)
	{
		*InterceptFunc = SYSTEMSERVICE_BY_FUNC_ID(NativeID);
		
		if(MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase + NativeID, TRUE, &OldProtect))
		{
			SYSTEMSERVICE_BY_FUNC_ID(NativeID) = NewHook;
			MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase + NativeID,OldProtect,&OldProtect);
			DbPrint(DC_SYS,DL_IMPORTANT,("%s ID = %x Addr = %x hooked\n", Description, NativeID, *InterceptFunc));
			bRet = TRUE;
		}
	}

	if (!bRet)
	{
		DbPrint(DC_SYS,DL_IMPORTANT,("%s hook failed\n", Description, NativeID, *InterceptFunc));
	}
	return bRet;
}

ULONG GetAddrOfShadowTable();
// Gruzdev ---------------------------------------------------------------------------------------
PVOID GetNtFuncByID(IN ULONG Service_ID, PVOID **p_pfServiceFunc OPTIONAL)
{
// если syscall к ntoskrnl.exe
	if (!(Service_ID & 0x3000))
	{
		if (p_pfServiceFunc)
			*p_pfServiceFunc = (PVOID *)&(SYSTEMSERVICE_BY_FUNC_ID(Service_ID));

		return (PVOID)SYSTEMSERVICE_BY_FUNC_ID(Service_ID);		
	}
// иначе к win32k.sys
	else
	{
		PServiceDescriptorTableEntry_t pKeServiceDescriptorTableShadow,
									   pWIN32KServiceDescriptorTable;
		ULONG WIN32K_ServiceID;
		PVOID *ppService;

		pKeServiceDescriptorTableShadow = (PServiceDescriptorTableEntry_t)GetAddrOfShadowTable();
		if (pKeServiceDescriptorTableShadow)
		{
// таблица win32k.sys с индексом 1
			pWIN32KServiceDescriptorTable = pKeServiceDescriptorTableShadow + 1;
			WIN32K_ServiceID = Service_ID & 0x0FFF;

			ppService = (PVOID *)(pWIN32KServiceDescriptorTable->ServiceTableBase + WIN32K_ServiceID);
			if (pWIN32KServiceDescriptorTable->ServiceTableBase && MmIsAddressValid(ppService))
			{
				if (p_pfServiceFunc)
					*p_pfServiceFunc = ppService;

				return *ppService;
			}
		}
	}

	return NULL;
}

BOOLEAN HookNtFuncByID(PVOID* InterceptFunc, PVOID NewHook, ULONG NativeID)
{
	BOOLEAN	bRet = FALSE;
	BOOLEAN		OldProtect;

// если syscall к ntoskrnl.exe
	if (!(NativeID & 0x3000))
	{
		*InterceptFunc = (PVOID)SYSTEMSERVICE_BY_FUNC_ID(NativeID);
		
		if(MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase + NativeID, TRUE, &OldProtect))
		{
			(PVOID)SYSTEMSERVICE_BY_FUNC_ID(NativeID) = NewHook;
			MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase + NativeID, OldProtect, &OldProtect);
			bRet = TRUE;
		}
	}
// иначе к win32k.sys
	else
	{
		PServiceDescriptorTableEntry_t pKeServiceDescriptorTableShadow,
									   pWIN32KServiceDescriptorTable;
		ULONG WIN32K_ServiceID;
		PVOID *ppService;

		pKeServiceDescriptorTableShadow = (PServiceDescriptorTableEntry_t)GetAddrOfShadowTable();
		if (pKeServiceDescriptorTableShadow)
		{
// таблица win32k.sys с индексом 1
			pWIN32KServiceDescriptorTable = pKeServiceDescriptorTableShadow + 1;
			WIN32K_ServiceID = NativeID & 0x0FFF;

			ppService = (PVOID *)(pWIN32KServiceDescriptorTable->ServiceTableBase + WIN32K_ServiceID);
			if (pWIN32KServiceDescriptorTable->ServiceTableBase && MmIsAddressValid(ppService))
			{
				*InterceptFunc = *ppService;

				if(MyVirtualProtect(ppService, TRUE, &OldProtect))
				{
					*ppService = NewHook;
					MyVirtualProtect(ppService, OldProtect, &OldProtect);
					
					bRet = TRUE;
				}
			}
		}
	}

	if (bRet)
	{
		DbPrint(DC_SYS,DL_IMPORTANT,("HookNtFuncByID: ID = %x Addr = %x hooked\n", NativeID, *InterceptFunc));
	}
	else
	{
		DbPrint(DC_SYS,DL_IMPORTANT,("HookNtFuncByID: hook failed ID = %x\n", NativeID));
	}

	return bRet;
}
//-------------------------------------------------------------------------------------------------

ULONG
GetProcessNameOffset()
{
	int i;
	PEPROCESS pSystemProcess = IoGetCurrentProcess();
	
	for(i = 0;i < 0x3000; i++)
	{
		if(!strncmp(SYSNAME, (PCHAR)pSystemProcess + i, strlen(SYSNAME)))
			return i;
	}

	return 0;
}

ULONG
GetRequestorThreadId(PIRP Irp)
{
	PETHREAD Th;
	if(Irp) {
		Th=Irp->Tail.Overlay.Thread;
		if((PVOID)Th>_MmHighestUserAddress) {
			return *(ULONG*)((CHAR*)Th+ThreadIdOffsetInETHREAD);
		}
	}
	return 0;
}

BOOLEAN
GetThreadIdOffsetInETHREAD(VOID)
{
	PVOID PsGetCurrentThreadIdEP;
	PVOID *pMmHighestUserAddress;
	if((pMmHighestUserAddress=GetExport(BaseOfNtOsKrnl,"MmHighestUserAddress",NULL))){
		_MmHighestUserAddress=*pMmHighestUserAddress;
	}
	if((PsGetCurrentThreadIdEP=GetExport(BaseOfNtOsKrnl,"PsGetCurrentThreadId",NULL))) {
		if((*(WORD*)PsGetCurrentThreadIdEP==0xA164) &&	 //	mov eax, large fs:XXXXXXXX
			(*((WORD*)PsGetCurrentThreadIdEP+3)==0x808B) &&//	mov eax, [eax+XXXXXXXX]
			(*((BYTE*)PsGetCurrentThreadIdEP+12)==0xc3)) { //	retn   
			ThreadIdOffsetInETHREAD=*((DWORD*)PsGetCurrentThreadIdEP+2);
			return TRUE;
		}
	}
	DbPrint(DC_SYS,DL_ERROR, ("!!! GetThreadIdOffsetInETHREAD fail\n"));
	return FALSE;
}

#endif //_HOOK_NT_

//+ ------------------------------------------------------------------------------------------
//+ windows 9x platform
#ifdef _HOOK_VXD_

PCHAR RegParams="SOFTWARE\\KasperskyLab\\"AVPGNAME"\\Parameters";
ULONG Obsfucator = 0;
PWIN16MUTEX	pWin16Mutex = 0;
ULONG W16mtxOThOffset = 0x10;
BOOLEAN bUpperTableInited = FALSE;

PCHAR GetProcName(PCHAR ProcessName, PVOID _Ignore)
{
PVOID	CurProc;
PVOID	ring3proc;
char	*name,*pname;
ULONG	i;
VMHANDLE CurVM;
	CurVM=Get_Cur_VM_Handle();
	if(Test_Sys_VM_Handle(CurVM) && (CurProc=VWIN32_GetCurrentProcessHandle())!=NULL) {
		if((ring3proc=(PVOID)SelectorMapFlat(CurVM,(DWORD)(*(PDWORD)((char *)CurProc+0x38))|0x7,0))==(PVOID)-1) {
			DbPrint(DC_SYS,DL_WARNING,("GetProcName. SelectorMapFlat failed.\n"));
			strcpy(ProcessName,UnknownStr);
		} else {
			pname=ProcessName;
			name=((char*)ring3proc)+0xF2;
			for(i=0;i<8 && *name;i++)
				*pname++=*name++;
			*pname=0;
			if(i==0) {
//				DbPrint(DC_SYS,DL_WARNING,("GetProcName. ZeroLenName.\n"));
				strcpy(ProcessName,UnknownStr);
			}
		}
	} else {
		sprintf(ProcessName,"VM%d",CurVM->CB_VMID);
		/* Кронгениально Киса! 
		     Но есть неудобство:
		     Здесь полный путь который могет не влезть в короткий PROCNAMELEN (а искать сначала последний бекслэш ломает)
		PBYTE PSP,EnvMCB;
		ULONG i;
		PSP = (PBYTE)(CurVM->CB_High_Linear + ((*(PWORD)(CurVM->CB_High_Linear + ((DWORD)DOSMGR_Get_IndosPtr() + 0x10))) << 4));
		if(*(PWORD)PSP==0x20CD) { // int 0x20
			EnvMCB = (PBYTE)(CurVM->CB_High_Linear +((*(PWORD)&PSP[0x2c]) << 4) - 0x10);
			if(EnvMCB[0]!='M' && EnvMCB[0]!='Z') {
				DWORD   j=0;
				for (i=8;i < 0x10;i++) {
					if (isalnum((int)PSP[i-0x10]))
						((PBYTE)ProcessName)[j++] = PSP[i-0x10];
					else break;
				}
			} else {
				for (i=0;i < (*(PWORD)&EnvMCB[3]) * 0x10;i++) {
					if (*(PWORD)&EnvMCB[0x10+i] == 0x1) {
						strncpy(ProcessName,&EnvMCB[0x10+i+2],PROCNAMELEN-1);
						ProcessName[PROCNAMELEN]=0;
						break;
					}
				}
			}
		} else {
			sprintf(ProcessName,"VM%d",CurVM->CB_VMID);
		}
		*/
	}
	return ProcessName;
}

BYTE _UpperCharA(BYTE ch)	//!! nehorosho
{
static CHAR UpperTable[257];
DWORD cou;
	if (bUpperTableInited == FALSE) {
		for (cou = 0; cou < sizeof(UpperTable); cou++) {
			UpperTable[cou] = (CHAR)cou;
		}
		UpperTable[sizeof(UpperTable) ] = 0;
//		_strupr(UpperTable + 1); 		//Compuware КАAAAAЗЛЫ!!!!
		__asm mov eax, offset UpperTable
		__asm inc eax
		__asm push eax
		VxDCall (_strupr);
		__asm add esp,4

		bUpperTableInited = TRUE;
	}
	return UpperTable[ch];
/*	if (ch>='a' && ch<='z')
		ch = ch - 0x20;
		//return _toupper(ch);
	return ch;*/
}

void SetObsfucator(DWORD obsf)
{
		Obsfucator = obsf ^ (DWORD)VWIN32_GetCurrentProcessHandle();
}

BOOLEAN OpenWhistleupper(PCHAR Whistleup,HANDLE *phWhistleUp,PVOID null)
{
	*phWhistleUp=*(HANDLE*)Whistleup;
	return TRUE;
}

VOID CloseWhistleupper(HANDLE *hWhistleUp)
{
	if(*hWhistleUp)
		VWIN32_CloseVxDHandle(*hWhistleUp);
	*hWhistleUp=NULL;
}

//IFSMgr_Ring0GetDriveInfo

ULONG __GetDriveType(BYTE DrvNum) //(00h = default, 01h = A:, etc)
{
	ALLREGS Regs; 

	DbPrint(DC_SYS,DL_NOTIFY, ("__GetDriveType : param %d\n", DrvNum));
/*
 Windows95 - LONG FILENAME - GET VOLUME INFORMATION
        AX = 71A0h
        DS:DX -> ASCIZ root name (e.g. "C:\")
        ES:DI -> buffer for file system name
        CX = size of ES:DI buffer
Return: CF clear if successful
            AX destroyed (0000h and 0200h seen)
            BX = file system flags (see #01783)
            CX = maximum length of file name [usually 255]
            DX = maximum length of path [usually 260]

            ES:DI buffer filled (ASCIZ, e.g. "FAT","NTFS","CDFS")
        CF set on error
            AX = error code
                7100h if function not supported
*/
	{
	VMHANDLE			hVM;
	DWORD				Actual;
	SEGOFFSET		pBuffer;
	DWORD				BuffSize=64;
	CHAR				str[64];
	CLIENT_STRUCT	SavedRegs;
	CLIENT_STRUCT	*pRegs;
	ULONG				ret;
	WORD				CopySeg;
	ret=0;//важно для V86MMGR_Free_Buffer
	sprintf(str,"%c:\\",DrvNum+'A'-1);
	hVM=Get_Cur_VM_Handle();
	pRegs=(CLIENT_STRUCT*)hVM->CB_Client_Pointer;
	Save_Client_State(&SavedRegs);
	_asm {
		mov ax, ss;// str должна лежать на стеке
		mov CopySeg, ax;
	}
	if(V86MMGR_Allocate_Buffer(hVM,pRegs,BuffSize,CopySeg,str,TRUE,&Actual,&pBuffer)) {
		if(Actual>=BuffSize) {
			//DbPrint(DC_SYS, DL_NOTIFY, ("V86MMGR_Allocate_Buffer - allocated\n"));		
			Begin_Nest_V86_Exec();
			//DbPrint(DC_SYS, DL_NOTIFY, ("Begin_Nest_V86_Exec.. done\n"));		
			_clientAX=0x71A0;
			_clientDS=HIWORD(pBuffer);
			_clientES=HIWORD(pBuffer);
			_clientDX=LOWORD(pBuffer);
			_clientDI=LOWORD(pBuffer)+(WORD)(BuffSize/2);
			_clientCX=(WORD)(BuffSize/2);
			Exec_Int(0x21);
			if(_clientFlags & 1) {
				DbPrint(DC_SYS,DL_ERROR, ("I2171A0 return carry flag\n"));
			} else {
				ret=1;
				//DbPrint(DC_SYS, DL_NOTIFY, ("Exec_Int(0x21) complete\n"));
			}
			End_Nest_Exec();
		} else {
			DbPrint(DC_SYS,DL_ERROR, ("V86MMGR_Allocate_Buffer - too small buffer\n"));
		}
		_asm {
			mov ax, ss;// str должна лежать на стеке
			mov CopySeg, ax;
		}
		V86MMGR_Free_Buffer(hVM,pRegs,Actual,CopySeg,str,ret);
	} else {
		DbPrint(DC_SYS,DL_ERROR, ("V86MMGR_Allocate_Buffer failed\n"));
	}
	Restore_Client_State(&SavedRegs);
	if(ret) {
		DbPrint(DC_SYS,DL_INFO, ("__GetDriveType (71A0) for drive %c: detect %s\n",DrvNum+'A'-1,str+BuffSize/2));
		if(!strcmp(str+BuffSize/2,"CDFS")) {
			DbPrint(DC_SYS,DL_INFO, ("__GetDriveType for drive %c: return DRIVE_CDROM\n",DrvNum+'A'-1));
			return DRIVE_CDROM;
		}
	}
	}

/* ------------------------------------------------------------------------------------------------
DOS 3.1+ - IOCTL - CHECK IF BLOCK DEVICE REMOTE
        AX = 4409h
        BL = drive number (00h = default, 01h = A:, etc)
Return: CF clear if successful
            DX = device attribute word
                bit 15: drive is SUBSTituted
                bit 13: (DR DOS 3.41/5.0 local drives only) always set
                        media ID needed
                bit 12: drive is remote
                bit  9: direct I/O not allowed
        CF set on error
*/
	Regs.REAX=0x4409;
	Regs.REBX=DrvNum;
	Exec_VxD_Int(0x21, &Regs);
	if(Regs.RFLAGS & 1) { //Carry flag check
		DbPrint(DC_SYS,DL_ERROR, ("__GetDriveType (4409) failed for drive %c:\n",DrvNum+'A'-1));
	} else {
		//if(Regs.REDX & (1<<15)) // drive is SUBSTituted
		if(Regs.REDX & (1<<12)) {// drive is remote
			DbPrint(DC_SYS,DL_INFO, ("__GetDriveType for drive %c: return DRIVE_REMOTE\n",DrvNum+'A'-1));
			return DRIVE_REMOTE;
		}
	}

/* ------------------------------------------------------------------------------------------------
DOS 3.0+ - IOCTL - CHECK IF BLOCK DEVICE REMOVABLE
        AX = 4408h
        BL = drive number (00h = default, 01h = A:, etc)
Return: CF clear if successful
            AX = media type (0000h removable, 0001h fixed)
        CF set on error
*/
	Regs.REAX=0x4408;
	Regs.REBX=DrvNum;
	Exec_VxD_Int(0x21, &Regs);
	if(Regs.RFLAGS & 1) { //Carry flag check
		DbPrint(DC_SYS,DL_ERROR, ("__GetDriveType (4408) failed for drive %c:\n",DrvNum+'A'-1));
	} else {
		if(Regs.REAX == 0) {// drive is removable
			DbPrint(DC_SYS,DL_INFO, ("__GetDriveType for drive %c: return DRIVE_REMOVABLE\n",DrvNum+'A'-1));
			return DRIVE_REMOVABLE;
		}
		if(Regs.REAX == 1) {// drive is fixed
			DbPrint(DC_SYS,DL_INFO, ("__GetDriveType for drive %c: return DRIVE_FIXED\n",DrvNum+'A'-1));
			return DRIVE_FIXED;
		}
	}
	DbPrint(DC_SYS,DL_INFO, ("__GetDriveType for drive %c: return DRIVE_UNKNOWN\n",DrvNum+'A'-1));
	return DRIVE_UNKNOWN;
}

#define MapP2L(Phys) MapPhysToLinear((PVOID)(Phys & MapP2L_phys),PAGE_SIZE,0)

UCHAR __declspec(naked) GetCr4(VOID)
{
	__asm {
		xor eax,eax;
		retn;
	}
}

#define IsLargePage(VirtAddr) FALSE
#define IsLargePageNative( VirtAddr)	FALSE

ULONG IsWaitingSafe(VOID)
{
	ULONG	dwCrCount;
	THREADHANDLE hThread;
	
	// Check CurVM->CB_VM_Status
	//			VMSTAT_EXCLUSIVE
	//			VMSTAT_BLOCKED
	//			VMSTAT_TS_SUSPENDED
	//			VMSTAT_TS_MAXPRI
	//			VMSTAT_V86INTSLOCKED
	//VM_Suspend
	
#ifdef __DBG__
	VMHANDLE CurVM,SysVM;
	BYTE InDosFlag;
	CurVM=Get_Cur_VM_Handle();
	SysVM=Get_Sys_VM_Handle();
	if(CurVM->CB_VM_Status & VMSTAT_EXCLUSIVE)
		DbPrint(DC_SYS,DL_NOTIFY,("CurVM %x owns VMSTAT_EXCLUSIVE status. It's only check for safety waiting.\n",CurVM));
	if(CurVM->CB_VM_Status & VMSTAT_BLOCKED)
		DbPrint(DC_SYS,DL_NOTIFY,("CurVM %x owns VMSTAT_BLOCKED status. It's only check for safety waiting.\n",CurVM));
	if(CurVM->CB_VM_Status & VMSTAT_TS_SUSPENDED)
		DbPrint(DC_SYS,DL_NOTIFY,("CurVM %x owns VMSTAT_TS_SUSPENDED status. It's only check for safety waiting.\n",CurVM));
	if(CurVM->CB_VM_Status & VMSTAT_TS_MAXPRI)
		DbPrint(DC_SYS,DL_NOTIFY,("CurVM %x owns VMSTAT_TS_MAXPRI status. It's only check for safety waiting.\n",CurVM));
	if(CurVM->CB_VM_Status & VMSTAT_V86INTSLOCKED)
		DbPrint(DC_SYS,DL_NOTIFY,("CurVM %x owns VMSTAT_V86INTSLOCKED status. It's only check for safety waiting.\n",CurVM));
	
	if(InDosFlag=*(PBYTE)(CurVM->CB_High_Linear + (DWORD)DOSMGR_Get_IndosPtr()))
		DbPrint(DC_SYS,DL_NOTIFY,("CurVM %x has InDosFlag=%d. It's only check for safety waiting.\n",CurVM,InDosFlag));
	
	if(SysVM!=CurVM) {
		if(SysVM->CB_VM_Status & VMSTAT_EXCLUSIVE)
			DbPrint(DC_SYS,DL_NOTIFY,("SysVM %x owns VMSTAT_EXCLUSIVE status. It's only check for safety waiting.\n",SysVM));
		if(SysVM->CB_VM_Status & VMSTAT_BLOCKED)
			DbPrint(DC_SYS,DL_NOTIFY,("SysVM %x owns VMSTAT_BLOCKED status. It's only check for safety waiting.\n",SysVM));
		if(SysVM->CB_VM_Status & VMSTAT_TS_SUSPENDED)
			DbPrint(DC_SYS,DL_NOTIFY,("SysVM %x owns VMSTAT_TS_SUSPENDED status. It's only check for safety waiting.\n",SysVM));
		if(SysVM->CB_VM_Status & VMSTAT_TS_MAXPRI)
			DbPrint(DC_SYS,DL_NOTIFY,("SysVM %x owns VMSTAT_TS_MAXPRI status. It's only check for safety waiting.\n",SysVM));
		if(SysVM->CB_VM_Status & VMSTAT_V86INTSLOCKED)
			DbPrint(DC_SYS,DL_NOTIFY,("SysVM %x owns VMSTAT_V86INTSLOCKED status. It's only check for safety waiting.\n",SysVM));
		
		if(InDosFlag=*(PBYTE)(SysVM->CB_High_Linear + (DWORD)DOSMGR_Get_IndosPtr()))
			DbPrint(DC_SYS,DL_NOTIFY,("SysVM %x has InDosFlag=%d. It's only check for safety waiting.\n",SysVM,InDosFlag));
	}
#endif //__DBG__
	//
	// Check CurThread->TCB_Flags
	//			THFLAG_RING0_THREAD
	//			THFLAG_ASYNC_THREAD
	// Test_Sys_Thread_Handle
	//	Get_VMM_Reenter_Count
	// PageSwap_Test_IO_Valid
	hThread=Get_Cur_Thread_Handle();
	Get_Crit_Status_Thread(&hThread,&dwCrCount);
	if(dwCrCount) {
		DbPrint(DC_SYS,DL_INFO,("Thread %x owns CS 0x%x times. Waiting unsafely.\n",hThread,dwCrCount));
		return 1;
	}
	if(((PTCB)(hThread))->TCB_Flags & THFLAG_OPEN_AS_IMMOVABLE_FILE ) {
		DbPrint(DC_SYS,DL_INFO,("Thread %x owns THFLAG_OPEN_AS_IMMOVABLE_FILE. Waiting unsafely.\n",hThread));
		return 1;
	}
	if(pWin16Mutex) {
		if(pWin16Mutex->OwnCnt!=0) {
			if(!(((PTCB)(hThread))->TCB_Flags & THFLAG_RING0_THREAD )) {
				if(Test_Sys_VM_Handle(Get_Cur_VM_Handle())) {
					if(hThread==*(DWORD*)(((PCHAR)(pWin16Mutex->OwnThrd))+W16mtxOThOffset)) {
						DbPrint(DC_SYS,DL_INFO,("Thread %x (%x) owns Win16Mutex (%x) 0x%x times. Waiting unsafely.\n",pWin16Mutex->OwnThrd,hThread,pWin16Mutex,pWin16Mutex->OwnCnt));
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
#endif // _HOOK_VXD_

//+ ------------------------------------------------------------------------------------------

void _UpperStringZeroTerninatedA(PCHAR pchin, PCHAR pchout)
{
#ifdef _HOOK_VXD_
CHAR*	tmp=pchout;
	if (pchin == NULL || pchout == NULL)
		return;
	while (*pchin != 0)
	{
		*pchout = _UpperCharA(*pchin);
		pchin++;
		pchout++;
	}
#else
//RtlUpperString
	if (pchin == NULL || pchout == NULL)
		return;
	while (*pchin != 0)
	{
		*pchout = _UpperCharA(*pchin);
		pchin++;
		pchout++;
	}
#endif
}
#ifdef _HOOK_VXD_
unsigned int wcslen(PWCHAR wcs)
{
        PWCHAR eos = wcs;

        while( *(eos++));
        
		return( (unsigned int)(eos - wcs - 1));
}
#endif

void _UpperStringZeroTerninatedW(PWCHAR pchin, PWCHAR pchout)
{
#ifdef _HOOK_VXD_
	while (*pchin != 0)
	{
		*pchout = _UpperCharW(*pchin);
		pchin++;
		pchout++;
	}

#else
	if (pchin == NULL || pchout == NULL)
		return;
	while (*pchin != 0)
	{
		*pchout = RtlUpcaseUnicodeChar(*pchin);
		pchin++;
		pchout++;
	}
#endif
}

// -----------------------------------------------------------------------------------------

BOOLEAN IsTimeExpired(__int64 ExpirationTime) 
{
	__int64	SysTime;
#ifdef _HOOK_VXD_
	{
	dos_time	DosTime;
	ULONG		AdditionalmSec;
		*(ULONG*)&DosTime=IFSMgr_Get_DOSTime(&AdditionalmSec);
		IFSMgr_DosToWin32Time(DosTime,(PFILETIME)&SysTime);
	}
#else
	KeQuerySystemTime((PLARGE_INTEGER)&SysTime);
#endif
	if(SysTime > ExpirationTime) {
		DbPrint(DC_FILTER,DL_SPAM, ("Expired cobject found\n"));
		return TRUE;
	}
	return FALSE;
}

__int64 GetTimeShift(ULONG TimeShift)
{
	__int64	SysTime;
#ifdef _HOOK_VXD_
	{
	dos_time	DosTime;
	ULONG		AdditionalmSec;
		*(ULONG*)&DosTime=IFSMgr_Get_DOSTime(&AdditionalmSec);
		IFSMgr_DosToWin32Time(DosTime,(PFILETIME)&SysTime);
	}
	SysTime += TimeShift * 10000L;		//!! in Sec/1000 intervals
#else
	KeQuerySystemTime((PLARGE_INTEGER)&SysTime);
	SysTime += TimeShift * 10000L;		// in Sec/1000 intervals
#endif
	return SysTime;
}
// -----------------------------------------------------------------------------------------


PKLG_EVENT InitKlgEvent(PCHAR WhistlerName)
{
	PKLG_EVENT pKlgEvent = NULL;

	pKlgEvent = ExAllocatePoolWithTag(NonPagedPool, sizeof(KLG_EVENT), 'EboS');
	if (pKlgEvent != NULL) {
		pKlgEvent->hWhistle = NULL;
		pKlgEvent->pWhistle = NULL;
		if(OpenWhistleupper(WhistlerName, &pKlgEvent->hWhistle,&pKlgEvent->pWhistle) == FALSE) {
			ExFreePool(pKlgEvent);
			pKlgEvent = NULL;
		}
	}
	return pKlgEvent;
}

void DoneKlgEvent(PKLG_EVENT* pKlgEvent)
{
	PKLG_EVENT pEvent = *pKlgEvent;
	if (pEvent != NULL)
	{
		if (pEvent->hWhistle != NULL)
			CloseWhistleupper(&pEvent->hWhistle);
		ExFreePool(pEvent);
		*pKlgEvent = NULL;
	}
}

PERESOURCE InitKlgResourse(void)
{
	PERESOURCE pMutex = (PERESOURCE) ExAllocatePoolWithTag(NonPagedPool, sizeof(ERESOURCE), 'MboS');
	if (pMutex != NULL)
		ExInitializeResourceLite(pMutex);

	return pMutex;
}

void FreeKlgResourse(PERESOURCE *pMutex) 
{
	if (pMutex != NULL)	{
		if (*pMutex != NULL)
		{
			ExDeleteResourceLite(*pMutex);
			ExFreePool(*pMutex);
			*pMutex = NULL;
		}
	}
}

// -----------------------------------------------------------------------------------------

BOOLEAN ExecutionOnHoldedThread(PVOID pInBuffer, DWORD dwInBufferSize, PVOID pOutBuffer, DWORD* pdwOutBufferSize)
{ 
	PEXEC_ON_HOLD pExHold = (PEXEC_ON_HOLD) pInBuffer;
	BOOLEAN bRet = FALSE;

	DbPrint(DC_SYS,DL_NOTIFY, ("HoldEx params: pInBuf %x, size: %d, pOutBuf: %x, size %d\n", pInBuffer, dwInBufferSize, pOutBuffer, *pdwOutBufferSize));
	switch (pExHold->m_FunctionID)
	{
	case __he_GetDriveType:
#ifdef _HOOK_VXD_
		if ((pOutBuffer != NULL) && (*pdwOutBufferSize >= sizeof(ULONG)) && (dwInBufferSize > sizeof(EXEC_ON_HOLD))) {
			* (ULONG*) pOutBuffer = __GetDriveType(*(BYTE*)(pExHold->m_ExecParams));
			*pdwOutBufferSize = sizeof(ULONG);
			bRet = TRUE;
		}
#endif
		break;
	}
	
	return bRet;
}

// -----------------------------------------------------------------------------------------
// Разные полезные функцейки

/*
PVOID GetImport(PVOID hModule,PCHAR FuncName)
{
PIMAGE_DOS_HEADER pDosHeader;
PIMAGE_NT_HEADERS pNTHeader;
PIMAGE_IMPORT_DESCRIPTOR importDesc;
PIMAGE_THUNK_DATA thunk, thunkIAT=0;
PIMAGE_IMPORT_BY_NAME pOrdinalName;
	pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		DbPrint(DC_SYS,DL_ERROR, ("MZ header not found\n"));
		return 0;
	}
	pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
		DbPrint(DC_SYS,DL_ERROR, ("PE header not found\n"));
		return 0;
	}
	importDesc=MakePtr(PIMAGE_IMPORT_DESCRIPTOR,0,pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if(importDesc==0){
		DbPrint(DC_SYS,DL_ERROR, ("No imports\n"));
		return 0;
	}
	importDesc=MakePtr(PIMAGE_IMPORT_DESCRIPTOR,hModule,importDesc);
	while(1) {
		// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
		if((importDesc->TimeDateStamp==0) && (importDesc->Name==0))
			break;
		thunk=MakePtr(PIMAGE_THUNK_DATA,0,importDesc->Characteristics);
		thunkIAT=MakePtr(PIMAGE_THUNK_DATA,0,importDesc->FirstThunk);
		if(thunk==0) {  // No Characteristics field?
			thunk = thunkIAT;// Yes! Gotta have a non-zero FirstThunk field then.
			if(thunk==0)   // No FirstThunk field?  Ooops!!!
				break;
		}
		thunk=MakePtr(PIMAGE_THUNK_DATA,hModule,thunk);
		thunkIAT=MakePtr(PIMAGE_THUNK_DATA,hModule,thunkIAT);
		while(1) {
			if(thunk->u1.AddressOfData==0)
				break;
			if(thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
				//Function has no name - only ordinal
			} else {
				pOrdinalName=MakePtr(PIMAGE_IMPORT_BY_NAME,hModule,thunk->u1.AddressOfData);
				if(strcmp(FuncName,pOrdinalName->Name)==0) {
					DbPrint(DC_SYS,DL_INFO, ("Import %s - %08x\n",FuncName,thunkIAT->u1.AddressOfData));
					return thunkIAT->u1.AddressOfData;
				}
			}
			thunk++;
			thunkIAT++;
		}
		importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
	}
	DbPrint(DC_SYS,DL_ERROR, ("ImportAddr for %s not found\n",FuncName));
	return 0;
}
*/

PVOID GetExport(PVOID hModule,PCHAR FuncName,DWORD *ExportAddr)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_EXPORT_DIRECTORY exportDir;
	DWORD i,j;
	DWORD *functions;
	WORD *ordinals;
	PCHAR *name;
	PVOID NativeEP=0;

//	__try {	
		pDosHeader=(PIMAGE_DOS_HEADER)hModule;
		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			DbPrint(DC_SYS,DL_ERROR, ("mz header not found\n"));
			return 0;
		}
		pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
			DbPrint(DC_SYS,DL_ERROR, ("PE header not found\n"));
			return 0;
		}
		exportDir=MakePtr(PIMAGE_EXPORT_DIRECTORY,hModule,
			pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		functions=MakePtr(DWORD*,hModule,exportDir->AddressOfFunctions);
		ordinals=MakePtr(WORD*,hModule,exportDir->AddressOfNameOrdinals);
		name=MakePtr(PCHAR*,hModule,exportDir->AddressOfNames);
		
		if((ULONG)FuncName<0x1000) {
			NativeEP=MakePtr(PVOID,hModule,functions[(ULONG)FuncName-exportDir->Base]);
			if(ExportAddr)
				*ExportAddr=(DWORD)(&(functions[(ULONG)FuncName-exportDir->Base]));
			return NativeEP;
		}
		
		for(i=0;i<exportDir->NumberOfFunctions;i++) {
			if(functions[i]==0)   // Skip over gaps in exported function
				continue;          // ordinals (the entrypoint is 0 for these
			//		if(NativeEP)
			//			break;
			for(j=0;j<exportDir->NumberOfNames;j++) {
				if(ordinals[j]==i) {  //!! А где ordinalbase!!!
					if(!strcmp(MakePtr(PCHAR,hModule,name[j]),FuncName)) {
						NativeEP=MakePtr(PVOID,hModule,functions[i]);
						DbPrint(DC_SYS,DL_INFO, ("Export Ord:%04d Ep:%08x %s - %08x\n",ordinals[j],NativeEP,FuncName,&(functions[i])));
						if(ExportAddr)
							*ExportAddr=(DWORD)(&(functions[i]));
						return NativeEP;
					}
				}
			}
		}
		DbPrint(DC_SYS,DL_ERROR, ("ExportAddr for %s not found\n",FuncName));
/*	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbPrint(DC_SYS, DL_ERROR, ("GetExport exception\n"));
		DbgBreakPoint();
	}*/
	
	return 0;
}

ULONG GetNativeID(IN PVOID NativeBase, IN PSTR NativeName)
{
	// This function should be called IRQL passive level only preferble from
	// DriverEntry. Note this function is absolutly Intel Based, can not be used
	// on Alpha processors without change!!!
	// This function picks up the Native ID since it can be change in different NT releases
	// therefor it has to be picked up from NTDLL's corresponding entry point first
	// during initialization. That can be done be checking the Export Table for the
	// function ZwQueryVirtualMemory. The first byte should be 0xB8 and after that
	// we can pick up the ULONG ID what we need to point into the correspoding table
	// The question is that during system startup the system process is running or not
	// if we load before that we can not see NTDLL.DLL. Should be checked
	// 77F67D7C                         public ZwQueryVirtualMemory
	// 77F67D7C                         ZwQueryVirtualMemory proc near          ;
	// 77F67D7C B8 81 00 00 00                                                  ;
	// 77F67D7C
	// 77F67D7C                         arg_0           = byte ptr  4
	// 77F67D7C
	// 77F67D7C                                         mov     eax, 81h        ;
	// 77F67D81 8D 54 24 04                             lea     edx, [esp+arg_0]
	// 77F67D85 CD 2E                                   int     2Eh
	// 77F67D87 C2 18 00                                retn    18h
	PVOID NativeEP;
	NativeEP=GetExport(NativeBase,NativeName,NULL);
	// Here we have a pointer to the EP of the API if NativeEP is not 0
	if(NativeEP) {
		if(((UCHAR*)((ULONG)NativeEP))[0]==0xB8) { // MOV EAX,XXXXXXXX?
			return ((ULONG*)((ULONG)NativeEP+1))[0];
		}
	}
	DbPrint(DC_SYS,DL_ERROR, ("GetNativeID fail\n"));
	return 0;
}

// -----------------------------------------------------------------------------------------
/*
DWORD GCsubstr (BYTE *st1, BYTE *end1, BYTE *st2, BYTE *end2)
{
	register BYTE *a1, *a2;
	BYTE *b1, *s1, *b2, *s2;
	DWORD max, i;
	max=0;
	b1=end1;
	b2=end2;
	for(a1=st1; a1<b1; a1++)
		for(a2=st2; a2<b2; a2++)
			if(*a1==*a2) {
				for(i = 1; a1[i]==a2[i]; i++) ;
				if(i>max) {
					max=i;
					s1=a1; s2=a2;
					b1=end1-max; b2=end2-max; }
			}
			if(max){
				max+=GCsubstr(s1+max, end1, s2+max, end2);
				max+=GCsubstr(st1, s1, st2, s2);
			}
			return max;
}
*/

DWORD GetSubstringPosBM(BYTE* Str, DWORD StrLen, BYTE* SubStr, DWORD SubStrLen)
{
	BYTE Table[0x100];
	int i;
	BYTE *PStr,*PEndStr,*PSubStr,*PEndSubStr;
	BYTE *PStrCur;
	if ((SubStrLen==0)||(StrLen<SubStrLen)) return -1;
	memset(Table,SubStrLen,sizeof(Table));
	for(i=SubStrLen-2;i>=0;i--)
		if (Table[SubStr[i]]==SubStrLen)
			Table[SubStr[i]]=(BYTE)(SubStrLen-i-1);
		PSubStr=PEndSubStr=SubStr+SubStrLen-1;
		PStrCur=PStr=Str+SubStrLen-1;
		PEndStr=Str+StrLen;
		do {
			//		if(CANCEL==-1) return -1;
			if((*PStr)==(*PSubStr)) {
				if(PSubStr==SubStr) return PStr-Str;
				PStr--;
				PSubStr--;
			} else {
				PSubStr=PEndSubStr;
				PStr=PStrCur+=Table[(*PStrCur)];
			}
		} while (PStr<PEndStr);
		return -1;
}

// -----------------------------------------------------------------------------------------

VOID __declspec(naked) __fastcall TouchPage(PVOID VAddr)
{
	__asm {
		pushfd;
		sti;
		mov ecx,[ecx];
		popfd;
		retn;
	}
}

#ifdef _HOOK_NT_

#ifdef __DBG__
LONG gMMapCounter = 0;
#endif

ULONG *MapP2L(ULONG Phys)//,ULONG PageSize)
{
	PHYSICAL_ADDRESS	MappedAddress;
	ULONG *BaseAddress;
	BaseAddress = NULL;
	MappedAddress.QuadPart = Phys & MapP2L_phys;//           MmNonCached погано работает под .net server'ом
	if((BaseAddress = MmMapIoSpace(MappedAddress, PAGE_SIZE, MmCached))!=NULL) { ////MmCached
#ifdef __DBG__
		if((ULONG)BaseAddress<0x80000000) {
			DbPrint(DC_SYS,DL_ERROR, ("MapP2L: bad mapping (%x) for %x\n",BaseAddress,Phys));
			DbgBreakPoint();
		}

		InterlockedIncrement(&gMMapCounter);
#endif
		return BaseAddress;
	}
	return BADP2LMAPPING;
}

ULONG *MapP2LCr3Pae(ULONG cr3)//,ULONG PageSize)
{
	PHYSICAL_ADDRESS	MappedAddress;
	ULONG *BaseAddress;
	BaseAddress = NULL;
	MappedAddress.QuadPart = cr3 & Cr3PAE4k_phys;//           MmNonCached погано работает под .net server'ом
	if((BaseAddress = MmMapIoSpace(MappedAddress, PAGE_SIZE, MmCached))!=NULL) { // //MmCached
#ifdef __DBG__
		if((ULONG)BaseAddress<0x80000000) {
			DbPrint(DC_SYS,DL_ERROR, ("MapP2Lcr3: bad mapping (%x) for %x\n",BaseAddress, cr3));
			DbgBreakPoint();
		}

		InterlockedIncrement(&gMMapCounter);
#endif
		return BaseAddress;
	}
	return BADP2LMAPPING;
}

void DonePageDirCr3(ULONG* PageDir)
{
#ifdef __DBG__
	InterlockedDecrement(&gMMapCounter);
#endif

	MmUnmapIoSpace(PageDir, PAGE_SIZE);
}

void DonePageDir(ULONG* PageDir)
{
#ifdef __DBG__
	InterlockedDecrement(&gMMapCounter);
#endif

	MmUnmapIoSpace((PVOID)((DWORD)PageDir & 0xfffff000), PAGE_SIZE);
}
#else
void DonePageDir(ULONG* PageDir)
{
}

#endif // _HOOK_NT_


ULONGLONG* GetPageDescriptorGeneric(PVOID VirtAddr)
{
	ULONG *tmp;
	ULONG *PageDir;
	ULONG myCr3;
	ULONG myCr4;
	
	__asm {
		mov eax,cr3;
		mov myCr3,eax;
	}
	myCr4 = ((LONG(__stdcall *)(VOID))(GetCr4))();
	
	TouchPage(VirtAddr);

	if((PageDir = MapP2L(myCr3)) != BADP2LMAPPING)
	{
		tmp = PageDir + ((ULONG)VirtAddr >> 22);
		if(!(*tmp & PDE_P))
		{
			DonePageDir((ULONG*)PageDir);
			return (ULONGLONG*)BADP2LMAPPING;
		}

		if((*tmp & PDE_PS) && (myCr4 & CR4_PSE))
		{ // 4m PSE page
			return (ULONGLONG*)tmp;
		}
		tmp = MapP2L(*tmp & 0xfffff000);
		DonePageDir((ULONG*)PageDir);

		if(tmp != BADP2LMAPPING)
			return (ULONGLONG*)(tmp + (((ULONG)VirtAddr & 0x3FFFFF)>>12));
	}
	
	return (ULONGLONG*)BADP2LMAPPING;
}

#define PTE_BASE 0xc0000000
#define PDE_BASE_X86    0xc0300000
#define PDE_BASE_X86PAE 0xc0600000
#define MiGetPdeAddressPAE(va)   ((ULONGLONG*)(PDE_BASE_X86PAE + ((((ULONG)(va)) >> 21) << 3)))
#define MiGetPteAddressPAE(va)   ((ULONGLONG*)(PTE_BASE + ((((ULONG)(va)) >> 12) << 3)))

#define MiGetPdeAddress(va)  ((ULONG*)(((((ULONG)(va)) >> 22) << 2) + PDE_BASE_X86))
#define MiGetPteAddress(va) ((ULONG*)(((((ULONG)(va)) >> 12) << 2) + PTE_BASE))

ULONGLONG* GetPDEEntryNativeLiteGeneric(PVOID VirtAddr)
{
	return (ULONGLONG*)MiGetPdeAddress(VirtAddr);
}

ULONGLONG* GetPageDescriptorNativeLiteGeneric(PVOID VirtAddr)
{
	return (ULONGLONG*) MiGetPteAddress(VirtAddr);
}

ULONGLONG* GetPageDescriptorNativeGeneric(PVOID VirtAddr)
{
	ULONG*  pde = MiGetPdeAddress(VirtAddr);
	ULONG* pte;
	
	TouchPage(VirtAddr);
	
	if (!(*pde & PDE_P))
		return (ULONGLONG*)BADP2LMAPPING;
	
	if (*pde & PDE_PS)
		return (ULONGLONG*)pde;
	
	pte = MiGetPteAddress(VirtAddr);
	if (!(*pte & PDE_P))
		return (ULONGLONG*)BADP2LMAPPING;
	
	return (ULONGLONG*)pte;
}

#ifdef _HOOK_NT_

PHYSICAL_ADDRESS *MapP2LEx(PHYSICAL_ADDRESS MappedAddress)
{
	PHYSICAL_ADDRESS *BaseAddress;

	BaseAddress = NULL;
	MappedAddress.LowPart = MappedAddress.LowPart & MapP2L_phys;

	if (MappedAddress.HighPart != 0)
		MappedAddress.HighPart = MappedAddress.HighPart & 0x1f;
	if((BaseAddress = MmMapIoSpace(MappedAddress, PAGE_SIZE, MmCached))!=NULL)	//MmCached
	{
#ifdef __DBG__
		if((ULONG)BaseAddress<0x80000000) {
			DbPrint(DC_SYS,DL_ERROR, ("MapP2LEx: bad mapping (%x)\n",BaseAddress));
			DbgBreakPoint();
		}

		InterlockedIncrement(&gMMapCounter);
#endif
		return BaseAddress;
	}
	return (PHYSICAL_ADDRESS*) BADP2LMAPPING;
}

ULONGLONG* GetPageDescriptorPAE(PVOID VirtAddr)
{
	PHYSICAL_ADDRESS *tmp;
	PHYSICAL_ADDRESS *readptr;
	ULONG myCr3;

//	PMDL pmdl1;
//	PVOID ptr1;
	
	ULONG PageDirectoryPointerOffset = (ULONG)VirtAddr >> 30;
	ULONG PageDirectoryOffset = ((ULONG)VirtAddr >> 21) & 0x1ff;
	ULONG PageTableOffset = ((ULONG)VirtAddr >> 12) & 0x1ff;

	if (VirtAddr == 0)
		return (ULONGLONG*)BADP2LMAPPING;
	
	__asm
	{
		mov eax,cr3;
		mov myCr3,eax;
	}

	TouchPage(VirtAddr);

/*
	ptr1 = (PVOID) (myCr3 & Cr3PAE4k_phys);
	pmdl1 = IoAllocateMdl(ptr1, PAGE_SIZE, FALSE, FALSE, NULL);
	if (pmdl1)
	{
		__try
		{
			MmProbeAndLockPages(pmdl1, KernelMode, IoReadAccess);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			IoFreeMdl(pmdl1);
			pmdl1 = NULL;
		}
	}*/
	
	readptr = (PHYSICAL_ADDRESS*) MapP2LCr3Pae(myCr3);
		
	/*if (pmdl1)
	{
		MmUnlockPages(pmdl1);
		IoFreeMdl(pmdl1);
	}*/

	if((ULONG*)readptr == BADP2LMAPPING)
	{
		DbPrint(DC_SYS,DL_ERROR, ("cr3 not valid %x\n", myCr3));
		DbgBreakPoint();
		return (ULONGLONG*)BADP2LMAPPING;
	}

	DbPrint(DC_SYS, DL_NOTIFY, ("cr3 %p -> %p\n", myCr3, (ULONG*)readptr));

	tmp = readptr + PageDirectoryPointerOffset;	//pdpe
	if (!(tmp->LowPart & PDE_P))
	{
		DbPrint(DC_SYS,DL_ERROR, ("pdpe invalid (Va=%x) pde not present\n",VirtAddr));
		DbgBreakPoint();
		DonePageDirCr3((ULONG*)readptr);
	}
	else
	{
		PHYSICAL_ADDRESS* oldreadptr = readptr;
		readptr = MapP2LEx(*tmp);
		
		DonePageDirCr3((ULONG*)oldreadptr);
		
		if (readptr == (PHYSICAL_ADDRESS*) BADP2LMAPPING)
		{
			DbPrint(DC_SYS,DL_ERROR, ("GetPageDescriptor (Va=%x) pde not present\n",VirtAddr));
			DbgBreakPoint();
		}
		else
		{
			tmp = readptr  + PageDirectoryOffset;	// pde
			
			if(!(tmp->LowPart & PDE_P))
			{
				DbPrint(DC_SYS,DL_ERROR, ("GetPageDescriptor (Va=%x) PageDescriptor not present\n",VirtAddr));
				
				DbgBreakPoint();
				DonePageDir((ULONG*)readptr);
				
				return (ULONGLONG*)BADP2LMAPPING;
			}
			
			// check 2mb pse in *tmp and return it if pse
			if (tmp->LowPart & PDE_PS)
			{
				DbPrint(DC_SYS,DL_IMPORTANT, ("GetPageDescriptor 2M PSE page in PAE mode (Va=%x)\n", VirtAddr));
				DbgBreakPoint();
				return (ULONGLONG*)tmp;
			}
			
			tmp = MapP2LEx(*tmp);
			DonePageDir((ULONG*)readptr);
			
			if(tmp != (PHYSICAL_ADDRESS*)BADP2LMAPPING)
			{
				tmp = tmp + PageTableOffset;

				return (ULONGLONG*)tmp;
			}
		}
	}
	
	DbgBreakPoint();
	return (ULONGLONG*)BADP2LMAPPING;
}

ULONGLONG* GetPageDescriptorNativePAE(PVOID VirtAddr)
{
	ULONGLONG* pde = MiGetPdeAddressPAE(VirtAddr);
	ULONGLONG* pte;
	
	TouchPage(VirtAddr);

	if (!(*(ULONG*)pde & PDE_P))
		return (ULONGLONG*)BADP2LMAPPING;
	
	if (*(ULONG*)pde & PDE_PS)
		return pde;
	
	pte = MiGetPteAddressPAE(VirtAddr);
	if (!(*(ULONG*)pte & PDE_P))
		return (ULONGLONG*)BADP2LMAPPING;
	
	return pte;
}

ULONGLONG* GetPDEEntryNativeLitePAE(PVOID VirtAddr)
{
	return MiGetPdeAddressPAE(VirtAddr);
}

ULONGLONG* GetPageDescriptorNativeLitePAE(PVOID VirtAddr)
{
	return MiGetPteAddressPAE(VirtAddr);
}

#endif // _HOOK_NT_

__declspec(naked) ULONG __fastcall HalpAcquireHighLevelLock(ULONG *SpinLock)
{
	__asm {
		pushfd
		pop     eax
acqlock:
		cli
		lock bts   dword ptr [ecx], 0
		jb      short startwait
		retn
startwait:
		push    eax
		popfd
waitloop:
		test    dword PTR [ecx], 1
		jz      short acqlock
		rep nop
		jmp     short waitloop
	}
}

__declspec(naked) VOID __fastcall HalpReleaseHighLevelLock (ULONG *SpinLock,ULONG NewIrql)
{
	__asm {
		mov     dword ptr [ecx], 0
		push    edx
		popfd
		retn
	}
}

// -----------------------------------------------------------------------------------------

VOID SetTspPAEMode(BOOLEAN bPAE);

BOOLEAN RecognizeFuncs(ULONG VirtAddr)
{
	BOOLEAN bRet = FALSE;
#ifdef _HOOK_NT_

	ULONG myCr4;
	myCr4 = ((LONG(__stdcall *)(VOID))(GetCr4))();

	if(myCr4 & CR4_PAE)
	{
		DbPrint(DC_SYS, DL_WARNING, ("GetPageDescriptor 4K PAE page (Va=%x)\n",VirtAddr));
		GetPageDescriptor = GetPageDescriptorPAE;
		GetPageDescriptorNative = GetPageDescriptorNativePAE;
		GetPDEEntryNativeLite = GetPDEEntryNativeLitePAE;
		GetPageDescriptorNativeLite = GetPageDescriptorNativeLitePAE;

		SetTspPAEMode(TRUE);
		bRet = TRUE;
	}
	else
#endif // _HOOK_NT_
	{
		GetPageDescriptor = GetPageDescriptorGeneric;
		GetPageDescriptorNative = GetPageDescriptorNativeGeneric;
		GetPDEEntryNativeLite = GetPDEEntryNativeLiteGeneric;
		GetPageDescriptorNativeLite = GetPageDescriptorNativeLiteGeneric;
		bRet = TRUE;
		SetTspPAEMode(FALSE);

		DbPrint(DC_SYS, DL_IMPORTANT, ("GetPageDescriptor 4k generic (Va=%x)\n",VirtAddr));
	}

	return bRet;
}

// stat
unsigned __int64 g_T = 0;
unsigned __int64 g_TW = 0;
LONG g_C = 0;
