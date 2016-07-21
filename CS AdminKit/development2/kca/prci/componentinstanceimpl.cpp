#include "./componentinstanceimpl.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
#include <std/win32/klos_win32v40.h>
#include <std/win32/klos_win32_errors.h>
#include <process.h>	//use getpid
#endif

#ifdef __unix
#include <unistd.h>     //use getpid
#include <dlfcn.h>
#define KLAVT_OFF
#endif

#ifdef N_PLAT_NLM
#  include "std/conv/wcharcrt.h"
#endif

#include <std/tmstg/timeoutstore.h>
#include <std/thrstore/threadstore.h>
#include <std/par/errors.h>
#include <std/thr/thread.h>
#include <std/par/params.h>
#include <std/par/make_env.h>
#include <std/par/helpersi.h>
#include <std/par/par_gsoap.h>
#include <transport/ev/ev_general.h>
#include <transport/tr/transportproxy.h>
#include <transport/avt/accesscheck.h>
#include <transport/ev/publish_ev.h>
#include <kca/prss/errors.h>
#include <kca/prss/helpersi.h>
#include <kca/prci/componentid.h>
#include <kca/prcp/proxybase.h>
#include <kca/prci/cmp_helpers.h>
#include <kca/prci/runtimeinfo.h>
#include <common/measurer.h>
#include <common/trace_username.h>
#include <common/kl_dll_port.h>
#include <kca/prci/errlocids.h>

#include "./prci_auth.h"
#include "./prci_inprocess.h"

#include "soapH.h"
#include <modulestub/prcisoapstub.h>

#define KLCS_MODULENAME L"KLPRCI"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;
using namespace std;
using namespace KLPRCI;

#define KLPRCI_CALLBACK_SET(_name)  \
    m_lck##_name.Disallow();        \
    m_lck##_name.Wait();            \
	m_p##_name = callback;          \
	m_p##_name##Context=context;    \
    m_lck##_name.Allow();

#define KLPRCI_CALLBACK_PREPARE_CALL(_name)     \
    CAutoObjectLock aol##_name(m_lck##_name);   \
    if(!prciacc)                                \
    {                                           \
        KLSTD_THROW_APP_PENDING();              \
    };


namespace KLPRCI
{
    /*********************************************************************
    *
    *               ComponentInstanceBase
    *
    *********************************************************************/

    ComponentInstanceBase::ComponentInstanceBase()
            :   m_pDataCS(KLSTD::CreateCS())
            //,   m_pCallCS(KLSTD::CreateCS())
            //,   m_CallStatisticsCS(KLSTD::CreateCS())
    {
        ;
    };

    ComponentInstanceBase::~ComponentInstanceBase()
    {
        ;
    };

    int TransportStatusNotifyCallback(
			    KLTRAP::Transport::ConnectionStatus	status, 
			    const wchar_t*				remoteComponentName,
			    const wchar_t*				localComponentName,
			    bool						bRemote);

    static KLTRAP::Transport::StatusNotifyCallback	g_pOldTransportCallback=NULL;


    //static CAutoPtr<InProcessComponents> g_pComponents;
    static KLSTD::LockCount g_lckModule;

    static void KLPRCI_OnComponentClosed(std::wstring& instanceId)
    {
        //if(g_pComponents)
	        //g_pComponents->OnComponentClosed(instanceId);
    };


    /*********************************************************************
    *
    *               CComponentInstance
    *
    *********************************************************************/
    
	CComponentInstance::CComponentInstance()
		:	ModuleClass(&g_lckModule)
		,	m_wstrSettingsLocation(KLPRSS_GetSettingsStorageLocation(SS_SETTINGS))
		,	m_wstrRunTimeLocation(KLPRSS_GetSettingsStorageLocation(SS_RUNTIME))
		,	m_bInitialized(false)
		,	m_lRefCounter(0)
		,	m_bAllowAutoStop(false)
		,	m_lTimeoutStop(KLSTD_INFINITE)
        ,   m_lTrRecvTimeout(KLSTD_INFINITE)
		,	m_pAcl(NULL)
		,	m_nAcl(0)
        ,   m_lPingTimeout(KLPRCI_DEFAULT_PING_TIMEOUT)
        ,   m_pRunTasksCallback(NULL)
        ,   m_pRunTasksCallbackContext(NULL)
        ,   m_pTasksControlCallback(NULL)
        ,   m_pTasksControlCallbackContext(NULL)
        ,   m_pRunMethodCallback(NULL)
        ,   m_pRunMethodCallbackContext(NULL)
        ,   m_pInstanceControlCallback(NULL)
        ,   m_pInstanceControlCallbackContext(NULL)
        ,   m_CurState(STATE_CREATED)
        ,   m_pTransport(KLTR_GetTransport())
        ,   m_pUpdateStatisticsCallback(NULL)
        ,   m_pUpdateStatisticsCallbackContext(NULL)
        ,   m_bInAgent(false)
        ,   m_bInNagent(false)
	{
        KLSTD_ASSERT(m_pTransport);
        m_lckRunTasksCallback.Allow();
        m_lckInstanceControlCallback.Allow();
        m_lckTasksControlCallback.Allow();
        m_lckRunMethodCallback.Allow();
        m_lckUpdateStatisticsCallback.Allow();
        //g_lckModule.Lock();
	};

    // szwAddress has format 'http://hostname'    
    void CComponentInstance::Create(
					const KLPRCI::ComponentId&	componentId,
					const wchar_t*				szwAsyncId,
                    unsigned short*				pDesirablePorts,
					const wchar_t*				szwAddress,
                    InstanceListenerType&       type)
    {
        KLSTD_CHK(szwAddress, szwAddress && szwAddress[0]);
        KLSTD_CHK(componentId,  !componentId.componentName.empty() &&
			                    !componentId.instanceId.empty() &&
			                    !componentId.productName.empty() &&
			                    !componentId.version.empty());
        ;
        const_cast<KLPRCI::ComponentId&>(m_ComponentID) = componentId;
		const_cast<KLPRCI::ComponentId&>(m_AgentID) = 
            KLPRCI::ComponentId(
				componentId.productName,
				componentId.version,
				KLCS_COMPONENT_AGENT,
				KLPRCI_CreateInstanceId(
                    KLCS_MAKEWELLKNOWN_AGENT(
                        componentId.productName).c_str()));
        const_cast<bool&>(m_bInAgent) = 
                    (m_ComponentID.componentName == KLCS_COMPONENT_AGENT);
		const_cast<bool&>(m_bInNagent) = 
                    (m_ComponentID.componentName == KLCS_COMPONENT_NAGENT);
        m_wstrAsyncID = std::wstring(szwAsyncId ? szwAsyncId : L"");
        ;
#ifndef KLAVT_OFF
        AVTL_AcquireAclLibrary(&m_pSecLib);
        KLSTD_ASSERT_THROW(m_pSecLib != NULL);

		if(!m_pSecLib->GetACLForComponent(
									componentId,
									(const KLAVT::ACE_DECLARATION**)&m_pAcl,
									&m_nAcl))
		{
			KLSTD_THROW(STDE_NOACCESS);
		};
		
		KLAVT_CreateACL(m_pAcl, m_nAcl, &m_pAclObj);
#endif        
        //!connecting to ss server
        KLPRSS_CreateSettingsStorage(
                        m_wstrSettingsLocation, 
                        CF_OPEN_EXISTING,
                        AF_READ,
                        &m_pSS);
        KLERR_BEGIN
            KLPRSS::ComponentInfo ci;
            KLPRSS_GetComponentInfo(
                        m_ComponentID.productName,
                        m_ComponentID.version,
                        m_ComponentID.componentName,
                        ci,
                        KLPRCI_DEFAULT_SS_TIMEOUT);
            typedef std::vector<std::wstring> wstring_vect_t;
            wstring_vect_t& names=ci.pwstrSsDataTasks;
            for(wstring_vect_t::iterator it=names.begin(); it != names.end(); ++it)
                m_setTasksToComplement.insert(*it);
            const_cast<long&>(m_lPingTimeout)=
                ci.lPingTimeout
                    ?((ci.lPingTimeout < 0) ? KLSTD_INFINITE: ci.lPingTimeout)
                    :KLPRCI_DEFAULT_PING_TIMEOUT;
            KLSTD_ASSERT(m_lPingTimeout ==KLSTD_INFINITE || m_lPingTimeout > 0);
            InitAsyncID(&ci);
        KLERR_ENDT(1)        
		
        KLTRAP::ConvertComponentIdToTransportName(m_wstrConnectionName, m_AgentID);
		KLTRAP::ConvertComponentIdToTransportName(m_wstrClientName, m_ComponentID);
        ;
        KLSTD_TRACE1(3, L"Creating CComponentInstance for '%ls'\n", m_wstrClientName.c_str());
        KLSTD::TraceImpersonated(3);
        ;
		KLERR_TRY
			g_Instances.addComponentForTransport(m_ComponentID.instanceId, this);
			bool bWasListenLocationAdded=false;
            std::wstring wstrTmpAddress;
			for(unsigned short* pPort=pDesirablePorts; pPort && *pPort && !bWasListenLocationAdded; ++pPort)
			{				
				KLERR_TRY
                    wstringstream ws;
                    ws << szwAddress << L":" << static_cast<wchar_t>(*pPort);
                    wstrTmpAddress = ws.str();

					m_pTransport->AddListenLocation(
										m_wstrClientName.c_str(),
										wstrTmpAddress.c_str(),
										m_nMyPort);
					bWasListenLocationAdded=true;
					m_wstrListenAddress=wstrTmpAddress;
				KLERR_CATCH(pError)
                    if(!KLERR_IfIgnore(
                            pError->GetId(),
                            KLTRAP::TRERR_WRONG_ADDR,
                            KLTRAP::TRERR_LOCATION_ALREADY_EXISTS,
                            0))
                    {
                        KLERR_RETHROW();
                    };
				KLERR_ENDTRY
			};			
			if(!bWasListenLocationAdded)
			{
				m_wstrListenAddress=szwAddress;
				
				m_pTransport->AddListenLocation(
									m_wstrClientName.c_str(),
									m_wstrListenAddress.c_str(),
									m_nMyPort);				
			};
            RegisterInstanceInStore(type);
            g_Instances.addComponentForSoap(m_ComponentID.instanceId, this);
			if( 
                !m_bInAgent &&
                CheckAgentExistence(m_ComponentID.productName, m_ComponentID.version))
            {
				KLERR_TRY
					KLSTD_TRACE0(3, L"Connecting to agent...\n");
					std::wstring wstrAgentAddress( FindAgent(m_wstrRunTimeLocation) );
					KLSTD_TRACE1(3, L"Agent address: '%ls'\n", wstrAgentAddress.c_str());
					bool bConnAdded=(!wstrAgentAddress.empty()) ? 
										m_pTransport->AddClientConnection(
											m_wstrClientName.c_str(),
											m_wstrConnectionName.c_str(),
											wstrAgentAddress.c_str(),
                                            m_lPingTimeout, // pingTimeout
                                            false, //connForGateway
                                            NULL, // pCustomCredentials
                                            KLTRAP::Transport::PriorityNormal,
                                            true) //makeTwoWayAuthentication
										: false;
					if(!bConnAdded)
						Trace1(KLCS_MODULENAME, L"...Failed to connect to agent\n");
					else
						KLSTD_TRACE0(3, L"...OK connecting to agent\n");
				KLERR_CATCH(pError)
					if(pError->GetId()==KLTRAP::TRERR_CONN_ALREADY_EXISTS)
						KLERR_MYTHROW1(ERR_INSTNAME_EXISTS, m_wstrClientName.c_str());
//					if(pError->GetId()!=KLTRAP::TRERR_CONN_HANSHAKE)
//						KLERR_RETHROW();
				KLERR_ENDTRY
			};
            SetStateI(m_CurState, false);//STATE_CREATED
			if(
                !m_bInAgent &&
                !m_bInNagent &&
                KLTR_GetTransport()->IsConnectionActive(
                        m_wstrClientName.c_str(),
                        m_wstrConnectionName.c_str()))
			{
			KLERR_BEGIN
				CAutoPtr<Params>	pData;
				GetStartParametersI(&pData);
				if(pData->DoesExist(c_AutoStopAllowed))
				{
					CAutoPtr<BoolValue>	p_bAllowAutoStop;
					GetValue(pData, c_AutoStopAllowed, &p_bAllowAutoStop);
					m_bAllowAutoStop=p_bAllowAutoStop->GetValue();
				};
				if(pData->DoesExist(c_TimeoutStopAllowed))
				{
					CAutoPtr<IntValue>	p_lTimeoutStop;
					GetValue(pData, c_TimeoutStopAllowed, &p_lTimeoutStop);
					m_lTimeoutStop=p_lTimeoutStop->GetValue();
				};
			KLERR_ENDT(3)
			};
            m_lckExternal.Allow();
		KLERR_CATCH(pError)
            Destroy();
			KLERR_RETHROW();
		KLERR_ENDTRY
    };

