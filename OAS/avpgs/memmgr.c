#include "memmgr.h"

BOOL PMMInit(DWORD flProtect, PMM_CTX** ppPMMCtx)
{
    SYSTEM_INFO aSystemInfo;
	PMM_CTX* pPMMCtx;

	if (!ppPMMCtx)
		return FALSE;
	pPMMCtx = (PMM_CTX*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PMM_CTX));
	if (!pPMMCtx)
		return FALSE;
    GetSystemInfo( &aSystemInfo );
    pPMMCtx->thePageSize = aSystemInfo.dwPageSize;
	pPMMCtx->flProtect = flProtect;
	*ppPMMCtx = pPMMCtx;
	return TRUE;
}


void PMMDone(PMM_CTX* pPMMCtx)
{
    PageControlBlock* aPCB = pPMMCtx->thePCB;
    while ( aPCB ) {
		PageControlBlock* aTmpPCB = aPCB->theNextPCB;
		VirtualFree ( aPCB, aPCB->thePagesCount * pPMMCtx->thePageSize, MEM_RELEASE );
		aPCB = aTmpPCB;
    }
}

void* PMMAlloc(PMM_CTX* pPMMCtx, size_t aSize)
{
    PageControlBlock* aPCB = pPMMCtx->thePCB;
    PageControlBlock* aPrevPCB = aPCB;
	unsigned int thePageSize = pPMMCtx->thePageSize;
	void* aResult;
	alignment_type aNumberOfNeededBlocks;
	
    // trying to find a memory block with needed size
    while ( aPCB ) {
		if ( aPCB->theNumberOfFreeBlocks * sizeof ( alignment_type ) > aSize )
			break;
		aPrevPCB = aPCB;
		aPCB = aPCB->theNextPCB;
    }
    // if a block hasn't been found, allocate new page of memory
    if ( !aPCB ) {
		unsigned int aPagesCount = ( aSize + sizeof ( PageControlBlock ) ) / thePageSize + 
			( ( ( aSize + sizeof ( PageControlBlock ) )  % thePageSize ) ? 1 : 0 );
		unsigned int aFullSize = thePageSize * aPagesCount;
		aPCB = (PageControlBlock*)( VirtualAlloc ( 0, aFullSize, MEM_RESERVE | MEM_COMMIT, pPMMCtx->flProtect ) );
		if ( !aPCB ) 
			return 0;
		if ( !pPMMCtx->thePCB )
			pPMMCtx->thePCB = aPCB;
		else
			aPrevPCB->theNextPCB = aPCB;
		aPCB->thePagesCount = aPagesCount;
		aPCB->theFirstUnusedBlock = (alignment_type*)( (unsigned char*) (aPCB) + sizeof ( PageControlBlock ) );
		aPCB->theNumberOfFreeBlocks = ( aFullSize - sizeof ( PageControlBlock ) ) / sizeof ( alignment_type );
#if defined (COMPLEX_MEMORY_MANAGER)
		// enumeration free memory in the block 
		for ( alignment_type i = 0; i < aPCB->theNumberOfFreeBlocks - 1 ; ++i )
			aPCB->theFirstUnusedBlock [ i ] = i + 1;
		aPCB->theFirstUnusedBlock [ aPCB->theNumberOfFreeBlocks - 1 ] = 0;
#endif
    }
    aResult = aPCB->theFirstUnusedBlock;
    aNumberOfNeededBlocks = aSize / sizeof ( alignment_type ) + ( ( aSize % sizeof ( alignment_type ) ) ? 1 : 0 );
    aPCB->theFirstUnusedBlock = aPCB->theFirstUnusedBlock + aNumberOfNeededBlocks;
    aPCB->theNumberOfFreeBlocks -= aNumberOfNeededBlocks;
    return aResult;
}

void PMMFree (PMM_CTX* pPMMCtx, void* mem) 
{
#if defined (COMPLEX_MEMORY_MANAGER)
	
#endif
}

BOOL PMMSetMemoryProtection(PMM_CTX* pPMMCtx, DWORD flProtect)
{
	PageControlBlock* aPCB;

    aPCB = pPMMCtx->thePCB;
    while ( aPCB ) {
		DWORD anOldProtection;
		if (!VirtualProtect ( aPCB, aPCB->thePagesCount * pPMMCtx->thePageSize, flProtect, &anOldProtection ))
			return FALSE;
		aPCB = aPCB->theNextPCB;
    }
    pPMMCtx->flProtect = flProtect ;
	return TRUE;
}

DWORD PMMGetMemoryProtection (PMM_CTX* pPMMCtx)
{
    return pPMMCtx->flProtect;
}
