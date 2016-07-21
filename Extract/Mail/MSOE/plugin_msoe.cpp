// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  25 October 2002,  17:48 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- msoe
// Sub project -- msoe
// Purpose     -- Outlook Express plugin
// Author      -- Mezhuev
// File Name   -- plugin_msoe.h
// Additional info
//    Plugin for working with all Outlook Express objects.
// -------------------------------------------
// AVP Prague stamp end


// AVP Prague stamp begin( Header includes,  )
#include "msoe_base.h"

#include "msoe_os.h"
#include "msoe_objptr.h"
#include "msoe_io.h"

#include <Prague/prague.h>

#include <string.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT 	g_root = NULL;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH :
			PROEObject::ThreadAttach(true);
			break;
		case DLL_THREAD_DETACH :
			PROEObject::ThreadAttach(false);
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_MSOE)) ) {
			//   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_MSOE );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=OS_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"OS\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=ObjPtr_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"ObjPtr\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=IO_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"IO\" interface"));
				return cFALSE;
			}
			PROEObject::Initialize(g_root);
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			PROEObject::Deinitialize();
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end

