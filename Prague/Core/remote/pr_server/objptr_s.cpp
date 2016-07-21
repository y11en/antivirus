#include "stdafx.h"
#include <rpc_objptr.h>

#define PR_OBJPTR_STUB		((hObjPtr)PR_THIS_STUB)

// ---
extern "C" tERROR PRObjPtr_IOCreate( handle_t IDL_handle, rpcObjPtr _this, /* [out][in][full] */ rpcIO *result, rpcSTRING name, tDWORD access_mode, tDWORD open_mode ) {
	return CALL_ObjPtr_IOCreate( PR_OBJPTR_STUB, (hIO*)PR_OUT_REMOTE(result), (hSTRING)PR_LOCAL(name), access_mode, open_mode );
}
// ---
extern "C" tERROR PRObjPtr_Copy( handle_t IDL_handle, rpcObjPtr _this, rpcOBJECT dst_name, tBOOL overwrite ) {
	return CALL_ObjPtr_Copy( PR_OBJPTR_STUB, PR_LOCAL(dst_name), overwrite );
}
// ---
extern "C" tERROR PRObjPtr_Rename( handle_t IDL_handle, rpcObjPtr _this, rpcOBJECT new_name, tBOOL overwrite ) {
	return CALL_ObjPtr_Rename( PR_OBJPTR_STUB, PR_LOCAL(new_name), overwrite );
}
// ---
extern "C" tERROR PRObjPtr_Delete( handle_t IDL_handle, rpcObjPtr _this ) {
	return CALL_ObjPtr_Delete( PR_OBJPTR_STUB );
}
// ---
extern "C" tERROR PRObjPtr_Reset( handle_t IDL_handle, rpcObjPtr _this, tBOOL to_root ) {
	return CALL_ObjPtr_Reset( PR_OBJPTR_STUB, to_root );
}
// ---
extern "C" tERROR PRObjPtr_Clone( handle_t IDL_handle, rpcObjPtr _this, /* [out][in][full] */ rpcObjPtr __RPC_FAR *result ) {
	return CALL_ObjPtr_Clone( PR_OBJPTR_STUB, (hObjPtr*)PR_OUT_REMOTE(result) );
}
// ---
extern "C" tERROR PRObjPtr_Next( handle_t IDL_handle, rpcObjPtr _this ) {
	return CALL_ObjPtr_Next( PR_OBJPTR_STUB );
}
// ---
extern "C" tERROR PRObjPtr_StepUp( handle_t IDL_handle, rpcObjPtr _this ) {
	return CALL_ObjPtr_StepUp( PR_OBJPTR_STUB );
}
// ---
extern "C" tERROR PRObjPtr_StepDown( handle_t IDL_handle, rpcObjPtr _this ) {
	return CALL_ObjPtr_StepDown( PR_OBJPTR_STUB );
}
// ---
extern "C" tERROR PRObjPtr_ChangeTo( handle_t IDL_handle, rpcObjPtr _this, rpcOBJECT name ) {
	return CALL_ObjPtr_ChangeTo( PR_OBJPTR_STUB, PR_LOCAL(name) );
}

PR_IMPLEMENT_STUB(IID_OBJPTR, PRrpc_objptr_ServerIfHandle)
