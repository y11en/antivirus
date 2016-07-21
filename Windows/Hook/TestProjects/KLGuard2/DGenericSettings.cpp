// DGenericSettings.cpp : implementation file
//

#include "stdafx.h"
#include "klguard2.h"
#include "DGenericSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDGenericSettings dialog


CDGenericSettings::CDGenericSettings(CGenericSettings* pGeneralSettings, CWnd* pParent /*=NULL*/)
	: CDialog(CDGenericSettings::IDD, pParent)
{
	m_pGeneralSettings = pGeneralSettings;
	//{{AFX_DATA_INIT(CDGenericSettings)
	m_chProtectAutoRun = pGeneralSettings->m_bProtectAutoRun;
	m_chProtectSelf = pGeneralSettings->m_bProtectSelf;
	m_chDontCotrnolWriteBySystem = pGeneralSettings->m_bDontControlWriteBySystemProcess;
	m_eLogFileName = pGeneralSettings->m_LogFileName;
	//}}AFX_DATA_INIT
}

void CDGenericSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDGenericSettings)
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_btApply);
	DDX_Check(pDX, IDC_CHECK_PROTECT_AUTORUN, m_chProtectAutoRun);
	DDX_Check(pDX, IDC_CHECK_PROTECTSELF, m_chProtectSelf);
	DDX_Text(pDX, IDC_EDIT_LOGFILENAME, m_eLogFileName);
	DDV_MaxChars(pDX, m_eLogFileName, 4096);
	DDX_Check(pDX, IDC_CHECK_DONTCONTROLWRITE_BYSYSTEM, m_chDontCotrnolWriteBySystem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDGenericSettings, CDialog)
	//{{AFX_MSG_MAP(CDGenericSettings)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_CHECK_PROTECT_AUTORUN, OnCheckProtectAutorun)
	ON_BN_CLICKED(IDC_CHECK_PROTECTSELF, OnCheckProtectself)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_DONTCONTROLWRITE_BYSYSTEM, OnCheckDontcontrolwriteBysystem)
	ON_EN_CHANGE(IDC_EDIT_LOGFILENAME, OnChangeEditLogfilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDGenericSettings message handlers

BOOL CDGenericSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_btApply.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDGenericSettings::OnOK() 
{
	// TODO: Add extra validation here
	if (m_btApply.IsWindowEnabled())
		OnButtonApply();
	
	CDialog::OnOK();
}

void CDGenericSettings::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CString title = _T("Select log file");
	CString strpath;
	if (BrowseForFolder(this->m_hWnd, title, strpath, TRUE, NULL))
	{
		if (m_eLogFileName != strpath)
		{
			m_eLogFileName = strpath;
			UpdateData(FALSE);
			m_btApply.EnableWindow(TRUE);
		}
	}	
}

void CDGenericSettings::OnCheckProtectAutorun() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}


void CDGenericSettings::OnCheckDontcontrolwriteBysystem() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}


void CDGenericSettings::OnCheckProtectself() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);	
}

void CDGenericSettings::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_pGeneralSettings->m_bProtectAutoRun = !!m_chProtectAutoRun;
	m_pGeneralSettings->m_bProtectSelf = !!m_chProtectSelf;
	m_pGeneralSettings->m_bDontControlWriteBySystemProcess = !!m_chDontCotrnolWriteBySystem;
	m_pGeneralSettings->m_LogFileName = m_eLogFileName;

	m_pGeneralSettings->GS_SaveSettings();
	
	m_btApply.EnableWindow(FALSE);	
}

void CDGenericSettings::OnChangeEditLogfilename() 
{
	m_btApply.EnableWindow(TRUE);
}
