// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  13 December 2004,  16:50 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Not defined
// Author      -- Mike Pavlyushchik
// File Name   -- plugin_scheduler.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



#include <ProductCore/plugin/p_scheduler.h>

#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktrace.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h>
	#include <Prague/iface/e_loader.h>
	#include <Prague/iface/e_ktrace.h>
IMPORT_TABLE_END

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Scheduler_Register( hROOT root );

// AVP Prague stamp end



EXTERN_C hROOT g_root = NULL;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	tDWORD count;

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
			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(::g_root,&count,import_table,PID_SCHEDULER)) ) {
			   PR_TRACE(( ::g_root, prtERROR, "cannot resolve import table for PID_SCHEDULER" ));
			   return cFALSE;
			}

			//register my exports
			//CALL_Root_RegisterExportTable( ::g_root, &count, export_table_..., PID_SCHEDULER );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(::g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( ::g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=Scheduler_Register(::g_root)) ) {
				PR_TRACE(( ::g_root, prtERROR, "cannot register \"Scheduler\" interface"));
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



