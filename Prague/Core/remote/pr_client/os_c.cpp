#include "StdAfx.h"
#include <rpc_os.h>

// ---
tERROR pr_call Local_OS_PtrCreate( hOS _this, hObjPtr* result, hOBJECT name ) {              // -- opens ObjPtr object;
	PR_PROXY_CALL( OS_PtrCreate( PR_THIS_PROXY, PR_OUT_LOCAL(result), PR_REMOTE(name) ) );
}
// ---
tERROR pr_call Local_OS_IOCreate( hOS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ) { // -- opens IO object;
	PR_PROXY_CALL( OS_IOCreate( PR_THIS_PROXY, PR_OUT_LOCAL(result), PR_REMOTE(name), access_mode, open_mode ) );
}
// ---
tERROR pr_call Local_OS_Copy( hOS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ) { // -- copies object inside OS;
	PR_PROXY_CALL( OS_Copy( PR_THIS_PROXY, PR_REMOTE(src_name), PR_REMOTE(dst_name), overwrite ) );
}
// ---
tERROR pr_call Local_OS_Rename( hOS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ) { // -- rename/move object by name inside OS;
	PR_PROXY_CALL( OS_Rename( PR_THIS_PROXY, PR_REMOTE(old_name), PR_REMOTE(new_name), overwrite ) );
}
// ---
tERROR pr_call Local_OS_Delete( hOS _this, hOBJECT name ) {              // -- phisically deletes an entity on OS;
	PR_PROXY_CALL( OS_Delete( PR_THIS_PROXY, PR_REMOTE(name) ) );
}


// ---
iOSVtbl os_iface = {
	Local_OS_PtrCreate,
	Local_OS_IOCreate,
	Local_OS_Copy,
	Local_OS_Rename,
	Local_OS_Delete,
}; // "OS" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_OS, os_iface)


