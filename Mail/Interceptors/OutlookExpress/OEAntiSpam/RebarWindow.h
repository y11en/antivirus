// RebarWindow.h: interface for the CRebarWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REBARWINDOW_H__7A5C3256_227E_4B95_A9D4_0A6CB7ADE355__INCLUDED_)
#define AFX_REBARWINDOW_H__7A5C3256_227E_4B95_A9D4_0A6CB7ADE355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <commctrl.h>

namespace
{
	typedef CWinTraits<WS_OVERLAPPEDWINDOW, 0>	CRebarWndTraits;
}

class CRebarWindow : public CWindowImpl<CRebarWindow, CWindow, CRebarWndTraits>
{
public:
	CRebarWindow();
	virtual ~CRebarWindow();

	void Initialize();
	
	DECLARE_WND_CLASS(TEXT("4C37C4F2-9754-441f-A17E-ACEEACEC9551"))
	
	BEGIN_MSG_MAP(CRebarWindow)
		MESSAGE_HANDLER(WM_NOTIFY, OnWmNotify)
	END_MSG_MAP()
		
private:
	LRESULT OnWmNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	std::wstring m_strSpamTooltipW;
	std::wstring m_strProbSpamTooltipW;
	std::wstring m_strConfigTooltipW;

	std::string m_strSpamTooltipA;
	std::string m_strProbSpamTooltipA;
	std::string m_strConfigTooltipA;

	bool m_bInited;
};

#endif // !defined(AFX_REBARWINDOW_H__7A5C3256_227E_4B95_A9D4_0A6CB7ADE355__INCLUDED_)
