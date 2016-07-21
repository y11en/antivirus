/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	TaskZeroVcount.h
 * \author	Andrew Kazachkov
 * \date	20.02.2004 14:42:00
 * \brief	
 * 
 */

#ifndef __KL_TASKZEROVCOUNT_H__
#define __KL_TASKZEROVCOUNT_H__


#include <kca/basecomp/cmpbase.h>

namespace KLSRV
{	
    class CTaskZeroVCount 
        :   public KLBASECOMP::TaskBaseNoImpersonation
    {
    public:
	    CTaskZeroVCount();
	    virtual ~CTaskZeroVCount();
	    void Initialize(
                        long                idTask,
                        KLSRV::SrvData*             pSrvData,
                        KLBASECOMP::ComponentBase*  pComponent,
                        KLPAR::Params*      pData);
		const wchar_t* GetTaskName();
		void OnControl(KLPRCI::TaskAction action);
		bool OnExecute();
        static const wchar_t c_szwTaskName[];
    protected:
        KLSRV::SrvData*                 m_pSrvData;
        KLSTD::CAutoPtr<KLPAR::Params>  m_pTaskData;
        KLSPLEX::CtxPtr                 m_pCtx;
    };
};

#endif //__KL_TASKZEROVCOUNT_H__