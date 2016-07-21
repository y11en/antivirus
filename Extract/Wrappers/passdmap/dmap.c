// -------- Wednesday,  09 August 2000,  13:39 --------
// Project -- AVP ver 4
// Sub project -- ZIP Extractor
// Purpose -- Encoded stream extractor
// ...........................................
// Author -- Andy Nikishin
// File Name -- dmap.c
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------


#define IO_PRIVATE_DEFINITION
#include <Prague/prague.h>
#include <Prague/pr_oid.h>

#include "dmap.h"
#include "crypt.h"

#include <memory.h>
#include <string.h>



#define TMP_AVAIL (fAVAIL_READ | fAVAIL_WRITE | fAVAIL_CHANGE_MODE)

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


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, object declaration )
typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	DMapData*          data;   // pointer to the "IO" data structure
} *hi_IO;
// AVP Prague stamp end( IO, object declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table prototypes )
// Interface "IO". Internal method table. Forward declarations
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
//tERROR pr_call IO_ObjectClose( hi_IO _this );
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
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,//IO_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL 
};
// AVP Prague stamp end( IO, Internal method table )
// --------------------------------------------------------------------------------


// AVP Prague stamp begin( IO, interface function forward declarations )
typedef   tERROR (pr_call *fnpIO_SeekRead)   ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SeekWrite)  ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SetSize)    ( hi_IO _this, tQWORD new_size );                    // -- ;
typedef   tERROR (pr_call *fnpIO_GetSize)    ( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );                  // -- returns requested size of the object;
typedef   tERROR (pr_call *fnpIO_Flush)      ( hi_IO _this );                                     // -- flush internal buffers;
// AVP Prague stamp end( IO, interface function forward declarations )



// AVP Prague stamp begin( IO, interface declaration )
struct iIOVtbl  
{
	fnpIO_SeekRead    SeekRead;
	fnpIO_SeekWrite   SeekWrite;
	fnpIO_GetSize     GetSize;
	fnpIO_SetSize     SetSize;
	fnpIO_Flush       Flush;
}; // IO
// AVP Prague stamp end( IO, interface declaration )



// AVP Prague stamp begin( IO, External method table prototypes )
// Interface "IO". External method table. Forward declarations
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end( IO, External method table prototypes )



// AVP Prague stamp begin( IO, External method table )
// Interface "IO". External method table.
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
// AVP Prague stamp begin( IO, Forwarded property methods declarations )
// Interface "IO". Get/Set property methods
tERROR pr_call DMAP_PROP_Get( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call DMAP_PROP_Set( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( IO, Forwarded property methods declarations )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// Interface "IO". Property table
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY     (pgINTERFACE_VERSION, 2 )
	mSHARED_PROPERTY_PTR (pgINTERFACE_COMMENT, "ZIP Encoded Streams Reader", 27 )
	mSHARED_PROPERTY     (pgOBJECT_ORIGIN, OID_GENERIC_IO )
	mSHARED_PROPERTY     (pgOBJECT_AVAILABILITY, TMP_AVAIL )

	mLOCAL_PROPERTY_FN  (pgOBJECT_NAME, DMAP_PROP_Get, DMAP_PROP_Set)
	mLOCAL_PROPERTY_FN  (pgOBJECT_PATH, DMAP_PROP_Get, DMAP_PROP_Set )
	mLOCAL_PROPERTY_FN  (pgOBJECT_FULL_NAME, DMAP_PROP_Get, NULL )

	mLOCAL_PROPERTY_BUF (pgOBJECT_ACCESS_MODE, DMapData, dwAccessMode, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF (pgOBJECT_OPEN_MODE,   DMapData, dwOpenMode,   cPROP_BUFFER_READ )

//	pgOBJECT_DELETE_ON_CLOSE DMap cannot delete any files
//	pgOBJECT_OWNER_OS not defined couse there is no parent OS
//	pgOBJECT_FILL_CHAR not implemented

// DMAP Local Properties
	mLOCAL_PROPERTY_BUF (ppMAP_AREA_START  ,DMapData, qwStartOffset, cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF (ppMAP_AREA_SIZE   ,DMapData, qwAreaSize,    cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF (ppMAP_AREA_ORIGIN ,DMapData, hIoOrigin,     cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT)
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_FATHER ,DMapData, hPropObject,   cPROP_BUFFER_WRITE)
// ZIP Password DMAP local properties
	mLOCAL_PROPERTY_FN  (ppMAP_PROP_PASSWORD, DMAP_PROP_Get, DMAP_PROP_Set )
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_CRC32, DMapData, crc32,cPROP_BUFFER_WRITE)
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_DATE, DMapData, last_mod_file_date, cPROP_BUFFER_WRITE)
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_TIME, DMapData, last_mod_file_time, cPROP_BUFFER_WRITE)
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_FLAG, DMapData, general_purpose_bit_flag, cPROP_BUFFER_WRITE)
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_ERRORCODE,DMapData, error,cPROP_BUFFER_READ)
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Caution !!! )

