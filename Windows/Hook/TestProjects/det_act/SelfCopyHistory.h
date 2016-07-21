// SelfCopyHistory.h: interface for the CSelfCopyHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELFCOPYHISTORY_H__518BF808_E76B_4C00_93C1_6A0F1989DF36__INCLUDED_)
#define AFX_SELFCOPYHISTORY_H__518BF808_E76B_4C00_93C1_6A0F1989DF36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

struct tStrWithLo
{
	int m_Idx;
	PWCHAR m_pwchStr;
	PWCHAR m_pwchStrLo;
};

typedef std::vector<tStrWithLo> _tvecStrIdx;

struct tSelfCopyHistory
{
	int m_ImagePathIdx;
	int m_DestinationPathIdx;
	int m_ParentImagePathIdx;
	DWORD m_dwOffset;
};

typedef std::vector<tSelfCopyHistory> _tvecSelfCopyHistory;

class CSelfCopyHistory  
{
public:
	CSelfCopyHistory();
	virtual ~CSelfCopyHistory();

	bool AddEvent(PWCHAR pwchImagePath, PWCHAR pwchDestinationPath, PWCHAR ParentImagePath, DWORD pos);

	bool Contain(PWCHAR pwchCmdLine);
	bool IsCopy2Path(PWCHAR pwchPath, PWCHAR* ppwchImagePath);

private:
	int						m_nNextIdx;
	_tvecStrIdx				m_IdxToStr;
	_tvecSelfCopyHistory	m_SelfCopyHistory;
	
	int GetStrIdx(PWCHAR pwchStr, bool bAllowCreate);
	int FindStr(PWCHAR pwchLowerStr);
	int RegisterStr(PWCHAR pwchStr, PWCHAR pwchLowerStr);
};

#endif // !defined(AFX_SELFCOPYHISTORY_H__518BF808_E76B_4C00_93C1_6A0F1989DF36__INCLUDED_)
