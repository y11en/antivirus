/*-----------------31.07.01 14:52-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Petrovitch                              *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"
#include <m_utils.h>

#include <Prague/iface/e_loader.h>


// ---
tERROR pr_call System_SyncCreate( hOBJECT object, hOBJECT* sync_result, tIID sync_iid, tPID sync_pid, tSTRING name ) {

	tERROR   error;
	tHANDLE* handle;
	hOBJECT  sync;
  tPO*     po;
  
	PR_TRACE_A0( object, "Enter System::SyncCreate" );
  enter_lf();

	_HC( handle,po, object, &error );
	if ( handle ) {
		error = _SyncCreate( po, handle, &sync, sync_iid, sync_pid, name );
		runlock(po);
	}
	else
		sync = 0;

	if ( sync_result )
		*sync_result = sync;

	leave_lf();
  PR_TRACE_A2( object, "Leave System::SyncCreate ret hOBJECT = %p, error = %terr", sync, error );
	return error;
}




// ---
tERROR pr_call System_SyncSet( hOBJECT object, hOBJECT sync_obj ) {
	
	tERROR   error;
	tHANDLE* handle;
	tHANDLE* sync_handle = 0;
  tPO*     po;
	tINT     lock = 1;
  
	PR_TRACE_A0( object, "Enter System::SyncSet" );
  enter_lf();
	
	if ( 0 == _HCC(handle,po,lock,object,&error) )
    ;
	else if ( handle->sync && sync_obj ) {
		PR_TRACE(( object, prtERROR, "krn\tSynchronization object is already set", handle->sync ));
		error = errOBJECT_ALREADY_EXISTS;
	}
	else if ( sync_obj && !_HC(sync_handle,0,sync_obj,&error) ) {
		PR_TRACE(( object, prtERROR, "krn\tSynchronization object(%p) not valid", sync_obj ));
	}
	else if ( !CHECK_IID_C(sync_handle,IID_CRITICAL_SECTION) && !CHECK_IID_C(sync_handle,IID_SEMAPHORE) && !CHECK_IID_C(sync_handle,IID_MUTEX) ) {
		PR_TRACE(( object, prtERROR, "krn\tIt is not a synchronization object(%tiid)", GET_IID_C(sync_handle) ));
		error = errINTERFACE_INCOMPATIBLE;
	}
	/*
	else if ( handle->sync && (0 == (handle->flags & hf_PROTECT_OBJECT_SET)) ) {
		PR_TRACE(( object, prtERROR, "krn\tSynchronization object already exits" ));
		error = errOBJECT_ALREADY_EXISTS;
	}
	*/
  else {
    wlock(po);
    handle->sync = sync_obj;
	  if ( sync_obj )
      handle->flags |= hf_PROTECT_OBJECT_SET;
	  else
      handle->flags &= ~hf_PROTECT_OBJECT_SET;
    wunlock(po);
  }

  if ( handle )
		runlockc(po,lock);
	
	leave_lf();
  PR_TRACE_A1( object, "Leave System::SyncSet error = %terr", error );
	return error;
}
	



// ---
tERROR pr_call System_SyncGet( hOBJECT object, hOBJECT* sync_result ) {

	tERROR   error;
	tHANDLE* handle;
	hOBJECT  sync;
  tPO*     po;
	tINT     lock = 1;
  
	PR_TRACE_A0( object, "Enter System::SyncGet" );
  enter_lf();

	_HCC( handle, po, lock, object, &error );
	if ( handle ) {
		sync = handle->sync;
		runlockc(po,lock);
	}
	else
		sync = 0;

  if ( !sync )
    error = errOBJECT_NOT_FOUND;

	if ( sync_result )
		*sync_result = sync;

  leave_lf();
  PR_TRACE_A2( object, "Leave System::SyncGet ret hOBJECT = %p, error = %terr", sync, error );
  return error;
}




