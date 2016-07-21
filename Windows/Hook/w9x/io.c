
#define MAKEWORD(low, high) ((WORD)((((WORD)(high)) << 8) | ((BYTE)(low))))
#define CARRY_FLAG  0x0001
#define PAGENUM(p)  (((ULONG)(p)) >> 12)
#define PAGEOFF(p)  (((ULONG)(p)) & 0xFFF)
#define PAGEBASE(p) (((ULONG)(p)) & ~0xFFF)
#define _NPAGES_(p, k)  ((PAGENUM((char*)p+(k-1)) - PAGENUM(p)) + 1)

extern DRP DRIVER_Drp;
extern ILB DRIVER_Ilb;


DWORD is_nec(VOID)
{
static DWORD nec2pc1 = 0;
VMHANDLE vmh;
CLIENT_STRUCT* pRegs;
CLIENT_STRUCT	SavedRegs;
	if(!nec2pc1)	{
		nec2pc1=1;
		vmh=Get_Sys_VM_Handle();
		Save_Client_State(&SavedRegs);
		Begin_Nest_V86_Exec();
		pRegs=(CLIENT_STRUCT*)vmh->CB_Client_Pointer;
		_clientAH = 4;
		_clientCX = 0;
		Exec_Int(0x1A);
		if(_clientCH!=0x19 && _clientCH!=0x20) {
			_clientAH = 0x12;
			_clientEFlags &= ~CARRY_FLAG;
			Exec_Int(0x1A);
			if (!(_clientEFlags & CARRY_FLAG)) nec2pc1 = 2;
		}
		End_Nest_Exec();
		Restore_Client_State(&SavedRegs);		
	}
	DbPrint(DC_NOCAT,DL_INFO, ("is_nec=%d \n",nec2pc1-1));
	return nec2pc1-1;
}


#pragma optimize("",off)
BOOL V86DiskLocking(WORD func, BYTE disk, BYTE level,  CLIENT_STRUCT* pRegs)
{
BOOL ret=0;
CLIENT_STRUCT	SavedRegs;
	Save_Client_State(&SavedRegs);
	_clientAX = 0x440d;
	_clientBX = MAKEWORD(disk,level); //lock level
	_clientCX = func;
	_clientDX = (level==1)?1:0;
	Exec_Int(0x21);
	ret=!(_clientEFlags & CARRY_FLAG);
	Restore_Client_State(&SavedRegs);		
	DbPrint(DC_NOCAT,DL_INFO, ("V86DiskLocking return %d\n",ret));
	return ret;
}
#pragma optimize("",on)

BOOL DiskLocking(WORD func, BYTE disk, BYTE level)
{
ALLREGS reg;
CLIENT_STRUCT	SavedRegs;
	Save_Client_State(&SavedRegs);
	reg.REAX = 0x440d;
	reg.REBX = MAKEWORD(disk,level); //lock level
	reg.RECX = func;
	reg.REDX = (level==1)?1:0;
	Exec_VxD_Int(0x21,&reg);
	Restore_Client_State(&SavedRegs);		
	DbPrint(DC_NOCAT,DL_INFO, ("DiskLocking return %d\n",!(reg.RFLAGS & CARRY_FLAG)));
	return !(reg.RFLAGS & CARRY_FLAG);
}

#define LockPhysical(disk,level) DiskLocking(0x084B,(disk),(level))
#define UnlockPhysical(disk)     DiskLocking(0x086B,(disk),0)
#define LockLogical(drive,level) DiskLocking(0x084A,(drive)+1,(level))
#define UnlockLogical(drive)     DiskLocking(0x086A,(drive)+1,0)

#define V86LockPhysical(disk,level,pRegs) V86DiskLocking(0x084B,(disk),(level), (pRegs))
#define V86UnlockPhysical(disk,pRegs)     V86DiskLocking(0x086B,(disk),0, (pRegs))
#define V86LockLogical(drive,level,pRegs) V86DiskLocking(0x084A,(BYTE)((drive)+1),(level) ,(pRegs))
#define V86UnlockLogical(drive,pRegs)     V86DiskLocking(0x086A,(BYTE)((drive)+1),0, (pRegs))



