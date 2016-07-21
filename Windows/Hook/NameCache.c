#include "namecache.h"

#ifdef _AI_MEM_USE
#pragma message("----------------------- AI Memory Menegment is ON")
#else
#pragma message("----------------------- AI Memory Menegment is OFF")
#endif
// -----------------------------------------------------------------------------------------

BOOLEAN CacheInited = FALSE;
BOOLEAN AiMemInited = FALSE;

// -----------------------------------------------------------------------------------------
// Sobko's implemetation of Bes's namecache

PKLG_NAME_CACHE g_pCommon_NameCache = NULL;
PKLG_NAME_CACHE g_pNameCache_DriveLetter = NULL;
PKLG_NAME_CACHE g_pCommon_NameCache_Context = NULL;

//+ ------------------------------------------------------------------------------------------

#ifdef _AI_MEM_USE
PAIMemObject g_pAiMem = NULL;
PAIMemObject g_pAiMemObjectName = NULL;
#endif


// -----------------------------------------------------------------------------------------
int
_AIMem_InitBlocks(PAIMemObject pAIMem, ULONG ItemSize)
{
	// init critical section in pList
	int res = 0;
	BYTE cou;
	PAIMemBlock pBlock;
	for (cou = 0; cou < sizeof(pAIMem->m_Blocks) / sizeof(pAIMem->m_Blocks[0]); cou++)
	{
		pBlock = (PAIMemBlock) ExAllocatePoolWithTag(NonPagedPool, ItemSize + sizeof(AIMemBlock) ,'~boS');
		if (pBlock == 0)
		{
			pAIMem->m_Blocks[cou] = 0;
			res = 1;
		}
		else
		{
			pBlock->m_AIMem_BlockType = 0;
			pBlock->m_Tag = 0;
			pBlock->m_Idx = cou;
			pAIMem->m_Blocks[cou] = pBlock;
		}
	}

	return !res;
}

void
_AIMem_FreeBlocks(PAIMemObject pAIMem)
{
	PAIMemBlock pBlock;
	BYTE cou;
	
	for (cou = 0; cou < sizeof(pAIMem->m_Blocks) / sizeof(pAIMem->m_Blocks[0]); cou++)
	{
		pBlock = pAIMem->m_Blocks[cou];
		if (pBlock != 0)
		{
			ExFreePool(pBlock);
			pAIMem->m_Blocks[cou] = 0;
		}
	}
}

void
_AIMem_Done(PAIMemObject pAIMem)
{
	if(pAIMem)
	{
		_AIMem_FreeBlocks(pAIMem);
		ExDeleteResourceLite(&pAIMem->m_CacheResource);
		ExFreePool(pAIMem);
	}
}

PAIMemObject
_AIMem_Init(int BlockSize)
{
	PAIMemObject pAIMem;
	pAIMem = (PAIMemObject) ExAllocatePoolWithTag(NonPagedPool, sizeof(AIMemObject), '~boS');
	if (pAIMem == 0)
		return 0;
	
	ExInitializeResourceLite(&pAIMem->m_CacheResource);
#ifdef __DBG__	
	pAIMem->m_dwMaxItems = 0;
	pAIMem->m_dwCurItems = 0;
#endif
	pAIMem->m_Hint = 0;
	pAIMem->m_BlockSize = BlockSize;
	if (_AIMem_InitBlocks(pAIMem, BlockSize) == 0)
	{
		_AIMem_Done(pAIMem);
		pAIMem = 0;
	}

	return pAIMem;
}

