#if !defined(_SIMPLE_THREAD_H)
#define _SIMPLE_THREAD_H

#pragma once

class CSimpleThread
{
public:
	CSimpleThread() : m_hEvent(NULL), m_hThread(NULL), m_nThreadId(0), m_bSelfDelete(false) {}

	~CSimpleThread()
	{
		if(!m_bSelfDelete)
			WaitForStop();
		CloseHandle(m_hThread); m_hThread = NULL;
	}
	
public:
	DWORD Run(bool bSelfDelete = false)
	{
		if(IsWorking())
			return 1;
		
		m_bSelfDelete = bSelfDelete;
		
		if(m_hThread)
		{
			CloseHandle(m_hThread); m_hThread = NULL;
		}

		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(!m_hEvent)
		{
			if(m_bSelfDelete)
				delete this;
			return 1;
		}
		
		m_hThread = CreateThread(NULL, 0x1000, DoWorkLink, this, 0, (DWORD *)&m_nThreadId);
		if(!m_hThread)
		{
			CloseHandle(m_hEvent); m_hEvent = NULL;
			if(m_bSelfDelete)
				delete this;
			return 1;
		}

		HANDLE hObj[2];
		hObj[0] = m_hThread;
		hObj[1] = m_hEvent;
		DWORD nObj = WaitForMultipleObjects(2, hObj, FALSE, INFINITE);
		if(nObj == WAIT_OBJECT_0)
		{
			CloseHandle(m_hEvent); m_hEvent = NULL;

			DWORD nErr;
			GetExitCodeThread(m_hThread, &nErr);
			CloseHandle(m_hThread); m_hThread = NULL;
			if(m_bSelfDelete)
				delete this;
			return nErr;
		}
		else
		{
			CloseHandle(m_hEvent); m_hEvent = NULL;
			return 0;
		}
	}
	
	DWORD Terminate(DWORD nExitCode)
	{
		return TerminateThread(m_hThread, nExitCode);
	}
	
	DWORD WaitForStop(DWORD nTimeout = INFINITE)
	{
		return WaitForSingleObject(m_hThread, nTimeout);
	}
	
	bool IsWorking()
	{
		DWORD nErr;
		if(GetExitCodeThread(m_hThread, &nErr))
			return nErr == STILL_ACTIVE;
		return false;
	}
	bool SetPriority(int nPriority)
	{
		if ( m_hThread )
			return SetThreadPriority(m_hThread, nPriority) ? true : false;
		return false;
	}
	int GetPriority()
	{
		if ( m_hThread )
			return GetThreadPriority(m_hThread);
		return THREAD_PRIORITY_NORMAL;
	}

protected:
	virtual DWORD OnRun() {return 0;}
	virtual DWORD OnDoWork() {return 0;}
	virtual void  OnTerminate() {}

protected:
	static DWORD __stdcall DoWorkLink(PVOID pContext)
	{
		return ((CSimpleThread *)pContext)->DoWork();
	}
	
	DWORD DoWork()
	{
		DWORD nErr = OnRun();
		if(!nErr)
		{
			SetEvent(m_hEvent);
			
			nErr = OnDoWork();
			OnTerminate();

			if(m_bSelfDelete)
				delete this;
		}
		return nErr;
	}

private:
	volatile HANDLE m_hEvent;
	volatile HANDLE m_hThread;
	volatile DWORD  m_nThreadId;
	volatile BOOL   m_bSelfDelete;
};

#endif _SIMPLE_THREAD_H
