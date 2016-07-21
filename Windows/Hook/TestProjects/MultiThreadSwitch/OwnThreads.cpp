#include "windows.h"

#include <Hook\avpgcom.h>
#include <Hook\FsdrvLib.h>
#include "OwnThreads.h"


LONG TotalThreads=0;
DWORD InCheck=0;
DWORD gEnteredRegs=0;
LONG LastInChTime=0;
extern DWORD EvExit;
extern HANDLE hPauseEvent;

ORIG_HANDLE *pOrigs=NULL;
DWORD gRegsCnt=0;
DWORD gRegSize=0;
DWORD gmin_number_of_shadows=0;

#define regsizeaddon 98237

DATABLOCK gOrigData;

BOOL InitOrigs(DWORD RegsCnt,DWORD m_RegSize)
{
ORIG_HANDLE Orig;
CHAR Msg[1024];
BOOL LockRet;
	strcpy(gOrigData.Sign,"Original");
	gOrigData.DataBlVA=0;
	gOrigData.ThID=GetCurrentThreadId();
	gOrigData.BlockNum=0;
	gOrigData.PageNum=0;
	gOrigData.UsageCnt=0;

	pOrigs=(ORIG_HANDLE*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,RegsCnt*sizeof(ORIG_HANDLE));
	gRegSize=m_RegSize;
	for(DWORD i=0;i<RegsCnt;i++) {
		Orig=FSDrvTSPRegister(gRegSize+regsizeaddon);
		pOrigs[i]=Orig;
		if(Orig) {
			FillBlock(Orig,gRegSize+regsizeaddon,&gOrigData);
			strcpy(Msg,"OrigInit ");
			PrintFillData(Msg+strlen(Msg),(PDATABLOCK)Orig);
			OutputDebugString(Msg);
			LockRet=FSDrvTSPLock(Orig,gRegSize,gmin_number_of_shadows);
			wsprintf(Msg, "FSDrvTSPLock(%x,%d,%d) ret %d\n",Orig,gRegSize,gmin_number_of_shadows,LockRet);
			OutputDebugString(Msg);
			if(LockRet) {
				gRegsCnt++;
			} else {
				FSDrvTSPUnregister(Orig);
				break;
			}
		} else {
			wsprintf(Msg, "FSDrvTSPRegister fail\n");
			OutputDebugString(Msg);
			return FALSE;
		}
		gOrigData.BlockNum++;
	}
	return TRUE;
}


VOID UnregOrigs(VOID)
{
CHAR Msg[1024];
ORIG_HANDLE Orig;
	gOrigData.BlockNum=0;
	for(DWORD i=0;i<gRegsCnt;i++) {
		Orig=pOrigs[i];
		if(Orig) {
			wsprintf(Msg,"Unreg Orig %x\n",Orig);
			OutputDebugString(Msg);
			gOrigData.BlockNum++;
			FSDrvTSPUnregister(Orig);
		}
	}
}

VOID DoneOrig(VOID)
{
	UnregOrigs();
	GlobalFree(pOrigs);
}

VOID StartThreads(DWORD ThCnt)
{
HANDLE hThrd;
DWORD	threadid;
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
	for (DWORD cou = 0; cou < ThCnt; cou++) {
		hThrd=CreateThread(NULL, 0, EventWaiterThread, (LPVOID) cou, 0, &threadid);
		SetThreadPriority(hThrd,THREAD_PRIORITY_ABOVE_NORMAL);
	}
	OutputDebugString("-------------------------------All Threads started\n");
	OutputDebugString("-------------------------------All Threads started\n");
	OutputDebugString("-------------------------------All Threads started\n");
	OutputDebugString("-------------------------------All Threads started\n");
	OutputDebugString("-------------------------------All Threads started\n");
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
}


DWORD WINAPI EventWaiterThread(void* pParam)
{
DWORD Unique = (DWORD) pParam;
char dbgmsg[MAX_PATH];
DATABLOCK FillData;
DATABLOCK OrigData;
FILETIME FtStart,Ft;
DWORD EnteredRegs;
DWORD i;

	InterlockedIncrement(&TotalThreads);
	wsprintf(dbgmsg, "PageSwapable: Unique %d (ThreadID %x)\n", Unique, GetCurrentThreadId());
	OutputDebugString(dbgmsg);

	memcpy(&OrigData,&gOrigData,sizeof(OrigData));
	memset(&FillData,0,sizeof(FillData));
	strcpy(FillData.Sign,"Shadow");
	FillData.ThID=GetCurrentThreadId();
	
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	while (EvExit == 0) {
		FillData.DataBlVA=0;
		FillData.BlockNum=0;
		FillData.PageNum=0;
		FillData.UsageCnt=0;
		EnteredRegs=0;
		OrigData.BlockNum=0;
		for(i=0;i<gRegsCnt && EvExit==0;i++) {
			WaitForSingleObject(hPauseEvent,INFINITE);
//			CheckBlock(pOrigs[i],gRegSize,&OrigData,"before ThEnter"); //check is not valid here
			if(FSDrvTSPThreadEnter(pOrigs[i])) {
//				CheckBlock(pOrigs[i],gRegSize,&OrigData,"after ThEnter");//когда идет ReuseOldShadow здесь мусор от старой триды
				gEnteredRegs++;
				FillBlock(pOrigs[i],gRegSize,&FillData);
				EnteredRegs++;
				//		SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
			} else {
				wsprintf(dbgmsg, "FSDrvTSPThreadEnter(%x) fail\n",pOrigs[i]);
				OutputDebugString(dbgmsg);
				break;
			}
			OrigData.BlockNum++;
			FillData.BlockNum++;
		}
		if(EnteredRegs) {
			InCheck++;
			GetSystemTimeAsFileTime(&FtStart);
			FillData.BlockNum=0;
			for(i=0;i<EnteredRegs && EvExit==0;i++) {
				FillData.UsageCnt=0;
				for(DWORD debugcnt=0;debugcnt<0x20 && EvExit==0;debugcnt++) {
					WaitForSingleObject(hPauseEvent,INFINITE);
					Sleep(10*debugcnt);
					CheckBlock(pOrigs[i],gRegSize,&FillData,"in check circle");
					FillData.UsageCnt++;
				}
				FillData.BlockNum++;
			}
			GetSystemTimeAsFileTime(&Ft);
			LastInChTime=(LONG)((*(__int64*)&Ft - *(__int64*)&FtStart)/10000000);
			InCheck--;
		}
		OrigData.BlockNum=0;
		FillData.BlockNum=0;
		for(DWORD i=0;i<EnteredRegs;i++) {
			WaitForSingleObject(hPauseEvent,INFINITE);
			CheckBlock(pOrigs[i],gRegSize,&FillData,"before ThLeave");
			wsprintf(dbgmsg, "%08x -----CHECK %d / %d ----- ", GetCurrentThreadId(),LastInChTime,EnteredRegs);
			PrintFillData(dbgmsg+strlen(dbgmsg),(PDATABLOCK)pOrigs[i]);
			OutputDebugString(dbgmsg);
			FSDrvTSPThreadLeave(pOrigs[i]);
			gEnteredRegs--;
//			CheckBlock(pOrigs[i],gRegSize,&OrigData,"after ThLeave");//check is not valid here
			OrigData.BlockNum++;
			FillData.BlockNum++;
		}			
	}

	wsprintf(dbgmsg, "PageSwapable: Exit Unique %d (ThreadID %x)\n", Unique, GetCurrentThreadId());
	OutputDebugString(dbgmsg);
	InterlockedDecrement(&TotalThreads);
	return 0;
}
