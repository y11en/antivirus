#include "TSPv2.h"
#include "debug.h"

#ifdef _HOOK_NT_
#include "nt/glbenvir.h"
#endif

DWORD gdwPAE = 0;
LONG gMinShadow = 4;

BOOLEAN gbWasReuse = FALSE;
BOOLEAN gbAllowFreeShadow = FALSE;

typedef struct _TSPPAGESET
{
	PVOID					VA;
	ULONGLONG*				Desc;
} TSPPAGESET,*PTSPPAGESET;

typedef struct _TSP_SHAD
{
	struct _TSP_SHAD		*Next;
	PVOID					OwnerTh;
	PVOID					ShadVa;
	TSPPAGESET				Pages[1];
}TSP_SHAD,*PTSP_SHAD;

typedef struct _TSP_REG
{
	struct _TSP_REG			*Next;
	ORIG_HANDLE				hOrig;//До TSPLock здесь память на инициализацию оригинала. После - условно некомиченное резервированное адресное пр-во (физические страницы от оригинала или от последней использованной тени)
	PVOID					OrigPageDir;//don't reinit
	FAST_MUTEX				ShadMutex;//don't reinit
	BOOLEAN					bOwned;
	ULONG					hOrigCommitedSize;
	WORD					notusedalign;
	PTSPPAGESET				OrigPageLst; 
	PVOID					pOrig;//До TSPLock - NULL. После - указатель на адрес оригинала (!!!физических страниц там нет!!!... :-) ...уже есть). 
										//Также является признаком успешной залочки.
	ULONG					DecomitDescr;//Значение дескриптора декоммиченной страницы
	ULONG					ReservedSize;
	LONG					MinNumShads;//Минимальное кол-во свободных (готовых к работе) тенек
	LONG					NumShads;
	LONG					CurShadsInUse;
	PTSP_SHAD				ShadList;
	PTSP_SHAD				CurOwner;
}TSP_REG,*PTSP_REG;

typedef struct _TSP_PROC
{
	struct _TSP_PROC		*Next;
	struct _TSP_REG			*RegList;
	PVOID					OwnerProcId;
	PVOID					OwnerProc;
#ifdef _HOOK_NT_
	ULONG					WorkingProcessor;
#endif
}TSP_PROC,*PTSP_PROC;

typedef struct _TSP_RS
{
	struct _TSP_RS			*Next;
	PTSP_REG				Rg;
	PTSP_SHAD				Sh;
}TSP_RS,*PTSP_RS;

typedef struct _TSP_THRD
{
	struct _TSP_THRD		*Next;
	PVOID					Th;
	PTSP_RS					RS;
}TSP_THRD,*PTSP_THRD;


BOOLEAN TSPInitedOk=FALSE;

PTSP_PROC TSPPrRoot=NULL;
FAST_MUTEX TSPPrMutex;

PTSP_THRD TSPThRoot=NULL;
ULONG TSPSpinLock;

#define TSP4Mb 0x400000

#define AcquireTSP(OldIrql)	*OldIrql=HalpAcquireHighLevelLock(&TSPSpinLock)
#define ReleaseTSP(NewIrql)	HalpReleaseHighLevelLock(&TSPSpinLock,NewIrql)

PTSP_THRD	TSPGetTh(PVOID Th)
{
PTSP_THRD	ThEntry;
	ThEntry=TSPThRoot;
	while(ThEntry) {
		if(ThEntry->Th==Th)
			break;
		ThEntry=ThEntry->Next;
	}
	return ThEntry;
}


__declspec(naked) VOID __fastcall TSPSwapPages(PTSPPAGESET Pa1,PTSPPAGESET Pa2)
{
	__asm {
//		wbinvd;
		push esi;
		push edi;
pgloop:
		mov esi,[ecx+PTSPPAGESET.Desc];
		or esi,esi;
		jz	stoploop;
		mov edi,[edx+PTSPPAGESET.Desc];
		or edi,edi;
		jz	stoploop;

		mov eax,[esi];
		lock xchg eax,[edi];
		lock xchg eax,[esi];
		
		mov eax, [gdwPAE];
		or eax, eax;
		jz swap_nopae;

		add esi, size ULONG;
		add edi, size ULONG;
		mov eax,[esi];
		lock xchg eax,[edi];
		lock xchg eax,[esi];

swap_nopae:
		
		mov eax,[ecx+PTSPPAGESET.VA];
		invlpg [eax];
		mov eax,[edx+PTSPPAGESET.VA];
		invlpg [eax];
		
		add ecx,size TSPPAGESET;
		add edx,size TSPPAGESET;
		jmp pgloop;
stoploop:
		pop edi;
		pop esi;
//		wbinvd;
		retn;
	}
}

__declspec(naked) ULONG __fastcall TSPCopyPgDesc(PTSPPAGESET Dst,PTSPPAGESET Src)
{
	__asm {
//		wbinvd;
		push esi;
		push edi;
		push ebx;
pgloop:
		mov esi,[edx+PTSPPAGESET.Desc];
		or esi,esi;
		jz	stoploop;
		mov edi,[ecx+PTSPPAGESET.Desc];
		or edi,edi;
		jz	stoploop;
		
		mov eax,[esi];
		lock xchg eax,[edi];

		mov ebx, [gdwPAE]
		or ebx, ebx;
		jz copy_nopae;

		add esi, size ULONG;
		add edi, size ULONG;
		mov eax,[esi];
		lock xchg eax,[edi];
copy_nopae:

		mov ebx,[ecx+PTSPPAGESET.VA];
		invlpg [ebx];
		
		add ecx,size TSPPAGESET;
		add edx,size TSPPAGESET;
		jmp pgloop;
stoploop:
		pop	ebx;
		pop edi;
		pop esi;
//		wbinvd;
		retn;
	}
}

/*__declspec(naked) VOID __fastcall TSPSetPgDesc(PTSPPAGESET Dst,ULONG Desc)
{
__asm {
push edi;
pgloop:
mov edi,[ecx+PTSPPAGESET.Desc];
or edi,edi;
jz	stoploop;

		mov eax,edx;
		lock xchg eax,[edi];
		
		  mov eax,[ecx+PTSPPAGESET.VA];
		  invlpg [eax];
		  
			add ecx,size TSPPAGESET;
			jmp pgloop;
			stoploop:
			pop edi;
			retn;
			}
}*/

#ifdef _HOOK_NT_
VOID TSPSetPgDescNative(PTSPPAGESET Dst, ULONG Desc)
{
ULONG* pAddr;
ULONGLONG* pAddrEx;
PVOID  Va;
//	__asm wbinvd;
	if (Dst == 0)
		return;
	while (Dst->Desc)
	{
		Va=Dst->VA;
		pAddrEx = GetPageDescriptorNative(Va);
		pAddr = (ULONG*) pAddrEx;
		if (pAddr != BADP2LMAPPING) {
			__asm {
				mov edi,[pAddr];
				mov eax,[Desc];
				lock xchg eax,[edi];

				mov eax, [gdwPAE];
				or eax, eax;
				jz setpg_nopae;
				
				add edi, size ULONG;
				xor eax, eax;
				lock xchg eax,[edi];
setpg_nopae:

				mov eax,[Va];
				invlpg [eax];
			}
		}
		Dst++;
	}
//	__asm wbinvd;
}

#endif // _HOOK_NT_

#ifndef _HOOK_NT_

#define CheckDescriptors(_Dsc, _Level)

#else

#define CheckDescriptors(_Dsc, _Level)

//typedef VOID (*_tpCheckDescriptors)(PTSPPAGESET Dsc,ULONG Level);
//_tpCheckDescriptors CheckDescriptors = NULL;

VOID CheckDescriptorsGeneric(PTSPPAGESET Dsc,ULONG Level)
{
#ifdef __DBG__
	ULONGLONG* pAddrEx;
	ULONG* pAddr;
	if (Dsc == 0)
		return;
	while(Dsc->Desc)
	{
		if(((*Dsc->Desc & PDE_P) == 0) && (*Dsc->Desc != 0x200))
		{
			ULONG* pd = (ULONG*) Dsc->Desc;
			DbgBreakPoint();
			KeBugCheckEx(0x000000E2,(ULONG)Dsc,(ULONG)Dsc->VA,(ULONG) pd,*pd);
		}
		if(Level>0) { //Регистр CR3 выставлен на нужный процесс
			pAddrEx = GetPDEEntryNativeLite(Dsc->VA);
			pAddr = (ULONG*) pAddrEx;
			if(*pAddr & PDE_P){
				pAddrEx = GetPageDescriptorNativeLite(Dsc->VA);
				pAddr = (ULONG*) pAddrEx;
				if((ULONG) *Dsc->Desc != *pAddr)
				{
					DbgBreakPoint();
					KeBugCheckEx(0x000000E2,(ULONG)Dsc,(ULONG)Dsc->VA,(ULONG)*Dsc->Desc,(ULONG)*pAddr);
				}
			} else if(*pAddr & PDE_TRN){
				DbgPrint("!!!!!!!!!!!!! Transition PTE !!!!!!!!!!!\n");
			} else
			{
				DbgBreakPoint();
				KeBugCheckEx(0x000000E2,(ULONG)Dsc,(ULONG)Dsc->VA,(ULONG)*Dsc->Desc,(ULONG)*pAddr);
			}
		}
		Dsc++;
	}
#endif // __DBG__
}

