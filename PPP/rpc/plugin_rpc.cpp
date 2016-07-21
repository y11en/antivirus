// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  31 May 2004,  15:52 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_rpc.cpp
// -------------------------------------------
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end

#include <stddef.h>
#include "../../Prague/Core/remote/pr_manager.h"

#define __PROXY_DECLARATION

#include <iface/i_taskmanager.h>
#include <iface/i_tmclient.h>
#include <iface/i_tmhost.h>
#include <iface/i_avs.h>
#include <iface/i_cryptohelper.h>
#include <iface/i_licensing.h>
#include <iface/i_report.h>
#include <iface/i_reportdb.h>
#include <iface/i_task.h>
#include <iface/i_mailtask.h>
#include <iface/i_processmonitor.h>
#include <iface/i_os.h>
#include <iface/i_objptr.h>
#include <iface/i_io.h>
#include <iface/i_qb.h>

cRoot*           g_root;
hPLUGIN          g_hParams=0;
PRRemoteManager* g_manager=NULL;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin extern and export declaration,  )

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

void RegisterRPC(hROOT root)
{
	tPROPID prop;
	if( PR_FAIL(root->RegisterCustomPropId(&prop, PR_REMOTE_MANAGER_PROP, pTYPE_PTR)) )
	{
		PR_TRACE((g_root, prtERROR, "pxstub\tPR_REMOTE_MANAGER_PROP not registered"));
		return;
	}

	g_manager = (PRRemoteManager*)root->propGetPtr(prop);
	if( !g_manager )
	{
		PR_TRACE((g_root, prtERROR, "pxstub\tPR_REMOTE_MANAGER_PROP not found"));
		return;
	}

	PR_REGISTER_PROXY(IID_TASK)
	PR_REGISTER_PROXY(IID_AVS)
	PR_REGISTER_PROXY(IID_AVSTREATS)
	PR_REGISTER_PROXY(IID_REPORT)
	PR_REGISTER_PROXY(IID_REPORTDB)
	PR_REGISTER_PROXY(IID_IO)
	PR_REGISTER_PROXY(IID_OS)
	PR_REGISTER_PROXY(IID_OBJPTR)
	PR_REGISTER_PROXY(IID_MAILTASK)
	PR_REGISTER_PROXY(IID_TASKMANAGER)
	PR_REGISTER_PROXY(IID_TM_CLIENT)
	PR_REGISTER_PROXY(IID_TMHOST)
	PR_REGISTER_PROXY(IID_LICENSING)
	PR_REGISTER_PROXY(IID_CRYPTOHELPER)
	PR_REGISTER_PROXY(IID_STRING)
	PR_REGISTER_PROXY(IID_PROCESSMONITOR)
	PR_REGISTER_PROXY(IID_QB)
}

// ---
extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
//	tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			::g_root = (cRoot*)hInstance;

			*pERROR = errOK;
			RegisterRPC( ::g_root );
			
			if(!g_hParams)
				*pERROR = ::g_root->LoadModule(&::g_hParams,"PARAMS.PPL",11,cCP_ANSI);


			//resolve  my imports
			//if ( PR_FAIL(*pERROR=CALL_Root_ResolveImportTable(::g_root,&count,import_table,PID_PARAMS)) ) {
			//	PR_TRACE(( ::g_root, prtERROR, "cannot resolve import table for PARAMS plugin" ));
			//   return cFALSE;
			//}

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

			if(PR_SUCC(((cRoot*)hInstance)->sysCheckObject((cObject*)g_hParams))) {
				g_hParams->Unload();
				g_hParams=NULL;
			}

			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



