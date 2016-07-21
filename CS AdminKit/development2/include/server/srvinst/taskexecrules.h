#ifndef __KL_TASKEXECRULES_H__
#define __KL_TASKEXECRULES_H__

#include <kca/basecomp/cmpbase.h>
namespace KLSRV
{	
    class CTaskExecRules : public KLBASECOMP::TaskBaseNoImpersonation
    {
    public:
	    CTaskExecRules();
	    virtual ~CTaskExecRules();
	    void Initialize(
                        long                        idTask,
                        KLSRV::SrvData*             pSrvData,
                        KLBASECOMP::ComponentBase*  pComponent,
                        KLPAR::Params*              pData);
		const wchar_t* GetTaskName();
		void OnControl(KLPRCI::TaskAction action);
		bool OnExecute();
        static const wchar_t c_szwTaskName[];
    protected:
        virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId );
    protected:
        KLSRV::SrvData*                 m_pSrvData;
        KLSTD::CAutoPtr<KLPAR::Params>  m_pTaskData;
        long                            m_lPercent, m_lPercentPrevious;
        KLTP::ThreadsPool::WorkerId     m_idWorker;
    };
};

#endif //__KL_TASKEXECRULES_H__
