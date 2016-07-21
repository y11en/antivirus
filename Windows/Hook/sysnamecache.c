#include "sysnamecache.h"
#include "lists.h"
#include "debug.h"

BOOLEAN
SysNameCacheInit(PKLG_SYS_NAME_CACHE* ppNameCache)
{
	PKLG_SYS_NAME_CACHE pNameCacheTmp;
	ULONG cou;
	if (ppNameCache == NULL)
	{
		DbPrint(DC_NAMEC, DL_WARNING, ("Whatta hella!? NameCacheInit: incorrect NameCache IN parameter\n"));
		return FALSE;
	}

	if (*ppNameCache != NULL)
	{
		DbPrint(DC_NAMEC, DL_WARNING, ("NameCache - already inited!\n"));
		return FALSE;
	}
	
	*ppNameCache = ExAllocatePoolWithTag(NonPagedPool, sizeof(KLG_SYS_NAME_CACHE), 'NboS');
	if  (*ppNameCache == NULL)
	{
		DbPrint(DC_NAMEC, DL_WARNING, ("NameCache not inited - low memory\n"));
		return FALSE;
	}

	pNameCacheTmp = *ppNameCache;
	ExInitializeResourceLite(&pNameCacheTmp->m_CacheResource);
	for (cou = 0; cou < SYSHASH_SIZE; cou++)
	{
		pNameCacheTmp->m_NameCache[cou] = NULL;
	}

	DbPrint(DC_NAMEC, DL_INFO, ("NameCacheInit complete\n"));

	return TRUE;
}

VOID
SysNameCacheDone(PKLG_SYS_NAME_CACHE* ppNameCache)
{
	PKLG_SYS_NAME_CACHE pNameCacheTmp;
	if (ppNameCache == NULL)
	{
		DbPrint(DC_NAMEC, DL_WARNING, ("Whatta hella!? NameCacheDone: incorrect NameCache IN parameter\n"));
	}
	
	pNameCacheTmp = *ppNameCache;
	if (pNameCacheTmp == NULL)
	{
		DbPrint(DC_NAMEC, DL_WARNING, ("NameCache - already done!\n"));
		return;
	}
	
	SysNameCacheClean(pNameCacheTmp);

	ExDeleteResourceLite(&pNameCacheTmp->m_CacheResource);
	ExFreePool(pNameCacheTmp);
	*ppNameCache = NULL;

	DbPrint(DC_NAMEC, DL_IMPORTANT, ("NameCacheDone complete\n"));
}

VOID
SysNameCacheClean(PKLG_SYS_NAME_CACHE pNameCache)
{
	ULONG	i;
	PSYSNAMECACHE	Tmp,Entry;
	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC, DL_INFO, ("NameCacheClean: wrong internal status - NULL\n"));
		return;
	}

	_RESOURCE_LOCK_W(&pNameCache->m_CacheResource);
	for(i = 0; i < SYSHASH_SIZE; i++)
	{
		Entry = pNameCache->m_NameCache[i];
		while(Entry)
		{
			Tmp = Entry->Next;
			ExFreePool(Entry);
			Entry = Tmp;
		}
		pNameCache->m_NameCache[i] = NULL;
	}
	DbPrint(DC_NAMEC, DL_NOTIFY, ("CacheCleanup-%#x\n",pNameCache));
	_RESOURCE_UNLOCK(&pNameCache->m_CacheResource);
}


VOID
SysNameCacheCleanupByProcInternal(PKLG_SYS_NAME_CACHE pNameCache, ULONG Owner)
{
	PSYSNAMECACHE	Entry;
	PSYSNAMECACHE	Tmp;
	PSYSNAMECACHE	Del;
	ULONG	i;
	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC, DL_INFO, ("NameCacheCleanupByProcInternal: wrong internal status - NULL\n"));
		return;
	}

	_RESOURCE_LOCK_W(&pNameCache->m_CacheResource);
	for(i = 0; i < SYSHASH_SIZE; i++)
	{
		Entry = pNameCache->m_NameCache[i];
		Tmp = NULL;
		while(Entry)
		{
			if(Entry->Owner == Owner)
			{
				if(Tmp)
					Tmp->Next=Entry->Next;
				else
					pNameCache->m_NameCache[i] = Entry->Next;
				Del = Entry;
				Entry = Entry->Next;
				ExFreePool(Del);
				continue;
			}
			Tmp = Entry;
			Entry = Entry->Next;
		}
	}
	DbPrint(DC_NAMEC, DL_NOTIFY, ("CacheCleanupByProc-%#x %#x\n", pNameCache, Owner));

	_RESOURCE_UNLOCK(&pNameCache->m_CacheResource);
}

BOOLEAN
SysNameCacheFree(PKLG_SYS_NAME_CACHE pNameCache, ULONG key)
{
	PSYSNAMECACHE	Entry;
	PSYSNAMECACHE	Prev;
	ULONG i;
	ULONG Owner;

	if (pNameCache == NULL)
		return FALSE;

	Owner = (ULONG) PsGetCurrentProcessId();
	Prev = NULL;
	i = SYSHASH_FUNC(key);
	
	_RESOURCE_LOCK_W(&pNameCache->m_CacheResource);

	Entry = pNameCache->m_NameCache[i];
	while(Entry)
	{
		if (Entry->Key == key)
		{
			if (Entry->Owner == Owner)
				break;
		}
		Prev = Entry;
		Entry = Entry->Next;
	}
	if(!Entry)
	{
		DbPrint(DC_NAMEC, DL_SPAM, ("CacheFree-%#x %#x not found\n",pNameCache,key));
	
		_RESOURCE_UNLOCK(&pNameCache->m_CacheResource);
		return FALSE;
	}

	if(Prev)
		Prev->Next=Entry->Next;
	else
		pNameCache->m_NameCache[i]=Entry->Next;

	ExFreePool(Entry);
	_RESOURCE_UNLOCK(&pNameCache->m_CacheResource);

	return TRUE;

}

