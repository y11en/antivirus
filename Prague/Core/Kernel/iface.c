/*-----------------19.03.00 00:00 -----------------
 * Project Prague                                 *
 * Subproject Kernel iface registering        *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#define cINTERFACE_POOL    64
#define cINTERFACE_ENTRIES 64

#include "kernel.h"
#include <Prague/iface/e_loader.h>

// ---
tINTERFACE  StaticInterfaceTable[cINTERFACE_ENTRIES];
tINTERFACE* InterfacePool[cINTERFACE_POOL+1];

tDWORD      PoolCurrent;  // index of current pool
tDWORD      IfaceCurrent; //
tDWORD      freed_cookie;
tBOOL       g_unsave_vtbl = cFALSE;


tBOOL pr_call _InterfaceTableInit( tINTERFACE *newtable );

// ---
tERROR pr_call _ObjectNotInitializedDummy( hOBJECT _this ) {
  PR_TRACE(( _this,prtERROR,"krn\tObject not initialized yet" ));
  return errOBJECT_NOT_INITIALIZED;
}


// ---
tERROR pr_call _ObjectFreedDummy( hOBJECT _this ) {
	PR_TRACE(( _this,prtFATAL,"krn\tObject already freed" ));
	INT3;
  return errOBJECT_ALREADY_FREED;
}


// ---
// not initialized function array
_ObjNotInitializedFunctionType _ObjNotInitializedFunctions[METHOD_MAX_COUNT];

// freed object function array
_ObjFreedFunctionType _ObjFreedFunctions[METHOD_MAX_COUNT];



// ---
tERROR _set_vtbl( tHANDLE* handle, const tPTR vtbl ) {
	tOBJECT*    obj = handle->obj;
	tINTERFACE* iface = handle->iface;
	if ( !obj || !iface )
		return errUNEXPECTED;
	if ( g_unsave_vtbl ) {
		if ( !obj->vtbl ) {
			if ( 0 == (iface->flags & IFACE_SET_VTBL) )
				iface->flags |= IFACE_SET_VTBL;
		}
		else if ( iface->flags & IFACE_SET_VTBL )
			;
		else
			return errNOT_SUPPORTED;
	}
	if ( !vtbl )
		*(tPTR*)&vtbl = iface->external;
	obj->vtbl = vtbl;
	return errOK;
}



// ---
void pr_call _InterfacePoolInit() {

	tUINT i;
	
	PR_TRACE_A0( 0, "Enter _InterfacePoolInit" );
	
	for( i=0; i<(sizeof(_ObjNotInitializedFunctions)/sizeof(_ObjNotInitializedFunctions[0])); i++ ) {
		_ObjNotInitializedFunctions[i] = _ObjectNotInitializedDummy;
		_ObjFreedFunctions[i] = _ObjectFreedDummy;
	}
	
	freed_cookie = 0;
	
	for( i=0; i<(cINTERFACE_POOL+1); i++ )
		InterfacePool[i] = NULL;
	
	PoolCurrent = 0;
	
	_InterfaceTableInit( StaticInterfaceTable );
	
	PR_TRACE_A0( 0, "Leave _InterfacePoolInit" );

}



// ---
void pr_call _InterfacePoolDeinit() {

	tUINT i;

	PR_TRACE_A0( 0, "Enter _InterfacePoolDeinit" );

	for ( i=0; i<PoolCurrent; i++ ) {
//    tERROR      e;
//    tUINT       j, c;
//    tINTERFACE* iface = InterfacePool[i];
//
//    c = (i < (PoolCurrent-1)) ? cINTERFACE_ENTRIES : IfaceCurrent;
//    
//    for( j=0; j<c; j++,iface++ ) {
//      if ( iface->cs ) {
//        tHANDLE* handle = _HCWL(iface->cs,&e);
//        _AddToChildList( handle, g_root );
//        _ObjectClose( 0, handle );
//      }
//    }

		if ( !InterfacePool[i] )
			break;
		else if ( i ) {
			PrFree( InterfacePool[i] );
			InterfacePool[i] = 0;
		}
	}

	PR_TRACE_A0( 0, "Leave _InterfacePoolDeinit" );

}



// ---
tBOOL pr_call _InterfaceTableInit( tINTERFACE *newtable ) {

	tUINT i;

	PR_TRACE_A0( 0, "Enter _InterfaceTableInit" );

	PoolCurrent++;
	if ( PoolCurrent < cINTERFACE_POOL ) {
		InterfacePool[PoolCurrent-1] = newtable;
		
		for ( i=0; i<cINTERFACE_ENTRIES; i++ )
			newtable[i].iid = IID_NONE;
		
		IfaceCurrent = 0;
		
		PR_TRACE( (0,prtNOTIFY,"krn\tInterface pool initialized") );
		PR_TRACE_A0( 0, "Leave _InterfaceTableInit" );
		return cTRUE;
	}
	else {
		PR_TRACE( (0,prtDANGER,"krn\tInterface pool limit reached") );
		PR_TRACE_A0( 0, "Leave _InterfaceTableInit" );
		return cFALSE;
	}
}



// ---
tERROR pr_call _InterfaceAlloc( tPO* po, tINTERFACE** iface ) {

	tERROR error = errOK;
	
	PR_TRACE_A0( 0, "Enter _InterfaceAlloc" );
	
	*iface = 0;
	if ( freed_cookie ) {
		
		tDWORD pool = (tDWORD)(freed_cookie >> 16);
		tDWORD ifpos = (tDWORD)(freed_cookie & 0xFFFF);
		tINTERFACE* i = _InterfaceFind( &freed_cookie, IID_NONE, PID_ANY, SUBTYPE_ANY, VID_ANY, cTRUE );
		
		wlock(po);
		if ( i ) {// advance cookie to next free interface
			// recede "pointer" one step back
			if ( (freed_cookie & 0xffff) > 0 )
				freed_cookie--;
			else
				freed_cookie += cINTERFACE_ENTRIES - (((tDWORD)1)<<16) - 1;
		}
		else // have used all freed interfaces
			freed_cookie = 0;
		wunlock(po);
		
		*iface = InterfacePool[pool] + ifpos;
		if ( *iface && ((*iface)->iid == IID_NONE) ) {
			PR_TRACE( (0, prtNOT_IMPORTANT,"krn\tNew interface allocated") );
			PR_TRACE_A0( 0, "Leave _InterfaceAlloc" );
			return errOK;
		}
	}

	if ( IfaceCurrent >= cINTERFACE_ENTRIES ) {
		if ( PR_SUCC(error=PrAlloc((tPTR*)iface,sizeof(tINTERFACE)*cINTERFACE_ENTRIES)) ) {
			if ( cTRUE != _InterfaceTableInit(*iface) ) {
				error = errINTERFACE_LIMIT_EXCEEDED;
				PrFree( *iface );
				*iface = 0;
			}
			else
				IfaceCurrent = 1;
		}
	}
	else
		*iface = InterfacePool[PoolCurrent-1] + IfaceCurrent++;
	
	if ( *iface ) {
		//    if ( g_root && PR_SUCC(_SyncCreateAvailable(MakeObject(g_root),&(*iface)->cs)) )
		//      _Remove( MakeHandle((*iface)->cs) );
		PR_TRACE( (0,prtNOT_IMPORTANT,"krn\tNew interface allocated") );
	}
	else {
		PR_TRACE( (0,prtERROR,"krn\tInterface NOT allocated") );
	}
	
	PR_TRACE_A0( 0, "Leave _InterfaceAlloc" );
	return error;
}



// ---
tBOOL pr_call FindToFree( tINTERFACE* iface, tINTERFACE* to_compare ) {
	return iface == to_compare;
}

// ---
void pr_call _InterfaceFree( tPO* po, tINTERFACE* iface ) {
	tDWORD cookie = 0;
	PR_TRACE_A0( 0, "Enter _InterfaceFree" );
	
	if ( _InterfaceFindInt(&cookie,(tCheckIFaceConditionFn)FindToFree,iface) ) {
		// recede "pointer" one step back
		if ( (cookie & 0xffff) > 0 )
			cookie--;
		else
			cookie += cINTERFACE_ENTRIES - (((tDWORD)1)<<16) - 1;
		
		wlock(po);
		if ( !freed_cookie || (freed_cookie > cookie) )
			freed_cookie = cookie;
		iface->iid = IID_NONE;
		wunlock(po);
		
		PR_TRACE( (0,prtNOT_IMPORTANT,"krn\tInterface freed [%tiid, %tpid, %tvid",iface->iid,iface->pid,iface->vid) );
	}
	
	PR_TRACE_A0( 0, "Leave _InterfaceFree" );
}



// ---
tINTERFACE* pr_call _InterfaceFindInt( tDWORD* cookie, tCheckIFaceConditionFn check_fn, tPTR param ) {

	tDWORD pool;
	tDWORD ifpos;
	tINTERFACE* iface = NULL;
	
	PR_TRACE_A0( 0, "Enter _InterfaceFindInt" );
	
	// init internal variables
	if ( cookie == NULL || *cookie == 0 ) {
		pool = 0;
		ifpos = 0;
	}
	else {
		pool = (tDWORD)(*cookie >> 16);
		ifpos = (tDWORD)(*cookie & 0xffff);
	}
	
	for ( ; ; ) {
		if ( ifpos >= cINTERFACE_ENTRIES ) {
			if ( pool < PoolCurrent ) {
				pool++;
				ifpos = 0;
			}
		  else {
			  iface = NULL;
			  break;
			}
		}

		if ( (pool < (PoolCurrent-1) || ifpos < IfaceCurrent) && InterfacePool[pool] != NULL  )
			iface = InterfacePool[pool] + ifpos;
		
		else {
			iface = NULL;
			break;
		}
		
		ifpos++;
		if ( !check_fn || check_fn(iface,param) )
			break;
	}

	if ( cookie != NULL )
		*cookie = (pool << 16) | ifpos;  // store cookie position
	
	PR_TRACE_A1( 0, "Leave _InterfaceFindInt ret iface = %p", iface );
	
	return iface;
}



#define C_IID( iid, iface )  ((iid == IID_ANY    ) || (iface->iid     == iid)) // check iid
#define C_SUB( sub, iface )  ((sub == SUBTYPE_ANY) || (iface->subtype == sub)) // check subtype
#define C_VID( vid, iface )  ((vid == VID_ANY    ) || (iface->vid     == vid)) // check vid

#define C_PID( pid, iface, check_all )  (((pid == PID_ANY) && ((check_all) || (iface->flags & IFACE_SYSTEM))) || ((pid != PID_ANY) && (iface->pid == pid))) // check pid


// ---
tINTERFACE* pr_call _InterfaceFind( tDWORD* cookie, tIID iid, tPID pid, tDWORD sub, tVID vid, tBOOL check_all ) {
	tDWORD       pool;
	tDWORD       ifpos;
	tINTERFACE*  iface = NULL;
	
	PR_TRACE_A0( 0, "Enter _InterfaceFind" );
	
	// init internal variables
	if ( cookie == NULL || *cookie == 0 ) {
		pool = 0;
		ifpos = 0;
	}
	else {
		pool = (tDWORD)(*cookie >> 16);
		ifpos = (tDWORD)(*cookie & 0xffff);
	}
	
	for ( ; ; ) {
		if ( ifpos >= cINTERFACE_ENTRIES ) {
			if ( pool < PoolCurrent ) {
				pool++;
				ifpos = 0;
			}
			else {
				iface = NULL;
				break;
			}
		}
		
		if ( (pool < (PoolCurrent-1) || ifpos < IfaceCurrent) && InterfacePool[pool] != NULL  )
			iface = InterfacePool[pool] + ifpos;
		
		else {
			iface = NULL;
			break;
		}
		
		ifpos++;
		if ( C_IID(iid,iface) && !(iface->flags & IFACE_CONDEMNED) && C_PID(pid,iface,check_all) && C_SUB(sub,iface) && C_VID(vid,iface) )
			break;
	}
	
	if ( cookie != NULL )
		*cookie = (pool << 16) | ifpos;  // store cookie position
	
	PR_TRACE_A1( 0, "Leave _InterfaceFind ret iface = %p", iface );
	return iface;
}




// ---
static tBOOL pr_call Check_InterfaceFindModule( tINTERFACE* iface, tPTR plugin ) {

	tBOOL ret;
	PR_TRACE_A0( 0, "Enter Check_InterfaceFindModule" );
	
	if ( ((iface->plugin == ((hPLUGIN)plugin)) && (iface->iid != IID_NONE)) )  // ??? > (iface->iid > IID_DEFAULT)) )
		ret = cTRUE;
	else
		ret = cFALSE;
	
	PR_TRACE_A1( 0, "Leave Check_InterfaceFindModule ret tBOOL = %d", ret );
	return ret;
}




// ---
tERROR pr_call _Interface_0_Init( tPO* po, tINTERFACE* iface ) {
	extern iINTERNAL INTERNAL_NULL_TABLE;

	wlock(po);
	iface->flags    &= ~IFACE_LOADED;
	iface->internal  = &INTERNAL_NULL_TABLE;                     // pointer for internal access functions
	iface->in_count  = sizeof(INTERNAL_NULL_TABLE)/sizeof(tPTR); // counter of internal access functions
	iface->external  = _ObjNotInitializedFunctions;              // exported interface functions
	iface->ex_count  = countof(_ObjNotInitializedFunctions);     // function count in table above
	iface->proptable = 0;
	iface->propcount = 0;
	wunlock(po);

	return errOK;
}



// ---
// unload interface
tBOOL pr_call _InterfaceUnload( tPO* po, tINTERFACE* iface ) {

	tERROR  error;
	hPLUGIN plugin;
	
	PR_TRACE_A0( 0, "Enter _InterfaceUnload" );

	if ( PrIsIntegralPlugin && PrIsIntegralPlugin(iface->pid) )
		error = errOK;
	else if ( 0 == (plugin = iface->plugin) ) {
		if ( iface->external && iface->ex_count )
			error = errOK;
		else {
			error = errMODULE_NOT_FOUND;
			PR_TRACE(( 0, prtERROR, "krn\tInterface(%tiid,%tpid) has no plugin handle - errMODULE_NOT_FOUND", iface->iid, iface->pid ));
		}
	}
	else {
		_PR_ENTER_PROTECTED_0(po) {
			error = CALL_Plugin_Unload( plugin, cFALSE );
		} _PR_LEAVE_PROTECTED_0( po, error );
	}

	if ( PR_SUCC(error) )
		--iface->obj_count;

	PR_TRACE_A1( 0, "Leave _InterfaceUnload ret tBOOL = %d", ret );
	return PR_SUCC(error);

//	tBOOL ret;
//	hPLUGIN plugin;
//	
//	PR_TRACE_A0( 0, "Enter _InterfaceUnload" );
//	
//	if ( iface->obj_count ) {
//		ret = cFALSE;
//		//PR_TRACE(( 0,prtNOT_IMPORTANT,"krn\tAttempt interface unloading. It has alive object(s) or do not subject of unloading[%tiid, %tpid, count=%u]",iface->iid,iface->pid,iface->obj_count ));
//	}           
//	else if ( !(iface->flags & (IFACE_CONDEMNED|IFACE_FREE_UNUSED)) ) {
//		ret = cFALSE;
//		//PR_TRACE(( 0,prtNOT_IMPORTANT,"krn\tAttempt interface unloading. It has alive object(s) or do not subject of unloading[%tiid, %tpid, count=%u]",iface->iid,iface->pid,iface->obj_count ));
//	}
//	else if ( 0 == (plugin=iface->plugin) ) {
//		ret = cTRUE;
//		PR_TRACE(( 0,prtNOT_IMPORTANT,"krn\tAttempt plugin unloading. Intraface has no plugin object to unload[%tiid, %tpid]",iface->iid,iface->pid ));
//	}
//	else {
//		tIID   pid = iface->pid;
//		tDWORD cookie = 0;
//		tINTERFACE* alive;
//
//		ret = cTRUE;
//		alive = _InterfaceFindInt( &cookie, Check_InterfaceFindModule, plugin );    // find any alive iface implemented by the plugin
//		while( alive && (alive->flags & (IFACE_CONDEMNED|IFACE_FREE_UNUSED)) && (alive->obj_count == 0) /*&& (alive->iid == IID_NONE)*/ )
//			alive = _InterfaceFindInt( &cookie, Check_InterfaceFindModule, plugin );
//		
//		if ( !alive ) {                                                             // if this plugin has no single alive interface it can be unloaded
//			tERROR e;
//			_PR_ENTER_PROTECTED_0(po) {
//				CALL_Plugin_Unload( plugin );
//			} _PR_LEAVE_PROTECTED_0(po,e);
//			wlock(po);
//			_Interface_0_Init( po, iface );
//			wunlock(po);
//			
//			//		if ( g_plugins == MakeHandle(plugin) )
//			//			g_plugins = g_plugins->next;
//			//    plugin->sys->ObjectClose( (hOBJECT)plugin );
//			PR_TRACE(( 0,prtNOTIFY,"krn\tPlugin unloaded[%tpid]",pid ));
//		}
//		else
//			PR_TRACE(( 0,prtNOT_IMPORTANT,"krn\tAttempt plugin unloading. Plugin has alive objects[%tiid, %tpid]",iface->iid,iface->pid ));
//	}
//	
//	PR_TRACE_A1( 0, "Leave _InterfaceUnload ret tBOOL = %d", ret );
//	return ret;
}




