// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  30 April 2002,  12:17 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// Copyright (c) Kaspersky Labs. 1996-2000.
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_objptr.c
//  File input/output for Win32
// Additional info
//   File input/output for Win32



// AVP Prague stamp begin( ObjPtr, Interface comment )
// --- 74d331e0_e06c_11d3_bf0e_00d0b7161fb8 ---
// ObjPtr interface implementation
// Short comments -- object enumerator
// Extended comment
//   Defines behavior of an object container/enumerator
//
//   Supplies object enumerate functionality on one level of hierarchy only.
//   For example: on logical drive object of this interface must enumerate objects in one folder.
//
//   Also must support creating new IO object in the container. There are two ways to do it. First is three steps protocol:
//     -- ObjectCreate system method with folder object as parent
//     -- dictate all necessary properties for the new object
//     -- ObjectCreateDone
//   Second is CreateNew folder's method which is wrapper of above protocol.
//   Advise: when caller invokes ObjectCreateDone method folder object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#ifdef __MWERKS__
	#include <string.h>
	#include <dirent.h>
	#include <sys\stat.h>
	#include <errno.h>
	#include "fnmatch.h"
	#define strcasecmp stricmp
#endif
#include <prague.h>
#include "nfio_os.h"
#include "nfio_objptr.h"
#include <pr_oid.h>
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface "ObjPtr". Inner data structure
// AVP Prague stamp end

#if defined (__unix__)
static tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime
#else
static tCODEPAGE codepage = cCP_OEM; // must be READ_ONLY at runtime
#endif


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct tag_hi_ObjPtr {
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	ObjPtrData*        data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

#if defined( __cplusplus )
}
#endif

// AVP Prague stamp end


static tERROR pr_call OP_SetMask( hi_ObjPtr _this, tSTRING buffer, tDWORD* size );
static tERROR pr_call OP_SetPath( hi_ObjPtr _this, tSTRING buffer, tDWORD* size );
static tERROR pr_call OP_SetRelPath( hi_ObjPtr _this, tPTR rel_path, tDWORD size );
static tERROR pr_call OP_getFName( ObjPtrData* data, tCHAR* buff );
static tERROR pr_call OP_ChangeTo( hi_ObjPtr _this, tCHAR* name );

