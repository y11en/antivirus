/*-----------------17.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"
#include <m_utils.h>
#include "k_root.h"

#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_string.h>

// ---
hOBJECT pr_call System_ParentGet( hOBJECT object, tIID iid ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::ParentGet" );
	enter_lf();
	
	if ( iid == IID_ROOT ) {
		error = errOK;
		handle = g_hRoot;
	}
	
	else if ( 0 == _HCC(handle,po,lock,object,&error) )
		;
	
	else {
		if ( handle->parent )
			handle = handle->parent;
		else
			handle = g_hRoot;
		if ( iid != IID_ANY ) {
			while ( handle ) {
				if ( iid == GET_IID_C(handle) )
					break;
				handle = handle->parent;
			}
		}
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ParentGet ret hOBJECT = %p, %terr", MakeObject(handle), error );
	return MakeObject(handle);
}



// ---
tERROR pr_call System_ParentSet( hOBJECT object, hOBJECT* old_parent, hOBJECT new_parent ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* prev;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ParentSet" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( handle ) {
		tHANDLE* dad;
		if ( _HC(dad,0,new_parent,&error) )
			prev = _ParentSet( po, handle, dad, &error );
		else
			prev = handle->parent ? handle->parent : g_hRoot;
		runlock(po);
	}
	else
		prev = 0;
	
	if ( old_parent )
		*(hOBJECT*)old_parent = MakeObject(prev);
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ParentSet ret hOBJECT = %p, %terr", MakeObject(prev), error );
	return error;
}



// ---
tERROR pr_call System_ObjectCreate( hOBJECT object, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* new_handle;
	tPO*     po;
	
	PR_TRACE_A3( object, "Enter System::ObjectCreate(%tiid,%tpid,sub=%u)", iid, pid, subtype );
	enter_lf();
	
	new_handle = 0;
	if ( iid == IID_ANY )
		error = errPARAMETER_INVALID;
	else if ( 0 != _HC(handle,po,object,&error) ) {
		if ( handle->flags & hf_OBJECT_CLOSED )
			error = errOBJECT_IS_CLOSED;
		else
			error = _ObjectIIDCreate( po, handle, &new_handle, iid, pid, subtype );
		runlock(po);
		if ( PR_FAIL(error) )
			new_handle = 0;
	}
	
	if ( new_obj )
		*(hOBJECT*)new_obj = MakeObject(new_handle);
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ObjectCreate ret hOBJECT = %p, %terr", MakeObject(new_handle), error );
	return error;
}




// ---
tERROR pr_call System_ObjectCreateDone( hOBJECT object ) {
	
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::ObjectCreateDone" );
	enter_lf();
	
	_HCC( handle, po,lock, object, &error );
	if ( handle ) {
		error = _ObjectCreateDone( po, handle );
		runlockc(po,lock);
	}
	leave_lf();
	PR_TRACE_A1( object, "Leave System::ObjectCreateDone ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_ObjectRevert( hOBJECT object, tBOOL reuse_custom_prop ) {
	
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ObjectBackToCreateStage" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( handle ) {
		tHANDLE* parent = handle->parent ? handle->parent : g_hRoot;
		if ( parent->iface && parent->iface->internal->ChildNew ) {
			PR_TRACE(( object, prtERROR, "krn\tobject(%tiid) cannot be reverted because its parent(%tiid) has ChildNew method implemented", GET_IID_C(handle), GET_IID_C(parent) ));
			error = errOBJECT_CANNOT_BE_REVERTED;
		}
		else if ( !handle->iface ) {
			PR_TRACE(( object, prtERROR, "krn\tobject cannot be reverted because it has no interface set yet" ));
			error = errOBJECT_BAD_INTERNAL_STATE;
		}
		//		else if ( handle->iface->internal->ObjectNew ) {
		//			PR_TRACE(( object, prtERROR, "krn\tobject(%tiid) cannot be reverted because it has ChildNew method implemented", handle->m_iid ));
		//			error = errOBJECT_CANNOT_BE_REVERTED;
		//		}
		else {
			tDATA iids[2] = {0};
			error = _ObjectRevert( po, handle, reuse_custom_prop, CALLER_INFO(object,iids) );
		}
		runlock(po);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::ObjectBackToCreateStage ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_ObjectCreateQuick( hOBJECT object, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* new_handle;
	tPO*     po;
	
	PR_TRACE_A3( object, "Enter System::ObjectCreateQuick(%tiid,%tpid,sub=%u)", iid, pid, subtype );
	enter_lf();
	
	new_handle = 0;
	
	if ( iid == IID_ANY ) 
		error = errPARAMETER_INVALID;
	
	else if ( 0 != _HC(handle,po,object,&error) ) {
		if ( handle->flags & hf_OBJECT_CLOSED )
			error = errOBJECT_IS_CLOSED;
		else if ( 
			PR_SUCC(error=_ObjectIIDCreate(po,handle,&new_handle,iid,pid,subtype)) &&
			PR_FAIL(error=_ObjectCreateDone(po,new_handle))
		) {
			tDATA iids[2] = {0};
			_ObjectClose( po, new_handle, CALLER_INFO(object,iids) );
		}
		runlock(po);
		if ( PR_FAIL(error) )
			new_handle = 0;
	}
	
	if ( new_obj )
		*(hOBJECT*)new_obj = MakeObject(new_handle);
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ObjectCreateQuick ret hOBJECT = %p, %terr", MakeObject(new_handle), error );
	return error;
}




// ---
tERROR pr_call System_ChildGetFirst( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* ret;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A2( object, "Enter System::ChildGetFirst(%tiid,%tpid)", iid, pid );
	enter_lf();
	
	_HCC( handle, po, lock, object, &error );
	if ( handle ) {
		ret = handle->child;
		error = errOBJECT_NOT_FOUND;
		while( ret ) {
			if ( CHECK_C(ret,iid,pid) ) {
				error = errOK;
				break;
			}
			else
				ret = ret->next;
		}
		runlockc(po,lock);
	}
	else
		ret = 0;
	
	if ( ret_obj )
		*ret_obj = MakeObject(ret);
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ChildGetFirst ret hOBJECT = %p, %terr", MakeObject(ret), error );
	return error;
}




// ---
tERROR pr_call System_ChildGetLast( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* ret;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A2( object, "Enter System::ChildGetLast(%tiid,%tpid)", iid, pid );
	enter_lf();
	
	_HCC( handle, po, lock, object, &error );
	if ( handle ) {
		ret = handle->child;
		while ( ret && ret->next )
			ret = ret->next;
		error = errOBJECT_NOT_FOUND;
		while( ret ) {
			if ( CHECK_C(ret,iid,pid) ) {
				error = errOK;
				break;
			}
			else
				ret = ret->prev;
		}
		runlockc(po,lock);
	}
	else
		ret = 0;
	if ( ret_obj )
		*ret_obj = MakeObject(ret);
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ChildGetLast ret hOBJECT = %p, %terr", MakeObject(ret), error );
	return error;
}



// ---
tERROR pr_call System_ObjectGetNext( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* ret;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A2( object, "Enter System::ObjectGetNext(%tiid,%tpid)", iid, pid );
	enter_lf();
	
	_HCC( handle, po, lock, object, &error );
	if ( handle ) {
		ret = handle->next;
		error = errOBJECT_NOT_FOUND;
		while( ret ) {
			if ( CHECK_C(ret,iid,pid) ) {
				error = errOK;
				break;
			}
			else
				ret = ret->next;
		}
		runlockc(po,lock);
	}
	else
		ret = 0;
	
	if ( ret_obj )
		*ret_obj = MakeObject(ret);
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ObjectGetNext ret hOBJECT = %p, %terr", MakeObject(ret), error );
	return error;
}



// ---
tERROR pr_call System_ObjectGetPrev( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* ret;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A2( object, "Enter System::ObjectGetPrev(%tiid,%tpid)", iid, pid );
	enter_lf();
	
	_HCC( handle, po, lock, object, &error );
	if ( handle ) {
		ret = handle->prev;
		error = errOBJECT_NOT_FOUND;
		while( ret ) {
			if ( CHECK_C(ret,iid,pid) ) {
				error = errOK;
				break;
			}
			else
				ret = ret->prev;
		}
		runlockc(po,lock);
	}
	else
		ret = 0;
	
	if ( ret_obj )
		*ret_obj = MakeObject(ret);
	leave_lf();
	PR_TRACE_A2( object, "Leave System::ObjectGetPrev ret hOBJECT = %p, %terr", MakeObject(ret), error );
	return error;
}



// ---
tERROR pr_call _ObjectNotInitializedDummy( hOBJECT _this );
tERROR pr_call _ObjectFreedDummy( hOBJECT _this );




// ---
tERROR pr_call System_ObjectCheck( hOBJECT obj, hOBJECT object, tIID iid, tPID pid, tDWORD subtype, tBOOL do_not_check_int_struct ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( obj, "Enter System::ObjectCheck" );
	enter_lf();
	
	//  handle = _HandleCheck_real( po, object, &error );
	
	if ( do_not_check_int_struct )
		_HCC(handle, po, lock, object, &error);
	else
		handle = _HandleCheck_real( po, object, &error );
	
	if ( handle ) {
		if ( !handle->iface )
			error = errINTERFACE_NOT_ASSIGNED_YET;
		
		else if ( handle->flags & (hf_OBJECT_CLOSED|hf_OBJECT_IS_CLOSING) ) // _is_vtbl(handle,_ObjectFreedDummy)
			error = errOBJECT_ALREADY_FREED;
		
		/*
		else if ( !handle->iface || !(handle->flags & hf_OPERATIONAL_MODE) )
			error = errOBJECT_NOT_INITIALIZED;
		*/
		
		else {
			tDWORD this_case = (iid != IID_ANY) + ((pid != PID_ANY) << 1) + ((subtype != SUBTYPE_ANY)<<2);
			
			switch ( this_case ) {
				case 0: 
					error = errOK;
					break;
				case 1: // iid
					error = _InterfaceCompatible( handle->iface, iid );
					break;
				case 2: // pid
					if ( handle->iface && (pid == GET_PID(handle)) )
						error = errOK;
					else
						error = errINTERFACE_INCOMPATIBLE;
					break;
				case 3: // iid + pid
					if ( CHECK_C(handle,iid,pid) )
						error = errOK;
					else
						error = errINTERFACE_INCOMPATIBLE;
					break;
				case 4: // subtype  
					if ( handle->iface && (handle->iface->subtype == subtype) )
						error = errOK;
					else
						error = errINTERFACE_INCOMPATIBLE;
					break;
				case 5: // iid + subtype
					if ( CHECK_IID_C(handle,iid) && (handle->iface->subtype == subtype) )
						error = errOK;
					else
						error = errINTERFACE_INCOMPATIBLE;
					break;
				case 6: // pid + subtype
					if ( handle->iface && CHECK_PID(handle,pid) && (handle->iface->subtype == subtype) )
						error = errOK;
					else
						error = errINTERFACE_INCOMPATIBLE;
					break;
				case 7: // iid + pid + subtype
					if ( handle->iface && CHECK_IID(handle,iid) && CHECK_PID(handle,pid) && (handle->iface->subtype == subtype) )
						error = errOK;
					else
						error = errINTERFACE_INCOMPATIBLE;
					break;
			}
			
			if ( PR_SUCC(error) && (do_not_check_int_struct == cFALSE) && handle->iface && handle->iface->internal->ObjectCheck )
				CALL_INT_METH_0( handle, ObjectCheck, error )
		}
		if ( do_not_check_int_struct )
			runlockc(po,lock);
		else
			runlock(po);
	}
	
	leave_lf();
	PR_TRACE_A1( obj, "Leave System::ObjectCheck ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_ObjectValid( hOBJECT obj, hOBJECT object ) {
	
	tERROR error;
	tPO*   po;
	
	PR_TRACE_A0( obj, "Enter System::ObjectValid" );
	
	enter_lf();
	if ( _HandleCheck_real(po,object,&error) )
		runlock(po);
	leave_lf();
	
	PR_TRACE_A1( obj, "Leave System::ObjectValid ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_ObjectValidateLock( hOBJECT object, hOBJECT to_lock, tIID iid, tPID pid ) {
	
	tPO*   po;
	tERROR error;
	tHANDLE* handle;
	PR_TRACE_A0( obj, "Enter System::ValidateLock" );
	
	enter_lf();
	handle = _HandleCheck_real( po, to_lock, &error );
	if ( handle ) {
		if ( handle->flags & hf_OBJECT_IS_CLOSING )
			error = errOBJECT_IS_CLOSING;
		else if ( handle->flags & hf_OBJECT_CLOSED )
			error = errOBJECT_IS_CLOSED;
		else {
			if ( (iid != IID_ANY) || (pid != PID_ANY) )
				error = System_ObjectCheck( object, to_lock, iid, pid, SUBTYPE_ANY, cTRUE );
			if ( PR_SUCC(error) ) {
				tDATA iids[2] = {0};
				HANDLE_LOCK( CALLER_INFO(object,iids), handle, prtNOT_IMPORTANT, "lock" );
			}
		}
		runlock(po);
	}
	leave_lf();
	
	PR_TRACE_A1( obj, "Leave System::ValidateLock ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_ObjectRelease( hOBJECT object ) {
	
	tPO*   po;
	tINT   lock = 1;
	tERROR err;
	tHANDLE* handle;
	PR_TRACE_A0( obj, "Enter System::Release" );
	
	enter_lf();
	_HCC( handle, po, lock, object, &err );
	if ( handle ) {
		err = _ObjectRelease( po, handle );
		runlockc(po,lock);
	}
	leave_lf();
	
	PR_TRACE_A1( obj, "Leave System::Release ret %terr", error );
	return err;
}



// ---
tERROR pr_call System_ObjectClose( hOBJECT object ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ObjectClose" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( handle ) {
		tDATA iids[2] = {0};

		#if !defined(HANDLE_CHECKED)
			if ( handle->flags & hf_OBJECT_CLOSED )
				error = errOBJECT_ALREADY_FREED;
			else {
		#endif
			
				error = _ObjectClose( po, handle, CALLER_INFO(object,iids) );
			
		#if !defined(HANDLE_CHECKED)
			}
		#endif
		runlock(po);
	}
	
	leave_lf();
	PR_TRACE_A1( 0/*must be 0 - because object is freed now*/, "Leave System::ObjectClose ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_ObjectFind( hOBJECT object, hOBJECT* result, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags ) {
	
	tERROR error;
	tHANDLE* handle;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ObjectGet" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( handle ) {
		tHANDLE* found = _HandleFind( po, handle, iid, pid, up, down, flags );
		runlock(po);
		
		if ( found ) {
			if ( result )
				*result = MakeObject( found );
		}
		else {
			if ( result )
				*result = 0;
			error = errOBJECT_NOT_FOUND;
		}
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::ObjectGet ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_ObjectSwap( hOBJECT object, hOBJECT o1, hOBJECT o2 ) {
	
	tERROR error;
	tHANDLE* handle;
	tHANDLE* h1;
	tHANDLE* h2;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ObjectSwap" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( !handle ) 
		;
	else if ( !o1 || !o2 ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::ObjectSwap - invalid parameter" ));
	}
	else if ( o1 == o2 ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::ObjectSwap - swap objects are equal" ));
	}
	else if ( (o1 == object) || (o2 == object) ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::ObjectSwap - parameter is equal to \"this\"" ));
	}
	else if ( !_HC(h1,0,o1,&error) || !_HC(h2,0,o2,&error) ) {
		PR_TRACE(( object, prtERROR, "krn\tSystem::ObjectSwap - one of the parameters is not an object" ));
	}
	else if ( GET_IID_C(h1) != GET_IID_C(h2) ) {
		error = errINTERFACE_INCOMPATIBLE;
		PR_TRACE(( object, prtERROR, "krn\tSystem::ObjectSwap - incompatible interface" ));
	}
	else {
		tHANDLE tmp;
		
		wlock(po);
		_ClearMsgHandlerCache( h1 );
		_ClearMsgHandlerCache( h2 );

		// handle hierarchy related data - must stay unganged
		#define SWAPF( h1, h2, f )  if ( h1->f != h2->f ) {tmp.f = h2->f; h2->f = h1->f; h1->f = tmp.f;}
		SWAPF( h1, h2, obj );
		SWAPF( h1, h2, iface );
		SWAPF( h1, h2, custom_prop );
		SWAPF( h1, h2, custom_prop_count );

		SWAPF( h1, h2, flags );
		SWAPF( h1, h2, ref );
		SWAPF( h1, h2, init_props );
		SWAPF( h1, h2, ext );

		SWAPF( h1, h2, mem );
		SWAPF( h1, h2, sync );
		SWAPF( h1, h2, tracer );
		SWAPF( h1, h2, trace_level.max );
		SWAPF( h1, h2, trace_level.min );

		if ( (h1->flags & hf_OBJECT_IS_CLOSING) != (h2->flags & hf_OBJECT_IS_CLOSING) ) {
			if ( h1->flags & hf_OBJECT_IS_CLOSING ) {
				h1->flags &= ~hf_OBJECT_IS_CLOSING;
				h2->flags |= hf_OBJECT_IS_CLOSING;
			}
			else {
				h2->flags &= ~hf_OBJECT_IS_CLOSING;
				h1->flags |= hf_OBJECT_IS_CLOSING;
			}
		}
		wunlock(po); 
	}
	
	if ( handle )
		runlock(po);
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::ObjectSwap ret %terr", error );
	return error;
}



#define STREAM_SIGNATURE   0x9d4ee2a8  
#define STREAM_VERSION     1
#define STREAM_HEADER_SIZE (sizeof(tDWORD)/*signature*/ + sizeof(tDWORD)/*version*/ + sizeof(tDWORD)/*obj_size*/ + sizeof(tDWORD)/*stream size*/)


// ---
tERROR pr_call System_ObjectGetInternalData( hOBJECT object, tDWORD* size, tDWORD obj_size, tBYTE* data, tDWORD data_size, tObjSerializeFunc convert, tObjCheckCustomPropFunc prop_enum, tPTR ctx ) {
	tERROR   err;
	tHANDLE* handle;
	tDWORD   stream_size = 0;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ObjectGetInternalData" );
	enter_lf();
	
	if ( !obj_size )
		obj_size = sizeof(hOBJECT);
	
	if ( !data != !data_size )
		err = errPARAMETER_INVALID;
	
	else if ( data && !convert && (obj_size!=sizeof(hOBJECT)) )
		err = errPARAMETER_INVALID;
	
	else if ( 0 == _HC(handle,po,object,&err) )
		;
	
	else {
		tDWORD* pstream_size = 0;
		tDWORD  custom_prop_size = 0;
		tDWORD  msg_handlers_size = 0;
		
		stream_size = STREAM_HEADER_SIZE;
		
		if ( data ) {
			TYPE_ASSIGN( tDWORD, data, STREAM_SIGNATURE );
			TYPE_ASSIGN( tDWORD, data, STREAM_VERSION );
			TYPE_ASSIGN( tDWORD, data, obj_size );         pstream_size = (tDWORD*)data;
			TYPE_ASSIGN( tDWORD, data, 0 );
			data_size -= STREAM_HEADER_SIZE;
		}
		
		err = _CustomPropertyGetAll( po, handle, obj_size, &custom_prop_size, &data, &data_size, convert, prop_enum, ctx );
		if ( PR_SUCC(err) )
			err = _GetMsgHandlerList( po, handle, obj_size, &msg_handlers_size, &data, &data_size, convert, ctx );
		runlock(po);
		
		if ( PR_SUCC(err) && pstream_size )
			*pstream_size = custom_prop_size + msg_handlers_size;
		stream_size += custom_prop_size + msg_handlers_size;
	}
	
	if ( size )
		*size = stream_size;
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::ObjectGetInternalData ret %terr", err );
	return err;
}




// ---
static tERROR pr_call _ConvertObjects( tPO* po, tHANDLE* handle, const tBYTE* data, tUINT count, tUINT obj_size, hOBJECT** obj_arr, tUINT obj_arr_count, tObjDeserializeFunc convert, tPTR ctx ) {
	tUINT         i, j, c;
	tBYTE*        b;
	tPROPID       p;
	tDWORD        s;
	tERROR        e;
	
	e = errOK;
	for( i=0, c=0, b=(tBYTE*)data; i<count; i++ ) {
		p = TYPE_GET( tPROPID, b );
		s = TYPE_GET( tDWORD, b );
		
		if ( IS_PROP_TYPE(p,pTYPE_STRING) || IS_PROP_TYPE(p,pTYPE_WSTRING) )
			TYPE_GET( tCODEPAGE, b );
		
		else if ( IS_PROP_TYPE(p,pTYPE_OBJECT) ) {
			c++;
			s = obj_size;
		}
		b += s;
	}
	
	if ( (obj_arr_count < c) && PR_FAIL(e=PrAlloc((tPTR*)obj_arr,c*sizeof(hOBJECT))) )
		return e;

	_PR_ENTER_PROTECTED_0(po) {
		hOBJECT* po = *obj_arr;
		for( i=0, j=0, b=(tBYTE*)data; i<count; i++ ) {
			p = TYPE_GET( tPROPID, b );
			s = TYPE_GET( tDWORD,  b );
			
			if ( IS_PROP_TYPE(p,pTYPE_STRING) || IS_PROP_TYPE(p,pTYPE_WSTRING) )
				TYPE_GET( tCODEPAGE, b );
			
			else if ( IS_PROP_TYPE(p,pTYPE_OBJECT) ) {
				if ( PR_FAIL(e=convert(MakeObject(handle),b,po++,ctx)) )
					break;
				s = obj_size;
			}
			b += s;
		}
	} _PR_LEAVE_PROTECTED_0( po, e );
	return e;
}



// ---
tERROR pr_call System_ObjectSetInternalData( hOBJECT object, tDWORD* size, tDWORD obj_size, const tBYTE* data, tDWORD data_size, tObjDeserializeFunc convert, tPTR ctx ) {
	tERROR   err;
	tHANDLE* handle = 0;
	tBOOL    wlocked = cFALSE;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::ObjectGetInternalData" );
	enter_lf();
	
	if ( !obj_size )
		obj_size = sizeof(hOBJECT);
	
	if ( !data )
		err = errPARAMETER_INVALID;
	
	else if ( !convert && (obj_size!=sizeof(hOBJECT)) )
		err = errPARAMETER_INVALID;
	
	else if ( data_size <= STREAM_HEADER_SIZE )
		err = errBUFFER_TOO_SMALL;
	
	else if ( STREAM_SIGNATURE != TYPE_GET(tDWORD,data) ) 
		err = errBAD_SIGNATURE;
	
	else if ( STREAM_VERSION != TYPE_GET(tDWORD,data) )
		err = errBAD_VERSION;
	
	else if ( obj_size != TYPE_GET(tDWORD,data) )
		err = errBAD_SIZE;
	
	else if ( data_size < (TYPE_GET(tDWORD,data)+STREAM_HEADER_SIZE) )
		err = errBUFFER_TOO_SMALL;
	
	else if ( 0 == _HC(handle,po,object,&err) )
		;
	
	else {
		tUINT    i, j;
		tDATA    out;
		tPROPID  pid;
		tDWORD   siz;
		hOBJECT  objs[32];
		hOBJECT* pobj = objs;
		hOBJECT* custom_prop_obj   = 0;
		tUINT    custom_prop_count = 0;
		tBYTE*   custom_prop_data  = 0;
		tDWORD   custom_prop_size  = 0;
		tDWORD   msg_handlers_size = 0;
		
		tNotificationSink* sink;
		tUINT              sink_count;
		tExceptionReg      reg;
		
		//tCustProp*         cp = handle->custom_prop;
		
		data_size -= STREAM_HEADER_SIZE;
		
		// perform 
		// _CustomPropertySetAll( tHANDLE* handle, tUINT obj_size, tDWORD* out_size,   tBYTE** buffer, tDWORD* buff_size, tObjDeserializeFunc func, tPTR ctx );
		// _CustomPropertySetAll( handle,                obj_size, &custom_prop_size, (tBYTE**)&data, &data_size,        convert,                   ctx );
		
		if ( data_size < CPLIST_HEAD_SIZE )
			err = errBUFFER_TOO_SMALL;
		
		else if ( CPLIST_SIGNATURE != TYPE_GET(tDWORD,data) ) {
			custom_prop_size += sizeof(tDWORD);
			err = errBAD_SIGNATURE;
		}
		
		else if ( CPLIST_VERSION != TYPE_GET(tDWORD,data) ) {
			custom_prop_size += sizeof(tDWORD);
			err = errBAD_VERSION;
		}
		
		else {
			
			tUINT size;
			
			custom_prop_count = TYPE_GET( tDWORD, data );
			size  = TYPE_GET( tDWORD, data );
			
			// ??? _CustomPropertyDeleteList( handle );
			
			if ( data_size < size )
				err = errBUFFER_TOO_SMALL;
			
			if ( convert && PR_FAIL(err=_ConvertObjects(po,handle,data,custom_prop_count,obj_size,&pobj,countof(objs),convert,ctx)) )
				;
			
			else {
				
				custom_prop_obj = pobj;
				custom_prop_data = (tBYTE*)data;
				custom_prop_size += 4 * sizeof(tDWORD);
				
				wlocked = cTRUE;
				wlock(po);
				
				for( i=0; i<custom_prop_count; i++ ) {
					pid = TYPE_GET( tPROPID, data );
					siz = TYPE_GET( tDWORD,  data );
					
					custom_prop_size += 2 * sizeof(tDWORD);
					
					if ( IS_PROP_TYPE(pid,pTYPE_STRING) || IS_PROP_TYPE(pid,pTYPE_WSTRING) ) {
						tCODEPAGE codepage = TYPE_GET( tCODEPAGE, data );
						custom_prop_size += sizeof(tDWORD);
						err = _CustomPropertySetStr( po, handle, &out, pid, (tBYTE*)data, siz, codepage, cFALSE );
					}
					else if ( IS_PROP_TYPE(pid,pTYPE_OBJECT) && convert ) {
						err = _CustomPropertySet( po, handle, &out, pid, custom_prop_obj++, siz, cFALSE );
						siz = obj_size;
					}
					else
						err = _CustomPropertySet( po, handle, &out, pid, (tBYTE*)data, siz, cFALSE );
					
					if ( PR_FAIL(err) )
						break;
					
					data += siz;
					custom_prop_size += siz;
				}
			}
		}
		
		if ( PR_SUCC(err) )
			err = _SetMsgHandlerList( po, handle, obj_size, &msg_handlers_size, (tBYTE**)&data, &data_size, convert, ctx );
		
		if ( wlocked )
			wunlock(po);
		
		// call sinks if they are
		if ( PR_SUCC(err) && (handle->flags & hf_CALLBACK_SENDER) && PR_SUCC(_ns_get_arr(cSNS_CUSTOM_PROP_SET,&sink,&sink_count)) && custom_prop_count && sink ) {
			runlock(po);
			
			if ( errOK == si.EnterProtectedCode(0,&reg) ) {
				tCODEPAGE cp;
				
				custom_prop_obj = pobj;
				for( i=0; i<custom_prop_count; i++ ) {
					tNotificationSink* s;
					tPTR               b;
					tUINT              l;
					
					pid = TYPE_GET( tPROPID, custom_prop_data );
					siz = TYPE_GET( tDWORD,  custom_prop_data );
					
					if ( IS_PROP_TYPE(pid,pTYPE_STRING) || IS_PROP_TYPE(pid,pTYPE_WSTRING) ) {
						cp = TYPE_GET( tCODEPAGE, custom_prop_data );
						b = custom_prop_data;
						l = siz;
					}
					else if ( IS_PROP_TYPE(pid,pTYPE_OBJECT) && convert ) {
						cp = cCP_NULL;
						b = custom_prop_obj++;
						l = sizeof(hOBJECT);
						siz = obj_size;
					}
					else {
						cp = cCP_NULL;
						b = custom_prop_data;
						l = siz;
					}
					
					for( j=0, s=sink; j<sink_count; j++,s++ )
						s->func( MakeObject(handle), pid, b, l, cp, s->ctx );
					custom_prop_data += siz;
				}
			}
			si.LeaveProtectedCode( 0, &reg );
		}
		else
			runlock(po);
		
		if ( size )
			*size = STREAM_HEADER_SIZE + custom_prop_size + msg_handlers_size;
	}

	leave_lf();
	PR_TRACE_A1( object, "Leave System::ObjectGetInternalData ret %terr", err );
	return err;
}



// -------------------------------------------------------------------------------------
// internal
// -------------------------------------------------------------------------------------
// ---
tDWORD pr_call _HandleChildCheck( tHANDLE* dad, tHANDLE* obj, tDWORD go_down ) {
	
	tHANDLE* tmp;
	tDWORD   ret = go_down;
	
	PR_TRACE_A1( MakeObject(dad), "Enter _HandleChildCheck down = %u", go_down );
	
	if ( go_down-- ) {
		tmp = dad->child;
		while( tmp && !ret ) {
			if ( tmp == obj )
				ret = go_down;
			else if ( go_down && tmp->child ) 
				ret = _HandleChildCheck( tmp->child, obj, go_down );
			else
				tmp = tmp->next;
		}
	}
	
	PR_TRACE_A1( MakeObject(dad), "Leave _HandleChildCheck ret tDWORD = %u", ret );
	return ret;
}


tERROR pr_call _ImportStringService();

// ---
static tBOOL pr_call _find_string_iface( tINTERFACE* iface, tPTR param ) {
	if ( iface->iid == IID_STRING )
		return cTRUE;
	return cFALSE;
}


// ---
tHANDLE* pr_call _ParentSet( tPO* po, tHANDLE* handle, tHANDLE* dad, tERROR* error ) {
	tHANDLE* old;
	
	PR_TRACE_A0( MakeObject(dad), "Enter _ParentSet" );
	
	old = handle->parent;
	if ( !old ) {
		*error = errOBJECT_INCOMPATIBLE;
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tROOT object cannot be reparent" ));
	}
	
	else if ( handle == dad ) {
		*error = errOBJECT_INCOMPATIBLE;
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tAttempt to set self object as new parent" ));
	}
	
	else if ( OF_ALL_LEVELS - _HandleChildCheck(handle,dad,OF_ALL_LEVELS) ) { // dad is really a child
		*error = errOBJECT_INCOMPATIBLE;
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tAttempt to set a child as new parent" ));
	}
	
	else if ( handle->flags & (hf_OBJECT_CLOSED/*|hf_OBJECT_IS_CLOSING*/) ) {
		*error = errOBJECT_INCOMPATIBLE;
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tAttempt to set new parent to the object condemned to close" ));
	}
	
	else {
		
		tPID  pid;
		tDATA size;
		tBOOL check;
		*error = errOK;
		
		wlock(po);
		_Remove( handle );
		
		// add to new parent list
		if ( CHECK_IID_C(dad,IID_ROOT) && CHECK_IID_C(handle,IID_PLUGIN) ) { 
			// special case: "plugin" under "root" object
			// kernel has a special place to put it -- g_plugins list
			handle->parent = 0;
			if ( g_plugins )
				_AddBefore( handle, g_plugins );
			g_plugins = handle;
			check = cTRUE;
		}
		else {
			check = cFALSE;
			_AddToChildList( handle, dad );
		}
		wunlock(po);
		
		if ( check && PR_SUCC(_PropertyGetSync(po,handle,&size,pgMODULE_ID,&pid,sizeof(pid))) && (pid==PID_STRING) ) {
			tDWORD cookie = 0;
			tINTERFACE* iface = _InterfaceFindInt( &cookie, _find_string_iface, 0 );
			if ( iface && !iface->plugin )
				iface->plugin = (hPLUGIN)MakeObject(handle);
			_ImportStringService();
		}
	}
	
	PR_TRACE_A1( MakeObject(handle), "Leave _ParentSet ret tHANDLE* = %p (old parent)", old );
	return old;
}



#define WANT_PROTECTION( handle, iface )                             \
  ( (((iface)->flags & IFACE_PROTECTED_BY_CS) ? IID_CRITICAL_SECTION : ( ((iface)->flags & IFACE_PROTECTED_BY_MUTEX) ? IID_MUTEX : IID_ANY)) )

/* #define WANT_PROTECTION( handle, iface )                           \ */
/*   ( (handle)->static_link ? IID_ANY : (((iface)->flags & IFACE_PROTECTED_BY_CS) ? IID_CRITICAL_SECTION : ( ((iface)->flags & IFACE_PROTECTED_BY_MUTEX) ? IID_MUTEX : IID_ANY)) ) */


#define SET_PROTECTION( handle, iface )                              \
	if ( (iface)->flags & IFACE_PROTECTED_BY_CS ) {                    \
		(handle)->flags |= hf_OBJECT_PROTECTED;                \
		_SyncCreate( po, (handle), 0, IID_CRITICAL_SECTION, PID_ANY, 0 );\
	}                                                                  \
	else if ( (iface)->flags & IFACE_PROTECTED_BY_MUTEX ) {            \
		(handle)->flags |= hf_OBJECT_PROTECTED;                \
		_SyncCreate( po, (handle), 0, IID_MUTEX, PID_ANY, 0 );           \
	}


#if defined(_PRAGUE_TRACE_)
	#define STR(s) (s)
#else
	#define STR(s) ("")
#endif


// ---
static tERROR pr_call _FinalizeExternalCreating( tPO* po, hOBJECT obj, tIID iid, tHANDLE** new_handle, tERROR error, tSTRING msg ) {
	
	tHANDLE* handle;
	tERROR error_to_forget;
	
	if ( PR_SUCC(error) ) {
		
		if ( !obj ) {
			PR_TRACE(( 0, prtERROR, "krn\tExternal object creating SUCCESSFUL but object is NULL" ));
		}
		
		if ( !_HC(handle,0,obj,&error) )
			PR_TRACE(( 0, prtIMPORTANT, "krn\t%s method returns bad object (ObjectValid error is %terr", msg, error ));
		
		else if ( !handle->iface ) {
			error = errOBJECT_NOT_INITIALIZED;
			PR_TRACE(( obj, prtIMPORTANT, "krn\t%s method returns object without InterfaceSet succeed", msg ));
		}
		
		else if ( PR_FAIL(error=_InterfaceCompatible(handle->iface,iid)) ) {
			PR_TRACE(( obj, prtERROR, "krn\t%s method returns incompatible object", msg ));
		}
		
		else {
			tIID iid = WANT_PROTECTION( handle, handle->iface );
			if ( iid != IID_ANY ) {
				wlock(po);
				handle->flags |= hf_OBJECT_PROTECTED;
				_SyncCreate( po, (handle), 0, iid, PID_ANY, 0 );
				wunlock(po);
			}
			if ( handle->iface->internal->ObjectInit )
				CALL_INT_METH_0( handle, ObjectInit, error );
		}
	}
	
	else if ( error == errOBJECT_NOT_CREATED ) {
		error = errOK;
		if ( obj && _HC(handle,0,obj,&error_to_forget) )
			_ObjectClose( po, handle, 0 );
		handle = 0;
	}
	
	else if ( obj )
		_HC( handle, 0, obj, &error_to_forget );
	
	else
		handle = 0;
	
	if ( PR_SUCC(error) )
		*new_handle = handle;
	else { 
		*new_handle = 0;
		if ( handle )
			_ObjectClose( po, handle, 0 );
	}
	
	return error;
}



//// ---
//static tERROR pr_call _ObjectCreateStaticClient( tPO* po, tHANDLE* handle, tHANDLE* host, tHANDLE** new_handle ) {
//  tERROR error;
//  
//  error = _HandleCreate( po, handle, new_handle, 0 );
//  if ( PR_SUCC(error) ) {
//    tINTERFACE* iface = host->iface;
//    
//    wlock(po);
//    mcpy( &(*new_handle)->flags, &host->flags, sizeof(tHANDLE)-((tDWORD)(&((tHANDLE*)0)->flags)) );
//    
//    (*new_handle)->m_iid = iface->iid;
//    (*new_handle)->flags &= ~hf_HIDDEN;
//    (*new_handle)->static_link = host;
//    (*new_handle)->lock_count = 1;
//    
//    host->lock_count++;
//    iface->obj_count++;
//    wunlock(po);
//    
//    if ( iface->internal->ObjectInit ) {
//      CALL_INT_METH_0( *new_handle, ObjectInit, error );
//    }
//    
//    if ( PR_FAIL(error) ) {
//      _ObjectClose( po, *new_handle );
//      *new_handle = 0;
//    }
//  }
//  return error;
//}




// ---
//static tERROR pr_call _ObjectCreateStatic( tPO* po, tHANDLE* handle, tINTERFACE* iface, tHANDLE** new_handle ) {
//  tERROR   error;
//  tHANDLE* host = g_statics;
//  
//  while( host ) {
//    if ( (host->iface == iface) /*VIK 12.02.03 && (host->flags & hf_HIDDEN)*/ ) // found!!! so we have to make a link to a static host
//      return _ObjectCreateStaticClient( po, handle, host, new_handle );
//    host = host->next;
//  }
//  
//  // if there is no static host found -> we'll create a new one
//  if ( iface->internal->ObjectNew ) {
//    hOBJECT obj;
//    CALL_INT_OBJ_NEW( handle, iface, error, &obj );
//    if ( PR_FAIL(error=_FinalizeExternalCreating(po,obj,iface->iid,&host,error,STR("ObjectNew"))) )
//      return error;
//  }
//  
//  else if ( PR_FAIL(error=_HandleCreate(po,g_hRoot,&host,0)) ) 
//    return error;
//  
//  else {
//    tIID iid = WANT_PROTECTION( host, iface );
//    if ( iid != IID_ANY ) {
//      wlock(po);
//      handle->flags |= hf_OBJECT_PROTECTED;
//      _SyncCreate( po, (handle), 0, iid, PID_ANY, 0 );
//      wunlock(po);
//    }
//    if ( PR_FAIL(error=_InterfaceSetNew(po,host,iface)) ) {
//      _ObjectClose( po, host );
//      return error;
//    }
//  }
//  
//  wlock(po);
//  _Remove( host );
//  if ( g_statics )
//    _AddBefore( host, g_statics ); // and place it as a brother of the root
//  g_statics = host;
//  wunlock(po);
//  
//  return _ObjectCreateStaticClient( po, handle, host, new_handle );
//}




// ---
tERROR pr_call _ObjectCreate( tPO* po, tHANDLE* handle, tHANDLE** new_handle, tINTERFACE* iface ) {
	
	hOBJECT obj;
	tERROR  error;
	PR_TRACE_A0( MakeObject(handle), "Enter _ObjectCreate" );
	
	obj = 0;
	*new_handle = 0;
	error = _InterfaceLoad( po, iface );

	if ( PR_SUCC(error) && (handle == g_hRoot) && (iface->iid == IID_STRING) )
		handle = MakeHandle(g_heap);
	
	if ( PR_SUCC(error) && handle->iface && handle->iface->internal->ChildNew ) {
		CALL_CHILD_NEW( handle, error, &obj, iface->iid, GET_PID(handle), iface->subtype );
		error = _FinalizeExternalCreating( po, obj, iface->iid, new_handle, error, STR("Parent::ChildNew") );
	}
	
	if ( PR_FAIL(error) )
		;
	else if ( *new_handle ) {
		PR_TRACE(( MakeObject(*new_handle), prtNOT_IMPORTANT, "krn\tObject created by ChildNew(%tiid,%tpid)", iface->iid, iface->pid));
	}
	else if ( PR_SUCC(error=_HandleCreate(po,handle,new_handle)) ) {
		if ( !*new_handle ) {
			PR_TRACE(( MakeObject(*new_handle), prtNOT_IMPORTANT, "krn\tHandleCreate returns errOK but handle is Zero" ));
		}
		else {
			tIID iid = WANT_PROTECTION( *new_handle, iface );
			if ( iid != IID_ANY ) {
				wlock(po);
				handle->flags |= hf_OBJECT_PROTECTED;
				_SyncCreate( po, (handle), 0, iid, PID_ANY, 0 );
				wunlock(po);
			}
			error = _InterfaceSetNew( po, *new_handle, iface );
			if ( PR_SUCC(error) && PSYNC_ON(handle) )
				(*new_handle)->flags |= hf_PROP_SYNCHRONIZED;
			if ( PR_SUCC(error) && MSYNC_ON(handle) )
				(*new_handle)->flags |= hf_MEM_SYNCHRONIZED;
		}
	}
	
	if ( PR_FAIL(error) ) {
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tCannot create object by %tiid. Error is %terr", iface->iid, error ));
		if ( *new_handle ) {
			_ObjectClose( po, *new_handle, 0 );
			*new_handle = 0;
		}
		_InterfaceUnload( po, iface );
	}
	else {
		PR_TRACE(( MakeObject(*new_handle), prtSPAM, "krn\tObject successfully created (%tiid,%tpid)", iface->iid, iface->pid));
	}
	
//exit_lbl:;
	PR_TRACE_A2( MakeObject(handle), "Leave _ObjectCreate ret hOBJECT = %p, %terr", *new_handle, error );
	return error;
}




// ---
tERROR pr_call _ObjectIIDCreate( tPO* po, tHANDLE* handle, tHANDLE** new_handle, tIID iid, tPID pid, tDWORD subtype ) {
	
	tDWORD      pos;
	tINTERFACE* iface;
	tERROR      error;
	
	PR_TRACE_A2( MakeObject(handle), "Enter _ObjectIIDCreate (%tiid, %tpid)", iid, pid );
	
	pos = 0;
	error = errINTERFACE_NOT_FOUND;
	iface = _InterfaceFind( &pos, iid, pid, subtype, VID_ANY, cFALSE );
	
	if ( !iface && (pid != PID_ANY) ) { // find out any compatible with "iid" interface in this plugin
		tDWORD cookie = 0;
		iface = _InterfaceFind( &cookie, IID_ANY, pid, subtype, VID_ANY, cFALSE );
		while( iface ) {
			if ( PR_SUCC(_InterfaceCompatible(iface,iid)) )
				break;
			iface = _InterfaceFind( &cookie, IID_ANY, pid, subtype, VID_ANY, cFALSE );
		}
	}
	
	//  while( iface ) {
	//    if ( errINTERFACE_NOT_LOADED == (error=_ObjectCreate(po,handle,new_handle,iface)) )
		//			iface = _InterfaceFind( &pos, iid, pid, subtype, VID_ANY, cFALSE );
	//		else
	//			break;
	//  }
	
	if ( iface )
		error = _ObjectCreate( po, handle, new_handle, iface );
	
	else {
		tDWORD ids[3];
		tDWORD size = sizeof(ids);
		
		ids[0] = iid;
		ids[1] = pid;
		ids[2] = subtype;
		
		PR_TRACE(( MakeObject(handle), prtDANGER, "krn\tOBJECT CANNOT BE CREATED because of interface (%tiid, %tpid) is not found", iid, pid ));
		_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_CREATE_FAILED_IFACE_NOT_FOUND, 0, ids, &size, 0 );
		*new_handle = 0;
	}
	
	PR_TRACE_A2( MakeObject(handle), "Leave _ObjectIIDCreate ret hOBJECT = %p, %terr", *new_handle, error );
	return error;
}