// ---
tERROR pr_call _InterfaceLoad( tPO* po, tINTERFACE* iface ) {

	tERROR error;
	hPLUGIN plugin;
	
	PR_TRACE_A0( 0, "Enter _InterfaceLoad" );

	if ( PrIsIntegralPlugin && PrIsIntegralPlugin(iface->pid) )
		error = errOK;

	else if ( 0 == (plugin=iface->plugin) ) {
		if ( iface->external && iface->ex_count )
			error = errOK;
		else {
			error = errMODULE_NOT_FOUND;
			PR_TRACE(( 0, prtERROR, "krn\tInterface(%tiid,%tpid) has no plugin handle - errMODULE_NOT_FOUND", iface->iid, iface->pid ));
		}
	}
	
	else {
		tERROR  err;
		hPLUGIN plug = g_now_loading;

		g_now_loading = plugin;
		_PR_ENTER_PROTECTED_0(po) {
			err = CALL_Plugin_Load( plugin );
		} _PR_LEAVE_PROTECTED_0( po, err );
		g_now_loading = plug;
		
		if ( iface->flags & IFACE_LOADED )
			error = errOK;
		else if ( PR_FAIL(iface->last_err) )
			error = iface->last_err;
		else if ( PR_FAIL(err) )
			error = err;
		else
			error = errINTERFACE_NOT_LOADED;
	
		if ( (error == errSYNCHRONIZATION_TIMEOUT) || (error == errSYNCHRONIZATION_FAILED) ) {
			tDWORD size = sizeof(error);
			_MsgSend( po, g_hRoot, pmc_KERNEL, pm_INTERFACE_LOAD_ERROR, 0, &error, &size, 0 );
		}
		if( PR_FAIL(error) )
			PR_TRACE(( 0, prtERROR, "krn\tiface loading failed[%tpid, %terr]", iface->pid, error ));
	}

	if ( PR_SUCC(error) )
		++iface->obj_count;

	PR_TRACE_A1( 0, "Leave _InterfaceLoad ret error = %terr", error );
	return error;

//	tERROR   error;
//	tHANDLE* handle;
//	tINT     lock = 1;
//	
//	PR_TRACE_A0( 0, "Enter _InterfaceLoad" );
//	
//	if ( iface->flags & IFACE_LOADED ) 
//		error = errOK;
//	
//	else if ( iface->pid == PID_KERNEL ) {
//		error = errOK;
//		PR_TRACE(( 0,prtNOTIFY,"krn\tKernel iface(%tiid) cannot be loaded)",iface->iid ));
//	}
//	
//	else if ( !iface->plugin ) {
//		error = errMODULE_NOT_FOUND;
//		PR_TRACE(( 0,prtERROR,"krn\tInterface has no plugin handle" ));
//	}
//	
//	else if ( 0 == _HCP(handle,0,lock,(hOBJECT)iface->plugin,&error) ) {
//		PR_TRACE(( 0,prtERROR,"krn\tInterface has bad plugin handle" ));
//	}
//	
//	else {
//		tBOOL loaded = cFALSE;
//		
//		error = _PropertyGetSync( po, handle, 0, pgIS_LOADED, &loaded, sizeof(loaded ));
//
//		if ( loaded ) {
//			if ( iface->flags & IFACE_LOADED )
//				error = errOK;
//			else if ( PR_FAIL(iface->last_err) ) {
//				error = iface->last_err;
//				PR_TRACE(( 0,prtERROR,"krn\tInterface cannot be loaded[%tiid, %tpid] (%terr)",iface->iid,iface->pid,error ));
//			}
//			else {
//				error = errINTERFACE_NOT_LOADED;
//				PR_TRACE(( 0,prtERROR,"krn\tInterface cannot be loaded[%tiid, %tpid] (error=??? errINTERFACE_NOT_LOADED)",iface->iid,iface->pid ));
//			}
//		}
//		else {
//			tERROR  err;
//			hPLUGIN plug;
//			
//			plug = g_now_loading;
//			g_now_loading = iface->plugin;
//			
//			_PR_ENTER_PROTECTED_0(po) {
//				err = CALL_Plugin_Load( iface->plugin );
//			} _PR_LEAVE_PROTECTED_0( po, err );
//			
//			g_now_loading = plug;
//			
//			if ( PR_SUCC(error) ) {
//				if ( iface->flags & IFACE_LOADED ) 
//					error = errOK;
//				else if ( PR_FAIL(iface->last_err) )
//					error = iface->last_err;
//				else if ( PR_FAIL(err) )
//					error = err;
//				else
//					error = errINTERFACE_NOT_LOADED;
//			}
//			
//			if ( PR_SUCC(error) )
//				PR_TRACE(( 0,prtNOTIFY,"krn\tPlugin loaded successfully[%tpid]",iface->pid ));
//			else
//				PR_TRACE(( 0,prtERROR,"krn\tPlugin loading failed[%tpid, %terr]",iface->pid,error ));
//		}
//	}
//	
//	PR_TRACE_A1( 0, "Leave _InterfaceLoad ret error = %terr", error );
//	return error;
}



