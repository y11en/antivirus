#ifndef __SRVINST_H__
#define __SRVINST_H__

#include <server/srv_common.h>
#include <common/klaof.h>

#include <transport/tr/transporttimeout.h>
#include <transport/spl/securitypolicy.h>
#include <kca/basecomp/cmpbase.h>
#include <kca/aud/auditsource.h>
//#include <kca/contie/connectiontiedobjectshelper.h>

#include <server/srvinst/klserver.h>
#include <server/srvinst/srvinstdata.h>

#include <server/srvinst/storages.h>
#include <server/srvinst/globalbcaster.h>

#include <server/evp/transeventsource.h>
#include <srvp/evp/eventsprocessor.h>
#include <server/evp/notificationpropertiesimpl.h>
#include <srvp/pol/policies.h>
#include <server/pol/policiesimpl.h>
#include <server/tsk/commonsrvtasktools.h>
#include <server/srvinst/srchfilter.h>
#include <server/prssp/ssproxycontrolsrvimpl.h>
#include <server/srvinst/connstore.h>
#include <server/srvinst/sync_items.h>
#include <server/srvinst/srv_sync_advise.h>

#include <server/updsrv/updsrvimpl.h>
#include <server/pkg/packagesimpl.h>
#include <srvp/rpt/reports.h>
#include "../rpt/reportsimpl.h"
#include "../srvstgs/admsrvstngs_impl.h"

#include <transport/wat/authserver.h>
//#include <server/srvinst/pinghandler.h>
//#include <server/srvinst/hstunavailhandler.h>
//#include <server/srvinst/hstgrp_handler.h>
//#include <server/srvinst/gsyn_handler.h>

#include <server/srvinst/chkfreespace.h>
#include <server/srvinst/srv_certdata.h>

#include <server/srvhrch/srvhrch_impl.h>
#include <server/srvhrch/srvhrch_master.h>
#include <server/srvhrch/srvhrch_gsyn.h>

#include "srvinst/srvremotehstgrpimpl.h"
#include "srvinst/srvremotechunkaccessorimpl.h"
#include "srvinst/srvremoteadhstimpl.h"
#include "srvinst/srvremotegsynimpl.h"
#include "srvinst/srvremotessimpl.h"
#include "srvinst/srvremotepingimpl.h"
#include "srvinst/srvremotelicimpl.h"
#include "srvinst/srv_ntwrk_counters.h"
#include "../dpns/SrvRemoteScanDiapasonsImp.h"
#include "../cleaner/SrvRemoteCleanerImpl.h"
#include "srvinst/taskbackup.h"
#include "ptch/patchessyncimpl.h"
#include "ptch/SrvRemotePatchesImpl.h"
#include "inv/SrvRemoteInventoryImpl.h"

#include "srvinst/serversyncnotifier.h"
#include "hstinfo2db/hstinfo2dbimpl.h"
#include "aklwngt/aklwngtimpl.h"
#include "srvinst/srv_hstrtsyncimpl.h"
#include "srvnetscan/srvnetscanimpl.h"
#include "srvinst/srvdeferredactionsimpl.h"
#include "srvinst/srv_rules_ex.h"
//#include "srvinst/srv_admlic.h"
#include "srvinst/srv_viractindeximpl.h"
//#include "srvinst/srv_srvhrch_findimpl.h"

#include <server/srvinst/srv_settings.h>

#include <common/klstd_stlobj.h>

//#include <server/db/importado.h>

using namespace KLTMSG;

namespace KLSRV
{
    typedef KLBASECOMP::TaskBaseNoImpersonation CServerTaskBase;

    typedef std::set< KLPRSS::product_version_t > ProductSet;

    class GroupTaskControlImpl;

	enum
	{
		CMDID_STARTTASKS=KLBASECOMP::CMDID_LAST+1,
        CMDID_ONSTART
	};

    class CTimeoutObjectsControl
        :   public  KLSRV::TimeoutObjectsControl
    {
    public:
        CTimeoutObjectsControl()
        {
            KLSTD_CreateCriticalSection(&m_pCS);
        };

        void Initialize(KLSTD::KLBaseQI* pOwner)
        {
            m_pOwner=pOwner;
        }

        virtual ~CTimeoutObjectsControl()
        {
            KLSTD_ASSERT( m_setObjects.size() == 0);
        };

        KLSTD_INTERAFCE_MAP_REDIRECT(m_pOwner);

        void OnTimeoutObjectAdded(const std::wstring& wstrKey)
        {
            KLSTD::AutoCriticalSection acs(m_pCS);
            m_setObjects.insert(wstrKey);
        };

        void OnTimeoutObjectRemoved(const std::wstring& wstrKey)
        {
            KLSTD::AutoCriticalSection acs(m_pCS);
            std::multiset<std::wstring>::iterator it=m_setObjects.find(wstrKey);
            if(it != m_setObjects.end())
                m_setObjects.erase(it);
        }
        void RemoveAllElements()
        {            
            KLSTD::CAutoPtr<KLTMSG::TimeoutStore2> pTmtStore;
            KLTMSG_GetCommonTimeoutStore(&pTmtStore);
            for(;;)
            {
                std::wstring wstrKey;
                {
                    KLSTD::AutoCriticalSection acs(m_pCS);
                    std::multiset<std::wstring>::iterator it=m_setObjects.begin();
                    if(it == m_setObjects.end())
                        break;
                    wstrKey=*it;
                    m_setObjects.erase(it);
                };
                pTmtStore->Remove(wstrKey);
            };
        };
    protected:
        KLSTD::KLBaseQI*                            m_pOwner;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pCS;
        std::multiset<std::wstring>                 m_setObjects;

    };

	struct prdcomp_t{
		long m_lProductId;
		long m_lProductComponentId;
		prdcomp_t():m_lProductId(0), m_lProductComponentId(0){}
        bool operator < (const prdcomp_t& x) const
		{
			return m_lProductId < x.m_lProductId || 
				m_lProductId == x.m_lProductId && m_lProductComponentId < x.m_lProductComponentId;
		}
        bool operator == (const prdcomp_t& x) const
        {
			return m_lProductId == x.m_lProductId && m_lProductComponentId == x.m_lProductComponentId;
		}
	};
	typedef std::map<prdcomp_t, bool> vec_prdcomp_t;
	typedef vec_prdcomp_t::const_iterator   it_vec_prdcomp_t;

