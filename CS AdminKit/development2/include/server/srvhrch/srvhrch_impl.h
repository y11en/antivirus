/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvhrch\srvhrch_impl.h
 * \author	Andrew Lashenkov
 * \date	12.01.2005 10:31:00
 * \brief	Серверная реализация интерфейса ChildServers
 * 
 */

#ifndef __KLSRVH_SRVHRCH_IMPL_H__
#define __KLSRVH_SRVHRCH_IMPL_H__

#include <server/srvhrch/srvhrch_srv.h>
#include <server/srvinst/srvinstdata.h>
#include <server/tsk/grouptaskcontrolsrv.h>
#include <std/tp/threadspool.h>
#include <server/pkg/packagessrv.h>
#include <srvp/rpt/reports.h>

namespace KLSRVH
{
	struct RptCacheEntry{
		int nReportType;
		time_t tmDesiredStartTime;
		time_t tmDesiredEndTime;
		KLSTD::MemoryChunkPtr ptrFilter;
		std::wstring wstrFilterHash;
		int nRecursionLevelLimit;
		int nPeriodToUpdateCacheInMinutes;
		RptCacheEntry()
		:
			nReportType(KLRPT::RT_INVALID_REPORT_TYPE), 
			tmDesiredStartTime(-1), 
			tmDesiredEndTime(-1),
			nRecursionLevelLimit(0),
			nPeriodToUpdateCacheInMinutes(0)
		{
		}
		bool operator==(const RptCacheEntry& rSrc) const
		{
			return rSrc.nReportType == nReportType
				&& rSrc.tmDesiredStartTime == tmDesiredStartTime
				&& rSrc.tmDesiredEndTime == tmDesiredEndTime
				&& rSrc.wstrFilterHash == wstrFilterHash
				&& (rSrc.nRecursionLevelLimit == -1 && nRecursionLevelLimit == -1 || 
					rSrc.nRecursionLevelLimit == nRecursionLevelLimit + 1)
				&& rSrc.nPeriodToUpdateCacheInMinutes == nPeriodToUpdateCacheInMinutes;
		}
	};

	typedef std::map<std::wstring, RptCacheEntry> RptCache;

	class SrvHierarchyImplBase:
		public KLSRVH::SrvHierarchySrv,
		public KLTP::ThreadsPool::Worker,
		public KLSRV::Deinitializer
	{
	};

    class SrvHierarchyImpl:	public KLSTD::KLBaseImpl<KLSRVH::SrvHierarchyImplBase>
    {
    public:
        SrvHierarchyImpl();
        virtual ~SrvHierarchyImpl();

        void Initialize(
			KLSRV::SrvData* pSrvData, 
            KLSRV::ServerCallbacks* pServerCallbacks,
			KLSTD::CAutoPtr<KLSRV::ServerHelpers> pServerHelpers,
			KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> pGroupTaskControl,
			KLSTD::CAutoPtr<KLPKG::PackagesSrv> pPackagesSrv);

        // Deinitializer method:
        void Deinitialize();

        // ChildServers methods:
		virtual void GetChildServers(
			KLDB::DbConnectionPtr pDbConn,
			long nGroupId,
			std::vector<KLSRVH::child_server_info_t>& vecServers);

		virtual void GetServerInfo(
			KLDB::DbConnectionPtr pDbConn,
			long nId,
			const wchar_t**	fields,
			int size,
			KLPAR::Params** info);

		virtual void GetServerInfoByInstanceId(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrInstanceId,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info);

		virtual void GetServerInfoByPublicKeyHash(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPublicKeyHash,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info);

		virtual long RegisterServer(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrDisplName,
			long nGroupId,
			void* pCertificate,
			long nCertificateLen,
			const std::wstring& wstrNetAddress,
			KLSTD::CAutoPtr<KLPAR::Params> pAdditionalInfo);
		
		virtual void DelServer(
			KLDB::DbConnectionPtr pDbConn,
			long nId);

		virtual void UpdateServer(
			KLDB::DbConnectionPtr pDbConn,
			long nId,
			KLSTD::CAutoPtr<KLPAR::Params> pInfo);

        // SrvHierarchyMaster methods:
		virtual void InitialPingMaster(	
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams);

		virtual void PingMaster(		
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams);

		virtual void ReportRemoteInstallationResult(
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrInstanceId,
			std::wstring wstrTaskFileId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent);
		
        // SrvHierarchySlave methods:
		virtual void PingSlave(			
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams);

		virtual bool ForceRetranslateUpdates(						
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams);

		virtual bool RecreateInstallationPackage(						
			const std::wstring& wstrPackageGUID,
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			bool bReplaceExisting,
			std::wstring& wstrCreatedPackageName);
		
		virtual void StartTask(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrTaskTsId,
			const std::wstring& wstrTaskTypeName,
			KLSTD::CAutoPtr<KLPAR::Params> parParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parOutParams);

		virtual bool StartRemoteInstallation(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped);

		virtual void OnDeliverReport(		
			const std::wstring& wstrRemoteConnectionName,
			const std::wstring& wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			const std::wstring& wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult);

		virtual void OnNotifyOnReportResult(
			const std::wstring& wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams);
		
		
		// Implementation:
		
		virtual long CheckServerExists(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrPublicKeyHash);

		virtual void GetServerInfoByHostName(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrHostName,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info);

		virtual void GetServerInfoByHostId(						
			KLDB::DbConnectionPtr pDbConn,
			long nHostId,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info);
		
		long CheckChildServerInstanceId(
			KLDB::DbConnectionPtr   pDbConn,
			const std::wstring&     wstrPublicKeyHash, 
			const std::wstring&     wstrVersion,
			const std::wstring&     wstrInstanceId,
            AVP_dword               nVersion,
			long&					nHostId);

		virtual void GetChildServersRecursive(
			KLDB::DbConnectionPtr pDbConn,
			long nGroupId,
			std::vector<KLSRVH::child_server_info_t>& vecServers);

		virtual void ForceRetranslateUpdatesOnSlaves(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams);
		
		virtual void GetServerGroupId( KLDB::DbConnectionPtr &pDbConn,
			long nId,
			long &nGroupId );

		virtual void GetHrchCache(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrInstanceId, 
			SrvHrchCache& cache);

		void SyncRptCacheInfo(
			KLDB::DbConnectionPtr pDbConn,
			KLPAR::ParamsPtr parParams);

		void GetRptCache(
			KLDB::DbConnectionPtr pDbConn,
			RptCache& dbRptCache);
		
    private:
        KLSTD::CAutoPtr<KLSRV::ServerCallbacks> m_pServerCallbacks;
		KLSTD::CAutoPtr<KLSRV::ServerHelpers> m_pServerHelpers;
		KLSRV::SrvData* m_pSrvData;
		KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> m_pGroupTaskControl;
		KLSTD::CAutoPtr<KLPKG::PackagesSrv> m_pPackagesSrv;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pDataCS;
		std::map<std::wstring, std::wstring> m_mapFileds;

		int m_nCurrRegisterServerCertLen;
		int m_nCurrRegisterServerParamsLen;
		int m_nCurrUpdateServerCertLen;
		int m_nCurrUpdateServerParamsLen;

		long m_lTicksToConnect;

		KLTP::ThreadsPool::WorkerId m_idPingSlavesWorker;

		void GetServerInfo(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrWhereClause,
			const wchar_t**	fields,
			int size,
			KLPAR::Params** info);
		
		void PublishSlaveConnectedEvent(KLDB::DbConnectionPtr, long nSlaveSrvId);

		int RunWorker(KLTP::ThreadsPool::WorkerId wId);

		void PingSlaves();

		void SetHierarchyInfo(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrInstanceId, 
			KLPAR::ParamsPtr parData);
    };
}

#endif //__KLSRVH_SRVHRCH_IMPL_H__
