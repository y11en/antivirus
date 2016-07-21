// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  18 October 2002,  14:56 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Test
// Sub project -- Not defined
// Purpose     -- making images form io
// Author      -- Sobko
// File Name   -- objptr.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include "objptr.h"
#include "io.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_PtrPrivate 
{
	hSTRING m_Path; // --
	hObjPtr m_hPtr; // --
} PtrPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	PtrPrivate*        data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectPreClose( hi_ObjPtr _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       ObjPtr_ObjectInit,
	(tIntFnObjectInitDone)   NULL,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   ObjPtr_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
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



// AVP Prague stamp begin( External method table prototypes,  )







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
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
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
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call ObjPtr_PROP_GetObjectPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_SetObjectPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetFullName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetObjectState( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetObjectSize( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call ObjPtr_PROP_GetObjectSizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
const tQWORD ObjPtr_OBJECT_HASH = 0; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "for enumerationg saved subst io", 32 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_ANY)) )
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "SubstIO_Ptr", 12 )
	mLOCAL_PROPERTY_REQUIRED_FN( pgOBJECT_PATH, ObjPtr_PROP_GetObjectPath, ObjPtr_PROP_SetObjectPath )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY_PTR( pgMASK, "*", 2 )
	mLOCAL_PROPERTY_FN( pgOBJECT_PTR_STATE, ObjPtr_PROP_GetObjectState, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_GetObjectSize, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_GetObjectSizeQ, NULL )
	mSHARED_PROPERTY( pgOBJECT_ATTRIBUTES, ((tDWORD)(0)) )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_GetFullName, NULL )
	mSHARED_PROPERTY_PTR( pgOBJECT_HASH, ObjPtr_OBJECT_HASH, sizeof(ObjPtr_OBJECT_HASH) )
mPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "ObjPtr". Register function
tERROR pr_call ObjPtr_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter ObjPtr::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OBJPTR,                             // interface identifier
		PID_SUBSTIO,                            // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
		VID_SOBKO,                              // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(PtrPrivate),                     // memory size
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



// AVP Prague stamp begin( Property method implementation, GetObjectPath )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR pr_call ObjPtr_PROP_GetObjectPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetObjectPath for property pgOBJECT_PATH" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetObjectPath for property pgOBJECT_PATH, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetObjectPath )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR pr_call ObjPtr_PROP_SetObjectPath( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method ObjPtr_PROP_SetObjectPath for property pgOBJECT_PATH" );

	*out_size = 0;
	CALL_String_ImportFromBuff(_this->data->m_Path, out_size, buffer, size, cCP_UNICODE, cSTRING_CONTENT_ONLY);

	PR_TRACE_A2( _this, "Leave *SET* method ObjPtr_PROP_SetObjectPath for property pgOBJECT_PATH, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetFullName )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call ObjPtr_PROP_GetFullName( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetFullName for property pgOBJECT_FULL_NAME" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetFullName for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectState )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PTR_STATE
