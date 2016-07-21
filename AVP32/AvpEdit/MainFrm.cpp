// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AvpEdit.h"

#include "MainFrm.h"

#include <Bases/Format/Base.h>
#include "REdit.h"
#include "OptDlg.h"

#include <ScanAPI/scanobj.h>
#include <ScanAPI/avp_dll.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
extern COptDlg OptDlg;
extern CRect WndPos;
extern CRect ScanWndPos;

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_RELOADBASES, OnFileReloadbases)
	ON_COMMAND(ID_FILE_SCANDIRECTORY, OnFileScandirectory)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
//	InitDB(malloc, free);
//		ClearDatabase();
	
}

CPtrArray Clipboard;

CMainFrame::~CMainFrame()
{
	int i;
	while(i=Clipboard.GetSize()){
		i--;
		CRecordEdit* ptr=(CRecordEdit*)Clipboard[i];
		if(ptr!=NULL)delete ptr; 
		Clipboard.RemoveAt(i);
	}
//	DoneDB();
	
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
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

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	LoadBarState("Options");

	ScanDialog.Create(IDD_SCANDIALOG,this);
	
	PostMessage(WM_COMMAND,ID_FILE_RELOADBASES,0);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnFileReloadbases() 
{
	char Name[512];
	char* p=Name;

	if(0!=_access(OptDlg.m_SetFile,0))
		GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,Name,&p);

	*p=0;
	char CurDir[512];
	GetCurrentDirectory(512,CurDir);
	SetCurrentDirectory(Name);

	strcpy(p,OptDlg.m_SetFile);
	
	if(AVP_LoadBase(Name,0))
		SetMessageText("All bases was loaded successfully.");
	else
		SetMessageText("Loding of bases is not coplete.");

	SetCurrentDirectory(CurDir);
}

void CMainFrame::OnFileScandirectory() 
{
	ScanDialog.SetOwner(this);
	ScanDialog.ShowWindow(SW_RESTORE);
	ScanDialog.SetActiveWindow();
	ScanDialog.SetFocus();
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if(ScanDialog.ScanThreadPtr){
		AVP_CancelProcessObject(1);
		ScanDialog.PostMessage(WM_CLOSE);
		PostMessage(WM_CLOSE);
		return;
	}

	ScanDialog.UpdateData();
	ScanDialog.GetWindowRect(&ScanWndPos);
	GetWindowRect(&WndPos);
	SaveBarState("Options");
	

	CMDIFrameWnd::OnClose();
}


void CMainFrame::OnViewOptions() 
{
	if(IDOK==OptDlg.DoModal())
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

