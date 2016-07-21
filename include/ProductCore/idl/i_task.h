#ifndef idl_i_task_h
#define idl_i_task_h

#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_idl.h>
#include "Prague/idl/std_idl.h"
#include "i_tm.h"



#if !defined(__cplusplus) || defined(_USE_VTBL)


typedef struct tag_hTask tag_hTask;
typedef tag_hTask* hTASK;
typedef struct tag_hTaskEx tag_hTaskEx;
typedef tag_hTaskEx* hTASKEX;
	//	interface tag_hTask definition
	#define MACRO_TASK_ID 0x17
	static const InterfaceId Task_IID = MACRO_TASK_ID;
	typedef tDWORD Task_tActionId;
	typedef tDWORD Task_tSerializableId;
	#define MACRO_TASK_PGTASK_TM mPROPERTY_MAKE_GLOBAL(pTYPE_PTR, 0x1002)
	static const PropertyId Task_pgTASK_TM = MACRO_TASK_PGTASK_TM;
	#define MACRO_TASK_PGTASK_STATE mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1003)
	static const PropertyId Task_pgTASK_STATE = MACRO_TASK_PGTASK_STATE;
	#define MACRO_TASK_PGTASK_SESSION_ID mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1004)
	static const PropertyId Task_pgTASK_SESSION_ID = MACRO_TASK_PGTASK_SESSION_ID;
	#define MACRO_TASK_PGTASK_ID mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1005)
	static const PropertyId Task_pgTASK_ID = MACRO_TASK_PGTASK_ID;
	#define MACRO_TASK_PGTASK_PERSISTENT_STORAGE mPROPERTY_MAKE_GLOBAL(pTYPE_PTR, 0x1006)
	static const PropertyId Task_pgTASK_PERSISTENT_STORAGE = MACRO_TASK_PGTASK_PERSISTENT_STORAGE;
	#define MACRO_TASK_PGTASK_PARENT_ID mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1007)
	static const PropertyId Task_pgTASK_PARENT_ID = MACRO_TASK_PGTASK_PARENT_ID;
	static const MessageClassId Task_pmc_TASK_STATE_CHANGED = 0x1000;
	static const MessageClassId Task_pmc_TASK_REPORT = 0x1001;
	static const MessageClassId Task_pmc_TASK_SETTINGS_CHANGED = 0x1002;
	static const MessageClassId Task_pmc_TASK_OS_REBOOT_REQUEST = 0x1003;
	struct iTaskVtbl;
	typedef struct iTaskVtbl iTaskVtbl;
	
	typedef struct tag_hTask {
		const iTaskVtbl* vtbl;
		const iSYSTEMVtbl* sys;
	} *hTask;
	typedef tERROR (pr_call *fnpTask_SetSettings) (hTask _this, const cSerializable* settings);
	typedef tERROR (pr_call *fnpTask_GetSettings) (hTask _this, cSerializable* settings);
	typedef tERROR (pr_call *fnpTask_AskAction) (hTask _this, Task_tActionId actionId, cSerializable* actionInfo);
	typedef tERROR (pr_call *fnpTask_SetState) (hTask _this, TaskManager_tTaskRequestState state);
	typedef tERROR (pr_call *fnpTask_GetStatistics) (hTask _this, cSerializable* statistics);
	struct iTaskVtbl {
		fnpTask_SetSettings	SetSettings;
		fnpTask_GetSettings	GetSettings;
		fnpTask_AskAction	AskAction;
		fnpTask_SetState	SetState;
		fnpTask_GetStatistics	GetStatistics;
	};
	#define CALL_Task_SetSettings(_this, settings)	((_this)->vtbl->SetSettings((_this), settings))
	#define CALL_Task_GetSettings(_this, settings)	((_this)->vtbl->GetSettings((_this), settings))
	#define CALL_Task_AskAction(_this, actionId, actionInfo)	((_this)->vtbl->AskAction((_this), actionId, actionInfo))
	#define CALL_Task_SetState(_this, state)	((_this)->vtbl->SetState((_this), state))
	#define CALL_Task_GetStatistics(_this, statistics)	((_this)->vtbl->GetStatistics((_this), statistics))
	static const tERROR Task_errTASK_CANNOT_UPDATE_SETTINGS_ON_RUNTIME = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 0 /*incremental code*/);
	static const tERROR Task_errTASK_STATE_UNKNOWN = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 1 /*incremental code*/);
	static const tERROR Task_errTASK_CANNOT_UPDATE_SETTINGS_ON_STOPPED = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 2 /*incremental code*/);
	static const tERROR Task_errTASK_DISABLED = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 3 /*incremental code*/);
	//	end of interface tag_hTask definition
	
	//	interface tag_hTaskEx definition
	#define MACRO_TASKEX_ID 0x2a
	static const InterfaceId TaskEx_IID = MACRO_TASKEX_ID;
	struct iTaskExVtbl;
	typedef struct iTaskExVtbl iTaskExVtbl;
	
	typedef struct tag_hTaskEx {
		const iTaskExVtbl* vtbl;
		const iSYSTEMVtbl* sys;
	} *hTaskEx;
	typedef tERROR (pr_call *fnpTaskEx_SetSettings) (hTaskEx _this, const cSerializable* settings);
	typedef tERROR (pr_call *fnpTaskEx_GetSettings) (hTaskEx _this, cSerializable* settings);
	typedef tERROR (pr_call *fnpTaskEx_AskAction) (hTaskEx _this, Task_tActionId actionId, cSerializable* actionInfo);
	typedef tERROR (pr_call *fnpTaskEx_SetState) (hTaskEx _this, TaskManager_tTaskRequestState state);
	typedef tERROR (pr_call *fnpTaskEx_GetStatistics) (hTaskEx _this, cSerializable* statistics);
	typedef tERROR (pr_call *fnpTaskEx_GetService) (hTaskEx _this, hOBJECT* service, TaskManager_tServiceId serviceId, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskEx_ReleaseService) (hTaskEx _this, hOBJECT service, hOBJECT client);
	struct iTaskExVtbl {
		fnpTaskEx_SetSettings	SetSettings;
		fnpTaskEx_GetSettings	GetSettings;
		fnpTaskEx_AskAction	AskAction;
		fnpTaskEx_SetState	SetState;
		fnpTaskEx_GetStatistics	GetStatistics;
		fnpTaskEx_GetService	GetService;
		fnpTaskEx_ReleaseService	ReleaseService;
	};
	#define CALL_TaskEx_SetSettings(_this, settings)	((_this)->vtbl->SetSettings((_this), settings))
	#define CALL_TaskEx_GetSettings(_this, settings)	((_this)->vtbl->GetSettings((_this), settings))
	#define CALL_TaskEx_AskAction(_this, actionId, actionInfo)	((_this)->vtbl->AskAction((_this), actionId, actionInfo))
	#define CALL_TaskEx_SetState(_this, state)	((_this)->vtbl->SetState((_this), state))
	#define CALL_TaskEx_GetStatistics(_this, statistics)	((_this)->vtbl->GetStatistics((_this), statistics))
	#define CALL_TaskEx_GetService(_this, service, serviceId, client)	((_this)->vtbl->GetService((_this), service, serviceId, client))
	#define CALL_TaskEx_ReleaseService(_this, service, client)	((_this)->vtbl->ReleaseService((_this), service, client))
	//	end of interface tag_hTaskEx definition
	



