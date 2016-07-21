#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
||defined(__LINUX__))
 #include "stdafx.h"
#else
 #ifdef __LINUX__
//  #include <os2.h>
  #include <stdlib.h>//malloc.h>
//  #include <time.h>
//  #include <dos.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <dirent.h>
  #include <string.h>
  #include <stdio.h>
  #include "typedef.h"
  #include "../TYPEDEF.H"
  #include "../../CommonFiles/Stuff/_CARRAY.H"
  #include "profile.h"
  #include <time.h>
  char* CheckSymlink(char *path, dev_t my_dev);
 #else
  #include <os2.h>
  #include "memalloc.h"
  #include <time.h>
  #include <dos.h>
  #include <fcntl.h>
  #include <string.h>
  #include "../typedef.h"
  #include "../_carray.h"
  #include "../_avpio.h"
 #endif
#endif

#include "loadkeys.h"

//#define AVPIOSTATIC

#ifdef __LINUX__
 #include "../../CommonFiles/_AVPIO.H"
 #include "../../CommonFiles/Sign/SIGN.H"
 #include "../../CommonFiles/serialize/KLDTSer.h"
 #include "../../CommonFiles/loadkeys/AVPKeyID.h"
 #include "../../CommonFiles/Stuff/TEXT2BIN.H"
#else
 #include "../_avpio.h"
 #include "../sign\sign.h"
 #include "../kldt\kldtser.h"
 #include "../kldt\avpkeyids.h"
 #include "../text2bin\text2bin.h"
#endif

//#define OLD_KEY_SUPPORT

#ifdef OLD_KEY_SUPPORT
//#include "key.h"
//#include "unsqu.h"
#include "../key.h"

AVP_Key *avpKey=NULL;

unsigned char avp_key_present=0, // AVP.KEY: 1 - present, 0 - absent
              avpKeyFlag=0;    // 1 - work as registered copy
                                 // 0 - work as unregistered evaluation copy

static DWORD LoadOldKey(char* filename, HDATA& hRootData, DWORD appID);
//static DWORD LoadOldKey(char* filename, CPtrArray* array, DWORD appID);
#endif

//static DWORD LoadKey(char* filename, CPtrArray* array, DWORD appID);
static DWORD LoadKey(char* filename, HDATA& hRootData, DWORD appID);
//static BOOL CheckKeyUseTime(AVP_dword* keynumber,AVP_dword life,SYSTEMTIME& st);
static BOOL CheckSelfExpir(SYSTEMTIME &st);

DWORD AddKey(char *KeyName,CPtrArray* array,DWORD appID)
 {
  DWORD ret=0;
  HDATA hRootData=0;
  DWORD r=LoadKey(KeyName,hRootData, appID);
#ifdef OLD_KEY_SUPPORT
  if(!r)r=LoadOldKey(KeyName,hRootData, appID);
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
  return ret;
 }
 
