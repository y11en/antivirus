  #include "DrvEventList.h"
#include "DefList.h"

typedef struct _DRV_EVENT_LISTITEM
{
    LIST_ENTRY			m_List;
    BYTE*				m_Event;
} DRV_EVENT_LISTITEM, *PDRV_EVENT_LISTITEM;

DrvEventList::DrvEventList()
{
	m_Errors = 0;
	m_ErrorFlags = 0;
	m_HeapCounter = 0;

	InitializeListHead(&m_EventList);
	InitializeCriticalSection(&m_Sync);
}

DrvEventList::~DrvEventList()
{
	DeleteCriticalSection(&m_Sync);
}

//+ ----------------------------------------------------------------------------------------

void DrvEventList::SetError(ULONG ErrFalg)
{
	m_ErrorFlags |= ErrFalg;
	InterlockedIncrement(&m_Errors);
}

//+ ----------------------------------------------------------------------------------------

BYTE* DrvEventList::Alloc(ULONG size)
{
	BYTE* ptr = (BYTE*) HeapAlloc(GetProcessHeap(), 0, size);
	if (ptr != NULL)
		InterlockedIncrement(&m_HeapCounter);

	return ptr;
}

void DrvEventList::Free(BYTE* ptr)
{
	if (ptr == 0)
		return;

	InterlockedDecrement(&m_HeapCounter);
	HeapFree(GetProcessHeap(), 0, ptr);
}

//+ ----------------------------------------------------------------------------------------

bool DrvEventList::AddNew(BYTE* ptr)
{
	PDRV_EVENT_LISTITEM pNewItem = (PDRV_EVENT_LISTITEM) Alloc(sizeof(DRV_EVENT_LISTITEM));
	if (pNewItem == NULL)
		return false;

	EnterCriticalSection(&m_Sync);
	pNewItem->m_Event = ptr;
	InsertTailList(&m_EventList, &pNewItem->m_List);
	LeaveCriticalSection(&m_Sync);

	return true;
}

BYTE* DrvEventList::GetFirst()
{
	BYTE* pRet = 0;

	EnterCriticalSection(&m_Sync);
	if (IsListEmpty(&m_EventList))
	{
		LeaveCriticalSection(&m_Sync);
		return 0;
	}

	PDRV_EVENT_LISTITEM pItem = (PDRV_EVENT_LISTITEM) (m_EventList.Flink);
	pRet = pItem->m_Event;
	
	RemoveEntryList(&pItem->m_List);
	
	LeaveCriticalSection(&m_Sync);
	Free((BYTE*) pItem);

	return pRet;
}