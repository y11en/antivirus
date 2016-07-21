// TasksStorageLocalProxy.cpp: implementation of the CTasksStorageLocalProxy class.
//
//////////////////////////////////////////////////////////////////////

#include <std/base/klstd.h>
#include <kca/prts/prtsdefs.h>
#include <kca/prts/taskstorageserver.h>
#include <kca/prts/taskinfo.h>
#include "TasksStorageLocalProxy.h"

namespace KLPRTS {

//////////////////////////////////////////////////////////////////////////
// CTasksStorageLocalProxy

#define CALL_METHOD2(method) \
	KLSTD::CAutoPtr<KLPRTS::TaskStorageServer> pTasksStorageServer; \
	KLPRTS_GetServerObjectByID(m_sServerObjectID, &pTasksStorageServer, true); \
	KLSTD_CHKMEM(pTasksStorageServer); \
	pTasksStorageServer->method

#define CALL_METHOD_WITH_RETVAL2(method, rc) \
	KLSTD::CAutoPtr<KLPRTS::TaskStorageServer> pTasksStorageServer; \
	KLPRTS_GetServerObjectByID(m_sServerObjectID, &pTasksStorageServer, true); \
	KLSTD_CHKMEM(pTasksStorageServer); \
	rc = pTasksStorageServer->method

CTasksStorageLocalProxy::CTasksStorageLocalProxy(const std::wstring& sServerObjectID)
{
	IMPLEMENT_LOCK;

	m_sServerObjectID = sServerObjectID;
	m_itTaskParamsNames = m_arTaskParamsNames.begin();
	KLPRTS_GetServerObjectByID(m_sServerObjectID, &m_pTasksStorageServer, false);
}

CTasksStorageLocalProxy::~CTasksStorageLocalProxy()
{
}

void CTasksStorageLocalProxy::ResetTasksIterator(const KLPRCI::ComponentId &filter, const std::wstring &taskName)
{
	AUTOLOCK;

	m_arTaskParamsNames.clear();
	m_itTaskParamsNames = m_arTaskParamsNames.begin();
	m_pTaskStorageParams = NULL;

	CALL_METHOD2(ResetTasksIterator(filter, taskName, &m_pTaskStorageParams));
	
	m_pTaskStorageParams->GetNames(m_arTaskParamsNames);
	m_itTaskParamsNames = m_arTaskParamsNames.begin();
}

void CTasksStorageLocalProxy::GetTaskByID(const std::wstring &taskId,
									 KLPRCI::ComponentId & id,
									 std::wstring & taskName,
									 bool& bStartOnEvent,
									 KLSCH::Task ** task,
									 KLPAR::Params** params,
									 KLPAR::Params** paramsTaskInfo)
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(params);

	KLSTD::CAutoPtr<KLPAR::Params> parParams;
	CALL_METHOD2(GetTaskByID(taskId, &parParams));

	CTaskInfoInternal TaskInfo(parParams);
	std::wstring sTaskID;
	bStartOnEvent = TaskInfo.IsStartOnEvent();
	*task = TaskInfo.GetTask(sTaskID, id, taskName, params, paramsTaskInfo);
	if (*params) REPLACE_PARAMS_VALUE(*params, c_szwTaskStorageId, StringValue, taskId.c_str());
}

void CTasksStorageLocalProxy::GetTaskByIDWithPolicyApplied(const std::wstring &taskId,
													  KLPRCI::ComponentId & id,
													  std::wstring & taskName,
													  bool& bStartOnEvent,
													  KLSCH::Task ** task,
													  KLPAR::Params** params,
													  KLPAR::Params** paramsTaskInfo)
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(params);

	KLSTD::CAutoPtr<KLPAR::Params> parParams;
	CALL_METHOD2(GetTaskByIDWithPolicyApplied(taskId, &parParams));

	CTaskInfoInternal TaskInfo(parParams);
	std::wstring sTaskID;
	bStartOnEvent = TaskInfo.IsStartOnEvent();
	*task = TaskInfo.GetTask(sTaskID, id, taskName, params, paramsTaskInfo);
	if (*params) REPLACE_PARAMS_VALUE(*params, c_szwTaskStorageId, StringValue, taskId.c_str());
}

bool CTasksStorageLocalProxy::GetNextTask(std::wstring &taskId,
									 KLPRCI::ComponentId &id, 
									 std::wstring &taskName, 
									 bool& bStartOnEvent, 
									 KLSCH::Task **task, 
									 KLPAR::Params** params,
									 KLPAR::Params** paramsTaskInfo)
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(params);

	AUTOLOCK;

	if (m_pTaskStorageParams == NULL) KLSTD_NOINIT(L"KLPRTS::m_pTaskStorageParams");

	bool rc = false;

	if (m_itTaskParamsNames != m_arTaskParamsNames.end())
	{
		KLSTD::CAutoPtr<KLPAR::ParamsValue> pParamsValue = (KLPAR::ParamsValue*)m_pTaskStorageParams->GetValue2((*m_itTaskParamsNames), true);
		KLSTD::CAutoPtr<KLPAR::Params> pTaskParams = pParamsValue->GetValue();
		CTaskInfoInternal TaskInfo(pTaskParams);

		*task = TaskInfo.GetTask(taskId, id, taskName, params, paramsTaskInfo);
		bStartOnEvent = TaskInfo.IsStartOnEvent();

		if (*params) REPLACE_PARAMS_VALUE(*params, c_szwTaskStorageId, StringValue, taskId.c_str());

		m_itTaskParamsNames++;

		rc = true;
	}

	return rc;
}

