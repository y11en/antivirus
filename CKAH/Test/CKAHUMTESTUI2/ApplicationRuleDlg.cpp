// ApplicationRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "ApplicationRuleDlg.h"
#include "ApplicationRuleElementDlg.h"
#include "RuleTimesDlg.h"
#include "Utils.h"
#include <shlwapi.h>
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CApplicationRuleDlg dialog


CApplicationRuleDlg::CApplicationRuleDlg (CKAHFW::HAPPRULE hRule, CWnd* pParent /*=NULL*/)
	: CDialog(CApplicationRuleDlg::IDD, pParent),
	m_hRule (NULL)
{
	//{{AFX_DATA_INIT(CApplicationRuleDlg)
	m_Name = _T("");
	m_Block = FALSE;
	m_Notify = FALSE;
	m_Log = FALSE;
	m_Enable = FALSE;
	m_AppPath = _T("");
	m_CmdLine = _T("");
	m_IsCmdLine = FALSE;
	//}}AFX_DATA_INIT
	ApplicationRule_Duplicate (hRule, &m_hRule);

	if (m_hRule)
	{
		ULONG nSize = ApplicationRule_GetName (m_hRule, NULL, 0);
		if (nSize)
		{
			WCHAR *pName = new WCHAR[nSize];
			if (pName && ApplicationRule_GetName (m_hRule, pName, nSize) == nSize - 1)
				m_Name = CString (pName);

			delete []pName;
		}

		nSize = ApplicationRule_GetApplicationName (m_hRule, NULL, 0);
		if (nSize)
		{
			WCHAR *pName = new WCHAR[nSize];
			if (pName && ApplicationRule_GetApplicationName (m_hRule, pName, nSize) == nSize - 1)
				m_AppPath = CString (pName);

			delete []pName;
		}

        bool bIsCmdLine = false;
        ApplicationRule_GetIsCommandLine (m_hRule, &bIsCmdLine);
        m_IsCmdLine = bIsCmdLine;

        if (m_IsCmdLine)
        {
            nSize = ApplicationRule_GetCommandLine (m_hRule, NULL, 0);
            if (nSize)
            {
                WCHAR *pCmdLine = new WCHAR[nSize];
                if (pCmdLine && ApplicationRule_GetCommandLine (m_hRule, pCmdLine, nSize) == nSize - 1)
                    m_CmdLine = CString (pCmdLine);

                delete []pCmdLine;
            }
        }

		bool bBlock = false;
		ApplicationRule_GetIsBlocking (m_hRule, &bBlock);
		m_Block = bBlock;
		bool bNotify = false;
		ApplicationRule_GetIsNotify (m_hRule, &bNotify);
		m_Notify = bNotify;
		bool bLog = false;
		ApplicationRule_GetIsLog (m_hRule, &bLog);
		m_Log = bLog;
		bool bEnable = false;
		ApplicationRule_GetIsEnabled (m_hRule, &bEnable);
		m_Enable = bEnable;
	}
}

CApplicationRuleDlg::~CApplicationRuleDlg ()
{
	ApplicationRule_Delete (m_hRule);
}

void CApplicationRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CApplicationRuleDlg)
	DDX_Control(pDX, IDC_ELTLIST, m_Elements);
	DDX_Text(pDX, IDC_AppRuleName, m_Name);
	DDX_Check(pDX, IDC_BlockAppRule, m_Block);
	DDX_Check(pDX, IDC_NotifyApp, m_Notify);
	DDX_Check(pDX, IDC_LogApp, m_Log);
	DDX_Check(pDX, IDC_EnableApp, m_Enable);
	DDX_Text(pDX, IDC_AppPath, m_AppPath);
	DDX_Text(pDX, IDC_CmdLine, m_CmdLine);
	DDX_Check(pDX, IDC_IsCmdLine, m_IsCmdLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CApplicationRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CApplicationRuleDlg)
	ON_BN_CLICKED(IDC_BrowseApp, OnBrowseApp)
	ON_BN_CLICKED(IDC_AddElement, OnAddElement)
	ON_BN_CLICKED(IDC_EditElement, OnEditElement)
	ON_BN_CLICKED(IDC_DeleteElement, OnDeleteElement)
	ON_BN_CLICKED(IDC_Times, OnTimes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApplicationRuleDlg message handlers

BOOL CApplicationRuleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_hRule)
	{
		AfxMessageBox (IDS_NO_RULE, MB_OK | MB_ICONSTOP);
		DestroyWindow ();
		return TRUE;
	}

	typedef HRESULT ( WINAPI *fnSHAutoComplete) ( HWND hwndEdit, DWORD dwFlags );
	
	HMODULE hShlwapi32 = ::LoadLibrary (_T("Shlwapi.dll"));
	if (hShlwapi32)
	{
		fnSHAutoComplete SHAutoComplete = (fnSHAutoComplete)::GetProcAddress (hShlwapi32, "SHAutoComplete");
		
		if (SHAutoComplete)
		{
			HWND hwndEdit = GetDlgItem (IDC_AppPath)->GetSafeHwnd ();
			SHAutoComplete (hwndEdit, SHACF_FILESYSTEM | SHACF_FILESYS_ONLY);
		}
		
		::FreeLibrary (hShlwapi32);
	}

    RefreshElementList();

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CApplicationRuleDlg::OnBrowseApp() 
{
	CFileDialog fd (TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Executable (*.exe; *.com)|*.exe; *.com|All files (*.*)|*.*||"), this);

	if (fd.DoModal () == IDOK)
	{
		m_AppPath = fd.GetPathName ();
		SetDlgItemText (IDC_AppPath, m_AppPath);
	}
}

//////////////////////////////////////////////////////////////////////////

void CApplicationRuleDlg::OnOK() 
{
	if (!UpdateData (TRUE) || m_AppPath.IsEmpty ())
		return;

	USES_CONVERSION;

	ApplicationRule_SetName (m_hRule, T2W ((LPTSTR)(LPCTSTR)m_Name));
	ApplicationRule_SetApplicationName (m_hRule, T2W ((LPTSTR)(LPCTSTR)m_AppPath));	
    ApplicationRule_SetIsCommandLine (m_hRule, !!m_IsCmdLine);
    if (m_IsCmdLine)
    {
        ApplicationRule_SetCommandLine (m_hRule, T2W ((LPTSTR)(LPCTSTR)m_CmdLine));	
    }
	ApplicationRule_SetIsEnabled (m_hRule, !!m_Enable);
	ApplicationRule_SetIsBlocking (m_hRule, !!m_Block);

    RefreshElementList();

	ApplicationRule_SetIsNotify (m_hRule, !!m_Notify);
	ApplicationRule_SetIsLog (m_hRule, !!m_Log);
	
	CDialog::OnOK();
}

void CApplicationRuleDlg::OnAddElement() 
{
	HAPPRULEELEMENT hElement = ApplicationRuleElement_Create ();

	CApplicationRuleElementDlg ElementDlg (hElement, GetParent());

	if (ElementDlg.DoModal () == IDOK)
	{
		ApplicationRule_AddElementToBack (m_hRule, ElementDlg.m_hElement);
		RefreshElementList ();
	}

	ApplicationRuleElement_Delete (hElement);
}

void CApplicationRuleDlg::OnEditElement() 
{
	int nSel = m_Elements.GetCurSel ();

	if (nSel != LB_ERR)
	{
		HAPPRULEELEMENT hElement = NULL;

		if (ApplicationRule_GetElement (m_hRule, nSel, &hElement) != srOK)
			return;

		CApplicationRuleElementDlg ElementDlg (hElement, GetParent());

		if (ElementDlg.DoModal () == IDOK)
		{
			ApplicationRule_DeleteElement (m_hRule, nSel);
			ApplicationRule_InsertElement (m_hRule, nSel, ElementDlg.m_hElement);
			RefreshElementList ();
		}

		ApplicationRuleElement_Delete (hElement);
	}
}

void CApplicationRuleDlg::OnDeleteElement() 
{
	int nSel = m_Elements.GetCurSel ();

	if (nSel != LB_ERR)
	{
		ApplicationRule_DeleteElement (m_hRule, nSel);
		RefreshElementList ();
	}		
}

void CApplicationRuleDlg::RefreshElementList()
{
	m_Elements.ResetContent ();

    int n = ApplicationRule_GetElementCount(m_hRule);

    for (int i = 0; i < n; ++i)
    {
        HAPPRULEELEMENT hElement = NULL;
        ApplicationRule_GetElement(m_hRule, i, &hElement);

        UCHAR proto;
        ApplicationRuleElement_GetProtocol(hElement, &proto);

        LPTSTR Proto = _T("Unknown");
        if      (proto == CKAHFW::PROTO_TCP)  Proto = _T("TCP");
	    else if (proto == CKAHFW::PROTO_UDP)  Proto = _T("UDP");
        else if (proto == CKAHFW::PROTO_ICMP) Proto = _T("ICMP");
	    else if (proto == CKAHFW::PROTO_ALL)  Proto = _T("ALL");

        CKAHFW::PacketDirection PacketDirection;
        ApplicationRuleElement_GetPacketDirection(hElement, &PacketDirection);

        CKAHFW::StreamDirection StreamDirection;
        ApplicationRuleElement_GetStreamDirection(hElement, &StreamDirection);

        CString name;
        name.Format("%s %s packet, %s stream", Proto, 
            PacketDirection == CKAHFW::pdIncoming ? _T("incoming") : (PacketDirection == CKAHFW::pdOutgoing ? _T("outgoing") : (PacketDirection == CKAHFW::pdBoth ? _T("both") : _T("unknown"))),
            StreamDirection == CKAHFW::sdIncoming ? _T("incoming") : (StreamDirection == CKAHFW::sdOutgoing ? _T("outgoing") : (StreamDirection == CKAHFW::sdBoth ? _T("both") : _T("unknown")))
        );
        
        m_Elements.AddString(name);

        ApplicationRuleElement_Delete (hElement);
    }

}

void CApplicationRuleDlg::OnTimes() 
{
    HRULETIMES hTimes = NULL;
    ApplicationRule_GetTimes (m_hRule, &hTimes);

	CRuleTimesDlg RuleTimesDlg (hTimes, this);

	if (RuleTimesDlg.DoModal () == IDOK)
	{
        ApplicationRule_SetTimes (m_hRule, RuleTimesDlg.m_hTimes);
	}

	RuleTimes_Delete (hTimes);
}
