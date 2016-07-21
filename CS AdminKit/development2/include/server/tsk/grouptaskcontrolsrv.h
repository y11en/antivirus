/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TSK/GroupTaskControlSrv.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Интерфейса GroupTaskControlSrv - серверный интерфейс для обеспечниея
 *          функциональности GroupTaskControl
 */

#ifndef __TSK_GROUPTASKCONTROLSRV_H__
#define __TSK_GROUPTASKCONTROLSRV_H__

#include <srvp/tsk/grouptaskcontrol.h>
#include <server/tsk/commonsrvtasktools.h>
#include <server/tsk/grouptaskcommonsrv.h>
#include <server/db/dbconnection.h>
#include <server/gsyn/srv_gsyn.h>
#include <srvp/pkg/packages.h>

#define KLTSK_RI_FT_ID	L"KLTSK_RI_FT_ID"	// STRING_T

KLPRCI::TaskState KLSRV_GetTaskStateFromHostState(const KLTSK::HostState hsHostState);

KLTSK::HostState KLSRV_GetHostStateFromTaskState(KLPRCI::TaskState taskNewState, int nSeverity, KLTSK::HostState hsDefault);

int KLSRV_GetEventSeverityFromHostState(const KLTSK::HostState hsHostState);

namespace KLTSK
{
    enum EStorageType { Group, Common, Host, GroupAndSupergroups };

    class GroupTaskControlSrv : public KLSTD::KLBase
    {
    public:
		enum SuperGroupUpdateAction{
			SG_DENY,
			SG_ALLOW,
			SG_SKIP
		};

        virtual ~GroupTaskControlSrv()
        {
        }

        // TasksStorage stubs:
        virtual void ResetTasksIterator (
			KLDB::DbConnectionPtr pDbConn,
            KLTSK::EStorageType eStorageType,
            int nTargetId,
            const KLPRCI::ComponentId & filter,
            const std::wstring & taskName,
            std::wstring & wstrIteratorId ) = 0;

        virtual bool GetNextTask (
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring& wstrIteratorId,
            KLPRCI::ComponentId & id,
            std::wstring & taskName,
			bool& bStartOnEvent,
            KLSCH::Task ** task,
			KLPAR::Params** params,
            KLPAR::Params** paramsTaskInfo) = 0;

        virtual void ReleaseTaskIterator( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrIteratorId ) = 0;

        virtual void GetTaskByID (
			KLDB::DbConnectionPtr pDbConn,
			int taskId,
            KLPRCI::ComponentId & id,
            std::wstring & taskName,
			bool& bStartOnEvent,
            KLSCH::Task ** task,
			KLPAR::Params** params,
            KLPAR::Params** paramsTaskInfo,
			bool bIncludeHostsList = true) = 0;

        virtual void GetTaskStatistics(
			KLDB::DbConnectionPtr pDbConn,
			int taskId,
			bool bCheckIfNeedSlaveServers,
			bool bNeedCompletedPercent,
			KLPAR::Params** ppStatistics) = 0;

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
			bool bRecreatingSuperTask = false ) = 0;

        virtual void UpdateTask ( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId, 
			KLPAR::Params* paramsTaskData ) = 0;

		enum ETaskUpdateActor{
			TUA_Unknown,
			TUA_GUI,
			TUA_Server,
			TUA_MainServer
		};
		virtual void UpdateTaskEx( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId, 
			KLPRCI::ComponentId compId,
			std::wstring taskName,
			KLSTD::CPointer<KLSCH::Task> pNewTask,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			ETaskUpdateActor eUupdateActor,
			bool bUpdateRunning,
			SuperGroupUpdateAction sg = SG_DENY,
			bool bSkipPrescribeRetranslationDirPath = false) = 0;

        virtual void DeleteTask( 
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			bool bCheckSupergroup) = 0;

        virtual void GetTaskStartEvent (
			KLDB::DbConnectionPtr pDbConn,
			const int taskId, 
			KLPRCI::ComponentId & filter,
			std::wstring & eventType,
			KLPAR::Params ** bodyFilter) = 0;

