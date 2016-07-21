
typedef struct _TSPPAGE {
	struct _TSPPAGE *Next;
	PVOID	OrigVA;		// MUST be aligned on PAGE bound (divisible by PAGE_SIZE)
	ULONG	*OrigDesc;	
	PVOID	ShadowVA;
	ULONG	*ShadowDesc;	
#ifdef _HOOK_NT_
	PMDL ShMdl;	
#endif	//_HOOK_NT_
	BOOLEAN	Swapped;
} TSPPAGE,*PTSPPAGE;

typedef struct _THSUBPRO {
	struct _THSUBPRO *Next;
	PVOID			Owner;
	PVOID			OwnerProc;
	PTSPPAGE	RegionList;
	DWORD			Flags;			//see TSP_??? bitfields
} THSUBPRO,*PTHSUBPRO;

PTHSUBPRO TSPRoot=NULL;
BOOLEAN TSPInitedOk=FALSE;

ULONG TSPSpinLock;

#ifdef __DBG__
ULONG InTSP=0;
VOID AcquireTSP(ULONG  *OldIrql)
{
ULONG ITSP;
	*OldIrql=HalpAcquireHighLevelLock(&TSPSpinLock);
	ITSP=InTSP++;//InterlockedExchangeAdd(&InTSP,1);
	if(ITSP){
		DbPrint(DC_TSP,DL_ERROR,("!!! InTSP=%d\n",ITSP));
		DbgBreakPoint();
	}
}

VOID ReleaseTSP(ULONG NewIrql)
{
ULONG Flags;
	__asm {
		pushfd
		pop eax
		mov Flags,eax
	}
	InterlockedDecrement(&InTSP);
	HalpReleaseHighLevelLock(&TSPSpinLock,NewIrql);
	if(Flags & 0x200) {
		DbPrint(DC_TSP,DL_ERROR,("!!! STI\n"));
		DbgBreakPoint();
	}
}
#else 
#define AcquireTSP(OldIrql)	*OldIrql=HalpAcquireHighLevelLock(&TSPSpinLock)
#define ReleaseTSP(NewIrql)	HalpReleaseHighLevelLock(&TSPSpinLock,NewIrql)
#endif

VOID TSPSwapRegions(PVOID OwnTh,PTHSUBPRO Root,BOOLEAN SwapDirection)
{
PTHSUBPRO	ThEntry;
PTSPPAGE	RgEntry;
	ThEntry=Root;
	while(ThEntry && ThEntry->Owner!=OwnTh) {
		ThEntry=ThEntry->Next;
	}
	if(ThEntry) {
		RgEntry=ThEntry->RegionList;
		while(RgEntry) {
			if(SwapDirection==RgEntry->Swapped) {
				DbPrint(DC_TSP,DL_WARNING, ("!!! TSPSwapRegions %x %s already done.\n",OwnTh,SwapDirection?"swap":"unswap"));
				RgEntry=RgEntry->Next;
				continue;
			}

//			TouchPage(RgEntry->OrigDesc);
//			TouchPage(RgEntry->OrigVA);
//			TouchPage(RgEntry->ShadowDesc);
//			TouchPage(RgEntry->ShadowVA);
			
#ifdef __DBG__
			if(!(*RgEntry->OrigDesc & 1)) {
				DbPrint(DC_TSP,DL_SPAM, ("!!! TSPSwapRegions orig page %x va %x is not present\n",*RgEntry->OrigDesc,RgEntry->OrigVA));
			}
			if(!(*RgEntry->ShadowDesc & 1)) {
				DbPrint(DC_TSP,DL_SPAM, ("!!! TSPSwapRegions shad page %x va %x is not present\n",*RgEntry->ShadowDesc,RgEntry->ShadowVA));
			}
#endif

			RgEntry->Swapped=!RgEntry->Swapped;
			__asm {
				mov edx,RgEntry;
				
				mov ebx,[edx+TSPPAGE.OrigDesc];
				mov ecx,[edx+TSPPAGE.ShadowDesc];
				mov eax,[ebx];
				lock xchg eax,[ecx];
				lock xchg eax,[ebx];
				
				mov eax,[edx+TSPPAGE.OrigVA];
				invlpg [eax];
				mov eax,[edx+TSPPAGE.ShadowVA];
				invlpg [eax];
			}
			RgEntry=RgEntry->Next;
		}
	}
}

