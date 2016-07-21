#include "stdafx.h"

//turbpa010412-[14323]{
#include "..\DBCleanup\DBCleanup.h"
// global that equal true when database files should be cleaned up
static BOOL DisableDBCleanUpFPI = FALSE;  
//turbpa010412-[14323]}

#include "..\FPI\avp_cs.h"

#ifdef FPIAPI_VER_MAJOR


#ifdef _DEBUG
#define BUILDMODE  " (Debug)"
#else
#define BUILDMODE  ""
#endif

#define MODULENAME "AVP Engine" BUILDMODE

#define FitsInStruct(structtype, pstruct, sizefield, field) \
    (offsetof(structtype,field) + sizeof(pstruct->field) <= pstruct->sizefield)

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif

#define MY_MIN_APIVERSION	(MAKEWORD(10, 1))		// FPI API 1.10
#define MY_MAX_APIVERSION	(MAKEWORD(11, 1))		// FPI API 1.11


LPFPIFILEIOFNTABLE FPI_IO=NULL; 
LPFPICALLBACK FPI_report=NULL;
BOOL     FPI_initialized = FALSE;

#define SET_NAME_LEN 0x20
char AVPSet_FullPathName[MAX_PATH]="";
char AVPSet_Name[SET_NAME_LEN]="AVP.SET";
char AVPVnd_Name[SET_NAME_LEN]="AVP.VND";
char AVPSet_Path[MAX_PATH]="";

void doerror(WORD param, DWORD refdata)
{
	if(FPI_report)
		FPI_report(FPIEVENT_ERROR, param, 0, NULL, refdata);
}

// doerrormsg:
// Report an error with message text via FPI callback.
void doerrormsg(WORD param, DWORD refdata, const char* msg)
{
    if(FPI_report)
		FPI_report(FPIEVENT_ERROR, param, FPIFLAG_MSGEXTRA, msg, refdata);
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

    if( apiversion_ && 
		(  HIWORD(apiversion_) < MY_MIN_APIVERSION
		|| HIWORD(apiversion_) > MY_MAX_APIVERSION))
    {
        doerror(FPIERROR_WRONGAPI, FPIREFDATA_GETMODULEINFORMATION);
        return FALSE;   // unknown FPI API version
    }

	if( moduleinfo_ )
	{
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fModuleID))
			moduleinfo_->fModuleID      = FPIMODULEID_AVP;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fModuleName))
			strcpy(moduleinfo_->fModuleName, MODULENAME);
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVendorName))
			strcpy(moduleinfo_->fVendorName, VER_COMPANYNAME_STR);

		// Version number taken from PROGTYPE.H
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVerMajor))
			moduleinfo_->fVerMajor      = VER_PRODUCTVERSION_HIGH;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVerMinor))
			moduleinfo_->fVerMinor      = VER_PRODUCTVERSION_LOW;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fVerBuild))
			moduleinfo_->fVerBuild      = VER_PRODUCTVERSION_BUILD;	// jmk990111

		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseYear))
			moduleinfo_->fDatabaseYear  = 0;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseMonth))
			moduleinfo_->fDatabaseMonth  = 0;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseDay))
			moduleinfo_->fDatabaseDay  = 0;


		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseYear))
			moduleinfo_->fDatabaseYear  = BaseInfo->LastUpdateYear;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseMonth))
			moduleinfo_->fDatabaseMonth = BaseInfo->LastUpdateMonth;
		if (FitsInStruct(FPIMODULEINFO,moduleinfo_,fStructSize,fDatabaseDay))
			moduleinfo_->fDatabaseDay   = BaseInfo->LastUpdateDay;
	}
    return TRUE;
}

BOOL FPIFN FPI_Uninitialize()
{
    if (!FPI_initialized)
    {
        doerror(FPIERROR_NOTINIT, FPIREFDATA_UNINITIALIZE);
		return FALSE;
    }
	if(!Avp95DeviceInstance->AVP95VXD_Initialize(NULL))
	{
//        doerrormsg( FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION,"Unloading failed");
	}

	FPI_IO=NULL;
    FPI_initialized = FALSE;
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD] FPI_Uninitialize TRUE");
#endif
	return TRUE;
}

