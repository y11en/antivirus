//+ ------------------------------------------------------------------------------------------
#include "thlist.h"
#include "../../CommonFiles/Service/sa.h"
//+ ------------------------------------------------------------------------------------------

//+ ------------------------------------------------------------------------------------------
//+ организация списков
typedef struct _tTHQueue
{
	IMPLIST_ENTRY		m_List;
	tDWORD				m_Count;
	tDWORD				m_StoppingCount;
}THQueue, *PTHQueue;

//+ ------------------------------------------------------------------------------------------
//+ данные задания
typedef struct _tTHPSingleTask
{
	IMPLIST_ENTRY		m_List;
	tTaskId				m_TaskId;
	HANDLE				m_hThread;
	tDWORD				m_Priority;
	tThreadTaskCallback	m_pCallbackFunc;
	tThreadState		m_State;
	tPTR				m_pTaskContext;
	HANDLE				m_hTaskComplete;
	HANDLE				m_hTaskLock;
	tDWORD				m_TaskDataLen;
	tBYTE				m_pTaskData[1];
}HPoolSingleTask, *PHPoolSingleTask;

//+ ------------------------------------------------------------------------------------------
//+ главная структура данных для пула потоков
struct _tTHPoolData : public _tTHPoolPublicData
{
	//	internal data
	tDWORD				m_arrTPr[TP_THREADPRIORITY_TIME_CRITICAL + 1];
	THQueue				m_Threads;
	THQueue				m_Tasks;
	LONG				m_AllocCount;
	tBOOL				m_bExit;
	
	HANDLE				m_hSingleTask;
	HANDLE				m_hExit;
	HANDLE				m_hSingleTaskComplete;

	tBOOL				m_bAllowExitNext;

	tDWORD				m_UnmarkedLen;
	tTaskId				m_TaskIdNext;

	HSA					m_pHSA;
};

struct _tTHPoolSeparateData : public _tTHPoolSeparate {
	HANDLE				m_hSeparateStarted;
};


//+ ------------------------------------------------------------------------------------------
//+ данные потока
typedef struct _tTHDog
{
	IMPLIST_ENTRY		m_List;
	DWORD				m_dwThreadId;
	tThreadState		m_State;
	HANDLE				m_hStart;
	HANDLE				m_hThread;

	tDWORD				m_YieldTimeout;
	tDWORD				m_EmptyCirclesCount;
	PTHPoolData			m_pTHPdata;
}THDog, *PTHDog;
