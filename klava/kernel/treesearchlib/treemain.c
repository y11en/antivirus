// treemain.c
//

#include "common.h"

static void InitBitMaskArray(sRT_CTX* pRTCtx)
{
	unsigned int i;
	uint8_t bMask;
	bMask = 1;
	for (i=0; i<8; i++)
	{
		pRTCtx->bBitMask[i] = bMask;
		bMask <<= 1;
	}
}

TREE_STATIC
void ResetRTContext(sRT_CTX* pRTCtx, sHEAP_CTX* pNewHeapCtx)
{
	uint32_t i;
	
	if (pRTCtx->pHeapCtx)
	{
		if (pRTCtx->pRTNodesPool)
			_HeapFree(pRTCtx->pHeapCtx, pRTCtx->pRTNodesPool);
		if (pRTCtx->sSpinners.Spinner)
			_HeapFree(pRTCtx->pHeapCtx, pRTCtx->sSpinners.Spinner);
		if (pRTCtx->sNewSpinners.Spinner)
			_HeapFree(pRTCtx->pHeapCtx, pRTCtx->sNewSpinners.Spinner);
	}

	ZeroMemory(pRTCtx, sizeof(sRT_CTX));

	for (i=0; i<countof(pRTCtx->TreeGetNextNodeXXX); i++)
		pRTCtx->TreeGetNextNodeXXX[i] = (tTreeGetNextNodeXXX)TreeGetNextNodeUnsupported;
	InitBitMaskArray(pRTCtx);
	pRTCtx->pHeapCtx = pNewHeapCtx;
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeInit(sHEAP_CTX* pStorageHeapCtx, sRT_CTX** ppRTCtx)
{
	sRT_CTX* pRTCtx;
	
	if (pStorageHeapCtx==NULL || ppRTCtx==NULL)
		return TREE_EINVAL;

	*ppRTCtx = NULL;

	pRTCtx = _HeapAlloc(pStorageHeapCtx, sizeof(sRT_CTX));
	if (pRTCtx == NULL)
		return TREE_ENOMEM;

	ZeroMemory(pRTCtx, sizeof(sRT_CTX));

	ResetRTContext(pRTCtx, pStorageHeapCtx);
	*ppRTCtx = pRTCtx;

	return TREE_OK;
}

static
void _TREE_CALL TreeInitSearcherContext (sRT_CTX* pRTCtx, sHEAP_CTX* pSearcherHeapCtx, sRT_CTX* pSearcherRTCtx)
{
	if (pSearcherHeapCtx == NULL)
		pSearcherHeapCtx = pRTCtx->pHeapCtx;

	memcpy(pSearcherRTCtx, pRTCtx, sizeof(sRT_CTX));
	pSearcherRTCtx->pHeapCtx = pSearcherHeapCtx;
	pSearcherRTCtx->pRTNodesPool = NULL;
	ZeroMemory(&pSearcherRTCtx->sSpinners, sizeof(pSearcherRTCtx->sSpinners));
	ZeroMemory(&pSearcherRTCtx->sNewSpinners, sizeof(pSearcherRTCtx->sNewSpinners));
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeGetSeacherContext(sRT_CTX* pRTCtx, sHEAP_CTX* pSeacherHeapCtx, sSEARCHER_RT_CTX** ppSeacherRTCtx)
{
	sRT_CTX* pSeacherRTCtx;

	if (pRTCtx==NULL || ppSeacherRTCtx==NULL)
		return TREE_EINVAL;

	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2)
		return TREE_ENOINIT;

	*ppSeacherRTCtx = NULL;

	if (pSeacherHeapCtx == NULL)
		pSeacherHeapCtx = pRTCtx->pHeapCtx;

	pSeacherRTCtx = _HeapAlloc(pSeacherHeapCtx, sizeof(sRT_CTX));
	if (pSeacherRTCtx == NULL)
		return TREE_ENOMEM;

	TreeInitSearcherContext (pRTCtx, pSeacherHeapCtx, pSeacherRTCtx);

	*ppSeacherRTCtx = (sSEARCHER_RT_CTX*)pSeacherRTCtx;

	return TREE_OK;
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeDoneSeacherContext(sSEARCHER_RT_CTX* pSeacherRTCtx)
{
	ResetRTContext((sRT_CTX*)pSeacherRTCtx, ((sRT_CTX*)pSeacherRTCtx)->pHeapCtx);
	_HeapFree(((sRT_CTX*)pSeacherRTCtx)->pHeapCtx, pSeacherRTCtx);
	return TREE_OK;
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeDone(sRT_CTX* pRTCtx)
{
        TreeUnload_Any ( pRTCtx );
	ResetRTContext(pRTCtx, pRTCtx->pHeapCtx);
	_HeapFree(pRTCtx->pHeapCtx, pRTCtx);
	return TREE_OK;
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeGetInfo(sRT_CTX* pRTCtx, TREE_INFO* pTreeInfo)
{
	if (pRTCtx==NULL || pTreeInfo==NULL)
		return TREE_EINVAL;
	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2)
		return TREE_ENOINIT;
	memcpy(pTreeInfo, &pRTCtx->sTreeInfo, sizeof(TREE_INFO));
	return TREE_OK;
}
