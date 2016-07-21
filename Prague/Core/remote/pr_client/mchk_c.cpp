#include "StdAfx.h"
#include <rpc_mchk.h>

// ---
tERROR pr_call Local_Mchk_Process( hMCHK _this, hOBJECT ScanIo ) {
	PR_PROXY_CALL( Mchk_Process( PR_THIS_PROXY, PR_REMOTE(ScanIo) ) );
}
// ---
tERROR pr_call Local_Mchk_Initialize( hMCHK _this ) { 
	PR_PROXY_CALL( Mchk_Initialize( PR_THIS_PROXY ) );
}

// AVP Prague stamp begin( Interface declaration,  )
iMchkVtbl mchk_iface = {
	Local_Mchk_Process,
	Local_Mchk_Initialize,
}; // "Engine" external virtual table prototype
// AVP Prague stamp end

PR_IMPLEMENT_PROXY(IID_MCHK, mchk_iface)