std::wstring CTasksStorageLocalProxy::AddTask(const KLPRCI::ComponentId &id, 
										 const std::wstring &taskName, 
										 const KLSCH::Task *task, 
										 const KLPAR::Params* params,
										 const KLPAR::Params* paramsTaskInfo)
{
	KLSTD_CHKINPTR(task);

    KLSTD_ASSERT(id.instanceId.empty());
	std::wstring rc;

	CTaskInfoInternal TaskInfo(KLPRTS_TASK_INVALID_ID, id, taskName, task, params, paramsTaskInfo);
	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	std::wstring sTaskID;
	CALL_METHOD_WITH_RETVAL2(AddTask(pTaskParams), sTaskID);

	return sTaskID;
}

void CTasksStorageLocalProxy::UpdateTask(const std::wstring &taskId,
									const KLPRCI::ComponentId& id, 
									const std::wstring& taskName, 
									const KLSCH::Task * task, 
									const KLPAR::Params* params,
									const KLPAR::Params* paramsTaskInfo)
{
	KLSTD_CHKINPTR(task);

    KLSTD_ASSERT(id.instanceId.empty());

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	CTaskInfoInternal TaskInfo(taskId, id, taskName, task, params, paramsTaskInfo);

	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	CALL_METHOD2(UpdateTask(taskId, pTaskParams));
}

void CTasksStorageLocalProxy::DeleteTask(const std::wstring &taskId)
{
	CALL_METHOD2(DeleteTask(taskId));
}

void CTasksStorageLocalProxy::SetTaskStartEvent(const std::wstring &taskId, 
										   const KLPRCI::ComponentId& filter, 
										   const std::wstring& eventType, 
										   const KLPAR::Params* bodyFilter)
{
	CALL_METHOD2(SetTaskStartEvent(taskId, filter, eventType, bodyFilter));
}

void CTasksStorageLocalProxy::GetTaskStartEvent(const std::wstring &taskId, 
										   KLPRCI::ComponentId& filter, 
										   std::wstring& eventType, 
										   KLPAR::Params** bodyFilter)
{
	KLSTD_CHKOUTPTR(bodyFilter);
	CALL_METHOD2(GetTaskStartEvent(taskId, filter, eventType, bodyFilter));
}

void CTasksStorageLocalProxy::AddTaskWithID(const std::wstring &taskId,
									   const KLPRCI::ComponentId &id,
									   const std::wstring &taskName, 
									   const KLSCH::Task *task, 
									   const KLPAR::Params* params,
									   const KLPAR::Params* paramsTaskInfo)
{
	if (taskId.empty()) KLSTD_THROW(KLSTD::STDE_BADPARAM);
	KLSTD_CHKINPTR(task);

    KLSTD_ASSERT(id.instanceId.empty());

	CTaskInfoInternal TaskInfo(KLPRTS_TASK_INVALID_ID, id, taskName, task, params, paramsTaskInfo);
	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);
	ADD_PARAMS_VALUE(pTaskParams, KLPRTS_ADD_TASK_TASK_PREDEFINED_ID, StringValue, taskId.c_str());

	std::wstring sTaskID;
	CALL_METHOD_WITH_RETVAL2(AddTask(pTaskParams), sTaskID);
}

std::wstring CTasksStorageLocalProxy::GetTaskFileName(const std::wstring &taskId)
{
	AUTOLOCK;
	return taskId + TASK_STORAGE_FILE_EXT;
}

// TasksStorage2

bool CTasksStorageLocalProxy::GetNextTask2(TaskInfo& ti)
{
	KLSTD_CHKOUTPTR(&ti.pTask);
	KLSTD_CHKOUTPTR(&ti.parTaskParams);

	AUTOLOCK;

	if (m_pTaskStorageParams == NULL) KLSTD_NOINIT(L"KLPRTS::m_pTaskStorageParams");

	bool rc = false;

	if (m_itTaskParamsNames != m_arTaskParamsNames.end())
	{
		KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
		GET_PARAMS_VALUE(m_pTaskStorageParams, (*m_itTaskParamsNames).c_str(), ParamsValue, PARAMS_T, pTaskParams);

		CTaskInfoInternal taskSerializer(pTaskParams);
		ti.parTaskParams = NULL;
		ti.parTaskInfo = NULL;
		ti.parEventBodyFilter = NULL;
		taskSerializer.GetTask(ti);
		if (ti.parTaskParams) REPLACE_PARAMS_VALUE(ti.parTaskParams, c_szwTaskStorageId, StringValue, ti.wstrTaskId.c_str());

		m_itTaskParamsNames++;

		rc = true;
	}

	return rc;
}

