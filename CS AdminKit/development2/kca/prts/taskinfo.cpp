// TaskInfo.cpp: implementation of the CTaskInfoInternal class.
//
//////////////////////////////////////////////////////////////////////

#include <build/general.h>
#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/err/klerrors.h>
#include <std/base/klbase.h>
#include <std/par/params.h>
#include <transport/tr/errors.h>
#include <std/sch/schedule.h>
#include <std/sch/millisecondsschedule.h>
#include <std/sch/everydayschedule.h>
#include <std/sch/everyweekschedule.h>
#include <std/sch/everymonthschedule.h>
#include <std/sch/secondsschedule.h>
#include <std/sch/weeksdaysschedule.h>
#include <std/sch/task.h>

#include <kca/prts/prxsmacros.h>
#include <kca/prci/componentid.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prts/prtsdefs.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prts/taskinfo.h>

const wchar_t KLCS_MODULENAME[] = L"PRTS:TaskInfo";

namespace KLPRTS {

using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLSCH;

//////////////////////////////////////////////////////////////////////////

CTaskInfoInternal::CTaskInfoInternal(const KLPRTS::TaskInfo& ti)
{
	_CopyTaskInfo(ti, m_ti);
}

CTaskInfoInternal::CTaskInfoInternal(const Params* pTaskParams)
{
	KLSTD_CHKINPTR(pTaskParams);

	m_ti.pTask = KLSCH_CreateTask();
	KLSTD_CHKMEM(m_ti.pTask);

	Deserialize(pTaskParams);
}

CTaskInfoInternal::~CTaskInfoInternal() 
{
	if (m_ti.pTask) delete m_ti.pTask;
}

void CTaskInfoInternal::Serialize(Params* pTaskParams)
{
	KLSTD_CHKMEM(m_ti.pTask);
	KLSTD_CHKINPTR(pTaskParams);

	// Serialize task external params
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_UNIQUE_ID, StringValue, m_ti.wstrTaskId.c_str());
	ADD_CID_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKID, m_ti.cidComponentId);
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_NAME, StringValue, m_ti.wstrTaskName.c_str());
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_ADDITIONAL_PARAMS, ParamsValue, m_ti.parTaskParams);
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_INFO_PARAMS, ParamsValue, m_ti.parTaskInfo);
	ADD_CID_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_FILTER_EVENTS, m_ti.cidPublisher);
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_EVENT_TYPE, StringValue, m_ti.wstrEventType.empty()?KLPRTS::PRTS_TASK_START_EVENT_NONE:m_ti.wstrEventType.c_str());
	if (m_ti.parEventBodyFilter) ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_EVENT_BODY_FILTER, ParamsValue, m_ti.parEventBodyFilter);

	// Serialize Task class settings
	_SerializeTaskClass(pTaskParams);
}

void CTaskInfoInternal::Deserialize(const Params* pTaskParams)
{
	KLSTD_CHKMEM(m_ti.pTask);
	KLSTD_CHKINPTR(pTaskParams);

	// Deserialize task external params
	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_UNIQUE_ID, StringValue, STRING_T, m_ti.wstrTaskId);
	GET_CID_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKID, m_ti.cidComponentId);
	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_NAME, StringValue, STRING_T, m_ti.wstrTaskName);
	GET_CID_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_FILTER_EVENTS, m_ti.cidPublisher);
	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_EVENT_TYPE, StringValue, STRING_T, m_ti.wstrEventType);
	if (m_ti.wstrEventType.empty()) m_ti.wstrEventType = KLPRTS::PRTS_TASK_START_EVENT_NONE;
	GET_PARAMS_VALUE_NO_THROW(pTaskParams, SS_VALUE_NAME_EVENT_BODY_FILTER, ParamsValue, PARAMS_T, m_ti.parEventBodyFilter);
	GET_PARAMS_VALUE_NO_THROW(pTaskParams, SS_VALUE_NAME_TASK_ADDITIONAL_PARAMS, ParamsValue, PARAMS_T, m_ti.parTaskParams);
	GET_PARAMS_VALUE_NO_THROW(pTaskParams, SS_VALUE_NAME_TASK_INFO_PARAMS, ParamsValue, PARAMS_T, m_ti.parTaskInfo);

	// Deserialize Task class settings
	_DeserializeTaskClass(pTaskParams);
}

