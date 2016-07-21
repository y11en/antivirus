#include "typedef.h"

#include <machine/types.h> //BSDi

#include <dirent.h>
#include <stdlib.h>//"malloc.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <ncurses.h>
#include "start.h"
#include "profile.h"
#include "kernel.h"
#include "avp_os2.h"
#include "../ScanAPI/Avp_msg.h"
#include "contain.h"
#include "disk.h"
#include "dword.h"
#include "resource.h"
#include "avp_rc.h"
#include "report.h"

#include "../TYPEDEF.H"
#include "../ScanAPI/Scanobj.h"
#include "../ScanAPI/BaseAPI.h"
#include "../../CommonFiles/Stuff/_CARRAY.H"

#include "loadkeys.h"
extern ULONG functionality;

char *strupr(char *str)
 {
  char *strTmp=str;
  while(*strTmp)
   {
    if((*strTmp>='a')&&(*strTmp<='z')) *strTmp+='A'-'a';
    strTmp++;
   }
  return str;
 }

FILE *StdIn=stdin;
int _getchar(void)
 {
  char chTmp,chIn=fgetc(StdIn);
//    for(;!feof(StdIn) && fseek(StdIn,sk,SEEK_SET)!=-1;)	fgetc(StdIn); //if( !=?)
//  while(!feof(StdIn) && (fgets(&chTmp,1,StdIn)!=NULL) );
  while(!feof(StdIn) && ((chTmp=fgetc(StdIn))!='\n')) ;

  return chIn;
//  return fgetc(StdIn);

//int fgetc(FILE *);
//int fgetpos(FILE* fp, fpos_t *__pos);
//FILE* fopen(__const char*, __const char*);
//size_t fread(void*, size_t, size_t, FILE*);
//FILE* freopen(__const char*, __const char*, FILE*);
//long int ftell(FILE* __fp);
 };

int __puts(const char *str)
 {
  int result;
//  _IO_size_t len = strlen (str);
//  _IO_cleanup_region_start ((void (*) __P ((void *)))_IO_funlockfile,_IO_stdout);
//  _IO_flockfile (_IO_stdout);
//  if(_IO_sputn(_IO_stdout,str,len)==len) result=len;
//  else result=EOF;
//  _IO_cleanup_region_end (1);
  return result;
 }

int _puts(char const *str)
 {
//  size_t c=strlen(str);
//  __SUIO uio;
//  __SIOV iov[2];

//  iov[0].iov_base=(void *)s;
//  iov[0].iov_len =c;
//  iov[1].iov_base="\n";
//  iov[1].iov_len =1;
//  uio.uio_resid  =c+1;
//  uio.uio_iov    =&iov[0];
//  uio.uio_iovcnt =2;
//  return (__sfvwrite(stdout,&uio) ? EOF:'\n');
//  int i;

  if(blDaemon==FALSE)
   for(; *str; ++str)
    switch (*str)
     {
      case '\t':
        //for(i=8-(StrPos() % 8);i>0;i--)
        putchar(' ');
        break;
      //case '\n': putch('\r');
      default:   putchar(*str);
     } 
  return TRUE;
 }

#define PVOIDFROMMP(mp) ((VOID*)(mp))
#define LONGFROMMP(mp)  ((ULONG)(mp))

unsigned char BinMBR[512]=
{
 0x33, 0xc0, 0xfa, 0x8e, 0xd0, 0xbc, 0x00, 0x7c, 0x8e, 0xc0, 0x8e, 0xd8, 0xfb, 0x8b, 0xf4, 0xbf,
 0x00, 0x06, 0xb9, 0x00, 0x02, 0xfc, 0xf3, 0xa4, 0xea, 0x1d, 0x06, 0x00, 0x00, 0xb0, 0x04, 0xbe,
 0xbe, 0x07, 0x80, 0x3c, 0x80, 0x74, 0x0c, 0x83, 0xc6, 0x10, 0xfe, 0xc8, 0x75, 0xf4, 0xbe, 0xbd,
 0x06, 0xeb, 0x43, 0x8b, 0xfe, 0x8b, 0x14, 0x8b, 0x4c, 0x02, 0x83, 0xc6, 0x10, 0xfe, 0xc8, 0x74,
 0x0a, 0x80, 0x3c, 0x80, 0x75, 0xf4, 0xbe, 0xbd, 0x06, 0xeb, 0x2b, 0xbd, 0x05, 0x00, 0xbb, 0x00,
 0x7c, 0xb8, 0x01, 0x02, 0xcd, 0x13, 0x73, 0x0c, 0x33, 0xc0, 0xcd, 0x13, 0x4d, 0x75, 0xef, 0xbe,
 0x9e, 0x06, 0xeb, 0x12, 0x81, 0x3e, 0xfe, 0x7d, 0x55, 0xaa, 0x75, 0x07, 0x8b, 0xf7, 0xea, 0x00,
 0x7c, 0x00, 0x00, 0xbe, 0x85, 0x06, 0x2e, 0xac, 0x0a, 0xc0, 0x74, 0x06, 0xb4, 0x0e, 0xcd, 0x10,
 0xeb, 0xf4, 0xfb, 0xeb, 0xfe,  'M',  'i',  's',  's',  'i',  'n',  'g',  ' ', 'o' ,  'p',  'e',
  'r',  'a',  't',  'i',  'n',  'g',  ' ',  's',  'y',  's',  't',  'e',  'm',    0,  'E',  'r',
  'r',  'o',  'r',  ' ',  'l',  'o',  'a',  'd',  'i',  'n',  'g',  ' ',  'o',  'p',  'e',  'r',
  'a',  't',  'i',  'n',  'g',  ' ',  's',  'y',  's',  't',  'e',  'm',    0,  'I',  'n',  'v',
  'a',  'l',  'i',  'd',  ' ',  'p',  'a',  'r',  't',  'i',  't',  'i',  'o',  'n',  ' ',  't',
  'a',  'b',  'l',  'e',    0,  'A',  'u',  't',  'h',  'o',  'r',  ' ',  '-',  ' ',  'S',  'i',
  'e',  'g',  'm',  'a',  'r',  ' ',  'S',  'c',  'h',  'm',  'i',  'd',  't',    0,    0,    0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,0x55,0xAA
};

unsigned char BinBOOT[512]=
{
 0xEB,0x3C,0x90,0x4D,0x53,0x44,0x4F,0x53,0x35,0x2E,0x30,0x00,0x02,0x04,0x01,0x00,
 0x02,0x00,0x02,0xFA,0xFD,0xF8,0x40,0x00,0x26,0x00,0x10,0x00,0x26,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x80,0x00,0x29,0xAC,0x9A,0x72,0x1B,0x4E,0x4F,0x20,0x4E,0x41,
 0x4D,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x31,0x36,0x20,0x20,0x20,0xFA,0x33,
 0xC0,0x8E,0xD0,0xBC,0x00,0x7C,0x16,0x07,0xBB,0x78,0x00,0x36,0xC5,0x37,0x1E,0x56,
 0x16,0x53,0xBF,0x3E,0x7C,0xB9,0x0B,0x00,0xFC,0xF3,0xA4,0x06,0x1F,0xC6,0x45,0xFE,
 0x0F,0x8B,0x0E,0x18,0x7C,0x88,0x4D,0xF9,0x89,0x47,0x02,0xC7,0x07,0x3E,0x7C,0xFB,
 0xCD,0x13,0x72,0x79,0x33,0xC0,0x39,0x06,0x13,0x7C,0x74,0x08,0x8B,0x0E,0x13,0x7C,
 0x89,0x0E,0x20,0x7C,0xA0,0x10,0x7C,0xF7,0x26,0x16,0x7C,0x03,0x06,0x1C,0x7C,0x13,
 0x16,0x1E,0x7C,0x03,0x06,0x0E,0x7C,0x83,0xD2,0x00,0xA3,0x50,0x7C,0x89,0x16,0x52,
 0x7C,0xA3,0x49,0x7C,0x89,0x16,0x4B,0x7C,0xB8,0x20,0x00,0xF7,0x26,0x11,0x7C,0x8B,
 0x1E,0x0B,0x7C,0x03,0xC3,0x48,0xF7,0xF3,0x01,0x06,0x49,0x7C,0x83,0x16,0x4B,0x7C,
 0x00,0xBB,0x00,0x05,0x8B,0x16,0x52,0x7C,0xA1,0x50,0x7C,0xE8,0x92,0x00,0x72,0x1D,
 0xB0,0x01,0xE8,0xAC,0x00,0x72,0x16,0x8B,0xFB,0xB9,0x0B,0x00,0xBE,0xE6,0x7D,0xF3,
 0xA6,0x75,0x0A,0x8D,0x7F,0x20,0xB9,0x0B,0x00,0xF3,0xA6,0x74,0x18,0xBE,0x9E,0x7D,
 0xE8,0x5F,0x00,0x33,0xC0,0xCD,0x16,0x5E,0x1F,0x8F,0x04,0x8F,0x44,0x02,0xCD,0x19,
 0x58,0x58,0x58,0xEB,0xE8,0x8B,0x47,0x1A,0x48,0x48,0x8A,0x1E,0x0D,0x7C,0x32,0xFF,
 0xF7,0xE3,0x03,0x06,0x49,0x7C,0x13,0x16,0x4B,0x7C,0xBB,0x00,0x07,0xB9,0x03,0x00,
 0x50,0x52,0x51,0xE8,0x3A,0x00,0x72,0xD8,0xB0,0x01,0xE8,0x54,0x00,0x59,0x5A,0x58,
 0x72,0xBB,0x05,0x01,0x00,0x83,0xD2,0x00,0x03,0x1E,0x0B,0x7C,0xE2,0xE2,0x8A,0x2E,
 0x15,0x7C,0x8A,0x16,0x24,0x7C,0x8B,0x1E,0x49,0x7C,0xA1,0x4B,0x7C,0xEA,0x00,0x00,
 0x70,0x00,0xAC,0x0A,0xC0,0x74,0x29,0xB4,0x0E,0xBB,0x07,0x00,0xCD,0x10,0xEB,0xF2,
 0x3B,0x16,0x18,0x7C,0x73,0x19,0xF7,0x36,0x18,0x7C,0xFE,0xC2,0x88,0x16,0x4F,0x7C,
 0x33,0xD2,0xF7,0x36,0x1A,0x7C,0x88,0x16,0x25,0x7C,0xA3,0x4D,0x7C,0xF8,0xC3,0xF9,
 0xC3,0xB4,0x02,0x8B,0x16,0x4D,0x7C,0xB1,0x06,0xD2,0xE6,0x0A,0x36,0x4F,0x7C,0x8B,
 0xCA,0x86,0xE9,0x8A,0x16,0x24,0x7C,0x8A,0x36,0x25,0x7C,0xCD,0x13,0xC3,0x0D,0x0A,
 0x4E,0x6F,0x6E,0x2D,0x53,0x79,0x73,0x74,0x65,0x6D,0x20,0x64,0x69,0x73,0x6B,0x20,
 0x6F,0x72,0x20,0x64,0x69,0x73,0x6B,0x20,0x65,0x72,0x72,0x6F,0x72,0x0D,0x0A,0x52,
 0x65,0x70,0x6C,0x61,0x63,0x65,0x20,0x61,0x6E,0x64,0x20,0x70,0x72,0x65,0x73,0x73,
 0x20,0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x20,0x77,0x68,0x65,0x6E,0x20,0x72,0x65,
 0x61,0x64,0x79,0x0D,0x0A,0x00,0x49,0x4F,0x20,0x20,0x20,0x20,0x20,0x20,0x53,0x59,
 0x53,0x4D,0x53,0x44,0x4F,0x53,0x20,0x20,0x20,0x53,0x59,0x53,0x00,0x00,0x55,0xAA
};

