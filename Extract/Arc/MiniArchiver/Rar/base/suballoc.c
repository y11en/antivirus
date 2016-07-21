#include <Prague/prague.h>

#include "const.h"
#include "proto.h"


#ifdef SA_DEBUG
#include <stdio.h>

RSTATIC uint SA_Base(byte *ptr, byte *base)
{
    uint val;
    if ( ptr != NULL )
    {
        val = (ptr-base);
    }
    else
    {
        val = ptr-NULL;
    }
    return val;
}

RSTATIC void RATTRIB SA_Dump(struct sRAR *data, int dump)
{
    int I;
    byte *base;
    FILE *file;
    uint losize;
    uint hisize;
    uint txsize;
    uint total;
    base = data->PPM.SubAlloc.HeapStart;

    Printf("Sub Allocator %08X" crlf, base);
    Printf("    SubAllocatorSize     = %08X" crlf, data->PPM.SubAlloc.SubAllocatorSize   );
    Printf("    AllocSize            = %08X" crlf, data->PPM.SubAlloc.AllocSize          );
    for ( I=0;I<N_INDEXES;I++ )
    Printf("      FreeList[%02X]       = %08X" crlf, I, SA_Base((byte*)(data->PPM.SubAlloc.FreeList[I].next), base)     );
    Printf("    *HeapStart           = %08X" crlf, SA_Base(data->PPM.SubAlloc.HeapStart, base)     );
    Printf("    *LoUnit              = %08X" crlf, SA_Base(data->PPM.SubAlloc.LoUnit, base)        );
    Printf("    *HiUnit              = %08X" crlf, SA_Base(data->PPM.SubAlloc.HiUnit, base)        );
    Printf("    GlueCount            = %08X" crlf, data->PPM.SubAlloc.GlueCount          );
    Printf("    *pText               = %08X" crlf, SA_Base(data->PPM.SubAlloc.pText, base)         );
    Printf("    *UnitsStart          = %08X" crlf, SA_Base(data->PPM.SubAlloc.UnitsStart, base)    );
    Printf("    *HeapEnd             = %08X" crlf, SA_Base(data->PPM.SubAlloc.HeapEnd, base)       );
    Printf("    *FakeUnitsStart      = %08X" crlf, SA_Base(data->PPM.SubAlloc.FakeUnitsStart, base));

    txsize = SA_Base(data->PPM.SubAlloc.pText, base);
    losize = data->PPM.SubAlloc.LoUnit - data->PPM.SubAlloc.UnitsStart;
    hisize = data->PPM.SubAlloc.HeapEnd - data->PPM.SubAlloc.HiUnit;
    if ( (int)(hisize) < 0 ) hisize = 0;
    Printf("    Text    uses %08X(%8d) bytes" crlf, txsize, txsize);
    Printf("    LoUnits uses %08X(%8d) bytes" crlf, losize, losize);
    Printf("    HiUnits uses %08X(%8d) bytes" crlf, hisize, hisize);

    total = txsize + losize + hisize;

    Printf("    Total alloc  %08X(%8d) bytes" crlf, data->PPM.SubAlloc.AllocSize, data->PPM.SubAlloc.AllocSize);
    Printf("    Total in use %08X(%8d) bytes, %2d%%" crlf, total, total, total*100/data->PPM.SubAlloc.AllocSize);
    if ( dump )
    {
        file = fopen("PPM.BIN","wb");
        if ( file )
        {
            fwrite(base, 1, data->PPM.SubAlloc.AllocSize, file);
            fclose(file);
        }
    }
}
#endif /*SA_DEBUG*/

RSTATIC void RATTRIB MB_insertAt(tMEM_BLK *what, tMEM_BLK *where)
{
    what->next = (what->prev = where)->next;
    where->next = what->next->prev = what;
}

RSTATIC void RATTRIB MB_remove(tMEM_BLK *what)
{
    what->prev->next = what->next;
    what->next->prev = what->prev;
}

