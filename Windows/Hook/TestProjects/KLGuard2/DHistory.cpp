// DHistory.cpp : implementation file
//

#include "stdafx.h"
#include "KLGuard2.h"
#include "DHistory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDHistory dialog


CDHistory::CDHistory(ULONG ProcessId, CString* pCaption, CString* pImagePath, _StrHistory* pReg, _StrHistory* pFile, _StrHistory* pFileSelf, 
					 CWnd* pParent /*=NULL*/)
	: CDialog(CDHistory::IDD, pParent)
{
	m_ProcessId = ProcessId;
	m_pImagePath = pImagePath;
	m_FileInfo.QueryInfo(pImagePath->GetBuffer(pImagePath->GetLength() + sizeof(TCHAR)));
	pImagePath->ReleaseBuffer();

	//{{AFX_DATA_INIT(CDHistory)
	m_eInfo = _T("");
	//}}AFX_DATA_INIT
	m_pCaption = pCaption;
	m_pReg = pReg;
	m_pFile = pFile;
	m_pFileSelf = pFileSelf;
}


void CDHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDHistory)
	DDX_Control(pDX, IDC_STATIC_ICON, m_stIcon);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_lHistory);
	DDX_Text(pDX, IDC_EDIT_INFO, m_eInfo);
	DDV_MaxChars(pDX, m_eInfo, 4096);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDHistory, CDialog)
	//{{AFX_MSG_MAP(CDHistory)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDHistory message handlers

BOOL CDHistory::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(*m_pCaption);

	m_stIcon.SetIcon(m_FileInfo.GetIcon(true));

	CString info = m_FileInfo.GetFileDescription();
	if (info.IsEmpty())
		info = _T("File description: <not available>");
	
	CString company = m_FileInfo.GetCompanyInfo();
	if (company.IsEmpty())
		company = _T("Company info: <not available>");
	
	CString version = m_FileInfo.GetProductVersion();
	if (version.IsEmpty())
		version = _T("Product version: <not available>");

	m_eInfo = info + _T("\r\n");
	m_eInfo += company + _T("\r\n");
	m_eInfo += version;

	CString strtmp;
	_StrHistory::iterator _it;

	for (_it = m_pReg->begin(); _it != m_pReg->end(); ++_it)
	{
		strtmp = *_it;
		m_lHistory.AddString(strtmp);
	}

	for (_it = m_pFile->begin(); _it != m_pFile->end(); ++_it)
	{
		strtmp = *_it;
		m_lHistory.AddString(strtmp);
	}

	for (_it = m_pFileSelf->begin(); _it != m_pFileSelf->end(); ++_it)
	{
		strtmp = *_it;
		m_lHistory.AddString(strtmp);
	}

	UpdateData(FALSE);

	RestorePosition();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDHistory::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_lHistory.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lMOVE,		lRIGHT(IDOK),				lRIGHT(lPARENT)				, -3},
			{lMOVE,		lBOTTOM(IDOK),				lBOTTOM(lPARENT)			, -3},
			{lMOVE,		lBOTTOM(IDC_BUTTON_CLEAR),	lBOTTOM(lPARENT)			, -3},
			{lSTRETCH,	lRIGHT(IDC_LIST_HISTORY),	lRIGHT(lPARENT)				, -3},
			{lSTRETCH,	lBOTTOM(IDC_LIST_HISTORY),	lTOP(IDOK)					, -5},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}
}

void CDHistory::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	gpProcList->DeleteHistoryFor(m_ProcessId, m_pImagePath);
	OnOK();
}

void CDHistory::RestorePosition()
{
	WINDOWPLACEMENT wpl;
	UINT dw = 0;

	if (g_pReestr->GetBinaryValue(_T("HistoryPosition"), (DWORD) &wpl, sizeof(wpl)))
		SetWindowPlacement(&wpl);
}

void CDHistory::SavePosition()
{
	WINDOWPLACEMENT wpl;
	GetWindowPlacement(&wpl);

	g_pReestr->SetBinaryValue(_T("HistoryPosition"), (DWORD)&wpl, sizeof(wpl));
}

BOOL CDHistory::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	SavePosition();

	return CDialog::DestroyWindow();
}

void CDHistory::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI)
{
	CWnd::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = 320;
	lpMMI->ptMinTrackSize.y = 200;
}

