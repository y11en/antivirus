// TaskForceSync.h: interface for the CTaskForceSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKFORCESYNC_H__0605BD89_735A_4C6B_9CB0_85ADC1B93F35__INCLUDED_)
#define AFX_TASKFORCESYNC_H__0605BD89_735A_4C6B_9CB0_85ADC1B93F35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <std/base/klstd.h>
#include <kca/basecomp/cmpbase.h>
#include <server/srvinst/srvinstdata.h>

namespace KLSRV
{	
    class CTaskForceSync: public KLBASECOMP::TaskBaseNoImpersonation
    {
    public:
	    CTaskForceSync();
	    virtual ~CTaskForceSync();

	    void Initialize(
                        long                        idTask,
                        KLBASECOMP::ComponentBase*  pCmpBase,
                        KLSTD::KLBaseQI*            pInst,
                        KLSRV::SrvData*             pSrvData,
                        KLPAR::Params*              pData);
		const wchar_t* GetTaskName();
		void OnControl(KLPRCI::TaskAction action);
		bool OnExecute();
        static const wchar_t c_szwTaskName[];
    protected:
        KLSTD::KLBaseQI*                m_pSrvInst;
        KLSTD::CAutoPtr<KLPAR::Params>  m_pTaskData;
        KLSRV::SrvData*                 m_pSrvData;
    };
};

#endif // !defined(AFX_TASKFORCESYNC_H__0605BD89_735A_4C6B_9CB0_85ADC1B93F35__INCLUDED_)
