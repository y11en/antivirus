// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  19 December 2005,  17:52 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Win32 native file IO
// Author      -- petrovitch
// File Name   -- win32_objptr.c
// Additional info
//    File input/output for Win32
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "win32_objptr.h"
// AVP Prague stamp end



#include "win32_os.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_token.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/plugin/p_win32_nfio.h>

// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- Object Enumerator (Pointer to Object)
//     Defines behavior of an object container/enumerator
//      Supplies object enumerate functionality on one level of hierarchy only.
//      For example: on logical drive object of this interface must enumerate objects in one folder.
//
//      Also must support creating new IO object in the container. There are two ways to do it. First is three steps protocol:
//         -- ObjectCreate system method with folder object as parent
//         -- dictate all necessary properties for the new object
//         -- ObjectCreateDone
//       Second is CreateNew folder's method which is wrapper of above protocol.
//       Advise: when caller invokes ObjectCreateDone method folder object receives ChildDone notification and can do all necessary job to process it.
//
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// data structure "ObjPtrData" is described in "o:\prague\nfio\win32_objptr.c" header file
// Interface "ObjPtr". Inner data structure
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



static tERROR pr_call OP_SetMask( hi_ObjPtr _this, tSTRING buffer, tDWORD* size );
static tERROR pr_call OP_SetPath( hi_ObjPtr _this, tSTRING buffer, tDWORD* size );
static tERROR pr_call OP_SetRelPath( hi_ObjPtr _this, tPTR rel_path, tUINT size );
static tERROR pr_call OP_getFName( ObjPtrData* data, tCHAR* buff );
static tERROR pr_call OP_getFNameW( hi_ObjPtr _this, tCHAR** buff );
static tERROR pr_call OP_getShortFName( ObjPtrData* data, tCHAR* buff );
static tERROR pr_call OP_getShortFNameW( hi_ObjPtr _this, tCHAR** buff );
static tERROR pr_call OP_ChangeTo( hi_ObjPtr _this, tCHAR* name );
static tERROR pr_call OP_InitEnum( hi_ObjPtr _this );
static tERROR pr_call OP_go( hi_ObjPtr _this );

// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
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



// AVP Prague stamp begin( External method table prototypes,  )

// ----------  operations with the current object  ----------
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING p_name, tDWORD p_access_mode, tDWORD p_open_mode );
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT p_dst_name, tBOOL p_overwrite );
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT p_new_name, tBOOL p_overwrite );
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this );

