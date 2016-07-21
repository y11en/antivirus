/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	taskdata.h
 * \author	Andrew Kazachkov
 * \date	15.08.2003 13:03:50
 * \brief	
 * 
 */

#ifndef __KL_TASKDATA_H__
#define __KL_TASKDATA_H__

#include <set>
#include <map>
#include <vector>


namespace KLPRCI
{
	struct task_data_t
	{
		const int						m_nID;
		const std::wstring				m_wstrAsyncID;
		const std::wstring				m_wstrName;
        const std::wstring              m_wstrTaskStorageId;
        bool                            m_bIsInvisible;
		TaskState						m_nState;
		int								m_nCompletion;
		long							m_lTimeout;
		void*							m_pUserData;
		KLSTD::CAutoPtr<KLPAR::Params>	m_pParams;        

        KLSTD::CAutoPtr<KLPAR::Params>	m_pParams2;
        long                            m_lThreadId;
        
        KLSTD::CAutoPtr<KLPAR::Params>	m_pTaskStateExtra;

		task_data_t(
					int					nID,
					long				lTimeout,
					const std::wstring& wstrAsyncID,
					const std::wstring& wstrName,
					KLPAR::Params*		pParams);

		task_data_t(const task_data_t& x);
	};

    class CTasks
    {
    public:
        void addTask(long idTask, task_data_t& taskdata);
        bool removeTask(long idTask);
	    task_data_t&  findTask(long idTask);
        const task_data_t&  findTask(long idTask) const;
        size_t size()const {return m_Tasks.size();};
        void getTasks(std::vector<long> & ids) const;
        void reload_begin(long idTask, KLPAR::Params* pNewData);
        KLSTD_NOTHROW void reload_commit(long idTask) throw();
        KLSTD_NOTHROW void reload_rollback(long idTask) throw();
    protected:
        typedef std::map<long, task_data_t> tasksmap_t;
        typedef std::set<std::wstring> strset_t;
        tasksmap_t  m_Tasks;
        strset_t    m_TsIds;
    };
}

#endif //__KL_TASKDATA_H__
