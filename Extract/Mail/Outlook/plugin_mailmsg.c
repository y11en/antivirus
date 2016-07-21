// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  20 December 2002,  16:22 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- plugin_mailmsg.h
// Additional info
//    Outlook message object system
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF


// AVP Prague stamp begin( Header includes,  )
#include "msg_mapi.h"
#include "msg_io.h"
#include "msg_objptr.h"
#include "msg_os.h"
#include "msg_tr.h"

#include <Prague/prague.h>
#include <Prague/iface\e_ktime.h>
// AVP Prague stamp end

hROOT g_root = NULL;

#define IMPEX_TABLE_CONTENT

IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h> 
IMPORT_TABLE_END

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS};

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pERROR = errOK;

//			if( FAILED(MAPIInitialize(&MAPIINIT)) )
//				return cFALSE;

			//resolve  my imports
			CALL_Root_ResolveImportTable(g_root,NULL,import_table,PID_MAILMSG);
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,PID_MAILMSG)) ) {
			//   PR_TRACE(( g_root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., PID_MAILMSG );

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

			if ( PR_FAIL(*pERROR=Transformer_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"Transformer\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
//			MAPIUninitialize();
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