#else  // !defined(__cplusplus) || defined(_USE_VTBL)


struct IDL_NAME(cTask);
typedef IDL_NAME(cTask)* IDL_NAME(hTASK);
struct IDL_NAME(cTaskEx);
typedef IDL_NAME(cTaskEx)* IDL_NAME(hTASKEX);


//	interface IDL_NAME(cTask) definition
struct pr_abstract Task : public StdBase {
	static const InterfaceId IID = 0x17;
	typedef tDWORD tActionId;
	typedef tDWORD tSerializableId;
	static const PropertyId IDL_NAME(pgTASK_TM) = mPROPERTY_MAKE_GLOBAL(pTYPE_PTR, 0x1002);
	static const PropertyId IDL_NAME(pgTASK_STATE) = mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1003);
	static const PropertyId IDL_NAME(pgTASK_SESSION_ID) = mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1004);
	static const PropertyId IDL_NAME(pgTASK_ID) = mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1005);
	static const PropertyId IDL_NAME(pgTASK_PERSISTENT_STORAGE) = mPROPERTY_MAKE_GLOBAL(pTYPE_PTR, 0x1006);
	static const PropertyId IDL_NAME(pgTASK_PARENT_ID) = mPROPERTY_MAKE_GLOBAL(pTYPE_DWORD, 0x1007);
	static const MessageClassId IDL_NAME(pmc_TASK_STATE_CHANGED) = 0x1000;
	static const MessageClassId IDL_NAME(pmc_TASK_REPORT) = 0x1001;
	static const MessageClassId IDL_NAME(pmc_TASK_SETTINGS_CHANGED) = 0x1002;
	static const MessageClassId IDL_NAME(pmc_TASK_OS_REBOOT_REQUEST) = 0x1003;
	static const tERROR IDL_NAME(errTASK_CANNOT_UPDATE_SETTINGS_ON_RUNTIME) = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 0 /*incremental code*/);
	static const tERROR IDL_NAME(errTASK_STATE_UNKNOWN) = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 1 /*incremental code*/);
	static const tERROR IDL_NAME(errTASK_CANNOT_UPDATE_SETTINGS_ON_STOPPED) = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 2 /*incremental code*/);
	static const tERROR IDL_NAME(errTASK_DISABLED) = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x17 /*implementation id*/, 3 /*incremental code*/);
	virtual tERROR pr_call SetSettings(const cSerializable* settings) = 0;
	virtual tERROR pr_call GetSettings(cSerializable* settings) = 0;
	virtual tERROR pr_call AskAction(Task::tActionId actionId, cSerializable* actionInfo) = 0;
	virtual tERROR pr_call SetState(TaskManager::tTaskRequestState state) = 0;
	virtual tERROR pr_call GetStatistics(cSerializable* statistics) = 0;
};


