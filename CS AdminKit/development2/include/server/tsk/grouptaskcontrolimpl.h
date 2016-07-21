/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TSK/GroupTaskControlImpl.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Заголовок серверной реализации интерфейса GroupTaskControl
 *
 */

#ifndef __PRGT_GROUPTASKCONTROLIMPL_H__
#define __PRGT_GROUPTASKCONTROLIMPL_H__

#include <server/tsk/grouptaskcontrolsrv.h>
#include <std/sch/scheduler.h>
#include <server/tsk/grouptaskcommonsrv.h>
#include <server/tsk/commonsrvtasktools.h>
#include <std/tmstg/timeoutstore.h>
#include <server/tsk/remoteinstall.h>
#include <server/evp/transeventsource.h>
#include <server/srvinst/klserver.h>
#include <server/gsyn/srv_gsyn.h>
#include <kca/prci/producttaskscontrol.h>
#include <transport/wat/common.h>
#include <server/evp/notificationpropertiesimpl.h>
#include <transport/wat/common.h>
#include <common/reporter.h>
#include <server/pkg/fileutils.h>
#include <server/pkg/packagesimpl.h>
#include <server/srv_common.h>
#include <server/srvinst/srvinstdata.h>
#include <server/evp/eventsprocessorsrv.h>
#include <server/srvhrch/srvhrch_srv.h>
#include <server/tsk/syncpkg.h>
#include <server/tsk/srv_upgr.h>

#define KLTSK_RI_VIRTUAL_PACKAGE_PATH		L"KLTSK_RI_VIRTUAL_PACKAGE_PATH"// STRING_T
#define KLTSK_RI_PACKAGES_INFO				L"KLTSK_RI_PACKAGES_INFO"		// PARAMS_T
#define KLTSK_RI_PACKAGE_PROD_DISPL_NAME	L"KLTSK_RI_PACKAGE_PROD_DISPL_NAME"	// STRING_T
#define KLTSK_RI_PACKAGE_PROD_DISPL_VER		L"KLTSK_RI_PACKAGE_PROD_DISPL_VER"	// STRING_T

bool KLSRV_IsGlobalTask(KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo, KLSTD::CAutoPtr<KLSTD::CriticalSection>);

int KLSRV_GetMaxTryCount(KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo, KLSTD::CAutoPtr<KLSTD::CriticalSection>);

bool KLSRV_ShallStopTask(KLSRV::GroupTaskParams* pGroupTaskParams, KLSTD::CAutoPtr<KLSTD::CriticalSection>);

void KLSRV_CreateSimpleTaskNotoficationDescr(long nDaysToStore, KLPAR::Params** ppParams);

long KLSRV_GetMainPackageId(const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, KLSTD::CAutoPtr<KLSTD::CriticalSection>);

long KLSRV_GetPackageId(const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, int nPkgNumber, KLSTD::CAutoPtr<KLSTD::CriticalSection>);

void KLSRV_AdjustRICompleteParams(int nTotalPackagesCount, int nPackageNumber, KLTSK::HostState& hsNewState, int& nProgress);

std::wstring KLSRV_GetUninstallVirtualPackagePath(const std::wstring& wstrFolder);

std::wstring KLSRV_GetUninstallVirtualPackageNetworkPath(const std::wstring& wstrFolder);

namespace KLUPDSRV{
	class UpdateSrvImpl;
};

namespace KLSRV
{
	struct GrpTskStartProps{
		AVP_dword m_dwProps;
		std::wstring m_wstrStartId;
        std::wstring m_wstrDomainToPerform;
        std::wstring m_wstrHostToPerform;
		GrpTskStartProps(
			AVP_dword dwProps,
			std::wstring wstrStartId = L"",
			std::wstring wstrDomainToPerform = L"",
			std::wstring wstrHostToPerform = L"")
		:
			m_dwProps(dwProps),
				m_wstrStartId(wstrStartId.empty()? KLSTD_CreateGUIDString() : wstrStartId),
			m_wstrDomainToPerform(wstrDomainToPerform),
			m_wstrHostToPerform(wstrHostToPerform)
		{
		}
	};