tSTRING pr_call FindSeparator( tSTRING buffer, tDWORD* size );
	

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = {
	(tIntFnRecognize)        NULL,
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
// Interface "ObjPtr". External method table. Forward declarations
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
// Interface "ObjPtr". External method table.
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call ObjPtr_PROP_GetName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_SetPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetFPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_is_folder( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_SetMask( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_set_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_sizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_set_sizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_get_attribs( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_set_attribs( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Global(shared) property table variables
// AVP Prague stamp end
#define ObjPtr_VERSION ((tVERSION)2)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "ObjPtr". Property table
mPROPERTY_TABLE(ObjPtr_PropTable)
 	mLOCAL_PROPERTY_BUF( plNATIVE_ERR, ObjPtrData, nfio_errno, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PTR_STATE, ObjPtrData, state, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, ObjPtrData, origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_get_size, ObjPtr_PROP_set_size )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_get_sizeQ, ObjPtr_PROP_set_sizeQ )
	mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, ObjPtr_PROP_get_attribs, ObjPtr_PROP_set_attribs )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, ObjPtrData, os, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "object enumerator", 24 )
  mLOCAL_PROPERTY_FN( pgOBJECT_NAME, ObjPtr_PROP_GetName, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, ObjPtr_PROP_GetPath, ObjPtr_PROP_SetPath )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_GetFPath, NULL )
	mLOCAL_PROPERTY( pgMASK, ObjPtrData, mask, cPROP_BUFFER_READ, NULL, ObjPtr_PROP_SetMask )
 	mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME_CP, codepage, sizeof(codepage) )
 	mSHARED_PROPERTY_PTR( pgOBJECT_PATH_CP, codepage, sizeof(codepage) )
 	mSHARED_PROPERTY_PTR( pgOBJECT_NAME_CP, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( plMASK_CP, codepage, sizeof(codepage) )
	mLOCAL_PROPERTY_FN( pgIS_FOLDER, ObjPtr_PROP_get_is_folder, NULL )
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
		PID_NW_NFIO,                         // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000002,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(ObjPtrData),                     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [error=0x%08x]",error) );
	#endif

	PR_TRACE_A1( root, "Leave ObjPtr::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetName )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR pr_call ObjPtr_PROP_GetName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetName for property pgOBJECT_NAME" );

	d = _this->data;
	d->nfio_errno = 0;
	
	if ( d->state != cObjPtrState_PTR ) {
		*out_size = 0;
		error = errOBJECT_BAD_INTERNAL_STATE;
	}
	else if ( !buffer ) {
		*out_size = d->name_len;
		error = errOK;
	}
	else if ( d->name_len > size ) {
		*out_size = d->name_len;
		error = errBUFFER_TOO_SMALL;
	}
	else {
		*out_size = d->name_len;
		memcpy( buffer, d->dentry->d_name, d->name_len );
		error = errOK;
	}

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetName for property pgOBJECT_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetPath )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR pr_call ObjPtr_PROP_GetPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tDWORD copy_len;
	tDWORD full_len;
	tBOOL  prefix = cFALSE;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetPath for property pgOBJECT_PATH" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->path ) {
		full_len = copy_len = strlen (d->path) + 1;
				
		if ( !buffer )
			;
		else if ( size < full_len )
			full_len = 0, error = errBUFFER_TOO_SMALL;
		else {
			if ( prefix ) {
				*buffer++ = '/';
			}
			memcpy( buffer, d->path, copy_len );
		}
	}
	else if ( !buffer )
		full_len = sizeof(tCHAR);
	else if ( size < (full_len=sizeof(tCHAR)) )
		full_len = 0, error = errBUFFER_TOO_SMALL;
	else
		*buffer = 0;

	*out_size = full_len;
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetPath for property pgOBJECT_PATH, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetPath )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- pgOBJECT_PATH
tERROR pr_call ObjPtr_PROP_SetPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	ObjPtrData* d;
	PR_TRACE_A1( _this, "Enter ObjPtr::PROP_SetPath(%s)", buffer ? buffer : "NULL" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( !buffer ) {
		size = sizeof(tCHAR) * MAX_PATH;
		error = errOK;
	}

	else if ( size > (sizeof(tCHAR)*MAX_PATH) ) {
		size = 0;
		error = errPARAMETER_INVALID;
	}
	
	else if ( PR_FAIL(error=OP_SetPath(_this,buffer,&size)) )
		;
	else if ( d->dirp )
		error = ObjPtr_Reset( _this, cFALSE );

	*out_size = size;
	PR_TRACE_A2( _this, "Leave ObjPtr::PROP_SetPath, ret tDWORD = %u(size), error = 0x%08X", *out_size, error );
	return error;

}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, GetFPath )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call ObjPtr_PROP_GetFPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error = errOK;
	tUINT  path_len;
	tUINT  full_len;
	tBOOL  prefix;
	tDWORD lsize;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetFPath for property pgOBJECT_FULL_NAME" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR ) {
		*out_size = 0;
		error = errOBJECT_BAD_INTERNAL_STATE;
		goto exit;
	}

	if ( !out_size )
		out_size = &lsize;

	prefix = cFALSE;
	if ( d->path ) {
		path_len = strlen(d->path);
		full_len = path_len + d->name_len;
	}
	else {
		path_len = 0;
		full_len = d->name_len;
	}

	if ( !buffer ) {
		*out_size = full_len;
		error = errOK;
	}
	else if ( size < full_len ) {
		*out_size = 0;
		error = errBUFFER_TOO_SMALL;
	}
	else {
		if ( prefix ) {
			*buffer++ = '/';
		}
		if ( path_len )
			memcpy( buffer, d->path, path_len );
		
		memcpy( buffer+path_len, d->dentry->d_name, d->name_len );
		*out_size = full_len;
	}
	
