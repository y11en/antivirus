// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  08 July 2003,  14:38 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- „тение секторов дл€ операционной системы Windows NT
// Author      -- Sobko
// File Name   -- io.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <prague.h>
#include "io.h"

// AVP Prague stamp end

#include "ptrthread.h"
#include "llda.h"
#define IO_This		_this->data

// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
// Extended comment -
//  Defines behavior of input/output of an object
//  Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_Private 
{
	WCHAR			m_ObjectName[1024];
	tDWORD			m_OpenMode;
	tDWORD			m_AccessMode;
	tDWORD			m_Availability;

	cDiskAccess*	m_pLLDisk;
	tORIG_ID		m_Expected_Origin;
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
tERROR pr_call IO_ObjectPreClose( hi_IO _this );
tERROR pr_call IO_ObjectClose( hi_IO _this );
tERROR pr_call IO_MsgReceive( hi_IO _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
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
tERROR pr_call IO_PROP_DiskPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_DiskPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_UndefinedPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_UndefinedPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
// Interface "IO". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, L"Low level Disk IO for Windows NT platform", 84 )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_ORIGIN, IO_PROP_DiskPropRead, IO_PROP_DiskPropWrite )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, Private, m_ObjectName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT)
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_UndefinedPropRead, IO_PROP_UndefinedPropWrite )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FULL_NAME, Private, m_ObjectName, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT)
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, Private, m_OpenMode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_FN( pgOBJECT_DELETE_ON_CLOSE, IO_PROP_UndefinedPropRead, IO_PROP_UndefinedPropWrite )
	mLOCAL_PROPERTY_BUF( pgOBJECT_AVAILABILITY, Private, m_Availability, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgOBJECT_BASED_ON, IO_PROP_UndefinedPropRead, IO_PROP_UndefinedPropWrite )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ACCESS_MODE, Private, m_AccessMode, cPROP_BUFFER_READ_WRITE)
	mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_UNICODE)) )
	mLOCAL_PROPERTY_FN( plMediaType, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plBytesPerSector, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plSectorsPerTrack, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plTracksPerCylinder, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plCylinders, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plRecognizedPartition, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plBootIndicator, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plPartitionType, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plDriveType, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plDriveNumber, IO_PROP_DiskPropRead, IO_PROP_DiskPropWrite )
	mLOCAL_PROPERTY_FN( pgOBJECT_FILL_CHAR, IO_PROP_UndefinedPropRead, IO_PROP_UndefinedPropWrite )
	mLOCAL_PROPERTY_FN( plIsPartition, IO_PROP_DiskPropRead, NULL )
	mLOCAL_PROPERTY_FN( plIsReadOnly, IO_PROP_DiskPropRead, NULL )
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
		PID_WINDISKIO,                          // plugin identifier
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



// AVP Prague stamp begin( Property method implementation, UndefinedPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_DELETE_ON_CLOSE
//  -- OBJECT_BASED_ON
//  -- OBJECT_FILL_CHAR
tERROR pr_call IO_PROP_UndefinedPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errNOT_SUPPORTED;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_UndefinedPropRead" );

	switch ( prop )
	{
		case pgOBJECT_PATH:
			*out_size = 0;
			break;
		case pgOBJECT_DELETE_ON_CLOSE:
			*out_size = 0;
			break;
		case pgOBJECT_BASED_ON:
			*out_size = 0;
			break;
		case pgOBJECT_FILL_CHAR:
			*out_size = 0;
			break;
		default :
			*out_size = 0;
			break;
	}


	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_UndefinedPropRead, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, UndefinedPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_DELETE_ON_CLOSE
