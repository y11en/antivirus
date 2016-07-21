#include <build/general.h>
#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/err/klerrors.h>
#include <std/base/klbase.h>
#include <std/par/params.h>
#include <std/par/paramsi.h>
#include <transport/tr/errors.h>
#include <std/sch/schedule.h>
#include <std/sch/task.h>
#include <transport/tr/transport.h>
#include <transport/tr/transportproxy.h>
#include <kca/prci/componentid.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/helpersi.h>
#include "soapH.h"
#include <kca/prts/prtsdefs.h>
#include <kca/prts/prxsmacros.h>
#include <kca/prts/taskinfo.h>
#include <kca/prts/taskstorageserver.h>
#include "TasksStorageProxyImpl.h"

const wchar_t KLCS_MODULENAME[] = L"PRTS:TaskStorage";

namespace KLPRTS {

using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLSCH;

//////////////////////////////////////////////////////////////////////////

CTasksStorageProxy::CTasksStorageProxy(const std::wstring& sServerObjectID, 
									   const KLPRCI::ComponentId& cidLocalComponent, 
									   const KLPRCI::ComponentId& cidComponent, 
									   bool bCloseConnectionAtEnd)
{
	INIT_TRANSPORT(cidLocalComponent, cidComponent);
	m_sServerObjectID = sServerObjectID;
	m_itTaskParamsNames = m_arTaskParamsNames.begin();
	m_bCloseConnectionAtEnd = bCloseConnectionAtEnd;
}

CTasksStorageProxy::~CTasksStorageProxy()
{
	KLERR_BEGIN
	{
		if (m_bCloseConnectionAtEnd)
			KLTR_GetTransport()->CloseClientConnection(m_sDECLTR_LocalComponentName.c_str(), 
													   m_sDECLTR_ConnectionName.c_str());
	}
	KLERR_ENDT(0)
}

void CTasksStorageProxy::ResetTasksIterator(const KLPRCI::ComponentId &filter, const std::wstring &taskName)
{
	m_arTaskParamsNames.clear();
	m_itTaskParamsNames = m_arTaskParamsNames.begin();
	m_pTaskStorageParams = NULL;
	
	BEGIN_SOAP_COMMAND(LOCK)
	{
		SOAPComponentId scid;
		CID_TO_SOAP_CID(filter, scid);

		KLPRTS_RESET_TASKS_ITERATOR_RESPONSE rcl;

		CALL_SOAP_COMMAND(soap_call_KLPRTS_ResetTasksIterator(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), scid, (wchar_t*)taskName.c_str(), rcl), rcl.rcError);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(rcl.parTaskListParamsOut, &m_pTaskStorageParams);
		m_pTaskStorageParams->GetNames(m_arTaskParamsNames);
		m_itTaskParamsNames = m_arTaskParamsNames.begin();
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::GetTaskByID(const std::wstring &taskId,
									 KLPRCI::ComponentId & id,
									 std::wstring & taskName,
									 bool& bStartOnEvent,
									 KLSCH::Task ** task,
									 KLPAR::Params** params,
									 KLPAR::Params** paramsTaskInfo)
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(params);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_GET_TASK_BY_ID_RESPONSE rcl;
		soap_default_KLPRTS_GET_TASK_BY_ID_RESPONSE(GET_SOAP, &rcl);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_GetTaskByID(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), rcl), rcl.rcError);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(rcl.parTaskParamsOut, &parParams);

		CTaskInfoInternal TaskInfo(parParams);
		std::wstring sTaskID;
		bStartOnEvent = TaskInfo.IsStartOnEvent();
		*task = TaskInfo.GetTask(sTaskID, id, taskName, params, paramsTaskInfo);
		if (*params) REPLACE_PARAMS_VALUE(*params, c_szwTaskStorageId, StringValue, taskId.c_str());
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::GetTaskByIDWithPolicyApplied( const std::wstring &taskId,
													   KLPRCI::ComponentId & id,
													   std::wstring & taskName,
													   bool& bStartOnEvent,
													   KLSCH::Task ** task,
													   KLPAR::Params** params,
													   KLPAR::Params** paramsTaskInfo)
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(params);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE rcl;
		soap_default_KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE(GET_SOAP, &rcl);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_GetTaskByIDWithPolicyApplied(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), rcl), rcl.rcError);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(rcl.parTaskParamsOut, &parParams);

		CTaskInfoInternal TaskInfo(parParams);
		std::wstring sTaskID;
		bStartOnEvent = TaskInfo.IsStartOnEvent();
		*task = TaskInfo.GetTask(sTaskID, id, taskName, params, paramsTaskInfo);
		if (*params) REPLACE_PARAMS_VALUE(*params, c_szwTaskStorageId, StringValue, taskId.c_str());
	}
	END_SOAP_COMMAND(UNLOCK)
}

