// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Friday,  04 July 2003,  11:33 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- msoe
// Sub project -- msoe
// Purpose     -- плагин для работы с Outlook Express
// Author      -- Mezhuev
// File Name   -- msoe_io.c
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
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end

#include "msoe_base.h"

// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end

// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_OEIO_Data 
{
	tDWORD       OpenMode;       // --
	tDWORD       AccessMode;     // --
	tBOOL        fDeleteOnClose; // --
	hOS          hOS;            // --
	tDWORD       Origin;         // --
	PROEObject * ioclass;        // --
	tBOOL        fModified;      // --
} OEIO_Data;

// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OEIO_Data*         data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectClose( hi_IO _this );
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       IO_ObjectInit,
	(tIntFnObjectInitDone)   IO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      IO_ObjectClose,
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
tERROR pr_call IO_PROP_GetProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SetAccess( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SaveData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_LoadData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SetProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
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
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "OE input/output object", 23 )
	mLOCAL_PROPERTY( pgOBJECT_ORIGIN, OEIO_Data, Origin, cPROP_BUFFER_WRITE, IO_PROP_GetProp, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_GetProp, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_GetProp, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_GetProp, NULL )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, OEIO_Data, OpenMode, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, OEIO_Data, AccessMode, cPROP_BUFFER_READ, NULL, IO_PROP_SetAccess )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, OEIO_Data, fDeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, IO_PROP_GetProp, NULL )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, OEIO_Data, hOS, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, IO_PROP_SaveData, IO_PROP_LoadData )
	mLAST_CALL_PROPERTY( IO_PROP_GetProp, IO_PROP_SetProp )
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
		PID_MSOE,                               // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(OEIO_Data),                      // memory size
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

