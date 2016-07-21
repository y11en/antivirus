/*-----------------17.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel utilities                    *
 * Author Petrovitch                              *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
--------------------------------------------------*/


#include "kernel.h"
#include <m_utils.h>

// all these methods must be called under SHARE_LEVEL_WRITE protection !!!


// --- 
tMsgHandler* pr_call _mh_add( tMHArray* mha, tHANDLE* handler, tDWORD flags, tDWORD msg_cls, tIID o_iid, tPID o_pid, tIID ctx_iid, tPID ctx_pid, tERROR* error ) {
  
	tMsgHandler* h;
	
	_check_handle_for_msg_recv( handler );

	h = _mh_find_handler( mha, 0, handler, msg_cls, o_iid, o_pid );

  if ( h ) {
    *error = errOBJECT_ALREADY_EXISTS;
    return 0;
  }

  if ( !mha->mcount ) { 
    if ( PR_SUCC(*error=PrAlloc((tPTR*)&mha->parr,8*sizeof(tMsgHandler))) )
      h = mha->parr;
    else
      return 0;
  }
  else if ( !(mha->mcount % 8) ) { 
    tMsgHandler* tmpa;
    if ( PR_FAIL(*error=PrAlloc((tPTR*)&h,(mha->mcount+8)*sizeof(tMsgHandler))) )
      return 0;
    mmove( h+1, mha->parr, mha->mcount*sizeof(tMsgHandler) );
    tmpa = mha->parr;
    mha->parr = h;
    PrFree( tmpa );
  }
  else {
    h = mha->parr;
    mmove( h+1, h, mha->mcount*sizeof(tMsgHandler) );
  }

  mha->mcount++;
  h->handler = handler;
  h->msg_cls = msg_cls;
  h->flags   = flags;
  h->obj_iid = o_iid;
  h->obj_pid = o_pid;
  h->ctx_iid = ctx_iid;
  h->ctx_pid = ctx_pid;
  *error = errOK;
  return h;
}



// ---
tBOOL pr_call _mh_remove_by_ind( tMHArray* mha, tDWORD ind ) {
  if ( ind < mha->mcount ) {
    tDWORD c = --(mha->mcount);
    tDWORD t = c - ind;

    if ( t )
      mmove( mha->parr+ind, mha->parr+ind+1, t * sizeof(tMsgHandler) );

    if ( !c ) {
      tMsgHandler* tmp = mha->parr;
      mha->parr = 0;
      PrFree( tmp );
    }
    else if ( !(c % 8) )
			PrRealloc( (tPTR*)&mha->parr, mha->parr, c * sizeof(tMsgHandler) );
    return cTRUE;
  }
  else
    return cFALSE;
}




// ---
tBOOL pr_call _mh_remove_all( tMHArray* mha ) {
  if ( mha->mcount ) {
    tMsgHandler* tmp = mha->parr;
    mha->mcount = 0;
    mha->parr = 0;
    PrFree( tmp );
  }
  return cTRUE;
}



// ---
tBOOL pr_call _mh_remove( tMHArray* mha, tHANDLE* handler, tDWORD msg_cls ) {
  tDWORD        i;
  tMsgHandler* h = &mha->parr[0];
  for( i=0; i<mha->mcount; i++,h++ ) {
    if ( (h->handler == handler) && (h->msg_cls==msg_cls) )
      return _mh_remove_by_ind( mha, i );
  }
  return cFALSE;
}



// --- 
tMsgHandler* pr_call _mh_find( tMHArray* mha, tDWORD* ind, tHANDLE* obj ) {
  tDWORD i = ind ? *ind : 0;
  tMsgHandler* h = mha->parr + i;
  for( ; i<mha->mcount; i++, h++ ) {
    if ( CHECK_C(obj,h->obj_iid,h->obj_pid) ) {
			tHANDLE* handle = h->handler;
			if ( OBJ_IS_NOT_RECEIVER(handle) )
				continue;
      if ( ind )
        *ind = i;
      return h;
    }
  }
  return 0;
}



// ---
tMsgHandler* pr_call _mh_find_handler( tMHArray* mha, tDWORD* start_point, tHANDLE* obj, tDWORD msg_cls, tIID o_iid, tPID o_pid ) {
  tDWORD  lsp;
  tDWORD* plsp;
  tMsgHandler* res;

  if ( start_point )
    plsp = start_point;
  else {
    lsp = 0;
    plsp = &lsp;
  }

	for( res=mha->parr+*plsp; *plsp<mha->mcount; (*plsp)++, res++ ) {
    if ( 
			(res->handler == obj) && 
			((msg_cls==MC_ANY) || (res->msg_cls == MC_ANY)  || (res->msg_cls == msg_cls)) && 
			((o_iid==IID_ANY)  || (res->obj_iid == IID_ANY) || (res->obj_iid == o_iid)) &&
			((o_pid==PID_ANY)  || (res->obj_pid == PID_ANY) || (res->obj_pid == o_pid))
		)
      return res;
  }

  return 0;
}


#define HANDLER_CLUSTER 16

// --- 
tMsgHandler* pr_call _mh_c_add( tMHCacheArray* mhc, tMsgHandler* mh, hOBJECT recv_point, tERROR* error ) {

  if ( !mhc->count ) {
    if ( PR_FAIL(*error=PrAlloc((tPTR*)&mhc->parr,HANDLER_CLUSTER*sizeof(tMsgCachedHandler))) )
			return 0;
	}
  else if ( !(mhc->count % HANDLER_CLUSTER) ) {
    if ( PR_FAIL(*error=PrRealloc((tPTR*)&mhc->parr,mhc->parr,(mhc->count+HANDLER_CLUSTER)*sizeof(tMsgHandler))) )
			return 0;
	}

	_check_handle_for_msg_recv( mh->handler );

  mhc->parr[ mhc->count ].handler    = mh;
  mhc->parr[ mhc->count ].recv_point = recv_point;
  mhc->count++;
  return mh;
}




// ---
tBOOL pr_call _mh_c_remove( tMHCacheArray* mhc, tMsgHandler* mh ) {
  tUINT i, c;
  for( i=0, c=mhc->count; i<c; i++ ) {
    if ( mhc->parr[i].handler == mh ) {
      tDWORD tail;
      c = --mhc->count;
      tail = c - i;
      if ( tail )
        mmove( mhc->parr + i, mhc->parr + i + 1, tail * sizeof(tMsgHandler*) );
      if ( !c ) {
        PrFree( mhc->parr );
        mhc->parr = 0;
      }
      else if ( !(c % 16) )
        PrRealloc( (tPTR*)&mhc->parr, mhc->parr, c * sizeof(tMsgHandler*) );
      return cTRUE;
    }
  }
  return cFALSE;
}



