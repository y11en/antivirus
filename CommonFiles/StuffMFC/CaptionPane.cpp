// CaptionPane.cpp : implementation file
//

#include "stdafx.h"
#include "CaptionPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptionPane

CCaptionPane::CCaptionPane()
{
	m_crTextColor = RGB(255, 255, 255);
	m_crBgColor = GetSysColor(COLOR_3DSHADOW);
	m_sFontName = "MS Sans Serif";
	m_dwFontStyle = FW_BOLD;
	m_dwFontHeight = 16;
}

CCaptionPane::~CCaptionPane()
{
}


BEGIN_MESSAGE_MAP(CCaptionPane, CStatic)
	//{{AFX_MSG_MAP(CCaptionPane)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptionPane message handlers

BOOL CCaptionPane::OnEraseBkgnd(CDC* pDC) 
{
	try
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		pDC->FillRect(rcClient, &CBrush(m_crBgColor));

		return TRUE;
	}
	catch(...)
	{
	}

	return FALSE;
}

void CCaptionPane::OnPaint() 
{
	try
	{
		CPaintDC dc(this); // device context for painting

		CRect R;
		GetClientRect(&R);
		R.left += 2;

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(m_crTextColor);

		CFont FTitle;
		FTitle.CreateFont(m_dwFontHeight, 0, 0, 0, m_dwFontStyle, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, m_sFontName);
		CFont *pFOld = dc.SelectObject(&FTitle);

		CSize SText = dc.GetTextExtent(m_sText);
		R.top += (R.bottom - R.top - SText.cy) / 2;

		dc.DrawText(m_sText, R, 0);
		dc.SelectObject(pFOld);
	}
	catch(...)
	{
	}
}

void CCaptionPane::SetText(CString sText)
{
	m_sText = sText;
	Invalidate();
}

void CCaptionPane::SetTextColor(COLORREF crColor)
{
	m_crTextColor = crColor;
	Invalidate();
}

void CCaptionPane::SetBgColor(COLORREF crColor)
{
	m_crBgColor = crColor;
	Invalidate();
}

void CCaptionPane::SetTextStyle(LPCTSTR pszFontName, DWORD dwStyle, DWORD dwHeight)
{
	if (pszFontName && *pszFontName) m_sFontName = pszFontName;
	m_dwFontStyle = dwStyle;
	m_dwFontHeight = dwHeight;

	Invalidate();
}
