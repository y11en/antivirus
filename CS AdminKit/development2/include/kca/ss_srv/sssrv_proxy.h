/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_proxy.h
 * \author	Andrew Kazachkov
 * \date	05.05.2003 14:56:31
 * \brief	
 * 
 */

#ifndef __KLSSSRV_PROXY_H__
#define __KLSSSRV_PROXY_H__

#include <kca/prss/settingsstorage.h>
#include <kca/prcp/proxybase.h>
#include "./sssrv_server.h"
#include "transport/wat/authserver.h"
#include "std/base/klbaseqi_imp.h"
#include "std/conv/klconv.h"

#define KLSSS_OPT_SSFORMAT_VERSION  L"KLSSS_OPT_SSFORMAT_VERSION"
#define KLSSS_OPT_SSFORMAT_ID       L"KLSSS_OPT_SSFORMAT_ID"

namespace KLSSS
{    
    const wchar_t c_szwSsServerComponent[]=L"KLSSS_COMPONENT_NAME";
   
    class KLSTD_NOVTABLE SettingsStorageProxyBase
        :   public KLPRSS::SettingsStorage
        ,   public KLPRSS::SsBulkMode
    {;};


    class SettingsStorageProxy
        :   public KLSTD::KLBaseImpl<SettingsStorageProxyBase>
        ,   public KLPRCP::CProxyBase
    {
    public:
        KLSTD_CLASS_DBG_INFO(KLSSS::SettingsStorageProxy);
        SettingsStorageProxy(const KLPRSS::ss_format_t* pFormat);
        virtual ~SettingsStorageProxy();
        ;
        KLSTD_INTERAFCE_MAP_BEGIN(KLPRSS::SettingsStorage)
            else if(IfUseLocal() && iid == KLSTD_IIDOF(KLPRSS::SsBulkMode))
            {                
                *ppObject=static_cast<KLPRSS::SsBulkMode*>(this);   \
                static_cast<KLPRSS::SsBulkMode*>(this)->AddRef(); \
            }
        KLSTD_INTERAFCE_MAP_END()
        ;
		void CreateAsCopy(
                const std::wstring&             wstrLocation,
				AVP_dword                       dwCreationFlags,
				AVP_dword                       dwAccessFlags,
                const std::wstring&             wstrID,
                KLSSS::SettingsStorageServer*   pServer,
                KLSTD::KLBase*                  pParent,
                const std::wstring&             wstrLocalComponentName,
                const std::wstring&             wstrConnName);
 
        void Create(
                const std::wstring&         wstrLocation,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags,
                const std::wstring&         wstrProduct,
                const std::wstring&         wstrVersion,
                const std::wstring&         wstrID);

        void Create(
                const KLPRCP::gateway_locations_t&  vecLocations,
                long                        lTimeout,
                bool                        bUseSSL,
                const KLPRCP::conn_attr_t*  pExtraAttributes,
                const std::wstring&         wstrLocation,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags);

        //SettingsStorage
        void Read(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
			    KLPAR::Params**     ppParams);

        void Read2(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
                KLPAR::Params*      pExtra,
			    KLPAR::Params**     ppParams);


        void Update(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
                KLPAR::Params * settings );

        void Add(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
                KLPAR::Params * settings );

        void Replace(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
                KLPAR::Params * settings );

        void Clear(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
                KLPAR::Params * settings );

        void Delete(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section,
                KLPAR::Params * settings );

        void CreateSection(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section );

        void DeleteSection(
                const wchar_t* name, 
                const wchar_t* version,
                const wchar_t* section );

        void SetTimeout( long timeout );

		void GetNames(
                const wchar_t*			name, 
                const wchar_t*			version,
                KLSTD::AKWSTRARR&       names);

		void AttrRead(
                const wchar_t*		name, 
                const wchar_t*		version,
                const wchar_t*		section,
			    const wchar_t*		attr,
			    KLPAR::Value**			ppValue) const;

        void CreateSettingsStorage(
				const wchar_t*              location,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags,
				KLPRSS::SettingsStorage**   ppStorage);

        //SsBulkMode
        void EnumAllWSections(KLPRSS::sections_t& sections);

        void ReadSections(
                const KLPRSS::sections_t&   sections,
                KLPAR::ArrayValue** ppData);

        void WriteSections(
                const KLPRSS::sections_t&   sections,
                KLPAR::ArrayValue*  pData);
    protected:
        bool IfUseLocal() const;
        void Write(
                const std::wstring& name, 
                const std::wstring& version,
                const std::wstring& section,
                AVP_dword           dwFlags,
                KLPAR::Params * settings );

        void OpenSettingsStorage(
                    const std::wstring& wstrLocation,
					AVP_dword           dwCreationFlags,
					AVP_dword           dwAccessFlags,
                    std::wstring&       wstrProxyId);

        void Close(const std::wstring& wstrProxyId);
    protected:
        KLSTD::CAutoPtr<KLPRCI::SecContext>             m_pCreator;
        KLPRSS::ss_format_t*                            m_pFormat;
        KLSTD::CAutoPtr<KLSSS::SettingsStorageServer>   m_pServer;
        std::wstring                                    m_wstrProxyId;
        std::wstring                                    m_wstrId;
        KLSTD::CAutoPtr<KLWAT::AuthServer>              m_pAuthServer;//@todo changes in wat requaired !!!
        KLSTD::CAutoPtr<KLSTD::KLBase>                  m_pParent;
    };
};

void KLSSS_CreateSettingsStorage(
                const std::wstring&         wstrLocation,
				AVP_dword                   dwCreationFlags,
				AVP_dword                   dwAccessFlags,                
                KLPRSS::SettingsStorage**   ppSS,
                const std::wstring&         wstrProduct=L"",
                const std::wstring&         wstrVersion=L"",
                const std::wstring&         wstrID=L"",
                const KLPRSS::ss_format_t*  pFormat = NULL);

void KLSSS_CreateRemoteSettingsStorage(
                KLTRAP::Transport::LocationsList&   vecLocations,
                long                                lTimeout,
                bool                                bSSL,
                const KLPRCP::conn_attr_t*          pExtraAttributes,
                const std::wstring&                 wstrLocation,
				AVP_dword                           dwCreationFlags,
				AVP_dword                           dwAccessFlags,
                KLPRSS::SettingsStorage**           ppSS,
                const KLPRSS::ss_format_t*          pFormat = NULL);

#endif //__KLSSSRV_PROXY_H__

// Local Variables:
// mode: C++
// End:
