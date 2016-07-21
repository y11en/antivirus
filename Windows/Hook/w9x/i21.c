
// Special thanks a lot for authors of multimon & vshield

WORD	OldI21PMSegment;
DWORD	OldI21PMOffset;
WORD	OldI21V86Segment;
DWORD	OldI21V86Offset;
PMCall_THUNK PMCallThunk;
V86Call_THUNK V86CallThunk;
VMReturns_THUNK VMReturnsThunk;

//+ ------------------------------------------------------------------------------------------
ULONG gDriveFlags['Z' - 'A' + 1];

void ClearDriveFlags()
{
	int cou = 0;
	for (cou = 'A'; cou <= 'Z'; cou++)
		gDriveFlags[cou - 'A'] = 0;
}


ULONG __GetDriveType(BYTE DrvNum); //(00h = default, 01h = A:, etc)
//+ ------------------------------------------------------------------------------------------


BOOLEAN I21HookUsed=FALSE;

ULONG UniqueKey4Handle(ULONG handle,BOOL PM_Mode) 
{
ULONG Pref;
	Pref=(ULONG)Get_Cur_VM_Handle();
	if(Test_Sys_VM_Handle((VMHANDLE)Pref)) {
		Pref=(ULONG)VWIN32_GetCurrentProcessHandle();
		DbPrint(DC_I21,DL_SPAM, ("UniqueKey4Handle through hCurProc=%x hFile=%x PM=%d\n",Pref,handle,PM_Mode));
	}	else {
		DbPrint(DC_I21,DL_SPAM, ("UniqueKey4Handle through hCurVM=%x hFile=%x PM=%d\n",Pref,handle,PM_Mode));
	}
	Pref=(Pref<<12)+handle;
	if(PM_Mode)
		Pref=Pref | 0x80000000;
	else 
		Pref=Pref & 0x7fffffff;
	return Pref;
}

//2119D - DOS 1+ - GET CURRENT DEFAULT DRIVE
ULONG GetCurrDrv(PCHAR str,BOOL PM_Mode)
{
VMHANDLE			hVM;
CLIENT_STRUCT	SavedRegs;
CLIENT_STRUCT	*pRegs;

if(PM_Mode) {//!!!! Мать ети кривые дос-экстендеры !!!!!!!!!!!!!
	DbPrint(DC_I21,DL_WARNING, ("!!! GetCurrDrv Call in PM mode\n"));
	return 0;	
}

	hVM=Get_Cur_VM_Handle();
	pRegs=(CLIENT_STRUCT*)hVM->CB_Client_Pointer;
	Save_Client_State(&SavedRegs);
	if(PM_Mode)
		Begin_Nest_Exec();
	else
		Begin_Nest_V86_Exec();
	_clientAH=0x19;
	if(PM_Mode)
		Build_Int_Stack_Frame(OldI21PMSegment,OldI21PMOffset);
	else
		Build_Int_Stack_Frame(OldI21V86Segment,OldI21V86Offset);
	Resume_Exec();
	*(WORD*)str=(':'<<8) + _clientAL+'A';
	End_Nest_Exec();
	Restore_Client_State(&SavedRegs);
	return 2;
}

