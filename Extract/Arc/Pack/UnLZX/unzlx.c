// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  26 November 2002,  12:32 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- LZX Decompressor
// Author      -- Andy Nikishin
// File Name   -- unzlx.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "unzlx.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
	const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	LZXData*               data; // pointer to the "Transformer" data structure
} *hi_Transformer;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Transformer_ObjectInit( hi_Transformer _this );
tERROR pr_call Transformer_ObjectInitDone( hi_Transformer _this );
tERROR pr_call Transformer_ObjectPreClose( hi_Transformer _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Transformer_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       Transformer_ObjectInit,
	(tIntFnObjectInitDone)   Transformer_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   Transformer_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpTransformer_ProcessChunck) ( hi_Transformer _this, tDWORD* result, tDWORD dwSze ); // -- Преобразовывает часть данных входного потока;
typedef   tERROR (pr_call *fnpTransformer_Proccess)      ( hi_Transformer _this, tQWORD* result );               // -- Преобразовывает весь входной поток;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iTransformerVtbl 
{
	fnpTransformer_ProcessChunck  ProcessChunck;
	fnpTransformer_Proccess       Proccess;
}; // "Transformer" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* result, tDWORD dwSze );
tERROR pr_call Transformer_Proccess( hi_Transformer _this, tQWORD* result );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iTransformerVtbl e_Transformer_vtbl = 
{
	Transformer_ProcessChunck,
	Transformer_Proccess
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transformer". Static(shared) property table variables
const tSTRING szCommnets = "LZX Decompressor"; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mLOCAL_PROPERTY_BUF( plWINDOW_SIZE, LZXData, dwWndSize, cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF( plRUN_INPUT_SIZE, LZXData, dwInputSize, cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, LZXData, hOutputSeqIo, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, LZXData, hInputSeqIo, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, LZXData, qwOutputSize, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, szCommnets, 14 )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
mPROPERTY_TABLE_END(Transformer_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Transformer". Register function
tERROR pr_call Transformer_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter Transformer::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TRANSFORMER,                        // interface identifier
		PID_LZXDECOMPRESSOR,                    // plugin identifier
		0x00000000,                             // subtype identifier
		Transformer_VERSION,                    // interface version
		VID_ANDY,                               // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(LZXstate),                       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Transformer(IID_TRANSFORMER) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave Transformer::Register method, ret tERROR = 0x%08x", error );
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
tERROR pr_call Transformer_ObjectInit( hi_Transformer _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Transformer::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Transformer_ObjectInitDone( hi_Transformer _this )
{
tERROR error;
LZXData* data;

	PR_TRACE_A0( _this, "Enter Transformer::ObjectInitDone method" );
	data = _this->data;

	if(data->dwWndSize == 0)
	{
		error = errPARAMETER_INVALID;
	}
	else
	{
		error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->lpWindow, (1 << data->dwWndSize) +0x1000);
		if(data->lpWindow == NULL)
		{
			error = errNOT_ENOUGH_MEMORY;
		}
		else
			error = LZXInit(data, data->lpWindow, data->dwWndSize);
	}

	PR_TRACE_A1( _this, "Leave Transformer::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Transformer_ObjectPreClose( hi_Transformer _this )
{
	PR_TRACE_A0( _this, "Enter Transformer::ObjectPreClose method" );
	CALL_SYS_ObjHeapFree(_this, _this->data->lpWindow);
	PR_TRACE_A1( _this, "Leave Transformer::ObjectPreClose method, ret tERROR = 0x%08x", errOK );
	return errOK;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, ProcessChunck )
// Result comment
//    количество реально выданных байт (>=dwSize)
//
// Parameters are:
tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* result, tDWORD dwSze )
{
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Transformer::ProcessChunck method, ret tDWORD = %u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Proccess )
// Result comment
//    размер распакованных данных
// Parameters are:
tERROR pr_call Transformer_Proccess( hi_Transformer _this, tQWORD* result )
{
tERROR error ;
tDWORD ret_val;
tBYTE* lpOutBuffer;
LZXData* data;

	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );
	ret_val = 0;
	data = _this->data;

	error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&lpOutBuffer, (tDWORD)data->qwOutputSize + 16);
	if(error == errOK)
	{
		data->dwRead = 0;
		ret_val = LZXDecompress(data, lpOutBuffer, (tDWORD)data->qwOutputSize , 1);
		if(0 != ret_val)
		{
			error = CALL_SeqIO_Write(data->hOutputSeqIo, NULL, lpOutBuffer, ret_val);
		}
		else
        {
            if      (data->err == DECR_DATAFORMAT)   error = errOBJECT_DATA_CORRUPTED;
            else if (data->err == DECR_ILLEGALDATA)  error = errOBJECT_DATA_CORRUPTED;
            else if (data->err == DECR_NOMEMORY)     error = errNOT_ENOUGH_MEMORY;
            else if (data->err == DECR_CHECKSUM)     error = errOBJECT_DATA_CORRUPTED;
            else if (data->err == DECR_INPUT)        error = errOBJECT_READ;
            else if (data->err == DECR_OUTPUT)       error = errOBJECT_WRITE;
			else                                     error = errNOT_OK;
        }

		if(data->dwInputSize)
		{
			if(data->dwInputSize < data->dwRead)
			{
				CALL_SeqIO_Seek(data->hInputSeqIo, NULL, data->dwRead - data->dwInputSize, cSEEK_BACK);
			}
			else if(data->dwInputSize > data->dwRead)
			{
				CALL_SeqIO_Seek(data->hInputSeqIo, NULL, data->dwInputSize - data->dwRead , cSEEK_FORWARD);
			}
		}
		
		if(lpOutBuffer)
			CALL_SYS_ObjHeapFree(_this, lpOutBuffer);
	}

	if ( result )
		*result = (tQWORD)ret_val;
	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



