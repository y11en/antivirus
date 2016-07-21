#if !(defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
 #include "stdafx.h"
#else
 #include <os2.h>
 #include "memalloc.h"
 #include <time.h>
 #include <dos.h>
 #include <fcntl.h>
 #include <string.h>
 #include "..\\typedef.h"
 #include "../../Common.Fil/_avpio.h"
 #include "../../Common.Fil/stuff/_CARRAY.H"
#endif
#include "LoadKeys.h"
//#include "..\fn.h"

#define AVPIOSTATIC
#include "../../Common.Fil/_avpio.h"
#include "../../Common.Fil/AvpkeyIDs.h"
#include "../../Common.Fil/sign\sign.h"
#include "../../Common.Fil/kldtser.h"
#include "../../Common.Fil/stuff/text2bin.h"
//#include "..\CommonFiles\serialize/kldtser.h"
//#include "..\CommonFiles\loadkeys/avpkeyid.h"
//#include "..\text2bin\text2bin.h"
//#include "..\common\calcsum\calcsum.h"

int _sprintf(char * buf, const char *fmt, ...);

#define MAX(a,b)   ((a) > (b) ? (a) : (b))

#define OLD_KEY_SUPPORT

#ifdef OLD_KEY_SUPPORT
//#include "key.h"
//#include "unsqu.h"
#include "..\key.h"

AVP_Key *avpKey=NULL;

unsigned char avp_key_present=0, // AVP.KEY: 1 - present, 0 - absent
              avpKeyFlag=0;    // 1 - work as registered copy
                                 // 0 - work as unregistered evaluation copy

static DWORD LoadOldKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData);
//static DWORD LoadOldKey(char* filename, HDATA& hRootData, DWORD appID);
//static DWORD LoadOldKey(char* filename, CPtrArray* array, DWORD appID);
#endif

static DWORD LoadKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData);
//static BOOL CheckKeyUseTime(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& st, AVP_dword* PLPIfTrial);
static BOOL CheckSelfExpir(SYSTEMTIME& st);

DWORD LoadKeys(DWORD appID, const char* path, DWORD* level, BOOL* valid, CPtrArray* array)
 {
  DWORD ret=0;
  struct find_t ffblk;
  char string[0x200];
  char* namePtr=NULL;
  if(!DATA_Init_Library((void*(*)(unsigned int))Malloc,Free)) return ret;
  *string=0;
  //GetFullPathName(path,0x200,string,&namePtr);
  strncpy(string,path,0x200);
  namePtr=strrchr(string,'\\');
  if(namePtr) namePtr++;
  else        namePtr=string;

  if(namePtr==NULL)namePtr=string+strlen(string);
//  strcpy(namePtr,"*.key");

  if(_dos_findfirst(string,_A_ARCH|_A_SYSTEM|_A_HIDDEN|_A_RDONLY,&ffblk)==0)
   {
    do
     {
      if((ffblk.attrib&0x10)==0)
      //if(0==(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
       {
        strcpy(namePtr,ffblk.name);//FindData.cFileName);
//        if(!strlen(FindData.cFileName)||(DWORD)-1==GetFileAttributes(string))
//         {
//          strcpy(namePtr,FindData.cAlternateFileName);
//          if(!strlen(FindData.cAlternateFileName)||(DWORD)-1==GetFileAttributes(string))
//                continue;
//         }
//        ret|=LoadKey(string, array, appID);
//#ifdef OLD_KEY_SUPPORT
//        if(!ret) ret|=LoadOldKey(string,array,appID);
//#endif
        HDATA hRootData=0;
        BOOL r=LoadKey(appID, string, level, valid, hRootData);
#ifdef OLD_KEY_SUPPORT
        if(!r)r=LoadOldKey(appID, string, level, valid, hRootData);
#endif
        ret|=r;

        if(hRootData)
         {
          int i=0;
          if(array)
           {
            AVP_dword dseq[5];
            char key1[0x80];
            DATA_Get_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0), 0, key1, 0x80);
            for(i=array->GetUpperBound();i>=0;i--)
             {
              char key2[0x80];
              HDATA hKey=(HDATA)array->GetAt(i);
              if(DATA_Get_Val(hKey, dseq, 0, key2, 0x80))
                if(!strcmp(key1,key2)) break;
             }
            if(i==-1) array->Add(hRootData);
           }

          if(i!=-1) DATA_Remove( hRootData, 0 );
         }
       }
     }while(_dos_findnext(&ffblk)==0);// FindNextFile(hFind, &FindData));
    _dos_findclose(&ffblk);
   }
  return ret;
 }