    KLSTD_NOTHROW void CComponentInstance::Destroy() throw()
    {
        m_lckExternal.Disallow();
        m_lckExternal.Wait();
        ;
		KLSTD_TRACE0(3, L"SetState(STATE_DESTROYED)\n");
		KLERR_IGNORE(SetStateI(STATE_DESTROYED, false));
		KLSTD_TRACE0(3, L"g_Instances.removeComponent(m_ComponentID.instanceId);\n");
		KLERR_IGNORE(g_Instances.removeComponentForTransport(m_ComponentID.instanceId));
        KLERR_IGNORE(g_Instances.removeComponentForSoap(m_ComponentID.instanceId));
		KLSTD_TRACE0(3, L"m_Subscriptions.Clear();\n");
        KLERR_IGNORE(m_Subscriptions.Clear(m_pDataCS));
		KLSTD_TRACE0(3, L"UnRegisterInstanceInStore();\n");			
		KLERR_IGNORE(UnRegisterInstanceInStore());
		KLERR_IGNORE(KLPRCI_OnComponentClosed((wstring&)this->m_ComponentID.instanceId));
		if(!m_bInAgent)
		{
			KLSTD_TRACE0(3, L"CloseClientConnection\n");
			KLERR_IGNORE(m_pTransport->CloseClientConnection(
													m_wstrClientName.c_str(),
													m_wstrConnectionName.c_str()));
		};
		KLSTD_TRACE0(3, L"DeleteListenerAndCloseConnections\n");
		KLERR_IGNORE(m_pTransport->DeleteListenerAndCloseConnections(
										m_wstrClientName.c_str(),
										m_wstrListenAddress.c_str(),
										m_nMyPort));
    };

	CComponentInstance::~CComponentInstance()
	{
		if (m_pSecLib!=NULL) 
        {
            KLERR_IGNORE(m_pSecLib->FreeACL(m_pAcl));
        };
		m_pAclObj=NULL;
        //g_lckModule.Unlock();
	};

    #define KLPRCI_CALLCTRL()    \
        CAutoObjectLock prciacc(m_lckExternal);    \
        if(!prciacc) \
        {   \
            KLSTD_THROW_APP_PENDING();             \
        };

    void CComponentInstance::CheckInitialized()
    {
        if(!m_bInitialized)
            KLERR_MYTHROW4(
                        ERR_NOTREADY,
                        m_ComponentID.productName.c_str(),
                        m_ComponentID.version.c_str(),
                        m_ComponentID.componentName.c_str(),
                        m_ComponentID.instanceId.c_str());
    }

	std::wstring CComponentInstance::FindAgent(const std::wstring& storage)
	{	
        return KLPRCI_FindInstance(m_AgentID, m_pSS);
	}

    bool CComponentInstance::IsRightless()
    {
        KLSTD_TRACE0(4, L"CComponentInstance::IsRightless...\n");
        bool bIsRightless=true;
        KLERR_TRY
            KLAVT::ACE_DECLARATION acl[]=
            {
                {KLAVT_ADMINISTRATORS_GROUP, 0x1, true},
                {KLAVT_LOCAL_SYSTEM, 0x1, true}
            };
            CAutoPtr<KLAVT::AccessControlList> pACL;
            KLAVT_CreateACL(acl, KLSTD_COUNTOF(acl), &pACL);
            bIsRightless=!pACL->AccessCheck(
                                KLWAT_GetCurrentToken(),
                                0x1);		
			//bIsRightless = !KLAVT_AccessCheck(0x1, acl, KLSTD_COUNTOF(acl), false);            
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            KLSTD_ASSERT(pError == NULL);
        KLERR_ENDTRY
        KLSTD_TRACE1(4, L"...CComponentInstance::IsRightless returned %u\n", bIsRightless);
        return bIsRightless;
    }

	void CComponentInstance::RegisterInstanceInStore(InstanceListenerType type)
	{
		//KLERR_BEGIN
			CAutoPtr<SettingsStorage>	pStorage;			
            m_pSS->CreateSettingsStorage(m_wstrRunTimeLocation, CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pStorage);
            pStorage->SetTimeout(KLPRCI_DEFAULT_SS_TIMEOUT);
            bool bIsRightless=IsRightless();
            if(!bIsRightless)
            {
			    KLPRSS_MAYEXIST(pStorage->CreateSection(
											    m_ComponentID.productName,
											    L"",
											    L""));
			    KLPRSS_MAYEXIST(pStorage->CreateSection(
											    m_ComponentID.productName, 
											    m_ComponentID.version,
											    L""));
			    KLPRSS_MAYEXIST(pStorage->CreateSection(
											    m_ComponentID.productName,
											    m_ComponentID.version,
											    KLPRSS_SECTION_COMMON_SETTINGS));
            };
			CAutoPtr<ParamsValue>	pValueToAdd;
			CAutoPtr<Params>		pInstanaceParams, pRootParams;
			KLPAR_CreateParams(&pRootParams); 
			param_entry_t instancepars[]=
			{
				param_entry_t(KLPRSS_VAL_INSTANCEPORT,      (long)m_nMyPort),
				param_entry_t(KLPRSS_VAL_LASTSTART,         time_wrapper_t(time(NULL))),
				param_entry_t(KLPRSS_VAL_CONNECTIONNAME,    m_wstrClientName.c_str()),
                param_entry_t(KLPRSS_VAL_CONNTYPE,          (long)type),
#ifndef N_PLAT_NLM
				param_entry_t(KLPRSS_VAL_PID,               (long)getpid()),
#else
				param_entry_t(KLPRSS_VAL_PID,               (long)GetNLMID()),
#endif
                param_entry_t(KLPRSS_VAL_PINGTIMEOUT,       m_lPingTimeout)
			};
            KLPAR::CreateParamsBody(instancepars, KLSTD_COUNTOF(instancepars), &pInstanaceParams);
			CreateValue(pInstanaceParams, &pValueToAdd);

			const wchar_t* path[]={
				m_ComponentID.componentName.c_str(),
				KLPRSS_VAL_INSTANCES,
				NULL
			};
			KLPAR_ReplaceValue(pRootParams,path, m_ComponentID.instanceId, pValueToAdd);
            if(!bIsRightless)
			    pStorage->Replace(
				    m_ComponentID.productName,
				    m_ComponentID.version,
				    KLPRSS_SECTION_COMMON_SETTINGS,
				    pRootParams);
            else
			    pStorage->Add(
				    m_ComponentID.productName,
				    m_ComponentID.version,
				    KLPRSS_SECTION_COMMON_SETTINGS,
				    pRootParams);
		//KLERR_ENDT(3)
	};

	void CComponentInstance::UnRegisterInstanceInStore()
	{
		KLERR_BEGIN
			bool bResult=RemoveInstance(
							m_wstrRunTimeLocation,
							m_ComponentID.productName,
							m_ComponentID.version,
							m_ComponentID.componentName,
							m_ComponentID.instanceId,
                            m_pSS);
			if(!bResult)
			{
				Trace(5, KLCS_MODULENAME, L"RemoveInstance failed in UnRegisterInstanceInStore\n");
			};
		KLERR_END
	};

    void CComponentInstance::InitAsyncID(KLPRSS::ComponentInfo* pInfo)
	{
        if(!m_wstrAsyncID.empty())
            return ;
		KLERR_BEGIN
            KLPRSS::ComponentInfo* pCi=NULL, tmpCi;
            if(pInfo)
                pCi=pInfo;
            else
            {	                
			    KLPRSS_GetComponentInfo(
						    m_ComponentID.productName,
						    m_ComponentID.version,
						    m_ComponentID.componentName,
						    tmpCi,
                            KLPRCI_DEFAULT_SS_TIMEOUT);
                pCi=&tmpCi;
            };
            KLPRSS::ComponentInfo& ci=*pCi;

			if(ci.dwModuleType == MTF_EXE)
			{
				CAutoPtr<Params> pEnv;
				SplitEnvironment(NULL, &pEnv);
				if(pEnv->DoesExist(KLPRCI::c_szwEnvAsyncIdName))
				{
					CAutoPtr<StringValue> p_wstrValue;
					GetValue(pEnv, KLPRCI::c_szwEnvAsyncIdName, &p_wstrValue);
					m_wstrAsyncID=p_wstrValue->GetValue();
				}
			}
		#ifdef _UNICODE
			else if(ci.dwModuleType == MTF_NTSERVICE)
			{
				;
			};
		#endif
	KLERR_ENDT(2)
	};

//////////////////////////////////////////////////
//
//			ComponentInstance
//
//////////////////////////////////////////////////

	void CComponentInstance::GetComponentId(KLPRCI::ComponentId &componentId)
	{
		componentId=m_ComponentID;
	};

    void CComponentInstance::GetStartParameters(KLPAR::Params** ppParams)
    {
        KLPRCI_CALLCTRL();
        GetStartParametersI(ppParams);
    };

	void CComponentInstance::GetStartParametersI(KLPAR::Params** ppParams)
	{        
		KLSTD_CHKOUTPTR(ppParams);
		if(m_bInAgent)
			KLSTD_THROW(STDE_NOFUNC);
        bool bHasParams = false;
        {
            AutoCriticalSection acs(m_pDataCS);
            bHasParams = (m_pDataFromAgent != NULL);
        };

		if(!bHasParams)
		{
            CAutoPtr<Params> pDataFromAgent;
            ;
			KLTRAP::TransportProxy proxy;
			proxy.Initialize(m_wstrClientName.c_str(), m_wstrConnectionName.c_str());
			KLTRAP::TransportConnectionLocker _locker(&proxy);
			SOAPComponentId cid;
            if(!m_wstrAsyncID.empty())
            {
                KLPRCI::ComponentId idTemp=m_ComponentID;            
                idTemp.instanceId=L"*";
                idTemp.instanceId+=m_wstrAsyncID;
                KLPRCP::ComponentIdForSOAP(_locker.Get(), cid, idTemp, true);
            }
            else
			    KLPRCP::ComponentIdForSOAP(_locker.Get(), cid, m_ComponentID, false);
			klagent_GetComponentStartParametersResponse r;
			soap_default_klagent_GetComponentStartParametersResponse(_locker.Get(), &r);
			soap_call_klagent_GetComponentStartParameters(
				_locker.Get(),
				NULL,
				NULL,
				cid,
				r);
			_locker.CheckResult();
			ParamsFromSoap(r.componentStartParams, &pDataFromAgent);
            if(!pDataFromAgent)
                KLPAR_CreateParams(&pDataFromAgent);
            ;
            {
                AutoCriticalSection acs(m_pDataCS);
                m_pDataFromAgent = pDataFromAgent;
            };
		};
        ;
        {
            AutoCriticalSection acs(m_pDataCS);
            m_pDataFromAgent.CopyTo(ppParams);
        };        
	};

	void CComponentInstance::GetInstanceId(std::wstring& wstrID) const
	{
        KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
		wstrID=m_ComponentID.instanceId;
	};

	void CComponentInstance::GetTaskParams(long idTask, KLPAR::Params* pFilter, KLPAR::Params** ppParams)
	{
        KLSTD_CHKOUTPTR(ppParams);
        KLPRCI_CALLCTRL();
        KLPAR::ParamsPtr pResult;
        {
            AutoCriticalSection cs(m_pDataCS);
		    const task_data_t& taskdata=m_Tasks.findTask(idTask);
            // acquire subtree
            KLPAR::ParamsPtr pSubtree;
		    KLPAR_GetParamsSubtree(taskdata.m_pParams, pFilter, &pSubtree);
            if(pSubtree)
                pSubtree->Duplicate(&pResult);
            
            // acquire c_szwTaskStateExtra if it is required
            if( pFilter && pFilter->DoesExist(c_szwTaskStateExtra) )
            {
                if(!pResult)
                    KLPAR_CreateParams(&pResult);
                KLERR_BEGIN
                    KLPAR::ParamsPtr pData;
                    if(taskdata.m_pTaskStateExtra)
                        taskdata.m_pTaskStateExtra->Duplicate(&pData);
                    KLSTD::CAutoPtr<KLPAR::ParamsValue> p_valData;
                    KLPAR::CreateValue(pData, &p_valData);
                    pResult->ReplaceValue(c_szwTaskStateExtra, p_valData);
                KLERR_ENDT(1)
            };
        };
        pResult.CopyTo(ppParams);
	};

	long  CComponentInstance::GetPingTimeout() const
	{
        return m_lPingTimeout;
	};

	void  CComponentInstance::Ping() const
	{
        KLERR_BEGIN
            KLPRCI_CALLCTRL();
		    KLTRAP::Transport *pTransport=KLTR_GetTransport();
            KLSTD_ASSERT(pTransport);
            pTransport->SendPingMessage(
                                            m_wstrClientName.c_str(),
                                            m_wstrConnectionName.c_str());
        KLERR_ENDT(1)
	};

	ComponentInstanceState  CComponentInstance::GetState() const
	{
		return m_CurState;
	};

    void CComponentInstance::SetState(ComponentInstanceState state)
    {
    KLERR_BEGIN
        //KLPRCI_CALLCTRL();
        SetStateI(state, false);
    KLERR_ENDT(1)
    }

	KLSTD_NOTHROW void CComponentInstance::SetState(
                            ComponentInstanceState state, 
                            bool bPublishOnly) throw()
    {
    KLERR_BEGIN
        //KLPRCI_CALLCTRL();
        SetStateI(state,  bPublishOnly);
    KLERR_ENDT(1)

    };

    void CComponentInstance::SetInstanceState(
                ComponentInstanceState  state, 
                KLSTD::precise_time_t   tmPublish,
                KLPAR::Params*          pExtraData)
    {
    KLERR_BEGIN
        //KLPRCI_CALLCTRL();
        SetStateI(state,  false, tmPublish);
    KLERR_ENDT(1)
    };

	KLSTD_NOTHROW void CComponentInstance::SetStateI(
                            ComponentInstanceState state, 
                            bool bPublishOnly,
                            KLSTD::precise_time_t   tmPublish) throw()
	{
		KLSTD_TRACE1(3, L"CComponentInstance::SetState(%d)\n", state)
        bool    bShuttingDown = false;
    KLERR_BEGIN
		ComponentInstanceState oldState;        
		{
			AutoCriticalSection cs(m_pDataCS);

			oldState=m_CurState;
			m_CurState=state;
		
			switch(m_CurState)
			{
			case STATE_CREATED:
                g_Instances.addComponentForSoap(m_ComponentID.instanceId, this);
				m_bInitialized=false;
				break;
			case STATE_INITIALIZING:
				g_Instances.addComponentForSoap(m_ComponentID.instanceId, this);
				m_bInitialized=false;
				break;
			case STATE_RUNNING:
				g_Instances.addComponentForSoap(m_ComponentID.instanceId, this);
				m_bInitialized=true;
				break;
			case STATE_SUSPENDING:
				g_Instances.addComponentForSoap(m_ComponentID.instanceId, this);
				m_bInitialized=true;
				break;
			case STATE_DEINITIALIZING:
				g_Instances.addComponentForSoap(m_ComponentID.instanceId, this);
				m_bInitialized=false;
				break;
			case STATE_FAILURE:
			case STATE_SHUTTING_DOWN:
				g_Instances.removeComponentForSoap(m_ComponentID.instanceId);
				m_bInitialized=false;
                bShuttingDown = true;
				break;
			case STATE_DESTROYED:
				KLSTD_ASSERT(!m_bInitialized);
				break;
			};
		};		
		KLPAR::param_entry_t params[]=
		{
			KLPAR::param_entry_t(c_evpInstanceOldState,	Value::INT_T,	(long)oldState),
			KLPAR::param_entry_t(c_evpInstanceNewState,	Value::INT_T,	(long)state),
			KLPAR::param_entry_t(KLPRCP::c_AsyncId, Value::STRING_T, m_wstrAsyncID.c_str()),
			KLPAR::param_entry_t(KLPRCP::c_CID_InstanceId, Value::STRING_T, m_ComponentID.instanceId.c_str())
		};
		publishEvent(c_EventInstanceState, params, KLSTD_COUNTOF(params), 0, -1L, tmPublish);
        KLERR_ENDT(1)
        if(!bPublishOnly)
        {
            if(bShuttingDown)
            {
                m_lckExternal.Disallow();
                KLTMSG::AsyncCall0T<CComponentInstance>::Start(this, &CComponentInstance::Destroy);
            };
        };

    };

	void CComponentInstance::GetSettings(KLPRCI::ProductSettings** ppSettings)
	{
		KLSTD_CHKOUTPTR(ppSettings);
		CAutoPtr<ProductSettings> pResult=this;
		pResult.CopyTo(ppSettings);
	};