    #define KLSRV_CALLCTRL()    \
        CAutoObjectLock srvacc(m_lckObject);    \
        if(!srvacc) \
        {   \
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);   \
        };

	class CServerInstanceBase
		:	public  KLBASECOMP::ComponentBaseImp
        ,   public  KLSRV::SrvMainBase
		,	public  ServerInstance
		,	public  Control
        ,   public  KLWAT::CustomCredentialsConverter
        ,   public  KLSRV::ServerHelpers
        ,   public  KLSRV::SrvObsoletePointers
        ,   public  KLSRV::ServerCallbacks
        ,   public  KLTRAP::ConnectionControl
        ,   public  KLSRV::ServerSettings
        ,   public  KLSPL::SecurityPolicy::GroupsInfoControl
        ,   public  KLSRV::SrvCheckAccess
        ,   public  KLSRV::SrvSettingsChangeSink
		,	public  KLAUD::ObjectLocalization
    {;};

	class CServerInstance
        :   public KLSTD::KLBaseImpl<CServerInstanceBase>
        ,   KLSTD::ModuleClass
	{
	public:
        KLSTD_INTERAFCE_MAP_BEGIN(KLSRV::ServerInstance)
            KLSTD_INTERAFCE_MAP_AOF()
            KLSTD_INTERAFCE_MAP_ENTRY(KLWAT::CustomCredentialsConverter)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::Control)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::TimeoutObjectsControl,   m_objTmtObjects)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteHstGrp,         m_objSrvRemoteHstGrp)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::HstGrpHostStatuses,      m_objSrvRemoteHstGrp)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::HstGrpGroupChanged,      m_objSrvRemoteHstGrp)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::HstGrpHostQuery,         m_objSrvRemoteHstGrp)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteChunkAccessor,  m_objSrvRemoteChunkAccessor)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteAdHst,          m_objSrvRemoteAdHst)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteGsyn,   m_objSrvRemoteGsyn)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvGroupSync,   m_objSrvRemoteGsyn)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteSs,     m_objSrvRemoteSs)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteNagentPings, m_objSrvRemoteNagentPings)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SsCachedForHost, m_objSrvRemoteNagentPings)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvHstSyncQueue, m_objSrvRemoteNagentPings)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvSyncAdviseControl, m_objSrvRemoteNagentPings)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvSyncAdviseSink, m_objSrvRemoteNagentPings)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvForceSynch, m_objSrvRemoteNagentPings)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteLicInfo, m_objSrvRemoteLicInfo)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemotePolicy, m_objPolicies)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::PoliciesSettings, m_objPolicies)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLPOL::PoliciesInternal, m_objPolicies)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteScanDiapasons, m_objRemoteScanDiapasons)
			KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteCleaner, m_objRemoteCleaner)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::ScanMsNetwork_Counters,  m_objScanMsNetwork_Counters)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvDeferredActions,      m_objSrvDeferredActions)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::ServerHelpers)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::SrvObsoletePointers)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::ServerCallbacks)
            KLSTD_INTERAFCE_MAP_ENTRY(KLTRAP::ConnectionControl)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::ServerSettings)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSPL::SecurityPolicy::GroupsInfoControl)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::SrvCheckAccess)
			KLSTD_INTERAFCE_MAP_ENTRY(KLAUD::ObjectLocalization)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(HST2DB::HostInfo2Db,            m_objHostInfo2Db)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvHstRtSync,            m_objSrvHstRtSync)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(SRVNETGSCAN::GeneralNetScan,    m_objGeneralNetScan)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(AKLWNGT::AkLwNgt,               m_objAkLwNgt)
			KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLPTCH::PatchesSyncSrv,         m_objPatchesSyncSrv)
			KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemotePatches,        m_objPatchesSrv)
			KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteInventory,      m_objRemoteInventory)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRulesExRemote,        m_objSrvRulesEx)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLHST::SrvHstMoveRule,          m_objSrvRulesEx)
            KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLHST::SrvRulesExCallbacks,     m_objSrvRulesEx)
            //KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLSRV::SrvRemoteSrvHrchFind,    m_objSrvRemoteSrvHrchFind)
        KLSTD_INTERAFCE_MAP_END()
        
        KLSTD_CLASS_DBG_INFO(KLSRV::CServerInstance);

        AOF_DECLARE_MAP();
            
		CServerInstance();
        void Construct(KLSTD::Reporter* pReporter, AVP_dword dwFlags=0);
		virtual ~CServerInstance();
		void OnSettingsChange(
						const std::wstring&	productName, 
						const std::wstring&	version,
						const std::wstring&	section, 
						const std::wstring&	parameterName,
						const KLPAR::Value*	oldValue,
						const KLPAR::Value*	newValue);

		bool OnCommand(KLBASECOMP::command_t& cmd, bool bSync);
		bool GetStopFlag();
        void CancelInitialization();
    //
        void PublishEvent(
                        const wchar_t*          szwEventName,
                        long                    lEventId,
                        long                    lSeverity,
                        KLPAR::param_entry_t*   pPars,
                        size_t                  nPars);

        void PublishEvent(
                        long                    lEventDn,
                        const wchar_t*          szwEventName,
                        long                    lEventId,
                        long                    lSeverity,
                        const wchar_t*          szwDefFormatString,                        
                        const wchar_t*          szwPar1 = NULL,
                        const wchar_t*          szwPar2 = NULL,
                        const wchar_t*          szwPar3 = NULL,
                        const wchar_t*          szwPar4 = NULL,
                        const wchar_t*          szwPar5 = NULL,
                        const wchar_t*          szwPar6 = NULL);

    //KLTRAP::ConnectionControl
        bool CheckConnection(
                    KLTRAP::ConnectionControl::ActionType   actionType, 
				    const std::wstring&                     localName,
				    const std::wstring&                     remoteName,
				    bool                                    remoteConnection);

	//KLWAT::CustomCredentialsConverter
        void ConvertCustomCredentials(
            const KLPAR::Params*        pCustomCredetials,
            KLWAT::CustomAccessToken&   customAccessToken,
			std::wstring&               wstrUserName,
			std::wstring&               wstrUserDomain,
			std::wstring&               wstrUserPassword );

        void AdjustSSLCredentials(
			void* pPublicKey,
			size_t nPublicKeySize,
            KLWAT::CustomAccessToken& customAccessToken);

        static void OnUpdateStatisticsCallback(void* pContext, KLPAR::Params* pFilter);
	//ComponentBaseCallbacks
		void OnInitialize();
        void OnInitialize_ReadSettings();
        void ReadDynamicSettings();
		KLSTD_NOTHROW void OnDeinitialize() throw();
        void Initialize_DbSpaceChecker();
        void Initialize_CleanOldStorages();
		void OnRunTasks(
						const std::wstring&		wstrTaskName,
						KLPAR::Params*			pTaskParams,
						long					idTask,
							long					lTimeout);
	//Control
		void DoInitialize();
        void DoInitialize_RecreateListeners();
		void DoDeinitialize();
        void DoDeinitialize_DestroyListeners();
		//
		KLPRCI::ComponentInstance* GetComponentInstance(){return m_pInstance;};