	struct wstring_i_less : std::binary_function<std::wstring, std::wstring, bool>
	{
		bool operator()(const std::wstring& _X, const std::wstring& _Y) const
		{
			return _wcsicmp( _X.c_str(), _Y.c_str() ) < 0;
		}
	};

	struct TaskStatusIterator : KLSTD::KLBaseImpl< KLTMSG::TimeoutObject >
	{
		KLTMSG_DECLARE_TYPEID(KLSRV::TaskStatusIterator)

		KLDB::DbRecordsetPtr pRecordset;

		std::set<std::wstring, wstring_i_less> setHostsInDB;

        KLTSK::HostState hsDefault;

#ifdef _USE_STATIC_HOSTS_VECTOR_
		HostSet setHosts;
		int nHostIdx;

		TaskStatusIterator() : nHostIdx(-1) {}
#else
		TaskStatusIterator(){}
#endif
	};
    
    class InProcPRTS
    {
    public:
        ~InProcPRTS();

        void Initialize( 
            const KLPRCI::ComponentId & cidMaster,
            const std::wstring & wstrTSPath,
            bool bGroupTasks,
            bool bAddAgentConnection );

        KLSTD::CAutoPtr<KLPRTS::TasksStorage2> GetTasksStorage();
    private:
        std::wstring m_wstrPRESServerId;
        KLSTD::CAutoPtr<KLPRTS::TasksStorage2> m_pTasksStorage;
    };

	class GroupTaskControlImplBase :
		public KLTSK::GroupTaskControlSrv,
		public KLTP::ThreadsPool::Worker,
		public KLSRV::Deinitializer{
	};

	class GroupTaskControlImpl : public KLSTD::KLBaseImpl<GroupTaskControlImplBase>{
    public:
        GroupTaskControlImpl();
        
        ~GroupTaskControlImpl();
        
        virtual void Initialize(
            KLSRV::SrvData* pSrvData,
            KLSTD::CAutoPtr<PackagesImpl> pPackagesImpl,
            KLSTD::CAutoPtr<NotificationPropertiesImpl> pNotificationPropertiesImpl,
            KLSTD::CAutoPtr<KLEVP::TransEventSource> pTransEventSource,
            KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> pEventsProcessor,
            CommonSrvGroupTaskTools*    pCommonSrvGroupTaskTools,
			long nSouperGroupId,
			KLUPDSRV::UpdateSrvImpl* pUpdateSrvImpl,
			KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> pSrvHierarchyImpl);
        
		void FinalInitialize();
		
        // Deinitializer methods
        virtual void Deinitialize();

		// KLTP::ThreadsPool::Worker methods:
		virtual int RunWorker(KLTP::ThreadsPool::WorkerId wId);

        // TasksStorage stubs:
        virtual void ResetTasksIterator (
			KLDB::DbConnectionPtr pDbConn,
			KLTSK::EStorageType eStorageType,
            int nTargetId,
            const KLPRCI::ComponentId & filter,
            const std::wstring & taskName,
            std::wstring & wstrIteratorId );

        virtual bool GetNextTask (
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrIteratorId,
            KLPRCI::ComponentId & id,
            std::wstring & taskName,
			bool& bStartOnEvent,
            KLSCH::Task ** task,
			KLPAR::Params** params,
            KLPAR::Params** paramsTaskInfo );

        virtual void ReleaseTaskIterator( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrIteratorId );

        virtual void GetTaskByID (
			KLDB::DbConnectionPtr pDbConn,
			int taskId,
            KLPRCI::ComponentId & id,
            std::wstring & taskName,
			bool& bStartOnEvent,
            KLSCH::Task ** task,
			KLPAR::Params** params,
            KLPAR::Params** paramsTaskInfo,
			bool bIncludeHostsList = true);

        virtual void GetTaskStatistics(
			KLDB::DbConnectionPtr pDbConn,
			int taskId,
			bool bCheckIfNeedSlaveServers,
			bool bNeedCompletedPercent,
			KLPAR::Params** ppStatistics );

