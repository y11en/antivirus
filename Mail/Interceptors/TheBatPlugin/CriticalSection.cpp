// CriticalSection.cpp: implementation of the CCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CriticalSection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCriticalSection::CCriticalSection()
{
	::InitializeCriticalSection(&m_cs);
	m_bIsConstructed = TRUE;
}

CCriticalSection::~CCriticalSection()
{
	if (m_bIsConstructed)
	{
		::DeleteCriticalSection(&m_cs);
		m_bIsConstructed = FALSE;
	}
}

void CCriticalSection::Enter()
{
	if (m_bIsConstructed)
		EnterCriticalSection(&m_cs);
}

void CCriticalSection::Leave()
{
	if (m_bIsConstructed)
		LeaveCriticalSection(&m_cs);
}