// ----------  navigation methods  ----------
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL p_to_root );
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result );
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT p_name );
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
tERROR pr_call ObjPtr_PROP_GetPropNotImplemented( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_SetPropNotImplemented( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_iGetFileTime( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_iSetFileTime( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetShortFullName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetShortName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetLongFullName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetLongName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



/*
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
const tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime
// AVP Prague stamp end
*/





// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "win32 object enumerator", 24 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, ObjPtrData, origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, ObjPtr_PROP_GetName, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, ObjPtr_PROP_GetPath, ObjPtr_PROP_SetPath )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_GetFPath, NULL )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, ObjPtrData, os, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgIS_FOLDER, ObjPtr_PROP_get_is_folder, NULL )
	mLOCAL_PROPERTY( pgMASK, ObjPtrData, mask, cPROP_BUFFER_READ, NULL, ObjPtr_PROP_SetMask )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PTR_STATE, ObjPtrData, state, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_get_size, ObjPtr_PROP_set_size )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_get_sizeQ, ObjPtr_PROP_set_sizeQ )
	mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, ObjPtr_PROP_get_attribs, ObjPtr_PROP_set_attribs )
	mLOCAL_PROPERTY_FN( pgOBJECT_HASH, ObjPtr_PROP_GetPropNotImplemented, ObjPtr_PROP_SetPropNotImplemented )
	mLOCAL_PROPERTY_FN( pgOBJECT_REOPEN_DATA, ObjPtr_PROP_GetPropNotImplemented, ObjPtr_PROP_SetPropNotImplemented )
	mLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, ObjPtr_PROP_iGetFileTime, ObjPtr_PROP_iSetFileTime )
	mLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, ObjPtr_PROP_iGetFileTime, ObjPtr_PROP_iSetFileTime )
	mLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, ObjPtr_PROP_iGetFileTime, ObjPtr_PROP_iSetFileTime )
	mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME_CP, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_PATH_CP, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME_CP, codepage, sizeof(codepage) )
	mLOCAL_PROPERTY_BUF( pgNATIVE_ERR, ObjPtrData, win32err, cPROP_BUFFER_READ )
	mSHARED_PROPERTY_PTR( plMASK_CP, codepage, sizeof(codepage) )
	mLOCAL_PROPERTY_BUF( plMASK_APPLIED_TO_FOLDERS, ObjPtrData, apply_mask_to_folders, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( plOBJECT_NATIVE_SHORT_FULLNAME, ObjPtr_PROP_GetShortFullName, NULL )
	mLOCAL_PROPERTY_FN( plOBJECT_NATIVE_SHORT_NAME, ObjPtr_PROP_GetShortName, NULL )
	mLOCAL_PROPERTY_FN( plOBJECT_NATIVE_LONG_FULLNAME, ObjPtr_PROP_GetLongFullName, NULL )
	mLOCAL_PROPERTY_FN( plOBJECT_NATIVE_LONG_NAME, ObjPtr_PROP_GetLongName, NULL )
mPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "ObjPtr". Register function
tERROR pr_call ObjPtr_Register( hROOT root ) {
	tERROR error;
	
	PR_TRACE_A0( root, "Enter ObjPtr::Register method" );
	
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OBJPTR,                             // interface identifier
		PID_WIN32_NFIO,                         // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
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
		PR_TRACE( (root,prtDANGER,"nfio\tObjPtr(IID_OBJPTR) registered [error=0x%08x]",error) );
#endif
	
	PR_TRACE_A1( root, "Leave ObjPtr::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


#if defined( _PRAGUE_TRACE_ )

	#define LOCAL_TRACE( _this, err, lvl, op )  _trace( _this, err, lvl, op )
	#define _STR(a) PR_TRACE_PREPARE_STR_CP(a,codepage)

	// ---
	static const tCHAR* _state( ObjPtrData* d ) {
		if ( d->folder_step ) {
			switch( d->state ) {
				case cObjPtrState_UNDEFINED : return "und,fold";
				case cObjPtrState_BEFORE    : return "bef,fold";
				case cObjPtrState_PTR       : return "ptr,fold";
				case cObjPtrState_AFTER     : return "aft,fold";
				default                     : return "unk,fold";
			}
		}
		else {
			switch( d->state ) {
				case cObjPtrState_UNDEFINED : return "und,file";
				case cObjPtrState_BEFORE    : return "bef,file";
				case cObjPtrState_PTR       : return "ptr,file";
				case cObjPtrState_AFTER     : return "aft,file";
				default                     : return "unk,file";
			}
		}
	}

	// ---
	static tVOID pr_call _trace( hi_ObjPtr _this, tERROR err, tTRACE_LEVEL lvl, const tCHAR* op ) {

		tTraceLevel* tl = OBJ_TRACE_LEVEL(_this);

		if ( (tl->min <= lvl) && (lvl <= tl->max) ) {

			tCHAR* fmt;
			ObjPtrData* d = _this->data;
			if ( PR_SUCC(err) ) {
				if ( run_under_nt )
					fmt = "nfio\tobjptr: operation(%s), winerr(%d), state(%s), obj(%S), size(%d), mask(%S)(%S), folder(%S)";
				else
					fmt = "nfio\tobjptr: operation(%s), winerr(%d), state(%s), obj(%s), size(%d), mask(%s)(%s), folder(%s)";
				PR_TRACE(( _this, lvl, fmt,
					 op, d->win32err, _state(d), d->fd.cFileName, d->fd.nFileSizeLow, d->m_generated_mask, d->mask, d->full
				));
			}
			else {
				if ( run_under_nt )
					fmt = "nfio\tobjptr: operation(%s), %terr, winerr(%d), state(%s), obj(%S), size(%d), mask(%S)(%S), folder(%S)";
				else
					fmt = "nfio\tobjptr: operation(%s), %terr, winerr(%d), state(%s), obj(%s), size(%d), mask(%s)(%s), folder(%s)";
				PR_TRACE(( _this, prtERROR, fmt,
					 op, err, d->win32err, _state(d), d->fd.cFileName, d->fd.nFileSizeLow, d->m_generated_mask, d->mask, d->full
				));
			}
		}
	}

#else

	#define LOCAL_TRACE( _this, err, lvl, op )

#endif


// AVP Prague stamp begin( Property method implementation, GetName )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR pr_call ObjPtr_PROP_GetName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetName for property pgOBJECT_NAME" );
	
	d = _this->data;
	d->win32err = 0;
	
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
		memcpy( buffer, d->fd.cFileName, *out_size=d->name_len );
		error = errOK;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetName for property pgOBJECT_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



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
	(void)d->win32err;
	
	if ( d->m_ppath ) {
		if ( run_under_nt ) {
			copy_len = _toui32(UNIS * ( 1 + wcslen(UNI(d->m_ppath)) ));
			full_len = calc_full_path_len( UNI(d->full), UNI(d->m_ppath), copy_len, &prefix );
		}
		else
			full_len = copy_len = _toui32(MBS * ( 1 + strlen(MB(d->m_ppath)) ));
		
		if ( !buffer )
			;
		else if ( size < full_len )
			full_len = 0, error = errBUFFER_TOO_SMALL;
		else {
			if ( prefix ) {
				*UNI(buffer) = *(UNI(buffer)+1) = PATH_DELIMETER_WIDECHAR;
				buffer += 2 * UNIS;
			}
			memcpy( buffer, d->m_ppath, copy_len );
		}
	}
	else if ( run_under_nt ) {
		full_len = UNIS;
		if ( !buffer )
			;
		else if ( size < full_len )
			full_len = 0, error = errBUFFER_TOO_SMALL;
		else
			*UNI(buffer) = 0;
	}
	else if ( !buffer )
		full_len = MBS;
	else if ( size < (full_len=MBS) )
		full_len = 0, error = errBUFFER_TOO_SMALL;
	else
		*buffer = 0;
	
	*out_size = full_len;
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetPath for property pgOBJECT_PATH, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



#if defined(_WIN32) 
	// ---
	tERROR pr_call _to_short_a( ObjPtrData* d, const tCHAR** ppath, tCHAR* dst ) {
		const tCHAR* ptr;
		const tCHAR* path;
		tCHAR        tmp[2*MAX_PATH];
		tUINT        len;

		if ( !fGetShortPathName )
			return errNOT_SUPPORTED;

		path = *ppath;
		if ( (*(path+1)==':') && (*(path+2)=='\\') ) // == "a:\..."
			ptr = strchr( path+3, '\\' );
		else if ( (*(path+0)=='\\') && (*(path+1)=='\\') ) { // == \\server\share\..
			ptr = strchr( path+2, '\\' );
			if ( !ptr )
				return errPARAMETER_INVALID;
			ptr = strchr( ptr+1, '\\' );
			if ( !ptr )
				return errPARAMETER_INVALID;
			ptr = strchr( ptr+1, '\\' );
		}
		else
			return errPARAMETER_INVALID;

		if ( !ptr )
			ptr = path + lstrlen( path );

		len = 0;
		for(;;) {
			tUINT clen = _toui32(ptr - path);
			if ( clen > (countof(tmp)-len) )
				return errBUFFER_TOO_SMALL;
			memcpy( tmp+len, path, clen );
			*(tmp+len+clen) = 0;
			len = fGetShortPathName( tmp, dst, MAX_PATH );
			if ( !len ) {
				d->win32err = GetLastError();
				return _ConvertWin32Err( d->win32err );
			}
			if ( !*ptr ) {
				*ppath = dst;
				return errOK;
			}
			path = ptr + 1;
			ptr = strchr( path, '\\' );
			if ( !ptr )
				ptr = path + lstrlen( path );
			memcpy( tmp, dst, len );
			if ( *(tmp+len-1) != '\\' )
				*(tmp+len++) = '\\';
		}
	}



	// ---
	tERROR pr_call _to_short_w( ObjPtrData* d, const tWCHAR** ppath, tWCHAR* dst ) {
		const tWCHAR* ptr;
		const tWCHAR* path;
		tWCHAR        tmp[2*MAX_PATH];
		tUINT         len;

		if ( !fGetShortPathName )
			return errNOT_SUPPORTED;

		path = *ppath;
		if ( (*(path+1)==L':') && (*(path+2)==L'\\') )
			ptr = wcschr( path+3, L'\\' );
		else if ( (*(path+0)==L'\\') && (*(path+1)==L'\\') ) { // == \\server\share\..
			tUINT adv = 0;
			len = _toui32(UNIS * (1+ wcslen(path)));
			if ( !get_prefix(UNI(path),len,&adv,&len) )
				return errPARAMETER_INVALID;
			if ( adv > 2 )
				ptr = wcschr( path+adv, L'\\' );
			else {
				ptr = wcschr( path+2, L'\\' );
				if ( !ptr )
					return errPARAMETER_INVALID;
				ptr = wcschr( ptr+1, L'\\' );
				if ( !ptr )
					return errPARAMETER_INVALID;
				ptr = wcschr( ptr+1, L'\\' );
			}
		}
		if ( !ptr )
			ptr = path + wcslen( path );

		len = 0;
		for(;;) {
			tUINT clen = _toui32(ptr - path);
			if ( clen > (countof(tmp)-len) )
				return errBUFFER_TOO_SMALL;
			if ( clen )
				memcpy( tmp+len, path, clen*UNIS );
			*(tmp+len+clen) = 0;
			len = fGetShortPathName( tmp, dst, MAX_PATH );
			if ( !len ) {
				d->win32err = GetLastError();
				return _ConvertWin32Err( d->win32err );
			}
			if ( !*ptr ) {
				*ppath = dst;
				return errOK;
			}
			path = ptr + 1;
			ptr = wcschr( path, L'\\' );
			if ( !ptr )
				ptr = path + wcslen( path );
			memcpy( tmp, dst, UNIS*len );
			if ( *(tmp+len-1) != L'\\' )
				*(tmp+len++) = L'\\';
		}
	}




	// ---
	tUINT _get_long_name_sect_a( const tCHAR* src, tCHAR* dst, tUINT dst_len, tBOOL last_sect ) {
		tUINT pl, nl;
		WIN32_FIND_DATA fd;
		HANDLE fh;

		if ( last_sect && fGetLongPathName )
			return fGetLongPathName( src, dst, dst_len );

		pl = 0; nl = 0;
		fh = fFindFirstFile( src, &fd );
		if ( fh ) {
			const tCHAR* ps;

			fFindClose( fh );
			ps = strrchr( src, '\\' );
			
			if ( ps )
				++ps;
			else
				ps = src;
			pl = _toui32(ps - src);
			nl = _toui32(strlen( fd.cFileName ));
			if ( (pl+nl) <= dst_len ) {
				memcpy( dst,    src,          pl );
				memcpy( dst+pl, fd.cFileName, nl+1 );
			}
		}
		return pl + nl;
	}



	// ---
	tERROR pr_call _to_long_a( const tCHAR* src, tCHAR* dst, tUINT dst_len, tDWORD* out_len, tDWORD* win32err ) {
		const tCHAR* ptr;
		tCHAR        tmp[2*MAX_PATH];
		tUINT        len;
		tBOOL        logical_drive = cFALSE;

		*out_len = 0;

		if ( (*(src+1)==':') && (*(src+2)=='\\') ) { // == "a:\..."
			ptr = strchr( src+3, '\\' );
			logical_drive = cTRUE;
		}
		else if ( (*(src+0)=='\\') && (*(src+1)=='\\') ) { // == \\server\share\..
			ptr = strchr( src+2, '\\' );
			if ( !ptr )
				return errPARAMETER_INVALID;
			ptr = strchr( ptr+1, '\\' );
			if ( !ptr )
				return errPARAMETER_INVALID;
			ptr = strchr( ptr+1, '\\' );
		}
		else
			return errPARAMETER_INVALID;

		if ( !ptr )
			ptr = src + lstrlen( src );

		len = 0;
		for(;;) {
			tUINT clen = _toui32(ptr - src);
			if ( clen > (countof(tmp)-len) )
				return errBUFFER_TOO_SMALL;
			memcpy( tmp+len, src, clen );
			*(tmp+len+clen) = 0;
			len = _get_long_name_sect_a( tmp, dst, MAX_PATH, !*ptr );
			if ( !len ) {
				tDWORD win_err = GetLastError();
				if ( win32err )
					*win32err = win_err;
				return _ConvertWin32Err( win_err );
			}
			*out_len = len + clen;
			if ( !*ptr ) {
				if ( logical_drive && islower(*dst) )
					*dst = toupper(*dst);
				*out_len += 1;
				return errOK;
			}
			src = ptr + 1;
			ptr = strchr( src, '\\' );
			if ( !ptr )
				ptr = src + lstrlen( src );
			memcpy( tmp, dst, len );
			if ( *(tmp+len-1) != '\\' )
				*(tmp+len++) = '\\';
		}
	}


	#define TMP_BUFF_SIZE MAX_PATH

	// ---
	tUINT _get_long_name_sect_w( const tWCHAR* src, tWCHAR* dst, tUINT dst_len, tBOOL last_sect ) {
		tUINT  pl, nl;
		HANDLE fh;
		WIN32_FIND_DATAW fd;

		if ( last_sect && fGetLongPathName )
			return fGetLongPathName( src, dst, dst_len );

		pl = 0, nl = 0;
		fh = fFindFirstFile( src, &fd );
		if ( fh ) {
			const tWCHAR* ps;

			fFindClose( fh );
			ps = wcsrchr( src, '\\' );
			
			if ( ps )
				++ps;
			else
				ps = src;
			pl = _toui32(ps - src);
			nl = _toui32(wcslen(fd.cFileName));
			if ( (pl+nl) <= dst_len ) {
				memcpy( dst,    src,          UNIS*pl );
				memcpy( dst+pl, fd.cFileName, UNIS*(nl+1) );
			}
		}
		return pl + nl;
	}


	// ---
	tERROR pr_call _to_long_w( hOBJECT obj, const tWCHAR* src, tWCHAR** dst, tUINT* dst_len, tDWORD* out_len, tDWORD* win32err ) {
		tERROR        err;
		const tWCHAR* ptr;
		tWCHAR        tmp[2*TMP_BUFF_SIZE];
		tWCHAR*       ptmp = tmp;
		tUINT         ptmp_len = countof(tmp);
		tUINT         len;
		tBOOL         dst_allocated = cFALSE;
		tBOOL         logical_drive = cFALSE;

		*out_len = 0;
		if ( win32err )
			*win32err = ERROR_SUCCESS;

		if ( (*(src+1)==L':') && (*(src+2)==L'\\') ) {
			ptr = wcschr( src+3, L'\\' );
			logical_drive = cTRUE;
		}
		else if ( (*src==L'\\') && (*(src+1)==L'\\') ) { // == \\server\share\..
			tUINT adv = 0;
			len = _toui32(UNIS * (1+ wcslen(src)));
			if ( !get_prefix(UNI(src),len,&adv,&len) )
				return errPARAMETER_INVALID;
			if ( adv > 2 )
				ptr = wcschr( src+adv, L'\\' );
			else {
				ptr = wcschr( src+2, L'\\' );
				if ( !ptr )
					return errPARAMETER_INVALID;
				ptr = wcschr( ptr+1, L'\\' );
				if ( !ptr )
					return errPARAMETER_INVALID;
				ptr = wcschr( ptr+1, L'\\' );
			}
		}
		if ( !ptr )
			ptr = src + wcslen( src );

		err = errOK;
		len = 0;
		dst_allocated = cFALSE;

		for(;;) {
			tUINT clen = _toui32(ptr - src);
			tUINT dlen = *dst_len / UNIS;

			if ( (len+clen) > ptmp_len ) {
				tPTR  mem;
				tUINT new_len = len + clen;
				new_len = TMP_BUFF_SIZE * ((new_len / TMP_BUFF_SIZE) + !!(new_len % TMP_BUFF_SIZE));
				err = CALL_SYS_ObjHeapAlloc( obj, &mem, UNIS*new_len );
				if ( PR_FAIL(err) )
					break;
				memcpy( mem, ptmp, UNIS*len );
				if ( ptmp != tmp )
					CALL_SYS_ObjHeapFree( obj, ptmp );
				ptmp_len = new_len;
				ptmp = mem;
			}

			if ( clen )
				memcpy( ptmp+len, src, clen*UNIS );

			*(ptmp+len+clen) = 0;

			do {
				tPTR   mem;
				len = _get_long_name_sect_w( ptmp, *dst, dlen, !*ptr );
				if ( !len ) {
					tDWORD win_err = GetLastError();
					if ( win32err )
						*win32err = win_err;
					err = _ConvertWin32Err( win_err );
					break;
				}
				if ( len <= dlen )
					break;
				dlen = TMP_BUFF_SIZE * (((1+dlen) / TMP_BUFF_SIZE) + !!((1+dlen) % TMP_BUFF_SIZE));
				err = CALL_SYS_ObjHeapAlloc( obj, &mem, UNIS * dlen );
				if ( PR_FAIL(err) )
					break;
				if ( dst_allocated )
					CALL_SYS_ObjHeapFree( obj, *dst );
				dst_allocated = cTRUE;
				*dst = (tWCHAR*)mem;
				*dst_len = UNIS * dlen;
			} while( 1 );

			*out_len = UNIS * len;
			if ( !*ptr ) {
				*out_len += UNIS;
				break;
			}
			else if ( PR_FAIL(err) )
				break;

			src = ptr + 1;
			ptr = wcschr( src, L'\\' );
			if ( !ptr )
				ptr = src + wcslen( src );
			memcpy( ptmp, *dst, UNIS*len );
			if ( *(ptmp+len-1) != L'\\' )
				*(ptmp+len++) = L'\\';
		}
		if ( ptmp != tmp )
			CALL_SYS_ObjHeapFree( obj, ptmp );
		if ( PR_SUCC(err) && logical_drive && iswlower(**dst) )
			**dst = towupper( **dst );
		return err;
	}

#endif


// AVP Prague stamp begin( Property method implementation, SetPath )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- pgOBJECT_PATH
tERROR pr_call ObjPtr_PROP_SetPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	ObjPtrData* d;
	PR_TRACE_A1( _this, "Enter ObjPtr::PROP_SetPath(%s)", buffer ? buffer : "NULL" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( !buffer ) {
		if ( run_under_nt )
			size = UNIS * MAX_PATH;
		else
			size = MBS * MAX_PATH;
		error = errOK;
	}
	else {
		#if defined(_WIN32)
			tCHAR short_name[MAX_PATH];
		#endif
		/*  commented 11.02.2005 by Pavel Mezhuev	
			else if ( run_under_nt && (size > (UNIS*MAX_PATH)) ) {
				size = 0;
				error = errPARAMETER_INVALID;
			}
		else */ if ( !run_under_nt && (size > (MBS*MAX_PATH)) && PR_FAIL(error=_to_short_a(d,&buffer,short_name)) ) {
			size = 0;
			//error = errPARAMETER_INVALID;
		}
		
		else if ( PR_FAIL(error=OP_SetPath(_this,buffer,&size)) )
			;
		else if ( d->handle )
			error = ObjPtr_Reset( _this, cFALSE );
	}

	//LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "set_path" );

	*out_size = size;
	PR_TRACE_A2( _this, "Leave ObjPtr::PROP_SetPath, ret tDWORD = %u(size), error = 0x%08X", *out_size, error );
	return error;
	
}
// AVP Prague stamp end




// AVP Prague stamp begin( Property method implementation, GetFPath )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call ObjPtr_PROP_GetFPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	tUINT  path_len;
	tUINT  full_len;
	tBOOL  prefix;
	tDWORD lsize;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetFPath for property pgOBJECT_FULL_NAME" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR ) {
		*out_size = 0;
		error = errOBJECT_BAD_INTERNAL_STATE;
		goto exit;
	}
	
	if ( !out_size )
		out_size = &lsize;
	
	prefix = cFALSE;
	if ( run_under_nt ) {
		if ( d->m_ppath ) {
			path_len = _toui32(UNIS * wcslen(UNI(d->m_ppath)));
			full_len = calc_full_path_len( UNI(d->full), UNI(d->m_ppath), path_len, &prefix );
		}
		else
			path_len = full_len = 0;
		full_len += d->name_len;
	}
	else if ( d->m_ppath ) {
		path_len = lstrlen( MB(d->m_ppath) );
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
			*UNI(buffer) = *(UNI(buffer)+1) = PATH_DELIMETER_WIDECHAR;
			buffer += 2 * UNIS;
		}
		if ( path_len )
			memcpy( buffer, d->m_ppath, path_len );
		memcpy( buffer+path_len, d->fd.cFileName, d->name_len );
		*out_size = full_len;
		error = errOK;
	}
	
exit:;
	 PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetFPath for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	 return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, get_is_folder )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- IS_FOLDER