VOID CheckDescriptorsPAE(PTSPPAGESET Dsc,ULONG Level)
{
#ifdef __DBG__
	ULONGLONG* pAddrEx;
	if (Dsc == 0)
		return;
	while(Dsc->Desc)
	{
		if(((*Dsc->Desc & PDE_P) == 0) && (*Dsc->Desc != 0x200))
		{
			ULONG* pd = (ULONG*) Dsc->Desc;
			DbgBreakPoint();
			KeBugCheckEx(0x000000E2,(ULONG)Dsc,(ULONG)Dsc->VA,(ULONG) pd,*pd);
		}
		if(Level>0) { //Регистр CR3 выставлен на нужный процесс
			pAddrEx = GetPDEEntryNativeLite(Dsc->VA);
			if(*(ULONG*)pAddrEx & PDE_P){
				pAddrEx = GetPageDescriptorNativeLite(Dsc->VA);
				if(*Dsc->Desc != *pAddrEx)
				{
					DbgBreakPoint();
					KeBugCheckEx(0x000000E2,(ULONG)Dsc,(ULONG)Dsc->VA,(ULONG)*Dsc->Desc,(ULONG)*pAddrEx);
				}
			} else if(*pAddrEx & PDE_TRN){
				DbgPrint("!!!!!!!!!!!!! Transition PTE !!!!!!!!!!!\n");
			} else
			{
				DbgBreakPoint();
				KeBugCheckEx(0x000000E2,(ULONG)Dsc,(ULONG)Dsc->VA,(ULONG)*Dsc->Desc,(ULONG)*pAddrEx);
			}
		}
		Dsc++;
	}
#endif // __DBG__
}
#endif

VOID TSPSwapInTh(PVOID ThIn)
{
PTSP_THRD	ThEntry;
PTSP_RS		RsEntry;
	ThEntry=TSPGetTh(ThIn);
	if(ThEntry==NULL)
		return;
	RsEntry=ThEntry->RS;
	while(RsEntry) {
		if(RsEntry->Rg->CurOwner!=RsEntry->Sh) {
			CheckDescriptors(RsEntry->Rg->OrigPageLst,0);
			if(RsEntry->Rg->CurOwner) {
				CheckDescriptors(&RsEntry->Rg->CurOwner->Pages[0],1);
				TSPSwapPages(RsEntry->Rg->OrigPageLst,&RsEntry->Rg->CurOwner->Pages[0]);
			}
			CheckDescriptors(&RsEntry->Sh->Pages[0],1);
			TSPSwapPages(RsEntry->Rg->OrigPageLst,&RsEntry->Sh->Pages[0]);
			RsEntry->Rg->CurOwner=RsEntry->Sh;
		}
		RsEntry=RsEntry->Next;
	}
}

VOID TSPSwapOutTh(PVOID ThOut,PTSP_REG RgEntry)
//if RgEntry==NULL swapout all regions which thread owns
{
PTSP_THRD	ThEntry;
PTSP_RS		RsEntry;
	ThEntry=TSPGetTh(ThOut);
	if(ThEntry==NULL)
		return;
	RsEntry=ThEntry->RS;
	while(RsEntry) {
		if(RsEntry->Rg->CurOwner==RsEntry->Sh) {
			if(RgEntry==NULL || RsEntry->Rg==RgEntry) {
				if(RsEntry->Rg->OrigPageLst) {
					CheckDescriptors(RsEntry->Rg->OrigPageLst,1);
					CheckDescriptors(&RsEntry->Sh->Pages[0],1);
					TSPSwapPages(RsEntry->Rg->OrigPageLst,&RsEntry->Sh->Pages[0]);
				}
				RsEntry->Rg->CurOwner=NULL;
			}
		}
		RsEntry=RsEntry->Next;
	}
}

VOID TSPDonePageList(PTSPPAGESET PageArr)
{
	ULONG i;
//!check	CheckDescriptors(PageArr,1);
	for(i=0;PageArr[i].Desc!=0;i++)
	{
		DonePageDir((ULONG*)PageArr[i].Desc);
		PageArr[i].Desc = 0;
	}
}

BOOLEAN TSPMakePageList(PTSPPAGESET PageArr,PCHAR Va,ULONG Size)
{
	ULONG i;
	ULONGLONG* PdEx;
	ULONG *Pd;
	for(i=0;i<Size/PAGE_SIZE;i++)
	{
		PageArr[i].VA=Va;
		PdEx = GetPageDescriptor(Va);
		if(PdEx!= (ULONGLONG*)BADP2LMAPPING)
		{
			Pd = (ULONG*) PdEx;
			PageArr[i].Desc = PdEx;
			__asm { //Set usermode bit
				mov eax,[Pd];
				lock or dword ptr [eax],4//4-Owner,
				lock and dword ptr [eax],0xFFFFFEE7// 100h-Global 10h-CacheDisable,8-PageWriteThrough
//				mov eax,[Va];
//				invlpg [eax];
			}
		}
		else
		{
			PageArr[i].Desc = 0;
			TSPDonePageList(PageArr);
			DbPrint(DC_TSP,DL_ERROR, ("TSPMakePageList failed. GetPageDescriptor return BADP2LMAPPING\n"));
			return FALSE;
		}
		Va+=PAGE_SIZE;
	}
	PageArr[i].VA=0;
	PageArr[i].Desc = 0;
	return TRUE;
}

PVOID TSPAllocRegion(ULONG Size)
{
PVOID ret;
#ifdef _HOOK_NT_
	ret=MmAllocateNonCachedMemory(Size);
#else //VXD
	ret=PageReserve(PR_PRIVATE,Size/PAGE_SIZE,PR_FIXED);
	if(ret) {
		if(!PageCommit((ULONG)ret>>0xc,Size/PAGE_SIZE,/*PD_NOINIT*/PD_FIXED,0,PC_FIXED | PC_USER | PC_WRITEABLE)) {
			PageFree((MEMHANDLE)ret,0);
			ret=NULL;
		}
	} 
#endif 
	return ret;
}

#ifdef _HOOK_NT_
//#define TSPFreeRegion(Addr) ExFreePool(Addr)
#define TSPFreeRegion(Addr,Size) MmFreeNonCachedMemory(Addr,Size)
#else
#define TSPFreeRegion(Addr,Size) PageFree((MEMHANDLE)Addr,0)
#endif //_HOOK_NT_


PTSP_SHAD TSPAllocShadow(PTSP_REG RgEntry,PVOID OwnTh,BOOLEAN *New)
{
PVOID ShadVa;
PTSP_SHAD ShEntry;
	*New=FALSE;
	if(OwnTh) {
		ExAcquireFastMutex(&RgEntry->ShadMutex);
		ShEntry=RgEntry->ShadList;
		while(ShEntry) {
			if(ShEntry->OwnerTh==NULL) {
				ShEntry->OwnerTh=OwnTh;
				RgEntry->CurShadsInUse++;
				DbPrint(DC_TSP,DL_INFO, ("TSPAllocShadow reuse old for th %x Num %d InUse %d\n",OwnTh,RgEntry->NumShads,RgEntry->CurShadsInUse));

				if (RgEntry->CurShadsInUse >= gMinShadow)
					gbWasReuse = TRUE;

				ExReleaseFastMutex(&RgEntry->ShadMutex);
				return ShEntry;
			}
			ShEntry=ShEntry->Next;
		}
		ExReleaseFastMutex(&RgEntry->ShadMutex);
	}
	ShadVa=TSPAllocRegion(RgEntry->hOrigCommitedSize);
	if(!ShadVa) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPAllocShadow fail on alloc sh\n"));
		return NULL;
	}
	ShEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSP_SHAD)+sizeof(TSPPAGESET)*(RgEntry->hOrigCommitedSize/PAGE_SIZE + 1),'tSeB');
	if(ShEntry) {
		if(TSPMakePageList(&ShEntry->Pages[0],ShadVa,RgEntry->hOrigCommitedSize)) {
			ExAcquireFastMutex(&RgEntry->ShadMutex);
			ShEntry->OwnerTh=OwnTh;
			ShEntry->ShadVa=ShadVa;
			ShEntry->Next=RgEntry->ShadList;
			RgEntry->ShadList=ShEntry;
			RgEntry->NumShads++;
			if(OwnTh)
				RgEntry->CurShadsInUse++;
			DbPrint(DC_TSP,DL_INFO, ("TSPAllocShadow alloc new for th %x Num %d InUse %d\n",OwnTh,RgEntry->NumShads,RgEntry->CurShadsInUse));
			gbWasReuse = TRUE;
			ExReleaseFastMutex(&RgEntry->ShadMutex);
			*New=TRUE;
			return ShEntry;
		} else {
			ExFreePool(ShEntry);
		}
	}
	TSPFreeRegion(ShadVa,RgEntry->hOrigCommitedSize);
	DbPrint(DC_TSP,DL_ERROR, ("TSPAllocShadow fail\n"));
	return NULL;
}

