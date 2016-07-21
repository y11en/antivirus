// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  18 October 2002,  12:46 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Test
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- plugin_substio.h
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include "io.h"
#include "objptr.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = 0;

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
			// register my interfaces
			if ( PR_FAIL(*pERROR=IO_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"IO\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=ObjPtr_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"Ptr\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			g_root = 0;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