#include <stdlib.h>
// *********************************************** 
// * check if name2 is a substr of name1         * 
// * All '\\' changed for '/'                    * 
// * All names are changed to uppercase register * 
// * Return value - 0 name2 is a substr of name1 * 
// *                1 not                        * 
// *               -1 error                      * 
// *********************************************** 
#define MAX_VOLUME 5
int CmpFileNames(char *name1,char *name2)
 {
  char *buf1=(char*)malloc(CCHMAXPATH+1);
  char *buf2=(char*)malloc(CCHMAXPATH+1);
  char *p=NULL;
  int len=0;
  //static char vol[MAX_VOLUME+1],dir[256],
  //              name[CCHMAXPATH+1],ext[CCHMAXPATH+1],dir1[256];

  if((buf1==NULL) || (buf2==NULL))
   {
    len=-1;
    goto _end;
   }
  strcpy(buf1,name1);
  strcpy(buf2,name2);

  p=buf1;
  while(p)
   {
    p=strchr(p,'\\');
    if(p!=NULL) *p++='/';
   }
  p=buf2;
  while(p)
   {
    p=strchr(p,'\\');
    if(p!=NULL) *p++='/';
   }

//  _splitpath(buf1,vol,dir1,name,ext);
//  if(dir1[0]!='/')
//   {
//    dir[0]='/';
//    dir[1]=0x0;
//    strcat(dir,dir1);
//   }
//  else
//   strcpy(dir,dir1);
//  _makepath(buf1,vol,dir,name,ext);

//  _splitpath(buf2,vol,dir1,name,ext);
//  if(dir1[0]!='/')
//   {
//    dir[0]='/';
//    dir[1]=0x0;
//    strcat(dir,dir1);
//   }
//  else
//   strcpy(dir,dir1);
//  _makepath(buf2,vol,dir,name,ext);

  len=strlen(buf1)-1;
  if(*(buf1+len)=='/') *(buf1+len)=0x0;
  len=strlen(buf2)-1;
  if(*(buf2+len)=='/') *(buf2+len)=0x0;

  len=strlen(buf2);
  if(len<=((int)strlen(buf1)))
   {
    p=&buf1[len];
    if(*p=='/') *p=0x0;
    else
     {
      p=strchr(p,'/');
      if(p) *p=0x0;
     }
   }
  else {len=1;goto _end;}

  if(strcasecmp(buf1,buf2)==0) len=0;
  else                      len=1;
_end:
  if(buf1) free(buf1);
  if(buf2) free(buf2);
  return len;
 }

#define  U(c) ((c) & 0377) 
#define  QUOT    '\\'       

int SyInSy(unsigned smin, unsigned smy, unsigned smax)
 {
  char left[2],right[2],middle [2];

  left  [0]=smin; left  [1]='\0';
  right [0]=smax; right [1]='\0';
  middle[0]=smy;  middle[1]='\0';

  return (strcoll(left,middle)<=0 && strcoll(middle,right)<=0);
 }

int Match(register char *teststr,register char *mask)
 {
  register int scc; // current char 
  int c,cc,lc;   //lc previos sumbol in list
  int ok,notflag;

  for(;;)
   {
    scc=U(*teststr++);          /* очередной символ строки  */
    switch (c=U(*mask++))
     { /* очередной символ шаблона */
      case QUOT:          /* a*\*b */
        c=U(*mask++);
        if(c==0) return(0); /* ошибка: pattern\ */
        else goto def;
      case '[':   /* любой символ из списка */
        ok=notflag = 0;
        lc=077777;      /* достаточно большое число */
        if(*mask== '!')
         { notflag=1; mask++; }
        while((cc=U(*mask++))
!=0)
         {
          if(cc==']')
           {    /* конец перечисления */
            if(ok)  break;      /* сопоставилось      */
            return (0);     /* не сопоставилось   */
           }
          if(cc=='-')
           {    /* интервал символов  */
            if(notflag)
             {
              /* не из диапазона - OK */
              if(!SyInSy (lc, scc,U(*mask++)))
 ok++;
              /* из диапазона - неудача */
              else return (0);
             }
            else
             {
              /* символ из диапазона - OK */
              if(SyInSy(lc,scc,U(*mask++)))
 ok++;
             }
           }
          else
           {
            if(cc == QUOT)
             {      /* [\[\]] */
              cc = U(*mask++);
              if(!cc) return(0);/* ошибка */
             }
            if(notflag)
             {
              if(scc && scc!=(lc=cc))
 ok++;          /* не входит в список */
              else return (0);
             }
            else
             {
              if(scc==(lc=cc)) ok++;/* входит в список */
             }
           }
         }
        if(cc == 0)
         {    /* конец строки */
          //printf("None ]\n")

;
          return (0);        /* ошибка */
         }
        continue;
      case '*':   /* любое число любых символов */
        if(!*mask)  return (1);
        for(teststr--;*teststr;teststr++)
          if(Match(teststr,mask))
 return (1);
        return (0);
      case 0:
        return (scc == 0);
      default: def:
        if(c!=scc) return (0);
        continue;
      case '?':   /* один любой символ */
        if(scc==0) return (0);
        continue;
     }
   }
 }

BOOL cmpFileMask(char* sTestFile)
 {
  if((prf_data.ExcludeFiles==0x01)||(prf_data.ExcludeFiles==0x03))
   {
    char s[85],*span;
    strcpy(s,prf_data.ExcludeMask);
    span=strtok((char*)s,";, ");
    while(span)
     {
      //printf("maskCompare %s %s\r\n",sTestFile,span);
//      if(maskCompare(sTestFile,span)==0)
      if(Match(sTestFile,span)
==1)
       {
        //printf("maskCompare True Str: %s, Mask: %s\r\n",sTestFile,span);
        return TRUE;
       }
      //else printf("maskCompare FALSE Str: %s, Mask: %s\r\n",sTestFile,span);
      span=strtok(NULL,";, ");
     }
   }
  return FALSE;
 }

ULONG StrICmp(char *str1,char *str2)
 {//str 2 должна совпадать или быть подстрокай str1 (str2<=str1)
  char tstStr1[CCHMAXPATH+1],tstStr2[CCHMAXPATH+1];
  strcpy((char *)tstStr1,str1);
  strcpy((char *)tstStr2,str2);
  ULONG len;
  len=strlen((char *)tstStr1)-1;
  if(*(tstStr1+len)=='/') *(tstStr1+len)=0x0;
  len=strlen((char *)tstStr2)-1;
  if(*(tstStr2+len)=='/') *(tstStr2+len)=0x0;

  if(strlen((char *)tstStr2)>strlen((char *)tstStr1)) return 1;

  len=0;
  while(tstStr2[len]==tstStr1[len])
   {
    len++;
    if(tstStr2[len]==0) return (0);
   }
  if(len<strlen((char*)tstStr2)) return 2;
  else return 0;
//  return (*(unsigned char *)s1 - *(unsigned char *)--s2);
 }

BOOL cmpDirMask(char* sTestDir)
 {
  if(strlen(sInfectedFolder)>0)
   if(StrICmp(sTestDir,sInfectedFolder)==0) // равна или поддиректория
    {
     return TRUE;
    }
  if(strlen(sSuspiciousFolder)>0)
   if(StrICmp(sTestDir,sSuspiciousFolder)==0)
    {
     return TRUE;
    }

  if((prf_data.ExcludeFiles==0x02)
||(prf_data.ExcludeFiles==0x03)
)
   {
    if(pTail!=NULL)
     {
      PLIST pList=pTail;
      do
       {
        if(pList->Selected==2)
         if(StrICmp(sTestDir,pList->szName)==0)
 return TRUE;
        pList=pList->pNext;
       } while(pList != NULL);
     }
   }
  return FALSE;
 }

BOOL cmpDirMask1(char* sTestDir)
 {
  if(pTail!=NULL)
   {
    PLIST pList=pTail;
    do
     {
      if(pList->Selected==1)
       if(StrICmp(sTestDir,pList->szName)==0)
 return TRUE;
      pList=pList->pNext;
     } while(pList != NULL);
   }
  return FALSE;
 }

_workStat workStat;

void strCpyEnd(char* dst,char* src)
 {
  int i,j;
  for(j=0;j<(STATLEN-6);j++) dst[j]=' ';
  dst[j+1]='\0';
  for(i=strlen(src)-1;(i>=0)&&(j>=0);i--,j--) dst[j]=src[i];
 }

