#include "memalloc.h"
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <ctype.h>

#include <stdio.h>


#include <os2.h>

#include "avp_os2.h"
#include "avp_rc.h"
#include "profile.h"
#include "disk.h"
#include "kernel.h"
#include "start.h"
#include "..\locale\resource.h"

#include "..\\typedef.h"
#ifndef _INTERN_VER_
#include "../../Common.Fil/stuff/_CARRAY.H"

#include "LoadKeys.h"
extern ULONG functionality;
#endif

#include "contain.h"

BOOL CheckNT(void)
 {
  //  определение из досовской программы, что она запущена под Windows
  //  NT или под Windows 95/98 без обращения к переменным окружения
//  RMI   rmi;

//  memset(&rmi,0,sizeof(RMI));
//  rmi.EAX=0x3306;//INT 21 3306 - DOS 5+ - GET TRUE VERSION NUMBER

//  if(SimulateRMInt(0x21,&rmi))
//   {
//    _printf("Major version(bl):%x minor version(bh):%x \r\n"
//           "revision(DL(bits 2-0, all others 0)):%x \r\n"
//           "version flags(dh bit 3: DOS is in ROM bit 4: DOS is in HMA):%x \r\n"
//           "al(FFh if true DOS version <5.0):%x\r\n",
//           rmi.EBX&0x000f,(rmi.EBX&0x00f0)>>8,
//           rmi.EDX&0x000f,(rmi.EDX&0x00f0)>>8,rmi.EAX&0x000f);
//   }
   //return (UCHAR)(rmi.EDX&0xff);
  //else return 0;
// * this function always returns the true version number, unlike AH=30h,
//   whose return value may be changed with SETVER
// * because of the conflict from the CBIS redirector (see next
//   entry), programs should check whether BH is less than 100 (64h)
//   and BL is at least 5 before accepting the returned BX as the true
//   version number; however, even this is not entirely reliable when
//   that redirector is loaded
// * fully reentrant
// * OS/2 v2.1 will return BX=0A14h (version 20.10)
// * the Windows NT DOS box returns BX=3205h (version 5.50)
// * Novell DOS 7 returns IBM v6.00, which some software displays as
//   IBM DOS v6.10 (because of the version mismatch in true IBM DOS
//   mentioned for INT 21/AH=30h); versions through Update 15 all
//   return revision code 00h
// * Windows95 and Windows95 SP1 return version 7.00; Windows95 OSR2
//   returns version 7.10
//BUG: DR DOS 5.0 and 6.0 return CF set/AX=0001h for INT 21/AH=33h
//     subfunctions other than 00h-02h and 05h, while MS-DOS returns AL=FFh
//     for invalid subfunctions
//SeeAlso: AH=30h,INT 2F/AX=122Fh,INT 2F/AX=E000h"SETDRVER"
  int handle;
  char Str[128],*Env;
  if((Env=getenv("SYSTEMROOT"))!=0) strcpy(Str,Env);
  else                              return FALSE;
  strcat(Str,"\\system32\\config\\sam");
  if((handle=sopen(Str,O_RDWR|O_BINARY,SH_DENYNO))!=(-1)) //,S_IWRITE
   {
    close(handle);
    return FALSE;
   }
  return TRUE;
 }

// Уничтожить все пpобелы в стpоке
void remove_space_(unsigned char *str)
{
 unsigned char *from=str,*to=str;

 while(*str)
  {
   if(*str!=' ')
    {
     *to=*from;
     if(from!=to) *from=' ';
     to++;
    }
   from++; str++;
  }
 *to=0;
}

profile_data prf_data; // данные из "avp.prf"

unsigned char prf_global_state; // Устанавливается при ошибке

// o_location[2] - две опции в закладке "Location"
// o_location[?]=1  - опция установлена
// o_location[?]=0 - опция не установлена
//unsigned char       o_location[2]; // [0] - "Local hard hisks",
//                                   // [1] - "Network drivers"

