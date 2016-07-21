// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  25 September 2002,  12:52 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- plugin for NTFS stream operation
// Author      -- Doukhvalow
// File Name   -- stream_optr.c
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include "stream_optr.h"
#include "stream_util.h"
#include "nfio/win32_io.h"
// AVP Prague stamp end



#define STACK_STR_LEN 100
#define PFSI(fsi)            ((FILE_STREAM_INFORMATION*)(fsi))
#define NEXT(fsi)            PFSI( MB(fsi) + (fsi)->NextEntryOffset )
#define CONST_SIZE           ((tUINT)(&PFSI(0)->StreamName[0]))
#define ALIGN_TO_8(a)        ((((a)/8) + !!((a)%8))*8)
#define NAME_REQUESTED_SIZE  (sizeof(FILE_NAME_INFORMATION)+MAX_PATH)
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_OPtrData {
  hIO                            base_io;        // IO object based on
  HANDLE                         base_handle;    // windows handle based on
  tWSTRING                       base_name;      // --
  tDWORD                         base_owned;     // i'm an owner of the windows handle
  const FILE_STREAM_INFORMATION* list;           // list of the streams
	tDWORD                         list_size;      // size of the allocated list
	tDWORD                         count;          // stream count
  const FILE_STREAM_INFORMATION* curr;           // current stream
	tBOOL                          create_empty;   // if set InitDone will succeed even on empty list
  tWCHAR                         mask[MAX_PATH]; // mask for enumeration
  tORIG_ID                       origin;         // object origin
  tObjPtrState                   state;          // state of the pointer
  tDWORD                         win32err;       // native windows error
} OPtrData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr {
  const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  OPtrData*          data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



tERROR pr_call _reinit_stream_list( hi_ObjPtr _this, HANDLE handle, IO_STATUS_BLOCK* pio );
tERROR pr_call _reinit_stream_list_and_restore_state( hi_ObjPtr _this, tWSTRING* name, tUINT size );
tVOID  pr_call _compact_streams( hOBJECT obj, FILE_STREAM_INFORMATION* beg, FILE_STREAM_INFORMATION** res, tDWORD* size, tDWORD* count );
tDWORD pr_call _calc_streams( FILE_STREAM_INFORMATION* beg );
tERROR pr_call _has_streams( hIO obj );
tERROR pr_call _get_full_name( hi_ObjPtr _this, tWSTRING* name, tUINT* size );
tERROR pr_call _combain_name( hi_ObjPtr _this, hOBJECT obj, tPTR* buffer, tDWORD* len, tWSTRING* stream );
tERROR pr_call _extract_name( hi_ObjPtr _this, hOBJECT obj, tPTR* buffer, tDWORD len );
tERROR pr_call _io_copy( hIO dst, hIO scr, tPTR buffer, tDWORD bsize );
tERROR pr_call _get_name( hOBJECT obj, tIID iid, tDWORD* out_len, tPTR buff, tDWORD len );
tERROR pr_call _ren_move( tPTR obj, tPTR existing, tPTR new_name, tBOOL overwrite, tDWORD* native_error );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_Recognize( hOBJECT _that );
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = {
  (tIntFnRecognize)        ObjPtr_Recognize,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       ObjPtr_ObjectInit,
  (tIntFnObjectInitDone)   ObjPtr_ObjectInitDone,
  (tIntFnObjectCheck)      NULL,
  (tIntFnObjectPreClose)   NULL,
  (tIntFnObjectClose)      ObjPtr_ObjectClose,
  (tIntFnChildNew)         NULL,
  (tIntFnChildInitDone)    NULL,
  (tIntFnChildClose)       NULL,
  (tIntFnMsgReceive)       NULL,
  (tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
// ----------- operations with the current object ----------
typedef   tERROR (pr_call *fnpObjPtr_IOCreate) ( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ); // -- creates object current in enumeration;
typedef   tERROR (pr_call *fnpObjPtr_Copy)     ( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite ); // -- copies the object pointed by the ptr;
typedef   tERROR (pr_call *fnpObjPtr_Rename)   ( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite ); // -- renames the object pointed by the ptr;
typedef   tERROR (pr_call *fnpObjPtr_Delete)   ( hi_ObjPtr _this );                         // -- deletes object pointed by ptr;

// ----------- navigation methods ----------
typedef   tERROR (pr_call *fnpObjPtr_Reset)    ( hi_ObjPtr _this, tBOOL to_root );          // -- Reinitializing of ObjEnum object;
typedef   tERROR (pr_call *fnpObjPtr_Clone)    ( hi_ObjPtr _this, hObjPtr* result );                         // -- Makes another ObjPtr object;
typedef   tERROR (pr_call *fnpObjPtr_Next)     ( hi_ObjPtr _this );                         // -- Returns next IO object;
typedef   tERROR (pr_call *fnpObjPtr_StepUp)   ( hi_ObjPtr _this );                         // -- Goes one level up;
typedef   tERROR (pr_call *fnpObjPtr_StepDown) ( hi_ObjPtr _this );                         // -- Goes one level up;
typedef   tERROR (pr_call *fnpObjPtr_ChangeTo) ( hi_ObjPtr _this, hOBJECT name );           // -- Changes folder for enumerating;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iObjPtrVtbl {

// ----------- operations with the current object ----------
  fnpObjPtr_IOCreate  IOCreate;
  fnpObjPtr_Copy      Copy;
  fnpObjPtr_Rename    Rename;
  fnpObjPtr_Delete    Delete;

// ----------- navigation methods ----------
  fnpObjPtr_Reset     Reset;
  fnpObjPtr_Clone     Clone;
  fnpObjPtr_Next      Next;
  fnpObjPtr_StepUp    StepUp;
  fnpObjPtr_StepDown  StepDown;
  fnpObjPtr_ChangeTo  ChangeTo;
}; // "ObjPtr" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// ----------  operations with the current object  ----------
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this );

// ----------  navigation methods  ----------
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root );
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result );
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iObjPtrVtbl e_ObjPtr_vtbl = {
  ObjPtr_IOCreate,
  ObjPtr_Copy,
  ObjPtr_Rename,
  ObjPtr_Delete,
  ObjPtr_Reset,
  ObjPtr_Clone,
  ObjPtr_Next,
  ObjPtr_StepUp,
  ObjPtr_StepDown,
  ObjPtr_ChangeTo
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call ObjPtr_PROP_get_name( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_set_io( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_set_mask( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_size_q( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_attributes( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_base_name( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_set_base_name( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, L"NTFS stream enumerator", 46 )
  mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, OPtrData, origin, cPROP_BUFFER_READ_WRITE )
  mLOCAL_PROPERTY_FN( pgOBJECT_NAME, ObjPtr_PROP_get_name, NULL )
	mSHARED_PROPERTY_PTR( pgOBJECT_PATH, L"", 2 )
  mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_get_name, NULL )
  mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY( pgOBJECT_BASED_ON, OPtrData, base_io, cPROP_BUFFER_READ | cPROP_WRITABLE_ON_INIT, NULL, ObjPtr_PROP_set_io )
  mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
  mLOCAL_PROPERTY( pgMASK, OPtrData, mask, cPROP_BUFFER_READ, NULL, ObjPtr_PROP_set_mask )
  mLOCAL_PROPERTY_BUF( pgOBJECT_PTR_STATE, OPtrData, state, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_get_size, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_get_size_q, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, ObjPtr_PROP_get_attributes, NULL )
  mLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( plBASE_FULL_NAME, ObjPtr_PROP_get_base_name, ObjPtr_PROP_set_base_name )
	mLOCAL_PROPERTY_BUF( plNATIVE_ERR, OPtrData, win32err, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plBASE_HANDLE, OPtrData, base_handle, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plCREATE_EMPTY, OPtrData, create_empty, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plSTREAM_COUNT, OPtrData, count, cPROP_BUFFER_READ )
  mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_UNICODE)) )
mPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "ObjPtr". Register function
tERROR pr_call ObjPtr_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter ObjPtr::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_OBJPTR,                             // interface identifier
    PID_NTFSSTREAM,                         // plugin identifier
    0x00000000,                             // subtype identifier
    ObjPtr_VERSION,                         // interface version
    VID_PETROVITCH,                         // interface developer
    &i_ObjPtr_vtbl,                         // internal(kernel called) function table
    (sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
    &e_ObjPtr_vtbl,                         // external function table
    (sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
    ObjPtr_PropTable,                       // property table
    mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
    sizeof(OPtrData),                       // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [error=0x%08x]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave ObjPtr::Register method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_name )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
tERROR pr_call ObjPtr_PROP_get_name( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error = errOK;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter *GET* multyproperty method ObjPtr_PROP_get_name" );

  d = _this->data;
  d->win32err = 0;
  if ( (d->state != cObjPtrState_PTR) || !d->curr ) {
    error = errOBJECT_BAD_INTERNAL_STATE;
    *out_size = 0;
  }
  else {
    *out_size = d->curr->StreamNameLength + WS(1);
    if ( !buffer )
      error = errOK;
    else if ( size < *out_size )
      error = errBUFFER_TOO_SMALL;
    else {
      memcpy( buffer, d->curr->StreamName, *out_size );
      error = errOK;
    }
  }

  PR_TRACE_A2( _this, "Leave *GET* multyproperty method ObjPtr_PROP_get_name, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_io )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_BASED_ON
tERROR pr_call ObjPtr_PROP_set_io( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
  OPtrData* d;
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_io for property pgOBJECT_BASED_ON" );

  d = _this->data;
  d->win32err = 0;
  if ( d->base_name ) {
    CALL_SYS_ObjHeapFree( _this, d->base_name );
    d->base_name = 0;
  }
  d->base_handle = 0;
  if (
    PR_FAIL(CALL_SYS_ObjectCheck(_this,*(hOBJECT*)buffer,IID_IO,PID_NATIVE_FIO,SUBTYPE_ANY,cFALSE)) ||
    CALL_SYS_PropertyIsPresent(*(hOBJECT*)buffer,g_pid_this_is_the_stream)
  ) {
    d->base_io = 0;
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cTRUE );
    *out_size = 0;
    error = errPROPERTY_VALUE_INVALID;
  }
  else {
    d->base_io = *(hIO*)buffer;
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cFALSE );
    *out_size = sizeof(hIO);
    error = errOK;
  }

	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_io for property pgOBJECT_BASED_ON, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_mask )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- MASK
tERROR pr_call ObjPtr_PROP_set_mask( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error = errPROPERTY_NOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_mask for property pgMASK" );
  *out_size = 0;
  PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_mask for property pgMASK, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_size )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_get_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error = errOK;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_size for property pgOBJECT_SIZE" );

  d = _this->data;
  d->win32err = 0;
  do {
    if ( (d->state != cObjPtrState_PTR) || !d->curr ) {
      error = errOBJECT_BAD_INTERNAL_STATE;
      *out_size = 0;
      break;
    }
    if ( d->curr->StreamSize.HighPart ) {
      error = errBUFFER_TOO_SMALL;
      *out_size = 0;
      break;
    }
    if ( buffer )
      *(tDWORD*)buffer = d->curr->StreamSize.LowPart;
    *out_size = sizeof(d->curr->StreamSize.LowPart);
    error = errOK;
  } while( 0 );

  PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_size for property pgOBJECT_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_size_q )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_get_size_q( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_size_q for property pgOBJECT_SIZE_Q" );

  d = _this->data;
  d->win32err = 0;
  do {
    if ( (d->state != cObjPtrState_PTR) || !d->curr ) {
      error = errOBJECT_BAD_INTERNAL_STATE;
      *out_size = 0;
      break;
    }
    error = errOK;
    if ( buffer )
      *(tQWORD*)buffer = *(tQWORD*)&d->curr->StreamSize;
    *out_size = sizeof(d->curr->StreamSize);
  } while( 0 );

  PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_size_q for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_attributes )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_get_attributes( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error;
  OPtrData* d;
  tWCHAR    nbuff[STACK_STR_LEN];
  tWSTRING  name;
  tDWORD    attr;
  tUINT     name_size;
  PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_attributes for property pgOBJECT_ATTRIBUTES" );

  name = nbuff;
  name_size = sizeof(nbuff);
  d = _this->data;
  d->win32err = 0;
  *out_size = 0;
  if ( d->state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  else if ( !buffer )
    *out_size = sizeof(tDWORD);
  else if ( PR_FAIL(error=_get_full_name(_this,&name,&name_size)) )
    ;
  else if ( 0xffffffffl == (attr=GetFileAttributesW(name)) ) {
    d->win32err = GetLastError();
    error = _ConvertWin32Err( d->win32err );
  }
  else {
    *(tDWORD*)buffer = attr;
    *out_size = sizeof(tDWORD);
  }

  if ( name != nbuff )
    CALL_SYS_ObjHeapFree( _this, name );

  PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_attributes for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_base_name )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- BASE_FULL_NAME
tERROR pr_call ObjPtr_PROP_get_base_name( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error = errOK;
  OPtrData* d;
  tDWORD len;
  PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_base_name for property plBASE_FULL_NAME" );

  d = _this->data;
  d->win32err = 0;
  if ( !d->base_name )
    len = 0;
  else
    *out_size = (tDWORD)WS( 1 + wcslen(d->base_name) );

  if ( !buffer )
    *out_size = len;
  else if ( size < len ) {
    *out_size = 0;
    error = errBUFFER_TOO_SMALL;
  }
  else
    memcpy( buffer, d->base_name, *out_size=len );

  PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_base_name for property plBASE_FULL_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_base_name )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- BASE_FULL_NAME
tERROR pr_call ObjPtr_PROP_set_base_name( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_base_name for property plBASE_FULL_NAME" );

  *out_size = 0;
  d = _this->data;
  d->win32err = 0;
  if ( PR_FAIL(error=_not_name_of_the_stream((tWSTRING)buffer)) )
    ;
  else if ( d->base_name ) {
    error = CALL_SYS_ObjHeapRealloc( _this, &d->base_name, d->base_name, size + sizeof(tWCHAR));
    if ( PR_SUCC(error) )
      memcpy( d->base_name, buffer, *out_size=size );
  }
  else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&d->base_name,size + sizeof(tWCHAR))) )
    ;
  else
    memcpy( d->base_name, buffer, *out_size=size );

  if ( PR_FAIL(error) )
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cFALSE );

  d->base_handle = 0;
  d->base_io = 0;

  PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_base_name for property plBASE_FULL_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, Recognize )