void initWorcStat(void)
 {
  setPrf();

  iniWorkStat();
  strCpyEnd(workStat.Sectors  ,LoadString(IDS_TAB5_STATIC_SECTORS    ,"Sector Objects :"));
  strCpyEnd(workStat.Files    ,LoadString(IDS_TAB5_STATIC_FILES      ,"Files :"));
  strCpyEnd(workStat.Folder   ,LoadString(IDS_TAB5_STATIC_FOLDERS    ,"Folders :"));
  strCpyEnd(workStat.Archives ,LoadString(IDS_TAB5_STATIC_ARCHIVES   ,"Archives :"));
  strCpyEnd(workStat.Packed   ,LoadString(IDS_TAB5_STATIC_PACKED     ,"Packed :"));
  strCpyEnd(workStat.countSize,LoadString(IDS_TAB5_STATIC_SPEED      ,"Speed (Kb/sec) :"));
  strCpyEnd(workStat.scanTime ,LoadString(IDS_TAB5_STATIC_TIME       ,"Scan time :"));
  strCpyEnd(workStat.knownVir ,LoadString(IDS_TAB5_STATIC_VIRUSES    ,"Known viruses :"));
  strCpyEnd(workStat.virBodies,LoadString(IDS_TAB5_STATIC_BODIES     ,"Virus bodies :"));
  strCpyEnd(workStat.Disinfect,LoadString(IDS_TAB5_STATIC_DISINFECTED,"Disinfected :"));
  strCpyEnd(workStat.Deleted  ,LoadString(IDS_TAB5_STATIC_DELETED    ,"Deleted :"));
  strCpyEnd(workStat.Warning  ,LoadString(IDS_TAB5_STATIC_WARNINGS   ,"Warnings :"));
  strCpyEnd(workStat.Suspis   ,LoadString(IDS_TAB5_STATIC_SUSPIC     ,"Suspicious :"));
  strCpyEnd(workStat.Corript  ,LoadString(IDS_TAB5_STATIC_CORRUPTED  ,"Corrupted :"));
  strCpyEnd(workStat.IOError  ,LoadString(IDS_TAB5_STATIC_IOERRORS   ,"I/O Errors :"));
 }

void iniWorkStat(void)
 {
  workStat.sect=0;
  workStat.fil=0;
  workStat.fold=0;
  workStat.arch=0;
  workStat.pack=0;
  workStat.CountSize=0;
  workStat.Tm.tm_hour=workStat.Tm.tm_min=workStat.Tm.tm_sec=0;
  workStat.kv=0;
  workStat.vb=0;
  workStat.df=0;
  workStat.deleted=0;
  workStat.warn=0;
  workStat.susp=0;
  workStat.cor=0;
  workStat.err=0;
 }

void iniWorkStatForEach(void)
 {
  time_t Timer;
  struct tm *Tm;

  Timer =time((time_t*)NULL);
  Tm=gmtime(&Timer);
  workStat.cekForEach=Tm->tm_mday*86400+Tm->tm_hour*3600+Tm->tm_min*60+Tm->tm_sec;
  workStat.sectForEach=0;
  workStat.filForEach=0;
  workStat.foldForEach=0;
  workStat.archForEach=0;
  workStat.packForEach=0;
  workStat.CountSizeForEach=0;
  workStat.TmForEach.tm_hour=workStat.TmForEach.tm_min=workStat.TmForEach.tm_sec=0;
  workStat.kvForEach=0;
  workStat.vbForEach=0;
  workStat.dfForEach=0;
  workStat.deletedForEach=0;
  workStat.warnForEach=0;
  workStat.suspForEach=0;
  workStat.corForEach=0;
  workStat.errForEach=0;
 }

void incWorkStat(ULONG sw)
 {
  switch (sw)
   {
    case INCL_SECTORS:   ++workStat.sect;    ++workStat.sectForEach;    break;
    case INCL_FILES:     ++workStat.fil;     ++workStat.filForEach;     break;
    case INCL_FOLDER:    ++workStat.fold;    ++workStat.foldForEach;    break;
    case INCL_ARCHIVES:  ++workStat.arch;    ++workStat.archForEach;    break;
    case INCL_PACKED:    ++workStat.pack;    ++workStat.packForEach;    break;
    case INCL_KNOWNVIR:  ++workStat.kv;      ++workStat.kvForEach;      break;
    case INCL_VIRBODIES: ++workStat.vb;      ++workStat.vbForEach;      break;
    case INCL_DISINFECT: ++workStat.df;      ++workStat.dfForEach;      break;
    case INCL_DELETED:   ++workStat.deleted; ++workStat.deletedForEach; break;
    case INCL_WARNING:   ++workStat.warn;    ++workStat.warnForEach;    break;
    case INCL_SUSPICION: ++workStat.susp;    ++workStat.suspForEach;    break;
    case INCL_CORRUPTED: ++workStat.cor;     ++workStat.corForEach;     break;
    case INCL_IOERRORS:  ++workStat.err;     ++workStat.errForEach;     break;
   }
 }

CDWordArray VirCountArray;
_SurBase SurBase;

#define DID_OK         1
#define DID_CANCEL     2

#define MB_OK          0x0000
#define MB_OKCANCEL    0x0001
#define MB_YESNO       0x0004
#define MB_OK    0x0000
ULONG MyWaitMessBox(CHAR *Mess,ULONG Param,BYTE skip_dlg)
 {
  ULONG RezMsg=(ULONG)-1;

//  SetCursorOff(TRUE);
  if(blDaemon==FALSE)
   {
    printf("\n%s",(char*)Mess);
    switch (Param)
     {
      case MB_OK:
        printf("\n\n");
        printf(LoadString(IDS_PRESS_KEY_L,"Press any key"));
        break;
      case MB_OKCANCEL:
        printf(" - Ok/Cancel ");
        break;
      case MB_YESNO:
        printf(" - Yes/No ");
        break;
     }
   }     
  if((skip_dlg!=1)
&&(blDaemon==FALSE))
   switch (_getchar())
    {
     case 'O':
     case 'o':
       RezMsg=DID_OK;
       break;
     case 'C':
     case 'c':
       RezMsg=DID_CANCEL;
       break;
     case 'Y':
     case 'y':
       RezMsg=DID_OK_YES;
       break;
     case 'N':
     case 'n':
       RezMsg=DID_CANCEL_NO;
     break;
    }
   else
    {
     switch (Param)
      {
       case MB_YESNO:    RezMsg=DID_OK_YES;break;
       case MB_OK:
       case MB_OKCANCEL:
       default:          RezMsg=DID_OK;    break;
      }
    }
//  SetCursorOff(FALSE);
  if(blDaemon==FALSE) putchar('\n');
  return RezMsg;
 }

int  ExitCode=1;
int  ApplyToAll=0;
int  LastAction;
int  MethodDelete=0;
BOOL SectorWarn=0;

ULONG DisinfectDlgProc(DisinfectDlg* dd);
BOOL SelfScan=FALSE;               // при самосканировании
char scan_goes,                    // продолжать ли процесс сканирования
                                   // 1 - продолжать 0 - не продолжать
     scan_end;                     // показывает как завершился процесс сканирования
                                   // 0 без прерывания/1 прерван пользователем
//     found_name[CCHMAXPATH];          // имя найденного файла

char           ask_cure_all_flag=0;       // флаг apply to all для ask_cure
char           ask_delete_all_flag=0;     // флаг apply to all для ask_delete

//unsigned char *make_name_ptr;  // pointer to _MAKE_NAME data
//unsigned char *send_param_ptr; // pointer to _SEND_PARAM data

#include <time.h>

#ifdef __cplusplus
   extern "C" {
#endif
void *_memcpy(void *__s1, const void *__s2, size_t __n );
void *_memmove( void *__s1, const void *__s2, size_t __n );
char *_strcpy( char *__s1, const char *__s2 );
char *_strncpy( char *__s1, const char *__s2, size_t __n );
time_t _time(time_t *__timer );
void _free( void *__ptr );
void *_malloc( size_t __size );
size_t wcslen( const wchar_t *string );
wchar_t *wcscpy( wchar_t *strDestination, const wchar_t *strSource );
#ifdef __cplusplus
   }
#endif
void *_memcpy(void *__s1, const void *__s2, size_t __n )
 { return memcpy(__s1,__s2,__n); }
void *_memmove( void *__s1, const void *__s2, size_t __n )
 { return memmove(__s1,__s2,__n); }
char *_strcpy( char *__s1, const char *__s2 )
 { return strcpy(__s1,__s2 ); }
char *_strncpy( char *__s1, const char *__s2, size_t __n )
 { return strncpy(__s1,__s2,__n ); }
time_t _time(time_t *__timer )
 { return time(__timer);}
void _free( void *__ptr )
 { free(__ptr); }
void *_malloc( size_t __size)
 { return malloc(__size);}
size_t wcslen(const wchar_t *string)
 {
  string=string;
  return 0;
 }
wchar_t *wcscpy(wchar_t *strDestination, const wchar_t *strSource)
 {
  strDestination=strDestination; strSource=strSource;
  return 0;
 }

#include "../../CommonFiles/AVPVndID.h"
#include "../../CommonFiles/Sign/SIGN.H"
#include "../../CommonFiles/serialize/KLDTSer.h"

static void** vndArray=NULL;
static int vndArrayCount=0;

static void VndDestroy()
 {
  if(vndArray)
   {
    for(int i=0;i<vndArrayCount;i++)
            delete [] vndArray[i];
    delete [] vndArray;
   }
  vndArrayCount=0;
  vndArray=NULL;
 }

static BOOL VndInit(const char* filename)
 {
  BOOL ret=FALSE;
  VndDestroy();
  if(filename==NULL) return FALSE;
  if(sign_check_file(filename,1,0,0,0)) return FALSE;
  if(!KLDT_Init_Library(malloc,free)) return FALSE;
  if(!DATA_Init_Library(malloc,free)) return FALSE;

  HDATA hRootData=NULL;
  if(!KLDT_Deserialize( filename, &hRootData ))return FALSE;
  if(hRootData==NULL) return FALSE;
  if(AVP_PID_VNDROOT!=DATA_Get_Id( hRootData, 0 ))
   {
    DATA_Remove( hRootData, 0 );
    return FALSE;
   }

  MAKE_ADDR1( hKernelAddr, AVP_PID_VNDKERNELDATA );

  // ╧юёЄЁюшЄ№ рфЁхё эр єЁютхэ№ AVP_PID_VNDINFO - AVP_PID_VNDEXPDATE
//  MAKE_ADDR2( hExpDateAddr, AVP_PID_VNDINFO, AVP_PID_VNDEXPDATE );
  // ╧юыєўшЄ№ фрЄє
//  AVP_date rcDate;
//  DATA_Get_Val( hRootData, hExpDateAddr, 0, &rcDate, sizeof(rcDate) );

//  SYSTEMTIME rcSysTime;
//  memset( &rcSysTime, 0, sizeof(rcSysTime) );
//  memcpy( &rcSysTime, &rcDate, sizeof(rcDate) );

//  SYSTEMTIME curSysTime;
//  GetSystemTime(&curSysTime);

//  if(curSysTime.wYear > rcSysTime.wYear) goto dr;
//  if(curSysTime.wYear < rcSysTime.wYear) goto time_ok;
//  if(curSysTime.wMonth > rcSysTime.wMonth) goto dr;
//  if(curSysTime.wMonth < rcSysTime.wMonth) goto time_ok;
//  if(curSysTime.wDay > rcSysTime.wDay) goto dr;
//  if(curSysTime.wDay < rcSysTime.wDay) goto time_ok;
//time_ok:

  HPROP hKernelProp;
  hKernelProp = DATA_Find_Prop( DATA_Find(hRootData, hKernelAddr), NULL, 0 );

  // ╧юыєўшЄ№     ъюышўхёЄтю ¤ыхьхэЄют т ьрёёштх
  AVP_dword nCount;
  nCount = PROP_Arr_Count( hKernelProp );

  vndArray= (void**)new DWORD[nCount];
  if(vndArray==NULL)goto dr;
  vndArrayCount=nCount;

  // ╤ърэшЁютрЄ№ ьрёёшт
  AVP_dword i;
  for(i=0;i<nCount;i++)
   {
    AVP_dword nItemSize = PROP_Arr_Get_Items( hKernelProp, i, 0, 0 );
    AVP_char *pBuffer = new AVP_char[nItemSize];
    if(pBuffer==NULL)break;
    PROP_Arr_Get_Items( hKernelProp, i, pBuffer, nItemSize );
    vndArray[i]=pBuffer;
   }
dr:
  // ╙фрыхэшх фхЁхтр
  DATA_Remove( hRootData, 0 );
  return ret;
 }

