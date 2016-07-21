#ifndef idl_i_tm_h
#define idl_i_tm_h

#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_idl.h>
#include "Prague/idl/std_idl.h"



#if !defined(__cplusplus) || defined(_USE_VTBL)


typedef struct tag_hTaskManager tag_hTaskManager;
typedef tag_hTaskManager* hTASKMANAGER;
typedef struct tag_hTmHost tag_hTmHost;
typedef tag_hTmHost* hTMHOST;
	//	interface tag_hTaskManager definition
	#define MACRO_TASKMANAGER_ID 0x27
	static const InterfaceId TaskManager_IID = MACRO_TASKMANAGER_ID;
	typedef tDWORD TaskManager_tTaskId;
	typedef tDWORD TaskManager_tServiceId;
	typedef tDWORD TaskManager_tTaskRequestState;
	typedef tDWORD TaskManager_tTaskState;
	typedef tDWORD TaskManager_cProfile;
	typedef tDWORD TaskManager_cProfileEx;
	typedef tDWORD TaskManager_cProfileList;
	typedef tDWORD TaskManager_tGetProfileInfoSource;
	static const tDWORD TaskManager_OPEN_TASK_MODE_CREATE = 2147483648;
	static const tDWORD TaskManager_OPEN_TASK_MODE_OPEN = 1073741824;
	static const tDWORD TaskManager_OPEN_TASK_MODE_OPEN_ALWAYS = 3221225472;
	static const tDWORD TaskManager_OPEN_TASK_MODE_MASK = 3221225472;
	static const tDWORD TaskManager_OPEN_TASK_MODE_CLOSE_TASK_IF_OWNER_DIED = 536870912;
	static const tDWORD TaskManager_fPROFILE_TEMPORARY = 1;
	static const tDWORD TaskManager_cCLEAN_REPORTS = 1;
	static const tDWORD TaskManager_cCLEAN_BACKUP_ITEMS = 2;
	static const tDWORD TaskManager_cCLEAN_QUARANTINE_ITEMS = 4;
	static const tDWORD TaskManager_cCLEAN_QB_ITEMS = 6;
	static const tDWORD TaskManager_cREQUEST_SYNCHRONOUS = 4294967295;
	static const tDWORD TaskManager_cREQUEST_DELAY_UP_TO_INTERNET_CONNECTION = 2147483648;
	static const tDWORD TaskManager_cREQUEST_DELAY_MAKE_PERSISTENT = 1073741824;
	static const tDWORD TaskManager_cREQUEST_RESTART_PREVIOUS_TASK = 536870912;
	static const tDWORD TaskManager_cREQUEST_DONOT_FORCE_RUN = 268435456;
	static const tDWORD TaskManager_cREQUEST_SYNCSTORE_DATA = 16777216;
	static const tDWORD TaskManager_cTM_INIT_FLAG_DEFAULT = 1;
	static const tDWORD TaskManager_cTM_INIT_FLAG_PROFILES = 4;
	static const tDWORD TaskManager_cTM_INIT_FLAG_STARTGUI = 8;
	static const tDWORD TaskManager_cREPORT_FILTER_NONE = 1;
	static const tDWORD TaskManager_cREPORT_FILTER_RECENT = 2;
	static const tDWORD TaskManager_cREPORT_RECORD_NEXT = 4;
	static const tDWORD TaskManager_cREPORT_RECORD_PREV = 8;
	static const tDWORD TaskManager_cREPORT_DELETE_FORCE = 16;
	static const tDWORD TaskManager_cREPORT_DELETE_PREV = 32;
	static const tDWORD TaskManager_cREPORT_DELETE_FILES_ONLY = 64;
	#define MACRO_TASKMANAGER_PGINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL(pTYPE_BINARY, 0x82)
	static const PropertyId TaskManager_pgINTERFACE_VERSION = MACRO_TASKMANAGER_PGINTERFACE_VERSION;
	#define MACRO_TASKMANAGER_PGINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x83)
	static const PropertyId TaskManager_pgINTERFACE_COMMENT = MACRO_TASKMANAGER_PGINTERFACE_COMMENT;
	#define MACRO_TASKMANAGER_PGTM_PRODUCT_CONFIG mPROPERTY_MAKE_GLOBAL(pTYPE_PTR, 0x1000)
	static const PropertyId TaskManager_pgTM_PRODUCT_CONFIG = MACRO_TASKMANAGER_PGTM_PRODUCT_CONFIG;
	#define MACRO_TASKMANAGER_PGBL_PRODUCT_NAME mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1004)
	static const PropertyId TaskManager_pgBL_PRODUCT_NAME = MACRO_TASKMANAGER_PGBL_PRODUCT_NAME;
	#define MACRO_TASKMANAGER_PGBL_PRODUCT_VERSION mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1005)
	static const PropertyId TaskManager_pgBL_PRODUCT_VERSION = MACRO_TASKMANAGER_PGBL_PRODUCT_VERSION;
	#define MACRO_TASKMANAGER_PGBL_PRODUCT_PATH mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1006)
	static const PropertyId TaskManager_pgBL_PRODUCT_PATH = MACRO_TASKMANAGER_PGBL_PRODUCT_PATH;
	#define MACRO_TASKMANAGER_PGBL_PRODUCT_DATA_PATH mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1007)
	static const PropertyId TaskManager_pgBL_PRODUCT_DATA_PATH = MACRO_TASKMANAGER_PGBL_PRODUCT_DATA_PATH;
	#define MACRO_TASKMANAGER_PGBL_COMPANY_NAME mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1008)
	static const PropertyId TaskManager_pgBL_COMPANY_NAME = MACRO_TASKMANAGER_PGBL_COMPANY_NAME;
	#define MACRO_TASKMANAGER_PGTM_IS_GUI_CONNECTED mPROPERTY_MAKE_GLOBAL(pTYPE_INT, 0x1009)
	static const PropertyId TaskManager_pgTM_IS_GUI_CONNECTED = MACRO_TASKMANAGER_PGTM_IS_GUI_CONNECTED;
	static const MessageClassId TaskManager_pmcPROFILE = 0x1000;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_STATE_CHANDED = 0x1000;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_CREATE = 0x1002;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_DELETE = 0x1003;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_MODIFIED = 0x1004;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_SETTINGS_MODIFIED = 0x1005;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_TASK_DISABLED_ON_RUN = 0x1006;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_LICENCE_CHECK_ERR = 0x1007;
	static const MessageId TaskManager_pmcPROFILE_pmPROFILE_TASK_DISABLED_IN_SAFE_MODE = 0x1008;
	static const MessageClassId TaskManager_pmcASK_ACTION = 0x1001;
	static const MessageClassId TaskManager_pmcTM_STARTUP_SCHEDULE = 0x1002;
	static const MessageClassId TaskManager_pmcTM_EVENTS = 0x208ad891;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_START = 0x384e7d1c;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_STARTUP_TASKS_GOING_TO_START = 0xffff2a49;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_STARTUP_TASKS_STARTED = 0xf927d6d6;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_OS_SAFE_MODE = 0x208ad891;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_GUI_CONNECTED = 0x506e3ae5;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_GUI_DISCONNECTED = 0x387e4b05;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_INET_CONNECTED = 0xf647945a;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_INET_DISCONNECTED = 0x844afb85;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_TASK_SETTINGS_CHANGED = 0x5a2b81f1;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_GOING_TO_STOP = 0xcdf441e7;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_INSTALL = 0xffff2a4a;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_IDLE = 0xffff2a4b;
	static const MessageId TaskManager_pmcTM_EVENTS_pmTM_EVENT_CHECK_STOP_ALLOWED = 0xffff2a4c;
	struct iTaskManagerVtbl;
	typedef struct iTaskManagerVtbl iTaskManagerVtbl;
	
	typedef struct tag_hTaskManager {
		const iTaskManagerVtbl* vtbl;
		const iSYSTEMVtbl* sys;
	} *hTaskManager;
	typedef tERROR (pr_call *fnpTaskManager_Init) (hTaskManager _this, tDWORD flags);
	typedef tERROR (pr_call *fnpTaskManager_Exit) (hTaskManager _this, tDWORD cause);
	typedef tERROR (pr_call *fnpTaskManager_GetProfileInfo) (hTaskManager _this, const tCHAR* profile_name, cSerializable* info);
	typedef tERROR (pr_call *fnpTaskManager_SetProfileInfo) (hTaskManager _this, const tCHAR* profile_name, const cSerializable* info, hOBJECT client, tDWORD delay);
	typedef tERROR (pr_call *fnpTaskManager_EnableProfile) (hTaskManager _this, const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_IsProfileEnabled) (hTaskManager _this, const tCHAR* profile_name, tBOOL* enabled);
	typedef tERROR (pr_call *fnpTaskManager_DeleteProfile) (hTaskManager _this, const tCHAR* profile_name, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_SetProfileState) (hTaskManager _this, const tCHAR* profile_name, TaskManager_tTaskRequestState state, hOBJECT client, tDWORD delay, tDWORD* task_id);
	typedef tERROR (pr_call *fnpTaskManager_GetProfileReport) (hTaskManager _this, const tCHAR* profile_name, const tCHAR* report_id, hOBJECT client, tDWORD p_access, hREPORT* report);
	typedef tERROR (pr_call *fnpTaskManager_CloneProfile) (hTaskManager _this, const tCHAR* profile_name, TaskManager_cProfile* profile, tDWORD flags, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_GetSettingsByLevel) (hTaskManager _this, const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, cSerializable* settings);
	typedef tERROR (pr_call *fnpTaskManager_GetProfilePersistentStorage) (hTaskManager _this, const tCHAR* profile_name, hOBJECT dad, hREGISTRY* storage, tBOOL rw);
	typedef tERROR (pr_call *fnpTaskManager_OpenTask) (hTaskManager _this, TaskManager_tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_ReleaseTask) (hTaskManager _this, TaskManager_tTaskId task_id);
	typedef tERROR (pr_call *fnpTaskManager_GetTaskState) (hTaskManager _this, TaskManager_tTaskId task_id, TaskManager_tTaskState* task_state);
	typedef tERROR (pr_call *fnpTaskManager_SetTaskState) (hTaskManager _this, TaskManager_tTaskId task_id, TaskManager_tTaskRequestState requested_state, hOBJECT client, tDWORD delay);
	typedef tERROR (pr_call *fnpTaskManager_GetTaskInfo) (hTaskManager _this, TaskManager_tTaskId task_id, cSerializable* info);
	typedef tERROR (pr_call *fnpTaskManager_SetTaskInfo) (hTaskManager _this, TaskManager_tTaskId task_id, const cSerializable* info);
	typedef tERROR (pr_call *fnpTaskManager_GetTaskReport) (hTaskManager _this, TaskManager_tTaskId task_id, const tCHAR* report_id, hOBJECT client, tDWORD p_access, hREPORT* report);
	typedef tERROR (pr_call *fnpTaskManager_GetTaskPersistentStorage) (hTaskManager _this, TaskManager_tTaskId task_id, hOBJECT dad, hREGISTRY* storage, tBOOL rw);
	typedef tERROR (pr_call *fnpTaskManager_AskAction) (hTaskManager _this, TaskManager_tTaskId task_id, tDWORD action_id, cSerializable* params);
	typedef tERROR (pr_call *fnpTaskManager_AskTaskAction) (hTaskManager _this, TaskManager_tTaskId task_id, tDWORD action_id, cSerializable* params);
	typedef tERROR (pr_call *fnpTaskManager_GetService) (hTaskManager _this, TaskManager_tTaskId task_id, hOBJECT client, const tCHAR* service_id, hOBJECT* service, tDWORD start_delay);
	typedef tERROR (pr_call *fnpTaskManager_ReleaseService) (hTaskManager _this, TaskManager_tTaskId task_id, hOBJECT service);
	typedef tERROR (pr_call *fnpTaskManager_LockTaskSettings) (hTaskManager _this, hOBJECT task, cSerializable* settings, tBOOL for_read_only);
	typedef tERROR (pr_call *fnpTaskManager_UnlockTaskSettings) (hTaskManager _this, hOBJECT task, cSerializable* settings, tBOOL modified);
	typedef tERROR (pr_call *fnpTaskManager_RegisterRemoteTask) (hTaskManager _this, hOBJECT tmhost, hOBJECT task, cSerializable* info, hOBJECT* host);
	typedef tERROR (pr_call *fnpTaskManager_RegisterClient) (hTaskManager _this, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_UnregisterClient) (hTaskManager _this, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_CleanupItems) (hTaskManager _this, tDWORD item_type, const tDATETIME* time_stamp, hOBJECT client);
	typedef tERROR (pr_call *fnpTaskManager_AddLicKey) (hTaskManager _this, hSTRING key_name);
	typedef tERROR (pr_call *fnpTaskManager_AddRecordToTaskReport) (hTaskManager _this, TaskManager_tTaskId task_id, tDWORD msg_cls, const cSerializable* record);
	typedef tERROR (pr_call *fnpTaskManager_GetTaskReportInfo) (hTaskManager _this, tDWORD flags, tDWORD index, const cSerializable* info);
	typedef tERROR (pr_call *fnpTaskManager_DeleteTaskReports) (hTaskManager _this, TaskManager_tTaskId task_id, tDWORD flags);
	struct iTaskManagerVtbl {
		fnpTaskManager_Init	Init;
		fnpTaskManager_Exit	Exit;
		fnpTaskManager_GetProfileInfo	GetProfileInfo;
		fnpTaskManager_SetProfileInfo	SetProfileInfo;
		fnpTaskManager_EnableProfile	EnableProfile;
		fnpTaskManager_IsProfileEnabled	IsProfileEnabled;
		fnpTaskManager_DeleteProfile	DeleteProfile;
		fnpTaskManager_SetProfileState	SetProfileState;
		fnpTaskManager_GetProfileReport	GetProfileReport;
		fnpTaskManager_CloneProfile	CloneProfile;
		fnpTaskManager_GetSettingsByLevel	GetSettingsByLevel;
		fnpTaskManager_GetProfilePersistentStorage	GetProfilePersistentStorage;
		fnpTaskManager_OpenTask	OpenTask;
		fnpTaskManager_ReleaseTask	ReleaseTask;
		fnpTaskManager_GetTaskState	GetTaskState;
		fnpTaskManager_SetTaskState	SetTaskState;
		fnpTaskManager_GetTaskInfo	GetTaskInfo;
		fnpTaskManager_SetTaskInfo	SetTaskInfo;
		fnpTaskManager_GetTaskReport	GetTaskReport;
		fnpTaskManager_GetTaskPersistentStorage	GetTaskPersistentStorage;
		fnpTaskManager_AskAction	AskAction;
		fnpTaskManager_AskTaskAction	AskTaskAction;
		fnpTaskManager_GetService	GetService;
		fnpTaskManager_ReleaseService	ReleaseService;
		fnpTaskManager_LockTaskSettings	LockTaskSettings;
		fnpTaskManager_UnlockTaskSettings	UnlockTaskSettings;
		fnpTaskManager_RegisterRemoteTask	RegisterRemoteTask;
		fnpTaskManager_RegisterClient	RegisterClient;
		fnpTaskManager_UnregisterClient	UnregisterClient;
		fnpTaskManager_CleanupItems	CleanupItems;
		fnpTaskManager_AddLicKey	AddLicKey;
		fnpTaskManager_AddRecordToTaskReport	AddRecordToTaskReport;
		fnpTaskManager_GetTaskReportInfo	GetTaskReportInfo;
		fnpTaskManager_DeleteTaskReports	DeleteTaskReports;
	};
	#define CALL_TaskManager_Init(_this, flags)	((_this)->vtbl->Init((_this), flags))
	#define CALL_TaskManager_Exit(_this, cause)	((_this)->vtbl->Exit((_this), cause))
	#define CALL_TaskManager_GetProfileInfo(_this, profile_name, info)	((_this)->vtbl->GetProfileInfo((_this), profile_name, info))
	#define CALL_TaskManager_SetProfileInfo(_this, profile_name, info, client, delay)	((_this)->vtbl->SetProfileInfo((_this), profile_name, info, client, delay))
	#define CALL_TaskManager_EnableProfile(_this, profile_name, enabled, recursive, client)	((_this)->vtbl->EnableProfile((_this), profile_name, enabled, recursive, client))
	#define CALL_TaskManager_IsProfileEnabled(_this, profile_name, enabled)	((_this)->vtbl->IsProfileEnabled((_this), profile_name, enabled))
	#define CALL_TaskManager_DeleteProfile(_this, profile_name, client)	((_this)->vtbl->DeleteProfile((_this), profile_name, client))
	#define CALL_TaskManager_SetProfileState(_this, profile_name, state, client, delay, task_id)	((_this)->vtbl->SetProfileState((_this), profile_name, state, client, delay, task_id))
	#define CALL_TaskManager_GetProfileReport(_this, profile_name, report_id, client, p_access, report)	((_this)->vtbl->GetProfileReport((_this), profile_name, report_id, client, p_access, report))
	#define CALL_TaskManager_CloneProfile(_this, profile_name, profile, flags, client)	((_this)->vtbl->CloneProfile((_this), profile_name, profile, flags, client))
	#define CALL_TaskManager_GetSettingsByLevel(_this, profile_name, settings_level, cumulative, settings)	((_this)->vtbl->GetSettingsByLevel((_this), profile_name, settings_level, cumulative, settings))
	#define CALL_TaskManager_GetProfilePersistentStorage(_this, profile_name, dad, storage, rw)	((_this)->vtbl->GetProfilePersistentStorage((_this), profile_name, dad, storage, rw))
	#define CALL_TaskManager_OpenTask(_this, task_id, profile_name, open_flags, client)	((_this)->vtbl->OpenTask((_this), task_id, profile_name, open_flags, client))
	#define CALL_TaskManager_ReleaseTask(_this, task_id)	((_this)->vtbl->ReleaseTask((_this), task_id))
	#define CALL_TaskManager_GetTaskState(_this, task_id, task_state)	((_this)->vtbl->GetTaskState((_this), task_id, task_state))
	#define CALL_TaskManager_SetTaskState(_this, task_id, requested_state, client, delay)	((_this)->vtbl->SetTaskState((_this), task_id, requested_state, client, delay))
	#define CALL_TaskManager_GetTaskInfo(_this, task_id, info)	((_this)->vtbl->GetTaskInfo((_this), task_id, info))
	#define CALL_TaskManager_SetTaskInfo(_this, task_id, info)	((_this)->vtbl->SetTaskInfo((_this), task_id, info))
	#define CALL_TaskManager_GetTaskReport(_this, task_id, report_id, client, p_access, report)	((_this)->vtbl->GetTaskReport((_this), task_id, report_id, client, p_access, report))
	#define CALL_TaskManager_GetTaskPersistentStorage(_this, task_id, dad, storage, rw)	((_this)->vtbl->GetTaskPersistentStorage((_this), task_id, dad, storage, rw))
	#define CALL_TaskManager_AskAction(_this, task_id, action_id, params)	((_this)->vtbl->AskAction((_this), task_id, action_id, params))
	#define CALL_TaskManager_AskTaskAction(_this, task_id, action_id, params)	((_this)->vtbl->AskTaskAction((_this), task_id, action_id, params))
	#define CALL_TaskManager_GetService(_this, task_id, client, service_id, service, start_delay)	((_this)->vtbl->GetService((_this), task_id, client, service_id, service, start_delay))
	#define CALL_TaskManager_ReleaseService(_this, task_id, service)	((_this)->vtbl->ReleaseService((_this), task_id, service))
	#define CALL_TaskManager_LockTaskSettings(_this, task, settings, for_read_only)	((_this)->vtbl->LockTaskSettings((_this), task, settings, for_read_only))
	#define CALL_TaskManager_UnlockTaskSettings(_this, task, settings, modified)	((_this)->vtbl->UnlockTaskSettings((_this), task, settings, modified))
	#define CALL_TaskManager_RegisterRemoteTask(_this, tmhost, task, info, host)	((_this)->vtbl->RegisterRemoteTask((_this), tmhost, task, info, host))
	#define CALL_TaskManager_RegisterClient(_this, client)	((_this)->vtbl->RegisterClient((_this), client))
	#define CALL_TaskManager_UnregisterClient(_this, client)	((_this)->vtbl->UnregisterClient((_this), client))
	#define CALL_TaskManager_CleanupItems(_this, item_type, time_stamp, client)	((_this)->vtbl->CleanupItems((_this), item_type, time_stamp, client))
	#define CALL_TaskManager_AddLicKey(_this, key_name)	((_this)->vtbl->AddLicKey((_this), key_name))
	#define CALL_TaskManager_AddRecordToTaskReport(_this, task_id, msg_cls, record)	((_this)->vtbl->AddRecordToTaskReport((_this), task_id, msg_cls, record))
	#define CALL_TaskManager_GetTaskReportInfo(_this, flags, index, info)	((_this)->vtbl->GetTaskReportInfo((_this), flags, index, info))
	#define CALL_TaskManager_DeleteTaskReports(_this, task_id, flags)	((_this)->vtbl->DeleteTaskReports((_this), task_id, flags))
	static const tERROR TaskManager_errDO_IT_YOURSELF = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 0 /*incremental code*/);
	static const tERROR TaskManager_warnTASK_STATE_CHANGED = PR_MAKE_ERR(0 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 1 /*incremental code*/);
	static const tERROR TaskManager_warnTASK_ALREADY_IN_STATE = PR_MAKE_ERR(0 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 2 /*incremental code*/);
	static const tERROR TaskManager_warnTASK_NEED_SAVE_SETTINGS = PR_MAKE_ERR(0 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 3 /*incremental code*/);
	static const tERROR TaskManager_errTASK_CANNOT_GO_TO_STATE = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 4 /*incremental code*/);
	//	end of interface tag_hTaskManager definition
	
	//	interface tag_hTmHost definition
	#define MACRO_TMHOST_ID 0xbf8b
	static const InterfaceId TmHost_IID = MACRO_TMHOST_ID;
	#define MACRO_TMHOST_PGINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL(pTYPE_BINARY, 0x82)
	static const PropertyId TmHost_pgINTERFACE_VERSION = MACRO_TMHOST_PGINTERFACE_VERSION;
	#define MACRO_TMHOST_PGINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x83)
	static const PropertyId TmHost_pgINTERFACE_COMMENT = MACRO_TMHOST_PGINTERFACE_COMMENT;
	struct iTmHostVtbl;
	typedef struct iTmHostVtbl iTmHostVtbl;
	
	typedef struct tag_hTmHost {
		const iTmHostVtbl* vtbl;
		const iSYSTEMVtbl* sys;
	} *hTmHost;
	typedef tERROR (pr_call *fnpTmHost_CreateTask) (hTmHost _this, hOBJECT host, cSerializable* info, hOBJECT* task);
	typedef tERROR (pr_call *fnpTmHost_DestroyTask) (hTmHost _this, hOBJECT task);
	struct iTmHostVtbl {
		fnpTmHost_CreateTask	CreateTask;
		fnpTmHost_DestroyTask	DestroyTask;
	};
	#define CALL_TmHost_CreateTask(_this, host, info, task)	((_this)->vtbl->CreateTask((_this), host, info, task))
	#define CALL_TmHost_DestroyTask(_this, task)	((_this)->vtbl->DestroyTask((_this), task))
	//	end of interface tag_hTmHost definition
	



