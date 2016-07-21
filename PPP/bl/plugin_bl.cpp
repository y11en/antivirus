// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  02 December 2005,  10:59 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_bl.cpp
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end

#include <pr_cpp.h>
#include <plugin/p_bl.h>


// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call ProductLogic_Register( hROOT root );

// AVP Prague stamp end

#define IMPEX_IMPORT_LIB
#include <plugin/p_wmihlpr.h>
#include <iface/e_loader.h>
#include <iface/e_ktime.h>
#include <iface/e_ktrace.h>
#include <plugin/p_string.h>
#include <plugin/p_ahfw.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <plugin/p_wmihlpr.h>
	#include <iface/e_loader.h>
	#include <iface/e_ktime.h>
    #include <iface/e_ktrace.h>
IMPORT_TABLE_END



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;
tPTR  g_hModule;

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

  switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
		g_hModule = hInstance;
		break;

    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;

    case PRAGUE_PLUGIN_LOAD :
      g_root = (hROOT)hInstance;
      *pERROR = errOK;
      //resolve  my imports
      g_root->ResolveImportTable( 0, import_table, PID_BL );

      //register my exports
      //CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_BL );

      //register my custom property ids
      //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
      //  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
      //  return cFALSE;
      //}

      // register my interfaces
      if ( PR_FAIL(*pERROR=ProductLogic_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"ProductLogic\" interface"));
        return cFALSE;
      }
      break;

    case PRAGUE_PLUGIN_UNLOAD :
      // free system resources
      // unregister my custom property ids -- you can drop it, kernel do it by itself
      // release    my imports             -- you can drop it, kernel do it by itself
      // unregister my exports             -- you can drop it, kernel do it by itself
      // unregister my interfaces          -- you can drop it, kernel do it by itself
	  g_root = NULL;
      break;
  }
  return cTRUE;
}
// AVP Prague stamp end

