// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  29 July 2004,  12:05 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Unix native file IO
// Author      -- petrovich
// File Name   -- unix_objptr.cpp
// Additional info
//    File input/output for Unix
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_nfio.h>
#include <iface/i_root.h>
#include <iface/i_string.h>
#include <iface/i_os.h>
#include <sys/types.h>
#include <dirent.h>
#include "unix_utils.h"
#include <pr_oid.h>
#include <iface/e_ktime.h>
#include <string.h>
#include <fnmatch.h>
#include <limits.h>
#include <errno.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct ObjPtrData : public cObjPtr /*cObjImpl*/
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call getName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call isFolder( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getMask( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setMask( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFileSize( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFileSize( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFileSize64( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFileSize64( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call isNotImplemented( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations

// operations with the current object
// group of methods to navigate object pointers
	tERROR pr_call IOCreate( hIO* result, hSTRING p_name, tDWORD p_access_mode, tDWORD p_open_mode );
	tERROR pr_call Copy( hOBJECT p_dst_name, tBOOL p_overwrite );
	tERROR pr_call Rename( hOBJECT p_new_name, tBOOL p_overwrite );
	tERROR pr_call Delete();

// navigation methods
// group of methods to navigate object pointers
	tERROR pr_call Reset( tBOOL p_to_root );
	tERROR pr_call Clone( hObjPtr* result );
	tERROR pr_call Next();
	tERROR pr_call StepUp();
	tERROR pr_call StepDown();
	tERROR pr_call ChangeTo( hOBJECT p_name );

// Data declaration
  tSTRING       m_mask;                // pointer to the amsk
  tBOOL         m_apply_mask_to_folders; // is mask applied to folder names
  tSTRING       m_path;                // full path string
  DIR*          m_dir_stream;          // directory stream
  union {
    struct dirent m_dir_entry;           // directory entry
#if defined (__sun__)
      char dummy [ offsetof(struct dirent,d_name) + PATH_MAX + 1 ];
#endif
  };
  tObjPtrState  m_state;               // state of the pointer
  hOS           m_os;                  // based on this object
  tDWORD        m_system_error;        // last system error (errno)
  tDWORD        m_origin;              // object origin
  tBOOL         m_process_folders;     // is folders start to be visible in the enumeration
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(ObjPtrData)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
const tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR ObjPtrData::ObjectInit()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::ObjectInit method" );

  // Place your code here
  m_state = cObjPtrState_UNDEFINED;
  m_origin = OID_GENERIC_IO;
  cObject* aOS = static_cast<cObject*> ( sysGetParent ( IID_ANY ) );
  if ( !aOS || (IID_OS != aOS->propGetIID () ) || ( PID_NFIO != aOS->propGetPID () ) )
    m_os = 0;
  else
    m_os = reinterpret_cast <cOS*> ( aOS );

  PR_TRACE_A1( this, "Leave ObjPtr::ObjectInit method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR ObjPtrData::ObjectInitDone()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::ObjectInitDone method" );

  // Place your code here
  error = Reset( cFALSE );
  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::ObjectInitDone method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR ObjPtrData::ObjectClose()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::ObjectClose method" );

  // Place your code here
  m_state = cObjPtrState_UNDEFINED;

  if ( m_dir_stream ) 
    closedir ( m_dir_stream );

  if ( m_mask )
    heapFree ( m_mask );

  if ( m_path )
    heapFree ( m_path );

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::ObjectClose method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getName )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR ObjPtrData::getName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtr::getName for property pgOBJECT_NAME" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( !buffer && size ) 
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) ) {
    *l_res_ptr = strlen ( m_dir_entry.d_name ) + 1;
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr )
      error = errBUFFER_TOO_SMALL;
    else
      strncpy ( buffer, m_dir_entry.d_name, *l_res_ptr );
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtr::getName for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}

// AVP Prague stamp begin( Property method implementation, getPath )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR ObjPtrData::getPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtr::getPath for property pgOBJECT_PATH" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  if ( !buffer && size ) 
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) && m_path ) {
    *l_res_ptr = strlen ( m_path ) + 1;
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr )
      error = errBUFFER_TOO_SMALL;
    else
      strncpy ( buffer, m_path, *l_res_ptr );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtr::getPath for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, setPath )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR ObjPtrData::setPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method ObjPtr::setPath for property pgOBJECT_PATH" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  if ( !buffer || !size )
    error = errPARAMETER_INVALID;

  tCHAR anAbsolutePath [ MAX_PATH ] = {0};
  error = _absolutePath ( buffer, size, anAbsolutePath, sizeof ( anAbsolutePath ) );
  
  if ( PR_SUCC ( error ) ) {
    heapFree ( m_path );
    error = heapAlloc ( reinterpret_cast <tPTR*> ( &m_path ),  strlen ( anAbsolutePath ) + 1 );
  }

  if ( PR_SUCC ( error ) ) 
    strcpy ( m_path, anAbsolutePath );

  if ( PR_SUCC ( error ) ) 
    error = Reset ( cFALSE );

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *SET* method ObjPtr::setPath for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFullName )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR ObjPtrData::getFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtrData::getFullName for property pgOBJECT_FULL_NAME" );

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( !buffer && size ) 
    error =  errPARAMETER_INVALID;

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  
  if ( PR_SUCC ( error ) ) {
    *l_res_ptr = strlen ( m_path ) + strlen ( m_dir_entry.d_name ) + 2;
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr )
      error = errBUFFER_TOO_SMALL;
    else {
      strcat ( buffer, m_path );
      strcat ( buffer, "/" );
      strcat ( buffer, m_dir_entry.d_name );
    }
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtrData::getFullName for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, isFolder )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- IS_FOLDER
tERROR ObjPtrData::isFolder( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtr::isFolder for property pgIS_FOLDER" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  
  if ( !buffer && size )
    error = errPARAMETER_INVALID;

  *l_res_ptr = sizeof ( tBOOL );

  if ( PR_SUCC ( error ) ) {
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr ) 
      error = errBUFFER_TOO_SMALL;
    else {
      tBOOL* aResult = reinterpret_cast <tBOOL*> ( buffer );
      error = _isFolder ( m_path,  m_dir_entry.d_name, *aResult, m_system_error );
    }
  }
  
  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtr::isFolder for property pgIS_FOLDER, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getMask )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- MASK
