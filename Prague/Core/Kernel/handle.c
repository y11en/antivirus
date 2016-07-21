/*-----------------20.03.00 09:31-------------------
* Project Prague                                 *
* Subproject Kernel interfaces                   *
* Author Andrew Andy Petrovitch                  *
* Copyright (c) Kaspersky Lab                    *
*                                                *
* Purpose -- Prague Kernel handle manipulation   *
*                                                *
--------------------------------------------------*/

#define cHANDLE_POOL    0x40
#define cHANDLE_ENTRIES 0x200

#include "kernel.h"
#include <m_utils.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktrace.h>


// ---
tHANDLE   StaticHandleTable[cHANDLE_ENTRIES+1];
tHANDLE** HandlePool;
tHANDLE** HandlePoolPtr;
tUINT     HandlePoolCount;
tHANDLE*  HandlePoolStatic[cHANDLE_POOL];
tHANDLE*  HandlePoolStaticPtr[cHANDLE_POOL];

tHANDLE*  free_head;
tHANDLE*  free_tail;

tUINT     g_handle_count;
tUINT     g_handle_count_peak;

tUINT     g_handle_ext_count;
tUINT     g_handle_ext_count_peak;

#if defined(CHECK_LOCKS)
	tVOID _cs_dbg_before_leave( tPO* po ) {
		if ( po ) {
			if ( po->usage < 0 ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tbefore leave\tSynchronization failure detected - usage(%i) on leave CS", po->usage ));
			}
			else if ( po->lcs ) {
				if ( (po->usage+1) != po->lcs->RecursionCount ) {
					PR_TRACE(( g_root, prtDANGER, "krn\tbefore leave\tSynchronization failure detected - usage(%i) != CS.RecursionCount(%i)", po->usage+1, po->lcs->RecursionCount ));
				}
			}
			else if ( po->usage ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tbefore leave\tSynchronization failure detected - usage(%i) when CS is NULL", po->usage ));
			}
		}
	}

	tVOID _cs_dbg_after_leave( tPO* po ) {
		if ( po ) {
			if ( po->usage < 0 ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tafter leave\tSynchronization failure detected - usage(%i) on leave CS", po->usage ));
			}
			else if ( !po->lcs && po->usage ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tafter leave\tSynchronization failure detected - usage(%i) when CS is NULL", po->usage ));
			}
		}
	}
	
	tVOID _cs_dbg_before_enter( tPO* po ) {
		if ( po ) {
			if ( po->usage < 0 ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tbefore enter\tSynchronization failure detected - usage(%i) on enter CS", po->usage ));
			}
			else if ( !po->lcs && po->usage ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tbefore enter\tSynchronization failure detected - usage(%i) when CS is NULL", po->usage ));
			}
		}
	}

	tVOID _cs_dbg_after_enter( tPO* po ) {
		if ( po ) {
			if ( po->usage < 0 ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tafter enter\tSynchronization failure detected - usage(%i) on enter CS", po->usage ));
			}
			else if ( po->lcs ) {
				if ( (po->usage+1) != po->lcs->RecursionCount ) {
					PR_TRACE(( g_root, prtDANGER, "krn\tafter enter\tSynchronization failure detected - usage(%i) != CS.RecursionCount(%i)", po->usage+1, po->lcs->RecursionCount ));
				}
			}
			else if ( po->usage ) {
				PR_TRACE(( g_root, prtDANGER, "krn\tafter enter\tSynchronization failure detected - usage(%i) when CS is NULL", po->usage ));
			}
		}
	}
#endif


// ---
#ifdef _DEBUG

typedef tERROR  (pr_call *tInt0) ( hOBJECT object );
typedef tERROR  (pr_call *tInt1) ( hOBJECT object, tPTR p1 );
typedef tERROR  (pr_call *tInt2) ( hOBJECT object, tPTR p1, tPTR p2 );
typedef tERROR  (pr_call *tInt4) ( hOBJECT object, tPTR p1, tPTR p2, tPTR p3, tPTR p4 );
typedef tERROR  (pr_call *tInt6) ( hOBJECT object, tPTR p1, tPTR p2, tPTR p3, tPTR p4, tPTR p5, tPTR p6 );
typedef tERROR  (pr_call *tInt7) ( hOBJECT object, tPTR p1, tPTR p2, tPTR p3, tPTR p4, tPTR p5, tPTR p6, tPTR p7 );
typedef tERROR  (pr_call *tInt8) ( hOBJECT object, tPTR p1, tPTR p2, tPTR p3, tPTR p4, tPTR p5, tPTR p6, tPTR p7, tPTR p8 );

