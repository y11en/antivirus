// loadstat.c
//

#include "common.h"

// ------------------------------------------------------------------------------------------------
TREE_STATIC
TREE_ERR _TREE_CALL TreeUnload_Static(sRT_CTX* pRTCtx)
{
	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
		return TREE_OK;

	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2) 
		return TREE_EUNEXPECTED;

	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_STATIC) 
		return TREE_EUNEXPECTED;

	if (!pRTCtx->bDataMapped && pRTCtx->pTreeBase)
		_HeapFree(pRTCtx->pHeapCtx, pRTCtx->pTreeBase);
	ResetRTContext(pRTCtx, pRTCtx->pHeapCtx);

	return TREE_OK;
}		

// ------------------------------------------------------------------------------------------------

TREE_STATIC
TREE_ERR _TREE_CALL TreeLoad_StaticOnMap(sRT_CTX* pRTCtx, void* pMappedData, uint32_t dwMappedDataSize)
{
	TREE_ERR error;
	TREE_INFO* pTreeInfo = (TREE_INFO*)pMappedData;

	if (TREE_FAILED(error = TreeUnload_Any(pRTCtx)))
		return error;

	if (pTreeInfo->dwMagic != MAGIC_TREE_DB_V2 || pTreeInfo->dwFormat != cFORMAT_STATIC)
		return TREE_ECORRUPT;

	if (dwMappedDataSize < pTreeInfo->dwTotalSize)
		return TREE_ESMALLBUF;

	memcpy(&pRTCtx->sTreeInfo, pTreeInfo, sizeof(TREE_INFO));
	pRTCtx->pTreeBase = pMappedData;
	pRTCtx->bDataMapped = 1;
	pRTCtx->pTreeHashTable = (uint8_t*)pMappedData + sizeof(TREE_INFO);
	pRTCtx->pRoot = (NODE*) ((uint8_t*)pMappedData + sizeof(TREE_INFO) + pRTCtx->sTreeInfo.dwHashTableSize);
	if (pTreeInfo->dwHashTableSize)
		pRTCtx->wHashTableANDMask = (uint16_t)(pTreeInfo->dwHashTableSize - 1);

	pRTCtx->TreeGetNextNodeXXX[NDT_O_Index32] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Index32;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Index32Fast] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Index32Fast;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Joined32] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Joined32;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_JoinedRT32] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_JoinedRT32;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Joined8] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Joined8;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_JoinedRT8] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_JoinedRT8;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Hashed] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Hashed;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_HashedRT] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_HashedRT;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Array] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Array;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_ArrayFast] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_ArrayFast;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Index8] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Index8;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_List] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_List;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_Tail] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_Tail;
	pRTCtx->TreeGetNextNodeXXX[NDT_O_AnyByteRT] = (tTreeGetNextNodeXXX)TreeGetNextNode_O_AnyByteRT;
	pRTCtx->TreeGetNextNodeXXX[NDT_Aster] = (tTreeGetNextNodeXXX)TreeGetNextNodeAsterRT;
	return TREE_OK;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
TREE_ERR _TREE_CALL TreeLoad_Static(sRT_CTX* pRTCtx, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	void * pMem;
	uint64_t qwStartPos;

	TREE_INFO sTreeInfo;

	if (TREE_FAILED(error = _Seek(pIOCtx, &qwStartPos, 0, TREE_IO_CTX_SEEK_CUR)))
		return error;

	if (TREE_FAILED(error = _SeekRead(pIOCtx, qwStartPos, (uint8_t*)&sTreeInfo, sizeof(sTreeInfo))))
		return error;

	if (sTreeInfo.dwMagic != MAGIC_TREE_DB_V2 || sTreeInfo.dwFormat != cFORMAT_STATIC)
		return TREE_ECORRUPT;

	if (TREE_FAILED(error = _HeapAlloc2(pRTCtx->pHeapCtx, &pMem, sTreeInfo.dwTotalSize)))
		return error;

	if (TREE_FAILED(error = _Read(pIOCtx, pMem, sTreeInfo.dwTotalSize)))
		_HeapFree(pRTCtx->pHeapCtx, pMem);

	if (TREE_SUCCEEDED(error))
		error = TreeLoad_StaticOnMap(pRTCtx, pMem, sTreeInfo.dwTotalSize);

	pRTCtx->bDataMapped = 0;

	if (TREE_FAILED(error))
		_Seek(pIOCtx, NULL, qwStartPos, TREE_IO_CTX_SEEK_SET); // restore IO position in case of error

	return error;
}

// ------------------------------------------------------------------------------------------------

#ifdef TREE_STATIC_ONLY

TREE_STATIC
TREE_ERR _TREE_CALL TreeLoad_Dynamic(sRT_CTX* pRTCtx, sIO_CTX* pIOCtx)
{
	return TREE_EUNEXPECTED;
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeUnload_Any(sRT_CTX* pRTCtx)
{
	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
		return TREE_OK;

	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2) 
		return TREE_EUNEXPECTED;

	if (pRTCtx->sTreeInfo.dwFormat == cFORMAT_STATIC) 
		return TreeUnload_Static(pRTCtx);

	return TREE_EUNEXPECTED;
}

#endif

// ------------------------------------------------------------------------------------------------
