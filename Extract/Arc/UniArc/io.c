// -------- Monday,  23 October 2000,  15:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Unversal Extractor
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- io.c
// -------------------------------------------

#define PR_IMPEX_DEF


#define IO_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <Extract/plugin/p_miniarc.h>

#include "io.h"
#include "os.h"

#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_os.h>
#include <Prague/pr_pid.h>

#include <Extract/iface/i_minarc.h>

#include "../../Wrappers/Dmap/plugin_dmap.h"

#define  IMPEX_IMPORT_LIB
#include <AV/plugin/p_avlib.h> 
#undef   IMPEX_IMPORT_LIB

#include <string.h>

tERROR ioCheckForFileInTree(tQWORD qwObjectId, hTREE Tree, hIO* pIO);

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
// Interface IO. Inner data structure
typedef struct tag_IO_Data 
{
	tBOOL    bObjectWasChanged; // --
	tBOOL    bInitDone;         // --
	hMINIARC hArc;
	hIO      hUnpackedIO;
	tQWORD   qwNeedObject;
	tDWORD   dwOpenMode;
	tDWORD   dwAccessMode;
	tQWORD   qwSize;
	tBOOL    bIsReadOnly;
	hOS      hFatherOS;
    hObjPtr  hFatherObjPtr;
	tBOOL    bDeleteOnClose;
	tCHAR*   szName;
	tCHAR*   szFullName;
} IO_Data;
// AVP Prague stamp end( IO, Data structure )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, object declaration )
typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	IO_Data*          data;   // pointer to the "IO" data structure
} *hi_IO;
// AVP Prague stamp end( IO, object declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal method table prototypes )
// Interface "IO". Internal method table. Forward declarations
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectInitDone( hi_IO _this );
tERROR pr_call IO_ObjectPreClose( hi_IO _this );
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
	(tIntFnIFaceTransfer)    NULL,
};
// AVP Prague stamp end( IO, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, interface function forward declarations )
typedef   tERROR (pr_call *fnpIO_SeekRead)  ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SeekWrite) ( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ); // -- ;
typedef   tERROR (pr_call *fnpIO_SetSize)   ( hi_IO _this, tQWORD new_size );                    // -- ;
typedef   tERROR (pr_call *fnpIO_GetSize)   ( hi_IO _this, tQWORD* result, IO_SIZE_TYPE type );                  // -- returns requested size of the object;
typedef   tERROR (pr_call *fnpIO_Flush)     ( hi_IO _this );                                     // -- flush internal buffers;
// AVP Prague stamp end( IO, interface function forward declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
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

tERROR ioCopyFile(hi_IO Src, hi_IO Dest);
tERROR iGetSize(hi_IO _this, tQWORD *retval);

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Forwarded property methods declarations )
// Interface "IO". Get/Set property methods
tERROR pr_call IO_PropertyGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call IO_PropertySet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( IO, Forwarded property methods declarations )
// --------------------------------------------------------------------------------

#if defined (_WIN64)
	#define _CRT_SECURE_DEPRECATE_MEMORY
	#include <memory.h>
