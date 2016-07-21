// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  20 December 2002,  16:22 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- mail_msg
// Purpose     -- Outlook message object system
// Author      -- Mezhuev
// File Name   -- msg_tr.c
// Additional info
//    Outlook message object system
//    Данный плагин предназначен для работы с Outlook сообщением.
//
//    Реализация основана на MAPI интерфейсе.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end




// AVP Prague stamp begin( Includes for interface,  )
#include "msg_mapi.h"
#include "msg_tr.h"

#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
//    Техническая реализация данного интерфейса. Предназначена для буферизации OLE Stream.
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
  // data structure "MsgTR_Data" is described in "O:\Prague\outlook\msg_tr.h" header file
  // data structure "MsgTR_Data" is described in "O:\Praque\outlook\msg_tr.h" header file
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
	const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	MsgTR_Data*             data; // pointer to the "Transformer" data structure
} *hi_Transformer;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Transformer_ObjectInit( hi_Transformer _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Transformer_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       Transformer_ObjectInit,
	(tIntFnObjectInitDone)   NULL,
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
tERROR pr_call Transformer_PROP_GetSize( hi_Transformer _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Transformer". Static(shared) property table variables
//! const tQWORD Transformer_TRANSFORM_SIZE = -1; // must be READ_ONLY at runtime
//! const tQWORD -1 = place the default value here; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Transformer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Transformer_VERSION )
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, MsgTR_Data, iseqio, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
//! 	mSHARED_PROPERTY_PTR( pgTRANSFORM_SIZE, Transformer_TRANSFORM_SIZE, sizeof(Transformer_TRANSFORM_SIZE) )
	mLOCAL_PROPERTY_FN( pgTRANSFORM_SIZE, Transformer_PROP_GetSize, NULL )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Transformer interface", 22 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_BASED_ON, MsgTR_Data, io, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, MsgTR_Data, oseqio, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
		PID_MAILMSG,                            // plugin identifier
		0x00000000,                             // subtype identifier
		Transformer_VERSION,                    // interface version
		VID_MEZHUEV,                            // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(MsgTR_Data),                     // memory size
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



// AVP Prague stamp begin( Property method implementation, GetSize )
// Interface "Transformer". Method "Get" for property(s):
//  -- TRANSFORM_SIZE
tERROR pr_call Transformer_PROP_GetSize( hi_Transformer _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) 
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method Transformer_PROP_GetSize for property pgTRANSFORM_SIZE" );

	error = TransformerGetSize(_this->data, (tQWORD*)buffer);
	*out_size = sizeof(tQWORD);

	PR_TRACE_A2( _this, "Leave *GET* method Transformer_PROP_GetSize for property pgTRANSFORM_SIZE, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
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
	MsgTR_Data *d = _this->data;
	PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );

	// Place your code here
	d->io = (hIO)CALL_SYS_ParentGet((hOBJECT)_this, IID_IO);

	PR_TRACE_A1( _this, "Leave Transformer::ObjectInit method, ret tERROR = 0x%08x", error );
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
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

	// Place your code here
	error = TransformerProcessChunck(_this->data, dwSze, &ret_val);

	if ( result )
		*result = (tDWORD)ret_val;
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
	tERROR error = errNOT_IMPLEMENTED;
	tQWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

	// Place your code here
	error = TransformerProcess(_this->data, &ret_val);

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



