// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  23 October 2001,  16:20 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2001.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- csection.c
// Additional info
//  This implementation finds loadable prague plugins in disk folder
// AVP Prague stamp end


//#define _CHECK_CS

#if defined (_WIN32)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define CriticalSection_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "csection.h"
#include <wincompat.h>
// AVP Prague stamp end


extern tBOOL g_2l_cs;
extern BOOL (*g_fTryEnterCriticalSection)(LPCRITICAL_SECTION);   // NT TryEnter function pointer

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// CriticalSection interface implementation
// Short comments -- Critical section behaviour
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface CriticalSection. Inner data structure

typedef struct tag_CSectData {
	CRITICAL_SECTION cs;          // critical section Windows object
	tDATA            caller_addr;
	tDATA            free_addr;
} CSectData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_CriticalSection {
	const iCriticalSectionVtbl* vtbl; // pointer to the "CriticalSection" virtual table
	const iSYSTEMVTBL*          sys;  // pointer to the "SYSTEM" virtual table
	CSectData*                  data; // pointer to the "CriticalSection" data structure
} *hi_CriticalSection;


// AVP Prague stamp end

#define CS_TRY
#define CS_EXCEPT

//#define CS_TRY     __try
//#define CS_EXCEPT  __except( EXCEPTION_EXECUTE_HANDLER ) { error = errUNEXPECTED; }

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call CriticalSection_ObjectInitDone( hi_CriticalSection _this );
tERROR pr_call CriticalSection_ObjectClose( hi_CriticalSection _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_CriticalSection_vtbl = {
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   CriticalSection_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      CriticalSection_ObjectClose,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpCriticalSection_Enter)    ( hi_CriticalSection _this, tSHARE_LEVEL level );   // -- locks synchronization object;
typedef   tERROR (pr_call *fnpCriticalSection_Leave)    ( hi_CriticalSection _this, tSHARE_LEVEL* plevel ); // -- unlocks synchronization object;
typedef   tERROR (pr_call *fnpCriticalSection_TryEnter) ( hi_CriticalSection _this, tSHARE_LEVEL level );   // -- try enter critical section, but do not block a calling thread;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iCriticalSectionVtbl {
	fnpCriticalSection_Enter     Enter;
	fnpCriticalSection_Leave     Leave;
	fnpCriticalSection_TryEnter  TryEnter;
}; // "CriticalSection" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level );
tERROR pr_call CriticalSection_Leave( hi_CriticalSection _this, tSHARE_LEVEL* plevel );
tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iCriticalSectionVtbl e_CriticalSection_vtbl = {
	CriticalSection_Enter,
	CriticalSection_Leave,
	CriticalSection_TryEnter
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "CriticalSection". Global(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// Interface "CriticalSection". Property table
mPROPERTY_TABLE(CriticalSection_PropTable)
	mSHARED_PROPERTY( pgSHARE_LEVEL_SUPPORT, cFALSE )
	mSHARED_PROPERTY( pgINTERFACE_VERSION, 1 )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "critical section object", 24 )
	mLOCAL_PROPERTY_BUF( pgOWNING_THREAD, CSectData, cs.OwningThread, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgOWNING_COUNT,  CSectData, cs.RecursionCount, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgCALLER_ADDR,   CSectData, caller_addr, cPROP_BUFFER_READ )
mPROPERTY_TABLE_END(CriticalSection_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "CriticalSection". Register function
tERROR pr_call CriticalSection_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"CriticalSection::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_CRITICAL_SECTION,                   // interface identifier
		PID_WIN32_PLUGIN_LOADER,                // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,                             // interface version
		VID_PETROVITCH,                         // interface developer
		&i_CriticalSection_vtbl,                // internal(kernel called) function table
		(sizeof(i_CriticalSection_vtbl)/sizeof(tPTR)),// internal function table size
		&e_CriticalSection_vtbl,                // external function table
		(sizeof(e_CriticalSection_vtbl)/sizeof(tPTR)),// external function table size
		CriticalSection_PropTable,              // property table
		mPROPERTY_TABLE_SIZE(CriticalSection_PropTable),// property table size
		sizeof(CSectData),                      // memory size
    g_2l_cs ? 0 : IFACE_SYSTEM              // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ldr\tCriticalSection(IID_CRITICAL_SECTION) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"CriticalSection::Register\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call CriticalSection_ObjectInitDone( hi_CriticalSection _this ) {

	CSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"CriticalSection::ObjectInitDone\" method" );

	CS_TRY {
		d = _this->data;
		InitializeCriticalSection( &d->cs );
		error = errOK;
	}
	CS_EXCEPT;
	
	PR_TRACE_A1( _this, "Leave \"CriticalSection::ObjectInitDone\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call CriticalSection_ObjectClose( hi_CriticalSection _this ) {

	tERROR error;
	PR_TRACE_A0( _this, "Enter \"CriticalSection::ObjectClose\" method" );

	if ( CALL_SYS_PropertyGetBool(_this,psOBJECT_OPERATIONAL) ) {
		CS_TRY {
			CSectData* d = _this->data;
			DeleteCriticalSection( &d->cs );
			memset( d, 0, sizeof(CSectData) );
		}
		CS_EXCEPT;
	}
	error = errOK;

	PR_TRACE_A1( _this, "Leave \"CriticalSection::ObjectClose\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


tERROR GetCallerId( tPTR stack_ptr, tDATA* id, tDWORD size );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Enter )
// Parameters are:
// tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this ) {
tERROR pr_call CriticalSection_Enter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
	CSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"CriticalSection::Enter\" method" );

	CS_TRY {
//    #ifdef _DEBUG
//      tDWORD thread_id;
//    #endif

    d = _this->data;

    #ifdef _DEBUG
//			thread_id = GetCurrentThreadId();
//      if ( d->cs.RecursionCount && ((tDWORD)d->cs.OwningThread != thread_id) ) {
//        d->cs.RecursionCount = d->cs.RecursionCount;
//      }
    #endif

		EnterCriticalSection( &d->cs );
    
    #if defined(_DEBUG) || defined(_CHECK_CS)
			if ( d->cs.RecursionCount == 1 )
				GetCallerId( &_this, &d->caller_addr, sizeof(tDATA) );
    #endif

    error = errOK;
  }
	CS_EXCEPT;

	PR_TRACE_A1( _this, "Leave \"CriticalSection::Enter\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Leave )
// Parameters are:
tERROR pr_call CriticalSection_Leave( hi_CriticalSection _this, tSHARE_LEVEL* plevel ) {
	CSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"CriticalSection::Leave\" method" );

	CS_TRY {
		d = _this->data;
    #if defined(_DEBUG) || defined(_CHECK_CS)
      if ( !d->cs.RecursionCount )
        d->cs.RecursionCount = d->cs.RecursionCount;
      if ( d->cs.RecursionCount == 1 ) {
				tDWORD thread = GetCurrentThreadId();
				if ( (tDWORD)d->cs.OwningThread != thread )
					d->cs.RecursionCount = d->cs.RecursionCount;
				else
				{
					GetCallerId( &_this, &d->free_addr, sizeof(tDATA) );
					d->caller_addr = -1;
				}
			}
    #endif
		LeaveCriticalSection( &d->cs );
		error = errOK;
	}
	CS_EXCEPT;
		
  if ( plevel )
    *plevel = SHARE_LEVEL_WRITE;

	PR_TRACE_A1( _this, "Leave \"CriticalSection::Leave\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TryEnter )
// Parameters are:
// tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this ) {
tERROR pr_call CriticalSection_TryEnter( hi_CriticalSection _this, tSHARE_LEVEL level ) {
	CSectData* d;
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"CriticalSection::TryEnter\" method" );

	CS_TRY {
		d = _this->data;
		if ( g_fTryEnterCriticalSection && g_fTryEnterCriticalSection(&d->cs) )
			error = errOK;
		else
			error = errNOT_OK;
	}
	CS_EXCEPT;
		
	PR_TRACE_A1( _this, "Leave \"CriticalSection::TryEnter\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


#endif //_WIN32
