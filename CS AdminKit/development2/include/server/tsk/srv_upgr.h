/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file server/tsk/srv_upgr.h
 * \author Андрей Лащенков
 * \date 15:36 17.04.2006
 * \brief Заголовок класса, занимающегося обновлением версии подчиненных серверов.
 */

#ifndef __TSK_SRV_UPGR_H__
#define __TSK_SRV_UPGR_H__

#include <server/tsk/syncpkg.h>

namespace KLSRV
{
	class SrvUpgrTsk;

	class SrvUpgrTskWorker: public KLSTD::KLBaseImpl<KLTMSG::TPWorker>{
	public:
		SrvUpgrTskWorker():m_pSrvUpgrTsk(NULL){}
		virtual ~SrvUpgrTskWorker(){}

		void SetSrvUpgrTsk(SrvUpgrTsk* pSrvUpgrTsk){m_pSrvUpgrTsk = pSrvUpgrTsk;}

		virtual int RunWorker(KLTMSG::TPWorkerId wId);
	protected:
		SrvUpgrTsk* m_pSrvUpgrTsk;
	};

    class SrvUpgrTsk: public SyncPkg
    {
		friend class GroupTaskControlImpl;
		friend class SrvUpgrTskWorker;
    public:
        SrvUpgrTsk();
        ~SrvUpgrTsk();

        virtual void Initialize(
            KLSRV::SrvData* pSrvData,
            KLSTD::CAutoPtr<KLSTD::CriticalSection> pCSTaskParams,
            KLSTD::CAutoPtr<KLSRV::PackagesImpl> pPackagesImpl,
            CommonSrvGroupTaskTools * pCommonSrvGroupTaskTools,
			KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> pEventsProcessor,
			KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> pGroupTaskControlSrv,
			KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> pSrvHierarchySrv);

        virtual void Deinitialize();

        virtual void DoTask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams );

		void DoSrvUpgrTsk(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams );

		void StartInstallation(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams);

		void StartInstallation(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams,
			const std::wstring& wstrHostName,
			const std::wstring& wstrInstanceId);

		bool StartRemoteInstallation(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped);

		void ReportRemoteInstallationResult(
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrServerHostId,
			std::wstring wstrTaskFileId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent);

		static bool GetAppEventStopCallback(void* pParam);

		void AddTaskCompletionWatch(KLDB::DbConnectionPtr pDbConn, const std::wstring& wstrTaskTsId);

		void RemoveTaskCompletionWatch(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrTaskTsId,
			KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pPrivateStorage);

		void GetResults(KLPAR::ParamsPtr& parResults);

		int RunWorker( KLTMSG::TPWorkerId wId );

	protected:
		SrvUpgrTskWorker m_Worker;

		void DoStartRemoteInstallation(
			KLPAR::ParamsPtr parTaskInfo,
			KLPAR::ParamsPtr parTaskParams);

		std::wstring m_wstrSubscriptionDataFilePath;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSTskLst;
		std::map<std::wstring, bool> m_mpTskLst;
		KLTP::ThreadsPool::WorkerId m_idWorker;
    };
}

#endif // __TSK_SRV_UPGR_H__
