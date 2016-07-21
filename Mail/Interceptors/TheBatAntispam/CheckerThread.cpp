// CheckerThread.cpp: implementation of the CCheckerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckerThread.h"
#include "../TheBatPlugin/remote_mbl.h"
//#include "../TheBatPlugin/Tracer.h"
#include "Tracing.h"

namespace
{
	const int CHECKING_INTERVAL = 1000;	// in milliseconds
}

CCheckerThread::CCheckerThread() :
	m_bIsRunning(false)
{
	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	// manual non-signaled event
}

bool CCheckerThread::Start()
{
	if (m_bIsRunning)
		return true;	// no need to start another thread

	ASTRACE(_T("Starting checker thread..."));
	DWORD dwThreadID = 0;
	CloseHandle(CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadID));
	return (dwThreadID != 0);
}

bool CCheckerThread::Stop()
{
	ASTRACE(_T("Stopping checker thread..."));
	SetEvent(m_hStopEvent);	
	return true;
}

hOBJECT _RemoteMBLGet();
DWORD WINAPI CCheckerThread::ThreadProc(LPVOID lpParam)
{
	ASTRACE(_T("Checker thread started"));
	CCheckerThread* pObj = reinterpret_cast<CCheckerThread*>(lpParam);
	if (!pObj)
	{
		ASTRACE(_T("Invalid parameters - checker tread stopped"));
		return 1;
	}

	try
	{
		while (WaitForSingleObject(pObj->m_hStopEvent, CHECKING_INTERVAL) == WAIT_TIMEOUT)
		{
			hOBJECT hBL = _RemoteMBLGet();
		}
	}
	catch (...)
	{
		ASTRACE(_T("Unhandled exception in checker thread"));
	}

	ASTRACE(_T("Checker thread stopped"));
	return 0;
}