// ---
tERROR pr_call _ObjectCreateDone( tPO* po, tHANDLE* handle ) {
	tERROR error = errOK;
	
	PR_TRACE_A0( MakeObject(handle), "Enter _ObjectCreateDone" );
	
	if ( CHECK_IID_C(handle,IID_NONE) ) {
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tinterface doesn't assigned to the object" ));
		error = errINTERFACE_NOT_ASSIGNED_YET;
	}
	else if ( handle->init_props ) {
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\tProperty required for init doesn't set properly to the object [%tiid, %tpid], init_props = 0x%08x", GET_IID(handle), GET_PID(handle), handle->init_props ));
		error = errOBJECT_NOT_INITIALIZED;
	}
	else if ( handle->flags & hf_OPERATIONAL_MODE ) {
		PR_TRACE(( MakeObject(handle), prtNOTIFY, "krn\tObjectInitDone for the object has already been done successfully" ));
		error = errOK;
	}
	else {
		tHANDLE* parent;
		tIID     prot_iid;
		
		if ( handle->iface->internal->ObjectInitDone ) {
			CALL_INT_METH_0( handle, ObjectInitDone, error );
			if ( PR_FAIL(error) ) {
				PR_TRACE(( MakeObject(handle), prtERROR, "krn\tObject(%tiid,%tpid) initialization(ObjectInitDone) failed (%terr)", GET_IID(handle), GET_PID(handle), error ));
				goto exit_lbl;
			}
		}
		
		wlock(po);
		if ( PR_FAIL(_WrapperWithdraw(po,handle,NotInitializedMethod,0)) )
			_set_vtbl( handle, 0 );
		handle->flags |= hf_OPERATIONAL_MODE;
		wunlock(po);
		
		prot_iid = WANT_PROTECTION( handle, handle->iface );
		if ( prot_iid != IID_ANY )
			_SyncProtectObject( po, handle, prot_iid, PID_ANY, 0 );
		
		parent = handle->parent;
		if ( parent && parent->iface && parent->iface->internal->ChildInitDone ) {
			CALL_INT_METH_1( parent, ChildInitDone, error, MakeObject(handle) );
			if ( PR_FAIL(error) ) {
				handle->flags &= ~hf_OPERATIONAL_MODE;
				PR_TRACE(( MakeObject(handle), prtERROR, "krn\tObject initialization failed (ChildInitDone failed -- %terr)", error ));
			}
		}
	}
	
exit_lbl:;
	PR_TRACE_A1( MakeObject(handle), "Leave _ObjectCreateDone ret %terr", error );
	return error;
}




