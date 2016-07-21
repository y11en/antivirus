#include "avp_io.h"
#include <_avpio.h>

#define MAKEWORD(low, high) ((WORD)((((WORD)(high)) << 8) | ((BYTE)(low))))
#define PAGENUM(p)  (((ULONG)(p)) >> 12)
#define PAGEOFF(p)  (((ULONG)(p)) & 0xFFF)
#define PAGEBASE(p) (((ULONG)(p)) & ~0xFFF)
#define _NPAGES_(p, k)  ((PAGENUM((char*)p+(k-1)) - PAGENUM(p)) + 1)
#define CARRY_FLAG  0x0001


extern DRP DRIVER_Drp;
extern ILB DRIVER_Ilb;

DWORD WINAPI _AvpMemoryRead (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    BYTE*  buffer	// pointer to buffer to read to 
	)
{
	DWORD ret=0;
	if(offset<=0x100000){
		if((offset+size)>0xA0000) return ret;
		offset+=Get_Sys_VM_Handle()->CB_High_Linear;
	}
	if(offset<0xC0000000) return ret;

	DWORD lBase;
	DWORD pageNo=PAGENUM(offset);
	DWORD pageCount=_NPAGES_(offset,size);
	DWORD off=offset;
	DWORD siz=PAGESIZE;
	BYTE* buf=buffer;
	
	for (DWORD i=0;i<pageCount,siz;i++,pageNo++,off+=siz){
		DWORD off_p=PAGENUM(off)*PAGESIZE;
		
		siz=PAGESIZE;
		if(off > off_p)
			siz -= off - off_p;
		if((off_p+PAGESIZE) > (offset+size))
			siz -= (off_p+PAGESIZE) - (offset+size);

		
		if(1 != PageCheckLinRange(pageNo,1,0)) break;;
		lBase=LinPageLock(pageNo,1,PAGEMAPGLOBAL);
		if(lBase){
			memcpy(buf,(void*)(lBase + PAGEOFF(off)),siz);
			LinPageUnLock(PAGENUM(lBase),1,PAGEMAPGLOBAL);
			ret+=siz;
			buf+=siz;
		}
	}
#ifdef DEBUG
	dprintf("AVP_IO: AvpMemoryRead = %08X %X: %X",offset,size,ret);
#endif
	return ret;
}
	
DWORD WINAPI _AvpMemoryWrite (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    BYTE*  buffer	// pointer to buffer to read to 
	)
{
	DWORD ret=0;
	if(offset<=0x100000){
		if((offset+size)>0xA0000) return ret;
		offset+=Get_Sys_VM_Handle()->CB_High_Linear;
	}
	if(offset<0xC0000000) return ret;
	
	DWORD lBase;
	DWORD pageNo=PAGENUM(offset);
	DWORD pageCount=_NPAGES_(offset,size);
	DWORD off=offset;
	DWORD siz=PAGESIZE;
	BYTE* buf=buffer;
	
	for (DWORD i=0;i<pageCount,siz;i++,pageNo++,off+=siz){
		DWORD off_p=PAGENUM(off)*PAGESIZE;
		
		siz=PAGESIZE;
		if(off > off_p)
			siz -= off - off_p;
		if((off_p+PAGESIZE) > (offset+size))
			siz -= (off_p+PAGESIZE) - (offset+size);
		
		
		if(1 != PageCheckLinRange(pageNo,1,0)) break;;
		lBase=LinPageLock(pageNo,1,PAGEMAPGLOBAL);
		if(lBase){
			memcpy((void*)(lBase + PAGEOFF(off)),buf,siz);
			LinPageUnLock(PAGENUM(lBase),1,PAGEMAPGLOBAL);
			ret+=siz;
			buf+=siz;
		}
	}
#ifdef DEBUG
	dprintf("AVP_IO: AvpMemoryRead = %08X %X: %X",offset,size,ret);
#endif
	return ret;
}

static int nec2pc1 = 0;

