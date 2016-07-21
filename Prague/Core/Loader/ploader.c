/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Prague loader                       *
 * Author Andrew Andy Petrovitch Graf             *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#define PR_IMPEX_DEF

#if defined(_M_AMD64) && !defined(_M_X64) // temporary for dskm.lib
	void __GSHandlerCheck(){}
#endif

#if ((_WIN32_WINNT < 0x0400) || (_WIN32_WINDOWS <= 0x0400)) 
	// make CreateWaitableTimer/SetWaitableTimer visible
	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0400
#endif

#include <stdlib.h>
#include <time.h>
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h> 
#include "loader.h"
#include "mod_load.h"
#include <Prague/pr_loadr.h>
#include "prformat.h"
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h> 
#include "csection.h"
#include "csect_2l.h"
#include "sem.h"
#include "mutex.h"
#include "syncevent.h"
#include "plugin.h"

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>

#if defined( __cplusplus )
extern "C" {
#endif

	hPLUGININSTANCE  g_hiKernel;
	tBOOL            g_2l_cs = cFALSE;
	hROOT            g_root;
	tWCHAR           l_plugins_path [ MAX_PATH + 1 ] = {0};
	tPTR             g_plugins_path = l_plugins_path; 
	tWCHAR           g_application  [ MAX_PATH + 1 ] = {0};
	tWCHAR           g_loader_path  [ MAX_PATH + 1 ] = {0};		
	tWCHAR           g_kernel_path  [ MAX_PATH + 1 ] = {0};
	tDSKMAppId*      g_app_id;

#if defined( __cplusplus )
}
#endif

tVOID Trace( tSTRING str, hOBJECT obj, tTRACE_LEVEL level );
tERROR PragueUnloadInt( hROOT* root ); 

#if defined(NEW_SIGN_LIB)
	tERROR pr_call SCheck_Register( hROOT root );
	tERROR pr_call SCheckList_Register( hROOT root );
#endif

#include "ploader_inc.c"

#define  IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_ktime.h> 
	#include <Prague/iface/e_ktrace.h> 
IMPORT_TABLE_END

EXPORT_TABLE(export_table)
	#include <Prague/iface/e_loader.h> 
EXPORT_TABLE_END
#undef IMPEX_TABLE_CONTENT


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
#include <stdarg.h>
PR_MAKE_TRACE_FUNC;



#define PrLdr_FacelessWnd_ TEXT("PrLdr_FacelessWnd_")

tERROR pr_call _init_string_iface( hROOT root );
tERROR pr_call _init_string_impex();

tERROR IntegratePlugins( tDWORD init_flags );


// ---
tERROR PrLoaderInit( hROOT root ) {
	tERROR error;
	
	if ( PR_FAIL(error=_init_string_iface(root)) )
		return error;
	
	if ( PR_FAIL(error=PrLoader_OS_depended_init(root)) )
		return error;
	
	if ( PR_FAIL(error=CriticalSection_Register_2l(root)) )
		return error;

	_init_dskm_main_sync();

	if ( PR_FAIL(error=CriticalSection_Register(root)) )
		return error;
	
	if ( PR_FAIL(error=Plugin_Register(root)) )
		return error;
	
	if ( PR_FAIL(error=Loader_Register(root)) )
		return error;
	
	//if ( PR_FAIL(error=IFace_Register(root)) )
	//  return error;
	if ( PR_FAIL(error=Event_Register(root)) )
		return error;
	if ( PR_FAIL(error=Semaphore_Register(root)) )
	    return error;
	if ( PR_FAIL(error=Mutex_Register(root)) )
		return error;

	#if defined(NEW_SIGN_LIB)
		if ( PR_FAIL(error=SCheck_Register(root)) )
			return error;
		if ( PR_FAIL(error=SCheckList_Register(root)) )
			return error;
	#endif

	return errOK;
}
// AVP Prague stamp end]




