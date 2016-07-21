#include "StdAfx.h"
#include <rpc_insider.h>

// ---
tERROR pr_call Local_Insider_StartTask( hINSIDER _this, const tBYTE* lpTaskData, tDWORD dwTaskDataSize ) {
	PR_PROXY_CALL( Insider_StartTask( PR_THIS_PROXY, lpTaskData, dwTaskDataSize ) );
}
// ---
tERROR pr_call Local_Insider_StopTask( hINSIDER _this ) { 
	PR_PROXY_CALL( Insider_StopTask( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_Insider_PauseTask( hINSIDER _this ) { 
	PR_PROXY_CALL( Insider_PauseTask( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_Insider_ContinueTask( hINSIDER _this ) { 
	PR_PROXY_CALL( Insider_ContinueTask( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_Insider_RestartTask( hINSIDER _this, const tBYTE* lpTaskData, tDWORD dwTaskDataSize ) { 
	PR_PROXY_CALL( Insider_RestartTask( PR_THIS_PROXY, lpTaskData, dwTaskDataSize ) );
}

// AVP Prague stamp begin( Interface declaration,  )
iInsiderVtbl insider_iface = {
	Local_Insider_StartTask,
	Local_Insider_StopTask,
	Local_Insider_PauseTask,
	Local_Insider_ContinueTask,
	Local_Insider_RestartTask,
}; // "Engine" external virtual table prototype
// AVP Prague stamp end

PR_IMPLEMENT_PROXY(IID_INSIDER, insider_iface)