#pragma optimize("",off)
static BOOL IO13v86(BYTE Fn, BYTE Disk, DWORD Sector, BYTE Head, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
{
	BOOL   ret=FALSE;
	int ntry=5;
	VMHANDLE vmh;
	DWORD count;
	SEGOFFSET rmBuffer;
	CLIENT_STRUCT* pRegs;
	CLIENT_STRUCT	SavedRegs;
	WORD DS_;
	
	if(is_nec()) 
		return FALSE;
	
	_asm mov DS_,ds;
	Save_Client_State(&SavedRegs);
	
	Begin_Nest_V86_Exec();
	vmh=Get_Sys_VM_Handle();
	pRegs=(CLIENT_STRUCT*)vmh->CB_Client_Pointer;
	if(!V86MMGR_Allocate_Buffer(vmh,pRegs,BufferSize,DS_,Buffer,Fn==3,&count,&rmBuffer))
		goto ene;
	if(Fn==3 && (Disk>0x20))
	{
		if(!(ret=V86LockPhysical(Disk,1,pRegs)))goto ret;
		if(!(ret=V86LockPhysical(Disk,2,pRegs)))goto ul;
		if(Fn==3)
			if(!(ret=V86LockPhysical(Disk,3,pRegs)))goto ul3;
	}
	ret = FALSE;
	
pntry:
/*
AH = 02h
AL = number of sectors to read (must be nonzero)
CH = low eight bits of cylinder number
CL = sector number 1-63 (bits 0-5)
high two bits of cylinder (bits 6-7, hard disk only)
DH = head number
DL = drive number (bit 7 set for hard disk)
ES:BX -> data buffer

*/
	_clientAX = MAKEWORD(NumSectors,Fn); // BIOS read, 1 sector
	_clientCX = (WORD)Sector;                // Sector 1, Cylinder 0
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
ul3:	V86UnlockPhysical(Disk,pRegs);
ul:		V86UnlockPhysical(Disk,pRegs);
	}
	
ret:
	V86MMGR_Free_Buffer(vmh,pRegs,count,DS_,Buffer,Fn==2);
ene:	
	End_Nest_Exec();
	Restore_Client_State(&SavedRegs);		
#ifdef __DBG__
	if(ret && (Fn==2 || Fn==3)) {
		DbPrint(DC_NOCAT,DL_INFO, ("Int13v86:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0));
	} else {
		DbPrint(DC_NOCAT,DL_INFO, ("Int13v86:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0));
	}
#endif
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


#define BDD_IN_DCB_OFFSET ((DWORD)&(((PDCB)0)->DCB_bdd))
#define IOR_IN_IOP_OFFSET ((ULONG)&(((IOP*)0)->IOP_ior))
static int iosRegister=0;
static BOOL IO13ios(   BYTE Fn, BYTE Disk, DWORD Sector, BYTE Head, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
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
PBDD pBdd;
	pBdd=IOS_Find_Int13_Drive(Disk);
//	pBdd=NULL; //Debug
	if(pBdd==NULL) 
		goto ret;
	pDcb=(PDCB)( (DWORD)pBdd - BDD_IN_DCB_OFFSET );    //was:0xC3
	if((pDcb->DCB_cmn.DCB_device_flags & DCB_DEV_RMM))
		return IO13v86(Fn, Disk, Sector, Head, NumSectors, BufferSize, Buffer);
	if(pDcb!=(PDCB)(pDcb->DCB_cmn.DCB_physical_dcb)) { //is this physical dev
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
		DWORD lBase;
		DWORD pageNo=PAGENUM(Buffer);
		DWORD pageCount=_NPAGES_(Buffer,NumSectors*pBdd->BDD_Num_Sec_Per_Track);
		if(pageCount != PageCheckLinRange(pageNo,pageCount,0)) 
			goto rel;
		lBase=LinPageLock(pageNo,pageCount,PAGEMAPGLOBAL);
		if(lBase) {
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
#ifdef __DBG__
	if(ret && (Fn==2 || Fn==3)) {
		DbPrint(DC_NOCAT,DL_INFO, ("Int13ios:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0));
	}else {
		DbPrint(DC_NOCAT,DL_INFO, ("Int13ios:  (%c:)  disk=%02X fn=%d sec=%08X h=%X ok=%d",(Disk>0x20)?'?':(Disk+'A'),Disk,Fn,Sector,Head,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],(Fn&2)?Buffer[0x1fe]:0,(Fn&2)?Buffer[0x1ff]:0));
	}
#endif
	return ret;
}

typedef struct _X_PACKET{
	DWORD sector;
	WORD  count;
	DWORD buffer;
}X_PACKET;
#pragma optimize("",on)


#pragma optimize("",off)
static BOOL IO2Xexec(BYTE Fn, BYTE Drive, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
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
	Save_Client_State(&SavedRegs);
	_asm mov DS_,ds;

	Begin_Nest_V86_Exec();
	vmh=Get_Sys_VM_Handle();
	pRegs=(CLIENT_STRUCT*)vmh->CB_Client_Pointer;
	if(!V86MMGR_Allocate_Buffer(vmh,pRegs,BufferSize,DS_,Buffer,Fn==0x26,&count,&rmBuffer))
		goto ene;

	packet.sector=(DWORD)Sector;
	packet.count=(WORD)NumSectors;
	packet.buffer=rmBuffer;
	if(!V86MMGR_Allocate_Buffer(vmh,pRegs,sizeof(X_PACKET),DS_,&packet,1,&count1,&rmPacket))
		goto ene1;
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
	if(Fn==0x26)	{
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
	Restore_Client_State(&SavedRegs);		
	
#ifdef __DBG__
	if(ret) {
		DbPrint(DC_NOCAT,DL_INFO, ("Int2Xexec: (%c:) drive=%02X fn=%X sector=%08X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",Drive+'A',Drive,Fn,Sector,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],Buffer[0x1fe],Buffer[0x1ff]));
	}	else {
		DbPrint(DC_NOCAT,DL_INFO, ("Int2Xexec: (%c:) drive=%02X fn=%X sector=%08X ok=%d",Drive+'A',Drive,Fn,Sector,ret));
	}
#endif
	return ret;
}
#pragma optimize("",on)

extern BOOL try_cdr_read(BYTE Drive, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer);

BOOL _AvpRead25(BYTE Drive, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
{
	WORD Error=0;
	BOOL bret=0;
	void* buf;
	BYTE Fn=0x25;
	BOOL hooked=FALSE;
	buf=malloc(BufferSize);
	if(!buf)
		return 0;
	VolFlush(Drive,VOL_DISCARD_CACHE);
	if(!bret){
		bret=R0_ReadAbsoluteDisk(Drive, NumSectors, Sector, buf, &Error);
		VWIN32_CopyMem(buf,Buffer,BufferSize,NULL);
#ifdef __DBG__
		if(bret) {
			DbPrint(DC_LLDISKIO, DL_INFO, ("Int25r0:  (%c:) drive=%02X fn=%X sector=%08X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",Drive+'A',Drive,Fn,Sector,bret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],Buffer[0x1fe],Buffer[0x1ff]));
		} else {
			DbPrint(DC_LLDISKIO, DL_INFO, ("Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d",Drive+'A',Drive,Fn,Sector,bret));
		}
#endif
	}
	if(!bret) {
		bret = IO2Xexec(Fn, Drive, Sector, NumSectors, BufferSize, (BYTE*)buf);
		VWIN32_CopyMem(buf, Buffer, BufferSize,NULL);
		if (!bret)
			bret = try_cdr_read(Drive, Sector, NumSectors, BufferSize, Buffer);
	}
	
	free(buf);
	return bret;
}

#pragma optimize("",on)

BOOL _AvpWrite26( BYTE Drive, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
{
	WORD Error;
	BOOL ret=0;
	void* buf;
	BYTE Fn=0x26;
	buf=malloc(BufferSize);
	if(!buf)
		return 0;
	VolFlush(Drive,VOL_DISCARD_CACHE);
	VWIN32_CopyMem(Buffer,buf,BufferSize,NULL);
	if(!ret) {
		ret= IO2Xexec(Fn, Drive, Sector, NumSectors, BufferSize, (BYTE*)buf);
	}
	if(!ret){
		ret=R0_WriteAbsoluteDisk(Drive, NumSectors, Sector, buf, &Error);
#ifdef DEBUG
		if(ret) {
			DbPrint(DC_LLDISKIO, DL_INFO, ("Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d %02X%02X%02X%02X%02X%02X...%02X%02X",Drive+'A',Drive,Fn,Sector,ret,Buffer[0],Buffer[1],Buffer[2],Buffer[3],Buffer[4],Buffer[5],Buffer[0x1fe],Buffer[0x1ff]));
		}	else {
			DbPrint(DC_LLDISKIO, DL_INFO, ("Int25r0:   (%c:) drive=%02X fn=%X sector=%08X ok=%d",Drive+'A',Drive,Fn,Sector,ret));
		}
#endif
	}
	free(buf);
	return ret;
}

BOOL _AvpRead13(BYTE Disk, BYTE Head, DWORD Cylinder, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
{
	BOOL ret=0;
	void* buf;
	buf=malloc(BufferSize);
	if(!buf)
		return 0;
	memset(buf,0,BufferSize);
	ret=IO13v86(2, Disk, Sector, Head, NumSectors, BufferSize, (BYTE*)buf);
	if(!ret){
		memset(buf,0,BufferSize);
		ret= IO13ios(2, Disk, Sector, Head, NumSectors, BufferSize, (BYTE*)buf);
	}
	VWIN32_CopyMem(buf,Buffer,BufferSize,NULL);
	free(buf);
	return ret;
}

BOOL _AvpWrite13(BYTE Disk, BYTE Head, DWORD Cylinder, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
{
	BOOL ret=0;
	void* buf=malloc(BufferSize);
	if(!buf)return 0;
	VWIN32_CopyMem(Buffer,buf,BufferSize,NULL);

	ret= IO13ios(3, Disk, Sector, Head, NumSectors, BufferSize, (BYTE*)buf);
	if (!ret)
		ret= IO13v86(3, Disk, Sector, Head, NumSectors, BufferSize, (BYTE*)buf);
	
	// this case - visnet na Me during cure MBR!!!
	/*ret= IO13v86(3, Disk, Sector, Head, NumSectors, BufferSize, (BYTE*)buf);
	if(!ret)
		ret= IO13ios(3, Disk, Sector, Head, NumSectors, BufferSize, (BYTE*)buf);*/
	
	free(buf);
	return ret;
}

BOOL _DiskIO(PDISK_IO pdio, BYTE* Buffer)
{
	switch(pdio->Func) {
	case 0x25:	//логический чтение
		return _AvpRead25(pdio->Drive, pdio->Sector, pdio->SectorCount, pdio->SizeOfBuffer, Buffer);
	case 0x26:	//логичесекий запись
		return _AvpWrite26(pdio->Drive, pdio->Sector, pdio->SectorCount, pdio->SizeOfBuffer, Buffer);
	case 0x13: // физический чтение
		return _AvpRead13(pdio->Drive, pdio->Head, pdio->Cylinder, pdio->Sector, pdio->SectorCount, pdio->SizeOfBuffer, Buffer);
	case 0x14: // физический запись
		return _AvpWrite13(pdio->Drive, pdio->Head, pdio->Cylinder, pdio->Sector, pdio->SectorCount, pdio->SizeOfBuffer, Buffer);
	}
	return FALSE;
}
