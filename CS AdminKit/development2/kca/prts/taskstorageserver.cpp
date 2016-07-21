	// TaskStorageServer.cpp: implementation of the CTaskStorageServer class.
//
//////////////////////////////////////////////////////////////////////

#include <build/general.h>
#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/err/klerrors.h>
#include <std/par/params.h>
#include <std/par/paramsi.h>
#include <std/conv/klconv.h>
#include <transport/tr/errors.h>
#include <std/sch/schedule.h>
#include <std/sch/task.h>
#include <kca/prci/componentid.h>
#include <kca/prci/prci_const.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/errors.h>
#include <kca/prss/helpersi.h>
#include <transport/tr/errors.h>
#include "soapH.h"
#include <transport/ev/event.h>
#include <transport/ev/eventsource.h>
#include <avp/parray.h>
#include <transport/tr/transport.h>
#include <transport/avt/accesscheck.h>

#include <kca/prts/prtsdefs.h>
#include <kca/prts/prxsmacros.h>
#include <kca/prts/taskinfo.h>
#include <kca/prts/taskstorageserver.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prts/tsk_list.h>
#include <kca/prci/cmp_helpers.h>
#include <kca/aud/auditsource.h>



#if defined(__unix) || defined(N_PLAT_NLM) // $%%^@$&^@%
#include <iostream>
#include <fstream>
#include <map>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "../../std/conv/_conv.h"
#endif

#include "TasksStorageLocalProxy.h"
#include "modulestub/prtssoapstub.h"

//////////////////////////////////////////////////////////////////////////

namespace KLPRTS {

using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;
using namespace KLSCH;
using namespace KLPRCI;

//////////////////////////////////////////////////////////////////////////

const wchar_t KLCS_MODULENAME[] = L"PRTS:TaskStorage";

typedef std::vector< std::wstring > TASK_PARAMS_NAMES;
const int PRTS_DELETE_STORAGE_TIMEOUT = KLSTD_INFINITE;

const wchar_t KLPRTS_INTERNAL_EVT_TASK_STORAGE_CHANGED[] = L"PRTS_TSChangedInternal";
const wchar_t EVTPAR_TSC_SUBTYPE[] = L"PRTS_EVTPAR_TSC_SUBTYPE";
typedef enum {
	TSC_TASK_ADDED,
	TSC_TASK_DELETED,
	TSC_TASK_UPDATED
};
const wchar_t EVTPAR_TSC_TASK_NAME[] = L"PRTS_EVTPAR_TSC_TASK_NAME";
const wchar_t EVTPAR_TSC_TASK_STORAGE_UID[] = L"PRTS_EVTPAR_TSC_TASK_STORAGE_UID";

    //ss format for tasks
    KLPRSS::ss_format_t g_oFormat;

    KLCSKCA_DECL void SetSsFormatId(KLPRSS::ss_formatid_t id)
    {
        g_oFormat.id = id;
    };

