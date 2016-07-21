// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  12 February 2003,  17:49 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_os.c
//  File input/output for Win32
// Additional info
//   File input/output for Win32




// AVP Prague stamp begin( OS, Interface comment )
// --- 213d8b50_e06c_11d3_bf0e_00d0b7161fb8 ---
// OS interface implementation
// Short comments -- object system interface
// Extended comment
//   Interface defines behavior of an object system. It responds for
//     - enumerate IO and Folder objects by creating Folder objects.
//     - create and delete IO and Folder objects
//   There are two possible ways to create or open IO object on OS. First is three steps protocol:
//     -- call ObjectCreate system method and get new IO (Folder) object
//     -- dictate properties of new object
//     -- call ObjectCreateDone system method and have working object
//   Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//   Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define OS_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "win32_os.h"
#include "win32_io.h"
#include "win32_objptr.h"
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
// Short comments -- object system interface
// Extended comment -
//  Interface defines behavior of an object system. It responds for
//    - enumerate IO and Folder objects by creating Folder objects.
//    - create and delete IO and Folder objects
//  There are two possible ways to create or open IO object on OS. First is three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface OS. Inner data structure

typedef struct tag_OS_Data {
	tDWORD win32_err; // windows last error
} OS_Data;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_OS {
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OS_Data*           data; // pointer to the "OS" data structure
} *hi_OS;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpOS_PtrCreate) ( hi_OS _this, hObjPtr* result, hOBJECT name );              // -- opens ObjPtr object;
typedef   tERROR (pr_call *fnpOS_IOCreate)  ( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ); // -- opens IO object;
typedef   tERROR (pr_call *fnpOS_Copy)      ( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ); // -- copies object inside OS;
typedef   tERROR (pr_call *fnpOS_Rename)    ( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ); // -- rename/move object by name inside OS;
typedef   tERROR (pr_call *fnpOS_Delete)    ( hi_OS _this, hOBJECT name );              // -- phisically deletes an entity on OS;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iOSVtbl {
	fnpOS_PtrCreate  PtrCreate;
	fnpOS_IOCreate   IOCreate;
	fnpOS_Copy       Copy;
	fnpOS_Rename     Rename;
	fnpOS_Delete     Delete;
}; // "OS" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// Interface "OS". External method table. Forward declarations
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name );
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iOSVtbl e_OS_vtbl = {
	OS_PtrCreate,
	OS_IOCreate,
	OS_Copy,
	OS_Rename,
	OS_Delete
};
// Interface "OS". External method table.
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// Interface "OS". Global(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)2)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "OS". Property table
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)(0x2f)) )
//! mSHARED_PROPERTY( pgFORMAT_MEDIA, can't extract default value )
 	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
  mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "win32 object system interface", 30 )
  mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
  mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "Win32 native file system", 25 )
  mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(MAX_PATH)) )
	mLOCAL_PROPERTY_BUF( pgNATIVE_ERR, OS_Data, win32_err, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
  mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_ANSI)) )
mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) {
  tERROR error;

	PR_TRACE_A0( root, "Enter \"OS::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_WIN32_NFIO,                         // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_PETROVITCH,                         // interface developer
		NULL,                                   // internal(kernel called) function table
		0,                                      // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(OS_Data),                        // memory size
		0                                       // interface flags
	);

  #ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"nfio\tOS(IID_OS) registered [error = 0x%08X]",error) );
  #endif

  PR_TRACE_A1( root, "Leave \"OS::Register\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, PtrCreate )
// Extended method comment
//  It responds for opening ObjPtr object by:
//    -- hString object holded folder name
//    -- hIO object's folder
//    -- another hObjPtr object (cloning)
//
// Behaviour comment
//  Must realise the same result as if client uses following three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//  If parameter name is NULL must open *ROOT* folder
//
// Parameters are:
//   -- "name": name of the object to create pointer
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name ) {
  tDWORD    len;
  tERROR    error = errOK;
  hObjPtr   ret_val = 0;
  tCHAR     path[MAX_PATH] = {0};
	tCHAR*    ppath = path;
	tIID      iid;
  OS_Data*  d;
	PR_TRACE_A0( _this, "Enter \"OS::PtrCreate\" method" );

  d = _this->data;
  d->win32_err = 0;

	iid = CALL_SYS_PropertyGetDWord( name, pgINTERFACE_ID );
	if ( iid == IID_OBJPTR )
		error = CALL_ObjPtr_Clone( (hObjPtr)name, &ret_val );

  else if ( PR_FAIL(error=_ExtractName(name,(tPTR*)&ppath,sizeof(path),pgOBJECT_PATH,&len,&iid)) )
    ;

	else if ( PR_FAIL(error=CALL_SYS_ObjectCreate(_this,(hOBJECT*)&ret_val,IID_OBJPTR,PID_WIN32_NFIO,SUBTYPE_ANY)) )
		;

	else if (
		PR_FAIL(error=CALL_SYS_PropertySetStr(ret_val,0,pgOBJECT_PATH,ppath,len,codepage)) ||
		PR_FAIL(error=CALL_SYS_ObjectCreateDone(ret_val))
	) {
    _this->data->win32_err = CALL_SYS_PropertyGetDWord( ret_val, pgNATIVE_ERR );
    CALL_SYS_ObjectClose( ret_val );
    ret_val = 0;
  }
	
	if ( ppath != path )
		CALL_SYS_ObjHeapFree( name, ppath );

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"OS::PtrCreate\" method, ret hObjPtr = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// --- Interface "OS". Method "IOCreate"
// Extended method comment
//   It responds for opening IO object by:
//     -- hString held objects name
//     -- hIO object's name (cloning)
//
// Behaviour comment
//   Must realise the same result as if client uses following three steps protocol:
//     -- call ObjectCreate system method and get new IO (Folder) object
//     -- dictate properties of new object
//     -- call ObjectCreateDone system method and have working object
//   Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//   If parameter name is NULL must open *ROOT* folder
//
// Parameter "name":
//   One of:
//     -- another hIO object (cloning)
//     -- string object (opens IO by name)
//
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ) {
  tDWORD   len = MAX_PATH;
  tERROR   error;
  hIO      ret_val = 0;
	tCHAR    obj_name[MAX_PATH];
  tPTR     pobj_name = obj_name;
  OS_Data* d;
  PR_TRACE_A0( _this, "Enter \"OS::IOCreate\" method" );

  d = _this->data;
  d->win32_err = 0;

  if ( PR_FAIL(error=_ExtractName(name,&pobj_name,pgOBJECT_FULL_NAME,len,&len,0)) )
		;
	else if ( PR_FAIL(error=CALL_SYS_ObjectCreate(_this,(hOBJECT*)&ret_val,IID_IO,PID_WIN32_NFIO,SUBTYPE_ANY)) )
		;
	else if (
		PR_FAIL(error=CALL_SYS_PropertySetStr(ret_val,0,pgOBJECT_FULL_NAME,pobj_name,len,codepage)) ||
		PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_OPEN_MODE,open_mode)) ||
		PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_ACCESS_MODE,access_mode)) ||
		PR_FAIL(error=CALL_SYS_ObjectCreateDone(ret_val))
	) {
    d->win32_err = CALL_SYS_PropertyGetDWord( ret_val, pgNATIVE_ERR );
		CALL_SYS_ObjectClose( ret_val );
		ret_val = 0;
	}

	if ( pobj_name != obj_name )
		CALL_SYS_ObjHeapFree( name, pobj_name );

	if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave \"OS::IOCreate\" method, ret hIO = %p, error = 0x%08X", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ) {
	tERROR   error;
	tCHAR    src_nb[MAX_PATH];
	tPTR     src_n = src_nb;
  tCHAR    dst_nb[MAX_PATH];
	tPTR     dst_n = dst_nb;
  OS_Data* d;
	PR_TRACE_A0( _this, "Enter \"OS::Copy\" method" );

  d = _this->data;
  d->win32_err = 0;

  if ( !fCopyFile ) {
		PR_TRACE(( _this, prtERROR, "nfio\tCopyFile export pointer cannot be imported from the Windows kernel" ));
		error = errINTERFACE_NOT_LOADED;
	}
	
	else if ( PR_FAIL(error=_ExtractName(src_name,&src_n,sizeof(src_nb),pgOBJECT_FULL_NAME,0,0)) )
		;
	
	else if ( PR_FAIL(error=_ExtractName(dst_name,&dst_n,sizeof(dst_nb),pgOBJECT_FULL_NAME,0,0)) )
		;

	else if ( fCopyFile(src_n,dst_n,!overwrite) )
		;

	else
		error = _ConvertWin32Err( d->win32_err = GetLastError() );
	
	if ( src_n != src_nb )
		CALL_SYS_ObjHeapFree( src_name, src_n );
	
	if ( dst_n != dst_nb )
		CALL_SYS_ObjHeapFree( dst_name, dst_n );
	
	PR_TRACE_A1( _this, "Leave \"OS::Copy\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// --- Interface "OS". Method "Delete"
// Extended method comment
//   phisically deletes an entity on OS
// Behaviour comment
//   if parameter "object" is really hIO must close it and delete after close.
// Parameter "object":
//   name of the object to delete may be addressed by:
//     -- hString
//     -- hIO
//     -- hObjPtr
//   if it is hIO or hFolder it must be closed before deleting
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name ) {
  tIID     iid = 0;
  tDWORD   len;
  tERROR   error;
  tCHAR    objname[MAX_PATH] = {0};
	tPTR     pobjname = objname;
  OS_Data* d;
  PR_TRACE_A0( _this, "Enter \"OS::Delete\" method" );

  d = _this->data;
  d->win32_err = 0;

  iid = CALL_SYS_PropertyGetDWord( name, pgINTERFACE_ID );

  if ( iid == IID_IO )
		error = CALL_SYS_PropertySetBool( name, pgOBJECT_DELETE_ON_CLOSE, cTRUE );
	
	else if ( !fDeleteFile )
		error = errINTERFACE_NOT_LOADED;

	else if ( PR_FAIL(error = _ExtractName(name,&pobjname,sizeof(objname),pgOBJECT_FULL_NAME,&len,&iid)) )
		;

	else if ( !fDeleteFile(pobjname) )
		error = _ConvertWin32Err( d->win32_err = GetLastError() );

	if ( PR_SUCC(error) && (iid == IID_OBJPTR) && PR_FAIL(error=CALL_ObjPtr_Next((hObjPtr)name)) ) {
		if ( (error == errOUT_OF_OBJECT) || (error == errEND_OF_THE_LIST) )
			error = errOK;
		else if ( PR_SUCC(CALL_ObjPtr_Reset((hObjPtr)name,cFALSE)) )
			PR_TRACE(( _this, prtERROR, "nfio\tPointer cannot be advanced. It has been reset" ));
		else
			PR_TRACE(( _this, prtERROR, "nfio\tPointer cannot be advanced and reset" ));
	}

	if ( pobjname != objname )
		CALL_SYS_ObjHeapFree( name, pobjname );

  PR_TRACE_A1( _this, "Leave \"OS::Delete\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// --- Interface "OS". Method "Rename"
// Extended method comment
//   Renames/moves object by name inside single OS
//   Must move object through folders if nessesary
// Parameter "old_name":
//   Name to be changed
// Parameter "new_name":
//   Target name
// Parameter "flags":
//   Rename mode. If cOS_REN_OVERWRITE flag indicated, overwrites target object if one exists before.
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ) {

  tERROR   error;

	tCHAR    o_name[MAX_PATH];
	tPTR     pold = o_name;
  tCHAR    n_name[MAX_PATH];
	tPTR     pnew = n_name;

	tIID     old_iid;
	tIID     new_iid = 0;
  OS_Data* d;
	
	PR_TRACE_A0( _this, "Enter \"OS::Rename\" method" );

  d = _this->data;
  d->win32_err = 0;

  old_iid = CALL_SYS_PropertyGetDWord( old_name, pgINTERFACE_ID );

  if ( old_iid == IID_OBJPTR ) {
    error = CALL_ObjPtr_Rename( (hObjPtr)old_name, new_name, overwrite );
    d->win32_err = CALL_SYS_PropertyGetDWord( old_name, pgNATIVE_ERR );
  }

	else if ( PR_FAIL(error=_ExtractName(old_name,&pold,sizeof(o_name),pgOBJECT_FULL_NAME,0,&old_iid)) )
		;
	
	else if ( PR_FAIL(error=_ExtractName(new_name,&pnew,sizeof(n_name),pgOBJECT_FULL_NAME,0,&new_iid)) )
		;
	else if ( PR_FAIL(error=_ren_move(_this,pold,pnew,overwrite,&d->win32_err)) )
    ;

	if ( pold != o_name )
		CALL_SYS_ObjHeapFree( old_name, pold );

	if ( pnew != n_name )
		CALL_SYS_ObjHeapFree( new_name, pnew );
	
  PR_TRACE_A1( _this, "Leave \"OS::Rename\" method, ret tERROR = 0x%08X", error );
  return error;
}
// AVP Prague stamp end