RSTATIC void RATTRIB SA_InsertNode(struct sRAR *data, void *p, uint indx)
{
  if ( indx >= N_INDEXES )
  {
    data->Error = cTRUE;
    return; // overrun
  }
  ((tNODE*)(p))->next = data->PPM.SubAlloc.FreeList[indx].next;
  data->PPM.SubAlloc.FreeList[indx].next = ((tNODE*)(p));
}

RSTATIC void * RATTRIB SA_RemoveNode(tRAR* data, int indx)
{
    tNODE *RetVal;

    RetVal = data->PPM.SubAlloc.FreeList[indx].next;
    
    /* dirty hack for fix bug */
    if ( (byte*)RetVal < data->PPM.SubAlloc.HeapStart ||
         (byte*)RetVal > data->PPM.SubAlloc.HeapEnd )
    {
      data->Error = cTRUE;
      return(NULL); /* overrun */
    }

    data->PPM.SubAlloc.FreeList[indx].next = RetVal->next;
    return RetVal;
}

RSTATIC void RATTRIB SA_SplitBlock(struct sRAR *data, void *pv, int OldIndx, int NewIndx)
{
    int i;
    int UDiff;
    byte *p;

    UDiff = data->PPM.SubAlloc.Indx2Units[OldIndx]-data->PPM.SubAlloc.Indx2Units[NewIndx];
    p = (byte*)(pv)+SA_U2B(data->PPM.SubAlloc.Indx2Units[NewIndx]);
    if (data->PPM.SubAlloc.Indx2Units[i=data->PPM.SubAlloc.Units2Indx[UDiff-1]] != UDiff)
    {
        SA_InsertNode(data,p,--i);
        p += SA_U2B(i=data->PPM.SubAlloc.Indx2Units[i]);
        UDiff -= i;
    }
    SA_InsertNode(data,p,data->PPM.SubAlloc.Units2Indx[UDiff-1]);
}

RSTATIC void RATTRIB SA_StopSubAllocator(struct sRAR *data)
{
    if ( data->PPM.SubAlloc.SubAllocatorSize )
    {
        data->PPM.SubAlloc.SubAllocatorSize = 0;
        RarFree(data,data->PPM.SubAlloc.HeapStart);
    }
    data->PPM.SubAlloc.HeapStart = NULL;
}

#define PTR_FIX(ptr, newbase, oldbase, oldend)  if ((ptr) >= (oldbase) && (ptr) <= (oldend) ) (ptr) = (newbase)+((ptr)-(oldbase))

RSTATIC void RATTRIB SA_UnitFix(tMEM_FIX *start, tMEM_FIX *end, byte *newbase, byte *oldbase, byte *oldend)
{
    while ( start < end )
    {
        PTR_FIX(start->ptr, newbase, oldbase, oldend);
        else
        {
            if ( start->ptr != NULL )
            {
                //Printf("Warning: %08X at %08X cannot be fixed" crlf, start->ptr, (byte*)(start)-newbase);
            }
        }
        start++;
    }
}

