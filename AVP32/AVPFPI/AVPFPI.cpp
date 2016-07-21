// AVPFPI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AVPFPI.h"
#include <io.h>
#include <_avpio.h>
#include <Sign/sign.h>
#include <Stuff/_carray.h>
#include <DupMem/DupMem.h>

#include <serialize/kldtser.h>
#include <AVPVndID.h>

#ifdef _DEBUG
//#define _DEBUG_DBVALID
#endif


//turbpa010412-[14323]{
#include "..\DBCleanup\DBCleanup.h"
// global that equal true when database files should be cleaned up
static BOOL DisableDBCleanUpFPI = FALSE;  
//turbpa010412-[14323]}

#include "..\FPI\avp_cs.h"

/*

static HANDLE g_hProcessHeap = NULL;

static DWORD g_AllocCounter=0;

// Alloc memory
// 
void *AvpMalloc( size_t size ) {
	if ( !g_hProcessHeap )
		g_hProcessHeap = ::GetProcessHeap();

  void *pResult = NULL;
  __try {
    pResult = ::HeapAlloc( g_hProcessHeap, 0 , size );//HEAP_ZERO_MEMORY
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
    pResult = NULL;
  }
  if(pResult) g_AllocCounter++;
  return pResult;
}

// Free memory
//
void AvpFree( void *memblock ) {
	if ( !g_hProcessHeap )
		g_hProcessHeap = ::GetProcessHeap();
	
  __try {
		if ( memblock )
			::HeapFree( g_hProcessHeap, 0, memblock );
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
  }
  if(memblock) g_AllocCounter--;
}

// Compact memory
//
void AvpCompact() {
	if ( !g_hProcessHeap )
		g_hProcessHeap = ::GetProcessHeap();
	
  __try {
	  ::HeapCompact( g_hProcessHeap, 0 );
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
  }
}


#define CDECL //__cdecl


// ---
void* CDECL operator new( size_t nSize ) {
	return ::AvpMalloc( nSize );
}


// ---
void* CDECL operator new( size_t nSize, void* p ) {
	return ::AvpMalloc( nSize );
}


// ---
void CDECL operator delete( void* p ) {
	::AvpFree( p );
}


// ---
void CDECL operator delete( void* p, void* pPlace ) {
	::AvpFree( p );
}

*/





BOOL LoadIO();

HINSTANCE hiIODll=NULL;
static void** vndArray=NULL;
static int vndArrayCount=0;
static CPtrArray fpiParent;
static CRITICAL_SECTION	mFpiParentBusy;
static int fpiInit=0;

typedef struct _ScanData{
	DWORD refdata_;
	BOOL CancelProcess;
}ScanData;

ScanData* data=0;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		    if(!LoadIO())return 0;
			DupInit(1);

			data=(ScanData*)DupAlloc(sizeof(ScanData),1);
			if(!data)return 0;
			memset(data, 0, sizeof(ScanData));
			if(!BAvpInit())return 0;
	
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			if(hiIODll){
				FreeLibrary(hiIODll);
				hiIODll=0;
			}
			if(data){
				DupFree(data);
				data=0;
			}
			DupDestroy();
			if(fpiInit){
				DeleteCriticalSection(&mFpiParentBusy);
				fpiInit--;
			}
			
#ifdef _DEBUG
		MessageBox(NULL,"unloading...","AVPFPI",MB_OK );
#endif
			break;
    }
    return TRUE;
}



#ifdef _DEBUG
#define MODULENAME "AVP FPI Engine (Debug)"
#else
#define MODULENAME "AVP FPI Engine"
#endif

#define FitsInStruct(structtype, pstruct, sizefield, field) \
    (offsetof(structtype,field) + sizeof(pstruct->field) <= pstruct->sizefield)

#define MY_MIN_APIVERSION	0x1A		// FPI API 1.10
#define MY_MAX_APIVERSION	0x1B		// FPI API 1.11


LPFPIFILEIOFNTABLE FPI_IO=NULL; 
LPFPICALLBACK FPI_report=NULL;
BOOL     FPI_initialized = FALSE;
DWORD open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;

DWORD MFlags=0x08150002;
int Ready=0; 

char kRegKey_GK95PlugIns [] = "Software\\Data Fellows\\F-Secure\\Gatekeeper\\Plug-Ins\\AVP";
char* basepath_name = "BasePath";


#define SET_NAME_LEN 0x20
char AVPSet_FullPathName[MAX_PATH]="";
char AVPSet_Name[SET_NAME_LEN]="AVP.SET";
char AVPVnd_Name[SET_NAME_LEN]="AVP.VND";
char AVPSet_Path[MAX_PATH]="";


BOOL GetSetName()
{
	DWORD bsize=MAX_PATH;
	HKEY hKey=0;
	DWORD type=REG_SZ;

	strncpyz(AVPSet_FullPathName,AVPSet_Path,MAX_PATH);

	if(*AVPSet_Path==0){
		if(ERROR_SUCCESS==RegCreateKey(HKEY_LOCAL_MACHINE, kRegKey_GK95PlugIns, &hKey))
		{
			RegQueryValueEx(hKey, basepath_name, NULL, &type, (BYTE*)AVPSet_FullPathName, &bsize);
			RegCloseKey(hKey);
		}
	}

	DWORD l=strlen(AVPSet_FullPathName);
	if(l && AVPSet_FullPathName[l-1]!='\\')
		strncat(AVPSet_FullPathName,"\\",MAX_PATH-1);
	strncat(AVPSet_FullPathName,AVPSet_Name,MAX_PATH-1);
	return 1;
}


extern LOAD_PROGRESS_STRUCT LPS;

// doerrormsg:
// Report an error with message text via FPI callback.
void doerrormsg(WORD param, DWORD refdata, const char* msg)
{
    if(FPI_report)
		FPI_report(FPIEVENT_ERROR, param, FPIFLAG_MSGEXTRA, msg, refdata);
}

