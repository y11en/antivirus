// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  08 April 2003,  20:10 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Process memory scan
// Purpose     -- Доступ к памяти процессов для операционных систем Windows 9x/NT
// Author      -- Sobko
// File Name   -- io.c
// Additional info
//    Триада интерфейсов позволяющая енумерировать процессы в памяти, читать и изменять данные в этой памяти
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "io.h"
#include "objptr.h"

#include <stuff/memmanag.h>

#include <windows.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
// Extended comment -
//  Defines behavior of input/output of an object
//  Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_PM_IOPrivate 
{
	tDWORD m_ObjectSize;                    // размер процесса в памяти
	tCHAR  m_ObjectName[cNAME_MAX_LEN];     // имя процесса
	tCHAR  m_ObjectPath[cNAME_MAX_LEN];     // имя файла из которого был загружен процесс
	tCHAR  m_ObjectFullName[cNAME_MAX_LEN]; // полное имя процесса
	tDWORD m_ObjectOpenMode;                // режим открытия процесса
	tDWORD m_AccessMode;                    // режим доступа к данным процесса
	tBOOL  m_DeleteOnClose;                 // призанк необходимости удалить процесс из памяти после завершения обработки
	tDWORD m_PID;                           // идентификатор процесса
	HANDLE m_hProcess;                      // хэндл процесса
	tDWORD m_NativeError;                   // --
	hOBJECT m_BasedOn;                       // --
} PM_IOPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	PM_IOPrivate*      data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
  // OBJECT DESCRIPTION SECTION TRANSFERRED TO CORRESPONDING HEADER FILE
  // PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectClose( hi_IO _this );
