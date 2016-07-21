#include "stdafx.h"
#include <rpc_insider.h>

#define PR_INSIDER_STUB		((hINSIDER)PR_THIS_STUB)

// ---
extern "C" tERROR PRInsider_StartTask( handle_t IDL_handle, rpcINSIDER _this, const tBYTE* lpTaskData, tDWORD dwTaskDataSize ) {
	return CALL_Insider_StartTask( PR_INSIDER_STUB, lpTaskData, dwTaskDataSize );
}
// ---
extern "C" tERROR PRInsider_StopTask( handle_t IDL_handle, rpcINSIDER _this ) {
	return CALL_Insider_StopTask( PR_INSIDER_STUB );
}
// ---
extern "C" tERROR PRInsider_PauseTask( handle_t IDL_handle, rpcINSIDER _this ) {
	return CALL_Insider_PauseTask( PR_INSIDER_STUB );
}
// ---
extern "C" tERROR PRInsider_ContinueTask( handle_t IDL_handle, rpcINSIDER _this ) {
	return CALL_Insider_ContinueTask( PR_INSIDER_STUB );
}
// ---
extern "C" tERROR PRInsider_RestartTask( handle_t IDL_handle, rpcINSIDER _this, const tBYTE* lpTaskData, tDWORD dwTaskDataSize ) {
	return CALL_Insider_RestartTask( PR_INSIDER_STUB, lpTaskData, dwTaskDataSize );
}

PR_IMPLEMENT_STUB(IID_INSIDER, PRrpc_insider_ServerIfHandle)
