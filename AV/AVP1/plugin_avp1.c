// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 July 2002,  17:48 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- Anti-Virus Plugin Engine 3.0
// Author      -- Graf
// File Name   -- plugin_avp1.h
// AVP Prague stamp end



#define PR_IMPEX_DEF

#define __pligin_avp1_static
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include "engine.h"
// AVP Prague stamp end

#include "../L_llio/l_llio.h"


#define PLUGIN_STATIC_DATA
#include "static.h"

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h> 

#include <AV/plugin/p_l_llio.h> 

#if !defined (__unix__)
	#define IMPEX_TABLE_CONTENT
	IMPORT_TABLE( import_table )
		#include <AV/plugin/p_l_llio.h> 
		#include <Prague/iface/e_loader.h> 
		#include <Prague/iface/e_ktime.h> 
	IMPORT_TABLE_END
#else	
	#define IMPEX_TABLE_CONTENT
	IMPORT_TABLE( import_table )
		#include <Prague/iface/e_loader.h> 
		#include <Prague/iface/e_ktime.h> 
	IMPORT_TABLE_END
#endif

hPLUGIN h_llio = 0;

// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	switch( dwReason ) {
		
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH :
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		g_root=NULL;
		break;
		
	case PRAGUE_PLUGIN_LOAD :{
			tERROR error=errOK;
			g_root=(hROOT)hInstance;
			if(PR_SUCC(error)) error=CALL_Root_ResolveImportTable( g_root,0,import_table,PID_AVP1);
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_virtual_name, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_readonly_error, npENGINE_OBJECT_READONLY_tERROR, pTYPE_ERROR| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_readonly_object, npENGINE_OBJECT_READONLY_hOBJECT, pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_integral_parent_io, npENGINE_INTEGRAL_PARENT_IO_hOBJECT, pTYPE_OBJECT );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_counter, npENGINE_OBJECT_DETECT_COUNTER_tDWORD, pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_scan_object, "avp1_scan_object", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_name, "propid_detect_name", pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_type, "propid_detect_type", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_certanity, "propid_detect_certanity", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_disinfectability, "propid_detect_disinfectability", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_danger, "propid_detect_danger", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_detect_behaviour, "propid_detect_behaviour", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_io_created, "avp1_io_created", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_io_position_ptr, "avp1_io_position", pTYPE_PTR );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_recursed, "avp1_recursed", pTYPE_DWORD|pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_workarea_need_save, "avp1_workarea_need_save", pTYPE_DWORD |pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_processing_error, "avp1_processing_error", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_has_special_cure, "avp1_has_special_cure", pTYPE_BOOL |pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_action_class_mask, npENGINE_OBJECT_PARAM_ACTION_CLASS_MASK_tDWORD, pTYPE_DWORD| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_hash_container, "propid_hash_container", pTYPE_OBJECT| pCUSTOM_HERITABLE  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_os_hash, "propid_os_hash", pTYPE_QWORD  );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME, npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME, pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_npENGINE_OBJECT_TRANSFORMER_NAME, npENGINE_OBJECT_TRANSFORMER_NAME, pTYPE_STRING );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_skip, npENGINE_OBJECT_SKIP_THIS_ONE_tBOOL, pTYPE_BOOL );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_object_executable_parent, npENGINE_OBJECT_EXECUTABLE_PARENT_IO_hOBJECT, pTYPE_OBJECT| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_boot_BytesPerSector, "propid_boot_BytesPerSector", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_boot_SectorsPerTrack, "propid_boot_SectorsPerTrack", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_boot_TracksPerCylinder, "propid_boot_TracksPerCylinder", pTYPE_DWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_boot_Cylinders, "propid_boot_Cylinders", pTYPE_QWORD );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_auto_password_bool, "propid_auto_password_bool", pTYPE_BOOL| pCUSTOM_HERITABLE );
			if(PR_SUCC(error)) error=CALL_Root_RegisterCustomPropId( g_root, &propid_action_class_mask_current, "propid_action_class_mask_current", pTYPE_DWORD| pCUSTOM_HERITABLE  );
			if(PR_SUCC(error)) error=Engine_Register(g_root);
			
			if(pERROR)*pERROR=error;
			if(PR_FAIL(error)) return cFALSE;
							 }
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		if(h_llio && PR_SUCC(CALL_SYS_ObjectValid(g_root,(hOBJECT)h_llio)))
		{
			CALL_Plugin_Unload( h_llio, cFALSE );
			h_llio=0;
		}
		g_root=NULL;
		break;
	}
	return cTRUE;
}
// AVP Prague stamp end



