#ifndef __THLIST_H
#define __THLIST_H

typedef struct _IMPLIST_ENTRY {
   struct _IMPLIST_ENTRY *Flink;
   struct _IMPLIST_ENTRY *Blink;
} IMPLIST_ENTRY, *PIMPLIST_ENTRY;



void
_inline
_impInitializeListHead(
    PIMPLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))


int
_inline
_impRemoveEntryList(
    PIMPLIST_ENTRY Entry
    )
{
    PIMPLIST_ENTRY Blink;
    PIMPLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (int)(Flink == Blink);
}

PIMPLIST_ENTRY
_inline
_impRemoveHeadList(
    PIMPLIST_ENTRY ListHead
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
    PIMPLIST_ENTRY ListHead
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


void
_inline
_impInsertTailList(
    PIMPLIST_ENTRY ListHead,
    PIMPLIST_ENTRY Entry
    )
{
    PIMPLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}


void
_inline
_impInsertHeadList(
    PIMPLIST_ENTRY ListHead,
    PIMPLIST_ENTRY Entry
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
	_TH_LOCK_W(_lock);\
	_impInsertTailList(_head, _list);\
	_TH_UNLOCK(_lock);}

#define REMOVE_ENTRY_LIST(_lock, _list) {\
	_TH_LOCK_W(_lock);\
	_impRemoveEntryList(_list);\
	_TH_UNLOCK(_lock);}

#endif //__THLIST_H