#include <sys/stat.h>
DWORD LoadKeys(const char* path,CPtrArray* array,DWORD appID)
 {
  struct stat fStats;
  DWORD ret=0;
  char string[0x200];
  char* namePtr=NULL;
  *string=0;
  strncpy(string,path,0x200);

  //if(strlen(string)>1) if(string[strlen(string)-1]=='/') string[strlen(string)-1]=0;
  namePtr=strrchr(string,'/');
  if(namePtr) namePtr++;
  if(namePtr==NULL)namePtr=string+strlen(string);
  //strcpy(namePtr,"*.key");

  if(lstat(string,&fStats)==-1)
   {
    if(strlen(namePtr)>0)
     {
      char *mask=strrchr(string,'/');
      *mask=0;
     } 
    if(lstat(string,&fStats)==-1) return 0;
   }    
  if(S_ISDIR(fStats.st_mode))
   {
    DIR *dp;
    if((dp=opendir(string))!=NULL)
     {
      int pathLen=strlen(string);
      char *dirName=string+pathLen;
      if(*(dirName-1)!='/')  *dirName++='/';
      struct dirent *dirp;
      while((dirp=readdir(dp))!=NULL)
       if(strcmp(dirp->d_name,".") && strcmp(dirp->d_name,".."))
        {
         strcpy(dirName,dirp->d_name);
         struct stat st;
         if(lstat(string,&st)!=-1)
          {
	   if(S_ISLNK(st.st_mode))
	    {
             if(prf_data.Symlinks==2)
	      {
	       char *TargetPath=CheckSymlink(string,fStats.st_dev);//dev);
	       if(TargetPath!=NULL)
                if(lstat(TargetPath,&st)!=-1)
  	         if(S_ISDIR(st.st_mode))
		  {
//                   if((prf_data.CrossFs==1)||((prf_data.CrossFs==0)&&(st.st_dev==dev)))
                    LoadKeys(TargetPath,array,appID);//,st.st_dev);
		  }
                 else
                  {
                   if(S_ISREG(st.st_mode) //&& ((!fStats.st_size && verbose>4)|| fStats.st_size)
                    )
                   ret|=AddKey(TargetPath,array,appID);
 	          }
	      } 
	    }  
	   else
	    if(S_ISDIR(st.st_mode))
	     {
              if((prf_data.CrossFs==1)||((prf_data.CrossFs==0) ))//&&(st.st_dev==dev)))
		LoadKeys(string,array,appID);//,dev);
	     }
            else
             {
              if(S_ISREG(st.st_mode) //&& ((!fStats.st_size && verbose>4)|| fStats.st_size)
                )
              ret|=AddKey(string,array,appID);
	     } 
          }
        } //end while ReadDir && !='.'
     if(closedir(dp)<0) ;
     string[pathLen]='\0';
    }
  } //if(S_ISDIR(fStats.st_mode))     
 else
  if(S_ISREG(fStats.st_mode))
   {
    ret|=AddKey(string,array,appID);
   }
  return ret;
 }

static DWORD LoadKey(char* filename, HDATA& hRootData, DWORD appID)
 {
  DWORD ret=0;

  if(sign_check_file(filename,1,0,0,0))
   {
   
    //HANDLE f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    //if(f==INVALID_HANDLE_VALUE) return ret;
    int f;
    if((f=open(filename,O_RDONLY))==-1) return ret;
    DWORD count;
    DWORD val=0;
    //ReadFile(f,&val,4,&count,0);
    read(f,(void far *)&val,4);
    //CloseHandle(f);
    close(f);
    if(count!=4 || val!=0x4b4c7377) return ret;
 //'wsLK' 0x77734c4b
    if(!DATA_Init_Library(malloc,free)) return ret;

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

  if(!KLDT_Init_Library(malloc,free)) return ret;
  if(!DATA_Init_Library(malloc,free)) return ret;

  AVP_dword dseq[5];
  AVP_dword level=0;

//  if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return ret;
  if(!KLDT_Deserialize( filename, &hRootData ))return ret;
  if(hRootData==NULL) return ret;
  AVP_dword pid;
  pid=MAKE_AVP_PID(appID, AVP_APID_GLOBAL, avpt_dword, 0);
  if(AVP_PID_KEYROOT!=DATA_Get_Id( hRootData, 0 )
  || !DATA_Get_Val( hRootData, DATA_Sequence(dseq,pid,0), 0, &level, sizeof(level)) )
   {
    DATA_Remove( hRootData, 0 );
    hRootData=0;
    return ret;
   }
  ret|=FN_MINIMAL;
  if(level>=10) ret|=FN_UPDATES;
  if(level>=20) ret|=FN_OPTIONS;
  if(level>=30) ret|=FN_FULL;

  //SYSTEMTIME stCurrent;
  SYSTEMTIME stExpir;
  //GetSystemTime(&stCurrent);
  time_t Timer;
  struct tm* stCurrent;
  Timer=time((time_t*)NULL);
  stCurrent=localtime(&Timer);
  memset(&stExpir,0,sizeof(SYSTEMTIME));
  if( !DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0),0,&stExpir,sizeof(AVP_date)) ) goto dr;

  AVP_dword keynumber[3];
  DATA_Get_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSERIALNUMBER,0), 0, keynumber, sizeof(keynumber));
  char buf[0x20];
  sprintf(buf,"%04ld-%06ld-%08ld",keynumber[0],keynumber[1],keynumber[2]);
  DATA_Add(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,0), PID_SERIALSTR, (DWORD)buf,0);

  AVP_dword life;
  if( DATA_Get_Val( hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLIFESPAN,0), 0, &life, sizeof(AVP_dword)) )
   {
    //if(!CheckKeyUseTime(keynumber,life,stExpir))goto time_bad;
   }

  if(stCurrent->tm_year >stExpir.wYear) goto time_bad;
  if(stCurrent->tm_year <stExpir.wYear) goto time_ok;
  if(stCurrent->tm_mon>stExpir.wMonth)goto time_bad;
  if(stCurrent->tm_mon<stExpir.wMonth)goto time_ok;
  if(stCurrent->tm_mday  >stExpir.wDay)  goto time_bad;
  if(stCurrent->tm_mday  <stExpir.wDay)  goto time_ok;
