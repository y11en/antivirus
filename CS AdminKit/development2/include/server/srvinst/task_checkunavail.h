/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	Task_CheckUnavail.h
 * \author	Andrew Kazachkov
 * \date	05.10.2004 18:58:27
 * \brief	
 * 
 */

#ifndef __KLTASK_CHECKUNAVAIL_H__
#define __KLTASK_CHECKUNAVAIL_H__

#include <kca/basecomp/cmpbase.h>
#include <server/srvinst/srvinst.h>
#include <list>

namespace KLSRV
{	
    class CTask_CheckUnavail
		:	public KLSRV::CServerTaskBase
	{
	public:
        struct unavail_host_t
        {
            long            m_lHost;
            unsigned long   m_ulIp;
        };

        struct second_ping_t
        {
            KLSTD::kltick_t m_clkMoment;
            unavail_host_t  m_host;
        };

        typedef std::list<second_ping_t> lst_second_ping_t;

		CTask_CheckUnavail();
		virtual ~CTask_CheckUnavail();
		void Initialize(
						long				        idTask,
                        KLSRV::SrvData*             pSrvData,
                        KLBASECOMP::ComponentBase*  pComponent,
                        KLSRV::ServerHelpers*       pServerHelpers);
		const wchar_t* GetTaskName(){return c_szwTaskName;};
		bool OnExecute();
		void OnControl(KLPRCI::TaskAction action);
		static const wchar_t c_szwTaskName[];
    protected:
        KLSTD_NOTHROW void ProcessHost(
                        KLDB::DbConnectionPtr   pCon,
                        const wchar_t*          szwAddress, 
                        long                    lHost)throw();

        unsigned long MakeIpAddr(const wchar_t* szwFullAddress);
        
        //if returns true than we should send second ping
        KLSTD_NOTHROW bool SendFirstPing(
                        KLDB::DbConnectionPtr   pCon,
                        const unavail_host_t&   info)throw();

        KLSTD_NOTHROW void SendSecondPing(
                        KLDB::DbConnectionPtr   pCon,
                        const unavail_host_t& info)throw();

        KLSTD_NOTHROW bool SendPing(
                        KLDB::DbConnectionPtr   pCon,
                        const unavail_host_t& info, 
                        bool& bExact) throw();

        KLSTD_NOTHROW void Publish(
                        KLDB::DbConnectionPtr   pCon,
                        long lHost, 
                        bool bWarning) throw();
	protected:
        lst_second_ping_t       m_lstSecondPing;
        KLSRV::SrvData*         m_pSrvData;
        KLSRV::ServerHelpers*   m_pServerHelpers;
        KLSTD::kltick_t         m_clkLastRun;
        const size_t            m_nPeriod;
        const size_t            m_nExecutePeriod;
        const size_t            m_nPingPeriod;
        const size_t            m_nPingTimeout;
        const size_t            m_nMaxPings;
	};
};

#endif //__KLTASK_CHECKUNAVAIL_H__