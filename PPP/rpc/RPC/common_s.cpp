#include "stdafx.h"
#include "rpc_common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// License

#include <iface/i_licensing.h>

#define PR_LIC_STUB		((hLICENSING)PR_THIS_STUB)

extern "C" tERROR PRLIC_GetActiveKeyInfo( handle_t IDL_handle, rpcLICENSING _this, rpcSER_PARAM(pResult) ) {
	return CALL_Licensing_GetActiveKeyInfo( PR_LIC_STUB, PR_DESERIALIZE(pResult) );
}
extern "C" tERROR PRLIC_GetInstalledKeysInfo( handle_t IDL_handle, rpcLICENSING _this, rpcSER_PARAM(pResult) ) {
	return CALL_Licensing_GetInstalledKeysInfo( PR_LIC_STUB, PR_DESERIALIZE(pResult) );
}
extern "C" tERROR PRLIC_RevokeActiveKey( handle_t IDL_handle, rpcLICENSING _this ) {
	return CALL_Licensing_RevokeActiveKey( PR_LIC_STUB );
}
extern "C" tERROR PRLIC_RevokeKey( handle_t IDL_handle, rpcLICENSING _this, rpcSER_PARAM(pKeySerialNumber) ) {
	return CALL_Licensing_RevokeKey( PR_LIC_STUB, PR_DESERIALIZE(pKeySerialNumber) );
}
extern "C" tERROR PRLIC_CheckKeyFile( handle_t IDL_handle, rpcLICENSING _this, const wchar_t* szKeyFileName, tDWORD dwKeyAddMode, rpcSER_PARAM(pResult) ) {
	return CALL_Licensing_CheckKeyFile( (hLICENSING)PR_THIS_SECURE_STUB, (tWSTRING)szKeyFileName, dwKeyAddMode, PR_DESERIALIZE(pResult) );
}
extern "C" tERROR PRLIC_CheckActiveKey( handle_t IDL_handle, rpcLICENSING _this, rpcSER_PARAM(pResult) ) {
	return CALL_Licensing_CheckActiveKey( PR_LIC_STUB, PR_DESERIALIZE(pResult) );
}
extern "C" tERROR PRLIC_AddKey( handle_t IDL_handle, rpcLICENSING _this, const wchar_t* szFileName, tDWORD dwKeyAddMode ) {
	return CALL_Licensing_AddKey( (hLICENSING)PR_THIS_SECURE_STUB, (tWSTRING)szFileName, dwKeyAddMode );
}
extern "C" tERROR PRLIC_AddOnlineKey( handle_t IDL_handle, rpcLICENSING _this, rpcSER_PARAM(pUserData), tDWORD dwKeyAddMode, rpcSER_PARAM(pResult) ) {
	return CALL_Licensing_AddOnlineKey( (hLICENSING)PR_THIS_SECURE_STUB, PR_DESERIALIZE(pUserData), dwKeyAddMode, PR_DESERIALIZE(pResult) );
}
extern "C" tERROR PRLIC_StopOnlineLoadKey( handle_t IDL_handle, rpcLICENSING _this ) {
	return CALL_Licensing_StopOnlineLoadKey( PR_LIC_STUB );
}

PR_IMPLEMENT_STUB(IID_LICENSING, PRrpc_licensing_ServerIfHandle)

//////////////////////////////////////////////////////////////////////////////////////////
// CryptoHelper

#include <iface/i_cryptohelper.h>

#define PR_CRYPTOHELPER_STUB		((hCRYPTOHELPER)PR_THIS_STUB)

extern "C" tERROR PRCRYPTOHELPER_Encode( handle_t IDL_handle, rpcCRYPTOHELPER _this, tDWORD* result, const tBYTE* pDecodedData, tDWORD nDataSize, tBYTE* pEncodedBuffer, tDWORD nBufferSize ) {
	return CALL_CryptoHelper_Encode( PR_CRYPTOHELPER_STUB, result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize );
}
extern "C" tERROR PRCRYPTOHELPER_Decode( handle_t IDL_handle, rpcCRYPTOHELPER _this, tDWORD* result, const tBYTE* pEncodedData, tDWORD nDataSize, tBYTE* pDecodedBuffer, tDWORD nBufferSize ) {
	return CALL_CryptoHelper_Decode( PR_CRYPTOHELPER_STUB, result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize );
}
extern "C" tERROR PRCRYPTOHELPER_CryptPassword( handle_t IDL_handle, rpcCRYPTOHELPER _this, rpcSER_PARAM(password), tDWORD flags ) {
	return CALL_CryptoHelper_CryptPassword( PR_CRYPTOHELPER_STUB, (cSerString*)(cSerializable*)PR_DESERIALIZE(password), flags );
}
extern "C" tERROR PRCRYPTOHELPER_IsMSTrusted( handle_t IDL_handle, rpcCRYPTOHELPER _this, const tWCHAR* file_name ) {
	return CALL_CryptoHelper_IsMSTrusted( (hCRYPTOHELPER)PR_THIS_SECURE_STUB, file_name );
}

PR_IMPLEMENT_STUB(IID_CRYPTOHELPER, PRrpc_cryptohelper_ServerIfHandle)
