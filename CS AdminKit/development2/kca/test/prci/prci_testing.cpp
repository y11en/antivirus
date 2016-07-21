

#ifdef _WIN32
#define STRICT
# include "windows.h"
#endif

#include "build/general.h"

#include "std/base/kldefs.h"
#include "std/thr/thread.h"
#include "std/thrstore/threadstore.h"
#include "std/err/klerrors.h"


#include "kca/prci/componentinstance.h"
#include "kca/prcp/componentproxy.h"

#include <kca/prci/simple_params.h>

#include <map>

#ifdef _WIN32
# include <process.h>
#endif

#include <common/measurer.h>

#define KLCS_MODULENAME L"prci_test"
using namespace KLPRCI;
using namespace KLPAR;
using namespace KLPRCP;
using namespace KLSTD;
using namespace std;

#define PERFTEST_TASK L"PERFTEST_TASK"

class CTestBase
{
public:
	static unsigned long KLSTD_THREADDECL taskrunthread(void *p)
	{
		Trace1(KLCS_MODULENAME, L"CTestBase::taskrunthread started...\n");
		KLERR_TRY
			((CTestBase*)p)->Run();
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(1, pError);
		KLERR_ENDTRY
		Trace1(KLCS_MODULENAME, L"...CTestBase::taskrunthread stopped\n");
		return 0;
	};

	CTestBase()
	{
		m_bDone=false;
		m_pThread=NULL;
		KLSTD_CreateThread(&m_pThread);
	};

	virtual ~CTestBase()
	{
		KLERR_BEGIN
			Stop();
		KLERR_END
	};

	void Start()
	{
		//Stop();
		m_bDone=false;
		m_pThread->Start(L"taskrunthread", taskrunthread, this);
	};
	
	void Stop()
	{
		if(m_pThread)
		{
			m_bDone=true;
			m_pThread->Join();
			delete m_pThread;
			m_pThread=NULL;
		};
	};
	virtual void Run()=0;
protected:
	volatile bool	m_bDone;
	KLSTD::Thread*	m_pThread;
};

volatile long g_GlobalCounter=0;

class CTestTask: public CTestBase
{
public:
	CTestTask(
            const std::wstring& wstrTask,
            long idTask,
            ComponentInstance* pInstance,
            Params* pParams,
            volatile long& lFaultCounter)
    :   CTestBase()
    ,   m_wstrTask(wstrTask)
    ,   m_idTask(idTask)
    ,   m_pInstance(pInstance)
    ,   m_pParams(pParams)
    ,   m_bChangedParams(false)
    ,   m_lFaultCounter(lFaultCounter)
	{
		KLSTD_InterlockedIncrement(&g_GlobalCounter);
        KLSTD_CreateCriticalSection(&m_pCS);
		Start();
	};

	virtual ~CTestTask()
	{
		Stop();
		Trace1(KLCS_MODULENAME, L"task %d stopped\n", m_idTask);
	};

	void OnControl(TaskAction action)
	{
		switch(action)
		{
		case ACTION_SUSPEND:
		case ACTION_RESUME:
		case ACTION_STATUS:
			KLSTD_THROW(STDE_NOFUNC);
			break;
		case ACTION_STOP:
			{				
				Trace1(KLCS_MODULENAME, L"task %d: ACTION_STOP was got - stopping\n", m_idTask);
				m_bDone=true;
			};
			break;
        case ACTION_RELOAD:
            {
                KLSTD_TRACE0(1, L"ACTION_RELOAD\n");
                AutoCriticalSection acs(m_pCS);

                CAutoPtr<ProductTasksControl> pTasks;
                m_pInstance->GetTasksControl(&pTasks);

                CAutoPtr<Params> pNewParams;
                pTasks->GetTaskParams(m_idTask, &pNewParams);
                KLSTD_ASSERT(m_pParams->Compare(pNewParams) != 0);
                m_pParams=pNewParams;
                m_bChangedParams=true;
                pTasks->SetState(m_idTask, TASK_RELOADING, NULL);
            };
            break;
		default:
			KLSTD_THROW_BADPARAM(action);
		};
	};

