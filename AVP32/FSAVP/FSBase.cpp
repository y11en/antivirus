////////////////////////////////////////////////////////////////////
//
//  FSBASE.CPP
//  Engine SYS Wrapper.
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include <vdw.h>

#include <stdio.h>
#include <stdarg.h>

#include "FSAVP.h"
#include "msglog.h"
#include "FSBase.h"
#include "sysio.h"

#include <Sign/sign.h>
#define __KLDT_NO_UNICODE__
#include <serialize/kldtser.h>
#include <AvpVndID.h>
#include <Version/ver_avp.h>

//turbpa010412-[14323]{
#if GATEKEEPER_API_VERSION != 0x100
// support only for FSAV 5.x

#include "..\DBCleanup\DBCleanup.h"
// global that equal true when database files should be cleaned up
static BOOL DisableDBCleanUp = FALSE;  
#endif
//turbpa010412-[14323]}


//#define __SEH__

//#define sign_check_file(a,b,c,d,e) SIGN_OK
//#define MY_OWN_STACK 
#ifdef MY_OWN_STACK 
ULONG OwnStackSize=0;//0x10000 
DWORD* MyStack=0;
DWORD* MyESP;
DWORD* OldESP;
#endif //MY_OWN_STACK 

static PCHAR FSAVP_Regname="F-Secure Anti-Virus AVP Plug-in";
static PCHAR FSAVP_Version="Version " VER_FILEVERSION_STR;
PCHAR FSAVP_Compilation=__TIMESTAMP__;


static PCHAR MessObjWarning=NULL;
static PCHAR MessObjSuspicion=NULL;
static PCHAR MessObjInfected=NULL;
static PCHAR MessObjDisinfected=NULL;
static PCHAR MessObjDeleted=NULL;

PVOID CallbackHandle=NULL;
static ULONG AllowUnload=1;
static PFILTER_CALLBACK CallbackStruct=NULL;
static PPLUGIN_INFO pinf=NULL;
KUstring BasePath(0x200,NonPagedPool);
KUstring TempPath(0x200,NonPagedPool);
KUstring SetFile(0x200,NonPagedPool);
static ULONG Priority=1000L;
static ULONG GKRegister = 0x00000001;
static LONG QueueMax=0x80;
static ULONG StopProcessArchivesAfterDetection=1;
static ULONG DisableSectorScan=0;
ULONG Enable = 0x00000001;
ULONG EventLogging = 0;
ULONG MFlags = 0x00110002;
ULONG BFlags = 0x00110002;
ULONG MFlagsODS = 0x00110002;
ULONG BFlagsODS = 0x00110002;
ULONG StaticMFlags=0;
char* SetName=NULL;
AVPStat* Stat=NULL;
AVPBaseInfo* BaseInfo=NULL;
BOOL add_detect=0;
static BOOL	Autoreload=0;//1;

KUstring sRegistryPath(0x200,NonPagedPool);
KDriver* pFSAVPDriver;
static DWORD RFlags=0;
static BOOL ShutDown=0;
static DWORD SequentialNumber=0;
DWORD LimitCompoundSize=0;
char* MessText[MESS_TEXT_MAX];

static KInterlockedCounter BusyCounter;
KInterlockedCounter InMyOpenClose;

//static KMutex BusyMutex(3);
//static FAST_MUTEX bMutex;
static KSEMAPHORE bSemafore;

BOOL WaitIfBusy()
{
	if(BusyCounter.Test()>QueueMax)
		return FALSE;
	++BusyCounter;

	if(InMyOpenClose.Test())
	{
		LARGE_INTEGER t;
		t=LiNeg(LiNMul(1,10000000UL));
		_Trace(TraceInfo,"KeWaitForSingleObject semaphore: %08X TIMED\n", &bSemafore);
		NTSTATUS stat=KeWaitForSingleObject( &bSemafore,  Executive, KernelMode, TRUE, &t);
		switch(stat)
		{
		case STATUS_SUCCESS:
			break;
		default:
		case STATUS_ALERTED:
		case STATUS_TIMEOUT:
			if(InMyOpenClose.Test())
			{
				--BusyCounter;
				return FALSE;	
			}
			goto wait;
		}
		return TRUE;
	}

wait:
	_Trace(TraceInfo,"KeWaitForSingleObject semaphore: %08X Infinite\n", &bSemafore);
	KeWaitForSingleObject( &bSemafore,  Executive, KernelMode, FALSE, NULL);
	return TRUE;
}

void ReleaseWait()
{
	--BusyCounter;
	KeReleaseSemaphore( &bSemafore ,0,1,0 );
}


static void** vndArray=NULL;
static int vndArrayCount=0;

static void VndDestroy()
{
	if(vndArray){
		for(int i=0;i<vndArrayCount;i++)
			if(vndArray[i])
			delete vndArray[i];
		delete vndArray;
	}
	vndArrayCount=0;
	vndArray=NULL;
}

void* __cdecl nt_malloc(unsigned size_){
    return new(POOL) BYTE[size_];
}

void __cdecl nt_free(void* ptr_){
	delete ptr_;
}