VOID TSPFreeShadow(PTSP_REG RgEntry, PVOID OwnerTh)
{
PTSP_SHAD ShEntry;
PTSP_SHAD ShPrev=NULL;
	ExAcquireFastMutex(&RgEntry->ShadMutex);
	ShEntry=RgEntry->ShadList;
	while(ShEntry) {
		if(ShEntry->OwnerTh==OwnerTh)
		{
			BOOLEAN bAllowFree = TRUE;
			LONG CurrShadInUse = 0;
			LONG min_shadows = RgEntry->MinNumShads > gMinShadow ? RgEntry->MinNumShads : gMinShadow;
			RgEntry->CurShadsInUse--;
			CurrShadInUse = RgEntry->NumShads - RgEntry->CurShadsInUse;
			if (CurrShadInUse <= min_shadows)
				bAllowFree = FALSE;

			if (bAllowFree)
			{
				if (gbWasReuse || !gbAllowFreeShadow)
				{
					DbPrint(DC_TSP, DL_SPAM, ("TSP disable free shadow (cur in use: %d, min %d)\n", RgEntry->CurShadsInUse, min_shadows));
					bAllowFree = FALSE;
				}
			}

			if(!bAllowFree)
			{
				ShEntry->OwnerTh=NULL;
				DbPrint(DC_TSP,DL_INFO, ("TSPFreeShadow release th %x Num %d InUse %d\n",OwnerTh,RgEntry->NumShads,RgEntry->CurShadsInUse));
				ExReleaseFastMutex(&RgEntry->ShadMutex);
				return;
			}
			DbPrint(DC_TSP, DL_SPAM, ("TSP destroy shadow\n"));
			gbAllowFreeShadow = FALSE;
			if(ShPrev)
				ShPrev->Next=ShEntry->Next;
			else
				RgEntry->ShadList=ShEntry->Next;
			RgEntry->NumShads--;
			DbPrint(DC_TSP,DL_INFO, ("TSPFreeShadow free th %x Num %d InUse %d\n",OwnerTh,RgEntry->NumShads,RgEntry->CurShadsInUse));
			ExReleaseFastMutex(&RgEntry->ShadMutex);
			TSPDonePageList(&ShEntry->Pages[0]);
			TSPFreeRegion(ShEntry->ShadVa,RgEntry->hOrigCommitedSize);
			ExFreePool(ShEntry);
			return;
		}
		ShPrev=ShEntry;
		ShEntry=ShEntry->Next;
	}
	ExReleaseFastMutex(&RgEntry->ShadMutex);
	DbPrint(DC_TSP,DL_ERROR, ("TSPFreeShadow OwnerTh %x not found\n",OwnerTh));
}

VOID TSPFreeAllShadows(PTSP_REG RgEntry)
{
PTSP_SHAD ShEntry;
PTSP_SHAD Tmp;
	ExAcquireFastMutex(&RgEntry->ShadMutex);
	DbPrint(DC_TSP,DL_INFO, ("TSPFreeAllShadows hOrig %x\n",RgEntry->hOrig));
	ShEntry=RgEntry->ShadList;
	while(ShEntry) {
		Tmp=ShEntry->Next;
		TSPDonePageList(&ShEntry->Pages[0]);
		TSPFreeRegion(ShEntry->ShadVa,RgEntry->hOrigCommitedSize);
		ExFreePool(ShEntry);
		ShEntry=Tmp;
		RgEntry->NumShads--;
	}
	RgEntry->ShadList=NULL;
	ExReleaseFastMutex(&RgEntry->ShadMutex);
}

PVOID TSPMakeMyPageDir(PVOID VAddr)
{
	PVOID MyDirVA;
	ULONGLONG* MyDirDscEx;
	ULONG* MyDirDsc;
	ULONG* PDE;
	ULONG* PTE;
	ULONGLONG* PDEEx;
	ULONGLONG* PTEEx;
	MyDirVA=TSPAllocRegion(PAGE_SIZE);

	if(MyDirVA)
	{
		memset(MyDirVA,0,PAGE_SIZE);
		
		MyDirDscEx = GetPageDescriptor(MyDirVA);
		MyDirDsc = (ULONG*) MyDirDscEx;
		if(MyDirDsc==BADP2LMAPPING)
		{
			DbPrint(DC_TSP,DL_ERROR, ("TSPMakeMyPageDir MyDirDsc==BADP2LMAPPING\n"));
			DbgBreakPoint();
		}
		else
		{
			PDEEx = GetPDEEntryNativeLite(VAddr);
			PTEEx = GetPageDescriptorNativeLite(VAddr);
			PDE = (ULONG*) PDEEx;
			PTE = (ULONG*) PTEEx;

#ifdef __DBG__
			if(*PDE != 0)
			{
				DbPrint(DC_TSP,DL_ERROR, ("TSPMakeMyPageDir PDir for Va %x already %x\n",VAddr,*PDE));
				DbgBreakPoint();
			}
#endif
			if (gdwPAE)
			{
				DbPrint(DC_TSP, DL_IMPORTANT, ("TSPMakeMyPageDir before MyDirDsc %#x->%I64x. PDE %#x->%I64x\n",
					MyDirDscEx, *MyDirDscEx, PDEEx, *PDEEx));
				__asm {
					mov ecx,[PDE];
					add ecx, size ULONG
					mov eax,[MyDirDsc];
					add eax, size ULONG
					mov eax,[eax]
					lock xchg eax,[ecx];


					mov ecx,[PDE];
					mov eax,[MyDirDsc];
					lock or dword ptr [eax],4//4-Owner,
					lock and dword ptr [eax],0xFFFFFEE7;// 100h-Global 10h-CacheDisable,8-PageWriteThrough
					mov eax,[eax]
					lock xchg eax,[ecx];
					mov eax,[PTE];
					invlpg [eax];
				}
				
				DbPrint(DC_TSP, DL_IMPORTANT, ("TSPMakeMyPageDir after MyDirDsc %#x->%I64x. PDE %#x->%I64x\n",
					MyDirDscEx, *MyDirDscEx, PDEEx, *PDEEx));
			}
			else
			{
				DbPrint(DC_TSP, DL_IMPORTANT, ("TSPMakeMyPageDir before MyDirDsc %#x->%#x. PDE %#x->%#x\n", 
					MyDirDsc, *MyDirDsc, PDE, *PDE));
				__asm {
					mov ecx,[PDE];
					mov eax,[MyDirDsc];
					lock or dword ptr [eax],4//4-Owner,
					lock and dword ptr [eax],0xFFFFFEE7;// 100h-Global 10h-CacheDisable,8-PageWriteThrough
					mov eax,[eax]
					lock xchg eax,[ecx];
					mov eax,[PTE];
					invlpg [eax];
				}
				
				DbPrint(DC_TSP, DL_IMPORTANT, ("TSPMakeMyPageDir after MyDirDsc %#x->%#x. PDE %#x->%#x\n", 
					MyDirDsc, *MyDirDsc, PDE, *PDE));
			}

			DonePageDir((ULONG*)MyDirDscEx);
			return MyDirVA;
		}
		TSPFreeRegion(MyDirVA, PAGE_SIZE);
	}
	DbPrint(DC_TSP,DL_ERROR, ("TSPMakeMyPageDir fail\n"));
	return NULL;
}


VOID TSPDoneMyPageDir(PVOID VAddr,PVOID MyDirVA)
{
	ULONG *PDE;
	ULONG *PTE;
	ULONGLONG* PDEEx;
	ULONGLONG* PTEEx;
	
	PDEEx = GetPDEEntryNativeLite(VAddr);
	PTEEx = GetPageDescriptorNativeLite(VAddr);
	
	PDE = (ULONG*) PDEEx;
	PTE = (ULONG*) PTEEx;
	__asm {
		mov ecx,[PDE];
		xor eax,eax;
		lock xchg eax,[ecx];

		mov eax, [gdwPAE];
		or eax, eax;
		jz donedir_nopae;
		
		add ecx, size ULONG
		xor eax,eax;
		lock xchg eax,[ecx];
donedir_nopae:
		
		mov eax,[PTE];
		invlpg [eax];
	}
	TSPFreeRegion(MyDirVA,PAGE_SIZE);
}

