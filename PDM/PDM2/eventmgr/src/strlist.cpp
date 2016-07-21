#include "../include/strlist.h"

cStrList::cStrList()
{
}

cStrList::~cStrList()
{
	for (tstrvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
		tstrfree(*_it);
}

bool cStrList::add(tcstring string)
{
	cAutoLock _lock(m_Sync);
	if (!string)
		return false;
	for (tstrvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		if (0 == tstrcmp(*_it, string))
			return true;
	}
	tstring str = tstrdup(string);
	if (!str)
		return false;
	m_list.push_back(str);
	return true;
}

bool cStrList::remove(tcstring string)
{
	cAutoLock _lock(m_Sync);
	bool res = false;
	if (!string)
		return false;
	for (tstrvector::iterator _it = m_list.begin(); _it != m_list.end(); )
	{
		if (0 == tstrcmp(*_it, string))
		{
			tstring s = *_it;
			_it = m_list.erase( _it );
			tstrfree(s);
			res = true;
			continue;
		}
		_it++;
	}
	return res;
}

bool cStrList::contain(tcstring string, bool bSubstr)
{
	cAutoLock _lock(m_Sync);
	for (tstrvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		bool bCmpRes = false;
		if (bSubstr)
			bCmpRes = (NULL != tstrstr(string, *_it));
		else
			bCmpRes = (0 == tstrcmp(string, *_it));
		if (true == bCmpRes)
			return true;
	}
	return false;
}