        virtual int AddTask (
			KLDB::DbConnectionPtr pDbConn,
            KLTSK::EStorageType eStorageType,
            int nTargetId,
            const KLPRCI::ComponentId & compId,
            const std::wstring & wstrTaskName,
            KLSCH::Task* pTask,
            KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
            KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
            bool bStartSyncIfAppropriable = true,
			const std::wstring& wstrFileIdentityToUse = L"",
			bool bRecreatingSuperTask = false );

        virtual void UpdateTask ( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId, 
			KLPAR::Params* paramsTaskData );

        virtual void DeleteTask( 
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId, 
			bool bCheckSupergroup);

        virtual void GetTaskStartEvent (
			KLDB::DbConnectionPtr pDbConn,
			const int taskId, 
			KLPRCI::ComponentId & filter,
			std::wstring & eventType,
			KLPAR::Params ** bodyFilter);

		virtual void SetTaskStartEvent (
			KLDB::DbConnectionPtr pDbConn,
			const int taskId, 
			const KLPRCI::ComponentId & filter,
			const std::wstring & eventType,
			const KLPAR::Params * bodyFilter);

        
        // GroupTaskControl stubs
        virtual void ForbidTaskForGroup( 
			KLDB::DbConnectionPtr pDbConn,
			int nGroupId, 
			int nTaskId );
        
        virtual void RunTaskNow(
			KLDB::DbConnectionPtr pDbConn,
            int nTaskId,
            const std::wstring & wstrDomainName,
            const std::wstring & wstrHostName,
            bool bEvenIfAlreadySucceeded,
			const std::wstring & wstrActionId);

        virtual void SuspendTask( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId);

        virtual void ResumeTask( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId);

        virtual void CancelTask( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId);

        // GroupTaskStatus stubs:
        virtual void ResetHostIteratorForTaskStatus(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrTaskId,
		    const std::vector<std::wstring>& vectFields,
			const std::vector<KLCSP::field_order_t>& vectFieldsToOrder,
            KLTSK::HostState       hsHostStateMask,
		    long            nLifetime,
            std::wstring&   wstrIteratorId );

        virtual bool GetNextHostStatus(
			KLDB::DbConnectionPtr pDbConn,
            std::wstring wstrHostIteratorId,
            long& nCount,
            KLPAR::Params** ppParHostStatus );

        virtual void ReleaseHostStatusIterator( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrIteratorId );
        
		virtual long GetHostStatusRecordsCount( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrHostIteratorId );

		virtual int GetHostStatusRecordRange( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrHostIteratorId, 
			long nStart, 
			long nEnd, 
			KLPAR::Params** ppParHostStatus );

        virtual void GetAllTasksOfHost(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            std::vector<int> & vectTaskIds );

        virtual int GetGTCIdFromTSId( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring & wstrTSId );

        virtual std::wstring GetTSIdFromGTCId( 
			KLDB::DbConnectionPtr pDbConn,
			int nGTCId );

        virtual std::wstring GetTaskFilePathFromIdentity( const std::wstring& wstrIdentity );

        virtual KLSCH::Task* FindTaskInScheduler(int nTaskId, bool bThrowOnError = true );

        virtual std::wstring GetTskDiskStorage();

        virtual void OnSyncNotification(
			KLDB::DbConnectionPtr pDbConn,
			long nSyncId, 
			long nHostId, 
			bool bSuccess, 
			const std::wstring & wstrDescr, 
			const KLSTD::precise_time_t& tmSyncStart);

        virtual void RecreateSuperTask(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrSsType);

        virtual void DeleteSuperTask(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrSsType);
		
		virtual bool ChangeSharedFolderLocation(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrLocalPathToUpdates, 
			const std::wstring& wstrNetworkPathToUpdates);

		virtual KLSTD::CAutoPtr<KLSRV::RemoteInstallEventPreprocessor> GetRemoteInstallEventPreprocessor();

        void RunTaskScopeNow(
			KLDB::DbConnectionPtr pDbConn,
			const std::vector<long> & vectTaskId);

		virtual bool UpgradeTasks(KLDB::DbConnectionPtr pDbConn);

		virtual bool UpdateRestoredTasks(KLDB::DbConnectionPtr pDbConn);

