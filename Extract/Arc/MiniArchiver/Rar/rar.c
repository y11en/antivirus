// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  19 December 2002,  15:38 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- RAR Archiver
// Sub project -- rar
// Purpose     -- RAR Archives processor
// Author      -- Dmitry Glavatskikh
// File Name   -- rar.c
// Additional info
//    RAR Archives processor. Works together with UniArc
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include "main.h"
// AVP Prague stamp end

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
#include <stdarg.h>

hROOT  g_root = 0;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR )
{
  switch( dwReason )
  {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;
    case PRAGUE_PLUGIN_LOAD : 
      g_root = (hROOT)(hInstance);
      *pERROR = (errOK);

      //resolve  my imports
      *pERROR = CALL_Root_ResolveImportTable(g_root,NULL,import_table,PID_RAR);
      if ( PR_FAIL(*pERROR) )
      {
        PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
        return(cFALSE);
      }

      // register my interfaces
      if ( PR_FAIL(*pERROR=RAR_Register(g_root)) )
      {
        PR_TRACE(( g_root, prtERROR, "cannot register \"RAR-MiniArchiver\" interface"));
        return cFALSE;
      }
      break;
    case PRAGUE_PLUGIN_UNLOAD :
      break;
  }

  return cTRUE;
}
// AVP Prague stamp end

