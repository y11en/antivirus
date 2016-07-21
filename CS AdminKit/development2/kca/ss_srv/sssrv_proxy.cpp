/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_proxy.cpp
 * \author	Andrew Kazachkov
 * \date	05.05.2003 14:49:00
 * \brief	
 * 
 */

#include <std/base/kldefs.h>
#include <std/par/paramsi.h>
#include <std/trc/trace.h>
#include <kca/prss/helpersi.h>
#include <kca/prss/settingsstorage.h>
#include <transport/tr/common.h>
#include <kca/ss_srv/ss_srv.h>
#include <kca/ss_srv/sssrv_server.h>
#include <kca/ss_srv/sssrv_proxy.h>
#include <kca/ss_srv/errors.h>
#include <kca/prci/prci_auth.h>
#include <common/win32_unimpersonate.h>

#include <map>

//using namespace std;
using namespace KLERR;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;

#define KLCS_MODULENAME L"KLSSS"

namespace KLSSS
{

#define UNIMPERSONATE() \
            KLWAT::CAutoUnimpersonate au;\
                KLSTD_ASSERT(!m_pAuthServer->IsThreadImpersonated());   \
                KLPRCI::AutoImpersonate ai(m_pCreator);

    SettingsStorageProxy::SettingsStorageProxy(const KLPRSS::ss_format_t* pFormat)
        :   m_pFormat(KLPRSS::DupFormat(pFormat))
    {
        KLSTD_ASSERT_THROW(m_pFormat != NULL);
        KLWAT_GetGlobalAuthServer(&m_pAuthServer);
        m_lckProxy.Lock();
    };

    SettingsStorageProxy::~SettingsStorageProxy()
    {
        KLERR_BEGIN
            if(!m_wstrProxyId.empty())
                Close(m_wstrProxyId);
        KLERR_ENDT(3)
        if(m_pFormat)
            free(m_pFormat);
        KLERR_IGNORE(CProxyBase::ClearConnections());
        m_lckProxy.Unlock();
   };

