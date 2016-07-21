////////////////////////////////////////////////////////////////////
//
//  LoadKeys.cpp
//  AVP key loading and preprocessing (old and new formats)
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "LoadKeys.h"


#define AVPIOSTATIC
#include <_avpio.h>
#include <Sign\sign.h>
#include <serialize/kldtser.h>
#include <loadkeys/avpkeyid.h>
#include <Stuff\text2bin.h>
#include <Stuff\calcsum.h>
#include <LoadKeys\CheckUse.h>
#define MAX(a,b)   ((a) > (b) ? (a) : (b))

//#define OLD_KEY_SUPPORT
#undef OLD_KEY_SUPPORT
//!!! WE STOPPED OLD KEY SUPPORT !!!

#ifdef OLD_KEY_SUPPORT
#include "key.h"
static BOOL LoadOldKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData);
#endif

static BOOL LoadKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData);
//static BOOL CheckKeyUseTime(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& stExpir, AVP_dword* PLPIfTrial);
//static BOOL CheckSelfExpir(SYSTEMTIME& stExpir);

BOOL LoadKeys(DWORD appID, const char* path, DWORD* level, BOOL* valid, CPtrArray* array)
{
	BOOL ret=0;
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	char string[0x200];
	char* namePtr=NULL;
	
	if(!DATA_Init_Library(malloc,free)) return ret;

	*string=0;
	GetFullPathName(path,0x200,string,&namePtr);
	if(namePtr==NULL)namePtr=string+strlen(string);
	strcpy(namePtr,"*.key");

	hFind = FindFirstFile(string, &FindData);
	if(hFind == INVALID_HANDLE_VALUE) return 0;
	do{
		if(0==(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			strcpy(namePtr,FindData.cFileName);
			if(!strlen(FindData.cFileName)||(DWORD)-1==GetFileAttributes(string))
			{
				strcpy(namePtr,FindData.cAlternateFileName);
				if(!strlen(FindData.cAlternateFileName)||(DWORD)-1==GetFileAttributes(string)) 
					continue;
			}
			HDATA hRootData=0;
			BOOL r=LoadKey(appID, string, level, valid, hRootData);
#ifdef OLD_KEY_SUPPORT
			if(!r)r=LoadOldKey(appID, string, level, valid, hRootData);
#endif
			ret|=r;
			
			if(hRootData){
				int i=0;
				if(array){
					AVP_dword dseq[5];
					char key1[0x80];
					DATA_Get_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0), 0, key1, 0x80);
					for(i=array->GetUpperBound();i>=0;i--)
					{	
						char key2[0x80];
						HDATA hKey=(HDATA)array->GetAt(i);
						if(DATA_Get_Val(hKey, dseq, 0, key2, 0x80))
							if(!strcmp(key1,key2))
								break;
					}
					if(i==-1) array->Add(hRootData);
				}
				
				if(i!=-1) DATA_Remove( hRootData, 0 );
			}
		}
	}while( FindNextFile(hFind, &FindData));
	FindClose(hFind);

	return ret;
}


