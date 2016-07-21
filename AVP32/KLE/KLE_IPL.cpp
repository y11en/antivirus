////////////////////////////////////////////////////////////////////
//
//  KLE_IPL.CPP
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../KLE/KLEAPI.h"
#include "../KLE/KLE_IPL.h"

#define MFLAGS ( MF_MASK_ALL | MF_SFX_ARCHIVED | MF_PACKED | MF_ARCHIVED | MF_CA | MF_MAILBASES | MF_MAILPLAIN )

KLE_F_GET_API_VERSION       KLEGetAPIVersion;
KLE_F_LOAD_BASES            KLELoadBases;
KLE_F_GET_BASES_INFO        KLEGetBasesInfo;
KLE_F_INIT_SCAN_OBJECT      KLEInitScanObject;	
KLE_F_CLEANUP_SCAN_OBJECT   KLECleanupScanObject;
KLE_F_SCAN_FILE             KLEScanFile;
KLE_F_CLEAN_FILE            KLECleanFile;
KLE_F_SCAN_BOOT             KLEScanBoot;
KLE_F_CLEAN_BOOT            KLECleanBoot;
KLE_F_SCAN_MBR              KLEScanMBR;
KLE_F_CLEAN_MBR             KLECleanMBR;
KLE_F_CLEAN_MEM             KLECleanMem;
KLE_F_STOP_SCAN             KLEStopScan;
KLE_F_GET_PROP_DWORD        KLEGetPropDword;
KLE_F_SET_PROP_DWORD        KLESetPropDword;
KLE_F_GET_PROP_STRING       KLEGetPropString;
KLE_F_SET_PROP_STRING       KLESetPropString;

KLE_FUNCTION_TABLE FunctionTable={
	sizeof(KLE_FUNCTION_TABLE),
	KLEGetAPIVersion,
	KLELoadBases,
	KLEGetBasesInfo,
	KLEInitScanObject,
	KLECleanupScanObject,
	KLEScanFile,
	KLECleanFile,
	KLEScanBoot,
	KLECleanBoot,
	KLEScanMBR,
	KLECleanMBR,
	KLECleanMem,
	KLEStopScan,
	KLEGetPropDword,
	KLESetPropDword,
	KLEGetPropString,
	KLESetPropString
};

extern LOAD_PROGRESS_STRUCT LPS;

PCHAR KLE_SYS_Regname="Kaspersky Labs Anti-Virus Engine";
PCHAR KLE_SYS_Version="Version " VER_FILEVERSION_STR;
PCHAR KLE_SYS_Compilation=__TIMESTAMP__;


KLE_F_CALLBACK* LoadCallback=0;

tERROR KLEObjectCallback(AVPScanObject* avpso, DWORD message,DWORD wParam,char* dbgStr)
{
	if(avpso){
		KLE_SCAN_OBJECT* so=(KLE_SCAN_OBJECT*)(avpso->RefData);
		if(so){
			char buf[0x200];
			switch(wParam)
			{
			case AVP_CALLBACK_OBJECT_DETECT:
				if(so->Status & KLE_STAT_INFECTED)
					break;
				so->Status |= KLE_STAT_INFECTED;
				BAvpMakeFullName(buf,avpso->VirusName);
				KLESetPropString(so,KLE_PROP_VIRUS_NAME_STRING,buf,0);
				break;
			case AVP_CALLBACK_OBJECT_WARNING:
				if(so->Status & (KLE_STAT_INFECTED|KLE_STAT_SUSPIC_MODIF))
					break;
				so->Status |= KLE_STAT_SUSPIC_MODIF;
				BAvpMakeFullName(buf,avpso->WarningName);
				KLESetPropString(so,KLE_PROP_VIRUS_NAME_STRING,buf,0);
				break;
			case AVP_CALLBACK_OBJECT_SUSP:
				if(so->Status & (KLE_STAT_INFECTED|KLE_STAT_SUSPIC_MODIF|KLE_STAT_SUSPICIOUS))
					break;
				so->Status |= KLE_STAT_SUSPICIOUS;
				BAvpMakeFullName(buf,avpso->SuspicionName);
				KLESetPropString(so,KLE_PROP_VIRUS_NAME_STRING,buf,0);
				break;
			default:
				*buf=0;
				break;
			}
			so->Status|=avpso->RFlags;
			if(so->fnCallback)	so->fnCallback(so,message,wParam,dbgStr);
			if(so->StopScan)	BAvpCancelProcessObject(1);
		}
		return KLE_ERR_OK;
	}
	return KLE_ERR_BAD_PARAMETER;
}

