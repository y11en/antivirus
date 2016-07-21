// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  31 August 2002,  14:12 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- plugin for NTFS stream operation
// Author      -- Doukhvalow
// File Name   -- plugin_ntfsstream.h
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include "stream_optr.h"
#include "stream_util.h"
// AVP Prague stamp end


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
  hROOT  g_root;
  // tDWORD count;

  switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;

    case PRAGUE_PLUGIN_LOAD :
      if ( 0x80000000l & GetVersion() ) {
        *pERROR = errMODULE_CANNOT_BE_INITIALIZED;
        return cFALSE;
      }
      
      g_ntdll = LoadLibrary( "ntdll.dll" );
      if ( !g_ntdll ) {
        *pERROR = errMODULE_CANNOT_BE_INITIALIZED;
        return cFALSE;
      }
      
      g_qif = (NQIF)GetProcAddress( g_ntdll, "NtQueryInformationFile" );
      if ( !g_qif ) {
        *pERROR = errMODULE_CANNOT_BE_INITIALIZED;
        return cFALSE;
      }

      g_root = (hROOT)hInstance;
      *pERROR = errOK;

      //resolve  my imports
      //if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_NTFSSTREAM)) ) {
      //   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
      //   return cFALSE;
      //}

      //register my exports
      //CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_NTFSSTREAM );

      // register my interfaces
      if ( PR_FAIL(*pERROR=ObjPtr_Register(g_root)) ) {
        PR_TRACE(( g_root, prtERROR, "cannot register \"ObjPtr\" interface"));
        return cFALSE;
      }
      CALL_Root_RegisterCustomPropId( g_root, &g_pid_this_is_the_stream, "native file io object is a stream really", pTYPE_BOOL );
      CALL_Root_RegisterCustomPropId( g_root, &g_pid_streams,            "native file io object streams", pTYPE_BINARY );
      break;

    case PRAGUE_PLUGIN_UNLOAD :
      // free system resources
      // unregister my custom property ids -- you can drop it, kernel do it by itself
      // release    my imports		         -- you can drop it, kernel do it by itself
      // unregister my exports		         -- you can drop it, kernel do it by itself
      // unregister my interfaces          -- you can drop it, kernel do it by itself
      if ( g_ntdll )
        FreeLibrary( g_ntdll );
	  g_root = NULL;
      break;
  }
  return cTRUE;
}
// AVP Prague stamp end



