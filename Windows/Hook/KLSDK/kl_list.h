#ifndef _KL_LIST_H_
#define _KL_LIST_H_

#include "kl_lock.h"

#define QUEUE_ENTRY   LIST_ENTRY  m_ListEntry;

template <class T> class CKl_List : public CKl_Object
{
public:
    
    LIST_ENTRY  m_head;
    ULONG       m_ListEntryOffset; // смещение в T, где находится LIST_ENTRY
    CKl_Lock    m_Lock;

    CKl_List();
    CKl_List(ULONG  nListEntryOffset);
    virtual ~CKl_List();

    T*      Head();                     // Получает указатель на первый элемент списка, если он есть. иначе 0
    T*      Tail();                     // Получает указатель на последний элемент списка, если он есть. иначе 0
    T*      Next            (T* Item);  // Получает указатель на последующий элемент
    T*      Prev            (T* Item);  // Получает указатель на предыдущий элемент
    T*      InterlockedNext (T* Item);  // Защищено спинлоком 
    T*      InterlockedPrev (T* Item);  // Защищено спинлоком

    void    InsertTail      (T* Item);        // 
    void    InsertHead      (T* Item);    
    void    InsertBefore    (T* Item, T* BeforeThisItem);
    void    InsertAfter     (T* Item, T* AfterThisItem);
    void    InsertByOrder   (T* Item);
    
    T*      Remove          (T* Item);
    T*      InterlockedRemove(T* Item);
    T*      RemoveHead();
    T*      RemoveTail();
    T*      InterlockedRemoveHead();
    T*      InterlockedRemoveTail();
    
    // Поиск в списке элемент, в смещении Offset которого находится значение Value
    T*      Find_DWORD            (ULONG  Offset, DWORD   Value);
    T*      InterlockedFind_DWORD (ULONG  Offset, DWORD   Value);

    T*      Find_PTR              (ULONG  Offset, PVOID   Value);
    T*      InterlockedFind_PTR   (ULONG  Offset, PVOID   Value);

    void    InsertHeadNoLock(T* Item);  // Помещает элемент в голову
    void    InsertTailNoLock(T* Item);  // Помещает элемент в хвост
    void    InsertBeforeNoLock(T* Item, T* BeforeThisItem); // Помещает элемент перед данным элементом
    void    InsertAfterNoLock (T* Item, T* AfterThisItem);  // Помещает элемент после данного элемента
    
    // помещает элемент X в список I():  I(n) > X > I(n+1)
    // т.е. если пользоваться только этой функцией получим список 
    // элементов, отсортированных по убыванию
    void    InsertByOrderNoLock(T* Item); 

    T*              Container(PLIST_ENTRY   pLE);
  	PLIST_ENTRY     ListEntryPointer(T* p);

    void    Lock();
    void    Unlock();
};

template <class T> inline ULONG ListEntryOffset(T* p)
{
	return (ULONG) &((T*)0)->m_ListEntry;	
}

template <class T> CKl_List<T>::CKl_List() : m_Lock()
{    
    InitializeListHead(&m_head);
   	m_ListEntryOffset = FIELD_OFFSET ( T, m_ListEntry );
}

template <class T> CKl_List<T>::CKl_List(ULONG  nListEntryOffset) : m_Lock()
{
    InitializeListHead(&m_head);
   	m_ListEntryOffset = nListEntryOffset;    
}

template <class T> CKl_List<T>::~CKl_List()
{
    T*  Item = NULL;
}


template <class T> inline T* CKl_List<T>::Container(PLIST_ENTRY pLE)
{
    return ( pLE == &m_head ) ? NULL : (T*)( (PCHAR)pLE - m_ListEntryOffset );
}

template <class T> inline PLIST_ENTRY CKl_List<T>::ListEntryPointer(T* p)
{
    return (PLIST_ENTRY)((PCHAR)p + m_ListEntryOffset);
}

template <class T> inline void CKl_List<T>::Lock()
{
    m_Lock.Acquire();
}

template <class T> inline void CKl_List<T>::Unlock()
{
    m_Lock.Release();
}

template <class T> inline T* CKl_List<T>::Remove(T* Item)
{    
    if ( Item )
    {
        RemoveEntryList( ListEntryPointer(Item) );
    }

    return Item;
}

