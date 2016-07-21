// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Thursday,  03 August 2006,  16:13 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- MiniKav
// Sub project -- Not defined
// Purpose     -- IO для чтения/записи в файл через драйвер MiniKav.sys
// Author      -- Sobko
// File Name   -- io.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



//+ ----------------------------------------------------------------------------------------
#include <Prague/prague.h>
#include <Prague/pr_oid.h>

#include "../../windows/Hook/hook/IDriver.h"
extern tPROPID propid_EventDefinition;			// cAVPG_EVENT_DEFINITION_PROP

// AVP Prague stamp begin( Includes for interface,  )
#include "io.h"
// AVP Prague stamp end

#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end


// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_Private 
{
	tDWORD m_AccessMode; // --
	tBOOL  m_DeleteOnClose; // --
	tDWORD m_Availability;  // --
	tDWORD m_OpenMode;      // --

	OBJ_BASIC_INFO m_BasicInfo;
} Private;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	Private*           data; // pointer to the "IO" data structure
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
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE p_type );
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD p_new_size );
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
tERROR pr_call IO_PROP_PropertyRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_ReadLastCallProperty( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_WriteLastCallProperty( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
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
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "IO from minikav driver", 23 )
	mSHARED_PROPERTY( pgOBJECT_ORIGIN, ((tORIG_ID)(OID_GENERIC_IO)) )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_PropertyRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_PropertyRead, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_PropertyRead, NULL )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, Private, m_OpenMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, Private, m_AccessMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, Private, m_DeleteOnClose, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOBJECT_AVAILABILITY, Private, m_Availability, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_BASED_ON, ((hOBJECT)(NULL)) )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mLAST_CALL_PROPERTY( IO_PROP_ReadLastCallProperty, IO_PROP_WriteLastCallProperty )
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
		PID_MKAVIO,                             // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(Private),                        // memory size
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



