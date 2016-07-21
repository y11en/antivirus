#include <std/base/klstd.h>
#include <std/thr/locks.h>
#include <std/sign/chksgn.h>
#include <std/par/errors.h>
#include <kca/prss/settingsstorage.h>
#include <transport/tr/transportproxy.h>
#include <transport/ev/common.h>
#include <transport/ev/eventsource.h>
#include "std/par/params.h"
#include "std/par/helpersi.h"
#include "std/par/par_gsoap.h"

#include "kca/prcp/proxybase.h"
#include "kca/prcp/componentproxy.h"
#include "kca/prcp/internal.h"
#include "kca/prcp/prcp_const.h"

#include "kca/prci/componentinstance.h"
#include "../prci/sink.h"

#include <kca/bl/agentbusinesslogic.h>
#include "std/stress/st_prefix.h"
#include "agent/aginst/agent.h"

#include "kca/prci/cmp_helpers.h"
#include <kca/prcp/errlocids.h>

#include "soapH.h"

#include <vector>
#include <map>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
	#include <windows.h>
	#include <tchar.h>
//	#include <aginst/sersec.h>
	#include <common/wintermprefix.h>
#endif

#ifdef __unix__
#include <wctype.h>
#endif

#ifdef N_PLAT_NLM
# include <in.h>
# include <inet.h>
# include "std/conv/wcharcrt.h"
unsigned long inet_addr( char * );
#endif

/*
	BEGIN DEPENDENCIES
	THIS MODULE DEPENDS ON FOLOWING MODULES:
		KLERR
		KLSTD
		KLPAR
		KLTRAP
		KLEV
		KLPRSS
		KLPRCI
	END DEPENDENCIES
*/

#define KLCS_MODULENAME L"KLPRCP"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLPRCP;
using namespace KLPRCI;
using namespace std;

void KLPRCI_LoadComponent(
					ComponentId&	id,
					KLPAR::Params*		pParameters, 
					std::wstring&		asyncId);

bool KLPRCI_UnloadComponent(ComponentId& id);

namespace KLPRCP
{
    const long c_lMaxComponentStartTimeout = 180000;
    KLSTD::LockCount g_lckModule;
};

IMPLEMENT_MODULE_INIT_DEINIT(KLPRCP)

IMPLEMENT_MODULE_INIT_BEGIN2( KLCSKCA_DECL, KLPRCP)
	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLPRCP::c_errorDescriptions,
		KLSTD_COUNTOF(KLPRCP::c_errorDescriptions));	
	KLERR_InitModuleLocalizationDefaults(
		KLCS_MODULENAME,
		KLPRCP::c_LocErrDescriptions,
		KLSTD_COUNTOF(KLPRCP::c_LocErrDescriptions));
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2( KLCSKCA_DECL, KLPRCP)
    g_lckModule.Wait();
	KLERR_DeinitModuleLocalizationDefaults(KLCS_MODULENAME);
	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);	
IMPLEMENT_MODULE_DEINIT_END()


KLCSKCA_DECL void KLPRCP_TestExcpt(const param_error &error)
{
	if(error.code){
		Error *pError=NULL;
		ExceptionFromSoap(error, &pError);
		throw pError;
	};
};

namespace KLPRCP{

	KLSTD_NOTHROW void ComponentIdForSOAP(
							struct soap*				soap,
							SOAPComponentId&			sid,
							const KLPRCI::ComponentId&	id,
							bool						bCopyStrings) throw()
	{			
		sid.productName=bCopyStrings?soap_strdup(soap, id.productName.c_str()):(wchar_t*)id.productName.c_str();
		sid.version=bCopyStrings?soap_strdup(soap, id.version.c_str()):(wchar_t*)id.version.c_str();
		sid.componentName=bCopyStrings?soap_strdup(soap, id.componentName.c_str()):(wchar_t*)id.componentName.c_str();
		sid.instanceId=bCopyStrings?soap_strdup(soap, id.instanceId.c_str()):(wchar_t*)id.instanceId.c_str();
	};

	KLSTD_NOTHROW void ComponentIdFromSOAP(
							KLPRCI::ComponentId&		id,
							const SOAPComponentId&		sid) throw()
	{
		id.productName=sid.productName;
		id.version=sid.version;
		id.componentName=sid.componentName;
		id.instanceId=sid.instanceId;
	};


	#define CHKRESULT()	(test_excpt(r.error))

	#define PROXY_LOCK()			KLTRAP::TransportConnectionLocker	\
											_locker(this->getTransportProxy())
	#define PROXY_CHECK()			_locker.CheckResult()
	#define PROXY_SOAP()			_locker.Get()

