//EventCache header file
#ifndef __EVENT_CACHE
#define __EVENT_CACHE

typedef void* (__cdecl *_Hash_Alloc)(void* pContext, unsigned int size, unsigned long tag);
typedef void  (__cdecl *_Hash_Free)(void* pContext, void** ppData);
typedef int (__cdecl *_Hash_Compare)(void* pContext, void* pData1, void* pData2, int DataLen);

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
	void*			m_pContext;
	_Hash_Alloc		m_pfAlloc;
	_Hash_Free		m_pfFree;
	_Hash_Compare	m_pfCompare;

	PHashTreeItem*	m_pItems;
	int				m_AccessBitMask;
	
	PHashTreeItem	m_pFirst;
	PHashTreeItem	m_pLast;
	
	unsigned int	m_ItemsCount;
}HashTree, *PHashTree;

#ifdef __cplusplus
extern "C" {
#endif
	PHashTree
		_Hash_InitNew (
			void* pContext,
			_Hash_Alloc pfAlloc,
			_Hash_Free pfFree,
			_Hash_Compare pfCompare,
			int AccessBitLen
			);

	void
		_Hash_Done (
			PHashTree pHash
			);
	
	unsigned int
		_Hash_Func (
			void* pData,
			int nDataLen
			);
	
	// -----------------------------------------------------------------------------------------
	PHashTreeItem
		_Hash_AddItem (
			PHashTree pHash,
			void* pData,
			int nDataLen,
			void* pUserData
			);
	
	void
		_Hash_DelItem (
			PHashTree pHash,
			PHashTreeItem pItem
			);
	
	PHashTreeItem
		_Hash_GetFirst (
			PHashTree pHash
			);
	
	PHashTreeItem
		_Hash_Find (
			PHashTree pHash,
			void* pData,
			int nDataLen
			);
	
#ifdef __cplusplus
}
#endif

#endif //__EVENT_CACHE