#include "memalloc.h"
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <ctype.h>

#include <os2.h>
//typedef long LONG;
//typedef unsigned long ULONG;
//typedef unsigned short USHORT;
//typedef short SHORT;
//#define VOID void
//#define CHAR char
//#define PCHAR char*
//#define BOOL unsigned char
//#define BYTE  unsigned char
//#define CCHMAXPATH 512
//#define MRESULT ULONG
//#define MPARAM ULONG

//#include "avp_os2.h"
#include "avp_rc.h"
#include "profile.h"
#include "disk.h"
#include "start.h"
#include "..\\locale\\resource.h"
#include "..\\typedef.h"
#include "../../Common.Fil/stuff/_CARRAY.H"

#include "LoadKeys.h"
extern ULONG functionality;

#include "tv32def.h"
#define Uses_TView
#define Uses_TScrollBar
#define Uses_TRect
#include "include\tv.h"
#define __TListViews
#include "classes.h"
#include "contain.h"
extern TListViews    *list;

void getPrf(void);

BOOL CheckNT(void)
 {
  int handle;
  char Str[128],*Env;
  if((Env=getenv("SYSTEMROOT"))!=0)
   strcpy(Str,Env);
  else
   return FALSE;
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

//char get_atoI_(char *par, char *in, LONG *out)
//{
// if(strnicmp(in,par,strlen(par))) return 0;
// in+=strlen(par);
// while(*in==' ') *in++;
// if(*in++!='=') return 0;
  // if(*(in+1)||*in<'0'||*in>max+'0') return 1;
// *out=atoi(in);
// return 1;
//}

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
  if(!stricmp(in,"No"))
   *out=0;
  else
   prf_global_state=PERR_COR;
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

void set_default_options_()
 {
  prf_data.List=NULL;
  prf_data.Path=0;
  prf_data.SubDirectories=1;
  prf_data.Memory=1;
  prf_data.Sectors=1;
  prf_data.ScanAllSectors=0;
  prf_data.ScanRemovable=1;
  prf_data.Files=1;
  prf_data.Packed=1;
  prf_data.Archives=1;
  prf_data.MailBases=0;
  prf_data.MailPlain=0;
  prf_data.FileMask=0;
  *(prf_data.UserMask)=0;
  prf_data.ExcludeFiles=1;
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
  prf_data.ExtReport=0;
  prf_data.Append=0;
  *(prf_data.ReportFileSize)=0;
  prf_data.ReportFileLimit=0;
  prf_data.OtherMessages=1;
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
    strcpy(sDestName,prg_path);
    strcat(sDestName,sFileName);
    if(access(sDestName,0)==-1)
     {
      strcpy(sDestName,start_path);
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

ULONG load_ini(char *name)
 {
  int   fil;
  char  dname[CCHMAXPATH];
  char  *buf,*next;
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

  strcpy(dname,name);
  if(access(name,0)==-1)
   {
    strcpy(dname,prg_path);
    strcat(dname,name);
    if(access(dname,0)==-1)
     {
      if(!strchr(name,'.')||strrchr(name,'.')<strrchr(name,0x5c))
       {
        strcat(name,".ini");
        if(access(name,0)==-1)
         {
          strcpy(dname,prg_path);
          strcat(dname,name);
         }
        else
         strcpy(dname,name);
       }
     }
   }
  else
   strcpy(dname,name);

  if(_dos_open(dname,O_RDONLY|O_BINARY,&fil)) return PERR_OPN;
  len=filelength(fil);
  buf=(char *)Malloc(len);
  if(!buf) return PERR_MEM;
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
  next=strtok(buf,"\r");
  while(next)
   {
    if(*next==0x0a) next++;
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
    next=strtok(0,"\r");
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
   if(DiskType(typePath)!=0)
     return TRUE;
    else
     return FALSE;
  else
   {
    _dos_setdrive(disk+1,&totaldrives);
    if(_getdrive()==(disk+1))
     return TRUE;
    else
     return FALSE;
   }
 }

char* bool_val(char *rezstr, char val)
 {
  if(val) strcpy(rezstr,"Yes"); else strcpy(rezstr,"No");
  return rezstr;
 }

char *_strtok(char *s,char** nextStr,const char *delim)
 {
  char *spanp; //register
  int  c,sc;   //register
  char *tok;
  static char *last;

  if(s==NULL && (s=last)==NULL)
   {
    *nextStr=NULL;
    return NULL;
   }
  //пропустить (span) лидирующие разделители (s += strspn(s, delim), sort of).
cont:
  c= *s++;
  for(spanp=(char*)delim;(sc= *spanp++)!=0;)
   {
    if(c==sc) goto cont;
   }

  //строка пустая
  if(c==0)
   {           /* no non-delimiter characters */
    last=NULL;
    *nextStr=NULL;
    return NULL;
   }
  tok=s-1;

  // Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
  // Note that delim must have one NUL; we stop if we see that, too.
  for(;;)
   {
    c = *s++;
    spanp = (char *)delim;
    do
     {
      if((sc = *spanp++) == c)
       {
        if(c==0) s=NULL;
        else     s[-1] = 0;
        last = s;
        *nextStr=last;
        return(tok);
       }
     } while(sc != 0);
   }
  /* NOTREACHED */
 }

ULONG WriteParam(HFILE hFile,char* Section,char* Param,char* value)
 {
  char  *buf,*next,*RestStr=NULL;
  ULONG filelen,RealRdWr;

  filelen=filelength(hFile);
  buf=(char*)Malloc(filelen+1);
  if(!buf) return PERR_MEM;
  lseek(hFile,0,SEEK_SET);

  if(_dos_read(hFile,(PVOID)buf,filelen,(unsigned*)&RealRdWr)!=0)
   {
    close(hFile);
    Free(buf);
    return PERR_DSK;
   }
  buf[filelen]=0;
  lseek(hFile,0,SEEK_SET);
  chsize(hFile,0); // обрезаем в нуль

  next=_strtok(buf,(char**)&RestStr,"\r");
  if((RestStr!=0)&&(*RestStr=='\n')) RestStr++;
  while(next)
   {
    if(*next=='\n') next++;
    if(!strnicmp(next,Section,strlen(Section))) // ==0 эквивалентны
     {
      while(next!=0)//&&(next[0]!='['))         // разбор раздела
       {
        if(*next=='\n') next++;
        if(!strnicmp(next,Param,strlen(Param))) // поиск рараметра
         {
          _dos_write(hFile,(PVOID)Param,strlen(Param),(unsigned*)&RealRdWr);
          _dos_write(hFile,(PVOID)value,strlen(value),(unsigned*)&RealRdWr);
          //запись оставшегося буфера
          if(RestStr!=0)
           {
            _dos_write(hFile,(PVOID)"\r\n",2,(unsigned*)&RealRdWr);
            _dos_write(hFile,(PVOID)RestStr,strlen(RestStr),(unsigned*)&RealRdWr);
           }
          return 0;
         }
        _dos_write(hFile,(PVOID)next,strlen(next),(unsigned*)&RealRdWr);
        _dos_write(hFile,(PVOID)"\r\n",2,(unsigned*)&RealRdWr);
        //if(prf_global_state != PERR_MEM)
         next=_strtok(0,(char**)&RestStr,"\r\n");
         if((RestStr!=0)&&(*RestStr=='\n')) RestStr++;
        //else next = 0;
       }
      _dos_write(hFile,(PVOID)Param,strlen(Param),(unsigned*)&RealRdWr); // параметра не нашли
      _dos_write(hFile,(PVOID)value,strlen(value),(unsigned*)&RealRdWr);
      //запись оставшегося буфера
      if(RestStr!=0)
       {
        _dos_write(hFile,(PVOID)"\r\n",2,(unsigned*)&RealRdWr);
        _dos_write(hFile,(PVOID)RestStr,strlen(RestStr),(unsigned*)&RealRdWr);
       }
      return 0;
     }
    _dos_write(hFile,(PVOID)next,strlen(next),(unsigned*)&RealRdWr);
    _dos_write(hFile,(PVOID)"\r\n",2,(unsigned*)&RealRdWr);
    //if(prf_global_state != PERR_MEM)
     next=_strtok(0,(char**)&RestStr,"\r");
    if((RestStr!=0)&&(*RestStr=='\n')) RestStr++;
    //else next = 0;
   }

  if(filelen!=0) _dos_write(hFile,(PVOID)"\r\n",2,(unsigned*)&RealRdWr);
  _dos_write(hFile,(PVOID)Section,strlen(Section),(unsigned*)&RealRdWr);
  _dos_write(hFile,(PVOID)"\r\n",2,(unsigned*)&RealRdWr);
  _dos_write(hFile,(PVOID)Param,strlen(Param),(unsigned*)&RealRdWr);
  _dos_write(hFile,(PVOID)value,strlen(value),(unsigned*)&RealRdWr);

  Free(buf);
  return 0;
 }

char save_profile_(char *name)
 {
  int hFile;
  unsigned RealRdWr,listLen,exclListLen;
  PLIST pList;

  getPrf();

  char dname[CCHMAXPATH];
  strcpy((char*)dname,name);
  if(!strrchr((char*)dname,'.')||(strrchr((char*)dname,'.')<strrchr((char*)dname,0x5c)))
   strcat((char*)dname,".prf");

  if((hFile=sopen(dname,O_RDWR|O_BINARY|O_CREAT,SH_DENYNO,S_IREAD|S_IWRITE))==(-1)) return PERR_OPN;

  char buff[10],*listBuf,*ExclListBuf;

  listLen=0;
  exclListLen=0;
  if(list->pTail!=NULL)
   {
    pList=list->pTail;
    do
     {
      if((pList->Selected==0)||(pList->Selected==1))
        listLen+=strlen(pList->szName)+2;
      else
        if(pList->Selected==2)
          exclListLen+=strlen(pList->szName)+2;
      pList=pList->pNext;
     } while(pList != NULL);
   }
  listBuf=(char*)Malloc(listLen+1);
  listBuf[0]=0;
  ExclListBuf=(char*)Malloc(exclListLen+1);
  ExclListBuf[0]=0;
  if(list->pTail!=NULL)
   {
    pList=list->pTail;
    do
     {
      if(( (strlen(pList->szName)==2) ||( (strlen(pList->szName)==3)&&(pList->szName[2]=='\\')) )
        && (pList->szName[1]==':')
        && (tolower(pList->szName[0])>='a') && (tolower(pList->szName[0])<='z')
        && (pList->Selected==0)) ;
      else
       if((pList->Selected==0)||(pList->Selected==1))
        {
         if(pList->Selected==1) strcat(listBuf,".");
         strcat(listBuf,pList->szName);
         if(pList->pNext != NULL) strcat(listBuf,";");
        }
       else
        if(pList->Selected==2)
         {
          strcat(ExclListBuf,pList->szName);
          if(pList->pNext != NULL) strcat(ExclListBuf,";");
         }
      pList=pList->pNext;
     } while(pList != NULL);
   }
  WriteParam(hFile,"[Location]","List=",listBuf);
  Free(listBuf);
//  WriteParam(hFile,"[Location]","Path=",bool_val((char*)buff,prf_data.Path));
  WriteParam(hFile,"[Location]","SubDirectories=",bool_val((char*)buff,prf_data.SubDirectories));

  WriteParam(hFile,"[Objects]","Memory=",bool_val((char*)buff,prf_data.Memory));
  WriteParam(hFile,"[Objects]","Sectors=",bool_val((char*)buff,prf_data.Sectors));
  WriteParam(hFile,"[Objects]","ScanAllSectors=",bool_val((char*)buff,prf_data.ScanAllSectors));
  WriteParam(hFile,"[Objects]","Files=",bool_val((char*)buff,prf_data.Files));
  WriteParam(hFile,"[Objects]","Packed=",bool_val((char*)buff,prf_data.Packed));
  WriteParam(hFile,"[Objects]","Archives=",bool_val((char*)buff,prf_data.Archives));
  WriteParam(hFile,"[Objects]","MailBases=",bool_val((char*)buff,prf_data.MailBases));
  WriteParam(hFile,"[Objects]","MailPlain=",bool_val((char*)buff,prf_data.MailPlain));
  WriteParam(hFile,"[Objects]","FileMask=",itoa(prf_data.FileMask,(char*)buff,10));
//  WinQueryWindowText(hWndNB2EF,81,prf_data.UserMask);
  WriteParam(hFile,"[Objects]","UserMask=",prf_data.UserMask);
  WriteParam(hFile,"[Objects]","ExcludeFiles=",bool_val((char*)buff,prf_data.ExcludeFiles));
  WriteParam(hFile,"[Objects]","ExcludeMask=",prf_data.ExcludeMask);
  WriteParam(hFile,"[Objects]","ExcludeDir=",ExclListBuf);
  Free(ExclListBuf);

  WriteParam(hFile,"[Actions]","InfectedAction=",itoa(prf_data.InfectedAction,(char*)buff,10));
  WriteParam(hFile,"[Actions]","InfectedCopy=",bool_val((char*)buff,prf_data.InfectedCopy));
//  WinQueryWindowText(hWndNB3EF1,CCHMAXPATH,prf_data.InfectedFolder);
  WriteParam(hFile,"[Actions]","InfectedFolder=",prf_data.InfectedFolder);
  WriteParam(hFile,"[Actions]","SuspiciousCopy=",bool_val((char*)buff,prf_data.SuspiciousCopy));
//  WinQueryWindowText(hWndNB3EF2,CCHMAXPATH,prf_data.SuspiciousFolder);
  WriteParam(hFile,"[Actions]","SuspiciousFolder=",prf_data.SuspiciousFolder);

  WriteParam(hFile,"[Options]","Warnings=",bool_val((char*)buff,prf_data.Warnings));
  WriteParam(hFile,"[Options]","CodeAnalyser=",bool_val((char*)buff,prf_data.CodeAnalyser));
  WriteParam(hFile,"[Options]","RedundantScan=",bool_val((char*)buff,prf_data.RedundantScan));
  WriteParam(hFile,"[Options]","ShowOK=",bool_val((char*)buff,prf_data.ShowOK));
  WriteParam(hFile,"[Options]","ShowPack=",bool_val((char*)buff,prf_data.ShowPack));
  WriteParam(hFile,"[Options]","Sound=",bool_val((char*)buff,prf_data.Sound));
//  WinQueryWindowText(hWndNB4EF1,CCHMAXPATH,prf_data.ReportFileName);
  WriteParam(hFile,"[Options]","ReportFileName=",prf_data.ReportFileName);
  WriteParam(hFile,"[Options]","Append=",bool_val((char*)buff,prf_data.Append));
  WriteParam(hFile,"[Options]","ReportFileLimit=",bool_val((char*)buff,prf_data.ReportFileLimit));
//  WinQueryWindowText(hWndNB4EF2,8,prf_data.ReportFileSize);
  WriteParam(hFile,"[Options]","ReportFileSize=",prf_data.ReportFileSize);
  WriteParam(hFile,"[Options]","Report=",bool_val((char*)buff,prf_data.Report));

  WriteParam(hFile,"[Monitor]","Color=",bool_val((char*)buff,prf_data.Color));

  _dos_write(hFile,(void *)"\r\n",2,&RealRdWr);
  if(close(hFile)) return PERR_DSK;
  return 0;
 }

// Загрузка prf-файла и проверка его на ошибки
char loadProfile(char *name)
{
 int   fl;
 char  *z0,section = 0xff,*buf,*next;
 unsigned z, zz;
 char  sectstr[]={"Location]\0Objects]\0Actions]\0Options]\0Customize\0Monitor]\0"};
 char  secofs[] ={0,10,19,28,37,47};
 char  dname[CCHMAXPATH];
 PLIST pList;
 CHAR  typePath[]=" :\\";

 if(list->pTail!=NULL)
  {
   do
    {
     pList=list->pTail;
     list->pTail=list->pTail->pNext;
     Free(pList);
    } while(list->pTail != NULL);
  }

  unsigned ulCurDisk,totaldrives;
  BYTE cFloppyDrives,disk;
  // Check the number of floppy drives
  cFloppyDrives=GetNumberFloppyDisks();

  ulCurDisk=_getdrive();
  for(disk=0;disk<27;disk++)
   {
    typePath[0]=(char)('a'+disk);
    if(TestPresentDisk(disk,typePath)) AddRecord(typePath,0,list);
   }
  _dos_setdrive(ulCurDisk,&totaldrives);

  strcpy(dname,name);
  if(access(name,0)==-1)
   {
    strcpy(dname,prg_path);
    strcat(dname,name);
    if(access(dname,0)==-1)
     {
      if(!strchr(name,'.')||strrchr(name,'.')<strrchr(name,0x5c))
       {
        strcat(name,".prf");
        if(access(name,0)==-1)
         {
          strcpy(dname,prg_path);
          strcat(dname,name);
         }
        else
         strcpy(dname,name);
       }
     }
   }
  else
   strcpy(dname,name);
  prf_global_state=0;
  // load bastard

  //unsigned bytes;
  //unsigned char *cbuf;

  if(_dos_open(dname,O_RDONLY|O_TEXT,&fl)) return PERR_OPN;
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
//    if(*next==0x0a) next++;
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
      case 0: if(get_str_("List",next,&(prf_data.List))) break;
//              if(get_bool_("Path",next,&(prf_data.Path))) break;
              get_bool_("SubDirectories",next,&(prf_data.SubDirectories));
              break;
      case 1: if(get_bool_("Memory",next,&(prf_data.Memory))) break;
              if(get_bool_("Sectors",next,&(prf_data.Sectors))) break;
              if(get_bool_("ScanAllSectors",next,&(prf_data.ScanAllSectors))) break;
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
      case 2: if(xget_str_("InfectedFolder",next,prf_data.InfectedFolder,CCHMAXPATH-1)) break;
              if(xget_str_("SuspiciousFolder",next,prf_data.SuspiciousFolder,CCHMAXPATH-1)) break;
              if(get_atoi_("InfectedAction",next,&(prf_data.InfectedAction),3)) break;
              if(get_bool_("InfectedCopy",next,&(prf_data.InfectedCopy))) break;
              get_bool_("SuspiciousCopy",next,&(prf_data.SuspiciousCopy));
              break;
      case 3: if(xxget_str_("ReportFileSize",next,prf_data.ReportFileSize,7)) break;
              if(get_bool_("Warnings",next,&(prf_data.Warnings))) break;
              if(get_bool_("CodeAnalyser",next,&(prf_data.CodeAnalyser))) break;
              if(get_bool_("RedundantScan",next,&(prf_data.RedundantScan))) break;
              if(get_bool_("ShowOK",next,&(prf_data.ShowOK))) break;
              if(get_bool_("ShowPack",next,&(prf_data.ShowPack))) break;
              if(get_bool_("Sound",next,&(prf_data.Sound))) break;
              if(get_bool_("Report",next,&(prf_data.Report))) break;
              if(xget_str_("ReportFileName",next,prf_data.ReportFileName,CCHMAXPATH-1)) break;
              if(get_bool_("ExtReport",next,&(prf_data.ExtReport))) break;
              if(get_bool_("Append",next,&(prf_data.Append))) break;
              if(get_bool_("ReportFileLimit",next,&(prf_data.ReportFileLimit))) break;
              break;
      case 4:
//              if(get_bool_("Sound",next,&(prf_data.Sound))) break;
              if(get_bool_("OtherMessages",next,&(prf_data.OtherMessages))) break;
      case 5:
              if(get_bool_("Color",next,&(prf_data.Color))) break;
              break;
     } // switch (section)
    if(prf_global_state!=PERR_MEM) next=strtok(0,"\r\n"); else next = 0;
   }
  Free(buf);
  if(CheckNT()) prf_data.Sectors=0;

  if(prf_data.List!=NULL)
   {
    z0=strtok(prf_data.List,"; ,");
    while(z0)
     {
//   if(z0[2]==0&&z0[1]==':')
//    {
//     if(toupper(*z0)>='A'&&toupper(*z0)<='Z'&&_exist(z0))
//      {
//      }
//    }
      if((z0[0]=='.')||(z0[0]=='*'))
       {
        if((strlen(z0)>4)&&(z0[strlen(z0)-1]=='\\')) z0[strlen(z0)-1]=0;
        AddRecord(&z0[1],1,list);
       }
      else
       {
        if((strlen(z0)>3)&&(z0[strlen(z0)-1]=='\\')) z0[strlen(z0)-1]=0;
        AddRecord(z0,0,list);
       }
      z0=strtok(0,"; ,");
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
      AddRecord(z0,2,list);
      z0=strtok(0,"; ,*");
     }

    Free(prf_data.ExcludeDir);
    prf_data.ExcludeDir=0;
   }

  remove_space_((unsigned char*)prf_data.UserMask);
  //o_location[0]=IsSelectAllLocHDD();
  //o_location[1]=IsSelectAllNetWork();
  //if(!avp_key_flag||avp_key_flag&&!(avp_key->Options&KEY_O_DISINFECT))
  if(!(functionality & FN_DISINFECT))
   {
    if(prf_data.InfectedAction==1||prf_data.InfectedAction==2)
     prf_data.InfectedAction=0;
   }

  return prf_global_state;
 }