PVOID TSPAllocPage(BOOLEAN KernelMemSpace)
{
PVOID Addr=NULL;
#ifdef _HOOK_NT_
ULONG RegionSize;
	RegionSize=PAGE_SIZE;
	if(KernelMemSpace) {
//		Addr=ExAllocatePoolWithTag(NonPagedPool,RegionSize,'tSeB');
		Addr=ExAllocatePoolWithTag(PagedPool,RegionSize,'tSeB');
	} else {
		ULONG ntStatus;
		if(ZwAllocateVirtualMemory((HANDLE)-1,&Addr,0,&RegionSize,MEM_COMMIT,PAGE_READWRITE)!=STATUS_SUCCESS){
			DbPrint(DC_TSP,DL_ERROR, ("TSPAllocPage fail on ZwAllocateVirtualMemory\n"));
		}
	}
#else
//!! Check for FIXED
	Addr=PageReserve(KernelMemSpace ? PR_SYSTEM : PR_PRIVATE,1,PR_FIXED);
	if(Addr) {
		if(PageCommit((ULONG)Addr>>0xc,1,/*PD_NOINIT*/PD_FIXED,0,PC_FIXED | PC_USER | PC_WRITEABLE)) {
			return Addr;
		} else {
			DbPrint(DC_TSP,DL_ERROR, ("TSPAllocPage fail on commit\n"));
			PageFree((MEMHANDLE)Addr,0);
			Addr=NULL;
		}
	} else {
		DbPrint(DC_TSP,DL_ERROR, ("TSPAllocPage fail on reserve\n"));
	}
#endif //_HOOK_NT_
	return Addr;
}

VOID TSPFreePage(PVOID Addr,BOOLEAN KernelMemSpace)
{
#ifdef _HOOK_NT_
	if(KernelMemSpace){
		ExFreePool(Addr);
	} else {
		ULONG RegionSize;
		RegionSize=0;
		_pfZwFreeVirtualMemory((HANDLE)-1,&Addr,&RegionSize,MEM_RELEASE);
	}
#else
	PageFree((MEMHANDLE)Addr,0);
#endif //_HOOK_NT_
}

//NTSTATUS (__stdcall *NtLockVirtualMemory)(HANDLE hProcess,PVOID BaseAddr,int LockSize,int LockType);

BOOLEAN TSPAllocShadow(PTSPPAGE RgEntry, ULONG Flags)
{
PVOID ShadVa;
	if(RgEntry->ShadowVA) {
		ShadVa=RgEntry->ShadowVA;
		DbPrint(DC_TSP,DL_WARNING, ("TSPMakeShadow: ShadVa=%x!=NULL. \n"));
	} else {
		ShadVa=TSPAllocPage((BOOLEAN)(Flags & TSP_KRNL_MEMSPACE_SHADOW));
		if(ShadVa==NULL) {
			DbPrint(DC_TSP,DL_ERROR, ("TSPMakeShadow: no mem. \n"));
			goto err;
		}
	}
	if((ULONG)ShadVa % PAGE_SIZE !=0) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPMakeShadow: ShadVa %x is not aligned. \n",ShadVa));
		goto err;
	}


#ifdef _HOOK_NT_
	{
	PMDL Mdl;
	KPROCESSOR_MODE ProcMod;
		if(Flags & TSP_KRNL_MEMSPACE_SHADOW)
			ProcMod=KernelMode;
		else
			ProcMod=UserMode;
		Mdl=IoAllocateMdl(ShadVa,PAGE_SIZE,FALSE,FALSE,NULL);
		if(Mdl) {
			__try {
				MmProbeAndLockPages(Mdl,ProcMod, IoModifyAccess);
			} __except(EXCEPTION_EXECUTE_HANDLER) {
				IoFreeMdl(Mdl);
				DbPrint(DC_TSP,DL_ERROR, ("TSPAllocPage fail on MmProbeAndLockPages\n"));
				goto err;
			}
		} else{
			DbPrint(DC_TSP,DL_ERROR, ("TSPAllocPage fail on IoAllocateMdl\n"));
			goto err;
		}
		RgEntry->ShMdl=Mdl;
	}
