// RegHistory.h: interface for the CRegHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGHISTORY_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
#define AFX_REGHISTORY_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

struct tStrReg
{
	PWCHAR m_pwchRegKeyName;
	PWCHAR m_pwchRegValueName;
	PWCHAR m_pwchRegValue;
	PWCHAR m_pwchRegValueReparsedFilename;
};

typedef std::vector<tStrReg> _tvecStrReg;

class CRegHistory  
{
public:
	CRegHistory();
	virtual ~CRegHistory();
	
public:
	int AddEvent(PWCHAR pwchRegKeyName, PWCHAR pwchRegValueName, PWCHAR pwchRegValue, tStrReg* pResult, int nMaxResults);
	bool Contain(PWCHAR pwchFileName, tStrReg* pResult = NULL);

private:
	_tvecStrReg m_StrReg;
};

#endif // !defined(AFX_REGHISTORY_H__6667244F_A631_4560_AE6A_37A4F9798DED__INCLUDED_)
