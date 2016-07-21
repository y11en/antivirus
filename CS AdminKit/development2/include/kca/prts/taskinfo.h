// TaskInfo.h: interface for the CTaskInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKINFO_H__F13CBD19_D305_470E_84A2_F8A8E158AF05__INCLUDED_)
#define AFX_TASKINFO_H__F13CBD19_D305_470E_84A2_F8A8E158AF05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <kca/prts/tasksstorage.h>

namespace KLPRTS {

class CTaskInfoInternal
{
	KLPRTS::TaskInfo m_ti;

public:
	CTaskInfoInternal(const KLPRTS::TaskInfo& ti);
	CTaskInfoInternal(const KLPAR::Params* pTaskParams);
	~CTaskInfoInternal();

	void Serialize(KLPAR::Params* pTaskParams);
	void Deserialize(const KLPAR::Params* pTaskParams);
	void GetTask(KLPRTS::TaskInfo& ti) {
		_CopyTaskInfo(m_ti, ti);
	}
	std::wstring GetTaskID() const {
		return m_ti.wstrTaskId;
	}
	void SetTaskID(std::wstring wstrTaskId){
		m_ti.wstrTaskId = wstrTaskId;
	}
	KLPRCI::ComponentId GetTaskCID() const {
		return m_ti.cidComponentId;
	}
	std::wstring GetTaskName() const {
		return m_ti.wstrTaskName;
	}	
	KLSTD::CAutoPtr<KLPAR::Params> GetTaskParams() const {
		return m_ti.parTaskParams;
	}
	void SetTaskParams(KLPAR::Params* parTaskParams){
		m_ti.parTaskParams = parTaskParams;
	}
	KLSTD::CAutoPtr<KLPAR::Params> GetTaskInfo() const {
		return m_ti.parTaskInfo;
	}
	void SetTaskInfo(KLPAR::Params* parTaskInfo){
		m_ti.parTaskInfo = parTaskInfo;
	}
	KLSCH::Task* GetTask() const {	// Do not destroy this pointer
		return m_ti.pTask;
	}
	void SetTask(const KLSCH::Task* pTask){
		KLSTD_CHKINPTR(pTask);
		if (m_ti.pTask) 
		{
			delete m_ti.pTask;
			m_ti.pTask = NULL;
		}
		pTask->Clone(&m_ti.pTask);
	}
	std::wstring GetTaskDisplayName();

protected:
	void _SerializeTaskClass(KLPAR::Params* pTaskParams);
	void _DeserializeTaskClass(const KLPAR::Params* pTaskParams);
	void _CopyTaskInfo(const KLPRTS::TaskInfo& tiSrc, KLPRTS::TaskInfo& tiDst);

// Obsolete
public:
	CTaskInfoInternal(const std::wstring UniqueTaskID, 
					  const KLPRCI::ComponentId &id, 
					  const std::wstring &taskName, 
					  const KLSCH::Task *task, 
					  const KLPAR::Params* parAdditionalTaskParams,
					  const KLPAR::Params* parTaskInfo);

	void SetTaskStartEvent(const KLPRCI::ComponentId& filter, const std::wstring& eventType, const KLPAR::Params* bodyFilter);
	void GetTaskStartEvent(KLPRCI::ComponentId& filter, std::wstring& eventType, KLPAR::Params** bodyFilter);

	static void CopyTask(const KLSCH::Task* pSrcTask, KLSCH::Task** ppDstTask);
	KLSCH::Task* GetTask(std::wstring& taskId, KLPRCI::ComponentId &id, std::wstring &taskName, KLPAR::Params** pparTaskAdditionalParams, KLPAR::Params** pparTaskInfo);
	bool IsStartOnEvent();
};

};

#endif // !defined(AFX_TASKINFO_H__F13CBD19_D305_470E_84A2_F8A8E158AF05__INCLUDED_)

