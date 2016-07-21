// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  10 December 2002,  12:29 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Archive support for KAV ver 4
// Sub project -- Not defined
// Purpose     -- Deflate packer plugin
// Author      -- Andy Nikishin
// File Name   -- deflate_t.c
// Additional info
//    Deflate packer plugin
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "deflate_t.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure
/*
typedef struct _tag_DeflateData 
{
	tERROR   error;              // --
	hSEQ_IO  hInputIO;           // --
	tBYTE*   Window;             // --
	hSEQ_IO  hOutputIO;          // --
//	tBOOL    bOutIoSetBySomeOne; // --
	z_stream id_stream;          // --
	tUINT    ucWindowSize;       // --
	tINT     iBufMul;            // --
	tDWORD   dwSizeOfFile;       // --
	tDWORD   dwYeldCounter;      // --
	hOBJECT  _Me;                // --
	tDWORD   dwCRC;              // --
} DeflateData;
*/
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
	const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	DeflateData*       data; // pointer to the "Transformer" data structure
} *hi_Transformer;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Transformer_ObjectInit( hi_Transformer _this );
tERROR pr_call Transformer_ObjectInitDone( hi_Transformer _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Transformer_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       Transformer_ObjectInit,
	(tIntFnObjectInitDone)   Transformer_ObjectInitDone,
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

tERROR WriteOutWithResize(z_streamp z, tPTR buffer, tDWORD dwSize);

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transformer". Static(shared) property table variables
const tVERSION Version = 2; // must be READ_ONLY at runtime
const tSTRING szComments = "Deflate packer"; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, Version, sizeof(Version) )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComments, 15 )
	mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, DeflateData, qwSizeOfFile, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, DeflateData, hInputIO, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, DeflateData, hOutputIO, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plDEFLATE_WINDOW_SIZE, DeflateData, ucWindowSize, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plDEFLATE_WINDOW, DeflateData, Window, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plDEFLATE_WINDOW_MULTIPIER, DeflateData, iBufMul, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plDEFLATE_INPUT_CRC, DeflateData, dwCRC, cPROP_BUFFER_READ )
