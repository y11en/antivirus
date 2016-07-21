// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  27 August 2002,  16:12 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- NetWare native file IO
// Author      -- petrovitch
// File Name   -- plugin_nw_nfio.h
// Additional info
//    File input/output for Novell NetWare
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#ifdef __MWERKS__
	#include <dirent.h>
	#include <sys\stat.h>
#endif

#include <prague.h>
#include "nfio_os.h"
#include "nfio_objptr.h"
#include "nfio_io.h"
#include "plugin_nfio.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;
//	tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_NW_NFIO)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_NW_NFIO );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=OS_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"OS\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=ObjPtr_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"ObjPtr\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=IO_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"IO\" interface"));
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