		/*!
		  \brief ForceRetranslateUpdates.

			\param pParams	[in] Произвольные дополнительные параметры ретрансляции.

			\returns - false, если задача рентрансляции обновлений не существует на подчиненном сервере.
		*/
		virtual bool ForceRetranslateUpdates(						
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams);

		virtual void GetTaskGroupId( 
			KLDB::DbConnectionPtr &pDbConn,
			int taskId,
			long &nGroupId );

		virtual void MarkTaskStatisticsChanged(long nTaskId);

		virtual void MarkHostTasksStatisticsChanged(long nHostId);

		virtual void MarkGroupTasksStatisticsChanged(long nGroupId);

		virtual KLSTD::CAutoPtr<KLPRTS::TasksStorage2> GetSrvTasksStorage();

		virtual void UpdateRiTaskFileStuff(
			KLSRV::GroupTaskParams* pGroupTaskParams,
			KLSCH::Task* pTask = NULL);

        virtual std::wstring GetSrvTskDiskStorage();

        virtual void SetNewStateForAllTaskHosts(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
            KLTSK::HostState hsNewStateForAllTaskHosts,
			KLTSK::RIFailReason eFailReason,
			const std::wstring& wstrStateDescr,
			KLTSK::HostState hsStateCodeToExclude1, 
			KLTSK::HostState hsStateCodeToExclude2,
			KLTSK::HostState hsStateCodeToExclude3);

		virtual long GetMainPackageId(KLDB::DbConnectionPtr pDbConn, const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS);

		virtual long GetPackageId(KLDB::DbConnectionPtr pDbConn, const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, int nPkgNumber, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS);
		
		virtual void UpdateHostsSyncObjH(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nTaskId,
                    long*                   pHosts,
                    size_t                  nHosts,
					KLSRV::SrvGS_UpdateAction      nAction = KLSRV::SGSUA_REPLACE);

		virtual std::wstring GetCatchedRunningTasks(KLDB::DbConnectionPtr pDbConn, long nPackageId);

		virtual void UpdateTasksPackageId(KLDB::DbConnectionPtr pDbConn, const std::wstring& wstrPackageGUID, long nPackageId);
		
		virtual void StartTask(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrTaskTsId,
			const std::wstring& wstrTaskTypeName,
			KLSTD::CAutoPtr<KLPAR::Params> parParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parOutParams);

		virtual bool GetDependentTasksForPkg(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId, 
			std::vector<KLPKG::Packages2::task_info_t>& vecDependTasks);

		virtual void StartTasksForCondition(KLDB::DbConnectionPtr pDbConn, KLPRTS::ETskStartCondition eCond, AVP_long lCondData, AVP_long lCondDataEx);		

		virtual bool StartRemoteInstallation(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped);

		virtual void ReportRemoteInstallationResult(
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrServerHostId,
			std::wstring wstrTaskFileId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent);

		virtual void UpdateVisibleHosts(const std::vector<long>& vecVisibleHosts);

		virtual long CallEventsProcessor(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			const std::wstring & wstrTaskFileIdentity,
			const std::wstring & wstrDomainName,
			const std::wstring & wstrHostName,
			KLPRCI::TaskState tsOldState,
			const KLTSK::HostState hsNewHostState,
			KLSRV::BoolEx bNeedReboot,
			int nSeverity,
			const std::wstring & wstrTaskDisplayName,
			const std::wstring & wstrDescription,
			KLSTD::CAutoPtr<KLPAR::Params> parAdditionalParams,
			time_t eventTime = -1,
			long eventTimeMsec = -1);

		void UpdateRetranslationList(KLDB::DbConnectionPtr pDbConn, bool bRunTaskIfNeeded = false);

		// implementation:
        void TaskWorker(GroupTaskParams* pGroupTaskParams);

        void ItsTimeToRunForTask(long nTaskId, GrpTskStartProps startProps);

        void ItsTimeToRunForTask(GroupTaskParams* pGroupTaskParams, GrpTskStartProps startProps);
		
		void SetSrvHierarchyImpl(KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> pSrvHierarchyImpl);

		void GetApplyPatches(KLDB::DbConnectionPtr pDbConn, bool& bSrv, bool& bNagt);
		
