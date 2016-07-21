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
// File Name   -- io.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_unixdiskio.h>
#include <iface/i_root.h>
#include <iface/i_string.h>
#include <pr_oid.h>
#include "diskio.h"
#include <iface/i_heap.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
// AVP Prague stamp end



#define IO_This		_this->data


// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_Private {
  tDWORD       m_OpenMode;                 // --
  tDWORD       m_AccessMode;               // --
  tDWORD       m_Availability;             // --
  DiskGeometry m_DiskGeometry;             // --
  tORIG_ID     m_Origin;                   // --
  hOBJECT      m_BasedOn;                  // --
  tCHAR        m_FullObjectName[MAX_PATH]; // --
  tDWORD       m_DriveType;                // --
  tDWORD       m_MediaType;                // --
} Private;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO {
  const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
  const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
  Private*           data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call IO_ObjectInit( hi_IO _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter IO::ObjectInit method" );

  // Place your code here
  error = errOK;
  IO_This->m_Origin = 0;
  PR_TRACE_A1( _this, "Leave IO::ObjectInit method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IO_ObjectInitDone( hi_IO _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter IO::ObjectInitDone method" );

  // Place your code here
  error = errOK;
  if ( IO_This->m_Origin == OID_PHYSICAL_DISK )
    getDevicePhysicalName ( IO_This->m_FullObjectName, IO_This->m_FullObjectName, sizeof ( IO_This->m_FullObjectName ) );
  if ( ( IO_This->m_Origin == OID_LOGICAL_DRIVE ) &&
       !getLogicalDiskByPhysicalDevice ( IO_This->m_FullObjectName, IO_This->m_FullObjectName, sizeof ( IO_This->m_FullObjectName ) ) )
    error = errOBJECT_CANNOT_BE_INITIALIZED;
  if ( error == errOK ) {
    if ( getDeviceGeometry ( IO_This->m_FullObjectName, &IO_This->m_DiskGeometry ) ) {
      switch ( getDeviceType ( IO_This->m_FullObjectName ) ) {
      case dtFloppy:
        if ( !IO_This->m_Origin )
          IO_This->m_Origin = OID_LOGICAL_DRIVE;
        IO_This->m_DriveType = cFILE_DEVICE_DISK;
        IO_This->m_MediaType = cMEDIA_TYPE_REMOVABLE;
        switch ( getDeviceMode ( IO_This->m_FullObjectName ) ) {
        case dmReadWrite:
          IO_This->m_AccessMode = fACCESS_RW |
            fACCESS_NO_EXTEND_RAND |
            fACCESS_NO_EXTEND |
            fACCESS_NO_TRUNCATE |
            fACCESS_NO_BUFFERING_READ |
            fACCESS_NO_BUFFERING_WRITE |
            fACCESS_NO_BUFFERING;
          IO_This->m_Availability = fAVAIL_READ | fAVAIL_WRITE;
          break;
        case dmReadOnly:
          IO_This->m_AccessMode = fACCESS_READ |
            fACCESS_NO_EXTEND_RAND |
            fACCESS_NO_EXTEND |
            fACCESS_NO_TRUNCATE |
            fACCESS_NO_BUFFERING_READ |
            fACCESS_NO_BUFFERING_WRITE |
            fACCESS_NO_BUFFERING |
            fACCESS_NO_CHANGE_MODE;
          IO_This->m_Availability = fAVAIL_READ;
          break;
        default:
          error = errOBJECT_CANNOT_BE_INITIALIZED;
          break;
        }
        break;
      case dtCDROM:
        if ( !IO_This->m_Origin )
          IO_This->m_Origin = OID_LOGICAL_DRIVE;
        IO_This->m_DriveType = cFILE_DEVICE_CD_ROM;
        IO_This->m_MediaType = cMEDIA_TYPE_REMOVABLE;
        IO_This->m_AccessMode = fACCESS_READ|
          fACCESS_NO_EXTEND_RAND |
          fACCESS_NO_EXTEND |
          fACCESS_NO_TRUNCATE |
          fACCESS_NO_BUFFERING_READ |
          fACCESS_NO_BUFFERING_WRITE |
          fACCESS_NO_BUFFERING |
          fACCESS_NO_CHANGE_MODE;
        IO_This->m_Availability = fAVAIL_READ;
        break;
      case dtPhysicalDevice:
        IO_This->m_Origin = OID_PHYSICAL_DISK;
        IO_This->m_DriveType = cFILE_DEVICE_DISK;
        IO_This->m_MediaType = cMEDIA_TYPE_FIXED;
        IO_This->m_AccessMode = fACCESS_RW |
          fACCESS_NO_EXTEND_RAND |
          fACCESS_NO_EXTEND |
          fACCESS_NO_TRUNCATE |
          fACCESS_NO_BUFFERING_READ |
          fACCESS_NO_BUFFERING_WRITE |
          fACCESS_NO_BUFFERING;
        IO_This->m_Availability = fAVAIL_READ | fAVAIL_WRITE;
        break;
      case dtLogicalDisk:
        IO_This->m_Origin = OID_LOGICAL_DRIVE;
        IO_This->m_DriveType = cFILE_DEVICE_DISK;
        IO_This->m_MediaType = cMEDIA_TYPE_FIXED;
        IO_This->m_AccessMode = fACCESS_RW |
          fACCESS_NO_EXTEND_RAND |
          fACCESS_NO_EXTEND |
          fACCESS_NO_TRUNCATE |
          fACCESS_NO_BUFFERING_READ |
          fACCESS_NO_BUFFERING_WRITE |
          fACCESS_NO_BUFFERING;
        IO_This->m_Availability = fAVAIL_READ | fAVAIL_WRITE;
        break;
      default:
        error = errOBJECT_CANNOT_BE_INITIALIZED;
        break;
      }
    }
    else
      error = errOBJECT_CANNOT_BE_INITIALIZED;
  }
  PR_TRACE_A1( _this, "Leave IO::ObjectInitDone method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DiskPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_ACCESS_MODE
//  -- PartitionType
//  -- BootIndicator
//  -- RecognizedPartition
//  -- Cylinders
//  -- TracksPerCylinder
//  -- SectorsPerTrack
//  -- BytesPerSector
tERROR pr_call IO_PROP_DiskPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  char aBuffer [MAX_PATH];
  char* aPtr = 0;
  PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_DiskPropRead" );

  switch ( prop ) {

  case pgOBJECT_NAME:
    strncpy ( aBuffer, IO_This->m_FullObjectName, sizeof ( aBuffer ) );
    aPtr = basename ( aBuffer );
    *out_size = strlen ( aPtr ) + 1;
    if ( buffer != NULL ) {
      if ( *out_size > size )
        error = errBUFFER_TOO_SMALL;
      else
        strncpy ( buffer, aPtr, *out_size );
    }
    break;

  case pgOBJECT_PATH:
    strncpy ( aBuffer, IO_This->m_FullObjectName, sizeof ( aBuffer ) );
    aPtr =  dirname ( aBuffer );
    *out_size = strlen ( aPtr ) + 1;
    if ( buffer != NULL ) {
      if ( *out_size > size )
        error = errBUFFER_TOO_SMALL;
      else
        strncpy ( buffer, aPtr, *out_size );
    }
    break;

    case pgOBJECT_ACCESS_MODE:
      *out_size = sizeof ( tDWORD );
      if ( buffer != NULL ) {
        if ( size < *out_size )
          error = errBUFFER_TOO_SMALL;
        else
          *(tDWORD*) buffer = IO_This->m_AccessMode;
      }
      break;

    case plPartitionType:
      error = errNOT_IMPLEMENTED;
      *out_size = 0;
      break;

    case plBootIndicator:
      error = errNOT_IMPLEMENTED;
      *out_size = 0;
      break;

    case plRecognizedPartition:
      error = errNOT_IMPLEMENTED;
      *out_size = 0;
      break;

    case plCylinders:
      *out_size = sizeof ( tDWORD );
      if ( buffer != NULL ) {
        if ( size < *out_size )
          error = errBUFFER_TOO_SMALL;
        else
          *(tDWORD*) buffer = IO_This->m_DiskGeometry.m_Cylinders;
      }
      break;

    case plTracksPerCylinder:
      *out_size = sizeof ( tDWORD );
      if ( buffer != NULL ) {
        if ( size < *out_size )
          error = errBUFFER_TOO_SMALL;
        else
          *(tDWORD*) buffer = IO_This->m_DiskGeometry.m_Heads;
      }
      break;

    case plSectorsPerTrack:
      *out_size = sizeof ( tDWORD );
      if ( buffer != NULL ) {
        if ( size < *out_size )
          error = errBUFFER_TOO_SMALL;
        else
          *(tDWORD*) buffer = IO_This->m_DiskGeometry.m_Sectors;
      }
      break;

    case plBytesPerSector:
      *out_size = sizeof ( tDWORD );
      if ( buffer != NULL ) {
        if ( size < *out_size )
          error = errBUFFER_TOO_SMALL;
        else
          *(tDWORD*) buffer = IO_This->m_DiskGeometry.m_SectorSize;
      }
      break;

    case plIsPartition:
      *out_size = sizeof ( tBOOL );
      if ( buffer != NULL ) {
        if ( size < *out_size )
          error = errBUFFER_TOO_SMALL;
        else {
          *(tBOOL*) buffer = getDeviceType ( IO_This->m_FullObjectName ) == dtPhysicalDevice;
        }
      }
      break;
      
    default:
      *out_size = 0;
      break;
  }

  PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_DiskPropRead, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DiskPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_DiskPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_DiskPropWrite for property pgOBJECT_ACCESS_MODE" );

  switch ( prop ) {

    case pgOBJECT_ACCESS_MODE:
/*       *out_size = sizeof ( tDWORD ); */
/*       if ( buffer != NULL ) { */
/*         if ( size < *out_size ) */
/*           error = errBUFFER_TOO_SMALL; */
/*         else  */
/*           *(tDWORD*) buffer = IO_This->m_AccessMode; */
/*       } */
      error = errNOT_IMPLEMENTED;
      break;

     default:
      *out_size = 0;
      break;
  }

  PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_DiskPropWrite for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
  tERROR error = errNOT_IMPLEMENTED;
  tDWORD ret_val = 0;
  int aFD = 0;
  PR_TRACE_A0( _this, "Enter IO::SeekRead method" );

  // Place your code here
  if (buffer == NULL) {
    ret_val = size;
  }
  else {
    aFD = open ( IO_This->m_FullObjectName, O_RDONLY );
    if ( aFD == -1 )
        error = errUNEXPECTED;
    else {
      if ( lseek ( aFD, offset, SEEK_SET ) != -1 ) {
        ret_val =  read ( aFD, buffer, size );

        if ( ret_val >= 0 )
          error = errOK;
        else
          error = errOBJECT_READ;
      }
      else
        error = errOBJECT_READ;
      close ( aFD );
    }

  }
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave IO::SeekRead method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
  tERROR error = errNOT_IMPLEMENTED;
  tDWORD ret_val = 0;
  int aFD;
  PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );

  if (!(IO_This->m_AccessMode & fACCESS_RW))
    error = errACCESS_DENIED;
  else {
    aFD = open ( IO_This->m_FullObjectName, O_RDWR );
    if ( aFD == -1 ) {
      if ( errno == EROFS )
        error = errACCESS_DENIED;
      else
        error = errUNEXPECTED;
    }
    else {
      if ( lseek ( aFD, offset, SEEK_SET ) != -1 ) {
        ret_val =  write ( aFD, buffer, size );

        if ( ret_val >= 0 ) {
          fsync ( aFD );
          error = errOK;
        }
        else
          error = errOBJECT_WRITE;
      }
      else
        error = errOBJECT_WRITE;
      close ( aFD );
    }

  }

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave IO::SeekWrite method, ret tDWORD = %u, %terr", ret_val, error );
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
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type )
{
  tERROR error = errNOT_IMPLEMENTED;
  tQWORD ret_val = 0;
  PR_TRACE_A0( _this, "Enter IO::GetSize method" );

  // Place your code here
  ret_val = IO_This->m_DiskGeometry.m_Cylinders *
    IO_This->m_DiskGeometry.m_Heads *
    IO_This->m_DiskGeometry.m_Sectors *
    IO_This->m_DiskGeometry.m_SectorSize;

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave IO::GetSize method, ret tQWORD = %I64u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter IO::SetSize method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave IO::SetSize method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call IO_Flush( hi_IO _this )
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter IO::Flush method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave IO::Flush method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Low level disk I/O for Unix-like systems", 41 )
  mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, Private, m_Origin, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
  mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, Private, m_FullObjectName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
  mSHARED_PROPERTY( pgOBJECT_OPEN_MODE, ((tDWORD)(fOMODE_OPEN_IF_EXIST)) )
  mLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, IO_PROP_DiskPropRead, IO_PROP_DiskPropWrite )
  mSHARED_PROPERTY( pgOBJECT_DELETE_ON_CLOSE, ((tBOOL)(cFALSE)) )
  mLOCAL_PROPERTY_BUF( pgOBJECT_AVAILABILITY, Private, m_Availability, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, Private, m_BasedOn, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_FN( plPartitionType, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( plBootIndicator, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( plRecognizedPartition, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( plCylinders, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( plTracksPerCylinder, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( plSectorsPerTrack, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_FN( plBytesPerSector, IO_PROP_DiskPropRead, NULL )
  mLOCAL_PROPERTY_BUF( plMediaType, Private, m_MediaType, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_BUF( plDriveType, Private, m_DriveType, cPROP_BUFFER_READ )
  mLOCAL_PROPERTY_FN( plIsPartition, IO_PROP_DiskPropRead, NULL )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = {
  (tIntFnRecognize)         NULL,
  (tIntFnObjectNew)         NULL,
  (tIntFnObjectInit)        IO_ObjectInit,
  (tIntFnObjectInitDone)    IO_ObjectInitDone,
  (tIntFnObjectCheck)       NULL,
  (tIntFnObjectPreClose)    NULL,
  (tIntFnObjectClose)       NULL,
  (tIntFnChildNew)          NULL,
  (tIntFnChildInitDone)     NULL,
  (tIntFnChildClose)        NULL,
  (tIntFnMsgReceive)        NULL,
  (tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iIOVtbl e_IO_vtbl = {
  (fnpIO_SeekRead)          IO_SeekRead,
  (fnpIO_SeekWrite)         IO_SeekWrite,
  (fnpIO_GetSize)           IO_GetSize,
  (fnpIO_SetSize)           IO_SetSize,
  (fnpIO_Flush)             IO_Flush
};
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter IO::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_IO,                                 // interface identifier
    PID_UNIXDISKIO,                         // plugin identifier
    0x00000000,                             // subtype identifier
    IO_VERSION,                             // interface version
    VID_SYCHEV,                             // interface developer
    &i_IO_vtbl,                             // internal(kernel called) function table
    (sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
    &e_IO_vtbl,                             // external function table
    (sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
    IO_PropTable,                           // property table
    mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
    sizeof(Private),                        // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave IO::Register method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_FILL_CHAR
// You have to implement propetry: pgOBJECT_HASH
// You have to implement propetry: pgOBJECT_REOPEN_DATA
// You have to implement propetry: plDriveNumber
// AVP Prague stamp end



