/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	timeoutstore.cpp
 * \author	Andrew Kazachkov
 * \date	21.11.2002 10:58:56
 * \brief	
 * 
 */

#include <build/general.h>
#include <std/err/error.h>
#include <std/tp/threadspool.h>
#include <std/thr/sync.h>
#include <std/err/klerrors.h>
#include <std/trc/trace.h>

#include <std/sch/scheduler.h>
#include <std/sch/errors.h>
#include <std/sch/schedule.h>
#include <std/sch/taskresults.h>
#include <std/sch/taskparams.h>
#include <std/sch/millisecondsschedule.h>
#include <std/tmstg/timeoutstore.h>
#include <std/thr/locks.h>


#include <map>
//#include <string>

using namespace std;
using namespace KLSTD;
using namespace KLSCH;

#define KLCS_MODULENAME L"KLTMSG"

static CPointer<Scheduler>  g_pScheduler;
static long                 g_lTimerId = 0;
static KLSTD::LockCount     g_cRefModule;

namespace KLTMSG
{
	template<class T, class Base, class Notificator>
		class CTimeoutStore: public KLBaseImpl<Base>
	{
		struct item_data_t
		{
			item_data_t()
				:	m_pItem(NULL)
				,	m_nId(0)
				,	m_bProlonged(false)
			{;};

			item_data_t(T* pItem, long nId)
				:	m_pItem(pItem)
				,	m_nId(nId)
                ,	m_bProlonged(false)
			{;};

			item_data_t(const item_data_t& x)
				:	m_pItem(x.m_pItem)
				,	m_nId(x.m_nId)
                ,	m_bProlonged(false)
			{;};

			CAutoPtr<T>		m_pItem;
			long			m_nId;
			bool			m_bProlonged;
		};

		typedef std::map<std::wstring, item_data_t > items_t;
		friend class CTaskParams;		
		class CTaskParams : public TaskParams
		{
		public:
			std::wstring	m_wstrKey;
			CTimeoutStore*	m_pThis;
			TaskId			m_idTask;			

			CTaskParams(const std::wstring& wstrKey, CTimeoutStore* pThis, TaskId idTask)
			{
				m_wstrKey=wstrKey;
				m_pThis=pThis;
				m_idTask=idTask;
			};

			CTaskParams(const CTaskParams& x)
			{
				m_wstrKey=x.m_wstrKey;
				m_pThis=x.m_pThis;
				m_idTask=x.m_idTask;
			};

			TaskParams *Clone()
			{
				return new CTaskParams(*this);
			};
		};

	public:
		CTimeoutStore() : KLBaseImpl<Base>()
		{
			KLSTD_CreateCriticalSection(&m_pCS);
            g_cRefModule.Lock();
		};

        void InternalClean(KLTMSG_REMOVE_REASON nReason)
        {
            for(;;)
            {
                CAutoPtr<T> pItem;                
                KLERR_BEGIN
                    std::wstring wstrKey;
                    {
                        AutoCriticalSection acs(m_pCS);
                        if(!m_Data.size())break;
                        typename items_t::iterator it=m_Data.begin();
                        pItem=it->second.m_pItem;
                        wstrKey=it->first;
                    };
                    if(!wstrKey.empty())
                        InternalRemove(wstrKey, NULL, nReason);
                KLERR_ENDT(1);
                pItem=NULL;
            };
        }
        void Clean()
        {
            InternalClean(KLTMSG_REMOVED_MANUALLY);
        };

		virtual ~CTimeoutStore()
		{
            InternalClean(KLTMSG_REMOVED_DTOR);
			g_cRefModule.Unlock();
		};

		std::wstring Insert(T* pItem, long lTimeout)
		{
			std::wstring wstrID=KLSTD_CreateLocallyUniqueString();
			Insert(pItem, lTimeout, wstrID);
			return wstrID;
		};
		
		void Insert(
				T* pItem,
				long lTimeout,
				const std::wstring& wstrKey)
		{
			KLSTD_ASSERT(pItem && lTimeout > 0);
			KLSTD_CHKINPTR(pItem);
			if(lTimeout <= 0)KLSTD_THROW_BADPARAM(lTimeout);
			CPointer<Task> pTask=KLSCH_CreateTask();
			TaskId idTask=KLSTD_InterlockedIncrement(&g_lTimerId);
			{
				CPointer<MillisecondsSchedule> pSch=KLSCH_CreateMillisecondsSchedule();
				CPointer<CTaskParams> pParams=new CTaskParams(wstrKey, this, idTask);
                KLSTD_CHKMEM(pParams);
				pSch->SetPeriod(lTimeout);
				pTask->SetSchedule(pSch);
				pTask->SetCallback(TaskCallbackProc);
				pTask->SetTaskParams(pParams);
				pTask->SetTaskId(idTask);
			};
			{
				AutoCriticalSection acs(m_pCS);
				if(m_Data.find(wstrKey) != m_Data.end())
					KLSTD_THROW(STDE_EXIST);
                m_Data.insert(KLSTD_TYPENAME items_t::value_type(wstrKey, item_data_t(pItem, idTask)));
				g_pScheduler->AddTask(pTask, idTask);
				KLSTD_TRACE1(3, L"Object \"%ls\" was put into the timeoutstore\n", wstrKey.c_str());
			};
		};


