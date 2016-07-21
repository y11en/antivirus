#if defined (_WIN32)

#include "thpimpl.h"
#include <tchar.h>

#include "../../windows/hook/Hook/FSSync.h"

/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	thpimpl.cpp
*		\brief	Реализация пула потоков для Windows платформ
*		
*		\author Andrew Sobko
*		\date	29.01.2004 14:27:33
*		
*		
*		
*/		

void
CloseHandleImp (
	PHANDLE phHandle
	)
{
	if (phHandle && *phHandle)
	{
		CloseHandle(*phHandle);
		*phHandle = NULL;
	}
}

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	if (!fnIsWow64Process)
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle("kernel32"),"IsWow64Process");
 
    if (fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }

    return bIsWow64;
}

typedef BOOL (WINAPI *LPFN_Wow64DisableWow64FsRedirection) (PVOID* OldValue);

BOOL DisableFSRedirector()
{
	LPFN_Wow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection = NULL;
	fnWow64DisableWow64FsRedirection = (LPFN_Wow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle("kernel32"), "Wow64DisableWow64FsRedirection");
 
    if (fnWow64DisableWow64FsRedirection)
    {
		PVOID pOldValue;
        if (fnWow64DisableWow64FsRedirection(&pOldValue))
            return TRUE;
    }
    
	return FALSE;
}

//+ ------------------------------------------------------------------------------------------

//! \fn				: THDogProc
//! \brief			: прототип поточной функции
//! \return			: DWORD WINAPI 
//! \param          : void* pParam
DWORD WINAPI THDogProc(void* pParam);

//+ ------------------------------------------------------------------------------------------
//+ формирование имени объекта синхронизации
PTCHAR THP_GetEventName(PTCHAR pEventName, PTCHAR Pref)
{
#ifdef _DEBUG
	LARGE_INTEGER qPerfomance;

	QueryPerformanceCounter((LARGE_INTEGER*) &qPerfomance);
	wsprintf(pEventName, "%s_%08X%08X", Pref, qPerfomance.HighPart, qPerfomance.LowPart);		

	return pEventName;
#else
	return NULL;
#endif
}

//+ ------------------------------------------------------------------------------------------
//+ выделение/освобождение памяти
void* THP_Alloc(PTHPoolData pTHPdata, tDWORD size)
{
	void* ptr = HeapAlloc(GetProcessHeap(), 0, size);
	if (ptr)
		InterlockedIncrement(&pTHPdata->m_AllocCount);

	return ptr;
}

void THP_Free(PTHPoolData pTHPdata, void* ptr)
{
	if (0 == ptr)
		return;
	InterlockedDecrement(&pTHPdata->m_AllocCount);
	HeapFree(GetProcessHeap(), 0, ptr);
}

//+ ------------------------------------------------------------------------------------------
// инициализация кольцевой очереди
void THP_QueueInit(PTHQueue pQueue)
{
	_impInitializeListHead(&pQueue->m_List);
	pQueue->m_Count = 0;
	pQueue->m_StoppingCount = 0;
}

HANDLE THP_CreateEvent(PTHPoolData pTHPdata, BOOL bManualReset, BOOL bInitialState, PTCHAR ptchPrefix)
{
	TCHAR ev_name[64]; 
	HANDLE hEv = CreateEvent(SA_Get(pTHPdata->m_pHSA), bManualReset, bInitialState, THP_GetEventName(ev_name, ptchPrefix));
	if (!hEv)
		hEv = CreateEvent(SA_Get(pTHPdata->m_pHSA), bManualReset, bInitialState, NULL);

	return hEv;
}
//+ ------------------------------------------------------------------------------------------
//+ базовая инициализация данных пула
void THP_InitData(PTHPoolData pTHPdata)
{
	pTHPdata->m_arrTPr[0] = THREAD_PRIORITY_IDLE;
	pTHPdata->m_arrTPr[1] = THREAD_PRIORITY_LOWEST;
	pTHPdata->m_arrTPr[2] = THREAD_PRIORITY_BELOW_NORMAL;
	pTHPdata->m_arrTPr[3] = THREAD_PRIORITY_NORMAL;
	pTHPdata->m_arrTPr[4] = THREAD_PRIORITY_ABOVE_NORMAL,
	pTHPdata->m_arrTPr[5] = THREAD_PRIORITY_HIGHEST;
	pTHPdata->m_arrTPr[6] = THREAD_PRIORITY_TIME_CRITICAL;

	THP_QueueInit(&pTHPdata->m_Threads);
	THP_QueueInit(&pTHPdata->m_Tasks);

	pTHPdata->m_AllocCount = 0;
	pTHPdata->m_bExit = cFALSE;

	InitializeCriticalSection(&pTHPdata->m_Lock);
	
	pTHPdata->m_pHSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);

	pTHPdata->m_hSingleTask = THP_CreateEvent(pTHPdata, FALSE, FALSE, _T("THPST"));
	pTHPdata->m_hExit = THP_CreateEvent(pTHPdata, TRUE, FALSE, _T("THPEE"));
	pTHPdata->m_hSingleTaskComplete = THP_CreateEvent(pTHPdata, TRUE, FALSE, _T("THPPC"));
	pTHPdata->m_bAllowExitNext = cTRUE;
	pTHPdata->m_UnmarkedLen = 0;
	pTHPdata->m_TaskIdNext = 0;
}

