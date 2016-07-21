// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  21 June 2002,  14:52 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- „тение секторов дл€ операционной системы Windows NT
// Author      -- Sobko
// File Name   -- plugin_windiskio.h
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include "objptr.h"
#include "io.h"

// AVP Prague stamp end

#define  IMPEX_EXPORT_LIB
#include "plugin/p_windiskio.h"

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE(export_table)
#include "plugin/p_windiskio.h"
EXPORT_TABLE_END

hROOT g_root;
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) 
{
	tERROR error;
	switch( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH :
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		g_root = (hROOT) hInstance;
		// register my interfaces
		if ( PR_FAIL(error = ObjPtr_Register(g_root)) )
			return cFALSE;
		if ( PR_FAIL(error = IO_Register(g_root)) )
			return cFALSE;

		CALL_Root_RegisterExportTable(g_root, NULL, export_table, PID_WINDISKIO);
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