static int is_nec()
{
CLIENT_STRUCT	SavedRegs;	
	Save_Client_State(&SavedRegs);
	if(!nec2pc1)
	{
		VMHANDLE vmh;
		CLIENT_STRUCT* pRegs;
		nec2pc1=1;

		Begin_Nest_V86_Exec();
		vmh=Get_Sys_VM_Handle();
		pRegs=(CLIENT_STRUCT*)vmh->CB_Client_Pointer;

		_clientAH = 4;
		_clientCX = 0;
		Exec_Int(0x1A);
		if(_clientCH==0x19) goto pc;
		if(_clientCH==0x20) goto pc;

		_clientAH = 0x12;
		_clientEFlags &= ~CARRY_FLAG;
		Exec_Int(0x1A);
		if (_clientEFlags & CARRY_FLAG) goto pc;

		nec2pc1 = 2;
	pc:
		End_Nest_Exec();

#ifdef DEBUG
		dprintf("AVP_IO: NEC_PC = %d",nec2pc1-1);
#endif

	}
	Restore_Client_State(&SavedRegs);
	return nec2pc1-1;
}

DWORD WINAPI _AvpGetDosMemSize()
{
	DWORD memSize=0;
	_AvpMemoryRead(is_nec()?0x280:0x413,2,(BYTE*)&memSize);
	memSize<<=10;
	if(memSize>0xA0000)memSize=0xA0000;
#ifdef DEBUG
	dprintf("AVP_IO: AvpGetDosMemSize = %08X",memSize);
#endif
 	return memSize; // size of DOS memory in bytes
}


DWORD WINAPI _AvpGetFirstMcbSeg()
{
	ALLREGS   callStruct;
	CLIENT_STRUCT	SavedRegs;	
	Save_Client_State(&SavedRegs);
	DWORD ret=0;
	memset(&callStruct,0,sizeof(ALLREGS));
	callStruct.REAX = 0x5200;
	callStruct.RFLAGS = CARRY_FLAG;
	Exec_VxD_Int(0x21,&callStruct);
	if (!(callStruct.RFLAGS & CARRY_FLAG))
		ret=(DWORD)*(WORD*)(callStruct.REBX-2);
#ifdef DEBUG
	dprintf("AVP_IO: AvpGetFirstMcbSeg = %08X",ret);
#endif
	Restore_Client_State(&SavedRegs);
	return ret;
}

ppIFSFileHookFunc pPrevHook;

int _cdecl MyIfsHook(pIFSFunc pfn, int fn, int Drive, int ResType,
		int CodePage, pioreq pir)
{
	return (*pPrevHook)(pfn, fn, Drive, ResType, CodePage, pir);
}

#pragma pack(1)

struct ifs_hook_struct
{
	ppIFSFileHookFunc HookProc;
 	ppIFSFileHookFunc PrevHookProc;
	ifs_hook_struct* PrevStructPtr;
};


static DWORD GetIfsApiHookTable(DWORD &count,DWORD* &hookPtr, DWORD maxCount)
{
#ifdef DEBUG
	PVOID base;
	char buf[0x80];
#endif
	ifs_hook_struct* IfsHookStructPtr;
	if(count>=maxCount)return count;

	pPrevHook = IFSMgr_InstallFileSystemApiHook(MyIfsHook);
	if(pPrevHook==0)return count;
	IfsHookStructPtr=*(ifs_hook_struct**)(((DWORD)pPrevHook)+4);
	if(0!=IFSMgr_RemoveFileSystemApiHook(MyIfsHook))return count;

	while(IfsHookStructPtr){

		if( (DWORD)IfsHookStructPtr < 0xC0000000
		||  (DWORD)IfsHookStructPtr >=0xFFFF0000
		||  (DWORD)IfsHookStructPtr->HookProc < 0xC0000000
		||  (DWORD)IfsHookStructPtr->HookProc >=0xFFFF0000
		) break;

#ifdef DEBUG
		base=GetVxDName((DWORD)IfsHookStructPtr->HookProc,buf);
		dprintf("AVP_IO: Hook %d %08X  %s",count,IfsHookStructPtr->HookProc,base?buf:"???");
#endif

		*hookPtr=(DWORD)IfsHookStructPtr->HookProc;
		hookPtr++;
		count++;
		IfsHookStructPtr=IfsHookStructPtr->PrevStructPtr;
	}
	
	DWORD addr;
	addr=(DWORD)*IFSMgr_InstallFileSystemApiHook;

	addr=*(DWORD*)(addr+2);
	*hookPtr=*(DWORD*)(addr);
/*
	_AvpMemoryRead(addr+2,4,(BYTE*)&addr);
	_AvpMemoryRead(addr,4,(BYTE*)hookPtr);
*/
#ifdef DEBUG
	base=GetVxDName((DWORD)*hookPtr,buf);
	dprintf("AVP_IO: IFSApiHook %d %08X %s",count,*hookPtr,base?buf:"???");
#endif
	
	hookPtr++;
	count++;

	return count;
}

