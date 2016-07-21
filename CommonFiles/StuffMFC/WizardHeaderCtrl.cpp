// WizardHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WizardHeaderCtrl.h"
#include <debugging.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardHeaderCtrl

CWizardHeaderCtrl::CWizardHeaderCtrl()
{
	m_crBackground = GetSysColor(COLOR_WINDOW);
	m_crText = GetSysColor(COLOR_WINDOWTEXT);
	m_dwHBitmapAlignment = BA_RIGHT;
	m_dwVBitmapAlignment = BA_VCENTRE;
	m_pPicture = NULL;
	m_hIcon = NULL;
	m_dwTitleFontHeight = 18;
	m_dwTextFontHeight = 8;
	m_sFontFaceName = "MS Sans Serif";
	m_bDrawBorder = TRUE;
	
	m_nPictureCX = BA_H_MARGIN;
	m_nPictureCY = BA_V_MARGIN;
	m_nTextCX = TEXT_MARGIN;
	m_nTextCY = TEXT_V_MARGIN;
}

BEGIN_MESSAGE_MAP(CWizardHeaderCtrl, CStatic)
//{{AFX_MSG_MAP(CWizardHeaderCtrl)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardHeaderCtrl message handlers

void CWizardHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);
	
	CRect R, RPic, RText;
	GetClientRect(&R);
	
	dc.FillSolidRect(R, m_crBackground);

	RText = R;
	RText.bottom -= 2;
	RText.top += m_nTextCY;
	RPic = R;
	RPic.bottom -= 2;
	RPic.left += m_nPictureCX;
	RPic.top += m_nPictureCY;

	if (m_pPicture)
	{
		BITMAP BmpInfo;
		::GetObject((HBITMAP)*m_pPicture, sizeof(BITMAP), &BmpInfo);

		switch(m_dwVBitmapAlignment) {
			case BA_TOP:
				RPic.bottom = RPic.top + BmpInfo.bmHeight;
			break;
			case BA_BOTTOM:
				RPic.top = RPic.bottom - BmpInfo.bmHeight;
			break;
			case BA_VCENTRE:
				int cy = ((RPic.bottom - RPic.top) - BmpInfo.bmHeight) / 2;
				if (cy > 0)
				{
					RPic.bottom -= cy;
					RPic.top += cy;
				}
			break;
		}

		switch(m_dwHBitmapAlignment) {
			case BA_LEFT:
				RPic.right = RPic.left + BmpInfo.bmWidth;
				RText.left = RPic.right + TEXT_MARGIN;
			break;
			case BA_RIGHT:
				RPic.left = RPic.right - BmpInfo.bmWidth;
				RText.right = RPic.left - TEXT_MARGIN;
				RText.left += TEXT_MARGIN;
			break;
			case BA_STRETCH:
			break;
		}

		DrawBitmap(&dc, RPic, SRCCOPY, m_pPicture);
	}

	if (m_hIcon)
	{
		switch(m_dwVBitmapAlignment) {
			case BA_TOP:
				RPic.bottom = RPic.top + 32;
			break;
			case BA_BOTTOM:
				RPic.top = RPic.bottom - 32;
			break;
			case BA_VCENTRE:
				int cy = ((RPic.bottom - RPic.top) - 32) / 2;
				if (cy > 0)
				{
					RPic.bottom -= cy;
					RPic.top += cy;
				}
			break;
		}

		switch(m_dwHBitmapAlignment) {
			case BA_LEFT:
				RPic.right = RPic.left + 32;
				RText.left = RPic.right + m_nTextCX;
			break;
			case BA_RIGHT:
				RPic.left = RPic.right - 32;
				RText.right = RPic.left - m_nTextCX;
				RText.left += m_nTextCX;
			break;
			case BA_STRETCH:
			break;
		}

		dc.DrawIcon(RPic.left, RPic.top, m_hIcon);
	}

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_crText);

	HFONT hOldFont = (HFONT)SendMessage(WM_GETFONT, 0, 0);

	// Draw Title text
	CFont FTitle;
	FTitle.CreateFont(m_dwTitleFontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
					  ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
					  DEFAULT_PITCH | FF_SWISS, m_sFontFaceName);

	CRect RTitle;

	CFont *pFOld = dc.SelectObject(&FTitle);

	RTitle = RText;
	CSize SText = dc.GetTextExtent(m_sTitle);
	RTitle.bottom = RTitle.top + SText.cy;

	dc.DrawText(m_sTitle, RTitle, DT_LEFT | DT_WORDBREAK);

	// Draw Text

	dc.SelectObject(CFont::FromHandle(hOldFont));

	RText.top = RTitle.bottom + TEXT_V_MARGIN;
	RText.left += 4 * TEXT_MARGIN;
	dc.DrawText(m_sText, RText, DT_LEFT | DT_WORDBREAK);

	dc.SelectObject(pFOld);

	if (m_bDrawBorder)
	{
		CPen P(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));

		CPen* pPOld = dc.SelectObject(&P);
		dc.MoveTo(R.left, R.bottom - 2);
		dc.LineTo(R.right, R.bottom - 2);

		dc.SelectObject(pPOld);
	}
}

