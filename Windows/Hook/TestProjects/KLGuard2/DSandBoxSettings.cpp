// DSandBoxSettings.cpp : implementation file
//

#include "stdafx.h"
#include "klguard2.h"
#include "DSandBoxSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSandBoxSettings dialog


CDSandBoxSettings::CDSandBoxSettings(CSandBoxSettings* pSettings, CWnd* pParent /*=NULL*/)
	: CDialog(CDSandBoxSettings::IDD, pParent)
{
	m_pSettings = pSettings;
	//{{AFX_DATA_INIT(CDSandBoxSettings)
	m_chAllowCreateProcess = pSettings->m_bAllowCreateProcess;
	m_chAllowWrite_SandBoxFolder = pSettings->m_bAllowWrite_SandBoxFolder;
	m_chAllowWrite_TempFolder = pSettings->m_bAllowWrite_TempFolder;
	m_chAllowWrite_Registry = pSettings->m_bAllowWrite_Registry;
	m_chAllowWrite_SystemFolder = pSettings->m_bAllowWrite_SystemFolder;
	m_chAllowWriteProgramFiles = pSettings->m_bAllowWrite_ProgramFiles;
	m_chLogEvent = pSettings->m_bLogEvent;
	m_chShowWarning = pSettings->m_bShowWarning;
	m_chTerminateProcess = pSettings->m_bTerminateProcess;
	m_eSandBoxPath = pSettings->GetSandBoxPath();
	//}}AFX_DATA_INIT
}

void CDSandBoxSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSandBoxSettings)
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_btApply);
	DDX_Check(pDX, IDC_CHECK_ALLOW_CREATE_PROCESS, m_chAllowCreateProcess);
	DDX_Check(pDX, IDC_CHECK_ALLOWWRITE_SANDBOXFOLDER, m_chAllowWrite_SandBoxFolder);
	DDX_Check(pDX, IDC_CHECK_ALLOWWRITE_TEMPFOLDER, m_chAllowWrite_TempFolder);
	DDX_Check(pDX, IDC_CHECK_DENYWRITE_REGISTRY, m_chAllowWrite_Registry);
	DDX_Check(pDX, IDC_CHECK_ALLOWRITE_SYSTEMFOLDER, m_chAllowWrite_SystemFolder);
	DDX_Check(pDX, IDC_CHECK_LOGEVENT, m_chLogEvent);
	DDX_Check(pDX, IDC_CHECK_SHOW_WARNING, m_chShowWarning);
	DDX_Check(pDX, IDC_CHECK_TERMINATE, m_chTerminateProcess);
	DDX_Text(pDX, IDC_EDIT_SANDBOX_PATH, m_eSandBoxPath);
	DDV_MaxChars(pDX, m_eSandBoxPath, 4096);
	DDX_Check(pDX, IDC_CHECK_ALLOWWRITE_PROGRAMFILES, m_chAllowWriteProgramFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSandBoxSettings, CDialog)
	//{{AFX_MSG_MAP(CDSandBoxSettings)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_ALLOWRITE_SYSTEMFOLDER, OnCheckAllowWriteSystemfolder)
	ON_BN_CLICKED(IDC_CHECK_ALLOW_CREATE_PROCESS, OnCheckAllowCreateProcess)
	ON_BN_CLICKED(IDC_CHECK_ALLOWWRITE_SANDBOXFOLDER, OnCheckAllowWriteSandBoxfolder)
	ON_BN_CLICKED(IDC_CHECK_ALLOWWRITE_TEMPFOLDER, OnCheckAllowWriteTempfolder)
	ON_BN_CLICKED(IDC_CHECK_DENYWRITE_REGISTRY, OnCheckDenywriteRegistry)
	ON_BN_CLICKED(IDC_CHECK_LOGEVENT, OnCheckLogevent)
	ON_BN_CLICKED(IDC_CHECK_SHOW_WARNING, OnCheckShowWarning)
	ON_BN_CLICKED(IDC_CHECK_TERMINATE, OnCheckTerminate)
	ON_BN_CLICKED(IDC_CHECK_ALLOWWRITE_PROGRAMFILES, OnCheckAllowwriteProgramfiles)
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSandBoxSettings message handlers

BOOL CDSandBoxSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	OnCheckAllowWriteSystemfolder();
	m_btApply.EnableWindow(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDSandBoxSettings::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CString title = _T("Select 'Sand box' folder");
	CString strpath;
	if (BrowseForFolder(this->m_hWnd, title, strpath, FALSE, NULL))
	{
		if (m_eSandBoxPath != strpath)
		{
			m_eSandBoxPath = strpath;
			UpdateData(FALSE);
			m_btApply.EnableWindow(TRUE);
		}
	}
}

void CDSandBoxSettings::OnCheckAllowWriteSystemfolder() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckAllowCreateProcess() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckAllowwriteProgramfiles() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);	
}

void CDSandBoxSettings::OnCheckAllowWriteSandBoxfolder() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckAllowWriteTempfolder() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckDenywriteRegistry() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckLogevent() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckShowWarning() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);
}

void CDSandBoxSettings::OnCheckTerminate() 
{
	// TODO: Add your control notification handler code here
	m_btApply.EnableWindow(TRUE);	
}

//+ ------------------------------------------------------------------------------------------
void CDSandBoxSettings::OnOK() 
{
	// TODO: Add extra validation here
	if (m_btApply.IsWindowEnabled())
		OnButtonApply();
	
	CDialog::OnOK();
}

void CDSandBoxSettings::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_pSettings->SetSandBoxPath(&m_eSandBoxPath);
	m_pSettings->m_bAllowCreateProcess = !!m_chAllowCreateProcess;

	m_pSettings->m_bAllowWrite_SystemFolder = !!m_chAllowWrite_SystemFolder;
	m_pSettings->m_bAllowWrite_ProgramFiles = !!m_chAllowWriteProgramFiles;
	m_pSettings->m_bAllowWrite_SandBoxFolder = !!m_chAllowWrite_SandBoxFolder;
	m_pSettings->m_bAllowWrite_TempFolder = !!m_chAllowWrite_TempFolder;

	m_pSettings->m_bAllowWrite_Registry = !!m_chAllowWrite_Registry;

	m_pSettings->m_bLogEvent = !!m_chLogEvent;
	m_pSettings->m_bShowWarning = !!m_chShowWarning;
	m_pSettings->m_bTerminateProcess = !!m_chTerminateProcess;

	m_pSettings->SaveSettings();
//+ ------------------------------------------------------------------------------------------
	m_btApply.EnableWindow(FALSE);
}

void CDSandBoxSettings::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CDialog::OnSettingChange(uFlags, lpszSection);
	
	// TODO: Add your message handler code here
	gpProcList->EnvironmentChanged();
}