std::wstring CTaskInfoInternal::GetTaskDisplayName()
{
	std::wstring tdName;
	if ( GetTaskInfo()==NULL ) return tdName;
	GET_PARAMS_VALUE_NO_THROW(GetTaskInfo(), TASK_DISPLAY_NAME, StringValue, STRING_T, tdName );

	return tdName;
}

void CTaskInfoInternal::_CopyTaskInfo(const KLPRTS::TaskInfo& tiSrc, KLPRTS::TaskInfo& tiDst)
{
	KLSTD_CHKINPTR(tiSrc.pTask);

	tiDst.cidComponentId = tiSrc.cidComponentId;
	tiDst.wstrTaskName = tiSrc.wstrTaskName;
	tiDst.wstrTaskId = tiSrc.wstrTaskId;
	tiDst.wstrEventType = tiSrc.wstrEventType;
	KLSTD_CHKOUTPTR(&tiDst.pTask);
	if (tiSrc.parTaskParams) tiSrc.parTaskParams->Clone(&tiDst.parTaskParams);
	if (tiSrc.parTaskInfo) tiSrc.parTaskInfo->Clone(&tiDst.parTaskInfo);
	tiSrc.pTask->Clone(&tiDst.pTask);
}

void CTaskInfoInternal::_SerializeTaskClass(Params* pTaskParams)
{
	KLSTD_CHKMEM(m_ti.pTask);
	KLSTD_CHKINPTR(pTaskParams);

	bool bParam;
	TaskClassId cidParam;
	int nParam;
	time_t timeParam;

	// Serialize task settings
	CHECK_RESULT(m_ti.pTask->GetClassId(cidParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_CLASS_ID, IntValue, cidParam);

	CHECK_RESULT(m_ti.pTask->GetTaskThreadPriority((Task::TaskThreadPriority&)nParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_PRIORITY, IntValue, nParam);
	
	CHECK_RESULT(m_ti.pTask->GetMaxExecutionTime(nParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_MAX_EXEC_TIME, IntValue, nParam);

	CHECK_RESULT(m_ti.pTask->GetDelAfterRunFlag(bParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_DEL_AFTER_RUN_FLAG, BoolValue, bParam);

	CHECK_RESULT(m_ti.pTask->GetLastExecutionTime(timeParam, nParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_LAST_EXEC_TIME, DateTimeValue, timeParam);
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_LAST_EXEC_TIME_SEC, IntValue, nParam);

	CHECK_RESULT(m_ti.pTask->GetStartDeltaTimeout(nParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_START_DELTA, IntValue, nParam);

	CHECK_RESULT(m_ti.pTask->GetPreparativeStartTimeout(nParam))
	ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_PREPARATIVE_START, IntValue, nParam);
	
	// Serialize task scheduler settings
	Schedule* schParam;
	CHECK_RESULT(m_ti.pTask->GetSchedule((const Schedule**)&schParam))
	if (schParam)
	{
		CHECK_RESULT(schParam->GetRunMissedFlag(bParam))
		ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_RUN_MISSED_FLAG, BoolValue, bParam);

		CHECK_RESULT(schParam->GetLifetime(timeParam))
		ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_LIFETIME, DateTimeValue, timeParam);

		CHECK_RESULT(schParam->GetFirstExecutionTime(timeParam, nParam))
		ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_FIRST_EXECUTION_TIME, DateTimeValue, timeParam);
		ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_FIRST_EXECUTION_TIME_SEC, IntValue, nParam);

		CHECK_RESULT(schParam->GetPeriod(nParam));
		ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_MS_PERIOD, IntValue, nParam);

		// Scheduler type specific settings
		nParam = schParam->GetType();
		if ( nParam==SCH_TYPE_WEEKSDAYS )
		{
			ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_TYPE, IntValue, SCH_TYPE_NONE);
		}
		else
		{
			ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_TYPE, IntValue, nParam);
		}

		int nDay, nHours, nMins, nSecs;
		switch(nParam){
			case SCH_TYPE_EVERYDAY:
				CHECK_RESULT(((EverydaySchedule*)schParam)->GetExecutionTime(nHours, nMins, nSecs));
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_ED_HOURS, IntValue, nHours);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_ED_MINS, IntValue, nMins);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_ED_SECS, IntValue, nSecs);
			break;
			case SCH_TYPE_EVERYWEEK:
				CHECK_RESULT(((EveryWeekSchedule*)schParam)->GetExecutionTime(nDay, nHours, nMins, nSecs));
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_DAY, IntValue, nDay);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_HOURS, IntValue, nHours);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_MINS, IntValue, nMins);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_SECS, IntValue, nSecs);
			break;
			case SCH_TYPE_EVERYMONTH:
				CHECK_RESULT(((EveryMonthSchedule*)schParam)->GetExecutionTime(nDay, nHours, nMins, nSecs));
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_DAY, IntValue, nDay);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_HOURS, IntValue, nHours);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_MINS, IntValue, nMins);
				ADD_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_SECS, IntValue, nSecs);
			case SCH_TYPE_WEEKSDAYS:
				KLSTD::CAutoPtr<KLPAR::Params> pWDSchParams;
				KLPAR_CreateParams( &pWDSchParams );
				int daysMask = 0;
				CHECK_RESULT(((WeeksDaysSchedule*)schParam)->GetExecutionTime(nHours, nMins, nSecs, daysMask));
				ADD_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_HOURS, IntValue, nHours);
				ADD_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_MINS, IntValue, nMins);
				ADD_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_SECS, IntValue, nSecs);				
				ADD_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_DAYS, IntValue, daysMask);

				KLSTD::CAutoPtr<KLPAR::Params> pParTaskInfo;

				GET_PARAMS_VALUE_NO_THROW(pTaskParams, SS_VALUE_NAME_TASK_INFO_PARAMS, ParamsValue, PARAMS_T, pParTaskInfo );
				if ( pParTaskInfo==NULL ) KLPAR_CreateParams( &pParTaskInfo );

				REPLACE_PARAMS_VALUE(pParTaskInfo, SS_VALUE_NAME_TASKSCH_WEEKDDAYS, ParamsValue, pWDSchParams);
				REPLACE_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_INFO_PARAMS, ParamsValue, pParTaskInfo);

			break;
		}
	}
}

