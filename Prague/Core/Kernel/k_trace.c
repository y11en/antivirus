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
// AVP Prague stamp begin( Interface version,  )
#define Tracer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Tracer_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
//! #include <prague.h>
#include "k_trace.h"
// AVP Prague stamp end



#include "kernel.h"
#include <stdarg.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktrace.h>


// ---
typedef struct tag_hi_Root *hi_Root;
tERROR pr_call Root_RegisterIFace( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tPTR internal_table, tDWORD internal_table_size, tPTR external_table, tDWORD external_table_size, tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags );


#define TRACE_BUFFER_LENGTH 0x2800


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Tracer. Inner data structure
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

//#if defined( __cplusplus )
//extern "C" {
//#endif
//
//typedef struct tag_hi_Tracer {
//	const iTracerVtbl* vtbl; // pointer to the "Tracer" virtual table
//	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
//	TraceData*         data; // pointer to the "Tracer" data structure
//} *hi_Tracer;
//
//#if defined( __cplusplus )
//}
//#endif
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Tracer_ObjectInit( hi_Tracer _this );
tERROR pr_call Tracer_ObjectInitDone( hi_Tracer _this );
tERROR pr_call Tracer_ObjectPreClose( hi_Tracer _this );
tERROR pr_call Tracer_ObjectClose( hi_Tracer _this );
// Interface "Tracer". Internal method table. Forward declarations
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Tracer_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        Tracer_ObjectInit,
	(tIntFnObjectInitDone)    Tracer_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Tracer_ObjectPreClose,
	(tIntFnObjectClose)       Tracer_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpTracer_Trace)         ( hi_Tracer _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, ... ); // -- outputs one line to trace log;
