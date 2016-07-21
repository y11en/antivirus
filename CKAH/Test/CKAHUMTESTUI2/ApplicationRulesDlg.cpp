// ApplicationRulesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "ApplicationRulesDlg.h"
#include "ApplicationRuleDlg.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CApplicationRulesDlg dialog


CApplicationRulesDlg::CApplicationRulesDlg (CKAHFW::HAPPRULES hRules, CWnd* pParent /*=NULL*/)
	: CDialog(CApplicationRulesDlg::IDD, pParent),
	m_hRules (NULL)
{
	//{{AFX_DATA_INIT(CApplicationRulesDlg)
	m_RecalcHashes = TRUE;
	//}}AFX_DATA_INIT
	ApplicationRules_Duplicate (hRules, &m_hRules);
}

CApplicationRulesDlg::~CApplicationRulesDlg ()
{
	ApplicationRules_Delete (m_hRules);
}

void CApplicationRulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CApplicationRulesDlg)
	DDX_Control(pDX, IDC_RULELIST, m_RuleListCtrl);
	DDX_Check(pDX, IDC_RecalcHash, m_RecalcHashes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CApplicationRulesDlg, CDialog)
	//{{AFX_MSG_MAP(CApplicationRulesDlg)
	ON_BN_CLICKED(IDC_AddApplicationRule, OnAddApplicationRule)
	ON_BN_CLICKED(IDC_EditApplicationRule, OnEditApplicationRule)
	ON_BN_CLICKED(IDC_DeleteApplicationRule, OnDeleteApplicationRule)
	ON_BN_CLICKED(IDC_RestoreApplicationRule, OnRestoreApplicationRule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApplicationRulesDlg message handlers

BOOL CApplicationRulesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RefreshList ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CApplicationRulesDlg::RefreshList ()
{
	m_RuleListCtrl.ResetContent ();

	int n = ApplicationRules_GetCount (m_hRules);

	for (int i = 0; i < n; ++i)
	{
		HAPPRULE hRule = NULL;
		if (ApplicationRules_GetItem (m_hRules, i, &hRule) != srOK)
			continue;

		ULONG nSize = ApplicationRule_GetName (hRule, NULL, 0);
		if (nSize)
		{
			WCHAR *pName = new WCHAR[nSize];
			if (pName)
			{
				if (ApplicationRule_GetName (hRule, pName, nSize) == nSize - 1)
				{
					CString RuleName (pName);
					m_RuleListCtrl.AddString (RuleName.IsEmpty () ? _T("<no name>") : RuleName);
				}

				delete []pName;
			}
		}

		ApplicationRule_Delete (hRule);
	}
}

void CApplicationRulesDlg::OnAddApplicationRule() 
{
	HAPPRULE hRule = ApplicationRule_Create ();

	CApplicationRuleDlg ApplicationRuleDlg (hRule, this);

	if (ApplicationRuleDlg.DoModal () == IDOK)
	{
		ApplicationRules_AddItemToBack (m_hRules, ApplicationRuleDlg.m_hRule);
		RefreshList ();
	}

	ApplicationRule_Delete (hRule);
}

void CApplicationRulesDlg::OnEditApplicationRule() 
{
	int nSel = m_RuleListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		HAPPRULE hRule = NULL;

		if (ApplicationRules_GetItem (m_hRules, nSel, &hRule) != srOK)
			return;

		CApplicationRuleDlg ApplicationRuleDlg (hRule, this);

		if (ApplicationRuleDlg.DoModal () == IDOK)
		{
			ApplicationRules_DeleteItem (m_hRules, nSel);
			ApplicationRules_InsertItem (m_hRules, nSel, ApplicationRuleDlg.m_hRule);
			RefreshList ();
		}

		ApplicationRule_Delete (hRule);
	}	
}

void CApplicationRulesDlg::OnDeleteApplicationRule() 
{
	int nSel = m_RuleListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		ApplicationRules_DeleteItem (m_hRules, nSel);
		RefreshList ();
		if(m_RuleListCtrl.SetCurSel(nSel) == LB_ERR)
			m_RuleListCtrl.SetCurSel(m_RuleListCtrl.GetCount() - 1);
	}	
}

struct DefApplicationRuleItem
{
	LPCTSTR FileName;
};