tERROR pr_call ObjPtr_PROP_get_is_folder( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_is_folder for property pgIS_FOLDER" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR ) {
		error = errOBJECT_BAD_INTERNAL_STATE;
		*out_size = 0;
	}
	else {
		error = errOK;
		*out_size = sizeof(tBOOL);
		if ( buffer ) {
			if ( d->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				*((tBOOL*)buffer) = cTRUE;
			else
				*((tBOOL*)buffer) = cFALSE;
		}
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_is_folder for property pgIS_FOLDER, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end




// AVP Prague stamp begin( Property method implementation, SetMask )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- MASK
tERROR pr_call ObjPtr_PROP_SetMask( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr::PROP_SetMask for property pgMASK" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( !buffer ) {
		if ( run_under_nt )
			size = UNIS * MAX_PATH;
		else
			size = MAX_PATH;
		error = errOK;
	}
	else if ( (run_under_nt && (size > (UNIS*MAX_PATH))) || (!run_under_nt && (size > MAX_PATH)) ) {
		size = 0;
		error = errBUFFER_TOO_SMALL;
	}
	else {
		tCHAR* p0 = run_under_nt ? ((tCHAR*)FindSeparatorW( UNI(buffer), &size )) : FindSeparator(buffer,&size);
		if ( !p0 )                                               // there is only name in the buffer
			error = OP_SetMask( _this, buffer, &size );
		else if ( *p0 == 0 ) {                                   // there is only path in the buffer
			size = _toui32(MB(p0) - MB(buffer));
			error = OP_SetPath( _this, buffer, &size );
		}
		else {                                                    // there are path and name in the buffer
			tDWORD psize = _toui32(MB(p0)-MB(buffer));
			tDWORD msize = size - psize;
			if ( PR_FAIL(error = OP_SetPath(_this,buffer,&psize)) )
				;
			else
				error = OP_SetMask( _this, buffer+(MB(p0)-MB(buffer)), &msize );
		}
		if ( PR_SUCC(error) && d->handle )
			error = ObjPtr_Reset( _this, cFALSE );
	}
	
	//LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "set_mask" );

	PR_TRACE_A3( _this, "Leave ObjPtr::PROP_SetMask method, ret hUINT = %u, error = 0x%08X, mask = %s", size, error, d->mask );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, get_size )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_get_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_size for property pgOBJECT_SIZE" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR ) {
		error = errOBJECT_BAD_INTERNAL_STATE;
		*out_size = 0;
	}
	else {
		error = errOK;
		*out_size = sizeof(tDWORD);
		if ( buffer ) {
			if ( d->fd.nFileSizeHigh ) {
				error = errBUFFER_TOO_SMALL;
				*out_size = 0;
			}
			else
				*((tDWORD*)buffer) = d->fd.nFileSizeLow;
		}
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_size for property pgOBJECT_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, set_size )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_set_size( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	tCHAR  path[MAX_PATH];
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_size for property pgOBJECT_SIZE" );
	
	_this->data->win32err = 0;
	if ( !buffer ) {
		error = errPARAMETER_INVALID;
		*out_size = 0;
	}
	else if ( run_under_nt ) {
		tCHAR* buff = path;
		error = OP_getFNameW( _this, &buff );
		if ( PR_SUCC(error) )
			error = errNOT_IMPLEMENTED;
		if ( buff != path )
			CALL_SYS_ObjHeapFree( _this, buff );
	}
	else if ( PR_SUCC(error=OP_getFName(_this->data,path)) )
		error = errNOT_IMPLEMENTED;
	
	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_size for property pgOBJECT_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



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
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR ) {
		error = errOBJECT_BAD_INTERNAL_STATE;
		*out_size = 0;
	}
	else {
		error = errOK;
		*out_size = sizeof(tQWORD);
		if ( buffer ) {
			((tqw*)buffer)->h = d->fd.nFileSizeHigh;
			((tqw*)buffer)->l = d->fd.nFileSizeLow;
		}
	}
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "get_sizeQ" );

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_sizeQ for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, set_sizeQ )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_set_sizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	tCHAR  path[MAX_PATH];
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_set_sizeQ for property pgOBJECT_SIZE_Q" );
	
	_this->data->win32err = 0;
	
	if ( !buffer ) {
		error = errPARAMETER_INVALID;
		*out_size = 0;
	}
	else if ( run_under_nt ) {
		tCHAR* buff = path;
		if ( PR_SUCC(error=OP_getFNameW(_this,&buff)) )
			error = errNOT_IMPLEMENTED;
		if ( buff != path )
			CALL_SYS_ObjHeapFree( _this, buff );
	}
	else if ( PR_SUCC(error=OP_getFName(_this->data,path)) )
		error = errNOT_IMPLEMENTED;
	
	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_sizeQ for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, get_attribs )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_get_attribs( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	ObjPtrData* d;
	tERROR      error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_get_attribs for property pgOBJECT_ATTRIBUTES" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR ) {
		*out_size = 0;
		error = errOBJECT_BAD_INTERNAL_STATE;
	}
	else {
		error = errOK;
		*out_size = sizeof(tDWORD);
		if (size==0 && buffer==NULL)
			error = errOK;
		else if (size < sizeof(tDWORD))
			error = errBUFFER_TOO_SMALL;
		else if ( buffer )
			*((tDWORD*)buffer) = d->fd.dwFileAttributes;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_get_attribs for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



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
	d->win32err = 0;
	
	*out_size = 0;
	
	if ( !fSetFileAttributes )
		error = errINTERFACE_NOT_LOADED;
	
	else if ( !buffer )
		error = errPARAMETER_INVALID;
	
	else if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;
	
	else if ( run_under_nt )
		error = OP_getFNameW( _this, &buff );
	
	else
		error = OP_getFName( d, buff );
	
	if ( PR_SUCC(error) ) {
		if ( fSetFileAttributes(buff,*(tDWORD*)buffer) ) {
			d->fd.dwFileAttributes = *(tDWORD*)buffer;
			*out_size = sizeof(tDWORD);
		}
		else
			error = _ConvertWin32Err( d->win32err = GetLastError() );
	}
	
	if ( buff != name )
		CALL_SYS_ObjHeapFree( _this, buff );
	
	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_set_attribs for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetPropNotImplemented )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_HASH
