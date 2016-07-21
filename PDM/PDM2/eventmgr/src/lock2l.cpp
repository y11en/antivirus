#include <windows.h>
#include "../include/lock.h"
#include "crtdbg.h"

typedef struct tCRITICAL_SECTION_2L
{
	long read_locks;
	long write_locks;
	CRITICAL_SECTION cs;
} CRITICAL_SECTION_2L, *PCRITICAL_SECTION_2L;

cLock::cLock()
{
	m_pLock = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CRITICAL_SECTION_2L));
	PCRITICAL_SECTION_2L pLock = (PCRITICAL_SECTION_2L)m_pLock;
	InitializeCriticalSection(&pLock->cs);
}

cLock::~cLock()
{
	PCRITICAL_SECTION_2L pLock = (PCRITICAL_SECTION_2L)m_pLock;
	_ASSERT(!pLock->write_locks && !pLock->read_locks);
	DeleteCriticalSection(&pLock->cs);
	HeapFree(GetProcessHeap(), 0, m_pLock);
}

void cLock::lock(bool exclusive /* = true */)
{
	PCRITICAL_SECTION_2L pLock = (PCRITICAL_SECTION_2L)m_pLock;
	EnterCriticalSection(&pLock->cs);
	if (exclusive || pLock->write_locks) // requested exclusive assess or we already have it
	{
		while (InterlockedExchangeAdd(&pLock->read_locks, 0)) Sleep(10); // wait all readers to leave cs
		pLock->write_locks++;
		return;
	}
	// requested shared access
	InterlockedIncrement(&pLock->read_locks);
	LeaveCriticalSection(&pLock->cs);
}

void cLock::unlock()
{
	PCRITICAL_SECTION_2L pLock = (PCRITICAL_SECTION_2L)m_pLock;
	if (pLock->write_locks) // if we have exclusive access
	{
		pLock->write_locks--;
		LeaveCriticalSection(&pLock->cs);
		return;
	}
	// we have shared access
	InterlockedDecrement(&pLock->read_locks);
}