BOOL load_avpbase(char* name)
 {
  BOOL ret=FALSE;
  char *curpath,*Folder;//[CCHMAXPATH];

  curpath=getcwd(NULL,0);

  Folder=name;
  name=strrchr(Folder,'/');
  if(name)
   {
    Folder[name-Folder]=0;
    name++;
   }
  else name=Folder;

  chdir(Folder);

  //printf("%s \n%s \n%s\n",name,Folder,getcwd(NULL,0));
  int SetFile;
  if((SetFile=open(name,O_RDONLY,S_IRUSR|S_IRGRP|S_IROTH|S_IWRITE))!=(-1))
   {
    char  *buf=0;
    unsigned z;
    z=lseek(SetFile,0,SEEK_END);
    lseek(SetFile,0,SEEK_SET);
    if((buf=(char *)malloc(z))!=0)
     {
      int KernFile;
      if(read(SetFile,(void *)buf,z)!=-1)
//       if( (strstr(strupr(strtok(buf,"\r\n")),"KERNEL.AV")!=0) &&
       if( (strcasecmp(strtok(buf,"\r\n"),"KERNEL.AV")!=0) &&
         ( (KernFile=open(buf,O_RDONLY,S_IWRITE))!=(-1)) )
        {
         close(KernFile);

         if(name && strlen(name))
          {
           char filename[0x200];
           char* n;
           strcpy(filename,name);
           n=AvpStrrchr(filename,'/');
           if(n) n++;
           else n=filename;
           strcpy(n,"AVP.VND");
           VndInit(filename);
//           printf("number of vnd keys=%d\r\n",vndArrayCount);
          }
         if(BAvpLoadBase(name))
          {
           printf(LoadString(IDS_BASES_LOADED,"Antiviral databases were loaded. Known records: %d\n"),SurBase.RecCount);
           //printf("\n");
           //printf(LoadString(IDS_LAST_UPDATE,"Last update:  %02d.%02d.%04d,  %d records."),SurBase.LastUpdateDay,SurBase.LastUpdateMonth,SurBase.LastUpdateYear,SurBase.RecCount);
           //WinSetWindowText(hWndBTNTxt,loadStr);
           ret=TRUE;
          }
         else
          {
           printf(LoadString(IDS_BASES_NOT_LOADED,"Loading of bases was not completed.\n"));
           printf("\n");
          }

        }
       else _puts("kernel.avc is absent in the first string of avp.set file.\n");
      free(buf);
     }
    close(SetFile);
   }

  if(name!=Folder)
   {
    Folder[(--name)-Folder]='/';
    name=Folder;
   }
  chdir(curpath);
//  Free(curpath);
  return ret;
 }

ULONG GetFlags(void)
{
 ULONG mFlags=0;
//  DWORD MFlags=tPage2.m_Mask;

 if(functionality)
  {
   if(prf_data.Packed   &&(functionality & FN_UNPACK))  mFlags|=MF_PACKED;
   if(prf_data.Archives &&(functionality & FN_EXTRACT)) mFlags|=MF_ARCHIVED;
   if(prf_data.MailBases&&(functionality & FN_MAIL))    mFlags|=MF_MAILBASES;
   if(prf_data.MailPlain&&(functionality & FN_MAIL))    mFlags|=MF_MAILPLAIN;
   if(prf_data.CodeAnalyser&&(functionality & FN_CA))   mFlags|=MF_CA;
  }
 if(prf_data.Warnings)     mFlags|=MF_WARNINGS;
 if(prf_data.RedundantScan)mFlags|=MF_REDUNDANT;
 switch (prf_data.FileMask)
  {
    case 0:
     mFlags|=MF_MASK_SMART;
     break;
    case 1:
     mFlags|=MF_MASK_PROG;
     break;
    case 2:
     mFlags|=MF_MASK_ALL;
     break;
    case 3:
     mFlags|=MF_MASK_USER;
     break;
  }

//MFlags|=MF_ALLWARNINGS;
//MFlags|=MF_ALLENTRY;
 return mFlags;
}

BOOL ScanFile(char *fullFileName,unsigned long fileSize,unsigned long mFlags)
 {
  // -- Check files by list -----
  AVPScanObject ScanObject;

  memset((void*)&ScanObject,0,sizeof(AVPScanObject));
  strcpy(ScanObject.Name,fullFileName);
  ScanObject.SO1.SType=OT_FILE;
  ScanObject.MFlags=mFlags;//MF_MASK_ALL|mFlags;
  ScanObject.Size=fileSize;//cbFile;
  return (BOOL)BAvpProcessObject(&ScanObject);
 }

int TidyPath (char *path);
char* CheckSymlink(char *path,dev_t my_dev);

void ScanTree(char *CurrDir,ULONG mFlags,dev_t dev)
 {
  struct stat fStats;
  char szFileName[CCHMAXPATH+1]; 

  strcpy(szFileName,CurrDir);
  if(strlen(szFileName)>1)
   if(szFileName[strlen(szFileName)-1]=='/') szFileName[strlen(szFileName)-1]=0;
  
  if(lstat(szFileName,&fStats)==-1) return;
  if(S_ISDIR(fStats.st_mode))
   {
//  if(cmpDirMask(szFileName)==TRUE) return;
    DIR *dp;
    //printf("%s\n",szFileName); //dir 

    incWorkStat(INCL_FOLDER);
    //if(!filesys)
    // filesys=fstats.st_dev;
    //else
    // if(filesys!=fstats.st_dev) // stay in file system 
    //   return(0);

    //char *ptr;
    //ptr=fullpath+strlen(fullpath);
    // *ptr=(*(ptr-1)=='/') ? 0 : '/';
    //ptr=(*ptr) ? ptr+1 : ptr;
    // *ptr=0;

    if((dp=opendir(szFileName))!=NULL)
     {
      int pathLen=strlen(szFileName);
      char *dirName=szFileName+pathLen;
      if(*(dirName-1)!='/')  *dirName++='/';
      struct dirent *dirp;
      //printf("open %s\n",szFileName);
      while((dirp=readdir(dp))!=NULL)
       {
        //printf("read %s\n",dirp->d_name);
        if(strcmp(dirp->d_name,".") && strcmp(dirp->d_name,".."))
         {
          strcpy(dirName,dirp->d_name);
          struct stat st;
          if(lstat(szFileName,&st)!=-1)
          //if(st.st_dev==dev)
           {
            //printf("read %s \n",szFileName);
	    if(S_ISLNK(st.st_mode))
	     {
              if(prf_data.Symlinks==2)
	       {
	        char *TargetPath=CheckSymlink(szFileName,fStats.st_dev);//dev);
                //printf("read symlink %s %s\n",szFileName,TargetPath);
	        if(TargetPath!=NULL)
                 if((scan_goes)&&(lstat(TargetPath,&st)!=-1))
  	          if(S_ISDIR(st.st_mode))
		   {
                    if((prf_data.SubDirectories!=0)&&
		     ( (prf_data.CrossFs==1)||((prf_data.CrossFs==0)&&(st.st_dev==dev))) )
		     ScanTree(TargetPath,mFlags,st.st_dev);
		   }
                  else
                   {//printf("read file%s \n",szFileName);
                    if(S_ISREG(st.st_mode) //&& ((!fStats.st_size && verbose>4)|| fStats.st_size)
                     )
	            ScanFile(TargetPath,st.st_size,mFlags);
 	           }
	       } 
	     }  
	    else
             if(scan_goes)
	      if(S_ISDIR(st.st_mode))
	       {
                if((prf_data.SubDirectories!=0)&&
		 ( (prf_data.CrossFs==1)||((prf_data.CrossFs==0)&&(st.st_dev==dev))) )
		 ScanTree(szFileName,mFlags,dev); //strlen(path), dev); 
	       }
              else
               {
                //printf("read file %s \n",szFileName);
                if(S_ISREG(st.st_mode) //&& ((!fStats.st_size && verbose>4)|| fStats.st_size)
                 )
	        ScanFile(szFileName,st.st_size,mFlags);
	       } 
           }
         }
       }	 
      if(closedir(dp)<0) ;//printf("Can't close directory: %s\n",szFileName);
      szFileName[pathLen]='\0';
     }
   }
  return;
 }