static BOOL VndInit( const char *filename )
{
	BOOL ret=FALSE;
	VndDestroy();
	if(filename==NULL) return FALSE;
	if(sign_check_file(filename,1,0,0,0)) return FALSE;
	if(!KLDT_Init_Library((void *(__stdcall *)(unsigned int))&nt_malloc,(void (__stdcall *)(void *))&nt_free)) return FALSE;
	if(!DATA_Init_Library((void *(__stdcall *)(unsigned int))&nt_malloc,(void (__stdcall *)(void *))&nt_free)) return FALSE;

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

#ifndef MBR_OR_BOOT_INFECTION
#define MBR_OR_BOOT_INFECTION       ((NTSTATUS)0xC10107D0L)
#endif

static int uncLength;
static char* PlainName;

char* makePlainName(char* dest, char* src)
{
		strcpy(dest,PlainName);
		strcat(dest,src + uncLength);
		return dest;
}

static NTSTATUS GkFillPluginMessage(PPLUGINMESSAGE pm,char *s, AVPScanObject* so, DWORD action)
{
	HandlerInfo* hi=(HandlerInfo*)so->Connection;
	if(	!hi || !CallbackHandle || !pm) return STATUS_INSUFFICIENT_RESOURCES ;

	if(so->RFlags & RF_DELETED)	
		action=(so->SType==OT_FILE)?ACTION_ACCESS_DENIED_REMOVED:ACTION_ACCESS_DENIED_DISINFECTED;

	RtlZeroMemory(pm,sizeof(PLUGINMESSAGE));
	pm->dwSize=sizeof(PLUGINMESSAGE);
	pm->dwVersion=PLUGIN_API_VERSION;
	pm->dwType=FS_MSG_TYPE_ALERT;              // message type (FS_MSG_TYPE_*)
	pm->dwAction=action;
	pm->dwSequentialNumber=SequentialNumber++; // sequential message number
	pm->dwLocalizationId = action;
	//		Others are 0.
	
	
#if GATEKEEPER_API_VERSION==0x0100
	
	switch(so->SType)
	{
	case OT_BOOT:
		pm->dwLocalizationId = (DWORD)MBR_OR_BOOT_INFECTION;
		pm->dwOptionalParameterSize=sprintf((PCHAR)pm->pbOptionalParameter, "D:%C", so->Drive +'A');
		break;
	case OT_MBR:
		pm->dwLocalizationId = (DWORD)MBR_OR_BOOT_INFECTION;
		pm->dwOptionalParameterSize=sprintf((PCHAR)pm->pbOptionalParameter, "M:%d", so->Disk & 0x0F);
		break;
	default: 
	case OT_FILE:
		pm->dwOptionalParameterSize=sprintf((PCHAR)pm->pbOptionalParameter, "%C",	(hi->networkAccess)?'F':'N');
		break;
	}
	
	strncpy(pm->pszMessageText,s,MAX_MESSAGE_SIZE-1);
#endif
	
#if GATEKEEPER_API_VERSION!=0x0100
	
	pm->dwTaskID=hi->dwTaskID;
	StatMutex.Wait();
	strncpy(pm->pszVirusName,Stat->LastVirusName,VIRUSNAME_SIZE-1);
	StatMutex.Release();
	
	char* n=new char[0x200];
	if(!n) return STATUS_INSUFFICIENT_RESOURCES;

	makePlainName(n,so->Name);
	
#if GATEKEEPER_VERSION < 0x0500
	strncpy(pm->pszMessageText,n,MAX_MESSAGE_SIZE-1);
#else
	strncpy(pm->pszFileName,n,MAX_MESSAGE_SIZE-1);
#endif

	switch(so->SType)
	{
	case OT_BOOT:
		pm->dwOptionalParameterSize=sprintf((PCHAR)pm->pbOptionalParameter, "D:%C", so->Drive +'A');
		pm->dwLocalizationId = Trap_FSAV_bootSectorVirusAlert;
		pm->dwObjectType = FS_OBJECT_BS;
		switch(action)
		{
		case ACTION_ACCESS_DENIED_DISINFECTED:
			pm->dwLocalizationId = Trap_FSAV_bootSectorVirusAlertDisinfected;
			break;
		case ACTION_ACCESS_DENIED_REMOVED:
			pm->dwLocalizationId = Trap_FSAV_bootSectorVirusAlertDisinfected;
			break;
		case ACTION_ACCESS_DENIED:
		default: 
			if(so->RFlags & RF_CURE_FAIL){
				if(so->RFlags & RF_ERROR)
					pm->dwLocalizationId = Trap_FSAV_bootSectorVirusAlertActionFailed;
				else
					pm->dwLocalizationId = Trap_FSAV_bootSectorVirusAlertNotDisinfected;
				break;
			}
			break;
		}
		break;
		case OT_MBR:
			pm->dwOptionalParameterSize=sprintf((PCHAR)pm->pbOptionalParameter, "M:%d", so->Disk & 0x0F);
			pm->dwLocalizationId = Trap_FSAV_mbrVirusAlert;
			pm->dwObjectType = FS_OBJECT_MBR;
			
			switch(action)
			{
			case ACTION_ACCESS_DENIED_DISINFECTED:
				pm->dwLocalizationId = Trap_FSAV_mbrVirusAlertDisinfected;
				break;
			case ACTION_ACCESS_DENIED_REMOVED:
				pm->dwLocalizationId = Trap_FSAV_mbrVirusAlertDisinfected;
				break;
			case ACTION_ACCESS_DENIED:
			default: 
				if(so->RFlags & RF_CURE_FAIL){
					if(so->RFlags & RF_ERROR)
						pm->dwLocalizationId = Trap_FSAV_mbrVirusAlertActionFailed;
					else
						pm->dwLocalizationId = Trap_FSAV_mbrVirusAlertNotDisinfected;
					break;
				}
				break;
			}
			break;
			default: 
			case OT_FILE:
				pm->dwOptionalParameterSize=sprintf((PCHAR)pm->pbOptionalParameter, "%C",	(hi->networkAccess)?'F':'N');
				pm->dwLocalizationId = Trap_FSAV_virusAlert;
				pm->dwObjectType = FS_OBJECT_FILE;
				if(so->InArc)
					pm->dwObjectType = FS_OBJECT_INSIDE_ARCHIVE;
				
				switch(action)
				{
				case ACTION_ACCESS_DENIED_DISINFECTED:
					pm->dwLocalizationId = Trap_FSAV_virusAlertDisinfected;
					break;
				case ACTION_ACCESS_DENIED_REMOVED:
					pm->dwLocalizationId = Trap_FSAV_virusAlertFileDeleted;
					break;
				case ACTION_ACCESS_DENIED:
				default: 
					if(so->RFlags & RF_CURE_FAIL){
						if(so->RFlags & RF_ERROR)
							pm->dwLocalizationId = Trap_FSAV_virusAlertActionFailed;
						else
							pm->dwLocalizationId = Trap_FSAV_virusAlertNotDisinfected;
						break;
					}
					break;
				}
				break;
	}

	delete n;	

#endif
	
/*
	if(EventLogging){

	}
*/
	return STATUS_SUCCESS;
}


#if GATEKEEPER_API_VERSION!=0x0100

static NTSTATUS GkSendErrorMessage(char *s, DWORD trap)
{
	NTSTATUS ret=STATUS_NO_MEMORY;

	PPLUGINMESSAGE pm=new (NonPagedPool)PLUGINMESSAGE;
	if(pm){
		RtlZeroMemory(pm,sizeof(PLUGINMESSAGE));
		pm->dwSize=sizeof(PLUGINMESSAGE);
		pm->dwVersion=PLUGIN_API_VERSION;
		pm->dwType=FS_MSG_TYPE_ERROR;              // message type (FS_MSG_TYPE_*)
		pm->dwAction=ACTION_NONE;
		pm->dwSequentialNumber=SequentialNumber++; // sequential message number
		pm->dwLocalizationId = trap;
//		Others are 0.
		strncpy(pm->pszMessageText,s,MAX_MESSAGE_SIZE-1);
		ret=_SendMessage(__FSAVP_ID__, FS_MSG_DEFAULT, pm);
	#if GATEKEEPER_API_VERSION!=0x0100
		delete pm;
	#endif
	}

	return ret;
}

#endif


static NTSTATUS GkSendMessage(char *s, AVPScanObject* so, DWORD action)
{
	NTSTATUS ret=STATUS_NO_MEMORY;
	PPLUGINMESSAGE pm=new (NonPagedPool)PLUGINMESSAGE;
	if(pm){
		ret=GkFillPluginMessage(pm,s,so,action);
		if(NT_SUCCESS(ret))
		{
			ret=_SendMessage(__FSAVP_ID__, FS_MSG_DEFAULT, pm);
	#if GATEKEEPER_API_VERSION!=0x0100
			delete pm;
	#endif
		}
		else
			delete pm;
	}
	return ret;
}

#define SM_SIZE 0x200
static char sm_s[SM_SIZE];
static DWORD sm_action;


static NTSTATUS wGkSendMessage(char *s, AVPScanObject* so, DWORD action)
{
	if(!s) return STATUS_NO_MEMORY;
	if(so){
		HandlerInfo* hi=(HandlerInfo*)so->Connection;
		if(so->InArc && hi && !(hi->checkStopAfterDetect))
			GkSendMessage(s, so, action);
	}
	strncpy(sm_s,s,SM_SIZE);
	sm_s[SM_SIZE-1]=0;
	sm_action=action;

	return STATUS_SUCCESS;
}


#define HIST_SIZE 100

static DWORD HistNameCs[HIST_SIZE];
static DWORD HistStatus[HIST_SIZE];
static DWORD HistTime[HIST_SIZE];
static int HistIdx=0;
static int HistCount=0;

NTSTATUS Flush()
{
	HistCount=0;
	if(CallbackHandle)
		_FlushCache();
	return STATUS_SUCCESS;
}

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

int baseShouldReload=0;
DWORD TaskID;
AVPScanObject* so_ptr=0;


#pragma optimize( "", off ) 

NTSTATUS ProcessObject(
	AVPScanObject* so,
	HandlerInfo* hi,
	int changed
	)
{
	NTSTATUS ret=STATUS_SUCCESS;

#ifdef _DEBUG
//	_asm int 3;
#endif

//	if(!Base)return ret;
	if(Autoreload && baseShouldReload)
	{
		baseShouldReload--;
		if(baseShouldReload==0)
		{
			_Trace(TraceInfo,"Reload changed SET: %s\n",SetName);
			BaseLoad();
		}
	}

#ifdef _DEBUG
	if( DisableSectorScan && so->SType & OT_SECTOR) return ret;
#endif

	if(so->SType == OT_FILE){
		DWORD attr=0;
		attr=_AvpGetFileAttributes(so->Name);
		if(attr & (FILE_ATTRIBUTE_DIRECTORY ))
		{
			so->RFlags|=RF_NOT_OPEN;
			return ret;
		}
	}


	if(!WaitIfBusy())return ret;

	if(so->Connection==0)so->Connection=(DWORD)hi;
	
	int i;
	if(hi && hi->plainFilename)	PlainName=hi->plainFilename;
	else PlainName=so->Name;
	char* pn;
	pn=PlainName;
	if(_AvpStrrchr(pn,'%'))pn="Object with bad name";
	int pnl;
	pnl=strlen(pn);
	_Trace(TraceInfo,"%-55.55s : Scan request\n",(pnl<55)?(pn):(pn+pnl-55));

#if GATEKEEPER_API_VERSION!=0x0100
	TaskID=hi?(hi->dwTaskID):0;
#endif

	sm_action=0;
	RFlags=0;

	LARGE_INTEGER liTimeNow;
	DWORD sum;
	sum=CSum((PUCHAR)so->Name,strlen(so->Name));
	KeQuerySystemTime (&liTimeNow);

	int j;
	j=HistIdx;
	for(i=0;i<HistCount;i++)
	{
		if(j==0)j=HistCount;
		j--;
		if(HistNameCs[j]==sum){
			BOOL wasinf=!NT_SUCCESS(HistStatus[j]);
			
			switch(changed){
			case 0:
				if(!wasinf) 
					goto ret_stat;
			case 2:
				if((liTimeNow.LowPart-HistTime[j])<20000000)
					goto ret_stat;
			default:
				break;
			}
			HistNameCs[j]=0;
			break;
ret_stat:			
/*
			if(	changed || (wasinf && ((liTimeNow.LowPart-HistTime[j])>20000000)))
			{
				HistNameCs[j]=0;
				break;
			}
*/
			ret=HistStatus[j];
			_Trace(TraceInfo,"%-55.55s : Return from History\n",(pnl<55)?(pn):(pn+pnl-55));
			goto ex;
		}
	}

	uncLength=strlen(so->Name);
	
//	so->MFlags|=MF_DBG_NO_SCAN;

	so_ptr=so;
#ifdef MY_OWN_STACK 
	if(MyStack){
		_asm mov OldESP, esp;
		_asm mov esp, MyESP;
	}
#endif// MY_OWN_STACK 

#ifdef __SEH__
	__try{
#endif
		BAvpProcessObject(so_ptr);
#ifdef __SEH__
	}
	__except( 1 ) {
		BAvpErrorFlush();
		_Trace(TraceInfo,"RF_KERNEL_FAULT\n\n");
		so_ptr->RFlags|=RF_KERNEL_FAULT;
		AvpCallback(AVP_CALLBACK_OBJECT_DONE,(DWORD)so_ptr);
		goto except;
	}
except:
#endif

#ifdef MY_OWN_STACK 
	if(MyStack){
		_asm mov esp,OldESP;
	}
#endif// MY_OWN_STACK 

	RFlags|=so->RFlags;

	_Trace(TraceInfo,"%-55.55s Q:%d RF:%08X\n",(pnl<55)?(pn):(pn+pnl-55),BusyCounter.Test(),RFlags);

	if(sm_action) 
	{
		ret=STATUS_ACCESS_DENIED;
		if(hi && hi->infected) *(hi->infected)=1;

		if(hi && hi->pPluginMessage)
		{
			PPLUGINMESSAGE pm=new (NonPagedPool)PLUGINMESSAGE;
			if(pm)
			{
				if(NT_SUCCESS(GkFillPluginMessage(pm,sm_s,so,sm_action)))
					*(hi->pPluginMessage)=pm;
				else
					delete pm;
			}
		}

	}
	else if((RFlags & RF_NOT_OPEN) && (so->SType & OT_SECTOR))
	{
		_Trace(TraceInfo,"STATUS_NO_MEDIA_IN_DEVICE %s\n",so->Name);
		ret=STATUS_NO_MEDIA_IN_DEVICE;
	}
	
	

	if(RFlags & (RF_ERROR))
		goto ex;

	HistNameCs[HistIdx]=sum;
	HistStatus[HistIdx]=ret;
	HistTime[HistIdx]=liTimeNow.LowPart;
	if(++HistIdx==HIST_SIZE)HistIdx=0;
	if(HistCount<HIST_SIZE)HistCount++;

ex:

	so_ptr=0;
//	InternalAccessRemoveAll();
	ReleaseWait();
	return ret;
}
#pragma optimize( "", on ) 

static NTSTATUS FileHandler(
	PCHAR uncFilename,
	PCHAR plainFilename,
	PPLUGINMESSAGE *pPluginMessage,
	BOOLEAN networkMount,
	BOOLEAN networkAccess,
	PBOOLEAN infected,
	ULONG mediaType,
	ULONG mediaInfo,
	PETHREAD petr
	)
{

	if(ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
	strcpy(_so.Name,uncFilename);
    _so.MFlags=MFlags;


	HandlerInfo hi;
    memset(&hi,0,sizeof(HandlerInfo));
	hi.uncFilename=uncFilename;
	hi.plainFilename=plainFilename;
	hi.pPluginMessage=pPluginMessage;
	hi.networkMount=networkMount;
	hi.networkAccess=networkAccess;
	hi.infected=infected;
	hi.mediaType=mediaType;
	hi.mediaInfo=mediaInfo;
	hi.petr=petr;
	hi.checkStopAfterDetect=1;

	return ProcessObject(&_so, &hi);
}

static NTSTATUS OpenFileHandler(
	PCHAR uncFilename,
	PCHAR plainFilename,
	PPLUGINMESSAGE *pPluginMessage,
	BOOLEAN networkMount,
	BOOLEAN networkAccess,
	PBOOLEAN infected,
	ULONG mediaType,
	ULONG mediaInfo,
	PETHREAD petr
	)
{
	_Trace(TraceInfo,"FileOpen handler: %s\n",plainFilename);
	return FileHandler(uncFilename, plainFilename, pPluginMessage, networkMount, networkAccess,
	 infected, mediaType, mediaInfo, petr);
}
static NTSTATUS RenameFileHandler(
	PCHAR uncFilename,
	PCHAR plainFilename,
	PPLUGINMESSAGE *pPluginMessage,
	BOOLEAN networkMount,
	BOOLEAN networkAccess,
	PBOOLEAN infected,
	ULONG mediaType,
	ULONG mediaInfo,
	PETHREAD petr
	)
{
	_Trace(TraceInfo,"FileRename handler: %s\n",plainFilename);
	return FileHandler(uncFilename, plainFilename, pPluginMessage, networkMount, networkAccess,
	 infected, mediaType, mediaInfo, petr);
}


static NTSTATUS ChangedFileHandler(
	PCHAR uncFilename,
	PCHAR plainFilename,
	PPLUGINMESSAGE *pPluginMessage,
	BOOLEAN networkMount,
	BOOLEAN networkAccess,
	PBOOLEAN infected,
	ULONG mediaType,
	ULONG mediaInfo,
	PETHREAD petr
	)
{
	if(/*networkAccess || */ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

	if(SetName)
	{
		KUstring st1(SetName+4,POOL);
		KUstring st2(plainFilename,POOL);
		if(RtlEqualUnicodeString(st1,st2,TRUE))
/*
		STRING st1,st2;
		RtlInitString(&st1,SetName+4);
		RtlInitString(&st2,plainFilename);
		if(0==RtlCompareString(&st1,&st2,TRUE))
*/		{
			baseShouldReload=1;//32;
			InternalAccessAdd(plainFilename);
			_Trace(TraceInfo,"ReloadRequest: %s\n",plainFilename);

		}
	}

 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=MFlags;

	strcpy(_so.Name,uncFilename);

	HandlerInfo hi;
    memset(&hi,0,sizeof(HandlerInfo));
	hi.uncFilename=uncFilename;
	hi.plainFilename=plainFilename;
	hi.pPluginMessage=pPluginMessage;
	hi.networkMount=networkMount;
	hi.networkAccess=networkAccess;
	hi.infected=infected;
	hi.mediaType=mediaType;
	hi.mediaInfo=mediaInfo;
	hi.petr=petr;
	hi.checkStopAfterDetect=1;

	return ProcessObject(&_so, &hi,TRUE);
}

static NTSTATUS ModifiedFileHandler(
	PCHAR uncFilename,
	PCHAR plainFilename,
	PPLUGINMESSAGE *pPluginMessage,
	BOOLEAN networkMount,
	BOOLEAN networkAccess,
	PBOOLEAN infected,
	ULONG mediaType,
	ULONG mediaInfo,
	PETHREAD petr
	)
{
	_Trace(TraceInfo,"FileModified handler: %s\n",plainFilename);
	return ChangedFileHandler(uncFilename, plainFilename, pPluginMessage, networkMount, networkAccess,
	 infected, mediaType, mediaInfo, petr);
}

static NTSTATUS CloseFileHandler(
	PCHAR uncFilename,
	PCHAR plainFilename,
	PPLUGINMESSAGE *pPluginMessage,
	BOOLEAN networkMount,
	BOOLEAN networkAccess,
	PBOOLEAN infected,
	ULONG mediaType,
	ULONG mediaInfo,
	PETHREAD petr
	)
{
	_Trace(TraceInfo,"FileClose handler: %s\n",plainFilename);
	return ChangedFileHandler(uncFilename, plainFilename, pPluginMessage, networkMount, networkAccess,
	 infected, mediaType, mediaInfo, petr);
}

#if GATEKEEPER_API_VERSION!=0x0100

static NTSTATUS ODScanFile(
	PCCHAR uncFilename,
	PCCHAR plainFilename, 
	PPLUGINMESSAGE *pPluginMessage,
	ULONG mediaType, 
	ULONG mediaInfo,
	DWORD taskID,
	DWORD action,
	BOOLEAN processArchives_
	)
{
	if(/*networkAccess || */ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=MFlagsODS;

    
	if(processArchives_){
		_so.MFlags|=MF_ARCHIVED;
		_so.MFlags|=MF_MAILBASES|MF_MAILPLAIN;
	}		
	else{
		_so.MFlags&=~MF_ARCHIVED;
		_so.MFlags&=~(MF_MAILBASES|MF_MAILPLAIN);
	}

	

		
	switch(action)
	{
	case FS_TASK_ACTION_DISINFECT:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_CUREALL;
		break;
	case FS_TASK_ACTION_DELETE:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DELETEALL;
		break;
	case FS_TASK_ACTION_RENAME:
	case FS_TASK_ACTION_ASK:
	case FS_TASK_ACTION_NONE:
	default:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DENYACCESS;
		break;
	}

	strcpy(_so.Name,uncFilename);

	HandlerInfo hi;
    memset(&hi,0,sizeof(HandlerInfo));
	hi.uncFilename=uncFilename;
	hi.plainFilename=plainFilename;
	hi.pPluginMessage=pPluginMessage;
	hi.mediaType=mediaType;
	hi.mediaInfo=mediaInfo;
	hi.dwTaskID=taskID;

	return ProcessObject(&_so, &hi,TRUE);
}


static NTSTATUS ODScanBootSector(
	PCCHAR driveLetter,
	PPLUGINMESSAGE *pPluginMessage, 
	DWORD taskID, 
	DWORD action
	)
{
	if(/*networkAccess || */ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=BFlagsODS;
	switch(action)
	{
	case FS_TASK_ACTION_DISINFECT:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_CUREALL;
		break;
	case FS_TASK_ACTION_DELETE:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DELETEALL;
		break;
	case FS_TASK_ACTION_RENAME:
	case FS_TASK_ACTION_ASK:
	case FS_TASK_ACTION_NONE:
	default:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DENYACCESS;
		break;
	}


	char* d="A:";
	d[0]=*driveLetter;

	strcpy(_so.Name,d);
	_so.SType=OT_BOOT; 
	_so.Drive=(d[0]|0x20)-'a';
	_so.Disk= ((_so.Drive)>1)?0x80:(_so.Drive);

	HandlerInfo hi;
    memset(&hi,0,sizeof(HandlerInfo));
	
	hi.uncFilename=_so.Name;
	hi.plainFilename=_so.Name;
	hi.driveLetter=(USHORT)driveLetter;
	hi.pPluginMessage=pPluginMessage;
	hi.dwTaskID=taskID?taskID:1;

	return ProcessObject(&_so, &hi,TRUE);
}
static NTSTATUS ODScanMBR(
	ULONG diskNumber,
	PPLUGINMESSAGE *pPluginMessage, 
	DWORD taskID, 
	DWORD action
	)
{
	if(/*networkAccess || */ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=BFlagsODS;
	switch(action)
	{
	case FS_TASK_ACTION_DISINFECT:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_CUREALL;
		break;
	case FS_TASK_ACTION_DELETE:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DELETEALL;
		break;
	case FS_TASK_ACTION_RENAME:
	case FS_TASK_ACTION_ASK:
	case FS_TASK_ACTION_NONE:
	default:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DENYACCESS;
		break;
	}

	sprintf(_so.Name,"%d",diskNumber);
	_so.MFlags|=MF_ALLENTRY;
	_so.SType=OT_MBR; 
	_so.Drive=2;
	_so.Disk=0x80+(BYTE)diskNumber;

	HandlerInfo hi;
    memset(&hi,0,sizeof(HandlerInfo));
	
	hi.uncFilename=_so.Name;
	hi.plainFilename=_so.Name;
	hi.pPluginMessage=pPluginMessage;
	hi.dwTaskID=taskID?taskID:1;

	return ProcessObject(&_so, &hi,TRUE);
}

int ReadString(char* buf,DWORD size, HANDLE f)
{
	DWORD bread;
	*buf=0;
	if( f==INVALID_HANDLE_VALUE)return 0;
	if( !_AvpReadFile(f,buf,size-1,&bread,NULL) || bread>size )return 0;
	buf[bread]=0;
	DWORD i;
	for( i=0;i<bread;i++ ,buf++){
		if(*buf==0x0D || *buf==0x0A || *buf==0){
			*buf=0;
			buf++;i++;
			if(*buf==0x0D || *buf==0x0A )i++;
			_AvpSetFilePointer(f,i-bread,NULL,FILE_CURRENT);
			break;
		}
	}
	return i;
}

#ifdef _DEBUG
//#define _DEBUG_DBVALID
#endif

BOOL DBValidation( PINTERNALMESSAGE msg)
{
#ifdef _DEBUG_DBVALID
	_asm int 3;
#endif
	_Trace(TraceInfo,"DB Validation: start\n");

	BOOL valid=1;
	char* set_current=new char[0x200];
	if(!set_current)return 0;
	char* set_new=new char[0x200];
	if(!set_new){
		delete set_current;
		return 0;
	}

	KUstring s(0x200,POOL);
	s.Assign(BasePath);
	s.Append(SetFile);
	RtlZeroMemory(set_current,0x200);
	s.ToPsz(set_current,0x200);
	if(msg->InputData==NULL)
		strcpy(set_new,set_current);
	else {
		strcpy(set_new,(*((char*)msg->InputData+1)==':')?NT_ROOT_PREFIX:"");
		strcat(set_new,(LPSTR)msg->InputData);
		int l=strlen(set_new);
		if(!l || set_new[l-1]!='\\'){
			strcat(set_new,"\\");
			l++;
		}
		s.Assign(SetFile);
		s.ToPsz(set_new+l,0x200-l);
	}

	char* n;
	char* n2;
	_AvpGetFullPathName(set_new,0x200,set_new,&n);
	_AvpGetFullPathName(set_current,0x200,set_current,&n2);

	_Trace(TraceInfo,"DB Validation cur set: %s\n",set_current);
	_Trace(TraceInfo,"DB Validation new set: %s\n",set_new);

	DWORD status=FSAV_DB_MISSING;
	DWORD filecount=0;
	DWORD l=sizeof(DWORD)*3;
	msg->dwStatus=FS_STATUS_OK;
	
	HANDLE f=INVALID_HANDLE_VALUE;

	do{
		if(*n==';')continue;
		if(*n==' ')continue;
		if(*n== 0 )continue;
		strcpy(n2,n);
		char* set=set_new;
again:
		switch(sign_check_file(set,1,vndArray,vndArrayCount,0))
		{
		case SIGN_OK:
			status=FSAV_DB_VALID;
			if(f==INVALID_HANDLE_VALUE)
				f=_AvpCreateFile(set_new,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			break;
		case SIGN_FILE_NOT_FOUND:
		case SIGN_CANT_READ:
			status=FSAV_DB_MISSING;
			if(set==set_new){
				set=set_current;
				goto again;
			}
			break;
		case SIGN_BAD_FILE:
		case SIGN_NOT_MATCH:
			status=FSAV_DB_CORRUPT;
			break;
		case SIGN_BUFFER_HAS_NO_NOTARIES:
		case SIGN_NOTARY_NOT_FOUND:
		case SIGN_BAD_OPEN_KEY:
			status=FSAV_DB_NOTDB;
			break;
		case SIGN_BAD_KEY_VERSION:
			status=FSAV_DB_VERSION;
			break;

// FSAV_DB_NOTCHECKED	
// FSAV_DB_DELETE		
// FSAV_DB_INVALID

		case SIGN_UNKNOWN_ERROR:
		case SIGN_NOT_ENOUGH_MEMORY:
		case SIGN_CANT_SIGN:
		case SIGN_BAD_PARAMETERS:
		case SIGN_ALREADY_SIGNED:
		case SIGN_BAD_PASSWORD:
		case SIGN_CALLBACK_FAILED:
		case SIGN_BUFFER_TOO_SMALL:
		case SIGN_CANT_WRITE:
			msg->dwStatus=FS_STATUS_FAILURE;
		default:
			status=FSAV_DB_NOTCHECKED;
			break;
		}
		filecount++;
		DWORD sl=1+strlen(n);
		if(l+sl+4*sizeof(DWORD) >= msg->dwOutputLength){
			msg->dwStatus=FS_STATUS_OUTPUTBUFFERTOOSMALL;
			l+=sl+4*sizeof(DWORD);
		}
		else{
			strcpy((char*)(msg->OutputData)+l,n);
			l+=sl;
			*(DWORD*)(msg->OutputData+l)=status;
			l+=sizeof(DWORD);
			*(DWORD*)(msg->OutputData+l)=0; //date
			l+=sizeof(DWORD);
			*(DWORD*)(msg->OutputData+l)=0; //reserved 1
			l+=sizeof(DWORD);
			*(DWORD*)(msg->OutputData+l)=0; //reserved 2
			l+=sizeof(DWORD);

			_Trace(TraceInfo,"DB Validation stat=%d %s\n",status,n);
		}

		if(status==FSAV_DB_VALID || status==FSAV_DB_NOTCHECKED)continue;
		valid=0;
	}while((f!=INVALID_HANDLE_VALUE) && ReadString(n,0x200-(n-set_new),f));

	if(f!=INVALID_HANDLE_VALUE)AvpCloseHandle(f);

	
	if(l+2 >= msg->dwOutputLength){
		msg->dwStatus=FS_STATUS_OUTPUTBUFFERTOOSMALL;
	}else {
		*(DWORD*)(msg->OutputData)=filecount;
		*(DWORD*)(msg->OutputData+4)=0; //reserved 1
		*(DWORD*)(msg->OutputData+8)=0; //reserved 2
		*(WORD*)(msg->OutputData+l)=0;
	}
	l+=2;
	msg->dwOutputLength=l;
	
	delete set_new;
	delete set_current;
	return valid;
}

long InternalMessageHandler( PINTERNALMESSAGE msg)
{
	switch(msg->dwType)
	{
	case FS_INTERNAL_MSG_DBRELOAD:
		{
			_Trace(TraceInfo,"Reload by FS_INTERNAL_MSG_DBRELOAD\n");
			//turbpa010412-[14323]{
			//		return NT_SUCCESS(BaseLoad());
			NTSTATUS Ret = BaseLoad();
			if( NT_SUCCESS( Ret ) )
			{
				if( DisableDBCleanUp == FALSE  )
				{
					AVP_CLEANUP_DB_RETURN_VALUE Res = 
						AVP_CLEANUP_DB_INTERNAL_ERROR;
						AVPCleanUpUnUsedFiles( SetName );
					_Trace(TraceInfo,"Delete OLD DB files, Set=%s Res=%i\n", SetName, (int)Res);
				}
			}
			return NT_SUCCESS( Ret );
			//turbpa010412-[14323]}
		}

	case FS_INTERNAL_MSG_AUTORELOAD_ON:
		Autoreload=1;
		break;

	case FS_INTERNAL_MSG_AUTORELOAD_OFF:
		Autoreload=0;
		break;

	case FS_INTERNAL_MSG_SCANNINGSTATUS:
		msg->dwStatus=(!ShutDown && Enable && BaseInfo->Status==BI_LOADED && BaseInfo->NumberOfRecords)?FS_STATUS_OK:FS_STATUS_FAILURE;
		break;

	case FS_INTERNAL_MSG_DBVALIDATION:
		_Trace(TraceInfo,"Validation by FS_INTERNAL_MSG_DBVALIDATION\n");
		return DBValidation(msg);

	case FS_INTERNAL_MSG_CANCEL:
		if(TaskID==*(DWORD*)msg->InputData)
		{
			BAvpCancelProcessObject(1);
		}
		break;

		//turbpa010412-[14323]{
	case FS_INTERNAL_MSG_DELETE_OLD_DB:
		{
			DisableDBCleanUp = msg->dwStatus > 0 ? TRUE : FALSE;
		}
		break;
		//turbpa010412-[14323]}
		
	default:
		break;
	}
	return 1;
}

#endif //GATEKEEPER_API_VERSION!=0x0100


static NTSTATUS MountVolumeHandler(
	PDEVICE_OBJECT pDeviceObject, 
	WCHAR driveLetter, 
	PPLUGINMESSAGE *pPluginMessage)
{
	if(ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
 	AVPScanObject* so=&_so;
	
	memset(so,0,sizeof(AVPScanObject));
    
	char* d="A:";
	d[0]=(char)driveLetter;

	strcpy(so->Name,"Boot sector of disk ");
	strcat(so->Name,d);
	so->MFlags=BFlags;
	so->SType=OT_BOOT; 
	so->Drive=(d[0]|0x20)-'a';
	so->Disk=so->Drive;


	switch (pDeviceObject->DeviceType)
	{
	case FILE_DEVICE_VIRTUAL_DISK:
		so->Disk=0x40;
		break;
	case FILE_DEVICE_CD_ROM:
		so->Disk=0x60 | so->Drive;
		break;
	case FILE_DEVICE_DISK:
		if(pDeviceObject->Characteristics & FILE_REMOTE_DEVICE)
			so->Disk=0x20;
		if(!(pDeviceObject->Characteristics & FILE_FLOPPY_DISKETTE))
			so->Disk=0x80;
		break;
	default:
		break;
	}

	HandlerInfo hi;
    memset(&hi,0,sizeof(HandlerInfo));
	hi.driveLetter=driveLetter;
	hi.pDeviceObject=pDeviceObject;
	hi.pPluginMessage=pPluginMessage;
	hi.uncFilename=so->Name;
	hi.plainFilename=so->Name;

	return ProcessObject(so, &hi,2);
}

static LRESULT WINAPI NewAvpCallback(WPARAM wParam,LPARAM lParam)
{
	LRESULT ret=0;
	AVPScanObject* so=(AVPScanObject*)lParam;
	char* s=0;


//    _DebugTrace(TraceInfo,"AvpCallback %04X %08X\n",wParam, lParam);

	switch(wParam){
	case AVP_CALLBACK_PUT_STRING:
		_Trace(TraceInfo,"<<< %s\n",lParam);
		break;
	case AVP_CALLBACK_ASK_DELETE:
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
					break;
				}
				ret=2;
				break;
			case MF_D_DELETEALL:
				ret=2;
				break;
			case MF_D_DENYACCESS:
			default:
				break;
		}
		break;

	case AVP_CALLBACK_ASK_CURE:
		{
			switch((so->MFlags) & MF_D_)
			{
			case MF_D_ASKCURE:
			case MF_D_ASKCUREDEBUG:
				break;
			case MF_D_CUREALL:
				ret=1;
				break;
			case MF_D_DELETEALL:
				ret=2;
				break;
			case MF_D_DENYACCESS:
			default:
				break;
			}
		}
		break;

	case AVP_CALLBACK_LOAD_PROGRESS:
		{
			LOAD_PROGRESS_STRUCT* lps=(LOAD_PROGRESS_STRUCT*)lParam;
			BaseInfo->NumberOfVirNames=lps->virCount;
			BaseInfo->NumberOfRecords=lps->recCount;
			strcpy(BaseInfo->SetName,lps->setName);
			BaseInfo->NumberOfBases = lps->NumberOfBases;
			BaseInfo->LastUpdateYear = lps->LastUpdateYear;
			BaseInfo->LastUpdateMonth = lps->LastUpdateMonth;
			BaseInfo->LastUpdateDay = lps->LastUpdateDay;
			BaseInfo->LastUpdateTime = lps->LastUpdateTime;
//			_DebugTrace(TraceInfo,"AvpLoading: %s %d\n",lps->curName,lps->curPos);
		}
        break;



	case AVP_CALLBACK_OBJECT_WARNING:
		s=new char[0x280];
		if(!s)
			break;

		StatMutex.Wait();
		if(!(so->MFlags & MF_APC_CURE_REQUEST))
			Stat->Warnings++;
		BAvpMakeFullName(Stat->LastVirusName,so->WarningName);
		makePlainName(Stat->LastInfectedObjectName,so->Name);
		if(MessObjWarning)
			sprintf(s,MessObjWarning,	Stat->LastInfectedObjectName, Stat->LastVirusName);
		else
			sprintf(s,"Object %s warning: %s.",	Stat->LastInfectedObjectName, Stat->LastVirusName);
		StatMutex.Release();

		wGkSendMessage(s,so,ACTION_ACCESS_DENIED);
//		_DebugTrace(TraceInfo,"%s\n",s);

		break;

	case AVP_CALLBACK_OBJECT_SUSP:
		s=new char[0x280];
		if(!s)
			break;

		StatMutex.Wait();
		if(!(so->MFlags & MF_APC_CURE_REQUEST))
			Stat->Suspicions++;
		BAvpMakeFullName(Stat->LastVirusName,so->SuspicionName);
		makePlainName(Stat->LastInfectedObjectName,so->Name);

		if(MessObjSuspicion)
			sprintf(s,MessObjSuspicion,	Stat->LastInfectedObjectName, Stat->LastVirusName);
		else
		sprintf(s,"Object %s suspicion: %s.",	Stat->LastInfectedObjectName, Stat->LastVirusName);
		StatMutex.Release();
		wGkSendMessage(s,so,ACTION_ACCESS_DENIED);
//		_DebugTrace(TraceInfo,"%s\n",s);
		break;
	
	case AVP_CALLBACK_OBJECT_DETECT:
		s=new char[0x280];
		if(!s)
			break;

		StatMutex.Wait();
		if(!(so->MFlags & MF_APC_CURE_REQUEST))
			Stat->Infected++;
		BAvpMakeFullName(Stat->LastVirusName,so->VirusName);
		makePlainName(Stat->LastInfectedObjectName,so->Name);

		if(MessObjInfected)
			sprintf(s,MessObjInfected,	Stat->LastInfectedObjectName, Stat->LastVirusName);
		else
		sprintf(s,"Object %s infected: %s.",Stat->LastInfectedObjectName, Stat->LastVirusName);
		StatMutex.Release();
		wGkSendMessage(s,so,ACTION_ACCESS_DENIED);
//		_DebugTrace(TraceInfo,"%s\n",s);
        break;

	case AVP_CALLBACK_OBJECT_CURE:
		s=new char[0x280];
		if(!s)
			break;
		StatMutex.Wait();
		Stat->Disinfected++;

		if(MessObjDisinfected)
			sprintf(s,MessObjDisinfected,	Stat->LastInfectedObjectName, Stat->LastVirusName);
		else
		sprintf(s,"Object %s disinfected: %s.",	Stat->LastInfectedObjectName, Stat->LastVirusName);
		StatMutex.Release();
		wGkSendMessage(s,so,ACTION_ACCESS_DENIED_DISINFECTED);
//		_DebugTrace(TraceInfo,"%s\n",s);

        break;

	case AVP_CALLBACK_OBJECT_DELETE:
		s=new char[0x280];
		if(!s)
			break;
		StatMutex.Wait();
		Stat->Deleted++;

		if(MessObjDeleted)
			sprintf(s,MessObjDeleted,	Stat->LastInfectedObjectName, Stat->LastVirusName);
		else
		sprintf(s,"Object %s deleted: %s.",	Stat->LastInfectedObjectName, Stat->LastVirusName);
		StatMutex.Release();
 		wGkSendMessage(s,so,ACTION_ACCESS_DENIED_REMOVED);
//		_DebugTrace(TraceInfo,"%s\n",s);
        break;

	case AVP_CALLBACK_OBJECT_CURE_FAIL:
		s=new char[0x280];
		if(!s)
			break;
		StatMutex.Wait();
		sprintf(s,"Object %s disinfection failed: %s.",	Stat->LastInfectedObjectName, Stat->LastVirusName);
		StatMutex.Release();
		wGkSendMessage(s,so,ACTION_ACCESS_DENIED);
//		_DebugTrace(TraceInfo,"%s\n",s);

        break;

	case AVP_CALLBACK_OBJECT_DONE:
		s=new char[0x280];
		if(!s)
			break;

		{
			StatMutex.Wait();
			HandlerInfo* hi=(HandlerInfo*)so->Connection;
			RFlags|=so->RFlags;
			if(!((so->RFlags) & (RF_NOT_A_PROGRAMM|RF_ERROR|RF_NOT_OPEN)))
			{
				if(!(so->MFlags & MF_APC_CURE_REQUEST))
					Stat->Objects++;
				makePlainName(Stat->LastObjectName,so->Name);
			}
			if(so->InArc){
#ifdef _DEBUG
				char n[0x200];
				makePlainName(n,so->Name);
				int i=strlen(n);
//	_DebugTrace(TraceInfo,"%-55.55s Q:%d RF:%08X\n",(i<55)?(n):(n+i-55),BusyCounter.Test(),so->RFlags);
#endif
				if(hi && hi->checkStopAfterDetect)
//				if(StopProcessArchivesAfterDetection)
				if(RFlags & (RF_DETECT|RF_WARNING|RF_SUSPIC))
					BAvpCancelProcessObject(1);
			}

			if((so->RFlags & (RF_DETECT|RF_WARNING|RF_SUSPIC)) && !(so->RFlags & RF_REPORT_DONE)){
#if GATEKEEPER_API_VERSION!=0x0100
				if(hi && (hi->dwTaskID==0))
#endif
				{
					DetectArrayAdd(so,hi?(hi->plainFilename):NULL,add_detect);
					DetectSetEvent();
				}
				if(so->RFlags & RF_CURE_FAIL)
				{
					sprintf(s,"Object %s disinfection failed: %s.",	Stat->LastInfectedObjectName, Stat->LastVirusName);
					wGkSendMessage(s,so,ACTION_ACCESS_DENIED);
				}
			}
			StatMutex.Release();

		}

		if(so->RFlags & RF_REPORT_DONE)
			break;
/*
#if GATEKEEPER_API_VERSION!=0x0100
		if(so->RFlags & RF_ERROR)
			GkSendErrorMessage("Some error", Trap_FSAV_scanningError);
#endif
*/
        break;

	case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
		InternalAccessAdd((const char*) lParam);
		ret=sign_check_file( (char*)lParam, 1, vndArray, vndArrayCount, 0);
		break;

	case AVP_CALLBACK_ERROR_FILE_OPEN:
	case AVP_CALLBACK_ERROR_SET:

#if GATEKEEPER_API_VERSION!=0x0100
		GkSendErrorMessage((char *)lParam, Trap_FSAV_missingDatabaseFile);
#endif
		break;

/*
	case AVP_CALLBACK_ERROR_KERNEL_PROC:
	case AVP_CALLBACK_ERROR_LINK:
	case AVP_CALLBACK_ERROR_SYMBOL_DUP:
	case AVP_CALLBACK_ERROR_SYMBOL:
	case AVP_CALLBACK_ERROR_FIXUPP:
		GkSendErrorMessage((char *)lParam, Trap_FSAV_cannotOpenFile);
		break;
*/
		
	case AVP_CALLBACK_SETFILE_INTEGRITY_FAILED:
#if GATEKEEPER_API_VERSION!=0x0100
//		GkSendErrorMessage((char *)lParam, Trap_FSAV_databaseFileCorrupt);
#endif
		goto integ_fail;
		break;

	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
#if GATEKEEPER_API_VERSION!=0x0100
		GkSendErrorMessage((char *)lParam, Trap_FSAV_databaseFileCorrupt);
#endif
integ_fail:
		{
			AVPScanObject so;
			memset(&so,0,sizeof(AVPScanObject));
			strcpy(so.Name,(char*)lParam);
			so.RFlags = RF_REPORT_DONE;
			so.Type = 0x8001;
			so.Connection=ret;
			DetectArrayAdd(&so,NULL,1);
		}
		break;
	
	case AVP_CALLBACK_ERROR_BASE:
#if GATEKEEPER_API_VERSION!=0x0100
		GkSendErrorMessage((char *)lParam, Trap_FSAV_notADatabaseFile);
#endif
		break;

	case AVP_CALLBACK_ROTATE: 
	case AVP_CALLBACK_ROTATE_OFF:
		break;

	default:
//	        _DebugTrace(TraceInfo,"AvpCallback %X \n",wParam);
        break;
	}

	if(s)
	delete s;
    return ret;
}

static	ChangeAnsiString( PCHAR* dest,IN PWSTR src, POOL_TYPE pool)
{
	KUstring s(src,POOL);
	int l=s.Size(); //it is size of unicode in bytes, it is definitely more than MBCS size.
	if(l){
		CHAR* buf=new (pool)CHAR[l];
		if(buf){
			s.ToPsz(buf,l);
			if(*dest) delete *dest;
			*dest = buf;
			return l;
		}
	}
	return 0;
}
void AddMaskEx(char* b, BOOL exclude)
{
	if(exclude)	BAvpAddExcludeMask(b);
	else		BAvpAddMask(b);
	if(b && strlen(b)>3 && *(WORD*)(b+1)=='\\:'){
		char s[200];
		strcpy(s,"\\??\\");
		strcat(s,b);
		if(exclude)	BAvpAddExcludeMask(s);
		else		BAvpAddMask(s);
		strcpy(s,"\\DosDevices\\");
		strcat(s,b);
		if(exclude)	BAvpAddExcludeMask(s);
		else		BAvpAddMask(s);
		strcpy(s,"\\Device\\Harddisk?\\Partition?\\");
		strcat(s,b+3);
		if(exclude)	BAvpAddExcludeMask(s);
		else		BAvpAddMask(s);
		strcpy(s,"\\Device\\HarddiskVolume?\\");
		strcat(s,b+3);
		if(exclude)	BAvpAddExcludeMask(s);
		else		BAvpAddMask(s);

	}
}

static SetMasks(PWSTR str, BOOL exclude)
{
	int ret=0;
	char* buff=new char[0x200];
	KUstring s(str,POOL);
	if(s.Length()>180) return ret;
	s.ToPsz(buff,0x200);
	int l=strlen(buff);
	char* b=buff+l;
	while(b>buff){
		b--;
		if(b==buff)goto add;
		if(*b==','){
			*b=0;
			b++;
add:
			AddMaskEx(b, exclude);
			ret++;
		}
	}
	delete buff;
	return ret;
}

static NTSTATUS SettingsModifiedHandler()
{
	NTSTATUS ret=STATUS_INSUFFICIENT_RESOURCES;
	KRegistryKey Params(sRegistryPath, L"Parameters");
	if (!NT_SUCCESS(Params.LastError()))
		return STATUS_INSUFFICIENT_RESOURCES;

#ifdef _TRACER
	Params.QueryValue(L"TraceDevice", (PULONG) &defTraceDevice);
	Params.QueryValue(L"TraceOutputLevel", (PULONG) &defTraceOutputLevel);
	Params.QueryValue(L"TraceBreakLevel", (PULONG) &defTraceBreakLevel);
	Tracer.SetTarget(defTraceDevice);
	Tracer.SetOutputLevel(defTraceOutputLevel);
	Tracer.SetBreakLevel(defTraceBreakLevel);
#endif

	_Trace(TraceInfo, "Tracing level %d\n",defTraceOutputLevel);


#define WSTR_LEN 0x200
	ULONG RegLength = WSTR_LEN;
	PWSTR str=new (POOL)WCHAR[WSTR_LEN];

  if(str)
  {
	*str=0;
#if GATEKEEPER_API_VERSION!=0x0100
	PCHAR ss = NULL;
	if(DFP_SUCCESS == FSGetPolicyString(OID_FSAV_language, &ss))
	{
		KUstring uniName(ss,NonPagedPool);
		uniName.ToWstr(str,WSTR_LEN);
	}else
#endif
	{	
		*str=0;
		RegLength = WSTR_LEN;
		if(NT_SUCCESS(Params.QueryValue(L"Language", str, RegLength, NonPagedPool)))
			if(!RegLength)
				*str=0;
	}

	if(*str)
	{
		KUstring messkey(0x200,NonPagedPool);
		messkey.Assign(sRegistryPath);
		messkey.Append(L"\\Messages");
		KRegistryKey Messages(messkey, str);
		if (NT_SUCCESS(Messages.LastError()))
		{
			_Trace(TraceInfo, "Load Messages\n");
			*str=0;
			RegLength = WSTR_LEN;
			if(NT_SUCCESS(Messages.QueryValue(L"ObjWarning", str, RegLength, NonPagedPool)))
				if(RegLength)	ChangeAnsiString(&MessObjWarning,str,NonPagedPool);

			*str=0;
			RegLength = WSTR_LEN;
			if(NT_SUCCESS(Messages.QueryValue(L"ObjSuspicion", str, RegLength, NonPagedPool)))
				if(RegLength)	ChangeAnsiString(&MessObjSuspicion,str,NonPagedPool);

			*str=0;
			RegLength = WSTR_LEN;
			if(NT_SUCCESS(Messages.QueryValue(L"ObjInfected", str, RegLength, NonPagedPool)))
				if(RegLength)	ChangeAnsiString(&MessObjInfected,str,NonPagedPool);
			
			*str=0;
			RegLength = WSTR_LEN;
			if(NT_SUCCESS(Messages.QueryValue(L"ObjDisinfected", str, RegLength, NonPagedPool)))
				if(RegLength)	ChangeAnsiString(&MessObjDisinfected,str,NonPagedPool);

			*str=0;
			RegLength = WSTR_LEN;
			if(NT_SUCCESS(Messages.QueryValue(L"ObjDeleted", str, RegLength, NonPagedPool)))
				if(RegLength)	ChangeAnsiString(&MessObjDeleted,str,NonPagedPool);
		}
	}


	Params.QueryValue(L"AllowUnload", (PULONG)&AllowUnload);
	if(AllowUnload) pFSAVPDriver->EnableUnload();
	else pFSAVPDriver->DisableUnload();

	Params.QueryValue(L"EventLogging", (PULONG)&EventLogging);
	Params.QueryValue(L"Enable", (PULONG)&Enable);
	Params.QueryValue(L"MFlags", (PULONG)&MFlags);
	MFlagsODS=MFlags;
	
	LONG Action=0;


#if GATEKEEPER_API_VERSION!=0x0100
DWORD v;
if(DFP_SUCCESS == FSGetPolicyInteger(OID_FSAV_fileScanInsideCompressedExecutables, &v))
{
	if(v)MFlags|=MF_PACKED;
	else MFlags&=~MF_PACKED;
}
if(DFP_SUCCESS == FSGetPolicyInteger(OID_FSAV_fileScanInsideArchives, &v))
{
	if(v)MFlags|=MF_ARCHIVED;
	else MFlags&=~MF_ARCHIVED;

	if(v)MFlags|=MF_MAILBASES|MF_MAILPLAIN;
	else MFlags&=~(MF_MAILBASES|MF_MAILPLAIN);
}
if(DFP_SUCCESS == FSGetPolicyInteger(OID_FSAV_fileScanInsideCompressedExecutables_ODS, &v))
{
	if(v)MFlagsODS|=MF_PACKED;
	else MFlagsODS&=~MF_PACKED;
}
if(DFP_SUCCESS == FSGetPolicyInteger(OID_FSAV_fileScanInsideArchives_ODS, &v))
{
	if(v)MFlagsODS|=MF_ARCHIVED|MF_MAILBASES|MF_MAILPLAIN;
	else MFlagsODS&=~(MF_ARCHIVED|MF_MAILBASES|MF_MAILPLAIN);
}

	Action=2;
	if(DFP_SUCCESS != FSGetPolicyInteger(OID_FSAV_actionOnInfectedBootSectors, (DWORD*)&Action))
		Params.QueryValue(L"Action", (PULONG)&Action);

	switch(Action){
	case 2: BFlags=(MFlags&~MF_D_)|MF_D_CUREALL; break;
	case 4: BFlags=(MFlags&~MF_D_)|MF_D_DELETEALL; break;
	default: 
	case 0: BFlags=(MFlags&~MF_D_)|MF_D_DENYACCESS; 
		break;
	}


	Action=2;
	if(DFP_SUCCESS != FSGetPolicyInteger(OID_FSAV_fileActionOnInfected, (DWORD*)&Action))
#endif
		Params.QueryValue(L"Action", (PULONG)&Action);

	switch(Action){
	case 2: MFlags=(MFlags&~MF_D_)|MF_D_CUREALL; break;
	case 4: MFlags=(MFlags&~MF_D_)|MF_D_DELETEALL; break;
	default: 
	case 0: MFlags=(MFlags&~MF_D_)|MF_D_DENYACCESS; 
		break;
	}

	Params.QueryValue(L"QueueMax", (PULONG)&QueueMax);
	Params.QueryValue(L"StopProcessArchivesAfterDetection", (PULONG)&StopProcessArchivesAfterDetection); 
	Params.QueryValue(L"LimitCompoundSize", (PULONG)&LimitCompoundSize); 
	Params.QueryValue(L"DisableSectorScan",	(PULONG)&DisableSectorScan); 

	*str=0;
	RegLength = WSTR_LEN;
	if(NT_SUCCESS(Params.QueryValue(L"SetFile", str, RegLength, NonPagedPool)))
		if(RegLength)
			SetFile.Assign(str);

	*str=0;
	RegLength = WSTR_LEN;
	if(NT_SUCCESS(Params.QueryValue(L"TempPath", str, RegLength, NonPagedPool)))
	{
		if(!RegLength) goto temproot;
		TempPath.Assign(str);
		if(TempPath.Last()!=L'\\'){
			TempPath.Append(L"\\");
		}
		HANDLE h=INVALID_HANDLE_VALUE;
		char* b= new char[0x200];
		if(b)
		{
			TempPath.ToPsz(b,0x200);
			strcat(b,"avp000.tmp");
			_DebugTrace(TraceInfo,"Trying Check Temp %s\n",b);
			h=_AvpCreateFile(b,GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ,0,CREATE_ALWAYS,0,0);
			if(h!=INVALID_HANDLE_VALUE)
			{
				AvpCloseHandle(h);
				AvpDeleteFile(b);
			}
			delete b;
		}
		if(h==INVALID_HANDLE_VALUE)
			goto temproot;
	}
	else
	{
temproot:
		TempPath.Assign(LNT_ROOT_PREFIX);
		TempPath.Append(L"C:\\");
	}
	
	BasePath.Assign(L"");

	*str=0;
	RegLength = WSTR_LEN;
	if(NT_SUCCESS(Params.QueryValue(L"BasePath", str, RegLength, NonPagedPool)))
		if(RegLength){
			BasePath.Append(str);
			if(BasePath.Last()!=L'\\')
				BasePath.Append(L"\\");
		}

	KUstring s(0x200,POOL);
	char* tn=new char[0x200];
	if(tn)
	{
		s.Assign(BasePath);
		s.Append(L"AVP.VND");
		RtlZeroMemory(tn,0x200);
		s.ToPsz(tn,0x200);
		VndInit(tn);

		s.Assign(BasePath);
		s.Append(SetFile);
		RtlZeroMemory(tn,0x200);
		s.ToPsz(tn,0x200);

		KUstring st1(SetName,POOL);
		KUstring st2(tn,POOL);
		if(!RtlEqualUnicodeString(st1,st2,TRUE))
		{
			strcpy(SetName,tn);
			ret=BaseLoad();
		}
		delete tn;
	}

	Params.QueryValue(L"GKRegister", (PULONG)&GKRegister);

	ULONG priority;
	Params.QueryValue(L"Priority", (PULONG)&priority);
/*
	if(GKRegister && CallbackHandle && Priority!=priority)
	{
		ret=BaseDeregister();
		if(!NT_SUCCESS(ret)){
			_Trace(TraceError, "Failed to Change Priority in FSGK");
			goto ret;
		}
	}
*/
	Priority=priority;

	if(GKRegister && !CallbackHandle)
	{
		ret=BaseRegister();
		if(!NT_SUCCESS(ret)){
			_Trace(TraceError, "Failed to register in FSGK");
			goto ret;
		}
	}
/*
#ifdef _DEBUG
	{
		PWSTR ws=0;
		RegLength = 0;
		CHAR* test=new CHAR[2];
		strcpy(test,"*");


		_asm int 3;
		Params.QueryValue(L"InclusionList", ws, RegLength, NonPagedPool);
		if(ws){
			ChangeAnsiString(&test,ws,NonPagedPool);
			delete ws;
		}
		delete test;
	}
#endif
*/
	
#if GATEKEEPER_API_VERSION==0x0100
	if(CallbackHandle && pinf)
	{
		PWSTR ws=0;
		RegLength = 0;
		Params.QueryValue(L"InclusionList", ws, RegLength, NonPagedPool);
		if(ws){
			ChangeAnsiString(&pinf->FilterExtensions,ws,NonPagedPool);
			delete ws;
		}

		ws=0;
		RegLength = 0;
		Params.QueryValue(L"ExclusionList", ws, RegLength, NonPagedPool);
		if(ws){
			ChangeAnsiString(&pinf->ExcludeExtensions,ws,NonPagedPool);
			delete ws;
		}
		_ChangeExtensions(pinf);
	}
#endif

	*str=0;
	RegLength = WSTR_LEN;
	Params.QueryValue(L"IncludeMask", str, RegLength, NonPagedPool);
	SetMasks(str,0);

	*str=0;
	RegLength = WSTR_LEN;
	Params.QueryValue(L"ExcludeMask", str, RegLength, NonPagedPool);
	SetMasks(str,1);

	ret=STATUS_SUCCESS;
  }

ret:
	if(str)delete str;
	return ret;
}



#pragma code_seg("INIT")

static NTSTATUS ScanMBRs()
{
	if(ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
 	AVPScanObject* so=&_so;

	ANSI_STRING ansiString;
	UNICODE_STRING uniName;
	char deviceName[0x100];
	PFILE_OBJECT fileObject;
	PDEVICE_OBJECT deviceObject;
	
	for(int disk=0;;disk++){
		sprintf(deviceName,"\\Device\\Harddisk%d\\Partition0",disk);
		RtlInitAnsiString(&ansiString,(const char*)deviceName);
		RtlAnsiStringToUnicodeString(&uniName,&ansiString,TRUE);
		NTSTATUS status=IoGetDeviceObjectPointer(
			&uniName,
			FILE_READ_ATTRIBUTES,
			&fileObject,
			&deviceObject);
		RtlFreeUnicodeString(&uniName);
		if(!NT_SUCCESS(status)) break;
		ObDereferenceObject(fileObject);


		memset(so,0,sizeof(AVPScanObject));
		sprintf(so->Name,"Master Boot Record of HDD%d",disk);
		so->MFlags=BFlags;
		so->MFlags|=MF_ALLENTRY;
		so->SType=OT_MBR; 
		so->Drive=2;
		so->Disk=0x80+disk;


		add_detect=1;
		ProcessObject(so);
		add_detect=0;

		if(sm_action)GkSendMessage(sm_s, so, sm_action);
	}
	return STATUS_SUCCESS;
}
/*
BOOLEAN IsFloppyInserted(PUNICODE_STRING driveName)
{
	NTSTATUS status;
	PFILE_OBJECT fileObject;
	PDEVICE_OBJECT deviceObject;
	LARGE_INTEGER liSeekPos;
	KEVENT event;
	IO_STATUS_BLOCK IoStatus;
	PIRP irp;
	PUCHAR buffer;

	status = IoGetDeviceObjectPointer( driveName,
										FILE_READ_ATTRIBUTES,
										&fileObject,
                                        &deviceObject);

	if (NT_SUCCESS(status))
	{
		deviceObject = fileObject->DeviceObject;

		buffer = (PUCHAR)ExAllocatePool(NonPagedPool, 512);
		liSeekPos.QuadPart = 0L;

		irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ,
										deviceObject,
										buffer,
										512,
										&liSeekPos,
										&event,
    									&IoStatus);

		ObDereferenceObject(fileObject);


		if (!irp)
		{
			return FALSE;
		}

		// Set the event object to the unsignaled state.
		// It will be used to signal request completion.
		KeInitializeEvent(&event, NotificationEvent, FALSE);

		status = IoCallDriver(deviceObject, irp);

		if (status == STATUS_PENDING)
		{
			KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
			status = IoStatus.Status;
		}

		ExFreePool(buffer);

		if (NT_SUCCESS(status))
			return TRUE;
	}

	return FALSE;
}

*/

static NTSTATUS ScanBoots()
{
	if(ShutDown || !Enable /*|| Base==NULL*/) return STATUS_SUCCESS;

 	AVPScanObject _so;
 	AVPScanObject* so=&_so;

	ANSI_STRING ansiString;
	UNICODE_STRING uniName;
	char deviceName[0x100];
	PFILE_OBJECT fileObject;
	PDEVICE_OBJECT deviceObject;
	
	for(char disk='A';disk<='Z';disk++){ 
		sprintf(deviceName,"\\DosDevices\\%c:",disk);
		RtlInitAnsiString(&ansiString,(const char*)deviceName);
		RtlAnsiStringToUnicodeString(&uniName,&ansiString,TRUE);

		NTSTATUS status=IoGetDeviceObjectPointer(
			&uniName,
			FILE_READ_ATTRIBUTES,
			&fileObject,
			&deviceObject);
		if(!NT_SUCCESS(status)){
			RtlFreeUnicodeString(&uniName);
			continue;
		}
		ObDereferenceObject(fileObject);

		memset(so,0,sizeof(AVPScanObject));
		sprintf(so->Name,"Boot sector of disk %c:",disk);
		so->MFlags=BFlags;
		so->SType=OT_BOOT; 
		so->Drive=(disk|0x20)-'a';
		so->Disk=so->Drive;


		switch (deviceObject->DeviceType)
		{
		case FILE_DEVICE_VIRTUAL_DISK:
			so->Disk=0x40;
			break;
		case FILE_DEVICE_CD_ROM:
			so->Disk=0x60 | so->Drive;
			break;
		case FILE_DEVICE_DISK:
			if(deviceObject->Characteristics & FILE_REMOTE_DEVICE)
				so->Disk=0x20;
			if(deviceObject->Characteristics & (FILE_REMOVABLE_MEDIA | FILE_FLOPPY_DISKETTE))
			{
				continue;
/*
//FS request from 05.12.00
				if (!IsFloppyInserted(&uniName)){
					RtlFreeUnicodeString(&uniName);
					continue;
				}
*/			
			}
			else 	so->Disk=0x80;
			break;
		default:
			RtlFreeUnicodeString(&uniName);
			continue;
		}
		RtlFreeUnicodeString(&uniName);

		add_detect=1;
		ProcessObject(so);
		add_detect=0;

		if(sm_action)GkSendMessage(sm_s, so, sm_action);
	}
	return STATUS_SUCCESS;
}


NTSTATUS BaseInit()
{
//	CTN_SHABuffer( NULL, NULL, 1 );

	_Trace(TraceInfo,"\n");
	_Trace(TraceInfo,"%s\n",FSAVP_Regname);
	_Trace(TraceInfo,"%s\n",FSAVP_Version);
	_Trace(TraceInfo,"Compilation: %s\n",FSAVP_Compilation);

	InternalAccessInit();
//	ExInitializeFastMutex( &bMutex );
	KeInitializeSemaphore( &bSemafore,1,1);

	SetFile.Assign(L"AVP.SET");
	
	if(!LoadIO()) return STATUS_INSUFFICIENT_RESOURCES;
	AvpCallback = NewAvpCallback;


#ifdef __SEH__
	__try{
#endif
		if(!BAvpInit())
			return STATUS_INSUFFICIENT_RESOURCES;
#ifdef __SEH__
	}
	__except( 1 ) {
			return STATUS_INSUFFICIENT_RESOURCES;
	}
#endif

	for(int i=0;i<MESS_TEXT_MAX;i++)MessText[i]=0;

	if(!(Stat=new (NonPagedPool)AVPStat)){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(Stat,sizeof(AVPStat));

	if(!(BaseInfo=new (NonPagedPool)AVPBaseInfo)){
		delete Stat; Stat=NULL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(BaseInfo,sizeof(AVPBaseInfo));

	if(!(SetName=new (NonPagedPool)char[0x200])){
		delete Stat; Stat=NULL;
		delete BaseInfo; BaseInfo=NULL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(SetName,0x200);

	NTSTATUS ret=SettingsModifiedHandler();
	if(!NT_SUCCESS(ret)){
		BaseUnload();
		BAvpDestroy();
		return ret;
	}

DWORD sm=1;
#if GATEKEEPER_API_VERSION!=0x0100
FSGetPolicyInteger(OID_FSAV_scanBootBlocksAtStartup, &sm);
#endif
	if(sm)
	{
		ScanMBRs();
		ScanBoots();
	}


	return STATUS_SUCCESS;
}

#pragma code_seg()


NTSTATUS BaseRegister()
{
	NTSTATUS r=STATUS_INSUFFICIENT_RESOURCES;

	if(!GKRegister) return STATUS_SUCCESS;


	ULONG version=GATEKEEPER_VERSION;

	if(CallbackStruct) goto c_pinf;

	CallbackStruct= new (NonPagedPool)FILTER_CALLBACK;
	if (CallbackStruct==NULL)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	RtlZeroMemory(CallbackStruct,sizeof(FILTER_CALLBACK));
	CallbackStruct->FileOpen = OpenFileHandler;
	CallbackStruct->FileRename = RenameFileHandler;
	CallbackStruct->FileClose = CloseFileHandler;
	CallbackStruct->FileModified	= ModifiedFileHandler;
	CallbackStruct->MountVolume	= MountVolumeHandler;
	CallbackStruct->SettingsModified = SettingsModifiedHandler;

#if GATEKEEPER_API_VERSION!=0x0100
	CallbackStruct->ODScanFile = ODScanFile;
	CallbackStruct->ODScanBootSector = ODScanBootSector;
	CallbackStruct->ODScanMBR = ODScanMBR;
	CallbackStruct->InternalMessage = InternalMessageHandler; 
#endif

c_pinf:
	if(pinf) goto c_reg;

	pinf= new (NonPagedPool)PLUGIN_INFO;

	if (pinf==NULL)
	{
		delete CallbackStruct;
		CallbackStruct=NULL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(pinf,sizeof(PLUGIN_INFO));
	
	pinf->PlugInId = __FSAVP_ID__;

	pinf->PlugInName = new (NonPagedPool)CHAR[strlen(FSAVP_Regname)+1];
	if(pinf->PlugInName) strcpy(pinf->PlugInName, FSAVP_Regname);

#if GATEKEEPER_API_VERSION==0x0100
	pinf->FilterExtensions = new (NonPagedPool)CHAR[2];
	if(pinf->FilterExtensions) strcpy(pinf->FilterExtensions, "*");

	pinf->ExcludeExtensions = new (NonPagedPool)CHAR[1];
	if(pinf->ExcludeExtensions) strcpy(pinf->ExcludeExtensions, "");
#else
	pinf->Extensions = FS_EXTENSIONS_DEFAULT;
	{
		KRegistryKey Params(sRegistryPath, L"Parameters");
		if (NT_SUCCESS(Params.LastError()))
		{
			PWSTR ws=0;
			DWORD RegLength = 0;
			Params.QueryValue(L"InclusionList", ws, RegLength, NonPagedPool);
			if(ws){
				if(RegLength && *ws==L'*')
					pinf->Extensions = FS_EXTENSIONS_ALL;
				delete ws;
			}
		}
	}
#endif

c_reg:

#if GATEKEEPER_API_VERSION==0x0100
	r=_FilterRegister(pFSAVPDriver->DriverObject(),CallbackStruct,pinf,&version,1L,Priority,&CallbackHandle);
#else
	r=FilterRegister(pFSAVPDriver->DriverObject(),CallbackStruct,pinf,&version,1L,&CallbackHandle);
#endif

	_Trace(TraceInfo,"Register: %d  %08X\n",NT_SUCCESS(r),CallbackHandle);

	return r;
}


NTSTATUS SaveRegistry()
{
	KRegistryKey Params(sRegistryPath, L"Parameters");
	if (!NT_SUCCESS(Params.LastError()))
		return STATUS_INSUFFICIENT_RESOURCES;

	Params.WriteValue(L"EventLogging", (ULONG)EventLogging);
	Params.WriteValue(L"Enable", (ULONG)Enable);
	Params.WriteValue(L"MFlags", (ULONG)MFlags);
	Params.WriteValue(L"LimitCompoundSize", (ULONG)LimitCompoundSize); 

	return STATUS_SUCCESS;
}

NTSTATUS BaseDestroy()
{
	NTSTATUS ret;
	ret=BaseDeregister();
	ret=BaseUnload();
	BAvpDestroy();

	if(Stat)	delete Stat;
	if(BaseInfo)	delete BaseInfo;
	if(SetName)	delete SetName;
	
	if(CallbackStruct) {
		delete CallbackStruct;
		CallbackStruct=NULL;
	}

	if(pinf){
		if(pinf->PlugInName) 
			delete pinf->PlugInName;
#if GATEKEEPER_API_VERSION==0x0100
		if(pinf->FilterExtensions)
			delete pinf->FilterExtensions;
		if(pinf->ExcludeExtensions)
			delete pinf->ExcludeExtensions;
#endif
		delete pinf;
		pinf=NULL;
	}

	for(int i=0;i<MESS_TEXT_MAX;i++)if(MessText[i])delete MessText[i];

	SaveRegistry();
	
	InternalAccessDestroy();
	VndDestroy();

	return ret;
}

NTSTATUS BaseDeregister()
{
	ShutDown=1;
	while (BusyCounter.Test()){
		LARGE_INTEGER delay;
		delay.QuadPart = -8000000L;
		KeDelayExecutionThread(KernelMode, FALSE, &delay);
	}
	NTSTATUS r;
	if(CallbackHandle){
		r=_FilterDeregister(__FSAVP_ID__, CallbackHandle);
		_Trace(TraceInfo,"Deregister: %d  %08X\n",NT_SUCCESS(r),CallbackHandle);
		CallbackHandle=NULL;
	}

	ShutDown=0;
	return r;
}

#pragma optimize( "", off ) 

NTSTATUS BaseUnload()
{
	ShutDown=1;
	while (BusyCounter.Test()){
		LARGE_INTEGER delay;
		delay.QuadPart = -8000000L;
		KeDelayExecutionThread(KernelMode, FALSE, &delay);
	}
    _Trace(TraceInfo,"delete Base\n");

#ifdef MY_OWN_STACK 
	if(MyStack){
		_asm mov OldESP, esp;
		_asm mov esp, MyESP;
	}
#endif// MY_OWN_STACK 

#ifdef __SEH__
	__try{
#endif
		BAvpLoadBase(NULL);
#ifdef __SEH__
	}
	__except( 1 ) {
		goto except;
	}
except:
#endif

#ifdef MY_OWN_STACK 
	if(MyStack){
		_asm mov esp,OldESP;
	}

	if(MyStack){
		delete MyStack;
		MyStack=0;
	}
#endif// MY_OWN_STACK 


	_Trace(TraceInfo,"Bases Unload complete.\n");

	ShutDown=0;
	return STATUS_SUCCESS;
}

int r_static;
NTSTATUS BaseLoad()
{
	NTSTATUS Ret=STATUS_INSUFFICIENT_RESOURCES;

#ifdef MY_OWN_STACK 
	if(OwnStackSize>0x2000 && !MyStack){
		MyStack=new DWORD[OwnStackSize];
		if(!MyStack)goto ret;
		MyESP=MyStack+OwnStackSize-0x40;
		_Trace(TraceInfo,"Setting OwnStackSize=%d\n",OwnStackSize);
	}
#endif// MY_OWN_STACK 

	if(!WaitIfBusy())	goto ret;

#if GATEKEEPER_API_VERSION!=0x0100
#ifdef _DEBUG_DBVALID
	INTERNALMESSAGE msg;
	unsigned char buf[1000];
	memset(&msg,0,sizeof(msg));
	msg.InputData=(BYTE*)"c:\\!base";
	msg.OutputData=buf;
	msg.dwOutputLength=1000;
	DBValidation( &msg);
#endif
#endif //GATEKEEPER_API_VERSION!=0x0100

	RtlZeroMemory(BaseInfo,sizeof(AVPBaseInfo));

	_Trace(TraceInfo,"BaseLoading... %s\n",SetName);
	BaseInfo->Status|=BI_LOADING;

#ifdef MY_OWN_STACK 
	if(MyStack){
		_asm mov OldESP, esp;
		_asm mov esp, MyESP;
	}
#endif// MY_OWN_STACK 

#ifdef __SEH__
	__try{
#endif
		r_static = BAvpLoadBase(SetName);
#ifdef __SEH__
	}
	__except( 1 ) {
		r_static = 0;
		goto except;
	}
except:
#endif

#ifdef MY_OWN_STACK 
	if(MyStack){
		_asm mov esp,OldESP;
	}
#endif// MY_OWN_STACK 

	BaseInfo->Status&=~BI_LOADING;
	BaseInfo->Status|=BI_LOADED;
	if(r_static && BaseInfo->NumberOfRecords)
	{
		_Trace(TraceInfo,"BaseLoading done.  viruses count: %d\n",BaseInfo->NumberOfVirNames);

		if(MFlags & MF_PACKED)
		_Trace(TraceInfo,"MF_PACKED\n");
		if(MFlags & MF_ARCHIVED)	
		_Trace(TraceInfo,"MF_ARCHIVED\n");
		if(MFlags & MF_MAILBASES)	
		_Trace(TraceInfo,"MF_MAILBASES\n");
		if(MFlags & MF_MAILPLAIN)	
		_Trace(TraceInfo,"MF_MAILPLAIN\n");
		if(MFlags & MF_CA		)
		_Trace(TraceInfo,"MF_CA\n");
		if(MFlags & MF_REDUNDANT)
		_Trace(TraceInfo,"MF_REDUNDANT\n");
		if(MFlags & MF_WARNINGS	)
		_Trace(TraceInfo,"MF_WARNINGS\n");
		if(MFlags & MF_ALLENTRY	)
		_Trace(TraceInfo,"MF_ALLENTRY\n");
		if(MFlags & MF_ALLWARNINGS	)
		_Trace(TraceInfo,"MF_ALLWARNINGS\n");
		if(MFlags & MF_CALLBACK_SAVE)
		_Trace(TraceInfo,"MF_CALLBACK_SAVE\n");
		Ret= STATUS_SUCCESS;

	}
	else
	{
		_Trace(TraceInfo,"BaseLoading FAILED\n");
		BaseInfo->Status|=BI_ERROR;
#if GATEKEEPER_API_VERSION!=0x0100
//		if(r_static)
			GkSendErrorMessage(SetName, Trap_FSAV_databaseFileCorrupt);
//		else	GkSendErrorMessage(SetName, Trap_FSAV_scannerCannotStart);
#endif
	}
	InternalAccessRemoveAll();
	ReleaseWait();
	Flush();
	
ret:
	return Ret;
}


#pragma optimize( "", on ) 