void call_int_meth( tPO* po, tHANDLE* h, void* m, tERROR* err, tDWORD n, tPTR p1, tPTR p2, tPTR p3, tPTR p4, tPTR p5, tPTR p6, tPTR p7, tPTR p8 ) {
	_PR_ENTER_PROTECTED(h) {
		switch( n ) {
			case 0: *err = ((tInt0)(m))( MakeObject(h) ); break;
			case 1: *err = ((tInt1)(m))( MakeObject(h), p1 ); break;
			case 2: *err = ((tInt2)(m))( MakeObject(h), p1, p2 ); break;
			case 4: *err = ((tInt4)(m))( MakeObject(h), p1, p2, p3, p4 ); break;
			case 6: *err = ((tInt6)(m))( MakeObject(h), p1, p2, p3, p4, p5, p6 ); break;
			case 7: *err = ((tInt7)(m))( MakeObject(h), p1, p2, p3, p4, p5, p6, p7 ); break;
			case 8: *err = ((tInt8)(m))( MakeObject(h), p1, p2, p3, p4, p5, p6, p7, p8 ); break;
			default: *err = errUNEXPECTED;
		}
	} _PR_LEAVE_PROTECTED(*err);
}
#endif



// ---
static tVOID pr_call hInit( tUINT ind, tHANDLE* handle ) {
	tQWORD j;
	HandlePoolPtr[ind] = handle;
	
	j = ((tQWORD)handle) & (~0xfffffffffffffff8L);
	if ( j )
		*((tBYTE**)&handle) += ((~0xfffffffffffffff8L) + 1) - j;
	HandlePool[ind] = handle;
	
	for ( j=0; j<cHANDLE_ENTRIES; ++j, ++handle )
		_HandleFree( 0, handle );
}




// ---
static tERROR pr_call _HandleTableInit( tPO* po, tHANDLE* new_table ) {
	
	tERROR    err;
	tUINT     i;
	tHANDLE** new_pool;
	tHANDLE** new_pool_ptr;
	tHANDLE*  nt = new_table;
	
	if ( !nt && PR_FAIL(err=PrAlloc((tPTR*)&nt,sizeof(tHANDLE)*(cHANDLE_ENTRIES+1))) ) {
		PR_TRACE(( 0, prtDANGER, "krn\t(_HandleTableInit) -- Memory came to the end..." ));
		return err;
	}
	
	wlock(po);
	for ( i=0; i<HandlePoolCount; i++ ) {
		if ( HandlePool[i] == NULL ) {
			hInit( i, nt );
			wunlock(po);
			return errOK;
		}
	}
	
	if ( HandlePoolCount == cHANDLE_POOL ) {
		if ( PR_FAIL(err=PrAlloc((tPTR*)&new_pool,2*cHANDLE_POOL*sizeof(tHANDLE*))) )
			;
		else if ( PR_FAIL(err=PrAlloc((tPTR*)&new_pool_ptr,2*cHANDLE_POOL*sizeof(tHANDLE*))) )
			PrFree( new_pool );
	}
	else if ( PR_SUCC(err=PrRealloc((tPTR*)&new_pool,HandlePool,(HandlePoolCount+cHANDLE_POOL)*sizeof(tHANDLE*))) )
		err = PrRealloc( (tPTR*)&new_pool_ptr, HandlePoolPtr, (HandlePoolCount+cHANDLE_POOL)*sizeof(tHANDLE*) );
	
	if ( PR_SUCC(err) ) {
		if ( HandlePoolCount == cHANDLE_POOL ) {
			mcpy( new_pool, HandlePool, cHANDLE_POOL*sizeof(tHANDLE*) );
			mcpy( new_pool_ptr, HandlePoolPtr, cHANDLE_POOL*sizeof(tHANDLE*) );
		}
		HandlePool = new_pool;
		HandlePoolPtr = new_pool_ptr;
		HandlePoolCount += cHANDLE_POOL;
		hInit( i, nt );
	}
	else if ( !new_table ) {
		PrFree( nt );
		PR_TRACE(( 0, prtDANGER, "krn\t(_HandleTableInit) -- Memory came to the end..." ));
	}
	wunlock(po);
	return err;
}



