#include "stdafx.h"
#include "rpc_common.h"

//////////////////////////////////////////////////////////////////////////////////////////
// License

#include <iface/i_licensing.h>

tERROR pr_call Local_LIC_GetActiveKeyInfo( hLICENSING _this, cSerializable* pResult ) {
	PR_PROXY_CALL_EX( LIC_GetActiveKeyInfo( PR_THIS_PROXY, PR_SERIALIZE(pResult) ), PR_SERCLEANUP(pResult) );
}
tERROR pr_call Local_LIC_GetInstalledKeysInfo( hLICENSING _this, cSerializable* pResult ) {
	PR_PROXY_CALL_EX( LIC_GetInstalledKeysInfo( PR_THIS_PROXY, PR_SERIALIZE(pResult) ), PR_SERCLEANUP(pResult) );
}
tERROR pr_call Local_LIC_RevokeActiveKey( hLICENSING _this ) {
	PR_PROXY_CALL( LIC_RevokeActiveKey( PR_THIS_PROXY ) );
}
tERROR pr_call Local_LIC_RevokeKey( hLICENSING _this, cSerializable* pKeySerialNumber ) {
	PR_PROXY_CALL_EX( LIC_RevokeKey( PR_THIS_PROXY, PR_SERIALIZE(pKeySerialNumber) ), PR_SERCLEANUP(pKeySerialNumber) );
}
tERROR pr_call Local_LIC_CheckKeyFile( hLICENSING _this, tWSTRING szKeyFileName, tDWORD dwKeyAddMode, cSerializable* pResult ) {
	PR_PROXY_CALL_EX( LIC_CheckKeyFile( PR_THIS_PROXY, szKeyFileName, dwKeyAddMode, PR_SERIALIZE(pResult) ), PR_SERCLEANUP(pResult) );
}
tERROR pr_call Local_LIC_CheckActiveKey( hLICENSING _this, cSerializable* pResult ) {
	PR_PROXY_CALL_EX( LIC_CheckActiveKey( PR_THIS_PROXY, PR_SERIALIZE(pResult) ), PR_SERCLEANUP(pResult) );
}
tERROR pr_call Local_LIC_AddKey( hLICENSING _this, tWSTRING szFileName, tDWORD dwKeyAddMode ) {
	PR_PROXY_CALL( LIC_AddKey( PR_THIS_PROXY, szFileName, dwKeyAddMode ) );
}
tERROR pr_call Local_LIC_AddOnlineKey( hLICENSING _this, cSerializable* pUserData, tDWORD dwKeyAddMode, cSerializable* pResult ) {
	PR_PROXY_CALL_EX( LIC_AddOnlineKey( PR_THIS_PROXY, PR_SERIALIZE(pUserData), dwKeyAddMode, PR_SERIALIZE(pResult) ), PR_SERCLEANUP(pResult) );
}
tERROR pr_call Local_LIC_StopOnlineLoadKey( hLICENSING _this ) {
	PR_PROXY_CALL( LIC_StopOnlineLoadKey( PR_THIS_PROXY ) );
}

iLicensingVtbl licensing_iface = {
	Local_LIC_GetActiveKeyInfo,
	Local_LIC_GetInstalledKeysInfo,
	Local_LIC_RevokeActiveKey,
	Local_LIC_RevokeKey,
	Local_LIC_CheckKeyFile,
	Local_LIC_CheckActiveKey,
	Local_LIC_AddKey,
	Local_LIC_AddOnlineKey,
	Local_LIC_StopOnlineLoadKey,
}; // "LICENSING" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_LICENSING, licensing_iface)


//////////////////////////////////////////////////////////////////////////////////////////
// CryptoHelper

#include <iface/i_cryptohelper.h>

tERROR pr_call Local_CRYPTOHELPER_Encode( hCRYPTOHELPER _this, tDWORD* result, const tBYTE* pDecodedData, tDWORD nDataSize, tBYTE* pEncodedBuffer, tDWORD nBufferSize ) {
	PR_PROXY_CALL( CRYPTOHELPER_Encode( PR_THIS_PROXY, result, pDecodedData, nDataSize, pEncodedBuffer, nBufferSize ) );
}
tERROR pr_call Local_CRYPTOHELPER_Decode( hCRYPTOHELPER _this, tDWORD* result, const tBYTE* pEncodedData, tDWORD nDataSize, tBYTE* pDecodedBuffer, tDWORD nBufferSize ) {
	PR_PROXY_CALL( CRYPTOHELPER_Decode( PR_THIS_PROXY, result, pEncodedData, nDataSize, pDecodedBuffer, nBufferSize ) );
}
tERROR pr_call Local_CRYPTOHELPER_CryptPassword( hCRYPTOHELPER _this, cSerString* password, tDWORD flags ) {
	PR_PROXY_CALL_EX( CRYPTOHELPER_CryptPassword( PR_THIS_PROXY, PR_SERIALIZE(password), flags ), PR_SERCLEANUP(password) );
}
tERROR pr_call Local_CRYPTOHELPER_IsMSTrusted( hCRYPTOHELPER _this, const tWCHAR* file_name ) {
	PR_PROXY_CALL( CRYPTOHELPER_IsMSTrusted( PR_THIS_PROXY, file_name ) );
}

iCryptoHelperVtbl cryptohelper_iface = {
	Local_CRYPTOHELPER_Encode,
	Local_CRYPTOHELPER_Decode,
	Local_CRYPTOHELPER_CryptPassword,
	Local_CRYPTOHELPER_IsMSTrusted,
}; // "CRYPTOHELPER" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_CRYPTOHELPER, cryptohelper_iface)
