#ifndef __thpimpl__
#define __thpimpl__

/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	thpimpl.h
*		\brief	Thread pool implementation
*		
*		\author Andrew Sobko
*		\date	29.01.2004 14:09:08
*		
*		Example:
	tERROR pr_call ThreadCallback(tThreadCallType CallType, tPTR pCommonThreadContext, tPTR* ppThreadContext)
	{
		tERROR err_ret = errOK;
		switch (CallType)
		{
		case TP_CBTYPE_THREAD_INIT_CONTEXT:
			break;
		case TP_CBTYPE_THREAD_DONE_CONTEXT:
			break;
		case TP_CBTYPE_THREAD_YIELD:
			break;
		}

		return err_ret;
	}

	tERROR pr_call ThreadTaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen)
	{
		switch (CallType)
		{
		case TP_CBTYPE_TASK_INIT_CONTEXT:
			break;
		case TP_CBTYPE_TASK_PROCESS:
			break;
		case TP_CBTYPE_TASK_DONE_CONTEXT:
			break;
		}
		return errOK;
	}

	use()
	{
		THPoolData ThreadPool;

		THP_InitData(&ThreadPool);

		ThreadPool.m_IdleCountMax = 3;
		ThreadPool.m_YieldTimeout = 2000;

		ThreadPool.m_MinThreads = 3;
		ThreadPool.m_MaxThreads = 5;
		ThreadPool.m_DefaultThreadPriority = TP_THREADPRIORITY_ABOVE_NORMAL;

		ThreadPool.m_bAllowTerminating = cTRUE;
		ThreadPool.m_TerminateTimeout = 3000;

		ThreadPool.m_cbThread = ThreadCallback;

		tThreadState state;
		tTaskId task_id;
		tERROR err;

		tPTR pTaskContext;
		tPTR pTaskData;
		tDWORD TaskDataLen;

		if (THP_Start(&ThreadPool))
		{
			char msg[64];
			for (int cou = 0; cou < 1024; cou++)
			{
				wsprintf(msg, "task%d", cou);
				err = THP_AddTask(&ThreadPool, &task_id, ThreadTaskCallback, msg, lstrlen(msg) + 1, 10);
			}

			err = THP_WaitForTaskDone(&ThreadPool, 30);
			state = THP_GetThreadState(&ThreadPool, 1000);

			err = THP_GetThreadContext(&ThreadPool, task_id, &pTaskContext, &pTaskData, &TaskDataLen, &state);
			if (PR_SUCC(err))
				THP_ReleaseThreadContext(&ThreadPool, task_id);
		}

		THP_Stop(&ThreadPool);

		THP_DoneData(&ThreadPool);
	}
*/

//+ ------------------------------------------------------------------------------------------
#include <Prague/prague.h>
#include <Prague/iface/i_threadpool.h>

struct tTHPoolData;
typedef struct _tTHPoolData THPoolData;
typedef THPoolData* PTHPoolData;

//+ ------------------------------------------------------------------------------------------


//! \fn				: THP_InitData
//! \brief			: инициализация данных для пула потоков
//! \return			: void 
//! \param          : PTHPoolData pTHPdata - this
void THP_InitData(PTHPoolData pTHPdata);

//! \fn				: THP_DoneData
//! \brief			: удаление объекта пула потоков
//! \return			: void 
//! \param          : PTHPoolData pTHPdata - this
void THP_DoneData(PTHPoolData pTHPdata);

//! \fn				: THP_Start
//! \brief			: запуск пула
//! \return			: tBOOL - 
//! \param          : PTHPoolData pTHPdata - this
tBOOL THP_Start(PTHPoolData pTHPdata);

//! \fn				: THP_Stop
//! \brief			: остановить пул (дождаться завершения всех заданий и потоков)
//! \return			: void 
//! \param          : PTHPoolData pTHPdata - this
void THP_Stop(PTHPoolData pTHPdata);

//+ ------------------------------------------------------------------------------------------