// Extended method comment
//    Static method. Kernel calls this method wihtout any created objects of this interface.
//    Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//    Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//    It must be quick (preliminary) analysis
// Parameters are:
tERROR pr_call ObjPtr_Recognize( hOBJECT _that ) {
  tERROR error;
  PR_TRACE_A2( 0, "Enter ObjPtr::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
  if ( PR_FAIL(error=CALL_SYS_ObjectCheck(_that,_that,IID_IO,PID_NATIVE_FIO,SUBTYPE_ANY,cFALSE)) )
    ;
  else if ( PR_SUCC(CALL_SYS_PropertyIsPresent(_that,g_pid_this_is_the_stream)) )
    error = errOBJECT_INCOMPATIBLE;
  else
    error = _has_streams( (hIO)_that );
  PR_TRACE_A2( 0, "Leave ObjPtr::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this ) {
  hOBJECT dad;
  PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInit method" );

  if ( PR_FAIL(CALL_SYS_ObjectCheck(_this,dad=CALL_SYS_ParentGet(_this,IID_ANY),IID_IO,PID_NATIVE_FIO,SUBTYPE_ANY,cFALSE)) )
    ;
  else if ( PR_SUCC(CALL_SYS_PropertyIsPresent(dad,g_pid_this_is_the_stream)) )
    ;
  else {
    _this->data->base_io = (hIO)dad;
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cTRUE );
  }

  PR_TRACE_A0( _this, "Leave ObjPtr::ObjectInit method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this ) {
  tERROR          error;
  OPtrData*       d;
  HANDLE          handle = 0;
  IO_STATUS_BLOCK iosb;
  tDWORD          fsi_size;
  PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInitDone method" );

  fsi_size = 0;
  d = _this->data;
  d->win32err = 0;

  if ( d->base_io ) {
    tDWORD size;
    if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(d->base_io,&size,pgOBJECT_FULL_NAME,0,0,cCP_UNICODE)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&d->base_name,size)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(d->base_io,0,pgOBJECT_FULL_NAME,d->base_name,size,cCP_UNICODE)) )
      ;
    else if ( PR_SUCC(CALL_SYS_PropertyGet(d->base_io,&fsi_size,g_pid_streams,0,0)) ) {
      if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&PFSI(d->list),fsi_size)) )
        ;
      else if ( PR_SUCC(error=CALL_SYS_PropertyGet(d->base_io,&d->list_size,g_pid_streams,PFSI(d->list),fsi_size)) ) {
        d->count = _calc_streams( PFSI(d->list) );
        CALL_SYS_PropertyDelete( d->base_io, g_pid_streams );
      }
    }
    else
      error = CALL_SYS_PropertyGet( d->base_io, 0, plNATIVE_HANDLE, &handle, sizeof(handle) );
  }
  else if ( !d->base_name || !*d->base_name )
    error = errOBJECT_NOT_INITIALIZED;
  else
    error = errOK;

  if ( PR_SUCC(error) && !d->list_size )
    error = _reinit_stream_list( _this, handle, &iosb );
  if ( PR_SUCC(error) && !d->list && !d->create_empty )
    error = errEND_OF_THE_LIST;

  PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInitDone method, ret tERROR = 0x%08x", error );
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
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this ) {
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter ObjPtr::ObjectClose method" );
  d = _this->data;
  if ( d->list )
    CALL_SYS_ObjHeapFree( _this, (tPTR)d->list );

  if ( d->base_name ) {
	  CALL_SYS_ObjHeapFree( _this, d->base_name );
	  d->base_name = 0;
  }

  PR_TRACE_A0( _this, "Leave ObjPtr::ObjectClose method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//    Creates hIO or hObjEnum object current in enumeration.
// Result comment
//    Returns hIO or hObjEnum object. Depends on current object.
// Parameters are:
//   "access_mode" : New object open mode
//   "open_mode"   : Attributes of the new object
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ) {
  tERROR error;
  hIO    ret_val = NULL;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter ObjPtr::IOCreate method" );

  d = _this->data;
  d->win32err = 0;

  if ( !name && (d->state != cObjPtrState_PTR) )
    error = errOBJECT_BAD_INTERNAL_STATE;

  else {
    tWCHAR  obj_name[STACK_STR_LEN];
    tWCHAR* pobj_name = obj_name;
    tUINT   sobj_name = sizeof(obj_name);

    if ( name ) {
      tDWORD nlen = 0;
      tUINT  blen = (tUINT)WS( wcslen(d->base_name) );
      if ( PR_FAIL(error=CALL_String_LengthEx(name,&nlen,cSTRING_WHOLE,cCP_UNICODE,cSTRING_Z)) )
        ;
      else if ( (((blen+WS(1)) + nlen) > sizeof(obj_name)) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&pobj_name,blen+nlen+WS(1))) )
        ;
      else {
        memcpy( pobj_name, d->base_name, blen );
        error = CALL_String_ExportToBuff( name, 0, cSTRING_WHOLE, MB(pobj_name)+blen, nlen, cCP_UNICODE, cSTRING_Z );
        if ( PR_SUCC(error) && (*UNI(MB(pobj_name)+blen) != L':') ) {
          memmove( MB(pobj_name)+blen+WS(1), MB(pobj_name)+blen, nlen );
          *UNI(MB(pobj_name)+blen) = L':';
        }
      }
    }
    else
      error = _get_full_name( _this, &pobj_name, &sobj_name );

    if ( PR_FAIL(error) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_ObjectCreate(_this,(hOBJECT*)&ret_val,IID_IO,PID_WIN32_NFIO,SUBTYPE_ANY)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_PropertySetStr(ret_val,0,pgOBJECT_NAME,pobj_name,0,cCP_UNICODE)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_OPEN_MODE,open_mode)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_ACCESS_MODE,access_mode)) )
      ;
    else if ( PR_FAIL(error=CALL_SYS_ObjectCreateDone(ret_val)) )
      ;
    else
      CALL_SYS_PropertySetBool( ret_val, g_pid_this_is_the_stream, cTRUE );

    if ( PR_FAIL(error) && ret_val ) {
      CALL_SYS_ObjectClose( ret_val );
      ret_val = 0;
    }

    if ( pobj_name != obj_name )
      CALL_SYS_ObjHeapFree( _this, pobj_name );
  }
  
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave ObjPtr::IOCreate method, ret hIO = %S, error = 0x%08x", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite ) {
  OPtrData* d;
  tERROR    error;
  PR_TRACE_A0( _this, "Enter ObjPtr::Copy method" );
  
  d = _this->data;
  d->win32err = 0;
  
  if ( d->state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  
  else {
    hIO  src_io = 0;
    hIO  dst_io = 0;
    while( 1 ) {
      tWCHAR  name[STACK_STR_LEN];
      tWCHAR* pname = name;
      tDWORD  sname = sizeof(name);
      tDWORD  omode = overwrite ? fOMODE_CREATE_ALWAYS : fOMODE_CREATE_IF_NOT_EXIST;
      tIID    dst_iid = CALL_SYS_PropertyGetDWord( dst_name, pgINTERFACE_ID );
      switch( dst_iid ) {
        case  IID_IO :
          dst_io = (hIO)dst_name;
          error = CALL_IO_SetSize( dst_io, 0 );
          break;
        case IID_OBJPTR:
          error = CALL_ObjPtr_IOCreate( (hi_ObjPtr)dst_name, &dst_io, 0, fACCESS_WRITE, omode );
          break;
        case IID_STRING:
          if ( PR_FAIL(error=_combain_name(_this,dst_name,&pname,&sname,0)) )
            break;
          if ( PR_FAIL(error=CALL_SYS_ObjectCreate(_this,&dst_io,IID_IO,PID_NATIVE_FIO,SUBTYPE_ANY)) )
            break;
          if ( PR_FAIL(error=CALL_SYS_PropertySetStr(dst_io,0,pgOBJECT_NAME,pname,0,cCP_UNICODE)) )
            break;
          if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(dst_io,pgOBJECT_ACCESS_MODE,fACCESS_WRITE)) )
            break;
          if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(dst_io,pgOBJECT_OPEN_MODE,omode)) )
            break;
          error = CALL_SYS_ObjectCreateDone( dst_io );
          break;
        default:
          error = errINTERFACE_INCOMPATIBLE;
      }
      if ( PR_FAIL(error) )
        break;
      if ( PR_FAIL(error=ObjPtr_IOCreate(_this,&src_io,0,fACCESS_READ,fOMODE_OPEN_IF_EXIST)) )
        break;
      if ( PR_FAIL(error=_io_copy(dst_io,src_io,0,0)) )
        break;
      switch( dst_iid ) {
        case IID_OBJPTR:
        case IID_STRING:
          error = _reinit_stream_list_and_restore_state( _this, &pname, sname );
          break;
      }
      break;
    }
    if ( src_io )
      CALL_SYS_ObjectClose( src_io );
    if ( dst_io && (dst_io != (hIO)dst_name) )
      CALL_SYS_ObjectClose( dst_io );
  }

  PR_TRACE_A1( _this, "Leave ObjPtr::Copy method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite ) {
  OPtrData* d;
  tERROR    error;
  tIID      new_iid = 0;
  tWCHAR    src[STACK_STR_LEN];
  tWCHAR    dst[STACK_STR_LEN];
  tWCHAR*   psrc = src;
  tUINT     ssrc = sizeof(src);
  tWCHAR*   pdst = dst;
  tDWORD    sdst = sizeof(dst);
  tWSTRING  nname;
  PR_TRACE_A0( _this, "Enter ObjPtr::Rename method" );

	d = _this->data;
	d->win32err = 0;

	if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;
  else if( PR_SUCC(error=ObjPtr_Copy(_this,new_name,overwrite)) ) {
    if ( PR_FAIL(ObjPtr_Delete(_this)) )
      error = errOBJECT_NOT_DELETED;
  }
  else if ( PR_FAIL(error=_get_full_name(_this,&psrc,&ssrc)) )
    ;
  else if ( PR_FAIL(error=_combain_name(_this,new_name,&pdst,&sdst,&nname)) )
		;
  else if ( PR_SUCC(error=_ren_move(_this,psrc,pdst,overwrite,&d->win32err)) ) 
    ;

  if ( PR_SUCC(error) )
    error = _reinit_stream_list_and_restore_state( _this, &pdst, sdst );
		
	if ( pdst != dst )
		CALL_SYS_ObjHeapFree( _this, pdst );
	
	if ( psrc != src )
		CALL_SYS_ObjHeapFree( _this, psrc );

  PR_TRACE_A1( _this, "Leave ObjPtr::Rename method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    pointer must be advanced to the next object
// Parameters are:
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this ) {
  tERROR   error;
  tWCHAR   name[STACK_STR_LEN];
  tWSTRING pname = name;
  tUINT    sname = sizeof(name);
  PR_TRACE_A0( _this, "Enter ObjPtr::Delete method" );

  if ( PR_FAIL(error=_get_full_name(_this,&pname,&sname)) )
    ;
  else if ( !DeleteFileW(name) )
    error = _ConvertWin32Err( _this->data->win32err = GetLastError() );
  else if ( PR_FAIL(error=ObjPtr_Next(_this)) && (error != errEND_OF_THE_LIST) && (error != errOUT_OF_OBJECT) )
    ;
  else
    error = _reinit_stream_list_and_restore_state( _this, &pname, sname );

  if ( pname != name )
    CALL_SYS_ObjHeapFree( _this, pname );

  PR_TRACE_A1( _this, "Leave ObjPtr::Delete method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Extended method comment
//    First call of Next method after Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root ) {
  tERROR error;
  IO_STATUS_BLOCK io;
  PR_TRACE_A0( _this, "Enter ObjPtr::Reset method" );

  error = _reinit_stream_list( _this, 0, &io );

  PR_TRACE_A1( _this, "Leave ObjPtr::Reset method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// Extended method comment
//    Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameters are:
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result ) {
  hOBJECT   dad;
  tERROR    error;
  hObjPtr   ptr;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter ObjPtr::Clone method" );

  ptr = 0;
  d = _this->data;
  d->win32err = 0;

  dad = CALL_SYS_ParentGet( _this, IID_ANY );
  while( 1 ) {
    if ( PR_FAIL(error=CALL_SYS_ObjectCreate(dad,(hOBJECT*)&ptr,IID_OBJPTR,PID_NTFSSTREAM,SUBTYPE_ANY)) )
      break;
    if ( PR_FAIL(error=CALL_SYS_PropertySetStr(ptr,0,plBASE_FULL_NAME,d->base_name,0,cCP_UNICODE)) )
      break;
    if ( d->mask && *d->mask && PR_FAIL(error=CALL_SYS_PropertySetStr(ptr,0,pgMASK,d->mask,0,cCP_UNICODE)) )
      break;
    if ( !d->count && PR_FAIL(error=CALL_SYS_PropertySetBool(ptr,plCREATE_EMPTY,cTRUE)) )
      break;
    if ( PR_FAIL(error=CALL_SYS_ObjectCreateDone(ptr)) )
      break;
    if ( (d->state == cObjPtrState_BEFORE) || (d->state == cObjPtrState_UNDEFINED) )
      break;
    if ( d->state == cObjPtrState_AFTER ) {
      while( 1 ) {
        error = CALL_ObjPtr_Next( (hi_ObjPtr)ptr );
        if ( (error == errOUT_OF_OBJECT) || (error == errEND_OF_THE_LIST) ) {
          error = errOK;
          break;
        }
        else if ( PR_FAIL(error) )
          break;
      }
    }
    else {
      tWSTRING name;
      error = CALL_SYS_ObjHeapAlloc( _this, &name, WS(MAX_PATH) );
      if ( PR_SUCC(error) ) {
        while( 1 ) {
          error = CALL_ObjPtr_Next( (hi_ObjPtr)ptr );
          if ( PR_FAIL(error) )
            break;
          if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(ptr,0,pgOBJECT_NAME,name,WS(MAX_PATH),cCP_UNICODE)) )
            break;
          if ( !_wcsicmp(d->curr->StreamName,name) )
            break;
        }
      }
    }
    break;
  }
  if ( PR_FAIL(error) ) {
    CALL_SYS_ObjectClose( ptr );
    ptr = 0;
  }

  if ( result )
    *result = ptr;
  PR_TRACE_A2( _this, "Leave ObjPtr::Clone method, ret hObjPtr = %u, error = 0x%08x", ptr, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Behaviour comment
//    First call of Next method after creating or Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this ) {
  tERROR error;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter ObjPtr::Next method" );

	d = _this->data;
	d->win32err = 0;

	if ( d->state == cObjPtrState_AFTER )
		error = errEND_OF_THE_LIST;

	else if ( d->state == cObjPtrState_UNDEFINED )
		error = errOBJECT_BAD_INTERNAL_STATE;

  else if ( d->state == cObjPtrState_BEFORE ) {
    d->curr = d->list;
    if ( !d->curr ) {
      d->state = cObjPtrState_AFTER;
      error = errEND_OF_THE_LIST;
    }
    else if ( d->curr->StreamNameLength && *d->curr->StreamName ) {
      d->state = cObjPtrState_PTR;
      error = errOK;
    }
    else {
      d->state = cObjPtrState_AFTER;
      error = errEND_OF_THE_LIST;
    }
  }

  else if ( !d->curr->NextEntryOffset ) {
    d->curr = 0;
    d->state = cObjPtrState_AFTER;
    error = errEND_OF_THE_LIST;
  }

  else {
    d->curr = NEXT(d->curr);
    if ( d->curr->StreamNameLength && *d->curr->StreamName )
      error = errOK;
    else {
      d->state = cObjPtrState_AFTER;
      error = errEND_OF_THE_LIST;
    }
  }

  PR_TRACE_A1( _this, "Leave ObjPtr::Next method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// Parameters are:
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this ) {
  PR_TRACE_A0( _this, "Enter ObjPtr::StepUp method" );
  PR_TRACE_A0( _this, "Leave ObjPtr::StepUp method, ret tERROR = errNOT_SUPPORTED" );
  return errNOT_SUPPORTED;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this ) {
  PR_TRACE_A0( _this, "Enter ObjPtr::StepDown method" );
  PR_TRACE_A0( _this, "Leave ObjPtr::StepDown method, ret tERROR = errNOT_SUPPORTED" );
  return errNOT_SUPPORTED;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// Parameters are:
//   "name" : The same as "object" parameter of OS::EnumCreate method
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name ) {
  tERROR  error;
  tWCHAR  bname[STACK_STR_LEN];
  tWCHAR* pname = bname;
  const FILE_STREAM_INFORMATION* fsi;
  OPtrData* d;
  PR_TRACE_A0( _this, "Enter ObjPtr::ChangeTo method" );

  d = _this->data;
  d->win32err = 0;
  fsi = d->list;
  if ( !fsi )
    error = errOBJECT_BAD_INTERNAL_STATE;
  else if ( PR_FAIL(error=_extract_name(_this,name,&pname,sizeof(bname))) )
    ;
  else {
    while( fsi->StreamNameLength ) {
      if ( !_wcsicmp(fsi->StreamName,pname) ) {
        d->curr = fsi;
        d->state = cObjPtrState_PTR;
        break;
      }
      else if ( !fsi->NextEntryOffset ) {
        d->state = cObjPtrState_BEFORE;
        error = errOBJECT_NOT_FOUND;
        break;
      }
      fsi = NEXT(fsi);
    }
  }

  if ( pname != bname )
    CALL_SYS_ObjHeapFree( _this, pname );
  PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret tERROR = 0x%08x", error );
  return error;
}
// AVP Prague stamp end





#define PREALLOCATED_SIZE 32768

// ---
tUINT pr_call _new_str_len( const FILE_STREAM_INFORMATION* wrk, tUINT* real ) {
  tUINT nstr_len = wrk->StreamNameLength;
  
  if ( (nstr_len >= WS(6)) && !_wcsnicmp(UNI(MB(wrk->StreamName)+nstr_len-WS(6)),L":$DATA",6) )
    nstr_len -= WS(6);
  if ( nstr_len && (*wrk->StreamName == L':') )
    nstr_len -= WS(1);
  if ( real )
    *real = nstr_len;
  if ( nstr_len ) {
    nstr_len += WS(1);
    return ALIGN_TO_8(nstr_len);
  }
  else
    return 0;
}



// ---
FILE_STREAM_INFORMATION* pr_call _fsi_copy( FILE_STREAM_INFORMATION* dst, const FILE_STREAM_INFORMATION** src, tDWORD* count ) {
  const FILE_STREAM_INFORMATION* next = NEXT(*src);
  tBOOL next_el = !!(*src)->NextEntryOffset;
  tUINT str_real_len;
  tUINT str_len = _new_str_len( *src, &str_real_len );

  if ( str_len ) {
    tUINT size;
    LARGE_INTEGER   stream_size       = (*src)->StreamSize;
    LARGE_INTEGER   stream_alloc_size = (*src)->StreamAllocationSize;
    tWSTRING        str               = (PFSI(*src)->StreamName[0] == L':') ? (PFSI(*src)->StreamName+1) : (PFSI(*src)->StreamName);
    size = CONST_SIZE + str_len;
    *src = next;
    dst->NextEntryOffset      = size;
    dst->StreamNameLength     = str_real_len;
    dst->StreamSize           = stream_size;
    dst->StreamAllocationSize = stream_alloc_size;
    memmove( dst->StreamName, str, str_real_len );
    memset( MB(dst->StreamName)+str_real_len, 0, str_len-str_real_len );
    (*count)++;
  }
  else {
    *src = next;
    dst->NextEntryOffset               = 0;
    dst->StreamNameLength              = 0;
    dst->StreamSize.LowPart            = 0;
    dst->StreamSize.HighPart           = 0;
    dst->StreamAllocationSize.LowPart  = 0;
    dst->StreamAllocationSize.HighPart = 0;
    *dst->StreamName                   = 0;
  }
  if ( next_el )
    return NEXT(dst);
  else
    return 0;
}



// ---
tVOID pr_call _compact_streams( hOBJECT obj, FILE_STREAM_INFORMATION* beg, FILE_STREAM_INFORMATION** res, tDWORD* size, tDWORD* count ) {
  tBOOL last;
  tUINT old_size, new_size;
  tDWORD local_count;
  FILE_STREAM_INFORMATION* prev;
  FILE_STREAM_INFORMATION* dst;
  FILE_STREAM_INFORMATION* wrk;

  if ( !count )
    count = &local_count;

  wrk = beg;
  old_size = 0;
  new_size = 0;
  *count = 0;
  
  do {
    tUINT curr_old_size;
    last = !wrk->NextEntryOffset;
    if ( last )
      curr_old_size = CONST_SIZE + ALIGN_TO_8(wrk->StreamNameLength + WS(1));
    else
      curr_old_size = wrk->NextEntryOffset;
    
    (*count)++;
    if ( (old_size+curr_old_size) < PREALLOCATED_SIZE ) {
      tUINT str_len = _new_str_len( wrk, 0 );
      tUINT curr_new_size = str_len ? (CONST_SIZE + str_len) : 0;
      
      old_size += curr_old_size;
      new_size += curr_new_size;
      prev = wrk;
      wrk = NEXT(wrk);
    }
    else if ( prev ) {
      PFSI(prev)->NextEntryOffset = 0;
      break;
    }
  } while( !last );
  
  if ( !new_size ) {
    CALL_SYS_ObjHeapFree( obj, (tPTR)beg );
    *res = 0;
    *size = 0;
    *count = 0;
  }
  else if ( new_size < old_size ) {
    wrk = beg;
    dst = PFSI(beg);
    *count = 0;
    while( 1 ) {
      FILE_STREAM_INFORMATION* next = _fsi_copy( dst, &wrk, count );
      if ( next )
        dst = next;
      else
        break;
    }
    dst->NextEntryOffset = 0;
    
    if ( PR_SUCC(CALL_SYS_ObjHeapRealloc(obj,(tPTR*)&dst,(tPTR)beg,new_size)) ) {
      *res = dst;
      *size = new_size;
    }
    else {
      *res = beg;
      *size = new_size;
    }
  }
  else if ( PR_SUCC(CALL_SYS_ObjHeapRealloc(obj,(tPTR*)&dst,(tPTR)beg,old_size)) ) {
    *res = dst;
    *size = old_size;
  }
  else {
    *res = beg;
    *size = old_size;
  }
}



// ---
tDWORD pr_call _calc_streams( FILE_STREAM_INFORMATION* beg ) {
  tDWORD count;

  if ( !beg )
    return 0;
  count = 0;
  while( 1 ) {
    count++;
    if ( !beg->NextEntryOffset )
      break;
    beg = NEXT(beg);
  }
  return count;
}



// ---
tERROR pr_call _reinit_stream_list( hi_ObjPtr obj, HANDLE handle, IO_STATUS_BLOCK* pio ) {
  tERROR    e;
  HANDLE    ph = handle;
  OPtrData* d = obj->data;
  FILE_STREAM_INFORMATION* beg;
  
  if ( !handle || (handle == INVALID_HANDLE_VALUE) ) {
    handle = CreateFileW( d->base_name, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0 );
    if ( handle == INVALID_HANDLE_VALUE ) {
      d->win32err = GetLastError();
      return _ConvertWin32Err( d->win32err );
    }
  }
  
  if ( d->list ) {
    CALL_SYS_ObjHeapFree( obj, (tPTR)d->list );
    d->list = 0;
    d->list_size = 0;
  }
  d->curr = 0;
  d->state = cObjPtrState_BEFORE;
  
  if ( PR_FAIL(e=CALL_SYS_ObjHeapAlloc(obj,(tPTR*)&beg,PREALLOCATED_SIZE)) ) 
    ;
  else if ( 0 != g_qif(handle,pio,PFSI(beg),PREALLOCATED_SIZE,FileStreamInformation) ) {
    CALL_SYS_ObjHeapFree( obj, (tPTR)beg );
    e = errOBJECT_CANNOT_BE_INITIALIZED;
  }
  
  if ( handle && (handle != INVALID_HANDLE_VALUE) && (handle != ph) )
    CloseHandle( handle );

  if ( PR_SUCC(e) )
    _compact_streams( (hOBJECT)obj, beg, &PFSI(d->list), &d->list_size, &d->count );
  return e;
}




// ---
tERROR pr_call _has_streams( hIO obj ) {
  tERROR e;
  HANDLE handle;
  tDWORD size;
  IO_STATUS_BLOCK iosb;
  const FILE_STREAM_INFORMATION* wrk;
  
  if ( PR_SUCC(CALL_SYS_PropertyIsPresent(obj,g_pid_streams)) )
    return errOK;

  if ( PR_FAIL(e=CALL_SYS_PropertyGet(obj,0,plNATIVE_HANDLE,&handle,sizeof(handle))) )
    return e;

  if ( PR_FAIL(e=CALL_SYS_ObjHeapAlloc(obj,(tPTR*)&wrk,PREALLOCATED_SIZE)) ) 
    return e;

  if ( 0 != g_qif(handle,&iosb,PFSI(wrk),PREALLOCATED_SIZE,FileStreamInformation) ) {
    CALL_SYS_ObjHeapFree( obj, (tPTR)wrk );
    return errOBJECT_CANNOT_BE_INITIALIZED;
  }
  
  _compact_streams( (hOBJECT)obj, PFSI(wrk), &PFSI(wrk), &size, 0 );
  if ( !wrk )
    return errOBJECT_NOT_FOUND;
  return CALL_SYS_PropertySet( obj, 0, g_pid_streams, PFSI(wrk), size );
}




// ---
tERROR pr_call _extract_name( hi_ObjPtr _this, hOBJECT obj, tPTR* buffer, tDWORD len ) {
  tIID   iid;
  tDWORD olen;
  tERROR error;
  tPTR*  lbuff;
  tPTR   local_buff = 0;
  tDWORD lsize;
  tUINT  pass = 0;
  
  iid = CALL_SYS_PropertyGetDWord( obj, pgINTERFACE_ID );
  lbuff = buffer;
  lsize = len;
  
  while( 1 ) {
    switch( iid ) {
      case IID_STRING : error = CALL_String_ExportToBuff( (hSTRING)obj, &olen, cSTRING_WHOLE, *lbuff, lsize, cCP_UNICODE, cSTRING_Z ); break;
      case IID_IO     : 
      case IID_OBJPTR : error = CALL_SYS_PropertyGetStr( obj, &olen, pgOBJECT_FULL_NAME, *lbuff, lsize, cCP_UNICODE ); break;
      default         : return errINTERFACE_INCOMPATIBLE;
    }
    
    if ( (pass == 0) && (error != errBUFFER_TOO_SMALL) ) 
      break;
    
    if ( pass > 1 )
      break;
    
    if ( pass == 0 ) {
      lbuff = &local_buff;
      lsize = 0;
      pass++;
    }
    else {
      lbuff = buffer;
      lsize = olen;
      if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,lbuff,lsize)) )
        return error;
      pass++;
    }
  }
  
  return error;
}



