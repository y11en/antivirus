#include "stdafx.h"

#include "EvCache.h"

void* (* _Hash_Alloc)(unsigned int) = 0;
void  (* _Hash_Free)(void*) = 0;
int (* _Hash_Compare)(void*, void*, int) = 0;

void (* _Hash_FreeItemValue)(void*, int, void*) = 0;

void _Hash_Init_Library( void* (*pfMemAlloc)(unsigned int), void (*pfMemFree)(void*), int (*pfMemCompare)(void*, void*, int), void (*pfFreeItemValue)(void*, int, void*))
{
	_Hash_Alloc = pfMemAlloc;
	_Hash_Free = pfMemFree;
	_Hash_Compare = pfMemCompare;
	_Hash_FreeItemValue = pfFreeItemValue;
}

// -----------------------------------------------------------------------------------------

unsigned int _Hash_Func(void* pData, int nDataLen)
{
	return (unsigned int)*(DWORD*)pData;
}

// -----------------------------------------------------------------------------------------

PHashTree _Hash_InitNew(int AccessBitLen)
{
	if ((_Hash_Alloc == 0) || (_Hash_Free == 0))
		return 0;
	
	if (AccessBitLen == 0)
		return 0;
	
	PHashTree pHash = (PHashTree) _Hash_Alloc(sizeof(HashTree));
	if (pHash != 0)
	{
		pHash->m_HashArrSize = 1 << AccessBitLen;

		pHash->m_pFirst = 0;		
		pHash->m_pLast = 0;

		pHash->m_pItems = (PHashTreeItem*) _Hash_Alloc(pHash->m_HashArrSize * sizeof(PHashTreeItem));
		for (int cou = 0; cou < pHash->m_HashArrSize; cou++)
			pHash->m_pItems[cou] = 0;
		
		pHash->m_AccessBitMask = pHash->m_HashArrSize - 1;
	}
	
	return pHash;
}

void _Hash_Done(PHashTree pHash, int nNeedFreeItem)
{
	if (pHash == 0)
		return;
	
	PHashTreeItem *pItemList;
	PHashTreeItem pItem;
	PHashTreeItem pItemTmp;
	for (int cou = 0; cou < (pHash->m_AccessBitMask + 1); cou++)
	{
		pItemList = &(pHash->m_pItems[cou]);
		pItem = *pItemList;
		while (pItem != 0)
		{
			pItemTmp = pItem;
			pItem = pItem->m_pNext;

			if (nNeedFreeItem != 0)
				_Hash_FreeItemValue(pItemTmp->m_pData, pItemTmp->m_DataLen, pItemTmp->m_pUserData);
			_Hash_Free(pItemTmp);
		}
		
	}
	
	_Hash_Free(pHash->m_pItems);
	_Hash_Free(pHash);
}

void _Hash_Free_AllItems(PHashTree pHash, int nNeedFreeItem)
{
	if (pHash == 0)
		return;
	
	PHashTreeItem *pItemList;
	PHashTreeItem pItem;
	PHashTreeItem pItemTmp;
	for (int cou = 0; cou < (pHash->m_AccessBitMask + 1); cou++)
	{
		pItemList = &(pHash->m_pItems[cou]);
		pItem = *pItemList;
		while (pItem != 0)
		{
			pItemTmp = pItem;
			pItem = pItem->m_pNext;
			
			if (nNeedFreeItem != 0)
				_Hash_FreeItemValue(pItemTmp->m_pData, pItemTmp->m_DataLen, pItemTmp->m_pUserData);
			_Hash_Free(pItemTmp);
		}
		
	}
	
	pHash->m_pFirst = 0;		
	pHash->m_pLast = 0;
	for (cou = 0; cou < pHash->m_HashArrSize; cou++)
		pHash->m_pItems[cou] = 0;
}

// -----------------------------------------------------------------------------------------
PHashTreeItem _Hash_AllocItem(void *pData, int nDataLen, void* pUserData, int Hash)
{
	PHashTreeItem pItem = (PHashTreeItem) _Hash_Alloc(sizeof(HashTreeItem));
	if (pItem != 0)
	{
		pItem->m_DataLen = nDataLen;
		pItem->m_Flags = 0;
		pItem->m_Hash = Hash;
		pItem->m_pPrev = 0;
		pItem->m_pNext = 0;
		pItem->m_pData = pData;
		pItem->m_pUserData = pUserData;

		pItem->m_pCommonPrev = 0;
		pItem->m_pCommonNext = 0;
	}
	return pItem;
}

// -----------------------------------------------------------------------------------------

