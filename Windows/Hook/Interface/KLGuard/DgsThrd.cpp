#include "stdafx.h"
#include "DgsThrd.h"

#define _MAX_THREAD_COUNT	30

#include <stuff\parray.h>
CPArray <DGS_THREAD> ThreadArray(0, 1, false);

extern void* _MM_Alloc(unsigned int nSize);
extern void _MM_Free(void* p);

CRITICAL_SECTION DgsThrd_CrSec;

_tDTCallback g_DTCallBack = NULL;
PVOID g_pGlobalData;

BOOL bExitFromDgsThread = FALSE;

// -----------------------------------------------------------------------------------------
DWORD WINAPI DogsThread(void* pParam)
{
	PDGS_THREAD pThread = (PDGS_THREAD) pParam;
	if (pThread != NULL)
	{
		if (g_DTCallBack(DT_INIT, pThread) == TRUE)
		{
			DWORD dwStatus;
			EnterCriticalSection(&DgsThrd_CrSec);
			ThreadArray.Add(pThread);
			LeaveCriticalSection(&DgsThrd_CrSec);

			if (pThread->dwStatus == _THRD_STATUS_BISY)
				SetEvent(pThread->hEvent);

			dwStatus = WAIT_TIMEOUT;
			while (dwStatus != WAIT_ABANDONED)
			{
				if (pThread->dwStatus == _THRD_STATUS_EXIT)
					dwStatus = WAIT_ABANDONED;
				else
				{
					if (dwStatus == WAIT_OBJECT_0)
					{
						/*char dbgmsg[MAX_PATH];
						wsprintf(dbgmsg, "Thread %x signaled\n", pThread->dwThreadID);
						OutputDebugString(dbgmsg);*/
			
						// processing event ------------------------------------------------------------
						g_DTCallBack(DT_PROCEED, pThread);
						// end processing --------------------------------------------------------------
						pThread->dwStatus = _THRD_STATUS_FREE;
					}
				}
				if (dwStatus != WAIT_ABANDONED)
					dwStatus = WaitForSingleObject(pThread->hEvent, 1000 * 15);
			}
		}
		else
		{
			DbgPrint(1, "i_avpg: WT. init failed\n");
		}
		g_DTCallBack(DT_CLOSE, pThread);

		CloseHandle(pThread->hEvent);
		_MM_Free(pThread);
	}
	return 0;
}
// -----------------------------------------------------------------------------------------
long g_uThreadCount = 0;

PDGS_THREAD DTAddNewThread(DWORD dwStatus, PVOID pUserData, DWORD BasePriority)
{
	PDGS_THREAD pThread;

	if (g_uThreadCount > _MAX_THREAD_COUNT)
		return NULL;

	EnterCriticalSection(&DgsThrd_CrSec);

	pThread = (PDGS_THREAD) _MM_Alloc(sizeof(DGS_THREAD));
	if (pThread != NULL)
	{
		pThread->dwStatus = dwStatus;
		pThread->pData = pUserData;
		
		pThread->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (pThread->hEvent != NULL)
		{
			pThread->hThread = CreateThread(NULL, 0, DogsThread, (void*) pThread, 0, &pThread->dwThreadID);
			InterlockedIncrement(&g_uThreadCount);
			if(pThread->hThread == NULL)
			{
				CloseHandle(pThread->hEvent);
				_MM_Free(pThread);
				pThread = NULL;
			}
			else
			{
				SetThreadPriority(pThread->hThread, BasePriority);
			}
		}
		else
		{
			// can't create event
		}
	}
	else
	{
		// can't alloacate memory
	}

	LeaveCriticalSection(&DgsThrd_CrSec);

	return pThread;
}