DWORD WINAPI _AvpGetIfsApiHookTable(DWORD* table, DWORD maxCount)
{
	DWORD count=0;
	Begin_Critical_Section(BLOCK_THREAD_IDLE);
	GetIfsApiHookTable(count,table, maxCount);
	End_Critical_Section();
	return count;
}

DWORD WINAPI _AvpGetDosTraceTable(DWORD* table, DWORD maxCount)
{
	DWORD val;
	DWORD count=0;
	if(!is_nec()){
		if(4==_AvpMemoryRead(0x04c,4,(BYTE*)&val)) table[count++]=val;
		if(4==_AvpMemoryRead(0x100,4,(BYTE*)&val)) table[count++]=val;
	}
		if(4==_AvpMemoryRead(0x084,4,(BYTE*)&val)) table[count++]=val;

#ifdef DEBUG
	dprintf("AVP_IO: AvpGetDosTraceTable = %d",count);
#endif
	return count;
}

BOOL IO13ios( BYTE Fn, BYTE Disk, WORD Sector, BYTE Head, WORD NumSectors, BYTE* Buffer);
BOOL IO13v86(BYTE Fn, BYTE Disk, WORD Sector, BYTE Head, WORD NumSectors, BYTE* Buffer);
//BOOL IO2Xios( BYTE Fn, BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer);
BOOL IO2Xexec(BYTE Fn, BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer);
	

#define LockPhysical(disk,level) DiskLocking(0x084B,(disk),(level))
#define UnlockPhysical(disk)     DiskLocking(0x086B,(disk),0)
#define LockLogical(drive,level) DiskLocking(0x084A,(drive)+1,(level))
#define UnlockLogical(drive)     DiskLocking(0x086A,(drive)+1,0)

#define V86LockPhysical(disk,level,pRegs) V86DiskLocking(0x084B,(disk),(level), (pRegs))
#define V86UnlockPhysical(disk,pRegs)     V86DiskLocking(0x086B,(disk),0, (pRegs))
#define V86LockLogical(drive,level,pRegs) V86DiskLocking(0x084A,(drive)+1,(level) ,(pRegs))
#define V86UnlockLogical(drive,pRegs)     V86DiskLocking(0x086A,(drive)+1,0, (pRegs))


#pragma optimize("",off)
BOOL V86DiskLocking(WORD func, BYTE disk, BYTE level,  CLIENT_STRUCT* pRegs)
{
	CLIENT_STRUCT	SavedRegs;	
	BOOL ret=0;
	Save_Client_State(&SavedRegs);
	
//	dprintf("AVP_IO: Enter V86DiskLocking disk %X  finc%X level%X",disk,func,level);
	_clientAX = 0x440d;
	_clientBX = MAKEWORD(disk,level); //lock level
	_clientCX = func;
	_clientDX = (level==1)?1:0;
	Exec_Int(0x21);
	ret=!(_clientEFlags & CARRY_FLAG);
//	dprintf("AVP_IO: V86DiskLocking done: %X",ret);
	Restore_Client_State(&SavedRegs);
	return ret;
}
#pragma optimize("",on)

BOOL DiskLocking(WORD func, BYTE disk, BYTE level)
{
	CLIENT_STRUCT	SavedRegs;	
	ALLREGS reg;
	Save_Client_State(&SavedRegs);
	reg.REAX = 0x440d;
	reg.REBX = MAKEWORD(disk,level); //lock level
	reg.RECX = func;
	reg.REDX = (level==1)?1:0;
	Exec_VxD_Int(0x21,&reg);
	Restore_Client_State(&SavedRegs);
	return !(reg.RFLAGS & CARRY_FLAG);
}