PHashTreeItem _Hash_AddItem(PHashTree pHash, void* pData, int nDataLen, void* pUserData)
{
	if (pHash == 0)
		return 0;
	
	PHashTreeItem pItem = 0;
	int Hash = _Hash_Func(pData, nDataLen);
	PHashTreeItem *pItemList = &(pHash->m_pItems[Hash & pHash->m_AccessBitMask]);
	if (*pItemList == 0)
	{
		pItem = _Hash_AllocItem(pData, nDataLen, pUserData, Hash);
		if (pItem != 0)
			*pItemList = pItem;
	}
	else
	{
		PHashTreeItem pItemLast = 0;
		pItem = *pItemList;
		char cFound = 0;
		while ((pItem != 0) && (cFound == 0))
		{
			if (pItem->m_DataLen == nDataLen)
				if (_Hash_Compare(pItem->m_pData, pData, nDataLen) == 0)
					cFound = 1;
			pItemLast = pItem;
			pItem = pItem->m_pNext;
		}
		
		if (cFound == 0)
		{
			pItem = _Hash_AllocItem(pData, nDataLen, pUserData, Hash);
			if (pItem != 0)
			{
				pItemLast->m_pNext = pItem;
				pItem->m_pPrev = pItemLast;
			}
		}
		else
			pItem = 0;
			
	}
	// add to common list
	if (pItem != 0)
	{
		pItem->m_pCommonPrev = pHash->m_pLast;
		if (pHash->m_pLast != 0)
			pHash->m_pLast->m_pCommonNext = pItem;
		pHash->m_pLast = pItem;

		if (pHash->m_pFirst == 0)
			pHash->m_pFirst = pItem;
	}
	return pItem;
}

void _Hash_DelItem(PHashTree pHash, PHashTreeItem pItem, int nNeedFreeItem)
{
	if (pHash == 0)
		return;
	
	if (pItem->m_pPrev != 0)
	{
		pItem->m_pPrev->m_pNext = pItem->m_pNext;
		if (pItem->m_pNext != 0)
			pItem->m_pNext->m_pPrev = pItem->m_pPrev;
	}
	else
	{
		PHashTreeItem *pItemList = &(pHash->m_pItems[pItem->m_Hash & pHash->m_AccessBitMask]);
		*pItemList = pItem->m_pNext;
		if (pItem->m_pNext != 0)
			pItem->m_pNext->m_pPrev = 0;
	}

	if (pHash->m_pLast == pItem)
	{
		pHash->m_pLast = pItem->m_pCommonPrev;
		if (pItem->m_pCommonPrev != 0)
			pItem->m_pCommonPrev->m_pCommonNext = 0;
		else
			pHash->m_pFirst = 0;
	}
	else if (pHash->m_pFirst == pItem)
	{
		pHash->m_pFirst = pItem->m_pCommonNext;
		if (pItem->m_pCommonNext != 0)
			pItem->m_pCommonNext->m_pCommonPrev = pItem->m_pCommonPrev;
		else
			pHash->m_pLast = 0;
	}
	else
	{
		pItem->m_pCommonPrev->m_pCommonNext = pItem->m_pCommonNext;
		pItem->m_pCommonNext->m_pCommonPrev = pItem->m_pCommonPrev;
	}
	
	
	if (nNeedFreeItem != 0)
		_Hash_FreeItemValue(pItem->m_pData, pItem->m_DataLen, pItem->m_pUserData);
	_Hash_Free(pItem);
}

// -----------------------------------------------------------------------------------------
PHashTreeItem _Hash_GetFirst(PHashTree pHash)
{
	if (pHash == 0)
		return 0;
	
	return pHash->m_pFirst;
}

PHashTreeItem _Hash_GetNext(PHashTreeItem pItem)
{
	return pItem->m_pCommonNext;
}

PHashTreeItem _Hash_Find(PHashTree pHash, void* pData, int nDataLen)
{
	if (pHash == 0)
		return 0;
	
	PHashTreeItem pItem;
	int Hash = _Hash_Func(pData, nDataLen);
	PHashTreeItem *pItemList = &(pHash->m_pItems[Hash & pHash->m_AccessBitMask]);
	if (*pItemList == 0)
		return 0;
	else
	{
		pItem = *pItemList;
		while (pItem != 0)
		{
			if (pItem->m_DataLen == nDataLen)
			{
				if (_Hash_Compare(pItem->m_pData, pData, nDataLen) == 0)
					return pItem;
			}
			pItem = pItem->m_pNext;
		}
	}

	return 0;
}