void CTaskInfoInternal::_DeserializeTaskClass(const Params* pTaskParams)
{
	KLSTD_CHKMEM(m_ti.pTask);
	KLSTD_CHKINPTR(pTaskParams);

	TaskClassId cidParam;
	int nParam;
	bool bParam;
	time_t timeParam;

	// Deserialize task settings
	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_CLASS_ID, IntValue, INT_T, cidParam);
	CHECK_RESULT(m_ti.pTask->SetClassId(cidParam))

	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_PRIORITY, IntValue, INT_T, nParam);
	CHECK_RESULT(m_ti.pTask->SetTaskThreadPriority((Task::TaskThreadPriority)nParam))

	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_MAX_EXEC_TIME, IntValue, INT_T, nParam);
	CHECK_RESULT(m_ti.pTask->SetMaxExecutionTime(nParam))

	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_DEL_AFTER_RUN_FLAG, BoolValue, BOOL_T, bParam);
	CHECK_RESULT(m_ti.pTask->SetDelAfterRunFlag(bParam))

	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_LAST_EXEC_TIME, DateTimeValue, DATE_TIME_T, timeParam);
	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_LAST_EXEC_TIME_SEC, IntValue, INT_T, nParam);
	CHECK_RESULT(m_ti.pTask->SetLastExecutionTime(timeParam, nParam))

	GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASK_START_DELTA, IntValue, INT_T, nParam);
	CHECK_RESULT(m_ti.pTask->SetStartDeltaTimeout(nParam))

	nParam = 0;
	GET_PARAMS_VALUE_NO_THROW(pTaskParams, SS_VALUE_NAME_TASK_PREPARATIVE_START, IntValue, INT_T, nParam);
	CHECK_RESULT(m_ti.pTask->SetPreparativeStartTimeout(nParam))

	// Scheduler type specific settings
	do
	{
		GET_PARAMS_VALUE_NO_THROW(pTaskParams, SS_VALUE_NAME_TASKSCH_TYPE, IntValue, INT_T, nParam);

		CPointer<Schedule> pSchedule;
		int nDay, nHours, nMins, nSecs;
		switch(nParam){
			case SCH_TYPE_MILLISECONS:
				pSchedule = KLSCH_CreateMillisecondsSchedule();
				KLSTD_CHKMEM(pSchedule);
			break;
			case SCH_TYPE_EVERYDAY:
				pSchedule = KLSCH_CreateEverydaySchedule();
				KLSTD_CHKMEM(pSchedule);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_ED_HOURS, IntValue, INT_T, nHours);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_ED_MINS, IntValue, INT_T, nMins);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_ED_SECS, IntValue, INT_T, nSecs);
				CHECK_RESULT(((EverydaySchedule*)(Schedule*)pSchedule)->SetExecutionTime(nHours, nMins, nSecs));
			break;
			case SCH_TYPE_EVERYWEEK:
				pSchedule = KLSCH_CreateEveryWeekSchedule();
				KLSTD_CHKMEM(pSchedule);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_DAY, IntValue, INT_T, nDay);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_HOURS, IntValue, INT_T, nHours);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_MINS, IntValue, INT_T, nMins);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EW_SECS, IntValue, INT_T, nSecs);
				CHECK_RESULT(((EveryWeekSchedule*)(Schedule*)pSchedule)->SetExecutionTime(nDay, nHours, nMins, nSecs));
			break;
			case SCH_TYPE_EVERYMONTH:
				pSchedule = KLSCH_CreateEveryMonthSchedule();
				KLSTD_CHKMEM(pSchedule);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_DAY, IntValue, INT_T, nDay);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_HOURS, IntValue, INT_T, nHours);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_MINS, IntValue, INT_T, nMins);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_EM_SECS, IntValue, INT_T, nSecs);
				CHECK_RESULT(((EveryMonthSchedule*)(Schedule*)pSchedule)->SetExecutionTime(nDay, nHours, nMins, nSecs));
			break;
			case SCH_TYPE_NONE:
				pSchedule = KLSCH_CreateEmptySchedule();
				KLSTD_CHKMEM(pSchedule);

				if ( m_ti.parTaskInfo!=NULL )
				{
					KLSTD::CAutoPtr<KLPAR::Params> pWDSchParams;
					GET_PARAMS_VALUE_NO_THROW(m_ti.parTaskInfo, SS_VALUE_NAME_TASKSCH_WEEKDDAYS, ParamsValue, PARAMS_T, pWDSchParams);
					if ( pWDSchParams!=NULL )
					{
						int daysMask = 0;
						pSchedule = NULL;
						pSchedule = KLSCH_CreateWeeksDaysSchedule();
						KLSTD_CHKMEM(pSchedule);				
						GET_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_HOURS, IntValue, INT_T, nHours);
						GET_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_MINS, IntValue, INT_T, nMins);
						GET_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_SECS, IntValue, INT_T, nSecs);
						GET_PARAMS_VALUE(pWDSchParams, SS_VALUE_NAME_TASKSCH_WD_DAYS, IntValue, INT_T, daysMask);

						CHECK_RESULT(((WeeksDaysSchedule*)(Schedule*)pSchedule)->SetExecutionTime(nHours, nMins, nSecs, daysMask));
					}					
				}
			break;
			case SCH_TYPE_SECONDS:
				pSchedule = KLSCH_CreateSecondsSchedule();
				KLSTD_CHKMEM(pSchedule);
			break;
		}

		if (pSchedule)	// Deserialize task scheduler settings
		{
			KLERR_BEGIN
			{
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_RUN_MISSED_FLAG, BoolValue, BOOL_T, bParam);
				CHECK_RESULT(pSchedule->SetRunMissedFlag(bParam))

				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_LIFETIME, DateTimeValue, DATE_TIME_T, timeParam);
				CHECK_RESULT(pSchedule->SetLifetime(timeParam))

				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_FIRST_EXECUTION_TIME, DateTimeValue, DATE_TIME_T, timeParam);
				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_FIRST_EXECUTION_TIME_SEC, IntValue, INT_T, nParam);
				CHECK_RESULT(pSchedule->SetFirstExecutionTime(timeParam, nParam))

				GET_PARAMS_VALUE(pTaskParams, SS_VALUE_NAME_TASKSCH_MS_PERIOD, IntValue, INT_T, nParam);
				CHECK_RESULT(pSchedule->SetPeriod(nParam));
			}
			KLERR_ENDT(3)
		}

		CHECK_RESULT(m_ti.pTask->SetSchedule(pSchedule));
	} 
	while(false);
}

