// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  19 November 2004,  16:46 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- msoe
// Sub project -- msoe
// Purpose     -- плагин для работы с Outlook Express
// Author      -- Mezhuev
// File Name   -- msoe_os.c
// Additional info
//    Данный плагин предназначен для работы со следующими объектами Outlook Express:
//    - почтовая система (идентифицируется по Identity, если не указан, то успользуется текущий)
//    - база сообщений в формате dbx.
//    - сообщение в формате RFC822 (eml файл)
//    - тела и аттачменты сообщения (message part)
//
//    Интерфейсы данного плагина не поддерживают идентификацию объектов по строке!!!
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define OS_PRIVATE_DEFINITION
// AVP Prague stamp end



#include "msoe_base.h"

// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
// AVP Prague stamp end



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
// Interface OS. Inner data structure

typedef struct tag_MSOE_Data {
//! 	tCHAR        osname[512]; // --
	PROEObject * osclass;     // --
	hIO          hIOBase;     // --
	tDWORD       ostype;      // --
	tBOOL        fModified;   // --
} MSOE_Data;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_OS {
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MSOE_Data*         data; // pointer to the "OS" data structure
} *hi_OS;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD p_type );
tERROR pr_call OS_ObjectInit( hi_OS _this );
tERROR pr_call OS_ObjectInitDone( hi_OS _this );
tERROR pr_call OS_ObjectClose( hi_OS _this );
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
	(tIntFnMsgReceive)        NULL,
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
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT p_name );
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT p_name, tDWORD p_access_mode, tDWORD p_open_mode );
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT p_src_name, hOBJECT p_dst_name, tBOOL p_overwrite );
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT p_old_name, hOBJECT p_new_name, tBOOL p_overwrite );
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT p_name );
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
tERROR pr_call OS_PROP_GetProp( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_SaveData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_LoadData( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call OS_PROP_SetProp( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
const tQWORD OS_OBJECT_HASH = 0; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define OS_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_msoe.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "msoe object system", 19 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OS_TYPE, MSOE_Data, ostype, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, OS_PROP_GetProp, NULL )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MSOE_Data, hIOBase, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(512)) )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)(0x2f)) )
	mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY_PTR( pgOBJECT_HASH, OS_OBJECT_HASH, sizeof(OS_OBJECT_HASH) )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, OS_PROP_SaveData, OS_PROP_LoadData )
	mLAST_CALL_PROPERTY( OS_PROP_GetProp, OS_PROP_SetProp )
mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter OS::Register method" );

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_OS,                                 // interface identifier
//! 		PID_MSOE,                               // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		OS_VERSION,                             // interface version
//! 		VID_MEZHUEV,                            // interface developer
//! 		&i_OS_vtbl,                             // internal(kernel called) function table
//! 		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
//! 		&e_OS_vtbl,                             // external function table
//! 		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
//! 		OS_PropTable,                           // property table
//! 		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
//! 		sizeof(MSOE_Data),                      // memory size
//! 		IFACE_EXPLICIT_PARENT                   // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_MSOE,                               // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(MSOE_Data),                      // memory size
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
	PR_TRACE_A0( _this, "Enter *GET* method OS_PROP_SaveData for property pgOBJECT_REOPEN_DATA" );

	if( _this->data->ostype > 1 )
		return errNOT_SUPPORTED;

	PRPrmBuff l_buff(buffer, size);
	if( !(l_buff << _this->data->ostype) )
		error = errBUFFER_TOO_SMALL;
	else
		*out_size = l_buff.m_cur_size;

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

	PRPrmBuff l_buff(buffer, size);
	l_buff >> _this->data->ostype;
	*out_size = l_buff.m_cur_size;

	CALL_SYS_PropertyInitialized(_this, pgOBJECT_OS_TYPE, cTRUE);

	PR_TRACE_A2( _this, "Leave *SET* method OS_PROP_LoadData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetProp )
// Interface "OS". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call OS_PROP_GetProp( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter *GET* method for *LAST CALL* property " );

	if( !obj )
		return errOBJECT_INVALID;

	PRPrmBuff l_buff(buffer, size);
	error = obj->GetProp(prop, l_buff);

	*out_size = l_buff.m_cur_size;

	PR_TRACE_A2( _this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetProp )
// Interface "OS". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call OS_PROP_SetProp( hi_OS _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter *SET* method for *LAST CALL* property " );

	if( !obj )
		return errOBJECT_INVALID;

	if( !obj->f_dirty )
		error = obj->SetAccessMode(fACCESS_RW);

	if( PR_SUCC(error) )
	{
		PRPrmBuff l_buff(buffer, size);
		error = obj->SetProp(prop, l_buff);
		if( PR_SUCC(error) )
			error != warnPROPERTY_NOT_COMMITED ? obj->f_dirty = 1 : error = errOK;

		*out_size = l_buff.m_cur_size;
	}

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
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD p_type )
{
	tERROR error = errINTERFACE_INCOMPATIBLE;
	PR_TRACE_A2( 0, "Enter OS::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

	// Place your code here
	if( PROEObject::Recognize(_that, p_type) != TYPE_UNDEF )
		error = errOK;

	PR_TRACE_A2( 0, "Leave OS::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
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
	MSOE_Data *d = _this->data;
	PR_TRACE_A0( _this, "Enter OS::ObjectInit method" );

	// Place your code here
	hOBJECT io = CALL_SYS_ParentGet((hOBJECT)_this, IID_IO);
	d->ostype = PROEObject::Recognize(io, 0);
	if( d->ostype != TYPE_UNDEF )
	{
		d->hIOBase = (hIO)io;
		CALL_SYS_PropertyInitialized(_this, pgOBJECT_OS_TYPE, cTRUE);
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
	tERROR error = errOK;
	MSOE_Data *d = _this->data;
	PR_TRACE_A0( _this, "Enter OS::ObjectInitDone method" );

	// Place your code here
	d->osclass = PROEObject::Construct(d->ostype, (hOBJECT)d->hIOBase);
	if( !d->osclass )
		error = errOBJECT_NOT_CREATED;

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
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter OS::ObjectClose method" );

	// Place your code here
	if( obj )
	{
		error = obj->SaveChanges();
		obj->Release();
	}

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
	tERROR  error = errOK;
	hObjPtr ret_val = NULL;
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter OS::PtrCreate method" );

	// Place your code here
	PROEFindObject l_find(FIND_OBJECT|FIND_FOLDER);
	if( !obj->GetObject(name, l_find) )
		error = errOBJECT_NOT_FOUND;
	else
	{
		error = CALL_SYS_ObjectCreate(_this, &ret_val, IID_OBJPTR, PID_MSOE, 0);
		if( PR_SUCC(error) )
			error = ObjPtr_SetObject(ret_val, l_find.folder, l_find.object);
		if( PR_SUCC(error) )
			error = CALL_SYS_ObjectCreateDone(ret_val);
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
	tERROR error = errOK;
	hIO    ret_val = NULL;
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter OS::IOCreate method" );

	// Place your code here
	PROEFindObject l_find(FIND_OBJANY|FIND_FOLDER);
	obj->GetObject((hOBJECT)name, l_find);

	error = IO_Create((hOS)_this, l_find, access_mode, open_mode, &ret_val);

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
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter OS::Copy method" );

	// Place your code here
	PROEFindObject l_src_find(FIND_OBJECT);
	PROEFindObject l_dst_find(FIND_FOLDER|FIND_OBJNAME);

	if( obj->GetObject(src_name, l_src_find) && obj->GetObject(dst_name, l_dst_find) )
		error = l_src_find.object->Copy(l_dst_find, overwrite);
	else
		error = errOBJECT_NOT_FOUND;

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
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter OS::Rename method" );

	// Place your code here
	PROEFindObject l_src_find(FIND_OBJECT);
	PROEFindObject l_dst_find(FIND_FOLDER|FIND_OBJNAME);

	if( obj->GetObject(old_name, l_src_find) && obj->GetObject(new_name, l_dst_find) )
		error = l_src_find.object->Rename(l_dst_find, overwrite);
	else
		error = errOBJECT_NOT_FOUND;

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
	PROEObject *obj = _this->data->osclass;
	PR_TRACE_A0( _this, "Enter OS::Delete method" );

	// Place your code here
	PROEFindObject l_find(FIND_OBJECT);
	if( !obj->GetObject((hOBJECT)name, l_find) )
		error = errOBJECT_INCOMPATIBLE;
	else
	{
		tDWORD l_iid = CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID);
		if( l_iid == IID_IO )
			error = CALL_SYS_PropertySetBool(name, pgOBJECT_DELETE_ON_CLOSE, cTRUE);
		else
			error = l_find.object->CheckDelete();
	}

	PR_TRACE_A1( _this, "Leave OS::Delete method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



PROEObject * OS_GetObject(hOS p_hobj)
{
	PROEObject *l_obj = ((hi_OS)p_hobj)->data->osclass;
	if( l_obj )
		l_obj->AddRef();
	return l_obj;
}