BOOLEAN TSPReCommitOrig(PTSP_REG RgEntry ,ULONG Size)
{
	ORIG_HANDLE hOrig = RgEntry ->hOrig;
#ifdef _HOOK_NT_
PVOID Addr;
ULONG RegionSize;
	Addr=hOrig;
	RegionSize=Size;

	DbgBreakPoint();
	if(ZwAllocateVirtualMemory(NtCurrentProcess(),&Addr,0,&RegionSize,MEM_COMMIT,PAGE_READWRITE)==STATUS_SUCCESS)
	{
		RgEntry->hOrigCommitedSize = Size;
		return TRUE;
	}
#else 
	if(PageCommit((ULONG)hOrig>>0xc,Size/PAGE_SIZE,/*PD_NOINIT*/PD_FIXED,0,PC_FIXED | PC_USER | PC_WRITEABLE))
	{
		RgEntry->hOrigCommitedSize = Size;
		return TRUE;
	}
#endif
	DbPrint(DC_TSP,DL_WARNING, ("TSPReCommitOrig fail Addr %x Sixe %x\n",hOrig,Size));
	return FALSE;
}

ORIG_HANDLE TSPAllocOrig(ULONG Size,PVOID *OPD)
{
#ifdef _HOOK_NT_
	ULONG RegionSize,a,s;
	PVOID Addr;
	PVOID MyOrigPageDir;
		s=TSP4Mb;
		for(a=TSP4Mb*4;a<0x80000000;a+=s)
		{
			Addr=(PVOID)a;
			RegionSize=s;
			if(ZwAllocateVirtualMemory(NtCurrentProcess(),&Addr,0,&RegionSize,MEM_RESERVE,/*PAGE_NO_ACCESS*/PAGE_READWRITE)==STATUS_SUCCESS)
			{
				MyOrigPageDir = TSPMakeMyPageDir(Addr);
				if(MyOrigPageDir)
				{
					RegionSize=Size;
					if(ZwAllocateVirtualMemory(NtCurrentProcess(),&Addr,0,&RegionSize,MEM_COMMIT,PAGE_READWRITE)==STATUS_SUCCESS)
					{
						*OPD=MyOrigPageDir;
						return Addr;
					}
					else
					{
						DbPrint(DC_TSP,DL_WARNING, ("TSPAllocOrig fail on commit Addr %x Sixe %x\n",Addr,RegionSize));
						TSPDoneMyPageDir(Addr,MyOrigPageDir);
					}
				}

				DbPrint(DC_TSP,DL_WARNING, ("check mem release\n"));
				DbgBreakPoint();
				RegionSize=0;
				_pfZwFreeVirtualMemory(NtCurrentProcess(),&Addr,&RegionSize,MEM_RELEASE);
			}
		}
		
		DbPrint(DC_TSP,DL_ERROR, ("TSPAllocOrig fail\n"));
		Addr=NULL;
#else // VXD
	PVOID Addr=NULL;
	ULONG SizeInPages;
		SizeInPages=Size/PAGE_SIZE;
	Addr=PageReserve(PR_PRIVATE,SizeInPages,PR_FIXED);
	if(Addr) {
		if(PageCommit((ULONG)Addr>>0xc,SizeInPages,/*PD_NOINIT*/PD_FIXED,0,PC_FIXED | PC_USER | PC_WRITEABLE)) {
			return Addr;
		} else {
			DbPrint(DC_TSP,DL_ERROR, ("TSPAllocOrig fail on commit\n"));
			PageFree((MEMHANDLE)Addr,0);
			Addr=NULL;
		}
	} else {
		DbPrint(DC_TSP,DL_ERROR, ("TSPAllocOrig fail on reserve\n"));
	}
#endif //_HOOK_NT_
	return Addr;
}

//MmMapUserAddressesToPage
//MiMapPageInHyperSpace for multiproc...
NTSTATUS TSPChangeOrigAndMakePList(PTSP_REG	RgEntry,ULONG NewSize)
{
ULONG RegionSize;
PVOID RegAddr;
NTSTATUS NtStatus=STATUS_UNSUCCESSFUL;

	RgEntry->OrigPageLst=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSPPAGESET)*(NewSize/PAGE_SIZE + 1),'tSeB');
	if(RgEntry->OrigPageLst) {
		if(TSPMakePageList(RgEntry->OrigPageLst,RgEntry->hOrig,NewSize)) {
#ifdef _HOOK_NT_
			RgEntry->pOrig=TSPAllocRegion(NewSize);
			if(RgEntry->pOrig) {
				PTSPPAGESET pOPL;
				memcpy(RgEntry->pOrig,RgEntry->hOrig,NewSize);
				pOPL=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSPPAGESET)*(NewSize/PAGE_SIZE + 1),'tSeB');
				if(pOPL) {
					if(TSPMakePageList(pOPL,RgEntry->pOrig,NewSize)) {
						RegAddr=RgEntry->hOrig;
						RegionSize = 0;//! was: RegionSize=RgEntry->Size;
						NtStatus = _pfZwFreeVirtualMemory(NtCurrentProcess(),&RegAddr,&RegionSize,MEM_DECOMMIT);
						if(NT_SUCCESS(NtStatus)) {
							ULONG	OldIrql;
							RgEntry->hOrigCommitedSize=NewSize;
							AcquireTSP(&OldIrql);
							RgEntry->DecomitDescr=TSPCopyPgDesc(RgEntry->OrigPageLst,pOPL);
							ReleaseTSP(OldIrql);
							//RgEntry->Size=NewSize;
							TSPDonePageList(pOPL);
							ExFreePool(pOPL);
							return STATUS_SUCCESS;
						} else {
							DbPrint(DC_TSP,DL_ERROR, ("TSPChangeOrigAndMakePList ZwFreeVirtualMemory fail NtStatus %d\n",NtStatus));
							DbgBreakPoint();
						}
						TSPDonePageList(pOPL);
					} else {
						DbPrint(DC_TSP,DL_ERROR, ("TSPChangeOrigAndMakePList hOrig %x fail on make pOPL\n",RgEntry->hOrig));
					}
					ExFreePool(pOPL);
				} else {
					DbPrint(DC_TSP,DL_WARNING, ("TSPChangeOrigAndMakePList alloc pOPL fail\n"));
					NtStatus=STATUS_NO_MEMORY;
				}
				TSPFreeRegion(RgEntry->pOrig,NewSize);
			} else {
				DbPrint(DC_TSP,DL_WARNING, ("TSPChangeOrigAndMakePList alloc new Orig fail\n"));
				NtStatus=STATUS_NO_MEMORY;
			}
#else // VXD
			RegAddr=(PCHAR)RgEntry->hOrig+NewSize;
			RegionSize=RgEntry->ReservedSize-NewSize;
			if(RegionSize==0 || PageDecommit((ULONG)RegAddr>>0xc,RegionSize/PAGE_SIZE,0)) {
				RgEntry->hOrigCommitedSize=NewSize;
				RgEntry->pOrig=RgEntry->hOrig;
				return STATUS_SUCCESS;
			} else {
				DbPrint(DC_TSP,DL_WARNING, ("TSPChangeOrigAndMakePList PageDecommit fail\n"));
				DbgBreakPoint();
			}
#endif //_HOOK_NT_
			TSPDonePageList(RgEntry->OrigPageLst);
		} else {
			DbPrint(DC_TSP,DL_ERROR, ("TSPChangeOrigAndMakePList hOrig %x fail on MakeOrigPagesList\n",RgEntry->hOrig));
		}
		ExFreePool(RgEntry->OrigPageLst);
	} else {
		DbPrint(DC_TSP,DL_WARNING, ("TSPChangeOrigAndMakePList alloc OrigPageLst fail\n"));
		NtStatus=STATUS_NO_MEMORY;
	}
	RgEntry->OrigPageLst=NULL;
	RgEntry->pOrig=NULL;
	return NtStatus;
}

VOID TSPFreeOrig(PTSP_REG	RgEntry)
{
PVOID Addr;
	if(RgEntry->OrigPageLst) {
#ifdef _HOOK_NT_
		{
			ULONG			OldIrql;
			/*AcquireTSP(&OldIrql);
			TSPSetPgDesc(RgEntry->OrigPageLst,RgEntry->DecomitDescr);
			ReleaseTSP(OldIrql);*/
//! check			CheckDescriptors(RgEntry->OrigPageLst,1);
			TSPSetPgDescNative(RgEntry->OrigPageLst, RgEntry->DecomitDescr);
			if(RgEntry->pOrig) {
				TSPFreeRegion(RgEntry->pOrig,RgEntry->hOrigCommitedSize);
				RgEntry->pOrig=NULL;
			}
		}
#endif //_HOOK_NT_
		TSPDonePageList(RgEntry->OrigPageLst);
		ExFreePool(RgEntry->OrigPageLst);
		RgEntry->OrigPageLst=NULL;
	}
	if(RgEntry->hOrigCommitedSize && RgEntry->hOrig) {
		Addr=RgEntry->hOrig;
#ifdef _HOOK_NT_
		{
			ULONG RegionSize;
			RegionSize=0;
			_pfZwFreeVirtualMemory(NtCurrentProcess(),&Addr,&RegionSize,MEM_DECOMMIT);
		}
#else
		PageDecommit((ULONG)Addr>>0xc,RgEntry->hOrigCommitedSize/PAGE_SIZE,0);
#endif //_HOOK_NT_
		RgEntry->hOrigCommitedSize=0;
		RgEntry->pOrig = NULL;
	}
	RgEntry->bOwned=FALSE;
}


