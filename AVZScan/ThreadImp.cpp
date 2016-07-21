// ProxySessionManager.cpp: implementation of the CThreadImp class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "ThreadImp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadImp::CThreadImp(): 
    m_hThread(NULL)
{
	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CThreadImp::~CThreadImp()
{
    Stop();
    if(m_hStopEvent)
		CloseHandle(m_hStopEvent);
}

tERROR CThreadImp::Start()
{
	if(m_hThread)
	{
		PR_ASSERT(0);
		return errUNEXPECTED;
	}
	if(!m_hStopEvent)
		return errNOT_ENOUGH_MEMORY;
	ResetEvent(m_hStopEvent);
	DWORD dwThreadId;
	m_hThread = CreateThread(NULL, 0, RunThreadFunc, this, 0, &dwThreadId);
	return m_hThread ? errOK : errNOT_ENOUGH_MEMORY;
}

tERROR CThreadImp::Stop()
{
	if(!m_hThread)
		return errOK;
	if(m_hStopEvent && m_hThread)
	{
		SetEvent(m_hStopEvent);
		DWORD exit_code;
		if(GetExitCodeThread(m_hThread, &exit_code) && exit_code == STILL_ACTIVE)
			WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	return errOK;
}

DWORD WINAPI CThreadImp::RunThreadFunc(LPVOID param)
{
	PR_VERIFY_SUCC(((CThreadImp *)param)->_Run());
	return 0;
}
