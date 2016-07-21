//EventCache header file

#ifndef __EVENT_CACHE
#define __EVENT_CACHE

typedef struct _THashTreeItem
{
	_THashTreeItem*	m_pPrev;
	_THashTreeItem*	m_pNext;
	
	int				m_Flags;
	int				m_Hash;
	
	void*			m_pData;
	int				m_DataLen;
	void*			m_pUserData;
	
	_THashTreeItem*	m_pCommonPrev;
	_THashTreeItem*	m_pCommonNext;
}HashTreeItem, *PHashTreeItem;

typedef struct _THashTree
{
	PHashTreeItem*	m_pItems;
	int				m_AccessBitMask;
	int				m_HashArrSize;
	
	PHashTreeItem	m_pFirst;
	PHashTreeItem	m_pLast;
}HashTree, *PHashTree;

#ifdef __cplusplus
extern "C" {
#endif
	
	void _Hash_Init_Library( void* (*pfMemAlloc)(unsigned int), void (*pfMemFree)(void*), int (*pfMemCompare)(void*, void*, int), void (*pfFreeItemValue)(void*, int, void*));
	PHashTree _Hash_InitNew(int AccessBitLen);
	void _Hash_Done(PHashTree pHash, int nNeedFreeItem);

	void _Hash_Free_AllItems(PHashTree pHash, int nNeedFreeItem);
	
	unsigned int _Hash_Func(void* pData, int nDataLen);
	
	// -----------------------------------------------------------------------------------------
	PHashTreeItem _Hash_AddItem(PHashTree pHash, void* pData, int nDataLen, void* pUserData);
	void _Hash_DelItem(PHashTree pHash, PHashTreeItem pItem, int nNeedFreeItem);
	
	PHashTreeItem _Hash_GetFirst(PHashTree pHash);
	//PHashTreeItem _Hash_GetNext(PHashTreeItem pItem);
	
	PHashTreeItem _Hash_Find(PHashTree pHash, void* pData, int nDataLen);
	
#ifdef __cplusplus
}
#endif

#endif //__EVENT_CACHE