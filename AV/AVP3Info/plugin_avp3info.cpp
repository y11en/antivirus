// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  07 October 2004,  12:40 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- plugin_avp3info.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end

#include <AV/plugin/p_avp3info.h>


#define IMPEX_IMPORT_LIB
#  include <Prague/plugin/p_win32_nfio.h>
#  include <Prague/iface/e_ktime.h>
#  include <Prague/plugin/p_string.h>

#define IMPEX_TABLE_CONTENT
   IMPORT_TABLE(import_table)
      #include <Prague/plugin/p_win32_nfio.h>
      #include <Prague/iface/e_ktime.h>
   IMPORT_TABLE_END

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call AVP3Info_Register( hROOT root );

// AVP Prague stamp end

PR_NEW_DECLARE ;

hROOT g_root = NULL ;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
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
            if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table,PID_AVP3INFO)) ) {
               PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
               return cFALSE;
            }

            //register my exports
            //CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_AVP3INFO );

            //register my custom property ids
            //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
            //  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
            //  return cFALSE;
            //}

            // register my interfaces
            if ( PR_FAIL(*pERROR=AVP3Info_Register(root)) ) {
                PR_TRACE(( root, prtERROR, "cannot register \"AVP3Info\" interface"));
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