bool CTasksStorageProxy::GetNextTask(std::wstring &taskId,
									 KLPRCI::ComponentId &id, 
									 std::wstring &taskName, 
									 bool& bStartOnEvent, 
									 KLSCH::Task **task, 
									 KLPAR::Params** params,
									 KLPAR::Params** paramsTaskInfo)
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(params);

	if (m_pTaskStorageParams == NULL) KLSTD_NOINIT(L"KLPRTS::m_pTaskStorageParams");

	bool rc = false;

	LOCK
	{
		if (m_itTaskParamsNames != m_arTaskParamsNames.end())
		{
			CAutoPtr<ParamsValue> pParamsValue = (ParamsValue*)m_pTaskStorageParams->GetValue2((*m_itTaskParamsNames), true);
			CAutoPtr<Params> pTaskParams = pParamsValue->GetValue();
			CTaskInfoInternal TaskInfo(pTaskParams);

			*task = TaskInfo.GetTask(taskId, id, taskName, params, paramsTaskInfo);
			bStartOnEvent = TaskInfo.IsStartOnEvent();

			if (*params) REPLACE_PARAMS_VALUE(*params, c_szwTaskStorageId, StringValue, taskId.c_str());

			m_itTaskParamsNames++;

			rc = true;
		}
	}
	UNLOCK

	return rc;
}

std::wstring CTasksStorageProxy::AddTask(const KLPRCI::ComponentId &id, 
										 const std::wstring &taskName, 
										 const KLSCH::Task *task, 
										 const KLPAR::Params* params,
										 const KLPAR::Params* paramsTaskInfo)
{
	KLSTD_CHKINPTR(task);

	std::wstring rc;

	CTaskInfoInternal TaskInfo(KLPRTS_TASK_INVALID_ID, id, taskName, task, params, paramsTaskInfo);
	CAutoPtr<Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_ADD_TASK_RESPONSE rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_AddTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), parparParams, rcl), rcl.rcError);

		rc = rcl.TaskID;
	}
	END_SOAP_COMMAND(UNLOCK)

	return rc;
}

void CTasksStorageProxy::UpdateTask(const std::wstring &taskId,
									const KLPRCI::ComponentId& id, 
									const std::wstring& taskName, 
									const KLSCH::Task * task, 
									const KLPAR::Params* params,
									const KLPAR::Params* paramsTaskInfo)
{
	KLSTD_CHKINPTR(task);
	
	CAutoPtr<Params> pTaskParams;
	CTaskInfoInternal TaskInfo(taskId, id, taskName, task, params, paramsTaskInfo);

	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		param_error rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_UpdateTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), parparParams, rcl), rcl);
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::DeleteTask(const std::wstring &taskId)
{
	BEGIN_SOAP_COMMAND(LOCK)
	{
		param_error rcl;

		CALL_SOAP_COMMAND(soap_call_KLPRTS_DeleteTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), rcl), rcl);
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::SetTaskStartEvent(const std::wstring &taskId, 
										   const KLPRCI::ComponentId& filter, 
										   const std::wstring& eventType, 
										   const KLPAR::Params* bodyFilter)
{
	BEGIN_SOAP_COMMAND(LOCK)
	{
		SOAPComponentId scidEventFilter;
		CID_TO_SOAP_CID(filter, scidEventFilter);

		param_error rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, bodyFilter, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_SetTaskStartEvent(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), scidEventFilter, (wchar_t*)eventType.c_str(), parparParams, rcl), rcl);
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::GetTaskStartEvent(const std::wstring &taskId, 
										   KLPRCI::ComponentId& filter, 
										   std::wstring& eventType, 
										   KLPAR::Params** bodyFilter)
{
	KLSTD_CHKOUTPTR(bodyFilter);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		SOAPComponentId scidEventFilter;
		CID_TO_SOAP_CID(filter, scidEventFilter);

		KLPRTS_GET_TASK_START_EVENT_RESPONSE rcl;

		rcl.sEventType = NULL;
		rcl.cidEventFilter.componentName = NULL;
		rcl.cidEventFilter.productName = NULL;
		rcl.cidEventFilter.version = NULL;
		rcl.cidEventFilter.instanceId = NULL;

		CALL_SOAP_COMMAND(soap_call_KLPRTS_GetTaskStartEvent(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), rcl), rcl.rcError);

		filter.componentName = rcl.cidEventFilter.componentName;
		filter.productName = rcl.cidEventFilter.productName;
		filter.version = rcl.cidEventFilter.version;
		filter.instanceId = rcl.cidEventFilter.instanceId;

		eventType = rcl.sEventType;

		KLPAR::ParamsFromSoap(rcl.parBodyFilterOut, bodyFilter);
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::AddTaskWithID(const std::wstring &taskId,
									   const KLPRCI::ComponentId &id,
									   const std::wstring &taskName, 
									   const KLSCH::Task *task, 
									   const KLPAR::Params* params,
									   const KLPAR::Params* paramsTaskInfo)
{
	if (taskId.empty()) KLSTD_THROW(STDE_BADPARAM);
	KLSTD_CHKINPTR(task);

	CTaskInfoInternal TaskInfo(KLPRTS_TASK_INVALID_ID, id, taskName, task, params, paramsTaskInfo);
	CAutoPtr<Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	ADD_PARAMS_VALUE(pTaskParams, KLPRTS_ADD_TASK_TASK_PREDEFINED_ID, StringValue, taskId.c_str());

	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_ADD_TASK_RESPONSE rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_AddTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), parparParams, rcl), rcl.rcError);
	}
	END_SOAP_COMMAND(UNLOCK)
}