// освобождение данных пула
void THP_DoneData(PTHPoolData pTHPdata)
{
	DeleteCriticalSection(&pTHPdata->m_Lock);
	if (pTHPdata->m_hSingleTask)
		CloseHandleImp(&pTHPdata->m_hSingleTask);

	if (pTHPdata->m_hExit)
		CloseHandleImp(&pTHPdata->m_hExit);

	if (pTHPdata->m_hSingleTaskComplete)
		CloseHandleImp(&pTHPdata->m_hSingleTaskComplete);

	if (pTHPdata->m_pHSA)
	{
		SA_Destroy(pTHPdata->m_pHSA);
		pTHPdata->m_pHSA = NULL;
	}
}

//+ ------------------------------------------------------------------------------------------
// регистрация нового потока
void THP_RegisterThread(PTHPoolData pTHPdata, PTHDog pDog)
{
	pTHPdata->m_Threads.m_Count++;
	_impInsertTailList(&pTHPdata->m_Threads.m_List, &pDog->m_List);
}

// разрегистрация потока (перед его завершением)
void THP_UnregisterThread(PTHDog pDog, tBOOL bTimeout, tBOOL bProtect = cTRUE)
{
	PTHPoolData pTHPdata = pDog->m_pTHPdata;
	if (cTRUE == bProtect)
		_TH_LOCK_W(&pTHPdata->m_Lock);

	_impRemoveEntryList(&pDog->m_List);
	pTHPdata->m_Threads.m_Count--;
	pTHPdata->m_Threads.m_StoppingCount--;
	if (bTimeout)
		pTHPdata->m_bAllowExitNext = cTRUE;

	if (cTRUE == bProtect)
		_TH_UNLOCK(&pTHPdata->m_Lock);
}

static void SetDebuggerThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
#ifdef _DEBUG
	// See MSDN (Setting a Thread Name (Unmanaged))
	struct tagTHREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		LPCSTR szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	} info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

    // TODO: check structure definition for x64 configuration
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
}

// создать новый поток
tBOOL THP_StartNewThread(PTHPoolData pTHPdata, tBOOL bWaitThreadStart)
{
	PTHDog pDog = (PTHDog) THP_Alloc(pTHPdata, sizeof(THDog));
	if (!pDog)
		return cFALSE;

	_TH_LOCK_W(&pTHPdata->m_Lock);

	HANDLE hThread;
	HANDLE hStart;
	{
		cthp_autosync autoSync(&pTHPdata->m_Lock);
		if (pTHPdata->m_bExit || (pTHPdata->m_Threads.m_Count >= pTHPdata->m_MaxThreads))
		{
			THP_Free(pTHPdata, pDog);
			return cFALSE;
		}

		hStart = pDog->m_hStart = THP_CreateEvent(pTHPdata, FALSE, FALSE, _T("THPDS"));
		if (!pDog->m_hStart)
		{
			THP_Free(pTHPdata, pDog);
			return cFALSE;
		}

		hThread = pDog->m_hThread = CreateThread(NULL, 0, THDogProc, (void*) pDog, CREATE_SUSPENDED, &pDog->m_dwThreadId);
		if (!pDog->m_hThread)
		{
			CloseHandleImp(&pDog->m_hStart);
			THP_Free(pTHPdata, pDog);

			return cFALSE;
		}

		pDog->m_State = TP_THREADSTATE_THREAD_STARTING;
		pDog->m_pTHPdata = pTHPdata;
		pDog->m_YieldTimeout = pTHPdata->m_YieldTimeout;

		THP_RegisterThread(pTHPdata, pDog);

		if (TP_THREADPRIORITY_NORMAL != pTHPdata->m_DefaultThreadPriority)
			SetThreadPriority(hThread, pTHPdata->m_arrTPr[pTHPdata->m_DefaultThreadPriority]);
	}

	HANDLE hEvents[2];
	hEvents[0] = hThread;
	hEvents[1] = hStart;

	ResumeThread(hThread);

	if (bWaitThreadStart)
	{
		// ожидание результатов запуска нового потока
		DWORD WaitResult = WaitForMultipleObjects(countof(hEvents), hEvents, FALSE, INFINITE);
		switch (WaitResult)
		{
		case WAIT_OBJECT_0:
		case WAIT_ABANDONED_0 + 1:
			return cFALSE;
		}
	}

	return cTRUE;
}

// стартовать пул
tBOOL THP_Start(PTHPoolData pTHPdata)
{
	tBOOL bResult = cTRUE;
	if (!pTHPdata->m_hSingleTask)
		return cFALSE;

	for (tDWORD cou = 0; (cou < pTHPdata->m_MinThreads) && (cTRUE == bResult); cou++)
		bResult = THP_StartNewThread(pTHPdata, cTRUE);

	return bResult;
}