//<-- Internal methods        

		int OnTransportStatusNotify(
							KLTRAP::Transport::ConnectionStatus	status, 
							const wchar_t*			remoteComponentName,
							const wchar_t*			localComponentName,
							bool					bRemote);

		//void OnPing(KLPAR::Params* pInfo, KLPAR::Params** ppResult);
		//void OnPingAsync(KLSTD::CAutoPtr<KLPAR::Params> pInfo);
        //void CheckDbConnections();
        void CheckHostStatus();
        void OnShareUnavailable(const std::wstring& wstrShareName);
		void DoDeliverReport(						
			const std::wstring& wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			const std::wstring& wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult);
		void OnDeliverReport(						
			const std::wstring& wstrRemoteConnectionName,
			const std::wstring& wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			const std::wstring& wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult);
		void DoNotifyOnReportResult(
			std::wstring wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams);
		void OnNotifyOnReportResult(
			const std::wstring& wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams);

        struct pinging_host_t
        {
            pinging_host_t()
                :   m_lHost(0)
                ,   m_dwFlags(0)
                ,   m_bForce(false)
            {
                ;
            };
            long                            m_lHost;
            std::wstring                    m_wstrNagent;
            std::wstring                    m_wstrHostId;
            AVP_dword                       m_dwFlags;
            KLSTD::CAutoPtr<KLPAR::Params>  m_pData;
            bool                            m_bForce;
        };

        void OnPingResolveAndApply(
                    KLSTD::CAutoPtr<KLPAR::Params>              pInfo, 
                    KLSRV::NagentConnection**                   ppConn,
                    pinging_host_t&                             data,
                    bool&                                       bNeedComputerName);

        /*
        void SS_Prepare(
                    long                lHostId,
                    const std::wstring& wstrHostName,
                    AVP_dword           dwFlags);

        void SS_UnPrepare(
                    long                    lHostId,
                    const std::wstring&     wstrName,
                    AVP_dword               dwFlags);
        */


        long RegisterKey(   KLDB::DbConnectionPtr   pCon, 
                            KLPAR::Params*          pKeyData);
        
        /*
        void UpdateProductStates(long lHost, KLPAR::Params* pData);

        void UpdateHostMacAddresses(long lHost, KLPAR::Params* pData);
        */

        void UpdateLicenseInfo(
                    KLDB::DbConnectionPtr   pCon,
                    const std::wstring& wstrHost,
					long				idHost,
                    KLPAR::Params*      pData,
					const std::wstring& wstrID);
        
        void UpdateOsInfo(
                    KLDB::DbConnectionPtr   pCon,
                    long                    lHostId,
                    const std::wstring&     wstrHost);

        void UpdateProductComponentsInfo(
            KLDB::DbConnectionPtr pCon,
			const std::wstring& wstrHost,
            long nHost,
            KLPAR::Params* pSyncData);
        
        void UpdateProductFixesInfo(
            KLDB::DbConnectionPtr pCon,
			const std::wstring& wstrHost,
            long nHost,
            KLPAR::Params* pSyncData);
		
        /*
        void SetHostRunningProducts(
                        long                lHost,
                        KLPAR::ArrayValue*  p_arrData);

        void GetHostRunningProducts(
                        const std::wstring& wstrHostId,
                        KLPAR::Params**     ppData);
        void GetHostInstalledProducts(
                        const std::wstring& wstrHostId,
                        std::list<KLHST::product_info_t>& products);
        */
       //! Initializing
		void DB_OnInitialize();
		KLSTD_NOTHROW void DB_OnDeinitialize() throw();
        /*!
          \brief	AcquireConnection

          \param	wstrInstance
          \param	lConnectionTimeout - 0 if use only existing connection
          \param	wstrAddress may be empty string if lConnectionTimeout is 0
          \param	ppConn
          \return	true, if success
        */
        bool AcquireConnection(
                            KLDB::DbConnectionPtr   pCon,
                            const std::wstring&		wstrHostId,
                            NagentConnection**      ppConn,
                            AVP_dword               dwFlags=ACF_DEFAULT);

        bool AcquireConnection2(
                            const std::wstring&     wstrInstance,
                            const std::wstring&     wstrAddress,
                            NagentConnection**      ppConn,
                            AVP_dword               dwFlags=ACF_DEFAULT);

        bool GetHostDisplayName(long lHost, std::wstring& wstrDisplayName);

        bool AcquireSlaveConnection(
                            const std::wstring&     wstrInstance,
                            const std::wstring&     wstrAddress,
                            ConnectionInfo**        ppConn,
                            AVP_dword               dwFlags=ACF_DEFAULT);

		void GetSlaveStaticInfo(
							KLSTD::CAutoPtr<KLSRV::ConnectionInfo>	pConnectionInfo, 
							KLPAR::Params*      pFilter, 
							KLPAR::Params**     ppData);
		
        long GetSuperGroupId();
        
        void InitiateHostSynForGsyn(long lGsyn, AVP_dword dwFlags);
        bool GetWinNames(
                        KLDB::DbConnectionPtr pCon,
                        const std::wstring& wstrHostsId,
                        std::wstring&       wstrWinHostName,
                        std::wstring&       wstrWinDomainName,
                        KLHST::DomainType&  nWinDomainType);

        bool GetHostDisplayName(
                                KLDB::DbConnectionPtr pCon,
                                long lHost, 
                                std::wstring& wstrDisplayName);
        void InitiateHostSynForGsyn(
                                KLDB::DbConnectionPtr pCon,
                                long lGsyn, 
                                AVP_dword dwFlags);

        long GetGsynHostsCount(long lSync, bool bExcludeUnavailable);
        std::wstring MakeGroupSyncPath(const std::wstring& wstrType);
    //ServerSettings
        long get_TrRemoteUnicastTime();
        long get_VisibilityPeriod();
        void get_UdpSettings(int& nPort, int& nCount);
        ;
        KLSTD::CAutoPtr<KLPAR::Value> GetParameter(
                            const wchar_t* szwName,
                            const wchar_t* szwSection = KLSRV_COMMON_SETTINGS,
                            const wchar_t* szwProduct = KLCS_PRODUCT_ADMSERVER, 
                            const wchar_t* szwVersion = KLCS_VERSION_ADMSERVER);
        
        KLPAR::ParamsPtr GetParameters(
                            const wchar_t** szwNames,
                            size_t          nNames,
                            const wchar_t*  szwSection = KLSRV_COMMON_SETTINGS,
                            const wchar_t*  szwProduct = KLCS_PRODUCT_ADMSERVER, 
                            const wchar_t*  szwVersion = KLCS_VERSION_ADMSERVER);
        ;
        KLSRV::HCHANGESINK Advise(
                            KLSRV::SrvSettingsChangeSink *pSink,
                            const wchar_t* szwSection = KLSRV_COMMON_SETTINGS,
                            const wchar_t* szwProduct = KLCS_PRODUCT_ADMSERVER, 
                            const wchar_t* szwVersion = KLCS_VERSION_ADMSERVER);

        void Unadvise(KLSRV::HCHANGESINK hSink);
    //KLSPL::SecurityPolicy::GroupsInfoControl
		void GetGroupHierarchy(
            const long&                             groupId, 
			KLSPL::SecurityPolicy::GroupHierarchy&  groupHierarchy );
		void GetNagentGroups(
            const std::wstring&                     nagentId,
			KLSPL::SecurityPolicy::GroupHierarchy&  groupHierarchy );
		void GetChildServersGroup( 
			const std::wstring&						serverId,
			KLSPL::SecurityPolicy::GroupHierarchy&  groupHierarchy );
		void GetHostGroups( const std::wstring& wstrHostName,
			KLSPL::SecurityPolicy::GroupHierarchy &groupHierarchy );
    //SrvSettingsChangeSink
        void OnNotify(
                    const wchar_t* szwProduct,
                    const wchar_t* szwVersion,
                    const wchar_t* szwSection);
    //SrvCheckAccess
        bool CheckAccessInCallForGroup(
	                    AVP_dword   dwActionGroupID,
	                    AVP_dword   dwAccessMasks,
			            long	    lGroupId,
	                    bool        bGenerateException,
                        const std::wstring* pwstrBasicAuthLogin);

        bool CheckAccessInCallForHost(
	                    AVP_dword       dwActionGroupID,
	                    AVP_dword       dwAccessMasks,
                        const wchar_t*  szwHostId,
	                    bool            bGenerateException,
                        const std::wstring* pwstrBasicAuthLogin);        
        
        bool CheckAccessInCallForGroupParent(
	                    AVP_dword   dwActionGroupID,
	                    AVP_dword   dwAccessMasks,
			            long	    lGroupId,
	                    bool        bGenerateException,
                        const std::wstring* pwstrBasicAuthLogin);

	//KLAUD::ObjectLocalization
		std::wstring LocalizeProductName( const std::wstring &productName );
        
	//<-- Internal DB Helpers
        void MakeBroadcastLocations(
					KLDB::DbConnectionPtr pDbConn, 
					const std::vector<std::wstring>& vecIds,
                    BcastLocs&					vecLocations,
					std::vector<std::wstring>*  pVecLocatedIds = NULL,
                    std::vector<std::wstring>*  pVecFailedIds = NULL);

		void FillLocationEx(
					KLTRAP::Transport::LocationEx&  loc,
					const std::wstring&             wstrLocation,
					const std::wstring&             wstrNagentInstaceId,
					const std::wstring&             wstrServerTrName,
					long                            idHost);

        void GetLocalHostId(std::wstring& wstrHostId);

        void GetLocalNagentInstance(std::wstring& wstrInstance);

        void GetLocation(std::wstring& wstrLocation);

    //ServerHelpers
        long FindHost(  KLDB::DbConnectionPtr pCon,
                        const std::wstring& strHostName);
        
        virtual bool Cache_AcquireHostInfo( 
                                KLDB::DbConnectionPtr   pCon,
                                long                    lHost,
                                host_info_t&            hi);

        //! returns -1L if not found
        virtual long Cache_FindHost(
                                KLDB::DbConnectionPtr   pCon,
                                const std::wstring&     wstrHostName,
                                host_info_t*            pHi);

        virtual void Cache_UpdateHostInfo(
                                KLDB::DbConnectionPtr   pCon,
                                long                    lHostId,
                                const host_info_t&      hi);

        virtual void Cache_ClearHostInfo(
                                KLDB::DbConnectionPtr       pCon,
                                long                        lHost);
    
        bool IsSsChanged(
                    const std::wstring& wstrHost, 
                    const std::wstring& wstrType);

        bool GetWinNames(
                        const std::wstring& wstrHostsId,
                        std::wstring&       wstrWinHostName,
                        std::wstring&       wstrWinDomainName,
                        KLHST::DomainType&  nWinDomainType);

		long FindHost(const wchar_t* szwHost);

        //OBSOLETE
        std::wstring GetHostNagentId(const std::wstring& wstrHostId);

        //OBSOLETE
        bool CheckHostNAgentId(
                        const std::wstring& wstrHostsId,
                        const std::wstring& wstrNAgentId) throw();

        bool CheckHostNAgentId(
                        KLDB::DbConnectionPtr pCon,
                        const std::wstring& wstrHostsId,
                        const std::wstring& wstrNAgentId);

        std::wstring GetHostNagentId(
                            KLDB::DbConnectionPtr pCon,
                            const std::wstring& wstrHostId);
        
        void GetGroupTaskControlSrv(
                        KLTSK::GroupTaskControlSrv** ppGtc );

        void get_ConnectionsCnt(long& lCnt, long& lNagCnt);

        long GetProductId(  KLDB::DbConnectionPtr   pCon,
                            const wchar_t*          szwProduct,
                            const wchar_t*          szwVersion);

        long GetProductDnId(KLDB::DbConnectionPtr   pCon,
                            const wchar_t*          szwDisplayName);

        std::wstring GetGrpTasksRootPath();
        
        std::wstring AcquireHostSsLocation(
                            const std::wstring& wstrHostId,
                            const std::wstring& wstrType);

		KLSTD::CAutoPtr<KLSRVH::SrvHierarchyMaster>	GetMasterServerProxy()
		{
			return m_pMasterServerProxy;
		}
        
        std::wstring AcquireSuperGsynPath(const std::wstring& wstrType);
        
        virtual std::wstring AcquireServerSyncId();

		virtual void GetInstalledProducts(
			KLDB::DbConnectionPtr pDbConn, 
			KLSRV::UpdProdVerMap& vecProdVers,
			KLSRV::UpdCompVerMap& vecCompVers,
			bool bIncludeSlaveServers,
			bool bIncludeComponentVersions,
			bool bProcessAdditional);
	//internal
        std::wstring AcquireSuperGsynPathI(
                            const std::wstring&     wstrType, 
                            std::wstring*           pwstrRelPath,
                            KLSRVH_GSYN_TYPE*       pnType,
                            KLSTD::MemoryChunk**    ppDigest);
    //KLSRV::SrvObsoletePointers
        KLBASECOMP::ComponentBase* GetComponentBase();
        KLEVP::TransEventSource* GetTransEventSource();
        KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> GetEventsProcessor();
		KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> GetGroupTaskControlSrv();		
		KLSTD::CAutoPtr<KLSRV::ReportsSrv> GetReportsSrv();		

        //
        //void WatchdogPing();

        /*
        void GetChildComputers(
                    long            idOU, 
                    const wchar_t** pFields,
                    size_t          nFields,
                    long            lTimeout,
                    std::wstring&   wstrIterator,
                    const std::wstring& wstrLocalName,
                    const std::wstring& wstrRemoteName);

        void GetChildOUs(
                    long            idOU, 
                    const wchar_t** pFields,
                    size_t          nFields,
                    long            lTimeout,
                    std::wstring&   wstrIterator,
                    const std::wstring& wstrLocalName,
                    const std::wstring& wstrRemoteName);
        */
        
        //void OnInstallerNotification(
                    //KLPAR::Params*  pData,
                    //KLPAR::Params** ppResult);
        
        void RemoveStores(
                        const wchar_t**         pStores,
                        size_t                  nStores,
                        std::vector<size_t>&    vecUnremoved);

        void RemoveStoresNoCS(
                        KLDB::DbConnectionPtr pCon,
                        const wchar_t**         pStores,
                        size_t                  nStores,
                        std::vector<size_t>&    vecUnremoved);

        void StartNewIntegration(const wchar_t* unused);
        
        void GetServerPublicKey(
                        KLSTD::MemoryChunk**    pOpenKey,
                        std::wstring&           wstrHash);

        void PutMasterGlobalKeys(
                        const void* pKey1,
                        size_t      nKey1,
                        const void* pKey2,
                        size_t      nKey2);

        void OnUnicastArrived(std::wstring  wstrRemoteComponentName);

		KLSTD::CAutoPtr<KLPAR::Params> ReadMasterServerParamsFromSS();
		
        //! uses current transaction
        void SetInstallAntivirusSoftware(
                        long                lIdGroup,
                        KLPAR::ArrayValue*  pPackages);

        //! uses current transaction
        void ResetInstallAntivirusSoftware(long lIdGroup);
        void RemoveFromAutoInstallAV(long lIdGroup, std::set<long>& setPackages);
        void AddToAutoInstallAV(long lIdGroup, std::set<long>& setPackages);

        void InsertAutoInstallRecord(
                        long    idGroup,
                        long    idPackage,
                        long    idGroupTask);        
        
	//--> Internal DB Helpers
        KLSTD_NOTHROW void OnAppHostChanged() throw();

        KLSTD_NOTHROW void OnGsyncForHostMade(
                    KLDB::DbConnectionPtr           pCon,
                    long                            lSyncId,
                    long                            idHost,
                    const std::wstring&             wstrType,
                    const std::wstring&             wstrLocation,
                    bool                            bSuccess,
                    KLERR::Error*                   pErrorDesc,
                    const KLSTD::precise_time_t&    tmSyncStart) throw();       
        
        KLSTD_NOTHROW void OnGsyncForHostMade(
                    long                            lSyncId,
                    long                            idHost,
                    const std::wstring&             wstrType,
                    const std::wstring&             wstrLocation,
                    bool                            bSuccess,
                    KLERR::Error*                   pErrorDesc,
                    const KLSTD::precise_time_t&    tmSyncStart) throw();
        
        /*!
          \brief	Method is called every time when...
                        - host group is changed
                        - host is added to group
                        - host is deleted
                    It is guaranteed not to be called in context of any db
                    transaction.
          \param	lHostId - id of host
          \exception method should not throw any exceptions
        */
        KLSTD_NOTHROW void OnHostMoved(
                KLDB::DbConnectionPtr       pCon,
                long                        lHostId);

        /*!
          \brief	This method is called when group sync from the master 
                    server must be added or updated.

          \param	pCon            db connection
          \param	nType           gsyn type
          \param	wstrFullPath    gsyn fs path
          \param	wstrType        gsyn full type
        */
        void OnGsyncFromMasterAdd(
                            KLDB::DbConnectionPtr   pCon,
                            KLSRVH_GSYN_TYPE        nType,
                            const std::wstring&     wstrFullPath,
                            const std::wstring&     wstrType);

        /*!
          \brief	This method is called when group sync from the master 
                    server must be deleted.

          \param	pCon            db connection
          \param	nType           gsyn type
          \param	wstrFullPath    gsyn fs path
          \param	wstrType        gsyn full type
        */
        void OnGsyncFromMasterDelete(
                            KLDB::DbConnectionPtr   pCon,
                            KLSRVH_GSYN_TYPE        nType,
                            const std::wstring&     wstrFullPath,
                            const std::wstring&     wstrType);

        /*!
          \brief	OnSuperPolicyArrived. If there's no such policy
                    function must create it. If policy already exists
                    function must must rewrite it.

          \param	wstrPath [in] - full filename of arrived data
          \param	wstrSsType [in] - ss type
        */
        void OnSuperPolicyArrived(  
                            KLDB::DbConnectionPtr pCon,
                            const   std::wstring& wstrPath,
                            const   std::wstring& wstrSsType);

        /*!
          \brief	OnSuperGtaskArrived. If there's no such group task
                    function must create it. If group task already exists
                    function must must rewrite it.

          \param	wstrPath [in] - full filename of arrived data
          \param	wstrSsType [in] - ss type
        */
        void OnSuperGtaskArrived(   
                                    KLDB::DbConnectionPtr pCon,
                                    const   std::wstring& wstrPath,
                                    const   std::wstring& wstrSsType);

        /*!
          \brief	OnSuperPolicyDeleting. If there's no such policy
                    function must simply return control WITHOUT 
                    throwing any exception.

          \param	wstrPath [in] - full filename of arrived data
          \param	wstrSsType [in] - ss type
        */
        void OnSuperPolicyDeleting( 
                                    KLDB::DbConnectionPtr pCon,
                                    const   std::wstring& wstrPath,
                                    const   std::wstring& wstrSsType);

        /*!
          \brief	OnSuperGtaskDeleting. If there's no such group task
                    function must simply return control WITHOUT
                    throwing any exception.
          \param	wstrPath [in] - full filename of arrived data
          \param	wstrSsType [in] - ss type
        */
        void OnSuperGtaskDeleting( 
                                    KLDB::DbConnectionPtr pCon,
                                    const   std::wstring& wstrPath,
                                    const   std::wstring& wstrSsType);

        virtual void OnAdmGroupChanged(
            KLDB::DbConnectionPtr       pCon,
            const AdmGrpChanged*        pData,
            size_t                      nData);

        virtual void OnHostsVisibilityChanged(
                        const long* plHosts,
                        size_t      nHosts);

		virtual void GetApplyPatches(KLDB::DbConnectionPtr pDbConn, bool& bSrv, bool& bNagt);