//2147D - DOS 2+ - "CWD" - GET CURRENT DIRECTORY
ULONG GetCurrDir(BYTE Drv,PCHAR str,BOOL PM_Mode)
{
VMHANDLE			hVM;
CLIENT_STRUCT	SavedRegs;
CLIENT_STRUCT	*pRegs;
DWORD				Actual;
SEGOFFSET		pBuffer;
DWORD				BuffSize=MAXPATHLEN;
WORD				CopySeg;
ULONG				ret;

	ret=0;//важно для V86MMGR_Free_Buffer
	*str=0;

if(PM_Mode) {
	DbPrint(DC_I21,DL_WARNING, ("!!! GetCurrDir Call in PM mode\n"));
	return ret;	
}

	hVM=Get_Cur_VM_Handle();
	pRegs=(CLIENT_STRUCT*)hVM->CB_Client_Pointer;
	Save_Client_State(&SavedRegs);
	if(V86MMGR_Allocate_Buffer(hVM,pRegs,BuffSize,0,NULL,FALSE,&Actual,&pBuffer)) {
		if(Actual>=BuffSize) {
			if(PM_Mode)
				Begin_Nest_Exec();
			else
				Begin_Nest_V86_Exec();
			_clientAX=0x7147;
			_clientDL=Drv;
			_clientDS=HIWORD(pBuffer);
			_clientSI=LOWORD(pBuffer);
			_clientFlags=_clientFlags | 1;
			if(PM_Mode) {
//				Build_Int_Stack_Frame(OldI21PMSegment,OldI21PMOffset);
			} else {
				Build_Int_Stack_Frame(OldI21V86Segment,OldI21V86Offset);
			}
			Resume_Exec();
			if(!(_clientFlags & 1)) {
				ret=1;//важно для V86MMGR_Free_Buffer
				*str='\\';
			} else {
				DbPrint(DC_I21,DL_WARNING, ("I2147 return carry flag\n"));
			}
			End_Nest_Exec();
		} else {
			DbPrint(DC_I21,DL_WARNING, ("V86MMGR_Allocate_Buffer - too small buffer\n"));
		}
		_asm {
			mov ax, ss;// str должна лежать на стеке
			mov CopySeg, ax;
		}
		V86MMGR_Free_Buffer(hVM,pRegs,Actual,CopySeg,str+ret,ret);
	} else {
		DbPrint(DC_I21,DL_WARNING, ("V86MMGR_Allocate_Buffer failed\n"));
	}
	Restore_Client_State(&SavedRegs);
	return strlen(str);
}

//!!!!!!!!!!!!! Имена типа NUL COM и т.п.EMMXXX0 и GetCurrDrv/GetCurrDir
PCHAR ConvertPathI21(PCHAR fullname,BYTE Seg,BYTE Off,BOOL PM_Mode)
{
PCHAR pName;
PCHAR pPath;
ULONG Len;
	pPath=fullname;
	*pPath=0;
	if((pName=Map_Flat(Seg,Off))==(PCHAR)0xffffffff) {
		*fullname=0;
		DbPrint(DC_I21,DL_ERROR, ("ConvertPathI21: Map_Flat failed\n"));
		DbgBreakPoint();				
		return fullname;
	}
	Len=strlen(pName);
	if(Len==0) {
		DbPrint(DC_I21,DL_WARNING, ("ConvertPathI21: empty name\n"));
		DbgBreakPoint();				
		return fullname;
	}

	if(Len<2 || ( *(pName+1)!=':' && *(WORD*)pName!='\\\\')) {
		pPath+=GetCurrDrv(pPath,PM_Mode);
	}

	if(*pName!='\\' && (Len<3 || *(WORD*)(pName+1)!='\\:')) {
		Len=GetCurrDir(0,pPath,PM_Mode);
		pPath+=Len;
		if(Len && *(pPath-1)!='\\')
			*pPath++='\\';
	}

	strcpy(pPath,pName);

	{
		_QWORD ResultDec;
		THREADHANDLE hThread;
		hThread = Get_Cur_Thread_Handle();
		if(((PTCB)(hThread))->TCB_Flags & THFLAG_OEM) {
			BcsToBcs(fullname, fullname, BCS_WANSI, BCS_OEM, strlen(fullname)+1, &ResultDec);
		}
	}
	return fullname;
}

#include <pshpack1.h>
typedef struct _EXEC_STATE {
	WORD	reserved;
	WORD	type;
	DWORD name;
	WORD	PSPseg;
	DWORD	start_addr;
	DWORD	prog_size;
}EXEC_STATE,*PEXEC_STATE;
#include <poppack.h>

PCHAR ConvertPath4Exec5(PCHAR fullname,PCLIENT_STRUCT pcrs,BOOL PM_Mode)
{
PEXEC_STATE	es;
PCHAR			Name;
VMHANDLE		hVM;
	hVM=Get_Cur_VM_Handle();
	es=Map_Flat(CLIENT_DS,CLIENT_DX);
	if(PM_Mode)
		Name=MapPointerToFlat(HIWORD(es->name),LOWORD(es->name),hVM);
	else
		Name=(PCHAR)(hVM->CB_High_Linear + (HIWORD(es->name)<<4) + LOWORD(es->name));
	strcpy(fullname,Name);
	return fullname;
}