// ---
static tBOOL _remove_msg_handler( tHANDLE* handle, tPTR param ) {
  tUINT        i, c;
  tMHArray*    mha;
  tMsgHandler* h;
	tBOOL        clear;
	tHANDLEX*    ext;
	
	if ( OBJ_IS_CONDEMNED(handle) )
		return cFALSE;

	ext = handle->ext;
	if ( !ext )
		return cFALSE;

  mha = &ext->mh_arr;
	clear = cFALSE;
  for( i=0, c=mha->mcount, h=mha->parr; i<c; i++ ) {
    if ( (h->handler == param) && _mh_remove_by_ind(mha,i) ) {
      c--;
      h = mha->parr + i--;
      clear = cTRUE;
    }
    else
      h++;
  }

  if ( clear )
    _ClearMsgHandlerCache( handle );

  return cFALSE;
}



// ---
static tERROR pr_call _ObjectDeinit( tPO* po, tHANDLE* handle, tBOOL* moved, tDATA* pids ) {
	
	tERROR err;
	tERROR lerr;
	tHANDLE* parent;
	tHANDLEX* ext;
	tBOOL mem_obj, trace_obj, sync_obj;
	PR_TRACE_A0( MakeObject(handle), "Enter ObjectDeinit" );
	
	*moved = cFALSE;
	
//	if ( handle->flags & hf_OBJECT_IS_CLOSING ) {
//		PR_TRACE(( MakeObject(handle), prtFATAL, "krn\t!!! E R R O R -- OBJECT_IS_CLOSED_NOW !!! - Handle is in closing process (possible called from another thread)" ));
//		PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit !!!ERROR!!! - errOBJECT_IS_CLOSING" );
//		return errOBJECT_IS_CLOSING;
//	}
	
//	if( handle->flags & hf_OBJECT_CLOSED ) {
//		PR_TRACE(( MakeObject(handle), prtIMPORTANT, "krn\tOBJECT_IS_CLOSED - Handle is closed by another caller (possible in another thread)" ));
//		PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit - errOBJECT_IS_CLOSED" );
//		return errOBJECT_IS_CLOSED;
//	}
	
	parent = handle->parent;
	lerr = err = errOK;
	
	// call parent close notification
	if ( parent && parent->iface && parent->iface->internal->ChildClose ) {
		CALL_INT_METH_1( parent, ChildClose, lerr, MakeObject(handle) );
		if ( PR_FAIL(lerr) ) {
			tDWORD err_size = sizeof(tERROR);
			_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_CHILD_CLOSE_FAILED, parent, &lerr, &err_size, pids );
			err = lerr;
		}
		if ( parent != handle->parent ) {
			*moved = cTRUE;
			handle->flags &= ~(hf_OBJECT_CLOSED | hf_OBJECT_IS_CLOSING);
			PR_TRACE(( MakeObject(handle), prtNOTIFY, "krn\tObject moved during ChildClose call" ));
			PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit" );
			return err;
		}
	}
	
	// call preclose procedure
	if ( handle->iface && handle->iface->internal->ObjectPreClose ) {
		CALL_INT_METH_0( handle, ObjectPreClose, lerr );
		if ( PR_FAIL(lerr) ) {
			tDWORD err_size = sizeof(tERROR);
			
			if ( handle == g_hRoot ) {
				wlock(po);
				handle->flags &= ~hf_OBJECT_IS_CLOSING;
				wunlock(po);
				PR_TRACE(( MakeObject(handle), prtDANGER, "krn\tSomebody wants to close Root object !!!" ));
				PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit !!!ERROR!!! - errOBJECT_IS_CLOSING" );
				return lerr;
			}
			
			_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_PRECLOSE_FAILED, 0, &lerr, &err_size, pids );
			err = lerr;
		}
		if ( parent != handle->parent ) {
			*moved = cTRUE;
			handle->flags &= ~(hf_OBJECT_CLOSED | hf_OBJECT_IS_CLOSING);
			PR_TRACE(( MakeObject(handle), prtNOTIFY, "krn\tObject moved during ObjectPreClose call" ));
			PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit" );
			return err;
		}
	}
	
	wlock(po);
	
	// remove parent's sync or mem object if any
	if ( parent ) {
		tPTR obj = MakeObject(handle);
		if ( parent->sync == obj )
			parent->sync = 0;
		else if ( parent->tracer == obj )
			parent->tracer = 0;
		else if ( parent->mem == obj )
			parent->mem = 0;
	}
	
	if ( handle->flags & hf_MESSAGE_HANDLER ) {
		tDWORD cookie = 0;
		while( _HandleFindStat(po,&cookie,_remove_msg_handler,handle) )
			;
	}
	wunlock(po);

	// close all children except heaps, tracers & syncs
	mem_obj = trace_obj = sync_obj = cFALSE;
	while ( handle->child ) {
		tHANDLE* victim = handle->child;
		while ( victim ) {
			if ( ((hOBJECT)handle->mem) == MakeObject(victim) ) { // CHECK_IID_C(victim,IID_HEAP) || CHECK_IID_C(victim,IID_TRACER))
				mem_obj = cTRUE;
				victim = victim->next;
				continue;
			}
			if ( ((hOBJECT)handle->tracer) == MakeObject(victim) ) {
				trace_obj = cTRUE;
				victim = victim->next;
				continue;
			}
			if ( ((hOBJECT)handle->sync) == MakeObject(victim) ) {
				sync_obj = cTRUE;
				victim = victim->next;
				continue;
			}
			break;
		}
		if ( !victim )
			break;
		lerr = _ObjectClose( po, victim, pids );
		if ( lerr == errOBJECT_IS_CLOSING )
			return errOBJECT_IS_CLOSING;
		if ( lerr == errOBJECT_IS_CLOSED )
			return errOBJECT_IS_CLOSED;
		if ( PR_FAIL(lerr) )
			err = lerr;
	}
	
	// call close procedure
	if ( handle->iface && handle->iface->internal->ObjectClose ) {
		CALL_INT_METH_0( handle, ObjectClose, lerr );
		if ( PR_FAIL(lerr) ) {
			tDWORD err_size = sizeof(tERROR);
			_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_CLOSE_FAILED, 0, &lerr, &err_size, pids );
			err = lerr;
		}
		if ( parent != handle->parent ) {
			*moved = cTRUE;
			handle->flags &= ~(hf_OBJECT_CLOSED | hf_OBJECT_IS_CLOSING);
			PR_TRACE(( MakeObject(handle), prtNOTIFY, "krn\tObject moved during ObjectClose call" ));
			PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit" );
			return err;
		}
	}
	
	PR_TRACE(( MakeObject(handle), prtNOT_IMPORTANT+6, "krn\tObject(%p, %tiid, %tpid) has been freed", MakeObject(handle), GET_IID_C(handle), GET_PID_C(handle) ));
	//_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_HAS_BEEN_FREED, 0, 0, 0, pids );
	
	// now we have only heap, tracer or synchronizer as child and are going to close it
	if ( trace_obj && handle->tracer ) {
		tHANDLE* hdl = MakeHandle( handle->tracer );
		handle->tracer = 0;
		lerr = _ObjectClose( po, hdl, pids );
		if ( lerr == errOBJECT_IS_CLOSING )
			return errOBJECT_IS_CLOSING;
		if ( lerr == errOBJECT_IS_CLOSED )
			return errOBJECT_IS_CLOSED;
		if ( PR_FAIL(lerr) )
			err = lerr;
	}
	
	if ( sync_obj && handle->sync ) {
		tHANDLE* hdl = MakeHandle( handle->sync );
		handle->sync = 0;
		lerr = _ObjectClose( po, hdl, pids );
		if ( lerr == errOBJECT_IS_CLOSING )
			return errOBJECT_IS_CLOSING;
		if ( lerr == errOBJECT_IS_CLOSED )
			return errOBJECT_IS_CLOSED;
		if ( PR_FAIL(lerr) )
			err = lerr;
	}
	
	if ( mem_obj && handle->mem ) {
		tHANDLE* hdl = MakeHandle( handle->mem );
		handle->mem = 0;
		lerr = _ObjectClose( po, hdl, pids );
		if ( lerr == errOBJECT_IS_CLOSING )
			return errOBJECT_IS_CLOSING;
		if ( lerr == errOBJECT_IS_CLOSED )
			return errOBJECT_IS_CLOSED;
		if ( PR_FAIL(lerr) )
			err = lerr;
	}
	
	while ( handle->child ) {
		lerr = _ObjectClose( po, handle->child, pids );
		if ( lerr == errOBJECT_IS_CLOSING )
			return errOBJECT_IS_CLOSING;
		if ( lerr == errOBJECT_IS_CLOSED )
			return errOBJECT_IS_CLOSED;
		if ( PR_FAIL(lerr) )
			err = lerr;
	}
	
	if ( handle->iface && handle->iface->internal->ObjectNew ) {
		CALL_INT_OBJ_NEW( handle, handle->iface, lerr, cFALSE );
	}
	
	wlock(po);
	handle->flags |= hf_HIDDEN;