static DWORD LoadKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData)
 {
  DWORD ret=0;
  DWORD Level=0;
  BOOL Valid=TRUE;

  if(sign_check_file(filename,1,0,0,0))
   {
    int f;
    if(_dos_open(filename,O_RDONLY,&f)) return ret;
    DWORD count;
    DWORD val=0;
    _dos_read(f,(void far *)&val,4,(unsigned int*)&count);
    _dos_close(f);
    if(count!=4 || val!='wsLK') return ret;
    if(!DATA_Init_Library((void*(*)(unsigned int))Malloc,Free)) return ret;

    hRootData=DATA_Add(0,0,AVP_PID_KEYROOT,0,0);
    if(!hRootData) return ret;

    DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

    HDATA hInfo;
    hInfo=DATA_Add(hRootData,0,AVP_PID_KEYINFO,0,0);
    DATA_Add(hInfo,0, PID_SERIALSTR,  (DWORD)"Corrupted", 0);
    return ret;
   }

  _AvpReadFile   =AvpReadFile;
  _AvpWriteFile  =AvpWriteFile;
  _AvpCloseHandle=AvpCloseHandle;
  _AvpCreateFile =AvpCreateFile;
  _AvpGetFileSize=AvpGetFileSize;

  if(!KLDT_Init_Library((void*(*)(unsigned int))Malloc,Free)) return ret;
  if(!DATA_Init_Library((void*(*)(unsigned int))Malloc,Free)) return ret;

  AVP_dword dseq[5];

  if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return ret;
  if(hRootData==NULL) return ret;
  ret=TRUE;
  AVP_dword pid;
  pid=MAKE_AVP_PID(appID, AVP_APID_GLOBAL, avpt_dword, 0);
  if(AVP_PID_KEYROOT!=DATA_Get_Id( hRootData, 0 )
  || !DATA_Get_Val( hRootData, DATA_Sequence(dseq,pid,0), 0, &Level, sizeof(Level)) )
   {
    DATA_Remove( hRootData, 0 );
    hRootData=0;
    return ret;
   }

//  ret|=FN_MINIMAL;
//  if(level>=10) ret|=FN_UPDATES;
//  if(level>=20) ret|=FN_OPTIONS;
//  if(level>=30) ret|=FN_FULL;

  struct dosdate_t stCurrent;
  SYSTEMTIME stExpir;
  _dos_getdate(&stCurrent);
  memset(&stExpir,0,sizeof(SYSTEMTIME));
  if( !DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0),0,&stExpir,sizeof(AVP_date)) ) goto dr;

  AVP_dword keynumber[3];
  DATA_Get_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSERIALNUMBER,0), 0, keynumber, sizeof(keynumber));
  char buf[0x20];
  _sprintf(buf,"%04d-%06d-%08d",keynumber[0],keynumber[1],keynumber[2]);
  DATA_Add(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,0), PID_SERIALSTR, (DWORD)buf,0);

  BOOL keyIsTrial;
  keyIsTrial=(DATA_Find(hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))?1:0;
  AVP_dword PLPos;
  PLPos=0;
  DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOS,0), 0, &PLPos, sizeof(AVP_dword));

  if(stCurrent.year > stExpir.wYear) Valid=FALSE;
  else if(stCurrent.year < stExpir.wYear) ;
  else if(stCurrent.month > stExpir.wMonth) Valid=FALSE;
  else if(stCurrent.month < stExpir.wMonth) ;
  else if(stCurrent.day > stExpir.wDay) Valid=FALSE;

  if(Valid)
   {
    AVP_dword life;
    if( DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLIFESPAN,0), 0, &life, sizeof(AVP_dword)) )
     {
      //if(!CheckKeyUseTime(keynumber,life,stExpir,keyIsTrial?&PLPos:NULL))Valid=FALSE;
     }
   }

  if(!Valid &&( keyIsTrial|| !CheckSelfExpir(stExpir))) Level=0;

  DATA_Set_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, (DWORD)&stExpir, sizeof(AVP_date));

dr:
  DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

  if(ret){
          if(level) *level=MAX(*level, Level);
          if(valid) *valid|=Valid;
  }
  return ret;
 }
