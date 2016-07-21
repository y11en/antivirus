// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  01 June 2004,  17:58 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- btdisk
// Sub project -- Not defined
// Purpose     -- загрузочные cd/dvd
// Author      -- Sobko
// File Name   -- objptr.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define ObjPtr_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_btdisk.h>

#include "btdiskimp.h"
// AVP Prague stamp end

//+ ------------------------------------------------------------------------------------------
#include <Prague/iface/e_ktrace.h>
#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_ktrace.h>
IMPORT_TABLE_END
//+ ------------------------------------------------------------------------------------------


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable PtrPrivate : public cObjPtr /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call NamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PathPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PathPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call FullNamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PtrStateRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SizePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call SizeQPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call AttrPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call HashPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ReopenPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ReopenPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call CreationTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call LastWriteTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call LastAccessTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations

// operations with the current object
// group of methods to navigate object pointers
	tERROR pr_call IOCreate( hIO* result, hSTRING p_name, tDWORD p_access_mode, tDWORD p_open_mode );
	tERROR pr_call Copy( hOBJECT p_dst_name, tBOOL p_overwrite );
	tERROR pr_call Rename( hOBJECT p_new_name, tBOOL p_overwrite );
	tERROR pr_call Delete();

// navigation methods
// group of methods to navigate object pointers
	tERROR pr_call Reset( tBOOL p_to_root );
	tERROR pr_call Clone( hObjPtr* result );
	tERROR pr_call Next();
	tERROR pr_call StepUp();
	tERROR pr_call StepDown();
	tERROR pr_call ChangeTo( hOBJECT p_name );

