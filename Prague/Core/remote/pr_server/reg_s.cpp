#include "stdafx.h"
#include <rpc_reg.h>

#define PR_REG_STUB		((hREGISTRY)PR_THIS_STUB)

// ---
extern "C" tERROR PRRegistry_OpenKey( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tRegKey __RPC_FAR *result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist ) {
	return CALL_Registry_OpenKey( PR_REG_STUB, result, key, szSubKeyName, bCreateIfNotExist );
}
// ---
extern "C" tERROR PRRegistry_OpenKeyByIndex( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tRegKey __RPC_FAR *result, tRegKey key, tDWORD index, tBOOL bClosePrev ) {
	return CALL_Registry_OpenKeyByIndex( PR_REG_STUB, result, key, index, bClosePrev );
}
// ---
extern "C" tERROR PRRegistry_OpenNextKey( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tRegKey __RPC_FAR *result, tRegKey key, tBOOL bClosePrev ) {
	return CALL_Registry_OpenNextKey( PR_REG_STUB, result, key, bClosePrev );
}
// ---
extern "C" tERROR PRRegistry_CloseKey( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key ) {
	return CALL_Registry_CloseKey( PR_REG_STUB, key );
}
// ---
extern "C" tERROR PRRegistry_GetKeyName( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key, /* [size_is][in] */ tCHAR __RPC_FAR *name_buff, tDWORD size, tBOOL bFullName ) {
	return CALL_Registry_GetKeyName( PR_REG_STUB, result, key, name_buff, size, bFullName );
}
// ---
extern "C" tERROR PRRegistry_GetKeyNameByIndex( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key, tDWORD index, /* [size_is][out] */ tCHAR __RPC_FAR *name_buff, tDWORD size, tBOOL bFullName ) {
	return CALL_Registry_GetKeyNameByIndex( PR_REG_STUB, result, key, index, name_buff, size, bFullName );
}
// ---
extern "C" tERROR PRRegistry_GetValue( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key, const tCHAR* szValueName, /* [out] */ tTYPE_ID __RPC_FAR *type, tPTR pValue, tDWORD size ) {
	return CALL_Registry_GetValue( PR_REG_STUB, result, key, szValueName, type, pValue, size );
}
// ---
extern "C" tERROR PRRegistry_GetValueByIndex( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key, tDWORD index, /* [out] */ tTYPE_ID __RPC_FAR *type, tPTR pValue, tDWORD size ) {
	return CALL_Registry_GetValueByIndex( PR_REG_STUB, result, key, index, type, pValue, size );
}
// ---
extern "C" tERROR PRRegistry_GetValueNameByIndex( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key, tDWORD index, /* [size_is][out] */ tCHAR __RPC_FAR *name_buff, tDWORD size ) {
	return CALL_Registry_GetValueNameByIndex( PR_REG_STUB, result, key, index, name_buff, size );
}
// ---
extern "C" tERROR PRRegistry_SetValue( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) {
	return CALL_Registry_SetValue( PR_REG_STUB, key, szValueName, type, pValue, size, bCreateIfNotExist );
}
// ---
extern "C" tERROR PRRegistry_SetValueByIndex( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) {
	return CALL_Registry_SetValueByIndex( PR_REG_STUB, key, index, type, pValue, size, bCreateIfNotExist );
}
// ---
extern "C" tERROR PRRegistry_GetKeyCount( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key ) {
	return CALL_Registry_GetKeyCount( PR_REG_STUB, result, key );
}
// ---
extern "C" tERROR PRRegistry_GetValueCount( handle_t IDL_handle, rpcREGISTRY _this, /* [out] */ tDWORD __RPC_FAR *result, tRegKey key ) {
	return CALL_Registry_GetValueCount( PR_REG_STUB, result, key );
}
// ---
extern "C" tERROR PRRegistry_DeleteKey( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key, const tCHAR* szSubKeyName ) {
	return CALL_Registry_DeleteKey( PR_REG_STUB, key, szSubKeyName );
}
// ---
extern "C" tERROR PRRegistry_DeleteKeyByInd( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key, tDWORD index ) {
	return CALL_Registry_DeleteKeyByInd( PR_REG_STUB, key, index );
}
// ---
extern "C" tERROR PRRegistry_DeleteValue( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key, const tCHAR* szValueName ) {
	return CALL_Registry_DeleteValue( PR_REG_STUB, key, szValueName );
}
// ---
extern "C" tERROR PRRegistry_DeleteValueByInd( handle_t IDL_handle, rpcREGISTRY _this, tRegKey key, tDWORD index ) {
	return CALL_Registry_DeleteValueByInd( PR_REG_STUB, key, index );
}
// ---
extern "C" tERROR PRRegistry_Clean( handle_t IDL_handle, rpcREGISTRY _this ) {
	return CALL_Registry_Clean( PR_REG_STUB );
}

PR_IMPLEMENT_STUB(IID_REGISTRY, PRrpc_registry_ServerIfHandle)