tERROR pr_call IO_MsgReceive( hi_IO _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
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
	(tIntFnMsgReceive)       IO_MsgReceive,
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
tERROR pr_call IO_PROP_CommonPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_CommonPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// Interface "IO". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)2)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
// Interface "IO". Property table
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_MEMORY_PROCESS)) )
	mLOCAL_PROPERTY_BUF( plIO_HANDLEPROCESS, PM_IOPrivate, m_hProcess, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Process memory IO", 18 )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ | fAVAIL_WRITE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, PM_IOPrivate, m_BasedOn, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, PM_IOPrivate, m_ObjectName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgOBJECT_PATH, PM_IOPrivate, m_ObjectPath, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, PM_IOPrivate, m_ObjectFullName, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, PM_IOPrivate, m_ObjectOpenMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, PM_IOPrivate, m_AccessMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, PM_IOPrivate, m_DeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgNATIVE_ERR, PM_IOPrivate, m_NativeError, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_REOPEN_DATA, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_BUF( plProcessID, PM_IOPrivate, m_PID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"IO::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_MEMSCAN,                             // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(PM_IOPrivate),                   // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"IO::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_CommonPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method IO_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA" );

	{
		tDWORD dwSize = sizeof(DWORD) + lstrlen(_this->data->m_ObjectName) + 1;
		if (buffer != NULL)
		{
			if (dwSize < size)
				error = errBUFFER_TOO_SMALL;
			else
			{
				*(DWORD*)buffer = _this->data->m_PID;
				lstrcpy((char*)buffer + sizeof(DWORD), _this->data->m_ObjectName);
			}
		}
		*out_size = dwSize;
	}

	PR_TRACE_A2( _this, "Leave *GET* method IO_PROP_CommonPropRead for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_REOPEN_DATA
tERROR pr_call IO_PROP_CommonPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA" );

	*out_size = size;
	if (size < sizeof(DWORD) + sizeof(tCHAR))
		error = errPARAMETER_INVALID;
	else
	{
		hObjPtr hPtr = 0;
		tCHAR ObjectName[cNAME_MAX_LEN];
		DWORD Pid = *(DWORD*) buffer;

		tCHAR ObjectNameTmp[cNAME_MAX_LEN];
		lstrcpyn(ObjectName, buffer + sizeof(DWORD), size > cNAME_MAX_LEN ? cNAME_MAX_LEN : size);

		if (lstrlen(ObjectName) == 0)
			error = errUNEXPECTED;
		else
		{
			error = errNOT_FOUND;

			if (PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &hPtr, IID_OBJPTR, PID_MEMSCAN, 0)))
			{
				while (PR_SUCC(CALL_ObjPtr_Next(hPtr)))
				{
					if (Pid == CALL_SYS_PropertyGetDWord(hPtr, plPID))
					{
						if (PR_FAIL(CALL_SYS_PropertyGetStr(hPtr, 0, pgOBJECT_NAME, ObjectNameTmp, sizeof(ObjectNameTmp), cCP_ANSI)))
							continue;

						if (lstrcmp(ObjectName, ObjectNameTmp) != 0)
							continue;

						CALL_SYS_PropertySet(_this, 0, plProcessID, &Pid, sizeof(tDWORD));
						CALL_SYS_PropertySetStr(_this, 0, pgOBJECT_NAME, ObjectName, lstrlen(ObjectName), cCP_ANSI);
						error = errOK;
						break;
					}
				}
				CALL_SYS_ObjectClose(hPtr);
			}
		}
	}

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_CommonPropWrite for property pgOBJECT_REOPEN_DATA, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetObjectSizeQ )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_SIZE_Q
tERROR pr_call IO_PROP_GetObjectSizeQ( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"IO_PROP_GetObjectSizeQ\" for property \"pgOBJECT_SIZE_Q\"" );

	*out_size = sizeof(tQWORD);
	if (buffer != NULL)
		*(tQWORD*) buffer = (tQWORD) _this->data->m_ObjectSize;

	PR_TRACE_A2( _this, "Leave *GET* method \"IO_PROP_GetObjectSizeQ\" for property \"pgOBJECT_SIZE_Q\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call IO_ObjectInit( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::ObjectInit\" method" );

	// Place your code here
	_this->data->m_PID = (tDWORD) -1;
	_this->data->m_hProcess = NULL;
	
	error = errNOT_SUPPORTED;
	{
		hOBJECT hParent = CALL_SYS_ParentGet(_this, IID_OS);
		if (hParent != 0)
		{
			if (PR_SUCC(CALL_SYS_ObjectCheck(_this, hParent, IID_OS, PID_MEMSCAN, SUBTYPE_ANY, cFALSE)))
			{
				_this->data->m_BasedOn = hParent;
				error = errOK;
			}
		}
	}
	
	CALL_SYS_RegisterMsgHandler(_this, pmc_MEMORY_PROCESS, rmhLISTENER, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	PR_TRACE_A1( _this, "Leave \"IO::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IO_ObjectInitDone( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::ObjectInitDone\" method" );

	// Place your code here
	error = errOK;
	if (_this->data->m_PID == (tDWORD) -1)
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	else
	{
		DWORD dwAccessMask = PROCESS_ALL_ACCESS;;
		_this->data->m_hProcess = OpenProcess(dwAccessMask, FALSE, _this->data->m_PID);
		if(_this->data->m_hProcess == NULL)
			error = errOBJECT_CANNOT_BE_INITIALIZED;
	}
	
	PR_TRACE_A1( _this, "Leave \"IO::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call IO_ObjectClose( hi_IO _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"IO::ObjectClose\" method" );

	// Place your code here
	_this->data->m_NativeError = ERROR_SUCCESS;
	if (_this->data->m_hProcess != NULL)
	{
		if (_this->data->m_DeleteOnClose == cTRUE)
		{
			if (TerminateProcess(_this->data->m_hProcess, 0) == 0)
			{
				tDWORD del_err;
				tDWORD size = sizeof(del_err);

				del_err = errNOT_OK;
				error = errIO_DELETE_ON_CLOSE_FAILED;
				_this->data->m_NativeError = GetLastError();
				
				CALL_SYS_SendMsg(_this, pmc_IO, pm_IO_DELETE_ON_CLOSE_FAILED, 0, &del_err, &size);
			}
			else
				CALL_SYS_SendMsg(_this, pmc_IO, pm_IO_DELETE_ON_CLOSE_SUCCEED, 0, 0, 0);
		}
		CloseHandle(_this->data->m_hProcess);
	}

	PR_TRACE_A1( _this, "Leave \"IO::ObjectClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//  Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
// "msg_cls_id"    : Message class identifier
// "msg_id"        : Message identifier
// "obj"           : Object as a reason of message
// "ctx"           : Context of the object processing
// "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
// "par_buf"       : Buffer of the parameters
// "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call IO_MsgReceive( hi_IO _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::MsgReceive\" method" );

	// Place your code here
	error = errOK;
	if (msg_id == pm_MEMORY_PROCESS_REQUEST_DELETE)
	{
		if ((par_buf != NULL) && (par_buf_len != NULL) && (*par_buf_len == sizeof(DWORD)))
		{
			if ((_this->data->m_PID == *(DWORD*) par_buf) && (_this->data->m_hProcess != NULL))
				IO_ObjectClose(_this);
		}
	}

	PR_TRACE_A1( _this, "Leave \"IO::MsgReceive\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	SIZE_T ret_val = 0;
	PR_TRACE_A0( _this, "Enter \"IO::SeekRead\" method" );

	// Place your code here
	_this->data->m_NativeError = ERROR_SUCCESS;
	if (_this->data->m_hProcess == NULL)
		error = errHANDLE_WRONG_STATUS;
	else
	{
		tDWORD InternalOffset = (tDWORD) offset;
		error = errOK;
		ret_val = 0;
		if (!ReadProcessMemory(_this->data->m_hProcess, (PVOID)offset, buffer, size, &ret_val))
		{
			if (ret_val == 0)
			{
				error = errOBJECT_READ;
				_this->data->m_NativeError = GetLastError();
			}
		}
	}

	if ( result )
		*result = (tDWORD)ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::SeekRead\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	SIZE_T ret_val = 0;
	PR_TRACE_A0( _this, "Enter \"IO::SeekWrite\" method" );

	// Place your code here
	_this->data->m_NativeError = ERROR_SUCCESS;
	if (_this->data->m_hProcess == NULL)
		error = errHANDLE_WRONG_STATUS;
	else
	{
		tDWORD OldProtection;
		tDWORD _OldProtection;
		tDWORD InternalOffset = (tDWORD) offset;
		
		VirtualProtectEx(_this->data->m_hProcess, (PVOID)InternalOffset, size, PAGE_EXECUTE_READWRITE, &OldProtection);
		if(!WriteProcessMemory(_this->data->m_hProcess, (PVOID)InternalOffset, buffer, size, &ret_val))
		{
			error = errOBJECT_WRITE;
			_this->data->m_NativeError = GetLastError();
		}
		else
			error = errOK;
		
		VirtualProtectEx(_this->data->m_hProcess, (PVOID)InternalOffset, size, OldProtection, &_OldProtection);
	}
	
	if ( result )
		*result = (tDWORD)ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::SeekWrite\" method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSize )
// Extended method comment
//  object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameters are:
// "type" : Size type is one of the following:
//            -- explicit
//            -- approximate
//            -- estimate
//
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type )
{
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter \"IO::GetSize\" method" );

	// Place your code here
	if (_this->data->m_hProcess == NULL)
		error = errHANDLE_WRONG_STATUS;
	else
		ret_val = 1024*1024*4;	// 4 Gb?

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::GetSize\" method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::SetSize\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"IO::SetSize\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//  Flush internal buffers.
// Behaviour comment
//  Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call IO_Flush( hi_IO _this )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter \"IO::Flush\" method" );

	// Place your code here
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave \"IO::Flush\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