    KLCSKCA_DECL KLPRSS::ss_formatid_t GetSsFormatId()
    {
        return g_oFormat.id;
    };


#define AUDIT_NOTIFY_TASK_MODIFIED( TaskInfo, action )\
{\
	bool isGroupTask = false;\
	bool isGlobalTask = false;\
	bool notifyFlag = true;\
	if ( TaskInfo.GetTaskInfo()!=NULL && TaskInfo.GetTaskInfo()->DoesExist(TASK_GROUPID_FOR_GROUP_TASKS) )\
	{\
		int groupId = (-1);\
		GET_PARAMS_VALUE_NO_THROW(TaskInfo.GetTaskInfo(), TASK_GROUPID_FOR_GROUP_TASKS, IntValue, INT_T, groupId);\
		if ( groupId==(-1) ) isGlobalTask = true;\
		isGroupTask = true;\
	}\
	if ( TaskInfo.GetTaskParams()!=NULL && TaskInfo.GetTaskParams()->DoesExist(c_szwTaskIsInvisible) )\
	{\
		bool isVisible = false;\
		GET_PARAMS_VALUE_NO_THROW(TaskInfo.GetTaskParams(), c_szwTaskIsInvisible, BoolValue, BOOL_T, isVisible);\
		if ( isVisible ) notifyFlag = false;\
	}\
	if ( notifyFlag )\
	{\
		if ( isGroupTask  )\
		{\
			if ( isGlobalTask )\
			{\
				KLAUD_NOTIFY_GLOBALTASK_MODIFIED( action, TaskInfo.GetTaskDisplayName(), TaskInfo.GetTaskCID().productName,\
					TaskInfo.GetTaskCID().version );\
			}else{\
				KLAUD_NOTIFY_GROUPTASK_MODIFIED( action, TaskInfo.GetTaskDisplayName(), TaskInfo.GetTaskCID().productName,\
					TaskInfo.GetTaskCID().version );\
			}\
		}\
		else\
		{\
			KLAUD_NOTIFY_TASK_MODIFIED( action, TaskInfo.GetTaskDisplayName(), TaskInfo.GetTaskCID().productName,\
				TaskInfo.GetTaskCID().version );\
		}\
	}\
}

//////////////////////////////////////////////////////////////////////////

/*!
* \brief –еализаци€ класса TasksStorageServer
*/

#define MAKE_SURE_THAT_INITIALIZED	if (!m_bInitialized) KLSTD_NOINIT(L"KLPRTS::CTaskStorageServer");

CTaskStorageServer::CTaskStorageServer(const bool bLocalCopyOfServerStorage, const std::wstring& sServerObjectID, const bool bDontPublishEvents)
{
	IMPLEMENT_LOCK;

	m_sServerObjectID = sServerObjectID;
	m_bInitialized = false;
	m_bLocalCopyOfServerStorage = bLocalCopyOfServerStorage;
	m_bDeleteAgentConnection = false;
	m_bDontPublishEvents = bDontPublishEvents;
	m_sTaskStorageUniqueID = KLSTD_CreateGUIDString();	
}

CTaskStorageServer::~CTaskStorageServer()
{
    if(m_bDeleteAgentConnection && m_PrivateInstance)
    {
        KLERR_IGNORE(m_PrivateInstance->SetState(KLPRCI::STATE_SHUTTING_DOWN));
        m_PrivateInstance=NULL;
    };
	_UnsubscribeAll();
}

void CTaskStorageServer::Initialize(const KLPRCI::ComponentId& cidMaster, const std::wstring& sTasksStorageFilePath, 
									const bool bAddAgentConnection, const wchar_t *wstrHostName )
{
	m_cidMaster = cidMaster;

	m_sTasksStorageFilePath = sTasksStorageFilePath;
	if (!m_sTasksStorageFilePath.empty() &&
		(m_sTasksStorageFilePath[m_sTasksStorageFilePath.size() - 1] == L'\\' ||
		 m_sTasksStorageFilePath[m_sTasksStorageFilePath.size() - 1] == L'/'))
	{
		m_sTasksStorageFilePath.resize(m_sTasksStorageFilePath.size() - 1);
	}

	KLSTD_CreateDirectory(m_sTasksStorageFilePath.c_str(), false);
	if(!KLSTD_IfExists(m_sTasksStorageFilePath.c_str()))
		KLSTD_THROW(STDE_NOTFOUND);

	if (bAddAgentConnection && cidMaster.productName != KLPRSS_PRODUCT_CORE)
	{
		KLERR_BEGIN
		{
            KLPRCI_CreateComponentInstance(cidMaster, &m_PrivateInstance);
            m_bDeleteAgentConnection =true;
            m_PrivateInstance->SetState(KLPRCI::STATE_RUNNING);
		}
		KLERR_ENDT(2)
        KLSTD_ASSERT(m_bDeleteAgentConnection);
	}

	if (!m_bDontPublishEvents)
	{
		KLEV::SubscriptionOptions flags;
		flags.sendToAgent = true;
		m_arEventSourceSubscriptionSIDs.push_back(
			KLEV_GetEventSource()->Subscribe(m_cidMaster,
											 ComponentId(), 
											 c_EventSettingsChange, 
											 NULL,
											 SubscriptionsStorageEventCallback,
											 (void*)this,
											 flags)
		);
		m_arEventSourceSubscriptionSIDs.push_back(
			KLEV_GetEventSource()->Subscribe(m_cidMaster,
											 ComponentId(m_cidMaster.productName, m_cidMaster.version, L"", L""), 
											 KLPRTS_INTERNAL_EVT_TASK_STORAGE_CHANGED, 
											 NULL,
											 SubscriptionsStorageEventCallback,
											 (void*)this,
											 flags)
		);
	}

	if (wstrHostName!=NULL) m_wstrHostId = wstrHostName;

	m_bInitialized = true;
}

void CTaskStorageServer::SubscriptionsStorageEventCallback(const KLPRCI::ComponentId& subscriber,
														   const KLPRCI::ComponentId& publisher,
														   const KLEV::Event *event,
														   void *context)
{
	KLSTD_CHKINPTR(context);
	KLSTD_CHKINPTR(event);

	if (context && event)
	{
		CTaskStorageServer* pTSS = (CTaskStorageServer*)context;
		CAutoPtr<Params> parEventBody = event->GetParams();
		if (event->GetType() == c_EventSettingsChange)
		{
			KLSTD_TRACE2(3, L"CTaskStorageServer(product:'%ls')::SubscriptionsStorageEventCallback(event:'%ls')\n", pTSS->m_cidMaster.productName.c_str(), c_EventSettingsChange);
			if (parEventBody && pTSS)
			{
				std::wstring s;
				GET_PARAMS_VALUE(parEventBody, c_evpSsType, StringValue, STRING_T, s);
				
				CAutoPtr<Params> parExpandedType;
				KLPRSS_ExpandType(s, &parExpandedType);

				GET_PARAMS_VALUE(parExpandedType, c_szwSSP_Type, StringValue, STRING_T, s);
				if (s != c_szwSST_TasksSS) return;
				GET_PARAMS_VALUE(parExpandedType, c_szwSSP_Product, StringValue, STRING_T, s);
				if (s != pTSS->m_cidMaster.productName) return;
				GET_PARAMS_VALUE(parExpandedType, c_szwSSP_Version, StringValue, STRING_T, s);
				if (s != pTSS->m_cidMaster.version) return;
				GET_PARAMS_VALUE(parEventBody, c_evpSection, StringValue, STRING_T, s);
				if (s != SS_SECTION_NAME) return;
				
				KLSTD_TRACE2(3, L"CTaskStorageServer(product:'%ls')::SubscriptionsStorageEventCallback(event:'%ls'):this product version\n", pTSS->m_cidMaster.productName.c_str(), c_EventSettingsChange);

				int nType = KLPRSS::DCT_ADDED;
				GET_PARAMS_VALUE_NO_THROW(parEventBody, c_evpChangeType, IntValue, INT_T, nType);
				GET_PARAMS_VALUE(parExpandedType, c_szwSSP_RelPath, StringValue, STRING_T, s);
				
				std::wstring sDir, sName, sExt;
				KLSTD_SplitPath(s, sDir, sName, sExt);

				std::wstring sEventType;
				switch (nType) {
					case KLPRSS::DCT_REMOVED:
						KLSTD_TRACE3(3, L"CTaskStorageServer(product:'%ls')::SubscriptionsStorageEventCallback(task: '%ls', event:'%ls'):DELETED\n", pTSS->m_cidMaster.productName.c_str(), sName.c_str(), c_EventSettingsChange);
						sEventType = EVENT_TYPE_TASK_DELETED;
					break;
					case KLPRSS::DCT_CHANGED:
						KLSTD_TRACE3(3, L"CTaskStorageServer(product:'%ls')::SubscriptionsStorageEventCallback(task: '%ls', event:'%ls'):UPDATED\n", pTSS->m_cidMaster.productName.c_str(), sName.c_str(), c_EventSettingsChange);
						sEventType = EVENT_TYPE_TASK_UPDATED;
					break;
					case KLPRSS::DCT_ADDED:
						KLSTD_TRACE3(3, L"CTaskStorageServer(product:'%ls')::SubscriptionsStorageEventCallback(task: '%ls', event:'%ls'):ADDED\n", pTSS->m_cidMaster.productName.c_str(), sName.c_str(), c_EventSettingsChange);
						sEventType = EVENT_TYPE_NEW_TASK;
					break;
					default:
						KLSTD_TRACE3(3, L"CTaskStorageServer(product:'%ls')::SubscriptionsStorageEventCallback(task: '%ls', event:'%ls'):UNSUPPORTED\n", pTSS->m_cidMaster.productName.c_str(), sName.c_str(), c_EventSettingsChange);
				}

				if (!sEventType.empty()) pTSS->_PublishEvent(sEventType, sName, false);
			}
		}
		else
		if (event->GetType() == KLPRTS_INTERNAL_EVT_TASK_STORAGE_CHANGED)
		{
			if (parEventBody && pTSS)
			{
				std::wstring sTaskStorageUniqueID;
				GET_PARAMS_VALUE(parEventBody, EVTPAR_TSC_TASK_STORAGE_UID, StringValue, STRING_T, sTaskStorageUniqueID);
				if (sTaskStorageUniqueID != pTSS->m_sTaskStorageUniqueID)
				{
					long nSubType = 0;
					std::wstring sName;

					GET_PARAMS_VALUE(parEventBody, EVTPAR_TSC_SUBTYPE, IntValue, INT_T, nSubType);
					GET_PARAMS_VALUE(parEventBody, EVTPAR_TSC_TASK_NAME, StringValue, STRING_T, sName);
					
					switch(nSubType) {
						case TSC_TASK_ADDED:
							pTSS->_PublishEvent(EVENT_TYPE_NEW_TASK, sName, false);
						break;
						case TSC_TASK_UPDATED:
							pTSS->_PublishEvent(EVENT_TYPE_TASK_UPDATED, sName, false);
						break;
						case TSC_TASK_DELETED:
							pTSS->_PublishEvent(EVENT_TYPE_TASK_DELETED, sName, false);
						break;
					}
				}
			}
		}
	}
}

void CTaskStorageServer::_UnsubscribeAll()
{
	for(std::vector<KLEV::SubscriptionId>::iterator i = m_arEventSourceSubscriptionSIDs.begin();i != m_arEventSourceSubscriptionSIDs.end();i++)
		KLEV_GetEventSource()->Unsubscribe(*i);

	m_arEventSourceSubscriptionSIDs.clear();
}

void CTaskStorageServer::ResetTasksIterator(const KLPRCI::ComponentId &cidFilter, const std::wstring &sTaskNameFilter, KLPAR::Params** ppTaskStorageParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(ppTaskStorageParams);

	AUTOLOCK;

	TASK_PARAMS_NAMES arTaskParamsNames;
	_GetNames(arTaskParamsNames);

	CAutoPtr<Params> parResult;
	KLPAR_CreateParams(&parResult);

	TASK_PARAMS_NAMES::iterator itPos = arTaskParamsNames.begin();
	while(itPos != arTaskParamsNames.end())
	{
		KLERR_BEGIN
		{
			CAutoPtr<Params> parTaskParams;
			_ReadTask(*itPos, &parTaskParams);

			CTaskInfoInternal TaskInfo(parTaskParams);

			if (CompareComponentID(cidFilter, TaskInfo.GetTaskCID()) &&
				((sTaskNameFilter.size() != 0)?(TaskInfo.GetTaskName() == sTaskNameFilter):true))
			{
				// the task matches filter
				CAutoPtr<ParamsValue> valTaskParamsValue;
				CreateValue(parTaskParams, &valTaskParamsValue);
				parResult->AddValue(*itPos, valTaskParamsValue);
			}
		}
		KLERR_ENDT(1)

		itPos++;
	}

	*ppTaskStorageParams = parResult.Detach();
}

void CTaskStorageServer::GetTaskByID(const std::wstring TaskID, KLPAR::Params** ppTaskParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(ppTaskParams);

	AUTOLOCK;

	_GetTaskByID(TaskID, ppTaskParams);
}

void CTaskStorageServer::GetTaskByIDWithPolicyApplied(const std::wstring TaskID, KLPAR::Params** ppTaskParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(ppTaskParams);

	CAutoPtr<Params> parTask;
	GetTaskByID(TaskID, &parTask);
	
	CTaskInfoInternal TaskInfo(parTask);
	if (TaskInfo.GetTaskParams())
	{
		KLPRCI::ComponentId cid = TaskInfo.GetTaskCID();

		CAutoPtr<Params> parOriginalTaskParams;
		SmartReadTaskParams(TaskInfo.GetTaskName(), cid.productName, cid.version, TaskInfo.GetTaskParams(), &parOriginalTaskParams);
		TaskInfo.SetTaskParams(parOriginalTaskParams);

		CAutoPtr<Params> parSerializedTask;
		KLPAR_CreateParams(&parSerializedTask);
		TaskInfo.Serialize(parSerializedTask);
		parSerializedTask.CopyTo(ppTaskParams);
	}
	else
		parTask.CopyTo(ppTaskParams); 
}

std::wstring CTaskStorageServer::AddTask(const KLPAR::Params* pTaskToAddParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(pTaskToAddParams);

	CAutoPtr<Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);

