
typedef struct _TSPPAGE {
	struct _TSPPAGE *Next;
	PVOID	OrigVA;		// MUST be aligned on PAGE bound (divisible by PAGE_SIZE)
	ULONG	*OrigDesc;	
	PVOID	ShadowVA;
	ULONG	*ShadowDesc;	
	BOOLEAN	Swapped;
} TSPPAGE,*PTSPPAGE;

typedef enum _SWAPDIR{
	SD_Ready=0,
	SD_Need4Swp,
	SD_SwInPrgs,
	SD_Swapped
}SWAPDIR;

typedef struct _THSUBPRO {
	struct _THSUBPRO *Next;
	PVOID			Owner;
	PVOID			OwnerProc;
	PTSPPAGE	RegionList;
	ULONG			Flags;			//see TSP_??? bitfields
	SWAPDIR		Sd;
} THSUBPRO,*PTHSUBPRO;

PTHSUBPRO TSPRoot=NULL;
BOOLEAN TSPInitedOk=FALSE;

ULONG TSPSpinLock;

LARGE_INTEGER DelayThreadInterval={-1000000,-1};
//LARGE_INTEGER DelayThreadInterval={-1,-1};

FAST_MUTEX  TSPCRSTMutex;
ULONG		TSPCRSTMClaimCnt;
PETHREAD TSPCRSTMOwnerTh=NULL;

VOID InitTSPCRST(VOID)
{
	ExInitializeFastMutex(&TSPCRSTMutex);
	TSPCRSTMClaimCnt=0;
}

ULONG EnterTSPCRST(VOID)
{
	ExAcquireFastMutex(&TSPCRSTMutex);
	TSPCRSTMOwnerTh=PsGetCurrentThread();
	return InterlockedIncrement(&TSPCRSTMClaimCnt);
}

VOID LeaveTSPCRST(VOID)
{
	InterlockedDecrement(&TSPCRSTMClaimCnt);
	ExReleaseFastMutex(&TSPCRSTMutex);
	TSPCRSTMOwnerTh=NULL;
}

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
	InterlockedDecrement(&InTSP);
	HalpReleaseHighLevelLock(&TSPSpinLock,NewIrql);
}
#else 
#define AcquireTSP(OldIrql)	*OldIrql=HalpAcquireHighLevelLock(&TSPSpinLock)
#define ReleaseTSP(NewIrql)	HalpReleaseHighLevelLock(&TSPSpinLock,NewIrql)
#endif

PVOID ShadOwnerThId=0;

VOID TSPTouchAndSwap(PTHSUBPRO ThEntry,BOOLEAN SwapDirection)
{
ULONG	Tmp;
ULONG	oldirql;
PTSPPAGE	RgEntry;
ULONG Flags;
	if(ThEntry) {
		if(SwapDirection) {
			if(ShadOwnerThId) {
				if(ShadOwnerThId==ThEntry->Owner) {
					DbPrint(DC_TSP,DL_WARNING, ("!!! TSPTouchAndSwap OldOwn already %x\n",ShadOwnerThId));
					DbgBreakPoint();
					return;
				}
				DbPrint(DC_TSP,DL_FATAL_ERROR, ("!!! TSPTouchAndSwap OldOwn(%x)!=0\n",ShadOwnerThId));
				DbgBreakPoint();
			}
		} else {
			if(!ShadOwnerThId) {
				DbPrint(DC_TSP,DL_WARNING, ("!!! TSPTouchAndSwap OldOwn already %x\n",ShadOwnerThId));
				DbgBreakPoint();
				return;
			}
		}
		RgEntry=ThEntry->RegionList;
		while(RgEntry) {
			if(SwapDirection==RgEntry->Swapped) {
				DbPrint(DC_TSP,DL_WARNING, ("!!! TSPTouchAndSwap %s already done.\n",SwapDirection?"swap":"unswap"));
				RgEntry=RgEntry->Next;
				continue;
			}
#ifdef __DBG__
			if(!(*RgEntry->OrigDesc & 1)) {
				TouchPage(RgEntry->OrigVA);
//				DbPrint(DC_TSP,DL_FATAL_ERROR, ("!!! TSPTouchAndSwap orig page %x va %x is not present\n",*RgEntry->OrigDesc,RgEntry->OrigVA));
			} 
			if(!(*RgEntry->ShadowDesc & 1)) {
				TouchPage(RgEntry->ShadowVA);
//				DbPrint(DC_TSP,DL_FATAL_ERROR, ("!!! TSPTouchAndSwap shad page %x va %x is not present\n",*RgEntry->ShadowDesc,RgEntry->ShadowVA));
			} 
#endif
			if(ThEntry->Flags & TSP_INIT_ON_SWAPIN) {
				if(ThEntry->Flags & TSP_NO_COPY_ORIG) {
					memset(RgEntry->ShadowVA,0,PAGE_SIZE);
				} else {
					memcpy(RgEntry->ShadowVA,RgEntry->OrigVA,PAGE_SIZE);
				}
				__asm { //Set usermode bit
					mov edx,RgEntry;
					mov eax,[edx+TSPPAGE.ShadowDesc];
					lock or dword ptr [eax],4//4-Owner,16-CacheDisable,8-PageWriteThrough
					lock and dword ptr [eax],0xFFFFFEFF// 100h-Global
				}
			}
			AcquireTSP(&oldirql);
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
			ReleaseTSP(oldirql);
			RgEntry=RgEntry->Next;
		}
		ThEntry->Flags &= ~TSP_INIT_ON_SWAPIN;
		if(SwapDirection) {
			InterlockedExchange((ULONG*)&ShadOwnerThId,(ULONG)ThEntry->Owner);
		} else {
			InterlockedExchange((ULONG*)&ShadOwnerThId,0);
		}
		DbPrint(DC_TSP,DL_NOTIFY, ("TSPTouchAndSwap  New ShadOwner %08x\n",ShadOwnerThId));
	}
}

PTHSUBPRO TSPIsMyThread(PVOID Owner)
{
	PTHSUBPRO	ThEntry;
	ThEntry=TSPRoot;
	while(ThEntry && ThEntry->Owner!=Owner) {
		ThEntry=ThEntry->Next;
	}
	return ThEntry;
}

VOID TSPSwapScheduler(PTHSUBPRO	SwapIn)
{
PTHSUBPRO	ThEntry;
PTHSUBPRO	ThPrev;
PTHSUBPRO	SwapOut;
ULONG	oldirql;
	if(EnterTSPCRST()==1) {
		DbPrint(DC_TSP,DL_INFO, ("  --- EnterTSPCRST\n"));
		//Найдем OUTера
		AcquireTSP(&oldirql);
		SwapOut=TSPIsMyThread((PVOID)ShadOwnerThId);
		if(SwapOut) 
			SwapOut->Sd=SD_Ready;
		ReleaseTSP(oldirql);
		if((SwapOut!=SwapIn) || (SwapIn->Flags & TSP_UNLINK_ON_SWAPOUT)) {
			if(SwapOut) {
				DbPrint(DC_TSP,DL_INFO, ("TSPSwapOut %08x--->\n",SwapOut->Owner));
				TSPTouchAndSwap(SwapOut,FALSE);
			}
			if(SwapIn->Flags & TSP_UNLINK_ON_SWAPOUT) {
				DbPrint(DC_TSP,DL_INFO, ("TSPSwap TSP_UNLINK_ON_SWAPOUT  %x \n",SwapIn->Owner));
				AcquireTSP(&oldirql);
				ThPrev=NULL;
				ThEntry=TSPRoot;
				while(ThEntry) {
					if(ThEntry == SwapIn) {
						if(ThPrev) {
							ThPrev->Next=ThEntry->Next;
						} else {
							TSPRoot=ThEntry->Next;
						}
						break;
					}
					ThPrev=ThEntry;
					ThEntry=ThEntry->Next;
				}
				ReleaseTSP(oldirql);
			} else {
				DbPrint(DC_TSP,DL_INFO, ("TSPSwapIn --->%08x\n",SwapIn->Owner));
				TSPTouchAndSwap(SwapIn,TRUE);
				SwapIn->Sd=SD_Swapped;
			}
			DbPrint(DC_TSP,DL_INFO, ("TSPSwap End %08x--->%08x\n",SwapOut?SwapOut->Owner:0,SwapIn?SwapIn->Owner:0));
		} else {
			DbPrint(DC_TSP,DL_INFO, ("TSPSwap SwapOut==SwapIn (%x)\n",SwapIn->Owner));
			DbgBreakPoint();
		}
	} else {
		DbPrint(DC_TSP,DL_INFO,("!!! InTSPCRST=%x\n",TSPCRSTMClaimCnt));
	}
	LeaveTSPCRST();
}