LRESULT WINAPI KLE_AvpCallback(WPARAM wParam,LPARAM lParam)
{
	LRESULT ret=0;
	AVPScanObject* so=(AVPScanObject*)lParam;


	switch(wParam){
	case AVP_CALLBACK_PUT_STRING:
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
        break;

	case AVP_CALLBACK_LOAD_BASE:
		if(LoadCallback)	LoadCallback(0,KLE_CALLBACK_BASE_LOAD_BEGIN,lParam,"Loading");
		break;

	case AVP_CALLBACK_LOAD_BASE_DONE:
		if(LoadCallback)	LoadCallback(0,KLE_CALLBACK_BASE_LOAD_DONE,lParam,"Loading");
		break;
		
	case AVP_CALLBACK_OBJECT_WARNING:
		KLEObjectCallback(so,KLE_CALLBACK_OBJECT_INFECTED,wParam,"SUSP_MODIFICATION");
		break;

	case AVP_CALLBACK_OBJECT_SUSP:
		KLEObjectCallback(so,KLE_CALLBACK_OBJECT_INFECTED,wParam,"SUSPECTED");
		break;
	
	case AVP_CALLBACK_OBJECT_DETECT:
		KLEObjectCallback(so,KLE_CALLBACK_OBJECT_INFECTED,wParam,"INFECTED");
        break;

	case AVP_CALLBACK_OBJECT_CURE:
        break;

	case AVP_CALLBACK_OBJECT_DELETE:
        break;

	case AVP_CALLBACK_OBJECT_CURE_FAIL:
        break;

	case AVP_CALLBACK_OBJECT_ARCHIVE_NAME:
		KLESetPropString((KLE_SCAN_OBJECT*)(so->RefData),KLE_PROP_ARCHIVE_NAME_STRING,so->VirusName,0);
		KLEObjectCallback(so,KLE_CALLBACK_ARCHIVE_NEXT_NAME,wParam,"ARCHIVE_NEXT_NAME");
		break;
		
	case AVP_CALLBACK_OBJECT_ARCHIVE:
		KLEObjectCallback(so,KLE_CALLBACK_ARCHIVE_BEGIN,wParam,"ARCHIVE_BEGIN");
        break;

	case AVP_CALLBACK_OBJECT_ARCHIVE_DONE:
		KLEObjectCallback(so,KLE_CALLBACK_ARCHIVE_DONE,wParam,"ARCHIVE_DONE");
        break;
		
	case AVP_CALLBACK_OBJECT_BEGIN:
		KLEObjectCallback(so,KLE_CALLBACK_OBJECT_BEGIN,wParam,"OBJECT_BEGIN");
        break;
		
	case AVP_CALLBACK_OBJECT_DONE:
		KLEObjectCallback(so,KLE_CALLBACK_OBJECT_DONE,wParam,"OBJECT_DONE");
        break;


	case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
		ret=sign_check_file( (char*)lParam, 1, 0, 0, 0);
//		ret=sign_check_file( (char*)lParam, 1, vndArray, vndArrayCount, 0);
		break;

	case AVP_CALLBACK_ERROR_FILE_OPEN:
	case AVP_CALLBACK_ERROR_SET:
//		GkSendErrorMessage((char *)lParam, Trap_FSAV_missingDatabaseFile);
		break;


	case AVP_CALLBACK_ERROR_KERNEL_PROC:
	case AVP_CALLBACK_ERROR_LINK:
	case AVP_CALLBACK_ERROR_SYMBOL_DUP:
	case AVP_CALLBACK_ERROR_SYMBOL:
	case AVP_CALLBACK_ERROR_FIXUPP:
//		GkSendErrorMessage((char *)lParam, Trap_FSAV_cannotOpenFile);
		break;

		
	case AVP_CALLBACK_SETFILE_INTEGRITY_FAILED:
//		GkSendErrorMessage((char *)lParam, Trap_FSAV_databaseFileCorrupt);
		break;

	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
//		GkSendErrorMessage((char *)lParam, Trap_FSAV_databaseFileCorrupt);
		break;
	
	case AVP_CALLBACK_ERROR_BASE:
//		GkSendErrorMessage((char *)lParam, Trap_FSAV_notADatabaseFile);
		break;

	case AVP_CALLBACK_ROTATE: 
	case AVP_CALLBACK_ROTATE_OFF:
		if(so){
			KLE_SCAN_OBJECT* kso=(KLE_SCAN_OBJECT*)(so->RefData);
			if(kso->fnCallback)	kso->fnCallback(kso,KLE_CALLBACK_YIELD,0,"Rotate");
			if(kso->StopScan)	BAvpCancelProcessObject(1);
		}
		break;

	default:
//	        _DebugTrace(TraceInfo,"AvpCallback %X \n",wParam);
        break;
	}
    return ret;
}




