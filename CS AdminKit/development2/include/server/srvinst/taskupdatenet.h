/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	TaskUpdateNet.h
 * \author	Andrew Kazachkov
 * \date	30.10.2002 14:09:07
 * \brief	
 * 
 */



#if !defined(AFX_TASKUPDATENET_H__B1A7E05B_0508_414A_B497_C7B881BF166F__INCLUDED_)
#define AFX_TASKUPDATENET_H__B1A7E05B_0508_414A_B497_C7B881BF166F__INCLUDED_

#include <kca/basecomp/cmpbase.h>
#include <srvp/hst/hosts.h>
#include <srvp/grp/groups.h>
#include <std/wnf/windowsnetinfo.h>
#include <server/srvinst/task_mthr_base.h>
#include <server/srvinst/task_settings_base.h>
#include <server/srvnetscan/srvnetscan.h>

#include <hash_map>

namespace KLSRV
{	

    bool CheckIfTaskUpdateNetEnabled(
                KLSRV::ServerSettings* pServerSettings);

	class CTaskUpdateNet
        :   public KLSRV::CTaskMultiThreadedBase
        ,   public KLSRV::TaskRestartableScan
        ,   public  KLSRV::TaskSettingsT<CTaskUpdateNet>
	{
	public:
		CTaskUpdateNet();
		virtual ~CTaskUpdateNet();
		void Initialize(
							long				        idTask,
                            KLSRV::SrvData*             pSrvData,
                            KLBASECOMP::ComponentBase*  pComponent);
		const wchar_t* GetTaskName(){return c_szwTaskName;};
        static const wchar_t c_szwTaskName[];

        KLSTD_INTERAFCE_MAP_BEGIN(KLBASECOMP::TaskBase)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::TaskRestartableScan)
        KLSTD_INTERAFCE_MAP_END()

    protected:
    //overridables
        void OnInitialize();
        void OnDeinitialize();
    public:
        struct tsk_settings_t
        {
            bool operator == (const tsk_settings_t& data) const
            {
                return (
                        m_lPeriod == data.m_lPeriod && 
                        m_bEnabled == data.m_bEnabled );
            };
            long    m_lPeriod;
            bool    m_bEnabled;
        };

        tsk_settings_t TaskSettings_GetSettings();
        tsk_settings_t TaskSettings_LoadSettings(
                            KLSRV::ServerSettings* pServerSettings);
        void TaskSettings_UseSettings(
                            const tsk_settings_t& settings);
    protected:
        void ClearState(const tsk_settings_t* pSettings = NULL);
    //KLSRV::TaskRestartableScan
        void Restart();
    //CTaskMultiThreadedBase
        virtual bool OnExecute();
        virtual void DoPass();
        virtual long GetRequiredThreadCount();
        virtual bool StartProcessing();
	protected:

        struct host_data_t
        {
			std::wstring            wstrWinHostName;
			long                    nNtDomain;
			std::wstring            wstrNtDomain;
            KLSTD::precise_time_t   tmLastVisible;
            long                    lRemovePrd;
            long                    lNotifyPrd;
            long                    lGroup;
            long                    lHost;
            std::wstring            wstrHostId;
            std::wstring            wstrDisplayName;
            bool                    bInvisAlreadyPublished;
            KLSTD::precise_time_t   tmLastInfoUpdate;
            KLSTD::precise_time_t   tmLastNagentConnected;
        };

