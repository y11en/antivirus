// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  30 October 2002,  14:24 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Test
// Sub project -- Not defined
// Purpose     -- making images form io
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



//+ ----------------------------------------------------------------------------------------
#define _BUFFER_SIZE	4096 * 4
#ifdef _DEBUG
#define DbgBreakPoint(obj) {PR_TRACE((obj, prtERROR, "Break: ""__FUNCTION__"", on %d", __LINE__)); _asm int 3}
#else
#define DbgBreakPoint(obj)
#endif

#include "../Dmap/plugin_dmap.h"

#include "memory.h"

typedef struct tag_UniProp
{
	tPROPID m_PropID;
	tDWORD  m_PropSize; // --
	tBYTE	m_pData[0];
} UniProp, *PUniProp;

//+ ----------------------------------------------------------------------------------------
/*
tQWORD		Tag
tDWORD		CommentLen
tWSTRING	CommentString
//tDATETIME	DateTime

  tDWORD		PropsCount
  ... propertys
  tPROPID		PropID
  tDWORD		PropSize
  ... end propertys
  
	//tDWORD		dwReserved1
	//tDWORD		dwReserved2
	//tDWORD		dwReserved3
	//tDWORD		dwReserved4
	
	  tBYTE		Data
*/
//+ ----------------------------------------------------------------------------------------


//+ ----------------------------------------------------------------------------------------


// AVP Prague stamp begin( Interface comment,  )
// IO interface implementation
// Short comments -- input/output interface
//    Defines behavior of input/output of an object
//    Important -- It is supposed several clients can use single IO object simultaneously. It has no internal current position.
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface IO. Inner data structure

