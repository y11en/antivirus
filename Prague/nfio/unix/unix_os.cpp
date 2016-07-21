// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  22 July 2004,  16:57 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Unix native file IO
// Author      -- petrovich
// File Name   -- unix_os.cpp
// Additional info
//    File input/output for Unix
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_nfio.h>
#include <iface/i_root.h>
#include <iface/i_string.h>
#include <iface/i_io.h>
#include <iface/i_objptr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "unix_utils.h"
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct OSData : public cOS /*cObjImpl*/
{
private:
// Internal function declarations

// Property function declarations

public:
// External function declarations
	tERROR pr_call PtrCreate( hObjPtr* result, hOBJECT p_name );
	tERROR pr_call IOCreate( hIO* result, hOBJECT p_name, tDWORD p_access_mode, tDWORD p_open_mode );
	tERROR pr_call Copy( hOBJECT p_src_name, hOBJECT p_dst_name, tBOOL p_overwrite );
	tERROR pr_call Rename( hOBJECT p_old_name, hOBJECT p_new_name, tBOOL p_overwrite );
	tERROR pr_call Delete( hOBJECT p_name );

// Data declaration
  tDWORD m_system_error; // last system error (errno)
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(OSData)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, PtrCreate )
// Extended method comment
//    It responds for opening ObjPtr object by:
//      -- hString object holded folder name
//      -- hIO object's folder
//      -- another hObjPtr object (cloning)
//
// Behaviour comment
//    Must realise the same result as if client uses following three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//    If parameter name is NULL must open *ROOT* folder
//
// Parameters are:
tERROR OSData::PtrCreate( hObjPtr* result, hOBJECT p_name ) 
{
  tERROR  error = errNOT_IMPLEMENTED;
  hObjPtr ret_val = NULL;
  PR_TRACE_A0( this, "Enter OS::PtrCreate method" );

  // Place your code here
  error = errOK;
  m_system_error = 0;
  
  if ( p_name ) {
    tIID anIID = p_name->propGetDWord ( pgINTERFACE_ID );
    if ( anIID == IID_OBJPTR )
      error = reinterpret_cast<hObjPtr> ( p_name )->Clone ( &ret_val );
    else
      if ( PR_SUCC ( error = sysCreateObject ( reinterpret_cast <hOBJECT*> ( &ret_val ), IID_OBJPTR, PID_NFIO, SUBTYPE_ANY ) ) ) {
        if ( anIID == IID_IO ) {
          tCHAR aSourcePath [ MAX_PATH ] = {0};
          error = p_name->propGetStr( 0, pgOBJECT_PATH, aSourcePath, sizeof ( aSourcePath ) );
          if ( PR_SUCC ( error ) )
            error = ret_val->propSetStr( 0, pgOBJECT_PATH, aSourcePath );
          if ( PR_SUCC ( error ) )
            error = ret_val->sysCreateObjectDone ();
          if ( PR_SUCC ( error ) )
            error = p_name->propGetStr( 0, pgOBJECT_NAME, aSourcePath, sizeof ( aSourcePath ) );        
          while ( PR_SUCC ( error ) ) {
            if ( PR_FAIL ( error = ret_val->Next () ) )
              continue;
            tCHAR aDestPath [ MAX_PATH ] = {0};
            if ( PR_FAIL ( error = ret_val->propGetStr( 0, pgOBJECT_NAME, aDestPath, sizeof ( aDestPath ) ) ) )
              continue;
          if ( !strcmp ( aSourcePath, aDestPath ) )
            break;
          }      
        }
        if ( anIID == IID_STRING ) {
          error = reinterpret_cast <hSTRING> ( p_name )->ExportToProp ( 0, cSTRING_WHOLE, (hOBJECT)ret_val, pgOBJECT_PATH );
          if ( PR_SUCC ( error ) )
            error = ret_val->sysCreateObjectDone ();
        }
      }
  }
  else {
    error = sysCreateObject ( reinterpret_cast <hOBJECT*> ( &ret_val ), IID_OBJPTR, PID_NFIO, SUBTYPE_ANY );
    if ( PR_SUCC ( error ) ) {
      tCHAR aRoot [] = "/";
      error = ret_val->propSetStr (  0, pgOBJECT_PATH, aRoot );
    }
    if ( PR_SUCC ( error ) ) 
      error = ret_val->sysCreateObjectDone ();
  }
  
  if ( PR_FAIL ( error ) && ret_val ) {
    m_system_error = ret_val->propGetDWord ( pgNATIVE_ERR );
    ret_val->sysCloseObject ();
    ret_val = 0; 
  }

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave OS::PtrCreate method, ret hObjPtr = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//    It responds for opening IO object by:
//      -- hString: open object by name
//      -- hIO: object's name (cloning)
//      -- hObjPtr: open object by ptr
// Behaviour comment
//    Must realise the same result as if client uses following three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//    If parameter name is NULL must open *ROOT* folder
//
// Parameters are:
tERROR OSData::IOCreate( hIO* result, hOBJECT p_name, tDWORD p_access_mode, tDWORD p_open_mode ) 
{
  tERROR error = errNOT_IMPLEMENTED;
  hIO    ret_val = NULL;
  PR_TRACE_A0( this, "Enter OS::IOCreate method" );

  // Place your code here
  error = errOK;
  m_system_error = 0;

  if ( !p_name )
    error = errPARAMETER_INVALID;

  if ( PR_SUCC ( error ) ) {
    tIID anIID = p_name->propGetDWord ( pgINTERFACE_ID );
    
    if ( ( anIID != IID_STRING ) && ( anIID != IID_IO ) && ( anIID != IID_OBJPTR ) )
      error = errPARAMETER_INVALID;
  }
  
  if ( PR_SUCC ( error ) ) 
    error= sysCreateObject ( reinterpret_cast<hOBJECT*> ( &ret_val ), IID_IO, PID_NFIO, SUBTYPE_ANY );

  tCHAR aFileName [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aFileName, sizeof ( aFileName ) );
  
  if ( PR_SUCC ( error ) )
    error = ret_val->propSetStr ( 0, pgOBJECT_FULL_NAME, aFileName ); 
  
  if ( PR_SUCC ( error ) )
    error = ret_val->propSetDWord ( pgOBJECT_OPEN_MODE, p_open_mode ); 
  
  if ( PR_SUCC ( error ) )
    error = ret_val->propSetDWord ( pgOBJECT_ACCESS_MODE, p_access_mode );
  
  if ( PR_SUCC ( error ) )
    error = ret_val->sysCreateObjectDone ();
  
  if ( PR_FAIL ( error ) && ret_val ) {
    m_system_error = ret_val->propGetDWord ( pgNATIVE_ERR );
    ret_val->sysCloseObject();
    ret_val = 0;
  }

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( this, "Leave OS::IOCreate method, ret hIO = %S, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR OSData::Copy( hOBJECT p_src_name, hOBJECT p_dst_name, tBOOL p_overwrite ) 
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter OS::Copy method" );

  // Place your code here
  error = errOK;
  m_system_error = 0;

  if ( !p_src_name || !p_dst_name )
    error = errPARAMETER_INVALID;
  
  if ( PR_SUCC ( error ) ) {
    tIID anIID = p_src_name->propGetDWord ( pgINTERFACE_ID );
    
    if ( ( anIID != IID_STRING ) && ( anIID != IID_OBJPTR ) ) 
      error = errPARAMETER_INVALID;
  }
   
  if ( PR_SUCC ( error ) ) {
    tIID anIID = p_dst_name->propGetDWord ( pgINTERFACE_ID );
    
    if ( ( anIID != IID_STRING ) && ( anIID != IID_OBJPTR ) ) 
      error = errPARAMETER_INVALID;
  }

  tCHAR aSource [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_src_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aSource, sizeof ( aSource ) );

  tCHAR aDestination [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_dst_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aDestination, sizeof ( aDestination ) );
  
  if ( PR_SUCC ( error ) )
    error = _copy ( aSource, aDestination, p_overwrite, m_system_error );
  
  PR_TRACE_A1( this, "Leave OS::Copy method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Extended method comment
//    Renames/moves object by name inside single OS
//    Must move object through folders if nessesary
// Parameters are:
//   "new_name"  : Target name
tERROR OSData::Rename( hOBJECT p_old_name, hOBJECT p_new_name, tBOOL p_overwrite ) 
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter OS::Rename method" );

  // Place your code here
  error = errOK;
  m_system_error = 0;

  if ( !p_old_name || !p_new_name ) 
    error = errPARAMETER_INVALID;

  tIID anIID;
  
  if ( PR_SUCC ( error ) ) {
    anIID = p_new_name->propGetDWord ( pgINTERFACE_ID );
    
    if ( ( anIID != IID_STRING ) && ( anIID != IID_OBJPTR ) ) 
      error = errPARAMETER_INVALID;
    
  }
  
  if ( PR_SUCC ( error ) ) {
    anIID = p_old_name->propGetDWord ( pgINTERFACE_ID );
    
    if ( ( anIID != IID_STRING ) && ( anIID != IID_OBJPTR ) ) 
      error = errPARAMETER_INVALID;
  }

  if ( anIID == IID_OBJPTR ) 
    error = reinterpret_cast<hObjPtr> ( p_old_name )->Rename ( p_new_name, p_overwrite );

  else {

    tCHAR aSource [ MAX_PATH ] = {0};
    
    if ( PR_SUCC ( error ) )
      error = p_old_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aSource, sizeof ( aSource ) );

    tCHAR aDestination [ MAX_PATH ] = {0};
    
    if ( PR_SUCC ( error ) ) 
      error = p_new_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aDestination, sizeof ( aDestination ) );

    if ( PR_SUCC ( error ) )
      error = _rename ( aSource, aDestination, p_overwrite, m_system_error );
  }
  PR_TRACE_A1( this, "Leave OS::Rename method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    phisically deletes an entity on OS
// Behaviour comment
//    if parameter "object" is really hIO or hObjPtr must close it and delete after close.
// Parameters are:
tERROR OSData::Delete( hOBJECT p_name ) 
{
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( this, "Enter OS::Delete method" );

  // Place your code here
  error = errOK;
  m_system_error = 0;

  if ( !p_name )
    error = errPARAMETER_INVALID;

  tIID anIID;

  if ( PR_SUCC ( error ) ) {
    anIID = p_name->propGetDWord ( pgINTERFACE_ID );
    
    if ( ( anIID != IID_STRING ) && ( anIID != IID_IO ) && ( anIID != IID_OBJPTR ) ) 
      error = errPARAMETER_INVALID;
  }

  tCHAR aFileName [ MAX_PATH ] = {0};
  
  if ( PR_SUCC ( error ) ) 
    error = p_name->propGetStr ( 0, pgOBJECT_FULL_NAME, aFileName, sizeof ( aFileName ) );
  
  if ( PR_SUCC ( error ) ) {
    if ( anIID == IID_IO )
      p_name->propSetBool ( pgOBJECT_DELETE_ON_CLOSE, cTRUE );
    if ( anIID == IID_OBJPTR )
      reinterpret_cast <hObjPtr> ( p_name )->Next ();
    if ( ( anIID == IID_OBJPTR ) || ( anIID == IID_STRING ) )
      error = _delete ( aFileName, m_system_error );
  }
  
  PR_TRACE_A1( this, "Leave OS::Delete method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "OS". Register function
tERROR OSData::Register( hROOT root ) 
{
  tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(OS_PropTable)
  mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "unix object system interface", 29 )
  mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
  mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "unix native file system", 24 )
  mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
  mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
  mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(MAX_PATH)) )
  mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cTRUE)) )
  mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)(0x2f)) )
  mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
  mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_ANSI)) )
  mpLOCAL_PROPERTY_BUF( pgNATIVE_ERR, OSData, m_system_error, cPROP_BUFFER_READ )
mpPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(OS)
mINTERNAL_TABLE_END(OS)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(OS)
  mEXTERNAL_METHOD(OS, PtrCreate)
  mEXTERNAL_METHOD(OS, IOCreate)
  mEXTERNAL_METHOD(OS, Copy)
  mEXTERNAL_METHOD(OS, Rename)
  mEXTERNAL_METHOD(OS, Delete)
mEXTERNAL_TABLE_END(OS)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

  PR_TRACE_A0( root, "Enter OS::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_OS,                                 // interface identifier
    PID_NFIO,                               // plugin identifier
    0x00000000,                             // subtype identifier
    OS_VERSION,                             // interface version
    VID_PETROVITCH,                         // interface developer
    &i_OS_vtbl,                             // internal(kernel called) function table
    (sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
    &e_OS_vtbl,                             // external function table
    (sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
    OS_PropTable,                           // property table
    mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
    sizeof(OSData)-sizeof(cObjImpl),        // memory size
    0                                       // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"OS(IID_OS) registered [%terr]",error) );
  #endif // _PRAGUE_TRACE_

  PR_TRACE_A1( root, "Leave OS::Register method, ret %terr", error );
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
  return error;
}

tERROR pr_call OS_Register( hROOT root ) { return OSData::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_HASH
// You have to implement propetry: pgOBJECT_REOPEN_DATA
// AVP Prague stamp end



