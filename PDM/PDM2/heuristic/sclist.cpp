#include "sclist.h"
/*
cSelfCopyList::cSelfCopyList()
{
}

cSelfCopyList::~cSelfCopyList()
{
	for (tsclvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
		tstrfree(_it->fullpath);
}

bool cSelfCopyList::add(tPid pid, tcstring selfcopy)
{
	cAutoLock _lock(m_Sync);
	if (!selfcopy)
		return false;
	for (tsclvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		if (0 == tstrcmp(_it->fullpath, selfcopy))
			return true;
	}
	tstring str = tstrdup(selfcopy);
	if (!str)
		return false;
	SELFCOPY_LIST_ITEM sci = {str, pid};
	m_list.push_back(sci);
	return true;
}

bool cSelfCopyList::remove(tPid pid)
{
	cAutoLock _lock(m_Sync);
	bool res = false;
	for (tsclvector::iterator _it = m_list.begin(); _it != m_list.end(); )
	{
		if (_it->pid == pid)
		{
			tstrfree(_it->fullpath);
			_it = m_list.erase( _it );
			res = true;
			continue;
		}
		_it++;
	}
	return res;
}

bool cSelfCopyList::contain(tcstring str, bool bSubstr, tPid* ppid)
{
	cAutoLock _lock(m_Sync);
	for (tsclvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		bool bCmpRes = false;
		if (bSubstr)
			bCmpRes = (NULL != tstrstr(str, _it->fullpath));
		else
			bCmpRes = (0 == tstrcmp(str, _it->fullpath));
		if (true == bCmpRes)
		{
			if (ppid)
				*ppid = _it->pid;
			return true;
		}
	}
	return false;
}

/*
size_t cSelfCopyList::count()
{
	cAutoLock _lock(m_Sync);
	return m_list.size();
}

SELFCOPY_LIST_ITEM& cSelfCopyList::operator[](size_t index)
{
	SELFCOPY_LIST_ITEM& item = NULL;
	{
		cAutoLock auto_lock(m_Sync, false);
		if (index < m_list.size())
			item = m_ProcList[index];
	}
	return item;
}
*/
