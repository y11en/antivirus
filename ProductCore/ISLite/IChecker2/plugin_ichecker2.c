// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  05 February 2003,  16:34 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- IChecker
// Author      -- Pavlushchik
// File Name   -- plugin_ichecker2.h
// Additional info
//    Integrated Solution Lite
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include "ichecker2.h"
#include "plugin_ichecker2.h"
//#include "../../avpgs/timestats.h"
#include "forcedbgout.h"
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

/*#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include "../../avpgs/timestats.h"
IMPORT_TABLE_END*/

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

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
/*			if ( PR_FAIL(CALL_Root_ResolveImportTable(g_root,0,import_table,PID_ICHECKER2)) ) {
			   PR_TRACE(( g_root, prtNOTIFY, "Cannot resolve exStatisticXXX import table for ICHECKER2" ));
			   exStatisticEnabled = 0;
			}*/

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_ICHECKER2 );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=IChecker_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"IChecker\" interface"));
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