// ---
void pr_call _HandlePoolInit() {
	HandlePool = HandlePoolStatic;
	HandlePoolPtr = HandlePoolStaticPtr;
	HandlePoolCount = countof(HandlePoolStatic);
	mset( HandlePoolStatic, 0, sizeof(HandlePoolStatic) );
	mset( HandlePoolStaticPtr, 0, sizeof(HandlePoolStaticPtr) );
	_HandleTableInit( 0, StaticHandleTable );
}


// ---
void pr_call _HandlePoolDeinit() {
	tUINT i;
	
	for ( i=1; i<HandlePoolCount; i++ ) {
		if ( HandlePool[i] == NULL )
			break;
		PrFree( HandlePoolPtr[i] );
	}
	
	if ( HandlePool != HandlePoolStatic ) {
		PrFree( HandlePool );
		PrFree( HandlePoolPtr );
	}
	free_head = free_tail = 0;
}



// ---
tVOID pr_call _ObjectInit0( tHANDLE* handle ) {
	tOBJECT* obj;
	if ( 0 != (obj=handle->obj) ) {
		_set_vtbl( handle, _ObjNotInitializedFunctions );
		obj->handle   = handle;
		obj->sys      = (iSYSTEMVtbl*)&si;
		*odata( obj ) = obj + 1;
	}
}



// ---
static tHANDLE* pr_call _FindFreeHandle( tPO* po ) {
	
	tHANDLE* found = free_head;
	while( found ) {
		if ( !(found->flags & hf_NOT_REUSABLE) ) {
			if ( found->prev )
				found->prev->next = found->next;
			else
				free_head = found->next;
			
			if ( found->next )
				found->next->prev = found->prev;
			else
				free_tail = found->prev;
			
			found->next = found->prev = 0;
			return found;
		}
		found = found->next;
	}
	
	if ( PR_FAIL(_HandleTableInit(po,0)) )
		return 0;
	
	return _FindFreeHandle( po );
}



// ---
tERROR pr_call _HandleCreate( tPO* po, tHANDLE* parent, tHANDLE** new_handle ) {

	tERROR error = errOK;
	PR_TRACE_A0( MakeObject(parent), "Enter _HandleCreate" );
	
	*new_handle = 0;

	wlock(po);
	*new_handle = _FindFreeHandle( po );

	if ( !*new_handle ) {
		error = errNOT_ENOUGH_MEMORY;
		wunlock(po);
	}
	else {
		
		// object init
		_ObjectInit0( *new_handle );
		
		// handle init
		mset( *new_handle, 0, sizeof(tHANDLE) );
		(*new_handle)->trace_level.min = prtMIN_TRACE_LEVEL;
		(*new_handle)->trace_level.max = prtMIN_TRACE_LEVEL;
		(*new_handle)->parent = parent;
		_AddToChildList( *new_handle, parent ); // add to parent list
		(*new_handle)->ref = 0;
		++g_handle_count;
		if ( g_handle_count > g_handle_count_peak )
			g_handle_count_peak = g_handle_count;
		
		wunlock(po);
		error = _CustomPropInherit( po, *new_handle );
	}
	
	PR_TRACE_A2( MakeObject(parent), "Leave _HandleCreate ret tHANDLE* = %p, %terr", *new_handle, error );
	return error;
}



// ---
void pr_call _HandleFree( tPO* po, tHANDLE* handle ) {

	PR_TRACE_A0( MakeObject(handle), "Enter _HandleFree" );
	
	wlock(po);
	handle->prev = free_tail;
	
	if ( free_tail != NULL )
		free_tail->next = handle;
	else
		free_head = handle;
	
	free_tail = handle;
	if ( handle->flags & hf_NOT_REUSABLE )
		handle->flags = hf_OBJECT_CLOSED | hf_NOT_REUSABLE;
	else
		handle->flags = hf_OBJECT_CLOSED;
	handle->next = 0;
	handle->child = 0;
	if ( handle->obj )
		_set_vtbl( handle, _ObjFreedFunctions );
	
	wunlock(po);
	
	PR_TRACE_A0( MakeObject(handle), "Leave _HandleFree, ret tVOID" );

}


