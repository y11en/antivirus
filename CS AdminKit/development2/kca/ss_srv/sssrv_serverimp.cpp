/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_server.cpp
 * \author	Andrew Kazachkov
 * \date	05.05.2003 13:22:09
 * \brief	
 * 
 */

#include <std/base/kldefs.h>
#include <std/par/paramsi.h>
#include <std/trc/trace.h>
#include <kca/prss/settingsstorage.h>
#include <transport/tr/transport.h>
#include <kca/prss/helpersi.h>
#include "kca/ss_srv/ss_srv.h"
#include "./sssrv_serverimp.h"
#include "kca/ss_srv/sssrv_proxy.h"
#include <kca/prcp/proxybase.h>
#include <kca/aud/auditsource.h>

#include <kca/prci/prci_auth.h>
#include "kca/ss_srv/sss_actions.h"


//using namespace std;
using namespace KLERR;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;

#define KLCS_MODULENAME L"KLSSS"

#define KLSSS_INITCONTEXT() \
			CAutoPtr<KLPRCI::SecContext> pSecContext;	\
			if(!KLPRCI_GetClientContext(&pSecContext))	\
				KLSTD_THROW(STDE_NOACCESS);
            

namespace KLSSS
{
    CSettingsStorageServer::CSettingsStorageServer()
        :   KLSTD::ModuleClass(&m_lckServer)
        ,   m_nPort(0)
        ,   m_bLocalOnly(false)
    {        
        AVTL_AcquireAclLibrary(&m_pSecLib);
        KLSTD_ASSERT(m_pSecLib);        
        KLSTD_CreateCriticalSection(&m_pCS);
    };

    CSettingsStorageServer::~CSettingsStorageServer()
    {       
        if(m_nPort)
        {
            KLERR_IGNORE(KLSSS_WritePortValue(
                                m_wstrProduct,
                                m_wstrVersion,
                                m_wstrId, 0));
            KLERR_IGNORE(KLTR_GetTransport()->DeleteListenLocation(
                                m_wstrTransportName.c_str(),
                                m_wstrLocation.c_str(),
                                m_nPort));
        };
    };

    bool CSettingsStorageServer::AccessCheck(
                        const wchar_t*      szwProxy,
                        const wchar_t*      szwSS,
                        const wchar_t*      szwProduct,
                        const wchar_t*      szwVersion,
                        const wchar_t*      szwSection,
                        KLPRCI::SecContext* pContext,
						AVP_dword           dwAccessMask,
						bool                bThrow)
    {   
        /*
        std::wstring wstrSS;        
        if(!szwSS || !szwSS[0])
        {
            KLSTD_CHKINPTR(szwProxy);
            CAutoPtr<SettingsStorage>   pSS;
            Storages_Find(szwProxy, &pSS, &wstrSS);
        }
        else
            wstrSS=szwSS;
        KLSEC_SECTION_DESC desc;
        desc.szwType=wstrSS.c_str();
        desc.lType=-1;
        desc.lSubType=-1;
        KLERR_BEGIN
            if(wstrSS == KLPRSS_GetSettingsStorageLocationI(SS_RUNTIME, SSOT_SMART))
            {
                desc.lType=SS_RUNTIME;
                desc.lSubType=SSOT_DIRECT;
            }
            else if(wstrSS == KLPRSS_GetSettingsStorageLocationI(SS_PRODINFO, SSOT_SMART))
            {
                desc.lType=SS_PRODINFO;
                desc.lSubType=SSOT_DIRECT;
            }
            else if(wstrSS == KLPRSS_GetSettingsStorageLocationI(SS_SETTINGS, SSOT_SMART))
            {
                desc.lType=SS_SETTINGS;
                desc.lSubType=SSOT_SMART;
            }
            else if(wstrSS == KLPRSS_GetSettingsStorageLocationI(SS_SETTINGS, SSOT_LOCAL_MACHINE))
            {
                desc.lType=SS_SETTINGS;
                desc.lSubType=SSOT_LOCAL_MACHINE;
            }
            else if(wstrSS == KLPRSS_GetSettingsStorageLocationI(SS_SETTINGS, SSOT_DIRECT))
            {
                desc.lType=SS_SETTINGS;
                desc.lSubType=SSOT_DIRECT;
            };
        KLERR_ENDT(1)
        
		KLAVT::ACE_DECLARATION*				pAcl=NULL;
		long								nAcl=0;        
		if(!m_pSecLib->GetACLForSection(
                                    desc,
                                    szwProduct,
                                    szwVersion,
                                    szwSection,
                                    (const KLAVT::ACE_DECLARATION**)&pAcl,
                                    &nAcl))
		{
			KLSTD_THROW(STDE_NOACCESS);
		};
        KLPRCI::AutoAcl	aa(m_pSecLib, pAcl, nAcl);
		bool checkRes = KLPRCI_AccessCheck(
                            pContext,
                            aa.GetAcl(),
                            aa.GetAclLen(),
                            dwAccessMask,
                            bThrow);

		if ( dwAccessMask & KLAUTH_SS_MODIFY_SECTION || dwAccessMask & KLAUTH_SS_CREATE_SECTION ||
			dwAccessMask & KLAUTH_SS_DELETE_SECTION )
		{
			KLAUD_NOTIFY_SS_CHANGED( szwProduct, szwVersion, szwSection );
		}        
		return checkRes;
        */
        return true;
    }