		virtual void SetTaskStartEvent (
			KLDB::DbConnectionPtr pDbConn,
			const int taskId, 
			const KLPRCI::ComponentId & filter,
			const std::wstring & eventType,
			const KLPAR::Params * bodyFilter) = 0;

        
        // GroupTaskControl stubs
        virtual void ForbidTaskForGroup( 
			KLDB::DbConnectionPtr pDbConn,
			int nGroupId, 
			int nTaskId ) = 0;
        
        virtual void RunTaskNow(
			KLDB::DbConnectionPtr pDbConn,
            int nTaskId,
            const std::wstring & wstrDomainName,
            const std::wstring & wstrHostName,
            bool bEvenIfAlreadySucceeded,
			const std::wstring& wstrActionId) = 0;

        virtual void SuspendTask( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId) = 0;

        virtual void ResumeTask( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId) = 0;

        virtual void CancelTask( 
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring& wstrActionId) = 0;

        // GroupTaskStatus stubs:
        virtual void ResetHostIteratorForTaskStatus(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrTaskId,
		    const std::vector<std::wstring>& vectFields,
			const std::vector<KLCSP::field_order_t>& vectFieldsToOrder,
            KLTSK::HostState       hsHostStateMask,
		    long            nLifetime,
            std::wstring&   wstrIteratorId ) = 0;

        virtual bool GetNextHostStatus(
			KLDB::DbConnectionPtr pDbConn,
            std::wstring wstrHostIteratorId,
            long& nCount,
            KLPAR::Params** ppParHostStatus ) = 0;

        virtual void ReleaseHostStatusIterator( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrIteratorId ) = 0;
        
		virtual long GetHostStatusRecordsCount( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrHostIteratorId ) = 0;

		virtual int GetHostStatusRecordRange( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrHostIteratorId, 
			long nStart, 
			long nEnd, 
			KLPAR::Params** ppParHostStatus ) = 0;

        virtual void GetAllTasksOfHost(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            std::vector<int> & vectTaskIds ) = 0;

        virtual int GetGTCIdFromTSId( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring & wstrTSId ) = 0;

        virtual std::wstring GetTSIdFromGTCId( 
			KLDB::DbConnectionPtr pDbConn,
			int nGTCId ) = 0;

		virtual std::wstring GetTaskFilePathFromIdentity(const std::wstring& wstrIdentity) = 0;

        virtual KLSCH::Task* FindTaskInScheduler(int nTaskId, bool bThrowOnError = true ) = 0;

        virtual std::wstring GetTskDiskStorage() = 0;

        virtual void OnSyncNotification(
			KLDB::DbConnectionPtr pDbConn,
			long nSyncId, 
			long nHostId, 
			bool bSuccess, 
			const std::wstring & wstrDescr, 
			const KLSTD::precise_time_t& tmSyncStart) = 0;

        virtual void RecreateSuperTask(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrSsType) = 0;

        virtual void DeleteSuperTask(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrSsType) = 0;
		
		virtual bool ChangeSharedFolderLocation(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrLocalPathToUpdates, 
			const std::wstring& wstrNetworkPathToUpdates) = 0;
		
		virtual KLSTD::CAutoPtr<KLSRV::RemoteInstallEventPreprocessor> GetRemoteInstallEventPreprocessor() = 0;

        virtual void RunTaskScopeNow(
			KLDB::DbConnectionPtr pDbConn,
			const std::vector<long> & vectTaskId/*,
			bool bSkipSlaveServers*/) = 0;

		virtual bool UpgradeTasks(KLDB::DbConnectionPtr pDbConn) = 0;
		
		virtual bool UpdateRestoredTasks(KLDB::DbConnectionPtr pDbConn) = 0;

		/*!
		  \brief ForceRetranslateUpdates.

			\param pParams	[in] Произвольные дополнительные параметры ретрансляции.

			\returns - false, если задача рентрансляции обновлений не существует на подчиненном сервере.
		*/
		virtual bool ForceRetranslateUpdates(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams) = 0;
		
