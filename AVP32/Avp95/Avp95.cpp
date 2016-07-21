////////////////////////////////////////////////////////////////////
//
//  AVP95.CPP
//  Engine VxD Wrapper, contains all entry points to the VxD.
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#define DEVICE_MAIN
//#define LAYERED_DRIVER
#include "stdafx.h"

#ifdef LAYERED_DRIVER
Declare_Layered_Driver(AVP95, DRP_MISC_PD,"AVP_SCAN_driver", 1, 0, 0)
#else
Declare_Virtual_Device(AVP95)
#endif

#undef DEVICE_MAIN

extern "C" size_t wcslen( const char *string ) {return strlen (string);};
extern "C" char*  wcscpy( char* dest, const char *src ) {return strcpy (dest,src);};

#undef memmove
extern "C" void* memmove( void *dest, const void *src, size_t count ){return memcpy(dest,src,count);};

extern char AVPVnd_Name[];

static DWORD APC_CallbackProcPtr=0;
static THREADHANDLE APC_VmThreadHandle=0;

static AVPCallback     fAVPCallback=0;        // AVP callback function pointer
static VMHANDLE MyVM;
static int Infected; 
int Ready=0; 
static int UseIFSMGR=0;
static DWORD Enable=1;
static DWORD StaticMFlags=0;
static DWORD LimitCompoundSize=0;
static DWORD StopCompoundAfterFirstDetection=1;

DWORD MFlags=0x08150002;

AVPStat* Stat=NULL;
AVPBaseInfo* BaseInfo=NULL;

static char* MessText[MESS_TEXT_MAX];

#define MYSTACKSIZE 0x8000
static MEMHANDLE hMem;	// Variable to hold memory handle returned
static PVOID LinAddr;	// Variable to hold linear address returned

						
static int Busy=0; 
static VMutex	mBusy;
static VMutex	mRegThread;
	
BOOL WaitIfBusy()
{
	Busy++;
	mBusy.enter();
	return TRUE;
}

void ReleaseWait()
{
	mBusy.leave();
	Busy--;
}
/*
#ifdef DEBUGPRINT
void printstack()
{
	WORD _ss;
	DWORD _sp;
	_asm{
		push eax;
		mov ax,ss;
		mov _ss,ax;
		mov eax,esp;
		mov _sp,eax;
		pop eax;
	}
    dprintf("-->STACK %04X:%08X",_ss,_sp);
}
#endif
*/


void** vndArray=NULL;
int vndArrayCount=0;

static void VndDestroy()
{
	if(vndArray){
		for(int i=0;i<vndArrayCount;i++)
			delete vndArray[i];
		delete vndArray;
	}
	vndArrayCount=0;
	vndArray=NULL;
}

void* _cdecl nt_malloc(unsigned size_){
    return new BYTE[size_];
}

void __cdecl nt_free(void* ptr_){
	delete ptr_;
}


static BOOL VndInit( const char *filename )
{
	BOOL ret=FALSE;
	VndDestroy();
	
#ifdef DEBUG
__asm int 3;
FPI_CustomSettings("MFlags=0x08950002; damn!");
#endif


	if(filename==NULL) return FALSE;
	if(sign_check_file(filename,1,0,0,0)) return FALSE;
	if(!KLDT_Init_Library(&nt_malloc,&nt_free)) return FALSE;
	if(!DATA_Init_Library(&nt_malloc,&nt_free)) return FALSE;

	HDATA hRootData=NULL;
	if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return FALSE;
	if(hRootData==NULL) return FALSE;
	MAKE_ADDR1( hKernelAddr, AVP_PID_VNDKERNELDATA );

	HPROP hKernelProp;
	hKernelProp = DATA_Find_Prop( DATA_Find(hRootData, hKernelAddr), NULL, 0 );

	// Получить	количество элементов в массиве
	AVP_dword nCount;
	nCount = PROP_Arr_Count( hKernelProp );

	vndArray= (void**)new DWORD[nCount];
	if(vndArray==NULL)goto dr;
	vndArrayCount=nCount;

	// Сканировать массив
	AVP_dword i;
	for ( i=0; i<nCount; i++ ) {
		AVP_dword nItemSize = PROP_Arr_Get_Items( hKernelProp, i, 0, 0 );
		AVP_char *pBuffer = new AVP_char[nItemSize];
		if(pBuffer==NULL)break;
		PROP_Arr_Get_Items( hKernelProp, i, pBuffer, nItemSize );
		vndArray[i]=pBuffer;
	}
dr:
	// Удаление дерева
	DATA_Remove( hRootData, 0 );
	return ret;

}



static BOOL CharToOem(LPCTSTR s,LPSTR d)
{
	WORD UniStr[0x200];
	_QWORD res;
	if(!s || !d) return FALSE;
	int l=strlen(s);
	if(l>0x1FF)l=0x1FF;
	BCSToUni(UniStr,(UCHAR*)s,l,BCS_WANSI,&res);
	UniToBCS((UCHAR*)d,UniStr,l*2,0x200,BCS_OEM,&res);
	d[l]=0;

	return TRUE;
}

#ifdef FPIAPI_VER_MAJOR
DWORD FPI_InfectionReport( DWORD action, DWORD flags, char* line, DWORD ptr)
{
#ifdef DEBUG
//	__asm int 3;
#endif
		if(FPI_report && ptr)
		{
			char s[0x200];
			sprintf(s,"\001%s\001",line);
			return FPI_report(FPIEVENT_INFECTION, (WORD)action , flags, s, ptr);
		}
		else return 0;
}
#endif

DWORD CurRefdata=0;

BOOL _QueryAbort(DWORD refdata_);

