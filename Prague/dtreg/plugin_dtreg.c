// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  07 August 2002,  17:18 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- AVP manager
// Sub project -- DTReg
// Purpose     -- secure registry unterface based on DataTree technology
// Author      -- petrovitch
// File Name   -- plugin_dtreg.h
// AVP Prague stamp end


#define PR_IMPEX_DEF

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include "dtreg.h"
// AVP Prague stamp end

#include <Prague/plugin/p_dtreg.h>
#include <Prague/iface/i_root.h>
#include <Serialize/KLDTSer.h>

#define  IMPEX_IMPORT_LIB
#  include <Prague/plugin/p_string.h>
#  include <Prague/iface/e_ktime.h>
#  include <Prague/iface/e_ktrace.h>
#  include <Prague/iface/e_loader.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table_string)
	#include <Prague/plugin/p_string.h>
IMPORT_TABLE_END

IMPORT_TABLE(import_table)
  #include <Prague/iface/e_ktime.h>
  #include <Prague/iface/e_ktrace.h>
  #include <Prague/iface/e_loader.h>
IMPORT_TABLE_END


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root;

// ---
tPTR pr_call alloc_mem( tSIZE_T size ) {
	tPTR ptr;
	if ( PR_SUCC(CALL_SYS_ObjHeapAlloc(g_root,&ptr,(tDWORD)size)) )
		return ptr;
	return 0;
}



// ---
tVOID pr_call free_mem( tPTR ptr ) {
  CALL_SYS_ObjHeapFree( g_root, ptr );
}



// ---
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
  // tDWORD count;

  switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;

    case PRAGUE_PLUGIN_LOAD :
      g_root = (hROOT)hInstance;
      *pERROR = errOK;
      if ( !KLDT_Init_Library(alloc_mem,free_mem) ) {
        *pERROR = errMODULE_CANNOT_BE_INITIALIZED;
        return cFALSE;
      }

      CALL_Root_ResolveImportTable( g_root, 0, import_table, PID_DTREG );
      CALL_Root_ResolveImportTable( g_root, 0, import_table_string, PID_DTREG );

      //register my exports
      //CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_DTREG );

      //register my custom property ids
      //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
      //  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
      //  return cFALSE;
      //}

      // register my interfaces
      if ( PR_FAIL(*pERROR=Registry_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"Registry\" interface"));
        return cFALSE;
      }
      break;

    case PRAGUE_PLUGIN_UNLOAD :
      KLDT_Deinit_Library( cTRUE );
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



