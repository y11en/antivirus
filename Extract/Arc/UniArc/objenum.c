// -------- Monday,  23 October 2000,  15:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Unversal Extractor
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- objenum.c
// -------------------------------------------




#define ObjPtr_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <Extract/plugin/p_miniarc.h>

#include "objenum.h"
#include "io.h"
#include "os.h"

#include <Prague/iface/i_string.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_heap.h>

#include <Extract/iface/i_minarc.h>

#include <memory.h>
#include <string.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Interface comment )
// --------------------------------------------
// --- 74d331e0_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
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
// AVP Prague stamp end( ObjPtr, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Data structure )
// Interface ObjPtr. Inner data structure
typedef struct tag_ObjPtr_Data 
{
	tQWORD qwLastObject;
	hMINIARC hArc;
	hOS hFatherOS;
	tBOOL bIsCheckForNewFiles;
	tDWORD dwPosOfNewFile;
	hIO hCurrentNewFileIo;
} ObjPtr_Data;
// AVP Prague stamp end( ObjPtr, Data structure )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, object declaration )
typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL*  sys;  // pointer to the "SYSTEM" virtual table
	ObjPtr_Data*      data;   // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;
// AVP Prague stamp end( ObjPtr, object declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Internal method table prototypes )
// Interface "ObjPtr". Internal method table. Forward declarations
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
// AVP Prague stamp end( ObjPtr, Internal method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Internal method table )
// Interface "ObjPtr". Internal method table.
static iINTERNAL i_ObjPtr_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       ObjPtr_ObjectInit,
	(tIntFnObjectInitDone)   NULL,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end( ObjPtr, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, interface function forward declarations )
// ----------- operations with the current object ----------
typedef   tERROR (pr_call *fnpObjPtr_IOCreate) ( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ); // -- creates object current in enumeration;
typedef   tERROR (pr_call *fnpObjPtr_Copy)     ( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite ); // -- copies the object pointed by the ptr;
typedef   tERROR (pr_call *fnpObjPtr_Rename)   ( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite ); // -- renames the object pointed by the ptr;
typedef   tERROR (pr_call *fnpObjPtr_Delete)   ( hi_ObjPtr _this );                         // -- deletes object pointed by ptr;

// ----------- navigation methods ----------
typedef   tERROR (pr_call *fnpObjPtr_Reset)    ( hi_ObjPtr _this, tBOOL to_root );          // -- Reinitializing of ObjPtr object;
typedef   tERROR (pr_call *fnpObjPtr_Clone)    ( hi_ObjPtr _this, hObjPtr* result );                         // -- Makes another ObjPtr object;
typedef   tERROR (pr_call *fnpObjPtr_Next)     ( hi_ObjPtr _this );                         // -- Returns next IO object;
typedef   tERROR (pr_call *fnpObjPtr_StepUp)   ( hi_ObjPtr _this );                         // -- Goes one level up;
typedef   tERROR (pr_call *fnpObjPtr_StepDown) ( hi_ObjPtr _this );                         // -- Goes one level up;
typedef   tERROR (pr_call *fnpObjPtr_ChangeTo) ( hi_ObjPtr _this, hOBJECT name );           // -- Changes folder for enumerating;
// AVP Prague stamp end( ObjPtr, interface function forward declarations )
// --------------------------------------------------------------------------------



// AVP Prague stamp begin( Interface declaration,  )
struct iObjPtrVtbl 
{
	
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
// AVP Prague stamp begin( ObjPtr, External method table prototypes )
// Interface "ObjPtr". External method table. Forward declarations
// operations with the current object
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this );

// navigation methods
// group of methods to navigate object pointers
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root );
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result );
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this );
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name );
// AVP Prague stamp end( ObjPtr, External method table prototypes )
// --------------------------------------------------------------------------------

