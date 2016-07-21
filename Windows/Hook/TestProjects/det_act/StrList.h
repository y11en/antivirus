// StrList.h: interface for the StrList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRLIST_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
#define AFX_STRLIST_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include "stuff/OwnSync.h"

typedef std::vector<PWCHAR> tStrList;

class CStrList
{
public:
	CStrList(bool bUniq = true, bool bCaseInsensetive = false) { m_bUniq = bUniq; m_bInsensetive = bCaseInsensetive; };
	virtual ~CStrList();
	
public:
	bool Add(PWCHAR pwcsString);
	bool Remove(PWCHAR pwcsString);
	bool Contain(PWCHAR pwcsString, bool bSubstr);
	bool m_bUniq;
	bool m_bInsensetive;
	COwnSync m_Sync;
private:
	tStrList m_List;
};

#endif // !defined(AFX_STRLIST_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