//  -- OBJECT_REOPEN_DATA
tERROR pr_call ObjPtr_PROP_GetPropNotImplemented( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method GetPropNotImplemented" );

	error = errPROPERTY_NOT_IMPLEMENTED;

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method GetPropNotImplemented, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetPropNotImplemented )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_HASH
//  -- OBJECT_REOPEN_DATA
tERROR pr_call ObjPtr_PROP_SetPropNotImplemented( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter *SET* multyproperty method SetPropNotImplemented" );

	error = errPROPERTY_NOT_IMPLEMENTED;

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method SetPropNotImplemented, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end




// AVP Prague stamp begin( Property method implementation, iGetFileTime )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_CREATION_TIME
//  -- OBJECT_LAST_WRITE_TIME
//  -- OBJECT_LAST_ACCESS_TIME
tERROR pr_call ObjPtr_PROP_iGetFileTime( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	FILETIME* pft;
	tDATETIME dt;
	ObjPtrData* d;
	tCHAR  name[MAX_PATH];
	tCHAR* buff = name;
	WIN32_FILE_ATTRIBUTE_DATA fad;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method iGetFileTime" );
	
	d = _this->data;
	d->win32err = 0;

	*out_size = sizeof(tDATETIME);
	if (size == 0 && buffer==NULL)
		return errOK;
	if (size < sizeof(tDATETIME) && buffer!=NULL)
		return errBUFFER_TOO_SMALL;
	if (!DTSet || !fGetFileAttributesEx)
		return errINTERFACE_NOT_LOADED;

	if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;
	else if ( run_under_nt )
		error = OP_getFNameW( _this, &buff );
	else
		error = OP_getFName( d, buff );


	if ( PR_SUCC(error) )	{
		if( !fGetFileAttributesEx(buff,GetFileExInfoStandard, &fad))
			error = _ConvertWin32Err( d->win32err = GetLastError());
	}

	if (PR_SUCC(error))
	{
		switch ( prop ) {
		case pgOBJECT_CREATION_TIME:
			pft = &fad.ftCreationTime;
			break;
		case pgOBJECT_LAST_WRITE_TIME:
			pft = &fad.ftLastWriteTime;
			break;
		case pgOBJECT_LAST_ACCESS_TIME:
			pft = &fad.ftLastAccessTime;
			break;
		default:
			*out_size = 0;
			error = errPROPERTY_NOT_IMPLEMENTED;
			break;
		}
	}

	if (PR_SUCC(error))
	{
		SYSTEMTIME st;
		if (!FileTimeToSystemTime(pft, &st))
			error = _ConvertWin32Err( d->win32err = GetLastError() );
		else
			error = DTSet(&dt, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds*1000000);
	}

	if (PR_SUCC(error))
	{
		*out_size = sizeof(tDATETIME);
		if (buffer)
			memcpy(buffer, dt, sizeof(tDATETIME));
	}

	PR_TRACE_A2( this, "Leave *GET* multyproperty method iGetFileTime, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, iSetFileTime )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_iSetFileTime( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter *SET* multyproperty method iSetFileTime" );

	error = errNOT_IMPLEMENTED;

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method iSetFileTime, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetShortFullName )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_NATIVE_SHORT_FULLNAME
tERROR pr_call ObjPtr_PROP_GetShortFullName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR err;
	tDWORD lsize = 0;
	tWCHAR lname[MAX_PATH];
	tPTR   plname = lname;
	tWCHAR short_name[MAX_PATH];
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetShortFullName for property plOBJECT_NATIVE_SHORT_PATH" );
	
	*out_size = 0;

	err = ObjPtr_PROP_GetFPath( _this, &lsize, pgOBJECT_FULL_NAME, MB(lname), sizeof(lname) );
	if ( err == errBUFFER_TOO_SMALL ) {
		err = CALL_SYS_ObjHeapAlloc( _this, &plname, lsize );
		if ( PR_SUCC(err) )
			err = ObjPtr_PROP_GetFPath( _this, &size, pgOBJECT_FULL_NAME, plname, lsize );
	}

	if ( PR_SUCC(err) ) {
		ObjPtrData* d = _this->data;
		if ( run_under_nt ) {
			tWCHAR* name = UNI(plname);
			err = _to_short_w( d, &name, short_name );
			if ( PR_SUCC(err) )
				lsize = _toui32(UNIS * (1 + wcslen(UNI(short_name))));
		}
		else {
			tCHAR* name = MB(plname);
			err = _to_short_a( d, &name, MB(short_name) );
			if ( PR_SUCC(err) )
				lsize = UNIS * (1 + lstrlen(MB(short_name)));
		}
		if ( PR_SUCC(err) ) {
			*out_size = lsize;
			if ( buffer ) {
				if ( size < lsize )
					err = errBUFFER_TOO_SMALL;
				else
					memcpy( buffer, short_name, lsize );
			}
		}
	}

	if ( plname != lname )
		CALL_SYS_ObjHeapFree( _this, plname );

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetShortFullName, ret tDWORD = %u(size), %terr", *out_size, err );
	return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetShortName )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_NATIVE_SHORT_NAME
tERROR pr_call ObjPtr_PROP_GetShortName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR err = errOK;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetShortName for property plOBJECT_NATIVE_SHORT_PATH" );
	
	d = _this->data;
	
	if ( d->state != cObjPtrState_PTR )
		err = errOBJECT_BAD_INTERNAL_STATE;
	else if ( run_under_nt ) {
		WIN32_FIND_DATAW* fd = (WIN32_FIND_DATAW*)&d->fd;
		tWCHAR* p = fd->cAlternateFileName;
		if ( !*p )
			p = fd->cFileName;
		*out_size = _toui32(UNIS * (1 + wcslen(p)));
		if ( buffer ) {
			if ( size < *out_size )
				err = errBUFFER_TOO_SMALL;
			else
				memcpy( buffer, p, *out_size );
		}
	}
	else {
		WIN32_FIND_DATAA* fd = (WIN32_FIND_DATAA*)&d->fd;
		tCHAR* p = fd->cAlternateFileName;
		if ( !*p )
			p = fd->cFileName;
		*out_size = MBS * (1 + lstrlen(p));
		if ( buffer ) {
			if ( size < *out_size )
				err = errBUFFER_TOO_SMALL;
			else
				memcpy( buffer, p, *out_size );
		}
	}

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetShortName, ret tDWORD = %u(size), %terr", *out_size, err );
	return err;
}



// AVP Prague stamp begin( Property method implementation, GetShortPath )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_NATIVE_SHORT_PATH
tERROR pr_call ObjPtr_PROP_GetLongFullName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR err;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetLongFullName for property plOBJECT_NATIVE_LONG_FULLNAME" );

	err = ObjPtr_PROP_GetFPath( _this, out_size, pgOBJECT_FULL_NAME, buffer, size );

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetLongFullName, ret tDWORD = %u(size), %terr", *out_size, err );
	return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetShortName )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_NATIVE_SHORT_NAME