// ---
tERROR pr_call _get_name( hOBJECT obj, tIID iid, tDWORD* out_len, tPTR buff, tDWORD len ) {
  switch( iid ) {
    case IID_STRING : return CALL_String_ExportToBuff( (hSTRING)obj, out_len, cSTRING_WHOLE, buff, len, cCP_UNICODE, cSTRING_Z );
    case IID_IO     : 
    case IID_OBJPTR : return CALL_SYS_PropertyGetStr( obj, out_len, pgOBJECT_FULL_NAME, buff, len, cCP_UNICODE );
    default         : return errINTERFACE_INCOMPATIBLE;
  }
}



// ---
tERROR pr_call _get_full_name( hi_ObjPtr _this, tWSTRING* name, tUINT* size ) {
  tUINT blen, nlen, nsize;
  tERROR error;
  OPtrData* d = _this->data;
  if ( (d->state != cObjPtrState_PTR) || !d->curr )
    return errOBJECT_BAD_INTERNAL_STATE;
  nsize = *size;
  blen = (tUINT)WS( wcslen(d->base_name) );
  nlen = d->curr->StreamNameLength + WS(1);
  if ( ((blen+nlen+WS(1)) > *size) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,name,nsize=blen+nlen+WS(1))) )
    return error;
  *size = nsize;
  memcpy( *name, d->base_name, blen );
  *UNI(MB(*name)+blen) = L':';
  memcpy( MB(*name)+blen+WS(1), d->curr->StreamName, d->curr->StreamNameLength + WS(1) );
  return errOK;
}