//  -- OBJECT_BASED_ON
//  -- OBJECT_FILL_CHAR
tERROR pr_call IO_PROP_UndefinedPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errNOT_SUPPORTED;
	PR_TRACE_A0( _this, "Enter *SET* multyproperty method IO_PROP_UndefinedPropWrite" );

	switch ( prop )
	{
		case pgOBJECT_PATH:
			*out_size = 0;
			break;
		case pgOBJECT_DELETE_ON_CLOSE:
			*out_size = 0;
			break;
		case pgOBJECT_BASED_ON:
			*out_size = 0;
			break;
		case pgOBJECT_FILL_CHAR:
			*out_size = 0;
			break;
		default:
			*out_size = 0;
			break;
	}

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method IO_PROP_UndefinedPropWrite, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DiskPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- PartitionType
//  -- BootIndicator
//  -- RecognizedPartition
//  -- Cylinders
//  -- TracksPerCylinder
//  -- SectorsPerTrack
//  -- BytesPerSector
//  -- MediaType
tERROR pr_call IO_PROP_DiskPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_DiskPropRead" );
	
	if (!IO_This->m_pLLDisk)
		return errUNEXPECTED;

	error = errOK;

	switch ( prop )
	{
	case plDriveNumber:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->GetDriveNumber() );
		break;
	
	case plDriveType:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->GetDriveType() );
		break;
	
	case plIsPartition:
		_integrated_type_prop_get( tBOOL, IO_This->m_pLLDisk->IsPartitional() );
		break;
	
	case pgOBJECT_ORIGIN:
		_integrated_type_prop_get( tORIG_ID, IO_This->m_pLLDisk->GetOrigin() );
		break;
	
	case plPartitionType:
		_integrated_type_prop_get( tBYTE, IO_This->m_pLLDisk->GetPartitionalType() );
		break;
	
	case plBootIndicator:
		_integrated_type_prop_get( tBYTE, IO_This->m_pLLDisk->IsBootable() );
		break;

	case plRecognizedPartition:
		_integrated_type_prop_get( tBYTE, IO_This->m_pLLDisk->IsPartitionRecognized() );
		break;

	case plCylinders:
		_integrated_type_prop_get( tQWORD, IO_This->m_pLLDisk->GetCylinders() );
		break;

	case plTracksPerCylinder:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->GetTracksPerCylinder() );
		break;

	case plSectorsPerTrack:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->GetSectorsPerTrack() );
		break;

	case plBytesPerSector:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->GetBytesPerSector() );
		break;

	case plMediaType:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->GetMediaType() );
		break;

	case plIsReadOnly:
		_integrated_type_prop_get( tDWORD, IO_This->m_pLLDisk->IsReadOnly() );
		break;
	}

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_DiskPropRead, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, DiskPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ORIGIN
tERROR pr_call IO_PROP_DiskPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_DiskPropWrite for property pgOBJECT_ORIGIN" );

	error = errNOT_IMPLEMENTED;
	if (pgOBJECT_ORIGIN == prop)
	{
		if (sizeof(tORIG_ID) == size && buffer)
		{
			PR_TRACE(( g_root, prtIMPORTANT, "wdiskio\ttrying access physical volume" ));
			IO_This->m_Expected_Origin = *(tORIG_ID*) buffer;
			error = errOK;
		}
		else
			error = errPARAMETER_INVALID;
	}

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_DiskPropWrite for property pgOBJECT_ORIGIN, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
	PR_TRACE_A0( _this, "Enter IO::ObjectInit method" );

	// Place your code here
	IO_This->m_AccessMode = fACCESS_RW;
	IO_This->m_Availability = fAVAIL_READ | fAVAIL_WRITE;

	error = errOK;

	PR_TRACE_A1( _this, "Leave IO::ObjectInit method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A0( _this, "Enter IO::ObjectInitDone method" );
	
	// Place your code here
	error = errOK;
	IO_This->m_pLLDisk = new cDrvAccess( (hOBJECT) _this );

	PR_TRACE(( g_root, prtIMPORTANT, "wdiskio\tcreate request '%S'", IO_This->m_ObjectName ));
	if (IO_This->m_pLLDisk)
	{
		if (!IO_This->m_pLLDisk->Init( IO_This->m_ObjectName ))
		{
			PR_TRACE(( g_root, prtIMPORTANT, "wdiskio\tdrv init failed" ));
			delete IO_This->m_pLLDisk;
			IO_This->m_pLLDisk = NULL;
		}
	}

	if (!IO_This->m_pLLDisk)
	{
		if (OID_PHYSICAL_DISK == IO_This->m_Expected_Origin)
		{
			PR_TRACE(( g_root, prtERROR, "wdiskio\trequested physical volume not supported via generic access" ));
			error = errNOT_SUPPORTED;
		}
		else
		{
			IO_This->m_pLLDisk = new cGenericAccess( (hOBJECT) _this);
			if (IO_This->m_pLLDisk)
			{
				if (!IO_This->m_pLLDisk->Init( IO_This->m_ObjectName ))
				{
					PR_TRACE(( g_root, prtIMPORTANT, "wdiskio\tgeneric init failed" ));
					delete IO_This->m_pLLDisk;
					IO_This->m_pLLDisk = NULL;
				}
			}
		}
	}

	if (IO_This->m_pLLDisk)
		error = errOK;
	else
		error = errOBJECT_NOT_CREATED;

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
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter ObjPtr::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	if (IO_This->m_pLLDisk)
		delete IO_This->m_pLLDisk;
	IO_This->m_pLLDisk = NULL;

	PR_TRACE_A1( _this, "Leave ObjPtr::ObjectPreClose method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A0( _this, "Enter IO::ObjectClose method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave IO::ObjectClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//  Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
// "msg_cls_id"    : Message class identifier
// "msg_id"        : Message identifier
// "send_point"    : Object where the message was send initially
// "ctx"           : Context of the object processing
// "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
// "par_buf"       : Buffer of the parameters
// "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call IO_MsgReceive( hi_IO _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IO::MsgReceive method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave IO::MsgReceive method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//  reads content of the object to the buffer. Returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::SeekRead method" );

	// Place your code here
	error = errOBJECT_BAD_INTERNAL_STATE;
	if (IO_This->m_pLLDisk && IO_This->m_pLLDisk->GetBytesPerSector())
	{
		tDWORD SectorSize = IO_This->m_pLLDisk->GetBytesPerSector();

		tPTR pBuffer = 0;
		tDWORD data_size = 0;
		tDWORD offset_allign;

		ResolveGranularity( SectorSize, &offset, size, &offset_allign, &data_size );

		tQWORD new_offset = offset - offset_allign;
		tDWORD SectorsCount = data_size / SectorSize;

		error = _this->sys->ObjHeapAlloc( (hOBJECT) _this, &pBuffer, data_size );
		if (PR_SUCC( error ))
		{
			error = IO_This->m_pLLDisk->Read( pBuffer, new_offset, data_size, &ret_val );
			if (PR_SUCC( error ))
			{
				ret_val = ret_val > size ? size : ret_val;
				CopyMemory( buffer, (tBYTE*) pBuffer + offset_allign, ret_val );
			}

			_this->sys->ObjHeapFree( (hOBJECT) _this, pBuffer );
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
//  writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );

	// Place your code here
	error = errUNEXPECTED;

	if (IO_This->m_pLLDisk && IO_This->m_pLLDisk->GetBytesPerSector())
	{
		if (!(IO_This->m_AccessMode & fACCESS_WRITE))
			error = errACCESS_DENIED;
		else
		{
			tDWORD SectorSize = IO_This->m_pLLDisk->GetBytesPerSector();

			tPTR pBuffer = 0;
			tDWORD data_size = 0;
			tDWORD offset_allign;

			ResolveGranularity( SectorSize, &offset, size, &offset_allign, &data_size );

			tQWORD new_offset = offset - offset_allign;
			tDWORD SectorsCount = data_size / SectorSize;

			error = _this->sys->ObjHeapAlloc( (hOBJECT) _this, &pBuffer, data_size );
			if (PR_SUCC( error ))
			{
				error = IO_This->m_pLLDisk->Read( pBuffer, new_offset, data_size, &ret_val );
				if (PR_SUCC( error ))
				{
					if (ret_val != data_size)
						error = errOUT_OF_OBJECT;
					else
					{
						CopyMemory( (tBYTE*) pBuffer + offset_allign, buffer, size );
						
						error = IO_This->m_pLLDisk->Write( pBuffer, new_offset, data_size, &ret_val );
						
						if (ret_val != data_size)
							error = errUNEXPECTED;
					}
				}

				_this->sys->ObjHeapFree( (hOBJECT) _this, pBuffer );
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
	PR_TRACE_A0( _this, "Enter IO::GetSize method" );

	// Place your code here
	if (IO_This->m_pLLDisk)
	{
		error = errOK;
		ret_val = IO_This->m_pLLDisk->GetSize();
	}
	else
		error = errUNEXPECTED;
	
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
	error = errNOT_SUPPORTED;

	PR_TRACE_A1( _this, "Leave IO::SetSize method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here
	error = errOK;

	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