void doerror(WORD param, DWORD refdata)
{
	if(FPI_report)
		FPI_report(FPIEVENT_ERROR, param, 0, NULL, refdata);
}
int ReadString(char* buf,DWORD size, HANDLE f);
/*
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
*/
#include <bases/format/base.h>
BOOL GetBaseInfo()
{
	bool ok=0;
	if( *AVPSet_FullPathName==0 ) GetSetName();

	char* n;
	char name[0x200];
	GetFullPathName(AVPSet_FullPathName,0x200,name,&n);

	DWORD open_reserved_=open_reserved_flag;
	open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;

	HANDLE f=Cache_AvpCreateFile(name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	while((f!=INVALID_HANDLE_VALUE) && ReadString(n,0x200-(n-name),f)){
		if(*n==';')continue;
		if(*n==' ')continue;
		if(*n== 0 )continue;
		HANDLE h=Cache_AvpCreateFile(name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if(h!=INVALID_HANDLE_VALUE)
		{
			AVP_BaseHeader          BaseHeader;
			LPS.curName=name;
			DWORD bread;
			
			Cache_AvpReadFile(h,&BaseHeader,sizeof(BaseHeader),&bread,0);
			
			if(*(DWORD*)BaseHeader.Authenticity==AVP_MAGIC){
				Cache_AvpSetFilePointer(h,0,0,FILE_BEGIN);
				Cache_AvpReadFile(h,&BaseHeader.Authenticity,sizeof(BaseHeader)-0x40,&bread,0);
			}
			
			if(BaseHeader.Magic!=AVP_MAGIC){
				Cache_AvpSetFilePointer(h,0x90,0,FILE_BEGIN);
				Cache_AvpReadFile(h,&BaseHeader.Magic,sizeof(BaseHeader)-0x80,&bread,0);
			}
			if(BaseHeader.Magic==AVP_MAGIC && BaseHeader.BlockHeaderTableSize)
			{
				ok=1;
				LPS.NumberOfBases++;
					LPS.CurBaseYear = BaseHeader.ModificationTime.wYear;
					LPS.CurBaseMonth = (BYTE)BaseHeader.ModificationTime.wMonth;
					LPS.CurBaseDay = (BYTE)BaseHeader.ModificationTime.wDay;
			
				if((BaseHeader.ModificationTime.wYear > LPS.LastUpdateYear)
					||((BaseHeader.ModificationTime.wYear == LPS.LastUpdateYear)
						&&((BaseHeader.ModificationTime.wMonth > LPS.LastUpdateMonth)
							||((BaseHeader.ModificationTime.wMonth == LPS.LastUpdateMonth)
								&&(BaseHeader.ModificationTime.wDay > LPS.LastUpdateDay)
							)
						)
					)
				)
				{
					LPS.LastUpdateYear = BaseHeader.ModificationTime.wYear;
					LPS.LastUpdateMonth = (BYTE)BaseHeader.ModificationTime.wMonth;
					LPS.LastUpdateDay = (BYTE)BaseHeader.ModificationTime.wDay;
				}
			}

			Cache_AvpCloseHandle(h);
		}
	}
	if(f!=INVALID_HANDLE_VALUE)
		Cache_AvpCloseHandle(f);

	open_reserved_flag=open_reserved_;

	
	return ok;
}




BOOL FPIFN FPI_GetModuleInformation (
	DWORD apiversion_, 
	LPFPIMODULEINFO moduleinfo_, 
	LPCFPIFILEIOFNTABLE fileiotable, 
	LPFPICALLBACK reportcallback_
	)
{
	FPI_IO = fileiotable;
    FPI_report = reportcallback_;

    if( moduleinfo_ && (moduleinfo_->fStructSize > sizeof(FPIMODULEINFO)))
    {
        doerror(FPIERROR_INVALIDPARAMETER, FPIREFDATA_GETMODULEINFORMATION);
        return FALSE;   // they expected us to fill more fields than we know about
    }
/*
    if( apiversion_ && 
		(  HIWORD(apiversion_) < MY_MIN_APIVERSION
		|| HIWORD(apiversion_) > MY_MAX_APIVERSION))
    {
        doerror(FPIERROR_WRONGAPI, FPIREFDATA_GETMODULEINFORMATION);
        return FALSE;   // unknown FPI API version
    }
*/
	GetBaseInfo();

	if( moduleinfo_ )
	{
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fModuleID))
			moduleinfo_->fModuleID      = FPIMODULEID_AVP;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fModuleName))
			strncpy(moduleinfo_->fModuleName, MODULENAME, FPIMAX_MODULENAME);
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVendorName))
			strncpy(moduleinfo_->fVendorName, VER_COMPANYNAME_STR, FPIMAX_VENDORNAME);

		// Version number taken from PROGTYPE.H
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVerMajor))
			moduleinfo_->fVerMajor      = VER_PRODUCTVERSION_HIGH;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVerMinor))
			moduleinfo_->fVerMinor      = VER_PRODUCTVERSION_LOW;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVerBuild))
			moduleinfo_->fVerBuild      = VER_PRODUCTVERSION_BUILD;	// jmk990111

		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseYear))
			moduleinfo_->fDatabaseYear  = LPS.LastUpdateYear;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseMonth))
			moduleinfo_->fDatabaseMonth = LPS.LastUpdateMonth;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseDay))
			moduleinfo_->fDatabaseDay   = LPS.LastUpdateDay;
	}
    return TRUE;
}

static void VndDestroy()
{
	if(vndArray){
		for(int i=0;i<vndArrayCount;i++)
			delete [] vndArray[i];
		delete [] vndArray;
	}
	vndArrayCount=0;
	vndArray=NULL;
}


