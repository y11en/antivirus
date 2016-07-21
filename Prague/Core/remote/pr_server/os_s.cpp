#include "stdafx.h"
#include <rpc_os.h>

#define PR_OS_STUB		((hOS)PR_THIS_STUB)

// ---
extern "C" tERROR PROS_PtrCreate( handle_t IDL_handle, rpcOS _this, /* [out][in][full] */ rpcObjPtr __RPC_FAR *result, rpcOBJECT name ) {
	return CALL_OS_PtrCreate( PR_OS_STUB, (hObjPtr*)PR_OUT_REMOTE(result), PR_LOCAL(name) );
}
// ---
extern "C" tERROR PROS_IOCreate( handle_t IDL_handle, rpcOS _this, /* [out][in][full] */ rpcIO __RPC_FAR *result, rpcOBJECT name, tDWORD access_mode, tDWORD open_mode ) {
	return CALL_OS_IOCreate( PR_OS_STUB, (hIO*)PR_OUT_REMOTE(result), PR_LOCAL(name), access_mode, open_mode );
}
// ---
extern "C" tERROR PROS_Copy( handle_t IDL_handle, rpcOS _this, rpcOBJECT src_name, rpcOBJECT dst_name, tBOOL overwrite ) {
	return CALL_OS_Copy( PR_OS_STUB, PR_LOCAL(src_name), PR_LOCAL(dst_name), overwrite );
}
// ---
extern "C" tERROR PROS_Rename( handle_t IDL_handle, rpcOS _this, rpcOBJECT old_name, rpcOBJECT new_name, tBOOL overwrite ) {
	return CALL_OS_Rename( PR_OS_STUB, PR_LOCAL(old_name), PR_LOCAL(new_name), overwrite );
}
// ---
extern "C" tERROR PROS_Delete( handle_t IDL_handle, rpcOS _this, rpcOBJECT name ) {
	return CALL_OS_Delete( PR_OS_STUB, PR_LOCAL(name) );
}

PR_IMPLEMENT_STUB(IID_OS, PRrpc_os_ServerIfHandle)
