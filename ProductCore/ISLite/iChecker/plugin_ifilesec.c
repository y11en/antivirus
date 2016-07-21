// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  19 October 2002,  17:46 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Integrated Solution Lite
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- plugin_ifilesec.h
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include "ifilesec.h"
#include "/prague/avpgs/timestats.h"
// AVP Prague stamp end

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include "/prague/avpgs/timestats.h"
IMPORT_TABLE_END


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;

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
			if ( PR_FAIL(CALL_Root_ResolveImportTable(root,0,import_table,PID_IFILESEC)) ) {
			   PR_TRACE(( root, prtERROR, "Cannot resolve exStatisticXXX import table for IFILESEC" ));
			   exStatisticEnabled = 0;
			}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_IFILESEC );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=iFileSecureStatus_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"iFileSecureStatus\" interface"));
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



