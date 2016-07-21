// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Wednesday,  09 April 2003,  11:21 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_io.c
// Additional info
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



#include "msg_mapi.h"
#include "..\..\Wrappers\superio\plugin_superio.h"
#include "..\..\Wrappers\superio\superio.h"



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/plugin/p_win32_nfio.h>

#include "msg_io.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure "MsgIO_Data" is described in "O:\Prague\outlook\msg_io.h" header file
  // data structure "MsgIO_Data" is described in "O:\Praque\Outlook\msg_io.h" header file
  // data structure "" is described in "O:\Praque\Outlook\msg_io.h" header file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MsgIO_Data*        data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectPreClose( hi_IO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       IO_ObjectInit,
	(tIntFnObjectInitDone)   IO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   IO_ObjectPreClose,
	(tIntFnObjectClose)      NULL,//IO_ObjectPreClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
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
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iIOVtbl e_IO_vtbl = 
{
	IO_SeekRead,
	IO_SeekWrite,
	IO_GetSize,
	IO_SetSize,
	IO_Flush
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call IO_PROP_Get( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_set_mode( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SaveData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_LoadData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_Set( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "message interior input/output interface", 40 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, MsgIO_Data, Origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_Get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_Get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_Get, NULL )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, MsgIO_Data, OpenMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, MsgIO_Data, AccessMode, cPROP_BUFFER_READ, NULL, IO_PROP_set_mode )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, MsgIO_Data, fDeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MsgIO_Data, hOS, cPROP_BUFFER_READ )
	mLAST_CALL_PROPERTY( IO_PROP_Get, IO_PROP_Set )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, IO_PROP_SaveData, IO_PROP_LoadData )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )

//#error Caution !!!
// You have to implement *GET* or *SET* propetries for:
//   - pgOBJECT_HASH


//#error Caution !!!

// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter IO::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_MAILMSG,                            // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(MsgIO_Data),                     // memory size
		IFACE_EXPLICIT_PARENT                   // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave IO::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Get )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_Get( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	MsgObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_Get" );

	// Place your code here
	if( prop != pgOBJECT_NATIVE_PATH )
	{
		error = ContentGetObject(_this->data, &obj_info);
		if( PR_SUCC(error) )
			error = ObjInfoGetProp(&obj_info, prop, buffer, size, out_size);
	}
	else
	{
		if( _this->data->superio )
			error = CALL_SYS_PropertyGetStr(_this->data->superio, out_size, prop, buffer, size, cCP_ANSI);
		else
			error = errOBJECT_INCOMPATIBLE;
	}

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_Get, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, set_mode )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_set_mode( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_set_mode for property pgOBJECT_ACCESS_MODE" );

	*out_size = sizeof(tDWORD);
	error = ContentSetMode(_this->data, *(tDWORD*)buffer);

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_set_mode for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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

	error = ContentSaveData(_this->data, out_size, buffer, size);

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

	error = ContentLoadData(_this->data, buffer, size);
	if( PR_SUCC(error) )
		error = ContentSetMode(_this->data, fACCESS_READ);

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_LoadData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, Set )
// Interface "IO". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_Set( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property
	tERROR error = errOK;
	MsgObj_Info obj_info;
	PR_TRACE_A0( _this, "Enter *SET* method for *LAST CALL* property " );

	error = ContentGetObject(_this->data, &obj_info);
	if( PR_SUCC(error) )
		error = ObjInfoSetProp(&obj_info, prop, buffer, size, out_size);

	if( PR_SUCC(error) )
		_this->data->fSaveChanges = cTRUE;

	PR_TRACE_A2( _this, "Leave *SET* method for property *LAST CALL*, ret tDWORD = %u, error = 0x%08x", *out_size, error );
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
	error = ContentCreate(_this->data, CALL_SYS_PropertyGetPtr(_this->data->hOS, plMAILMSG_MESSAGE));

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
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::ObjectInitDone method" );

	// Place your code here
	
	if( _this->data->stream )
	{
		tCHAR l_name[MAX_PATH] = "", l_buff[MAX_PATH];
		tQWORD size = 0;
		hIO l_io = NULL;
		tERROR l_error;
		tPID l_pid = PID_TMPFILE;

		ContentGetSize(_this->data, IO_SIZE_TYPE_APPROXIMATE, &size);
		if( size >= 0x100000 && GetTempPath(sizeof(l_buff), l_buff) &&
				GetTempFileName(l_buff, "PR", 0 , l_name) )
			l_pid = PID_NATIVE_FIO;

		l_error = CALL_SYS_ObjectCreate(_this, &l_io, IID_IO, l_pid, SUBTYPE_ANY);
		if( PR_SUCC(l_error) )
		{
			CALL_SYS_PropertySetDWord(l_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS|fOMODE_TEMPORARY);
			CALL_SYS_PropertySetDWord(l_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
			CALL_SYS_PropertySetBool(l_io, plCONTROL_SIZE_ON_READ, cFALSE);
			CALL_SYS_PropertySetStr(l_io, 0, pgOBJECT_NAME, l_name, 0, cCP_ANSI );
			l_error = CALL_SYS_ObjectCreateDone(l_io);	
		}

		if( PR_SUCC(l_error) )
		{
			_this->data->superio = (hIO)l_io;
			l_error = ContentCopy(_this->data, cTRUE);
		}
	}

	PR_TRACE_A1( _this, "Leave IO::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call IO_ObjectPreClose( hi_IO _this ) 
{
	tERROR error = errOK;
	tDWORD msg_id = 0, msg_err;
	PR_TRACE_A0( _this, "Enter IO::ObjectPreClose method" );

	// Place your code here
	if( _this->data->fModified || _this->data->fSaveChanges )
	{
		if( PR_FAIL(msg_err = ContentCommit(_this->data)) )
			msg_id = pm_IO_CLOSE_ERROR;
	}

	ContentStreamClose(_this->data);

	if( _this->data->fDeleteOnClose )
	{
		MsgObj_Info obj_info;

		ContentGetObject(_this->data, &obj_info);

		if( PR_FAIL(msg_err = ObjInfoDelete(&obj_info)) )
			msg_id = pm_IO_DELETE_ON_CLOSE_FAILED;
		else
			msg_id = pm_IO_DELETE_ON_CLOSE_SUCCEED;
	}

	if( msg_id )
	{
		tDWORD size = sizeof(msg_err);
		CALL_SYS_SendMsg( _this, pmc_IO, msg_id, 0, &msg_err, &size );
	}

	ContentDestroy(_this->data);

	PR_TRACE_A1( _this, "Leave IO::ObjectPreClose method, ret tERROR = 0x%08x", error );
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
	if( !buffer )
		error = errPARAMETER_INVALID;
	else if( _this->data->superio )
		error = CALL_IO_SeekRead((hi_IO)_this->data->superio, &ret_val, offset, buffer, size);
	else
		error = ContentSeekRead(_this->data, offset, buffer, size, &ret_val);

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
	if( !buffer )
		error = errPARAMETER_INVALID;
	else if( _this->data->superio )
		error = CALL_IO_SeekWrite((hi_IO)_this->data->superio, &ret_val, offset, buffer, size);
	else
		error = ContentSeekWrite(_this->data, offset, buffer, size, &ret_val);

	if( PR_SUCC(error) )
		_this->data->fModified = cTRUE;

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
	if( _this->data->superio )
		error = CALL_IO_GetSize((hi_IO)_this->data->superio, &ret_val, type);
	else
		error = ContentGetSize(_this->data, type, &ret_val);

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
	if( _this->data->superio )
		error = CALL_IO_SetSize((hi_IO)_this->data->superio, new_size);
	else
		error = ContentSetSize(_this->data, new_size);

	if( PR_SUCC(error) )
		_this->data->fModified = cTRUE;

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
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here
	error = ContentCommit(_this->data);

	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



