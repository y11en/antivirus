/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	naginst.h
 * \author	Andrew Kazachkov
 * \date	21.02.2003 18:21:18
 * \brief	
 * 
 */

#ifndef __KLNAGINST_H__
#define __KLNAGINST_H__

#include <std/base/klstd.h>
#include <std/thr/locks.h>
#include <std/par/params.h>
#include <std/par/par_conv.h>
#include <nagent/naginst/nagent_const.h>
//#include <hst/hosts.h>
#include <common/reporter.h>
#include <kca/prcp/proxy_attrib.h>
#include <kca/contie/connectiontiedobjectshelper.h>
#include "common/locevents.h"
//#include "../naginst/nag_strings.h"

#include <kca/bl/businesslogic.h>
#include <nagent/connapp/conn_networklistinfo.h>



namespace KLNAG
{
    typedef std::pair<  KLPRSS::product_version_t, 
                        KLPRCI::ComponentInstanceState> pair_appstat_t;
    typedef std::vector<pair_appstat_t> vec_pair_appstat_t;

    typedef std::pair<  KLPRSS::product_version_t, KLPRCI::TaskState> pair_task_state_t;
    typedef std::pair<  std::wstring, 
                        pair_task_state_t> app_tsk_state_t;

    typedef std::list<app_tsk_state_t>      vec_app_tsk_state_t;
    typedef vec_app_tsk_state_t::iterator   it_vec_app_tsk_state_t;
    
    struct ctx_app_tsk_state_t
    {
        ctx_app_tsk_state_t()
            :   m_bFullSend(false)
        {;};
        vec_app_tsk_state_t m_vecData;
        bool                m_bFullSend;
    private://prohibit copiing
        ctx_app_tsk_state_t(const ctx_app_tsk_state_t&);
    };

    typedef std::pair<  
            KLPRSS::product_version_t, 
            KLPAR::ParAdapt >  prod2params_t;

    typedef std::vector<prod2params_t>  vec_prod2params_t;

    typedef std::pair<KLPRSS::product_version_t, long> prod2long_t;
    typedef std::vector< prod2long_t >          vec_prod2long_t;

    
    
    class KLSTD_NOVTABLE NagentHelpers
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Retrieves KLCONNAPP::NetworkListInfo interface for a 
                    specified product. Returned pointer must be returned 
                    as soon as possible. One shouldn't store it, it's 
                    better to store NagentHelpers pointer. 

          \param	szwProduct IN product name
          \param	szwVersion IN product version
          \param	ppNetworkListInfo OUT NetworkListInfo interface pointer
          \return	true if product connector is loaded, false otherwise
          \exception KLERR:::Error* in case of error
        */
        virtual bool AcquireNetworkListInfo(
                    const wchar_t*                  szwProduct,
                    const wchar_t*                  szwVersion,
                    KLCONNAPP::NetworkListInfo**    ppNetworkListInfo) = 0;
        

    /*!
      \brief	Checks if the product has connector

      \param	szwProduct IN product name
      \param	szwVersion IN product version
      \param	bHasConnector OUT true if specified product has connector
      \return	false if unknown product
      \exception KLERR:::Error* in case of error
    */
        virtual bool IfProductHasConnector(
                    const wchar_t*                  szwProduct,
                    const wchar_t*                  szwVersion,
                    bool&                           bHasConnector) = 0;

        /*!
          \brief	Returns true if nagent is reinitializing now

          \return	true if reinitializing
        */
        virtual bool IsReiniting() = 0;

        virtual KLSTD::CAutoPtr<CONTIE::ConnectionTiedObjectsHelper> 
                    GetConTiedObjectsHelper() = 0;
        
