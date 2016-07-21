/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	klserver.h
 * \author	Andrew Kazachkov
 * \date	05.11.2002 19:23:21
 * \brief	
 * 
 */

#ifndef __KLSERVER_H__A4A9B656_0FC2_43fd_B9E0_402213751EB1
#define __KLSERVER_H__A4A9B656_0FC2_43fd_B9E0_402213751EB1

/*#include <hst/hosts.h>
#include <grp/groups.h>
#include <pkg/packages.h>
#include <gsyn/groupsync.h>
#include <prcp/proxybase.h>
#include <basecomp/cmpbase.h>
#include "./server_constants.h"
#include <std/reporter.h>
#include "./connstore.h"*/

#include <std/base/klbase.h>
#include <std/base/klbaseqi_imp.h>

#include <common/reporter.h>

#include <transport/tr/transport.h>

#include <srvp/hst/hosts.h>
#include <srvp/grp/groups.h>
#include <srvp/gsyn/groupsync.h>

#include <server/srvinst/connstore.h>
#include <server/db/dbconnection.h>
#include <kca/aud/auditsource.h>
#include <kca/basecomp/cmpbase.h>
#include <server/evp/transeventsource.h>
//#include <srvp/pkg/packages.h>
//#include <srvp/gsyn/groupsync.h>
#include <server/srvhrch/srvhrch_master.h>
#include <server/evp/eventsprocessorsrv.h>
#include <server/rpt/rpt_srv.h>

#include <common/measurer.h>

namespace KLTSK
{
    class GroupTaskControlSrv;
};

namespace KLSRV
{
    class KLSTD_NOVTABLE PoliciesSettings : public KLSTD::KLBaseQI
    {
    public:
        virtual std::wstring GetDiskStorage() = 0;
        virtual std::wstring GetRelativeSyncPath( long nPolicyId ) = 0;
        virtual std::wstring GetPolicyPath( long nPolicyId ) = 0;
    };

    class KLSTD_NOVTABLE Deinitializer : public KLSTD::KLBase
    {
    public:
        virtual void Deinitialize() = 0;
    };
    
    class KLSTD_NOVTABLE TimeoutObjectsControl: public KLSTD::KLBaseQI
    {
    public:
        virtual void OnTimeoutObjectAdded(const std::wstring& wstrKey) = 0;
        virtual void OnTimeoutObjectRemoved(const std::wstring& wstrKey) = 0;
    };

	class KLSTD_NOVTABLE Control : public KLSTD::KLBaseQI
	{
	public:
		virtual void DoInitialize()=0;
		virtual bool GetStopFlag() = 0;        
		virtual void DoDeinitialize()=0;
        virtual void CancelInitialization() = 0;
	};

    typedef KLTRAP::Transport::LocationExList BcastLocs;

    class KLSTD_NOVTABLE SrvForceSynch
        : public KLSTD::KLBaseQI
    {
    public:
        virtual void ForceSynchronization(
            KLDB::DbConnectionPtr   pCon,
            const std::wstring&     wstrHost,
            const std::wstring&     wstrType,
            KLPRCI::ProductTasksControl* pTasksControl,
            long                    lTaskId) = 0;
        
        virtual void AddHostsForSync(
            KLDB::DbConnectionPtr       pCon,
            const KLPAR::ArrayValue*    arrHosts,
            const std::wstring&         wstrSSType,
            KLPRCI::ProductTasksControl* pTasksControl,
            long                        lTaskId) = 0;

        virtual void AddGroupHostsForSync(
            KLDB::DbConnectionPtr   pCon,
            long                    idGroup,
            const std::wstring&     wstrSSType,
            KLPRCI::ProductTasksControl* pTasksControl,
            long                    lTaskId) = 0;
    };
    
    class KLSTD_NOVTABLE SsCachedForHost
        :   public KLSTD::KLBaseQI
    {
    public:
        //! returns changed flag for ss (tries to cache it)
        virtual bool IsSsChanged(
                    const std::wstring& wstrHost, 
                    const std::wstring& wstrType) = 0;
        
        //! creates Ss
        virtual KLSTD::CAutoPtr<KLPRSS::SettingsStorage> AcquireHostSs(
								const std::wstring& wstrHost, 
								const std::wstring& wstrType,
                                AVP_dword           dwCreation,
                                AVP_dword           dwAccess) = 0;

        //! for use by sync ONLY !!! Use AcquireHostSs instead !!!
        virtual std::wstring AcquireSsPath(
                    const std::wstring& wstrHost, 
                    const std::wstring& wstrType) = 0;
    };

