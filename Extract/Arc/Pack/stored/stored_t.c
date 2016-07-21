// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  10 December 2002,  14:41 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Stored compressor
// Author      -- Andy Nikishin
// File Name   -- stored_t.c
// Additional info
//    No compression compressor
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Transformer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "stored_t.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Transformer interface implementation
// Short comments -- Generic convertion interface
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Transformer. Inner data structure

typedef struct tag_StoredData 
{
	hSEQ_IO hInputIO;   // --
	hSEQ_IO hOutputIO;  // --
	tQWORD  qwFileSize; // --
	tBYTE*  Buffer;
} StoredData;

// AVP Prague stamp end

#define cBUFFER_SIZE 0x1000

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Transformer 
{
	const iTransformerVtbl* vtbl; // pointer to the "Transformer" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	StoredData*             data; // pointer to the "Transformer" data structure
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

tERROR CopyFile(hi_Transformer _this, hSEQ_IO hSrc, hSEQ_IO Dest, tBYTE* Buffer, tDWORD dwSizeOfCopy);

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
const tVERSION PluginVersion = 1; // must be READ_ONLY at runtime
const tSTRING Comment = "Stored packer"; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Transformer_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, PluginVersion, sizeof(PluginVersion) )
//	mSHARED_PROPERTY( pgINTERFACE_VERSION, PluginVersion )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, Comment, 14 )
//	GodeGen: property "pgINTERFACE_COMMENT" - can't extract default static value
	mLOCAL_PROPERTY_BUF( pgTRANSFORM_SIZE, StoredData, qwFileSize, cPROP_BUFFER_WRITE )
	mLOCAL_PROPERTY_BUF( pgINPUT_SEQ_IO, StoredData, hInputIO, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_SEQ_IO, StoredData, hOutputIO, cPROP_BUFFER_READ_WRITE )
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
		PID_STORED,                             // plugin identifier
		0x00000000,                             // subtype identifier
		PluginVersion,                          // interface version
		VID_ANDY,                               // interface developer
		&i_Transformer_vtbl,                    // internal(kernel called) function table
		(sizeof(i_Transformer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_Transformer_vtbl,                    // external function table
		(sizeof(e_Transformer_vtbl)/sizeof(tPTR)),// external function table size
		Transformer_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(Transformer_PropTable),// property table size
		sizeof(StoredData),                     // memory size
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
	PR_TRACE_A0( _this, "Enter Transformer::ObjectInit method" );

	error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&_this->data->Buffer, cBUFFER_SIZE);

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

	error = errOK;
	if(_this->data->hInputIO == NULL || _this->data->hOutputIO == NULL)
	{
		error = errOBJECT_NOT_INITIALIZED;
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
tERROR pr_call Transformer_ProcessChunck( hi_Transformer _this, tDWORD* result, tDWORD dwSize ) 
{
tERROR error ;
tDWORD ret_val;
tQWORD qwFileSize;
tQWORD qwFilePos;
StoredData* data;

	PR_TRACE_A0( _this, "Enter Transformer::ProcessChunck method" );

	data = _this->data;
	ret_val = 0;
	error = CALL_IO_GetSize((hIO)data->hOutputIO, &qwFileSize, IO_SIZE_TYPE_EXPLICIT);
	if(error == errOK)
	{
		CALL_SeqIO_Seek(data->hOutputIO, &qwFilePos, 0, cSEEK_SET);
		
		if(qwFilePos + dwSize > qwFileSize)
		{
			CALL_IO_SetSize(data->hOutputIO, qwFilePos + dwSize);
		}
		
		error = CopyFile(_this, data->hInputIO, data->hOutputIO, data->Buffer, dwSize);
		if(PR_SUCC(error))
			ret_val = dwSize;
	}
	
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
tQWORD ret_val ;
StoredData* data;
	PR_TRACE_A0( _this, "Enter Transformer::Proccess method" );

	data = _this->data;
	CALL_IO_GetSize((hIO)data->hInputIO, &ret_val, IO_SIZE_TYPE_EXPLICIT);
///	CALL_IO_SetSize(data->hOutputIO, ret_val);
	error = CopyFile(_this, data->hInputIO, data->hOutputIO, data->Buffer, (tDWORD) ret_val);
	if(PR_FAIL(error))
		ret_val = 0;
	
	if ( result )
		*result = ret_val;
	
	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Transformer::Proccess method, ret tQWORD = %I64u, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



tERROR CopyFile(hi_Transformer _this, hSEQ_IO hSrc, hSEQ_IO Dest, tBYTE* Buffer, tDWORD dwSizeOfCopy)
{
tDWORD read;
tERROR error;
tDWORD wrote;
	
	error = errOBJECT_READ;
	wrote = 0;
	error = CALL_SeqIO_Read(hSrc, &read, Buffer, cBUFFER_SIZE > dwSizeOfCopy ? dwSizeOfCopy: cBUFFER_SIZE);
	while(read != 0 )
	{
		error = CALL_SeqIO_Write(Dest, NULL, Buffer, read);
		
		if(errOK != error || wrote+read >= dwSizeOfCopy)
			break;
		wrote += read;
		error = CALL_SeqIO_Read(hSrc, &read, Buffer, cBUFFER_SIZE);
		if(wrote+read > dwSizeOfCopy)
			read = dwSizeOfCopy - wrote;
		
		error = CALL_SYS_SendMsg(hSrc, pmc_PROCESSING, pm_PROCESSING_YIELD, _this, 0 ,0 );
		if(error == errOPERATION_CANCELED)
			return error;
		error = errOK;
	}
	CALL_IO_Flush(Dest);
	if(error == errEOF)
		error = errOK;
	return error;
}