static BOOL LoadKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData)
{
	BOOL ret=0;
	DWORD Level=0;
	BOOL Valid=TRUE;

	if(sign_check_file(filename,1,0,0,0))
	{
		HANDLE f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if(f==INVALID_HANDLE_VALUE) return ret;
		DWORD count;
		DWORD val=0;
		ReadFile(f,&val,4,&count,0);
		CloseHandle(f);
		if(count!=4 || val!='wsLK') return ret;
		if(!DATA_Init_Library(malloc,free)) return ret;

		hRootData=DATA_Add(0,0,AVP_PID_KEYROOT,0,0);
		if(!hRootData) return ret;

		DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

		HDATA hInfo;
		hInfo=DATA_Add(hRootData,0,AVP_PID_KEYINFO,0,0);
		DATA_Add(hInfo,0, PID_SERIALSTR,  (DWORD)"Corrupted", 0);
		return ret;
	}

	AvpCloseHandle=CloseHandle;
	AvpWriteFile=WriteFile;
	AvpCreateFile=CreateFile;
	AvpGetFileSize=GetFileSize;
	AvpReadFile=ReadFile;

	if(!KLDT_Init_Library(malloc,free)) return ret;
	if(!DATA_Init_Library(malloc,free)) return ret;

	AVP_dword dseq[5];


	if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return ret;
	if(hRootData==NULL) return ret;

	if(!DATA_Find(hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,0)))goto drem;

	ret=TRUE;
	if(appID)
	{
		if(AVP_PID_KEYROOT!=DATA_Get_Id( hRootData, 0 )
			|| !DATA_Get_Val(
				hRootData, DATA_Sequence(dseq, 
					MAKE_AVP_PID(appID, AVP_APID_GLOBAL, avpt_dword, 0),
				0), 0, &Level, sizeof(Level)
			) 
		)
		{
drem:		
			DATA_Remove( hRootData, 0 );
			hRootData=0;
			return ret;
		}
	}

	SYSTEMTIME stExpir;
	memset(&stExpir,0,sizeof(SYSTEMTIME));
	if( !DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, &stExpir, sizeof(AVP_date)) ) goto dr;

	AVP_dword keynumber[3];
	DATA_Get_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSERIALNUMBER,0), 0, keynumber, sizeof(keynumber));
	char buf[0x20];
	sprintf(buf,"%04X-%06X-%08X",keynumber[0],keynumber[1],keynumber[2]);
	DATA_Add(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,0), PID_SERIALSTR, (DWORD)buf,0);


	BOOL keyIsTrial;
	keyIsTrial=(DATA_Find(hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))?1:0;
	AVP_dword PLPos;
	PLPos=0;
	DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOS,0), 0, &PLPos, sizeof(AVP_dword));

	AVP_dword life;
	life=0;
	DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLIFESPAN,0), 0, &life, sizeof(AVP_dword));
	Valid=CheckKeyUseTime(keynumber,life,stExpir,keyIsTrial?&PLPos:NULL);

	if(!Valid &&( keyIsTrial|| !CheckSelfExpir(stExpir)))
		Level=0;

	DATA_Set_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, (DWORD)&stExpir, sizeof(AVP_date));

dr:
	DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

	if(ret){
		if(level) *level=MAX(*level, Level);
		if(valid) *valid|=Valid;
	}
	return ret;

}

#ifdef OLD_KEY_SUPPORT

extern DWORD unsqu (BYTE *i_buf,BYTE *o_buf);

static BOOL LoadOldKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData)
{
	BOOL ret=0;
	HANDLE f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f==INVALID_HANDLE_VALUE) return ret;

	AVP_KeyHeader KeyHeader;
	AVP_Key* Key;
	char *cbuf;
	char *buf;
	DWORD count;
	DWORD Level=0;
	BOOL Valid=TRUE;
	BOOL platf=FALSE;
	UINT i;
	ReadFile(f,&KeyHeader,sizeof(AVP_KeyHeader),&count,0);
	if(KeyHeader.Magic!=(DWORD)AVP_KEY_MAGIC){
		CloseHandle(f);
		return ret;
	}
	ret=TRUE;
	cbuf=new char[KeyHeader.CompressedSize];
	buf=new char[KeyHeader.UncompressedSize];
	ReadFile(f,cbuf,KeyHeader.CompressedSize,&count,0);
	CloseHandle(f);

	if(KeyHeader.CompressedCRC!=CalcSum((BYTE*)cbuf,KeyHeader.CompressedSize))goto clo;
	for(i=0;i<KeyHeader.CompressedSize;i++) cbuf[i]^=(BYTE)i;
	if(KeyHeader.UncompressedSize!=unsqu((BYTE*)cbuf,(BYTE*)buf))goto clo;

	Key=(AVP_Key*)buf;
	
	switch(appID)
	{
	case AVP_APID_SCANNER_WIN_95:
		platf=!!(Key->Platforms & KEY_P_WIN95);
		break;
	case AVP_APID_SCANNER_WIN_NTWS:
		platf=!!(Key->Platforms & KEY_P_WINNT);
		break;
	case AVP_APID_MONITOR_WIN_95:
		platf=!!(Key->Platforms & KEY_P_WIN95) && !!(Key->Options & KEY_O_MONITOR);
		break;
	case AVP_APID_MONITOR_WIN_NTWS:
		platf=!!(Key->Platforms & KEY_P_WINNT) && !!(Key->Options & KEY_O_MONITOR);
		break;
	case AVP_APID_OS_2:
		platf=!!(Key->Platforms & KEY_P_OS2);
		break;
	case AVP_APID_SCANNER_DOS_LITE:
		platf=!!(Key->Platforms & KEY_P_DOSLITE);
		break;
	case AVP_APID_NOVELL:         
		platf=!!(Key->Platforms & KEY_P_NOVELL);
		break;
	case AVP_APID_SCANNER_DOS_32:
	case AVP_APID_SCANNER_DOS_16:
		platf=!!(Key->Platforms & KEY_P_DOS);
		break;
	case AVP_APID_CONTROLCENTER:
	case AVP_APID_UPDATOR:       
		platf=!!(Key->Platforms & KEY_P_WIN95) || !!(Key->Platforms & KEY_P_WINNT);
		break;
	case 0:
		platf=1;
		break;
	case AVP_APID_INSPECTOR:   
	case AVP_APID_WEBINSPECTOR:
	case AVP_APID_EXCHANGE:
	case AVP_APID_NT_SERVER:
	default:
		break;
	}
	
	if(!platf) goto clo;

	SYSTEMTIME stCurrent;
	SYSTEMTIME stExpir;
	GetLocalTime(&stCurrent);