time_bad:
  if(DATA_Find(hRootData, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0))
   || !CheckSelfExpir(stExpir)) ret=0;
  else ret &= ~FN_UPDATES;
time_ok:

  DATA_Set_Val(hRootData,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, (DWORD)&stExpir, sizeof(AVP_date));

dr:
  DATA_Add(hRootData,0,PID_FILENAME,(DWORD)filename,0);

  return ret;
 }

//static
BOOL CheckKeyUseTime(AVP_dword* keynumber, AVP_dword life,SYSTEMTIME& st)
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
//   {
//    char res[0x200];
//    res[BinToText(keynumber,3*sizeof(AVP_dword),res,0x200)]=0;
//    DWORD type=REG_BINARY;
//    DWORD size=sizeof(time_t);
//    if(ERROR_SUCCESS!=RegQueryValueEx(hKey,res,NULL,&type,(LPBYTE)&tt_first,&size))
//               RegSetValueEx(hKey,res,NULL,type,(LPBYTE)&tt_first,size);
//      RegCloseKey(hKey);
//   }
keynumber=keynumber;

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
static DWORD LoadOldKey(char* filename, HDATA& hRootData, DWORD appID)
 {
  DWORD ret=0;
  AVP_KeyHeader  avpKeyHeader;

  int handle;
  unsigned char *cbuf;

  if((handle=open(filename,O_RDONLY))==-1) return ret;
  if((read(handle,(void far *)&avpKeyHeader,sizeof(avpKeyHeader))
==-1)
  ||avpKeyHeader.Magic!=AVP_KEY_MAGIC||avpKeyHeader.Version!=3
  ||((cbuf=(unsigned char *)malloc(avpKeyHeader.CompressedSize))==0))
   { close(handle); return ret; }

  //if(KeyHeader.CompressedCRC==CalcSum((BYTE*)cbuf,KeyHeader.CompressedSize))
  if(((avpKey=(struct _AVP_Key *)malloc(avpKeyHeader.UncompressedSize))==NULL)
  ||(read(handle,(void *)cbuf,avpKeyHeader.CompressedSize)==-1))
   { close(handle); free(cbuf); return ret; }
  close(handle);

  if(avpKeyHeader.CompressedCRC==CalcSum((BYTE*)cbuf,avpKeyHeader.CompressedSize))
   {
    for(unsigned int i=0;i<avpKeyHeader.CompressedSize;i++) cbuf[i]^=(BYTE)i;
    if(avpKeyHeader.UncompressedSize==unsqu(cbuf,(unsigned char *)avpKey) )//&&!FFake)
     {
      avp_key_present=1;

      BOOL app_ok;
      app_ok=FALSE;
      switch(appID)
       {
        case AVP_APID_SCANNER_WIN_95:
          app_ok=!!(avpKey->Platforms & KEY_P_WIN95);
          break;
        case AVP_APID_SCANNER_WIN_NTWS:
          app_ok=!!(avpKey->Platforms & KEY_P_WINNT);
          break;
        case AVP_APID_MONITOR_WIN_95:
          app_ok=!!(avpKey->Platforms & KEY_P_WIN95) && !!(avpKey->Options & KEY_O_MONITOR);
          break;
        case AVP_APID_MONITOR_WIN_NTWS:
          app_ok=!!(avpKey->Platforms & KEY_P_WINNT) && !!(avpKey->Options & KEY_O_MONITOR);
          break;
        case AVP_APID_OS_2:
          app_ok=!!(avpKey->Platforms & KEY_P_OS2);
          break;
        case AVP_APID_SCANNER_DOS_LITE:
          app_ok=!!(avpKey->Platforms & KEY_P_DOSLITE);
          break;
        case AVP_APID_NOVELL:
          app_ok=!!(avpKey->Platforms & KEY_P_NOVELL);
          break;
        case AVP_APID_SCANNER_DOS_32:
        case AVP_APID_SCANNER_DOS_16:
          app_ok=!!(avpKey->Platforms & KEY_P_DOS);
          break;
        case AVP_APID_CONTROLCENTER:
        case AVP_APID_UPDATOR:
          app_ok=!!(avpKey->Platforms & KEY_P_WIN95) || !!(avpKey->Platforms & KEY_P_WINNT);
          break;
        case AVP_APID_INSPECTOR:
        case AVP_APID_WEBINSPECTOR:
        case AVP_APID_EXCHANGE:
        case AVP_APID_NT_SERVER:
        default:
          break;
       }

      if(app_ok)
       {
        ret=FN_MINIMAL;
        ret|=FN_OPTIONS;

        if(avpKey->Options & KEY_O_REMOTELAUNCH)ret|=FN_NETWORK;
        if(avpKey->Options & KEY_O_REMOTESCAN)  ret|=FN_NETWORK;

        if(!(avpKey->Options & KEY_O_DISINFECT))ret&=~FN_DISINFECT;
        if(!(avpKey->Options & KEY_O_UNPACK))   ret&=~FN_UNPACK;
        if(!(avpKey->Options & KEY_O_EXTRACT))  ret&=~FN_EXTRACT;

        //SYSTEMTIME stCurrent;
        SYSTEMTIME stExpir;
        //GetSystemTime(&stCurrent);
        time_t Timer;
        struct tm* stCurrent;
        Timer=time((time_t*)NULL);
        stCurrent=localtime(&Timer);
        memset(&stExpir,0,sizeof(SYSTEMTIME));
        stExpir.wYear =avpKey->ExpirYear;
        stExpir.wMonth=(unsigned char)avpKey->ExpirMonth;
        stExpir.wDay  =(unsigned char)avpKey->ExpirDay;
        if(stCurrent->tm_year >stExpir.wYear) goto time_bad;
        if(stCurrent->tm_year <stExpir.wYear) goto time_ok;
        if(stCurrent->tm_mon>stExpir.wMonth)goto time_bad;
        if(stCurrent->tm_mon<stExpir.wMonth)goto time_ok;
        if(stCurrent->tm_mday  >stExpir.wDay)  goto time_bad;
        if(stCurrent->tm_mday  <stExpir.wDay)  goto time_ok;
time_bad:
        if(!!(avpKey->Flags & KEY_F_INFOEVERYLAUNCH)
           || !(avpKey->Flags & KEY_F_REGISTERED)
           || !CheckSelfExpir(stExpir))
         ret=0;
        else
         ret &= ~FN_UPDATES;
time_ok:

        if(DATA_Init_Library(malloc,free))
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
  free(cbuf);

  return ret;
 }

#endif