DWORD _Sleep(){
	Enable_VM_Ints();	// Optional, allows hardware interrupts (like STI)
	Begin_Nest_Exec();	// Create nested execution context
	Resume_Exec();	// Ring-3 transfer allows scheduler to run
	End_Nest_Exec();	// Terminate nested block 
#ifdef FPIAPI_VER_MAJOR
	return (DWORD)_QueryAbort(0);
#endif
	return 0;
}


LRESULT WINAPI _AvpCallbackRedir(WPARAM wParam,LPARAM lParam)
{
//	static char buf[512];
	DWORD r;
	static char s[512];
	AVPScanObject* so=(AVPScanObject*)lParam;
	LRESULT ret=0;

#ifdef DEBUGPRINT_CALLBACKS
	if(so)
    dprintf("AVPCallback %X: %08X %s (%s)", wParam, so->RFlags, so->Name, lParam );
#endif

    switch(wParam){

	case AVP_CALLBACK_OBJ_NAME:
		goto ret2;
	case AVP_CALLBACK_LOAD_PROGRESS:
		{
		LOAD_PROGRESS_STRUCT* lps=(LOAD_PROGRESS_STRUCT*)lParam;
//		strncpy(BaseInfo->SetName, lps->setName,0x200);
		BaseInfo->NumberOfVirNames=lps->virCount;
		BaseInfo->NumberOfRecords=lps->recCount;
        BaseInfo->NumberOfBases = lps->NumberOfBases;
		BaseInfo->LastUpdateYear = lps->LastUpdateYear;
		BaseInfo->LastUpdateMonth = lps->LastUpdateMonth;
		BaseInfo->LastUpdateDay = lps->LastUpdateDay;
		BaseInfo->LastUpdateTime = lps->LastUpdateTime;
		}
// No break! rotate.

	case AVP_CALLBACK_ROTATE: 
	case AVP_CALLBACK_ROTATE_OFF:
/*
		{
			static DWORD loTimeRotate=0;
			DWORD loTime = Get_Last_Updated_System_Time();
			if(loTimeRotate==loTime) return 0;
			loTimeRotate = loTime;
		}
*/
		if(_Sleep())	return 1;
		goto ret2;

	case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
		ret=sign_check_file((char*)lParam,1,vndArray, vndArrayCount, 0);
#ifdef DEBUGPRINT
		    dprintf("AVP_Sign_Check_INtegrity %d: %s", ret, lParam );
#endif
		break;

	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
#ifdef FPIAPI_VER_MAJOR
	        doerrormsg(  FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
#endif
integ_fail:
			if(APC_CallbackProcPtr){
				AVPScanObject* nso=new AVPScanObject;
				memset(nso,0,sizeof(AVPScanObject));
				strcpy(nso->Name,(char*)lParam);
				nso->RFlags = RF_REPORT_DONE;
				nso->Type = 0x8001;
				nso->Connection=ret;
				VWIN32_QueueUserApc((PVOID)APC_CallbackProcPtr,(DWORD)nso,APC_VmThreadHandle);
			}
		goto ret2;

	case AVP_CALLBACK_SETFILE_INTEGRITY_FAILED:
#ifdef FPIAPI_VER_MAJOR
//	        doerrormsg(  FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
#endif
		goto integ_fail;

	case AVP_CALLBACK_ERROR_BASE:
#ifdef FPIAPI_VER_MAJOR
	        doerrormsg(  FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
#endif
		goto ret2;
	case AVP_CALLBACK_ERROR_FILE_OPEN:
/*		
#ifdef FPIAPI_VER_MAJOR
	        doerrormsg(   FPIERROR_FILEOPEN, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
#endif
		goto ret2;
*/

	case AVP_CALLBACK_ERROR_SET:
#ifdef FPIAPI_VER_MAJOR
	        doerrormsg(   FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION, BaseInfo->SetName);
#endif
		goto ret2;
	case AVP_CALLBACK_ERROR_KERNEL_PROC:
	case AVP_CALLBACK_ERROR_LINK:
#ifdef FPIAPI_VER_MAJOR
	        doerrormsg(    FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
#endif
		goto ret2;

	case AVP_CALLBACK_MB_OK:
	case AVP_CALLBACK_MB_OKCANCEL:
	case AVP_CALLBACK_MB_YESNO:
	case AVP_CALLBACK_PUT_STRING: 

	case AVP_CALLBACK_GET_SECTOR_IMAGE:
ret2:
	    if(fAVPCallback)  ret=fAVPCallback(wParam, (void*)lParam, NULL);
		break;

	case AVP_CALLBACK_ASK_DELETE:
		if(StaticMFlags && so)
		switch((so->MFlags) & MF_D_)
		{
			case MF_D_ASKCURE:
			case MF_D_ASKCUREDEBUG:
			case MF_D_CUREALL:
				if(so->MFlags & MF_FPI_MODE
					|| (!(so->MFlags & MF_APC_CURE_REQUEST)
					&& (so->SType == OT_FILE)))
				{
					so->RFlags|=RF_CURE_FAIL;
					return 0;
				}
				return 2;
			case MF_D_DELETEALL:
				return 2;
			case MF_D_DENYACCESS:
			default:
				return 0;
		}
		goto ret3;

	case AVP_CALLBACK_ASK_CURE:
#ifdef DEBUGPRINT
    dprintf("AVP_CALLBACK_ASK_CURE");
#endif
#ifdef FPIAPI_VER_MAJOR
		if(FPI_report && so && so->Connection)
			if (FPI_report(FPIEVENT_QUERY, FPIACTION_DISINFECT , FPIFLAG_MSGEXTRA, Stat->LastVirusName, so->Connection))
				return 1;
#endif
		if(StaticMFlags && so)
			switch((so->MFlags) & MF_D_)
			{
			case MF_D_ASKCURE:
			case MF_D_ASKCUREDEBUG:
				sprintf(s,MessText[0]?MessText[0]:"AVP Monitor\n\n\n\n\n\nObject    %s\n Infected by virus:  %s\n\nTry to disinfect?",
					strlen(so->Name)>60?(so->Name +strlen(so->Name) -60):
					so->Name,Stat->LastVirusName);
				CharToOem(s,s);
				r=SHELL_SYSMODAL_Message(MyVM, MB_SYSTEMMODAL|MB_DEFBUTTON2|(
					(((so->MFlags) & MF_D_)==MF_D_ASKCURE)?MB_YESNO:MB_YESNOCANCEL),
					s, NULL);

				if(r==IDCANCEL){
					Infected--;
					return 0;
				}
				if(r==IDYES)return 1;
				return 0;
			case MF_D_CUREALL:
				return 1;
			case MF_D_DELETEALL:
				return 2;
			case MF_D_DENYACCESS:
			default:
				return 0;
			}

		goto ret3;
	case AVP_CALLBACK_OBJECT_WARNING:
		if(!(so->MFlags & MF_APC_CURE_REQUEST))
			Stat->Warnings++;
		BAvpMakeFullName(Stat->LastVirusName,so->WarningName);
		strcpy(Stat->LastInfectedObjectName,so->Name);
		if(StaticMFlags)
		switch((so->MFlags) & MF_D_)
		{
		case MF_D_ASKCURE:
		case MF_D_ASKCUREDEBUG:
			sprintf(s,MessText[2]?MessText[2]:"AVP Monitor\n\n\n\n\n\nObject    %s\n Infected by new variant of virus:  %s\n\nDeny Access?",
				strlen(so->Name)>60?(so->Name +strlen(so->Name) -60):
				so->Name,Stat->LastVirusName);
			CharToOem(s,s);
			r=SHELL_SYSMODAL_Message(MyVM, MB_SYSTEMMODAL|MB_YESNO, s, NULL);
			if(r!=IDYES) break;
		default:
			Infected++;
			break;
		}
#ifdef DEBUGPRINT
		dprintf("Object %s             WARNING detected:  %s",so->Name, Stat->LastVirusName);
#endif
		goto ret3;
	case AVP_CALLBACK_OBJECT_SUSP:
		if(!(so->MFlags & MF_APC_CURE_REQUEST))
			Stat->Suspicions++;
		BAvpMakeFullName(Stat->LastVirusName,so->SuspicionName);
		strcpy(Stat->LastInfectedObjectName,so->Name);
		if(StaticMFlags)
		switch((so->MFlags) & MF_D_)
		{
		case MF_D_ASKCURE:
		case MF_D_ASKCUREDEBUG:
			sprintf(s,MessText[1]?MessText[1]:"AVP Monitor\n\n\n\n\n\nObject    %s\n Suspicted for virus:  %s\n\nDeny Access?",
				strlen(so->Name)>60?(so->Name +strlen(so->Name) -60):
				so->Name,Stat->LastVirusName);
			CharToOem(s,s);
			r=SHELL_SYSMODAL_Message(MyVM, MB_SYSTEMMODAL|MB_YESNO, s, NULL);
			if(r!=IDYES) break;
		default:
			Infected++;
			break;
		}
#ifdef DEBUGPRINT
		dprintf("Object %s             SUSPICION detected:  %s",so->Name, Stat->LastVirusName);
#endif
		goto ret3;
	case AVP_CALLBACK_OBJECT_DETECT:
		if(!(so->MFlags & MF_APC_CURE_REQUEST))
			Stat->Infected++;
		Infected++;
		BAvpMakeFullName(Stat->LastVirusName,so->VirusName);
		strcpy(Stat->LastInfectedObjectName,so->Name);
#ifdef DEBUGPRINT
		dprintf("Object %s             VIRUS detected:  %s",so->Name, Stat->LastVirusName);
#endif
		goto ret3;

	case AVP_CALLBACK_OBJECT_CURE:
		Stat->Disinfected++;
		Infected--;
		goto ret3;

	case AVP_CALLBACK_OBJECT_CURE_FAIL:
		goto ret3;


	case AVP_CALLBACK_OBJECT_DELETE:
		Stat->Deleted++;
		Infected--;
		goto ret3;

	case AVP_CALLBACK_OBJECT_DONE:
#ifdef DEBUGPRINT
		dprintf("Object %s             process   done:  %08X  %08X",so->Name, so->RFlags, so->MFlags);
#endif
		if(so->RFlags & RF_REPORT_DONE)
			goto ret3;
		if(!((so->RFlags) & (RF_ERROR|RF_NOT_A_PROGRAMM|RF_NOT_OPEN)))
		{
			strcpy(Stat->LastObjectName,so->Name);
			if(!(so->MFlags & MF_APC_CURE_REQUEST))
				Stat->Objects++;
		}
		if(so->RFlags & (RF_DETECT|RF_WARNING|RF_SUSPIC))
		{
			if(!(so->InArc && (so->RFlags & (RF_CURE_FAIL|RF_DELETE)))){
				if(StopCompoundAfterFirstDetection)
					BAvpCancelProcessObject(1);

				if(APC_CallbackProcPtr ){
					AVPScanObject* nso=new AVPScanObject;
					memcpy(nso,so,sizeof(AVPScanObject));
					VWIN32_QueueUserApc((PVOID)APC_CallbackProcPtr,(DWORD)nso,APC_VmThreadHandle);
				}
			}

#ifdef FPIAPI_VER_MAJOR
			int action=FPIACTION_NONE;
			int flag=FPIFLAG_MSGEXTRA;

			if(so->RFlags & RF_DELETED){
				action=FPIACTION_DISINFECT;
#ifdef FPIACTION_DELETE
				if(!(so->SType & OT_SECTOR))action=FPIACTION_DELETE;
#endif
			}
			else if(so->RFlags & RF_CURE)	action=FPIACTION_DISINFECT;
			else if(so->RFlags & RF_DETECT)	action=FPIACTION_NONE;
			else if(so->RFlags & RF_WARNING)	flag|=FPIFLAG_SUSPECTED;
			else if(so->RFlags & RF_SUSPIC)	flag|=FPIFLAG_HEURISTIC;
#endif
			if((so->RFlags & RF_IN_CURABLE_EMBEDDED) && (so->RFlags & RF_CURE_FAIL))
				goto ret3;

			FPI_InfectionReport( action , flag, Stat->LastVirusName, so->Connection);

		}


		goto ret3;

	case AVP_CALLBACK_OBJECT_PACKED:
	case AVP_CALLBACK_OBJECT_ARCHIVE:
	case AVP_CALLBACK_OBJECT_UNKNOWN:
	case AVP_CALLBACK_OBJECT_MESS:
ret3:
	    if(!fAVPCallback)break;
        ret=fAVPCallback(wParam, NULL, so);
		break;
    default:
		break;
    }

//	if(!Ready) ret=1;
	return ret;
}

Avp95VM::Avp95VM(VMHANDLE hVM) : VVirtualMachine(hVM) 
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_Avp95VM create" );
#endif
}

Avp95Thread::Avp95Thread(THREADHANDLE hThread) : VThread(hThread) 
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_Avp95Thread create" );
#endif
}

BOOL LoadIO();

//DWORD gk_pm_proc=0;
Avp95Device* Avp95DeviceInstance;

Avp95Device::Avp95Device()
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_LoadIO" );
#endif
    LoadIO();
	AvpCallback     =_AvpCallbackRedir;

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_Avp95Device created" );
#endif
	Avp95DeviceInstance=this;
/*
	PDDB pddb=Get_DDB(0,"GK95    ");
	if(pddb!=0)
	{
		gk_pm_proc=pddb->DDB_PM_API_Proc;
		_asm{
			push ebp
			mov  ebp, esp
			sub  esp, 20;
			mov  word ptr [ebp+0x1C],0x0800;
			mov  word ptr [ebp+0x10],0x0100
		}
        ((void (__stdcall *)(void))gk_pm_proc )();
		_asm mov word ptr [ebp+0x1C],0x0002;
        ((void (__stdcall *)(void))gk_pm_proc )();
		_asm{
			add esp, 20;
			pop ebp;
		}
	}
*/
}

