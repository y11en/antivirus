/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	task_mthr_base.h
 * \author	Andrew Kazachkov
 * \date	31.08.2005 14:53:50
 * \brief	
 * 
 */

#ifndef __KLSRV_TASK_MTHR_BASE_H__
#define __KLSRV_TASK_MTHR_BASE_H__

#include <kca/basecomp/cmpbase.h>

namespace KLSRV
{	
    class CTaskMultiThreadedBase
		:	public KLBASECOMP::TaskBaseNoImpersonation
	{
	public:
		CTaskMultiThreadedBase();
		virtual ~CTaskMultiThreadedBase();
    protected:
		void Initialize(
						long						        idTask,
                        KLSRV::SrvData*                     pSrvData,
                        KLBASECOMP::ComponentBase*          pComponent);
        virtual int RunWorker(KLTP::ThreadsPool::WorkerId wId);		
		virtual bool OnExecute();
		virtual void OnControl(KLPRCI::TaskAction action);
    protected:
        virtual void DoPass() = 0;
        virtual long GetRequiredThreadCount();
        virtual bool CheckThreadCondition();
        virtual bool StartProcessing() = 0;
    protected:
        void SetFinishedProcessing();
        bool GetFinishedProcessing();
        void SetPassPersiod(long lPassPeriod);
        void SetEnabled(bool bEnabled);
        bool GetEnabled();
        void SetRestartFlag();
        void SetLastRun(time_t tmLastRun);
    public:
        KLSRV::SrvData*             m_pSrvData;
    protected:
        long                        m_lMaxThreads;
	private:
        KLTP::ThreadsPool::WorkerId m_idWorker;
        volatile long               m_lThreads;        
        long                        m_lPeriod;        
        KLSTD::CReentProtect        m_rpInitialize;
        KLSTD::ObjectLock           m_lckInitialize;
        KLSTD::kltick_t             m_clkLastRun;
        long                        m_lPasPeriod;
        volatile bool               m_bInitialized;
        volatile bool               m_bDisabled, m_bMustRestart;
	};
};


#endif //__KLSRV_TASK_MTHR_BASE_H__