	AUTOLOCK;

	std::wstring UniqueTaskID;
	GET_PARAMS_VALUE_NO_THROW(pTaskToAddParams, KLPRTS_ADD_TASK_TASK_PREDEFINED_ID, StringValue, STRING_T, UniqueTaskID);
	if (UniqueTaskID.empty())
		UniqueTaskID = (m_bLocalCopyOfServerStorage?KLPRTS_LOCAL_TASK_ATTRIBUTE:L"") + KLSTD_CreateGUIDString();

	CTaskInfoInternal TaskInfo(pTaskToAddParams);
	TaskInfo.SetTaskID(UniqueTaskID);
	TaskInfo.Serialize(pTaskParams);


	CAutoPtr<SettingsStorage> pStorage;
    //KLPRSS_CreateNewSettingsStorage(m_sTasksStorageFilePath + SLASH + UniqueTaskID + TASK_STORAGE_FILE_EXT, &pStorage);	
	KLPRSS_CreateSettingsStorage2(
                m_sTasksStorageFilePath + SLASH + UniqueTaskID + TASK_STORAGE_FILE_EXT, 
                KLSTD::CF_OPEN_ALWAYS,
                KLSTD::AF_READ|KLSTD::AF_WRITE,
                &pStorage,
                NULL,
                &g_oFormat);
	pStorage->CreateSection(SS_PRODUCT_NAME, L"", L"");
	pStorage->CreateSection(SS_PRODUCT_NAME, SS_VERSION, L"");
	pStorage->CreateSection(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME);
	pStorage->Add(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME, pTaskParams);
	
