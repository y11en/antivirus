// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  23 October 2001,  16:20 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- csection.c
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// AVP Prague stamp end

#if defined (__unix__)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define CriticalSection_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <pthread.h>
#include <errno.h>
#include <Prague/prague.h>
#include "csection.h"
// AVP Prague stamp end

#if defined (__linux__)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

extern tBOOL g_2l_cs;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// CriticalSection interface implementation
// Short comments -- Critical section behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// AVP Prague stamp end

typedef struct tag_CSectData {
  pthread_mutex_t mutex;          // critical section Windows object
  tDWORD          last_error;  // last win error
} CSectData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_CriticalSection {
  const iCriticalSectionVtbl* vtbl; // pointer to the "CriticalSection" virtual table
  const iSYSTEMVTBL*          sys;  // pointer to the "SYSTEM" virtual table
  CSectData*                  data; // pointer to the "CriticalSection" data structure
} *hi_CriticalSection;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call CriticalSection_ObjectInitDone( hi_CriticalSection _this );
tERROR pr_call CriticalSection_ObjectClose( hi_CriticalSection _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_CriticalSection_vtbl = {
  (tIntFnRecognize)        NULL,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       NULL,
  (tIntFnObjectInitDone)   CriticalSection_ObjectInitDone,
  (tIntFnObjectCheck)      NULL,
  (tIntFnObjectPreClose)   NULL,
  (tIntFnObjectClose)      CriticalSection_ObjectClose,
  (tIntFnChildNew)         NULL,
  (tIntFnChildInitDone)    NULL,
  (tIntFnChildClose)       NULL,
  (tIntFnMsgReceive)       NULL,
  (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpCriticalSection_Enter)    ( hi_CriticalSection _this, tSHARE_LEVEL level );   // -- locks synchronization object;
typedef   tERROR (pr_call *fnpCriticalSection_Leave)    ( hi_CriticalSection _this, tSHARE_LEVEL* plevel ); // -- unlocks synchronization object;
typedef   tERROR (pr_call *fnpCriticalSection_TryEnter) ( hi_CriticalSection _this, tSHARE_LEVEL level );   // -- try enter critical section, but do not block a calling thread;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iCriticalSectionVtbl {
  fnpCriticalSection_Enter     Enter;
  fnpCriticalSection_Leave     Leave;
  fnpCriticalSection_TryEnter  TryEnter;
}; // "CriticalSection" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level );
tERROR pr_call CriticalSection_Leave( hi_CriticalSection _this, tSHARE_LEVEL* plevel );
tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iCriticalSectionVtbl e_CriticalSection_vtbl = {
  CriticalSection_Enter,
  CriticalSection_Leave,
  CriticalSection_TryEnter
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "CriticalSection". Global(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "CriticalSection". Property table
mPROPERTY_TABLE(CriticalSection_PropTable)
     mSHARED_PROPERTY( pgSHARE_LEVEL_SUPPORT, cFALSE )
     mLOCAL_PROPERTY_BUF( pgLAST_OS_ERROR, CSectData, last_error, cPROP_BUFFER_READ )
     mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
     mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "critical section object", 24 )
     mPROPERTY_TABLE_END(CriticalSection_PropTable)
     // AVP Prague stamp end



     // --------------------------------------------------------------------------------
     // AVP Prague stamp begin( Registration call,  )
     // Interface "CriticalSection". Register function
     tERROR pr_call CriticalSection_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter \"CriticalSection::Register\" method" );

  error = CALL_Root_RegisterIFace(
                                  root,                                   // root object
                                  IID_CRITICAL_SECTION,                   // interface identifier
                                  PID_WIN32_PLUGIN_LOADER,                // plugin identifier
                                  0x00000000,                             // subtype identifier
                                  0x00000001,                             // interface version
                                  VID_PETROVITCH,                         // interface developer
                                  &i_CriticalSection_vtbl,                // internal(kernel called) function table
                                  (sizeof(i_CriticalSection_vtbl)/sizeof(tPTR)),// internal function table size
                                  &e_CriticalSection_vtbl,                // external function table
                                  (sizeof(e_CriticalSection_vtbl)/sizeof(tPTR)),// external function table size
                                  CriticalSection_PropTable,              // property table
                                  mPROPERTY_TABLE_SIZE(CriticalSection_PropTable),// property table size
                                  sizeof(CSectData),                      // memory size
                                  g_2l_cs ? 0 : IFACE_SYSTEM              // interface flags
                                  );

#ifdef _PRAGUE_TRACE_
  if ( PR_FAIL(error) )
    PR_TRACE( (root,prtDANGER,"CriticalSection(IID_CRITICAL_SECTION) registered [error=0x%08x]",error) );
#endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave \"CriticalSection::Register\" method, ret tERROR = 0x%08x", error );
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
tERROR pr_call CriticalSection_ObjectInitDone( hi_CriticalSection _this ) {

  CSectData* d;
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter \"CriticalSection::ObjectInitDone\" method" );

  d = _this->data;
  pthread_mutexattr_t anAttr;
  if ( ( d->last_error = pthread_mutexattr_init ( &anAttr ) ) != 0 ) 
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) && ( d->last_error = pthread_mutexattr_settype ( &anAttr, PTHREAD_MUTEX_RECURSIVE ) ) != 0 ) 
    error = errOBJECT_NOT_INITIALIZED;
	
  if ( PR_SUCC ( error ) && ( d->last_error = pthread_mutex_init ( &d->mutex, &anAttr ) ) != 0 ) 
    error = errOBJECT_NOT_INITIALIZED;
	
  pthread_mutexattr_destroy ( &anAttr );
  PR_TRACE_A1( _this, "Leave \"CriticalSection::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
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
tERROR pr_call CriticalSection_ObjectClose( hi_CriticalSection _this ) {

  tERROR error;
  PR_TRACE_A0( _this, "Enter \"CriticalSection::ObjectClose\" method" );

  CSectData* d = _this->data;
  pthread_mutex_destroy ( &d->mutex );
  error = errOK;

  PR_TRACE_A1( _this, "Leave \"CriticalSection::ObjectClose\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Enter )
// Parameters are:
// tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this ) {
tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
  CSectData* d;
  tERROR error;
  PR_TRACE_A0( _this, "Enter \"CriticalSection::Enter\" method" );

  d = _this->data;  
  PR_TRACE ((_this, prtSPAM, "ldr\t%s %u is locking mutex", __FUNCTION__, pthread_self ()));
  d->last_error = pthread_mutex_lock ( &d->mutex );
  switch( d->last_error ) {
  case 0         : error = errOK;             break;
  case EINVAL    : error = errOBJECT_NOT_INITIALIZED; break;
  case ETIMEDOUT : error = errTIMEOUT;        break;
  default        : error = errUNEXPECTED;     break;
  }
  PR_TRACE ((_this, prtSPAM, "ldr\t%s %u has finished with error %terr", __FUNCTION__, pthread_self (), error));
  PR_TRACE_A1( _this, "Leave \"CriticalSection::Enter\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Leave )
// Parameters are:
tERROR pr_call CriticalSection_Leave( hi_CriticalSection _this, tSHARE_LEVEL* plevel ) {
  CSectData* d;
  tERROR error;
  PR_TRACE_A0( _this, "Enter \"CriticalSection::Leave\" method" );

  d = _this->data;
  PR_TRACE ((_this, prtSPAM, "ldr\t%s %u is leaving mutex", __FUNCTION__, pthread_self ()));
  d->last_error = pthread_mutex_unlock ( &d->mutex );
  switch( d->last_error ) {
  case 0         : error = errOK;             break;
  case EINVAL    : error = errOBJECT_NOT_INITIALIZED; break;
  case ETIMEDOUT : error = errTIMEOUT;        break;
  default        : error = errUNEXPECTED;     break;
  }

  if ( plevel )
    *plevel = SHARE_LEVEL_WRITE;
  PR_TRACE ((_this, prtSPAM, "ldr\t%s %u has left with error %terr", __FUNCTION__, pthread_self (), error));
  PR_TRACE_A1( _this, "Leave \"CriticalSection::Leave\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TryEnter )
// Parameters are:
// tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this ) {
tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
  CSectData* d;
  tERROR error;
  PR_TRACE_A0( _this, "Enter \"CriticalSection::TryEnter\" method" );

  d = _this->data;
  PR_TRACE ((_this, prtSPAM, "ldr\t%s %u is trying to lock mutex", __FUNCTION__, pthread_self ()));
  d->last_error = pthread_mutex_trylock ( &d->mutex );
  switch( d->last_error ) {
  case 0         : error = errOK;             break;
  case EINVAL    : error = errOBJECT_NOT_INITIALIZED; break;
  case EBUSY     : error = errNOT_OK;        break;
  default        : error = errUNEXPECTED;     break;
  }
  PR_TRACE ((_this, prtSPAM, "ldr\t%s %u has finished with error %terr", __FUNCTION__, pthread_self (), error));
  PR_TRACE_A1( _this, "Leave \"CriticalSection::TryEnter\" method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end

#endif //__unix__