tERROR iCheckForFileInTree(tQWORD qwObjectId, hTREE Tree, hIO* pIO);
tBOOL SetNewFileData(hi_ObjPtr _this, hTREE tree, tDWORD* pdwConter, hIO* pio);

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External method table )
// Interface "ObjPtr". External method table.
static iObjPtrVtbl e_ObjPtr_vtbl = 
{
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
// AVP Prague stamp end( ObjPtr, External method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Forwarded property methods declarations )
// Interface "ObjPtr". Get/Set property methods
tERROR pr_call ObjPtr_PropertySet( hi_ObjPtr _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call ObjPtr_PropertyGet( hi_ObjPtr _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( ObjPtr, Forwarded property methods declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Global property variable declaration )
// Interface "ObjPtr". Global(shared) property table variables
const tVERSION ObjPtr_Version = 2; // must be READ_ONLY at runtime
const tSTRING  ObjPtr_Comment = "Universal Archiver (ObjPtr)"; // must be READ_ONLY at runtime
// AVP Prague stamp end( ObjPtr, Global property variable declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Property table )
// Interface "ObjPtr". Property table
mPROPERTY_TABLE(ObjEnum_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, ObjPtr_Version, sizeof(ObjPtr_Version) )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Universal Archiver (ObjPtr)", 29 )

	mLOCAL_PROPERTY_FN ( pgOBJECT_NAME, ObjPtr_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN ( pgOBJECT_PATH, ObjPtr_PropertyGet, NULL  )
	mLOCAL_PROPERTY_FN ( pgOBJECT_FULL_NAME, ObjPtr_PropertyGet, NULL )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, ObjPtr_Data, hFatherOS, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN ( pgOBJECT_SIZE_Q, ObjPtr_PropertyGet, NULL)
	mLOCAL_PROPERTY_FN ( pgOBJECT_COMPRESSED_SIZE, ObjPtr_PropertyGet, NULL )
    mLOCAL_PROPERTY_FN ( pgOBJECT_SIGNATURE, ObjPtr_PropertyGet, NULL )
    mLOCAL_PROPERTY_FN ( pgOBJECT_COMPRESSION_METHOD, ObjPtr_PropertyGet, NULL )
    mLOCAL_PROPERTY_FN ( pgOBJECT_PASSWORD_PROTECTED, ObjPtr_PropertyGet, NULL )

//	mLOCAL_PROPERTY_FN ( pgOBJECT_ATTRIBUTES, ObjPtr_PropertyGet, NULL )
//	mLOCAL_PROPERTY_FN ( pgMASK, ObjPtr_PropertyGet, ObjPtr_PropertySet )
	mLOCAL_PROPERTY_FN ( pgIS_FOLDER, ObjPtr_PropertyGet, NULL  )

mPROPERTY_TABLE_END(ObjEnum_PropTable)
// AVP Prague stamp end( ObjPtr, Property table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Registration call )
// Interface "ObjPtr". Register function
tERROR pr_call ObjPtr_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"ObjPtr::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OBJPTR,                            // interface identifier
		PID_UNIVERSAL_ARCHIVER,                 // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000002,                             // interface version
		VID_ANDY,                               // interface developer
		&i_ObjPtr_vtbl,                        // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),  // internal function table size
		&e_ObjPtr_vtbl,                        // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),  // external function table size
		ObjEnum_PropTable,                      // property table
		mPROPERTY_TABLE_SIZE(ObjEnum_PropTable),// property table size
		sizeof(ObjPtr_Data),                   // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( error != errOK )
			PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJENUM) registered [error=%u]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"ObjPtr::Register\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( ObjPtr, Registration call )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Property method implementation )
// Interface "OS". Method "Get" for property(s):
//  -- pgOBJECT_NAME
//  -- pgOBJECT_PATH
//  -- pgOBJECT_ATTRIBUTES
//  -- pgMASK
//  -- pgIS_FOLDER
//  -- pgOBJECT_OS_TYPE


tERROR pr_call ObjPtr_PropertyGet( hi_ObjPtr _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error;
tDWORD size1;
	error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"OS_PropertGet\" for property \"pgOBJECT_OS_TYPE\"" );
	
	if(out_size)
		*out_size = 0;

	switch (prop)
	{
		case pgOBJECT_PATH:
		case pgOBJECT_NAME:
		case pgOBJECT_FULL_NAME:
			if(_this->data->hCurrentNewFileIo)
			{
				return CALL_SYS_PropertyGetStr(_this->data->hCurrentNewFileIo, out_size, prop, buffer, size, cCP_ANSI);
			}
			size1 = sizeof(tQWORD);
			CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwLastObject, size1);
			return CALL_SYS_PropertyGetStr(_this->data->hArc, out_size, prop, buffer, size, cCP_ANSI);
			
		case pgIS_FOLDER:
		case pgOBJECT_OS_TYPE:
		case pgOBJECT_SIZE_Q:
        case pgOBJECT_COMPRESSED_SIZE:
        case pgOBJECT_SIGNATURE:
        case pgOBJECT_COMPRESSION_METHOD:
        case pgOBJECT_PASSWORD_PROTECTED:
			if(_this->data->hCurrentNewFileIo)
			{
				return CALL_SYS_PropertyGet(_this->data->hCurrentNewFileIo, out_size, prop, buffer, size);
			}
			size1 = sizeof(tQWORD);
			CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwLastObject, size1);
			return CALL_SYS_PropertyGet(_this->data->hArc, out_size, prop, buffer, size);
	}

	return error;
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Property method implementation )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_NEW
//  -- pgMASK
tERROR pr_call ObjPtr_PropertySet( hi_ObjPtr _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method \"ObjEnum_PropertySet\" for property \"pgOBJECT_NEW\"" );
	
	if(out_size)
		*out_size = 0;

	return CALL_SYS_PropertyGet(_this->data->hArc, out_size, prop, buffer, size);

	PR_TRACE_A2( _this, "Leave *SET* method \"ObjEnum_PropertySet\" for property \"pgOBJECT_NEW\", ret tUINT = %u(size), error = %u", *out_size, error );
	return error;
}
// AVP Prague stamp end( ObjPtr, Property method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, Internal (kernel called) interface method implementation )
// --- Interface "ObjPtr". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this ) 
{
tERROR error ;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::ObjectInit\" method" );
	error = errPARAMETER_INVALID;

	_this->data->hFatherOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this,IID_OS);
	if(_this->data->hFatherOS )
	{
		_this->data->hArc = (hMINIARC)CALL_SYS_PropertyGetObj(_this->data->hFatherOS, ppMINI_ARCHIVER_OBJECT);
		if(_this->data->hArc )
		{
			error = errOK;
		}
	}


	PR_TRACE_A1( _this, "Leave \"ObjPtr::ObjectInit\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( ObjPtr, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  Creates hIO or hObjEnum object current in enumeration.
// Result comment
//  Returns hIO or hObjEnum object. Depends on current object.
// Parameters are:
// "access_mode" : New object open mode
// "open_mode"   : Attributes of the new object
tERROR pr_call ObjPtr_IOCreate( hi_ObjPtr _this, hIO* result, hSTRING name, tDWORD access_mode, tDWORD open_mode ) 
{
tERROR  error;
hIO ret_val;
hObjPtr hPtr;
	
	PR_TRACE_A0( _this, "Enter \"ObjPtr::CurrentObjCreate\" method" );
	if(result == NULL )
	{
		PR_TRACE_A2( _this, "Leave \"OS::IOCreate\" method, ret hIO = %p, error = 0x%08x", NULL, errPARAMETER_INVALID );
		return errPARAMETER_INVALID;
	}

	ret_val = NULL;

// Сделать проверку на имя, если не NULL, то создать ObjPtr найти в нем файл (не забывать про пути) и открыть его
	if(name != NULL)
	{
		error = CALL_ObjPtr_Clone (_this, &hPtr);
		if(PR_SUCC(error))
		{
			error = CALL_ObjPtr_ChangeTo((hi_ObjPtr)hPtr, (hOBJECT)name);
			if(PR_SUCC(error))
			{
				error = CALL_ObjPtr_IOCreate((hi_ObjPtr)hPtr, &ret_val/*result*/, NULL, access_mode, open_mode);
				if(PR_SUCC(error))
				{
					CALL_SYS_ParentSet(ret_val/**result*/, NULL, (hOBJECT)_this);
				}
			}
			CALL_SYS_ObjectClose(hPtr);
		}
	}
	else
	{
		if(_this->data->hCurrentNewFileIo)
		{
			ret_val = _this->data->hCurrentNewFileIo;
			error = errOK;
		}
		else
		{
			error = CALL_SYS_ObjectCreate(_this, &ret_val, IID_IO, PID_UNIVERSAL_ARCHIVER, 0);
			if(PR_SUCC(error))
			{
				CALL_SYS_PropertySetQWord(ret_val, ppOBJECT_TO_OPEN, _this->data->qwLastObject);// Must be first
				CALL_SYS_PropertySetObj(ret_val, ppMINI_ARCHIVER_OBJECT , (hOBJECT)_this->data->hArc);
				CALL_SYS_PropertySetDWord(ret_val, pgOBJECT_ACCESS_MODE , access_mode);
				CALL_SYS_PropertySetDWord(ret_val, pgOBJECT_OPEN_MODE , open_mode);
				
				error = CALL_SYS_ObjectCreateDone( ret_val);
                if (PR_FAIL(error))
                {
                    CALL_SYS_ObjectClose(ret_val);
                    ret_val = NULL;
                }
			}
		}
	}
	
	*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"ObjPtr::CurrentObjCreate\" method, ret hOBJECT = %p, error = %u", ret_val, error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call ObjPtr_Copy( hi_ObjPtr _this, hOBJECT dst_name, tBOOL overwrite ) 
{
tERROR error;
hSTRING src_name;
	
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Copy\" method" );
	
	error = CALL_SYS_ObjectCreateQuick( _this, &src_name, IID_STRING, PID_ANY, SUBTYPE_ANY );
	if(PR_SUCC(error))
	{
		CALL_String_ImportFromProp( src_name, 0, (hOBJECT)_this, pgOBJECT_FULL_NAME );
		error = CALL_OS_Copy( _this->data->hFatherOS, (hOBJECT)src_name, dst_name, overwrite ) ;
		CALL_SYS_ObjectClose((hOBJECT)src_name);
	}
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::Copy\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR pr_call ObjPtr_Rename( hi_ObjPtr _this, hOBJECT new_name, tBOOL overwrite ) 
{
tERROR error = errNOT_IMPLEMENTED;
/*
hSTRING old_name;
tDWORD dwSize;	
*/
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Rename\" method" );
/*	
	error = CALL_SYS_ObjectCreateQuick( _this, &old_name, IID_STRING, PID_ANY, SUBTYPE_ANY );
	if(PR_SUCC(error))
	{
		CALL_String_ImportFromProp( old_name, 0, (hOBJECT)_this, pgOBJECT_NAME );
//		error = CALL_SYS_BroadcastMsg(_this->data->hFatherOS, IID_OS, cBROADCAST_OBJECT_ITSELF, msg_clsBROADCAST, UNI_MSG_FILE_RENAME, (hOBJECT)old_name, (hOBJECT)new_name, 0);
		dwSize = sizeof(hOBJECT);
		error = CALL_SYS_SendMsg(_this->data->hFatherOS, msg_clsBROADCAST, UNI_MSG_FILE_RENAME, (hOBJECT)old_name, (hOBJECT*)&new_name, &dwSize);
		CALL_SYS_ObjectClose((hOBJECT)old_name);
	}
*/	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::Rename\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//  pointer must be advanced to the next object
// Parameters are:
tERROR pr_call ObjPtr_Delete( hi_ObjPtr _this ) 
{
tERROR error;
tDWORD dwSize = sizeof(_this->data->qwLastObject);
	
	PR_TRACE_A0( _this, "Enter \"ObjPtr::Delete\" method" );
	
	error = CALL_SYS_SendMsg(_this->data->hFatherOS, msg_clsBROADCAST, UNI_MSG_FILE_DELETE, NULL, &_this->data->qwLastObject, &dwSize);
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::Delete\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External (user called) interface method implementation )
// --- Interface "ObjPtr". Method "Reset"
// Extended method comment
//   First call of Next method after Reset must return *FIRST* object in enumeration
tERROR pr_call ObjPtr_Reset( hi_ObjPtr _this, tBOOL to_root )
{
tERROR error ;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::Reset\" method" );
	_this->data->qwLastObject = 0;
	_this->data->bIsCheckForNewFiles = cFALSE;
	_this->data->dwPosOfNewFile = 0;
	_this->data->hCurrentNewFileIo = NULL;
	
	error = CALL_MiniArchiver_Reset(_this->data->hArc);

	PR_TRACE_A1( _this, "Leave \"ObjPtr::Reset\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External (user called) interface method implementation )
// --- Interface "ObjPtr". Method "Clone"
// Extended method comment
//   Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameter "parent":
//   Parent of the new enumerator
tERROR pr_call ObjPtr_Clone( hi_ObjPtr _this, hObjPtr* result ) 
{
hi_ObjPtr hNew;
tERROR error;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::Clone\" method" );

	hNew = NULL;

	error = CALL_SYS_ObjectCreate(_this, (hOBJECT*)&hNew, IID_OBJPTR, PID_UNIVERSAL_ARCHIVER,0);
	if(PR_SUCC(error))
	{
		memcpy(hNew->data, _this->data, sizeof(ObjPtr_Data));
		if(errOK != CALL_SYS_ObjectCreateDone((hOBJECT)hNew))
		{
			CALL_SYS_ObjectClose(hNew);
			hNew=NULL;
			error = errOBJECT_NOT_CREATED;
		}
	}
	else
		error=errOBJECT_NOT_CREATED;

	if ( result )
		*result = (hObjPtr)hNew;
	PR_TRACE_A2( _this, "Leave \"ObjPtr::Clone\" method, ret hObjPtr = %p, error = %u", hNew, error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External (user called) interface method implementation )
// --- Interface "ObjPtr". Method "Next"
// Behaviour comment
//   First call of Next method after creating or Reset must return *FIRST* object in enumeration
tERROR pr_call ObjPtr_Next( hi_ObjPtr _this) 
{
tERROR error;
ObjPtr_Data* data;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::Next\" method" );
	data = _this->data;

// TO DO -- add support for New, Renamed Objects!

	error = errOK;
	if(data->bIsCheckForNewFiles)
	{
		if(!SetNewFileData(_this, ((hi_OS)data->hFatherOS)->data->hNewTree, &data->dwPosOfNewFile, &data->hCurrentNewFileIo))
		{
//			if(!SetRenamedFileData(data, ((hi_OS)data->hFatherOS)->data->hRenamedList, &data->dwPosOfRenamedFile))
			{
				data->hCurrentNewFileIo = NULL;
				data->bIsCheckForNewFiles = cFALSE;
				error = errEND_OF_THE_LIST;
			}
		}
		goto loc_exit;
	}
	
	error = CALL_MiniArchiver_Next(data->hArc, &data->qwLastObject, data->qwLastObject);
    if(PR_SUCC(error))
    {
	    if(((hi_OS)data->hFatherOS)->data->hDeletedTree) // Check for deleted
	    {
		    if(errOK != iCheckForFileInTree(data->qwLastObject, ((hi_OS)data->hFatherOS)->data->hDeletedTree, NULL))
			    return ObjPtr_Next(_this);
	    }
        if(CALL_SYS_PropertyGetBool(_this, pgIS_FOLDER)) // Skip folders
        {
            return ObjPtr_Next(_this);
        }
    }
	else if(error == errEND_OF_THE_LIST && ((hi_OS)data->hFatherOS)->data->hNewTree)	// Check for New
	{
		data->bIsCheckForNewFiles = cTRUE;
		error = ObjPtr_Next(_this);
	}
loc_exit:
	

	PR_TRACE_A1( _this, "Leave \"ObjPtr::Next\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External (user called) interface method implementation )
// --- Interface "ObjPtr". Method "StepUp"
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this) 
{
tERROR error;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::StepUp\" method" );
	error = errNOT_IMPLEMENTED;

// TODO
// Place your code here

	PR_TRACE_A1( _this, "Leave \"ObjPtr::StepUp\" method, ret tBOOL = %u, error = 0x%08x", error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR pr_call ObjPtr_StepDown( hi_ObjPtr _this ) 
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"ObjPtr::StepDown\" method" );
	
	// Place your code here
	
	PR_TRACE_A1( _this, "Leave \"ObjPtr::StepDown\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External (user called) interface method implementation )
// --- Interface "ObjPtr". Method "ChangeTo"
// Parameter "object":
//   The same as "object" parameter of OS::EnumCreate method
tERROR pr_call ObjPtr_ChangeTo( hi_ObjPtr _this, hOBJECT name ) 
{
tERROR error;
// TODO Allocate these mem blocks (not in stack)
tCHAR szNameBuffer[512];
tCHAR buffer[512];
tDWORD dwLen;
hSTRING hObjectName;
//hObjPtr hPtr;
tBOOL IsSansative; 
tDWORD dwNameLen;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::ChangeTo\" method" );
	error = errOBJECT_INCOMPATIBLE;
	CALL_SYS_ObjectCreateQuick(_this, &hObjectName, IID_STRING, PID_STRING, 0);
	IsSansative = CALL_SYS_PropertyGetBool(_this, pgNAME_CASE_SENSITIVE);

	switch(CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID))
	{
		case IID_STRING:
			error = CALL_String_ExportToBuff( (hSTRING) name, &dwLen, cSTRING_WHOLE, szNameBuffer, sizeof(szNameBuffer), 0, cSTRING_Z);
			goto loc_create;
		case IID_OBJPTR:
			CALL_SYS_PropertyGetStr(((hObjPtr)name), 0, pgOBJECT_FULL_NAME, szNameBuffer, sizeof(szNameBuffer), cCP_ANSI);
			goto loc_create;
			//			error = CALL_ObjPtr_Clone((hObjPtr)name, &ret_val);
			//break;
		case IID_IO:
			error = CALL_SYS_PropertyGetStr(((hOBJECT)name), NULL, pgOBJECT_FULL_NAME, szNameBuffer, sizeof(szNameBuffer), cCP_ANSI);
loc_create:
			CALL_ObjPtr_Reset(_this, cTRUE);
			dwNameLen = _toui32(strlen(szNameBuffer));
			
			while (PR_SUCC(error = CALL_ObjPtr_Next(_this)) )
			{
				error = CALL_SYS_PropertyGetStr(_this, 0, pgOBJECT_FULL_NAME, buffer, sizeof(buffer), cCP_ANSI);
				if(PR_SUCC (error))
				{
					if(hObjectName)
					{
						CALL_String_ImportFromProp( hObjectName , NULL, (hOBJECT)_this, pgOBJECT_FULL_NAME);
						if(IsSansative)
						{
							error = CALL_String_CompareBuff(hObjectName, cSTRING_WHOLE, szNameBuffer, dwNameLen , cCP_ANSI, fSTRING_COMPARE_CASE_SENSITIVE | fSTRING_COMPARE_LEN_SENSITIVE);
						}
						else
						{
							error = CALL_String_CompareBuff(hObjectName, cSTRING_WHOLE, szNameBuffer, dwNameLen , cCP_ANSI, fSTRING_COMPARE_LEN_SENSITIVE | fSTRING_COMPARE_UPPER);
						}
						if(error == 0)
						{
							break;
						}
					}
					else if(0 == strcmp(buffer, szNameBuffer))
					{
						break;
					}
				}
			}
			if(error != errOK)
			{
				error = errOBJECT_NOT_FOUND;
			}
			break;
		default:
			error = errOBJECT_INCOMPATIBLE;
			break;
	}
	
	if(hObjectName)
	{
		CALL_SYS_ObjectClose(hObjectName);
	}
	PR_TRACE_A1( _this, "Leave \"ObjPtr::ChangeTo\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( ObjPtr, External (user called) interface method implementation )
// --- Interface "ObjPtr". Method "NewObject"
// Parameter "parent":
//   Parent of the new object
// Parameter "name":
//   name of the new object
// Parameter "iid":
//   Interface id to create object. May be IID_IO or IID_OBJENUM only. Default value (IID_ANY) means IID_IO.
// Parameter "mode":
//   New object open mode
// Parameter "attributes":
//   Attributes of the new object
/*
tERROR pr_call ObjEnum_NewObject( hi_ObjPtr _this, hOBJECT* result, hOBJECT parent, hSTRING name, tIID iid, tDWORD mode, tDWORD attributes ) 
{
tERROR error;
hOS hUniOS;

	PR_TRACE_A0( _this, "Enter \"ObjPtr::NewObject\" method" );

	hUniOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this,IID_OS);

	error =  errPARAMETER_INVALID ;
	if(result != NULL)
	{
// Create New Folder
		if(iid == IID_OBJENUM )
		{
			error =  errNOT_IMPLEMENTED;
//			error = errOBJECT_READ_ONLY;
//			if( CALL_SYS_PropertyGetDWord(_this->data->hArc, pgIS_READONLY) != cTRUE)
//			{
//				
//			}
		}
// Create New Object
		else if(iid == IID_IO || IID_ANY)
		{
			error = CALL_OS_IOCreate( hUniOS, (hIO*)result, (hOBJECT)name, mode, attributes ) ;
		}
	}


	if(error != errOK)
	{
		CALL_SYS_ErrorCodeSet( _this, error );
		if(result)
			*result = NULL;
	}
	PR_TRACE_A2( _this, "Leave \"ObjPtr::NewObject\" method, ret hOBJECT = %p, error = %u", *result, error );
	return error;
}
// AVP Prague stamp end( ObjPtr, External (user called) interface method implementation )
// --------------------------------------------------------------------------------

*/


tERROR iCheckForFileInTree(tQWORD qwObjectId, hTREE Tree, hIO* pIO)
{
tTREENODE node;
tDWORD dwBuf[3];
	
	node = 0;
	if(pIO)
		*pIO = NULL;
	if(Tree)
	{
        *(tQWORD*)(&dwBuf[0]) = qwObjectId;
        *(tDWORD*)(&dwBuf[2]) = 0;

		CALL_Tree_Search(Tree, &node, dwBuf, sizeof(dwBuf));
		if(node && pIO)
		{
			tDWORD dwTmp;
			
			CALL_Tree_NodeDataGet(Tree, &dwTmp, node, dwBuf, sizeof(dwBuf));
			*pIO=(hIO)*(tDWORD*)(&dwBuf[2]);
		}
	}
	if(node != 0)
		return errNOT_OK;
	
	return errOK;
}


tBOOL SetNewFileData(hi_ObjPtr _this, hTREE tree, tDWORD* pdwConter, hIO* pio)
{
tTREENODE node;
tERROR error;
tDWORD i;
tDWORD dwBuf[3];
	
	if(!tree)
		return cFALSE;
	error = CALL_Tree_First(tree, &node);
	if(error || node == 0)
	{
		return cFALSE;
	}
	for(i=0; i < *pdwConter; i++)
	{
		error = CALL_Tree_Next(tree, &node, node);
		if(error || node == 0)
		{
			return cFALSE;
		}
	}
	(*pdwConter)+=1;
	
	CALL_Tree_NodeDataGet(tree, NULL, node, dwBuf, sizeof(dwBuf));

	*pio = (hIO)*((tDWORD*)&dwBuf[2]);
	if(*pio)
	{
		return cTRUE;
	}

	return cFALSE;
}