BOOL Avp95Device::OnSysCriticalInit(VMHANDLE hSysVM, PCHAR pszCmdTail, PVOID refData)
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_OnSysCriticalInit" );
#endif
        return TRUE;
}

BOOL Avp95Device::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_OnDeviceInit" );
#endif
	MyVM=hSysVM;
    return TRUE;
}

VOID Avp95Device::OnSystemExit(VMHANDLE hSysVM)
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_OnSystemExit" );
#endif
}

VOID Avp95Device::OnSysCriticalExit()
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_OnSysCriticalExit" );
#endif
}

CPtrArray* RTreadArray=NULL;
void AvpmRegisterThread(BOOL reg)
{
	void* cth=(void*) Get_Cur_Thread_Handle();
	mRegThread.enter();
	if(reg){
		if (!RTreadArray) RTreadArray=new CPtrArray;
		if (RTreadArray) RTreadArray->Add(cth);
	}else{
		int i=-1;
		if(RTreadArray){
			for(i=RTreadArray->GetUpperBound();i>=0;i--)
				if (RTreadArray->GetAt(i)==cth)break;
		}
		if(i!=-1)RTreadArray->RemoveAt(i);
		if(!RTreadArray->GetSize())
		{
			delete RTreadArray;
			RTreadArray=NULL;
		}
	}
	mRegThread.leave();

//	if(!reg)return;
//	HANDLE h=_AvpCreateFile(
//		reg?"C:\\FSAV_000.NUL":"C:\\FSAV_001.NUL"
//		,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
//	if(h!= INVALID_HANDLE_VALUE) _AvpCloseHandle(h);
}

