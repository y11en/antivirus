// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  06 April 2005,  15:53 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин распаковки данных, упакованных методом Deflate
// Author      -- Dmitry Glavatskikh
// File Name   -- plugin_inflate.c
// Additional info
//    В этом плагине реализована поддержка как стандартного потока Deflate, так и специальной модификации для программы упаковки исполняемых файлов Petite
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_inflate )
#define __public_plugin_inflate
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Extract/iface/i_transformer.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_INFLATE  ((tPID)(63034))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_ANDY  ((tVID)(65))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Transformer )
// Transformer interface implementation
// Short comments -- Generic convertion interface

// constants
#define INFLATE_RESET_STATE            ((tINT)(0)) //  --
#define INFLATE_RESET_FULL             ((tINT)(1)) //  --
#define INFLATE_RESET_STATEIGNOREWND   ((tINT)(2)) //  --

// properties
#define plINFLATE_PETITE_FLAG      mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002000 )
#define plINFLATE_WINDOW_SIZE      mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plINFLATE_WINDOW           mPROPERTY_MAKE_LOCAL( pTYPE_PTR     , 0x00002002 )
#define plINFLATE_WINDOW_MULTIPIER mPROPERTY_MAKE_LOCAL( pTYPE_INT     , 0x00002003 )
#define plINFLATE_INF64_FLAG       mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002004 )
#define plINFLATE_NSIS_FLAG        mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002005 )
#define plINFLATE_ZLIBCHECK_FLAG   mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002006 )

// plugin interface
typedef   tERROR (pr_call *fnpInflateTransformer_ProcessEx)( hTRANSFORMER _this, tQWORD* Written, tQWORD* Readed ); // -- ;

typedef struct tag_iInflateTransformerVtbl {
	struct iTransformerVtbl prt;
	fnpInflateTransformer_ProcessEx  ProcessEx;
} iInflateTransformerVtbl;

typedef struct tag_InflateTransformer {
	const iInflateTransformerVtbl* vtbl; // pointer to the "InflateTransformer" virtual table
	const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
} *hInflateTransformer;

#define CALL_InflateTransformer_ProcessEx( _this, Written, Readed )                 ((_this)->vtbl->ProcessEx( (hTRANSFORMER)(_this), Written, Readed ))

#if defined (__cplusplus)
struct cInflateTransformer : public cTransformer {
	virtual tERROR pr_call ProcessEx( tQWORD* Written, tQWORD* Readed ) = 0;

	operator hOBJECT() { return (hOBJECT)this; }
	operator hInflateTransformer()   { return (hInflateTransformer)this; }

	tBOOL pr_call get_plINFLATE_PETITE_FLAG() { return (tBOOL)getBool(plINFLATE_PETITE_FLAG); }
	tERROR pr_call set_plINFLATE_PETITE_FLAG( tBOOL value ) { return setBool(plINFLATE_PETITE_FLAG,(tBOOL)value); }

	tDWORD pr_call get_plINFLATE_WINDOW_SIZE() { return (tDWORD)getDWord(plINFLATE_WINDOW_SIZE); }
	tERROR pr_call set_plINFLATE_WINDOW_SIZE( tDWORD value ) { return setDWord(plINFLATE_WINDOW_SIZE,(tDWORD)value); }

	tPTR pr_call get_plINFLATE_WINDOW() { return (tPTR)getPtr(plINFLATE_WINDOW); }
	tERROR pr_call set_plINFLATE_WINDOW( tPTR value ) { return setPtr(plINFLATE_WINDOW,(tPTR)value); }

	tINT pr_call get_plINFLATE_WINDOW_MULTIPIER() { return (tINT)getUInt(plINFLATE_WINDOW_MULTIPIER); }
	tERROR pr_call set_plINFLATE_WINDOW_MULTIPIER( tINT value ) { return setUInt(plINFLATE_WINDOW_MULTIPIER,(tUINT)value); }

	tBOOL pr_call get_plINFLATE_INF64_FLAG() { return (tBOOL)getBool(plINFLATE_INF64_FLAG); }
	tERROR pr_call set_plINFLATE_INF64_FLAG( tBOOL value ) { return setBool(plINFLATE_INF64_FLAG,(tBOOL)value); }

	tBOOL pr_call get_plINFLATE_NSIS_FLAG() { return (tBOOL)getBool(plINFLATE_NSIS_FLAG); }
	tERROR pr_call set_plINFLATE_NSIS_FLAG( tBOOL value ) { return setBool(plINFLATE_NSIS_FLAG,(tBOOL)value); }

	tBOOL pr_call get_plINFLATE_ZLIBCHECK_FLAG() { return (tBOOL)getBool(plINFLATE_ZLIBCHECK_FLAG); }
	tERROR pr_call set_plINFLATE_ZLIBCHECK_FLAG( tBOOL value ) { return setBool(plINFLATE_ZLIBCHECK_FLAG,(tBOOL)value); }

};

#endif // if defined (__cplusplus)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_inflate
// AVP Prague stamp end



