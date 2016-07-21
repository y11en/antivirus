#define _POINTERPAGESIZE	20

//#include <pshpack1.h>

typedef struct TSINGLEPAGE
{
	DWORD dwData;
	PVOID pData;
	PVOID pNextLevel;
}SINGLEPAGE, *PSINGLEPAGE;

typedef struct TPOINTERPAGE
{
	DWORD CurrentPos;
	SINGLEPAGE Page[_POINTERPAGESIZE];
	PVOID pNext; //PPOINTERPAGE
}POINTERPAGE, *PPOINTERPAGE;

//#include <poppack.h>

/*
void FreePages(PPOINTERPAGE* pPageRoot)
{
	PPOINTERPAGE pPage = *pPageRoot;
	while (pPage != NULL)
	{
		*pPageRoot = (PPOINTERPAGE) pPage->pNext;
		ExFreePool(pPage);
		pPage = *pPageRoot;
	}
	*pPageRoot = NULL;
}
*/

__inline PSINGLEPAGE GetPData(PPOINTERPAGE* pPageRoot, DWORD dwData)
{
	PPOINTERPAGE pPage = *pPageRoot;
	DWORD cou;
	while (pPage != NULL)
	{
		for (cou = 0; cou < pPage->CurrentPos; cou++)
		{
			if (pPage->Page[cou].dwData == dwData)
				return &pPage->Page[cou];
		}
		pPage = (PPOINTERPAGE) pPage->pNext;
	}
	return NULL;
}

PSINGLEPAGE AddNewData(PPOINTERPAGE* pPageRoot, DWORD dwData, PVOID pData, PVOID pNextLevel)
{
	PSINGLEPAGE bRet = NULL;
	PPOINTERPAGE pPage;

	if (*pPageRoot == NULL)
	{
		pPage = (PPOINTERPAGE) ExAllocatePoolWithTag(NonPagedPool,sizeof(POINTERPAGE),'PboS');
		if (pPage != NULL)
		{
			pPage->CurrentPos = 0;
			pPage->pNext = NULL;
		}
		*pPageRoot = pPage;
	}
	pPage = *pPageRoot;
	while ((pPage != NULL) && (bRet == NULL))
	{
		if(pPage->CurrentPos == _POINTERPAGESIZE) {
			PPOINTERPAGE pPageTmp = (PPOINTERPAGE) pPage->pNext;
			if (pPageTmp != NULL)
				pPage = pPageTmp;
			else {
				pPageTmp = (PPOINTERPAGE) ExAllocatePoolWithTag(NonPagedPool,sizeof(POINTERPAGE),'PboS');
				if (pPageTmp == NULL)
				{
					DbPrint(DC_NOCAT,DL_ERROR, ("Block for filters init failed (no memory)!\n"));		
				}
				else
				{
					pPageTmp->pNext = NULL;

					pPageTmp->CurrentPos = 0;

					pPageTmp->Page[pPageTmp->CurrentPos].dwData = dwData;
					pPageTmp->Page[pPageTmp->CurrentPos].pData = pData;
					pPageTmp->Page[pPageTmp->CurrentPos].pNextLevel = pNextLevel;
					
					bRet = &pPageTmp->Page[pPageTmp->CurrentPos];

					pPageTmp->CurrentPos++;

					pPage->pNext = pPageTmp;
					DbPrint(DC_NOCAT,DL_NOTIFY, ("new page for filters queue allocated\n"));
				}
			}
		}
		else
		{
			pPage->Page[pPage->CurrentPos].dwData = dwData;
			pPage->Page[pPage->CurrentPos].pData = pData;
			pPage->Page[pPage->CurrentPos].pNextLevel = pNextLevel;
			
			bRet = &pPage->Page[pPage->CurrentPos];

			pPage->CurrentPos++;

		}
	}
	return bRet;
}
// -----------------------------------------------------------------------------------------
