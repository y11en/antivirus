#include "stdafx.h"
#include <rpc_ifenum.h>

#define PR_IFACE_ENUM_STUB		((hIFACE_ENUM)PR_THIS_STUB)

// ---
extern "C" tERROR PRIFaceEnum_Init( handle_t IDL_handle, rpcIFACE_ENUM _this, tIID iid, tPID pid, tVID vid, rpcOBJECT object  ) {
	return CALL_IFaceEnum_Init( PR_IFACE_ENUM_STUB, iid, pid, vid, PR_LOCAL(object)  );
}
// ---
extern "C" tERROR PRIFaceEnum_Clone( handle_t IDL_handle, rpcIFACE_ENUM _this, rpcIFACE_ENUM __RPC_FAR *result, rpcOBJECT dad ){
	return CALL_IFaceEnum_Clone( PR_IFACE_ENUM_STUB, (hIFACE_ENUM*)PR_OUT_REMOTE(result), PR_LOCAL(dad) );
}
// ---
extern "C" tERROR PRIFaceEnum_Reset( handle_t IDL_handle, rpcIFACE_ENUM _this ) {
	return CALL_IFaceEnum_Reset( PR_IFACE_ENUM_STUB );
}
// ---
extern "C" tERROR PRIFaceEnum_Next( handle_t IDL_handle, rpcIFACE_ENUM _this ) {
	return CALL_IFaceEnum_Next( PR_IFACE_ENUM_STUB );
}
// ---
extern "C" tERROR PRIFaceEnum_GetIFaceProp( handle_t IDL_handle, rpcIFACE_ENUM _this, /* [out][in][full] */ tDWORD __RPC_FAR *result, tPROPID prop_id, tCHAR *buffer, tDWORD size ) {
	return CALL_IFaceEnum_GetIFaceProp( PR_IFACE_ENUM_STUB, result, prop_id, buffer, size );
}
// ---
extern "C" tERROR PRIFaceEnum_ObjectCreate( handle_t IDL_handle, rpcIFACE_ENUM _this, /* [out][in][full] */ rpcOBJECT __RPC_FAR *result, rpcOBJECT parent ) {
	return CALL_IFaceEnum_ObjectCreate( PR_IFACE_ENUM_STUB, PR_OUT_REMOTE(result), PR_LOCAL(parent) );
}
// ---
extern "C" tERROR PRIFaceEnum_Recognize( handle_t IDL_handle, rpcIFACE_ENUM _this, rpcOBJECT object, tDWORD type ) {
	return CALL_IFaceEnum_Recognize( PR_IFACE_ENUM_STUB, PR_LOCAL(object), type );
}
// ---
extern "C" tERROR PRIFaceEnum_Load( handle_t IDL_handle, rpcIFACE_ENUM _this ) {
	return CALL_IFaceEnum_Load( PR_IFACE_ENUM_STUB );
}
// ---
extern "C" tERROR PRIFaceEnum_Free( handle_t IDL_handle, rpcIFACE_ENUM _this ) {
	return CALL_IFaceEnum_Free( PR_IFACE_ENUM_STUB );
}

PR_IMPLEMENT_STUB(IID_IFACE_ENUM, PRrpc_ifenum_ServerIfHandle)
