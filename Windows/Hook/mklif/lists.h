#ifndef __LISTS_H
#define __LISTS_H

/*!
*		
*		
*		
*		\file	lists.h
*		\brief	копия кода для работы с двунаправленными списками
*		
*		\author system
*		\date	12.09.2003 11:32:00
*		
*		
*/		




typedef struct _IMPLIST_ENTRY {
   struct _IMPLIST_ENTRY *Flink;
   struct _IMPLIST_ENTRY *Blink;
} IMPLIST_ENTRY, *PIMPLIST_ENTRY;


VOID
_inline
_impInitializeListHead(
    IN PIMPLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

#define _impIsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))


BOOLEAN
_inline
_impRemoveEntryList(
    IN PIMPLIST_ENTRY Entry
    )
{
    PIMPLIST_ENTRY Blink;
    PIMPLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN)(Flink == Blink);
}

PIMPLIST_ENTRY
_inline
_impRemoveHeadList(
    IN PIMPLIST_ENTRY ListHead
    )
{
    PIMPLIST_ENTRY Flink;
    PIMPLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

PIMPLIST_ENTRY
_inline
_impRemoveTailList(
    IN PIMPLIST_ENTRY ListHead
    )
{
    PIMPLIST_ENTRY Blink;
    PIMPLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}


VOID
_inline
_impInsertTailList(
    IN PIMPLIST_ENTRY ListHead,
    IN PIMPLIST_ENTRY Entry
    )
{
    PIMPLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}


VOID
_inline
_impInsertHeadList(
    IN PIMPLIST_ENTRY ListHead,
    IN PIMPLIST_ENTRY Entry
    )
{
    PIMPLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

#define _impPopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_IMPLIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }

#define _impPushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)



#define INSERT_TAIL_LIST(_lock, _head, _list) {\
	_RESOURCE_LOCK_W(_lock);\
	_impInsertTailList(_head, _list);\
	_RESOURCE_UNLOCK(_lock);}

#define REMOVE_ENTRY_LIST(_lock, _list) {\
	_RESOURCE_LOCK_W(_lock);\
	_impRemoveEntryList(_list);\
	_RESOURCE_UNLOCK(_lock);}

#endif