// ---
tERROR pr_call _combain_name( hi_ObjPtr _this, hOBJECT obj, tPTR* buffer, tDWORD* len, tWSTRING* stream ) {
  tIID   iid;
  tUINT  blen;
  tUINT  olen;
  tERROR error;
  tBOOL  colon;
  tWSTRING stream_name;
  OPtrData* d = _this->data;
  
  iid  = CALL_SYS_PropertyGetDWord( obj, pgINTERFACE_ID );
  if ( PR_FAIL(error=_get_name(obj,iid,&olen,0,0)) )
    return error;
  
  blen = (tUINT)WS( wcslen(d->base_name) );
  
  if ( (blen+olen+1) > *len) {
    if( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,buffer,blen+olen+1)) )
      return error;
    else
      *len = blen + olen + 1;
  }

  memcpy( *buffer, d->base_name, blen );
  stream_name = UNI(MB(*buffer) + blen);
  if ( PR_FAIL(error=_get_name(obj,iid,&olen,stream_name,*len-blen-1)) )
    return error;
  
  if ( PR_FAIL(error=_is_stream_name(stream_name,&colon)) )
    return error;
  
  if ( !colon ) {
    memmove( stream_name+1, stream_name, olen );
    *stream_name = L':';
  }

  if ( stream )
    *stream = stream_name + 1;
  
  return errOK;
}




