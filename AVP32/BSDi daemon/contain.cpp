#include "typedef.h"
#include "malloc.h"
#include <string.h>
#include "../TYPEDEF.H"
#include "../../CommonFiles/Stuff/_CARRAY.H"

#include "loadkeys.h"
extern ULONG functionality;

#include "avp_rc.h"
#include "contain.h"
//#include "disk.h"

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
    pListTop=(LIST*)malloc(sizeof(LIST));
    pTail=pListTop;
   }
  else
   {
    pListTop->pNext=(LIST*)malloc(sizeof(LIST));
    pListTop=pListTop->pNext;
   }
  strcpy(pListTop->szName,ItemName);
  pListTop->szType=itemtype;
  pListTop->Selected=Select;
  pListTop->pNext=NULL;
//printf("pListTop:%s %d\n",pListTop->szName,pListTop->pNext)
;
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
  //CHAR typePath[]=" :\\";
  //typePath[0]=StrName[0];
  //if(((avp_key_flag==0)||(!(avp_key->Options&KEY_O_REMOTESCAN)))
//  if(!(functionality & FN_NETWORK)
 &&(DiskType((CHAR*)typePath)==IDI_NET_ON) );
//  else
  // InsertRecords(strlwr(StrName),DiskType(StrName),Select);
   InsertRecords(StrName,0,Select);
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
        if(pList->szType==IDI_FLOPPY_12||pList->szType==IDI_FLOPPY_144)
         pList->Selected=znach;
        break;
       case 2:
        if(pList->szType==IDI_LOCAL1||
pList->szType==IDI_CD)
         pList->Selected=znach;
        break;
       case 3:
        if(pList->szType==IDI_NET_ON)
         pList->Selected=znach;
        break;
       }
      pList=pList->pNext;
     } while(pList != NULL);
   }
 }

