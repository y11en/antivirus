
#include "stdafx.h"
#include "ApartThread.h"

#define WC_TERMINATE		 0xd1
#define WC_DOING			 0xd2


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW				    
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// Constructor
CApartThread::CApartThread() 	 
{
	m_hThread	= NULL ;
	m_WorkCom	= WC_DOING;
	m_bComInit	= FALSE;
	m_Priority	= THREAD_PRIORITY_NORMAL;
	m_dwDelay = 0;

	m_evReady		= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_evRestart		= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_evDoRestart	= CreateEvent(NULL, TRUE, FALSE, NULL);

	m_ThreadId = 0;
} 
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Destructor
CApartThread::~CApartThread()
{
	CloseHandle(m_evReady);
	CloseHandle(m_evRestart);
	CloseHandle(m_evDoRestart);
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// StartThread
//   - Create and start the thread.
BOOL CApartThread::Create() 
{
	if (m_hThread) return TRUE;
	m_WorkCom = WC_DOING;
	m_hThread = ::CreateThread(NULL,              // Default security
	                           0,                 // Default stack size
	                           RealThreadProc,
	                           (void*)this,
	                           CREATE_SUSPENDED,  // Create the thread suspended.
	                           &m_ThreadId) ;     // Get the Thread ID.
	if (m_hThread == NULL)
		return FALSE ;

	SetThreadPriority(m_hThread,m_Priority);
	ResumeThread(m_hThread);

	WaitEvent(m_evReady);
	return TRUE ;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
BOOL CApartThread::SetPriority(int i_nPriority)
{
	m_Priority = i_nPriority;
	if(m_hThread)
		return SetThreadPriority(m_hThread, m_Priority);

	return FALSE;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
BOOL  CApartThread::Suspend()
{
  if(m_hThread==NULL || 0xFFFFFFFF==SuspendThread(m_hThread))
    return FALSE;
  return TRUE;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
BOOL  CApartThread::Resume()
{
  if(m_hThread==NULL || 0xFFFFFFFF==ResumeThread(m_hThread))
    return FALSE;
  return TRUE;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
BOOL  CApartThread::OnInit()
{
	if(m_bComInit)
		CoInitialize(NULL);
	return TRUE;
}

BOOL  CApartThread::OnExit()
{
	if(m_bComInit)
		CoUninitialize();
	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CApartThread::QueryRestart(BOOL bSync, BOOL i_bAutoRestart) 
{
	// ѕосле Create устанавливаетс€ состо€ние готовности
	if(!m_hThread && !Create()) 
		return FALSE;
	// ≈сли готовность уже есть
	if(WaitEvent(m_evReady,0)) 
	{
		if(i_bAutoRestart)
			SetEvent(m_evDoRestart);
		return TRUE;
	}

	// ƒелаем запрос на готовность
	::SetEvent(m_evRestart);

	// ∆дем ответа
	BOOL bRet;

	if(bSync)
		bRet = WaitEvent(m_evReady);
	else
		bRet = WaitEvent(m_evReady, 0);

	if(i_bAutoRestart)
		SetEvent(m_evDoRestart);

	return bRet;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
BOOL CApartThread::QueryRestartEx(DWORD i_dwWaitTimeout, BOOL i_bAutoRestart) 
{
	// ѕосле Create устанавливаетс€ состо€ние готовности
	if(!m_hThread && !Create()) 
		return FALSE;

	// ≈сли готовность уже есть
	if(WaitEvent(m_evReady,0)) 
	{
		if(i_bAutoRestart)
			SetEvent(m_evDoRestart);
		return TRUE;
	}

	// ƒелаем запрос на готовность
	::SetEvent(m_evRestart);
	// ∆дем ответа
	BOOL bRet;

	bRet = WaitEvent(m_evReady, i_dwWaitTimeout);
	if(i_bAutoRestart)
		SetEvent(m_evDoRestart);

	return bRet;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
BOOL CApartThread::Restart(BOOL i_bStopAfterStart)
{
	// ѕосле Create устанавливаетс€ состо€ние готовности
	if(!m_hThread && !Create()) 
		return FALSE;
	if(!WaitEvent(m_evReady,0)) return FALSE;
	if(i_bStopAfterStart)
		return TRUE;
	SetEvent(m_evDoRestart);
	while(m_hThread != NULL && WaitEvent(m_evDoRestart,0)) 
		SleepEx(1, FALSE);
	return TRUE;
}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
void CApartThread::Terminate(DWORD i_dwWaitTimeout)
{
	if(m_hThread != NULL)
	{			
		HANDLE hThread = m_hThread;
		int nCurPriority = GetThreadPriority(GetCurrentThread());
		BOOL bRes = SetThreadPriority(m_hThread, nCurPriority);
		nCurPriority = GetThreadPriority(m_hThread);

		// if thread didn't terminated itself, 
		// do not touch inner data - may be data collision occured
		if(QueryRestartEx(i_dwWaitTimeout))
			OnDestroy();	
		
		if(0 == i_dwWaitTimeout)
			i_dwWaitTimeout = 250;

		m_WorkCom = WC_TERMINATE;
		Restart();

		DWORD dwExitCode = STILL_ACTIVE;
		DWORD dwStartTicks = GetTickCount();
		do{
			if(i_dwWaitTimeout && (GetTickCount() - dwStartTicks) > i_dwWaitTimeout)
			{
				m_hThread = NULL;
				m_ThreadId = 0;
				TerminateThread(m_hThread, 0);
				return;
			}
			SleepEx(0, FALSE);
			if(m_hThread)
				GetExitCodeThread(m_hThread, &dwExitCode);
		} while(dwExitCode==STILL_ACTIVE && m_hThread != NULL);

		m_hThread = NULL;
		m_ThreadId = 0;

		CloseHandle(hThread);
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// returns true if time out expired and not restart quieried
// and false when restart quieried
BOOL	CApartThread::WaitTimeout(DWORD i_dwMsecs)
{
	return WAIT_TIMEOUT == WaitForSingleObject(m_evRestart, i_dwMsecs);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// Thread procedure
//
DWORD WINAPI CApartThread::RealThreadProc(void* pv) 
{
	CApartThread* pThreadComp = reinterpret_cast<CApartThread*>(pv) ;
	return pThreadComp->ClassThreadProc() ;
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// Thread procedure
//
// exception handling not enabled warning
#pragma warning (disable:4530 )

DWORD CApartThread::ClassThreadProc()
{
	m_bInitResult = OnInit();
	for(;;)
	{
		::SetEvent(m_evReady);			// сигнал о том, что мы ждем
		WaitEvent(m_evDoRestart,INFINITE);

		// код выполн€емый только в этой кодовой нитке
		::ResetEvent(m_evReady);
		::ResetEvent(m_evRestart);
		::ResetEvent(m_evDoRestart);

		// с этого момента начинаетс€ совместное выполнение кода

		// задержка перед запуском
		if(m_WorkCom==WC_DOING && m_dwDelay)
		{
			DWORD delay = m_dwDelay;
			while(delay-- && !IsRestartQueried())
				Sleep(0);
			if(IsRestartQueried())
					continue;
		}

		switch(m_WorkCom)
		{
			case WC_DOING:		 
#ifndef _DEBUG		
				try
				{
					Run();
				}
				catch(...)
				{
					OnExit();
					OnRunException();
					m_hThread = NULL ;
					m_ThreadId = 0;
					ExitThread(0);
				}
#else	
				Run();
#endif
				break;
			case WC_TERMINATE: 
				OnExit();
				m_hThread = NULL ;
				m_ThreadId = 0;
				ExitThread(0);
		}
	}
	return 0 ;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