	void CComponentInstance::GetTasksControl(KLPRCI::ProductTasksControl** ppTasks)
	{
		KLSTD_CHKOUTPTR(ppTasks);
		CAutoPtr<ProductTasksControl> pResult=this;
		pResult.CopyTo(ppTasks);
	};

	void CComponentInstance::GetStatistics(KLPRCI::ProductStatistics** ppStatistics)
	{
		KLSTD_CHKOUTPTR(ppStatistics);
		CAutoPtr<ProductStatistics> pResult=this;
		pResult.CopyTo(ppStatistics);
	};

	void CComponentInstance::SetInstanceControlCallback(void* context, InstanceControlCallback	callback)
	{
        KLPRCI_CALLBACK_SET(InstanceControlCallback);
	};

	bool CComponentInstance::CheckEventsSubscription( const std::wstring &eventType, 
						KLPAR::Params * eventBody )
	{
        //KLPRCI_CALLCTRL();
		KLPRCI::ComponentId	publisher;
		{
			AutoCriticalSection cs(m_pDataCS);
			publisher = m_ComponentID;
		}

		KLEV::EventSource* pEvSource=KLEV_GetEventSource();

		return pEvSource->CheckEventsSubscription( publisher, eventType, eventBody );
	};

	void CComponentInstance::Publish(
                    const std::wstring& eventType, 
                    KLPAR::Params* eventBody,
                    long lifetime)
	{
    KLERR_BEGIN
        //KLPRCI_CALLCTRL();
        KLEV_PublishEvent(m_ComponentID, eventType, eventBody, lifetime);
    KLERR_ENDT(1)
	};

	void CComponentInstance::PublishEvent(
			    const std::wstring&	    eventType, 
			    KLPAR::Params*		    eventBody,
                KLSTD::precise_time_t   tmPublish)
    {
    KLERR_BEGIN
        //KLPRCI_CALLCTRL();
        KLEV_PublishEvent(m_ComponentID, eventType, eventBody, 0, tmPublish);
    KLERR_ENDT(1)
    };

	KLSTD_NOTHROW void CComponentInstance::InitiateUnload() throw()
	{
		KLERR_BEGIN
			KLPRCI_OnComponentClosed((wstring&)m_ComponentID.instanceId);
		KLERR_ENDT(1)
	};
    
    KLSTD_NOTHROW std::wstring CComponentInstance::GetLocation() throw()
    {
        return m_wstrListenAddress;
    };

    KLSTD_NOTHROW void CComponentInstance::Close(bool bSuccess) throw()
    {
        SetState(bSuccess?STATE_SHUTTING_DOWN:STATE_FAILURE, true);
        Destroy();
    };

//////////////////////////////////////////////////
//
//			ProductTasksControl
//
//////////////////////////////////////////////////



	void publishEvent(
			const KLPRCI::ComponentId&      idComponent,
			const std::wstring&             wstrEventName,
			KLPAR::param_entry_t*           pData,
			int                             nData,
			long                            lTimeout,
            long                            lSeverity,
            KLSTD::precise_time_t           tmPublish)
	{
        KLEV_PublishEvent2(
                        idComponent, 
                        wstrEventName,
                        pData, 
                        nData, 
                        lTimeout, 
                        lSeverity,
                        tmPublish);
	};


    void CComponentInstance::SetState(long taskId, TaskState state, const KLPAR::Params * results)
    {
        this->SetTaskState(
                        taskId, 
                        state, 
                        const_cast<KLPAR::Params*>(results), 
                        KLSTD::precise_time_t());
    };

    void CComponentInstance::SetTaskState(
                    long taskId, 
                    TaskState state, 
                    KLPAR::Params* results,
                    KLSTD::precise_time_t   tmPublish)
	{
        //KLPRCI_CALLCTRL();
        ;
		long			lTimeout;
		TaskState		nOldState;
		std::wstring	asyncId, wstrTsId;		
        bool bIsInvisible=false;
        KLSTD::CAutoPtr<KLPAR::StringValue> p_strDisplayName;
        KLPAR::ParamsPtr pTaskStateExtra = results;
        {
            bool bDoUnlock=false;
            //AutoCriticalSection acs_call(m_pCallCS);// we call UnlockInstance
		    {            
			    AutoCriticalSection cs(m_pDataCS);
			    task_data_t& taskdata=m_Tasks.findTask(taskId);
			    nOldState=taskdata.m_nState;
			    taskdata.m_nState=state;
                taskdata.m_pTaskStateExtra =  pTaskStateExtra;
                bIsInvisible=taskdata.m_bIsInvisible;
                if(!taskdata.m_wstrTaskStorageId.empty())
                {
                    KLSTD::CAutoPtr<KLPAR::StringValue> p_strFoo;
                KLERR_BEGIN
                    const wchar_t* path[] = 
                    {
                        KLPRTS::c_szwTaskParamTaskInfo,
                        NULL
                    };                    
                    KLPAR_GetValue( taskdata.m_pParams, 
                                    path, 
                                    KLPRTS::TASK_DISPLAY_NAME, 
                                    (KLPAR::Value**)&p_strFoo);
                    KLSTD_ASSERT_THROW(p_strFoo);
                    KLPAR_CHKTYPE(p_strFoo, STRING_T, KLPRTS::TASK_DISPLAY_NAME);
                KLERR_ENDT(1)
                    p_strDisplayName = p_strFoo;                    
                };
			    ;
			    lTimeout=taskdata.m_lTimeout;
			    asyncId=taskdata.m_wstrAsyncID;
                wstrTsId=taskdata.m_wstrTaskStorageId;
			    if(state==TASK_COMPLETED || state==TASK_FAILURE)
			    {
				    if(m_Tasks.removeTask(taskId))
				    {
					    bDoUnlock=true;
					    //UnlockInstance();
					    KLSTD_TRACE1(2, L"Task %d was destroyed\n", taskId);
				    };
			    };
		    };
		    if(bDoUnlock)
			    UnlockInstance();
        };
        KLSTD_TRACE2(
                    4, 
                    L"Settings Task %d state %d\n",
                    taskId,
                    state);
		KLPAR::param_entry_t params[]=
		{
			KLPAR::param_entry_t(c_evpTaskId,		(long)taskId),
			KLPAR::param_entry_t(c_evpTaskOldState,(long)nOldState),
			KLPAR::param_entry_t(c_evpTaskNewState,(long)state),
			KLPAR::param_entry_t(c_evpTaskResults,	(KLPAR::Params*)results),
			KLPAR::param_entry_t(c_evpTaskAsyncID,	asyncId.c_str()),
			KLPAR::param_entry_t(c_evpTaskTsId,	wstrTsId.c_str()),
            KLPAR::param_entry_t(c_evpTaskInvisible, bIsInvisible),
            KLPAR::param_entry_t(KLPRTS::TASK_DISPLAY_NAME, p_strDisplayName)
		};
        publishEvent(
                        c_EventTaskState,
                        params,
                        KLSTD_COUNTOF(params),
                        0,
                        (state != KLPRCI::TASK_FAILURE)
                            ?   KLEVP_EVENT_SEVERITY_INFO
                            :   KLEVP_EVENT_SEVERITY_ERROR,
                        tmPublish);
	};

	void CComponentInstance::SetTaskCompletion(long taskId, int percent)
	{
        //KLPRCI_CALLCTRL();
		long lTimeout;
		std::wstring asyncId, wstrTsId;
        bool bIsInvisible=false;
        bool bChanged = false;
		{
			AutoCriticalSection cs(m_pDataCS);
			task_data_t& taskdata=m_Tasks.findTask(taskId);
            bChanged = (taskdata.m_nCompletion != percent);
            if(bChanged)
            {
			    taskdata.m_nCompletion=percent;
			    lTimeout=taskdata.m_lTimeout;
			    asyncId=taskdata.m_wstrAsyncID;
                wstrTsId=taskdata.m_wstrTaskStorageId;
                bIsInvisible=taskdata.m_bIsInvisible;
            };
		};
        if(bChanged)
        {
		    KLPAR::param_entry_t params[]=
		    {
			    KLPAR::param_entry_t(c_evpTaskId,		taskId),
			    KLPAR::param_entry_t(c_evpTaskAsyncID,	asyncId.c_str()),
			    KLPAR::param_entry_t(c_evpTaskPercent,	(long)percent),
			    KLPAR::param_entry_t(c_evpTaskTsId,	wstrTsId.c_str()),
                KLPAR::param_entry_t(c_evpTaskInvisible, bIsInvisible)
		    };
		    publishEvent(c_EventTaskCompletion, params, KLSTD_COUNTOF(params), lTimeout);
        };
	};

	void CComponentInstance::SetRunTasksCallback(void* context, RunTasksCallback callback)
	{
        KLPRCI_CALLBACK_SET(RunTasksCallback);
	};

	void CComponentInstance::SetTasksControlCallback(void* context, TasksControlCallback callback)
	{
        KLPRCI_CALLBACK_SET(TasksControlCallback);
	};

    void CComponentInstance::SetRunMethodsCallback( 
				        void*				        context,
                        KLPRCI::RunMethodCallback   callback )
    {
        KLPRCI_CALLBACK_SET(RunMethodCallback);
    };

   long CComponentInstance::CallRunTasksCallback(
				const std::wstring & taskName,
				const KLPAR::Params * params,
				const std::wstring& asyncId,
				long timeout)
   {	   
       KLPRCI_CALLCTRL();
	   return RunTask(taskName, params, asyncId, timeout);
   };

   void* CComponentInstance::SetTaskData(long taskId, void* pData)
   {
       KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
		task_data_t& taskdata=m_Tasks.findTask(taskId);
		void* pResult=taskdata.m_pUserData;
		taskdata.m_pUserData=pData;
		return pResult;
   };
   