void CWizardHeaderCtrl::DrawBitmap(CDC *pDC, CRect R, UINT nFlags, CBitmap *pBitmap)
{
	CDC memDC;
	CBitmap* pOld=NULL;
	memDC.CreateCompatibleDC(pDC);

	ASSERT(pBitmap->m_hObject != NULL);

	BITMAP BmpInfo;
	::GetObject((HBITMAP)*pBitmap, sizeof(BITMAP), &BmpInfo);
	
	pOld = memDC.SelectObject((CBitmap*)pBitmap);
	if (pOld == NULL) return; //Destructors will clean up
	
	pDC->StretchBlt(R.left, R.top, R.Width(), R.Height(), &memDC, 0, 0, BmpInfo.bmWidth, BmpInfo.bmHeight, nFlags);
	
	memDC.SelectObject(pOld);	
}

void CWizardHeaderCtrl::SetTitleFontFace(CString sFontFace, BOOL bRedraw)
{
	m_sFontFaceName = sFontFace;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetTitle(CString sTitle, BOOL bRedraw)
{
	m_sTitle = sTitle;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetText(CString sText, BOOL bRedraw)
{
	m_sText = sText;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetTextColor(COLORREF col, BOOL bRedraw)
{
	m_crText = col;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetBgColor(COLORREF col, BOOL bRedraw)
{
	m_crBackground = col;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetTitleFontHeight(DWORD dwHeight, BOOL bRedraw)
{
	m_dwTitleFontHeight = dwHeight;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetAlignment(DWORD dwHAligh, DWORD dwVAligh, BOOL bRedraw)
{
	m_dwHBitmapAlignment = dwHAligh;
	m_dwVBitmapAlignment = dwVAligh;

	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetPicture(CBitmap *pPicture, BOOL bRedraw)
{
	m_pPicture = pPicture;
	m_hIcon = NULL;
	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::Redraw()
{
	Invalidate(TRUE);
	UpdateWindow();
}

void CWizardHeaderCtrl::SetIcon(HICON hIcon, BOOL bRedraw)
{
	m_hIcon = hIcon;
	m_pPicture = NULL;
	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetMargins(int nPictureCX, int nPictureCY, int nTextCX, int nTextCY, BOOL bRedraw)
{
	m_nPictureCX = nPictureCX;
	m_nPictureCY = nPictureCY;
	m_nTextCY = nTextCY;
	m_nTextCX = nTextCX;
	if (bRedraw) Redraw();
}

void CWizardHeaderCtrl::SetDrawBorder(BOOL bDraw, BOOL bRedraw)
{
	m_bDrawBorder = bDraw;
	if (bRedraw) Redraw();
}
