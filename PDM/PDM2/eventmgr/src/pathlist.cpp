#include "../include/pathlist.h"

//+ ------------------------------------------------------------------------------------------

cPathList::cPathList()
{
}

//+ ------------------------------------------------------------------------------------------

cPathList::~cPathList()
{
	for (tPathList::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
		tstrfree(*_it);
}

//+ ------------------------------------------------------------------------------------------

bool cPathList::add(tcstring path)
{
	if (!path)
		return false;
	
	if (!*path)
		return false;
	
	if (contain(path))
		return true;

	tstring copy = tstrdup(path);
	if (!copy)
		return false;
	
	m_list.push_back(copy);
	return true;
}

size_t cPathList::count()
{
	return m_list.size();
}

//+ ------------------------------------------------------------------------------------------

bool cPathList::contain(tcstring path)
{
	for (tPathList::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		if (0 == tstrcmp(*_it, path))
			return true;
	}
	return false;
}

bool cPathList::remove(tcstring path)
{
	for (tPathList::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		if (0 == tstrcmp(*_it, path))
		{
			tstrfree(*_it);
			m_list.erase(_it);
			return true;
		}
	}
	return false;
}

//+ ------------------------------------------------------------------------------------------

//bool cPathList::containPathForSubstr(tcstring substr)
//{
//	for (tPathList::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
//	{
//		if (tstrstr(*_it, substr))
//			return true;
//	}
//	return false;
//}

//+ ------------------------------------------------------------------------------------------

bool cPathList::containSubstrForPath(tcstring path)
{
	for (tPathList::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		if (tstrstr(path, *_it))
			return true;
	}
	return false;
}

//+ ------------------------------------------------------------------------------------------

tcstring cPathList::operator[](size_t index)
{
	if (index >= m_list.size())
		return NULL;
	return m_list[index];
}
