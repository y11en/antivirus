#include <windows.h>
#include "lock.h"

#define cs ((CRITICAL_SECTION*)m_pData)

cLock::cLock()
{
	m_pData = new CRITICAL_SECTION();
	if (!m_pData)
		return;
	InitializeCriticalSection(cs);
}

cLock::~cLock()
{
	if (!m_pData)
		return;
	DeleteCriticalSection(cs);
	delete m_pData;
}

void cLock::Enter(bool bExclusive)
{
	if (!m_pData)
		return;
	EnterCriticalSection(cs);
}

void cLock::Leave()
{
	if (!m_pData)
		return;
	LeaveCriticalSection(cs);
}
