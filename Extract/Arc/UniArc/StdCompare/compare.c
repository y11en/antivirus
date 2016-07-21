// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  31 October 2002,  13:24 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Prague
// Sub project -- StdCompare
// Purpose     -- Standard Compare Interfaces
// Author      -- mike
// File Name   -- compare.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Compare_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "compare.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Compare interface implementation
// Short comments -- compare interface
// Extended comment -
//  Used as user defined interface in TREE and HDB interfaces for sorting and searching. May be implemented as static ( witout any settings or as normal interface
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Compare. Inner data structure


// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Compare {
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
struct iCompareVtbl {
	fnpCompare_Compare  Compare;
}; // "Compare" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Compare_Compare( hi_Compare _this, tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iCompareVtbl e_Compare_vtbl = {
	Compare_Compare
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Compare_PROP_ReadPropNotImpl( hi_Compare _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Compare_PROP_WritePropNotImpl( hi_Compare _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Compare". Static(shared) property table variables
// Interface "Compare". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Compare_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Compare_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Standard Compare tQWORD", 24 )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Compare_VERSION )
	mLOCAL_PROPERTY_FN( pgCOMPARE_SETTINGS, Compare_PROP_ReadPropNotImpl, Compare_PROP_WritePropNotImpl )
mPROPERTY_TABLE_END(Compare_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Compare". Register function
tERROR pr_call Compare_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"Compare::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_COMPARE,                            // interface identifier
		PID_STDCOMPARE,                         // plugin identifier
		SUBTYPE_COMPARE_QWORD,                  // subtype identifier
		Compare_VERSION,                        // interface version
		VID_MIKE,                               // interface developer
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

	PR_TRACE_A1( root, "Leave \"Compare::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, ReadPropNotImpl )
// Interface "Compare". Method "Get" for property(s):
//  -- COMPARE_SETTINGS
tERROR pr_call Compare_PROP_ReadPropNotImpl( hi_Compare _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errPROPERTY_NOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter *GET* method Compare_PROP_ReadPropNotImpl for property pgCOMPARE_SETTINGS" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *GET* method Compare_PROP_ReadPropNotImpl for property pgCOMPARE_SETTINGS, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, WritePropNotImpl )
// Interface "Compare". Method "Set" for property(s):
//  -- COMPARE_SETTINGS
tERROR pr_call Compare_PROP_WritePropNotImpl( hi_Compare _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errPROPERTY_NOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter *SET* method Compare_PROP_WritePropNotImpl for property pgCOMPARE_SETTINGS" );

	*out_size = 0;

	PR_TRACE_A2( _this, "Leave *SET* method Compare_PROP_WritePropNotImpl for property pgCOMPARE_SETTINGS, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Compare )
// Extended method comment
//  compare two raw data blocks.
// Result comment
//  one of following constants: cCOMPARE_LESS ( data1 less then data2), cCOMPARE_EQUIL (data1 and data2 is equil), cCOMPARE_GREATER (data1 greater then data2)
// Parameters are:
tERROR pr_call Compare_Compare( hi_Compare _this, tINT* result, tPTR data1, tDWORD size1, tPTR data2, tDWORD size2 )
{
	tERROR error = errOK;
	tINT   ret_val = cCOMPARE_EQUIL;
	PR_TRACE_A0( _this, "Enter \"Compare::Compare\" method" );

	if (size1 < sizeof(tQWORD) || size2 < sizeof(tQWORD))
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



