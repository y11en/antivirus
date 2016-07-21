// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  01 October 2002,  14:42 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- MiniKav
// Sub project -- Not defined
// Purpose     -- Ring 0 IO from MiniKav
// Author      -- Sobko
// File Name   -- plugin_mkavio.h
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include "io.h"
// AVP Prague stamp end

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktrace.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h>
	#include <Prague/iface/e_loader.h>
	#include <Prague/iface/e_ktrace.h>
IMPORT_TABLE_END

#undef IMPEX_TABLE_CONTENT
#undef IMPEX_IMPORT_LIB


//+ ----------------------------------------------------------------------------------------
#include "../../windows/Hook/hook/minikav_api.h"
#include "windows.h"

#include <AV/iface/i_avpg.h>
tPROPID propid_EventDefinition;			// cAVPG_EVENT_DEFINITION_PROP
//+ ----------------------------------------------------------------------------------------

EXTERN_C hROOT g_root = NULL;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


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
			root = (hROOT)hInstance;
			g_root = root;

			if ( PR_FAIL( CALL_Root_ResolveImportTable(root, NULL, import_table, PID_MKAVIO) ) )
			{
				PR_TRACE(( root, prtERROR, "e_ktime exports cannot be imported" ));
				return cFALSE;
			}


			if (PR_FAIL(CALL_Root_RegisterCustomPropId((hROOT) hInstance, &propid_EventDefinition, cAVPG_EVENT_DEFINITION_PROP, pTYPE_DWORD | pCUSTOM_HERITABLE)))
			{
				PR_TRACE(( root, prtERROR, "cannot register custom property cAVPG_EVENT_DEFINITION_PROP"));
				return cFALSE;
			}
			
			*pERROR = errOK;

			// register my interfaces
			if ( PR_FAIL(*pERROR=IO_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"IO\" interface"));
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



