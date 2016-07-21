// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  26 March 2004,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- IWorms Generic Detection
// Author      -- Pavlushchik
// File Name   -- plugin_iwgen.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include <windows.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Converter_Register( hROOT g_root );

// AVP Prague stamp end

hROOT g_root = NULL;


// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

DWORD (WINAPI *g_pGetModuleFileNameExW)(HANDLE hProcess,  HMODULE hModule,  LPWSTR lpFilename,  DWORD nSize) = NULL;
HINSTANCE g_hPsapiLib = NULL;

BOOL ResolveDynamicLinking()
{
	OSVERSIONINFO osvi;

	memset(&osvi, 0, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (!GetVersionEx(&osvi))
		return FALSE;
	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
		return FALSE;
	g_hPsapiLib = LoadLibrary("psapi.dll");
	if (g_hPsapiLib == NULL)
		return FALSE;
	*(void**)&g_pGetModuleFileNameExW = GetProcAddress(g_hPsapiLib, "GetModuleFileNameExW");
	if (g_pGetModuleFileNameExW == NULL)
	{
		FreeLibrary(g_hPsapiLib);
		g_hPsapiLib = NULL;
		return FALSE;
	}
	return TRUE;
}

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH :
			break;
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			if(ResolveDynamicLinking())
				*pERROR = errOK;
			else
				*pERROR = errOBJECT_CANNOT_BE_INITIALIZED;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_IWGEN)) ) {
			//   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_IWGEN );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=Converter_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"Converter\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			if (g_hPsapiLib)
				FreeLibrary(g_hPsapiLib);
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



