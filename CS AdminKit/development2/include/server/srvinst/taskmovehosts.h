// TaskMoveHosts.h: interface for the CTaskMoveHosts class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKMOVEHOSTS_H__4EBDED9E_5C9F_40A3_93B3_8BD75D307A11__INCLUDED_)
#define AFX_TASKMOVEHOSTS_H__4EBDED9E_5C9F_40A3_93B3_8BD75D307A11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <kca/basecomp/cmpbase.h>
namespace KLSRV
{	
    class CTaskMoveHosts : public KLBASECOMP::TaskBaseNoImpersonation
    {
    public:
	    CTaskMoveHosts();
	    virtual ~CTaskMoveHosts();
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
        KLSRV::SrvData*                 m_pSrvData;
        KLSTD::CAutoPtr<KLPAR::Params>  m_pTaskData;
    };
};

#endif // !defined(AFX_TASKMOVEHOSTS_H__4EBDED9E_5C9F_40A3_93B3_8BD75D307A11__INCLUDED_)