PTSP_REG	TSPGetRg(ORIG_HANDLE hOrig,PTSP_PROC *PrE, ULONG maxSize)
// if(PrE) кладет туда ProcEntry для текущего процесса
// if(hOrig) возвращает RegEntry для этого hOrig
// в противном случае возвращает первый свободный RegEntry
{
	PTSP_PROC	PrEntry;
	PTSP_REG	RgEntry;
	PVOID			CurProcId;
	CurProcId=PsGetCurrentProcessId();
	if(PrE)
		*PrE=NULL;
	PrEntry=TSPPrRoot;
	while(PrEntry) {
		if(PrEntry->OwnerProcId==CurProcId) {
			if(PrE)
				*PrE=PrEntry;
			RgEntry=PrEntry->RegList;
			if(hOrig) {
				while(RgEntry) {
					if(RgEntry->hOrig==hOrig)
						return RgEntry;
					RgEntry=RgEntry->Next;
				}
			} else { //hOrig==NULL
				while(RgEntry) {
					if(!RgEntry->bOwned)
					{
						if (maxSize == RgEntry->ReservedSize)
							return RgEntry;
					}
					RgEntry=RgEntry->Next;
				}
			}
			break;
		}
		PrEntry=PrEntry->Next;
	}
	return NULL;
}


#ifdef _HOOK_NT_
ULONG *ProcessorUsageArr; // Указатель на массив заюзанных процов

ULONG TSPSelectProcessor(VOID)
{
int i;
ULONG MinVal;
ULONG MinIdx;
	MinIdx=*KeNumberProcessors-1;
	MinVal=ProcessorUsageArr[MinIdx];
	for(i=MinIdx;i>=0;i--){
		if(ProcessorUsageArr[i]<MinVal) {
			MinVal=ProcessorUsageArr[i];
			MinIdx=i;
		}
	}
	InterlockedIncrement(ProcessorUsageArr+MinIdx);
	return MinIdx;
}

VOID TSPUnselectProcessor(ULONG ProcNum)
{
	if((ULONG)(*KeNumberProcessors) > ProcNum)
		InterlockedDecrement(ProcessorUsageArr+ProcNum);
}
#endif

NTSTATUS TSPRegister(ULONG maxSize,ORIG_HANDLE *phOrig)
{
PTSP_PROC	PrEntry;
PTSP_REG	RgEntry;
	*phOrig=NULL;
	if(!TSPInitedOk) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPRegister fail\n"));
		return STATUS_UNSUCCESSFUL;
	}
	if(maxSize % PAGE_SIZE)
		maxSize+=PAGE_SIZE - (maxSize % PAGE_SIZE);
	if(maxSize>TSP4Mb) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPRegister fail, maxSize>4Mb\n"));
		return STATUS_INVALID_PARAMETER;
	}
	ExAcquireFastMutex(&TSPPrMutex);
	RgEntry=TSPGetRg(NULL,&PrEntry, maxSize);
	if(PrEntry==NULL) {
		PrEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSP_PROC),'tSeB');
		if(PrEntry) {
			PrEntry->OwnerProc=PsGetCurrentProcess();
			PrEntry->OwnerProcId=PsGetCurrentProcessId();
#ifdef _HOOK_NT_
			if(*KeNumberProcessors>1) {
				PrEntry->WorkingProcessor=TSPSelectProcessor();
			}
#endif
			PrEntry->RegList=NULL;
			PrEntry->Next=TSPPrRoot;
			TSPPrRoot=PrEntry;
		} else {
			ExReleaseFastMutex(&TSPPrMutex);
			DbPrint(DC_TSP,DL_ERROR, ("TSPRegister fail\n"));
			return STATUS_NO_MEMORY;
		}
	}
	if(!RgEntry) {
		RgEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSP_REG),'tSeB');
		if(!RgEntry) {
			ExReleaseFastMutex(&TSPPrMutex);
			DbPrint(DC_TSP,DL_ERROR, ("TSPRegister fail\n"));
			return STATUS_NO_MEMORY;
		}
		memset(RgEntry,0,sizeof(TSP_REG));
		RgEntry->hOrig=TSPAllocOrig(maxSize,&RgEntry->OrigPageDir);
		if(RgEntry->hOrig) {
			RgEntry->Next=PrEntry->RegList;
			PrEntry->RegList=RgEntry;
			ExInitializeFastMutex(&RgEntry->ShadMutex);
			DbPrint(DC_TSP,DL_NOTIFY, ("TSPRegister new hOrig=%x maxSize=%d\n ProcId=%d",RgEntry->hOrig,maxSize,PrEntry->OwnerProcId));
		} else {
			ExFreePool(RgEntry);
			ExReleaseFastMutex(&TSPPrMutex);
			DbPrint(DC_TSP,DL_ERROR, ("TSPRegister fail\n"));
			return STATUS_NO_MEMORY;
		}
	} else {
		if(!TSPReCommitOrig(RgEntry,maxSize)) {
			ExReleaseFastMutex(&TSPPrMutex);
			DbPrint(DC_TSP,DL_ERROR, ("TSPRegister fail\n"));
			return STATUS_NO_MEMORY;
		} 
		RgEntry->MinNumShads=0;
		RgEntry->NumShads=0;
		RgEntry->CurShadsInUse=0;
		DbPrint(DC_TSP,DL_NOTIFY, ("TSPRegister old hOrig=%x maxSize=%d\n ProcId=%d",RgEntry->hOrig,maxSize,PrEntry->OwnerProcId));
	}
	RgEntry->hOrigCommitedSize=maxSize;
	RgEntry->ReservedSize=maxSize;
	RgEntry->bOwned=TRUE;
	*phOrig=RgEntry->hOrig;
	ExReleaseFastMutex(&TSPPrMutex);
	return STATUS_SUCCESS;
}

NTSTATUS TSPUnregister(ORIG_HANDLE hOrig)
{
PTSP_PROC	PrEntry;
PTSP_REG	RgEntry;
	if(!TSPInitedOk)
		return STATUS_UNSUCCESSFUL;
	ExAcquireFastMutex(&TSPPrMutex);
	RgEntry=TSPGetRg(hOrig,&PrEntry, 0);
	if(!RgEntry) {
		ExReleaseFastMutex(&TSPPrMutex);
		DbPrint(DC_TSP,DL_ERROR, ("TSPUnregister hOrig %x\n",hOrig));
		return STATUS_NOT_FOUND;
	}
	if(RgEntry->CurShadsInUse) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPUnregister hOrig %x ProcId %d InUse %d\n",hOrig,PrEntry->OwnerProcId,RgEntry->CurShadsInUse));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_UNSUCCESSFUL;
	}
	TSPFreeAllShadows(RgEntry);
	TSPFreeOrig(RgEntry);
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPUnregister hOrig %x ProcId %d\n",hOrig,PrEntry->OwnerProcId));
//	PrEntry->OwnerProcId = 0;
	ExReleaseFastMutex(&TSPPrMutex);
	
//	TSPProcCrash(0);

	return STATUS_SUCCESS;
}

VOID TSPProcCrash(PVOID	ProcId)
{
PTSP_PROC	PrEntry;
PTSP_PROC	PrPrev;
PTSP_REG	RgEntry;
PTSP_REG	RgTmp;
	if(!TSPInitedOk)
		return;
	ExAcquireFastMutex(&TSPPrMutex);
	PrPrev=NULL;
	PrEntry=TSPPrRoot;
	while(PrEntry){
		if(PrEntry->OwnerProcId==ProcId){
			if(PrPrev)
				PrPrev->Next=PrEntry->Next;
			else
				TSPPrRoot=PrEntry->Next;
#ifdef _HOOK_NT_
			if(*KeNumberProcessors>1) {
				TSPUnselectProcessor(PrEntry->WorkingProcessor);
			}
#endif
			RgEntry=PrEntry->RegList;
			while(RgEntry){
				RgTmp=RgEntry->Next;
				TSPFreeAllShadows(RgEntry);
				ExDestroyFastMutex(&RgEntry->ShadMutex);
				TSPFreeOrig(RgEntry);
#ifdef _HOOK_NT_
				if(RgEntry->OrigPageDir)
				{
					TSPDoneMyPageDir(RgEntry->hOrig,RgEntry->OrigPageDir);
				}
#endif
				ExFreePool(RgEntry);
				RgEntry=RgTmp;
			}
			ExFreePool(PrEntry);
			ExReleaseFastMutex(&TSPPrMutex);
			DbPrint(DC_TSP,DL_NOTIFY, ("TSPProcCrash ProcId %x\n",ProcId));
			return;
		}
		PrPrev=PrEntry;
		PrEntry=PrEntry->Next;
	}
	ExReleaseFastMutex(&TSPPrMutex);
}