extern PDDB AvpIoDDB; 

BOOL Avp95Device::OnSysDynamicDeviceInit()
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_OnSysDynamicDeviceInit" );
#endif

	if(!WaitIfBusy()) return 0;
	if(Stat==NULL)Stat=new AVPStat;
	if(Stat==NULL) return 0;
	memset(Stat,0,sizeof(AVPStat));
	if(BaseInfo==NULL)BaseInfo=new AVPBaseInfo;
	if(BaseInfo==NULL){
		delete Stat;
		Stat=NULL;
		return 0;
	}
	memset(BaseInfo,0,sizeof(AVPBaseInfo));

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_BAvpInit" );
#endif
	AvpmRegisterThread(1);

	if(!BAvpInit())return FALSE;
	if(FALSE==PageAllocate(MYSTACKSIZE/0x1000, PG_SYS, 0, 0, 
		0,0,0, PAGELOCKED, &hMem, &LinAddr)
		)	return FALSE;
	for(int i=0;i<MESS_TEXT_MAX;i++)MessText[i]=0;

	
//	PDEVICEINFO pHandle;
//	VXDLDR_LoadDevice("AVP_IO.VXD", VXDLDR_INIT_DEVICE, &pHandle, &AvpIoDDB);

	ReleaseWait();

	return TRUE;
}