tERROR KLE_CALL KLEInitScanObject( KLE_SCAN_OBJECT** pso)
{
	if(pso)*pso=new KLE_SCAN_OBJECT;
	if(*pso){
		memset(*pso,0,sizeof(KLE_SCAN_OBJECT));
		(*pso)->RefData=(void*)new AVPScanObject;
		if((*pso)->RefData){
			memset((*pso)->RefData,0,sizeof(AVPScanObject));
			((AVPScanObject*)((*pso)->RefData))->RefData=(DWORD)*pso;
			return KLE_ERR_OK;
		}
	}
	return KLE_ERR_MEM_ALLOC;
}

tERROR KLE_CALL KLECleanupScanObject( KLE_SCAN_OBJECT* so)
{
	if(so){
		if(so->ObjectName)
			delete so->ObjectName;
		if(so->TempPath)
			delete so->TempPath;
		if(so->RefData)
			delete so->RefData;
		if(so->VirusName)
			delete so->VirusName;
		if(so->VirusType)
			delete so->VirusType;
		delete so;
		return KLE_ERR_OK;
	}
	return KLE_ERR_MEM_ALLOC;
}

tERROR KLE_CALL KLELoadBases( tVOID* setname, tBOOL bUnicode, KLE_F_CALLBACK* fnCallback)
{
	tERROR err=KLE_ERR_UNKNOWN;
	char buf[0x200];
	*buf=0;

	if(bUnicode){
		return KLE_ERR_NOT_IMPLEMENTED;
	}
	else{
#ifdef NT_DRIVER
		if(*((char*)setname+1)==':')
			strcat(buf,NT_ROOT_PREFIX);
#endif
		strcat(buf,(char*)setname);
	}
	
	
	if(WaitIfBusy()){
		LoadCallback=fnCallback;
		BAvpLoadBase(buf);
		LoadCallback=0;
		ReleaseWait();
	}
	
	if(LPS.recCount)
		err=KLE_ERR_OK;
	
	return err;
}