#elif defined (_WIN32)
	#if !defined(_INC_STRING)
		size_t __cdecl strlen(const tCHAR *);
		#if !defined(_INC_MEMORY)
			void* __cdecl memcpy(void*, const void*, size_t);
		#endif
	#endif
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Global property variable declaration )
// Interface "IO". Global(shared) property table variables
const tVERSION IO_Version = 2; // must be READ_ONLY at runtime
const tSTRING IO_Comment = "Universal Archiver (IO)"; // must be READ_ONLY at runtime
// AVP Prague stamp end( IO, Global property variable declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property table )
// Interface "IO". Property table
mPROPERTY_TABLE(IO_PropTable)
	mSHARED_PROPERTY_PTR ( pgINTERFACE_VERSION, IO_Version, sizeof(IO_Version) )
	mSHARED_PROPERTY_VAR ( pgINTERFACE_COMMENT, IO_Comment, 24 )
	mLOCAL_PROPERTY_FN ( pgOBJECT_AVAILABILITY, IO_PropertyGet, NULL)

	mLOCAL_PROPERTY_FN ( pgOBJECT_ORIGIN, IO_PropertyGet, NULL)
	mLOCAL_PROPERTY_FN ( pgOBJECT_COMPRESSED_SIZE, IO_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN ( pgOBJECT_NAME, IO_PropertyGet, NULL  )
	mLOCAL_PROPERTY_FN ( pgOBJECT_PATH, IO_PropertyGet, NULL  )
	mLOCAL_PROPERTY_FN ( pgOBJECT_FULL_NAME, IO_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN ( pgOBJECT_OPEN_MODE, IO_PropertyGet, IO_PropertySet )
	mLOCAL_PROPERTY_FN ( pgOBJECT_ACCESS_MODE, IO_PropertyGet, IO_PropertySet )
	mLOCAL_PROPERTY_FN ( pgOBJECT_FILL_CHAR, IO_PropertyGet, NULL )
    mLOCAL_PROPERTY_FN ( pgOBJECT_VOLUME_NAME, IO_PropertyGet, NULL )
	
    mLOCAL_PROPERTY_FN ( pgOBJECT_SIGNATURE, IO_PropertyGet, NULL )
    mLOCAL_PROPERTY_FN ( pgOBJECT_COMPRESSION_METHOD, IO_PropertyGet, NULL )
    mLOCAL_PROPERTY_FN ( pgOBJECT_PASSWORD_PROTECTED, IO_PropertyGet, NULL )
    
	mLOCAL_PROPERTY_BUF ( pgOBJECT_BASED_ON, IO_Data, hFatherOS, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF ( pgOBJECT_DELETE_ON_CLOSE, IO_Data, bDeleteOnClose, cPROP_BUFFER_WRITE | cPROP_BUFFER_READ)
	
	mLOCAL_PROPERTY_BUF ( ppOBJECT_TO_OPEN, IO_Data, qwNeedObject, cPROP_BUFFER_WRITE | cPROP_BUFFER_READ)	
	mLOCAL_PROPERTY_BUF ( ppMINI_ARCHIVER_OBJECT,  IO_Data, hArc, cPROP_BUFFER_WRITE)	

mPROPERTY_TABLE_END(IO_PropTable)
// AVP Prague stamp end( IO, Property table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Registration call )
// Interface "IO". Register function
tERROR pr_call IO_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"IO::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IO,                                 // interface identifier
		PID_UNIVERSAL_ARCHIVER,                 // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000002,                             // interface version
		VID_ANDY,                               // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(IO_Data),                        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( error != errOK )
			PR_TRACE( (root,prtDANGER,"IO(IID_IO) registered [error=%u]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"IO::Register\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( IO, Registration call )
// --------------------------------------------------------------------------------






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property method implementation )
// Interface "IO". Method "Get" for property(s):
//  -- pgOBJECT_FULL_NAME
//  -- pgOBJECT_OPEN_MODE
//  -- pgOBJECT_SIZE
//  -- pgOBJECT_NAME
//  -- pgOBJECT_PATH
//  -- pgOBJECT_ATTRIBUTES
//  -- pgOBJECT_ORIGIN

tERROR pr_call IO_PropertyGet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tUINT size1;
tERROR error;
tUINT ns;

	PR_TRACE_A0( _this, "Enter *GET* multyproperty method \"IO_PropertyGet\"" );

	switch ( prop ) 
	{
		case pgOBJECT_FULL_NAME :
			if(buffer == NULL || size == 0)
			{
				size = _toui32(strlen( _this->data->szFullName ) + 1);
				break;
			}
			if(_this->data->szFullName)
			{
				ns = _toui32(strlen( _this->data->szFullName ));
				
				if ( ns+1 > size )
				{
					error = errBUFFER_TOO_SMALL;
					size = ns + 1;
				}
				else
				{
					memcpy( buffer, _this->data->szFullName, ns+1 );
					size = ns + 1;
				}
			}
			else
			{
				size1 = sizeof(tQWORD);
				CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwNeedObject, size1);
				return CALL_SYS_PropertyGetStr(_this->data->hArc, out_size, prop, buffer, size, cCP_ANSI);
			}
			break;
		case pgOBJECT_NAME :
			if(buffer == NULL || size == 0)
			{
				size = _toui32(strlen( _this->data->szName ) + 1);
				break;
			}
			if(_this->data->szName)
			{
				ns = _toui32(strlen( _this->data->szName ));
				
				if ( ns+1 > size )
				{
					error = errBUFFER_TOO_SMALL;
					size = ns+1;
				}
				else
				{
					memcpy( buffer, _this->data->szName, ns+1 );
					size = ns + 1;
				}
			}
			else
			{
				size1 = sizeof(tQWORD);
				CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwNeedObject, size1);
				return CALL_SYS_PropertyGetStr(_this->data->hArc, out_size, prop, buffer, size, cCP_ANSI);
			}
			break;
		case pgOBJECT_SIZE_Q:
			size = sizeof(tQWORD);
			if ( !buffer && !size ) 
			{
				break;
			}
			if(size < sizeof(tQWORD))
			{
				error = errBUFFER_TOO_SMALL;
				break;
			}
			error = iGetSize(_this,((tQWORD*)buffer));
			break;
		case pgOBJECT_OPEN_MODE :
		case pgOBJECT_ACCESS_MODE :
		case pgOBJECT_PATH :
		case pgOBJECT_ORIGIN :
		case pgOBJECT_COMPRESSED_SIZE:
		case pgOBJECT_FILL_CHAR:
		case pgOBJECT_AVAILABILITY:
        case pgOBJECT_VOLUME_NAME:
        case pgOBJECT_SIGNATURE:
        case pgOBJECT_COMPRESSION_METHOD:
        case pgOBJECT_PASSWORD_PROTECTED:
			size1 = sizeof(tQWORD);
			error = CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwNeedObject, size1);
			return CALL_SYS_PropertyGet(_this->data->hArc, out_size, prop, buffer, size);

		default :
			size = 0;
			break;
	}

	if(out_size )
		*out_size = size;

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method \"IO_PropertyGet\", ret tUINT = %u(size), error = %u", *out_size, errOK );
	return errOK;
}
// AVP Prague stamp end( IO, Property method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Property method implementation )
// Interface "IO". Method "Set" for property(s):
//  -- OBJECT_OPEN_MODE
//  -- OBJECT_NEW
tERROR pr_call IO_PropertySet( hi_IO _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error;
IO_Data* data;
hIO io;
tDWORD size1;

	PR_TRACE_A0( _this, "Enter *SET* multyproperty method \"IO_PropertySet\"" );
	error = errOK;
		data = _this->data;

	if ( buffer && size )
	{
		switch ( prop ) 
		{
			case pgOBJECT_ACCESS_MODE:
				if(!(data->dwAccessMode & fACCESS_NO_CHANGE_MODE) || !data->bInitDone )
				{
					if(data->bIsReadOnly && *((tDWORD*)buffer) & fACCESS_WRITE )
					{
						size = 0;
						return errOBJECT_READ_ONLY;
					}

                    if((data->dwAccessMode & fACCESS_WRITE) != fACCESS_WRITE && (*((tDWORD*)buffer) & fACCESS_WRITE) == fACCESS_WRITE)
					{
					    error = CALL_SYS_SendMsg( (hOBJECT)data->hFatherOS, msg_clsBROADCAST, UNI_MSG_FILE_RW, _this, 0, 0 );
                        if(PR_FAIL(error))
                        {
                            size = 0;
                            goto loc_exit;
                        }
                    }

// TODO memory 
					if(data->szFullName == NULL)
					{
						//size1 = sizeof(tQWORD);
						error = CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwNeedObject, sizeof(tQWORD));

						CALL_SYS_PropertyGetStr(data->hArc, &size1, pgOBJECT_FULL_NAME, NULL, 0, cCP_ANSI);
						if(size1 != 0)
						{
							CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->szFullName, size1 + 16);
							//size2 = sizeof(tQWORD);
							error = CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwNeedObject, sizeof(tQWORD));
							error = CALL_SYS_PropertyGetStr(data->hArc, NULL, pgOBJECT_FULL_NAME, data->szFullName, size1 + 16, cCP_ANSI);
						}
					}

					if(ioCheckForFileInTree(data->qwNeedObject, ((hi_OS)data->hFatherOS)->data->hOpenTree, &io) != errOK)
					{
						if(io)
						{
						tDWORD dwOpenedAttrib;

							dwOpenedAttrib = CALL_SYS_PropertyGetDWord( io, pgOBJECT_OPEN_MODE);

// if file opened for writing -- fail this request
// if file opened for reading only and user wants to open for writing -- fail
							if((data->dwAccessMode & fACCESS_WRITE) == fACCESS_WRITE || (*((tDWORD*)buffer) & fACCESS_WRITE) == fACCESS_WRITE)
							{
								size = 0;
								error = errACCESS_DENIED;
								goto loc_exit;
							}
						}
					}
					if(data->hUnpackedIO && DMAP_ID==(tPID)CALL_SYS_PropertyGetDWord((hOBJECT)data->hUnpackedIO, pgPLUGIN_ID) && (*((tDWORD*)buffer) & fACCESS_WRITE))
					{
						error=(tERROR)CALL_SYS_ObjectCreate(_this,(hOBJECT*)&io, IID_IO, PID_TMPFILE,0);
						if(PR_SUCC(error))
						{
							CALL_SYS_PropertySetDWord( (hOBJECT)io, pgOBJECT_ACCESS_MODE,fACCESS_WRITE);
							CALL_SYS_PropertySetDWord( (hOBJECT)io, pgOBJECT_OPEN_MODE,fOMODE_OPEN_IF_EXIST + fOMODE_CREATE_IF_NOT_EXIST + fOMODE_TRUNCATE + 
								fOMODE_SHARE_DENY_READ);
							error = CALL_SYS_ObjectCreateDone(io);
						}
						if(PR_SUCC(error))
						{
							if(errOK == ioCopyFile(_this, (hi_IO)io))
								CALL_SYS_ObjectSwap(_this, io,(hOBJECT)data->hUnpackedIO);
//							CALL_SYS_ObjectClose(io);
						}
						else
						{
							size=0;
							goto loc_exit;
						}
					}
					error = CALL_SYS_PropertySetDWord(data->hArc, prop, *((tDWORD*)buffer));
                    if(PR_SUCC(error))
                    {
					    data->dwAccessMode = *((tDWORD*)buffer);
                    }
					size = sizeof(tDWORD);
				}
				else
				{
					size = sizeof(tDWORD);
					error = errACCESS_DENIED;
					goto loc_exit;
				}
				break;

      case pgOBJECT_OPEN_MODE:
        size = 0;
        break;

			default :
				size = 0;
				break;
		}
	}
	else if ( !buffer && !size ) 
	{
		switch(prop)
		{
			case pgOBJECT_ACCESS_MODE:
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

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method \"IO_PropertySet\", ret tUINT = %u(size), error = %u", *out_size, error );
	return error;
}
// AVP Prague stamp end( IO, Property method implementation )
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
tERROR pr_call IO_ObjectInit( hi_IO _this ) 
{
	PR_TRACE_A0( _this, "Enter \"IO::ObjectInit\" method" );
	_this->data->hFatherOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this, IID_OS);
    _this->data->hFatherObjPtr = (hObjPtr)CALL_SYS_ParentGet((hOBJECT)_this, IID_OBJPTR);
    _this->data->qwSize = 0xFFFFFFFFFFFFFFFF;
	
	PR_TRACE_A0( _this, "Leave \"IO::ObjectInit\" method, " );
	return errOK;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
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
IO_Data* data;
tDWORD size;

	PR_TRACE_A0( _this, "Enter \"IO::ObjectInitDone\" method" );
	error = errPARAMETER_INVALID;
	data=_this->data;

	if(data->hArc)
	{
		data->bObjectWasChanged=cFALSE;
		data->bInitDone = cTRUE;

		//if(data->hUnpackedIO)
		//	CALL_IO_GetSize((hi_IO)data->hUnpackedIO, &data->qwSize, IO_SIZE_TYPE_EXPLICIT);
		data->bIsReadOnly = CALL_SYS_PropertyGetDWord(data->hArc, pgIS_READONLY);
		
		size = sizeof(tQWORD);
		error = CALL_SYS_PropertySet(data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &data->qwNeedObject, size);
		
		CALL_SYS_PropertyGetStr(data->hArc, &size, pgOBJECT_NAME, NULL, 0, cCP_ANSI);
		if(size != 0)
		{
			CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->szName, size+16);
			//size1 = sizeof(tQWORD);
			error = CALL_SYS_PropertySet(data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &data->qwNeedObject, sizeof(tQWORD));
			error = CALL_SYS_PropertyGetStr(data->hArc, &size, pgOBJECT_NAME, data->szName, size + 16, cCP_ANSI);
		}
		
		if(data->szFullName == NULL)
		{
			//size = sizeof(tQWORD);
			error = CALL_SYS_PropertySet(data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &data->qwNeedObject, sizeof(tQWORD));
			
			CALL_SYS_PropertyGetStr(data->hArc, &size, pgOBJECT_FULL_NAME, NULL, 0, cCP_ANSI);
			if(size != 0)
			{
				CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->szFullName, size+16);
				//size1 = sizeof(tQWORD);
				error = CALL_SYS_PropertySet(data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &data->qwNeedObject, sizeof(tQWORD));
				error = CALL_SYS_PropertyGetStr(data->hArc, &size, pgOBJECT_FULL_NAME, data->szFullName, size + 16, cCP_ANSI);
			}
		}

        if (PR_SUCC(error))
        {
		    error = CALL_MiniArchiver_ObjCreate( data->hArc, &data->hUnpackedIO, data->qwNeedObject, (hIO)_this );
        }

        if (error == errOBJECT_PASSWORD_PROTECTED)
        {
			data->hUnpackedIO = NULL;

			error = AVLIB_PswHeuristic(data->hFatherOS, data->hFatherObjPtr, &data->hUnpackedIO);

			if(!data->hUnpackedIO)
            {
				error=errOBJECT_PASSWORD_PROTECTED;
            }
			else
			{
				CALL_SYS_SendMsg( _this, pmc_IO, pm_IO_PASSWORD_EXE_HEURISTIC, 0, 0, 0 );
				CALL_SYS_PropertySetDWord(_this, pgOBJECT_ORIGIN, CALL_SYS_PropertyGetDWord(data->hUnpackedIO, pgOBJECT_ORIGIN));
			}
        }

