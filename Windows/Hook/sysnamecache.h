#ifndef __SYSNAMECACHE_H
#define __SYSNAMECACHE_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	sysnamecache.h
*		\brief	кеш объектов
*		
*		\author Andrew Sobko (наследование кода из namecache)
*		\date	12.09.2003 13:02:42
*		
*		примечание: описание функций см. namecache.h
*		особенности реализации: всегда проверяется owner объекта, нет счетчиков refcount
*		
*		
*/		

#include <ntifs.h>

#define SYSHASH_SIZE  128        // MUST be a power of 2

#define SYSHASH_FUNC(_object)  (((ULONG)(_object) >> 8) & (SYSHASH_SIZE - 1))
#define SYSHASH4FOBJ(_object)  ((ULONG)(_object))

// -----------------------------------------------------------------------------------------

typedef struct _SYSNAMECACHE {
	ULONG	Key;
	struct _SYSNAMECACHE *Next;
	ULONG	Owner;
	ULONG	ObjectNameLen;
	CHAR	Name[];
} SYSNAMECACHE,*PSYSNAMECACHE;

typedef struct _KLG_SYS_NAME_CACHE
{
	PSYSNAMECACHE m_NameCache[SYSHASH_SIZE];
	ERESOURCE m_CacheResource;
} KLG_SYS_NAME_CACHE, *PKLG_SYS_NAME_CACHE;

//+ ------------------------------------------------------------------------------------------

BOOLEAN
SysNameCacheInit(PKLG_SYS_NAME_CACHE* ppNameCache);

VOID
SysNameCacheDone(PKLG_SYS_NAME_CACHE* ppNameCache);

//+ ------------------------------------------------------------------------------------------

VOID
SysNameCacheCleanupByProcInternal(PKLG_SYS_NAME_CACHE pNameCache, ULONG Owner);

BOOLEAN
SysNameCacheFree(PKLG_SYS_NAME_CACHE pNameCache, ULONG key);

VOID
SysNameCacheClean(PKLG_SYS_NAME_CACHE pNameCache);

VOID
SysNameCacheStore(PKLG_SYS_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG NameLen, ULONG Tag);

PVOID
SysNameCacheGet(PKLG_SYS_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf);

#endif //__SYSNAMECACHE_H