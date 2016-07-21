// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  15 December 2006,  14:31 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- io.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <AV/plugin/p_avs.h>

#include <klava/klavsys.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable KLAVIo : public cIO 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call ReadProp( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call WriteProp( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Data declaration
	KLAV_IO_Object* m_io; // --
	cObj* m_subst_io; // --
// AVP Prague stamp end

	cStrObj m_sName;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(KLAVIo)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR KLAVIo::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IO::ObjectInitDone method" );

	// Place your code here
	if( !m_io )
		error = errOBJECT_NOT_INITIALIZED;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR KLAVIo::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IO::ObjectPreClose method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, ReadProp )
// Interface "IO". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR KLAVIo::ReadProp( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method for *LAST CALL* property " );

	if( m_subst_io )
		return m_subst_io->propGet(out_size, prop, buffer, size);

	switch(prop)
	{
	case pgOBJECT_FULL_NAME:
		{
			tDWORD len = m_sName.memsize(cCP_ANSI);
			if( buffer )
			{
				if( size < len )
					error = errBUFFER_TOO_SMALL;
				else
					memcpy(buffer, (tCHAR*)m_sName.c_str(cCP_ANSI), len);
			}
			*out_size = len;
		}
		break;

	case pgOBJECT_ACCESS_MODE:
		if( buffer )
			*(tDWORD*)buffer = fACCESS_RW;
		*out_size = sizeof(tDWORD);
		break;

	default:
		return errPROPERTY_NOT_FOUND;
	}

	PR_TRACE_A2( this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Property method implementation, WriteProp )
// Interface "IO". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR KLAVIo::WriteProp( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method for *LAST CALL* property " );

	if( m_subst_io )
		return m_subst_io->propSet(out_size, prop, buffer, size);

	switch(prop)
	{
	case pgOBJECT_FULL_NAME:   m_sName = (tCHAR*)buffer; break;
	case pgOBJECT_ACCESS_MODE: break;
	default: return errPROPERTY_NOT_FOUND;
	}

	*out_size = size;

	PR_TRACE_A2( this, "Leave *SET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
tERROR KLAVIo::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::SeekRead method" );

	// Place your code here
	error = m_io->seek_read(p_offset, p_buffer, p_size, result);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
tERROR KLAVIo::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::SeekWrite method" );

	// Place your code here
	error = m_io->seek_write(p_offset, p_buffer, p_size, result);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
tERROR KLAVIo::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::GetSize method" );

	// Place your code here
	error = m_io->get_size(result);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
tERROR KLAVIo::SetSize( tQWORD p_new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::SetSize method" );

	// Place your code here
	error = m_io->set_size(p_new_size);

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
tERROR KLAVIo::Flush()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::Flush method" );

	// Place your code here
	error = m_io->flush();

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IO". Register function
tERROR KLAVIo::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Comment", 8 )
	mpLOCAL_PROPERTY_BUF( plKLAV_IO_OBJECT, KLAVIo, m_io, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( plSUBST_IO_OBJECT, KLAVIo, m_subst_io, cPROP_BUFFER_READ_WRITE )
	mpLAST_CALL_PROPERTY( FN_CUST(ReadProp), FN_CUST(WriteProp) )
mpPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IO)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(IO)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(IO)
	mEXTERNAL_METHOD(IO, SeekRead)
	mEXTERNAL_METHOD(IO, SeekWrite)
	mEXTERNAL_METHOD(IO, GetSize)
	mEXTERNAL_METHOD(IO, SetSize)
	mEXTERNAL_METHOD(IO, Flush)
mEXTERNAL_TABLE_END(IO)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "IO::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_AVS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(KLAVIo)-sizeof(cObjImpl),        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call IO_Register( hROOT root ) { return KLAVIo::Register(root); }
// AVP Prague stamp end

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_ORIGIN
// You have to implement propetry: pgOBJECT_NAME
// You have to implement propetry: pgOBJECT_PATH
// You have to implement propetry: pgOBJECT_FULL_NAME
// You have to implement propetry: pgOBJECT_OPEN_MODE
// You have to implement propetry: pgOBJECT_ACCESS_MODE
// You have to implement propetry: pgOBJECT_DELETE_ON_CLOSE
// You have to implement propetry: pgOBJECT_AVAILABILITY
// You have to implement propetry: pgOBJECT_BASED_ON
// You have to implement propetry: pgOBJECT_FILL_CHAR
// You have to implement propetry: pgOBJECT_HASH
// You have to implement propetry: pgOBJECT_REOPEN_DATA
// You have to implement propetry: pgOBJECT_ATTRIBUTES
// You have to implement propetry: pgOBJECT_CREATION_TIME
// You have to implement propetry: pgOBJECT_LAST_WRITE_TIME
// You have to implement propetry: pgOBJECT_LAST_ACCESS_TIME
// AVP Prague stamp end