void*
_AIMem_Alloc(PAIMemObject pAIMem, ULONG size, int tag)
{
	void *p = 0;
	PAIMemBlock pBlock;
	BYTE cou;

	if (AiMemInited == FALSE || pAIMem == NULL)
		return p;

	AcquireResource(&pAIMem->m_CacheResource, TRUE);
#ifdef __DBG__
	pAIMem->m_dwCurItems++;
	if (pAIMem->m_dwCurItems > pAIMem->m_dwMaxItems)
	{
		pAIMem->m_dwMaxItems = pAIMem->m_dwCurItems;
		DbPrint(DC_NAMEC, DL_SPAM, ("AiMem: New max level %d\n", pAIMem->m_dwMaxItems));
	}
#endif
	if (size > pAIMem->m_BlockSize)
	{
		// default allocation
		pBlock = (PAIMemBlock) ExAllocatePoolWithTag(NonPagedPool, size + sizeof(AIMemBlock), tag);
		if (pBlock == 0)
		{
			ReleaseResource(&pAIMem->m_CacheResource);
			return p;
		}
		else
		{
			pBlock->m_AIMem_BlockType = 1;
			pBlock->m_Tag = tag;

			p = ((BYTE*) pBlock) + sizeof(AIMemBlock);
		}
	}
	else
	{
		pBlock = pAIMem->m_Blocks[pAIMem->m_Hint];
		if (pBlock->m_Tag == 0)
		{
			pBlock->m_Tag = tag;
			p = ((BYTE*) pBlock) + sizeof(AIMemBlock);
			ReleaseResource(&pAIMem->m_CacheResource);
			
			return p;
		}
		
		for (cou = 0; cou < sizeof(pAIMem->m_Blocks) / sizeof(pAIMem->m_Blocks[0]); cou++)
		{
			pBlock = pAIMem->m_Blocks[cou];
			if (pBlock->m_Tag == 0)
			{
				pBlock->m_Tag = tag;
				p = ((BYTE*) pBlock) + sizeof(AIMemBlock);
				ReleaseResource(&pAIMem->m_CacheResource);
				
				return p;
			}				
		}
		pBlock = (PAIMemBlock) ExAllocatePoolWithTag(NonPagedPool, size + sizeof(AIMemBlock), tag);
		if (pBlock != 0)
		{
			pBlock->m_AIMem_BlockType = 1;
			pBlock->m_Tag = tag;
			
			p = ((BYTE*) pBlock) + sizeof(AIMemBlock);
		}
	}

	ReleaseResource(&pAIMem->m_CacheResource);
	return p;
}

void
_AIMem_Free(PAIMemObject pAIMem, void* p)
{
	PAIMemBlock pBlock;
	if (pAIMem == NULL)
	{
		// debug info
		DbgBreakPoint();
		return;
	}

#ifdef __DBG__
	pAIMem->m_dwCurItems--;
#endif
	
	pBlock = (PAIMemBlock) (((BYTE*) p) - sizeof(AIMemBlock));
	if (pBlock->m_AIMem_BlockType == 1)
		ExFreePool(pBlock);
	else
	{
		AcquireResource(&pAIMem->m_CacheResource, TRUE);		
		pBlock->m_Tag = 0;
		pAIMem->m_Hint = pBlock->m_Idx;
		ReleaseResource(&pAIMem->m_CacheResource);
	}
}

// -----------------------------------------------------------------------------------------

VOID
NameCacheClean(PKLG_NAME_CACHE pNameCache)
{
	ULONG	i;
	PNAMECACHE	Tmp,Entry;
	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_INFO, ("NameCacheClean: wrong internal status - NULL\n"));
		return;
	}

	AcquireResource(&pNameCache->m_CacheResource, TRUE);
	for(i =0; i < HASH_SIZE; i++)
	{
		Entry=pNameCache->m_NameCache[i];
		while(Entry)
		{
			Tmp=Entry->Next;
			ExFreePool(Entry);
			Entry = Tmp;
#ifdef __DBG__
			pNameCache->m_NCLen--;
			pNameCache->m_NameCacheLen[i]--;
#endif
		}
		pNameCache->m_NameCache[i] = NULL;
	}
	DbPrint(DC_NAMEC,DL_NOTIFY, ("CacheCleanup-%x. NCLen=%d\n",pNameCache,pNameCache->m_NCLen));
	ReleaseResource(&pNameCache->m_CacheResource);
}