#define FLAGS_TO_CLEAR (               \
	hf_OPERATIONAL_MODE      | \
	hf_MEMORY_LOCKED         | \
	hf_MESSAGE_HANDLER       | \
	hf_MESSAGE_HANDLER_CACHE | \
	hf_OBJECT_PROTECTED      | \
	hf_PROTECT_OBJECT_SET    | \
	hf_OBJECT_IS_CLOSING)
	handle->flags &= ~FLAGS_TO_CLEAR;
#undef FLAGS_TO_CLEAR
	
	if ( PR_FAIL(_WrapperWithdraw(po,handle,0,0)) ) // remove all wrappers
		_set_vtbl( handle, 0 );
	
	// free allocated memory
	ext = handle->ext;
	if ( ext ) {
		handle->ext = 0;
		if ( g_handle_ext_count )
			--g_handle_ext_count;
		wunlock(po);
		if ( ext->mh_arr.parr ) {
			PrFree( ext->mh_arr.parr );
			ext->mh_arr.mcount = 0;
			ext->mh_arr.parr = 0;
		}
		if ( ext->mh_c_arr.parr ) {
			PrFree( ext->mh_c_arr.parr );
			ext->mh_c_arr.count = 0;
			ext->mh_c_arr.parr = 0;
		}
		PrFree( ext );
	}
	else
		wunlock(po);
	
	PR_TRACE_A1( MakeObject(handle), "Leave ObjectDeinit, %terr", err );
	return err;
}



