// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  10 January 2003,  12:15 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- ARJ Archiver
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- arjpack.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "arjpack.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure
/*
typedef struct tag_ArjPackData 
{
	tQWORD  qwSize;    // --
	hSEQ_IO hInputIo;  // --
	hSEQ_IO hOutputIo; // --
	tWORD right[2 * NC - 1];
	tWORD left[2 * NC - 1];
	tBYTE c_len[NC];
	tBYTE pt_len[NPT];
} ArjPackData;
*/
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
	const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	ArjPackData*            data; // pointer to the "Transformer" data structure
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
const tSTRING szComment = "Arj Packer"; // must be READ_ONLY at runtime
//! const tSTRING szComment = place the default value here; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
	mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, ArjPackData, qwSize, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, ArjPackData, hInputIo, cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF( plCOMPRESSON_METHOD, ArjPackData, dwMethod, cPROP_BUFFER_WRITE )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 11 )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, ArjPackData, hOutputIo, cPROP_BUFFER_WRITE )
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
		PID_ARJPACKER,                          // plugin identifier
		0x00000000,                             // subtype identifier
		Transformer_VERSION,                    // interface version
		VID_ANDY,                               // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(ArjPackData),                    // memory size
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
tERROR error ;
ArjPackData* data;

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
ArjPackData* data;
tERROR error;
tQWORD qwTmp;

	PR_TRACE_A0( _this, "Enter Transformer::ObjectInitDone method" );

	data = _this->data;
	error = errOBJECT_NOT_INITIALIZED;

	if(data->hOutputIo != NULL && data->hInputIo != NULL)
	{
		data->_Me = (hOBJECT)_this;
//		data->origsize = (tDWORD)data->qwDecompressSize;
		CALL_SeqIO_GetSize(data->hInputIo, &qwTmp, IO_SIZE_TYPE_EXPLICIT);
//		data->compsize = (tDWORD)qwTmp;
//		data->last_len = data->compsize;
//		data->UnArjWrite = WriteOutWithResize;
//		data->incnt = 0;
//		data->error = errOK;
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
tERROR pr_call Transformer_ObjectPreClose( hi_Transformer _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Transformer::ObjectPreClose method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Transformer::ObjectPreClose method, ret tERROR = 0x%08x", error );
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
ArjPackData* data;

	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

	error = errOK;
	data = _this->data;
	data->origsize = (tDWORD)data->qwSize;
	if(data->qwSize == 392)
	{
		data->qwSize = 392;
	}

	data->current_bufsiz = BUFSIZ_DEFAULT;
/*	data->depth = 0;
	data->bitbuf = 0;
	data->bitcount = 0;
	data->dicbit = 0;
	data->dicbit = 0;
	data->dwCompressed = 0;
	data->output_mask = 0;
	data->output_pos = 0;
	data->heapsize = 0;
	data->fpcount = 0;
	data->treesize = 0;
	data->numchars = 0;
	data->dicsiz_cur = 0;
	data->dicpos = 0;
	data->tc_passes = 0;
	data->st_n = 0;
	data->dic_alloc = 0;
	data->out_bytes = 0;
	data->out_avail = 0;
	data->cpos = 0;
	data->bufsiz = 0;*/

	if(data->dwMethod == 1 || data->dwMethod == 2 || data->dwMethod == 3)
		encode(data, data->dwMethod, 4);
	else if(data->dwMethod == 4)
		encode_f(data);
//	else if(method==9)
//		hollow_encode();
	
	if ( result )
		*result = data->dwCompressed;
	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", data->dwCompressed, error );
	return error;
}
// AVP Prague stamp end



