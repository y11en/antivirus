#if !defined(AFX_COLORCTRL_H__CA4DE73C_CDC9_11D3_B261_00104BB13A66__INCLUDED_)
#define AFX_COLORCTRL_H__CA4DE73C_CDC9_11D3_B261_00104BB13A66__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TemplDef.h" // message map extensions for templates

///////////////////////////////////////////////////////////////////////////////
// class CColorCtrl
//
// Author: Yury Goltsman
// email:  ygprg@hotmail.com
// Copyright © 2000, Yury Goltsman
//
// This code provided "AS IS," without any kind of warranty.
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// version : 1.2
// bug fixing
// added typedefs for some mfc controls
// derived templates CColorCtrlEx and CBlinkCtrlEx with initial 
// template parameters added
// message map macro added
//
// version : 1.1
// bug fixing
//

enum
{
	CC_BLINK_NOCHANGE = 0,
	CC_BLINK_FAST     = 500,
	CC_BLINK_NORMAL   = 1000,
	CC_BLINK_SLOW     = 2000
};

enum
{
	CC_BLINK_TEXT = 1,
	CC_BLINK_BK   = 2,
	CC_BLINK_BOTH = CC_BLINK_TEXT|CC_BLINK_BK
};

#define CC_SYSCOLOR(ind) (0x80000000|((ind) & ~CLR_DEFAULT))

template<class BASE_TYPE = CWnd>
class CColorCtrl : public BASE_TYPE
{
public:
	CColorCtrl();
	virtual ~CColorCtrl(){}

public:

	void SetTextColor(COLORREF rgbText = CLR_DEFAULT);
	COLORREF GetTextColor(){ return GetColor(m_rgbText); }
	void SetTextBlinkColors(COLORREF rgbBlinkText1, COLORREF rgbBlinkText2);

	void SetBkColor(COLORREF rgbBk = CLR_DEFAULT);
	COLORREF GetBkColor(){ return GetColor(m_rgbBk); }
	void SetBkBlinkColors(COLORREF rgbBlinkBk1, COLORREF rgbBlinkBk2);

	void StartBlink(int iWho = CC_BLINK_BOTH, UINT nDelay = CC_BLINK_NOCHANGE);
	void StopBlink(int iWho = CC_BLINK_BOTH);
	UINT GetDelay() { return m_nDelay; }

protected:
	void SetBlinkTimer()
	{
		// set timer if blinking mode
		if(m_nTimerID <= 0 && (m_fBlinkText || m_fBlinkBk))
			m_nTimerID = SetTimer(1, m_nDelay, NULL);
	}
	void KillBlinkTimer()
	{
		// reset timer
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
		m_iBlinkPhase = 0;
	}
	COLORREF GetColor(COLORREF clr)
	{
		DWORD mask = clr & CLR_DEFAULT;
		if(mask == 0x80000000)
			return ::GetSysColor(clr & ~CLR_DEFAULT); // system color
		if(mask == CLR_DEFAULT)
			return CLR_DEFAULT;										 // default color
		return clr & ~CLR_DEFAULT;									 // normal color
	}
protected:
	UINT m_nTimerID;
	int m_iBlinkPhase;
	UINT m_nDelay;
	BOOL m_fBlinkText;
	BOOL m_fBlinkBk;

	COLORREF m_rgbText;
	COLORREF m_rgbBlinkText[2];

	COLORREF m_rgbBk;
	COLORREF m_rgbBlinkBk[2];
	CBrush m_brBk;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CColorCtrl)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_TEMPLATE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
typedef CColorCtrl<CEdit>      CColoredEdit;
typedef CColorCtrl<CButton>    CColoredButton;
typedef CColorCtrl<CStatic>    CColorEdit;
typedef CColorCtrl<CScrollBar> CColoredScrollBar;
typedef CColorCtrl<CListBox>   CColoredListBox;
typedef CColorCtrl<CComboBox>  CColoredComboBox;
typedef CColorCtrl<CDialog>    CColoredDialog;
/////////////////////////////////////////////////////////////////////////////