        virtual void InitiateRestart() = 0;
    };
    
    class KLSTD_NOVTABLE NagentProperties
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void get_CustomCredentials(KLPAR::Params** ppCustCred) = 0;
        virtual void get_ServerCertificate(KLPAR::BinaryValue** ppCert) = 0;
        virtual void get_HostId(std::wstring&   wstrHostId) = 0;
        virtual bool get_Reconnecting() = 0;


        /*!
          \brief	Returns connection data. Call IfWorkingOffline() 
                    method first !!!
        */
        virtual void get_ConnectionData(
                            std::wstring&           wstrServerAddress,
                            std::vector<int>&       vecServerPorts,
                            std::vector<int>&       vecServerSslPorts,
                            bool&                   bUseSsl,
                            KLPAR::Params**         ppCustCred,
                            KLPAR::BinaryValue**    ppCert,
                            KLPRCP::proxy_auth_t&   proxyHost,
                            bool&                   bCompressTraffic) = 0;
        
        /*!
          \brief	Returns connection data. Call IfWorkingOffline() 
                    method first !!!

          \param    bForCommonUse specify 'false' if need server connection 
                    data for downloading updates, specify 'true' otherwise
          \retval   ppProfile profile or NULL if using roaming profile
        */
        virtual void get_ConnectionData2(
                            bool            bForCommonUse,
                            KLPAR::Params** ppData) = 0;

        virtual void get_InstalledProducts(
                            std::vector<KLPRSS::product_version_t>& vecInstalledProducts) =0;
        virtual void get_RtpComponentFilter(
                            KLPRCI::ComponentId& idRtpComponentFilter,
                            bool&                bIsConnector) = 0;
        virtual void get_RtpStatePath(
                            std::wstring&   wstrPathRtpState) = 0;
        virtual void get_RtpState(
                            KLBL::GlobalRptState&   nLastRtpState,
                            long&                   lLastRtpStateErrorCode) = 0;
        virtual bool get_AppStates(
                            vec_pair_appstat_t&       vecAppStates,
                            bool                      bClearChaged) = 0;

        virtual void put_AppStatesChanged() = 0;
        
        virtual bool get_TskStates(
                            vec_app_tsk_state_t&    vecAppTskStates,
                            bool                    bClearChaged) = 0;

        virtual void put_TskStatesChanged() = 0;

        virtual void get_UdpPacketData(
                            std::wstring&           wstrLocation,
                            int&                    nTimeout,
                            KLPAR::BinaryValue**    ppKey) = 0;

        virtual void get_ProductsAvBasesInfo(vec_prod2params_t& vecProd2Params) = 0;

        /*!
          \brief	Returns true if network agent shouldn't make attempts 
                    to connect to the administration server. 
                    Don't try to connect to the administration server if 
                    IfWorkingOffline() returns true !!!

          \return	true if nagent is working offline
        */
        virtual bool IfWorkingOffline() = 0;
        
        //! returns number of uncured objects
        virtual void get_UncuredNumber(vec_prod2long_t& vecData) = 0;
        
        /*!
          \brief	Whether at least one ping has been sent since 
                    nagent installation and hostid is known. 
                    It's senseless to connect to the server before at 
                    least one ping has been sent since nagent installation. 

          \return	true if may connect
        */
        virtual bool MayConnect2Server() = 0;
    };
    
	class KLSTD_NOVTABLE NAginst
        :   public KLSTD::KLBaseQI
	{
	public:
		virtual void GetRunTimeInfo(
                                    KLPAR::Params* pFilter,
                                    KLPAR::Params** ppInfo) = 0;

		virtual void GetStaticInfo(
                                    KLPAR::Params* pFilter,
                                    KLPAR::Params** ppInfo) = 0;

		virtual void GetStatistics(
                                    KLPAR::Params* pFilter,
                                    KLPAR::Params** ppInfo) = 0;

		virtual long GetInstancePort(
                                    const KLPRCI::ComponentId&  id,
                                    std::wstring&               wstrLocation,
                                    long&                       lType) = 0;        

        virtual void StartProduct(
                                    const wchar_t*  szwProductName,
                                    const wchar_t*  szwProductVersion,
                                    KLPAR::Params*  pExtra,
                                    KLPAR::Params** ppResults) = 0;
        virtual void StopProduct(
                                    const wchar_t*  szwProductName,
                                    const wchar_t*  szwProductVersion,
                                    KLPAR::Params*  pExtra,
                                    KLPAR::Params** ppResults) = 0;        
        
        //! For testing purposes only !!! Do not use !!!
        virtual void StressOn(long lPar1, long lPar2) = 0;
        //! For testing purposes only !!! Do not use !!!
        virtual void StressOff() = 0;
        //! For testing purposes only !!! Do not use !!!
        virtual void GetPingCount(long& lAll, long& lSucc) = 0;
        //! For testing purposes only !!! Do not use !!!
        virtual void GetSyncCount(long& lAll, long& lSucc) = 0;
        virtual void NotifySSChange(
                        const wchar_t*  szwProduct,
					    const wchar_t*  szwVersion,
                        const wchar_t*  szwSection,
                        long            lSsType) = 0;
        virtual void RemoveStores(
                        const wchar_t**         pStores,
                        size_t                  nStores,
                        std::vector<size_t>&    vecUnremoved) = 0;
        virtual void StartNewIntegration(const wchar_t* szwHostId) = 0;
        
        virtual void GetHostPublicKey(
                        KLSTD::MemoryChunk**    pOpenKey,
                        std::wstring&           wstrHash) = 0;
        virtual void PutGlobalKeys(
                        const void* pKey1,
                        size_t      nKey1,
                        const void* pKey2,
                        size_t      nKey2) = 0;
        KLSTD_NOTHROW virtual void OnShuttingDown() throw() = 0;

        virtual void OnNotifyAddrChange() = 0;
        
        virtual bool IsProductRunning(
                        const wchar_t*  szwProduct,
					    const wchar_t*  szwVersion) = 0;
	};

    class KLSTD_NOVTABLE NaginstCallbacks
        :   public KLSTD::KLBaseQI
    {
    public:
   		virtual bool ReleaseActiveRemoteInstallationTask(const std::wstring& wstrTaskId) = 0;
        
        /*!
          \brief	Method is called before connector is being loaded

            \param	szwProduct IN product name
            \param	szwVersion IN product version
            \param	dwFlags IN connector flags (CIF_*)
            \param	pProductInfo IN product registration record
        */
        virtual void OnConnectorLoading(
                                const wchar_t*      szwProduct,
                                const wchar_t*      szwVersion,
                                AVP_dword           dwFlags, 
                                KLPAR::ParamsPtr    pProductInfo) = 0;

        /*!
          \brief	Method is called after connector was unloaded

            \param	szwProduct IN product name
            \param	szwVersion IN product version
            \param	dwFlags IN connector flags (CIF_*)
            \param	pProductInfo IN product registration record
        */
        virtual void OnConnectorUnLoaded(
                                const wchar_t*      szwProduct,
                                const wchar_t*      szwVersion,
                                AVP_dword           dwFlags, 
                                KLPAR::ParamsPtr    pProductInfo) = 0;

        /*!
          \brief	Method is called on nagent start to check pending RI tasks.
        */
		virtual void CheckIncompleteTasks() = 0;
		//TODO: add methods here

        /*!
          \brief	Method is called when user forces all syncs for the host.
        */
		virtual void OnUserForceSyncRequest(KLPAR::ParamsPtr parInfo) = 0;

        virtual void AfterChangeWithPolicy() = 0;
    };

    class KLSTD_NOVTABLE NagentCallback
    {
    public:
        virtual void MarkForRestart() = 0;
        virtual bool IsReiniting() = 0;
    };

    class KLSTD_NOVTABLE FastProductParameters
        :   public KLSTD::KLBaseQI
    {
    public:
        //! One shouldn't change put data.
        virtual void SetParameter( 
                    const wchar_t*                  szwProduct,
                    const wchar_t*                  szwVersion,
                    const wchar_t*                  szwSection,
                    const wchar_t*                  szwName,
                    KLSTD::CAutoPtr<KLPAR::Value>   pValue) = 0;

        //! One shouldn't change put data.
        virtual void SetParameters( 
                    const wchar_t*      szwProduct,
                    const wchar_t*      szwVersion,
                    const wchar_t*      szwSection,
                    KLPAR::ParamsPtr    pValues) = 0;

        //! returns specified parameter. One shouldn't change returned data.
        virtual KLSTD::CAutoPtr<KLPAR::Value> GetParameter( 
                    const wchar_t*      szwProduct,
                    const wchar_t*      szwVersion,
                    const wchar_t*      szwSection,
                    const wchar_t*      szwName) = 0;
        
        //! returns specified parameters. One shouldn't change returned data.
        virtual KLPAR::ParamsPtr GetParameters( 
                    const wchar_t*      szwProduct,
                    const wchar_t*      szwVersion,
                    const wchar_t*      szwSection,
                    const wchar_t**     pszwNames,
                    size_t              nNames) = 0;

        virtual void FlushAll() = 0;
    };    
    
    extern KLSTD::LockCount g_lckModule;
    extern volatile bool g_bShouldModifyWithPolicy;
    
    void AfterChangeWithPolicy();
};

