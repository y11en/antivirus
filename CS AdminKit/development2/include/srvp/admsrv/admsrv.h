/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	admsrv.h
 * \author	Andrew Kazachkov
 * \date	15.10.2003 19:57:47
 * \brief	
 * 
 */

#ifndef __KL_ADMSRV_H__
#define __KL_ADMSRV_H__

#include <std/base/klstd.h>
#include <std/par/params.h>

#include <kca/prss/settingsstorage.h>
#include <kca/prci/componentid.h>
#include <kca/prcp/componentproxy.h>
#include <transport/ev/sinks_stuff.h>
#include <common/klak_versions.h>
#include <server/srvinst/server_constants.h>

/*!
    Version encoding rule
        
        KLADMSRV_SV<VER>[_MP<MPNUM>][_FIX<FXNUM>]

        where
            VER - version number
            MPNUM - MP number
            FXNUM - CF number

*/

#define KLADMSRV_SV_50          KLAK_VER_50
#define KLADMSRV_SV_MP1         KLAK_VER_5_MP1
#define KLADMSRV_SV_MP2         KLAK_VER_5_MP2
#define KLADMSRV_SV_MP2_FIX1    KLAK_VER_5_MP2_FIX1
#define KLADMSRV_SV_MP3         KLAK_VER_5_MP3
#define KLADMSRV_SV_MP3_FIX1    KLAK_VER_5_MP3_FIX1
#define KLADMSRV_SV_MP4         KLAK_VER_5_MP4
#define KLADMSRV_SV_60          KLAK_VER_60
#define KLADMSRV_SV6_MP1        KLAK_VER_6_MP1
#define KLADMSRV_SV6_MP1_FIX1   KLAK_VER_6_MP1_FIX1
#define KLADMSRV_SV70           KLAK_VER_70
#define KLADMSRV_SV_CURRENT     KLAK_VER_CURRENT


namespace KLADMSRV
{
    
    const wchar_t c_szwSsServerPort[]=L"KLADMSRV_SSS_PORT";
    const wchar_t c_szwSsServerId[]=L"KLADMSRV_SSS_ID";
    const wchar_t c_szwSsServerProductName[]=L"KLADMSRV_PRODUCT_NAME";
    const wchar_t c_szwSsServerProductVersion[]=L"KLADMSRV_PRODUCT_VERSION";
    const wchar_t c_szwSsServerProductFullVersion[]=L"KLADMSRV_PRODUCT_FULL_VERSION";
    const wchar_t c_szwSsServerComponentVersion[]=L"KLADMSRV_COMPONENT_VERSION";
    const wchar_t c_szwSsServerHostName[]=L"KLADMSRV_SERVER_HOSTNAME";
    const wchar_t c_szwSsServerCert[]=L"KLADMSRV_SERVER_CERT";
    const wchar_t c_szwSsServerVersionId[]=L"KLADMSRV_SERVER_VERSION_ID";   //INT_T, KLADMSRV_SV_*
    const wchar_t c_szwSsServerKey[]=L"KLADMSRV_SERVER_KEY";   //BINARY_T
    const wchar_t c_szwSsServerUnderSystemAccount[]=L"KLADMSRV_SERVER_UNDER_SYSTEM_ACCOUNT";   //BOOL_T

    //! array of server addresses that can be used by clients to connect to the administration server
    const wchar_t c_szwSsServerAddresses[]=L"KLADMSRV_SERVER_ADDRESSSES";   //ARRAY_T|STRING_T

