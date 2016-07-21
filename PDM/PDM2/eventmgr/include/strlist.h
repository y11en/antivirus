// strlist.h: interface for the strlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_strlist_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
#define AFX_strlist_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "tstring.h"
#include "lock.h"

typedef std::vector<tstring> tstrvector;

class cStrList
{
public:
	cStrList();
	~cStrList();
	
public:
	bool add(tcstring str);
	bool remove(tcstring str);
	bool contain(tcstring str, bool bSubstr);
private:
	tstrvector m_list;
	cLock m_Sync;
};

#endif // !defined(AFX_strlist_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
