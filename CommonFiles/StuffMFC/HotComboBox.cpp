// HotComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "HotComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHotComboBox

IMPLEMENT_DYNCREATE(CHotComboBox, CComboBox)

CHotComboBox::CHotComboBox()
{
	m_clr3DFace = GetSysColor(COLOR_3DFACE);
	m_clr3DLight = GetSysColor(COLOR_3DLIGHT);
	m_clr3DHilight = GetSysColor(COLOR_3DHILIGHT);
	m_clr3DShadow = GetSysColor(COLOR_3DSHADOW);
	m_clr3DDkShadow = GetSysColor(COLOR_3DDKSHADOW);

	m_fGotFocus = false;
	m_fTimerSet = false;
	m_bAutoComplete = TRUE;
}

CHotComboBox::~CHotComboBox()
{
}


BEGIN_MESSAGE_MAP(CHotComboBox, CComboBox)
	//{{AFX_MSG_MAP(CHotComboBox)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_NCMOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHotComboBox message handlers

void CHotComboBox::OnPaint() 
{
	Default();

	if (m_fGotFocus) {
		DrawBorder();
	} else {
		DrawBorder(false);
	}
}

void CHotComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBox::OnSetFocus(pOldWnd);
	
	m_fGotFocus = true;
	DrawBorder();
}

void CHotComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	m_fGotFocus = false;
	DrawBorder(false);
}

void CHotComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_fGotFocus) {
		if (!m_fTimerSet) {
			DrawBorder();
			SetTimer(1, 10, NULL);
			m_fTimerSet = true;
		}
	}
	
	CComboBox::OnMouseMove(nFlags, point);
}

void CHotComboBox::OnTimer(UINT nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(&rcItem);

	if(!rcItem.PtInRect(pt)) {
		KillTimer(1);

		m_fTimerSet = false;

		if (!m_fGotFocus) {
			DrawBorder(false);
		}
		return;
	}
	
	CComboBox::OnTimer(nIDEvent);
}

void CHotComboBox::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	if (!m_fGotFocus) {
		if (!m_fTimerSet) {
			DrawBorder();
			SetTimer(1, 10, NULL);
			m_fTimerSet = true;
		}
	}
	
	CComboBox::OnNcMouseMove(nHitTest, point);
}

void CHotComboBox::OnSysColorChange() 
{
	CComboBox::OnSysColorChange();
	
	m_clr3DFace = GetSysColor(COLOR_3DFACE);
	m_clr3DLight = GetSysColor(COLOR_3DLIGHT);
	m_clr3DHilight = GetSysColor(COLOR_3DHILIGHT);
	m_clr3DShadow = GetSysColor(COLOR_3DSHADOW);
	m_clr3DDkShadow = GetSysColor(COLOR_3DDKSHADOW);
}

void CHotComboBox::OnSetfocus() 
{
	m_fGotFocus = true;
	DrawBorder();
}

void CHotComboBox::OnKillfocus() 
{
	m_fGotFocus = false;
	DrawBorder(false);
}

void CHotComboBox::DrawBorder(bool fHot)
{
	CDC *pDC = GetDC();
	CRect rcItem;
	DWORD dwExStyle = GetExStyle();

	GetWindowRect(&rcItem);
	ScreenToClient(&rcItem);

	if (!IsWindowEnabled()) {
		fHot = false;
//		fHot = true;
	}

	// make sure that rcItem start at the smallest point
	if (dwExStyle & (WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME)) {
		if (dwExStyle & WS_EX_STATICEDGE) {
			rcItem.DeflateRect(1, 1, 0, 0);
		}
		if (dwExStyle & WS_EX_DLGMODALFRAME) {
			rcItem.DeflateRect(1, 1, 0, 0);
		}
		rcItem.DeflateRect(1, 1, 0, 0);
	} else {
		rcItem.DeflateRect(1, 1);
	}

	if (fHot) {
		pDC->Draw3dRect(rcItem, m_clr3DDkShadow, m_clr3DLight);
		rcItem.InflateRect(1, 1);
		pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DHilight);

		if (dwExStyle & WS_EX_DLGMODALFRAME) {
			pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DFace);
			rcItem.InflateRect(1, 1, 0, 0);
			if (dwExStyle & WS_EX_STATICEDGE) {
				rcItem.DeflateRect(0, 0, 2, 2);
			} else {
				rcItem.DeflateRect(0, 0, 1, 1);
			}
			pDC->Draw3dRect(rcItem, m_clr3DDkShadow, m_clr3DDkShadow);
		}

		if (dwExStyle & WS_EX_STATICEDGE) {
			if (dwExStyle & WS_EX_DLGMODALFRAME) {
				rcItem.InflateRect(1, 1);
			} else {
				rcItem.InflateRect(1, 1, 0, 0);
			}
			pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DHilight);
		}
	} else {
		pDC->Draw3dRect(rcItem, m_clr3DHilight, m_clr3DFace);
		rcItem.InflateRect(1, 1);
		pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DHilight);

//		if (dwExStyle & WS_EX_DLGMODALFRAME) {
//			rcItem.InflateRect(1, 1, 0, 0);
//			if (dwExStyle & WS_EX_STATICEDGE) {
//				rcItem.InflateRect(0, 0, 1, 1);
//				pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
//				rcItem.DeflateRect(0, 0, 3, 3);
//			} else {
//				rcItem.DeflateRect(0, 0, 1, 1);
//			}
//			pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
//		}
//
//		if (dwExStyle & WS_EX_STATICEDGE) {
//			if (dwExStyle & WS_EX_DLGMODALFRAME) {
//				rcItem.InflateRect(1, 1);
//			} else {
//				rcItem.InflateRect(1, 1, 0, 0);
//			}
//			pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
//		}
	}

	ReleaseDC(pDC);
}

BOOL CHotComboBox::PreTranslateMessage(MSG* pMsg)
{
	// Need to check for backspace/delete. These will modify the text in
	// the edit box, causing the auto complete to just add back the text
	// the user has just tried to delete. 

	if (pMsg->message == WM_KEYDOWN)
	{
		m_bAutoComplete = TRUE;

		int nVirtKey = (int) pMsg->wParam;
		if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
			m_bAutoComplete = FALSE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CHotComboBox::OnEditUpdate() 
{
  // if we are not to auto update the text, get outta here
  if (!m_bAutoComplete) 
      return;

  // Get the text in the edit box
  CString str;
  GetWindowText(str);
  int nLength = str.GetLength();
  
  // Currently selected range
  DWORD dwCurSel = GetEditSel();
  WORD dStart = LOWORD(dwCurSel);
  WORD dEnd   = HIWORD(dwCurSel);

  // Search for, and select in, and string in the combo box that is prefixed
  // by the text in the edit box
  if (SelectString(-1, str) == CB_ERR)
  {
      SetWindowText(str);		// No text selected, so restore what was there before
      if (dwCurSel != CB_ERR)
        SetEditSel(dStart, dEnd);	//restore cursor postion
  }

  // Set the text selection as the additional text that we have added
  if (dEnd < nLength && dwCurSel != CB_ERR)
      SetEditSel(dStart, dEnd);
  else
      SetEditSel(nLength, -1);
}
