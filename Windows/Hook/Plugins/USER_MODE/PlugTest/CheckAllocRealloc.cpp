#include "kl_object.h"
#include "kldef.h"
#include "kl_alloc.h"

void
CheckAllocRealloc()
{
    printf ("*********************************************************************\n");
    printf ("*******************Check_AllocRealloc********************************\n");
    printf ("*********************************************************************\n");

    PVOID   mem1, mem2;
    ULONG   AllocSize = 10;
    char*   AllocStr = "HelloWorld";
    ULONG   RellocSize1 = 0,
            RellocSize2 = 5,
            RellocSize3 = AllocSize,
            RellocSize4 = 20;
    
    mem1 = KlAlloc( 0 );
    printf ("KlAlloc(0) = %x\n", mem1 );
    KlFree ( mem1 );

    if ( mem1 = KlAlloc( AllocSize ) )
    {
        memcpy ( mem1, AllocStr, min ( AllocSize, strlen (AllocStr) ) );
        ASSERT ( KlMemSize(mem1) == AllocSize )

        
        printf("allocated %d bytes = %x\n", KlMemSize(mem1), mem1 );
        KlFree ( mem1 );
    }

    if ( mem1 = KlAlloc( AllocSize ) )
    {
        memcpy ( mem1, AllocStr, min ( AllocSize, strlen (AllocStr) ) );
        ASSERT ( KlMemSize(mem1) == AllocSize )
        
        printf("allocated %d bytes = %x\n", KlMemSize(mem1), mem1 );

        mem2 = KlRealloc( mem1, RellocSize1 );
        
        printf("realloc %x to %x; mem2=%x\n", AllocSize, RellocSize1, mem2);

        KlFree(mem2);
    }

    if ( mem1 = KlAlloc( AllocSize ) )
    {
        memcpy ( mem1, AllocStr, min ( AllocSize, strlen (AllocStr) ) );
        ASSERT ( KlMemSize(mem1) == AllocSize )
        
        printf("allocated %d bytes = %x\n", KlMemSize(mem1), mem1 );

        mem2 = KlRealloc( mem1, RellocSize2 );
        
        printf("realloc %x to %x; mem2=%x\n", AllocSize, RellocSize1, mem2);

        KlFree(mem2);
    }

    if ( mem1 = KlAlloc( AllocSize ) )
    {
        memcpy ( mem1, AllocStr, min ( AllocSize, strlen (AllocStr) ) );
        ASSERT ( KlMemSize(mem1) == AllocSize )
        
        printf("allocated %d bytes = %x\n", KlMemSize(mem1), mem1 );

        mem2 = KlRealloc( mem1, RellocSize3 );
        
        printf("realloc %x to %x; mem2=%x\n", AllocSize, RellocSize1, mem2);

        KlFree(mem2);
    }

    if ( mem1 = KlAlloc( AllocSize ) )
    {
        memcpy ( mem1, AllocStr, min ( AllocSize, strlen (AllocStr) ) );
        ASSERT ( KlMemSize(mem1) == AllocSize )
        
        printf("allocated %d bytes = %x\n", KlMemSize(mem1), mem1 );

        mem2 = KlRealloc( mem1, RellocSize4 );
        
        printf("realloc %x to %x; mem2=%x\n", AllocSize, RellocSize1, mem2);

        KlFree(mem2);
    }
}