    /*!
            c_szwSsServerDbInfo - database info, contains following data 
                        (supported by any Administration Server version)
            SQL Server:
                KLDBCON_DBTYPE  STRING_T   type of db server, value KLDBCON_DBTYPE_MSSQL
                KLDBCON_HOST    STRING_T   SQL Server instance name
                KLDBCON_LOGIN   STRING_T   User name (if SQL Server auth. is used)
                KLDBCON_DB      STRING_T   DB name


            MySQL
                KLDBCON_DBTYPE  STRING_T   type of db server, value KLDBCON_DBTYPE_MYSQL - mysql                        
                KLDBCON_HOST    STRING_T   MySQL server address
                KLDBCON_PORT    INT_T      MySQL server port
                KLDBCON_LOGIN   STRING_T   User name
                KLDBCON_DB      STRING_T   DB name
    */
    const wchar_t c_szwSsServerDbInfo[] = L"KLSRV_CONNECTION_DATA"; //PARAMS_T

    const wchar_t c_szwNetworkAttrsArray[] = KLSRV_SRV_ADDRESSES_ARRAY;

    typedef KLEV::HSINKID HSINKID; // KLADMSRV::HSINKID
    typedef KLEV::AdviseEvSink AdmServerAdviseSink;

    class KLSTD_NOVTABLE AdmServerCompletion
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Returns completion percent
          \return	completion percent
        */
        virtual int GetCompletionPercent() = 0;
    };

    //! Typical ids for AdmServerAdviseSink::OnNotify
    const int c_nAdviseSinkFailure = 0; //!operation failed, see parameter pError
    const int c_nAdviseSinkOK = 1; //!operation succeeded
    const int c_nAdviseSinkCompletion = 2; //!call query interface KLADMSRV::AdmServerCompletion from parameter pResults

    struct  proxyhost_info_t
    {
        const char*     m_pszAddress;//<address>:<port>
        const char*     m_pszLogin;
        const char*     m_pszPassword;
    };

    class KLSTD_NOVTABLE AdmServer: public KLSTD::KLBaseQI
    {
    public:
        virtual void GetServerInformation(KLPAR::Params** ppServerInfo) = 0;
        virtual void AcquireServerSS(
                        const std::wstring&         wstrLocation,
				        AVP_dword                   dwCreationFlags,
				        AVP_dword                   dwAccessFlags,
                        KLPRSS::SettingsStorage**   ppSS) = 0;

        KLSTD_NOTHROW virtual bool Unadvise(HSINKID hSink) throw () = 0;

        virtual void GetComponentProxy(
                        KLPRCP::ComponentProxy** ppComponentProxy) = 0;

        virtual void Clone(KLADMSRV::AdmServer** ppClone) = 0;
        virtual void StartTaskOnServer(
                        const std::wstring&             wstrTaskname,
                        KLPAR::Params*                  pTaskData,
                        KLADMSRV::AdmServerAdviseSink*  pSink,
                        KLADMSRV::HSINKID&              hSink) = 0;

        /*!
          \brief	Запрос сертификата сервера

          \param	ppCert - указатель на переменную, в которую будет
                    записан указатель на сертификат сервера.
          \return	bool false, если сервер не поддерживает SSL
        */
        virtual bool RetrieveServerCertificate(KLPAR::BinaryValue** ppCert) = 0;
    };

    typedef std::vector<KLPRCI::ComponentId>  vec_comps_t;
    
    class KLSTD_NOVTABLE AdmServer2: public AdmServer
    {
    public:
        virtual void CreateSlaveProxy(
                    const KLADMSRV::vec_comps_t&    vecSlavePath,
                    long                            lGatewayTimeout,
                    AdmServer2**                    ppAdmServer) = 0;
    };

    class KLSTD_NOVTABLE AdmServer3: public AdmServer2
    {
    public:
        /*!
          \brief	Allows to acquire server values 

          \param	pFilter         [in]  filter, contains required values 
                                    Following values can be used: 
                                            c_szwSsServerAddresses
                                            c_szwSsServerDbInfo
          \param	ppServerInfo    [out] results 
        */
        virtual void GetServerData(
                        KLPAR::Params*  pFilter, 
                        KLPAR::Params** ppServerInfo) = 0;
    };
    
    //! interface is supported since v 6 MP1
    class KLSTD_NOVTABLE ProxyProps
        :   public  KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Checks wheter connection exist in transport

          \return	true if exists
        */
        virtual bool IsProxyValid() = 0;
    };
    

    //! checks whether administration server network attributes changed, Administration server KLADMSRV_SV70 or higher
    class KLSTD_NOVTABLE AdmSrvNetworkAttrsChanged
        :   public  KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Checks if administration server network attributes have 
                    changed since previous IfNetworkAttrsChanged call.

          \param	pOldAttributes old attributes (may be NULL)

          \retval	ppNewAttributes new attributes, contains 
                    c_szwNetworkAttrsArray as array of strings, each 
                    string is administration server address,
                    may be NULL

          \return	true if network attributes in pOldAttributes and ppNewAttributes differ,
                    returns false if pOldAttributes is NULL

          \exception throws an exception if error occured

        Sample code:

            ...
            KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSettingsStorage;
            pAdmServer->AcquireServerSS(
                            KLPRSS_MakeTypeG(KLPRSS::c_szwSST_HostSS),
                            KLSTD::CF_OPEN_EXISTING,
                            KLSTD::AF_READ|KLSTD::AF_WRITE,
                            &pSettingsStorage);

            KLPAR::ParamsPtr pNewAttributes, pOldAttributes;

            KLPRSS_MAYNOTEXIST(pSettingsStorage->Read(
                    KLCS_PRODUCT_ADMSERVER,
                    KLCS_VERSION_ADMSERVER,
                    c_szwPreviousNetworkAttributes,
                    &pOldAttributes));

            if( pAdmSrvNetworkAttrsChanged->IfNetworkAttrsChanged(
                            pOldAttributes, 
                            &pNewAttributes) )
            {
                //  notify user about attributers changed, pNewAttributes 
                //  contains new attributes, pOldAttributes contains old
                //  attributes
                ...
            };

        //save previous attributes
            if(pNewAttributes)
            {
                KLPRSS_MAYEXIST(pSettingsStorage->CreateSection(
	                    KLCS_PRODUCT_ADMSERVER,
	                    KLCS_VERSION_ADMSERVER,
	                    c_szwPreviousNetworkAttributes));

                pSettingsStorage->Clear(
	                KLCS_PRODUCT_ADMSERVER,
	                KLCS_VERSION_ADMSERVER,
	                c_szwPreviousNetworkAttributes,
	                pNewAttributes);
            }; 
        */
        virtual bool IfNetworkAttrsChanged(
                    KLPAR::Params* pOldAttributes,
                    KLPAR::Params** ppNewAttributes) = 0;
    };
}