VOID TSPSwapIndirect(VOID)
{
ULONG	oldirql;
PTHSUBPRO	ThEntry;
PTHSUBPRO	SwapIn=NULL;
PVOID			CurTh;
	if(KeGetCurrentIrql()<=APC_LEVEL) {
		AcquireTSP(&oldirql);
		CurTh=PsGetCurrentThreadId();
		ThEntry=TSPRoot;
		while(ThEntry) {
			if(ThEntry->Owner==CurTh){
				if(ThEntry->Sd==SD_Need4Swp) {
					SwapIn=ThEntry;
					ThEntry->Sd=SD_SwInPrgs;
				}
				break;
			} 
			ThEntry=ThEntry->Next;
		}
		ReleaseTSP(oldirql);
		if(SwapIn) {
			TSPSwapScheduler(SwapIn);
		}
	}
}

VOID TSPMarkForSwap(PVOID OldThreadId,PVOID NewThreadId)
{
PTHSUBPRO	ThEntry;
	if(OldThreadId==NewThreadId)
		return;
	ThEntry=TSPRoot;
	while(ThEntry) {
		if(ThEntry->Owner==NewThreadId){
			if(ThEntry->Sd==SD_Ready) {
//				DbPrint(DC_TSP,DL_INFO, ("TSPMarkForSwap %x\n",NewThreadId));
				ThEntry->Sd=SD_Need4Swp;
			}
			break;
		} 
		ThEntry=ThEntry->Next;
	}
}

/*
VOID TSPSwapRegions(PVOID OwnTh,PTHSUBPRO Root,BOOLEAN SwapDirection)
{
ULONG	Tmp;
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
#ifdef __DBG__
//blablabla:
			if(!(*RgEntry->OrigDesc & 1)) {
				DbPrint(DC_TSP,DL_FATAL_ERROR, ("!!! TSPSwapRegions orig page %x va %x is not present\n",*RgEntry->OrigDesc,RgEntry->OrigVA));
//				DbgBreakPoint();
//				TouchPage(RgEntry->OrigVA);
//				goto blablabla;
			}
			if(!(*RgEntry->ShadowDesc & 1)) {
				DbPrint(DC_TSP,DL_FATAL_ERROR, ("!!! TSPSwapRegions shad page %x va %x is not present\n",*RgEntry->ShadowDesc,RgEntry->ShadowVA));
//				DbgBreakPoint();
//				TouchPage(RgEntry->ShadowVA);
//				goto blablabla;
			}
#endif
			RgEntry->Swapped=!RgEntry->Swapped;
//Swap page discriptors
//			DbPrint(DC_TSP,DL_SPAM, ("TSPSwapPages OrigVA=%x ShadVA=%x OrigPD=%x ShadPD=%x\n",RgEntry->OrigVA,RgEntry->ShadowVA,RgEntry->OrigDesc,RgEntry->ShadowDesc));
			Tmp=*RgEntry->OrigDesc;
			*RgEntry->OrigDesc = *RgEntry->ShadowDesc;
			*RgEntry->ShadowDesc=Tmp;
			__asm {
				mov eax,RgEntry;
				mov ebx,[eax+TSPPAGE.OrigVA]
				invlpg [ebx];
//				mov ebx,[ebx]
				mov ebx,[eax+TSPPAGE.ShadowVA]
				invlpg [ebx];
//				mov ebx,[ebx]
			}
			RgEntry=RgEntry->Next;
		}
//Flush TLB
//		__asm {
//			mov eax,cr3;
//			mov cr3,eax;
//			wbinvd;
//		}
	}
}
*/