static BOOL VndSetInit(const char* setfilename)
{
		BOOL ret=FALSE;
		char filename[MAX_PATH*2];
		char* n=filename;
		
		if(setfilename==NULL) return FALSE;

		strncpyz(filename,setfilename,MAX_PATH);
		n=_AvpStrrchr(filename,'\\');
		if(n) n++;
		else n=filename;
		strncpyz(n,AVPVnd_Name,MAX_PATH);
		
		if(sign_check_file(filename,1,0,0,0)) return FALSE;
		if(!KLDT_Init_Library(malloc,free)) return FALSE;
		if(!DATA_Init_Library(malloc,free)) return FALSE;
		
		HDATA hRootData=NULL;
		if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return FALSE;
		if(hRootData==NULL) return FALSE;
		if(AVP_PID_VNDROOT!=DATA_Get_Id( hRootData, 0 )){
			DATA_Remove( hRootData, 0 );
			return FALSE;
		}
		
		MAKE_ADDR1( hKernelAddr, AVP_PID_VNDKERNELDATA );
		
		HPROP hKernelProp;
		hKernelProp = DATA_Find_Prop( DATA_Find(hRootData, hKernelAddr), NULL, 0 );
		
		// Получить	количество элементов в массиве
		AVP_dword nCount = PROP_Arr_Count( hKernelProp );
		if(nCount==0) goto dr;

		VndDestroy();
		vndArray= (void**)new DWORD[nCount];
		if(vndArray==NULL)goto dr;

		ret=TRUE;

		// Сканировать массив
		AVP_dword i;
		for ( i=0; i<nCount; i++ ) {
			AVP_dword nItemSize = PROP_Arr_Get_Items( hKernelProp, i, 0, 0 );
			AVP_char *pBuffer = new AVP_char[nItemSize];
			if(pBuffer==NULL)break;
			PROP_Arr_Get_Items( hKernelProp, i, pBuffer, nItemSize );
			vndArray[i]=pBuffer;
		}
		vndArrayCount=i;
dr:
		// Удаление дерева
		DATA_Remove( hRootData, 0 );
		return ret;
}

BOOL FPIFN FPI_Uninitialize()
{
    if (!FPI_initialized)
    {
        doerror(FPIERROR_NOTINIT, FPIREFDATA_UNINITIALIZE);
		return FALSE;
    }
	if(DupEnter(NULL)){
		__try{
			Ready=0;
			data->refdata_=0;
			BAvpLoadBase(NULL);
			VndDestroy();
			if(!BAvpDestroy())
			{
				//        doerrormsg( FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION,"Unloading failed");
			}
		}
		__finally{
			DupExit(NULL);
		}
	}
	
	FPI_IO=NULL;
    FPI_initialized = FALSE;
	return TRUE;
}


BOOL FPIFN FPI_Initialize()
{
	FPI_initialized = FALSE;
	if( *AVPSet_FullPathName==0 ) GetSetName();
	if(DupEnter(NULL)){
		open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;

		__try{
			data->refdata_=0;
			VndSetInit(AVPSet_FullPathName);
			if(BAvpLoadBase(AVPSet_FullPathName)){
				Ready=1;
				FPI_initialized = TRUE;
			}
			else{
				doerrormsg( FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION,AVPSet_FullPathName);
				FPI_IO=NULL;
			}
		}
		__finally{
			open_reserved_flag=0;
			DupExit(NULL);
		}
	}

#ifdef _DEBUG_DBVALID
//	FPI_ValidateDatabases("C:\\!", 0);
#endif
		


	return FPI_initialized;
}


BOOL ObjDoError(AVPScanObject &_so,DWORD refdata_)
{
	if(data->CancelProcess){
		return FALSE;
	}
	
	if(_so.RFlags & RF_NOT_A_PROGRAMM)
	{
		if(!(_so.SType & OT_SECTOR))
			return TRUE;
	}
	if(_so.RFlags & RF_NOT_OPEN){
		if(_so.SType & OT_SECTOR)
			doerror( FPIERROR_SECTORREAD, refdata_);
		else
	        doerror( FPIERROR_FILEOPEN, refdata_);
		return FALSE;
	}
	if(_so.RFlags & RF_ERROR){
		if(_so.RFlags & RF_CURE_FAIL){
			if(_so.SType & OT_SECTOR)
				doerror( FPIERROR_SECTORWRITE, refdata_);
			else
				doerror( FPIERROR_FILEWRITE, refdata_);
		}
		else{
			if(_so.SType & OT_SECTOR)
				doerror( FPIERROR_SECTORREAD, refdata_);
			else
				doerror( FPIERROR_FILEREAD, refdata_);
		}
		return FALSE;
	}
	return TRUE;
}



static fpiParentAdd(LPCSTR filename_,FPICONTAINERHANDLE parent_)
{
	int ret=-1;
	DWORD* data=new DWORD[2];
	if(data){
		data[0]=(DWORD)filename_;
		data[1]=parent_;
		if(!fpiInit){
			InitializeCriticalSection(&mFpiParentBusy);
			fpiInit++;
		}
		EnterCriticalSection(&mFpiParentBusy);
		ret=fpiParent.Add(data);
		LeaveCriticalSection(&mFpiParentBusy);
	}
	return ret;
}
static fpiParentRemove(LPCSTR filename_)
{
	EnterCriticalSection(&mFpiParentBusy);
	int i=fpiParent.GetSize();
	while(i-->0){
		DWORD* data=(DWORD*)fpiParent.GetAt(i);
		if(data[0]==(DWORD)filename_){
			fpiParent.RemoveAt(i);
			delete data;
			break;
		}
	}
	LeaveCriticalSection(&mFpiParentBusy);
	return i;
}

FPICONTAINERHANDLE fpiParentGet(LPCSTR filename_)
{
	FPICONTAINERHANDLE ret=0;
	if(!fpiInit){
		InitializeCriticalSection(&mFpiParentBusy);
		fpiInit++;
	}
	EnterCriticalSection(&mFpiParentBusy);
	int i=fpiParent.GetSize();
	while(i-->0){
		DWORD* data=(DWORD*)fpiParent.GetAt(i);
		if(data[0]==(DWORD)filename_){
			ret=data[1];
			break;
		}
	}
	LeaveCriticalSection(&mFpiParentBusy);
	return ret;
}


#include <stuff/dprintf.h>


