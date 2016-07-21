// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  29 October 2007,  15:17 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- mdb_os.c
// Additional info
//    Данный плагин предназначен для работы со следующими объектами Outlook:
//    - почтовая система для указанного Profile, если не указан, то успользуется текущий.
//    - база сообщений (pst файл)
//    - сообщение (Outlook Message)
//
//    Плагин реализован на основе MAPI интерфейса.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define OS_PRIVATE_DEFINITION
// AVP Prague stamp end



#include "mdb_mapi.h"

// AVP Prague stamp begin( Includes for interface,  )
#include "mdb_os.h"
// AVP Prague stamp end



typedef tCHAR tPST_SIGNATURE[4];
tPST_SIGNATURE g_pst_signature = {0x21, 0x42, 0x44, 0x4e};


// AVP Prague stamp begin( Interface comment,  )
// OS interface implementation
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
//
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure "MDB_Data" is described in "O:\Prague\Outlook\mdb_os.c" header file
  // data structure "MDB_Data" is described in "O:\Prague\outlook\mdb_os.h" header file
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD type );
tERROR pr_call OS_ObjectInit( hi_OS _this );
tERROR pr_call OS_ObjectInitDone( hi_OS _this );
tERROR pr_call OS_ObjectClose( hi_OS _this );
tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_OS_vtbl = {
	(tIntFnRecognize)         OS_Recognize,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        OS_ObjectInit,
	(tIntFnObjectInitDone)    OS_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       OS_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        OS_MsgReceive,
	(tIntFnIFaceTransfer)     NULL
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
struct iOSVtbl {
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
static iOSVtbl e_OS_vtbl = {
	(fnpOS_PtrCreate)         OS_PtrCreate,
	(fnpOS_IOCreate)          OS_IOCreate,
	(fnpOS_Copy)              OS_Copy,
	(fnpOS_Rename)            OS_Rename,
	(fnpOS_Delete)            OS_Delete
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call OS_PROP_SaveData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_LoadData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "message database object system", 31 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OS_TYPE, MDB_Data, type, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, MDB_Data, name, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MDB_Data, hRealIO, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(512)) )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)(0x2f)) )
	mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, OS_PROP_SaveData, OS_PROP_LoadData )
	mLOCAL_PROPERTY_BUF( plMDB_PASSWORD, MDB_Data, password, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plMDB_IS_PUBLIC_FOLDERS, MDB_Data, is_public_folders, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plMDB_IS_PRIVATE_FOLDERS, MDB_Data, is_private_folders, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( plMDB_IS_FORCE_UI_SUPPRESS, MDB_Data, is_force_ui_suppress, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_HASH
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter OS::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_MDB,                                // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(MDB_Data),                       // memory size
		0                                       // interface flags
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
	PRProp_Buff l_buff = {buffer, size, tid_PTR, 0};

	PR_TRACE_A0( _this, "Enter *GET* method OS_PROP_SaveData for property pgOBJECT_REOPEN_DATA" );

	error = PRBuffSetDWORD(&l_buff, _this->data->type);
	*out_size = l_buff.out_size;

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

	_this->data->type = *(tDWORD*)buffer;
	CALL_SYS_PropertyInitialized(_this, pgOBJECT_OS_TYPE, cTRUE);

	PR_TRACE_A2( _this, "Leave *SET* method OS_PROP_LoadData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
//! tERROR pr_call OS_Recognize( hOBJECT _that )
//! tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD type )
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD p_type )
{
	tERROR error = errINTERFACE_INCOMPATIBLE;
//	PR_TRACE_A2( 0, "Enter OS::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

	// Place your code here
	if(PR_SUCC(CALL_SYS_ObjectCheck(_that, _that, IID_IO, /*!!!PID_ANY*/PID_NATIVE_FIO, SUBTYPE_ANY, cFALSE)))
	{
		tPST_SIGNATURE	l_pst_signature;
		if(errOK == CALL_IO_SeekRead((hIO)_that, NULL, 0, &l_pst_signature, sizeof(tPST_SIGNATURE)))
			if( !memcmp(&l_pst_signature, &g_pst_signature, sizeof(tPST_SIGNATURE)) )
				error = errOK;
	}

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
	hIO hPstIO = NULL;
	tCHAR *pstname = NULL;
	PR_TRACE_A0( _this, "Enter OS::ObjectInit method" );

	// Place your code here
	hPstIO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this, IID_IO);
	if( hPstIO && OS_Recognize((hOBJECT)hPstIO, 0) == errOK )
	{
		_this->data->hRealIO = hPstIO;
		_this->data->type = MAIL_OS_TYPE_PST;
//		CALL_SYS_PropertyGetStr(hPstIO, NULL, pgOBJECT_FULL_NAME, _this->data->name, MAX_PATH, cCP_ANSI);
		CALL_SYS_PropertyInitialized(_this, pgOBJECT_NAME, cTRUE);
		CALL_SYS_PropertyInitialized(_this, pgOBJECT_OS_TYPE, cTRUE);
	}
	_this->data->is_public_folders = cFALSE;
	_this->data->is_private_folders = cFALSE;
	_this->data->is_force_ui_suppress = cFALSE;

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
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter OS::ObjectInitDone method" );

	// Place your code here

	error = SessionCreate(_this->data);

	if( PR_SUCC(error) )
		CALL_SYS_RegisterMsgHandler(_this, pmc_OS_CHANGE, pm_OS_CHANGE, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

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
	error = SessionDestroy(_this->data);

	PR_TRACE_A1( _this, "Leave OS::ObjectClose method, ret tERROR = 0x%08x", error );
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
tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter OS::MsgReceive method" );

	// Place your code here
	if( msg_cls_id == pmc_OS_CHANGE && msg_id == pm_OS_CHANGE )
		_this->data->fModified = cTRUE;

	PR_TRACE_A1( _this, "Leave OS::MsgReceive method, ret %terr", error );
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
	MDBObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter OS::PtrCreate method" );

	// Place your code here
	error = ObjInfoGet(&obj_info, (hOBJECT)_this, name, 0);

	if( PR_SUCC(error) )
		error = CALL_SYS_ObjectCreateQuick(_this, &ret_val, IID_OBJPTR, PID_MDB, 0);

	if( PR_SUCC(error) )
		error = EnumSetObject(CUST_TO_MDBEnum_Data(ret_val), &obj_info);

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
	hIO    ret_val = NULL;
	MDBObj_Info obj_info;

	PR_TRACE_A0( _this, "Enter OS::IOCreate method" );

	// Place your code here
	error = ObjInfoGet(&obj_info, (hOBJECT)_this, name, open_mode);
	if( PR_SUCC(error) && result )
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
	tERROR error = errOK;
	MDBObj_Info src_obj, dst_obj;
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
	tERROR error = errOK;
	MDBObj_Info old_obj, new_obj;
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
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter OS::Delete method" );

	// Place your code here
	if( CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID) == IID_IO )
		error = CALL_SYS_PropertySetBool(name, pgOBJECT_DELETE_ON_CLOSE, cTRUE);
	else
	{
		MDBObj_Info obj_info;
		if( !PR_FAIL(ObjInfoGet(&obj_info, (hOBJECT)_this, name, 0)) )
			error = ObjInfoDelete(&obj_info);
	}

	PR_TRACE_A1( _this, "Leave OS::Delete method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