tERROR pr_call ObjPtr_PROP_GetLongName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR err;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetLongName for property plOBJECT_NATIVE_LONG_NAME" );

	err = ObjPtr_PROP_GetName( _this, out_size, pgOBJECT_NAME, buffer, size );

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetLongName, ret tDWORD = %u(size), %terr", *out_size, err );
	return err;
}



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
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInit method" );
	
	d = _this->data;
	d->win32err = 0;
	d->origin = OID_GENERIC_IO;
	//d->m_do_not_make_prefix = cTRUE;
	
	if ( !fFindFirstFile || !fFindNextFile ) {
		PR_TRACE(( _this, prtERROR, "nfio\tFindFirstFile and/or FindNextFile export pointer(s) cannot be exported from the Windows kernel" ));
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	
	else {
		hOBJECT dad = CALL_SYS_ParentGet( _this, IID_ANY );
		if (
			(IID_OS         == CALL_SYS_PropertyGetDWord(dad,pgINTERFACE_ID)) &&
			(PID_WIN32_NFIO == CALL_SYS_PropertyGetDWord(dad,pgPLUGIN_ID))
		)
			d->os = (hOS)dad;
		
		d->mask = d->mask_buff;
		if ( run_under_nt ) {
			memcpy( d->mask, L"*", 2*UNIS );
			d->name_len = UNIS;
		}
		else {
			memcpy( d->mask, "*", 2*MBS );
			d->name_len = MBS;
		}
		error = errOK;
	}
	
	
	PR_TRACE_A0( _this, "Leave ObjPtr::ObjectInit method, ret tERROR = errOK" );
	return error;
}
// AVP Prague stamp end



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
	if ( !d->handle ) {
		d->win32err = 0;
		e = ObjPtr_Reset( _this, cFALSE );
	}
	else
		e = errOK;
	
	LOCAL_TRACE( _this, e, prtNOT_IMPORTANT, "init_done" );

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInitDone method, ret tERROR = 0x%08x", e );
	return e;
}
// AVP Prague stamp end



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

	LOCAL_TRACE( _this, errOK, prtNOT_IMPORTANT, "close" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( d->handle ) {
		if ( !fFindClose || fFindClose(d->handle) )
			d->win32err = NO_ERROR;
		else
			d->win32err = GetLastError();
		d->handle = 0;
		d->name_len = 0;
	}

	#if defined( _PRAGUE_TRACE_ )
		if ( d->m_generated_mask )
			CALL_SYS_ObjHeapFree( _this, (tPTR)d->m_generated_mask );
	#endif

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




// ---
tERROR pr_call _make_file_name( hi_ObjPtr _this, hSTRING name, tPTR* pobj_name, tUINT* pobj_size ) {
	tERROR err = errOK;
	ObjPtrData* d = _this->data;
	tDWORD name_len = 0;
	tUINT  path_len;
	if ( d->full ) {
		if ( run_under_nt )
			path_len = _toui32(wcslen( UNI(d->full) )*UNIS);	//Andy: This function returns number of SYMBOLS not bytes in string. So we have to multiply it to sizeof(WCHAR)
		else
			path_len = _toui32(lstrlen( MB(d->full) ));
	}
	else
		path_len = 0;
	if ( PR_FAIL(err=CALL_String_LengthEx(name,&name_len,cSTRING_WHOLE,codepage,cSTRING_Z)) )
		;
	else if ( ((path_len+name_len) > *pobj_size) && PR_FAIL(err=CALL_SYS_ObjHeapAlloc(_this,pobj_name,path_len+name_len)) )
		;
	else {
		*pobj_size = path_len+name_len;
		memcpy( *pobj_name, d->full, path_len );
		err = CALL_String_ExportToBuff( name, 0, cSTRING_WHOLE, MB(*pobj_name)+path_len, name_len, codepage, cSTRING_Z );
	}

	return err;
}


// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  Creates hIO or hObjPtr object current in enumeration.
// Result comment
//  Returns hIO or hObjPtr object. Depends on current object.
// Parameters are:
// "access_mode" : New object open mode
// "open_mode"   : Attributes of the new object
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ) {

	PR_TRACE_VARS;
	ObjPtrData* d;
	tERROR      error;
	hIO         ret_val = NULL;
	PR_TRACE_A0( _this, "Enter ObjPtr::IOCreate method" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( !name && (d->state != cObjPtrState_PTR) )
		error = errOBJECT_BAD_INTERNAL_STATE;
	
	else {
		tCHAR  obj_name[MAX_PATH];
		tCHAR* pobj_name = obj_name;
		tUINT  pobj_size = sizeof(obj_name);
		
		if ( name )
			error = _make_file_name( _this, name, &pobj_name, &pobj_size );
		else if ( run_under_nt )
			error = OP_getFNameW( _this, &pobj_name );
		else
			error = OP_getFName( d, pobj_name );
		
		if ( d->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			open_mode |= fOMODE_FOLDER_AS_IO;
		
		if ( PR_FAIL(error) )
			;
		else if ( PR_FAIL(error=CALL_SYS_ObjectCreate(_this,(hOBJECT*)&ret_val,IID_IO,PID_WIN32_NFIO,SUBTYPE_ANY)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_OPEN_MODE,open_mode)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_PropertySetDWord(ret_val,pgOBJECT_ACCESS_MODE,access_mode)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_PropertySetStr(ret_val,0,pgOBJECT_NAME,pobj_name,0,codepage)) )
			;
		else if ( PR_SUCC(error=CALL_SYS_ObjectCreateDone(ret_val)) )
			;
		else if ( /*!run_under_nt &&*/ !name && (error == errOBJECT_NOT_FOUND) ) {
			tWCHAR  short_name[MAX_PATH];
			tWCHAR* pshort = short_name;

			//tCHAR* p_name = pobj_name;
			//error = _to_short_a( d, &p_name, short_name );
			if ( run_under_nt )
				error = OP_getShortFNameW( _this, &MB(pshort) );
			else
				error = OP_getShortFName( d, MB(pshort) );
			if ( PR_SUCC(error) ) {
				PR_TRACE(( _this, prtIMPORTANT, "nfio\tobjptr:CreateIO failed (errOBJECT_NOT_FOUND), try open by short name \"%s%S\"", PR_TRACE_PREPARE_STR_CP(pshort,codepage)) );
				error = CALL_SYS_PropertySetStr( ret_val, 0, pgOBJECT_NAME, pshort, 0, codepage );
			}
			else {
				PR_TRACE(( _this, prtIMPORTANT, "nfio\tobjptr:CreateIO, getShortName failed (%terr) for \"%s%S\"", error, PR_TRACE_PREPARE_STR_CP(pobj_name,codepage) ));
			}

			if ( PR_SUCC(error) ) {
				CALL_SYS_PropertySetBool( ret_val, plSHORT_NAME_4_REOPEN, cTRUE );
				error = CALL_SYS_ObjectCreateDone( ret_val );
			}
			PR_TRACE(( _this, PR_TRACE_COND_LEVEL(error,prtIMPORTANT,prtERROR), "nfio\tobjptr:CreateIO by short_name %terr - \"%s%S\"", error, PR_TRACE_PREPARE_STR_CP(pshort,codepage) ));
			if ( pshort != short_name )
				CALL_SYS_ObjHeapFree( _this, pshort );
		}
		else {
			PR_TRACE(( _this, prtERROR, "nfio\tobjptr:CreateIO failed (%terr) by \"%s%S\"", error, PR_TRACE_PREPARE_STR_CP(pobj_name,codepage) ));
		}
		
		if ( PR_FAIL(error) && ret_val ) {
			CALL_SYS_ObjectClose( ret_val );
			ret_val = 0;
		}
		
		if ( pobj_name != obj_name )
			CALL_SYS_ObjHeapFree( _this, pobj_name );
	}
	
	if ( result )
		*result = ret_val;

	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "IOCreate" );

	PR_TRACE_A2( _this, "Leave ObjPtr::IOCreate method, ret hIO = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



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
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;
	
	else if ( !fCopyFile ) {
		PR_TRACE(( _this, prtERROR, "nfio\tCopyFile export pointer cannot be imported from the Windows kernel" ));
		error = errINTERFACE_NOT_LOADED;
	}
	
	else if ( PR_FAIL(error=_ExtractName(dst_name,(tPTR*)&pdst,sizeof(dst),pgOBJECT_FULL_NAME,0,0)) )
		;
	else if ( run_under_nt && PR_FAIL(error=OP_getFNameW(_this,&psrc)) )
		;
	else if ( !run_under_nt && PR_FAIL(error=OP_getFName(d,psrc)) )
		;
	else if ( !fCopyFile(psrc,pdst,!overwrite) )
		error = _ConvertWin32Err( d->win32err = GetLastError() );
	
	if ( pdst != dst )
		CALL_SYS_ObjHeapFree( _this, pdst );
	
	if ( psrc != src )
		CALL_SYS_ObjHeapFree( _this, psrc );
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "copy" );

	PR_TRACE_A1( _this, "Leave ObjPtr::Copy method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end





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
	tCHAR       name[UNIS*MAX_PATH];
	PR_TRACE_A0( _this, "Enter ObjPtr::Rename method" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( d->state != cObjPtrState_PTR )
		error = errOBJECT_BAD_INTERNAL_STATE;
	
	else if ( !fMoveFile ) {
		PR_TRACE(( _this, prtERROR, "nfio\tMoveFile export pointer cannot be imported from the Windows kernel" ));
		error = errINTERFACE_NOT_LOADED;
	}
	
	else if ( PR_FAIL(error=_ExtractName(new_name,(tPTR*)&pdst,sizeof(dst),pgOBJECT_FULL_NAME,0,&new_iid)) )
		;
	else if ( run_under_nt && PR_FAIL(error=OP_getFNameW(_this,&psrc)) )
		;
	else if ( !run_under_nt && PR_FAIL(error=OP_getFName(d,psrc)) )
		;
	else if ( PR_FAIL(error=_ren_move(_this,psrc,pdst,overwrite,&d->win32err)) )
		;
	else if ( PR_SUCC(is_folder(pdst,0)) ) { // has to point to renamed object
		size = _toui32(run_under_nt ? UNIS*wcslen(UNI(pdst)) : lstrlen(MB(pdst)));
		memcpy( name, d->fd.cFileName, d->name_len );
		if ( is_relative_path(pdst,size) )
			error = OP_SetRelPath( _this, pdst, size );
		else
			error = OP_SetPath( _this, pdst, &size );
	}
	else {
		tPTR p0;
		size = 0;
		
		p0 = run_under_nt ? ((tPTR)FindSeparatorW(UNI(pdst),&size)) : ((tPTR)FindSeparator(MB(pdst),&size));
		
		if ( !p0 )// just a name
			memcpy( name, pdst, size );
		
			/*
			else if ( !*p0 ) // nonsense -- it must not be path
			*UNI(name) = 0;
		*/
		else { // path with a name
			tUINT psize = _toui32(MB(p0) - MB(pdst));
			tUINT nsize = size - psize;
			
			memcpy( name, p0, nsize );
			if ( is_relative_path(MB(pdst),psize) )
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
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "rename" );

	PR_TRACE_A1( _this, "Leave ObjPtr::Rename method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//  pointer must be advanced to the next object
// Parameters are:
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this ) {
	tERROR error;
	tCHAR  name[MAX_PATH];
	tCHAR* pname = name;
	tBOOL  bRes;
	PR_TRACE_VARS;
	PR_TRACE_A0( _this, "Enter ObjPtr::Delete method" );
	
	if ( !fDeleteFile )
		error = errINTERFACE_NOT_LOADED;
	
	else if ( !run_under_nt && PR_FAIL(error=OP_getFName(_this->data,pname)) )
		;
	else if ( run_under_nt && PR_FAIL(error=OP_getFNameW(_this,&pname)) )
		;
	else
	{
		if (_this->data->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			PR_TRACE(( _this, prtNOTIFY, "nfio\tobjptr: going to delete folder \"%s%S\"", PR_TRACE_PREPARE_STR_CP(name,codepage) ));
			bRes = fRemoveDirectory(name);
		}
		else {
			PR_TRACE(( _this, prtNOTIFY, "nfio\tobjptr: going to delete file \"%s%S\"", PR_TRACE_PREPARE_STR_CP(name,codepage) ));
			bRes = fDeleteFile(name);
		}
		if (!bRes) {
			tDWORD win_err = _this->data->win32err = GetLastError();
			error = _ConvertWin32Err( win_err );
			PR_TRACE(( _this, prtERROR, "nfio\tobjptr: %terr(winerr=%d) cannot delete \"%s%S\"", error, win_err, PR_TRACE_PREPARE_STR_CP(name,codepage) ));
		}
		else {
			PR_TRACE(( _this, prtNOTIFY, "nfio\tobjptr: deleted:\"%s%S\"", PR_TRACE_PREPARE_STR_CP(name,codepage) ));
			ObjPtr_Next( _this );
		}
	}
	
	if ( pname != name )
		CALL_SYS_ObjHeapFree( _this, pname );
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "delete" );

	PR_TRACE_A1( _this, "Leave ObjPtr::Delete method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// ---
tERROR pr_call _go_to_root( ObjPtrData* d ) {
	const tCHAR* pa;
	if ( !d->full )
		return errNOT_OK;

	if ( run_under_nt ) {
		const tWCHAR* pw;
		do {
			pw = _find_w_unc_prefix( UNI(d->full), d->full_len );
			if ( pw ) {
				pw = _find_w_unc_path( pw, d->full_len-_toui32(MB(pw)-d->full), cTRUE );
				if ( !pw )
					return errNOT_OK;
				if ( IS_W_DELIMETER(*pw) )
					++pw;
				break;
			}
			pw = _find_w_prefix( UNI(d->full), d->full_len );
			if ( pw ) {
				tUINT rest = d->full_len - _toui32(MB(pw)-d->full);
				if ( (rest<3*UNIS) || (pw[1]!=L':') || !IS_W_DELIMETER(pw[2]) )
					return errNOT_OK;
				pw += 3;
				break;
			}
			pw = _find_w_unc_path( UNI(d->full), d->full_len, cFALSE );
			if ( pw ) {
				if ( pw && IS_W_DELIMETER(*pw) )
					++pw;
				break;
			}
			pw = UNI(d->full);
			if ( (d->full_len < 3*UNIS) || (pw[1]!=L':') || !IS_W_DELIMETER(pw[2]) )
				return errNOT_OK;
			pw += 3;
		} while( 0 );
		*UNI(pw++) = 0;
		d->full_len = _toui32(MB(pw) - d->full);
		return errOK;
	}

	pa = _find_a_unc_path( MB(d->full), d->full_len, cFALSE );
	if ( pa ) {
		if ( IS_A_DELIMETER(*pa) )
			++pa;
	}
	if ( (d->full_len > 3*MBS) && (pa[1]==':') && IS_A_DELIMETER(pa[2]) )
		pa += 3;
	else
		return errNOT_OK;
	*MB(pa++) = 0;
	d->full_len = _toui32(MB(pa) - d->full);
	return errOK;
}



// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// --- Interface "ObjPtr". Method "Reset"
// Extended method comment
//   First call of Next method after Reset must return *FIRST* object in enumeration
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root ) {
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::Reset method" );
	
	d = _this->data;
	d->state = cObjPtrState_BEFORE;
	d->curr_mask_applied_to_folders = d->apply_mask_to_folders;
	
	if ( !d->handle || !fFindClose || fFindClose(d->handle) )
		d->win32err = NO_ERROR;
	else
		d->win32err = GetLastError();

	d->handle = 0;
	d->name_len = 0;
	d->m_index = 0;
	d->m_dot_passed = cFALSE;
	d->go_func = OP_InitEnum;
	d->folder_step = cFALSE;
	
	if ( to_root )
		_go_to_root( d );

	LOCAL_TRACE( _this, errOK, prtNOT_IMPORTANT, "reset" );

	PR_TRACE_A0( _this, "Leave ObjPtr::Reset method, ret tBOOL = cTRUE, error = errOK" );
	return errOK;
}
// AVP Prague stamp end



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
	d->win32err = 0;
	
	dad = CALL_SYS_ParentGet( _this, IID_ANY );
	if ( PR_FAIL(error=CALL_SYS_ObjectCreate(dad,(hOBJECT*)&ptr,IID_OBJPTR,PID_WIN32_NFIO,SUBTYPE_ANY)) )
		;
	else if ( d->m_ppath && PR_FAIL(error=CALL_SYS_PropertySetStr(ptr,0,pgOBJECT_PATH,d->m_ppath,0,codepage)) )
		;
	else {
		CALL_SYS_PropertySetStr( ptr, 0, pgMASK, d->mask, 0, codepage );
		CALL_SYS_PropertySetBool( ptr, plMASK_APPLIED_TO_FOLDERS, d->apply_mask_to_folders );
		error = CALL_SYS_ObjectCreateDone( ptr );
	}
	
	if ( PR_FAIL(error) )
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
		error = OP_ChangeTo( ptr, (tCHAR*)d->fd.cFileName );
//		tPTR name;
//		tDWORD size = run_under_nt ? (UNIS*MAX_PATH) : MAX_PATH;
//		error = CALL_SYS_ObjHeapAlloc( _this, &name, size );
//		if(PR_SUCC(error)){
//			while( 1 ) {
//				error = CALL_ObjPtr_Next(ptr);
//				if ( PR_FAIL(error) )
//					break;
//				if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(ptr,0,pgOBJECT_NAME,name,size,codepage)) )
//					break;
//				if ( run_under_nt ) {
//					if ( !wcsicmp(UNI(d->fd.cFileName),UNI(name)) )
//						break;
//				}
//				else if ( !lstrcmpi(MB(d->fd.cFileName),MB(name)) )
//					break;
//			}
//			CALL_SYS_ObjHeapFree(_this,name);
//		}
	}
	
	if ( PR_FAIL(error) ) {
		CALL_SYS_ObjectClose( ptr );
		ptr = 0;
	}
	
	if ( result )
		*result = (hObjPtr)ptr;

	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "clone" );
	
	PR_TRACE_A2( _this, "Leave ObjPtr::Clone method, ret hObjPtr = %p, error = 0x%08X", ptr, error );
	return error;
}
// AVP Prague stamp end



// ---
static tERROR pr_call OP_processError( hi_ObjPtr _this ) {
	ObjPtrData* d = _this->data;
	switch( d->win32err ) {
		case ERROR_SUCCESS :
			#if defined (_WIN32)
				if ( run_under_nt )
					d->name_len = _toui32(UNIS * wcslen( d->fd.cFileName ) + UNIS);
			else
				#endif
					d->name_len = _toui32(MBS * lstrlen( MB(d->fd.cFileName) ) + MBS);
			d->state = cObjPtrState_PTR;
			return errOK;

		case ERROR_FILE_NOT_FOUND:
		case ERROR_NO_MORE_FILES :
			if ( d->folder_step ) {
				*d->fd.cFileName = 0;
				#if defined (_WIN32)
					if ( run_under_nt )
						d->name_len = UNIS;
				else
					#endif
						d->name_len = MBS;
				d->state = cObjPtrState_AFTER;
				LOCAL_TRACE( _this, errEND_OF_THE_LIST, prtNOT_IMPORTANT, "end_of_the_list" );
				return errEND_OF_THE_LIST;
			}
			d->folder_step = cTRUE;
			return OP_InitEnum( _this );

		default:
			*d->fd.cFileName = 0;
			#if defined (_WIN32)
				if ( run_under_nt )
					d->name_len = UNIS;
			else
				#endif
					d->name_len = MBS;
			d->state = cObjPtrState_UNDEFINED;
			return _ConvertWin32Err( d->win32err );
	}
}



// ---
static tERROR pr_call OP_go( hi_ObjPtr _this ) {
	ObjPtrData* d = _this->data;
	if ( fFindNextFile(d->handle,&d->fd) ) {
		++d->m_index;
		d->win32err = ERROR_SUCCESS;
	}
	else
		d->win32err = GetLastError();
	return OP_processError( _this );
}



// ---
static tERROR pr_call OP_InitEnum( hi_ObjPtr _this ) {
	tERROR err;
	ObjPtrData* d = _this->data;
	
	if ( !d->handle || !fFindClose || fFindClose(d->handle) )
		d->win32err = NO_ERROR;
	else
		d->win32err = GetLastError();
	
	d->handle = 0;
	d->m_index = 0;
	*d->fd.cFileName = 0;

	if ( run_under_nt )
		d->name_len = UNIS;
	else
		d->name_len = MBS;

	if ( d->folder_step > 1 )
		err = errEND_OF_THE_LIST;
	
	else {
		tCHAR  fm[MAX_PATH];
		tCHAR* pfm = fm;
		tUINT  pl;
		tUINT  ml;
		tPTR   lmask;

		err = errOK;
		d->win32err = ERROR_SUCCESS;

		if ( run_under_nt ) {
			pl = _toui32(( d->full ) ? UNIS * wcslen( UNI(d->full) ) : 0);
			if ( d->folder_step && !d->curr_mask_applied_to_folders ) {
				ml = 2 * UNIS;
				lmask = L"*";
			}
			else {
				ml = _toui32(UNIS * wcslen( UNI(d->mask) ) + UNIS);
				lmask = d->mask;
			}
			if ( (pl + ml) > (UNIS*UNICODE_MAX_PATH) )
				err = errBUFFER_TOO_SMALL;
			else if ( (pl+ml) > MAX_PATH )
				err = CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&pfm, pl+ml );
		}
		else {
			pl =( d->full ) ? MBS * lstrlen( d->full ) : 0;
			if ( d->folder_step && !d->curr_mask_applied_to_folders ) {
				ml = 2 * MBS;
				lmask = "*";
			}
			else {
				ml = MBS * lstrlen( d->mask ) + MBS;
				lmask = d->mask;
			}
			if ( (pl + ml) > MAX_PATH )
				err = errBUFFER_TOO_SMALL;
		}
		
		if ( PR_SUCC(err) ) {
			memcpy( pfm,    d->full, pl );
			memcpy( pfm+pl, lmask,   ml );
			
			if ( INVALID_HANDLE_VALUE == (d->handle=fFindFirstFile(pfm,&d->fd)) ) {
				d->handle = 0;
				d->win32err = GetLastError();

				if( d->win32err == ERROR_ACCESS_DENIED && ((tBYTE)GetVersion() >= 6) )
				{
					hTOKEN token = NULL;
					tERROR err = CALL_SYS_ObjectCreateQuick( _this, &token, IID_TOKEN, PID_SYSTEM, SUBTYPE_ANY );
					if ( PR_SUCC(err) )
						err = CALL_Token_Impersonate(token, eitAsAdmin);
					if ( PR_SUCC(err) )
					{
						if ( INVALID_HANDLE_VALUE == (d->handle=fFindFirstFile(pfm,&d->fd)) )
							d->handle = 0;
						else
							d->win32err = 0;

						CALL_Token_Impersonate(token, eitAsUser);
					}

					if( token )
						CALL_SYS_ObjectClose(token);
				}
			}
			
			#if defined( _PRAGUE_TRACE_ )

				if ( ml ) {
					tERROR e = errOK;
					if ( d->m_generated_mask_len < ml ) {
						if ( d->m_generated_mask )
							CALL_SYS_ObjHeapFree( _this, (tPTR)d->m_generated_mask );
						e = CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&d->m_generated_mask, ml );
					}
					if ( PR_SUCC(e) )
						memcpy( d->m_generated_mask, lmask, ml );
				}
				else if ( d->m_generated_mask ) {
					if ( run_under_nt )
						*UNI(d->m_generated_mask) = 0;
					else
						*MB(d->m_generated_mask) = 0;
				}

				LOCAL_TRACE( _this, errOK, prtNOT_IMPORTANT, "init_enum" );
			#endif

			if ( pfm != fm )
				CALL_SYS_ObjHeapFree( _this, pfm );

			d->go_func = OP_go;
			err = OP_processError( _this );
		}
	}

	return err;
}