		virtual void GetTaskGroupId( 
			KLDB::DbConnectionPtr &pDbConn,
			int taskId,
			long &nGroupId ) = 0;

// Stubs for GroupTaskHistoryProxy
        /* virtual void ResetHistoryIterator(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
		    const std::vector<std::wstring>& vectFields,
		    long        nLifetime,
            std::wstring&   wstrIteratorId ) = 0;

        virtual bool GetNextHistory(
			KLDB::DbConnectionPtr pDbConn,
            std::wstring wstrIteratorId,
            long& nCount,
            KLPAR::Params** ppParHistory ) = 0;

        virtual void ReleaseHistoryIterator( const std::wstring& wstrIteratorId ) = 0; */
		
		virtual void MarkTaskStatisticsChanged(long nTaskId) = 0;

		virtual void MarkHostTasksStatisticsChanged(long nHostId) = 0;

		virtual void MarkGroupTasksStatisticsChanged(long nGroupId) = 0;

		virtual KLSTD::CAutoPtr<KLPRTS::TasksStorage2> GetSrvTasksStorage() = 0;

		virtual void UpdateRiTaskFileStuff(
			KLSRV::GroupTaskParams* pGroupTaskParams,
			KLSCH::Task* pTask = NULL) = 0;

        virtual std::wstring GetSrvTskDiskStorage() = 0;

        virtual void SetNewStateForAllTaskHosts(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
            KLTSK::HostState hsNewStateForAllTaskHosts,
			KLTSK::RIFailReason eFailReason,
			const std::wstring& wstrStateDescr,
			KLTSK::HostState hsStateCodeToExclude1, 
			KLTSK::HostState hsStateCodeToExclude2,
			KLTSK::HostState hsStateCodeToExclude3) = 0;

		virtual long GetMainPackageId(KLDB::DbConnectionPtr pDbConn, const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS) = 0;

		virtual long GetPackageId(KLDB::DbConnectionPtr pDbConn, const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, int nPkgNumber, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS) = 0;
		
		virtual void UpdateHostsSyncObjH(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nTaskId,
                    long*                   pHosts,
                    size_t                  nHosts,
                    KLSRV::SrvGS_UpdateAction      nAction = KLSRV::SGSUA_REPLACE) = 0;
		
		virtual std::wstring GetCatchedRunningTasks(KLDB::DbConnectionPtr pDbConn, long nPackageId) = 0;
		
		virtual void UpdateTasksPackageId(KLDB::DbConnectionPtr pDbConn, const std::wstring& wstrPackageGUID, long nPackageId) = 0;
		
		virtual void StartTask(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrTaskTsId,
			const std::wstring& wstrTaskTypeName,
			KLSTD::CAutoPtr<KLPAR::Params> parParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parOutParams) = 0;
		
		virtual bool GetDependentTasksForPkg(
			KLDB::DbConnectionPtr pDbConn,
			long nPackageId, 
			std::vector<KLPKG::Packages2::task_info_t>& vecDependTasks) = 0;
		
		virtual void StartTasksForCondition(KLDB::DbConnectionPtr pDbConn, KLPRTS::ETskStartCondition eCond, AVP_long lCondData, AVP_long lCondDataEx) = 0;
		
		virtual bool StartRemoteInstallation(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped) = 0;

		virtual void ReportRemoteInstallationResult(
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrServerHostId,
			std::wstring wstrTaskFileId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent) = 0;

		virtual void UpdateVisibleHosts(const std::vector<long>& vecVisibleHosts) = 0;
		
        virtual void GenerateEventsForAllTaskHosts(
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
			const std::wstring& wstrTaskName) = 0; 
		
		virtual void UpdateRetranslationList(KLDB::DbConnectionPtr pDbConn, bool bRunTaskIfNeeded = false) = 0;
		
		virtual void GetUpdaterTasks(
			KLDB::DbConnectionPtr pDbConn,
			std::vector<long>& vecTaskIds, 
			bool bServersOnly) = 0;

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
			long eventTimeMsec = -1) = 0;
    };
}

#endif // __TSK_GROUPTASKCONTROLSRV_H__
