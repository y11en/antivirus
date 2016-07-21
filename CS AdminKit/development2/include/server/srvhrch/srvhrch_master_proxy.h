/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch_master_proxy.h
 * \author Andrew Lashenkov
 * \date	24.01.2005 10:24:00
 * \brief Прокси интерфейса мастер-сервера иерархии серверов.
 *
 */

#ifndef KLSRVH_SRVHRCH_MASTER_PROXY_H
#define KLSRVH_SRVHRCH_MASTER_PROXY_H

#include <std/base/klbaseqi_imp.h>
#include <server/srvhrch/srvhrch_master.h>
#include <kca/prcp/proxybase.h>
#include <std/err/klerrors.h>

namespace KLSRVH {

	class SrvHierarchyMasterProxy 
	:	
		public KLSTD::KLBaseImpl<KLSRVH::SrvHierarchyMaster>,
		public KLPRCP::CProxyBase
    {
    public:
        KLSTD_SINGLE_INTERAFCE_MAP(KLSRVH::SrvHierarchyMaster);

        SrvHierarchyMasterProxy();
        
        ~SrvHierarchyMasterProxy();

        // Deinitializer method:
        void Deinitialize();

        // SrvHierarchyMaster methods:

		void InitialPingMaster(						
			KLSTD::CAutoPtr<KLPAR::Params> pInParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams);

		void PingMaster(						
			KLSTD::CAutoPtr<KLPAR::Params> pInParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams);

		void DeliverReport(						
			std::wstring wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			std::wstring wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult);
		
		void ReportRemoteInstallationResult(
			std::wstring wstrTaskTsId,
			KLSTD::CAutoPtr<KLPAR::Params> parResultTaskStateEvent);
		
		void NotifyOnReportResult(
			std::wstring wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams);
	};

} // namespace KLSRVH

#endif // KLSRVH_SRVHRCH_MASTER_PROXY_H

