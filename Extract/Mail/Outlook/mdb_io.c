// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  22 November 2004,  14:53 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- outloook
// Sub project -- MDB
// Purpose     -- плагин для почтовой системы Outlook
// Author      -- Mezhuev
// File Name   -- mdb_io.c
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
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end

#include "mdb_mapi.h"

// AVP Prague stamp begin( Includes for interface,  )
#include "mdb_io.h"
// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectClose( hi_IO _this );
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        IO_ObjectInit,
	(tIntFnObjectInitDone)    IO_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       IO_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpIO_SeekRead)  ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- reads content of the object to the buffer;
typedef   tERROR (pr_call *fnpIO_SeekWrite) ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- writes buffer to the object;
typedef   tERROR (pr_call *fnpIO_GetSize)   ( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );         // -- returns size of the requested type of the object;
typedef   tERROR (pr_call *fnpIO_SetSize)   ( hi_IO _this, tQWORD new_size );           // -- resizes the object;
typedef   tERROR (pr_call *fnpIO_Flush)     ( hi_IO _this );                            // -- flush internal buffers;
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iIOVtbl 
{
	fnpIO_SeekRead   SeekRead;
	fnpIO_SeekWrite  SeekWrite;
	fnpIO_GetSize    GetSize;
	fnpIO_SetSize    SetSize;
	fnpIO_Flush      Flush;
}; // "IO" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE p_type );
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD p_new_size );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
static iIOVtbl e_IO_vtbl = 
{
	(fnpIO_SeekRead)          IO_SeekRead,
	(fnpIO_SeekWrite)         IO_SeekWrite,
	(fnpIO_GetSize)           IO_GetSize,
	(fnpIO_SetSize)           IO_SetSize,
	(fnpIO_Flush)             IO_Flush
};
// AVP Prague stamp end

// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call IO_PROP_msg_prop_get( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_msg_prop_set( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SaveData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_LoadData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)2)
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "outlook message", 16 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, MDBIO_Data, Origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_msg_prop_get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_msg_prop_get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_msg_prop_get, NULL )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, MDBIO_Data, OpenMode, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, MDBIO_Data, AccessMode, cPROP_BUFFER_READ, NULL, IO_PROP_msg_prop_set )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, MDBIO_Data, fDeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MDBIO_Data, hOS, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FILL_CHAR, MDBIO_Data, FillChar, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, IO_PROP_SaveData, IO_PROP_LoadData )
	mLOCAL_PROPERTY_BUF( plMDB_MESSAGE, MDBIO_Data, message, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_HASH
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter IO::Register method" );

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_IO,                                 // interface identifier
//! 		PID_MDB,                                // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		IO_VERSION,                             // interface version
//! 		VID_MEZHUEV,                            // interface developer
//! 		&i_IO_vtbl,                             // internal(kernel called) function table
//! 		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
//! 		&e_IO_vtbl,                             // external function table
//! 		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
//! 		IO_PropTable,                           // property table
//! 		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
//! 		sizeof(MDBIO_Data),                     // memory size
//! 		IFACE_EXPLICIT_PARENT                   // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_MDB,                                // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(MDBIO_Data),                     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave IO::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, msg_prop_get )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_msg_prop_get( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	MDBObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_msg_prop_get" );

	// Place your code here
	error = MessageGetObject(_this->data, &obj_info);
	if( PR_SUCC(error) )
		error = ObjInfoGetProp(&obj_info, prop, buffer, size, out_size);

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_msg_prop_get, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, msg_prop_set )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_msg_prop_set( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method msg_prop_set for property pgOBJECT_ACCESS_MODE" );

	if( prop == pgOBJECT_ACCESS_MODE && buffer && (*(tDWORD*)buffer & fACCESS_WRITE) )
	{
		MDB_Data * mdb = CUST_TO_MDB_Data(_this->data->hOS);
		if( mdb->hNativeIO == mdb->hRealIO )
			error = warnSET_PARENT_ACCESS_MODE_NOT_NEEDED;
	}
	else
		*out_size = 0;

	PR_TRACE_A2( _this, "Leave *SET* method msg_prop_set for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, SaveData )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_SaveData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method IO_PROP_SaveData for property pgOBJECT_REOPEN_DATA" );

	error = MessageSaveData(_this->data, out_size, buffer, size);

	PR_TRACE_A2( _this, "Leave *GET* method IO_PROP_SaveData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, LoadData )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_LoadData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_LoadData for property pgOBJECT_REOPEN_DATA" );

	error = MessageLoadData(_this->data, buffer, size);

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_LoadData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
tERROR pr_call IO_ObjectInit( hi_IO _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::ObjectInit method" );

	// Place your code here
	_this->data->hOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this,IID_OS);

	PR_TRACE_A1( _this, "Leave IO::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IO_ObjectInitDone( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	IMAPISession *l_session = NULL;
	PR_TRACE_A0( _this, "Enter IO::ObjectInitDone method" );

	// Place your code here
	error = MessageCreate(_this->data);

	PR_TRACE_A1( _this, "Leave IO::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call IO_ObjectClose( hi_IO _this )
{

	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::ObjectClose method" );

	// Place your code here
	if( _this->data->fDeleteOnClose && _this->data->hOS )
	{
		MDBObj_Info obj_info;
		tDWORD msg_id, del_err, size = sizeof(del_err);

		MessageGetObject(_this->data, &obj_info);
		if( PR_FAIL(del_err = ObjInfoDelete(&obj_info)) )
			msg_id = pm_IO_DELETE_ON_CLOSE_FAILED;
		else
			msg_id = pm_IO_DELETE_ON_CLOSE_SUCCEED;
		CALL_SYS_SendMsg( _this, pmc_IO, msg_id, 0, &del_err, &size );
	}

	error = MessageDestroy(_this->data);

	PR_TRACE_A1( _this, "Leave IO::ObjectClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::SeekRead method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave IO::SeekRead method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave IO::SeekWrite method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::GetSize method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave IO::GetSize method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IO::SetSize method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave IO::SetSize method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call IO_Flush( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

