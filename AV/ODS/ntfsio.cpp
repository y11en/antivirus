// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  22 March 2007,  14:00 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- ntfsio.cpp
// -------------------------------------------
// AVP Prague stamp end

#if defined (_WIN32)

// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/plugin/p_win32_nfio.h>

#include <AV/plugin/p_ods.h>
// AVP Prague stamp end



extern "C"
{
#include "../../Extract/Disk/NtfsRead/ntfs.h"
}

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable NTFSIo : public cIO 
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
	tDWORD m_bDeleteOnClose; // --
	tDWORD m_nAccessMode;    // --
	tPTR   m_ntfs;           // --
// AVP Prague stamp end

	cStrObj m_sName;
	NTFS_FILE* m_file;
	NTFS_ATTR* m_attr;
	bool       m_own_ntfs;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(NTFSIo)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR NTFSIo::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IO::ObjectInitDone method" );

	// Place your code here
	cStrObj sAttrName, sPath(m_sName);
	tDWORD pos = sPath.find_last_of(L":", 3);
	if( pos )
	{
		sAttrName.assign(sPath, pos+1);
		sPath.erase(pos);
	}

	PR_TRACE((this, prtALWAYS_REPORTED_MSG, "ODS\tTry open file %S(%S)", sPath.data(), sAttrName.data()));

	if( !m_ntfs )
	{
		tWCHAR drive[2] = {sPath.at(0), 0};
		m_ntfs = (HNTFS)NtfsOpen(drive);
		m_own_ntfs = true;
	}

	if( m_ntfs )
		m_file = NtfsOpenFile((HNTFS)m_ntfs, NtfsFindFileW((HNTFS)m_ntfs, sPath.data()));

	if( m_file )
		m_attr = NtfsFindAttr(m_file, AttribData, sAttrName.empty() ? NULL : sAttrName.data());

	if( !m_attr )
	{
		PR_TRACE((this, prtALWAYS_REPORTED_MSG, "ODS\tCannot open file %S(%S)", sPath.data(), sAttrName.data()));
		error = errOBJECT_NOT_FOUND;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR NTFSIo::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IO::ObjectPreClose method" );

	// Place your code here
	if( m_file )
		NtfsCloseFile(m_file);

	if( m_own_ntfs && m_ntfs )
		NtfsClose((HNTFS)m_ntfs);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReadProp )
// Interface "IO". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR NTFSIo::ReadProp( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method for *LAST CALL* property " );

	*out_size = sizeof(tDWORD);
	switch(prop)
	{
	case pgOBJECT_FULL_NAME:
	case plNATIVE_PATH:
		{
			tDWORD len = m_sName.memsize(cCP_UNICODE);
			tDWORD offs = prop == plNATIVE_PATH ? 4*sizeof(tWCHAR) : 0;

			if( buffer )
			{
				if( size < len+offs )
					error = errBUFFER_TOO_SMALL;
				else
				{
					memcpy(buffer+offs, (tCHAR*)m_sName.data(), len);
					if( offs )
						memcpy(buffer, L"\\??\\", offs);
				}
			}
			*out_size = len+offs;
		}
		break;

	case plNATIVE_HANDLE:
		*out_size = sizeof(tPTR);
		break;

	case pgOBJECT_AVAILABILITY:
		if( buffer )
			*(tDWORD*)buffer = fAVAIL_READ|fAVAIL_CHANGE_MODE|fAVAIL_DELETE_ON_CLOSE;
		break;

	case pgOBJECT_ACCESS_MODE:
		if( buffer )
			*(tDWORD*)buffer = fACCESS_READ;
		break;

	default:
		*out_size = 0;
		return errPROPERTY_NOT_FOUND;
	}

	PR_TRACE_A2( this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, WriteProp )
// Interface "IO". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR NTFSIo::WriteProp( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method for *LAST CALL* property " );

	switch(prop)
	{
	case pgOBJECT_FULL_NAME:
		m_sName = (tWCHAR*)buffer;
		break;

	case pgOBJECT_OPEN_MODE: break;
	case pgOBJECT_ACCESS_MODE:
		if( *(tDWORD*)buffer & fACCESS_WRITE )
			return errLOCKED;
		return errOK;

	default: return errPROPERTY_NOT_FOUND;
	}

	*out_size = sizeof(tDWORD);

	PR_TRACE_A2( this, "Leave *SET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR NTFSIo::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errOK;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::SeekRead method" );

	// Place your code here
	if( !NtfsReadAttributeEx(m_attr, p_offset, p_buffer, p_size, (DWORD*)result) )
		error = errOBJECT_READ;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR NTFSIo::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::SeekWrite method" );

	// Place your code here

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
tERROR NTFSIo::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
	tERROR error = errOK;
	tQWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::GetSize method" );

	// Place your code here
	*result = m_attr->nDataLen;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR NTFSIo::SetSize( tQWORD p_new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::SetSize method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR NTFSIo::Flush()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IO::Flush method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IO". Register function
tERROR NTFSIo::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "NTFS IO", 8 )
	mpLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, NTFSIo, m_bDeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( plNTFS, NTFSIo, m_ntfs, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_UNICODE)) )
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
		PID_ODS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_MEZHUEV,                            // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(NTFSIo)-sizeof(cObjImpl),        // memory size
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

tERROR pr_call IO_Register( hROOT root ) { return NTFSIo::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgOBJECT_ORIGIN
// You have to implement propetry: pgOBJECT_NAME
// You have to implement propetry: pgOBJECT_PATH
// You have to implement propetry: pgOBJECT_FULL_NAME
// You have to implement propetry: pgOBJECT_OPEN_MODE
// You have to implement propetry: pgOBJECT_ACCESS_MODE
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


#else //_WIN32
#include <prague.h>
tERROR pr_call IO_Register( hROOT root ) { return errOK; }

#endif //_WIN32