	class CComponentProxy
		:	public ComponentProxy
		,	public CProxyBase
        ,   public KLSTD::ModuleClass
	{
    public:
        KLSTD_CLASS_DBG_INFO(KLPRCP::CComponentProxy);
		CComponentProxy()
			:	KLSTD::ModuleClass(&KLPRCP::g_lckModule)
		{
			::KLSTD_CreateCriticalSection(&m_pCS);
            KLCS_MODULE_INCREMENT_OBJECT_COUNTER(KLPRCP);
		};

		virtual ~CComponentProxy()
		{
            KLCS_MODULE_DECREMENT_OBJECT_COUNTER(KLPRCP);
			ClearConnections();
		};

		void GetInstanceId(std::wstring& wstrID) const
		{
			AutoCriticalSection acs(m_pCS);
			if(m_RemoteComponentID.instanceId.empty())
			{
				KLTRAP::ConvertTransportNameToComponentId(
								(KLPRCI::ComponentId &)m_RemoteComponentID,
								m_wstrRemoteComponentName);
			};			
			wstrID=m_RemoteComponentID.instanceId;
		};

		KLPRCI::ComponentId GetComponentID()
		{
			AutoCriticalSection acs(m_pCS);
			if(m_proxyID.instanceId.empty())
			{
				KLTRAP::ConvertTransportNameToComponentId(
								(KLPRCI::ComponentId &)m_proxyID,
								m_wstrProxyName);
			};			
			return m_proxyID;
		};

		KLPRCI::ComponentId GetRemoteComponentID()
		{
			AutoCriticalSection acs(m_pCS);
			if(m_RemoteComponentID.instanceId.empty())
			{
				KLTRAP::ConvertTransportNameToComponentId(
								(KLPRCI::ComponentId &)m_RemoteComponentID,
								m_wstrRemoteComponentName);
			};			
			return m_RemoteComponentID;
		};

		bool IsValid()
		{
			bool bResult=false;
			KLERR_BEGIN
				bResult=KLTR_GetTransport()->IsConnectionActive(
							m_wstrProxyName.c_str(),
							m_wstrRemoteComponentName.c_str());
			KLERR_ENDT(3)
			return bResult;
		};

		KLTRAP::TransportProxy* getTransportProxy() const 
		{		
			return (CComponentProxy*)this;
		};

		long GetPingTimeout() const
		{
            KLERR_BEGIN
                KLPRSS::ComponentInfo ci;
                KLPRSS_GetComponentInfo(
                            m_RemoteComponentID.productName,
                            m_RemoteComponentID.version,
                            m_RemoteComponentID.componentName,
                            ci);
                return ci.lPingTimeout;
            KLERR_ENDT(3)
            return KLPRCI_DEFAULT_PING_TIMEOUT;
		};
		
		KLPRCI::ComponentInstanceState GetState() const
		{
			PROXY_LOCK();
			klprci_GetStateResponse r;
			soap_default_klprci_GetStateResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_GetState(PROXY_SOAP(), NULL, NULL, pID, r);
			PROXY_CHECK();
			CHKRESULT();
			return (KLPRCI::ComponentInstanceState)r.state;
		};

		void GetStatistics(KLPAR::Params * statistics) const
		{
			PROXY_LOCK();
			KLSTD_CHKINPTR(statistics);
			klprci_GetStatisticsResponse r;
			soap_default_klprci_GetStatisticsResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			param__params statisticsIn;
			ParamsForSoap(PROXY_SOAP(), statistics, statisticsIn);
			soap_call_klprci_GetStatistics(PROXY_SOAP(), NULL, NULL, pID, statisticsIn, r);
			PROXY_CHECK();
			CHKRESULT();
			CAutoPtr<Params> pParamsOut;
			ParamsFromSoap(r.statisticsOut, &pParamsOut);
			statistics->MoveFrom(pParamsOut);			
		};

		void Stop()
		{
			PROXY_LOCK();
			klprci_StopResponse r;
			soap_default_klprci_StopResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_Stop(PROXY_SOAP(), NULL, NULL, pID, r);
			PROXY_CHECK();
			CHKRESULT();
		};

		void Suspend()
		{
			klprci_SuspendResponse r={0};
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			PROXY_LOCK();				
			soap_call_klprci_Suspend(PROXY_SOAP(), NULL, NULL, pID, r);
			PROXY_CHECK();
			CHKRESULT();
		};

		void Resume()
		{
			PROXY_LOCK();
			klprci_ResumeResponse r;
			soap_default_klprci_ResumeResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_Resume(PROXY_SOAP(), NULL, NULL, pID, r);
			PROXY_CHECK();
			CHKRESULT();
		};

		long RunTask(const std::wstring & taskName, const KLPAR::Params * params, const std::wstring& asyncId, long timeout=0)
		{
			CAutoPtr<KLPAR::Params> pParamsToSend=(KLPAR::Params*)params;
			if(!pParamsToSend)
				KLPAR_CreateParams(&pParamsToSend);
			PROXY_LOCK();
			klprci_RunTaskResponse r;
			soap_default_klprci_RunTaskResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			param__params settings;
			ParamsForSoap(PROXY_SOAP(), pParamsToSend, settings);
			soap_call_klprci_RunTask(
						PROXY_SOAP(),
						NULL,
						NULL,
						pID,
						(wchar_t*)taskName.c_str(), 
						settings,
						(wchar_t*)asyncId.c_str(),
						timeout,
						r);
			PROXY_CHECK();
			CHKRESULT();
			return r.taskId;
		};

		void GetTaskName(long taskId, std::wstring& wstrTaskName)
		{
			PROXY_LOCK();
			klprci_GetTaskNameResponse r;
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			soap_call_klprci_GetTaskName(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
			wstrTaskName=r.name;
		};

		class CStartTaskSyncClass
		{
		public:
            CStartTaskSyncClass()
                :   m_nState(TASK_FAILURE)
            {;};
            KLPRCI::TaskState       m_nState;
			CAutoPtr<Semaphore>		pSemaphore;
            std::wstring            asyncId;
			CAutoPtr<Params>		pResult;            
			static void OnEventCallback(
						const KLPRCI::ComponentId & id,
						const std::wstring & eventType,
						const KLPAR::Params * eventBody,
						time_t time,
						void * context)
			{
				KLSTD_ASSERT(context);
				CStartTaskSyncClass* pThis=(CStartTaskSyncClass*)context;
				if(!pThis || !pThis->pSemaphore)return;
				if(eventBody){
					const IntValue* pNewState=(const IntValue*)eventBody->
													GetValue2(c_evpTaskNewState, false);
					CAutoPtr<Params> pResults;
					if(eventBody->DoesExist(c_evpTaskResults))
						pResults=GetParamsValue((Params*)eventBody, c_evpTaskResults);
					if(pNewState->GetType()==Value::INT_T)
					{
						switch(pNewState->GetValue()){
						case TASK_FAILURE:
						case TASK_COMPLETED:
                            pThis->m_nState=(KLPRCI::TaskState)pNewState->GetValue();
							pThis->pResult=pResults;
							pThis->pSemaphore->Post();
							break;
						};
					};
				};
			};
		};


		long RunTaskSync(
				const std::wstring&		taskName,
				const KLPAR::Params*	params,
				KLPAR::Params**			results,
				long					timeout,
				const std::wstring&		asyncID,
                KLPRCI::TaskState*      pTaskState)
		{
            long lResult = 0;            
            if(c_szwAsyncMethod == asyncID)
            {
			    PROXY_LOCK();
			    klprci_RunMethodResponse r;
			    soap_default_klprci_RunMethodResponse(PROXY_SOAP(), &r);
			    wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
                param__params inres;
                KLPAR::ParamsForSoap(PROXY_SOAP(), params, inres);
			    soap_call_klprci_RunMethod(
                            PROXY_SOAP(), 
                            NULL, 
                            NULL, 
                            pID, 
                            const_cast<wchar_t*>(taskName.c_str()), 
                            inres,
                            timeout,
                            r);
                KLPAR::ParamsFromSoap(r.res, results);
			    PROXY_CHECK();
			    CHKRESULT();
            }
            else
            {
			    std::wstring wstrAsyncID=
					    asyncID.empty()?::KLPRCI_CreateInstanceId(NULL):asyncID;
			    param_entry_t pars[]=
			    {
				    param_entry_t(c_evpTaskAsyncID,	Value::STRING_T, wstrAsyncID.c_str())
			    };
			    CAutoPtr<Params> pFilter;
			    CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pFilter);
			    
			    CStartTaskSyncClass			data;
			    data.asyncId=wstrAsyncID;
			    ::KLSTD_CreateSemaphore(&data.pSemaphore, 0);

			    KLPRCI::HSUBSCRIBE			hSubscribe=NULL;
			    KLERR_TRY				
				    Subscribe(
					    c_EventTaskState,
					    pFilter,
					    CStartTaskSyncClass::OnEventCallback,
					    &data,
					    hSubscribe);
				    lResult=RunTask(taskName, params, wstrAsyncID, timeout);
				    if(timeout<=0)timeout=KLSTD_INFINITE;
				    CheckAndWait(data.pSemaphore, timeout);
                    if(results)
				        data.pResult.CopyTo(results);
                    if(pTaskState)
                        *pTaskState=data.m_nState;
			    KLERR_CATCH(pError)
				    KLERR_SAY_FAILURE(2, pError);
			    KLERR_FINALLY
				    if(hSubscribe)
					    StopSubscription(hSubscribe);
				    KLERR_RETHROW();
			    KLERR_ENDTRY
            };			
			return lResult;
		};

		void StopTask(long taskId)
		{
			PROXY_LOCK();
			klprci_TaskStopResponse r;
			soap_default_klprci_TaskStopResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_TaskStop(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
		};

		void SuspendTask(long taskId)
		{
			PROXY_LOCK();
			klprci_TaskSuspendResponse r;
			soap_default_klprci_TaskSuspendResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_TaskSuspend(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
		};

		void ResumeTask(long taskId)
		{
			klprci_TaskResumeResponse r;
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			PROXY_LOCK();
			soap_call_klprci_TaskResume(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
		};
        
        void ReloadTask(long taskId, KLPAR::Params* pNewSettings)
        {
            CAutoPtr<Params> pParamsToSend=pNewSettings;
			if(!pParamsToSend)
				KLPAR_CreateParams(&pParamsToSend);
            klprci_TaskReloadResponse r;
            wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
            PROXY_LOCK();
            param__params pars;
            ParamsForSoap(PROXY_SOAP(), pParamsToSend, pars);
            soap_call_klprci_TaskReload(PROXY_SOAP(), NULL, NULL, pID, taskId, pars, r);            
			PROXY_CHECK();
			CHKRESULT();
        };
		
		void GetTasksList( std::vector<long> & ids) const
		{
			PROXY_LOCK();
			klprci_GetTasksListResponse r;
			soap_default_klprci_GetTasksListResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			soap_call_klprci_GetTasksList(PROXY_SOAP(), NULL, NULL, pID, r);
			PROXY_CHECK();
			CHKRESULT();
			ids.resize(r.taskslist.__size);
			for(int i=0; i < r.taskslist.__size; ++i)
				ids[i]=r.taskslist.__ptr[i];
		};

		void GetTaskParams(
							long			idTask,
							KLPAR::Params*	pFilter, 
							KLPAR::Params**	ppResult)
		{
			KLSTD_CHKOUTPTR(ppResult);
			PROXY_LOCK();
			klprci_GetTasksParamsResponse r;
			soap_default_klprci_GetTasksParamsResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			param__params parsIn;
			ParamsForSoap(PROXY_SOAP(), pFilter, parsIn);
			soap_call_klprci_GetTasksParams(
										PROXY_SOAP(),
										NULL,
										NULL,
										pID,
										idTask,
										parsIn,
										r);
			PROXY_CHECK();
			CHKRESULT();
			ParamsFromSoap(r.pars, ppResult);
		};


		void GetTaskName(long taskId, std::wstring& wstrTaskName) const
		{
			PROXY_LOCK();
			klprci_GetTaskNameResponse r;
			soap_default_klprci_GetTaskNameResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();
			soap_call_klprci_GetTaskName(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
			wstrTaskName=r.name?r.name:L"";
		};

		KLPRCI::TaskState GetTaskState(long taskId) const
		{
			PROXY_LOCK();
			klprci_GetTaskStateResponse r;
			soap_default_klprci_GetTaskStateResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_TaskGetState(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
			return (KLPRCI::TaskState)r.state;
		};

		int GetTaskCompletion(long taskId) const
		{
			PROXY_LOCK();
			klprci_GetTaskCompletionResponse r;
			soap_default_klprci_GetTaskCompletionResponse(PROXY_SOAP(), &r);
			wchar_t* pID=(wchar_t*)m_RemoteComponentID.instanceId.c_str();			
			soap_call_klprci_TaskGetCompletion(PROXY_SOAP(), NULL, NULL, pID, taskId, r);
			PROXY_CHECK();
			CHKRESULT();
			return (KLPRCI::TaskState)r.percent;
		};

		void SubscribeEx(
				const KLPRCI::ComponentId&			subscriber,
				const KLPRCI::ComponentId&			publisher,
				const std::wstring&					eventType,
				const KLPAR::Params*				eventBodyFilter,
				EventCallback						callback,
				void*								context,
				KLPRCI::HSUBSCRIBE&					hSubscribe,
				const KLEV::SubscriptionOptions&	subOptions)
		{
			KLEV::EventSource* pEventSrc=KLEV_GetEventSource();
			KLSTD_ASSERT(pEventSrc);
			CSubscribeSink* pSubscribeSink=new CSubscribeSink(context, callback);
			KLSTD_CHKMEM(pSubscribeSink);

			CAutoPtr<AdviseSink> pSink;
			pSink.Attach(pSubscribeSink);
			{
				AutoCriticalSection cs(m_pCS);
				hSubscribe=m_Subscriptions.addNew(pSink);
			};
			subscribe(subscriber, publisher, eventType, (Params*)eventBodyFilter, pSink, subOptions);
		};

		void Subscribe(
				const std::wstring&		eventType,
				const KLPAR::Params*	eventBodyFilter,
				EventCallback			callback,
				void*					context,
				KLPRCI::HSUBSCRIBE&		hSubscribe)
		{
            KLEV::SubscriptionOptions Options;
            Options.useSubscriberConnectionsOnly = true;
            Options.useOnlyLocalConnection = false;
			SubscribeEx(
				m_proxyID,
				m_RemoteComponentID,
				eventType,
				eventBodyFilter,
				callback,
				context,
				hSubscribe,
				Options);
		};

		void StopSubscription(KLPRCI::HSUBSCRIBE hSubscribe)
		{
            KLSTD::CAutoPtr<KLPRCI::AdviseSink> pAdviseSink;
            {
                AutoCriticalSection cs(m_pCS);
				m_Subscriptions.remove(hSubscribe, &pAdviseSink);
            };
            pAdviseSink = NULL;
        };

        void SetOwner(KLSTD::KLBase*    pOwner)
        {
            m_pOwner = pOwner;
        };
	protected:
		KLPRCI::ComponentId						m_RemoteComponentID, m_proxyID;
		CAdviseSinks<HSUBSCRIBE>				m_Subscriptions;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCS;
        KLSTD::CAutoPtr<KLSTD::KLBase>          m_pOwner;
	};

	class CAgentProxy
		:	public CComponentProxy
		,	public AgentProxy
	{
	public:
		CAgentProxy()
			:	CComponentProxy()
			,	AgentProxy()
		{;};

		void GetInstanceId(std::wstring& wstrID) const
		{
			CComponentProxy::GetInstanceId(wstrID);
		};		

		long GetPingTimeout() const
		{			
			return CComponentProxy::GetPingTimeout();
		};
		
		KLPRCI::ComponentInstanceState GetState() const
		{
			return CComponentProxy::GetState();
		};

		void GetStatistics(KLPAR::Params * statistics) const
		{
			CComponentProxy::GetStatistics(statistics);
		};

		void Stop()
		{
			CComponentProxy::Stop();
		};

		void Suspend()
		{
			CComponentProxy::Suspend();
		};

		void Resume()
		{
			CComponentProxy::Resume();
		};

		long RunTask(const std::wstring & taskName, const KLPAR::Params * params, const std::wstring& asyncId, long timeout=0)
		{
			return CComponentProxy::RunTask(taskName, params, asyncId, timeout);
		};

		long RunTaskSync(
				const std::wstring&		taskName, 
				const KLPAR::Params*	params,
				KLPAR::Params**			results,
				long					timeout,
				const std::wstring&		wstrAsyncID,
                KLPRCI::TaskState*      pTaskState)
		{
			return CComponentProxy::RunTaskSync(
                                taskName,
                                params,
                                results,
                                timeout,
                                wstrAsyncID,
                                pTaskState);
		};


		void StopTask(long taskId)
		{
			CComponentProxy::StopTask(taskId);
		};

		void SuspendTask(long taskId)
		{
			CComponentProxy::SuspendTask(taskId);
		};

		void ResumeTask(long taskId)
		{
			CComponentProxy::ResumeTask(taskId);
		};
		
        void ReloadTask(long taskId, KLPAR::Params* pNewSettings)
        {
            CComponentProxy::ReloadTask(taskId, pNewSettings);
        };

		void GetTasksList( std::vector<long> & ids) const
		{
			CComponentProxy::GetTasksList(ids);
		};

		void GetTaskParams(
							long			idTask,
							KLPAR::Params*	pFilter, 
							KLPAR::Params**	ppResult)
		{
			CComponentProxy::GetTaskParams(idTask, pFilter, ppResult);
		};


		void GetTaskName(long taskId, std::wstring& wstrTaskName) const
		{
			CComponentProxy::GetTaskName(taskId, wstrTaskName);
		};

		KLPRCI::TaskState GetTaskState(long taskId) const
		{
			return CComponentProxy::GetTaskState(taskId);
		};

		int GetTaskCompletion(long taskId) const
		{
			return CComponentProxy::GetTaskCompletion(taskId);
		};

		void Subscribe(
				const std::wstring&		eventType,
				const KLPAR::Params*	eventBodyFilter,
				EventCallback			callback,
				void*					context,
				KLPRCI::HSUBSCRIBE&		hSubscribe)
		{
			CComponentProxy::Subscribe(
									eventType,
									eventBodyFilter,
									callback,
									context,
									hSubscribe);
		};

		void StopSubscription(KLPRCI::HSUBSCRIBE hSubscribe)
		{
			CComponentProxy::StopSubscription(hSubscribe);
		};

		KLPRCI::ComponentId GetComponentID()
		{
			return CComponentProxy::GetComponentID();
		};

		KLPRCI::ComponentId GetRemoteComponentID()
		{
			return CComponentProxy::GetRemoteComponentID();
		};

		bool IsValid()
		{
			return CComponentProxy::IsValid();
		};

        class CStartComponentSyncClass
		{
		public:
			CAutoPtr<Semaphore>		m_pSemaphore;
			std::wstring			m_asyncId;
			std::wstring			m_instanceId;
			bool					m_bFirstTime;
			volatile long			m_lEntered;
            volatile long           m_lSuccessCount;
            bool                    m_bResult;
            CAutoPtr<Error>         m_pError;

			CStartComponentSyncClass()
				:	m_bFirstTime(true)
				,	m_lEntered(0)
                ,   m_lSuccessCount(2)
                ,   m_bResult(true){;};
			static void OnEventCallback(
						const KLPRCI::ComponentId & id,
						const std::wstring & eventType,
						const KLPAR::Params * eventBody,
						time_t time,
						void * context)
			{
				KLSTD_ASSERT(context && eventBody);
				CStartComponentSyncClass* pThis=(CStartComponentSyncClass*)context;
				if(!pThis || !pThis->m_pSemaphore || !eventBody)return;
				if(KLSTD_InterlockedIncrement(&pThis->m_lEntered) != 1)
				{
					KLSTD_ASSERT(false);
					KLSTD_InterlockedDecrement(&pThis->m_lEntered);
					return;
				};
                if(pThis->m_bFirstTime && pThis->m_bResult && pThis->m_lSuccessCount > 0)
                {
				    KLERR_TRY
                        if(eventType == c_EventComponentStarted)
                            KLSTD_InterlockedDecrement(&pThis->m_lSuccessCount);
                        else if(eventType == c_EventComponentShutdowned)
                        {
                            KLSTD_THROW(STDE_UNAVAIL);
                        }
                        else if(eventType == c_EventInstanceState)
                        {
					        long nState=GetIntValue((Params*)eventBody, c_evpTaskNewState);
					        switch(nState)
					        {
					        case STATE_DESTROYED:
					        case STATE_SHUTTING_DOWN:
					        case STATE_FAILURE:
					        case STATE_DEINITIALIZING:
    //					    case STATE_SUSPENDING:
                                pThis->m_bResult=false;
//                                break;
                            case STATE_RUNNING:
                                KLSTD_InterlockedDecrement(&pThis->m_lSuccessCount);
						        pThis->m_instanceId=GetStringValue((Params*)eventBody, c_CID_InstanceId);
						        KLSTD_ASSERT(!pThis->m_instanceId.empty());
                                break;
                            };
                        };
					    if(pThis->m_lSuccessCount == 0)
					    {
						    pThis->m_bFirstTime=false;
						    pThis->m_pSemaphore->Post();
					    };
                        KLSTD_ASSERT_THROW(pThis->m_lSuccessCount >= 0);
				    KLERR_CATCH(pError)
                        pThis->m_bFirstTime=false;
                        pThis->m_pError=pError;
                        pThis->m_bResult=false;
                        pThis->m_pSemaphore->Post();
                    KLERR_ENDTRY
                };
				KLSTD_InterlockedDecrement(&pThis->m_lEntered);
			};
		};

		#define LOGON_MASK 0x000000FF

		void StartComponentAsync_I( 
						KLPRCI::ComponentId&	id,
						KLPAR::Params*			pParameters,
						std::wstring			wstrAsyncId,
						AVP_dword				dwFlags,
						long					lTimeoutStop,
						long					lLogonType,
						const wchar_t*			szwUser,
						const wchar_t*			szwDomain,
						const wchar_t*			szwPassword,
						KLPAR::Params*			pInData,
						KLPAR::Params**			ppOutData)
		{
			PROXY_LOCK();
			klagent_StartComponentExResponse r;
			soap_default_klagent_StartComponentExResponse(PROXY_SOAP(), &r);
			
			SOAPComponentId sid;
			sid.soap_default(PROXY_SOAP());
			ComponentIdForSOAP(PROXY_SOAP(), sid, id, false);

			param__params par, inData;
			ParamsForSoap(PROXY_SOAP(), pParameters, par);
			ParamsForSoap(PROXY_SOAP(), pInData, inData);

			soap_call_klagent_StartComponentEx(
							PROXY_SOAP(),
							NULL,
							NULL,
							sid,
							par,
							(wchar_t*)wstrAsyncId.c_str(),
							dwFlags,
							-1,							
							lTimeoutStop,
							lLogonType,
							(wchar_t*)szwUser,
							(wchar_t*)szwDomain,
							(wchar_t*)szwPassword,
							inData,
							r);
			PROXY_CHECK();
			CHKRESULT();  
			ParamsFromSoap(r.extraOutParams, ppOutData);
		};

		void StartComponentAsync_I( 
						KLPRCI::ComponentId&	id,
						KLPAR::Params*			pParameters,
						std::wstring			wstrAsyncId,
						AVP_dword				dwFlags,
						long					lTimeoutStop,
						long					lLogonType,
						const wchar_t*			szwUser,
						const wchar_t*			szwDomain,
						const wchar_t*			szwPassword)
		{
			KLSTD_USES_CONVERSION;
			KLSTD_ASSERT(pParameters && !wstrAsyncId.empty());
			CAutoPtr<KLPAR::Params> pInData, pOutData;
			KLPAR_CreateParams(&pInData);
			StartComponentAsync_I(
							id,
							pParameters,
							wstrAsyncId,
							dwFlags,
							lTimeoutStop,
							lLogonType,
							szwUser,
							szwDomain,
							szwPassword,
							pInData,
							&pOutData);
			if(dwFlags & SCF_SHAREPROCESS)
				KLPRCI_LoadComponent(id, pParameters, wstrAsyncId);
		};

		// AgentProxy

		void StartComponentEx( 
						KLPRCI::ComponentId&	id,
						KLPAR::Params*			pParameters,
						std::wstring			wstrAsyncId,
						AVP_dword				dwFlags,
						long					lAsyncTimeout,
						long					lTimeoutStop,
						long					lLogonType,
						const wchar_t*			szwUser,
						const wchar_t*			szwDomain,
						const wchar_t*			szwPassword)
		{
            if(KLSTD_GetShutdownFlag())
                KLSTD_THROW(STDE_UNAVAIL);
            ;
			if(wstrAsyncId.empty())
				wstrAsyncId=::KLPRCI_CreateInstanceId(NULL);
            ;
            if(lAsyncTimeout == KLSTD_INFINITE || lAsyncTimeout > c_lMaxComponentStartTimeout)
                lAsyncTimeout = c_lMaxComponentStartTimeout;
            ;
			CAutoPtr<Params> pParameters2;
			if(!pParameters)
				KLPAR_CreateParams(&pParameters2);
			else
				pParameters->Clone(&pParameters2);

			CAutoPtr<BoolValue>		p_bAutoStop;
			CAutoPtr<IntValue>		p_lTimeoutStop;
			CAutoPtr<StringValue>	p_wstrAsyncId;

			CreateValue(wstrAsyncId.c_str(), &p_wstrAsyncId);			
			CreateValue((dwFlags & SCF_AUTOSTOP)!=0, &p_bAutoStop);
			CreateValue(lTimeoutStop, &p_lTimeoutStop);

			pParameters2->ReplaceValue(c_ComponentAsyncId, p_wstrAsyncId);
			pParameters2->ReplaceValue(c_AutoStopAllowed, p_bAutoStop);
			pParameters2->ReplaceValue(c_TimeoutStopAllowed, p_lTimeoutStop);		
			;
			if(dwFlags & SCF_SYNC)
			{
				id.instanceId.clear();//=L"";
//				dwFlags&=~SCF_SYNC;
				param_entry_t params[]=
				{
				   param_entry_t(c_AsyncId, Value::STRING_T, wstrAsyncId.c_str())
				};
				CAutoPtr<Params> pFilter;
				CreateParamsBody(params, KLSTD_COUNTOF(params), &pFilter);
				CStartComponentSyncClass	data;
				data.m_asyncId=wstrAsyncId;
				::KLSTD_CreateSemaphore(&data.m_pSemaphore, 0);
				KLSTD_ASSERT(data.m_pSemaphore);
				KLPRCI::HSUBSCRIBE			hSubscribe=NULL, hSubscribe2=NULL, hSubscribe3=NULL;
				KLERR_TRY
					SubscribeEx(
						id,
						c_EventInstanceState,
						pFilter,
						CStartComponentSyncClass::OnEventCallback,
						&data,
						hSubscribe);

                //! 07.05.2003 We should guarantee that agent already knows about component's start !!!
					Subscribe(
						c_EventComponentStarted,
						pFilter,
						CStartComponentSyncClass::OnEventCallback,
						&data,
						hSubscribe2);

                    Subscribe(
                        c_EventComponentShutdowned,
                        pFilter,
						CStartComponentSyncClass::OnEventCallback,
						&data,
                        hSubscribe3);
				
					StartComponentAsync_I(
									id,
									pParameters2,
									wstrAsyncId,
									dwFlags,
									lTimeoutStop,
									lLogonType,
									szwUser,
									szwDomain,
									szwPassword);
                    CheckAndWait(data.m_pSemaphore, lAsyncTimeout, KLPRCP_WAITTIMESLICE, NULL, true);
					//if(!data.m_pSemaphore->Wait(lAsyncTimeout))
						//KLSTD_THROW(STDE_TIMEOUT);
                    if(!data.m_bResult || data.m_pError)
                    {
                        if(data.m_pError!=NULL)
                            throw data.m_pError.Detach();
                        else
                            KLSTD_THROW(STDE_GENERAL);
                    };
					id.instanceId=data.m_instanceId;
					KLSTD_ASSERT(!id.instanceId.empty());
				KLERR_CATCH(pError)
					KLERR_SAY_FAILURE(1, pError);
				KLERR_FINALLY
					if(hSubscribe)
						StopSubscription(hSubscribe);
                    if(hSubscribe2)
                        StopSubscription(hSubscribe2);
                    if(hSubscribe3)
                        StopSubscription(hSubscribe3);
					KLERR_RETHROW();
				KLERR_ENDTRY;
			}
			else
				StartComponentAsync_I(
								id,
								pParameters2,
								wstrAsyncId,
								dwFlags,
								lTimeoutStop,
								lLogonType,
								szwUser,
								szwDomain,
								szwPassword);
		};

		void StartComponent(
		   const KLPRCI::ComponentId&	id,
		   KLPAR::Params*				pParameters2,
		   std::wstring					asyncId,
		   bool							bAutoStop,
		   bool							bStartInThisProcess,
		   long							lTimeoutStop)
		{
			StartComponentEx(
					(KLPRCI::ComponentId&)id,
					pParameters2,
					asyncId,
					(bStartInThisProcess ? SCF_SHAREPROCESS : 0)|(bAutoStop?SCF_AUTOSTOP:0),
					0,
					lTimeoutStop,
					0,
					NULL,
					NULL,
					NULL);
		};

		void StartComponentSync(
				KLPRCI::ComponentId&		id,
				KLPAR::Params*				pParameters,
				long						timeout,
				bool						bAutoStop,
				bool						bStartInThisProcess,
				long						lTimeoutStop)
		{
			StartComponentEx(
					(KLPRCI::ComponentId&)id,
					pParameters,
					L"",
					SCF_SYNC | (bStartInThisProcess ? SCF_SHAREPROCESS : 0)|(bAutoStop?SCF_AUTOSTOP:0),
					timeout,
					lTimeoutStop,
					0,
					NULL,
					NULL,
					NULL);
		};
		
		void GetComponentsList( const KLPRCI::ComponentId & filter, 
                                std::vector<KLPRCI::ComponentId>& components)
		{
			PROXY_LOCK();
			klagent_GetStartedComponentsListResponse r;
			SOAPComponentId sid;

			soap_default_klagent_GetStartedComponentsListResponse(PROXY_SOAP(), &r);			
			ComponentIdForSOAP(PROXY_SOAP(), sid, filter, false);
			soap_call_klagent_GetStartedComponentsList(PROXY_SOAP(), NULL, NULL, sid, r);
			PROXY_CHECK();
			CHKRESULT();
			components.resize(r.componentsListOut.__size);
			for(int i=0; i < r.componentsListOut.__size; ++i)
			{
				SOAPComponentId &sid=r.componentsListOut.__ptr[i];
				ComponentIdFromSOAP(components[i], sid);				
			};
		};
	   
		bool IsComponentStarted(const KLPRCI::ComponentId & id)
		{
			PROXY_LOCK();
			klagent_IsComponentStartedResponse r;
			SOAPComponentId sid;

			soap_default_klagent_IsComponentStartedResponse(PROXY_SOAP(), &r);			
			ComponentIdForSOAP(PROXY_SOAP(), sid, id, false);
			soap_call_klagent_IsComponentStarted(PROXY_SOAP(), NULL, NULL, sid, r);
			PROXY_CHECK();
			CHKRESULT();
			return r.res!=0;
		};

		void GetInstanceInfo(
			const KLPRCI::ComponentId&	id,
			KLPAR::Params**				ppParams)
		{
			KLSTD_CHKOUTPTR(ppParams);
			PROXY_LOCK();

			SOAPComponentId cid;
			KLPRCP::ComponentIdForSOAP(PROXY_SOAP(), cid, id, false);
			klagent_GetComponentStartParametersResponse r;
			soap_default_klagent_GetComponentStartParametersResponse(PROXY_SOAP(), &r);
			soap_call_klagent_GetComponentStartParameters(
				PROXY_SOAP(),
				NULL,
				NULL,
				cid,
				r);
			PROXY_CHECK();
			CHKRESULT();
			ParamsFromSoap(r.componentStartParams, ppParams);
		};
	   
	   void SubscribeEx(
                        const KLPRCI::ComponentId & componentFilter, 
                        const std::wstring & eventType,
                        const KLPAR::Params * eventBodyFilter,
                        EventCallback callback,
                        void * context,
						KLPRCI::HSUBSCRIBE&			hSubscribe)
	   {
		   const KLPRCI::ComponentId& idRemoteComponent = GetRemoteComponentID();
		   KLEV::SubscriptionOptions opt;
		   opt.sendToAgent = true;
           opt.useSubscriberConnectionsOnly = true;
           opt.useOnlyLocalConnection = false;           
           opt.agentProductName = idRemoteComponent.productName;
           opt.agentVersionName = idRemoteComponent.version;
		   CComponentProxy::SubscribeEx(
									GetComponentID(),
									componentFilter,
									eventType,
									eventBodyFilter,
									callback,
									context,
									hSubscribe,
									opt);
	   };
	   
		void StopSubscriptionEx(KLPRCI::HSUBSCRIBE hSubscribe)
		{
			CComponentProxy::StopSubscription(hSubscribe);
		};

		void GetTasksStorage(KLPRTS::TasksStorage** storage)
		{
			KLSTD_CHKOUTPTR(storage);
            const KLPRCI::ComponentId& idComponent = GetRemoteComponentID();
			::KLPRTS_CreateTasksStorageProxy(
                KLAGINST::GetLocalTsName(   idComponent.productName,
                                            idComponent.version),
				GetComponentID(),
				idComponent,
				storage);
		};

		void GetEventsStorage(KLPRES::EventsStorage ** storage)
		{
			KLSTD_CHKOUTPTR(storage);
            const KLPRCI::ComponentId& idComponent = GetRemoteComponentID();
			KLPRES_CreateEventsStorageProxy(
				KLAGINST::GetLocalEsName(   idComponent.productName,
                                            idComponent.version),
				GetComponentID(),
				idComponent,
				storage);
		};

        void CreateComponentProxy(
							const KLPRCI::ComponentId & id,
							ComponentProxy ** proxy)
		{
			long nPort=0;
			{
				PROXY_LOCK();
				SOAPComponentId cid;
				KLPRCP::ComponentIdForSOAP(PROXY_SOAP(), cid, id, false);
				klagent_GetComponentPortResponse r;
				soap_default_klagent_GetComponentPortResponse(PROXY_SOAP(),&r);

				soap_call_klagent_GetComponentPort(
					PROXY_SOAP(),
					NULL,
					NULL,
					cid,
					r);
				PROXY_CHECK();
				CHKRESULT();
				nPort=r.nPort;
			};
            const std::wstring& wstrAddress=KLPRCP_MakeConnectionAddress(nPort, true);

	        CAutoPtr<CComponentProxy> pProxy;
	        pProxy.Attach(new RcClassImpl<CComponentProxy>);
	        KLSTD_CHKMEM(pProxy);
	        pProxy->create_new_client_connection(id, wstrAddress.c_str());
	        pProxy->GetRemoteComponentID();
	        pProxy->GetComponentID();
	        pProxy.CopyTo((CComponentProxy**)proxy);
		}

		void StartTasksFromTSAsync(
						const std::wstring&			wstrUID,						
						const std::wstring&			wstrTaskId,
						long						lTimeout)
		{
			PROXY_LOCK();
			klagent_StartTaskStorageTaskResponse r;
			soap_default_klagent_StartTaskStorageTaskResponse(PROXY_SOAP(),&r);

			soap_call_klagent_StartTaskStorageTask(
				PROXY_SOAP(),
				NULL,
				NULL,
				(wchar_t*)wstrTaskId.c_str(),
				(wchar_t*)wstrUID.c_str(),
				lTimeout,
				r);
			PROXY_CHECK();
			CHKRESULT();
		};

		class CStartTasksFromTS
		{
		public:
			CAutoPtr<Semaphore>		pSemaphore;
			CAutoPtr<Params>		pResult;
			static void OnEventCallback(
						const KLPRCI::ComponentId & id,
						const std::wstring & eventType,
						const KLPAR::Params * eventBody,
						time_t time,
						void * context)
			{
				KLSTD_ASSERT(context);
				CStartTasksFromTS* pThis=(CStartTasksFromTS*)context;
				KLSTD_ASSERT(pThis && pThis->pSemaphore);
				if(!pThis || !pThis->pSemaphore)return;
				pThis->pResult=(KLPAR::Params*)eventBody;
				pThis->pSemaphore->Post();
			};
		};


		void StartTasksFromTS(
						const std::wstring&			wstrTaskId,
						long						lTimeout,
						KLPRCI::ComponentId&		idComponent,
						std::wstring&				wstrTaskName,
						long&						lTaskId)
		{
            if(KLSTD_GetShutdownFlag())
                KLSTD_THROW(STDE_UNAVAIL);
            ;
			KLSTD_USES_CONVERSION;
            std::wstring wstrUID=KLSTD_CreateLocallyUniqueString();
			param_entry_t pars[]=
			{
				param_entry_t(c_evpUID, wstrUID.c_str())
			};
			CAutoPtr<Params> pEventBodyFilter;
			CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pEventBodyFilter);

			CStartTasksFromTS data;
			::KLSTD_CreateSemaphore(&data.pSemaphore, 0);
			KLPRCI::HSUBSCRIBE			hSubscribe=NULL;
			KLERR_TRY
				Subscribe(
					c_EventTaskTsResult,
					pEventBodyFilter,
					CStartTasksFromTS::OnEventCallback,
					&data,
					hSubscribe);
				StartTasksFromTSAsync(
									wstrUID, 
									wstrTaskId,
                                    lTimeout);
				if(lTimeout<=0)lTimeout=KLSTD_INFINITE;
				CheckAndWait(data.pSemaphore, lTimeout, KLPRCP_WAITTIMESLICE, NULL, true);
				KLSTD_ASSERT_THROW(data.pResult != NULL);
				if(data.pResult->DoesExist(KLBLAG::c_szwErrorInfo))
				{
					CAutoPtr<Params> pErrorInfo;
					pErrorInfo=GetParamsValue(data.pResult, KLBLAG::c_szwErrorInfo);
					KLSTD_ASSERT_THROW(pErrorInfo != NULL);					
					long nCode=GetIntValue(pErrorInfo, KLBLAG::c_szwErrorCode);
					std::wstring wstrModule=GetStringValue(pErrorInfo, KLBLAG::c_szwErrorModule);
					std::wstring wstrMessage=GetStringValue(pErrorInfo, KLBLAG::c_szwErrorMsg);
					std::wstring wstrFileName=GetStringValue(pErrorInfo, KLBLAG::c_szwErrorFileName);
					long nLine=GetIntValue(pErrorInfo, KLBLAG::c_szwErrorLineNumber);
					CAutoPtr<KLERR::Error> pError;
					KLERR_CreateError(
							&pError,
							wstrModule.c_str(),
							nCode,
							KLSTD_W2CA(wstrFileName.c_str()),
							nLine,
							wstrMessage.c_str());
					throw pError.Detach();
				}
				else
				{
					wstrTaskName=GetStringValue(data.pResult, KLBLAG::c_szwServerTaskName);
					idComponent.productName=GetStringValue(data.pResult, KLBLAG::c_szwServerProduct);
					idComponent.version=GetStringValue(data.pResult, KLBLAG::c_szwServerVersion);
					idComponent.componentName=GetStringValue(data.pResult, KLBLAG::c_szwServerComponent);
					idComponent.instanceId=GetStringValue(data.pResult, KLBLAG::c_szwServerInstance);
					lTaskId=GetIntValue(data.pResult, KLBLAG::c_szwTaskId);
				};
			KLERR_CATCH(pError)
				KLERR_SAY_FAILURE(2, pError);
			KLERR_FINALLY
				if(hSubscribe)
				{
					KLERR_IGNORE(StopSubscription(hSubscribe));
				};
				KLERR_RETHROW();
			KLERR_ENDTRY;
		};

		KLTRAP::TransportProxy* getTransportProxy()const
		{		
			return CComponentProxy::getTransportProxy();
		}
	};
};

KLCSKCA_DECL void KLPRCP_AttachComponentProxy(
                    const std::wstring&         wstrLocalConnection,
					const std::wstring&         wstrRemoteConnection,
					KLPRCP::ComponentProxy**    ppProxy,
                    KLSTD::KLBase*              pOwner)
{
	KLSTD_CHKOUTPTR(ppProxy);
	CAutoPtr<CComponentProxy> pProxy;
	pProxy.Attach(new RcClassImpl<CComponentProxy>);
	KLSTD_CHKMEM(pProxy);
	pProxy->Initialize(wstrLocalConnection, wstrRemoteConnection);
	pProxy->GetRemoteComponentID();
	pProxy->GetComponentID();
    pProxy->SetOwner(pOwner);
	pProxy.CopyTo((CComponentProxy**)ppProxy);
}

KLCSKCA_DECL void KLPRCP_CreateGatewayedComponentProxy(
                        KLTRAP::Transport::LocationsList&   vecLocations,
                        long                                lTimeout,
                        bool                                bSSL,
                        const KLPRCP::conn_attr_t*          pExtraAttr,
                        KLPRCP::ComponentProxy**            ppProxy)
{
	KLSTD_CHKOUTPTR(ppProxy);
	KLSTD_CHK(vecLocations, vecLocations.size() > 0);
	CAutoPtr<ComponentProxy> pProxy;
    CComponentProxy* p=new RcClassImpl<CComponentProxy>;
	KLSTD_CHKMEM(p);
	pProxy.Attach(p);	
	p->create_new_gatewayed_connection(vecLocations, lTimeout, bSSL, pExtraAttr);
	p->GetRemoteComponentID();
	p->GetComponentID();
	pProxy.CopyTo(ppProxy);
};

static void CreateComponentProxyInternal(
						const KLPRCI::ComponentId&  id,
						KLPRCP::ComponentProxy**    ppProxy,
                        const KLPRCP::conn_attr_t*  pExtraAttr)
{
	KLSTD_CHKOUTPTR(ppProxy);
	KLSTD_CHK(
		id,
		(	!id.productName.empty() &&
			!id.version.empty() &&
			!id.componentName.empty() &&
			!id.instanceId.empty()));
	
	CAutoPtr<CComponentProxy> pProxy;
    pProxy.Attach( new RcClassImpl<CComponentProxy> );
	KLSTD_CHKMEM(pProxy);
	pProxy->InitializeLocal(id, pExtraAttr);
	pProxy->GetRemoteComponentID();
	pProxy->GetComponentID();
	pProxy.CopyTo((CComponentProxy**)ppProxy);
};

KLCSKCA_DECL void KLPRCP_CreateComponentProxy(
						const KLPRCI::ComponentId & id,
						KLPRCP::ComponentProxy** ppProxy)
{
    CreateComponentProxyInternal(id, ppProxy, NULL);
};

KLCSKCA_DECL void KLPRCP_CreateComponentProxy2( 
                        const KLPRCI::ComponentId&  id,
                        KLPRCP::ComponentProxy**	ppProxy,
                        const KLPRCP::conn_attr_t*  pExtraAttr)
{
    CreateComponentProxyInternal(id, ppProxy, pExtraAttr);
}

KLCSKCA_DECL void KLPRCP_CreateTransparentComponentProxy( 
                        const KLPRCI::ComponentId & id,
                        KLPRCP::ComponentProxy**	ppProxy)
{
    KLPRCP::conn_attr_t ca;
    ca.m_bIsTransparent = true;
    CreateComponentProxyInternal(id, ppProxy, &ca);
};

namespace
{
        void SplitAddress(
				const wchar_t*	/*[in]*/    address,
				std::wstring*   /*[out]*/   protocol,
				int*            /*[out]*/   port,
				std::wstring*	/*[out]*/   hostname)
    {
	    KLSTD_USES_CONVERSION;
	    if(!address || !address[0])
		    KLSTD_THROW_BADPARAM(address);
	    std::wstring wstrProto, wstrAddrOrIp, wstrPort;
	    {
		    const wchar_t c_szwPrefix[]=L"://";
		    const wchar_t* pAddrStart=wcsstr(address, c_szwPrefix);
		    if(!pAddrStart)
                pAddrStart=address;
		    else
            {
			    wstrProto=std::wstring(address, pAddrStart-address);
			    pAddrStart+=KLSTD_COUNTOF(c_szwPrefix)-1;		
		    };
		    wchar_t* pAddrEnd=wcsrchr((wchar_t *)pAddrStart, L':');
		    if(!pAddrEnd)
			    wstrAddrOrIp=std::wstring(pAddrStart);
		    else
            {
			    wstrPort=pAddrEnd+1;
			    wstrAddrOrIp=std::wstring(pAddrStart, pAddrEnd-pAddrStart);
		    };
		    if(wstrAddrOrIp.empty())
            {
                KLSTD_THROW_BADPARAM(address);
            };
        };
	    if(hostname)
            *hostname=wstrAddrOrIp;
	    if(protocol)
		    *protocol=wstrProto;
	    if(port)
		    *port=wstrPort.empty() ? 0L : wcstol(wstrPort.c_str(), NULL, 0);
    };
};


// [<proto>://]<host_or_ip>[:<ip_address>]
KLCSKCA_DECL void KLPRCP_SplitAddress(
				const wchar_t*	/*[in]*/ address,
				std::wstring*		/*[out]*/protocol,
				int*        	/*[out]*/port,
				std::wstring*		/*[out]*/hostname)
{
    SplitAddress(address, protocol, port, hostname);
};


KLCSKCA_DECL void KLPRCP_CreateAgentProxyForConnection(
					const std::wstring& wstrLocalConnection,
					const std::wstring& wstrRemoteConnection,
					KLPRCP::AgentProxy** ppProxy)
{
	KLSTD_CHKOUTPTR(ppProxy);
	CAutoPtr<AgentProxy> pProxy;
	CAgentProxy* p=new RcClassImpl<CAgentProxy>;
	KLSTD_CHKMEM(p);
	pProxy.Attach(p);	
	p->Initialize(wstrLocalConnection, wstrRemoteConnection);
	p->GetRemoteComponentID();
	p->GetComponentID();
	pProxy.CopyTo(ppProxy);
}

KLCSKCA_DECL void KLPRCP_CreateGatewayedAgentProxy(
                        KLTRAP::Transport::LocationsList&   vecLocations,
                        long                                lTimeout,
                        bool                                bSSL,
                        const KLPRCP::conn_attr_t*          pExtraAttr,
                        KLPRCP::AgentProxy**	            ppProxy)
{
	KLSTD_CHKOUTPTR(ppProxy);
	KLSTD_CHK(vecLocations, vecLocations.size() > 0);
	CAutoPtr<AgentProxy> pProxy;
	CAgentProxy* p=new RcClassImpl<CAgentProxy>;
	KLSTD_CHKMEM(p);
	pProxy.Attach(p);	
	p->create_new_gatewayed_connection(vecLocations, lTimeout, bSSL, pExtraAttr);
	p->GetRemoteComponentID();
	p->GetComponentID();
	pProxy.CopyTo(ppProxy);
};

KLCSKCA_DECL bool CheckAgentExistence(
   						const std::wstring&			wstrProductName,
						const std::wstring&			wstrVersion)
{
    KLSTD_USES_CONVERSION;
    bool bResult=false;
#ifdef _WIN32
    HANDLE hMutex=NULL;
    KLERR_TRY
        const std::wstring& wstrMutexName=MAKE_AGENT_MUTEX_NAME(wstrProductName, wstrVersion);
        hMutex=OpenMutex(0, FALSE, KLSTD_W2CT(wstrMutexName.c_str()));
        bResult = (hMutex || GetLastError()==ERROR_ACCESS_DENIED);
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(4, pError);
    KLERR_FINALLY
        if(hMutex)
            CloseHandle(hMutex);
    KLERR_ENDTRY
#else
    return true;
#endif
    return bResult;
};

KLCSKCA_DECL void KLPRCP_CreateAgentProxy(
						const std::wstring&			wstrProductName,
						const std::wstring&			wstrVersion,
						KLPRCP::AgentProxy**	ppProxy)
{
	KLSTD_CHKOUTPTR(ppProxy);
	KLSTD_CHK(wstrProductName, !wstrProductName.empty());
	KLSTD_CHK(wstrVersion, !wstrVersion.empty());
    if(!CheckAgentExistence(wstrProductName, wstrVersion))
        KLERR_MYTHROW1(KLPRCP::ERR_CANT_CONNECT, KLCS_MAKEWELLKNOWN_AGENT(wstrProductName).c_str());
	ComponentId idAgent(
					wstrProductName,
					wstrVersion,
					KLCS_COMPONENT_AGENT,
					KLCS_MAKEWELLKNOWN_AGENT(wstrProductName));	
	CAutoPtr<AgentProxy> pProxy;
	CAgentProxy* p=new RcClassImpl<CAgentProxy>;
	KLSTD_CHKMEM(p);
	pProxy.Attach(p);	
	p->InitializeLocal(idAgent);
	p->GetRemoteComponentID();
	p->GetComponentID();
	pProxy.CopyTo(ppProxy);
};

void KLPRCI_UnloadInProcessComponents();


void KLPRCI_GetLoadedInProcessComponents(
						KLPRCI::ComponentId&			filter,
						std::list<KLPRCI::ComponentId>&	ids);

KLCSKCA_DECL void KLPRCP_GetLoadedInProcessComponents(
						KLPRCI::ComponentId&				filter,
						std::vector<KLPRCI::ComponentId>&	ids)
{
	std::list<KLPRCI::ComponentId> lstIDs;
	KLPRCI_GetLoadedInProcessComponents(filter, lstIDs);
	ids.resize(lstIDs.size());
	int i=0;
	for(std::list<KLPRCI::ComponentId>::iterator it=lstIDs.begin(); it != lstIDs.end(); ++it, ++i)
	{
		ids[i]=*it;
	}
};

KLCSKCA_DECL void KLPRCP_UnloadInProcessComponents()
{
	KLPRCI_UnloadInProcessComponents();
};

KLCSKCA_DECL bool KLPRCP_UnloadInProcessComponent(KLPRCI::ComponentId& id)
{
	return KLPRCI_UnloadComponent(id);
}

KLCSKCA_DECL void KLPRCP_LoadInProcessComponent(
						KLPRCI::ComponentId&	id,
						KLPAR::Params*			pParameters,
						std::wstring			wstrAsyncId,
						AVP_dword				dwFlags,
						long					lAsyncTimeout,
						long					lTimeoutStop)
{
	KLSTD_CHK(dwFlags , ((dwFlags & ~(SCF_AUTOSTOP|SCF_SYNC)) == 0));
		
	if(wstrAsyncId.empty())
		wstrAsyncId=::KLPRCI_CreateInstanceId(NULL);
	CAutoPtr<Params> pParameters2;
	if(!pParameters)
		KLPAR_CreateParams(&pParameters2);
	else
		pParameters->Clone(&pParameters2);

	CAutoPtr<BoolValue>		p_bAutoStop;
	CAutoPtr<IntValue>		p_lTimeoutStop;
	CAutoPtr<StringValue>	p_wstrAsyncId;

	CreateValue(wstrAsyncId.c_str(), &p_wstrAsyncId);
	CreateValue(dwFlags & SCF_AUTOSTOP, &p_bAutoStop);
	CreateValue(lTimeoutStop, &p_lTimeoutStop);

	pParameters2->ReplaceValue(c_ComponentAsyncId, p_wstrAsyncId);
	pParameters2->ReplaceValue(c_AutoStopAllowed, p_bAutoStop);
	pParameters2->ReplaceValue(c_TimeoutStopAllowed, p_lTimeoutStop);
	KLPRCI_LoadComponent(id, pParameters2, wstrAsyncId);
}

KLCSKCA_DECL std::wstring KLPRCP_CreateLocalAddress(int nPort)
{
	return makeLocalAddress(nPort);
};

KLCSKCA_DECL std::wstring KLPRCP_CreateNonLocalAddress(const wchar_t* szwHostName, int nPort)
{
    if(szwHostName && szwHostName[0])
        return KLPRCP_MakeAddress2(szwHostName, nPort);
    else
        return makeRemoteAddress(NULL, nPort);    
};