#include <pshpack1.h>
typedef struct _FCB {
	BYTE	DrvNum;
	CHAR	FileName[8];
	CHAR	FileExt[3];
	WORD	CurrBlock;
	WORD	RecSize;
	DWORD	FileSize;
	WORD	Date;
	WORD	Time;
	BYTE	NumInSFT;
	BYTE	Attr;
	BYTE	Reserved[6];
	BYTE	RecInCurrBlk;
	DWORD RndAccessRecNum;
} FCB,*PFCB;
#include <poppack.h>

PCHAR GetFNameFCB(PCHAR fullname,BYTE Seg,BYTE Off,BOOL PM_Mode)
{
PFCB fcb;
ULONG i;
PCHAR pPath;
ULONG Len;
	pPath=fullname;
	fcb=Map_Flat(Seg,Off);
	if(fcb->DrvNum) {
		*(WORD*)pPath=(':'<<8) + fcb->DrvNum+'@';
		pPath+=2;
	} else {
		pPath+=GetCurrDrv(pPath,PM_Mode);
	}
	Len=GetCurrDir(fcb->DrvNum,pPath,PM_Mode);
	pPath+=Len;
	if(Len && *(pPath-1)!='\\')
		*pPath++='\\';
	for(i=0;i<8;i++) {
		if(fcb->FileName[i]!=' ')
			*pPath++=fcb->FileName[i];
		else
			break;
	}
	*pPath++='.';
	for(i=0;i<3;i++) {
		if(fcb->FileExt[i]!=' ')
			*pPath++=fcb->FileExt[i];
		else
			break;
	}
	*pPath=0;
	return fullname;
}