std::wstring CTasksStorageProxy::GetTaskFileName(const std::wstring &taskId)
{
	return taskId + TASK_STORAGE_FILE_EXT;
}

// TasksStorage2

bool CTasksStorageProxy::GetNextTask2(TaskInfo& ti)
{
	if (m_pTaskStorageParams == NULL) KLSTD_NOINIT(L"KLPRTS::m_pTaskStorageParams");

	bool rc = false;

	LOCK
	{
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

			++m_itTaskParamsNames;

			rc = true;
		}
	}
	UNLOCK

	return rc;
}

void CTasksStorageProxy::GetTaskByID2(const std::wstring &taskId, TaskInfo& ti)
{
	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_GET_TASK_BY_ID_RESPONSE rcl;
		soap_default_KLPRTS_GET_TASK_BY_ID_RESPONSE(GET_SOAP, &rcl);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_GetTaskByID(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), rcl), rcl.rcError);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(rcl.parTaskParamsOut, &parParams);

		CTaskInfoInternal taskSerializer(parParams);
		ti.parTaskParams = NULL;
		ti.parTaskInfo = NULL;
		ti.parEventBodyFilter = NULL;
		taskSerializer.GetTask(ti);
		if (ti.parTaskParams) REPLACE_PARAMS_VALUE(ti.parTaskParams, c_szwTaskStorageId, StringValue, taskId.c_str());
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::GetTaskByIDWithPolicyApplied2(const std::wstring &taskId, TaskInfo& ti)
{
	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE rcl;
		soap_default_KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE(GET_SOAP, &rcl);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_GetTaskByIDWithPolicyApplied(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)taskId.c_str(), rcl), rcl.rcError);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(rcl.parTaskParamsOut, &parParams);

		CTaskInfoInternal taskSerializer(parParams);
		ti.parTaskParams = NULL;
		ti.parTaskInfo = NULL;
		ti.parEventBodyFilter = NULL;
		taskSerializer.GetTask(ti);
		if (ti.parTaskParams) REPLACE_PARAMS_VALUE(ti.parTaskParams, c_szwTaskStorageId, StringValue, taskId.c_str());
	}
	END_SOAP_COMMAND(UNLOCK)
}

std::wstring CTasksStorageProxy::AddTask2(const TaskInfo& ti)
{
	KLSTD_CHKINPTR(ti.pTask);

	std::wstring rc;

	CTaskInfoInternal taskSerializer(ti);
	taskSerializer.SetTaskID(KLPRTS_TASK_INVALID_ID);

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	taskSerializer.Serialize(pTaskParams);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_ADD_TASK_RESPONSE rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_AddTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), parparParams, rcl), rcl.rcError);

		rc = rcl.TaskID;
	}
	END_SOAP_COMMAND(UNLOCK)

	return rc;
}

void CTasksStorageProxy::AddTaskWithID2(const std::wstring &taskId, const TaskInfo& ti)
{
	if (taskId.empty()) KLSTD_THROW(KLSTD::STDE_BADPARAM);
	KLSTD_CHKINPTR(ti.pTask);

	CTaskInfoInternal taskSerializer(ti);
	taskSerializer.SetTaskID(KLPRTS_TASK_INVALID_ID);

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	taskSerializer.Serialize(pTaskParams);

	ADD_PARAMS_VALUE(pTaskParams, KLPRTS_ADD_TASK_TASK_PREDEFINED_ID, StringValue, taskId.c_str());

	BEGIN_SOAP_COMMAND(LOCK)
	{
		KLPRTS_ADD_TASK_RESPONSE rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_AddTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), parparParams, rcl), rcl.rcError);
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::UpdateTask2(const TaskInfo& ti)
{
	KLSTD_CHKINPTR(ti.pTask);
	
	CTaskInfoInternal TaskInfo(ti);

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		param_error rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_UpdateTask2(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)ti.wstrTaskId.c_str(), parparParams, rcl), rcl);
	}
	END_SOAP_COMMAND(UNLOCK)
}

