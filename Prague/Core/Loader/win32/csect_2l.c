// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  06 March 2003,  12:50 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- csect_2l.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// AVP Prague stamp end


#if defined (_WIN32)
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define CriticalSection_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_root.h>

#include "shareexclusiveresource.h"
// AVP Prague stamp end

#include <Prague/plugin/p_win32loader.h>

extern tBOOL g_2l_cs;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// CriticalSection interface implementation
// Short comments -- Critical section behaviour
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface CriticalSection. Inner data structure

typedef struct tag_CSectData {
  RWManager mgr; // control structure
} CSectData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_CriticalSection {
  const iCriticalSectionVtbl* vtbl; // pointer to the "CriticalSection" virtual table
  const iSYSTEMVTBL*          sys;  // pointer to the "SYSTEM" virtual table
  CSectData*                  data; // pointer to the "CriticalSection" data structure
} *hi_CriticalSection;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
static tERROR pr_call CriticalSection_ObjectInitDone( hi_CriticalSection _this );
static tERROR pr_call CriticalSection_ObjectClose( hi_CriticalSection _this );
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
typedef   tERROR (pr_call *fnpCriticalSection_Enter)    ( hi_CriticalSection _this, tSHARE_LEVEL level ); // -- locks synchronization object;
typedef   tERROR (pr_call *fnpCriticalSection_Leave)    ( hi_CriticalSection _this, tSHARE_LEVEL* prev_level ); // -- unlocks synchronization object;
typedef   tERROR (pr_call *fnpCriticalSection_TryEnter) ( hi_CriticalSection _this, tSHARE_LEVEL level ); // -- try enter critical section, but do not block a calling thread;
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
static tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level );
static tERROR pr_call CriticalSection_Leave( hi_CriticalSection _this, tSHARE_LEVEL* prev_level );
static tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level );
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
// Interface "CriticalSection". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define CriticalSection_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(CriticalSection_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, CriticalSection_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "critical section object with two level access", 46 )
  mSHARED_PROPERTY( pgSHARE_LEVEL_SUPPORT, ((tBOOL)(cTRUE)) )
mPROPERTY_TABLE_END(CriticalSection_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "CriticalSection". Register function
tERROR pr_call CriticalSection_Register_2l( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter CriticalSection::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_CRITICAL_SECTION,                   // interface identifier
    PID_WIN32LOADER_2LEVEL_CSECT,           // plugin identifier
    0x00000000,                             // subtype identifier
    CriticalSection_VERSION,                // interface version
    VID_PETROVITCH,                         // interface developer
    &i_CriticalSection_vtbl,                // internal(kernel called) function table
    (sizeof(i_CriticalSection_vtbl)/sizeof(tPTR)),// internal function table size
    &e_CriticalSection_vtbl,                // external function table
    (sizeof(e_CriticalSection_vtbl)/sizeof(tPTR)),// external function table size
    CriticalSection_PropTable,              // property table
    mPROPERTY_TABLE_SIZE(CriticalSection_PropTable),// property table size
    sizeof(CSectData),                      // memory size
    g_2l_cs ? IFACE_SYSTEM : 0              // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"ldr\tCriticalSection(IID_CRITICAL_SECTION) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave CriticalSection::Register method, %terr", error );
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
static tERROR pr_call CriticalSection_ObjectInitDone( hi_CriticalSection _this ) {
  tERROR error;
  CSectData* d;
  PR_TRACE_A0( _this, "Enter CriticalSection::ObjectInitDone method" );

  d = _this->data;
  if ( RWM_Initialize(&d->mgr,0,0) )
    error = errOK;
  else
    error = errOBJECT_CANNOT_BE_INITIALIZED;

  PR_TRACE_A1( _this, "Leave CriticalSection::ObjectInitDone method, %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
static tERROR pr_call CriticalSection_ObjectClose( hi_CriticalSection _this ) {
  CSectData* d;
  PR_TRACE_A0( _this, "Enter CriticalSection::ObjectClose method" );

  d = _this->data;
  RWM_Delete( &d->mgr );

  PR_TRACE_A0( _this, "Leave CriticalSection::ObjectClose method, errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Enter )
// Parameters are:
static tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
  tERROR error;
  CSectData* d;
  PR_TRACE_A0( _this, "Enter CriticalSection::Enter method" );

#ifdef _DEBUG
  if ( (level != SHARE_LEVEL_READ) && (level != SHARE_LEVEL_WRITE) ) {
    PR_TRACE(( _this, prtERROR, "ldr\tCritical section enter - invalid argument" ));
    error= errPARAMETER_INVALID;
  }
  else {
#endif

  d = _this->data;
  if ( RWM_Wait(&d->mgr,level) )
    error = errOK;
  else
    error = errUNEXPECTED;

#ifdef _DEBUG
  }
#endif

  PR_TRACE_A1( _this, "Leave CriticalSection::Enter method, %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Leave )
// Parameters are:
static tERROR pr_call CriticalSection_Leave( hi_CriticalSection _this, tSHARE_LEVEL* prev_level ) {
  tERROR error;
  PR_TRACE_A0( _this, "Enter CriticalSection::Leave method" );

  if ( RWM_Release(&_this->data->mgr,prev_level) )
    error = errOK;
  else
    error = errUNEXPECTED;

  PR_TRACE_A1( _this, "Leave CriticalSection::Leave method, %terr", error );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TryEnter )
// Parameters are:
static tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter CriticalSection::TryEnter method" );

  // !!!!!!!!!!!!!!! wait for Sobko to implement this feature

  PR_TRACE_A1( _this, "Leave CriticalSection::TryEnter method, %terr", error );
  return error;
}
// AVP Prague stamp end


#endif //_WIN32
