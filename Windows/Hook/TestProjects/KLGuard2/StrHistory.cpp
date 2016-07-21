#include "stdafx.h"
#include "StrHistory.h"
#include "servicestuff.h"

CStrHistory::CStrHistory()
{
}

CStrHistory::~CStrHistory()
{
}

unsigned long CStrHistory::GetEventCount()
{
	return m_History.size();
}

void CStrHistory::PushEvent(CString* pstrMsg)
{
	CAutoLock auto_lock(&m_Sync);
	m_History.push_back(*pstrMsg);
}

bool CStrHistory::PopEvent(CString* pstrMsg)
{
	CAutoLock auto_lock(&m_Sync);

	if (m_History.empty())
		return false;

	*pstrMsg = m_History[0];
	m_History.erase(m_History.begin());
	
	return true;
}