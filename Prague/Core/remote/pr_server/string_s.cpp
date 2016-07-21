#include "stdafx.h"
#include <rpc_string.h>

#define PR_STRING_STUB		((hSTRING)PR_THIS_STUB)

// ---
extern "C" tERROR PRString_ExportToProp( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, tSTR_RANGE range, rpcOBJECT to, tPROPID to_propid ) {
	return CALL_String_ExportToProp( PR_STRING_STUB, result, range, PR_LOCAL(to), to_propid );
}
// ---
extern "C" tERROR PRString_ExportToStr( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, tSTR_RANGE range, rpcSTRING to ) {
	return CALL_String_ExportToStr( PR_STRING_STUB, result, range, (hSTRING)PR_LOCAL(to) );
}
// ---
extern "C" tERROR PRString_ExportToBuff( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, tSTR_RANGE range, tCHAR *to, tDWORD length, tCODEPAGE cp, tDWORD flags ) {
	return CALL_String_ExportToBuff( PR_STRING_STUB, result, range, to, length, cp, flags );
}
// ---
extern "C" tERROR PRString_ImportFromProp( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, rpcOBJECT from, tPROPID propid ) {
	return CALL_String_ImportFromProp( PR_STRING_STUB, result, PR_LOCAL(from), propid );
}
// ---
extern "C" tERROR PRString_ImportFromStr( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, rpcSTRING from, tSTR_RANGE range ) {
	return CALL_String_ImportFromStr( PR_STRING_STUB, result, (hSTRING)PR_LOCAL(from), range );
}
// ---
extern "C" tERROR PRString_ImportFromBuff( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, tCHAR *from, tDWORD length, tCODEPAGE cp, tDWORD flags ) {
	return CALL_String_ImportFromBuff( PR_STRING_STUB, result, from, length, cp, flags );
}
// ---
extern "C" tERROR PRString_Length( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result ) {
	return CALL_String_Length( PR_STRING_STUB, result );
}
// ---
extern "C" tERROR PRString_LengthEx( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tDWORD __RPC_FAR *result, tSTR_RANGE range, tCODEPAGE cp, tDWORD flags ) {
	return CALL_String_LengthEx( PR_STRING_STUB, result, range, cp, flags );
}
// ---
extern "C" tERROR PRString_GetCP( handle_t IDL_handle, rpcSTRING _this, /* [out] */ tCODEPAGE __RPC_FAR *result ) {
	return CALL_String_GetCP( PR_STRING_STUB, result );
}
// ---
extern "C" tERROR PRString_SetCP( handle_t IDL_handle, rpcSTRING _this, tCODEPAGE cp ) {
	return CALL_String_SetCP( PR_STRING_STUB, cp );
}

PR_IMPLEMENT_STUB(IID_STRING, PRrpc_string_ServerIfHandle)
