// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  12 August 2004,  12:34 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Low level disk I/O for Unix-like systems
// Author      -- Sychev
// File Name   -- objptr.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_unixdiskio.h>
#include <iface/i_root.h>
#include <iface/i_string.h>
#include <iface/i_io.h>
#include <pr_oid.h>
#include "diskio.h"
#include <iface/i_heap.h>
// AVP Prague stamp end



#define IO_This		_this->data

// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_Private {
  hOBJECT  m_BasedOn;             // --
  tSTRING* m_DeviceNamesList;     // --
  tDWORD   m_DeviceNamesNumber;   // --
  tDWORD   m_CurrentListPosition; // --
  hHEAP    m_Heap;                // --
} Private;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr {
  const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  Private*           data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInit method" );

  // Place your code here
  //  IO_This->m_Dir = 0;
  IO_This->m_DeviceNamesList = 0;
  IO_This->m_DeviceNamesNumber = 0;
  IO_This->m_CurrentListPosition = -1;
  IO_This->m_Heap = 0;
  error = errOK;
  PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInit method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  glob_t aGlobBuffer;
  tDWORD i = 0;

  PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInitDone method" );

  // Place your code here
  error = errOBJECT_CANNOT_BE_INITIALIZED;
  if ( selectDevices ( &aGlobBuffer ) &&
       PR_SUCC ( error = CALL_SYS_ObjectCreate( _this, (hOBJECT*)&IO_This->m_Heap, IID_HEAP, PID_ANY, 0 ) ) ) {
    if ( PR_SUCC ( error = CALL_Heap_Alloc ( IO_This->m_Heap,
                                             (tPTR*)&IO_This->m_DeviceNamesList,
                                             aGlobBuffer.gl_pathc * sizeof ( tSTRING ) ) ) ) {
      for ( ; i < aGlobBuffer.gl_pathc; ++i ) {
        if ( PR_SUCC ( error = CALL_Heap_Alloc ( IO_This->m_Heap,
                                                 (tPTR*)&IO_This->m_DeviceNamesList [ i ],
                                                 strlen ( aGlobBuffer.gl_pathv [ i ] ) + 1 ) ) )
          strcat ( IO_This->m_DeviceNamesList [ i ], aGlobBuffer.gl_pathv [ i ] );
        else
          break;
      }
      IO_This->m_DeviceNamesNumber = aGlobBuffer.gl_pathc;
      globfree ( &aGlobBuffer );
    }
    if ( PR_FAIL ( error ) )
      CALL_SYS_ObjectClose ( IO_This->m_Heap );
  }
  PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInitDone method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this )
{

  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter ObjPtr::ObjectClose method" );

  // Place your code here
  #warning "Consideration is needed!"
  //CALL_SYS_ObjectClose ( IO_This->m_Heap );
  PR_TRACE_A1( _this, "Leave ObjPtr::ObjectClose method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PtrPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ORIGIN
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_AVAILABILITY
//  -- OBJECT_PTR_STATE
//  -- OBJECT_SIZE
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_PtrPropRead( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  char aBuffer [MAX_PATH] = {0};
  tORIG_ID anOrigin;
  tQWORD result = 0;
  DiskGeometry aDiskGeometry;
  PR_TRACE_A0( _this, "Enter *GET* multyproperty method ObjectClose" );

  if ( prop == pgOBJECT_PTR_STATE ) {
    *out_size = sizeof ( cObjPtrState_UNDEFINED );
    if ( buffer != NULL ) {
      if ( *out_size > size )
        error = errBUFFER_TOO_SMALL;
      else {
        if ( IO_This->m_DeviceNamesList == 0 )
          *buffer = cObjPtrState_UNDEFINED;
        else {
          if ( IO_This->m_CurrentListPosition == (tDWORD)-1 )
            *buffer = cObjPtrState_BEFORE;
          else
            if ( IO_This->m_CurrentListPosition < IO_This->m_DeviceNamesNumber )
              *buffer = cObjPtrState_PTR;
            else
              *buffer = cObjPtrState_AFTER;
        }
        error = errOK;
      }
    }
  }
  else
    if ( ( IO_This->m_CurrentListPosition == (tDWORD)-1 ) ||
         ( IO_This->m_CurrentListPosition >= IO_This->m_DeviceNamesNumber ) )
      error = errOBJECT_BAD_INTERNAL_STATE;

  if ( PR_SUCC ( error ) )
    switch ( prop ) {

    case pgOBJECT_ORIGIN:
      *out_size = sizeof ( tORIG_ID );
      if ( buffer != NULL ) {
        if ( *out_size > size )
          error = errBUFFER_TOO_SMALL;
        else
          switch ( getDeviceType ( IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ] ) ) {
          case dtFloppy:
          case dtCDROM:
          case dtLogicalDisk:
            anOrigin = OID_LOGICAL_DRIVE;
            break;
          case dtPhysicalDevice:
            anOrigin = OID_PHYSICAL_DISK;
            break;
          default:
            error = errUNEXPECTED;
            break;
          }
        memcpy ( buffer, &anOrigin, *out_size );
      }
      break;

    case pgOBJECT_NAME:
      strncpy ( aBuffer, IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ], sizeof ( aBuffer ) );
      memmove ( aBuffer, basename ( aBuffer ), sizeof ( aBuffer ) );
      *out_size = strlen ( aBuffer ) + 1;
      if ( buffer != NULL ) {
        if ( *out_size > size )
          error = errBUFFER_TOO_SMALL;
        else
          strncpy ( buffer, aBuffer, *out_size );
      }
      break;

    case pgOBJECT_PATH:
      strncpy ( aBuffer, IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ], sizeof ( aBuffer ) );
      memmove ( aBuffer, dirname ( aBuffer ), sizeof ( aBuffer ) );
      *out_size = strlen ( aBuffer ) + 1;
      if ( buffer != NULL ) {
        if ( *out_size > size )
          error = errBUFFER_TOO_SMALL;
        else
          strncpy ( buffer, aBuffer, *out_size );
      }
      break;

    case pgOBJECT_FULL_NAME:
      *out_size = strlen ( IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ] ) + 1;
      if ( buffer != NULL ) {
        if ( *out_size > size )
          error = errBUFFER_TOO_SMALL;
        else
          strncpy ( buffer, IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ], *out_size );
      }
      break;

    case pgOBJECT_AVAILABILITY:
      *out_size = 0;
      error = errNOT_IMPLEMENTED;
      break;



    case pgOBJECT_SIZE:
      if (buffer != NULL) {
        *out_size = sizeof ( tDWORD );
        if ( *out_size > size )
          error = errBUFFER_TOO_SMALL;
        else {
          result = CALL_SYS_PropertyGetQWord ( _this, pgOBJECT_SIZE_Q );
          *(tDWORD*)buffer = (tDWORD) result;
          error = errOK;
        }
      }
      break;

    case pgOBJECT_SIZE_Q:
      *out_size = sizeof ( tQWORD );
      if ( buffer != NULL)  {
        if ( *out_size > size)
          error = errBUFFER_TOO_SMALL;
        else {
          if ( getDeviceGeometry ( IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ], &aDiskGeometry ) )
            *(tQWORD*)buffer = aDiskGeometry.m_Cylinders *
              aDiskGeometry.m_Heads *
              aDiskGeometry.m_Sectors *
              aDiskGeometry.m_SectorSize;
          else
            error = errUNEXPECTED;
        }
      }
      break;

    default:
      *out_size = 0;
      break;
    }

  PR_TRACE_A2( _this, "Leave *GET* multyproperty method ObjectClose, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//    Creates hIO or hObjEnum object current in enumeration.
// Result comment
//    Returns hIO or hObjEnum object. Depends on current object.
// Parameters are:
//   "access_mode" : New object open mode
//   "open_mode"   : Attributes of the new object
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode )
{
  tERROR error = errNOT_IMPLEMENTED;
  hIO    ret_val = NULL;
  PR_TRACE_A0( _this, "Enter ObjPtr::IOCreate method" );

  // Place your code here
  if ( name || (open_mode != fOMODE_OPEN_IF_EXIST))
    error = errNOT_SUPPORTED;
  else {
    hSTRING hStr = 0;
    if ( PR_SUCC ( CALL_SYS_ObjectCreateQuick(_this, &hStr, IID_STRING, PID_ANY, 0 ) ) ) {
      CALL_String_SetCP ( hStr, cCP_ANSI );
      error = CALL_SYS_ObjectCreate ( _this, &ret_val, IID_IO, PID_UNIXDISKIO, 0 );
      if ( PR_SUCC ( error ) ) {
        tDWORD len;
        if ( PR_SUCC ( CALL_String_AddFromProp ( hStr, &len, (hOBJECT) _this, pgOBJECT_NAME ) ) ) {
          error = CALL_String_ExportToProp ( hStr, &len, cSTRING_WHOLE, (hOBJECT) ret_val, pgOBJECT_NAME );
          if ( PR_FAIL ( CALL_SYS_ObjectCreateDone ( ret_val ) ) ) {
            CALL_SYS_ObjectClose ( ret_val );
            ret_val = 0;
            error = errOBJECT_NOT_CREATED;
          }
          else
            error = errOK;
        }
      }
			
      CALL_SYS_ObjectClose ( hStr );
    }
  }
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave ObjPtr::IOCreate method, ret hIO = %S, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::Copy method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave ObjPtr::Copy method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::Rename method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave ObjPtr::Rename method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    pointer must be advanced to the next object
// Parameters are:
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::Delete method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave ObjPtr::Delete method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Extended method comment
//    First call of Next method after Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::Reset method" );

  // Place your code here
  IO_This->m_CurrentListPosition = -1;

  PR_TRACE_A1( _this, "Leave ObjPtr::Reset method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// Extended method comment
//    Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameters are:
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result )
{
  tERROR  error = errNOT_IMPLEMENTED;
  hObjPtr ret_val = NULL;
  PR_TRACE_A0( _this, "Enter ObjPtr::Clone method" );

  // Place your code here

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave ObjPtr::Clone method, ret hObjPtr = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Behaviour comment
//    First call of Next method after creating or Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::Next method" );

  // Place your code here
  error = errEND_OF_THE_LIST;
  while ( ( ++IO_This->m_CurrentListPosition < IO_This->m_DeviceNamesNumber ) &&
          ( getDeviceMode ( IO_This->m_DeviceNamesList [ IO_This->m_CurrentListPosition ] ) == dmUnknown ) );
  if ( IO_This->m_CurrentListPosition < IO_This->m_DeviceNamesNumber )
    error = errOK;
  PR_TRACE_A1( _this, "Leave ObjPtr::Next method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// Parameters are:
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::StepUp method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave ObjPtr::StepUp method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::StepDown method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave ObjPtr::StepDown method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// Parameters are:
//   "name" : The same as "object" parameter of OS::EnumCreate method
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter ObjPtr::ChangeTo method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Enum of disk devices", 21 )
  mLOCAL_PROPERTY_FN( pgOBJECT_ORIGIN, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_NAME, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_PATH, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, Private, m_BasedOn, cPROP_BUFFER_READ )
  mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
  mLOCAL_PROPERTY_FN( pgOBJECT_PTR_STATE, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_PtrPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_PtrPropRead, NULL )
mPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = {
  (tIntFnRecognize)         NULL,
  (tIntFnObjectNew)         NULL,
  (tIntFnObjectInit)        ObjPtr_ObjectInit,
  (tIntFnObjectInitDone)    ObjPtr_ObjectInitDone,
  (tIntFnObjectCheck)       NULL,
  (tIntFnObjectPreClose)    NULL,
  (tIntFnObjectClose)       ObjPtr_ObjectClose,
  (tIntFnChildNew)          NULL,
  (tIntFnChildInitDone)     NULL,
  (tIntFnChildClose)        NULL,
  (tIntFnMsgReceive)        NULL,
  (tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iObjPtrVtbl e_ObjPtr_vtbl = {
  (fnpObjPtr_IOCreate)      ObjPtr_IOCreate,
  (fnpObjPtr_Copy)          ObjPtr_Copy,
  (fnpObjPtr_Rename)        ObjPtr_Rename,
  (fnpObjPtr_Delete)        ObjPtr_Delete,
  (fnpObjPtr_Reset)         ObjPtr_Reset,
  (fnpObjPtr_Clone)         ObjPtr_Clone,
  (fnpObjPtr_Next)          ObjPtr_Next,
  (fnpObjPtr_StepUp)        ObjPtr_StepUp,
  (fnpObjPtr_StepDown)      ObjPtr_StepDown,
  (fnpObjPtr_ChangeTo)      ObjPtr_ChangeTo
};
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "ObjPtr". Register function
tERROR pr_call ObjPtr_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter ObjPtr::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_OBJPTR,                             // interface identifier
    PID_UNIXDISKIO,                         // plugin identifier
    0x00000000,                             // subtype identifier
    ObjPtr_VERSION,                         // interface version
    VID_SYCHEV,                             // interface developer
    &i_ObjPtr_vtbl,                         // internal(kernel called) function table
    (sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
    &e_ObjPtr_vtbl,                         // external function table
    (sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
    ObjPtr_PropTable,                       // property table
    mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
    sizeof(Private),                        // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave ObjPtr::Register method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgMASK
// You have to implement propetry: pgOBJECT_ATTRIBUTES
// You have to implement propetry: pgOBJECT_HASH
// You have to implement propetry: pgOBJECT_REOPEN_DATA
// AVP Prague stamp end



