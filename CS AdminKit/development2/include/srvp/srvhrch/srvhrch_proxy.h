/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file srvhrch/srvhrch_proxy.h
 * \author Andrew Lashenkov
 * \date	30.12.2004 16:17:00
 * \brief Прокси для интерфейса списка подчиненных серверов.
 *
 */
#ifndef KLSRVH_SRVHRCHPROXY_H
#define KLSRVH_SRVHRCHPROXY_H

#include <std/base/klbaseqi_imp.h>
#include <std/err/klerrors.h>

#include <kca/prci/componentid.h>
#include <kca/prcp/proxybase.h>

#include <srvp/srvhrch/srvhrch.h>
#include <srvp/srvhrch/srvhrch_client.h>


namespace KLSRVH
{
	class ChildServersProxyBase
	:	
		public KLSRVH::ChildServers, 
		public KLPRCP::CProxyBase
	{
	};

    class ChildServersProxy : 
	public KLSTD::KLBaseImpl<ChildServersProxyBase>{
    public:
		ChildServersProxy();
		virtual ~ChildServersProxy();

        KLSTD_INTERAFCE_MAP_REDIRECT(m_pOwner);
		
	    void Initialize(
                    KLSTD::KLBaseQI*    pOwner,
                    KLPRCP::CProxyBase* pOwnersProxy)
        {
            KLSTD_ASSERT(pOwnersProxy != NULL && pOwner != NULL);
            m_pOwnersProxy=pOwnersProxy;
            m_pOwner=pOwner;
        };
    protected:
        KLSTD::KLBaseQI*    m_pOwner;
        KLPRCP::CProxyBase* m_pOwnersProxy;
		
    public:
		virtual void GetChildServers(
			long nGroupId,
			std::vector<child_server_info_t>& vecServers);

		virtual void GetServerInfo(
			long nId,
			const wchar_t**	fields,
			int size,
			KLPAR::Params** info);

		virtual void GetServerInfoByInstanceId(						
			const std::wstring& wstrInstanceId,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info);

		virtual void GetServerInfoByPublicKeyHash(						
			const std::wstring& wstrPublicKeyHash,
			const wchar_t**		fields,
			int					size,
			KLPAR::Params**     info);

		virtual long RegisterServer(
			const std::wstring& wstrDisplName,
			long nGroupId,
			void* pCertificate,
			long nCertificateLen,
			const std::wstring& wstrNetAddress,
			KLSTD::CAutoPtr<KLPAR::Params> pAdditionalInfo);

		virtual void DelServer(long nId);

		virtual void UpdateServer(
			long nId,
			KLSTD::CAutoPtr<KLPAR::Params> pInfo );
    };
}

#endif //KLSRVH_SRVHRCHPROXY_H