#endif //_HOOK_NT_
	
	if(Flags & TSP_NO_COPY_ORIG) {
		memset(ShadVa,0,PAGE_SIZE);
		memchr(RgEntry->OrigVA,1,1);
	} else {
		memcpy(ShadVa,RgEntry->OrigVA,PAGE_SIZE);
	}
	if((RgEntry->OrigDesc=GetPageDiscriptor(RgEntry->OrigVA))!=BADP2LMAPPING) {
		if((RgEntry->ShadowDesc=GetPageDiscriptor(ShadVa))!=BADP2LMAPPING) {
//			if(Flags & TSP_KRNL_MEMSPACE_SHADOW) {
				*RgEntry->ShadowDesc=*RgEntry->ShadowDesc | 4;//+16);//Set usermode bit
				*RgEntry->ShadowDesc=*RgEntry->ShadowDesc & 0xFFFFFEFF;// 100h-Global
//			}
			RgEntry->ShadowVA=ShadVa;
			return TRUE;
		}
		DonePageDir(RgEntry->OrigDesc);
	}
	DbPrint(DC_TSP,DL_ERROR, ("TSPMakeShadow failed. GetPageDiscriptor return BADP2LMAPPING\n"));
err:
	if(ShadVa)
		TSPFreePage(ShadVa,(BOOLEAN)(Flags & TSP_KRNL_MEMSPACE_SHADOW));
	RgEntry->ShadowVA=NULL;
	return FALSE;
}

VOID TSPFreeShadow(PTSPPAGE RgEntry,BOOLEAN KernelMemSpace)
{
	if(RgEntry->ShadowVA) {
		DonePageDir(RgEntry->OrigDesc);
		DonePageDir(RgEntry->ShadowDesc);
#ifdef _HOOK_NT_
		MmUnlockPages(RgEntry->ShMdl);
		IoFreeMdl(RgEntry->ShMdl);
#endif //_HOOK_NT_
		TSPFreePage(RgEntry->ShadowVA,KernelMemSpace);
		RgEntry->ShadowVA=NULL;
	}
}

//!! А при вызове на прибиении триды я не обвалюсь на TSPFreeShadow??? Какой будет контекст памяти?
// В NT прибиение триды происходит на ней же
// В 9x они не равны, вызов опасен!!!
VOID DelTSPRegions(PTHSUBPRO ThEntry)
{
PTSPPAGE	RgEntry;
PTSPPAGE	Tmp;
	DbPrint(DC_TSP,DL_NOTIFY, ("DelTSPRegios OwnTh=%x OwnProc=%x Flags=%x\n",ThEntry->Owner,ThEntry->OwnerProc,ThEntry->Flags));
	RgEntry=ThEntry->RegionList;
	while(RgEntry) {
		if(RgEntry->Swapped) {
			DbPrint(DC_TSP,DL_ERROR, ("!!! DelTSPRegions: %x is swapped.\n",ThEntry->Owner));
			DbgBreakPoint();
		}
		Tmp=RgEntry->Next;
		DbPrint(DC_TSP,DL_SPAM, ("DelTSPRegion Owner=%x OrigVA=%x ShadVA=%x OrigPD=%x ShadPD=%x\n",
				ThEntry->Owner,RgEntry->OrigVA,RgEntry->ShadowVA,RgEntry->OrigDesc,RgEntry->ShadowDesc));
		TSPFreeShadow(RgEntry,(BOOLEAN)(ThEntry->Flags & TSP_KRNL_MEMSPACE_SHADOW));
		ExFreePool(RgEntry);
		RgEntry=Tmp;
	}
	ExFreePool(ThEntry);
}