/////////////////////////////////////////////////////////////////////////////////////
// Obsolete

CTaskInfoInternal::CTaskInfoInternal(const std::wstring UniqueTaskID, 
					 const KLPRCI::ComponentId &id, 
					 const std::wstring &taskName, 
					 const KLSCH::Task *task, 
					 const Params* parAdditionalTaskParams,
					 const Params* parTaskInfo)
{
	KLPRTS::TaskInfo ti;
	ti.wstrTaskId = UniqueTaskID;
	ti.cidComponentId = id;
	ti.wstrTaskName = taskName;
	ti.pTask = const_cast<KLSCH::Task *>(task);
	ti.parTaskParams = const_cast<KLPAR::Params*>(parAdditionalTaskParams);
	ti.parTaskInfo = const_cast<KLPAR::Params*>(parTaskInfo);
	_CopyTaskInfo(ti, m_ti);
}

void CTaskInfoInternal::SetTaskStartEvent(const KLPRCI::ComponentId& filter, const std::wstring& eventType, const KLPAR::Params* bodyFilter)
{
	m_ti.parEventBodyFilter = NULL;
	if (bodyFilter) bodyFilter->Clone(&m_ti.parEventBodyFilter);
	m_ti.cidPublisher = filter;
	m_ti.wstrEventType = eventType;
}