		void GetAt(const std::wstring& wstrKey, T** ppItem)
		{
			KLSTD_CHKOUTPTR(ppItem);
			AutoCriticalSection acs(m_pCS);
			typename items_t::iterator it=m_Data.find(wstrKey);
			if(it==m_Data.end())
				KLSTD_THROW(STDE_NOTFOUND);
			it->second.m_pItem.CopyTo(ppItem);
		};

        bool InternalRemove(
                    const std::wstring& wstrKey,
                    T** ppItem,
                    KLTMSG_REMOVE_REASON nReason)
        {
            bool bResult=false;
			item_data_t data;
			{
				AutoCriticalSection acs(m_pCS);
				typename items_t::iterator it=m_Data.find(wstrKey);
				if(it!=m_Data.end())
				{				
					if(ppItem)
						it->second.m_pItem.CopyTo(ppItem);				
					wstring wstrID=it->first;
					data=it->second;				
					m_Data.erase(it);
					g_pScheduler->DelTask(data.m_nId);
					KLSTD_TRACE1(3, L"Item \"%ls\" was taken out of the timeoutstore\n", wstrID.c_str());
					bResult=true;
				}
			};
            if(bResult)
            {
                Notificator(data.m_pItem, wstrKey, nReason);
            };
			return bResult;
        }

		bool Remove(const std::wstring& wstrKey, T** ppItem=NULL)
		{
            return InternalRemove(wstrKey, ppItem, KLTMSG_REMOVED_MANUALLY);
		};

		void Prolong(const std::wstring& wstrKey)
		{
			AutoCriticalSection acs(m_pCS);
			typename items_t::iterator it=m_Data.find(wstrKey);
			if(it==m_Data.end())
				KLSTD_THROW(STDE_NOTFOUND);
			//CTaskParams* pTask=(CTaskParams*)(TaskParams*)(T*)it->second.m_pItem;			
			it->second.m_bProlonged=true;
		};

	protected:
		items_t						m_Data;
		CAutoPtr<CriticalSection>	m_pCS;

		static Errors TaskCallbackProc(const TaskParams* tParams, TaskResults **tResult )
		{
			KLERR_BEGIN
				CTaskParams* pParams=(CTaskParams*)tParams;
				KLSTD_ASSERT(pParams && pParams->m_pThis);
				CAutoPtr<CTimeoutStore> pThis=pParams->m_pThis;
				std::wstring& wstrKey=pParams->m_wstrKey;
				bool bDoRemove=false;
				{
					AutoCriticalSection acs(pThis->m_pCS);
					typename items_t::iterator it=pThis->m_Data.find(wstrKey);
					if(it!=pThis->m_Data.end())
					{
						if(it->second.m_bProlonged)
							it->second.m_bProlonged=false;
						else
							bDoRemove=true;
					};
				};
                if(bDoRemove)
				    pParams->m_pThis->InternalRemove(
                                            wstrKey,
                                            NULL,
                                            KLTMSG_REMOVED_TIMEOUT);
			KLERR_ENDT(1)
			return KLSCH::ERR_NONE;
		};
	};

	class CWorker : public KLBaseImpl<KLTP::ThreadsPool::Worker>
	{
	public:
		CWorker(Scheduler* pSched)
			:	KLBaseImpl<KLTP::ThreadsPool::Worker>()
			,	m_pSched(pSched)
		{
			;
		};

		virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId )
		{
			KLSTD_ASSERT(m_pSched);
			CPointer<TaskResults> pResults;
			m_pSched->WaitTaskExecution(&pResults, NULL);
			return 0;
		};
	protected:
		Scheduler* m_pSched;
	};


	class CTimerTaskParams : public TaskParams
	{
	public:
		CTimerTaskParams(KLTMSG::TimerCallback* pCallback, long nId)
			:	m_pCallback(pCallback)
			,	m_nId(nId){;};
		CTimerTaskParams(const CTimerTaskParams& x)
			:	m_pCallback(x.m_pCallback)
			,	m_nId(x.m_nId){;};
		virtual ~CTimerTaskParams(){m_pCallback=NULL;};
		virtual TaskParams *Clone(){return new CTimerTaskParams(*this);};

		static Errors TaskCallbackProc(const TaskParams* tParams, TaskResults **tResult )
		{
			CTimerTaskParams* pThis=(CTimerTaskParams*)tParams;
			KLERR_BEGIN
				pThis->m_pCallback->OnTimerCallback(pThis->m_nId);
			KLERR_END
			return ERR_NONE;
		};
	protected:
		KLSTD::CAutoPtr<KLTMSG::TimerCallback>	m_pCallback;
		const long								m_nId;
	};

};

