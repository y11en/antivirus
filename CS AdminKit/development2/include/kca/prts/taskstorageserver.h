// TaskStorageServer.h: interface for the CTaskStorageServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKSTORAGESERVER_H__22C7106F_BC94_4645_A582_F9902C20D329__INCLUDED_)
#define AFX_TASKSTORAGESERVER_H__22C7106F_BC94_4645_A582_F9902C20D329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tasksstorage.h"
#include <transport/ev/eventsource.h>
#include <kca/prci/componentinstance.h>
#include "prxsmacros.h"

namespace KLPRTS {

const wchar_t TASK_STORAGE_FILE_EXT[] = L".klt";
const wchar_t TASK_STORAGE_FILE_MASK[] = L"*.klt";
const wchar_t KLPRTS_ADD_TASK_TASK_PREDEFINED_ID[] = L"KLPRTS_ADD_TASK_TASK_PREDEFINED_ID";
	
/*!
* \brief Реализация класса TasksStorageServer
*/
class TaskStorageServer : public KLSTD::KLBase
{
public:
	virtual void ResetTasksIterator(const KLPRCI::ComponentId &cidFilter,
									const std::wstring &sTaskNameFilter,
									KLPAR::Params** ppTaskStorageParams) = 0;
	virtual void GetTaskByID(const std::wstring TaskID, KLPAR::Params** ppTaskParams) = 0;
    virtual void GetTaskByIDWithPolicyApplied(const std::wstring TaskID, KLPAR::Params** ppTaskParams) = 0;
	
	virtual std::wstring AddTask(const KLPAR::Params* pTaskToAddParams) = 0;
	virtual void UpdateTask(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams) = 0;
	virtual void UpdateTask2(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams){};
	virtual void ReplaceTask(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams){};
	virtual void DeleteTask(const std::wstring TaskId) = 0;
	virtual void SetTaskStartEvent( const std::wstring TaskId,
									const KLPRCI::ComponentId& filter,
									const std::wstring& eventType,
									const KLPAR::Params* bodyFilter) = 0;
	virtual void GetTaskStartEvent( const std::wstring TaskId,
									KLPRCI::ComponentId& filter,
									std::wstring& eventType,
									KLPAR::Params** bodyFilter) = 0;
	virtual std::wstring& getID() = 0;
	virtual void PutTask(const std::wstring TaskID, const KLPAR::Params* pTaskParams) = 0;
	virtual std::wstring GetHostId() const = 0;
};

class CTaskStorageServer : public KLSTD::KLBaseImpl<TaskStorageServer>
{
public:
	std::wstring m_sServerObjectID;

	CTaskStorageServer(const bool bLocalCopyOfServerStorage, const std::wstring& sServerObjectID, const bool bDontPublishEvents );
	virtual ~CTaskStorageServer();

	void Initialize(const KLPRCI::ComponentId& cidMaster, const std::wstring& sTasksStorageFilePath, 
		const bool bAddAgentConnection, const wchar_t *wstrHostName );

	void ResetTasksIterator(const KLPRCI::ComponentId &cidFilter, const std::wstring &sTaskNameFilter, KLPAR::Params** ppTaskStorageParams);
	void GetTaskByID(const std::wstring TaskID, KLPAR::Params** ppTaskParams);
	void GetTaskByIDWithPolicyApplied(const std::wstring TaskID, KLPAR::Params** ppTaskParams);
	std::wstring AddTask(const KLPAR::Params* pTaskToAddParams);
	void UpdateTask(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams);
	void UpdateTask2(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams);
	void ReplaceTask(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams);
	void DeleteTask(const std::wstring TaskId);
	void SetTaskStartEvent(const std::wstring TaskId, const KLPRCI::ComponentId& filter, const std::wstring& eventType, const KLPAR::Params* bodyFilter);
	void GetTaskStartEvent(const std::wstring TaskId, KLPRCI::ComponentId& filter, std::wstring& eventType, KLPAR::Params** bodyFilter);
	std::wstring& getID();
	void PutTask(const std::wstring TaskID, const KLPAR::Params* pTaskParams);
	std::wstring GetHostId() const;

	static void Create();
	static void Deinitialize();
	static void SubscriptionsStorageEventCallback(const KLPRCI::ComponentId & subscriber, 
												  const KLPRCI::ComponentId & publisher,
												  const KLEV::Event *event,
												  void *context);
	static bool IsLocalTask(const std::wstring TaskID)
	{
		return (wcsncmp(TaskID.c_str(), KLPRTS_LOCAL_TASK_ATTRIBUTE, KLSTD_COUNTOF(KLPRTS_LOCAL_TASK_ATTRIBUTE) - 1) == 0);
	}

public:
	KLCSKCA_DECL static bool CompareComponentID(const KLPRCI::ComponentId& CIDFilter, const KLPRCI::ComponentId& CID2);
protected:
	DECLARE_LOCK;

	bool m_bLocalCopyOfServerStorage;
	bool m_bInitialized;
	bool m_bDontPublishEvents;
	std::wstring m_sTasksStorageFilePath, m_sTaskStorageUniqueID;
	KLPRCI::ComponentId m_cidMaster;
	std::vector<KLEV::SubscriptionId> m_arEventSourceSubscriptionSIDs;
	bool m_bDeleteAgentConnection;
    //<-- Changed by andkaz 10.11.2003 18:33:23
    KLSTD::CAutoPtr<KLPRCI::ComponentInstance> m_PrivateInstance;
    // -->    	
	std::wstring	m_wstrHostId;

	// true - if storage can modify this task
	void _CheckIsOperationPermitted(const std::wstring TaskID)
	{
		if (m_bLocalCopyOfServerStorage && !IsLocalTask(TaskID))
		{
			// local storage and server task - not permitted
			KLSTD_THROW(KLSTD::STDE_NOTPERM);	
		}
	}
	void _PublishEvent(const std::wstring sEventName, const std::wstring TaskID, bool bPublishInternalEvent = true);
	void _UnsubscribeAll();
	void _GetTaskByID(std::wstring TaskID, KLPAR::Params** ppTaskParams);
	void _GetNames(std::vector<std::wstring>& arNames);
	void _ReadTask(std::wstring sTaskStorageFileName, KLPAR::Params** ppTaskParams);
	void _UpdateTaskInternal(const std::wstring TaskID, const KLPAR::Params* pTaskToUpdateParams, bool bReplace,
		bool notifyTaskModify = true );
};

} // namespace 

KLCSKCA_DECL void KLPRTS_GetServerObjectByID(const std::wstring&	sServerObjectID,
								KLPRTS::TaskStorageServer** ppTasksStorageServer,
								bool bThrowNotExistException);
KLCSKCA_DECL void KLPRTS_AddServerObject(KLPRTS::TaskStorageServer*	pServer);

DECLARE_MODULE_INIT_DEINIT2( KLCSKCA_DECL, KLPRTS );

#endif // !defined(AFX_TASKSTORAGESERVER_H__22C7106F_BC94_4645_A582_F9902C20D329__INCLUDED_)

// Local Variables:
// mode: C++
// End:

