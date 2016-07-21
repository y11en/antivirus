// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  27 September 2004,  16:24 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_qb.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end

#include <Prague/iface/e_ktrace.h>


hROOT  g_root;

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call PosIO_SP_Register( hROOT root );
extern tERROR pr_call QBRestorer_Register( hROOT root );
extern tERROR pr_call QB_Register( hROOT root );

// AVP Prague stamp end

PR_NEW_DECLARE;

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <Prague/plugin/p_string.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_ktrace.h>
#include <Prague/plugin/p_win32_nfio.h>
IMPORT_TABLE_END

#include <ProductCore/plugin/p_qb.h>


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
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
			//resolve  my imports
			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,NULL,import_table, PID_QB)) ) {
			   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for PID_QB" ));
			   return cFALSE;
			}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_QB );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=QB_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"QB\" interface"));
				return cFALSE;
			}
			if ( PR_FAIL(*pERROR=PosIO_SP_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"PosIO_SP\" interface"));
				return cFALSE;
			}
			if ( PR_FAIL(*pERROR=QBRestorer_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"QBRestorer\" interface"));
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



