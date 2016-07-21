// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  26 December 2006,  15:17 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- plugin_licensing60.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end



#include <plugin/p_licensing60.h>

#define IMPEX_IMPORT_LIB
    #include <iface/e_ktime.h>
    #include <iface/e_loader.h>
    #include <plugin/p_win32_nfio.h>

#define IMPEX_TABLE_CONTENT
    IMPORT_TABLE(import_table)
        #include <iface/e_ktime.h>
        #include <iface/e_loader.h>
        #include <plugin/p_win32_nfio.h>
    IMPORT_TABLE_END

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Licensing_Register( hROOT root );

// AVP Prague stamp end



hROOT g_root = NULL ;


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
    hROOT  root;
    tDWORD count;

    switch( dwReason ) {
        case DLL_PROCESS_ATTACH:
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH :
        case DLL_THREAD_DETACH :
            break;

        case PRAGUE_PLUGIN_LOAD :
            g_root = root = (hROOT)hInstance;
            *pERROR = errOK;
            //resolve  my imports
            if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table,PID_LICENSING60)) ) {
               PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
               return cFALSE;
            }

            //register my exports
            //CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_LICENSING60 );

            //register my custom property ids
            //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
            //  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
            //  return cFALSE;
            //}

            // register my interfaces
            if ( PR_FAIL(*pERROR=Licensing_Register(root)) ) {
                PR_TRACE(( root, prtERROR, "cannot register \"Licensing\" interface"));
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



