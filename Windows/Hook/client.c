#include "client.h"
#include "filter.h"
#include "evqueue.h"

#ifdef _HOOK_NT_ 
	#include "nt/glbenvir.h"
#endif

#define _WatchInterval	3	// in second
#define _LID		0xff0000
#define _INVALID_PROCESS_VALUE	-1
#define _INVALID_THREAD_VALUE	00

ULONG gDriverSpecificID = _LID;

#ifdef _HOOK_NT_ 
void
FidBox_Allow();
#endif


// clients
// global queue registered clients
PERESOURCE gpClientsMutex = NULL;			// to protect client's list
PQCLIENT gpClients = NULL;

ULONG gActiveClientCounter = 0;

NTSTATUS
GlobalClientsInit(void)
{
	DbPrint(DC_CLIENT, DL_IMPORTANT, ("GlobalClientsInit\n"));
	gpClientsMutex = InitKlgResourse();
	if (gpClientsMutex == NULL)
	{
		DbPrint(DC_CLIENT, DL_ERROR, ("Can't init gpClientsMutex\n"));
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

void
GlobalClientsDone(void)
{
	DbPrint(DC_CLIENT, DL_IMPORTANT, ("GlobalClientsDone\n"));

	FreeKlgResourse(&gpClientsMutex);
	gpClientsMutex = NULL;
}

void
AcquireClientResource(BOOLEAN bExclusive)
{
	AcquireResource(gpClientsMutex, bExclusive);
}

void
ReleaseClientResource()
{
	ReleaseResource(gpClientsMutex);
}

#ifdef __DBG__
void
ShowSubClientInfo(PQSUBCLIENT pSubClient)
{
	while (pSubClient != NULL)
	{
		DbPrint(DC_CLIENT, DL_NOTIFY, ("SubClient: ProcessID %#x (%u), ThreadID %#x (%u) State %x\n", 
			pSubClient->m_ProcessId, pSubClient->m_ProcessId, pSubClient->m_ThreadId, pSubClient->m_ThreadId, 
			pSubClient->m_ActivityState));
		pSubClient = pSubClient->m_pNext;
	}
}

BOOLEAN
OutputClientsInfo()
{
	if (gActiveClientCounter != 0)
	{
		PQCLIENT pClient;

		AcquireClientResource(FALSE);
		
		pClient = gpClients;
		
		DbPrint(DC_CLIENT, DL_NOTIFY, ("Client info start ------------------------------------\n"));
		while(pClient)
		{
			// output info
			DbPrint(DC_CLIENT, DL_NOTIFY, ("ClntInf: ID %8d, Prty %d, EC %d, Ev %d (Unmrkd %d), Fls %x\n", 
				pClient->m_AppID, pClient->m_Priority, pClient->m_Enters, pClient->m_EventList.m_QLen, 
				pClient->m_EventList.m_QUnmarkedLen, pClient->m_ClientFlags));
			
			ShowSubClientInfo(pClient->m_pSubClient);
			ShowEventsInfo(&pClient->m_EventList);
			
			pClient = pClient->m_pNext;
		}
		DbPrint(DC_CLIENT, DL_NOTIFY, ("Client info end --------------------------------------\n"));

		ReleaseClientResource();
		return TRUE;
	}
	return FALSE;
}
#else
	#define OutputClientsInfo()
#endif
// -----------------------------------------------------------------------------------------
static DWORD crc_32_tab[] = {     /* CRC polynomial 0xedb88320 */
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
		0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
		0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
		0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
		0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
		0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
		0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
		0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
		0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
		0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
		0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
		0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
		0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
		0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
		0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
		0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
		0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
		0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
		0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
		0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
		0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
		0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
		0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
		0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
		0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
		0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
		0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
		0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
		0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
		0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
		0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
		0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
		0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
		0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
		0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
		0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
		0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
		0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
		0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
		0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
		0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
		0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
		0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

DWORD
CalcCacheID(ULONG size, BYTE* ptr, DWORD dwInitValue)
{
	DWORD i;
	DWORD crc = dwInitValue;
	for (i = 0; i < size; i++)
	{
		crc = UPDC32 (ptr[i], crc);
	}
	return (crc);
}

DWORD
CalcCacheIDUnSens(ULONG size, BYTE* ptr, const DWORD dwInitValue, const BOOLEAN IsUnicode)
{
	DWORD crc = dwInitValue;
	
	if (IsUnicode)
	{
		WCHAR* pwch = (WCHAR*) ptr;
		WCHAR c;
		BYTE second;
		while(size != 0)
		{
			c = *pwch++;
			if (c != 0)
			{
				c = _UpperCharW((WCHAR) c);
				// calc crc for 1 wchar
				crc = UPDC32(*(BYTE*)&c, crc);
				second = *((BYTE*)&c + 1);
				if (second != 0)
					crc = UPDC32(second, crc);
			}

			if (size >= sizeof(WCHAR))
				size -= sizeof(WCHAR);
			else
			{
				// pochemu data ne virovnena?
				// why data is not alligned?
				DbgBreakPoint();
				size = 0;
			}
		}
	}
	else
	{
		char c;
		while(size-- != 0) {
			c = *ptr++;
			if (c != 0)
			{
				c = _UpperCharA((CHAR) c);
        		// calc crc for 1 byte
				crc = UPDC32 (c, crc);
			}
		}
	}

    return crc;
}

#ifdef __DBG__
void
Obj_CacheCheck(PQCLIENT pClient)
{
/*	PCACHE_OBJ pCachePrev;
	PCACHE_OBJ pCacheObj;
	DWORD dwCount = 0;
	DWORD dwCacheSize = 0;
	
	AcquireResource(pClient->m_pCacheMutex, FALSE);

	pCacheObj = pClient->m_pCacheHead;
	pCachePrev = pClient->m_pCacheHead->m_pPrev;
	do {
		dwCount++;
		if (pCacheObj->m_CacheObj.m_dwVal != _INVALID_VALUE)
			dwCacheSize++;
		if (pCachePrev != pCacheObj->m_pPrev)
			DbPrint(DC_OBJC,DL_ERROR, ("CacheV invalid internal state!\n"));
		pCachePrev = pCacheObj;
		pCacheObj = pCacheObj->m_pNext;
	} while (pCacheObj != pClient->m_pCacheHead);

	DbPrint(DC_OBJC,DL_INFO, ("CacheV size is %d\n", dwCacheSize));

	if (dwCount != pClient->m_CacheSize)
		DbPrint(DC_OBJC,DL_ERROR, ("CacheV blocks invalid! Count is %d\n", dwCount));


	DbPrint(DC_OBJC,DL_ERROR, ("CacheV Head: %02d    Tail: %2d\n", pClient->m_pCacheHead - pClient->m_pCache, 
	pClient->m_pCacheTail - pClient->m_pCache));
	
	for (dwCount = 0;dwCount < 5; dwCount++)
	{
		pCacheObj = pClient->m_pCache + dwCount;
		DbPrint(DC_OBJC,DL_ERROR, ("CacheV internal: %02d    Next: %02d    Prev: %02d\n", dwCount, 
		pCacheObj->m_pNext - pClient->m_pCache, pCacheObj->m_pPrev - pClient->m_pCache));
	}

	DbPrint(DC_OBJC,DL_ERROR, ("------------------------------\n"));

	for (dwCount = pClient->m_CacheSize - 5; dwCount < pClient->m_CacheSize; dwCount++) {
		pCacheObj = pClient->m_pCache + dwCount;
		DbPrint(DC_OBJC,DL_ERROR, ("CacheV internal: %02d    Next: %02d    Prev: %02d\n", dwCount, 
		pCacheObj->m_pNext - pClient->m_pCache, pCacheObj->m_pPrev - pClient->m_pCache));
	}

	ReleaseResource(pClient->m_pCacheMutex);*/
}
#else
	#define Obj_CacheCheck(pClient)
#endif

BOOLEAN
Obj_CacheInit(PQCLIENT pClient, ULONG NewSize)
{
	PCACHE_OBJ pCacheObj = NULL;
	DWORD idx;
	int CacheID;

	ULONG cache_size_tmp;
	
	if (!NewSize)
		return TRUE;

	if (!pClient->m_pCacheMutex)
		return FALSE;
	
	AcquireResource(pClient->m_pCacheMutex, TRUE);

	pClient->m_CacheSize = NewSize;

	if (pClient->m_CacheSize < _CL_CACHE_QUEUES)
	{
//		DbPrint(DC_OBJC,DL_IMPORTANT, ("CacheV size increased (new size = %d)!\n", _CL_CACHE_QUEUES));
		pClient->m_CacheSize = _CL_CACHE_QUEUES;
	}
	else
	{
		if ((pClient->m_CacheSize % _CL_CACHE_QUEUES) != 0)
			pClient->m_CacheSize = pClient->m_CacheSize - (pClient->m_CacheSize % _CL_CACHE_QUEUES);
	}

	cache_size_tmp = sizeof(CACHE_OBJ) * (pClient->m_CacheSize / _CL_CACHE_QUEUES);
	
//	DbPrint(DC_OBJC, DL_IMPORTANT, ("CacheV size in bytes %d! Items %d\n", 
//		cache_size_tmp * _CL_CACHE_QUEUES,
//		_CL_CACHE_QUEUES * pClient->m_CacheSize / _CL_CACHE_QUEUES));

	for (CacheID = 0; CacheID < _CL_CACHE_QUEUES; CacheID++)
	{
		pClient->m_CacheCl[CacheID].m_pCache = NULL;
		pClient->m_CacheCl[CacheID].m_pCacheHead = NULL;

		pClient->m_CacheCl[CacheID].m_pCache = (PCACHE_OBJ) ExAllocatePoolWithTag(NonPagedPool, cache_size_tmp, 'cboS');

		if (pClient->m_CacheCl[CacheID].m_pCache != NULL)
		{
			for (idx = 0;idx < pClient->m_CacheSize / _CL_CACHE_QUEUES; idx++)
			{
				pCacheObj = pClient->m_CacheCl[CacheID].m_pCache + idx;
				pCacheObj->m_pNext = pCacheObj + 1;
				pCacheObj->m_pPrev = pCacheObj - 1;
				pCacheObj->m_CacheObj.m_dwVal = _INVALID_VALUE;
			}
			
			pClient->m_CacheCl[CacheID].m_pCacheHead = pClient->m_CacheCl[CacheID].m_pCache;
			pClient->m_CacheCl[CacheID].m_pCacheHead->m_pPrev = pCacheObj;
			
			if (pCacheObj)
				pCacheObj->m_pNext = pClient->m_CacheCl[CacheID].m_pCacheHead;
		}
		else
		{
			pClient->m_CacheSize = 0;
//			DbPrint(DC_OBJC,DL_ERROR, ("CacheV init failed (no memory)!\n"));
		}
	}
	
	ReleaseResource(pClient->m_pCacheMutex);

	Obj_CacheCheck(pClient);

	return FALSE;
}

BOOLEAN
Obj_CacheFree(PQCLIENT pClient)
{
	int CacheID;
	
	if (pClient->m_pCacheMutex == NULL)
		return TRUE;

	if (!pClient->m_CacheSize)
		return TRUE;

	AcquireResource(pClient->m_pCacheMutex, TRUE);

	for (CacheID = 0; CacheID < _CL_CACHE_QUEUES; CacheID++)
	{
		if (pClient->m_CacheCl[CacheID].m_pCache != NULL)
		{
			ExFreePool(pClient->m_CacheCl[CacheID].m_pCache);
			pClient->m_CacheCl[CacheID].m_pCache = NULL;
			pClient->m_CacheCl[CacheID].m_pCacheHead = NULL;
		}
	}
	pClient->m_CacheSize = 0;
	ReleaseResource(pClient->m_pCacheMutex);
	
	return TRUE;
}

DWORD
Obj_IsCached(PQCLIENT pClient, DWORD dwVal, BOOLEAN bNeedProtect, PVERDICT pVerdict)
{
	PCACHE_OBJ pCacheObj;
	BOOLEAN bExpired = FALSE;

	int CacheID = dwVal & _CL_CACHE_ACCESSMASK;

	if ((dwVal == _INVALID_VALUE) || (pClient->m_CacheCl[CacheID].m_pCache == NULL))
	{
//		DbPrint(DC_OBJC,DL_SPAM, ("Couldn't use iscached(%x-%x)!\n", pClient->m_CacheCl[CacheID].m_pCache, dwVal));
		return _CL_CACHE_NOTCACHED;
	}
	
	if (bNeedProtect == TRUE)
		AcquireResource(pClient->m_pCacheMutex, TRUE);

	pCacheObj = pClient->m_CacheCl[CacheID].m_pCacheHead;
	do
	{
		if (pCacheObj->m_CacheObj.m_dwVal == _INVALID_VALUE)
		{
			DbPrint(DC_OBJC,DL_INFO, ("CacheV miss! (val %x)\n", dwVal));
			break;
		}

		if ((pCacheObj->m_CacheObj.m_dwVal == dwVal))
		{
			if (pCacheObj->m_CacheObj.m_ExpirationTime != 0)
			{
				if (IsTimeExpired(pCacheObj->m_CacheObj.m_ExpirationTime)) // check for expiration
				{
					DbPrint(DC_OBJC,DL_NOTIFY, ("CacheV value %x expired.\n", pCacheObj->m_CacheObj.m_dwVal));
					bExpired = TRUE;
				}
			}

			DbPrint(DC_OBJC,DL_SPAM, ("Cache hit %x Verdict %d!\n", dwVal, pCacheObj->m_CacheObj.m_Verdict));

			if (pVerdict != NULL)
				*pVerdict = pCacheObj->m_CacheObj.m_Verdict;
			if (pCacheObj != pClient->m_CacheCl[CacheID].m_pCacheHead)
			{
				pCacheObj->m_pPrev->m_pNext = pCacheObj->m_pNext;
				pCacheObj->m_pNext->m_pPrev = pCacheObj->m_pPrev;

				pCacheObj->m_pNext = pClient->m_CacheCl[CacheID].m_pCacheHead;
				pCacheObj->m_pPrev = pClient->m_CacheCl[CacheID].m_pCacheHead->m_pPrev;

				pCacheObj->m_pPrev->m_pNext = pCacheObj;
				pCacheObj->m_pNext->m_pPrev = pCacheObj;

				pClient->m_CacheCl[CacheID].m_pCacheHead = pCacheObj;
			}
			if (bExpired)
			{
				pCacheObj->m_CacheObj.m_dwVal = _INVALID_VALUE;
				pClient->m_CacheCl[CacheID].m_pCacheHead = pCacheObj->m_pNext;
			}

			if (bNeedProtect == TRUE)
				ReleaseResource(pClient->m_pCacheMutex);

			if (bExpired)
				return _CL_CACHE_NOTCACHED;

			if (pCacheObj->m_CacheObj.m_bDirtyFlag == FALSE)
				return _CL_CACHE_CACHED;	
			return _CL_CACHE_CACHED_DIRTY;
		}

		pCacheObj = pCacheObj->m_pNext;
	} while (pCacheObj != pClient->m_CacheCl[CacheID].m_pCacheHead);

	if (bNeedProtect == TRUE)
		ReleaseResource(pClient->m_pCacheMutex);
	
	return _CL_CACHE_NOTCACHED;
}

BOOLEAN
Obj_CacheAdd(PQCLIENT pClient, DWORD dwVal, BOOLEAN bDirty, VERDICT Verdict, ULONG ExpirationTimeShift)
{
	int CacheID = dwVal & _CL_CACHE_ACCESSMASK;

	if (pClient->m_CacheCl[CacheID].m_pCache == NULL)
	{
		DbPrint(DC_OBJC,DL_WARNING, ("Couldn't use cache(pCache is NULL)!\n"));
		return FALSE;
	}
	
	if (dwVal == _INVALID_VALUE)
	{
		DbPrint(DC_OBJC,DL_WARNING, ("Invalid cache value!\n"));
		return FALSE;
	}
	
	AcquireResource(pClient->m_pCacheMutex, TRUE);
	
	pClient->m_CacheCl[CacheID].m_pCacheHead = pClient->m_CacheCl[CacheID].m_pCacheHead->m_pPrev;
	pClient->m_CacheCl[CacheID].m_pCacheHead->m_CacheObj.m_dwVal = dwVal;
	
	pClient->m_CacheCl[CacheID].m_pCacheHead->m_CacheObj.m_bDirtyFlag = bDirty;
	pClient->m_CacheCl[CacheID].m_pCacheHead->m_CacheObj.m_Verdict = Verdict;

	DbPrint(DC_OBJC,DL_NOTIFY, ("CacheV add %x (dirty %d)!\n", dwVal, bDirty));

	if (ExpirationTimeShift != 0)
		pClient->m_CacheCl[CacheID].m_pCacheHead->m_CacheObj.m_ExpirationTime = GetTimeShift(ExpirationTimeShift);
	else
		pClient->m_CacheCl[CacheID].m_pCacheHead->m_CacheObj.m_ExpirationTime = 0;

	ReleaseResource(pClient->m_pCacheMutex);
	Obj_CacheCheck(pClient);
	
	return FALSE;
}

// Set new veridct for cache object
void
Obj_CacheUpdate(PQCLIENT pClient, DWORD dwVal, VERDICT Verdict, ULONG ExpirationTimeShift)
{
	PCACHE_OBJ pCacheObj;
	int CacheID = dwVal & _CL_CACHE_ACCESSMASK;
	
	if (pClient->m_CacheCl[CacheID].m_pCache == NULL || dwVal == _INVALID_VALUE)
	{
		DbPrint(DC_OBJC,DL_WARNING, ("Couldn't use cache update(%x-%x)!\n", 
			pClient->m_CacheCl[CacheID].m_pCache, dwVal));
		
		return;
	}
	
	AcquireResource(pClient->m_pCacheMutex, TRUE);

	pCacheObj = pClient->m_CacheCl[CacheID].m_pCacheHead;
	do
	{
		if (pCacheObj->m_CacheObj.m_dwVal == _INVALID_VALUE)
		{
			DbPrint(DC_OBJC,DL_INFO, ("CacheV miss!\n"));
			break;
		}
		if (pCacheObj->m_CacheObj.m_dwVal == dwVal)
		{
			DbPrint(DC_OBJC, DL_SPAM, ("Cache item %#x updated old->new !%d -> %d\n", dwVal, 
				pCacheObj->m_CacheObj.m_Verdict, Verdict));

			pCacheObj->m_CacheObj.m_bDirtyFlag = FALSE;
			pCacheObj->m_CacheObj.m_Verdict = Verdict;
			if (ExpirationTimeShift != 0)
				pCacheObj->m_CacheObj.m_ExpirationTime = GetTimeShift(ExpirationTimeShift);
			else
				pCacheObj->m_CacheObj.m_ExpirationTime = 0;
			if (pCacheObj != pClient->m_CacheCl[CacheID].m_pCacheHead)
			{
				pCacheObj->m_pPrev->m_pNext = pCacheObj->m_pNext;
				pCacheObj->m_pNext->m_pPrev = pCacheObj->m_pPrev;

				pCacheObj->m_pNext = pClient->m_CacheCl[CacheID].m_pCacheHead;
				pCacheObj->m_pPrev = pClient->m_CacheCl[CacheID].m_pCacheHead->m_pPrev;

				pCacheObj->m_pPrev->m_pNext = pCacheObj;
				pCacheObj->m_pNext->m_pPrev = pCacheObj;

				pClient->m_CacheCl[CacheID].m_pCacheHead = pCacheObj;
			}
			
			ReleaseResource(pClient->m_pCacheMutex);
			
			return;
		}

		pCacheObj = pCacheObj->m_pNext;
	} while (pCacheObj != pClient->m_CacheCl[CacheID].m_pCacheHead);

	ReleaseResource(pClient->m_pCacheMutex);

	DbPrint(DC_OBJC, DL_INFO, ("Couldn't update cacheitem - not found(%x)!\n", dwVal));
}


BOOLEAN
Obj_CacheDel(PQCLIENT pClient, DWORD dwVal)
{
	int CacheID;
	
	if (dwVal == _INVALID_VALUE)
		return FALSE;

	CacheID = dwVal & _CL_CACHE_ACCESSMASK;

	AcquireResource(pClient->m_pCacheMutex, TRUE);

	if (Obj_IsCached(pClient, dwVal, FALSE, NULL) != 0)
	{
		pClient->m_CacheCl[CacheID].m_pCacheHead->m_CacheObj.m_dwVal = _INVALID_VALUE;

		pClient->m_CacheCl[CacheID].m_pCacheHead = pClient->m_CacheCl[CacheID].m_pCacheHead->m_pNext;

		DbPrint(DC_OBJC,DL_INFO, ("CacheV reset %x!\n", dwVal));

		ReleaseResource(pClient->m_pCacheMutex);
		Obj_CacheCheck(pClient);

		return TRUE;
	}
	
	ReleaseResource(pClient->m_pCacheMutex);
	Obj_CacheCheck(pClient);
	
	return FALSE;
}

BOOLEAN Obj_CacheReset(PQCLIENT pClient)
{
	PCACHE_OBJ pCacheObj;
	int CacheID;
	
	if ((pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE) && (pClient->m_pCacheMutex != NULL))
	{
		AcquireResource(pClient->m_pCacheMutex, TRUE);

		DbPrint(DC_OBJC,DL_WARNING, ("Cleaning cache for client %d\n", pClient->m_AppID));

		for (CacheID = 0; CacheID < _CL_CACHE_QUEUES; CacheID++)
		{
			if (pClient->m_CacheCl[CacheID].m_pCacheHead != NULL)
			{
				pCacheObj = pClient->m_CacheCl[CacheID].m_pCacheHead;
				do
				{
					pCacheObj->m_CacheObj.m_dwVal = _INVALID_VALUE;
					pCacheObj = pCacheObj->m_pNext;
				} while (pCacheObj != pClient->m_CacheCl[CacheID].m_pCacheHead);
			}
		}
	
		ReleaseResource(pClient->m_pCacheMutex);
		Obj_CacheCheck(pClient);
		
		return TRUE;
	}
	else
		DbPrint(DC_OBJC,DL_ERROR, ("Failed to clean cache - structures correpted? %d\n", pClient->m_AppID));

	return FALSE;
}

// -----------------------------------------------------------------------------------------
// Get and lock client structures

void
CheckClientPauseFlag (
	PQCLIENT pClient
	)
{
	PQSUBCLIENT pSubTmp = GetFreeSubClient(pClient);
	if (pSubTmp)
		ReleaseSubClient(pSubTmp);
	else
		pClient->m_ClientFlags |= _CLIENT_FLAG_PAUSED;
}

BOOLEAN
IsExistClients()
{
	BOOLEAN bRet = FALSE;

	AcquireClientResource(FALSE);

	if (gpClients != NULL)
		bRet = TRUE;
	
	ReleaseClientResource();

	return bRet;
}

PQCLIENT
AcquireClient(ULONG AppID)
{
	PQCLIENT pClient;
	PQCLIENT pNextClient;

	if (AppID == _SYSTEM_APPLICATION)
		return NULL;

	AcquireClientResource(TRUE);

	pClient = NULL;
	pNextClient = gpClients;

	while((pNextClient != NULL) && (pClient == NULL))
	{
		if(pNextClient->m_AppID == AppID)
		{
			pClient = pNextClient;
			pClient->m_Enters++;
		}
		else
			pNextClient = pNextClient->m_pNext;
	}
	
	ReleaseClientResource();

	return pClient;
}

void
ReleaseClient(PQCLIENT pClient)
{
	AcquireClientResource(TRUE);

	if (pClient->m_Enters == 0)
	{
		DbPrint(DC_CLIENT,DL_ERROR, ("ClientAPI: Release not locked client!\n"));
		DbgBreakPoint();
	}
	else
		pClient->m_Enters--;

	ReleaseClientResource();
}

void
ReleaseClientAndSubClient(PQCLIENT pClient, PQSUBCLIENT pSubClient)
{
	if (pClient || pSubClient)
	{
		AcquireClientResource(TRUE);

		if (pClient)
		{
			if (pClient->m_Enters == 0)
			{
				DbPrint(DC_CLIENT,DL_ERROR, ("ClientAPI: Release not locked client!\n"));
				DbgBreakPoint();
			}
			else
				pClient->m_Enters--;
		}

		if (pSubClient)
		{
			if (pSubClient->m_Enters)
				pSubClient->m_Enters--;
			else
			{
				DbPrint(DC_CLIENT,DL_ERROR, ("ClientAPI: Release not locked sub client!\n"));
				DbgBreakPoint();

			}
		}

		ReleaseClientResource();
	}
}

BOOLEAN
IsExistClient(ULONG AppID)
{
	BOOLEAN bRet = FALSE;
	PQCLIENT pNextClient;
	
	AcquireClientResource(FALSE);

	pNextClient = gpClients;

	while((pNextClient != NULL) && (bRet == FALSE))
	{
		if(pNextClient->m_AppID == AppID)
			bRet = TRUE;
		else
			pNextClient = pNextClient->m_pNext;
	}
	
	ReleaseClientResource();

	return bRet;
}

NTSTATUS
ClientResetCache(ULONG AppID)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PQCLIENT pClient = AcquireClient(AppID);
	
	if (pClient != NULL)
	{
		if (Obj_CacheReset(pClient))
			Status = STATUS_SUCCESS;
		
		ReleaseClient(pClient);
	}
	else
		DbPrint(DC_CLIENT, DL_WARNING, ("IOCTLHOOK_ResetCache - client not found (%d)!\n", AppID));

	return Status;
}

ULONG
GetClientPriority(ULONG AppID)
{
	ULONG Priority = 0;
	PQCLIENT pClient = AcquireClient(AppID);
	if(pClient)
	{
		Priority = pClient->m_Priority;
		ReleaseClient(pClient);
	}

	return Priority;
}

// must be acquired by client mutex
BOOLEAN
IsSubClientConnected(PQSUBCLIENT pSubClient)
{
	if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
	{
		if (!(pSubClient->m_ActivityState & (_INTERNAL_APP_STATE_WATCHDOGPAUSED | _INTERNAL_APP_STATE_DEAD)))
			return TRUE;
	}

	return FALSE;
}

// must be acquired by client mutex
PQSUBCLIENT
GetCurrentSubClient(PQSUBCLIENT pSubClient)
{
	ULONG ProcessID = (ULONG) PsGetCurrentProcessId();
	ULONG ThreadID = (ULONG) PsGetCurrentThreadId();

#ifdef _HOOK_VXD_
	ProcessID = ProcessID ^ Obsfucator;
#endif 
	
	while (pSubClient != NULL)
	{
		if ((pSubClient->m_ProcessId == ProcessID) && (pSubClient->m_ThreadId == ThreadID))
			return pSubClient;
		
		pSubClient = pSubClient->m_pNext;
	}
	
	return NULL;
}

// must be acquired by client mutex
PQSUBCLIENT
GetConnectedSubClientByProc(PQSUBCLIENT pSubClient, ULONG ProcessID)
{
#ifdef _HOOK_VXD_
	ProcessID = ProcessID ^ Obsfucator;
#endif 
	while (pSubClient != NULL)
	{
		if ((pSubClient->m_ProcessId == ProcessID) && (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED))
			return pSubClient;
		
		pSubClient = pSubClient->m_pNext;
	}
	
	return NULL;
}

// must be acquired by client mutex
BOOLEAN
IsExistConnectedSubClient(PQSUBCLIENT pSubClient)
{
	if (pSubClient == NULL)
		return FALSE;

	while (pSubClient != NULL)
	{
		if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
			return TRUE;
		
		pSubClient = pSubClient->m_pNext;
	}

	return FALSE;
}
// must be acquired by client mutex
PQSUBCLIENT
GetSubClientByProcID(ULONG ProcID, PQCLIENT* ppClient)
{
	PQSUBCLIENT pSubClient;
	PQCLIENT pClient = gpClients;
	
	while(pClient)
	{
		pSubClient = pClient->m_pSubClient;
		while (pSubClient != NULL)
		{
			if (pSubClient->m_ProcessId == ProcID)
			{
				pClient->m_Enters++;
				*ppClient = pClient;
				return pSubClient;
			}

			pSubClient = pSubClient->m_pNext;
		}

		pClient = pClient->m_pNext;
	}

	return NULL;
}

// must be acquired by client mutex
PQSUBCLIENT
GetSubClientByKTHREAD(ULONG pkThread, ULONG* pAppId)
{
	PQSUBCLIENT pSubClient;
	PQCLIENT pClient = gpClients;
	
	while(pClient)
	{
		pSubClient = pClient->m_pSubClient;
		while (pSubClient != NULL)
		{
			if (pSubClient->m_pkThread == pkThread)
			{
				if (pAppId)
					*pAppId = pClient->m_AppID;
				return pSubClient;
			}

			pSubClient = pSubClient->m_pNext;
		}

		pClient = pClient->m_pNext;
	}

	return NULL;
}

// must be acquired by client mutex
PQSUBCLIENT
AddSubClient(PQCLIENT pClient)
{
	PQSUBCLIENT pSubClient = ExAllocatePoolWithTag(NonPagedPool, sizeof(QSUBCLIENT), 'SboS');
	
	if (pSubClient == NULL)
		return NULL;

	pSubClient->m_ActivityState = _INTERNAL_APP_STATE_NONE;
	
	pSubClient->m_pWhistleUp = NULL;
	pSubClient->m_pWhistlerChanged = NULL;
	
	pSubClient->m_pNext = pClient->m_pSubClient;
	pClient->m_pSubClient = pSubClient;

	pSubClient->m_Tasks = 0;

	pSubClient->m_ProcessId = _INVALID_PROCESS_VALUE;
	pSubClient->m_ThreadId = _INVALID_THREAD_VALUE;

	#ifdef _HOOK_NT_ 
		pSubClient->m_pkThread = (ULONG) PsGetCurrentThread();
	#else
		pSubClient->m_pkThread = 0;
	#endif

	pSubClient->m_SuspendCount = 0;

	pSubClient->m_Enters = 0;
	
	return pSubClient;
}

void SuspendSubClient(ULONG pkThread)
{
	ULONG AppId = _SYSTEM_APPLICATION;
	PQSUBCLIENT pSubClient;

	AcquireClientResource(TRUE);

	pSubClient = GetSubClientByKTHREAD(pkThread, &AppId);

	if (pSubClient)
		pSubClient->m_SuspendCount++;

	ReleaseClientResource();
	
	if (_SYSTEM_APPLICATION != AppId)
		SkipEventForClient(AppId);
}

void ResumeSubClient(ULONG pkThread)
{
	PQSUBCLIENT pSubClient;

	AcquireClientResource(TRUE);

	pSubClient = GetSubClientByKTHREAD(pkThread, NULL);
	if (pSubClient)
	{
		if (pSubClient->m_SuspendCount)
			pSubClient->m_SuspendCount--;
	}

	ReleaseClientResource();
}

PQSUBCLIENT
GetFreeSubClient(PQCLIENT pClient)
{
	PQSUBCLIENT pSubFree = NULL;
	PQSUBCLIENT pSubClient;

	AcquireClientResource(TRUE);

	pSubClient = pClient->m_pSubClient;

	if (pSubClient == NULL)
	{
		ReleaseClientResource();
		return NULL;
	}

	do
	{
		if (!(pSubClient->m_ActivityState & _INTERNAL_APP_STATE_WATCHDOGPAUSED))
		{
			if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_ACTIVE)
			{
				if (pSubFree == NULL)
					pSubFree = pSubClient;
				else
				{
					if (pSubFree->m_Tasks > pSubClient->m_Tasks)
						pSubFree = pSubClient;
				}

				if (pSubFree->m_SuspendCount)
					pSubFree = NULL;
				else if (!pSubFree->m_Tasks)
					break;
			}
		}

		pSubClient = pSubClient->m_pNext;


	} while (pSubClient != NULL);

	if (pSubFree)
		pSubFree->m_Enters++;

	ReleaseClientResource();

	return pSubFree;
}

void
ReleaseSubClient(PQSUBCLIENT pSubClient)
{
	AcquireClientResource(TRUE);

	if (pSubClient->m_Enters)
		pSubClient->m_Enters--;
	else
	{
		DbgBreakPoint();
	}

	ReleaseClientResource();
}

void
InitClientStruct(PQCLIENT pClient, ULONG AppID)
{
	gActiveClientCounter++;

	pClient->m_AppID = AppID;
	
	pClient->m_Enters = 1;					// for new client
	pClient->m_FiltersCount = 0;
	pClient->m_ClientsSafeValue = 0;		// default client is safe

	pClient->m_CacheSize = 0;
	
	pClient->m_DefaultVerdict = Verdict_Default;

	pClient->m_pSubClient = NULL;
	pClient->m_pNext = NULL;
}

void
ClientSetFlowTimeout(PFLOWTIMEOUT pFlow)
{
	PQCLIENT pClient = AcquireClient(pFlow->m_AppID);
	if (pClient != NULL)
	{
		//SetClientFlowTimeout(pClient);
		ReleaseClient(pClient);
	}
	else
		DbPrint(DC_CLIENT, DL_WARNING, ("IOCTLHOOK_SetFlowTimeout - client not found(%d)!\n", pFlow->m_AppID));
}


BOOLEAN
RegisterApplication(PAPP_REGISTRATION pReg, PCLIENT_INFO pClientInfo)
{
	PQCLIENT pClient;
	PQSUBCLIENT pSubClient;

#ifdef _HOOK_NT_ 
	FidBox_Allow();
#endif

	if (pReg->m_InterfaceVersion != 0)
	{
		if (pReg->m_InterfaceVersion != HOOK_INTERFACE_NUM)
			return FALSE;
	}
	
	if (pReg->m_AppID == _SYSTEM_APPLICATION)
		return FALSE;

	if (pReg->m_Priority == 0)
		return FALSE;

#ifdef _HOOK_NT_ 
	else
	{
		if (pReg->m_AppID == AVPG_MPraguePlugin)
			gFSNeedRescan++;
	}
#endif

	ActivityWatchDogCheck();	// for free dead or unloaded client

	AcquireClientResource(TRUE);

	if (InstallerProtection == pReg->m_AppID)
	{
		if (pReg->m_SyncAddr != (ULONG) PsGetCurrentProcessId())
		{
			ReleaseClientResource();

			DbgBreakPoint();
			
			return FALSE;
		}
	}

	if (pReg->m_AppID == AVPG_Driver_Specific)
	{
		pReg->m_ClientFlags |=_CLIENT_FLAG_POPUP_TYPE;
		pReg->m_ClientFlags &= (~_CLIENT_FLAG_SAVE_FILTERS);	// disable flag SAVE_TO_REGESTRY
		pReg->m_AppID &= (~_INTERNAL_APP_STATE_CONNECTED);
		DbPrint(DC_CLIENT,DL_NOTIFY, ("RegisterApplication: Driver specific\n"));
		while (pReg->m_AppID == AVPG_Driver_Specific)
		{
			pReg->m_AppID = gDriverSpecificID;
			pClient = AcquireClient(gDriverSpecificID);
			if (pClient != NULL)
			{
				ReleaseClient(pClient);
				pReg->m_AppID = AVPG_Driver_Specific;
			}
			else
				DbPrint(DC_CLIENT,DL_NOTIFY, ("RegisterApplication: founded id %d\n", pReg->m_AppID));

			gDriverSpecificID++;
			if (gDriverSpecificID == AVPG_Driver_Specific)
				gDriverSpecificID = _LID;
		}
	}
	else
	{
		pClient = AcquireClient(pReg->m_AppID);
	}
	
	if(pClient == NULL)
	{ 
		// we did not read this client from registry
		if(!(pClient = ExAllocatePoolWithTag(NonPagedPool, sizeof(QCLIENT), 'CboS')))
		{
			DbPrint(DC_CLIENT,DL_ERROR, ("RegisterApplication failed (no memory).\n"));
		
			ReleaseClientResource();
			return FALSE;
		}
		pClient->m_Priority = pReg->m_Priority;
		if (InitEventList(&pClient->m_EventList) == FALSE)
		{
			DbPrint(DC_CLIENT,DL_ERROR, ("RegisterApplication failed (event queue init failed).\n"));
			ExFreePool(pClient);
		
			ReleaseClientResource();
			return FALSE;
		}

		if (pReg->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
		{
			pClient->m_pCacheMutex = InitKlgResourse();

			if (pClient->m_pCacheMutex == NULL)
			{
				DbPrint(DC_CLIENT,DL_ERROR, ("RegisterApplication failed (cache init failed).\n"));
				DoneEventList(&pClient->m_EventList);
				ExFreePool(pClient);
			
				ReleaseClientResource();
				return FALSE;
			}
		}
		else
			pClient->m_pCacheMutex = NULL;

		InitClientStruct(pClient, pReg->m_AppID);

		pSubClient = AddSubClient(pClient);

		pClient->m_pNext = gpClients;
		gpClients = pClient;
	}
	else
	{
		if (!(pClient->m_ClientFlags & _CLIENT_FLAG_PARALLEL))
		{
			PQSUBCLIENT pSubClient = pClient->m_pSubClient;
			while (pSubClient)
			{
				if (!(pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED))
				{
					ReleaseClient(pClient);
					DbPrint(DC_CLIENT, DL_WARNING, ("App %d already registered\n", pReg->m_AppID));
					
					ReleaseClientResource();
					return FALSE;
				}
				
				pSubClient = pSubClient->m_pNext;
			}
		}
		
		if (pReg->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
		{
			if (pClient->m_CacheSize != pReg->m_CacheSize)
			{
				DbPrint(DC_CLIENT, DL_IMPORTANT, ("App %d on register - change cache size (cache disabled)\n", 
					pReg->m_AppID));
				
				Obj_CacheFree(pClient);
			}
		}
		
		pSubClient = AddSubClient(pClient);
	}
// ------ 
	pClient->m_ClientFlags = pReg->m_ClientFlags;
	CheckClientPauseFlag( pClient );

	// -----------------------------------------------------------------------------------------
	// 9x processing
#ifdef _HOOK_VXD_
	if(pWin16Mutex == NULL)
		pWin16Mutex=(PWIN16MUTEX)pReg->m_SyncAddr;
#endif
	SetObsfucator(pReg->m_CurProcId);
	// -----------------------------------------------------------------------------------------

	if (pSubClient != NULL)
	{
		pSubClient->m_ActivityWatchDog = ACTIVITY_WATCHDOG_TIMEOUT;
		pSubClient->m_BlueScreenTimeout = pReg->m_BlueScreenTimeout;
		pSubClient->m_ProcessId = pReg->m_CurProcId;
		pSubClient->m_ThreadId = (ULONG)PsGetCurrentThreadId();

		pSubClient->m_pWhistleUp = InitKlgEvent(pReg->m_WhistleUp);
		pSubClient->m_pWhistlerChanged = InitKlgEvent(pReg->m_WFChanged);

		if ((pSubClient->m_pWhistleUp != NULL) && (pSubClient->m_pWhistlerChanged != NULL))
		{
			pSubClient->m_ActivityState |= _INTERNAL_APP_STATE_CONNECTED;
			
			DbPrint(DC_CLIENT,DL_NOTIFY, ("Application %d pRegistered. gActiveClientCounter=%d (ProcID %x)\n", 
				pReg->m_AppID, gActiveClientCounter, pReg->m_CurProcId));
		}
		else
		{
			DbPrint(DC_CLIENT,DL_ERROR, ("Wistleuppers opening error\n"));
			DoneKlgEvent(&pSubClient->m_pWhistleUp);
			DoneKlgEvent(&pSubClient->m_pWhistlerChanged);
			
			pSubClient->m_pWhistleUp = NULL;
			pSubClient->m_pWhistlerChanged = NULL;
		}
	}

	if (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
	{
		if (!pClient->m_CacheSize)
			Obj_CacheInit(pClient, pReg->m_CacheSize);
	}
// ------ 
	pClientInfo->m_ClientFlags = pClient->m_ClientFlags;
	pClientInfo->m_Priority = pClient->m_Priority;
	pClientInfo->m_CacheSize = pClient->m_CacheSize;
	pClientInfo->m_AppID = pClient->m_AppID;

	ReleaseClient(pClient);
	
	ReleaseClientResource();

	OutputClientsInfo();

	return TRUE;
}

BOOLEAN
RegisterAppFromSav(ULONG AppID, PCLIENT_SAV pClientSav)
{
	PQCLIENT pClient;

	AcquireClientResource(TRUE);

	pClient = AcquireClient(AppID);
	if (pClient != NULL)
	{
		DbPrint(DC_CLIENT,DL_ERROR, ("Restore from save failed - client %d already registered!\n", AppID));
		ReleaseClient(pClient);
		ReleaseClientResource();

		return FALSE;
	}

	if(!(pClient = ExAllocatePoolWithTag(NonPagedPool, sizeof(QCLIENT), 'CboS')))
	{
		DbPrint(DC_CLIENT,DL_ERROR, ("RegisterApplication failed (no memory).\n"));
		ReleaseClientResource();
		
		return FALSE;
	}

	InitClientStruct(pClient, AppID);
	
	if (InitEventList(&pClient->m_EventList) == FALSE)
	{
		DbPrint(DC_CLIENT,DL_ERROR, ("RegisterApplication failed (event queue init failed).\n"));
	
		ReleaseClientResource();
		return FALSE;
	}

	if (pClientSav->ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
	{
		pClient->m_pCacheMutex = InitKlgResourse();

		if (pClient->m_pCacheMutex == NULL)
		{
			DbPrint(DC_CLIENT,DL_ERROR, ("RegisterApplication failed (cache init failed).\n"));
			DoneEventList(&pClient->m_EventList);
			ExFreePool(pClient);
		
			ReleaseClientResource();

			return FALSE;
		}
	}
	else
		pClient->m_pCacheMutex = NULL;

	pClient->m_Priority = pClientSav->Priority;
	pClient->m_ClientFlags = pClientSav->ClientFlags;

	if (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
		Obj_CacheInit(pClient, pClientSav->CacheSize);

	if (!(pClient->m_ClientFlags & _CLIENT_FLAG_PAUSEONREGISTER)) 
		pClient->m_ClientFlags &= ~_CLIENT_FLAG_PAUSED;
	else
	{
		pClient->m_ClientFlags |= _CLIENT_FLAG_PAUSED;
		DbgBreakPoint();
	}

	pClient->m_pNext = gpClients;
	gpClients = pClient;

	ReleaseClientResource();

	ReleaseClient(pClient);

	DbPrint(DC_CLIENT,DL_IMPORTANT, ("Application %d pRegistered from Save. gActiveClientCounter=%d\n", 
		AppID, gActiveClientCounter));

	return TRUE;
}

BOOLEAN
DisconnectSubClient(PQSUBCLIENT pSubClient, BOOLEAN bDead)
{
	if (!(pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED))
		return FALSE;

	DbPrint(DC_CLIENT,DL_IMPORTANT, ("Sub %#x disconnecting...\n", pSubClient->m_ProcessId));
	
	pSubClient->m_ActivityState &= (~_INTERNAL_APP_STATE_CONNECTED);
	pSubClient->m_ActivityState &= (~_INTERNAL_APP_STATE_ACTIVE);

	pSubClient->m_ProcessId = _INVALID_PROCESS_VALUE;
	pSubClient->m_ThreadId = _INVALID_THREAD_VALUE;

	pSubClient->m_pkThread = 0;

	if (bDead == TRUE)
	{
		pSubClient->m_ActivityState |= _INTERNAL_APP_STATE_DEAD;

		pSubClient->m_pWhistleUp->hWhistle = NULL;					// not valid handle for dead application
		pSubClient->m_pWhistlerChanged->hWhistle = NULL;			// see line upper

		DoneKlgEvent(&(pSubClient->m_pWhistleUp));
		DoneKlgEvent(&(pSubClient->m_pWhistlerChanged));
	}
	else
	{
		DoneKlgEvent(&(pSubClient->m_pWhistleUp));
		DoneKlgEvent(&(pSubClient->m_pWhistlerChanged));
	}

	DbPrint(DC_CLIENT,DL_IMPORTANT, ("gActiveClientCounter = %d\n", gActiveClientCounter));

	return TRUE;
}

void
DisconnectClientByProcID(ULONG ProcID)
{
	PQCLIENT pClient;
	PQCLIENT pClientSave;
	PQSUBCLIENT pSubClient;
	ULONG SubProcessId;

	BOOLEAN bExit = FALSE;

	BOOLEAN bWasSub = FALSE;

	pClientSave = NULL;

	while(!bExit)
	{
		pClient = NULL;
		
		AcquireClientResource(TRUE);

		pSubClient = GetSubClientByProcID(ProcID, &pClient);
		if (!pSubClient)
			bExit = TRUE;
		else
		{
			DbPrint(DC_CLIENT, DL_IMPORTANT, ("Client %d disconnect by ProcessID %x\n", pClient->m_AppID, ProcID));

			SubProcessId = pSubClient->m_ProcessId;			
			DisconnectSubClient(pSubClient, TRUE);

			bWasSub = TRUE;
		}

		ReleaseClientResource();

		if (pClient)
		{
			DbPrint(DC_CLIENT,DL_IMPORTANT, ("Client %d release events marked by sub %#x...\n", pClient->m_AppID, SubProcessId));

			ReleaseMarkedEvents(pClient->m_AppID, &pClient->m_EventList, SubProcessId);

			if (pClientSave != pClient)
			{
				pClientSave = pClient;
		  		SaveFilters(pClient);

				if (!bWasSub)
					ReleaseEvents(pClient->m_AppID, &pClient->m_EventList);

				bWasSub = FALSE;
			}

			ReleaseClient(pClient);
		}
	}
}

void DisconnectClientByThreadID(ULONG ThreadID)
{
	PQCLIENT pClientTmp;
	PQCLIENT pClient;
	PQSUBCLIENT pSubClient;

	AcquireClientResource(TRUE);
	
	pClient = gpClients;
	
	while(pClient)
	{
		pClientTmp = pClient->m_pNext;

		pSubClient = pClient->m_pSubClient;
		while (pSubClient != NULL)
		{
			if (pSubClient->m_ThreadId == ThreadID)
			{
				DbPrint(DC_CLIENT,DL_WARNING, ("Client %d - ThreadID %x exit\n", pClient->m_AppID, ThreadID));
				pSubClient->m_ThreadId = _INVALID_THREAD_VALUE;
			}

			pSubClient = pSubClient->m_pNext;
		}
		pClient = pClientTmp;
	}

	ReleaseClientResource();
}

BOOLEAN ClientActivity(PHDSTATE In, PHDSTATE Out)
{
	BOOLEAN bRet = FALSE;
	PQCLIENT pClient = AcquireClient(In->AppID);
	PQSUBCLIENT pSubClient;

	BOOLEAN bReleaseLock = FALSE;

	ULONG Activity = In->Activity;

	if(!pClient)
	{
		DbPrint(DC_CLIENT, DL_WARNING, ("ClientActivity failed. AppID %#x not registered\n", In->AppID));
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	if ((In->AppID == _SYSTEM_APPLICATION) && (In->Activity == -1))
	{
		ULONG cou;
		
		ReleaseClient(pClient);
		pClient = NULL;

		DbgBreakPoint();
		
		for (cou = 1; cou < 65535; cou++)
			DisconnectClientByProcID(cou);
		
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////



	AcquireClientResource(TRUE);
	bReleaseLock = TRUE;

	pSubClient = GetCurrentSubClient(pClient->m_pSubClient);

	if ((pSubClient == NULL) || (!(pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)))
	{
		DbPrint(DC_CLIENT, DL_SPAM, ("Forbidden activity by App %d - not found for proc: %#x thr: %#x.\n\tSearch by process...\n", 
			In->AppID, (ULONG) PsGetCurrentProcessId(), (ULONG) PsGetCurrentThreadId()));
		
		pSubClient = GetConnectedSubClientByProc(pClient->m_pSubClient, (ULONG) PsGetCurrentProcessId());
		if (pSubClient == NULL)
		{
			DbPrint(DC_CLIENT, DL_WARNING, ("ClientActivity. Forbidden activity by App %d - client not found\n", 
				In->AppID));
			
			DbgBreakPoint();
		}
		else
		{
			DbPrint(DC_CLIENT, DL_INFO, ("ClientActivity. activity by App %d - client thread changed %#x (%d)\n", 
				In->AppID, pSubClient->m_ThreadId, pSubClient->m_ThreadId));
		}
	}
	//! unsafe usage pSubClient
	if (pSubClient != NULL)
	{
		bRet = TRUE;

		if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_WATCHDOGPAUSED)
		{
			DbPrint(DC_DOG, DL_WARNING, ("SubClient %d is in active mode now! (after semidead)\n", pClient->m_AppID));
			pSubClient->m_ActivityState &= ~_INTERNAL_APP_STATE_WATCHDOGPAUSED;
		}

		switch(In->Activity)
		{
		case _AS_SaveFilters:
			bReleaseLock = FALSE;
			ReleaseClientResource();
			DbPrint(DC_CLIENT,DL_NOTIFY, ("Save filters for client %d\n",In->AppID));
			SaveFilters(pClient);
			break;
		case _AS_DontChange:
			pSubClient->m_ActivityWatchDog = ACTIVITY_WATCHDOG_TIMEOUT;
			break;
		case _AS_GoSleep:
			DbPrint(DC_CLIENT,DL_NOTIFY, ("Try to change ClientActivity %d - %s\n",In->AppID, "Sleep"));
			
			if (InstallerProtection == pClient->m_AppID) // special case - pause all
			{
				PQSUBCLIENT pSubClient;
				pSubClient = pClient->m_pSubClient;

				while (pSubClient)
				{
					pSubClient->m_ActivityState &= ~_INTERNAL_APP_STATE_ACTIVE;
					pSubClient = pSubClient->m_pNext;
				}
			}
			else
			{
				if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_ACTIVE)
				{
					pSubClient->m_ActivityState &= ~_INTERNAL_APP_STATE_ACTIVE;
					DbPrint(DC_CLIENT,DL_NOTIFY, ("State changed for App %d. Client now in sleep mode\n", In->AppID));
				}
				else
				{
					DbPrint(DC_CLIENT,DL_NOTIFY, ("ClientActivity. App %d - client already in sleep mode\n",In->AppID));
					bRet = FALSE;
				}
			}
			
			CheckClientPauseFlag( pClient );
			
			break;
		case _AS_GoActive:
			DbPrint(DC_CLIENT,DL_NOTIFY, ("Try to change ClientActivity %d - %s\n", In->AppID, "Active"));
			if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_ACTIVE) {
				DbPrint(DC_CLIENT, DL_WARNING, ("State didn't changed: Client %d already in active mode\n",In->AppID));
				bRet = FALSE;
			}
			else
			{
				pSubClient->m_ActivityState |= _INTERNAL_APP_STATE_ACTIVE;
				pClient->m_ClientFlags &= ~_CLIENT_FLAG_PAUSED;
				DbPrint(DC_CLIENT,DL_NOTIFY, ("ClientActivity. Client %d now in active mode\n", In->AppID));
			}
			break;
		case _AS_Unload:
			if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
			{
				pSubClient->m_ActivityState &= (~_INTERNAL_APP_STATE_ACTIVE);
			
				DisconnectSubClient(pSubClient, FALSE);

				DbPrint(DC_CLIENT,DL_IMPORTANT, ("ClientActivity %d - Unload. gActiveClientCounter=%d\n",
					In->AppID, gActiveClientCounter));
			}
			else
			{
				DbPrint(DC_CLIENT, DL_WARNING, ("ClientActivity %d failed. Must be registered! gActiveClientCounter=%d\n", 
					In->AppID, gActiveClientCounter));
				bRet = FALSE;
			}
			break;
		case _AS_UnRegisterStayResident:
			if (pClient->m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE)
			{
				DbPrint(DC_CLIENT, DL_WARNING, ("ClientActivity %d Unload failed (popup application). ClientgActiveClientCounter=%d\n", 
					In->AppID, gActiveClientCounter));
				In->Activity = _AS_Unload;
				bRet = ClientActivity(In, Out);
			}
			else
			{
				if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
				{
					DisconnectSubClient(pSubClient, FALSE);

					DbPrint(DC_CLIENT,DL_IMPORTANT, ("ClientActivity %d - Unload. gActiveClientCounter=%d\n",
						In->AppID,gActiveClientCounter));
				}
				else
				{
					DbPrint(DC_CLIENT, DL_WARNING, ("ClientActivity %d failed. Must be connected! gActiveClientCounter=%d\n",
						In->AppID,gActiveClientCounter));
					bRet = FALSE;
				}
			}
			break;
		case _AS_IncreaseUnsafe:
			InterlockedIncrement(&pClient->m_ClientsSafeValue);
			DbPrint(DC_CLIENT,DL_INFO, ("ClientActivity %d - increase unsafe to %d\n",
				In->AppID, pClient->m_ClientsSafeValue));
			break;
		case _AS_DecreaseUnsafe:
			if(pClient->m_ClientsSafeValue == 0)
			{
				DbPrint(DC_CLIENT, DL_WARNING, ("ClientActivity %d - decrease unsafe failed! Already zero!!!\n",
					In->AppID));
			}
			else
				InterlockedDecrement(&pClient->m_ClientsSafeValue);
			DbPrint(DC_CLIENT,DL_INFO, ("ClientActivity %d - decrease unsafe to %d\n", In->AppID, 
				pClient->m_ClientsSafeValue));
			break;
		case _AS_SetPauseOnStart:
			DbPrint(DC_CLIENT, DL_INFO, ("ClientActivity %d - restore fromregestry in pause mode\n", In->AppID));
			pClient->m_ClientFlags |= _CLIENT_FLAG_PAUSEONREGISTER;
			break;
		case _AS_ResetPauseOnStart:
			DbPrint(DC_CLIENT, DL_INFO, ("ClientActivity %d - restore from regestry in active mode\n", In->AppID));
			pClient->m_ClientFlags &= ~_CLIENT_FLAG_PAUSEONREGISTER;
			break;
		}

		if ((In->Activity != _AS_IncreaseUnsafe) && (In->Activity != _AS_DecreaseUnsafe))
		{
			if (pSubClient->m_ActivityState & _INTERNAL_APP_STATE_ACTIVE)
				Out->Activity = _AS_Active;
			else
				Out->Activity = _AS_Sleep;
		}
		else
			Out->Activity = pClient->m_ClientsSafeValue;
		
		Out->AppID = In->AppID;
		Out->QLen = pClient->m_EventList.m_QLen;
		Out->QUnmarkedLen = pClient->m_EventList.m_QUnmarkedLen;

		if (Out->QLen == 0) // increased task count but not performed by client
			pSubClient->m_Tasks = 0;
	}

	if (bReleaseLock)
		ReleaseClientResource();

	if (_AS_GoActive == Activity)
		Di_PauseClient(pClient->m_AppID, FALSE);
	else if (_AS_GoSleep == Activity)
		Di_PauseClient(pClient->m_AppID, TRUE);

	ReleaseClient(pClient);

#ifdef __DBG__
	if ((Activity != _AS_IncreaseUnsafe) && (Activity != _AS_DecreaseUnsafe) && (Activity!= _AS_DontChange))
		OutputClientsInfo();
#endif

	return bRet;
}

// -----------------------------------------------------------------------------------------

BOOLEAN
FreeUnusedClients(void)
{
	BOOLEAN bWasFree = FALSE;
	PQCLIENT pClientTmp;
	PQCLIENT pClient;
	PQCLIENT pClientPrev;

	PQCLIENT pClientFreeFilters = NULL;
	PQCLIENT pClientEvents = NULL;

	AcquireClientResource(TRUE);
	
	pClient = gpClients;
	pClientPrev = NULL;

	//!
	while(pClient)
	{
		pClientTmp = pClient->m_pNext;

		if (IsExistConnectedSubClient(pClient->m_pSubClient) == FALSE)
		{
			if (pClient->m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE)
			{
				if (pClient->m_EventList.m_QLen)
				{
					DbgBreakPoint();
					pClientEvents = pClient;
				}

				if (pClient->m_FiltersCount)
				{
					if (pClientFreeFilters == NULL)
					{
						DbPrint(DC_CLIENT, DL_WARNING, ("Client %d - will clear own filters\n", pClient->m_AppID));
						pClientFreeFilters = pClient;
					}
				}
			}
		}
			
		if (pClient->m_pSubClient)
		{
			// deleting subclients
			PQSUBCLIENT pSubClientPrev = NULL;
			PQSUBCLIENT pSubClient = pClient->m_pSubClient;
		
			while (pSubClient != NULL)
			{
				if ((pSubClient->m_ActivityState & _INTERNAL_APP_STATE_DEAD) || 
					(pSubClient->m_ActivityState == _INTERNAL_APP_STATE_NONE))
				{
					if (pSubClient->m_Enters > 0)
					{
						DbPrint(DC_CLIENT, DL_WARNING, ("SubClient locked %d times on free\n", pSubClient->m_Enters));

						pSubClientPrev =  pSubClient;
						pSubClient = pSubClient->m_pNext;
					}
					else
					{
						PQSUBCLIENT SubTmp = pSubClient->m_pNext;
						
						DbPrint(DC_CLIENT, DL_WARNING, ("Client %d - free dead sub client %#x\n", 
							pClient->m_AppID, pSubClient->m_ProcessId));

						DoneKlgEvent(&(pSubClient->m_pWhistleUp));
						DoneKlgEvent(&(pSubClient->m_pWhistlerChanged));

						ExFreePool(pSubClient);
						
						if (pSubClientPrev == NULL)
							pClient->m_pSubClient = SubTmp;
						else
							pSubClientPrev->m_pNext = SubTmp;

						pSubClient = SubTmp;
					}
				}
				else
				{
					pSubClientPrev =  pSubClient;
					pSubClient = pSubClient->m_pNext;
				}
			}
		}
		
		if (!pClient->m_pSubClient && (pClient != pClientFreeFilters))
		{
			if (!pClient->m_Enters && (pClient->m_ClientFlags & _CLIENT_FLAG_POPUP_TYPE))
			{
				if (pClient->m_EventList.m_QLen)
					pClientEvents = pClient;
				else
				{
					bWasFree = TRUE;
					DbPrint(DC_CLIENT, DL_WARNING, ("Unused client found = %d \n", pClient->m_AppID));

					DoneEventList(&pClient->m_EventList);
					
					if (pClient == gpClients)
						gpClients = gpClients->m_pNext;
					else
						pClientPrev->m_pNext = pClient->m_pNext;

					if (pClient->m_ClientFlags & _CLIENT_FLAG_USE_DRIVER_CACHE)
					{
						Obj_CacheFree(pClient);
						FreeKlgResourse(&pClient->m_pCacheMutex);
					}
					
					ExFreePool(pClient);
					pClient = NULL;
					
					gActiveClientCounter--;
				}
			}
		}
		
		if (pClient != NULL)
			pClientPrev = pClient;
		
		pClient = pClientTmp;
	}

	if (pClientEvents)
		pClientEvents->m_Enters++;

	if (pClientFreeFilters)
		pClientFreeFilters->m_Enters++;

	ReleaseClientResource();

	//+ ------------------------------------------------------------------------------------------
	
	if (pClientEvents)
		ReleaseEvents(pClientEvents->m_AppID, &pClientEvents->m_EventList);
	
	if (pClientFreeFilters != NULL)
		_FreeAppFilters(pClientFreeFilters);

	if (pClientEvents || pClientFreeFilters)
	{
		AcquireClientResource(TRUE);
		if (pClientEvents)
			pClientEvents->m_Enters--;

		if (pClientFreeFilters)
			pClientFreeFilters->m_Enters--;
	
		ReleaseClientResource();
	}

	return bWasFree;
}

// ----  ActivityWatchDog  -------------------------------------------------------------------------------------

#ifdef __DBG__
ULONG gCounter = 0;
#endif

VOID ActivityWatchDogCheck(VOID)
{
	PQCLIENT pClient;
	PQSUBCLIENT pSubClient;
	
	DbPrint(DC_DOG,DL_INFO, ("ActivityWatchDog check. gActiveClientCounter=%d \n", gActiveClientCounter));
	
	AcquireClientResource(TRUE);

	pClient = gpClients;
	while(pClient != NULL)
	{
		pSubClient = pClient->m_pSubClient;
		while (pSubClient != NULL)
		{
			if(pSubClient->m_ActivityState & _INTERNAL_APP_STATE_CONNECTED)
			{
				if (!(pClient->m_ClientFlags & _CLIENT_FLAG_WITHOUTWATCHDOG))
				{
					pSubClient->m_ActivityWatchDog -= _WatchInterval;
					if(pSubClient->m_ActivityWatchDog < 0) {
						
						if (pClient->m_ClientFlags & _CLIENT_FLAG_WATCHDOG_USEPAUSE)
						{
							pSubClient->m_ActivityState |= _INTERNAL_APP_STATE_WATCHDOGPAUSED;
							DbPrint(DC_DOG,DL_WARNING, ("SubClient %d is semidead. waiting activity\n", pClient->m_AppID));
							pSubClient->m_ActivityWatchDog = _WatchInterval * 3;
						}
						else
						{
							DbPrint(DC_DOG,DL_WARNING, ("Client %d is dead\n", pClient->m_AppID));
							
							DisconnectSubClient(pSubClient, TRUE);
						}
					}
				}
			}

			pSubClient = pSubClient->m_pNext;
		}
		pClient = pClient->m_pNext;
	}
	
	ReleaseClientResource();
	
	if (FreeUnusedClients())
		FreeUnusedClients();
#ifdef __DBG__
	gCounter++;
	if ((gCounter % 30) == 0)
	{
		if (OutputClientsInfo())
		{
			DbPrint(DC_DOG,DL_IMPORTANT, ("DeadLock count %d\n", gDeadLocks));
			DbPrint(DC_DOG,DL_IMPORTANT, ("ReplyWaiter enters %d\n", gReplyWaiterEnters));
		}
	}
#endif
}

BOOLEAN AWDogStop=FALSE;

#ifdef _HOOK_NT_ 
PVOID		ActivityWatchDogThread = NULL;

void
TSP_LoadCheck();

void
SysMon_Check();

// Gruzdev
VOID ShadowTable_Init();
VOID ShadowTable_Check();

VOID ActivityWatchDog(PVOID Context)
{
	LARGE_INTEGER	Interval;

	DbPrint(DC_DOG,DL_IMPORTANT, ("ActivityWatchDog loop...\n"));
	
	//	Interval=LiNeg(LiNMul(_WatchInterval,10000000UL)); //LiNeg & LiNMul absent in whistler DDK
	*(__int64*)&Interval=-((__int64)_WatchInterval*10000000L);
	DbPrint(DC_DOG,DL_NOTIFY, ("ActivityWatchDog started\n"));
	while(!AWDogStop)
	{
		ActivityWatchDogCheck();
		ProcessDelayedQueue();
		FS_CheckNewDevices();
		TSP_LoadCheck();
		
		SysMon_Check();
// Gruzdev
		ShadowTable_Init();
		ShadowTable_Check();

		KeDelayExecutionThread(KernelMode,FALSE,&Interval);
	}
	PsTerminateSystemThread(STATUS_SUCCESS);
}
#endif //_HOOK_NT_

#ifdef _HOOK_VXD_ 
TIMEOUTHANDLE hActivityWatchDog = 0;
TIMEOUT_THUNK   pAWDTimeout_thunk;

VOID __stdcall OnAWDTimeout(VMHANDLE hVM,PCLIENT_STRUCT pcrs,VOID *refdata,DWORD Extra)
{
	if(!AWDogStop)
	{
		ActivityWatchDogCheck();
		ProcessDelayedQueue();
		hActivityWatchDog=Set_Global_Time_Out(_WatchInterval*1000,0,OnAWDTimeout,&pAWDTimeout_thunk);
	}
}
#endif // _HOOK_VXD_ 

// located in "INIT" section, look for #pragma alloc_text("INIT",...)
NTSTATUS StartAWDog(VOID) 
{
#ifdef _HOOK_NT_ 
	HANDLE	hAWDogThread=0;
	NTSTATUS	ntStatus;
	// Run Activity WatchDog thread
	if((ntStatus = PsCreateSystemThread(&hAWDogThread, THREAD_ALL_ACCESS, NULL, 0, NULL, ActivityWatchDog, NULL)) != STATUS_SUCCESS)
		return ntStatus;

	DbPrint(DC_DOG,DL_IMPORTANT, ("ActivityWatchDog started\n"));	

	ntStatus = ObReferenceObjectByHandle(hAWDogThread, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, 
		(VOID**)&ActivityWatchDogThread, NULL);

	if(hAWDogThread)
		ZwClose(hAWDogThread);

	if(!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_DOG,DL_ERROR, ("Reference ActivityWatchDogThread failed, status=%x\n",ntStatus));
		return ntStatus;
	}
#else
	if(!(hActivityWatchDog = Set_Global_Time_Out(_WatchInterval*1000,0,OnAWDTimeout,&pAWDTimeout_thunk)))
		return STATUS_UNSUCCESSFUL;
#endif //_HOOK_NT_
	
	return STATUS_SUCCESS;
}

VOID StopAWDog(VOID)
{
	AWDogStop = TRUE;
#ifdef _HOOK_NT_ 
	if(ActivityWatchDogThread)
	{
		KeWaitForSingleObject(ActivityWatchDogThread,Executive,KernelMode,FALSE,NULL);
		ObDereferenceObject(ActivityWatchDogThread);
	}
#else
	if(hActivityWatchDog)
		Cancel_Time_Out(hActivityWatchDog);
	hActivityWatchDog=0;
#endif //_HOOK_NT_
	DbPrint(DC_DOG,DL_NOTIFY, ("ActivityWatchDog stopped\n"));
}

void ReleasePopupEventsForProcess(ULONG ProcessId, ULONG HookId)
{
	PQCLIENT pClient = AcquireClient(KL_PDM);
	if (pClient != NULL)
	{
		ReleasePopupEventsByProcess(&pClient->m_EventList, ProcessId, HookId);

		ReleaseClient(pClient);
	}
}