BOOL Avp95Device::OnSysDynamicDeviceExit()
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_OnSysDynamicDeviceExit" );
#endif
	Ready=0;

	if(!WaitIfBusy()) return 0;

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_BAvpDestroy" );
#endif
	BAvpDestroy();
	VndDestroy();
	PageFree(hMem,0);
	for(int i=0;i<MESS_TEXT_MAX;i++)if(MessText[i])delete MessText[i];

	AvpmRegisterThread(0);
	
	if(	AvpIoDDB){
		DWORD ret=VXDLDR_UnloadDevice(-1,"AVP_IO");
		AvpIoDDB=NULL;
	}

	if(Stat!=NULL){
		delete Stat;
		Stat=NULL;
	}
	if(Stat!=NULL){
		delete Stat;
		Stat=NULL;
	}

/*
	if(gk_pm_proc){
		_asm{
			push ebp
			mov  ebp, esp
			sub  esp, 20;
			mov  word ptr [ebp+0x1C],0x0800;
			mov  word ptr [ebp+0x10],0x0101
		}
        ((void (__stdcall *)(void))gk_pm_proc )();
		_asm{
			add esp, 20;
			pop ebp;
		}
	}
*/

	ReleaseWait();

    return TRUE;
}


static DWORD s_AVP95VXD_Get_Version(DWORD AVP95object_)
{
    Avp95Device * avp95 = (Avp95Device *)AVP95object_;
    return avp95->AVP95VXD_Get_Version();
}

DWORD Avp95Device::AVP95VXD_Get_Version()
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_Get_Version()");
#endif
	
    return (VER_PRODUCTVERSION_HIGH << 24) | (VER_PRODUCTVERSION_LOW << 16) | VER_PRODUCTVERSION_BUILD;
}


static DWORD s_AVP95VXD_Initialize(DWORD AVP95object_, PCHAR dbname_)
{
    Avp95Device * avp95 = (Avp95Device *)AVP95object_;
	if(StaticMFlags) return 0;
    return avp95->AVP95VXD_Initialize(dbname_);
}

extern DWORD open_reserved_flag;

DWORD Avp95Device::AVP95VXD_Initialize(PCHAR dbname_)
{
	if(!WaitIfBusy()) return 0;
	
	char filename[0x200];
	strcpy(filename,dbname_);
	char* nam=AvpStrrchr(filename,'\\');
	if(nam){
		strcpy(nam+1,AVPVnd_Name);
		VndInit(filename);
	}

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_Initialize(%s)", (char *)dbname_);
#endif
	
	memset(BaseInfo,0,sizeof(AVPBaseInfo));
	strncpy(BaseInfo->SetName,dbname_,0x200);
	BaseInfo->Status|=BI_LOADING;
	Ready=0;
	open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;

	
#ifdef _DEBUG
	FPI_ValidateDatabases(NULL, 0);
#endif
	
	Ready=BAvpLoadBase(dbname_);

#ifdef _DEBUG
	FPI_ValidateDatabases("c:\\bas", 0);
	Ready=BAvpLoadBase(dbname_);
#endif
	
	open_reserved_flag=0;
	BaseInfo->Status&=~BI_LOADING;
	BaseInfo->Status|=BI_LOADED;
	if(!Ready)
		BaseInfo->Status|=BI_ERROR;
	
#ifdef DEBUGPRINT
    dprintf("[AVP95VXD]Loading of bases is done with %d", Ready);
#endif
	
	ReleaseWait();
	
    return Ready;
}




static DWORD s_AVP95VXD_RegisterCallback(DWORD AVP95object_, AVPCallback callback_)
{
    Avp95Device * avp95 = (Avp95Device *)AVP95object_;
    return avp95->AVP95VXD_RegisterCallback(callback_);
}

DWORD Avp95Device::AVP95VXD_RegisterCallback(AVPCallback callback_)
{
#ifdef DEBUGPRINT
    dprintf("[AVP95VXD]_RegisterCallback(%p)", (void *)callback_);
#endif
    fAVPCallback = callback_;
    return 0;
}


static DWORD s_AVP95VXD_ProcessObject(DWORD AVP95object_, AVPScanObject * obj_)
{
	void* cth=(void*) Get_Cur_Thread_Handle();
	int i=-1;
	mRegThread.enter();
	if(RTreadArray){
		for(i=RTreadArray->GetUpperBound();i>=0;i--)
			if (RTreadArray->GetAt(i)==cth)break;
	}
	mRegThread.leave();
	if(i!=-1) return 0;


    Avp95Device * avp95 = (Avp95Device *)AVP95object_;

	if(UseIFSMGR) return 0;
	if(StaticMFlags)
		obj_->MFlags=MFlags;

    return avp95->AVP95VXD_ProcessObject(obj_);
}


#define HIST_SIZE 100

static DWORD HistNameCs[HIST_SIZE];
static DWORD HistStatus[HIST_SIZE];
static DWORD HistTime[HIST_SIZE];
static int HistIdx=0;
static int HistCount=0;


static DWORD CSum(BYTE* mass,DWORD len)
{
	register DWORD i;
	register DWORD s;
	register BYTE* m;
	s=0;
	m=mass;
	for (i=0 ;i<len; i++,m++){
		s^=s<<1;
		s^=*m;
	}
	return s;
}

