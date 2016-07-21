// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  07 October 2002,  13:37 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- hash Container
// Sub project -- Compare Plugin
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- compare.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Compare_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "compare.h"

#include <Prague/prague.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Compare interface implementation
// Short comments -- compare interface
//    Used as user defined interface in TREE and HDB interfaces for sorting and searching. May be implemented as static ( witout any settings or as normal interface
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Compare. Inner data structure


// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Compare 
{
	const iCompareVtbl* vtbl; // pointer to the "Compare" virtual table
	const iSYSTEMVTBL*  sys;  // pointer to the "SYSTEM" virtual table
} *hi_Compare;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpCompare_Compare) ( hi_Compare _this, tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 ); // -- Compare two raw data blocks;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iCompareVtbl 
{
	fnpCompare_Compare  Compare;
}; // "Compare" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Compare_Compare( hi_Compare _this, tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iCompareVtbl e_Compare_vtbl = 
{
	Compare_Compare
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Compare". Static(shared) property table variables
const tVERSION zVersion = 1; // must be READ_ONLY at runtime
const tSTRING szComment = "Hash Container Compare Plugin"; // must be READ_ONLY at runtime
const tBYTE Setting[] = { 0x00 }; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Compare_PropTable)
	mSHARED_PROPERTY_VAR( pgINTERFACE_VERSION, zVersion, sizeof(zVersion) )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, szComment, 6 )
	mSHARED_PROPERTY_VAR( pgCOMPARE_SETTINGS, Setting, sizeof(Setting) )
mPROPERTY_TABLE_END(Compare_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Compare". Register function
tERROR pr_call Compare_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter Compare::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_COMPARE,                            // interface identifier
		PID_HASHCONT_COMPARE,                   // plugin identifier
		0x00000000,                             // subtype identifier
		zVersion,                               // interface version
		VID_ANDY,                               // interface developer
		NULL,                                   // internal(kernel called) function table
		0,                                      // internal function table size
		&e_Compare_vtbl,                        // external function table
		(sizeof(e_Compare_vtbl)/sizeof(tPTR)),  // external function table size
		Compare_PropTable,                      // property table
		mPROPERTY_TABLE_SIZE(Compare_PropTable),// property table size
		0,                                      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Compare(IID_COMPARE) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave Compare::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Compare )
// Extended method comment
//    compare two raw data blocks.
// Result comment
//    one of following constants: cCOMPARE_LESS ( data1 less then data2), cCOMPARE_EQUIL (data1 and data2 is equil), cCOMPARE_GREATER (data1 greater then data2)
// Parameters are:
tERROR pr_call Compare_Compare( hi_Compare _this, tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 ) 
{
tERROR error = errOK;
tINT   ret_val = cCOMPARE_EQUIL;

	PR_TRACE_A0( _this, "Enter \"Compare::Compare\" method" );
	
	if(result==NULL)
		return errPARAMETER_INVALID;

	if (size1 < (sizeof(tQWORD) )  || size2 < (sizeof(tQWORD)))
		return errPARAMETER_INVALID;
	
	if (*(tQWORD*)data1 > *(tQWORD*)data2)
		ret_val = cCOMPARE_GREATER;

	else if (*(tQWORD*)data1 < *(tQWORD*)data2)
		ret_val = cCOMPARE_LESS;
	
	if ( result )
		*result = ret_val;
	
	PR_TRACE_A2( _this, "Leave \"Compare::Compare\" method, ret tINT = %d, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end



