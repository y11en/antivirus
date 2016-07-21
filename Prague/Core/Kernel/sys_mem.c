/*-----------------17.03.00 09:31-------------------
 * Project Prague                                 *
 * Subproject Kernel interfaces                   *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague System API                      *
 *                                                *
--------------------------------------------------*/


#include "kernel.h"
#include <Prague/iface/i_heap.h>

// ---
tERROR pr_call System_MemorySize( hOBJECT object, tDWORD* size ) {

  PR_TRACE(( object, prtERROR, "krn\tSystem::MemorySize became obsolete" ));
  return errOBSOLETE_METHOD_CALLED;

//  tERROR   error;
//  tHANDLE* handle;
//  tPO*     po;
//  tINT     lock = 1;
//
//  PR_TRACE_A0( object, "Enter System::MemorySize" );
//  enter_lf();
//
//  _HCC( handle, po, lock, object, &error );
//  if ( handle ) {
//    if ( size )
//      *size = handle->m_memsize;
//    else
//      *size = 0;
//    runlockc(po,lock,handle);
//  }
//  else
//    error = errPARAMETER_INVALID;
//
//  leave_lf();
//  PR_TRACE_A1( object, "Leave System::MemorySize ret tDWORD = %u", size ? *size : handle ? handle->memsize : 0 );
//  return error;
}



// ---
tERROR pr_call System_MemoryRealloc( hOBJECT object, tDWORD size ) {

  PR_TRACE(( object, prtERROR, "krn\tSystem::MemoryRealloc became obsolete" ));
  return errOBSOLETE_METHOD_CALLED;

//  tERROR   error;
//  tHANDLE* handle;
//  tPO*     po;
//  
//  PR_TRACE_A0( object, "Enter System::MemoryRealloc" );
//  enter_lf();
//
//  _HC( handle, po, object, &error );
//  if ( handle ) {
//    if ( handle->iface && (size < handle->iface->propmaxsize) ) // check size of property
//      error = errOBJECT_DATA_SIZE_UNDERSTATED;
//    else
//      error = _MemoryRealloc( po, handle, size );
//    runlock()
//  }
//
//  leave_lf();
//  PR_TRACE_A1( object, "Leave System::MemoryRealloc ret %terr", error );
//  return error;
}



// ---
tERROR pr_call System_ObjHeapAlloc( hOBJECT object, tPTR* result, tDWORD size ) {
	
	tERROR   error;
	tHANDLE* handle;
	tPTR     ret_val = 0;
	hHEAP    mem;
	tPO*     po;
	tINT     lock = 1;
	
	PR_TRACE_A0( object, "Enter System::ObjHeapAlloc" );
	
	if ( result == NULL )
		error = errPARAMETER_INVALID;
	else if ( !size )
		error = errOK;
	else {
		enter_lf();
		_HCM( handle, po, lock, object, &error );
		if ( handle ) {
			tHANDLE* h = handle;
			while( h && !h->mem )
				h = h->parent;
			if ( h && h->mem )
				mem = h->mem;
			else
				mem = g_heap;
			if ( PR_SUCC(error=mem->vtbl->Alloc(mem,&ret_val,size+sizeof(hHEAP))) ) {
				*(hHEAP*)ret_val = mem;
				(*(hHEAP**)&ret_val)++;
			}
			runlockc(po,lock);
		}
		leave_lf();
	}

	if ( result )
		*result = ret_val;

	PR_TRACE_A2( object, "Leave System::ObjHeapAlloc ret %terr, ptr = %p", error, ret_val );
	return error;
}




