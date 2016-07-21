/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file server/tsk/syncpkg.h
 * \author Андрей Лащенков
 * \date 15:36 17.04.2006
 * \brief Заголовок класса, занимающегося рспространением инсталляционных пакетов на подчиненные сервера.
 */

#ifndef __TSK_SYNC_PKG_H__
#define __TSK_SYNC_PKG_H__

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
#include <server/srvhrch/srvhrch_srv.h>

#define KLSRV_TSK_SYNC_PACKAGES_FT_IDS		L"KLSRV_TSK_SYNC_PACKAGES_FT_IDS"		// ARRAY_T

namespace KLSRV
{
    class GroupTaskControlImpl;

	class SyncPkgBase: 
		public KLSRV::Deinitializer{
	};

    class SyncPkg: public KLSTD::KLBaseImpl<SyncPkgBase>
    {
		friend class GroupTaskControlImpl;
    public:
        SyncPkg();
        ~SyncPkg();

        virtual void Initialize(
            KLSRV::SrvData* pSrvData,
            KLSTD::CAutoPtr<KLSTD::CriticalSection> pCSTaskParams,
            KLSTD::CAutoPtr<KLSRV::PackagesImpl> pPackagesImpl,
            CommonSrvGroupTaskTools * pCommonSrvGroupTaskTools,
			KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> pEventsProcessor,
			KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> pGroupTaskControlSrv,
			KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> pSrvHierarchySrv);

        bool IsInitialized() { return m_bInitialized; };

        virtual void Deinitialize();

		void SetSrvHierarchySrv(KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> pSrvHierarchySrv)
		{
			m_pSrvHierarchySrv = pSrvHierarchySrv;
		}

        virtual void DoTask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams );
	
		void CancelTask(KLDB::DbConnectionPtr pDbConn, GroupTaskParams* pGroupTaskParams);

		bool IsAttachedToPackage(KLDB::DbConnectionPtr pDbConn, GroupTaskParams* pGroupTaskParams, long nPackageId);

    protected:
        KLDB::DbConnectionPtr m_pConnection;
        KLPRCI::ComponentId m_compIdServerFT;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSTaskParams;
		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSFT;

        KLSTD::CAutoPtr<KLSRV::PackagesImpl> m_pPackagesImpl;
        CommonSrvGroupTaskTools * m_pCommonSrvGroupTaskTools;
        KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> m_pEventsProcessor;
		KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> m_pGroupTaskControlSrv;
		KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> m_pSrvHierarchySrv;
		KLSRV::SrvData* m_pSrvData;
        bool m_bInitialized;
		bool m_bDefaultUsePackagesIds;

        std::vector<KLEV::SubscriptionId> m_vectSubscrId;

		struct FTEvent{
			enum EType{
				UNKNOWN,
				START_UPLOAD, 
				PROGRESS, 
				UPLOADED} m_eType;
			std::wstring m_wstrFileId;
			std::wstring m_wstrTSFileId;
			long m_nTaskId;
			long m_nPkgId;
			int m_nPkgNum;
			int m_nPkgCount;
			std::wstring m_wstrHostName;
			int m_nUploadPercent;
			FTEvent():m_eType(UNKNOWN), m_nTaskId(0),m_nUploadPercent(0),m_nPkgId(0),m_nPkgNum(0),m_nPkgCount(0){}
		};
		typedef KLCMDQUEUE::CmdQueue<FTEvent, FTEvent&, SyncPkg> FTEventsQueue;
		KLSTD::CPointer<FTEventsQueue>  m_pFTEventsQueue;

        void DoSyncPkg(
			KLDB::DbConnectionPtr pDbConn,
			GroupTaskParams* pGroupTaskParams);

        long CallEventsProcessor(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
            const std::wstring & wstrTaskFileIdentity,
            const std::wstring & wstrDomainName,
            const std::wstring & wstrHostName,
            KLPRCI::TaskState tsOldState,
            const KLTSK::HostState hsNewHostState,
            int nSeverity,
            const std::wstring & wstrTaskDisplayName,
            const std::wstring & wstrDescription,
			KLSTD::CAutoPtr<KLPAR::Params> parAdditionalParams,
			time_t eventTime = -1,
			long eventTimeMsec = -1);

		void UpdateStateForHost(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId, 
			const std::wstring& wstrFileIdentity,
			const std::wstring& wstrTaskDisplName,
			const std::wstring& wstrDomainName,
			const std::wstring& wstrHostName,
			KLTSK::HostState hsNewState, 
			KLPRCI::TaskState tsOldState,
			const std::wstring& wstrMsg, 
			int nProgress, 
			bool bSetFullStateInDB,
			time_t tmRiseTime = 0,
			long lRiseTimeMs = 0,
			KLSTD::CAutoPtr<KLPAR::Params> parAdditionalTaskStateParams = NULL
		);
		
		void StopFileTransfer(KLDB::DbConnectionPtr pDbConn, GroupTaskParams* pGroupTaskParams);
		
		void StartFileTransfer(
			KLDB::DbConnectionPtr pDbConn, 
			size_t nPkgNumber,
			size_t nPkgCount,
			long nPkgId,
			const std::wstring& wstrPackageLocalPath, 
			GroupTaskParams* pGroupTaskParams, 
			const std::wstring& wstrStartId,
			bool bStartNow);

		void GetHostsForTask(
			KLDB::DbConnectionPtr pDbConn, 
			GroupTaskParams* pGroupTaskParams, 
			HostInfoStateMap& mapHostInfoState);

		void OnFtHostFileUpload(const KLEV::Event* pEvent);

		void OnFTEvent(FTEvent& event);

		long GetPackageId(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			size_t nIdx);

		void RemoveGhostPackagesFromFT(KLDB::DbConnectionPtr pDbConn, GroupTaskParams* pGroupTaskParams, std::vector<long>& vecPackages);

		static void OnFtHostFileUploadStatic(
			const KLPRCI::ComponentId& subscriber,
			const KLPRCI::ComponentId& publisher,
			const KLEV::Event *event,
			void *context );
    };
}

#endif // __TSK_SYNC_PKG_H__
