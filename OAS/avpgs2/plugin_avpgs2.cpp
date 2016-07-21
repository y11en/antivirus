// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  27 July 2007,  13:16 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- KAV
// Sub project -- OAS
// Purpose     -- Плагин для обработки событий из klif.sys
// Author      -- Rubin
// File Name   -- plugin_avpgs2.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call AVPG2_Register( hROOT root );

// AVP Prague stamp end

#include <oas/iface/i_avpg2.h>
tPROPID propid_EventProcessID = 0;
tPROPID propid_DosDeviceName = 0;
tPROPID propid_CustomOrigin = 0;
tPROPID propid_Background = 0;
tPROPID propid_UserAccessed = 0;
tPROPID propid_UserCreated = 0;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT  g_root = NULL;

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) {
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pError = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pError=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_AVPGS2)) ) {
			//   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_AVPGS2 );

			//register my custom property ids
			//if ( PR_FAIL(*pError=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}
			*pError = CALL_Root_RegisterCustomPropId(g_root, &propid_CustomOrigin, cAVPG2_PROP_CUSTOM_ORIGIN, pTYPE_ORIG_ID );
			if (PR_SUCC(*pError) )
				*pError = CALL_Root_RegisterCustomPropId(g_root, &propid_DosDeviceName, cAVPG2_PROP_DOS_DEVICE_NAME, pTYPE_WSTRING );
			if (PR_SUCC(*pError) )
				*pError = CALL_Root_RegisterCustomPropId( g_root, &propid_EventProcessID, cAVPG2_PROP_PROCESS_ID, pTYPE_DWORD );

			CALL_Root_RegisterCustomPropId( g_root, &propid_Background, cAVPG2_PROP_BACKGROUND_PROCESSING, pTYPE_BOOL );
			CALL_Root_RegisterCustomPropId( g_root, &propid_UserAccessed, cAVPG2_PROP_USER_ACCESSED, pTYPE_PTR );
			CALL_Root_RegisterCustomPropId( g_root, &propid_UserCreated, cAVPG2_PROP_USER_CREATED, pTYPE_PTR );

			// register my interfaces
			if ( PR_FAIL(*pError=AVPG2_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"AVPG2\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



