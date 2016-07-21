// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  11 December 2002,  14:07 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- SuperIO
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- superio.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "superio.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- interface for IO buffering and conversion
//    Supplies IO interface for any data presentation which provides data converters.
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_tSUPERIO_DATA 
{
	hTRANSFORMER input_converter;  // --
	hTRANSFORMER output_converter; // --
	hIO        tmp_io;           // --
	hSEQ_IO      tmp_seqio;        // --
	hOBJECT    based_on;         // --
	tQWORD       current_pos;      // --
	tDWORD       flags;            // --
	tDWORD       read_chuck_size;  // --
	tDWORD       access_mode;      // --
} tSUPERIO_DATA;

// AVP Prague stamp end



#define	fSUPERIO_EOF			1
#define	fSUPERIO_MODIFY			2
#define	fSUPERIO_READ_CHUNK		8

#define	READ_ALL				-1

#define	DEF_READ_CHUCK_SIZE		1024

tERROR	ReadFromConverter(tSUPERIO_DATA *d, tDWORD size);
tERROR	WriteToConverter(tSUPERIO_DATA *d);

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	tSUPERIO_DATA*     data; // pointer to the "IO" data structure
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
	(tIntFnObjectClose)      NULL,
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
tERROR pr_call IO_PROP_ReadProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_WriteProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SetMode( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SetConverter( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
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
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "SuperIO interface", 18 )
	mLOCAL_PROPERTY( plINPUT_CONVERTER, tSUPERIO_DATA, input_converter, cPROP_BUFFER_READ | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED, NULL, IO_PROP_SetConverter )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, tSUPERIO_DATA, based_on, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLAST_CALL_PROPERTY( IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_BUF( plREAD_CHUCK_SIZE, tSUPERIO_DATA, read_chuck_size, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, tSUPERIO_DATA, access_mode, cPROP_BUFFER_READ, NULL, IO_PROP_SetMode )
	mLOCAL_PROPERTY_FN( pgOBJECT_FILL_CHAR, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_OPEN_MODE, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY_FN( pgOBJECT_ORIGIN, IO_PROP_ReadProp, IO_PROP_WriteProp )
	mLOCAL_PROPERTY( plOUTPUT_CONVERTER, tSUPERIO_DATA, output_converter, cPROP_BUFFER_READ, NULL, IO_PROP_SetConverter )
mPROPERTY_TABLE_END(IO_PropTable)
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
		PID_SUPERIO,                            // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(tSUPERIO_DATA),                  // memory size
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



// AVP Prague stamp begin( Property method implementation, SetMode )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_SetMode( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	tDWORD access_mode = *(tDWORD*)buffer;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_SetMode for property pgOBJECT_ACCESS_MODE" );

	if( d->based_on )
		error = CALL_SYS_PropertySetDWord(d->based_on, prop, access_mode);
	if(PR_SUCC(error))
		d->access_mode = access_mode;

	if( out_size )
		*out_size = sizeof(tDWORD);

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_SetMode for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetConverter )
// Interface "IO". Method "Set" for property(s):
//  -- INPUT_CONVERTER
tERROR pr_call IO_PROP_SetConverter( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_SetConverter for property plINPUT_CONVERTER" );

	if( prop == plINPUT_CONVERTER )
	{
		d->input_converter = *(hTRANSFORMER*)buffer;
		error = CALL_SYS_PropertySetObj(d->input_converter, pgOUTPUT_SEQ_IO, (hOBJECT)d->tmp_seqio);
	}
	else
	{
		d->output_converter = *(hTRANSFORMER*)buffer;
		error = CALL_SYS_PropertySetObj(d->output_converter, pgINPUT_SEQ_IO, (hOBJECT)d->tmp_seqio);
	}

	*out_size = sizeof(hTRANSFORMER);

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_SetConverter for property plINPUT_CONVERTER, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReadProp )
// Interface "IO". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_ReadProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method for *LAST CALL* property " );

	if( d->based_on )
	{
		if( PR_PROP_TYPE(prop) == pTYPE_STRING )
			error = CALL_SYS_PropertyGetStr(d->based_on, out_size, prop, buffer, size, cCP_ANSI);
		else
			error = CALL_SYS_PropertyGet(d->based_on, out_size, prop, buffer, size);
	}

	PR_TRACE_A2( _this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, WriteProp )
// Interface "IO". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_WriteProp( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method for *LAST CALL* property " );

	if( d->based_on )
		error = CALL_SYS_PropertySet(d->based_on, out_size, prop, buffer, size);

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
	tERROR error = errNOT_IMPLEMENTED;
	tSUPERIO_DATA *d = _this->data;
	PR_TRACE_A0( _this, "Enter IO::ObjectInit method" );

	// Place your code here
	d->based_on = CALL_SYS_ParentGet((hOBJECT)_this, IID_ANY);

	error = CALL_SYS_ObjectCreate(_this, &d->tmp_io, IID_IO, PID_TMPFILE, 0);
	if(PR_SUCC(error))
	{
		CALL_SYS_PropertySetDWord(d->tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetDWord(d->tmp_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
		error = CALL_SYS_ObjectCreateDone(d->tmp_io);
		if(PR_SUCC(error))
			error = CALL_SYS_ObjectCreateQuick((hOBJECT)d->tmp_io, &d->tmp_seqio, IID_SEQIO, PID_ANY, 0);
	}

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
	tSUPERIO_DATA *d = _this->data;
	PR_TRACE_A0( _this, "Enter IO::ObjectInitDone method" );

	// Place your code here
	d->read_chuck_size = DEF_READ_CHUCK_SIZE;
	if( d->input_converter && error == errOK )
		error = CALL_SYS_ObjectCreateDone(d->input_converter);
	if( d->output_converter && error == errOK )
		error = CALL_SYS_ObjectCreateDone(d->output_converter);

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
    tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::ObjectPreClose method" );

	// Place your code here
    (void)d;
	error = IO_Flush(_this);

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
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::SeekRead method" );

	// Place your code here
	if( !(d->flags & fSUPERIO_EOF) && d->current_pos < offset + size )
		error = ReadFromConverter(d, (tDWORD)(offset - d->current_pos + size));

	if( PR_SUCC(error) )
		error = CALL_IO_SeekRead((hi_IO)d->tmp_io, &ret_val, offset, buffer, size);

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
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );

	// Place your code here
	if( !(d->access_mode & fACCESS_WRITE) )
		return errOBJECT_READ_ONLY;

	if( !(d->flags & fSUPERIO_EOF) )
		error = ReadFromConverter(d, READ_ALL);

	if(PR_SUCC(error))
		error = CALL_IO_SeekWrite((hi_IO)d->tmp_io, &ret_val, offset, buffer, size);

	if(PR_SUCC(error))
		d->flags |= fSUPERIO_MODIFY;

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
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::GetSize method" );

	// Place your code here
	if( (d->flags & fSUPERIO_EOF) || type == IO_SIZE_TYPE_EXPLICIT )
	{
		if( !(d->flags & fSUPERIO_EOF) )
			error = ReadFromConverter(d, READ_ALL);
		if(PR_SUCC(error))
			error = CALL_IO_GetSize((hi_IO)d->tmp_io, &ret_val, IO_SIZE_TYPE_EXPLICIT);
	}
	else
		ret_val = CALL_SYS_PropertyGetQWord(d->input_converter, pgTRANSFORM_SIZE);

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
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::SetSize method" );

	// Place your code here
	if( !(d->flags & fSUPERIO_EOF) )
		error = ReadFromConverter(d, READ_ALL);

	if(PR_SUCC(error))
		error = CALL_IO_SetSize((hi_IO)d->tmp_io, new_size);

	if(PR_SUCC(error))
		d->flags |= fSUPERIO_MODIFY;

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
	tSUPERIO_DATA *d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here
	if( d->output_converter && (d->flags & fSUPERIO_MODIFY) )
	{
		error = WriteToConverter(d);
		d->flags &= ~fSUPERIO_MODIFY;
	}

	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


void SetEOF(tSUPERIO_DATA *d)
{
	CALL_SYS_ObjectClose(d->input_converter);
	d->input_converter = NULL;
	d->flags |= fSUPERIO_EOF;
}

tERROR ReadFromConverter(tSUPERIO_DATA *d, tDWORD size)
{
	tERROR error = errOK;
	tQWORD read_size;

	if( size != READ_ALL || (d->flags & fSUPERIO_READ_CHUNK) ) // chunk reading
	{
		tDWORD read_size;
		if( size < d->read_chuck_size )
			size = d->read_chuck_size;
		error = CALL_Transformer_ProcessChunck(d->input_converter, &read_size, size);
		if( error != errNOT_IMPLEMENTED )
		{
			d->flags |= fSUPERIO_READ_CHUNK;
			if( PR_SUCC(error) )
				if( PR_SUCC(CALL_IO_Flush(d->tmp_seqio)) )
				{
					d->current_pos += read_size;
					if( read_size < size || error == errEOF )
						SetEOF(d);
				}
			return error;
		}
	}

	// try read all data
	error = CALL_Transformer_Proccess(d->input_converter, &read_size);
	if(PR_SUCC(error))
		error = CALL_IO_Flush(d->tmp_seqio);
	if(PR_SUCC(error))
		SetEOF(d);

	return error;
}

tERROR WriteToConverter(tSUPERIO_DATA *d)
{
	tERROR error = errOK;
	tQWORD write_size;

	error = CALL_IO_Flush(d->tmp_seqio);
	if(PR_SUCC(error))
		error = CALL_Transformer_Proccess(d->output_converter, &write_size);
	return error;
}