void CTasksStorageProxy::ReplaceTask(const TaskInfo& ti)
{
	KLSTD_CHKINPTR(ti.pTask);
	
	CTaskInfoInternal TaskInfo(ti);

	KLSTD::CAutoPtr<KLPAR::Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	BEGIN_SOAP_COMMAND(LOCK)
	{
		param_error rcl;
		param__params parparParams;

		KLPAR::ParamsForSoap(GET_SOAP, pTaskParams, parparParams);

		CALL_SOAP_COMMAND(soap_call_KLPRTS_ReplaceTask(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)ti.wstrTaskId.c_str(), parparParams, rcl), rcl);
	}
	END_SOAP_COMMAND(UNLOCK)
}

}

///////////////////////////////////////////////////////////////////////////////

KLCSKCA_DECL void KLPRTS_CreateTasksStorageProxy(const std::wstring& sServerObjectID,
											   const KLPRCI::ComponentId& cidLocalComponent, 
											   const KLPRCI::ComponentId& cidComponent, 
											   KLPRTS::TasksStorage** ppTasksStorage,
											   bool bCloseConnectionAtEnd)
{
	KLSTD_CHKOUTPTR(ppTasksStorage);

	*ppTasksStorage = new KLPRTS::CTasksStorageProxy(sServerObjectID, cidLocalComponent, cidComponent, bCloseConnectionAtEnd);
	KLSTD_CHKMEM(*ppTasksStorage);
}

KLCSKCA_DECL void KLPRTS_CreateTasksStorageProxy2( const std::wstring& sServerObjectID,
												   const KLPRCI::ComponentId& cidLocalComponent, 
												   const KLPRCI::ComponentId& cidComponent, 
												   KLPRTS::TasksStorage2** ppTasksStorage2,
												   bool bCloseConnectionAtEnd)
{
	KLSTD_CHKOUTPTR(ppTasksStorage2);

	*ppTasksStorage2 = new KLPRTS::CTasksStorageProxy(sServerObjectID, cidLocalComponent, cidComponent, bCloseConnectionAtEnd);
	KLSTD_CHKMEM(*ppTasksStorage2);
}

///////////////////////////////////////////////////////////////////////////////

KLCSKCA_DECL void KLPRTS_SerializeTaskToParams(
            const std::wstring UniqueTaskID, 
            const KLPRCI::ComponentId &id, 
            const std::wstring &taskName, 
            const KLSCH::Task *task, 
            const KLPAR::Params* parTaskParams,
            const KLPAR::Params* parTaskInfo,
            KLPAR::Params* parDestination,
            const std::wstring &wstrStartEvent /* = L"" */,
            const KLPRCI::ComponentId &cidStartEventFilter /* = KLPRCI::ComponentId() */,
            const KLPAR::Params* parStartEventFilter /* = NULL */ )
{
    KLSTD_ASSERT( parDestination != NULL );
    
    KLPRTS::CTaskInfoInternal taskInfo( UniqueTaskID, id, taskName, task, parTaskParams, parTaskInfo );

    if( ! wstrStartEvent.empty() )
    {
        taskInfo.SetTaskStartEvent(cidStartEventFilter, wstrStartEvent, parStartEventFilter);
    }

	taskInfo.Serialize( parDestination );
}

KLCSKCA_DECL void KLPRTS_DeserializeTaskFromParams(
            KLPAR::Params* parSource,
            std::wstring& UniqueTaskID,
            KLPRCI::ComponentId& id,
            std::wstring& taskName,
            KLSCH::Task** task,
            KLPAR::Params** parTaskParams,
            KLPAR::Params** parTaskInfo,
            std::wstring * pwstrStartEvent /* = NULL */,
            KLPRCI::ComponentId * pcidStartEventFilter /* = NULL */,
            KLPAR::Params** parStartEventFilter /* = NULL */ )
{
	KLSTD_CHKOUTPTR(task);
	KLSTD_CHKOUTPTR(parTaskParams);
    
    KLPRTS::CTaskInfoInternal taskInfo( parSource );

	*task = taskInfo.GetTask(UniqueTaskID, id, taskName, parTaskParams, parTaskInfo);

    if( pwstrStartEvent && pcidStartEventFilter && parStartEventFilter )
    {
        taskInfo.GetTaskStartEvent( *pcidStartEventFilter, *pwstrStartEvent, parStartEventFilter);
    }
}
