#ifndef _TASKS_STORAGE_PROXY_IMPL_H_
#define _TASKS_STORAGE_PROXY_IMPL_H_

#include <kca/prts/tasksstorage.h>

namespace KLPRTS {

class CTasksStorageProxy : public KLSTD::KLBaseImpl<TasksStorage2>
{
public:
	CTasksStorageProxy(const std::wstring& sServerObjectID, 
					   const KLPRCI::ComponentId& cidLocalComponent, 
					   const KLPRCI::ComponentId& cidComponent, 
					   bool bCloseConnectionAtEnd);
	virtual ~CTasksStorageProxy();
	
// TasksStorage2 interaface

	virtual void ResetTasksIterator(const KLPRCI::ComponentId &filter, const std::wstring &taskName);
	virtual void GetTaskByID(const std::wstring &taskId,
							 KLPRCI::ComponentId & id,
							 std::wstring & taskName,
							 bool& bStartOnEvent,
							 KLSCH::Task ** task,
							 KLPAR::Params** params,
							 KLPAR::Params** paramsTaskInfo);
    virtual void GetTaskByIDWithPolicyApplied (const std::wstring &taskId,
											   KLPRCI::ComponentId & id,
											   std::wstring & taskName,
											   bool& bStartOnEvent,
											   KLSCH::Task ** task,
											   KLPAR::Params** params,
											   KLPAR::Params** paramsTaskInfo);
	virtual bool GetNextTask(std::wstring &taskId,
							 KLPRCI::ComponentId &id, 
							 std::wstring &taskName, 
							 bool& bStartOnEvent, 
							 KLSCH::Task **task, 
							 KLPAR::Params** params,
							 KLPAR::Params** paramsTaskInfo);
	virtual std::wstring AddTask(const KLPRCI::ComponentId &id, 
								 const std::wstring &taskName, 
								 const KLSCH::Task *task, 
								 const KLPAR::Params* params,
								 const KLPAR::Params* paramsTaskInfo);
	virtual void UpdateTask(const std::wstring &taskId,
							const KLPRCI::ComponentId& id, 
							const std::wstring& taskName, 
							const KLSCH::Task * task, 
							const KLPAR::Params* params,
							const KLPAR::Params* paramsTaskInfo);
	virtual void DeleteTask(const std::wstring &taskId);
	virtual void SetTaskStartEvent(const std::wstring &taskId, 
								   const KLPRCI::ComponentId& filter, 
								   const std::wstring& eventType, 
								   const KLPAR::Params* bodyFilter);
	virtual void GetTaskStartEvent(const std::wstring &taskId, 
								   KLPRCI::ComponentId& filter, 
								   std::wstring& eventType, 
								   KLPAR::Params** bodyFilter);
	virtual void AddTaskWithID(const std::wstring &taskId,
							   const KLPRCI::ComponentId &id,
							   const std::wstring &taskName, 
							   const KLSCH::Task *task, 
							   const KLPAR::Params* params,
							   const KLPAR::Params* paramsTaskInfo);
	virtual std::wstring GetTaskFileName(const std::wstring &taskId);

// TasksStorage2 interface

	virtual bool GetNextTask2(TaskInfo& ti);
	virtual void GetTaskByID2(const std::wstring &taskId, TaskInfo& ti);
	virtual void GetTaskByIDWithPolicyApplied2(const std::wstring &taskId, TaskInfo& ti);
	virtual std::wstring AddTask2(const TaskInfo& ti);
	virtual void AddTaskWithID2(const std::wstring &taskId, const TaskInfo& ti);
	virtual void UpdateTask2(const TaskInfo& ti);
	virtual void ReplaceTask(const TaskInfo& ti);

protected:
	DECLARE_TRANSPORT;
	DECLARE_LOCK;

	bool m_bCloseConnectionAtEnd;
	// Iteration filters
	TASK_PARAMS_NAMES m_arTaskParamsNames;
	TASK_PARAMS_NAMES::iterator m_itTaskParamsNames;
	KLSTD::CAutoPtr<KLPAR::Params> m_pTaskStorageParams;
	std::wstring m_sServerObjectID;
};

}

#endif			// _TASKS_STORAGE_PROXY_IMPL_H_
