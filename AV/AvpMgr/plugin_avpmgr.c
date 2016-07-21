// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  11 February 2005,  15:57 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- Менеджер антивирусной обработки (Anti-Virus Processing Manager).//eng:Anti-Virus Processing Manager.
// Author      -- Graf
// File Name   -- plugin_avpmgr.c
// Additional info
//    Модуль AvpMgr обеспечиает антивирусную функциональность верхнего уровня для приложений.
//    //eng:
//    AvpMgr plugin - Anti-Virus Processing Manager - represents AV Engine to Applications.
//
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



#include <string.h>

#define PLUGIN_STATIC_DATA
#include "static.h"
#define  IMPEX_IMPORT_LIB
#include <Extract/plugin/p_windiskio.h> 

#include <Prague/pr_oid.h>
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/e_ktrace.h> 
#include <Prague/iface/e_ktime.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <ProductCore/plugin/p_ichecker2.h>
#include <AV/plugin/p_avlib.h> 

// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call Engine_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <AV/plugin/p_avpmgr.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( export_table )
#include <AV/plugin/p_avpmgr.h>
EXPORT_TABLE_END
// AVP Prague stamp end



#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/pr_oid.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h> 
#include <Prague/iface/e_loader.h> 
#include <Prague/plugin/p_win32_nfio.h>

#include <AV/plugin/p_avlib.h> 
#include <Extract/plugin/p_windiskio.h> 
#include <ProductCore/plugin/p_ichecker2.h>

#include "../../Extract/Arc/MiniArchiver/Rar/rar.h"
IMPORT_TABLE_END

//==============================================================
#ifdef KLAVPMGR
#include <AV/plugin/p_klavcore.h>
#endif // KLAVPMGR
//==============================================================

#include "../../Extract/Arc/UniArc/os.h"
#include "../AvLib/avlib.h"

hPLUGIN hWindiskIO = 0;
hROOT g_root = NULL;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	tDWORD count;
	switch( dwReason ) {
		
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH :
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		{
			tERROR error=errOK;
			g_root=(hROOT)hInstance;
			if(PR_SUCC(error)) error=CALL_Root_RegisterExportTable( g_root, &count, export_table, PID_AVPMGR);
			if(PR_SUCC(error)) error=CALL_Root_ResolveImportTable( g_root,0,import_table,PID_AVPMGR);
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_virtual_name, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_readonly_error, npENGINE_OBJECT_READONLY_tERROR, pTYPE_ERROR| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_set_write_access_error, npENGINE_OBJECT_SET_WRITE_ACCESS_tERROR, pTYPE_ERROR );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_readonly_object, npENGINE_OBJECT_READONLY_hOBJECT, pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_integral_parent_io, npENGINE_INTEGRAL_PARENT_IO_hOBJECT, pTYPE_OBJECT );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_parent_io, "propid_object_parent_io", pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_executable_parent, npENGINE_OBJECT_EXECUTABLE_PARENT_IO_hOBJECT, pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_hash_temp_list, "propid_hash_temp_list", pTYPE_OBJECT );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_state, npENGINE_OBJECT_DETECT_STATE_tDWORD, pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_counter, npENGINE_OBJECT_DETECT_COUNTER_tDWORD, pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_name, "propid_detect_name", pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_type, "propid_detect_type", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_certanity, "propid_detect_certanity", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_disinfectability, "propid_detect_disinfectability", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_danger, "propid_detect_danger", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_behaviour, "propid_detect_behaviour", pTYPE_DWORD );

			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_os_offset_list, "propid_os_offset_list", pTYPE_OBJECT );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_os_offset, "propid_os_offset", pTYPE_QWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_hash_container, "propid_hash_container", pTYPE_OBJECT| pCUSTOM_HERITABLE  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_postprocess_list, "propid_postprocess_list", pTYPE_OBJECT  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_os_hash, "propid_os_hash", pTYPE_QWORD  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_processing_error, "propid_processing_error", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_handled_error, "propid_handled_error", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_main, "propid_object_main", pTYPE_OBJECT| pCUSTOM_HERITABLE  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_session, npENGINE_OBJECT_SESSION_hOBJECT, pTYPE_OBJECT| pCUSTOM_HERITABLE  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_uniarc_mini_pid, cUNIARC_PROPERTY, pTYPE_PID  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_action_class_mask, npENGINE_OBJECT_PARAM_ACTION_CLASS_MASK_tDWORD, pTYPE_DWORD| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME, npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME, pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_npENGINE_OBJECT_TRANSFORMER_NAME, npENGINE_OBJECT_TRANSFORMER_NAME, pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_password_list, "propid_password_list", pTYPE_OBJECT );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_password_node, "propid_password_node", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_action_class_mask_current, "propid_action_class_mask_current", pTYPE_DWORD| pCUSTOM_HERITABLE  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_skip, npENGINE_OBJECT_SKIP_THIS_ONE_tBOOL, pTYPE_BOOL );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_creator_engine_pid, "propid_creator_engine_pid", pTYPE_PID );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_reopen_data_io, "propid_reopen_data_io", pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_reopen_data_offset, "propid_reopen_data_offset", pTYPE_QWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_reopen_user_data, "propid_reopen_user_data", pTYPE_DWORD| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_infected_parent, "propid_object_infected_parent", pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_auto_password_bool, "propid_auto_password_bool", pTYPE_BOOL| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_boot_BytesPerSector, "propid_boot_BytesPerSector", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_os_passed, "propid_object_os_passed", pTYPE_BOOL );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_password_str, "propid_password_str", pTYPE_STRING| pCUSTOM_HERITABLE );
//==============================================================
#ifdef KLAVPMGR
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_klavpmgr_action_name, npKLAVPMGR_ACTION_NAME, pTYPE_STRING );
#endif // KLAVPMGR
//==============================================================
						
			if(PR_SUCC(error)) error=Engine_Register(g_root);
			if(pERROR)*pERROR=error;
			if(PR_FAIL(error)) return cFALSE;
		}
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :

//==============================================================
#ifdef KLAVPMGR
		CALL_Root_UnregisterCustomPropId( g_root, npKLAVPMGR_ACTION_NAME);
#endif // KLAVPMGR
//==============================================================

		if(hWindiskIO && PR_SUCC(CALL_SYS_ObjectValid(g_root,(hOBJECT)hWindiskIO)) )
		{
			CALL_Plugin_Unload( hWindiskIO, cFALSE );
			hWindiskIO=0;
		}
		g_root = NULL;
		break;
	}
	return cTRUE;
}
// AVP Prague stamp end


