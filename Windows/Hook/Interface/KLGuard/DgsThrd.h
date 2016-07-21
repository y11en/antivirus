#ifndef __DOGS_THREAD
#define __DOGS_THREAD

#include "windows.h"

typedef enum _eDT_CALLTYPE
{
	DT_INIT,
	DT_PROCEED,
	DT_CLOSE,
}DT_CALLTYPE, *PDT_CALLTYPE;

#define _THRD_STATUS_FREE		0x0	//thread is free
#define _THRD_STATUS_BISY		0x1	//thread is bisy
#define _THRD_STATUS_EXIT		0x2	//closing thread

typedef struct _tDgsThrd
{
	HANDLE hThread;
	HANDLE hEvent;
	DWORD dwThreadID;

	DWORD dwStatus;
	PVOID pData;
}DGS_THREAD, *PDGS_THREAD;

typedef BOOL (_cdecl *_tDTCallback)(DT_CALLTYPE eCallType, PDGS_THREAD pDogsThread);

BOOL DTInit(_tDTCallback DTCallback, DWORD dwDTinitCount, PVOID pGlobalData);
void DTDone(PVOID pGlobalData);
BOOL DTProceedData(PVOID pEventData);
PDGS_THREAD DTGetFreeThread(DWORD *pFreeCount);	// thread will be marked as bisy


#endif //__DOGS_THREAD