//don't set overbuff in function (on start) to false - it may be used recurse
PVOID
NameCacheGetBase(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf, 
				 BOOLEAN bCheckZero, PULONG pRefCount)
{
	PNAMECACHE Entry;
	PVOID ret = NULL;
	ULONG i;
	ULONG ZeroLen;

	i = HASH_FUNC(key);

	Entry=pNameCache->m_NameCache[i];
	while(Entry && Entry->Key!=key)
		Entry=Entry->Next;

#ifdef _HOOK_NT_	
#define _SINGLE_SYMBOL	WCHAR
#else
#define _SINGLE_SYMBOL	CHAR
#endif

	if (bCheckZero)
		ZeroLen = sizeof(_SINGLE_SYMBOL);
	else
		ZeroLen = 0;

	if(Entry)
	{
		if (pRefCount)
			*pRefCount = Entry->RefCount;
		ret=Entry->Name;
		if (Entry->ObjectNameLen > (uNameSize - ZeroLen))
			*pbOverBuf = TRUE;
		if(name)
		{
			if (Entry->ObjectNameLen > (uNameSize - ZeroLen))
			{
				DbPrint(DC_NAMEC, DL_SPAM, ("CacheGet-Request buffer too small. Data truncated in %d. stored %d\n", 
					uNameSize, Entry->ObjectNameLen));
				memcpy(name,Entry->Name, uNameSize);
				*(_SINGLE_SYMBOL*)((PCHAR)name+uNameSize - ZeroLen)=0;
			}
			else
				memcpy(name,Entry->Name, Entry->ObjectNameLen);
		}
		DbPrint(DC_NAMEC,DL_INFO, ("CacheGet-%x %x ("_STRING_DEF") Refs %d. NCLen=%d\n", pNameCache, key, ret,
			Entry->RefCount,pNameCache->m_NCLen));
	}
	else
		DbPrint(DC_NAMEC,DL_SPAM, ("CacheGet-%x %x not found. NCLen=%d\n",pNameCache,key,pNameCache->m_NCLen));
	
	return ret;

}

PVOID
NameCacheGet(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf)
{
	PVOID pRet = NULL;
	if (!CacheInited)
		return pRet;

	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_SPAM, ("NameCacheGet: wrong internal status - NULL\n"));
		return pRet;
	}
		
	AcquireResource(&pNameCache->m_CacheResource, FALSE);
	
	pRet = NameCacheGetBase(pNameCache, key, name, uNameSize, pbOverBuf, TRUE, NULL);
		
	ReleaseResource(&pNameCache->m_CacheResource);

	return pRet;
}

PVOID
NameCacheGetBinary(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf, PULONG pRefCount)
{
	PVOID pRet = NULL;
	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_SPAM, ("NameCacheGet: wrong internal status - NULL\n"));
		return pRet;
	}
		
	AcquireResource(&pNameCache->m_CacheResource, FALSE);
	
	pRet = NameCacheGetBase(pNameCache, key, name, uNameSize, pbOverBuf, FALSE, pRefCount);
		
	ReleaseResource(&pNameCache->m_CacheResource);

	return pRet;

}

VOID
NameCacheGetReplaceBinary(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID Value, DWORD ValueLen)
{
	PVOID pRet = NULL;
	BOOLEAN bOverBuf = FALSE;
	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_SPAM, ("NameCacheReplaceBinary: wrong internal status - NULL\n"));
		return;
	}
		
	AcquireResource(&pNameCache->m_CacheResource, FALSE);
	
	pRet = NameCacheGetBase(pNameCache, key, NULL, ValueLen, &bOverBuf, FALSE, NULL);
	if (bOverBuf == TRUE)
	{
		DbPrint(DC_NAMEC, DL_WARNING, ("NameCacheReplaceBunary: size mismatch\n"));
		DbgBreakPoint();
	}
	else
	{
		if (pRet != NULL)
			memcpy(pRet, Value, ValueLen);
	}
		
	ReleaseResource(&pNameCache->m_CacheResource);
}

