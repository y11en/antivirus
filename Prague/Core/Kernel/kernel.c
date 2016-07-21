/*-----------------20.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 *                                                *
 * Purpose -- Prague Kernel handle manipulation   *
 *                                                *
--------------------------------------------------*/

#define _CRTIMP

#include "kernel.h"
#include "k_root.h"
#include "k_ifenum.h"
#include "k_trace.h"
#include "receiver.h"
//#include "impex.h"
#include <m_utils.h>

#include <Prague/pr_loadr.h>
#include <Prague/plugin/p_string.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktime.h>

//extern tINTERFACE   StaticInterfaceTable[];
//extern tHANDLE      StaticHandleTable[];
extern tHANDLE**		HandlePool;
extern tINTERFACE*  InterfacePool[];

extern tHANDLE* free_head;
extern tHANDLE* free_tail;

typedef tERROR  (pr_call *typeEnterProtectedCode  )( hOBJECT object, tExceptionReg* reg );
typedef tERROR  (pr_call *typeLeaveProtectedCode  )( hOBJECT object, tExceptionReg* reg );
typedef tERROR  (pr_call *typeGetExceptionInfo    )( hOBJECT object, tExceptionReg* reg, tDWORD* excpt_code, tPTR* info );
typedef tERROR  (pr_call *typeThrowException      )( hOBJECT object, tDWORD excpt_code );

tHANDLE*        g_hRoot;
hROOT           g_root;
iPROT*          Prot;
tBOOL           g_exit_now = cFALSE;
tSynchroEvent*  g_synchro_beg_closers;
tSynchroEvent*  g_synchro_end_closers;
//tHANDLE*      g_condemned_list;

// ---
tINT pr_call _compare_timestamp( hPLUGIN first, hPLUGIN second ) {
	tDT dtf = {0};
	tDT dts = {0};
	if ( first == second )
		return 0;
	if ( first )
		CALL_SYS_PropertyGet( first, 0, pgTIME_STAMP, &dtf, sizeof(dtf) );
	if ( second )
		CALL_SYS_PropertyGet( second, 0, pgTIME_STAMP, &dts, sizeof(dts) );
	return DTCmp( (const tDT*)&dtf, (const tDT*)&dts );
}



PR_MAKE_TRACE_FUNC;

// --- Kernel initialize function
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tCROParam* param ) {
	
	tHANDLE*     handle;
	tINTERFACE*  iface;
	tUINT        i;
	const tDATA* protection_prop;
	hROOT        root;
	tERROR       err;
	
	switch( dwReason ) {
		case DLL_PROCESS_ATTACH   :
		case DLL_PROCESS_DETACH   :
		case DLL_THREAD_ATTACH    :
		case DLL_THREAD_DETACH    : return 1;
		case PRAGUE_PLUGIN_UNLOAD : 
			root = *(hROOT*)param_ROOT(param);
			g_exit_now = cTRUE;
			err = CALL_SYS_ObjectClose( root );
			g_exit_now = cFALSE;
			if ( param_ERROR(param) )
				*(tERROR*)param_ERROR(param) = err;
			*(hROOT*)param_ROOT(param) = 0;
			return 1;
	}			
	
	#if defined(_DEBUG) && !defined(_M_X64) && !defined(_M_IA64)
		set_dbg_report_func( 0 );
	#endif
	
	if ( param_PROT(param) ) {
		if ( PR_FAIL(*(tERROR*)param_ERROR(param)=((iPROT*)param_PROT(param))->Initialize()) )
			return cFALSE;
		si.EnterProtectedCode = (typeEnterProtectedCode)((iPROT*)param_PROT(param))->Enter;
		si.LeaveProtectedCode = (typeLeaveProtectedCode)((iPROT*)param_PROT(param))->Leave;
		si.GetExceptionInfo   = (typeGetExceptionInfo)((iPROT*)param_PROT(param))->GetExceptionInfo;
		si.ThrowException     = (typeThrowException)((iPROT*)param_PROT(param))->Throw;
	}
	Prot = param_PROT(param);

	if ( (param->size == sizeof(tCROParam)) && (param->flags & CROFLAG_UNSAVE_VTBL) )
		g_unsave_vtbl = cTRUE;
	else
		g_unsave_vtbl = cFALSE;

	for( i=0; i<sizeof(si.reserved)/sizeof(si.reserved[0]); i++ )
		si.reserved[i] = System_Reserved;
	
	// initialize internal data srtuctures
	_HandlePoolInit();
	_InterfacePoolInit();
	
	// register kernel interfaces
	if ( PR_FAIL(*(tERROR*)param_ERROR(param)=Root_Register(0)) )
		return cFALSE;
	
	iface = InterfacePool[0];//&StaticInterfaceTable[0];
	
	// create root object
	handle = HandlePool[0];//StaticHandleTable;
	free_head = handle->next;
	free_head->prev = NULL;
	
	iface->flags = IFACE_UNSWAPPABLE | IFACE_LOADED;
	
	// handle init
	mset( handle, 0, sizeof(tHANDLE) );
	handle->iface = iface;
	handle->trace_level.max = prtMIN_TRACE_LEVEL;
	handle->trace_level.min = prtMIN_TRACE_LEVEL;
	
	handle->obj = (tOBJECT*)&root_obj;
	_ObjectInit0( handle );

	_MemInitialize();
	_chk_ext_0( handle );

	g_hRoot = handle;
	g_root  = (hROOT)&handle->obj->vtbl;
	
	*((tPTR*)&root_obj.o.vtbl) = iface->external;
	//  root_obj.o.handle          = handle;
	//  root_obj.o.sys             = (iSYSTEMVtbl*)&si;
	//  *((tPTR*)&root_obj.o.ptr)  = &root_obj.d;
	
	handle->ref = 0;
	//iface->obj_count++;
	
	mset( &root_obj.d, 0, sizeof(tRootData) );
	
	root = (hROOT)MakeObject(handle);
	
	IFaceEnum_Register( root );
	Tracer_Register( root );
	MsgReceiver_Register( root );
	/*
	if ( Synchro )
	Sync_Register( (hROOT)MakeObject(g_hRoot) );
	*/
	
	protection_prop = _PropTableSearch( handle->iface, pgEXCEPTION_CATCHING_AVAILABLE, 0 );
	if ( protection_prop )
		((tDATA*)protection_prop)[1] = param_PROT(param) ? cTRUE : cFALSE;
	
	_TracerInitObj( 0 );
	//_ImpexInitObj( 0 );
	
	if ( param_ROOT(param) )
		*(hROOT*)param_ROOT(param) = root;
	
	return cTRUE;
}



