
#include <stdlib.h>

#define __MEMMANAGER_C__
    #include "memmngr.h"
#undef  __MEMMANAGER_C__


unsigned g_nLocalMemoryManager = 0;

//CLocalMemoryManager g_LocalMemoryManager;


void LocalMemoryManager_Init(
        CLocalMemoryManager* pmngr,
        unsigned   nBlockLimit,
        unsigned   nPoolSize,
        unsigned   nPoolsReallocStep,
        unsigned   nBlocksReallocStep
        )
{
    pmngr->m_nBlockLimit            = nBlockLimit;
    pmngr->m_nPoolSize              = nPoolSize;
    pmngr->m_nPoolsReallocStep      = nPoolsReallocStep;
    pmngr->m_nBlocksReallocStep     = nBlocksReallocStep;
    pmngr->m_pPools                 = 0;
    pmngr->m_nPoolsCount            = 0;
    pmngr->m_pMemBlocks             = 0;
    pmngr->m_nBlocksCount           = 0;
    pmngr->m_nBlocksReserved        = 0;
}

int MemBlocksCmp (const void *elem1, const void *elem2 )
{
    CMemBlockPtr *p1, *p2;
    p1 = (CMemBlockPtr*)elem1;
    p2 = (CMemBlockPtr*)elem2;
    if ( p1->m_bUsed != p2->m_bUsed )
        return p1->m_bUsed - p2->m_bUsed;
    if ( 0 == p1->m_bUsed )
        return p1->m_nReserved - p2->m_nReserved;
    return 0; //p1->m_nSize - p2->m_nSize; // need to sort only free part
}

void* LocalMemoryManager_New (CLocalMemoryManager* pmngr, unsigned nCount)
{
    unsigned i;
    int iMinInd = -1;
    unsigned bAllocNewBlock = 0;
    int iMinPool = -1;
    unsigned nFreeSize = 0, nMinFreeSize = 0;
    unsigned bAllocateNewPool = 0;
    char* pRet = 0;
	const unsigned int nMoreKoef = 2;

    if (nCount > pmngr->m_nBlockLimit || nCount > pmngr->m_nPoolSize)
        return malloc(nCount);

    g_nLocalMemoryManager ++;
    // find minimum suffice free existing block (free blocks sorted first)
	if( pmngr->m_nBlocksCount > 1 )
		qsort(pmngr->m_pMemBlocks, pmngr->m_nBlocksCount, sizeof (CMemBlockPtr), MemBlocksCmp);
    for (i = 0; i < pmngr->m_nBlocksCount && 0 == pmngr->m_pMemBlocks[i].m_bUsed; i ++ )
        if ( nCount <= pmngr->m_pMemBlocks[i].m_nReserved && nCount * nMoreKoef > pmngr->m_pMemBlocks[i].m_nReserved )
        {
			if( -1 == iMinInd || pmngr->m_pMemBlocks[i].m_nReserved < pmngr->m_pMemBlocks[iMinInd].m_nReserved )
				iMinInd = i;
        }

	if ( -1 != iMinInd )
	{
		// found free block
		pmngr->m_pMemBlocks [iMinInd].m_bUsed = 1;
		pmngr->m_pMemBlocks [iMinInd].m_nSize = nCount;
		memset(pmngr->m_pMemBlocks [iMinInd].m_pBuffer, 0, nCount);
		return pmngr->m_pMemBlocks [iMinInd].m_pBuffer;
	}

    // find suffice end of pool
    // and allocate new block at the end of some existing or new pool 
    for (i = 0; i < pmngr->m_nPoolsCount; i ++)
        if ( pmngr->m_pPools[i].m_pBuffer &&
             ( nFreeSize = pmngr->m_pPools[i].m_nReserved - pmngr->m_pPools[i].m_nBytesUsed ) >= nCount
             /*  && (-1 == iMinPool || nFreeSize < nMinFreeSize)  // comment to speed up */
           )
        {
            iMinPool = i;
            /* nMinFreeSize = nFreeSize; // comment to speed up */
			break;
        }
    if (-1 == iMinPool)
        // allocate new block in new pool 
        for (i = 0; i < pmngr->m_nPoolsCount; i ++)
            if (0 == pmngr->m_pPools[i].m_pBuffer)
            {
                // found clear pool
                bAllocateNewPool = 1; 
                iMinPool = i;
                break;
            }

    if (-1 == iMinPool)
    {
        // add new pools descriptors
        CMemPool* pNewPoolsArray = (CMemPool*)realloc(pmngr->m_pPools, (pmngr->m_nPoolsCount + pmngr->m_nPoolsReallocStep)*sizeof(CMemPool));
        if ( 0 == pNewPoolsArray )
            return 0;
        pmngr->m_pPools = pNewPoolsArray;
        memset(pmngr->m_pPools + pmngr->m_nPoolsCount, 0, pmngr->m_nPoolsReallocStep*sizeof(CMemPool));
        iMinPool = pmngr->m_nPoolsCount;
        pmngr->m_nPoolsCount += pmngr->m_nPoolsReallocStep;
        bAllocateNewPool = 1;
    }
        
    if (bAllocateNewPool)
    {
        pmngr->m_pPools[iMinPool].m_pBuffer    = calloc(pmngr->m_nPoolSize, 1U);
        pmngr->m_pPools[iMinPool].m_nReserved  = pmngr->m_nPoolSize;
        pmngr->m_pPools[iMinPool].m_nBytesUsed = 0;
    }

    pRet = (char*)pmngr->m_pPools[iMinPool].m_pBuffer + pmngr->m_pPools[iMinPool].m_nBytesUsed;
    pmngr->m_pPools[iMinPool].m_nBytesUsed += nCount;

    // add pointer to pmngr->m_pMemBlocks
    if (pmngr->m_nBlocksCount >= pmngr->m_nBlocksReserved)
    {
        CMemBlockPtr* pNewBlocksArray = (CMemBlockPtr*)realloc(pmngr->m_pMemBlocks, (pmngr->m_nBlocksReserved + pmngr->m_nBlocksReallocStep)*sizeof(CMemBlockPtr));
        if (0 == pNewBlocksArray)
            return 0;
        pmngr->m_pMemBlocks = pNewBlocksArray;
        memset(pmngr->m_pMemBlocks + pmngr->m_nBlocksReserved, 0, pmngr->m_nBlocksReallocStep*sizeof(CMemBlockPtr));
        pmngr->m_nBlocksReserved += pmngr->m_nBlocksReallocStep;
    }
    pmngr->m_pMemBlocks[pmngr->m_nBlocksCount].m_bUsed		= 1;
    pmngr->m_pMemBlocks[pmngr->m_nBlocksCount].m_nSize		= nCount;
    pmngr->m_pMemBlocks[pmngr->m_nBlocksCount].m_nReserved	= nCount;
    pmngr->m_pMemBlocks[pmngr->m_nBlocksCount].m_pBuffer	= (void*)pRet;
    pmngr->m_nBlocksCount ++;

    return pRet;
}