BOOL DTInit(_tDTCallback DTCallback, DWORD dwDTinitCount, PVOID pGlobalData)
{
	BOOL bErr = FALSE;

	g_DTCallBack = DTCallback;
	if (g_DTCallBack == NULL)
		return FALSE;

	g_pGlobalData = pGlobalData;

	InitializeCriticalSection(&DgsThrd_CrSec);

	for (DWORD dwCou = 0; (dwCou < dwDTinitCount) && (bErr == FALSE); dwCou++)
	{
		if (DTAddNewThread(_THRD_STATUS_FREE, g_pGlobalData, THREAD_PRIORITY_NORMAL) == NULL)
			bErr = TRUE;
	}
	if (bErr == TRUE)
	{
		DTDone(pGlobalData);
		DbgPrint(1, "DTInit Failed\n");
		return !bErr;
	}
	return TRUE;
}

void DTDone(PVOID pGlobalData)
{
	HANDLE hThread;
	PDGS_THREAD pThread;
	
	bExitFromDgsThread = TRUE;

	EnterCriticalSection(&DgsThrd_CrSec);
	for (DWORD dwCou = 0; dwCou < (DWORD) ThreadArray.Count(); dwCou++)
	{
		pThread = ThreadArray[dwCou];
		if (pThread != NULL)
		{
			hThread = pThread->hThread;
			
			pThread->pData = pGlobalData;
			pThread->dwStatus = _THRD_STATUS_EXIT;
			
			SetEvent(pThread->hEvent);
			if (WaitForSingleObject(hThread, /*INFINITE*/ 1000 * 15) == WAIT_TIMEOUT) //one minutes
			{
				DbgPrint(1, "i_avpg: DTDone: thread timeout elapsed - kill\n");
				__try
				{
					g_DTCallBack(DT_CLOSE, pThread);
					TerminateThread(hThread, 0);
				}
				__except(TRUE)
				{
				}
			}
		}
	}
	
	ThreadArray.RemoveAll();

	LeaveCriticalSection(&DgsThrd_CrSec);
	DeleteCriticalSection(&DgsThrd_CrSec);
}

PDGS_THREAD DTGetFreeThread(DWORD *pFreeCount)	// thread will be marked as bisy
{
	PDGS_THREAD pFreeThread = NULL;
	PDGS_THREAD pThread = NULL;

	*pFreeCount = 0;

	EnterCriticalSection(&DgsThrd_CrSec);
	if (ThreadArray.Count() == 0)
	{
		LeaveCriticalSection(&DgsThrd_CrSec);
		return NULL;
	}
	for (DWORD dwCou = 0; dwCou < (DWORD) ThreadArray.Count(); dwCou++)
	{
		pThread = ThreadArray[dwCou];
		if (pThread != NULL)
		{
			if (pThread->dwStatus == _THRD_STATUS_FREE)
			{
				if (pFreeThread == NULL)
				{
					pFreeThread = pThread;
					pThread->dwStatus = _THRD_STATUS_BISY;
				}
				(*pFreeCount)++;
			}
		}
		else
		{
			DbgPrint(1, "DogsThread: ThreadParam is NULL!(%x)\n", dwCou);
		}
	}
	LeaveCriticalSection(&DgsThrd_CrSec);

	return pFreeThread;
}

BOOL DTProceedData(PVOID pEventData)
{
	BOOL bRetVal = TRUE;

	PDGS_THREAD pFreeThread = NULL;
	DWORD dwFreeThreadCount;

	while ((pFreeThread == NULL) && (bExitFromDgsThread == FALSE))
	{
		pFreeThread = DTGetFreeThread(&dwFreeThreadCount);
		if (pFreeThread == NULL)
		{
			pFreeThread = DTAddNewThread(_THRD_STATUS_BISY, pEventData, THREAD_PRIORITY_NORMAL);
		}
		if (pFreeThread == NULL)
			Sleep(300);
	}

	if (pFreeThread != NULL)
	{
		pFreeThread->pData = pEventData;
		SetEvent(pFreeThread->hEvent);
		/*if (dwFreeThreadCount < 10)
			DbgPrint(1, "DogsThread: Free thread count is %d\n", dwFreeThreadCount);*/
	}
	else
		bRetVal = FALSE;

	return bRetVal;
}