// найти свободную задачу с максимальным приоритетом
PHPoolSingleTask THP_GetSingleTask(PTHPoolData pTHPdata, HANDLE hThread)
{
	PHPoolSingleTask pSingleTask = NULL;
	cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

	if (0 == pTHPdata->m_UnmarkedLen)
		return pSingleTask;

	PIMPLIST_ENTRY Flink;
	Flink = pTHPdata->m_Tasks.m_List.Flink;
	PHPoolSingleTask pTaskTmp;

	while(Flink != &pTHPdata->m_Tasks.m_List)
	{
		pTaskTmp = (PHPoolSingleTask) Flink;
		Flink = Flink->Flink;

		if (!pTaskTmp->m_hThread)
		{
			pTaskTmp->m_hThread = hThread;
			pSingleTask = pTaskTmp;
			pTHPdata->m_UnmarkedLen--;

			if (pTHPdata->m_UnmarkedLen)
				SetEvent(pTHPdata->m_hSingleTask);

			pSingleTask->m_State = TP_THREADSTATE_TASK_BEFORE_INIT;

			// изменить приоритет при необходимости
			DWORD ThPriority = GetThreadPriority(hThread);
			if (ThPriority != pTHPdata->m_arrTPr[pTHPdata->m_DefaultThreadPriority])
				SetThreadPriority(hThread, pTHPdata->m_arrTPr[pTHPdata->m_DefaultThreadPriority]);
			break;
		}
	}

	return pSingleTask;
}

// удалить задачу из списка и освободить хендлы
void THP_RemoveTaskFromList(PTHPoolData pTHPdata, PHPoolSingleTask pSingleTask)
{
	if (pSingleTask->m_hTaskComplete)
		CloseHandleImp(&pSingleTask->m_hTaskComplete);

	if (pSingleTask->m_hTaskLock)
		CloseHandleImp(&pSingleTask->m_hTaskLock);

	pTHPdata->m_Tasks.m_Count--;
	_impRemoveEntryList(&pSingleTask->m_List);
}

// освободить связанные с задачей данные
void THP_FreeTask(PTHPoolData pTHPdata, PHPoolSingleTask pSingleTask, tBOOL bProtect = cTRUE)
{
	if (cTRUE == bProtect)
		_TH_LOCK_W(&pTHPdata->m_Lock);

	if(pSingleTask->m_hTaskComplete)
		SetEvent( pSingleTask->m_hTaskComplete );
	
	if (pSingleTask->m_hTaskLock)
		SetEvent(pSingleTask->m_hTaskLock);

	if (!pTHPdata->m_bDelayedDone)
	{
		THP_RemoveTaskFromList(pTHPdata, pSingleTask);
		THP_Free(pTHPdata, pSingleTask);
	}
	
	if (cTRUE == bProtect)
		_TH_UNLOCK(&pTHPdata->m_Lock);
}

// остановить работу пула
void THP_Stop(PTHPoolData pTHPdata)
{
	tDWORD termt;
	tBOOL bAllowTerm;
	HANDLE hThread;
	DWORD dwWaitStatus;

	PIMPLIST_ENTRY Flink;

	PHPoolSingleTask pSingleTask;

	_TH_LOCK_W(&pTHPdata->m_Lock);
	pTHPdata->m_bExit = cTRUE;
	_TH_UNLOCK(&pTHPdata->m_Lock);

	if (pTHPdata->m_bQuickDone)
	{
		// free delayed tasks
		pTHPdata->m_bDelayedDone = cFALSE;		// switch to synchronize mode
		while (cTRUE)
		{
			pSingleTask = THP_GetSingleTask(pTHPdata, GetCurrentThread());
			if (!pSingleTask)
				break;
			pSingleTask->m_pCallbackFunc(TP_CBTYPE_TASK_DONE_CONTEXT, 0, &pSingleTask->m_pTaskContext, 
				pSingleTask->m_pTaskData, pSingleTask->m_TaskDataLen);
			THP_FreeTask(pTHPdata, pSingleTask);
		}
	}
	// wait free task
	_TH_LOCK_W(&pTHPdata->m_Lock);

	while (pTHPdata->m_Tasks.m_Count)
	{
		_TH_UNLOCK(&pTHPdata->m_Lock);

		Sleep(100);
		_TH_LOCK_W(&pTHPdata->m_Lock);
	}
	_TH_UNLOCK(&pTHPdata->m_Lock);

	// command to stop
	_TH_LOCK_W(&pTHPdata->m_Lock);
	{
		if (!pTHPdata->m_hExit)
		{
			_TH_UNLOCK(&pTHPdata->m_Lock);
			return;
		}
		SetEvent(pTHPdata->m_hExit);
	}

	termt = pTHPdata->m_TerminateTimeout;
	bAllowTerm = pTHPdata->m_bAllowTerminating;

	_TH_UNLOCK(&pTHPdata->m_Lock);

	// waiting for exit

	_TH_LOCK_R(&pTHPdata->m_Lock);
	while (pTHPdata->m_Threads.m_Count)
	{
		PTHDog pDog;
		Flink = pTHPdata->m_Threads.m_List.Flink;
		pDog = (PTHDog) Flink;
		DWORD id = pDog->m_dwThreadId;

		hThread = NULL;
		if (!DuplicateHandle(GetCurrentProcess(), pDog->m_hThread, GetCurrentProcess(), &hThread, THREAD_ALL_ACCESS, FALSE, 0))
			hThread = NULL;
			
		_TH_UNLOCK(&pTHPdata->m_Lock);

		if (!hThread)
			dwWaitStatus = 0;
		else
		{
			if (bAllowTerm)
				dwWaitStatus = WaitForSingleObject(hThread, termt);
			else
				dwWaitStatus = WaitForSingleObject(hThread, INFINITE);

			CloseHandleImp(&hThread);
		}

		if (WAIT_TIMEOUT == dwWaitStatus || WAIT_OBJECT_0 == dwWaitStatus)
		{
			_TH_LOCK_W(&pTHPdata->m_Lock);
			Flink = pTHPdata->m_Threads.m_List.Flink;
			while (Flink != &pTHPdata->m_Threads.m_List)
			{
				PTHDog cur = (PTHDog) Flink;
				Flink = Flink->Flink;
				if (pDog == cur && id == cur->m_dwThreadId)
				{
					if (WAIT_TIMEOUT == dwWaitStatus)
					  TerminateThread(pDog->m_hThread, 1);
					//! send TP_THREADSTATE_THREAD_STOPPING?
					THP_UnregisterThread(pDog, cFALSE, cFALSE);
					THP_Free(pTHPdata, pDog);
					break;
				}
			}

			_TH_UNLOCK(&pTHPdata->m_Lock);
		}

		_TH_LOCK_R(&pTHPdata->m_Lock);
	}
	
	_TH_UNLOCK(&pTHPdata->m_Lock);
}

