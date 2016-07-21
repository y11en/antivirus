// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  18 August 2006,  14:21 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- PPP
// Sub project -- Not defined
// Purpose     -- Report Storage
// Author      -- Pavlushchik
// File Name   -- plugin_report.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



#include <Prague/iface/i_mutex.h>
#include <ProductCore/plugin/p_report.h>

#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/e_ktime.h> 

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( imp_loader )
#include <Prague/iface/e_ktime.h> 
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/e_ktrace.h>
IMPORT_TABLE_END


// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Report_Register( hROOT root );

// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pERROR = errOK;

			if (PR_FAIL(*pERROR = CALL_Root_ResolveImportTable(g_root, NULL, imp_loader, PID_REPORT)))
			{
				PR_TRACE(( g_root, prtERROR, "Cannot import loader exports into PID_REPORT, %terr", *pERROR));
				return cFALSE;
			}
			
			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_REPORT );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=Report_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"Report\" interface"));
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



