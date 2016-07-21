/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TSK/grouptaskcommonsrv.h
 * \author Михаил Кармазин
 * \date 11:59 04.01.2003
 * \brief Общие объявления для серверной части групповых задач.
 */

#ifndef __TSK_GROUP_TASK_COMMON_SRV_H__
#define __TSK_GROUP_TASK_COMMON_SRV_H__

#include <std/sch/taskparams.h>
#include <std/base/klstd.h>
#include <kca/prci/componentid.h>
#include <kca/prts/prxsmacros.h>
#include <map>
#include <srvp/tsk/grouptaskcontrol.h>
#include <srvp/grp/groups.h>
#include <srvp/tsk/klri_const.h>

// ID interval from 1 to MAX_SYSTEM_TASK_ID reserved for internal tasks, which inserted 
// into GroupTaskControlImpl::m_pScheduler during initialization. Upper bound of that 
// interval provided by identity settings of [tsk_task].[task_id] field.
#define MAX_SYSTEM_TASK_ID                  99
#define REMOTE_INSTALL_CLEANER_TASK_ID      1  // ID shall be between 1 and MAX_SYSTEM_TASK_ID - see comments above.
#define KLSRV_UPD_PRODVER_LIST_TASK_ID      2  // ID shall be between 1 and MAX_SYSTEM_TASK_ID - see comments above.
#define KLSRV_EVP_CLEAN_EVENTS_TASK_ID      3  // ID shall be between 1 and MAX_SYSTEM_TASK_ID - see comments above.
#define KLSRV_TSK_RI_RUN_MISSED_TASK_ID     4  // ID shall be between 1 and MAX_SYSTEM_TASK_ID - see comments above.
#define REMOTE_INSTALL_SUB_PKG_TASK_ID      5  // ID shall be between 1 and MAX_SYSTEM_TASK_ID - see comments above.

#define USER_NAME_FIELD_SIZE        256
#define HOST_NAME_FIELD_SIZE        256
#define DOMAIN_NAME_FIELD_SIZE      256
#define STATE_DESCR_FIELD_SIZE      1000
#define EVENT_DESCR_FIELD_SIZE      1000
#define NULL_PATH_ID				0
#define NULL_DEADLINE               -1
#define CURRENT_PATH_ID             -1
#define CURRENT_DEADLINE            0

#define GTN_UPD_PRODVER_LIST			L"Update Products List"

namespace KLSRV
{
    class GroupTaskControlImpl;

    class CGroupTaskState : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        CGroupTaskState()
        {
            m_bTaskRunning = false;
            m_bShallStop = false;
            m_bSuspended = false;
            m_bEvenIfAlreadySucceeded = false;
            m_bEvenIfTryCountExceed = false;
            m_bManualStart = false;
			m_bPreparativeStart = false;
            m_pThread = NULL;
            m_nOverallTaskFailCount = 0;
			m_bForMissedHostsOnly = false;
        }

        bool m_bTaskRunning;
        bool m_bShallStop;
        bool m_bSuspended;
        bool m_bEvenIfAlreadySucceeded;
        bool m_bEvenIfTryCountExceed;
        bool m_bManualStart;
		bool m_bPreparativeStart;
		bool m_bForMissedHostsOnly;

        std::wstring m_wstrDomainToPerform;
        std::wstring m_wstrHostToPerform;

		std::wstring m_wstrStartId;

        KLSTD::Thread* m_pThread;
        int m_nOverallTaskFailCount;
    };

    class GroupTaskParams : public KLSCH::TaskParams
    {
    public:
        void* m_pOwner;
        int m_nTaskId;
        std::wstring m_wstrFileIdentity;
        KLPRCI::ComponentId m_compId;
        std::wstring m_taskName;
        KLSTD::CAutoPtr<KLPAR::Params> m_parTaskParams;
        KLSTD::CAutoPtr<KLPAR::Params> m_parTaskInfo;
        KLSTD::CAutoPtr<CGroupTaskState> m_pGroupTaskState;
		GroupTaskParams* m_pMainGroupTaskParams;

        GroupTaskParams( void* pOwner,
                         int nTaskId,
                         const std::wstring & wstrFileIdentity,
                         KLPRCI::ComponentId compId,
                         std::wstring taskName,
                         KLSTD::CAutoPtr<KLPAR::Params> parTaskParams,
                         KLSTD::CAutoPtr<KLPAR::Params> parTaskInfo,
                         KLSTD::CAutoPtr<CGroupTaskState> pGroupTaskState = NULL,
						 GroupTaskParams* pMainGroupTaskParams = NULL)
        {
            m_pOwner = pOwner;
            m_nTaskId = nTaskId;
            m_wstrFileIdentity = wstrFileIdentity;
            m_compId = compId;
            m_taskName = taskName;
            m_parTaskParams = parTaskParams;
            m_parTaskInfo = parTaskInfo;
			m_pMainGroupTaskParams = pMainGroupTaskParams;

            if( ! pGroupTaskState )
            {
                m_pGroupTaskState.Attach( new CGroupTaskState );
                KLSTD_CHKMEM( m_pGroupTaskState );
            } else
            {
                m_pGroupTaskState = pGroupTaskState;
            }
        }

	    KLSCH::TaskParams *Clone()
	    {
            GroupTaskParams* pGroupTaskParams =
                new GroupTaskParams( m_pOwner, m_nTaskId, m_wstrFileIdentity,
                     m_compId, m_taskName, m_parTaskParams, m_parTaskInfo, m_pGroupTaskState, m_pMainGroupTaskParams);
            KLSTD_CHKMEM( pGroupTaskParams );

		    return pGroupTaskParams;
	    }

        std::wstring GetDisplayName()
        {  
            if( m_nTaskId == REMOTE_INSTALL_CLEANER_TASK_ID )
            {
                return L"Task cleaner";
            } 
			else if( m_nTaskId == KLSRV_UPD_PRODVER_LIST_TASK_ID )
			{
                return GTN_UPD_PRODVER_LIST;
			}
			else if( m_parTaskInfo != NULL )
            {
                std::wstring wstrDisplayName;
                GET_PARAMS_VALUE( m_parTaskInfo, KLPRTS::TASK_DISPLAY_NAME, StringValue, STRING_T, wstrDisplayName );
                return wstrDisplayName;
            } else
            {
                return L"Unknown task name";
            }            
        }
    };
    
    /* class GroupTaskIterator : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        GroupTaskIterator()
        {
            m_nCurrentIndex = 0;
        }

        std::vector<int> m_vectTaskId;
        int m_nCurrentIndex;
    }; */
}

#endif // __TSK_GROUP_TASK_COMMON_SRV_H__
