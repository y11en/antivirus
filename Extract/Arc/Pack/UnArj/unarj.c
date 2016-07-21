// AVP Prague stamp begin( Interface header,  )
// -------- Saturday,  04 January 2003,  16:47 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Decompressor for ARJ Archives
// Author      -- Andy Nikishin
// File Name   -- unarj.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "unarj.h"
// AVP Prague stamp end

//tPTR  __cdecl memset(tPTR, tINT, tINT);

// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
	const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	UnArj_Data*             data; // pointer to the "Transformer" data structure
} *hi_Transformer;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Transformer_ObjectInit( hi_Transformer _this );
tERROR pr_call Transformer_ObjectInitDone( hi_Transformer _this );
tERROR pr_call Transformer_ObjectPreClose( hi_Transformer _this );
// AVP Prague stamp end


tERROR WriteOut(UnArj_Data* data, tPTR buffer, tDWORD dwSize);
tERROR WriteOutWithResize(UnArj_Data* data, tPTR buffer, tDWORD dwSize);

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Transformer_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       Transformer_ObjectInit,
	(tIntFnObjectInitDone)   Transformer_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,//Transformer_ObjectPreClose,
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
const tSTRING szComment = "UnArj Decompressor"; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
	mLOCAL_PROPERTY_BUF( plDECOMPRESSON_METHOD, UnArj_Data, dwDecoMethod, cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, UnArj_Data, hOutputIo, cPROP_BUFFER_READ_WRITE)
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, UnArj_Data, hInputIo, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, UnArj_Data, qwDecompressSize, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 19 )
//! 	GodeGen: property "pgINTERFACE_COMMENT" - can't extract default static value
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
		PID_UNARJ,                              // plugin identifier
		0x00000000,                             // subtype identifier
		Transformer_VERSION,                    // interface version
		VID_ANDY,                               // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(UnArj_Data),                     // memory size
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
UnArj_Data* data;

	PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );
	
	data = _this->data;
	error = CALL_SYS_ObjectCreate((hOBJECT)_this, &data->hHeap, IID_HEAP, PID_ANY,0);
	if(error == errOK)
	{
		CALL_SYS_PropertySetDWord(data->hHeap, pgHEAP_GRANULARITY, 128*1024);
		error = CALL_SYS_ObjectCreateDone(data->hHeap);
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
tERROR error ;
UnArj_Data* data;
tQWORD qwTmp;

	PR_TRACE_A0( _this, "Enter Transformer::ObjectInitDone method" );

	data = _this->data;
	error = errOBJECT_NOT_INITIALIZED;
	if(data->hOutputIo != NULL && data->hInputIo != NULL)
	{
		data->_Me = (hOBJECT)_this;
		data->origsize = (tDWORD)data->qwDecompressSize;
		CALL_SeqIO_GetSize(data->hInputIo, &qwTmp, IO_SIZE_TYPE_EXPLICIT);
		data->compsize = (tDWORD)qwTmp;
		data->last_len = data->compsize;
		data->UnArjWrite = WriteOutWithResize;
		data->incnt = 0;
		data->error = errOK;
		error = errOK;
	}
	
	PR_TRACE_A1( _this, "Leave Transformer::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
/*
tERROR pr_call Transformer_ObjectPreClose( hi_Transformer _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Transformer::ObjectPreClose method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Transformer::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
*/
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ProcessChunck )
// Result comment
//    количество реально выданных байт (>=dwSize)
//
// Parameters are:
tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* result, tDWORD dwSze )
{
tERROR error ;
tDWORD ret_val;
	PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

	error = errEOF;

	switch(_this->data->dwDecoMethod)
	{
		case DEFLATED1 :
		case DEFLATED2 :
		case DEFLATED3 : 
			ret_val = (tDWORD)decode_main(_this->data); 
			break;
		case DEFLATED4 : 
			ret_val = (tDWORD)decode_f(_this->data);
			break;
		default :
			break;
	}
		
	if(ret_val == 0)
		error = errOBJECT_DATA_CORRUPTED;
	
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
tQWORD ret_val, qwTmp;
UnArj_Data* data;

	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

	error = errOK;
	data = _this->data;

	//CALL_SeqIO_SetSize(data->hOutputIo, data->qwDecompressSize);
	data->incnt = 0;
	data->bitbuf = 0;
	data->bitcount = 0;
	data->origsize = (tDWORD)data->qwDecompressSize;
	CALL_SeqIO_GetSize(data->hInputIo, &qwTmp, IO_SIZE_TYPE_EXPLICIT);
	data->compsize = (tDWORD)qwTmp;
	data->last_len = data->compsize;
	data->dwDecoded = 0;

/*	memset(data->pt_table, 0, sizeof(data->pt_table));
	memset(data->right, 0, sizeof(data->right));
	memset(data->left, 0, sizeof(data->left));
	memset(data->c_len, 0, sizeof(data->c_len));
	memset(data->text, 0, sizeof(data->text));
	memset(data->inbuf, 0, sizeof(data->inbuf));
	memset(data->pt_len, 0, sizeof(data->pt_len));
*/
	switch(data->dwDecoMethod)
	{
		case DEFLATED1 :
		case DEFLATED2 :
		case DEFLATED3 : 
			ret_val = (tDWORD)decode_main(data); 
			break;
		case DEFLATED4 : 
			ret_val = (tDWORD)decode_f(data);
			break;
		default :
			break;
	}
	if(ret_val == 0)
		error = errOBJECT_DATA_CORRUPTED;
	
	if ( result )
		*result = ret_val;

	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end





tERROR WriteOut(UnArj_Data* data, tPTR buffer, tDWORD dwSize)
{
tERROR err;
	
	if(data->dwYeldCounter++ > 0x100)
	{
		data->dwYeldCounter = 0;
		err = CALL_SYS_SendMsg(data->hOutputIo, pmc_PROCESSING, pm_PROCESSING_YIELD, data->_Me, 0 ,0 );
		if(err == errOPERATION_CANCELED)
			return err;
	}
	data->dwDecoded += dwSize;
#ifdef _DEBUG
	if(errOK!=CALL_SeqIO_Write(data->hOutputIo, NULL, buffer,dwSize))
	{
		return errOBJECT_WRITE;
	}
	return errOK;
#else
	return CALL_SeqIO_Write(data->hOutputIo, NULL, buffer,dwSize);
#endif 
}

tERROR WriteOutWithResize(UnArj_Data* data, tPTR buffer, tDWORD dwSize)
{
tQWORD qwOldSize;
tERROR err;
	
	if(data->dwYeldCounter++ > 0x100)
	{
		data->dwYeldCounter = 0;
		err = CALL_SYS_SendMsg(data->hOutputIo, pmc_PROCESSING, pm_PROCESSING_YIELD, data->_Me, 0 ,0 );
		if(err == errOPERATION_CANCELED)
			return err;
	}
	CALL_SeqIO_GetSize(data->hOutputIo, &qwOldSize, IO_SIZE_TYPE_EXPLICIT);
	CALL_SeqIO_SetSize(data->hOutputIo, qwOldSize+dwSize);
	data->dwDecoded += dwSize;
	
#ifdef _DEBUG
	if(errOK!=CALL_SeqIO_Write(data->hOutputIo, NULL, buffer,dwSize))
	{
		return errOBJECT_WRITE;
	}
	return errOK;
#else
	return CALL_SeqIO_Write(data->hOutputIo, NULL, buffer,dwSize);
#endif 
}

tBYTE* ArjNew(UnArj_Data* data, tDWORD dwBytes)
{
tBYTE * lpData;
	
	CALL_Heap_Alloc(data->hHeap, (tPTR*)&lpData, dwBytes+16);
	if(lpData == NULL)
	{
		data->error = errNOT_ENOUGH_MEMORY;
	}
	return lpData;
}


tVOID ArjDelete(UnArj_Data* data, tPTR lpdata)
{
	if(lpdata == NULL)
	{
		return ;
	}
	CALL_Heap_Free(data->hHeap, lpdata);
}



