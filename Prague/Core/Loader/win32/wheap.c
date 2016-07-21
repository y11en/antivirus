// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  18 August 2004,  11:14 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- wheap.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end

#if defined (_WIN32)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Heap_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include "WHeap.h"
#include "pr_dbg_info.h"
#include "loader.h"
// AVP Prague stamp end

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktrace.h>

#include <minmax.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Heap interface implementation
// Short comments -- распределение, освобождение памяти
//    Интерфейс предоставляет услуги по распределению, перераспределению и освобождению регионов памяти.
// AVP Prague stamp end




HANDLE create_heap_protector;

/* #define enter  \ */
/*   if ( alloc_count_protector ) \ */
/*     WaitForSingleObject( alloc_count_protector, INFINITE ) */

/* #define leave \ */
/*   if ( alloc_count_protector ) \ */
/*     WaitForSingleObject( alloc_count_protector, INFINITE ) */

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Heap. Inner data structure
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Heap_ObjectInitDone( hi_Heap _this );
tERROR pr_call Heap_ObjectClose( hi_Heap _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Heap_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        NULL,
	(tIntFnObjectInitDone)    Heap_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       Heap_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpHeap_Alloc)   ( hi_Heap _this, tPTR* result, tDWORD size );               // -- //rus:распределяет регион памяти размером "size" байт.//eng:Allocate "size" bytes from heap;
