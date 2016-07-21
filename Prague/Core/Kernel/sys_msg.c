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

#include <Prague/iface/e_loader.h>


#define RECV_CLUSTER 4
#define MSG_CACHE_HANDLE_CLUSTER 16

// ---
struct tag_Receiver {
	tHANDLE* handler;
	tIID     iid;
	tPID     pid;
	hOBJECT  recv_point;
	tDWORD   flags;
};
typedef struct tag_Receiver tReceiver;


static tDWORD pr_call _BroadcastMsg( tPO* po, tHANDLE* handle, tIID iid, tDWORD type, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, tPTR pbuff, tDWORD* blen );
static tERROR pr_call _add_receiver( tMsgHandler* mh, hOBJECT recv_point, tReceiver** recvs, tUINT* ind );
static tERROR pr_call _add_top( tHANDLE* handle, tHANDLE*** arr, tUINT* ind );


#define lineno_to_str2(x)  #x
#define lineno_to_str(x) lineno_to_str2(x)
#define todo(desc)        message(__FILE__ "(" lineno_to_str(__LINE__) "): TODO: " desc)


// ---
#if defined( _DEBUG )
	tVOID _check_handle_for_msg_recv( tHANDLE* handle ) {
		if ( !handle || OBJ_IS_NOT_RECEIVER(handle) || !handle->iface || !handle->iface->internal || !handle->iface->internal->MsgReceive ) {
			handle = handle;
			//#pragma todo("Root\tINT3 must be removed before release!")
			//__asm int 3;
		}
	}
#endif

// ---
//#define ACCEPT_BY_METHOD
#if defined( ACCEPT_BY_METHOD )

tBOOL _accept_receiver( tPO* po, tMsgHandler* mh, tDWORD msg_cls, tHANDLE* ctx ) {
		tHANDLE* h = mh->handler;
		if ( !h || !h->obj || OBJ_IS_NOT_RECEIVER(h) )
			return cFALSE;
		if ( (mh->msg_cls != MC_ANY) && (mh->msg_cls != msg_cls) )
			return cFALSE;
		if ( (mh->ctx_iid != IID_ANY) && (!ctx || (ctx->iid != mh->ctx_iid)) )
			return cFALSE;
		if ( mh->ctx_pid != PID_ANY ) {
			tPID pid;
			if ( !ctx )
				return cFALSE;
			if ( ctx->iface->pid_user )
				pid = _PropertyGetPID( po, ctx, ctx->iface->pid_user );
			else
				pid = ctx->iface->pid;
			if ( pid != mh->ctx_pid )
				return cFALSE;
		}
		return cTRUE;
	}
	#define ACCEPT_RECEIVER( po, mh, msg_cls, ctx ) _accept_receiver( po, mh, msg_cls, ctx )

#else

	#define ACCEPT_RECEIVER( po, mh, msg_cls, ctx ) (                         \
		mh->handler && mh->handler->obj                                     &&  \
		OBJ_IS_RECEIVER(mh->handler)                                        &&  \
		((mh->msg_cls == MC_ANY ) || (mh->msg_cls == msg_cls))              &&  \
		((mh->ctx_iid == IID_ANY) || (ctx && CHECK_IID_C(ctx,mh->ctx_iid))) &&  \
		((mh->ctx_pid == PID_ANY) || (ctx && CHECK_PID_C(ctx,mh->ctx_pid))) )

#endif


#define ACCEPT_SELF_RECEIVER( po, mh, msg_cls, handle, ctx )   (    \
	ACCEPT_RECEIVER(po,mh,msg_cls,ctx) ) &&                           \
	CHECK_IID_C(handle,mh->obj_iid) && CHECK_PID_C(handle,mh->obj_pid)
  



// ---
tERROR pr_call System_SendMsg( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, tPTR context, tPTR pbuff, tDWORD* blen ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* ctx;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::SendMsg" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( handle ) {
		ctx = 0;
		if ( !context || _HC(ctx,0,context,&error) ) {
			tDATA iids[2] = {0};
			error = _MsgSend( po, handle, msg_cls, msg_id, ctx, pbuff, blen, CALLER_INFO(object,iids) );
		}
		else
			PR_TRACE(( object, prtNOTIFY, "krn\tSystem::SendMsg[context object invalid](%tpmc)", msg_cls ));
		runlock(po); 
	}
	else
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::SendMsg[object invalid](%tpmc)", msg_cls ));
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::SendMsg ret %terr", error );
	return error;
}