   void CComponentInstance::GetTaskParams(long idTask, KLPAR::Params** pParams)
   {
       KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
        task_data_t& taskdata=m_Tasks.findTask(idTask);
        if(taskdata.m_pParams2 != NULL && KLSTD_GetCurrentThreadId() == taskdata.m_lThreadId)
            taskdata.m_pParams2.CopyTo(pParams);
        else
            taskdata.m_pParams.CopyTo(pParams);
   };

//////////////////////////////////////////////////
//
//			ProductStatistics
//
//////////////////////////////////////////////////
	void CComponentInstance::SetStatisticsData( KLPAR::Params * statistics)
	{
        KLPRCI_CALLCTRL();
		KLSTD_CHKINPTR(statistics);
		AutoCriticalSection cs(m_pDataCS);
		m_pStatistics=statistics;
	};

	void CComponentInstance::ReplaceStatisticsData( KLPAR::Params * statistics)
	{
        KLPRCI_CALLCTRL();
		KLSTD_CHKINPTR(statistics);
		CAutoPtr<KLPAR::ValuesFactory> pFactory;
		::KLPAR_CreateValuesFactory(&pFactory);
		CAutoPtr<KLPAR::Params>	pNewStatistics;
		statistics->Clone(&pNewStatistics);

		AutoCriticalSection cs(m_pDataCS);
		if(!m_pStatistics)
			::KLPAR_CreateParams(&m_pStatistics);
		KLPAR::Write(pFactory, m_pStatistics, pNewStatistics, CF_OPEN_ALWAYS);
	};

	void CComponentInstance::DeleteStatisticsData(KLPAR::Params * statistics )
	{
        //KLPRCI_CALLCTRL();
		KLSTD_CHKINPTR(statistics);
		AutoCriticalSection cs(m_pDataCS);
		if(m_pStatistics)
		{
			CAutoPtr<KLPAR::ValuesFactory> pFactory;
			::KLPAR_CreateValuesFactory(&pFactory);		
			KLPAR::Write(pFactory, m_pStatistics, statistics, CF_CLEAR);
		};
	};


    void CComponentInstance::SetUpdateStatisticsCallback(
					void*					    context,
                    UpdateStatisticsCallback    callback)
    {
        KLPRCI_CALLBACK_SET(UpdateStatisticsCallback);
    };

    void CComponentInstance::call_UpdateStatisticsCallback(KLPAR::Params* pFilter)
    {
        KLPRCI_CALLCTRL();
        KLPRCI_CALLBACK_PREPARE_CALL(UpdateStatisticsCallback);
        if(m_pUpdateStatisticsCallback)
            m_pUpdateStatisticsCallback(m_pUpdateStatisticsCallbackContext, pFilter);
    };
//////////////////////////////////////////////////
//
//			ProductSettings
//
//////////////////////////////////////////////////

	std::wstring CComponentInstance::GetSettingsStorageLocation(
                                        KLPRSS::SS_OPEN_TYPE nOpenType) const
	{
        if(nOpenType == SSOT_SMART)
		    return m_wstrSettingsLocation;
        return KLPRSS_GetSettingsStorageLocation(
                                                KLPRSS::SS_SETTINGS,
                                                nOpenType);
	};


	void CComponentInstance::SubscribeOnSettingsChange( 
				const std::wstring&		productName, 
				const std::wstring&		version,
				const std::wstring&		section, 
				const std::wstring&		parameterName,
				void*					context,
				SettingsChangeCallback	callback,
				HSUBSCRIBE&				hSubscription)
	{
        KLPRCI_CALLCTRL();
		KLEV::EventSource* pEventSrc=KLEV_GetEventSource();
		KLSTD_ASSERT(pEventSrc);

		KLPAR::param_entry_t params[]={
			//KLPAR::param_entry_t(c_evpProductName,		productName.c_str()),
			//KLPAR::param_entry_t(c_evpVersion,			version.c_str()),
			//KLPAR::param_entry_t(c_evpSection,			section.c_str()),
            KLPAR::param_entry_t(c_evpIs_SETTINGS,     true),
			//KLPAR::param_entry_t(c_evpParameterName,	parameterName.c_str())
		};
		CAutoPtr<Params> pEventFilter;
		CreateParamsBody(params, KLSTD_COUNTOF(params), &pEventFilter);		
        if(!productName.empty())
        {
            CAutoPtr<StringValue> pName;
            CreateValue(productName.c_str(), &pName);
            pEventFilter->AddValue(c_evpProductName, pName);
        };		
        if(!version.empty())
        {
            CAutoPtr<StringValue> pName;
            CreateValue(version.c_str(), &pName);
            pEventFilter->AddValue(c_evpVersion, pName);
        };		
        if(!section.empty())
        {
            CAutoPtr<StringValue> pName;
            CreateValue(section.c_str(), &pName);
            pEventFilter->AddValue(c_evpSection, pName);
        };		
        if(!parameterName.empty())
        {
            CAutoPtr<StringValue> pParameterName;
            CreateValue(parameterName.c_str(), &pParameterName);
            pEventFilter->AddValue(c_evpParameterName, pParameterName);
        };		
		CNotifyChangesSink* pNotifySink=new CNotifyChangesSink(context, callback);
		KLSTD_CHKMEM(pNotifySink);
		CAutoPtr<AdviseSink> pSink;
		pSink.Attach(pNotifySink);
		{
			AutoCriticalSection cs(m_pDataCS);
			hSubscription=m_Subscriptions.addNew(pSink);
		};
		KLEV::SubscriptionOptions so;
		so.sendToAgent=true;
        so.useSubscriberConnectionsOnly = false;
        so.useOnlyLocalConnection = true;           
        so.agentProductName = m_ComponentID.productName;
        so.agentVersionName = m_ComponentID.version;
		KLPRCI::ComponentId emptyId;
		subscribe(m_ComponentID, emptyId, c_EventSettingsChange, pEventFilter, pSink, so, true);
	};

	void CComponentInstance::CancelSettingsChangeSubscription(HSUBSCRIBE hSubscription)
	{
    KLERR_BEGIN
        //KLPRCI_CALLCTRL();
        // EventSource должен иметь критическую секцию, защищающую вызов Unsubscribe и вызов колбэка		
		KLEV::EventSource* pEventSrc=KLEV_GetEventSource();
		KLSTD_ASSERT(pEventSrc);
        KLSTD::CAutoPtr<KLPRCI::AdviseSink> pAdviseSink;
		{
			AutoCriticalSection cs(m_pDataCS);
			m_Subscriptions.remove(hSubscription, &pAdviseSink);
		};
        pAdviseSink = NULL;
    KLERR_ENDT(1)
	};

	void CComponentInstance::NotifyAboutSettingsChange(
			const std::wstring&     productName, 
			const std::wstring&     version,
			const std::wstring&     section, 
			const std::wstring&     parameterName,
			const KLPAR::Value*     oldValue,
			const KLPAR::Value*     newValue,
            KLPRSS::SS_OPEN_TYPE    type)
	{
        KLPRCI_CALLCTRL();
        bool bUser= (type == KLPRSS::SSOT_CURRENT_USER /* || type == KLPRSS::SSOT_SMART */);
        const std::wstring& wstrSID=bUser ? KLPRCI_GetCurrentSID().c_str() : std::wstring(L"");
		KLPAR::param_entry_t params[]={
			KLPAR::param_entry_t(c_evpProductName,		productName.c_str()),
			KLPAR::param_entry_t(c_evpVersion,			version.c_str()),
			KLPAR::param_entry_t(c_evpSection,			section.c_str()),
            KLPAR::param_entry_t(c_evpSsType,          m_wstrSettingsLocation.c_str()),
            KLPAR::param_entry_t(c_evpIs_SETTINGS,     true),
			KLPAR::param_entry_t(c_evpParameterName,	parameterName.c_str()),
			KLPAR::param_entry_t(c_evpOldValue,		(Value*)oldValue),
			KLPAR::param_entry_t(c_evpNewValue,		(Value*)newValue),
            KLPAR::param_entry_t(c_evpUserName,        wstrSID.c_str())
		};
		publishEvent(c_EventSettingsChange, params, KLSTD_COUNTOF(params), 0);
	};

//////////////////////////////////////////////////
//
//			ComponentProxy
//
//////////////////////////////////////////////////

	void Read(const Params* pSource, Params* pDest)
	{		
		ParamsNames names;
		pDest->GetNames(names);
		if(names.size() == 0)
			pDest->CopyFrom(pSource);
		for(int i=0; i < (int)names.size(); ++i){
			if(!pSource->DoesExist(names[i]))continue;
			const Value* pSrcValue=pSource->GetValue2(names[i], true);
			if(pSrcValue->GetType()!=Value::PARAMS_T){
				CAutoPtr<Value> pDstValue;
				pSrcValue->Clone(&pDstValue);
				pDest->ReplaceValue(names[i], pDstValue);
			}
			else{
				CAutoPtr<Value> pDstValue;
				pDest->GetValue(names[i], &pDstValue);
				KLPAR_CHKTYPE(pDstValue, PARAMS_T, names[i].c_str());
				Params* pTmpSource=((ParamsValue*)(Value*)pSrcValue)->GetValue();
				Params* pTmpDest=((ParamsValue*)(Value*)pDstValue)->GetValue();
				if(pTmpSource && pTmpDest)
					Read(pTmpSource, pTmpDest);
			};
		};
	};
	void CComponentInstance::GetStatistics(KLPAR::Params * statistics)
	{
        KLPRCI_CALLCTRL();
		KLSTD_CHKINPTR(statistics);
        call_UpdateStatisticsCallback(statistics);
		AutoCriticalSection cs(m_pDataCS);
		if(m_pStatistics)
			Read(m_pStatistics, statistics);
	};