RSTATIC rarbool RATTRIB SA_MemFix(struct sRAR *data, void *newptr, void *oldptr)
{
    if ( newptr == NULL )
    {
        data->PPM.SubAlloc.SubAllocatorSize = 0;
        data->PPM.SubAlloc.HeapStart = NULL;
    }
    else
    {
        #ifdef SA_DEBUG
            SA_Dump(data,1);
        #endif

        data->PPM.SubAlloc.HeapStart = newptr;

        if ( newptr != oldptr )
        {
            byte *oldbase;
            byte *oldend;
            byte *newbase;
            uint cnt;
            byte **list;
	    byte *tmp;

            //Printf("SA_MemFix: old and new ptrs does not match" crlf);
            //return FALSE;

            // fix PPM data
            oldbase = oldptr;
            oldend  = data->PPM.SubAlloc.HeapEnd;
            newbase = newptr;

            cnt = N_INDEXES;
            list = (byte**)(data->PPM.SubAlloc.FreeList);
            while ( cnt-- )
            {
                PTR_FIX(*list,newbase, oldbase, oldend);
                list++;
            }

            // SubAllocator fix
            PTR_FIX(data->PPM.SubAlloc.LoUnit        , newbase, oldbase, oldend);
            PTR_FIX(data->PPM.SubAlloc.HiUnit        , newbase, oldbase, oldend);
            PTR_FIX(data->PPM.SubAlloc.pText         , newbase, oldbase, oldend);
            PTR_FIX(data->PPM.SubAlloc.UnitsStart    , newbase, oldbase, oldend);
            PTR_FIX(data->PPM.SubAlloc.HeapEnd       , newbase, oldbase, oldend);
            PTR_FIX(data->PPM.SubAlloc.FakeUnitsStart, newbase, oldbase, oldend);

            // allocated units fix
            SA_UnitFix((tMEM_FIX*)data->PPM.SubAlloc.UnitsStart, (tMEM_FIX*)data->PPM.SubAlloc.LoUnit , newbase, oldbase, oldend);
            SA_UnitFix((tMEM_FIX*)data->PPM.SubAlloc.HiUnit    , (tMEM_FIX*)data->PPM.SubAlloc.HeapEnd, newbase, oldbase, oldend);

            // model fix
	    tmp = (byte*)data->PPM.MinContext;
            PTR_FIX(tmp, newbase, oldbase, oldend);
	    tmp = (byte*)data->PPM.MedContext;
            PTR_FIX(tmp, newbase, oldbase, oldend);
	    tmp = (byte*)data->PPM.MaxContext;
            PTR_FIX(tmp, newbase, oldbase, oldend);
	    tmp = (byte*)data->PPM.FoundState;
            PTR_FIX(tmp, newbase, oldbase, oldend);
            #ifdef SA_DEBUG
                SA_Dump(data,0);
            #endif
        }
    }
    return TRUE;
}

RSTATIC rarbool RATTRIB SA_StartSubAllocator(struct sRAR *data, int SASize)
{
    rarbool rcode;
    uint required;

    rcode = TRUE;

    required = SASize << 20;       // size in Mb
    #ifndef AVP
//    Printf("Allocating %d Mbytes" crlf, SASize);
    #endif
    if ( data->PPM.SubAlloc.SubAllocatorSize != required)
    {

        SA_StopSubAllocator(data);
        data->PPM.SubAlloc.AllocSize = required/FIXED_UNIT_SIZE*UNIT_SIZE+UNIT_SIZE;
        if ( (data->PPM.SubAlloc.HeapStart = RarAlloc(data,data->PPM.SubAlloc.AllocSize + 2048, SA_MemFix)) == NULL)
        {
            rcode = FALSE;
        }
        else
        {
            memset(data->PPM.SubAlloc.HeapStart + data->PPM.SubAlloc.AllocSize, 0, 1024);
            data->PPM.SubAlloc.HeapEnd = data->PPM.SubAlloc.HeapStart+data->PPM.SubAlloc.AllocSize-UNIT_SIZE;
            data->PPM.SubAlloc.SubAllocatorSize = required;
        }
    }
    return rcode;
}

