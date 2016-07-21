// RegAutoRuns.h: interface for the CRegAutoRuns class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGAUTORUNS_H__B4011A8F_FB2A_4CEE_A385_93D145DE1788__INCLUDED_)
#define AFX_REGAUTORUNS_H__B4011A8F_FB2A_4CEE_A385_93D145DE1788__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CRegAutoRuns  
{
public:
	CRegAutoRuns();
	virtual ~CRegAutoRuns();
	bool IsAutoRunsKey(PWCHAR pwchKeyName, PWCHAR pwchValueName);
};

#endif // !defined(AFX_REGAUTORUNS_H__B4011A8F_FB2A_4CEE_A385_93D145DE1788__INCLUDED_)
