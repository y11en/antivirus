// -------- Wednesday,  19 July 2000,  14:33 --------
// Project -- Prague
// Sub project -- Temporary File IO
// Purpose -- Temp Files
// ...........................................
// Author -- Andy Nikishin, Graf
// File Name -- tempfile.c
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------

// Фича -- по-умолчанию атрибут DELETE_ON_CLOSE установлен в cTRUE и если надо открыть существующий файл
// то надо не забыть поставить этот атрибут в cFALSE
#if defined (__unix__)

#define IO_PRIVATE_DEFINITION

#include <stdlib.h>

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <plugin/p_tempfile.h>

#include <Prague/iface/i_root.h>
#include <Prague/iface/i_os.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* 
 * Maximal size of a file in memory (in kilobytes)
 * When specified size exceed, the file will be dumped to disk. 
 * If set to 0, files in memory are disabled.
 */
#define MAX_MEMFILE_SIZE	0


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Interface comment )
// --------------------------------------------
// --- 74d331e1_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// IO interface implementation
// Short comments -- input/output interface
// Extended comment
//   Defines behavior of input/output of an object
//   Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position. 
// AVP Prague stamp end( IO, Interface comment )
// --------------------------------------------------------------------------------

#define TMP_AVAIL (fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE )


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Data structure )
// Interface "IO". Inner data structure

typedef struct tag_TempFilesData 
{
	tCHAR    		m_szFileName[MAX_PATH];
	tCHAR    		m_szPath[MAX_PATH];
	tDWORD   		m_dwfAccessMode; // -- 
	tDWORD   		m_dwfOpenMode;
//	tDWORD   		m_dwcbFileSize;
	tORIG_ID 		m_ObjectOrigin;
	tBYTE    		byFillChar;
	tBOOL    		bDeleteOnClose;
	tBOOL    		bInitDone;
	hOBJECT  		hFatherOS;
	hSTRING  		hStr;
	int		 		fd;

} TempFilesData;
// AVP Prague stamp end( IO, Data structure )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, object declaration )
typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	TempFilesData*     data;   // pointer to the "IO" data structure
} *hi_IO;
// AVP Prague stamp end( IO, object declaration )
// --------------------------------------------------------------------------------

typedef   tERROR (pr_call *fnpIO_SeekRead)   ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SeekWrite)  ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SetSize)    ( hi_IO _this, tQWORD new_size );                    // -- ;
typedef   tERROR (pr_call *fnpIO_GetSize)    ( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );                  // -- returns requested size of the object;
typedef   tERROR (pr_call *fnpIO_Flush)      ( hi_IO _this );                                     // -- flush internal buffers;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External method table )
// Interface "IO". External method table.
struct iIOVtbl  
{
	fnpIO_SeekRead    SeekRead;
	fnpIO_SeekWrite   SeekWrite;
	fnpIO_GetSize     GetSize;
	fnpIO_SetSize     SetSize;
	fnpIO_Flush       Flush;
}; // IO