void CTasksStorageLocalProxy::GetTaskByID2(const std::wstring &taskId, TaskInfo& ti)
{
	KLSTD_CHKOUTPTR(&ti.pTask);
	KLSTD_CHKOUTPTR(&ti.parTaskParams);

	KLSTD::CAutoPtr<KLPAR::Params> parParams;
	CALL_METHOD2(GetTaskByID(taskId, &parParams));

	CTaskInfoInternal taskSerializer(parParams);
	ti.parTaskParams = NULL;
	ti.parTaskInfo = NULL;
	ti.parEventBodyFilter = NULL;
	taskSerializer.GetTask(ti);
	if (ti.parTaskParams) REPLACE_PARAMS_VALUE(ti.parTaskParams, c_szwTaskStorageId, StringValue, taskId.c_str());
}

void CTasksStorageLocalProxy::GetTaskByIDWithPolicyApplied2(const std::wstring &taskId, TaskInfo& ti)
{
	KLSTD_CHKOUTPTR(&ti.pTask);
	KLSTD_CHKOUTPTR(&ti.parTaskParams);

	KLSTD::CAutoPtr<KLPAR::Params> parParams;
	CALL_METHOD2(GetTaskByIDWithPolicyApplied(taskId, &parParams));

	CTaskInfoInternal taskSerializer(parParams);
	ti.parTaskParams = NULL;
	ti.parTaskInfo = NULL;
	ti.parEventBodyFilter = NULL;
	taskSerializer.GetTask(ti);
	if (ti.parTaskParams) REPLACE_PARAMS_VALUE(ti.parTaskParams, c_szwTaskStorageId, StringValue, taskId.c_str());
}

std::wstring CTasksStorageLocalProxy::AddTask2(const TaskInfo& ti)
{
	KLSTD_CHKINPTR(ti.pTask);

	std::wstring rc;

	CTaskInfoInternal taskSerializer(ti);
	taskSerializer.SetTaskID(KLPRTS_TASK_INVALID_ID);

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	taskSerializer.Serialize(pTaskParams);

	std::wstring sTaskID;
	CALL_METHOD_WITH_RETVAL2(AddTask(pTaskParams), sTaskID);

	return sTaskID;
}

void CTasksStorageLocalProxy::AddTaskWithID2(const std::wstring &taskId, const TaskInfo& ti)
{
	if (taskId.empty()) KLSTD_THROW(KLSTD::STDE_BADPARAM);
	KLSTD_CHKINPTR(ti.pTask);

	CTaskInfoInternal taskSerializer(ti);
	taskSerializer.SetTaskID(KLPRTS_TASK_INVALID_ID);

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	taskSerializer.Serialize(pTaskParams);

	ADD_PARAMS_VALUE(pTaskParams, KLPRTS_ADD_TASK_TASK_PREDEFINED_ID, StringValue, taskId.c_str());

	std::wstring sTaskID;
	CALL_METHOD_WITH_RETVAL2(AddTask(pTaskParams), sTaskID);
}

void CTasksStorageLocalProxy::UpdateTask2(const TaskInfo& ti)
{
	KLSTD_CHKINPTR(ti.pTask);
	
	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);

	CTaskInfoInternal TaskInfo(ti);
	TaskInfo.Serialize(pTaskParams);
	
	CALL_METHOD2(UpdateTask2(ti.wstrTaskId, pTaskParams));
}

void CTasksStorageLocalProxy::ReplaceTask(const TaskInfo& ti)
{
	KLSTD_CHKINPTR(ti.pTask);
	
	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);

	CTaskInfoInternal TaskInfo(ti);
	TaskInfo.Serialize(pTaskParams);
	
	CALL_METHOD2(ReplaceTask(ti.wstrTaskId, pTaskParams));
}


}

KLCSKCA_DECL void KLPRTS_CreateLocalTasksStorageProxy(const std::wstring& sServerObjectID, 
													  KLPRTS::TasksStorage** ppTasksStorage)
{
	KLSTD_CHKOUTPTR(ppTasksStorage);

	*ppTasksStorage = new KLPRTS::CTasksStorageLocalProxy(sServerObjectID);
	KLSTD_CHKMEM(*ppTasksStorage);
}

KLCSKCA_DECL void KLPRTS_CreateLocalTasksStorageProxy2(const std::wstring& sServerObjectID, 
													   KLPRTS::TasksStorage2** ppTasksStorage)
{
	KLSTD_CHKOUTPTR(ppTasksStorage);

	*ppTasksStorage = new KLPRTS::CTasksStorageLocalProxy(sServerObjectID);
	KLSTD_CHKMEM(*ppTasksStorage);
}
