// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  30 July 2002,  10:02 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- plugin_win32_reg.h
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include "registry.h"
#include "reg_comm.h"
// AVP Prague stamp end

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>
#include <Prague/iface/e_loader.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/plugin/p_string.h>
	#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT    g_root;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;
	tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = root = (hROOT)hInstance;
			*pERROR = errOK;

			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table,PID_WIN32_REG)) ) {
				PR_TRACE(( root, prtERROR, "cannot resolve import table for WIN32_REG" ));
				return cFALSE;
			}


			pZwQueryKey = (tZwQueryKey)GetProcAddress( GetModuleHandle("NTDLL.DLL"), "ZwQueryKey" );

			bWOW64 = FALSE;
			bWOWFlagsAvailable = FALSE;
#ifdef _WIN32
			{
				typedef BOOL WINAPI fnISWOW64PROCESS(HANDLE hProcess, PBOOL Wow64Process);
				fnISWOW64PROCESS * fnIsWow64Process = (fnISWOW64PROCESS *)GetProcAddress(GetModuleHandle("kernel32.dll"), "IsWow64Process");
				
				if( fnIsWow64Process && !fnIsWow64Process(GetCurrentProcess(), &bWOW64) )
					bWOW64 = FALSE;
				bWOWFlagsAvailable = PrGetOsVersion && PrGetOsVersion() >= OSVERSION_XP;
			}
#endif // _WIN32
      
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_WIN32_REG)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_WIN32_REG );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=Registry_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"Registry\" interface"));
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