NTSTATUS TSPLock(PTSPRS_LOCK pls)
{
PTSP_REG	RgEntry;
LONG			i;
BOOLEAN		Hax;
PTSP_SHAD ShEntry;
NTSTATUS	NtStatus;
	if(!TSPInitedOk)
		return STATUS_UNSUCCESSFUL;
	ExAcquireFastMutex(&TSPPrMutex);
	RgEntry=TSPGetRg(pls->hOrig,NULL, 0);
	if(!RgEntry) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPLock hOrig %x not found\n",pls->hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_NOT_FOUND;
	}
	if(RgEntry->pOrig) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPLock hOrig %x already locked!\n",pls->hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_UNSUCCESSFUL;
	}
	if(pls->real_size==0) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPLock hOrig %x Nothing to lock. realSize=0\n",pls->hOrig,RgEntry->ReservedSize,pls->real_size));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_UNSUCCESSFUL;
	}
	if(RgEntry->ReservedSize < pls->real_size) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPLock hOrig %x maxSize (%d) less then realSize (%d)\n",pls->hOrig,RgEntry->ReservedSize,pls->real_size));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_UNSUCCESSFUL;
	}
	if(pls->real_size % PAGE_SIZE)
		pls->real_size += PAGE_SIZE - pls->real_size % PAGE_SIZE;
	NtStatus=TSPChangeOrigAndMakePList(RgEntry,pls->real_size);
	if(!NT_SUCCESS(NtStatus)) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPLock hOrig %x fail on TSPChangeOrigAndMakePList\n",RgEntry->hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		return NtStatus;
	}
	{
		LONG min_shadow = pls->min_number_of_shadows;
		
		if (gMinShadow > min_shadow)
			min_shadow = gMinShadow;

		for(i=0; i < min_shadow;i++)
		{
			ShEntry=TSPAllocShadow(RgEntry,NULL,&Hax);
			if(ShEntry) {
				memcpy(ShEntry->ShadVa,RgEntry->hOrig,RgEntry->hOrigCommitedSize);
				RgEntry->MinNumShads++;
			}
		}
	}
	ExReleaseFastMutex(&TSPPrMutex);
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPLock hOrig %x\n",pls->hOrig));
	return STATUS_SUCCESS;
}

NTSTATUS TSPThreadEnter(ORIG_HANDLE hOrig)
{
PTSP_PROC PrEntry;
PTSP_REG	RgEntry;
ULONG			OldIrql;
PTSP_THRD	ThEntry;
PVOID			CurTh;
PTSP_RS		RS;
BOOLEAN		NewShad;

#ifdef _HOOK_NT_
ULONG			OldAffMsk;
#endif

	if(!TSPInitedOk) {
		DbPrint(DC_TSP,DL_ERROR, ("TSP is not inited\n"));
		return STATUS_UNSUCCESSFUL;
	}
	ExAcquireFastMutex(&TSPPrMutex);
	RgEntry=TSPGetRg(hOrig,&PrEntry, 0);
	if(!RgEntry) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPThreadEnter hOrig %x not found\n",hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_NOT_FOUND;
	}
	if(!RgEntry->pOrig) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPThreadEnter hOrig %x isn't locked!\n",hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		return STATUS_UNSUCCESSFUL;
	}		
	CurTh=(PVOID)PsGetCurrentThreadId();

	AcquireTSP(&OldIrql);
	ThEntry=TSPGetTh(CurTh);
	if(ThEntry) {
		RS=ThEntry->RS;
		while(RS) {
			if(RS->Rg==RgEntry) {
				ReleaseTSP(OldIrql);
				DbPrint(DC_TSP,DL_WARNING, ("TSPThreadEnter Th %x already entered hOrig %x\n",CurTh,hOrig));
				ExReleaseFastMutex(&TSPPrMutex);
				DbgBreakPoint();
				return STATUS_SUCCESS;
			}
			RS=RS->Next;
		}
	}
	ReleaseTSP(OldIrql);
#ifdef _HOOK_NT_
	if(*KeNumberProcessors>1)
		OldAffMsk=KeSetAffinityThread(PsGetCurrentThread(),1 << PrEntry->WorkingProcessor);
#endif
	RS=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSP_RS),'tSeB');
	if(RS) {
		RS->Next=NULL;
		RS->Rg=RgEntry;
		RS->Sh=TSPAllocShadow(RgEntry,CurTh,&NewShad);
		if(RS->Sh) {
#ifdef _HOOK_NT_
			if(NewShad)
				memcpy(RS->Sh->ShadVa,RgEntry->pOrig,RgEntry->hOrigCommitedSize);
#endif
			AcquireTSP(&OldIrql);
			ThEntry=TSPGetTh(CurTh);
			if(!ThEntry) {
				ReleaseTSP(OldIrql);
				ThEntry=ExAllocatePoolWithTag(NonPagedPool,sizeof(TSP_THRD),'tSeB');
				if(!ThEntry) {
#ifdef _HOOK_NT_
					if(*KeNumberProcessors>1)
						KeSetAffinityThread(PsGetCurrentThread(),OldAffMsk);
#endif
					TSPFreeShadow(RgEntry,CurTh);
					ExFreePool(RS);
					DbPrint(DC_TSP,DL_ERROR, ("TSPThreadEnter hOrig %x fail\n",hOrig));
					ExReleaseFastMutex(&TSPPrMutex);
					return STATUS_NO_MEMORY;
				}
				AcquireTSP(&OldIrql);
				ThEntry->Th=CurTh;
				ThEntry->Next=TSPThRoot;
				TSPThRoot=ThEntry;
			} else {
				RS->Next=ThEntry->RS;
			}
			ThEntry->RS=RS;
			TSPSwapInTh(CurTh);
//!!--->>
			ReleaseTSP(OldIrql);
#ifdef _HOOK_VXD_
			if(NewShad)
				memcpy(RgEntry->hOrig,RS->Sh->ShadVa,RgEntry->hOrigCommitedSize);
#endif
//!!		ReleaseTSP(OldIrql);
			DbPrint(DC_TSP,DL_NOTIFY, ("TSPThreadEnter hOrig %x\n",hOrig));
			ExReleaseFastMutex(&TSPPrMutex);
			return STATUS_SUCCESS;
		}
		ExFreePool(RS);
	}
#ifdef _HOOK_NT_
	if(*KeNumberProcessors>1)
		KeSetAffinityThread(PsGetCurrentThread(),OldAffMsk);
#endif
	DbPrint(DC_TSP,DL_ERROR, ("TSPThreadEnter hOrig %x fail\n",hOrig));
	ExReleaseFastMutex(&TSPPrMutex);
	return STATUS_NO_MEMORY;
}

NTSTATUS TSPThreadLeave(ORIG_HANDLE hOrig)
{
PTSP_REG	RgEntry;
PTSP_THRD	ThEntry;
PTSP_THRD	ThPrev;
PVOID			CurTh;
PTSP_RS		RS;
PTSP_RS		RSPrev;
ULONG			OldIrql;
	if(!TSPInitedOk) {
		DbPrint(DC_TSP,DL_ERROR, ("TSP is not inited\n"));
		return STATUS_UNSUCCESSFUL;
	}
	ExAcquireFastMutex(&TSPPrMutex);
	RgEntry=TSPGetRg(hOrig,NULL, 0);
	if(!RgEntry) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPThreadLeave hOrig %x not found\n",hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		DbgBreakPoint();
		return STATUS_NOT_FOUND;
	}
	if(!RgEntry->pOrig) {
		DbPrint(DC_TSP,DL_ERROR, ("TSPThreadLeave hOrig %x isn't locked!\n",hOrig));
		ExReleaseFastMutex(&TSPPrMutex);
		DbgBreakPoint();
		return STATUS_UNSUCCESSFUL;
	}		
	CurTh=(PVOID)PsGetCurrentThreadId();
	AcquireTSP(&OldIrql);
	ThEntry=TSPThRoot;
	ThPrev=NULL;
	while(ThEntry) {
		if(ThEntry->Th==CurTh)
			break;
		ThPrev=ThEntry;
		ThEntry=ThEntry->Next;
	}
	if(!ThEntry) {
		ReleaseTSP(OldIrql);
		DbPrint(DC_TSP,DL_ERROR, ("TSPThreadLeave Th %x not found\n",CurTh));
		ExReleaseFastMutex(&TSPPrMutex);
		DbgBreakPoint();
		return STATUS_NOT_FOUND;
	}
	TSPSwapOutTh(CurTh,RgEntry);
	RS=ThEntry->RS;
	RSPrev=NULL;
	while(RS) {
		if(RS->Rg==RgEntry) {
			if(RSPrev)
				RSPrev->Next=RS->Next;
			else
				ThEntry->RS=RS->Next;
			break;
		}
		RSPrev=RS;
		RS=RS->Next;
	}
	//Если это последний RS то можно грохнуть ThEntry
	if(ThEntry->RS) {
		ThEntry=NULL;
	} else {
		if(ThPrev)
			ThPrev->Next=ThEntry->Next;
		else
			TSPThRoot=ThEntry->Next;
	}
	ReleaseTSP(OldIrql);
	if(ThEntry) {
#ifdef _HOOK_NT_
		if(*KeNumberProcessors>1)
			KeSetAffinityThread(PsGetCurrentThread(),(1 << *KeNumberProcessors)-1);
#endif
		ExFreePool(ThEntry);
	}
	if(RS) {
		ExFreePool(RS);
	} else {
		DbPrint(DC_TSP,DL_ERROR, ("TSPThreadLeave RS for Th %x hOrig %x not found\n",CurTh,hOrig));
		DbgBreakPoint();
	}
	TSPFreeShadow(RgEntry,CurTh);
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPThreadLeave hOrig %x\n",hOrig));
	ExReleaseFastMutex(&TSPPrMutex);
	return STATUS_SUCCESS;
}