	AUDIT_NOTIFY_TASK_MODIFIED( TaskInfo, KLAUD::ActionAdded );

	_PublishEvent(EVENT_TYPE_NEW_TASK, UniqueTaskID);

	return UniqueTaskID;
}

void CTaskStorageServer::PutTask(const std::wstring TaskID, const KLPAR::Params* pDataToPut)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(pDataToPut);
	_CheckIsOperationPermitted(TaskID);

	AUTOLOCK;

	CTaskInfoInternal TaskInfo(pDataToPut);
	TaskInfo.SetTaskID(TaskID);

	CAutoPtr<Params> pTaskParams;
	KLPAR_CreateParams(&pTaskParams);
	TaskInfo.Serialize(pTaskParams);

	CAutoPtr<SettingsStorage> pStorage;
    //KLPRSS_OpenSettingsStorageRW(m_sTasksStorageFilePath + SLASH + TaskID + TASK_STORAGE_FILE_EXT, &pStorage);    
	KLPRSS_CreateSettingsStorage2(
                m_sTasksStorageFilePath + SLASH + TaskID + TASK_STORAGE_FILE_EXT,
                KLSTD::CF_OPEN_ALWAYS,
                KLSTD::AF_READ|KLSTD::AF_WRITE,
                &pStorage,
                NULL,
                &g_oFormat);
    
	bool bIsNew = false;
    KLERR_BEGIN
	{
	    pStorage->CreateSection(SS_PRODUCT_NAME, L"", L"");
        bIsNew = true;
	}
    KLERR_END

    KLPRSS_MAYEXIST(pStorage->CreateSection(SS_PRODUCT_NAME, L"", L""));
	KLPRSS_MAYEXIST(pStorage->CreateSection(SS_PRODUCT_NAME, SS_VERSION, L""));
	KLPRSS_MAYEXIST(pStorage->CreateSection(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME));
	pStorage->Clear(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME, pTaskParams);

    _PublishEvent(bIsNew?EVENT_TYPE_NEW_TASK:EVENT_TYPE_TASK_UPDATED, TaskID);
}

std::wstring CTaskStorageServer::GetHostId() const
{
	return m_wstrHostId;
}

void CTaskStorageServer::UpdateTask(const std::wstring TaskID, const Params* pTaskToUpdateParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(pTaskToUpdateParams);
	_CheckIsOperationPermitted(TaskID);

	AUTOLOCK;

	CAutoPtr<Params> parOldTaskParams, parTaskToUpdateParamsCopy;
	_GetTaskByID(TaskID, &parOldTaskParams);

	// ƒелаем 'легкую' копию, чтобы не мен€ть поведение метода, 
	// так как parTaskToUpdateParams объ€влен как const в интерфейсе
	((Params*)pTaskToUpdateParams)->Duplicate(&parTaskToUpdateParamsCopy);

	COPY_PARAMS_VALUE_NO_THROW(parTaskToUpdateParamsCopy, parOldTaskParams, SS_VALUE_NAME_FILTER_EVENTS_PRODUCT_NAME);
	COPY_PARAMS_VALUE_NO_THROW(parTaskToUpdateParamsCopy, parOldTaskParams, SS_VALUE_NAME_FILTER_EVENTS_VERSION);
	COPY_PARAMS_VALUE_NO_THROW(parTaskToUpdateParamsCopy, parOldTaskParams, SS_VALUE_NAME_FILTER_EVENTS_COMPONENT_NAME);
	COPY_PARAMS_VALUE_NO_THROW(parTaskToUpdateParamsCopy, parOldTaskParams, SS_VALUE_NAME_FILTER_EVENTS_INSTANCE_ID);
	COPY_PARAMS_VALUE_NO_THROW(parTaskToUpdateParamsCopy, parOldTaskParams, SS_VALUE_NAME_EVENT_TYPE);
	COPY_PARAMS_VALUE_NO_THROW(parTaskToUpdateParamsCopy, parOldTaskParams, SS_VALUE_NAME_EVENT_BODY_FILTER);

	_UpdateTaskInternal(TaskID, parTaskToUpdateParamsCopy, false);
}

