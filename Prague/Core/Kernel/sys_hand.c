/*-----------------17.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"



// ---
tERROR pr_call System_HandleCreate( hOBJECT object, hOBJECT* new_obj, tDWORD memsize ) {

  PR_TRACE(( object, prtERROR, "krn\tSystem::HandleCreate became obsolete" ));
	return errOBSOLETE_METHOD_CALLED;

//  tERROR   error;
//  tHANDLE* handle;
//  tHANDLE* new_handle;
//  tPO*     po;
//
//  PR_TRACE_A0( object, "Enter System::HandleCreate" );
//  enter_lf();
//
//  _HC( handle, po, object, &error );
//  if ( handle ) {
//    error = _HandleCreate( po, handle, &new_handle, memsize );
//    runlock(po);
//  }
//
//  if ( new_obj )
//    *new_obj = MakeObject(new_handle);
//
//  leave_lf();
//  PR_TRACE_A2( object, "Leave System::HandleCreate ret hOBJECT = %p, %terr", MakeObject(new_handle), error );
//  return error;
}




// ---
tERROR pr_call System_InterfaceSet( hOBJECT object, tIID iid, tPID pid, tDWORD subtype ) {

  PR_TRACE(( object, prtERROR, "krn\tSystem::InterfaceSet became obsolete" ));
  return errOBSOLETE_METHOD_CALLED;

//  tHANDLE* handle;
//  tERROR   error;
//  tPO*     po;
//  PR_TRACE_A0( object, "Enter System::InterfaceSet" );
//  enter_lf();
//
//  if ( (iid == IID_NONE) || (iid == IID_ANY) ) // ??? || (pid == PID_ANY)
//    error = errPARAMETER_INVALID;
//
//  else if ( 0 != (handle=_HandleCheck(po,object,&error,1)) ) {
//    
//    if ( (handle->iid != IID_NONE) && (handle->iid != iid) )
//      error = errINTERFACE_INCOMPATIBLE;
//
//    else {
//      tDWORD pos = 0;
//      tINTERFACE* iface;
//      
//      error = errINTERFACE_NOT_FOUND;
//      iface = _InterfaceFind( &pos, iid, pid, subtype, VID_ANY, cFALSE );
//
//      while( iface ) {
//        if ( !(iface->flags & IFACE_CONDEMNED) ) {
//          error = _InterfaceReplace( po, handle, iface );
//          if ( error != errINTERFACE_NOT_LOADED )
//            break;
//        }
//        iface = _InterfaceFind( &pos, iid, pid, subtype, VID_ANY, cFALSE );
//      }
//    }
//    runlock(po);
//  }
//  
//  leave_lf();
//  PR_TRACE_A1( object, "Leave System::InterfaceSet ret t%terr", error );
//  return error;
}



// ---
tERROR pr_call _Recognize( tPO* po, tHANDLE* handle, tIID iid, tPID pid, tDWORD type ) {
  
  tERROR      error;
  tINTERFACE* iface = _InterfaceFind( 0, iid, pid, SUBTYPE_ANY, VID_ANY, cTRUE );
  
  if ( !iface ) {
    error = errINTERFACE_NOT_FOUND;
    PR_TRACE(( MakeObject(handle), prtERROR, "krn\tInterface(%tiid,%tpid) cannot be loaded %terr", iid, pid, error ));
  }
  else if ( PR_FAIL(error=_InterfaceLoad(po,iface)) ) {


    PR_TRACE(( MakeObject(handle), prtERROR, "krn\tInterface(%tiid,%tpid) cannot be loaded %terr", iid, pid, error ));
  }
  else if ( !iface->internal->Recognize ) {
    error = errINTERFACE_HAS_NO_RECOGNIZER;
    PR_TRACE(( MakeObject(handle), prtSPAM, "krn\tInterface(%tiid,%tpid) has no recognizer", iid, pid ));
  }
  
  else {
		_PR_ENTER_PROTECTED_0(po) {
      error = iface->internal->Recognize( MakeObject(handle), type );
		} _PR_LEAVE_PROTECTED_0( po, error );
    PR_TRACE(( MakeObject(handle), prtNOTIFY, "krn\tinterface internal Recognize(%tiid,%tpid) returns %terr", iid, pid, error ));
  }
	if ( iface )
		_InterfaceUnload( po, iface );
  return error;
}



// ---
tERROR pr_call System_Recognize( hOBJECT object, tIID iid, tPID pid, tDWORD type ) {
  tHANDLE* handle;
  tERROR   error;
  tPO*     po;
	tINT     lock = 1;

  PR_TRACE_A0( object, "Enter System::Recognize" );
  enter_lf();

  if ( (iid == IID_NONE) || (iid == IID_ANY) || (pid == PID_ANY) )
    error = errPARAMETER_INVALID;
  
  else if ( 0 != _HCC(handle,po,lock,object,&error) ) {
    error = _Recognize( po, handle, iid, pid, type );
    runlockc(po,lock);
  }
    
  leave_lf();
  PR_TRACE_A1( object, "Leave System::Recognize ret t%terr", error );
  return error;
}



// ---
tERROR pr_call System_RecognizeByList( hOBJECT object, tPID* result, tIID iid, const tPID* pid_list, tDWORD count, tDWORD type ) {
  tHANDLE* handle;
  tERROR   error;
  tPO*     po;
	tINT     lock = 1;
  PR_TRACE_A0( object, "Enter System::RecognizeByList" );
  enter_lf();
  
  if ( result )
    *result = PID_ANY;

  if ( (iid == IID_NONE) || (iid == IID_ANY) )
    error = errPARAMETER_INVALID;
  
  else if ( 0 != _HCC(handle,po,lock,object,&error) ) {
    tUINT i;
    error = errINTERFACE_NOT_FOUND;
    for( i=0; i<count; i++,pid_list++ ) {
      tERROR l_err;
      if ( *pid_list == PID_ANY )
        continue;
      l_err = _Recognize( po, handle, iid, *pid_list, type );
      if ( PR_SUCC(l_err) ) {
        error = l_err;
        if ( result )
          *result = *pid_list;
        break;
      }
    }
    runlockc(po,lock);
  }
  
  leave_lf();
  PR_TRACE_A1( object, "Leave System::RecognizeByList ret t%terr", error );
  return error;
}