//! 	mLOCAL_PROPERTY_BUF( plppDEFLATE_INPUT_CRC, _tag_DeflateData, dwCRC, cPROP_BUFFER_READ )
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
		PID_DEFLATE,                            // plugin identifier
		0x00000000,                             // subtype identifier
		Version,                                // interface version
		VID_ANDY,                               // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(DeflateData),               // memory size
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
tERROR error;
	PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );

	error = CALL_SYS_ObjectCreate((hOBJECT)_this, &_this->data->hHeap, IID_HEAP, PID_ANY,0);
	if(error == errOK)
	{
		CALL_SYS_PropertySetDWord(_this->data->hHeap, pgHEAP_GRANULARITY, 128*1024);
		error = CALL_SYS_ObjectCreateDone(_this->data->hHeap);
	}
	
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
DeflateData* data;
tERROR error;

	PR_TRACE_A0( _this, "Enter Transformer::ObjectInitDone method" );

	error = errOBJECT_NOT_INITIALIZED;
	data = _this->data;
	if(data->hOutputIO != NULL && data->hInputIO != NULL)
	{
		if(data->ucWindowSize != 0)
		{
			data->ucWindowSize = 32*1024;
			if(data->Window == NULL)
				data->Window = _New(data, _this->data->ucWindowSize);
			error = errOK;
			if(data->Window == NULL)
			{
				error = errNOT_ENOUGH_MEMORY;
			}
		}
	}
	if(error == errOK)
	{
		data->_Me = (hOBJECT)_this;
		data->error = errOK;
		data->id_stream.data = data;

//		error = deflateInit(&data->id_stream, Z_DEFAULT_COMPRESSION);
//		if(error != errOK)
//			error = errPROPERTY_INVALID;
/*		data->id_stream.z_fixed_mem=(inflate_huft *)New(data, sizeof(inflate_huft)*FIXEDH);
		if(data->id_stream.z_fixed_mem==NULL)
		{
			data->error=errNOT_ENOUGH_MEMORY;
		}
		else
		{
			data->fixed_mem=data->id_stream.z_fixed_mem;
			
			data->fixed_built = 0;
			data->error=inflateInit2_(&data->id_stream,data->iBufMul,data->Window,data->ucWindowSize);
			if (data->error != errOK)
			{
				Delete(data, (tPTR)data->id_stream.z_fixed_mem);
				data->fixed_mem=NULL;
			}
		}
		error = data->error;*/
	}

	PR_TRACE_A1( _this, "Leave Transformer::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ProcessChunck )
// Result comment
//    количество реально выданных байт (>=dwSize)
//
// Parameters are:
tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* result, tDWORD dwSze )
{
tERROR error;
tQWORD qret_val;

	PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

	error = Transformer_Proccess( _this, &qret_val);

	if ( result )
		*result = (tDWORD)qret_val;
	PR_TRACE_A2( _this, "Leave Transformer::ProcessChunck method, ret tDWORD = %u, error = 0x%08x", (tDWORD)qret_val, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Proccess )
// Result comment
//    размер распакованных данных
// Parameters are:
tERROR pr_call Transformer_Proccess( hi_Transformer _this, tQWORD* result )
{
DeflateData* data;
tERROR  error;
tQWORD qwLen;
tQWORD ret_val;

	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );
	data = _this->data;
	ret_val = 0;
	error = errPARAMETER_INVALID;
	
	if(data->qwSizeOfFile!=0)
	{
		qwLen = data->qwSizeOfFile;
	}
	else
		CALL_IO_GetSize((hIO)data->hInputIO, &qwLen, IO_SIZE_TYPE_EXPLICIT);
	data->id_stream.zAlloc = NULL;
	data->id_stream.zfree = NULL;
	data->id_stream.opaque = NULL;
	data->id_stream.dwCRC = 0;
	error = deflateInit2_(&data->id_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, data->iBufMul, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, sizeof(data->id_stream));
	data->id_stream.Write = WriteOutWithResize;
	
	data->id_stream.avail_in = (tDWORD)qwLen;
	while (data->id_stream.total_in != (tDWORD)qwLen /*&& data->id_stream.total_out < comprLen*/) 
	{
		error = deflate(&data->id_stream, Z_NO_FLUSH);
		if(error != errOK)
			break;
	}
	if(error == errOK)
	{
		for (;;) 
		{
			error = deflate(&data->id_stream, Z_FINISH);
			if (error == Z_STREAM_END)
			{
				error = errOK;
				break;
			}
			if(error != errOK)
				break;
		}
		if(error == errOK)
		{
			data->dwCRC = data->id_stream.dwCRC;
			ret_val = data->id_stream.total_out;
		}
		else
		{
			error = errNOT_OK;
		}
	}
	
	deflateEnd(&data->id_stream);
	
	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



tERROR WriteOutWithResize(z_streamp z, tPTR buffer, tDWORD dwSize)
{
tQWORD qwOldSize;
tERROR err;
	
	if(z->data->dwYeldCounter++ > 0x100)
	{
		z->data->dwYeldCounter = 0;
		err = CALL_SYS_SendMsg(z->data->hOutputIO, pmc_PROCESSING, pm_PROCESSING_YIELD, z->data->_Me, 0 ,0 );
		if(err == errOPERATION_CANCELED)
			return err;
	}
	CALL_SeqIO_GetSize(z->data->hOutputIO, &qwOldSize, IO_SIZE_TYPE_EXPLICIT);
	CALL_SeqIO_SetSize(z->data->hOutputIO, qwOldSize+dwSize);
	
#ifdef _DEBUG
	if(errOK!=CALL_SeqIO_Write(z->data->hOutputIO, NULL, buffer,dwSize))
	{
		Tracev((" 2 Write Out %d Bytes Error",dwSize));
		return errOBJECT_WRITE;
	}
	return errOK;
#else
	return CALL_SeqIO_Write(z->data->hOutputIO, NULL, buffer,dwSize);
#endif 
}
