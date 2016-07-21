// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  06 October 2004,  13:32 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_mctask.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call MailTask_Register( hROOT root );

// AVP Prague stamp end



cRoot* g_root;

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
			g_root = (cRoot*)hInstance;
			root = (hROOT)hInstance;
			*pERROR = errOK;
			PR_TRACE( (hInstance, prtSPAM, "trace init") );

			// register my interfaces
			if ( PR_FAIL(*pERROR=MailTask_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"MailTask\" interface"));
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