// ---
static tBOOL check_auxiliary_folder( ObjPtrData* d ) {
	const tWCHAR* p = d->fd.cFileName;
	#if defined(_WIN32)
		if ( run_under_nt ) {
			if ( (d->m_index == 0) && (*p == L'.') && (*(p+1) == 0) ) { // str == "." 
				d->m_dot_passed = cTRUE;
				return cTRUE;
			}
			if ( d->m_dot_passed && (d->m_index == 1) && (*p == L'.') && (*(p+1) == L'.') && (*(p+2) == 0) ) // or str == ".."
				return cTRUE;
			return cFALSE;
		}
		else
	#endif
	{
		const tCHAR* p1 = (const tCHAR*)p;
		if ( (d->m_index == 0) && (*p1 == '.') && (*(p1+1) == 0) ) { // str == "." 
			d->m_dot_passed = cTRUE;
			return cTRUE;
		}
		if ( d->m_dot_passed && (d->m_index == 1) && (*p1 == '.') && (*(p1+1) == '.') && (*(p1+2) == 0) ) // or str == ".."
			return cTRUE;
		return cFALSE;
	}
}



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
	d->win32err = 0;
	
	if ( d->state == cObjPtrState_AFTER )
		error = errEND_OF_THE_LIST;
	
	else if ( d->state == cObjPtrState_UNDEFINED )
		error = errOBJECT_BAD_INTERNAL_STATE;
	
	else {
		for ( ; PR_SUCC(error=d->go_func(_this)); ) {
			if ( d->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
				if ( check_auxiliary_folder(d) )
					continue;
				if ( !d->folder_step )
					continue;
			}
			else if ( d->folder_step )
				continue;
			break;
		}
	}
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "next" );

	PR_TRACE_A2( _this, "Leave ObjPtr::Next method, ret tERROR = 0x%08X, name = %s", error, d->fd.cFileName );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// --- Interface "ObjPtr". Method "StepUp"
// tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this ) {
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this ) {
	tERROR       error;
	ObjPtrData*  d;
	tDWORD       len;
	tCHAR        name_buff[MAX_PATH];
	tCHAR*       pname = name_buff;
	PR_TRACE_A0( _this, "Enter ObjPtr::StepUp method" );
	
	d = _this->data;
	d->win32err = 0;
	error = errEND_OF_THE_LIST;
	
	if ( run_under_nt ) {
		if ( 0 < (len=_toui32(wcslen(UNI(d->m_ppath)))) ) {
			tWCHAR* end;
			//Andy: ¬ этом случае ptr будет указывать на последний 0, а перед ним "\" надо уменьшить на 1
			tWCHAR* ptr = UNI(d->m_ppath) + len;
			// Andy: что и сделано (если указывает на 0
			if ( (*ptr == 0))
				ptr--;
			if ( (*ptr == L'\\') || (*ptr == L'/') )
				ptr--;
			end = ptr + 1;
			while ( ptr > UNI(d->m_ppath) ) {
				//Andy: и вот тут-то мы попадаем на последний "\" а потм считаем, что это найденный предыдущий "\"
				if ( (*ptr != L'\\') && (*ptr != L'/') )
					ptr--;
				else if ( (sizeof(name_buff) < UNIS * (end-ptr)) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,(tPTR*)&pname,_toui32(UNIS * (end-ptr)))) )
					break;
				else {
					memcpy( pname, ptr+1, UNIS*(end-ptr-1) );
					*(UNI(pname) + (end-ptr-1)) = 0;
					*(UNI(ptr)+1) = 0;
					wcscpy_s( UNI(d->mask), 2, L"*" );
					error = ObjPtr_Reset( _this, cFALSE );
					while( PR_SUCC(error=ObjPtr_Next(_this)) && _wcsicmp(UNI(d->fd.cFileName),UNI(pname)) )
						;
					if ( pname != name_buff )
						CALL_SYS_ObjHeapFree( _this, pname );
					break;
				}
			}
		}
	}
	else if ( 0 < (len=lstrlen(d->m_ppath)) ) {
		tCHAR* end;
		tCHAR* ptr = d->m_ppath + len;
		if ( (*ptr == 0))
			ptr--;
		if ( (*ptr == '\\') || (*ptr == '/') )
			ptr--;
		end = ptr + 1;
		while ( ptr > d->m_ppath ) {
			if ( (*ptr == '\\') || (*ptr == '/') ) {
				memcpy( name_buff, ptr+1, end-ptr-1 );
				name_buff[end-ptr-1] = 0;
				*(ptr+1) = 0;
				*((tDWORD*)d->mask) = MAKELONG( MAKEWORD('*','.'), MAKEWORD('*',0) );
				error = ObjPtr_Reset( _this, cFALSE );
				while( PR_SUCC(error=ObjPtr_Next(_this)) && lstrcmpi(MB(d->fd.cFileName),name_buff) )
					;
				break;
			}
			ptr--;
		}
	}
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "step up" );

	PR_TRACE_A2( _this, "Leave ObjPtr::StepUp method, ret tERROR = 0x%8x, path = %s", error, d->m_ppath );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this ) {
	tUINT pl;
	tUINT nl;
	tERROR error;
	ObjPtrData* d;
	PR_TRACE_A0( _this, "Enter ObjPtr::StepDown method" );
	
	d = _this->data;
	d->win32err = 0;
	
	if ( (d->state != cObjPtrState_PTR) || !(d->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		error = errOBJECT_BAD_INTERNAL_STATE;
	
	else if ( run_under_nt ) {
		if ( !d->full )
			error = errOBJECT_NOT_INITIALIZED;
		else {
			tUINT len;
			tUINT prefix_len = _toui32(d->m_ppath - d->full);
			pl = _toui32(UNIS * wcslen( UNI(d->full+prefix_len) ));
			nl = _toui32(UNIS * wcslen( UNI(d->fd.cFileName) ));
			len = prefix_len + pl + nl + 2*UNIS;
			if ( len > UNICODE_MAX_PATH )
				error = errBUFFER_TOO_SMALL;
			else if ( (d->full_len < len) && PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->full,d->full,len)) )
				;
			else {
				d->m_ppath = d->full + prefix_len;
				d->full_len = len;
				memcpy( d->m_ppath+pl, d->fd.cFileName, nl );
				*UNI(MB(d->m_ppath)+pl+nl+0) = PATH_DELIMETER_WIDECHAR;
				*UNI(MB(d->m_ppath)+pl+nl+1) = 0;
				error = ObjPtr_Reset( _this, cFALSE );
			}
		}
	}
	else if ( !d->m_ppath )
		error = errOBJECT_NOT_INITIALIZED;
	else {
		pl = lstrlen( MB(d->m_ppath) );
		nl = lstrlen( MB(d->fd.cFileName) );
		if ( (pl+nl+2) > MAX_PATH )
			error = errBUFFER_TOO_SMALL;
		else if ( (d->full_len < (pl+nl+2)) && PR_FAIL(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->full,d->full,(pl+nl+2))) )
			;
		else {
			d->m_ppath = d->full;
			d->full_len = pl + nl + 2;
			memcpy( d->m_ppath+pl, d->fd.cFileName, nl );
			*(d->m_ppath+pl+nl+0) = PATH_DELIMETER_CHAR;
			*(d->m_ppath+pl+nl+1) = 0;
			error = ObjPtr_Reset( _this, cFALSE );
		}
	}
	
	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "step down" );

	PR_TRACE_A1( _this, "Leave ObjPtr::StepDown method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end





// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// --- Interface "ObjPtr". Method "ChangeTo"
// Parameter "name":
//   The same as "object" parameter of OS::EnumCreate method
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT oname ) {
	tERROR error;
	tCHAR  file_name[MAX_PATH] = {0};
	tCHAR* pfile_name = file_name;
	tDWORD size;
	tCHAR  name[UNIS*MAX_PATH];
	PR_TRACE_A0( _this, "Enter ObjPtr::ChangeTo method" );

	
	_this->data->win32err = 0;
	
	if ( PR_FAIL(error=_ExtractName(oname,(tPTR*)&pfile_name,sizeof(file_name),pgOBJECT_FULL_NAME,NULL,NULL)) )
		;
	else {
		tPTR p0;
		size = 0;
		
		p0 = run_under_nt ? ((tPTR)FindSeparatorW(UNI(pfile_name),&size)) : ((tPTR)FindSeparator(MB(pfile_name),&size));
		if ( !p0 ) { // just a name
			if ( size > sizeof(name) )
				error = errBUFFER_TOO_SMALL;
			else
				memcpy( name, pfile_name, size );
		}
		else {
			tUINT psize = _toui32(MB(p0) - MB(pfile_name));
			tUINT nsize = size - psize;
			
			if ( nsize > sizeof(name) )
				error = errBUFFER_TOO_SMALL;
			else {
				memcpy( name, p0, nsize );
				if ( is_relative_path(MB(pfile_name),psize) )
					error = OP_SetRelPath( _this, pfile_name, psize );
				else
					error = OP_SetPath( _this, pfile_name, &psize );
			}
		}
	}

	if ( PR_SUCC(error) )	{
		if ( (run_under_nt ? *UNI(name) : *MB(name) ) != 0 )
			error = OP_ChangeTo( _this, name );
		else
			error = errOBJECT_NOT_FOUND;
	}
	
	if ( pfile_name != file_name )
		CALL_SYS_ObjHeapFree( _this, pfile_name );

	LOCAL_TRACE( _this, error, prtNOT_IMPORTANT, "change_to" );

	PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret tERROR = 0x%08X", error );
	return error;
}
// AVP Prague stamp end