void CTaskStorageServer::_UpdateTaskInternal(const std::wstring TaskID, const Params* pTaskToUpdateParams, bool bReplace,
		bool notifyTaskModify )
{
	CAutoPtr<SettingsStorage> pStorage;
    //KLPRSS_CreateSettingsStorage(m_sTasksStorageFilePath + SLASH + TaskID + TASK_STORAGE_FILE_EXT, CF_OPEN_EXISTING, AF_WRITE, &pStorage);
	KLPRSS_CreateSettingsStorage2(m_sTasksStorageFilePath + SLASH + TaskID + TASK_STORAGE_FILE_EXT, CF_OPEN_EXISTING, AF_WRITE, &pStorage, NULL, &g_oFormat);

	if (bReplace)
		pStorage->Clear(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME, const_cast<Params*>(pTaskToUpdateParams));
	else
		pStorage->Replace(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME, const_cast<Params*>(pTaskToUpdateParams));
	
	_PublishEvent(EVENT_TYPE_TASK_UPDATED, TaskID);


	CTaskInfoInternal TaskInfo(pTaskToUpdateParams);

	if ( notifyTaskModify ) AUDIT_NOTIFY_TASK_MODIFIED( TaskInfo, KLAUD::ActionModified );
}

void CTaskStorageServer::UpdateTask2(const std::wstring TaskID, const Params* pTaskToUpdateParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(pTaskToUpdateParams);
	_CheckIsOperationPermitted(TaskID);

	AUTOLOCK;
	_UpdateTaskInternal(TaskID, pTaskToUpdateParams, false);
}

void CTaskStorageServer::ReplaceTask(const std::wstring TaskID, const Params* pTaskToUpdateParams)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKINPTR(pTaskToUpdateParams);
	_CheckIsOperationPermitted(TaskID);

	AUTOLOCK;
	_UpdateTaskInternal(TaskID, pTaskToUpdateParams, true);
}

void CTaskStorageServer::DeleteTask(const std::wstring TaskId)
{
	MAKE_SURE_THAT_INITIALIZED;
	_CheckIsOperationPermitted(TaskId);

	AUTOLOCK;

	KLERR_BEGIN
	{
		CAutoPtr<KLPAR::Params> pTaskParams;
		_GetTaskByID(TaskId, &pTaskParams);		
		CTaskInfoInternal TaskInfo(pTaskParams);
		AUDIT_NOTIFY_TASK_MODIFIED( TaskInfo, KLAUD::ActionDeleted );
	}
	KLERR_ENDT(2)

	KLPRSS_RemoveSettingsStorage(std::wstring(m_sTasksStorageFilePath + SLASH + TaskId + TASK_STORAGE_FILE_EXT).c_str(), PRTS_DELETE_STORAGE_TIMEOUT);
	_PublishEvent(EVENT_TYPE_TASK_DELETED, TaskId);
}

void CTaskStorageServer::SetTaskStartEvent(const std::wstring TaskId, const KLPRCI::ComponentId& filter, const std::wstring& eventType, const KLPAR::Params* bodyFilter)
{
	MAKE_SURE_THAT_INITIALIZED;
	_CheckIsOperationPermitted(TaskId);

	AUTOLOCK;

	CAutoPtr<KLPAR::Params> pTaskParams;
	_GetTaskByID(TaskId, &pTaskParams);

	CTaskInfoInternal TaskInfo(pTaskParams);
	TaskInfo.SetTaskStartEvent(filter, eventType, bodyFilter);
	
	pTaskParams->Clear();
	TaskInfo.Serialize(pTaskParams);

	_UpdateTaskInternal(TaskId, pTaskParams, false, false); // т.к. изменени€ задачи осуществл€етс€ всегда двум€ вызовами ( второй SetEvent ) то на втором вызове не надо публикавать событие аудита
}

std::wstring& CTaskStorageServer::getID()
{
	return m_sServerObjectID;
};

