// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  15 February 2005,  17:51 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Alexey Antropov
// File Name   -- decodeio.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define DecodeIO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/e_clsid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>
#include <Prague/plugin/p_tempfile.h>

#include <AV/plugin/p_startupenum2.h>
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable DecodeIO : public cIO /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations
	tERROR pr_call IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Data declaration
// AVP Prague stamp end



private:
	hIO		m_hIO;



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(DecodeIO)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "DecodeIO". Static(shared) property table variables
const tCODEPAGE PlugInCP = cCP_UNICODE; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
//! tERROR (null)::ObjectInit()
tERROR DecodeIO::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::ObjectInit method" );

	// Place your code here

	error = sysCreateObject((hOBJECT*)&m_hIO, IID_IO, PID_NATIVE_FIO);
	if (PR_FAIL(error))
		PR_TRACE((this, prtERROR, "decodeio\tCannot create IO object, %terr", error));
	
	return error;
}
// AVP Prague stamp end



#define COPY_BUFFER_SIZE 0x4000
tERROR CopyIOContent(hIO dest_io,hIO src_io)
{
	tBYTE buf[COPY_BUFFER_SIZE];
	tQWORD written=0;
	tDWORD size=0;
	tQWORD qwSize=0;
	tERROR error=CALL_IO_GetSize((hIO)src_io, &qwSize,IO_SIZE_TYPE_EXPLICIT);

	while(PR_SUCC(error))
	{
		error=CALL_IO_SeekRead(src_io,&size,written,buf,COPY_BUFFER_SIZE);
		if(size==0)
			break;
		if(PR_SUCC(error))
		{
			error=CALL_IO_SeekWrite(dest_io,&size,written,buf,size);
			written+=size;
		}
		if(PR_SUCC(error))
			error = CALL_SYS_SendMsg(src_io,pmc_PROCESSING,pm_PROCESSING_YIELD,0,0,0);
	}

	if(written==qwSize)
		error=errOK;
	else if(PR_SUCC(error))
		error=errNOT_OK;

	if(PR_SUCC(error)) 
		error=CALL_IO_SetSize(dest_io,written);

	if(PR_SUCC(error)) 
		error=CALL_IO_Flush(dest_io);
	return error;
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
//! tERROR (null)::ObjectInitDone()
tERROR DecodeIO::ObjectInitDone()
{
//	tERROR (*SignCheck)(hOBJECT obj,tBOOL use_default_key,tVOID* keys[],tDWORD key_count,tBOOL all_of,tDWORD* sign_lib_error_code);
//	void (*SignCheck)(void);
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::ObjectInitDone method" );

	// Place your code here

//INT3;

	error = m_hIO->propSetDWord(pgOBJECT_OPEN_MODE, m_hIO->propGetDWord(pgOBJECT_OPEN_MODE) | fOMODE_SHARE_DENY_WRITE);
	if (PR_SUCC(error))
		error = m_hIO->sysCreateObjectDone();
	if (PR_FAIL(error))
		PR_TRACE((this, prtERROR, "decodeio\tCannot done IO object, %terr", error));

	if (PR_SUCC(error))
	{
		/*error=CALL_Root_ResolveImportFunc(g_root,(tFUNC_PTR*)(&SignCheck),ECLSID_LOADER,0x25f0bac6l,IID_DECODEIO);
		if(SignCheck)
			error = SignCheck((hOBJECT)m_hIO, 1,0,0,0,0);
		else 
			error = errMODULE_NOT_VERIFIED;*/
	}
	
	if (PR_SUCC(error))
	{
		hIO hTempIO = NULL;
		tQWORD 
		error = sysCreateObjectQuick((hOBJECT*)&hTempIO, IID_IO, PID_TEMPIO);
		if (PR_SUCC(error))
			error = CopyIOContent(hTempIO, m_hIO);
		if (PR_SUCC(error))
		{
			m_hIO->sysCloseObject();
			m_hIO = hTempIO;
		}
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
//! tERROR (null)::ObjectPreClose()
tERROR DecodeIO::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::ObjectPreClose method" );

	// Place your code here

	m_hIO->sysCloseObject();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, IOPropGet )
// Interface "DecodeIO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//! tERROR (null)::IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
//! tERROR DecodeIO::IOPropGet( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size )
tERROR DecodeIO::IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method IOPropGet" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
			error = CALL_SYS_PropertyGetStr(m_hIO,out_size, prop, buffer, size, cCP_UNICODE);
			break;
		case pgOBJECT_ORIGIN:
		case pgOBJECT_OPEN_MODE:
		case pgOBJECT_ACCESS_MODE:
		case pgOBJECT_DELETE_ON_CLOSE:
		case pgOBJECT_AVAILABILITY:
		case pgOBJECT_BASED_ON:
		case pgOBJECT_FILL_CHAR:
		case pgOBJECT_HASH:
		case pgOBJECT_REOPEN_DATA:
		case pgOBJECT_ATTRIBUTES:
		case pgOBJECT_CREATION_TIME:
		case pgOBJECT_LAST_WRITE_TIME:
		case pgOBJECT_LAST_ACCESS_TIME:
		case pgINTERFACE_VERSION:
		case pgINTERFACE_COMMENT:
			error = m_hIO->propGet(out_size, prop, buffer, size);
			break;

		default:
			error = errPARAMETER_INVALID;
			break;
	}

	PR_TRACE_A2( this, "Leave *GET* multyproperty method IOPropGet, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, IOPropSet )
// Interface "DecodeIO". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//! tERROR (null)::IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
//! tERROR DecodeIO::IOPropSet( tDWORD* out_size, tPROPID prop, tWCHAR* buffer, tDWORD size )
tERROR DecodeIO::IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* multyproperty method IOPropSet" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
			error = CALL_SYS_PropertySetStr(m_hIO,out_size, prop, buffer, size, cCP_UNICODE);
			break;

		case pgOBJECT_ORIGIN:
		case pgOBJECT_OPEN_MODE:
		case pgOBJECT_ACCESS_MODE:
		case pgOBJECT_DELETE_ON_CLOSE:
		case pgOBJECT_AVAILABILITY:
		case pgOBJECT_BASED_ON:
		case pgOBJECT_FILL_CHAR:
		case pgOBJECT_HASH:
		case pgOBJECT_REOPEN_DATA:
		case pgOBJECT_ATTRIBUTES:
		case pgOBJECT_CREATION_TIME:
		case pgOBJECT_LAST_WRITE_TIME:
		case pgOBJECT_LAST_ACCESS_TIME:
			error = m_hIO->propSet(out_size, prop, buffer, size);
			break;

		default:
			error = errPARAMETER_INVALID;
			break;
	}

		PR_TRACE_A2( this, "Leave *SET* multyproperty method IOPropSet, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
//! tERROR (null)::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
tERROR DecodeIO::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tQWORD	i;
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::SeekRead method" );

	// Place your code here

//INT3;

	error=m_hIO->SeekRead(result,p_offset,p_buffer,p_size);

	if (((tCHAR*)p_buffer)[0]&0x80)
	{
		for (i=0;i<p_size;i++) ((tCHAR*)p_buffer)[i]^=0xaa;
	}
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
//! tERROR (null)::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
tERROR DecodeIO::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tQWORD	i;
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::SeekWrite method" );

	// Place your code here

	for (i=0;i<p_size;i++) ((tCHAR*)p_buffer)[i]^=0x55;

	error=m_hIO->SeekWrite(result,p_offset,p_buffer,p_size);

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
//! tERROR (null)::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
tERROR DecodeIO::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "DecodeIO::GetSize method" );

	// Place your code here

	error=m_hIO->GetSize(result,p_type);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
//! tERROR (null)::SetSize( tQWORD p_new_size )
tERROR DecodeIO::SetSize( tQWORD p_new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::SetSize method" );

	// Place your code here

	error=m_hIO->SetSize(p_new_size);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
//! tERROR (null)::Flush()
//! tERROR (null)::Flush()
tERROR DecodeIO::Flush()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "DecodeIO::Flush method" );

	// Place your code here

	error=m_hIO->Flush();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "DecodeIO". Register function
tERROR DecodeIO::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(DecodeIO_PropTable)
	mpLOCAL_PROPERTY_FN( pgINTERFACE_VERSION, FN_CUST(IOPropGet), NULL )
	mpLOCAL_PROPERTY_FN( pgINTERFACE_COMMENT, FN_CUST(IOPropGet), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ORIGIN, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_NAME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_PATH, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_OPEN_MODE, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_AVAILABILITY, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_BASED_ON, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_FILL_CHAR, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_HASH, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_REOPEN_DATA, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mSHARED_PROPERTY_PTR( pgPLUGIN_CODEPAGE, PlugInCP, sizeof(PlugInCP) )
mpPROPERTY_TABLE_END(DecodeIO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(DecodeIO)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(DecodeIO)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(DecodeIO)
	mEXTERNAL_METHOD(DecodeIO, SeekRead)
	mEXTERNAL_METHOD(DecodeIO, SeekWrite)
	mEXTERNAL_METHOD(DecodeIO, GetSize)
	mEXTERNAL_METHOD(DecodeIO, SetSize)
	mEXTERNAL_METHOD(DecodeIO, Flush)
mEXTERNAL_TABLE_END(DecodeIO)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "DecodeIO::Register method", &error );

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_DECODEIO,                           // interface identifier
//! 		PID_STARTUPENUM2,                       // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		DecodeIO_VERSION,                       // interface version
//! 		Cannot put vendor id
//! 		NULL,                                   // internal(kernel called) function table
//! 		0,                                      // internal function table size
//! 		&e_DecodeIO_vtbl,                       // external function table
//! 		(sizeof(e_DecodeIO_vtbl)/sizeof(tPTR)), // external function table size
//! 		DecodeIO_PropTable,                     // property table
//! 		mPROPERTY_TABLE_SIZE(DecodeIO_PropTable),// property table size
//! 		0,                                      // memory size
//! 		0                                       // interface flags
//! 	);
//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_DECODEIO,                           // interface identifier
//! 		PID_STARTUPENUM2,                       // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		DecodeIO_VERSION,                       // interface version
//! 		Cannot put vendor id
//! 		&i_DecodeIO_vtbl,                       // internal(kernel called) function table
//! 		(sizeof(i_DecodeIO_vtbl)/sizeof(tPTR)), // internal function table size
//! 		&e_DecodeIO_vtbl,                       // external function table
//! 		(sizeof(e_DecodeIO_vtbl)/sizeof(tPTR)), // external function table size
//! 		DecodeIO_PropTable,                     // property table
//! 		mPROPERTY_TABLE_SIZE(DecodeIO_PropTable),// property table size
//! 		0,                                      // memory size
//! 		0                                       // interface flags
//! 	);
//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_DECODEIO,                           // interface identifier
//! 		PID_STARTUPENUM2,                       // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		DecodeIO_VERSION,                       // interface version
//! 		Cannot put vendor id
//! 		&i_DecodeIO_vtbl,                       // internal(kernel called) function table
//! 		(sizeof(i_DecodeIO_vtbl)/sizeof(tPTR)), // internal function table size
//! 		&e_DecodeIO_vtbl,                       // external function table
//! 		(sizeof(e_DecodeIO_vtbl)/sizeof(tPTR)), // external function table size
//! 		DecodeIO_PropTable,                     // property table
//! 		mPROPERTY_TABLE_SIZE(DecodeIO_PropTable),// property table size
//! 		sizeof(DecodeIO)-sizeof(cObjImpl),      // memory size
//! 		0                                       // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_DECODEIO,                           // interface identifier
		PID_STARTUPENUM2,                       // plugin identifier
		0x00000000,                             // subtype identifier
		DecodeIO_VERSION,                       // interface version
		VID_ALEX,                               // interface developer
		&i_DecodeIO_vtbl,                       // internal(kernel called) function table
		(sizeof(i_DecodeIO_vtbl)/sizeof(tPTR)), // internal function table size
		&e_DecodeIO_vtbl,                       // external function table
		(sizeof(e_DecodeIO_vtbl)/sizeof(tPTR)), // external function table size
		DecodeIO_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(DecodeIO_PropTable),// property table size
		sizeof(DecodeIO)-sizeof(cObjImpl),      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"DecodeIO(IID_DECODEIO) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call DecodeIO_Register( hROOT root ) { return DecodeIO::Register(root); }
// AVP Prague stamp end



