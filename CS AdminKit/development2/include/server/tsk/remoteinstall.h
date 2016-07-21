/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TSK/RemoteInstall.h
 * \author Михаил Кармазин
 * \date 11:55 04.01.2003
 * \brief Заголовок класса, занимающегося удаленной инсталляцией.
 */

#ifndef __TSK_REMOTE_INSTALL_H__
#define __TSK_REMOTE_INSTALL_H__

#include <server/tsk/grouptaskcommonsrv.h>
#include <server/tsk/grouptaskcontrolsrv.h>
#include <server/tsk/commonsrvtasktools.h>
#include <server/evp/transeventsource.h>
#include <transport/ev/eventsource.h>
#include <common/rmtinstutils.h>
#include <common/reporter.h>
#include <server/pkg/fileutils.h>
#include <server/pkg/packagesimpl.h>
#include <server/wnst/windowsinstall.h>
#include <server/srv_common.h>
#include <server/srvinst/srvinstdata.h>
#include <server/evp/eventsprocessorsrv.h>
#include <std/tp/tpcmdqueue.h>
#include <server/fts/serverfiledescription.h>

namespace KLTMSG{
	
	class SemaphoresList: public KLSTD::KLBaseImpl<KLSTD::KLBase>{
	public:
		SemaphoresList(size_t nCount, int nTimeoutInMs);
		KLSTD::CAutoPtr<KLSTD::Semaphore> GetSemaphore();
		void ReleaseSemaphore(KLSTD::CAutoPtr<KLSTD::Semaphore> pSemaphore);
	protected:
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
		KLSTD::CAutoPtr<KLSTD::Semaphore> m_pSemaphore;
		std::list<KLSTD::KLAdapt<KLSTD::CAutoPtr<KLSTD::Semaphore> > > m_lstSemaphores;
		int m_nTimeoutInMs;
	};
	
	class TimeoutCallParameterBase: public KLSTD::KLBaseImpl<KLSTD::KLBase> {
	public:
		virtual ~TimeoutCallParameterBase(){}
		KLSTD::CAutoPtr<KLERR::Error> m_pError;
	};
		
	typedef KLSTD::CAutoPtr<TimeoutCallParameterBase> TimeoutCallParameter;
};

namespace KLSRV
{
    class GroupTaskControlImpl;

    struct LoginData
    {  // domain name stored as a key in a map, which contains logins
        std::wstring wstrUserName;
        long nTaskId;
    };
	
	class HostLock: public KLSTD::KLBaseImpl<KLSTD::KLBase>{
	public:
		HostLock();
		virtual ~HostLock();

		virtual bool Lock(const std::wstring& wstrHostId, int nWaitTimeOutInMs = -1);

		virtual void Unlock(const std::wstring& wstrHostId);

	protected:
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
		struct LockInfo{
			int m_nThreadId;
			int m_nLockCount;
			LockInfo():m_nThreadId(0), m_nLockCount(0){}
			LockInfo(int nThreadId):m_nThreadId(nThreadId), m_nLockCount(1){}
		};
		std::map<std::wstring, LockInfo> m_mapLockedHosts;
		KLSTD::CAutoPtr<KLSTD::Semaphore> m_pSem;
		volatile long m_lWaitersCounter;
	};

	class AutoHostLock{
	public:
		AutoHostLock(KLSTD::CAutoPtr<HostLock> pHostLock, const std::wstring wstrHostId);

		AutoHostLock();

		virtual ~AutoHostLock();

		virtual bool Lock(KLSTD::CAutoPtr<HostLock> pHostLock, const std::wstring& wstrHostId, int nWaitTimeOutInMs = -1);

	protected:
		KLSTD::CAutoPtr<HostLock> m_pHostLock;
		std::wstring m_wstrHostId;
	};

	class CheckOnlineParameter: public KLTMSG::TimeoutCallParameterBase{
	public:
		CheckOnlineParameter(const std::wstring& wstrHostName)
		:
			m_wstrHostName(wstrHostName),
			m_bResult(false)
		{
		}
		std::wstring m_wstrHostName;
		bool m_bResult;
	};
	
