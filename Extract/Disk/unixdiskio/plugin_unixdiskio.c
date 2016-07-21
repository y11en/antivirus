// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  20 May 2005,  18:12 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Low level disk I/O for Unix-like systems
// Author      -- Sychev
// File Name   -- plugin_unixdiskio.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include "diskio.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call ObjPtr_Register( hROOT root );
extern tERROR pr_call IO_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <plugin/p_unixdiskio.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <plugin/p_unixdiskio.h>
EXPORT_TABLE_END
// AVP Prague stamp end

hROOT g_root = 0;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
  tDWORD count;

  switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;

    case PRAGUE_PLUGIN_LOAD :
      g_root = (hROOT)hInstance;
      *pERROR = errOK;
      //resolve  my imports
      //if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_UNIXDISKIO)) ) {
      //   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
      //   return cFALSE;
      //}

      //register my exports
      CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_UNIXDISKIO );

      //register my custom property ids
      //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
      //  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
      //  return cFALSE;
      //}

      // register my interfaces
      if ( PR_FAIL(*pERROR=ObjPtr_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"ObjPtr\" interface"));
        return cFALSE;
      }

      if ( PR_FAIL(*pERROR=IO_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"IO\" interface"));
        return cFALSE;
      }
      break;

    case PRAGUE_PLUGIN_UNLOAD :
      // free system resources
      // unregister my custom property ids -- you can drop it, kernel do it by itself
      // release    my imports		         -- you can drop it, kernel do it by itself
      // unregister my exports		         -- you can drop it, kernel do it by itself
      // unregister my interfaces          -- you can drop it, kernel do it by itself
      g_root = 0;
      break;
  }
  return cTRUE;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, exDiskExQueryLockStatus )
// Parameters are:
tBOOL exDiskExQueryLockStatus( tCHAR p_DrvName, tBOOL* p_pbLocked ) 
{

  // Place your code here
/*   enum DeviceModes aDeviceMode = getDeviceMode ( p_DrvName ); */
/*   if ( p_pbLocked ) */
/*     *p_pbLocked = aDeviceMode == dmReadOnly; */
/*   return aDeviceMode != dmUnknown; */
  return 0;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, exDiskExLogical_Next )
// Parameters are:
tERROR exDiskExLogical_Next( hSTRING p_hStrLogical ) 
{

  // Place your code here

  return 0;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, exDiskExLogical_GetDriveType )
// Parameters are:
tERROR exDiskExLogical_GetDriveType( hSTRING p_hStrLogical, tDWORD* p_pDriveType ) 
{

  // Place your code here

  return 0;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, exDiskExIsPartition )
// Parameters are:
tERROR exDiskExIsPartition( tCHAR p_DrvName, tBOOL* p_pbIsPartition ) 
{

  // Place your code here

  return 0;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, exDiskExQueryDriveID )
// Parameters are:
tERROR exDiskExQueryDriveID( tSTRING p_DrvName, tDWORD* p_DriveID, tDWORD p_DriveIdLen ) 
{

  // Place your code here

  return 0;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, exDiskExIsSubst )
// Parameters are:
tERROR exDiskExIsSubst( hSTRING p_hStrLogical ) 
{

  // Place your code here

  return 0;
}
// AVP Prague stamp end



