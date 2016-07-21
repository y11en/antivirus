// klavtree.h
//

#ifndef klavtree_h_INCLUDED
#define klavtree_h_INCLUDED

#include <klava/klavdef.h>

#include "../../klava/kernel/treesearchlib/tree.h"

#define KLAV_TREE_XOR_BYTE 0x5A

#define KLAV_TREE_ESCAPE_BYTE  0x9B

#define KLAV_TREE_CODE_AND     0x00
#define KLAV_TREE_CODE_RANGE   0x10
#define KLAV_TREE_CODE_ASTER   0x11

// Heap allocator implementation (on top of KLAV_Alloc)
KLAV_EXTERN_C void * _TREE_CALL KlavTreeAlloc (void *pHeap, unsigned int dwBytes);
KLAV_EXTERN_C void * _TREE_CALL KlavTreeRealloc (void *pHeap, void *pMem, unsigned int dwBytes);
KLAV_EXTERN_C void   _TREE_CALL KlavTreeFree (void *pHeap, void *pMem);

KLAV_EXTERN_C void KlavTreeHeapInit (sHEAP_CTX *pctx, KLAV_Alloc *alloc);

#endif // klavtree_h_INCLUDED