// проверка - может ли текущий поток завершиться. разрешен выход потоков по череди
tBOOL THP_CheckAllowExit(PTHPoolData pTHPdata)
{
	tBOOL bAllow = cFALSE;
	_TH_LOCK_W(&pTHPdata->m_Lock);
	
	if (pTHPdata->m_bAllowExitNext)
	{
		if (pTHPdata->m_Threads.m_Count > pTHPdata->m_MinThreads)
		{
			if (pTHPdata->m_Tasks.m_Count >= pTHPdata->m_Threads.m_Count)
			{
				// exit not allowed
			}
			else
			{
				bAllow = cTRUE;
				pTHPdata->m_Threads.m_StoppingCount++;
				pTHPdata->m_bAllowExitNext = cFALSE;
			}
		}
	}

	_TH_UNLOCK(&pTHPdata->m_Lock);

	return bAllow;
}

//+ ------------------------------------------------------------------------------------------
// установить новый статус для потока
void THP_SetThreadState(PTHDog pDog, tThreadState State)
{
	_TH_LOCK_W(&pDog->m_pTHPdata->m_Lock);
	pDog->m_State = State;
	_TH_UNLOCK(&pDog->m_pTHPdata->m_Lock);
}

// установить статус задачи
void THP_SetTaskState(PTHPoolData pTHPdata, PHPoolSingleTask pSingleTask, tThreadState State)
{
	_TH_LOCK_W(&pTHPdata->m_Lock);
	pSingleTask->m_State = State;
	_TH_UNLOCK(&pTHPdata->m_Lock);
}

// проверка блокировка контекста задачи
void THP_CheckTaskAccess(PTHPoolData pTHPdata, PHPoolSingleTask pSingleTask, tBOOL bSet)
{
	if (pSingleTask->m_hTaskLock)
	{
		if (bSet)
			SetEvent(pSingleTask->m_hTaskLock);
		else
			WaitForSingleObject(pSingleTask->m_hTaskLock, INFINITE);
	}
}

//+ ------------------------------------------------------------------------------------------
// поточные функции

DWORD WINAPI THSeparateProc(void* pParam)
{
	_tTHPoolSeparateData* pSeparate = (_tTHPoolSeparateData*) pParam;

	tThreadTaskCallback pSeparateCallback = pSeparate->m_pSeparateCallback;
	tDWORD SeparateData = pSeparate->m_SeparateData;

	SetEvent(pSeparate->m_hSeparateStarted);

	pSeparateCallback(TP_CBTYPE_TASK_PROCESS, 0, 0, (tPTR) SeparateData, 0);

	return 0;
}

