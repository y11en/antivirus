// ToolBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "NewToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar

CDlgToolBar::CDlgToolBar() : m_init(false)
{
}

CDlgToolBar::~CDlgToolBar()
{
}

BEGIN_MESSAGE_MAP(CDlgToolBar, CToolBar)
	//{{AFX_MSG_MAP(CDlgToolBar)
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar message handlers

UINT CDlgToolBar::OnNcHitTest(CPoint point) 
{
	if (!m_init)
	{
		int n;
		RECT rect;
		CString tiptext;
		CString sFullText;
		m_TipCtrl.Create(this);
		for (int i = 0; i < GetToolBarCtrl().GetButtonCount(); i++)
		{
			GetItemRect(i, &rect);
			sFullText = LoadStr(GetItemID(i));
			n = sFullText.Find('\n');
			if (n != -1)
			{
				tiptext = sFullText.Mid(n + 1);
				n = tiptext.Find('\n');
				if (n != -1) tiptext.SetAt(n, 0);
			}
			
			m_TipCtrl.AddTool(this, tiptext, &rect, i+1);
		}
		m_TipCtrl.SetDelayTime(500);
		m_TipCtrl.Activate(true);
		m_init = true;
	}

	return CToolBar::OnNcHitTest(point);
}

void CDlgToolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = WM_MOUSEMOVE;
	msg.wParam = (WPARAM)nFlags;
	msg.lParam = MAKELPARAM(LOWORD(point.x), LOWORD(point.y));
	msg.time = 0;
	msg.pt.x = LOWORD(msg.lParam);
	msg.pt.y = HIWORD(msg.lParam);
	if (m_TipCtrl.m_hWnd != NULL)
		m_TipCtrl.RelayEvent(&msg);
	
	CToolBar::OnMouseMove(nFlags, point);
}
