#include <fltKernel.h>
#include "gencache.h"

#define GEN_CACHE_SIGNATURE				0x21081979
// hash func
#define GEN_HASH_FUNC( __x__, __s__ )	(ULONG) ( (ULONGLONG) ( __x__ ) & ( __s__ - 1 ) )

typedef struct _GEN_CACHE_ITEM
{
	LIST_ENTRY		m_List;
	ULONG			m_Signature;
	ULONGLONG		m_Key;
	UCHAR			m_Data[1];
} GEN_CACHE_ITEM, *PGEN_CACHE_ITEM;

BOOLEAN GenCacheInit(
	__in ULONG HashSize,
	__in ULONG DataSize,
	__in_opt t_fGenCacheItemAlloc cbAlloc,
	__in_opt t_fGenCacheItemFree cbFree,
	__out PGEN_CACHE *p_pGenCache
	)
{
	PGEN_CACHE pCache;
	ULONG i;

	pCache = (PGEN_CACHE) ExAllocatePoolWithTag(
		NonPagedPool,
		FIELD_OFFSET( GEN_CACHE, m_HashTable ) + HashSize * sizeof( LIST_ENTRY ),
		'CGrg'
		);
	if ( !pCache )
		return FALSE;

	pCache->m_HashSize = HashSize;
	pCache->m_DataSize = DataSize;
	pCache->m_cbAlloc = cbAlloc;
	pCache->m_cbFree = cbFree;
	ExInitializeFastMutex( &pCache->m_CacheLock );

	for ( i = 0; i < HashSize; i++ )
		InitializeListHead( &pCache->m_HashTable[i] );

	*p_pGenCache = pCache;

	return TRUE;
}

VOID GenCacheDone(
	__inout PGEN_CACHE *p_pCache
	)
{
	PGEN_CACHE pCache;
	ULONG Hash;

	ASSERT( p_pCache );
	pCache = *p_pCache;
	ASSERT( pCache );

	ExAcquireFastMutex( &pCache->m_CacheLock );

	for ( Hash = 0; Hash < pCache->m_HashSize; Hash++ )
	{
		PLIST_ENTRY pListHead;
		PGEN_CACHE_ITEM pCacheItem;

		pListHead = &pCache->m_HashTable[Hash];

		pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
		while ( pCacheItem != (PGEN_CACHE_ITEM) pListHead )
		{
			PGEN_CACHE_ITEM pTmpCacheItem;

			ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

			pTmpCacheItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink;

			// call pre-free callback
			if ( pCache->m_cbFree )
			{
				pCache->m_cbFree(
					pCacheItem->m_Key,
					&pCacheItem->m_Data
					);
			}

			ExFreePool( pCacheItem );
			pCacheItem = pTmpCacheItem;
		}
	}

	ExReleaseFastMutex( &pCache->m_CacheLock );

	ExFreePool( *p_pCache );

	*p_pCache = NULL;
}

PVOID GenCacheGet(
	__in PGEN_CACHE pCache,
	__in ULONGLONG Key,
	__in BOOLEAN bCreateNew,
	__out PBOOLEAN p_bNewReturned
  )
{
	ULONG Hash;
	PLIST_ENTRY pListHead;
	PGEN_CACHE_ITEM pCacheItem;
	PGEN_CACHE_ITEM pRetItem = NULL;
	BOOLEAN bNewReturned = FALSE;

	ASSERT( pCache );
	ASSERT( p_bNewReturned );

	Hash = GEN_HASH_FUNC( Key, pCache->m_HashSize );

	ExAcquireFastMutex( &pCache->m_CacheLock );

	pListHead = &pCache->m_HashTable[Hash];

	for ( pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
		pCacheItem != (PGEN_CACHE_ITEM) pListHead;
		pCacheItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink 
		)
	{
		ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

		if ( Key == pCacheItem->m_Key )
		{
			pRetItem = pCacheItem;
			break;
		}	
	}

	if ( !pRetItem && bCreateNew )
	{
		pRetItem = (PGEN_CACHE_ITEM) ExAllocatePoolWithTag(
			NonPagedPool,
			FIELD_OFFSET( GEN_CACHE_ITEM, m_Data ) + pCache->m_DataSize,
			'ICrg'
			);
		if ( pRetItem )
		{
			pRetItem->m_Signature = GEN_CACHE_SIGNATURE;
			pRetItem->m_Key = Key;

			InsertTailList( pListHead, &pRetItem->m_List );

			if ( pCache->m_cbAlloc )
			{
				pCache->m_cbAlloc(
					pRetItem->m_Key,
					&pRetItem->m_Data
					);
			}

			bNewReturned = TRUE;
		}
	}

	ExReleaseFastMutex( &pCache->m_CacheLock );

	*p_bNewReturned = bNewReturned;

	return pRetItem ? &pRetItem->m_Data : NULL;
}

VOID GenCacheFree(
	__in PGEN_CACHE pCache,
	__in ULONGLONG Key
	)
{
	ULONG Hash;
	PLIST_ENTRY pListHead;
	PGEN_CACHE_ITEM pCacheItem;
	PGEN_CACHE_ITEM pFoundItem = NULL;

	ASSERT( pCache );

	Hash = GEN_HASH_FUNC( Key, pCache->m_HashSize );

	ExAcquireFastMutex( &pCache->m_CacheLock );

	pListHead = &pCache->m_HashTable[Hash];

	for (	pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
			pCacheItem != (PGEN_CACHE_ITEM) pListHead;
			pCacheItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink 
		)
	{
		ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

		if ( Key == pCacheItem->m_Key )
		{
			pFoundItem = pCacheItem;
			break;
		}	
	}

	if ( pFoundItem )	
	{
		// call pre-free callback
		if ( pCache->m_cbFree )
		{
			pCache->m_cbFree(
				pCacheItem->m_Key,
				&pCacheItem->m_Data
				);
		}

		RemoveEntryList( &pFoundItem->m_List );		
		ExFreePool( pFoundItem );
	}

	ExReleaseFastMutex( &pCache->m_CacheLock );
}

VOID GenCacheCleanupByKeyMask(
	__in PGEN_CACHE pCache,
	__in ULONGLONG Key,
	__in ULONGLONG KeyMask
	)
{
	ULONG Hash;
	PLIST_ENTRY pListHead;
	PGEN_CACHE_ITEM pCacheItem;

	ASSERT( pCache );

	Hash = GEN_HASH_FUNC( Key, pCache->m_HashSize );

	ExAcquireFastMutex( &pCache->m_CacheLock );

	pListHead = &pCache->m_HashTable[Hash];

	pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
	while ( pCacheItem != (PGEN_CACHE_ITEM) pListHead )
	{
		PGEN_CACHE_ITEM pNextItem;

		ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

		pNextItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink;

		if ( ( KeyMask & Key ) == ( KeyMask & pCacheItem->m_Key ) )
		{
			// call pre-free callback
			if ( pCache->m_cbFree )
			{
				pCache->m_cbFree(
					pCacheItem->m_Key,
					&pCacheItem->m_Data
					);
			}

			RemoveEntryList( &pCacheItem->m_List );		
			ExFreePool( pCacheItem );
		}

		pCacheItem = pNextItem;
	}

	ExReleaseFastMutex( &pCache->m_CacheLock );
}
