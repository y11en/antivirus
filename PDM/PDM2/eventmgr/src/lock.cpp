#include <windows.h>
#include "../include/lock.h"

cLock::cLock()
{
	m_pLock = HeapAlloc(GetProcessHeap(), 0, sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((CRITICAL_SECTION*)m_pLock);
}

cLock::~cLock()
{
	DeleteCriticalSection((CRITICAL_SECTION*)m_pLock);
	HeapFree(GetProcessHeap(), 0, m_pLock);
}

void cLock::lock(bool exclusive /* = true */)
{
	EnterCriticalSection((CRITICAL_SECTION*)m_pLock);
}

void cLock::unlock()
{
	LeaveCriticalSection((CRITICAL_SECTION*)m_pLock);
}

