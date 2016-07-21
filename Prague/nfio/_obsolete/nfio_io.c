// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  27 August 2002,  16:15 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Novell NW
// Sub project -- Not defined
// Purpose     -- NetWare native file IO
// Author      -- petrovitch
// File Name   -- nw_io.c
// Additional info
//    File input/output for Novell NetWare
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end

#define DEF_BUFF_SIZE 0x200

// AVP Prague stamp begin( Includes for interface,  )
#if defined (__MWERKS__)
	#include <nwfile.h>
	#include <nwerrno.h>
	#include <string.h>
	#include <io.h>
	#include <errno.h>
#endif

#include <prague.h>
#include <pr_oid.h>
#include <pr_prop.h>
#include "nfio_io.h"

// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO {
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	NFIO_IOData*       data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectClose( hi_IO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = {
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
struct iIOVtbl {
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
static iIOVtbl e_IO_vtbl = {
	IO_SeekRead,
	IO_SeekWrite,
	IO_GetSize,
	IO_SetSize,
	IO_Flush
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call IO_PROP_GetName( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SetName( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_GetPath( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_SetPath( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_GetFullName( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_set_access( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Global(shared) property table variables
#if defined (__unix__)
static tCODEPAGE codepage = cCP_ANSI; // must be READ_ONLY at runtime
#else
static tCODEPAGE codepage = cCP_OEM; // must be READ_ONLY at runtime
#endif
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)2)
// AVP Prague stamp end

tSTRING pr_call FindSeparator( tSTRING buffer, tDWORD* size );
tERROR pr_call SetName( hi_IO _this, tSTRING buffer, tDWORD* size ); 
tERROR pr_call SetPath( hi_IO _this, tSTRING buffer, tDWORD* size );
tERROR pr_call SetFull( hi_IO _this, tSTRING buffer, tDWORD* size, tDWORD path_len );
tERROR pr_call _convert_nfio_errno (int errcode);
tERROR pr_call FillUpTo( hi_IO _this, tQWORD* offset );

// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IO_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "NW input/output interface", 29 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, NFIO_IOData, origin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( pgOBJECT_NAME, IO_PROP_GetName, IO_PROP_SetName )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, IO_PROP_GetPath, IO_PROP_SetPath )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FULL_NAME, IO_PROP_GetFullName, IO_PROP_SetName )
	mLOCAL_PROPERTY_BUF( pgOBJECT_OPEN_MODE, NFIO_IOData, open_mode, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY( pgOBJECT_ACCESS_MODE, NFIO_IOData, access_mode, cPROP_BUFFER_READ, NULL, IO_PROP_set_access )
	mLOCAL_PROPERTY_BUF( pgOBJECT_DELETE_ON_CLOSE, NFIO_IOData, del_on_close, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY( pgOBJECT_AVAILABILITY, ((tDWORD)(fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE|fAVAIL_BUFFERING|fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE)) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, NFIO_IOData, os, cPROP_BUFFER_READ )
	mSHARED_PROPERTY( pgOBJECT_FILL_CHAR, ((tBYTE)(0)) )
	mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME_CP, codepage, sizeof(codepage) )
	mSHARED_PROPERTY_PTR( pgOBJECT_FULL_NAME_CP, codepage, sizeof(codepage) )
	mLOCAL_PROPERTY_BUF( plNATIVE_ERR, NFIO_IOData, nfio_errno, cPROP_BUFFER_READ )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter IO::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_NW_NFIO,                            // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(NFIO_IOData),                       // memory size
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


// AVP Prague stamp begin( Property method implementation, GetName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR pr_call IO_PROP_GetName( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tUINT len;
	tERROR  error = errOK;
	NFIO_IOData* d;
	PR_TRACE_A0( _this, "Enter *GET* method \"IO_PROP_GetName\" for property \"pgOBJECT_FULL_NAME\"" );
	
	d = _this->data;
	len = sizeof(tCHAR) * ( 1 + strlen(d->pname) );
	
	if ( buffer ) {
		if ( len > size ) {
			error = errBUFFER_TOO_SMALL;
			len = 0;
		}
		else
			memcpy( buffer, d->pname, len );
	}

	*out_size = len;
	PR_TRACE_A3( _this, "Leave \"IO::PROP_GetName\" method, ret hUINT = %u, error = 0x%08X, name = \"%s\"", size, error, d->pname );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetName )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_SetName( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	tERROR error = errOK;
	NFIO_IOData* d = _this->data;

	PR_TRACE_A1( _this, "Enter \"IO::PROP_SetName(%s)\"", buffer ? buffer : "NULL" );

	if ( !buffer )
		size = MAX_PATH;

	else {
		tCHAR* p0 = FindSeparator( buffer, &size );
		
		if ( !p0 )                                  // there is only name in the buffer
			error = SetName( _this, buffer, &size );
		else if ( *p0 == 0 ) {                      // there is only path in the buffer
			size = p0 - buffer;
			error = SetPath( _this, buffer, &size );
		}
		else                                        // there are path and name in the buffer
			error = SetFull( _this, buffer, &size, p0-buffer );
			
			if ( PR_SUCC(error) && (prop != pgOBJECT_NAME) )
				CALL_SYS_PropertyInitialized( _this, pgOBJECT_NAME,cTRUE );
		}
	
	*out_size = size;
	PR_TRACE_A3( _this, "Leave \"IO::PROP_SetName\" method, ret hUINT = %u, error = 0x%08X, name = \"%s\"", size, error, d->pname );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetPath )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_PATH
tERROR pr_call IO_PROP_GetPath( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	tUINT  copy_len;
	tUINT  full_len;
	tBOOL  pfx = cFALSE;
	NFIO_IOData* d = _this->data;
	PR_TRACE_A0( _this, "Enter *GET* method \"IO_PROP_GetPath\" for property \"pgOBJECT_PATH\"" );

	copy_len = d->pname - d->ppath;
	full_len = copy_len + sizeof(tCHAR);

	if ( !buffer )
		*out_size = full_len;

	else if ( size < full_len ) {
		error = errBUFFER_TOO_SMALL;
		*out_size = 0;
	}

	else {
		if ( pfx ) {
			*buffer++ = '/';
		}
		memcpy( buffer, d->ppath, copy_len );
		*(MB(buffer)+copy_len) = 0;
	}

	PR_TRACE_A2( _this, "Leave *GET* method \"IO_PROP_GetPath\" for property \"pgOBJECT_PATH\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetPath )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_PATH
tERROR pr_call IO_PROP_SetPath( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	NFIO_IOData* d = _this->data;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_SetPath for property \"pgOBJECT_PATH\"" );

	if ( !buffer )
		size = MAX_PATH;
	else {
		if ( !size && *buffer )
			size = strlen(buffer) * sizeof(tCHAR);
		error = SetPath( _this, buffer, &size );
		if ( PR_SUCC(error) )
			CALL_SYS_PropertyInitialized( _this, pgOBJECT_NAME, cTRUE );
	}

	*out_size = size;

	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_SetPath for property \"pgOBJECT_PATH\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetFullName )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_GetFullName( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tUINT  copy_len;
	tUINT  full_len;
	tBOOL  pfx = cFALSE;
	tERROR error = errOK;
	NFIO_IOData* d = _this->data;
	PR_TRACE_A0( _this, "Enter *GET* method IO::PROP_GetFullName for property \"pgOBJECT_FULL_NAME\"" );

	full_len = copy_len = sizeof(tCHAR) * ( 1 + strlen(d->ppath) );

	if ( !buffer )
		*out_size = full_len;

	else if ( full_len > size ) {
    	error = errBUFFER_TOO_SMALL;
		*out_size = 0;
  }
  else {
		if ( pfx ) {
			*buffer++ = PATH_SEPARATOR;
		}
		memcpy( buffer, d->ppath, copy_len );
		*out_size = full_len;
  }
	
	PR_TRACE_A2( _this, "Leave *GET* method IO::PROP_GetFullName for property \"pgOBJECT_FULL_NAME\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, set_access )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_set_access( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	NFIO_IOData* d = _this->data;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method \"IO_PROP_set_access\" for property \"pgOBJECT_ACCESS_MODE\"" );

	if ( d->fd != -1 && (d->access_mode & fACCESS_NO_CHANGE_MODE) ) {
		size = 0;
		error = errPROPERTY_RDONLY;
	}

	else if ( !buffer != !size ) {
		size = 0;
		error = errPARAMETER_INVALID;
	}

	else if ( buffer ) {
			
		tDWORD mode = *(tDWORD*)buffer;
		
		size = sizeof(d->access_mode);

		if ( (mode & mACCESS) == 0 )
			mode |= fACCESS_READ;
		
		error = errOK;
		if ( d->access_mode == mode )
			;
		
		else if ( d->fd == -1 )
			d->access_mode = mode;

		else if ( (d->access_mode & fACCESS_RW) == (mode & fACCESS_RW) )
			d->access_mode = mode;

		else {
			tDWORD old_mode = d->access_mode;
			
			close (d->fd);
			d->fd = -1;
			d->access_mode = mode;
			if ( PR_FAIL(error=IO_ObjectInitDone(_this)) ) {
				tERROR second;
				d->access_mode = old_mode;
				second = IO_ObjectInitDone( _this );
				if ( PR_FAIL(second) )
					error = second;
				size = 0;
			}
		}
	}
	else
		size = sizeof(d->access_mode);
	
	PR_TRACE_A2( _this, "Leave *SET* method \"IO_PROP_set_access\" for property \"pgOBJECT_ACCESS_MODE\", ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call IO_ObjectInit( hi_IO _this ) {
	tERROR error=errOK;
	hOBJECT os;
	NFIO_IOData* d = _this->data;

	PR_TRACE_A0( _this, "Enter \"IO::ObjectInit\" method" );

	d->nfio_errno = 0;
	d->open_mode = fOMODE_OPEN_IF_EXIST;
	d->access_mode = fACCESS_READ;
	d->origin = OID_GENERIC_IO;
		
	os = CALL_SYS_ParentGet( (hOBJECT)_this, IID_OS );
	while( os ) {
		hOBJECT child;
		CALL_SYS_ChildGetFirst( os, &child, IID_IO, PID_NW_NFIO );
		while( child ) {
			if ( child == (hOBJECT)_this ) {
				d->os = (hOS)os;
				os = 0;
				break;
			}
			if ( PR_FAIL(CALL_SYS_ObjectGetNext(child,&child,IID_ANY,PID_ANY)) ) {
				os = 0;
				break;
			}
		}
		if ( os )
			os = CALL_SYS_ParentGet( os, IID_OS );
	}
	
	error = errOK;

	PR_TRACE_A1( _this, "Leave \"IO::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;

}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call IO_ObjectInitDone( hi_IO _this ) {
	  NFIO_IOData* d=_this->data;
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter \"IO::ObjectInitDone\" method" );

	d->nfio_errno = 0;

	if ( !d->full_name || (0 == *MB(d->ppath)) ) {
	    d->fd = -1;
	    error = errOBJECT_NOT_INITIALIZED;
		PR_TRACE(( _this,prtERROR,"IO::ObjectInitDone -- err=errOBJECT_NOT_INITIALIZED, bad object name (%s)",d->ppath ));
	}
	else {

		int flags = 0;
		tDWORD omode = d->open_mode & (mOMODE_CREATION | mOMODE_SHARE);

		if ( omode & fOMODE_TRUNCATE )
			flags |= (O_CREAT | O_TRUNC);

		if ( omode & fOMODE_CREATE_IF_NOT_EXIST )
			flags |= O_CREAT;
		
		if ( d->access_mode & (fACCESS_READ|fACCESS_WRITE) )
			flags |= O_RDWR;
		else if ( d->access_mode & fACCESS_READ )
			flags |= O_RDONLY;
		else if ( d->access_mode & fACCESS_WRITE )
			flags |= O_WRONLY;
		
		d->fd = open (d->full_name, flags, S_IRUSR|S_IWUSR);
		d->nfio_errno = errno;
		
		if ( d->fd == -1 ) {

			error = errno;
			PR_TRACE(( _this, prtERROR, "IO::ObjectInitDone error == 0x%08x (name=%s)\n flags (%x)", d->nfio_errno, d->full_name, flags));
		}
		else {
			struct stat stbuf;

			if ( stat (d->full_name, &stbuf) == 0 )
			{
				d->size = stbuf.st_size;
			}
			else 
			{
				d->size = 0;
			}
		}
	}

	PR_TRACE_A1( _this, "Leave \"IO::ObjectInitDone\" method, ret tERROR = 0x%08X", error );
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

  	NFIO_IOData* d=_this->data;
  	tERROR error = errOK;
  	PR_TRACE_A0( _this, "Enter \"IO::ObjectClose\" method" );

	d->nfio_errno = 0;

  	if ( d->fd != -1 ) {

    close (d->fd);
  	d->nfio_errno = errno;
	d->fd = -1;

    if ( d->del_on_close ) {
      tDWORD msg_id;
      tDWORD del_err;
      tDWORD size = sizeof(del_err);
		
		if ( unlink(d->full_name) == 0 ) {
	        d->nfio_errno = 0;
	        del_err     = errOK;
	        msg_id      = pm_IO_DELETE_ON_CLOSE_SUCCEED;
	        PR_TRACE(( _this, prtSPAM, "Object \"%s\" deleted on close", d->full_name ));
		}
		else {
        	del_err     = _convert_nfio_errno( d->nfio_errno = errno );
	        msg_id      = pm_IO_DELETE_ON_CLOSE_FAILED;
	        error       = errIO_DELETE_ON_CLOSE_FAILED;
			CALL_SYS_SendMsg( _this, pmc_IO, pm_IO_DELETE_ON_CLOSE_FAILED, 0, &del_err, &size );
			PR_TRACE(( _this, prtSPAM, "Object \"%s\" cannot be deleted. Win error is -- %d", d->full_name, d->nfio_errno ));
		}
    	CALL_SYS_SendMsg( _this, pmc_IO, msg_id, 0, &del_err, &size );
    }
  }
	if ( d->full_name ) {
		CALL_SYS_ObjHeapFree( _this, d->full_name );
		d->full_name = 0;
	}

  PR_TRACE_A0( _this, "Leave \"IO::ObjectClose\" method, ret tERROR = errOK" );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    reads content of the object to the buffer. Returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) 
{
	tERROR       error;
  	NFIO_IOData* d= _this->data;
 	PR_TRACE_A0( _this, "Enter \"IO::SeekRead\" method" );

  	if ( !buffer || !size ) {
    	size = 0;
    	error = errPARAMETER_INVALID;
		goto done;
  	}

	if ( offset > cMAX_LONGLONG ) {
		size = 0;
		error = errOBJECT_SEEK;
		goto done;
	}

  
  	d->nfio_errno = 0;
  
  	if ( ((tLONGLONG)offset) >= d->size ) {
    	size = 0;
    	error = errOUT_OF_OBJECT;
		goto done;
	}

	if ( (((tLONGLONG)offset) + size) > d->size )
		size = (tDWORD)(d->size - offset);
	
	if (
		( d->cp != ((tLONGLONG)offset) ) 
		&& lseek (d->fd, offset, SEEK_SET) == -1 )
	{
		size = 0;
		error = errOBJECT_SEEK;
		goto done;
	}

	if ( read (d->fd, buffer, size) == size )
		error = errOK;
	else {
		d->nfio_errno = errno;
    	error = errOBJECT_READ;
	}
	
	d->cp = lseek (d->fd, 0, SEEK_CUR);

done:;
  if ( result )
    *result = size;

  PR_TRACE_A2( _this, "Leave \"IO::SeekRead\" method, ret tDWORD = %u, error = 0x%08X", size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
	NFIO_IOData*  d = _this->data;
	tERROR        error=errOK;
	tLONGLONG     rest;
	tDWORD        written = 0;
	tLONGLONG     ext = 0;
	PR_TRACE_A0( _this, "Enter \"IO::SeekWrite\" method" );
		
	  if ( !buffer ) {
	    size = 0;
	    error = errPARAMETER_INVALID;
			goto done;
	  }
		
	  if ( offset > cMAX_LONGLONG ) {
			size = 0;
			error = errOBJECT_SEEK;
			goto done;
		}
		
	  	d->nfio_errno = 0;
		
		if ( !(d->access_mode & fACCESS_WRITE) ) {
	    size = 0;
	    error = errOBJECT_READ_ONLY;
			goto done;
		}
		
		rest = ((tLONGLONG)d->size) - ((tLONGLONG)offset); // rest of the file
		
		if ( rest == 0 ) {
			if ( d->access_mode & fACCESS_NO_EXTEND ) {
				size = 0;
				error = errEOF;
				goto done;
			}
			if ( lseek (d->fd, 0, SEEK_END) == -1 ) {
				error = errOBJECT_SEEK;
				goto done;
			}
			ext = size;
		}
		else if ( rest < 0 ) {
			if ( d->access_mode & fACCESS_NO_EXTEND_RAND ) {
				error = errOBJECT_RESIZE;
				size = 0;
				goto done;
			}
			else if ( PR_FAIL(error=FillUpTo(_this,&offset)) ) { // size updated already to new value
				size = 0;
				goto done;
			}
			ext = size;
		}
		else {
			if (
				( d->cp != ((tLONGLONG)offset) ) &&
				lseek (d->fd, offset, SEEK_SET) == -1 )
			{
				error = errOBJECT_SEEK;
				goto done;
			}
			if ( size > rest ) {
				if ( d->access_mode & fACCESS_NO_EXTEND ) {
					size = (tDWORD)rest;
					ext = 0;
				}
				else
					ext = ((tLONGLONG)size) - rest;
			}
			else
				ext = 0;
		}
		
		if ( write (d->fd, buffer, size) ) {
			error = errOK;
			if ( ext != 0 ) {
				if ( size != written )
					ext += written - size;
				d->size += ext;
			}
		}
		else {
			d->nfio_errno = errno;
	    error = errOBJECT_WRITE;
		}
		
	done:;
	  if ( d ) {
		d->cp = lseek (d->fd, 0, SEEK_CUR);
	  }
		if ( result )
			*result = written;
		PR_TRACE_A2( _this, "Leave \"IO::SeekWrite\" method, ret tDWORD = %u, error = 0x%08X", written, error );
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
	PR_TRACE_A0( _this, "Enter \"IO::GetSize\" method" );
  	if ( result )
		*result = _this->data->size;
	_this->data->nfio_errno = 0;
	PR_TRACE_A1( _this, "Leave \"IO::GetSize\" method, ret tQWORD = %I64d, error = errOK", _this->data->size );
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size ) 
{
	NFIO_IOData* d = _this->data;
  	tERROR       error=errOK;
	PR_TRACE_A0( _this, "Enter \"IO::SetSize\" method" );

	d->nfio_errno = 0;

  	if ( !(d->access_mode & fACCESS_WRITE) ) {
		error = errOBJECT_READ_ONLY;
		goto done;
	}

	if ( new_size > cMAX_LONGLONG ) {
		error = errOBJECT_SEEK;
		goto done;
	}
	
	if ( ((tLONGLONG)new_size) > d->size ) {
		if ( d->access_mode & fACCESS_NO_EXTEND )
			error = errOBJECT_INCOMPATIBLE;
		else
		{
#if defined (__MWERKS__)		
			error = chsize( d->fd, new_size );
			if(error!=-1)	d->size=new_size;
			else 
			{
				error=errOBJECT_RESIZE;
				d->nfio_errno = errno;
			}
#else
			error = FillUpTo( _this, &new_size );
#endif			
		}
	}
	else if ( ((tLONGLONG)new_size) < d->size ) {
		if ( d->access_mode & fACCESS_NO_TRUNCATE )
			error = errOBJECT_INCOMPATIBLE;
		else {
#if defined (__MWERKS__)
			if ( chsize (d->fd, new_size) == 0 )	{
#else
			if ( ftruncate (d->fd, new_size) == 0 )	{
#endif						
				d->size = new_size;
				error = errOK;
			}
			else {
				d->nfio_errno = errno;
				error = errOBJECT_RESIZE;
			}
		}
	}
	else // size the same
		error = errOK;
	
done:
	PR_TRACE_A1( _this, "Leave \"IO::SetSize\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Extended method comment
//    Flush internal buffers.
// Behaviour comment
//    Flushes internal buffers. Must return errOK if object opened in RO.
// Parameters are:
tERROR pr_call IO_Flush( hi_IO _this ) {
	tERROR error = errOK;
	NFIO_IOData* d = _this->data;
	PR_TRACE_A0( _this, "Enter IO::Flush method" );

	// Place your code here
#if defined (__MWERKS__)	
	if(FEFlushWrite (d->fd)!=0) {d->nfio_errno=NetWareErrno;error=errUNEXPECTED;}
#else
	sync();
#endif
	
	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// ---
tSTRING pr_call FindSeparator( tSTRING buffer, tDWORD* size ) {
	tCHAR* p0;
	tCHAR* p1;
	tCHAR* p2;
	
	if ( !*size )
		*size = (strlen(buffer) + 1) * MBS;
	
	p0 = strrchr( buffer, '\\' );
	p1 = strrchr( buffer, '/' );
	p2 = strrchr( buffer, ':' );
	
	if ( p0 > (buffer + *size) )
		p0 = 0;
	if ( p1 > (buffer + *size) )
		p1 = 0;
	if ( p2 > (buffer + *size) )
		p2 = 0;
	
	if ( p0 < p1 )
		p0 = p1;
	if ( p0 < p2 )
		p0 = p2;
	if ( p0 )
		p0++;
	
	return p0;
}

tERROR pr_call SetName( hi_IO _this, tSTRING buffer, tDWORD* size ) 
{
	NFIO_IOData* d = _this->data;
	tUINT        path_len;
	tUINT        new_size;

	while( *size && (0 == *(buffer + *size - 1)) )
		(*size)--;
	
	if ( !*size ) {
		if ( d->pname )
			*(d->pname) = 0;
		return errOK;
	}

	path_len = d->pname - d->ppath;
	new_size = path_len + *size + MBS;

	if ( new_size > MAX_PATH )
		return errBUFFER_TOO_SMALL;

	if ( new_size > d->full_name_buff_len ) {
		tERROR error = CALL_SYS_ObjHeapRealloc( _this, &d->full_name, d->full_name, new_size );
		if ( PR_FAIL(error) ) {
			*size = 0;
			return error;
		}
		d->ppath = d->full_name;
		d->pname = d->ppath + path_len;
		d->full_name_buff_len = new_size;
	}

	memcpy( d->pname, buffer, *size );
	*(MB(d->pname) + *size) = 0;
	return errOK;
}




// ---
tERROR pr_call SetPath( hi_IO _this, tSTRING buffer, tDWORD* size ) {
	NFIO_IOData* d = _this->data;
	tUINT name_len;
	tUINT new_size;
	tCHAR symb;
	tUINT add_slash;
	
	while( *size && *(buffer + *size - 1) == 0 )
		(*size)--;

	if ( !*size ) {
		if ( !d->pname )
			;
		else if ( d->pname == d->ppath )
			;
		else if ( !*d->pname )
			*(d->ppath = d->pname) = 0;
		else {
			memmove( d->ppath, d->pname, strlen(d->pname)+1 );
			d->pname = d->ppath;
		}
		return errOK;
	}

	symb = *(buffer + *size - 1);
	add_slash = ((':' != symb) && ('\\' != symb) && ('/' != symb)); // we have to add slash to the end of the path
	name_len = 1 + (d->pname ? strlen(d->pname) : 0);
	new_size = *size + name_len + add_slash;
	
	if ( new_size > MAX_PATH )
		return errBUFFER_TOO_SMALL;

	if ( new_size > d->full_name_buff_len ) {
		tSTRING tmp;
		tERROR  error = CALL_SYS_ObjHeapAlloc( _this, &tmp, new_size );

		if ( PR_FAIL(error) ) {
			*size = 0;
			return error;
		}
		
		memcpy( tmp, buffer, *size );
		if ( add_slash ) {
			*(tmp + *size) = PATH_SEPARATOR;
			(*size) += add_slash;
		}

		if ( d->pname )
			memcpy( tmp+*size, d->pname, name_len );
		else
			*(tmp+*size) = 0;

		if ( d->full_name )
			CALL_SYS_ObjHeapFree( _this, d->full_name );

		d->full_name = d->ppath = tmp;
		d->pname = d->ppath + *size;
		d->full_name_buff_len = new_size;
	}
	else {
		tUINT path_len = d->pname - d->ppath;
		if ( (*size+add_slash) != path_len ) {
			memmove( d->ppath+*size+add_slash, d->ppath+path_len, name_len );
			d->pname = d->ppath + (*size+add_slash);
		}
		memcpy( d->ppath, buffer, *size );
		if ( add_slash ) {
			*(d->ppath + *size) = PATH_SEPARATOR;
			(*size) += add_slash;
		}
	}
	return errOK;
}



// ---
tERROR pr_call SetFull( hi_IO _this, tSTRING buffer, tDWORD* size, tDWORD path_len ) {
	NFIO_IOData* d = _this->data;
	tUINT new_size;
	
	while( *size && *(buffer + *size - 1) == 0 )
		(*size)--;
	
	if ( !*size ) {
		if ( 0 != (d->pname=d->ppath) )
			*(d->ppath) = 0;
		return errOK;
	}
	
	new_size = *size + MBS;
	if ( new_size > MAX_PATH ) {
		*size = 0;
		return errBUFFER_TOO_SMALL;
	}
	
	if ( new_size > d->full_name_buff_len ) {
		tSTRING tmp;
		tERROR  error = CALL_SYS_ObjHeapAlloc( _this, &tmp, new_size );
		if ( PR_FAIL(error) ) {
			*size = 0;
			return error;
		}
		if ( d->full_name )
			CALL_SYS_ObjHeapFree( _this, d->full_name );
		d->ppath = d->full_name = tmp;
		d->full_name_buff_len = new_size;
	}

	memcpy( d->ppath, buffer, *size );
	*(d->ppath + *size) = 0;
	d->pname = d->ppath + path_len;

	return errOK;
}

// ---
tERROR pr_call FillUpTo( hi_IO _this, tQWORD* offset ) {

	NFIO_IOData*  d;
	tPTR   ptr;
	tERROR error;
	tDWORD bsize;
	tDWORD written;
	tQWORD tmp;
	tBYTE  def_buff[100];
	
	d = _this->data;
	d->nfio_errno = 0;

	tmp = lseek (d->fd, 0, SEEK_END); // set file pointer to the end of the file get it value

	if ( tmp == -1 )
		return errOBJECT_SEEK;

	// *offset > end of file -- MUST be checked before function call
	tmp = ((tLONGLONG)*offset) - tmp; // have to write to the file

	if ( (tmp <= (10 * sizeof(def_buff))) || PR_FAIL(CALL_SYS_ObjHeapAlloc(_this,&ptr,bsize=DEF_BUFF_SIZE)) ) {
		ptr = def_buff;
		bsize = sizeof(def_buff);
	}
	memset( ptr, 0, bsize );

	error = errOK;
	while( tmp > 0 ) {
		if ( tmp < bsize )
			written = (tDWORD)tmp;
		else
			written = bsize;

		if ( write (d->fd, ptr, written) == written )
			tmp -= written;
		else {
			d->nfio_errno = errno;
			error = errOBJECT_WRITE;
			break;
		}
	}

	if ( ptr != def_buff )
		CALL_SYS_ObjHeapFree( _this, ptr );

	d->cp = lseek (d->fd, 0, SEEK_CUR);
	d->size = d->cp;

	return error;
}