exit:;
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetFPath for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_is_folder )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- IS_FOLDER
tERROR pr_call ObjPtr_PROP_get_is_folder( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_is_folder for property pgIS_FOLDER" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR ) {
		error = errOBJECT_BAD_INTERNAL_STATE;
		*out_size = 0;
	}
	else {
		error = errOK;
		*out_size = sizeof(tBOOL);
		if ( buffer ) {
			if ( S_ISDIR (d->dstat.st_mode) )
				*((tBOOL*)buffer) = cTRUE;
			else
				*((tBOOL*)buffer) = cFALSE;
		}
	}

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_is_folder for property pgIS_FOLDER, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, SetMask )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- MASK
tERROR pr_call ObjPtr_PROP_SetMask( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr::PROP_SetMask for property pgMASK" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( !buffer ) {
		size = MAX_PATH;
		error = errOK;
	}
	else if ( size > MAX_PATH ) {
		size = 0;
		error = errBUFFER_TOO_SMALL;
	}
	else {
		tCHAR* p0 = FindSeparator (buffer, &size);
		if ( !p0 )                                               // there is only name in the buffer
			error = OP_SetMask( _this, buffer, &size );
		else if ( *p0 == 0 ) {                                   // there is only path in the buffer
			size = p0 - buffer;
			error = OP_SetPath( _this, buffer, &size );
		}
		else {                                                    // there are path and name in the buffer
			tDWORD psize = p0-buffer;
			tDWORD msize = size - psize;
			if ( PR_FAIL(error = OP_SetPath(_this,buffer,&psize)) )
				;
			else
				error = OP_SetMask( _this, buffer+(p0-buffer), &msize );
		}
		if ( PR_SUCC(error) && d->dirp )
			error = ObjPtr_Reset( _this, cFALSE );
	}

	PR_TRACE_A3( _this, "Leave ObjPtr::PROP_SetMask method, ret hUINT = %u, error = 0x%08X, mask = %s", size, error, d->mask );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_size )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_get_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_size for property pgOBJECT_SIZE" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR ) {
		error = errOBJECT_BAD_INTERNAL_STATE;
		*out_size = 0;
	}
	else {
		error = errOK;
		*out_size = sizeof(tDWORD);
		if ( buffer ) {
			*((tDWORD*)buffer) = d->dstat.st_size;
		}
	}

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_size for property pgOBJECT_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_size )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_set_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	tCHAR  path[MAX_PATH];
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_size for property pgOBJECT_SIZE" );

	_this->data->nfio_errno = 0;
	if ( !buffer ) {
		error = errPARAMETER_INVALID;
		*out_size = 0;
	}
	else if ( PR_SUCC(error=OP_getFName(_this->data,path)) )
		error = errNOT_IMPLEMENTED;

	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_size for property pgOBJECT_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_sizeQ )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_get_sizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	typedef struct tag_qw {
		tDWORD l;
		tDWORD h;
	} tqw;
	
	ObjPtrData* d;
	tERROR      error;

	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_sizeQ for property pgOBJECT_SIZE_Q" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR ) {
		error = errOBJECT_BAD_INTERNAL_STATE;
		*out_size = 0;
	}
	else {
		error = errOK;
		*out_size = sizeof(tQWORD);
		if ( buffer ) {
			((tqw*)buffer)->h = 0;
			((tqw*)buffer)->l = d->dstat.st_size;
		}
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_sizeQ for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_sizeQ )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_set_sizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	tCHAR  path[MAX_PATH];
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_sizeQ for property pgOBJECT_SIZE_Q" );

	_this->data->nfio_errno = 0;

	if ( !buffer ) {
		error = errPARAMETER_INVALID;
		*out_size = 0;
	}
	else if ( PR_SUCC(error=OP_getFName(_this->data,path)) )
		error = errNOT_IMPLEMENTED;
	
	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_sizeQ for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_attribs )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_get_attribs( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR      error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_attribs for property pgOBJECT_ATTRIBUTES" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR ) {
		*out_size = 0;
		error = errOBJECT_BAD_INTERNAL_STATE;
	}
	else {
		error = errOK;
		*out_size = sizeof(tDWORD);
		if ( buffer )
			*((tDWORD*)buffer) = d->dstat.st_mode;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_attribs for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_attribs )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_set_attribs( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	tCHAR  name[MAX_PATH];
	tCHAR* buff = name;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_attribs for property pgOBJECT_ATTRIBUTES" );

	d = _this->data;
	d->nfio_errno = 0;

	return errNOT_IMPLEMENTED;
/*
	*out_size = 0;

	if ( !fSetFileAttributes )
		error = errINTERFACE_NOT_LOADED;

	else if ( !buffer )
		error = errPARAMETER_INVALID;

	else if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;

	else
		error = OP_getFName( d, buff );

	if ( PR_SUCC(error) ) {
		if ( fSetFileAttributes(buff,*(tDWORD*)buffer) ) {
			d->dentry->dwFileAttributes = *(tDWORD*)buffer;
			*out_size = sizeof(tDWORD);
		}
		else
			error = _convert_nfio_errno( d->nfio_errno = errno );
	}
		
	if ( buff != name )
		CALL_SYS_ObjHeapFree( _this, buff );
*/
	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_attribs for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this ) {
	tERROR   error;
	ObjPtrData* d;
	hOBJECT dad;
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInit method" );

	d = _this->data;
	d->nfio_errno = 0;
	d->origin = OID_GENERIC_IO;

	dad = CALL_SYS_ParentGet( _this, IID_ANY );
	if (
		(IID_OS         == CALL_SYS_PropertyGetDWord(dad,pgINTERFACE_ID)) &&
		(PID_NW_NFIO == CALL_SYS_PropertyGetDWord(dad,pgPLUGIN_ID))
	)
		d->os = (hOS)dad;
		
	d->mask = d->mask_buff;
	strcpy (d->mask, "*.*");
	d->name_len = sizeof(tCHAR);
	error = errOK;
	
	PR_TRACE_A0( _this, "Leave ObjPtr::ObjectInit method, ret tERROR = errOK" );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// --- Interface "ObjPtr". Method "ObjectInitDone"
// Extended method comment
//   Notification that all necessary properties have been set and object must go to operation state
// Behaviour comment
//
// Result comment
//   Returns value differ from errOK if object cannot function properly
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this ) {
	tERROR e;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInitDone method" );
	
	d = _this->data;
	if ( !d->dirp ) {
		d->nfio_errno = 0;
		e = ObjPtr_Reset( _this, cFALSE );
	}
	else
		e = errOK;
	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInitDone method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// --- Interface "ObjPtr". Method "ObjectClose"
// Extended method comment
//   Kernel warns object it must be closed
// Behaviour comment
//   Object takes all necessary "before closing" actions
// Result comment
//
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this ) {
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectClose method" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->dirp ) {
		closedir ( d->dirp );
		d->nfio_errno = errno;
		d->dirp = 0;
		d->name_len = 0;
	}

	if ( d->full ) {
		CALL_SYS_ObjHeapFree( _this, d->full );
		d->full = 0;
		d->full_len = 0;
	}

	if ( d->mask != d->mask_buff ) {
		CALL_SYS_ObjHeapFree( _this, d->mask );
		d->mask = d->mask_buff;
		d->mask_len = sizeof(d->mask_buff);
	}

	PR_TRACE_A0( _this, "Leave ObjPtr::ObjectClose method, ret hERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  Creates hIO or hObjPtr object current in enumeration.
// Result comment
//  Returns hIO or hObjPtr object. Depends on current object.
// Parameters are:
// "access_mode" : New object open mode
// "open_mode"   : Attributes of the new object
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ) {
	ObjPtrData* d;
	tERROR      error;
	hIO         ret_val = NULL;
	PR_TRACE_A0( _this, "Enter ObjPtr::IOCreate method" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( !name && (d->state != cObjPtrState_PTR) )
		error = errOBJECT_BAD_INTERNAL_STATE;

	else {
		tCHAR  obj_name[MAX_PATH];
		tCHAR* pobj_name = obj_name;

		if ( name ) {
			tDWORD name_len = 0;
			tUINT  path_len;
			if ( d->full ) {
				path_len = strlen (d->full);
			}
			else
				path_len = 0;
			if ( PR_FAIL(error=CALL_String_LengthEx(name,&name_len,0,codepage,cSTRING_Z)) )
				;
			else if ( ((path_len+name_len) > sizeof(obj_name)) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this, (tPTR) &pobj_name,path_len+name_len)) )
				;
			else {
				memcpy( pobj_name, d->full, path_len );
				error = CALL_String_ExportToBuff( name, 0, cSTRING_WHOLE, obj_name+path_len, name_len, codepage, cSTRING_Z );
			}
		}
		else
			error = OP_getFName( d, pobj_name );

		if ( S_ISDIR (d->dstat.st_mode) )
			open_mode |= fOMODE_FOLDER_AS_IO;
		
		if ( PR_FAIL(error) )
			;
		else if ( PR_FAIL(error=CALL_SYS_ObjectCreate(_this,(hOBJECT*)&ret_val,IID_IO,PID_NW_NFIO,SUBTYPE_ANY)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_PropertySetStr(ret_val,0,pgOBJECT_NAME,obj_name,0,codepage)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_OPEN_MODE,open_mode)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_ACCESS_MODE,access_mode)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_ObjectCreateDone(ret_val)) ) 
			;

		if ( PR_FAIL(error) ) {
			CALL_SYS_ObjectClose( ret_val );
			ret_val = 0;
		}
		
		if ( pobj_name != obj_name )
			CALL_SYS_ObjHeapFree( _this, pobj_name );
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave ObjPtr::IOCreate method, ret hIO = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite ) {
	ObjPtrData* d;
	tERROR      error;
	tCHAR       src[MAX_PATH];
	tCHAR       dst[MAX_PATH];
	tCHAR*      psrc = src;
	tCHAR*      pdst = dst;
	PR_TRACE_A0( _this, "Enter ObjPtr::Copy method" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;

	else if ( PR_FAIL(error=_ExtractName(dst_name,(tPTR)&pdst,sizeof(dst),0,0)) )
		;
	else if ( PR_FAIL(error=OP_getFName(d,psrc)) )
		;
	else
		error = _nfio_copy_file (psrc, pdst, overwrite, &d->nfio_errno);

	if ( pdst != dst )
		CALL_SYS_ObjHeapFree( _this, pdst );

	if ( psrc != src )
		CALL_SYS_ObjHeapFree( _this, psrc );
	
	PR_TRACE_A1( _this, "Leave ObjPtr::Copy method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite ) {
	ObjPtrData* d;
	tERROR      error;
	tIID        new_iid = 0;
	tCHAR       src[MAX_PATH];
	tCHAR       dst[MAX_PATH];
	tCHAR*      psrc = src;
	tCHAR*      pdst = dst;
	tDWORD      size;
	tCHAR       name[MAX_PATH];
	PR_TRACE_A0( _this, "Enter ObjPtr::Rename method" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;

	else if ( PR_FAIL(error=_ExtractName(new_name,(tPTR)&pdst,sizeof(dst),0,&new_iid)) )
		;
	else if ( PR_FAIL(error=OP_getFName(d,psrc)) )
		;
	else if ( PR_FAIL(error=_ren_move(_this,(tPTR)psrc,pdst,overwrite,&d->nfio_errno)) )
		;
	else if ( PR_SUCC(is_folder(pdst,0)) ) { // has to point to renamed object
		size = strlen(pdst);
		memcpy( name, d->dentry->d_name, d->name_len );
		if ( is_relative_path(pdst,size) )
			error = OP_SetRelPath( _this, pdst, size );
		else
			error = OP_SetPath( _this, pdst, &size );
	}
	else {
		tPTR p0;
		size = 0;

		p0 = (tPTR)FindSeparator(pdst, &size);

		if ( !p0 )// just a name
			memcpy( name, pdst, size );

		else { // path with a name
			tDWORD psize = (tCHAR*) p0 - pdst;
			tUINT nsize = size - psize;

			memcpy( name, p0, nsize );
			if ( is_relative_path(pdst,psize) )
				error = OP_SetRelPath( _this, pdst, psize );
			else
				error = OP_SetPath( _this, pdst, &psize );
		}
	}
		
	if ( PR_SUCC(error) )
		error = OP_ChangeTo( _this, name );

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
//  pointer must be advanced to the next object
// Parameters are:
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this ) {
	tERROR error;
	tCHAR  name[MAX_PATH];
	tCHAR* pname = name;
	PR_TRACE_A0( _this, "Enter ObjPtr::Delete method" );

	if ( PR_FAIL(error=OP_getFName(_this->data,pname)) )
		;
	else if ( unlink (name) )
		error = _convert_nfio_errno( _this->data->nfio_errno = errno );
	else
		error = ObjPtr_Next( _this );

	if ( pname != name )
		CALL_SYS_ObjHeapFree( _this, pname );

	PR_TRACE_A1( _this, "Leave ObjPtr::Delete method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// --- Interface "ObjPtr". Method "Reset"
// Extended method comment
//   First call of Next method after Reset must return *FIRST* object in enumeration
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root ) {
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::Reset method" );

	d = _this->data;
	d->state = cObjPtrState_BEFORE;

	if ( d->dirp ) {
		closedir ( d->dirp );
		d->dirp = 0;
		d->nfio_errno = errno;
	}
	else
		d->nfio_errno = 0;

	PR_TRACE_A0( _this, "Leave ObjPtr::Reset method, ret tBOOL = cTRUE, error = errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// --- Interface "ObjPtr". Method "Clone"
// Extended method comment
//   Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameter "parent":
//   Parent of the new enumerator
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result ) {

	hOBJECT    dad;
	tERROR     error;
	hi_ObjPtr  ptr = 0;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::Clone method" );

	d = _this->data;
	d->nfio_errno = 0;

	dad = CALL_SYS_ParentGet( _this, IID_ANY );
	if ( PR_FAIL(error=CALL_SYS_ObjectCreate(dad,(hOBJECT*)&ptr,IID_OBJPTR,PID_NW_NFIO,SUBTYPE_ANY)) )
		;
	else if ( d->path && PR_FAIL(error=CALL_SYS_PropertySetStr(ptr,0,pgOBJECT_PATH,d->path,0,codepage)) )
		;
	else if ( PR_FAIL(error=CALL_SYS_PropertySetStr(ptr,0,pgMASK,d->mask,0,codepage)) )
		;
	else if ( PR_FAIL(error=CALL_SYS_ObjectCreateDone(ptr)) )
		;
	else if ( (d->state == cObjPtrState_BEFORE) || (d->state == cObjPtrState_UNDEFINED) )
		;
	else if ( d->state == cObjPtrState_AFTER ) {
		while( 1 ) {
			error = CALL_ObjPtr_Next(ptr);
			if ( (error == errOUT_OF_OBJECT) || (error == errEND_OF_THE_LIST) ) {
				error = errOK;
				break;
			}
			else if ( PR_FAIL(error) )
				break;
		}
	}
	else {
		tPTR name;
		tDWORD size = MAX_PATH;
		error = CALL_SYS_ObjHeapAlloc( _this, &name, size );
		while( 1 ) {
			error = CALL_ObjPtr_Next(ptr);
			if ( PR_FAIL(error) )
				break;
			if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(ptr,0,pgOBJECT_NAME,name,size,codepage)) )
				break;
			if ( !strcasecmp(d->dentry->d_name,name) )
				break;
		}
	}

	if ( PR_FAIL(error) ) {
		CALL_SYS_ObjectClose( ptr );
		ptr = 0;
	}

	if ( result )
		*result = (hObjPtr)ptr;
	PR_TRACE_A2( _this, "Leave ObjPtr::Clone method, ret hObjPtr = %p, error = 0x%08X", ptr, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// --- Interface "ObjPtr". Method "Next"
// Behaviour comment
//   First call of Next method after creating or Reset must return *FIRST* object in enumeration
// tERROR pr_call ObjPtr_Next( hi_ObjPtr _this ) {
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this ) {
	tERROR      error;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::Next method" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( d->state == cObjPtrState_AFTER )
		error = errEND_OF_THE_LIST;

	else if ( d->state == cObjPtrState_UNDEFINED )
		error = errOBJECT_BAD_INTERNAL_STATE;

	else if ( !d->dirp && (d->dirp = opendir (d->full)) == 0 )
		error = d->nfio_errno = _convert_nfio_errno (errno);

	else {
		error = errOK;
		for ( ;; ) {
		
			d->dentry = readdir (d->dirp);
			
			if ( d->dentry )
			{
				if ( !strcmp(d->dentry->d_name, ".")
				|| !strcmp(d->dentry->d_name, "..")
				|| fnmatch (d->mask, d->dentry->d_name, FNM_PATHNAME|FNM_PERIOD) )
					continue;
				
				d->nfio_errno = 0;
				d->name_len = sizeof(tCHAR) * strlen(d->dentry->d_name) + sizeof(tCHAR);
				d->state = cObjPtrState_PTR;
			}
			else
			{
				d->state = cObjPtrState_AFTER;
				d->nfio_errno = error = errEND_OF_THE_LIST;
			}

			break;
		}
	}
	
	PR_TRACE_A2( _this, "Leave ObjPtr::Next method, ret tERROR = 0x%08X, name = %s", error, d->dentry->d_name );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// --- Interface "ObjPtr". Method "StepUp"
// tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this ) {
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this ) {
	tERROR       error;
	ObjPtrData*  d;
	tDWORD       len;
	tCHAR        name_buff[MAX_PATH];
	PR_TRACE_A0( _this, "Enter ObjPtr::StepUp method" );

	d = _this->data;
	d->nfio_errno = 0;
	error = errEND_OF_THE_LIST;
		
	if ( 0 < (len=strlen(d->path)) )
	{
		tCHAR* end;
		tCHAR* ptr = d->path + len;
	
		if ( (*ptr == '\\') || (*ptr == '/') )
			ptr--;
		end = ptr;
		while ( ptr > d->path ) {
			if ( (*ptr == '\\') || (*ptr == '/') ) {
				memcpy( name_buff, ptr+1, end-ptr-1 );
				name_buff[end-ptr-1] = 0;
				*(ptr+1) = 0;
				strcpy (d->mask, "*.*");
				error = ObjPtr_Reset( _this, cFALSE );
				while( PR_SUCC(error=ObjPtr_Next(_this)) && strcasecmp (d->dentry->d_name,name_buff) )
					;
				break;
			}
			ptr--;
		}
	}

	PR_TRACE_A2( _this, "Leave ObjPtr::StepUp method, ret tERROR = 0x%8x, path = %s", error, d->path );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this ) {
	tUINT pl;
	tUINT nl;
	tERROR error;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::StepDown method" );

	d = _this->data;
	d->nfio_errno = 0;

	if ( (d->state != cObjPtrState_PTR) || !(S_ISDIR (d->dstat.st_mode)) )
		error = errOBJECT_BAD_INTERNAL_STATE;

	else {
		pl = strlen( d->path );
		nl = strlen( d->dentry->d_name );
		if ( (pl+nl+1) > MAX_PATH )
			error = errBUFFER_TOO_SMALL;
		else if ( (d->full_len < (pl+nl+1)) && PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR)&d->full,d->full,(pl+nl+1))) )
			;
		else {
			d->path = d->full;
			memcpy( d->path+pl, d->dentry->d_name, nl );
			strcpy (d->path+pl+nl, "/");
			error = ObjPtr_Reset( _this, cFALSE );
		}
	}

	PR_TRACE_A1( _this, "Leave ObjPtr::StepDown method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// --- Interface "ObjPtr". Method "ChangeTo"
// Parameter "name":
//   The same as "object" parameter of OS::EnumCreate method
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT oname ) {
	tERROR error;
	tCHAR  file_name[MAX_PATH] = {0};
	tCHAR* pfile_name = file_name;
	PR_TRACE_A0( _this, "Enter ObjPtr::ChangeTo method" );

	_this->data->nfio_errno = 0;
	if ( PR_SUCC(error=_ExtractName(oname,(tPTR)&pfile_name,sizeof(file_name),0,0)) ) {
		tUINT  pl;
		tUINT  nl;
		tCHAR  fname[MAX_PATH] = {0};
		tCHAR* pfname = fname;
		ObjPtrData* d = _this->data;

		pl = strlen( d->path );
		nl = strlen( pfile_name ) + 1;

		if ( (pl+nl) > MAX_PATH )
			error = errBUFFER_TOO_SMALL;
		
		if ( PR_SUCC(error) ) {
			memcpy( pfname,    d->full,    pl );
			memcpy( pfname+pl, pfile_name, nl );
			
			if ( PR_SUCC(error=is_folder(pfile_name,&d->nfio_errno)) )
				error = OP_ChangeTo( _this, pfile_name );
		}

		if ( pfname != fname )
			CALL_SYS_ObjHeapFree( _this, pfname );
	}

	if ( pfile_name != file_name )
		CALL_SYS_ObjHeapFree( _this, pfile_name );
	PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret tERROR = 0x%08X", error );
	return error;
}
// AVP Prague stamp end




// ---
tERROR pr_call OP_ChangeTo( hi_ObjPtr _this, tCHAR* name ) {

	ObjPtrData* d = _this->data;
	tERROR error = ObjPtr_Reset( _this, cFALSE );

	if ( PR_FAIL(error) )
		return error;

	while( PR_SUCC(error=ObjPtr_Next(_this)) ) {
		if ( !strcasecmp (d->dentry->d_name,name) )
			return errOK;
	}
	return error;
}




// ---
tERROR pr_call OP_SetMask( hi_ObjPtr _this, tSTRING buffer, tDWORD* size ) {
	ObjPtrData* d = _this->data;

	*size = mb_len( buffer, *size );
	if ( !*size ) {
		strcpy( d->mask, "*.*" );
		*size = 4 * sizeof(tCHAR);
		return errOK;
	}
	else if ( *size > MAX_PATH ) {
		*size = 0;
		return errBUFFER_TOO_SMALL;
	}
    else
    	*size += sizeof(tCHAR);
	
	if ( *size > d->mask_len ) {
		tERROR error;
		if ( d->mask == d->mask_buff )
			error = CALL_SYS_ObjHeapAlloc( _this, (tPTR)&d->mask, *size );
		else
			error = CALL_SYS_ObjHeapRealloc( _this, (tPTR)&d->mask, d->mask, *size );
		if ( PR_SUCC(error) )
			d->mask_len = *size;
		else {
			*size = 0;
			return error;
		}
	}
	
	memcpy( d->mask, buffer, *size );
	return errOK;
}




// ---
static tERROR pr_call OP_SetPath( hi_ObjPtr _this, tSTRING buffer, tDWORD* size ) {
	ObjPtrData*   d = _this->data;
	tDWORD        flen;
	const tWCHAR*	prefix;
	tDWORD        prefix_len;
	
	flen = *size = mb_len( buffer, *size );
	if( (flen>sizeof(tCHAR)) && ( *(buffer+flen-1) == '/') )
		flen -= sizeof(tCHAR);
	flen += sizeof(tCHAR) + sizeof(tCHAR); // slash and terminated zero
	prefix = 0;
	prefix_len = 0;
	
	if ( *size ) {
		if ( d->full_len < flen ) {
			tERROR error = CALL_SYS_ObjHeapAlloc( _this, (tPTR)&d->full, flen );
			if ( PR_SUCC(error) )
				d->full_len = flen;
			else {
				*size = 0;
				return error;
			}
		}

		if ( prefix )
			memcpy( d->full, prefix, prefix_len );
		memcpy( d->full+prefix_len, buffer, *size );

      	if ( *(d->full + (*size) - sizeof(tCHAR)) != '/' )
			  *(d->full + (*size)++) = '/';
			*(d->full + (*size) ) = 0;
			d->path = d->full;
	}
	else if ( d->full ) {
		*d->full = 0;
	}
	return errOK;
}



// ---
tERROR pr_call OP_getFName( ObjPtrData* data, tCHAR* buff ) {
	
	if ( data->path ) {
		tUINT path_len = strlen( data->path );
		if ( (path_len + data->name_len) > MAX_PATH )
			return errBUFFER_TOO_SMALL;
		if ( path_len ) {
			memcpy( buff, data->path, path_len );
			buff += path_len;
		}
	}
	memcpy( buff, data->dentry->d_name, data->name_len );
	
	return errOK;
}


// ---
tERROR pr_call OP_SetRelPath( hi_ObjPtr _this, tPTR rel_path, tDWORD size ) {
	
	tERROR error;
	tPTR   new_path;
	tUINT  osize;
	tUINT  add;

	ObjPtrData* d = _this->data;
	
	osize = d->full ? strlen(d->full) : 0;
	add = sizeof(tCHAR);
	if ( (size+osize+add) > MAX_PATH )
		return errBUFFER_TOO_SMALL;

	error = CALL_SYS_ObjHeapAlloc( _this, &new_path, size+osize+add );
	if ( PR_FAIL(error) )
		return error;
	
	memcpy( new_path,       d->full,  osize );
	memcpy( (char *)new_path+osize, rel_path, size );
	size += osize;
	error = OP_SetPath( _this, new_path, &size );
	CALL_SYS_ObjHeapFree( _this, (tPTR)new_path );
	return error;
}
