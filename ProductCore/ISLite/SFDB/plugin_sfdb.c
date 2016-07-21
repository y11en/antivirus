// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  16 February 2005,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Integrated Solution
// Sub project -- Integrated Solution Lite
// Purpose     -- Secure Files Database Plugin
// Author      -- mike
// File Name   -- plugin_sfdb.c
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



#include <ProductCore/plugin/p_sfdb.h> 
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/e_ktime.h> 

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call SecureFileDatabase_Register( hROOT root );

// AVP Prague stamp end



#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/e_ktime.h> 
IMPORT_TABLE_END

hROOT g_root = NULL;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pERROR = errOK;
			if (PR_FAIL(*pERROR = CALL_Root_ResolveImportTable( g_root, NULL, import_table, PID_SFDB)))
			{
				PR_TRACE(( g_root, prtERROR, "Cannot resolve import table for SCR_VBS: %terr", *pERROR));
				return cFALSE;
			}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_SFDB );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=SecureFileDatabase_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"SecureFileDatabase\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



