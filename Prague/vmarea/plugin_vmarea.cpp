// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  14 April 2005,  18:08 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sychev
// File Name   -- plugin_vmarea.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <klsys/virtualmemory/virtualmemory.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call SimpleObject_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <Prague/plugin/p_vmarea.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <Prague/plugin/p_vmarea.h>
EXPORT_TABLE_END
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT  g_root = 0;

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
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
      //if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_VMAREA)) ) {
      //   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
      //   return cFALSE;
      //}

      //register my exports
      CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_VMAREA );

      //register my custom property ids
      //if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
      //  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
      //  return cFALSE;
      //}

      // register my interfaces
      if ( PR_FAIL(*pERROR=SimpleObject_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"SimpleObject\" interface"));
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



// AVP Prague stamp begin( Plugin export method implementation, vm_allocate )
// Parameters are:
tPTR vm_allocate( tDWORD p_aSize, tDWORD p_aProtection, tPTR p_aStartAddress )
{
  // Place your code here
  KLSysNS::Protection aProtection = KL_VIRTMEM_NONE;
  if ( p_aProtection & PROTECTION_READ )
    aProtection |= KL_VIRTMEM_READ;
  if ( p_aProtection & PROTECTION_WRITE )
    aProtection |= KL_VIRTMEM_WRITE;
  if ( p_aProtection & PROTECTION_EXECUTE )
    aProtection |= KL_VIRTMEM_EXECUTE;
  return KLSysNS::vm_allocate ( p_aSize, aProtection, p_aStartAddress );
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, vm_deallocate )
// Parameters are:
tERROR vm_deallocate( tPTR p_anAddress, tDWORD p_aSize )
{
  // Place your code here
  KLSysNS::vm_deallocate ( p_anAddress, p_aSize );
  return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, vm_protect )
// Parameters are:
//! tDWORD vm_protect( tPTR p_anAddress, tDWORD p_aSize, tDWORD p_aProtection )
tBOOL vm_protect( tPTR p_anAddress, tDWORD p_aSize, tDWORD p_aProtection )
{
  // Place your code here
  KLSysNS::Protection aProtection = KL_VIRTMEM_NONE;
  if ( p_aProtection & PROTECTION_READ )
    aProtection |= KL_VIRTMEM_READ;
  if ( p_aProtection & PROTECTION_WRITE )
    aProtection |= KL_VIRTMEM_WRITE;
  if ( p_aProtection & PROTECTION_EXECUTE )
    aProtection |= KL_VIRTMEM_EXECUTE;
  if ( KLSysNS::vm_protect ( p_anAddress, p_aSize, aProtection ) )
    return cTRUE;
  return cFALSE;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, vm_lock )
// Parameters are:
tBOOL vm_lock( tPTR p_anAddress, tDWORD p_aSize )
{
  // Place your code here
  return KLSysNS::vm_lock ( p_anAddress, p_aSize );
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, vm_unlock )
// Parameters are:
tERROR vm_unlock( tPTR p_anAddress, tDWORD p_aSize )
{
  // Place your code here
  KLSysNS::vm_unlock ( p_anAddress, p_aSize );
  return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, vm_pagesize )
// Parameters are:
tDWORD vm_pagesize()
{
  // Place your code here
  return KLSysNS::vm_pagesize ();
}
// AVP Prague stamp end



