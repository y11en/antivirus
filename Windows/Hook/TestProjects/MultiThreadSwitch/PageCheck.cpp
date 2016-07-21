#include "windows.h"

#include "OwnThreads.h"

extern DWORD EvExit;
extern HANDLE hPauseEvent;

VOID FillPage(PVOID pPage,PDATABLOCK FillData)
{
PDATABLOCK Tmp;
int i;
	Tmp=(PDATABLOCK)pPage;
	FillData->DataBlVA=pPage;
	for(i=__PAGE_SIZE/sizeof(DATABLOCK)-1;i>=0;i--) {
		memcpy(Tmp+i,FillData,sizeof(DATABLOCK));
	}
}

VOID FillBlock(PVOID pBlock,DWORD BlockSize,PDATABLOCK FillData)
{
DWORD PagesInBlock;
	PagesInBlock=BlockSize/__PAGE_SIZE;
	FillData->PageNum=0;
	for(DWORD i=0;i<PagesInBlock;i++) {
		FillPage((((BYTE*)(pBlock))+i*__PAGE_SIZE),FillData);
		FillData->PageNum++;
	}
}

BOOL CheckPage(PVOID pPage,PDATABLOCK FillData,PCHAR Comment)
{
PDATABLOCK Tmp;
int i;
CHAR Str[1024];
BOOL ret=TRUE;
DATABLOCK DataFromPage;
	Tmp=(PDATABLOCK)pPage;
	FillData->DataBlVA=pPage;
	for(i=__PAGE_SIZE/sizeof(DATABLOCK)-1;i>=0 && EvExit==0;i--) {
		DWORD FromPage=*(DWORD*)(Tmp+i);
		DWORD FromPage2=*(DWORD*)(Tmp+i);
//		WaitForSingleObject(hPauseEvent,INFINITE);
		if(FromPage2!=FromPage) {
			OutputDebugString("Bla! FromPage2!=FromPage\n");
		}
		memcpy(&DataFromPage,Tmp+i,sizeof(DATABLOCK));
		if(memcmp(&DataFromPage,FillData,sizeof(DATABLOCK))) {
			__asm int 3;
			wsprintf(Str, "%08x !!!!!!!! MISMATCH Page=%x Addr=%x DETECTED %s !!!!!!!!\n",GetCurrentThreadId(),pPage,Tmp+i,Comment);
			strcpy(Str+strlen(Str),"ForCheck ");
			PrintFillData(Str+strlen(Str),FillData);
			strcpy(Str+strlen(Str),"FromPage ");
			PrintFillData(Str+strlen(Str),&DataFromPage);
			strcpy(Str+strlen(Str),"InPageNow");
			PrintFillData(Str+strlen(Str),Tmp+i);
			OutputDebugString(Str);
			__asm int 3;
			ret=FALSE;
		}
		(Tmp+i)->UsageCnt++;
	}
	return ret;		
}

VOID CheckBlock(PVOID pBlock,DWORD BlockSize,PDATABLOCK FillData,PCHAR Comment)
{
DWORD PagesInBlock;
	PagesInBlock=BlockSize/__PAGE_SIZE;
	FillData->PageNum=0;
	for(DWORD i=0;i<PagesInBlock && EvExit==0;i++) {
		WaitForSingleObject(hPauseEvent,INFINITE);
		CheckPage((((BYTE*)(pBlock))+i*__PAGE_SIZE),FillData,Comment);
		FillData->PageNum++;
	}
}


VOID PrintFillData(PCHAR Buff, PDATABLOCK FillData)
{
	wsprintf(Buff,"FillData Sig=%s Th=%x Bl=%x Pg=%x Cnt=%x VA=%x \n",FillData->Sign,FillData->ThID,FillData->BlockNum,FillData->PageNum,FillData->UsageCnt,FillData->DataBlVA);
}
