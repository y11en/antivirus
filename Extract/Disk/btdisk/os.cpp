// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  01 June 2004,  14:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- btdisk
// Sub project -- Not defined
// Purpose     -- загрузочные cd/dvd
// Author      -- Sobko
// File Name   -- os.cpp
// -------------------------------------------
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

#include <Extract/plugin/p_btdisk.h>

#include "btdiskimp.h"
// AVP Prague stamp end


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable OsPrivate : public cOS /*cObjImpl*/ 
{
private:
// Internal function declarations
	static tERROR pr_call Recognize( hOBJECT _that, tDWORD p_type );
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call NamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call NamePropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call HashPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ReopenPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ReopenPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call PtrCreate( hObjPtr* result, hOBJECT p_name );
	tERROR pr_call IOCreate( hIO* result, hOBJECT p_name, tDWORD p_access_mode, tDWORD p_open_mode );
	tERROR pr_call Copy( hOBJECT p_src_name, hOBJECT p_dst_name, tBOOL p_overwrite );
	tERROR pr_call Rename( hOBJECT p_old_name, hOBJECT p_new_name, tBOOL p_overwrite );
	tERROR pr_call Delete( hOBJECT p_name );

// Data declaration
	_os_btdiskdata* m_pData; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(OsPrivate)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "OS". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, Recognize )