// ---
tERROR pr_call System_SyncClose( hOBJECT object ) {
  
  tERROR   error;
  tHANDLE* handle;
  tPO*     po;
  
  PR_TRACE_A0( object, "Enter System::SyncClose" );
  enter_lf();
  
  _HC( handle,po, object, &error );
  if ( handle ) {
    if ( !handle->sync ) {
      error = errOBJECT_NOT_FOUND;
      PR_TRACE(( object, prtNOTIFY, "krn\tSynchronization object not exsists" ));
    }
    /*
    else if ( 0 == (handle->flags & hf_PROTECT_OBJECT_SET) ) {
    error = errOBJECT_INCOMPATIBLE;
    PR_TRACE(( object, prtNOTIFY, "krn\tSynchronization object cannot be closed" ));
    }
    */
    else {
      hOBJECT sync;
      tDATA   iids[2] = {0};
      
      sync = handle->sync;
      wlock(po);
      handle->sync = 0;
      wunlock(po);

      _ObjectClose( po, MakeHandle(sync), CALLER_INFO(object,iids) );
		}
		runlock(po);
	}

	leave_lf();
  PR_TRACE_A1( object, "Leave System::SyncClose error = %terr", error );
	return error;
}




// ---
tERROR pr_call System_SyncLock( hOBJECT object, tDWORD wait_time ) {

	tERROR   error;
	tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;
  
	PR_TRACE_A0( object, "Enter System::SyncLock" );
  enter_lf();

	_HCC( handle, po, lock, object, &error );
	if ( handle ) {

    hMUTEX sync = (hMUTEX)handle->sync;
    runlockc(po,lock);
    
		if ( sync )
			error = sync->vtbl->Lock( sync, wait_time );
		else
			error = errOBJECT_NOT_FOUND;
	}

	leave_lf();
  PR_TRACE_A1( object, "Leave System::SyncLock error = %terr", error );
	return error;
}




// ---
tERROR pr_call System_SyncUnlock( hOBJECT object ) {

	tERROR   error;
	tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;
  
	PR_TRACE_A0( object, "Enter System::SyncLock" );
  enter_lf();

	_HCC( handle, po, lock, object, &error );
	if ( handle != NULL ) {
    
    hMUTEX sync = (hMUTEX)handle->sync;
    runlockc(po,lock);

    if ( sync )
			error = sync->vtbl->Release( sync );
		else
			error = errOBJECT_NOT_FOUND;
	}

	leave_lf();
  PR_TRACE_A1( object, "Leave System::SyncUnlock error = %terr", error );
	return error;
}





// ---
tERROR pr_call System_SyncProtectObject( hOBJECT object, tIID sync_iid, tPID sync_pid, tSTRING name ) {
	tERROR   error;
	tHANDLE* handle;
  tPO*     po;
  
	PR_TRACE_A0( object, "Enter System::SyncProtectObject" );
  enter_lf();

	_HC( handle, po, object, &error );
	if ( handle ) {
		error = _SyncProtectObject( po, handle, sync_iid, sync_pid, name );
		runlock(po);
	}

	leave_lf();
  PR_TRACE_A1( object, "Leave System::SyncProtectObject error = %terr", error );
	return error;
}




// ---
tERROR pr_call System_SyncUnprotectObject( hOBJECT object ) {
	tERROR   error;
	tHANDLE* handle;
  tPO*     po;
  
	PR_TRACE_A0( object, "Enter System::SyncUnprotectObject" );
  enter_lf();

	_HC( handle, po, object, &error );
	if ( handle ) {
		error = _SyncUnprotectObject( po, handle );
		runlock(po);
	}

	leave_lf();
  PR_TRACE_A1( object, "Leave System::SyncUnprotectObject error = %terr", error );
	return error;
}