// AVP Prague stamp begin( Property method implementation, PropertyRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_PropertyRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_PropertyRead" );

	{
		tCODEPAGE CodePage = cCP_UNICODE;
		hSTRING hString = (hSTRING) CALL_SYS_ParentGet(_this, IID_STRING);
		
		if (hString == NULL)
			error = errNOT_IMPLEMENTED;
		else
		{
			switch ( prop ) {
			case pgOBJECT_NAME:
				error = CALL_String_ExportToBuff(hString, out_size, cSTRING_WHOLE, buffer, size, CodePage, cSTRING_Z);
				if (PR_SUCC(error) && buffer && *out_size)
				{
					PWCHAR pwch = (PWCHAR)(buffer + *out_size) - 1;
					
					while (pwch > (PWCHAR) buffer)
					{
						if (pwch[0] == L'\\')
						{
							pwch++;

							*out_size = buffer + *out_size - (tCHAR*) pwch;
							memcpy(buffer, pwch, *out_size);

							break;
						}
						pwch--;
					}
				}
				break;
			
			case pgOBJECT_PATH:
				error = CALL_String_ExportToBuff(hString, out_size, cSTRING_WHOLE, buffer, size, CodePage, cSTRING_Z);

				if (PR_SUCC(error) && buffer && *out_size)
				{
					PWCHAR pwch = (PWCHAR)(buffer + *out_size) - 1;
					
					while (pwch > (PWCHAR) buffer)
					{
						if (pwch[0] == L'\\')
						{
							pwch[0] = 0;

							break;
						}
						pwch--;
						*out_size = *out_size - 2;
					}
				}

				break;
			
			case pgOBJECT_FULL_NAME:
				error = CALL_String_ExportToBuff(hString, out_size, cSTRING_WHOLE, buffer, size, CodePage, cSTRING_Z);
				break;
			
			default:
				*out_size = 0;
				break;
			}
		}
	}

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_PropertyRead, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReadLastCallProperty )
// Interface "IO". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_ReadLastCallProperty( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errNOT_SUPPORTED;
	PR_TRACE_A0( _this, "Enter *GET* method for *LAST CALL* property " );

	*out_size = 0;

	{
		PDRV_EVENT_DESCRIBE pDrvDescr = NULL;

		if (PR_FAIL(CALL_SYS_PropertyIsPresent(_this, propid_EventDefinition)))
		{
			PR_TRACE(( _this, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP"));
			return errNOT_SUPPORTED;
		}

		if (PR_SUCC(CALL_SYS_PropertyGet(_this, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
		{
			if (pDrvDescr != NULL)
			{
				switch(prop)
				{
				case plMKAV_FIDBOX:
					if (!buffer || !size)
						error = errUNEXPECTED;
					else
					{
						EVENT_OBJECT_REQUEST InRequest;
						ULONG retsize;

						memset(&InRequest, 0, sizeof(InRequest));

						InRequest.m_AppID = pDrvDescr->m_AppID;
						InRequest.m_Mark = pDrvDescr->m_Mark;
						InRequest.m_RequestType = _event_request_get_fidbox;

						retsize = size;

						*out_size = size;
						error = errOK;

						if (!IDriverEventObjectRequest(pDrvDescr->m_hDevice, &InRequest, buffer, &retsize))
							memset(buffer, 0 , size);
					}

					break;

				case pgOBJECT_NAME_CP:
				case pgOBJECT_FULL_NAME_CP:
				case pgOBJECT_PATH_CP:
					*out_size = sizeof(cCP_UNICODE);
					error = errOK;
					if (buffer)
					{
						if (size >= *out_size)
							*(tCODEPAGE*)buffer = cCP_UNICODE;
						else
							error = errBUFFER_TOO_SMALL;
					}

					break;

				case pgOBJECT_NAME:
				case pgOBJECT_PATH:
				case pgOBJECT_FULL_NAME:
					error = IO_PROP_PropertyRead(_this, out_size, prop, buffer, size);

					break;
				case pgOBJECT_NATIVE_IO:
					{
						// not implemented
					}
					break;

				case pgOBJECT_CREATION_TIME:
					*out_size = sizeof(tDATETIME);
					error = errOK;
					if (buffer)
					{
						if (size >= *out_size && buffer)
						{
							SYSTEMTIME st;
							if (FileTimeToSystemTime((FILETIME*) &_this->data->m_BasicInfo.CreationTime, &st))
							{
								tDATETIME dt;
								error = DTSet(&dt, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds*1000000);
								memcpy(buffer, &dt, *out_size);
							}
							else
								error = errUNEXPECTED;
						}
						else
							error = errBUFFER_TOO_SMALL;
					}

					break;

				case pgOBJECT_LAST_WRITE_TIME:
					*out_size = sizeof(tDATETIME);
					error = errOK;
					if (buffer)
					{
						if (size >= *out_size && buffer)
						{
							SYSTEMTIME st;
							if (FileTimeToSystemTime((FILETIME*) &_this->data->m_BasicInfo.LastWriteTime, &st))
							{
								tDATETIME dt;
								error = DTSet(&dt, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds*1000000);
								memcpy(buffer, &dt, *out_size);
							}
							else
								error = errUNEXPECTED;
						}
						else
							error = errBUFFER_TOO_SMALL;
					}
					break;
					
				case pgOBJECT_ATTRIBUTES:
					*out_size = sizeof(_this->data->m_BasicInfo.ObjAttributes);
					error = errOK;
					if (buffer)
					{
						if (size >= *out_size && buffer)
							memcpy(buffer, &_this->data->m_BasicInfo.ObjAttributes, *out_size);
						else
							error = errBUFFER_TOO_SMALL;
					}
					break;

				default:
					break;
				}
			}
		}
	}


	PR_TRACE_A2( _this, "Leave *GET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, WriteLastCallProperty )
// Interface "IO". Method "Set" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IO_PROP_WriteLastCallProperty( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	// for the sake of "*LAST CALL*" property

	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter *SET* method for *LAST CALL* property " );

	*out_size = 0;
	error = errNOT_SUPPORTED;

	if (PR_FAIL(CALL_SYS_PropertyIsPresent(_this, propid_EventDefinition)))
	{
		PR_TRACE(( _this, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP"));
	}
	else
	{
		PDRV_EVENT_DESCRIBE pDrvDescr = NULL;
		if (PR_SUCC(CALL_SYS_PropertyGet(_this, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
		{
			if (pDrvDescr != NULL)
			{
				error = errOK;
				
				#define _bufsize 1024
				
				if (plMKAV_FIDBOX == prop
					&& buffer
					&& size
					&& size < (_bufsize - sizeof(EVENT_OBJECT_REQUEST))
					)
				{
					CHAR tmpbuffer[_bufsize];
					PEVENT_OBJECT_REQUEST pInRequest = (PEVENT_OBJECT_REQUEST) tmpbuffer;
					
					ULONG retsize;

					memset(pInRequest, 0, _bufsize);

					pInRequest->m_AppID = pDrvDescr->m_AppID;
					pInRequest->m_Mark = pDrvDescr->m_Mark;
					pInRequest->m_RequestType = _event_request_set_fidbox;
					pInRequest->m_RequestBufferSize = size;

					memcpy(pInRequest->m_Buffer, buffer, size);
					retsize = 0;

					if (!IDriverEventObjectRequest(pDrvDescr->m_hDevice, pInRequest, NULL, &retsize))
						error = errUNEXPECTED;
				}
				else
				{
					PR_TRACE(( _this, prtERROR, "LastCallPropWrite 0x%x not supported", prop));
				}
			}
		}
	}

	PR_TRACE_A2( _this, "Leave *SET* method for property *LAST CALL*, ret tDWORD = %u, %terr", *out_size, error );
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
	PR_TRACE_A0( _this, "Enter IO::ObjectInit method" );
	
	// Place your code here
	error = errOK;
	
	_this->data->m_AccessMode = fACCESS_RW | fACCESS_NO_BUFFERING | fACCESS_NO_CHANGE_MODE;
	_this->data->m_DeleteOnClose = cFALSE;
	_this->data->m_Availability = fAVAIL_READ | fAVAIL_WRITE;
	_this->data->m_OpenMode = fOMODE_OPEN_IF_EXIST;
	
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
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IO::ObjectInitDone method" );
	
	// Place your code here
	error = errPARAMETER_INVALID;
	{
		if (PR_FAIL(CALL_SYS_PropertyIsPresent(_this, propid_EventDefinition)))
		{
			PR_TRACE(( _this, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP"));
		}
		else
		{
			PDRV_EVENT_DESCRIBE pDrvDescr = NULL;
			if (PR_SUCC(CALL_SYS_PropertyGet(_this, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
			{
				if (pDrvDescr != NULL)
				{
					EVENT_OBJECT_REQUEST InRequest;
					ULONG retsize;

					memset(&InRequest, 0, sizeof(InRequest));

					InRequest.m_AppID = pDrvDescr->m_AppID;
					InRequest.m_Mark = pDrvDescr->m_Mark;
					InRequest.m_RequestType = _event_request_get_basicinfo;

					retsize = sizeof(_this->data->m_BasicInfo);
					
					if (!IDriverEventObjectRequest(pDrvDescr->m_hDevice, &InRequest, &_this->data->m_BasicInfo, &retsize))
						memset(&_this->data->m_BasicInfo, 0, sizeof(_this->data->m_BasicInfo));
					
					error = errOK;
				}
			}
		}
	}

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
	PR_TRACE_A0( _this, "Enter IO::ObjectClose method" );
	
	// Place your code here
	
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
	PR_TRACE_A0( _this, "Enter IO::SeekRead method" );
	
	// Place your code here
	error = errPARAMETER_INVALID;
	if ((buffer != NULL) && (size != 0))
	{
		PDRV_EVENT_DESCRIBE pDrvDescr = NULL;

		if (PR_FAIL(CALL_SYS_PropertyIsPresent(_this, propid_EventDefinition)))
		{
			PR_TRACE(( g_root, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP"));
			return error;
		}

		error = errUNEXPECTED;
		if (PR_SUCC(CALL_SYS_PropertyGet(_this, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
		{
			if (pDrvDescr != NULL)
			{
				tQWORD curretnsize;
				if (PR_SUCC(IO_GetSize(_this, &curretnsize, IO_SIZE_TYPE_EXPLICIT)))
				{
					if (offset >= curretnsize)
						error = errEOF;
					else
					{
						EVENT_OBJECT_REQUEST InRequest;
						memset(&InRequest, 0, sizeof(InRequest));

						InRequest.m_AppID = pDrvDescr->m_AppID;
						InRequest.m_Mark = pDrvDescr->m_Mark;
						InRequest.m_RequestType = _event_request_read;						
						InRequest.m_Offset = ((PULARGE_INTEGER)&offset)->LowPart;
						InRequest.m_OffsetHi = ((PULARGE_INTEGER)&offset)->HighPart;
						
						ret_val = size;
						if (IDriverEventObjectRequest(pDrvDescr->m_hDevice, &InRequest, buffer,  &ret_val))
							error = errOK;
						else
						{
							PR_TRACE(( g_root, prtIMPORTANT, "mkavio\tread failed: error 0x%x (offset 0x%x:%x)",
								GetLastError(),
								((PULARGE_INTEGER)&offset)->HighPart,
								((PULARGE_INTEGER)&offset)->LowPart
								));
							
							error = errNOT_OK;
							ret_val = 0;
						}
					}
				}
			}
		}
	}
	
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
	PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );
	
	// Place your code here
	error = errPARAMETER_INVALID;
	if ((buffer != NULL) && (size != 0))
	{
		if (PR_FAIL(CALL_SYS_PropertyIsPresent(_this, propid_EventDefinition)))
		{
			PR_TRACE(( _this, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP"));
		}
		else
		{
			PDRV_EVENT_DESCRIBE pDrvDescr = NULL;
			if (PR_SUCC(CALL_SYS_PropertyGet(_this, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
			{
				if (pDrvDescr != NULL)
				{
					ULONG write;
					PEVENT_OBJECT_REQUEST pInRequest = 0;
					if (PR_FAIL(CALL_SYS_ObjHeapAlloc(_this, &pInRequest, sizeof(EVENT_OBJECT_REQUEST) + size)))
						error = errNOT_ENOUGH_MEMORY;
					else
					{
						pInRequest->m_AppID = pDrvDescr->m_AppID;
						pInRequest->m_Mark = pDrvDescr->m_Mark;
						pInRequest->m_RequestType = _event_request_write;
						pInRequest->m_RequestBufferSize = size;
						RtlCopyMemory(pInRequest->m_Buffer, buffer, size);
						
						pInRequest->m_Offset = ((PULARGE_INTEGER)&offset)->LowPart;
						pInRequest->m_OffsetHi = ((PULARGE_INTEGER)&offset)->HighPart;

						write = size;
						
						if (IDriverEventObjectRequest(pDrvDescr->m_hDevice, pInRequest, pInRequest, &write))
						{
							ret_val = write;
							error = errOK;
						}
						else
						{
							DWORD dw = GetLastError();
							error = errOBJECT_BAD_INTERNAL_STATE;
						}

						CALL_SYS_ObjHeapFree(_this, pInRequest);
					}
				}
			}
		}
	}
	
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
	PR_TRACE_A0( _this, "Enter IO::GetSize method" );

	// Place your code here
	error = errPARAMETER_INVALID;
	if (result != NULL)
	{
		if (PR_FAIL(CALL_SYS_PropertyIsPresent(_this, propid_EventDefinition)))
		{
			PR_TRACE(( _this, prtERROR, "cannot find custom property cAVPG_EVENT_DEFINITION_PROP"));
		}
		else
		{
			PDRV_EVENT_DESCRIBE pDrvDescr = NULL;
			if (PR_SUCC(CALL_SYS_PropertyGet(_this, 0, propid_EventDefinition, &pDrvDescr, sizeof(pDrvDescr))))
			{
				if (pDrvDescr != NULL)
				{
					ULONG retsize;
					EVENT_OBJECT_REQUEST InRequest;
					memset(&InRequest, 0, sizeof(InRequest));

					InRequest.m_AppID = pDrvDescr->m_AppID;
					InRequest.m_Mark = pDrvDescr->m_Mark;
					InRequest.m_RequestType = _event_request_get_size;

					retsize = sizeof(tQWORD);
					
					if (IDriverEventObjectRequest(pDrvDescr->m_hDevice, &InRequest, &ret_val, &retsize))
						error = errOK;
				}
			}
		}
	}

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
	PR_TRACE_A0( _this, "Enter IO::SetSize method" );

	// Place your code here

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
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here
	error = errOK;

	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