char kRegKey_GK95PlugIns [] = "Software\\Data Fellows\\F-Secure\\Gatekeeper\\Plug-Ins\\AVP";
char* basepath_name = "BasePath";

BOOL FPIFN FPI_Initialize()
{
	char buf[0x200];
	DWORD bsize=0x200;
	HKEY hKey=0;
	DWORD dwType = REG_SZ;
				
	*buf=0;
	if(*AVPSet_Path){
		strncpy(buf,AVPSet_Path,MAX_PATH);
	}
	else if(ERROR_SUCCESS==RegCreateKey(HKEY_LOCAL_MACHINE, kRegKey_GK95PlugIns, &hKey)){
		RegQueryValueEx(hKey, basepath_name, NULL, &dwType, (BYTE*)buf, &bsize);
		RegCloseKey(hKey);
	}
	int l=strlen(buf);
	if(l && buf[--l]!='\\')strcat(buf,"\\");

	strcat(buf,AVPSet_Name);
#ifdef DEBUGPRINT
//	dprintf("[AVP95VXD] BasePath=%s",buf );
#endif
	if(!Avp95DeviceInstance->AVP95VXD_Initialize(buf))
	{
//      doerrormsg( FPIERROR_NODBFILE, FPIREFDATA_GETMODULEINFORMATION,buf);
//		FPI_IO=NULL;
//		return FALSE;
	}
	FPI_initialized = TRUE;
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD] FPI_Initialize TRUE");
#endif
	return TRUE;
}

static CPtrArray fpiParent;
static VMutex	mFpiParentBusy;

static fpiParentAdd(LPCSTR filename_,FPICONTAINERHANDLE parent_)
{
	int ret=-1;
	DWORD* data=new DWORD[2];
	if(data){
		data[0]=(DWORD)filename_;
		data[1]=parent_;
		mFpiParentBusy.enter();
		ret=fpiParent.Add(data);
		mFpiParentBusy.leave();
	}
	return ret;
}
static fpiParentRemove(LPCSTR filename_)
{
	mFpiParentBusy.enter();
	int i=fpiParent.GetSize();
	while(i-->0){
		DWORD* data=(DWORD*)fpiParent.GetAt(i);
		if(data[0]==(DWORD)filename_){
			fpiParent.RemoveAt(i);
			delete data;
			break;
		}
	}
	mFpiParentBusy.leave();
	return i;
}

FPICONTAINERHANDLE fpiParentGet(LPCSTR filename_)
{
	FPICONTAINERHANDLE ret=0;
	mFpiParentBusy.enter();
	int i=fpiParent.GetSize();
	while(i-->0){
		DWORD* data=(DWORD*)fpiParent.GetAt(i);
		if(data[0]==(DWORD)filename_){
			ret=data[1];
			break;
		}
	}
	mFpiParentBusy.leave();
	return ret;
}



BOOL ObjDoError(AVPScanObject &_so,DWORD refdata_)
{
#ifdef DEBUGPRINT
	dprintf("[AVP95VXD] ObjDoError %s %08X t:%08X",_so.Name,_so.RFlags ,_so.SType);
#endif
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

BOOL FPIFN FPI_ScanFile (LPCSTR filename_, FPICONTAINERHANDLE parent_, WORD action_, DWORD refdata_)
{

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD] FPIFN FPI_ScanFile  ***************>  %s",filename_ );
#endif	
	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
	_so.Connection=refdata_;
    _so.MFlags=MFlags;
	_so.MFlags|=MF_APC_CURE_REQUEST|MF_FPI_MODE;
	_so.SType=OT_FILE; 

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
	strcpy(_so.Name,filename_);
	_so.NativeName=(DWORD)filename_;
	fpiParentAdd(filename_,parent_);
	Avp95DeviceInstance->AVP95VXD_ProcessObject(&_so);
	fpiParentRemove(filename_);
	return ObjDoError(_so,refdata_);
}

