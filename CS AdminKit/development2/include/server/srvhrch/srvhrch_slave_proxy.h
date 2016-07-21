/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch_slave_proxy.h
 * \author Andrew Lashenkov
 * \date	01.08.2005 16:43:00
 * \brief Прокси интерфейса подчиненного сервера иерархии серверов.
 *
 */

#ifndef KLSRVH_SRVHRCH_SLAVE_PROXY_H
#define KLSRVH_SRVHRCH_SLAVE_PROXY_H

#include <std/base/klbaseqi_imp.h>
#include <server/srvhrch/srvhrch_slave.h>
#include <kca/prcp/proxybase.h>
#include <std/err/klerrors.h>

namespace KLSRVH {

	class SrvHierarchySlaveProxy 
	:	
		public KLSTD::KLBaseImpl<KLSRVH::SrvHierarchySlave>,
		public KLPRCP::CProxyBase
    {
    public:
        KLSTD_SINGLE_INTERAFCE_MAP(KLSRVH::SrvHierarchySlave);

        SrvHierarchySlaveProxy();
        
        ~SrvHierarchySlaveProxy();

        // Deinitializer method:
        void Deinitialize();

        // SrvHierarchySlave methods:

		void PingSlave(						
			KLSTD::CAutoPtr<KLPAR::Params> pInParams,
			KLSTD::CAutoPtr<KLPAR::Params>& pOutParams);

		bool ForceRetranslateUpdates(						
			KLSTD::CAutoPtr<KLPAR::Params> pParams);
		
		bool RecreateInstallationPackage(						
			const std::wstring& wstrPackageGUID,
			const std::wstring& wstrNewPackageName,
			const std::wstring& wstrFileId,
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			const std::wstring& wstrProductDisplName,
			const std::wstring& wstrProductDisplVersion,
			bool bReplaceExisting,
			std::wstring& wstrCreatedPackageName);
		
		void StartTask(						
			const std::wstring& wstrTaskTsId,
			const std::wstring& wstrTaskTypeName,
			KLSTD::CAutoPtr<KLPAR::Params> parParams,
			KLSTD::CAutoPtr<KLPAR::Params>& parOutParams);
		
		bool StartRemoteInstallation(
			KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
			KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
			bool& bAlreadyInstalledSkipped);
	};

} // namespace KLSRVH

#endif // KLSRVH_SRVHRCH_SLAVE_PROXY_H