VOID CleanupTSP(VOID)
{
ULONG	oldirql;
PTHSUBPRO	ThEntry;
PTHSUBPRO	ThTmp;
	DbPrint(DC_TSP,DL_NOTIFY, ("CleanUpTSP\n"));
	AcquireTSP(&oldirql);
	ThEntry=TSPRoot;
	TSPRoot=NULL;
	ReleaseTSP(oldirql);
	while(ThEntry) {
		ThTmp=ThEntry->Next;
		DelTSPRegions(ThEntry);
		ThEntry=ThTmp;
	}
}


VOID DelTSPOwner(PVOID Owner)
{
ULONG	oldirql;
PTHSUBPRO	ThEntry;
PTHSUBPRO	ThPrev=NULL;
	AcquireTSP(&oldirql);
	ThEntry=TSPRoot;
	while(ThEntry && ThEntry->Owner!=Owner) {
		ThPrev=ThEntry;
		ThEntry=ThEntry->Next;
	}
	if(!ThEntry) {
		ReleaseTSP(oldirql);
//		DbPrint(DC_TSP,DL_SPAM, ("DelTSPOwner %x not found.\n",Owner));
		return;
	}
	if(ThPrev) {
		ThPrev->Next=ThEntry->Next;
	} else {
		TSPRoot=ThEntry->Next;
	}
	TSPSwapRegions(Owner,ThEntry,FALSE);
	ReleaseTSP(oldirql);
	DbPrint(DC_TSP,DL_NOTIFY, ("DelTSPOwner %x \n",Owner));
	DelTSPRegions(ThEntry);
}

