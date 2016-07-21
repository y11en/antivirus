// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Saturday,  10 April 2004,  17:27 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- plugin_string2.cpp
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call String_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <plugin/p_string2.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <plugin/p_string2.h>
EXPORT_TABLE_END
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_STRING2)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_STRING2 );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=String_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"String\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strlen )
// Parameters are:
tDWORD pr_strlen( const tVOID* p_str, tCODEPAGE p_cp, tDWORD p_size ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strlenEx )
// Parameters are:
tDWORD pr_strlenEx( const tVOID* p_src_str, tCODEPAGE p_src_cp, tDWORD p_src_size, tCODEPAGE p_dst_cp ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strcpy )
// Parameters are:
tERROR pr_strcpy( tVOID* p_dst_str, tCODEPAGE p_dst_cp, tDWORD p_dst_size, const tVOID* p_src_str, tCODEPAGE p_src_cp, tDWORD p_src_size ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strcpy2 )
// Parameters are:
tERROR pr_strcpy2( tVOID* p_dst_buff, tCODEPAGE p_dst_cp, tDWORD p_dst_size, hOBJECT p_src_obj, tPROPID p_src_propid ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strcpyEx )
// Parameters are:
tERROR pr_strcpyEx( tVOID* p_dst_str, tCODEPAGE p_dst_cp, tDWORD p_dst_size, tDWORD p_dst_flags, const tVOID* p_src_str, tCODEPAGE p_src_cp, tDWORD p_src_size, tDWORD p_src_flags ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strcat )
// Parameters are:
tERROR pr_strcat( const tVOID* p_str, tCODEPAGE p_str_cp, tDWORD p_str_size, const tVOID* p_add_str, tCODEPAGE p_add_cp, tDWORD p_add_size ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strcat2 )
// Parameters are:
tERROR pr_strcat2( tVOID* p_dst_buff, tCODEPAGE p_dst_cp, tDWORD p_dst_size, hOBJECT p_src_obj, tPROPID p_src_propid ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strcmp )
// Parameters are:
tERROR pr_strcmp( const tVOID* p_str1, tCODEPAGE p_str1_cp, tDWORD p_str1_size, const tVOID* p_str2, tCODEPAGE p_str2_cp, tDWORD p_str2_size, tDWORD p_flags ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strstr )
// Parameters are:
tPTR pr_strstr( const tVOID* p_str, tCODEPAGE p_str_cp, tDWORD p_str_size, const tVOID* p_sub_str, tCODEPAGE p_sub_cp, tDWORD p_sub_size ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strchr )
// Parameters are:
tPTR pr_strchr( const tVOID* p_str, tCODEPAGE p_str_cp, tDWORD p_str_size, tDWORD p_find_symb ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strCalcDstSize )
// Parameters are:
tDWORD pr_strCalcDstSize( tCODEPAGE p_src_cp, tDWORD p_src_size, tCODEPAGE p_dst_cp ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strExportToProp )
// Parameters are:
tERROR pr_strExportToProp( hOBJECT p_dst_obj, tPROPID p_dst_propid, const tVOID* p_src_buff, tCODEPAGE p_src_cp, tDWORD p_src_size ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strGetSymb )
// Parameters are:
tDWORD pr_strGetSymb( const tVOID* p_str, tCODEPAGE p_str_cp, tDWORD p_str_size ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin export method implementation, pr_strStep )
// Parameters are:
tPTR pr_strStep( const tVOID* p_str, tCODEPAGE p_str_cp, tDWORD p_str_size, tINT p_steps ) {

	// Place your code here

	return 0;
}
// AVP Prague stamp end