int TestObject(char *szFileName)
 {
  if(cmdline.ListFile)
   {
    char *fil;
    fil=strtok((char*)cmdline.ListFile,"\r\n");
    while(fil)
     {   
 
      if(strcmp(fil,szFileName)==0)
       {
        struct stat statbuf;
        lstat(fil,&statbuf); //+ link info
        //if(lstat(szFileName,&fStats)==-1) return;
        if(S_ISREG(statbuf.st_mode) ) return ScanFile(szFileName,statbuf.st_size,GetFlags());
       }	 
      fil=strtok(NULL,"\r\n");
     } 
    return -1;
   }
   
  struct stat st;
  if(lstat(szFileName,&st)==-1) return -2;//printf("no object(%s). LStat()=-1",szFileName);
  //printf("test %s \n",szFileName);
  if(S_ISLNK(st.st_mode))
   {
    //if((prf_data.Symlinks==1)||(prf_data.Symlinks==2))
    if(prf_data.Symlinks==2)
     {                                        
      char *TargetPath=CheckSymlink(szFileName,st.st_dev);//dev);
      //printf("read symlink %s %s\n",szFileName,TargetPath);
      if(TargetPath!=NULL)
       if((scan_goes)&&(lstat(TargetPath,&st)!=-1))
        if(S_ISDIR(st.st_mode))
	 {
          //printf("test dir from symlink %s \n",szFileName);
//        if((prf_data.CrossFs==1)||((prf_data.CrossFs==0)&&(st.st_dev==dev)))
	   ScanTree(TargetPath,GetFlags(),st.st_dev);
	  return 0;
	 }
        else
         {//printf("test file from symlink %s \n",szFileName);
          if(S_ISREG(st.st_mode) )//&& ((!fStats.st_size && verbose>4)|| fStats.st_size)
	    return ScanFile(TargetPath,st.st_size,GetFlags());
 	 }
     } 
   }  
  else
   if(scan_goes && prf_data.Files) //printf("not test file %s",szFileName);
    if(S_ISDIR(st.st_mode))
     {
      //printf("test dir from symlink %s \n",szFileName);
      //char testPath[PATH_MAX+2];
      //if(*FileName=='/') *testPath = '\0';
      //else          strcpy(testPath,start_path);
      //struct stat stScan;
      //TidyPath(strcat(testPath,FileName));
      //if(lstat(testPath,&stScan)==-1) ;//perror(testPath);
      //else
      // {
      //  return ScanTree(testPath,GetFlags(),stScan.st_dev);
 // strlen(testPath), stScan.st_dev); 
      // } 
      ScanTree(szFileName,GetFlags(),st.st_dev); 
      return 5;//0;
     }
    else
     {//printf("Test file %s \n",szFileName);
      if(S_ISREG(st.st_mode) )//&& ((!fStats.st_size && verbose>4)|| fStats.st_size)
       return ScanFile(szFileName,st.st_size,GetFlags());
     }
//  chdir((char*)curpath);
  return -1;
 }

#ifdef
 CHECK_SECT
extern char CurChkDev[40];
//SectorCheck SectChk[SECTORCHECKLIMIT];
//ULONG ulSectChk=0;

#include "SysInstall/sysinstall.h"
#include <sys/mount.h>
int printAllMount(void);
struct statfs *findMntPnt(char* mntname,struct statfs *statfsbuf);

// This code inside SCAN THREAD!
int CheckBoot(CHAR *path,DWORD type,char *szDev)
 {
  AVPScanObject ScanObject;
  if(path[0]!='/')return 0;
  CHAR Name[CCHMAXPATH];
  sprintf(Name,"%s%s",LoadString(IDS_BOOT_SECTOR,"Boot sector of drive "),szDev);//&path[2]);

  memset((void*)&ScanObject,0,sizeof(AVPScanObject));
  ScanObject.SO1.SType=OT_BOOT;
  ScanObject.MFlags=GetFlags();//tPage2.Mask;

  strcpy(ScanObject.Name,Name);
  ULONG ret=0;
//  if(ulSectChk<SECTORCHECKLIMIT)
   {
    ScanObject.SO1.Drive=(BYTE)(0x20);//+ulSectChk);
    switch(type)
     {
      case IDI_NET_ON:  //DRIVE_REMOTE:
       ScanObject.SO1.Disk=0x20; break;
      //case DRIVE_RAMDISK:
      //  ScanObject.Disk=0x40; break;
      case IDI_CD:     //DRIVE_CDROM:
       ScanObject.SO1.Disk=0x60; break;
      case IDI_LOCAL1: //case DRIVE_FIXED:
       ScanObject.SO1.Disk=0x80; break;
      case IDI_FLOPPY_12:
      case IDI_FLOPPY_144:  // DRIVE_REMOVABLE:
       ScanObject.SO1.Disk=ScanObject.SO1.Drive; break;
      default:
       return 0;
    }
    strcpy(CurChkDev,"/dev/");
    strcat(CurChkDev,szDev);
//    strcpy(SectChk[ulSectChk].sDev,"/dev/");
//    strcat(SectChk[ulSectChk].sDev,szDev);
//    ulSectChk++;
    //printf("chask boot: %s %d\n",ScanObject.Name,ScanObject.SO1.Disk);
    ret=BAvpProcessObject(&ScanObject);
//    ulSectChk--;
   }

  return ret;
 }

extern struct chunk *chunk_info[16];

typedef BOOL (*lp_AvpRead13 )(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors,BYTE *Buffer);
int CheckMBRS()
 {
  DWORD ret=0;
  ULONG nDisks='z';
  //if(AvpRead13)
  // for(;nDisks<16;nDisks++)
  //  {
  //   BYTE Buffer[0x200];
  //   if(!AvpRead13((BYTE)(0x80+nDisks),1,0,1,Buffer)) break;
  //  }
  //if(DosPhysicalDisk(INFO_COUNT_PARTITIONABLE_DISKS,(PVOID)&nDisks,2,NULL,0)==0)

  // /dev/sd[0-6][a-h] SCSI c - all disk a - root of disk ufs
  // /dev/rmtN strimer  N - number
  // /dev/fd[0-1]
  // /dev/wd[0-3]s[1-4][a-h]
  //char phusdev[]="/dev/hd ";
  char phusdev[40];
  //strcpy(phusdev,CurChkDev);
  //strcat(phusdev,"s");
  int  fd;
  int len=strlen(phusdev);

  Device **devs = NULL;

  devs=deviceFind(strrchr(CurChkDev,'/')?
 (strrchr(CurChkDev,'/')+1):CurChkDev,DEVICE_TYPE_DISK);
  if(!devs)
   {
    printf("Unable to find disk %s!\n",CurChkDev);
    return FALSE;//DITEM_FAILURE;
 //BSDi
   }
  // Toggle enabled status?
  if(!devs[0]->enabled)
   {
    devs[0]->enabled = TRUE;
    Disk *d=(Disk *)(devs[0]->privates);

    // Set up the chunk array
    record_chunks(d);
    // Now print our overall state
    if(d)
     {
      for(int i=0;chunk_info[i];i++)
       {
        if(strstr("-",chunk_info[i]->name)!=0) continue;
	
        //printf("%8s\n",
chunk_info[i]->name);
        strcpy(phusdev,"/dev/");
        strcat(phusdev,chunk_info[i]->name);
        if((fd=open(phusdev,O_RDWR))!=-1)
         {
          close(fd);

          AVPScanObject ScanObject;
          memset((void*)&ScanObject,0,sizeof(AVPScanObject));
          ScanObject.SO1.SType=OT_MBR;
          ScanObject.MFlags=GetFlags();//tPage2.Mask;
  //      ScanObject.MFlags|=MF_ALLWARNINGS;
          ScanObject.MFlags|=MF_ALLENTRY;
          ScanObject.SO1.Drive=2;

          //sprintf(ScanObject.Name,LoadString(IDS_MBR_SECTOR,"Master Boot Record of hd%c"),i);
          sprintf(ScanObject.Name,"Master Boot Record of %s",chunk_info[i]->name);

          strcpy(CurChkDev,phusdev);
          ret&=BAvpProcessObject(&ScanObject);
          if(scan_goes==0)break;
         } 
       }     
 
  //      print_chunks(d);
     }
  //    diskPartition(devs[0]);
   }
  else
      devs[0]->enabled = FALSE;

  //for(int i='a';i<((int)nDisks);i++)
//  for(int i='1';i<='4';i++)
//   {
//    phusdev[len]=i;
//    phusdev[len+1]=0;
//    if((fd=open(phusdev,O_RDWR))!=-1)
//     {
//      close(fd);

//      AVPScanObject ScanObject;
//      memset((void*)&ScanObject,0,sizeof(AVPScanObject));
//      ScanObject.SO1.SType=OT_MBR;
//      ScanObject.MFlags=GetFlags();//tPage2.Mask;
  //    ScanObject.MFlags|=MF_ALLWARNINGS;
//      ScanObject.MFlags|=MF_ALLENTRY;
//      ScanObject.SO1.Drive=2;

//
//      sprintf(ScanObject.Name,LoadString(IDS_MBR_SECTOR,"Master Boot Record of hd%c"),i);

//
  //      if(ulSectChk<SECTORCHECKLIMIT)
  //       {
  //        strcpy(SectChk[ulSectChk].sDev,phusdev);
  //        ScanObject.SO1.Disk=(BYTE)(0x80+ulSectChk);
  //        ulSectChk++;
//      strcpy(CurChkDev,phusdev);
  //      ret&=BAvpProcessObject(&ScanObject);
  //        ulSectChk--;
  //       }
//      if(scan_goes==0)break;
//     }
//   }
  //printf("CheckMBRS device %s\n",phusdev);
  return ret;
 }
#endif

int TestSectors(void)
 {
  ULONG mFlags;

  scan_goes=1;
  ExitCode=0;
  ApplyToAll=0;
  MethodDelete=0;
  SectorWarn=0;
  mFlags=GetFlags();

  VirCountArray.RemoveAll();
  iniWorkStat();

  iniWorkStatForEach();
#ifdef
 CHECK_SECT
  BOOL MBRS;
#endif

  // -- Begin of Sectors Check --
  if(prf_data.Sectors)
   { // check BOOT
#ifdef
 CHECK_SECT
       deviceGetAll();

       if(!cmdline.skip_boot)
        {
  //printAllMount();
         Device **devs;
         devs=deviceFind(NULL,DEVICE_TYPE_DISK);

         MBRS=FALSE;
         if((pTail!=NULL)
&&(devs!=0))
          {
           pList=pTail;
           do
            {
             if(pList->Selected==1)
              {
               if(pList->szName[0]=='/')
                {
                 struct statfs statfsbuf;
                 if(findMntPnt(pList->szName,&statfsbuf)!=NULL)
                   for(int i=0;devs[i];i++)
                    {
                     if(strstr(devs[i]->name,statfsbuf.f_mntfromname)==0)
                      {
                       printf("%s %s\n",statfsbuf.f_mntfromname,devs[i]->name);
                       if(scan_goes==0)goto ret;
                       CheckBoot(pList->szName,IDI_LOCAL1,devs[i]->name);
                       MBRS=TRUE;
                      }
                    }
 //                 DWORD type;
 //                 type=DiskType(pList);
 //                 switch(type)
 //                  {
 //                   case IDI_LOCAL1:   //case DRIVE_FIXED:
 //                     if(scan_goes==0)goto ret;
 //                     CheckBoot(pList->szName,type,pList->szDev);
 //                     MBRS=TRUE;
 //                   default: break; //All other types check later.
 //                  }

                }
              } // if(Disk[_ddd]&DISK_SELECTED)
             pList=pList->pNext;
            } while((pList != NULL)&&scan_goes);
          }
        }//if(!cmdline.skip_boot)

       // check MBR
       if(!cmdline.skip_mbr)
       {
        if(scan_goes==0)goto ret;
        if(MBRS)        CheckMBRS();
       }
#endif
   } // -- End of Sectors Check ----
      
  if(prf_data.RepForEachDisk==1) reportForEach("Sectors");
  // -- End of Files Check ----
  //reportDone();
  if(scan_goes==0)
  scan_goes=(char)0;

  if(workStat.suspForEach) ExitCode=3;
  if(workStat.warnForEach) ExitCode=2;
  if(workStat.vbForEach)
  if(workStat.vbForEach==workStat.dfForEach) ExitCode=5;
  else ExitCode=4;
  //if(flgScan==1) flgScan=0;
  return ExitCode;
 }

