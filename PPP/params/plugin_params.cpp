// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Saturday,  04 October 2003,  18:15 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_params.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end

#if !defined(_CRTIMP)
	#define _CRTIMP 
#endif

#define __DECLARE_SERIALIZABLE
#define __DECLARE_SERIALIZABLE_BASE

#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_ser_v1_skip.h>
#include <PPP/plugin/p_params.h>
#include <AntiPhishing/plugin/p_antiphishingtask.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/e_loader.h>

#include <structs/s_avp3info.h>
#include <structs/s_bl.h>
#include <structs/s_avs.h>
#include <structs/s_qb.h>
#include <structs/s_scaner.h>
#include <structs/s_oas.h>
#include <structs/s_scheduler.h>
#include <structs/s_gui.h>
#include <structs/s_taskmanager.h>
#include <structs/s_report.h>
#include <NetDetails/structs/s_netdetails.h>

#include <structs/s_licensing.h>
#include <structs/s_licmgr.h>

#include <Updater/transport.h>
#include <Updater/updater.h>

#if defined (_WIN32)
#include <structs/s_mc.h>
#include <structs/s_mc_trafficmonitor.h>
#include <structs/s_mailwasher.h>
#include <structs/s_mc_oe.h>
#include <structs/s_as.h>
#include <structs/s_ids.h>
#include <structs/s_antispam.h>
#include <structs/s_mcou_antispam.h>
#include <structs/s_spamtest.h>
#include <structs/s_httpscan.h>
#include <structs/s_httpprotocoller.h>
#include <Hips/structs/s_hips.h>
#include <NetWatch/structs/s_NetWatch.h>

#include <structs/s_ipresolver.h>
#include <structs/s_antidial.h>
#include <structs/s_antiphishing.h>
#include <structs/s_pdm2rt.h>
#include <structs/s_urlflt.h>
#include <OAS/structs/s_oas.h>
#include <structs/s_avzscan.h>
#include <structs/s_statistics.h>

#endif // __unix__

IMPORT_TABLE(import_table)
	#define IMPEX_TABLE_CONTENT
	#include <iface/e_ktrace.h>
	#include <iface/e_loader.h>
IMPORT_TABLE_END

cSerRegistrar* g_serializable_registrar_head;

cSkipFieldAfterV1_Serializer g_SkipFieldAfterV1_Serializer;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin extern and export declaration,  )

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

cRoot* g_root;

// ---
extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	const cSerRegistrar* tmp;
	tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			::g_root = (cRoot*)hInstance;

			*pERROR = errOK;
			tmp = g_serializable_registrar_head;
			while( tmp ) {
				::g_root->RegisterSerializableDescriptor( tmp->m_dsc );
				tmp = tmp->m_next;
			}

			//resolve  my imports
			if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(::g_root,&count,import_table,PID_PARAMS)) ) {
				PR_TRACE(( ::g_root, prtERROR, "cannot resolve import table for PARAMS plugin" ));
			   return cFALSE;
			}

			//register my exports
			//CALL_Root_RegisterExportTable( ::g_root, &count, export_table_..., PID_PARAMS );

			//register my custom property ids
			//if ( PR_FAIL(*pERROR=CALL_Root_RegisterCustomPropId(::g_root,&some_propid_variable,"some_property_name",pTYPE_DWORD)) ) {
			//  PR_TRACE(( ::g_root, prtERROR, "cannot register custom property ..." ));
			//  return cFALSE;
			//}

			// register my interfaces
			//if ( PR_FAIL(*pERROR=SomeInterface_Register(::g_root)) ){
			//  PR_TRACE(( ::g_root, prtERROR, "cannot register \"SomeInterface\" interface"));
			//  return cFALSE;
			//}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			tmp = g_serializable_registrar_head;
			while( tmp ) {
				::g_root->UnregisterSerializableDescriptor( tmp->m_dsc );
				tmp = tmp->m_next;
			}
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