typedef   tERROR (pr_call *fnpHeap_Realloc) ( hi_Heap _this, tPTR* result, tPTR ptr, tDWORD size );     // -- //rus:Изменяет размер ранее распределенного региона пямяти//eng:Changes size of previously allocated block;
typedef   tERROR (pr_call *fnpHeap_Free)    ( hi_Heap _this, tPTR ptr );                  // -- //rus:освобождает ранее распределенный регион.//eng:frees previously allocated block. ;
typedef   tERROR (pr_call *fnpHeap_Size)    ( hi_Heap _this, tDWORD* result, tPTR ptr );                  // -- //rus:возвращает размер распределенного региона//eng:returns size of previously allocated block;
typedef   tERROR (pr_call *fnpHeap_Clear)   ( hi_Heap _this );                            // -- //rus:освободить все регионы, выделенные этим объектом//eng:Free all alocations made from this heap;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iHeapVtbl {
	fnpHeap_Alloc    Alloc;
	fnpHeap_Realloc  Realloc;
	fnpHeap_Free     Free;
	fnpHeap_Size     Size;
	fnpHeap_Clear    Clear;
}; // "Heap" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Heap_Alloc( hi_Heap _this, tPTR* result, tDWORD size );
tERROR pr_call Heap_Realloc( hi_Heap _this, tPTR* result, tPTR ptr, tDWORD size );
tERROR pr_call Heap_Free( hi_Heap _this, tPTR ptr );
tERROR pr_call Heap_Size( hi_Heap _this, tDWORD* result, tPTR ptr );
tERROR pr_call Heap_Clear( hi_Heap _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iHeapVtbl e_Heap_vtbl = {
	(fnpHeap_Alloc)           Heap_Alloc,
	(fnpHeap_Realloc)         Heap_Realloc,
	(fnpHeap_Free)            Heap_Free,
	(fnpHeap_Size)            Heap_Size,
	(fnpHeap_Clear)           Heap_Clear
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Heap_PROP_heap_size( hi_Heap _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Heap". Static(shared) property table variables
tDWORD g_alloc_all = 0; // must be READ_ONLY at runtime
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Heap_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Heap_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Heap_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Windows based Heap", 19 )
	mLOCAL_PROPERTY_BUF( pgHEAP_GRANULARITY, HeapData, m_granularity, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgHEAP_MOVE_ALLOWED, HeapData, m_move_allowed, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plEARLY_INITIALIZED, HeapData, m_early_initialized, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
#if defined(MEM_CHECK)
	mLOCAL_PROPERTY_BUF( plREQUESTED_HEAP_SIZE, HeapData, m_requested_size, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plALLOCATED_POINTERS, HeapData, m_allocated_count, cPROP_BUFFER_READ )
	mSHARED_PROPERTY_PTR( plGLOBAL_ALLOCATED, g_alloc_all, sizeof(g_alloc_all) )
	mLOCAL_PROPERTY_FN( plWIN32_HEAP_SIZE, Heap_PROP_heap_size, NULL )
	#if defined(MEM_CHECK_ADV)
		mLOCAL_PROPERTY_BUF( plWIN32_HEAP_TAG, HeapData, m_check_data.m_head.m_tag, cPROP_BUFFER_READ_WRITE )
	#endif
#endif
mPROPERTY_TABLE_END(Heap_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Heap". Register function
tERROR pr_call Heap_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"Heap::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_HEAP,                               // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		Heap_VERSION,                           // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Heap_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Heap_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Heap_vtbl,                           // external function table
		(sizeof(e_Heap_vtbl)/sizeof(tPTR)),     // external function table size
		Heap_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Heap_PropTable),   // property table size
		sizeof(HeapData),                       // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ldr\tHeap(IID_HEAP) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"Heap::Register\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


#ifdef MEM_CHECK

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, heap_size )
// Interface "Heap". Method "Get" for property(s):
//  -- WIN32_HEAP_SIZE
tERROR pr_call Heap_PROP_heap_size( hi_Heap _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter *GET* method \"Heap_PROP_heap_size\" for property \"plWIN32_HEAP_SIZE\"" );

  if ( buffer )
		*(tDWORD*)buffer = _heap_calc_tagged_size( _this );
	*out_size = sizeof(tDWORD);
		
	PR_TRACE_A2( _this, "Leave *GET* method \"Heap_PROP_heap_size\" for property \"plWIN32_HEAP_SIZE\", ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end


#endif

// ---
tERROR pr_call _InitHeap( hi_Heap heap ) {
	HeapData* d = heap->data;
	
	WaitForSingleObject( create_heap_protector, INFINITE );
	d->m_hHeap = HeapCreate( 0, 0, 0 );
	ReleaseMutex( create_heap_protector );
	
	PR_TRACE(( heap, prtNOT_IMPORTANT, "ldr\tPrHeap(0x%p) - WinHeap(0x%p) created", heap, d->m_hHeap ));

	if ( d->m_hHeap ) {
		_heap_check_init( heap );
		return errOK;
	}
	return errOBJECT_CANNOT_BE_INITIALIZED;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Heap_ObjectInitDone( hi_Heap _this ) {
	tERROR error;
	HeapData* d;
	PR_TRACE_A0( _this, "Enter \"Heap::ObjectInitDone\" method" );
	
	d = _this->data;
  if ( d->m_early_initialized )
    error = _InitHeap( _this );
  else
    error = errOK;

	PR_TRACE(( _this, prtNOT_IMPORTANT, "ldr\tPrHeap(0x%p)/WinHeap(0x%p) created - %terr", _this, d->m_hHeap, error ));
	PR_TRACE_A1( _this, "Leave \"Heap::ObjectInitDone\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call Heap_ObjectClose( hi_Heap _this ) {
	
	HeapData* d;
	HANDLE heap = 0;
	tBOOL succ = cTRUE;
	PR_TRACE_A0( _this, "Enter \"Heap::ObjectClose\" method" );
	
	d = _this->data;

	#if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
		_heap_check_trace_output( _this, prtERROR );
	#endif

	_heap_check_deinit( _this );

	if ( d->m_hHeap != NULL )	{
		heap = d->m_hHeap;
		d->m_hHeap = 0;
		_heap_locked_sub( &g_alloc_all, d->m_requested_size );

		WaitForSingleObject( create_heap_protector, INFINITE );
		if ( HeapValidate(heap,0,0) )
			HeapDestroy( heap );
		else
			succ = cFALSE;
		ReleaseMutex( create_heap_protector );
	}

	if ( succ ) {
		PR_TRACE(( _this, prtNOT_IMPORTANT, "ldr\tPrHeap(0x%p)/WinHeap(0x%p) destroyed", _this, heap ));
	}
	else {
		PR_TRACE(( _this, prtERROR, "ldr\tPrHeap(0x%p)/WinHeap(0x%p): error checking heap before destroy", _this, heap ));
	}

	PR_TRACE_A0( _this, "Leave \"Heap::ObjectClose\" method, ret tERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Alloc )
// Extended method comment
//  Allocates size bytes from heap. If this heap does not contain required amount of bytes new heap will be created and memory will allocated from new one. Size of new heap will HEAP_GRANULARITY
// Result comment
//  ppointer to allocated memory
// Parameters are:
tERROR pr_call Heap_Alloc( hi_Heap _this, tPTR* result, tDWORD size ) {
	tERROR error;
	tPTR   ret_val;
	HeapData* d;
	PR_TRACE_A0( _this, "Enter \"Heap::Alloc\" method" );

	d = _this->data;

    ret_val = NULL;

  if ( d->m_hHeap )
    error = errOK;
  else if ( d->m_early_initialized )
    error = errOBJECT_NOT_INITIALIZED;
  else {
    d->m_early_initialized = cTRUE;
    error = _InitHeap( _this );
  }

	if ( PR_SUCC(error) ) {
		_heap_check_pre_alloc( _this, &size );
		if ( 0 != (ret_val=HeapAlloc(d->m_hHeap,HEAP_ZERO_MEMORY,size)) ) {
			_heap_check_post_alloc( _this, &ret_val, &size );
			_heap_locked_add( &d->m_allocated_count, 1 );
			_heap_locked_add( &d->m_requested_size, size );
			_heap_locked_add( &g_alloc_all, size );
			error = errOK;
		}
		else {
			tDWORD e = GetLastError();
			_heap_check_post_alloc( _this, &ret_val, &size );
			PR_TRACE(( _this, prtERROR, "ldr\tHeapAlloc failed. Win error - %u(0x%08x)", e, e ));
			error = errUNEXPECTED;
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"Heap::Alloc\" method, ret tPTR = 0x%p, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Realloc )
// Extended method comment
//  changes size of previvously allocated block. Procedure tries to preserve pointer to block. If it is not possible behaviour depends on HEAP_MOVE_ALLOWED property. If property is cTRUE function will return new pointer. All data in memory block will copied into new one, If move is disabled function will fail
// Result comment
//  pointer to reallocated block or NULL if function fails.
// Parameters are:
tERROR pr_call Heap_Realloc( hi_Heap _this, tPTR* result, tPTR ptr, tDWORD size ) {
	tERROR error;
	tPTR   ret_val;
	HeapData* d;
  tDWORD old_size;

	if ( !ptr )
		return Heap_Alloc( _this, result, size );
	
	PR_TRACE_A0( _this, "Enter \"Heap::Realloc\" method" );

	ret_val = NULL;
  d = _this->data;

	error = _heap_check_obj( _this, &ptr );
	if ( PR_SUCC(error) ) {

    if ( d->m_hHeap )
      error = errOK;
    else if ( d->m_early_initialized )
      error = errOBJECT_NOT_INITIALIZED;
    else {
      d->m_early_initialized = cTRUE;
      error = _InitHeap( _this );
    }

    if ( PR_SUCC(error) ) {
			_heap_check_pre_alloc( _this, &size );
			old_size = _toui32(_heap_check_obj_size(_this, ptr));

      if ( 0 != (ret_val=HeapReAlloc(d->m_hHeap,HEAP_ZERO_MEMORY,ptr,size)) ) {
				_heap_check_post_alloc( _this, &ret_val, &size );
        _heap_locked_add( &d->m_requested_size, size - old_size );
        _heap_locked_add( &g_alloc_all, size - old_size );
        error = errOK;
  	  }
  	  else {
  		  tDWORD e = GetLastError();
				_heap_check_post_alloc( _this, &ptr, &size );
        PR_TRACE(( _this, prtERROR, "ldr\tHeapReAlloc failed. Win error - %u(0x%08x)", e, e ));
  		  error = errUNEXPECTED;
      }
    }
  }	

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"Heap::Realloc\" method, ret tPTR = 0x%p, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Free )
// Parameters are:
tERROR pr_call Heap_Free( hi_Heap _this, tPTR ptr ) {
  tERROR error;
  HeapData* d;

  PR_TRACE_A0( _this, "Enter \"Heap::Free\" method" );

  d = _this->data;

  if ( d->m_hHeap )
    error = errOK;
  else if ( d->m_early_initialized )
    error = errOBJECT_NOT_INITIALIZED;
  else { // initialize on Free?!? nonsense!!!
    error = errOBJECT_NOT_INITIALIZED;
    //d->m_early_initialized = cTRUE;
    //error = _InitHeap( _this );
  }

	if ( PR_SUCC(error) )
		error = _heap_check_pre_delete( _this, &ptr );

  if ( PR_SUCC(error) ) {
    tDWORD old_size = _toui32(_heap_check_obj_size(_this,ptr));
	
    if ( HeapFree(d->m_hHeap,0,ptr) ) {
			_heap_check_post_delete( _this, ptr );
			_heap_locked_sub( &d->m_allocated_count, 1 );
			_heap_locked_sub( &d->m_requested_size,  min(d->m_requested_size, old_size));
			_heap_locked_sub( &g_alloc_all, min(d->m_requested_size, old_size) );
    }
    else {
			_heap_check_post_delete( _this, ptr );
			error = errUNEXPECTED;
		}
  }

	PR_TRACE_A1( _this, "Leave \"Heap::Free\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Size )
// Result comment
//  size of allocated block. If pointer is not valid function will return 0 and error code will set.
// Parameters are:
tERROR pr_call Heap_Size( hi_Heap _this, tDWORD* result, tPTR ptr ) {
	tERROR error;
	tDWORD ret_val;
	PR_TRACE_A0( _this, "Enter \"Heap::Size\" method" );

	error = _heap_check_obj( _this, &ptr );
	if ( PR_SUCC(error) )
		ret_val = _toui32(_heap_check_obj_size(_this,ptr));
	else
		ret_val = 0;
	
	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"Heap::Size\" method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clear )
// Parameters are:
tERROR pr_call Heap_Clear( hi_Heap _this ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"Heap::Clear\" method" );

	Heap_ObjectClose( _this );
	memset( _this->data, 0, sizeof(HeapData) );
	error = Heap_ObjectInitDone( _this );
	
	PR_TRACE_A1( _this, "Leave \"Heap::Clear\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


#endif //_WIN32