tERROR pr_call ObjPtr_PROP_GetObjectState( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetObjectState for property pgOBJECT_PTR_STATE" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetObjectState for property pgOBJECT_PTR_STATE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectSize )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR pr_call ObjPtr_PROP_GetObjectSize( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetObjectSize for property pgOBJECT_SIZE" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetObjectSize for property pgOBJECT_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectSizeQ )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_GetObjectSizeQ( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method ObjPtr_PROP_GetObjectSizeQ for property pgOBJECT_SIZE_Q" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method ObjPtr_PROP_GetObjectSizeQ for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
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
	error = errOK;
	_this->data->m_Path = NULL;
	_this->data->m_hPtr = NULL;
	if (PR_FAIL(CALL_SYS_ObjectCreateQuick(_this, &_this->data->m_Path, IID_STRING, PID_ANY, SUBTYPE_ANY)))
		error = errOBJECT_CANNOT_BE_INITIALIZED;

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call ObjPtr_ObjectPreClose( hi_ObjPtr _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectPreClose method" );
	
	// Place your code here
	error = errOK;
	if (_this->data->m_Path != NULL)
	{
		CALL_SYS_ObjectClose(_this->data->m_Path);
		_this->data->m_Path = NULL;
	}
	
	ObjPtr_Reset(_this, cTRUE);
	
	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectPreClose method, ret tERROR = 0x%08x", error );
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
	if (PR_SUCC(error = CALL_SYS_ObjectCreateQuick(_this, &ret_val, IID_IO, PID_SUBSTIO, SUBTYPE_ANY)))
	{
		tDWORD err_len = sizeof(tERROR);
		hIO htmpIO = NULL;
		
		error = errOK;
		if (PR_FAIL(error = CALL_ObjPtr_IOCreate((hi_ObjPtr)_this->data->m_hPtr, &htmpIO, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST)))
			error = errNOT_SUPPORTED;
		else
		{
			if (PR_FAIL(CALL_SYS_SendMsg(ret_val, pmc_SUBST_IO, pm_SUBST_IO_RESTORE_CONTEXT, htmpIO, &error, &err_len)))
				error = errNOT_SUPPORTED;
			
			CALL_SYS_ObjectClose(htmpIO);
		}
		
		if (PR_FAIL(error))
		{
			CALL_SYS_ObjectClose(ret_val);
			ret_val = NULL;
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave ObjPtr::IOCreate method, ret hIO = %S, error = 0x%08x", ret_val, error );
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

	PR_TRACE_A1( _this, "Leave ObjPtr::Copy method, ret tERROR = 0x%08x", error );
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

	PR_TRACE_A1( _this, "Leave ObjPtr::Rename method, ret tERROR = 0x%08x", error );
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

	PR_TRACE_A1( _this, "Leave ObjPtr::Delete method, ret tERROR = 0x%08x", error );
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
	if (_this->data->m_hPtr != NULL)
	{
		CALL_SYS_ObjectClose(_this->data->m_hPtr);
		_this->data->m_hPtr = NULL;
	}

	PR_TRACE_A1( _this, "Leave ObjPtr::Reset method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A2( _this, "Leave ObjPtr::Clone method, ret hObjPtr = %u, error = 0x%08x", ret_val, error );
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
	error = errUNEXPECTED;
	if (_this->data->m_hPtr == NULL)
	{
		if (PR_SUCC(error = CALL_SYS_ObjectCreate(_this, &_this->data->m_hPtr, IID_OBJPTR,PID_NATIVE_FIO, 0)))
			if (PR_SUCC(error = CALL_String_ExportToProp(_this->data->m_Path, NULL, cSTRING_WHOLE, (hOBJECT) _this->data->m_hPtr, pgOBJECT_PATH)))
				if (PR_SUCC(error = CALL_SYS_ObjectCreateDone(_this->data->m_hPtr)))
					error = errOK;
				
				if (PR_FAIL(error) && (_this->data->m_hPtr != NULL))
				{
					CALL_SYS_ObjectClose(_this->data->m_hPtr);
					_this->data->m_hPtr = NULL;
				}
	}
	
	if (_this->data->m_hPtr == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
	{
		do
		{
			if (PR_SUCC(error = CALL_ObjPtr_Next((hi_ObjPtr)_this->data->m_hPtr)))
			{
				hIO htmpIO = NULL;
				if (PR_SUCC(ObjPtr_IOCreate(_this, &htmpIO, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST)))
				{
					CALL_SYS_ObjectClose(htmpIO);
					break;
				}
			}
		} while (error == errOK);

		if (PR_FAIL(error))
			error = errEND_OF_THE_LIST;
	}

	PR_TRACE_A1( _this, "Leave ObjPtr::Next method, ret tERROR = 0x%08x", error );
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

	PR_TRACE_A1( _this, "Leave ObjPtr::StepUp method, ret tERROR = 0x%08x", error );
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

	PR_TRACE_A1( _this, "Leave ObjPtr::StepDown method, ret tERROR = 0x%08x", error );
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

	PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