    class KLSTD_NOVTABLE ScanMsNetwork_Counters
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void put_DomainBeingScanned(
                    const std::wstring& wstrDomainBeingScanned) = 0;
        virtual void get_DomainBeingScanned(
                    std::wstring& wstrDomainBeingScanned) = 0;
        
        virtual void put_NetworkWasScanned() = 0;
        virtual bool get_NetworkWasScanned() = 0;
        
        virtual void put_LastFastScan(time_t tmLastFastScan) = 0;
        virtual void get_LastFastScan(time_t& tmLastFastScan) = 0;
        
        virtual void put_LastFullScan(time_t tmLastFullScan) = 0;
        virtual void get_LastFullScan(time_t& tmLastFullScan) = 0;
        
        virtual void put_NetworkScanPercent(long lPercent) = 0;
        virtual void get_NetworkScanPercent(long& lPercent) = 0;
        
        virtual void put_FullNetworkScanPercent(long  lPercent) = 0;
        virtual void get_FullNetworkScanPercent(long& lPercent) = 0;

        virtual void put_DpnsScanPercent(long  lPercent) = 0;
        virtual void get_DpnsScanPercent(long& lPercent) = 0;

        virtual void put_LastDpnsScan(time_t tmLastScan) = 0;
        virtual void get_LastDpnsScan(time_t& tmLastScan) = 0;

        virtual void put_AdScanPercent(long  lPercent) = 0;
        virtual void get_AdScanPercent(long& lPercent) = 0;