// ---
const tPTR NotInitializedMethod( tHANDLE* handle, tPTR param, tBOOL ex_in, tDWORD num, tDWORD* data ) {
  *data = 0;
  return &_ObjectNotInitializedDummy;
}




// ---
tERROR pr_call _InterfaceSetNew( tPO* po, tHANDLE* handle, tINTERFACE* iface ) {
	tERROR error;
	
	PR_TRACE_A0( 0, "Enter _InterfaceSetNew" );
	
	error = _MemoryRealloc( po, handle, iface->memsize );

	if ( PR_SUCC(error) ) {
		
		if ( iface->internal->ObjectNew ) {
			CALL_INT_OBJ_NEW( handle, iface, error, cTRUE ); // constructor call
			if ( !_is_vtbl(handle,iface->external) )
				iface->external = (tPTR)handle->obj->vtbl;
		}
		
		wlock(po);
		
		handle->iface       = iface;
		handle->init_props  = _PropInitDword( iface->proptable, iface->propcount );
		//handle->wrappers    = 0;
		//handle->wrap_data   = 0;
		//handle->wrap_data2  = 0;
		
		if ( iface->internal->ObjectNew )
			_ObjectInit0( handle );
		
		if ( PR_FAIL(_WrapperAdd(po,handle,NotInitializedMethod,0)) )
			_set_vtbl( handle, _ObjNotInitializedFunctions );
		
		//if ( !(iface->flags & IFACE_OWN_CP) )
		//	handle->cp = iface->cp;
		
		wunlock(po);
		
		_TraceLevelPropSet( po, handle, iface->trace_level.max, iface->trace_level.min );
		
		PR_TRACE(( MakeObject(handle), prtNOT_IMPORTANT+5, "krn\tObject(%p, %tiid, %tpid) has been created successfully", MakeObject(handle), iface->iid, iface->pid ));
		//_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_HAS_BEEN_CREATED, 0, 0, 0, 0 );
		
		if ( iface->internal->ObjectInit )
			CALL_INT_METH_0( handle, ObjectInit, error );
		
		//if ( PR_SUCC(error) )
		//	iface->obj_count++;
	}
	
	PR_TRACE_A1( 0, "Leave _InterfaceSetNew ret error = %terr", error );
	return error;
}