template <class T> inline T* CKl_List<T>::InterlockedRemove(T* Item)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    if ( Item )
    {
        RemoveEntryList( ListEntryPointer(Item) );
    }
    
    return Item;
}

template <class T> inline T* CKl_List<T>::Head()
{
    return Container( m_head.Flink );
}

template <class T> inline T* CKl_List<T>::Tail()
{
    return Container( m_head.Blink );
}

template <class T> inline T* CKl_List<T>::Next(T* Item)
{
    return (Item == NULL) ? Container( m_head.Flink ) : Container( ListEntryPointer(Item)->Flink );
}

template <class T> inline T* CKl_List<T>::Prev(T* Item)
{
    return (Item == NULL) ? Container( m_head.Blink ) : Container( ListEntryPointer(Item)->Blink );
}

template <class T> inline T* CKl_List<T>::InterlockedNext(T* Item)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    return Next(Item);
}

template <class T> inline T* CKl_List<T>::InterlockedPrev(T* Item)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    return Prev(Item);
}

template <class T> inline void CKl_List<T>::InsertTailNoLock(T* Item)
{
    InsertTailList( &m_head, ListEntryPointer(Item) );
}

template <class T> inline void CKl_List<T>::InsertHeadNoLock(T* Item)
{
    InsertHeadList( &m_head, ListEntryPointer(Item) );
}

template <class T> inline void CKl_List<T>::InsertTail(T* Item)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    InsertTailNoLock( Item );
}

template <class T> inline void CKl_List<T>::InsertHead(T* Item)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    InsertHeadNoLock( Item );
}

template <class T> inline void CKl_List<T>::InsertBefore(T* Item, T* BeforeThisItem)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    if ( BeforeThisItem == NULL )
		InsertHead(Item);
	else
	{
		LIST_ENTRY le = *ListEntryPointer(BeforeThisItem);		

		ListEntryPointer(BeforeThisItem)->Blink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Blink = le.Blink;

		le.Blink->Flink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Flink = ListEntryPointer(BeforeThisItem);
	}    
}

template <class T> inline void CKl_List<T>::InsertAfter(T* Item, T* AfterThisItem)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    if ( AfterThisItem == NULL )
		InsertTail(Item);
	else
	{
		LIST_ENTRY le = *ListEntryPointer(AfterThisItem);

		ListEntryPointer(AfterThisItem)->Flink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Flink = le.Flink;

		le.Flink->Blink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Blink = ListEntryPointer(AfterThisItem);
	}    
}

template <class T> inline void CKl_List<T>::InsertByOrder(T* Item)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    InsertByOrderNoLock( Item );
}

template <class T> inline void CKl_List<T>::InsertByOrderNoLock(T* Item)
{
    T*  CurrentItem = NULL;    
    
    while ( CurrentItem = Next(CurrentItem) )
    {
        if ( *CurrentItem < *Item )
            break;
    }

    if ( CurrentItem == NULL )
    {
        InsertTailNoLock( Item );
        return;
    }
    
    InsertBeforeNoLock( Item, CurrentItem );
}

template <class T> inline void CKl_List<T>::InsertBeforeNoLock(T* Item, T* BeforeThisItem)
{    
    if ( BeforeThisItem == NULL )
    {
        InsertHeadNoLock( Item );
    }		
	else
	{
		LIST_ENTRY le = *ListEntryPointer(BeforeThisItem);		

		ListEntryPointer(BeforeThisItem)->Blink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Blink = le.Blink;

		le.Blink->Flink = ListEntryPointer(Item);
		ListEntryPointer(Item)->Flink = ListEntryPointer(BeforeThisItem);
	}
}

template <class T> inline void CKl_List<T>::InsertAfterNoLock(T* Item, T* AfterThisItem)
{
    if ( AfterThisItem == NULL )
    {        
        InsertTailNoLock( Item );
    }		
	else
	{
		LIST_ENTRY le           = *ListEntryPointer(AfterThisItem);
        PLIST_ENTRY pItemLE     = ListEntryPointer(Item);
        PLIST_ENTRY pAfterItem  = ListEntryPointer(AfterThisItem);

        pAfterItem->Flink   = pItemLE;
        pItemLE->Flink      = le.Flink;

        le.Flink->Blink     = pItemLE;
        pItemLE->Blink      = pAfterItem;
	}
}