DWORD ProcessObject(AVPScanObject * so){
	DWORD ret=1;

#ifdef DEBUGPRINT
    dprintf("[AVP95VXD]_ProcessObject type=%X, disk=%X, drive=%X, %s ", so->SType,so->Disk,so->Drive,so->Name);
#endif
	CurRefdata=so->Connection;

	DWORD LoDWord = Get_Last_Updated_System_Time(); //millisec
	DWORD sum=CSum((PUCHAR)so->Name,strlen(so->Name));

	int j=HistIdx;
	for(int i=0;i<HistCount;i++)
	{
		if(j==0)j=HistCount;
		j--;
		if(HistNameCs[j]==sum){
			BOOL wasinf=!!(0xF & HistStatus[j]);
			if((so->MFlags & MF_APC_CURE_REQUEST) ||
				!wasinf ||
				((LoDWord-HistTime[j])>2000 // 2 sec
				&& wasinf)){
				HistNameCs[j]=0;
			}
			else{
				so->RFlags=HistStatus[j];
//				_DebugTrace(TraceInfo,"Use value from History: %s\n",so->Name);
				goto ex;
			}
			break;
		}
	}

//	__try{
		ret=BAvpProcessObject(so);
//	}
//	__except( 1 ) {
//		ret=1;
//		so->RFlags|=RF_KERNEL_FAULT;
//		_AvpCallback(AVP_CALLBACK_OBJECT_DONE,(DWORD)so);
//		goto ex;
//	}

	if(so->RFlags & (RF_ERROR | RF_NOT_OPEN))
		goto ex;

	HistNameCs[HistIdx]=sum;
	HistStatus[HistIdx]=so->RFlags;
	HistTime[HistIdx]=LoDWord;
	if(++HistIdx==HIST_SIZE)HistIdx=0;
	if(HistCount<HIST_SIZE)HistCount++;

ex:
	
	CurRefdata=0;
	return ret;
}

DWORD Avp95Device::AVP95VXD_ProcessObject(AVPScanObject * obj_)
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_EnterWait    : q:%d %s", Busy, (char *)obj_->Name);
#endif
	if(!Enable) return 0;
	if(!Ready) return 0;
	if(!WaitIfBusy()) return 0;
	if(!Ready) return 0;//!!!
	if(obj_->SType & OT_MEMORY)UseIFSMGR=1;

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD]_ProcessObject: q:%d %s", Busy, (char *)obj_->Name);
#endif

	Infected=0;
	
	if(LimitCompoundSize && (obj_->SType == OT_FILE))
	{
		WORD err;
		_WIN32_FIND_DATA_BCS wfd;
		HANDLE h=R0_FindFirstFile(obj_->Name,0,&wfd,&err);
		if(h!=INVALID_HANDLE_VALUE && h)
		{
			R0_FindCloseFile(h,&err);
			if(wfd.nFileSizeHigh || (wfd.nFileSizeLow>LimitCompoundSize))
				obj_->MFlags&=((DWORD)-1)^(MF_ARCHIVED|MF_PACKED|MF_MAILBASES|MF_MAILPLAIN);

		}
	}

#ifdef DEBUGPRINT
	dprintf("OBJ MFlags: %08X  ",obj_->MFlags);
#endif

	Call_On_My_Stack(ProcessObject,(DWORD)obj_,(PDWORD)((BYTE*)LinAddr+MYSTACKSIZE),MYSTACKSIZE);
//	ProcessObject(obj_);

#ifdef DEBUG_1
#ifdef DEBUGPRINT
    dprintf("Result of scan %s : %08X",obj_->Name,obj_->RFlags );
	static char buf[512];
	if(obj_->RFlags & RF_DETECT)
		dprintf("Infected: %s ",BAvpMakeFullName(buf,obj_->VirusName) );
	else if(obj_->RFlags & RF_WARNING)
			dprintf("Warning: %s ",BAvpMakeFullName(buf,obj_->WarningName) );
	else if(obj_->RFlags & RF_SUSPIC)
			dprintf("Suspicion: %s ",BAvpMakeFullName(buf,obj_->SuspicionName) );
#endif
#endif


	DWORD ret=0;
	if(Infected)ret=1;

	UseIFSMGR=0;
	ReleaseWait();
    return ret;
}


static PCHAR s_AVP95VXD_MakeFullName(DWORD AVP95object_, PCHAR buf_, PCHAR ptr_)
    {
    Avp95Device * avp95 = (Avp95Device *)AVP95object_;
    return avp95->AVP95VXD_MakeFullName(buf_,ptr_);
    }

PCHAR Avp95Device::AVP95VXD_MakeFullName(PCHAR buf_, PCHAR ptr_)
    {
	if(!Ready){
		*buf_=0;
		return buf_;
	}
#ifdef DEBUGPRINT
//    dprintf("AVP95VXD_MakeFullName()");
#endif
	return BAvpMakeFullName(buf_,ptr_);
	}




/*
    =====================================================================
    Avp95Device::OnControlMessage()

    This is the overloaded control message handler.  Only messages in the
    BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL ... END_RESERVED_PRIVATE_SYSTEM_CONTROL
    range may be processed here without passing to the default message
    handler.

    Private message IDs are defined in avp95api.h.

    Register usage for private messages:
        input:
            eax : control message ID
            edx : parameter 1

        output:
            ecx : return value 2 (not used now)
            edx : return value 1


    Private control message descriptions:

        AVP95CTRL_GETPTR
            Returns a pointer to AVP95Device object or a function.  All
            the functions later called must pass the pointer to AVP95Device
            as the first parameter so that the C++ member function could be
            called for the device.  All the C++ member functions have the
            identical parameter lists to their counterparts to the "public"
            functions defined in avp95api.h.
    =====================================================================
*/