// ---
//tERROR pr_call _InterfaceReplace( tPO* po, tHANDLE* handle, tINTERFACE* iface ) {
//	
//	tERROR error;
//	PR_TRACE_A0( 0, "Enter _InterfaceSet" );
//	
//	if ( (handle->m_iid != IID_NONE) && (handle->m_iid != iface->iid) )
//		error = errINTERFACE_INCOMPATIBLE;
//	
//	else if ( !handle->iface || (handle->iface->iid == IID_NONE) )
//		error = _InterfaceSetNew( po, handle, iface );
//	
//	else if ( PR_SUCC(error=_MemoryRealloc(po,handle,iface->memsize)) ) {
//		
//		error = _InterfaceLoad( po, iface );
//		if ( (error == errSYNCHRONIZATION_TIMEOUT) || (error == errSYNCHRONIZATION_FAILED) ) {
//			tDWORD size = sizeof(error);
//			_MsgSend( po, handle, pmc_KERNEL, pm_INTERFACE_LOAD_ERROR, 0, &error, &size, 0 );
//		}
//		
//		if ( PR_SUCC(error) ) {
//			tINTERFACE* oiface = handle->iface;
//			tIntFnIFaceTransfer oit = oiface->internal->IFaceTransfer;
//			tIntFnIFaceTransfer nit = iface->internal->IFaceTransfer;
//			
//			if ( (oiface->subtype != iface->subtype) && (oit || nit) ) {
//				_PR_ENTER_PROTECTED_0( po ) {
//					if ( 
//						(!oit || PR_SUCC(error=oit(MakeObject(handle),iface->subtype,IFACE_TRANSFER_QUERY))) &&
//						(!nit || PR_SUCC(error=nit(MakeObject(handle),oiface->subtype,IFACE_TRANSFER_QUERY))) &&
//						(!oit || PR_SUCC(error=oit(MakeObject(handle),iface->subtype,IFACE_TRANSFER_PERFORM))) &&
//						(!nit || PR_SUCC(error=nit(MakeObject(handle),oiface->subtype,IFACE_TRANSFER_PERFORM)))
//					)
//						error = errOK;
//				} _PR_LEAVE_PROTECTED_0( po, error );
//			}
//			
//			wlock(po);
//			handle->iface = iface;
//			if ( oiface->obj_count ) {
//				iface->obj_count++;
//				--oiface->obj_count;
//			}
//			wunlock(po);
//			
//			if ( !oiface->obj_count )
//				_InterfaceUnload( po, oiface );
//			
//		}
//	}
//	
//	PR_TRACE_A1( 0, "Leave _InterfaceSet ret error = %terr", error );
//	return error;
//}



