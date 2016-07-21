#include "StdAfx.h"
#include <rpc_string.h>


// ----------- Methods to get string value ----------
// ---
tERROR pr_call Local_String_ExportToProp( hSTRING _this, tDWORD* result, tSTR_RANGE range, hOBJECT to, tPROPID to_propid ) { // -- saves string content to object's property;
	PR_PROXY_CALL( String_ExportToProp( PR_THIS_PROXY, result, range, PR_REMOTE(to), to_propid ) );
}
// ---
tERROR pr_call Local_String_ExportToStr( hSTRING _this, tDWORD* result, tSTR_RANGE range, hSTRING to ) { // -- saves string content to another string;
	PR_PROXY_CALL( String_ExportToStr( PR_THIS_PROXY, result, range, PR_REMOTE(to) ) );
}
// ---
tERROR pr_call Local_String_ExportToBuff( hSTRING _this, tDWORD* result, tSTR_RANGE range, tPTR to, tDWORD length, tCODEPAGE cp, tDWORD flags ) { // -- saves content of string to target buffer;
	PR_PROXY_CALL( String_ExportToBuff( PR_THIS_PROXY, result, range, (tCHAR*)to, length, cp, flags ) );
}

// ----------- Methods to put string value ----------
// ---
tERROR pr_call Local_String_ImportFromProp( hSTRING _this, tDWORD* result, hOBJECT from, tPROPID propid ) { // -- Loads string from object's property;
	PR_PROXY_CALL( String_ImportFromProp( PR_THIS_PROXY, result, PR_REMOTE(from), propid ) );
}
// ---
tERROR pr_call Local_String_ImportFromStr( hSTRING _this, tDWORD* result, hSTRING from, tSTR_RANGE range ) { // -- loads contents from source string;
	PR_PROXY_CALL( String_ImportFromStr( PR_THIS_PROXY, result, PR_REMOTE(from), range ) );
}
// ---
tERROR pr_call Local_String_ImportFromBuff( hSTRING _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ) { // -- loads content of the source buffer to itself;
	PR_PROXY_CALL( String_ImportFromBuff( PR_THIS_PROXY, result, (tCHAR*)from, length, cp, flags ) );
}

// ----------- Miscelanous methods ----------
// ---
tERROR pr_call Local_String_Length( hSTRING _this, tDWORD* result ) {                   // -- returns current length of the string in symbols;
	PR_PROXY_CALL( String_Length( PR_THIS_PROXY, result ) );
}
// ---
tERROR pr_call Local_String_LengthEx( hSTRING _this, tDWORD* result, tSTR_RANGE range, tCODEPAGE cp, tDWORD flags ) { // -- returns size of the (sub)string in bytes or in symbols;
	PR_PROXY_CALL( String_LengthEx( PR_THIS_PROXY, result, range, cp, flags ) );
}
// ---
tERROR pr_call Local_String_GetCP( hSTRING _this, tCODEPAGE* result ) {                   // -- returns current codepage;
	PR_PROXY_CALL( String_GetCP( PR_THIS_PROXY, result ) );
}
// ---
tERROR pr_call Local_String_SetCP( hSTRING _this, tCODEPAGE cp ) {     // -- sets default code page for the string;
	PR_PROXY_CALL( String_SetCP( PR_THIS_PROXY, cp ) );
}


// ---
iStringVtbl string_iface = {
	
	// ----------- Methods to get string value ----------
	Local_String_ExportToProp,
	Local_String_ExportToStr,
	Local_String_ExportToBuff,
	NULL,
	
	// ----------- Methods to put string value ----------
	Local_String_ImportFromProp,
	Local_String_ImportFromStr,
	Local_String_ImportFromBuff,
	NULL,
	
	// ----------- Concatenation methods ----------
	NULL,
	NULL,
	NULL,
	NULL,
	
	// ----------- Miscelanous methods ----------
	NULL,
	Local_String_Length,
	Local_String_LengthEx,
	Local_String_GetCP,
	Local_String_SetCP,
	
	// ----------- Compare methods ----------
	NULL,
	NULL,
	NULL,
	NULL,
	
	// ----------- Find methods ----------
	NULL,
	NULL,
	NULL,
	NULL,
	
	// ----------- Range concerned methods ----------
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	
	// ----------- Modification methods ----------
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
}; // "String" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_STRING, string_iface)


