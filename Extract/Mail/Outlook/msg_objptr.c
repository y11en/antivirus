// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  19 March 2003,  13:36 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_objptr.c
// Additional info
//    Outlook message object system
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define ObjPtr_PRIVATE_DEFINITION
// AVP Prague stamp end




// AVP Prague stamp begin( Includes for interface,  )
#include "msg_mapi.h"
#include "msg_objptr.h"

#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
//    Интерфейс позволяет навигировать по содержимому Outlook сообщения (тела, аттачменты, вложенные сообщения, OLE объекты). Объект может быть создан следующими способами:
//    - вызов ObjectCreate с родителем OS.
//    - вызов PtrCreate интерфейса OS.
// Short comments -- pointer to the object
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure "MsgEnum_Data" is described in "O:\Prague\outlook\msg_objptr.h" header file
  // data structure "MsgEnum_Data" is described in "O:\Praque\Outlook\msg_objptr.h" header file
  // data structure "" is described in "O:\Praque\Outlook\msg_objptr.h" header file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MsgEnum_Data*      data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call ObjPtr_ObjectInitDone( hi_ObjPtr _this );
tERROR pr_call ObjPtr_ObjectClose( hi_ObjPtr _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_ObjPtr_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
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
tERROR pr_call ObjPtr_PROP_enum_prop_get( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(ObjPtr_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "message enumerator object", 26 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, MsgEnum_Data, Origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, ObjPtr_PROP_enum_prop_get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, ObjPtr_PROP_enum_prop_get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, ObjPtr_PROP_enum_prop_get, NULL )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MsgEnum_Data, hOS, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgIS_FOLDER, MsgEnum_Data, fFolder, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PTR_STATE, MsgEnum_Data, ptrState, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE, ObjPtr_PROP_enum_prop_get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, ObjPtr_PROP_enum_prop_get, NULL )
	mSHARED_PROPERTY( pgOBJECT_ATTRIBUTES, ((tDWORD)(0)) )
mPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )

//#error Caution !!!
// You have to implement *GET* or *SET* propetries for:
//   - pgMASK
//   - pgOBJECT_HASH
//   - pgOBJECT_REOPEN_DATA


//#error Caution !!!

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
		PID_MAILMSG,                            // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
		VID_MEZHUEV,                            // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(MsgEnum_Data),                   // memory size
		IFACE_EXPLICIT_PARENT                   // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave ObjPtr::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, enum_prop_get )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_SIZE
//  -- OBJECT_SIZE_Q
tERROR pr_call ObjPtr_PROP_enum_prop_get( hi_ObjPtr _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	MsgObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method ObjPtr_PROP_enum_prop_get" );

	// Place your code here
	error = EnumGetObject(_this->data, &obj_info);
	if( PR_SUCC(error) )
		error = ObjInfoGetProp(&obj_info, prop, buffer, size, out_size);

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method ObjPtr_PROP_enum_prop_get, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
	IMessage *l_message = NULL;
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectInitDone method" );

	// Place your code here
	_this->data->hOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this,IID_OS);
	CALL_SYS_PropertyGet(_this->data->hOS, NULL, plMAILMSG_MESSAGE, &l_message , sizeof(l_message));

	error = EnumCreate(_this->data, l_message);

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectInitDone method, ret tERROR = 0x%08x", error );
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
	error = EnumDestroy(_this->data);

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectClose method, ret tERROR = 0x%08x", error );
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
	if( !name )
		error = CALL_OS_IOCreate(_this->data->hOS, &ret_val, (hOBJECT)_this, access_mode, open_mode);
	else
	{
		MsgObj_Info obj_info;
		error = ObjInfoGet(&obj_info, (hOBJECT)_this, (hOBJECT)name, open_mode);
		if( PR_SUCC(error) )
			error = ObjInfoIOCreate(&obj_info, _this->data->hOS, &ret_val, open_mode, access_mode);
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
	error = CALL_OS_Copy(_this->data->hOS, (hOBJECT)_this, dst_name, overwrite);

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
	error = CALL_OS_Rename(_this->data->hOS, (hOBJECT)_this, new_name, overwrite);

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
	error = CALL_OS_Delete(_this->data->hOS, (hOBJECT)_this);

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
	error = EnumReset(_this->data);

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
	error = CALL_OS_PtrCreate(_this->data->hOS, result, (hOBJECT)_this);

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
	error = EnumNext(_this->data);

	PR_TRACE_A1( _this, "Leave ObjPtr::Next method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// Parameters are:
tERROR pr_call ObjPtr_StepUp( hi_ObjPtr _this )
{
	tERROR error = errOBJECT_INCOMPATIBLE;
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
	tERROR error = errOBJECT_INCOMPATIBLE;
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
	MsgObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter ObjPtr::ChangeTo method" );

	// Place your code here
	error = ObjInfoGet(&obj_info, (hOBJECT)_this, name, 0);
	if( PR_SUCC(error) )
		error = EnumSetObject(_this->data, &obj_info);

	PR_TRACE_A1( _this, "Leave ObjPtr::ChangeTo method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