BOOLEAN AddTSPRegion(PTHREAD_PAGE_SET pPageSet)
{
PTHSUBPRO	ThEntry;
PTHSUBPRO	ThETmp;
PTSPPAGE	RgEntry;
PVOID			OwnTh;
PVOID			OwnPr;
DWORD			i,j;
ULONG			oldirql;
	if(!TSPInitedOk) {
		DbPrint(DC_TSP,DL_WARNING, ("TSP is not Inited\n"));
		return FALSE;
	}
#ifdef _HOOK_NT_ 
	OwnTh=PsGetCurrentThreadId();
	OwnPr=PsGetCurrentProcess();
#else
	OwnTh=(PVOID)Get_Cur_Thread_Handle();
	OwnPr=NULL;
#endif //_HOOK_NT_
	//Check params
#ifdef __DBG__
	AcquireTSP(&oldirql);
	ThETmp=TSPRoot;
	while(ThETmp && ThETmp->Owner!=OwnTh) {
		ThETmp=ThETmp->Next;
	}
	ReleaseTSP(oldirql);
	if(ThETmp) {
			DbPrint(DC_TSP,DL_WARNING, ("!!! AddTSPRegion thread %x already exist in swaplist. Delete old one\n",OwnTh));
			DbgBreakPoint();
			DelTSPOwner(OwnTh);
	}
#endif //__DBG__
	
	for(i=0;i<pPageSet->m_PageCount;i++) {
		DbPrint(DC_TSP,DL_NOTIFY, ("AddTSPRegions OwnTh=%x OwnProc=%x OVA[%d]=%x Size=%d\n",
			OwnTh,OwnPr,i,pPageSet->m_PageParams[i].m_OrigAddr,pPageSet->m_PageParams[i].m_RegionSize));
		if(pPageSet->m_PageParams[i].m_RegionSize==0) {
			DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion pPageSet->m_PageParams[%d].m_RegionSize==0\n",i));
			DbgBreakPoint();
			return FALSE;
		}
		if(pPageSet->m_PageParams[i].m_OrigAddr==NULL) {
			DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion pPageSet->m_PageParams[%d].m_OrigAddr==NULL\n",i));
			DbgBreakPoint();
			return FALSE;
		}
		if((ULONG)(pPageSet->m_PageParams[i].m_OrigAddr) % PAGE_SIZE !=0) {
			DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion pPageSet->m_PageParams[%d].m_OrigAddr=%x is not aligned on PAGE_SIZE\n",i,pPageSet->m_PageParams[i].m_OrigAddr));
			DbgBreakPoint();
			return FALSE;
		}

/*	//Check regions overlapping
		RgEntry=ThEntry->RegionList;
		while(RgEntry) {
			if((
				pPageSet->m_PageParams[i].m_StartAddress >= RgEntry->OrigVA && 
				(PCHAR)pPageSet->m_PageParams[i].m_StartAddress < (PCHAR)RgEntry->OrigVA+PAGE_SIZE) || (
				RgEntry->OrigVA >= pPageSet->m_PageParams[i].m_StartAddress &&
				(PCHAR)RgEntry->OrigVA	< (PCHAR)(pPageSet->m_PageParams[i].m_StartAddress)+pPageSet->m_PageParams[i].m_RegionSize*PAGE_SIZE
				)) {
				DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion failed, pPageSet->m_PageParams[%d].m_StartAddress=%x overlapped \n",i,pPageSet->m_PageParams[i].m_StartAddress));
				DbgBreakPoint();
				DelTSPt(OwnTh);
				return FALSE;
			}
			RgEntry=RgEntry->Next;
		}
*/
	}
	ThEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(THSUBPRO),'TSeB');
	if(ThEntry) {
//!!!!!!!!!!!!!!!!!!!!!!!!
		ThEntry->Flags=pPageSet->m_PageParams[0].m_Flags;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!^!!!!!!!!!
		ThEntry->Owner=OwnTh;
		ThEntry->OwnerProc=OwnPr;
		ThEntry->RegionList=NULL;
		ThEntry->Next=NULL;
		
	} else {
		DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion failed, no memory...\n"));
		return FALSE;
	}
	for(i=0;i<pPageSet->m_PageCount;i++) {
		for(j=0;j<pPageSet->m_PageParams[i].m_RegionSize;j++) {
			RgEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSPPAGE),'TSeB');
			if(RgEntry) {
				RgEntry->OrigVA=(PVOID)((PCHAR)(pPageSet->m_PageParams[i].m_OrigAddr)+j*PAGE_SIZE);
				RgEntry->Swapped=FALSE;
				
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				if(ThEntry->Flags & TSP_KRNL_MEMSPACE_SHADOW) {
					RgEntry->ShadowVA=NULL;
//				} else {
//					RgEntry->ShadowVA=(PVOID)((PCHAR)(pPageSet->m_PageParams[i].m_ShadAddr)+j*PAGE_SIZE);
//				}
				if(!TSPAllocShadow(RgEntry,ThEntry->Flags)) {
					DelTSPRegions(ThEntry);
					DonePageDir(PageDir);
					return FALSE;
				}
				RgEntry->Next=ThEntry->RegionList;
				ThEntry->RegionList=RgEntry;
				DbPrint(DC_TSP,DL_SPAM, ("AddTSPRegion Owner=%x OrigVA=%x ShadVA=%x OrigPD=%x ShadPD=%x\n",
					OwnTh,RgEntry->OrigVA,RgEntry->ShadowVA,RgEntry->OrigDesc,RgEntry->ShadowDesc));
			} else {
				DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion failed, no memory...\n"));
				DelTSPRegions(ThEntry);
				DonePageDir(PageDir);
				return FALSE;
			}
		}
	}
	DonePageDir(PageDir);
	AcquireTSP(&oldirql);
	TSPSwapRegions(OwnTh,ThEntry,TRUE);
	//Check list
	ThETmp=TSPRoot;
	while(ThETmp && ThETmp->Owner!=OwnTh) {
		ThETmp=ThETmp->Next;
	}
	if(ThETmp) { //Already in list
		RgEntry=ThEntry->RegionList;//Вообщето они уже и так равны (см.выше)
		while(RgEntry->Next) {//находим последний
			RgEntry=RgEntry->Next;
		}
		RgEntry->Next=ThETmp->RegionList;//подцепляем к последнему существующий список
		ThETmp->RegionList=ThEntry->RegionList;
	} else { //Add new to list
		ThEntry->Next=TSPRoot;
		TSPRoot=ThEntry;
		ThEntry=NULL;// Важно!
	}
	ReleaseTSP(oldirql);
	if(ThEntry) //Если трида уже была в списке - добиваем временную память
		ExFreePool(ThEntry);
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------