	class InstallServiceParameter: public KLTMSG::TimeoutCallParameterBase{
	public:
		InstallServiceParameter(
			const std::wstring& wstrMachineName, 
			const std::wstring& wstrServiceName,
			const std::wstring& wstrLocalPathToServiceExecutable,
			const std::wstring& wstrRIId,
			const KLWNST::SecurityCtx& sc)
		:
			m_wstrMachineName(wstrMachineName),
			m_wstrServiceName(wstrServiceName),
			m_wstrLocalPathToServiceExecutable(wstrLocalPathToServiceExecutable),
			m_wstrRIId(wstrRIId),
			m_sc(sc)
		{
		}
		std::wstring m_wstrMachineName;
		std::wstring m_wstrServiceName;
		std::wstring m_wstrLocalPathToServiceExecutable;
		std::wstring m_wstrRIId;
		
		const KLWNST::SecurityCtx m_sc;
	};

	class RemoveServiceParameter: public KLTMSG::TimeoutCallParameterBase{
	public:
		RemoveServiceParameter(const std::wstring& wstrMachineName, const std::wstring& wstrServiceName)
		:
			m_wstrMachineName(wstrMachineName),
			m_wstrServiceName(wstrServiceName)
		{
		}
		std::wstring m_wstrMachineName;
		std::wstring m_wstrServiceName;
	};

	class InstallationSession: public KLSTD::KLBaseImpl<KLSTD::KLBase>{
	public:
		// construction / destruction:
		InstallationSession(
			int nThreadsCount,
            GroupTaskParams* pGroupTaskParams,
			GroupTaskParams* pMainGroupTaskParams,
			const std::wstring & wstrPackageLocalPath,
			const std::wstring & wstrPackageNetworkPath,
			int nPackageNumber,
			const std::wstring & wstrProductName, 
			const std::wstring & wstrProductVersion, 
			const std::wstring & wstrProductDisplVersion,
			bool bIsNextStartSheduled,
			bool bManualStart,
			bool bFirstPkgSkipped,
			bool bSkipCompleted,
			const std::wstring& wstrRIId);

		virtual ~InstallationSession();

		// operations:
		bool WaitForFreeThread(int nMS);
		void ReleaseThread();
		KLSTD::CAutoPtr<KLSTD::CriticalSection> GetCriticalSectionForData();
		void GetResult(int& nTryedHostCount, int& nFinallyFailedCount);

		// public data:
        KLSTD::CPointer<GroupTaskParams> m_pGroupTaskParams;
		KLSTD::CPointer<GroupTaskParams> m_pMainGroupTaskParams;
		const std::wstring m_wstrPackageLocalPath;
		const std::wstring m_wstrPackageNetworkPath;
		const int m_nPackageNumber;
		const std::wstring m_wstrProductName;
		const std::wstring m_wstrProductVersion;
		const std::wstring m_wstrProductDisplVersion;
		const bool m_bIsNextStartSheduled;
        int m_nTryedHostCount;
        int m_nFinallyFailedCount;
		bool m_bManualStart;
		bool m_bFirstPkgSkipped;
		bool m_bSkipCompleted;
		std::wstring m_wstrRIId;

	protected:
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
		KLSTD::CAutoPtr<KLSTD::Semaphore> m_pSemaphore;
	};
	typedef KLSTD::CAutoPtr<InstallationSession> InstallationSessionPtr;

	struct RiTskStartParams 
	{
		HostIdentity m_HostIdentity;
		bool m_bManualStart;
		bool m_bFirstPkgSkipped;

		RiTskStartParams(HostIdentity HostIdentity, bool bManualStart, bool bFirstPkgSkipped)
		:
			m_HostIdentity(HostIdentity), m_bManualStart(bManualStart), m_bFirstPkgSkipped(bFirstPkgSkipped)
		{
		}
	};
	
