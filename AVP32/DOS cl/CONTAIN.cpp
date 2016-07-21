#include <os2.h>
#include <string.h>
#include "memalloc.h"

#include "..\\typedef.h"
#ifndef _INTERN_VER_
#include "../../Common.Fil/stuff/_CARRAY.H"

#include "LoadKeys.h"
extern ULONG functionality;
#endif

//typedef long LONG;
//typedef unsigned long ULONG;
//typedef unsigned long APIRET;
//#define VOID void
//#define CHAR char
//#define BOOL unsigned char
//#define BYTE  unsigned char
//#define CCHMAXPATH 512
//#define TRUE  1
//#define FALSE 0

#include "avp_rc.h"
#include "contain.h"
//#include "key.h"
#include "disk.h"

PLIST pTail=NULL,pListTop;

BOOL InsertRecords(char* ItemName,LONG itemtype,BYTE Select)
 {
  BOOL     fSuccess = TRUE;
  PLIST pList;

  if(pTail!=NULL)
   {
    pList=pTail;
    do
     {
      if((strcmp(pList->szName,ItemName)==0)&&
         (pList->szType==itemtype))
       {
        if(pList->Selected==Select) return FALSE;
        else {pList->Selected=Select; return TRUE;}
       }
      pList=pList->pNext;
     } while(pList != NULL);
   }

  if(pTail==NULL)
   {
    pListTop=(LIST*)Malloc(sizeof(LIST));
    pTail=pListTop;
   }
  else
   {
    pListTop->pNext=(LIST*)Malloc(sizeof(LIST));
    pListTop=pListTop->pNext;
   }
  strcpy(pListTop->szName,ItemName);
  pListTop->szType=itemtype;
  pListTop->Selected=Select;
  pListTop->pNext=NULL;

  return fSuccess;
 }

VOID SelectAllRecords(BYTE sel)
 {
  PLIST pList;
  if(pTail!=NULL)
   {
    pList=pTail;
    do
     {
      pList->Selected=sel;
      pList=pList->pNext;
     } while(pList != NULL);
    }
 }

unsigned long IfSelectRecords(unsigned char TypeSel)
 {
  PLIST pList;
  ULONG kol=0;
  if(pTail!=NULL)
   {
    pList=pTail;
    do
     {
       if(pList->Selected==TypeSel) kol++;
       pList=pList->pNext;
     } while(pList != NULL);
    }
  return kol;
 }

VOID AddRecord(CHAR* StrName,BYTE Select)
 {
  CHAR typePath[]=" :\\";
  typePath[0]=StrName[0];
//  if(((avpKeyFlag==0)||(!(avpKey->Options&KEY_O_REMOTESCAN)))
#ifndef _INTERN_VER_
  if(!(functionality & FN_NETWORK)
    &&(DiskType((CHAR*)typePath)==IDI_NET_ON)) ;
  else
#endif
//   InsertRecords(strlwr(StrName),DiskType(StrName),Select);
   InsertRecords(StrName,DiskType(StrName),Select);
 }

void SelectDiskType(LONG Type,BYTE znach)
 {
  PLIST pList;

  if(pTail!=NULL)
   {
    pList=pTail;
    do
     {
//IDI_FOLD_CLOSE
//IDI_FOLD_OPEN
//IDI_FOLD_NONE
      switch (Type)
       {
       case 1:
        if(pList->szType==IDI_FLOPPY_12||pList->szType==(IDI_FLOPPY_12+1)||
           pList->szType==IDI_FLOPPY_144||pList->szType==(IDI_FLOPPY_144+1))
         pList->Selected=znach;
        break;
       case 2:
        if(pList->szType==IDI_LOCAL1||pList->szType==(IDI_LOCAL1+1)||
//           pList->szType==IDI_LOCAL1_NONE||//pList->szType==IDI_LOCAL2||
           pList->szType==IDI_CD||pList->szType==(IDI_CD+1))
         pList->Selected=znach;
        break;
       case 3:
        if(pList->szType==IDI_NET_ON||pList->szType==(IDI_NET_ON+1))//||pList->szType==IDI_NET_OFF)
         pList->Selected=znach;
        break;
       }
      pList=pList->pNext;
     } while(pList != NULL);
   }
 }