	void CComponentInstance::call_InstanceControlCallback(InstanceAction action)
	{
        KLPRCI_CALLCTRL();
        KLPRCI_CALLBACK_PREPARE_CALL(InstanceControlCallback);
        //AutoCriticalSection acs_call(m_pCallCS);
        CheckInitialized();
		if(m_pInstanceControlCallback)
		{
			CAutoPtr<SecContext> pSecContext;
			KLPRCI_GetClientContext(&pSecContext);
			AutoImpersonate ai(pSecContext);
			m_pInstanceControlCallback(m_pInstanceControlCallbackContext, action);
		}
		else
			KLSTD_THROW(STDE_NOFUNC);
	};

	void CComponentInstance::call_RunTasksCallback(const std::wstring& taskName, KLPAR::Params* pData, long lTaskID, long timeout)
	{
        KLPRCI_CALLCTRL();
        KLSTD_TRACE2(
                    3,
                    L"call_RunTasksCallback('%ls', '%ls')...\n",
                    taskName.c_str(),
                    m_wstrClientName.c_str())
		//AutoCriticalSection acs_call(m_pCallCS);
        CheckInitialized();
        KLPRCI_CALLBACK_PREPARE_CALL(RunTasksCallback);
		if(m_pRunTasksCallback)
		{            
			CAutoPtr<SecContext> pSecContext;
			KLPRCI_GetClientContext(&pSecContext);
			AutoImpersonate ai(pSecContext);
            KLSTD::TraceImpersonated(3);
			m_pRunTasksCallback(m_pRunTasksCallbackContext, taskName, pData, lTaskID, timeout);
		}
        else
			KLSTD_THROW(STDE_NOFUNC);
        KLSTD_TRACE2(
                    3,
                    L"...call_RunTasksCallback('%ls', '%ls')\n",
                    taskName.c_str(),
                    m_wstrClientName.c_str())
	};

    void CComponentInstance::RunMethod(
							const std::wstring& MethodName,
							KLPAR::Params*      params,
							long                timeout,
                            KLPAR::Params**     ppResult)
    {
        call_RunMethodCallback(
                        MethodName,
                        params,
                        timeout,
                        ppResult);
    };

	void CComponentInstance::call_RunMethodCallback(
					                    const std::wstring&		wstrMethodName,
                                        KLPAR::Params*	        pInParams,
                                        long					lTimeout,
                                        KLPAR::Params**         pOutParams)
	{
        KLPRCI_CALLCTRL();
        CheckInitialized();
        KLPRCI_CALLBACK_PREPARE_CALL(RunMethodCallback);
        if(!m_pRunMethodCallback)
            KLSTD_THROW(STDE_NOFUNC);
		
        CAutoPtr<SecContext> pSecContext;
		KLPRCI_GetClientContext(&pSecContext);
		AutoImpersonate ai(pSecContext);
        KLSTD::TraceImpersonated(3);
		m_pRunMethodCallback(m_pRunMethodCallbackContext, wstrMethodName, pInParams, lTimeout, pOutParams);
    };
    

	void CComponentInstance::call_TasksControlCallback(
                                                long            TaskId,
                                                TaskAction      action,
                                                KLPAR::Params*  pParams)
	{
        KLPRCI_CALLCTRL();
        CheckInitialized();
        KLPRCI_CALLBACK_PREPARE_CALL(TasksControlCallback);
        //{
            //AutoCriticalSection acs_call(m_pCallCS);
            if(!m_pTasksControlCallback)
                KLSTD_THROW(STDE_NOFUNC);
        //};        
        CAutoPtr<Params> pMergedData;
        if(action == ACTION_RELOAD)
        {
            std::wstring wstrTaskName;
            {
                AutoCriticalSection cs(m_pDataCS);
                const task_data_t& data = m_Tasks.findTask(TaskId);
                wstrTaskName=data.m_wstrName;
            };
            AdjustTasksParams(wstrTaskName, (Params*)pParams, &pMergedData);
        };
		CAutoPtr<SecContext> pSecContext;
		KLPRCI_GetClientContext(&pSecContext);
		AutoImpersonate ai(pSecContext);            
        KLERR_TRY
            if(action == ACTION_RELOAD)
            {
                AutoCriticalSection cs(m_pDataCS);
                m_Tasks.reload_begin(TaskId, pMergedData);
            };
            {
                //AutoCriticalSection acs_call(m_pCallCS);
                //if(!m_pTasksControlCallback)
                    //KLSTD_THROW(STDE_NOFUNC);
                m_pTasksControlCallback(m_pTasksControlCallbackContext, TaskId, action);
            }            
            if(action == ACTION_RELOAD)
            {
                AutoCriticalSection cs(m_pDataCS);
                m_Tasks.reload_commit(TaskId);
            };
        KLERR_CATCH(pError)
            if(action == ACTION_RELOAD)
            {
                AutoCriticalSection cs(m_pDataCS);
                m_Tasks.reload_rollback(TaskId);
            };
            KLERR_RETHROW();
        KLERR_ENDTRY
	};

	void CComponentInstance::Stop()
	{
        KLPRCI_CALLCTRL();
		call_InstanceControlCallback(INSTANCE_STOP);
	};

	void CComponentInstance::Suspend()
	{
        KLPRCI_CALLCTRL();
		call_InstanceControlCallback(INSTANCE_SUSPEND);
	};

	void CComponentInstance::Resume()
	{
        KLPRCI_CALLCTRL();
		call_InstanceControlCallback(INSTANCE_RESUME);
	};

	long CComponentInstance::m_nLastTaskID=0;

    void CComponentInstance::AdjustTasksParams(
                        const std::wstring&     wstrName,
                        KLPAR::Params*          pData,
                        KLPAR::Params**         ppResutl)
    {
        KLPRCI_CALLCTRL();
        CAutoPtr<Params> pResult;
        bool bWasHandled=false;
        if(m_setTasksToComplement.find(wstrName) != m_setTasksToComplement.end())
        {
            SmartReadTaskParams(
                    wstrName,
                    m_ComponentID.productName,
                    m_ComponentID.version,
                    pData,
                    &pResult,
                    m_lTrRecvTimeout);
            bWasHandled = true;
        };
        if(!bWasHandled)
            pResult=pData;
        /*
        if(wstrName == L"KLODS_TA_SCAN_TASK")
        {
            std::wstring wstrTempFile, wstrDir, wstrName, wstrExt;
            KLSTD_GetTempFile(wstrTempFile);
            KLSTD_SplitPath(wstrTempFile, wstrDir, wstrName, wstrExt);
            KLSTD_MakePath(wstrDir, L"$KLODS_TA_SCAN_TASK_A-" + wstrName, L".xml", wstrTempFile);
            KLPAR_SerializeToFileName(wstrTempFile, pResult);
            KLSTD_MakePath(wstrDir, L"$KLODS_TA_SCAN_TASK_B-" + wstrName, L".xml", wstrTempFile);
            KLPAR_SerializeToFileName(wstrTempFile, pData);
        };
        */
        pResult.CopyTo(ppResutl);
    };


    void CComponentInstance::AddTask(
                    long                lTaskId,
					const std::wstring& wstrTaskType,
					KLPAR::Params*      pParams,
					const std::wstring& asyncId,
					long                timeout,
                    KLSTD::precise_time_t   tmPublish)
    {
        KLPRCI_CALLCTRL();
        if(!lTaskId)
            lTaskId = KLPRCI_AllocTaskId();
        DoRunTask(wstrTaskType, pParams, asyncId, timeout, lTaskId, false);
    };

    void CComponentInstance::DoRunTask( 
                    const std::wstring&     taskName,
                    const KLPAR::Params*    pTaskParams, 
                    const std::wstring&     asyncId,
                    long                    timeout,
                    long                    lTaskID,
                    bool                    bUseCallback,
                    KLSTD::precise_time_t   tmPublish)
    {
        KLPRCI_CALLCTRL();
        if(bUseCallback)
		{
			//AutoCriticalSection cs_call(m_pCallCS);
            KLPRCI_CALLBACK_PREPARE_CALL(RunTasksCallback);
			if(!m_pRunTasksCallback)
                KLSTD_THROW(STDE_NOFUNC);
		};
        CAutoPtr<Params> pData;
        AdjustTasksParams(taskName, (Params*)pTaskParams, &pData);
        std::wstring wstrTsId;
        ;
        {
            //AutoCriticalSection cs_call(m_pCallCS); // we use UnlockInstance
            {
                AutoCriticalSection cs(m_pDataCS);
                task_data_t td(lTaskID, timeout, asyncId, taskName, (Params*)pData);
                m_Tasks.addTask(lTaskID, td);
            };
            LockInstance();
		    KLSTD_TRACE1(2, L"Task %d was inserted\n", lTaskID);
		    KLERR_TRY
			    if(!pData)
				    KLPAR_CreateParams((KLPAR::Params**)&pData);
			    CAutoPtr<StringValue> pAsyncValue;
			    CreateValue(asyncId.c_str(), &pAsyncValue);
			    ((Params*)pData)->ReplaceValue(c_szwTaskAsyncId, pAsyncValue);
                if(bUseCallback)
			        call_RunTasksCallback(taskName, (Params*)pData, lTaskID, timeout);
		    KLERR_CATCH(pError)
			    bool bDoUnlock=false;
			    {
				    AutoCriticalSection cs(m_pDataCS);
				    if(m_Tasks.removeTask(lTaskID))
				    {
					    bDoUnlock=true;
					    //UnlockInstance();
					    KLSTD_TRACE1(2, L"Task %d was destroyed\n", lTaskID);
				    };
			    };
			    if(bDoUnlock)
				    UnlockInstance();
			    KLERR_RETHROW();
		    KLERR_ENDTRY		
            
		    KLERR_BEGIN
			    if(pTaskParams)
                    wstrTsId=GetStringValue(
                                    (KLPAR::Params*)pTaskParams,
                                    KLPRTS::c_szwTaskStorageId);
            KLERR_END
        };

		KLPAR::param_entry_t pars[]=
		{
			KLPAR::param_entry_t(c_evpTaskAsyncID,	Value::STRING_T,	asyncId.c_str()),
            KLPAR::param_entry_t(c_evpTaskTsId,	Value::STRING_T,	wstrTsId.c_str()),
			KLPAR::param_entry_t(c_evpTaskId,	    Value::INT_T,		lTaskID)
		};
		publishEvent(c_EventTaskStarted, pars, KLSTD_COUNTOF(pars), 0, -1L, tmPublish);
    };