/////////////////////////
void ObjectMessage(AVPScanObject* ScanObject,const char* Message,UCHAR AttrMess)
 {
  CHAR LogStr[0x210];
  if(ScanObject!=NULL)
   {
    if(blDaemon==FALSE) putchar('\r');
    if(ScanObject->Entry)
     {
      if(blDaemon==FALSE)
       {
        printf(LoadString(IDS_ENTRY_FORMAT,"%s  Entry #%d\t"),ScanObject->Name,ScanObject->Entry);
        putchar('\t');
       }	
      if(prf_data.Report)
       {
        sprintf(LogStr,LoadString(IDS_ENTRY_FORMAT,"%s  Entry #%d\t"),ScanObject->Name,ScanObject->Entry);
        reportPutS(LogStr);
       }
      //sprintf(LogStr,LoadString(IDS_ENTRY_FORMAT,"%s  Entry #%d"),ScanObject->Name,ScanObject->Entry);
     }
    else
     {
      if(strlen(ScanObject->Name)<54)
       {//sprintf(LogStr,"%s\t",ScanObject->Name);
        strcpy(LogStr,ScanObject->Name);
        strcat(LogStr,"\t");
       }	
      else
       {
        strncpy(LogStr,ScanObject->Name,24);
        LogStr[24]=0;
        strcat(LogStr," ... ");
        strcat(LogStr,&ScanObject->Name[strlen(ScanObject->Name)-25]);
        strcat(LogStr,"\t");
       }
      //printf(LogStr);
      if(blDaemon==FALSE) _puts(LogStr);
      if(prf_data.Report) reportPutS(ScanObject->Name);
     }

    if(Message!=NULL)
     {
      //strcat(LogStr,"\t");
      //strcat(LogStr,(char*)Message);
      //strcat(LogStr,"\r\n");
      //printf((char*)Message);
      if(blDaemon==FALSE)
       {
        printf((char*)Message);
// PutStrAttr((char*)Message,AttrMess);
        putchar('\n');
         // PutStrAttr("\n",attr);
       }
      if(prf_data.Report)
       {
        sprintf(LogStr,"\t%s\r\n",(char*)Message);
        reportPutS(LogStr);
       }
     }
//    if(strlen(LogStr)!=0)
//     {
//      putch('\r');
//      printf(LogStr);
//      reportPutS(LogStr);
//     }
   }
 }

BOOL CopyToFolder(char *Name,char *Folder)
 {
  static BOOL flCreate=TRUE;
  static BOOL flCopy=TRUE;
  static BOOL flCurrent=FALSE;
  CHAR  *curpath;
  int i;
  APIRET rc;
  curpath=getcwd(NULL,0);
  if(Folder[0]!='/')
   {
    char Folderpath[CCHMAXPATH];
    strcpy(Folderpath,prg_path);
    if(Folderpath[strlen(Folderpath)-1]=='/') Folderpath[strlen(Folderpath)-1]=0;
    chdir(Folderpath);
   }
  if(chdir(Folder))
   {
    if(flCreate)
     if(DID_OK_YES==MyWaitMessBox(LoadString(IDS_FOLDER_CREATE,"Folder for infected (suspicious) files doesn't exist.\nCreate folder?"),MB_YESNO,cmdline.skip_dlg))
      {
       mkdir(Folder,S_ISUID|S_ISGID|S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH);
       if(chdir(Folder))
        {
         MyWaitMessBox(LoadString(IDS_FOLDER_NOT_CREATE_L,"Folder for infected (suspicious) files was not created."),MB_OK,cmdline.skip_dlg);
         if(DID_OK_YES!=MyWaitMessBox(LoadString(IDS_FOLDER_COPY_L,"Copy infected (suspicious) files into current directory?"),MB_YESNO,cmdline.skip_dlg))
          {
           MyWaitMessBox(LoadString(IDS_FOLDER_NOT_COPY_L,"Infected (suspicious) files will not copy"),MB_OK,cmdline.skip_dlg);
           flCopy=FALSE;
          }
         else flCurrent=TRUE;
        }
      }
     else
      {
       flCreate=FALSE;
       if(DID_OK_YES!=MyWaitMessBox(LoadString(IDS_FOLDER_COPY_L,"Copy infected (suspicious) files into current directory?"),MB_YESNO,cmdline.skip_dlg))
        {
         MyWaitMessBox(LoadString(IDS_FOLDER_NOT_COPY_L,"Infected (suspicious) files will not copy"),MB_OK,cmdline.skip_dlg);
         flCopy=FALSE;
        }
       else flCurrent=TRUE;
//       if(DID_OK_YES!=MyWaitMessBox(LoadString(IDS_APPLY_TOALL,"Apply to all infected object?"),MB_YESNO,cmdline.skip_dlg))
//         flAllCopy=FALSE;
      }
   }

  if(flCurrent==TRUE)
 chdir(prg_path);
  if(flCopy==TRUE)
   {
    //CHAR NewName[CCHMAXPATH];
    //for(i=0;(i<strlen(Name))&&(Name[i]!='/');i++) ;
    //strncpy(NewName,Name,i);
    //NewName[i]=0;
    //for(;(i>=0)&&(NewName[i]!='\\');i--);
    //rc=DosCopy(NewName,&NewName[i+1],DCPY_EXISTING|DCPY_FAILEAS);
    for(i=strlen(Name);(i>=0)&&(Name[i]!='/');i--);
    rc=copyReg(Name,&Name[i+1]);
   }
  chdir(curpath);

  //Free(curpath);
  return TRUE;
 }

void sound(unsigned short chast,unsigned long delay) // frequency Hz, delay microseconds
 {
 }
 