VOID
SysNameCacheStore(PKLG_SYS_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG NameLen, ULONG Tag)
{
	PSYSNAMECACHE	Entry;
	PSYSNAMECACHE	Prev;
	ULONG Owner;
	ULONG i;
	if(!key)
		return;

	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_INFO, ("NameCacheStore: wrong internal status - NULL\n"));
		return;
	}
	
	Owner = (ULONG) PsGetCurrentProcessId();
	Prev = NULL;
	i = SYSHASH_FUNC(key);
	AcquireResource(&pNameCache->m_CacheResource, TRUE);
	
	Entry = pNameCache->m_NameCache[i];
	while(Entry)
	{
		if (Entry->Key == key)
		{
			if (Entry->Owner == Owner)
				break;
		}
		Prev = Entry;
		Entry = Entry->Next;
	}
	if(Entry) 
	{
		//DbPrint(DC_NAMEC,DL_SPAM, ("!!! NameCacheStore-%x: dupe %x Old %x ("_STRING_DEF") New %x ("_STRING_DEF")\n",pNameCache,Entry->Key,Entry->Owner,Entry->Name,Owner,name));
		if(Prev)
			Prev->Next = Entry->Next;
		else
			pNameCache->m_NameCache[i] = Entry->Next;
		ExFreePool(Entry);
	}
	
	Entry = ExAllocatePoolWithTag(NonPagedPool, sizeof(SYSNAMECACHE) + NameLen, Tag);
	if(Entry)
	{
		Entry->Key = key;
		Entry->Owner = Owner;
		Entry->ObjectNameLen = NameLen;
		memcpy(Entry->Name, name, NameLen);
		Entry->Next = pNameCache->m_NameCache[i];
		pNameCache->m_NameCache[i] = Entry;
		ReleaseResource(&pNameCache->m_CacheResource);
		return;
	}	

	DbPrint(DC_NAMEC,DL_ERROR, ("NameCacheStore-%x failed, no memory...\n",pNameCache));
	ReleaseResource(&pNameCache->m_CacheResource);
}

//don't set overbuff in function (on start) to false - it may be used recurse
PVOID
SysNameCacheGetBase(PKLG_SYS_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf, BOOLEAN bCheckZero)
{
	PSYSNAMECACHE	Entry;
	PVOID ret = NULL;
	ULONG i;
	ULONG ZeroLen;

	ULONG Owner = (ULONG) PsGetCurrentProcessId();
	i = SYSHASH_FUNC(key);

	Entry = pNameCache->m_NameCache[i];
	while(Entry)
	{
		if (Entry->Key == key)
		{
			if (Entry->Owner == Owner)
				break;
		}
		Entry = Entry->Next;
	}

	if (bCheckZero)
		ZeroLen = sizeof(WCHAR);
	else
		ZeroLen = 0;

	if(Entry)
	{
		ret = Entry->Name;
		if (Entry->ObjectNameLen > (uNameSize - ZeroLen))
			*pbOverBuf = TRUE;
		if(name)
		{
			if (Entry->ObjectNameLen > (uNameSize - ZeroLen))
			{
				DbPrint(DC_NAMEC, DL_SPAM, ("CacheGet-Request buffer too small. Data truncated in %d. stored %d\n", uNameSize, Entry->ObjectNameLen));
				memcpy(name,Entry->Name, uNameSize);
				*(WCHAR*)((PCHAR)name+uNameSize - ZeroLen) = 0;
			}
			else
				memcpy(name,Entry->Name, Entry->ObjectNameLen);
		}
		//DbPrint(DC_NAMEC,DL_INFO, ("CacheGet-%x %x ("_STRING_DEF") Refs %d. NCLen=%d\n",pNameCache,key,ret,Entry->RefCount,pNameCache->m_NCLen));
	}
	/*else
	{
		DbPrint(DC_NAMEC,DL_SPAM, ("CacheGet-%x %x not found. NCLen=%d\n",pNameCache,key,pNameCache->m_NCLen));
	}*/
	return ret;

}

PVOID
SysNameCacheGet(PKLG_SYS_NAME_CACHE pNameCache, ULONG key, PVOID name, ULONG uNameSize, BOOLEAN* pbOverBuf)
{
	PVOID pRet = NULL;
	if (pNameCache == NULL)
	{
		DbPrint(DC_NAMEC,DL_SPAM, ("NameCacheGet: wrong internal status - NULL\n"));
		return pRet;
	}
		
	AcquireResource(&pNameCache->m_CacheResource, FALSE);
	
	pRet = SysNameCacheGetBase(pNameCache, key, name, uNameSize, pbOverBuf, TRUE);
		
	ReleaseResource(&pNameCache->m_CacheResource);

	return pRet;
}