static tERROR ScanObjectSync(AVPScanObject* avpso)
{
	if(!WaitIfBusy())
		return KLE_ERR_SYNCRONYSATION;

	BAvpProcessObject(avpso);
	
	ReleaseWait();
	return KLE_ERR_OK;
}

	
tERROR KLE_CALL KLEScanFile ( KLE_SCAN_OBJECT* so, tVOID* filename, tBOOL bUnicode)
{
	if(!so) 
		return KLE_ERR_BAD_PARAMETER;
	if(filename)
		KLESetPropString( so, KLE_PROP_OBJECT_NAME_STRING, filename, bUnicode);
	if(!so->ObjectName)
		return KLE_ERR_BAD_PARAMETER;
	AVPScanObject* avpso=(AVPScanObject*)(so->RefData);

#ifdef NT_DRIVER
	KUstring uName(0x200,NonPagedPool);
	uName.Assign(*(so->ObjectName));
	if(*(((PUNICODE_STRING)uName)->Buffer +1) == ':'){
		uName.Assign(LNT_ROOT_PREFIX);
		uName.Append(*(so->ObjectName));
	}
	avpso->NativeName=(DWORD)((PUNICODE_STRING)uName)->Buffer;
	if(!NT_SUCCESS(uName.ToPsz(avpso->Name,0x200))){
		strcpy(avpso->Name,"Object with bad name");
	}
#else
	strcpy(avpso->Name,so->ObjectName);
#endif
	avpso->MFlags=MFLAGS;
	
	return ScanObjectSync(avpso);
}

tERROR KLE_CALL KLECleanFile ( KLE_SCAN_OBJECT* so, tVOID* filename, tBOOL bUnicode)
{
	DWORD options;
	KLEGetPropDword( so, KLE_PROP_OPTIONS_DWORD, &options);
	options|=KLE_OPTION_CLEAN_VIRUS;
	KLESetPropDword( so, KLE_PROP_OPTIONS_DWORD, options);
	return KLEScanFile( so, filename, bUnicode );
}

tERROR KLE_CALL KLEGetAPIVersion( tDWORD* dwVersion)
{
	if(!dwVersion) 
		return KLE_ERR_BAD_PARAMETER;
	*dwVersion=KLE_API_Version;
	return KLE_ERR_OK;
}

tERROR KLE_CALL KLEGetBasesInfo( LOAD_PROGRESS* pLP)
{
	if(!pLP) 
		return KLE_ERR_BAD_PARAMETER;
//	*pLP=&LPS;
	return KLE_ERR_OK;
}

tERROR KLE_CALL KLEScanBoot( KLE_SCAN_OBJECT* so, tBYTE drive)
{
	if (!so)
		return KLE_ERR_BAD_PARAMETER;
	AVPScanObject* avpso=(AVPScanObject*)(so->RefData);
	
	if(!so->ObjectName){
		sprintf(avpso->Name,"Boot sector of %c:",drive+'A');
		KLESetPropString( so, KLE_PROP_OBJECT_NAME_STRING, avpso->Name, 0);
	}
	avpso->Type=OT_BOOT;
	avpso->Drive=drive;
	avpso->Disk=(drive>1)?0x80:drive;
	avpso->MFlags=MFLAGS;

	return ScanObjectSync(avpso);
}

tERROR KLE_CALL KLECleanBoot( KLE_SCAN_OBJECT* so, tBYTE drive)
{
	DWORD options;
	KLEGetPropDword( so, KLE_PROP_OPTIONS_DWORD, &options);
	options|=KLE_OPTION_CLEAN_VIRUS;
	KLESetPropDword( so, KLE_PROP_OPTIONS_DWORD, options);
	return KLEScanBoot( so, drive );
}

tERROR KLE_CALL KLEScanMBR( KLE_SCAN_OBJECT* so, tBYTE disk)
{
	if (!so)
		return KLE_ERR_BAD_PARAMETER;
	AVPScanObject* avpso=(AVPScanObject*)(so->RefData);

	if(!so->ObjectName){
		sprintf(avpso->Name,"MBR of HDD%d",disk);
		KLESetPropString( so, KLE_PROP_OBJECT_NAME_STRING, avpso->Name, 0);
	}
	avpso->Type=OT_MBR;
	avpso->Drive=2;
	avpso->Disk=0x80+disk;
	avpso->MFlags=MFLAGS;
	avpso->MFlags|=MF_ALLENTRY;
	
	return ScanObjectSync(avpso);
}

tERROR KLE_CALL KLECleanMBR( KLE_SCAN_OBJECT* so, BYTE disk)
{
	DWORD options;
	KLEGetPropDword( so, KLE_PROP_OPTIONS_DWORD, &options);
	options|=KLE_OPTION_CLEAN_VIRUS;
	KLESetPropDword( so, KLE_PROP_OPTIONS_DWORD, options);
	return KLEScanMBR( so, disk );
}