VERDICT I21Dispatch(PCHAR fullPathName, PCLIENT_STRUCT pcrs, BOOL PM_Mode)
{
	VERDICT		Verdict = Verdict_NotFiltered;
	PCHAR		Action;
	CHAR		ClientAL;
	
	BOOLEAN bOverBuf;

	DWORD		FunctionMj = 0;
	DWORD		FunctionMi = 0;

	BYTE		*RequestData;
	PSINGLE_PARAM pSingleParam;
	PFILTER_EVENT_PARAM pParam;

	ULONG drvflg = 0;

	//! sdesa remote flag attach on url
	ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN + 1;
	ReqDataSize += sizeof(SINGLE_PARAM) + sizeof(ULONG);	// drive flags

	FunctionMj = pcrs->CBRS.Client_AH;
	ClientAL=pcrs->CBRS.Client_AH;
	if(ClientAL==0x44 //IOCTL
		|| ClientAL==0x71//LFN
		|| ClientAL== 0x73//FAT32
		)
		FunctionMi = pcrs->CBRS.Client_AL;
	else
		FunctionMi = 0;

	if(IsNeedFilterEvent(FLTTYPE_I21, FunctionMj, FunctionMi) == FALSE){
		if(FunctionMj == 0x3e) //CLOSE
			NameCacheFree(g_pCommon_NameCache, UniqueKey4Handle(pcrs->CWRS.Client_BX, PM_Mode));
		return Verdict;
	}
	
	RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
	if (RequestData == NULL)
		return Verdict;
	pParam = (PFILTER_EVENT_PARAM) RequestData;

	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_I21, FunctionMj, FunctionMi, PreProcessing, 2);

	pSingleParam = (PSINGLE_PARAM) pParam->Params;
	SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);

	strcpy(fullPathName,UnknownStr);
	switch(pcrs->CBRS.Client_AH) {
	case 0x0f://OPEN FCB
	case 0x10://CLOSE FCB
	case 0x11://FINDFIRST FCB
	case 0x13://DELETE FCB
	case 0x14://SEQUENTIAL READ FCB
	case 0x15://SEQUENTIAL WRITE FCB
	case 0x16://CREATE FCB
	case 0x21://READ RECORD FCB
	case 0x22://WRITE RECORD FCB
	case 0x27://READ BLOCK FCB
	case 0x28://WRITE BLOCK FCB
		GetFNameFCB(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
		break;
	case 0x17://RENAME FCB
		GetFNameFCB(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
		//!! Second Name
		break;
	case 0x46://FORCE DUPLICATE FILE HANDLE
		//???????????????????
		break;
	case 0x4b://EXEC
		if(pcrs->CBRS.Client_AL==5) {//SET EXECUTION STATE
			ConvertPath4Exec5(fullPathName,pcrs,PM_Mode);
			break;
		} else if(pcrs->CBRS.Client_AL>4 || pcrs->CBRS.Client_AL==2)
			break;
		// else ниже!
	case 0x39://MKDIR
	case 0x3a://RMDIR
	case 0x3b://CHDIR
	case 0x3c://CREATE
	case 0x3d://OPEN
	case 0x41://DELETE
	case 0x43://FILE ATTRIBUTES
	case 0x4e://FINDFIRST
	case 0x5a://CREATE TEMP
	case 0x5b://CREATE NEW
		ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
		break;
	case 0x3e://CLOSE
	case 0x3f://READ
	case 0x40://WRITE
	case 0x42://LSEEK
	case 0x45://DUPLICATE FILE HANDLE
	case 0x57://FILE'S DATE-TIME
	case 0x5c://FLOCK
	case 0x68://FFLUSH
	case 0x6a://COMMIT
		bOverBuf = FALSE;
		Action=NameCacheGet(g_pCommon_NameCache, UniqueKey4Handle(pcrs->CWRS.Client_BX,PM_Mode),fullPathName, MAXPATHLEN, &bOverBuf);
		if((pcrs->CBRS.Client_AH==0x3e) && Action) //CLOSE
			NameCacheFree(g_pCommon_NameCache, UniqueKey4Handle(pcrs->CWRS.Client_BX,PM_Mode));
		break;
	case 0x56://RENAME
		ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
//!!!!		strcat(fullPathName," to ");
//!!!!		ConvertPathI21(fullPathName+strlen(fullPathName),CLIENT_ES,CLIENT_DI,PM_Mode);
		break;
	case 0x6c://EXT OPEN/CREATE
		ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_SI,PM_Mode);
		break;
	case 0x44://IOCTL
		switch(pcrs->CBRS.Client_AL) {
		case 0x0b://SET SHARING RETRY COUNT
			*fullPathName=0;
			break;
		case 0x00://GET DEV INFO
		case 0x01://SET DEV INFO
		case 0x02://READ CHAR DEV
		case 0x03://WRITE CHAR DEV
		case 0x06://GET INPUT STATUS
		case 0x07://GET OUTPUT STATUS
		case 0x0a://CHECK IF HANDLE IS REMOTE
		case 0x0c://GENERIC CHARACTER DEVICE REQUEST
		case 0x10://QUERY GENERIC IOCTL CAPABILITY (HANDLE)
			bOverBuf = FALSE;
			NameCacheGet(g_pCommon_NameCache, UniqueKey4Handle(pcrs->CWRS.Client_BX,PM_Mode),fullPathName, MAXPATHLEN, &bOverBuf);
			break;
		case 0x04://READ BLOCK DEV
		case 0x05://WRITE BLOCK DEV
		case 0x08://CHECK IF BLOCK DEVICE REMOVABLE
		case 0x09://CHECK IF BLOCK DEVICE REMOTE
		case 0x0d://GENERIC BLOCK DEVICE REQUEST
		case 0x0e://GET LOGICAL DRIVE MAP
		case 0x0f://SET LOGICAL DRIVE MAP
		case 0x11://QUERY GENERIC IOCTL CAPABILITY (DRIVE)
			if(pcrs->CBRS.Client_BL==0)
				strcpy(fullPathName,"DefaultDrive");
			else {
				*fullPathName=pcrs->CBRS.Client_BL+'@';
				*(WORD*)(fullPathName+1)=':';
			}
		}
		break;
	case 0x71://LFN
		switch(pcrs->CBRS.Client_AL) {
		case 0x39://MKDIR
		case 0x3a://RMDIR
		case 0x3b://CHDIR
		case 0x41://DELETE
		case 0x43://FILE ATTRIBUTES
		case 0x4e://FINDFIRST
			ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
			break;
		case 0x56://RENAME
			ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
//!!!!!!!!!!!			strcat(fullPathName," to ");
//!!!!!!!!!!!			ConvertPathI21(fullPathName+strlen(fullPathName),CLIENT_ES,CLIENT_DI,PM_Mode);
			break;
		case 0x6c://EXT OPEN/CREATE
			ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_SI,PM_Mode);
			break;
		default:
			fullPathName[0]=0;
		}
		break;
	case 0x73://FAT32
		switch(pcrs->CBRS.Client_AL) {
		case 0x05://EXTENDED ABSOLUTE DISK READ/WRITE
			if(pcrs->CWRS.Client_CX!=0xffff)
				break;
		case 0x00://DRIVE LOCKING GET
		case 0x01://DRIVE LOCKING SET
		case 0x02://GET EXTENDED DPB
		case 0x04://SET DPB TO USE FOR FORMATTING
			if(pcrs->CBRS.Client_DL==0)
				strcpy(fullPathName,"DefaultDrive");
			else {
				*fullPathName=pcrs->CBRS.Client_DL+'@';
				*(WORD*)(fullPathName+1)=':';
			}
			break;
		case 0x03://GET EXTENDED FREE SPACE ON DRIVE
			ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_DX,PM_Mode);
			break;
		}
		break;
	default:
		fullPathName[0]=0;
	}

	if (fullPathName[0] != 0)
	{
		if (fullPathName[0] == '\\')
		{
			drvflg |= _CONTEXT_OBJECT_FLAG_SRCREMOTE;
		}
		else
		{
			BYTE number = fullPathName[0];
			number |= 0x20;
			if (number >= 'a' && number <= 'z')
			{
				if (!gDriveFlags[number - 'a'])
				{
					if ((0x71 == FunctionMj) && (FunctionMi == 0x4e))
					{
						DbPrint(DC_SYS, DL_NOTIFY, ("__GetDriveType for cache"));
						gDriveFlags[number - 'a'] = __GetDriveType((BYTE) (number - 'a' + 1));
					}
				}

				if (DRIVE_REMOTE == gDriveFlags[number - 'a'])
					drvflg |= _CONTEXT_OBJECT_FLAG_SRCREMOTE;
			}

		}
	}

