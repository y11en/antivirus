#if !defined (_C_PAGEMEMORYMANAGER_H_)
#define _C_PAGEMEMORYMANAGER_H_

#include <windows.h>

typedef unsigned int size_t;
typedef unsigned short alignment_type;

typedef struct tag_PageControlBlock PageControlBlock;

typedef struct tag_PageControlBlock{
	unsigned int thePagesCount;
	PageControlBlock* theNextPCB;
	alignment_type* theFirstUnusedBlock;
	unsigned int theNumberOfFreeBlocks;
}PageControlBlock;

typedef struct tag_PMM_CTX {
	DWORD flProtect;
	PageControlBlock* thePCB;
	unsigned int thePageSize;
} PMM_CTX;

BOOL PMMInit(DWORD flProtect, PMM_CTX** ppPMMCtx);
void PMMDone(PMM_CTX* pPMMCtx);
void* PMMAlloc(PMM_CTX* pPMMCtx, size_t);
void PMMFree(PMM_CTX* pPMMCtx, void*);
BOOL PMMSetMemoryProtection(PMM_CTX* pPMMCtx, DWORD);
DWORD PMMGetMemoryProtection(PMM_CTX* pPMMCtx);

#endif // _C_PAGEMEMORYMANAGER_H_