namespace KLTMSG
{
    class KlBaseNotificator
    {
    public:
        inline KlBaseNotificator(
                        KLSTD::KLBase*          pObject,
                        const std::wstring&     wstrKey,
                        KLTMSG_REMOVE_REASON    nReason){;};
    };

    class TimeoutObjectNotificator
    {
    public:
        inline TimeoutObjectNotificator(
                        KLTMSG::TimeoutObject*  pObject,
                        const std::wstring&     wstrKey,
                        KLTMSG_REMOVE_REASON    nReason)
        {
            if(pObject)
                pObject->KLTMSG_OnRemoved(wstrKey, nReason);
        };
    };
}

void KLTMSG_CreateTimeoutStore(KLTMSG::TimeoutStore** ppStore)
{
    *ppStore=new KLTMSG::CTimeoutStore<
                                KLBase,
                                KLTMSG::TimeoutStore,
                                KLTMSG::KlBaseNotificator>;
    KLSTD_CHKMEM(*ppStore);
};

void KLTMSG_CreateTimeoutStore2(KLTMSG::TimeoutStore2** ppStore)
{
	*ppStore=new KLTMSG::CTimeoutStore<
                                KLTMSG::TimeoutObject,
                                KLTMSG::TimeoutStore2,
                                KLTMSG::TimeoutObjectNotificator>;
    KLSTD_CHKMEM(ppStore);
};


static KLSTD::CAutoPtr<KLTMSG::TimeoutStore2>	g_pTmStorage;
static KLSTD::CAutoPtr<KLTMSG::CWorker>			g_pWorker;
static KLTP::ThreadsPool::WorkerId				g_wId=0;

IMPLEMENT_MODULE_INIT_DEINIT(KLTMSG)

IMPLEMENT_MODULE_INIT_BEGIN(KLTMSG)
	g_pScheduler=KLSCH_CreateScheduler();
	g_pScheduler->SetWaitTime(0);
	g_pWorker.Attach(new KLTMSG::CWorker(g_pScheduler));
	KLTMSG_CreateTimeoutStore2(&g_pTmStorage);
	KLTP::ThreadsPool *pPool=KLTP_GetThreadsPool();
	KLSTD_ASSERT(pPool);
	pPool->AddWorker(&g_wId, L"KLTMSG::CWorker", g_pWorker);
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN(KLTMSG)
	KLTP::ThreadsPool *pPool=KLTP_GetThreadsPool();
	KLSTD_ASSERT(pPool);
	pPool->DeleteWorker(g_wId);
	g_wId=0;
	g_pWorker=NULL;
    if(g_pTmStorage)
        g_pTmStorage->Clean();
	g_pTmStorage=NULL;	
    g_cRefModule.Wait();
	g_pScheduler=NULL;
IMPLEMENT_MODULE_DEINIT_END()

KLCSC_DECL void KLTMSG_CleanCommonTimeoutStore()
{
    CAutoPtr<KLTMSG::TimeoutStore2> pStore;
    KLERR_BEGIN
        KLTMSG_GetCommonTimeoutStore(&pStore);
        pStore->Clean();
    KLERR_ENDT(1)
};

void KLCSC_DECL KLTMSG_GetCommonTimeoutStore(KLTMSG::TimeoutStore2** ppStore)
{
	KLSTD_CHKOUTPTR(ppStore);
	g_pTmStorage.CopyTo(ppStore);
	if(!*ppStore)
    {
        KLSTD_NOINIT(KLCS_MODULENAME);
    };
};

KLCSC_DECL long KLTMSG_SetTimer(KLTMSG::TimerCallback* pCallback, long lTimeout, long* plTimer)
{
	KLSTD_CHKINPTR(pCallback);
	KLSTD_CHK(lTimeout, lTimeout > 0);
	CPointer<Task> pTask=KLSCH_CreateTask();
	TaskId idTask=KLSTD_InterlockedIncrement(&g_lTimerId);
	if(plTimer)
		*plTimer=idTask;
	{
		CPointer<MillisecondsSchedule> pSch=KLSCH_CreateMillisecondsSchedule();
		pSch->SetPeriod(lTimeout);
		pTask->SetSchedule(pSch);
		pTask->SetCallback(KLTMSG::CTimerTaskParams::TaskCallbackProc);
        KLTMSG::CTimerTaskParams* p=new KLTMSG::CTimerTaskParams(pCallback, lTimeout);
        KLSTD_CHKMEM(p);
		pTask->SetTaskParams(p);
		pTask->SetTaskId(idTask);
	};
	g_pScheduler->AddTask(pTask, idTask);
	
	return idTask;
};

KLCSC_DECL void KLTMSG_KillTimer(long nTimer)
{
	g_pScheduler->DelTask(nTimer);
};