/*!
  \brief	Возвращает указатель на прокси сервера

  \param    szwAddress - адрес сервера в виде
                http://<имя или ip-адрес>[:<порт>]
  \param	ppServer - указатель на переменную, в которую будет записан
                указатель на интерфейс KLADMSRV::AdmServer. С помощью
                метода QueryInterface у данного интерфейса можно запросить
                следующие интерфейсы:
                    KLHST::Hosts,
                    KLGRP::Groups,
                    KLPKG::Packages,
                    KLEVP::NotificationProperties,
                    KLGSYN::GroupSync,
                    KLTSK::GroupTaskControl,
                    KLPOL::Policies,
                    KLRPT::Reports,
                    KLLICSRV::SrvLicences,
                    KLEVP::EventPropertiesFactory.
  \param	pPorts  - указатель на массив портов
  \param	bUseSSL - использовать ли SSL
  \param	szwUserName     - имя пользователя для basic-аутентификации (
                флаг bUseSSL должен быть выставлен в true)
  \param	szwDomainName   - имя домена для basic-аутентификации (
                флаг bUseSSL должен быть выставлен в true)
  \param	szwPassword     - пароль для basic-аутентификации (
                флаг bUseSSL должен быть выставлен в true)
  \param	pServerCertificate  - SSL-сертификат сервера (
                флаг bUseSSL должен быть выставлен в true)

  \exception    KLPRCP::ERR_CANT_CONNECT - сервер недоступен по указанному адресу
  \exception    KLTRAP::TRERR_SSL_CONNECT_ERR - ошибка установления SSL соединения
  \exception    KLTRAP::TRERR_SSL_SERVER_AUTH_ERR - ошибка SSL аутентификации сервера
*/

