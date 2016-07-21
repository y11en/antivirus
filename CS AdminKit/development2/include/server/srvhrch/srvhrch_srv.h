/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvhrch\srvhrch_srv.h
 * \author	Andrew Lashenkov
 * \date	29.07.2005
 * \brief	Сервернай интерфейс ChildServers & SrvHierarchyMaster & SrvHierarchySlave
 * 
 */

#ifndef __KLSRVH_SRVHRCH_SRV_H__
#define __KLSRVH_SRVHRCH_SRV_H__

#include <server/srv_common.h>
#include <server/srvinst/klserver.h>
#include <srvp/srvhrch/srvhrch_lst.h>
#include <server/db/dbconnection.h>

#define KLSRVH_RPT_CACHE_STRUCT_VER L"KLSRVH_RPT_CACHE_STRUCT_VER"
#define KLSRVH_RPT_CACHE_STRUCT L"KLSRVH_RPT_CACHE_STRUCT"

namespace KLSRVH
{
	struct SrvHrchCacheEntry{
		std::wstring wstrParentId;
		std::wstring wstrDisplayName;
		bool operator==(const SrvHrchCacheEntry& rSrc) const
		{
			return rSrc.wstrParentId == wstrParentId && rSrc.wstrDisplayName == wstrDisplayName;
		}
	};

	typedef std::map<std::wstring, SrvHrchCacheEntry> SrvHrchCache;

    class SrvHierarchySrv:	public KLSTD::KLBase {
    public:
        virtual ~SrvHierarchySrv(){}

        // ChildServers methods:
		virtual void GetChildServers(
			KLDB::DbConnectionPtr pDbConn,
			long nGroupId,
			std::vector<KLSRVH::child_server_info_t>& vecServers) = 0;

		virtual void GetServerInfo(
			KLDB::DbConnectionPtr pDbConn,
			long nId,
			const wchar_t**	fields,
			int size,
			KLPAR::Params** info) = 0;

		virtual void GetServerInfoByInstanceId(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrInstanceId,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info) = 0;

		virtual void GetServerInfoByPublicKeyHash(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPublicKeyHash,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info) = 0;

		virtual long RegisterServer(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrDisplName,
			long nGroupId,
			void* pCertificate,
			long nCertificateLen,
			const std::wstring& wstrNetAddress,
			KLSTD::CAutoPtr<KLPAR::Params> pAdditionalInfo) = 0;
		
		virtual void DelServer(
			KLDB::DbConnectionPtr pDbConn,
			long nId) = 0;

		virtual void UpdateServer(
			KLDB::DbConnectionPtr pDbConn,
			long nId,
			KLSTD::CAutoPtr<KLPAR::Params> pInfo) = 0;

        // SrvHierarchyMaster methods:
		virtual void InitialPingMaster(	
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams) = 0;

		virtual void PingMaster(		
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams) = 0;

		virtual void OnDeliverReport(		
			const std::wstring& wstrRemoteConnectionName,
			const std::wstring& wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			const std::wstring& wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult) = 0;

		virtual void ReportRemoteInstallationResult(
			KLDB::DbConnectionPtr pDbConn,
			std::wstring wstrInstanceId,
			std::wstring wstrTaskFileId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent) = 0;
		
		virtual void OnNotifyOnReportResult(
			const std::wstring& wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams) = 0;
		
        // SrvHierarchySlave methods:
		virtual void PingSlave(			
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams) = 0;

		virtual bool ForceRetranslateUpdates(						
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams) = 0;
		
		virtual bool RecreateInstallationPackage(						
			const std::wstring& wstrPackageGUID,
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			bool bReplaceExisting,
			std::wstring& wstrCreatedPackageName) = 0;

		virtual void StartTask(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrTaskTsId,
			const std::wstring& wstrTaskTypeName,
			KLSTD::CAutoPtr<KLPAR::Params> parParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parOutParams) = 0;

		virtual bool StartRemoteInstallation(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped) = 0;
		
		// Implementation:
		
		virtual long CheckServerExists(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrPublicKeyHash) = 0;

		virtual void GetServerInfoByHostName(						
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrHostName,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info) = 0;

		virtual void GetServerInfoByHostId(						
			KLDB::DbConnectionPtr pDbConn,
			long nHostId,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info) = 0;


        //<-- Changed by andkaz 24.04.2006 14:27:52
        /*
        // -->
		virtual long CheckChildServerInstanceId(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrPublicKeyHash, 
			const std::wstring& wstrVersion,
			const std::wstring& wstrInstanceId) = 0;
        //<-- Changed by andkaz 24.04.2006 14:27:52
        */
        // -->

		virtual void GetChildServersRecursive(
			KLDB::DbConnectionPtr pDbConn,
			long nGroupId,
			std::vector<KLSRVH::child_server_info_t>& vecServers) = 0;

		virtual void ForceRetranslateUpdatesOnSlaves(
			KLDB::DbConnectionPtr pDbConn,
			KLSTD::CAutoPtr<KLPAR::Params> pParams) = 0;
		

		// Authorization helpers

		virtual void GetServerGroupId( KLDB::DbConnectionPtr &pDbConn,
			long nId,
			long &nGroupId ) = 0;
		
		// Cache accessors:
		virtual void GetHrchCache(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrInstanceId, 
			SrvHrchCache& cache) = 0;
    };
}

#endif //__KLSRVH_SRVHRCH_SRV_H__