        virtual void put_LastAdScan(time_t tmLastScan) = 0;
        virtual void get_LastAdScan(time_t& tmLastScan) = 0;
    };

    enum
    {
        ADMGRPCHGD_HOSTLIST = 0x1   //list of host changed
    };

    struct AdmGrpChanged
    {
        AdmGrpChanged()
            :   m_lGroupId(-1L)
            ,   m_dwMask(0)
        {
            ;
        };
        long        m_lGroupId; //! group id
        AVP_dword   m_dwMask;   //! ADMGRPCHGD_* flags
    };

    class KLSTD_NOVTABLE ServerCallbacks: public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Marks for synchronization all hosts that require it
        */
        virtual void SynchronizeHosts(KLDB::DbConnectionPtr pCon) = 0;

        virtual KLSTD_NOTHROW void OnHostMoved(
                KLDB::DbConnectionPtr       pCon,
                long                        lHostId) throw() = 0;
        
        virtual KLSTD_NOTHROW void OnGsyncForHostMade(
                KLDB::DbConnectionPtr           pCon,
                long                            lSyncId,
                long                            idHost,
                const std::wstring&             wstrType,
                const std::wstring&             wstrLocation,
                bool                            bSuccess,
                KLERR::Error*                   pErrorDesc,
                const KLSTD::precise_time_t&    tmSyncStart) throw() = 0;

        virtual void UpdateLicenseInfo(
                    KLDB::DbConnectionPtr           pCon,
                    const std::wstring& wstrHost,
					long				idHost,
                    KLPAR::Params*      pData,
					const std::wstring& wstrID) = 0;

        virtual void RereadHostStatusAsync() = 0;

		virtual void OnMasterServerDisconnect(KLPRCI::ComponentId compoentId, KLSTD::precise_time_t t) = 0;
		
		virtual void RereadMasterServerDataAndConnect() = 0;

		virtual void CheckMasterServerConnection() = 0;

        //for use by gsyn ONLY !!!
        virtual std::wstring GetClientStoreLocation(const std::wstring& wstrType) = 0;
        
        //for use by gsyn ONLY !!!
        virtual void OnFinishedGsync(const std::wstring& wstrType) = 0;

        virtual void OnInitialPingMaster(
                                KLDB::DbConnectionPtr pCon,
                                const std::wstring& wstrPublicKeyHash,
								const std::wstring& wstrVersion,
                                const std::wstring& wstrInstanceId,
								KLSTD::CAutoPtr<KLPAR::Params> pParams) = 0;

        virtual bool IsSlaveServerConnected(const std::wstring& wstrSrvInstanceId) = 0;

        virtual void CloseSlaveServerConnection(const std::wstring& wstrSrvInstanceId) = 0;
		
		virtual KLSTD::CAutoPtr<KLPAR::BinaryValue> GetServerCertificate() = 0;
        
        virtual void OnShareUnavailable(const std::wstring& wstrShareName) = 0;

		virtual void DoDeliverReport(						
			const std::wstring& wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			const std::wstring& wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult) = 0;
		
		virtual void OnDeliverReport(						
			const std::wstring& wstrRemoteConnectionName,
			const std::wstring& wstrRequestId,
			int nRecipientDeliverLevel,
			int nSenderDeliverLevel,
			const std::wstring& wstrSenderInstanceId,
			KLSTD::CAutoPtr<KLPAR::Params> parDeliveryParams,
			KLSTD::CAutoPtr<KLPAR::Params> parReportResult) = 0;

		virtual void OnNotifyOnReportResult(
			const std::wstring& wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams) = 0;
        
        virtual void UpdateOsInfo(
                    KLDB::DbConnectionPtr   pCon,
                    long                    lHostId,
                    const std::wstring&     wstrHost) = 0;

        virtual void UpdateProductComponentsInfo(
            KLDB::DbConnectionPtr pCon,
			const std::wstring& wstrHost,
            long nHost,
            KLPAR::Params* pSyncData) = 0;
        
        virtual void UpdateProductFixesInfo(
            KLDB::DbConnectionPtr pCon,
			const std::wstring& wstrHost,
            long nHost,
            KLPAR::Params* pSyncData) = 0;
        
        /*!
          \brief	method is called if group chnage occured

          \param	pCon            IN  database connection wrapper
          \param	pData           IN  pointer to array of AdmGrpChanged structures
          \param	nData           IN  length of pData array
        */
        virtual void OnAdmGroupChanged(
            KLDB::DbConnectionPtr       pCon,
            const AdmGrpChanged*        pData,
            size_t                      nData) = 0;
        
        /*!
          \brief	OnHostsVisibility

          \param	plHosts IN  array of host ids
          \param	nHosts  IN  length of array
        */
        virtual void OnHostsVisibilityChanged(
                        const long* plHosts,
                        size_t      nHosts) = 0;
    
        /*!
          \brief	GetApplyPatches

          \param	pDbConn	IN  database connection wrapper
          \param	bSrv	OUT should auto apply server patches
          \param	bNagt	OUT should auto apply nagent patches
        */
		virtual void GetApplyPatches(KLDB::DbConnectionPtr pDbConn, bool& bSrv, bool& bNagt) = 0;

		virtual void DoNotifyOnReportResult(
			std::wstring wstrRequestId,
			long lResultCode,
			KLSTD::CAutoPtr<KLPAR::Params> parParams) = 0;
	};

    enum
    {
        ACF_DEFAULT=0,
        ACF_USE_MAIN_CONNECTION=1,
        ACF_USE_EXTRA_CONNECTION=2,
        ACF_DONT_FORCE=4
    };

	typedef std::map<std::wstring, std::vector<std::wstring> > UpdProdVerMap;
	typedef UpdProdVerMap UpdCompVerMap;

    typedef enum
    {
        HI_ORDINARY     =   0,
        HI_SLAVESERVER  =   1,
        HI_MOBILE_DEVICE=   2
    }SRVHOSTINFO;

    struct host_info_t
    {        
        host_info_t()
            :   m_nType(HI_ORDINARY)
            ,   m_bKeepConnection(false)
        {;};
        std::wstring            m_wstrHostId;
        std::wstring            m_wstrNagentId;
        SRVHOSTINFO             m_nType;
        KLSTD::MemoryChunkPtr   m_pHashPingResolve;
        KLSTD::MemoryChunkPtr   m_pHashPingOthers;
        bool                    m_bKeepConnection;
    };

    class KLSTD_NOVTABLE ServerHelpers: public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Finds hostid by hostname

          \param	pCon        db connection
          \param	strHostName hostname
          \return	hostid or -1 if not found
        */
        virtual long FindHost(  KLDB::DbConnectionPtr pCon,
                                const std::wstring& strHostName) = 0;

        /*!
          \brief	Returns Windows host name, domain name and type
                    (workgroup or domain) 

          \param	pCon                db connection
          \param	wstrHostsId         hostname
          \param	std::wstring&       WinHostName
          \param	std::wstring&       wstrWinDomainName
          \param	KLHST::DomainType&  nWinDomainType
          \return	returns false if no such host
        */
        virtual bool GetWinNames(
                        KLDB::DbConnectionPtr pCon,
                        const std::wstring& wstrHostsId,
                        std::wstring&       wstrWinHostName,
                        std::wstring&       wstrWinDomainName,
                        KLHST::DomainType&  nWinDomainType) = 0;

        /*!
          \brief	Rerurns host display name

          \param	pCon                db connection
          \param	lHost               hostid
          \param	wstrDisplayName     display name
          \return	returns false if no such host
        */
        virtual bool GetHostDisplayName(
                                KLDB::DbConnectionPtr pCon,
                                long lHost, 
                                std::wstring& wstrDisplayName) = 0;


        /*!
          \brief	Initiates synchronization with hosts for specified gsyn

          \param	pCon    db connection
          \param	lGsyn   gsyn id
          \param	dwFlags synchronization flags
        */
        virtual void InitiateHostSynForGsyn(
                                KLDB::DbConnectionPtr pCon,
                                long lGsyn, 
                                AVP_dword dwFlags) = 0;

        virtual void PublishEvent(
                        const wchar_t*          szwEventName,
                        long                    lEventId,
                        long                    lSeverity,
                        KLPAR::param_entry_t*   pPars,
                        size_t                  nPars) = 0;

        virtual void PublishEvent(
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
                        const wchar_t*          szwPar6 = NULL) = 0;

        virtual void MakeBroadcastLocations(
					KLDB::DbConnectionPtr pDbConn, 
					const std::vector<std::wstring>& vecIds,
                    BcastLocs&					vecLocations,
					std::vector<std::wstring>*	pVecLocatedIds = NULL,
                    std::vector<std::wstring>*  pVecFailedIds = NULL) = 0;

		virtual void FillLocationEx(
					KLTRAP::Transport::LocationEx&  loc,
					const std::wstring&             wstrLocation,
					const std::wstring&             wstrNagentInstaceId,
					const std::wstring&             wstrServerTrName,
					long                            idHost) = 0;
        
        virtual void GetLocalHostId(std::wstring& wstrHostId) = 0;
        
        virtual std::wstring MakeGroupSyncPath(const std::wstring& wstrType) = 0;

        virtual void GetLocalNagentInstance(std::wstring& wstrInstance) = 0;

        virtual void GetLocation(std::wstring& wstrLocation) = 0;

        virtual bool AcquireConnection(
                            KLDB::DbConnectionPtr   pCon,
                            const std::wstring&		wstrHostId,
                            NagentConnection**      ppConn,
                            AVP_dword               dwFlags=ACF_DEFAULT) = 0;

        virtual bool AcquireConnection2(
                            const std::wstring&     wstrInstance,
                            const std::wstring&     wstrAddress,
                            NagentConnection**      ppConn,
                            AVP_dword               dwFlags=ACF_DEFAULT) = 0;

        //!OBSOLETE
        virtual long get_TrRemoteUnicastTime() = 0;

        virtual bool AcquireSlaveConnection(
                            const std::wstring&     wstrInstance,
                            const std::wstring&     wstrAddress,
                            ConnectionInfo**        ppConn,
                            AVP_dword               dwFlags=ACF_DEFAULT) = 0;

		virtual void GetSlaveStaticInfo(
							KLSTD::CAutoPtr<KLSRV::ConnectionInfo>	pConnectionInfo, 
							KLPAR::Params*      pFilter, 
							KLPAR::Params**     ppData) = 0;
		
        virtual long GetSuperGroupId() = 0;
       
        //OBSOLETE
        virtual void InitiateHostSynForGsyn(long lGsyn, AVP_dword dwFlags) = 0;
        //OBSOLETE
        virtual bool GetWinNames(
                        const std::wstring& wstrHostsId,
                        std::wstring&       wstrWinHostName,
                        std::wstring&       wstrWinDomainName,
                        KLHST::DomainType&  nWinDomainType) = 0;
        //OBSOLETE
        virtual bool GetHostDisplayName(long lHost, std::wstring& wstrDisplayName) = 0;

        virtual KLSTD::CAutoPtr<KLPAR::BinaryValue> GetServerCertificate() = 0;

        virtual bool CheckHostNAgentId(
                        KLDB::DbConnectionPtr pCon,
                        const std::wstring& wstrHostsId,
                        const std::wstring& wstrNAgentId) = 0;

        virtual std::wstring GetHostNagentId(
                            KLDB::DbConnectionPtr pCon,
                            const std::wstring& wstrHostId) = 0;
        
        virtual void GetGroupTaskControlSrv(
                        KLTSK::GroupTaskControlSrv** ppGtc ) = 0;
		
	    virtual void get_ConnectionsCnt(long& lCnt, long& lNagCnt) = 0;

        /*!
          \brief	Returns id of specified product. If such name doesn't 
                    exist in the database it is added.

          \param	LDB::DbConnectionPtr   pCon
          \param	szwProduct  [in] product name (Example: "1103")
          \param	szwVersion  [in] 
          \return	id
        */
        virtual long GetProductId(
                            KLDB::DbConnectionPtr   pCon,
                            const wchar_t*          szwProduct,
                            const wchar_t*          szwVersion) = 0;

        /*!
          \brief	Returns id of specified product display name.  If 
                    such display name doesn't exist in the database it is 
                    added.

          \param	LDB::DbConnectionPtr   pCon
          \param	szwDisplayName
          \return	id
        */
        virtual long GetProductDnId(
                            KLDB::DbConnectionPtr   pCon,
                            const wchar_t*          szwDisplayName) = 0;
		
        /*!
          \brief	Returns path to group tasks root disk storage.

          \return	wstring
        */
        virtual std::wstring GetGrpTasksRootPath() = 0;
        

        /*!
          \brief	Returns ss location of specified type for specified host.
                    Use functions KLPRSS_MakeTypeG and KLPRSS_MakeTypeP to
                    acquire ss type string.
                    For example, KLPRSS_MakeTypeG(KLPRSS::c_szwSST_HostSS) 
                    returns to SS_SETTINGS type string and 
                    KLPRSS_MakeTypeG(KLPRSS::c_szwSST_ProdinfoSS) returns 
                    SS_PRODINFO type string. 

          \param	wstrHostId IN host id
          \param	wstrType IN ss type string
          \return	location
        */        
        virtual std::wstring AcquireHostSsLocation(
                            const std::wstring& wstrHostId,
                            const std::wstring& wstrType) = 0;
		
        /*!
          \brief	Returns SrvHierarchyMaster proxy.
		  */
		virtual KLSTD::CAutoPtr<KLSRVH::SrvHierarchyMaster>	GetMasterServerProxy() = 0;
        
        virtual std::wstring AcquireSuperGsynPath(const std::wstring& wstrType) = 0;

        virtual std::wstring AcquireServerSyncId() = 0;
		
		virtual void GetInstalledProducts(
			KLDB::DbConnectionPtr pDbConn, 
			KLSRV::UpdProdVerMap& vecProdVers,
			KLSRV::UpdCompVerMap& vecCompVers,
			bool bIncludeSlaveServers,
			bool bIncludeComponentVersions,
			bool bProcessAdditional) = 0;

        //! returns false if not found
        virtual bool Cache_AcquireHostInfo( 
                                KLDB::DbConnectionPtr   pCon,
                                long                    lHost,
                                host_info_t&            hi) = 0;

        //! returns -1L if not found
        virtual long Cache_FindHost(
                                KLDB::DbConnectionPtr   pCon,
                                const std::wstring&     wstrHostName,
                                host_info_t*            pHi = NULL) = 0;

        virtual void Cache_UpdateHostInfo(
                                KLDB::DbConnectionPtr   pCon,
                                long                    lHostId,
                                const host_info_t&      hi) = 0;

        virtual void Cache_ClearHostInfo(
                                KLDB::DbConnectionPtr       pCon,
                                long                        lHost) = 0;

        virtual void CreateSuperGsynSs(
                    KLDB::DbConnectionPtr pCon,
                    const wchar_t*      szwSsSrc,
                    const wchar_t*      szwSsDst) = 0;
    };

    //! returns obsolete pointers (that cannot be acquired via QueryInterface)
    class KLSTD_NOVTABLE SrvObsoletePointers
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Returns ComponentBase pointer. 

          \return	ComponentBase pointer
        */
        virtual KLBASECOMP::ComponentBase* GetComponentBase() = 0;

        //! returns TransEventSource pointer
        virtual KLEVP::TransEventSource* GetTransEventSource() = 0;

        //! returns EventsProcessorSrv pointer
        virtual KLSTD::CAutoPtr<KLEVP::EventsProcessorSrv> GetEventsProcessor() = 0;

        //! returns GroupTaskControlSrv pointer
        virtual KLSTD::CAutoPtr<KLTSK::GroupTaskControlSrv> GetGroupTaskControlSrv() = 0;

        //! returns ReportsSrv pointer
        virtual KLSTD::CAutoPtr<KLSRV::ReportsSrv> GetReportsSrv() = 0;
    };


    class KLSTD_NOVTABLE SrvHstSyncQueue
        : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Adds specified host into synch queue

          \param	pCon    DB connection
          \param	pHosts hosts array
          \param	nHosts number of hosts in array          
        */
        virtual void AddHostsToSyncQueue(
            KLDB::DbConnectionPtr       pCon,
            const long*                 pHosts,
            size_t                      nHosts) = 0;

        /*!
          \brief	Adds all host from group and its subgroups into synch 
                    queue

          \param	pCon    DB connection
          \param	idGroup group id
        */
        virtual void AddGroupToSyncQueue(
            KLDB::DbConnectionPtr       pCon,
            long                        idGroup) = 0;
    };

    KLSTD_DECLARE_HANDLE(HCHANGESINK); // KLSRV::HCHANGESINK
    
    class KLSTD_NOVTABLE SrvSettingsChangeSink
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void OnNotify(
                        const wchar_t* szwProduct,
                        const wchar_t* szwVersion,
                        const wchar_t* szwSection) = 0;
    };

    typedef KLSTD::CAutoPtr<SrvSettingsChangeSink> SrvSettingsChangeSinkPtr;
    
    class KLSTD_NOVTABLE ServerSettings
        :   public KLSTD::KLBaseQI
    {
    public:
        //! unicast wait timeout in milliseconds
        virtual long get_TrRemoteUnicastTime() = 0;

        //! host visibility period in seconds
        virtual long get_VisibilityPeriod() = 0;
        
        //! udp settings
        virtual void get_UdpSettings(int& nPort, int& nCount) = 0;

        //! returns specified parameter. One shouldn't change returned data.
        virtual KLSTD::CAutoPtr<KLPAR::Value> GetParameter(
                            const wchar_t* szwName,
                            const wchar_t* szwSection = KLSRV_COMMON_SETTINGS,
                            const wchar_t* szwProduct = KLCS_PRODUCT_ADMSERVER, 
                            const wchar_t* szwVersion = KLCS_VERSION_ADMSERVER) = 0;
        
        //! returns specified parameters. One shouldn't change returned data.
        virtual KLPAR::ParamsPtr GetParameters(
                            const wchar_t** szwNames,
                            size_t          nNames,
                            const wchar_t*  szwSection = KLSRV_COMMON_SETTINGS,
                            const wchar_t*  szwProduct = KLCS_PRODUCT_ADMSERVER, 
                            const wchar_t*  szwVersion = KLCS_VERSION_ADMSERVER) = 0;

        /*!
          \brief	Adds sink

          \param	szwSection IN 
          \param	szwProduct IN 
          \param	szwVersion IN 
          \param	pSink IN
          \return	sink id
        */
        virtual KLSRV::HCHANGESINK Advise(
                            KLSRV::SrvSettingsChangeSink *pSink,
                            const wchar_t* szwSection = KLSRV_COMMON_SETTINGS,
                            const wchar_t* szwProduct = KLCS_PRODUCT_ADMSERVER, 
                            const wchar_t* szwVersion = KLCS_VERSION_ADMSERVER) = 0;

        /*!
          \brief	Removes sink

          \param	hSink IN 
        */
        virtual void Unadvise(KLSRV::HCHANGESINK hSink) = 0;
    };

    typedef KLSTD::CAutoPtr<ServerSettings> ServerSettingsPtr;

	class KLSTD_NOVTABLE ServerInstance : public KLSTD::KLBaseQI
	{
	public:
        //temp
		//virtual void OnPing(KLPAR::Params* pInfo, KLPAR::Params** ppResult) = 0;
		//virtual void OnPingAsync(KLSTD::CAutoPtr<KLPAR::Params> pInfo) = 0;
        virtual void GetRunTimeInfo(KLPAR::Params* pFilter, KLPAR::Params** ppData) = 0;
        virtual void GetStaticInfo(KLPAR::Params* pFilter, KLPAR::Params** ppData) = 0;
		///
        
        virtual void GetUpdatesInfo(KLPAR::Params* pFilter, KLPAR::Params** ppData) = 0;		

        virtual void SetBundleOptions( KLPAR::Params* pOptions ) = 0;

        virtual void SetGeneralOptions( KLPAR::Params* pOptions ) = 0;

        virtual void GetGeneralOptions( KLPAR::Params** ppOptions ) = 0;

		virtual Control* GetControl()=0;
        
        virtual void ForceUpdate()=0;
        //Callbacks
        virtual void OnAppHostChanged() = 0;
		virtual int OnTransportStatusNotify(
							KLTRAP::Transport::ConnectionStatus	status, 
							const wchar_t*			remoteComponentName,
							const wchar_t*			localComponentName,
							bool					bRemote) = 0;        

        virtual bool CheckHostNAgentId(
                        const std::wstring& wstrHostsId,
                        const std::wstring& wstrNAgentId) throw() = 0;

        //virtual void WatchdogPing() = 0;

        //virtual void OnInstallerNotification(
                    //KLPAR::Params*  pData,
                    //KLPAR::Params** ppResult) = 0;
        ;
        virtual void RemoveStores(
                        const wchar_t**         pStores,
                        size_t                  nStores,
                        std::vector<size_t>&    vecUnremoved) = 0;

        virtual void StartNewIntegration(const wchar_t* unused) = 0;
        
        virtual void GetServerPublicKey(
                        KLSTD::MemoryChunk**    pOpenKey,
                        std::wstring&           wstrHash) = 0;

        virtual void PutMasterGlobalKeys(
                        const void* pKey1,
                        size_t      nKey1,
                        const void* pKey2,
                        size_t      nKey2) = 0;
        virtual long GetSuperGroupId() = 0;
        
        virtual void OnUnicastArrived(std::wstring  wstrRemoteComponentName) = 0;
        
		virtual void GetAvailableUpdatesInfo(
			const wchar_t*      szwLocalization,
			KLPAR::Params** ppAvailableUpdateComps,
			KLPAR::Params** ppAvailableUpdateApps) = 0;

		virtual void GetRequiedUpdatesInfo(
			KLPAR::Params** ppInstalledComps,
			KLPAR::Params** ppInstalledApps) = 0;

	};
    
    class KLSTD_NOVTABLE SrvCheckAccess
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual bool CheckAccessInCallForGroup(
	                    AVP_dword   dwActionGroupID,
	                    AVP_dword   dwAccessMasks,
			            long	    lGroupId,
	                    bool        bGenerateException = true,
                        const std::wstring* pwstrBasicAuthLogin = NULL) = 0;

        virtual bool CheckAccessInCallForHost(
	                    AVP_dword       dwActionGroupID,
	                    AVP_dword       dwAccessMasks,
                        const wchar_t*  szwHostId,
	                    bool            bGenerateException = true,
                        const std::wstring* pwstrBasicAuthLogin = NULL) = 0;
        
        virtual bool CheckAccessInCallForGroupParent(
	                    AVP_dword   dwActionGroupID,
	                    AVP_dword   dwAccessMasks,
			            long	    lGroupId,
	                    bool        bGenerateException = true,
                        const std::wstring* pwstrBasicAuthLogin = NULL) = 0;
    };

    #define KLSRV_AUDIT_BEGIN(_type)                    \
        KLERR_BEGIN                                     \
            if(KLAUD_GetAuditSource()->IsAuditActive(   \
                KLAUD::AuditObjectAdministrationGroup)) \
            {                                           \
            KL_TMEASURE_BEGIN(L"Publishing audit events", 4)


    #define KLSRV_AUDIT_END()                           \
            KL_TMEASURE_END()                           \
            };                                          \
        KLERR_ENDT(1)
    
    class TaskRestartableScan
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void Restart() = 0;
    };

    class KLSTD_NOVTABLE HstGrpGroupChanged
        :   public KLSTD::KLBaseQI
    {
    public:
        //! server must check for groups changed
        virtual void GroupChangeOccured() = 0;

        //! sets change mask for groups (usually src and dts) -1L means NULL
        virtual void SetChangeMask(
                            KLDB::DbConnectionPtr   pCon, 
                            long lGrp1, 
                            long lGrp2, 
                            AVP_dword dwMask) = 0;
    };

    class KLSTD_NOVTABLE HstGrpHostStatuses
        :   public KLSTD::KLBaseQI
    {
    public:
        //! mark host's status as unactual
        virtual void HostStatusPossiblyChanged(
                        KLDB::DbConnectionPtr   pCon, 
                        long                    lHost) = 0;

        //! mark hosts' statuses as unactual
        virtual void HostStatusPossiblyChangedForHosts(
                        KLDB::DbConnectionPtr   pCon, 
                        long*                   pHosts, 
                        size_t                  nHosts ) = 0;

        //! mark group hosts' statuses as unactual
        virtual void HostStatusPossiblyChangedForGroup(
                        KLDB::DbConnectionPtr   pCon,
                        long                    lGroup) = 0;

        //! asynchroniousely updates host times
        virtual void UpdateHostTimes(
                        long                    lHost,
                        time_t                  tmLastVisible,
                        time_t                  tmLastInfoUpdate = KLSTD::c_invalid_time_t,
                        time_t                  tmLastConnected = KLSTD::c_invalid_time_t,
                        bool                    bUpdateStatus = false) = 0;
    };
    
    class KLSTD_NOVTABLE HstGrpHostQueryCallback
        :   public KLSTD::KLBaseQI
    {
    public:
        //! must be called after locking connection but before executing query
        virtual void PreExec(
                        KLDB::DbConnectionPtr   pCon) = 0;

        //! must be called after executing query but before unlocking connection 
        virtual void PostExec(
                        KLDB::DbConnectionPtr   pCon) = 0;
    };

    class KLSTD_NOVTABLE HstGrpHostQuery
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void ConstructQuery_ToFindHosts(
            const std::wstring&                 wstrFilter,
		    const std::vector<std::wstring>&    vecFieldsToReturn,
            const KLCSP::vec_field_order_t&     vecFieldsToOrder,
            const std::wstring&                 wstrHostsInnerJoin,
            std::wstring&                       wstrQuery,
            HstGrpHostQueryCallback**           ppCallback,
            bool                                bCheckAccess) = 0;
    };
    
    class KLSTD_NOVTABLE SrvAdmLic
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual std::wstring GetSsPath() = 0;
    };
};

KLCSSRV_DECL void KLSRV_CreateServerInstance(
                                    KLSTD::Reporter*        pReporter, 
                                    AVP_dword               dwFlags, 
                                    KLSRV::ServerInstance** ppInstance);

KLCSSRV_DECL void KLHST_SetServerInstance(
                                    KLSRV::ServerInstance* pServerInstance);

KLCSSRV_DECL void KLHST_GetServerInstance(
                                    KLSRV::ServerInstance** ppServerInstance);

#endif //__KLSERVER_H__A4A9B656_0FC2_43fd_B9E0_402213751EB1