    private:
    
		class GroupTaskIterator : public KLSTD::KLBaseImpl<KLTMSG::TimeoutObject>
        {
        public:
			KLTMSG_DECLARE_TYPEID(KLSRV::GroupTaskIterator)

			GroupTaskIterator():eStorageType(KLTSK::Common), nTargetId(KLGRP::GROUPID_INVALID_ID){};
			KLDB::DbRecordsetPtr pRecordset;
            KLPRCI::ComponentId compIdFilter;
            std::wstring wstrTaskNameFilter;
            KLTSK::EStorageType eStorageType;
            long nTargetId;
        };

        KLSTD::CPointer<KLSTD::Thread>              m_pSchedulerThread;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCSTask;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCSTaskParams;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCSTaskFile;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCSCheckSendTasksStatistics;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCSScheduler;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCSVisibleHosts;
        KLSRV::SrvData* m_pSrvData;
        KLSTD::CAutoPtr<PackagesImpl> m_pPackagesImpl;
        KLSTD::CAutoPtr<NotificationPropertiesImpl> m_pNotificationPropertiesImpl;
        KLSTD::CAutoPtr<KLEVP::TransEventSource> m_pTransEventSource;
        RemoteInstall m_remoteInstall;
		SyncPkg m_SyncPkg;
		SrvUpgrTsk m_SrvUpgrTsk;
        CommonSrvGroupTaskTools * m_pCommonSrvTaskTools;
        KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> m_pEventsProcessor;

        std::vector<KLEV::SubscriptionId> m_vectTESSubscriptions;
		
		long m_nSuperGroupId;
		std::wstring m_wstrTskDiskStorage;

		KLUPDSRV::UpdateSrvImpl* m_pUpdateSrvImpl;

		KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> m_pSrvHierarchyImpl;

        long m_nTempListId;

        std::wstring m_wstrServerTSPath;

		std::wstring m_wstrServerFullVersion;

		KLSTD::CAutoPtr<KLSRV::SrvGroupSync> m_pSrvGroupSync;

		KLTP::ThreadsPool::WorkerId m_idCheckSendTasksStatistics;
		KLTP::ThreadsPool::WorkerId m_idCheckMissedTasks;
		KLTP::ThreadsPool::WorkerId m_idCheckDeletedTasks;
		KLTP::ThreadsPool::WorkerId m_idCheckTasksToRecreate;
		KLTP::ThreadsPool::WorkerId m_idCheckSyncEvents;
		KLTP::ThreadsPool::WorkerId m_idCheckReportsCache;
		typedef std::map<int, bool> TBoolContainer;
		TBoolContainer m_mapTasksUpdated;
		TBoolContainer m_mapGroupsUpdated;
		TBoolContainer m_mapHostsUpdated;

		unsigned long m_ulLastFullStatisticsUpdateTime;
		
		InProcPRTS m_SrvInProcPRTS;
		KLSTD::CAutoPtr<KLPRTS::TasksStorage2> m_pSrvTasksStorage;

		std::map<long, bool> m_mpVisibleHosts;

		bool m_bDeletedTaskPresent;
		bool m_bTaskToRecreatePresent;
		unsigned long m_ulLastDeletedTaskCheckTime;
		unsigned long m_ulLastCheckTasksToRecreate;
		unsigned long m_ulLastCheckSyncEvents;
		bool m_bForceCheckSyncEvents;

		bool m_bApplyPatchesReady;
		bool m_bApplyPatchesSrv;
		bool m_bApplyPatchesNagt;

		bool	m_bCheckBasesFlag;
		long	m_nCheckBasesTaskId;

		bool	m_bCheckBasesDoSyncFolders;

		std::wstring					m_wstrSyncFoldersUpdTaskName;
		KLPRCI::ComponentId				m_syncFoldersUpdTaskCompId;
		KLSTD::CAutoPtr<KLPAR::Params>	m_pSyncFoldersUpdTaskParams;


		// implementation:
		std::wstring GetSuperTaskFileIdentity(
			const std::wstring& wstrFullPath);