//		data->hFatherOS = (hOS)CALL_SYS_ParentGet((hOBJECT)_this, IID_OS);
	}

	PR_TRACE_A1( _this, "Leave \"IO::ObjectInitDone\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Internal (kernel called) interface method implementation )
// --- Interface "IO". Method "ObjectPreClose"
// Extended method comment
//   Kernel warns object it is going to close all children
// Behaviour comment
//   Object takes all necessary "before preclosing" actions
// Result comment
//   
tERROR pr_call IO_ObjectPreClose( hi_IO _this ) 
{
tDWORD dwMsg;
tERROR error;
IO_Data* data;
tBOOL bSolid;

	PR_TRACE_A0( _this, "Enter \"IO::ObjectPreClose\" method" );

	data = ((hi_IO)_this)->data;

	CALL_SYS_PropertyGet(_this, NULL, psOBJECT_OPERATIONAL, &bSolid, sizeof(bSolid));
	if(bSolid == cFALSE)
	{
		if(data->szFullName)
		{
			CALL_SYS_ObjHeapFree(_this, data->szFullName);
			data->szFullName = NULL;
		}
		if(data->szName)
		{
			CALL_SYS_ObjHeapFree(_this, data->szName);
			data->szName = NULL;
		}
        /*
        This io should not be allocated by MiniArchiver by this point

		if(data->hUnpackedIO)
		{
			CALL_SYS_ObjectClose(data->hUnpackedIO);
			data->hUnpackedIO = NULL;
		}
        */

		return errOK;
	}
	
	bSolid = cFALSE;
	CALL_SYS_PropertyGet(_this->data->hArc, NULL, pgIS_SOLID, &bSolid, sizeof(bSolid));


//	hZipOs=(hOS)CALL_SYS_ParentGet(_this, IID_OS);
//	if(hZipOs)
	{
/*		if(!data->bObjectWasChanged && bSolid)
		{
//			CALL_SYS_ParentSet(_this, NULL, (hOBJECT)hZipOs);
			CALL_SYS_ParentSet(_this, NULL, (hOBJECT)data->hFatherOS);
			dwMsg = UNI_MSG_FILE_CLOSE_KEEP_OPEN;
		}
		else*/ if(data->bObjectWasChanged)
		{
			CALL_SYS_ParentSet(_this, NULL, (hOBJECT)data->hFatherOS);
			dwMsg = UNI_MSG_FILE_CLOSE_CHANGE;
			data->bObjectWasChanged = cFALSE;
		}
		else
		{
			dwMsg = UNI_MSG_FILE_CLOSE;
		}
		if(data->bDeleteOnClose)
		{
			dwMsg = UNI_MSG_FILE_DELETE_ON_CLOSE;
		}
		error = CALL_SYS_SendMsg( (hOBJECT)data->hFatherOS, msg_clsBROADCAST, dwMsg, _this, 0, 0 );

		if (data->bDeleteOnClose) 
		{
		tDWORD del_err;
		tDWORD size = sizeof(del_err);
			
			if ( PR_SUCC(error)) 
			{
				del_err = errOK;
				dwMsg = pm_IO_DELETE_ON_CLOSE_SUCCEED;
				PR_TRACE(( _this, prtSPAM, "Object deleted on close"));
			}
			else 
			{
				del_err = error;
				dwMsg = pm_IO_DELETE_ON_CLOSE_FAILED;
				//error = errIO_DELETE_ON_CLOSE_FAILED;
				error = errOK;
				PR_TRACE(( _this, prtSPAM, "Object cannot be deleted. Error is -- %d", error ));
			}
            data->bDeleteOnClose = cFALSE;

			CALL_SYS_SendMsg( _this, pmc_IO, dwMsg, 0, &del_err, &size );
		}
	}

	if(dwMsg == UNI_MSG_FILE_CLOSE)
	{
		if(data->szFullName)
		{
			CALL_SYS_ObjHeapFree(_this, data->szFullName);
			data->szFullName = NULL;
		}
		if(data->szName)
		{
			CALL_SYS_ObjHeapFree(_this, data->szName);
			data->szName = NULL;
		}
		if(data->hUnpackedIO)
		{
			CALL_SYS_ObjectClose(data->hUnpackedIO);
			data->hUnpackedIO = NULL;
		}
	}
	PR_TRACE_A1( _this, "Leave \"IO::ObjectPreClose\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( IO, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekRead"
tERROR pr_call IO_SeekRead( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) 
{
tERROR error;
tDWORD ret_val;

	PR_TRACE_A0( _this, "Enter \"IO::SeekRead\" method" );
	ret_val = 0;

    if (!buffer)
    {
        if (result)
            *result = 0;
        return errPARAMETER_INVALID;
    }
    if (size == 0)
    {
        if (result)
            *result = 0;
        return errPARAMETER_INVALID;
    }

	if(_this->data->hUnpackedIO)
		error = CALL_IO_SeekRead((hi_IO)_this->data->hUnpackedIO ,&ret_val, offset, buffer, size);
	else
		error = errOBJECT_NOT_CREATED;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"IO::SeekRead\" method, ret tDWORD = %u, error = %u", ret_val, error );
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




 
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "SeekWrite"
tERROR pr_call IO_SeekWrite( hi_IO _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) 
{
tERROR error ;
IO_Data* data;
tDWORD write;
tQWORD iosize;

	PR_TRACE_A0( _this, "Enter \"IO::SeekWrite\" method" );
	data=_this->data;
	error = errOBJECT_NOT_CREATED;
	write = 0;
	if (!buffer)
    {
        if (result)
            *result = 0;
        return errPARAMETER_INVALID;
    }
    if (size == 0)
    {
        if (result)
            *result = 0;
        return errPARAMETER_INVALID;
    }

	if((data->dwAccessMode & fACCESS_WRITE ) != fACCESS_WRITE || data->bIsReadOnly == cTRUE)
	{
        if (result)
		    *result = 0;
		return errOBJECT_READ_ONLY;
	}
	if(data->hUnpackedIO)
	{

        error = iGetSize(_this,&iosize);
        if (PR_FAIL(error))
        {
            if (result)
                *result = 0;
		    return error;
        }

        if (data->dwAccessMode & fACCESS_NO_EXTEND)
        {
            if (offset+size>iosize)
            {
                if (result)
				    *result = 0;
				return errOBJECT_RESIZE;
            }
        }
        else if (data->dwAccessMode & fACCESS_NO_EXTEND_RAND)
        {
            if (offset>iosize)
            {
                if (result)
				    *result = 0;
				return errOBJECT_RESIZE;
            }
        }

        if (offset+size > iosize)
        {
            error = IO_SetSize(_this, (tQWORD)(offset+size));
            if (PR_FAIL(error))
            {
                if (result)
                    *result = 0;
                return error;
            }
        }

/*
		if(!(data->dwAccessMode & fACCESS_NO_EXTEND))
		{
            error = iGetSize(_this,&iosize);
            if (PR_FAIL(error))
            {
                *result = 0;
                goto loc_exit;
            }
			if(offset<=iosize)
			{
				if(offset+size>iosize)
				{
					if(errOK != IO_SetSize(_this, (tQWORD)(offset+size)))
					{
						*result = 0;
						error=errOBJECT_RESIZE;
						goto loc_exit;
					}
				}
			}
			else
			{
				*result = 0;
				error=errOBJECT_SEEK;
				goto loc_exit;
			}
		}
		if(!(data->dwAccessMode & fACCESS_NO_EXTEND_RAND))
		{
            error = iGetSize(_this,&iosize);
            if (PR_FAIL(error))
            {
                *result = 0;
                goto loc_exit;
            }
			if(offset+size>iosize)
			{
				if(errOK != IO_SetSize(_this, offset+size))
				{
					*result = 0;
					error=errOBJECT_RESIZE;
					goto loc_exit;
				}
			}
		}
*/
		error = CALL_IO_SeekWrite((hi_IO)data->hUnpackedIO, result, offset, buffer, size);
		if(errOK == error)
			data->bObjectWasChanged = cTRUE;
	}
/* loc_exit: */
	PR_TRACE_A2( _this, "Leave \"IO::SeekWrite\" method, ret tDWORD = %u, error = %u", write, error );
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Resize"
tERROR pr_call IO_SetSize( hi_IO _this, tQWORD new_size ) 
{
tERROR error;
IO_Data* data;

	PR_TRACE_A0( _this, "Enter \"IO::Resize\" method" );
	data=_this->data;
	error = errOBJECT_NOT_CREATED;

	if((data->dwAccessMode & fACCESS_WRITE ) != fACCESS_WRITE || data->bIsReadOnly == cTRUE)
	{
		error = errOBJECT_READ_ONLY;
	}
	else if(data->hUnpackedIO)
	{
		error = CALL_IO_SetSize((hi_IO)data->hUnpackedIO, new_size);
		if(error == errOK)
		{
			data->qwSize = new_size;
			data->bObjectWasChanged = cTRUE;
		}
	}
	
	PR_TRACE_A2( _this, "Leave \"IO::Resize\" method, ret tDWORD = %u, error = %u", new_size, error );
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
        return iGetSize(_this,result);
	}
	return errPARAMETER_INVALID;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, External (user called) interface method implementation )
// --- Interface "IO". Method "Flush"
// Extended method comment
//   Flush internal buffers.
// Behaviour comment
//   Flushes internal buffers. Must return errOK if object opened in RO.
tERROR pr_call IO_Flush( hi_IO _this ) 
{
tERROR error ;

	PR_TRACE_A0( _this, "Enter \"IO::Flush\" method" );

	if(_this->data->hUnpackedIO)
		error = CALL_IO_Flush((hi_IO)_this->data->hUnpackedIO);
	else
		error = errOBJECT_NOT_CREATED;
	
	PR_TRACE_A1( _this, "Leave \"IO::Flush\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( IO, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



tERROR ioCopyFile(hi_IO Src, hi_IO Dest)
{
tDWORD read;
tERROR error;
tDWORD dwPos;
///hHEAP hHeap;
tBYTE* Buffer;
#define cBufferSize 0x1000

//	error=errOBJECT_NOT_CREATED;

//	error=(tERROR)CALL_SYS_ObjectCreate((hOBJECT)Src,(hOBJECT*)&hHeap, IID_HEAP, PID_ANY,0);
//	if(PR_SUCC(error))
//	{
//		CALL_SYS_PropertySetDWord(hHeap, pgHEAP_GRANULARITY, cBufferSize+0x100);
//		if(errOK == CALL_SYS_ObjectCreateDone(hHeap));
//		{
			error = errOK;
//			if(errOK != CALL_Heap_Alloc(hHeap,&Buffer, cBufferSize))
			error = CALL_SYS_ObjHeapAlloc(Src, (tPTR*)&Buffer, cBufferSize);
			if(PR_FAIL(error))
			{
				error = errNOT_ENOUGH_MEMORY;
//				CALL_SYS_ObjectClose(hHeap);
			}
//		}
//	}

	if(PR_SUCC(error))
	{
		error = CALL_IO_SeekRead(Src, &read ,0, Buffer,cBufferSize);
		dwPos = 0;
		while(read != 0 )
		{
			error = CALL_IO_SeekWrite(Dest,NULL, dwPos, Buffer, read);
			if(error != errOK)
				break;
			dwPos += read;
			CALL_IO_SeekRead(Src,&read, dwPos, Buffer,cBufferSize);
		}
		//CALL_SYS_ObjectClose(hHeap);
		CALL_SYS_ObjHeapFree(Src, Buffer);
	}
	return error;
}


// TODO implement it!!!
tERROR ioCheckForFileInTree(tQWORD qwObjectId, hTREE Tree, hIO* pIO)
{
	*pIO = NULL;
	return errOK;
}

tERROR iGetSize(hi_IO _this, tQWORD *retval)
{
    tERROR error = errOK;
    if (_this->data->qwSize == LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF))
    {
		if(_this->data->hUnpackedIO)
        {
            //the long way to do it:
            error = CALL_IO_GetSize((hi_IO)_this->data->hUnpackedIO, &_this->data->qwSize, IO_SIZE_TYPE_EXPLICIT);

            //the sort way
            /*
            error = CALL_SYS_PropertySet(_this->data->hArc, NULL, pgMINIARC_PROP_OBJECT_ID, &_this->data->qwNeedObject, sizeof(tQWORD));
            if (PR_SUCC(error))
            {
                error = CALL_SYS_PropertyGet(_this->data->hArc, NULL, pgOBJECT_SIZE_Q, &_this->data->qwSize, sizeof(tQWORD));
            }
            */
        }
        else
        {
            error = errOBJECT_NOT_CREATED;
        }
    }
    *retval = _this->data->qwSize;
    return error;
}
