// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  23 October 2007,  19:29 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- plugin_updater2005.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end
#include <Prague/iface/e_ktime.h>
#include <Updater/updater.h>



#define __DECLARE_SERIALIZABLE
#include <Prague/pr_serdescriptor.h>
#define IMPEX_IMPORT_LIB
    #include <Prague/iface/e_loader.h>
    #include <Prague/iface/e_ktrace.h>
    #include <Prague/iface/e_ktime.h>
 #define IMPEX_TABLE_CONTENT
    IMPORT_TABLE(import_table)
        #include <Prague/iface/e_loader.h>
        #include <Prague/iface/e_ktrace.h>
        #include <Prague/iface/e_ktime.h>
    IMPORT_TABLE_END


// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );
extern tERROR pr_call BasesVerifier_Register( hROOT root );

// AVP Prague stamp end



extern tERROR pr_call UpdaterConfigurator_Register(hROOT);
hROOT g_root = 0;
cSerRegistrar *g_serializable_registrar_head = 0;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

extern "C" tBOOL __stdcall DllMain(tPTR hInstance, tDWORD dwReason, tERROR *error)
{
	hROOT root;
	tDWORD count = 0;
    const cSerRegistrar *registrar = 0;

	switch(dwReason)
    {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = root = (hROOT)hInstance;
			*error = errOK;
			// resolve imports
			if(PR_FAIL(*error = CALL_Root_ResolveImportTable(root, &count, import_table, PID_UPDATER)))
            {
			   PR_TRACE((root, prtERROR, "cannot resolve import table for PID_UPDATER"));
			   return cFALSE;
			}

			// register Update Task plugin
			if(PR_FAIL(*error = Task_Register(root)))
            {
				PR_TRACE((root, prtERROR, "cannot register \"Task\" interface"));
				return cFALSE;
			}

            // register Updater Configure plugin
            if(PR_FAIL(*error = UpdaterConfigurator_Register(root)))
            {
                PR_TRACE((root, prtERROR, "cannot register \"UpdaterSettingsPasser\" interface"));
                return cFALSE;
            }
			if ( PR_FAIL(*error=BasesVerifier_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"BasesVerifier\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD:
            break;
	}
	return cTRUE;
}
// AVP Prague stamp end



