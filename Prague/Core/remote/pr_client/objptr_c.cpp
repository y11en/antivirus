#include "StdAfx.h"
#include <rpc_objptr.h>

// ----------- operations with the current object ----------
tERROR pr_call Local_ObjPtr_IOCreate( hObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ) { // -- creates object current in enumeration;
	PR_PROXY_CALL( ObjPtr_IOCreate( PR_THIS_PROXY, PR_OUT_LOCAL(result), PR_REMOTE(name), access_mode, open_mode ) );
}
// ---
tERROR pr_call Local_ObjPtr_Copy( hObjPtr _this, hOBJECT dst_name, tBOOL overwrite ) { // -- copies the object pointed by the ptr;
	PR_PROXY_CALL( ObjPtr_Copy( PR_THIS_PROXY, PR_REMOTE(dst_name), overwrite ) );
}
// ---
tERROR pr_call Local_ObjPtr_Rename( hObjPtr _this, hOBJECT new_name, tBOOL overwrite ) { // -- renames the object pointed by the ptr;
	PR_PROXY_CALL( ObjPtr_Rename( PR_THIS_PROXY, PR_REMOTE(new_name), overwrite ) );
}
// ---
tERROR pr_call Local_ObjPtr_Delete( hObjPtr _this ) {                         // -- deletes object pointed by ptr;
	PR_PROXY_CALL( ObjPtr_Delete( PR_THIS_PROXY ) );
}

// ---// ----------- navigation methods ----------
tERROR pr_call Local_ObjPtr_Reset( hObjPtr _this, tBOOL to_root ) {          // -- Reinitializing of ObjEnum object;
	PR_PROXY_CALL( ObjPtr_Reset( PR_THIS_PROXY, to_root ) );
}
// ---
tERROR pr_call Local_ObjPtr_Clone( hObjPtr _this, hObjPtr* result ) {                         // -- Makes another ObjPtr object;
	PR_PROXY_CALL( ObjPtr_Clone( PR_THIS_PROXY, PR_OUT_LOCAL(result) ) );
}
// ---
tERROR pr_call Local_ObjPtr_Next( hObjPtr _this ) {                         // -- Returns next IO object;
	PR_PROXY_CALL( ObjPtr_Next( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_ObjPtr_StepUp( hObjPtr _this ) {                         // -- Goes one level up;
	PR_PROXY_CALL( ObjPtr_StepUp( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_ObjPtr_StepDown( hObjPtr _this ) {                         // -- Goes one level up;
	PR_PROXY_CALL( ObjPtr_StepDown( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_ObjPtr_ChangeTo( hObjPtr _this, hOBJECT name ) {           // -- Changes folder for enumerating;
	PR_PROXY_CALL( ObjPtr_ChangeTo( PR_THIS_PROXY, PR_REMOTE(name) ) );
}



// ---
iObjPtrVtbl objptr_iface = {
	
	// ----------- operations with the current object ----------
	Local_ObjPtr_IOCreate,
	Local_ObjPtr_Copy,
	Local_ObjPtr_Rename,
	Local_ObjPtr_Delete,
	
	// ----------- navigation methods ----------
	Local_ObjPtr_Reset,
	Local_ObjPtr_Clone,
	Local_ObjPtr_Next,
	Local_ObjPtr_StepUp,
	Local_ObjPtr_StepDown,
	Local_ObjPtr_ChangeTo,
}; // "ObjPtr" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_OBJPTR, objptr_iface)


