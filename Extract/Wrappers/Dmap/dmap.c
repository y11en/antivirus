// -------- Thursday,  20 July 2000,  19:44 --------
// Project -- Prague
// Sub project -- Direct mapper
// Purpose -- Mapping plugin
// ...........................................
// Author -- Andy Nikishin
// File Name -- dmap.c
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------


#define IO_PRIVATE_DEFINITION

#include "plugin_dmap.h"
#include "dmap.h"

#include <Prague/pr_oid.h>
#include <Prague/pr_sys.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>

#include <Extract/iface/m_dmap.h>

#include <string.h>

#if !defined (MAX_PATH)
	#define MAX_PATH 260
#endif	

#define TMP_AVAIL (fAVAIL_READ | fAVAIL_WRITE | fAVAIL_EXTEND | fAVAIL_TRUNCATE | fAVAIL_CHANGE_MODE)

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
// AVP Prague stamp begin( IO, Data structure )
// Interface "IO". Inner data structure
typedef struct tag_DMapData 
{
	tCHAR   szName[MAX_PATH];	//File Name of mapped file
	tCHAR   szPath[MAX_PATH];	//Path to mapper file
	tQWORD  qwStartOffset;	//Offset in parent IO to mapped file
	tQWORD  qwAreaSize;		// Size of mapped file
	hIO     hIoOrigin;			//IO of father object
	hOBJECT hPropObject;	//Handle of object to get Global properties for mapped object
	tDWORD  dwAccessMode;		// Object access flags
	tDWORD  dwOpenMode;		//Object open|creation flags
	tBOOL   bInitDone;
	tDWORD  dwOrigin;
	tBYTE   byFillChar;
	hOBJECT hFatherOS;
} DMapData;
// AVP Prague stamp end( IO, Data structure )
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
tERROR pr_call IO_ObjectInitDone( hi_IO _this );

tERROR pr_call DMAP_PROP_Get( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call DMAP_PROP_Set( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
//tERROR pr_call IO_PROP_DMAP_PropGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( IO, Internal method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table )
// Interface "IO". Internal method table.
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   IO_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
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


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External method table prototypes )
// Interface "IO". External method table. Forward declarations
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size );
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size );
tERROR pr_call IO_GetSize( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );
tERROR pr_call IO_Flush( hi_IO _this );
// AVP Prague stamp end( IO, External method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
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


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// Interface "IO". Property table
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY     (pgINTERFACE_VERSION, 2 )
	mSHARED_PROPERTY_PTR (pgINTERFACE_COMMENT, "Direct mapping plugin", 22)
	mSHARED_PROPERTY     (pgOBJECT_AVAILABILITY, TMP_AVAIL)
