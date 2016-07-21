// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  18 October 2004,  11:28 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Properties map
// Author      -- Sychev
// File Name   -- plugin_propertiesmap.cpp
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end

#include <plugin/p_propertiesmap.h>

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h> 


#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
#include <plugin/p_win32_nfio.h> 
	IMPORT_TABLE_END


// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Registry_Register(hROOT root);

// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = 0;

extern "C" tBOOL __stdcall DllMain(tPTR hInstance, tDWORD dwReason, tERROR* pERROR)
{
	hROOT  root;
	tDWORD count;

	switch(dwReason){
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH :
		break;

	case PRAGUE_PLUGIN_LOAD :
		g_root = root = (hROOT)hInstance;
		*pERROR = errOK;
		//resolve  my imports
		if(PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table,PID_PROPERTIESMAP))) {
			PR_TRACE((root, prtERROR, "cannot resolve import table for propertiesmap"));
			return cFALSE;
		}

		//register my exports
		//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_PROPERTIESMAP );

		//register my custom property ids
		//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
		//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
		//  return cFALSE;
		//}

		// register my interfaces
		if(PR_FAIL(*pERROR=Registry_Register(root))) {
			PR_TRACE(( root, prtERROR, "cannot register \"Registry\" interface"));
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



