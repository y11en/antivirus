#if !defined(AFX_PATHLIST_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
#define AFX_PATHLIST_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "strlist.h"
#include "cpath.h"

typedef std::vector<tstring> tPathList;

class cPathList
{
public:
	cPathList();
	~cPathList();
	
	bool add(tcstring path);
	bool remove(tcstring path);
	size_t count();
	bool contain(tcstring path);
	bool containPathForSubstr(tcstring substr);
	bool containSubstrForPath(tcstring path);
	tcstring operator[](size_t index);

private:
	tPathList m_list;
};


#include <algorithm>
#include <list>
#include "lock.h"
#include "types.h"

template<typename T> class _tPathList : public std::list<T> 
{
public:    
 typedef std::list<T> Base;
 using Base::end;
 using Base::begin;
 typedef typename Base::iterator iterator;
 
	iterator find(tcstring fullpath)
	{
		return std::find(begin(), end(), fullpath);
	}
	
	iterator add(tcstring fullpath, bool replace_existing = false)
	{
		iterator it = find(fullpath);
		if (it != end())
		{
			if (!replace_existing)
				return end();
			erase(it);
		}
		it = insert(end(), T());
		it->fullpath = tstrdup(fullpath);
		return it;
	}
	bool remove(tcstring fullpath)
	{
		iterator it = find(fullpath);
		if (it == NULL)
			return false;
		erase(it);
		return true;
	}
	
	bool contain(tcstring fullpath)
	{
		return find(fullpath) != end();
	}

	bool containSubstrForPath(tcstring fullpath)
	{
		for (iterator _it = begin(); _it != end(); _it++)
		{
			if (tstrstr(fullpath, *_it))
				return true;
		}		
		return false;
	}
	
	bool containPathForSubstr(tcstring substr)
	{
		for (iterator _it = begin(); _it != end(); _it++)
		{
			if (tstrstr(*_it, substr))
				return true;
		}
		return false;
	}
};

class _cPathItem
{
public:
	_cPathItem() : fullpath(0) {};
	~_cPathItem() {
		if (fullpath)
		{
			tstrfree(fullpath);
			fullpath = 0;
		}
	};

	operator tcstring()
	{
		return fullpath;
	}

	tcstring operator = (tcstring _fullpath)
	{
		if (fullpath)
			tstrfree(fullpath);
		fullpath = 0;
		if (_fullpath)
			fullpath = tstrdup(fullpath);
		return fullpath;
	}
	
	bool operator == (tcstring _fullpath)
	{
		return !tstrcmp(_fullpath, fullpath);
	}

public:
	tcstring fullpath;

};

class _cPathPidItem : public _cPathItem
{
public:
	tPid pid;
};
typedef _tPathList<_cPathPidItem> _cPathPidList;
class cPathPidListSafe : private _cPathPidList , private cLock
{
public:

	bool add(tPid pid, tcstring fullpath)
	{
		if (!fullpath)
			return false;
		cAutoLock _lock(*this);
		iterator it = _cPathPidList::add(fullpath);
		if (it == end())
			return false;
		it->pid = pid;
		return true;
	}
	
	bool remove(tPid pid)
	{
		cAutoLock _lock(*this);
		bool res = false;
		for (iterator _it = begin(); _it != end(); )
		{
			if (_it->pid == pid)
			{
				_it = erase( _it );
				res = true;
				continue;
			}
			_it++;
		}
		return res;
	}
	
	bool contain(tcstring fullpath, tPid* ppid)
	{
		cAutoLock _lock(*this);
		iterator it = _cPathPidList::find(fullpath);
		if (it == end())
			return false;
		if (ppid)
			*ppid = it->pid;
		return true;
	}

	bool resolve_contain(cEnvironmentHelper* pEnvironmentHelper, tcstring fullpath, tPid* ppid)
	{
		cAutoLock _lock(*this);
		for (iterator it = _cPathPidList::begin(); it != _cPathPidList::end(); it++)
		{
			if (0 == tstrcmp(cPath(pEnvironmentHelper, it->fullpath).getFull(), fullpath))
			{
				if (ppid)
					*ppid = it->pid;
				return true;
			}
		}
		return false;
	}
	bool containSubstrForPath(tcstring fullpath) 
	{
		return _cPathPidList::containSubstrForPath(fullpath);
	}
	bool containPathForSubstr(tcstring substr)
	{
		return _cPathPidList::containPathForSubstr(substr);
	}
};

class _cFileFormatItem : public _cPathItem
{
public:
	_cFileFormatItem() : pfi(0) {};
	~_cFileFormatItem() {
		if (pfi)
			delete pfi;
	}
	FILE_FORMAT_INFO* pfi;
};
class cFileFormatList : public _tPathList<_cFileFormatItem>
{
public:
	bool add(tcstring fullpath, FILE_FORMAT_INFO* pfi, bool replace_existing = false)
	{
		iterator it = _tPathList<_cFileFormatItem>::add(fullpath);
		if (it == end())
			return false;
		it->pfi = pfi;
		return true;
	}
};

#endif // !defined(AFX_PATHLIST_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
