// klavtree.cpp
//

#include <klava/klavtree.h>

#include <string.h>

#ifdef _MSC_VER
#pragma intrinsic(memset)
#endif

////////////////////////////////////////////////////////////////
// Heap context

/* REALLOC_ZEROINIT version

struct HeapBlock
{
	uint32_t size;
	uint32_t pad;
};

void * _TREE_CALL KlavTreeAlloc (void *pHeap, unsigned int dwBytes)
{
	KLAV_Alloc *alloc = (KLAV_Alloc *)pHeap;
	HeapBlock *blk = (HeapBlock *)alloc->alloc (dwBytes + sizeof (HeapBlock));
	blk->size = dwBytes;
	blk->pad  = 0;
	::memset (blk + 1, 0, dwBytes);
	return blk + 1;
}

void * _TREE_CALL KlavTreeRealloc (void *pHeap, void *pMem, unsigned int dwBytes)
{
	if (pMem == 0)
	{
		return KlavTreeAlloc (pHeap, dwBytes);
	}
	KLAV_Alloc *alloc = (KLAV_Alloc *)pHeap;
	HeapBlock *blk = (HeapBlock *)pMem - 1;
	uint32_t old_size = blk->size;

	HeapBlock *nblk = (HeapBlock *)alloc->realloc (blk, dwBytes + sizeof (HeapBlock));
	nblk->size = dwBytes;
	nblk->pad = 0;

	if (dwBytes > old_size)
	{
		uint8_t *p = (uint8_t *)(nblk + 1);
		::memset (p + old_size, 0, dwBytes - old_size);
	}

	return nblk + 1;
}

void _TREE_CALL KlavTreeFree (void *pHeap, void *pMem)
{
	if (pMem != 0)
	{
		KLAV_Alloc *alloc = (KLAV_Alloc *)pHeap;
		HeapBlock *blk = (HeapBlock *)pMem - 1;
		alloc->free (blk);
	}
}

*/

void * _TREE_CALL KlavTreeAlloc (void *pHeap, unsigned int dwBytes)
{
	KLAV_Alloc *alloc = (KLAV_Alloc *)pHeap;

	void * data = alloc->alloc (dwBytes);
	if (data != 0)
	{
		::memset (data, 0, dwBytes);
	}

	return data;
}

void * _TREE_CALL KlavTreeRealloc (void *pHeap, void *pMem, unsigned int dwBytes)
{
	KLAV_Alloc *alloc = (KLAV_Alloc *)pHeap;
	return alloc->realloc (pMem, dwBytes);
}

void _TREE_CALL KlavTreeFree (void *pHeap, void *pMem)
{
	KLAV_Alloc *alloc = (KLAV_Alloc *)pHeap;
	alloc->free (pMem);
}

void KlavTreeHeapInit (sHEAP_CTX *pctx, KLAV_Alloc *alloc)
{
	pctx->pHeap = alloc;
	pctx->Alloc = KlavTreeAlloc;
	pctx->Realloc = KlavTreeRealloc;
	pctx->Free = KlavTreeFree;
}


