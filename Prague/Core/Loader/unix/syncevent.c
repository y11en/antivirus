// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  16 June 2004,  16:38 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- syncevent.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end

#if defined (__unix__)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Event_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "syncevent.h"
// AVP Prague stamp end



#include "loader.h"
#include <Prague/iface/i_mutex.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Event interface implementation
// Short comments -- Event behaviour
// AVP Prague stamp end



#if defined( __cplusplus )
extern "C" {
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Event. Inner data structure

typedef struct tag_EventData {
        pthread_mutex_t m_mutex;
        pthread_cond_t m_condition;
        tBOOL  m_busy;
	tCHAR  name[MAX_PATH]; // name of the object
	tBOOL  open;          // do not try to create
	tBOOL  manual;        // evant is manually reseted
	tDWORD initial_state; // initial state
	tDWORD last_error;    // last win error
	tBOOL  global;        // --
} EventData;

// AVP Prague stamp end



#if defined( __cplusplus )
}
#endif


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Event {
	const iEventVtbl*  vtbl; // pointer to the "Event" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	EventData*         data; // pointer to the "Event" data structure
} *hi_Event;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Event_ObjectInitDone( hi_Event _this );
tERROR pr_call Event_ObjectClose( hi_Event _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Event_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        NULL,
	(tIntFnObjectInitDone)    Event_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       Event_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpEvent_Wait)     ( hi_Event _this, tDWORD timeout );          // -- wait for event to be signaled;