//! \fn				: THP_AddTask
//! \brief			: добавить задание в пул. при необходимости увеличить количестов потоков
//! \return			: tERROR - если успешно: errOK.. прочие: код ошибки
//! \param          : PTHPoolData pTHPdata - this
//! \param          : tTaskId* pTaskId - возвращаемое значение: идентификатор созданного задания
//! \param          : tThreadTaskCallback pTaskCallback - адрес функции для обработки задания
//! \param          : tPTR pTaskData - данные для задания
//! \param          : tDWORD DataLen - длина данных
//! \param          : tDWORD TaskPriority - приоритет задания (чем больше тем выше)
tERROR THP_AddTask(PTHPoolData pTHPdata, tTaskId* pTaskId, tThreadTaskCallback pTaskCallback, tPTR pTaskData, 
				   tDWORD DataLen,tDWORD TaskPriority);

//! \fn				: THP_GetThreadState
//! \brief			: текущее состояние задачи
//! \return			: tThreadState - статус
//! \param          : PTHPoolData pTHPdata - this
//! \param          : tTaskId TaskId - идентификатор задания (получен в THP_AddTask)
tThreadState THP_GetThreadState(PTHPoolData pTHPdata, tTaskId TaskId);

//! \fn				: THP_WaitForTaskDone
//! \brief			: дождатся завершения указанного задания
//! \return			: tERROR - если успешно: errOK.. прочие: код ошибки
//! \param          : PTHPoolData pTHPdata - this
//! \param          : tTaskId TaskId - идентификатор задания
tERROR THP_WaitForTaskDone(PTHPoolData pTHPdata, tTaskId TaskId);

//! \fn				: THP_GetThreadContext
//! \brief			: дождаться сотсояния потока, когда можно заблокировать доступ к данным и контексту задания
//!						поток будет остановлен до освобождения контекста
//! \return			: tERROR - если успешно: errOK.. прочие: код ошибки
//! \param          : PTHPoolData pTHPdata - this
//! \param          : tTaskId TaskId - идентификатор задания
//! \param          : tPTR* ppTaskContext - контекст задачи
//! \param          : tPTR* ppTaskData - данные задачи
//! \param          : tDWORD* pTaskDatalen - длина данных
//! \param          : tThreadState* pState - текущее состояние потока/задачи
tERROR THP_GetThreadContext(PTHPoolData pTHPdata, tTaskId TaskId, tPTR* ppTaskContext, tPTR* ppTaskData, 
							tDWORD* pTaskDatalen, tThreadState* pState);

//! \fn				: THP_ReleaseThreadContext
//! \brief			: освободить заблокированный контекст в THP_GetThreadContext
//! \return			: void 
//! \param          : PTHPoolData pTHPdata - this
//! \param          : tTaskId TaskId - идентификатор задачи
void THP_ReleaseThreadContext(PTHPoolData pTHPdata, tTaskId TaskId);

//! \fn				: THP_SetThreadPriorityByTask
//! \brief			: установить новый приоритет потока, обрабатывающего указанную задачу
//! \return			: tERROR - если успешно: errOK.. прочие: код ошибки
//! \param          : PTHPoolData pTHPdata - this
//! \param          : tTaskId TaskId - идентифиатор задачи
//! \param          : tThreadPriority ThreadPriority - новый приоритет потока
tERROR THP_SetThreadPriorityByTask(PTHPoolData pTHPdata, tTaskId TaskId, tThreadPriority ThreadPriority);

//! \fn				: RemoveTaskFromQueue
//! \brief			: удаление задания из очереди
//! \return			: tERROR 
//! \param          : tTaskId TaskId
tERROR THP_RemoveTaskFromQueue(PTHPoolData pTHPdata, tTaskId TaskId);

//! \fn				: THP_WaitTaskDoneNotify
//! \brief			: ожидание завершения любого задания и вызов на данном потоке колбэка done
//! \return			: tERROR 
//! \param          : PTHPoolData pTHPdata
//! \param          : tDWORD Timeout
tERROR THP_WaitTaskDoneNotify(PTHPoolData pTHPdata, tDWORD Timeout);

//! \fn				: THP_CreateSeparateThread
//! \brief			: создание потока для отдельного задания
//! \return			: tERROR 
//! \param          : PTHPoolData pTHPdata
//! \param          : pSeparateCallback - адрес поточной функции
//! \param          : SeparateData - данные для задачи
tERROR THP_CreateSeparateThread(PTHPoolData pTHPdata, tThreadTaskCallback pSeparateCallback, tDWORD SeparateData);

