// DFolder.cpp : implementation file
//

#include "stdafx.h"
#include "allowedrun.h"
#include "DFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFolder dialog


CDFolder::CDFolder(CString* pFolder, CWnd* pParent /*=NULL*/)
	: CDialog(CDFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFolder)
	m_eFolder = _T("");
	//}}AFX_DATA_INIT
	m_pFolder = pFolder;
}


void CDFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFolder)
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_eFolder);
	DDV_MaxChars(pDX, m_eFolder, 4096);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFolder, CDialog)
	//{{AFX_MSG_MAP(CDFolder)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFolder message handlers
BOOL CDFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_eFolder = *m_pFolder;
	UpdateData(FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDFolder::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CString szFolder;
	CString title = "Select folder for deny";
	if (BrowseForFolder(this->m_hWnd, title, szFolder))
	{
		m_eFolder = szFolder;
		UpdateData(FALSE);

		GetDlgItem(IDC_EDIT_FOLDER)->SetFocus();
	}
}

void CDFolder::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if (m_eFolder.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_FOLDER)->SetFocus();
		return;
	}

	*m_pFolder = m_eFolder;
	
	CDialog::OnOK();
}
