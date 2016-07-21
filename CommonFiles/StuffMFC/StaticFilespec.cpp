/*
 *  StaticFilespec.cpp
 *
 *  CStaticFilespec implementation
 *    A simple class for displaying long filespecs in static text controls
 *
 *  Usage:          See http://www.codeguru.com/controls/cstatic_filespec.shtml
 *  Author:         Ravi Bhavnani
 *  Edit history:   24-Nov-1997   Ravi B    Original creation
 */

#include "stdafx.h"
#include "StaticFilespec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticFilespec

CStaticFilespec::CStaticFilespec(DWORD  dwFormat/* = DT_LEFT | DT_NOPREFIX | DT_VCENTER */, BOOL bPathEllipsis/* = FALSE */)
{
	m_bPathEllipsis = bPathEllipsis;
	m_dwFormat = dwFormat;
	m_crBg = ::GetSysColor (COLOR_3DFACE);
	m_bBold = FALSE;
}

BEGIN_MESSAGE_MAP(CStaticFilespec, CWnd)
	//{{AFX_MSG_MAP(CStaticFilespec)
	ON_MESSAGE( WM_SETTEXT, OnSetText )
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticFilespec message handlers

BOOL CStaticFilespec::OnEraseBkgnd(CDC* pDC) 
{
	RECT rectWnd;
	
	GetClientRect (&rectWnd);
	pDC->FillSolidRect (&rectWnd, m_crBg);	// Erase background

	return (TRUE);
}

void CStaticFilespec::OnPaint() 
{
	CPaintDC dc(this);				// device context for painting
	
	CFont* pOld = NULL;				// Set default font
	CFont Font;
	CWnd* pWndParent = GetParent();	// parent window
	if (pWndParent) pOld = dc.SelectObject(pWndParent->GetFont());
	
	if(m_bBold)
	{
		LOGFONT lf;
		CFont* pFont = GetFont();

		if (!pFont) return; // nothing to do
		
		pFont->GetLogFont(&lf);

		lf.lfWeight = FW_BOLD;
		Font.CreateFontIndirect(&lf);
		dc.SelectObject(&Font);
	}
	
	// Draw text

	CString strText;				// window text
	GetWindowText(strText);

	RECT rectWnd;					// window rectangle
	GetClientRect(&rectWnd);

	dc.SetBkMode(TRANSPARENT);
	::DrawTextEx(dc.m_hDC, (LPTSTR)(LPCTSTR)strText, strText.GetLength(), &rectWnd, m_dwFormat | (m_bPathEllipsis?DT_PATH_ELLIPSIS:DT_END_ELLIPSIS), NULL);

	if (pOld) dc.SelectObject(pOld);
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CStaticFilespec::OnSetText(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(WM_SETTEXT, wParam, lParam);
	Invalidate();

	return (TRUE);
}
   
// End StaticFilespec.cpp

void CStaticFilespec::SetBgColor(COLORREF cr)
{
	m_crBg = cr;
}

void CStaticFilespec::SetBold(BOOL bBold)
{
	m_bBold = bBold;
	Invalidate();
}