// ---
static tBOOL pr_call _find_iface_by_iid( tINTERFACE* iface, tPTR param ) {
	return _toi(iface->iid) == _toi(param);
}



// ---
tERROR pr_call _InterfaceCompatibleTopBase( tINTERFACE* iface, tIID* iid ) {
	tERROR      err;
	tINTERFACE* if2;
	
	if ( !iface )
		return errINTERFACE_NOT_ASSIGNED_YET;
	
	if ( iface->compat == IID_ANY ) {
		*iid = IID_ANY;
		return errINTERFACE_INCOMPATIBLE;
	}
	
	if2 = _InterfaceFindInt( 0, _find_iface_by_iid, (tPTR)_toi(iface->compat) );
	
	if ( 0 == if2 ) {
		*iid = iface->compat;
		return errOK;
	}
	
	err = _InterfaceCompatibleTopBase( if2, iid );
	if ( err == errINTERFACE_INCOMPATIBLE ) {
		*iid = iface->compat;
		return errOK;
	}
	
	if ( PR_FAIL(err) ) {
		*iid = IID_ANY;
		return err;
	}
	
	return err;
}




// ---
tERROR pr_call _InterfaceCompatible( tINTERFACE* iface, tIID iid ) {
  
  if ( iface->iid == iid )
    return errOK;
  
  if ( iface->compat == PID_ANY ) 
    return errINTERFACE_INCOMPATIBLE;
  
  if ( iface->compat == iid )
    return errOK;

  iface = _InterfaceFindInt( 0, _find_iface_by_iid, (tPTR)_toi(iface->compat) );
	if ( !iface )
		return errINTERFACE_NOT_FOUND;
  return _InterfaceCompatible( iface, iid );
}




