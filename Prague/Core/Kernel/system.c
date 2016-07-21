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

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_tracer.h>


// system interface table
iSYSTEMVTBL si = {

  // property management functions -- sys_prop.c
  System_PropertyGet,
  System_PropertyGetBool,
  System_PropertyGetWord,
  System_PropertyGetUInt,
  System_PropertyGetDWord,
  System_PropertyGetQWord,

  System_PropertySet,
  System_PropertySetBool,
  System_PropertySetWord,
  System_PropertySetUInt,
  System_PropertySetDWord,
  System_PropertySetQWord,

  System_PropertyDelete,
  System_PropertySize,

  System_PropertyBroadcast,

  // object management sys_obj.c
  System_ParentGet,
  System_ParentSet,
  System_HandleCreate,
  System_InterfaceSet,
  System_ObjectCreate,
  System_ObjectCreateDone,
  System_ObjectCheck,
  System_ObjectValid,
  System_ObjectClose,
  System_ObjectSwap,
  System_ObjectFind,

  // error management functions -- here
  System_ErrorCodeGet,

  // message management functions -- sys_msg.c
  System_BroadcastMsg,
  System_SendMsg,
  System_RegisterMsgHandler,
  System_UnregisterMsgHandler,

  // trace management -- sys_trc.c
  System_TraceLevelGet,
  System_TraceLevelSet,
  System_TracerGet,

  // code protection methods -- system.c
  System_EnterProtectedCode,
  System_LeaveProtectedCode,
  System_GetExceptionInfo,
  System_ThrowException,

  // synchronization
  System_SyncCreate,
  System_SyncGet,
  System_SyncClose,
  System_SyncLock,
  System_SyncUnlock,
  System_SyncProtectObject,
  System_SyncUnprotectObject,

  System_ObjectCreateQuick,

  // user data -- system.c
  System_UserDataGet,
  System_UserDataSet,

	// child/object enumerate methods -- sys_obj.c
	System_ChildGetFirst,
	System_ChildGetLast,
	System_ObjectGetNext,
	System_ObjectGetPrev,

	// ordinary memory management
	System_ObjHeapAlloc,
	System_ObjHeapRealloc,
	System_ObjHeapFree,
	System_ObjHeapResolve,

	System_SyncSet,

  System_PropertyGetStr,
  System_PropertySetStr,
  System_PropertyGetStrCP,
  System_PropertyIsPresent,
	System_ObjectRevert,
	
  System_PropertyGetPtr,
  System_PropertyGetObj,
  System_PropertySetPtr,
  System_PropertySetObj,
  
  System_PropertyEnumId,
  System_PropertyEnumCustomId,

  System_PropertyGetByte,
  System_PropertySetByte,
	
  System_RegisterMsgHandlerList,
  System_GetMsgHandlerList,
  System_UnregisterMsgHandlerList,
  
  System_IntMsgReceive,

  System_ObjectGetInternalData,
  System_ObjectSetInternalData,

  System_Recognize,
  System_RecognizeByList,

	System_ObjectValidateLock,
	System_ObjectRelease,

  // rserved method pointers -- here
  { 0 },
  
  // memory management -- sys_mem.c
  System_MemorySize,
  System_MemoryRealloc,

  // error management functions -- here
  System_ErrorCodeSet,
	System_PropertyInitialized,
};



// ---
tERROR pr_call System_UserDataGet( hOBJECT object, tDWORD* data ) {
  tERROR   error;
  tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;

  PR_TRACE_A0( object, "Enter System::UserDataGet" );
  enter_lf();

  if ( 0 != _HCC(handle,po,lock,object,&error) ) {
		if ( data ) {
			if ( handle->ext )
				*data = handle->ext->user_data;
			else {
				error = errNOT_FOUND;
				*data = 0;
			}
      runlockc(po,lock);
    }
		else if ( handle->ext )
			error = errOK;
		else
			error = errNOT_FOUND;
  }
  else {
    PR_TRACE(( object, prtIMPORTANT, "krn\tSystem::UserDataGet error = errPARAMETER_INVALID (data == 0)" ));
  }

  leave_lf();
  #ifdef _PRAGUE_TRACE_
    if ( data )
      PR_TRACE_A2( object, "Leave System::UserDataGet ret tDWORD = %u, %terr", error, *data );
    else
      PR_TRACE_A1( object, "Leave System::UserDataGet ret tDWORD = ???, %terr", error );
  #endif
  return error;
}




// ---
tERROR pr_call System_UserDataSet ( hOBJECT object, tDWORD data ) {
  tERROR   error;
  tHANDLE* handle;
  tPO*     po;
	tINT     lock = 1;

  PR_TRACE_A0( object, "Enter System::UserDataSet" );
  enter_lf();

  if ( 0 != _HCC(handle,po,lock,object,&error) ) {
		tHANDLEX* ext;
    wlock(po);
		if ( PR_SUCC(_chk_ext(po,handle,ext,error)) )
			ext->user_data = data;
    wunlock(po);
    runlockc(po,lock);
  }

  leave_lf();
  PR_TRACE_A1( object, "Leave System::UserDataSet ret %terr", error );
  return error;
}




// ---
tERROR pr_call System_Reserved( hOBJECT object ) {
  return errNOT_IMPLEMENTED;
}



// ---
void pr_call System_ErrorCodeSet( hOBJECT object, tERROR error ) {

  PR_TRACE(( object, prtERROR, "krn\tSystem::ErrorCodeSet became obsolete" ));

	/*
  tERROR   err;
  tHANDLE* handle;
  PR_TRACE_A1( object, "Enter System::ErrorCodeSet(%terr)", error );

  if ( (error != errOK) && (0!=_HC(handle,po,object,&err)) ) {
    sync_lock( g_cs, handle );
    handle->error = error;
    sync_unlock( g_cs, handle );
  }

  PR_TRACE_A0( object, "Leave System::ErrorCodeSet ret tVOID" );
	*/
}



// ---
tERROR pr_call System_ErrorCodeGet( hOBJECT object ) {

  PR_TRACE(( object, prtERROR, "krn\tPrKernel\tSystem::ErrorCodeSet became obsolete" ));
  return errOBSOLETE_METHOD_CALLED;
	
	/*
  tERROR error;
  tHANDLE* handle;
  PR_TRACE_A0( object, "Enter System::ErrorCodeGet" );

  _HC( handle, po, object, &error );
  if ( handle != NULL ) {
    sync_lock( g_cs, handle );
    error = handle->error;
    handle->error = errOK;
    sync_unlock( g_cs, handle );
  }

  PR_TRACE_A1( object, "Leave System::ErrorCodeGet ret %terr", error );
  return error;
	*/
}



// ---
tERROR pr_call System_EnterProtectedCode( hOBJECT object, tExceptionReg* reg ) {
  if ( reg ) {
    mset( reg, 0, sizeof(tExceptionReg) );
    return errOK;
  }
  else
    return errPARAMETER_INVALID;
}



// ---
tERROR pr_call System_LeaveProtectedCode( hOBJECT object, tExceptionReg* reg ) {
  return errNOT_SUPPORTED;
}



// ---
tERROR pr_call System_GetExceptionInfo( hOBJECT object, tExceptionReg* reg, tDWORD* ex_code, tPTR* info ) {
  if ( ex_code )
    *ex_code = 0;
  if ( info )
    *info = 0;
  return errNOT_SUPPORTED;
}



// ---
tERROR pr_call System_ThrowException( hOBJECT object, tDWORD ex_code ) {
  return errNOT_SUPPORTED;
}