void CTaskStorageServer::GetTaskStartEvent(const std::wstring TaskId, KLPRCI::ComponentId& filter, std::wstring& eventType, KLPAR::Params** bodyFilter)
{
	MAKE_SURE_THAT_INITIALIZED;
	KLSTD_CHKOUTPTR(bodyFilter);

	AUTOLOCK;

	CAutoPtr<KLPAR::Params> pTaskParams;
	_GetTaskByID(TaskId, &pTaskParams);

	CTaskInfoInternal TaskInfo(pTaskParams);
	TaskInfo.GetTaskStartEvent(filter, eventType, bodyFilter);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void CTaskStorageServer::_PublishEvent(const std::wstring sEventType, const std::wstring TaskID, bool bPublishInternalEvent)
{
	if (m_bDontPublishEvents) 
	{
		KLSTD_TRACE2(3, L"CTaskStorageServer(product:'%ls')::_PublishEvent(event:'%ls'):m_bDontPublishEvents==TRUE\n", m_cidMaster.productName.c_str(), sEventType.c_str());
		return;
	}
	CAutoPtr<Params> parEventBody;
	KLPAR_CreateParams(&parEventBody);
	ADD_PARAMS_VALUE(parEventBody, EVENT_BODY_PARAM_NAME_TASKID, StringValue, (wchar_t*)TaskID.c_str());

	time_t t;
	CAutoPtr<KLEV::Event> pEvent;
	KLEV_CreateEvent(&pEvent, m_cidMaster, sEventType, parEventBody, time(&t));
	KLSTD_CHKMEM(pEvent);

	KLSTD_CHKMEM(KLEV_GetEventSource());
	KLEV_GetEventSource()->PublishEvent(pEvent);

	KLSTD_TRACE2(3, L"CTaskStorageServer(product:'%ls')::_PublishEvent(event:'%ls'):Published\n", m_cidMaster.productName.c_str(), sEventType.c_str());

	///////////////////////////////////////////////////////////////////
	// Internal event

	if (!bPublishInternalEvent) 
	{
		KLSTD_TRACE2(3, L"CTaskStorageServer(product:'%ls')::_PublishEvent(event:'%ls'):bPublishInternalEvent== FALSE\n", m_cidMaster.productName.c_str(), sEventType.c_str());
		return;
	}

	long nSubtype = 0;
	if (sEventType == EVENT_TYPE_NEW_TASK)
		nSubtype = TSC_TASK_ADDED;
	else
	if (sEventType == EVENT_TYPE_TASK_UPDATED)
		nSubtype = TSC_TASK_UPDATED;
	else
	if (sEventType == EVENT_TYPE_TASK_DELETED)
		nSubtype = TSC_TASK_DELETED;
	else
		return;

	parEventBody = NULL;
	KLPAR_CreateParams(&parEventBody);
	ADD_PARAMS_VALUE(parEventBody, EVTPAR_TSC_TASK_STORAGE_UID, StringValue, m_sTaskStorageUniqueID.c_str());
	ADD_PARAMS_VALUE(parEventBody, EVTPAR_TSC_SUBTYPE, IntValue, nSubtype);
	ADD_PARAMS_VALUE(parEventBody, EVTPAR_TSC_TASK_NAME, StringValue, TaskID.c_str());

	pEvent = NULL;
	KLEV_CreateEvent(&pEvent, m_cidMaster, KLPRTS_INTERNAL_EVT_TASK_STORAGE_CHANGED, parEventBody, time(&t));
	KLSTD_CHKMEM(pEvent);
	KLEV_GetEventSource()->PublishEvent(pEvent);

	KLSTD_TRACE2(3, L"CTaskStorageServer(product:'%ls')::_PublishEvent(event:'%ls'):Published internal event\n", m_cidMaster.productName.c_str(), sEventType.c_str());
}

bool CTaskStorageServer::CompareComponentID(const KLPRCI::ComponentId& CIDFilter, const KLPRCI::ComponentId& CID2)
{
#define CHECK_FIELD(field) (CIDFilter.field.size() == 0?true:(CIDFilter.field == CID2.field))

	return (CHECK_FIELD(productName) &&
			CHECK_FIELD(version) &&
			CHECK_FIELD(componentName) &&
			CHECK_FIELD(instanceId));
}

void CTaskStorageServer::_GetTaskByID(std::wstring TaskID, KLPAR::Params** ppTaskParams)
{
	_ReadTask(TaskID + TASK_STORAGE_FILE_EXT, ppTaskParams);
}

void CTaskStorageServer::_GetNames(std::vector<std::wstring>& arNames)
{
	std::set<wstring> setNames;
	TSKLIST::GetTasksFilesList(m_sTasksStorageFilePath, setNames);

	arNames.clear();
	arNames.reserve(setNames.size());
	for(std::set<std::wstring>::iterator it=setNames.begin();it != setNames.end();++it)
	{
		arNames.push_back(*it);
	}
}

void CTaskStorageServer::_ReadTask(std::wstring sTaskStorageFileName, KLPAR::Params** ppTaskParams)
{
	KLERR_TRY
	{
		CAutoPtr<SettingsStorage> pStorage;
		KLPRSS_CreateSettingsStorage(m_sTasksStorageFilePath + SLASH + sTaskStorageFileName, CF_OPEN_EXISTING, AF_READ, &pStorage);
		pStorage->Read(SS_PRODUCT_NAME, SS_VERSION, SS_SECTION_NAME, ppTaskParams);
	}
	KLERR_CATCH(p)
	{
		switch(p->GetId()) {
			case STDE_NOTFOUND:
			case STDE_NOENT:
			case KLPRSS::NOT_EXIST:
			case KLPAR::NOT_EXIST:
				KLSTD_THROW(STDE_NOTFOUND);
			break;
		}
		KLERR_RETHROW();
	}
	KLERR_ENDTRY

	KLSTD_ASSERT(ppTaskParams);
	KLSTD_CHKMEM(ppTaskParams);
	if ((*ppTaskParams)->GetSize() == 0) KLSTD_THROW(STDE_NOTFOUND);
}

//////////////////////////////////////////////////////////////////////////

void CTaskStorageServer::Deinitialize()
{
}

//////////////////////////////////////////////////////////////////////////
// CTasksStorageServerList

typedef KLSTD::KLAdapt<KLSTD::CAutoPtr<KLPRTS::TaskStorageServer> > TASKS_STORAGE_SERVER;
typedef std::map<std::wstring, TASKS_STORAGE_SERVER> TASKS_STORAGE_SERVER_LIST;

class CTasksStorageServerList : public TASKS_STORAGE_SERVER_LIST
{
public:
	CTasksStorageServerList() {
		IMPLEMENT_LOCK;
	}

	void Add(KLPRTS::TaskStorageServer* pTaskStorageServer)
	{
		KLSTD_CHKINPTR(pTaskStorageServer);

		AUTOLOCK;
		insert(value_type(pTaskStorageServer->getID(), TASKS_STORAGE_SERVER(pTaskStorageServer)));
	}

	void GetServerObjectByID(const std::wstring& sServerObjectID, KLPRTS::TaskStorageServer** ppTaskStorageServer, bool bThrowNotExistException = true)
	{
		KLSTD_CHKOUTPTR(ppTaskStorageServer);

		AUTOLOCK;

		iterator it = find(sServerObjectID);
		if (it != end())
		{
			KLSTD::CAutoPtr<KLPRTS::TaskStorageServer> pTaskStorageServerAutoPtr = (*it).second.m_T;
			pTaskStorageServerAutoPtr.CopyTo(ppTaskStorageServer);
			return;
		}

		*ppTaskStorageServer = NULL;
		if (bThrowNotExistException) KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	}

	void DeleteServerObjectByID(const std::wstring& sServerObjectID, bool bThrowNotExistException = false)
	{
		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer> pServerTDelete;

		AUTOLOCK;

		iterator it = find(sServerObjectID);
		if (it != end()) 
		{
			erase(it);
			return;
		}

		if (bThrowNotExistException) KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	}

protected:
	DECLARE_LOCK;
};

CTasksStorageServerList* m_pTasksStorageServerList = NULL;

} // end namespace KLPRTS

