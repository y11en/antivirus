#include <os2.h>
#include "memalloc.h"
#include <string.h>
#include "..\\typedef.h"
#include "../../Common.Fil/stuff/_CARRAY.H"

#include "LoadKeys.h"
extern ULONG functionality;

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
#include "disk.h"

#include "tv32def.h"
#define Uses_TView
#define Uses_TScrollBar
#define Uses_TRect
#include "include\tv.h"
#define __TListViews
#include "classes.h"
#include "contain.h"
//PLIST pTail=NULL,pListTop;

BOOL InsertRecords(char* ItemName,LONG itemtype,BYTE Select,TListViews *lv)
 {
  BOOL  fSuccess = TRUE;
  PLIST pList;
  BOOL  add=TRUE;

  if((lv->pTail!=NULL)&&(Select!=0xff))
   {
    pList=lv->pTail;
    do
     {
      if(pList->szType==itemtype)
       {
        char tstStr1[CCHMAXPATH+1],tstStr2[CCHMAXPATH+1];
        strcpy((char *)tstStr1,pList->szName);
        strcpy((char *)tstStr2,ItemName);
        ULONG len;
        len=strlen((char *)tstStr1)-1;
        if(*(tstStr1+len)=='\\') *(tstStr1+len)=0;
        len=strlen((char *)tstStr2)-1;
        if(*(tstStr2+len)=='\\') *(tstStr2+len)=0;

        if(stricmp((char *)tstStr1,(char *)tstStr2)==0)
//      if(strcmp(pList->szName,ItemName)==0)
         {
          if(pList->Selected==Select) return FALSE;
          else
           {
            pList->Selected=Select;
            add=FALSE;
           }
         }
       }
      pList=pList->pNext;
     } while(pList != NULL);
   }

  if(add==TRUE)
   {
    if(lv->pTail==NULL)
     {
      lv->pListTop=(LIST*)Malloc(sizeof(LIST));
      lv->pTail=lv->pListTop;
     }
    else
     {
      lv->pListTop->pNext=(LIST*)Malloc(sizeof(LIST));
      lv->pListTop=lv->pListTop->pNext;
     }
    strcpy(lv->pListTop->szName,ItemName);
    lv->pListTop->szType=itemtype;
    lv->pListTop->Selected=Select;
    lv->pListTop->pNext=NULL;
   }
  return fSuccess;
 }

VOID SelectAllRecords(BYTE sel,TListViews *lv)
 {
  PLIST pList;
  if(lv->pTail!=NULL)
   {
    pList=lv->pTail;
    do
     {
       pList->Selected=sel;
       pList=pList->pNext;
     } while(pList != NULL);
    }
 }
/*
ULONG IfSelectRecords(BYTE TypeSel,TListViews *lv)
 {
  PLIST pList;
  ULONG kol=0;
  if(lv->pTail!=NULL)
   {
    pList=lv->pTail;
    do
     {
       if(pList->Selected==TypeSel) kol++;
       pList=pList->pNext;
     } while(pList != NULL);
    }
  return kol;
 }
*/
VOID AddRecord(CHAR* StrName,BYTE Select,TListViews *lv)
 {
  CHAR typePath[]=" :\\";
  typePath[0]=StrName[0];
  //if(((avp_key_flag==0)||(!(avp_key->Options&KEY_O_REMOTESCAN)))
  if(!(functionality & FN_NETWORK)
    &&(DiskType((CHAR*)typePath)==IDI_NET_ON)) ;
  else
   InsertRecords(strlwr(StrName),DiskType(StrName),Select,lv);
 }

void SelectDiskType(LONG Type,BYTE znach,TListViews *lv)
 {
  PLIST pList;

  if(lv->pTail!=NULL)
   {
    pList=lv->pTail;
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

