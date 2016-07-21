#include "calcsum.h"

typedef struct TCRCPAGE
{
	DWORD CurrentPos;
	DWORD Page[_CRCPAGESIZE];
	PVOID pNext; //PCRCPAGE
}CRCPAGE, *PCRCPAGE;

int g_PageCount = 0;

void FreeCrcPages(PCRCPAGE* pPageRoot)
{
	PCRCPAGE pPage = *pPageRoot;
	while (pPage != NULL)
	{
		*pPageRoot = (PCRCPAGE) pPage->pNext;
		VirtualFree(pPage, sizeof(CRCPAGE), MEM_DECOMMIT);
		pPage = *pPageRoot;
	}
	*pPageRoot = NULL;
}

BOOL CheckOldCRC(PCRCPAGE* pPageRoot, DWORD dwCrc)
{
	PCRCPAGE pPage = *pPageRoot;
	while (pPage != NULL)
	{
		for (DWORD cou = 0; cou < pPage->CurrentPos; cou++)
		{
			if (pPage->Page[cou] == dwCrc)
				return TRUE;
		}
		pPage = (PCRCPAGE) pPage->pNext;
	}
	return FALSE;
}

void AddNewCRC(PCRCPAGE* pPageRoot, DWORD dwCrc)
{
	PCRCPAGE pPage;

	if (*pPageRoot == NULL)
	{
		pPage = (PCRCPAGE) VirtualAlloc(NULL, sizeof(CRCPAGE), MEM_COMMIT, PAGE_READWRITE);
		if (pPage != NULL)
		{
			g_PageCount++;
			pPage->CurrentPos = 0;
			pPage->pNext = NULL;
			ZeroMemory(pPage->Page, _CRCPAGESIZE * sizeof(DWORD));
		}
		*pPageRoot = pPage;
	}
	pPage = *pPageRoot;
	while (pPage != NULL)
	{
		if(pPage->CurrentPos == _CRCPAGESIZE)
			pPage = (PCRCPAGE) pPage->pNext;
		else
		{
			pPage->Page[pPage->CurrentPos++] = dwCrc;
			//ODS(("KAVSC: new crc stored\n"));
			if(pPage->CurrentPos == _CRCPAGESIZE)
			{
				pPage->pNext = (PCRCPAGE) VirtualAlloc(NULL, sizeof(CRCPAGE), MEM_COMMIT, PAGE_READWRITE);
				if (pPage->pNext == NULL)
					pPage->CurrentPos--; // крутим пока память не выделит
				else
				{
					g_PageCount++;
					if (g_PageCount > _CRCPAGECOUNT)
					{
						PCRCPAGE pPageRootOld = *pPageRoot;
						*pPageRoot = (PCRCPAGE) (*pPageRoot)->pNext;
						VirtualFree(pPageRootOld, sizeof(CRCPAGE), MEM_DECOMMIT);
						g_PageCount--;
					}

					pPage = (PCRCPAGE) pPage->pNext;
					pPage->CurrentPos = 0;
					pPage->pNext = NULL;
					ZeroMemory(pPage->Page, _CRCPAGESIZE * sizeof(DWORD));
				}
			}
			pPage = NULL;
		}
	}
}

DWORD CountCRC32(int size, LPBYTE ptr, DWORD dwOldCRC32)
{
    dwOldCRC32 = ~dwOldCRC32;
    while(size-- != 0)
    {
        unsigned long i;
        unsigned long c = (unsigned long) (*ptr++);
        for( i=0; i<8; i++)
        {
            if (((dwOldCRC32^c)&1)!=0)
                dwOldCRC32 = (dwOldCRC32>>1) ^ 0xEDB88320;
            else
                dwOldCRC32 = (dwOldCRC32>>1);
            c>>=1;
        }
    }
    return ~dwOldCRC32;
}
// -----------------------------------------------------------------------------------------
