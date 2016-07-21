// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  27 April 2007,  17:05 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Voronkov
// File Name   -- plugin_proxyserveraddressdetectorimplementation.cpp
// -------------------------------------------
// AVP Prague stamp end
#define PR_IMPEX_DEF
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end
// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call ProxyServerAddressDetector_Register( hROOT root );

// AVP Prague stamp end
#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktime.h>
#include <Prague/plugin/p_string.h>
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = 0;

tBOOL __stdcall DllMain(tPTR hInstance, tDWORD dwReason, tERROR *pError)
{
    switch( dwReason )
    {
        case DLL_PROCESS_ATTACH:
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case PRAGUE_PLUGIN_LOAD:
            g_root = (hROOT)hInstance;
            *pError = ProxyServerAddressDetector_Register(g_root);

            // register my interfaces
            if(PR_FAIL(*pError))
            {
                PR_TRACE(( g_root, prtERROR, "cannot register \"ProxyServerAddressDetector\" interface"));
                return cFALSE;
            }
            break;

        case PRAGUE_PLUGIN_UNLOAD:
            g_root = 0;
            break;
    }
    return cTRUE;
}
// AVP Prague stamp end