		void UpdateTaskEx( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId, 
			KLPRCI::ComponentId compId,
			std::wstring taskName,
			KLSTD::CPointer<KLSCH::Task> pNewTask,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			ETaskUpdateActor eUpdateActor,
			bool bUpdateRunning,
			SuperGroupUpdateAction sg = SG_DENY,
			bool bSkipPrescribeRetranslationDirPath = false);

        void GenerateEventsForAllTaskHosts(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			int nSeverity,
			const std::wstring& wstrEventType,
			const std::wstring& wstrDescr,
			KLTSK::HostState hsNewStateForAllTaskHosts,
			KLTSK::HostState hsStateCodeToExclude1,
			KLTSK::HostState hsStateCodeToExclude2,
			KLTSK::HostState hsStateCodeToExclude3,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			const KLPRCI::ComponentId & compId,
			const std::wstring& wstrTaskName); 

        void GenerateSyncEventsForAllTaskHosts(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			KLTSK::HostState hsNewStateForAllTaskHosts,
			KLTSK::HostState hsStateCodeToExclude1,
			KLTSK::HostState hsStateCodeToExclude2,
			KLTSK::HostState hsStateCodeToExclude3,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			const KLPRCI::ComponentId & compId,
			const std::wstring& wstrTaskName); 

        void CreateTaskThread( GroupTaskParams* pGroupTaskParams );

		static void OnTransactionalEventStatic(
			const KLEVP::caller_id_t& caller,
			void* pDatabase,
			const std::wstring & eventType,
			const KLPAR::Params * eventBody,
			KLSTD::precise_time_t tEventRiseTime,
			void* pParam );

        void OnTransactionalEvent(
            const KLEVP::caller_id_t& caller,
	        void* pDatabase,
	        const std::wstring & wstrEventType,
	        const KLPAR::Params * pParamsEventBody,
	        KLSTD::precise_time_t tEventRiseTime );
		
        KLSTD::CPointer<KLSCH::Scheduler>            m_pScheduler;
		bool										 m_bStopScheduler;

        std::wstring GetTaskStorageForRemoteTasksRelPath( long nTargetId );

		void GetUpdaterTasks(
			KLDB::DbConnectionPtr pDbConn,
			std::vector<std::wstring>& vecTaskIds, 
			bool bServersOnly);

		void GetUpdaterTasks(
			KLDB::DbConnectionPtr pDbConn,
			std::vector<long>& vecTaskIds, 
			bool bServersOnly);

        void RunTaskNowEx(
			KLDB::DbConnectionPtr pDbConn,
            GroupTaskCommonData & taskCommonData,
            const std::wstring & wstrDomainName,
            const std::wstring & wstrHostName,
            bool bEvenIfAlreadySucceeded,
			const std::wstring & wstrActionId,
			bool bSkipSlaveServers);

        void RunTaskScopeNowEx(
			KLDB::DbConnectionPtr pDbConn,
			const std::vector<long> & vectTaskId,
			bool bSkipSlaveServers);

        void CancelTaskEx( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId,
			bool bSkipSlaveServers);

        // function shall be called within ADO_CALL_* block
        int AddGroupTaskToDB( 
			KLDB::DbConnectionPtr pDbConn,
			KLTSK::EStorageType eStorageType,
			int nTargetId,
			KLPAR::Params* paramsTaskParams,
			KLPAR::Params* paramsTaskInfo,
			const std::wstring & wstrTaskName,
			const KLPRCI::ComponentId & cid,
			bool bTaskMustBeUnique,
			bool bRunMissed);

        bool PrescribeRetranslationDirPath( KLPAR::Params * parTaskParams );

        // function shall be called within ADO_CALL_* block
        void UpdateGroupTaskInDB(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
			const GroupTaskCommonData& taskCommonData, 
            KLPAR::Params* paramsTaskParams,
            KLPAR::Params* paramsTaskInfo,
			bool bRunMissed);

        // function shall be called within ADO_CALL_* block
		