//	GetSystemTime(&stCurrent);
	memset(&stExpir,0,sizeof(SYSTEMTIME));
	stExpir.wYear =Key->ExpirYear;
	stExpir.wMonth=Key->ExpirMonth;
	stExpir.wDay  =Key->ExpirDay;
	if(stCurrent.wYear > stExpir.wYear) Valid=FALSE;
	else if(stCurrent.wYear < stExpir.wYear) ;
	else if(stCurrent.wMonth > stExpir.wMonth) Valid=FALSE;
	else if(stCurrent.wMonth < stExpir.wMonth) ;
	else if(stCurrent.wDay > stExpir.wDay) Valid=FALSE;

	if( !CheckSelfExpir(stExpir))
	{
		Valid=FALSE;
	}
	else
	{
		if(Key->Options & KEY_O_DISINFECT)
		{
			Level=10;

			if((Key->Options & KEY_O_UNPACK)
			|| (Key->Options & KEY_O_EXTRACT))
				Level=20;

			if((Key->Options & KEY_O_REMOTELAUNCH)
			|| (Key->Options & KEY_O_REMOTESCAN))
				Level=30;
		}
		else{
			if(Key->Flags & KEY_F_INFOEVERYLAUNCH)
				Valid=FALSE;
			else
				Level=5;
		}
	}

	if(!DATA_Init_Library(malloc,free)) goto clo;

	hRootData=DATA_Add(0,0,AVP_PID_KEYROOT,0,0);
	if(!hRootData) goto clo;

	DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

	HDATA hInfo;
	hInfo=DATA_Add(hRootData,0,AVP_PID_KEYINFO,0,0);

	OemToChar(buf+Key->RegNumberOffs, buf+Key->RegNumberOffs);
	OemToChar(buf+Key->CopyInfoOffs, buf+Key->CopyInfoOffs);
	OemToChar(buf+Key->SupportOffs, buf+Key->SupportOffs);

	DATA_Add(hInfo,0, PID_SERIALSTR, (DWORD)buf+Key->RegNumberOffs,0);
	DATA_Add(hInfo,0, AVP_PID_KEYPLPOSNAME,  (DWORD)"AVP Key v1.0", 0);
	DATA_Add(hInfo,0, AVP_PID_KEYLICENCEINFO, (DWORD)buf+Key->CopyInfoOffs,0);
	DATA_Add(hInfo,0, AVP_PID_KEYSUPPORTINFO, (DWORD)buf+Key->SupportOffs,0);
	if(stExpir.wDay!=0 && Valid)
	DATA_Add(hInfo,0, AVP_PID_KEYEXPDATE, (DWORD)&stExpir, sizeof(AVP_date));

clo:
	delete cbuf;
	delete buf;

	if(ret){
		if(level) *level=MAX(*level, Level);
		if(valid) *valid|=Valid;
	}
	return ret;
}

#endif OLD_KEY_SUPPORT

