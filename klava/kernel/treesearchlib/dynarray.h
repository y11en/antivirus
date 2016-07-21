// dynarray.h
//

#ifndef dynarray_h_INCLUDED
#define dynarray_h_INCLUDED

#include "common.h"

typedef struct tag_DYN_ARRAY DYN_ARRAY;

TREE_ERR CreateDynArray(sHEAP_CTX* pHeapCtx, DYN_ARRAY** ppDynArray, uint32_t dwItemSize);
TREE_ERR DestroyDynArray(DYN_ARRAY** ppDynArray);
TREE_ERR DynArraySetItem(DYN_ARRAY* pDynArray, uint32_t dwIndex, void * pItem);
TREE_ERR DynArrayGetItem(DYN_ARRAY* pDynArray, uint32_t dwIndex, void * pItem);
TREE_ERR DynArrayAddItem(DYN_ARRAY* pDynArray, void * pItem);
TREE_ERR DynArrayPush(DYN_ARRAY* pDynArray, void * pItem);
TREE_ERR DynArrayPop(DYN_ARRAY* pDynArray, void * pItem);
uint32_t DynArrayGetCount(DYN_ARRAY* pDynArray);

#endif // dynarray_h_INCLUDED