tERROR ObjPtrData::getMask( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgMASK" );
  
  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  if ( !buffer && size )
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) && m_mask ) {
    *l_res_ptr = strlen ( m_mask ) + 1;
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr )
      error = errBUFFER_TOO_SMALL;
    else
      strncpy ( buffer, m_mask, *l_res_ptr );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgMASK, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setMask )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- MASK
tERROR ObjPtrData::setMask( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method ObjPtr::setMask for property pgMASK" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( !buffer || !size ) 
    error =  errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) ) {
    heapFree ( m_mask );
    m_mask = 0;
  }

  if ( PR_SUCC ( error = heapAlloc ( reinterpret_cast<tPTR*> ( &m_mask ), size + 1 ) ) )
    strncpy ( m_mask, buffer, size );
  
  if ( PR_SUCC ( error ) )
    error = Reset ( cFALSE );

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *SET* method ObjPtr::setMask for property pgMASK, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFileSize )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR ObjPtrData::getFileSize( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtr::getFileSize for property pgOBJECT_SIZE" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;
  
  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  
  if ( size && !buffer )
    error = errPARAMETER_INVALID;

  *l_res_ptr = sizeof ( tDWORD );

  if ( !size )
    error = errOK;
  else if ( size < *l_res_ptr )
    error = errBUFFER_TOO_SMALL;
  else {
    tQWORD aFileSize = 0;
    if ( PR_SUCC ( error ) ) 
      error = _getFileSize ( m_path, m_dir_entry.d_name, aFileSize, m_system_error );
    
    if ( PR_SUCC ( error ) && ( aFileSize > cMAX_UINT ))
      error = errBUFFER_TOO_SMALL;
    
    if ( PR_SUCC ( error ) ) {
      tDWORD* aResult = reinterpret_cast <tDWORD*> ( buffer );
      *aResult = aFileSize;
    }
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtr::getFileSize for property pgOBJECT_SIZE, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFileSize )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_SIZE
tERROR ObjPtrData::setFileSize( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method ObjPtr::setFileSize for property pgOBJECT_SIZE" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  
  if ( !buffer || !size || size < sizeof ( tDWORD ) )  
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) ) {
    tQWORD aFileSize = *(reinterpret_cast <tDWORD*> ( buffer ) );
    error = _setFileSize ( m_path, m_dir_entry.d_name, aFileSize, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *SET* method ObjPtr::setFileSize for property pgOBJECT_SIZE, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFileSize64 )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR ObjPtrData::getFileSize64( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtr::setFileSize for property pgOBJECT_SIZE_Q" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  
  if ( size && !buffer )
    error = errPARAMETER_INVALID;

  *l_res_ptr = sizeof ( tQWORD );

  if ( !size )
    error = errOK;
  else if ( size < sizeof ( tDWORD ) )
    error = errBUFFER_TOO_SMALL;
  else {
    tQWORD* aFileSize = reinterpret_cast <tQWORD*> ( buffer );;
    error = _getFileSize ( m_path, m_dir_entry.d_name, *aFileSize, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtr::setFileSize for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFileSize64 )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_SIZE_Q
tERROR ObjPtrData::setFileSize64( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method ObjPtr::setFileSize64 for property pgOBJECT_SIZE_Q" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;
  
  if ( !buffer || !size || size < sizeof ( tQWORD ) )  
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) ) {
    tQWORD aFileSize = *(reinterpret_cast <tQWORD*> ( buffer ) );
    error = _setFileSize ( m_path, m_dir_entry.d_name, aFileSize, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *SET* method ObjPtr::setFileSize64 for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFileAttributes )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR ObjPtrData::getFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjPtr::getFileAttributes for property pgOBJECT_ATTRIBUTES" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( size && !buffer )
    error = errPARAMETER_INVALID;

  *l_res_ptr = sizeof ( tDWORD );

  if ( !size )
    error = errOK;
  else if ( size < sizeof ( tDWORD ) )
    error = errBUFFER_TOO_SMALL;
  else {
    tDWORD* aResult = reinterpret_cast <tDWORD*> ( buffer );
    error = _getFileAttributes ( m_path, m_dir_entry.d_name, *aResult, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* method ObjPtr::getFileAttributes for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFileAttributes )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR ObjPtrData::setFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method ObjPtr::setFileAttributes for property pgOBJECT_ATTRIBUTES" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( !buffer || !size || size < sizeof ( tDWORD ) )  
    error = errPARAMETER_INVALID;
  
  if ( PR_SUCC ( error ) ) {
    tDWORD aFileAttributes = *(reinterpret_cast <tDWORD*> ( buffer ) );
    error = _setFileAttributes ( m_path, m_dir_entry.d_name, aFileAttributes, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *SET* method ObjPtr::setFileAttributes for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, isNotImplemented )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_HASH
//  -- OBJECT_REOPEN_DATA
tERROR ObjPtrData::isNotImplemented( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter *GET* multyproperty method ObjPtr::isNotImplemented" );
  PR_TRACE_A2( this, "Leave *GET* multyproperty method ObjPtr::isNotImplemented, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFileTime )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR ObjPtrData::getFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* multyproperty method ObjPtr::getFileTime" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( size && !buffer )
    error = errPARAMETER_INVALID;

  *l_res_ptr = sizeof ( tDATETIME);
  
  if ( !size )
    error = errOK;
  else if ( size < sizeof ( tDATETIME ) )
    error = errBUFFER_TOO_SMALL;
  else {
    tDATETIME* aResult = reinterpret_cast <tDATETIME*> ( buffer );
    error = _getFileTime ( m_path, m_dir_entry.d_name, prop, *aResult, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *GET* multyproperty method ObjPtr::getFileTime, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFileTime )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR ObjPtrData::setFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* multyproperty method ObjPtr::setFileTime" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  m_system_error = 0;

  if ( m_state != cObjPtrState_PTR )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( !buffer || !size || size < sizeof ( tDATETIME ) )  
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) ) {
    tDATETIME* aFileTime = reinterpret_cast <tDATETIME*> ( buffer );
    error = _getFileTime ( m_path, m_dir_entry.d_name, prop, *aFileTime, m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave *SET* multyproperty method ObjPtr::setFileTime, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
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
tERROR ObjPtrData::IOCreate( hIO* result, hSTRING p_name, tDWORD p_access_mode, tDWORD p_open_mode )
{
  tERROR error = errOK;
  hIO    ret_val = NULL;
  PR_TRACE_A0( this, "Enter ObjPtr::IOCreate method" );

  // Place your code here
  m_system_error = 0;
  
  if ( !p_name && ( m_state != cObjPtrState_PTR )  )
    error = errOBJECT_BAD_INTERNAL_STATE;

  if ( PR_SUCC ( error ) )
    error = sysCreateObject ( reinterpret_cast <hOBJECT*> ( &ret_val ), IID_IO, PID_NFIO, SUBTYPE_ANY );
  
  if ( PR_SUCC ( error ) )
    error=ret_val->propSetStr ( 0, pgOBJECT_PATH, m_path );
      
  if ( PR_SUCC ( error ) )
    if ( p_name ) 
      error = p_name->ExportToProp ( 0, cSTRING_WHOLE,  reinterpret_cast <hOBJECT> ( ret_val ), pgOBJECT_NAME );
    else 
      error=ret_val->propSetStr ( 0, pgOBJECT_NAME, m_dir_entry.d_name ) ;

  if ( PR_SUCC ( error ) ) 
    error = ret_val->propSetDWord ( pgOBJECT_OPEN_MODE, p_open_mode );
  
  if ( PR_SUCC ( error ) ) 
    error = ret_val->propSetDWord ( pgOBJECT_ACCESS_MODE, p_access_mode );
  
  if ( PR_SUCC ( error ) ) 
    error = ret_val->sysCreateObjectDone ();
  
  if ( PR_FAIL(error) && ret_val ) {
    ret_val->sysCloseObject ();
    ret_val = 0; 
  }
  
  if ( result )
    *result = ret_val;

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave ObjPtr::IOCreate method, ret hIO = %S, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR ObjPtrData::Copy( hOBJECT p_dst_name, tBOOL p_overwrite )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::Copy method" );

  // Place your code here
  m_system_error = 0;

  tIID anIID = p_dst_name->propGetDWord ( pgINTERFACE_ID );
  
  if ( ( anIID != IID_STRING ) || ( anIID != IID_OBJPTR ) ) 
    error = errPARAMETER_INVALID;

  tCHAR aSource [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = propGetStr ( 0, pgOBJECT_FULL_NAME, aSource, sizeof ( aSource ) );

  tCHAR aDestination [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_dst_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aDestination, sizeof ( aDestination ) );
  
  if ( PR_SUCC ( error ) )
    error = _copy ( aSource, aDestination, p_overwrite, m_system_error );
  
  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::Copy method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR ObjPtrData::Rename( hOBJECT p_new_name, tBOOL p_overwrite )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::Rename method" );

  // Place your code here
  m_system_error = 0;

  tIID anIID = p_new_name->propGetDWord ( pgINTERFACE_ID );
  
  if ( ( anIID != IID_STRING ) || ( anIID != IID_OBJPTR ) ) 
    error = errPARAMETER_INVALID;

  tCHAR aSourceName [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = propGetStr ( 0, pgOBJECT_FULL_NAME, aSourceName, sizeof ( aSourceName ) );

  tCHAR aDestinationName [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_new_name->propGetStr ( 0, pgOBJECT_NAME, aDestinationName, sizeof ( aDestinationName ) );

  tCHAR aDestinationDir [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_new_name->propGetStr ( 0, pgOBJECT_PATH, aDestinationDir, sizeof ( aDestinationDir ) );


  if ( PR_SUCC ( error ) )
    error = _rename ( aDestinationDir, aSourceName, aDestinationDir, aDestinationName, p_overwrite, m_system_error );
  
  tBOOL aProcessFolders = m_process_folders;
  
  if ( PR_SUCC ( error ) )
    error = Reset ( cFALSE );

  if ( PR_SUCC ( error ) ) {
    m_process_folders = aProcessFolders;
    while ( PR_SUCC ( error = Next () ) && strcmp ( m_dir_entry.d_name, aDestinationName ) );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));  
  PR_TRACE_A1( this, "Leave ObjPtr::Rename method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    pointer must be advanced to the next object
// Parameters are:
tERROR ObjPtrData::Delete()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::Delete method" );

  // Place your code here
  m_system_error = 0;

  tCHAR aFileName [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = propGetStr ( 0, pgOBJECT_FULL_NAME, aFileName, sizeof ( aFileName ) );

  if ( PR_SUCC ( error ) ) 
    error = Next ();
  
  if ( PR_SUCC ( error ) ) 
    error = _delete ( aFileName, m_system_error );

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::Delete method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Extended method comment
//    First call of Next method after Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR ObjPtrData::Reset( tBOOL p_to_root )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::Reset method" );

  // Place your code here
  if ( !m_path && !p_to_root ) {
    tCHAR aCurrentDir [] = "./";
    error = setPath ( 0, pgOBJECT_PATH, aCurrentDir, sizeof ( aCurrentDir ) );
  }

  m_state = cObjPtrState_UNDEFINED;

  m_process_folders = cFALSE;

  if ( PR_SUCC ( error ) && m_dir_stream &&  closedir ( m_dir_stream ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );    
  }

  if (  PR_SUCC ( error ) && p_to_root ) {
    if ( m_path )
      heapFree ( m_path );
    tCHAR aRoot [] = "/";
    error = heapAlloc ( reinterpret_cast <tPTR*> ( &m_path ), sizeof ( aRoot ) );
    if ( PR_SUCC ( error ) )
      strcat ( m_path, aRoot );
  }
    
  if ( PR_SUCC ( error ) && !( m_dir_stream = opendir ( m_path ) ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );
  }

  if ( PR_SUCC ( error ) ) {
    m_state = cObjPtrState_BEFORE;
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::Reset method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// Extended method comment
//    Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameters are:
tERROR ObjPtrData::Clone( hObjPtr* result )
{
  tERROR  error = errOK;
  hObjPtr ret_val = NULL;
  PR_TRACE_A0( this, "Enter ObjPtr::Clone method" );

  // Place your code here
  hOBJECT aParent = sysGetParent ( IID_ANY );
  
  error = aParent->sysCreateObject ( reinterpret_cast <hOBJECT*> ( &ret_val ), IID_OBJPTR, PID_NFIO, SUBTYPE_ANY );
 
  if ( PR_SUCC ( error ) )
    error = ret_val->propSetStr ( 0, pgOBJECT_PATH, m_path );

  if ( PR_SUCC ( error ) && m_mask )
    error = ret_val->propSetStr ( 0, pgMASK, m_mask );

  if ( PR_SUCC ( error ) )
    error = ret_val->propSetBool ( plMASK_APPLIED_TO_FOLDERS, m_apply_mask_to_folders );

  if ( PR_SUCC ( error ) )
    error = ret_val->sysCreateObjectDone ();
  
  if ( PR_SUCC ( error ) && ( m_state != cObjPtrState_BEFORE ) && ( m_state != cObjPtrState_UNDEFINED ) ) {
    tERROR anError = errOK;
    while ( PR_SUCC ( anError ) ) {
      if ( PR_FAIL ( anError = ret_val->Next () ) )
        continue;
      if ( m_state == cObjPtrState_AFTER )
        continue;
      tCHAR aFileName [ MAX_PATH ] = {0};
      if ( PR_FAIL ( anError = ret_val->propGetStr( 0, pgOBJECT_NAME, aFileName, sizeof ( aFileName ) ) ) )
        continue;
      if ( !strcmp ( m_dir_entry.d_name, aFileName ) )
        break;
    }
    if ( PR_FAIL ( anError ) )
      if ( ( m_state != cObjPtrState_AFTER ) || ( anError != errEND_OF_THE_LIST ) )
        error = anError;
  }
  
  if ( PR_FAIL(error) && ret_val  ) {
    ret_val->sysCloseObject ();
    ret_val = 0;
  }

  if ( result )
    *result = ret_val;

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A2( this, "Leave ObjPtr::Clone method, ret hObjPtr = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Behaviour comment
//    First call of Next method after creating or Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR ObjPtrData::Next()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::Next method" );

  // Place your code here
  if ( m_state == cObjPtrState_AFTER )
    error = errEND_OF_THE_LIST;

  if ( m_state == cObjPtrState_UNDEFINED )
    error = errOBJECT_BAD_INTERNAL_STATE;

  while ( PR_SUCC ( error ) ) {
    dirent* aDirEntry = 0;
    if ( readdir_r ( m_dir_stream, &m_dir_entry, &aDirEntry ) || !aDirEntry ) {
      m_state = cObjPtrState_AFTER;
      error = errEND_OF_THE_LIST;
    } 
    if ( ( m_state == cObjPtrState_AFTER ) && 
         !m_process_folders && 
         PR_SUCC ( error = Reset ( cFALSE ) ) ) {
      m_process_folders = cTRUE;
      error = errOK;
      continue;
    }
    if ( PR_FAIL ( error ) )
      break;
    tBOOL anIsFolder = cFALSE;
    if ( PR_FAIL ( error = _isFolder ( m_path, m_dir_entry.d_name, anIsFolder, m_system_error ) ) )
      continue;
    if ( anIsFolder != m_process_folders )
      continue;
    if ( anIsFolder ) {
      if ( ( ( m_dir_entry.d_name [ 0 ] == '.' ) && 
             ( m_dir_entry.d_name [ 1 ] == 0 ) ) ||
           ( ( m_dir_entry.d_name [ 0 ] == '.' ) && 
             ( m_dir_entry.d_name [ 1 ] == '.' ) && 
             ( m_dir_entry.d_name [ 2 ] == 0 ) ) )
        continue;
      if ( !m_mask || ( m_apply_mask_to_folders && !fnmatch ( m_mask, m_dir_entry.d_name, 0 ) ) ) {
        m_state = cObjPtrState_PTR;
        break;
      }
    }
    else 
      if ( !m_mask || !fnmatch ( m_mask, m_dir_entry.d_name, 0 ) ) {
        m_state = cObjPtrState_PTR;
        break;
      }
  }
  
  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::Next method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// Parameters are:
tERROR ObjPtrData::StepUp()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::StepUp method" );

  // Place your code here
  if ( m_state == cObjPtrState_UNDEFINED )
    error = errOBJECT_BAD_INTERNAL_STATE;

  tDWORD aSize = 0;
  tCHAR aCurrentDir [ MAX_PATH ] = {0};

  if ( PR_SUCC ( error ) ) 
    error = _fileName ( m_path, strlen ( m_path ), aCurrentDir, sizeof ( aCurrentDir ), aSize );

  tCHAR aNewDir [ MAX_PATH ] = {0};  
  if ( PR_SUCC ( error ) ) 
    error = _dirName ( m_path, strlen ( m_path ), aNewDir, sizeof ( aNewDir ), aSize );

  if ( PR_SUCC ( error ) )
    // if a new path and an old path is differ ( the old path may be the root directory )
    if ( aSize != ( strlen ( m_path ) + 1 ) ) { 
      strcpy ( m_path, aNewDir );
      error = Reset ( cFALSE );    
    }
    else
      error = errNOT_FOUND;
 
  if ( PR_SUCC ( error ) ) {
    m_process_folders = cTRUE;
    while ( PR_SUCC ( error = Next () ) && strcmp ( m_dir_entry.d_name, aCurrentDir ) );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));  
  PR_TRACE_A1( this, "Leave ObjPtr::StepUp method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR ObjPtrData::StepDown()
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::StepDown method" );

  // Place your code here
  if ( m_state == cObjPtrState_UNDEFINED )
    error = errOBJECT_BAD_INTERNAL_STATE;

  tBOOL anIsFolder = cFALSE;

  // check if not a folder
  if ( PR_SUCC ( error ) &&
       PR_SUCC ( error = _isFolder ( m_path, m_dir_entry.d_name, anIsFolder, m_system_error ) ) &&
       !anIsFolder )
    error = errOBJECT_INCOMPATIBLE;
  
  tSTRING aNewPath;
  if ( PR_SUCC ( error ) )
    error = heapAlloc ( reinterpret_cast <tPTR*> ( &aNewPath ), strlen ( m_path ) + strlen ( m_dir_entry.d_name ) + 2 );
  
  // compose a new path
  if ( PR_SUCC ( error ) ) {
    strcat ( aNewPath, m_path );
    strcat ( aNewPath, "/" );
    strcat ( aNewPath, m_dir_entry.d_name  );
    heapFree ( m_path );
    m_path = aNewPath;
    error = Reset ( cFALSE );
  }

//   if ( m_mask ) {
//     heapFree ( m_mask );
//     m_mask = 0;
//   }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::StepDown method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// Parameters are:
//   "name" : The same as "object" parameter of OS::EnumCreate method
tERROR ObjPtrData::ChangeTo( hOBJECT p_name )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter ObjPtr::ChangeTo method" );

  // Place your code here
  m_system_error = 0;

  tSTRING aNewPath = 0;
  tSTRING aNewMask = 0;

  tIID anIID = p_name->propGetDWord ( pgINTERFACE_ID );
  if ( anIID == IID_STRING ) {
    cString* aString = reinterpret_cast<cString*> ( p_name );
    tDWORD aSize = 0;
    error = aString->LengthEx (  &aSize, STR_RANGE(0, cSTRING_WHOLE_LENGTH), codepage, cSTRING_CONTENT_ONLY );
    if ( PR_SUCC ( error ) ) 
      error = heapAlloc ( reinterpret_cast <tPTR*> ( &aNewPath ), aSize + 1 );
    if ( PR_SUCC ( error ) )
      error = aString->ExportToBuff ( 0, STR_RANGE(0, cSTRING_WHOLE_LENGTH), aNewPath, aSize, codepage, cSTRING_CONTENT_ONLY );
  }
  else  if ( ( anIID == IID_OBJPTR ) || ( anIID == IID_IO ) ) {
    tDWORD aSize = 0;
    p_name->propGetStr ( &aSize, pgOBJECT_PATH, 0, 0 );
    error = heapAlloc ( reinterpret_cast <tPTR*> ( &aNewPath ), aSize + 1 );
    if ( PR_SUCC ( error ) )
      error = p_name->propGetStr ( 0, pgOBJECT_PATH, aNewPath, aSize );
  }

  if ( PR_SUCC ( error ) && ( anIID == IID_OBJPTR ) ) {
    tDWORD aSize = 0;
    p_name->propGetStr ( &aSize, pgMASK, 0, 0 );
    if ( aSize ) {
      error = heapAlloc ( reinterpret_cast <tPTR*> ( &aNewMask ), aSize + 1 );
      if ( PR_SUCC ( error ) )
        error = p_name->propGetStr ( 0, pgOBJECT_PATH, aNewMask, aSize );
    }
  }

  if ( PR_SUCC ( error ) ) {
    m_apply_mask_to_folders = p_name->propGetBool ( plMASK_APPLIED_TO_FOLDERS );
    if ( m_path )
      heapFree ( m_path );
    m_path = aNewPath;
    if ( m_mask )
      heapFree ( m_mask );
    m_mask = aNewMask;
    error = Reset ( cFALSE );
  }

  if ( PR_SUCC ( error ) && ( anIID == IID_OBJPTR ) || ( anIID == IID_IO ) ) {
    tCHAR aCurrentFile [ MAX_PATH ] = {0};
    
    error = p_name->propGetStr ( 0, pgOBJECT_NAME, aCurrentFile, sizeof ( aCurrentFile ) );

    if ( PR_SUCC ( error ) )
      while ( PR_SUCC ( error = Next () ) && strcmp ( m_dir_entry.d_name, aCurrentFile ) );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_objptr\t %s %terr", __FUNCTION__, error ));
  PR_TRACE_A1( this, "Leave ObjPtr::ChangeTo method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ObjPtr". Register function
tERROR ObjPtrData::Register( hROOT root ) 
{
  tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(ObjPtr_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "unix object enumerator", 23 )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, ObjPtrData, m_origin, cPROP_BUFFER_READ_WRITE )
  mpLOCAL_PROPERTY_FN( pgOBJECT_NAME, FN_CUST(getName), NULL )
  mpLOCAL_PROPERTY_FN( pgOBJECT_PATH, FN_CUST(getPath), FN_CUST(setPath) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, FN_CUST(getFullName), NULL )
  mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, ObjPtrData, m_os, cPROP_BUFFER_READ )
  mpLOCAL_PROPERTY_FN( pgIS_FOLDER, FN_CUST(isFolder), NULL )
  mpLOCAL_PROPERTY_FN( pgMASK, FN_CUST(getMask), FN_CUST(setMask) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_PTR_STATE, ObjPtrData, m_state, cPROP_BUFFER_READ )
  mpLOCAL_PROPERTY_FN( pgOBJECT_SIZE, FN_CUST(getFileSize), FN_CUST(setFileSize) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, FN_CUST(getFileSize64), FN_CUST(setFileSize64) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, FN_CUST(getFileAttributes), FN_CUST(setFileAttributes) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_HASH, FN_CUST(isNotImplemented), FN_CUST(isNotImplemented) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_REOPEN_DATA, FN_CUST(isNotImplemented), FN_CUST(isNotImplemented) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, FN_CUST(getFileTime), FN_CUST(setFileTime) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, FN_CUST(getFileTime), FN_CUST(setFileTime) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, FN_CUST(getFileTime), FN_CUST(setFileTime) )
  mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, codepage, sizeof(codepage) )
  mSHARED_PROPERTY_PTR( pgOBJECT_NAME_CP, codepage, sizeof(codepage) )
  mSHARED_PROPERTY_PTR( pgOBJECT_PATH_CP, codepage, sizeof(codepage) )
  mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME_CP, codepage, sizeof(codepage) )
  mpLOCAL_PROPERTY_BUF( pgNATIVE_ERR, ObjPtrData, m_system_error, cPROP_BUFFER_READ )
  mSHARED_PROPERTY_PTR( plMASK_CP, codepage, sizeof(codepage) )
  mpLOCAL_PROPERTY_BUF( plMASK_APPLIED_TO_FOLDERS, ObjPtrData, m_apply_mask_to_folders, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ObjPtr)
  mINTERNAL_METHOD(ObjectInit)
  mINTERNAL_METHOD(ObjectInitDone)
  mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(ObjPtr)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ObjPtr)
  mEXTERNAL_METHOD(ObjPtr, IOCreate)
  mEXTERNAL_METHOD(ObjPtr, Copy)
  mEXTERNAL_METHOD(ObjPtr, Rename)
  mEXTERNAL_METHOD(ObjPtr, Delete)
  mEXTERNAL_METHOD(ObjPtr, Reset)
  mEXTERNAL_METHOD(ObjPtr, Clone)
  mEXTERNAL_METHOD(ObjPtr, Next)
  mEXTERNAL_METHOD(ObjPtr, StepUp)
  mEXTERNAL_METHOD(ObjPtr, StepDown)
  mEXTERNAL_METHOD(ObjPtr, ChangeTo)
mEXTERNAL_TABLE_END(ObjPtr)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

  PR_TRACE_A0( root, "Enter ObjPtr::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_OBJPTR,                             // interface identifier
    PID_NFIO,                               // plugin identifier
    0x00000000,                             // subtype identifier
    ObjPtr_VERSION,                         // interface version
    VID_PETROVITCH,                         // interface developer
    &i_ObjPtr_vtbl,                         // internal(kernel called) function table
    (sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
    &e_ObjPtr_vtbl,                         // external function table
    (sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
    ObjPtr_PropTable,                       // property table
    mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
    sizeof(ObjPtrData)-sizeof(cObjImpl),    // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave ObjPtr::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
  return error;
}

tERROR pr_call ObjPtr_Register( hROOT root ) { return ObjPtrData::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// Property function "ObjPtrData::isNotImplemented" is used as *GET* and *SET* simultaneously
// Property function "ObjPtrData::isNotImplemented" is used as *GET* and *SET* simultaneously
// AVP Prague stamp end