// ---
// message management functions
tDWORD pr_call System_BroadcastMsg( hOBJECT object, tIID iid, tDWORD type, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* blen ) {
	
	tERROR   error;
	tHANDLE* handle;
	tDWORD   count = 0;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::BroadcastMsg" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( handle ) {
		tHANDLE* h;
		if ( !ctx || _HC(h,0,(hOBJECT)ctx,&error) ) {
			count = _BroadcastMsg( po, handle, iid, type, msg_cls, msg_id, object, ctx, pbuff, blen );
		}
		else
			PR_TRACE(( object, prtNOTIFY, "krn\tSystem::BroadcastMsg[context object invalid](%tiid, type=%u, %tpmc)", iid, type, msg_cls ));
		runlock(po);
	}
	else
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::BroadcastMsg[object invalid](%tiid, type=%u, %tpmc)", iid, type, msg_cls ));
	
	leave_lf();
	PR_TRACE_A2( object, "Leave System::BroadcastMsg ret tDWORD = %u, error = %terr", count, error );
	return count;
}




// ---
tERROR pr_call System_IntMsgReceive( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen ) {
	tERROR   error;
	tHANDLE* handle;
	tIntMsgReceive msg_func;
	tPO*     po;
	
	PR_TRACE_A0( object, "Enter System::IntMsgReceive()" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( !handle ) 
		PR_TRACE(( object, prtERROR, "krn\tSystem::IntMsgReceive() - %terr (object invalid)", error ));
	else if ( !handle->iface ) {
		runlock(po);
		error = errOBJECT_NOT_INITIALIZED;
		PR_TRACE(( object, prtERROR, "krn\tSystem::IntMsgReceive() - errOBJECT_NOT_INITIALIZED" ));
	}
	else if ( 0 == (msg_func=handle->iface->internal->MsgReceive) ) {
		runlock(po);
		error = errINTERFACE_INCOMPATIBLE;
		PR_TRACE(( object, prtERROR, "krn\tSystem::IntMsgReceive(), error=errINTERFACE_INCOMPATIBLE object has no MsgReceive internal method" ));
	}
	else {
		tExceptionReg er;
		tDATA iids[2] = {0};

		HANDLE_LOCK( CALLER_INFO(object,iids), handle, prtNOT_IMPORTANT, "lock" ); // ensure object is alive

		runlock(po);
		if ( errOK == si.EnterProtectedCode(0,&er) )
			error = msg_func( CUST_TO_C_OBJ(object), msg_cls, msg_id, CUST_TO_C_OBJ(send_point), CUST_TO_C_OBJ(ctx), CUST_TO_C_OBJ(recieve_point), pbuff, blen );
		else
			error = errUNEXPECTED;
		si.LeaveProtectedCode( 0, &er );

		rlock(po);
		_ObjectRelease( po, handle );
		runlock(po);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::IntMsgReceive() ret %terr", error );
	return error;
}




// ---
tERROR pr_call System_RegisterMsgHandler( hOBJECT object, tDWORD msg_cls, tDWORD flags, tPTR tree_top, tIID obj_iid, tPID obj_pid, tIID ctx_iid, tPID ctx_pid ) {
	
	tERROR   error;
	tHANDLE* top;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::RegisterMsgHandler" );
	enter_lf();
	
	_HC( handle, po, object, &error );
	if ( !handle )
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::RegisterMsgHandler(obj=[%tiid, %tpid], ctx=[%tiid, %tpid]), %terr - object invalid", obj_iid, obj_pid, ctx_iid, ctx_pid, error ));
	
	else if ( !handle->iface ) {
		error = errINTERFACE_NOT_ASSIGNED_YET;
		PR_TRACE(( object, prtERROR, "krn\tSystem::RegisterMsgHandler(obj=[%tiid, %tpid], ctx=[%tiid, %tpid]), error=errINTERFACE_NOT_ASSIGNED_YET", obj_iid, obj_pid, ctx_iid, ctx_pid ));
	}
	
	else if ( !handle->iface || !handle->iface->internal->MsgReceive ) {
		error = errINTERFACE_INCOMPATIBLE;
		PR_TRACE(( object, prtERROR, "krn\tSystem::RegisterMsgHandler(obj=[%tiid, %tpid], ctx=[%tiid, %tpid]), error=errINTERFACE_INCOMPATIBLE", obj_iid, obj_pid, ctx_iid, ctx_pid ));
	}
	
	else if ( !tree_top ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::RegisterMsgHandler(obj=[%tiid, %tpid], ctx=[%tiid, %tpid]), error=errPARAMETER_INVALID[tree top object not specified]", obj_iid, obj_pid, ctx_iid, ctx_pid ));
	}
	
	else if ( !_HCC(top,0,lock,(hOBJECT)tree_top,&error) )
		PR_TRACE(( object, prtERROR,"krn\tSystem::RegisterMsgHandler(obj=[%tiid, %tpid], ctx=[%tiid, %tpid]), %terr[tree top object invalid]", obj_iid, obj_pid, ctx_iid, ctx_pid, error ));
	
	else {
		wlock(po);
		_ClearMsgHandlerCache( top );
		if ( PR_SUCC(error=_chk_ext_0(top)) ) {
			_mh_add( &top->ext->mh_arr, handle, flags, msg_cls, obj_iid, obj_pid, ctx_iid, ctx_pid, &error );
			if ( errOK == error )
				handle->flags |= hf_MESSAGE_HANDLER;
		}
		wunlock(po); 
	}

  if ( handle )
    runlockc(po,lock);

  leave_lf();
  PR_TRACE_A1( object, "Leave System::RegisterMsgHandler ret %terr", error );
  return error;
}



// ---
tERROR pr_call System_UnregisterMsgHandler( hOBJECT object, tDWORD msg_cls, tPTR tree_top ) {
	
	tERROR   error;
	tHANDLE* top = 0;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::UnregisterMsgHandler" );
	enter_lf();
	
	_HCC( handle, po, lock, object, &error );
	if ( !handle ) 
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::UnregisterMsgHandler[object invalid](%tpmc, %terr)", msg_cls, error ));
	
	else if ( !tree_top ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::UnregisterMsgHandler[tree top object not specified](%tpmc, error=errPARAMETER_INVALID)", msg_cls ));
	}
	
	else if ( 0 == (top=_handle_check(0,(hOBJECT)tree_top,&error)) /*!_HC(top,0,(hOBJECT)tree_top,&error)*/ )
		PR_TRACE(( object, prtERROR, "krn\tSystem::UnregisterMsgHandler[tree top object invalid](%tpmc, %terr)", msg_cls, error ));
	
	else {
		tDWORD ind = 0;
		wlock(po);
		if ( PR_SUCC(error=_chk_ext_0(top)) ) {
			tHANDLEX* ext = top->ext;
			error = errOBJECT_NOT_FOUND;
			if ( msg_cls == MC_ANY ) {
				while ( _mh_find_handler(&ext->mh_arr,&ind,handle,MC_ANY,IID_ANY,PID_ANY) ) {
					_mh_remove_by_ind( &ext->mh_arr, ind );
					error = errOK;
				}
			}
			else if ( _mh_find_handler(&ext->mh_arr,&ind,handle,msg_cls,IID_ANY,PID_ANY) ) {
				_mh_remove_by_ind( &ext->mh_arr, ind );
				error = errOK;
			}
		}
		if ( error == errOK )
			_ClearMsgHandlerCache( top );
		wunlock(po);
	}
	
	if ( handle )
		runlockc(po,lock);
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::UnregisterMsgHandler ret %terr", error );
	return error;
}


// ---
tERROR pr_call System_RegisterMsgHandlerList( hOBJECT object, const tMsgHandlerDescr* handler_list, tDWORD handler_count ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::RegisterMsgHandlerList" );
	enter_lf();
	
	if ( !handler_list || !handler_count ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::RegisterMsgHandlerList(), %terr - handler_list", error ));
	}
	
	else if ( 0 == _HCC(handle,po,lock,object,&error) ) 
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::RegisterMsgHandlerList(), %terr - object invalid", error ));
	
	else {
		tUINT     i;
		tHANDLE*  top;
		const tMsgHandlerDescr* hl = handler_list;
		tHANDLE*  tops[MSG_CACHE_HANDLE_CLUSTER];
		tHANDLE** ptops = tops;
		tUINT     count = 0;
		
		error = errOK;
		for( i=0; (i<handler_count) && PR_SUCC(error); i++,hl++ ) {
			top = _handle_check( 0, hl->object, &error );
			if ( !top )
				PR_TRACE(( object, prtERROR,"krn\tSystem::RegisterMsgHandlerList(), %terr[tree top object invalid]", error ));
			else if ( !top->iface ) {
				error = errINTERFACE_NOT_ASSIGNED_YET;
				PR_TRACE(( object, prtERROR, "krn\tSystem::RegisterMsgHandlerList(), error=errINTERFACE_NOT_ASSIGNED_YET for tree_top object" ));
			}
			else if ( !top->iface->internal->MsgReceive ) {
				error = errINTERFACE_INCOMPATIBLE;
				PR_TRACE(( object, prtERROR, "krn\tSystem::RegisterMsgHandler(), error=errINTERFACE_INCOMPATIBLE tree_top has no MsgReceive internal method" ));
			}
			else
				error = _add_top( top, &ptops, &count );
		}
		
		if ( PR_SUCC(error) ) {
			hl = handler_list;
			wlock(po);
			_ClearMsgHandlerCache( handle );
			if ( PR_SUCC(error=_chk_ext_0(handle)) ) {
				tHANDLEX* ext = handle->ext;
				for( i=0; (i<handler_count) && PR_SUCC(error); i++,hl++ ) {
					_mh_add( &ext->mh_arr, ptops[i], hl->flags, hl->msg_cls, hl->obj_iid, hl->obj_pid, hl->ctx_iid, hl->ctx_pid, &error );
					if ( errOK == error )
						ptops[i]->flags |= hf_MESSAGE_HANDLER;
				}
			}
			wunlock(po);
		}
		
		if ( ptops != tops )
			PrFree( ptops );
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::RegisterMsgHandlerList ret %terr", error );
	return error;
}



// ---
// cBROADCAST_AND             0x00    // result is true if all objects accepted broadcast
// cBROADCAST_OR              0x01    // result is true if at least one object accepted broadcast
// cBROADCAST_COUNT           0x02    // result is count of accepted broadcasts
// cBROADCAST_FIRST_LEVEL     0x10 // depth of broadcasts is one level down
// cBROADCAST_ALL_LEVELS      0x30 // depth of broadcasts is all levels down
// cBROADCAST_OBJECT_ITSELF   0x40 // message is sent to object itself
static tDWORD pr_call _BroadcastMsg( tPO* po, tHANDLE* handle, tIID iid, tDWORD type, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, tPTR pbuff, tDWORD* blen ) {
	
	tERROR error;
	tDWORD res;
	tDATA iids[2] = { 0x11011, 0x11011 };
	
	PR_TRACE_A0( MakeObject(handle), "Enter _BroadcastMsg" );
	
	if ( (type & cBROADCAST_OBJECT_ITSELF) && CHECK_IID_C(handle,iid) && handle->iface && handle->iface->internal->MsgReceive ) {
		
		HANDLE_LOCK( iids, handle, prtNOT_IMPORTANT, "lock" ); // enshure object is locked
		
		CALL_MSG_RECV( handle, error, msg_cls, msg_id, CUST_TO_C_OBJ(obj), ctx, CUST_TO_C_OBJ(obj), pbuff, blen );
		_ObjectRelease( po, handle );
		res = (error == errOK_DECIDED) || PR_FAIL(error);
	}
	else
		res = 0;
	
	if ( handle->child && (type & cBROADCAST_FIRST_LEVEL) ) {
		
		tHANDLE* h = handle->child;
		for( h=handle->child; h; h=h->next ) {
			tDWORD again;
			if ( (type & cBROADCAST_ALL_LEVELS) == cBROADCAST_ALL_LEVELS )
				again = _BroadcastMsg( po, h, iid, (type|cBROADCAST_OBJECT_ITSELF), msg_cls, msg_id, obj, ctx, pbuff, blen );
			else if ( CHECK_IID_C(h,iid) && h->iface && h->iface->internal->MsgReceive ) {

				HANDLE_LOCK( iids, h, prtNOT_IMPORTANT, "lock" ); // enshure object is locked
				
				CALL_MSG_RECV( h, error, msg_cls, msg_id, CUST_TO_C_OBJ(obj), ctx, CUST_TO_C_OBJ(obj), pbuff, blen );
				_ObjectRelease( po, h );
				again = (error == errOK_DECIDED) || PR_FAIL(error);
			}
			else
				continue;
			
			switch( type & (cBROADCAST_AND|cBROADCAST_OR|cBROADCAST_COUNT) ) {
				case cBROADCAST_AND   : res = res && again; break;
				case cBROADCAST_OR    : res = res || again; break;
				case cBROADCAST_COUNT :
				default               : res = res +  again; break;
			}
		}
	}
	
	PR_TRACE_A1( MakeObject(handle), "Leave _BroadcastMsg ret tDWORD = %u", res );
	return res;
}




// ---
tERROR pr_call _MsgSend( tPO* po, tHANDLE* handle, tDWORD msg_cls, tDWORD msg_id, tHANDLE* ctx, tPTR pbuff, tDWORD* blen, tDATA* iids ) {
	tERROR        e;
	tDWORD        i, c;
	tMsgHandler*  mh;
	tReceiver     l_recvs[RECV_CLUSTER];
	tReceiver*    recvs;
	tUINT         count;
	tERROR        add_err;
	hOBJECT       o;
	tHANDLEX*     ext;


	if ( PR_FAIL(_chk_ext(po,handle,ext,e)) )
		return e;

	o       = MakeObject( handle );
	recvs   = l_recvs;
	count   = 0;
	add_err = errOK;

	// own message handlers
	for( i=0, c=ext->mh_arr.mcount, mh=ext->mh_arr.parr; (i<c) && PR_SUCC(add_err); ++i, ++mh ) {
		if ( ACCEPT_SELF_RECEIVER(po,mh,msg_cls,handle,ctx) ) 
			add_err = _add_receiver( mh, o, &recvs, &count );
	}
	
	// cache parent's handlers if any
	if ( 0 == (handle->flags & hf_MESSAGE_HANDLER_CACHE) ) {
		tHANDLE* h = handle->parent;
		wlock(po);
		while( h ) { // gather message handler cache
			tDWORD id = 0;
			if ( h->ext ) {
				mh = _mh_find( &h->ext->mh_arr, &id, handle );
				while( mh ) {
					id++;
					_mh_c_add( &ext->mh_c_arr, mh, MakeObject(h), &e );
					mh = _mh_find( &h->ext->mh_arr, &id, handle );
				}
			}
			h = h->parent;
		}
		handle->flags |= hf_MESSAGE_HANDLER_CACHE;
		wunlock(po);
	}
	
	// cached (parent's) message handlers
	for( i=0, c=ext->mh_c_arr.count; (i<c) && PR_SUCC(add_err); i++ ) {
		tMsgCachedHandler* mhc = ( ext->mh_c_arr.parr + i );
		if ( ACCEPT_RECEIVER(po,mhc->handler,msg_cls,ctx) )
			add_err = _add_receiver( mhc->handler, mhc->recv_point, &recvs, &count );
	}
	
	e = errOK_NO_DECIDERS;
	if ( count ) {
		tUINT         i;
		tReceiver*    r               = recvs;
		tBOOL         decision        = cFALSE;
		tBOOL         deciders        = cFALSE;
		tERROR        exception_fired = errOK;
		hOBJECT       ohandle         = MakeObject(handle);
		hOBJECT       octx            = MakeObject(ctx);
		
		HANDLE_LOCK( iids, handle, prtNOT_IMPORTANT, "lock" ); //ensure object is alive at _MsgSend !!!
		
		for( i=0; i<count; ++i,++r ) {
			if ( !(r->flags & MSG_FLAGS_DECIDER) || !decision ) {
				hOBJECT          obj;
				tIntFnMsgReceive msg;
				tERROR           err = errOK;
				tHANDLE*         h = r->handler;

				//if ( !obj || !msg || (r->iid != h->iid) || (r->pid != GET_PID(h)) ) {// cached handle became rotten
				if ( OBJ_IS_NOT_RECEIVER(h) ) { 
					h->flags &= ~hf_NOT_REUSABLE;
					//_ObjectRelease( po, h );
					r->handler = 0;
					continue;
				}
				
				msg = h->iface->internal->MsgReceive;
				if ( !msg )
					continue;

				obj = MakeObject(h);
				
				HANDLE_LOCK( iids, h, prtNOT_IMPORTANT, "lock" ); // enshure object is locked

				_PR_ENTER_PROTECTED( h ) {
					err = msg( obj, msg_cls, msg_id, ohandle, octx, r->recv_point, pbuff, blen );
				} _PR_LEAVE_PROTECTED( exception_fired );

				h->flags &= ~hf_NOT_REUSABLE;
				_ObjectRelease( po, h );  // :-) heh, they can safely destroy it!!!
				r->handler = 0;
				
				if ( r->flags & MSG_FLAGS_DECIDER ) {
					deciders = cTRUE;
					if ( PR_FAIL(err) || (err == errOK_DECIDED) ) {
						e = err;
						decision = cTRUE;
					}
				}
			}
		}
		
		if ( PR_FAIL(exception_fired) && deciders && !decision )
			e = errUNEXPECTED;
		
		//for( i=0,r=recvs; i<count; ++i,++r ) {
		//	if ( r->handler )
		//		_ObjectRelease( po, r->handler );
		//}
		
		_ObjectRelease( po, handle );

		if ( recvs != l_recvs )
			PrFree( recvs );
	}
	return e;
}



// ---
tVOID pr_call _ClearMsgHandlerCache( tHANDLE* handle ) {
	tHANDLE* child = handle->child;
	while( child ) {
		tHANDLEX* ext = child->ext;
		if ( ext && (child->flags & hf_MESSAGE_HANDLER_CACHE) ) {
			tMsgCachedHandler* tmp = ext->mh_c_arr.parr;
			child->flags &= ~hf_MESSAGE_HANDLER_CACHE;
			ext->mh_c_arr.count = 0;
			ext->mh_c_arr.parr = 0;
			if ( tmp )
				PrFree( tmp );
		}
		_ClearMsgHandlerCache( child );
		child = child->next;
	}
}



// ---
static tERROR pr_call _add_top( tHANDLE* handle, tHANDLE*** arr, tUINT* ind ) {
	if ( *ind && !(*ind%MSG_CACHE_HANDLE_CLUSTER) ) {
		tERROR    error;
		tHANDLE** new_ptr;
		if ( *ind == MSG_CACHE_HANDLE_CLUSTER ) {
			error = PrAlloc( (tPTR*)&new_ptr, 2*sizeof(tHANDLE*)*MSG_CACHE_HANDLE_CLUSTER );
			if ( PR_FAIL(error) )
				return error;
			mcpy( new_ptr, *arr, sizeof(tHANDLE*)*MSG_CACHE_HANDLE_CLUSTER );
		}
		else if ( PR_FAIL(error=PrRealloc((tPTR*)&new_ptr,*arr,*ind+sizeof(tHANDLE*)*MSG_CACHE_HANDLE_CLUSTER)) )
			return error;
		*arr = new_ptr;
	}
	*((*arr) + (*ind)++) = handle;
	return errOK;
}




#define HANDLER_LIST_SIGNATURE    0x34813749
#define HANDLER_LIST_VERSION      1
#define HANDLER_SIZE( os )        (os + sizeof(tDWORD) + sizeof(tDWORD) + sizeof(tIID) + sizeof(tPID) + sizeof(tIID) + sizeof(tPID))
#define HANDLER_LIST_SIZE(c,os)   (c * HANDLER_SIZE(os))
#define HANDLER_LIST_HEADER_SIZE  (sizeof(tDWORD)/*signature*/ + sizeof(tDWORD)/*version*/ + sizeof(tDWORD)/*handler count*/)

#define PB(b)                     (*(tBYTE**)&b)



// ---
tERROR pr_call _GetMsgHandlerList( tPO* po, tHANDLE* handle, tUINT obj_size, tUINT* result, tBYTE** buff, tDWORD* buff_size, tObjSerializeFunc func, tPTR ctx ) {
	
	tMHArray* mha;
	tMHArray  lmha = { 0, 0 };
	tUINT     size;
	tERROR    error = errOK;
	tPTR      buffer = *buff;
	
	if ( !obj_size )
		obj_size = sizeof(hOBJECT);
	
	mha  = handle->ext ? &handle->ext->mh_arr : &lmha;
	size = HANDLER_LIST_HEADER_SIZE + HANDLER_LIST_SIZE(mha->mcount,obj_size);
	
	if ( result )
		*result = size;
	
	if ( !buffer )
		;
	
	else if ( (*buff_size) < size ) {
		error = errBUFFER_TOO_SMALL;
		PR_TRACE(( MakeObject(handle), prtERROR, "krn\t_GetMsgHandlerList(%terr)", error ));
	}
	
	else {
		tUINT        i  = 0;
		tUINT        c  = (tUINT)mha->mcount;
		tMsgHandler* hl;
		
		TYPE_ASSIGN( tDWORD, buffer, HANDLER_LIST_SIGNATURE );
		TYPE_ASSIGN( tDWORD, buffer, HANDLER_LIST_VERSION );
		TYPE_ASSIGN( tDWORD, buffer, mha->mcount);
		
		hl = mha->parr;
		
		for( ; i<c; i++, hl++ ) {
			if ( func ) {
				error = func( MakeObject(handle), MakeObject(hl->handler), buffer, ctx );
				if ( PR_FAIL(error) )
					break;
				PB(buffer) += obj_size;
			}
			else
				TYPE_ASSIGN( hOBJECT, buffer, MakeObject(hl->handler) );
			TYPE_ASSIGN( tDWORD, buffer, hl->flags );
			TYPE_ASSIGN( tDWORD, buffer, hl->msg_cls );
			TYPE_ASSIGN( tIID,   buffer, hl->obj_iid );
			TYPE_ASSIGN( tPID,   buffer, hl->obj_pid );
			TYPE_ASSIGN( tIID,   buffer, hl->ctx_iid );
			TYPE_ASSIGN( tPID,   buffer, hl->ctx_pid );
		}
		if ( PR_SUCC(error) ) {
			*buff      += size;
			*buff_size -= size;
		}
	}
	
	return error;
}



// ---
tERROR pr_call _SetMsgHandlerList( tPO* po, tHANDLE* handle, tUINT obj_size, tUINT* result, tBYTE** buff, tDWORD* buff_size, tObjDeserializeFunc func, tPTR ctx ) {
	
	tERROR    error = errOK;
	tPTR      buffer = *buff;
	tUINT     size = 0;
	hOBJECT   objs[32];
	hOBJECT*  pobj = objs;
	tHANDLEX* ext;
	
	if ( !buffer )
		error = errPARAMETER_INVALID;
	
	else if ( HANDLER_LIST_SIGNATURE != TYPE_GET(tDWORD,buffer) ) {
		size += sizeof(tDWORD);
		error = errBAD_SIGNATURE;
	}
	
	else if ( HANDLER_LIST_VERSION != TYPE_GET(tDWORD,buffer) ) {
		size += 2 * sizeof(tDWORD);
		error = errBAD_VERSION;
	}
	
	else {
		
		tUINT count = TYPE_GET( tDWORD, buffer );
		
		size += 3 * sizeof(tDWORD);
		
		if ( (*buff_size) < HANDLER_LIST_SIZE(count,obj_size) )
			error = errBUFFER_TOO_SMALL;
		
		//    else if ( 0 != ((mha=&handle->mh_arr)->mcount) )
		//      error = errOBJECT_BAD_INTERNAL_STATE;
		
		else if ( !count ) {
			*buff      += size;
			*buff_size -= size;
		}

		else if ( PR_FAIL(_chk_ext(po,handle,ext,error)) )
			;

		else if ( func && (count > countof(objs)) && PR_FAIL(error=PrAlloc((tPTR*)&pobj,count*sizeof(hOBJECT))) )
			;
		
		else {
			tUINT     i;
			tDWORD    flags,   msg_cls;
			tIID      obj_iid, ctx_iid;
			tPID      obj_pid, ctx_pid;
			hOBJECT*  co;
			tMHArray* mha = &ext->mh_arr;
			
			if ( func ) {
				tPTR cp = buffer;
				tDWORD el_size = obj_size + sizeof(tDWORD) + sizeof(tDWORD) + sizeof(tIID) + sizeof(tPID) + sizeof(tIID) + sizeof(tPID);
				
				for( i=0, co=pobj, cp=buffer; (i<count); i++,co++,PB(cp)+=el_size ) {
					_PR_ENTER_PROTECTED_0(po) {
						error = func( MakeObject(handle), cp, co, ctx );
					} _PR_LEAVE_PROTECTED_0( po, error );
					if ( PR_FAIL(error) )
						break;
				}
			}
			
			wlock(po);
			//_mh_remove_all( mha ); // ???
			_ClearMsgHandlerCache( handle );
			
			for( i=0, co=pobj; i<count; i++ ) {
				tHANDLE* h;
				hOBJECT obj;
				if ( func ) {
					obj = *(co++);
					PB(buffer) += obj_size;
				}
				else
					obj = TYPE_GET( hOBJECT, buffer );
				flags   = TYPE_GET( tDWORD, buffer );
				msg_cls = TYPE_GET( tDWORD, buffer );
				obj_iid = TYPE_GET( tIID,   buffer );
				obj_pid = TYPE_GET( tPID,   buffer );
				ctx_iid = TYPE_GET( tIID,   buffer );
				ctx_pid = TYPE_GET( tPID,   buffer );

				h = MakeHandle(obj);
				if ( h ) {
					_mh_add( mha, h, flags, msg_cls, obj_iid, obj_pid, ctx_iid, ctx_pid, &error );
					if ( PR_FAIL(error) )
						break;
				}
				size += HANDLER_SIZE( obj_size );
			}
			wunlock(po);
			
			*buff      += size;
			*buff_size -= size;
			
			if ( pobj != objs )
				PrFree( pobj );
		}
	}
	
	if ( result )
		*result = size;
	
	return error;
}




// ---
tERROR pr_call System_GetMsgHandlerList( hOBJECT object, tDWORD* result_count, tMsgHandlerDescr* handler_list, tDWORD handler_count ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::GetMsgHandlerList" );
	enter_lf();
	
	if ( !handler_list != !handler_count ) {
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtERROR, "krn\tSystem::GetMsgHandlerList(%terr)", error ));
	}
	else if ( 0 == _HCC(handle,po,lock,object,&error) ) 
		PR_TRACE(( object, prtERROR, "krn\tSystem::GetMsgHandlerList[object invalid](%terr)", error ));

	else if ( !handle->ext ) {
		if ( result_count )
			*result_count = 0;
		runlockc(po,lock);
	}

	else {
		tMHArray* mha = &handle->ext->mh_arr;
		
		error = errOK;
		if ( result_count )
			*result_count = mha->mcount;
		
		if ( !handler_list || !mha->mcount )
			;
		
		else if ( handler_count < mha->mcount ) {
			error = errBUFFER_TOO_SMALL;
			PR_TRACE(( object, prtERROR, "krn\tSystem::GetMsgHandlerList(%terr)", error ));
		}
		else {
			tUINT i = 0;
			tUINT c = (tUINT)mha->mcount;
			
			mcpy( handler_list, mha->parr, mha->mcount * sizeof(tMsgHandlerDescr) );
			for( ; i<c; i++, handler_list++ )
				handler_list->object = MakeObject( (tHANDLE*)handler_list->object );
		}
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::GetMsgHandlerList ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_UnregisterMsgHandlerList( hOBJECT object, const tMsgHandlerDescr* handler_list, tDWORD handler_count ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::UnregisterMsgHandlerList" );
	enter_lf();
	
	_HCC( handle, po, lock, object, &error );
	if ( !handle ) 
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::UnregisterMsgHandlerList(), %terr - object invalid", error ));
	
	else if ( !handler_list || !handler_count ) {
		runlockc(po,lock);
		error = errPARAMETER_INVALID;
		PR_TRACE(( object, prtNOTIFY, "krn\tSystem::UnregisterMsgHandlerList(), error = errPARAMETER_INVALID - handler list not specified" ));
	}
	
	else {
		tUINT     i;
		tHANDLE*  top;
		const tMsgHandlerDescr* hl = handler_list;
		tHANDLE*  tops[MSG_CACHE_HANDLE_CLUSTER];
		tHANDLE** ptops = tops;
		tUINT     count = 0;
		
		error = errOK;
		for( i=0; (i<handler_count) && PR_SUCC(error); i++,hl++ ) {
			top = _handle_check( 0, hl->object, &error );
			if ( !top )
				PR_TRACE(( object, prtERROR,"krn\tSystem::UnregisterMsgHandlerList(), %terr[tree top object invalid]", error ));
			else
				error = _add_top( top, &ptops, &count );
		}
		
		if ( PR_SUCC(error) ) {
			tDWORD ind;
			tHANDLEX* ext = handle->ext;
			error = errOBJECT_NOT_FOUND;

			if ( ext ) {
				hl = handler_list;
				wlock(po);
				for( i=0; i<handler_count; i++,hl++ ) {
					ind = 0;
					if ( hl->msg_cls == MC_ANY ) {
						while ( _mh_find_handler(&ext->mh_arr,&ind,ptops[i],MC_ANY,IID_ANY,PID_ANY) ) {
							_mh_remove_by_ind( &ext->mh_arr, ind );
							error = errOK;
						}
					}
					else if ( _mh_find_handler(&ext->mh_arr,&ind,ptops[i],hl->msg_cls,IID_ANY,PID_ANY) ) {
						_mh_remove_by_ind( &ext->mh_arr, ind );
						error = errOK;
					}
				}
				if ( error == errOK )
					_ClearMsgHandlerCache( handle );
				wunlock(po);
			}
		}
		
		if ( ptops != tops )
			PrFree( ptops );
		
		runlockc(po,lock);
	}
	
	leave_lf();
	PR_TRACE_A1( object, "Leave System::UnregisterMsgHandlerList ret %terr", error );
	return error;
}




// ---
static tERROR pr_call _add_receiver( tMsgHandler* mh, hOBJECT recv_point, tReceiver** recvs, tUINT* ind ) {
	tHANDLE* handle;
	tReceiver* curr;
	if ( *ind && !(*ind%RECV_CLUSTER) ) {
		tERROR     error;
		tReceiver* new_ptr;
		if ( *ind == RECV_CLUSTER ) {
			error = PrAlloc( (tPTR*)&new_ptr, 2*sizeof(tReceiver)*RECV_CLUSTER );
			if ( PR_FAIL(error) )
				return error;
			mcpy( new_ptr, *recvs, sizeof(tReceiver)*RECV_CLUSTER );
		}
		else if ( PR_FAIL(error=PrRealloc((tPTR*)&new_ptr,*recvs,(*ind+RECV_CLUSTER)*sizeof(tReceiver))) ) // was: *ind+sizeof(tReceiver)*RECV_CLUSTER
			return error;
		*recvs = new_ptr;
	}
	handle = mh->handler;

	handle->flags |= hf_NOT_REUSABLE; //ensure we can validate object later
	
	//HANDLE_LOCK( handle, prtNOT_IMPORTANT, "lock" ); // enshure object is locked
	
	curr = (*recvs) + *ind;
	curr->handler    = handle;
	curr->iid        = GET_IID_C(handle);
	curr->recv_point = recv_point;
	curr->flags      = mh->flags;

	if ( handle->iface->pid_user )
		curr->pid = _PropertyGetPID( 0, handle, handle->iface->pid_user );
	else
		curr->pid = handle->iface->pid;

	_check_handle_for_msg_recv( handle );
	
	(*ind)++;
	return errOK;
}



