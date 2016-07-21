#include "EvCache.h"

// -----------------------------------------------------------------------------------------

unsigned int _Hash_Func(void* pData, int nDataLen)
{
	unsigned int dwSum = 0;
	while ((nDataLen) >= 0)
		dwSum += (*((char*)pData) + nDataLen--);
	
	return dwSum;
}

// -----------------------------------------------------------------------------------------

PHashTree
_Hash_InitNew (
	void* pContext,
	_Hash_Alloc pfAlloc,
	_Hash_Free pfFree,
	_Hash_Compare pfCompare,
	int AccessBitLen
	)
{
	if (!pfAlloc || !pfFree || !pfCompare)
		return 0;
	
	if (!AccessBitLen)
		return 0;
	
	PHashTree pHash = (PHashTree) pfAlloc( pContext, sizeof(HashTree), 0 );
	if (!pHash)
		return 0;

	pHash->m_pContext = pContext;
	pHash->m_pfAlloc = pfAlloc;
	pHash->m_pfFree = pfFree;
	pHash->m_pfCompare = pfCompare;

	int HashSize = 1 << AccessBitLen;
	
	pHash->m_ItemsCount = 0;

	pHash->m_pFirst = 0;		
	pHash->m_pLast = 0;

	pHash->m_pItems = (PHashTreeItem*) pfAlloc( pContext, HashSize * sizeof(PHashTreeItem), 0 );
	for (int cou = 0; cou < HashSize; cou++)
		pHash->m_pItems[cou] = 0;
	
	pHash->m_AccessBitMask = HashSize - 1;
	
	return pHash;
}

void
_Hash_Done (
	PHashTree pHash
	)
{
	if (!pHash)
		return;
	
	PHashTreeItem *pItemList;
	PHashTreeItem pItem;
	PHashTreeItem pItemTmp;
	
	for (int cou = 0; cou < (pHash->m_AccessBitMask + 1); cou++)
	{
		pItemList = &(pHash->m_pItems[cou]);
		pItem = *pItemList;
		while (pItem)
		{
			pItemTmp = pItem;
			pItem = pItem->m_pNext;

			pHash->m_pfFree( pHash->m_pContext, (void**) &pItemTmp );
		}
		
	}

	pHash->m_pfFree( pHash->m_pContext, (void**) &pHash->m_pItems );
	pHash->m_pfFree( pHash->m_pContext, (void**) &pHash );
}

// -----------------------------------------------------------------------------------------
PHashTreeItem
_Hash_AllocItem (
	PHashTree pHash,
	void *pData,
	int nDataLen,
	void* pUserData,
	int Hash
	)
{
	PHashTreeItem pItem = (PHashTreeItem) pHash->m_pfAlloc( pHash->m_pContext, sizeof(HashTreeItem), 0 );
	if (!pItem)
		return 0;

	pItem->m_DataLen = nDataLen;
	pItem->m_Flags = 0;
	pItem->m_Hash = Hash;
	pItem->m_pPrev = 0;
	pItem->m_pNext = 0;
	pItem->m_pData = pData;
	pItem->m_pUserData = pUserData;

	pItem->m_pCommonPrev = 0;
	pItem->m_pCommonNext = 0;

	return pItem;
}

// -----------------------------------------------------------------------------------------

