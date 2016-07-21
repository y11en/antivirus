#include <Prague/prague.h>

#include "const.h"
#include "proto.h"


#define CHECK_NONE  0
#define CHECK_USED  1
#define CHECK_FREE  2
#ifdef MEM_DEBUG
static char *MemComment;
#endif
#ifdef MEM_DEBUG

RSTATIC void RATTRIB MemDump(tRAR *data)
{
    tMEMPAGE *page;
    tMEMENTRY *entry;
    uint count;

    entry = data->MemList;
    count = data->MemEntryCount;

    Printf("Global allocation: %d entries" crlf, count);

    while ( count-- )
    {
        Printf("    %8d bytes allocated at %08X" crlf, entry->size, entry->ptr);
        entry++;
    }

    page = &data->MemPage;
    return ;
    Printf("\n\f\nMemory dump\n");
    while ( page != NULL )
    {
        tUNIT *unit;
        uint index;

        Printf("Page %08X" crlf, page);
        Printf("    page->prev          = %08X" crlf , page->prev);
        Printf("    page->next          = %08X" crlf , page->next);
        Printf("    page->free_count    = %08X" crlf , page->free_count   );
        Printf("    page->unitsize      = %08X" crlf , page->unitsize     );
        Printf("    page->count         = %08X" crlf , page->count        );
        Printf("    page->size          = %08X" crlf , page->size         );
        Printf("    page->*start        = %08X" crlf , (byte*)(page->start)-(byte*)(page));
        Printf("    page->*units        = %08X" crlf , (byte*)(page->units)-(byte*)(page));
        Printf("    page->*units_end    = %08X" crlf , (byte*)(page->units_end)-(byte*)(page));
        unit = page->units;

        index = 0;

        while ( unit < page->units_end )
        {
            uint cnt;

            cnt = unit->state & UNIT_COUNT;

            if ( cnt )
            {
                index = 0;
                //Printf(crlf);
            }
            else
            {
                index++;
            }
            #ifdef MEM_DEBUG
            if ( unit->state & UNIT_USED )
            {
                Printf("      [%04X] [%04X] %s %04X == %s" crlf, unit-page->units, index, (unit->state & UNIT_USED) ? "USED":"FREE", cnt, unit->comment);
            }
            else
            #endif
            Printf("      [%04X] [%04X] %s %04X" crlf, unit-page->units, index, (unit->state & UNIT_USED) ? "USED":"FREE", cnt);
            unit+=cnt;
        }
        page = page->next;
    }
}

RSTATIC void RATTRIB MemCheck(tRAR *data)
{
    tMEMPAGE *page;
    rarbool error;

    page = &data->MemPage;
    error = FALSE;

    while ( page != NULL )
    {
        if ( page->prev != NULL )
        {
            tUNIT *unit;
            uint used;
            uint free;
            uint cnt;
            uint mode;
            //rarbool error;

            unit = page->units;

            used = 0;
            free = 0;
            mode = CHECK_NONE;
            cnt = 0;

            while ( unit < page->units_end && !error )
            {
                switch ( mode )
                {
                    case CHECK_NONE:
                        cnt = unit->state & UNIT_COUNT;

                        if ( cnt == 0 )
                        {
                            //Printf("    Invalid state for group header %04X: %04X" crlf, unit-page->units, unit->state);
                            error = TRUE;
                        }

                        if ( unit->state & UNIT_USED )
                        {
                            mode = CHECK_USED;
                            used++;
                        }
                        else
                        {
                            mode = CHECK_FREE;
                            free++;
                        }

                        cnt--;
                        unit++;
                        break;

                    case CHECK_USED:

                        if ( cnt )
                        {
                            if ( unit->state == UNIT_USED )
                            {
                                cnt--;
                                unit++;
                                used++;
                            }
                            else
                            {
                                //Printf("    Invalid used unit %04X: %04X" crlf, unit-page->units, unit->state);
                                error = TRUE;
                            }
                        }
                        else
                        {
                            mode = CHECK_NONE;
                        }
                        break;

                    case CHECK_FREE:
                        if ( cnt )
                        {
                            if ( unit->state == 0)
                            {
                                cnt--;
                                unit++;
                                free++;
                            }
                            else
                            {
                                //Printf("    Invalid free unit %04X: %04X" crlf, unit-page->units, unit->state);
                                error = TRUE;
                            }
                        }
                        else
                        {
                            mode = CHECK_NONE;
                        }
                        break;

                }
            }

            if ( cnt && !error )
            {
                //Printf("In last group %d units out of page" crlf, cnt);
                error = TRUE;
            }

            if ( page->free_count != free && !error )
            {
                //Printf("Invalid free_count value: %d instead of %d" crlf, page->free_count, free);
                error = TRUE;
            }

            if ( (used+free) != page->count && !error )
            {
                //Printf("Invalid total unit count: %d instead of %d" crlf, page->count, used+free);
                error = TRUE;
            }

            if ( error )
            {
                //Printf("Error on page %08X" crlf, page);
            }
            else
            {
                //MemPageDump(page);
            }

        }
        page = page->next;
    }

    if ( error )
    {
        //Printf("Error on page %08X" crlf, page);
        #ifdef MEM_DEBUG
        MemDump(data);
        #endif
    }
    else
    {
        //MemDump(data);
    }

}

