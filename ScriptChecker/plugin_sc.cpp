// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  23 December 2003,  13:46 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Script Checker
// Author      -- Pavlushchik
// File Name   -- plugin_sc.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end

#include <AV/iface/i_engine.h>

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );

// AVP Prague stamp end

hROOT g_root;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

tPROPID propid_object_detect_counter = 0;

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
			g_root = root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_SCRIPTCHECKER)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_SCRIPTCHECKER );

			//register my custom property ids
			if (PR_FAIL(*pERROR = CALL_Root_RegisterCustomPropId( root, &propid_object_detect_counter, npENGINE_OBJECT_DETECT_COUNTER_tDWORD, pTYPE_DWORD )))
				return cFALSE;

			// register my interfaces
			if ( PR_FAIL(*pERROR=Task_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"Task\" interface"));
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