//#error Caution !!!
// You have to implement "GET or SET" properties for: 
//   - pgOBJECT_NEW


// AVP Prague stamp end( IO, Caution !!! )
// --------------------------------------------------------------------------------

//#define CALL_SYS_ObjectCreate1(obj,new_obj,iid,pid,subtype)          (obj->sys->ObjectCreate       (((hOBJECT)(obj)),new_obj,iid,pid,subtype))

tERROR IO_ObjectInit( hi_IO _this ) 
{
DMapData* data;
tERROR error;

	data = _this->data;

	data->error = 0xffffffff;
	error = CALL_SYS_ObjectCreate((hOBJECT)_this,&data->hHeap, IID_HEAP, PID_ANY,0);
	if(error == errOK)
	{
		CALL_SYS_PropertySetDWord(data->hHeap, pgHEAP_GRANULARITY, 64*1024);
		if(errOK!=CALL_SYS_ObjectCreateDone(data->hHeap))
		{
			CALL_SYS_ObjectClose(data->hHeap);
			data->hHeap = NULL;
			error=errOBJECT_NOT_CREATED;
		}
	}

	return error;
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
tERROR pr_call IO_ObjectInitDone( hi_IO _this ) 
{
DMapData* data;
tERROR error = errOK;

	data = _this->data;
	if(data->hIoOrigin && data->qwAreaSize>12)
	{
		data->error=errDECRIPT_WARNING;
		error=CALL_IO_SeekRead((hi_IO)data->hIoOrigin, NULL, data->qwStartOffset, data->h, RAND_HEAD_LEN);
		if(data->lpszPassword)
			data->error=decrypt(data, data->lpszPassword);
		data->qwAreaSize -= 12;
		data->qwStartOffset += 12;
		data->dwAccessMode = fACCESS_READ | fACCESS_NO_EXTEND | fACCESS_NO_TRUNCATE | fACCESS_NO_BUFFERING | fACCESS_NO_CHANGE_MODE;
		data->dwOpenMode = fOMODE_OPEN_IF_EXIST;
	}
	else
		error=errPARAMETER_INVALID;

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
/*
tERROR pr_call IO_ObjectClose( hi_IO _this ) 
{
	if(_this->data->hHeap)
		CALL_SYS_ObjectClose(_this->data->hHeap);
	return errOK;
}
*/
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekRead"
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ) 
{
tDWORD read;
DMapData* data;
tQWORD i;
tDWORD j;
tDWORD dwRead;
tBYTE* TmpBuffer;
tDWORD dwReadOneMore;
tERROR error;

	read = 0;
	data=_this->data;
	if(result==NULL)
		result=&dwReadOneMore;
	*result = 0;
	if ( offset < data->qwAreaSize )
	{
		read = ( offset + count > data->qwAreaSize ) ? (tDWORD)(data->qwAreaSize - offset) : count;
		if((tDWORD)(offset)<data->qwLastPosition)
		{
			error = CALL_Heap_Alloc(data->hHeap,(tPTR*)&TmpBuffer, 1024);
			if(error!=errOK)
			{
				goto loc_Exit;
			}
			init_keys(data,data->lpszPassword);
			for(i=0;i<data->qwStartOffset+offset;i+=1024)
			{
				dwRead = ( i + 1024 > data->qwStartOffset+offset ) ? (tDWORD)(data->qwStartOffset+offset - i): 1024;
				CALL_IO_SeekRead( (hi_IO)data->hIoOrigin, &dwRead, (tDWORD)(data->qwStartOffset+(tDWORD)i), (tPTR)TmpBuffer, dwRead );
				for(j=0;j<dwRead ;j++)
					update_keys(data, TmpBuffer[j]^=decrypt_byte(data));
			}

			CALL_Heap_Free(data->hHeap,(tPTR)TmpBuffer);
		}
		else if((tDWORD)(offset)>data->qwLastPosition)
		{
			error=CALL_Heap_Alloc(data->hHeap,(tPTR*)&TmpBuffer,1024);
			if(error!=errOK)
			{
				goto loc_Exit;
			}
			for(i=data->qwLastPosition;i<data->qwStartOffset+offset; i+=1024)
			{
				dwRead = ( i + 1024 > data->qwStartOffset+offset ) ? (tDWORD)(data->qwStartOffset+offset - i): 1024;
				CALL_IO_SeekRead( (hi_IO)data->hIoOrigin, &dwRead, (tDWORD)(data->qwStartOffset+(tDWORD)i), (tPTR)TmpBuffer, dwRead );
				for(j=0;j<dwRead ;j++)
					update_keys(data, TmpBuffer[j]^=decrypt_byte(data));
			}

			CALL_Heap_Free(data->hHeap,(tPTR)TmpBuffer);
		}
		error = CALL_IO_SeekRead( (hi_IO)data->hIoOrigin, result, data->qwStartOffset+offset, buffer, read );
		if(PR_SUCC(error))
		{
			for(i=0;i<read;i++)
				update_keys(data, (((tBYTE*)buffer)[i])^=decrypt_byte(data));
			data->qwLastPosition=(tDWORD)(offset+read);
		}
	}
	else
	{
		error=errEOF;
		*result=0;
	}

loc_Exit:
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekWrite"
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ) 
{
	if(result!=NULL)
		*result=0;
	return errNOT_SUPPORTED;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Resize"
tERROR pr_call IO_SetSize( hi_IO _this,  tQWORD new_size ) 
{
	return errNOT_SUPPORTED;
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
		*result=_this->data->qwAreaSize;
		return errOK;
	}
	return errPARAMETER_INVALID;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------


tERROR pr_call IO_Flush( hi_IO _this ) 
{
	return errOK;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Registration call )
// Interface "IO". Register function
tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* error ) 
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
			(hROOT)hInstance,                                   // interface identifier
			IID_IO,                                 // interface identifier
			PID_ZIPPASSWORDMAP,                     // plugin identifier
			0x00000000,                             // subtype identifier
			0x00000002,                             // interface version
			VID_ANDY,                               // interface developer
			&i_IO_vtbl,                             // internal(kernel called) function table
			(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
			&e_IO_vtbl,                             // external function table
			(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
			IO_PropTable,                           // property table
			mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
			sizeof(DMapData),                       // memory size
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


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property method implementation )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
tERROR pr_call DMAP_PROP_Get( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size )
{
tERROR err;
DMapData* data;
tUINT uiTmp;

	err = errOK;
	data=_this->data;

	if ( buffer && size )
	{
		switch(prop)
		{
			case ppMAP_PROP_PASSWORD:
				uiTmp=_toui32(strlen(data->lpszPassword)+1);
				if(uiTmp>=size)
				{
					memcpy(buffer, data->lpszPassword, uiTmp);
				}
				size= uiTmp;
				break;

/*			case pgOBJECT_SIZE_Q:
				size= sizeof(tQWORD);
				*((tQWORD*)buffer)=(tQWORD)data->qwAreaSize;
				break;*/
			case pgOBJECT_NAME:
				if(data->szName[0]!=0)
				{
					size=_toui32(strlen(data->szName)+1);
					memcpy(buffer, data->szName, size);
				}
				else if(data->hPropObject)
					return CALL_SYS_PropertyGet(data->hPropObject, out_size, prop, buffer, size);
				else
				{
					err = errPARAMETER_INVALID;
					size = 0;
				}
				break;
			case pgOBJECT_PATH:
				if(data->szPath[0]!=0)
				{
					size=_toui32(strlen(data->szPath)+1);
					memcpy(buffer, data->szPath, size);
				}
				else if(data->hPropObject)
					return CALL_SYS_PropertyGet(data->hPropObject, out_size, prop, buffer, size);
				else
				{
					err = errPARAMETER_INVALID;
					size = 0;
				}
				break;
			case pgOBJECT_FULL_NAME:
			{
			tUINT ps = _toui32(strlen( data->szPath ));
			tUINT ns = _toui32(strlen( data->szName ));

				if ( ps+ns+1 > size )
				{
					err = errBUFFER_TOO_SMALL;
					size = 0;
				}
				else
				{
					memcpy( buffer,    data->szPath, ps );
					memcpy( buffer+ps, data->szName, ns+1 );
					size = ps + ns + 1;
				}
			}
		}
	}

	else if ( !buffer && !size ) 
	{
		switch(prop)
		{
			case ppMAP_PROP_PASSWORD:
				size=_toui32(strlen(data->lpszPassword)+1);
				break;
//			case pgOBJECT_SIZE_Q:
//				size=sizeof(tQWORD);
//				break;
			case pgOBJECT_NAME:
			case pgOBJECT_PATH:
			case pgOBJECT_FULL_NAME:
				size = MAX_PATH;
				break;
/*			case pgOBJECT_ACCESS_MODE:
				size=sizeof(tDWORD);
				break;*/
		}
		
	}

	else
	{
		err = errPARAMETER_INVALID;
		size = 0;
	}

	if(out_size)
		*out_size=size;
	return err;
}


tERROR pr_call DMAP_PROP_Set( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR err;
DMapData* data;

	err = errOK;
	data=_this->data;

	if ( buffer && size )
	{
		switch(prop)
		{
			case ppMAP_PROP_PASSWORD:
				if(data->lpszPassword!=NULL)
				{
					CALL_Heap_Free(data->hHeap, data->lpszPassword);
				}
				if(errOK==CALL_Heap_Alloc(data->hHeap, (tPTR*)&data->lpszPassword, size+1))
				{
					memcpy(data->lpszPassword, buffer, size);
					data->lpszPassword[size]=0;
					if(data->error != 0xffffffff)
						data->error=decrypt(data, data->lpszPassword);
				}
				else
					err = errNOT_ENOUGH_MEMORY;
				break;
			case pgOBJECT_NAME:
				memcpy(data->szName, buffer, size);
				break;
			case pgOBJECT_PATH:
				memcpy(data->szPath, buffer, size);
				break;
			case pgOBJECT_FULL_NAME:
				err=errPROPERTY_RDONLY;
				size = 0;
				goto loc_exit;
		}
	}

	else if ( !buffer && !size ) 
	{
		switch(prop)
		{
			case ppMAP_PROP_PASSWORD:
			case pgOBJECT_NAME:
			case pgOBJECT_PATH:
				size = MAX_PATH;
				break;
		}
		
	}

	else
	{
		err = errPARAMETER_INVALID;
		size = 0;
	}
loc_exit:
	if(out_size)
		*out_size=size;
	return err;
}
// AVP Prague stamp end( IO, Property method implementation )
// --------------------------------------------------------------------------------