#ifdef _HOOK_NT_
DWORD ThreadSwitchCallbackPatchRet;
DWORD ThreadIdOffsetInETHREAD=0x1e4;

BYTE PatchedCode[]={0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0xC3};
PVOID PatchedCodeF=PatchedCode;

VOID __declspec(naked) ThreadSwitchCallback(VOID)//
{
ULONG	oldirql;
HANDLE OldThreadId;
HANDLE NewThreadId;
#define VariablesSize 0x10
	__asm {
		pushf;
		pushad;
		mov     ebp, esp;
		sub esp,VariablesSize;
		add esi,ThreadIdOffsetInETHREAD;
		add edi,ThreadIdOffsetInETHREAD;
		mov eax,[esi];
		mov NewThreadId,eax;
		mov eax,[edi];
		mov OldThreadId,eax;
		
	}
#ifdef __DBG__
		{
			PTHSUBPRO	ThEntry;
			AcquireTSP(&oldirql);
			ThEntry=TSPRoot;
			while(ThEntry && ThEntry->Owner!=OldThreadId && ThEntry->Owner!=NewThreadId) {
				ThEntry=ThEntry->Next;
			}
			ReleaseTSP(oldirql);
			if(ThEntry) {
				DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %08x->%08x -------------\n",OldThreadId,NewThreadId));
			}	else {
				DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %08x->%08x \n",OldThreadId,NewThreadId));
			}
		}
#endif // __DBG__
//	if(OldThreadId != NewThreadId) {
		AcquireTSP(&oldirql);
		TSPSwapRegions((PVOID)OldThreadId,TSPRoot,FALSE);
		TSPSwapRegions((PVOID)NewThreadId,TSPRoot,TRUE);
		ReleaseTSP(oldirql);
		
//	}
	__asm {
		add esp,VariablesSize
		popad
		popf
		call [PatchedCodeF]
		jmp [ThreadSwitchCallbackPatchRet];
	}
}

BOOLEAN PatchSwapContextRoutine(PVOID NTOSKRNLBase)
{
DWORD SSPos;
static BYTE mov_byte_ptr_es_esi_2Dh_2[]={0x26,0xC6,0x46,0x2D,0x02}; //mov byte ptr es:[esi+2Dh], 2
static BYTE mov_ecx_pebx_cmp_pebx_plusXXXXXXXX_0[]={0x8B, 0x0B, 0x83, 0xBB};//mov ecx, [ebx];cmp dword ptr [ebx+XXXXXXXX], 0
BYTE			PatchData[10];
ULONG			PatchDataSize;
ULONG			oldirql;
BOOLEAN		OldProtect;
PVOID ThreadSwitchCallbackPatchAddr=NULL;
	try {
		if((SSPos=GetSubstringPosBM((BYTE*) NTOSKRNLBase, 0x150000,mov_byte_ptr_es_esi_2Dh_2,sizeof(mov_byte_ptr_es_esi_2Dh_2)))!=-1)
			ThreadSwitchCallbackPatchAddr=(PVOID)((DWORD)NTOSKRNLBase+SSPos);
	} except (EXCEPTION_EXECUTE_HANDLER) {
	}
	if(ThreadSwitchCallbackPatchAddr==NULL) {
		DbPrint(DC_TSP,DL_ERROR, ("GetSwapContextRoutine failed\n"));
		return FALSE;
	}
	if(*NtBuildNumber>2195) { //WinWX
		PatchDataSize=7;
		if((SSPos=GetSubstringPosBM((BYTE*)ThreadSwitchCallbackPatchAddr, 0x100,mov_ecx_pebx_cmp_pebx_plusXXXXXXXX_0,sizeof(mov_ecx_pebx_cmp_pebx_plusXXXXXXXX_0)))!=-1){
			ThreadSwitchCallbackPatchAddr=(PVOID)((DWORD)ThreadSwitchCallbackPatchAddr+SSPos+2);
		}	else {
			DbPrint(DC_TSP,DL_ERROR, ("GetSwapContextRoutine for XP failed\n"));
			return FALSE;
		}
	} else { //NT4 & 2k
		PatchDataSize=5;
	}

	memset(PatchData,0x90,sizeof(PatchData));
	PatchData[0]=0xe9;
	*(ULONG*)(PatchData+1)=(DWORD)ThreadSwitchCallback-(DWORD)ThreadSwitchCallbackPatchAddr-5;
	ThreadSwitchCallbackPatchRet=(ULONG)ThreadSwitchCallbackPatchAddr+PatchDataSize;
	memcpy(PatchedCode,ThreadSwitchCallbackPatchAddr,PatchDataSize);

	if(MyVirtualProtect(ThreadSwitchCallbackPatchAddr,TRUE,&OldProtect)) {
		AcquireTSP(&oldirql);
		memcpy(ThreadSwitchCallbackPatchAddr,&PatchData,PatchDataSize);
		ReleaseTSP(oldirql);
		MyVirtualProtect(ThreadSwitchCallbackPatchAddr,OldProtect,&OldProtect);
		return TRUE;
	}
	return FALSE;
}


