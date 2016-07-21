// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------

#include <list>

// -------------------------------------------

struct AVSThreadInfo
{
	AVSThreadInfo(tDWORD nThreadId) :
		m_nThreadId(nThreadId), m_hDupThread(NULL){}

	~AVSThreadInfo()
	{
		if( m_hDupThread )
			CloseHandle(m_hDupThread);
	}

	tDWORD	m_nThreadId;
	tDWORD	m_nYieldTime;
	tBOOL	m_bProcess;

	HANDLE	m_hDupThread;
};

struct AVSThreads : public std::list<AVSThreadInfo>
{
public:
	AVSThreads()
	{
		m_tlsThreads = TlsAlloc();
		InitializeCriticalSection(&m_lock);
	}

	~AVSThreads()
	{
		DeleteCriticalSection(&m_lock);
		TlsFree(m_tlsThreads);
	}

	AVSThreadInfo * GetThreadInfo()
	{
		AVSThreadInfo * pInfo = (AVSThreadInfo*)TlsGetValue(m_tlsThreads);
		if( !pInfo )
		{
			EnterCriticalSection(&m_lock);

			push_back(GetCurrentThreadId());
			pInfo = &*rbegin();

			DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &pInfo->m_hDupThread,
				THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_TERMINATE, FALSE, 0);

			TlsSetValue(m_tlsThreads, pInfo);
			LeaveCriticalSection(&m_lock);
		}
		return pInfo;
	}

	void DetachThread()
	{
		AVSThreadInfo * pInfo = (AVSThreadInfo*)TlsGetValue(m_tlsThreads);
		if( !pInfo )
			return;

		EnterCriticalSection(&m_lock);
		for(iterator it = begin(); it != end(); ++it)
		{
			AVSThreadInfo * itInfo = &*it;
			if( itInfo == pInfo )
			{
				erase(it);
				break;
			}
		}
		LeaveCriticalSection(&m_lock);
	}

	void EnumThreads()
	{
		EnterCriticalSection(&m_lock);
		for(iterator it = begin(); it != end(); ++it)
			OnEnumThread(&*it);
		LeaveCriticalSection(&m_lock);
	}

	virtual void OnEnumThread(AVSThreadInfo *pInfo){}

private:
	tDWORD	m_tlsThreads;
	CRITICAL_SECTION m_lock;
};

// -------------------------------------------
