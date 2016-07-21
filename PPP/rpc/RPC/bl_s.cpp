#include "stdafx.h"
#include "rpc_bl.h"
#include <iface/i_taskmanager.h>
#include <iface/i_tmclient.h>
#include <iface/i_tmhost.h>

#define PR_TASKMANAGER_STUB		((hTASKMANAGER)PR_THIS_STUB)

extern "C" tERROR PRTaskManager_GetProfileInfo( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, rpcSER_PARAM(info) ) {
	return CALL_TaskManager_GetProfileInfo( PR_TASKMANAGER_STUB, profile_name, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRTaskManager_SetProfileInfo( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, rpcSER_PARAM(info), rpcOBJECT client, tDWORD delay ) {
	return CALL_TaskManager_SetProfileInfo( PR_TASKMANAGER_STUB, profile_name, PR_DESERIALIZE(info), PR_LOCAL(client), delay );
}
extern "C" tERROR PRTaskManager_EnableProfile( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, tBOOL enabled, tBOOL recursive, rpcOBJECT client ) {
	return CALL_TaskManager_EnableProfile( PR_TASKMANAGER_STUB, profile_name, enabled, recursive, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_IsProfileEnabled( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, tBOOL* enabled ) {
	return CALL_TaskManager_IsProfileEnabled( PR_TASKMANAGER_STUB, profile_name, enabled );
}
extern "C" tERROR PRTaskManager_DeleteProfile( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, rpcOBJECT client ) {
	return CALL_TaskManager_DeleteProfile( PR_TASKMANAGER_STUB, profile_name, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_GetSettingsByLevel( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, tDWORD settings_level, tBOOL cumulative, rpcSER_PARAM(settings) ) {
	return CALL_TaskManager_GetSettingsByLevel( PR_TASKMANAGER_STUB, profile_name, settings_level, cumulative, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRTaskManager_GetProfilePersistentStorage( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR* profile_name, rpcOBJECT dad, rpcOBJECT* storage, tBOOL rw ) {
	return CALL_TaskManager_GetProfilePersistentStorage( PR_TASKMANAGER_STUB, profile_name, PR_LOCAL(dad), (cRegistry**)PR_OUT_REMOTE_REF(storage), rw );
}
extern "C" tERROR PRTaskManager_OpenTask( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId* task_id, const tCHAR* profile_name, tDWORD open_flags, rpcOBJECT client ) {
	return CALL_TaskManager_OpenTask( PR_TASKMANAGER_STUB, task_id, profile_name, open_flags, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_ReleaseTask( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id ) {
	return CALL_TaskManager_ReleaseTask( PR_TASKMANAGER_STUB, task_id );
}
extern "C" tERROR PRTaskManager_GetTaskState( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, tDWORD* task_state ) {
	return CALL_TaskManager_GetTaskState( PR_TASKMANAGER_STUB, task_id, (tTaskState*) task_state );
}
extern "C" tERROR PRTaskManager_SetTaskState( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, tTaskRequestState requested_state, rpcOBJECT client, tDWORD delay ) {
	return CALL_TaskManager_SetTaskState( (hTASKMANAGER)PR_THIS_SECURE_STUB, task_id, requested_state, PR_LOCAL(client), delay );
}
extern "C" tERROR PRTaskManager_GetTaskInfo( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, rpcSER_PARAM(info) ) {
	return CALL_TaskManager_GetTaskInfo( PR_TASKMANAGER_STUB, task_id, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRTaskManager_SetTaskInfo( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, rpcSER_PARAM(info) ) {
	return CALL_TaskManager_SetTaskInfo( PR_TASKMANAGER_STUB, task_id, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRTaskManager_DeleteTaskReports( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, tBOOL force_delete_running_reports ) {
	return CALL_TaskManager_DeleteTaskReports( PR_TASKMANAGER_STUB, task_id, force_delete_running_reports );
}
extern "C" tERROR PRTaskManager_AskAction( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, tDWORD action_id, rpcSER_PARAM(params) ) {
	return CALL_TaskManager_AskAction( PR_TASKMANAGER_STUB, task_id, action_id, PR_DESERIALIZE(params) );
}
extern "C" tERROR PRTaskManager_AskTaskAction( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, tDWORD action_id, rpcSER_PARAM(params) ) {
	return CALL_TaskManager_AskTaskAction( PR_TASKMANAGER_STUB, task_id, action_id, PR_DESERIALIZE(params) );
}
extern "C" tERROR PRTaskManager_GetService( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, rpcOBJECT client, const tCHAR __RPC_FAR *service_id, rpcOBJECT* service, tDWORD delay ) {
	return CALL_TaskManager_GetService( PR_TASKMANAGER_STUB, task_id, PR_LOCAL(client), service_id, PR_OUT_REMOTE_REF(service), delay );
}
extern "C" tERROR PRTaskManager_ReleaseService( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, rpcOBJECT service ) {
	return CALL_TaskManager_ReleaseService( PR_TASKMANAGER_STUB, task_id, PR_LOCAL(service) );
}
extern "C" tERROR PRTaskManager_RegisterRemoteTask( handle_t IDL_handle, rpcTASKMANAGER _this, rpcOBJECT tmhost, rpcOBJECT task, rpcSER_PARAM(info), rpcOBJECT* host ) {
	return CALL_TaskManager_RegisterRemoteTask( PR_TASKMANAGER_STUB, PR_LOCAL(tmhost), PR_LOCAL(task), PR_DESERIALIZE(info), PR_OUT_REMOTE(host) );
}
extern "C" tERROR PRTaskManager_SetProfileState( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR __RPC_FAR *profile_name, tTaskRequestState state, rpcOBJECT client, tDWORD delay, tTaskId* task_id ) {
	return CALL_TaskManager_SetProfileState( (hTASKMANAGER)PR_THIS_SECURE_STUB, profile_name, state, PR_LOCAL(client), delay, task_id );
}
extern "C" tERROR PRTaskManager_GetProfileReport( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR __RPC_FAR *profile_name, const tCHAR __RPC_FAR *report_id, rpcOBJECT client, tDWORD access, rpcOBJECT __RPC_FAR *report ) {
	return CALL_TaskManager_GetProfileReport( PR_TASKMANAGER_STUB, profile_name, report_id, PR_LOCAL(client), access, (hREPORT*)PR_OUT_REMOTE(report) );
}
extern "C" tERROR PRTaskManager_CloneProfile( handle_t IDL_handle, rpcTASKMANAGER _this, const tCHAR __RPC_FAR *profile_name, rpcSER_PARAM(profile), tDWORD flags, rpcOBJECT client ) {
	return CALL_TaskManager_CloneProfile( PR_TASKMANAGER_STUB, profile_name, (cProfile*)(cSerializable*)PR_DESERIALIZE(profile), flags, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_GetTaskReport( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, const tCHAR __RPC_FAR *report_id, rpcOBJECT client, tDWORD access, rpcOBJECT __RPC_FAR *report ) {
	return CALL_TaskManager_GetTaskReport( PR_TASKMANAGER_STUB, task_id, report_id, PR_LOCAL(client), access, (hREPORT*)PR_OUT_REMOTE(report) );
}
extern "C" tERROR PRTaskManager_GetTaskPersistentStorage( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, rpcOBJECT dad, rpcOBJECT* storage, tBOOL rw ) {
	return CALL_TaskManager_GetTaskPersistentStorage( PR_TASKMANAGER_STUB, task_id, PR_LOCAL(dad), (cRegistry**)PR_OUT_REMOTE_REF(storage), rw );
}
extern "C" tERROR PRTaskManager_AddRecordToTaskReport( handle_t IDL_handle, rpcTASKMANAGER _this, tTaskId task_id, tDWORD msg_cls, rpcSER_PARAM(info) ) {
	return CALL_TaskManager_AddRecordToTaskReport( PR_TASKMANAGER_STUB, task_id, msg_cls, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRTaskManager_RegisterClient( handle_t IDL_handle, rpcTASKMANAGER _this, rpcOBJECT client ) {
	return CALL_TaskManager_RegisterClient( PR_TASKMANAGER_STUB, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_UnregisterClient( handle_t IDL_handle, rpcTASKMANAGER _this, rpcOBJECT client ) {
	return CALL_TaskManager_UnregisterClient( PR_TASKMANAGER_STUB, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_CleanupItems( handle_t IDL_handle, rpcTASKMANAGER _this, tDWORD item_type, const tDATETIME* time_stamp, rpcOBJECT client ) {
	return CALL_TaskManager_CleanupItems( PR_TASKMANAGER_STUB, item_type, time_stamp, PR_LOCAL(client) );
}
extern "C" tERROR PRTaskManager_AddLicKey( handle_t IDL_handle, rpcTASKMANAGER _this, rpcOBJECT key_name ) {
	return CALL_TaskManager_AddLicKey( PR_TASKMANAGER_STUB, (hSTRING)PR_LOCAL(key_name) );
}
extern "C" tERROR PRTaskManager_Init( handle_t IDL_handle, rpcTASKMANAGER _this, tDWORD flags ) {
	return CALL_TaskManager_Init( PR_TASKMANAGER_STUB, flags );
}
extern "C" tERROR PRTaskManager_Exit( handle_t IDL_handle, rpcTASKMANAGER _this, tDWORD cause ) {
	return CALL_TaskManager_Exit( PR_TASKMANAGER_STUB, cause );
}
extern "C" tERROR PRTaskManager_GetTaskReportInfo( handle_t IDL_handle, rpcTASKMANAGER _this, tDWORD flags, tDWORD index, rpcSER_PARAM(info) ) {
	return CALL_TaskManager_GetTaskReportInfo( PR_TASKMANAGER_STUB, flags, index, PR_DESERIALIZE(info) );
}
PR_IMPLEMENT_STUB(IID_TASKMANAGER, PRrpc_taskmanager_ServerIfHandle)


#define PR_TMCLIENT_STUB		((hTM_CLIENT)PR_THIS_STUB)

extern "C" tERROR PRTmClient_OpenRequest( handle_t IDL_handle, rpcTMCLIENT _this, tDWORD request_id, tDWORD request_type, rpcSER_PARAM(params) ) {
	return CALL_TmClient_OpenRequest( PR_TMCLIENT_STUB, request_id, request_type, PR_DESERIALIZE(params) );
}
extern "C" tERROR PRTmClient_ProcessRequest( handle_t IDL_handle, rpcTMCLIENT _this, tDWORD request_id, tDWORD request_type, rpcSER_PARAM(params) ) {
	return CALL_TmClient_ProcessRequest( PR_TMCLIENT_STUB, request_id, request_type, PR_DESERIALIZE(params) );
}
extern "C" tERROR PRTmClient_CloseRequest( handle_t IDL_handle, rpcTMCLIENT _this, tDWORD request_id ) {
	return CALL_TmClient_CloseRequest( PR_TMCLIENT_STUB, request_id );
}
extern "C" tERROR PRTmClient_Exit( handle_t IDL_handle, rpcTMCLIENT _this, tDWORD cause ) {
	return CALL_TmClient_Exit( PR_TMCLIENT_STUB, cause );
}
PR_IMPLEMENT_STUB(IID_TM_CLIENT, PRrpc_tmclient_ServerIfHandle)


#define PR_TMHOST_STUB		((hTMHOST)PR_THIS_STUB)

extern "C" tERROR PRTmHost_CreateTask( handle_t IDL_handle, rpcTMHOST _this, rpcOBJECT host, rpcSER_PARAM(info), rpcOBJECT* task ) {
	return CALL_TmHost_CreateTask( PR_TMHOST_STUB, PR_LOCAL(host), PR_DESERIALIZE(info), PR_OUT_REMOTE_REF(task) );
}
extern "C" tERROR PRTmHost_DestroyTask( handle_t IDL_handle, rpcTMHOST _this, rpcOBJECT task ) {
	return CALL_TmHost_DestroyTask( PR_TMHOST_STUB, PR_LOCAL(task) );
}
PR_IMPLEMENT_STUB(IID_TMHOST, PRrpc_tmhost_ServerIfHandle)