NTSTATUS (__stdcall *NtSetInformationProcess)(HANDLE ProcessHandle,ULONG ProcessInformationClass,PVOID ProcessInformation,ULONG ProcessInformationLength);

NTSTATUS __stdcall HNtSetInformationProcess(HANDLE ProcessHandle,ULONG ProcessInformationClass,PVOID ProcessInformation,ULONG ProcessInformationLength)
{
NTSTATUS	NtStatus;
PEPROCESS ProcObj;
ULONG			oldirql;
PTHSUBPRO	ThEntry;
//	DbPrint(DC_TSP,DL_INFO, ("NtSetInformationProcess ProcessHandle=%x ProcessInformationClass=%x\n",ProcessHandle,ProcessInformationClass));
	if(ProcessInformationClass==0x15){
		if(ProcessHandle!=(PVOID)-1){
			NtStatus=ObReferenceObjectByHandle(ProcessHandle,0,NULL,KernelMode,&ProcObj,NULL);
			if(NT_SUCCESS(NtStatus)) {
				AcquireTSP(&oldirql);
				ThEntry=TSPRoot;
				while(ThEntry && ThEntry->OwnerProc!=ProcObj) {
					ThEntry=ThEntry->Next;
				}
				ReleaseTSP(oldirql);
				ObDereferenceObject(ProcObj);
				if(ThEntry) {
					DbPrint(DC_TSP,DL_NOTIFY, ("Attempt to set affinity for my client (%x) detected. ACCESS_DENIED\n",ProcObj));
					return STATUS_ACCESS_DENIED;
				}
			} else {
				DbPrint(DC_TSP,DL_ERROR, ("ObReferenceProcessObjectByHandle fail. ProcessHandle=%x NtStatus=%x\n",ProcessHandle,NtStatus));
			}
		}
	}
	return NtSetInformationProcess(ProcessHandle,ProcessInformationClass,ProcessInformation,ProcessInformationLength);
}

BOOLEAN HookSetProcessAffinity(VOID)
{
PVOID	NtDllBase;
ULONG	NativeID;
BOOLEAN		OldProtect;
	GetNativeBase(&NtDllBase,"NTDLL.DLL");
	if(NtDllBase) {

//		if(!(NativeID=GetNativeID((PVOID)NtDllBase,"NtLockVirtualMemory")))
//			return FALSE;
//		NtLockVirtualMemory=(long (__stdcall *)(void *,void *,int ,int ))*(*KeServiceDescriptorTable+NativeID);
		
		if(NativeID=GetNativeID((PVOID)NtDllBase,"NtSetInformationProcess")) {
			NtSetInformationProcess=(long (__stdcall *)(void *,unsigned long ,void *,unsigned long))*(*KeServiceDescriptorTable+NativeID);
			if(MyVirtualProtect(*KeServiceDescriptorTable+NativeID,TRUE,&OldProtect)) {
				*(*KeServiceDescriptorTable+NativeID)=(ULONG)HNtSetInformationProcess;
				MyVirtualProtect(*KeServiceDescriptorTable+NativeID,OldProtect,&OldProtect);
				DbPrint(DC_TSP,DL_NOTIFY, ("NtSetInformationProcess hooked\n"));
				return TRUE;
			}
		} else {
			DbPrint(DC_TSP,DL_ERROR, ("ID for NtSetInformationProcess not found\n"));
		}
	}
	return FALSE;
}