#else  // !defined(__cplusplus) || defined(_USE_VTBL)


struct IDL_NAME(cTaskManager);
typedef IDL_NAME(cTaskManager)* IDL_NAME(hTASKMANAGER);
struct IDL_NAME(cTmHost);
typedef IDL_NAME(cTmHost)* IDL_NAME(hTMHOST);
struct IDL_NAME(cRegistry);
typedef IDL_NAME(cRegistry)* IDL_NAME(hREGISTRY);
struct IDL_NAME(cReport);
typedef IDL_NAME(cReport)* IDL_NAME(hREPORT);
struct IDL_NAME(cString);
typedef IDL_NAME(cString)* IDL_NAME(hSTRING);


//	interface IDL_NAME(cTaskManager) definition
struct pr_abstract TaskManager : public StdBase {
	static const InterfaceId IID = 0x27;
	typedef tDWORD tTaskId;
	typedef tDWORD tServiceId;
	typedef tDWORD tTaskRequestState;
	typedef tDWORD tTaskState;
	typedef tDWORD cProfile;
	typedef tDWORD cProfileEx;
	typedef tDWORD cProfileList;
	typedef tDWORD tGetProfileInfoSource;
	static const tDWORD IDL_NAME(OPEN_TASK_MODE_CREATE) = 2147483648;
	static const tDWORD IDL_NAME(OPEN_TASK_MODE_OPEN) = 1073741824;
	static const tDWORD IDL_NAME(OPEN_TASK_MODE_OPEN_ALWAYS) = 3221225472;
	static const tDWORD IDL_NAME(OPEN_TASK_MODE_MASK) = 3221225472;
	static const tDWORD IDL_NAME(OPEN_TASK_MODE_CLOSE_TASK_IF_OWNER_DIED) = 536870912;
	static const tDWORD IDL_NAME(fPROFILE_TEMPORARY) = 1;
	static const tDWORD IDL_NAME(cCLEAN_REPORTS) = 1;
	static const tDWORD IDL_NAME(cCLEAN_BACKUP_ITEMS) = 2;
	static const tDWORD IDL_NAME(cCLEAN_QUARANTINE_ITEMS) = 4;
	static const tDWORD IDL_NAME(cCLEAN_QB_ITEMS) = 6;
	static const tDWORD IDL_NAME(cREQUEST_SYNCHRONOUS) = 4294967295;
	static const tDWORD IDL_NAME(cREQUEST_DELAY_UP_TO_INTERNET_CONNECTION) = 2147483648;
	static const tDWORD IDL_NAME(cREQUEST_DELAY_MAKE_PERSISTENT) = 1073741824;
	static const tDWORD IDL_NAME(cREQUEST_RESTART_PREVIOUS_TASK) = 536870912;
	static const tDWORD IDL_NAME(cREQUEST_DONOT_FORCE_RUN) = 268435456;
	static const tDWORD IDL_NAME(cREQUEST_SYNCSTORE_DATA) = 16777216;
	static const tDWORD IDL_NAME(cTM_INIT_FLAG_DEFAULT) = 1;
	static const tDWORD IDL_NAME(cTM_INIT_FLAG_PROFILES) = 4;
	static const tDWORD IDL_NAME(cTM_INIT_FLAG_STARTGUI) = 8;
	static const tDWORD IDL_NAME(cREPORT_FILTER_NONE) = 1;
	static const tDWORD IDL_NAME(cREPORT_FILTER_RECENT) = 2;
	static const tDWORD IDL_NAME(cREPORT_RECORD_NEXT) = 4;
	static const tDWORD IDL_NAME(cREPORT_RECORD_PREV) = 8;
	static const tDWORD IDL_NAME(cREPORT_DELETE_FORCE) = 16;
	static const tDWORD IDL_NAME(cREPORT_DELETE_PREV) = 32;
	static const tDWORD IDL_NAME(cREPORT_DELETE_FILES_ONLY) = 64;
	static const PropertyId IDL_NAME(pgINTERFACE_VERSION) = mPROPERTY_MAKE_GLOBAL(pTYPE_BINARY, 0x82);
	static const PropertyId IDL_NAME(pgINTERFACE_COMMENT) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x83);
	static const PropertyId IDL_NAME(pgTM_PRODUCT_CONFIG) = mPROPERTY_MAKE_GLOBAL(pTYPE_PTR, 0x1000);
	static const PropertyId IDL_NAME(pgBL_PRODUCT_NAME) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1004);
	static const PropertyId IDL_NAME(pgBL_PRODUCT_VERSION) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1005);
	static const PropertyId IDL_NAME(pgBL_PRODUCT_PATH) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1006);
	static const PropertyId IDL_NAME(pgBL_PRODUCT_DATA_PATH) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1007);
	static const PropertyId IDL_NAME(pgBL_COMPANY_NAME) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x1008);
	static const PropertyId IDL_NAME(pgTM_IS_GUI_CONNECTED) = mPROPERTY_MAKE_GLOBAL(pTYPE_INT, 0x1009);
	static const MessageClassId IDL_NAME(pmcPROFILE) = 0x1000;
	static const MessageId pmcPROFILE_pmPROFILE_STATE_CHANDED = 0x1000;
	static const MessageId pmcPROFILE_pmPROFILE_CREATE = 0x1002;
	static const MessageId pmcPROFILE_pmPROFILE_DELETE = 0x1003;
	static const MessageId pmcPROFILE_pmPROFILE_MODIFIED = 0x1004;
	static const MessageId pmcPROFILE_pmPROFILE_SETTINGS_MODIFIED = 0x1005;
	static const MessageId pmcPROFILE_pmPROFILE_TASK_DISABLED_ON_RUN = 0x1006;
	static const MessageId pmcPROFILE_pmPROFILE_LICENCE_CHECK_ERR = 0x1007;
	static const MessageId pmcPROFILE_pmPROFILE_TASK_DISABLED_IN_SAFE_MODE = 0x1008;
	static const MessageClassId IDL_NAME(pmcASK_ACTION) = 0x1001;
	static const MessageClassId IDL_NAME(pmcTM_STARTUP_SCHEDULE) = 0x1002;
	static const MessageClassId IDL_NAME(pmcTM_EVENTS) = 0x208ad891;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_START = 0x384e7d1c;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_STARTUP_TASKS_GOING_TO_START = 0xffff2a49;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_STARTUP_TASKS_STARTED = 0xf927d6d6;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_OS_SAFE_MODE = 0x208ad891;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_GUI_CONNECTED = 0x506e3ae5;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_GUI_DISCONNECTED = 0x387e4b05;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_INET_CONNECTED = 0xf647945a;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_INET_DISCONNECTED = 0x844afb85;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_TASK_SETTINGS_CHANGED = 0x5a2b81f1;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_GOING_TO_STOP = 0xcdf441e7;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_INSTALL = 0xffff2a4a;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_IDLE = 0xffff2a4b;
	static const MessageId pmcTM_EVENTS_pmTM_EVENT_CHECK_STOP_ALLOWED = 0xffff2a4c;
	static const tERROR IDL_NAME(errDO_IT_YOURSELF) = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 0 /*incremental code*/);
	static const tERROR IDL_NAME(warnTASK_STATE_CHANGED) = PR_MAKE_ERR(0 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 1 /*incremental code*/);
	static const tERROR IDL_NAME(warnTASK_ALREADY_IN_STATE) = PR_MAKE_ERR(0 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 2 /*incremental code*/);
	static const tERROR IDL_NAME(warnTASK_NEED_SAVE_SETTINGS) = PR_MAKE_ERR(0 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 3 /*incremental code*/);
	static const tERROR IDL_NAME(errTASK_CANNOT_GO_TO_STATE) = PR_MAKE_ERR(1 /*severity*/, fac_INTERFACE_DECLARED /*facility*/, 0x27 /*implementation id*/, 4 /*incremental code*/);
	virtual tERROR pr_call Init(tDWORD flags) = 0;
	virtual tERROR pr_call Exit(tDWORD cause) = 0;
	virtual tERROR pr_call GetProfileInfo(const tCHAR* profile_name, cSerializable* info) = 0;
	virtual tERROR pr_call SetProfileInfo(const tCHAR* profile_name, const cSerializable* info, hOBJECT client, tDWORD delay) = 0;
	virtual tERROR pr_call EnableProfile(const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, hOBJECT client) = 0;
	virtual tERROR pr_call IsProfileEnabled(const tCHAR* profile_name, tBOOL* enabled) = 0;
	virtual tERROR pr_call DeleteProfile(const tCHAR* profile_name, hOBJECT client) = 0;
	virtual tERROR pr_call SetProfileState(const tCHAR* profile_name, TaskManager::tTaskRequestState state, hOBJECT client, tDWORD delay, tDWORD* task_id) = 0;
	virtual tERROR pr_call GetProfileReport(const tCHAR* profile_name, const tCHAR* report_id, hOBJECT client, tDWORD p_access, IDL_NAME(hREPORT)* report) = 0;
	virtual tERROR pr_call CloneProfile(const tCHAR* profile_name, TaskManager::cProfile* profile, tDWORD flags, hOBJECT client) = 0;
	virtual tERROR pr_call GetSettingsByLevel(const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, cSerializable* settings) = 0;
	virtual tERROR pr_call GetProfilePersistentStorage(const tCHAR* profile_name, hOBJECT dad, hREGISTRY* storage, tBOOL rw) = 0;
	virtual tERROR pr_call OpenTask(TaskManager::tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, hOBJECT client) = 0;
	virtual tERROR pr_call ReleaseTask(TaskManager::tTaskId task_id) = 0;
	virtual tERROR pr_call GetTaskState(TaskManager::tTaskId task_id, TaskManager::tTaskState* task_state) = 0;
	virtual tERROR pr_call SetTaskState(TaskManager::tTaskId task_id, TaskManager::tTaskRequestState requested_state, hOBJECT client, tDWORD delay) = 0;
	virtual tERROR pr_call GetTaskInfo(TaskManager::tTaskId task_id, cSerializable* info) = 0;
	virtual tERROR pr_call SetTaskInfo(TaskManager::tTaskId task_id, const cSerializable* info) = 0;
	virtual tERROR pr_call GetTaskReport(TaskManager::tTaskId task_id, const tCHAR* report_id, hOBJECT client, tDWORD p_access, IDL_NAME(hREPORT)* report) = 0;
	virtual tERROR pr_call GetTaskPersistentStorage(TaskManager::tTaskId task_id, hOBJECT dad, IDL_NAME(hREGISTRY)* storage, tBOOL rw) = 0;
	virtual tERROR pr_call AskAction(TaskManager::tTaskId task_id, tDWORD action_id, cSerializable* params) = 0;
	virtual tERROR pr_call AskTaskAction(TaskManager::tTaskId task_id, tDWORD action_id, cSerializable* params) = 0;
	virtual tERROR pr_call GetService(TaskManager::tTaskId task_id, hOBJECT client, const tCHAR* service_id, hOBJECT* service, tDWORD start_delay) = 0;
	virtual tERROR pr_call ReleaseService(TaskManager::tTaskId task_id, hOBJECT service) = 0;
	virtual tERROR pr_call LockTaskSettings(hOBJECT task, cSerializable* settings, tBOOL for_read_only) = 0;
	virtual tERROR pr_call UnlockTaskSettings(hOBJECT task, cSerializable* settings, tBOOL modified) = 0;
	virtual tERROR pr_call RegisterRemoteTask(hOBJECT tmhost, hOBJECT task, cSerializable* info, hOBJECT* host) = 0;
	virtual tERROR pr_call RegisterClient(hOBJECT client) = 0;
	virtual tERROR pr_call UnregisterClient(hOBJECT client) = 0;
	virtual tERROR pr_call CleanupItems(tDWORD item_type, const tDATETIME* time_stamp, hOBJECT client) = 0;
	virtual tERROR pr_call AddLicKey(IDL_NAME(hSTRING) key_name) = 0;
	virtual tERROR pr_call AddRecordToTaskReport(TaskManager::tTaskId task_id, tDWORD msg_cls, const cSerializable* record) = 0;
	virtual tERROR pr_call GetTaskReportInfo(tDWORD flags, tDWORD index, const cSerializable* info) = 0;
	virtual tERROR pr_call DeleteTaskReports(TaskManager::tTaskId task_id, tDWORD flags) = 0;
};