BOOL WINAPI _AvpGetDiskParam (
    BYTE   disk,
    BYTE   drive,
    WORD*   CX,
    BYTE*	DH
	)
{
	DWORD Num_Cylinders;
	DWORD Num_Sec_Per_Track;
	DWORD Num_Heads;
	PBDD pBdd;
	BOOL ret=FALSE;
//	DWORD DriveInfo;
	if(CX)*CX=0;
	if(DH)*DH=0;
	BYTE* Buffer;

#ifdef DEBUG
	dprintf("AVP_IO:*");
#endif

	Buffer=(BYTE*)malloc(0x200);
	if(!Buffer)return 0;

	if(!ret)if(_AvpRead13(disk,1,0,1,Buffer)) ret=TRUE;
	if(!ret)if(_AvpRead25(drive,0,1,Buffer)) ret=TRUE;

	free(Buffer);

	
	pBdd=IOS_Find_Int13_Drive(disk);
//	pBdd=NULL; //Debug

	if(!pBdd)
	{	
		BYTE buf[8];
		if(IO13v86(8, disk, 0, 0, 0, buf)){
			if(CX)*CX=*(WORD*)buf;
			if(DH)*DH=buf[2];
		}
	}
	else
	{
		Num_Cylinders = pBdd->BDD_Num_Cylinders;
		Num_Sec_Per_Track = pBdd->BDD_Num_Sec_Per_Track;
		Num_Heads = pBdd->BDD_Num_Heads;

		if((Num_Cylinders) < 0x400
		&& (Num_Sec_Per_Track) < 0x40
		&& (Num_Heads) <0x100)
		{
calc:		
			WORD w=((BYTE)(Num_Cylinders -1))<<8;
			w|=(0xC0)&((Num_Cylinders -1)>>2);
			w|=(0x3F)&(BYTE)(Num_Sec_Per_Track);
			if(CX)*CX=w;
			if(DH)*DH=(BYTE)(Num_Heads -1);
		}
		else{
#define BDD_IN_DCB_OFFSET ((DWORD)&(((PDCB)0)->DCB_bdd))
#ifdef DEBUG
	dprintf("AVP_IO: PDCB calc");
#endif

			PDCB pDcb=(PDCB)( (DWORD)pBdd - BDD_IN_DCB_OFFSET );    //was:0xC3
//			DDCB* pdcb=(DDCB*)((int)pBdd-sizeof(DDCB)+sizeof(DCB_BLOCKDEV));
	// А вот здесь я получаю Large параметры :(
			Num_Heads=(WORD)pDcb->DCB_bdd.DCB_apparent_head_cnt;
			Num_Cylinders=(WORD)pDcb->DCB_bdd.DCB_apparent_cyl_cnt;
			Num_Sec_Per_Track=(WORD)pDcb->DCB_bdd.DCB_apparent_spt;
			goto calc;
		}

	}

#ifdef DEBUG
	dprintf("AVP_IO: AvpGetDiskParam disk:%X drive:%X ok=%d CX:%X, DH:%X",disk,drive,ret,CX?*CX:-1,DH?*DH:-1);
#endif
	return ret;
}
/*
//----------------------------------------------------------------------
//
// VCacheFindBlockHook
//
// FindBlock is the main routine called to determine whether a block
// is in the cache or not. We break it into two calls: a lookup followed
// by a create if that is requested so that we can distinguis misses
// from creates.
//
//----------------------------------------------------------------------
static BOOL VCacheFindBlockHooked=FALSE;
static DeviceService_THUNK   thunkVCacheFindBlockHook;
static DWORD curSector;

#pragma optimize("",off)

VOID __stdcall VCacheFindBlockHook( PDSFRAME pDS )
{
	DSFRAME  saveregs;

//	dprintf("FindBlock fsi/opt=%08X sec=%08X vol=%08X ",pDS->REAX,pDS->REBX,pDS->REDI);

	saveregs = *pDS;
	pDS->REAX &= ~0xFF;
    Call_Previous_Hook_Proc( pDS, &thunkVCacheFindBlockHook );
	if( !(pDS->RFLAGS & CARRY_FLAG)
		&& pDS->RESI 
		&& (0==VCache_TestHold((void*)pDS->RESI))
		&& ((saveregs.REBX > 0xFFFFFF00 ) || (saveregs.REBX == curSector ))
		)
	{
		//force sector rereading to cache
		VCache_FreeBlock((void*)pDS->RESI);
	}
	*pDS = saveregs;
	Call_Previous_Hook_Proc( pDS, &thunkVCacheFindBlockHook );
}
*/