DWORD WINAPI THDogProc(void* pParam)
{
	PTHDog pDog = (PTHDog) pParam;
	PTHPoolData pTHPdata = pDog->m_pTHPdata;

	if (pTHPdata->m_ThreadPoolName[0])
		SetDebuggerThreadName(pDog->m_dwThreadId, pTHPdata->m_ThreadPoolName);

	pDog->m_EmptyCirclesCount = 0;

	tERROR err = errOK;

	if (IsWow64())
		DisableFSRedirector();

	tPTR pThreadContext = 0;
	tBOOL bContextInited = cFALSE;

	if (PR_SUCC(err))
	{
		THP_SetThreadState(pDog, TP_THREADSTATE_THREAD_BEFORE_INIT_);
		if (pTHPdata->m_cbThread)
		{
			THP_SetThreadState(pDog, TP_THREADSTATE_THREAD_INITING);
			err = pTHPdata->m_cbThread(TP_CBTYPE_THREAD_INIT_CONTEXT, pTHPdata->m_pThreadData, &pThreadContext);
			if (PR_SUCC(err))
				bContextInited = TRUE;
		}
		
		THP_SetThreadState(pDog, TP_THREADSTATE_THREAD_AFTER_INIT);
	}

	DWORD dwWaitStatus;
	HANDLE hEvents[2];
	hEvents[0] = pTHPdata->m_hSingleTask;
	hEvents[1] = pTHPdata->m_hExit;

	tBOOL bTimeout = cFALSE;
	if (PR_SUCC(err))
	{
		SetEvent(pDog->m_hStart);
		//+ processing
		tBOOL bExit = cFALSE;
		while (!bExit)
		{
			dwWaitStatus = WaitForMultipleObjects(countof(hEvents), hEvents, FALSE, pDog->m_YieldTimeout);
			switch (dwWaitStatus)
			{
			case WAIT_OBJECT_0:
				pDog->m_EmptyCirclesCount = 0;
				{
					// get single task
					PHPoolSingleTask pTask = THP_GetSingleTask(pTHPdata, pDog->m_hThread);
					while (pTask)
					{
						THP_SetTaskState(pTHPdata, pTask, TP_THREADSTATE_TASK_PROCESSING_INIT);
						
						THP_CheckTaskAccess(pTHPdata, pTask, cFALSE);
						err = pTask->m_pCallbackFunc(TP_CBTYPE_TASK_INIT_CONTEXT, pThreadContext, &pTask->m_pTaskContext, pTask->m_pTaskData, pTask->m_TaskDataLen);

						if (PR_FAIL(err))
							THP_SetTaskState(pTHPdata, pTask, TP_THREADSTATE_TASK_INITFAILED);
						else
						{
							THP_SetTaskState(pTHPdata, pTask, TP_THREADSTATE_TASK_AFTER_INIT);

							THP_CheckTaskAccess(pTHPdata, pTask, cTRUE);
							THP_CheckTaskAccess(pTHPdata, pTask, cFALSE);
							
							THP_SetTaskState(pTHPdata, pTask, TP_THREADSTATE_TASK_PROCESSING);
							pTask->m_pCallbackFunc(TP_CBTYPE_TASK_PROCESS, pThreadContext, &pTask->m_pTaskContext, pTask->m_pTaskData, pTask->m_TaskDataLen);
							THP_SetTaskState(pTHPdata, pTask, TP_THREADSTATE_TASK_BEFORE_DONE);

							THP_CheckTaskAccess(pTHPdata, pTask, cTRUE);

							THP_CheckTaskAccess(pTHPdata, pTask, cFALSE);
							if (!pTHPdata->m_bDelayedDone)
							{
								pTask->m_pCallbackFunc(TP_CBTYPE_TASK_DONE_CONTEXT, pThreadContext, &pTask->m_pTaskContext, pTask->m_pTaskData, pTask->m_TaskDataLen);
								THP_SetTaskState(pTHPdata, pTask, TP_THREADSTATE_TASK_AFTER_DONE);
							}
							else
							{
								_TH_LOCK_W(&pTHPdata->m_Lock);
								SetEvent(pTHPdata->m_hSingleTaskComplete);
								_TH_UNLOCK(&pTHPdata->m_Lock);
							}
						}
						
						THP_CheckTaskAccess(pTHPdata, pTask, cTRUE);

						THP_FreeTask(pTHPdata, pTask);

						pTask = THP_GetSingleTask(pTHPdata, pDog->m_hThread);
					}
				}
				break;
			case WAIT_OBJECT_0 + 1:
				bExit = cTRUE;
				break;
			case WAIT_TIMEOUT:
				if (pTHPdata->m_IdleCountMax)
				{
					if (pTHPdata->m_cbThread)
						pTHPdata->m_cbThread(TP_CBTYPE_THREAD_YIELD, pTHPdata->m_pThreadData, &pThreadContext);

					if (((tDWORD) -1) !=  pTHPdata->m_IdleCountMax)
						pDog->m_EmptyCirclesCount++;
					
					if (pDog->m_EmptyCirclesCount >= pTHPdata->m_IdleCountMax)
					{
						bExit = cTRUE;
						bTimeout = cTRUE;
					}

					if (bExit)
						bExit = THP_CheckAllowExit(pTHPdata);
				}
				else
				{
					bExit = THP_CheckAllowExit(pTHPdata);
					if (cTRUE == bExit)
						bTimeout = cTRUE;
				}
				break;
			//case WAIT_ABANDONED:
			default:
				//! error !!! thread alive when sync handle is dead
				bExit = cTRUE;
				break;
			}
		}
	}
	
	//+ exit

	THP_SetThreadState(pDog, TP_THREADSTATE_THREAD_BEFORE_STOP);
	if (pTHPdata->m_cbThread)
	{
		THP_SetThreadState(pDog, TP_THREADSTATE_THREAD_STOPPING);
		if (bContextInited)
			err = pTHPdata->m_cbThread(TP_CBTYPE_THREAD_DONE_CONTEXT, pTHPdata->m_pThreadData, &pThreadContext);
	}

	THP_SetThreadState(pDog, TP_THREADSTATE_THREAD_STOPPED);

	_TH_LOCK_W(&pTHPdata->m_Lock);
	CloseHandleImp(&pDog->m_hStart);
	CloseHandleImp(&pDog->m_hThread);
	THP_UnregisterThread(pDog, bTimeout, cFALSE);
	THP_Free(pTHPdata, pDog);
	_TH_UNLOCK(&pTHPdata->m_Lock);

	return 0;
}