    void SettingsStorageProxy::OpenSettingsStorage(
            const std::wstring& wstrLocation,
			AVP_dword           dwCreationFlags,
			AVP_dword           dwAccessFlags,
            std::wstring&       wstrProxyId)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            const std::wstring wstrUserStorePath=L"";
            /* uncomment this if using SSOT_SMART
            // we should get SS_USER fs location BEFORE we unimpersonated
            const std::wstring& wstrUserStorePath=
                        KLPRSS_TypeToFileSystemPath(
                                    KLPRSS_MakeTypeG(c_szwSST_CUserSS));
            */
            // we should unimpersonate as we could be impersonated under "rightless" user
            UNIMPERSONATE();
            m_pServer->CreateSettingsStorage(
                                        L"",
                                        wstrLocation,
                                        dwCreationFlags,
                                        dwAccessFlags,
                                        wstrProxyId,
                                        wstrUserStorePath,
                                        m_pFormat);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, CreateSettingsStorage);
            CAutoPtr<Params> pOptions;
            KLPAR::param_entry_t par[]=
            {
                KLPAR::param_entry_t(KLSSS_OPT_SSFORMAT_VERSION, (long)m_pFormat->nVersion),
                KLPAR::param_entry_t(KLSSS_OPT_SSFORMAT_ID,     (long)m_pFormat->id)
            };
            KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pOptions);
            param__params options;
            ParamsForSoap(KLPRCP_PROXY_SOAP(), pOptions, options);
            soap_call_klsssrv_CreateSettingsStorage(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)wstrLocation.c_str(),
                                dwCreationFlags,
                                dwAccessFlags,
                                options,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
            wstrProxyId=r.wstrProxyId ? r.wstrProxyId : L"";
        };
    };

    void SettingsStorageProxy::Close(const std::wstring& wstrProxyId)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->Close(wstrProxyId);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, Close);
            soap_call_klsssrv_Close(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)wstrProxyId.c_str(),
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
        };
    };
    ;
    void SettingsStorageProxy::Create(
            const std::wstring& wstrLocation,
			AVP_dword           dwCreationFlags,
			AVP_dword           dwAccessFlags,
            const std::wstring& wstrProduct,
            const std::wstring& wstrVersion,
            const std::wstring& wstrID)
    {        
        m_wstrId=wstrID;
        KLSTD_ASSERT(!m_wstrId.empty());

        KLERR_BEGIN
            if(GetSsServer(wstrID, &m_pServer, false))//try to use to local SSS
                KLPRCI_CreateSecContext(NULL, &m_pCreator);
        KLERR_ENDT(1)

        if(m_pServer == NULL)// we can't use local SSS, use through tran-t
        {
            KLPRCI::ComponentId id(
                            wstrProduct,
                            wstrVersion,
                            c_szwSsServerComponent,
                            m_wstrId);
            int nPort=0;
            KLERR_TRY    
                nPort=KLSSS_ReadPortValue(
                                            wstrProduct,
                                            wstrVersion,
                                            wstrID);
            KLERR_CATCH(pError)
                KLSSS_THROW_SERVER_UNAVAILABLE(
                                    wstrID,
                                    pError ? pError->GetMsg() : L"?");
            KLERR_ENDTRY
		    create_new_client_connection(
                                id,
                                KLPRCP::makeLocalAddress(nPort).c_str());
        };
        OpenSettingsStorage(
                            wstrLocation,
                            dwCreationFlags,
                            dwAccessFlags,
                            m_wstrProxyId);
    };

    void SettingsStorageProxy::Create(
                const KLPRCP::gateway_locations_t&  vecLocations,
                long                        lTimeout,
                bool                        bSSL,
                const KLPRCP::conn_attr_t*  pExtraAttributes,
                const std::wstring&         wstrLocation,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags)
    {
        KLSTD_CHK(vecLocations, vecLocations.size() > 1);
        KLPRCI::ComponentId idComponent;
        idComponent.GetFromString(
                vecLocations[vecLocations.size()-1].remoteComponentName);
        const std::wstring& wstrID=idComponent.instanceId;
        m_wstrId=wstrID;
        KLSTD_ASSERT(!m_wstrId.empty());
		create_new_gatewayed_connection(vecLocations, lTimeout, bSSL, pExtraAttributes);
        OpenSettingsStorage(
                            wstrLocation,
                            dwCreationFlags,
                            dwAccessFlags,
                            m_wstrProxyId);
    };


	void SettingsStorageProxy::CreateAsCopy(
            const std::wstring&             wstrLocation,
			AVP_dword                       dwCreationFlags,
			AVP_dword                       dwAccessFlags,
            const std::wstring&             wstrID,
            KLSSS::SettingsStorageServer*   pServer,
            KLSTD::KLBase*                  pParent,
            const std::wstring&             wstrLocalComponentName,
            const std::wstring&             wstrConnName)
    {
        KLSTD_ASSERT(!wstrID.empty());
        m_wstrId=wstrID;
        m_pServer=pServer;
        m_pParent=pParent;
        if(m_pServer == NULL)
            Initialize(wstrLocalComponentName, wstrConnName);
        else
        {
            KLPRCI_CreateSecContext(NULL, &m_pCreator);
        };
        OpenSettingsStorage(wstrLocation, dwCreationFlags, dwAccessFlags, m_wstrProxyId);
    };

    bool SettingsStorageProxy::IfUseLocal() const
    {
        //return (m_pServer && !m_pAuthServer->IsThreadImpersonated()); //@todo changes in WAT required !!!
        return m_pServer!=NULL;
    };

    void SettingsStorageProxy::Read(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
			KLPAR::Params**     ppParams)
    {
        Read2(name, version, section, NULL, ppParams);
    };

    void SettingsStorageProxy::Read2(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params*      pExtra,
			KLPAR::Params**     ppParams)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->Read(m_wstrProxyId, name, version, section, pExtra, ppParams);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, Read);
            param__params extra;
            ParamsForSoap(KLPRCP_PROXY_SOAP(), pExtra, extra);
            soap_call_klsssrv_Read(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                (wchar_t*)name,
                                (wchar_t*)version,
                                (wchar_t*)section,
                                extra,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
            ParamsFromSoap(r.result, ppParams);
        };
    };

    void SettingsStorageProxy::Update(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params * settings )
    {
        Write(name, version, section, CF_OPEN_EXISTING, settings);
    };

    void SettingsStorageProxy::Add(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params * settings )
    {
        Write(name, version, section, CF_CREATE_NEW, settings);
    };

    void SettingsStorageProxy::Replace(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params*      settings)
    {
        Write(name, version, section, CF_OPEN_ALWAYS, settings);
    };

    void SettingsStorageProxy::Clear(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params * settings )
    {
        Write(name, version, section, CF_CREATE_ALWAYS, settings);
    };

    void SettingsStorageProxy::Delete(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section,
            KLPAR::Params * settings )
    {
        Write(name, version, section, CF_CLEAR, settings);
    };

    void SettingsStorageProxy::CreateSection(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section )
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->CreateSection(m_wstrProxyId, name, version, section);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, CreateSection);
            soap_call_klsssrv_CreateSection(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                (wchar_t*)name,
                                (wchar_t*)version,
                                (wchar_t*)section,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
        };
    };

    void SettingsStorageProxy::DeleteSection(
            const wchar_t* name, 
            const wchar_t* version,
            const wchar_t* section )
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->DeleteSection(m_wstrProxyId, name, version, section);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, DeleteSection);
            soap_call_klsssrv_DeleteSection(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                (wchar_t*)name,
                                (wchar_t*)version,
                                (wchar_t*)section,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
        };
    };

    void SettingsStorageProxy::SetTimeout( long timeout )
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->SetTimeout(m_wstrProxyId, timeout);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, SetTimeout);
            soap_call_klsssrv_SetTimeout(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                timeout,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
        };
    };

	void SettingsStorageProxy::GetNames(
            const wchar_t*			name, 
            const wchar_t*			version,
            KLSTD::AKWSTRARR&       names)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            std::vector<std::wstring> vecNames;
            m_pServer->GetNames(m_wstrProxyId, name, version, vecNames);
            KLSTD::klwstrarr_t result(vecNames.size());
            for(size_t i = 0; i < vecNames.size(); ++i)
            {
                KLSTD::klwstr_t x(vecNames[i].c_str());
                result.setat(i, x.detach());
            };
            names = result.detach();
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, GetNames);
            soap_call_klsssrv_GetNames(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                (wchar_t*)name,
                                (wchar_t*)version,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
            KLSTD::klwstrarr_t result(r.names.__size);
            if(r.names.__size)
            {
                for(int i=0; i < r.names.__size; ++i)
                {
                    KLSTD::klwstr_t x(r.names.__ptr[i]);
                    result.setat(i, x.detach());
                };
            };
            names = result.detach();
        };
    };

	void SettingsStorageProxy::AttrRead(
            const wchar_t*		name, 
            const wchar_t*		version,
            const wchar_t*		section,
			const wchar_t*		attr,
			KLPAR::Value**			ppValue) const
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->AttrRead(m_wstrProxyId, name, version, section, attr, ppValue);
        }
        else
        {
            KLPRCP_PROXY_LOCK2((SettingsStorageProxy*)this);
            KLPRCP_DEF_RESPONSE(klsssrv_, AttrRead);
            soap_call_klsssrv_AttrRead(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                (wchar_t*)name,
                                (wchar_t*)version,
                                (wchar_t*)section,
                                (wchar_t*)attr,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
            ValueFromSoap(r.value, ppValue);
        };
    };

    void SettingsStorageProxy::Write(
            const std::wstring& name, 
            const std::wstring& version,
            const std::wstring& section,
            AVP_dword           dwFlags,
            KLPAR::Params*      settings)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->Write(m_wstrProxyId, name, version, section, dwFlags, settings);
        }
        else
        {
            KLPRCP_PROXY_LOCK();
            KLPRCP_DEF_RESPONSE(klsssrv_, Write);
            param__params data;
            ParamsForSoap(KLPRCP_PROXY_SOAP(), settings, data);
            soap_call_klsssrv_Write(
                                KLPRCP_PROXY_SOAP(),
                                NULL,
                                NULL,
                                (wchar_t*)m_wstrId.c_str(),
                                (wchar_t*)m_wstrProxyId.c_str(),
                                (wchar_t*)name.c_str(),
                                (wchar_t*)version.c_str(),
                                (wchar_t*)section.c_str(),
                                dwFlags,
                                data,
                                r);
            KLPRCP_PROXY_CHECK();
            KLPRCP_CHKRESULT();            
        };
    };

    void SettingsStorageProxy::CreateSettingsStorage(
				const wchar_t*              location,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags,
				KLPRSS::SettingsStorage**   ppStorage)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        KLSTD_CHKOUTPTR(ppStorage);
        SettingsStorageProxy* pProxy=new SettingsStorageProxy(m_pFormat);
        KLSTD_CHKMEM(pProxy);
        CAutoPtr<KLPRSS::SettingsStorage> pSS;
        pSS.Attach(pProxy);
        pProxy->CreateAsCopy(
                    location,
                    dwCreationFlags,
                    dwAccessFlags,
                    m_wstrId,
                    m_pServer,
                    m_pParent ? m_pParent : static_cast<KLPRSS::SettingsStorage*>(this),
                    m_wstrProxyName,
                    m_wstrRemoteComponentName);
        pSS.CopyTo(ppStorage);
    };

    void SettingsStorageProxy::EnumAllWSections(KLPRSS::sections_t& sections)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->EnumAllWSections(m_wstrProxyId, sections);
        }
        else
        {
            KLSTD_ASSERT(!"EnumAllWSections");
            KLSTD_NOTIMP();
        };
    }

    void SettingsStorageProxy::ReadSections(
                                    const KLPRSS::sections_t&   sections,
                                    KLPAR::ArrayValue** ppData)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->ReadSections(m_wstrProxyId, sections, ppData);
        }
        else
        {
            KLSTD_ASSERT(!"EnumAllWSections");
            KLSTD_NOTIMP();
        };
    }

    void SettingsStorageProxy::WriteSections(
                                    const KLPRSS::sections_t&   sections,
                                    KLPAR::ArrayValue*  pData)
    {
        KLSTD_ASSERT(!m_wstrId.empty());
        if(IfUseLocal())
        {
            UNIMPERSONATE();
            m_pServer->WriteSections(m_wstrProxyId, sections, pData);
        }
        else
        {
            KLSTD_ASSERT(!"EnumAllWSections");
            KLSTD_NOTIMP();
        };
    }
};

