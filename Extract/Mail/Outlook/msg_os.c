// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  19 March 2003,  13:31 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_os.c
// Additional info
//    Outlook message object system
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define OS_PRIVATE_DEFINITION
// AVP Prague stamp end



#include "msg_mapi.h"
#include "mdb_io.h"

// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "msg_os.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
//    Интерфейс предназначен для построения объектной системы на основе Outlook сообщения.
//    Интерфейс предназначен для построения объектной системы на основе Outlook сообщения.
// Short comments -- object system interface
//    Interface defines behavior of an object system. It responds for
//      - enumerate IO and Folder objects by creating Folder objects.
//      - create and delete IO and Folder objects
//    There are two possible ways to create or open IO object on OS. First is three steps protocol:
//      -- call ObjectCreate system method and get new IO (Folder) object
//      -- dictate properties of new object
//      -- call ObjectCreateDone system method and have working object
//    Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//    Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//    Способы создания объекта OS:
//    - при инициализации нужно указать свойство MAILMSG_MESSAGE (IMAPIMessage*)
//    - указать объект MDB_IO в качестве родителя в вызове ObjectCreate
//
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure "Msg_Data" is described in "O:\Prague\outlook\msg_os.h" header file
  // data structure "Msg_Data" is described in "O:\Praque\Outlook\msg_os.h" header file
  // data structure "" is described in "O:\Praque\Outlook\msg_os.h" header file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_OS 
{
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	Msg_Data*          data; // pointer to the "OS" data structure
} *hi_OS;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call OS_Recognize( hOBJECT _that );
tERROR pr_call OS_ObjectInit( hi_OS _this );
tERROR pr_call OS_ObjectInitDone( hi_OS _this );
tERROR pr_call OS_ObjectClose( hi_OS _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_OS_vtbl = 
{
	(tIntFnRecognize)        OS_Recognize,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       OS_ObjectInit,
	(tIntFnObjectInitDone)   OS_ObjectInitDone,
//! 	(tIntFnObjectInitDone)   NULL,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      OS_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpOS_PtrCreate) ( hi_OS _this, hObjPtr* result, hOBJECT name );              // -- opens ObjPtr object;
typedef   tERROR (pr_call *fnpOS_IOCreate)  ( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ); // -- opens IO object;
typedef   tERROR (pr_call *fnpOS_Copy)      ( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ); // -- copies object inside OS;
typedef   tERROR (pr_call *fnpOS_Rename)    ( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ); // -- rename/move object by name inside OS;
typedef   tERROR (pr_call *fnpOS_Delete)    ( hi_OS _this, hOBJECT name );              // -- phisically deletes an entity on OS;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iOSVtbl 
{
	fnpOS_PtrCreate  PtrCreate;
	fnpOS_IOCreate   IOCreate;
	fnpOS_Copy       Copy;
	fnpOS_Rename     Rename;
	fnpOS_Delete     Delete;
}; // "OS" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name );
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iOSVtbl e_OS_vtbl = 
{
	OS_PtrCreate,
	OS_IOCreate,
	OS_Copy,
	OS_Rename,
	OS_Delete
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call OS_PROP_SaveData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_LoadData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_Get( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_Set( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "outlook message object system", 30 )
	mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
//	mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "outlook object system", 22 )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, Msg_Data, hIO, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(MAX_PATH)) )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)(0x2f)) )
	mLOCAL_PROPERTY_BUF( plMAILMSG_MESSAGE, Msg_Data, message, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_BUF( plMAILMSG_ISEMBENDED, Msg_Data, isembended, cPROP_BUFFER_READ_WRITE )
	mLAST_CALL_PROPERTY( OS_PROP_Get, OS_PROP_Set )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, OS_PROP_SaveData, OS_PROP_LoadData )
	mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )

//#error Caution !!!
// You have to implement *GET* or *SET* propetries for:
//   - pgOBJECT_HASH


//#error Caution !!!

// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter OS::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_MAILMSG,                            // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(Msg_Data),                       // memory size
		IFACE_EXPLICIT_PARENT                   // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"OS(IID_OS) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave OS::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SaveData )
// Interface "OS". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call OS_PROP_SaveData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method OS_PROP_SaveData for property pgOBJECT_REOPEN_DATA" );

	return errNOT_SUPPORTED;
	*out_size = sizeof(tDWORD);

	PR_TRACE_A2( _this, "Leave *GET* method OS_PROP_SaveData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, LoadData )
// Interface "OS". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call OS_PROP_LoadData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method OS_PROP_LoadData for property pgOBJECT_REOPEN_DATA" );

	*out_size = sizeof(tDWORD);

	PR_TRACE_A2( _this, "Leave *SET* method OS_PROP_LoadData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Get )
// Interface "OS". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call OS_PROP_Get( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method for *LAST CALL* property " );

	error = MessageGetProp(_this->data, prop, buffer, size, out_size);

	PR_TRACE_A2( _this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Set )
// Interface "OS". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call OS_PROP_Set( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method for *LAST CALL* property " );

	error = MessageSetProp(_this->data, prop, buffer, size, out_size);

	PR_TRACE_A2( _this, "Leave *SET* method for property *LAST CALL*, ret tDWORD = %u, error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, Recognize )
// Extended method comment
//    Static method. Kernel calls this method wihtout any created objects of this interface.
//    Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//    Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//    It must be quick (preliminary) analysis
// Parameters are:
tERROR pr_call OS_Recognize( hOBJECT _that )
{
	tERROR error = errINTERFACE_INCOMPATIBLE;
	IMessage *l_message = NULL;
//	PR_TRACE_A2( 0, "Enter OS::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

	// Place your code here
	if( !PR_FAIL(CALL_SYS_ObjectCheck(_that, _that, IID_IO, PID_MAILMSG, 0, cFALSE)) )
	{
		if( CUST_TO_MsgIO_Data(_that)->objmethod == ATTACH_EMBEDDED_MSG )
			error = errOK;
	}
	else if( 
		!PR_FAIL(CALL_SYS_ObjectCheck(_that, _that, IID_ANY, PID_MDB, 0, cFALSE)) &&
		!PR_FAIL(CALL_SYS_PropertyGet(_that, NULL, plMDB_MESSAGE, &l_message , sizeof(l_message))) )
		error = errOK;

//	PR_TRACE_A2( 0, "Leave OS::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
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
tERROR pr_call OS_ObjectInit( hi_OS _this )
{
	tERROR error = errOK;
	hOBJECT base_on = NULL;
	PR_TRACE_A0( _this, "Enter OS::ObjectInit method" );

	// Place your code here
	base_on = CALL_SYS_ParentGet((hOBJECT)_this, IID_IO);

	if( base_on && PR_SUCC(OS_Recognize(base_on)) )
	{
		_this->data->hIO = (hIO)base_on;
		if( PR_SUCC(CALL_SYS_ObjectCheck(_this, base_on, IID_IO, PID_MAILMSG, 0, cFALSE)) )
		{
			_this->data->isembended = cTRUE;
			CALL_SYS_PropertyInitialized(_this, plMAILMSG_MESSAGE, cTRUE);
		}
		else
			CALL_SYS_PropertySetPtr(_this, plMAILMSG_MESSAGE, CALL_SYS_PropertyGetPtr(base_on, plMDB_MESSAGE));
	}

	PR_TRACE_A1( _this, "Leave OS::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call OS_ObjectInitDone( hi_OS _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter OS::ObjectInitDone method" );

	// Place your code here
	error = MessageCreate(_this->data);

	PR_TRACE_A1( _this, "Leave OS::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call OS_ObjectClose( hi_OS _this )
{

	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter OS::ObjectClose method" );

	// Place your code here
	if( _this->data->fSaveChanges )
		MessageCommit(_this->data);

	error = MessageDestroy(_this->data);

	PR_TRACE_A1( _this, "Leave OS::ObjectClose method, ret tERROR = 0x%08x", error );
	return error;
}
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
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name )
{
	tERROR  error = errNOT_IMPLEMENTED;
	hObjPtr ret_val = NULL;
	MsgObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter OS::PtrCreate method" );

	// Place your code here
	error = ObjInfoGet(&obj_info, (hOBJECT)_this, name, 0);

	if( PR_SUCC(error) )
		error = CALL_SYS_ObjectCreateQuick(_this, &ret_val, IID_OBJPTR, PID_MAILMSG, 0);

	if( PR_SUCC(error) )
		error = EnumSetObject(CUST_TO_MsgEnum_Data(ret_val), &obj_info);

	if( PR_FAIL(error) && ret_val )
	{
		CALL_SYS_ObjectClose(ret_val);
		ret_val = NULL;
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave OS::PtrCreate method, ret hObjPtr = %u, error = 0x%08x", ret_val, error );
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
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode )
{
	tERROR error = errNOT_IMPLEMENTED;
	MsgObj_Info obj_info;
	hIO    ret_val = NULL;
	PR_TRACE_A0( _this, "Enter OS::IOCreate method" );

	// Place your code here
	error = ObjInfoGet(&obj_info, (hOBJECT)_this, name, open_mode);
	if( PR_SUCC(error) )
		error = ObjInfoIOCreate(&obj_info, (hOS)_this, &ret_val, open_mode, access_mode);

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave OS::IOCreate method, ret hIO = %S, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	MsgObj_Info src_obj, dst_obj;
	PR_TRACE_A0( _this, "Enter OS::Copy method" );

	// Place your code here
	if( PR_FAIL(ObjInfoGet(&src_obj, (hOBJECT)_this, src_name, 0)) )
		error = errOBJECT_INCOMPATIBLE;
	if( PR_FAIL(ObjInfoGet(&dst_obj, (hOBJECT)_this, dst_name, 0)) )
		error = errOBJECT_INCOMPATIBLE;

	if( PR_SUCC(error) )
		error = ObjInfoCopy(&src_obj, &dst_obj, cFALSE, overwrite);

	PR_TRACE_A1( _this, "Leave OS::Copy method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Extended method comment
//    Renames/moves object by name inside single OS
//    Must move object through folders if nessesary
// Parameters are:
//   "new_name"  : Target name
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	MsgObj_Info old_obj, new_obj;
	PR_TRACE_A0( _this, "Enter OS::Rename method" );

	// Place your code here
	if( PR_FAIL(ObjInfoGet(&old_obj, (hOBJECT)_this, old_name, 0)) )
		error = errOBJECT_INCOMPATIBLE;
	if( PR_FAIL(ObjInfoGet(&new_obj, (hOBJECT)_this, new_name, 0)) )
		error = errOBJECT_INCOMPATIBLE;

	if( PR_SUCC(error) )
		error = ObjInfoCopy(&old_obj, &new_obj, cFALSE, overwrite);

	PR_TRACE_A1( _this, "Leave OS::Rename method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    phisically deletes an entity on OS
// Behaviour comment
//    if parameter "object" is really hIO or hObjPtr must close it and delete after close.
// Parameters are:
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter OS::Delete method" );

	// Place your code here
	if( CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID) == IID_IO )
		error = CALL_SYS_PropertySetBool(name, pgOBJECT_DELETE_ON_CLOSE, cTRUE);
	else
	{
		MsgObj_Info obj_info;
		if( !PR_FAIL(ObjInfoGet(&obj_info, (hOBJECT)_this, name, 0)) )
			error = ObjInfoDelete(&obj_info);
	}

	PR_TRACE_A1( _this, "Leave OS::Delete method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



