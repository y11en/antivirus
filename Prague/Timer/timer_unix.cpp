// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  25 February 2005,  19:15 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Timer
// Author      -- Pavlushchik
// File Name   -- timer.cpp
// -------------------------------------------
// AVP Prague stamp end

#if defined (__unix__)

// AVP Prague stamp begin( Interface version,  )
#define Timer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_timer.h>
// AVP Prague stamp end
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

tLONG g_nextTimerID = 20000;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable Timer : public cTimer /*cObjImpl*/ {
private:
// Internal function declarations
  tERROR pr_call ObjectInit();
  tERROR pr_call ObjectInitDone();
  tERROR pr_call ObjectPreClose();
  
  // Property function declarations
  tERROR pr_call SetFrequency( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
  tERROR pr_call SetEnabled( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
  
public:
  // External function declarations
  
  // Data declaration
  tDWORD m_timerID; // --
  tDWORD m_frequency; // --
  tBOOL  m_enabled;  // --
  int    m_pipe [2];
  // AVP Prague stamp end
  tERROR setTimer ( tBOOL enabled, tDWORD frequency );

public:
  
  // AVP Prague stamp begin( C++ class declaration end,  )
public:
  mDECLARE_INITIALIZATION(Timer)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Timer". Static(shared) property table variables
// AVP Prague stamp end

void* ThreadProc (void * arg )
{
  Timer* l_timer = reinterpret_cast <Timer*> ( arg );
  if ( !l_timer )
    return 0;
  if ( pipe ( l_timer->m_pipe ) )
    return 0;
  while ( true ) {
    struct timeval l_timeout;
    l_timeout.tv_sec = l_timer->m_frequency / 1000;
    l_timeout.tv_usec = ( l_timer->m_frequency % 1000 ) * 1000;
    fd_set aReadingSet;
    FD_ZERO ( &aReadingSet );
    FD_SET ( l_timer->m_pipe [ 0 ], &aReadingSet );
    int l_count = select ( l_timer->m_pipe [ 0 ] + 1, &aReadingSet, 0, 0, &l_timeout );
    if ( l_count != 0 )
      break;
    if ( l_count == 0 ) {
      PR_TRACE ((l_timer, prtNOTIFY, "timer\t ID:%d timer expired", l_timer->m_timerID ));
      l_timer->sysSendMsg(pmc_TIMER, l_timer->m_timerID, NULL, NULL, NULL);          
    }
  }
  if (l_timer->m_pipe [ 1 ] != -1  ) {
    close ( l_timer->m_pipe [ 1 ] );
    l_timer->m_pipe [ 1 ] = -1;
  }
  close ( l_timer->m_pipe [ 0 ] );
  l_timer->m_pipe [ 0 ] = -1;
  return 0;
}


tERROR Timer::setTimer ( tBOOL enabled, tDWORD frequency )
{
  if ( m_enabled && ( m_frequency != frequency ) ) {
    close ( m_pipe [ 1 ] );
    m_pipe [ 1 ] = -1;
    sched_yield ();
    m_enabled = cFALSE;
  }
  m_frequency = frequency;    
  if ( enabled && !m_enabled ) {
    pthread_t l_thread;
    if ( pthread_create ( &l_thread, 0, ThreadProc, this ) ) {
      m_enabled = cFALSE;
      return errUNEXPECTED;
    }
    pthread_detach ( l_thread );
    m_enabled = cTRUE;
  }
  return errOK;
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR Timer::ObjectInit()
{
  tERROR error = errOK;
  PR_TRACE_FUNC_FRAME( "Timer::ObjectInit method" );
  pthread_mutex_lock ( &g_mutex );
  m_timerID = g_nextTimerID++; 
  pthread_mutex_unlock ( &g_mutex );
  m_frequency = 1000; 
  m_enabled = cFALSE; 
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR Timer::ObjectInitDone()
{
  tERROR error = errOK;
  PR_TRACE_FUNC_FRAME( "Timer::ObjectInitDone method" );
  setTimer ( m_enabled, m_frequency );
  return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR Timer::ObjectPreClose()
{
  tERROR error = errOK;
  PR_TRACE_FUNC_FRAME( "Timer::ObjectPreClose method" );
  setTimer ( cFALSE, 0 );
  return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( Property method implementation, SetFrequency )
// Interface "Timer". Method "Set" for property(s):
//  -- FREQUENCY
tERROR Timer::SetFrequency( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method SetFrequency for property pgFREQUENCY" );
  if ( out_size )
    *out_size = 0;
  
  tDWORD l_frequency = 0;

  if (!buffer || size<sizeof(tDWORD))
    error = errPARAMETER_INVALID;
  else
    l_frequency = *reinterpret_cast<tDWORD*> ( buffer );
  
  error = setTimer (m_enabled, l_frequency );
  
  if ( PR_SUCC(error) && out_size )
    *out_size = sizeof(tDWORD);

  PR_TRACE_A2( this, "Leave *SET* method SetFrequency for property pgFREQUENCY, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetEnabled )
// Interface "Timer". Method "Set" for property(s):
//  -- ENABLED
tERROR Timer::SetEnabled( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
  tERROR error = errOK;
  PR_TRACE_A0( this, "Enter *SET* method SetEnabled for property pgENABLED" );
  if ( out_size )
    *out_size = 0;
  
  tBOOL l_enabled = cFALSE;

  if (!buffer || size<sizeof(tBOOL))
    error = errPARAMETER_INVALID;
  else
    l_enabled = *reinterpret_cast<tBOOL*> ( buffer );

  error = setTimer ( l_enabled, m_frequency );

  if ( PR_SUCC(error) && out_size )
    *out_size = sizeof(tBOOL);
    
  PR_TRACE_A2( this, "Leave *SET* method SetEnabled for property pgENABLED, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Timer". Register function
tERROR Timer::Register( hROOT root ) {
  tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Timer_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Timer_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Timer", 6 )
	mpLOCAL_PROPERTY( pgFREQUENCY, Timer, m_frequency, cPROP_BUFFER_READ, NULL, FN_CUST(SetFrequency) )
	mpLOCAL_PROPERTY_BUF( pgTIMER_ID, Timer, m_timerID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY( pgENABLED, Timer, m_enabled, cPROP_BUFFER_READ, NULL, FN_CUST(SetEnabled) )
mpPROPERTY_TABLE_END(Timer_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Timer)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(Timer)
// AVP Prague stamp end


// AVP Prague stamp begin( External method table,  )
//mEXTERNAL_TABLE_BEGIN(Timer)
//mEXTERNAL_TABLE_END(Timer)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

  PR_TRACE_FUNC_FRAME_( *root, "Timer::Register method", &error );
 
 error = CALL_Root_RegisterIFace(
                                 root,                                   // root object
                                 IID_TIMER,                              // interface identifier
                                 PID_TIMER,                              // plugin identifier
                                 0x00000000,                             // subtype identifier
                                 Timer_VERSION,                          // interface version
                                 VID_MIKE,                               // interface developer
                                 &i_Timer_vtbl,                          // internal(kernel called) function table
                                 (sizeof(i_Timer_vtbl)/sizeof(tPTR)),    // internal function table size
                                 NULL,                                   // external function table
                                 0,                                      // external function table size
                                 Timer_PropTable,                        // property table
                                 mPROPERTY_TABLE_SIZE(Timer_PropTable),  // property table size
                                 sizeof(Timer)-sizeof(cObjImpl),         // memory size
                                 IFACE_SYSTEM                            // interface flags
                                 );
 
#ifdef _PRAGUE_TRACE_
 if ( PR_FAIL(error) )
   PR_TRACE( (root,prtDANGER,"Timer(IID_TIMER) registered [%terr]",error) );
#endif // _PRAGUE_TRACE_
 // AVP Prague stamp end
 
 
 
 // AVP Prague stamp begin( C++ class regitration end,  )
 return error;
}

tERROR pr_call Timer_Register( hROOT root ) { return Timer::Register(root); }
// AVP Prague stamp end

#endif //__unix__