// AVP Prague stamp begin( Property method implementation, GetProp )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_GetProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PROEObject *obj = _this->data->ioclass;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_GetProp" );

	if( !obj )
		return errUNEXPECTED;

	PRPrmBuff l_buff(buffer, size);
	switch ( prop )
	{
		case pgOBJECT_NAME: error = obj->GetNameZ(l_buff); break;
		case pgOBJECT_PATH: error = obj->GetFullNameZ(l_buff, cTRUE); break;
		case pgOBJECT_FULL_NAME: error = obj->GetFullNameZ(l_buff, cFALSE); break;
		case pgOBJECT_ORIGIN: l_buff << (_this->data->Origin ? _this->data->Origin : (tDWORD)obj->GetOrigin()); error = l_buff.Error(); break;
		case pgOBJECT_AVAILABILITY: l_buff << obj->GetAvailability(); error = l_buff.Error(); break;
		default: error = obj->GetProp(prop, l_buff);
	}
	*out_size = l_buff.m_cur_size;

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_GetProp, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, SetAccess )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_SetAccess( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	OEIO_Data *d = _this->data;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_SetAccess for property pgOBJECT_ACCESS_MODE" );

	tDWORD l_new_access = *(tDWORD*)buffer;
	if( d->ioclass )
	{
		if( l_new_access != d->AccessMode )
			error = d->ioclass->SetAccessMode(l_new_access);
	}
	else
		error = errOBJECT_NOT_INITIALIZED;

	if( PR_SUCC(error) )
	{
		*out_size = sizeof(tDWORD);
		d->AccessMode = l_new_access;
	}

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_SetAccess for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, SaveData )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_SaveData( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PROEObject *obj = _this->data->ioclass;
	PR_TRACE_A0( _this, "Enter *GET* method IO_PROP_SaveData for property pgOBJECT_REOPEN_DATA" );

	PRPrmBuff l_buff(buffer, size);
	error = obj->SaveObject(l_buff);
	*out_size = l_buff.m_cur_size;

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

	PROEObject *os_obj = OS_GetObject(_this->data->hOS);

	PRPrmBuff l_buff(buffer, size);
	_this->data->ioclass = os_obj->LoadObject(l_buff);
	*out_size = l_buff.m_cur_size;

	os_obj->Release();

	if( !_this->data->ioclass )
		error = errOBJECT_NOT_FOUND;
	else
		_this->data->AccessMode = fACCESS_READ;

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_LoadData for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, SetProp )
// Interface "IO". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_SetProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PROEObject *obj = _this->data->ioclass;
	PR_TRACE_A0( _this, "Enter *SET* method for *LAST CALL* property " );

	PRPrmBuff l_buff(buffer, size);
	error = obj->SetProp(prop, l_buff);
	if( PR_SUCC(error) )
		error != warnPROPERTY_NOT_COMMITED ? obj->f_dirty = 1 : error = errOK;

	*out_size = l_buff.m_cur_size;

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
	_this->data->hOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this, IID_OS);

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
	if( !_this->data->ioclass )
		error = errOBJECT_NOT_CREATED;

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
	OEIO_Data *d = _this->data;
	PROEObject *obj = d->ioclass;
	PR_TRACE_A0( _this, "Enter IO::ObjectClose method" );

	// Place your code here
	if( obj )
	{
		tDWORD msg_id = 0, error;
		if( d->fDeleteOnClose )
		{
			if( PR_FAIL(error = obj->CheckDelete()) )
				msg_id = pm_IO_DELETE_ON_CLOSE_FAILED;
			else
				msg_id = pm_IO_DELETE_ON_CLOSE_SUCCEED;
		}
		else if( PR_FAIL(error = obj->SaveChanges()) )
			msg_id = pm_IO_CLOSE_ERROR;

		if( msg_id )
		{
			tDWORD size = sizeof(error);
			CALL_SYS_SendMsg(_this, pmc_IO, msg_id, 0, &error, &size);
		}

		obj->Release();
	}

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
	PROEObject *obj = _this->data->ioclass;
	PR_TRACE_A0( _this, "Enter IO::SeekRead method" );

	// Place your code here
	if( !buffer )
		error = errPARAMETER_INVALID;
	else
		error = obj->SeekRead(&ret_val, offset, buffer, size);
	if( PR_SUCC(error) && ret_val < size )
		error = errEOF;

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
	OEIO_Data *d = _this->data;
	PROEObject *obj = d->ioclass;
	PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );

	// Place your code here
	if( !buffer )
		error = errPARAMETER_INVALID;
	else if( !(d->AccessMode & fACCESS_WRITE) )
		error = errOBJECT_READ_ONLY;
	else if( !PR_FAIL(error = obj->SeekWrite(&ret_val, offset, buffer, size)) )
		obj->f_dirty = 1;

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
	PROEObject *obj = _this->data->ioclass;
	PR_TRACE_A0( _this, "Enter IO::GetSize method" );

	// Place your code here
	error = obj->GetSize(&ret_val, type);

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
	OEIO_Data *d = _this->data;
	PROEObject *obj = d->ioclass;
	PR_TRACE_A0( _this, "Enter IO::SetSize method" );

	// Place your code here
	if( !(d->AccessMode & fACCESS_WRITE) )
		error = errOBJECT_READ_ONLY;
	else if( !PR_FAIL(error = obj->SetSize(new_size)) )
		obj->f_dirty = 1;

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
	PROEObject *obj = _this->data->ioclass;
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here
	error = obj->Flush();

	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

tERROR IO_Create(hOS p_os, PROEFindObject &p_find, tDWORD p_access_mode, tDWORD p_open_mode, hIO* p_result)
{
	if( !p_find.object && p_find.folder && (p_open_mode & fOMODE_CREATE_IF_NOT_EXIST) )
		p_find.object = p_find.folder->CreateChild(p_open_mode, p_find.objname);

	if( !p_find.object )
		return errOBJECT_NOT_FOUND;

	if( p_find.object->f_initfail )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	tERROR error = CALL_SYS_ObjectCreate(p_os, p_result, IID_IO, PID_MSOE, 0);
	if( error != errOK )
		return error;

	hi_IO l_io = (hi_IO)(*p_result);
	l_io->data->ioclass = p_find.object;
	p_find.object->AddRef();

	CALL_SYS_PropertySetDWord(l_io, pgOBJECT_OPEN_MODE, p_open_mode);
	error = CALL_SYS_PropertySetDWord(l_io, pgOBJECT_ACCESS_MODE, p_access_mode);

	if( PR_SUCC(error) )
		error = CALL_SYS_ObjectCreateDone(l_io);

	return error;
}

PROEObject * IO_GetObject(hIO p_obj)
{
	PROEObject *l_obj = ((hi_IO)p_obj)->data->ioclass;
	if( l_obj )
		l_obj->AddRef();
	return l_obj;
}