// ---
tERROR pr_call _ObjectRevert( tPO* po, tHANDLE* handle, tBOOL reuse_custom_prop, tDATA* pids ) {
	
	tERROR err;
	tBOOL  moved;
	PR_TRACE_A0( MakeObject(handle), "Enter _ObjectRevert" );
	
	/*if ( handle->iface && (handle->iface->flags & IFACE_STATIC) )
		err = errOBJECT_CANNOT_BE_REVERTED;
	else*/ 

	if ( handle->flags & hf_OBJECT_IS_CLOSING ) {
		PR_TRACE(( MakeObject(handle), prtFATAL, "krn\t!!! E R R O R -- OBJECT_IS_CLOSED_NOW !!! - Handle is in closing process (possible called from another thread)" ));
		PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit !!!ERROR!!! - errOBJECT_IS_CLOSING" );
		return errOBJECT_IS_CLOSING;
	}
	
	if( handle->flags & hf_OBJECT_CLOSED ) {
		PR_TRACE(( MakeObject(handle), prtIMPORTANT, "krn\tOBJECT_IS_CLOSED - Handle is closed by another caller (possible in another thread)" ));
		PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit - errOBJECT_IS_CLOSED" );
		return errOBJECT_IS_CLOSED;
	}
	
	wlock(po);
	handle->flags |= hf_OBJECT_IS_CLOSING;
	wunlock(po);
	
	if ( PR_FAIL(err=_ObjectCheckLocked(po,handle,pids)) )
		;
	else if ( (errOBJECT_IS_CLOSING == (err=_ObjectDeinit(po,handle,&moved,pids))) || (err==errOBJECT_IS_CLOSED) || (err==errLOCKED/*only for closing root object*/) )
		;
	else if ( moved )
		err = errOBJECT_CANNOT_BE_REVERTED;
	else {
		if ( !reuse_custom_prop )
			_CustomPropertyDeleteList( po, handle );
		_CustomPropInherit( po, handle );
		
		wlock(po);
		if ( handle->obj && handle->iface && handle->iface->memsize )
			memset( handle->obj, 0, handle->iface->memsize );
		SET_PROTECTION( handle, handle->iface );
		wunlock(po);
		
		err = _InterfaceSetNew( po, handle, handle->iface );
		
		if ( PR_FAIL(err) ) {
			_ObjectClose( po, handle, pids );
			handle = 0;
		}
	}
	
	PR_TRACE_A0( MakeObject(handle), "Leave _ObjectRevert" );
	return err;
}


