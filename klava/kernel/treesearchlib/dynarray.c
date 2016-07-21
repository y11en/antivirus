// dynarray.c
//
//

#include "dynarray.h"

struct tag_DYN_ARRAY
{
	uint32_t   dwItemSize;
	uint32_t   dwItems;
	uint32_t   dwAllocatedSize;
	sHEAP_CTX* pHeapCtx;
	void *     pData;
};

TREE_ERR CreateDynArray(sHEAP_CTX* pHeapCtx, DYN_ARRAY** ppDynArray, uint32_t dwItemSize)
{
	DYN_ARRAY* pDynArray;
	if (pHeapCtx==NULL || ppDynArray == NULL || dwItemSize==0)
		return TREE_EINVAL;
	pDynArray = _HeapAlloc(pHeapCtx, sizeof(DYN_ARRAY));
	if (pDynArray == NULL)
	{
		*ppDynArray = NULL;
		return TREE_ENOMEM;
	}
	pDynArray->pHeapCtx = pHeapCtx;
	pDynArray->dwItemSize = dwItemSize;
	*ppDynArray = pDynArray;
	return TREE_OK;
}

TREE_ERR DestroyDynArray(DYN_ARRAY** ppDynArray)
{
	DYN_ARRAY* pDynArray;
	if (ppDynArray == NULL)
		return TREE_EINVAL;
	pDynArray = *ppDynArray;
	if (pDynArray == NULL)
		return TREE_EINVAL;

	if (pDynArray->pData)
		_HeapFree(pDynArray->pHeapCtx, pDynArray->pData);
	_HeapFree(pDynArray->pHeapCtx, pDynArray);
	*ppDynArray = NULL;
	return TREE_OK;
}

TREE_ERR DynArraySetItem(DYN_ARRAY* pDynArray, uint32_t dwIndex, void * pItem)
{
	if (pDynArray == NULL)
		return TREE_EINVAL;
	if (pDynArray->dwAllocatedSize < (dwIndex+1) * pDynArray->dwItemSize)
	{
		uint32_t dwAllocSize;
		uint32_t dwIncrement = 1;
		if (pDynArray->dwItems > 1000)
			dwIncrement = 100;
		else
		if (pDynArray->dwItems > 10)
			dwIncrement = 10;
		dwAllocSize = (dwIndex + dwIncrement) * pDynArray->dwItemSize;
		if (pDynArray->pData == NULL)
			pDynArray->pData = _HeapAlloc(pDynArray->pHeapCtx, dwAllocSize);
		else
			pDynArray->pData = _HeapReAlloc(pDynArray->pHeapCtx, pDynArray->pData, dwAllocSize);
		if (pDynArray->pData == NULL)
		{
			pDynArray->dwAllocatedSize = 0;
			pDynArray->dwItems = 0;
			return TREE_ENOMEM;
		}
		pDynArray->dwAllocatedSize = dwAllocSize;
	}
	memcpy((uint8_t*)(pDynArray->pData) + dwIndex * pDynArray->dwItemSize, pItem, pDynArray->dwItemSize);
	if (pDynArray->dwItems <= dwIndex)
		pDynArray->dwItems = dwIndex+1;
	return TREE_OK;
}

TREE_ERR DynArrayGetItem(DYN_ARRAY* pDynArray, uint32_t dwIndex, void * pItem)
{
	if (pDynArray == NULL)
		return TREE_EINVAL;
	if (pDynArray->dwAllocatedSize < (dwIndex+1))
		return TREE_EINVAL;
	memcpy(pItem, (uint8_t*)(pDynArray->pData) + dwIndex * pDynArray->dwItemSize, pDynArray->dwItemSize);
	return TREE_OK;
}

TREE_ERR DynArrayAddItem(DYN_ARRAY* pDynArray, void * pItem)
{
	return DynArraySetItem(pDynArray, pDynArray->dwItems, pItem);
}

TREE_ERR DynArrayPush(DYN_ARRAY* pDynArray, void * pItem)
{
	return DynArraySetItem(pDynArray, pDynArray->dwItems, pItem);
}

TREE_ERR DynArrayPop(DYN_ARRAY* pDynArray, void * pItem)
{
	TREE_ERR error;
	if (pDynArray->dwItems == 0)
		return TREE_ENOTFOUND;
	error = DynArrayGetItem(pDynArray, pDynArray->dwItems-1, pItem);
	if (TREE_SUCCEEDED(error))
		pDynArray->dwItems--;
	return error;
}

uint32_t DynArrayGetCount(DYN_ARRAY* pDynArray)
{
	return pDynArray->dwItems;
}