DWORD Avp95Device::OnControlMessage(
                               DWORD _Edi,
                               DWORD _Esi,
                               DWORD _Ebp,
                               DWORD _Esp,
                               DWORD _Ebx,
                               DWORD _Edx,
                               DWORD _Ecx,
                               DWORD _Eax)
    {
    if (_Eax == AVP95CTRL_GETPTR)
        {                                           // AVP95CTRL_GETPTR: return pointer to AVP95Device object or a function
#ifdef DEBUGPRINT
//        dprintf("AVP95CTRL_GETPTR control message received, edx==%x", (int)_Edx);
#endif

        DWORD ret_pointer = 0L;                     // must return NULL if no pointer found

        switch (_Edx)
            {
            case AVP95CTRL_GETPTR_AVP95_OBJ:        // return pointer to this
                ret_pointer = (DWORD)this;
                break;

            case AVP95CTRL_GETPTR_GET_VERSION:
                ret_pointer = (DWORD)s_AVP95VXD_Get_Version;
                break;

            case AVP95CTRL_GETPTR_INITIALIZE:
                ret_pointer = (DWORD)s_AVP95VXD_Initialize;
                break;

            case AVP95CTRL_GETPTR_REGISTERCALLBACK:
                ret_pointer = (DWORD)s_AVP95VXD_RegisterCallback;
                break;

            case AVP95CTRL_GETPTR_PROCESSOBJECT:
                ret_pointer = (DWORD)s_AVP95VXD_ProcessObject;
                break;

            case AVP95CTRL_GETPTR_MAKEFULLNAME:
                ret_pointer = (DWORD)s_AVP95VXD_MakeFullName;
                break;

#ifdef FPIAPI_VER_MAJOR
            case AVP95CTRL_FPICTRL_GETPTR_GET_MODULE_INFORMATION: 
                ret_pointer = (DWORD)FPI_GetModuleInformation;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_INITIALIZE:
                ret_pointer = (DWORD)FPI_Initialize;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_UNINITIALIZE:           
                ret_pointer = (DWORD)FPI_Uninitialize;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_SCAN_FILE:
                ret_pointer = (DWORD)FPI_ScanFile;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_SCAN_BOOT_BLOCK:
                ret_pointer = (DWORD)FPI_ScanBootBlock;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_SCAN_MEMORY:
                ret_pointer = (DWORD)FPI_ScanMemory;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_VALIDATE_DATABASES: 
                ret_pointer = (DWORD)FPI_ValidateDatabases;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_REPORT_SCANNING_STATUS:
                ret_pointer = (DWORD)FPI_ReportScanningStatus;
                break;
            case AVP95CTRL_FPICTRL_GETPRT_RELOAD_DATABASES:
                ret_pointer = (DWORD)FPI_ReloadDatabases;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_AUTO_RELOAD_OFF:
                ret_pointer = (DWORD)FPI_AutoReloadOff;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_AUTO_RELOAD_ON:
                ret_pointer = (DWORD)FPI_AutoReloadOn;
                break;
            case AVP95CTRL_FPICTRL_GETPTR_CUSTOM_SETTINGS:
                ret_pointer = (DWORD)FPI_CustomSettings;
                break;

#endif
            }

        _asm    mov edx, dword ptr [ret_pointer];   // pass the result in edx

        return TRUE;
        }

    return VDevice::OnControlMessage(_Edi, _Esi, _Ebp, _Esp, _Ebx, _Edx, _Ecx, _Eax);
    }