// найти задачу по идентификатору
PHPoolSingleTask THP_GetTaskById(PTHPoolData pTHPdata, tTaskId TaskId)
{
	PHPoolSingleTask pSingleTask = NULL;

	PIMPLIST_ENTRY Flink;
	Flink = pTHPdata->m_Tasks.m_List.Flink;

	while(Flink != &pTHPdata->m_Tasks.m_List)
	{
		pSingleTask = (PHPoolSingleTask) Flink;
		Flink = Flink->Flink;

		if (pSingleTask->m_TaskId == TaskId)
			return pSingleTask;
	}

	return NULL;
}

//+ ------------------------------------------------------------------------------------------
// добавить задание
tERROR THP_AddTask(PTHPoolData pTHPdata, tTaskId* pTaskId, tThreadTaskCallback pTaskCallback, tPTR pTaskData,
				   tDWORD DataLen, tDWORD TaskPriority)
{
	tTaskId TaskIdTmp = 0;
	if (!pTaskCallback)
		return errPARAMETER_INVALID;

	tBOOL bStartNewThread = cFALSE;
	tBOOL bWaitThreadStart = cFALSE;
	{
		cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

		if (pTHPdata->m_bExit)
			return errOBJECT_ALREADY_FREED;

		if (pTHPdata->m_TaskQueue_MaxLen)
		{
			if (pTHPdata->m_Tasks.m_Count + 1 > pTHPdata->m_TaskQueue_MaxLen)
				return errNOT_OK;
		}

		PHPoolSingleTask pSingleTask = (PHPoolSingleTask) THP_Alloc(pTHPdata, sizeof(HPoolSingleTask) + DataLen);
		if (!pSingleTask)
			return errNOT_ENOUGH_MEMORY;

		pSingleTask->m_hTaskComplete = NULL;

		if (pTHPdata->m_bQuickExecute)
			pSingleTask->m_hTaskLock = NULL;
		else
		{
			pSingleTask->m_hTaskLock = THP_CreateEvent(pTHPdata, FALSE, TRUE, _T("THPTL"));
			if (!pSingleTask->m_hTaskLock)
				return errNOT_OK;
		}
		
		pSingleTask->m_TaskId = pTHPdata->m_TaskIdNext++;
		if (pTaskId)
			*pTaskId = pSingleTask->m_TaskId;
		TaskIdTmp = pSingleTask->m_TaskId;
		pSingleTask->m_hThread = NULL;
		pSingleTask->m_Priority = TaskPriority;
		pSingleTask->m_TaskDataLen = DataLen;
		pSingleTask->m_pCallbackFunc = pTaskCallback;
		pSingleTask->m_State = TP_THREADSTATE_TASK_WAITING_THREAD;
		pSingleTask->m_pTaskContext = 0;
		memcpy(pSingleTask->m_pTaskData, pTaskData, DataLen);

		PIMPLIST_ENTRY Flink;
		Flink = pTHPdata->m_Tasks.m_List.Flink;
		PHPoolSingleTask pTaskTmp;
		tBOOL bLast = cFALSE;

		while(cTRUE)
		{
			if (Flink == &pTHPdata->m_Tasks.m_List)
			{
				bLast = cTRUE;
				break;
			}

			pTaskTmp = (PHPoolSingleTask) Flink;
			if (pTaskTmp->m_Priority < TaskPriority)
				break;
			Flink = Flink->Flink;
		}

		//+ add task to list
		if (!bLast && Flink == pTHPdata->m_Tasks.m_List.Flink)
			_impInsertHeadList(&pTHPdata->m_Tasks.m_List, &pSingleTask->m_List);
		else
			_impInsertHeadList(Flink->Blink, &pSingleTask->m_List);

		pTHPdata->m_Tasks.m_Count++;

		pTHPdata->m_UnmarkedLen++;
		//- add task to list

		// go! go! go!
		SetEvent(pTHPdata->m_hSingleTask);

		if (0 == pTHPdata->m_MaxThreads)
		{
			// allow to add task without THREADS!!!
		}
		else
		{
			if (pTHPdata->m_Threads.m_Count < pTHPdata->m_MaxThreads)
			{
				if (pTHPdata->m_Threads.m_Count - pTHPdata->m_Threads.m_StoppingCount <= pTHPdata->m_Tasks.m_Count)
				{
					bStartNewThread = cTRUE;
					if (0 == pTHPdata->m_Threads.m_Count)
						bWaitThreadStart = cTRUE;
				}
			}
		}
	}

	if (bStartNewThread)
	{
		if (!THP_StartNewThread(pTHPdata, bWaitThreadStart))
		{
			cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);
			if (!pTHPdata->m_Threads.m_Count)
			{
				PHPoolSingleTask pSingleTask = THP_GetTaskById(pTHPdata, TaskIdTmp);
				if (pSingleTask)
				{
					THP_FreeTask(pTHPdata, pSingleTask, cFALSE);
					return errNOT_OK;
				}
			}
		}
	}

	return errOK;
}