// ---
tERROR pr_call System_ObjHeapRealloc( hOBJECT object, tPTR* result, tPTR orig, tDWORD size ) {
	
	tERROR   error;
	tPTR     ret_val = 0;
	tPO*     po;
	tINT     lock = 1;
	tHANDLE* h;
	PR_TRACE_A0( object, "Enter System::ObjHeapRealloc" );
	
	if ( result == NULL )
		error = errPARAMETER_INVALID;
	
	else if ( !orig ) 
		error = System_ObjHeapAlloc( object, &ret_val, size );
	
	else {
		enter_lf();
		if ( 0 != _HCM(h,po,lock,object,&error) ) {
			hHEAP heap;
			
			(*(hHEAP**)&orig)--;
			heap = *(hHEAP*)orig;
			
			#ifdef _DEBUG
				if ( !_HC(h,0,(hOBJECT)heap,&error) ) {
					PR_TRACE(( object, prtERROR, "krn\tpointer to memory block (%p) is not valid", (*(hHEAP**)&orig)+1 ));
					ret_val = 0;
					error = errPARAMETER_INVALID;
					INT3; //Graf 28.02.03
				}
				else {
			#endif
					error = heap->vtbl->Realloc( heap, &ret_val, orig, size+sizeof(hHEAP) );
					if ( PR_SUCC(error) ) {
						if ( orig != ret_val )
							*(hHEAP*)ret_val = heap;
						(*(hHEAP**)&ret_val)++;
					}
					else
						ret_val = 0;
			#ifdef _DEBUG
				}
			#endif
			
			runlockc(po,lock);
		}
		leave_lf();
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( object, "Leave System::ObjHeapRealloc ret %terr, ptr = %p", error, ret_val );
	return error;
}



// ---
tERROR pr_call System_ObjHeapFree( hOBJECT object, tPTR ptr ) {
	
  tERROR error;
  tHANDLE* h;
  PR_TRACE_A0( object, "Enter System::ObjHeapFree" );
  //enter_lf();
	
	if ( !ptr )
		error = errOK;
	else if ( _HC(h,0,object,&error) ) {
    (*(hHEAP**)&ptr)--;
		#ifdef _DEBUG
			if ( !_HC(h,0,*(hOBJECT*)ptr,&error) ) {
				PR_TRACE(( object, prtERROR, "krn\tpointer to memory block (%p) is not valid", (*(hHEAP**)&ptr)+1 ));
				error = errPARAMETER_INVALID;
				INT3;
			}
			else {
		#endif

      error = (*(hHEAP*)ptr)->vtbl->Free( *(hHEAP*)ptr, ptr );

		#ifdef _DEBUG
			}
		#endif

    //runlock(po);
	}
		
	//leave_lf();
  PR_TRACE_A1( object, "Leave System::ObjHeapFree ret %terr", error );
	return error;
}



// ---
tERROR pr_call System_ObjHeapResolve( hOBJECT object, hHEAP* result, tPTR ptr ) {
	
	tERROR error;
	hHEAP  heap;
	PR_TRACE_A0( object, "Enter System::ObjHeapResolve" );
	//enter_lf();
	
	if ( !ptr ) {
		heap = 0;
		error = errPARAMETER_INVALID;
	}
	//else if ( !_HC(h,0,object,&error) ) 
	//  heap = 0;
	else {
		
		#ifdef _DEBUG
			tHANDLE* h;
		#endif

		heap = *((*(hHEAP**)&ptr)-1);

		#ifdef _DEBUG
			if ( !_HC(h,0,(hOBJECT)heap,&error) ) {
				PR_TRACE(( object, prtERROR, "krn\tpointer to memory block (%p) is not valid(%terr)", ptr, error ));
				error = errPARAMETER_INVALID;
				INT3; //Graf 28.02.03
			}
		#endif
  }
		
	if ( result )
		*result = heap;

	//leave_lf();
	PR_TRACE_A2( object, "Leave System::ObjHeapResolve ret %terr, hHEAP = %p", error, heap );
  return error;
}



// ---
tBOOL FindHandleByIfacePtr( tHANDLE* h, tPTR p ) {
  return (h->iface == p) && OBJ_IS_NOT_CONDEMNED(h);
}


// ---
//tBOOL FindHandleByStaticHandle( tHANDLE* h, tPTR p ) {
//  return h->static_link == p;
//}


// ---
//static void pr_call _SetNewMemForFakedStatic( tPO* po, tHANDLE* real_static, tINTERFACE* iface, tPTR mem, tDWORD size ) {
//  tHANDLE* handle;
//  tDWORD   cookie = 0;
//  
//  while ( !real_static && (0 != (handle=_HandleFindStat(po,&cookie,FindHandleByIfacePtr,iface))) )
//    real_static = handle->static_link;
//
//  if ( real_static ) {
//    real_static->memsize = size;
//    *hdata(real_static) = mem;
//    cookie = 0;
//    while( 0 != (handle=_HandleFindStat(po,&cookie,FindHandleByStaticHandle,real_static)) ) {
//      handle->memsize = size;
//      *((tPTR*)&handle->obj.data) = mem;
//    }
//  }
//}


// ---
tERROR pr_call _MemoryRealloc( tPO* po, tHANDLE* handle, tDWORD size ) {

  tOBJECT* obj;
  tERROR   error;
  PR_TRACE_A0( MakeObject(handle), "Enter _MemoryRealloc" );

  if ( handle->obj == NULL ) {  // alloc new memory
		error = PrAlloc( (tPTR*)&obj, sizeof(tOBJECT) + size );
    if ( PR_SUCC(error) ) {
      wlock(po);
      handle->obj = obj;
      _ObjectInit0( handle );
      wunlock(po);
    }
  }

  else {
    wlock(po);
    if ( PR_SUCC(error=PrRealloc((tPTR*)&handle->obj,handle->obj,sizeof(tOBJECT)+size)) ) {
      *odata(handle->obj) = handle->obj + 1;
    }
    wunlock(po);
  }

  PR_TRACE_A1( MakeObject(handle), "Leave _MemoryRealloc ret %terr", error );
  return error;
}



