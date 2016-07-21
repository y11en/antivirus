#include "strlist.h"
#include "stuff/OwnSync.h"
#include "stuff/mm.h"
#include "stuff/servfuncs.h"

CStrList::~CStrList()
{
	for (tStrList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
		FreeStr(*_it);
}

bool CStrList::Add(PWCHAR pwcsString)
{
	CAutoLock _lock(m_Sync);
	if (!pwcsString)
		return false;
	pwcsString = GetCopyStr( pwcsString );
	if (!pwcsString)
		return false;
	if (m_bUniq)
	{
		for (tStrList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
		{
			bool bCmpResult;
			if (m_bInsensetive)
				bCmpResult = (0 == wcsicmp(*_it, pwcsString));
			else
				bCmpResult = (0 == wcscmp(*_it, pwcsString));
			if (bCmpResult == true)
				return true;
		}
	}
	m_List.push_back(pwcsString);
	return true;
}

bool CStrList::Remove(PWCHAR pwcsString)
{
	CAutoLock _lock(m_Sync);
	bool res = false;
	if (!pwcsString)
		return false;
	for (tStrList::iterator _it = m_List.begin(); _it != m_List.end(); )
	{
		bool bCmpResult;
		if (m_bInsensetive)
			bCmpResult = (0 == wcsicmp(*_it, pwcsString));
		else
			bCmpResult = (0 == wcscmp(*_it, pwcsString));
		if (bCmpResult == true)
		{
			PWCHAR s = *_it;
			_it = m_List.erase( _it );
			FreeStr(s);
			res = true;
			continue;
		}
		_it++;
	}
	return res;
}

bool CStrList::Contain(PWCHAR pwcsString, bool bSubstr)
{
	CAutoLock _lock(m_Sync);
	for (tStrList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
	{
		bool bCmpRes = false;
		if (m_bInsensetive)
		{
			if (bSubstr)
				bCmpRes = (NULL != wcsistr(pwcsString, *_it));
			else
				bCmpRes = (0 == wcsicmp(pwcsString, *_it));
		}
		else
		{
			if (bSubstr)
				bCmpRes = (NULL != wcsstr(pwcsString, *_it));
			else
				bCmpRes = (0 == wcscmp(pwcsString, *_it));
		}
		if (true == bCmpRes)
			return true;
	}
	return false;
}