//! \fn				: THP_EnumTaskContext
//! \brief			: перебор задач в очереди
//! \return			: tERROR 
//! \param          : PTHPoolData pTHPdata
//! \param          : EnumMode - режим перебора
//! \param          : pTaskId - возвращаемое значение: идентификатор задачи
//! \param          : ppTaskContext - возвращаемое значение: указатель на контекст
//! \param          : ppTaskData - возвращаемое значение: указатель на данные
//! \param          : pTaskDatalen - возвращаемое значение: указатель на длину данных
//! \param          : pState - возвращаемое значение: состояние потока
tERROR THP_EnumTaskContext(PTHPoolData pTHPdata, tThreadTaskEnum EnumMode, tTaskId* pTaskId, tPTR* ppTaskContext, tPTR* ppTaskData, tDWORD* pTaskDatalen, tThreadState* pState);


#if defined (_WIN32)
#include <windows.h>
typedef CRITICAL_SECTION Lock_T;
#else
#include <pthread.h>
typedef pthread_mutex_t Lock_T;
#define INFINITE (tDWORD)-1
#endif

struct  _tTHPoolPublicData {
  tDWORD				m_MinThreads;
  tDWORD				m_MaxThreads;
  tThreadPriority		m_DefaultThreadPriority;
  tBOOL				    m_bAllowTerminating;
  tDWORD				m_TerminateTimeout;
  tDWORD				m_YieldTimeout;
  tDWORD				m_TaskQueue_MaxLen;
  tDWORD				m_IdleCountMax;
  tThreadCallback		m_cbThread;
  tPTR					m_pThreadData;
  tBOOL				    m_bSetInvisible;
  tBOOL				    m_bDelayedDone;
  tBOOL				    m_bQuickDone;
  tBOOL				    m_bQuickExecute;
  Lock_T                m_Lock;
  tCHAR                 m_ThreadPoolName[32];
  _tTHPoolPublicData ()
    : m_MinThreads ( 0 ),
      m_MaxThreads ( 1 ),
      m_DefaultThreadPriority ( TP_THREADPRIORITY_NORMAL ),
      m_bAllowTerminating ( cFALSE ),
      m_TerminateTimeout ( INFINITE ),
      m_YieldTimeout ( 10000 ),
      m_TaskQueue_MaxLen ( 0 ),
      m_IdleCountMax ( 0 ),
      m_cbThread ( 0 ),
      m_pThreadData ( 0 ),
      m_bSetInvisible ( cTRUE ),
	  m_bDelayedDone ( cFALSE ),
	  m_bQuickDone ( cFALSE ),
	  m_bQuickExecute ( cFALSE )
  { m_ThreadPoolName[0] = 0; }
};

struct  _tTHPoolSeparate {
        tThreadTaskCallback m_pSeparateCallback;
        tDWORD              m_SeparateData;
};

#if defined (_WIN32)
#include "thpimpl_win32.h"
#define _TH_LOCK_R(_lock) {EnterCriticalSection(_lock);}
#define _TH_LOCK_W(_lock) {EnterCriticalSection(_lock);}
#define _TH_UNLOCK(_lock) {LeaveCriticalSection(_lock);}
#else
#include "thpimpl_unix.h"
#define _TH_LOCK_R(_lock) {pthread_mutex_lock(_lock);}
#define _TH_LOCK_W(_lock) {pthread_mutex_lock(_lock);}
#define _TH_UNLOCK(_lock) {pthread_mutex_unlock(_lock);}

#endif

typedef enum _thplock_type {
  _thp_lock_n = 0,
  _thp_lock_r = 1,
  _thp_lock_w = 2,
}_thplock_type;

class cthp_autosync
{
 public:
  cthp_autosync( Lock_T* pSync, _thplock_type LockType = _thp_lock_n) {
    m_pSync = pSync;
    switch (LockType) {
    case _thp_lock_w:
      _TH_LOCK_W(m_pSync);
      break;
    case _thp_lock_r:
      _TH_LOCK_R(m_pSync);
      break;
    case _thp_lock_n:
      break;
    }
  }
  
  ~cthp_autosync(){
    _TH_UNLOCK(m_pSync);
  }
 private:
  Lock_T* m_pSync;
};

#endif // __thpimpl__

