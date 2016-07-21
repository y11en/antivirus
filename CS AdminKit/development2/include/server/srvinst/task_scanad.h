/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	Task_ScanAD.h
 * \author	Andrew Kazachkov
 * \date	13.10.2004 13:45:14
 * \brief	
 * 
 */

#ifndef __KLTASK_SCAN_AD_H__
#define __KLTASK_SCAN_AD_H__

#include <kca/basecomp/cmpbase.h>
#include <std/wnf/windowsnetinfo.h>
#include "./srvinst.h"
#include <list>
#include <server/srvinst/task_settings_base.h>
#include <common/ak_cached_data.h>
#include <std/wnf/windowsnetinfo.h>

namespace KLSRV
{	
    class CTask_ScanAD
        :	public KLSRV::CServerTaskBase
        ,   public KLSRV::TaskRestartableScan
        ,   public  KLSRV::TaskSettingsT<CTask_ScanAD>
	{
	public:
		CTask_ScanAD();
		virtual ~CTask_ScanAD();

        KLSTD_INTERAFCE_MAP_BEGIN(KLBASECOMP::TaskBase)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::TaskRestartableScan)
        KLSTD_INTERAFCE_MAP_END()

		void Initialize(
						long				idTask,
                        KLSRV::SrvData*     pSrvData,
                        CServerInstance*	pInst);
        void OnInitialize();
        void OnDeinitialize();
		const wchar_t* GetTaskName(){return c_szwTaskName;};
		bool OnExecute();
		void OnControl(KLPRCI::TaskAction action);
		static const wchar_t c_szwTaskName[];
    //KLSRV::TaskRestartableScan
        virtual void Restart();
    protected:
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
        long FindUnit(KLDB::DbConnectionPtr   pCon, const wchar_t* szwGuid, bool& bNeedScan);
        void ProcessSubunits(
                            KLDB::DbConnectionPtr   pCon, 
                            long                    lParentUnit, 
                            KLWNF::AD_OBJ_LIST&     lstSubunits);
        void ProcessHosts(
                            KLWNF::WindowsNetInfo*  pNetInfo,
                            KLDB::DbConnectionPtr   pCon, 
                            long                    lParentUnit, 
                            KLWNF::AD_OBJ_LIST&     lstHosts);
    public:
        class CallDistinguished2Nt4
        {
        public:
            CallDistinguished2Nt4()
            {;};
            std::wstring operator() (
                        KLWNF::WindowsNetInfo*     pNetInfo, 
                        const std::wstring& wstrDistinguished);
        };
        KLSTD::PairCache<
            KLSTD::PairCachePropsHash<
                            std::wstring, 
                            KLWNF::WindowsNetInfo*, 
                            std::wstring, 
                            CallDistinguished2Nt4> > m_oCacheDistinguished2Nt4;
        ;
        class CallNtDomain2Id
        {
        public:
            CallNtDomain2Id()
            {;};
            long operator() (
                        KLDB::DbConnectionPtr   pCon, 
                        const std::wstring&     wstrName);
        };
        KLSTD::PairCache<
            KLSTD::PairCachePropsHash<
                            long, 
                            KLDB::DbConnectionPtr, 
                            std::wstring, 
                            CallNtDomain2Id> > m_oCacheNtDomain2Id;
        ;
        class CallDnsDomain2Id
        {
        public:
            CallDnsDomain2Id()
            {;};
            long operator() (
                        KLDB::DbConnectionPtr   pCon, 
                        const std::wstring&     wstrName);
        };
        KLSTD::PairCache<   
            KLSTD::PairCachePropsHash<
                            long, 
                            KLDB::DbConnectionPtr, 
                            std::wstring, 
                            CallDnsDomain2Id> > m_oCacheDnsDomain2Id;
    public:
        KLSRV::SrvData*             m_pSrvData;
    protected:
        CServerInstance*	    m_pSrvInst;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCacheCS;
        KLSTD::CAutoPtr<KLSRV::HstGrpGroupChanged>      m_pHstGrpGroupChanged;
        KLSTD::CAutoPtr<KLSRV::ServerHelpers>		    m_pServerHelpers;
        KLSTD::CAutoPtr<SRVNETGSCAN::GeneralNetScan>    m_pGeneralNetScan;
        unsigned long           m_clkLastRun;
        size_t                  m_nPeriod;
        bool                    m_bEnabled;
        bool                    m_bIncludeContainers;
        bool                    m_bRestarting;
 	};
};

#endif //__KLTASK_SCAN_AD_H__