#ifdef __DBG__
if(fullPathName[0]!=0) {
	Action=HeapAllocate(MAX_MSG_LEN,0);
	if(Action) {
		DbPrint(DC_I21,DL_INFO, ("%c: %s %s %s\n",PM_Mode?'p':'v',GetProcName(pParam->ProcName, NULL),VxDGetFunctionStr(Action,pParam->HookID, pParam->FunctionMj, pParam->FunctionMi),fullPathName));
		HeapFree(Action,0);
	} else {
		DbPrint(DC_I21,DL_INFO, ("%c: %s %s %s\n",PM_Mode?'p':'v',GetProcName(pParam->ProcName, NULL),"--NO-MEM--",fullPathName));
	}
}
#endif //__DBG__

	if(fullPathName[0] != 0)
	{
		pSingleParam->ParamSize = strlen(fullPathName)+1;
		memcpy(pSingleParam->ParamValue, fullPathName, pSingleParam->ParamSize);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, drvflg);

		Verdict = FilterEvent(pParam, NULL);
	}
	ExFreePool(RequestData);
	return Verdict;
}

typedef struct _I21_CALLBACK_REF {
	DWORD Func;
	BOOL PM_Mode;
	CHAR Name[1];
}I21_CALLBACK_REF,*PI21_CALLBACK_REF;

