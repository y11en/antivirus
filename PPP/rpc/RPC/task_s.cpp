#include "stdafx.h"
#include "rpc_task.h"
#include <iface/i_task.h>
#include <iface/i_mailtask.h>
#include <iface/i_processmonitor.h>

#define PR_TASK_STUB		((hTASK)PR_THIS_STUB)

extern "C" tERROR PRTask_SetSettings( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(settings) ) {
	return CALL_Task_SetSettings( PR_TASK_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRTask_GetSettings( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(settings) ) {
	return CALL_Task_GetSettings( PR_TASK_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRTask_AskAction( handle_t IDL_handle, rpcTASK _this, tDWORD actionId, rpcSER_PARAM(askinfo) ) {
	return CALL_Task_AskAction( PR_TASK_STUB, (tActionId)actionId, PR_DESERIALIZE(askinfo) );
}
extern "C" tERROR PRTask_SetState( handle_t IDL_handle, rpcTASK _this, tDWORD state ) {
	return CALL_Task_SetState( PR_TASK_STUB, (tTaskRequestState)state );
}
extern "C" tERROR PRTask_GetStatistics( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(statistics) ) {
	return CALL_Task_GetStatistics( PR_TASK_STUB, PR_DESERIALIZE(statistics) );
}
PR_IMPLEMENT_STUB(IID_TASK, PRrpc_task_ServerIfHandle)


#define PR_MAILTASK_STUB		((hMAILTASK)PR_THIS_STUB)

extern "C" tERROR PRMailTask_SetSettings( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(settings) ) {
	return CALL_MailTask_SetSettings( PR_MAILTASK_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRMailTask_GetSettings( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(settings) ) {
	return CALL_MailTask_GetSettings( PR_MAILTASK_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRMailTask_AskAction( handle_t IDL_handle, rpcTASK _this, tDWORD actionId, rpcSER_PARAM(askinfo) ) {
	return CALL_MailTask_AskAction( PR_MAILTASK_STUB, (tActionId)actionId, PR_DESERIALIZE(askinfo) );
}
extern "C" tERROR PRMailTask_SetState( handle_t IDL_handle, rpcTASK _this, tDWORD state ) {
	return CALL_MailTask_SetState( PR_MAILTASK_STUB, (tTaskState)state );
}
extern "C" tERROR PRMailTask_GetStatistics( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(statistics) ) {
	return CALL_MailTask_GetStatistics( PR_MAILTASK_STUB, PR_DESERIALIZE(statistics) );
}
extern "C" tERROR PRMailTask_Process( handle_t IDL_handle, rpcTASK _this, rpcOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ) {
	return CALL_MailTask_Process( PR_MAILTASK_STUB, PR_LOCAL(p_hTopIO), (tTotalPrimaryAction*)p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut);
}
PR_IMPLEMENT_STUB(IID_MAILTASK, PRrpc_mctask_ServerIfHandle)


#define PR_PROCESSMONITOR_STUB		((hPROCESSMONITOR)PR_THIS_STUB)

extern "C" tERROR PRProcessMonitor_SetSettings( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(settings) ) {
	return CALL_ProcessMonitor_SetSettings( PR_PROCESSMONITOR_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRProcessMonitor_GetSettings( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(settings) ) {
	return CALL_ProcessMonitor_GetSettings( PR_PROCESSMONITOR_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRProcessMonitor_AskAction( handle_t IDL_handle, rpcTASK _this, tDWORD actionId, rpcSER_PARAM(askinfo) ) {
	return CALL_ProcessMonitor_AskAction( PR_PROCESSMONITOR_STUB, (tActionId)actionId, PR_DESERIALIZE(askinfo) );
}
extern "C" tERROR PRProcessMonitor_SetState( handle_t IDL_handle, rpcTASK _this, tDWORD state ) {
	return CALL_ProcessMonitor_SetState( PR_PROCESSMONITOR_STUB, (tTaskRequestState)state );
}
extern "C" tERROR PRProcessMonitor_GetStatistics( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(statistics) ) {
	return CALL_ProcessMonitor_GetStatistics( PR_PROCESSMONITOR_STUB, PR_DESERIALIZE(statistics) );
}
extern "C" tERROR PRProcessMonitor_CalcObjectHash( handle_t IDL_handle, rpcTASK _this, rpcOBJECT object, tQWORD* hash ) {
	return CALL_ProcessMonitor_CalcObjectHash( PR_PROCESSMONITOR_STUB, PR_LOCAL(object), hash );
}
extern "C" tERROR PRProcessMonitor_GetProcessInfo( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(info) ) {
	return CALL_ProcessMonitor_GetProcessInfo( PR_PROCESSMONITOR_STUB, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRProcessMonitor_IsProcessTrusted( handle_t IDL_handle, rpcTASK _this, rpcSER_PARAM(data), tBOOL* result ) {
	return CALL_ProcessMonitor_IsProcessTrusted( PR_PROCESSMONITOR_STUB, PR_DESERIALIZE(data), result );
}
PR_IMPLEMENT_STUB(IID_PROCESSMONITOR, PRrpc_processmonitor_ServerIfHandle)