void CTaskInfoInternal::GetTaskStartEvent(KLPRCI::ComponentId& filter, std::wstring& eventType, KLPAR::Params** bodyFilter)
{
	KLSTD_CHKOUTPTR(bodyFilter);
	filter = m_ti.cidPublisher;
	eventType = m_ti.wstrEventType;
	if (m_ti.parEventBodyFilter) m_ti.parEventBodyFilter->Clone(bodyFilter);
}

bool CTaskInfoInternal::IsStartOnEvent()
{
	return (!m_ti.wstrEventType.empty() && m_ti.wstrEventType.compare(PRTS_TASK_START_EVENT_NONE) != 0);
}

KLSCH::Task* CTaskInfoInternal::GetTask(std::wstring& taskId, KLPRCI::ComponentId &id, std::wstring &taskName, Params** pparTaskAdditionalParams, Params** pparTaskInfo)
{
	KLPRTS::TaskInfo ti;
	_CopyTaskInfo(m_ti, ti);
	
	id = ti.cidComponentId;
	taskName = ti.wstrTaskName;
	taskId = ti.wstrTaskId;
	if (pparTaskAdditionalParams) ti.parTaskParams.CopyTo(pparTaskAdditionalParams);
	if (pparTaskInfo) m_ti.parTaskInfo.CopyTo(pparTaskInfo);
	return ti.pTask;
}

};

