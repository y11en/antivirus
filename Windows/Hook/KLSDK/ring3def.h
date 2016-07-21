#ifndef _RING3_DEF_H__
#define _RING3_DEF_H__

#include "windows.h"
#include "stdio.h"
#include "memory.h"

typedef LONG NTSTATUS, *PNTSTATUS, NDIS_STATUS, *PNDIS_STATUS;
typedef HANDLE NDIS_PACKET_POOL, *PNDIS_PACKET_POOL, NDIS_HANDLE, *PNDIS_HANDLE;

typedef struct _NDIS_BUFFER {
    struct _NDIS_BUFFER *Next;          //pointer to next buffer descriptor in chain
    PVOID   VirtualAddress;               //linear address of this buffer
    HANDLE  Pool;             //pointer to pool so we can free to correct pool
    UINT    Length;                        //length of this buffer
    UINT    Signature;                     //character signature for debug "NBUF"
} NDIS_BUFFER, * PNDIS_BUFFER;

typedef struct _NDIS_PACKET_PRIVATE
{
	UINT				PhysicalCount;	// number of physical pages in packet.
	UINT				TotalLength;	// Total amount of data in the packet.
	PNDIS_BUFFER		Head;			// first buffer in the chain
	PNDIS_BUFFER		Tail;			// last buffer in the chain

	// if Head is NULL the chain is empty; Tail doesn't have to be NULL also

	PNDIS_PACKET_POOL	Pool;			// so we know where to free it back to
	UINT				Count;
	ULONG				Flags;			
	BOOLEAN				ValidCounts;
	UCHAR				NdisPacketFlags;	// See fPACKET_xxx bits below
	USHORT				NdisPacketOobOffset;
} NDIS_PACKET_PRIVATE, * PNDIS_PACKET_PRIVATE;

typedef struct _NDIS_PACKET
{
	NDIS_PACKET_PRIVATE	Private;

	union
	{
		struct					// For Connection-less miniports
		{
			UCHAR	MiniportReserved[8];
			UCHAR	WrapperReserved[8];
		};

		struct
		{
			//
			// For de-serialized miniports. And by implication conn-oriented miniports.
			// This is for the send-path only. Packets indicated will use WrapperReserved
			// instead of WrapperReservedEx
			//
			UCHAR	MiniportReservedEx[12];
			UCHAR	WrapperReservedEx[4];
		};

		struct
		{
			UCHAR	MacReserved[16];
		};
	};

	ULONG			Reserved[2];			// For compatibility with Win95
	UCHAR			ProtocolReserved[1];

} NDIS_PACKET, *PNDIS_PACKET, **PPNDIS_PACKET;

#ifndef DbgPrint
	#define DbgPrint printf
#endif

#ifdef _DEBUG
    #ifdef _M_IX86
        #define DbgBreakPoint() /*{ __asm int 3; }*/
    #else 
        #define DbgBreakPoint()        
    #endif
#else
    #define DbgBreakPoint()
#endif

#define KL_MEM_ALLOC(_size_) malloc((_size_))
#define KL_MEM_FREE(_mem_) free((_mem_))

#define ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag) malloc( NumberOfBytes )
#define ExFreePool( P ) free ( P )

#ifdef _M_IX86_
#define ASSERT(c)   if (c == 0) { __asm int 3 }
#else
#define ASSERT(c)
#endif

#define KL_TIME						__int64
#define PKL_TIME					*__int64
#define KlGetSystemTime(_Time_)		GetSystemTimeAsFileTime((PFILETIME)_Time_);

#define KlSleep(_Time_)     Sleep(_Time_ * 1000)

#define ONE_SECOND_TIME				(KL_TIME)0x989680
#define HALF_SECOND_TIME			(KL_TIME)0x4C4B40

#define SPIN_LOCK					CRITICAL_SECTION
#define IRQL                        UCHAR
#define AllocateSpinLock(_Lock_)	InitializeCriticalSection ((_Lock_));

#define AcquireSpinLock(_Lock_, _OldIrql)		EnterCriticalSection( (_Lock_) )
#define ReleaseSpinLock(_Lock_, _OldIrql)		LeaveCriticalSection( (_Lock_) )

#define KL_EVENT					HANDLE
#define PKL_EVENT					*HANDLE
#define KlWaitEvent(_Event_)		WaitForSingleObject((KL_EVENT)*((KL_EVENT*)_Event_), INFINITE)

#define KlInitializeEvent(_Event_, _Type_, _State_)		*(_Event_) = CreateEvent(NULL, (_Type_), (_State_), NULL )
#define KlSetEvent(_Event_)								SetEvent(*(_Event_))
#define KlClearEvent(_Event_)							ResetEvent(*(_Event_))
#define KlCloseEvent(_Event_)                           CloseHandle(*( _Event_ ))

#define KlCompareMemory(_Dst_, _Src_, _Length_)         !memcmp( (_Dst_), (_Src_), (_Length_) )

#define RtlCopyMemory(_Destination, _Source,_Length)	memcpy((_Destination),(_Source),(_Length))
#define RtlFillMemory(_Destination, _Length,_Fill)		memset((_Destination),(_Fill),(_Length))

typedef enum _POOL_TYPE {
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS,
	MaxPoolType
} POOL_TYPE;

typedef enum _EVENT_TYPE {    
    SynchronizationEvent,    // Not Manual
    NotificationEvent      // manual = TRUE that's why = 1
} EVENT_TYPE;

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead) )

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) (\
    ( ((ListHead)->Flink == (ListHead)) ? TRUE : FALSE ) )

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {\
    PLIST_ENTRY FirstEntry;\
    FirstEntry = (ListHead)->Flink;\
    FirstEntry->Flink->Blink = (ListHead);\
    (ListHead)->Flink = FirstEntry->Flink;\
    }

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Entry;\
    _EX_Entry = (Entry);\
    _EX_Entry->Blink->Flink = _EX_Entry->Flink;\
    _EX_Entry->Flink->Blink = _EX_Entry->Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

#define InsertTailList(ListHead,Entry) \
    (Entry)->Flink = (ListHead);\
    (Entry)->Blink = (ListHead)->Blink;\
    (ListHead)->Blink->Flink = (Entry);\
    (ListHead)->Blink = (Entry)

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) \
    (Entry)->Flink = (ListHead)->Flink;\
    (Entry)->Blink = (ListHead);\
    (ListHead)->Flink->Blink = (Entry);\
    (ListHead)->Flink = (Entry)



//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
    PSINGLE_LIST_ENTRY FirstEntry;\
    FirstEntry = (ListHead)->Next;\
    (ListHead)->Next = FirstEntry;\
    }


//
//  VOID
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)

//Threads Creating
typedef
VOID (__cdecl *THREADINITPROC)( VOID );

typedef
VOID (__cdecl *PKSTART_ROUTINE)( PVOID StartContext );

typedef struct _START_PARAMS {
		PKSTART_ROUTINE StartRoutine;   // Function to call
		PVOID StartContext;				// Paramerer for this function
		PVOID  ThreadHandle;				// Handle of the thread		
} START_PARAMS,*PSTART_PARAMS;

#endif