        bool GetNextComputer(host_data_t& host);
        void ScanNetwork();
		void ScanHost(
                        KLDB::DbConnectionPtr   pCon,
                        long                    lHost,
						KLWNF::WindowsNetInfo*	pNetInfo,
						const std::wstring&		wstrDomain,
						long					idDomain,
						const std::wstring&		wstrWinHostName,
                        long                    lGroup,
                        bool                    bPublishedInvisible,
                        bool                    bNagentConnectedRecently);		
    protected:
        KLSTD::CAutoPtr<KLSRV::HstGrpGroupChanged> m_pHstGrpGroupChanged;
        KLSTD::CAutoPtr<KLSRV::ScanMsNetwork_Counters> m_pScanMsNetwork_Counters;
	protected:
        long                                    m_lInitialCount;
        bool                                    m_bUseMAC;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
        KLDB::DbRecordsetPtr                    m_pRs;
        KLSTD::CReentProtect                    m_rpDeinitialize;
        volatile long                           m_cntDivider;
        volatile long                           m_lCount;
        std::wstring                            m_wstrLocalHostId;
        volatile bool                           m_bEnabled, m_bRestarting;
        long                                    m_lPeriod;
	};

	class CFastUpdateNet
        :   public KLSRV::CTaskMultiThreadedBase
        ,   public KLSRV::TaskRestartableScan
        ,   public KLSRV::TaskSettingsT<CFastUpdateNet>
	{
	public:
		CFastUpdateNet();
		virtual ~CFastUpdateNet();
		void Initialize(                            
							long				        idTask,
                            KLSRV::SrvData*             pSrvData,
                            KLBASECOMP::ComponentBase*  pComponent);
		const wchar_t* GetTaskName(){return c_szwTaskName;};
		static const wchar_t c_szwTaskName[];

        KLSTD_INTERAFCE_MAP_BEGIN(KLBASECOMP::TaskBase)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::TaskRestartableScan)
        KLSTD_INTERAFCE_MAP_END()

    protected:
    //overridables
        void OnInitialize();
        void OnDeinitialize();
    public:
        struct tsk_settings_t
        {
            bool operator == (const tsk_settings_t& data) const
            {
                return (
                        m_lPeriod == data.m_lPeriod && 
                        m_bEnabled == data.m_bEnabled );
            };
            long    m_lPeriod;
            bool    m_bEnabled;
        };

        tsk_settings_t TaskSettings_GetSettings();
        tsk_settings_t TaskSettings_LoadSettings(
                            KLSRV::ServerSettings* pServerSettings);
        void TaskSettings_UseSettings(
                            const tsk_settings_t& settings);
    protected:
    //KLSRV::TaskRestartableScan
        void Restart();
    //CTaskMultiThreadedBase
        virtual bool OnExecute();
        void DoPass();
        long GetRequiredThreadCount();
        bool StartProcessing();
	protected:
        bool GetNextComputer(
                long&           nNtDomain,
                long&           nGroup,
                std::wstring&   wstrWinHostName);        
    protected:
        void ClearState(const tsk_settings_t* pSettings = NULL);
        //procentege
        void Percent_Clear();
        void Percent_Full();
        void Percent_Set();
        void Percent_Notify();
        void SetDomainBeingScanned(const std::wstring& wstrDomainScanned);
    protected:
        struct domain_t
        {
            std::wstring    m_wstrName;
            bool            m_bDomain;
        };
        void DoPass_AcquireDomains(bool bWorkgroups);
        void DoPass_ProcessHosts();
        bool GetNextDomain(domain_t& domain);
        /*
		void ScanNetwork();
        void ScanDomainsorWorkgroups(
                        KLDB::DbConnectionPtr   pCon,
                        bool bScanDomains, 
                        KLWNF::WindowsNetInfo* pNetInfo);
        */
		void ScanDomain(
                        KLDB::DbConnectionPtr   pCon,
                        domain_t&               domain,
						KLWNF::WindowsNetInfo*  pNetInfo);

        //returns true if new host, false otherwise
		/*
        bool SetHostInfo(
                        KLDB::DbConnectionPtr   pCon,
                        long                    nNtDomain,
                        long                    nDnsDomain,
                        long                    nGroup,
                        const std::wstring&     wstrWinHostName,
                        const std::wstring&     wstrDnsHostName,
                        bool                    bIsSureVisible,
                        std::wstring&           wstrHostId);
		*/

        void SetDomainHostsInfo(
                        KLDB::DbConnectionPtr       pCon,
                        const KLWNF::OBJ_LIST2000&  lstHosts,
                        long                        nNtDomain,
                        domain_t&                   domain,
                        long                        nDefaultDnsDomain,
                        bool                        bIsWorkGroup);
    protected:
        long GetDnsDomain(
                KLDB::DbConnectionPtr       pCon,
                const std::wstring&         wstrDnsDomain);
    protected:
        KLSTD::CAutoPtr<KLSRV::HstGrpGroupChanged> m_pHstGrpGroupChanged;
    protected:
        //KLSRV::SrvData*     m_pSrvData;
        long                m_lPercent;
                            //m_nNumerator, 
                            //m_nDenominator, 
                            //m_nBase, 
                            //m_nRelPercent;
        long                m_lScanDelay;
        volatile bool       m_bEnabled, m_bRestarting;
        volatile bool       m_bCancelEnumeration;
        long                m_lPeriod;
        //volatile long       m_lDomainListState;//0 - no, 1 - only wkg or domains, 2- both wkg and domains
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pPercentCS;
        volatile long                           m_lMaxValue;
        KLSTD::CReentProtect    m_rpDeinitialize;
        std::list<domain_t> m_lstDomains;
        volatile long       m_nList;
        //cached data
        typedef std::hash_map<std::wstring, long>    map_str2long_t;
        typedef map_str2long_t::iterator        it_map_str2long_t;
        typedef map_str2long_t::value_type      val_map_str2long_t;

        map_str2long_t  m_mapDnsDomains;
        //state
        volatile long       m_lDListPending, m_lWListPending;
        volatile long       m_lDListAcquired, m_lWListAcquired;

        const bool  m_bUseAD;
	};
    

    KLSTD::CAutoPtr<SRVNETGSCAN::HostInfo> CFastUpdateNet_CreateFastUpdNetHostInfo(
						    KLSRV::HstGrpGroupChanged*	pHstGrpGroupChanged,
						    KLSRV::ServerHelpers*		pServerHelpers,
						    int							nNtDomain,
						    int							lDnsDomain,
						    const std::wstring&			wstrWinHostName,
						    const std::wstring&			wstrDnsHostname,
						    const std::wstring&			wstrWinDomain);
};

#endif // !defined(AFX_TASKUPDATENET_H__B1A7E05B_0508_414A_B497_C7B881BF166F__INCLUDED_)
