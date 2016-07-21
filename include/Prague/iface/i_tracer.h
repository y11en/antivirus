// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  26 October 2006,  17:41 --------
// File Name   -- (null)i_tracer.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_tracer__37b137fa_ae7b_11d4_b757_00d0b7170e50 )
#define __i_tracer__37b137fa_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_receiv.h>
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Tracer interface implementation
// Short comments -- output debug/tracer information
//    Interface to output debug/tracer information. To overwrite it just register interface with the same IID and PID but with newer version (2 at least) before Root::StartTrace call
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_TRACER  ((tIID)(4))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
typedef tFUNC_PTR                      tOUTPUT_FUNC; // output function prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hTRACER;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iTracerVtbl;
typedef struct iTracerVtbl iTracerVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cTracer;
	typedef cTracer* hTRACER;
#else
	/*typedef*/ struct tag_hTRACER {
		const iTracerVtbl* vtbl; // pointer to the "Tracer" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	};// *hTRACER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Tracer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpTracer_Trace)         ( hTRACER _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, ... ); // -- outputs one line to trace log;
	typedef   tERROR (pr_call *fnpTracer_TraceV)        ( hTRACER _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, tVOID* arg_list ); // -- outputs one line to trace log;
	typedef   tERROR (pr_call *fnpTracer_Start)         ( hTRACER _this, tDWORD* result );                    // -- starts tracing process;
	typedef   tERROR (pr_call *fnpTracer_Stop)          ( hTRACER _this, tDWORD* result );                    // -- ;
	typedef   tERROR (pr_call *fnpTracer_IsRunning)     ( hTRACER _this );                    // -- returns if tracer in running mode;
	typedef   tERROR (pr_call *fnpTracer_TraceLevelSet) ( hTRACER _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tVID vid, tDWORD level, tDWORD min_level ); // -- set trace level by interface or/and by pid;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iTracerVtbl {
	fnpTracer_Trace          Trace;
	fnpTracer_TraceV         TraceV;
	fnpTracer_Start          Start;
	fnpTracer_Stop           Stop;
	fnpTracer_IsRunning      IsRunning;
	fnpTracer_TraceLevelSet  TraceLevelSet;
}; // "Tracer" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Tracer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION         mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT         mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgOUTPUT_FUNC               mPROPERTY_MAKE_GLOBAL( pTYPE_FUNC_PTR, 0x00001000 )
#define pgRESERVE_TRACE_BUFFER_SIZE mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001001 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Tracer_Trace( _this, obj, level, format )                                         ((_this)->vtbl->Trace( (_this), obj, level, format ))
	#define CALL_Tracer_TraceV( _this, obj, level, format, arg_list )                                   ((_this)->vtbl->TraceV( (_this), obj, level, format, arg_list ))
	#define CALL_Tracer_Start( _this, result )                                                          ((_this)->vtbl->Start( (_this), result ))
	#define CALL_Tracer_Stop( _this, result )                                                           ((_this)->vtbl->Stop( (_this), result ))
	#define CALL_Tracer_IsRunning( _this )                                                              ((_this)->vtbl->IsRunning( (_this) ))
	#define CALL_Tracer_TraceLevelSet( _this, op, iid, pid, subtype, vid, level, min_level )            ((_this)->vtbl->TraceLevelSet( (_this), op, iid, pid, subtype, vid, level, min_level ))
#else // if !defined( __cplusplus )
	#define CALL_Tracer_Trace( _this, obj, level, format )                                         ((_this)->Trace( obj, level, format ))
	#define CALL_Tracer_TraceV( _this, obj, level, format, arg_list )                                   ((_this)->TraceV( obj, level, format, arg_list ))
	#define CALL_Tracer_Start( _this, result )                                                          ((_this)->Start( result ))
	#define CALL_Tracer_Stop( _this, result )                                                           ((_this)->Stop( result ))
	#define CALL_Tracer_IsRunning( _this )                                                              ((_this)->IsRunning( ))
	#define CALL_Tracer_TraceLevelSet( _this, op, iid, pid, subtype, vid, level, min_level )            ((_this)->TraceLevelSet( op, iid, pid, subtype, vid, level, min_level ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iTracer {
		virtual tERROR pr_call Trace( hOBJECT obj, tTRACE_LEVEL level, tSTRING format, ... ) = 0; // -- outputs one line to trace log
		virtual tERROR pr_call TraceV( hOBJECT obj, tTRACE_LEVEL level, tSTRING format, tVOID* arg_list ) = 0; // -- outputs one line to trace log
		virtual tERROR pr_call Start( tDWORD* result ) = 0; // -- starts tracing process
		virtual tERROR pr_call Stop( tDWORD* result ) = 0;
		virtual tERROR pr_call IsRunning() = 0; // -- returns if tracer in running mode
		virtual tERROR pr_call TraceLevelSet( tDWORD op, tIID iid, tPID pid, tDWORD subtype, tVID vid, tDWORD level, tDWORD min_level ) = 0; // -- set trace level by interface or/and by pid
	};

	struct pr_abstract cTracer : public iTracer, public iObj {

		OBJ_IMPL( cTracer );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hTRACER()   { return (hTRACER)this; }

		tFUNC_PTR pr_call get_pgOUTPUT_FUNC() { return (tFUNC_PTR)getPtr(pgOUTPUT_FUNC); }
		tERROR pr_call set_pgOUTPUT_FUNC( tFUNC_PTR value ) { return setPtr(pgOUTPUT_FUNC,(tPTR)value); }

		tDWORD pr_call get_pgRESERVE_TRACE_BUFFER_SIZE() { return (tDWORD)getDWord(pgRESERVE_TRACE_BUFFER_SIZE); }
		tERROR pr_call set_pgRESERVE_TRACE_BUFFER_SIZE( tDWORD value ) { return setDWord(pgRESERVE_TRACE_BUFFER_SIZE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_tracer__37b137fa_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