#if defined( _DEBUG )

	// ---
	tHANDLE* pr_call _handle_check_debug_func( tPO* po, tVOID* obj, tERROR* err ) {
		tHANDLE* h = MakeHandle(obj);
		if ( h ) {
			*err = errOK;
			if ( po && (po->usage > 2) ) {
				po->usage = po->usage;
			}
			rlock(po);
			return h;
		}
		*err = errHANDLE_INVALID;
		return 0;
	}


	// ---
	tVOID pr_call _handle_uncheck_p_debug_func( tPO* po, tHANDLE* h ) {
		if ( PSYNC_OFF(h) )
			return;
		if ( po && po->cs ) {
			--po->usage;
			po->cs->vtbl->Leave( (po)->cs,0 );
		}
	}

#endif


// ---
tHANDLE* pr_call _handle_check( tPO* po, tVOID* obj, tERROR* err ) {
	tHANDLE* phandle;
	
	if ( !obj ) {
		*err = errOBJECT_INVALID;
		return 0;
	}

	if ( PrSafelyReadMemory ) {
		tDWORD       size;
		tHANDLE**    probably_handle;
		tINT         offs;
		tProveObject prove;
		tOBJECT*     back;
		tINTERFACE*  handle_iface;
		tPTR         handle_iface_vtbl;
		tPTR         obj_vtbl;

		if ( PR_FAIL(PrSafelyReadMemory(OBJD(obj,OBJ_PROVE_OFFSET),&prove,sizeof(prove),&size)) ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( prove.cpp_data == OBJD(obj,CPP_OBJ_TO_DATA_OFFSET) ) {  // its a "c++" object
			probably_handle = &KOBJ(OBJD(obj,-CPP_OBJ_TO_OBJ_OFFSET))->handle;
			offs = CPP_HDL_TO_OBJ_OFFSET;
		}
		else if ( prove.c_data == OBJD(obj,C_OBJ_TO_DATA_OFFSET) ) { // it a "c" object
			probably_handle = &KOBJ(OBJD(obj,-C_OBJ_TO_OBJ_OFFSET))->handle;
			offs = C_HDL_TO_OBJ_OFFSET; 
		}
		else {                                                // its a wrong object
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( PR_FAIL(PrSafelyReadMemory(probably_handle,&phandle,sizeof(phandle),&size)) ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( PR_FAIL(PrSafelyReadMemory(&phandle->obj,&back,sizeof(back),&size)) ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( OBJD(back,offs) != obj ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( PR_FAIL(PrSafelyReadMemory(&phandle->iface,&handle_iface,sizeof(handle_iface),&size)) ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( PR_FAIL(PrSafelyReadMemory(&handle_iface->external,&handle_iface_vtbl,sizeof(handle_iface_vtbl),&size)) ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( PR_FAIL(PrSafelyReadMemory(&back->vtbl,&obj_vtbl,sizeof(obj_vtbl),&size)) ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		if ( handle_iface_vtbl != obj_vtbl ) {
			*err = errOBJECT_INVALID;
			return 0;
		}
		*err = errOK;
		rlock(po);
		return phandle;
	}

	*err = errOK;
	rlock(po);
	return MakeHandle(obj);
}





// ---
tHANDLE* pr_call _HandleCheck( tPO* po, hOBJECT object, tERROR* error, tBOOL any ) {

	tHANDLE** pool;
	tUINT     pool_index;
	tHANDLE*  handle = 0;
	tHANDLE*  found;
	//tExceptionReg reg;
	
	PR_TRACE_A0( 0, "Enter _HandleCheck" );
	
	if ( !object ) {
		*error = errHANDLE_INVALID;
		PR_TRACE(( 0, prtNOTIFY, "krn\tFailed _HandleCheck - errHANDLE_INVALID" ));
		PR_TRACE_A0( 0, "Leave _HandleCheck failed - errHANDLE_INVALID" );
		return 0;
	}
	
	found = 0;
//	rlock();

	handle = _handle_check( po, object, error );
//	if ( si.EnterProtectedCode == System_EnterProtectedCode ) {
//		tOBJECT obj;
//		if ( PR_SUCC(*error = PrSafelyReadMemory(OBJD(object,-1),&obj,sizeof(obj),0)) ) {
//			if ( obj.data == OBJD(object,3) )
//				handle = (tHANDLE*)obj.handle;
//			else {
//				if ( obj.sys == OBJD(object,2) )
//					*error = PrSafelyReadMemory( OBJD(object,-2), &handle, sizeof(handle), 0 );
//			}
//		}
//	}
//	else {
//		if ( errOK == si.EnterProtectedCode(0,&reg) ) {
//			*error = errOK;
//			handle = MakeHandle( object );																																  
//		}
//		else {
//			*error = errHANDLE_INVALID;
//			PR_TRACE(( 0, prtNOTIFY, "krn\tFailed _HandleCheck - errHANDLE_INVALID" ));
//		}
//		si.LeaveProtectedCode( 0, &reg );
//	}
	
	if ( !handle || PR_FAIL(*error) ) {
		handle = 0;
		//runlock();
		*error = errHANDLE_INVALID;
		PR_TRACE_A0( 0, "Leave _HandleCheck failed - errHANDLE_INVALID" );
		return 0;
	}
	
	for( pool=HandlePool,pool_index=0; !found && (pool_index<HandlePoolCount) && *pool; ++pool_index,++pool ) {
		tHANDLE* end = *pool + cHANDLE_ENTRIES;
		if ( (*pool<=handle) && (handle<=end) ) {
			tDATA diff = ((tBYTE*)handle) - ((tBYTE*)*pool);
			if ( !diff || !(diff % sizeof(tHANDLE)) )
				found = handle;
			break;
		}
	}
	
	if ( !found ) {
		runlock(po);
		*error = errHANDLE_INVALID;
		PR_TRACE(( 0, prtNOTIFY, "krn\tFailed _HandleCheck - errHANDLE_INVALID" ));
		PR_TRACE_A0( 0, "Leave _HandleCheck failed - errHANDLE_INVALID" );
		return 0;
	}
	
	if ( any ) {
		*error = errOK;
		PR_TRACE_A0( 0, "Leave _HandleCheck -- ok" );
		return found;
	}
	
	if ( !found->iface ) {
		runlock(po);
		*error = errINTERFACE_NOT_ASSIGNED_YET;
		PR_TRACE( (0,prtIMPORTANT,"krn\tFailed _HandleCheck - errINTERFACE_NOT_ASSIGNED_YET") );
		PR_TRACE_A0( 0, "Leave _HandleCheck failed - errHANDLE_INVALID" );
		return 0;
	}
	
	if ( found->flags & hf_OBJECT_CLOSED ) {
		runlock(po);
		*error = errOBJECT_ALREADY_FREED;
		PR_TRACE( (0,prtIMPORTANT,"krn\tFailed _HandleCheck - errOBJECT_ALREADY_FREED") );
		PR_TRACE_A0( 0, "Leave _HandleCheck failed - errHANDLE_INVALID" );
		return 0;
	}
	
	if ( found->flags & hf_OBJECT_IS_CLOSING ) {
		*error = warnOBJECT_IS_CLOSING;
		PR_TRACE( (0,prtIMPORTANT,"krn\t_HandleCheck warning - warnOBJECT_IS_CLOSING") );
	}
	else
		*error = errOK;
	
	PR_TRACE_A0( 0, "Leave _HandleCheck -- ok" );
	return found;
}




// ---
tHANDLE* pr_call _HandleFindStat( tPO* po, tDWORD* cookie, tCheckHandConditionFn check_fn, tPTR param ) {
	tUINT    pool;
	tUINT    hpos;
	tHANDLE* handle = 0;
	PR_TRACE_A0( 0, "Enter _HandleFindStat" );
	
	// init internal variables
	if ( cookie == NULL || *cookie == 0 ) {
		pool = 0;
		hpos = 0;
	}
	else {
		pool = (tDWORD)(*cookie >> 16);
		if ( pool >= HandlePoolCount ) {
			handle = 0;
			return 0;
		}
		hpos = (tDWORD)(*cookie & 0xffff);
	}
	
	for ( ; ; ) {
		if ( hpos >= cHANDLE_ENTRIES ) {
			pool++;
			hpos = 0;
			if ( (pool >= HandlePoolCount) || !HandlePool[pool] ) {
				handle = 0;
				break;
			}
		}
		
		handle = HandlePool[pool] + hpos;
		
		if ( handle )
			hpos++;
		else
			break;
		
		if ( check_fn ) {
			if ( check_fn(handle,param) )
				break;
		}
		else if ( !(handle->flags & hf_OBJECT_CLOSED) )
			break;
	}
	
	if ( cookie != NULL )
		*cookie = (pool << 16) | hpos;  // store cookie position
	
	PR_TRACE_A1( 0, "Leave _HandleFindStat ret handle = %p", handle );
	return handle;
}



static tHANDLE* pr_call _FindInCache( tHANDLE* handle, tIID iid, tPID pid, tDWORD up, tDWORD down );
static tVOID    pr_call _AddToCache( tPO* po, tHANDLE* handle, tHANDLE* found, tDWORD up, tDWORD down );
//static tVOID    pr_call _DisplaceFromCache( tHANDLE* handle, tHANDLE* item );
static tHANDLE* pr_call _HandleIntFind( tHANDLE* handle, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags, tDWORD* c_up, tDWORD* c_down );


// ---
tHANDLE* pr_call _HandleFind( tPO* po, tHANDLE* handle, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags ) {
	tDWORD c_up;
	tDWORD c_down;
	tHANDLE* found;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _HandleFind" );
	
	// check cache for the handle
	found = _FindInCache( handle, iid, pid, up, down );
	if ( !found ) {
		
		c_up = c_down = 0;
		found = _HandleIntFind( handle, iid, pid, up, down, flags, &c_up, &c_down );
		
		// cache it
		if ( found )
			_AddToCache( po, handle, found, c_up, c_down );
	}
	
	PR_TRACE_A1( MakeObject(handle), "Leave _HandleFind ret tHANDLE* = %p", found );
	return found;
}



#define HALF       ((8*sizeof(tDWORD))/2)
#define UP(d)      ((d) & (((tDWORD)-1) >> HALF))
#define DOWN(d)    ((d) >> HALF)
       

// ---
static tHANDLE* pr_call _FindInCache( tHANDLE* handle, tIID iid, tPID pid, tDWORD up, tDWORD down ) {
	tERROR    error;
	tHANDLE*  found;
	tHANDLEX*	ext;
	hOBJECT   fobj;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _FindInCache" );
	
	if ( 0 == (ext=handle->ext) )
		;
	else if ( 0 == (found=ext->find_data1) )
		;
	else if ( up > UP(ext->find_data2) )
		found = 0;
	else if ( down < DOWN(ext->find_data2) )
		found = 0;
	else if ( fobj=MakeObject(found), !_HC(found,0,fobj,&error) )
		found = 0;
	else if ( !CHECK_C(found,iid,pid) )
		found = 0;
	
	PR_TRACE_A1( MakeObject(handle), "Leave _FindInCache ret tHANDLE* = %p", found );
	return found;
}


// ---
static tVOID pr_call _AddToCache( tPO* po, tHANDLE* handle, tHANDLE* found, tDWORD up, tDWORD down ) {
	PR_TRACE_A0( MakeObject(handle), "Enter _AddToCache" );
	
	wlock(po);
	if ( PR_SUCC(_chk_ext_0(handle)) ) {
		handle->ext->find_data1 = found;
		handle->ext->find_data2 = (up & (((tDWORD)-1) >> HALF)) | (down << HALF);
	}
	wunlock(po);
	
	PR_TRACE_A0( MakeObject(handle), "Leave _AddToCache ret tVOID" );
}



// ---
/*
static tVOID pr_call _DisplaceFromCache( tHANDLE* handle, tHANDLE* item ) {
PR_TRACE_A0( MakeObject(handle), "Enter _DisplaceFromCache" );

	if ( *((tHANDLE**)&handle->find_data1) == item )
	handle->find_data1 = handle->find_data2 = 0;
	
		PR_TRACE_A0( MakeObject(handle), "Leave _DisplaceFromCache ret tVOID" );
}
*/



// ---
static tHANDLE* pr_call _HandleIntFind( tHANDLE* handle, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags, tDWORD* c_up, tDWORD* c_down ) {

	tHANDLE* f;
	tBOOL    r, c;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _HandleIntFind" );
	
	f = 0;
	c = (flags & OFF_CHILD_FIRST) ? cTRUE : cFALSE;
	
	if ( flags & OFF_ITSELF ) {
		r = cTRUE;
		flags &= ~OFF_ITSELF;
	}
	else
		r = cFALSE;
	
	if ( !c && r && CHECK_C(handle,iid,pid) ) 
		f = handle;
	
	else if ( down > 1 ) {
		tHANDLE* t;
		f = handle->child;
		if ( c ) {
			while( f ) {
				if ( CHECK_C(f,iid,pid) )
					break;
				f = f->next;
			}
			if ( f ) {
				if ( c_down )
					(*c_down)++;
			}
			else {
				down--;
				f = handle->child;
				if ( c_down )
					(*c_down)++;
				while ( f ) {
					t = _HandleIntFind( f, iid, pid, 0, down, flags, c_up, c_down );
					if ( t ) {
						f = t;
						break;
					}
					f = f->next;
				}
				if ( c_down && !f )
					(*c_down)--;
			}
		}
		else {
			while( f ) {
				t = _HandleIntFind( f, iid, pid, 0, down-1, flags, c_up, c_down );
				if ( t ) {
					f = t;
					break;
				}
				f = f->next;
			}
			if ( !f ) {
				f = handle->child;
				while( f ) {
					if ( CHECK_C(f,iid,pid) )
						break;
					f = f->next;
				}
				if ( f && c_down )
					(*c_down)++;
			}
		}
	}
	else if ( down ) {
		f = handle->child;
		while( f ) {
			if ( CHECK_C(f,iid,pid) )
				break;
			f = f->next;
		}
		if ( f && c_down )
			(*c_down)++;
	}
	
	if ( !f ) {
		if ( c && r && CHECK_C(handle,iid,pid) ) 
			f = handle;
		else if ( handle->parent && up ) {
			if ( c_up )
				(*c_up)++;
			f = _HandleIntFind( handle->parent, iid, pid, up-1, !!down, flags, c_up, c_down );
			if ( !f && c_up )
				(*c_up)--;
		}
	}
	
	PR_TRACE_A1( MakeObject(handle), "Leave _HandleIntFind ret tHANDLE* = %p", f );
	return f;
}

#undef HALF
#undef UP
#undef DOWN



// ---
tVOID pr_call _Remove( tHANDLE* handle ) {

	if ( handle->next )
		handle->next->prev = handle->prev;

	if ( handle->prev )
		handle->prev->next = handle->next;
	else if ( handle->parent )
		handle->parent->child = handle->next;

	handle->prev = 
	handle->next = 
	handle->parent = 0;
}



// ---
tVOID pr_call _AddBefore( tHANDLE* handle, tHANDLE* pos ) {
	if ( 0 == (handle->prev = pos->prev) ) {
		if ( pos->parent )
			pos->parent->child = handle;
	}
	else
		pos->prev->next = handle;
	handle->next = pos;
	pos->prev = handle;
	handle->parent = pos->parent;
}



// ---
tVOID pr_call _AddAfter( tHANDLE* handle, tHANDLE* pos ) {
	handle->prev = pos;
	if ( 0 != (handle->next = pos->next) )
		pos->next->prev = handle;
	pos->next = handle;
	handle->parent = pos->parent;
}



// ---
tVOID pr_call _AddToChildList( tHANDLE* handle, tHANDLE* parent ) {
	if ( 0 != (handle->next = parent->child) )
		handle->next->prev = handle;
	parent->child = handle;
	handle->prev = 0;
	handle->parent = parent;
}



// ---
tUINT IMPEX_FUNC(pr_handle_enum)( tHandleEnumFunc func, tVOID* params ) {
	tPO*      po;
	tHANDLE** pool;
	tUINT     pool_index;
	tUINT     handle_count = 0;
	
	enter_lf();
	rlock(po);
	for( pool=HandlePool,pool_index=0; (pool_index<HandlePoolCount) && *pool; ++pool_index,++pool ) {
		tHANDLE*  handle;
		tUINT     handle_index;
		for( handle=*pool,handle_index=0; handle_index<cHANDLE_ENTRIES; ++handle_index,++handle ) {
			if ( OBJ_IS_CONDEMNED(handle) )
				continue;
			++handle_count;
			if ( func )
				func( handle, params );
		}
	}
	runlock(po);
	leave_lf();
	return handle_count;
}



