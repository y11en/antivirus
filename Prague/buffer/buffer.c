// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  17 February 2004,  15:56 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- buffer interface
// Purpose     -- plugin implements buffer interface
// Author      -- petrovitch
// File Name   -- buffer.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Buffer_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "buffer.h"
// AVP Prague stamp end

hROOT g_root = NULL;

typedef struct tag_QWord {
  #if defined (MACHINE_IS_BIG_ENDIAN)
	  tDWORD h;
	  tDWORD l;	
  #else	
	  tDWORD l;
	  tDWORD h;
  #endif
} tQWord;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Buffer interface implementation
// Short comments -- интерфейс доступа к региону памяти//eng:"piece of memory" interface
//    //rus:Интерфейс предоставляет методы работы с регионом памяти, такие как блокирование разблокирование, получение указателя на память.
//    Интерфейс совместим с интерфейсом "IO", т.е. содержит все его методы и свойства. Это позволяет использовать объект интерфейса как с объект интерфейса "IO" в любом контексте, где используется "IO".
//    Объект интерфейса поддерживает внутренний счетчик блокировок, и если он не 0, то метды "SeekWrite", "SetSize", "AddLock" и "SizeLock" возвращают ошибку "errOBJECT_IS_LOCKED".
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Buffer. Inner data structure

typedef struct tag_BufferData {
  tORIG_ID origin; // object origin
  tUINT    locks;  // lock count
  tUINT    size;   // size of the memory piece
  tBYTE*   ptr;    // memory piece
	tBOOL    preallocated; // --
} BufferData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Buffer {
	const iBufferVtbl* vtbl; // pointer to the "Buffer" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	BufferData*        data; // pointer to the "Buffer" data structure
} *hi_Buffer;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Buffer_ObjectInitDone( hi_Buffer _this );
tERROR pr_call Buffer_ObjectPreClose( hi_Buffer _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Buffer_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        NULL,
	(tIntFnObjectInitDone)    Buffer_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Buffer_ObjectPreClose,
	(tIntFnObjectClose)       NULL,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



//#define PB(d)        ((tBYTE*)(d))
//#define PDATA(d)     (PB(d) + sizeof(BufferData))
//#define DATA_SIZE(s) ((s) + sizeof(BufferData))

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )

// -----------  IO declared methods ----------
typedef   tERROR (pr_call *fnpBuffer_SeekRead)  ( hi_Buffer _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- //rus:вычитывает содержимое объекта в буфер памяти//eng:reads content of the object to the buffer;
typedef   tERROR (pr_call *fnpBuffer_SeekWrite) ( hi_Buffer _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- //rus:копирует буфер в регион памяти//eng:writes buffer to the object;
typedef   tERROR (pr_call *fnpBuffer_GetSize)   ( hi_Buffer _this, tQWORD* result, IO_SIZE_TYPE type );     // -- размер распределенного региона памяти//eng:returns size of the requested type of the object;
typedef   tERROR (pr_call *fnpBuffer_SetSize)   ( hi_Buffer _this, tQWORD new_size );       // -- //rus:перераспределяет регион с новым размером//eng:resizes the object;
typedef   tERROR (pr_call *fnpBuffer_Flush)     ( hi_Buffer _this );                        // -- в данном интерфейсе не должен ничего делать//eng:there is nothing to do here;

// -----------  buffer declared methods ----------
typedef   tERROR (pr_call *fnpBuffer_Lock)      ( hi_Buffer _this, tPTR* result );                        // -- //rus:фиксирует регион памяти и возвращает указатель на него//eng:lock memory and get access to it;
typedef   tERROR (pr_call *fnpBuffer_AddLock)   ( hi_Buffer _this, tPTR* result, tDWORD size_to_add );    // -- //rus:добавляет указанный размер к региону и возвращает указатель на добавленную память//eng:add some space to the object and returns pointer to the added space;
typedef   tERROR (pr_call *fnpBuffer_SizeLock)  ( hi_Buffer _this, tPTR* result, tDWORD size );           // -- //rus:перераспределяет регион с новым размером, фиксирует его и вохвращает указатель на содержимое//eng:set size of the buffer, lock it and returns pointer to the content;
typedef   tERROR (pr_call *fnpBuffer_Unlock)    ( hi_Buffer _this );                        // -- //rus:разблокирует регион//eng:unlock memory;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iBufferVtbl {

// -----------  IO declared methods ----------
  fnpBuffer_SeekRead   SeekRead;
  fnpBuffer_SeekWrite  SeekWrite;
  fnpBuffer_GetSize    GetSize;
  fnpBuffer_SetSize    SetSize;
  fnpBuffer_Flush      Flush;

// -----------  buffer declared methods ----------
  fnpBuffer_Lock       Lock;
  fnpBuffer_AddLock    AddLock;
  fnpBuffer_SizeLock   SizeLock;
  fnpBuffer_Unlock     Unlock;
}; // "Buffer" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )

//  IO declared methods
// //rus:методы декларированные в интерфейсе "IO"
tERROR pr_call Buffer_SeekRead( hi_Buffer _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call Buffer_SeekWrite( hi_Buffer _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call Buffer_GetSize( hi_Buffer _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call Buffer_SetSize( hi_Buffer _this, tQWORD new_size );
tERROR pr_call Buffer_Flush( hi_Buffer _this );

//  buffer declared methods
// //rus:методы декларированные в интерфейсе "buffer"
tERROR pr_call Buffer_Lock( hi_Buffer _this, tPTR* result );
tERROR pr_call Buffer_AddLock( hi_Buffer _this, tPTR* result, tDWORD size_to_add );
tERROR pr_call Buffer_SizeLock( hi_Buffer _this, tPTR* result, tDWORD size );
tERROR pr_call Buffer_Unlock( hi_Buffer _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iBufferVtbl e_Buffer_vtbl = {
	(fnpBuffer_SeekRead)      Buffer_SeekRead,
	(fnpBuffer_SeekWrite)     Buffer_SeekWrite,
	(fnpBuffer_GetSize)       Buffer_GetSize,
	(fnpBuffer_SetSize)       Buffer_SetSize,
	(fnpBuffer_Flush)         Buffer_Flush,
	(fnpBuffer_Lock)          Buffer_Lock,
	(fnpBuffer_AddLock)       Buffer_AddLock,
	(fnpBuffer_SizeLock)      Buffer_SizeLock,
	(fnpBuffer_Unlock)        Buffer_Unlock
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Buffer". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Buffer_VERSION ((tVERSION)2)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/plugin/p_buffer.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Buffer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Buffer_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "piece of memory", 16 )

// ----------  IO inherited properties  ----------
// Предполагается, что ВСЕ свойства этого интерфейса будут реализованы статическими, за исключением, возможно, pgOBJECT_OPEN_MODE, pgOBJECT_ACCESS_MODE и pgOBJECT_BASED_ON.
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "", 1 )
	mSHARED_PROPERTY_PTR( pgOBJECT_PATH, "", 1 )
	mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME, "", 1 )
	mSHARED_PROPERTY( pgOBJECT_OPEN_MODE, ((tDWORD)(fOMODE_CREATE_IF_NOT_EXIST|fOMODE_TRUNCATE|fOMODE_SHARE_DENY_RW|fOMODE_SHARE_DENY_DELETE)) )
	mSHARED_PROPERTY( pgOBJECT_ACCESS_MODE, ((tDWORD)(fACCESS_RW|fACCESS_NO_CHANGE_MODE)) )
	mSHARED_PROPERTY( pgOBJECT_DELETE_ON_CLOSE, ((tBOOL)(cTRUE)) )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mSHARED_PROPERTY( pgINTERFACE_COMPATIBILITY, ((tIID)(IID_IO)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, BufferData, origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgLOCK_COUNT, BufferData, locks, cPROP_BUFFER_READ )

// ----------  own properties  ----------
	mLOCAL_PROPERTY_BUF( plPREALLOCATED_BUFFER, BufferData, ptr, cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plPREALLACATED_BUFFER_SIZE, BufferData, size, cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT )
mPROPERTY_TABLE_END(Buffer_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Buffer". Register function
tERROR pr_call Buffer_Register( hROOT root ) {
  tERROR error;
  PR_TRACE_A0( root, "Enter Buffer::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_BUFFER,                             // interface identifier
    PID_BUFFER,                             // plugin identifier
    0x00000000,                             // subtype identifier
    Buffer_VERSION,                         // interface version
    VID_PETROVITCH,                         // interface developer
    &i_Buffer_vtbl,                         // internal(kernel called) function table
    (sizeof(i_Buffer_vtbl)/sizeof(tPTR)),   // internal function table size
    &e_Buffer_vtbl,                         // external function table
    (sizeof(e_Buffer_vtbl)/sizeof(tPTR)),   // external function table size
    Buffer_PropTable,                       // property table
    mPROPERTY_TABLE_SIZE(Buffer_PropTable), // property table size
    sizeof(BufferData),                     // memory size
    IFACE_SYSTEM                            // interface flags
  );

  #ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"Buffer(IID_BUFFER) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave Buffer::Register method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Buffer_ObjectInitDone( hi_Buffer _this ) 
{
	tERROR error;
  BufferData* d;
	PR_TRACE_A0( _this, "Enter Buffer::ObjectInitDone method" );
	
	error = errOK;
	
	d = _this->data;
	if( (!d->ptr) && (!d->size) )
		d->preallocated = cFALSE;
	else if( d->ptr && d->size )
		d->preallocated = cTRUE;
	else
		error = errPROPERTY_VALUE_INVALID; 
	
	PR_TRACE_A1( _this, "Leave Buffer::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call Buffer_ObjectPreClose( hi_Buffer _this ) {
  BufferData* d;
  PR_TRACE_A0( _this, "Enter Buffer::ObjectPreClose method" );

  d = _this->data;
  if ( d->locks ) {
    PR_TRACE(( _this, prtERROR, "Buffer locked(%u) on close", d->locks ));
  }
  if ( !d->preallocated && d->ptr ) {
    CALL_SYS_ObjHeapFree( _this, d->ptr );
    d->ptr = 0;
  }

  PR_TRACE_A0( _this, "Leave Buffer::ObjectPreClose method, ret errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Parameters are:
tERROR pr_call Buffer_SeekRead( hi_Buffer _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
  tERROR      error;
	tDWORD      offs;
  BufferData* d;
  PR_TRACE_A0( _this, "Enter Buffer::SeekRead method" );

	if ( !buffer || !size ) {
		size = 0;
		error = errPARAMETER_INVALID;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekRead(buffer=%p,size=%u) -- parameter(s) invalid", buffer, size) );
	}
	else if ( ((tQWord*)&offset)->h ) {
		size = 0;
		error = errOUT_OF_OBJECT;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekRead(offset=%I64u,size=%u) -- QWORD operation doesn't supported", offset, size) );
	}
  else if ( (d=_this->data), (d->size < (offs=((tQWord*)&offset)->l)) ) {
    size = 0;
    error = errOUT_OF_OBJECT;
    PR_TRACE( (_this,prtIMPORTANT,"Buffer::SeekRead(offset=%u,size=%u) [Attempt to read out of object, memsize=%u]",offs,size,d->size) );
  }
	else if ( d->size == offs ) {
    size = 0;
    error = errEOF;
	}
  else {
		if ( (offs+size) > d->size ) {
      PR_TRACE( (_this,prtIMPORTANT,"Buffer::SeekRead(offset=%u,size=%u) [Requested size is greater than rest of the object(%u)]",offs,size,(d->size-offs)) );
			size = d->size - offs;
      error = errEOF;
    }
    else
      error = errOK;
    memcpy( buffer, d->ptr+offset, size );
  }

  if ( result )
    *result = size;

  PR_TRACE_A2( _this, "Leave Buffer::SeekRead method, ret tDWORD = %u, %terr", size, error );
  return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Parameters are:
tERROR pr_call Buffer_SeekWrite( hi_Buffer _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
  tERROR error;
  tDWORD offs;
  BufferData* d;
  
  PR_TRACE_A0( _this, "Enter Buffer::SeekWrite method" );

	d = _this->data;

  if ( d->preallocated )
		error = errOBJECT_READ_ONLY;
  else if ( !buffer || !size ) {
    size = 0;
    error = errPARAMETER_INVALID;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekWrite() -- parameter(s) invalid (buffer=%p, size=%u", buffer, size) );
  }
	else if ( ((tQWord*)&offset)->h ) {
		size = 0;
		error = errOUT_OF_OBJECT;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekWrite(offset=%I64u,size=%u) -- QWORD operation doesn't supported", offset, size) );
	}
  else if ( d->locks ) {
    size = 0;
    error = errOBJECT_IS_LOCKED;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekWrite() -- Attempt to write to locked(%u) object",d->locks) );
  }
  else if ( (offs=((tQWord*)&offset)->l) > d->size ) {
    size = 0;
    error = errOUT_OF_OBJECT;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekWrite() [Attempt to write out of object, offset=%u, memsize=%u]",offset,d->size) );
  }
  else if ( ((offs+size) > d->size) && PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->ptr,d->ptr,offs+size)) ) {
    size = 0;
    PR_TRACE( (_this,prtERROR,"Buffer::SeekWrite() cannot realloc memory piece to %u",offs+size) );
  }
  else {
    error = errOK;
    if ( (offs+size) > d->size )
      d->size = offs + size;
    memcpy( d->ptr+offs, buffer, size );
  }

  if ( result )
    *result = size;
  PR_TRACE_A2( _this, "Leave Buffer::SeekWrite method, ret tDWORD = %u, %terr", size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//  object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
// "type" : Size type is one of the following:
//            -- explicit
//            -- approximate
//            -- estimate
//
tERROR pr_call Buffer_GetSize( hi_Buffer _this, tQWORD* ret, IO_SIZE_TYPE type ) {
	PR_TRACE_A0( _this, "Enter Buffer::GetSize method" );

	if ( ret ) {
    ((tQWord*)ret)->h = 0;
    ((tQWord*)ret)->l = _this->data->size;
	}

  PR_TRACE_A2( _this, "Leave Buffer::GetSize method, ret tQWORD = %I64u, %terr", (ret ? *ret : -1), errOK );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call Buffer_SetSize( hi_Buffer _this, tQWORD new_size ) {
	tERROR error;	
  BufferData* d;
  
	PR_TRACE_A0( _this, "Enter Buffer::SetSize method" );

  d = _this->data;
  if(_this->data->preallocated)
		error = errOBJECT_READ_ONLY;
  else if ( d->locks ) {
    error = errOBJECT_IS_LOCKED;
    PR_TRACE( (_this,prtERROR,"Buffer::SetSize() -- Attempt to write to locked(%u) object",d->locks) );
  }
  else if ( ((tQWord*)&new_size)->h ) {
		error = errOUT_OF_OBJECT;
    PR_TRACE( (_this,prtERROR,"Buffer::SetSize(offset=%I64u) -- QWORD operation doesn't supported", new_size) );
	}
  else if ( PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->ptr,d->ptr,((tQWord*)&new_size)->l)) )
    ;
  else
    d->size = ((tQWord*)&new_size)->l;

	PR_TRACE_A1( _this, "Leave Buffer::SetSize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//  Flush internal buffers.
// Behaviour comment
//  Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call Buffer_Flush( hi_Buffer _this ) {
  PR_TRACE_A0( _this, "Enter Buffer::Flush method" );
  PR_TRACE_A0( _this, "Leave Buffer::Flush method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Lock )
// Extended method comment
//  Lock memory and get ptr to access to it. Object memory cannot be overwritten while object is locked
// Parameters are:
tERROR pr_call Buffer_Lock( hi_Buffer _this, tPTR* result ) {
  tERROR error;
  BufferData* d;
  PR_TRACE_A0( _this, "Enter Buffer::Lock method" );

  d = _this->data;
  if ( d->locks ) {
    error = errOBJECT_IS_LOCKED;
    PR_TRACE( (_this,prtERROR,"Buffer::Lock() -- Attempt to lock an object which is already locked(lock_count is %u)]",d->locks) );
  }
  else {
    error = errOK;
    d->locks = 1;
  }

  if ( result )
    *result = PR_SUCC(error) ? d->ptr : 0;

  PR_TRACE_A2( _this, "Leave Buffer::Lock method, ret tPTR = %p, %terr", (PR_SUCC(error) ? d->ptr : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddLock )
// Parameters are:
// tERROR pr_call Buffer_AddLock( hi_Buffer _this, tPTR* result, tDWORD size_to_add, tBOOL save_content ) {
tERROR pr_call Buffer_AddLock( hi_Buffer _this, tPTR* result, tDWORD size_to_add ) {
  tERROR error;  
  BufferData* d;
  tDWORD      s;
  
  PR_TRACE_A0( _this, "Enter Buffer::AddLock method" );

  d = _this->data;
  if( d->preallocated )
		error = errOBJECT_READ_ONLY;
  else if ( d->locks ) {
    error = errOBJECT_IS_LOCKED;
    PR_TRACE( (_this,prtERROR,"Buffer::AddLock() -- Attempt to lock an object which is already locked(lock_count is %u)]",d->locks) );
  }
  else if ( PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->ptr,d->ptr,d->size+size_to_add)) ) {
    PR_TRACE( (_this,prtERROR,"Buffer::AddLock() - cannot resize object") );
  }
  else {
    error = errOK;
    s = d->size;
    d->size += size_to_add;
    d->locks = 1;
  }

  if ( result )
    *result = PR_SUCC(error) ? (d->ptr+s) : 0;

  PR_TRACE_A2( _this, "Leave Buffer::AddLock method, ret tPTR = %p, %terr", (PR_SUCC(error) ? (d->ptr+s) : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SizeLock )
// Parameters are:
tERROR pr_call Buffer_SizeLock( hi_Buffer _this, tPTR* result, tDWORD size ) {
	tERROR error;
	BufferData* d;
	
	PR_TRACE_A0( _this, "Enter Buffer::SizeLock method" );

  d = _this->data;
	if ( d->preallocated )
		error = errOBJECT_READ_ONLY;
  else if ( d->locks ) {
    error = errOBJECT_IS_LOCKED;
    PR_TRACE( (_this,prtERROR,"Buffer::SizeLock() -- Attempt to lock an object which is already locked(lock_count is %u)]",d->locks) );
  }
  else if ( PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->ptr,d->ptr,size)) ) {
    PR_TRACE( (_this,prtERROR,"Buffer::SizeLock() - cannot resize object") );
  }
  else {
    error = errOK;
    d->size = size;
    d->locks = 1;
  }
	
  if ( result )
    *result = PR_SUCC(error) ? d->ptr : 0;
	
	PR_TRACE_A2( _this, "Leave Buffer::SizeLock method, ret tPTR = %p, %terr", (PR_SUCC(error) ? d->ptr : 0), error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Unlock )
// Extended method comment
//  Unlocks memory and returns lock count. If lock count is zero client can write and reallock attached memory
// Parameters are:
tERROR pr_call Buffer_Unlock( hi_Buffer _this ) {
  PR_TRACE_A0( _this, "Enter Buffer::Unlock method" );

  _this->data->locks = 0;

  PR_TRACE_A0( _this, "Leave Buffer::Unlock method, ret errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin definitions,  )
#include <stdarg.h>
PR_MAKE_TRACE_FUNC;

/*PR_EXPORT*/ tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
	switch( dwReason ) {
		
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;
			
		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			// register my interfaces
			if ( PR_FAIL(*pERROR==Buffer_Register((hROOT)hInstance)) )
				return cFALSE;
			
			// register my custom property ids
			// register my exports
			// resolve  my imports
			// use some system resources
			break;
			
		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end