// ---
tERROR pr_call _SyncCreate( tPO* po, tHANDLE* handle, hOBJECT* sync_result, tIID sync_iid, tPID sync_pid, tSTRING name ) {

	tERROR error;
  tHANDLE* sync;

	PR_TRACE_A2( MakeObject(handle), "Enter _SyncCreate (%tiid, name = %s)", sync_iid, name ? name : "" );

	error = errOK;
	switch( sync_iid ) {
		case IID_CRITICAL_SECTION :
		case IID_SEMAPHORE			  :
		case IID_MUTEX					  :
			//if ( handle->static_link )
			//  error = errOBJECT_INCOMPATIBLE;
			//else {
				if ( 0 != (sync=MakeHandle(handle->sync)) ) {
					if ( handle->flags & hf_PROTECT_OBJECT_SET ) {
						error = errOBJECT_ALREADY_EXISTS;
					}
					else if ( !CHECK_C(sync,sync_iid,sync_pid) ) {
						_ObjectClose( po, sync, 0 );
						sync = 0;
					}
				}
				if ( !sync ) {
					tDATA size;
					error = _ObjectIIDCreate( po, handle, &sync, sync_iid, sync_pid, 0 );
					if ( PR_SUCC(error) && (sync_iid != IID_CRITICAL_SECTION) && name && *name )
						error = _PropertySet( po, sync, &size, pgOBJECT_NAME, name, (tDWORD)slen(name) );
					if ( sync && (PR_FAIL(error) || PR_FAIL(error=_ObjectCreateDone(po,sync))) )
						_ObjectClose( po, sync, 0 );
          wlock(po);
					handle->sync = MakeObject(sync);
          wunlock(po);
				}
			//}
			break;
		default :
			error = errPARAMETER_INVALID; 
			break;
	}

	if ( sync_result )
		*sync_result = handle->sync;

	PR_TRACE_A2( MakeObject(handle), "Leave _SyncCreate ret tHANDLE = %p, error = %terr", handle->sync, error );
	return error;
}




// ---
/*
tERROR pr_call _SyncClose( tHANDLE* handle ) {

	tERROR error;

	PR_TRACE_A0( MakeObject(handle), "Enter _SyncClose ()" );

	if ( handle->sync )
		error = _ObjectClose( MakeHandle(handle->sync) );
	else
		error = errOK;

	PR_TRACE_A1( MakeObject(handle), "Leave _SyncClose error = %terr", error );
	return error;
}                                                    
*/

static tERROR pr_call _ProtectWrapper( hOBJECT obj ) {
	tERROR error;
	tHANDLE* handle = MakeHandle(obj);

  _PR_ENTER_PROTECTED_EXT( handle, error ) {

	  error = _WrapperCallNext( &obj );

  } _PR_LEAVE_PROTECTED_EXT( error );
  return error;
}



// ---
static const tPTR _ProtectSetter( tHANDLE* handle, tPTR param, tBOOL ex_in, tDWORD num, tDWORD* data ) {
  *data = 0;
  return &_ProtectWrapper;
}





// ---
tERROR pr_call _SyncProtectObject( tPO* po, tHANDLE* handle, tIID sync_iid, tPID sync_pid, tSTRING name ) {
	tERROR  error;

  PR_TRACE_A0( MakeObject(handle), "Enter _SyncProtectObject" );

  if ( PR_SUCC(error= _SyncCreate(po,handle,0,sync_iid,sync_pid,name)) && PR_SUCC(error=_WrapperAdd(po,handle,_ProtectSetter,0)) ) {
    wlock(po);
    handle->flags |= hf_OBJECT_PROTECTED;
    wunlock(po);
  }

	PR_TRACE_A1( MakeObject(handle), "Leave _SyncProtectObject error = %terr", error );
	return error;
}



// ---
tERROR pr_call _SyncUnprotectObject( tPO* po, tHANDLE* handle ) {
	tERROR error;
	PR_TRACE_A0( MakeObject(handle), "Enter _SyncUnprotectObject" );

	if ( handle->flags & hf_OBJECT_PROTECTED ) {
    hOBJECT sync;
    
    wlock(po);
		handle->flags &= ~hf_OBJECT_PROTECTED;

		if ( PR_FAIL(_WrapperWithdraw(po,handle,_ProtectSetter,0)) )
			_set_vtbl( handle, 0 );

    sync = handle->sync;
    handle->sync = 0;
    wunlock(po);
    
    if ( /*!handle->static_link &&*/ sync && (MakeHandle(sync)->parent == handle) && (0 == (handle->flags & hf_PROTECT_OBJECT_SET)) )
			error = _ObjectClose( po, MakeHandle(sync), 0 );
    else
      error = errOK;

  }
	else
		error = errOBJECT_NOT_FOUND;

	PR_TRACE_A1( MakeObject(handle), "Leave _SyncUnprotectObject error = %terr", error );
	return error;
}