VOID
NameCacheStore(PKLG_NAME_CACHE pNameCache, ULONG key, PVOID name, DWORD NameLen, BOOLEAN bAllowIncreaseRef, DWORD Tag)
{
	PNAMECACHE	Entry;
	PNAMECACHE	Prev;
	VOID	*Owner;
	ULONG i;
	if(!key) {
		DbPrint(DC_NAMEC, DL_INFO, ("!!! NameCacheStore-%x NULL key for "_STRING_DEF"\n", pNameCache, name));
		//DbgBreakPoint();
		return;
	}

	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_INFO, ("NameCacheStore: wrong internal status - NULL\n"));
		return;
	}
	
	if(name==UnknownStr)
		return;
	Owner=PsGetCurrentProcessId();
	Prev=NULL;
	i = HASH_FUNC(key);
	AcquireResource(&pNameCache->m_CacheResource, TRUE);
	
	Entry=pNameCache->m_NameCache[i];
	while(Entry && Entry->Key!=key)
	{
		Prev=Entry;
		Entry=Entry->Next;
	}
	if(Entry) 
	{
		if (bAllowIncreaseRef == TRUE)
		{
			Entry->RefCount++;
			DbPrint(DC_NAMEC, DL_INFO, ("NameCacheStore-%x: Owner=%x Ref++ key %x ("_STRING_DEF") NCLen=%d (ref = %d)\n",
				pNameCache,Entry->Owner,key,name,pNameCache->m_NCLen, Entry->RefCount));
			ReleaseResource(&pNameCache->m_CacheResource);
			return;
		}
		else
		{
			DbPrint(DC_NAMEC,DL_SPAM, ("!!! NameCacheStore-%x: dupe %x Old %x ("_STRING_DEF") New %x ("_STRING_DEF")\n",
				pNameCache,Entry->Key,Entry->Owner,Entry->Name,Owner,name));
			if(Prev)
				Prev->Next = Entry->Next;
			else
				pNameCache->m_NameCache[i]=Entry->Next;
	#ifdef __DBG__
			pNameCache->m_NameCacheLen[i]--;
			pNameCache->m_NCLen--;
	#endif
			ExFreePool(Entry);
		}
	}
	
	Entry=ExAllocatePoolWithTag(NonPagedPool,sizeof(NAMECACHE)+NameLen, Tag);
	if(Entry)
	{
		Entry->Key=key;
		Entry->RefCount=1;
		Entry->Owner=Owner;
		Entry->ObjectNameLen = NameLen;
		memcpy(Entry->Name, name, NameLen);
		Entry->Next=pNameCache->m_NameCache[i];
		pNameCache->m_NameCache[i]=Entry;
#ifdef __DBG__
		pNameCache->m_NameCacheLen[i]++;
		pNameCache->m_NCLen++;
		DbPrint(DC_NAMEC,DL_INFO, ("NameCacheStore-%x: Owner=%x Added key %x ("_STRING_DEF") NCLen=%d\n",
			pNameCache, Entry->Owner, key, name, pNameCache->m_NCLen));
#endif
		ReleaseResource(&pNameCache->m_CacheResource);
		
		return;
	}	
	DbPrint(DC_NAMEC,DL_ERROR, ("NameCacheStore-%x failed, no memory... NCLen=%d\n", pNameCache, pNameCache->m_NCLen));
	ReleaseResource(&pNameCache->m_CacheResource);
}

