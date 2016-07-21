#include "StdAfx.h"
#include <rpc_engine.h>

// ---
tERROR pr_call Local_Engine_Process( hENGINE _this, hOBJECT obj ) {   // -- Обработать объект.//eng:Process object.;
	PR_PROXY_CALL( Engine_Process( PR_THIS_PROXY, PR_REMOTE(obj) ) );
}
// ---
tERROR pr_call Local_Engine_GetAssociatedInfo( hENGINE _this, hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ) { // -- Получить ассоциированные данные.//eng:Get associated data.;
	PR_PROXY_CALL( Engine_GetAssociatedInfo( PR_THIS_PROXY, PR_REMOTE(obj), info_id, param, (tCHAR*)buffer, size, out_size ) );
}
// ---
tERROR pr_call Local_Engine_GetExtendedInfo( hENGINE _this, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ) { // -- Получить дополнительные свойства.//eng:Get extended information.;
	PR_PROXY_CALL( Engine_GetExtendedInfo( PR_THIS_PROXY, info_id, param, (tCHAR*)buffer, size, out_size ) );
}
// ---
tERROR pr_call Local_Engine_LoadConfiguration( hENGINE _this, hREGISTRY reg, tRegKey key ) { // -- Инициализирует конфигурацию Engine ;
	PR_PROXY_CALL( Engine_LoadConfiguration( PR_THIS_PROXY, PR_REMOTE(reg), key ) );
}

// AVP Prague stamp begin( Interface declaration,  )
iEngineVtbl engine_iface = {
	Local_Engine_Process,
	Local_Engine_GetAssociatedInfo,
	Local_Engine_GetExtendedInfo,
	Local_Engine_LoadConfiguration,
}; // "Engine" external virtual table prototype
// AVP Prague stamp end

PR_IMPLEMENT_PROXY(IID_ENGINE, engine_iface)