RSTATIC void RATTRIB SA_InitSubAllocator(struct sRAR *data)
{
    uint Size2;
    uint RealSize2;
    uint Size1;
    uint RealSize1;
    int i;
    int k;

    memset(data->PPM.SubAlloc.FreeList,0,sizeof(data->PPM.SubAlloc.FreeList));

    data->PPM.SubAlloc.pText = data->PPM.SubAlloc.HeapStart;

    Size2 = FIXED_UNIT_SIZE*(data->PPM.SubAlloc.SubAllocatorSize/8/FIXED_UNIT_SIZE*7);
    RealSize2 = Size2/FIXED_UNIT_SIZE*UNIT_SIZE;
    Size1 = data->PPM.SubAlloc.SubAllocatorSize-Size2;
    RealSize1 = Size1/FIXED_UNIT_SIZE*UNIT_SIZE+Size1%FIXED_UNIT_SIZE;

    data->PPM.SubAlloc.HiUnit = data->PPM.SubAlloc.HeapStart+data->PPM.SubAlloc.SubAllocatorSize;
    data->PPM.SubAlloc.LoUnit = data->PPM.SubAlloc.UnitsStart=data->PPM.SubAlloc.HeapStart+RealSize1;
    data->PPM.SubAlloc.FakeUnitsStart = data->PPM.SubAlloc.HeapStart+Size1;
    data->PPM.SubAlloc.HiUnit = data->PPM.SubAlloc.LoUnit+RealSize2;

    for (i=0,k=1;i < N1       ;i++,k += 1) data->PPM.SubAlloc.Indx2Units[i]=k;
    for (k++;i < (N1+N2)      ;i++,k += 2) data->PPM.SubAlloc.Indx2Units[i]=k;
    for (k++;i < (N1+N2+N3)   ;i++,k += 3) data->PPM.SubAlloc.Indx2Units[i]=k;
    for (k++;i < (N1+N2+N3+N4);i++,k += 4) data->PPM.SubAlloc.Indx2Units[i]=k;

    for (data->PPM.SubAlloc.GlueCount=k=i=0;k < 128;k++)
    {
        i += (data->PPM.SubAlloc.Indx2Units[i] < k+1);
        data->PPM.SubAlloc.Units2Indx[k]=i;
    }

    //SA_Dump(data, 0);
}

RSTATIC void RATTRIB SA_GlueFreeBlocks(struct sRAR *data)
{
    tMEM_BLK  s0;
    tMEM_BLK *p;
    tMEM_BLK *p1;

    int i;
    int k;
    int sz;

    if ( data->PPM.SubAlloc.LoUnit != data->PPM.SubAlloc.HiUnit ) *data->PPM.SubAlloc.LoUnit = 0;



    for (i=0, s0.next=s0.prev=&s0;i < N_INDEXES;i++)
    {
        while ( data->PPM.SubAlloc.FreeList[i].next )
        {
            p=(tMEM_BLK*)SA_RemoveNode(data,i);
            MB_insertAt(p,&s0);
            p->Stamp=0xFFFF;
            p->NU=data->PPM.SubAlloc.Indx2Units[i];
        }
    }

    for (p=s0.next;p != &s0;p=p->next)
    {
        while ((p1=p+p->NU)->Stamp == 0xFFFF && ((int)(p->NU)+p1->NU) < 0x10000)
        {
            MB_remove(p1);
            p->NU += p1->NU;
        }
    }

    while ((p=s0.next) != &s0)
    {
        for (MB_remove(p), sz=p->NU;sz > 128;sz -= 128, p += 128) SA_InsertNode(data,p,N_INDEXES-1);

        if (data->PPM.SubAlloc.Indx2Units[i=data->PPM.SubAlloc.Units2Indx[sz-1]] != sz)
        {
            k=sz-data->PPM.SubAlloc.Indx2Units[--i];
            SA_InsertNode(data,p+(sz-k),k-1);
        }
        SA_InsertNode(data,p,i);
    }
}