//pgOBJECT_DELETE_ON_CLOSE DMap cannot delete any files
	mSHARED_PROPERTY     (pgOBJECT_DELETE_ON_CLOSE, cFALSE)

	mLOCAL_PROPERTY_FN (pgOBJECT_NAME,      DMAP_PROP_Get, DMAP_PROP_Set)
	mLOCAL_PROPERTY_FN (pgOBJECT_PATH,      DMAP_PROP_Get, DMAP_PROP_Set)
	mLOCAL_PROPERTY_FN (pgOBJECT_FULL_NAME, DMAP_PROP_Get, NULL)

	mLOCAL_PROPERTY_FN (pgOBJECT_OPEN_MODE,   DMAP_PROP_Get, DMAP_PROP_Set)
	mLOCAL_PROPERTY_FN (pgOBJECT_ACCESS_MODE, DMAP_PROP_Get, DMAP_PROP_Set)

	mLOCAL_PROPERTY_BUF (pgOBJECT_FILL_CHAR ,DMapData, byFillChar,  cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
//	pgOBJECT_OWNER_OS not defined couse there is no parent OS
	mLOCAL_PROPERTY_BUF (pgOBJECT_BASED_ON  ,DMapData, hFatherOS,   cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	
// Interface defined properties
	mLOCAL_PROPERTY_BUF (pgOBJECT_ORIGIN,   DMapData, dwOrigin,      cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
    mLOCAL_PROPERTY_BUF (ppMAP_AREA_START,  DMapData, qwStartOffset, cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
    mLOCAL_PROPERTY_BUF (ppMAP_AREA_SIZE,   DMapData, qwAreaSize,    cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
    mLOCAL_PROPERTY_BUF (ppMAP_AREA_ORIGIN, DMapData, hIoOrigin,     cPROP_BUFFER_READ | cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF (ppMAP_PROP_FATHER, DMapData, hPropObject,   cPROP_BUFFER_WRITE )

mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------


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
tERROR error;

	error = errOK;
	if(_this->data->hIoOrigin == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		_this->data->bInitDone = cTRUE;

	_this->data->dwOrigin = CALL_SYS_PropertyGetDWord( _this, pgOBJECT_ORIGIN);

// Set Default values
	if(_this->data->dwOrigin == 0)
		_this->data->dwOrigin = OID_GENERIC_IO;

	_this->data->dwAccessMode = CALL_SYS_PropertyGetDWord(_this->data->hIoOrigin, pgOBJECT_ACCESS_MODE);

	if(_this->data->dwAccessMode == 0)
		_this->data->dwAccessMode = fACCESS_READ | fACCESS_NO_EXTEND | fACCESS_NO_TRUNCATE | fACCESS_NO_BUFFERING | fACCESS_NO_CHANGE_MODE;
	return error;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekRead"
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) 
{
tDWORD dwRead;
DMapData* data;
tERROR error;
tDWORD read;
tBOOL  eof;

    eof = cFALSE;
    read = 0;
    data=_this->data;
    if(result==NULL)
        result=&dwRead;
    if ( offset < data->qwAreaSize )
    {
        if ( offset + (tQWORD)size > data->qwAreaSize )
        {
            read = (tDWORD)(data->qwAreaSize - offset);
            eof = cTRUE;
        }
        else
        {
            read = size;
        }
        error = CALL_IO_SeekRead((hi_IO) data->hIoOrigin, result, data->qwStartOffset+offset, buffer, read );
        if (error == errOK && eof)
        {
            error = errEOF;
        }
    }
    else if (offset > data->qwAreaSize )
    {
        error=errOUT_OF_OBJECT;
        *result=0;
    }
    else
    {
        error=errEOF;
        *result=0;
    }

    return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekWrite"
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD count ) 
{
tDWORD write;
DMapData* data;
tDWORD dwWritten;
tERROR error;
tQWORD n_size;
tDWORD blen = sizeof(n_size);

	write = 0;
	data = _this->data;
	error = errOK;

	if(result == NULL)
		result = &dwWritten;
	if((data->dwAccessMode & fACCESS_WRITE) == fACCESS_WRITE)
	{
		if(offset + count > data->qwAreaSize)
		{
			if((data->dwAccessMode & fACCESS_NO_EXTEND) == 0)
			{
				if(offset <= data->qwAreaSize)
				{
					n_size = offset + count;
					error = CALL_SYS_SendMsg (_this, pmc_DMAP_IO, pm_DMAP_IO_RESIZE, NULL, &n_size, &blen);
				}
				else
					error = errEOF;
			}
			else if((data->dwAccessMode & fACCESS_NO_EXTEND_RAND) == 0)
			{
				n_size = offset + count;
				error = CALL_SYS_SendMsg (_this, pmc_DMAP_IO, pm_DMAP_IO_RESIZE, NULL, &n_size, &blen);
			}
		}
		if(error == errOK)
		{
			if ( offset < data->qwAreaSize)
			{
				write = (offset + count > data->qwAreaSize ) ? (tDWORD)(data->qwAreaSize - offset) : count;
				error = CALL_IO_SeekWrite( (hi_IO)data->hIoOrigin, result, data->qwStartOffset+offset, buffer, write );
			}
			else
				error = errEOF;
		}
	}
	else
		error = errOBJECT_READ_ONLY;

	if(error!=errOK)
	{
		*result=0;
	}

	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "ResizeQ"
tERROR pr_call IO_SetSize( hi_IO _this,  tQWORD new_size ) 
{
tQWORD 	n_size=new_size;
tDWORD	blen=sizeof(tQWORD);
tERROR	err;

	//DMAP Resize goes upper to someone who know how to handle it
	if((_this->data->dwAccessMode & fACCESS_NO_EXTEND) == 0)
	{
		err=CALL_SYS_SendMsg (_this, pmc_DMAP_IO, pm_DMAP_IO_RESIZE, NULL, &n_size, &blen);
		
		if (err==errOK)
			return errOK; //somebody processed our request!
	}
	return errNOT_SUPPORTED;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SizeQ"
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
	return CALL_IO_Flush((hi_IO)(_this->data->hIoOrigin));
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
				(hROOT)hInstance,
				IID_IO,                                 // interface identifier
				DMAP_ID,                                // plugin identifier
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
				IFACE_PROP_TRANSFER                     // interface flags
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


tERROR pr_call DMAP_PROP_Get( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR err;
DMapData* data;

	err = errOK;
	data=_this->data;

	if ( buffer && size )
	{
		switch(prop)
		{
//			case pgOBJECT_ATRIBUTES:
/*			case pgOBJECT_SIZE_Q:
				size= sizeof(tQWORD);
				*((tQWORD*)buffer)=(tQWORD)data->qwAreaSize;
				break;*/
			case pgOBJECT_ACCESS_MODE:
				size= sizeof(tDWORD);
				*((tDWORD*)buffer)=(tDWORD)data->dwAccessMode;
				break;
			case pgOBJECT_OPEN_MODE:
				size= sizeof(tDWORD);
				*((tDWORD*)buffer)=(tDWORD)data->dwOpenMode;
				break;
			case pgOBJECT_NAME_CP:
				if(data->szName[0] != 0)
				{
					*(tCODEPAGE*)buffer=cCP_ANSI;
					size=sizeof(tCODEPAGE);
				}
				else if(data->hPropObject)
					return CALL_SYS_PropertyGet(data->hPropObject, out_size, prop, buffer, size);
				else
				{
					err = errPARAMETER_INVALID;
					size = 0;
				}
				break;
			case pgOBJECT_NAME:
				if(data->szName[0] != 0)
				{
					size=_toui32(strlen(data->szName)+1);
					memcpy(buffer, data->szName, size);
				}
				else if(data->hPropObject){
					tCODEPAGE cp;
					CALL_SYS_PropertyGet(data->hPropObject, 0, pgOBJECT_NAME_CP, &cp, sizeof(tCODEPAGE));
					return CALL_SYS_PropertyGetStr(data->hPropObject, out_size, prop, buffer, size, cp);
				}
				else
				{
					err = errPARAMETER_INVALID;
					size = 0;
				}
				break;
			case pgOBJECT_PATH_CP:
				if(data->szPath[0] != 0)
				{
					*(tCODEPAGE*)buffer=cCP_ANSI;
					size=sizeof(tCODEPAGE);
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
				if(data->szPath[0] != 0)
				{
					size=_toui32(strlen(data->szPath)+1);
					memcpy(buffer, data->szPath, size);
				}
				else if(data->hPropObject){
					tCODEPAGE cp;
					CALL_SYS_PropertyGet(data->hPropObject, 0, pgOBJECT_PATH_CP, &cp, sizeof(tCODEPAGE));
					return CALL_SYS_PropertyGetStr(data->hPropObject, out_size, prop, buffer, size,cp);
				}
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
///				break;
			case pgOBJECT_NAME:
			case pgOBJECT_PATH:
			case pgOBJECT_FULL_NAME:
				size = MAX_PATH;
				break;
			case pgOBJECT_OPEN_MODE:
			case pgOBJECT_ACCESS_MODE:
				size=sizeof(tDWORD);
				break;
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
			case pgOBJECT_ACCESS_MODE:
				if((data->dwAccessMode & fACCESS_NO_CHANGE_MODE) != fACCESS_NO_CHANGE_MODE )
				{
					data->dwAccessMode = *((tDWORD*)buffer);
					size = sizeof(tDWORD);
					// send message to notify creator that
					// access mode changed!
					err = CALL_SYS_SendMsg (_this, pmc_DMAP_IO, pm_DMAP_IO_AMODE_CHANGE, NULL, &buffer, &size);
					//just ignore this error...	
				}
/*				else if((*((tDWORD*)buffer)) & fACCESS_READ )
				{
				}*/
				else
				{
					err = errPROPERTY_NOT_WRITABLE;
					size = 0;
					goto loc_exit;
				}
				break;
			case pgOBJECT_OPEN_MODE:
				if(data->bInitDone != cTRUE)
				{
					data->dwOpenMode=*((tDWORD*)buffer);
					size=sizeof(tDWORD);
					// send message to notify creator that
					// open mode changed!
					err=CALL_SYS_SendMsg (_this, pmc_DMAP_IO, pm_DMAP_IO_OMODE_CHANGE, NULL, &buffer, &size);
					//just ignore this error...	
				}
				else
				{
					err = errPROPERTY_NOT_WRITABLE;
					size = 0;
					goto loc_exit;
				}
				break;
			case pgOBJECT_NAME:
				if(size > sizeof(data->szName)-1)
				{
					err = errBUFFER_TOO_SMALL;
					size = MAX_PATH;
				}
				else
					memcpy(data->szName, buffer, size);
				break;
			case pgOBJECT_PATH:
				if(size > sizeof(data->szName)-1)
				{
					err = errBUFFER_TOO_SMALL;
					size = MAX_PATH;
				}
				else
					memcpy(data->szPath, buffer, size);
				break;
			case pgOBJECT_FULL_NAME:
				err = errPROPERTY_RDONLY;
				size = 0;
				goto loc_exit;
		}
	}

	else if ( !buffer && !size ) 
	{
		switch(prop)
		{
			case pgOBJECT_NAME:
			case pgOBJECT_PATH:
				size = MAX_PATH;
				break;
			case pgOBJECT_OPEN_MODE:
			case pgOBJECT_ACCESS_MODE:
				size=sizeof(tDWORD);
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