// вернуть статус задачи
tThreadState THP_GetThreadState(PTHPoolData pTHPdata, tTaskId TaskId)
{
	cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

	PHPoolSingleTask pSingleTask = THP_GetTaskById(pTHPdata, TaskId);

	if (pSingleTask)
		return pSingleTask->m_State;

	return TP_THREADSTATE_THREAD_NOTFOUND;
}

// ожидание завершения задачи
tERROR THP_WaitForTaskDone(PTHPoolData pTHPdata, tTaskId TaskId)
{
	HANDLE hTaskComplete = NULL;
	if (!pTHPdata->m_bExit)
	{
		cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

		PHPoolSingleTask pSingleTask = THP_GetTaskById( pTHPdata, TaskId );
		if (pSingleTask && TP_THREADSTATE_TASK_AFTER_DONE > pSingleTask->m_State)
		{
			if (!pSingleTask->m_hTaskComplete)
				pSingleTask->m_hTaskComplete = THP_CreateEvent(pTHPdata, TRUE, FALSE, _T("THPTC"));
			if (pSingleTask->m_hTaskComplete)
			{
				DuplicateHandle( GetCurrentProcess(), pSingleTask->m_hTaskComplete,
								GetCurrentProcess(), &hTaskComplete,
								0, FALSE, DUPLICATE_SAME_ACCESS
								);
			}
		}
	}

	if (!hTaskComplete)
		return errNOT_FOUND;

	DWORD dwWaitStatus = WaitForSingleObject( hTaskComplete, INFINITE );

	CloseHandleImp( &hTaskComplete );

	return dwWaitStatus == WAIT_OBJECT_0 ? errOK : errNOT_OK;
}

// получить контекст задачи (с блокировкой потока)
tERROR THP_GetThreadContext(PTHPoolData pTHPdata, tTaskId TaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDatalen, tThreadState* pState)
{
	HANDLE hTaskLock = NULL;
	PHPoolSingleTask pSingleTaskLocked = NULL;
	
	if (pTHPdata->m_bQuickExecute)
		return errOPERATION_CANCELED;

	if (!ppTaskContext || !ppTaskData || !pState || !pTaskDatalen)
		return errPARAMETER_INVALID;
	else
	{
		cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

		if (pTHPdata->m_bExit)
			return errOBJECT_ALREADY_FREED;

		PHPoolSingleTask pSingleTask = THP_GetTaskById(pTHPdata, TaskId);
		
		if (!pSingleTask)
			return errNOT_FOUND;
		
		if (!pSingleTask->m_hTaskLock)
			return errNOT_FOUND;

		if (TP_THREADSTATE_TASK_AFTER_DONE > pSingleTask->m_State)
		{
			pSingleTaskLocked = pSingleTask;
			hTaskLock = pSingleTask->m_hTaskLock;
		}
	}
	
	// object found
	// lock it!

	DWORD dwWaitStatus = WaitForSingleObject(hTaskLock, INFINITE);
	if (WAIT_OBJECT_0 == dwWaitStatus)
	{
		cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);
		PHPoolSingleTask pSingleTask = THP_GetTaskById(pTHPdata, TaskId);
		if (!pSingleTask)
			return errOBJECT_ALREADY_FREED;

		*ppTaskContext = pSingleTaskLocked->m_pTaskContext;
		*ppTaskData = pSingleTaskLocked->m_pTaskData;
		*pTaskDatalen = pSingleTaskLocked->m_TaskDataLen;
		*pState = pSingleTaskLocked->m_State;
		return errOK;
	}

	return errNOT_OK;
}

// освободить поток
void THP_ReleaseThreadContext(PTHPoolData pTHPdata, tTaskId TaskId)
{
	cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

	PHPoolSingleTask pSingleTask = THP_GetTaskById(pTHPdata, TaskId);

	if (pSingleTask)
	{
		if (pSingleTask->m_hTaskLock)
			SetEvent(pSingleTask->m_hTaskLock);
	}
}