tERROR XSeek(TempFilesData* data, tQWORD dwDistance, tQWORD * pdwNewPosition );
tERROR XWrite(TempFilesData* data, tPTR pvBuffer, tDWORD dwcbToWrite, tDWORD * pdwcbActuallyWritten );
tERROR XRead(TempFilesData* data, tPTR pvBuffer, tDWORD dwcbToRead, tDWORD* pdwcbActuallyRead );
tERROR XSetSize(TempFilesData* data, tQWORD dwLength );
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table prototypes )
// Interface "IO". Internal method table. Forward declarations
static tERROR IO_ObjectInit( hi_IO _this ) ;
static tERROR IO_ObjectInitDone( hi_IO _this );
static tERROR IO_ObjectPreClose( hi_IO _this );
// AVP Prague stamp end( IO, Internal method table prototypes )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table )
// Interface "IO". Internal method table.
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       IO_ObjectInit,
	(tIntFnObjectInitDone)   IO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   IO_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL 
};
// AVP Prague stamp end( IO, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External method table prototypes )
// Interface "IO". External method table. Forward declarations
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SetSize( hi_IO _this,  tQWORD new_size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end( IO, External method table prototypes )
// --------------------------------------------------------------------------------
static iIOVtbl e_IO_vtbl = 
{
	IO_SeekRead,
	IO_SeekWrite,
	IO_GetSize,
	IO_SetSize,
	IO_Flush
};

// AVP Prague stamp end( IO, External method table )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Forwarded property methods declarations )
// Interface "IO". Get/Set property methods
tERROR pr_call IO_PROP_TempIO_PropertyGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call IO_PROP_TempIO_PropertySet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
//tERROR pr_call IO_PROP_IO_NewOblectCreate( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( IO, Forwarded property methods declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// Interface "IO". Property table
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY     (pgINTERFACE_VERSION, 2 )
	mSHARED_PROPERTY_PTR (pgINTERFACE_COMMENT, "Temporary File IO", 18 )
	mSHARED_PROPERTY     (pgOBJECT_AVAILABILITY, TMP_AVAIL  )
	mLOCAL_PROPERTY_BUF  (pgOBJECT_ORIGIN, TempFilesData, m_ObjectOrigin, cPROP_BUFFER_READ|cPROP_BUFFER_WRITE )

	mLOCAL_PROPERTY_BUF (pgOBJECT_NAME, TempFilesData, m_szFileName, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF (pgOBJECT_PATH, TempFilesData, m_szPath,     cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN  (pgOBJECT_FULL_NAME,   IO_PROP_TempIO_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN  (pgOBJECT_ACCESS_MODE, IO_PROP_TempIO_PropertyGet, IO_PROP_TempIO_PropertySet )
	
	mLOCAL_PROPERTY_FN  (pgOBJECT_OPEN_MODE,       IO_PROP_TempIO_PropertyGet, IO_PROP_TempIO_PropertySet)
	mLOCAL_PROPERTY_BUF (pgOBJECT_FILL_CHAR,       TempFilesData, byFillChar,      cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF (pgOBJECT_DELETE_ON_CLOSE, TempFilesData, bDeleteOnClose,  cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF (pgOBJECT_BASED_ON,        TempFilesData, hFatherOS,       cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )

mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------


static void get_temp_file_name (const char * path, char * buf)
{
	static unsigned tmp_file_count = 1;

	while ( 1 )
	{
		sprintf (buf, "%s/%x%x%04x.tmp", 
				path,
				(unsigned) tmp_file_count++,
				(unsigned) getpid(), 
				(unsigned) rand());

		if ( access (buf, R_OK) != 0 && errno == ENOENT )
			break;
	}
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property method implementation )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_FULL_NAME
tERROR pr_call IO_PROP_TempIO_PropertyGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
	tERROR error;
	TempFilesData* data;

	data=_this->data;
	error = errOK;
	if ( buffer && size )
	{
		switch( prop )
		{
			case pgOBJECT_FULL_NAME:
			{
			tUINT ns = strlen( data->m_szFileName );

				if ( /*ps+*/ns+1 > size )
				{
					error = errBUFFER_TOO_SMALL;
					size = 0;
				}
				else
				{
					memcpy( buffer/*+ps*/, data->m_szFileName, ns+1 );
					size = /*ps +*/ ns + 1;
				}
				break;
			}
			case pgOBJECT_ACCESS_MODE:
				if(size >= sizeof(tDWORD))
				{
					*((tDWORD*)buffer) = data->m_dwfAccessMode;
				}
				else
				{
					error = errBUFFER_TOO_SMALL;
				}
				size = sizeof(tDWORD);
				break;
			case pgOBJECT_OPEN_MODE:
				if(size >= sizeof(tDWORD))
				{
					*((tDWORD*)buffer) = data->m_dwfOpenMode;
				}
				else
				{
					error = errBUFFER_TOO_SMALL;
				}
				size = sizeof(tDWORD);
				break;
		}
	}
	else if ( !buffer && !size ) 
	{
		switch(prop)
		{
			case pgOBJECT_FULL_NAME:
				size = MAX_PATH;
				break;
			case pgOBJECT_ACCESS_MODE:
			case pgOBJECT_OPEN_MODE:
				size = sizeof(tDWORD);
				break;
		}
	}
	else
	{
		error = errPARAMETER_INVALID;
		size = 0;
	}

	if(out_size)
		*out_size = size;
	return error;
}
// AVP Prague stamp end( IO, Property method implementation )
// --------------------------------------------------------------------------------

tERROR pr_call IO_PROP_TempIO_PropertySet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
	tERROR error;
	TempFilesData* data;
	
	data=_this->data;
	error = errOK;
	if ( buffer && size )
	{
//		size = 0;

		if(data->bInitDone)
		{
			error = errPROPERTY_NOT_WRITABLE;
			goto loc_exit;

		}
		if(size < sizeof(tDWORD))
		{
			error = errBUFFER_TOO_SMALL;
			goto loc_exit;
		}

		switch( prop )
		{
			case pgOBJECT_ACCESS_MODE:
				size = sizeof(tDWORD);
				data->m_dwfAccessMode = *((tDWORD*)buffer);
				break;
			case pgOBJECT_OPEN_MODE:
				size = sizeof(tDWORD);
				data->m_dwfOpenMode = *((tDWORD*)buffer);
				break;
		}
	}
	else if ( !buffer && !size ) 
	{
		switch(prop)
		{
			case pgOBJECT_ACCESS_MODE:
			case pgOBJECT_OPEN_MODE:
				size = sizeof(tDWORD);
				break;
		}
	}
	else
	{
		error = errPARAMETER_INVALID;
		size = 0;
	}
loc_exit:
	if(out_size)
		*out_size = size;
	return error;
}


// AVP Prague stamp end( IO, Property method implementation )
// --------------------------------------------------------------------------------


tERROR IO_ObjectInit( hi_IO _this ) 
{
	_this->data->m_ObjectOrigin = OID_GENERIC_IO;
	_this->data->bDeleteOnClose = cFALSE;
	_this->data->bInitDone = cFALSE;
	_this->data->fd = -1;
	return errOK;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal (kernel called) interface method implementation )
// --- Interface "IO". Method "ObjectInitDone"
// Extended method comment
//   Notification that all necessary properties have been set and object must go to operation state
// Behaviour comment
//   
// Result comment
//   Returns value differ from errOK if object cannot function properly
tERROR IO_ObjectInitDone( hi_IO _this ) 
{
	TempFilesData * data = _this->data;
	tERROR error = errOK;
	tDWORD size;
	error = CALL_SYS_ObjectCreateQuick(_this, &_this->data->hStr, IID_STRING, PID_ANY, SUBTYPE_ANY );

	if ( data->m_szPath[0] == 0 )
	{
		error = CALL_SYS_SendMsg (_this, pmc_TEMPFILE, pm_TEMPFILE_GETPATH, data->hStr, 0, 0);
                CALL_String_Length(data->hStr, &size);
		if(error!=errOK_DECIDED || size==0)
		{
			if ( getenv("TMP") && access (getenv("TMP"), R_OK|W_OK) == 0 )
				strcpy (data->m_szPath, getenv("TMP"));
			else if ( getenv("TEMP") && access (getenv("TEMP"), R_OK|W_OK) == 0 )
				strcpy (data->m_szPath, getenv("TEMP"));
			else
				strcpy (data->m_szPath, "/tmp");
		}
		else
		{
			error = CALL_String_ExportToBuff( data->hStr, &size, cSTRING_WHOLE, data->m_szPath, sizeof(data->m_szPath), 0, cSTRING_Z);
		}
	}

	if ( data->m_szFileName[0] == 0 )
		get_temp_file_name (data->m_szPath, data->m_szFileName);

	umask (0);
	_this->data->fd = open (data->m_szFileName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP);

	if ( _this->data->fd == -1 )
		return errCREATE_TEMP;

	_this->data->bInitDone = cTRUE;
	return error;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal (kernel called) interface method implementation )
// --- Interface "IO". Method "ObjectClose"
// Extended method comment
//   Kernel warns object it must be closed
// Behaviour comment
//   Object takes all necessary "before closing" actions
// Result comment
//   
tERROR IO_ObjectPreClose( hi_IO _this ) 
{
	tERROR error = errOK;

	if ( _this->data->fd != -1 )
	{
		close (_this->data->fd);
		_this->data->fd = -1;
		//		if ( _this->data->bDeleteOnClose == cTRUE )
		unlink (_this->data->m_szFileName);
	}

	return error;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekRead"
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	tERROR error=errOK;
	tDWORD dwRead;
	TempFilesData * data = _this->data;
	int readlen;

	if ( result == NULL )
		result = &dwRead;

	if( !buffer )
		return errPARAMETER_INVALID;
	else if(data->fd == -1)
		return errOBJECT_ALREADY_FREED;

	if ( lseek (data->fd, offset, SEEK_SET) == -1 )
		return errOBJECT_SEEK;

	readlen = read (data->fd, buffer, size);
	if ( readlen < 0 )
		return errOBJECT_READ;
        else if ( !readlen )
		error = errEOF;
	*result = readlen;
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekWrite"
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) 
{
	tERROR error = errOK;
	tDWORD dwRead;
	TempFilesData * data = _this->data;
	int writelen;

	if ( result == NULL )
		result = &dwRead;
/*
	if ( (data->m_dwfAccessMode & fACCESS_NO_EXTEND) == 0) //!!
	{
		if ( offset <= data->m_dwcbFileSize )
		{
			if ( offset + size > data->m_dwcbFileSize )
			{
				if ( errOK != IO_SetSize(_this, offset+size) )
				{
					error = errOBJECT_RESIZE;
					goto loc_WriteExit;
				}
			}
		}
		else
		{
			error = errOBJECT_SEEK;
			goto loc_WriteExit;
		}
	}

	if ( (data->m_dwfAccessMode & fACCESS_NO_EXTEND_RAND) == 0 )
	{
		if ( offset + size > data->m_dwcbFileSize )
		{
			if ( errOK != IO_SetSize (_this, offset+size) )
			{
				error = errOBJECT_RESIZE;
				goto loc_WriteExit;
			}
		}
	}
*/
	if ( lseek (data->fd, offset, SEEK_SET) == -1 )
	{
		error = errOBJECT_SEEK;
		goto loc_WriteExit;
	}

	writelen = write (data->fd, buffer, size);
	if ( writelen < 0 )
		return errOBJECT_WRITE;

	*result = writelen;

loc_WriteExit:
	return error;
}

// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Resize"
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
	tERROR error = errOBJECT_RESIZE;
	TempFilesData* data = _this->data;

	if ( (data->m_dwfAccessMode & fACCESS_NO_TRUNCATE) == 0 )
	{
		if ( ftruncate (data->fd, new_size) == 0 )
			error = errOK;
	}

	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Size"
// Extended method comment
//   object returns size of requested type (see description of the "type" parameter. Some objects may return same value for all size types
// Parameter "type":
//   Size type is one of the following:
//     -- explicit
//     -- approximate
//     -- estimate
//   
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type ) 
{
	if(result)
	{
		struct stat FileParam;

		if ( fstat (_this->data->fd, &FileParam) != -1)
			*result = (tQWORD) FileParam.st_size;

		return errOK;
	}
	return errPARAMETER_INVALID;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


tERROR pr_call IO_Flush( hi_IO _this )
{
	sync();
	return errOK;
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Registration call )
// Interface "IO". Register function
tBOOL DllMain( tPTR hInstance, tDWORD dwReason, tERROR* error ) 
{
	switch( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH :
	case DLL_THREAD_DETACH : 
		break;
		
	case PRAGUE_PLUGIN_LOAD :
		// register my interfaces
		*error = CALL_Root_RegisterIFace(
			(hROOT)hInstance,
			IID_IO,                                 // interface identifier
			TEMPIO_ID,                              // plugin identifier
			0x00000000,                             // subtype identifier
			0x00000002,                             // interface version
			VID_ANDY,                               // interface developer
			&i_IO_vtbl,                             // internal(kernel called) function table
			(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
			&e_IO_vtbl,                             // external function table
			(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
			IO_PropTable,                           // property table
			mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
			sizeof(TempFilesData),                  // memory size
			0                                       // interface flags
		);
		if ( PR_FAIL(*error  ))
			return cFALSE;
		// register my custom property ids
		// register my exports
		// resolve  my imports
		// use some system resources
		break;
		
	case PRAGUE_PLUGIN_UNLOAD :
		// free system resources
		// unregister my custom property ids -- you can drop it, kernel do it by itself
		// release    my imports		         -- you can drop it, kernel do it by itself
		// unregister my exports		         -- you can drop it, kernel do it by itself
		// unregister my interfaces          -- you can drop it, kernel do it by itself
		break;
	}
	return cTRUE;
}
// AVP Prague stamp end( IO, Registration call )
// --------------------------------------------------------------------------------

#endif //__unix__
