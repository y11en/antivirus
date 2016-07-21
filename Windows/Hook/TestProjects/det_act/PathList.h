// CPathList.h: interface for the CPathList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETPATH_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
#define AFX_NETPATH_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include "StrList.h"

typedef std::vector<PWCHAR> tPathList;

class CPathList
{
public:
	CPathList();
	virtual ~CPathList();
	
public:
	bool Add(PWCHAR pwchPath);
	bool Remove(PWCHAR pwchPath);
	int  Count();
	bool ContainPath(PWCHAR pwchPath);
	bool ContainBeginningForPath(PWCHAR pwchPath);
	bool ContainPathStartingWith(PWCHAR pwchPathStart);
	bool ContainPathForSubstr(PWCHAR pwchSubstr);
	bool ContainSubstrForPath(PWCHAR pwchPath);
	bool IsNetworkPath(PWCHAR pwchPath);

private:
	tPathList m_List;
};

#endif // !defined(AFX_NETPATH_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
