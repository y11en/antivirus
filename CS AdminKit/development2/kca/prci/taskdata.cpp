
#ifdef _WIN32
    #define STRICT
    #include <windows.h>
#endif

#include "std/base/klbase.h"
#include "std/trc/trace.h"
#include "std/err/klerrors.h"
#include "std/thr/thread.h"
#include "kca/prci/componentinstance.h"
#include "kca/prci/errors.h"
#include "kca/prts/tasksstorage.h"
#include <kca/prci/errlocids.h>
#include <std/err/errloc_intervals.h>
#include "./taskdata.h"

#define KLCS_MODULENAME L"KLPRCI"

namespace KLPRCI
{
    task_data_t::task_data_t(
					int					nID,
					long				lTimeout,
					const std::wstring& wstrAsyncID,
					const std::wstring& wstrName,
					KLPAR::Params*		pParams)
		:	m_nID(nID)
		,	m_wstrAsyncID(wstrAsyncID)
        ,	m_wstrName(wstrName)
        ,   m_nState(TASK_CREATED)
        ,   m_nCompletion(0)
        ,	m_lTimeout(lTimeout)
        ,   m_pUserData(NULL)			
		,	m_pParams(pParams)
        ,   m_bIsInvisible(false)
        ,   m_lThreadId(0)
        ,	m_pParams2(NULL)
	{
        if(pParams && pParams->DoesExist(KLPRTS::c_szwTaskStorageId))
            const_cast<std::wstring&>(m_wstrTaskStorageId)=
                KLPAR::GetStringValue(pParams, KLPRTS::c_szwTaskStorageId);        
        m_bIsInvisible=m_wstrTaskStorageId.empty();
        if(pParams && pParams->DoesExist(KLPRCI::c_szwTaskIsInvisible))
            const_cast<bool&>(m_bIsInvisible)=
                KLPAR::GetBoolValue(pParams, KLPRCI::c_szwTaskIsInvisible);
        if(!m_pParams)
            KLPAR_CreateParams(&m_pParams);
    };

    task_data_t::task_data_t(const task_data_t& x)
		:	m_nID(x.m_nID)
		,	m_wstrAsyncID(x.m_wstrAsyncID)
        ,	m_wstrName(x.m_wstrName)
        ,   m_nState(x.m_nState)
        ,   m_nCompletion(x.m_nCompletion)
        ,	m_lTimeout(x.m_lTimeout)
        ,   m_pUserData(x.m_pUserData)
		,	m_pParams(x.m_pParams)
        ,   m_wstrTaskStorageId(x.m_wstrTaskStorageId)
        ,   m_bIsInvisible(x.m_bIsInvisible)
        ,   m_lThreadId(x.m_lThreadId)
        ,	m_pParams2(x.m_pParams2)
	{;};

    void CTasks::addTask(long idTask, task_data_t& taskdata)
    {
        tasksmap_t::iterator itTask=m_Tasks.find(idTask);
        if(itTask!=m_Tasks.end())
            KLSTD_THROW_BADPARAM(idTask);

        if(!taskdata.m_pParams)
            KLPAR_CreateParams(&taskdata.m_pParams);

        if(!taskdata.m_wstrTaskStorageId.empty())
        {
            strset_t::iterator it=m_TsIds.find(taskdata.m_wstrTaskStorageId);
            if(it != m_TsIds.end())
            {
                KLSTD::CAutoPtr<KLPAR::StringValue> p_strDisplayName;
            KLERR_BEGIN
                const wchar_t* path[] = 
                {
                    KLPRTS::c_szwTaskParamTaskInfo,
                    NULL
                };                    
                KLPAR_GetValue( taskdata.m_pParams, 
                                path, 
                                KLPRTS::TASK_DISPLAY_NAME, 
                                (KLPAR::Value**)&p_strDisplayName);
                KLSTD_ASSERT_THROW(p_strDisplayName);
                KLPAR_CHKTYPE(p_strDisplayName, STRING_T, KLPRTS::TASK_DISPLAY_NAME);
            KLERR_ENDT(1)
                KLERR_LOCTHROW1(
                    KLERR::ErrLocAdapt(
                        PRCIEL_TASK_IS_RUNNING,
                        KLERR_LOCMOD_PRCI,
                        p_strDisplayName
                            ?   p_strDisplayName->GetValue()
                            :   L"?"),
                    KLCS_MODULENAME,
                    ERR_TSTASK_ALREADY_RUNNING, 
                    taskdata.m_wstrTaskStorageId.c_str());
            };
            m_TsIds.insert(taskdata.m_wstrTaskStorageId);
        }
        m_Tasks.insert(tasksmap_t::value_type(idTask, taskdata));
        KLSTD_TRACE2(
                    4,
                    L"CTasks::addTask, idTask = %u, TaskStorageId='%ls'\n",
                    idTask,
                    taskdata.m_wstrTaskStorageId.c_str());
    }