//--> Internal methods

//<-- ServerInstance
        Control* GetControl(){return static_cast<KLSRV::Control*>(this);};
        
        void GetRunTimeInfo(KLPAR::Params* pFilter, KLPAR::Params** ppData);
        void GetStaticInfo(KLPAR::Params* pFilter, KLPAR::Params** ppData);
		;
        void GetUpdatesInfo(KLPAR::Params* pFilter, KLPAR::Params** ppData);

        void SetBundleOptions( KLPAR::Params* pOptions );

        void SetGeneralOptions( KLPAR::Params* pOptions );

        void GetGeneralOptions( KLPAR::Params** ppOptions );

		void GetAvailableUpdatesInfo(
			const wchar_t*      szwLocalization,
			KLPAR::Params** ppAvailableUpdateComps,
			KLPAR::Params** ppAvailableUpdateApps);

		void GetRequiedUpdatesInfo(
			KLPAR::Params** ppInstalledComps,
			KLPAR::Params** ppInstalledApps);
		
        /* void GetUpdatesOptions(KLUPDSRV::Options & options);

        void SetUpdatesOptions(const KLUPDSRV::Options & options); */

        // <!-- Updater stuff:
        void InitializeUpdaterStuff();

        void DeinitializeUpdaterStuff();

        void GetOveralSystemProducts( std::vector<KLPRSS::product_version_t> & vectProducts );

        std::wstring GetLastRecordedUpdateTimestamp(
            KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSettingsStorage,
            KLPAR::Params** ppReadSection = NULL );

        void  GetAutoInstallAVSSection(
            KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSettingsStorage,
            KLPAR::Params** ppReadSection );

        void ForceUpdate();

        KLEV::SubscriptionId m_subscrIdOnFinishUpdate;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSUpdater;