// ---
typedef struct tag_LockEventItem {
  hSYNC_EVENT ev;
	tBOOL       used;
} tLockEventItem;

static tLockEventItem g_lock_event_pool[0x10] = {0}; // all events will be closed on _ObjectClose(g_hRoot)
static tUINT          g_lock_event_count = 0;


// ---
static tERROR _add_to_sync_list( tPO* po, tHANDLE* handle, tSynchroEvent** head, tSynchroEvent* el ) {
	tUINT           i;
	hSYNC_EVENT     ev = 0;
	tHANDLE*        ev_hdl;
	tLockEventItem* evi;

	wlock(po);
	handle->flags |= hf_LOCK_EVENT_SET;
	wunlock(po);

	for( i=0,evi=g_lock_event_pool; i<g_lock_event_count; ++i,++evi ) {
		if ( !evi->used ) {
			evi->used = cTRUE;
			ev = evi->ev;
		}
	}

	if ( !ev ) {
		tERROR err = _ObjectIIDCreate( po, g_hRoot, &ev_hdl, IID_EVENT, PID_ANY, SUBTYPE_ANY );
		if ( PR_SUCC(err) ) {
			tBOOL manual_reset = cTRUE;
			tDATA size;
			err = _PropertySet( po, ev_hdl, &size, pgMANUAL_RESET, &manual_reset, sizeof(manual_reset) );
		}
		if ( PR_SUCC(err) )
			err = _ObjectCreateDone( po, ev_hdl );
		if ( PR_SUCC(err) ) {
			ev = (hSYNC_EVENT)MakeObject(ev_hdl);
			if ( g_lock_event_count < countof(g_lock_event_pool) ) {
				++g_lock_event_count;
				evi->ev = ev;
				evi->used = cTRUE;
			}
		}
		if ( PR_FAIL(err) )
			return err;
	}

	el->m_handle = handle;
	el->m_event = ev;

	wlock(po);
	el->m_next = *head;
	*head = el;
	wunlock(po);
	return errOK;
}