	void Run()
	{
		ThreadObjectsAutoCleaner tc(KLSTD_GetGlobalThreadStore());
		CAutoPtr<ProductTasksControl> pTaskCtrl;
		KLERR_TRY
			m_pInstance->GetTasksControl(&pTaskCtrl);
			pTaskCtrl->SetState(m_idTask, TASK_RUNNING);
			for(int i=0; i < 20 && !m_bDone; ++i)
			{
				KLSTD_Sleep(500);
                if(m_bChangedParams)
                {
                    KLSTD_TRACE0(1, L"TaskReload detected !\n");
                    AutoCriticalSection acs(m_pCS);
                    m_bChangedParams=false;

                    CAutoPtr<ProductTasksControl> pTasks;
                    m_pInstance->GetTasksControl(&pTasks);                    
                    
                    CAutoPtr<Params> pNewParams;
                    pTasks->GetTaskParams(m_idTask, &pNewParams);
                    //KLSTD_ASSERT(m_pParams->Compare(pNewParams) == 0);
                    pTasks->SetState(m_idTask, TASK_RUNNING, NULL);
                };
			};
			Trace1(KLCS_MODULENAME, L"Stopping task %d\n", m_idTask);
		KLERR_CATCH(pError)
			KLSTD_InterlockedIncrement(&m_lFaultCounter);
		KLERR_FINALLY
			if(pTaskCtrl)
				pTaskCtrl->SetState(m_idTask, TASK_COMPLETED);
			KLSTD_InterlockedDecrement(&g_GlobalCounter);
		KLERR_RETHROW();
		KLERR_ENDTRY
	};

protected:
    volatile long&                  m_lFaultCounter;
	const std::wstring				m_wstrTask;
	const long						m_idTask;
	ComponentInstance*				m_pInstance;
	CAutoPtr<Params>				m_pParams;
    CAutoPtr<CriticalSection>       m_pCS;
    bool                            m_bChangedParams;
};

class CTestComponent: public CTestBase
{
public:
	CTestComponent(ComponentId id, Semaphore* pSemaphore, volatile long& lTasksFaults)
        :   CTestBase()
        ,   m_ComponentId(id)
        ,   m_pSemaphore(pSemaphore)
        ,   m_lTasksFaults(lTasksFaults)

	{
		Trace1(KLCS_MODULENAME, L"Creating ComponentInstance...\n");
		KLPRCI_CreateComponentInstance(m_ComponentId, &this->m_pInstance);
		Trace1(KLCS_MODULENAME, L"...creating ComponentInstance OK\n");

		m_pInstance->SetInstanceControlCallback(this, OnInstanceControl);
		CAutoPtr<ProductTasksControl> pTasksControl;
		m_pInstance->GetTasksControl(&pTasksControl);
		pTasksControl->SetRunTasksCallback(this, OnRunTasks);
		pTasksControl->SetTasksControlCallback(this, OnTasksControl);		
		m_pInstance->SetState(KLPRCI::STATE_RUNNING);
		Start();
	};
	virtual ~CTestComponent()
	{
		AutoCriticalSection asc(m_pCS);		
		m_pInstance->SetInstanceControlCallback(NULL, NULL);
		CAutoPtr<ProductTasksControl> pTasksControl;
		m_pInstance->GetTasksControl(&pTasksControl);
		pTasksControl->SetRunTasksCallback(this, NULL);
		pTasksControl->SetTasksControlCallback(this, NULL);
		Stop();
		Trace1(KLCS_MODULENAME, L"Component stopped\n");
		while(m_tasks.size())
		{
			CTestTask* pTask=m_tasks.begin()->second;
			delete pTask;
			m_tasks.erase(m_tasks.begin());
		};
		m_tasks.clear();
	};
	static void OnInstanceControl(void* context, InstanceAction action)
	{
		switch(action)
		{
		case INSTANCE_SUSPEND:
		case INSTANCE_RESUME:
			KLSTD_THROW(STDE_NOFUNC);
			break;
		case INSTANCE_STOP:
			{
				Trace1(KLCS_MODULENAME, L"INSTANCE_STOP was got - stopping\n");
				CTestComponent* pThis=(CTestComponent*)context;
				pThis->m_bDone=true;
				pThis->m_pInstance->SetState(KLPRCI::STATE_SHUTTING_DOWN);
			};
			break;
		default:
			KLSTD_THROW_BADPARAM(action);
		};
	};
	static void OnRunTasks(
					void*					context,
					const std::wstring&		taskName,
				    KLPAR::Params*	params,
					long					taskId,
					long					timeout)
	{
		KLSTD_ASSERT(context);
		CTestComponent* pThis=(CTestComponent*)context;
		AutoCriticalSection asc(pThis->m_pCS);
		tasks_t::iterator it=pThis->m_tasks.find(taskId);
		if(it!=pThis->m_tasks.end())KLSTD_THROW(STDE_EXIST);
        if(taskName != PERFTEST_TASK)
        {
		    CTestTask* pTask=new CTestTask(taskName,
								    taskId,
								    pThis->m_pInstance,
								    (Params*)params,
                                    pThis->m_lTasksFaults);
		    pThis->m_tasks[taskId]=pTask;
        }
        else
        {
            KLERR_TRY
                CAutoPtr<ProductTasksControl> pTaskControl;
                pThis->m_pInstance->GetTasksControl(&pTaskControl);
                KLSTD_ASSERT(pTaskControl!=NULL);
                pTaskControl->SetState(taskId, TASK_COMPLETED);
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(1, pError);
                KLSTD_ASSERT(false);
                KLERR_RETHROW();
            KLERR_ENDTRY
        };
	};

