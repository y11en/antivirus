/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_serverimp.h
 * \author	Andrew Kazachkov
 * \date	06.05.2003 10:40:15
 * \brief	Internal definition of class CSettingsStorageServer
 * 
 */

#ifndef __KLSSSRV_SERVERIMP_H__
#define __KLSSSRV_SERVERIMP_H__

#include <std/conv/klconv.h>
#include <transport/avtl/acllib.h>
#include <map>
#include "kca/ss_srv/sssrv_server.h"

namespace KLSSS
{
    /*
        connection --> proxyid_1 --> ss1
                       proxyid_2 --> ss2
                       proxyid_3 --> ss3
    */
    
    struct ss_data_t
    {
        ss_data_t(KLPRSS::SettingsStorage* pSS, const std::wstring& wstrLocation)
            :   m_pSS(pSS)
            ,   m_wstrLocation(wstrLocation)
        {;};            
        KLSTD::CAutoPtr<KLPRSS::SettingsStorage>    m_pSS;
        const std::wstring                          m_wstrLocation;
    };

    //! maps proxyid to CAutoPtr<SettingsStorage>
    typedef std::map<std::wstring, ss_data_t> id2storage_t;

    //! maps connection name to proxyid
    typedef std::multimap<std::wstring, std::wstring> conn2id_t;

    class CSettingsStorageServer
        :   public KLSTD::KLBaseImpl<SettingsStorageServer>
        ,   public KLSTD::ModuleClass
    {
    public:
        KLSTD_CLASS_DBG_INFO(KLSSS::CSettingsStorageServer);
        CSettingsStorageServer();
        virtual ~CSettingsStorageServer();

        bool AccessCheck(
                            const wchar_t*      szwProxy,
                            const wchar_t*      szwSS,
                            const wchar_t*      szwProduct,
                            const wchar_t*      szwVersion,
                            const wchar_t*      szwSection,
                            KLPRCI::SecContext* pContext,
							AVP_dword           dwAccessMask,
							bool                bThrow);
        
        void Create(const std::wstring& wstrProduct,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrID,
                    bool                bLocalOnly);

        //SettingsStorageServer
        void OnCloseConnection(const std::wstring& wstrConnection);

        void Close(const std::wstring& wstrProxyId);

        void CreateSettingsStorage(
                    const std::wstring& wstrConnection,
                    const std::wstring& wstrLocation,
					AVP_dword           dwCreationFlags,
					AVP_dword           dwAccessFlags,
                    std::wstring&       wstrProxyId,
                    const std::wstring& wstrSSUser,
                    const KLPRSS::ss_format_t* pFormat);

        void CreateSettingsStorage(
                    const std::wstring& wstrConnection,
                    const std::wstring& wstrLocation,
					AVP_dword           dwCreationFlags,
					AVP_dword           dwAccessFlags,
                    std::wstring&       wstrProxyId,
                    const std::wstring& wstrSSUser,
                    KLPAR::Params*      pFormat);

        void Read(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection,
                    KLPAR::Params*      pExtra,
                    KLPAR::Params**     ppData);

        void Write(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection,
                    AVP_dword           dwFlags,
                    KLPAR::Params*      pData);


        void CreateSection(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection);

        void DeleteSection(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection);

        void SetTimeout(
                    const std::wstring& wstrProxyId,
                    AVP_dword           dwTimeout);

		void GetNames(
                    const std::wstring&         wstrProxyId,
                    const std::wstring&         wstrName,
                    const std::wstring&         wstrVersion,
			        std::vector<std::wstring>&	vecNames);
        
        void AttrRead(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection,
                    const std::wstring& wstrAttr,
                    KLPAR::Value**      ppValue);

        void EnumAllWSections(
                    const std::wstring& wstrProxyId,
                    KLPRSS::sections_t& sections);

        void ReadSections(
                    const std::wstring& wstrProxyId,
                    const KLPRSS::sections_t&   sections,
                    KLPAR::ArrayValue** ppData);

        void WriteSections(
                    const std::wstring& wstrProxyId,
                    const KLPRSS::sections_t&   sections,
                    KLPAR::ArrayValue*  pData);        
    protected:        
        void Storages_Add(
                    const std::wstring&         wstrId,
                    const std::wstring&         wstrConnection,
                    KLPRSS::SettingsStorage*    pSS,
                    const std::wstring&         wstrLocation);

        void Storages_Find(
                    const std::wstring&         wstrId,
                    KLPRSS::SettingsStorage**   ppSS,
                    std::wstring*               pwstrLocation=NULL);

        /*!
          \brief	Removes all storages for connection
          \param	wstrConnection - connection name
        */
        void Storages_RemoveForConection(const std::wstring& wstrConnection);

        /*!
          \brief	Removes storage by id
          \param	wstrId - id
          \param	ppSS - optional
          \return	false if not found
        */
        bool Storages_Remove(
                    const std::wstring&         wstrId,
                    KLPRSS::SettingsStorage**   ppSS);
    protected://data
        KLSTD::CAutoPtr<AVTL::AclLibrary>       m_pSecLib;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
        id2storage_t                            m_mapId2Storage;
        conn2id_t                               m_mapConn2Id;
        ;
        std::wstring m_wstrProduct, m_wstrVersion, m_wstrId;
        bool m_bLocalOnly;
        std::wstring m_wstrLocation, m_wstrTransportName;
        int m_nPort;
    };    
};

#endif //__KLSSSRV_SERVERIMP_H__

// Local Variables:
// mode: C++
// End:
