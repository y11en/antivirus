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

#if defined (_WIN32)

// AVP Prague stamp begin( Interface version,  )
#define Timer_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_timer.h>
// AVP Prague stamp end



#if (_WIN32_WINNT < 0x0400) || !defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0400 // to enable waitable timer functions
#endif
#include <windows.h>

LONG g_dwNextTimerID = 20000;

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
	tDWORD m_dwTimerID; // --
	tDWORD m_dwFrequency; // --
	tBOOL  m_bEnabled;  // --
// AVP Prague stamp end



public:
	HANDLE m_hWaitableTimer;
	HANDLE m_hExitEvent;
	HANDLE m_hThread;
	DWORD  m_dwThreadID;
	tBOOL  m_bClosing;
	tBOOL  m_bInited;
	CRITICAL_SECTION m_cs;

	tERROR StartThread();
	tERROR StopThread();
	tERROR ReinitTimer(tDWORD dwMilliseconds);
	tERROR EnableTimer(tBOOL bEnable);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(Timer)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Timer". Static(shared) property table variables
// AVP Prague stamp end



DWORD WINAPI TimerThreadProc(LPVOID lpThreadParameter)
{
	Timer* _this = (Timer*)lpThreadParameter;
	DWORD  dwWaitRes;
	HANDLE hArray[2];
	BOOL   bExit = FALSE;

	// m_hExitEvent signaled when timer disabled 
	// and thread should be stopped
	hArray[0] = _this->m_hExitEvent;
	// waitable timer signaled when timer period elapsed
	hArray[1] = _this->m_hWaitableTimer;
	
	PR_TRACE((_this, prtNOTIFY, "timer\tTimer %d thread started, %d ms", _this->m_dwTimerID, _this->m_dwFrequency));

	do
	{
		dwWaitRes = WaitForMultipleObjects(countof(hArray), (HANDLE*)&hArray, FALSE, INFINITE);
		switch (dwWaitRes)
		{
		case WAIT_OBJECT_0:
			bExit = TRUE;
			break;
		case WAIT_OBJECT_0 + 1:
			if (!_this->m_bClosing) // double check
			{
				PR_TRACE((_this, prtNOTIFY, "timer\tTimer signaled %d", _this->m_dwTimerID));
				_this->sysSendMsg(pmc_TIMER, _this->m_dwTimerID, NULL, NULL, NULL);
			}
			break;
		case WAIT_FAILED:
			PR_TRACE((_this, prtERROR, "timer\tWait failed, timer=%d", _this->m_dwTimerID));
			bExit = TRUE;
		default:
			PR_TRACE((_this, prtERROR, "timer\tUnknown wait result %d timer=%d", dwWaitRes, _this->m_dwTimerID));
			bExit = TRUE;
			break;
		}
	} while (!bExit);

	// thread itself responsible to close handles
	CloseHandle(hArray[0]);
	CloseHandle(hArray[1]);
	PR_TRACE((_this, prtNOTIFY, "timer\tTimer %d thread exited", _this->m_dwTimerID));
	return 0;
}

tERROR Timer::ReinitTimer(tDWORD dwMilliseconds)
{
	if (m_hWaitableTimer)
	{
		LARGE_INTEGER liDueTime;
		LONG nMilliseconds = (LONG)dwMilliseconds;
		if (nMilliseconds <= 0) 
			return errPARAMETER_INVALID;

		liDueTime.QuadPart = - ((__int64)nMilliseconds * 10000);  // (milliseconds*10000)
		if (!SetWaitableTimer(m_hWaitableTimer, &liDueTime, nMilliseconds, NULL, NULL, FALSE))
		{
			PR_TRACE((this, prtERROR, "timer\tTimer %d failed to reinitialize, win32err=%d", m_dwTimerID, GetLastError()));
			return errUNEXPECTED;
		}
		PR_TRACE((this, prtNOTIFY, "timer\tTimer %d reinitialized with period %d ms", m_dwTimerID, nMilliseconds));
	}
	return errOK;
}

tERROR Timer::StartThread()
{
	if (m_hExitEvent || m_hWaitableTimer)
	{
		PR_TRACE((g_root, prtERROR, "timer\tHandles still initialized but must not be, possible resources leak!"));
		INT3;
		m_hExitEvent = NULL;
		m_hWaitableTimer = NULL;
	}

	m_hWaitableTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (!m_hWaitableTimer)
	{
		PR_TRACE((this, prtERROR, "timer\tTimer %d CreateWaitableTimer failed, win32err=%d", m_dwTimerID, GetLastError()));
		return errUNEXPECTED;
	}

	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hExitEvent)
	{
		PR_TRACE((this, prtERROR, "timer\tTimer %d CreateEvent failed, win32err=%d", m_dwTimerID, GetLastError()));
		CloseHandle(m_hWaitableTimer);
		m_hWaitableTimer = NULL;
		return errUNEXPECTED;
	}
	
	// start timer thread
	m_hThread = CreateThread(NULL, 0, TimerThreadProc, this, 0, &m_dwThreadID);
	if (!m_hThread)
	{
		PR_TRACE((this, prtERROR, "timer\tTimer %d CreateThread failed, win32err=%d", m_dwTimerID, GetLastError()));
		CloseHandle(m_hWaitableTimer);
		m_hWaitableTimer = NULL;
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
		return errUNEXPECTED;
	}
	PR_TRACE((this, prtNOTIFY, "timer\tTimer %d thread created", m_dwTimerID));
	return errOK;
}

tERROR Timer::StopThread()
{
	if (m_hWaitableTimer)
	{
		CancelWaitableTimer(m_hWaitableTimer); // stop timer
		// now thread itself responsible to close m_hWaitableTimer handle, overwise WaitForxxx returns WAIT_FAILED
		// CloseHandle(m_hWaitableTimer);
		m_hWaitableTimer = NULL;
	}

	if (m_hExitEvent)
	{
		SetEvent(m_hExitEvent);    // signal thread to exit
		// now thread itself responsible to close m_hExitEvent handle
		// CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	if (m_hThread)
	{
		if (GetCurrentThreadId() == m_dwThreadID)
		{
			PR_TRACE((this, prtNOTIFY, "timer\tTimer %d stop requested on own callback message", m_dwTimerID));
		}
		else
		{
			while (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 5000)) // wait for 5 seconds
			{
				PR_TRACE((this, prtDANGER, "timer\tTimer %d waiting for thread stop, TID=%u(0x%x), 5 seconds elapsed - possible deadlock", m_dwTimerID, m_dwThreadID, m_dwThreadID)); 
			}
			m_dwThreadID = 0;
			PR_TRACE((this, prtNOTIFY, "timer\tTimer %d thread stopped", m_dwTimerID));
		}
		CloseHandle(m_hThread);
		m_hThread = 0;
	}
	PR_TRACE((this, prtNOTIFY, "timer\tTimer %d stopped", m_dwTimerID));
	return errOK;
}

tERROR Timer::EnableTimer(tBOOL bEnable)
{
	tERROR error = errOK;
	
	if (bEnable)
	{
		error = StartThread();
		if (PR_SUCC(error))
			error = ReinitTimer(m_dwFrequency);
		if (PR_SUCC(error))
		{
			PR_TRACE((this, prtNOTIFY, "timer\tTimer %d enabled, freq=%d ms", m_dwTimerID, m_dwFrequency));
			m_bEnabled = cTRUE;
		}
		else
		{
			PR_TRACE((this, prtNOTIFY, "timer\tTimer %d enable failed, %terr", m_dwTimerID, error));
			StopThread(); // something goes wrong
		}
		return error;
	}

	// if !bEnable

	m_bEnabled = cFALSE;
	error = StopThread();
	PR_TRACE((this, prtNOTIFY, "timer\tTimer %d disabled, %terr", m_dwTimerID, error));
	return error;
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

	m_bEnabled = cFALSE;
	m_dwTimerID = InterlockedIncrement(&g_dwNextTimerID);
	m_dwFrequency = 1000;

	InitializeCriticalSection(&m_cs);

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
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Timer::ObjectInitDone method" );
	
	PR_TRACE((this, prtNOTIFY, "timer\tTimer %d created, freq=%d ms, in %s state", m_dwTimerID, m_dwFrequency, m_bEnabled ? "enabled" : "DISABLED"));
	if (m_bEnabled)
		error = EnableTimer(cTRUE);	
	else
		error = errOK;
	m_bInited = cTRUE;
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

	// set closing flag, by entering critical
	// section we ensure all other threads out
	// of critical code
	EnterCriticalSection(&m_cs);
	m_bClosing = cTRUE;
	LeaveCriticalSection(&m_cs);
	// now we sure new enable request
	// will be declined
	// stop timer if still running
	if (m_bEnabled)
		EnableTimer(cFALSE);
	if (GetCurrentThreadId() == m_dwThreadID)
	{
		PR_TRACE((this, prtFATAL, "timer\tTIMER OBJECT CLOSING ON ITS CALLBACK MESSAGE! THREAD CONTEXT BECOME INVALID! TimerID=%d, TID=%d(0x%x)", m_dwTimerID, m_dwThreadID, m_dwThreadID));
		INT3;
	}
	DeleteCriticalSection(&m_cs);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetFrequency )
// Interface "Timer". Method "Set" for property(s):
//  -- FREQUENCY
tERROR Timer::SetFrequency( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	tDWORD dwMilliseconds;
	PR_TRACE_A0( this, "Enter *SET* method SetFrequency for property pgFREQUENCY" );

	*out_size = 0;

	if (!buffer || size<sizeof(tDWORD))
		return errPARAMETER_INVALID;

	dwMilliseconds = *(tDWORD*)buffer;
	
	if (PR_SUCC(error = ReinitTimer(dwMilliseconds)))
		m_dwFrequency = dwMilliseconds;

	if ( PR_SUCC(error) )
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
	tBOOL  bEnabled;
	PR_TRACE_A0( this, "Enter *SET* method SetEnabled for property pgENABLED" );

	*out_size = 0;

	if (!buffer || size<sizeof(tBOOL))
		return errPARAMETER_INVALID;

	bEnabled = *(tBOOL*)buffer;

	EnterCriticalSection(&m_cs);
	if (m_bClosing)
		error = errOBJECT_ALREADY_FREED;
	else
	{
		if (m_bEnabled == bEnabled)
		{
			PR_TRACE((this, prtNOTIFY, "timer\tTimer %d already %s", m_dwTimerID, m_bEnabled ? "enabled" : "disabled"));
			error = errOK; // already in state
		}
		else
		{
			if (m_bInited)
				error = EnableTimer(bEnabled);
			else
				m_bEnabled = bEnabled; // will be enabled in InitDone
		}
	}
	LeaveCriticalSection(&m_cs);

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
	mpLOCAL_PROPERTY( pgFREQUENCY, Timer, m_dwFrequency, cPROP_BUFFER_READ, NULL, FN_CUST(SetFrequency) )
	mpLOCAL_PROPERTY_BUF( pgTIMER_ID, Timer, m_dwTimerID, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY( pgENABLED, Timer, m_bEnabled, cPROP_BUFFER_READ, NULL, FN_CUST(SetEnabled) )
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

#endif //_WIN32

