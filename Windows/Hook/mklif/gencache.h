#ifndef __GENCACHE_H
#define __GENCACHE_H
//--------------------------------------------------------------------------
#define		HANDLE_CACHE_KEY_FUNC( __pid__, __handle__ )	( ( ( (ULONGLONG) ( (ULONG) ( __handle__ ) & 0xffffffff ) ) << 32 ) | ( (ULONG) ( __pid__ ) & 0xffffffff ) )
#define		HANDLE_CACHE_PID_MASK							0xffffffff

// callback on-alloc
typedef VOID ( *t_fGenCacheItemAlloc ) (
										__in ULONGLONG Key,
										__in PVOID pData
										);
// callback on-free
typedef VOID ( *t_fGenCacheItemFree ) (
									   __in ULONGLONG Key,
									   __in PVOID pData
									   );

typedef struct _GEN_CACHE
{
	ULONG					m_HashSize;
	ULONG					m_DataSize;

	t_fGenCacheItemAlloc	m_cbAlloc;
	t_fGenCacheItemFree		m_cbFree;

	FAST_MUTEX		m_CacheLock;		// global cache lock
	LIST_ENTRY		m_HashTable[1];
} GEN_CACHE, *PGEN_CACHE;

BOOLEAN GenCacheInit(
	__in ULONG HashSize,
	__in ULONG DataSize,
	__in_opt t_fGenCacheItemAlloc cbAlloc,
	__in_opt t_fGenCacheItemFree cbFree,
	__out PGEN_CACHE *p_pGenCache
	);
VOID GenCacheDone(
	__inout PGEN_CACHE *p_pCache
	);
PVOID GenCacheGet(
	__in PGEN_CACHE pCache,
	__in ULONGLONG Key,
	__in BOOLEAN bCreateNew,
	__out PBOOLEAN p_bNewReturned
	);
VOID GenCacheFree(
	__in PGEN_CACHE pCache,
	__in ULONGLONG Key
	);
VOID GenCacheCleanupByKeyMask(
	__in PGEN_CACHE pCache,
	__in ULONGLONG Key,
	__in ULONGLONG KeyMask
	);
//--------------------------------------------------------------------------
#endif