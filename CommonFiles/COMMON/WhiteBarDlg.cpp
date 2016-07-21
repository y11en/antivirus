// WhiteBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "WhiteBarDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCWhiteBarDlg dialog

IMPLEMENT_DYNAMIC(CCWhiteBarDlg, CCDialog)

CCWhiteBarDlg::CCWhiteBarDlg(UINT nIDTemplate, UINT nHelpId, CWnd * pParent/* = NULL*/, _dword Flags, LPCTSTR pszSectionName /*= NULL*/, CSize InitialSize /*= NULL*/)
	: CCDialog(nIDTemplate, pParent, Flags, pszSectionName, InitialSize),
		m_pUserLayoutRule (NULL),
		m_nHelpId (nHelpId)
{
	//{{AFX_DATA_INIT(CCWhiteBarDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	RULE end_rule = { lEND };
	m_LayoutRule.push_back  (end_rule);

	if (m_nHelpId == WHITEBAR_NO_HELP_ID)
		m_Flags |= DIALOG_FLAGS_DISABLE_HELP;
}


void CCWhiteBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CCDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCWhiteBarDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCWhiteBarDlg, CCDialog)
    //{{AFX_MSG_MAP(CCWhiteBarDlg)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP

    ON_NOTIFY( EN_LINK, IDC_HELP_LINK_WHITEBAR, OnHelpPress )
	HTMLHELP_MSGS ()

END_MESSAGE_MAP()

//DEFINE_EMPTY_DEF_HELPCONTEXT (CCWhiteBarDlg)
DEFINE_DEF_HELPCONTEXT (CCWhiteBarDlg, m_nHelpId)
IMPL_USE_WND_HTMLHELP (CCWhiteBarDlg)

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