        void UpdateTaskFileStuff(
			KLDB::DbConnectionPtr pDbConn,
            GroupTaskCommonData & taskCommonData,
            const KLSCH::Task * pNewTask,
            KLPAR::Params * parTaskParams,
            KLPAR::Params * parTaskInfo,
			KLPAR::Params * parMandatoryTaskParams,
			KLPAR::Params * parMandatoryTaskParamsDeleteMask,
            GroupTaskParams ** ppGroupTaskParamsNew,
            long & nNewRemoteSyncId,
			bool& bNeedRemoveRemoteSyncId);

        void UpdateFileIdentityAndSyncIdInDB(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
            const std::wstring & wstrFileIdentity,
            long nRemoteSyncId,
			bool bIsRemoteInstallTask);

        // function shall be called within ADO_CALL_* block
        void DeleteTaskFromDB(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId );
        
        KLSTD::CAutoPtr<KLPAR::Params> SetGroupTaskAction(
			KLDB::DbConnectionPtr pDbConn,
            GroupTaskCommonData & taskCommonData,
            KLTSK::GroupTaskAction gta,
			std::wstring wstrActionId,
			bool bSkipSlaveServers);

		void GetInstalledProducts(
			KLDB::DbConnectionPtr pDbConn, 
			KLSRV::UpdProdVerMap& vecProdVers,
			KLSRV::UpdCompVerMap& vecCompVers,
			bool bIncludeSlaveServers);

        void ReadTasksFromDB(KLDB::DbConnectionPtr pDbConn);

        void AddRemoteCleanerTask();

		void AddRemoteSubPkgTask();

        long InsertTempHostList( 
			KLDB::DbConnectionPtr pDbConn,
            KLPAR::Params* parTaskParams,
            KLPAR::Params* parTaskInfo ); // shall be called within KLADO_CALL_* block

        void SetNotificationProperties(
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId, 
			KLPAR::Params * parTaskInfo );

        void SetNotificationProperties( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring & wstrTaskId, 
			KLPAR::Params * parTaskInfo );

        void CreateTaskFileStuff(
			KLDB::DbConnectionPtr pDbConn,
            int nTargetId,
            int nTaskId,
            const KLPRCI::ComponentId & compId,
            const std::wstring & wstrTaskName,
            const KLSCH::Task* pTask,
            KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
            KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
            GroupTaskDestinatation gtDestination,
            GroupTaskParams ** ppGroupTaskParams,
            long & nRemoteSyncId,
            bool bStartSyncIfAppropriable,
			const std::wstring& wstrFileIdentityToUse,
			std::wstring& wstrFileIdentity);
        
        long AddSynchObject(
			KLDB::DbConnectionPtr pDbConn,
            long nTargetId,
			const std::wstring & wstrType,
            const std::wstring & wstrTaskId,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
			const std::wstring & wstrTSRelPath,
			const std::wstring & wstrFileName,
			HostSet& setHostsForGlobalTask,
			bool bNeedFullPath);

		long AddSlaveSrvSynchObject(
			KLDB::DbConnectionPtr pDbConn,
			long nTargetId,
			const std::wstring & wstrTaskId,
			const std::wstring & wstrFileName);

		void UpdateSyncObject(
			KLDB::DbConnectionPtr pDbConn,
			const GroupTaskCommonData& taskCommonData,
			HostSet& setHosts);

        std::wstring GetTaskStorageForRemoteTasksPath( long nTargetId );

        KLSTD::CAutoPtr<KLPRTS::TasksStorage> GetTaskStorage(
            const std::wstring & wstrTrServerCompName,
            const std::wstring & wstrServerAddr,
            const KLPRCI::ComponentId & cidServerComponent,
            const std::wstring & wstrPRESServerId );

		bool DeleteSubtasks(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams, 
			bool bInclPersist);

		void AddUpdateRetranslationList(KLDB::DbConnectionPtr pDbConn);

		void AddCleanOldEventsTask();
		void CleanOldEvents();

		void DoSyncPackage(KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams);

		void DoUpgradeSrv(KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams);

		void CreateTmpHostListTable(KLDB::DbConnectionPtr pDbConn);

		void CheckStartTasksForNewUpdates(
			KLDB::DbConnectionPtr pDbConn, 
			int nTaskId,
			const KLPAR::Params * pParamsEventBody);

