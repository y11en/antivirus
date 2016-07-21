#ifndef __KL_COMPONENTINSTANCEIMPL_H__
#define __KL_COMPONENTINSTANCEIMPL_H__

#include "./componentinstancebaseimpl.h"

namespace KLPRCI
{
	class CComponentInstance
        :	public KLSTD::RcClassImpl<ComponentInstanceBase>
        ,   public KLSTD::ModuleClass
	{
	public:
        KLSTD_INTERAFCE_MAP_BEGIN(KLPRCI::ComponentInstance2)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ComponentInstance)           
            KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ProductTasksControl3)
		    KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ProductTasksControl2)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ProductTasksControl)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ProductStatistics)
		    KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ProductStatistics2)
		    KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::ProductSettings)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPRCI::TasksControlInternal)
        KLSTD_INTERAFCE_MAP_END()
        
        KLSTD_CLASS_DBG_INFO(KLPRCI::CComponentInstance);
		CComponentInstance();
		virtual ~CComponentInstance();
		;
        void Create(
					const KLPRCI::ComponentId&	componentId,
					const wchar_t*				szwAsyncId,
                    unsigned short*				pDesirablePorts,
					const wchar_t*				szwAddress,
                    InstanceListenerType&       type);
        KLSTD_NOTHROW void Destroy() throw();
        ;		
		// ComponentInstance
		void GetComponentId(KLPRCI::ComponentId &componentId);
		void GetStartParameters(KLPAR::Params** ppParams);
		void GetInstanceId(std::wstring& wstrID) const;
		void GetTaskParams(long idTask, KLPAR::Params* pFilter, KLPAR::Params** ppParams);
		long GetPingTimeout() const;
		void Ping() const;
		ComponentInstanceState GetState() const;
		void SetState(ComponentInstanceState state);
		void GetSettings(KLPRCI::ProductSettings** ppSettings);
		void GetTasksControl(KLPRCI::ProductTasksControl** ppTasks);
		void GetStatistics(KLPRCI::ProductStatistics** ppStatistics);
		void SetInstanceControlCallback(void* context, InstanceControlCallback	callback);
		bool CheckEventsSubscription( const std::wstring &eventType, KLPAR::Params * eventBody );
		void Publish(const std::wstring& eventType, KLPAR::Params* eventBody,long lifetime);
		KLSTD_NOTHROW void InitiateUnload() throw();
        KLSTD_NOTHROW std::wstring GetLocation() throw();
        KLSTD_NOTHROW void Close(bool bSuccess) throw();
        
        virtual void SetInstanceState(
                        ComponentInstanceState  state, 
                        KLSTD::precise_time_t   tmPublish,
                        KLPAR::Params*          pExtraData);

		virtual void PublishEvent(
				const std::wstring&	    eventType, 
				KLPAR::Params*		    eventBody,
                KLSTD::precise_time_t   tmPublish);

        KLSTD_NOTHROW void SetState(ComponentInstanceState state, bool bPublishOnly) throw();
		
		// ProductTasksControl
		void SetState(long taskId, TaskState state, const KLPAR::Params * results);
		void SetTaskCompletion(long taskId, int percent);
		void SetRunTasksCallback(void* context, RunTasksCallback callback);
		void SetTasksControlCallback(void* context, TasksControlCallback callback);
        void SetRunMethodsCallback( 
				            void*				        context,
                            KLPRCI::RunMethodCallback   callback );
		long CallRunTasksCallback(
						const std::wstring & taskName,
						const KLPAR::Params * params,
						const std::wstring& asyncId,
						long timeout);
		void* SetTaskData(long taskId, void* pData);
        void GetTaskParams(long idTask, KLPAR::Params** pParams);
		KLPRCI::ComponentId GetComponentID();
		KLPRCI::ComponentId GetRemoteComponentID();
        
        virtual void SetTaskState(
                        long taskId, 
                        TaskState state, 
                        KLPAR::Params* results,
                        KLSTD::precise_time_t   tmPublish);

	
		// ProductStatistics2
		void SetStatisticsData( KLPAR::Params * statistics);
		void ReplaceStatisticsData( KLPAR::Params * statistics);
		void DeleteStatisticsData(KLPAR::Params * statistics );
        void SetUpdateStatisticsCallback(
						void*					    context,
                        UpdateStatisticsCallback    callback);
		// ProductSettings
		std::wstring GetSettingsStorageLocation (KLPRSS::SS_OPEN_TYPE nOpenType) const;
		void SubscribeOnSettingsChange( 
                    const std::wstring&		productName, 
                    const std::wstring&		version,
                    const std::wstring&		section, 
                    const std::wstring&		parameterName,
					void*					context,
                    SettingsChangeCallback	callback,
					HSUBSCRIBE&				hSubscription);
		void CancelSettingsChangeSubscription(HSUBSCRIBE	hSubscription);
		void NotifyAboutSettingsChange(
				const std::wstring&     productName, 
				const std::wstring&     version,
				const std::wstring&     section, 
				const std::wstring&     parameterName,
				const KLPAR::Value*     oldValue,
				const KLPAR::Value*     newValue,
                KLPRSS::SS_OPEN_TYPE    type);
    // ComponentInstanceBase
		bool IsInitialized(){return m_bInitialized;};
        void GetStatistics(KLPAR::Params * statistics);
        void Stop();
        void Suspend();
        void Resume();
        long RunTask(
								const std::wstring& taskName,
								const KLPAR::Params* params,
								const std::wstring& asyncId=L"",
								long timeout = 0);
		void GetTaskName(long taskId, std::wstring& wstrTaskName) const;
        void StopTask(long taskId);
        void SuspendTask(long taskId);
        void ResumeTask(long taskId);
        void TaskReload(long taskId, KLPAR::Params* pNewSetings);
        void GetTasksList( std::vector<long> & ids) const;
        KLPRCI::TaskState GetTaskState(long taskId) const;
        int GetTaskCompletion(long taskId) const;
        void AddTask(
                        long                    lTaskId,
						const std::wstring&     wstrTaskType,
						KLPAR::Params*          pParams,
						const std::wstring&     asyncId=L"",
						long                    timeout = 0,
                        KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t());

        void RunMethod(
								const std::wstring& MethodName,
								KLPAR::Params* params,
								long timeout,
                                KLPAR::Params** ppResult);
		// Native
        void AdjustTasksParams(
                        const std::wstring&     wstrName,
                        KLPAR::Params*          pData,
                        KLPAR::Params**         ppResutl);
		void LockInstance();
		void UnlockInstance();		
		bool AccessCheckTask_InCall(
								const std::wstring& wstrTask,
								AVP_dword           dwAccessMask,
								bool                bThrow = true);

		bool AccessCheckTask_InCall(
								long		idTask,
								AVP_dword   dwAccessMask,
								bool        bThrow = true);
	protected:
        void GetStartParametersI(KLPAR::Params** ppParams);
        KLSTD_NOTHROW void SetStateI(
                        ComponentInstanceState state, 
                        bool bPublishOnly,
                        KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t()) throw();
        void DoRunTask(
                    const std::wstring&     taskName,
                    const KLPAR::Params*    pTaskParams, 
                    const std::wstring&     asyncId,
                    long                    timeout,
                    long                    lTaskID,
                    bool                    bUseCallback,
                    KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t());

        bool IsRightless();
        void CheckInitialized();
		//int GetAgentPort(const std::wstring& storage);
		std::wstring FindAgent(const std::wstring& storage);
		KLSTD::CAutoPtr<KLPAR::Params>	m_pDataFromAgent;
		volatile long					m_lRefCounter;
		bool							m_bAllowAutoStop;
		long							m_lTimeoutStop;
        long                            m_lTrRecvTimeout;
        const long                      m_lPingTimeout;

		void InitAsyncID(KLPRSS::ComponentInfo* pInfo=NULL);
		void publishEvent(
                        const std::wstring      &wstrEventName,
                        KLPAR::param_entry_t*   pData,
                        int                     nData,
                        long                    lTimeout,
                        long                    lSeverity = -1L,
                        KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t())
		{
			KLPRCI::publishEvent(
                        m_ComponentID, 
                        wstrEventName, 
                        pData, 
                        nData, 
                        lTimeout, 
                        lSeverity,
                        tmPublish);
		};
        void RegisterInstanceInStore(InstanceListenerType type);
		void UnRegisterInstanceInStore();
	public:		
		static long								m_nLastTaskID;
	private:
        KLSTD::CAutoPtr<KLPRSS::SettingsStorage>m_pSS;
		std::wstring							m_wstrListenAddress;
		bool									m_bInitialized;
        std::set<std::wstring>                  m_setTasksToComplement;
        
        KLSTD::ObjectLock                       m_lckRunTasksCallback;
		RunTasksCallback						m_pRunTasksCallback;
		void*									m_pRunTasksCallbackContext;
		void call_RunTasksCallback(const std::wstring& taskName, KLPAR::Params* pData, long lTaskID, long timeout);		
        ;
        KLSTD::ObjectLock                       m_lckTasksControlCallback;
		TasksControlCallback					m_pTasksControlCallback;
		void*									m_pTasksControlCallbackContext;
        void call_TasksControlCallback(long TaskId, TaskAction action, KLPAR::Params* pParams=NULL);
        ;
        KLSTD::ObjectLock                       m_lckRunMethodCallback;
		RunMethodCallback					    m_pRunMethodCallback;
		void*									m_pRunMethodCallbackContext;
        void call_RunMethodCallback(
					            const std::wstring&		wstrMethodName,
                                KLPAR::Params*	        pInParams,
                                long					lTimeout,
                                KLPAR::Params**         pOutParams);

        ;
        KLSTD::ObjectLock                       m_lckInstanceControlCallback;
		InstanceControlCallback					m_pInstanceControlCallback;
		void*									m_pInstanceControlCallbackContext;
		void call_InstanceControlCallback(InstanceAction action);
        ;
        KLSTD::ObjectLock                       m_lckUpdateStatisticsCallback;
        UpdateStatisticsCallback                m_pUpdateStatisticsCallback;
        void*                                   m_pUpdateStatisticsCallbackContext;
        void call_UpdateStatisticsCallback(KLPAR::Params* pFilter);
        ;
		CAdviseSinks<HSUBSCRIBE>				m_Subscriptions;
		CTasks								    m_Tasks;
		const KLPRCI::ComponentId				m_ComponentID, m_AgentID;
		ComponentInstanceState					m_CurState;
		std::wstring							m_wstrClientName, m_wstrConnectionName, m_wstrAgentConnection;
		std::wstring							m_wstrAsyncID;
		
		KLSTD::CAutoPtr<KLPAR::Params>			m_pStatistics;
		const std::wstring						m_wstrSettingsLocation, m_wstrRunTimeLocation;
		KLTRAP::Transport*						m_pTransport;
		int										m_nMyPort;
		const bool								m_bInAgent, m_bInNagent;
		KLAVT::ACE_DECLARATION*					m_pAcl;
		long									m_nAcl;
		KLSTD::CAutoPtr<KLAVT::AccessControlList>	m_pAclObj;
        KLSTD::CAutoPtr<AVTL::AclLibrary>       m_pSecLib;
        mutable KLSTD::ObjectLock               m_lckExternal;
	};
};

#endif //__KL_COMPONENTINSTANCEIMPL_H__