void LocalMemoryManager_Delete (CLocalMemoryManager* pmngr, void* p)
{
    unsigned i;
    for (i = 0; i < pmngr->m_nBlocksCount; i ++ )
        if (p == pmngr->m_pMemBlocks[i].m_pBuffer)
        {
            pmngr->m_pMemBlocks[i].m_bUsed = 0;
            return;
        }
    free(p);
}


#include <crtdbg.h>
void*   LocalMemoryManager_Realloc (CLocalMemoryManager* pmngr, void* p, unsigned nNewSize)
{
    void        *ret = NULL;
    unsigned    i;

    if (NULL == p)
        return NULL;
    for (i = 0; i < pmngr->m_nBlocksCount; i ++ )
        if (p == pmngr->m_pMemBlocks[i].m_pBuffer)
        {
            if (0 == nNewSize)
            {
                pmngr->m_pMemBlocks[i].m_bUsed = 0;
                return NULL; // rule of realloc
            }
            if ( 0 == pmngr->m_pMemBlocks[i].m_bUsed )
                return NULL;
            if  ( nNewSize <= pmngr->m_pMemBlocks[i].m_nReserved )
			{
				pmngr->m_pMemBlocks[i].m_nSize = nNewSize;
                return p;
			}
            ret = LocalMemoryManager_New(pmngr, nNewSize);
            pmngr->m_pMemBlocks[i].m_bUsed = 0;
            if (ret)
                memcpy(ret, p, min(pmngr->m_pMemBlocks[i].m_nSize, nNewSize));
            return ret;
        }
    ret = realloc(p, nNewSize);
    return ret;
}

void LocalMemoryManager_Destroy  (CLocalMemoryManager *pmngr)
{
    unsigned i;

    for (i = 0; i < pmngr->m_nPoolsCount; i ++ )
        if ( pmngr->m_pPools[i].m_pBuffer )
            free(pmngr->m_pPools[i].m_pBuffer);
    
    free(pmngr->m_pPools);
    free(pmngr->m_pMemBlocks);

    pmngr->m_pPools            = 0;
    pmngr->m_nPoolsCount       = 0;
    pmngr->m_pMemBlocks        = 0;
    pmngr->m_nBlocksCount      = 0;
    pmngr->m_nBlocksReserved   = 0;
}

char* LocalMemoryManager_Strdup  (CLocalMemoryManager *pmngr, const char *strSource)
{
    size_t size;
    char* ret;
    
    size = strlen (strSource);
    ret = (char*) LocalMemoryManager_New (pmngr, size + 1);
	strcpy(ret, strSource);
    return ret;
}
