// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  19 November 2004,  17:28 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- plugin_ichstrms.c
// -------------------------------------------
// AVP Prague stamp end


#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call IChecker_Register( hROOT root );

// AVP Prague stamp end


#include <windows.h>
#include <iface/e_loader.h>
#include <iface/e_ktime.h> 

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <iface/e_loader.h>
#include <iface/e_ktime.h> 
IMPORT_TABLE_END


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

OSVERSIONINFO os;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pERROR = errOK;
			
			os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx (&os);
//???			if(	os.dwPlatformId != VER_PLATFORM_WIN32_NT )
//				return cFALSE;
			
//			if (!InitSfcApi())
//			{
//				PR_TRACE((g_root, prtERROR, "ICheckerStreams failed to load SFC"));
//			}

			//resolve  my imports
//			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,NULL,import_table,PID_ICHSTRMS)) ) {
//			   PR_TRACE(( g_root, prtERROR, "Cannot resolve import table for iCheckerStreams" ));
//			   return cFALSE;
//			}

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_ICHSTRMS );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=IChecker_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"IChecker\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			
//			DoneSfcApi();
			g_root = NULL;
			
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



