#ifndef _NAMECACHE_H
#define _NAMECACHE_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	NameCache.h
*		\brief	кеш объектов, механизма быстрого выделения памяти
*		
*		\author Sergey Belov, Andrew Sobko
*		\date	12.09.2003 11:35:34
*		
*		
*		
*		
*/		

#include "osspec.h"
#include "debug.h"

//+ ------------------------------------------------------------------------------------------
//+ доступ к объекту 
#define HASH_FUNC(_object)  (((ULONG)(_object) >> 8) & (HASH_SIZE - 1))
#define HASH4FOBJ(_fobject)  ((ULONG)(_fobject))

//+ размер таблицы
#ifdef _HOOK_VXD_ 
	#define HASH_SIZE  128        // MUST be a power of 2
#else
	#define HASH_SIZE  256        // MUST be a power of 2
#endif

//+ цепочка объектов в кеше
typedef struct _NAMECACHE {
	ULONG	Key;
	struct _NAMECACHE *Next;
	ULONG	RefCount;
	VOID	*Owner;
	DWORD ObjectNameLen;
	CHAR	Name[];
} NAMECACHE,*PNAMECACHE;

//+ структура кеша
typedef struct _KLG_NAME_CACHE
{
	PNAMECACHE	m_NameCache[HASH_SIZE];
	ERESOURCE	m_CacheResource;
#ifdef __DBG__
	ULONG			m_NameCacheLen[HASH_SIZE];
	ULONG			m_NCLen;
#endif
} KLG_NAME_CACHE, *PKLG_NAME_CACHE;

//+ ------------------------------------------------------------------------------------------
//+ общие кеши
extern PKLG_NAME_CACHE g_pCommon_NameCache;
extern PKLG_NAME_CACHE g_pNameCache_DriveLetter;
extern PKLG_NAME_CACHE g_pCommon_NameCache_Context;

//! \fn				: GlobalCacheInit
//! \brief			: инициализация общих кешов и 'AIMem'
//! \return			: 
void
GlobalCacheInit();

//! \fn				: GlobalCacheDone
//! \brief			: удаление всех связанных данных
//! \return			: 
void
GlobalCacheDone();

//! \fn				: NameCacheInit
//! \brief			: инициализация структуры кеша
//! \return			: TRUE - успешно / FALSE - сбой при инициализации
//! \param          : PKLG_NAME_CACHE* ppNameCache - указатель на указатель (будет выделена память для нового объекта)
BOOLEAN
NameCacheInit(PKLG_NAME_CACHE* ppNameCache);

//! \fn				: NameCacheDone
//! \brief			: удаление кеша
//! \return			: 
//! \param          : PKLG_NAME_CACHE* ppNameCache - указатель на указатель (будет освобождена память)
void
NameCacheDone(PKLG_NAME_CACHE* ppNameCache);

//! \fn				: NameCacheCleanupByProc
//! \brief			: очистка кешов с учетом Owner-а (используется при прибивании процесса)
//! \return			: 
//! \param          : VOID *ProcHandle - идентификатор процесса-владельца
VOID
NameCacheCleanupByProc(VOID *ProcHandle);

//! \fn				: NameCacheGet
//! \brief			: получить объект из кеша
//! \return			: указатель на объект (не использовать без синхронизации)
//! \param          : PKLG_NAME_CACHE pNameCache - кеш
//! \param          : ULONG key - ключ для поиска
//! \param          : PVOID name - указтель на буфер, в который будет произведена попытка скопировать связанные с ключом данные
//! \param          : ULONG uNameSize - размер буфера
//! \param          : BOOLEAN* pbOverBuf - признак переполнения буфера
PVOID
NameCacheGet(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf);

//! \fn				: NameCacheStore
//! \brief			: сохранить объект в кеше (если объект не может быть сохранён - игнорирование запроса)
//! \return			: 
//! \param          : PKLG_NAME_CACHE pNameCache - кеш
//! \param          : ULONG key - ключ
//! \param          : PVOID name - данные
//! \param          : DWORD NameLen - длина данных (в байтах)
//! \param          : BOOLEAN bAllowIncreaseRef - разрешение на использования счетчика ссылок (при запрете найденный 
//						блок будет обновлён)
//! \param          : DWORD Tag - тэг, используемый для выделения памяти для сохранения в кеше
VOID
NameCacheStore(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, DWORD NameLen, BOOLEAN bAllowIncreaseRef, DWORD Tag);

//! \fn				: NameCacheClean
//! \brief			: очистить кеш
//! \return			: 
//! \param          : PKLG_NAME_CACHE pNameCache - кеш
VOID
NameCacheClean(PKLG_NAME_CACHE pNameCache);

//! \fn				: NameCacheFree
//! \brief			: удалить объект из кеша (с учетом счетчика)
//! \return			: TRUE - объект удалён / FALSE - объект не найден
//! \param          : PKLG_NAME_CACHE pNameCache - кеш
//! \param          : ULONG key - ключ
BOOLEAN
NameCacheFree(PKLG_NAME_CACHE pNameCache, ULONG key);

//! \fn				: NameCacheGetBinary
//! \brief			: получить ассоциированные с ключём данные как бинарный блок
//! \return			: указатель на существующий блок (не использовать без синхронизации)
//! \param          : PKLG_NAME_CACHE pNameCache - кеш
//! \param          : ULONG key - ключ
//! \param          : PVOID name - указатель на буфер
//! \param          : ULONG uNameSize - размр буфера
//! \param          : BOOLEAN* pbOverBuf - признак переолнения буфера
PVOID
NameCacheGetBinary(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf, PULONG pRefCount);

//! \fn				: NameCacheGetReplaceBinary
//! \brief			: заменить существующие данные в кеше по ключу
//! \return			: 
//! \param          : PKLG_NAME_CACHE pNameCache -кеш
//! \param          : ULONG key - ключ
//! \param          : PVOID Value - указатель на новые данные
//! \param          : DWORD ValueLen - длина новых данных
VOID
NameCacheGetReplaceBinary(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID Value, DWORD ValueLen);

//+ ------------------------------------------------------------------------------------------

typedef struct _tAIMemBlock
{
	BYTE		m_AIMem_BlockType;	// 0 - private, 1 - extrenal
	BYTE		m_Idx;
	int		m_Tag;
}AIMemBlock, *PAIMemBlock;

typedef struct _tAIMemObject
{
#ifdef __DBG__
	DWORD			m_dwMaxItems;
	DWORD			m_dwCurItems;
#endif
	ERESOURCE	m_CacheResource;
	ULONG			m_Hint;
	ULONG			m_BlockSize;
	PAIMemBlock	m_Blocks[32];
}AIMemObject, *PAIMemObject;

//+ ------------------------------------------------------------------------------------------

#ifdef _AI_MEM_USE
extern PAIMemObject g_pAiMem;
extern PAIMemObject g_pAiMemObjectName;
#endif
//+ ------------------------------------------------------------------------------------------

void*
_AIMem_Alloc(PAIMemObject pAIMem, ULONG size, int tag);

void
_AIMem_Free(PAIMemObject pAIMem, void* p);

#endif // _NAMECACHE_H