// ---
PR_EXPORT tERROR pr_call PragueLoad( hROOT* root_object, tDWORD init_flags, root_check_func root_check_func_ptr, tPTR rcf_params, tDSKMAppId* app_id ) {
	
	tERROR            error = errOK;
	tCROParam         param = { root_object, &error, 0, sizeof(tCROParam), CROFLAG_UNSAVE_VTBL };
	tINT              plugin_type;
	tCreateRootObject CreateRootObject;
	PR_TRACE_VARS;
	
	if ( !root_object )
		return errPARAMETER_INVALID;
	
	if ( g_hiKernel ) {
		*root_object = g_root;
		return errOK;
	}
	
	error = errOK;
	CreateRootObject = 0;
	*root_object = 0;
	
	if ( init_flags & PR_LOADER_FLAG_APP_ID )
		g_app_id = app_id;
	else
		g_app_id = 0;

	_init_dskm_main_folder();

	#if !defined(NOT_VERIFY)
		if ( PR_FAIL(error=_check_by_mask(g_loader_path,cCP)) )
			return error;
		if ( PR_FAIL(error=_check_by_mask(g_kernel_path,cCP)) )
			return error;
	#endif

	#if defined (OLD_METADATA_SUPPORT)
		KLDT_Init_Library( malloc, free );
	#endif // OLD_METADATA_SUPPORT
	
	PrLoader_OS_depended_load();

	//param_ROOT(&param)  = root_object; 
	//param_ERROR(&param) = &error; 
	//param_PROT(&param) = 0; 

	srand( (unsigned)time(NULL) ); 
	
	if ( init_flags & PR_LOADER_FLAG_2L_CS_DEFAULT ) {
		init_flags &= ~PR_LOADER_FLAG_2L_CS_DEFAULT;
		g_2l_cs = cTRUE;
	}

	plugin_type = CheckPluginByName( g_kernel_path, cCP );
	error = LoadPlugin( 0, g_kernel_path, cCP, plugin_type, &g_hiKernel, (tPluginInit*)&CreateRootObject, 0, 0 );  
	
	if ( PR_FAIL(error) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't load kernel module(\"%s%S\") %terr", PR_TRACE_PREPARE_NT(g_kernel_path), error ));
		return error;
	}
	if ( !CreateRootObject ) {
		PR_TRACE(( 0, prtERROR, "ldr\tKernel module(\"%s%S\") has no entry point", PR_TRACE_PREPARE_NT(g_kernel_path) ));
		PragueUnloadInt( 0 );
		return errMODULE_HAS_NO_INIT_ENTRY;
	}
	if ( !CreateRootObject((tPTR)g_hiKernel,PRAGUE_PLUGIN_LOAD,&param) || PR_FAIL(error) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't initialize kernel root - %terr", error ));
		PragueUnloadInt( 0 );
		return error;
	}
	
	g_root = *root_object;
	
	if ( root_check_func_ptr && (errOK != (error=root_check_func_ptr(*root_object,rcf_params))) ) {
		PR_TRACE( (0, prtERROR, "ldr\tRoot check failed (%terr)", error) );
		PragueUnloadInt( root_object );
		return error;
	}
	
	if ( (init_flags & PR_LOADER_START_TRACER) && (0 == CALL_SYS_PropertyGetPtr(*root_object,pgOUTPUT_FUNC)) )
		CALL_SYS_PropertySetPtr( *root_object, pgOUTPUT_FUNC, Trace );
	
	if ( PR_FAIL(error=PrLoaderInit(*root_object)) ) {
		PR_TRACE( (0, prtERROR, "ldr\tCan't initialize loader interfaces (%terr)", error) );
		PragueUnloadInt( root_object );
		return error;
	}
	
	if ( PR_FAIL(error=CreateModule(*root_object,0,g_application,cCP,PID_APPLICATION)) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't create application module object (%terr)", error ));
		PragueUnloadInt( root_object );
		return error;
	}
	if ( PR_FAIL(error=CreateModule(*root_object,g_loader,g_loader_path,cCP,PID_LOADER)) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't create loader module object (%terr)", error ));
		PragueUnloadInt( root_object );
		return error;
	}
	
	CALL_Root_RegisterExportTable( *root_object, 0, export_table, PID_LOADER );
	
	if ( PR_FAIL(error=CreateModule(*root_object,g_hiKernel,g_kernel_path,cCP,PID_KERNEL)) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't create kernel module object (%terr)", error ));
		PragueUnloadInt( root_object );
		return error;
	}
	if ( PR_FAIL(error=CreateModule(*root_object,g_loader,g_loader_path,cCP,PID_STRING)) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't create loader module object (%terr)", error ));
		PragueUnloadInt( root_object );
		return error;
	}
	if ( PR_FAIL(error=CALL_SYS_ObjectCreateDone(*root_object)) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't complete initialization of kernel root" ));
		PragueUnloadInt( root_object );
		return error;
	}
	if ( PR_FAIL(error=_init_string_impex()) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCan't register string impex", error ));
		PragueUnloadInt( root_object );
		return error;
	}
	
	CALL_Root_ResolveImportTable( *root_object, 0, import_table, PID_LOADER );
	if ( DTSet )
		InitTime();	// fix by Mike&Sobko 28/06/04	// must be recalled when time changed outside
	
	IntegratePlugins( init_flags );
	return errOK;
}



// ---
PR_EXPORT tERROR PragueUnload( hROOT root ) {
	
	tCreateRootObject CloseRoot;
	tERROR err = errOK;

	PrStopIdle( cTRUE );
	_free_dskm_main_session();
	_deinit_dskm_main_sync();

	if ( root && (0 != (CloseRoot=(tCreateRootObject)GetStartAddress(g_hiKernel))) ) {
		tCROParam param = { &root, &err, 0, sizeof(tCROParam), 0 };
		CloseRoot( (tPTR)g_hiKernel, PRAGUE_PLUGIN_UNLOAD, &param );
	}

	g_root = 0;

	#if defined (OLD_METADATA_SUPPORT)
		KLDT_Deinit_Library( cFALSE );
	#endif // OLD_METADATA_SUPPORT

	if ( g_hiKernel ) {
		UnloadPlugin( g_hiKernel, 0, 0 );
		g_hiKernel = 0;
	}

	PrLoader_OS_depended_unload();
	return err;
}



// ---
tERROR PragueUnloadInt( hROOT* root ) {
  tERROR error = PragueUnload( root ? *root : 0 );
  if ( root )
    *root = 0;
  return error;  
}