BOOLEAN
NameCacheFree(PKLG_NAME_CACHE pNameCache, ULONG key)
{
	PNAMECACHE	Entry;
	PNAMECACHE	Prev;
	ULONG i;

	if (pNameCache == NULL)
		return FALSE;

	Prev = NULL;
	i = HASH_FUNC(key);
	AcquireResource(&pNameCache->m_CacheResource, TRUE);
	Entry=pNameCache->m_NameCache[i];
	while(Entry && Entry->Key!=key)
	{
		Prev=Entry;
		Entry=Entry->Next;
	}
	if(!Entry)
	{
		DbPrint(DC_NAMEC,DL_SPAM, ("CacheFree-%x %x not found. NCLen=%d\n",pNameCache,key,pNameCache->m_NCLen));
		ReleaseResource(&pNameCache->m_CacheResource);
	
		return FALSE;
	}
	Entry->RefCount--;
	if(Entry->RefCount)
	{
		DbPrint(DC_NAMEC,DL_INFO, ("CacheFree-%x. Owner=%x Ref decr for key %x ("_STRING_DEF") NCLen=%d\n",
			pNameCache, Entry->Owner, Entry->Key, Entry->Name, pNameCache->m_NCLen));
		ReleaseResource(&pNameCache->m_CacheResource);
		
		return TRUE;
	}
	if(Prev)
		Prev->Next = Entry->Next;
	else
		pNameCache->m_NameCache[i]=Entry->Next;

#ifdef __DBG__
	pNameCache->m_NCLen--;
	pNameCache->m_NameCacheLen[i]--;
	DbPrint(DC_NAMEC,DL_INFO, ("CacheFree-%x. Owner=%x Freed key %x ("_STRING_DEF") NCLen=%d\n", pNameCache, 
		Entry->Owner, Entry->Key, Entry->Name, pNameCache->m_NCLen));
#endif
	ReleaseResource(&pNameCache->m_CacheResource);
	ExFreePool(Entry);
	
	return TRUE;
}

VOID
NameCacheCleanupByProcInternal(PKLG_NAME_CACHE pNameCache, VOID *ProcHandle)
{
	PNAMECACHE	Entry;
	PNAMECACHE	Tmp;
	PNAMECACHE	Del;
	ULONG	i;
#ifdef __DBG__
	ULONG	Deleted=0;
#endif
	if (pNameCache == NULL)
	{
		if (CacheInited == TRUE)
			DbPrint(DC_NAMEC,DL_INFO, ("NameCacheFree: wrong internal status - NULL\n"));
		return;
	}

	AcquireResource(&pNameCache->m_CacheResource, TRUE);
	for(i = 0; i < HASH_SIZE; i++)
	{
		Entry=pNameCache->m_NameCache[i];
		Tmp=NULL;
		while(Entry)
		{
			if(Entry->Owner==ProcHandle)
			{
				DbPrint(DC_NAMEC,DL_INFO, ("CacheCleanupByProc-%x %x Del %x ("_STRING_DEF")\n",
					pNameCache,ProcHandle, Entry->Key, Entry->Name));
				Entry->RefCount--;
				if(Entry->RefCount)
				{
					//!! ????
					Tmp=Entry;
					Entry=Entry->Next;
					continue;	
				}
				if(Tmp)
					Tmp->Next = Entry->Next;
				else
					pNameCache->m_NameCache[i]=Entry->Next;
#ifdef __DBG__
				pNameCache->m_NCLen--;
				pNameCache->m_NameCacheLen[i]--;
				Deleted++;
#endif
				Del=Entry;
				Entry=Entry->Next;
				ExFreePool(Del);
				continue;
			}
			Tmp=Entry;
			Entry=Entry->Next;
		}
	}
	DbPrint(DC_NAMEC,DL_NOTIFY, ("CacheCleanupByProc-%x %x NCLen=%d Deleted=%d\n", pNameCache, 
		ProcHandle, pNameCache->m_NCLen,Deleted));
	ReleaseResource(&pNameCache->m_CacheResource);
}

VOID
NameCacheCleanupByProc(VOID *ProcHandle)
{
	NameCacheCleanupByProcInternal(g_pCommon_NameCache, ProcHandle);
}

