#include "stdafx.h"
#include <rpc_mchk.h>

#define PR_MCHK_STUB		((hMCHK)PR_THIS_STUB)

// ---
extern "C" tERROR PRMchk_Process( handle_t IDL_handle, rpcMCHK _this, rpcOBJECT ScanIo ) {
	return CALL_Mchk_Process( PR_MCHK_STUB, PR_LOCAL(ScanIo) );
}
// ---
extern "C" tERROR PRMchk_Initialize( handle_t IDL_handle, rpcMCHK _this ) {
	return CALL_Mchk_Initialize( PR_MCHK_STUB );
}

PR_IMPLEMENT_STUB(IID_MCHK, PRrpc_mchk_ServerIfHandle)
