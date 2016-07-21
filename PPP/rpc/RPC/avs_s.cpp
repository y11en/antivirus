#include "stdafx.h"
#include "rpc_avs.h"
#include <iface/i_avs.h>

#define PR_AVS_STUB			((hAVS)PR_THIS_STUB)
#define PR_AVSTREATS_STUB	((hAVSTREATS)PR_THIS_STUB)

extern "C" tERROR PRAVS_SetSettings( handle_t IDL_handle, rpcAVS _this, rpcSER_PARAM(settings) ) {
	return CALL_AVS_SetSettings( PR_AVS_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRAVS_GetSettings( handle_t IDL_handle, rpcAVS _this, rpcSER_PARAM(settings) ) {
	return CALL_AVS_GetSettings( PR_AVS_STUB, PR_DESERIALIZE(settings) );
}
extern "C" tERROR PRAVS_AskAction( handle_t IDL_handle, rpcAVS _this, tDWORD actionId, rpcSER_PARAM(askinfo) ) {
	return CALL_AVS_AskAction( PR_AVS_STUB, (tActionId)actionId, PR_DESERIALIZE(askinfo) );
}
extern "C" tERROR PRAVS_SetState( handle_t IDL_handle, rpcAVS _this, tDWORD state ) {
	return CALL_AVS_SetState( PR_AVS_STUB, (tTaskState)state );
}
extern "C" tERROR PRAVS_GetStatistics( handle_t IDL_handle, rpcAVS _this, rpcSER_PARAM(statistics) ) {
	return CALL_AVS_GetStatistics( PR_AVS_STUB, PR_DESERIALIZE(statistics) );
}
extern "C" tERROR PRAVS_GetTreats( handle_t IDL_handle, rpcAVS _this, tDWORD taskid, rpcAVSTREATS* treats ) {		// -- ;
	return CALL_AVS_GetTreats( PR_AVS_STUB, taskid, (hAVSTREATS*)PR_OUT_REMOTE_REF(treats) );
}
extern "C" tERROR PRAVS_ReleaseTreats( handle_t IDL_handle, rpcAVS _this, rpcAVSTREATS treats ) {		// -- ;
	return CALL_AVS_ReleaseTreats( PR_AVS_STUB, (hAVSTREATS)PR_LOCAL(treats) );
}
extern "C" tERROR PRAVS_ProcessCancel( handle_t IDL_handle, rpcAVS _this, tDWORD process_id ) {		// -- ;
	return CALL_AVS_ProcessCancel( PR_AVS_STUB, process_id );
}
PR_IMPLEMENT_STUB(IID_AVS, PRrpc_avs_ServerIfHandle)


extern "C" tERROR PRAVSTreats_GetTreatInfo( handle_t IDL_handle, rpcAVSTREATS _this, tDWORD filter, tDWORD pos, rpcSER_PARAM(info) ) {		// -- ;
	return CALL_AVSTreats_GetTreatInfo( PR_AVSTREATS_STUB, filter, pos, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRAVSTreats_Process( handle_t IDL_handle, rpcAVSTREATS _this, tDWORD filter, rpcSER_PARAM(process_info) ) {		// -- ;
	return CALL_AVSTreats_Process( (hAVSTREATS)PR_THIS_SECURE_STUB, filter, PR_DESERIALIZE(process_info) );
}
extern "C" tERROR PRAVSTreats_GetInfo( handle_t IDL_handle, rpcAVSTREATS _this, rpcSER_PARAM(info) ) {		// -- ;
	return CALL_AVSTreats_GetInfo( PR_AVSTREATS_STUB, PR_DESERIALIZE(info) );
}
extern "C" tERROR PRAVSTreats_ProcessCancel( handle_t IDL_handle, rpcAVSTREATS _this ) {		// -- ;
	return CALL_AVSTreats_ProcessCancel( PR_AVSTREATS_STUB );
}
PR_IMPLEMENT_STUB(IID_AVSTREATS, PRrpc_avstreats_ServerIfHandle)
