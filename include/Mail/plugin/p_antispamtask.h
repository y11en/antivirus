// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  06 May 2005,  13:51 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- plugin_antispamtask.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_antispamtask )
#define __public_plugin_antispamtask
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Mail/iface/i_antispamfilter.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_ANTISPAMTASK  ((tPID)(40021))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_DENISOV  ((tVID)(88))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, AntispamFilter )
// AntispamFilter interface implementation
// Short comments --

// properties
#define plTASK_PERSISTANCE_STORAGE mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )

// message class
//! #define pmc_ANTISPAM_TRAIN 0x7c47bd9d //

// message class
//! #define pmc_ANTISPAM_PROCESS 0x7b85cef0 //

// message class
//! #define pmc_ANTISPAM_REPORT 0x3e006bbe //

// plugin interface
typedef   tERROR (pr_call *fnpAntispamTaskAntispamFilter_Init)( hANTISPAMFILTER _this, const cSerializable* p_pSettings ); // -- ;

typedef struct tag_iAntispamTaskAntispamFilterVtbl {
	struct iAntispamFilterVtbl prt;
	fnpAntispamTaskAntispamFilter_Init  Init;
} iAntispamTaskAntispamFilterVtbl;

typedef struct tag_AntispamTaskAntispamFilter {
	const iAntispamTaskAntispamFilterVtbl* vtbl; // pointer to the "AntispamTaskAntispamFilter" virtual table
	const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
} *hAntispamTaskAntispamFilter;

#define CALL_AntispamTaskAntispamFilter_Init( _this, p_pSettings )                    ((_this)->vtbl->Init( (_this), p_pSettings ))

#if defined (__cplusplus)
struct cAntispamTaskAntispamFilter : public cAntispamFilter {
	virtual tERROR pr_call Init( const cSerializable* p_pSettings ) = 0;

	operator hOBJECT() { return (hOBJECT)this; }
	operator hAntispamTaskAntispamFilter()   { return (hAntispamTaskAntispamFilter)this; }

	hOBJECT pr_call get_plTASK_PERSISTANCE_STORAGE() { return (hOBJECT)getObj(plTASK_PERSISTANCE_STORAGE); }
	tERROR pr_call set_plTASK_PERSISTANCE_STORAGE( hOBJECT value ) { return setObj(plTASK_PERSISTANCE_STORAGE,(hOBJECT)value); }

};

#endif // if defined (__cplusplus)
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, TaskEx )
// TaskEx interface implementation
// Short comments --
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_antispamtask
// AVP Prague stamp end