DECLARE_MODULE_INIT_DEINIT2(KLCSNAGT_DECL, KLNAG);

KLCSNAGT_DECL void KLNAG_GetNetworkAgent(KLNAG::NAginst** ppNagent);

KLCSNAGT_DECL void KLNAG_CreateNetworkAgent(
                        KLSTD::Reporter * pReporter,
                        KLNAG::NagentCallback* pNagentCallback = NULL);

KLCSNAGT_DECL void KLNAG_DestroyNetworkAgent(bool bShuttingDown);

KLCSNAGT_DECL void KLNAG_NotifyAddrChange();

namespace KLNAG
{
    typedef enum 
    {
        nag_sf_test_gsyn = 1
    }nag_stress_flags;
};

//For debug purposes only !!!
KLCSNAGT_DECL void KLNAG_StressOn(long lPar1, long lPar2);
KLCSNAGT_DECL void KLNAG_GetPingCount(long& lAll, long& lSucc);
KLCSNAGT_DECL void KLNAG_GetSyncCount(long& lAll, long& lSucc);
KLCSNAGT_DECL void KLNAG_GetGSyncTestCount(long& lAll, long& lSucc);
KLCSNAGT_DECL void KLNAG_StressOff();

KLCSNAGT_DECL void PrepareHostForImageCopy();

KLSTD_NOTHROW AVP_dword LoadNagentFlag(const wchar_t* szName, AVP_dword dwDefaultValue) throw();
KLSTD_NOTHROW void SaveNagentFlag(const wchar_t* szName, AVP_dword dwValue) throw();
KLSTD_NOTHROW AVP_longlong LoadNagentFlag(const wchar_t* szName, AVP_longlong llDefaultValue) throw();
KLSTD_NOTHROW void SaveNagentFlag(const wchar_t* szName, AVP_longlong llValue) throw();
KLSTD_NOTHROW bool LoadNagentFlag(const wchar_t* szName, bool bDefaultValue) throw();
KLSTD_NOTHROW void SaveNagentFlag(const wchar_t* szName, bool bValue) throw();

#endif //__KLNAGINST_H__