// Extended method comment
//    Static method. Kernel calls this method wihtout any created objects of this interface.
//    Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//    Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//    It must be quick (preliminary) analysis
// Parameters are:
//   "type" : type of recognized object
tERROR OsPrivate::Recognize( hOBJECT _that, tDWORD p_type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tBOOL  ret_val = errINTERFACE_INCOMPATIBLE;
	PR_TRACE_A2( 0, "Enter OS::Recognize method for object %p (iid=%u) ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A3( 0, "Leave OS::Recognize method for object %p (iid=%u), ret tBOOL = %u", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID), ret_val );
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
tERROR OsPrivate::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::ObjectInit method" );

	// Place your code here
	m_pData = 0;
	if (PR_FAIL(heapAlloc((tPTR*) &m_pData, sizeof(_os_btdiskdata))))
		return errOBJECT_CANNOT_BE_INITIALIZED;

	m_pData->m_pObjectName = 0;
	m_pData->m_Hash = 0;

	cString* hParent = (cString*) sysGetParent(IID_STRING);
	if (hParent)
		hParent->ExportToProp(NULL, cSTRING_WHOLE, *this, pgOBJECT_NAME);

	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR OsPrivate::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::ObjectInitDone method" );

	// Place your code here
	if (!m_pData->m_pObjectName)
		return errOBJECT_NOT_INITIALIZED;

	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR OsPrivate::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	if (m_pData)
	{
		if (m_pData->m_pObjectName)
		{
			m_pData->m_pObjectName->sysCloseObject();
			m_pData->m_pObjectName = 0;
		}
		
		heapFree(m_pData);
		m_pData = 0;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR OsPrivate::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "OS::ObjectClose method" );

	// Place your code here

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
tERROR OsPrivate::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, NamePropRead )
// Interface "OS". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR OsPrivate::NamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method NamePropRead for property pgOBJECT_NAME" );

	if (m_pData->m_pObjectName)
		error = m_pData->m_pObjectName->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);
	else
	{
		*out_size = 0;
		error = errOBJECT_NOT_INITIALIZED;
	}

	PR_TRACE_A2( this, "Leave *GET* method NamePropRead for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, NamePropWrite )
// Interface "OS". Method "Set" for property(s):
//  -- OBJECT_NAME
tERROR OsPrivate::NamePropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method NamePropWrite for property pgOBJECT_NAME" );

	if (!m_pData->m_pObjectName)
	{
		if (PR_SUCC(sysCreateObjectQuick((cObject**) &m_pData->m_pObjectName, IID_STRING)))
			m_pData->m_pObjectName->SetCP(cCP_UNICODE);
		else
		{
			*out_size = 0;
			m_pData->m_pObjectName = 0;
			error = errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}

	if (m_pData->m_pObjectName)
		error = m_pData->m_pObjectName->ImportFromBuff(out_size, buffer, size, cCP_UNICODE, cSTRING_WHOLE);

	PR_TRACE_A2( this, "Leave *SET* method NamePropWrite for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, HashPropRead )
// Interface "OS". Method "Get" for property(s):
//  -- OBJECT_HASH
tERROR OsPrivate::HashPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method HashPropRead for property pgOBJECT_HASH" );

	if (!m_pData)
		error = errOBJECT_NOT_INITIALIZED;
	else
		_integrated_type_prop_get(tQWORD, m_pData->m_Hash);

	PR_TRACE_A2( this, "Leave *GET* method HashPropRead for property pgOBJECT_HASH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReopenPropRead )
// Interface "OS". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR OsPrivate::ReopenPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ReopenPropRead for property pgOBJECT_REOPEN_DATA" );

	*out_size = sizeof(_bt_reopen_data);
	if (buffer)
	{
		if (sizeof(_bt_reopen_data) > size)
			error = errBAD_SIZE;
		else
		{
			_bt_reopen_data* preopen = (_bt_reopen_data*) buffer;
			tSYMBOL symbol;
			if (PR_SUCC(m_pData->m_pObjectName->Symbol(&symbol, STR_RANGE(0, 1), 0, cCP_ANSI, 0)))
			{
				preopen->m_DrvLetter = (tCHAR) symbol;
				preopen->m_BootAreaIdx = 0;
			}
		}
	}


	PR_TRACE_A2( this, "Leave *GET* method ReopenPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReopenPropWrite )
// Interface "OS". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR OsPrivate::ReopenPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ReopenPropWrite for property pgOBJECT_REOPEN_DATA" );

	*out_size = sizeof(_bt_reopen_data);
	if (!buffer)
		error = errPARAMETER_INVALID;
	else
	{
		if (sizeof(_bt_reopen_data) > size)
			error = errBAD_SIZE;
		else
		{
			_bt_reopen_data* preopen = (_bt_reopen_data*) buffer;
				cString* pstrtmp;
			if (PR_FAIL(sysCreateObjectQuick((cObject**) &pstrtmp, IID_STRING)))
				error = errUNEXPECTED;
			else
			{
				pstrtmp->SetCP(cCP_UNICODE);
				if (PR_SUCC(pstrtmp->ImportFromBuff(0, &preopen->m_DrvLetter, sizeof(preopen->m_DrvLetter), cCP_ANSI,
					cSTRING_CONTENT_ONLY)))
				{
					pstrtmp->ExportToProp(0, cSTRING_WHOLE, (cObject*) this, pgOBJECT_NAME);
				}

				pstrtmp->sysCloseObject();
			}
		}
	}

	PR_TRACE_A2( this, "Leave *SET* method ReopenPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
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
tERROR OsPrivate::PtrCreate( hObjPtr* result, hOBJECT p_name )
{
	tERROR  error = errNOT_IMPLEMENTED;
	hObjPtr ret_val = NULL;
	PR_TRACE_FUNC_FRAME( "OS::PtrCreate method" );

	// Place your code here
	error = errOBJECT_NOT_CREATED;

	if (PR_SUCC(sysCreateObject((cObject**) &ret_val, IID_OBJPTR, PID_BTDISK)))
	{
		cString* tmpStr;
		
		if (p_name && PR_SUCC(sysCheckObject(p_name, IID_STRING)))
			tmpStr = (cString*) p_name;
		else
			tmpStr = m_pData->m_pObjectName;

		if (PR_SUCC(tmpStr->ExportToProp(NULL, cSTRING_WHOLE, (cObject*) ret_val, pgOBJECT_PATH)))
			error = ret_val->sysCreateObjectDone();

		if (PR_FAIL(error))
		{
			ret_val->sysCloseObject();
			ret_val = 0;
		}
	}

	*result = ret_val;

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
tERROR OsPrivate::IOCreate( hIO* result, hOBJECT p_name, tDWORD p_access_mode, tDWORD p_open_mode )
{
	tERROR error = errNOT_IMPLEMENTED;
	hIO    ret_val = NULL;
	PR_TRACE_FUNC_FRAME( "OS::IOCreate method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR OsPrivate::Copy( hOBJECT p_src_name, hOBJECT p_dst_name, tBOOL p_overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::Copy method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Extended method comment
//    Renames/moves object by name inside single OS
//    Must move object through folders if nessesary
// Parameters are:
//   "new_name"  : Target name
tERROR OsPrivate::Rename( hOBJECT p_old_name, hOBJECT p_new_name, tBOOL p_overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::Rename method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    phisically deletes an entity on OS
// Behaviour comment
//    if parameter "object" is really hIO or hObjPtr must close it and delete after close.
// Parameters are:
tERROR OsPrivate::Delete( hOBJECT p_name )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "OS::Delete method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "OS". Register function
tERROR OsPrivate::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, OS_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, L"bootable cd or dvd", 38 )
	mSHARED_PROPERTY( pgOBJECT_OS_TYPE, ((tOS_ID)(OS_TYPE_GENERIC)) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(NamePropRead), FN_CUST(NamePropWrite) )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgNAME_MAX_LEN, ((tDWORD)(cNAME_MAX_LEN)) )
	mSHARED_PROPERTY( pgNAME_CASE_SENSITIVE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgNAME_DELIMITER, ((tWORD)("\\")) )
	mSHARED_PROPERTY( pgFORMAT_MEDIA, ((tBOOL)(cFALSE)) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_HASH, FN_CUST(HashPropRead), NULL )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, FN_CUST(ReopenPropRead), FN_CUST(ReopenPropWrite) )
mpPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(OS)
	mINTERNAL_STATIC(Recognize)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(OS)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(OS)
	mEXTERNAL_METHOD(OS, PtrCreate)
	mEXTERNAL_METHOD(OS, IOCreate)
	mEXTERNAL_METHOD(OS, Copy)
	mEXTERNAL_METHOD(OS, Rename)
	mEXTERNAL_METHOD(OS, Delete)
mEXTERNAL_TABLE_END(OS)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "OS::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_BTDISK,                             // plugin identifier
		0x00000000,                             // subtype identifier
		OS_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(OsPrivate)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"OS(IID_OS) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call OS_Register( hROOT root ) { return OsPrivate::Register(root); }
// AVP Prague stamp end