BOOL WINAPI _AvpRead25(	BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	WORD Error=0;
	BOOL ret=0;
	void* buf;
	BYTE Fn=0x25;
	BOOL hooked=FALSE;

	buf=malloc(NumSectors*1024);
	if(!buf)return 0;

	VolFlush(Drive,VOL_DISCARD_CACHE);

/*
	PVOID                 VCacheFindBlockService;
	curSector=Sector;
// hook the findblock service
	if(!VCacheFindBlockHooked)
	{
		VCacheFindBlockService = Hook_Device_Service(GetVxDServiceOrdinal(VCache_FindBlock),
		      VCacheFindBlockHook, &thunkVCacheFindBlockHook);
		VCacheFindBlockHooked=TRUE;
		hooked=TRUE;
	}
*/

	if(!ret){
		ret=R0_ReadAbsoluteDisk(Drive,NumSectors,Sector,buf,&Error);
		VWIN32_CopyMem(buf,Buffer,NumSectors*512,NULL);
#ifdef DEBUG
	if(ret)
	dprintf("AVP_IO: Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",Drive+'A',Drive,Fn,Sector,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],Buffer[0x1fe],Buffer[0x1ff]);
	else
	dprintf("AVP_IO: Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d",Drive+'A',Drive,Fn,Sector,ret);
//		if(!ret) dprintf("AVP_IO: Int25r0:  Error=%04X",Error);
#endif
	}
	if(!ret)
	{
		ret= IO2Xexec(Fn, Drive, Sector, NumSectors, (BYTE*)buf);
		VWIN32_CopyMem(buf,Buffer,NumSectors*512,NULL);
	}
/*
	// unhook the findblock service
	if(hooked){
		Unhook_Device_Service(GetVxDServiceOrdinal(VCache_FindBlock),
			VCacheFindBlockHook, &thunkVCacheFindBlockHook);
		VCacheFindBlockHooked=FALSE;
	}
*/
	free(buf);
	return ret;
}
#pragma optimize("",on)

BOOL WINAPI _AvpWrite26( BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	WORD Error;
	BOOL ret=0;
	void* buf;
	BYTE Fn=0x26;

	buf=malloc(NumSectors*1024);
	if(!buf)return 0;

	VolFlush(Drive,VOL_DISCARD_CACHE);
	
	VWIN32_CopyMem(Buffer,buf,NumSectors*512,NULL);

	if(!ret){
		ret=R0_WriteAbsoluteDisk(Drive,NumSectors,Sector,buf,&Error);
#ifdef DEBUG
	if(ret)
	dprintf("AVP_IO: Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",Drive+'A',Drive,Fn,Sector,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],Buffer[0x1fe],Buffer[0x1ff]);
	else
	dprintf("AVP_IO: Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d",Drive+'A',Drive,Fn,Sector,ret);
#endif
	}
	if(!ret)
	{
		ret= IO2Xexec(Fn, Drive, Sector, NumSectors, (BYTE*)buf);
	}
	
	free(buf);
	return ret;
}

BOOL WINAPI _AvpRead13 (BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
	BOOL ret=0;
	void* buf=malloc(NumSectors*1024);
	if(!buf)return 0;
	memset(buf,0,512);

		ret= IO13v86(2, Disk, Sector, Head, NumSectors, (BYTE*)buf);
	if(!ret){
		memset(buf,0,512);
		ret= IO13ios(2, Disk, Sector, Head, NumSectors, (BYTE*)buf);
	}
	VWIN32_CopyMem(buf,Buffer,NumSectors*512,NULL);
	free(buf);
	return ret;
}

BOOL WINAPI _AvpWrite13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
	BOOL ret=0;
	void* buf=malloc(NumSectors*1024);
	if(!buf)return 0;
	VWIN32_CopyMem(Buffer,buf,NumSectors*512,NULL);

		ret= IO13v86(3, Disk, Sector, Head, NumSectors, (BYTE*)buf);
	if(!ret)
		ret= IO13ios(3, Disk, Sector, Head, NumSectors, (BYTE*)buf);

	free(buf);
	return ret;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	GetIop