PHashTreeItem
_Hash_AddItem (
	PHashTree pHash,
	void* pData,
	int nDataLen,
	void* pUserData
	)
{
	if (!pHash)
		return 0;
	
	PHashTreeItem pItem = 0;
	
	int Hash = _Hash_Func( pData, nDataLen );
	
	PHashTreeItem *pItemList = &(pHash->m_pItems[Hash & pHash->m_AccessBitMask]);
	if (!*pItemList)
	{
		pItem = _Hash_AllocItem( pHash, pData, nDataLen, pUserData, Hash );
		if (pItem)
			*pItemList = pItem;
	}
	else
	{
		PHashTreeItem pItemLast = 0;
		pItem = *pItemList;
		char cFound = 0;
		while (pItem && !cFound)
		{
			if (pItem->m_DataLen == nDataLen)
			{
				if (!pHash->m_pfCompare( pHash->m_pContext, pItem->m_pData, pData, nDataLen ))
					cFound = 1;
			}
			
			pItemLast = pItem;
			pItem = pItem->m_pNext;
		}
		
		if (!cFound)
		{
			pItem = _Hash_AllocItem( pHash, pData, nDataLen, pUserData, Hash );
			if (pItem)
			{
				pItemLast->m_pNext = pItem;
				pItem->m_pPrev = pItemLast;
			}
		}
		else
			pItem = 0;
			
	}
	// add to common list
	
	if (pItem)
	{
		pItem->m_pCommonPrev = pHash->m_pLast;
		if (pHash->m_pLast)
			pHash->m_pLast->m_pCommonNext = pItem;
		pHash->m_pLast = pItem;

		if (!pHash->m_pFirst)
			pHash->m_pFirst = pItem;
		
		pHash->m_ItemsCount++;
	}
	
	return pItem;
}

void
_Hash_DelItem (
	PHashTree pHash,
	PHashTreeItem pItem
	)
{
	if (!pHash)
		return;
	
	if (pItem->m_pPrev)
	{
		pItem->m_pPrev->m_pNext = pItem->m_pNext;
		if (pItem->m_pNext)
			pItem->m_pNext->m_pPrev = pItem->m_pPrev;
	}
	else
	{
		PHashTreeItem *pItemList = &(pHash->m_pItems[pItem->m_Hash & pHash->m_AccessBitMask]);
		*pItemList = pItem->m_pNext;
		if (pItem->m_pNext)
			pItem->m_pNext->m_pPrev = 0;
	}

	if (pHash->m_pLast == pItem)
	{
		pHash->m_pLast = pItem->m_pCommonPrev;
		if (pItem->m_pCommonPrev)
			pItem->m_pCommonPrev->m_pCommonNext = 0;
		else
			pHash->m_pFirst = 0;
	}
	else if (pHash->m_pFirst == pItem)
	{
		pHash->m_pFirst = pItem->m_pCommonNext;
		if (pItem->m_pCommonNext)
			pItem->m_pCommonNext->m_pCommonPrev = 0;
		else
			pHash->m_pLast = 0;
	}
	else
	{
		pItem->m_pCommonPrev->m_pCommonNext = pItem->m_pCommonNext;
		pItem->m_pCommonNext->m_pCommonPrev = pItem->m_pCommonPrev;
	}

	pHash->m_ItemsCount--;
	
	pHash->m_pfFree( pHash->m_pContext, (void**) &pItem );
}

// -----------------------------------------------------------------------------------------
PHashTreeItem
_Hash_GetFirst (
	PHashTree pHash
	)
{
	if (!pHash)
		return 0;
	
	return pHash->m_pFirst;
}

PHashTreeItem
_Hash_GetNext (
	PHashTreeItem pItem
	)
{
	return pItem->m_pCommonNext;
}

PHashTreeItem
_Hash_Find (
	PHashTree pHash,
	void* pData,
	int nDataLen
	)
{
	if (!pHash)
		return 0;
	
	PHashTreeItem pItem;
	int Hash = _Hash_Func(pData, nDataLen);

	PHashTreeItem *pItemList = &(pHash->m_pItems[Hash & pHash->m_AccessBitMask]);

	if (!*pItemList)
		return 0;
	else
	{
		pItem = *pItemList;
		while (pItem)
		{
			if (pItem->m_DataLen == nDataLen)
			{
				if (!pHash->m_pfCompare( pHash->m_pContext, pItem->m_pData, pData, nDataLen ))
					return pItem;
			}
			pItem = pItem->m_pNext;
		}
	}

	return 0;
}