template<class BASE_TYPE>
CColorCtrl<BASE_TYPE>::CColorCtrl()
{
	// set control to non-blinking mode
	m_nTimerID = 0;
	m_iBlinkPhase = 0;
	m_nDelay = CC_BLINK_NORMAL;

	m_fBlinkText = FALSE;
	m_fBlinkBk = FALSE;

	// set foreground colors
	m_rgbText = CLR_DEFAULT;
	m_rgbBlinkText[0] = m_rgbText;
	m_rgbBlinkText[1] = m_rgbText;

	// set background colors
	m_rgbBk = CLR_DEFAULT;
	m_rgbBlinkBk[0] = m_rgbBk;
	m_rgbBlinkBk[1] = m_rgbBk;
}

BEGIN_TEMPLATE_MESSAGE_MAP(class BASE_TYPE, CColorCtrl<BASE_TYPE>, BASE_TYPE)
	//{{AFX_MSG_MAP(CColorCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_TEMPLATE_MESSAGE_MAP()


template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::SetTextColor(COLORREF rgbText)
{
	m_rgbText = rgbText;
	if(::IsWindow(m_hWnd))
		Invalidate();
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::SetTextBlinkColors(COLORREF rgbBlinkText1, COLORREF rgbBlinkText2)
{
	m_rgbBlinkText[0] = rgbBlinkText1;
	m_rgbBlinkText[1] = rgbBlinkText2;
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::SetBkColor(COLORREF rgbBk)
{
	m_rgbBk = rgbBk;

	if(::IsWindow(m_hWnd))
		Invalidate();
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::SetBkBlinkColors(COLORREF rgbBlinkBk1, COLORREF rgbBlinkBk2)
{
	m_rgbBlinkBk[0] = rgbBlinkBk1;
	m_rgbBlinkBk[1] = rgbBlinkBk2;
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::StartBlink(int iWho, UINT nDelay)
{
	if(iWho & CC_BLINK_TEXT)
		m_fBlinkText = TRUE;
	if(iWho & CC_BLINK_BK)
		m_fBlinkBk = TRUE;

	if(nDelay != CC_BLINK_NOCHANGE)
	{
		m_nDelay = nDelay;
		if (m_nTimerID > 0)
		{
			ASSERT(::IsWindow(m_hWnd));
			// reset old timer if delay changed
			KillBlinkTimer();
		}
	}
	ASSERT(m_fBlinkText||m_fBlinkBk);
	if(!::IsWindow(m_hWnd))
		return;
	// if no timer - set it
	SetBlinkTimer();
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::StopBlink(int iWho)
{
	if(iWho & CC_BLINK_TEXT)
		m_fBlinkText = FALSE;
	if(iWho & CC_BLINK_BK)
		m_fBlinkBk = FALSE;

	if(m_nTimerID > 0 && !m_fBlinkText && !m_fBlinkBk)
	{
		ASSERT(::IsWindow(m_hWnd));
		// stop timer if no blinking and repaint
		KillBlinkTimer();
		Invalidate();
	}
}

template<class BASE_TYPE>
HBRUSH CColorCtrl<BASE_TYPE>::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// Get foreground color
	COLORREF rgbText = GetColor(m_fBlinkText ? m_rgbBlinkText[m_iBlinkPhase] 
	                                         : m_rgbText);
	// Get background color
	COLORREF rgbBk = GetColor(m_fBlinkBk ? m_rgbBlinkBk[m_iBlinkPhase] 
	                                     : m_rgbBk);
	
	// if both colors are default - use default colors
	if(rgbText == CLR_DEFAULT && rgbBk == CLR_DEFAULT)
		return 0;

	// if one of colors is default - get system color for it
	if(rgbBk == CLR_DEFAULT)
	{
		// text color specified and background - not.
		switch(nCtlColor)
		{
			case CTLCOLOR_EDIT:
			case CTLCOLOR_LISTBOX:
				rgbBk = GetSysColor(COLOR_WINDOW);
				break;
			case CTLCOLOR_BTN:
			case CTLCOLOR_DLG:
			case CTLCOLOR_MSGBOX:
			case CTLCOLOR_STATIC:
			default:
				rgbBk = ::GetSysColor(COLOR_BTNFACE);
				break;
			case CTLCOLOR_SCROLLBAR:
				// for scroll bar no meaning in text color - use parent OnCtlColor
				return 0; 
		}
	}
	else if(rgbText == CLR_DEFAULT)
	{
		// background color specified and text - not.
		switch(nCtlColor)
		{
			default:
				rgbText = ::GetSysColor(COLOR_WINDOWTEXT);
				break;
			case CTLCOLOR_BTN:
				rgbText = ::GetSysColor(COLOR_BTNTEXT);
				break;
		}
	}

	ASSERT(rgbText != CLR_DEFAULT);
	ASSERT(rgbBk != CLR_DEFAULT);
	
	// set colors
	pDC->SetTextColor(rgbText);
	pDC->SetBkColor(rgbBk);
	pDC->SetBkMode(TRANSPARENT);
	// update brush
	m_brBk.DeleteObject();
	m_brBk.CreateSolidBrush(rgbBk);

	return (HBRUSH)m_brBk;
}

template<class BASE_TYPE>
HBRUSH CColorCtrl<BASE_TYPE>::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(pWnd->m_hWnd == m_hWnd)
		return BASE_TYPE::OnCtlColor(pDC, pWnd, nCtlColor);	// for dialogs
	return BASE_TYPE::OnCtlColor(pDC, this, nCtlColor);		// send reflect message
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::OnDestroy()
{
	// kill timer
	KillBlinkTimer();
	BASE_TYPE::OnDestroy();
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::OnTimer(UINT nIDEvent)
{
	if (nIDEvent != m_nTimerID)
	{
		BASE_TYPE::OnTimer(nIDEvent);
		return;
	}

	// if there is active blinking
	if (m_fBlinkBk || m_fBlinkText)
	{
		// change blinking phase and repaint
		m_iBlinkPhase = !m_iBlinkPhase;
		Invalidate();
	}
}

template<class BASE_TYPE>
void CColorCtrl<BASE_TYPE>::PreSubclassWindow() 
{
	// set timer if blinking mode
	SetBlinkTimer();
	BASE_TYPE::PreSubclassWindow();
}

template<class BASE_TYPE>
int CColorCtrl<BASE_TYPE>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (BASE_TYPE::OnCreate(lpCreateStruct) == -1)
		return -1;
	// set timer if blinking mode
	SetBlinkTimer();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CColorCtrlEx

template<class BASE_TYPE, int InitialTextColor = CLR_DEFAULT, int InitialBkColor = CLR_DEFAULT>
class CColorCtrlEx : public CColorCtrl<BASE_TYPE>
{
public:
	CColorCtrlEx()
	{
		m_rgbText = InitialTextColor;
		m_rgbBk = InitialBkColor;
	}
};

///////////////////////////////////////////////////////////////////////////////
// class CBlinkCtrlEx

template<class BASE_TYPE, 
         int InitialTextColor0 = CLR_DEFAULT, 
				 int InitialTextColor1 = CLR_DEFAULT, 
				 int InitialBkColor0 = CLR_DEFAULT,
				 int InitialBkColor1 = CLR_DEFAULT,
				 int InitialDelay = CC_BLINK_NORMAL>
class CBlinkCtrlEx : public CColorCtrl<BASE_TYPE>
{
public:
	CBlinkCtrlEx()
	{
		m_nDelay = InitialDelay;
		m_rgbBlinkText[0] = InitialTextColor0;
		m_rgbBlinkText[1] = InitialTextColor1;
		if(InitialTextColor0 != CLR_DEFAULT || InitialTextColor1 != CLR_DEFAULT)
			m_fBlinkText = TRUE;
		
		m_rgbBlinkBk[0] = InitialBkColor0;
		m_rgbBlinkBk[1] = InitialBkColor1;
		if(InitialBkColor0 != CLR_DEFAULT || InitialBkColor1 != CLR_DEFAULT)
			m_fBlinkBk = TRUE;
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCTRL_H__CA4DE73C_CDC9_11D3_B261_00104BB13A66__INCLUDED_)