BOOL FPIFN FPI_ScanFile (LPCSTR filename_, FPICONTAINERHANDLE parent_, WORD action_, DWORD refdata_)
{
	BOOL ret=0;
	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=MFlags;
	_so.MFlags|=MF_APC_CURE_REQUEST|MF_FPI_MODE;
	_so.SType=OT_FILE; 
//	if(0/*processArchives_*/) _so.MFlags|=MF_ARCHIVED;
//	else _so.MFlags&=~MF_ARCHIVED;
		
	switch(action_)
	{
	case FPIACTION_DISINFECT:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_CUREALL;
		break;
	case FPIACTION_NONE:
	default:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DENYACCESS;
		break;

	}
	strncpyz(_so.Name,filename_,AVP_MAX_PATH);
	_so.NativeName=(DWORD)filename_;

//	_so.MFlags&=~MF_CA; //DEBUGGING #22261
	
	BOOL ExclusiveOK=0;
	if(!DupEnter( &ExclusiveOK)){
		doerror( FPIERROR_UNKNOWN, refdata_);
	}
	else
	{
		data->refdata_=refdata_;
		data->CancelProcess=0;
		fpiParentAdd(filename_,parent_);
		__try{
			BAvpProcessObject( &_so );
		}
		__finally{
			fpiParentRemove(filename_);
			ret=ObjDoError(_so,refdata_);
			DupExit(&ExclusiveOK);
		}
	}
	
	return ret;
}

BOOL FPIFN FPI_ScanMemory (WORD action_, DWORD refdata_)
{
 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=MFlags;
	_so.MFlags|=MF_APC_CURE_REQUEST|MF_FPI_MODE;
	_so.SType=OT_MEMORY; 
		
	strncpyz(_so.Name,"Memory",AVP_MAX_PATH);

	BOOL ExclusiveOK=0;
	if(!DupEnter( &ExclusiveOK)){
		doerror( FPIERROR_UNKNOWN, refdata_);
	}
	else
	{
		data->refdata_=refdata_;
		data->CancelProcess=0;
		__try{
			BAvpProcessObject( &_so );
		}
		__finally{
			DupExit(&ExclusiveOK);
		}
	}
	return ObjDoError(_so,refdata_);
}

BOOL FPIFN FPI_ScanBootBlock (WORD drive_, WORD blocktype_, WORD blocksize_, WORD action_, DWORD refdata_)
{
 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
    _so.MFlags=MFlags;
	_so.MFlags|=MF_APC_CURE_REQUEST|MF_FPI_MODE;

	
	switch(action_)
	{
	case FPIACTION_DISINFECT:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_CUREALL;
		break;
	case FPIACTION_NONE:
	default:
		_so.MFlags&=~MF_D_;
		_so.MFlags|=MF_D_DENYACCESS;
		break;

	}
	switch(blocktype_){
	case FPIBOOTBLOCK_MBR: 
		_so.SType=OT_MBR; 
		_so.MFlags|=MF_ALLENTRY;
		sprintf(_so.Name,"MBR of disk %2X",drive_);
		_so.Drive=(BYTE)drive_;
		_so.Disk=((_so.Drive)>=2)?0x80:(_so.Drive);
		break;
	case FPIBOOTBLOCK_BOOT:
	default:
		_so.SType=OT_BOOT; 
		sprintf(_so.Name,"Boot sector of disk %c:",'A'+drive_);
		_so.Drive=(BYTE)drive_;
		_so.Disk= ((_so.Drive)>=2)?0x80:(_so.Drive);
		break;
	}

	BOOL ExclusiveOK=0;
	if(!DupEnter( &ExclusiveOK)){
		doerror( FPIERROR_UNKNOWN, refdata_);
	}
	else
	{
		data->refdata_=refdata_;
		data->CancelProcess=0;
		__try{
			BAvpProcessObject( &_so );
		}
		__finally{
			DupExit(&ExclusiveOK);
		}
	}
	return ObjDoError(_so,refdata_);
}

