// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  22 April 2003,  14:21 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин работы с временными файлами
// Author      -- Andy Nikishin
// File Name   -- plugin_temporaryio.h
// Additional info
//    Этот плагин создан для работы с временными файлами. При этом файлы первоначально создаются в памяти. Если размер файла превышает некое пороговое значение (в текущей реализации 16 килобайт), происходит создание файла на диске, и в последующем идет работа с файлом через бефер памяти.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include "tempfile.h"
#include "posio.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT  root;
	// tDWORD count;

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
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table_variable...,TEMPIO_ID)) ) {
			//   PR_TRACE(( root, prtERROR, "cannot resolve import table for ..." ));
			//   return cFALSE;
			//}

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., TEMPIO_ID );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=IO_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"IO\" interface"));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=PosIO_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "cannot register \"PosIO\" interface"));
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