	long CComponentInstance::RunTask( const std::wstring & taskName, const KLPAR::Params * pTaskParams, const std::wstring& asyncId, long timeout)
	{
        KLPRCI_CALLCTRL();
        long lResult = KLPRCI_AllocTaskId();
        DoRunTask(taskName, pTaskParams, asyncId, timeout, lResult, true);
		return lResult;
	};

	void CComponentInstance::StopTask(long taskId)
	{
        KLPRCI_CALLCTRL();
		call_TasksControlCallback(taskId, ACTION_STOP);
	};

	void CComponentInstance::SuspendTask(long taskId)
	{
        KLPRCI_CALLCTRL();
		call_TasksControlCallback(taskId, ACTION_SUSPEND);
	};

	void CComponentInstance::ResumeTask(long taskId)
	{
        KLPRCI_CALLCTRL();
		call_TasksControlCallback(taskId, ACTION_RESUME);
	};

    void CComponentInstance::TaskReload(long taskId, KLPAR::Params* pNewSetings)
    {
        KLPRCI_CALLCTRL();
        call_TasksControlCallback(taskId, ACTION_RELOAD, pNewSetings);
    };

	void CComponentInstance::GetTasksList( std::vector<long> & ids) const
	{		
        KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
        m_Tasks.getTasks(ids);
	};

	void CComponentInstance::GetTaskName(long taskId, std::wstring& wstrTaskName) const
	{
        KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
		const task_data_t& taskdata=m_Tasks.findTask(taskId);
		wstrTaskName=taskdata.m_wstrName;
	};

	KLPRCI::TaskState CComponentInstance::GetTaskState(long taskId) const
	{
        KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
		const task_data_t& taskdata=m_Tasks.findTask(taskId);
		return taskdata.m_nState;
	};

	int CComponentInstance::GetTaskCompletion(long taskId) const
	{
        KLPRCI_CALLCTRL();
		AutoCriticalSection cs(m_pDataCS);
		const task_data_t& taskdata=m_Tasks.findTask(taskId);
		return taskdata.m_nCompletion;
	};

	// Native
	void CComponentInstance::LockInstance()
	{
		KLSTD_InterlockedIncrement(&m_lRefCounter);

        KLSTD_TRACE3(
                    4,
                    L"Locked instance '%ls'-'%ls', m_lRefCounter=%u\n",
                    m_ComponentID.componentName.c_str(),
                    m_ComponentID.instanceId.c_str(),
                    m_lRefCounter);

	};
	void CComponentInstance::UnlockInstance()
	{
        KLSTD_TRACE3(
                    4,
                    L"Unlocking instance '%ls'-'%ls', m_lRefCounter=%u\n",
                    m_ComponentID.componentName.c_str(),
                    m_ComponentID.instanceId.c_str(),
                    m_lRefCounter);
		KLERR_BEGIN
            if(KLSTD_InterlockedDecrement(&m_lRefCounter) == 0 && m_bAllowAutoStop)
				call_InstanceControlCallback(INSTANCE_AUTOSTOP);
		KLERR_END
	};

	bool CComponentInstance::AccessCheckTask_InCall(
							const std::wstring& wstrTask,
							AVP_dword dwAccessMask,
							bool bThrow)
	{		
#ifndef KLAVT_OFF
		KLAVT::ACE_DECLARATION*				pAcl=NULL;
		long								nAcl=0;
		if(!m_pSecLib->GetACLForComponentTask(
									m_ComponentID,
									wstrTask,
									(const KLAVT::ACE_DECLARATION**)&pAcl,
									&nAcl))
		{
			KLSTD_THROW(STDE_NOACCESS);
		};
		AutoAcl	aa(m_pSecLib, pAcl, nAcl);
		return KLAVT_AccessCheck_InCall(aa.GetAcl(), aa.GetAclLen(), dwAccessMask, bThrow);
#else
		return true;
#endif
	};

	bool CComponentInstance::AccessCheckTask_InCall(
										long		idTask,
										AVP_dword dwAccessMask,
										bool bThrow)
	{
		wstring wstrTaskName;
		{
			AutoCriticalSection cs(m_pDataCS);
			const task_data_t& taskdata=m_Tasks.findTask(idTask);
			wstrTaskName=taskdata.m_wstrName.c_str();
		};
		return AccessCheckTask_InCall(wstrTaskName, dwAccessMask, bThrow);
	};

};

using namespace KLPRCI;

namespace KLPRCI
{
    int TransportStatusNotifyCallback(
			KLTRAP::Transport::ConnectionStatus	status, 
			const wchar_t*				remoteComponentName,
			const wchar_t*				localComponentName,
			bool						bRemote)
    {
	    KLSTD_TRACE4(
		    4,
		    L"TransportStatusNotifyCallback %d, \'%ls\', \'%ls\', bRemote=%u\n",
		    status,
		    remoteComponentName,
		    localComponentName,
            (int)bRemote);
	    
	    if( bRemote && remoteComponentName && remoteComponentName[0] &&
            localComponentName && localComponentName[0] &&
		    (status == KLTRAP::Transport::ComponentConnected ||
		    status==KLTRAP::Transport::ComponentDisconnected))
	    {
		    KLERR_BEGIN
			    ComponentId idLocal, idRemote;
			    bool bResult=
                        KLTRAP::ConvertTransportNameToComponentId(idLocal, localComponentName) &&
                        KLTRAP::ConvertTransportNameToComponentId(idRemote, remoteComponentName);
			    KLSTD_ASSERT(bResult);
                if(
                    bResult &&
                    idRemote.componentName!=KLPRCP::c_szwTransparentProxyName &&
                    idRemote.componentName!=KLCS_COMPONENT_NAGENT &&
                    idRemote.componentName!=KLCS_COMPONENT_AGENT)
                {
                    KLSTD::CAutoPtr<ComponentInstanceBase> pInstance =
							        g_Instances.getComponentForTransport(idLocal.instanceId);
			        switch(status)
			        {
				        case KLTRAP::Transport::ComponentConnected:
					        KLSTD_TRACE2(
						        3,
						        L"Added new connection from %ls' to '%ls'\n",
						        remoteComponentName,
						        localComponentName);
					        pInstance->LockInstance();
					        break;
				        case KLTRAP::Transport::ComponentDisconnected:
					        KLSTD_TRACE2(
						        3,
						        L"Removed connection from '%ls' to '%ls'\n",
						        remoteComponentName,
						        localComponentName);
					        pInstance->UnlockInstance();
					        break;
			        };
                };
		    KLERR_END	
	    };
	    if(g_pOldTransportCallback)
		    return g_pOldTransportCallback(status, remoteComponentName, localComponentName, bRemote);
	    return 0;
    };
};

static std::wstring KLPRCI_CreateRemoteInstanceId(
									const char* szaRemoteHost,
									const wchar_t* szwWellKnownName)
{
	if(szwWellKnownName && szwWellKnownName[0])
	{
		wchar_t szwBuffer[512]=L"";
		//KLSTD_SWPRINTF(szwBuffer, KLSTD_COUNTOF(szwBuffer), L"%ls-%hs", szwWellKnownName, szaRemoteHost);
		KLSTD_SWPRINTF(szwBuffer, KLSTD_COUNTOF(szwBuffer), L"%ls", szwWellKnownName, szaRemoteHost);
#ifdef N_PLAT_NLM
		return szwWellKnownName;
#else
		return szwBuffer;
#endif
	}
	else
//		return CreateLocallyUniqueString(szaRemoteHost);
		return KLSTD_CreateLocallyUniqueString();
};

KLCSKCA_DECL std::wstring KLPRCI_CreateRemoteInstanceId(
									const wchar_t* szwRemote,
									const wchar_t* szwWellKnownName)
{
	KLSTD_CHKINPTR(szwRemote);
	KLSTD_USES_CONVERSION;	
	return KLPRCI_CreateRemoteInstanceId(KLSTD_W2CA(szwRemote), szwWellKnownName);
};

//\brief Возвращает переданнаный для данного процесса componentId
KLCSKCA_DECL bool KLPRCI_GetStartedProcessComponentId( KLPRCI::ComponentId &componentId )
{
	KLSTD_USES_CONVERSION;	

	KLSTD::CAutoPtr<KLPAR::Params> pEnv;
	KLSTD::CAutoPtr<KLPAR::StringValue> p_wstrValue;
	KLPAR::SplitEnvironment(NULL, &pEnv);
	if(!pEnv->DoesExist(KLPRCI::c_szwEnvComponentIdName))
		return false;
	KLPAR::GetValue(pEnv, KLPRCI::c_szwEnvComponentIdName, &p_wstrValue);
	
	return KLTRAP::ConvertTransportNameToComponentId( componentId, p_wstrValue->GetValue());
}

/*
KLCSKCA_DECL std::string GetHostDnsName()
{
	char name[256]="";
	gethostname(name, KLSTD_COUNTOF(name));
	hostent* hp=gethostbyname(name);
	if(!hp || !hp->h_name)
		KLSTD_THROW(KLSTD::STDE_UNAVAIL);
	return hp->h_name;
};
*/

KLCSKCA_DECL std::wstring KLPRCI_CreateInstanceId(const wchar_t* szwWellKnownName)
{
	return KLPRCI_CreateRemoteInstanceId(""/*GetHostDnsName().c_str()*/, szwWellKnownName);
};