DWORD Avp95Device::OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams)
{
	switch(pDIOCParams->dioc_IOCtlCode){

	case IOCTL_AVP_GET_VERSION:
//OutBuf: DWORD Version; OutBufSize 4;
		if(pDIOCParams->dioc_cbOutBuf<4)return 1;
		*(DWORD*)(pDIOCParams->dioc_OutBuf)=AVP_IOCTL_Version;
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
		break;

	case IOCTL_AVP_GET_MFLAGS:
		if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;
		*(DWORD*)(pDIOCParams->dioc_OutBuf)=MFlags;
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
		break;

	case IOCTL_AVP_SET_MFLAGS:
		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
		MFlags=*(DWORD*)(pDIOCParams->dioc_InBuf);
#ifdef DEBUGPRINT
		dprintf("[AVP95VXD]SET MFlags: %08X",MFlags);
#endif
		break;

	case IOCTL_AVP_GET_STAT:
		if(pDIOCParams->dioc_cbOutBuf<sizeof(AVPStat))return 1;
		Stat->MFlags=MFlags;
		Stat->Enable=Enable;
		Stat->StaticMFlags=StaticMFlags;
		memcpy(pDIOCParams->dioc_OutBuf,Stat,sizeof(AVPStat));
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(AVPStat);
		break;

	case IOCTL_AVP_STATIC_MFLAGS:
		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
		StaticMFlags=*(DWORD*)(pDIOCParams->dioc_InBuf);
		break;

	case IOCTL_AVP_LOAD_BASE:
		if(pDIOCParams->dioc_cbInBuf<sizeof(char*))return 1;
		AvpmRegisterThread(1);
		AVP95VXD_Initialize((char*)(pDIOCParams->dioc_InBuf));
		AvpmRegisterThread(0);
		break;

	case IOCTL_AVP_PROCESS_OBJECT:
		if(pDIOCParams->dioc_cbInBuf!=sizeof(AVPScanObject))return 1;
		AvpmRegisterThread(1);
		AVP95VXD_ProcessObject((AVPScanObject*)(pDIOCParams->dioc_InBuf));
		AvpmRegisterThread(0);
		break;

	case IOCTL_AVP_SET_ALERT_TEXT:
		if(!(pDIOCParams->dioc_InBuf))return 1;
		if(!(pDIOCParams->dioc_cbInBuf))return 1;
		if(MessText[0])delete MessText[0];
		if(!(MessText[0]=new char[pDIOCParams->dioc_cbInBuf + 1]))return 1;
		strcpy(	MessText[0],(char*)(pDIOCParams->dioc_InBuf));
		break;

	case IOCTL_AVP_GET_BASE_INFO:
		if(pDIOCParams->dioc_cbOutBuf<sizeof(AVPBaseInfo))return 1;
		memcpy(pDIOCParams->dioc_OutBuf,BaseInfo,sizeof(AVPBaseInfo));
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(AVPBaseInfo);
		break;

	case IOCTL_AVP_ENABLE:
		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
		Enable=*(DWORD*)(pDIOCParams->dioc_InBuf);
		break;

	case IOCTL_AVP_ADD_MASK:
//		if(pDIOCParams->dioc_cbInBuf<sizeof(char*))return 1;
		BAvpAddMask((char*)(pDIOCParams->dioc_InBuf));
		break;

	case IOCTL_AVP_ADD_EXCLUDE_MASK:
//		if(pDIOCParams->dioc_cbInBuf<sizeof(char*))return 1;
		BAvpAddExcludeMask((char*)(pDIOCParams->dioc_InBuf));
		break;

	case IOCTL_AVP_SET_MESSAGE_TEXT:
		if(pDIOCParams->dioc_cbInBuf<2*sizeof(DWORD))return 1;
		{
			int i=((DWORD*)pDIOCParams->dioc_InBuf)[0];
			if(i>=MESS_TEXT_MAX)return 1;
			char* s=(char*)((DWORD*)pDIOCParams->dioc_InBuf)[1];
			if(s==NULL)return 1;
			if(MessText[i])delete MessText[i];
			if(!(MessText[i]=new char[strlen(s) + 1]))return 1;
			strcpy(	MessText[i],s);
		}
		break;

	case IOCTL_AVP_RELOAD_BASE:
		{
		char set[0x200];
		if(pDIOCParams->dioc_cbInBuf==0){
			if(0==*BaseInfo->SetName)return 1;
			else strcpy(set,BaseInfo->SetName);
		}else strcpy(set,(const char*)pDIOCParams->dioc_InBuf);
		AvpmRegisterThread(1);
		AVP95VXD_Initialize(set);
		AvpmRegisterThread(0);
		break;
		}

	case IOCTL_AVP_LIMITCOMPOUNDSIZE:
		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
		LimitCompoundSize=*(DWORD*)(pDIOCParams->dioc_InBuf);
		break;

	case IOCTL_AVP_REGISTERTHREAD:
		AvpmRegisterThread(1);
		break;

	case IOCTL_AVP_UNREGISTERTHREAD:
		AvpmRegisterThread(0);
		break;

	case IOCTL_AVP_MAKE_FULL_NAME  :
		if(pDIOCParams->dioc_cbInBuf!=sizeof(DWORD))return 1;
		if(pDIOCParams->dioc_cbOutBuf<0x80)return 1;
		AVP95VXD_MakeFullName((char*)pDIOCParams->dioc_OutBuf,
			*(char**)(pDIOCParams->dioc_InBuf));
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=strlen((char*)pDIOCParams->dioc_OutBuf);

		break;

	case IOCTL_AVP_CALLBACK_CREATE:
#ifdef DEBUGPRINT
//		dprintf("CREATE CALLBACK\n");
#endif
		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
		if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;

		APC_CallbackProcPtr=*(DWORD*)(pDIOCParams->dioc_InBuf);
		*(DWORD*)(pDIOCParams->dioc_OutBuf)=APC_CallbackProcPtr;

		APC_VmThreadHandle=Get_Cur_Thread_Handle();
		
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
#ifdef DEBUGPRINT
		dprintf("[AVP95VXD]CREATE CALLBACK: APC_CallbackProcPtr: %08X",APC_CallbackProcPtr);
#endif
	
		break;
	case IOCTL_AVP_CALLBACK_SYNC_DATA:
//InBuf:  DWORD DataPtr; InBufSize 4
		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
		delete (AVPScanObject*)(pDIOCParams->dioc_InBuf);

		break;
	case IOCTL_AVP_CALLBACK_DELETE:
//		if(pDIOCParams->dioc_cbInBuf<sizeof(DWORD))return 1;
//		if(APC_CallbackProcPtr!=*(DWORD*)(pDIOCParams->dioc_InBuf))return 1;
		APC_CallbackProcPtr=0;

		break;

	case IOCTL_AVP_CHECK_FILE_INTEGRITY:
//InBuf: char* SetName; InBufSize strlen(SetName);
		if(!(pDIOCParams->dioc_InBuf))return 1;
		if(!(pDIOCParams->dioc_cbInBuf))return 1;
		if(pDIOCParams->dioc_cbOutBuf<sizeof(int))return 1;
		{
			int res;
			res=sign_check_file((char*)(pDIOCParams->dioc_InBuf),1,vndArray, vndArrayCount, 0);
			memcpy(pDIOCParams->dioc_OutBuf,&res,sizeof(int));
			if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(int);
//dprintf("[AVP95VXD]sign_check_file: %d %s",res,pDIOCParams->dioc_InBuf);
		}
		break;
	default:
		return VDevice::OnW32DeviceIoControl(pDIOCParams);
	}
	return DEVIOCTL_NOERROR;
}


BOOL Avp95VM::OnSysVMInit()
{
        return TRUE;
}

VOID Avp95VM::OnSysVMTerminate()
{
}