tERROR KLE_CALL KLECleanMem( KLE_SCAN_OBJECT* so)
{
	if (!so)
		return KLE_ERR_BAD_PARAMETER;
	AVPScanObject* avpso=(AVPScanObject*)(so->RefData);
	
	if(!so->ObjectName){
		sprintf(avpso->Name,"Memory");
		KLESetPropString( so, KLE_PROP_OBJECT_NAME_STRING, avpso->Name, 0);
	}
	avpso->Type=OT_MEMORY;
	avpso->MFlags=MFLAGS;
	
	return ScanObjectSync(avpso);
}

tERROR KLE_CALL KLEStopScan( KLE_SCAN_OBJECT* so, tERROR reason)
{
	so->StopScan=reason;
	so->Status|=KLE_STAT_STOPPED;
	return KLE_ERR_OK;
}

tERROR KLE_CALL KLEGetPropDword( KLE_SCAN_OBJECT* so, tDWORD propid, tDWORD* data)
{
	if(!so || !data) 
		return KLE_ERR_BAD_PARAMETER;
	AVPScanObject* avpso=(AVPScanObject*)(so->RefData);

	switch(propid)
	{
	case KLE_PROP_CALLBACK_PTR_DWORD:
		*data=(DWORD)so->fnCallback;
		break;
	case KLE_PROP_STATUS_DWORD:
		so->Status|=avpso->RFlags;
		*data=so->Status;
		break;
	case KLE_PROP_OPTIONS_DWORD:
		*data=avpso->MFlags;
		break;
	case KLE_PROP_ARC_LAYER_DWORD:
		*data=avpso->InArc;
		break;
	case KLE_PROP_PAC_LAYER_DWORD:
		*data=avpso->InPack;
		break;
	case KLE_PROP_USER_DATA_DWORD:
		*data=(DWORD)so->UserData;
		break;
	default:
		return KLE_ERR_BAD_PROPID;
	}
	return KLE_ERR_OK;
}

tERROR KLE_CALL KLESetPropDword( KLE_SCAN_OBJECT* so, tDWORD propid, tDWORD data)
{
	if(!so) 
		return KLE_ERR_BAD_PARAMETER;
	AVPScanObject* avpso=(AVPScanObject*)(so->RefData);
	
	switch(propid)
	{
	case KLE_PROP_CALLBACK_PTR_DWORD:
		so->fnCallback=(KLE_F_CALLBACK*)data;
		break;
	case KLE_PROP_OPTIONS_DWORD:
		avpso->MFlags=data;
		break;
	case KLE_PROP_USER_DATA_DWORD:
		so->UserData=(void*)data;
		break;
	case KLE_PROP_STATUS_DWORD:
	case KLE_PROP_ARC_LAYER_DWORD:
	case KLE_PROP_PAC_LAYER_DWORD:
		return KLE_ERR_PROP_READONLY;
	default:
		return KLE_ERR_BAD_PROPID;
	}
	return KLE_ERR_OK;
}


