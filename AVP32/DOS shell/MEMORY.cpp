//#include <malloc.h>
#include <conio.h>

#include "memalloc.h"

typedef unsigned long  BOOL;
typedef unsigned long  ULONG;
typedef unsigned short USHORT;
typedef unsigned char  BYTE;

typedef struct _IndexOfMemory
 {
  USHORT di;
  USHORT si;
 } IndexOfMemory;

typedef struct _MCB
 {
  struct _MCB *Prev;  //pointers to prev node for b-tree and for this node
  struct _MCB *Next;  //pointers to prev node for b-tree and for this node
  ULONG       Size;   //size of this free block
  struct
   {
    struct _MCB *PrevFree; //цепочка для поиска свободных блоков
    struct _MCB *NextFree;
    BOOL        flags;
   };
 }MCB,near* PMCB; //pointer to mcb

//full header for subheap with DPMI memory block handle
typedef struct _HCB
 {
  IndexOfMemory IOM;
  ULONG         FreeSize; //maximum value from memory
  struct _HCB   *pNextHCB;
  MCB           *pStartMCB;
 }HCB,near *PHCB;

PHCB pStartHCB=0,pTopHCB;

#define SubHeapSize       0x40000//28000
#define Alignment         8 //0x1000  4Кб

#ifdef __cplusplus
   extern "C" {
#endif
ULONG DPMIAllocMem(ULONG size,void* pMemory);
ULONG DPMIFreeMem(void* pMemory);
#ifdef __cplusplus
   }
#endif

//unsigned long ulFree=0,ulMalloc=0,defMem=0;
void Free(void *ptr)
 {
//ulFree++;
  if(ptr==0) return;
  PMCB pOldMCB;
  pOldMCB=(PMCB)((BYTE*)ptr-sizeof(MCB));
  if(pOldMCB->flags==0) return;

  ULONG NewFree;
  NewFree=pOldMCB->Size;
//defMem-=NewFree;
  if(pOldMCB->Prev==0)
   pOldMCB->flags=0;
  else
   for(PMCB pMCB=pOldMCB->Prev;pMCB!=0;pMCB=pMCB->Prev)
    if((pMCB->flags==0)||(pMCB->Prev==0))
     {
      pOldMCB->flags=0;
      pOldMCB->NextFree=pMCB->NextFree;
      pMCB->NextFree=pOldMCB;
      if(pOldMCB->NextFree!=0) pOldMCB->NextFree->PrevFree=pOldMCB;
      pOldMCB->PrevFree=pMCB;
      goto conc;
     }
//cprintf("1.\r\n");
conc:
  //for(;(pOldMCB->Prev==pOldMCB->PrevFree)&&(pOldMCB->Prev!=0);pOldMCB=pOldMCB->Prev);
  for(;(pOldMCB->Next==pOldMCB->NextFree)&&(pOldMCB->Next!=0);pOldMCB=pOldMCB->Next)
   {
    //вот эдесь глюк - под Win98 больше 1го блока не сливает, под дос и os/2 все нормально
    if((pOldMCB->Next->Prev==pOldMCB)&&
       (pOldMCB->NextFree->PrevFree==pOldMCB))
     {
//cprintf("1.\r\n");
      pOldMCB->Size+=sizeof(MCB)+pOldMCB->Next->Size;
      pOldMCB->Next=pOldMCB->Next->Next;
      pOldMCB->Next->Prev=pOldMCB;
      pOldMCB->NextFree=pOldMCB->NextFree->NextFree;
      if(pOldMCB->NextFree!=0)pOldMCB->NextFree->PrevFree=pOldMCB;
      return;
//cprintf("1..\r\n");
     }
    else goto exfor2;
   }
exfor2:
  //while(pMCB->PrevFree!=0) pMCB=pMCB->PrevFree;
  //PHCB pHCB;
  //pHCB=(PHCB)((BYTE*)pMCB-sizeof(HCB));
  //pHCB->FreeSize+=NewFree;
//cprintf("1!\r\n");
  return;
  //  free(__ptr);
  //  DPMIFreeMem((BYTE*)ptr-sizeof(IndexOfMemory));
 }