PVOID TSPAllocPage(BOOLEAN KernelMemSpace)
{
PVOID Addr=NULL;
#ifdef _HOOK_NT_
ULONG RegionSize;
	RegionSize=PAGE_SIZE;
	if(KernelMemSpace) {
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

BOOLEAN TSPAllocShadow(PTSPPAGE RgEntry, ULONG Flags)
{
PVOID ShadVa;
	if(RgEntry->ShadowVA) {
		ShadVa=RgEntry->ShadowVA;
		DbPrint(DC_TSP,DL_WARNING, ("TSPMakeShadow: ShadVa=%x!=NULL. \n"));
	} else {
		ShadVa=TSPAllocPage((BOOLEAN)(Flags & TSP_KRNL_MEMSPACE_SHADOW));
		if(ShadVa==NULL) {
			DbPrint(DC_TSP,DL_ERROR, ("TSPMakeShadow: no mem for sh page. \n"));
			goto err;
		}
	}
	if((ULONG)ShadVa % PAGE_SIZE !=0) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPMakeShadow: ShadVa %x is not aligned. \n",ShadVa));
		goto err;
	}
	if((RgEntry->OrigDesc=GetPageDiscriptor(RgEntry->OrigVA))!=BADP2LMAPPING) {
		if((RgEntry->ShadowDesc=GetPageDiscriptor(ShadVa))!=BADP2LMAPPING) {
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
	DbPrint(DC_TSP,DL_INFO, ("DelTSPRegios OwnTh=%x OwnProc=%x Flags=%x\n",ThEntry->Owner,ThEntry->OwnerProc,ThEntry->Flags));
	RgEntry=ThEntry->RegionList;
	while(RgEntry) {
		if(RgEntry->Swapped) {
			DbPrint(DC_TSP,DL_ERROR, ("!!! DelTSPRegions: %x is swapped.\n",ThEntry->Owner));
			DbgBreakPoint();
		}
		Tmp=RgEntry->Next;
//		DbPrint(DC_TSP,DL_SPAM, ("DelTSPRegion Owner=%x OrigVA=%x ShadVA=%x OrigPD=%x ShadPD=%x\n",
//				ThEntry->Owner,RgEntry->OrigVA,RgEntry->ShadowVA,RgEntry->OrigDesc,RgEntry->ShadowDesc));
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
	AcquireTSP(&oldirql);
	ThEntry=TSPRoot;
	while(ThEntry && ThEntry->Owner!=Owner) {
		ThEntry=ThEntry->Next;
	}
	if(!ThEntry) {
		ReleaseTSP(oldirql);
		return;
	} // Do not delete here!!!! Mark for delete/
	ThEntry->Flags|=TSP_UNLINK_ON_SWAPOUT;
	ThEntry->Sd=SD_Ready;
	ReleaseTSP(oldirql);

	DbPrint(DC_TSP,DL_NOTIFY, ("DelTSPOwner 1 %x \n",Owner));
	//после этой команды трида должна оказаться в SwapIn с установленным флагом удаления Ы?
	KeDelayExecutionThread(KernelMode,FALSE,&DelayThreadInterval); 
	DbPrint(DC_TSP,DL_NOTIFY, ("DelTSPOwner 2 %x \n",Owner));
	DelTSPRegions(ThEntry);
	DbPrint(DC_TSP,DL_NOTIFY, ("DelTSPOwner 3 %x \n",Owner));
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
		DbPrint(DC_TSP,DL_INFO, ("AddTSPRegions OwnTh=%x OwnProc=%x OVA[%d]=%x Size=%d\n",
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
		ThEntry->Flags|=TSP_INIT_ON_SWAPIN;
		ThEntry->Owner=OwnTh;
		ThEntry->OwnerProc=OwnPr;
		ThEntry->RegionList=NULL;
		ThEntry->Sd=SD_Ready;
		ThEntry->Next=NULL;
	} else {
		DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion failed, no memory for ThEntry...\n"));
		return FALSE;
	}
	for(i=0;i<pPageSet->m_PageCount;i++) {
		for(j=0;j<pPageSet->m_PageParams[i].m_RegionSize;j++) {
			RgEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSPPAGE),'TSeB');
			if(RgEntry) {
				RgEntry->OrigVA=(PVOID)((PCHAR)(pPageSet->m_PageParams[i].m_OrigAddr)+j*PAGE_SIZE);
				RgEntry->Swapped=FALSE;
				RgEntry->ShadowVA=NULL;
				if(!TSPAllocShadow(RgEntry,ThEntry->Flags)) {
					DelTSPRegions(ThEntry);
					return FALSE;
				}
				RgEntry->Next=ThEntry->RegionList;
				ThEntry->RegionList=RgEntry;
//				DbPrint(DC_TSP,DL_SPAM, ("AddTSPRegion Owner=%x OrigVA=%x ShadVA=%x OrigPD=%x ShadPD=%x\n",
//					OwnTh,RgEntry->OrigVA,RgEntry->ShadowVA,RgEntry->OrigDesc,RgEntry->ShadowDesc));
			} else {
				DbPrint(DC_TSP,DL_ERROR, ("AddTSPRegion failed, no memory for RgEntry...\n"));
				DelTSPRegions(ThEntry);
				return FALSE;
			}
		}
	}
	DbPrint(DC_TSP,DL_INFO, ("AddTSPRegions: OwnTh=%x added. Link to list.\n",OwnTh));
//	EnterTSPCRST();
//	TSPTouchAndSwap(ThEntry,TRUE);
//	TSPSwapRegions(OwnTh,ThEntry,TRUE);
	AcquireTSP(&oldirql);
	//Check list
	ThETmp=TSPRoot;
	while(ThETmp && ThETmp->Owner!=OwnTh) {
		ThETmp=ThETmp->Next;
	}
	if(ThETmp) { //Already in list
		DbPrint(DC_TSP,DL_WARNING, ("!!! AddTSPRegion thread %x already exist in swaplist.\n",OwnTh));
		DbgBreakPoint();
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
//	LeaveTSPCRST();
	DbPrint(DC_TSP,DL_NOTIFY, ("AddTSPRegions: OwnTh=%x added.\n",OwnTh));
	KeDelayExecutionThread(KernelMode,FALSE,&DelayThreadInterval);
	DbPrint(DC_TSP,DL_INFO, ("AddTSPRegions return.OwnTh=%x\n",OwnTh));
	if(ThEntry) //Если трида уже была в списке - добиваем временную память
		ExFreePool(ThEntry);
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------

#ifdef _HOOK_NT_
DWORD ThreadSwitchCallbackPatchRet;

BYTE PatchedCodeSC[]={0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0xC3};
PVOID PatchedCodeSCF=PatchedCodeSC;

VOID __declspec(naked) ThreadSwitchCallback(VOID)//
{
ULONG	oldirql;
HANDLE OldThreadId;
HANDLE NewThreadId;
#define TSCVariablesSize 0x10
	__asm {
		pushfd;
		pushad;
		mov     ebp, esp;
		sub esp,TSCVariablesSize;
		add esi,ThreadIdOffsetInETHREAD;
		add edi,ThreadIdOffsetInETHREAD;
		mov eax,[esi];
		mov NewThreadId,eax;
		mov eax,[edi];
		mov OldThreadId,eax;
		
	}
	AcquireTSP(&oldirql);
		DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %c%08x ->%c%08x\n",TSPIsMyThread(OldThreadId)?'!':' ',OldThreadId,TSPIsMyThread(NewThreadId)?'!':' ',NewThreadId));
		TSPMarkForSwap((PVOID)OldThreadId,(PVOID)NewThreadId);
	ReleaseTSP(oldirql);
	__asm {
		add esp,TSCVariablesSize
		popad
		popfd
		call [PatchedCodeSCF]
		jmp [ThreadSwitchCallbackPatchRet];
	}
}

BOOLEAN PatchSwapC(PVOID NTOSKRNLBase)
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
	memcpy(PatchedCodeSC,ThreadSwitchCallbackPatchAddr,PatchDataSize);

	if(MyVirtualProtect(ThreadSwitchCallbackPatchAddr,TRUE,&OldProtect)) {
		AcquireTSP(&oldirql);
		memcpy(ThreadSwitchCallbackPatchAddr,&PatchData,PatchDataSize);
		ReleaseTSP(oldirql);
		MyVirtualProtect(ThreadSwitchCallbackPatchAddr,OldProtect,&OldProtect);
		return TRUE;
	}
	return FALSE;
}


#include <pshpack1.h>
#define PATCH_THUNK_SAVE_SIZE 10
typedef struct _PATCH_THUNK {
	ULONG	PushCallOpC;
	ULONG HookerAddr;
	USHORT	PopOpC;
	BYTE	SavedCode[PATCH_THUNK_SAVE_SIZE];
	BYTE	JmpNOpC;
	ULONG	RetAddr;
} PATCH_THUNK, *PPATCH_THUNK;
#include <poppack.h>

BOOLEAN HookSysExit(BYTE* HookAddr,PVOID HookerAddr,PPATCH_THUNK Thunk)
{
//FA F7 45 70 00 00 02 00              cli test    dword ptr [ebp+70h], 20000h //XP chk mp & betas
//F7 44 24 70 00 00 02 00                 test    dword ptr [esp+70h], 20000h //All nt4 & 2k fre
// mov     bx, fs //all 2k-chk
ULONG			PatchDataSize=0;
BYTE			PatchData[PATCH_THUNK_SAVE_SIZE];
ULONG			oldirql;
BOOLEAN		OldProtect;
BOOLEAN		LeadCli;
	if(HookAddr) {
		if(*HookAddr==0xfa) {//cli
			HookAddr++;
			LeadCli=TRUE;
			PatchDataSize++;
		} else {
			LeadCli=FALSE;
		}
		if(*HookAddr==0xf7) {
			if(*(ULONG*)(HookAddr+1)==0x7045)
				PatchDataSize+=7;
			else if(*(ULONG*)(HookAddr+1)==0x702444)
				PatchDataSize+=8;
			else
				PatchDataSize=0;
			if(PatchDataSize) {
				if(LeadCli)
					HookAddr--;
				memset(PatchData,0x90,sizeof(PatchData));
				PatchData[0]=0xe9;
				*(ULONG*)(PatchData+1)=(ULONG)&(Thunk->PushCallOpC)-(ULONG)HookAddr-5;
				memset(Thunk->SavedCode,0x90,PATCH_THUNK_SAVE_SIZE);
				memcpy(Thunk->SavedCode,HookAddr,PatchDataSize);
				Thunk->HookerAddr= (ULONG)HookerAddr - (ULONG)&(Thunk->HookerAddr) - 4;
				Thunk->RetAddr= (ULONG)HookAddr + PatchDataSize - (ULONG)&(Thunk->RetAddr) - 4;
				Thunk->JmpNOpC=0xe9;
				Thunk->PopOpC=0x9d61;
				Thunk->PushCallOpC=0xe8609c90;
				if(MyVirtualProtect(HookAddr,TRUE,&OldProtect)) {
					AcquireTSP(&oldirql);
					memcpy(HookAddr,&PatchData,PatchDataSize);
					ReleaseTSP(oldirql);
					MyVirtualProtect(HookAddr,OldProtect,&OldProtect);
					return TRUE;
				}
			}
		}
	}
	DbPrint(DC_TSP,DL_ERROR, ("HookSysExit fail\n"));
	return FALSE;
}

PATCH_THUNK EoiHlpThunk;
PATCH_THUNK KiServiceExit2Thunk;
PATCH_THUNK KiServiceExitThunk;

BOOLEAN HookSystemExits(PVOID NtoskrnlBase)
{
ULONG	NativeID;
PVOID NtContinue;
DWORD SSPos;
PVOID	NtDllBase;
DWORD SrchSig;
BYTE* KiServiceExit2;
BYTE* KiServiceExit;
	__try {
		if(HookSysExit(GetExport(NtoskrnlBase,"Kei386EoiHelper",NULL),TSPSwapIndirect,&EoiHlpThunk)) {
			GetNativeBase(&NtDllBase,"NTDLL.DLL");
			if(NtDllBase) {
				if(NativeID=GetNativeID((PVOID)NtDllBase,"NtContinue")) {
					NtContinue=(PVOID)(*(*KeServiceDescriptorTable+NativeID));
					if(NtContinue) {
						SrchSig=0xe9e58b5d;
						if((SSPos=GetSubstringPosBM(NtContinue, 0x80,(BYTE*)&SrchSig,sizeof(SrchSig)))!=-1){
							NtContinue=(BYTE*)NtContinue+SSPos;
							if(*((DWORD*)NtContinue+2)==SrchSig) {
								KiServiceExit2=(BYTE*)(*((DWORD*)NtContinue+1))+(DWORD)((DWORD*)NtContinue+1)+4;
								KiServiceExit=(BYTE*)(*((DWORD*)NtContinue+3))+(DWORD)((DWORD*)NtContinue+3)+4;
								if(HookSysExit(KiServiceExit2,TSPSwapIndirect,&KiServiceExit2Thunk) &&
									HookSysExit(KiServiceExit,TSPSwapIndirect,&KiServiceExitThunk))
									return TRUE;
							}
						}
					}
				}
			}
		}
	} __except(EXCEPTION_EXECUTE_HANDLER) {
	}
	DbPrint(DC_TSP,DL_ERROR, ("HookSystemExits fail\n"));
	return FALSE;
}

/*
DWORD EoiHlpPatchRet;
DWORD PatchedCodeEH[]={0x90909090, 0x90909090, 0x25ff9090, (DWORD)&EoiHlpPatchRet};
PVOID PatchedCodeEHF=PatchedCodeEH;

VOID __declspec(naked) MyEoiHlp(VOID)
{
	__asm {
		pushfd;
		pushad;
	}
	if(KeGetCurrentIrql()<=APC_LEVEL)
		TSPSwapIndirect();
	__asm {
		popad;
		popfd;
		jmp [PatchedCodeEHF];
	}
}

BOOLEAN HookEoiHlp(PVOID NtoskrnlBase)
{
BYTE* Kei386EoiHelperAddr;
BYTE			PatchData[10];
ULONG			PatchDataSize=0;
ULONG			oldirql;
BOOLEAN		OldProtect;
	Kei386EoiHelperAddr=GetExport(NtoskrnlBase,"Kei386EoiHelper",NULL);
	if(Kei386EoiHelperAddr) {
		//FA F7 45 70 00 00 02 00              cli test    dword ptr [ebp+70h], 20000h //XP chk mp & betas
		//F7 44 24 70 00 00 02 00                 test    dword ptr [esp+70h], 20000h //All nt4 & 2k fre
		// mov     bx, fs //all 2k-chk
		if(*Kei386EoiHelperAddr==0xfa) //cli
			Kei386EoiHelperAddr++;
		if(*Kei386EoiHelperAddr==0xf7) {
			if(*(ULONG*)(Kei386EoiHelperAddr+1)==0x7045)
				PatchDataSize=7;
			else if(*(ULONG*)(Kei386EoiHelperAddr+1)==0x702444)
				PatchDataSize=8;
			else
				PatchDataSize=0;
			if(PatchDataSize) {
				memset(PatchData,0x90,sizeof(PatchData));
				PatchData[0]=0xe9;
				*(ULONG*)(PatchData+1)=(DWORD)MyEoiHlp-(DWORD)Kei386EoiHelperAddr-5;
				EoiHlpPatchRet=(ULONG)Kei386EoiHelperAddr+PatchDataSize;
				memcpy(PatchedCodeEH,Kei386EoiHelperAddr,PatchDataSize);
				if(MyVirtualProtect(Kei386EoiHelperAddr,TRUE,&OldProtect)) {
					AcquireTSP(&oldirql);
					memcpy(Kei386EoiHelperAddr,&PatchData,PatchDataSize);
					ReleaseTSP(oldirql);
					MyVirtualProtect(Kei386EoiHelperAddr,OldProtect,&OldProtect);
					return TRUE;
				}
			}
		}
	}
	DbPrint(DC_TSP,DL_ERROR, ("HookEoiHlp fail\n"));
	return FALSE;
}
*/

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

VOID (__fastcall *OldKfLowerIrql)(KIRQL NewIrql);

VOID __declspec(naked) __fastcall MyKfLowerIrql(KIRQL NewIrql)
{
	__asm {
		push ecx;
		call OldKfLowerIrql;
		pop ecx;
		cmp cl,1;
		jg toret;
		call TSPSwapIndirect;
toret:
		ret;
	}
}

BOOLEAN HookHal(PVOID NtoskrnlBase)
{
BOOLEAN		OldProtect;
DWORD SSPos;
ULONG	oldirql;
PVOID	HalBase;
PVOID KfLowerIrqlAddr;
ULONG* KfLowerIrqlImpAddr;
BYTE *ImportsStart;
	if(*NtBuildNumber>1381) { //>Nt4
		ImportsStart=(BYTE*)NtoskrnlBase;
	} else {
		IMAGE_SECTION_HEADER *SectHeader;
		SectHeader=IMAGE_FIRST_SECTION(MakePtr(PIMAGE_NT_HEADERS,NtoskrnlBase,((PIMAGE_DOS_HEADER)NtoskrnlBase)->e_lfanew))+1;
		ImportsStart=(BYTE*)NtoskrnlBase+SectHeader->VirtualAddress;
	}
	GetNativeBase(&HalBase,"HAL.DLL");
	if(HalBase) {
		if((KfLowerIrqlAddr=GetExport(HalBase,"KfLowerIrql",NULL))) {
			if((SSPos=GetSubstringPosBM(ImportsStart, 0x2000,(BYTE*)&KfLowerIrqlAddr,sizeof(KfLowerIrqlAddr)))!=-1){
				KfLowerIrqlImpAddr=(ULONG*)(ImportsStart+SSPos);
				if(MyVirtualProtect(KfLowerIrqlImpAddr,TRUE,&OldProtect)) {
					OldKfLowerIrql=(void(__fastcall *)(UCHAR))*KfLowerIrqlImpAddr;
					AcquireTSP(&oldirql);
					*KfLowerIrqlImpAddr=(ULONG)MyKfLowerIrql;
					ReleaseTSP(oldirql);
					MyVirtualProtect(KfLowerIrqlImpAddr,OldProtect,&OldProtect);
					return TRUE;
				}
			}
		}
	}
	DbPrint(DC_TSP,DL_ERROR, ("HookHal fail\n"));
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
		DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %08x->%08x -------------\n",OldThreadHandle,NewThreadHandle));
	}	else {
		DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %08x->%08x \n",OldThreadHandle,NewThreadHandle));
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
VOID	*NtoskrnlBase;
	TSPSpinLock=0;
	InitTSPCRST();
	HookSetProcessAffinity();
	GetNativeBase(&NtoskrnlBase,"NTOSKRNL.EXE");
	if(NtoskrnlBase) {
		if(HookHal(NtoskrnlBase)) {
			if(HookSystemExits(NtoskrnlBase)) {
//			if(HookEoiHlp(NtoskrnlBase)) {
				if(PatchSwapC(NtoskrnlBase)) {
					DbPrint(DC_TSP,DL_IMPORTANT, ("TSPInit ok\n"));
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





