using namespace KLSSS;

void KLSSS_CreateSettingsStorage(
                const std::wstring&         wstrLocation,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags,
                KLPRSS::SettingsStorage**   ppSS,
                const std::wstring&         wstrProduct,
                const std::wstring&         wstrVersion,
                const std::wstring&         wstrID,
                const KLPRSS::ss_format_t*  pFormat)
{
    KLSTD_ASSERT(!wstrID.empty());
    KLSTD_CHKOUTPTR(ppSS);
    SettingsStorageProxy* pProxy=new SettingsStorageProxy(pFormat);
    KLSTD_CHKMEM(pProxy);
    CAutoPtr<KLPRSS::SettingsStorage> pSS;
    pSS.Attach(pProxy);
    pProxy->Create(
                wstrLocation,
                dwCreationFlags,
                dwAccessFlags,
                wstrProduct,
                wstrVersion,
                wstrID);
    pSS.CopyTo(ppSS);
};

void KLSSS_CreateRemoteSettingsStorage(
                KLTRAP::Transport::LocationsList&   vecLocations,
                long                                lTimeout,
                bool                                bSSL,
                const KLPRCP::conn_attr_t*          pExtraAttributes,
                const std::wstring&                 wstrLocation,
				AVP_dword                           dwCreationFlags,
				AVP_dword                           dwAccessFlags,
                KLPRSS::SettingsStorage**           ppSS,
                const KLPRSS::ss_format_t*          pFormat)
{    
    KLSTD_CHKOUTPTR(ppSS);
    SettingsStorageProxy* pProxy=new SettingsStorageProxy(pFormat);
    KLSTD_CHKMEM(pProxy);
    CAutoPtr<KLPRSS::SettingsStorage> pSS;
    pSS.Attach(pProxy);
    pProxy->Create(
                vecLocations,
                lTimeout,
                bSSL,
                pExtraAttributes,
                wstrLocation,
                dwCreationFlags,
                dwAccessFlags);
    pSS.CopyTo(ppSS);
}

