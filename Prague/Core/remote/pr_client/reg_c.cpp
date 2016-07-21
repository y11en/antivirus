#include "StdAfx.h"
#include <rpc_reg.h>

// ---
tERROR pr_call Local_Registry_OpenKey( hREGISTRY _this, tRegKey* result, tRegKey key, const tCHAR* szSubKeyName, tBOOL bCreateIfNotExist ) { // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must PR_PROXY_CALL( errKEY_NOT_EXISTS warning;
	PR_PROXY_CALL( Registry_OpenKey( PR_THIS_PROXY, result, key, szSubKeyName, bCreateIfNotExist ) );
}
// ---
tERROR pr_call Local_Registry_OpenKeyByIndex( hREGISTRY _this, tRegKey* result, tRegKey key, tDWORD index, tBOOL bClosePrev ) { // -- if parametr bCreateIfNotExist is cTRUE and key was really created method must PR_PROXY_CALL( errKEY_NOT_EXISTS warning;
	PR_PROXY_CALL( Registry_OpenKeyByIndex( PR_THIS_PROXY, result, key, index, bClosePrev ) );
}
// ---
tERROR pr_call Local_Registry_OpenNextKey( hREGISTRY _this, tRegKey* result, tRegKey key, tBOOL bClosePrev ) { // -- opens next key on the same level;
	PR_PROXY_CALL( Registry_OpenNextKey( PR_THIS_PROXY, result, key, bClosePrev ) );
}
// ---
tERROR pr_call Local_Registry_CloseKey( hREGISTRY _this, tRegKey key ) { // -- closes the key;
	PR_PROXY_CALL( Registry_CloseKey( PR_THIS_PROXY, key ) );
}
// ---
tERROR pr_call Local_Registry_GetKeyName( hREGISTRY _this, tDWORD* result, tRegKey key, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) { // -- returns name of the key;
	PR_PROXY_CALL( Registry_GetKeyName( PR_THIS_PROXY, result, key, name_buff, size, bFullName ) );
}
// ---
tERROR pr_call Local_Registry_GetKeyNameByIndex( hREGISTRY _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size, tBOOL bFullName ) { // -- returns name of the subkey by index;
	PR_PROXY_CALL( Registry_GetKeyNameByIndex( PR_THIS_PROXY, result, key, index, name_buff, size, bFullName ) );
}
// ---
tERROR pr_call Local_Registry_GetValue( hREGISTRY _this, tDWORD* result, tRegKey key, const tCHAR* szValueName, tTYPE_ID* type, tPTR pValue, tDWORD size ) { // -- gets key value, returns byte length of the value put in the buffer;
	PR_PROXY_CALL( Registry_GetValue( PR_THIS_PROXY, result, key, szValueName, type, pValue, size ) );
}
// ---
tERROR pr_call Local_Registry_GetValueByIndex( hREGISTRY _this, tDWORD* result, tRegKey key, tDWORD index, tTYPE_ID* type, tPTR pValue, tDWORD size ) { // -- gets key value, returns byte length of the value put in the buffer;
	PR_PROXY_CALL( Registry_GetValueByIndex( PR_THIS_PROXY, result, key, index, type, pValue, size ) );
}
// ---
tERROR pr_call Local_Registry_GetValueNameByIndex( hREGISTRY _this, tDWORD* result, tRegKey key, tDWORD index, tCHAR* name_buff, tDWORD size ) { // -- gets key name, returns byte length of the name put in the buffer;
	PR_PROXY_CALL( Registry_GetValueNameByIndex( PR_THIS_PROXY, result, key, index, name_buff, size ) );
}
// ---
tERROR pr_call Local_Registry_SetValue( hREGISTRY _this, tRegKey key, const tCHAR* szValueName, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) { // -- set key value. Creates key if it is not exist and parameter bCreateIfNotExist is true. Returns warning errKEY_NOT_EXISTS in this case;
	PR_PROXY_CALL( Registry_SetValue( PR_THIS_PROXY, key, szValueName, type, pValue, size, bCreateIfNotExist ) );
}
// ---
tERROR pr_call Local_Registry_SetValueByIndex( hREGISTRY _this, tRegKey key, tDWORD index, tTYPE_ID type, tPTR pValue, tDWORD size, tBOOL bCreateIfNotExist ) { // -- set key value, creates key if it is not exist and PR_PROXY_CALL( warning errKEY_NOT_EXISTS in this case;
	PR_PROXY_CALL( Registry_SetValueByIndex( PR_THIS_PROXY, key, index, type, pValue, size, bCreateIfNotExist ) );
}
// ---
tERROR pr_call Local_Registry_GetKeyCount( hREGISTRY _this, tDWORD* result, tRegKey key ) { // -- gets subkey count;
	PR_PROXY_CALL( Registry_GetKeyCount( PR_THIS_PROXY, result, key ) );
}
// ---
tERROR pr_call Local_Registry_GetValueCount( hREGISTRY _this, tDWORD* result, tRegKey key ) { // -- gets subkey count;
	PR_PROXY_CALL( Registry_GetValueCount( PR_THIS_PROXY, result, key ) );
}
// ---
tERROR pr_call Local_Registry_DeleteKey( hREGISTRY _this, tRegKey key, const tCHAR* szSubKeyName ) { // -- deletes the key;
	PR_PROXY_CALL( Registry_DeleteKey( PR_THIS_PROXY, key, szSubKeyName ) );
}
// ---
tERROR pr_call Local_Registry_DeleteKeyByInd( hREGISTRY _this, tRegKey key, tDWORD index ) { // -- deletes the key;
	PR_PROXY_CALL( Registry_DeleteKeyByInd( PR_THIS_PROXY, key, index ) );
}
// ---
tERROR pr_call Local_Registry_DeleteValue( hREGISTRY _this, tRegKey key, const tCHAR* szValueName ) { // -- deletes the key value;
	PR_PROXY_CALL( Registry_DeleteValue( PR_THIS_PROXY, key, szValueName ) );
}
// ---
tERROR pr_call Local_Registry_DeleteValueByInd( hREGISTRY _this, tRegKey key, tDWORD index ) { // -- deletes the key value;
	PR_PROXY_CALL( Registry_DeleteValueByInd( PR_THIS_PROXY, key, index ) );
}
// ---
tERROR pr_call Local_Registry_Clean( hREGISTRY _this ) {            // -- clean the object contained registry data from the root key;
	PR_PROXY_CALL( Registry_Clean( PR_THIS_PROXY ) );
}





// ---
iRegistryVtbl registry_iface = {
	Local_Registry_OpenKey,
	Local_Registry_OpenKeyByIndex,
	Local_Registry_OpenNextKey,
	Local_Registry_CloseKey,
	Local_Registry_GetKeyName,
	Local_Registry_GetKeyNameByIndex,
	Local_Registry_GetValue,
	Local_Registry_GetValueByIndex,
	Local_Registry_GetValueNameByIndex,
	Local_Registry_SetValue,
	Local_Registry_SetValueByIndex,
	Local_Registry_GetKeyCount,
	Local_Registry_GetValueCount,
	Local_Registry_DeleteKey,
	Local_Registry_DeleteKeyByInd,
	Local_Registry_DeleteValue,
	Local_Registry_DeleteValueByInd,
	Local_Registry_Clean,
}; // "Registry" external virtual table prototype


PR_IMPLEMENT_PROXY(IID_REGISTRY, registry_iface)