    void CSettingsStorageServer::Create(
                    const std::wstring& wstrProduct,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrID,
                    bool                bLocalOnly)
    {        
        m_wstrProduct=wstrProduct;
        m_wstrVersion=wstrVersion;
        m_wstrId=wstrID;
        m_bLocalOnly=bLocalOnly;
        m_wstrLocation=KLPRCP::makeLocalAddress(0);
        KLPRCI::ComponentId idComponent(
                        wstrProduct,
                        wstrVersion,
                        c_szwSsServerComponent,
                        wstrID);
        KLTRAP::ConvertComponentIdToTransportName(
                                                    m_wstrTransportName,
                                                    idComponent);
        ;
        KLTR_GetTransport()->AddListenLocation(                                
                                m_wstrTransportName.c_str(),
                                m_wstrLocation.c_str(),
                                m_nPort);
        KLSSS_WritePortValue(m_wstrProduct, m_wstrVersion, m_wstrId, m_nPort);
    };

    //SettingsStorageServer

    void CSettingsStorageServer::OnCloseConnection(
                                            const std::wstring& wstrConnection)
    {        
        KLERR_BEGIN
            Storages_RemoveForConection(wstrConnection);
        KLERR_ENDT(1)
    };

    void CSettingsStorageServer::Close(const std::wstring& wstrProxyId)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        pSecContext,
                        KLAUTH_SS_CONNECT,
                        true);
        Storages_Remove(wstrProxyId, NULL);
    };

    void CSettingsStorageServer::CreateSettingsStorage(
                const std::wstring& wstrConnection,
                const std::wstring& wstrLocation,
				AVP_dword           dwCreationFlags,
				AVP_dword           dwAccessFlags,
                std::wstring&       wstrProxyId,
                const std::wstring& wstrSSUser,
                const KLPRSS::ss_format_t* pFormat)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        NULL,
                        wstrLocation.c_str(),
                        NULL,
                        NULL,
                        NULL,
                        pSecContext,
                        KLAUTH_SS_CONNECT,
                        true);
        CAutoPtr<SettingsStorage> pSS;
        wstrProxyId=KLSTD_CreateLocallyUniqueString();
        KLPRSS_CreateSettingsStorageDirect2(
                                wstrLocation,
                                dwCreationFlags,
                                dwAccessFlags,
                                &pSS,
                                wstrSSUser,
                                pFormat);
        Storages_Add(wstrProxyId, wstrConnection, pSS, wstrLocation);
    };
    
    void CSettingsStorageServer::CreateSettingsStorage(
                const std::wstring& wstrConnection,
                const std::wstring& wstrLocation,
				AVP_dword           dwCreationFlags,
				AVP_dword           dwAccessFlags,
                std::wstring&       wstrProxyId,
                const std::wstring& wstrSSUser,
                KLPAR::Params*      pFormat)
    {
        KLPRSS::ss_format_t sf;
        if(pFormat)
        {        
            sf.nVersion = GetIntValue(pFormat, KLSSS_OPT_SSFORMAT_VERSION);
            sf.id = (KLPRSS::ss_formatid_t)GetIntValue(pFormat, KLSSS_OPT_SSFORMAT_ID);
            if(sf.nVersion != 1)
                KLSTD_NOTIMP();
        };
        CreateSettingsStorage(
                    wstrConnection,            
                    wstrLocation,
                    dwCreationFlags,
                    dwAccessFlags,
                    wstrProxyId,
                    wstrSSUser,
                    &sf);
    };

    void CSettingsStorageServer::Read(
                const std::wstring& wstrProxyId,
                const std::wstring& wstrName,
                const std::wstring& wstrVersion,
                const std::wstring& wstrSection,
                KLPAR::Params*      pExtra,
                KLPAR::Params**     ppData)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        wstrName.c_str(),
                        wstrVersion.c_str(),
                        wstrSection.c_str(),
                        pSecContext,
                        KLAUTH_SS_CONNECT|KLAUTH_SS_READ_SECTION,
                        true);
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        if(!pExtra)
            pSS->Read(wstrName, wstrVersion, wstrSection, ppData);
        else
            pSS->Read2(wstrName, wstrVersion, wstrSection, pExtra, ppData);
    };

    void CSettingsStorageServer::Write(
                const std::wstring& wstrProxyId,
                const std::wstring& wstrName,
                const std::wstring& wstrVersion,
                const std::wstring& wstrSection,
                AVP_dword           dwFlags,
                KLPAR::Params*      pData)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        wstrName.c_str(),
                        wstrVersion.c_str(),
                        wstrSection.c_str(),
                        pSecContext,
                        KLAUTH_SS_CONNECT|((dwFlags == CF_CREATE_NEW)
                            ?   KLAUTH_SS_APPEND_SECTION
                            :   KLAUTH_SS_MODIFY_SECTION),
                        true);
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        switch(dwFlags)
        {
        case CF_OPEN_EXISTING:
            pSS->Update(wstrName, wstrVersion, wstrSection, pData);
            break;
        case CF_CREATE_NEW:
            pSS->Add(wstrName, wstrVersion, wstrSection, pData);
            break;
        case CF_OPEN_ALWAYS:
            pSS->Replace(wstrName, wstrVersion, wstrSection, pData);
            break;
        case CF_CREATE_ALWAYS:
            pSS->Clear(wstrName, wstrVersion, wstrSection, pData);
            break;
        case CF_CLEAR:
            pSS->Delete(wstrName, wstrVersion, wstrSection, pData);
            break;
        default:
            KLSTD_THROW_BADPARAM(dwFlags);
        };
    };

    void CSettingsStorageServer::CreateSection(
                const std::wstring& wstrProxyId,
                const std::wstring& wstrName,
                const std::wstring& wstrVersion,
                const std::wstring& wstrSection)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        wstrName.c_str(),
                        wstrVersion.c_str(),
                        wstrSection.c_str(),
                        pSecContext,
                        KLAUTH_SS_CREATE_SECTION|KLAUTH_SS_CONNECT,
                        true);
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        pSS->CreateSection(wstrName, wstrVersion, wstrSection);
    };

    void CSettingsStorageServer::DeleteSection(
                const std::wstring& wstrProxyId,
                const std::wstring& wstrName,
                const std::wstring& wstrVersion,
                const std::wstring& wstrSection)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        wstrName.c_str(),
                        wstrVersion.c_str(),
                        wstrSection.c_str(),
                        pSecContext,
                        KLAUTH_SS_DELETE_SECTION|KLAUTH_SS_CONNECT,
                        true);
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        pSS->DeleteSection(wstrName, wstrVersion, wstrSection);
    };

    void CSettingsStorageServer::SetTimeout(
                const std::wstring& wstrProxyId,
                AVP_dword           dwTimeout)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        pSecContext,
                        KLAUTH_SS_CONNECT,
                        true);
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        pSS->SetTimeout(dwTimeout);
    };

	void CSettingsStorageServer::GetNames(
                const std::wstring&         wstrProxyId,
                const std::wstring&         wstrName,
                const std::wstring&         wstrVersion,
			    std::vector<std::wstring>&	vecNames)
    {
        KLSSS_INITCONTEXT();
        AccessCheck(
                        wstrProxyId.c_str(),
                        NULL,
                        wstrName.c_str(),
                        wstrVersion.c_str(),
                        NULL,
                        pSecContext,
                        KLAUTH_SS_ENUM_SECTIONS|KLAUTH_SS_CONNECT,
                        true);
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        pSS->GetNames(wstrName, wstrVersion, vecNames);
    };
    
    void CSettingsStorageServer::AttrRead(
                const std::wstring& wstrProxyId,
                const std::wstring& wstrName,
                const std::wstring& wstrVersion,
                const std::wstring& wstrSection,
                const std::wstring& wstrAttr,
                KLPAR::Value**      ppValue)
    {
        KLSSS_INITCONTEXT();
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        pSS->AttrRead(wstrName, wstrVersion, wstrSection, wstrAttr, ppValue);
    };
    
    void CSettingsStorageServer::EnumAllWSections(
                const std::wstring& wstrProxyId,
                KLPRSS::sections_t& sections)
    {
        KLSSS_INITCONTEXT();
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        KLSTD::CAutoPtr<KLPRSS::SsBulkMode> pSsBulkMode;
        pSS->QueryInterface(
                                KLSTD_IIDOF(KLPRSS::SsBulkMode),
                                (void**)&pSsBulkMode);
        if(!pSsBulkMode)
            KLSTD_NOTIMP();
        ;
        pSsBulkMode->EnumAllWSections(sections);
    };

    void CSettingsStorageServer::ReadSections(
                const std::wstring& wstrProxyId,
                const KLPRSS::sections_t&   sections,
                KLPAR::ArrayValue** ppData)
    {
        KLSSS_INITCONTEXT();
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        KLSTD::CAutoPtr<KLPRSS::SsBulkMode> pSsBulkMode;
        pSS->QueryInterface(
                                KLSTD_IIDOF(KLPRSS::SsBulkMode),
                                (void**)&pSsBulkMode);
        if(!pSsBulkMode)
            KLSTD_NOTIMP();
        ;
        pSsBulkMode->ReadSections(sections, ppData);
    };

    void CSettingsStorageServer::WriteSections(
                const std::wstring& wstrProxyId,
                const KLPRSS::sections_t&   sections,
                KLPAR::ArrayValue*  pData)
    {
        KLSSS_INITCONTEXT();
        CAutoPtr<SettingsStorage> pSS;
        Storages_Find(wstrProxyId, &pSS);
        KLSTD::CAutoPtr<KLPRSS::SsBulkMode> pSsBulkMode;
        pSS->QueryInterface(
                                KLSTD_IIDOF(KLPRSS::SsBulkMode),
                                (void**)&pSsBulkMode);
        if(!pSsBulkMode)
            KLSTD_NOTIMP();
        ;
        pSsBulkMode->WriteSections(sections, pData);
    };

    void CSettingsStorageServer::Storages_Add(
                const std::wstring&         wstrId,
                const std::wstring&         wstrConnection,
                KLPRSS::SettingsStorage*    pSS,
                const std::wstring&         wstrLocation)
    {
        KLSTD_ASSERT_THROW(!wstrId.empty());

        KLSTD_CHKINPTR(pSS);
        AutoCriticalSection acs(m_pCS);
        id2storage_t::iterator it=m_mapId2Storage.find(wstrId);
        if(it != m_mapId2Storage.end())
            KLSTD_THROW(STDE_EXIST);

        m_mapId2Storage.insert(id2storage_t::value_type(
                                            wstrId,
                                            ss_data_t(pSS, wstrLocation)));

        if(!wstrConnection.empty())
            m_mapConn2Id.insert(conn2id_t::value_type(wstrConnection, wstrId));

        KLSTD_TRACE3(
                3,
                L"Added store '%ls' in ss-server '%ls' (now %d stores)\n",
                wstrId.c_str(),
                m_wstrId.c_str(),
                m_mapId2Storage.size());

    };

    void CSettingsStorageServer::Storages_Find(
                const std::wstring&         wstrId,
                KLPRSS::SettingsStorage**   ppSS,
                std::wstring*               pwstrLocation)
    {
        KLSTD_ASSERT_THROW(!wstrId.empty());

        KLSTD_CHKOUTPTR(ppSS);
        AutoCriticalSection acs(m_pCS);
        id2storage_t::iterator it=m_mapId2Storage.find(wstrId);
        if(it == m_mapId2Storage.end())
            KLSTD_THROW(STDE_NOTFOUND);
        it->second.m_pSS.CopyTo(ppSS);
        if(pwstrLocation)
            *pwstrLocation=it->second.m_wstrLocation;
    };

    void CSettingsStorageServer::Storages_RemoveForConection(const std::wstring& wstrConnection)
    {
       KLSTD_ASSERT_THROW(!wstrConnection.empty());
        for(;;)
        {
            CAutoPtr<KLPRSS::SettingsStorage> pTmp;
            {
                AutoCriticalSection acs(m_pCS);
                conn2id_t::iterator itconn=m_mapConn2Id.find(wstrConnection);
                if(itconn == m_mapConn2Id.end())
                    break;
                id2storage_t::iterator it=m_mapId2Storage.find(itconn->second);
                m_mapConn2Id.erase(itconn);
                if(it == m_mapId2Storage.end())
                    continue;
                pTmp=it->second.m_pSS;
                std::wstring wstrId=it->first;
                m_mapId2Storage.erase(it);
                KLSTD_TRACE3(
                        3,
                        L"Removed store '%ls' in ss-server '%ls' (now %d stores)\n",
                        wstrId.c_str(),
                        m_wstrId.c_str(),
                        m_mapId2Storage.size());
            };
            pTmp=NULL;
        };
    };

    bool CSettingsStorageServer::Storages_Remove(
                const std::wstring&         wstrId,
                KLPRSS::SettingsStorage**   ppSS)
    {
        CAutoPtr<SettingsStorage> pSS;
        {
            AutoCriticalSection acs(m_pCS);
            id2storage_t::iterator it=m_mapId2Storage.find(wstrId);
            if(it == m_mapId2Storage.end())
                return false;
            pSS=it->second.m_pSS;
            m_mapId2Storage.erase(it);
            KLSTD_TRACE3(
                    3,
                    L"Removed store '%ls' in ss-server '%ls' (now %d stores)\n",
                    wstrId.c_str(),
                    m_wstrId.c_str(),
                    m_mapId2Storage.size());
        };
        if(ppSS)
            pSS.CopyTo(ppSS);
        return true;
    };
};

using namespace KLSSS;

void KLSSS_CreateSettingsStorageServer(
                    const std::wstring&             wstrProduct,
                    const std::wstring&             wstrVersion,
                    const std::wstring&             wstrID,
                    KLSSS::SettingsStorageServer**  ppServer,
                    bool                            bLocalOnly)
{
    KLSTD_ASSERT(g_bWasInitialized);
    if(!g_bWasInitialized)KLSTD_NOINIT(KLCS_MODULENAME);

    bLocalOnly = false;//@todo changes in transport and WAT required !!!
    CSettingsStorageServer* p=new CSettingsStorageServer;
    KLSTD_CHKMEM(p);
    CAutoPtr<SettingsStorageServer> pServer;
    pServer.Attach(p);
    p->Create(wstrProduct, wstrVersion, wstrID, bLocalOnly);
    pServer.CopyTo(ppServer);
};