		unsigned long GroupTaskSchedulerWorker();
		
		std::wstring AdjustWakeOnLanTask(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId,
			const KLSCH::Task* pTask,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo);

		bool DeleteWakeOnLanTask(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams);

		void CheckSendTasksStatistics();

		void CheckMissedTasks();

		std::wstring GetSrvTasksStoragePath(bool bFullPath);

		void ReadOldRITask(const std::wstring& wstrIdentity, KLPAR::Params** ppParams, int nTaskId = 0);

		void DeleteOldRITask(const std::wstring& wstrIdentity);

		bool CheckTaskAction(
			KLDB::DbConnectionPtr pDbConn, 
			int nTaskId, 
			KLSTD::CAutoPtr<KLPAR::Params> pParTaskInfo,
			KLTSK::GroupTaskAction& gta,
			std::wstring& wstrActionId);

		void AdjustSystemParameters(
			ETaskUpdateActor eUpdateActor,
			KLPRCI::ComponentId compId,
			std::wstring taskName,
			KLPAR::ParamsPtr parOldTaskParams, 
			KLPAR::ParamsPtr parOldTaskInfo, 
			KLPAR::ParamsPtr parTaskParams, 
			KLPAR::ParamsPtr parTaskInfo,
			bool& bActionSkipped);

		void RemoveSyncIdInDB(
  			KLDB::DbConnectionPtr pDbConn,
			long nTaskId);

		bool IsRITaskRunning(KLDB::DbConnectionPtr pDbConn, long nTaskId);

		void UpdatePackageIdInDB(
  			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			long nPackageId );

		void UpdateRITaskPackageId(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrPackageGUID, 
			long nPackageId,
			long nTaskId,
			bool bUpdateAttachedTasks);
		
		void UpdateSyncPkgPackageId(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrPackageGUID, 
			long nPackageId,
			long nTaskId);
		
		void PerformGroupTaskAction(
			KLDB::DbConnectionPtr pDbConn, 
			KLTSK::GroupTaskAction gta, 
			long nTaskId, 
			std::wstring wstrActionId);
		
		void UpgradeTaskSyncAbsolutePath(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskCommonData& taskCommonData);

		void CheckUpgradeAutoUpdateTask(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskCommonData& taskCommonData);

		void DeleteGhostTasksAfterRestore(KLDB::DbConnectionPtr pDbConn);

		bool WriteDeinstallationRebootProps(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrVirtualPackageFolderPath, 
			KLPAR::ParamsPtr parTaskParams);

		void WriteInstallationProps(
			KLDB::DbConnectionPtr pDbConn, const std::wstring& wstrFileIdentity, KLPAR::ParamsPtr parTaskParams);
		
		void DoRecreateSuperTask(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrSSPath,
			const std::wstring& wstrRelpath,
			const std::wstring& wstrTaskFileIdentity,
			long nTaskId);

		void CheckDeletedTasks();
		void CheckTasksToRecreate();
		void CheckSyncEvents();

		void CheckDeleteGhostFTs();

		void AdjustMoveRule(
			KLDB::DbConnectionPtr pDbConn,	
			int nTaskId,
			const std::wstring& wstrFileIdentity,
			KLPAR::ParamsPtr parTaskInfo,
			KLPAR::ParamsPtr parTaskParams);

		void AdjustBackupDACL(const std::wstring& wstrBackupPath);

		void DoActionAfterRetranslation(KLDB::DbConnectionPtr pDbConn, 
								int nTaskId, const KLPAR::Params *pParamsEventBody);

		void ReadCheckBasesTaskSettings(KLDB::DbConnectionPtr pDbConn);

		bool IsTaskCompleted(KLDB::DbConnectionPtr pDbConn, int nTaskId);

		void SyncUpdatersFoldersAfterCheck(int nTaskId);

		void SyncUpdatersFoldersTaskPublishState(int nTaskId, KLTSK::HostState hsNewState,
												const std::wstring &wstrDescr);

		static unsigned long KLSTD_THREADDECL GroupTaskSchedulerWorker( void *argp );
    };    
}

#endif // __PRGT_GROUPTASKCONTROLIMPL_H__
