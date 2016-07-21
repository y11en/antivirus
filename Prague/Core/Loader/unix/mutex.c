// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  03 June 2004,  18:15 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- mutex.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end

#if defined (__unix__)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Mutex_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "mutex.h"
// AVP Prague stamp end



#include "loader.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Mutex interface implementation
// Short comments -- Mutex behaviour
// AVP Prague stamp end

#if defined (__linux__)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Mutex. Inner data structure

typedef struct tag_MutexData {
  pthread_mutex_t mutex;        // handle of the Windows object
  char*  name;          // name of the object
  tBOOL  open;          // do not try to create
  tBOOL  initial_owner; // initially owned
  tDWORD last_error;    // last win error
  tBOOL  global;        // --
} MutexData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Mutex {
  const iMutexVtbl*  vtbl; // pointer to the "Mutex" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  MutexData*         data; // pointer to the "Mutex" data structure
} *hi_Mutex;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Mutex_ObjectInitDone( hi_Mutex _this );
tERROR pr_call Mutex_ObjectClose( hi_Mutex _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Mutex_vtbl = {
  (tIntFnRecognize)         NULL,
  (tIntFnObjectNew)         NULL,
  (tIntFnObjectInit)        NULL,
  (tIntFnObjectInitDone)    Mutex_ObjectInitDone,
  (tIntFnObjectCheck)       NULL,
  (tIntFnObjectPreClose)    NULL,
  (tIntFnObjectClose)       Mutex_ObjectClose,
  (tIntFnChildNew)          NULL,
  (tIntFnChildInitDone)     NULL,
  (tIntFnChildClose)        NULL,
  (tIntFnMsgReceive)        NULL,
  (tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpMutex_Lock)    ( hi_Mutex _this, tDWORD timeout );          // -- locks synchronization object;
typedef   tERROR (pr_call *fnpMutex_Release) ( hi_Mutex _this );                          // -- unlocks synchronization object;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iMutexVtbl {
  fnpMutex_Lock     Lock;
  fnpMutex_Release  Release;
}; // "Mutex" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Mutex_Lock( hi_Mutex _this, tDWORD p_timeout );
tERROR pr_call Mutex_Release( hi_Mutex _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iMutexVtbl e_Mutex_vtbl = {
  Mutex_Lock,
  Mutex_Release
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Mutex_PROP_get_name( hi_Mutex _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Mutex_PROP_name( hi_Mutex _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Mutex". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Mutex_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Mutex_PropTable)
     mSHARED_PROPERTY( pgINTERFACE_VERSION, Mutex_VERSION )
     mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "mutex object", 13 )
     mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, Mutex_PROP_get_name, Mutex_PROP_name )
     mLOCAL_PROPERTY_BUF( pgJUST_OPEN, MutexData, open, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
     mLOCAL_PROPERTY_BUF( pgINITIAL_OWNER, MutexData, initial_owner, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
     mLOCAL_PROPERTY_BUF( pgLAST_OS_ERROR, MutexData, last_error, cPROP_BUFFER_READ )
     mLOCAL_PROPERTY_BUF( pgGLOBAL, MutexData, global, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
     mPROPERTY_TABLE_END(Mutex_PropTable)
     // AVP Prague stamp end



     // --------------------------------------------------------------------------------
     // AVP Prague stamp begin( Registration call,  )
     // Interface "Mutex". Register function
     tERROR pr_call Mutex_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter \"Mutex::Register\" method" );

  error = CALL_Root_RegisterIFace(
                                  root,                                   // root object
                                  IID_MUTEX,                              // interface identifier
                                  PID_WIN32LOADER,                        // plugin identifier
                                  0x00000000,                             // subtype identifier
                                  Mutex_VERSION,                          // interface version
                                  VID_PETROVITCH,                         // interface developer
                                  &i_Mutex_vtbl,                          // internal(kernel called) function table
                                  (sizeof(i_Mutex_vtbl)/sizeof(tPTR)),    // internal function table size
                                  &e_Mutex_vtbl,                          // external function table
                                  (sizeof(e_Mutex_vtbl)/sizeof(tPTR)),    // external function table size
                                  Mutex_PropTable,                        // property table
                                  mPROPERTY_TABLE_SIZE(Mutex_PropTable),  // property table size
                                  sizeof(MutexData),                      // memory size
                                  IFACE_SYSTEM                            // interface flags
                                  );

#ifdef _PRAGUE_TRACE_
  if ( PR_FAIL(error) )
    PR_TRACE( (root,prtDANGER,"Mutex(IID_MUTEX) registered [error=0x%08x]",error) );
#endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave \"Mutex::Register\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_name )
// Interface "Mutex". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR pr_call Mutex_PROP_get_name( hi_Mutex _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  MutexData* d;
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter *GET* method get_name for property pgOBJECT_NAME" );
	
  d = _this->data;
	
  if ( d->name )
    *out_size = strlen( d->name ) + 1;
  else
    *out_size = 1;

  if ( buffer ) {
    if ( size < *out_size )
      error = errBUFFER_TOO_SMALL;
    else if ( d->name )
      memcpy( buffer, d->name, *out_size );
    else
      *(tCHAR*)buffer = 0;
  }
	
  PR_TRACE_A2( _this, "Leave *GET* method get_name for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, name )
// Interface "Mutex". Method "Set" for property(s):
//  -- OBJECT_NAME
tERROR pr_call Mutex_PROP_name( hi_Mutex _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tDWORD len;
  MutexData* d;
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter *SET* method \"Mutex_PROP_name\" for property \"pgOBJECT_NAME\"" );

  len = 0;
  d = _this->data;
  d->last_error = 0;

  if ( d->name )
    free ( d->name );

  if ( buffer && *buffer ) {
    len = strlen(buffer) + 1;
    d->name = calloc( sizeof ( char ), len );
    if ( d->name )
      memcpy( d->name, buffer, len );
    else {
      error = errNOT_ENOUGH_MEMORY;
      d->last_error = ENOMEM;
    }
  }
  *out_size = len;

  PR_TRACE_A2( _this, "Leave *SET* method \"Mutex_PROP_name\" for property \"pgOBJECT_NAME\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Mutex_ObjectInitDone( hi_Mutex _this ) {
  MutexData* d;
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter \"Mutex::ObjectInitDone\" method" );

  d = _this->data;
  d->last_error = 0;

  if ( d->open )
    error = errOBJECT_NOT_INITIALIZED;

  pthread_mutexattr_t anAttr;
  if ( ( d->last_error = pthread_mutexattr_init ( &anAttr ) ) != 0 ) 
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) && ( d->last_error = pthread_mutexattr_settype ( &anAttr, PTHREAD_MUTEX_RECURSIVE ) ) != 0 ) 
    error = errOBJECT_NOT_INITIALIZED;
	
  if ( PR_SUCC ( error ) && ( d->last_error = pthread_mutex_init ( &d->mutex, &anAttr ) ) != 0 ) 
    error = errOBJECT_NOT_INITIALIZED;
	
  pthread_mutexattr_destroy ( &anAttr );
        
  PR_TRACE_A1( _this, "Leave \"Mutex::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
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
tERROR pr_call Mutex_ObjectClose( hi_Mutex _this ) {

  MutexData* d;
  PR_TRACE_A0( _this, "Enter \"Mutex::ObjectClose\" method" );

  d = _this->data;
  d->last_error = 0;
  pthread_mutex_destroy ( &d->mutex );
  if ( d->name ) {
    free ( d->name );
    d->name = 0;
  }
	
  PR_TRACE_A0( _this, "Leave \"Mutex::ObjectClose\" method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Lock )
// Parameters are:
tERROR pr_call Mutex_Lock( hi_Mutex _this, tDWORD timeout ) {
  MutexData* d;
  tERROR error;
  PR_TRACE_A0( _this, "Enter \"Mutex::Lock\" method" );

  d = _this->data;
  d->last_error = pthread_mutex_lock ( &d->mutex );
  switch( d->last_error ) {
  case 0         : error = errOK;             break;
  case EINVAL    : error = errOBJECT_NOT_INITIALIZED; break;
  case ETIMEDOUT : error = errTIMEOUT;        break;
  default        : error = errUNEXPECTED;     break;
  }
  
  PR_TRACE_A1( _this, "Leave \"Mutex::Lock\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Release )
// Parameters are:
tERROR pr_call Mutex_Release( hi_Mutex _this ) {
  MutexData* d;
  tERROR error;
  PR_TRACE_A0( _this, "Enter \"Mutex::Release\" method" );

  d = _this->data;
  d->last_error = pthread_mutex_unlock ( &d->mutex );
  switch( d->last_error ) {
  case 0         : error = errOK;             break;
  case EINVAL    : error = errOBJECT_NOT_INITIALIZED; break;
  default        : error = errUNEXPECTED;     break;
  }
	
  PR_TRACE_A1( _this, "Leave \"Mutex::Release\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end

#endif //__unix__