    bool CTasks::removeTask(long idTask)
    {
        bool bResult=false;
		tasksmap_t::iterator it=m_Tasks.find(idTask);
		if(it!=m_Tasks.end())
		{
            KLSTD_TRACE2(
                4,
                L"CTasks::removeTask, idTask = %u, TaskStorageId='%ls'\n",
                idTask,
                it->second.m_wstrTaskStorageId.c_str());

            if(!it->second.m_wstrTaskStorageId.empty())
                m_TsIds.erase(it->second.m_wstrTaskStorageId);
			m_Tasks.erase(it);
			bResult=true;
		}
        else
        {
            KLSTD_TRACE1(
                4,
                L"CTasks::removeTask FAILED!!!, idTask = %u\n",
                idTask);
        }
        return bResult;
    }

	task_data_t&  CTasks::findTask(long idTask)
    {
		tasksmap_t::iterator it=m_Tasks.find(idTask);
		if(it==m_Tasks.end())
            KLSTD_THROW(KLSTD::STDE_NOTFOUND);
		return it->second;
    }

    const task_data_t&  CTasks::findTask(long idTask) const
    {
		tasksmap_t::const_iterator it=m_Tasks.find(idTask);
		if(it==m_Tasks.end())
            KLSTD_THROW(KLSTD::STDE_NOTFOUND);
		return it->second;
    }

    void CTasks::getTasks(std::vector<long> & ids) const
    {
        ids.reserve(m_Tasks.size());
        for(tasksmap_t::const_iterator it=m_Tasks.begin(); it != m_Tasks.end(); ++it)
            ids.push_back(it->first);
    }

    template <class t, class T> static void DoAddValue(
                        KLPAR::Params*      pData,
                        const std::wstring& wstrName,
                        t                   val,
                        T*)
    {
        KLSTD_ASSERT(pData!=NULL);
        if(wstrName.empty())
            pData->DeleteValue(wstrName, false);
        else
        {
            KLSTD::CAutoPtr<T> pValue;
            KLPAR::CreateValue(val, &pValue);
            pData->ReplaceValue(wstrName, pValue);
        };
    }

    void CTasks::reload_begin(long idTask, KLPAR::Params* pData)
    {
        task_data_t& taskdata=findTask(idTask);
        KLSTD::CAutoPtr<KLPAR::Params> pNewData;
        if(pData)
            pNewData=pData;
        else
            KLPAR_CreateParams(&pNewData);

        DoAddValue(pNewData, KLPRTS::c_szwTaskStorageId, taskdata.m_wstrTaskStorageId.c_str(), (KLPAR::StringValue*)NULL);
        DoAddValue(pNewData, KLPRCI::c_szwTaskAsyncId, taskdata.m_wstrAsyncID.c_str(), (KLPAR::StringValue*)NULL);
        DoAddValue(pNewData, KLPRCI::c_szwTaskIsInvisible, taskdata.m_bIsInvisible, (KLPAR::BoolValue*)NULL);
        
        taskdata.m_pParams2=pNewData;
        taskdata.m_lThreadId=KLSTD_GetCurrentThreadId();

        KLSTD_TRACE2(
            4,
            L"CTasks::reload_begin, idTask = %u, TaskStorageId='%ls'\n",
            idTask,
            taskdata.m_wstrTaskStorageId.c_str());

    };

    KLSTD_NOTHROW void CTasks::reload_commit(long idTask) throw()
    {
        KLERR_BEGIN
            task_data_t& taskdata=findTask(idTask);
            taskdata.m_pParams=taskdata.m_pParams2;
            taskdata.m_lThreadId=0;
            taskdata.m_pParams2=NULL;
            ;
            KLSTD_TRACE2(
                4,
                L"CTasks::reload_commit, idTask = %u, TaskStorageId='%ls'\n",
                idTask,
                taskdata.m_wstrTaskStorageId.c_str());
        KLERR_ENDT(1)
    }

    KLSTD_NOTHROW void CTasks::reload_rollback(long idTask) throw()
    {
        KLERR_BEGIN            
            task_data_t& taskdata=findTask(idTask);
            taskdata.m_lThreadId=0;
            taskdata.m_pParams2=NULL;
            KLSTD_TRACE2(
                4,
                L"CTasks::reload_rollback, idTask = %u, TaskStorageId='%ls'\n",
                idTask,
                taskdata.m_wstrTaskStorageId.c_str());
        KLERR_ENDT(1)
    };
};