BOOL CCWhiteBarDlg::OnInitDialog ()
{
    OnSetMetricsDLU();
    
	CCDialog::OnInitDialog();
	
	GetButtonReverseList(m_btnList);
	
	SetWhiteBar();
	
	SetStdButtons();

	SetLayout ( NULL );

	Layout_ComputeLayout ( GetSafeHwnd (), GetLayoutRules() );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCWhiteBarDlg::ExternalInit ()
{
    OnSetMetricsDLU();

	GetButtonReverseList(m_btnList);
	
	SetWhiteBar();
	
	SetStdButtons();

	SetLayout ( NULL );

	Layout_ComputeLayout ( GetSafeHwnd (), GetLayoutRules() );	
}

void CCWhiteBarDlg::SetWhiteBar()
{
	CRect rcDlg;
	GetWindowRect(rcDlg);
	rcDlg.bottom+=GetWhiteBar_Height();
	MoveWindow(rcDlg);
	
	CRect rect ( MinRect () );
	rect.bottom += GetWhiteBar_Height ();
	MinRect ( rect );
	
	VERIFY(m_wndWhiteBar.Create(_T(""), WS_CHILD|WS_VISIBLE|SS_WHITERECT, CRect(0,0,0,0), this, IDC_WHITEBAR));
	m_wndWhiteBar.SetWindowPos(this, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
	
	if (m_Flags & DIALOG_FLAGS_DISABLE_HELP)
		return;
	
	VERIFY ( m_wndHelpIcon.Create ( NULL, WS_VISIBLE | WS_CHILD | SS_ICON, CRect(0,0, 24, 24), this, IDC_HELP_ICON_WHITEBAR ) );
	
	m_wndHelpIcon.SetIcon  ( ::LoadIconLR ( IDI_KAP_HELP, 24) );
	m_wndHelpIcon.SetWindowPos(&m_wndWhiteBar, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);
	
	CString szText;
	szText.LoadString  (IDS_COM_HELP);
	CRect rcLink(0,0,0,0);
	
	VERIFY ( m_wndHelpLink.Create  (  (LPCTSTR) szText, WS_VISIBLE | WS_CHILD | WS_TABSTOP | SS_LEFTNOWORDWRAP, rcLink, this, IDC_HELP_LINK_WHITEBAR ) );
	
	m_wndHelpLink.GetNeedSize (rcLink);
	m_wndHelpLink.SetWindowPos(&m_wndWhiteBar, 0,0,rcLink.Width  (), rcLink.Height  (), SWP_NOMOVE);
	
}

void CCWhiteBarDlg::GetButtonReverseList( std::vector<HWND> & btnList )
{
	if (m_Flags & DIALOG_FLAGS_MOVE_STD_BTNS)
	{
		InsertIfExist(btnList, IDCLOSE);
		InsertIfExist(btnList, IDNO);
		InsertIfExist(btnList, IDYES);
		InsertIfExist(btnList, IDIGNORE);
		InsertIfExist(btnList, IDRETRY);
		InsertIfExist(btnList, IDABORT);
		InsertIfExist(btnList, IDCANCEL);
		InsertIfExist(btnList, IDOK);
	}
	else
	{
		for (std::vector<DWORD>::reverse_iterator i = m_btnCustomList.rbegin  (); i!=m_btnCustomList.rend  (); ++i)
			InsertIfExist(btnList, *i);
	}
}
/*
static HWND GetDlgItemEx (HWND hWnd, UINT nID)
{
	TCHAR szBuf[7]; // "BUTTON" + NUL character

	for (HWND hWndChild = ::GetTopWindow (hWnd); hWndChild != NULL;
			hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		if (GetDlgCtrlID (hWndChild) == nID)
		{
			GetClassName (hWndChild, szBuf, sizeof(szBuf) / sizeof(TCHAR));
			if (lstrcmpi (szBuf, _T("Button")) == 0)
				break;
		}
	}

	return hWndChild;
}
*/
void CCWhiteBarDlg::InsertIfExist(std::vector<HWND> & btnList, DWORD dwIDC)
{
//	CWnd* pStdButton = CWnd::FromHandle (GetDlgItemEx (GetSafeHwnd (), dwIDC));
	HWND hwndStdButton = ::GetDlgItem (GetSafeHwnd (), dwIDC);

	if (hwndStdButton && ::IsWindow(hwndStdButton))
	{
		btnList.push_back  (hwndStdButton);

		CWnd::FromHandle (hwndStdButton)->ModifyStyle  (0, BS_FLAT);
		
/*		CString szText;
		pStdButton->GetWindowText  (szText);
		
		if (szText==_T(""))
		{
			pStdButton->SetWindowText  (_T("Tets"));
		}
*/
	}
	
	
}

void CCWhiteBarDlg::OnSetMetricsDLU()
{
	SetMetricsDLU  (30, 7, 50, 14, 7);
}

void CCWhiteBarDlg::SetMetricsDLU(UINT wbh, UINT rmw, UINT sbw, UINT sbh, UINT sbmw)
{
	ASSERT(::IsWindow  (GetSafeHwnd  ()));

	CRect rcMap(0, 0, rmw, wbh);
	MapDialogRect  (rcMap);
	
	nWHITEBAR_HEIGHT = rcMap.Height  ();
	nRIGHT_MARGIN = rcMap.Width  ();

	rcMap.SetRect  (0, 0, sbw, sbh);
	MapDialogRect  (rcMap);

	nSTANDARD_BTN_HIGHT = rcMap.Height  ();
	nSTANDARD_BTN_WIDTH = rcMap.Width  ();

	rcMap.SetRect  (0, 0, sbmw, sbh);
	MapDialogRect  (rcMap);
	
	
	nSTANDARD_BTN_MARGIN = rcMap.Width  ();
	
}

UINT CCWhiteBarDlg::GetWhiteBar_Height()
{
	return nWHITEBAR_HEIGHT;
}
UINT CCWhiteBarDlg::GetRightMargin_Width()
{
	return nRIGHT_MARGIN;
}
UINT CCWhiteBarDlg::GetStdBtn_Width()
{
	return nSTANDARD_BTN_WIDTH;
}
UINT CCWhiteBarDlg::GetStdBtn_Hight()
{
	return nSTANDARD_BTN_HIGHT;
}
UINT CCWhiteBarDlg::GetStdBtnMargin_Width()
{
	return nSTANDARD_BTN_MARGIN;
}

void CCWhiteBarDlg::OnSize(UINT nType, int cx, int cy) 
{
	CCDialog::OnSize(nType, cx, cy);
	
	if ( ::IsWindow(m_wndWhiteBar.GetSafeHwnd  ()) )
	{
		Layout_ComputeLayout ( GetSafeHwnd (), GetLayoutRules() );
	}
}

RULE* CCWhiteBarDlg::GetLayoutRules()
{
	if ( m_pUserLayoutRule && ::IsWindow  (GetSafeHwnd  ()) )
	{
		SetLayout  (m_pUserLayoutRule);
		m_pUserLayoutRule = NULL;
	}

	return &m_LayoutRule.front();
	
}

void CCWhiteBarDlg::SetLayout ( RULE* pLayoutRule)
{

	if (!::IsWindow  (GetSafeHwnd  ()))
	{
		m_pUserLayoutRule = pLayoutRule;
		return;
	}

	m_LayoutRule.clear  ();
	
	if (pLayoutRule)
	{
		for (int i = 0; pLayoutRule[i].Action != lEND; ++i)
			m_LayoutRule.push_back  (pLayoutRule[i]);
	}
	
	{
		RULE rule1 = { lSTRETCH, lLEFT(IDC_WHITEBAR), lLEFT(lPARENT), 0 };
		RULE rule2 = { lSTRETCH, lRIGHT(IDC_WHITEBAR), lRIGHT(lPARENT), 0 };
		RULE rule3 = { lSTRETCH, lTOP(IDC_WHITEBAR), lBOTTOM(lPARENT), -30 };
		RULE rule4 = { lSTRETCH, lBOTTOM(IDC_WHITEBAR), lBOTTOM(lPARENT), 0 };
		
		m_LayoutRule.push_back  (rule1);
		m_LayoutRule.push_back  (rule2);
		m_LayoutRule.push_back  (rule3);
		m_LayoutRule.push_back  (rule4);
		
		if ( !(m_Flags & DIALOG_FLAGS_DISABLE_HELP) )
		{
			RULE rule5 = { lMOVE, lLEFT(IDC_HELP_ICON_WHITEBAR), lLEFT(lPARENT), GetRightMargin_Width () };
			RULE rule6 = { lMOVE, lBOTTOM(IDC_HELP_ICON_WHITEBAR), lBOTTOM(lPARENT), -7 };
			
			RULE rule7 = { lMOVE, lLEFT(IDC_HELP_LINK_WHITEBAR), lRIGHT(IDC_HELP_ICON_WHITEBAR), 4 };
			RULE rule8 = { lMOVE, lBOTTOM(IDC_HELP_LINK_WHITEBAR), lBOTTOM(lPARENT), -9 };
			
			m_LayoutRule.push_back  (rule5);
			m_LayoutRule.push_back  (rule6);
			m_LayoutRule.push_back  (rule7);
			m_LayoutRule.push_back  (rule8);
		}
	}
	
	
	std::vector<HWND>::iterator i = m_btnList.begin  ();

	if (i != m_btnList.end ())
	{
		int dwIDRight = lPARENT;
		int nID = ::GetWindowLong( *i, GWL_ID );
		RULE rule1 = {	lMOVE, lRIGHT(nID), lRIGHT(dwIDRight), -7};
		RULE rule2 = {	lMOVE, lBOTTOM(nID), lBOTTOM(lPARENT), -7};
		dwIDRight = nID;
		m_LayoutRule.push_back  (rule1);
		m_LayoutRule.push_back  (rule2);
		
		++i;
		
		for ( ; i != m_btnList.end  (); ++i )
		{
			nID = ::GetWindowLong( *i, GWL_ID );
			
			RULE rule1 = {	lMOVE, lRIGHT(nID), lLEFT(dwIDRight), -7};
			RULE rule2 = {	lMOVE, lBOTTOM(nID), lBOTTOM(lPARENT), -7};
			
			m_LayoutRule.push_back  (rule1);
			m_LayoutRule.push_back  (rule2);
			
			dwIDRight = nID;
		}
	}
	
	
	RULE end_rule = { lEND };
	m_LayoutRule.push_back  (end_rule);

}

void CCWhiteBarDlg::SetStdButtons()
{
	if (m_Flags & DIALOG_FLAGS_MOVE_STD_BTNS)
	{
		for ( std::vector<HWND>::iterator i = m_btnList.begin  (); i != m_btnList.end  (); ++i )
		{
			if (::IsWindow (*i))
				::SetWindowPos  (*i, m_wndWhiteBar.GetSafeHwnd (), 0, 0, GetStdBtn_Width (), GetStdBtn_Hight  (), SWP_NOMOVE);
		}
	}
	else
	{
		for ( std::vector<HWND>::iterator i = m_btnList.begin  (); i != m_btnList.end  (); ++i )
		{
			if (::IsWindow (*i))
				::SetWindowPos  (*i, m_wndWhiteBar.GetSafeHwnd (), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

}

void CCWhiteBarDlg::OnHelpPress (NMHDR * pNotifyHeader, LRESULT * pResult )
{
	*pResult = 0;
	OnHelp  ();
}

void CCWhiteBarDlg::OnHelp()
{
	if (m_nHelpId != WHITEBAR_NO_HELP_ID)
		WinHelp (m_nHelpId, HELP_CONTEXT, true);
}

void CCWhiteBarDlg::AddCustomButton(DWORD dwID)
{
	ASSERT((m_Flags & DIALOG_FLAGS_MOVE_STD_BTNS)==0);
	m_btnCustomList.push_back  (dwID);
}