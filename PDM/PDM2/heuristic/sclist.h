// sclist.h: interface for the sclist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_sclist_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
#define AFX_sclist_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
	
class cSelfCopyList;

#include <vector>
#include "../eventmgr/include/tstring.h"
#include "../eventmgr/include/lock.h"
#include "proclist.h"

typedef struct tag_SELFCOPY_LIST_ITEM
{
	tcstring fullpath;
	tPid   pid;
} SELFCOPY_LIST_ITEM;

typedef std::vector<SELFCOPY_LIST_ITEM> tsclvector;

class cSelfCopyList
{
public:
	cSelfCopyList();
	~cSelfCopyList();
	
public:
	bool add(tPid pid, tcstring fullname);
	bool remove(tPid pid);
	bool contain(tcstring str, bool bSubstr, tPid* ppid);
//	size_t count();
//	SELFCOPY_LIST_ITEM& operator[](size_t index);
	
private:
	tsclvector m_list;
	cLock m_Sync;
};
*/


#endif // !defined(AFX_sclist_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
