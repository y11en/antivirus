// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  02 June 2005,  13:26 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- plugin_antiphishing.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <AntiPhishing/plugin/p_antiphishing.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );

// AVP Prague stamp end



#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_string.h>
//#include <plugin/p_win32api.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END

/*#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table_win32api)
	#include <plugin/p_win32api.h>
IMPORT_TABLE_END*/


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT  g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) {
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pError = errOK;
			//resolve  my imports
			if ( PR_FAIL(*pError=CALL_Root_ResolveImportTable(g_root,NULL,import_table,PID_ANTIPHISHING)) )
			{
			   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for PID_ANTIPHISHING" ));
			   return cFALSE;
			}

			/*if ( PR_FAIL(CALL_Root_ResolveImportTable(g_root,NULL,import_table_win32api,PID_ANTIPHISHING)) )
			{
			   PR_TRACE(( g_root, prtERROR, "cannot resolve Win32API import table for PID_ANTIPHISHING" ));
			}*/

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_ANTIPHISHING );

			//register my custom property ids
			//if ( PR_FAIL(*pError=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pError=Task_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"Task\" interface"));
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



