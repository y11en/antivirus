/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	Task_Broadcaster.h
 * \author	Andrew Kazachkov
 * \date	27.03.2003 14:00:36
 * \brief	
 * 
 */

#if !defined(AFX_TASK_BROADCASTER_H__9179591A_E8A9_4709_835E_3BED92E03F82__INCLUDED_)
#define AFX_TASK_BROADCASTER_H__9179591A_E8A9_4709_835E_3BED92E03F82__INCLUDED_

#include <kca/basecomp/cmpbase.h>
#include "./srvinst.h"

namespace KLSRV
{	
    class CTask_Broadcaster
		:	public KLSRV::CServerTaskBase
	{
	public:
		CTask_Broadcaster();
		virtual ~CTask_Broadcaster();
		void Initialize(
						long									idTask,
						CServerInstance*						pInst,
						KLPAR::Params*							pData,
                        KLSRVBCASTER::GlobalSrvBroadcaster*     pGlobalSrvCaster);
        virtual int RunWorker(KLTP::ThreadsPool::WorkerId wId);
		const wchar_t* GetTaskName(){return c_szwTaskName;};
		bool OnExecute();
		void OnControl(KLPRCI::TaskAction action);
		static const wchar_t c_szwTaskName[];
	protected:
        KLTP::ThreadsPool::WorkerId m_idWorker;
        KLSTD::CAutoPtr<KLSRVBCASTER::GlobalSrvBroadcaster> m_pGlobalSrvCaster;
        volatile long       m_lThreads;
        CServerInstance*    m_pInst;
	};
};

#endif // !defined(AFX_TASK_BROADCASTER_H__9179591A_E8A9_4709_835E_3BED92E03F82__INCLUDED_)