template <class T> inline T* CKl_List<T>::RemoveHead()
{
    PLIST_ENTRY ListEntry   = RemoveHeadList(&m_head);
    T*          RetVal      = Container(ListEntry);

    return RetVal;
}

template <class T> inline T* CKl_List<T>::RemoveTail()
{
    PLIST_ENTRY ListEntry   = RemoveTailList(&m_head);
    T*          RetVal      = Container(ListEntry);

    return RetVal;
}

template <class T> inline T* CKl_List<T>::InterlockedRemoveHead()
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    return RemoveHead();
}

template <class T> inline T* CKl_List<T>::InterlockedRemoveTail()
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    return RemoveTail();
}

template <class T> inline T* CKl_List<T>::InterlockedFind_DWORD(ULONG  Offset, DWORD   Value)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    return Find_DWORD ( Offset, Value );
}

template <class T> inline T* CKl_List<T>::InterlockedFind_PTR(ULONG  Offset, PVOID   Value)
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    return Find_PTR ( Offset, Value );
}
/*
template <class T> inline ULONG CKl_List<T>::GetItemCount()
{
    CKl_AutoLock Lock( &m_Lock.Lock );

    T*      Item  = NULL;
    ULONG   count = 0;

    while ( Item = Next(Item) )
    {
        count++;
    }

    m_ItemCount = count;

    return count;
}
*/

template <class T> inline T* CKl_List<T>::Find_DWORD(ULONG  Offset, DWORD   Value)
{
    T*          RetVal  = NULL;
	PLIST_ENTRY	p       = NULL;

    for ( p = m_head.Flink; p != &m_head; p = p->Flink )
    {
        T* Temp = Container(p);
        
        if ( *(DWORD*) ( (PCHAR)Temp + Offset ) == Value )
            return Temp;
    }

    return NULL;
}

template <class T> inline T* CKl_List<T>::Find_PTR(ULONG  Offset, PVOID   Value)
{
    T*          RetVal  = NULL;
	PLIST_ENTRY	p       = NULL;

    for ( p = m_head.Flink; p != &m_head; p = p->Flink )
    {
        T* Temp = Container(p);
        
        if ( *(PVOID*) ( (PCHAR)Temp + Offset ) == Value )
            return Temp;
    }

    return NULL;
}
/*
template <class T> inline T* CKl_List<T>::Find(ULONG  Offset, PVOID   Value, T* StartItem, BOOLEAN bForward)
{
   	T*          RetVal  = NULL;
	PLIST_ENTRY	p       = NULL;
	PLIST_ENTRY	pStart;
	BOOLEAN     bFound  = FALSE;

	// Если лист не пуст, то будем искать
	if ( !IsListEmpty(&m_head) )
	{
		
		// Направление поиска
		if (bForward)
		{
		
			// Если StartItem==NULL, начнем поиск с головы состава			
            pStart = p = ( StartItem == NULL ) ? m_head.Flink : ListEntryPointer(StartItem);

			do
			{
                T* Temp = Container(p);
				
				// If the container pointer of the node being searched matches
				// the item's pointer we found it, stop searching
				if (*(ULONG*)((ULONG)Temp + Offset) == (ULONG)Value)
				{
					bFound = TRUE;
					break;
				}
				
				// Advance to the next node in the list
				p = p->Flink;
				
				// Jump over the list head since it's not really a
				// node of the list
				if (p == &m_head)
					p = p->Flink;

			// Keep searching until we reach the node we started searching from
			} while  (p != pStart);
		}
		else
		{
			
			// For a backward search start searching from the tail if no
			// start node was specified.
            pStart = p = ( StartItem == NULL ) ? m_head.Blink : ListEntryPointer(StartItem);

			do 
			{
				if ((ULONG)Container(p) + Offset == (ULONG)Value)
				{
					bFound = TRUE;
					break;
				}
				
				// Go to the previous node for a backward search
				p = p->Blink;

				if (p == &m_head)
					p = p->Blink;
			} while  (p != pStart);
		}
	}

	// If the entry was found, pass back the pointer to its container
	if ( bFound )
		RetVal = Container(p);

	return RetVal;
}
*/

#endif // _KL_LIST_H_