/*
static BOOL CheckKeyUseTime(AVP_dword* keynumber, AVP_dword life, SYSTEMTIME& st, AVP_dword* PLPIfTrial)
 {
  tm tm_cur;
  time_t tt_cur;
  time_t tt_first;

  time(&tt_cur);
  if(NULL==gmtime(&tt_cur)) return TRUE;
  tm_cur=*gmtime(&tt_cur);
  tt_first = tt_cur;

//  DWORD disp;
//  HKEY hKey;
//  if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_CLASSES_ROOT,"LK.Auto",0,REG_NONE,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL,&hKey,&disp))
   {
    char res[0x200];
    res[BinToText(keynumber,3*sizeof(AVP_dword),res,0x200)]=0;
//    DWORD type=REG_BINARY;
    DWORD size=sizeof(time_t);
//    if(ERROR_SUCCESS!=RegQueryValueEx(hKey,res,NULL,&type,(LPBYTE)&tt_first,&size))
//               RegSetValueEx(hKey,res,NULL,type,(LPBYTE)&tt_first,size);
    if(PLPIfTrial)
     {
      res[BinToText(PLPIfTrial,sizeof(AVP_dword),res,0x200)]=0;
//      type=REG_BINARY;
      size=sizeof(time_t);
//      if(ERROR_SUCCESS!=RegQueryValueEx(hKey,res,NULL,&type,(LPBYTE)&tt_first,&size))
//                    RegSetValueEx(hKey,res,NULL,type,(LPBYTE)&tt_first,size);
     }

//    RegCloseKey(hKey);
   }

  tt_first+=life*60*60*24;
  if(gmtime(&tt_first))
   {
    tm tm_first=*gmtime(&tt_first);
    tm_first.tm_year+=1900;
    tm_first.tm_mon+=1;
    if(st.wYear>tm_first.tm_year)
     {
      st.wYear=(unsigned short)tm_first.tm_year;
      st.wMonth=(unsigned char)tm_first.tm_mon;
      st.wDay=(unsigned char)tm_first.tm_mday;
     }
    else
     if(st.wYear==tm_first.tm_year)
      {
       if(st.wMonth>tm_first.tm_mon)
        {
         st.wMonth=(unsigned char)tm_first.tm_mon;
         st.wDay=(unsigned char)tm_first.tm_mday;
        }
       else
        if(st.wMonth==tm_first.tm_mon)
         {
          if(st.wDay>tm_first.tm_mday)
                  st.wDay=(unsigned char)tm_first.tm_mday;
         }
      }
   }

  if(difftime(tt_cur,tt_first)>0) return FALSE;
  return TRUE;
 }
*/
//#include <imagehlp.h>
#include <stdlib.h>
static BOOL CheckSelfExpir(SYSTEMTIME &st)
 {
  char *Month3[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
  char *date=__DATE__;

  //printf("%s\r\n",date);
  //date[3]=0; date[6]=0;
  tm tm_mod;
  for(int mi=0;mi<12;mi++)
   if(strncmp(Month3[mi],date,3)==0) tm_mod.tm_mon=mi+1;
  tm_mod.tm_mday=atoi(&date[4]);
  tm_mod.tm_year=atoi(&date[7]);
  //printf("%d %d %d\r\n",tm_mod.tm_mon,tm_mod.tm_mday,tm_mod.tm_year);
  //tt_mod=(time_t)GetTimestampForLoadedLibrary((HINSTANCE)GetModuleHandle(NULL));
  //if(gmtime(&tt_mod))
   {
    //tm tm_mod=*gmtime(&tt_mod);
    //tm_mod.tm_year+=1900;
    //tm_mod.tm_mon+=1;
    if(st.wYear>tm_mod.tm_year) return TRUE;
    if(st.wYear<tm_mod.tm_year) return FALSE;
    if(st.wMonth>tm_mod.tm_mon) return TRUE;
    if(st.wMonth<tm_mod.tm_mon) return FALSE;
    if(st.wDay>=tm_mod.tm_mday) return TRUE;
   }
  return FALSE;
 }

#ifdef OLD_KEY_SUPPORT

extern DWORD unsqu (BYTE *i_buf,BYTE *o_buf);

static DWORD CalcSum(BYTE* mass,DWORD len)
 {
  register DWORD i;
  register DWORD s;
  register BYTE* m;
  s=0;
  m=mass;
  if(len<4)
   {
    for (i=0; i<len ; i++,m++)
     {
      s^=s<<8;
      s^=*m;
     }
    return s;
   }

  for(i=0; i<4 ; i++,m++)
   {
    s^=s<<8;
    s^=*m;
   }
  for ( ;i<len; i++,m++)
   {
    s^=s<<1;
    s^=*m;
   }
  return s;
 }

//static DWORD LoadOldKey(char* filename,CPtrArray* array,DWORD appID)
//static DWORD LoadOldKey(char* filename, HDATA& hRootData, DWORD appID)
static DWORD LoadOldKey(DWORD appID, const char* filename, DWORD* level, BOOL* valid, HDATA& hRootData)
 {
  DWORD ret=0;
  AVP_KeyHeader  avpKeyHeader;

  int handle;
  unsigned bytes;
  unsigned char *cbuf;
  DWORD Level=0;
  BOOL  Valid=TRUE;
  BOOL  platf=FALSE;

  if(_dos_open(filename,O_RDONLY,&handle)) return ret;
  if(_dos_read(handle,(void far *)&avpKeyHeader,sizeof(avpKeyHeader),&bytes)
  ||avpKeyHeader.Magic!=AVP_KEY_MAGIC||avpKeyHeader.Version!=3
  ||((cbuf=(unsigned char *)Malloc(avpKeyHeader.CompressedSize))==0))
   { _dos_close(handle); return ret; }

  //if(KeyHeader.CompressedCRC==CalcSum((BYTE*)cbuf,KeyHeader.CompressedSize))
  if(((avpKey=(struct _AVP_Key *)Malloc(avpKeyHeader.UncompressedSize))==NULL)
  ||_dos_read(handle,(void *)cbuf,avpKeyHeader.CompressedSize,&bytes))
   { _dos_close(handle); Free(cbuf); return ret; }
  _dos_close(handle);

  if(avpKeyHeader.CompressedCRC==CalcSum((BYTE*)cbuf,avpKeyHeader.CompressedSize))
   {
    for(int i=0;i<avpKeyHeader.CompressedSize;i++) cbuf[i]^=(BYTE)i;
    if(avpKeyHeader.UncompressedSize==unsqu(cbuf,(unsigned char *)avpKey) )//&&!FFake)
     {
      avp_key_present=1;

      switch(appID)
       {
        case AVP_APID_SCANNER_WIN_95:
          platf=!!(avpKey->Platforms & KEY_P_WIN95);
          break;
        case AVP_APID_SCANNER_WIN_NTWS:
          platf=!!(avpKey->Platforms & KEY_P_WINNT);
          break;
        case AVP_APID_MONITOR_WIN_95:
          platf=!!(avpKey->Platforms & KEY_P_WIN95) && !!(avpKey->Options & KEY_O_MONITOR);
          break;
        case AVP_APID_MONITOR_WIN_NTWS:
          platf=!!(avpKey->Platforms & KEY_P_WINNT) && !!(avpKey->Options & KEY_O_MONITOR);
          break;
        case AVP_APID_OS_2:
          platf=!!(avpKey->Platforms & KEY_P_OS2);
          break;
        case AVP_APID_SCANNER_DOS_LITE:
          platf=!!(avpKey->Platforms & KEY_P_DOSLITE);
          break;
        case AVP_APID_NOVELL:
          platf=!!(avpKey->Platforms & KEY_P_NOVELL);
          break;
        case AVP_APID_SCANNER_DOS_32:
        case AVP_APID_SCANNER_DOS_16:
          platf=!!(avpKey->Platforms & KEY_P_DOS);
          break;
        case AVP_APID_CONTROLCENTER:
        case AVP_APID_UPDATOR:
          platf=!!(avpKey->Platforms & KEY_P_WIN95) || !!(avpKey->Platforms & KEY_P_WINNT);
          break;
        case AVP_APID_INSPECTOR:
        case AVP_APID_WEBINSPECTOR:
        case AVP_APID_EXCHANGE:
        case AVP_APID_NT_SERVER:
        default:
          break;
       }

      if(platf)
       {
        ret=FN_MINIMAL;
        ret|=FN_OPTIONS;

        if(avpKey->Options & KEY_O_REMOTELAUNCH)ret|=FN_NETWORK;
        if(avpKey->Options & KEY_O_REMOTESCAN)  ret|=FN_NETWORK;

        if(!(avpKey->Options & KEY_O_DISINFECT))ret&=~FN_DISINFECT;
        if(!(avpKey->Options & KEY_O_UNPACK))   ret&=~FN_UNPACK;
        if(!(avpKey->Options & KEY_O_EXTRACT))  ret&=~FN_EXTRACT;

        struct dosdate_t stCurrent;
        //SYSTEMTIME stCurrent;
        SYSTEMTIME stExpir;
        _dos_getdate(&stCurrent);
        //GetSystemTime(&stCurrent);
        memset(&stExpir,0,sizeof(struct dosdate_t));//SYSTEMTIME));
        stExpir.wYear =avpKey->ExpirYear;
        stExpir.wMonth=(unsigned char)avpKey->ExpirMonth;
        stExpir.wDay  =(unsigned char)avpKey->ExpirDay;
        if(stCurrent.year > stExpir.wYear) Valid=FALSE;
        else if(stCurrent.year < stExpir.wYear) ;
        else if(stCurrent.month > stExpir.wMonth) Valid=FALSE;
        else if(stCurrent.month < stExpir.wMonth) ;
        else if(stCurrent.day > stExpir.wDay) Valid=FALSE;
        //if(stCurrent.year >stExpir.wYear) goto time_bad;
        //if(stCurrent.year <stExpir.wYear) goto time_ok;
        //if(stCurrent.month>stExpir.wMonth)goto time_bad;
        //if(stCurrent.month<stExpir.wMonth)goto time_ok;
        //if(stCurrent.day  >stExpir.wDay)  goto time_bad;
        //if(stCurrent.day  <stExpir.wDay)  goto time_ok;
//time_bad:
//        if(!!(avpKey->Flags & KEY_F_INFOEVERYLAUNCH)
//           || !(avpKey->Flags & KEY_F_REGISTERED)
//           || !CheckSelfExpir(stExpir))
//          goto clo;
//         ret=0;
//        else
//         ret &= ~FN_UPDATES;
//time_ok:
        if(!CheckSelfExpir(stExpir))
          Valid=FALSE;
        else
         {
         if(avpKey->Options & KEY_O_DISINFECT)
          {
           Level=10;
           if((avpKey->Options & KEY_O_UNPACK)||(avpKey->Options & KEY_O_EXTRACT)) Level=20;
           if((avpKey->Options & KEY_O_REMOTELAUNCH)||(avpKey->Options & KEY_O_REMOTESCAN)) Level=30;
          }
         else
          {
           if(avpKey->Flags & KEY_F_INFOEVERYLAUNCH) Valid=FALSE;
           else Level=5;
          }
         }

        if(DATA_Init_Library((void*(*)(unsigned int))Malloc,Free))
         {
          hRootData=DATA_Add(0,0,AVP_PID_KEYROOT,0,0);
          if(hRootData)
           {
            DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

            HDATA hInfo;
            hInfo=DATA_Add(hRootData,0,AVP_PID_KEYINFO,0,0);

            //OemToChar(buf+Key->RegNumberOffs, buf+Key->RegNumberOffs);
            //OemToChar(buf+Key->CopyInfoOffs, buf+Key->CopyInfoOffs);
            //OemToChar(buf+Key->SupportOffs, buf+Key->SupportOffs);

            DATA_Add(hInfo,0, PID_SERIALSTR,(DWORD)avpKey+avpKey->RegNumberOffs,0);
            DATA_Add(hInfo,0, AVP_PID_KEYPLPOSNAME,  (DWORD)"AVP Key v1.0", 0);
            DATA_Add(hInfo,0, AVP_PID_KEYLICENCEINFO,(DWORD)avpKey+avpKey->CopyInfoOffs,0);
            DATA_Add(hInfo,0, AVP_PID_KEYSUPPORTINFO,(DWORD)avpKey+avpKey->SupportOffs,0);
            if(stExpir.wDay!=0)
             DATA_Add(hInfo,0, AVP_PID_KEYEXPDATE, (DWORD)&stExpir, sizeof(AVP_date));
           }
         }
       }
     }
    else ret=0;
    if(!(avpKey->Flags&KEY_F_REGISTERED)||!(avpKey->Platforms&KEY_P_DOS))
     ret=0;
   }
  else ret=0;
//clo:
  Free(cbuf);

  if(ret)
   {
    if(level) *level=MAX(*level, Level);
    if(valid) *valid|=Valid;
   }
  return ret;
 }

#endif




