#include "StdAfx.h"
#include <rpc_ifenum.h>

// ---
tERROR pr_call Local_IFaceEnum_Init( hIFACE_ENUM _this, tIID iid, tPID pid, tVID vid, hOBJECT object ) { // -- Initialization procedure;
	PR_PROXY_CALL( IFaceEnum_Init( PR_THIS_PROXY, iid, pid, vid, PR_REMOTE(object) ) );
}
// ---
tERROR pr_call Local_IFaceEnum_Clone( hIFACE_ENUM _this, hIFACE_ENUM* result, hOBJECT dad ) {     // -- Clone current enumerator;
	PR_PROXY_CALL( IFaceEnum_Clone( PR_THIS_PROXY, PR_OUT_LOCAL(result), PR_REMOTE(dad) ) );
}
// ---
tERROR pr_call Local_IFaceEnum_Reset( hIFACE_ENUM _this ) {                  // -- Resets current position of enumerator;
	PR_PROXY_CALL( IFaceEnum_Reset( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_IFaceEnum_Next( hIFACE_ENUM _this ) {                  // -- Advance current pointer;
	PR_PROXY_CALL( IFaceEnum_Next( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_IFaceEnum_GetIFaceProp( hIFACE_ENUM _this, tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD size ) { // -- Get static properties of current interface;
	PR_PROXY_CALL( IFaceEnum_GetIFaceProp( PR_THIS_PROXY, result, prop_id, (tCHAR*)buffer, size ) );
}
// ---
tERROR pr_call Local_IFaceEnum_ObjectCreate( hIFACE_ENUM _this, hOBJECT* result, hOBJECT parent ) {  // -- Create instance of current interface;
	PR_PROXY_CALL( IFaceEnum_ObjectCreate( PR_THIS_PROXY, PR_OUT_LOCAL(result), PR_REMOTE(parent) ) );
}
// ---
tERROR pr_call Local_IFaceEnum_Recognize( hIFACE_ENUM _this, hOBJECT object, tDWORD type ) {  // -- regonize the "object";
	PR_PROXY_CALL( IFaceEnum_Recognize( PR_THIS_PROXY, PR_REMOTE(object), type ) );
}
// ---
tERROR pr_call Local_IFaceEnum_Load( hIFACE_ENUM _this ) {                  // -- load the interface and increment objects count;
	PR_PROXY_CALL( IFaceEnum_Load( PR_THIS_PROXY ) );
}
// ---
tERROR pr_call Local_IFaceEnum_Free( hIFACE_ENUM _this ) {                  // -- decrements objects count and unload the interface if possible;
	PR_PROXY_CALL( IFaceEnum_Free( PR_THIS_PROXY ) );
}


// ---
iIFaceEnumVtbl ifaceenum_iface = {
	Local_IFaceEnum_Init,
		Local_IFaceEnum_Clone,
		Local_IFaceEnum_Reset,
		Local_IFaceEnum_Next,
		Local_IFaceEnum_GetIFaceProp,
		Local_IFaceEnum_ObjectCreate,
		Local_IFaceEnum_Recognize,
		Local_IFaceEnum_Load,
		Local_IFaceEnum_Free,
}; // "IFaceEnum" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_IFACE_ENUM, ifaceenum_iface)