BOOL FPIFN FPI_ScanMemory (WORD action_, DWORD refdata_)
{
 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
	_so.Connection=refdata_;
    _so.MFlags=MFlags;
	_so.MFlags|=MF_APC_CURE_REQUEST|MF_FPI_MODE;
	_so.SType=OT_MEMORY; 
		
	strcpy(_so.Name,"Memory");

	Avp95DeviceInstance->AVP95VXD_ProcessObject(&_so);
	return ObjDoError(_so,refdata_);
}

BOOL FPIFN FPI_ScanBootBlock (WORD drive_, WORD blocktype_, WORD blocksize_, WORD action_, DWORD refdata_)
{
 	AVPScanObject _so;
    memset(&_so,0,sizeof(AVPScanObject));
	_so.Connection=refdata_;
    _so.MFlags=MFlags;
	_so.MFlags|=MF_APC_CURE_REQUEST|MF_FPI_MODE;

#ifdef DEBUGPRINT
	dprintf("[AVP95VXD] ScanBootBlock  %d",drive_ );
#endif	
		
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
		_so.Drive=2;
		_so.Disk=(BYTE)drive_;
		break;
	case FPIBOOTBLOCK_BOOT:
	default:
		_so.SType=OT_BOOT; 
		char* d="A:";
		d[0]='A'+drive_;
		strcpy(_so.Name,"Boot sector of disk ");
		strcat(_so.Name,d);
		_so.Drive=(d[0]|0x20)-'a';
		_so.Disk= ((_so.Drive)>=2)?0x80:(_so.Drive);
		break;
	}


	Avp95DeviceInstance->AVP95VXD_ProcessObject(&_so);
	return ObjDoError(_so,refdata_);
}

//#define INVALID_HANDLE_VALUE ((void*)-1)
#include <_avpio.h>
#include <Sign/sign.h>
extern void** vndArray;
extern int vndArrayCount;

int AVPVXDReadString(char* buf,DWORD size, HANDLE f)
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
#define _DEBUG_DBVALID
#endif

DWORD open_reserved_flag=0;

int FPIFN FPI_ValidateDatabases(LPCSTR dir_, DWORD refdata_)
{
#ifdef _DEBUG_DBVALID
	_asm int 3;
#endif

	BOOL valid=1;
	char set_current[0x200];
	char set_new[0x200];
	strcpy(set_current,BaseInfo->SetName);
	if(dir_==NULL)
		strcpy(set_new,set_current);
	else {
		strcpy(set_new,dir_);
		int l=strlen(set_new);
		if(!l || set_new[l-1]!='\\'){
			strcat(set_new,"\\");
			l++;
		}
//		SetFile.ToPsz(set_new+l, 0x200-l);
		strcpy(set_new+l,AVPSet_Name);
	}

	char* n;
	char* n2;
	_AvpGetFullPathName(set_new,0x200,set_new,&n);
	_AvpGetFullPathName(set_current,0x200,set_current,&n2);

	DWORD status=FSAV_DB_MISSING;
	DWORD filecount=0;
	
	open_reserved_flag=FPIFILEIO_OPENFLAG_DATABASE;
	
	HANDLE f=_AvpCreateFile(set_new,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
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
			if(set==set_new) status=FSAV_DB_CORRUPT;
			break;
		case SIGN_BUFFER_HAS_NO_NOTARIES:
		case SIGN_NOTARY_NOT_FOUND:
		case SIGN_BAD_OPEN_KEY:
			if(set==set_new) status=FSAV_DB_NOTDB;
			break;
		case SIGN_BAD_KEY_VERSION:
			if(set==set_new) status=FSAV_DB_VERSION;
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
			if(set==set_new) status=FSAV_DB_NOTCHECKED;
			break;
		}
		filecount++;

		if(FPI_report)
			if(!(status==FSAV_DB_VALID && set==set_current))
				FPI_report(FPIEVENT_DBVALIDATION, (WORD)status, 0, n, refdata_);


		if(status==FSAV_DB_VALID || status==FSAV_DB_NOTCHECKED)continue;
		valid=0;
	}while((f!=INVALID_HANDLE_VALUE) && AVPVXDReadString(n,0x200-(n-set_new),f));

	if(f!=INVALID_HANDLE_VALUE)AvpCloseHandle(f);
	open_reserved_flag=0;

	return valid;
}