#if !defined(PR_IDL_LEGACY)

	#define CALL_TaskManager_Init(_this, flags)	((_this)->Init(flags))
	#define CALL_TaskManager_Exit(_this, cause)	((_this)->Exit(cause))
	#define CALL_TaskManager_GetProfileInfo(_this, profile_name, info)	((_this)->GetProfileInfo(profile_name, info))
	#define CALL_TaskManager_SetProfileInfo(_this, profile_name, info, client, delay)	((_this)->SetProfileInfo(profile_name, info, client, delay))
	#define CALL_TaskManager_EnableProfile(_this, profile_name, enabled, recursive, client)	((_this)->EnableProfile(profile_name, enabled, recursive, client))
	#define CALL_TaskManager_IsProfileEnabled(_this, profile_name, enabled)	((_this)->IsProfileEnabled(profile_name, enabled))
	#define CALL_TaskManager_DeleteProfile(_this, profile_name, client)	((_this)->DeleteProfile(profile_name, client))
	#define CALL_TaskManager_SetProfileState(_this, profile_name, state, client, delay, task_id)	((_this)->SetProfileState(profile_name, state, client, delay, task_id))
	#define CALL_TaskManager_GetProfileReport(_this, profile_name, report_id, client, p_access, report)	((_this)->GetProfileReport(profile_name, report_id, client, p_access, report))
	#define CALL_TaskManager_CloneProfile(_this, profile_name, profile, flags, client)	((_this)->CloneProfile(profile_name, profile, flags, client))
	#define CALL_TaskManager_GetSettingsByLevel(_this, profile_name, settings_level, cumulative, settings)	((_this)->GetSettingsByLevel(profile_name, settings_level, cumulative, settings))
	#define CALL_TaskManager_GetProfilePersistentStorage(_this, profile_name, dad, storage, rw)	((_this)->GetProfilePersistentStorage(profile_name, dad, storage, rw))
	#define CALL_TaskManager_OpenTask(_this, task_id, profile_name, open_flags, client)	((_this)->OpenTask(task_id, profile_name, open_flags, client))
	#define CALL_TaskManager_ReleaseTask(_this, task_id)	((_this)->ReleaseTask(task_id))
	#define CALL_TaskManager_GetTaskState(_this, task_id, task_state)	((_this)->GetTaskState(task_id, task_state))
	#define CALL_TaskManager_SetTaskState(_this, task_id, requested_state, client, delay)	((_this)->SetTaskState(task_id, requested_state, client, delay))
	#define CALL_TaskManager_GetTaskInfo(_this, task_id, info)	((_this)->GetTaskInfo(task_id, info))
	#define CALL_TaskManager_SetTaskInfo(_this, task_id, info)	((_this)->SetTaskInfo(task_id, info))
	#define CALL_TaskManager_GetTaskReport(_this, task_id, report_id, client, p_access, report)	((_this)->GetTaskReport(task_id, report_id, client, p_access, report))
	#define CALL_TaskManager_GetTaskPersistentStorage(_this, task_id, dad, storage, rw)	((_this)->GetTaskPersistentStorage(task_id, dad, storage, rw))
	#define CALL_TaskManager_AskAction(_this, task_id, action_id, params)	((_this)->AskAction(task_id, action_id, params))
	#define CALL_TaskManager_AskTaskAction(_this, task_id, action_id, params)	((_this)->AskTaskAction(task_id, action_id, params))
	#define CALL_TaskManager_GetService(_this, task_id, client, service_id, service, start_delay)	((_this)->GetService(task_id, client, service_id, service, start_delay))
	#define CALL_TaskManager_ReleaseService(_this, task_id, service)	((_this)->ReleaseService(task_id, service))
	#define CALL_TaskManager_LockTaskSettings(_this, task, settings, for_read_only)	((_this)->LockTaskSettings(task, settings, for_read_only))
	#define CALL_TaskManager_UnlockTaskSettings(_this, task, settings, modified)	((_this)->UnlockTaskSettings(task, settings, modified))
	#define CALL_TaskManager_RegisterRemoteTask(_this, tmhost, task, info, host)	((_this)->RegisterRemoteTask(tmhost, task, info, host))
	#define CALL_TaskManager_RegisterClient(_this, client)	((_this)->RegisterClient(client))
	#define CALL_TaskManager_UnregisterClient(_this, client)	((_this)->UnregisterClient(client))
	#define CALL_TaskManager_CleanupItems(_this, item_type, time_stamp, client)	((_this)->CleanupItems(item_type, time_stamp, client))
	#define CALL_TaskManager_AddLicKey(_this, key_name)	((_this)->AddLicKey(key_name))
	#define CALL_TaskManager_AddRecordToTaskReport(_this, task_id, msg_cls, record)	((_this)->AddRecordToTaskReport(task_id, msg_cls, record))
	#define CALL_TaskManager_GetTaskReportInfo(_this, flags, index, info)	((_this)->GetTaskReportInfo(flags, index, info))
	#define CALL_TaskManager_DeleteTaskReports(_this, task_id, flags)	((_this)->DeleteTaskReports(task_id, flags))


