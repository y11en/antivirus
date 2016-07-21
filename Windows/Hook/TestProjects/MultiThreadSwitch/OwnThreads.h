#ifndef __OWNMULTI
#define __OWNMULTI

#define __PAGE_SIZE 0x1000

typedef struct _DATABLOCK {
	CHAR	Sign[12];
	DWORD ThID;
	DWORD BlockNum;
	DWORD PageNum;
	DWORD UsageCnt;
	PVOID DataBlVA;
}DATABLOCK,*PDATABLOCK;

DWORD WINAPI EventWaiterThread(void* pParam);
VOID FillBlock(PVOID pBlock,DWORD BlockSize,PDATABLOCK FillData);
VOID CheckBlock(PVOID pBlock,DWORD BlockSize,PDATABLOCK FillData,PCHAR Comment);
VOID PrintFillData(PCHAR Buff, PDATABLOCK FillData);
BOOL InitOrigs(DWORD RegsCnt,DWORD m_RegSize);
VOID StartThreads(DWORD ThCnt);
VOID UnregOrigs(VOID);
VOID DoneOrig(VOID);


#endif __OWNMULTI
