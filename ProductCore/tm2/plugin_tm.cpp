// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 March 2006,  11:56 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- plugin_tm.cpp
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
// AVP Prague stamp end

#include <pr_serializable.h>
#include <pr_serdescriptor.h>

#include <iface/e_ktime.h>
#include <iface/e_ktrace.h>
#include <iface/e_loader.h>
#include <plugin/p_tm.h>
#include "tm_profile.h"

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <iface/e_ktime.h>
	#include <iface/e_loader.h>
	#include <iface/e_ktrace.h>
	#include <plugin/p_win32_nfio.h>
	#include <plugin/p_string.h>
IMPORT_TABLE_END

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call TaskManager_Register( hROOT root );
extern tERROR pr_call TmHost_Register( hROOT root );

// AVP Prague stamp end

cRoot*         g_root;
cSerRegistrar* g_serializable_registrar_head;

PR_NEW_DECLARE;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

extern "C" void __cdecl startup();

// ---
extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	tDWORD count;
	const cSerRegistrar* tmp;
	
	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (cRoot*)hInstance;
			*pERROR = errOK;

			tmp = ::g_serializable_registrar_head;
			while( tmp ) {
				::g_root->RegisterSerializableDescriptor( tmp->m_dsc );
				tmp = tmp->m_next;
			}
			
			//resolve  my imports
			if ( PR_FAIL(*pERROR=g_root->ResolveImportTable(&count,import_table,PID_TM)) ) {
				PR_TRACE(( g_root, prtERROR, "tm\tcannot resolve import table for BL" ));
				return cFALSE;
			}

			if ( PR_FAIL(*pERROR=TaskManager_Register((hROOT)g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "tm\tcannot register \"Task\" interface"));
				return cFALSE;
			}
			
			if ( PR_FAIL(*pERROR=TmHost_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"TmHost\" interface"));
				return cFALSE;
			}
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

#undef  __pr_serializable_h
#define __DECLARE_SERIALIZABLE

#include <pr_serializable.h>
#include <pr_serdescriptor.h>

IMPLEMENT_SERIALIZABLE_BEGIN( cTmTaskType, 0 )
	SER_KEY_VALUE( m_sType, "TaskType" )
	SER_VALUE( m_nSerIdSettings,    tid_DWORD, "idSettings" )
	SER_VALUE( m_nSerIdStatistics,  tid_DWORD, "idStatistics" )
	SER_VECTOR( m_aReports, cTaskReportDsc::eIID, "reports" )
	SER_VALUE_M( iid,               tid_DWORD )
	SER_VALUE_M( pid,               tid_DWORD )
	SER_VALUE_M( serviceId,         tid_DWORD )
IMPLEMENT_SERIALIZABLE_END()

IMPLEMENT_SERIALIZABLE_BEGIN( cTmProfile, 0 )
	SER_BASE( cProfileEx, 0 )
	SER_VALUE( m_bInstalled,        tid_BOOL,  "installed" )
	SER_VALUE( m_dwReleaseTimeout,  tid_DWORD, "ReleaseTimeout" )
	SER_VALUE( m_timeLastSchedRun,  tid_DWORD, "LastSchedRun" )
	SER_VALUE_M( iid,               tid_DWORD )
	SER_VALUE_M( pid,               tid_DWORD )
	SER_VALUE_M( runInSafeMode,     tid_BOOL )
IMPLEMENT_SERIALIZABLE_END();

IMPLEMENT_SERIALIZABLE_BEGIN( cScheduleParams, 0 )
	SER_VALUE( m_sName,             tid_STRING_OBJ, "name" )
	SER_VALUE( m_bWaitForInet,      tid_BOOL,       "WaitForInet" )
	SER_VALUE( m_state,             tid_DWORD,      "state" )
	SER_VALUE( m_nFlags,            tid_DWORD,      "flags" )
	SER_VALUE_PTR( m_info,          cSerializable::eIID, "info" )
IMPLEMENT_SERIALIZABLE_END()