RSTATIC void * RATTRIB SA_AllocUnitsRare(struct sRAR *data, int indx)
{
    void *RetVal;
    int i;

    if ( !data->PPM.SubAlloc.GlueCount )
    {
        data->PPM.SubAlloc.GlueCount = 255;
        SA_GlueFreeBlocks(data);
        if ( data->PPM.SubAlloc.FreeList[indx].next )
            return SA_RemoveNode(data, indx);
    }

    i = indx;

    do
    {
        if (++i == N_INDEXES)
        {
            int j;

            data->PPM.SubAlloc.GlueCount--;
            i = SA_U2B(data->PPM.SubAlloc.Indx2Units[indx]);
            j = 12*data->PPM.SubAlloc.Indx2Units[indx];

            if (data->PPM.SubAlloc.FakeUnitsStart-data->PPM.SubAlloc.pText > j)
            {
                data->PPM.SubAlloc.FakeUnitsStart-=j;
                data->PPM.SubAlloc.UnitsStart -= i;
                return(data->PPM.SubAlloc.UnitsStart);
            }

            return(NULL);
        }
    } while ( !data->PPM.SubAlloc.FreeList[i].next );

    RetVal=SA_RemoveNode(data,i);
    SA_SplitBlock(data,RetVal,i,indx);
    return RetVal;
}


RSTATIC void * RATTRIB SA_AllocUnits(struct sRAR *data, uint NU)
{
    uint indx;
    void *RetVal;

    if ( !NU || NU > 128 )
    {
      data->Error = (cTRUE);
      return NULL; // error
    }

    indx = data->PPM.SubAlloc.Units2Indx[NU-1];
    if ( indx >= N_INDEXES )
    {
      data->Error = (cTRUE);
      return NULL; // error
    }

    if ( data->PPM.SubAlloc.FreeList[indx].next )
    {
        return SA_RemoveNode(data,indx);
    }

    RetVal = data->PPM.SubAlloc.LoUnit;
    data->PPM.SubAlloc.LoUnit += SA_U2B(data->PPM.SubAlloc.Indx2Units[indx]);

    if (data->PPM.SubAlloc.LoUnit <= data->PPM.SubAlloc.HiUnit)
    {
        return RetVal;
    }

    data->PPM.SubAlloc.LoUnit -= SA_U2B(data->PPM.SubAlloc.Indx2Units[indx]);
    return SA_AllocUnitsRare(data,indx);
}

RSTATIC void * RATTRIB SA_AllocContext(struct sRAR *data)
{
    if ( data->PPM.SubAlloc.HiUnit != data->PPM.SubAlloc.LoUnit) return (data->PPM.SubAlloc.HiUnit -= UNIT_SIZE);
    if ( data->PPM.SubAlloc.FreeList->next ) return SA_RemoveNode(data,0);
    return SA_AllocUnitsRare(data,0);
}

RSTATIC void * RATTRIB SA_ExpandUnits(struct sRAR *data, void *OldPtr,int OldNU)
{
    int i0;
    int i1;
    void *ptr;

    i0 = data->PPM.SubAlloc.Units2Indx[OldNU-1];
    i1 = data->PPM.SubAlloc.Units2Indx[OldNU-1+1];

    if (i0 == i1) return OldPtr;

    ptr = SA_AllocUnits(data,OldNU+1);

    if ( ptr )
    {
        memcpy(ptr,OldPtr,SA_U2B(OldNU));
        SA_InsertNode(data,OldPtr,i0);
    }
    return ptr;
}


RSTATIC void * RATTRIB SA_ShrinkUnits(struct sRAR *data, void* OldPtr,int OldNU,int NewNU)
{
    int i0;
    int i1;

    i0 = data->PPM.SubAlloc.Units2Indx[OldNU-1];
    i1 = data->PPM.SubAlloc.Units2Indx[NewNU-1];

    if (i0 == i1) return OldPtr;

    if ( data->PPM.SubAlloc.FreeList[i1].next )
    {
        void *ptr;
        ptr = SA_RemoveNode(data,i1);
        memcpy(ptr,OldPtr,SA_U2B(NewNU));
        SA_InsertNode(data,OldPtr,i0);
        return ptr;
    }
    else
    {
        SA_SplitBlock(data,OldPtr,i0,i1);
        return OldPtr;
    }
}


RSTATIC void RATTRIB SA_FreeUnits(struct sRAR *data, void *ptr,int OldNU)
{
    SA_InsertNode(data, ptr,data->PPM.SubAlloc.Units2Indx[OldNU-1]);
}

