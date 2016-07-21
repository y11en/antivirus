// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  07 October 2002,  16:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Multi part DMAP wrapper
// Author      -- Andy Nikishin
// File Name   -- mdmap.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define IO_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "mdmap.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



typedef struct tag_Region 
{
	hIO     hFatherIo;      // --
	tDWORD  dwSizeOfBlock;   // --
	tQWORD  qwOffsetInFatherIo; // --
	tQWORD  qwOffsetInObject;
	struct tag_Region* Next;     // --
} Region;

__inline Region* GetRegion(Region* pRegion, tQWORD offset);

// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_MDMAP_Data 
{
	tDWORD dwOrigin;    // --
	hIO    hFatherOS;   // --
	tBYTE  byFillChar;  // --
	hOBJECT hPropObject;      // --
	tBOOL   bInitDone;        // --
	tDWORD  dwAccessMode;     // --
	tCHAR   szName[MDMAP_MAX_PATH]; // --
	tCHAR   szPath[MDMAP_MAX_PATH]; // --
	tDWORD  dwOpenMode;       // --
	tQWORD  qwObjectSize;     // --
	Region* pRegion;          // --
	tQWORD  qwTempOffset;     // --
	tDWORD  dwTempSize;       // --
	hIO     hTempIO;          // --
} MDMAP_Data;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	MDMAP_Data*        data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
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
tERROR pr_call IO_PROP_MDMAP_Prop_Get( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IO_PROP_MDMAP_Prop_Set( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IO". Static(shared) property table variables
const tVERSION vVersion = 2; // must be READ_ONLY at runtime
const tSTRING szComment = "MultiDMAP"; // must be READ_ONLY at runtime
const tBOOL bDelOnClose = cFALSE; // must be READ_ONLY at runtime
const tDWORD dwAvail = 0; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, vVersion, sizeof(vVersion) )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 10 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_ORIGIN, MDMAP_Data, dwOrigin, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, IO_PROP_MDMAP_Prop_Get, IO_PROP_MDMAP_Prop_Set )
	mLOCAL_PROPERTY_FN( pgOBJECT_PATH, IO_PROP_MDMAP_Prop_Get, IO_PROP_MDMAP_Prop_Set )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, IO_PROP_MDMAP_Prop_Get, NULL )
	mLOCAL_PROPERTY_FN( pgOBJECT_OPEN_MODE, IO_PROP_MDMAP_Prop_Get, IO_PROP_MDMAP_Prop_Set )
	mLOCAL_PROPERTY_FN( pgOBJECT_ACCESS_MODE, IO_PROP_MDMAP_Prop_Get, IO_PROP_MDMAP_Prop_Set )
	mSHARED_PROPERTY_PTR( pgOBJECT_DELETE_ON_CLOSE, bDelOnClose, sizeof(bDelOnClose) )
	mSHARED_PROPERTY_PTR( pgOBJECT_AVAILABILITY, dwAvail, sizeof(dwAvail) )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MDMAP_Data, hFatherOS, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOBJECT_FILL_CHAR, MDMAP_Data, byFillChar, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_FN( plMAP_RESET_DATA, NULL, IO_PROP_MDMAP_Prop_Set )
	mLOCAL_PROPERTY_BUF( plMAP_PROP_FATHER, MDMAP_Data, hPropObject, cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_FN( plMAP_AREA_ORIGIN, NULL, IO_PROP_MDMAP_Prop_Set )
	mLOCAL_PROPERTY_FN( plMAP_AREA_SIZE, NULL, IO_PROP_MDMAP_Prop_Set )
	mLOCAL_PROPERTY_FN( plMAP_AREA_START, NULL, IO_PROP_MDMAP_Prop_Set )
mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
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
		PID_MDMAP,                              // plugin identifier
		0x00000000,                             // subtype identifier
		vVersion,                               // interface version
		VID_ANDY,                               // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(MDMAP_Data),                     // memory size
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



// AVP Prague stamp begin( Property method implementation, MDMAP_Prop_Get )
// Interface "IO". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_MDMAP_Prop_Get( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
tERROR err;
MDMAP_Data* data;

	PR_TRACE_A0( _this, "Enter *GET* multyproperty method IO_PROP_MDMAP_Prop_Get" );

	err = errOK;
	data = _this->data;
	
	if ( buffer && size )
	{
		switch ( prop )
		{

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
		break;
			
		case pgOBJECT_OPEN_MODE:
			size= sizeof(tDWORD);
			*((tDWORD*)buffer)=(tDWORD)data->dwOpenMode;
			break;
			
		case pgOBJECT_ACCESS_MODE:
			size= sizeof(tDWORD);
			*((tDWORD*)buffer)=(tDWORD)data->dwAccessMode;
			break;
			
		default:
			*out_size = 0;
			break;
		}
	}
	else if ( !buffer && !size )
	{
		switch(prop)
		{
		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
			size = MDMAP_MAX_PATH;
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
	
	PR_TRACE_A2( _this, "Leave *GET* multyproperty method IO_PROP_MDMAP_Prop_Get, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	if(out_size)
		*out_size=size;
	return err;
	
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, MDMAP_Prop_Set )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_ACCESS_MODE
tERROR pr_call IO_PROP_MDMAP_Prop_Set( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
tERROR err;
MDMAP_Data* data;
Region* pRegion;
Region* pTmp;

	PR_TRACE_A0( _this, "Enter *SET* multyproperty method IO_PROP_MDMAP_Prop_Set" );
	err = errOK;
	data=_this->data;
	
	if ( buffer && size )
	{

		switch ( prop )
		{

			case pgOBJECT_FULL_NAME:
				err = errPROPERTY_RDONLY;
				size = 0;
				goto loc_exit;
			case pgOBJECT_NAME:
				if(size > sizeof(data->szName)-1)
				{
					err = errBUFFER_TOO_SMALL;
					size = MDMAP_MAX_PATH;
				}
				else
					memcpy(data->szName, buffer, size);
				break;
			case pgOBJECT_PATH:
				if(size > sizeof(data->szName)-1)
				{
					err = errBUFFER_TOO_SMALL;
					size = MDMAP_MAX_PATH;
				}
				else
					memcpy(data->szPath, buffer, size);
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
				
			case plMAP_AREA_ORIGIN:
				if(data->bInitDone == cFALSE)
				{
					data->hTempIO = *((hIO*)buffer);
					err = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&pRegion, sizeof(Region));
					if(PR_SUCC(err))
					{
					tQWORD qwTmp;

						data->qwObjectSize += data->dwTempSize;
						pRegion->dwSizeOfBlock = data->dwTempSize;
						pRegion->hFatherIo = data->hTempIO;
						pRegion->qwOffsetInFatherIo= data->qwTempOffset;
						pRegion->Next = NULL;
						size = sizeof(hIO);
						qwTmp = 0;
						if(data->pRegion == NULL)
						{
							data->pRegion = pRegion;
						}
						else
						{
							pTmp = data->pRegion;
							while(pTmp)
							{
								qwTmp += pTmp->dwSizeOfBlock;
								if(pTmp->Next == NULL)
								{
									pTmp->Next = (Region*)pRegion;
									break;
								}
								else
								{
									pTmp = pTmp->Next;
								}
							}
						}
						pRegion->qwOffsetInObject = qwTmp;
					}
					else
						goto loc_exit;
				}
				else
				{
					err = errPROPERTY_NOT_WRITABLE;
					size = 0;
					goto loc_exit;
				}
				break;
			case plMAP_AREA_SIZE:
				if(data->bInitDone == cFALSE)
				{
					data->dwTempSize = *((tDWORD*)buffer);
					size = sizeof(tDWORD);
				}
				else
				{
					err = errPROPERTY_NOT_WRITABLE;
					size = 0;
					goto loc_exit;
				}
				break;
			case plMAP_AREA_START:
				if(data->bInitDone == cFALSE)
				{
					data->qwTempOffset = *((tQWORD*)buffer);
					size = sizeof(tQWORD);
				}
				else
				{
					err = errPROPERTY_NOT_WRITABLE;
					size = 0;
					goto loc_exit;
				}
				break;
			case plMAP_RESET_DATA:
				data->bInitDone = cFALSE;
				data->qwObjectSize = 0;
				while(data->pRegion )
				{
					pRegion = data->pRegion;
					data->pRegion = (Region*)data->pRegion->Next;
					CALL_SYS_ObjHeapFree(_this, pRegion);
				}
				size = 0;
				break;
			case pgOBJECT_ACCESS_MODE:
				if((*((tDWORD*)buffer) & fACCESS_NO_CHANGE_MODE) == 0 || data->bInitDone != cTRUE)
				{
					data->dwAccessMode=*((tDWORD*)buffer);
					size=sizeof(tDWORD);
					// send message to notify creator that
					// access mode changed!
					err=CALL_SYS_SendMsg (_this, pmc_DMAP_IO, pm_DMAP_IO_AMODE_CHANGE, NULL, &buffer, &size);
					//just ignore this error...	
				}
				else
				{
					err = errPROPERTY_NOT_WRITABLE;
					size = 0;
					goto loc_exit;
				}
				break;
				
		}
	}
	else if ( !buffer && !size )
	{
		switch(prop)
		{
		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
			size = MDMAP_MAX_PATH;
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
		*out_size = size;
	
	PR_TRACE_A2( _this, "Leave *SET* multyproperty method IO_PROP_MDMAP_Prop_Set, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return err;
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
tERROR error;
	
	error=errOK;
	if(_this->data->pRegion == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		_this->data->bInitDone = cTRUE;
	
	_this->data->dwOrigin = CALL_SYS_PropertyGetDWord( _this, pgOBJECT_ORIGIN);
	
	// Set Default values
	if(_this->data->dwOrigin == 0)
		_this->data->dwOrigin = OID_GENERIC_IO;
	if(_this->data->dwAccessMode == 0)
		_this->data->dwAccessMode = fACCESS_READ | fACCESS_NO_EXTEND | fACCESS_NO_TRUNCATE | fACCESS_NO_BUFFERING | fACCESS_NO_CHANGE_MODE;
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekRead )
// Extended method comment
//    //eng:returns real count of bytes read
// Parameters are:
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
tERROR error;
tDWORD read;
MDMAP_Data* data;
tDWORD dwAlreadyRead;
Region* pRegion;
tDWORD dwOffsettInBlock;
tDWORD dwResult;

	PR_TRACE_A0( _this, "Enter IO::SeekRead method" );
	error = errNOT_IMPLEMENTED;
	data = _this->data;
	read = 0;

	if(result == NULL)
		result = &dwResult;

	dwAlreadyRead = 0;
	
	if ( offset < data->qwObjectSize )
	{
		pRegion = data->pRegion;
		while(size)
		{
			pRegion = GetRegion(pRegion, offset);
			if(pRegion == NULL)
			{
				error = errEOF;
				goto l_exit;
			}
			// прочитать или весь регион или ту часть, что надо
			dwOffsettInBlock = (tDWORD)((offset - pRegion->qwOffsetInObject) + pRegion->qwOffsetInFatherIo);

			read = ( (tDWORD)((offset - pRegion->qwOffsetInObject) + size) > pRegion->dwSizeOfBlock ) ? (tDWORD)(pRegion->dwSizeOfBlock - (offset - pRegion->qwOffsetInObject) ) : size;


			error = CALL_IO_SeekRead((hi_IO) pRegion->hFatherIo, result, dwOffsettInBlock  , &(((tBYTE*)buffer)[dwAlreadyRead]), read );
			dwAlreadyRead += *result;
			size -= *result;
			offset += *result;
			if(error != errOK)
			{
				if(error != errEOF)
				{
					dwAlreadyRead = 0;
				}
				goto l_exit;
			}
		}
	}
	else
	{
		error = errEOF;
	}
l_exit:
	*result = dwAlreadyRead;
	PR_TRACE_A2( _this, "Leave IO::SeekRead method, ret tDWORD = %u, error = 0x%08x", read, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SeekWrite )
// Extended method comment
//    writes buffer to the object. Returns real count of bytes written
// Parameters are:
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size )
{
	PR_TRACE_A0( _this, "Enter IO::SeekWrite method" );
	if ( result )
		*result = 0;
	PR_TRACE_A2( _this, "Leave IO::SeekWrite method, ret tDWORD = %u, error = 0x%08x", 0, errOBJECT_READ_ONLY );
	return errOBJECT_READ_ONLY;
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
	PR_TRACE_A0( _this, "Enter IO::GetSize method" );
	if ( result )
		*result = _this->data->qwObjectSize;
	PR_TRACE_A2( _this, "Leave IO::GetSize method, ret tQWORD = %I64u, error = 0x%08x", ret_val, errOK );
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSize )
// Parameters are:
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size )
{
	PR_TRACE_A0( _this, "Enter IO::SetSize method" );
	PR_TRACE_A1( _this, "Leave IO::SetSize method, ret tERROR = 0x%08x", errOBJECT_READ_ONLY );
	return errOBJECT_READ_ONLY;
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
	PR_TRACE_A0( _this, "Enter IO::Flush method" );
	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", errOBJECT_READ_ONLY );
	return errOBJECT_READ_ONLY;
}
// AVP Prague stamp end





__inline Region* GetRegion(Region* pRegion, tQWORD offset)
{

	while(pRegion)
	{
		if( pRegion->qwOffsetInObject + (tQWORD)pRegion->dwSizeOfBlock  > offset)
		{
			return pRegion;
		}
		pRegion = pRegion->Next;
	}
	return NULL;
}