// Data declaration
	_ptr_btdiskdata* m_pData; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(PtrPrivate)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ObjPtr". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR PtrPrivate::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::ObjectInit method" );

	// Place your code here
	m_pData = 0;
	if (PR_FAIL(heapAlloc((tPTR*) &m_pData, sizeof(_ptr_btdiskdata))))
		return errOBJECT_CANNOT_BE_INITIALIZED;
	
	m_pData->m_pDiskIO = 0;
	
	if (PR_FAIL(sysCreateObjectQuick((cObject**) &m_pData->m_pObjectName, IID_STRING)))
		m_pData->m_pObjectName = 0;
	else if (PR_FAIL(sysCreateObjectQuick((cObject**) &m_pData->m_pObjectPath, IID_STRING)))
		m_pData->m_pObjectPath = 0;

	if (!m_pData->m_pObjectName || !m_pData->m_pObjectPath)
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	else
	{
		m_pData->m_pObjectName->SetCP(cCP_UNICODE);
		m_pData->m_pObjectPath->SetCP(cCP_UNICODE);
		
		m_pData->m_PtrState = cObjPtrState_BEFORE;
		m_pData->m_BootAreaIdxNext = 0;

		error = errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR PtrPrivate::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::ObjectInitDone method" );

	// Place your code here
	if (PR_SUCC(sysCreateObject((cObject**) &m_pData->m_pDiskIO, IID_IO, PID_WINDISKIO)))
	{
		if (PR_SUCC(m_pData->m_pObjectPath->ExportToProp(NULL, cSTRING_WHOLE, (cObject*) m_pData->m_pDiskIO, pgOBJECT_NAME)))
			error = m_pData->m_pDiskIO->sysCreateObjectDone();
		if (PR_FAIL(error))
		{
			PR_TRACE(( g_root, prtWARNING, "btdisk\tinit done failed" ));
			m_pData->m_pDiskIO->sysCloseObject();
			m_pData->m_pDiskIO = 0;
		}
	}

	if (m_pData->m_pDiskIO)
	{
		tDWORD sector_size = m_pData->m_pDiskIO->propGetDWord(plBytesPerSector);
		if (_cd_sector_size != sector_size)
			error = errNOT_SUPPORTED;
		else
			m_pData->m_pObjectName->Cut(cSTRING_WHOLE, 0);
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
tERROR PtrPrivate::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	if (m_pData)
	{
		if (m_pData->m_pObjectName)
		{
			m_pData->m_pObjectName->sysCloseObject();
			m_pData->m_pObjectName = 0;
		}

		if (m_pData->m_pObjectPath)
		{
			m_pData->m_pObjectPath->sysCloseObject();
			m_pData->m_pObjectPath = 0;
		}

		if (m_pData->m_pDiskIO)
		{
			m_pData->m_pDiskIO->sysCloseObject();
			m_pData->m_pDiskIO = 0;
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
tERROR PtrPrivate::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ObjPtr::ObjectClose method" );

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
tERROR PtrPrivate::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, NamePropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR PtrPrivate::NamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method NamePropRead for property pgOBJECT_NAME" );

	error = m_pData->m_pObjectName->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);

	PR_TRACE_A2( this, "Leave *GET* method NamePropRead for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PathPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR PtrPrivate::PathPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method PathPropRead for property pgOBJECT_PATH" );

	error = m_pData->m_pObjectPath->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);

	PR_TRACE_A2( this, "Leave *GET* method PathPropRead for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PathPropWrite )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR PtrPrivate::PathPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method PathPropWrite for property pgOBJECT_PATH" );

	error = m_pData->m_pObjectPath->ImportFromBuff(out_size, buffer, size, cCP_UNICODE, cSTRING_WHOLE);

	PR_TRACE_A2( this, "Leave *SET* method PathPropWrite for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, FullNamePropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR PtrPrivate::FullNamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method FullNamePropRead for property pgOBJECT_FULL_NAME" );

	cString* pstrtmp;
	if (PR_FAIL(sysCreateObjectQuick((cObject**) &pstrtmp, IID_STRING)))
		error = errUNEXPECTED;
	else
	{
		pstrtmp->ImportFromStr(0, m_pData->m_pObjectPath, cSTRING_WHOLE);
		pstrtmp->AddFromBuff(0, ":\\", 2, cCP_ANSI, cSTRING_Z);
		pstrtmp->AddFromStr(0, m_pData->m_pObjectName, cSTRING_WHOLE);

		error = pstrtmp->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);

		pstrtmp->sysCloseObject();
	}

	PR_TRACE_A2( this, "Leave *GET* method FullNamePropRead for property pgOBJECT_FULL_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PtrStateRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_PTR_STATE
tERROR PtrPrivate::PtrStateRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method PtrStateRead for property pgOBJECT_PTR_STATE" );

	if (!m_pData)
		error = errOBJECT_NOT_INITIALIZED;
	else
		_integrated_type_prop_get(tObjPtrState, m_pData->m_PtrState);

	PR_TRACE_A2( this, "Leave *GET* method PtrStateRead for property pgOBJECT_PTR_STATE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SizePropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE
tERROR PtrPrivate::SizePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method SizePropRead for property pgOBJECT_SIZE" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	{
		_integrated_type_prop_get(tDWORD, (m_pData->m_disk_descr.m_sector_count * _cd_sector_size));
	}

	PR_TRACE_A2( this, "Leave *GET* method SizePropRead for property pgOBJECT_SIZE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SizeQPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR PtrPrivate::SizeQPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method SizeQPropRead for property pgOBJECT_SIZE_Q" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else
	{
		_integrated_type_prop_get(tQWORD, (m_pData->m_disk_descr.m_sector_count * _cd_sector_size));
	}

	PR_TRACE_A2( this, "Leave *GET* method SizeQPropRead for property pgOBJECT_SIZE_Q, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, AttrPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_ATTRIBUTES
tERROR PtrPrivate::AttrPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method AttrPropRead for property pgOBJECT_ATTRIBUTES" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else
	{
		_integrated_type_prop_get(tDWORD, m_pData->m_disk_descr.m_platform_id);
	}

	PR_TRACE_A2( this, "Leave *GET* method AttrPropRead for property pgOBJECT_ATTRIBUTES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, HashPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_HASH
tERROR PtrPrivate::HashPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method HashPropRead for property pgOBJECT_HASH" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;

	*out_size = 0;

	PR_TRACE_A2( this, "Leave *GET* method HashPropRead for property pgOBJECT_HASH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReopenPropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR PtrPrivate::ReopenPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ReopenPropRead for property pgOBJECT_REOPEN_DATA" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else
	{
		*out_size = sizeof(_bt_reopen_data);
		if (buffer)
		{
			if (sizeof(_bt_reopen_data) > size)
				error = errBAD_SIZE;
			else
			{
				_bt_reopen_data* preopen = (_bt_reopen_data*) buffer;
				tSYMBOL symbol;
				if (PR_SUCC(m_pData->m_pObjectPath->Symbol(&symbol, STR_RANGE(0, 1), 0, cCP_ANSI, 0)))
				{
					preopen->m_DrvLetter = (tCHAR) symbol;
					preopen->m_BootAreaIdx = m_pData->m_BootAreaIdxNext - 1;
				}
			}
		}
	}

	PR_TRACE_A2( this, "Leave *GET* method ReopenPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReopenPropWrite )
// Interface "ObjPtr". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR PtrPrivate::ReopenPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
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

			m_pData->m_BootAreaIdxNext = preopen->m_BootAreaIdx;

			cString* pstrtmp;
			if (PR_FAIL(sysCreateObjectQuick((cObject**) &pstrtmp, IID_STRING)))
				error = errUNEXPECTED;
			else
			{
				pstrtmp->SetCP(cCP_UNICODE);
				if (PR_SUCC(pstrtmp->ImportFromBuff(0, &preopen->m_DrvLetter, sizeof(preopen->m_DrvLetter), cCP_ANSI,
					cSTRING_CONTENT_ONLY)))
				{
					pstrtmp->ExportToProp(0, cSTRING_WHOLE, (cObject*) this, pgOBJECT_PATH);
				}
				
				pstrtmp->sysCloseObject();
			}
		}
	}

	PR_TRACE_A2( this, "Leave *SET* method ReopenPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CreationTimePropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_CREATION_TIME
tERROR PtrPrivate::CreationTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method CreationTimePropRead for property pgOBJECT_CREATION_TIME" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else
	{
		/*tDATETIME dt = 0;
		 m_pData->m_vtoc.volume_creation_date_and_time.year
		 m_pData->m_vtoc.volume_creation_date_and_time.month
		 m_pData->m_vtoc.volume_creation_date_and_time.day
		_integrated_type_prop_get(tDATETIME, dt);*/
		*out_size = 0;
		error = errNOT_SUPPORTED;
	}

	PR_TRACE_A2( this, "Leave *GET* method CreationTimePropRead for property pgOBJECT_CREATION_TIME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, LastWriteTimePropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_LAST_WRITE_TIME
tERROR PtrPrivate::LastWriteTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method LastWriteTimePropRead for property pgOBJECT_LAST_WRITE_TIME" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else
	{
		*out_size = 0;
		error = errNOT_SUPPORTED;
	}

	PR_TRACE_A2( this, "Leave *GET* method LastWriteTimePropRead for property pgOBJECT_LAST_WRITE_TIME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, LastAccessTimePropRead )
// Interface "ObjPtr". Method "Get" for property(s):
//  -- OBJECT_LAST_ACCESS_TIME
tERROR PtrPrivate::LastAccessTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method LastAccessTimePropRead for property pgOBJECT_LAST_ACCESS_TIME" );

	if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else
	{
		*out_size = 0;
		error = errNOT_SUPPORTED;
	}

	PR_TRACE_A2( this, "Leave *GET* method LastAccessTimePropRead for property pgOBJECT_LAST_ACCESS_TIME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//    Creates hIO or hObjEnum object current in enumeration.
// Result comment
//    Returns hIO or hObjEnum object. Depends on current object.
// Parameters are:
//   "access_mode" : New object open mode
//   "open_mode"   : Attributes of the new object
tERROR PtrPrivate::IOCreate( hIO* result, hSTRING p_name, tDWORD p_access_mode, tDWORD p_open_mode )
{
	tERROR error = errNOT_IMPLEMENTED;
	hIO    ret_val = NULL;
	PR_TRACE_FUNC_FRAME( "ObjPtr::IOCreate method" );

	// Place your code here

	if (p_name || (fACCESS_WRITE & p_access_mode) || (fOMODE_OPEN_IF_EXIST != p_open_mode))
		error = errPARAMETER_INVALID;
	else if (!m_pData || cObjPtrState_PTR != m_pData->m_PtrState)
		error = errOBJECT_BAD_INTERNAL_STATE;
	else 
		error = errOK;

	if (PR_SUCC(error) && PR_SUCC(error = sysCreateObject((cObject**) &ret_val, IID_IO, PID_BTDISK)))
	{
		tSYMBOL symbol;
		_bt_reopen_data reopen;
		m_pData->m_pObjectName->ExportToProp(NULL, cSTRING_WHOLE, (cObject*) ret_val, pgOBJECT_NAME);
		if (PR_SUCC(m_pData->m_pObjectPath->Symbol(&symbol, STR_RANGE(0, 1), 0, cCP_ANSI, 0)))
		{
			reopen.m_DrvLetter = (tCHAR) symbol;
			reopen.m_BootAreaIdx = m_pData->m_BootAreaIdxNext - 1;
			ret_val->propSet(0, pgOBJECT_REOPEN_DATA, &reopen, sizeof(reopen));
		}
		
		error = ret_val->sysCreateObjectDone();

		if (PR_FAIL(error))
		{
			ret_val->sysCloseObject();
			ret_val = 0;
		}
	}

	PR_TRACE(( g_root, prtWARNING, "btdisk\tio create result %terr", error ));

	*result = ret_val;	

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR PtrPrivate::Copy( hOBJECT p_dst_name, tBOOL p_overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::Copy method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Parameters are:
tERROR PtrPrivate::Rename( hOBJECT p_new_name, tBOOL p_overwrite )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::Rename method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Delete )
// Extended method comment
//    pointer must be advanced to the next object
// Parameters are:
tERROR PtrPrivate::Delete()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::Delete method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Extended method comment
//    First call of Next method after Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR PtrPrivate::Reset( tBOOL p_to_root )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::Reset method" );

	// Place your code here
	if (!m_pData)
		error = errUNEXPECTED;
	else
	{
		m_pData->m_PtrState = cObjPtrState_BEFORE;
		m_pData->m_BootAreaIdxNext = 0;

		m_pData->m_pObjectName->Cut(cSTRING_WHOLE, 0);
		
		error = errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// Extended method comment
//    Makes another ObjPtr object. New object must be as identical to source one as possible. Position must be the same
// Parameters are:
tERROR PtrPrivate::Clone( hObjPtr* result )
{
	tERROR  error = errNOT_IMPLEMENTED;
	hObjPtr ret_val = NULL;
	PR_TRACE_FUNC_FRAME( "ObjPtr::Clone method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// Behaviour comment
//    First call of Next method after creating or Reset must return *FIRST* object in enumeration
// Parameters are:
tERROR PtrPrivate::Next()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::Next method" );

	// Place your code here
	if (cFALSE == _get_bootarea_sector_by_idx(m_pData->m_pDiskIO, 
		m_pData->m_BootAreaIdxNext, &m_pData->m_disk_descr))
	{
		m_pData->m_PtrState = cObjPtrState_AFTER;
		error = errEND_OF_THE_LIST;
	}
	else
	{
		//! get name
		tCHAR pathname[0x100];
		pr_sprintf(pathname, sizeof(pathname), "boot_area%04d", m_pData->m_BootAreaIdxNext);

		m_pData->m_pObjectName->ImportFromBuff(0, pathname, 0, cCP_ANSI, cSTRING_WHOLE);

		m_pData->m_BootAreaIdxNext++;
		m_pData->m_PtrState = cObjPtrState_PTR;
		error = errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepUp )
// Parameters are:
tERROR PtrPrivate::StepUp()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::StepUp method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StepDown )
// Parameters are:
tERROR PtrPrivate::StepDown()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::StepDown method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ChangeTo )
// Parameters are:
//   "name" : The same as "object" parameter of OS::EnumCreate method
tERROR PtrPrivate::ChangeTo( hOBJECT p_name )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ObjPtr::ChangeTo method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ObjPtr". Register function
tERROR PtrPrivate::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

	CALL_Root_ResolveImportTable( root, NULL, import_table, PID_BTDISK);
	if (!pr_sprintf)
		return errUNEXPECTED;


// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(ObjPtr_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ObjPtr_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, L"bootable area (cd/dvd) enumerator", 68 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_GENERIC_IO)) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_NAME, FN_CUST(NamePropRead), NULL )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(PathPropRead), FN_CUST(PathPropWrite) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, FN_CUST(FullNamePropRead), NULL )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgIS_FOLDER, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY_PTR( pgMASK, "*", 2 )
	mpLOCAL_PROPERTY_FN( pgOBJECT_PTR_STATE, FN_CUST(PtrStateRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_SIZE, FN_CUST(SizePropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_SIZE_Q, FN_CUST(SizeQPropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ATTRIBUTES, FN_CUST(AttrPropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_HASH, FN_CUST(HashPropRead), NULL )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, FN_CUST(ReopenPropRead), FN_CUST(ReopenPropWrite) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, FN_CUST(CreationTimePropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, FN_CUST(LastWriteTimePropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, FN_CUST(LastAccessTimePropRead), NULL )
mpPROPERTY_TABLE_END(ObjPtr_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ObjPtr)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(ObjPtr)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ObjPtr)
	mEXTERNAL_METHOD(ObjPtr, IOCreate)
	mEXTERNAL_METHOD(ObjPtr, Copy)
	mEXTERNAL_METHOD(ObjPtr, Rename)
	mEXTERNAL_METHOD(ObjPtr, Delete)
	mEXTERNAL_METHOD(ObjPtr, Reset)
	mEXTERNAL_METHOD(ObjPtr, Clone)
	mEXTERNAL_METHOD(ObjPtr, Next)
	mEXTERNAL_METHOD(ObjPtr, StepUp)
	mEXTERNAL_METHOD(ObjPtr, StepDown)
	mEXTERNAL_METHOD(ObjPtr, ChangeTo)
mEXTERNAL_TABLE_END(ObjPtr)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "ObjPtr::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OBJPTR,                             // interface identifier
		PID_BTDISK,                             // plugin identifier
		0x00000000,                             // subtype identifier
		ObjPtr_VERSION,                         // interface version
		VID_SOBKO,                              // interface developer
		&i_ObjPtr_vtbl,                         // internal(kernel called) function table
		(sizeof(i_ObjPtr_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_ObjPtr_vtbl,                         // external function table
		(sizeof(e_ObjPtr_vtbl)/sizeof(tPTR)),   // external function table size
		ObjPtr_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(ObjPtr_PropTable), // property table size
		sizeof(PtrPrivate)-sizeof(cObjImpl),    // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ObjPtr(IID_OBJPTR) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call ObjPtr_Register( hROOT root ) { return PtrPrivate::Register(root); }
// AVP Prague stamp end