//////////////////////////////////////////////////////////////////////////

using namespace KLPRTS;

KLCSKCA_DECL void KLPRTS_CreateTasksStorageServer(const std::wstring& sServerObjectID, const KLPRCI::ComponentId& cidMaster, const std::wstring& sTasksStorageFilePath, 
												  const bool bLocalCopyOfServerStorage, const bool bAddAgentConnection, 
												  const bool bDontPublishEvents,
												  const wchar_t *wstrHostName )
{
	KLSTD::CAutoPtr<CTaskStorageServer> pTaskStorageServer;
	pTaskStorageServer.Attach(new KLPRTS::CTaskStorageServer(bLocalCopyOfServerStorage, sServerObjectID, bDontPublishEvents ));
	if (!pTaskStorageServer) KLSTD_THROW(KLSTD::STDE_NOMEMORY);
	pTaskStorageServer->Initialize(cidMaster, sTasksStorageFilePath, bAddAgentConnection, wstrHostName);
	m_pTasksStorageServerList->Add(pTaskStorageServer);
}

KLCSKCA_DECL void KLPRTS_DeleteTasksStorageServer(const std::wstring& sServerObjectID)
{
	m_pTasksStorageServerList->DeleteServerObjectByID(sServerObjectID);
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MODULE_INIT_DEINIT(KLPRTS)

IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL, KLPRTS)
{
	if (m_pTasksStorageServerList) KLSTD_THROW(KLSTD::STDE_NOTPERM);
	m_pTasksStorageServerList = new CTasksStorageServerList;

	KLPRTS::RegisterGSOAPStubFunctions();
}
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL, KLPRTS)
{
	KLPRTS::UnregisterGSOAPStubFunctions();
	if (m_pTasksStorageServerList) delete m_pTasksStorageServerList;
	m_pTasksStorageServerList = NULL;
	KLPRTS::CTaskStorageServer::Deinitialize();
}
IMPLEMENT_MODULE_DEINIT_END()

//////////////////////////////////////////////////////////////////////////
//
// SOAP interface implementation
//
//////////////////////////////////////////////////////////////////////////

#define DO_COMMAND	 try {

#define HANDLE_ERRORS(rc) \
rc.code = KLSTD::STDE_NOERROR; \
} catch(KLERR::Error* err) { \
	if (err) { \
		KLPAR::ExceptionForSoap(pSOAP, err, rc); \
		err->Release(); \
	} \
} \
catch(std::exception&) { \
	KLSTD::CAutoPtr<KLERR::Error> err; \
	KLERR_CreateError(&err, KLPRTS::KLCS_MODULENAME, KLSTD::STDE_GENERAL, __FILE__, __LINE__); \
	KLPAR::ExceptionForSoap(pSOAP, err, rc); \
}

//////////////////////////////////////////////////////////////////////////

#define ACCESS_CHECK(action) \
	KLSTD::CAutoPtr<KLPRTS::TaskStorageServer> pTaskStorageServer; \
	m_pTasksStorageServerList->GetServerObjectByID(std::wstring(pszServerObjectID), &pTaskStorageServer); \
	KLSTD_CHKMEM(pTaskStorageServer); \
	if ( pTaskStorageServer->GetHostId().empty() )\
		KLAVT_AccessCheckForAction_InCall( KLAVT_ACT_PRTS, action);\
	else\
		KLAVT_AccessCheckForActionForHost_InCall( KLAVT_ACT_PRTS, action, pTaskStorageServer->GetHostId() );

#define CALL_METHOD(method) \
	pTaskStorageServer->method

#define CALL_METHOD_WITH_RETVAL(method, rc) \
	rc = pTaskStorageServer->method


SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_ResetTasksIteratorStub(struct soap* pSOAP, 
													wchar_t *pszServerObjectID,
													SOAPComponentId cidFilter, 
													wchar_t *pszTaskNameFilter, 
													struct KLPRTS_RESET_TASKS_ITERATOR_RESPONSE &rc)
{	
	DO_COMMAND
	{

		KLSTD_CHKINPTR(pszServerObjectID);
		KLSTD_CHKINPTR(pszTaskNameFilter);

		ACCESS_CHECK(KLAVT_READ);

		std::wstring sTaskNameFilter = pszTaskNameFilter;
		KLPRCI::ComponentId cid(cidFilter.productName, 
								cidFilter.version, 
								cidFilter.componentName, 
								cidFilter.instanceId);
		KLSTD::CAutoPtr<KLPAR::Params> parParams;

		
		CALL_METHOD(ResetTasksIterator(cid, sTaskNameFilter, &parParams));		
		KLPAR::ParamsForSoap(pSOAP, parParams, rc.parTaskListParamsOut);		
	}
	HANDLE_ERRORS(rc.rcError)	

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_GetTaskByIDStub(struct soap* pSOAP, 
											 wchar_t *pszServerObjectID,
											 wchar_t *TaskID, 
											 struct KLPRTS_GET_TASK_BY_ID_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_READ);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		CALL_METHOD(GetTaskByID(TaskID, &parParams));
		KLPAR::ParamsForSoap(pSOAP, parParams, rc.parTaskParamsOut);
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_GetTaskByIDWithPolicyAppliedStub(struct soap* pSOAP, 
											 wchar_t *pszServerObjectID,
											 wchar_t *TaskID, 
											 struct KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_READ);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		CALL_METHOD(GetTaskByIDWithPolicyApplied(TaskID, &parParams));
		KLPAR::ParamsForSoap(pSOAP, parParams, rc.parTaskParamsOut);
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_AddTaskStub(struct soap* pSOAP, 
										 wchar_t *pszServerObjectID,
										 param__params parTaskParams, 
										 struct KLPRTS_ADD_TASK_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_WRITE);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;
		KLPAR::ParamsFromSoap(parTaskParams, &parParams);
		