    static void OnTasksControl(
					void*		context,
					long		taskId,
					TaskAction	action)
	{
		KLSTD_ASSERT(context);
		CTestComponent* pThis=(CTestComponent*)context;
		AutoCriticalSection asc(pThis->m_pCS);
		tasks_t::iterator it=pThis->m_tasks.find(taskId);
		if(it==pThis->m_tasks.end())KLSTD_THROW(STDE_NOTFOUND);
		it->second->OnControl(action);
	};

	static void OnSettingsChangeCallback(
			void*				context,
			const std::wstring&	productName, 
			const std::wstring&	version,
			const std::wstring&	section, 
			const std::wstring&	parameterName,
			const KLPAR::Value*	oldValue,
			const KLPAR::Value*	newValue )
	{
		KLSTD::Semaphore * pSem = (KLSTD::Semaphore *)context;

		Trace1(
			KLCS_MODULENAME,
			L"Settings Changed (%ls->%ls->%ls->%ls)\n",
			productName.c_str(),
			version.c_str(),
			section.c_str(),
			parameterName.c_str());

		pSem->Post();
	};


	void Run()
	{
        m_pSemaphore->Wait(KLSTD_INFINITE);
		Trace1(KLCS_MODULENAME, L"Testing method ProductStatistics::SetStatisticsData...\n");
		{
			for(int i=0; i < 100; ++i)
			{
				event_param_t params[]=
				{
					event_param_t(L"Ten",	Value::INT_T,		(long)rand()),
					event_param_t(L"One",		Value::INT_T,	(long)rand()),
					event_param_t(L"Two",		Value::INT_T,	(long)rand())
				};

				CAutoPtr<Params> pParams;
				makeParamsBody(params, KLSTD_COUNTOF(params), &pParams);
				CAutoPtr<ProductStatistics> pStat;
				m_pInstance->GetStatistics(&pStat);
				pStat->SetStatisticsData(pParams);
				pStat=NULL;
				KLSTD_Sleep(100);				
			};
		};
		Trace1(KLCS_MODULENAME, L"...testing method ProductStatistics::SetStatisticsData OK\n");
		;
		;
		Trace1(KLCS_MODULENAME, L"Testing method ComponentInstance::Publish...\n");
		m_pInstance->Publish(L"TestEvent", NULL, 0);
		Trace1(KLCS_MODULENAME, L"...testing method ComponentInstance::Publish OK\n");
		;        
		Trace1(KLCS_MODULENAME, L"Start event bombing...\n");
		int nEvents=0;
		while(!m_bDone)
		{
			// Bombing with events
            /*
            if(nEvents < 100)
            {
                m_pInstance->Publish(L"TestEvent2", NULL, 0);
			    ++nEvents;
            };
            */
#ifdef _WIN32
            long lTimeout = long(GetVersion()) < 0 ? 1000 : 100;
#else
			long lTimeout = 1000;
#endif
			KLSTD_Sleep(lTimeout);
		};
		Trace1(KLCS_MODULENAME, L"...event bombing OK (%d events)\n", nEvents);
		;
		Trace1(KLCS_MODULENAME, L"Testing method ProductSettings::SubscribeOnSettingsChange...\n");
		CAutoPtr<KLSTD::Semaphore> pSem;		
		CAutoPtr<ProductSettings> pSettings;
		KLPRCI::HSUBSCRIBE		hSubscription=NULL;
		const std::wstring c_wstrSectionName=KLSTD_CreateLocallyUniqueString();
		KLERR_TRY
			m_pInstance->GetSettings(&pSettings);
			KLSTD_CreateSemaphore( &pSem, 0 );		
			
			
			pSettings->SubscribeOnSettingsChange(
							m_ComponentId.productName,
							m_ComponentId.version,
							c_wstrSectionName,
							L"",
							pSem,
							OnSettingsChangeCallback,
							hSubscription);
			Trace1(KLCS_MODULENAME, L"Testing method ProductSettings::NotifyAboutSettingsChange...\n");
			pSettings->NotifyAboutSettingsChange(
				m_ComponentId.productName,
				m_ComponentId.version,
				c_wstrSectionName,
				L"",
				NULL,
				NULL);

			if(pSem->Wait()==false)
				Trace1(
					KLCS_MODULENAME,
					L"...testing method ProductSettings::NotifyAboutSettingsChange FAILED (possibly agent isn't running)\n");
			else
				Trace1(
					KLCS_MODULENAME,
					L"...testing method ProductSettings::NotifyAboutSettingsChange OK\n");
			;
			Trace1(KLCS_MODULENAME, L"...testing method ProductSettings::SubscribeOnSettingsChange OK\n");
		KLERR_CATCH(pError)
			Trace1(KLCS_MODULENAME, L"...FAILED testing method ProductSettings::SubscribeOnSettingsChange (%ls)\n", pError?pError->GetMsg():L"Unknown");			
		KLERR_FINALLY
			if(pSettings && hSubscription)
			{
				pSettings->CancelSettingsChangeSubscription(hSubscription);
			};
		KLERR_ENDTRY
		;
		Trace1(KLCS_MODULENAME, L"Waiting for stopping all tasks...\n");
		while(g_GlobalCounter > 0)
			KLSTD_Sleep(500);
		KLSTD_ASSERT(g_GlobalCounter== 0);
		Trace1(KLCS_MODULENAME, L"...Waiting for stopping all tasks OK\n");
	};
protected:
    volatile long                                   m_lTasksFaults;
    CAutoPtr<Semaphore>                             m_pSemaphore;
	CAutoPtr<ComponentInstance>						m_pInstance;
	CAutoPtr<CriticalSection>						m_pCS;
	typedef std::map<long, CTestTask*>	tasks_t;
	tasks_t											m_tasks;
	const ComponentId								m_ComponentId;
};

void MyEventCallback(
					 const KLPRCI::ComponentId & id,
					 const std::wstring & eventType,
					 const KLPAR::Params * eventBody,
					 time_t time,
					 void * context)
{
	Trace1(KLCS_MODULENAME, L"Event \"%ls\" was caught\n", eventType.c_str());
	Semaphore* pSemaphore=(Semaphore*)context;
	pSemaphore->Post();
};

void MyEventCallback2(
					 const KLPRCI::ComponentId & id,
					 const std::wstring & eventType,
					 const KLPAR::Params * eventBody,
					 time_t time,
					 void * context)
{
	;//Trace1(KLCS_MODULENAME, L"Event \"%s\" was caught in callback #%d\n", eventType.c_str(), context);
};

void testPRCI()
{
	ThreadObjectsAutoCleaner tc(KLSTD_GetGlobalThreadStore());
	const ComponentId id(
			KLCS_PRODUCT_ADMINKIT,
			KLCS_VERSION_ADMINKIT,
			L"test",
			KLPRCI_CreateInstanceId(NULL));

//	KLTR_GetTransport()->SetTimeouts(10000000, 10000000, 1000);		
    volatile long lTasksFaults = 0;
    CAutoPtr<Semaphore> pMainSyncSem;
    KLSTD_CreateSemaphore(&pMainSyncSem, 0);    
	KLERR_TRY        
		CTestComponent test(id, pMainSyncSem, lTasksFaults);
		CAutoPtr<ComponentProxy> pProxy;
        CAutoPtr<Semaphore> pSemaphore;
        HSUBSCRIBE hSubscribe=NULL;
		KLERR_TRY
		    Trace1(KLCS_MODULENAME, L"Creating ComponentProxy...\n");
		    KLERR_TRY
			    KLPRCP_CreateComponentProxy(id, &pProxy);			
		    KLERR_CATCH(pError)
			    Trace1(KLCS_MODULENAME, L"...creating ComponentProxy FAILED\n");
			    KLERR_RETHROW();
		    KLERR_ENDTRY
		    Trace1(KLCS_MODULENAME, L"...creating ComponentProxy OK\n");
            ;
            ;
		    Trace1(KLCS_MODULENAME, L"Testing method ComponentProxy::Subscribe...\n");
		    
		    KLSTD_CreateSemaphore(&pSemaphore, 0);
		    
		    pProxy->Subscribe(L"TestEvent", NULL, MyEventCallback, pSemaphore, hSubscribe);
            pMainSyncSem->Post();
            
		    {
			    for(int j=0; j < 10; ++j){
				    HSUBSCRIBE hSubscribe2=NULL;
				    pProxy->Subscribe(L"TestEvent2", NULL, MyEventCallback2, (void*)j, hSubscribe2);
			    };
		    };
		    Trace1(KLCS_MODULENAME, L"...testing method ComponentProxy::Subscribe OK\n");
            ;
            ;
            KL_TMEASURE_BEGIN(L"running 100 tasks asynchronously", 1);
            for(int t=0; t < 100; ++t)
            {
                long taskId=pProxy->RunTask(PERFTEST_TASK, NULL);
                KLSTD_ASSERT(taskId > 0);
            };
            KL_TMEASURE_END();
		    ;
            KL_TMEASURE_BEGIN(L"running 100 tasks synchronously", 1);
            for(int t=0; t < 100; ++t)
            {
                CAutoPtr<Params> pData;
                long taskId=pProxy->RunTaskSync(PERFTEST_TASK, NULL, &pData);
                KLSTD_TRACE1(3, L"pProxy->RunTaskSync called, t = %u\n", t);
                KLSTD_ASSERT(taskId > 0);
            };
            KL_TMEASURE_END();
            ;
		    Trace1(KLCS_MODULENAME, L"Testing method GetState...\n");
		    ComponentInstanceState state=pProxy->GetState();
		    Trace1(KLCS_MODULENAME, L"...testing method GetState OK (%d)\n", state);
		    ;
		    Trace1(KLCS_MODULENAME, L"Testing remote method GetStatistics...\n");
		    {
			    event_param_t params[]=
			    {
				    event_param_t(L"Ten",		Value::INT_T,	(long)0),
				    event_param_t(L"One",		Value::INT_T,	(long)0),
				    event_param_t(L"Two",		Value::INT_T,	(long)0)
			    };
			    CAutoPtr<Params> pParams;
			    makeParamsBody(params, KLSTD_COUNTOF(params), &pParams);
    //			KLPAR_SerializeToFileName(L"c:/dbg1.xml", pParams);
			    for(int i=0; i < 100; ++i){
				    pProxy->GetStatistics(pParams);
				    CAutoPtr<IntValue> pVal;

				    pParams->GetValue(L"Ten", (Value**)&pVal);
				    KLPAR_CHKTYPE(pVal, INT_T, L"Ten");	
				    Trace3(KLCS_MODULENAME, L"%ls=%d\n", L"Ten", pVal->GetValue());

				    pVal=NULL;
				    pParams->GetValue(L"One", (Value**)&pVal);
				    KLPAR_CHKTYPE(pVal, INT_T, L"One");
				    Trace3(KLCS_MODULENAME, L"%ls=%d\n", L"One", pVal->GetValue());

				    pVal=NULL;
				    pParams->GetValue(L"Two", (Value**)&pVal);
				    KLPAR_CHKTYPE(pVal, INT_T, L"Two");
				    Trace3(KLCS_MODULENAME, L"%ls=%d\n\n", L"Two", pVal->GetValue());

				    KLSTD_Sleep(100);
			    };
		    };
		    Trace1(KLCS_MODULENAME, L"...testing remote method GetStatistics OK\n");
		    ;
		    Trace1(KLCS_MODULENAME, L"Testing method RunTask...\n");
		    const int nTasks=10;
		    for(int i=0; i < nTasks; ++i)
		    {
			    wchar_t szwTaskName[16]=L"";
			    KLSTD_SWPRINTF(szwTaskName, KLSTD_COUNTOF(szwTaskName), L"Task%d", i+1);
			    event_param_t params[]=
			    {
				    event_param_t(L"Ten",		Value::INT_T,		(long)10),
				    event_param_t(L"One",		Value::INT_T,		(long)1),
				    event_param_t(L"Two",		Value::INT_T,		(long)2)
			    };
			    CAutoPtr<Params> pParams;
			    makeParamsBody(params, KLSTD_COUNTOF(params), &pParams);

			    //long taskId=pProxy->RunTaskSync(szwTaskName, pParams, NULL);
			    //long taskId=
			    long taskId=pProxy->RunTask(szwTaskName, pParams);
                {
			        event_param_t params[]=
			        {
				        event_param_t(L"Ten",		Value::INT_T,		(long)10),
				        event_param_t(L"One",		Value::INT_T,		(long)1),
				        event_param_t(L"Two",		Value::INT_T,		(long)2),
                        event_param_t(L"Five",		Value::INT_T,		(long)5),
			        };
			        CAutoPtr<Params> pParams;
			        makeParamsBody(params, KLSTD_COUNTOF(params), &pParams);
                    pProxy->ReloadTask(taskId, pParams);
                };
		    };
		    Trace1(KLCS_MODULENAME, L"...testing method RunTask OK (%d)\n", state);
		    ;
		    Trace1(KLCS_MODULENAME, L"Testing method GetTasksList...\n");
		    std::vector<long> tasks;
		    pProxy->GetTasksList(tasks);
		    //if((int)tasks.size()!=nTasks)
			//    KLSTD_THROW(STDE_GENERAL);
		    for(int j=0; j < nTasks/2; ++j)
			    pProxy->StopTask(tasks[j]);
		    Trace1(KLCS_MODULENAME, L"...testing method GetTasksList OK (%d)\n", state);
		    ;
		    Trace1(KLCS_MODULENAME, L"Waiting for event...\n");
		    pSemaphore->Wait(KLSTD_INFINITE);
		    Trace1(KLCS_MODULENAME, L"...waiting for event OK\n");
            KLSTD_ASSERT_THROW(lTasksFaults == 0);
		    ;
		    Trace1(KLCS_MODULENAME, L"Testing method StopSubscription()...\n");
			    KLSTD_Sleep(500);
                if(pProxy)
			        pProxy->StopSubscription(hSubscribe);
		    Trace1(KLCS_MODULENAME, L"...testing method StopSubscription() OK\n");
		    ;
		    Trace1(KLCS_MODULENAME, L"Testing method Stop()...\n");
            if(pProxy)
			    pProxy->Stop();
		    Trace1(KLCS_MODULENAME, L"...Testing method Stop()\n");
            ;
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            pProxy = NULL;
            KLERR_RETHROW();
		KLERR_ENDTRY
	KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(1, pError);		
	KLERR_FINALLY
	    KLERR_RETHROW();
	KLERR_ENDTRY
};
