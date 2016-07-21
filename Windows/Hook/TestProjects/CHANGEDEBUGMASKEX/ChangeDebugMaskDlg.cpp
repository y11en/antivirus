// ChangeDebugMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChangeDebugMask.h"
#include "ChangeDebugMaskDlg.h"
#include "Common\Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskDlg dialog 

CChangeDebugMaskDlg::CChangeDebugMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeDebugMaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeDebugMaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChangeDebugMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeDebugMaskDlg)
	DDX_Control(pDX, IDC_COMBO_DRIVER, m_cbDriver);
	DDX_Control(pDX, IDC_LIST_MASK, m_lMask);
	DDX_Control(pDX, IDC_COMBO_DEBUGLEVEL, m_cbLevel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeDebugMaskDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeDebugMaskDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_UNCHECKALL, OnButtonUncheckall)
	ON_BN_CLICKED(IDC_BUTTON_CHECKALL, OnButtonCheckall)
	ON_CBN_SELCHANGE(IDC_COMBO_DRIVER, OnSelchangeComboDriver)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskDlg message handlers

BOOL CChangeDebugMaskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	ListView_SetExtendedListViewStyle(m_lMask.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	
	m_lMask.InsertColumn(0, "Category", LVCFMT_LEFT, 180);
	m_lMask.InsertColumn(1, "Comments", LVCFMT_LEFT, 180);

	LoadDebugPlugins();
	m_cbDriver.SetCurSel(0);
	ReloadSettings(m_cbDriver.GetItemData(0));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChangeDebugMaskDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChangeDebugMaskDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CChangeDebugMaskDlg::OnOK() 
{
	// TODO: Add extra validation here
	OnButtonApply();
	UnloadDebugPlugins();
	CDialog::OnOK();
}

void CChangeDebugMaskDlg::OnButtonUncheckall() 
{
	// TODO: Add your control notification handler code here
	int count = m_lMask.GetItemCount();
	for (int cou = 0; cou < count; cou++)
		m_lMask.SetCheck(cou, FALSE);
}

void CChangeDebugMaskDlg::OnButtonCheckall() 
{
	// TODO: Add your control notification handler code here
	int count = m_lMask.GetItemCount();
	for (int cou = 0; cou < count; cou++)
		m_lMask.SetCheck(cou, TRUE);
}

void CChangeDebugMaskDlg::OnSelchangeComboDriver() 
{
	// TODO: Add your control notification handler code here
	ReloadSettings(m_cbDriver.GetItemData(m_cbDriver.GetCurSel()));
}

void CChangeDebugMaskDlg::ReloadSettings(DWORD dwID)
{
	HINSTANCE hPluginDll = (HINSTANCE)dwID;
	m_cbLevel.ResetContent();
	m_lMask.DeleteAllItems();

	PGETMODULEINFO GetModuleInfo = (PGETMODULEINFO) GetProcAddress(hPluginDll, SZ_GET_MODULE_INFO);
	PTURNDBGSETTINGS TurnDbgSettings = (PTURNDBGSETTINGS) GetProcAddress(hPluginDll, SZ_TURN_DBG_SETTINGS);
	if(GetModuleInfo && TurnDbgSettings){
		TCHAR* szModuleName = NULL;
		TCHAR** szDbgLevels = NULL;
		TCHAR** szCategories = NULL;
		TCHAR** szCatComments = NULL;
		ULONG Level = 0;
		ULONG Mask = 0;
		if( GetModuleInfo(&szModuleName, &szDbgLevels, &szCategories, &szCatComments) &&
			TurnDbgSettings(FALSE, &Level, &Mask)){
			//Insert DbgLevels
			int cou = -1;
			while(lstrcmp(szDbgLevels[++cou], EndOfArray))
				m_cbLevel.AddString(szDbgLevels[cou]);
			
			m_cbLevel.SetCurSel(Level);

			//Insert categories
			cou = -1;
			int count= 0;
			while(lstrcmp(szCategories[++cou], EndOfArray)){
				if(lstrcmp(szCategories[cou], NullStr)){
					int dwx = m_lMask.InsertItem(count++, szCategories[cou]);
					m_lMask.SetCheck(dwx, Mask & (1 << cou));
					m_lMask.SetItemData(dwx, cou);
				}
			}


		}else
			AfxMessageBox(_T("Debug plugin call error"));
	}
}


void CChangeDebugMaskDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	HINSTANCE hPluginDll = (HINSTANCE)m_cbDriver.GetItemData(m_cbDriver.GetCurSel());
	ULONG Level = (ULONG)m_cbLevel.GetCurSel();
	ULONG Mask = 0;
	int count = m_lMask.GetItemCount();
	for (int cou = 0; cou < count; cou++)
		Mask |= m_lMask.GetCheck(cou)?(1L<<m_lMask.GetItemData(cou)):0L;

	PTURNDBGSETTINGS TurnDbgSettings = (PTURNDBGSETTINGS) GetProcAddress(hPluginDll, SZ_TURN_DBG_SETTINGS);
	if(TurnDbgSettings)
		TurnDbgSettings(TRUE, &Level, &Mask);

}

void CChangeDebugMaskDlg::LoadDebugPlugins()
{
	WIN32_FIND_DATA FileData;
	BOOL fFinished = FALSE;
	TCHAR	buf[MAX_PATH];
	TCHAR	szDirName[MAX_PATH];
	TCHAR	szFileName[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, buf);
	_stprintf(szDirName, _T("%s\\"), buf);
	_stprintf(szFileName, _T("%s%s"), szDirName, _T("*.dll"));
	HANDLE hSearch = FindFirstFile(szFileName, &FileData);

	if (hSearch != INVALID_HANDLE_VALUE) 
	{
		while (!fFinished) 
		{ 
			TRY{
			_stprintf(szFileName, _T("%s%s"), szDirName, FileData.cFileName);
			HINSTANCE hPluginDll = AfxLoadLibrary(szFileName);
			if (hPluginDll)
			{
			   PGETMODULEINFO GetModuleInfo = (PGETMODULEINFO) GetProcAddress(hPluginDll, SZ_GET_MODULE_INFO);
			   if(GetModuleInfo){
				   TCHAR* szModuleName = NULL;
				   TCHAR** szDbgLevels = NULL;
				   TCHAR** szCategories = NULL;
				   TCHAR** szCatComments = NULL;
				   if(GetModuleInfo(&szModuleName, &szDbgLevels, &szCategories, &szCatComments))
				   	   if(szModuleName)
						   m_cbDriver.SetItemData(m_cbDriver.AddString(szModuleName), (DWORD)hPluginDll);
					
			   }
			}
			}CATCH(CException, e){
				e->ReportError();
			}END_CATCH
			if (!FindNextFile(hSearch, &FileData)) 
			{
				fFinished = TRUE; 
			} 
		} 
	}
}

void CChangeDebugMaskDlg::UnloadDebugPlugins()
{
	for (int i=0;i < m_cbDriver.GetCount();i++)
		AfxFreeLibrary((HINSTANCE)m_cbDriver.GetItemData(i));
	
	m_cbDriver.ResetContent();
	m_cbLevel.ResetContent();
	m_lMask.DeleteAllItems();
}

void CChangeDebugMaskDlg::OnCancel() 
{
	UnloadDebugPlugins();
	CDialog::OnCancel();
}