typedef   tERROR (pr_call *fnpTracer_TraceV)        ( hi_Tracer _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, tVOID* arg_list ); // -- outputs one line to trace log;
typedef   tERROR (pr_call *fnpTracer_Start)         ( hi_Tracer _this, tDWORD* result );                              // -- starts tracing process;
typedef   tERROR (pr_call *fnpTracer_Stop)          ( hi_Tracer _this, tDWORD* result );                              // -- ;
typedef   tERROR (pr_call *fnpTracer_IsRunning)     ( hi_Tracer _this );                              // -- returns if tracer in running mode;
typedef   tERROR (pr_call *fnpTracer_TraceLevelSet) ( hi_Tracer _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tVID vid, tDWORD level, tDWORD min_level ); // -- set trace level by interface or/and by pid;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iTracerVtbl  {
  fnpTracer_Trace          Trace;
  fnpTracer_TraceV         TraceV;
  fnpTracer_Start          Start;
  fnpTracer_Stop           Stop;
  fnpTracer_IsRunning      IsRunning;
  fnpTracer_TraceLevelSet  TraceLevelSet;
}; // Tracer
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// Interface "Tracer". External method table. Forward declarations
tERROR pr_call Tracer_Trace( hi_Tracer _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, ... );
tERROR pr_call Tracer_TraceV( hi_Tracer _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, tVOID* arg_list );
tERROR pr_call Tracer_Start( hi_Tracer _this, tDWORD* result );
tERROR pr_call Tracer_Stop( hi_Tracer _this, tDWORD* result );
tERROR pr_call Tracer_IsRunning( hi_Tracer _this );
tERROR pr_call Tracer_TraceLevelSet( hi_Tracer _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tVID vid, tDWORD level, tDWORD min_level );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iTracerVtbl e_Tracer_vtbl = {
	(fnpTracer_Trace)         Tracer_Trace,
	(fnpTracer_TraceV)        Tracer_TraceV,
	(fnpTracer_Start)         Tracer_Start,
	(fnpTracer_Stop)          Tracer_Stop,
	(fnpTracer_IsRunning)     Tracer_IsRunning,
	(fnpTracer_TraceLevelSet) Tracer_TraceLevelSet
};
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Tracer". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Tracer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Tracer_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "kernel tracer interface", 24 )
	mLOCAL_PROPERTY_BUF( pgOUTPUT_FUNC, TraceData, output_func, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgRESERVE_TRACE_BUFFER_SIZE, TraceData, output_buff_size, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(Tracer_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Tracer". Register function
tERROR pr_call Tracer_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter Tracer::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TRACER,                             // interface identifier
		PID_KERNEL,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Tracer_VERSION,                         // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Tracer_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Tracer_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Tracer_vtbl,                         // external function table
		(sizeof(e_Tracer_vtbl)/sizeof(tPTR)),   // external function table size
		Tracer_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Tracer_PropTable), // property table size
		sizeof(TraceData),                      // memory size
		IFACE_SYSTEM | IFACE_UNSWAPPABLE        // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE(( root, prtDANGER, "krn\tTracer(IID_TRACER) registered [%terr]", error ));
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave Tracer::Register method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// ---
tERROR pr_call _TracerInitObj( tPO* po ) {
	tERROR error;
	
	if ( g_tracer )
		return errOK;
	if ( PR_FAIL(error=CALL_SYS_ObjectCreateQuick(MakeObject(g_hRoot),&g_tracer,IID_TRACER,PID_ANY,SUBTYPE_ANY)) )
		;
	else {
		wlock(po);
		_Remove( MakeHandle(g_tracer) ); // cut it from the hierarchy
		g_hRoot->tracer = g_tracer;
		wunlock(po);
	}
	
	return error;
}



// ---
tERROR pr_call _TracerDeinitObj( tPO* po ) {
	if ( g_tracer ) {
		tERROR error;
		tHANDLE* handle;
		hOBJECT obj = (hOBJECT)g_tracer;
		g_tracer = 0;
		if ( _HC(handle,0,obj,&error) && PR_SUCC(error) ) {
			wlock(po);
			_AddToChildList( handle, g_hRoot );
			wunlock(po);
			CALL_SYS_ObjectClose( obj );
		}
	}
	return errOK;
}









// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// --- Interface "Tracer". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call Tracer_ObjectInit( hi_Tracer _this ) {
	tERROR     e;
	tHANDLE*   n;
	tHANDLE*   t;
	tPO*       po;
	TraceData* d;

	d = _this->data;
	d->output_buff_size = TRACE_BUFFER_LENGTH+1;

	enter_lf();
	
	_HC( t, po, (hOBJECT)_this, &e );
	if ( t ) {
		if ( 0 != (n=t->next) ) {
			wlock(po);
			_Remove( t );
			while( n->next ) // find last
				n = n->next;
			_AddAfter( t, n );
			wunlock(po);
		}
		runlock(po);
	}
	
	leave_lf();
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Tracer_ObjectInitDone( hi_Tracer _this ) {

	if ( 1 == CALL_SYS_PropertyGetDWord(_this,pgOBJECT_USAGE_COUNT) ) {
		tERROR     e;
		TraceData* d = _this->data;
		tHANDLE*   h;
		CALL_SYS_PropertyGet( g_root, 0, pgOUTPUT_FUNC, &d->output_func, sizeof(d->output_func) );
		if ( _HC(h,0,(hOBJECT)_this,&e) ) {
			h->trace_level.max = prtERROR;
			h->trace_level.min = prtMIN_TRACE_LEVEL;
		}
	}
	else {
		// Code for connect a new satellite for the static object
	}

	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//  Kernel warns object it is going to close all children
// Behaviour comment
//  Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call Tracer_ObjectPreClose( hi_Tracer _this ) {
	PR_TRACE_A0( _this, "Enter Tracer::ObjectPreClose method" );
	PR_TRACE_A0( _this, "Leave Tracer::ObjectPreClose method, ret errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// --- Interface "Tracer". Method "ObjectClose"
// Extended method comment
//   Kernel warns object it must be closed
// Behaviour comment
//   Object takes all necessary "before closing" actions
// Result comment
//
tERROR pr_call Tracer_ObjectClose( hi_Tracer _this ) {
	if ( 1 == CALL_SYS_PropertyGetDWord(_this,pgOBJECT_USAGE_COUNT) ) {
		TraceData* d = _this->data;
		while( d->running )
			Tracer_Stop( _this, 0 );
//		if ( d->cs ) {
//			tHANDLE* h = MakeHandle(d->cs);
//			if ( OBJ_IS_NOT_CONDEMNED(h) )
//				CALL_SYS_ObjectClose( d->cs );
//		}
	}
	else {
		Tracer_Stop( _this, 0 );
	}
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Trace )
// --- Interface "Tracer". Method "Trace"
// Parameter "obj":
//   object issued output trace string
tERROR pr_call Tracer_Trace( hi_Tracer _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, ... ) {
	TraceData* d = _this->data;
	va_list    arg;
	tCHAR      buffer[TRACE_BUFFER_LENGTH];
	tCHAR*     pbuff;
	tUINT      pbuff_size;
		
//	if ( d->locked )
//		return errNOT_OK;
	
	if ( !d->running )
		return errNOT_OK;
	
	if ( !d->output_func )
		return errNOT_OK;
	
	if ( (d->output_buff_size > TRACE_BUFFER_LENGTH) && g_kernel_heap && PR_SUCC(PrAlloc(&pbuff,d->output_buff_size)) )
		pbuff_size = d->output_buff_size;
	else {
		pbuff = buffer;
		pbuff_size = TRACE_BUFFER_LENGTH;
	}

//	if ( !d->cs && (MakeHandle(obj) != g_hRoot) ) {
//		tPO* po;
//    tUINT running = d->running;
//    enter_lf();
//    d->running = 0;
//    rlock(po);
//    _SyncCreateAvailable( (hOBJECT)_this, &d->cs );
//    runlock(po);
//		d->running = running;
//    leave_lf();
//  }
//	if ( d->cs )
//		CALL_CriticalSection_Enter( d->cs, SHARE_LEVEL_READ );
	
//	d->locked++;

	va_start( arg, format );
	if (format && *format==0x02) // binary trace
		*pbuff = 0;
	else {
		if (format && *format==0x01) // binary trace prefix
			format += 8;
		pr_vsprintf( pbuff, pbuff_size, format, VA_LIST_ADDR(arg) );
	}
	d->output_func( pbuff, obj, level, format, VA_LIST_ADDR(arg) );
	va_end( arg );

//	d->locked--;

//	if ( d->cs )
//		CALL_CriticalSection_Leave( d->cs, 0 );

	if ( pbuff != buffer )
		PrFree( pbuff );

	return errOK;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TraceV )
// Parameters are:
// "obj"      : object issued output trace string
tERROR pr_call Tracer_TraceV( hi_Tracer _this, hOBJECT obj, tTRACE_LEVEL level, tSTRING format, tVOID* arg_list ) {
	TraceData* d = _this->data;
	tCHAR      buffer[TRACE_BUFFER_LENGTH];
	tCHAR*     pbuff;
	tUINT      pbuff_size;
	
	if ( !d->running )
		return errNOT_OK;
	
	if ( !d->output_func )
		return errNOT_OK;
	
	if ( (d->output_buff_size > TRACE_BUFFER_LENGTH) && g_kernel_heap && PR_SUCC(PrAlloc(&pbuff,d->output_buff_size)) )
		pbuff_size = d->output_buff_size;
	else {
		pbuff = buffer;
		pbuff_size = TRACE_BUFFER_LENGTH;
	}

//	if ( !d->cs && (obj != (hOBJECT)g_root) ) {
//		tPO* po;
//		tUINT running = d->running;
//		enter_lf();
//		d->running = 0;
//		rlock(po);
//		_SyncCreateAvailable( (hOBJECT)_this, &d->cs );
//		runlock(po);
//		d->running = running;
//		leave_lf();
//	}

//	if ( d->cs ) {
//		CALL_CriticalSection_Enter( d->cs, SHARE_LEVEL_READ );
//		if ( d->locked ) {
//			 CALL_CriticalSection_Leave( d->cs, 0 );
//			return errNOT_OK;
//		}
//		d->locked++;
//		CALL_CriticalSection_Leave( d->cs, 0 );
//	}
//	else if ( d->locked )
//		return errNOT_OK;
//	else
//		d->locked++;
	
	if (format && *format==0x02) // binary trace
		*pbuff = 0;
	else {
		if (format && *format==0x01) // binary trace prefix
			format += 8;
		pr_vsprintf( pbuff, pbuff_size, format, arg_list );
	}
	d->output_func( pbuff, obj, level, format, arg_list );
	
//	if ( d->cs ) {
//		CALL_CriticalSection_Enter( d->cs, SHARE_LEVEL_READ );
//		d->locked--;
//		CALL_CriticalSection_Leave( d->cs, 0 );
//	}
//	else
//		d->locked--;

	if ( pbuff != buffer )
		PrFree( pbuff );

	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Start )
// --- Interface "Tracer". Method "Start"
tERROR pr_call Tracer_Start( hi_Tracer _this, tDWORD* result ) {
	TraceData* d = _this->data;
	d->running++;
	if (  1 == d->running )
		Tracer_Trace( _this, (hOBJECT)_this, prtNOTIFY, "Tracer started" );
	if ( result )
		*result = d->running;
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Stop )
// --- Interface "Tracer". Method "Stop"
tERROR pr_call Tracer_Stop( hi_Tracer _this, tDWORD* result ) {
	TraceData* d = _this->data;
	if ( d->running ) {
		if ( 1 == d->running )
			Tracer_Trace( _this, (hOBJECT)_this, prtNOTIFY, "Tracer stoped" );
		d->running--;
	}
	if ( result )
		*result = d->running;
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, IsRunning )
// --- Interface "Tracer". Method "IsRunning"
tERROR pr_call Tracer_IsRunning( hi_Tracer _this ) {
	if ( _this->data->running )
		return errOK;
	return errNOT_OK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TraceLevelSet )
// --- Interface "Tracer". Method "TraceLevelSet"
// Extended method comment
//   set trace level for all new or/and existing objects of specified interface or/and plugin
// Parameter "who":
//   who is affected by this method. Possible values see pr_sys.h tlg... constants
// Parameter "iid":
//   Includes interface id to a filter. Can be IID_ANY
// Parameter "pid":
//   Includes plugin id to a filter. Can be PID_ANY
// Parameter "min_level":
//   minimum trace level for affected instances, prtMIN_TRACE_LEVEL by default
tERROR pr_call Tracer_TraceLevelSet( hi_Tracer _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tVID vid, tDWORD level, tDWORD min_level ) {
	tERROR error;
	tPO*   po;
	PR_TRACE_A0( _this, "Enter Tracer::TraceLevelSet method" );
	enter_lf();

	rlock(po);
	error = _TraceLevelSetID( po, op, iid, pid, subtype, vid, level, min_level );
	runlock(po);

	leave_lf();
	PR_TRACE_A1( _this, "Leave Tracer::TraceLevelSet method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// ---
tBOOL pr_call pr_trace( tVOID* obj, tTRACE_LEVEL level, tSTRING format, ... ) {
	
	tHANDLE*  handle;
	hi_Tracer tracer;
	va_list   arg;
	tERROR    error;

	if ( !obj ) {
		if ( g_hRoot )
			handle = g_hRoot;
		else
			return cFALSE;
	}
	else if ( !_HC(handle,0,obj,&error) )
		return cFALSE;
	
	if ( handle->trace_level.max < level )
		return cFALSE;
	if ( level < handle->trace_level.min )
		return cFALSE;
	if ( 0 == (tracer=(hi_Tracer)handle->tracer) )
		return cFALSE;
	if ( !(MakeHandle(tracer)->flags & hf_OPERATIONAL_MODE) )
		return cFALSE;
	//if ( tracer->data->locked )
	//	return cFALSE;

	va_start( arg, format );
	error = Tracer_TraceV( tracer, MakeObject(handle), level, format, VA_LIST_ADDR(arg) );
	va_end( arg );
	return PR_SUCC( error );
}




// ---
tBOOL pr_call pr_trace_v( tVOID* obj, tTRACE_LEVEL level, tSTRING format, va_list argptr ) {
	
	tHANDLE*  handle;
	hi_Tracer tracer;
	tERROR    error;

	if ( !obj ) {
		if ( g_hRoot )
			handle = g_hRoot;
		else
			return cFALSE;
	}
	else if ( !_HC(handle,0,obj,&error) )
		return cFALSE;
	
	if ( handle->trace_level.max < level )
		return cFALSE;
	if ( level < handle->trace_level.min )
		return cFALSE;
	if ( 0 == (tracer=(hi_Tracer)handle->tracer) )
		return cFALSE;
	if ( !(MakeHandle(tracer)->flags & hf_OPERATIONAL_MODE) )
		return cFALSE;
	//if ( tracer->data->locked )
	//	return cFALSE;

	error = Tracer_TraceV( tracer, MakeObject(handle), level, format, argptr );
	return PR_SUCC( error );
}




//int out_ids( char* buffer, int length, const char* format, tDWORD p1, tDWORD p2 ) {
//  return pr_vsprintf( buffer, length, format, va_beg(format) );
//}

//    #define LETTER(a,l) ( ((a)==l) || ((a)==(l-'a'+'A')) )
//    #define ENTER(a)    ( LETTER(a[0],'e') && LETTER(a[1],'n') && LETTER(a[2],'t') && LETTER(a[3],'e') && LETTER(a[4],'r') )
//    #define LEAVE(a)    ( LETTER(a[0],'l') && LETTER(a[1],'e') && LETTER(a[2],'a') && LETTER(a[3],'v') && LETTER(a[4],'e') )


//    int       l;
//    tHANDLE*  h = MakeHandle( obj );
//
//    if ( d->indent && LEAVE(format) )
//      d->indent--;
//
//    if ( !h )
//      l = out_ids( d->buffer, sizeof(d->buffer), "[null object] - ", 0, 0 );
//    else if ( h->iid == IID_NONE ) {
//      if ( h->iface )
//        l = out_ids( d->buffer, sizeof(d->buffer), "[freed,%05u] - ", h->iface->pid, 0 );
//      else
//        l = out_ids( d->buffer, sizeof(d->buffer), "[freed,unkno] - ", 0, 0 );
//    }
//    else if ( h->iface )
//      l = out_ids( d->buffer, sizeof(d->buffer), "[%05u,%05u] - ", h->iid, h->iface->pid );
//    else
//      l = out_ids( d->buffer, sizeof(d->buffer), "[%05u,unkno] - ", h->iid, 0 );
//
//    if ( d->indent ) {
//      int c;
//      if ( d->indent > 20 )
//        c = l + 40;
//      else
//        c = l + d->indent * 2;
//      for( ; l<c; l++ )
//        d->buffer[l] = ' ';
//    }
//
//    pr_vsprintf( d->buffer+l, sizeof(d->buffer)-l, format, list );
//    d->output_func( d->buffer );
//
//    if ( ENTER(format) )
//      d->indent++;
//
