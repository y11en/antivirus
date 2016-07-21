#include "StdAfx.h"
#include <assert.h>
#include <process.h>
#include "ThreadPool.h"

using namespace std;
using namespace KLUTIL;

CThreadPool::CThreadPool(unsigned nThreadCount) throw()
{
	m_evStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hJobCount = CreateSemaphore(NULL, 0, MAXLONG, NULL);
	m_nBusyThreadCount = 0;
	
	SetThreadCount(nThreadCount);
}

CThreadPool::~CThreadPool() throw()
{
	StopAllThreads();
	CloseHandle(m_evStop);
	CloseHandle(m_hJobCount);
}

void CThreadPool::StopAllThreads() throw()
{
	SetEvent(m_evStop);
	
	// Выкинем ненужные задания
	while (WaitForSingleObject(m_hJobCount, 0) == WAIT_OBJECT_0)
	{
		CCritSec::CLock lock(m_cs);
		assert(!m_JobQueue.empty());
		m_JobQueue.front()->Free();
		m_JobQueue.pop();
	}
	
	if(!m_hThreads.empty())
	{
		WaitForMultipleObjects(m_hThreads.size(), &m_hThreads.front(), TRUE, INFINITE);
		for (vector<HANDLE>::iterator it = m_hThreads.begin(); it!=m_hThreads.end(); ++it)
			CloseHandle(*it);
		m_hThreads.clear();
	}
	assert(m_nBusyThreadCount==0);
	
	assert(m_JobQueue.empty()); // добавили задания на стадии удаления пула
	ResetEvent(m_evStop);
}

void CThreadPool::SetThreadCount(unsigned nThreadCount) throw()
{
	assert(nThreadCount <= MAXIMUM_WAIT_OBJECTS); // количество потоков не может превышать MAXIMUM_WAIT_OBJECTS
	if (nThreadCount > MAXIMUM_WAIT_OBJECTS)
		nThreadCount = MAXIMUM_WAIT_OBJECTS;
	CCritSec::CLock lock(m_cs);
	if (m_hThreads.size() >= nThreadCount)
		return;
	m_nMaxThreadCount = nThreadCount;
	m_hThreads.reserve(nThreadCount);
}

void CThreadPool::Process(ThreadFunc pfnThreadFunc, void* pParam) throw()
{
	class CThreadJob: public IThreadJob
	{
		void* m_pParam;
		ThreadFunc m_pfnThread;
	public:
		CThreadJob(ThreadFunc pfnThread, void* pParam): m_pfnThread(pfnThread), m_pParam(pParam) {}
		void Process(HANDLE evStop) throw() { m_pfnThread(m_pParam, evStop); }
	};

	Process(new CThreadJob(pfnThreadFunc, pParam));
}

void CThreadPool::Process(IThreadJob* pJob) throw()
{
	assert(pJob);
	assert(WaitForSingleObject(m_evStop, 0) == WAIT_TIMEOUT); // нельзя добавлять задания, если завершаемся
	
	CCritSec::CLock lock(m_cs);
	
	unsigned nPotentiallyBusy = m_JobQueue.size() + m_nBusyThreadCount;
	
	// если лимит потоков ещё не исчерпан, и нет ни одного свободного, запустим один
	if (nPotentiallyBusy<m_nMaxThreadCount && m_hThreads.size()<=nPotentiallyBusy)
	{
		assert(m_hThreads.size() == nPotentiallyBusy); // не должно быть такого
		unsigned nThreadId;
		m_hThreads.push_back(reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, CThreadPool::WorkThread, this, 0, &nThreadId)));
	}
	
	m_JobQueue.push(pJob);
	ReleaseSemaphore(m_hJobCount, 1, NULL);
}

unsigned __stdcall CThreadPool::WorkThread(void* pParam)
{
	CThreadPool* pThreadPool = static_cast<CThreadPool*>(pParam);
	HANDLE hEvnts[2] = { pThreadPool->m_hJobCount, pThreadPool->m_evStop };
	while (WaitForMultipleObjects(2, hEvnts, FALSE, INFINITE)==WAIT_OBJECT_0)
	{
		pThreadPool->m_cs.Lock();
		InterlockedIncrement(&(pThreadPool->m_nBusyThreadCount));
		IThreadJob* pJob = pThreadPool->m_JobQueue.front();
		pThreadPool->m_JobQueue.pop();
		pThreadPool->m_cs.Unlock();
		try
		{
			pJob->Process(pThreadPool->m_evStop);
			pJob->Free();
		}
		catch (...)
		{
#ifdef _DEBUG
			throw;
#endif
		}
		InterlockedDecrement(&(pThreadPool->m_nBusyThreadCount));
	}
	return 0;
}