int FPIFN FPI_ValidateDatabases(LPCSTR dir_, DWORD refdata_)
{

#ifdef _DEBUG_DBVALID
	_asm int 3;
	//	FPI_CustomSettings("MFlags=0x08950002");
#endif

	BOOL valid=1;

	char set_current[MAX_PATH*2];
	char set_new[MAX_PATH*2];

	
	if( *AVPSet_FullPathName==0 ) GetSetName();

	strncpyz(set_current,AVPSet_FullPathName,MAX_PATH);
	if(dir_){
		strncpyz(set_new,dir_,MAX_PATH);
		DWORD l=strlen(set_new);
		if(l && set_new[l-1]!='\\')
			strncat(set_new,"\\",MAX_PATH-1);
		strncat(set_new,AVPSet_Name,MAX_PATH-1);
	}
	else{
		strncpyz(set_new,set_current,MAX_PATH);
	}
	


	BOOL ExclusiveOK=0;
	if(DupEnter( &ExclusiveOK))
		__try{
			data->refdata_=refdata_;
			data->CancelProcess=0;

			char* n;
			char* n2;
			AvpGetFullPathName(set_new,MAX_PATH,set_new,&n);
			AvpGetFullPathName(set_current,MAX_PATH,set_current,&n2);
			
			if(!VndSetInit(set_new))
				VndSetInit(set_current);
			
			DWORD status=FSAV_DB_MISSING;
			DWORD filecount=0;
			
			DWORD open_reserved_=open_reserved_flag;
			open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;
			
			HANDLE f=Cache_AvpCreateFile(set_new,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			do{
				if(*n==';')continue;
				if(*n==' ')continue;
				if(*n== 0 )continue;
				strncpyz(n2,n,MAX_PATH);
				char* set=set_new;
		again:
				switch(sign_check_file(set,1,vndArray,vndArrayCount,0))
				{
				case SIGN_OK:
					status=FSAV_DB_VALID;
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
					if(set==set_new)
						status=FSAV_DB_CORRUPT;
					break;
				case SIGN_BUFFER_HAS_NO_NOTARIES:
				case SIGN_NOTARY_NOT_FOUND:
				case SIGN_BAD_OPEN_KEY:
					if(set==set_new)
						status=FSAV_DB_NOTDB;
					break;
				case SIGN_BAD_KEY_VERSION:
					if(set==set_new)
						status=FSAV_DB_VERSION;
					break;

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
					valid=0;
				default:
					if(set==set_new)
						status=FSAV_DB_NOTCHECKED;
					break;
				}
				filecount++;

				if(FPI_report && !(status==FSAV_DB_VALID && set==set_current))
				{
					DWORD bread=0;
					HANDLE h=Cache_AvpCreateFile(set,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
					if(h!=INVALID_HANDLE_VALUE)
					{
						AVP_BaseHeader          BaseHeader;
						
						Cache_AvpSetFilePointer(h,0,0,FILE_BEGIN);
						Cache_AvpReadFile(h,&BaseHeader,sizeof(BaseHeader),&bread,0);
						
						if(*(DWORD*)BaseHeader.Authenticity==AVP_MAGIC){
							Cache_AvpSetFilePointer(h,0,0,FILE_BEGIN);
							Cache_AvpReadFile(h,&BaseHeader.Authenticity,sizeof(BaseHeader)-0x40,&bread,0);
						}
						
						if(BaseHeader.Magic!=AVP_MAGIC){
							Cache_AvpSetFilePointer(h,0x90,0,FILE_BEGIN);
							Cache_AvpReadFile(h,&BaseHeader.Magic,sizeof(BaseHeader)-0x80,&bread,0);
						}
						bread=0;
						if(BaseHeader.Magic==AVP_MAGIC && BaseHeader.BlockHeaderTableSize)
						{
							bread=((DWORD)(BaseHeader.ModificationTime.wYear) << 16)
								| ((DWORD)(BaseHeader.ModificationTime.wMonth) << 8)
								| ((DWORD)BaseHeader.ModificationTime.wDay);
						}
						Cache_AvpCloseHandle(h);
					}

					FPI_report(FPIEVENT_DBVALIDATION, (WORD)status, bread, n, refdata_);
				}


				if(status==FSAV_DB_VALID || status==FSAV_DB_NOTCHECKED)continue;
				valid=0;
			}while((f!=INVALID_HANDLE_VALUE) && ReadString(n,MAX_PATH-(n-set_new),f));

			if(f!=INVALID_HANDLE_VALUE)Cache_AvpCloseHandle(f);
			open_reserved_flag=open_reserved_;

			VndSetInit(set_current);
		}
		__finally{
			DupExit(&ExclusiveOK);
		}

	
	return valid;
}

BOOL FPIFN FPI_ReportScanningStatus(void)
{
	return (!!Ready && LPS.recCount);
}
void FPIFN FPI_ReloadDatabases(void)
{
	DWORD open_reserved_=open_reserved_flag;
	open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;

	//turbpa010412-[14323]{
	if( *AVPSet_FullPathName == 0 ) 
		GetSetName();
	
	if(*AVPSet_FullPathName){
		if(DupEnter(NULL)){
			__try{
				data->refdata_=0;
				Ready=0;
				VndSetInit(AVPSet_FullPathName);
				BAvpLoadBase(AVPSet_FullPathName);
				Ready=1;
			}
			__finally{
				DupExit(NULL);
			}
		}
		
		if( DisableDBCleanUpFPI == FALSE )
		{
			AVP_CLEANUP_DB_RETURN_VALUE Res = 
				AVPCleanUpUnUsedFiles( AVPSet_FullPathName );
		}
	}

	open_reserved_flag=open_reserved_;
}
void FPIFN FPI_AutoReloadOff(void)
{
}
void FPIFN FPI_AutoReloadOn(void)
{
}

DllExport DWORD FPIFN FPI_QuerySignatureCount(void)
{
	return LPS.recCount;
}


char* _TempPath=NULL;

BOOL AVP_SetTempPath(char* name)
{
	char* ptr=new char[strlen(name)+1];
	if(!ptr)return 0;
	strcpy(ptr,name);

	if (_access(ptr,0)){
		delete ptr;
		return FALSE;
	}

	if(_TempPath)
		delete _TempPath;
	_TempPath=ptr;
	return TRUE;
}

DWORD WINAPI _AvpGetTempPath( DWORD nBufferLength, LPTSTR lpBuffer )
{
	DWORD ret;
	if(_TempPath)
		ret=strlen(strncpy(lpBuffer, _TempPath, nBufferLength));
	else{
		ret=GetTempPath(nBufferLength, lpBuffer);
		AVP_SetTempPath(lpBuffer);
	}
	return ret;
}

void FPIFN FPI_CustomSettings(LPCSTR settings_)
{
	char* buf;
	if(!settings_ || !(buf=new char[strlen(settings_)+1])) return;
	strcpy(buf,settings_);
	char* setting=buf;
	
	DisableDBCleanUpFPI = FALSE; //flush the seting
	
	while(*setting){
		if(*setting==' '){
			setting++;
			continue;
		}
		char* c=strchr(setting,';');
		if(c) *c=0;
		if(!strncmp(setting,FPI_CS_AVP_MFlags,strlen(FPI_CS_AVP_MFlags))){
			MFlags=strtoul(setting+strlen(FPI_CS_AVP_MFlags),0,0);
		}
		else if(!strncmp(setting,FPI_CS_AVP_TEMP,strlen(FPI_CS_AVP_TEMP))){
			AVP_SetTempPath(setting+strlen(FPI_CS_AVP_TEMP));
		}
		else if(!strncmp(setting,FPI_CS_AVP_BASE_SET,strlen(FPI_CS_AVP_BASE_SET))){
			strncpy(AVPSet_Name,setting+strlen(FPI_CS_AVP_BASE_SET),SET_NAME_LEN);
		}
		else if(!strncmp(setting,FPI_CS_AVP_BASE_VND,strlen(FPI_CS_AVP_BASE_VND))){
			strncpy(AVPVnd_Name,setting+strlen(FPI_CS_AVP_BASE_VND),SET_NAME_LEN);
		}
		else if(!strncmp(setting,FPI_CS_AVP_BASE_PATH,strlen(FPI_CS_AVP_BASE_PATH))){
			strncpy(AVPSet_Path,setting+strlen(FPI_CS_AVP_BASE_PATH),MAX_PATH);
		}
		else if(!strncmp(setting,FPI_CS_AVP_KeepUnusedDB,strlen(FPI_CS_AVP_KeepUnusedDB))){
			DisableDBCleanUpFPI = TRUE;			
		}
		
		if(c) setting=c+1;
		else break;
	}
	delete buf;
}



_TCHAR* WINAPI _AvpConvertChar(_TCHAR* str_,INT ct,_TCHAR* cp_)
{
#ifndef SYSTOS2
	switch(ct)
	{
	case 1:
		break;
	case 5:
		if (cp_ && (*(WORD*)cp_==0xFFFE || *(WORD*)cp_==0xFEFF))
		{
			char buff[AVP_MAX_PATH];
			memset(buff,AVP_MAX_PATH,0);
			if(*(WORD*)cp_==0xFEFF){
				DWORD i=0;
				char *p=cp_+2;
				char *q=cp_+3;
				while( *p || *q || i++< AVP_MAX_PATH){
					char c=*q;
					*(q++)=*p;
					*(p++)=c;
				}
			}
			
			if(WideCharToMultiByte(CP_ACP,0,(LPCWSTR)(cp_+2),-1,buff,AVP_MAX_PATH,NULL,NULL))
			{
				strncpy(str_,buff,AVP_MAX_PATH);
				break;
			}
		}
	default:
	case 0:
		OemToChar(str_,str_);
		break;
		
	}
#endif
	return str_;
}

_TCHAR* WINAPI _AvpStrlwr(_TCHAR* str_)
{
#ifdef _MBCS
        return strlwr(str_);
#else
        if(str_)
        {
                _TCHAR* c=str_;
                while(*c){
                        *c|=0x20;
                        c++;
                }
        }
        return str_;
#endif
}

_TCHAR* WINAPI _AvpStrrchr(_TCHAR* str_, INT chr_){

#ifdef _MBCS
        return strrchr(str_,chr_);
#else

        if(str_)
        {
                _TCHAR* c=str_ + strlen(str_);
                while(c>=str_)
                {
                        if(*c==chr_){
                                        return c;
                        }
                        c--;
                }
        }
        return NULL;
#endif
}


HINSTANCE hDllInst;

HINSTANCE LoadLibraryInst( LPCTSTR name, HINSTANCE hDllInst )
{
	HINSTANCE hi;
	char DllName[MAX_PATH];
	char* lpFilePart;
	GetModuleFileName(hDllInst,DllName,MAX_PATH);
	GetFullPathName(DllName,MAX_PATH,DllName,&lpFilePart);
	strncpyz(lpFilePart,name,MAX_PATH-(lpFilePart-DllName));
	hi=LoadLibrary(DllName);
	if(hi) return hi;
	else return LoadLibrary(name);
}

DWORD WINAPI ExecSpecial(
						 char* FuncName, 
						 DWORD wParam, 
						 DWORD lParam
						 )
{
	char buf[0x200];
	char DllName[MAX_PATH];
	char* lpFilePart;
	if(!FuncName || !*FuncName) return 0;
	
	{
		GetModuleFileName(hDllInst,DllName,MAX_PATH);
		GetFullPathName(DllName,MAX_PATH,DllName,&lpFilePart);
		strncpyz(lpFilePart,"AVC*.DLL",MAX_PATH);
		
		HINSTANCE hi;
		WIN32_FIND_DATA fd;
		
		HANDLE fi=FindFirstFile(  DllName, &fd);
		if(fi!=INVALID_HANDLE_VALUE)
		{
			do{
				strncpyz(lpFilePart,fd.cFileName,MAX_PATH);
				hi=LoadLibrary(DllName);
				if(!hi){
					strncpyz(lpFilePart,fd.cAlternateFileName,MAX_PATH);
					hi=LoadLibrary(DllName);
				}
				if(hi){
					void* ptr=GetProcAddress(hi,FuncName);
					if(ptr){
						DWORD ret=0;
						if( sign_check_file( DllName, 1, vndArray, vndArrayCount, 0) )
							_AvpCallback(AVP_CALLBACK_FILE_INTEGRITY_FAILED,(long)DllName);
						else
							ret=((DWORD (*)(DWORD,DWORD))ptr)(wParam, lParam);
						FreeLibrary(hi);
						return ret;
					}
				}
				FreeLibrary(hi);
			}while(FindNextFile(fi,&fd));
		}	
		sprintf(buf,"ExecSpecial can't resolve name: %s",FuncName);
		_AvpCallback(AVP_CALLBACK_PUT_STRING,(long)buf);
	}
	return 0;
}

BOOL _QueryAbort(DWORD refdata_)
{
	if(!refdata_)
		refdata_=data->refdata_;
	if(refdata_ && FPI_report && !data->CancelProcess){
		data->CancelProcess=FPI_report(FPIEVENT_QUERYABORT, 0 , 0, NULL, refdata_);
		if(data->CancelProcess){
			doerror( FPIERROR_ABORTED, refdata_);
			BAvpCancelProcessObject(1);
		}
	}
	return data->CancelProcess;
}

LRESULT WINAPI _AvpCallback(WPARAM wParam,LPARAM lParam)
{
	static char s[512];
	AVPScanObject* so=(AVPScanObject*)lParam;
	LRESULT ret=0;
	BOOL stack_align=0;
	_asm{
		test esp,3;
		jz no_align;
		push bx;
	}
	stack_align=1;
no_align:

    switch(wParam){

	case AVP_CALLBACK_ROTATE: 
	case AVP_CALLBACK_ROTATE_OFF:
		ret=_QueryAbort(0);
		break;

	case AVP_CALLBACK_LOAD_PROGRESS:
		break;

	case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
		{
			DWORD open_reserved_=open_reserved_flag;
			open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;

			ret= sign_check_file( (char*)lParam, 1, vndArray, vndArrayCount, 0);

			open_reserved_flag=open_reserved_;
			goto ret;
		}

	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
        doerrormsg(  FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
		break;

	case AVP_CALLBACK_SETFILE_INTEGRITY_FAILED:
        doerrormsg(  FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
		break;

	case AVP_CALLBACK_ERROR_BASE:
        doerrormsg(  FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
		break;

	case AVP_CALLBACK_ERROR_FILE_OPEN:
        doerrormsg(   FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
		break;

	case AVP_CALLBACK_ERROR_SET:
        doerrormsg(   FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
		break;

	case AVP_CALLBACK_ERROR_KERNEL_PROC:
	case AVP_CALLBACK_ERROR_LINK:
//        doerrormsg(    FPIERROR_BADDBFILE, FPIREFDATA_GETMODULEINFORMATION, (char*)lParam);
		break;

	case AVP_CALLBACK_ASK_DELETE:
		if((so->SType==OT_BOOT) && (so->Disk < 0x80)){
			ret= 2;
			goto ret;
		}
		break;

	case AVP_CALLBACK_ASK_CURE:
		if(FPI_report && so)
		{
			char VirusName[0x200];
			BAvpMakeFullName(VirusName,so->WarningName);
			if (FPI_report(FPIEVENT_QUERY, FPIACTION_DISINFECT , FPIFLAG_MSGEXTRA, VirusName, data->refdata_))
			{
				ret= 1;
				goto ret;
			}
		}
		break;

	case AVP_CALLBACK_OBJECT_WARNING:
	case AVP_CALLBACK_OBJECT_SUSP:
	case AVP_CALLBACK_OBJECT_DETECT:
	case AVP_CALLBACK_OBJECT_CURE:
	case AVP_CALLBACK_OBJECT_CURE_FAIL:
	case AVP_CALLBACK_OBJECT_DELETE:
		break;

	case AVP_CALLBACK_OBJECT_DONE:

		if(so->RFlags & (RF_DETECT|RF_WARNING|RF_SUSPIC))
		{
			char VirusName[0x200];

			if(so->RFlags & RF_DETECT)
				BAvpMakeFullName(VirusName,so->VirusName);
			else if(so->RFlags & RF_WARNING)
				BAvpMakeFullName(VirusName,so->WarningName);
			else 
				BAvpMakeFullName(VirusName,so->SuspicionName);


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

			if((so->RFlags & RF_IN_CURABLE_EMBEDDED) && (so->RFlags & RF_CURE_FAIL))
				break;

			if(FPI_report)
			{
				char s[0x200];
				sprintf(s,"\001%s\001",VirusName);
				ret= FPI_report(FPIEVENT_INFECTION, (WORD)action , flag, s, data->refdata_);
				goto ret;
			}

		}
		break;

	case AVP_CALLBACK_OBJECT_PACKED:
	case AVP_CALLBACK_OBJECT_ARCHIVE:
	case AVP_CALLBACK_OBJECT_UNKNOWN:
	case AVP_CALLBACK_OBJECT_MESS:
    default:
		break;
    }

ret:
	if(stack_align)
		_asm pop bx;
	return ret;
}

t_AvpRead13*	__AvpRead13 = NULL;
t_AvpWrite13*	__AvpWrite13 = NULL;
t_AvpRead25*	__AvpRead25 = NULL;
t_AvpWrite26*	__AvpWrite26 = NULL;
t_AvpGetDiskParam*	__AvpGetDiskParam = NULL;
t_AvpGetFirstMcbSeg* __AvpGetFirstMcbSeg = NULL;
t_AvpGetDosMemSize* __AvpGetDosMemSize = NULL;
t_AvpGetIfsApiHookTable* __AvpGetIfsApiHookTable = NULL;
t_AvpGetDosTraceTable* __AvpGetDosTraceTable = NULL;
t_AvpMemoryRead* __AvpMemoryRead = NULL;
t_AvpMemoryWrite* __AvpMemoryWrite = NULL;

BOOL WINAPI _AvpGetDiskParam (
    BYTE   disk,
    BYTE   drive,
    WORD*   CX,
    BYTE*	DH
	)
{
	BOOL ret=FALSE;
	if(CX)*CX=0;
	if(DH)*DH=0;


#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO && (disk<0x20))
	{
		char Buffer[1024];
		ret=FPI_IO->fFnRWSectorPhys(disk, 0, 0, 1, 0, Buffer, 512);
		if(!ret)	
			ret=FPI_IO->fFnRWSectorLogc(drive, 0, 0, Buffer, 512);

		FPI_DISK_GEOMETRY_struct dg;
		if(FPI_IO->fFnGetDriveGeometry(drive, &dg))
		{
			DWORD NumCyl=0x3FF;  //dg.Cylinders.LowPart
			if(
//				() < 0x400	&& 
				(dg.SectorsPerTrack) < 0x40		&& 
				(dg.TracksPerCylinder) <0x100)
			{
				WORD w=((BYTE)(NumCyl -1))<<8;
				w|=(0xC0)&((NumCyl -1)>>2);
				w|=(0x3F)&(BYTE)(dg.SectorsPerTrack);
				if(CX)*CX=w;
				if(DH)*DH=(BYTE)(dg.TracksPerCylinder -1);
			}
		}
		if(ret && *CX)
			return ret;
	}
#endif

	if(__AvpGetDiskParam) return __AvpGetDiskParam(disk,drive,CX,DH);
	else return 0;
}

BOOL WINAPI _AvpRead25(	BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO)
	{
#ifdef DEBUGPRINT
		dprintf("AVP_IO: RWSectorLog drive:%X sector:%X",Drive,Sector);
#endif
		_QueryAbort(0);
		BOOL r=FPI_IO->fFnRWSectorLogc(Drive, Sector, 0, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif

	if(__AvpRead25) return __AvpRead25(Drive, Sector, NumSectors, Buffer);
	else return 0;
}

BOOL WINAPI _AvpWrite26( BYTE Drive, DWORD Sector, WORD NumSectors, BYTE* Buffer)
{
#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO)
	{
		_QueryAbort(0);
		BOOL r=FPI_IO->fFnRWSectorLogc(Drive, Sector, 1, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif

	if(__AvpWrite26) return __AvpWrite26(Drive, Sector, NumSectors, Buffer);
	else return 0;
}

#define CyllinerFromInt13(Sector)  (((Sector<<2)&0x300)|(Sector>>8))
#define SectorFromInt13(Sector)    ((Sector & 0x003F) )//- 1)

BOOL WINAPI _AvpRead13 (BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
#ifdef FPIAPI_VER_MAJOR

	if(FPI_IO)
	{
		_QueryAbort(0);
        WORD SecNo=SectorFromInt13(Sector);
        WORD CylNo=CyllinerFromInt13(Sector);

		BOOL r=FPI_IO->fFnRWSectorPhys(Disk, Head, CylNo, SecNo, 0, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif

	if(__AvpRead13) return __AvpRead13(Disk, Sector, Head, NumSectors, Buffer);
	else return 0;
}

BOOL WINAPI _AvpWrite13(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors, BYTE* Buffer)
{
#ifdef FPIAPI_VER_MAJOR
	if(FPI_IO)
	{
		_QueryAbort(0);
        WORD SecNo=SectorFromInt13(Sector);
        WORD CylNo=CyllinerFromInt13(Sector);

		BOOL r=FPI_IO->fFnRWSectorPhys(Disk, Head, CylNo, SecNo, 1, Buffer, NumSectors*512);
		if(r) return r;
	}
#endif
 
	if(__AvpWrite13) return __AvpWrite13(Disk, Sector, Head, NumSectors, Buffer);
	else return 0;
}

DWORD WINAPI _AvpMemoryRead (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    LPBYTE  buffer	// pointer to buffer to read to 
	)
{
	if(__AvpMemoryRead) return __AvpMemoryRead(offset,size,buffer);
	doerror(FPIERROR_NOTINIT, FPIREFDATA_GETMODULEINFORMATION);
	return 0;
}
	
DWORD WINAPI _AvpMemoryWrite (
    DWORD   offset,	// offset
    DWORD   size,	// size in bytes
    LPBYTE  buffer	// pointer to buffer to read to 
	)
{
	if(__AvpMemoryWrite) return __AvpMemoryWrite(offset,size,buffer);
	doerror(FPIERROR_NOTINIT, FPIREFDATA_GETMODULEINFORMATION);
	return 0;
}

DWORD WINAPI _AvpGetDosMemSize ()
{
	if(__AvpGetDosMemSize) return __AvpGetDosMemSize();
	doerror(FPIERROR_NOTINIT, FPIREFDATA_GETMODULEINFORMATION);
	return 0;
}

DWORD WINAPI _AvpGetFirstMcbSeg ()
{
	if(__AvpGetFirstMcbSeg) return __AvpGetFirstMcbSeg();
	doerror(FPIERROR_NOTINIT, FPIREFDATA_GETMODULEINFORMATION);
	return 0;
}

DWORD WINAPI _AvpGetIfsApiHookTable (
	DWORD* table, 
	DWORD size //size in DWORDs
	)
{
	if(__AvpGetIfsApiHookTable) return __AvpGetIfsApiHookTable(table,size);
	doerror(FPIERROR_NOTINIT, FPIREFDATA_GETMODULEINFORMATION);
	return 0;
}

DWORD WINAPI _AvpGetDosTraceTable (
	DWORD* table, 
	DWORD size //size in DWORDs
	)
{
	if(__AvpGetDosTraceTable) return __AvpGetDosTraceTable(table,size);
	doerror(FPIERROR_NOTINIT, FPIREFDATA_GETMODULEINFORMATION);
	return 0;
}



BOOL LoadIO()
{

        OSVERSIONINFO os;
        os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
        GetVersionEx(&os);

		hiIODll=LoadLibraryInst((os.dwPlatformId==VER_PLATFORM_WIN32_NT)?"avp_iont.dll":"avp_io32.dll",hDllInst);

    if(hiIODll)
	{
		__AvpRead13               =(t_AvpRead13*)GetProcAddress(hiIODll,"_AvpRead13");
        __AvpWrite13              =(t_AvpWrite13*)GetProcAddress(hiIODll,"_AvpWrite13");
        __AvpRead25               =(t_AvpRead25*)GetProcAddress(hiIODll,"_AvpRead25");
        __AvpWrite26              =(t_AvpWrite26*)GetProcAddress(hiIODll,"_AvpWrite26");
		__AvpGetDiskParam  =(t_AvpGetDiskParam*)GetProcAddress(hiIODll,"_AvpGetDiskParam");

        __AvpGetFirstMcbSeg =(t_AvpGetFirstMcbSeg*)GetProcAddress(hiIODll,"_AvpGetFirstMcbSeg");
        __AvpGetDosMemSize =(t_AvpGetDosMemSize*)GetProcAddress(hiIODll,"_AvpGetDosMemSize");
        __AvpGetIfsApiHookTable =(t_AvpGetIfsApiHookTable*)GetProcAddress(hiIODll,"_AvpGetIfsApiHookTable");
        __AvpGetDosTraceTable =(t_AvpGetDosTraceTable*)GetProcAddress(hiIODll,"_AvpGetDosTraceTable");
        __AvpMemoryRead  =(t_AvpMemoryRead*)GetProcAddress(hiIODll,"_AvpMemoryRead");
        __AvpMemoryWrite  =(t_AvpMemoryWrite*)GetProcAddress(hiIODll,"_AvpMemoryWrite");
	}
    AvpRead13 = _AvpRead13;
	AvpWrite13 = _AvpWrite13;
	AvpRead25 = _AvpRead25;
	AvpWrite26 = _AvpWrite26;
	AvpGetDiskParam = _AvpGetDiskParam;

	AvpGetFirstMcbSeg = _AvpGetFirstMcbSeg;
	AvpGetDosMemSize = _AvpGetDosMemSize;
	AvpGetIfsApiHookTable = _AvpGetIfsApiHookTable;
	AvpGetDosTraceTable = _AvpGetDosTraceTable;
	AvpMemoryRead = _AvpMemoryRead;
	AvpMemoryWrite = _AvpMemoryWrite;

	
	AvpCallback = _AvpCallback;
	AvpStrrchr = _AvpStrrchr;
	AvpStrlwr = _AvpStrlwr;
	AvpConvertChar = _AvpConvertChar;
	AvpGetTempPath = _AvpGetTempPath;

	AvpWriteFile = WriteFile;
	AvpCreateFile = CreateFile;
	AvpCloseHandle = CloseHandle;
	AvpReadFile = ReadFile;
	AvpGetFileSize = GetFileSize;
	AvpSetFilePointer = SetFilePointer;
	AvpSetEndOfFile = SetEndOfFile;
	AvpDeleteFile = DeleteFile;
	AvpGetFileAttributes = GetFileAttributes;
	AvpSetFileAttributes = SetFileAttributes;
	
	AvpGetDiskFreeSpace = GetDiskFreeSpace;
	AvpGetFullPathName  = GetFullPathName;
	AvpGetTempFileName = GetTempFileName;
	AvpExecSpecial = ExecSpecial;
	
    return TRUE;
}
