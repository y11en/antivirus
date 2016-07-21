// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  01 February 2005,  15:06 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Ovcharik
// File Name   -- io.cpp
// -------------------------------------------
// AVP Prague stamp end

#include "StdAfx.h"

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include "p_comstream.h"

#include <string>
#include <atlbase.h>
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct ComStreamIO : public cIO 
{
private:
	CComPtr<IStream> m_spStream;
	std::string m_strFullName;
	std::string m_strObjName;
	std::string m_strObjPath;
	BOOL m_bDeleteOnClose;

// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call ReadProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call WriteProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Data declaration
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(ComStreamIO)
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR ComStreamIO::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter IO::ObjectInit method" );

	m_bDeleteOnClose = FALSE;

	PR_TRACE_A1( this, "Leave IO::ObjectInit method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR ComStreamIO::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter IO::ObjectInitDone method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave IO::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR ComStreamIO::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter IO::ObjectPreClose method" );

	// Place your code here
	if (m_bDeleteOnClose)
	{
		tDWORD del_err = errOK;
		tDWORD err_size = sizeof(del_err);
		CALL_SYS_SendMsg(this, pmc_IO, pm_IO_DELETE_ON_CLOSE_SUCCEED, 0, &del_err, &err_size);
	}

	PR_TRACE_A1( this, "Leave IO::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, ReadProperties )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR ComStreamIO::ReadProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectPreClose for property pgOBJECT_FULL_NAME" );

	if (out_size)
		*out_size = 0;

	switch(prop)
	{
	case pgOBJECT_FULL_NAME:
		{
			std::string::size_type nReqSize = m_strFullName.size() + 1;
			if(buffer)
				strncpy_s(buffer, size, m_strFullName.c_str(), _TRUNCATE);
			if(out_size)
				*out_size = (tDWORD)nReqSize;
		}
		break;

	case pgOBJECT_NAME:
		{
			std::string::size_type nReqSize = m_strObjName.size() + 1;
			if(buffer)
				strncpy_s(buffer, size, m_strObjName.c_str(), _TRUNCATE);
			if(out_size)
				*out_size = (tDWORD)nReqSize;
		}
		break;
		
	case pgOBJECT_PATH:
		{
			std::string::size_type nReqSize = m_strObjPath.size() + 1;
			if(buffer)
				strncpy_s(buffer, size, m_strObjPath.c_str(), _TRUNCATE);
			if(out_size)
				*out_size = (tDWORD)nReqSize;
		}
		break;

	case plStream:
		{
			IStream** pOutBuffer = reinterpret_cast<IStream**>(buffer);

			if(pOutBuffer)
				*pOutBuffer = static_cast<IStream*>(m_spStream);

			if (out_size)
				*out_size = sizeof(IStream*);
		}
		break;

	case pgOBJECT_ACCESS_MODE:
		{
			if(out_size)
				*out_size = sizeof(int);
			int nAccessMode = fACCESS_RW;
			if(buffer)
				*(int *)buffer = nAccessMode;
		}
		break;

	case pgOBJECT_DELETE_ON_CLOSE:
		if(out_size)
			*out_size = sizeof(BOOL);
		if(buffer)
			*(BOOL *)buffer = m_bDeleteOnClose;
		break;
	}

	PR_TRACE_A2( this, "Leave *GET* method ObjectPreClose for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, WriteProperties )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_FULL_NAME
tERROR ComStreamIO::WriteProperties( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ObjectPreClose for property pgOBJECT_FULL_NAME" );

	if (out_size)
		*out_size = 0;

	switch(prop)
	{
	case pgOBJECT_FULL_NAME:
		if(buffer)
		{
			m_strFullName.assign(buffer, buffer + size);
			m_strFullName.resize(strlen(m_strFullName.c_str()));
		}
		break;

	case pgOBJECT_NAME:
		if(buffer)
		{
			m_strObjName.assign(buffer, buffer + size);
			m_strObjName.resize(strlen(m_strObjName.c_str()));
		}
		break;

	case pgOBJECT_PATH:
		if(buffer)
		{
			m_strObjPath.assign(buffer, buffer + size);
			m_strObjPath.resize(strlen(m_strObjPath.c_str()));
		}
		break;

	case plStream:
		{
			IStream** pStreamBuffer = reinterpret_cast<IStream**>(buffer);
			if (pStreamBuffer)
				m_spStream = *pStreamBuffer;
		}
		break;

	case pgOBJECT_ACCESS_MODE:
		// whatever
		break;

	case pgOBJECT_DELETE_ON_CLOSE:
		if (buffer)
			m_bDeleteOnClose = *buffer;
		break;
	}

	PR_TRACE_A2( this, "Leave *SET* method ObjectPreClose for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR ComStreamIO::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter IO::SeekRead method" );

	if (!m_spStream)
		error = errOBJECT_NOT_INITIALIZED;
	else
	{
		LARGE_INTEGER liOffset, liNewOffset;
		liOffset.QuadPart = p_offset;
		if (FAILED(m_spStream->Seek(liOffset, STREAM_SEEK_SET, (ULARGE_INTEGER*) &liNewOffset)))
			return errOBJECT_SEEK;

		if (p_offset != liNewOffset.QuadPart)
			return errOUT_OF_OBJECT;

		// now read from the stream
		ULONG nBytesRead = 0;
		if (FAILED(m_spStream->Read(p_buffer, p_size, &nBytesRead)))
			return errOBJECT_READ;

		if (nBytesRead < p_size)
			error = errEOF;

		if ( result )
			*result = nBytesRead;
	}

	PR_TRACE_A2( this, "Leave IO::SeekRead method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR ComStreamIO::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errUNEXPECTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter IO::SeekWrite method" );

	if (!m_spStream)
		error = errOBJECT_NOT_INITIALIZED;
	else
	{
		LARGE_INTEGER liOffset;
		liOffset.QuadPart = p_offset;
		if (SUCCEEDED(m_spStream->Seek(liOffset, STREAM_SEEK_SET, NULL)))
		{
			// now write to the stream
			ULONG nBytesWritten = 0;
			if (SUCCEEDED(m_spStream->Write(p_buffer, p_size, &nBytesWritten)))
			{
				ret_val = nBytesWritten;
				error = errOK;
			}
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave IO::SeekWrite method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//    object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
//   "type" : Size type is one of the following:
//              -- explicit
//              -- approximate
//              -- estimate
//
tERROR ComStreamIO::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter IO::GetSize method" );

	if (!m_spStream)
		error = errOBJECT_NOT_INITIALIZED;
	else
	{
		STATSTG stat;
		if (FAILED(m_spStream->Stat(&stat, STATFLAG_NONAME)))
			error = errUNEXPECTED;
		else
		{
			ret_val = stat.cbSize.QuadPart;
			error = errOK;
		}
	}

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave IO::GetSize method, ret tQWORD = %I64u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR ComStreamIO::SetSize( tQWORD p_new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( this, "Enter IO::SetSize method" );

	if (!m_spStream)
		error = errOBJECT_NOT_INITIALIZED;
	else
	{
		ULARGE_INTEGER li;
		li.QuadPart = p_new_size;
		if (SUCCEEDED(m_spStream->SetSize(li)))
			error = errOK;
		else
			error = errUNEXPECTED;
	}

	PR_TRACE_A1( this, "Leave IO::SetSize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR ComStreamIO::Flush()
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter IO::Flush method" );

	// Place your code here

	PR_TRACE_A1( this, "Leave IO::Flush method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IO". Register function
tERROR ComStreamIO::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IO_PropTable)
	mpLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, FN_CUST(ReadProperties), FN_CUST(WriteProperties) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_NAME, FN_CUST(ReadProperties), FN_CUST(WriteProperties) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_PATH, FN_CUST(ReadProperties), FN_CUST(WriteProperties) )
	mpLOCAL_PROPERTY_FN( plStream, FN_CUST(ReadProperties), FN_CUST(WriteProperties) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, FN_CUST(ReadProperties), FN_CUST(WriteProperties) )
	mLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, FN_CUST(ReadProperties), FN_CUST(WriteProperties) )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE | fAVAIL_DELETE_ON_CLOSE)) )
mpPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IO)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(IO)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(IO)
	mEXTERNAL_METHOD(IO, SeekRead)
	mEXTERNAL_METHOD(IO, SeekWrite)
	mEXTERNAL_METHOD(IO, GetSize)
	mEXTERNAL_METHOD(IO, SetSize)
	mEXTERNAL_METHOD(IO, Flush)
mEXTERNAL_TABLE_END(IO)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_A0( root, "Enter IO::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_COMSTREAM,                          // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_KASPERSKY_LAB /*VID_OVCHARIK*/,
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(ComStreamIO)-sizeof(cObjImpl),   // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave IO::Register method, ret %terr", error );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call IO_Register( hROOT root ) { return ComStreamIO::Register(root); }
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// You have to implement propetry: pgOBJECT_ORIGIN
// You have to implement propetry: pgOBJECT_NAME
// You have to implement propetry: pgOBJECT_PATH
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



