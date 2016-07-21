#include "stdafx.h"
#include "rpc_bl.h"
#include <iface/i_taskmanager.h>
#include <iface/i_tmclient.h>
#include <iface/i_tmhost.h>

tERROR pr_call Local_TaskManager_GetProfileInfo( hTASKMANAGER _this, const tCHAR* profile_name, cSerializable* info ) {		// -- ;
	PR_PROXY_CALL_EX( TaskManager_GetProfileInfo( PR_THIS_PROXY, profile_name, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_TaskManager_SetProfileInfo( hTASKMANAGER _this, const tCHAR* profile_name, const cSerializable* info, hOBJECT client, tDWORD delay ) {		// -- ;
	PR_PROXY_CALL_EX( TaskManager_SetProfileInfo( PR_THIS_PROXY, profile_name, PR_SERIALIZE(info), PR_REMOTE(client), delay ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_TaskManager_EnableProfile( hTASKMANAGER _this, const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, hOBJECT client ) {		// -- ;
	PR_PROXY_CALL( TaskManager_EnableProfile( PR_THIS_PROXY, profile_name, enabled, recursive, PR_REMOTE(client) ) );
}
tERROR pr_call Local_TaskManager_IsProfileEnabled( hTASKMANAGER _this, const tCHAR* profile_name, tBOOL* enabled ) {		// -- ;
	PR_PROXY_CALL( TaskManager_IsProfileEnabled( PR_THIS_PROXY, profile_name, enabled ) );
}
tERROR pr_call Local_TaskManager_DeleteProfile( hTASKMANAGER _this, const tCHAR* profile_name, hOBJECT client ) {		// -- ;
	PR_PROXY_CALL( TaskManager_DeleteProfile( PR_THIS_PROXY, profile_name, PR_REMOTE(client) ) );
}
tERROR pr_call Local_TaskManager_GetSettingsByLevel( hTASKMANAGER _this, const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, cSerializable* settings ) {		// -- ;
	PR_PROXY_CALL_EX( TaskManager_GetSettingsByLevel( PR_THIS_PROXY, profile_name, settings_level, cumulative, PR_SERIALIZE(settings) ), PR_SERCLEANUP(settings) );
}
tERROR pr_call Local_TaskManager_GetProfilePersistentStorage( hTASKMANAGER _this, const tCHAR* profile_name, hOBJECT dad, hREGISTRY* storage, tBOOL rw ) { // -- ;
	PR_PROXY_CALL( TaskManager_GetProfilePersistentStorage( PR_THIS_PROXY, profile_name, PR_REMOTE(dad), PR_OUT_LOCAL(storage), rw ) );
}
tERROR pr_call Local_TaskManager_OpenTask( hTASKMANAGER _this, tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, hOBJECT client ) {		// -- ;
	PR_PROXY_CALL( TaskManager_OpenTask( PR_THIS_PROXY, task_id, profile_name, open_flags, PR_REMOTE(client) ) );
}
tERROR pr_call Local_TaskManager_ReleaseTask( hTASKMANAGER _this, tTaskId task_id ) {		// -- ;
	PR_PROXY_CALL( TaskManager_ReleaseTask( PR_THIS_PROXY, task_id ) );
}
tERROR pr_call Local_TaskManager_GetTaskState( hTASKMANAGER _this, tTaskId task_id, tTaskState* task_state ) {		// -- ;
	PR_PROXY_CALL( TaskManager_GetTaskState( PR_THIS_PROXY, task_id, (tDWORD*) task_state ) );
}
tERROR pr_call Local_TaskManager_SetTaskState( hTASKMANAGER _this, tTaskId task_id, tTaskRequestState requested_state, hOBJECT client, tDWORD delay ) {		// -- ;
	PR_PROXY_CALL( TaskManager_SetTaskState( PR_THIS_PROXY, task_id, requested_state, PR_REMOTE(client), delay ) );
}
tERROR pr_call Local_TaskManager_GetTaskInfo( hTASKMANAGER _this, tTaskId task_id, cSerializable* info ) {		// -- ;
	PR_PROXY_CALL_EX( TaskManager_GetTaskInfo( PR_THIS_PROXY, task_id, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_TaskManager_SetTaskInfo( hTASKMANAGER _this, tTaskId task_id, const cSerializable* info ) {		// -- ;
	PR_PROXY_CALL_EX( TaskManager_SetTaskInfo( PR_THIS_PROXY, task_id, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_TaskManager_DeleteTaskReports( hTASKMANAGER _this, tTaskId task_id, tBOOL force_delete_running_reports ) {		// -- ;
	PR_PROXY_CALL( TaskManager_DeleteTaskReports( PR_THIS_PROXY, task_id, force_delete_running_reports ) );
}

tERROR pr_call Local_TaskManager_AskAction( hTASKMANAGER _this, tTaskId task_id, tDWORD action_id, cSerializable* params ) {
	PR_PROXY_CALL_EX( TaskManager_AskAction( PR_THIS_PROXY, task_id, action_id, PR_SERIALIZE(params) ), PR_SERCLEANUP(params) );
}

tERROR pr_call Local_TaskManager_AskTaskAction( hTASKMANAGER _this, tTaskId task_id, tDWORD action_id, cSerializable* params ) {
	PR_PROXY_CALL_EX( TaskManager_AskTaskAction( PR_THIS_PROXY, task_id, action_id, PR_SERIALIZE(params) ), PR_SERCLEANUP(params) );
}

tERROR pr_call Local_TaskManager_GetService( hTASKMANAGER _this, tTaskId task_id, hOBJECT client, const tCHAR* service_id, hOBJECT* service, tDWORD delay ) {
	PR_PROXY_CALL( TaskManager_GetService( PR_THIS_PROXY, task_id, PR_REMOTE(client), service_id, PR_OUT_LOCAL(service), delay ) );
}

tERROR pr_call Local_TaskManager_ReleaseService( hTASKMANAGER _this, tTaskId task_id, hOBJECT service ) {
	PR_PROXY_CALL( TaskManager_ReleaseService( PR_THIS_PROXY, task_id, PR_REMOTE_DEREF(service) ) );
}

tERROR pr_call Local_TaskManager_RegisterRemoteTask( hTASKMANAGER _this, hOBJECT tmhost, hOBJECT task, cSerializable* info, hOBJECT* host ) {
	PR_PROXY_CALL_EX( TaskManager_RegisterRemoteTask( PR_THIS_PROXY, PR_REMOTE(tmhost), PR_REMOTE(task), PR_SERIALIZE(info), PR_OUT_LOCAL(host) ), PR_SERCLEANUP(info) );
}

tERROR pr_call Local_TaskManager_SetProfileState( hTASKMANAGER _this, const tCHAR* profile_name, tTaskRequestState state, hOBJECT client, tDWORD delay, tDWORD* task_id ) {
	PR_PROXY_CALL( TaskManager_SetProfileState( PR_THIS_PROXY, profile_name, state, PR_REMOTE(client), delay, task_id ) );
}

tERROR pr_call Local_TaskManager_GetProfileReport( hTASKMANAGER _this, const tCHAR* profile_name, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ) {
	PR_PROXY_CALL( TaskManager_GetProfileReport( PR_THIS_PROXY, profile_name, report_id, PR_REMOTE(client), access, PR_OUT_LOCAL(report) ) );
}

tERROR pr_call Local_TaskManager_CloneProfile( hTASKMANAGER _this, const tCHAR* profile_name, cProfile* profile, tDWORD flags, hOBJECT client ) {
	PR_PROXY_CALL_EX( TaskManager_CloneProfile( PR_THIS_PROXY, profile_name, PR_SERIALIZE(profile), flags, PR_REMOTE(client) ), PR_SERCLEANUP(profile) );
}

tERROR pr_call Local_TaskManager_GetTaskReport( hTASKMANAGER _this, tTaskId task_id, const tCHAR* report_id, hOBJECT client, tDWORD access, hREPORT* report ) {
	PR_PROXY_CALL( TaskManager_GetTaskReport( PR_THIS_PROXY, task_id, report_id, PR_REMOTE(client), access, PR_OUT_LOCAL(report) ) );
}

tERROR pr_call Local_TaskManager_GetTaskPersistentStorage( hTASKMANAGER _this, tTaskId task_id, hOBJECT dad, hREGISTRY* storage, tBOOL rw ) { // -- ;
	PR_PROXY_CALL( TaskManager_GetTaskPersistentStorage( PR_THIS_PROXY, task_id, PR_REMOTE(dad), PR_OUT_LOCAL(storage), rw ) );
}

tERROR pr_call Local_TaskManager_AddRecordToTaskReport( hTASKMANAGER _this, tTaskId task_id, tDWORD msg_cls, const cSerializable* info ) { // -- ;
	PR_PROXY_CALL_EX( TaskManager_AddRecordToTaskReport( PR_THIS_PROXY, task_id, msg_cls, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}

tERROR pr_call Local_TaskManager_RegisterClient( hTASKMANAGER _this, hOBJECT client ) {
	PR_PROXY_CALL( TaskManager_RegisterClient( PR_THIS_PROXY, PR_REMOTE(client) ) );
}

tERROR pr_call Local_TaskManager_UnregisterClient( hTASKMANAGER _this, hOBJECT client ) {
	PR_PROXY_CALL( TaskManager_UnregisterClient( PR_THIS_PROXY, PR_REMOTE(client) ) );
}

tERROR pr_call Local_TaskManager_CleanupItems( hTASKMANAGER _this, tDWORD item_type, const tDATETIME* time_stamp, hOBJECT client ) {
	PR_PROXY_CALL( TaskManager_CleanupItems( PR_THIS_PROXY, item_type, time_stamp, PR_REMOTE(client) ) );
}

tERROR pr_call Local_TaskManager_AddLicKey( hTASKMANAGER _this, hSTRING key_name ) {
	PR_PROXY_CALL( TaskManager_AddLicKey( PR_THIS_PROXY, PR_REMOTE(key_name) ) );
}

tERROR pr_call Local_TaskManager_Init( hTASKMANAGER _this, tDWORD flags ) {
	PR_PROXY_CALL( TaskManager_Init( PR_THIS_PROXY, flags ) );
}

tERROR pr_call Local_TaskManager_Exit( hTASKMANAGER _this, tDWORD cause ) {
	PR_PROXY_CALL( TaskManager_Exit( PR_THIS_PROXY, cause ) );
}

tERROR pr_call Local_TaskManager_GetTaskReportInfo( hTASKMANAGER _this, tDWORD flags, tDWORD index, const cSerializable* info ) {
	PR_PROXY_CALL_EX( TaskManager_GetTaskReportInfo( PR_THIS_PROXY, flags, index, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}

iTaskManagerVtbl taskmanager_iface = {
	Local_TaskManager_Init,
	Local_TaskManager_Exit,
	Local_TaskManager_GetProfileInfo,
	Local_TaskManager_SetProfileInfo,
	Local_TaskManager_EnableProfile,
	Local_TaskManager_IsProfileEnabled,
	Local_TaskManager_DeleteProfile,
	Local_TaskManager_SetProfileState,
	Local_TaskManager_GetProfileReport,
	Local_TaskManager_CloneProfile,
	Local_TaskManager_GetSettingsByLevel,
	Local_TaskManager_GetProfilePersistentStorage,

	Local_TaskManager_OpenTask,
	Local_TaskManager_ReleaseTask,
	Local_TaskManager_GetTaskState,
	Local_TaskManager_SetTaskState,
	Local_TaskManager_GetTaskInfo,
	Local_TaskManager_SetTaskInfo,
	Local_TaskManager_GetTaskReport,
	Local_TaskManager_GetTaskPersistentStorage,

	Local_TaskManager_AskAction,
	Local_TaskManager_AskTaskAction,
	Local_TaskManager_GetService,
	Local_TaskManager_ReleaseService,
	NULL,
	NULL,
	Local_TaskManager_RegisterRemoteTask,

	Local_TaskManager_RegisterClient,
	Local_TaskManager_UnregisterClient,

	Local_TaskManager_CleanupItems,
	Local_TaskManager_AddLicKey,

	Local_TaskManager_AddRecordToTaskReport,
	Local_TaskManager_GetTaskReportInfo,
	Local_TaskManager_DeleteTaskReports,
};

PR_IMPLEMENT_PROXY(IID_TASKMANAGER, taskmanager_iface)


tERROR pr_call Local_TmClient_OpenRequest( hTM_CLIENT _this, tDWORD request_id, tDWORD request_type, cSerializable* params ) {
	PR_PROXY_CALL( TmClient_OpenRequest( PR_THIS_PROXY, request_id, request_type, PR_SERIALIZE(params) ) );
}

tERROR pr_call Local_TmClient_ProcessRequest( hTM_CLIENT _this, tDWORD request_id, tDWORD request_type, cSerializable* params ) {
	PR_PROXY_CALL( TmClient_ProcessRequest( PR_THIS_PROXY, request_id, request_type, PR_SERIALIZE(params) ) );
}

tERROR pr_call Local_TmClient_CloseRequest( hTM_CLIENT _this, tDWORD request_id ) {
	PR_PROXY_CALL( TmClient_CloseRequest( PR_THIS_PROXY, request_id ) );
}

tERROR pr_call Local_TmClient_Exit( hTM_CLIENT _this, tDWORD cause ) {
	PR_PROXY_CALL( TmClient_Exit( PR_THIS_PROXY, cause ) );
}

iTmClientVtbl tmclient_iface = {
	Local_TmClient_OpenRequest,
	Local_TmClient_ProcessRequest,
	Local_TmClient_CloseRequest,
	Local_TmClient_Exit
};

PR_IMPLEMENT_PROXY(IID_TM_CLIENT, tmclient_iface)


tERROR pr_call Local_TmHost_CreateTask( hTMHOST _this, hOBJECT host, cSerializable* info, hOBJECT* task ) {		// -- ;
	PR_PROXY_CALL_EX( TmHost_CreateTask( PR_THIS_PROXY, PR_REMOTE(host), PR_SERIALIZE(info), PR_OUT_LOCAL(task) ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_TmHost_DestroyTask( hTMHOST _this, hOBJECT task ) {		// -- ;
	PR_PROXY_CALL( TmHost_DestroyTask( PR_THIS_PROXY, PR_REMOTE_DEREF(task) ) );
}

iTmHostVtbl tmhost_iface = {
	Local_TmHost_CreateTask,
	Local_TmHost_DestroyTask,
};

PR_IMPLEMENT_PROXY(IID_TMHOST, tmhost_iface)
