#include <os2.h>
#include "memalloc.h"
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <i86.h>
#include <ctype.h>
#include <share.h>
#include <c:\lang\watcom\h\sys\types.h>
#include <c:\lang\watcom\h\sys\stat.h>
#include <dos.h>
#include <direct.h>
#include <fcntl.h>
//#include "key.h"
#include "start.h"
#include "disk.h"
#include "profile.h"
#include "kernel.h"
#include "avp_os2.h"
#include "../../Common.Fil/ScanApi/avp_msg.h"
#include "dword.h"
#include "..\locale\resource.h"
#include "contain.h"
#include "avp_rc.h"
#include "report.h"

#include "..\\typedef.h"
#include "../../Common.Fil/ScanApi/scanobj.h"
#include "../../Common.Fil/ScanApi/baseapi.h"

VOID AvpSleep(DWORD ms_);
#ifndef _INTERN_VER_
#include "../../Common.Fil/_avpio.h"
#include "../../Common.Fil/stuff/_CARRAY.H"

#include "LoadKeys.h"
extern ULONG functionality;
#endif
DWORD GetFileAttr(char* lpFileName);

unsigned char BinMBR[512]=
{
 0xFA,0x2B,0xC0,0x8E,0xD0,0x8E,0xC0,0x8E,0xD8,0xB8,0x00,0x7C,0x8B,0xE0,0xFB,0x8B,
 0xF0,0xBF,0x00,0x7E,0xFC,0xB9,0x00,0x01,0xF3,0xA5,0xE9,0x00,0x02,0xB9,0x10,0x00,
 0x8B,0x36,0x85,0x7E,0xF6,0x04,0x80,0x75,0x08,0x83,0xEE,0x10,0xE2,0xF6,0xEB,0x37,
 0x90,0xBF,0xBE,0x07,0x57,0xB9,0x08,0x00,0xF3,0xA5,0x5E,0xBB,0x00,0x7C,0x8B,0x14,
 0x8B,0x4C,0x02,0xBD,0x05,0x00,0xB8,0x01,0x02,0xCD,0x13,0x73,0x09,0x2B,0xC0,0xCD,
 0x13,0x4D,0x74,0x19,0xEB,0xF0,0xBE,0xFE,0x7D,0xAD,0x3D,0x55,0xAA,0x75,0x14,0xBE,
 0xBE,0x07,0xEA,0x00,0x7C,0x00,0x00,0x8B,0x36,0x87,0x7E,0xEB,0x0A,0x8B,0x36,0x89,
 0x7E,0xEB,0x04,0x8B,0x36,0x8B,0x7E,0xAC,0x0A,0xC0,0x74,0xFE,0xBB,0x07,0x00,0xB4,
 0x0E,0xCD,0x10,0xEB,0xF2,0xEE,0x7F,0x8D,0x7E,0xA7,0x7E,0xC8,0x7E,0x0D,0x0A,0x49,
 0x6E,0x76,0x61,0x6C,0x69,0x64,0x20,0x50,0x61,0x72,0x74,0x69,0x74,0x69,0x6F,0x6E,
 0x20,0x54,0x61,0x62,0x6C,0x65,0x00,0x0D,0x0A,0x45,0x72,0x72,0x6F,0x72,0x20,0x4C,
 0x6F,0x61,0x64,0x69,0x6E,0x67,0x20,0x4F,0x70,0x65,0x72,0x61,0x74,0x69,0x6E,0x67,
 0x20,0x53,0x79,0x73,0x74,0x65,0x6D,0x00,0x0D,0x0A,0x4D,0x69,0x73,0x73,0x69,0x6E,
 0x67,0x20,0x4F,0x70,0x65,0x72,0x61,0x74,0x69,0x6E,0x67,0x20,0x53,0x79,0x73,0x74,
 0x65,0x6D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x55,0xAA
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
/***********************************************/
/* check if name2 is a substr of name1         */
/* All '\' changed for '/'                     */
/* All names are changed to uppercase register */
/* Return value - 0 name2 is a substr of name1 */
/*                1 not                        */
/*               -1 error                      */
/***********************************************/
#define MAX_VOLUME 5
int CmpFileNames(char *name1,char *name2)
 {
  char *buf1=(char*)Malloc(CCHMAXPATH+1);
  char *buf2=(char*)Malloc(CCHMAXPATH+1);
  char *p=NULL;
  int len=0;
  static char vol[MAX_VOLUME+1],dir[256],
              name[_MAX_PATH+1],ext[CCHMAXPATH+1],dir1[256];

  if((buf1==NULL) || (buf2==NULL))
   {
    len=-1;
    goto _end;
   }
  strcpy(buf1,name1);
  strcpy(buf2,name2);

  strupr(buf1);
  strupr(buf2);
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

  _splitpath(buf1,vol,dir1,name,ext);
  if(dir1[0]!='/')
   {
    dir[0]='/';
    dir[1]=0x0;
    strcat(dir,dir1);
   }
  else
   strcpy(dir,dir1);
  _makepath(buf1,vol,dir,name,ext);

  _splitpath(buf2,vol,dir1,name,ext);
  if(dir1[0]!='/')
   {
    dir[0]='/';
    dir[1]=0x0;
    strcat(dir,dir1);
   }
  else
   strcpy(dir,dir1);
  _makepath(buf2,vol,dir,name,ext);

  len=strlen(buf1)-1;
  if(*(buf1+len)=='/') *(buf1+len)=0x0;
  len=strlen(buf2)-1;
  if(*(buf2+len)=='/') *(buf2+len)=0x0;

  len=strlen(buf2);
  if(len<=strlen(buf1))
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

  if(stricmp(buf1,buf2)==0) len=0;
  else                      len=1;
_end:
  if(buf1) free(buf1);
  if(buf2) free(buf2);
  return len;
 }
/*
//Функция для проверки соответствия строки упрощенному регулярному выражению
//Метасимволы шаблона:
//
//        * - любое число любых символов (0 и более);
//        ? - один любой символ.
//                      Усложнение:
//        [буквы]  - любая из перечисленных букв.
//        [!буквы] - любая из букв, кроме перечисленных.
//        [h-z]    - любая из букв от h до z включительно.

#define  U(c) ((c) & 0377)  // подавление расширения знака
#define  QUOT    '\\'       // экранирующий символ

// Проверить, что smy лежит между smax и smin
int _syinsy (unsigned smin, unsigned smy, unsigned smax)
 {
  char left[2],right[2],middle [2];

  left  [0]=(char)smin;left  [1]='\0';
  right [0]=(char)smax;right [1]='\0';
  middle[0]=(char)smy; middle[1]='\0';

  return (strcoll(left, middle) <= 0 && strcoll(middle, right) <= 0);
 }

// teststr - сопоставляемая строка
// mask    - шаблон. Символ \ отменяет спецзначение метасимвола.
int _match(register char *teststr,register char *mask)
 {
  register int    scc; // текущий символ строки
  int     c, cc, lc;   // lc - предыдущий символ в [...] списке
  int     ok, notflag;

  for(;;)
   {
    scc=U(*teststr++);          // очередной символ строки
    switch (c = U (*mask++))
     { // очередной символ шаблона
      case QUOT:          // a*\*b
        c = U (*mask++);
        if( c == 0 ) return(0); // ошибка: pattern\
        else goto def;
      case '[':   // любой символ из списка
        ok = notflag = 0;
        lc = 077777;      // достаточно большое число
        if(*mask== '!')
         { notflag=1; mask++; }

        while ((cc=U(*mask++))!=0)
         {
          if(cc == ']')
           {    // конец перечисления
            if (ok) break;      // сопоставилось
            return (0);     // не сопоставилось
           }
          if(cc == '-')
           {    // интервал символов
            if (notflag)
             {
              // не из диапазона - OK
              if (!_syinsy (lc, scc, U (*mask++))) ok++;
              // из диапазона - неудача
              else return (0);
             }
            else
             {
              // символ из диапазона - OK
              if (_syinsy (lc, scc, U (*mask++))) ok++;
             }
           }
          else
           {
            if (cc == QUOT)
             {      // [\[\]]
              cc = U(*mask++);
              if(!cc) return(0);// ошибка
             }
            if (notflag)
             {
              if (scc && scc != (lc = cc)) ok++; // не входит в список
              else return (0);
             }
            else
             {
              if(scc == (lc = cc)) ok++;// входит в список
             }
           }
         }
        if(cc == 0)
         {    // конец строки
          printf("Нет ]\n");
          return (0);        // ошибка
         }
        continue;
      case '*':   // любое число любых символов
        if(!*mask)  return (1);
        for(teststr--;*teststr;teststr++)
            if(match(teststr,mask)) return (1);
        return (0);
      case 0:
        return (scc == 0);
      default:
        if(c != scc) return (0);
        continue;
      case '?':   // один любой символ
        if (scc == 0) return (0);
        continue;
     }
   }
 }

int match(register char *teststr,register char *mask);
BOOL cmpFileMask(char* sTestFile)
 {
  if(prf_data.ExcludeFiles==1)
   {
    char s[85],*span;
    strcpy(s,prf_data.ExcludeMask);
    span=strtok((char*)s,";, ");
    while(span)
     {
      //printf("maskCompare %s %s\r\n",sTestFile,span);
      if(match(sTestFile,span)==1)
       {
        //printf("maskCompare пропущен %s (%s)\r\n",sTestFile,span);
        return TRUE;
       }
      span=strtok(NULL,";, ");
     }
   }
  return FALSE;
 }
*/
ULONG StrICmp(char *str1,char *str2)
 {//str 2 должна совпадать или быть подстрокай str1 (str2<=str1)
  char tstStr1[CCHMAXPATH+1],tstStr2[CCHMAXPATH+1];
  strcpy((char *)tstStr1,str1);
  strcpy((char *)tstStr2,str2);
  ULONG len;
  len=strlen((char *)tstStr1)-1;
  if(*(tstStr1+len)=='\\') *(tstStr1+len)=0x0;
  len=strlen((char *)tstStr2)-1;
  if(*(tstStr2+len)=='\\') *(tstStr2+len)=0x0;

  if(strlen((char *)tstStr2)>strlen((char *)tstStr1)) return 1;

  strupr((char*)tstStr1);
  strupr((char*)tstStr2);

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

  if(prf_data.ExcludeFiles==1)
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

_workStat workStat;

void iniWorkStat(void)
 {
  workStat.sect=0;
  workStat.fil=0;
  workStat.fold=0;
  workStat.arch=0;
  workStat.pack=0;
  workStat.CountSize=0;
  workStat.st.hour=workStat.st.minute=workStat.st.second=0;
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
  struct dosdate_t scandate;
  struct dostime_t scantime;

  _dos_getdate(&scandate);                       // получить текущую дату
  _dos_gettime(&scantime);

  workStat.cekForEach=scandate.day*86400+scantime.hour*3600+scantime.minute*60+scantime.second;
  workStat.sectForEach=0;
  workStat.filForEach=0;
  workStat.foldForEach=0;
  workStat.archForEach=0;
  workStat.packForEach=0;
  workStat.CountSizeForEach=0;
  workStat.stForEach.hour=workStat.stForEach.minute=workStat.stForEach.second=0;
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

ULONG MyWaitMessBox(CHAR *Mess,ULONG Param,BYTE skip_dlg)
 {
  ULONG RezMsg=-1;

  SetCursorOff(TRUE);
  _printf("\n%s",(char*)Mess);
  switch (Param)
   {
    case MB_OK:
      _puts("\n\n");
      _puts(LoadString(IDS_PRESS_KEY_L,"Press any key\n"));
      break;
    case MB_OKCANCEL:
      _puts(" - Ok/Cancel\n");
      break;
    case MB_YESNO:
      _puts(" - Yes/No\n");
      break;
   }
  if(skip_dlg!=1)
   switch (getch())
    {
     case 0x0d:
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
  SetCursorOff(FALSE);
  putch('\n');
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
size_t _wcslen( const wchar_t *string );
wchar_t *_wcscpy( wchar_t *strDestination, const wchar_t *strSource );
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
 { Free(__ptr); }
void *_malloc( size_t __size)
 { return Malloc(__size);}
size_t _wcslen(const wchar_t *string)
 {
  string=string;
  return 0;
 }
wchar_t *_wcscpy(wchar_t *strDestination, const wchar_t *strSource)
 {
  strDestination=strDestination; strSource=strSource;
  return 0;
 }

#ifndef _INTERN_VER_
#include "../../Common.Fil/AvpVndIDs.h"
#include "../../Common.Fil/sign\sign.h"
#include "../../Common.Fil/kldtser.h"

void** vndArray=NULL;
int vndArrayCount=0;

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
  if(!KLDT_Init_Library((void*(*)(unsigned int))Malloc,Free)) return FALSE;
  if(!DATA_Init_Library((void*(*)(unsigned int))Malloc,Free)) return FALSE;

  HDATA hRootData=NULL;
//  if(!KLDT_Deserialize( filename, &hRootData ))return FALSE;
  if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return FALSE;
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
  for( i=0; i<nCount; i++ )
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
#endif

BOOL load_avpbase(char* name)
 {
  BOOL ret=FALSE;
  char *curpath,*Folder;//[CCHMAXPATH];
  unsigned drives;

  curpath=getcwd(NULL,0);

  Folder=name;
  name=strrchr(Folder,'\\');
  if(name)
   {
    Folder[name-Folder]=0;
    name++;
   }
  else name=Folder;

  if(Folder[1]==':') _dos_setdrive(tolower(Folder[0])-'a'+1,&drives);
  chdir(Folder);

  int SetFile;
  if((SetFile=sopen(name,O_RDONLY|O_BINARY,SH_DENYNO,S_IWRITE))!=(-1))
   {
    char  *buf=0;
    unsigned z, zz;
    z=filelength(SetFile);
    if((buf=(char *)Malloc(z))!=0)
     {
      int KernFile;
      if(_dos_read(SetFile,(void *)buf,z,&zz)!=-1)
       if( (strstr(strupr(strtok(buf,"\r\n")),"KERNEL.AV")!=0)&&
         ( (KernFile=sopen(buf,O_RDONLY|O_BINARY,SH_DENYNO,S_IWRITE))!=(-1)) )
        {
         close(KernFile);

#ifndef _INTERN_VER_
         if(name && strlen(name))
          {
           char filename[0x200];
           char* n;
           strcpy(filename,name);
           n=AvpStrrchr(filename,'\\');
           if(n) n++;
           else n=filename;
           strcpy(n,"AVP.VND");
           VndInit(filename);
           //_printf("number of vnd keys=%d\r\n",vndArrayCount);
          }
#endif
         if(BAvpLoadBase(name))
          {
           putch('\r');
           _printf(LoadString(IDS_BASES_LOADED,"Antiviral databases were loaded. Known records: %d\n"),SurBase.RecCount);
           _puts("\n");

           //_rintf(LoadString(IDS_LAST_UPDATE,"Последнее обновление:  %02d.%02d.%04d,  %d вирусов.\n"),SurBase.LastUpdateDay,SurBase.LastUpdateMonth,SurBase.LastUpdateYear,SurBase.RecCount);
           //WinSetWindowText(hWndBTNTxt,loadStr);
           ret=TRUE;
          }
         else
          {
           putch('\r');
           _puts(LoadString(IDS_BASES_NOT_LOADED,"Loading of bases was not completed.\n"));
           _puts("\n");
          }
        }
       else _puts(LoadString(IDS_SET_DAMAGED,"kernel.avc is absent in the first string of avp.set file.\n"));
      Free(buf);
     }
    close(SetFile);
   }

  if(name!=Folder)
   {
    Folder[(--name)-Folder]='\\';
    name=Folder;
   }
  _dos_setdrive(tolower(curpath[0])-'a'+1,&drives);
  chdir(curpath);
//  Free(curpath);
  return ret;
 }

ULONG GetFlags(void)
{
 ULONG mFlags=0;
//  DWORD MFlags=tPage2.m_Mask;

// if(avpKeyFlag)
#ifndef _INTERN_VER_
 if(functionality)
  {
//   if(prf_data.Packed   &&(avpKey->Options&KEY_O_UNPACK))
//   if(prf_data.Archives &&(avpKey->Options&KEY_O_EXTRACT))
//   if(prf_data.MailBases&&(avpKey->Options&KEY_O_MAILBASES))
//   if(prf_data.MailPlain&&(avpKey->Options&KEY_O_MAILPLAIN))
//   if(prf_data.CodeAnalyser&&(avpKey->Options&KEY_O_CA))
   if(prf_data.Packed   &&(functionality & FN_UNPACK))  mFlags|=MF_PACKED;
   if(prf_data.Archives &&(functionality & FN_EXTRACT)) mFlags|=MF_ARCHIVED;
   if(prf_data.MailBases&&(functionality & FN_MAIL))    mFlags|=MF_MAILBASES;
   if(prf_data.MailPlain&&(functionality & FN_MAIL))    mFlags|=MF_MAILPLAIN;
   if(prf_data.CodeAnalyser&&(functionality & FN_CA))      mFlags|=MF_CA;
  }
#else
   if(prf_data.Packed)       mFlags|=MF_PACKED;
   if(prf_data.Archives)     mFlags|=MF_ARCHIVED;
   if(prf_data.MailBases)    mFlags|=MF_MAILBASES;
   if(prf_data.MailPlain)    mFlags|=MF_MAILPLAIN;
   if(prf_data.CodeAnalyser) mFlags|=MF_CA;
#endif
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

 if(prf_data.ExcludeFiles) mFlags|=MF_MASK_EXCLUDE;
//MFlags|=MF_ALLWARNINGS;
//MFlags|=MF_ALLENTRY;
 return mFlags;
}

BOOL ScanFile(char *fullFileName,unsigned long fileSize,unsigned long mFlags)
 {
  AVPScanObject ScanObject;

  memset((void*)&ScanObject,0,sizeof(AVPScanObject));
  strcpy(ScanObject.Name,fullFileName);
  ScanObject.Type=OT_FILE;
  ScanObject.MFlags=mFlags;//MF_MASK_ALL|mFlags;
  ScanObject.Size=fileSize;//cbFile;
  return (BOOL)BAvpProcessObject(&ScanObject);
 }

//typedef struct _DirList
// {
//  CHAR    szDirName[CCHMAXPATH];  // Found file's path & name
//  struct  _DirList  *pNextDir;    // Pointer of next file
// } DIRLIST;
//typedef DIRLIST FAR *PDIRLIST;

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct find_t FIND_T;

void srcTree(CHAR *CurrDir,ULONG mFlags)
 {
#ifndef WIN32
  FIND_T ffblk,tmpMffblk;
#else
  HANDLE hf;
  WIN32_FIND_DATA fd,fdTmp;
#endif
//#ifdef WIN32
//  fd.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM;
//  hf = FindFirstFile(CurrDir,&fd);
//  if(hf != INVALID_HANDLE_VALUE)
//#else
//  if(_dos_findfirst(CurrDir,_A_RDONLY|_A_HIDDEN|_A_SYSTEM|_A_ARCH,&ffblk) == 0)
//#endif
//   {
//#ifdef WIN32
//    if ( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
//#else
//    if ( !(mffblk.attrib&0x10) )
//#endif
//     check_file(CurrDir); //File found
//   }
//#ifdef WIN32
//  FindClose(hf);
//#else
//  _dos_findclose(&ffblk);
//#endif

  static char  szFilename[CCHMAXPATH+1],szFullFileName[CCHMAXPATH+1];
//  PDIRLIST pTail=NULL,pDir;

  strcpy(szFilename,CurrDir);
  if(szFilename[strlen(szFilename)-1]=='\\') szFilename[strlen(szFilename)-1]=0;

  if(cmpDirMask(szFilename)==TRUE) return;

  strcat(szFilename,"\\*.*");

  incWorkStat(INCL_FOLDER);
#ifdef WIN32
  fd.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_DIRECTORY;
  hf = FindFirstFile(szFilename,&fd);
  if(hf != INVALID_HANDLE_VALUE)
#else
//  if(_dos_findfirst(szFilename,0x37,&mffblk) == 0)
  if(_dos_findfirst(szFilename,_A_SUBDIR|_A_ARCH|_A_SYSTEM|_A_HIDDEN|_A_RDONLY,&ffblk)==0)
#endif
   {
    do
     {
#ifdef WIN32
      if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
#else
      if(ffblk.attrib&0x10)
#endif
       {
#ifdef WIN32
        if(strcmp(fd.cFileName,".") && strcmp(fd.cFileName,"..") )
#else
        if(strcmp(ffblk.name,".") && strcmp(ffblk.name,"..") )
#endif
         {
          if(prf_data.SubDirectories)
           {
            char tempDir[CCHMAXPATH+1];
            if(CurrDir[strlen(CurrDir)-1] == '\\')
             {
#ifdef WIN32
#ifdef SHORT_NAME
              if(CurrDir,fd.cAlternateFileName[0] != 0)
               sprintf(tempDir,"%s%s",CurrDir,fd.cAlternateFileName);
              else
#endif
               sprintf(tempDir,"%s%s",CurrDir,fd.cFileName);
#else
              sprintf(tempDir,"%s%s",CurrDir,ffblk.name);
#endif
             }
            else
             {
#ifdef WIN32
#ifdef SHORT_NAME
              if(CurrDir,fd.cAlternateFileName[0] != 0)
               sprintf(tempDir,"%s\\%s",CurrDir,fd.cAlternateFileName);
              else
#endif
               sprintf(tempDir,"%s\\%s",CurrDir,fd.cFileName);
#else
              sprintf(tempDir,"%s\\%s",CurrDir,ffblk.name);
#endif
             }
#ifdef WIN32
            memcpy(&fdTmp,&fd,sizeof(WIN32_FIND_DATA));
            if(scan_goes) srcTree(tempDir,mFlags);
            memcpy(&fd,&fdTmp,sizeof(WIN32_FIND_DATA));
#else
            memcpy(&tmpMffblk,&ffblk,sizeof(FIND_T));
            if(scan_goes) srcTree(tempDir,mFlags);
            memcpy(&ffblk,&tmpMffblk,sizeof(FIND_T));
#endif
           }
         }
       }
      else
       {
        if(CurrDir[strlen(CurrDir)-1] == '\\')
         {
#ifdef WIN32
#ifdef SHORT_NAME
          if(CurrDir,fd.cAlternateFileName[0] != 0)
           sprintf(szFullFileName,"%s%s",CurrDir,fd.cAlternateFileName);
          else
#endif
           sprintf(szFullFileName,"%s%s",CurrDir,fd.cFileName);
#else
          sprintf(szFullFileName,"%s%s",CurrDir,ffblk.name);
#endif
         }
        else
         {
#ifdef WIN32
#ifdef SHORT_NAME
          if(CurrDir,fd.cAlternateFileName[0] != 0)
           sprintf(szFullFileName,"%s\\%s",CurrDir,fd.cAlternateFileName);
          else
#endif
           sprintf(szFullFileName,"%s\\%s",CurrDir,fd.cFileName);
#else
          sprintf(szFullFileName,"%s\\%s",CurrDir,ffblk.name);
#endif
         }
        // File found
        if(scan_goes) ScanFile(szFullFileName,ffblk.size,mFlags);
       }
#ifdef WIN32
     } while(scan_goes&&(FindNextFile(hf,&fd)!=0));
#else
     } while(scan_goes&&(_dos_findnext(&ffblk)==0));
#endif
   }
#ifdef WIN32
  FindClose(hf);
#else
  _dos_findclose(&ffblk);
#endif
 }

//          if(prf_data.SubDirectories)
//           {
//            //if(!DosSetCurrentDir(FindBuffer.achName))
//            // {
//              if(pTail==NULL)
//               {
//                pDir=(PDIRLIST)Malloc(sizeof(DIRLIST));
//                pTail=pDir;
//               }
//              else
//               {
//                pDir->pNextDir=(PDIRLIST)Malloc(sizeof(DIRLIST));
//                pDir=pDir->pNextDir;
//               }
//              strcpy(pDir->szDirName,ffblk.name);
//              pDir->pNextDir=NULL;
//            // }
//            //else // unable to cd!
//             //{ }
//           }
//         }
//        else
//         {
//          if(CurrDir[strlen(CurrDir)-1]=='\\')
//           sprintf(szFullFileName,"%s%s",CurrDir,ffblk.name);
//          else
//           sprintf(szFullFileName,"%s\\%s",CurrDir,ffblk.name);
//
//          if(scan_goes) ScanFile(szFullFileName,ffblk.size,mFlags);
//         }
//     } while(scan_goes&&(_dos_findnext(&ffblk)==0));
//    _dos_findclose(&ffblk);
//   }
//  if((prf_data.SubDirectories!=0)&&(pTail!=NULL))
//   {
//    pDir=pTail;
//    do
//     {
//      char tempDir[CCHMAXPATH+1];
//      if(CurrDir[strlen(CurrDir)-1]=='\\')
//       sprintf(tempDir,"%s%s",CurrDir,pDir->szDirName);
//      else
//       sprintf(tempDir,"%s\\%s",CurrDir,pDir->szDirName);
//      if(scan_goes) srcTree(tempDir,mFlags);
//      pTail=pDir;
//      pDir=pDir->pNextDir;
//      Free(pTail);
//     } while(pDir != NULL);
//   }

// This code inside SCAN THREAD!
CheckBoot(CHAR *path,DWORD type)
 {
  AVPScanObject ScanObject;
  if(path[1]!=':')return 0;
  CHAR Name[CCHMAXPATH];
  sprintf(Name,"%s%s",LoadString(IDS_BOOT_SECTOR,"Boot sector of drive "),path);//&path[2]);

  memset((void*)&ScanObject,0,sizeof(AVPScanObject));
  ScanObject.SType=OT_BOOT;
  ScanObject.MFlags=GetFlags();//tPage2.Mask;

  strcpy(ScanObject.Name,Name);
  ScanObject.Drive=(BYTE)((path[0]|0x20)-'a');
  switch(type)
   {
    case IDI_NET_ON:  //DRIVE_REMOTE:
     ScanObject.Disk=0x20; break;
//    case DRIVE_RAMDISK:
//     ScanObject.Disk=0x40; break;
    case IDI_CD:     //DRIVE_CDROM:
     ScanObject.Disk=0x60; break;
    case IDI_LOCAL1: //case DRIVE_FIXED:
     ScanObject.Disk=0x80; break;
    case IDI_FLOPPY_12:
    case IDI_FLOPPY_144:  // DRIVE_REMOVABLE:
     ScanObject.Disk=ScanObject.Drive;       break;
    default:
     return 0;
  }
  return BAvpProcessObject(&ScanObject);
}

typedef BOOL (*lp_AvpRead13 )(BYTE Disk, WORD Sector ,BYTE Head ,WORD NumSectors,BYTE *Buffer);
CheckMBRS()
 {
  DWORD ret=0;
  ULONG nDisks=0;

  //if(AvpRead13)
    for(;nDisks<16;nDisks++)
     {
      BYTE Buffer[0x200];
      if(!AvpRead13((BYTE)(0x80+nDisks),1,0,1,Buffer)) break;
     }
//  if(DosPhysicalDisk(INFO_COUNT_PARTITIONABLE_DISKS,(PVOID)&nDisks,2,NULL,0)==0)
   for(int i=0;i<nDisks;i++)
    {
     CHAR Name[CCHMAXPATH];
     AVPScanObject ScanObject;
     memset((void*)&ScanObject,0,sizeof(AVPScanObject));
     ScanObject.SType=OT_MBR;
     ScanObject.MFlags=GetFlags();//tPage2.Mask;
//   ScanObject.MFlags|=MF_ALLWARNINGS;
     ScanObject.MFlags|=MF_ALLENTRY;
     ScanObject.Drive=2;

     sprintf(Name,LoadString(IDS_MBR_SECTOR,"Master Boot Record of HDD%d"),i+1);
     strcpy(ScanObject.Name,Name);
     ScanObject.Disk=(BYTE)(0x80+i);

     ret&=BAvpProcessObject(&ScanObject);
     if(scan_goes==0)break;
    }
  return ret;
 }

DWORD GetFileAttr(char* lpFileName);
BOOL  SetFileAttr(char* lpFileName,WORD FileAttr);
void  GetTraceLines(void);
UINT ScanAllPaths()
 {
  struct stat statbuf;
  CHAR  s[CCHMAXPATH],*curpath;
  PLIST pList;
  UINT  ret=0;
  ULONG mFlags;

  reportInit();

  if(!(*cmdline.ListFile||(BOOL)IfSelectRecords(1) ) ) //!clStart)
   {
    if(prf_data.OtherMessages)
     avp_mess(AVP_CALLBACK_MB_OK,(ULONG)LoadString(IDS_NOTHING_LOCATION_L,"Nothing to scan.\x0d\x0aYou should select at least one directory to scan."));
    f_help=1;
   }
  else
   if((prf_data.Memory==0)&&(prf_data.Sectors==0)&&(prf_data.Files==0))
    {
     if(prf_data.OtherMessages)
      avp_mess(AVP_CALLBACK_MB_OK,(ULONG)LoadString(IDS_NOTHING_FILES_L,"Nothing to scan.\x0d\x0aYou should select Files and/or Sectors in *.prf file."));
     f_help=1;
    }
   else
//    if(prf_data.InfectedAction==3)
//     {
//      if(avp_mess(MPFROMLONG(AVP_CALLBACK_MB_OKCANCEL),LoadString(IDS_SHURE_DELETEALL,"Вы уверены в том, что Вы хотите УДАЛИТЬ ВСЕ зараженные объекты?"))==DID_OK)
//       goto scanstart;
//      else
//       WinSendMsg(hWndNBook,BKM_TURNTOPAGE,(MPARAM)realPageInfo[2].ulPageID,NULL);
//     }
//    else
    {
     scan_goes=1;
     ExitCode=0;
     ApplyToAll=0;
     MethodDelete=0;
     SectorWarn=0;
     mFlags=GetFlags();

     BAvpAddExcludeMask(NULL);
     if(prf_data.ExcludeFiles==1)
      {
       char s[85],*span;
       strcpy((char *)s,prf_data.ExcludeMask);
       span=strtok((char*)s,";, ");
       while(span)
        {
         BAvpAddExcludeMask(strupr(span));
         span=strtok(NULL,";, ");
        }
      }

     //CHAR slid[30];
     //printf(slid,"%d \n\n",SliderScan());
     if(prf_data.FileMask==3)
      {
       BAvpAddMask(NULL);
       strcpy(s,strupr(prf_data.UserMask));
       CHAR *span;
       span=strtok((char*)s,";, ");
       while(span)
        {
         if(strlen(span)<13) BAvpAddMask(span); //strlwr(span)
         span=strtok(NULL,";, ");
        }
      }
    //ScanObject.mode=prf_data.FileMask;

     VirCountArray.RemoveAll();
     iniWorkStat();

     if(prf_data.Memory)
      {
       //GetTraceLines();

       AVPScanObject ScanObject;
       memset((void*)&ScanObject,0,sizeof(AVPScanObject));
       ScanObject.SType=OT_MEMORY;
       ScanObject.MFlags=GetFlags();//tPage2.Mask;
       strcpy(ScanObject.Name,LoadString(IDS_MEMORY,"Memory"));
       BAvpProcessObject(&ScanObject);
       if(scan_goes==0) goto ret;
      }

     BOOL MBRS;
     // -- Begin of Sectors Check --
     if(prf_data.Sectors)
      { // check BOOT
       if(!cmdline.skip_boot)
        {
         MBRS=FALSE;
         CHAR Path[]=" :\\";
         if(pTail!=NULL)
          {
           pList=pTail;
           do
            {
             if(pList->Selected==1)
              {
               if(pList->szName[1]==':')
                {
                 Path[0]=pList->szName[0];
                 DWORD type;
                 type=DiskType(Path);
                 switch(type)
                  {
                   case IDI_LOCAL1:   //case DRIVE_FIXED:
                     if(scan_goes==0)goto ret;
                     CheckBoot(Path,type);
                     MBRS=TRUE;
                   default: break; //All other types check later.
                  }
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
      } // -- End of Sectors Check ----

     // -- Check files by list -----
     if(*cmdline.ListFile)
      {
       char OldFileMask=prf_data.FileMask;
       prf_data.FileMask=2;

       int hListFile;
       ULONG Len,ulFilesize=0;      //новая длина существующего файла
       CHAR *buf,*fil;//,scandir[CCHMAXPATH];
       if((hListFile=open(cmdline.ListFile,O_RDONLY|O_BINARY))!=(-1))
        {
         fstat(hListFile,&statbuf);
         if((buf=(CHAR*)Malloc(statbuf.st_size))!=0)
          {
           if((Len=read(hListFile,buf,statbuf.st_size))!=(-1))
            {
             if(Len<statbuf.st_size) buf[Len]=0;
             else buf[statbuf.st_size]=0;
             fil=strtok((char*)buf,"\r\n");
             while(fil)
              {
               stat(fil,&statbuf);
               if(!(GetFileAttr(fil) & FILE_DIRECTORY)) //file
                {
                 // варианты : *.*,  *:\..., *.*\*.*, ..\*.*
                 strupr((char*)fil); //в верхн регистр
                 //ScanObject.drive=*_tpth-'A';
                 ////DosQueryCurrentDir(0,curpath+1);
                       //strcpy(scandir,GNAM(fil)); //выделяем директорию
                       //*GNAM(fil)=0;            //получаем ?:\\
                       //if(fil[strlen(fil)-1]=='\\') fil[strlen(fil)-1]=0;
                       //DosSetDefaultDisk(fil[0]-'A'+1);
                       //DosSetCurrentDir(scandir);
                 //*found_name=fil[0];
                 //*(found_name+1)=':';
                 //*(found_name+2)='\\';
                 //if(!(!avpKeyFlag&&!(Disk[toupper(*found_name)-'A']) //нет кея и нет диска
                 //  ||avpKeyFlag&&!(avpKey->Options&KEY_O_REMOTESCAN)//есть кей но нет скан сети
                 //  &&!(Disk[toupper(*found_name)-'A'])) )
                 // {
                 //  getcurdir(0,found_name+3);
                 //  if(found_name[strlen(found_name)-1]!='\\') strcat(found_name,"\\");

                 //  strcat(found_name,tname);
                 //  strcpy(ScanObject.name,tname);
                 //  *make_name_ptr=0;

                 //  if(scan_goes) srcTree(Name,mFlags);
                 ScanFile(fil,statbuf.st_size,mFlags);
                 //  chdir(curpath);
                 // }
                }
               fil=strtok(NULL,"\r\n");
              }
            }
           Free(buf);
          }
         close(hListFile);
         if(cmdline.KillList==1)
          {
           //SetFileAttr(cmdline.ListFile,(WORD)GetFileAttr(cmdline.ListFile&(~_A_RDONLY));
           unlink(cmdline.ListFile);
           //remove(cmdline.ListFile);
          }
        }
       prf_data.FileMask=OldFileMask;
      }

     if((pTail!=NULL)&&(*cmdline.ListFile==0))
      {
       curpath=getcwd(NULL,0);
       pList=pTail;
       do
        {
         if(pList->Selected==1)
          {
           if((strlen(pList->szName)==2)&&(pList->szName[1]==':')&&
              (tolower(pList->szName[0])>='a') && (tolower(pList->szName[0])<='z') )
            strcat(pList->szName,"\\");
nextex:    if(stat(pList->szName,&statbuf)==0) //if(ExistDisk(pList->szName[0])!=0)
            {
             //if(statbuf.st_attr==(DWORD)-1)
             // {
             //  if(Path.Mid(1)!=":")goto nextdisk;
             // }
             if(GetFileAttr(pList->szName) & FILE_DIRECTORY)
              {
begin:         if(prf_data.Sectors && pList->szName[1]==':'&&!cmdline.skip_boot&&scan_goes)
                switch(pList->szType)
                 {
//                  case DRIVE_RAMDISK:
     //             case DRIVE_CDROM:
                  case IDI_FLOPPY_12:
                  case IDI_FLOPPY_144:  // DRIVE_REMOVABLE:
                    if(scan_goes==0)goto ret;
                    CheckBoot(pList->szName,pList->szType);
                  case IDI_LOCAL1:   //case DRIVE_FIXED: //Already checked.
                  default:
                  break;
                 }
                if(scan_goes==0)goto ret;
                if(prf_data.Files)
                 {
                  if(ExistDisk(pList->szName[0])==0)
                   {
                    //DosSetDefaultDisk(pList->szName[0]-'a'+1);
                    if(toupper(pList->szName[0])>='A'&&toupper(pList->szName[0])<='Z'
                      &&(pList->szName[1]==':')&&(strlen(pList->szName)==2))
                     strcat(pList->szName,"\\");
                    iniWorkStatForEach();
                    srcTree(pList->szName,mFlags);
                    if(prf_data.RepForEachDisk==1) reportForEach(pList->szName);
                   }
                  if(scan_goes==0)goto ret;

//                 if(size==1 && (type==DRIVE_REMOVABLE || type==DRIVE_CDROM))
                  if((strlen(pList->szName)<=3)&&(pList->szName[1]==':')&&
                    (tolower(pList->szName[0])>='a') && (tolower(pList->szName[0])<='z')&&
                    ((pList->szType==IDI_FLOPPY_12)||(pList->szType==IDI_CD)||(pList->szType==IDI_FLOPPY_144))
                    &&(!cmdline.one_floppy))
                   {
                    if( (flScanAll==FALSE)&&(DID_OK==MyWaitMessBox(
                       ((pList->szType==IDI_CD)?
                         LoadString( IDS_NEXT_CDROM,"Insert next CDROM disk..." ):
                         LoadString(IDS_NEXT_FLOPPY,"Insert next floppy disk...")),MB_OKCANCEL,0)))
                     {
                      //if(prf_data.Files) CountSpace(pList->szName,&tPage5.m_TotalSize,&scan_goes);
                      //continue;
                      goto begin;
                     }
                   }
                 }
              } //директория
             else
              {
               if(scan_goes==0)goto ret;
               ScanFile(pList->szName,statbuf.st_size,mFlags);
               if(scan_goes==0)goto ret;
              } //fail
            } //stat==0
           else
            {
             //if(size==1 && (type==DRIVE_REMOVABLE || type==DRIVE_CDROM))
             if((strlen(pList->szName)<=3)&&(pList->szName[1]==':')&&
               (tolower(pList->szName[0])>='a') && (tolower(pList->szName[0])<='z')&&
               ((pList->szType==IDI_FLOPPY_12)||(pList->szType==IDI_CD)||(pList->szType==IDI_FLOPPY_144)) )
              {
               if((prf_data.ScanRemovable==1)&&(flScanAll==FALSE)&&(DID_OK==MyWaitMessBox(
                 ((pList->szType==IDI_CD)?
                   LoadString(IDS_NEXT_CDROM,"Insert next CDROM disk..." ):
                   LoadString(IDS_NEXT_FLOPPY,"Insert next floppy disk...")),MB_OKCANCEL,0)))
                goto nextex;
              }
            }
           } // if(Disk[_ddd]&DISK_SELECTED)
          pList->Selected=FALSE;
          pList=pList->pNext;
//nextsel: ;
         } while((pList != NULL)&&scan_goes);
       unsigned drives;
       _dos_setdrive(curpath[0]-'a'+1,&drives);
       chdir((char*)curpath);
       //Free(curpath);
      }
    }
ret:
//  if(clListDelete && !(scan_goes==0))
//   {
//    DeleteFile(clList);
//    clListDelete=0;
//   }
//  clStart=0;
//  ScanList.RemoveAll();
  // -- End of Files Check ----
  if(workStat.susp) ExitCode=3;
  if(workStat.warn) ExitCode=2;
  if(workStat.vb)
   if(workStat.vb == workStat.df) ExitCode=5;
     else ExitCode=4;
  if((scan_goes==1)&&(flgScan==1)) flgScan=0;
  scan_goes=0;
  reportDone();
//   WinPostMsg(hWndClient,WM_COMMAND,MPFROM2SHORT(ID_BTN,0),
//     MPFROM2SHORT(CMDSRC_PUSHBUTTON,TRUE));

  return ret;
 }

//------------- -----------
#define inportb  inp
#define outportb outp
void sound(unsigned short chast,unsigned long delay) // frequency Hz, delay microseconds
 {
  //При проверке в фоновом окне под Windows 9x AVPDOS32 может зависнуть при выдаче звукового
  //сигнала. Это моя бага. Нужно будет найти слабое место.
  unsigned char port61;
  union REGPACK regs; //REGS regs;
  //ULONG  time, time1;

  memset(&regs,0,sizeof(REGPACK));
  //reg.w.ax = 0;
  //intr(0x1A,&reg);

  //((unsigned short*)(&time))[0] = reg.w.dx;
  //((unsigned short*)(&time))[1] = reg.w.cx;

  //time1 = time;
  //time ++;
  //while( time1 != time )
  // {
  //  memset(&reg,0,sizeof(REGPACK));
  //  reg.w.ax = 0;
  //  intr(0x1A,&reg);
  //  ((unsigned short*)(&time1))[0] = reg.w.dx;
  //  ((unsigned short*)(&time1))[1] = reg.w.cx;
  // }

  port61=(UCHAR)inportb(0x61);
  outportb(0x61,port61|3);               // Enable sound
  outportb(0x43,0xB6);                   // Enable sound

  outportb(0x42,((unsigned char*)(&chast))[0]);  // Enable sound
  outportb(0x42,((unsigned char*)(&chast))[1]);  // Enable sound

  //time ++;
  //while(time1!=time)
  // {
  //  memset(&reg,0,sizeof(REGPACK));
  //  reg.w.ax = 0;
  //  intr(0x1A, &reg);
  //  ((unsigned short*)(&time1))[0] = reg.w.dx;
  //  ((unsigned short*)(&time1))[1] = reg.w.cx;
  // }

  regs.w.cx=((unsigned short*)(&delay))[1];
  regs.w.dx=((unsigned short*)(&delay))[0];
  regs.h.ah=0x86;
  intr(0x15,&regs);

  outportb(0x61,port61&0xFC);  // Disablee sound
 }

void ObjectMessage(AVPScanObject* ScanObject,const char* Message,UCHAR AttrMess)
 {
  CHAR LogStr[0x210];
  if(ScanObject!=NULL)
   {
    putch('\r');
    if(ScanObject->Entry)
     {
      char tMess[0x50];
      strcpy(tMess,(char*)Message);

      _printf(LoadString(IDS_ENTRY_FORMAT,"%s  Entry #%d\t"),ScanObject->Name,ScanObject->Entry);
      _puts("\t");
      if(prf_data.Report)
       {
        sprintf(LogStr,LoadString(IDS_ENTRY_FORMAT,"%s  Entry #%d\t"),ScanObject->Name,ScanObject->Entry);
        reportPutS(LogStr);
       }
      strcpy((char*)Message,tMess);
      //sprintf(LogStr,LoadString(IDS_ENTRY_FORMAT,"%s  Entry #%d"),ScanObject->Name,ScanObject->Entry);
     }
    else
     {
      if((strlen(ScanObject->Name)<54)&&(prf_data.LongStrings==0))
       {
        sprintf(LogStr,"%s\t",ScanObject->Name);
       }
      else
       {
        strncpy(LogStr,ScanObject->Name,24);
        LogStr[24]=0;
        strcat(LogStr," ... ");
        strcat(LogStr,&ScanObject->Name[strlen(ScanObject->Name)-25]);
        strcat(LogStr,"\t");
       }
      _puts(LogStr);
      if(prf_data.Report) reportPutS(ScanObject->Name);
     }

    if(Message!=NULL)
     {
      //strcat(LogStr,"\t");
      //strcat(LogStr,(char*)Message);
      //strcat(LogStr,"\r\n");
      //_puts((char*)Message);
      PutStrAttr((char*)Message,AttrMess);
      PutStrAttr("\n",attr);
      if(prf_data.Report)
       {
        sprintf(LogStr,"\t%s\r\n",(char*)Message);
        reportPutS(LogStr);
       }
     }
//    if(strlen(LogStr)!=0)
//     {
//      putch('\r');
//      _puts(LogStr);
//      reportPutS(LogStr);
//     }
   }
 }

BOOL CopyToFolder(char *Name,char *Folder)
 {
  static BOOL flCreate=TRUE;
  static BOOL flCopy=TRUE;
  static BOOL flCurrent=FALSE;
  CHAR  Folderpath[CCHMAXPATH],*curpath;
  int i;
  APIRET rc;
  unsigned drives;
  curpath=getcwd(NULL,0);
  if(Folder[1]==':')
   {
    _dos_setdrive(tolower(Folder[0])-'a'+1,&drives);
    _dos_getdrive(&drives);
    if(drives!=(tolower(Folder[0])-'a'+1))
      MyWaitMessBox(LoadString(IDS_DISK_NOT_FOUND_L,"Disk for infected (suspicious) files doesn't exist."),MB_OK,cmdline.skip_dlg);
   }
  else
   {
    strcpy(Folderpath,prgPath);
    if(Folderpath[strlen(Folderpath)-1]=='\\') Folderpath[strlen(Folderpath)-1]=0;
    _dos_setdrive(tolower(Folderpath[0])-'a'+1,&drives);
    chdir(Folderpath);
   }
  if(chdir(Folder))
   {
    if(flCreate)
     if(DID_OK_YES==MyWaitMessBox(LoadString(IDS_FOLDER_CREATE,"Folder for infected (suspicious) files doesn't exist.\nCreate folder?"),MB_YESNO,cmdline.skip_dlg))
      {
       mkdir(Folder);
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
   {
    _dos_setdrive(tolower(prgPath[0])-'a'+1,&drives);
    chdir(prgPath);
   }
  if(flCopy==TRUE)
   {
    //CHAR NewName[CCHMAXPATH];
    //for(i=0;(i<strlen(Name))&&(Name[i]!='/');i++) ;
    //strncpy(NewName,Name,i);
    //NewName[i]=0;
    //for(;(i>=0)&&(NewName[i]!='\\');i--);
    //rc=DosCopy(NewName,&NewName[i+1],DCPY_EXISTING|DCPY_FAILEAS);
    for(i=strlen(Name);(i>=0)&&(Name[i]!='\\');i--);
    rc=copyfile(Name,&Name[i+1]);
   }
  _dos_setdrive(curpath[0]-'a'+1,&drives);
  chdir(curpath);
//  Free(curpath);
  return TRUE;
 }

LONG avp_mess(DWORD mp1,DWORD mp2)
 {
  AVPScanObject* ScanObject;//=(AVPScanObject*)lParam;
  char s[0x200],tempb[0x200];
  char *np,Chr;
  LONG ret=0;


  if(cmdline.abort_disable!=1)
   if(kbhit())
    if(getch()==27)
     if(scan_goes!=0)
      {
       _puts("\n");
       _puts(LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
       _puts(" (Yes/No)");
       SetCursorOff(TRUE);
       Chr=(char)getch();
       _puts("\n");
       if((Chr=='y')||(Chr=='Y'))
        {
         prf_data.ShowOK=0;
         ExitCode=1;
         scan_goes=0;
         BAvpCancelProcessObject(1);
        }
       SetCursorOff(FALSE);
      }

  switch(LONGFROMMP(mp1))
   {
    case AVP_CALLBACK_PUT_STRING:
      _puts((char*)mp2); _puts("\r\n");
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
         _puts("\r                                                                           \r");
         _printf(LoadString(IDS_LOADBASE_FORMAT,"Loading %s %d%%"),lps->curName,lps->curPos*100/lps->curLength);
         putch(' ');// \r');
        }
      break;
    case AVP_CALLBACK_LINK_NAME:         //0x0109 const char* sourcename DEBUG!
      //_puts("LINK_NAME        \n:");
      break;
    case AVP_CALLBACK_ASK_DELETE:        //0x010A ScanObject* (For DELETE method) 2-delete; other-skip
      if(MethodDelete) return MethodDelete;
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_ASK_DELETE,"Disinfection of %s \x0d\x0ainfected by virus %s is impossible.\x0d\x0a\x0d\x0aDelete this object permanently?"),ScanObject->Name,
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
//      if(cmpFileMask((char*)mp2)==TRUE) return 17;
      if(SelfScan==0)
       {
        _puts("\r                                                      \r");
        if(strlen((char*)mp2)<50)
          _puts((char*)mp2);
        else
         {
          char LogStr[56];
          strncpy(LogStr,(char*)mp2,24);
          LogStr[24]=0;
          strcat(LogStr," ... ");
          strcat(LogStr,&((char*)mp2)[strlen((char*)mp2)-25]);
          _puts(LogStr);
         }
        _puts("  ");
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
      _puts("ERROR_SYMBOL_DUP \n");
      break;
    case AVP_CALLBACK_ERROR_SYMBOL:      //0x0407  char* symbolname
      _puts("ERROR_SYMBOL     \n");
      break;
    case AVP_CALLBACK_ERROR_FIXUPP:      //0x0408  int fixuppcode
      _puts("ERROR_FIXUPP     \n");
      break;
    case AVP_CALLBACK_OBJECT_DONE:       //0x0200 ScanObject*
     ScanObject=(AVPScanObject*)mp2;

     if(!ScanObject->Handle)
      {
//       DWORD fs=GetCompressedFileSize(ScanObject->Name,NULL);
//       if(fs==(DWORD)(-1))
         workStat.CountSize+=ScanObject->Size/1024+1;
         workStat.CountSizeForEach+=ScanObject->Size/1024+1;
//       else workStat.CountSize+=fs/1024+1;
      }

     if(ScanObject->RFlags & RF_INTERFACE_FAULT)
      {
       ObjectMessage(ScanObject,LoadString(IDS_INTERFACE_FAULT,"intrface fault.") ,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_KERNEL_FAULT)
      {
       ObjectMessage(ScanObject,LoadString(IDS_KERNEL_FAULT,"kernel fault.")  ,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }

     if(ScanObject->RFlags & RF_NOT_A_PROGRAMM)
      {
//       ObjectMessage(ScanObject,"RF_NOT_A_PROGRAMM.");
       break;
      }
     if(ScanObject->RFlags & RF_NOT_OPEN)
      {
       if(prf_data.ShowOK)
        {
         ObjectMessage(ScanObject,LoadString(IDS_OBJ_IOERROR,"I/O error.") ,attr);
         ScanObject->RFlags|=RF_REPORT_DONE;
         //getch();
        }
       return scan_goes;
      }
     if(ScanObject->SType==OT_FILE && !ScanObject->InPack) incWorkStat(INCL_FILES);
     if(ScanObject->SType & OT_SECTOR) incWorkStat(INCL_SECTORS);

     if(ScanObject->RFlags & RF_REPORT_DONE) return scan_goes;

     if(ScanObject->RFlags & RF_DISK_OUT_OF_SPACE)
      {
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_DISKOUTOFSPACE,"disk out of space.") ,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_CORRUPT)
      {
       incWorkStat(INCL_CORRUPTED);
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_CORRUPTED,"corrupted.") ,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_IO_ERROR)
      {
       incWorkStat(INCL_IOERRORS);
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_IOERROR,"I/O error.") ,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_CURE_FAIL)
      {
       sprintf(s,LoadString(IDS_OBJ_DISINF_FAILED,"disinfection failed: %s"),BAvpMakeFullName(tempb,ScanObject->VirusName));
       ObjectMessage(ScanObject,s,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(ScanObject->RFlags & RF_PROTECTED)
     {
//     if(prf_data.ShowOK)
       ObjectMessage(ScanObject,LoadString(IDS_OBJ_PROTECTED,"password protected."),attr);
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
         ObjectMessage(ScanObject,LoadString(IDS_OBJ_OK,"ok.") ,attr);
       ScanObject->RFlags|=RF_REPORT_DONE;
      }
     if(SelfScan && (ScanObject->RFlags & RF_DETECT))
      {
//       WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,(PSZ)LoadString(IDS_NEEDTORESTART,""),
//         (PSZ)"AVP for DOS32",1,MB_OK|MB_APPLMODAL|MB_MOVEABLE);
       BAvpCancelProcessObject(1);
       scan_goes=0;
       ExitCode=7;
//       WinPostMsg(hWndClient,WM_CLOSE,NULL,NULL);
      }
     return scan_goes;
//   break;
    case AVP_CALLBACK_OBJECT_MESS:       //0x0201 ScanObject* (ScanObject->VirName is *message)
      ScanObject=(AVPScanObject*)mp2;
      ObjectMessage(ScanObject,ScanObject->VirusName,attr);
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
             if(ScanObject->SType==OT_FILE)
               sprintf(dd.m_sObject,"%s",LoadString(IDS_DISINF_OFILE,"File"));
             if(ScanObject->SType & OT_SECTOR)
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
               sound(1193180/880,30000); //1193180/880
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
             if(DID_OK_YES==MyWaitMessBox(LoadString(IDS_APPLY_TOALL,"Apply to all infected objects"),MB_YESNO,cmdline.skip_dlg))
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
     if(ret && !SectorWarn && (ScanObject->SType & OT_SECTOR))
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
     workStat.kvForEach++;
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
        if(prf_data.Sound) sound(1193180/1760,30000);
        ScanObject=(AVPScanObject*)mp2;
        sprintf(s,LoadString(IDS_OBJ_WARNING,"warning: %s"),BAvpMakeFullName(tempb,ScanObject->WarningName));
        ObjectMessage(ScanObject,s,(prf_data.Color==1)? 0x0c:0x0f);
        ScanObject->RFlags|=RF_REPORT_DONE;
        incWorkStat(INCL_WARNING);
        if(prf_data.SuspiciousCopy) CopyToFolder(ScanObject->Name,prf_data.SuspiciousFolder);
       }
      break;
    case AVP_CALLBACK_OBJECT_SUSP:       //0x0205 ScanObject*
      if(prf_data.Sound) sound(1193180/1760,30000);
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
       ObjectMessage(ScanObject,s,attr);
      }
     if(!ScanObject->InPack) incWorkStat(INCL_PACKED);
     break;
    case AVP_CALLBACK_OBJECT_ARCHIVE:    //0x0207 ScanObject*
      if(prf_data.ShowPack)
       {
        ScanObject=(AVPScanObject*)mp2;
        BAvpMakeFullName(tempb,ScanObject->VirusName);
        sprintf(s,LoadString(IDS_OBJ_ARCHIVE,"archive: %s"),tempb);
        ObjectMessage(ScanObject,s,attr);
       }
      incWorkStat(INCL_ARCHIVES);
      break;
    case AVP_CALLBACK_OBJECT_DELETE:     //0x0208 ScanObject*
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_OBJ_DELETED,"deleted: %s"),BAvpMakeFullName(tempb,ScanObject->VirusName));
      ObjectMessage(ScanObject,s,(prf_data.Color==1)? 0x0c:0x0f);//attr);
      ScanObject->RFlags|=RF_REPORT_DONE;
      incWorkStat(INCL_DELETED);
      break;
    case AVP_CALLBACK_OBJECT_UNKNOWN:    //0x0209 ScanObject*
      ScanObject=(AVPScanObject*)mp2;
      sprintf(s,LoadString(IDS_OBJ_UNKNOWN,"%s: unknown format."),BAvpMakeFullName(tempb,ScanObject->VirusName));
      ScanObject->RFlags|=RF_REPORT_DONE;
      ObjectMessage(ScanObject,s,attr);
      break;
#ifndef _INTERN_VER_
    case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
      return sign_check_file( (char*)mp2,1,vndArray,vndArrayCount,0);
    case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
_printf("FILE_INTEGRITY_FAILED %s\r\n",(char*)mp2);
      //ret=SignReportMB((char*)mp2,-1001);
      //if(ret) PostMessage(WM_CLOSE);  //1 - Exit 0 - continue
      break;
#endif
    default: break;
   }
  return ret;
 }

ULONG DisinfectDlgProc(DisinfectDlg* dd)
 {
  ULONG ret;
  //(LoadString(IDS_DISINFECT_DIALOG,"" )); //Title

  _printf("%s  ",dd->m_sObject);
  _printf("%s %s\n",LoadString(IDS_INFECTED_BY,"Infected by virus:"),dd->m_sVirus);

  _puts(LoadString(IDS_TAB_ACTIONS,"Actions"));

  switch (dd->m_Action)
   {
    case 0:
      _printf(" - %s (Ok/",LoadString(IDS_DISINFECT_R,"Report only"));
      _printf("%s/",LoadString(IDS_DISINFECT_D_L,"disInfect"));
      _printf("%s/Cancel/Stop)\n",LoadString(IDS_DISINFECT_DEL,"Delete"));
      break;
    case 1:
      _printf(" - %s (Ok/",LoadString(IDS_DISINFECT_D_L,"disInfect"));
      _printf("%s/",LoadString(IDS_DISINFECT_R,"Report only"));
      _printf("%s/Cancel/Stop)\n",LoadString(IDS_DISINFECT_DEL,"Delete"));
      break;
    case 2:
      _printf(" - %s (Ok/",LoadString(IDS_DISINFECT_DEL,"Delete"));
      _printf("%s/",LoadString(IDS_DISINFECT_R,"Report only"));
      _printf("%s/Cancel/Stop)\n",LoadString(IDS_DISINFECT_D_L,"disInfect"));
      break;
   }
//  (LoadString(IDS_COPYINFECTED,"~Копировать в отдельную папку"));
//  (LoadString(IDS_APPLY_TOALL,"П~рименять ко всем зараженным объектам"));

//  WinSendMsg(hWndTmp,BM_SETCHECK,MPFROMSHORT(dd->m_All),NULL);
//  WinSendMsg(hWndTmp,BM_SETCHECK,MPFROMSHORT(dd->m_CopyFolder),NULL);

  if(cmdline.skip_dlg!=1)
    switch (getch())
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
      case 0x0d:
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
