#include "EvCache.h"

#include <Prague/prague.h>

void* (* _Hash_Alloc)(hOBJECT,void**,unsigned int) = 0;
void  (* _Hash_Free)(hOBJECT,void*) = 0;

void _Hash_Init_Library( void* (*pfMemAlloc)(hOBJECT,void**,unsigned int), void (*pfMemFree)(hOBJECT,void*)/*, int (*pfMemCompare)(void*, void*, int), void (*pfFreeItemValue)(void*, int, void*)*/)
{
	_Hash_Alloc = pfMemAlloc;
	_Hash_Free = pfMemFree;
}

// -----------------------------------------------------------------------------------------

PHashTree _Hash_InitNew(hOBJECT hObj,int AccessBitLen)
{
	if ((_Hash_Alloc == 0) || (_Hash_Free == 0))
		return 0;
	
	if (AccessBitLen == 0)
		return 0;
	
	PHashTree pHash=NULL;
	pHash = (PHashTree) _Hash_Alloc(hObj,(void**)&pHash,sizeof(HashTree));
	if (pHash != 0)
	{
		int HashSize = 1 << AccessBitLen;

		pHash->m_pItems = (PHashTreeItemArray*) _Hash_Alloc(hObj,(void**)&pHash->m_pItems,HashSize * sizeof(PHashTreeItem));
		
		pHash->m_AccessBitMask = HashSize - 1;
	}
	return pHash;
}

void _Hash_Done(hOBJECT hObj,PHashTree pHash)
{
	if (!pHash)
		return;
	_Hash_DelAllItems(hObj,pHash);
	_Hash_Free(hObj,pHash->m_pItems);
	_Hash_Free(hObj,pHash);
}

void _Hash_DelAllItems(hOBJECT hObj,PHashTree pHash)
{
	if (!pHash)
		return;
	PHashTreeItemArray* pItemList;
	for (int cou = 0; cou < (pHash->m_AccessBitMask + 1); cou++)
	{
		pItemList = &(pHash->m_pItems[cou]);
		if (*pItemList)
			_Hash_Free(hObj,*pItemList);
		pHash->m_pItems[cou]=0;
	}
}

// -----------------------------------------------------------------------------------------
void _Hash_AllocItem(hOBJECT hObj,PHashTreeItemArray* pItem,DWORD HashTreeItemCount,DWORD m_pCRCh,DWORD m_pCRCl,bool bSubLevelUse)
{
	if (!pItem) return;
	*pItem = (PHashTreeItemArray) _Hash_Alloc(hObj,(void**)pItem,sizeof(DWORD)+sizeof(HashTreeItem)*(HashTreeItemCount+1));
	if (*pItem != 0)
	{
		(*pItem)->m_pItem[HashTreeItemCount].m_pCRCh=m_pCRCh;
		(*pItem)->m_pItem[HashTreeItemCount].m_pCRCl=m_pCRCl;
		if (bSubLevelUse)
			(*pItem)->m_pItem[HashTreeItemCount].m_pSubLevelUse=1;
		else
			(*pItem)->m_pItem[HashTreeItemCount].m_pSubLevelUse=0;
		(*pItem)->Count++;
	}
	return;
}

// -----------------------------------------------------------------------------------------

DWORD _Hash_AddItem(hOBJECT pObj,PHashTree pHash,DWORD m_pCRCh,DWORD m_pCRCl,bool bSubLevelUse)
{
	int					Count=0;
	if (!pHash)
		return errHASH_ERROR;
	PHashTreeItemArray	*pItemList=&(pHash->m_pItems[m_pCRCh & pHash->m_AccessBitMask]);
	if (*pItemList!=0)
	{
		for (int cou=0;cou<(*pItemList)->Count;cou++)
		{
			if (((*pItemList)->m_pItem[cou].m_pCRCh==m_pCRCh)&((*pItemList)->m_pItem[cou].m_pCRCl==m_pCRCl)) 
				return errHASH_FOUND;
		}
		Count=(*pItemList)->Count;
	}
	_Hash_AllocItem(pObj,pItemList,Count,m_pCRCh,m_pCRCl,bSubLevelUse);
	if (*pItemList == NULL) 
		return errHASH_ERROR;
	return errHASH_ADDED;
}

DWORD _Hash_Find(PHashTree pHash,DWORD m_pCRCh,DWORD m_pCRCl,bool* bSubLevelUse)
{
	if (!pHash)
		return errHASH_ERROR;
	PHashTreeItemArray	*pItemList=&(pHash->m_pItems[m_pCRCh & pHash->m_AccessBitMask]);
	if (*pItemList!=0)
	{
		for (int cou=0;cou<(*pItemList)->Count;cou++)
		{
			if (((*pItemList)->m_pItem[cou].m_pCRCh==m_pCRCh)&((*pItemList)->m_pItem[cou].m_pCRCl==m_pCRCl)) 
			{
				if (bSubLevelUse)
					*bSubLevelUse=!!(*pItemList)->m_pItem[cou].m_pSubLevelUse;
				return errHASH_FOUND;
			}
		}
	}
	return errHASH_ERROR;
}