typedef struct tag_IOPrivate 
{
	hIO m_hIO; // --
	hSTRING m_hComment; // --
	hTREE   m_hTree;    // --
} IOPrivate;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IO 
{
	const iIOVtbl*     vtbl; // pointer to the "IO" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	IOPrivate*         data; // pointer to the "IO" data structure
} *hi_IO;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IO_ObjectInit( hi_IO _this );
tERROR pr_call IO_ObjectPreClose( hi_IO _this );
tERROR pr_call IO_MsgReceive( hi_IO _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IO_vtbl = 
{
	(tIntFnRecognize)        NULL,
		(tIntFnObjectNew)        NULL,
		(tIntFnObjectInit)       IO_ObjectInit,
		(tIntFnObjectInitDone)   NULL,
		(tIntFnObjectCheck)      NULL,
		(tIntFnObjectPreClose)   IO_ObjectPreClose,
		(tIntFnObjectClose)      NULL,
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
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define IO_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
	mPROPERTY_TABLE(IO_PropTable)
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_HASH, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FILL_CHAR, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_BASED_ON, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_AVAILABILITY, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_DELETE_ON_CLOSE, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_ACCESS_MODE, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_OPEN_MODE, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FULL_NAME, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_ORIGIN, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "substed io", 11 )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_CODEPAGE, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgINTERFACE_VERSION, IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
	mLAST_CALL_PROPERTY( IO_PROP_CommonPropRead, IO_PROP_CommonPropWrite )
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
		PID_SUBSTIO,                            // plugin identifier
		0x00000000,                             // subtype identifier
		IO_VERSION,                             // interface version
		VID_SOBKO,                              // interface developer
		&i_IO_vtbl,                             // internal(kernel called) function table
		(sizeof(i_IO_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_IO_vtbl,                             // external function table
		(sizeof(e_IO_vtbl)/sizeof(tPTR)),       // external function table size
		IO_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(IO_PropTable),     // property table size
		sizeof(IOPrivate),                      // memory size
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



// AVP Prague stamp begin( Property method implementation, CommonPropRead )
// Interface "IO". Method "Get" for property(s):
//  -- INTERFACE_VERSION
tERROR pr_call IO_PROP_CommonPropRead( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method IO_PROP_CommonPropRead for property pgINTERFACE_VERSION" );

	*out_size = 0;	
	{
		tTREENODE tNode;
		if (PR_FAIL(CALL_Tree_Search(_this->data->m_hTree, &tNode, &prop, sizeof(prop))))
			error = errPROPERTY_NOT_FOUND;
		else
		{
			tUINT tmpsize = 0;
			PUniProp pUni = NULL;
			
			error = errNOT_OK;
			
			if (PR_SUCC(CALL_Tree_NodeDataGet(_this->data->m_hTree, &tmpsize, tNode, 0, tmpsize)))
			{
				if (PR_SUCC(CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&pUni, tmpsize)))
				{
					if (PR_SUCC(CALL_Tree_NodeDataGet(_this->data->m_hTree, &tmpsize, tNode, pUni, tmpsize)))
					{
						*out_size = pUni->m_PropSize;
						if ((size != 0) && (size <= pUni->m_PropSize))
							memcpy(buffer, pUni->m_pData, pUni->m_PropSize);
						
						error = errOK;
					}
					CALL_SYS_ObjHeapFree(_this, pUni);
				}
			}
			
		}
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method IO_PROP_CommonPropRead for property pgINTERFACE_VERSION, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, CommonPropWrite )
// Interface "IO". Method "Set" for property(s):
//  -- INTERFACE_VERSION
tERROR pr_call IO_PROP_CommonPropWrite( hi_IO _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method IO_PROP_CommonPropWrite for property pgINTERFACE_VERSION" );

	*out_size = 0;
	{
		if (PR_FAIL(CALL_Tree_Search(_this->data->m_hTree, NULL, &prop, sizeof(prop))))
		{
			PUniProp pUni = NULL;
			if (PR_FAIL(CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&pUni, sizeof(UniProp) + size)))
				error = errNOT_ENOUGH_MEMORY;
			else
			{
				tTREENODE tNode;
				pUni->m_PropID = prop;
				pUni->m_PropSize = size;
				memcpy(pUni->m_pData, buffer, size);
				
				error = CALL_Tree_Add(_this->data->m_hTree, &tNode, pUni, sizeof(UniProp) + size);
				
				CALL_SYS_ObjHeapFree(_this, pUni);
				if (PR_FAIL(error))
					*out_size = size;
			}
		}
		/*else
			DbgBreakPoint(_this);*/
	}
	
	PR_TRACE_A2( _this, "Leave *SET* method IO_PROP_CommonPropWrite for property pgINTERFACE_VERSION, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
	_this->data->m_hIO = NULL;
	_this->data->m_hComment = NULL;
	
	if (PR_FAIL(CALL_SYS_ObjectCreateQuick(_this, &_this->data->m_hTree, IID_TREE, PID_ANY, SUBTYPE_ANY)))
		error = errOBJECT_CANNOT_BE_INITIALIZED;
	
	CALL_SYS_RegisterMsgHandler(_this, pmc_SUBST_IO, rmhDECIDER, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	
	PR_TRACE_A1( _this, "Leave IO::ObjectInit method, ret tERROR = 0x%08x", error );
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
	PR_TRACE_A0( _this, "Enter IO::ObjectPreClose method" );
	
	// Place your code here
	if (_this->data->m_hIO != NULL)
	{
		CALL_SYS_ObjectClose(_this->data->m_hIO);
		_this->data->m_hIO = NULL;
	}
	
	if (_this->data->m_hComment != NULL)
	{
		CALL_SYS_ObjectClose(_this->data->m_hComment);
		_this->data->m_hComment = NULL;
	}
	
	if (_this->data->m_hTree != NULL)
	{
		CALL_SYS_ObjectClose(_this->data->m_hTree);
		_this->data->m_hTree = NULL;
	}
	
	
	PR_TRACE_A1( _this, "Leave IO::ObjectPreClose method, ret tERROR = 0x%08x", error );
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
tERROR pr_call IO_MsgReceive( hi_IO _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter IO::MsgReceive method" );
	
	// Place your code here
	error = errOK;
	if (msg_cls_id == pmc_SUBST_IO)
	{
		switch (msg_id)
		{
		case pm_SUBST_IO_SAVE_CONTEXT:
			if ((par_buf ==NULL) || (par_buf_len == NULL) || (*par_buf_len != sizeof(tERROR)))
				error = errPARAMETER_INVALID;
			else
			{
				if (PR_FAIL(CALL_SYS_ObjectCheck(send_point, send_point, IID_IO, PID_ANY, 0, cFALSE)) ||
					PR_FAIL(CALL_SYS_ObjectCheck(ctx, ctx, IID_IO, PID_ANY, 0, cFALSE)))
					*(tERROR*)par_buf = errPARAMETER_INVALID;
				else
				{
					tQWORD offset, header_offset, prop_offset;
					tDWORD dwRead;
					tDWORD dwWrite;
					tPTR buffer = NULL;
					
					// if dmap -> взять проперти у родителя...у любого io которое не dmap :)
					hOBJECT hPropHolder = ctx;
					
					if (PR_SUCC(CALL_SYS_ObjectCheck(hPropHolder, hPropHolder, IID_IO, DMAP_ID, 0, cFALSE)))
					{
						do
						{
							hPropHolder = CALL_SYS_ParentGet(hPropHolder, IID_IO);
							if (hPropHolder != NULL)
							{
								if (PR_FAIL(CALL_SYS_ObjectCheck(hPropHolder, hPropHolder, IID_IO, DMAP_ID, 0, cFALSE)))
									break;
							}
							
						} while (hPropHolder != NULL);
					}
					if (hPropHolder == NULL)
						*(tERROR*)par_buf = errPARAMETER_INVALID;
					else
					{
						tQWORD Tag = _SUBST_TAG;
						tDWORD comment_len = 0;
						tBOOL tHeaderErr = cFALSE;
						*(tERROR*)par_buf = errOK;
						
						header_offset = sizeof(Tag);
						
						if (PR_FAIL(CALL_IO_SeekWrite((hi_IO)send_point, NULL, 0, &Tag, sizeof(Tag))))
							tHeaderErr = cTRUE;
						else
						{
							if (_this->data->m_hComment != NULL)
							{
								if (PR_FAIL(CALL_String_Length(_this->data->m_hComment, &comment_len)))
									tHeaderErr = cTRUE;
								else
								{
									if (PR_FAIL(CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, &comment_len, sizeof(comment_len))))
										tHeaderErr = cTRUE;
									else
									{
										tPTR ptr = NULL;
										CALL_SYS_ObjHeapAlloc(_this, &ptr, comment_len);
										if (ptr == NULL)
											tHeaderErr = cTRUE;
										else
										{
											CALL_String_ExportToBuff(_this->data->m_hComment, NULL, cSTRING_WHOLE, ptr, comment_len, cCP_ANSI, cSTRING_CONTENT_ONLY);
											if (PR_FAIL(CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset + sizeof(tDWORD), ptr, comment_len)))
												tHeaderErr = cTRUE;
											
											CALL_SYS_ObjHeapFree(_this, ptr);
										}
									}
								}
							}
							else
							{
								if (PR_FAIL(CALL_IO_SeekWrite((hi_IO)send_point, NULL, 0, &comment_len, sizeof(comment_len))))
									tHeaderErr = cTRUE;
							}
							
							header_offset += comment_len + sizeof(tDWORD);	// comment len value
							prop_offset = header_offset;
							header_offset += sizeof(tDWORD);	//reserved place for props count
							
							//! copy props from hPropHolder

							if (tHeaderErr == cFALSE)
							{
								tPTR pProp;
								tDWORD total_props = 0;
								tPROPID props;
								tDWORD prop_size;
								tDWORD coockie = 0;
								
								tBOOL bExit = cFALSE;
								tBOOL bCustom = cFALSE;
								tERROR err;
								//+ ----------------------------------------------------------------------------------------
								while (!bExit)
								{
									if (!bCustom)
									{
										err = CALL_SYS_PropertyEnumId(hPropHolder, &props, &coockie);
										if (PR_FAIL(err))
										{
											bCustom = cTRUE;
											coockie = 0;
										}
									}
									
									if (bCustom)
										err = CALL_SYS_PropertyEnumCustomId(hPropHolder, &props, &coockie);

									if (PR_FAIL(err))
										bExit = cTRUE;
									else
									{
										if (!IS_PROP_TYPE(props, pTYPE_PTR))
										{
											if (PR_SUCC(CALL_SYS_PropertyGet(hPropHolder, &prop_size, props, NULL, 0)))
											{
												if (PR_FAIL(CALL_SYS_ObjHeapAlloc(_this, &pProp, prop_size)))
													tHeaderErr = cTRUE;
												else
												{
													if (PR_SUCC(CALL_SYS_PropertyGet(hPropHolder, &prop_size, props, pProp, prop_size)))
													{
														CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, &props, sizeof(props));
														header_offset += sizeof(props);
														
														CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, &prop_size, sizeof(prop_size));
														header_offset += sizeof(prop_size);
														
														CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, pProp, prop_size);
														header_offset += prop_size;
														
														total_props++;
													}
													
													CALL_SYS_ObjHeapFree(_this, pProp);
													pProp = NULL;
												}
											}
										}
									}
								}
								
								{
									tPROPID props = pgOBJECT_CODEPAGE;
									tDWORD prop_size = sizeof(tCODEPAGE);

									tCODEPAGE cp = CALL_SYS_PropertyGetDWord(hPropHolder, props);

									CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, &props, sizeof(props));
									header_offset += sizeof(props);

									CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, &prop_size, sizeof(prop_size));
									header_offset += sizeof(prop_size);
									
									CALL_IO_SeekWrite((hi_IO)send_point, NULL, header_offset, &cp, prop_size);
									header_offset += prop_size;

									total_props++;
								}
								
								CALL_IO_SeekWrite((hi_IO)send_point, NULL, prop_offset, &total_props, sizeof(total_props));
								
								//+ ----------------------------------------------------------------------------------------
							}
						}
						
						if (tHeaderErr == cTRUE)
							*(tERROR*)par_buf = errOBJECT_NOT_CREATED;
						
						if (PR_FAIL(CALL_SYS_ObjHeapAlloc(_this, &buffer, _BUFFER_SIZE)))
							*(tERROR*)par_buf = errNOT_ENOUGH_MEMORY;
						else
						{
							offset = 0L;
							do
							{
								if (PR_FAIL(CALL_IO_SeekRead((hi_IO)ctx, &dwRead, offset, buffer, _BUFFER_SIZE)))
									*(tERROR*)par_buf = errOBJECT_READ;
								else
								{
									if (PR_FAIL(CALL_IO_SeekWrite((hi_IO)send_point, &dwWrite, offset + header_offset, buffer, dwRead)))
										*(tERROR*)par_buf = errOBJECT_WRITE;
									else
									{
										if (dwRead != dwWrite)
											*(tERROR*)par_buf = errUNEXPECTED;
									}
									
									offset += dwRead;
								}
								
							}while ((dwRead == _BUFFER_SIZE) && (*(tERROR*)par_buf == errOK));
							CALL_SYS_ObjHeapFree(_this, buffer);
							
							if (PR_FAIL(*(tERROR*)par_buf))
							{
								//DbgBreakPoint(_this);
							}
							else
							{
								//! restore data to internal io
								/*t
								ERROR err = errOK;
								tDWORD len = sizeof(err);
								IO_MsgReceive(_this, pmc_SUBST_IO, pm_SUBST_IO_RESTORE_CONTEXT, _this, (hi_IO)ctx, NULL, &err, &len);
								*/
							}
						}
					}
				}
				error = errOK_DECIDED;
			}
			break;
		case pm_SUBST_IO_RESTORE_CONTEXT:
			if ((par_buf ==NULL) || (par_buf_len == NULL) || (*par_buf_len != sizeof(tERROR)))
				error = errPARAMETER_INVALID;
			else
			{
				if (PR_FAIL(CALL_SYS_ObjectCheck(ctx, ctx, IID_IO, PID_ANY, 0, cFALSE)))
					error = errPARAMETER_INVALID;
				else
				{
					if (_this->data->m_hIO != NULL)
					{
						CALL_SYS_ObjectClose(_this->data->m_hIO);
						_this->data->m_hIO = NULL;
					}
					if (PR_FAIL(CALL_SYS_ObjectCreateQuick(_this, &_this->data->m_hIO, IID_IO, PID_TMPFILE, SUBTYPE_ANY)))
						*(tERROR*)par_buf = errUNEXPECTED;
					else
					{
						tQWORD offset = 0;
						tQWORD Tag;
						tDWORD len = sizeof(Tag);
						
						*(tERROR*)par_buf = errNOT_SUPPORTED;
						if (PR_SUCC(CALL_IO_SeekRead((hi_IO) ctx, &len, 0, &Tag, len)))
						{
							if (Tag == _SUBST_TAG)
							{
								//! read attributes
								tDWORD props_count = 0;
								tDWORD comment_len;
								tPTR ptr = NULL;
								
								offset += sizeof(tQWORD);
								if (PR_SUCC(CALL_IO_SeekRead((hi_IO) ctx, &len, offset, &comment_len, sizeof(tDWORD))))
								{
									offset += sizeof(tDWORD);
									if (comment_len != 0)
									{
										if (PR_SUCC(CALL_SYS_ObjHeapAlloc(_this, &ptr, comment_len)))
										{
											if (_this->data->m_hComment == NULL)
											{
												CALL_SYS_ObjectCreateQuick(_this, &_this->data->m_hComment, IID_STRING, PID_ANY, SUBTYPE_ANY);
												CALL_String_SetCP(_this->data->m_hComment, cCP_ANSI);
											}
											
											if (_this->data->m_hComment != NULL)
											{
												if (PR_SUCC(CALL_IO_SeekRead((hi_IO) ctx, &comment_len, offset, ptr, comment_len)))
													CALL_String_ImportFromBuff(_this->data->m_hComment, NULL, ptr, comment_len, cCP_ANSI, cSTRING_CONTENT_ONLY);
											}
											
											CALL_SYS_ObjHeapFree(_this, ptr);
										}
									}
									offset += comment_len;
									
									if (PR_SUCC(CALL_IO_SeekRead((hi_IO) ctx, &len, offset, &props_count, sizeof(tDWORD))))
									{
										tDWORD cou;
										tDWORD prop_len;
										tPROPID prop_id;
										
										offset += sizeof(tDWORD);
										
										for(cou = 0; cou < props_count; cou++)
										{
											CALL_IO_SeekRead((hi_IO) ctx, &len, offset, &prop_id, sizeof(prop_id));
											offset += sizeof(prop_id);
											
											CALL_IO_SeekRead((hi_IO) ctx, &len, offset, &prop_len, sizeof(prop_len));
											offset += sizeof(prop_len);
											
											if (PR_SUCC(CALL_SYS_ObjHeapAlloc(_this, &ptr, prop_len)))
											{
												if (PR_SUCC(CALL_IO_SeekRead((hi_IO) ctx, &len, offset, ptr, prop_len)))
													IO_PROP_CommonPropWrite(_this, &len, prop_id, ptr, prop_len);
												
												offset += prop_len;
												CALL_SYS_ObjHeapFree(_this, ptr);
											}
										}
										
										// copy data
										if (PR_FAIL(CALL_SYS_ObjHeapAlloc(_this, &ptr, _BUFFER_SIZE)))
											*(tERROR*)par_buf = errNOT_ENOUGH_MEMORY;
										else
										{
											tDWORD dwRead, dwWrite;
											tQWORD dest_offset = 0L;
											do
											{
												if (PR_FAIL(CALL_IO_SeekRead((hi_IO)ctx, &dwRead, offset, ptr, _BUFFER_SIZE)))
													*(tERROR*)par_buf = errOBJECT_READ;
												else
												{
													if (PR_FAIL(CALL_IO_SeekWrite((hi_IO) _this->data->m_hIO, &dwWrite, dest_offset, ptr, dwRead)))
														*(tERROR*)par_buf = errOBJECT_WRITE;
													else
													{
														if (dwRead != dwWrite)
															*(tERROR*)par_buf = errUNEXPECTED;
													}
													
													offset += dwRead;
													dest_offset += dwRead;
												}
											}while ((dwRead == _BUFFER_SIZE) && (*(tERROR*)par_buf == errOK));
											
											CALL_SYS_ObjHeapFree(_this, ptr);
											
											*(tERROR*)par_buf = errOK;
										}
									}
								}
								
							}
						}
					}
				}
				error = errOK_DECIDED;
			}
			break;
		case pm_SUBST_IO_SET_COMMENT:
			if (PR_SUCC(CALL_SYS_ObjectCheck(ctx, ctx, IID_STRING, PID_ANY, 0, cFALSE)))
			{
				if (_this->data->m_hComment == NULL)
				{
					CALL_SYS_ObjectCreateQuick(_this, &_this->data->m_hComment, IID_STRING, PID_ANY, SUBTYPE_ANY);
				}
				
				if (_this->data->m_hComment != NULL)
				{
					CALL_String_SetCP(_this->data->m_hComment, cCP_ANSI);
					CALL_String_ImportFromStr(_this->data->m_hComment, NULL, (hSTRING) ctx, cSTRING_WHOLE);
					error = errOK_DECIDED;
				}
			}
			break;
		}
	}
	
	PR_TRACE_A1( _this, "Leave IO::MsgReceive method, ret tERROR = 0x%08x", error );
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
	if (_this->data->m_hIO == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_IO_SeekRead((hi_IO)_this->data->m_hIO, &ret_val, offset, buffer, size);
	
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
	if (_this->data->m_hIO == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_IO_SeekWrite((hi_IO)_this->data->m_hIO, &ret_val, offset, buffer, size);
	
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
	
	if (_this->data->m_hIO == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_IO_GetSize((hi_IO)_this->data->m_hIO, &ret_val, type);
	
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
	if (_this->data->m_hIO == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_IO_SetSize((hi_IO)_this->data->m_hIO, new_size);
	
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
	
	if (_this->data->m_hIO == NULL)
		error = errOBJECT_NOT_INITIALIZED;
	else
		error = CALL_IO_Flush((hi_IO)_this->data->m_hIO);
	
	PR_TRACE_A1( _this, "Leave IO::Flush method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