BOOLEAN
NameCacheInit(PKLG_NAME_CACHE* ppNameCache)
{
	PKLG_NAME_CACHE pNameCacheTmp;
	DWORD cou;
	if (ppNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_WARNING, ("Whatta hella!? NameCacheInit: incorrect NameCache IN parameter\n"));
		DbgBreakPoint();
		return FALSE;
	}

	if (*ppNameCache != NULL)
	{
		DbPrint(DC_NAMEC,DL_WARNING, ("NameCache - already inited!\n"));
		DbgBreakPoint();
		return FALSE;
	}
	
	*ppNameCache = ExAllocatePoolWithTag(NonPagedPool, sizeof(KLG_NAME_CACHE), 'NboS');
	if  (*ppNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_WARNING, ("NameCache not inited - low memory\n"));
		DbgBreakPoint();
		return FALSE;
	}

	pNameCacheTmp = *ppNameCache;
	ExInitializeResourceLite(&pNameCacheTmp->m_CacheResource);
	for (cou = 0; cou < HASH_SIZE; cou++)
	{
		pNameCacheTmp->m_NameCache[cou] = NULL;
#ifdef __DBG__
		pNameCacheTmp->m_NameCacheLen[cou]=0;
#endif
	}
#ifdef __DBG__
	pNameCacheTmp->m_NCLen = 0;
	
#endif
	DbPrint(DC_NAMEC, DL_INFO, ("NameCacheInit complete\n"));

	return TRUE;
}

void NameCacheDone(PKLG_NAME_CACHE* ppNameCache)
{
	PKLG_NAME_CACHE pNameCacheTmp;
	if (ppNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_WARNING, ("Whatta hella!? NameCacheDone: incorrect NameCache IN parameter\n"));
		DbgBreakPoint();
	}
	
	pNameCacheTmp = *ppNameCache;
	if (pNameCacheTmp == NULL)
	{
		DbPrint(DC_NAMEC,DL_WARNING, ("NameCache - already done!\n"));
		DbgBreakPoint();
		return;
	}
	
	NameCacheClean(pNameCacheTmp);

	ExDeleteResourceLite(&pNameCacheTmp->m_CacheResource);
	ExFreePool(pNameCacheTmp);
	*ppNameCache = NULL;

	DbPrint(DC_NAMEC,DL_IMPORTANT, ("NameCacheDone complete\n"));
}

// end Sobko's implemetation of Bes's namecache
// -----------------------------------------------------------------------------------------

void GlobalCacheInit()
{
	if(CacheInited)
	{
		DbPrint(DC_NAMEC,DL_WARNING, ("Whatta hella!? GlobalCacheInit already done.\n"));
		return;
	}

	NameCacheInit(&g_pCommon_NameCache);
	NameCacheInit(&g_pNameCache_DriveLetter);
	NameCacheInit(&g_pCommon_NameCache_Context);

#ifdef _AI_MEM_USE
	g_pAiMem = _AIMem_Init(sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM) * 3 + _FILE_MAX_PATHLEN + 1 + sizeof(ULONG) + __SID_LENGTH);
	g_pAiMemObjectName = _AIMem_Init(2048);
#endif
	
	CacheInited=TRUE;
	AiMemInited = TRUE;
	DbPrint(DC_NAMEC,DL_IMPORTANT, ("GlobalCacheInit\n"));
}

void GlobalCacheDone()
{
	return;

	//! not use -> drive unloadable

	CacheInited=FALSE;
	NameCacheDone(&g_pCommon_NameCache);
	NameCacheDone(&g_pNameCache_DriveLetter);
	NameCacheDone(&g_pCommon_NameCache_Context);

#ifdef _AI_MEM_USE
	AiMemInited = FALSE;
	_AIMem_Done(g_pAiMem);
	_AIMem_Done(g_pAiMemObjectName);
#endif

	DbPrint(DC_NAMEC,DL_IMPORTANT, ("GlobalCacheDone\n"));
}
