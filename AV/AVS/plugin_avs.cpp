// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  30 January 2007,  14:16 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_avs.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call AVS_Register( hROOT root );
extern tERROR pr_call AVSTreats_Register( hROOT root );
extern tERROR pr_call IO_Register( hROOT root );
extern tERROR pr_call AVSSession_Register( hROOT root );

// AVP Prague stamp end

PR_NEW_DECLARE;
hROOT g_root;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
			break;
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_AVS)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_AVS );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=AVS_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"AVS\" interface"));
				return cFALSE;
			}
			if ( PR_FAIL(*pERROR=AVSSession_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"AVSSession\" interface"));
				return cFALSE;
			}
			if ( PR_FAIL(*pERROR=AVSTreats_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"AVSTreats\" interface"));
				return cFALSE;
			}
			if ( PR_FAIL(*pERROR=IO_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"IO\" interface"));
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

