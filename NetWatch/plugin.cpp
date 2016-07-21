#define PR_IMPEX_DEF
#include <Prague/pr_cpp.h>
#include "NetWatcher.h"


#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktrace.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
    #include <Prague/iface/e_ktrace.h>
IMPORT_TABLE_END
hROOT g_root;
PR_MAKE_TRACE_FUNC;
// ---
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) {
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			PR_TRACE(( g_root, prtERROR, "attach \"SampleI\" interface"));
			//printf("test attach\n");
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pError = errOK;

			PR_TRACE(( g_root, prtERROR, "try to register \"SampleI\" interface"));
			// register my interfaces
			if ( PR_FAIL(*pError=IDL_NAME(cNetWatcher)::Register (g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"SampleI\" interface"));
				return cFALSE;
			}
			g_root->ResolveImportTable( 0, import_table, IDL_NAME(cNetWatcher)::PID );
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