RSTATIC void  RATTRIB  ArrayDump     (tARRAY *array, char *comment)
{
    Printf("    %s->ptr   = %08X" crlf, comment, array->ptr  );
    Printf("    %s->count = %08X" crlf, comment, array->count);
    Printf("    %s->size  = %08X" crlf, comment, array->size );
}
#endif /* MEM_DEBUG */

RSTATIC void   RATTRIB  MemMove       (void *dst, void *src, uint count)
{
    if (dst <= src || (char *)dst >= ((char *)src + count))
    {
        while (count--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
    }
    else
    {
        dst = (char *)dst + count - 1;
        src = (char *)src + count - 1;

        while (count--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst - 1;
            src = (char *)src - 1;
        }
    }
}

RSTATIC void * RATTRIB RarAlloc(tRAR *data, uint size, tMEMFIXPROC MemFix)
{
    void *ptr;

    ptr = NULL;

    if ( data->MemEntryCount < MEMENTRIES )
    {
        ptr = Malloc(data, size);

        //Printf("Allocated %d at %08X (%d)" crlf, size, ptr, data->MemEntryCount);
        if ( ptr != NULL )
        {
            tMEMENTRY *entry;
            entry = data->MemList+data->MemEntryCount;
            entry->ptr = ptr;
            entry->size = size;
            entry->MemFix = MemFix;
            data->MemEntryCount++;
        }
    }
    return ptr;
}

RSTATIC void RATTRIB RarFreeAll(tRAR *data)
{
    uint count;
    tMEMENTRY *entry;
    count = data->MemEntryCount;
    entry = data->MemList+count;

    while ( count-- )
    {
        entry--;
        //Printf("RarFreeAll: %d at %08X" crlf, entry->size, entry->ptr);
        Free(data, entry->ptr);
    }
}

RSTATIC void RATTRIB RarFree(tRAR *data, void *ptr)
{
    uint count;
    tMEMENTRY *entry;
    count = data->MemEntryCount;
    entry = data->MemList;

    while ( count-- )
    {
        if ( ptr == entry->ptr )
        {
            //Printf("Free %d at %08X" crlf, entry->size, entry->ptr );
            Free(data, entry->ptr);
            memcpy(entry, entry+1, count*sizeof(tMEMENTRY));
            data->MemEntryCount--;
            break;
        }
        entry++;
    }
}

RSTATIC void RATTRIB MemPageInit(tMEMPAGE *page, tMEMPAGE *old, rarbool load)
{

    page->units = (tUNIT*)(page+1);

    if ( load )
    {
        page->old = page->start;
    }
    else
    {
        page->unitsize = old->unitsize;
        page->count = old->count;
        page->size = old->unitsize*old->count;
        page->old = NULL;
        page->free_count = old->count;

        memset(page->units,0,old->count*sizeof(tUNIT));
        page->units->state = (word)(old->count);
    }

    page->units_end = page->units+page->count;

    page->prev = old;
    page->next = NULL;
    page->start = ((byte*)(page))+sizeof(tMEMPAGE)+sizeof(tUNIT)*page->count;


    old->next = page;
}

RSTATIC rarbool RATTRIB MemPageFix(tRAR *data, void *newptr, void *oldptr)
{
    tMEMPAGE *page;

    page = &data->MemPage;

    if ( newptr == NULL )
    {
        page->next = NULL;
    }
    else
    {

        while ( page->next != NULL ) page = page->next;
        MemPageInit(newptr, page, TRUE);
    }
    return TRUE;
}

RSTATIC tMEMPAGE * RATTRIB MemPageAlloc(tRAR *data, tMEMPAGE *old)
{
    tMEMPAGE *page;

    page = (tMEMPAGE*)RarAlloc(data,sizeof(tMEMPAGE)+(old->unitsize+sizeof(tUNIT))*old->count,MemPageFix);

    if ( page != NULL )
    {
        MemPageInit(page, old, FALSE);
    }
    return page;
}

RSTATIC void RATTRIB MemJoinNext(tMEMPAGE *page, tUNIT *unit)
{
    tUNIT *free;

    free = unit+unit->state;        // block is free, bit used == 0

    while ( free < page->units_end )
    {
        if ( (free->state & UNIT_USED) == 0 )
        {
            unit->state = (word)(unit->state+free->state);
            free->state = 0;
            free = unit+unit->state;
        }
        else
        {
            break;
        }
    }
}

RSTATIC void RATTRIB MemFreeHere(tMEMPAGE *page, tUNIT *unit)
{
    uint free;

    if ( unit->state & UNIT_USED )
    {
        // number of units to free
        free = unit->state & UNIT_COUNT;

        if ( free )
        {
            // free all units in group, optional
            memset(unit+1,0,(free-1)*sizeof(tUNIT));
            unit->state = (word)(free);     // clear used flag
            page->free_count += free;

            // check next group
            MemJoinNext(page,unit);

        }
        else
        {
            //Printf("Trying to free not a first unit in group");
        }

    }
    else
    {
        //Printf("Trying to free already freed unit");
    }

}

RSTATIC void * RATTRIB MemAllocHere(tMEMPAGE *page, tUNIT *unit, uint unitcount)
{
    void *ptr;

    /* bugfix, 22.08.2005 */
    if ( unitcount == 0 )
      return(NULL); /* error */
    
    if ( unit->state != unitcount  )  // unit is free
    {
        // split free block
        unit[unitcount].state = (word)(unit->state-unitcount);
    }
    unit->state = (word)(unitcount | UNIT_USED) ;
    #ifdef MEM_DEBUG
    unit->comment = MemComment;
    #endif

    ptr = page->start+(unit-page->units) * page->unitsize;
    page->free_count -= unitcount;

    // optional
    unitcount--;
    unit++;

    while ( unitcount-- )
    {
        unit->state = UNIT_USED;
        unit++;
    }
    return ptr;
}

RSTATIC void * RATTRIB MemAllocInPage(tMEMPAGE *page, uint unitcount)
{
    // alloc units and collect free blocks
    byte *ptr;

    ptr = NULL;
    if ( !unitcount )
      return(NULL);

    if ( unitcount <= page->free_count )
    {
        tUNIT *unit;
        tUNIT *best;

        unit = page->units;

        best = NULL;

        while ( unit < page->units_end )
        {
            if ( (unit->state & UNIT_USED) == 0 )
            {
                MemJoinNext(page,unit);

                if ( unit->state == unitcount )
                {
                    best = unit;
                    break;
                }
                else if ( unit->state > unitcount )
                {
                    best = unit;
                }
            }

            unit += unit->state & UNIT_COUNT;
        }

        if ( best != NULL )
        {
            ptr = MemAllocHere(page,best,unitcount);
        }
        else
        {
            //Printf("Memory fragmentation too much" crlf);
        }
    }
    return ptr;
}

RSTATIC void RATTRIB  MemInit       (struct sRAR *data, uint unitsize, uint count)
{
    // just store page parameters
    data->MemEntryCount      = 0;
    data->MemPage.prev       = NULL;
    data->MemPage.next       = NULL;
    data->MemPage.free_count = 0;
    data->MemPage.unitsize   = unitsize;
    data->MemPage.count      = count;
    data->MemPage.start      = NULL;
    data->MemPage.size       = 0;
    data->MemPage.units      = NULL;
    data->MemPage.units_end  = NULL;
    data->MemPage.old        = NULL;
}

RSTATIC void * RATTRIB MemAlloc      (tRAR *data, tMEMPAGE *page, uint unitcount)
{
    void *ptr;

    ptr = NULL;
    while ( unitcount && ptr == NULL )
    {
        if ( page->next == NULL )
        {
            if ( MemPageAlloc(data, page) == NULL )
            {
                // no more memory available
                break;
            }
        }

        page = page->next;

        ptr = MemAllocInPage(page,unitcount);
    }
    return ptr;
}

RSTATIC tMEMPAGE * RATTRIB MemFindPage(tMEMPAGE *page, void *ptr)
{

    // find target page
    while ( page != NULL )
    {
        uint unitno;

        unitno = (uint)((byte*)(ptr) - page->start);

        if ( unitno >= page->size )
        {
            page = page->next;
        }
        else
        {
            if ( unitno % page->unitsize )
            {
                //Printf("Trying to find unaligned memory block" crlf);
                page = NULL;
            }
            break;
        }
    }
    return page;
}

#ifdef MEM_DEBUG
RSTATIC void * RATTRIB iMemRealloc    (struct sRAR *data, uint *allocsize, void *oldptr, uint newsize, char *comment)
#else
RSTATIC void * RATTRIB iMemRealloc    (struct sRAR *data, uint *allocsize, void *oldptr, uint newsize)
#endif
{
    tMEMPAGE *page;
    uint unitcount;
    void *ptr;

    #ifdef MEM_DEBUG
    MemComment = comment;
    #endif

    MemCheck(data);

    page = &data->MemPage;
    ptr = NULL;

    unitcount = (newsize+page->unitsize-1) / page->unitsize;

    if ( unitcount > page->count )
    {
        //Printf("Requested allocation size too large" crlf);
    }
    else
    {
        if ( oldptr != NULL )
        {
            page = MemFindPage(page, oldptr);

            if ( page == NULL )
            {
                //Printf("Realloc: unknown memory page" crlf);
            }
            else
            {
                tUNIT *unit;
                uint unitno;
                uint cnt;

                unitno = (uint)((byte*)(oldptr) - page->start) / page->unitsize;
                unit = page->units+unitno;
                cnt  = unit->state & UNIT_COUNT;

                if ( cnt < unitcount )
                {
                    // expand allocated space
                    tUNIT *next;
                    next = unit+cnt;

                    if ( next <page->units_end && (next->state & UNIT_USED) == 0 && next->state >= (unitcount-cnt) )
                    {
                        // allocate at the end
                        unit->state = (word)(unitcount | UNIT_USED);
                        MemAllocHere(page, next, unitcount-cnt);
                        next->state = UNIT_USED;
                        ptr = oldptr;
                    }
                    else
                    {
                        // end allocation failed
                        // allocate new space
                        ptr = MemAlloc(data, &data->MemPage, unitcount);

                        if ( ptr != NULL )
                        {
                            memcpy(ptr,oldptr,cnt*page->unitsize);
                            MemFreeHere(page,unit);
                        }
                    }
                }
                else
                {
                    if ( cnt > unitcount )
                    {
                        // shrink allocated space
                        unit->state = (word)(unitcount | UNIT_USED);
                        unit += unitcount;
                        unit->state = (word)((cnt-unitcount) | UNIT_USED);

                        MemFreeHere(page,unit);
                    }
                    ptr = oldptr;
                }
            }
        }
        else
        {
            ptr = MemAlloc(data, page, unitcount);
        }
    }
    if ( ptr != NULL && allocsize != NULL)
    {
        *allocsize = unitcount * page->unitsize;
    }
    return ptr;
}

RSTATIC void RATTRIB  MemFree       (struct sRAR *data, void *oldptr)
{
    tMEMPAGE *page;
    tUNIT *unit;
    //MemDump(data);
    MemCheck(data);

    page = MemFindPage(&data->MemPage, oldptr);

    if ( page != NULL )
    {
        unit = page->units + (((byte*)(oldptr))-page->start)/page->unitsize;
        MemFreeHere(page, unit);
    }
    else
    {
        //Printf("Free: unknown memory page" crlf);
    }
}

RSTATIC void RATTRIB  MemReset      (struct sRAR *data)
{
    // free all allocated space
    tMEMPAGE *page;

    page = &data->MemPage;

    // find last allocated page
    while ( page->next != NULL )
    {
        page = page->next;
    }

    while ( page->prev != NULL )
    {
        page = page->prev;
        RarFree(data,page->next);
    }
    data->MemPage.next = NULL;
}

RSTATIC void   RATTRIB  MemScan       (struct sRAR *data)
{
    // scan and join free blocks
    tUNIT *unit;
    tMEMPAGE *page;

    page = &data->MemPage;

    while ( page != NULL )
    {
        unit = page->units;

        while ( unit < page->units_end )
        {
            if ( (unit->state & UNIT_USED) == 0 )
            {
                MemJoinNext(page,unit);
            }
            unit += unit->state & UNIT_COUNT;
        }
        page = page->next;
    }
}

RSTATIC void * RATTRIB  MemPtrFix(struct sRAR *data, void *ptr)
{
    void *newptr;
    tMEMPAGE *page;
    page = &data->MemPage;

    newptr = NULL;

    while ( page != NULL )
    {
        uint off;

        off = (uint)((byte*)(ptr) - page->old);

        if ( off < page->size )
        {
            newptr = (void*)(page->start+off);
            break;
        }
        page = page->next;
    }
    return newptr;
}

RSTATIC void RATTRIB ArrayInit(tARRAY *array, uint size)
{
  array->ptr   = NULL;
  array->size  = (tWORD)(size);
  array->count = 0;
}

#ifdef MEM_DEBUG
RSTATIC void * RATTRIB iArrayAdd(tRAR *data, tARRAY *array, uint count, char *comment)
#else
RSTATIC void * RATTRIB iArrayAdd(tRAR *data, tARRAY *array, uint count)
#endif
{
    byte *ptr;
    ptr = MemRealloc(data,NULL,array->ptr,(count+array->count)*array->size, comment);
    if ( ptr != NULL )
    {
        array->ptr = ptr;
        if ( array->count ) ptr +=  array->count*array->size;
        array->count = (word)(array->count+count);
    }
    return ptr;
}

RSTATIC void RATTRIB ArrayDelete(tRAR *data, tARRAY *array)
{
    if ( array->ptr != NULL )
    {
        MemFree(data, array->ptr);
        array->ptr = NULL;
    }
    array->count = 0;
}

RSTATIC rarbool RATTRIB ArrayFix(struct sRAR *data, tARRAY *array)
{
    rarbool rcode;

    rcode = TRUE;
    if ( array->count )
    {
        array->ptr = MemPtrFix(data, array->ptr);
        if ( array->ptr == NULL )
        {
            rcode = FALSE;
        }
    }
    return rcode;
}
