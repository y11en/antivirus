// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  03 July 2002,  15:36 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Триада интерфейсов для енумерации модулей загруженных в процесс
// Author      -- Sobko
// File Name   -- plugin_memmodscan.h
// Additional info
//    Реализация для операционных систем Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include "os.h"
#include "objptr.h"
#include "io.h"
// AVP Prague stamp end

#include <windows.h>
#include <stuff/MemManag.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) 
{
	// tDWORD count;

	switch( dwReason ) 
	{
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			{
				OSVERSIONINFO OsVersionInfo;
				ZeroMemory(&OsVersionInfo, sizeof(OSVERSIONINFO));
				OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

				if (GetVersionEx(&OsVersionInfo) == FALSE)
					return cFALSE;

				if (InitWinMemManagement(OsVersionInfo.dwPlatformId, _MM__Manage_Alloc, _MM__Manage_Free) == FALSE)
					return cFALSE;
			}
			
			g_root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,PID_MEMMODSCAN)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_MEMMODSCAN );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=OS_Register(g_root)) ) 
			{
				PR_TRACE(( g_root, prtERROR, "cannot register \"OS\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=ObjPtr_Register(g_root)) ) 
			{
				PR_TRACE(( g_root, prtERROR, "cannot register \"ObjPtr\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=IO_Register(g_root)) ) 
			{
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
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