//
// Purpose
//	Allocate an IOP
//
// Parameters
//	pSvc		address of the ILB service routine
//	IopSize		size of the IOP to allocate
//	DeltaIor	byte offset from start of IOP to start of IOR
//	flags		flags to provide on allocation call
//	pResult		location to receive result
//
// Return Value
//	Returns a pointer to the allocated IOP
//
static IOP* GetIop(PFNISP pSvc, USHORT IopSize, ULONG DeltaIor, 
	UCHAR flags, WORD* pResult)
{
	ISP_IOP_alloc AllocIsp;

	AllocIsp.ISP_i_c_hdr.ISP_func = ISP_CREATE_IOP;
	AllocIsp.ISP_IOP_size = IopSize;
	AllocIsp.ISP_delta_to_ior = DeltaIor;
	AllocIsp.ISP_i_c_flags = flags;

	pSvc((ISP*)&AllocIsp);
	if (pResult)
		*pResult = AllocIsp.ISP_i_c_hdr.ISP_result;

	return (IOP*)AllocIsp.ISP_IOP_ptr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Function
//	ReleaseIop
//
// Purpose
//	Releases a previously allocate IOP
//
// Parameters
//	pSvc	address of the ILB service routine
//	pIop	address of the IOP to free
//
// Return Value
//	Returns the result from the dealloc call
//
static WORD ReleaseIop(PFNISP pSvc, IOP* pIop)
{
	ISP_mem_dealloc DeallocIsp;

	DeallocIsp.ISP_mem_da_hdr.ISP_func = ISP_DEALLOC_MEM;
	DeallocIsp.ISP_mem_ptr_da = (ULONG)pIop;

	pSvc((ISP*)&DeallocIsp);
	
	return DeallocIsp.ISP_mem_da_hdr.ISP_result;
}


static int iosRegister=0;
static BOOL IO13ios(   BYTE Fn, BYTE Disk, WORD Sector, BYTE Head, WORD NumSectors, BYTE* Buffer)
{
	BOOL   ret=FALSE;
	WORD some=0;
	PDCB pDcb;
	IOP* pIop;
	IOR* pIor;
	IOP* peIop;
	DWORD explen;
	WORD iopSize;
	DWORD sector;

	PBDD pBdd=IOS_Find_Int13_Drive(Disk);
//	pBdd=NULL; //Debug
	if(pBdd==NULL)goto ret;


#define BDD_IN_DCB_OFFSET ((DWORD)&(((PDCB)0)->DCB_bdd))

	pDcb=(PDCB)( (DWORD)pBdd - BDD_IN_DCB_OFFSET );    //was:0xC3

	if((pDcb->DCB_cmn.DCB_device_flags & DCB_DEV_RMM))
		return IO13v86(Fn, Disk, Sector, Head, NumSectors, Buffer);
//		goto ret;
	
	if(pDcb!=(PDCB)(pDcb->DCB_cmn.DCB_physical_dcb)) //is this physical dev
	{
		some=8;
		pDcb=(PDCB)(some + (DWORD)pDcb);
		if(pDcb!=(PDCB)(pDcb->DCB_cmn.DCB_physical_dcb)) ///? Old beta-version DCB struct?
			goto ret;
	}

	sector=Sector<<2; //temporary use sector as CyllNo
    sector&=0x0300;	  //
    sector|=Sector>>8;// conversion from int13
					  
	sector=sector*pBdd->BDD_Num_Heads + Head;
	sector*=pBdd->BDD_Num_Sec_Per_Track;
	sector+=(Sector & 0x003F)-1;


	if(!iosRegister){
		iosRegister=1;
		IOS_Register(&DRIVER_Drp);
	}
	
	explen=pDcb->DCB_cmn.DCB_expansion_length;

	iopSize=((PDCB)(((DWORD)pDcb)-some))->DCB_max_sg_elements;
	iopSize<<=3;
	iopSize+=(WORD)explen;
	iopSize+=sizeof(IOP);

#define IOR_IN_IOP_OFFSET ((ULONG)&(((IOP*)0)->IOP_ior))
	
	peIop=GetIop(DRIVER_Ilb.ILB_service_rtn,iopSize,explen + IOR_IN_IOP_OFFSET,2,0);

	pIop=(IOP*)(((DWORD)peIop) + explen);
	pIor=(IOR*)(((DWORD)peIop) + explen + IOR_IN_IOP_OFFSET);
	pIop->IOP_original_dcb=(PDCB_COMMON)pDcb; 

	pIor->IOR_sgd_lin_phys=((DWORD)pIor) + sizeof(IOR); 
	//pIor->IOR_num_sgds=0;
	pIor->IOR_next=0;
	pIor->IOR_flags=IORF_HIGH_PRIORITY |IORF_SYNC_COMMAND |IORF_VERSION_002 | IORF_PHYS_CMD;
	pIor->IOR_req_vol_handle=0;
	pIor->IOR_callback=0;
	pIor->IOR_func=(Fn==3)?IOR_WRITE:IOR_READ;
	pIor->IOR_vol_designtr=Disk; //disknum c: = 2 ???????
	pIor->IOR_start_addr[0]=sector;
	pIor->IOR_start_addr[1]=0;
	pIor->IOR_xfer_count=NumSectors;
//	pIor->IOR_buffer_ptr=(DWORD)Buffer;
// But I should to lock It.
	{
		DWORD pageNo=PAGENUM(Buffer);
		DWORD pageCount=_NPAGES_(Buffer,NumSectors*pBdd->BDD_Num_Sec_Per_Track);
		if(pageCount != PageCheckLinRange(pageNo,pageCount,0)) goto rel;
		DWORD lBase=LinPageLock(pageNo,pageCount,PAGEMAPGLOBAL);

		if(lBase)
		{
			pIor->IOR_buffer_ptr=lBase + PAGEOFF(Buffer);

			DRIVER_Ilb.ILB_int_io_criteria_rtn(pIop);
			IOS_SendCommand(pIor,pBdd);

			if(!(pIor->IOR_status & (IORS_ERROR_DESIGNTR|IORS_NOT_READY)))
				ret=TRUE;

			LinPageUnLock(PAGENUM(lBase),pageCount,PAGEMAPGLOBAL);
		}
	}
rel:
	ReleaseIop(DRIVER_Ilb.ILB_service_rtn,peIop);

ret:
#ifdef DEBUG
	if(ret && (Fn==2 || Fn==3))
	dprintf("AVP_IO: Int13ios:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0);
	else
	dprintf("AVP_IO: Int13ios:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0);
#endif
	return ret;
}

#pragma optimize("",off)
static BOOL IO13v86(BYTE Fn, BYTE Disk, WORD Sector, BYTE Head, WORD NumSectors, BYTE* Buffer)
{
	BOOL   ret=FALSE;
	int ntry=5;
	VMHANDLE vmh;
	DWORD count;
	SEGOFFSET rmBuffer;
	CLIENT_STRUCT* pRegs;
	CLIENT_STRUCT	SavedRegs;	
	WORD DS_;

	if(is_nec()) return FALSE;
	Save_Client_State(&SavedRegs);
	
	_asm mov DS_,ds;

	Begin_Nest_V86_Exec();
	vmh=Get_Sys_VM_Handle();
	pRegs=(CLIENT_STRUCT*)vmh->CB_Client_Pointer;
	if(!V86MMGR_Allocate_Buffer(vmh,pRegs,NumSectors*512,DS_,Buffer,Fn==3,&count,&rmBuffer))
		goto ene;

	if(Fn==3 && (Disk>0x20)){
		if(!(ret=V86LockPhysical(Disk,1,pRegs)))goto ret;
		if(!(ret=V86LockPhysical(Disk,2,pRegs)))goto ul;
		if(Fn==3)
			if(!(ret=V86LockPhysical(Disk,3,pRegs)))goto ul3;
	}
	ret = FALSE;



pntry:
	_clientAX = MAKEWORD(NumSectors,Fn); // BIOS read, 1 sector
	_clientCX = Sector;                // Sector 1, Cylinder 0
	_clientDX = MAKEWORD(Disk,Head);   // Head 0, Drive #
	_clientBX = (WORD)GET_OFFSET(rmBuffer);
	_clientES = (WORD)GET_SEGMENT(rmBuffer);

	Exec_Int(0x13);

	if (_clientAH==0)
		ret = TRUE;

	if (!(_clientEFlags & CARRY_FLAG))
		ret = TRUE;

	if(ret==FALSE && --ntry){
		_clientAX = 0;
		_clientDX = (WORD)MAKEWORD(Disk,0);   // Head 0, Drive #
		Exec_Int(0x13);
		goto pntry;
	}
	

	if(ret && Fn==8){
        *(WORD*)Buffer=_clientCX;
		Buffer[2]=_clientDH;
	}

	if(Fn==3 && (Disk>0x20))
	{
		V86UnlockPhysical(Disk,pRegs);
ul3:V86UnlockPhysical(Disk,pRegs);
ul:	V86UnlockPhysical(Disk,pRegs);
	}

ret:
	
	V86MMGR_Free_Buffer(vmh,pRegs,count,DS_,Buffer,Fn==2);
ene:	
	End_Nest_Exec();

#ifdef DEBUG
	if(ret && (Fn==2 || Fn==3))
	dprintf("AVP_IO: Int13v86:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0);
	else
	dprintf("AVP_IO: Int13v86:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0);
#endif
	Restore_Client_State(&SavedRegs);
	return ret;
}

typedef struct _X_PACKET{
	DWORD sector;
	WORD  count;
	DWORD buffer;
}X_PACKET;
#pragma optimize("",on)


#pragma optimize("",off)
static BOOL IO2Xexec(BYTE Fn, BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
	BOOL   ret=FALSE;
	VMHANDLE vmh;
	DWORD count,count1;
	SEGOFFSET rmBuffer;
	SEGOFFSET rmPacket;
	X_PACKET packet;
	CLIENT_STRUCT* pRegs;
	WORD DS_;
	CLIENT_STRUCT	SavedRegs;	
	_asm mov DS_,ds;
	Save_Client_State(&SavedRegs);
	
	Begin_Nest_V86_Exec();
	vmh=Get_Sys_VM_Handle();
	pRegs=(CLIENT_STRUCT*)vmh->CB_Client_Pointer;
	if(!V86MMGR_Allocate_Buffer(vmh,pRegs,NumSectors*512,DS_,Buffer,Fn==0x26,&count,&rmBuffer))
		goto ene;

	packet.sector=(DWORD)Sector;
	packet.count=NumSectors;
	packet.buffer=rmBuffer;
	if(!V86MMGR_Allocate_Buffer(vmh,pRegs,sizeof(X_PACKET),DS_,&packet,1,&count1,&rmPacket))
		goto ene1;
/*
	{
		DWORD lockType;
		DWORD lockFlags;
		DWORD lockOwner;
		int   lockState;
		lockState=IFSMgr_GetLockState(Drive,&lockType,&lockFlags,&lockOwner);

#ifdef DEBUG
	dprintf("AVP_IO: LockState=%d type=%08X flags=%08X owner=%08X",lockState,lockType,lockFlags,lockOwner);
#endif
		if(-1!=lockState) goto ret;
	}
*/
	if(Fn==0x26)
	{
	if(!(ret=V86LockLogical(Drive,1,pRegs)))goto ret;
	if(!(ret=V86LockLogical(Drive,2,pRegs)))goto ul;
	if(Fn==0x26)
		if(!(ret=V86LockLogical(Drive,3,pRegs)))goto ul3;
	}
	ret = FALSE;
	
	if(!ret){
		_clientAX = Drive;
		_clientCX = (WORD)-1;                //should be -1
		_clientBX = (WORD)GET_OFFSET(rmPacket);
		_clientDS = (WORD)GET_SEGMENT(rmPacket);
		
		Exec_Int(Fn);

		if (!(_clientEFlags & CARRY_FLAG))
			ret = TRUE;
	}
	if(!ret){
		_clientAX = 0x7305;
		_clientSI = (Fn==0x26)?0x0001:0x0000;
		_clientCX = (WORD)-1;                //should be -1
		_clientDX = Drive + 1;
		_clientBX = (WORD)GET_OFFSET(rmPacket);
		_clientDS = (WORD)GET_SEGMENT(rmPacket);
		
		Exec_Int(0x21);

		if (!(_clientEFlags & CARRY_FLAG))
			ret = TRUE;
	}

	if(Fn==0x26)
	{
		if(Fn==0x26)
			V86UnlockLogical(Drive,pRegs);
ul3:	V86UnlockLogical(Drive,pRegs);
ul:		V86UnlockLogical(Drive,pRegs);
	}
ret:
	V86MMGR_Free_Buffer(vmh,pRegs,count1,0,0,0);
ene1:
	V86MMGR_Free_Buffer(vmh,pRegs,count,DS_,Buffer,Fn==0x25);
ene:	
	End_Nest_Exec();

#ifdef DEBUG
	if(ret)
	dprintf("AVP_IO: Int2Xexec: (%c:) drive=%02X fn=%X sector=%08X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",Drive+'A',Drive,Fn,Sector,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],Buffer[0x1fe],Buffer[0x1ff]);
	else
	dprintf("AVP_IO: Int2Xexec: (%c:) drive=%02X fn=%X sector=%08X ok=%d",Drive+'A',Drive,Fn,Sector,ret);
#endif
	Restore_Client_State(&SavedRegs);
	return ret;
}
#pragma optimize("",on)

