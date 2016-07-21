//EventCache header file
#ifndef __EVENT_CACHE
#define __EVENT_CACHE

#include "windows.h"

#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#define errHASH_ERROR	0
#define errHASH_FOUND	1
#define errHASH_ADDED	2


typedef struct _THashTreeItem
{
	DWORD			m_pCRCh;
	DWORD			m_pCRCl;
	BYTE			m_pSubLevelUse;
}HashTreeItem, *PHashTreeItem;

typedef struct _THashTreeItemArray
{
	int				Count;
	HashTreeItem	m_pItem[1];
}THashTreeItemArray, *PHashTreeItemArray;

typedef struct _THashTree
{
	PHashTreeItemArray*	m_pItems;
	int					m_AccessBitMask;
}HashTree, *PHashTree;

#ifdef __cplusplus
extern "C" {
#endif
	
	void _Hash_Init_Library( void* (*pfMemAlloc)(hOBJECT,void**,unsigned int), void (*pfMemFree)(hOBJECT,void*)/*, int (*pfMemCompare)(void*, void*, int), void (*pfFreeItemValue)(void*, int, void*)*/);
	PHashTree _Hash_InitNew(hOBJECT hObj,int AccessBitLen);
	void _Hash_Done(hOBJECT hObj,PHashTree pHash);
	
	unsigned int _Hash_Func(void* pData, int nDataLen);
	
	// -----------------------------------------------------------------------------------------
	DWORD _Hash_AddItem(hOBJECT pObj,PHashTree pHash,DWORD m_pCRCh,DWORD m_pCRCl,bool bSubLevelUse);
	void _Hash_DelAllItems(hOBJECT hObj,PHashTree pHash);
	
	DWORD _Hash_Find(PHashTree pHash,DWORD m_pCRCh,DWORD m_pCRCl,bool* bSubLevelUse);
	
#ifdef __cplusplus
}
#endif

#endif //__EVENT_CACHE