VOID AvpSleep(DWORD ms_)
;
LONG avp_mess(DWORD mp1,DWORD mp2)
 {
  AVPScanObject* ScanObject;//=(AVPScanObject*)lParam;
  char s[0x200],tempb[0x200];
  char Chr;
  LONG ret=0;


  if(cmdline.abort_disable!=1)
//   if(kbhit())
//    if(getch()!=ERR)
 //curses.h
//    if(getch()==27)
 //curses.h
     if(scan_goes!=0)
      {
//       printf("\n");
//       printf(LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
//       printf(" (Yes/No)");
//       //SetCursorOff(TRUE);
//       Chr=(char)_getchar();
//       printf("\n");
       if((Chr=='y')||(Chr=='Y'))
        {
//         prf_data.ShowOK=0;
//         ExitCode=1;
//         scan_goes=0;
//         BAvpCancelProcessObject(1);
        }
       //SetCursorOff(FALSE);
      }

  switch(LONGFROMMP(mp1))
   {
   
    case AVP_CALLBACK_PUT_STRING:
      //printf("%s\r\n",(char*)mp2);
      break;
    case AVP_CALLBACK_ROTATE:
      AvpSleep(0);
      break;
    case AVP_CALLBACK_ROTATE_OFF:
      break;
    case AVP_CALLBACK_MB_OK:
      ret=MyWaitMessBox((char*)PVOIDFROMMP(mp2),MB_OK,cmdline.skip_dlg);
      break;
    case AVP_CALLBACK_MB_OKCANCEL:
      ret=MyWaitMessBox((char*)PVOIDFROMMP(mp2),MB_OKCANCEL,cmdline.skip_dlg);
      break;
    case AVP_CALLBACK_MB_YESNO:
      ret=MyWaitMessBox((char*)PVOIDFROMMP(mp2),MB_YESNO,cmdline.skip_dlg);
      break;
    case AVP_CALLBACK:
      //h_AVP_KERNEL=(HWND)PVOIDFROMMP(mp2);
      ret=1;
      break;
    case AVP_INTERNAL_CHECK_DIR:
//      StatusLine((char*)mp2);
      break;
    case AVP_CALLBACK_LOAD_PROGRESS:     //0x0108 LOAD_PROGRESS_STRUCT*
      LOAD_PROGRESS_STRUCT *lps;
      lps=(LOAD_PROGRESS_STRUCT*)PVOIDFROMMP(mp2);

      SurBase.VirCount=lps->virCount;
      SurBase.RecCount=lps->recCount;
      SurBase.LastUpdateYear=lps->LastUpdateYear;
      SurBase.LastUpdateMonth=lps->LastUpdateMonth;
      SurBase.LastUpdateDay=lps->LastUpdateDay;
      SurBase.LastUpdateTime=lps->LastUpdateTime;
      memcpy(SurBase.RecCountArray,lps->RecCountArray,4*8);
      if(lps->curLength!=0)
        {
         char tLoad[]="                                                                           ";
//         if(strlen((char*)mp2)<50)
           sprintf(&tLoad[1],LoadString(IDS_LOADBASE_FORMAT,"Loading %s %d%%"),lps->curName,lps->curPos*100/lps->curLength);
//         else
//          {
//           char LogStr[56];
//           strncpy(LogStr,(char*)mp2,24);
//           LogStr[24]=0;
//           strcat(LogStr," ... ");
//           strcat(LogStr,&((char*)mp2)[strlen((char*)mp2)-25]);
//           printf(LogStr);
//          }
//        }
	 printf(tLoad);
         putchar('\r');
         fflush(stdout);
        }
      break;
    case AVP_CALLBACK_LINK_NAME:         //0x0109 const char* sourcename DEBUG!
      //printf("LINK_NAME        \n:");
      break;
    case AVP_CALLBACK_ASK_DELETE:        //0x010A ScanObject* (For DELETE method) 2-delete; other-skip
      if(MethodDelete) return MethodDelete;
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_ASK_DELETE,"Disinfection of %s \ninfected by virus %s is impossible. \n\n Delete this object permanently?"),ScanObject->Name,
               BAvpMakeFullName(tempb,ScanObject->VirusName));
      ret=MyWaitMessBox(s,MB_YESNO,cmdline.skip_dlg);//MB_ICONQUESTION|MB_DEFBUTTON2);
      if(ret==DID_OK_YES)
       {
        ret=2;
        sprintf(s,LoadString(IDS_ASK_DELETE_ALL,"Delete all nondisinfectable objects?"));
       }
      else
       {
        ret=1;
        sprintf(s,LoadString(IDS_ASK_DELETE_SKIP_ALL,"Do not delete all  nondisinfectable objects?"));
       }
      if(DID_OK_YES==MyWaitMessBox(s,MB_YESNO,cmdline.skip_dlg))//|MB_ICONQUESTION))
        MethodDelete=ret;
      break;
    case AVP_CALLBACK_GET_SECTOR_IMAGE:  //0x010B -  (For overwriting sectors)    BYTE* SectorImage
      if((ULONG)mp2==ST_MBR)
       return (LONG)BinMBR;
      else
       return (LONG)BinBOOT;
    case AVP_CALLBACK_OBJ_NAME:          //0x010C char* name
      char *Folder,*Name;

      Folder=(char*)mp2;

      Name=strrchr(Folder,'/');
      if(Name)
       {
        Folder[Name-Folder]=0;
        Name++;
       }
      else Name=Folder;

      if(Name!=Folder)
       {
        //if(prf_data.TestOnlySelected==1)
         if(cmpDirMask1(Folder)==FALSE) ret=17; //есть в списке
        if(cmpDirMask(Folder)==TRUE) ret=17;   //есть в списке
       }

      if(cmpFileMask(Name)==TRUE) ret=17;
      //if(cmpFileMask((char*)mp2)==TRUE) return 17;

      if(Name!=Folder)
       {
        Folder[(--Name)-Folder]='/';
        Name=Folder;
       }
      if(ret==17) return ret;

      if((SelfScan==0)
&&(blDaemon==FALSE))
       {
        printf("\r                                                      \r");
        if((strlen((char*)mp2)<54)
&&(prf_data.LongStrings==0)) _puts((char*)mp2);
        else
         {
          char LogStr[56];
          strncpy(LogStr,(char*)mp2,24);
          LogStr[24]=0;
          strcat(LogStr," ... ");
          strcat(LogStr,&((char*)mp2)[strlen((char*)mp2)-25]);
          printf(LogStr);
         }
       }
      break;
    case AVP_CALLBACK_ERROR_FILE_OPEN:   //0x0400  char* filename
      sprintf(s,LoadString(IDS_ERROR_FILE_OPEN,"Can't open file: %s"),(char*)PVOIDFROMMP(mp2));//lParam
      ret=MyWaitMessBox((char*)s,MB_OK,cmdline.skip_dlg);
      return ret;
    case AVP_CALLBACK_ERROR_SET:         //0x0401  char* filename
      sprintf(s,LoadString( IDS_ERROR_SET,"Wrong set file: %s"),(char*)PVOIDFROMMP(mp2));     //lParam
      ret=MyWaitMessBox((char*)s,MB_OK,cmdline.skip_dlg);
      return ret;
    case AVP_CALLBACK_ERROR_BASE:        //0x0402  char* filename
      sprintf(s,LoadString(IDS_ERROR_BASE,"Old or corrupted antiviral base: %s"),(char*)PVOIDFROMMP(mp2));    //lParam
      ret=MyWaitMessBox((char*)s,MB_OK,cmdline.skip_dlg);
      return ret;
    case AVP_CALLBACK_ERROR_KERNEL_PROC: //0x0403  char* procname
      sprintf(s,LoadString(IDS_ERROR_KERNEL_PROC,"Base kernel proc absent: %s"),"Error");//(char*)lParam);
      ret=MyWaitMessBox((char*)s,MB_OK,cmdline.skip_dlg);
      return ret;
    case AVP_CALLBACK_ERROR_LINK:        //0x0405  LOAD_PROGRESS_STRUCT*
      sprintf(s,LoadString(IDS_ERROR_LINK,"Object code not linked."));
      ret=MyWaitMessBox((char*)s,MB_OK,cmdline.skip_dlg);
      return ret;
    case AVP_CALLBACK_ERROR_SYMBOL_DUP:  //0x0406  char* symbolname
      if(blDaemon==FALSE)printf("ERROR_SYMBOL_DUP \n");
      break;
    case AVP_CALLBACK_ERROR_SYMBOL:      //0x0407  char* symbolname
      if(blDaemon==FALSE)printf("ERROR_SYMBOL     \n");
      break;
    case AVP_CALLBACK_ERROR_FIXUPP:      //0x0408  int fixuppcode
      if(blDaemon==FALSE)printf("ERROR_FIXUPP     \n");
      break;
    case AVP_CALLBACK_OBJECT_DONE:       //0x0200 ScanObject*
     ScanObject=(AVPScanObject*)mp2;

     if(!ScanObject->Handle)
      {
//       DWORD fs=GetCompressedFileSize(ScanObject->Name,NULL);
//       if(fs==(DWORD)(-1))
         workStat.CountSize+=ScanObject->Size/1024+1;
//       else workStat.CountSize+=fs/1024+1;
      }

     if(ScanObject->RFlags & RF_INTERFACE_FAULT)
      {
       ObjectMessage(ScanObject,LoadString(IDS_INTERFACE_FAULT,"intrface fault."),0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_KERNEL_FAULT)
      {
       ObjectMessage(ScanObject,LoadString(IDS_KERNEL_FAULT,"kernel fault.")  ,0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }

     if(ScanObject->RFlags & RF_NOT_A_PROGRAMM)
      {
       //ObjectMessage(ScanObject,"RF_NOT_A_PROGRAMM.");
       break;
      }
     if(ScanObject->RFlags & RF_NOT_OPEN)
      {
       if(prf_data.ShowOK)
        {
         ObjectMessage(ScanObject,LoadString(IDS_OBJ_IOERROR,"I/O error.") ,0);
         ScanObject->RFlags|=RF_REPORT_DONE;
         //_getchar();
        }
       return scan_goes;
      }
     if(ScanObject->SO1.SType==OT_FILE && !ScanObject->InPack) incWorkStat(INCL_FILES);
     if(ScanObject->SO1.SType & OT_SECTOR) incWorkStat(INCL_SECTORS);

     if(ScanObject->RFlags & RF_REPORT_DONE) return scan_goes;

     if(ScanObject->RFlags & RF_DISK_OUT_OF_SPACE)
      {
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_DISKOUTOFSPACE,"disk out of space.") ,0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_CORRUPT)
      {
       incWorkStat(INCL_CORRUPTED);
if(blDaemon==FALSE) printf("\n");       
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_CORRUPTED,"corrupted.") ,0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_IO_ERROR)
      {
       incWorkStat(INCL_IOERRORS);
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_IOERROR,"I/O error.") ,0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_CURE_FAIL)
      {
       sprintf(s,LoadString(IDS_OBJ_DISINF_FAILED,"disinfection failed: %s"),BAvpMakeFullName(tempb,ScanObject->VirusName));
       ObjectMessage(ScanObject,s,0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_PROTECTED)
      {
//     if(prf_data.ShowOK)
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_PROTECTED,"password protected."),0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }

     if(!(ScanObject->RFlags & RF_REPORT_DONE))
      {
#ifdef _DEBUG
       if(ScanObject->RFlags && !(ScanObject->RFlags & RF_CURE))
        {
         s.Format(_T("RFlags: %08X"),ScanObject->RFlags);
         ObjectMessage(ScanObject,s);
         delete(s);
        }
       else
#endif
       if(prf_data.ShowOK && !SelfScan)
         ObjectMessage(ScanObject,LoadString(IDS_OBJ_OK,"ok.") ,0);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(SelfScan && (ScanObject->RFlags & RF_DETECT))
      {
     //  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,(PSZ)LoadString(IDS_NEEDTORESTART,""),
     //    (PSZ)"AVP for DOS32",1,MB_OK|MB_APPLMODAL|MB_MOVEABLE);
       BAvpCancelProcessObject(1);
       scan_goes=0;
       ExitCode=7;
     //  WinPostMsg(hWndClient,WM_CLOSE,NULL,NULL);
      }
     return scan_goes;
//   break;
    case AVP_CALLBACK_OBJECT_MESS:       //0x0201 ScanObject* (ScanObject->VirName is *message)
      ScanObject=(AVPScanObject*)mp2;
      ObjectMessage(ScanObject,ScanObject->VirusName,0);
      break;
    case AVP_CALLBACK_OBJECT_DETECT:     //0x0202 ScanObject*
      ScanObject=(AVPScanObject*)mp2;
      BAvpMakeFullName(tempb,ScanObject->VirusName);
      sprintf(s,LoadString(IDS_OBJ_INFECTED,"infected: %s"),tempb);
      ObjectMessage(ScanObject,s,(prf_data.Color==1)? 0x0c:0x0f);
      ScanObject->RFlags|=RF_REPORT_DONE;
      break;
    case AVP_CALLBACK_ASK_CURE:          //0x0107 ScanObject*                     0-skip; 1-try to disinfect; 2-delete;
      {
       BOOL CopyInfected=prf_data.InfectedCopy;
       ScanObject=(AVPScanObject*)mp2;
       //if(ScanObject->InArc)ret=0;else

       switch(prf_data.InfectedAction)
        {
         case 1:
           DisinfectDlg dd;
           if(!ApplyToAll)
            {
             if(ScanObject->SO1.SType==OT_FILE)
               sprintf(dd.m_sObject,"%s",LoadString(IDS_DISINF_OFILE,"File"));
             if(ScanObject->SO1.SType & OT_SECTOR)
               sprintf(dd.m_sObject,"%s",LoadString(IDS_DISINF_OSECTOR,"Sector"));

             strcat(dd.m_sObject,"    ");
             strcat(dd.m_sObject,ScanObject->Name);
             dd.m_sVirus[0]=0;
             BAvpMakeFullName(dd.m_sVirus,ScanObject->VirusName);
             dd.m_Action=LastAction;
             dd.m_All=0;
             dd.m_CopyFolder=prf_data.InfectedCopy;
             if(prf_data.Sound)
              {
               sound(1193180/880,30000);
//             if(!sndPlaySound("infected.wav", SND_ASYNC))
//               WinAlarm(HWND_DESKTOP,WA_WARNING);
              }

             ULONG r;

             r=DisinfectDlgProc(&dd);

             if(r==DID_CANCEL)
              {
               ret=0;
               break;
              }
             if(r==IDC_STOP)
              {
               BAvpCancelProcessObject(1);
               ExitCode=1;
               scan_goes=0;
               ret=0;
               break;
              }
             LastAction=dd.m_Action;
             CopyInfected|=dd.m_CopyFolder;
             if(DID_OK_YES==MyWaitMessBox(LoadString(IDS_APPLY_TOALL,"Apply to all infected objects?"),MB_YESNO,cmdline.skip_dlg))
               dd.m_All=1;
             ApplyToAll=dd.m_All;
             if(ApplyToAll) prf_data.InfectedCopy=(char)CopyInfected;
            }
           ret=LastAction;
           break;
         case 2:
           ret= 1; //disinfect
           break;
         case 3:
           ret= 2; //delete
           break;
         default:
         case 0:
           if(prf_data.Sound)
            {
             sound(1193180/880,30000);
//           if(!sndPlaySound("infected.wav", SND_ASYNC))
//             WinAlarm(HWND_DESKTOP,WA_WARNING);
            }
           ret= 0; //test only
           break;
        }
       if(CopyInfected) CopyToFolder(ScanObject->Name,prf_data.InfectedFolder);
      }
     if(ret && !SectorWarn && (ScanObject->SO1.SType & OT_SECTOR))
      {
       if(DID_OK_YES!=MyWaitMessBox(LoadString(IDS_CURE_SECTOR,"Disinfection of sectors is dangerous procedure.\x0d\x0aWe recommend to make backup of whole disk contents.\x0d\x0a\x0d\x0aTry to perform disinfection rigt now?"),
            MB_YESNO,cmdline.skip_dlg))//|MB_ICONSTOP|MB_DEFBUTTON2))
        {
         ret=0;
         BAvpCancelProcessObject(1);
         ExitCode=1;
         scan_goes=0; //KillScan=1;
        }
       SectorWarn=1;
      }
     char *np;
     np=ScanObject->VirusName;
     while(*np=='#')
      {
       if(*(--np)=='\n') break;
       while(*(--np)) if(*np=='\n') break;
       np++;
      }

     INT i;
     for(i=VirCountArray.GetUpperBound();i>=0;i--)
       if(VirCountArray[i]==(ULONG)np) goto noadd;
     workStat.kv=1+VirCountArray.Add((ULONG)np);
     //incWorkStat(INCL_KNOWNVIR);
noadd:  incWorkStat(INCL_VIRBODIES);
     break;
    case AVP_CALLBACK_OBJECT_CURE:       //0x0203 ScanObject*
     ScanObject=(AVPScanObject*)mp2;
     BAvpMakeFullName(tempb,ScanObject->VirusName);
     sprintf(s,LoadString(IDS_OBJ_DISINFECTED,"disinfected: %s"),tempb);
     ObjectMessage(ScanObject,s,(prf_data.Color==1)? 0x0c:0x0f);
     ScanObject->RFlags|=RF_REPORT_DONE;
     incWorkStat(INCL_DISINFECT);
     break;
    case AVP_CALLBACK_OBJECT_WARNING:    //0x0204 ScanObject*
      if(prf_data.Warnings)
       {
        if(prf_data.Sound)
 sound(1193180/880,30000);
        ScanObject=(AVPScanObject*)mp2;
        sprintf(s,LoadString(IDS_OBJ_WARNING,"warning: %s"),BAvpMakeFullName(tempb,ScanObject->WarningName));
        ObjectMessage(ScanObject,s,(prf_data.Color==1)? 0x0c:0x0f);
        ScanObject->RFlags|=RF_REPORT_DONE;
        incWorkStat(INCL_WARNING);
        if(prf_data.SuspiciousCopy) CopyToFolder(ScanObject->Name,prf_data.SuspiciousFolder);
       }
      break;
    case AVP_CALLBACK_OBJECT_SUSP:       //0x0205 ScanObject*
      if(prf_data.Sound)
 sound(1193180/880,30000);
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_OBJ_SUSPICION,"suspicion: %s"),BAvpMakeFullName(tempb,ScanObject->SuspicionName));
      ObjectMessage(ScanObject,s,(prf_data.Color==1)? 0x0c:0x0f);
      ScanObject->RFlags|=RF_REPORT_DONE;
      incWorkStat(INCL_SUSPICION);
      if(prf_data.SuspiciousCopy) CopyToFolder(ScanObject->Name,prf_data.SuspiciousFolder);
      break;
    case AVP_CALLBACK_OBJECT_PACKED:     //0x0206 ScanObject*
     ScanObject=(AVPScanObject*)mp2;
     if(prf_data.ShowPack)
      {
       BAvpMakeFullName(tempb,ScanObject->VirusName);
       sprintf(s,LoadString(IDS_OBJ_PACKED,"packed: %s"),tempb);
       ObjectMessage(ScanObject,s,0);
      }
     if(!ScanObject->InPack) incWorkStat(INCL_PACKED);
     break;
    case AVP_CALLBACK_OBJECT_ARCHIVE:    //0x0207 ScanObject*
      if(prf_data.ShowPack)
       {
        ScanObject=(AVPScanObject*)mp2;
        BAvpMakeFullName(tempb,ScanObject->VirusName);
        sprintf(s,LoadString(IDS_OBJ_ARCHIVE,"archive: %s"),tempb);
        ObjectMessage(ScanObject,s,0);
       }
      incWorkStat(INCL_ARCHIVES);
      break;
    case AVP_CALLBACK_OBJECT_DELETE:     //0x0208 ScanObject*
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_OBJ_DELETED,"deleted: %s"),BAvpMakeFullName(tempb,ScanObject->VirusName));
      ObjectMessage(ScanObject,s,0);
      ScanObject->RFlags|=RF_REPORT_DONE;
      incWorkStat(INCL_DELETED);
      break;
    case AVP_CALLBACK_OBJECT_UNKNOWN:    //0x0209 ScanObject*
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_OBJ_UNKNOWN,"%s: unknown format."),BAvpMakeFullName(tempb,ScanObject->VirusName));
      ScanObject->RFlags|=RF_REPORT_DONE;
      ObjectMessage(ScanObject,s,0);
      break;
      
    case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
      return sign_check_file( (char*)mp2,1,vndArray,vndArrayCount,0);
    case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
      if(blDaemon==FALSE) printf("FILE_INTEGRITY_FAILED %s\r\n",(char*)mp2);
      //ret=SignReportMB((char*)mp2,-1001);
      //if(ret) PostMessage(WM_CLOSE);  //1 - Exit 0 - continue
      break;
    default: break;
   }   
  return ret;
 }

