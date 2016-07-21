// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  29 October 2007,  15:24 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- plugin_mdb.c
// Additional info
//    Данный плагин предназначен для работы со следующими объектами Outlook:
//    - почтовая система для указанного Profile, если не указан, то успользуется текущий.
//    - база сообщений (pst файл)
//    - сообщение (Outlook Message)
//
//    Плагин реализован на основе MAPI интерфейса.
//
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

#include "mdb_mapi.h"

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end

#include <Prague/iface/e_ktime.h>

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call OS_Register( hROOT root );
extern tERROR pr_call ObjPtr_Register( hROOT root );
extern tERROR pr_call IO_Register( hROOT root );

// AVP Prague stamp end



hROOT g_root = NULL;

IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h> 
IMPORT_TABLE_END

tBOOL g_is_load_mapi = cFALSE;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH :
			SessionThreadAttach();
			break;
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pERROR = errOK;

			if( PR_FAIL(*pERROR=SessionInitialize()) )
				return cFALSE;

			//resolve  my imports
			CALL_Root_ResolveImportTable(g_root,NULL,import_table,PID_MDB);

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_MDBOS );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=OS_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"OS\" interface"));
				return cFALSE;
			}

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
			SessionDeinitialize();
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