// ---
tERROR pr_call _get_ptr_name( hi_ObjPtr _this, tWSTRING* name, tUINT* size, tBOOL allocated ) {
  tDWORD s;
  tWSTRING n = allocated ? *name : 0;
  tERROR e = CALL_SYS_PropertyGetStr( _this, &s, pgOBJECT_NAME, *name, *size, cCP_UNICODE );
  while( e == errBUFFER_TOO_SMALL ) {
    e = CALL_SYS_ObjHeapRealloc( _this, &n, n, *size+STACK_STR_LEN );
    if ( PR_FAIL(e) )
      return e;
    *name = n;
    *size += STACK_STR_LEN;
    e = CALL_SYS_PropertyGetStr( _this, &s, pgOBJECT_NAME, *name, *size, cCP_UNICODE );
  }
  return e;
}




// ---
tERROR pr_call _reinit_stream_list_and_restore_state( hi_ObjPtr _this, tWSTRING* name, tUINT size ) {
  IO_STATUS_BLOCK io;
  tObjPtrState state;
  tERROR error;
  const FILE_STREAM_INFORMATION* curr;
  tWCHAR   cname[STACK_STR_LEN];
  tWSTRING pcname;
  tUINT    scname;
  
  state = _this->data->state;
  **name = 0;

  if ( (state != cObjPtrState_PTR) || !_this->data->curr )
    return errOK;

  curr = _this->data->curr;
  if ( ((curr->StreamNameLength+WS(1)) > size) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,name,curr->StreamNameLength+WS(1))) )
    return error;

  memcpy( *name, curr->StreamName, curr->StreamNameLength+WS(1) );

  if ( PR_FAIL(error=_reinit_stream_list(_this,0,&io)) )
    return error;

  if ( (state == cObjPtrState_BEFORE) || (state == cObjPtrState_UNDEFINED) ) {
    _this->data->state = state;
    return errOK;
  }

  if ( state == cObjPtrState_AFTER ) {
    while( PR_SUCC(error=CALL_ObjPtr_Next(_this)) )
      ;
    if ( (error == errOUT_OF_OBJECT) || (error == errEND_OF_THE_LIST) )
      return errOK;
    else
      return error;
  }

  pcname = cname;
  scname = sizeof(cname);
  while( PR_SUCC(error=CALL_ObjPtr_Next(_this)) ) {
    if ( PR_FAIL(error=_get_ptr_name(_this,&pcname,&size,scname==sizeof(cname))) )
      break;
    if ( !_wcsicmp(*name,pcname) )
      break;
  }
  return error;
}


  
/*
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_handle )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- BASE_HANDLE
tERROR pr_call ObjPtr_PROP_set_handle( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR    error;
  OPtrData* d;
  IO_STATUS_BLOCK iosb;
  FILE_NAME_INFORMATION* name = 0;
  PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_handle for property plBASE_HANDLE" );

  d = _this->data;
  d->win32err = 0;
  d->base_io = 0;
  if ( d->base_name ) {
    CALL_SYS_ObjHeapFree( _this, d->base_name );
    d->base_name = 0;
  }

  if ( !*(HANDLE*)buffer ) {
    d->base_handle = 0;
    *out_size = sizeof(HANDLE);
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cFALSE );
    error = errOK;
  }
  else if ( (*(HANDLE*)buffer == INVALID_HANDLE_VALUE) || (FILE_TYPE_DISK != GetFileType(*(HANDLE*)buffer)) ) {
    d->base_handle = 0;
    *out_size = 0;
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cFALSE );
    error = errPROPERTY_VALUE_INVALID;
  }
  else if ( g_qif(*(HANDLE*)buffer,&iosb,name,NAME_REQUESTED_SIZE,FileNameInformation) )
    error = errUNEXPECTED;
  else if ( PR_FAIL(error=_not_name_of_the_stream(name->FileName)) )
    ;
  else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&d->base_name,name->FileNameLength+WS(1))) )
    ;
  else {
    d->base_handle = *(HANDLE*)buffer;
    *out_size = sizeof(HANDLE);
    memcpy( d->base_name, name->FileName, name->FileNameLength+WS(1) );
    CALL_SYS_PropertyInitialized( _this, plBASE_FULL_NAME, cTRUE );
    error = errOK;
  }

  if ( name )
    CALL_SYS_ObjHeapFree( _this, name );

	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_handle for property plBASE_HANDLE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
  return error;
}
// AVP Prague stamp end



 */