tERROR KLE_CALL KLEGetPropString( KLE_SCAN_OBJECT* so, tDWORD propid, tVOID* buffer, tBOOL bUnicode, tDWORD* size)
{
#ifdef NT_DRIVER
	KUstring* kus=0;
#else
#define buf kus
#endif
	char* buf=0;
	
	if(!so || !buffer) 
		return KLE_ERR_BAD_PARAMETER;
	switch(propid)
	{
	case KLE_PROP_TEMP_PATH_STRING:
		kus=so->TempPath;
		goto uni;
	case KLE_PROP_OBJECT_NAME_STRING:
		kus=so->ObjectName;
		goto uni;
	case KLE_PROP_ARCHIVE_NAME_STRING:
		buf=so->ArchiveName;
		goto ansi;
	case KLE_PROP_VIRUS_NAME_STRING:
		buf=so->VirusName;
		goto ansi;
	default:
		return KLE_ERR_BAD_PROPID;
	}
uni:
#ifdef NT_DRIVER
	if(!kus)
		return KLE_ERR_PROP_NOT_ALLOCATED;

	if(bUnicode){
		DWORD l=kus->Size();
		if(size){
			if(l > *size){
				*size=l;
				return KLE_ERR_BUFFER_TOO_SMALL;
			}
			*size=l;
		}
		memcpy(buffer,((PUNICODE_STRING)(*kus))->Buffer,l);
		*((DWORD*)buffer+kus->Length())=0;
	}else
	{
		char b[0x200];
		kus->ToPsz(b,0x200);
		DWORD l= strlen(b)+1;
		if(size){
			if(l > *size){
				*size=l;
				return KLE_ERR_BUFFER_TOO_SMALL;
			}
			*size=l;
		}
		memcpy(buffer,b,l);
	}
	return KLE_ERR_OK;
#endif	
ansi:
	if(!buf)
		return KLE_ERR_PROP_NOT_ALLOCATED;

	if(bUnicode){
#ifdef NT_DRIVER
		KUstring ku(buf,NonPagedPool);
		DWORD l=ku.Size();
		if(size){
			if(l > *size){
				*size=l;
				return KLE_ERR_BUFFER_TOO_SMALL;
			}
			*size=l;
		}
		memcpy(buffer,((PUNICODE_STRING)ku)->Buffer,l);
		*((char*)buffer+l)=0;
#else
		return KLE_ERR_BAD_PARAMETER;
#endif
	}
	else{
		DWORD l=strlen(buf)+1;
		if(size){
			if(l > *size){
				*size=l;
				return KLE_ERR_BUFFER_TOO_SMALL;
			}
			*size=l;
		}
		strcpy((char*)buffer,buf);
	}
	return KLE_ERR_OK;
}

tERROR KLE_CALL KLESetPropString( KLE_SCAN_OBJECT* so, tDWORD propid, tVOID* buffer, tBOOL bUnicode)
{
#ifdef NT_DRIVER
	KUstring** kus=0;
#else
#define buf kus
#endif
	char** buf=0;

	
	if(!so || !buffer) 
		return KLE_ERR_BAD_PARAMETER;
	switch(propid)
	{
	case KLE_PROP_TEMP_PATH_STRING:
		kus=&so->TempPath;
		goto uni;
	case KLE_PROP_OBJECT_NAME_STRING:
		kus=&so->ObjectName;
		goto uni;
	case KLE_PROP_ARCHIVE_NAME_STRING:
		buf=&so->ArchiveName;
		goto ansi;
	case KLE_PROP_VIRUS_NAME_STRING:
		buf=&so->VirusName;
		goto ansi;
	default:
		return KLE_ERR_BAD_PROPID;
	}
uni:
#ifdef NT_DRIVER
	{
		KUstring* tus=0;
		if(bUnicode) 
			tus=new KUstring((PWSTR)buffer,NonPagedPool);
		else
			tus=new KUstring((PSZ)buffer,NonPagedPool);
		if(tus && kus)
		{
			if(*kus) delete *kus;
			*kus=tus;
		}else	
			return KLE_ERR_MEM_ALLOC;
	}
	return KLE_ERR_OK;
#endif
ansi:
	{
		char* tbuf=0;
		if(bUnicode){
#ifdef NT_DRIVER
			KUstring us((PCWSTR)buffer);
			WORD l=us.Size();
			tbuf=new char[l];
			if(tbuf)
				us.ToPsz(tbuf,l);
#else
			return KLE_ERR_BAD_PARAMETER;
#endif
		}else{
			DWORD l=strlen((char*)buffer)+1;
			tbuf=new char[l];
			if(tbuf)
				strcpy(tbuf,(char*)buffer);
		}
		if(tbuf)
		{
			if(*buf) delete* buf;
			*buf=tbuf;
		}else	
			return KLE_ERR_MEM_ALLOC;
	}
	return KLE_ERR_OK;
}

