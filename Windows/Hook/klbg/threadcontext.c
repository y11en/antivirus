#include <fltkernel.h>
#include "main.h"
#include "threadcontext.h"

#define HASH_TABLE_SIZE			0x1000
#define HASH_FN( _tid_ )		((ULONG_PTR) _tid_ & 0xfff )

typedef struct _HASH_TABLE_ENTRY
{
	LIST_ENTRY		m_CollisionListHead;
} HASH_TABLE_ENTRY, *PHASH_TABLE_ENTRY;

typedef struct _THREAD_CONTEXT_HEADER
{
	LIST_ENTRY	m_List;
	ULONG		m_RefCount;
	HANDLE		m_ThreadId;
} THREAD_CONTEXT_HEADER, *PTHREAD_CONTEXT_HEADER;
//------------------------------------------------------------------------------------------

static ERESOURCE g_erThreadContextsLock;
static PHASH_TABLE_ENTRY g_pHashTable = NULL;

PTHREAD_CONTEXT_HEADER GetHashEntry( __in HANDLE Tid )
{
	PLIST_ENTRY pCollisionListHead;
	PTHREAD_CONTEXT_HEADER pCollisionListEntry;

	pCollisionListHead = &g_pHashTable[HASH_FN( Tid )].m_CollisionListHead;

	pCollisionListEntry = (PTHREAD_CONTEXT_HEADER) pCollisionListHead->Flink;
	while ( pCollisionListHead != (PLIST_ENTRY) pCollisionListEntry )
	{
		if ( Tid == pCollisionListEntry->m_ThreadId )
			return pCollisionListEntry;

		pCollisionListEntry = (PTHREAD_CONTEXT_HEADER) pCollisionListEntry->m_List.Flink;
	}

	return NULL;
}

// retval:	TRUE if no thread with pContextHeader->m_ThreadId existed in the cache
//			else FALSE
BOOLEAN AddHashEntry( __in PTHREAD_CONTEXT_HEADER pContextHeader )
{
	PLIST_ENTRY pCollisionListHead;
	PTHREAD_CONTEXT_HEADER pCollisionListEntry;

	pCollisionListHead = &g_pHashTable[HASH_FN( pContextHeader->m_ThreadId )].m_CollisionListHead;

	pCollisionListEntry = (PTHREAD_CONTEXT_HEADER) pCollisionListHead->Flink;
	while ( pCollisionListHead != (PLIST_ENTRY) pCollisionListEntry )
	{
		if ( pContextHeader->m_ThreadId == pCollisionListEntry->m_ThreadId )
			return FALSE;

		pCollisionListEntry = (PTHREAD_CONTEXT_HEADER) pCollisionListEntry->m_List.Flink;
	}

	InsertTailList( pCollisionListHead, &pContextHeader->m_List );

	return TRUE;
}

PTHREAD_CONTEXT AllocateThreadContext( 
									  __in POOL_TYPE PoolType,
									  __in ULONG cbContext
									  )
{
	PTHREAD_CONTEXT_HEADER pContextHeader;

	pContextHeader = (PTHREAD_CONTEXT_HEADER) ExAllocatePoolWithTag( 
		PoolType,
		sizeof( THREAD_CONTEXT_HEADER ) + cbContext,
		'CTbz'
		);
	if ( !pContextHeader )
		return NULL;

	InitializeListHead( &pContextHeader->m_List );
	pContextHeader->m_RefCount = 1;
	pContextHeader->m_ThreadId = (HANDLE) -1LL;

	return (PTHREAD_CONTEXT) ((PCHAR) pContextHeader + sizeof( THREAD_CONTEXT_HEADER ));
}

VOID ReleaseThreadContext( __in PTHREAD_CONTEXT pContext )
{
	PTHREAD_CONTEXT_HEADER pHeader;

	pHeader = (PTHREAD_CONTEXT_HEADER) ((PCHAR) pContext - sizeof(THREAD_CONTEXT_HEADER));

	AcquireResourceExclusive( &g_erThreadContextsLock );

	pHeader->m_RefCount--;
	if ( 0 == pHeader->m_RefCount )
	{
		if ( &pHeader->m_List != pHeader->m_List.Flink )	// remove from hash collision list
			RemoveEntryList( &pHeader->m_List );			//
		ExFreePool( pHeader );								// free allocated memory
	}

	ReleaseResource( &g_erThreadContextsLock );
}

// retval:	TRUE if no thread with ThreadId Id existed in the cache
//			else FALSE
BOOLEAN SetThreadContext( 
						 __in PTHREAD_CONTEXT pContext,
						 __in HANDLE ThreadId 
						 )
{
	PTHREAD_CONTEXT_HEADER pHeader;
	BOOLEAN bResult;

	pHeader = (PTHREAD_CONTEXT_HEADER) ((PCHAR) pContext - sizeof(THREAD_CONTEXT_HEADER));
	pHeader->m_ThreadId = ThreadId;

	AcquireResourceExclusive( &g_erThreadContextsLock );
	bResult = AddHashEntry( pHeader );
	ReleaseResource( &g_erThreadContextsLock ) ;

	return bResult;
}

PTHREAD_CONTEXT GetThreadContext( __in HANDLE ThreadId )
{
	PTHREAD_CONTEXT_HEADER pThreadContextHeader;

	AcquireResourceExclusive( &g_erThreadContextsLock );

	pThreadContextHeader = GetHashEntry( ThreadId );
	if ( !pThreadContextHeader )
	{
		ReleaseResource( &g_erThreadContextsLock );
		return NULL;
	}

	pThreadContextHeader->m_RefCount++;

	ReleaseResource( &g_erThreadContextsLock );

	return (PTHREAD_CONTEXT) ((PCHAR) pThreadContextHeader + sizeof(THREAD_CONTEXT_HEADER));
}

BOOLEAN InitThreadContexts()
{
	NTSTATUS status;
	ULONG i;

	g_pHashTable = (PHASH_TABLE_ENTRY) ExAllocatePoolWithTag(
		PagedPool,
		HASH_TABLE_SIZE * sizeof(HASH_TABLE_ENTRY),
		'THbz'
		);
	if ( !g_pHashTable )
		return FALSE;

	status = ExInitializeResourceLite( &g_erThreadContextsLock );
	if ( !NT_SUCCESS(status) )
	{
		ExFreePool( g_pHashTable );
		return FALSE;
	}

	for ( i = 0; i < HASH_TABLE_SIZE; i++)
		InitializeListHead( &g_pHashTable[i].m_CollisionListHead );

	return TRUE;
}

VOID UnInitThreadContexts()
{
	ULONG i;

// free all items allocated
	for ( i = 0; i < HASH_TABLE_SIZE; i++)
	{
		PLIST_ENTRY pHead;
		PLIST_ENTRY pEntry;

		pHead = &g_pHashTable[i].m_CollisionListHead;
		pEntry = pHead->Flink;
		while ( pHead != pEntry )
		{
			PLIST_ENTRY pNextEntry;

			pNextEntry = pEntry->Flink;
			ExFreePool( pEntry );
			pEntry = pNextEntry;
		}
	}

	ExFreePool( g_pHashTable );

	ExDeleteResourceLite( &g_erThreadContextsLock );
}