const DefApplicationRuleItem g_DefApplicationRuleItemList[] =
{
	{ _T("iexplore.exe") },
	{ _T("outlook.exe") },
	{ _T("mapisp32.exe") },
	{ _T("msimn.exe") },
	{ _T("thebat.exe") },
	{ _T("msmsgs.exe") },
	{ _T("aim.exe") },
	{ _T("icq.exe") },
	{ _T("lsass.exe") },
	{ _T("ndisuio.sys") },
	{ _T("ntoskrnl.exe") },
	{ _T("services.exe") },
	{ _T("svchost.exe") },
	{ _T("userinit.exe") },
	{ _T("winlogon.exe") },
	{ _T("wuauclt.exe") }
};

CString GetAppPath (LPCTSTR szExeName)
{
	CString szPath;
	
	TCHAR path[MAX_PATH];
	DWORD dwBufLen = MAX_PATH;
	
	_stprintf (path, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s"), szExeName);
	
	HKEY hKey;
	
    if (ERROR_SUCCESS == ::RegOpenKeyEx (HKEY_LOCAL_MACHINE, path, 0L, KEY_READ, &hKey))
	{
		if (ERROR_SUCCESS == ::RegQueryValueEx (hKey, NULL, NULL, NULL, (LPBYTE) path, &dwBufLen))
		{
			if (dwBufLen)
			{
				::ExpandEnvironmentStrings (path, szPath.GetBuffer  ( MAX_PATH ), MAX_PATH);
				szPath.ReleaseBuffer ();
			}
		}
		::RegCloseKey( hKey );
	}
	
	return szPath;
	
}

void CApplicationRulesDlg::OnRestoreApplicationRule() 
{
	USES_CONVERSION;

	if (AfxMessageBox (IDS_PROMPT_RESTORE_DEFAULT, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		HAPPRULES hRules = ApplicationRules_Create ();

		if (hRules)
		{
			for (int i = 0; i < sizeof (g_DefApplicationRuleItemList) / sizeof (g_DefApplicationRuleItemList[0]); ++i)
			{
				CString szFileName = GetAppPath (g_DefApplicationRuleItemList[i].FileName);

				if (szFileName.IsEmpty())
				{
					TCHAR szBuffer[MAX_PATH];
					LPTSTR lpFileStart;

					if (SearchPath(NULL,
							g_DefApplicationRuleItemList[i].FileName,
							NULL,
							sizeof(szBuffer) / sizeof(szBuffer[0]), szBuffer, &lpFileStart))
					{
						szFileName = szBuffer;
					}
				}

				if (!szFileName.IsEmpty () && ::GetFileAttributes (szFileName) != (DWORD)-1)
				{
					HAPPRULE hRule = ApplicationRule_Create ();
					HAPPRULEELEMENT hElement = ApplicationRuleElement_Create ();
					ApplicationRuleElement_SetIsActive (hElement, true);

					if (hRule && hElement)
					{
						ApplicationRule_SetName (hRule, T2W (g_DefApplicationRuleItemList[i].FileName));
						ApplicationRule_SetApplicationName (hRule, T2W (szFileName));
						ApplicationRule_SetIsEnabled (hRule, true);
						ApplicationRule_SetIsBlocking (hRule, false);
						
						ApplicationRule_AddElementToBack (hRule, hElement);

						ApplicationRules_AddItemToBack (hRules, hRule);
					}
					ApplicationRuleElement_Delete (hElement);
					ApplicationRule_Delete (hRule);
				}
			}
			ApplicationRules_Delete (m_hRules);
			ApplicationRules_Duplicate (hRules, &m_hRules);
			ApplicationRules_Delete (hRules);

			RefreshList ();
		}
	}
}

void CApplicationRulesDlg::OnOK() 
{
	if(!UpdateData())
		return;

	if(m_RecalcHashes)
	{
		int n = ApplicationRules_GetCount (m_hRules);

		UCHAR Hash[FW_HASH_SIZE];

		for (int i = 0; i < n; ++i)
		{
			HAPPRULE hRule = NULL;
			if (ApplicationRules_GetItem (m_hRules, i, &hRule) != srOK)
				continue;

			ULONG nSize = ApplicationRule_GetApplicationName (hRule, NULL, 0);
			if (nSize)
			{
				WCHAR *pName = new WCHAR[nSize];
				if (pName)
				{
					if (ApplicationRule_GetApplicationName (hRule, pName, nSize) == nSize - 1 &&
						CKAHFW::GetApplicationChecksum (pName, Hash) == CKAHUM::srOK)
					{
						ApplicationRule_SetApplicationChecksum (hRule, Hash);
					}

					delete []pName;
				}
			}

			ApplicationRule_Delete (hRule);
		}
	}
	
	CDialog::OnOK();
}