VOID TSPThreadCrash(PVOID ThId)
{
ULONG			OldIrql;
PTSP_THRD	ThEntry;
PTSP_THRD	ThPrev;
PTSP_RS		RS;
PTSP_RS		RStmp;
	if(!TSPInitedOk)
		return;
	ExAcquireFastMutex(&TSPPrMutex);
	AcquireTSP(&OldIrql);
	ThEntry=TSPThRoot;
	ThPrev=NULL;
	while(ThEntry) {
		if(ThEntry->Th==ThId)
			break;
		ThPrev=ThEntry;
		ThEntry=ThEntry->Next;
	}
	if(!ThEntry) {
		ReleaseTSP(OldIrql);
		ExReleaseFastMutex(&TSPPrMutex);
		return;
	}
	TSPSwapOutTh(ThId,NULL);
	if(ThPrev)
		ThPrev->Next=ThEntry->Next;
	else
		TSPThRoot=ThEntry->Next;
	ReleaseTSP(OldIrql);
	RS=ThEntry->RS;
	while(RS) {
		if(RS->Rg) {
			TSPFreeShadow(RS->Rg,ThId);
			DbPrint(DC_TSP,DL_NOTIFY, ("TSPThreadCrash Th %x hOrig %x\n",ThId,RS->Rg->hOrig));
		}
		RStmp=RS;
		RS=RS->Next;
		ExFreePool(RStmp);
	}
	ExFreePool(ThEntry);
	ExReleaseFastMutex(&TSPPrMutex);
	//	DbPrint(DC_TSP,DL_NOTIFY, ("TSPThreadCrash Th %x \n",ThId));
}


#ifdef _HOOK_NT_
NTSTATUS (__stdcall *NtSetInformationProcess)(HANDLE ProcessHandle,ULONG ProcessInformationClass,PVOID ProcessInformation,ULONG ProcessInformationLength);

NTSTATUS __stdcall HNtSetInformationProcess(HANDLE ProcessHandle,ULONG ProcessInformationClass,PVOID ProcessInformation,ULONG ProcessInformationLength)
{
NTSTATUS	NtStatus;
PEPROCESS ProcObj;
ULONG			oldirql;
PTSP_PROC	PrEntry;
//	DbPrint(DC_TSP,DL_INFO, ("NtSetInformationProcess ProcessHandle=%x ProcessInformationClass=%x\n",ProcessHandle,ProcessInformationClass));
	if(ProcessInformationClass==0x15){
		if(ProcessHandle!=NtCurrentProcess() && TSPInitedOk){
			NtStatus=ObReferenceObjectByHandle(ProcessHandle,0,NULL,KernelMode, (PVOID*) &ProcObj,NULL);
			if(NT_SUCCESS(NtStatus)) {
				ExAcquireFastMutex(&TSPPrMutex);
				PrEntry=TSPPrRoot;;
				while(PrEntry && PrEntry->OwnerProc!=ProcObj) {
					PrEntry=PrEntry->Next;
				}
				ExReleaseFastMutex(&TSPPrMutex);
				ObDereferenceObject(ProcObj);
				if(PrEntry) {
					DbPrint(DC_TSP,DL_IMPORTANT, ("Attempt to set affinity for my client (%x) detected. ACCESS_DENIED\n",ProcObj));
					return STATUS_ACCESS_DENIED;
				}
			} else {
				DbPrint(DC_TSP,DL_ERROR, ("ObReferenceProcessObjectByHandle fail. ProcessHandle=%x NtStatus=%x\n",ProcessHandle,NtStatus));
			}
		}
	}
	return NtSetInformationProcess(ProcessHandle,ProcessInformationClass,ProcessInformation,ProcessInformationLength);
}

// located in "INIT" section, look for #pragma alloc_text("INIT",...)
BOOLEAN HookSetProcessAffinity(VOID)
{
ULONG	NativeID;
BOOLEAN		OldProtect;
	if(NativeID=GetNativeID(BaseOfNtDllDll,"NtSetInformationProcess")) {
		NtSetInformationProcess=(long (__stdcall *)(void *,unsigned long ,void *,unsigned long))SYSTEMSERVICE_BY_FUNC_ID(NativeID);
		if(MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase+NativeID,TRUE,&OldProtect)) {
			SYSTEMSERVICE_BY_FUNC_ID(NativeID)=(ULONG)HNtSetInformationProcess;
			MyVirtualProtect(KeServiceDescriptorTable.ServiceTableBase+NativeID,OldProtect,&OldProtect);
			DbPrint(DC_TSP,DL_NOTIFY, ("NtSetInformationProcess hooked\n"));
			return TRUE;
		}
	} else {
		DbPrint(DC_TSP,DL_ERROR, ("ID for NtSetInformationProcess not found\n"));
	}
	return FALSE;
}

DWORD ThreadSwitchCallbackPatchRet;

ULONG PatchedCodeSC[]={0x90909090,0x90909090,0x25ff9090,(ULONG)&ThreadSwitchCallbackPatchRet};
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
	//	DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %c%08x ->%c%08x\n",TSPIsMyThread(OldThreadId)?'!':' ',OldThreadId,TSPIsMyThread(NewThreadId)?'!':' ',NewThreadId));
	TSPSwapInTh((PVOID)NewThreadId);
	//	TSPMarkForSwap((PVOID)OldThreadId,(PVOID)NewThreadId);
	ReleaseTSP(oldirql);
	__asm {
		add esp,TSCVariablesSize;
		popad;
		popfd;
		jmp [PatchedCodeSCF];
	}
}