// ---
static tVOID _remove_from_sync_list( tPO* po, tSynchroEvent** head, tSynchroEvent* el ) {
	tUINT i;
	hSYNC_EVENT     ev;
	tLockEventItem* evi;

	wlock(po);
	el->m_handle->flags &= ~hf_LOCK_EVENT_SET;
	if ( *head == el )
		*head = el->m_next;
	else {
		tSynchroEvent* ptr = *head;
		while( ptr ) {
			if ( ptr->m_next == el ) {
				ptr->m_next = el->m_next;
				break;
			}
			ptr = ptr->m_next;
		}
	}
	wunlock(po);

	ev = el->m_event;
	for( i=0,evi=g_lock_event_pool; i<g_lock_event_count; ++i,++evi ) {
		if ( evi->ev == ev ) {
			CALL_Event_SetState( ev, cFALSE );
			evi->used = cFALSE;
			return;
		}
	}
	_ObjectClose( po, MakeHandle(ev), 0 );
}


// ---
static hSYNC_EVENT	_find_in_sync_list( tSynchroEvent* head, const tHANDLE* handle ) {
	while( head ) {
		if ( head->m_handle == handle )
			return head->m_event;
		head = head->m_next;
	}
	return 0;
}





#if defined( _DEBUG )

	tVOID _lock_handle( tHANDLE* handle, tDWORD trace_level, const tCHAR* description ) {
	}