// изменить приоритет потока (изменение пройдет только для задания - которое ещё не выполняется)
tERROR THP_SetThreadPriorityByTask(PTHPoolData pTHPdata, tTaskId TaskId, tThreadPriority ThreadPriority)
{
	cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);

	PHPoolSingleTask pSingleTask = THP_GetTaskById(pTHPdata, TaskId);

	if (pSingleTask)
	{
		SetThreadPriority(pSingleTask->m_hThread, pTHPdata->m_arrTPr[pTHPdata->m_DefaultThreadPriority]);
		return errOK;
	}

	return errNOT_FOUND;
}

tERROR THP_RemoveTaskFromQueue(PTHPoolData pTHPdata, tTaskId TaskId)
{
	_TH_LOCK_W(&pTHPdata->m_Lock);
	
	PHPoolSingleTask pTask = THP_GetTaskById(pTHPdata, TaskId);

	if (pTask)
	{
		if (!pTask->m_hThread)
		{
			THP_RemoveTaskFromList(pTHPdata, pTask);
			_TH_UNLOCK(&pTHPdata->m_Lock);		
			
			pTask->m_pCallbackFunc(TP_CBTYPE_TASK_DONE_CONTEXT, 0, &pTask->m_pTaskContext, pTask->m_pTaskData, 
				pTask->m_TaskDataLen);			

			THP_Free(pTHPdata, pTask);
			return errOK;
		}

		_TH_UNLOCK(&pTHPdata->m_Lock);		
		return errLOCKED;
	}

	_TH_UNLOCK(&pTHPdata->m_Lock);
	return errNOT_FOUND;
}
//+ ------------------------------------------------------------------------------------------

tERROR THP_WaitTaskDoneNotify(PTHPoolData pTHPdata, tDWORD Timeout)
{
	return errNOT_IMPLEMENTED;
}

tERROR THP_CreateSeparateThread(PTHPoolData pTHPdata, tThreadTaskCallback pSeparateCallback, tDWORD SeparateData)
{
	{
		cthp_autosync autoSync(&pTHPdata->m_Lock, _thp_lock_w);
		if (pTHPdata->m_bExit)
			return errOBJECT_ALREADY_FREED;
	}

	_tTHPoolSeparateData Separate;

	Separate.m_hSeparateStarted = THP_CreateEvent(pTHPdata, FALSE, FALSE, _T("THPSE"));
	if (!Separate.m_hSeparateStarted)
		return errOBJECT_CANNOT_BE_INITIALIZED;

	Separate.m_pSeparateCallback = pSeparateCallback;
	Separate.m_SeparateData = SeparateData;

	DWORD dwThreadId;
	HANDLE hSeparateThread = CreateThread(NULL, 0, THSeparateProc, &Separate, CREATE_SUSPENDED, &dwThreadId);

	if (!hSeparateThread)
		return errOBJECT_CANNOT_BE_INITIALIZED;

	ResumeThread(hSeparateThread);
	WaitForSingleObject(Separate.m_hSeparateStarted, INFINITE);

	CloseHandleImp(&Separate.m_hSeparateStarted);

	CloseHandleImp(&hSeparateThread);

	return errOK;
}

// перебрать контексты задач (с блокировкой потока)
tERROR THP_EnumTaskContext(PTHPoolData pTHPdata, tThreadTaskEnum EnumMode, tTaskId* pTaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDatalen, tThreadState* pState)
{
	if (pTHPdata->m_bExit)
		return errOBJECT_ALREADY_FREED;

	if (TP_ENUM_ABORT == EnumMode)
	{
		_TH_UNLOCK(&pTHPdata->m_Lock);
		return errOK;
	}

	if (!pTaskId || !ppTaskContext || !ppTaskData || !pState || !pTaskDatalen)
		return errPARAMETER_INVALID;

	if (TP_ENUM_START == EnumMode)
		_TH_LOCK_W(&pTHPdata->m_Lock);
	
	PHPoolSingleTask pSingleTaskFound = NULL;
	PHPoolSingleTask pSingleTask;
		
	PIMPLIST_ENTRY Flink;
	Flink = pTHPdata->m_Tasks.m_List.Flink;

	while(Flink != &pTHPdata->m_Tasks.m_List)
	{
		pSingleTask = (PHPoolSingleTask) Flink;
		Flink = Flink->Flink;
			
		if (TP_ENUM_START != EnumMode)
		{
			if (pSingleTask->m_TaskId == *pTaskId)
				EnumMode = TP_ENUM_START;
			
			continue;
		}
		
		if (TP_THREADSTATE_TASK_BEFORE_DONE > pSingleTask->m_State)
		{
			pSingleTaskFound = pSingleTask;
			break;
		}
	}

	if (!pSingleTaskFound)
	{
		_TH_UNLOCK(&pTHPdata->m_Lock);
		
		return errNOT_FOUND;
	}

	*pTaskId = pSingleTaskFound->m_TaskId;
	*ppTaskContext = pSingleTaskFound->m_pTaskContext;
	*ppTaskData = pSingleTaskFound->m_pTaskData;
	*pTaskDatalen = pSingleTaskFound->m_TaskDataLen;
	*pState = pSingleTaskFound->m_State;

	return errOK;
}

#endif //_WIN32