KLCSSRVP_DECL void KLADMSRV_CreateAdmServer(
                const wchar_t*          szwAddress,
                KLADMSRV::AdmServer**   ppServer,
                const int*              pPorts = NULL,
                bool                    bUseSSL = true,
                const wchar_t*          szwUserName = NULL,
                const wchar_t*          szwDomainName = NULL,
                const wchar_t*          szwPassword = NULL,
                KLPAR::BinaryValue*     pServerCertificate = NULL);


KLCSSRVP_DECL void KLADMSRV_CreateAdmServer2(
                const wchar_t*                      szwAddress,
                KLADMSRV::AdmServer**               ppServer,
                const int*                          pPorts = NULL,
                bool                                bUseSSL = true,
                const wchar_t*                      szwUserName = NULL,
                const wchar_t*                      szwDomainName = NULL,
                const wchar_t*                      szwPassword = NULL,
                KLPAR::BinaryValue*                 pServerCertificate = NULL,
                const KLADMSRV::proxyhost_info_t*   pProxyHostInfo = NULL);

KLCSSRVP_DECL void KLADMSRV_CreateAdmServer3(
                const wchar_t*                      szwAddress,
                KLADMSRV::AdmServer**               ppServer,
                const KLADMSRV::vec_comps_t&        vecSlavePath,
                long                                lGatewayTimeout,
                const int*                          pPorts = NULL,
                bool                                bUseSSL = true,
                const wchar_t*                      szwUserName = NULL,
                const wchar_t*                      szwDomainName = NULL,
                const wchar_t*                      szwPassword = NULL,
                KLPAR::BinaryValue*                 pServerCertificate = NULL,
                const KLADMSRV::proxyhost_info_t*   pProxyHostInfo = NULL);

KLCSSRVP_DECL void KLADMSRV_CreateAdmServer4(
                const wchar_t*                      szwAddress,
                KLADMSRV::AdmServer**               ppServer,
                const KLADMSRV::vec_comps_t&        vecSlavePath,
                long                                lGatewayTimeout,
                const int*                          pPorts = NULL,
                bool                                bUseSSL = true,
                const wchar_t*                      szwUserName = NULL,
                const wchar_t*                      szwDomainName = NULL,
                const wchar_t*                      szwPassword = NULL,
                KLPAR::BinaryValue*                 pServerCertificate = NULL,
                const KLADMSRV::proxyhost_info_t*   pProxyHostInfo = NULL,
                bool                                bCompressTraffic = true);

/*!
  \brief	KLADMSRV_CreateConnectionDescriptor

  \param	ppDesc OUT - variable where to write
*/
KLCSSRVP_DECL void KLADMSRV_CreateConnectionDescriptor(KLSTD::KLBaseQI** ppDesc);


/*!
  \brief	KLADMSRV_ShutdownConnection

  \param	pDesc - connection descriptor
  \return	bool - true if succeded, false if connection wasn't opened
*/
KLCSSRVP_DECL bool KLADMSRV_ShutdownConnection(KLSTD::KLBaseQI* pDesc);





/************************************************************************/
/*                         OBSOLETE                                     */
/************************************************************************/

    //!obsolete dont't use these constants
    #define KLADMSRV_SV_51          KLAK_VER_51
    #define KLADMSRV_SV_52          KLAK_VER_52
    #define KLADMSRV_SV_53          KLAK_VER_53
    #define KLADMSRV_SV_54          KLAK_VER_54
    #define KLADMSRV_SV_61			KLAK_VER_61
    #define KLADMSRV_SV6_MP2		KLAK_VER_6_MP2

#endif //__KL_ADMSRV_H__
