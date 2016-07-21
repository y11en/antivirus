// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  16 December 2004,  14:32 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Calculates MD5 hash
// Author      -- Mike Pavlushchik
// File Name   -- hash.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Hash_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>

#include "../Hash_API/hash_api.h"
#include "../Hash_API/MD5/hash_md5.h"
#include "hash.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Hash interface implementation
// Short comments -- Calculates hash for data
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Hash. Inner data structure

typedef struct tag_sLocals {
	tPTR   pCtx;              // Контекст
	tBYTE  HashInitValue[16]; // --
	tDWORD HashSize;          // --
	tBOOL  bBadInternalState; // --
//! 	tBOOL  bStateOK;          // --
} sLocals;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Hash {
	const iHashVtbl*   vtbl; // pointer to the "Hash" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	sLocals*           data; // pointer to the "Hash" data structure
} *hi_Hash;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Hash_ObjectInit( hi_Hash _this );
tERROR pr_call Hash_ObjectInitDone( hi_Hash _this );
tERROR pr_call Hash_ObjectPreClose( hi_Hash _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Hash_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        Hash_ObjectInit,
	(tIntFnObjectInitDone)    Hash_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Hash_ObjectPreClose,
	(tIntFnObjectClose)       NULL,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpHash_Reset)   ( hi_Hash _this );                            // -- Подготовить объект для подсчета нового хэша;
typedef   tERROR (pr_call *fnpHash_Update)  ( hi_Hash _this, tBYTE* pData, tDWORD dwSize ); // -- Обсчитать очередной блок данных;
typedef   tERROR (pr_call *fnpHash_GetHash) ( hi_Hash _this, tBYTE* pHash, tDWORD dwSize ); // -- Взять значание хеша для обработанных данных;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iHashVtbl {
	fnpHash_Reset    Reset;
	fnpHash_Update   Update;
	fnpHash_GetHash  GetHash;
}; // "Hash" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Hash_Reset( hi_Hash _this );
tERROR pr_call Hash_Update( hi_Hash _this, tBYTE* pData, tDWORD dwSize );
tERROR pr_call Hash_GetHash( hi_Hash _this, tBYTE* pHash, tDWORD dwSize );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iHashVtbl e_Hash_vtbl = {
	(fnpHash_Reset)           Hash_Reset,
	(fnpHash_Update)          Hash_Update,
	(fnpHash_GetHash)         Hash_GetHash
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Hash". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Hash_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_hash_md5.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Hash_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Hash_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Calculates MD5 hash for data", 29 )
	mLOCAL_PROPERTY_BUF( pgHASH_INIT_VALUE, sLocals, HashInitValue, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgHASH_SIZE, sLocals, HashSize, cPROP_BUFFER_READ )
mPROPERTY_TABLE_END(Hash_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Hash". Register function
tERROR pr_call Hash_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter Hash::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_HASH,                               // interface identifier
		PID_HASH_MD5,                           // plugin identifier
		0x00000000,                             // subtype identifier
		Hash_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Hash_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Hash_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Hash_vtbl,                           // external function table
		(sizeof(e_Hash_vtbl)/sizeof(tPTR)),     // external function table size
		Hash_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Hash_PropTable),   // property table size
		sizeof(sLocals),                        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Hash(IID_HASH) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave Hash::Register method, ret tERROR = 0x%08x", error );
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
tERROR pr_call Hash_ObjectInit( hi_Hash _this )
{
	tERROR error;
	PR_TRACE_A0( _this, "Enter Hash::ObjectInit method" );

	_this->data->HashSize = 128;
	_this->data->bBadInternalState = cTRUE;

	error = CALL_SYS_ObjHeapAlloc(_this, &_this->data->pCtx, hash_api_md5.ContextSize);
	if (PR_SUCC(error))
		error = Hash_Reset(_this);

	PR_TRACE_A1( _this, "Leave Hash::ObjectInit method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Hash_ObjectInitDone( hi_Hash _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Hash::ObjectInitDone method" );

	_this->data->bBadInternalState = cFALSE;

	PR_TRACE_A1( _this, "Leave Hash::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call Hash_ObjectPreClose( hi_Hash _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Hash::ObjectPreClose method" );

	_this->data->bBadInternalState = cTRUE;
	if (_this->data->pCtx)
		CALL_SYS_ObjHeapFree(_this, _this->data->pCtx);

	PR_TRACE_A1( _this, "Leave Hash::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



#define COUNT_OF_BYTES_FROM_BITS(bits)	( (bits & 7) ? (bits/8 + 1) : (bits/8) )

// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// Behaviour comment
//    Сбрасывает состояние объекта и подготавливает его к новому обсчету.
// Parameters are:
tERROR pr_call Hash_Reset( hi_Hash _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Hash::Reset method" );

	if (!hash_api_md5.Init(0, _this->data->pCtx, hash_api_md5.ContextSize, COUNT_OF_BYTES_FROM_BITS(_this->data->HashSize), NULL))
	{
		error = errUNEXPECTED;
		_this->data->bBadInternalState = cTRUE;
	}
	else
	{
		_this->data->bBadInternalState = cFALSE;
	}

	PR_TRACE_A1( _this, "Leave Hash::Reset method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Update )
// Behaviour comment
//    Последовательными вызовами метода Update производится обсчет всего потока данных. Для извлечения промежуточного (между блоками) или конечного значения хеша нужно воспользоваться функцией GetHash.
// Parameters are:
//   "dwSize" : Указывается в байтах
tERROR pr_call Hash_Update( hi_Hash _this, tBYTE* pData, tDWORD dwSize )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Hash::Update method" );

	if (_this->data->bBadInternalState)
		return errOBJECT_BAD_INTERNAL_STATE;
	
	if (pData == NULL)
		return errPARAMETER_INVALID;

	if (!hash_api_md5.Update(_this->data->pCtx, pData, dwSize))
	{
		error = errUNEXPECTED;
		_this->data->bBadInternalState = cTRUE;
	}

	PR_TRACE_A1( _this, "Leave Hash::Update method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetHash )
// Behaviour comment
//    Имплементация по возможности должна позволять получать как конечные, так и промежуточные значения хеша (Reset, Update, GetHash, Update, GetHash, etc...). Если имплементация позволяет получать только конечные значения, между вызовами методов GetHash и Reset метод Update должен возвращать errOBJECT_BAD_INTERNAL_STATE.
// Parameters are:
//   "dwSize" : Указывается в байтах
tERROR pr_call Hash_GetHash( hi_Hash _this, tBYTE* pHash, tDWORD dwSize )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Hash::GetHash method" );

	if (_this->data->bBadInternalState)
		return errOBJECT_BAD_INTERNAL_STATE;

	if (pHash == NULL)
		return errPARAMETER_INVALID;

	if (COUNT_OF_BYTES_FROM_BITS(_this->data->HashSize) > dwSize)
		return errBUFFER_TOO_SMALL;

	if (!hash_api_md5.GetCurrentHash(_this->data->pCtx, (unsigned long*)&dwSize, pHash))
	{
		error = errUNEXPECTED;
		_this->data->bBadInternalState = cTRUE;
	}

	PR_TRACE_A1( _this, "Leave Hash::GetHash method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



