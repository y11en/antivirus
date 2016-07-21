#include "PathList.h"
#include "stuff/debug.h"
#include "stuff/servfuncs.h"



CPathList::CPathList()
{
}

CPathList::~CPathList()
{
	for (tPathList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
		FreeStr(*_it);
}

bool CPathList::Add(PWCHAR pwchPath)
{
	if (!pwchPath)
		return false;
	if (!*pwchPath)
		return false;
	PWCHAR pwchLongPath = g_Service.GetLongPathName(pwchPath);
	if (!pwchLongPath)
		return false;


	if (GetFileAttributes(pwchLongPath) & FILE_ATTRIBUTE_DIRECTORY)
	{
		int len = wcslen(pwchLongPath);
		if (pwchLongPath[len-1] != '\\')
		{
			PWCHAR pwchTmp = (PWCHAR)global_Alloc((len + 2)*sizeof(WCHAR));
			if (!pwchTmp)
			{
				return false;
			}
			memcpy(pwchTmp, pwchLongPath, len*sizeof(WCHAR));
			pwchTmp[len] = '\\';
			pwchTmp[len+1] = 0;
			FreeStr(pwchLongPath);
			pwchLongPath = pwchTmp;
		}
	}


	if (ContainPath(pwchLongPath))
	{
		// already contained
		FreeStr(pwchLongPath);
		return true;
	}
	
	m_List.push_back(pwchLongPath);
	return true;
}

int CPathList::Count()
{
	return m_List.size();
}

//+ ------------------------------------------------------------------------------------------

bool CPathList::IsNetworkPath(PWCHAR pwchPath)
{
	PWCHAR p = pwchPath;
	if (NULL == p)
		return false;
	if (p[0] == '\\' && p[1] == '\\')
	{
		if (p[2] != '?' || p[3] != '\\') // \\?\ 
			return true;
		p = p + 4; // cut unicode prefix
		if (p[0] == 'U' && p[1] == 'N' && p[2] == 'C' && p[3] == '\\') // \\?\UNC\ 
			return true;
	}
	return false;
}

//+ ------------------------------------------------------------------------------------------

bool CPathList::ContainPath(PWCHAR pwchPath)
{
	for (tPathList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
	{
		if (ComparePath(*_it, pwchPath, false))
			return true;
	}
	return false;
}

//+ ------------------------------------------------------------------------------------------

bool CPathList::ContainPathForSubstr(PWCHAR pwchSubstr)
{
	for (tPathList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
	{
		PWCHAR pwchLongPath = g_Service.GetLongPathName(*_it);
		if (pwchLongPath) 
		{
			PWCHAR pwchRes = wcsistr(pwchLongPath, pwchSubstr);
			FreeStr(pwchLongPath);
			if (pwchRes)
				return true;
		}
	}
	return false;
}

//+ ------------------------------------------------------------------------------------------

bool CPathList::ContainSubstrForPath(PWCHAR pwchPath)
{
	PWCHAR pwchLongPath = g_Service.GetLongPathName(pwchPath);
	_autoFree _af(&pwchLongPath);
	if (!pwchLongPath)
		return false;
	for (tPathList::iterator _it = m_List.begin(); _it != m_List.end(); _it++)
	{
		if (wcsistr(pwchPath, *_it))
			return true;
	}
	return false;
}