//KLPRSS
//#define KLCSC_NOT_USE_SS_SERVER

KLCSKCA_DECL void KLPRSS_CreateSettingsStorage2(
					const std::wstring&         location,
					AVP_dword                   dwCreationFlags,
					AVP_dword                   dwAccessFlags,
					KLPRSS::SettingsStorage**   ppStorage,
                    KLPRSS::ss_server_t*        pServerId,
                    const KLPRSS::ss_format_t*  pFormat)
{
#ifdef KLCSC_NOT_USE_SS_SERVER
    KLPRSS_CreateSettingsStorageDirect2(
                                location,
                                dwCreationFlags,
                                dwAccessFlags,
                                ppStorage,
                                L"",
                                pFormat);
#else    
    KLSTD_ASSERT(g_bWasInitialized);    
    if(!g_bWasInitialized)KLSTD_NOINIT(KLCS_MODULENAME);

    if(location == KLPRSS_GetSettingsStorageLocation(SS_SETTINGS, SSOT_CURRENT_USER))
    {
        KLPRSS_CreateSettingsStorageDirect2(
                                    location,
                                    dwCreationFlags,
                                    dwAccessFlags,
                                    ppStorage,
                                    L"",
                                    pFormat);
    }
    else
    {
        std::wstring wstrProduct, wstrVersion, wstrID;
        if( pServerId &&
            !pServerId->id.empty() &&
            !pServerId->product.empty() &&
            !pServerId->version.empty())
        {
            wstrProduct=pServerId->product;
            wstrVersion=pServerId->version;
            wstrID=pServerId->id;
        }
        else
        {
            KLSSS_GetDefaultServer(wstrProduct, wstrVersion, wstrID);
            if(wstrProduct.empty() || wstrVersion.empty())
                ::KLSTD_GetDefaultProductAndVersion(wstrProduct, wstrVersion);
        };

        KLSSS_CreateSettingsStorage(
                                    location,
                                    dwCreationFlags,
                                    dwAccessFlags,
                                    ppStorage,
                                    wstrProduct,
                                    wstrVersion,
                                    wstrID,
                                    pFormat);
    };
#endif
}

KLCSKCA_DECL void KLPRSS_CreateSettingsStorage(
					const std::wstring&         location,
					AVP_dword                   dwCreationFlags,
					AVP_dword                   dwAccessFlags,
					KLPRSS::SettingsStorage**   ppStorage,
                    KLPRSS::ss_server_t*        pServerId)
{
    KLPRSS_CreateSettingsStorage2(
                        location,
                        dwCreationFlags,
                        dwAccessFlags,
                        ppStorage,
                        pServerId,
                        NULL);
};