#endif // !defined(PR_IDL_LEGACY)


struct pr_abstract IDL_NAME(cTaskManager) : public TaskManager, public iObj {
	OBJ_IMPL( IDL_NAME(cTaskManager) );
};
//	end of interface IDL_NAME(cTaskManager) definition

//	interface IDL_NAME(cTmHost) definition
struct pr_abstract TmHost : public StdBase {
	static const InterfaceId IID = 0xbf8b;
	static const PropertyId IDL_NAME(pgINTERFACE_VERSION) = mPROPERTY_MAKE_GLOBAL(pTYPE_BINARY, 0x82);
	static const PropertyId IDL_NAME(pgINTERFACE_COMMENT) = mPROPERTY_MAKE_GLOBAL(pTYPE_STRING, 0x83);
	virtual tERROR pr_call CreateTask(hOBJECT host, cSerializable* info, hOBJECT* task) = 0;
	virtual tERROR pr_call DestroyTask(hOBJECT task) = 0;
};


#if !defined(PR_IDL_LEGACY)

	#define CALL_TmHost_CreateTask(_this, host, info, task)	((_this)->CreateTask(host, info, task))
	#define CALL_TmHost_DestroyTask(_this, task)	((_this)->DestroyTask(task))


#endif // !defined(PR_IDL_LEGACY)


struct pr_abstract IDL_NAME(cTmHost) : public TmHost, public iObj {
	OBJ_IMPL( IDL_NAME(cTmHost) );
};
//	end of interface IDL_NAME(cTmHost) definition




#endif  // !defined(__cplusplus) || defined(_USE_VTBL)



#endif //idl_i_tm_h




