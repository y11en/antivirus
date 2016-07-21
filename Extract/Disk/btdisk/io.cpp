// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  25 August 2004,  12:59 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- btdisk
// Sub project -- Not defined
// Purpose     -- загрузочные cd/dvd
// Author      -- Sobko
// File Name   -- io.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_btdisk.h>

#include "btdiskimp.h"
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable IOPrivate : public cIO /*cObjImpl*/ 
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
	tERROR pr_call NamePropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PathPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call PathPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call FullNamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call AccessModePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call AccessModePropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call HashPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ReopenPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call ReopenPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call CreationTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call LastWriteTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call LastAccessTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call LastPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
	tERROR pr_call GetSize( tQWORD* result, IO_SIZE_TYPE p_type );
	tERROR pr_call SetSize( tQWORD p_new_size );
	tERROR pr_call Flush();

// Data declaration
	_io_btdiskdata* m_pData; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(IOPrivate)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR IOPrivate::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::ObjectInit method" );

	// Place your code here
	m_pData = 0;
	if (PR_FAIL(heapAlloc((tPTR*) &m_pData, sizeof(_ptr_btdiskdata))))
		return errOBJECT_CANNOT_BE_INITIALIZED;

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
		m_pData->m_BootAreaIdx = 0;

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
tERROR IOPrivate::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::ObjectInitDone method" );

	if (PR_SUCC(sysCreateObject((cObject**) &m_pData->m_pDiskIO, IID_IO, PID_WINDISKIO)))
	{
		if (PR_SUCC(m_pData->m_pObjectPath->ExportToProp(NULL, cSTRING_WHOLE, (cObject*) m_pData->m_pDiskIO, pgOBJECT_NAME)))
			error = m_pData->m_pDiskIO->sysCreateObjectDone();
		if (PR_FAIL(error))
		{
			m_pData->m_pDiskIO->sysCloseObject();
			m_pData->m_pDiskIO = 0;
		}
	}

	if (m_pData->m_pDiskIO)
	{
		tDWORD sector_size = m_pData->m_pDiskIO->propGetDWord(plBytesPerSector);
		if (_cd_sector_size != sector_size)
			error = errNOT_SUPPORTED;

		if (!_get_bootarea_sector_by_idx(m_pData->m_pDiskIO, m_pData->m_BootAreaIdx, &m_pData->m_disk_descr))
		{
			m_pData->m_pDiskIO->sysCloseObject();
			m_pData->m_pDiskIO = 0;

			error = errNOT_FOUND;
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
tERROR IOPrivate::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::ObjectPreClose method" );

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
tERROR IOPrivate::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "IO::ObjectClose method" );

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
tERROR IOPrivate::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, NamePropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR IOPrivate::NamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method NamePropRead for property pgOBJECT_NAME" );

	error = m_pData->m_pObjectName->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);

	PR_TRACE_A2( this, "Leave *GET* method NamePropRead for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, NamePropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_NAME
tERROR IOPrivate::NamePropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method NamePropWrite for property pgOBJECT_NAME" );

	error = m_pData->m_pObjectName->ImportFromBuff(out_size, buffer, size, cCP_UNICODE, cSTRING_WHOLE);

	PR_TRACE_A2( this, "Leave *SET* method NamePropWrite for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PathPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR IOPrivate::PathPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method PathPropRead for property pgOBJECT_PATH" );

	error = m_pData->m_pObjectPath->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);

	PR_TRACE_A2( this, "Leave *GET* method PathPropRead for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PathPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR IOPrivate::PathPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method PathPropWrite for property pgOBJECT_PATH" );

	error = m_pData->m_pObjectPath->ImportFromBuff(out_size, buffer, size, cCP_UNICODE, cSTRING_WHOLE);

	PR_TRACE_A2( this, "Leave *SET* method PathPropWrite for property pgOBJECT_PATH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, FullNamePropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR IOPrivate::FullNamePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method FullNamePropRead for property pgOBJECT_FULL_NAME" );

	cString* pstrtmp;
	if (PR_FAIL(sysCreateObjectQuick((cObject**) &pstrtmp, IID_STRING)))
		error = errUNEXPECTED;
	else
	{
		pstrtmp->SetCP(cCP_UNICODE);
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



// AVP Prague stamp begin( Property method implementation, AccessModePropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR IOPrivate::AccessModePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method AccessModePropRead for property pgOBJECT_ACCESS_MODE" );

	_integrated_type_prop_get(tDWORD, fACCESS_READ);

	PR_TRACE_A2( this, "Leave *GET* method AccessModePropRead for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, AccessModePropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR IOPrivate::AccessModePropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method AccessModePropWrite for property pgOBJECT_ACCESS_MODE" );

	*out_size = 0;
	if (size != sizeof(tDWORD))
		error = errPARAMETER_INVALID;
	else
	{
		tDWORD access_mode = *(tDWORD*) buffer;
		if (access_mode & fACCESS_WRITE)
			error = errOBJECT_WRITE_NOT_SUPPORTED;
		else
			error = errOK;
	}

	PR_TRACE_A2( this, "Leave *SET* method AccessModePropWrite for property pgOBJECT_ACCESS_MODE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, HashPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_HASH
tERROR IOPrivate::HashPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method HashPropRead for property pgOBJECT_HASH" );

	*out_size = 0;

	PR_TRACE_A2( this, "Leave *GET* method HashPropRead for property pgOBJECT_HASH, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReopenPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR IOPrivate::ReopenPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
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
			if (PR_SUCC(m_pData->m_pObjectPath->Symbol(&symbol, STR_RANGE(0, 1), 0, cCP_ANSI, 0)))
			{
				preopen->m_DrvLetter = (tCHAR) symbol;
				preopen->m_BootAreaIdx = m_pData->m_BootAreaIdx;
			}
		}
	}


	PR_TRACE_A2( this, "Leave *GET* method ReopenPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReopenPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR IOPrivate::ReopenPropWrite( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method ReopenPropWrite for property pgOBJECT_REOPEN_DATA" );

	_bt_reopen_data* preopen;
	if (sizeof(_bt_reopen_data) != size)
	{
		*out_size = 0;
		error = errBAD_SIZE;
	}
	else
	{
		preopen = (_bt_reopen_data*) buffer;
		m_pData->m_BootAreaIdx = preopen->m_BootAreaIdx;

		cString* pstrtmp;
		if (PR_FAIL(sysCreateObjectQuick((cObject**) &pstrtmp, IID_STRING)))
			error = errUNEXPECTED;
		else
		{
			pstrtmp->SetCP(cCP_UNICODE);
			if (PR_SUCC(pstrtmp->ImportFromBuff(0, &preopen->m_DrvLetter, sizeof(preopen->m_DrvLetter), cCP_ANSI,
				cSTRING_CONTENT_ONLY)))
			{
				error = pstrtmp->ExportToProp(0, cSTRING_WHOLE, (cObject*) this, pgOBJECT_PATH);
			}

			pstrtmp->sysCloseObject();

			tCHAR pathname[0x100];
			pr_sprintf(pathname, sizeof(pathname), "boot_area%04d", preopen->m_BootAreaIdx);

			m_pData->m_pObjectName->ImportFromBuff(0, pathname, 0, cCP_ANSI, cSTRING_WHOLE);

		}

	}

	PR_TRACE_A2( this, "Leave *SET* method ReopenPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CreationTimePropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_CREATION_TIME
tERROR IOPrivate::CreationTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method CreationTimePropRead for property pgOBJECT_CREATION_TIME" );

	*out_size = 0;
	error = errNOT_SUPPORTED;

	PR_TRACE_A2( this, "Leave *GET* method CreationTimePropRead for property pgOBJECT_CREATION_TIME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, LastWriteTimePropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_LAST_WRITE_TIME
tERROR IOPrivate::LastWriteTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method LastWriteTimePropRead for property pgOBJECT_LAST_WRITE_TIME" );

	*out_size = 0;
	error = errNOT_SUPPORTED;

	PR_TRACE_A2( this, "Leave *GET* method LastWriteTimePropRead for property pgOBJECT_LAST_WRITE_TIME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, LastAccessTimePropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_LAST_ACCESS_TIME
tERROR IOPrivate::LastAccessTimePropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method LastAccessTimePropRead for property pgOBJECT_LAST_ACCESS_TIME" );

	*out_size = 0;
	error = errNOT_SUPPORTED;

	PR_TRACE_A2( this, "Leave *GET* method LastAccessTimePropRead for property pgOBJECT_LAST_ACCESS_TIME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, LastPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR IOPrivate::LastPropRead( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method for *LAST CALL* property " );

	switch (prop)
	{
	case pgOBJECT_NAME_CP:
	case pgOBJECT_PATH_CP:
	case pgOBJECT_FULL_NAME_CP:
		_integrated_type_prop_get(cCODEPAGE, cCP_UNICODE);
		break;
	case plPartitionType:
		error = errNOT_SUPPORTED;
		break;
	case plBootIndicator:
		_integrated_type_prop_get(tDWORD, cTRUE);
		break;
	case plRecognizedPartition:
		error = errNOT_SUPPORTED;
		break;
	case plCylinders:
		error = errNOT_SUPPORTED;
		break;
	case plTracksPerCylinder:
		error = errNOT_SUPPORTED;
		break;
	case plSectorsPerTrack:
		error = errNOT_SUPPORTED;
		break;
	case plBytesPerSector:
		_integrated_type_prop_get(tDWORD, 512);
		break;
	case plMediaType:
		_integrated_type_prop_get(tDWORD, cMEDIA_TYPE_REMOVABLE);
		break;
	case plDriveType:
		_integrated_type_prop_get(tDWORD, cFILE_DEVICE_CD_ROM);
		break;
	case plDriveNumber:
		error = errNOT_SUPPORTED;
		break;
	default:
		*out_size = 0;
		error = errNOT_SUPPORTED;
	}

	PR_TRACE_A2( this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR IOPrivate::SeekRead( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::SeekRead method" );

	// Place your code here
	if (p_offset > m_pData->m_disk_descr.m_sector_count * _cd_sector_size)
		error = errOUT_OF_OBJECT;
	else
	{
		if (p_offset + p_size > m_pData->m_disk_descr.m_sector_count * _cd_sector_size)
			p_size = (tDWORD)(p_offset + (tQWORD) p_size - ((tQWORD)m_pData->m_disk_descr.m_sector_count) * _cd_sector_size);
		error = m_pData->m_pDiskIO->SeekRead(result, p_offset + m_pData->m_disk_descr.m_load_rba * _cd_sector_size, p_buffer, p_size);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR IOPrivate::SeekWrite( tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::SeekWrite method" );

	// Place your code here
	error = errNOT_SUPPORTED;

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
tERROR IOPrivate::GetSize( tQWORD* result, IO_SIZE_TYPE p_type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_FUNC_FRAME( "IO::GetSize method" );

	// Place your code here
	if (!result)
		error = errPARAMETER_INVALID;
	else
	{
		error = errOK;
		*result = m_pData->m_disk_descr.m_sector_count * _cd_sector_size;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR IOPrivate::SetSize( tQWORD p_new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::SetSize method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR IOPrivate::Flush()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "IO::Flush method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "IO". Register function
tERROR IOPrivate::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, L"single bootable area on cd/drvd", 64 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_LOGICAL_DRIVE)) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(NamePropRead), FN_CUST(NamePropWrite) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(PathPropRead), FN_CUST(PathPropWrite) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, FN_CUST(FullNamePropRead), NULL )
	mSHARED_PROPERTY( pgOBJECT_OPEN_MODE, ((tDWORD)(fACCESS_READ)) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, FN_CUST(AccessModePropRead), FN_CUST(AccessModePropWrite) )
	mSHARED_PROPERTY( pgOBJECT_DELETE_ON_CLOSE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ)) )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)("")) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_HASH, FN_CUST(HashPropRead), NULL )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, FN_CUST(ReopenPropRead), FN_CUST(ReopenPropWrite) )
	mSHARED_PROPERTY( pgOBJECT_ATTRIBUTES, ((tDWORD)(fATTRIBUTE_READONLY)) )
	mpLOCAL_PROPERTY_FN( pgOBJECT_CREATION_TIME, FN_CUST(CreationTimePropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_WRITE_TIME, FN_CUST(LastWriteTimePropRead), NULL )
	mpLOCAL_PROPERTY_FN( pgOBJECT_LAST_ACCESS_TIME, FN_CUST(LastAccessTimePropRead), NULL )
	mpLAST_CALL_PROPERTY( FN_CUST(LastPropRead), NULL )
mpPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(IO)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
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
		PID_BTDISK,                             // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(IOPrivate)-sizeof(cObjImpl),     // memory size
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

tERROR pr_call IO_Register( hROOT root ) { return IOPrivate::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// Last call property is checked, but neither *GET* nor *SET* function names are not defined
// AVP Prague stamp end



