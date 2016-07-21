// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  18 December 2002,  13:59 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- plugin_arj.h
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF


// AVP Prague stamp begin( Header includes,  )
#include "arj.h"

#include <Prague/prague.h>
#include <Prague/iface/e_ktime.h> 
#include <Prague/iface/e_loader.h>
// AVP Prague stamp end


#define  IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_ktime.h> 
#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT  g_root;

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

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_ARJ );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=MiniArchiver_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"MiniArchiver\" interface"));
				return cFALSE;
			}
			//resolve  my imports
			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable((hROOT)hInstance , NULL, import_table, PID_APPLICATION)))
				return cFALSE;
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



