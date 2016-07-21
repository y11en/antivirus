#include "kldef.h"

PVOID 
KlAlloc(size_t size, unsigned long tag /* = 'NeG-' */)
{
    PVOID   memory;

    memory = ExAllocatePoolWithTag( NonPagedPool,  size + sizeof ( size_t ), tag);

    if ( memory )
    {
        *(size_t*)memory = size;
        
        return (PVOID) ( (PCHAR)memory + sizeof( size_t ) );
    }

    return NULL;
}

void 
KlFree(PVOID    memory)
{
    if ( memory )
        KL_MEM_FREE ( (PVOID)( (PCHAR)memory - sizeof(size_t) ) );
}

size_t 
KlMemSize(PVOID memory)
{
    return *(size_t*) ( (PCHAR)memory - sizeof(size_t) );
}

PVOID 
KlRealloc(PVOID OldMemory, size_t NewSize, unsigned long tag)
{
    PVOID  NewMemory   = 0;
    size_t OldSize     = KlMemSize( OldMemory );

    if ( NewSize == 0 )
    {
        KlFree( OldMemory );
        return 0;
    }

    if ( OldMemory == NULL )
    {
        return KlAlloc(NewSize, tag);
    }

    if ( OldSize > NewSize )
    {
        // allocate less than there were before
        NewMemory = KlAlloc( NewSize, tag );
        
        if ( NewMemory )
        {
            RtlCopyMemory ( NewMemory, OldMemory, NewSize );
            KlFree( OldMemory );
            return NewMemory;
        }
        else
        {
            // h.z.

            return 0;
        }
    }
    
    // OldSize <= NewSize
    NewMemory = KlAlloc( NewSize, tag );

    if ( NewMemory )
    {
        RtlCopyMemory ( NewMemory, OldMemory, OldSize );
        KlFree( OldMemory );
        return NewMemory;
    }

    return 0;
}