#endif



// ---
tERROR pr_call _ObjectCheckLocked( tPO* po, tHANDLE* handle, tDATA* pids ) {
	
	tERROR err = errOK;
	
	if ( handle->ref ) {
		hSYNC_EVENT event = 0;
		tSynchroEvent synchro = {0};

		PR_TRACE_HANDLE_LOCKS( handle, prtIMPORTANT, "object is locked on close" );
		_MsgSend( po, handle->parent ? handle->parent : g_hRoot, pmc_KERNEL, pm_OBJECT_IS_LOCKED_ON_CLOSE, handle, 0, 0, pids );
		
		/*event = _find_in_sync_list( g_synchro_beg_closers, handle );
		if ( event ) { // some thread is already waiting for release 
		}
		
		else*/ if ( PR_SUCC(err=_add_to_sync_list(po,handle,&g_synchro_beg_closers,&synchro)) )
			event = synchro.m_event;

		if ( !handle->ref ) {
			PR_TRACE_HANDLE_LOCKS( handle, prtIMPORTANT, "object is released while we were getting synchroevent" );
		}
		else if ( !event ) {
			if ( PR_SUCC(err) )
				err = errUNEXPECTED;
			PR_TRACE_HANDLE_LOCKS( handle, prtERROR, "object is still locked on close, cannot find/create synchroevent" );
		}
		else {
			if ( po && po->usage ) {
				PR_TRACE(( MakeObject(handle), prtERROR, "krn\tinternal critical section is locked on locked object!!!! deadlock may be?" ));
			}

			runlock(po);
			CALL_Event_Wait( event, cSYNC_INFINITE );
			rlock(po);

			if ( handle->ref ) {
				PR_TRACE_HANDLE_LOCKS( handle, prtERROR, "object is still locked on close" );
			}
		}
		
		if ( synchro.m_event )
			_remove_from_sync_list( po, &g_synchro_beg_closers, &synchro );

		if ( PR_SUCC(err) ) {
			PR_TRACE_HANDLE_LOCKS( handle, prtNOT_IMPORTANT, "Object is released on close, and i'm going to free it :-) " );
		}
		else {
			tDWORD err_size = sizeof(tERROR);
			_MsgSend( po, handle, pmc_KERNEL, pm_OBJECT_CLOSE_FAILED, 0, &err, &err_size, pids );
			err = errOK; // has to kill object anyway
		}
	}
	
	return err; 
}



// ---
tERROR pr_call _ObjectRelease( tPO* po, tHANDLE* handle ) {
	tERROR err = errOK;
	
	if ( handle->ref ) {
		--handle->ref;
		PR_TRACE_HANDLE_LOCKS( handle, prtNOT_IMPORTANT, "unref" );
	}
	else {
		PR_TRACE_HANDLE_LOCKS( handle, prtIMPORTANT, "attempt to unref unreferenced object" );
	}
	
	if ( !handle->ref ) {
		if ( (handle->flags & hf_LOCK_EVENT_SET) ) {
			hSYNC_EVENT ev = _find_in_sync_list( g_synchro_beg_closers, handle );
			if ( ev ) {
				PR_TRACE_HANDLE_LOCKS( handle, prtNOT_IMPORTANT, "pulse release event" );
				err = CALL_Event_SetState( ev, cTRUE );
			}
			else {
				PR_TRACE_HANDLE_LOCKS( handle, prtIMPORTANT, "cannot find release event" );
				err = errSYNCHRONIZATION_OBJECT_ABANDONED;
			}
		}
	}
	
	return err;
}



// ---
tERROR pr_call _ObjectClose( tPO* po, tHANDLE* handle, tDATA* pids ) {
	tERROR err;
	tBOOL  moved;
	PR_TRACE_A0( MakeObject(handle), "Enter _ObjectClose" );
	
	if ( handle->flags & hf_OBJECT_IS_CLOSING ) {
		PR_TRACE(( MakeObject(handle), prtFATAL, "krn\t!!! E R R O R -- OBJECT_IS_CLOSED_NOW !!! - Handle is in closing process (possible called from another thread)" ));
		PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit !!!ERROR!!! - errOBJECT_IS_CLOSING" );
		return errOBJECT_IS_CLOSING;
	}
	
	if( handle->flags & hf_OBJECT_CLOSED ) {
		PR_TRACE(( MakeObject(handle), prtIMPORTANT, "krn\tOBJECT_IS_CLOSED - Handle is closed by another caller (possible in another thread)" ));
		PR_TRACE_A0( MakeObject(handle), "Leave ObjectDeinit - errOBJECT_IS_CLOSED" );
		return errOBJECT_IS_CLOSED;
	}
	
	wlock(po);
	handle->flags |= hf_OBJECT_IS_CLOSING;
	wunlock(po);
	
	if ( PR_FAIL(err=_ObjectCheckLocked(po,handle,pids)) )
		;
	else if ( (errOBJECT_IS_CLOSING == (err=_ObjectDeinit(po,handle,&moved,pids))) || (err==errOBJECT_IS_CLOSED) || (err==errLOCKED) )
		;
	else if ( moved )
		;
	else {
		tOBJECT*    mem;
		tINTERFACE* iface = handle->iface;
		
		if ( iface )
			_InterfaceUnload( po, iface );
		
		wlock(po);
		if ( handle == g_plugins )
			g_plugins = g_plugins->next;
		_Remove( handle );                 // remove it from the parent list
		
		mem = handle->obj;
		handle->obj = 0;
		
		// free custom properties
		_CustomPropertyDeleteList( po, handle );
		
		// free object handle
		handle->flags &= ~hf_OBJECT_IS_CLOSING;
		handle->flags |= hf_OBJECT_CLOSED;
		_HandleFree( po, handle );
		
		if ( g_handle_count )
			g_handle_count--;
		
		if ( pids ) {
			handle->mem               = (hHEAP)*(pids+0);
			handle->custom_prop_count = (tDWORD)*(pids+1);
		}
		else {
			handle->mem               = 0;
			handle->custom_prop_count = 0;
		}
		
		wunlock(po);
		
		// free allocated memory
		if ( mem != NULL ) {
			#if defined( _DEBUG )
				mem->saved_handle = mem->handle;
				mem->handle = 0;
			#endif
			PrFree( mem );
		}
	}
	
	PR_TRACE_A0( MakeObject(handle), "Leave _ObjectClose" );
	return err;
}