ULONG DisinfectDlgProc(DisinfectDlg* dd)
 {
  ULONG ret;
  //(LoadString( IDS_DISINFECT_DIALOG,"" )); //Title

  
  if(blDaemon==FALSE)
   {
    printf("%s  ",dd->m_sObject);
    printf("%s %s\n",LoadString(IDS_INFECTED_BY,"Infected by virus:"),dd->m_sVirus);

    printf(LoadString(IDS_TAB_ACTIONS,"Actions"));

    switch (dd->m_Action)
     {
      case 0:
        printf(" - %s (Ok/",LoadString(IDS_DISINFECT_R,"Report only"));
        printf("%s/",LoadString(IDS_DISINFECT_D,"disInfect"));
        printf("%s/Cancel/Stop) ",LoadString(IDS_DISINFECT_DEL,"Delete"));
        break;
      case 1:
        printf(" - %s (Ok/",LoadString(IDS_DISINFECT_D,"disInfect"));
        printf("%s/",LoadString(IDS_DISINFECT_R,"Report only"));
        printf("%s/Cancel/Stop) ",LoadString(IDS_DISINFECT_DEL,"Delete"));
        break;
      case 2:
        printf(" - %s (Ok/",LoadString(IDS_DISINFECT_DEL,"Delete"));
        printf("%s/",LoadString(IDS_DISINFECT_R,"Report only"));
        printf("%s/Cancel/Stop) ",LoadString(IDS_DISINFECT_D,"disInfect"));
        break;
     }
//  (LoadString(IDS_COPYINFECTED,"~Копировать в отдельную папку"));
//  (LoadString(IDS_APPLY_TOALL,"П~рименять ко всем зараженным объектам"));

//  WinSendMsg(hWndTmp,BM_SETCHECK,MPFROMSHORT(dd->m_All),NULL);
//  WinSendMsg(hWndTmp,BM_SETCHECK,MPFROMSHORT(dd->m_CopyFolder),NULL);

   }
  if((cmdline.skip_dlg!=1)
&&(blDaemon==FALSE))
    switch (_getchar())
     {
      case 'R': //Report
      case 'r':
      case 'N':
      case 'n':
      case 'Т':
      case 'т':
        dd->m_Action=0;
        ret=DID_OK;
        break;
      case 'I': //disinfect
      case 'i':
      case 'K':
      case 'k':
      case 'Л':
      case 'л':
        dd->m_Action=1;
        ret=DID_OK;
        break;
      case 'D': //Delete
      case 'd':
      case 'E':
      case 'e':
      case 'У':
      case 'у':
        dd->m_Action=2;
        ret=DID_OK;
        break;
      case 'O': //Ok
      case 'o':
      case 'Щ':
      case 'щ':
        ret=DID_OK;
        //dd->m_All=(USHORT)WinSendMsg(hWndTmp,BM_QUERYCHECK,NULL,NULL);
        //dd->m_CopyFolder=(USHORT)WinSendMsg(hWndTmp,BM_QUERYCHECK,NULL,NULL);
        break;
      case 'C':
      case 'c':
      case 'С':
      case 'с':
        ret=DID_CANCEL;
        break;
      case 'S':
      case 's':
      case 'Ы':
      case 'ы':
        ret=IDC_STOP;
        break;
      default: ret=DID_CANCEL;
     }
   else
     ret=DID_OK;
//  putch('\n');

  return ret;
 }