// ---
tERROR pr_call OP_ChangeTo( hi_ObjPtr _this, tCHAR* name ) {
	WIN32_FIND_DATAW* fd;
	tERROR error = ObjPtr_Reset( _this, cFALSE );
	typedef int (__cdecl * tcmpi)( const void*, const void* );
	tcmpi cmpi;
	
	if ( PR_FAIL(error) )
		return error;
	
	if ( run_under_nt ) 
		cmpi = (tcmpi)_wcsicmp;
	else
		cmpi = (tcmpi)_strcmpi;
	
	fd = &_this->data->fd;
	
	while( PR_SUCC(error=ObjPtr_Next(_this)) ) {
		//PR_TRACE((_this, prtNOTIFY, run_under_nt ? "nfio\tOP_ChangeTo: %S->%S" : "nfio\tOP_ChangeTo: %s->%s", fd->cFileName, name));
		if ( !cmpi(fd->cFileName,name) )
			return errOK;
	}
	PR_TRACE((_this, prtNOTIFY, run_under_nt ? "nfio\tOP_ChangeTo: %S not found" : "nfio\tOP_ChangeTo: %s not found", name));
	return error;
}




// ---
tERROR pr_call OP_SetMask( hi_ObjPtr _this, tSTRING buffer, tDWORD* size ) {
	ObjPtrData* d = _this->data;
	
	if ( run_under_nt ) {
		*size = uni_len( UNI(buffer), *size );
		if ( !*size ) {
			wcscpy_s( UNI(d->mask), 2, L"*" );
			*size = 5 * UNIS;
			return errOK;
		}
		else
			*size += UNIS;
	}
	else {
		*size = mb_len( buffer, *size );
		if ( !*size ) {
			lstrcpy( d->mask, "*" );
			*size = 4 * MBS;
			return errOK;
		}
		else if ( *size > MAX_PATH ) {
			*size = 0;
			return errBUFFER_TOO_SMALL;
		}
		else
			*size += MBS;
	}
	
	if ( *size > d->mask_len ) {
		tERROR error;
		if ( d->mask == d->mask_buff )
			error = CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&d->mask, *size );
		else
			error = CALL_SYS_ObjHeapRealloc( _this, (tPTR*)&d->mask, d->mask, *size );
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
	
	if ( run_under_nt ) {
		tDWORD adv;
		
		flen = *size = uni_len( UNI(buffer), *size );
		//while( (((tINT)flen) > 0) && (*UNI(MB(buffer)+flen-UNIS) == L' ') ) {
		//	flen -= UNIS;
		//	*size -= UNIS;
		//}
		if( (flen>UNIS) && IS_W_DELIMETER(*UNI(MB(buffer)+flen-UNIS)) )
			flen -= UNIS;
		flen += UNIS + UNIS; // slash and terminated zero
		if ( d->m_do_not_make_prefix && (flen<MAX_PATH/2) && !_find_w_unc_prefix(UNI(buffer),flen) && !_find_w_prefix(UNI(buffer),flen) ) {
			adv = 0;
			prefix = 0;
			prefix_len = 0;
		}
		else
			prefix = get_prefix( UNI(buffer), *size, &adv, &prefix_len );
		if ( adv ) {
			buffer += adv * UNIS;
			*size -= adv * UNIS;
			flen -= adv * UNIS;
		}
		flen += prefix_len;
	}
	else {
		flen = *size = mb_len( buffer, *size );
		//while( flen && *MB(MB(buffer)+flen-MBS) == ' ' ) {
		//	flen -= MBS;
		//	*size -= MBS;
		//}
		if( (flen>MBS) && IS_A_DELIMETER(*(MB(buffer)+flen-MBS) ) )
			flen -= MBS;
		flen += MBS + MBS; // slash and terminated zero
		prefix = 0;
		prefix_len = 0;
	}
	
	if ( *size ) {
		if ( d->full_len < flen ) {
			tERROR error = CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&d->full, flen );
			if ( PR_SUCC(error) )
				d->full_len = flen;
			else {
				*size = 0;
				return error;
			}
		}
		
		if ( prefix )
			memcpy( d->full, prefix, prefix_len );
		memcpy( MB(d->full)+prefix_len, buffer, *size );
		
		if ( run_under_nt ) {
			if ( !IS_W_DELIMETER(*UNI(MB(d->full) + prefix_len + (*size) - UNIS)) )
				*UNI(MB(d->full) + prefix_len + (*size)++) = PATH_DELIMETER_WIDECHAR;
			*UNI(MB(d->full) + prefix_len + (*size) ) = 0;
			d->m_ppath = MB(d->full) + prefix_len;
		}
		else {
			if ( !IS_A_DELIMETER(*(MB(d->full) + (*size) - MBS)) )
				*(MB(d->full) + (*size)++) = PATH_DELIMETER_CHAR;
			*(MB(d->full) + (*size) ) = 0;
			d->m_ppath = d->full;
		}
	}
	else {
		d->m_ppath = d->full;
		if ( d->full ) {
			if ( run_under_nt )
				*UNI(d->full) = 0;
			else
				*MB(d->full) = 0;
		}
	}
	return errOK;
}



// ---
tERROR pr_call OP_getFName( ObjPtrData* data, tCHAR* buff ) {
	
	if ( data->m_ppath ) {
		tUINT path_len = lstrlen( MB(data->m_ppath) );
		if ( (path_len + data->name_len) > MAX_PATH )
			return errBUFFER_TOO_SMALL;
		if ( path_len ) {
			memcpy( buff, data->m_ppath, path_len );
			buff += path_len;
		}
	}
	memcpy( buff, data->fd.cFileName, data->name_len );
	
	return errOK;
}



// ---
tERROR pr_call OP_getFNameW( hi_ObjPtr _this, tCHAR** buff ) {
	tERROR      error;
	tUINT       len;
	ObjPtrData* d = _this->data;
	
	if ( d->m_ppath )
		len = _toui32(UNIS * wcslen(UNI(d->full)));
	else
		len = 0;
	
	if ( ((len+d->name_len) > MAX_PATH) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,(tPTR*)buff,len+d->name_len)) )
		return error;
	
	if ( len )
		memcpy( *buff, d->full, len );
	memcpy( *buff+len, d->fd.cFileName, d->name_len );
	return errOK;
}





// ---
static tERROR pr_call OP_getShortFName( ObjPtrData* data, tCHAR* buff ) {
	
	WIN32_FIND_DATAA* fd = (WIN32_FIND_DATAA*)&data->fd;
	tUINT name_len = 1 + lstrlen( MB(fd->cAlternateFileName) );

	if ( data->m_ppath ) {
		tUINT path_len = lstrlen( MB(data->m_ppath) );
		if ( (path_len + name_len) > MAX_PATH )
			return errBUFFER_TOO_SMALL;
		if ( path_len ) {
			memcpy( buff, data->m_ppath, path_len );
			buff += path_len;
		}
	}
	memcpy( buff, fd->cAlternateFileName, name_len );
	
	return errOK;
}



// ---
static tERROR pr_call OP_getShortFNameW( hi_ObjPtr _this, tCHAR** buff ) {
	tERROR      error;
	tUINT       len;
	tUINT       name_len;
	ObjPtrData* d = _this->data;
	WIN32_FIND_DATAW* fd = (WIN32_FIND_DATAW*)&d->fd;
	
	if ( d->m_ppath )
		len = _toui32(UNIS * wcslen(UNI(d->full)));
	else
		len = 0;
	
	name_len = _toui32(UNIS * (1 + wcslen(fd->cAlternateFileName)));
	if ( ((len+name_len) > MAX_PATH) && PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,(tPTR*)buff,len+d->name_len)) )
		return error;
	
	if ( len )
		memcpy( *buff, d->full, len );
	memcpy( *buff+len, fd->cAlternateFileName, name_len );
	return errOK;
}





// ---
tERROR pr_call OP_SetRelPath( hi_ObjPtr _this, tPTR rel_path, tUINT size ) {
	
	tERROR error;
	tPTR   new_path;
	tUINT  osize;
	tUINT  add;
	
	ObjPtrData* d = _this->data;
	
	if ( run_under_nt ) {
		osize = _toui32(d->full ? (UNIS * wcslen(UNI(d->full))) : 0);
		add = UNIS;
		if ( (size+osize+add) > (UNIS*UNICODE_MAX_PATH) )
			return errBUFFER_TOO_SMALL;
	}
	else {
		osize = d->full ? lstrlen(MB(d->full)) : 0;
		add = MBS;
		if ( (size+osize+add) > MAX_PATH )
			return errBUFFER_TOO_SMALL;
	}
	
	error = CALL_SYS_ObjHeapAlloc( _this, &new_path, size+osize+add );
	if ( PR_FAIL(error) )
		return error;
	
	memcpy( MB(new_path),       d->full,  osize );
	memcpy( MB(new_path)+osize, rel_path, size );
	size += osize;
	error = OP_SetPath( _this, new_path, &size );
	CALL_SYS_ObjHeapFree( _this, new_path );
	return error;
}



