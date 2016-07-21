// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  27 October 2006,  11:54 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- k_trace.c
// Additional info
//    Kernel interface implementations
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __k_trace_c__89b3245c_a701_402f_95c5_8adb3cb156a4 )
#define __k_trace_c__89b3245c_a701_402f_95c5_8adb3cb156a4
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
#define PID_KERNEL  ((tPID)(1))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// --- e3431081_608f_11d4_8b36_a0794cc10000 ---
// Tracer interface implementation
// Extended comment -
// Short comments -- output debug/tracer information
// Extended comment
//   Interface to output debug/tracer information. To overwrite it just register interface with the same IID and PID but with newer version (2 at least) before Root::StartTrace call
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	EXTERN_C tERROR pr_call Tracer_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Tracer. Inner data structure

//! #if defined( __cplusplus )
//! extern "C" {
//! #endif
//!
typedef struct tag_TraceData {
	tUINT             running;     // running mode
	tOUTPUT_FUNC      output_func; // pointer to function to output string
	tDWORD       output_buff_size; // minimum size of output buffer
	//tUINT             locked;      // tracer is locked (recursion)
	//hCRITICAL_SECTION cs;          // --
	//tCHAR             buffer[0x400]; // buffer to prepare output string
} TraceData;


// to get pointer to the structure TraceData from the hOBJECT obj
//! #if defined( __cplusplus )
//! }
//! #endif
//!
  // data structure "TraceData" is described in "D:\Prague\Kernel\k_trace.c" source file
#define CUST_TO_TraceData(object)  ((TraceData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Tracer {
	const iTracerVtbl* vtbl; // pointer to the "Tracer" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	TraceData*         data; // pointer to the "Tracer" data structure
} *hi_Tracer;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // k_trace_c
// AVP Prague stamp end



