/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel NFIO Win32 memory            *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#pragma warning(push,3)
#include <wincompat.h>
#include <prague.h>
#include <kernel/k_mem.h>

//#include <malloc.h>


tMEMHANDLE pr_call MemAlloc   ( tDWORD size );
tMEMHANDLE pr_call MemRealloc ( tMEMHANDLE handle, tDWORD size );
tBOOL      pr_call MemFree    ( tMEMHANDLE handle );
tPTR       pr_call MemLock    ( tMEMHANDLE handle );
tUINT      pr_call MemUnlock  ( tMEMHANDLE handle );
tERROR     pr_call MemInit    ( tVOID );
tERROR     pr_call MemDeinit  ( tVOID );


iMEM NfioMemoryTable = {
  MemAlloc  ,
  MemRealloc,
  MemFree   ,
  MemLock   ,
  MemUnlock ,
  MemInit   ,
  MemDeinit ,
};


HANDLE process_heap;

// --- 
tERROR pr_call MemInit( tVOID ) {
  process_heap = GetProcessHeap();
  if ( process_heap )
    return errOK;
  else
    return errOBJECT_CANNOT_BE_INITIALIZED;
}



// ---
tERROR pr_call MemDeinit( tVOID ) {
  return errOK;
}



// --- 
tMEMHANDLE pr_call MemRealloc( tMEMHANDLE handle, tDWORD size ) {
  return HeapReAlloc( process_heap, HEAP_ZERO_MEMORY, handle, size );
}



// ---
tMEMHANDLE pr_call MemAlloc( tDWORD size ) {
  tMEMHANDLE r;
  __try {
    r = HeapAlloc( process_heap, HEAP_ZERO_MEMORY, size );
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
    r = 0;
  }
  return r;
}



// ---
tBOOL pr_call MemFree( tMEMHANDLE handle ) {
  __try {
    HeapFree( process_heap, 0, handle );
    return cTRUE;
  }
  __except( EXCEPTION_EXECUTE_HANDLER ) {
    return cFALSE;
  }
}




// ---
tPTR pr_call MemLock( tMEMHANDLE handle ) {
  return (tPTR)handle;
}



// ---
tUINT pr_call MemUnlock( tMEMHANDLE handle ) {
  return 1;
}



