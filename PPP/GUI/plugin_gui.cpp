// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  16 October 2003,  15:33 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_gui.cpp
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

#include "StdAfx.h"
#include "ComAvpGui.h"

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end

#define IMPEX_IMPORT_LIB
#  include <iface/e_ktrace.h>
#  include <iface/e_loader.h>
#  include <iface/e_ktime.h>
#  include <plugin/p_win32_nfio.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <iface/e_ktrace.h>
	#include <iface/e_loader.h>
	#include <iface/e_ktime.h>
	#include <plugin/p_win32_nfio.h>
IMPORT_TABLE_END

cSerRegistrar * g_serializable_registrar_head = NULL;


// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Task_Register( hROOT root );
// AVP Prague stamp end

hROOT g_root;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

HINSTANCE g_hInstance = NULL;

extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	hROOT                root;
	tDWORD               count;
	const cSerRegistrar* tmp;

	switch( dwReason )
	{
		case DLL_PROCESS_ATTACH:
#if defined (_WIN32)
			GUIControllerInit(hInstance);
#endif // _WIN32
			break;
		case DLL_PROCESS_DETACH:
#if defined (_WIN32)
			GUIControllerDone();
#endif // _WIN32
			break;
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = root = (hROOT)hInstance;
			*pERROR = errOK;
			//resolve  my imports
			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(root,&count,import_table,PID_GUI)) ) {
				PR_TRACE(( root, prtERROR, "gui\tcannot resolve import table for GUI" ));
				return cFALSE;
			}

			for(tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
				::g_root->RegisterSerializableDescriptor(tmp->m_dsc);

			//register my exports
			//CALL_Root_RegisterExportTable( root, &count, export_table_..., PID_GUI );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( root, prtERROR, "gui\tcannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			if ( PR_FAIL(*pERROR=Task_Register(root)) ) {
				PR_TRACE(( root, prtERROR, "gui\tcannot register \"GUI\" interface"));
				return cFALSE;
			}
#if defined (_WIN32)			
			GUIControllerRegister();
#endif // _WIN32
			break;

		case PRAGUE_PLUGIN_UNLOAD :
#if defined (_WIN32)			
			GUIControllerUnregister();
#endif // _WIN32			
			for(tmp = g_serializable_registrar_head; tmp; tmp = tmp->m_next)
				::g_root->UnregisterSerializableDescriptor(tmp->m_dsc);

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