#if !defined(PR_IDL_LEGACY)

	#define CALL_Task_SetSettings(_this, settings)	((_this)->SetSettings(settings))
	#define CALL_Task_GetSettings(_this, settings)	((_this)->GetSettings(settings))
	#define CALL_Task_AskAction(_this, actionId, actionInfo)	((_this)->AskAction(actionId, actionInfo))
	#define CALL_Task_SetState(_this, state)	((_this)->SetState(state))
	#define CALL_Task_GetStatistics(_this, statistics)	((_this)->GetStatistics(statistics))


#endif // !defined(PR_IDL_LEGACY)


struct pr_abstract IDL_NAME(cTask) : public Task, public iObj {
	OBJ_IMPL( IDL_NAME(cTask) );
};
//	end of interface IDL_NAME(cTask) definition

//	interface IDL_NAME(cTaskEx) definition
struct pr_abstract TaskEx : public Task {
	static const InterfaceId IID = 0x2a;
	virtual tERROR pr_call SetSettings(const cSerializable* settings) = 0;
	virtual tERROR pr_call GetSettings(cSerializable* settings) = 0;
	virtual tERROR pr_call AskAction(Task::tActionId actionId, cSerializable* actionInfo) = 0;
	virtual tERROR pr_call SetState(TaskManager::tTaskRequestState state) = 0;
	virtual tERROR pr_call GetStatistics(cSerializable* statistics) = 0;
	virtual tERROR pr_call GetService(hOBJECT* service, TaskManager::tServiceId serviceId, hOBJECT client) = 0;
	virtual tERROR pr_call ReleaseService(hOBJECT service, hOBJECT client) = 0;
};


#if !defined(PR_IDL_LEGACY)

	#define CALL_TaskEx_SetSettings(_this, settings)	((_this)->SetSettings(settings))
	#define CALL_TaskEx_GetSettings(_this, settings)	((_this)->GetSettings(settings))
	#define CALL_TaskEx_AskAction(_this, actionId, actionInfo)	((_this)->AskAction(actionId, actionInfo))
	#define CALL_TaskEx_SetState(_this, state)	((_this)->SetState(state))
	#define CALL_TaskEx_GetStatistics(_this, statistics)	((_this)->GetStatistics(statistics))
	#define CALL_TaskEx_GetService(_this, service, serviceId, client)	((_this)->GetService(service, serviceId, client))
	#define CALL_TaskEx_ReleaseService(_this, service, client)	((_this)->ReleaseService(service, client))


#endif // !defined(PR_IDL_LEGACY)


struct pr_abstract IDL_NAME(cTaskEx) : public TaskEx, public iObj {
	OBJ_IMPL( IDL_NAME(cTaskEx) );
};
//	end of interface IDL_NAME(cTaskEx) definition




#endif  // !defined(__cplusplus) || defined(_USE_VTBL)



#endif //idl_i_task_h




