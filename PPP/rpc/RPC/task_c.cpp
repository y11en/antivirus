#include "stdafx.h"
#include "rpc_task.h"
#include <iface/i_task.h>
#include <iface/i_mailtask.h>
#include <iface/i_processmonitor.h>


tERROR pr_call Local_Task_SetSettings( hTASK _this, const cSerializable * settings ) {		// -- ;
	PR_PROXY_CALL_EX( Task_SetSettings( PR_THIS_PROXY, PR_SERIALIZE(settings) ), PR_SERCLEANUP(settings) );
}
tERROR pr_call Local_Task_GetSettings( hTASK _this, cSerializable * settings ) {		// -- ;
	PR_PROXY_CALL_EX( Task_GetSettings( PR_THIS_PROXY, PR_SERIALIZE(settings) ), PR_SERCLEANUP(settings) );
}
tERROR pr_call Local_Task_AskAction( hTASK _this, tActionId action, cSerializable * askinfo ) {		// -- ;
	PR_PROXY_CALL_EX( Task_AskAction( PR_THIS_PROXY, action, PR_SERIALIZE(askinfo) ), PR_SERCLEANUP(askinfo) );
}
tERROR pr_call Local_Task_SetState( hTASK _this, tTaskRequestState state ) {		// -- ;
	PR_PROXY_CALL( Task_SetState( PR_THIS_PROXY, state ) );
}
tERROR pr_call Local_Task_GetStatistics( hTASK _this, cSerializable * statistics ) {		// -- ;
	PR_PROXY_CALL_EX( Task_GetStatistics( PR_THIS_PROXY, PR_SERIALIZE(statistics) ), PR_SERCLEANUP(statistics) );
}

iTaskVtbl task_iface = {
	Local_Task_SetSettings,
	Local_Task_GetSettings,
	Local_Task_AskAction,
	Local_Task_SetState,
	Local_Task_GetStatistics,
}; // "GUI" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_TASK, task_iface)

//////////////////////////////////////////////////////////////////////////////////////////
// mctask

tERROR pr_call Local_MailTask_Process( hTASK _this, hOBJECT p_hTopIO, tDWORD* p_hTotalPrimaryAction, tSTRING p_szMailTaskName, tDWORD p_dwTimeOut ) {		// -- ;
	PR_PROXY_CALL( MailTask_Process( PR_THIS_PROXY, PR_REMOTE(p_hTopIO), p_hTotalPrimaryAction, p_szMailTaskName, p_dwTimeOut ) );
}

iMailTaskVtbl mctask_iface = {
	(fnpMailTask_SetSettings)Local_Task_SetSettings,
	(fnpMailTask_GetSettings)Local_Task_GetSettings,
	(fnpMailTask_AskAction)Local_Task_AskAction,
	(fnpMailTask_SetState)Local_Task_SetState,
	(fnpMailTask_GetStatistics)Local_Task_GetStatistics,
	(fnpMailTask_Process)Local_MailTask_Process,
}; // "GUI" external virtual table prototype

PR_IMPLEMENT_PROXY_CM(IID_MAILTASK, mctask_iface, IID_TASK)

//////////////////////////////////////////////////////////////////////////////////////////
// ProcessMonitor

#include <iface/i_processmonitor.h>

tERROR pr_call Local_ProcessMonitor_CalcObjectHash( hPROCESSMONITOR _this, hOBJECT object, tQWORD* hash ) {
	PR_PROXY_CALL( ProcessMonitor_CalcObjectHash( PR_THIS_PROXY, PR_REMOTE(object), hash ) );
}

tERROR pr_call Local_ProcessMonitor_GetProcessInfo( hPROCESSMONITOR _this, cSerializable* info ) {
	PR_PROXY_CALL_EX( ProcessMonitor_GetProcessInfo( PR_THIS_PROXY, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}

tERROR pr_call Local_ProcessMonitor_IsProcessTrusted( hPROCESSMONITOR _this, cSerializable* data, tBOOL* result ) {
	PR_PROXY_CALL_EX( ProcessMonitor_IsProcessTrusted( PR_THIS_PROXY, PR_SERIALIZE(data), result ), PR_SERCLEANUP(data) );
}

iProcessMonitorVtbl processmonitor_iface = {
	(fnpProcessMonitor_SetSettings)Local_Task_SetSettings,
	(fnpProcessMonitor_GetSettings)Local_Task_GetSettings,
	(fnpProcessMonitor_AskAction)Local_Task_AskAction,
	(fnpProcessMonitor_SetState)Local_Task_SetState,
	(fnpProcessMonitor_GetStatistics)Local_Task_GetStatistics,
	Local_ProcessMonitor_CalcObjectHash,
	Local_ProcessMonitor_GetProcessInfo,
	Local_ProcessMonitor_IsProcessTrusted,
};

PR_IMPLEMENT_PROXY_CM(IID_PROCESSMONITOR, processmonitor_iface, IID_TASK)