	class RemoteInstallBase: 
		public KLSRV::RemoteInstallEventPreprocessor,
		public KLSRV::Deinitializer{
	};

    class RemoteInstall: public KLSTD::KLBaseImpl<RemoteInstallBase>
    {
		friend class GroupTaskControlImpl;
    public:
        RemoteInstall();
        ~RemoteInstall();

        void Initialize(
            KLSRV::SrvData* pSrvData,
            KLSTD::CAutoPtr<KLSTD::CriticalSection> pCSTaskParams,
            KLSTD::CAutoPtr<KLSRV::PackagesImpl> pPackagesImpl,
            CommonSrvGroupTaskTools * pCommonSrvGroupTaskTools,
			KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> pEventsProcessor,
			KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> pGroupTaskControlSrv);

        bool IsInitialized() { return m_bInitialized; };

        void Deinitialize();

        void DoTask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams );

        void CompleteTaskParams(
            KLPAR::Params * parTaskParams,
            KLPAR::Params * parTaskInfo );

        void DeleteTaskStuff( GroupTaskParams* pGroupTaskParams );

		void SetTaskStateFromNAgentTaskState(
			KLDB::DbConnectionPtr pDbConn, long nNAgTaskId, const KLEVP::caller_id_t& caller,
			const KLPAR::Params * pParamsEventBody, time_t tmRiseTime, long lRiseTimeMs);

		/*
		void SetTaskStateFromSubTaskState(GroupTaskParams* pGroupTaskParams, const KLEVP::caller_id_t& caller,
			const KLPAR::Params * pParamsEventBody, time_t tmRiseTime, long lRiseTimeMs);
		*/
		
		bool CheckRemovedLoginScripts(int nTaskId, const std::wstring& wstrFileIdentity, 
			KLSTD::CAutoPtr<KLPAR::Params> parOldTaskParams, 
			KLSTD::CAutoPtr<KLPAR::Params> parNewTaskParams,
			bool bThrowException = true);
		
		virtual void CheckRemoteInstallResultEvent(
			KLDB::DbConnectionPtr pDbConn,
			KLSRV::EventAndCallerInfo& eventAndCallerInfo);

		void OnRITskState(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskCommonData& taskCommonData,
			GroupTaskParams* pGroupTaskParams,
			KLPRCI::TaskState nTskState,
			const KLEVP::caller_id_t& caller);