		std::wstring sTaskID;
		CALL_METHOD_WITH_RETVAL(AddTask(parParams), sTaskID);

		rc.TaskID = KLPAR::soap_strdup(pSOAP, sTaskID.c_str());
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_UpdateTaskStub(struct soap* pSOAP, 
											wchar_t *pszServerObjectID,
											wchar_t *TaskID, 
											param__params parTaskParams, 
											param_error &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_WRITE);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;

		KLPAR::ParamsFromSoap(parTaskParams, &parParams);
		CALL_METHOD(UpdateTask(TaskID, parParams));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_UpdateTask2Stub(struct soap* pSOAP, 
												 wchar_t *pszServerObjectID,
												 wchar_t *TaskID, 
												 param__params parTaskParams, 
												 param_error &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_WRITE);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;

		KLPAR::ParamsFromSoap(parTaskParams, &parParams);
		CALL_METHOD(UpdateTask2(TaskID, parParams));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_ReplaceTaskStub(struct soap* pSOAP, 
												 wchar_t *pszServerObjectID,
												 wchar_t *TaskID, 
												 param__params parTaskParams, 
												 param_error &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_WRITE);

		KLSTD::CAutoPtr<KLPAR::Params> parParams;

		KLPAR::ParamsFromSoap(parTaskParams, &parParams);
		CALL_METHOD(ReplaceTask(TaskID, parParams));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_DeleteTaskStub(struct soap* pSOAP, 
											wchar_t *pszServerObjectID,
											wchar_t *TaskID, 
											param_error &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_DELETE);

		CALL_METHOD(DeleteTask(TaskID));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_SetTaskStartEventStub(struct soap* pSOAP, 
												   wchar_t *pszServerObjectID,
												   wchar_t *TaskID, 
												   SOAPComponentId cidEventFilter, 
												   wchar_t* pszEventType, 
												   param__params parBodyFilter, 
												   param_error &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);
		KLSTD_CHKINPTR(pszEventType);

		ACCESS_CHECK(KLAVT_WRITE);

		std::wstring sEventType = pszEventType;
		KLPRCI::ComponentId cid(cidEventFilter.productName, 
								cidEventFilter.version, 
								cidEventFilter.componentName, 
								cidEventFilter.instanceId);
		KLSTD::CAutoPtr<KLPAR::Params> parParams;

		KLPAR::ParamsFromSoap(parBodyFilter, &parParams);
		CALL_METHOD(SetTaskStartEvent(TaskID, cid, sEventType, parParams));
	}
	HANDLE_ERRORS(rc)

	return SOAP_OK;
}

SOAP_FMAC1 int SOAP_FMAC2 KLPRTS_GetTaskStartEventStub(struct soap* pSOAP, 
												   wchar_t *pszServerObjectID,
												   wchar_t *TaskID, 
												   struct KLPRTS_GET_TASK_START_EVENT_RESPONSE &rc)
{
	DO_COMMAND
	{
		KLSTD_CHKINPTR(pszServerObjectID);

		ACCESS_CHECK(KLAVT_READ);

		std::wstring sEventType;
		KLPRCI::ComponentId cid;

		KLSTD::CAutoPtr<KLPAR::Params> parParams;

		CALL_METHOD(GetTaskStartEvent(TaskID, cid, sEventType, &parParams));
		KLPAR::ParamsForSoap(pSOAP, parParams, rc.parBodyFilterOut);

		rc.cidEventFilter.componentName = KLPAR::soap_strdup(pSOAP, cid.componentName.c_str());
		rc.cidEventFilter.productName = KLPAR::soap_strdup(pSOAP, cid.productName.c_str());
		rc.cidEventFilter.version = KLPAR::soap_strdup(pSOAP, cid.version.c_str());
		rc.cidEventFilter.instanceId = KLPAR::soap_strdup(pSOAP, cid.instanceId.c_str());
		rc.sEventType = KLPAR::soap_strdup(pSOAP, sEventType.c_str());
	}
	HANDLE_ERRORS(rc.rcError)

	return SOAP_OK;
}

void KLPRTS_GetServerObjectByID(const std::wstring&	sServerObjectID,
								KLPRTS::TaskStorageServer** ppTasksStorageServer,
								bool bThrowNotExistException)
{
	KLSTD_CHKOUTPTR(ppTasksStorageServer);
	KLSTD_ASSERT_THROW(m_pTasksStorageServerList != NULL);
	m_pTasksStorageServerList->GetServerObjectByID(sServerObjectID,
												   ppTasksStorageServer,
												   bThrowNotExistException);
};

void KLPRTS_AddServerObject(KLPRTS::TaskStorageServer*	pServer)
{
	KLSTD_ASSERT_THROW(m_pTasksStorageServerList != NULL);
	m_pTasksStorageServerList->Add(pServer);
};

KLCSKCA_DECL bool KLPRTS_IsLocalTask(const std::wstring &taskId)
{
	return KLPRTS::CTaskStorageServer::IsLocalTask(taskId);
}