extern int Ready; 
extern Avp95Device* Avp95DeviceInstance;
extern AVPBaseInfo* BaseInfo;

BOOL FPIFN FPI_ReportScanningStatus(void)
{
	return (!!Ready && BaseInfo->NumberOfRecords);
}
void FPIFN FPI_ReloadDatabases(void)
{
	char setPath [0x200];

	//turbpa010412-[14323]{
	if(BaseInfo && Avp95DeviceInstance)
	{
		strcpy(setPath,BaseInfo->SetName);
		Avp95DeviceInstance->AVP95VXD_Initialize(setPath);
		if( DisableDBCleanUpFPI == FALSE )
		{
			DWORD bsize=0x200;
			HKEY hKey=0;
			
			if( *BaseInfo->SetName )
			{
				strcpy( setPath, BaseInfo->SetName);
#ifdef DEBUGPRINT
				dprintf("use version1=%s", setPath);
#endif
			}
			else // sometimes Set file name is blank and get it from Registry
			{
				*setPath=0;
				DWORD dwType = REG_SZ;
				if(*AVPSet_Path){
					strncpy(setPath,AVPSet_Path,MAX_PATH);
				}
				else if(ERROR_SUCCESS==RegCreateKey(HKEY_LOCAL_MACHINE, kRegKey_GK95PlugIns, &hKey))
				{
					RegQueryValueEx(hKey, basepath_name, NULL, &dwType, (BYTE*)setPath, &bsize);
					RegCloseKey(hKey);
				}
				int l=strlen(setPath);
				if(l && setPath[--l]!='\\')
					strcat(setPath,"\\");
				
				strcat(setPath,AVPSet_Name);
#ifdef DEBUGPRINT
				dprintf("use version2=%s", setPath);
#endif
			}


			AVP_CLEANUP_DB_RETURN_VALUE Res = 
			AVPCleanUpUnUsedFiles( setPath );
#ifdef DEBUGPRINT
			dprintf( "delete with status = %i %s\n", (int)Res, setPath );
#endif
				
			
		} 
#ifdef DEBUGPRINT
		else
			dprintf("deletion of old files is disabled ");
#endif

	}
//turbpa010412-[14323]}


}
void FPIFN FPI_AutoReloadOff(void)
{
}
void FPIFN FPI_AutoReloadOn(void)
{
}
extern LOAD_PROGRESS_STRUCT LPS;

DWORD FPIFN FPI_QuerySignatureCount(void)
{
	return LPS.recCount;
}
/*
void FPIFN FPI_CustomSettings(LPCSTR settings_)
{
//turbpa010412-[14323]{
//	if(!strncmp(settings_,"MFlags=",7)){
//		char* ptr=0;
//		MFlags=strtol(settings_+7,&ptr,0);
//	}
	//copied from AVPFPI.DLL
	char* settings;
	if(!settings_ || !(settings=new char[strlen(settings_)+1])) return;
	char *pPrtToDelete = settings;

	strcpy(settings,settings_);
	char* c=settings;
	char* setting=settings;
	
		DisableDBCleanUpFPI = FALSE; //flush the seting

	while(c && *setting){
		c=strchr(setting,';');
		if(c) *c=0;
		if(!strncmp(setting,"MFlags=",7)){
			char* p=0;
			MFlags=strtol(setting+7,&p,0);
		}
		else if(!strncmp(setting,"KeepUnusedDB=",13)){
			DisableDBCleanUpFPI = TRUE;			
		}

		if(c) setting=c+1;
	}

	delete [] pPrtToDelete;
//turbpa010412-[14323]}
}
*/
extern BOOL AVP_SetTempPath(char* name);

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

extern DWORD CurRefdata;

BOOL _QueryAbort(DWORD refdata_)
{
	BOOL ret=0;
	if(!refdata_)
		refdata_=CurRefdata;
	if(refdata_ && FPI_report){
		ret=FPI_report(FPIEVENT_QUERYABORT, 0 , 0, NULL, refdata_);
		if(ret)	BAvpCancelProcessObject(1);
	}
	return ret;
}
#endif

