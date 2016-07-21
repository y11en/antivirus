// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  23 July 2004,  16:24 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Unix native file IO
// Author      -- petrovich
// File Name   -- unix_io.cpp
// Additional info
//    File input/output for Unix
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_nfio.h>
#include <iface/i_root.h>
#include <iface/i_string.h>
#include <iface/i_os.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pr_oid.h>
#include "unix_utils.h"
#include <string.h>
#include <errno.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct IOData : public cIO /*cObjImpl*/
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call getName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setAccessMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call isNotImplemented( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call checkExclusive( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Data declaration
  tSTRING     m_full_name;          // full name buffer
  tINT        m_file_descriptor;    // file descriptor
  tDWORD      m_open_mode;          // open mode
  tDWORD      m_access_mode;        // file attributes
  tBOOL       m_del_on_close;       // must be deleted on close
  cOS*        m_os;                 // owner os
  tDWORD      m_system_error;       // last system error (errno)
  tDWORD      m_origin;             // object origin
  tBOOL       m_restore_attribs;    // file attributes has been changed on InitDone and need to be resore
  tBOOL       m_ignore_ro_attrib;   // --
  tBOOL       m_control_size_on_read; // check and control size on operations
  struct stat m_file_stat;          // initial status of file
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(IOData)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
const tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime
// AVP Prague stamp end

extern tDWORD g_totalWrites;

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR IOData::ObjectInit() 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter IO::ObjectInit method" );

  // Place your code here
  m_file_descriptor= -1;
  m_open_mode = fOMODE_OPEN_IF_EXIST;
  m_access_mode = fACCESS_READ;
  cObject* aOS = static_cast<cObject*> ( sysGetParent ( IID_ANY ) );
  if ( !aOS || (IID_OS != aOS->propGetIID () ) || ( PID_NFIO != aOS->propGetPID () ) )
    m_os = 0;
  else
    m_os = reinterpret_cast <cOS*> ( aOS );
  m_origin =  OID_GENERIC_IO;
  m_ignore_ro_attrib = cFALSE;
  m_control_size_on_read = cTRUE;

  PR_TRACE_A1( this, "Leave IO::ObjectInit method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR IOData::ObjectInitDone() 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter IO::ObjectInitDone method" );

  // Place your code here
  if (!m_full_name)
    error = errOBJECT_NOT_INITIALIZED;

  tBOOL isFolder = cFALSE;
  if ( PR_SUCC ( error ) )
    _isFolder(m_full_name, "", isFolder, m_system_error );

  if ( PR_SUCC ( error ) && isFolder && !(m_open_mode & fOMODE_FOLDER_AS_IO ) )
    error = errPARAMETER_INVALID;

  tCHAR aBuffer [ MAX_PATH ] = {0};
  tDWORD aSize = 0;
  if ( PR_SUCC ( error ) )
    error = _dirName ( m_full_name, strlen ( m_full_name ), aBuffer, sizeof ( aBuffer ), aSize );

  tCHAR anAbsolutePath [ MAX_PATH ] = {0};
  if ( PR_SUCC ( error ) )
    error = _absolutePath ( aBuffer, aSize, anAbsolutePath, sizeof ( anAbsolutePath ) );    

  if ( PR_SUCC ( error ) ) 
    error = _fileName ( m_full_name, strlen ( m_full_name ), aBuffer, sizeof ( aBuffer ), aSize );

  tCHAR* aNewFullName = 0;
  if ( PR_SUCC ( error ) && 
       PR_SUCC ( error = heapAlloc ( reinterpret_cast<tPTR*> ( &aNewFullName ), 
                                     strlen ( anAbsolutePath ) + strlen ( aBuffer ) + 2 ) ) ) {
      // copy a new file name to the full file name
      strcat ( aNewFullName, anAbsolutePath );
      strcat ( aNewFullName, "/" );
      strcat ( aNewFullName, aBuffer );
      // if the full file name is not empty
      if ( m_full_name )
        heapFree ( m_full_name );
      m_full_name = aNewFullName;
  }

  m_system_error = 0;

  if ( PR_SUCC ( error ) ) {
    int  aFlags = 0;
    if ( m_open_mode & fOMODE_OPEN_IF_EXIST )
      ;//aFlags = O_CREAT | O_EXCL;
    if ( m_open_mode & fOMODE_CREATE_IF_NOT_EXIST )
      aFlags = O_CREAT | O_EXCL;
    if ( m_open_mode & fOMODE_TRUNCATE )
      aFlags |= O_TRUNC;
    if ( ( m_open_mode & mOMODE_CREATION ) == fOMODE_OPEN_ALWAYS )
      aFlags = O_CREAT;
    if ( ( m_open_mode & mOMODE_CREATION ) == fOMODE_CREATE_ALWAYS )
      aFlags = O_CREAT | O_TRUNC;

    int anAccess = 0;
    if ( (m_access_mode & fACCESS_READ) == fACCESS_READ ) 
      anAccess = O_RDONLY;
    if ( (m_access_mode & fACCESS_WRITE) == fACCESS_WRITE ) 
      anAccess = O_WRONLY;
    if ( (m_access_mode & fACCESS_RW) == fACCESS_RW ) 
      anAccess = O_RDWR;
    aFlags |= anAccess;

    mode_t aMode = S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP;

    if ( m_ignore_ro_attrib && access ( m_full_name, W_OK ) && ( errno == EACCES ) ) {
      if ( PR_SUCC ( error ) && chmod ( m_full_name, S_IWUSR ) ) {
        m_system_error = errno;
        error = systemErrors2PragueErrors ( m_system_error );
      }
      if ( PR_SUCC ( error ) )
        m_restore_attribs = cTRUE;
    }

    m_file_descriptor = open ( m_full_name, aFlags, ( aFlags & O_CREAT ) ? aMode : 0 );
    
    if ( m_file_descriptor == -1 ) {
      m_system_error = errno;
      error = systemErrors2PragueErrors ( m_system_error );
    }

    if ( PR_SUCC ( error ) && fstat ( m_file_descriptor, &m_file_stat ) ) {
      m_system_error = errno;
      error = systemErrors2PragueErrors ( m_system_error );
    }

  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A1( this, "Leave IO::ObjectInitDone method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR IOData::ObjectClose() 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter IO::ObjectClose method" );

  // Place your code here
  m_system_error = 0;

  m_del_on_close |= ( m_open_mode & fOMODE_TEMPORARY );// || ( ( m_open_mode & mOMODE_CREATION ) ==  fOMODE_CREATE_ALWAYS );

  if ( ( m_file_descriptor != -1 ) && !m_del_on_close && m_restore_attribs ) {
    // restore attributes 
    if ( PR_SUCC ( error ) && fchmod ( m_file_descriptor, m_file_stat.st_mode ) ) {
      m_system_error = errno;
      error = systemErrors2PragueErrors ( m_system_error );
    }
  }

  if ( m_file_descriptor != -1 ) {
    close ( m_file_descriptor );
    m_file_descriptor = -1;
  }

  if ( PR_SUCC ( error ) && ( m_del_on_close || ( m_open_mode & fOMODE_TEMPORARY ) ) && m_full_name && unlink ( m_full_name ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  if ( m_full_name )
    heapFree ( m_full_name ); 

  PR_TRACE_A1( this, "Leave IO::ObjectClose method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR IOData::getName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method IO::getName for property pgOBJECT_NAME" );
  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  if ( size && !buffer )
    error = errPARAMETER_INVALID;
  
  if ( PR_SUCC ( error ) && m_full_name ) 
    error = _fileName ( m_full_name, strlen ( m_full_name ), buffer, size, *l_res_ptr );
  PR_TRACE_A2( this, "Leave *GET* method IO::getName for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setName )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_NAME
tERROR IOData::setName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method IO::setName for property pgOBJECT_NAME" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  if ( !buffer || !size )
    error =  errPARAMETER_INVALID;
  
  tDWORD aSize = 0;
  if ( m_full_name )
    aSize = strlen ( m_full_name );
  tCHAR* aNewFullFileName = 0;
  if ( PR_SUCC ( error ) && 
       PR_SUCC ( error = heapAlloc ( reinterpret_cast<tPTR*> ( &aNewFullFileName ), 
                                     aSize + size + 2 ) ) ) {
    if ( m_full_name ) {
      strcat ( aNewFullFileName, m_full_name );
      strcat ( aNewFullFileName, "/" );    
    }
    strcat ( aNewFullFileName, buffer );
    if ( m_full_name  ) 
      // free the old block of memory 
      heapFree ( m_full_name );
    // reassing pointer to the full file name
    m_full_name = aNewFullFileName;
  }

  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave *SET* method IO::setName for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getPath )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR IOData::getPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method IO::getPath for property pgOBJECT_PATH" );
  
  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  if ( size && !buffer )
    error = errPARAMETER_INVALID;
  
  if ( PR_SUCC ( error ) && m_full_name ) 
    error = _dirName ( m_full_name, strlen ( m_full_name ), buffer, size, *l_res_ptr );
  PR_TRACE_A2( this, "Leave *GET* method IO::getPath for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setPath )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR IOData::setPath( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method IO::setPath for property pgOBJECT_PATH" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  if ( !buffer || !size )
    error = errPARAMETER_INVALID;
  
  tDWORD aSize = 0;
  if ( m_full_name )
    aSize = strlen ( m_full_name );
  tCHAR* aNewFullFileName = 0;
  if ( PR_SUCC ( error ) && 
       PR_SUCC ( error = heapAlloc ( reinterpret_cast<tPTR*> ( &aNewFullFileName ), 
                                     aSize + size + 2 ) ) ) {
    strcat ( aNewFullFileName, buffer );
    if ( m_full_name ) {
      strcat ( aNewFullFileName, "/" );    
      strcat ( aNewFullFileName, m_full_name );
    }
    if ( m_full_name  ) 
      // free the old block of memory 
      heapFree ( m_full_name );
    // reassing pointer to the full file name
    m_full_name = aNewFullFileName;
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave *SET* method IO::setPath for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFullName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR IOData::getFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgOBJECT_FULL_NAME" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  if ( m_full_name ) {
    *l_res_ptr = strlen ( m_full_name ) + 1;
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr )
      error = errBUFFER_TOO_SMALL;
    else
      strncpy ( buffer, m_full_name, *l_res_ptr );
  }

  PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFullName )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_FULL_NAME
tERROR IOData::setFullName( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method ObjectClose for property pgOBJECT_FULL_NAME" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;

  if ( !buffer || !size )
    error = errPARAMETER_INVALID;

  tCHAR* aNewFullName = 0;
  if ( PR_SUCC ( error ) && 
       PR_SUCC ( error = heapAlloc ( reinterpret_cast<tPTR*> ( &aNewFullName ), 
                                     size + 1 ) ) ) {
      // copy a new file name to the full file name
      strncpy ( aNewFullName, buffer, size );
	  aNewFullName[size] = 0; // just a precaution
	  *l_res_ptr = size; // Need to return amount of bytes set
      // if the full file name is not empty
      if ( m_full_name )
        heapFree ( m_full_name );
      m_full_name = aNewFullName;
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave *SET* method ObjectClose for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setAccessMode )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR IOData::setAccessMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method IO::setAccessMode for property pgOBJECT_ACCESS_MODE" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  // if the buffer is empty or the size is empty, return error
  if ( !buffer || !size || size < sizeof ( tDWORD ) ) 
    error = errPARAMETER_INVALID;

  tBOOL aDonePerformed = propIsOperational();

  // if ObjectInitDone has been performed and fACCESS_NO_CHANGE_MODE is set, return error
  if ( aDonePerformed && (m_access_mode & fACCESS_NO_CHANGE_MODE) ) {
    size = 0;
    error = errACCESS_DENIED;
  }

  if ( PR_SUCC ( error ) ) {
    tDWORD aNewMode = *( reinterpret_cast<tDWORD*> ( buffer ) ) & mACCESS;
    // if old access mode and new access mode are not the same
    if ( m_access_mode != aNewMode ) {
      *l_res_ptr = sizeof(m_access_mode);
      // if ObjectInitDone hasn't been performed or read/write access is not changed, just set new access mode
      if ( !aDonePerformed || 
           ( m_access_mode & fACCESS_RW ) == ( aNewMode & fACCESS_RW ) )
        m_access_mode = aNewMode;
      else {
        // else try to backup old mode, set new mode  and perform ObjectInitDone
        tINT anOldFileDescriptor  = m_file_descriptor;
        tDWORD anOldMode = m_access_mode;
        m_access_mode = aNewMode;
        // if ObjectInitDone is failed
        if ( PR_FAIL ( error = ObjectInitDone() ) ) {
          // restore old file descriptor and access mode
          m_access_mode = anOldMode;
          m_file_descriptor = anOldFileDescriptor;
        }
        else 
          // close old file descriptor
          close ( anOldFileDescriptor );
      }
    }
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave *SET* method IO::setAccessMode for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, isNotImplemented )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_HASH
//  -- OBJECT_REOPEN_DATA
//  -- EXTENDED_NAME_PROCESSING
tERROR IOData::isNotImplemented( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error =  errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter *GET* multyproperty method IO::isNotImplemented" );

  PR_TRACE_A2( this, "Leave *GET* multyproperty method IO::isNotImplemented, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFileAttributes )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR IOData::getFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method IO::getFileAttributes for property pgOBJECT_ATTRIBUTES" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  if ( size && !buffer )
    error = errPARAMETER_INVALID;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) ) {
    *l_res_ptr = sizeof ( tDWORD );
    if ( !size )
      error = errOK;
    else if ( size < *l_res_ptr ) 
      error = errBUFFER_TOO_SMALL;
    else {
      tDWORD* aResult = reinterpret_cast<tDWORD*>(buffer);
      error = _getFileAttributes ( m_full_name, *aResult, m_system_error );
    }
  }

  PR_TRACE_A2( this, "Leave *GET* method IO::getFileAttributes for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFileAttributes )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR IOData::setFileAttributes( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method IO::setFileAttributes for property pgOBJECT_ATTRIBUTES" );

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = 0;
  m_system_error = 0;

  if ( !buffer || !size || size < sizeof ( tDWORD ) )  
    error = errPARAMETER_INVALID;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) ) {
    tDWORD aFileAttributes = *(reinterpret_cast<tDWORD*> (buffer));
    error = _setFileAttributes ( m_full_name, aFileAttributes, m_system_error);
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave *SET* method IO::setFileAttributes for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, getFileTime )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR IOData::getFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* multyproperty method IO::getFileTime" );

  m_system_error = 0;

  if ( size && !buffer )
    error = errPARAMETER_INVALID;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;
  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = sizeof(tDATETIME);

  if ( !size ) 
    error = errOK;
  else if ( size < sizeof( tDATETIME ) )
    error = errBUFFER_TOO_SMALL;
  else {
    tDATETIME* aDateTime = reinterpret_cast <tDATETIME*> ( buffer );
    error = _getFileTime ( m_full_name, prop, *aDateTime, m_system_error );
  }

  PR_TRACE_A2( this, "Leave *GET* multyproperty method IO::getFileTime, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, setFileTime )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR IOData::setFileTime( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* multyproperty method IO::setFileTime" );

  m_system_error = 0;

 if ( !buffer || !size || size < sizeof ( tDATETIME ) )  
    error = errPARAMETER_INVALID;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  tDWORD l_result = 0;
  tDWORD * l_res_ptr = &l_result;
  if ( out_size )
    l_res_ptr = out_size;
  *l_res_ptr = sizeof(tDATETIME);

  if ( PR_SUCC ( error ) ) {
    tDATETIME* aDateTime = reinterpret_cast <tDATETIME*> ( buffer );
    error = _setFileTime ( m_full_name, prop, *aDateTime, m_system_error );
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave *SET* multyproperty method IO::setFileTime, ret tDWORD = %u(size), %terr", *l_res_ptr, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, checkExclusive )
// Interface "IO". Method "Get" for property(s):
//  -- EXCLUSIVE_ACCESS
tERROR IOData::checkExclusive( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *GET* method IO::checkExclusive for property plEXCLUSIVE_ACCESS" );

  *out_size = 0;
  m_system_error = 0;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  PR_TRACE_A2( this, "Leave *GET* method IO::checkExclusive for property plEXCLUSIVE_ACCESS, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR IOData::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size ) 
{
  tERROR error = errOK;
  tDWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter IO::SeekRead method" );

  // Place your code here
  m_system_error = 0;

  if ( !p_buffer || !p_size ) 
    error = errPARAMETER_INVALID;
  
  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) && m_control_size_on_read ) {
    struct stat aFileStat;
    if ( fstat ( m_file_descriptor, &aFileStat ) ) {
      m_system_error = errno;
      error = systemErrors2PragueErrors ( m_system_error );   
    }
    if ( PR_SUCC ( error ) && ( ( aFileStat.st_size - p_offset ) < 0 ) )
      error = errOUT_OF_OBJECT;
    if ( PR_SUCC ( error ) && ( ( aFileStat.st_size  - p_offset - p_size ) < 0 ))
      error = errEOF;
  }

  if ( PR_SUCC ( error ) && ( lseek ( m_file_descriptor, p_offset, SEEK_SET ) == static_cast<off_t> ( -1 ) ) ) {
    m_system_error = errno;
    error = errOBJECT_SEEK;
  }

  if ( PR_SUCC ( error ) && ( ( ret_val = read ( m_file_descriptor, p_buffer, p_size ) ) == static_cast<size_t> (-1) ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );
    ret_val = 0;
  }  

  if ( PR_SUCC ( error ) && !ret_val )
    error = errEOF;

  if ( result )
    *result = ret_val;
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s error %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave IO::SeekRead method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR IOData::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size ) 
{
  tERROR error = errOK;
  tDWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter IO::SeekWrite method" );

  // Place your code here
  m_system_error = 0;

  if ( !p_buffer || !p_size )
    error = errPARAMETER_INVALID;

  if ( !(m_access_mode & fACCESS_WRITE ) )
    error = errOBJECT_READ_ONLY;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  struct stat aFileStat;
  if ( PR_SUCC ( error ) &&
       ( fstat ( m_file_descriptor, &aFileStat ) ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );   
  }
  
  // if end of the file is reached and extention of the file is not allowed
  if ( PR_SUCC ( error ) && 
       ( ( p_offset - aFileStat.st_size ) == 0 ) &&
       ( m_access_mode & ( fACCESS_NO_EXTEND ) ) )
    error = errOBJECT_RESIZE;

  if ( PR_SUCC ( error ) && 
       ( ( p_offset - aFileStat.st_size ) > 0 ) && 
       ( m_access_mode & ( fACCESS_NO_EXTEND|fACCESS_NO_EXTEND_RAND ) ) )
    error = errOBJECT_RESIZE;

  if ( PR_SUCC ( error ) && ( m_access_mode & ( fACCESS_NO_EXTEND|fACCESS_NO_EXTEND_RAND ) ) )
    p_size = ( p_size < ( aFileStat.st_size - p_offset ) ) ? p_size : ( aFileStat.st_size - p_offset );

  if ( PR_SUCC ( error ) && ( lseek ( m_file_descriptor, p_offset , SEEK_SET) == static_cast<off_t> ( -1 ) )) {
    m_system_error = errno;
    error = errOBJECT_SEEK;
  }

  if ( PR_SUCC ( error ) && ( ( ret_val = write ( m_file_descriptor, p_buffer, p_size ) ) == static_cast<size_t> (-1) ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );
    ret_val = 0;
  }  
  
  g_totalWrites += ret_val;

  if ( result )
    *result = ret_val;
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s error %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A2( this, "Leave IO::SeekWrite method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR IOData::GetSize( tQWORD* result, IO_SIZE_TYPE p_type ) 
{
  tERROR error = errOK;
  tQWORD ret_val = 0;
  PR_TRACE_A0( this, "Enter IO::GetSize method" );

  // Place your code here
  m_system_error = 0;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;


  if ( PR_SUCC ( error ) ) 
    error = _getFileSize ( m_full_name, ret_val, m_system_error );

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave IO::GetSize method, ret tQWORD = %I64u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR IOData::SetSize( tQWORD p_new_size ) 
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter IO::SetSize method" );

  // Place your code here
  m_system_error = 0;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  struct stat aFileStat;
  if ( PR_SUCC ( error ) &&
       ( fstat ( m_file_descriptor, &aFileStat ) ) ) {
    m_system_error = errno;
    error = systemErrors2PragueErrors ( m_system_error );   
  }
  
  tDWORD anAvailableFlags = propGetDWord ( pgOBJECT_AVAILABILITY );
  if ( PR_SUCC ( error ) && 
       ( aFileStat.st_size - p_new_size > 0 )  &&
       ( ( m_access_mode & fACCESS_NO_TRUNCATE ) ||
         !( anAvailableFlags & fAVAIL_TRUNCATE ) ) )
    error = errOBJECT_RESIZE;

  if ( PR_SUCC ( error ) && 
       ( aFileStat.st_size - p_new_size > 0 )  &&
       ( ( m_access_mode & ( fACCESS_NO_EXTEND|fACCESS_NO_EXTEND_RAND ) ) ||
         !( anAvailableFlags & ( fAVAIL_EXTEND_SEQ | fAVAIL_EXTEND_RAND ) ) ) )
    error = errOBJECT_RESIZE;


  if ( PR_SUCC ( error ) ) 
    error = _setFileSize ( m_full_name, p_new_size, m_system_error );

  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A1( this, "Leave IO::SetSize method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR IOData::Flush() 
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter IO::Flush method" );

  // Place your code here
  m_system_error = 0;

  if ( m_file_descriptor == -1 ) 
    error = errOBJECT_NOT_INITIALIZED;

  if ( PR_SUCC ( error ) ) {
    if ( fsync ( m_file_descriptor ) ) {
      m_system_error = errno;
      error = systemErrors2PragueErrors ( m_system_error );
    }
  }
  PR_TRACE ((this, prtNOTIFY, "nfio_io\t%s: file %s %terr", __FUNCTION__, m_full_name, error ));
  PR_TRACE_A1( this, "Leave IO::Flush method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IO". Register function
tERROR IOData::Register( hROOT root ) 
{
  tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IO_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "unix input/output interface", 28 )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, IOData, m_origin, cPROP_BUFFER_READ_WRITE )
  mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(getName), FN_CUST(setName) )
  mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(getPath), FN_CUST(setPath) )
  mpLOCAL_PROPERTY( pgOBJECT_FULL_NAME, IOData, m_full_name, cPROP_BUFFER_READ | cPROP_WRITABLE_ON_INIT, FN_CUST(getFullName), FN_CUST(setFullName) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, IOData, m_open_mode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
  mpLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, IOData, m_access_mode, cPROP_BUFFER_READ, NULL, FN_CUST(setAccessMode) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, IOData, m_del_on_close, cPROP_BUFFER_READ_WRITE )
  mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
  mpLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, IOData, m_os, cPROP_BUFFER_READ )
  mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_HASH, FN_CUST(isNotImplemented), FN_CUST(isNotImplemented) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_REOPEN_DATA, FN_CUST(isNotImplemented), FN_CUST(isNotImplemented) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, FN_CUST(getFileAttributes), FN_CUST(setFileAttributes) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, FN_CUST(getFileTime), FN_CUST(setFileTime) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, FN_CUST(getFileTime), FN_CUST(setFileTime) )
  mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, FN_CUST(getFileTime), FN_CUST(setFileTime) )
  mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, codepage, sizeof(codepage) )
  mSHARED_PROPERTY_PTR( pgOBJECT_NAME_CP, codepage, sizeof(codepage) )
  mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME_CP, codepage, sizeof(codepage) )
  mpLOCAL_PROPERTY_BUF( pgNATIVE_ERR, IOData, m_system_error, cPROP_BUFFER_READ )
  mpLOCAL_PROPERTY_BUF( plNATIVE_HANDLE, IOData, m_file_descriptor, cPROP_BUFFER_READ )
  mpLOCAL_PROPERTY_FN( plEXCLUSIVE_ACCESS, FN_CUST(checkExclusive), NULL )
  mpLOCAL_PROPERTY_BUF( plRESTORE_ATTRIBUTES, IOData, m_restore_attribs, cPROP_BUFFER_READ_WRITE )
  mpLOCAL_PROPERTY_BUF( plIGNORE_RO_ATTRIBUTE, IOData, m_ignore_ro_attrib, cPROP_BUFFER_READ_WRITE )
  mpLOCAL_PROPERTY_BUF( plCONTROL_SIZE_ON_READ, IOData, m_control_size_on_read, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
  mpLOCAL_PROPERTY_FN( plEXTENDED_NAME_PROCESSING, FN_CUST(isNotImplemented), FN_CUST(isNotImplemented) )
mpPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IO)
  mINTERNAL_METHOD(ObjectInit)
  mINTERNAL_METHOD(ObjectInitDone)
  mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(IO)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(IO)
  mEXTERNAL_METHOD(IO, SeekRead)
  mEXTERNAL_METHOD(IO, SeekWrite)
  mEXTERNAL_METHOD(IO, GetSize)
  mEXTERNAL_METHOD(IO, SetSize)
  mEXTERNAL_METHOD(IO, Flush)
mEXTERNAL_TABLE_END(IO)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

  PR_TRACE_A0( root, "Enter IO::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_IO,                                 // interface identifier
    PID_NFIO,                               // plugin identifier
    0x00000000,                             // subtype identifier
    IO_VERSION,                             // interface version
    VID_PETROVITCH,                         // interface developer
    &i_IO_vtbl,                             // internal(kernel called) function table
    (sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
    &e_IO_vtbl,                             // external function table
    (sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
    IO_PropTable,                           // property table
    mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
    sizeof(IOData)-sizeof(cObjImpl),        // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave IO::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
  return error;
}

tERROR pr_call IO_Register( hROOT root ) { return IOData::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// Property function "IOData::isNotImplemented" is used as *GET* and *SET* simultaneously
// Property function "IOData::isNotImplemented" is used as *GET* and *SET* simultaneously
// Property function "IOData::isNotImplemented" is used as *GET* and *SET* simultaneously
// AVP Prague stamp end