typedef   tERROR (pr_call *fnpEvent_SetState) ( hi_Event _this, tBOOL signaled );          // -- unlocks synchronization object;
typedef   tERROR (pr_call *fnpEvent_Pulse)    ( hi_Event _this );                          // -- pulse event for manual reset events;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iEventVtbl {
	fnpEvent_Wait      Wait;
	fnpEvent_SetState  SetState;
	fnpEvent_Pulse     Pulse;
}; // "Event" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Event_Wait( hi_Event _this, tDWORD p_timeout );
tERROR pr_call Event_SetState( hi_Event _this, tBOOL p_signaled );
tERROR pr_call Event_Pulse( hi_Event _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iEventVtbl e_Event_vtbl = {
	(fnpEvent_Wait)           Event_Wait,
	(fnpEvent_SetState)       Event_SetState,
	(fnpEvent_Pulse)          Event_Pulse
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Event". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Event_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/plugin/p_win32loader.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_mutex.h>
// AVP Prague stamp end

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Event_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Event_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "event synchronization object", 29 )
	mLOCAL_PROPERTY_BUF( pgOBJECT_NAME, EventData, name, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgJUST_OPEN, EventData, open, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgMANUAL_RESET, EventData, manual, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgLAST_OS_ERROR, EventData, last_error, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgINITIAL_STATE, EventData, initial_state, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_BUF( pgGLOBAL, EventData, global, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mPROPERTY_TABLE_END(Event_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "SyncEvent". Register function
tERROR pr_call Event_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"Event::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_EVENT,                              // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		Event_VERSION,                          // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Event_vtbl,                          // internal(kernel called) function table
		(sizeof(i_Event_vtbl)/sizeof(tPTR)),    // internal function table size
		&e_Event_vtbl,                          // external function table
		(sizeof(e_Event_vtbl)/sizeof(tPTR)),    // external function table size
		Event_PropTable,                        // property table
		mPROPERTY_TABLE_SIZE(Event_PropTable),  // property table size
		sizeof(EventData),                      // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Event(IID_EVENT) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"Event::Register\" method, ret tERROR = 0x%08x", error );
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
tERROR pr_call Event_ObjectInitDone( hi_Event _this ) {
	tERROR error = errOK;
	EventData* d;
	PR_TRACE_A0( _this, "Enter \"SyncEvent::ObjectInitDone\" method" );

	d = _this->data;

        if ( ( d->last_error = pthread_mutex_init ( &d->m_mutex, 0 ) ) != 0 )
          error = errOBJECT_NOT_INITIALIZED;
        if ( PR_SUCC ( error ) && ( ( d->last_error = pthread_cond_init ( &d->m_condition, 0 ) ) != 0 ) )
          error = errOBJECT_NOT_INITIALIZED;
        pthread_mutex_lock ( &d->m_mutex );
        d->m_busy = !d->initial_state;
        pthread_mutex_unlock ( &d->m_mutex );
	PR_TRACE_A1( _this, "Leave \"SyncEvent::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
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
tERROR pr_call Event_ObjectClose( hi_Event _this ) {

	EventData* d;
	PR_TRACE_A0( _this, "Enter \"SyncEvent::ObjectClose\" method" );

	d = _this->data;
	d->last_error = 0;
        if ( CALL_SYS_PropertyGetBool ( _this, psOBJECT_OPERATIONAL ) ) {
          pthread_mutex_destroy ( &d->m_mutex );
          pthread_cond_destroy ( &d->m_condition );
        }
	PR_TRACE_A0( _this, "Leave \"SyncEvent::ObjectClose\" method, ret tERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Wait )
// Parameters are:
tERROR pr_call Event_Wait( hi_Event _this, tDWORD timeout ) {
	tERROR error = errOK;
	EventData* d;
	PR_TRACE_A0( _this, "Enter \"SyncEvent::Wait\" method" );

	d = _this->data;
	d->last_error = 0;

        if ( !CALL_SYS_PropertyGetBool ( _this, psOBJECT_OPERATIONAL ) )        
          error = errOBJECT_NOT_INITIALIZED;
        else {
          struct timespec aTimeOut;
          if ( timeout != (tDWORD) -1 ) {
            struct timeval aNow;
            gettimeofday( &aNow, 0 );
            aTimeOut.tv_sec = aNow.tv_sec + timeout / 1000 ;
            aTimeOut.tv_nsec = aNow.tv_usec * 1000 + timeout % 1000 * 1000000;
          }
          PR_TRACE ((_this, prtNOTIFY, "ldr\t%s %u is awaiting of event with timeout %d msec", __FUNCTION__, pthread_self (), timeout));
          pthread_mutex_lock ( &d->m_mutex );
          if ( timeout != (tDWORD) -1 ) {
            while ( d->m_busy && ( d->last_error != ETIMEDOUT ) )
              d->last_error = pthread_cond_timedwait ( &d->m_condition, &d->m_mutex, &aTimeOut );
            if ( d->last_error == ETIMEDOUT )
              error = errTIMEOUT; 
          }
          else
            while ( d->m_busy )
              d->last_error = pthread_cond_wait ( &d->m_condition, &d->m_mutex );
          d->m_busy = d->manual ? cFALSE : cTRUE;
          pthread_mutex_unlock ( &d->m_mutex );        
          PR_TRACE ((_this, prtNOTIFY, "ldr\t%s %u has ended of event, error: %terr ", __FUNCTION__, pthread_self (), error));
        }

	PR_TRACE_A1( _this, "Leave \"SyncEvent::Wait\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR pr_call Event_SetState( hi_Event _this, tBOOL signaled ) {
	tERROR error = errOK;
	EventData* d;
	PR_TRACE_A0( _this, "Enter \"SyncEvent::SetState\" method" );

	d = _this->data;
	d->last_error = 0;
        if ( !CALL_SYS_PropertyGetBool ( _this, psOBJECT_OPERATIONAL ) )        
          error = errOBJECT_NOT_INITIALIZED;
        else {
          if ( signaled ) {
            PR_TRACE ((_this, prtNOTIFY, "ldr\t%s %u is setting state of event to free", __FUNCTION__, pthread_self ()));
            pthread_mutex_lock ( &d->m_mutex );
            d->m_busy = cFALSE;
            d->last_error = pthread_cond_broadcast ( &d->m_condition );
            pthread_mutex_unlock ( &d->m_mutex );
            PR_TRACE ((_this, prtNOTIFY, "ldr\t%s %u has set state of event to free", __FUNCTION__, pthread_self ()));
          }
          else {
            PR_TRACE ((_this, prtNOTIFY, "ldr\t%s %u is setting state of event to busy", __FUNCTION__, pthread_self ()));
            pthread_mutex_lock ( &d->m_mutex );
            d->m_busy = cTRUE;
            pthread_mutex_unlock ( &d->m_mutex );        
            PR_TRACE ((_this, prtNOTIFY, "ldr\t%s %u has set state of event to busy", __FUNCTION__, pthread_self ()));
          }
        }
	PR_TRACE_A1( _this, "Leave \"SyncEvent::SetState\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Pulse )
// Parameters are:
tERROR pr_call Event_Pulse( hi_Event _this ) {
	tERROR error = errOK;
	EventData* d;
	PR_TRACE_A0( _this, "Enter \"SyncEvent::Pulse\" method" );

	d = _this->data;
	d->last_error = 0;
        if ( !CALL_SYS_PropertyGetBool ( _this, psOBJECT_OPERATIONAL ) )        
          error = errOBJECT_NOT_INITIALIZED;
        else {
          pthread_mutex_lock ( &d->m_mutex );
          d->m_busy = cFALSE;
          d->last_error = pthread_cond_broadcast ( &d->m_condition );
          pthread_mutex_unlock ( &d->m_mutex );
          sched_yield ();
          pthread_mutex_lock ( &d->m_mutex );
          while ( d->m_busy )
            d->last_error = pthread_cond_wait ( &d->m_condition, &d->m_mutex );
          d->m_busy = cTRUE;
          pthread_mutex_unlock ( &d->m_mutex );        
        }
	PR_TRACE_A1( _this, "Leave \"SyncEvent::Pulse\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

#endif //__unix__

