/********************************************************************
	created:	2005/09/30
	created:	30:9:2005   13:47
	filename: 	client-filter\librules\memmngr.h
	file path:	client-filter\librules
	file base:	memmngr
	file ext:	h
	author:		Evgeny Smirnov
	
	purpose:	memory manager for preventing allocate excess
*********************************************************************/

#ifndef  __MEMMANAGER_H__
#define  __MEMMANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *   memory block descriptor
 */
typedef struct
{
	void           *m_pBuffer;
	unsigned	    m_nSize;
	unsigned	    m_nReserved;
    unsigned char   m_bUsed;

} CMemBlockPtr;

/*
 *   pool contains stacked memory blocks
 */
typedef struct
{
	void       *m_pBuffer;
	unsigned	m_nReserved;
	unsigned	m_nBytesUsed;

} CMemPool;

/*
 *   manage some memory pools
 */
typedef struct
{
    CMemPool       *m_pPools;          /*  pools descriptors                   */
	unsigned		m_nPoolsCount;      /*  active pools descriptors count      */

	CMemBlockPtr   *m_pMemBlocks;       /*  active memory blocks descriptors (in the all active pools)  */
	unsigned		m_nBlocksReserved;  /*  allocated blocks descriptors count                          */
	unsigned		m_nBlocksCount;     /*  active pools descriptors count                              */

    unsigned        m_nBlockLimit;      /* this manager only for a little blocks,
                                         * blocks more than m_nBlockLimit doesn't need optimization */
    unsigned        m_nPoolSize;
    unsigned        m_nPoolsReallocStep;
    unsigned        m_nBlocksReallocStep;

} CLocalMemoryManager;

void        LocalMemoryManager_Init(
                CLocalMemoryManager* pmngr,
                unsigned   nBlockLimit,
                unsigned   nPoolSize,
                unsigned   nPoolsReallocStep,
                unsigned   nBlocksReallocStep
                );

void        LocalMemoryManager_Destroy (CLocalMemoryManager *pmngr);

void*       LocalMemoryManager_New     (CLocalMemoryManager *pmngr, unsigned nCount);
void        LocalMemoryManager_Delete  (CLocalMemoryManager *pmngr, void* p);
void*       LocalMemoryManager_Realloc (CLocalMemoryManager *pmngr, void* p, unsigned nNewSize);
char*       LocalMemoryManager_Strdup  (CLocalMemoryManager *pmngr, const char *strSource);


// CLocalMemoryManager g_LocalMemoryManager;
// g_LocalMemoryManager must be declared in files, who include this header - at least once

#ifndef __MEMMANAGER_C__
    extern  CLocalMemoryManager g_LocalMemoryManager;

    #define calloc(x)       LocalMemoryManager_New      (&g_LocalMemoryManager, x)
    #define malloc(x)       LocalMemoryManager_New      (&g_LocalMemoryManager, x)
    #define free(x)         LocalMemoryManager_Delete   (&g_LocalMemoryManager, x)
    #define realloc(x, n)   LocalMemoryManager_Realloc  (&g_LocalMemoryManager, x, n)
    #define strdup(x)       LocalMemoryManager_Strdup   (&g_LocalMemoryManager, x)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif//__MEMMANAGER_H__