// ---
tERROR pr_call _InterfaceCheck( tINTERFACE** ret_iface, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module ) {

  tERROR err = errOK;

  // find apropriate entry in iface table
	tINTERFACE* iface = _InterfaceFind( 0, iid, pid, subtype, vid, cTRUE );
	if ( !iface && (flags & IFACE_SYSTEM) )
		iface = _InterfaceFind( 0, iid, PID_ANY, SUBTYPE_ANY, VID_ANY, cFALSE );

	if ( iface && (iface->pid == PID_KERNEL) )
		iface = 0;

	else if ( iface ) { // iface already regisered
		tINT     cmp;
    tVERSION ver = 0;
		
		iface->last_err = errOK;

		if ( iface->plugin && iface->index && PR_SUCC(CALL_Plugin_GetInterfaceProperty(iface->plugin,0,iface->index,pgINTERFACE_VERSION,&ver,sizeof(ver))) && (ver > version) )
			version = ver;

		if ( iface->version < version ) // such iface already presents
      err = warnINTERFACE_VERSION_RECENT;

		else if ( iface->version > version ) {
			tCHAR* fmt = __FILE__LINE__ "krn\tregistering interface(%tiid, %tpid, %tvid) version conflict(registered=%u,now=%u)";
			PR_TRACE(( MakeObject(g_hRoot), prtERROR, fmt, iid, pid, vid, iface->version, version ));
			//iface->last_err = error = errINTERFACE_VERSION_CONFLICT;
			err = errINTERFACE_VERSION_CONFLICT;
      iface = 0;
		}
		
    else if ( 0 > (cmp=_compare_timestamp(iface->plugin,implemented_module)) )
      err = warnINTERFACE_TIMESTAMT_RECENT;

    else if ( 0 < cmp ) {
      err = errINTERFACE_VERSION_CONFLICT;
      PR_TRACE(( MakeObject(g_hRoot), prtIMPORTANT, "krn\tregistering interface(%tiid, %tpid, %tvid) has an older timestamp", iid, pid, vid ));
    }

		else {
      //err = errINTERFACE_ALREADY_REGISTERED;
			PR_TRACE(( MakeObject(g_hRoot), prtIMPORTANT, "krn\tregistering interface(%tiid, %tpid, %tvid) has the same timestamp. Twice may be ?", iid, pid, vid ));
		}

		/*
		if ( iface->external && (iface->external != _ObjNotInitializedFunctions) ) {
			iface->last_err = error = errINTERFACE_VERSION_CONFLICT;
			iface = 0;
			PR_TRACE(( _this, prtERROR,"krn\tRegistering interface twice (%tiid, %tpid, %tvid, subtype=%u, version=%u)",iid,pid,vid,subtype,version) );
		}
		*/
	}

	/*
	else {
		PR_TRACE(( _this, prtERROR, "krn\tRegistering interface(%tiid, %tpid, %tvid, ver=%u) without preliminary loaded description", iid, pid, vid, version ));
		error = errINTERFACE_NOT_LOADED;
	}
	*/

  if ( PR_FAIL(err) )
    iface = 0;

  if ( ret_iface )
    *ret_iface = iface;

  return err;
}