char get_str_(char *par,char *in,char **out)
{
 if(strnicmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') *in++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!strlen(in)) return 1;
 if(*out) Free(*out);
// if((*out=strdup(in))==NULL)
 if((*out=(char *)Malloc(strlen(in)+1))==NULL) return -1;
 if(strcpy(*out,in)==NULL) { prg_error=1; /*exit_();*/ return -1; }
 if(!*out) prf_global_state=PERR_MEM;
 return 1;
}

char xget_str_(char *par, char *in, char *out, int lim)
{
 if(strnicmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') *in++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!strlen(in)) return 1;
 if(strlen(in)>lim) return 0;
 strcpy(out,in);
 return 1;
}

char xxget_str_(char *par, char *in, char *out,int lim)
{
 if(strnicmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') *in++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!strlen(in)) return 1;
 if(strlen(in)>lim) return 0;
 while(*in) {if(*in>='0'&&*in<='9') *out++=*in; *in++;}
 *out=0;
 return 1;
}

char get_atoi_(char *par, char *in, char *out, char max)
{
 if(strnicmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') *in++;
 if(*in++!='=') return 0;
 if(*(in+1)||*in<'0'||*in>max+'0') return 1;
 *out=(char)(*in-'0');
 return 1;
}

char get_atoI_(char *par, char *in, LONG *out)
 {
  if(strnicmp(in,par,strlen(par))) return 0;
  in+=strlen(par);
  while(*in==' ') *in++;
  if(*in++!='=') return 0;
  // if(*(in+1)||*in<'0'||*in>max+'0') return 1;
  *out=atoi(in);
  return 1;
 }

char get_bool_(char *par, char *in, char *out)
{
 if(strnicmp(in,par,strlen(par))) return 0;
 in+=strlen(par);
 while(*in==' ') *in++;
 if(*in++!='=') return 0;
 while(*in==' ') in++;
 if(!stricmp(in,"Yes"))
  *out=1;
 else
  if(!stricmp(in,"No")) *out=0;
  else                  prf_global_state=PERR_COR;
 return 1;
}

//void bool_val_(char *z, char val)
//{
// if(val) strcpy(z,"Yes"); else strcpy(z,"No");
//}

//char _exist(char *path)
//{
  //  HDIR FileHandle;
  //  FILEFINDBUF3 FindBuffer;
  //  ULONG FindCount;
//  char rez;

  //  FileHandle=0x0001;
  //  FindCount=1;

//  if(*(path+1)!=':') return 0;
//  if(*(path+strlen(path)-1)==0x5c) *(path+strlen(path)-1)=0;
  //  if(Disk[toupper(*path)-'A']&&
  //   (!*(path+2)||
  //    (DosFindFirst(path,&FileHandle,MUST_HAVE_DIRECTORY,
  //     (PVOID)&FindBuffer,sizeof(FindBuffer),&FindCount,FIL_STANDARD)!=0)&&
  //     FindBuffer.attrFile&0x010)) rez=1;
  //  DosFindClose(FileHandle);

//  if(rez==1)return 1;
//   else return 0;
//}

void setDefaultOptions()
 {
  prf_data.List=NULL;
  prf_data.Path=0;
  prf_data.SubDirectories=1;
  prf_data.Memory=1;
//  prf_data.ScanAllSector=0;
  prf_data.Sectors=1;
  prf_data.Files=1;
  prf_data.ScanRemovable=1;
  prf_data.Packed=1;
  prf_data.Archives=1;
  prf_data.MailBases=0;
  prf_data.MailPlain=0;
  prf_data.FileMask=0;
  *(prf_data.UserMask)=0;
  prf_data.ExcludeFiles=0;
  prf_data.ExcludeDir=NULL;
  strcpy(prf_data.ExcludeMask,"*.txt *.cmd");
  prf_data.InfectedAction=0;
  prf_data.InfectedCopy=0;
  strcpy(prf_data.InfectedFolder,"infected");
  prf_data.SuspiciousCopy=0;
  strcpy(prf_data.SuspiciousFolder,"suspic");
  prf_data.Warnings=1;
  prf_data.CodeAnalyser=1;
  prf_data.RedundantScan=0;
  prf_data.ShowOK=0;
  prf_data.ShowPack=1;
  prf_data.Sound=1;
  prf_data.Report=0;
  strcpy(prf_data.ReportFileName,"report.txt");
  prf_data.ExtReport=1;
  prf_data.RepForEachDisk=0;
  prf_data.LongStrings=0;
  prf_data.Append=0;
  *(prf_data.ReportFileSize)=0;
  prf_data.ReportFileLimit=0;
  prf_data.OtherMessages=1;
  prf_data.OpenHandles=0;
  prf_data.Color=0;
 }

ULONG SearchFile(char* sFileName,char* sDestName,char* sExt)
 {
  //нет точки или последняя точка раньше '\'(т.е. в директории)
  if(strlen(sExt)>0)
   if(!strchr(sFileName,'.') || (strrchr(sFileName,'.')<strrchr(sFileName,'\\')) )
     strcat(sFileName,sExt);

  if(access(sFileName,0)==-1)
   {
    strcpy(sDestName,prgPath);
    strcat(sDestName,sFileName);
    if(access(sDestName,0)==-1)
     {
      strcpy(sDestName,startPath);
      strcat(sDestName,sFileName);
      if(access(sDestName,0)==-1) strcpy(sDestName,sFileName);
     }
   }
  else
   strcpy(sDestName,sFileName);

  return 0;
 }

char  DefProfile[0x50],LocFile[0x50],KeyFile[0x50],KeysPath[CCHMAXPATH],
      SetFile[0x50],BasePath[CCHMAXPATH];

//#include <stdio.h>
ULONG load_ini(char *name)
 {
  int   fil;
  char  dname[CCHMAXPATH],*buf=NULL,*next;
  unsigned len,realread;
  char  section;
  char  sectstr[]={"AVP32]\0Configuration]\0"};
  char  secofs[] ={0,7};

  strcpy(DefProfile,"defdos32.prf");
  strcpy(LocFile,"AVP_LOC.DLL");
  strcpy(KeyFile,"AVPDOS32.KEY");
  strcpy(KeysPath,"");
  strcpy(SetFile,"AVP.SET");
  strcpy(BasePath,"");

  SearchFile(name,dname,".ini");

  if(_dos_open(dname,O_RDONLY|O_TEXT,&fil)) return PERR_OPN;
  len=filelength(fil);
  if((buf=(char *)Malloc(len))==NULL) return PERR_MEM;
  if(_dos_read(fil,(void *)buf,len,&realread))
   {
    close(fil);
    Free(buf);
    return PERR_DSK;
   }
  if(close(fil))
   {
    Free(buf);
    return PERR_DSK;
   }

  // ok, now parse file in mem
  next=strtok(buf,"\r\n");
  while(next)
   {
    //if(*next==0x0a) next++;
    while(*next==' ') next++;
    if(*next=='[')
     {
      next++;
      int fl;
      section=0xff;
      for(fl=0;fl<2;fl++)
       if(!stricmp(next,sectstr+secofs[fl]))
        {section =(char)fl; break;}
     }
    else
     switch(section)
     {
      case 0:
        if(xget_str_("DefaultProfile",next,DefProfile,CCHMAXPATH-1)) break;
        if(xget_str_("LocFile",next,LocFile,CCHMAXPATH-1)) break;
           //AVP_LOC.DLL
              break;
      case 1:
        if(xget_str_("KeyFile",next,KeyFile,CCHMAXPATH-1)) break;
        if(xget_str_("KeysPath",next,KeysPath,CCHMAXPATH-1)) break;
           //Avp.key
        if(xget_str_("SetFile",next,SetFile,CCHMAXPATH-1)) break;
           //AVP.SET
        if(xget_str_("BasePath",next,BasePath,CCHMAXPATH-1)) break;
           //base
              break;
     } // switch (section)
    next=strtok(0,"\r\n");
    //else next = 0;
   }
  Free(buf);

  return 0;
 }

BOOL TestPresentDisk(UCHAR disk,char *typePath)
 {
  //if((LogicalDriveMap&mask)&&((i > 1) || (i < (int)cFloppyDrives)))
  unsigned totaldrives;
  if((disk==0)||(disk==1)) //a или б
   if(DiskType(typePath)!=0) return TRUE;
   else                      return FALSE;
  else
   {
    _dos_setdrive(disk+1,&totaldrives);
    if(_getdrive()==(disk+1)) return TRUE;
    else                      return FALSE;
   }
 }

char sInfectedFolder[CCHMAXPATH],sSuspiciousFolder[CCHMAXPATH];
// Загрузка prf-файла и проверка его на ошибки
char loadProfile(char *name,char *szRep)
 {
  int   fl;
  char  *z0,section = 0xff,*buf,*next;
  unsigned z, zz;
  char  sectstr[]={"Location]\0Objects]\0Actions]\0Options]\0Customize]\0Monitor]\0"};
  char  secofs[] ={0,10,19,28,37,48};
  char  dname[CCHMAXPATH];
  PLIST pList;
  CHAR  typePath[]=" :\\";

  if(pTail!=NULL)
   {
    do
     {
      pList=pTail;
      pTail=pTail->pNext;
      Free(pList);
     } while(pTail != NULL);
   }

  unsigned ulCurDisk,totaldrives;
  BYTE cFloppyDrives,disk;
  // Check the number of floppy drives
  cFloppyDrives=GetNumberFloppyDisks();

  ulCurDisk=_getdrive();
  for(disk=0;disk<27;disk++)
   {
    typePath[0]=(char)('a'+disk);
    if(TestPresentDisk(disk,typePath)) AddRecord(typePath,0);
   }
  _dos_setdrive(ulCurDisk,&totaldrives);

  SearchFile(name,dname,".prf");
  prf_global_state=0;
  // load bastard

  //unsigned bytes;
  //unsigned char *cbuf;

  if(_dos_open(dname,O_RDONLY|O_TEXT,&fl)) return PERR_OPN;
  if(szRep!=NULL)
   {
    USHORT date,time;
    _dos_getftime(fl,&date,&time);
    _sprintf(&szRep[strlen(szRep)]," (from %02d-%02d-%02d %02d:%02d:%02d)",
      YEAR(date),MONTH(date),DAY(date),HOUR(time),MINUTE(time),SECOND(time) );
   }

  z=filelength(fl);
  buf=(char *)Malloc(z);
  if(!buf) return PERR_MEM;
  if(_dos_read(fl,(void *)buf,z,&zz))
   {
    close(fl);
    Free(buf);
    return PERR_DSK;
   }
  if(close(fl))
   {
    Free(buf);
    return PERR_DSK;
   }
  // ok, now parse file in mem
  next=strtok(buf,"\r\n");
  while(next)
   {
    while(*next==' ') next++;
    if(*next=='[')
     {
      next++;
      int fl;
      for(fl=0;fl<6;fl++)
       if(!stricmp(next,sectstr+secofs[fl]))
        {section =(char)fl; break;}
     }
    else
     switch(section)
      {
       case 0:if(get_str_("List",next,&(prf_data.List))) break;
//              if(get_bool_("Path",next,&(prf_data.Path))) break;
              get_bool_("SubDirectories",next,&(prf_data.SubDirectories));
              break;
       case 1:if(get_bool_("Memory",next,&(prf_data.Memory))) break;
              if(get_bool_("Sectors",next,&(prf_data.Sectors))) break;
//              if(get_bool_("ScanAllSector",next,&(prf_data.ScanAllSector))) break;
              if(get_bool_("Files",next,&(prf_data.Files))) break;
              if(get_bool_("Packed",next,&(prf_data.Packed))) break;
              if(get_bool_("Archives",next,&(prf_data.Archives))) break;
              if(get_bool_("MailBases",next,&(prf_data.MailBases))) break;
              if(get_bool_("MailPlain",next,&(prf_data.MailPlain))) break;
              if(get_atoi_("FileMask",next,&(prf_data.FileMask),3)) break;
              if(xget_str_("UserMask",next,prf_data.UserMask,80)) break;
              if(get_bool_("ExcludeFiles",next,&(prf_data.ExcludeFiles))) break;
              if(xget_str_("ExcludeMask",next,prf_data.ExcludeMask,80)) break;
              if(get_str_("ExcludeDir",next,&(prf_data.ExcludeDir))) break;
              break;
       case 2:if(xget_str_("InfectedFolder",next,prf_data.InfectedFolder,CCHMAXPATH-1)) break;
              if(xget_str_("SuspiciousFolder",next,prf_data.SuspiciousFolder,CCHMAXPATH-1)) break;
              if(get_atoi_("InfectedAction",next,&(prf_data.InfectedAction),3)) break;
              if(get_bool_("InfectedCopy",next,&(prf_data.InfectedCopy))) break;
              get_bool_("SuspiciousCopy",next,&(prf_data.SuspiciousCopy));
              break;
       case 3:if(xxget_str_("ReportFileSize",next,prf_data.ReportFileSize,7)) break;
              if(get_bool_("Warnings",next,&(prf_data.Warnings))) break;
              if(get_bool_("CodeAnalyser",next,&(prf_data.CodeAnalyser))) break;
              if(get_bool_("RedundantScan",next,&(prf_data.RedundantScan))) break;
              if(get_bool_("ShowOK",next,&(prf_data.ShowOK))) break;
              if(get_bool_("ShowPack",next,&(prf_data.ShowPack))) break;
              if(get_bool_("Sound",next,&(prf_data.Sound))) break;
              if(get_bool_("Report",next,&(prf_data.Report))) break;
              if(xget_str_("ReportFileName",next,prf_data.ReportFileName,CCHMAXPATH-1)) break;
              if(get_bool_("ExtReport",next,&(prf_data.ExtReport))) break;
              if(get_bool_("RepForEachDisk",next,&(prf_data.RepForEachDisk))) break;
              if(get_bool_("LongStrings",next,&(prf_data.LongStrings))) break;
              if(get_bool_("Append",next,&(prf_data.Append))) break;
              if(get_bool_("ReportFileLimit",next,&(prf_data.ReportFileLimit))) break;
              break;
       case 4:
//              if(get_bool_("Sound",next,&(prf_data.Sound))) break;
              if(get_bool_("OtherMessages",next,&(prf_data.OtherMessages))) break;
              if(get_atoI_("OpenHandles",next,(long*)&(prf_data.OpenHandles))) break;
       case 5:
              if(get_bool_("Color",next,&(prf_data.Color))) break;
              break;
      } // switch (section)
    if(prf_global_state != PERR_MEM) next=strtok(0,"\r\n"); else next=0;
   }
  Free(buf);
  if(CheckNT()) prf_data.Sectors=0;

  if(prf_data.List!=NULL)
   {
    z0=strtok(prf_data.List,"; ");
    while(z0)
     {
//      if((strlen(z0)>3)&&(z0[strlen(z0)-1]=='\\')) z0[strlen(z0)-1]=0;
//   if(z0[2]==0&&z0[1]==':')
//    {
//     if(toupper(*z0)>='A'&&toupper(*z0)<='Z'&&_exist(z0))
//      {
//      }
//    }
      if((z0[0]=='.')||(z0[0]=='*'))
       {
        if((strlen(z0)>4)&&(z0[strlen(z0)-1]=='\\')) z0[strlen(z0)-1]=0;
        AddRecord(&z0[1],1);
       }
      z0=strtok(0,";");
     }
    Free(prf_data.List);
    prf_data.List=0;
   }

  if(prf_data.ExcludeDir!=NULL)
   {
    z0=strtok(prf_data.ExcludeDir,"; ,*");
    while(z0)
     {
      if((strlen(z0)>3)&&(z0[strlen(z0)-1]=='\\')) z0[strlen(z0)-1]=0;
      AddRecord(z0,2);
      z0=strtok(0,"; ,*");
     }
    Free(prf_data.ExcludeDir);
    prf_data.ExcludeDir=0;
   }

  remove_space_((unsigned char*)prf_data.UserMask);
  //o_location[0]=IsSelectAllLocHDD();
  //o_location[1]=IsSelectAllNetWork();
//  if(!avpKeyFlag||avpKeyFlag&&!(avpKey->Options&KEY_O_DISINFECT))
#ifndef _INTERN_VER_
  if(!(functionality & FN_DISINFECT))
    if(prf_data.InfectedAction==1||prf_data.InfectedAction==2)
     prf_data.InfectedAction=0;
#endif

  if(strlen(prf_data.InfectedFolder)>0)
   {
    if(prf_data.InfectedFolder[1]!=':')
     {
      if(prgPath[strlen(prgPath)-1]=='\\')
       _sprintf(sInfectedFolder,"%s%s",prgPath,prf_data.InfectedFolder);
      else
       _sprintf(sInfectedFolder,"%s\\%s",prgPath,prf_data.InfectedFolder);
     }
    else
     strcpy(sInfectedFolder,prf_data.InfectedFolder);
   }
  else
   sInfectedFolder[0]=0;
  if(strlen(prf_data.SuspiciousFolder)>0)
   {
    if(prf_data.SuspiciousFolder[1]!=':')
     {
      if(prgPath[strlen(prgPath)-1]=='\\')
       _sprintf(sSuspiciousFolder,"%s%s",prgPath,prf_data.SuspiciousFolder);
      else
       _sprintf(sSuspiciousFolder,"%s\\%s",prgPath,prf_data.SuspiciousFolder);
     }
    else
     strcpy(sSuspiciousFolder,prf_data.SuspiciousFolder);
   }
  else
   sSuspiciousFolder[0]=0;

  return prf_global_state;
 }