void *Malloc(ULONG Size)
 {//  return malloc(__size);
//ulMalloc++;
  if(Size==0) return 0;

  Size=(Size+sizeof(MCB)+(Alignment-1)) & ~(Alignment-1);
//defMem+=Size;

  if(pStartHCB!=0)
   for(PHCB pHCB=pStartHCB;pHCB!=0;pHCB=pHCB->pNextHCB)
    {
     //if(pHCB->FreeSize>=(Size+sizeof(MCB)))
      {
       for(PMCB pMCB_=pHCB->pStartMCB;pMCB_!=0;pMCB_=pMCB_->NextFree)
        if((pMCB_->Size>=Size)&&(pMCB_->flags==0))
         {
          if(pMCB_->Size>(Size+sizeof(MCB)))
           {
            PMCB pNewMCB;
            pNewMCB=(PMCB)((BYTE*)pMCB_+sizeof(MCB)+Size);
            pNewMCB->Prev=pMCB_;
            pNewMCB->Next=pMCB_->Next;
            pMCB_->Next=pNewMCB;
            if(pNewMCB->Next!=0) pNewMCB->Next->Prev=pNewMCB;

            pNewMCB->NextFree=pMCB_->NextFree;
            if(pNewMCB->NextFree!=0) pNewMCB->NextFree->PrevFree=pNewMCB;
            if(pMCB_->Prev!=0)
             {
              pNewMCB->PrevFree=pMCB_->PrevFree;
              pNewMCB->PrevFree->NextFree=pNewMCB;
             }
            else
             {
              pNewMCB->PrevFree=pMCB_;
              pMCB_->NextFree=pNewMCB;
             }
            pNewMCB->Size=pMCB_->Size-(sizeof(MCB)+Size);
            pNewMCB->flags=0;
            pMCB_->Size=Size;
            pMCB_->NextFree=pNewMCB;
            //pHCB->FreeSize-=sizeof(MCB);
           }
          else
           if(pMCB_->Prev!=0)
            {
             if(pMCB_->NextFree!=0) pMCB_->NextFree->PrevFree=pMCB_->PrevFree;
             pMCB_->PrevFree->NextFree=pMCB_->NextFree;
            }
          pMCB_->flags=1;
          //pHCB->FreeSize-=pMCB_->Size;
          return (BYTE*)pMCB_+sizeof(MCB);
         }
      }
    } //for

  ULONG AllocSize;
  AllocSize=(SubHeapSize>(Size+sizeof(HCB)+sizeof(MCB)))?
    (SubHeapSize):(Size+sizeof(HCB)+sizeof(MCB));
  if(pStartHCB==0)
   {
    if(DPMIAllocMem(AllocSize,&pTopHCB)) return 0;
    pStartHCB=pTopHCB;
   }
  else
   {
    if(DPMIAllocMem(AllocSize,&pTopHCB->pNextHCB)) return 0;
    pTopHCB=pTopHCB->pNextHCB;
   }
  pTopHCB->pNextHCB=0;
  pTopHCB->pStartMCB=(PMCB)((BYTE*)pTopHCB+sizeof(HCB));

  pTopHCB->pStartMCB->Prev=pTopHCB->pStartMCB->PrevFree=0;//pTopHCB->pStartMCB;
  pTopHCB->pStartMCB->Next=pTopHCB->pStartMCB->NextFree=0;
  pTopHCB->pStartMCB->Size=AllocSize-sizeof(HCB)-sizeof(MCB);
  pTopHCB->pStartMCB->flags=0;
  //pTopHCB->FreeSize=pTopHCB->pStartMCB->Size;

  PMCB pMCB;
  pMCB=pTopHCB->pStartMCB;

  if((pMCB->Size>=Size)&&(pMCB->flags==0))
   {
    if(pMCB->Size>(Size+sizeof(MCB)))
     {
      PMCB pNewMCB;
      pNewMCB=(PMCB)((BYTE*)pMCB+sizeof(MCB)+Size);
      pNewMCB->Prev=pNewMCB->PrevFree=pMCB;
      pNewMCB->Next=pNewMCB->NextFree=0;
      pNewMCB->Size=pMCB->Size-(sizeof(MCB)+Size);
      pNewMCB->flags=0;
      pMCB->Next=pNewMCB;
      pMCB->NextFree=pNewMCB;
      pMCB->Size=Size;
      //pTopHCB->FreeSize-=sizeof(MCB);
     }
    pMCB->flags=1;
    //pTopHCB->FreeSize-=pMCB->Size;
    return (BYTE*)pMCB+sizeof(MCB);
   }

  return 0;
 }