static void PrepareAddress(const wchar_t* szwAddress, std::wstring& wstrRealAddress, InstanceListenerType& type)
{
    if(!szwAddress || !szwAddress[0] || wcscmp(szwAddress, KLPRCI::c_szAddrLocal) ==0)
    {
        wstrRealAddress=KLPRCP::makeLocalAddress(0);
        type=ILT_RPC;
    }
    else
    if(wcscmp(szwAddress, KLPRCI::c_szAddrAny) == 0)
    {
        wstrRealAddress=KLPRCP::makeRemoteAddress(0);
        type=ILT_SOCKET;
    }
    else
    {
        wstrRealAddress=KLPRCP_MakeAddress2(szwAddress, 0);
        type=ILT_SOCKET;
    };
};

KLCSKCA_DECL void KLPRCI_CreateComponentInstance2(
                    const KLPRCI::ComponentId&	componentId,
                    const wchar_t*				szwAsyncId,
					KLPRCI::ComponentInstance**	component)
{
KL_TMEASURE_BEGIN(L"KLPRCI_CreateComponentInstance2", 3)
	KLSTD_CHKOUTPTR(component);

    wstring wstrRealAddress;
    InstanceListenerType type;
    PrepareAddress(NULL, wstrRealAddress, type);
    ;
    KLPRCI::ComponentId idComponent = componentId;
    if(idComponent.instanceId.empty())
        idComponent.instanceId = KLPRCI_CreateInstanceId(NULL);
    ;
    KLSTD::CAutoPtr<KLPRCI::ComponentInstance> pComponent;
    KLPRCI::CComponentInstance* p = new CComponentInstance;
	KLSTD_CHKMEM(p);
    pComponent.Attach(p);
    p->Create(  idComponent,
				(szwAsyncId && szwAsyncId[0])
                    ?   szwAsyncId
                    :   KLSTD_CreateLocallyUniqueString().c_str(),
                NULL,
                wstrRealAddress.c_str(), 
                type);
    ;
    pComponent.CopyTo(component);
KL_TMEASURE_END()
};

KLCSKCA_DECL void KLPRCI_CreateComponentInstance(
										const ComponentId&	componentId,
										ComponentInstance**	component,
										unsigned short*		pDesirablePorts,
										const wchar_t*		szwAddress)
{
KL_TMEASURE_BEGIN(L"Creating Component Instance", 3)
	KLSTD_CHKOUTPTR(component);

    wstring wstrRealAddress;
    InstanceListenerType type;
    PrepareAddress(szwAddress, wstrRealAddress, type);
    ;
    KLPRCI::ComponentId idComponent = componentId;
    if(idComponent.instanceId.empty())
        idComponent.instanceId=KLPRCI_CreateInstanceId(NULL);
    ;
    KLSTD::CAutoPtr<KLPRCI::ComponentInstance> pComponent;
    KLPRCI::CComponentInstance* p = new CComponentInstance;
    KLSTD_CHKMEM(p);
    pComponent.Attach(p);
    p->Create(idComponent, NULL, pDesirablePorts,wstrRealAddress.c_str(), type);
    pComponent.CopyTo(component);
KL_TMEASURE_END()
};

KLCSKCA_DECL void KLPRCI_CreateDllComponentInstance(
										const ComponentId&	componentId,
										KLPAR::Params*		pParams,
										ComponentInstance**	component,
										unsigned short*		pDesirablePorts,
										const wchar_t*		szwAddress)
{
KL_TMEASURE_BEGIN(L"Creating Dll Component Instance", 3)
	KLSTD_CHKOUTPTR(component);

    wstring wstrRealAddress;
    InstanceListenerType type;
    PrepareAddress(szwAddress, wstrRealAddress, type);
    ;
	CAutoPtr<StringValue> p_wstrAsyncId;
	if(pParams && pParams->DoesExist(c_ComponentAsyncId))
		GetValue(pParams, c_ComponentAsyncId, &p_wstrAsyncId);
    ;
    KLPRCI::ComponentId idComponent = componentId;
    if(idComponent.instanceId.empty())
        idComponent.instanceId = KLPRCI_CreateInstanceId(NULL);
    ;
    KLSTD::CAutoPtr<KLPRCI::ComponentInstance> pComponent;
    KLPRCI::CComponentInstance* p = new CComponentInstance;
	KLSTD_CHKMEM(p);
    pComponent.Attach(p);
    p->Create(  idComponent,
				p_wstrAsyncId
                    ?   p_wstrAsyncId->GetValue()
                    :   NULL,
                pDesirablePorts,
                wstrRealAddress.c_str(), 
                type);
    ;
    pComponent.CopyTo(component);
KL_TMEASURE_END()
};

KLCSKCA_DECL long KLPRCI_AllocTaskId()
{
	return KLSTD_InterlockedIncrement(&CComponentInstance::m_nLastTaskID);
};

IMPLEMENT_MODULE_INIT_DEINIT(KLPRCI)

IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL, KLPRCI)
	KLPRCI::RegisterGSOAPStubFunctions();

	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLPRCI::c_errorDescriptions,
		KLSTD_COUNTOF(KLPRCI::c_errorDescriptions));

	KLERR_InitModuleLocalizationDefaults(
		KLCS_MODULENAME,
		KLPRCI::c_LocErrDescriptions,
		KLSTD_COUNTOF(KLPRCI::c_LocErrDescriptions));

	g_pOldTransportCallback=KLTR_GetTransport()->SetStatusCallback(TransportStatusNotifyCallback);
//#ifdef _WIN32
	//g_pComponents = Create_InProcess();
//#endif
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL, KLPRCI)
	//if(g_pComponents)
		//g_pComponents->Shutdown();
    //
    g_lckModule.Wait();
    KLTR_GetTransport()->SetStatusCallback(g_pOldTransportCallback);
	g_pOldTransportCallback=NULL;
    //g_pComponents = NULL;	
    KLSTD_ASSERT(KLPRCI::g_Instances.getComponentsNumber() == 0);
	KLPRCI::g_Instances.Clear();
    KLERR_DeinitModuleLocalizationDefaults(KLCS_MODULENAME);
	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);

	KLPRCI::UnregisterGSOAPStubFunctions();
IMPLEMENT_MODULE_DEINIT_END()


KLCSKCA_DECL void KLPRCI_GetRuntimeInfo(
				const std::wstring&			wstrLocation,
				const KLPRCI::ComponentId&	id,
				KLPAR::Params**				ppInfo,
				long						lTimeout)
{
	KLSTD_CHKOUTPTR(ppInfo);
	KLSTD_CHK(lTimeout, lTimeout == KLSTD_INFINITE || lTimeout > 0);
	;
	CAutoPtr<SettingsStorage>	pStorages;
	::KLPRSS_OpenSettingsStorageR(wstrLocation, &pStorages);
	pStorages->SetTimeout(lTimeout);

	CAutoPtr<Params> pParams;
	pStorages->Read(
		id.productName,
		id.version,
		KLPRSS_SECTION_COMMON_SETTINGS,
		&pParams);
	;
	if(!pParams->DoesExist(id.componentName))
		KLSTD_THROW(STDE_NOTFOUND);
	CAutoPtr<Params> pComponent=GetParamsValue(pParams, id.componentName);
	if(!pComponent)
		KLSTD_THROW(STDE_NOTFOUND);
	;
	if(!pComponent->DoesExist(KLPRSS_VAL_INSTANCES))
		KLSTD_THROW(STDE_NOTFOUND);
	CAutoPtr<Params> pInstances=GetParamsValue(pComponent, KLPRSS_VAL_INSTANCES);
	if(!pInstances)
		KLSTD_THROW(STDE_NOTFOUND);	
	;
	if(!pInstances->DoesExist(id.instanceId))
		KLSTD_THROW(STDE_NOTFOUND);
	CAutoPtr<Params> pResult=GetParamsValue(pInstances, id.instanceId);
	if(!pResult)
		KLSTD_THROW(STDE_NOTFOUND);
	pResult.CopyTo(ppInfo);
};


KLCSKCA_DECL bool KLPRCI_GetClientContext(KLPRCI::SecContext** ppContext)
{
    KLSTD_CHKOUTPTR(ppContext);
    KLERR_BEGIN
		CAutoPtr<KLWAT::ClientContext> pContext;
		if(KLSTD_GetGlobalThreadStore()->IsObjectHere(KLWAT_SEC_CONTEXT))
			KLSTD_GetGlobalThreadStore()->GetStoredObject(
													KLWAT_SEC_CONTEXT,
													(KLBase**)&pContext);
		KLPRCI_CreateSecContext(pContext, ppContext);
	KLERR_ENDT(1)
	return (*ppContext)!=NULL;
};

void KLPRCI_LoadComponent(
					ComponentId&	id,
					KLPAR::Params*		pParameters, 
					std::wstring&		asyncId)
{
    //if(!g_pComponents)
        //KLSTD_NOINIT(KLCS_MODULENAME);
	//g_pComponents->LoadComponent(id,pParameters, asyncId);
    KLSTD_NOTIMP();
};

bool KLPRCI_UnloadComponent(ComponentId& id)
{
    KLSTD_NOTIMP();
    //if(!g_pComponents)
        //KLSTD_NOINIT(KLCS_MODULENAME);
	//return g_pComponents->UnloadComponent(id.instanceId);
}

KLCSKCA_DECL void KLPRCI_SetShutdownFlag()
{
    //if(!g_pComponents)
        //KLSTD_NOINIT(KLCS_MODULENAME);
	//g_pComponents->SetShutdownFlag();
};

void KLPRCI_UnloadInProcessComponents()
{
    //if(!g_pComponents)
        //KLSTD_NOINIT(KLCS_MODULENAME);
	//g_pComponents->UnloadAll();
};

void KLPRCI_GetLoadedInProcessComponents(
						KLPRCI::ComponentId&			filter,
						std::list<KLPRCI::ComponentId>&	ids)
{
    //if(!g_pComponents)
        //KLSTD_NOINIT(KLCS_MODULENAME);
	//g_pComponents->Find(filter, ids);
};