// located in "INIT" section, look for #pragma alloc_text("INIT",...)
BOOLEAN PatchSwapC(VOID)
{
DWORD SSPos;
static BYTE mov_byte_ptr_es_esi_2Dh_2[]={0x26,0xC6,0x46,0x2D,0x02}; //mov byte ptr es:[esi+2Dh], 2
static BYTE mov_ecx_pebx_cmp_pebx_plusXXXXXXXX_0[]={0x8B, 0x0B, 0x83, 0xBB};//mov ecx, [ebx];cmp dword ptr [ebx+XXXXXXXX], 0
static BYTE test_dword_ptr_esi_24h_1[]={0xF7,0x46,0x24,0x01,0x00,0x00,0x00};//test dword ptr [esi+24h], 1
static BYTE cmp_byte_ptr_esi_5Dh_0[]={0x80,0x7E,0x5D,0x00,0x74,0x04,0xf3,0x90};// cmp byte ptr [esi+5Dh], 0;jz @+4;pause
static BYTE build3790[]={0xff,0x43,0x10,0xff,0x33,0x83,0x7b,0x08,0x00};

BYTE			PatchData[10];
ULONG			PatchDataSize;
ULONG			oldirql;
BOOLEAN		OldProtect;
PVOID			ThreadSwitchCallbackPatchAddr;
PIMAGE_DOS_HEADER pDosHeader;
PIMAGE_NT_HEADERS pNTHeader;
PIMAGE_SECTION_HEADER SectHeader;
PVOID FirstSectStart;
ULONG FirstSectSize;
	pDosHeader=(PIMAGE_DOS_HEADER)BaseOfNtOsKrnl;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		DbPrint(DC_TSP,DL_ERROR, ("mz header not found\n"));
		return FALSE;
	}
	pNTHeader=MakePtr(PIMAGE_NT_HEADERS,BaseOfNtOsKrnl,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
		DbPrint(DC_TSP,DL_ERROR, ("PE header not found\n"));
		return FALSE;
	}
	SectHeader=IMAGE_FIRST_SECTION(pNTHeader);
	FirstSectStart=(BYTE*)BaseOfNtOsKrnl + SectHeader->VirtualAddress;
	FirstSectSize=SectHeader->SizeOfRawData;
	
	ThreadSwitchCallbackPatchAddr=NULL;
	try {
		if(*NtBuildNumber>2600) {//.NET 
			BYTE *SearchStr;
			ULONG SearchLen;
			ULONG appendix;
			PatchDataSize=5;
			if(*NtBuildNumber>=3790) {//2003 server
				SearchStr=build3790;
				SearchLen=sizeof(build3790);
				appendix=0;
			} else if(*NtBuildNumber>3604) {//.NET 3663 and more
				SearchStr=cmp_byte_ptr_esi_5Dh_0;
				SearchLen=sizeof(cmp_byte_ptr_esi_5Dh_0);
				appendix=10;
			} else {//.NET 3604
				SearchStr=test_dword_ptr_esi_24h_1;
				SearchLen=sizeof(test_dword_ptr_esi_24h_1);
				appendix=9;
			}
			if((SSPos=GetSubstringPosBM((BYTE*) FirstSectStart, FirstSectSize,SearchStr,SearchLen))!=-1)
				if(*(ULONG*)((DWORD)FirstSectStart+SSPos+appendix)==0xff1043ff) // inc dword ptr [ebx+10h];push dword ptr [ebx]
					ThreadSwitchCallbackPatchAddr=(PVOID)((DWORD)FirstSectStart+SSPos+appendix);
		} else {//Nt4,W2k,WXP
			if((SSPos=GetSubstringPosBM((BYTE*) FirstSectStart, FirstSectSize,mov_byte_ptr_es_esi_2Dh_2,sizeof(mov_byte_ptr_es_esi_2Dh_2)))!=-1){
				if(*NtBuildNumber>2195) { //WinXP
					PVOID TmpTSCPA=(PVOID)((DWORD)FirstSectStart+SSPos);
					PatchDataSize=7;
					if((SSPos=GetSubstringPosBM((BYTE*)TmpTSCPA, 0x100,mov_ecx_pebx_cmp_pebx_plusXXXXXXXX_0,sizeof(mov_ecx_pebx_cmp_pebx_plusXXXXXXXX_0)))!=-1)
						ThreadSwitchCallbackPatchAddr=(PVOID)((DWORD)TmpTSCPA+SSPos+2);
				} else { //NT4 & 2k
					ThreadSwitchCallbackPatchAddr=(PVOID)((DWORD)FirstSectStart+SSPos);
					PatchDataSize=5;
				}
			}
		}
	} except (EXCEPTION_EXECUTE_HANDLER) {
	}
	if(ThreadSwitchCallbackPatchAddr==NULL) {
		DbPrint(DC_TSP,DL_ERROR, ("GetSwapContext failed\n"));
		return FALSE;
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
#endif

#ifdef _HOOK_VXD_
THREADSWITCH_THUNK ThreadSwitchThunk;
VOID __stdcall ThreadSwitchCallback(THREADHANDLE OldThreadHandle,THREADHANDLE NewThreadHandle)
{
	ULONG			oldirql;
#ifdef __DBG__
	{
		PTSP_THRD	ThEntry;
		AcquireTSP(&oldirql);
		ThEntry=TSPGetTh((PVOID)NewThreadHandle);
		ReleaseTSP(oldirql);
		if(ThEntry) {
			DbPrint(DC_TSP,DL_SPAM, ("ThreadSwitchCallback %08x->%08x -------------\n",OldThreadHandle,NewThreadHandle));
		}
	}
#endif // __DBG__
		AcquireTSP(&oldirql);
		TSPSwapInTh((PVOID)NewThreadHandle);
		//		TSPSwapRegions((PVOID)OldThreadHandle,TSPRoot,FALSE);
		//		TSPSwapRegions((PVOID)NewThreadHandle,TSPRoot,TRUE);
		ReleaseTSP(oldirql);
}
#endif //_HOOK_VXD_

#ifdef _HOOK_NT_
// located in "INIT" section, look for #pragma alloc_text("INIT",...)
BOOLEAN TSPIsAllowed(VOID) 
{
BOOLEAN ret=TRUE;
#ifndef _TSP_DEDICATED_DRIVER_
NTSTATUS				ntstatus;
HKEY					ParKeyHandle;
UNICODE_STRING		ValueName;
PVOID					LoadBuff=NULL;
OBJECT_ATTRIBUTES	objectAttributes;
ULONG					Res;
	if((LoadBuff = ExAllocatePoolWithTag(PagedPool,PAGE_SIZE,'tSeB')))
	{
		InitializeObjectAttributes(&objectAttributes,&RegParams,OBJ_CASE_INSENSITIVE,NULL,NULL);
		if((ntstatus = ZwOpenKey(&ParKeyHandle,KEY_READ,&objectAttributes))==STATUS_SUCCESS)
		{
			RtlInitUnicodeString(&ValueName,L"NoTSP");
			if(STATUS_SUCCESS == (ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
				LoadBuff, PAGE_SIZE, &Res)))
			{
				if(*(ULONG*)(((PKEY_VALUE_PARTIAL_INFORMATION)LoadBuff)->Data) == 1)
				{
					ret=FALSE;
				}
			}
			ZwClose(ParKeyHandle);
		}
		ExFreePool(LoadBuff);
	}
#endif // _TSP_DEDICATED_DRIVER_
	return ret;
}
#endif // _HOOK_NT_

// located in "INIT" section, look for #pragma alloc_text("INIT",...)
BOOLEAN TSPInit(VOID)
{
	ExInitializeFastMutex(&TSPPrMutex);
	TSPSpinLock = 0;

	return FALSE;
#ifdef _HOOK_NT_
	{
		ProcessorUsageArr = ExAllocatePoolWithTag(PagedPool,*KeNumberProcessors * sizeof(ULONG),'tSeB');
		if(ProcessorUsageArr)
		{
			RtlZeroMemory(ProcessorUsageArr, *KeNumberProcessors * sizeof(ULONG));
			if(TSPIsAllowed())
			{
				HookSetProcessAffinity();
				if(PatchSwapC())
				{
					DbPrint(DC_TSP,DL_IMPORTANT, ("TSPInit ok\n"));
					TSPInitedOk = TRUE;
					
					return TRUE;
				}
			}
		}
		if(ProcessorUsageArr)
			ExFreePool(ProcessorUsageArr);
		DbPrint(DC_TSP,DL_ERROR, ("TSPInit - not inited\n"));
		
		return FALSE;
	}
#else
	Call_When_Thread_Switched(ThreadSwitchCallback, &ThreadSwitchThunk);
	TSPInitedOk = TRUE;
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPInit ok\n"));
	
	return TRUE;
#endif // _HOOK_NT_
}

BOOLEAN TSPDone(VOID)
{
BOOLEAN ret=TRUE;
	if(TSPInitedOk)
	{
		TSPInitedOk=FALSE;
#ifdef _HOOK_NT_
		ExFreePool(ProcessorUsageArr);
#else
		ret=Cancel_Call_When_Thread_Switched(ThreadSwitchCallback,&ThreadSwitchThunk);
#endif // _HOOK_NT_
	}
	ExDestroyFastMutex(&TSPPrMutex);
	DbPrint(DC_TSP,DL_NOTIFY, ("TSPDone %d\n",ret));
	
	return ret;
	
}

BOOLEAN
TspExistRegisterdProcess(PVOID ProcId)
{
	BOOLEAN bExist = FALSE;
	
	PTSP_PROC	PrEntry;

	if(!TSPInitedOk)
		return bExist;
	
	ExAcquireFastMutex(&TSPPrMutex);
	
	PrEntry = TSPPrRoot;

	while(PrEntry && !bExist)
	{
		if(PrEntry->OwnerProcId == ProcId)
		{
			if(PrEntry->RegList)
				bExist = TRUE;
		}
		PrEntry = PrEntry->Next;
	}
	ExReleaseFastMutex(&TSPPrMutex);

	return bExist;
}

VOID
SetTspPAEMode(BOOLEAN bPAE)
{
#ifdef _HOOK_NT_
	if (bPAE)
	{
		gdwPAE = 1;
//		CheckDescriptors = CheckDescriptorsPAE;
	}
	else
	{
//		CheckDescriptors = CheckDescriptorsGeneric;
	}
#endif // _HOOK_NT_
}

VOID
TSPSetMinShadow(ULONG MinShadow)
{
	gMinShadow = MinShadow;
	if(gMinShadow > 100)
		gMinShadow = 100;
}

void
TSP_LoadCheck()
{
	static int check_timeout = 0;

	if ((check_timeout % 20) == 0)
	{
		DbPrint(DC_TSP, DL_SPAM, ("TSP load check\n"));
		if (gbWasReuse)
		{
			gbAllowFreeShadow = FALSE;
			DbPrint(DC_TSP, DL_SPAM, ("TSP was reuse\n"));
			gbWasReuse = FALSE;
		}
		else
		{
			if (!gbAllowFreeShadow)
			{
				gbAllowFreeShadow = TRUE;
				DbPrint(DC_TSP, DL_SPAM, ("TSP allow free shadow\n"));
			}
		}
	}

	check_timeout++;
}