#else
THREADSWITCH_THUNK ThreadSwitchThunk;
VOID __stdcall ThreadSwitchCallback(THREADHANDLE OldThreadHandle,THREADHANDLE NewThreadHandle)
{
ULONG			oldirql;
#ifdef __DBG__
	{
	PTHSUBPRO	ThEntry;
	AcquireTSP(&oldirql);
	ThEntry=TSPRoot;
	while(ThEntry && ThEntry->Owner!=(PVOID)OldThreadHandle && ThEntry->Owner!=(PVOID)NewThreadHandle) {
				ThEntry=ThEntry->Next;
	}
	ReleaseTSP(oldirql);
	if(ThEntry) {
				DbPrint(DC_TSP,DL_INFO, ("ThreadSwitchCallback IRQL=%x %08x->%08x -------------\n",KeGetCurrentIrql(),OldThreadHandle,NewThreadHandle));
	}	else {
				DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %s %08x->%08x \n",KeGetCurrentIrql()>DISPATCH_LEVEL?"---IRQL>DISPATCH_LEVEL---":" ",OldThreadHandle,NewThreadHandle));
	}
	}
#endif // __DBG__
//	if(OldThreadHandle != NewThreadHandle) {
//		DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %08x->%08x\n",OldThreadHandle,NewThreadHandle));
		AcquireTSP(&oldirql);
		TSPSwapRegions((PVOID)OldThreadHandle,TSPRoot,FALSE);
		TSPSwapRegions((PVOID)NewThreadHandle,TSPRoot,TRUE);
		ReleaseTSP(oldirql);
//	}
}
#endif // _HOOK_NT_

BOOLEAN TSPInit(VOID)
{
#ifdef _HOOK_NT_
PVOID PsGetCurrentThreadIdEP;
VOID	*NtoskrnlBase;
	TSPSpinLock=0;
	HookSetProcessAffinity();
	GetNativeBase(&NtoskrnlBase,"NTOSKRNL.EXE");
	if(NtoskrnlBase) {
		if((PsGetCurrentThreadIdEP=GetExport(NtoskrnlBase,"PsGetCurrentThreadId",NULL))) {
			if((*(WORD*)PsGetCurrentThreadIdEP==0xA164) &&	 //	mov eax, large fs:XXXXXXXX
				(*((WORD*)PsGetCurrentThreadIdEP+3)==0x808B) &&//	mov eax, [eax+XXXXXXXX]
				(*((BYTE*)PsGetCurrentThreadIdEP+12)==0xc3)) { //	retn   
				ThreadIdOffsetInETHREAD=*((DWORD*)PsGetCurrentThreadIdEP+2);
				if(PatchSwapContextRoutine(NtoskrnlBase)) {
					DbPrint(DC_TSP,DL_NOTIFY, ("TSPInit ok\n"));
					TSPInitedOk=TRUE;
					return TRUE;
				}
			}
		}
	}
	DbPrint(DC_TSP,DL_ERROR, ("TSPInit fail\n"));
	return FALSE;
/*
	KeSetSwapContextNotifyRoutine(ThreadSwitchCallback);
	KeSetThreadSelectNotifyRoutine(ThreadSwitchCallback1);
*/
#else
	Call_When_Thread_Switched(ThreadSwitchCallback,&ThreadSwitchThunk);
	TSPInitedOk=TRUE;
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPInit ok\n"));
	return TRUE;
#endif // _HOOK_NT_
}

BOOLEAN TSPDone(VOID)
{
BOOLEAN ret=TRUE;
	TSPInitedOk=FALSE;
#ifdef _HOOK_NT_
#else
	ret=Cancel_Call_When_Thread_Switched(ThreadSwitchCallback,&ThreadSwitchThunk);
#endif // _HOOK_NT_
	CleanupTSP();
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPDone %d\n",ret));
	return ret;
}
