// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  10 April 2003,  11:47 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- „тение секторов дл€ операционной системы Windows NT
// Author      -- Sobko
// File Name   -- objptr.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <prague/pr_cpp.h>
#include "objptr.h"
// AVP Prague stamp end

#include "ptrthread.h"
#include "llda.h"

#define IO_This		_this->data
//#define _NAMES_BUF_LEN (1024 * 16 * sizeof(WCHAR))


// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_PrivatePtr
{
	tBOOL			m_Tmp;
	tObjPtrState	m_State;
	cDiskAccess*	m_pLLDisk;
} PrivatePtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	PrivatePtr*        data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInit( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectPreClose( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
tERROR pr_call ObjPtr_MsgReceive( hi_ObjPtr _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       ObjPtr_ObjectInit,
	(tIntFnObjectInitDone)   ObjPtr_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   ObjPtr_ObjectPreClose,
	(tIntFnObjectClose)      ObjPtr_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       ObjPtr_MsgReceive,
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
tERROR pr_call ObjPtr_PROP_PtrPropRead( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// Interface "ObjPtr". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, L"enum disk devices", 36 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_PHYSICAL_DISK)) )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_BASED_ON, ObjPtr_PROP_PtrPropRead, NULL )
	mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY_PTR( pgMASK, "*", 2 )
	mLOCAL_PROPERTY_FN( pgOBJECT_PTR_STATE, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, ObjPtr_PROP_PtrPropRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_CODEPAGE, ObjPtr_PROP_PtrPropRead, NULL )
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
		PID_WINDISKIO,                          // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
		VID_SOBKO,                              // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(PrivatePtr),                     // memory size
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



// AVP Prague stamp begin( Property method implementation, PtrPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_AVAILABILITY
//  -- OBJECT_BASED_ON
//  -- OBJECT_PTR_STATE
//  -- OBJECT_SIZE
//  -- OBJECT_SIZE_Q
//  -- OBJECT_ATTRIBUTES
tERROR pr_call ObjPtr_PROP_PtrPropRead( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	tDWORD len = 0;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method ObjPtr_PROP_PtrPropRead" );

	error = errNOT_SUPPORTED;
	*out_size = 0;

	switch ( prop )
	{
		case pgOBJECT_CODEPAGE:
			error = errOK;
			_integrated_type_prop_get( tCODEPAGE, cCP_UNICODE );
			break;
		
		case pgOBJECT_NAME:
			if (IO_This->m_pLLDisk)
				error = IO_This->m_pLLDisk->ExportNameToBuff( out_size, buffer, size );
			break;

		case pgOBJECT_PATH:
			break;
		
		case pgOBJECT_FULL_NAME:
			error = IO_This->m_pLLDisk->ExportNameToBuff( out_size, buffer, size );
			break;

		case pgOBJECT_PTR_STATE:
			_integrated_type_prop_get( tObjPtrState, IO_This->m_State );
			break;

		case pgOBJECT_SIZE:
			if (IO_This->m_pLLDisk)
			{
				tDWORD size = (tDWORD) IO_This->m_pLLDisk->GetSize();
				_integrated_type_prop_get( tDWORD, (tDWORD) size );
			}
			break;

		case pgOBJECT_SIZE_Q:
			if (IO_This->m_pLLDisk)
			{
				tQWORD size = (tQWORD) IO_This->m_pLLDisk->GetSize();
				_integrated_type_prop_get( tQWORD, size );
			}
			break;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* multyproperty method ObjPtr_PROP_PtrPropRead, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
	
	IO_This->m_pLLDisk = new cDrvAccess( (hOBJECT) _this );
	if (IO_This->m_pLLDisk)
	{
		if (!IO_This->m_pLLDisk->Init( NULL ))
		{
			delete IO_This->m_pLLDisk;
			IO_This->m_pLLDisk = NULL;
		}
	}

	if (!IO_This->m_pLLDisk)
	{
		IO_This->m_pLLDisk = new cGenericAccess( (hOBJECT) _this);
		if (IO_This->m_pLLDisk)
		{
			if (!IO_This->m_pLLDisk->Init( NULL ))
			{
				delete IO_This->m_pLLDisk;
				IO_This->m_pLLDisk = NULL;
			}
		}
	}

	if (IO_This->m_pLLDisk)
		error = errOK;
	else
		error = errUNEXPECTED;

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInit method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInitDone method" );

	// Place your code here
	error = errOK;
	IO_This->m_State = cObjPtrState_BEFORE;

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInitDone method, ret tERROR = 0x%08x", error );
	
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

	if (IO_This->m_pLLDisk)
		delete IO_This->m_pLLDisk;
	IO_This->m_pLLDisk = NULL;


	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectPreClose method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call ObjPtr_MsgReceive( hi_ObjPtr _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter ObjPtr::MsgReceive method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave ObjPtr::MsgReceive method, ret tERROR = 0x%08x", error );
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
	if (fOMODE_OPEN_IF_EXIST != open_mode)
		error = errNOT_SUPPORTED;
	else
	{
		hSTRING hStr = 0;
		if (PR_SUCC(CALL_SYS_ObjectCreateQuick( (hOBJECT)_this, &hStr, IID_STRING, PID_ANY, 0)))
		{
			CALL_String_SetCP(hStr, cCP_UNICODE);
			error = CALL_SYS_ObjectCreate((hOBJECT)_this, &ret_val, IID_IO, PID_WINDISKIO, 0);
			if (PR_SUCC(error))
			{
				tDWORD len;
				if (PR_SUCC(CALL_String_AddFromProp(hStr, &len, (hOBJECT) _this, pgOBJECT_NAME)))
				{
					error = CALL_String_ExportToProp(hStr, &len, cSTRING_WHOLE, (hOBJECT) ret_val, pgOBJECT_NAME);
					if (PR_FAIL(CALL_SYS_ObjectCreateDone(ret_val)))
					{
						CALL_SYS_ObjectClose(ret_val);
						ret_val = 0;
						error = errOBJECT_NOT_CREATED;
					}
					else
						error = errOK;
				}
			}
			
			CALL_SYS_ObjectClose(hStr);
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
	error = errNOT_SUPPORTED;

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
	error = errNOT_SUPPORTED;

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
	error = errNOT_SUPPORTED;

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
	if (IO_This->m_pLLDisk)
	{
		error = errOK;
		IO_This->m_pLLDisk->Reset();
	}
	else
		error = errUNEXPECTED;

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
	error = errNOT_SUPPORTED;
	
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

	if (IO_This->m_pLLDisk)
		error = IO_This->m_pLLDisk->Next();
	else
		error = errUNEXPECTED;

	if (PR_FAIL( error ))
		IO_This->m_State = cObjPtrState_AFTER;
	else
		IO_This->m_State = cObjPtrState_PTR;

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
	error = errNOT_SUPPORTED;

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
	error = errNOT_SUPPORTED;

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
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