PI21_CALLBACK_REF I21IsNeedPostProc(PCHAR fullPathName,PCLIENT_STRUCT pcrs,BOOL PM_Mode)
{
ULONG NameLen;
PI21_CALLBACK_REF	ref=NULL;
	switch(pcrs->CBRS.Client_AH) {
	case 0x71://LFN
		if(pcrs->CBRS.Client_AL!=0x6c) //EXT OPEN/CREATE)
			break;
	case 0x3c://CREATE
	case 0x3d://OPEN
	case 0x10://close fcb
	case 0x3e://CLOSE
	case 0x45://DUPLICATE FILE HANDLE
	case 0x5a://CREATE TEMP
	case 0x5b://CREATE NEW
	case 0x6c://EXT OPEN/CREATE
		NameLen=strlen(fullPathName);
		if(NameLen) {
			ref=HeapAllocate(NameLen+sizeof(I21_CALLBACK_REF),0);
			if(ref) {
				ref->Func=pcrs->CBRS.Client_AH;
				ref->PM_Mode=PM_Mode;
				strcpy(ref->Name,fullPathName);
			}
		}
	}
	return ref;
}

VOID __stdcall I21PostProcCallback(VMHANDLE hVM,PI21_CALLBACK_REF RefData,PCLIENT_STRUCT pcrs,DWORD Flags) 
{
	DbPrint(DC_I21,DL_INFO, ("I21PostProcCallback CarryFlg=%x AX=%x Func=%x Name %s\n",pcrs->CWRS.Client_Flags & 1,pcrs->CWRS.Client_AX,RefData->Func,RefData->Name));
	if(RefData->Func==0x3e /*CLOSE*/) {
		BYTE		*RequestData;
		PSINGLE_PARAM pSingleParam;
		PFILTER_EVENT_PARAM pParam;
		ULONG ReqDataSize = sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) + MAXPATHLEN + 1;
		//! sdesa remote flag attach on url
		RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, 'RboS');
		if(RequestData) {
			pParam = (PFILTER_EVENT_PARAM) RequestData;
			
			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_I21, RefData->Func, 0, PostProcessing, 1);

			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
			if(RefData->Name[0]!=0) {
				pSingleParam->ParamSize = strlen(RefData->Name)+1;
				memcpy(pSingleParam->ParamValue, RefData->Name, pSingleParam->ParamSize);
				FilterEvent(pParam, NULL);
			}
			ExFreePool(RequestData);
		}
	} else { //here are CREATE operations HandleStorage
		if(!(pcrs->CWRS.Client_Flags & 1)) {
			if(RefData->Func == 0x5a /*CREATE TEMP*/) {
				CHAR		fullPathName[MAXPATHLEN];
				fullPathName[0]=0;
				ConvertPathI21(fullPathName,CLIENT_DS,CLIENT_DX,RefData->PM_Mode);
				NameCacheStore(g_pCommon_NameCache, UniqueKey4Handle(pcrs->CWRS.Client_AX,RefData->PM_Mode),fullPathName,strlen(fullPathName)+1, FALSE, 'AboS');
			} else {
				NameCacheStore(g_pCommon_NameCache, UniqueKey4Handle(pcrs->CWRS.Client_AX,RefData->PM_Mode),RefData->Name,strlen(RefData->Name)+1, FALSE, 'AboS');
			}
		}
	}
	HeapFree(RefData,0);
}

