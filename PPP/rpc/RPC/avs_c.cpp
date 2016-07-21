#include "stdafx.h"
#include "rpc_avs.h"
#include <iface/i_avs.h>


tERROR pr_call Local_AVS_SetSettings( hAVS _this, const cSerializable * settings ) {		// -- ;
	PR_PROXY_CALL_EX( AVS_SetSettings( PR_THIS_PROXY, PR_SERIALIZE(settings) ), PR_SERCLEANUP(settings) );
}
tERROR pr_call Local_AVS_GetSettings( hAVS _this, cSerializable * settings ) {		// -- ;
	PR_PROXY_CALL_EX( AVS_GetSettings( PR_THIS_PROXY, PR_SERIALIZE(settings) ), PR_SERCLEANUP(settings) );
}
tERROR pr_call Local_AVS_AskAction( hAVS _this, tActionId action, cSerializable * askinfo ) {		// -- ;
	PR_PROXY_CALL_EX( AVS_AskAction( PR_THIS_PROXY, action, PR_SERIALIZE(askinfo) ), PR_SERCLEANUP(askinfo) );
}
tERROR pr_call Local_AVS_SetState( hAVS _this, tTaskState state ) {		// -- ;
	PR_PROXY_CALL( AVS_SetState( PR_THIS_PROXY, state ) );
}
tERROR pr_call Local_AVS_GetStatistics( hAVS _this, cSerializable * statistics ) {		// -- ;
	PR_PROXY_CALL_EX( AVS_GetStatistics( PR_THIS_PROXY, PR_SERIALIZE(statistics) ), PR_SERCLEANUP(statistics) );
}
tERROR pr_call Local_AVS_GetTreats( hAVS _this, tDWORD taskid, hAVSTREATS* treats ) {		// -- ;
	PR_PROXY_CALL( AVS_GetTreats( PR_THIS_PROXY, taskid, PR_OUT_LOCAL(treats) ) );
}
tERROR pr_call Local_AVS_ReleaseTreats( hAVS _this, hAVSTREATS treats ) {		// -- ;
	PR_PROXY_CALL( AVS_ReleaseTreats( PR_THIS_PROXY, PR_REMOTE_DEREF(treats) ) );
}
tERROR pr_call Local_AVS_ProcessCancel( hAVS _this, tDWORD process_id ) {		// -- ;
	PR_PROXY_CALL( AVS_ProcessCancel( PR_THIS_PROXY, process_id ) );
}

iAVSVtbl avs_iface = {
	Local_AVS_SetSettings,
	Local_AVS_GetSettings,
	Local_AVS_AskAction,
	Local_AVS_SetState,
	Local_AVS_GetStatistics,
	NULL,
	NULL,
	NULL,
	NULL,
	Local_AVS_GetTreats,
	Local_AVS_ReleaseTreats,
	NULL,
	Local_AVS_ProcessCancel
}; // 

PR_IMPLEMENT_PROXY_CM(IID_AVS, avs_iface, IID_TASK)

tERROR pr_call Local_AVSTreats_GetTreatInfo( hAVSTREATS _this, tDWORD filter, tDWORD pos, cSerializable* info ) {		// -- ;
	PR_PROXY_CALL_EX( AVSTreats_GetTreatInfo( PR_THIS_PROXY, filter, pos, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_AVSTreats_Process( hAVSTREATS _this, tDWORD filter, const cSerializable* process_info ) {		// -- ;
	PR_PROXY_CALL_EX( AVSTreats_Process( PR_THIS_PROXY, filter, PR_SERIALIZE(process_info) ), PR_SERCLEANUP(process_info) );
}
tERROR pr_call Local_AVSTreats_GetInfo( hAVSTREATS _this, cSerializable* info ) {		// -- ;
	PR_PROXY_CALL_EX( AVSTreats_GetInfo( PR_THIS_PROXY, PR_SERIALIZE(info) ), PR_SERCLEANUP(info) );
}
tERROR pr_call Local_AVSTreats_ProcessCancel( hAVSTREATS _this ) {		// -- ;
	PR_PROXY_CALL( AVSTreats_ProcessCancel( PR_THIS_PROXY ) );
}

iAVSTreatsVtbl avstreats_iface = {
	Local_AVSTreats_GetTreatInfo,
	Local_AVSTreats_Process,
	Local_AVSTreats_GetInfo,
	Local_AVSTreats_ProcessCancel
}; // 

PR_IMPLEMENT_PROXY(IID_AVSTREATS, avstreats_iface)