		long AdjustNAgentRITask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams,
			KLTSK::EStorageType eStorageType,
			const std::wstring& wstrHostId = L"",
			bool bCreateEmpty = false,
			bool bAdditional = false);
		
		//void CancelTask(GroupTaskParams* pGroupTaskParams);
		bool DeleteSubtasks(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, 
			bool bInclPersist);

		bool CheckDeleteSubtasks(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			KLSTD::CAutoPtr<KLPAR::Params> parOldTaskParams, 
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams);
		
		static bool IsProductInstalled(
			KLDB::DbConnectionPtr pDbConn, 
			KLSRV::SrvData* pSrvData,
			const std::wstring& wstrHostId, 
			const std::wstring& wstrProductName, 
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplVersion);
	
    private:
        //std::vector<LoginData> m_vectUsedLogins;
        typedef std::multimap<std::wstring, LoginData> MultimapLogins;
        typedef std::multimap<std::wstring, std::wstring> MultimapHosts;
        typedef MultimapLogins::iterator ItLogin;
        typedef MultimapHosts::iterator ItHost;
        
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSNeedReboot;

		HostLock m_HostLock;

        struct TaskPathData
        {
            /* если bGhostInstall==true, то есть просто папка, из 
               которой еще работает инст. пакет, а задачи (например, уже) нет.
               В таком случае только nPathId и wstrFolderPath имеют
               значение. */
            bool bGhostInstall;
            long nPathId;
            std::wstring wstrFolderPath;

            time_t tDeadline;
            std::wstring wstrDomainName;
            std::wstring wstrHostName;
            long nMasterTaskId;
            std::wstring wstrFileIdentity;
            std::wstring wstrMasterTaskDisplayName;
            long nTryCount;
			int nUnused;
            time_t tClearTime;

            //std::wstring wstrDnsName;
			KLSTD::CAutoPtr<KLPAR::Params> m_pCredentials;
        };

        KLDB::DbConnectionPtr m_pConnection;
        KLPRCI::ComponentId m_compIdServerFT;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSTaskParams;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSFT;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSNagt;

        KLSTD::CAutoPtr<KLSRV::PackagesImpl> m_pPackagesImpl;
        CommonSrvGroupTaskTools * m_pCommonSrvGroupTaskTools;
        KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> m_pEventsProcessor;
		KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> m_pGroupTaskControlSrv;
		KLSRV::SrvData* m_pSrvData;
        bool m_bInitialized;

		std::map<long, bool> m_mpTskNeedRebootProcessed;

		KLSTD::CAutoPtr<KLTMSG::SemaphoresList> m_pSemaphoresList;

		unsigned long m_ulLastClean_tsk_glued_hosts;
		unsigned long m_ulLastClean_tsk_ri_hosts_starts;
		unsigned long m_ulLastCheckRiTimeout;

		struct HostRIInfo: public HostIdentity{
			HostInfoState m_HostInfoState;
			const std::vector<KLWNST::SecurityCtx>& m_vecSecCtxs;
			bool m_bFtActivated;
			HostRIInfo(
				const HostIdentity& hostIdentity,
				const HostInfoState& hostInfoState, 
				const std::vector<KLWNST::SecurityCtx>& vecSecCtxs,
				bool bFtActivated)
			: 
				HostIdentity(hostIdentity), m_HostInfoState(hostInfoState), m_vecSecCtxs(vecSecCtxs), m_bFtActivated(bFtActivated)
			{
			}
		};

		bool m_bUseAsyncInstall;
		
        std::vector<KLEV::SubscriptionId> m_vectSubscrId;

		struct FTEvent{
			enum EType{
				UNKNOWN,
				START_UPLOAD, 
				PROGRESS, 
				UPLOADED} m_eType;
			std::wstring m_wstrFileId;
			int m_nTaskId;
			std::wstring m_wstrHostName;
			int m_nUploadPercent;
			int m_nTotalPackagesCnt;
			int m_nPackageNum;
			int m_nMainTaskId;
			std::wstring m_wstrProdDisplName, m_wstrProdDisplVer;
			FTEvent():m_eType(UNKNOWN), m_nTaskId(0),m_nUploadPercent(0),m_nTotalPackagesCnt(1),m_nPackageNum(0),m_nMainTaskId(0){}
		};
		typedef KLCMDQUEUE::CmdQueue<FTEvent, FTEvent&, RemoteInstall> FTEventsQueue;
		KLSTD::CPointer<FTEventsQueue>  m_pFTEventsQueue;

		std::wstring m_wstrServerFullVersion;

		KLWNST::NetRes2ConnMap m_mpNetRes2ConnMap;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSNetRes2ConnMap;

        void DoRemoteInstall(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams/*,
			bool bForPredefinedHostsOnly*/);
		void DoRemoteInstallSubpackageAsync(
			long nTaskId,
			RiTskStartParams StartParams);
		void DoRemoteInstallSubpackage(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId,
			RiTskStartParams StartParams);
        void PrepareRemoteInstall(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams);
		
        void DoRemoteInstallClean(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams );

		void DoRemoteInstallSubPackages(KLDB::DbConnectionPtr pDbConn);

        void DoEventsClean();
        void CheckMachine(
			KLDB::DbConnectionPtr pDbConn,
            const HostIdentity & hostIdentity,
            const HostInfoState & hostInfoState,
			const std::vector<KLWNST::SecurityCtx>& vecSecCtxs,
			bool bFtActivated,
			InstallationSessionPtr pSession);
        void CheckMachineSessionWrap(
			HostRIInfo hrii,
			InstallationSessionPtr pSession);

        void GetLogins(
			int nTaskId, 
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, 
			MultimapLogins& multimapLogins,
			std::vector<std::wstring>* pvecDomains = NULL);

		bool DeleteLoginScripts(MultimapLogins& multimapLogins, KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, 
			const std::wstring& wstrFileIdentity, bool bUseThrow);

        bool IsLoginScriptSet( const std::wstring& wstrDomainName,
                               const std::wstring& wstrUserName );

        bool IsNoOtherInstallationsOnMachine(
			KLDB::DbConnectionPtr pDbConn,
            GroupTaskParams* pGroupTaskParams,
            const HostIdentity & hostIdentity,
			const std::vector<long>& vecTasksToSkip,
			std::vector<long>& vecOtherActiveTasks,
			std::vector<long>& vecActiveSubTasks);

        bool SetupPackageForInstall(
			KLDB::DbConnectionPtr pDbConn,
            GroupTaskParams* pGroupTaskParams,
			long nMasterTaskId,
            const std::wstring & wstrPackageLocalPath,
			const std::wstring & wstrPackageNetworkPath,
			int nPackageNumber,
            const HostIdentity & hostIdentity,
            const HostInfoState & hostInfoState,
			const std::vector<KLWNST::SecurityCtx>& vecSecCtxs,
			const std::wstring & wstrRIId,
			bool& bFtActivated,
			std::wstring& strErrorMsg,
			bool& bRunningEventSent,
			bool& bShouldChangeTryCount,
			KLTSK::RIFailReason& eFailReason);

        std::wstring CreateFolderForServiceInstall(
            const HostIdentity & hostIdentity,
            const HostInfoState & hostInfoState,
            const std::wstring & wstrFolderName,
			std::wstring & wstrResourceName);

        bool ArrangeLoginScript(
			KLDB::DbConnectionPtr pDbConn,
            const HostIdentity & hiServer,
            const HostInfoState & hisServer,
            GroupTaskParams* pGroupTaskParams,
			const std::wstring& wstrPackageLocalPath,
			const std::wstring& wstrPackageNetworkPath,
			std::wstring& strErrorMsg);

        void ArrangeServiceInstall(
			KLDB::DbConnectionPtr pDbConn,
            const HostIdentity & hostIdentity,
            const HostInfoState & hostInfoState,
            GroupTaskParams* pGroupTaskParams,
			long nMasterTaskId,
            const std::wstring & wstrPackageLocalPath,
			int nPackageNumber,
            const std::wstring & wstrDestFolderPath,
            const std::wstring & wstrServiceName,
			const std::wstring & wstrRIId);
        
        long AddFolderToClean(
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const std::wstring & wstrHostId, 
            const std::wstring & wstrFolderToClean,
			time_t tFolderDeadline,
            const std::vector<KLWNST::SecurityCtx>& vecSecCtxs);

        void GetPathsRecordedInDBAsInProgress( 
			KLDB::DbConnectionPtr pDbConn, 
			std::vector<TaskPathData>& vectTaskPathDataInDB );

        void CheckHostCurrentState(
			KLDB::DbConnectionPtr pDbConn,
			const TaskPathData & hostTaskDataInDB );
		
        bool GetCurrentInstallState( 
			std::wstring wstrFolder, 
			std::wstring& wstrReport, 
			KLTSK::RebootState& rebootState,
			GroupTaskParams* pGroupTaskParams,
			KLTSK::HostState& hsHostState,
			KLTSK::RIFailReason& eFailReason);

		void RemoveService(KLTMSG::TimeoutCallParameter pRemoveServiceParameter);

		void RemoveServiceForInstallationFolder(
			const std::wstring& wstrHostName,
			const std::wstring& wstrFolderPath );

        void CleanupInstallationFolder(
			const std::wstring& wstrHostName,
			std::wstring wstrFolderPath,
			bool bOnlyKPD = false);

        void RemovePathFromDB(
			KLDB::DbConnectionPtr pDbConn, 
			long nPathId,
			bool bCleanTskHostState);

		KLTSK::HostState ProcessInstallState(
			const std::wstring& wstrProcessResultCode,
			const std::wstring& wstrFirstResultCode, 
			const std::wstring& wstrSecondResultCode, 
			const std::wstring& wstrReportFileContent,
			KLTSK::RebootState rebootState,
			std::wstring& wstrReport);

		bool GetHostFinishStateDetails(const KLTSK::HostState hsHostState, 
			KLTSK::RebootState rebootState,
			const std::wstring& wstrReport,
			int& nPercentCompleted,
			std::wstring& wstrNotification,
			bool bIsUninstall);

		bool GetPackageInfo(
			KLPAR::ParamsPtr parTaskParams, 
			long nPkgId,
			std::wstring& wstrProductDisplName, 
			std::wstring& wstrProductDisplVersion);

		std::wstring GetMultipackageDescriptionPrefix(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId, 
			int nPackageNumber);
		
		void UpdateStateForHost(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams, 
			long nMasterTaskId,
			const HostIdentity& hostIdentity,
			int nPackageNumber,
			KLTSK::HostState hsNewState, 
			KLTSK::RIFailReason eFailReason,
			BoolEx bNeedReboot,
			KLPRCI::TaskState tsOldState,
			long nPathId,
			const std::wstring& wstrMsg, 
			int nProgress, 
			int nTryCountDelta, 
			bool bSetFullStateInDB,
			time_t tmRiseTime = 0,
			long lRiseTimeMs = 0,
			time_t tTaskDeadline = NULL_DEADLINE,
			KLSTD::CAutoPtr<KLPAR::Params> parAdditionalTaskStateParams = NULL,
			bool bProgressIsPreprocessedAlready = false,
			bool bProcessStarted = false,
			bool bKeepState = false
		);
		void UpdateStateForHost(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId, 
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			long nMasterTaskId,
			const std::wstring& wstrFileIdentity,
			const std::wstring& wstrTaskDisplName,
			const std::wstring& wstrDomainName,
			const std::wstring& wstrHostName,
			int nPackageNumber,
			int nTotalPackagesCount,
			KLTSK::HostState hsNewState, 
			KLTSK::RIFailReason eFailReason,
			BoolEx bNeedReboot,
			KLPRCI::TaskState tsOldState,
			long nPathId,
			const std::wstring& wstrMsg, 
			int nProgress, 
			int nTryCountDelta, 
			bool bSetFullStateInDB,
			time_t tmRiseTime = 0,
			long lRiseTimeMs = 0,
			time_t tTaskDeadline = NULL_DEADLINE,
			KLSTD::CAutoPtr<KLPAR::Params> parAdditionalTaskStateParams = NULL,
			bool bProgressIsPreprocessedAlready = false,
			bool bProcessStarted = false,
			bool bKeepState = false
		);
		
		void AddRebootStateDescription(long lRebootState, std::wstring& wstrDescription);
		
		std::wstring GetAdministrativeShare(
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId,
			const HostIdentity& hostIdentity,
			const HostInfoState& hostInfoState);
		
		void AddConnectionToAdministrativeShare(
			const std::wstring& wstrResourceName,
			const std::vector<KLWNST::SecurityCtx>& vecSecCtxs,
			KLSTD::CAutoPtr<KLWNST::NetResourceConnection> pNRC,
			AVP_dword& dwSystemErrorCode);

		enum FTOperation{
			FT_REPLACE_HOSTS,
			FT_ADD_HOSTS,
			FT_REMOVE_HOSTS
		};

		void StopFileTransfer(KLDB::DbConnectionPtr pDbConn, GroupTaskParams* pGroupTaskParams);
		
		size_t PrepareStartFileTransfer(
			KLDB::DbConnectionPtr pDbConn, 
			long nPkgId,
			InstallationSessionPtr pSession, 
			bool bSkipRunning,
			bool bSkipCompleted,
			bool bForNewNagentsOnly,
			KLFTS::HostsIdsList& hosts,
			KLPAR::Params** pparFTParams,
			const std::wstring& wstrHostIdToPerform = L"",
			std::vector<std::pair<std::wstring, bool> >* pvecAlreadyInstalledHosts = NULL,
			std::vector<std::wstring>* pvecAlreadyCompletedHosts = NULL);

		void RemoteInstall::DoStartFileTransfer(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrPackageLocalPath, 
			InstallationSessionPtr pSession,
			KLFTS::HostsIdsList& hosts,
			KLSTD::CAutoPtr<KLPAR::Params> parFTParams,
			const std::wstring& wstrHostIdToPerform = L"");

		size_t StartFileTransfer(
			KLDB::DbConnectionPtr pDbConn, 
			long nPkgId,
			const std::wstring& wstrPackageLocalPath, 
			InstallationSessionPtr pSession, 
			bool bSkipCompleted,
			bool bForNewNagentsOnly,
			const std::wstring& wstrHostIdToPerform = L"",
			std::vector<std::pair<std::wstring, bool> >* pvecAlreadyInstalledHosts = NULL,
			std::vector<std::wstring>* pvecAlreadyCompletedHosts = NULL);
		
		bool IsNextStartSheduled(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams);

		void GetHostsForTask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams, 
			HostInfoStateMap& mapHostInfoState);

		std::wstring FormatISSetupProcessError(long lProcessResultCode);
		
		std::wstring FormatISError(const std::wstring& wstrFirstResultCode);
		
		bool IsNAgentTaskActiveForHost(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams, 
			const std::wstring& wstrHostName);
		
		void SetNeedRebootProcessed(const TaskPathData& taskPathDataInDB);
		bool GetNeedRebootProcessed(const TaskPathData& taskPathDataInDB);
		void ResetNeedRebootProcessed(const TaskPathData& taskPathDataInDB);

		void ReportCrashedTaskSuspended(GroupTaskParams* pGroupTaskParams);

		void CheckReleaseFileTransfer(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams, 
			const std::wstring& wstrHostName);

		void CheckMachineOnline(KLTMSG::TimeoutCallParameter pCheckOnlineParameter);

		void InstallService(KLTMSG::TimeoutCallParameter pInstallServiceParameter);
		
		std::wstring GetHostNetworkName(KLDB::DbConnectionPtr pDbConn, int nTaskId, const std::wstring& wstrHostId);
		
		bool GetNextHostForTask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams, 
			bool bGetOptimal,
			HostIdentity& hostIdentity,
			HostInfoState& hostInfoState,
			int nStartConditionTaskId,
			int& nPhase);
		
		void CleanupInstallationFolderAndRemovePathFromDB(
			KLDB::DbConnectionPtr pDbConn,
			const TaskPathData & taskPathDataInDB,
			bool bOnlyKPD = false);
		
		void ClearPathId(
			KLDB::DbConnectionPtr pDbConn,
			int nTaskId, 
			const std::wstring& wstrHostId,
			long nPathId = 0);

		void OnFtHostFileUpload(const KLEV::Event* pEvent);

		void OnFTEvent(FTEvent& event);

		void CheckRiTimeout(KLDB::DbConnectionPtr pDbConn);

		bool WasFTStartedForFile(
			const HostIdentity& hostIdentity,
			const std::wstring& wstrFileId);

		void PublishAlreadyInstalledEvent(
			KLDB::DbConnectionPtr pDbConn, 
			InstallationSessionPtr pSession, 
			const std::wstring& wstrHostId,
			bool bAlreadySucceed = false,
			bool bDisableSubtaskStart = false);

		bool RemoteInstall::CheckConflicts(
			const std::vector<std::wstring>& vecDomains,
			const MultimapLogins& multimapLogins,
			const KLWNST::SecurityCtx& ctx,
			std::wstring& strErrorMsg);
		
		static void OnFtHostFileUploadStatic(
			const KLPRCI::ComponentId& subscriber,
			const KLPRCI::ComponentId& publisher,
			const KLEV::Event *event,
			void *context );
    };
}

#endif // __TSK_REMOTE_INSTALL_H__
