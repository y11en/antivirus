#include "stdafx.h"
#include <rpc_engine.h>

#define PR_ENGINE_STUB		((hENGINE)PR_THIS_STUB)

// ---
extern "C" tERROR PREngine_Process( handle_t IDL_handle, rpcENGINE _this, rpcOBJECT obj ) {
	return CALL_Engine_Process( PR_ENGINE_STUB, PR_LOCAL(obj) );
}
// ---
extern "C" tERROR PREngine_GetAssociatedInfo( handle_t IDL_handle, rpcENGINE _this, rpcOBJECT obj, tDWORD info_id, tDWORD param, /* [size_is][out] */ tCHAR __RPC_FAR *buffer, tDWORD size, /* [out] */ tDWORD __RPC_FAR *out_size ) {
	return CALL_Engine_GetAssociatedInfo( PR_ENGINE_STUB, PR_LOCAL(obj), info_id, param, buffer, size, out_size );
}
// ---
extern "C" tERROR PREngine_GetExtendedInfo( handle_t IDL_handle, rpcENGINE _this, tDWORD info_id, tDWORD param, /* [size_is][out] */ tCHAR __RPC_FAR *buffer, tDWORD size, /* [out] */ tDWORD __RPC_FAR *out_size ) {
	return CALL_Engine_GetExtendedInfo( PR_ENGINE_STUB, info_id, param, buffer, size, out_size );
}
// ---
extern "C" tERROR PREngine_LoadConfiguration( handle_t IDL_handle, rpcENGINE _this, rpcREGISTRY reg, tRegKey key ) {
	return CALL_Engine_LoadConfiguration( PR_ENGINE_STUB, (hREGISTRY)PR_LOCAL(reg), key );
}

PR_IMPLEMENT_STUB(IID_ENGINE, PRrpc_engine_ServerIfHandle)