void I21HookProc(PCLIENT_STRUCT pcrs,BOOL PM_Mode)
{
VERDICT	Verdict;
PI21_CALLBACK_REF	ref;
CHAR		fullPathName[MAXPATHLEN];
BYTE		fn;
	if(I21HookUsed) {
		fn=pcrs->CBRS.Client_AH;
		fullPathName[0]=0;
		Verdict=I21Dispatch(fullPathName,pcrs,PM_Mode);
		if(Verdict==Verdict_Discard && 
			(fn==0x10 //CLOSE FCB 
			|| fn==0x3e	//CLOSE
			)) {
			DbPrint(DC_I21,DL_INFO, ("%c: Verdict changed to Pass for close operation\n",PM_Mode?'p':'v'));
			Verdict=Verdict_Pass;
		} else if(!_PASS_VERDICT(Verdict)) {
		/*		switch(pcrs->CBRS.Client_AH) {
		default:
		;
		}*/
			if(Verdict == Verdict_Kill) {
				DbPrint(DC_I21,DL_NOTIFY, ("Kill\n"));
				KillCurrentProcess();
			} else if(Verdict == Verdict_Discard) {
				DbPrint(DC_I21,DL_NOTIFY, ("Discard\n"));
			}
			pcrs->CWRS.Client_AX=5;//access denied
			pcrs->CWRS.Client_Flags |= 1;
			Enable_VM_Ints();
			Simulate_Far_Ret_N(2);
			return;
		}
		if((ref=I21IsNeedPostProc(fullPathName,pcrs,PM_Mode))) {
			Simulate_Iret();
			Call_When_VM_Returns(I21PostProcCallback,(VOID*)ref,0,&VMReturnsThunk);
			if(PM_Mode)
				Build_Int_Stack_Frame(OldI21PMSegment,OldI21PMOffset);
			else
				Build_Int_Stack_Frame(OldI21V86Segment,OldI21V86Offset);
			return;
		}
	}
	if(PM_Mode)
		Simulate_Far_Jmp(OldI21PMSegment,OldI21PMOffset);
	else
		Simulate_Far_Jmp(OldI21V86Segment,OldI21V86Offset);
}

VOID __stdcall I21PMHookProc(VMHANDLE hVM, PVOID RefData, PCLIENT_STRUCT pcrs)
{
	I21HookProc(pcrs,TRUE);
}

VOID __stdcall I21V86HookProc(VMHANDLE hVM, PVOID RefData, PCLIENT_STRUCT pcrs)
{
	I21HookProc(pcrs,FALSE);
}


BOOLEAN I21Hooked=FALSE;

BOOLEAN I21Hook(VOID)
{
ULONG CallBackAddr;
#ifdef __DBG__
VMHANDLE CurVM;
	CurVM=Get_Cur_VM_Handle();
	DbPrint(DC_I21,DL_INFO, ("Current VM%d%s\n",CurVM->CB_VMID,Test_Sys_VM_Handle(CurVM)?" - System":""));
#endif //__DBG__
	
	ClearDriveFlags();

	if(!I21Hooked) {
		I21Hooked=TRUE;
		Get_PM_Int_Vector(0x21,&OldI21PMSegment,&OldI21PMOffset);
		if(CallBackAddr=Allocate_PM_Call_Back(NULL,I21PMHookProc,&PMCallThunk)) {
			Set_PM_Int_Vector(0x21,(WORD)(CallBackAddr >> 0x10),CallBackAddr & 0xffff);
			DbPrint(DC_I21,DL_IMPORTANT, ("Int21 PM Hooked\n"));
		} else {
			DbPrint(DC_I21,DL_ERROR, ("Allocate_PM_Call_Back failed\n"));
		}
		
		Get_V86_Int_Vector(0x21,&OldI21V86Segment,&OldI21V86Offset);
		if(CallBackAddr=Allocate_V86_Call_Back(NULL,I21V86HookProc,&V86CallThunk)) {
			Set_V86_Int_Vector(0x21,(WORD)(CallBackAddr >> 0x10),CallBackAddr & 0xffff);
			DbPrint(DC_I21,DL_IMPORTANT, ("Int21 V86 Hooked\n"));
		} else {
			DbPrint(DC_I21,DL_ERROR, ("Allocate_V86_Call_Back failed\n"));
		}
	}
	return TRUE;
}

BOOLEAN I21UnHook(VOID)
{
	if(I21Hooked) {
		Set_V86_Int_Vector(0x21,OldI21V86Segment,OldI21V86Offset);
		DbPrint(DC_I21,DL_IMPORTANT, ("Int21 V86 Unhooked\n"));
		Set_PM_Int_Vector(0x21,OldI21PMSegment,OldI21PMOffset);
		DbPrint(DC_I21,DL_IMPORTANT, ("Int21 PM Unhooked\n"));
		I21Hooked=FALSE;
	}
	return TRUE;
}