// --> ServerInstance
        void CreateSuperGsynSs(
                    KLDB::DbConnectionPtr pCon,
                    const wchar_t*      szwSsSrc,
                    const wchar_t*      szwSsDst);

        /*!
          \brief	RemoveSuperGsyns

          \param	bAll    IN  removes ALL super gsyncs if true, deletes only undeleted earlier otherwise
        */
        KLSTD_NOTHROW void RemoveSuperGsyns(bool bAll) throw();
    private:
        const std::wstring  m_wstrLocalHostId;
	public:
		//settings
        //bool            m_bDpnsEnabled;
        volatile long m_lVisibilityPeriod;
        const std::wstring  m_wstrLocalNagent;
        volatile        m_bCancelInit;
        const std::wstring  m_wstrServerId;
        std::wstring    m_wstrLocPath;
        long            m_lTrRemoteConnectTime,m_lTrRemoteSndRcvTime, m_lTrRemoteUnicastTime;
        long            m_lTrLocalConnectTime,m_lTrLocalSndRcvTime;
		long			m_lSyncLifeTime;
		long			m_lSyncLockTime;
		long			m_lSyncSecPacketSize;
        const std::wstring  m_wstrGSyncRoot;//Root folder for group syncs files
        const std::wstring  m_wstrStoresRoot;//Root folder for synchronized stores
        const std::wstring  m_wstrGsynStoresSuper;
        //long          m_lFastUpdateNetPeriod, m_lFullUpdateNetPeriod, m_lDpnsPeriod;
	private:
		KLPRCI::ComponentId m_SlaveComponentId;
	public:
        KLSTD::Reporter*							m_pReporter;
        KLPAR::ParamsPtr                            m_pDbConnectionInfo;
        const std::wstring                          m_wstrDatabaseName;
        const std::wstring                          m_wstrInstanceName;
        const std::wstring                          m_wstrProductFullVersion;

        ;
        KLDB::DbConnStoragePtr      m_pDbConnStorage;
        KLSTD::CPointer<SrvData>&   m_pSrvData;
        ;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pDynamicDataCS;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pListenersCS;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pSuperGsyncsCS;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>		m_pCachesCS;
        ;		
        void CreateSchema( const std::wstring& wstrServerInstance,
            const std::wstring& wstrDatabaseName );
        
        void MakeGsyncForHost(
                            const std::wstring& wstrLocal,
                            const std::wstring& wstrRemote,
                            long                idHost,
			                const std::wstring& wstrLocation,
                            const std::wstring& wstrType,
                            long                SyncId);
        KLSTD_NOTHROW void ReportFailureGsyncForHost(
                            KLERR::Error* pError,
                            long          idHost,
                            long          SyncId) throw();
        KLSTD_NOTHROW void ReportFailureSync(
                            const std::wstring& wstrSStype,
                            const std::wstring& wstrHostId,
                            long                idHost,
                            KLERR::Error*       pError) throw();
	public:
        void GetNotificationProperties(KLSRV::NotificationPropertiesImpl** ppNotificationPropertiesImpl);
		void GetReports(KLSRV::ReportsImpl** ppReports);

        CommonSrvGroupTaskTools                             m_commonSrvTaskTools;

	protected:
        //state flags
        int     m_bDbUnavailReported:1;
        KLSTD::CAutoPtr< CONTIE::ConnectionTiedObjectsHelper>       m_pTiedObjects;
        //KLSTD::CAutoPtr< KLPOL::Policies>                   m_pPolicies;
        KLSTD::CAutoPtr<KLPKG::PackagesSrv>                 m_pPackages;
        CTimeoutObjectsControl                              m_objTmtObjects;
        SrvRemoteHstGrpImpl                                 m_objSrvRemoteHstGrp;
        SrvRemoteChunkAccessorImpl                          m_objSrvRemoteChunkAccessor;
        SrvRemoteAdHstImpl                                  m_objSrvRemoteAdHst;
        KLGSYNSRV::SrvRemoteGsynImpl                        m_objSrvRemoteGsyn;
        SrvRemoteSsImpl                                     m_objSrvRemoteSs;
        SrvRemoteNagentPingsImpl                            m_objSrvRemoteNagentPings;
        SrvRemoteLicInfoImpl                                m_objSrvRemoteLicInfo;
        KLSRVPOL::PoliciesImpl                              m_objPolicies;
        SrvRemoteScanDiapasonsImpl                          m_objRemoteScanDiapasons;
		SrvRemoteCleanerImpl								m_objRemoteCleaner;
		PatchesSyncSrvImpl									m_objPatchesSyncSrv;
		SrvRemotePatchesImpl								m_objPatchesSrv;
		SrvRemoteInventoryImpl								m_objRemoteInventory;
        SrvRulesExImpl                                      m_objSrvRulesEx;
        ScanMsNetwork_CountersImpl                          m_objScanMsNetwork_Counters;
        KLSRV::SrvDeferredActionsImpl                       m_objSrvDeferredActions;
        HST2DB::CHostInfo2DbImpl                            m_objHostInfo2Db;
        AKLWNGT::CAkLwNgtImpl                               m_objAkLwNgt;
        HSTRTSYN::SrvHstRtSyncImpl                          m_objSrvHstRtSync;
        SRVNETGSCAN::CGeneralNetScanImpl                    m_objGeneralNetScan;
        KLRPTVIRACT::SrvViractIndexImpl                     m_objSrvViractIndex;
        //KLSRVHRCHFIND::SrvRemoteSrvHrchFindImpl             m_objSrvRemoteSrvHrchFind;
        
        CNagentConnectionsStorage                           m_Connections;
        CConnectionsInfoStorage                             m_SlaveServersConnectionsStrg;
		KLSTD::CAutoPtr<KLSRV::ConnectionInfo>				m_pMasterServerConnection;
		KLSTD::CAutoPtr<KLSRVH::SrvHierarchyMaster>			m_pMasterServerProxy;
        CSyncItems                                          m_SyncItems;
        //KLFLT::str2str_map_t					    	    m_mapGrpFindHostsFilter;
        KLFLT::str2str_map_t					    	    m_mapHst;
		//KLFLT::str2str_map_t								m_mapKeyInfoType;
		AVP_dword									        m_dwFlags;
		KLSTD::CAutoPtr<TimeoutStore2>				        m_pTimeoutStore;
        KLSTD::CPointer<CStorages>                          m_pStorages;
