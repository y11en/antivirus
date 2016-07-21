// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  09 December 2002,  12:48 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Explode decompressor
// Author      -- Andy Nikishin
// File Name   -- explode_t.c
// Additional info
//    Explode decompressor. Uses for ZIP Archivers
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "explode_t.h"
#include "explodei.h"

#include "Prague/iface/i_io.h"
#include "Prague/iface/i_seqio.h"
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
	ExplodeData*            data; // pointer to the "Transformer" data structure
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



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transformer". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Explode mini extractor", 23 )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, ExplodeData, hOutputIO, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, ExplodeData, hInputIO, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, ExplodeData, Zip_ucsize, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plEXPLODE_BIT_FLAG, ExplodeData, general_purpose_bit_flag, cPROP_BUFFER_WRITE )
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
		PID_EXPLODE,                            // plugin identifier
		0x00000000,                             // subtype identifier
		Transformer_VERSION,                    // interface version
		VID_ANDY,                               // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(ExplodeData),                    // memory size
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
		if(PR_SUCC(error))
			error = CALL_Heap_Alloc(_this->data->hHeap, (tPTR*)&_this->data->slide, 0x8000);
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

	PR_TRACE_A0( _this, "Enter Transformer::ObjectInitDone method" );
	error = errOBJECT_NOT_INITIALIZED;

	if(_this->data->hInputIO != NULL && _this->data->hOutputIO != NULL )
	{
		error = errOK;
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
tERROR error ;
tDWORD ret_val;
	PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

	error = errOK;
	_this->data->Zip_csize = (tDWORD)_this->data->Zip_ucsize;
	_this->data->me = (hOBJECT)_this;
	ret_val = explode(_this->data);
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
tERROR error;
tQWORD ret_val;
	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

	error = errOK;
	_this->data->Zip_csize = (tDWORD)_this->data->Zip_ucsize;
	_this->data->me = (hOBJECT)_this;
	ret_val = explode(_this->data);
	if(ret_val == 0)
		error = errOBJECT_DATA_CORRUPTED;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



