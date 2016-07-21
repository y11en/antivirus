//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "RegEditView.h"
#include "PersistentData.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CMainFrame* GlobalGetMainWnd()
{
	return (CMainFrame*) AfxGetMainWnd();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CMainFrame::CMainFrame()
{
	m_szOnePane = CSize(200, 100);
	m_szTwoPane = CSize(200, 100);
}

CMainFrame::~CMainFrame()
{
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam == VK_TAB	&&
		(	pMsg->hwnd==*m_wndSplitter.GetPane(0,0)	||
			pMsg->hwnd==*m_wndSplitter.GetPane(0,1)))	
	{
		CLeftView&		rLeftView	= *STATIC_DOWNCAST(CLeftView, m_wndSplitter.GetPane(0,0));
		CRegEditView&	rTblView	= *STATIC_DOWNCAST(CRegEditView, m_wndSplitter.GetPane(0,1));

		if(pMsg->hwnd==rLeftView)
			rTblView.SetFocus();
		else
			rLeftView.SetFocus();

		return TRUE;
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// Save/Restore Placement Helpers
// (Д.Дж.Круглински, "Основы VISUAL C++", глава 14, с.248-250)

static const TCHAR s_sSettings[]=_T("Common Settings");
static const TCHAR s_sFormat[]=_T("%d,%d,%d,%d; %d,%d");

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// запомнить состояние окна и его панелей в INI-файле или реестре
//
void CMainFrame::SavePlacement() 
{
	WINDOWPLACEMENT wp;
	wp.length=sizeof(wp);
	GetWindowPlacement (&wp);
	
	BOOL bIconic=FALSE,bMaximized=FALSE;
	switch (wp.showCmd) 
	{
    case SW_SHOWNORMAL:
		break;
    case SW_SHOWMAXIMIZED:
		bMaximized=TRUE;
		break;
    case SW_SHOWMINIMIZED:
		bIconic=TRUE;
		if (wp.flags) bMaximized=TRUE;
		break;
	}
	
	CString sBuffer;
	sBuffer.Format (s_sFormat,wp.rcNormalPosition.left,wp.rcNormalPosition.top,
		wp.rcNormalPosition.right,wp.rcNormalPosition.bottom,
		bIconic,bMaximized);	
	

	GlobalGetApp()->SetPersistentData(DATA_CODE_MAIN_FRAME, sBuffer);
	SaveBarState (s_sSettings);

	sBuffer = GetSplitterInfo();
	GlobalGetApp()->SetPersistentData(DATA_CODE_SPLITTER, sBuffer);

	sBuffer = STATIC_DOWNCAST(CRegEditView, m_wndSplitter.GetPane(0,1))->GetColumnsInfo();
	GlobalGetApp()->SetPersistentData(DATA_CODE_COLUMNS, sBuffer);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// восстановить состояние окна и его панелей из INI-файла или реестра; вызывается из
// CEasyTraceApp::InitInstance
//
void CMainFrame::RestorePlacement (int& cmdShow) 
{
	LoadBarState (s_sSettings);
	
	CString sBuffer = GlobalGetApp()->GetPersistentData(DATA_CODE_MAIN_FRAME);
	if (sBuffer.IsEmpty()) return;
	
	WINDOWPLACEMENT wp;
	GetWindowPlacement (&wp);
	
	BOOL bIconic,bMaximized;
	_stscanf (sBuffer,s_sFormat,&wp.rcNormalPosition.left,&wp.rcNormalPosition.top,
		&wp.rcNormalPosition.right,&wp.rcNormalPosition.bottom,
		&bIconic,&bMaximized);
	
	if (bIconic) {
		wp.showCmd=SW_SHOWMINNOACTIVE;
		if (bMaximized) wp.flags=WPF_RESTORETOMAXIMIZED;
		else				 wp.flags=WPF_SETMINPOSITION;
	}
	else {
		if (bMaximized) {
			wp.showCmd=SW_SHOWMAXIMIZED;
			wp.flags=WPF_RESTORETOMAXIMIZED;
		}
		else {
			wp.showCmd=SW_SHOWNORMAL;
			wp.flags=WPF_SETMINPOSITION;
		}
	}
	
	SetWindowPlacement (&wp);
	cmdShow=wp.showCmd;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
static const CString sFormat = _T("%d,%d;%d,%d");

CString CMainFrame::GetSplitterInfo()				
{ 
	int tmp;

	m_wndSplitter.GetColumnInfo	(0, (int&)m_szOnePane.cx, tmp);
	m_wndSplitter.GetRowInfo		(0, (int&)m_szOnePane.cy, tmp);
	m_wndSplitter.GetColumnInfo	(1, (int&)m_szTwoPane.cx, tmp);
	m_wndSplitter.GetRowInfo		(0, (int&)m_szTwoPane.cy, tmp);

	CString ret;
	ret.Format(sFormat,
	m_szOnePane.cx, m_szOnePane.cy, m_szTwoPane.cx, m_szTwoPane.cy);
	return ret;
} 

void CMainFrame::RestoreSplitterInfo(CString str)
{ 
	_stscanf (str,sFormat,
		&m_szOnePane.cx, &m_szOnePane.cy, &m_szTwoPane.cx, &m_szTwoPane.cy);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CMainFrame::SetStatusIdleMsg(LPCTSTR i_strMsg)
{
	m_strStatusIdleMsg = i_strMsg;
	PostMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	CString strSplitter = 
		GlobalGetApp()->GetPersistentData(DATA_CODE_SPLITTER);

	if(!strSplitter.IsEmpty())
		RestoreSplitterInfo(strSplitter);

	if ( !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), m_szOnePane, pContext) ||
		 !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CRegEditView), m_szTwoPane, pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	if(nID == AFX_IDS_IDLEMESSAGE)
		rMessage = m_strStatusIdleMsg;
	else
		CFrameWnd::GetMessageString(nID, rMessage);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CMainFrame message handlers

CRegEditView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CRegEditView* pView = DYNAMIC_DOWNCAST(CRegEditView, pWnd);
	return pView;
}



// ---
void CMainFrame::OnNewKey() {
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CLeftView* pView = DYNAMIC_DOWNCAST(CLeftView, pWnd);
	if ( pView )
		pView->OnNewKey();
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{

	CRegEditView* pView = GetRightPane(); 


	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CMainFrame::OnViewStyle(UINT nCommandID)
{
	CRegEditView* pView = GetRightPane();

	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CMainFrame::OnClose() 
{
	SavePlacement();
	CFrameWnd::OnClose();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