//		volatile long								        m_lThreads;
		KLSTD::CAutoPtr<KLEVP::TransEventSource>	        m_pTransEventSource;
        KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv>          m_pEventsProcessor;
        KLSTD::CAutoPtr<NotificationPropertiesImpl>         m_pNotificationPropertiesImpl;
		KLSTD::CAutoPtr<KLSRV::ReportsImpl>					m_pReportsImpl;
        KLSTD::CAutoPtr<KLSRV::GroupTaskControlImpl>		m_pGroupTaskControlImpl;
        KLSTD::CAutoPtr<KLSRVH::SrvHierarchyImpl >			m_pSrvHierarchyImpl;
		KLSTD::CAutoPtr<KLSRVS::AdmServerSettingsImpl>		m_pAdmServerSettingsImpl;
		
		KLSTD::CAutoPtr<ServerSyncNotifier>					m_pServerSyncNotifier;
		

        //KLSTD::CAutoPtr<KLEVP::EventsToServer>              m_pEventsToServer;
        //KLTP::ThreadsPool::WorkerId				          m_idEventsToServer;
        int RunWorker(KLTP::ThreadsPool::WorkerId wId);
        ;

        std::vector< KLSTD::KLAdapt<KLSTD::CAutoPtr<KLSRV::Deinitializer> > > m_vectClassesToDeinitialize;
        
        int                 m_nOpenedUdpPort;
        int                 m_nUdpPort;
        std::vector<int>    m_vecPorts, m_vecSslPorts, m_vecLwNgtPorts;
        long                m_nLwNgtThreads;
        bool                m_bOpenLwNgtPort;
        std::wstring        m_wstrSelfDomainName;
        std::wstring        m_wstrSelfHostName;

        KLUPDSRV::UpdateSrvImpl m_updateSrvImpl;
        void SetOrderToLoad(KLDB::DbConnectionPtr pConnection, KLPAR::Params* pOrders );
        void InitializePushUpdates();

		KLSTD::CAutoPtr<PackagesImpl> InitializePackages();
        void InitializeGroupTasks( KLSTD::CAutoPtr<PackagesImpl> pPackagesImpl );

		void InitializeEventProcessing();
        
        KLSTD::CAutoPtr<KLPRSSP::SSProxyControlSrv> InitializePRSSProxy();
        //KLSTD::CAutoPtr< KLSTD::KLBaseImpl<KLSRV::PoliciesImpl> > InitializePolicies( KLPRSSP::SSProxyControlSrv * pSSProxyControlSrv );
		KLSTD::CAutoPtr<KLSRV::ReportsSrv> InitializeReports(KLDB::DbConnectionPtr pDbConnection);

		KLSTD::CAutoPtr<KLSRVH::SrvHierarchySrv> InitializeSrvHierarchy();
	    KLSTD::CAutoPtr<KLSRVS::AdmServerSettingsImpl> InitializeAdmServerSettings();

        void CheckSettings();
        void CheckPackagesSettings();
        void CheckSharedFolder( KLPRSS::SettingsStorage* pSettingsStorage );

        void ResolveExistingNetworkSharePath(
            const std::wstring& wstrNetworkSharePath,
            KLPRSS::SettingsStorage* pSettingsStorage );

        void CreateNewShare(
            const std::wstring& wstrLocalSharePath,
            const std::wstring& wstrNewShareName,
            KLPRSS::SettingsStorage* pSettingsStorage );

        void CheckGroupTasksSettings();
        
        void CheckUpdaterSettings();

        void OnUpdateStatistics(KLPAR::Params* pFilter);
        
        void RereadHostStatusAsync();
		
		void OnMasterServerDisconnect(KLPRCI::ComponentId compoentId, KLSTD::precise_time_t t);

		void RereadMasterServerDataAndConnect();

		void CheckMasterServerConnection();

        void CheckSlaves();

		void DeleteMasterSrvConnChecker();

        std::wstring GetClientStoreLocation(const std::wstring& wstrType);
        
        void OnFinishedGsync(const std::wstring& wstrType);

        KLSTD_NOTHROW void OnDeletingGsyncNoCS(
                KLDB::DbConnectionPtr pCon,
                const std::wstring& wstrType) throw();

        KLSTD_NOTHROW void DoCleanupUndeletedSuperGsyncs() throw();

        void OnInitialPingMaster(
                        KLDB::DbConnectionPtr pCon,
                        const std::wstring& wstrPublicKeyHash,
						const std::wstring& wstrVersion,
                        const std::wstring& wstrInstanceId,
                        KLSTD::CAutoPtr<KLPAR::Params> pParams);

        bool IsSlaveServerConnected(const std::wstring& wstrSrvInstanceId);

        void CloseSlaveServerConnection(const std::wstring& wstrSrvInstanceId);

		KLSTD::CAutoPtr<KLPAR::BinaryValue> GetServerCertificate();
		
        void SynchronizeHosts(KLDB::DbConnectionPtr pCon);
    protected:
        void ResolveMe();
		bool AdjustMasterSrvConnection(KLSTD::CAutoPtr<KLPAR::Params> pMasterSrvData);
		
		void ConnectToMasterSrv(
			const std::wstring& wstrMasterSrvAddress,
			std::vector<int> vecMasterSrvPorts,
			const KLPRCI::ComponentId& idMasterSrv,
			KLSTD::CAutoPtr<KLPAR::BinaryValue> pMasterSrvCert,
			const KLPRCP::proxy_auth_t& proxyHost,
			bool bIsGateway,
			bool bMainConnection,
			bool bCompressTraffic);

        //cached data
        KLSTD::hmap_str2long_t  m_mapProd2id;
        KLSTD::hmap_str2long_t  m_mapProdDn2id;
        const std::wstring  m_wstrTrLocalName;
        const std::wstring  m_wstrExtraListnerTrName;
        const std::wstring  m_wstrTrLocalNameAsSlave;
        const long          m_lMaxConnections;
        const long          m_lPingSsWait, m_lPingLimit;
        const bool          m_bSrvForceConnClose;
        const long          m_lSuperGroup;
    protected:
        struct CallNagent2AuthIDPar
        {
            CallNagent2AuthIDPar(
                            KLDB::DbConnectionPtr   pCon, 
                            const std::wstring&     wstrNagentId)
                :   m_pCon(pCon)
                ,   m_wstrNagentId(wstrNagentId)
            {;};
            KLDB::DbConnectionPtr   m_pCon;
            std::wstring            m_wstrNagentId;
        };
        class CCallNagent2AuthID
        {
        public:
            CCallNagent2AuthID()
            {;};
            std::wstring operator() (CallNagent2AuthIDPar& par, const std::wstring& wstrNagent);
        };
        KLSTD::PairCache<   KLSTD::PairCachePropsHash<std::wstring, 
                            CallNagent2AuthIDPar&, 
                            std::wstring, 
                            CCallNagent2AuthID> > m_oChacheNagent2AuthID;
    protected:
        class CallHostStr2Int
        {
        public:
            long operator() (KLDB::DbConnectionPtr pCon, const std::wstring& wstrHostd);
        };
        KLSTD::PairCache<   KLSTD::PairCachePropsHash<long, 
                            KLDB::DbConnectionPtr, 
                            std::wstring, 
                            CallHostStr2Int> > m_oChacheHostStr2Int;



        class CallHostId2HostInfo
        {
        public:
            class CHostInfo
                :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
                ,   public host_info_t
            {
            public:
                CHostInfo(){;};
                virtual ~CHostInfo(){;};
            };
            typedef KLSTD::CAutoPtr<CHostInfo>      CHostInfoPtr;
            typedef KLSTD::KLAdapt<CHostInfoPtr>    AdaptedHostInfoPtr;

            AdaptedHostInfoPtr operator() (KLDB::DbConnectionPtr pCon, long lHostId);
        };
        KLSTD::PairCache<   KLSTD::PairCachePropsHash<CallHostId2HostInfo::AdaptedHostInfoPtr, 
                            KLDB::DbConnectionPtr, 
                            long, 
                            CallHostId2HostInfo> > m_oChacheHostIdHostInfo;
    private:
        KLSTD::CAutoPtr<KLSTD::MemoryChunk> m_pMasterPrivateKey;
        KLSTD::CAutoPtr<KLSTD::MemoryChunk> m_pMasterPublicKey;
    protected:
        KLSRV::HCHANGESINK  m_hSsChanged_HOSTSTATUS;
        KLPRCI::HSUBSCRIBE  m_hSsChanged_MASTER_SRV;
        KLSRV::HCHANGESINK  m_hSsChanged_MainSection;
        //KLTP::ThreadsPool::WorkerId m_idDbConWatchDog;
        KLTP::ThreadsPool::WorkerId m_idDbChkHostStatus;
        KLTP::ThreadsPool::WorkerId m_idMasterSrvConnChecker;
        KLTP::ThreadsPool::WorkerId m_idCheckSlaves;
        KLSTD::ObjectLock&          m_lckObject;
        KLSTD::CReentProtect        m_lckDbChkHstMoveRule;
        //CCallCtlr   m_oCallCtrl;
        KLSTD::CAutoPtr<KLSRVBCASTER::GlobalSrvBroadcaster> m_pGlobalSrvCaster;
        KLSTD::CAutoPtr<DiskAndDbSpaceChecker>  m_pDiskAndDbSpaceChecker;
        KLSTD::CAutoPtr<SectionsStore>          m_pSectionsStore;
        //CPingHandler*       m_pPingHandler;
        //CHstGrpHandler*     m_pHstGrpHandler;
        //CGsynHandler*       m_pGsynHandler;
        //CHstUnavailHandler* m_pHstUnavailHandler;
        
        //SSL
        int             m_nSslPort;
        std::wstring    m_wstrSslLocation;
        KLSTD::CAutoPtr<KLPAR::BinaryValue> m_pCert;
        KLSTD::CAutoPtr<KLSRV::CertData>    m_pCertData;
        KLSTD::CAutoPtr<KLPAR::BinaryValue> m_pPubServerKey, m_pPrvServerKey;
        KLSTD::CAutoPtr<KLPAR::BinaryValue> m_pServerKeyHash;
        ;
        //Non SSL
        int             m_nPort;
        std::wstring    m_wstrLocation;
        //Extra
        int             m_nExtraListenerPort;
        //LwNgt
        int             m_nLwNgtPort;
        std::wstring    m_wstrLwNgtLocation;

    //protected:  // stored procedures - ADO commands:
        //ADODB::_CommandPtr		m_pCmdDeleteGroup;
        //ADODB::_ParameterPtr	m_pAdoPar_DeleteGroup_GroupId;
        //void CheckCmdDeleteGroup();
    public://statistics counters
        srv_ping_statistics_t m_oPingStat;
        //volatile long m_lPingsCnt, m_lPingsJnCnt, m_lSyncCnt;//, m_lSyncRealCnt, m_lSyncSucCnt, m_lSyncJnCnt;    
		KLTRAP::TransportTimeout m_MasterSrvConnTimeout;
		unsigned long m_ulMasterSrvConnTicks;
		unsigned long m_ulMasterSrvLastPingTicks;
		unsigned long m_ulMasterSrvLastPrdListUpdate;
		AVP_dword m_dwMasterSrvDataChecksum;
		std::list<std::wstring> m_lstMainSrvConnectFailReported;
		std::wstring m_wstrLastMasterSrvConnectError;
	protected:
		KLSTD::CAutoPtr<CTaskBackup> m_pTaskBackup;

		KLSRV::UpdProdVerMap m_vecLastSent2MasterProdVers, m_vecLastSent2MasterProdVersCandidate;
		KLSRV::UpdCompVerMap m_vecLastSent2MasterCompVers, m_vecLastSent2MasterCompVersCandidate;
		bool m_bProdVersSent;
        /*
	protected:
		void AddGrpFindHostsData(
			const std::wstring& wstrFld,
			const std::wstring& wstrFilter, 				
			const std::wstring& wstrType)
		{
	        m_mapGrpFindHostsFilter[wstrFld] = wstrFilter;
			m_mapKeyInfoType[wstrFld] = wstrType;
		}
		long m_lGrpFindHstTmpTbl;
        */
	protected:
		void OnSlaveSrvDisconnectedEvent(KLPRCI::ComponentId idRemoteComponentId, KLSTD::precise_time_t t);
		void PublishSlaveSrvDisconnectedEvent(
			KLDB::DbConnectionPtr pDbConn, KLPRCI::ComponentId idRemoteComponentId, KLSTD::precise_time_t t);
		void PublishMasterSrvConnectedEvent(KLSTD::CAutoPtr<KLPAR::Params> pMasterSrvData);

		long GetProductComponentId(
			KLDB::DbConnectionPtr pCon,
			const std::wstring& wstrComponentName, 
			const std::wstring& wstrComponentVersion);

		void MakePSSProductComponents(
			KLDB::DbConnectionPtr pCon,
			KLPAR::Params*      pSyncData,
			long                nHostId,
			const std::wstring&		wstrLocation,
			std::map<long, bool>& mapUnchangedProducts,
			vec_prdcomp_t&		vecInPSS);

		void MakeDBProductComponents(
			KLDB::DbConnectionPtr pCon,
			long                nHostId,
			const std::map<long, bool>& mapUnchangedProducts,
			vec_prdcomp_t&		vecInDB);

		bool CheckIfNeedSendProductsInfo(KLDB::DbConnectionPtr pDbConn, KLPAR::ParamsPtr parIn);

		void UpdateSendProductsInfo();
	};
};

#endif //__SRVINST_H__
