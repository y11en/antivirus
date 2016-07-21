// ResizablePage.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResizablePage.h"
#include "Util.h"
#include <windowsx.h>
#include "ResourceHelper.h"
#include "resource.h"

typedef std::map<HWND, std::basic_string<TCHAR> > HWND2STRING_MAP;

struct CCMapWrapper
{
	HWND2STRING_MAP m_TooltipMap;
};

//******************************************************************************
//
//******************************************************************************
void CResizablePage::Construct ()
{
	m_bInitDone = FALSE;
	m_pLayoutRule = NULL;

	TOOLTIP_TIMER_ID = 0;
	m_hwndChild = NULL;

	m_pTitleTip = NULL;

	m_pTooltipMap = new CCMapWrapper;

	m_nLockedIconIndex = -1;
	
}

CResizablePage::~CResizablePage ()
{
	delete m_pTooltipMap;
	delete m_pTitleTip;
}

IMPLEMENT_DYNCREATE(CResizablePage, CPropertyPage)



BEGIN_MESSAGE_MAP(CResizablePage, CPropertyPage)
	//{{AFX_MSG_MAP(CResizablePage)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CResizablePage::OnInitDialog ()
{
	CPropertyPage::OnInitDialog ();
	
	if (m_pTitleTip)
		delete m_pTitleTip;
	m_pTitleTip = new CTitleTip;
	m_pTitleTip->SetWrapLongText ( TRUE );
	m_pTitleTip->m_nDelayConst = 0;
	m_pTitleTip->SetAutoHide(false);
	m_pTitleTip->SetAbsolutePositionFlag(false);
	m_pTitleTip->Create ( this );

	if (!m_ImageList.m_hImageList)
	{
		VERIFY(m_ImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 0, 1));
		m_nLockedIconIndex = m_ImageList.Add(LoadIconLR(IDI_COMMON_CTRL_LOCK, 16));
	}
	

	
/*	// gets the initial size as the min track size
	CRect rc;
	GetWindowRect(&rc);
*/
	m_bInitDone = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CResizablePage::ComputeLayoutEx ()
{
    if  ( m_bInitDone )
    {
//        ArrangeLayout ();
        
        if  ( m_pLayoutRule )
        {
            Layout_ComputeLayout( GetSafeHwnd (), m_pLayoutRule );
        }
        
    }
}

void CResizablePage::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (m_bInitDone)
	{
//		ArrangeLayout();
		
		if ( m_pLayoutRule )
		{
			Layout_ComputeLayout( GetSafeHwnd(), m_pLayoutRule );
		}
		
	}
	
}

void CResizablePage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HWND hPopupWnd = GetSafeHwnd ();
	while ( hPopupWnd && !( ::GetWindowLong  ( hPopupWnd, GWL_STYLE ) & WS_POPUP ) )
	{
		hPopupWnd = ::GetParent  (hPopupWnd);
	}
	
	if  (hPopupWnd) ::PostMessage ( hPopupWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELONG(point.x, point.y) );
				
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CResizablePage::SetCtrlTooltip(HWND hWndCtrl, LPCTSTR szTooltip)
{
	if (::IsWindow(hWndCtrl) && m_pTooltipMap!=NULL)
	{
		if (szTooltip == NULL)
			m_pTooltipMap->m_TooltipMap.erase(hWndCtrl);
		else
			m_pTooltipMap->m_TooltipMap[hWndCtrl] = szTooltip;
	}
}

void CResizablePage::SetCtrlTooltip(int nID, LPCTSTR szTooltip)
{
	SetCtrlTooltip (::GetDlgItem ( GetSafeHwnd(), nID ), szTooltip);
}

BOOL CResizablePage::PreTranslateMessage(MSG* pMsg) 
{
    if  ( ::IsWindow ( m_pTitleTip->GetSafeHwnd () ) )
    {
        m_pTitleTip->PreTranslateMessage ( pMsg );
    }
	
	if (pMsg->message == WM_MOUSEMOVE && m_pTooltipMap!=NULL)
	{
		CPoint point ( GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) );
		::ClientToScreen (pMsg->hwnd, &point);
		ScreenToClient(&point);

	    HWND hWndChild = ::ChildWindowFromPoint ( GetSafeHwnd (), point );
    
		if  ( ::IsWindow(hWndChild) && m_ptMouseClientPos != point)
		{
			HWND2STRING_MAP::iterator i = m_pTooltipMap->m_TooltipMap.find(hWndChild);
			if ( i != m_pTooltipMap->m_TooltipMap.end() )
			{
				if (TOOLTIP_TIMER_ID)
					KillTimer(TOOLTIP_TIMER_ID);

				TOOLTIP_TIMER_ID = SetTimer (17, GetDoubleClickTime (), NULL);
				TRACE("SetTimer(TOOLTIP_TIMER_ID)\n");
				
				m_ptMouseClientPos = point;
				m_hwndChild = hWndChild;
				m_szTooltipNext = i->second;

			}
		}
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CResizablePage::OnTimer(UINT nIDEvent) 
{
	if 	(nIDEvent == TOOLTIP_TIMER_ID)
	{
		TRACE("OnTimer(TOOLTIP_TIMER_ID)\n");
		KillTimer(TOOLTIP_TIMER_ID);
		TOOLTIP_TIMER_ID = 0;
		
		CPoint ptCur;
		VERIFY(::GetCursorPos(&ptCur));
		ScreenToClient(&ptCur);

		if (abs(ptCur.x - m_ptMouseClientPos.x) < GetSystemMetrics(SM_CXDOUBLECLK) &&
			abs(ptCur.y - m_ptMouseClientPos.y) < GetSystemMetrics(SM_CYDOUBLECLK) &&
			::ChildWindowFromPoint ( GetSafeHwnd (), ptCur ) == m_hwndChild)
		{
			CRect rect;
			::GetWindowRect (m_hwndChild, rect);
			ScreenToClient ( &rect );
			
			m_pTitleTip->Show ( rect, m_szTooltipNext.c_str(), true );				
		}
	}

	CPropertyPage::OnTimer(nIDEvent);
}

/*
void CResizablePage::SetCtrlLockedState(int nID, bool bAlignTop, bool bLocked)
{
	SetCtrlLockedState(::GetDlgItem ( GetSafeHwnd(), nID ), bAlignTop, bLocked);
}

void CResizablePage::SetCtrlLockedState(HWND hWnd, bool bAlignTop, bool bLocked)
{
	if (::IsWindow(hWnd))
	{
		if (bLocked)
			m_LockCtrlsSet[hWnd] = bAlignTop;
		else
			m_LockCtrlsSet.erase(hWnd);
	}

}

void CResizablePage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	for( HWND2RECT_SET::iterator i = m_LockCtrlsSet.begin(); i!=m_LockCtrlsSet.end(); ++i)
	{
		if (::IsWindow(i->first))
		{
			CRect rcCtrl;
			::GetWindowRect(i->first, &rcCtrl);
			ScreenToClient(&rcCtrl);
			
			CPoint point(rcCtrl.left - 19, i->second?(rcCtrl.top-1):(rcCtrl.top + (rcCtrl.Height() -16)/2));
			
			m_ImageList.DrawIndirect (&dc, m_nLockedIconIndex, point, CSize(16, 16), CPoint (0, 0), ILD_NORMAL, SRCCOPY, CLR_NONE, 0);
		}
	}

	// Do not call CPropertyPage::OnPaint() for painting messages
}

*/
