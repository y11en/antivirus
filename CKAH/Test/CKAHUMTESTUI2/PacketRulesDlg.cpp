// PacketRulesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "PacketRulesDlg.h"
#include "PacketRuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CPacketRulesDlg dialog


CPacketRulesDlg::CPacketRulesDlg(CKAHFW::HPACKETRULES hRules, CWnd* pParent /*=NULL*/)
	: CDialog(CPacketRulesDlg::IDD, pParent),
	m_hRules (NULL)
{
	//{{AFX_DATA_INIT(CPacketRulesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	PacketRules_Duplicate (hRules, &m_hRules);
}

CPacketRulesDlg::~CPacketRulesDlg ()
{
	PacketRules_Delete (m_hRules);
}

void CPacketRulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPacketRulesDlg)
	DDX_Control(pDX, IDC_LIST2, m_RuleListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPacketRulesDlg, CDialog)
	//{{AFX_MSG_MAP(CPacketRulesDlg)
	ON_BN_CLICKED(IDC_AddPacketRule, OnAddPacketRule)
	ON_BN_CLICKED(IDC_EditPacketRule, OnEditPacketRule)
	ON_BN_CLICKED(IDC_DeletePacketRule, OnDeletePacketRule)
	ON_BN_CLICKED(IDC_RestorePacketRule, OnRestorePacketRule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPacketRulesDlg message handlers

BOOL CPacketRulesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RefreshList ();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPacketRulesDlg::RefreshList ()
{
	m_RuleListCtrl.ResetContent ();

	int n = PacketRules_GetCount (m_hRules);

	for (int i = 0; i < n; ++i)
	{
		HPACKETRULE hRule = NULL;
		if (PacketRules_GetItem (m_hRules, i, &hRule) != srOK)
			continue;

		ULONG nSize = PacketRule_GetName (hRule, NULL, 0);
		if (nSize)
		{
			WCHAR *pName = new WCHAR[nSize];
			if (pName)
			{
				if (PacketRule_GetName (hRule, pName, nSize) == nSize - 1)
				{
					CString RuleName (pName);
					m_RuleListCtrl.AddString (RuleName.IsEmpty () ? _T("<no name>") : RuleName);
				}

				delete []pName;
			}
		}

		PacketRule_Delete (hRule);
	}
}

void CPacketRulesDlg::OnAddPacketRule() 
{
	HPACKETRULE hRule = PacketRule_Create ();

	CPacketRuleDlg PacketRuleDlg (hRule, this);

	if (PacketRuleDlg.DoModal () == IDOK)
	{
		PacketRules_AddItemToBack (m_hRules, PacketRuleDlg.m_hRule);
		RefreshList ();
	}

	PacketRule_Delete (hRule);
}

void CPacketRulesDlg::OnEditPacketRule() 
{
	int nSel = m_RuleListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		HPACKETRULE hRule = NULL;

		if (PacketRules_GetItem (m_hRules, nSel, &hRule) != srOK)
			return;

		CPacketRuleDlg PacketRuleDlg (hRule, this);

		if (PacketRuleDlg.DoModal () == IDOK)
		{
			PacketRules_DeleteItem (m_hRules, nSel);
			PacketRules_InsertItem (m_hRules, nSel, PacketRuleDlg.m_hRule);
			RefreshList ();
		}

		PacketRule_Delete (hRule);
	}	
}

void CPacketRulesDlg::OnDeletePacketRule() 
{
	int nSel = m_RuleListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		PacketRules_DeleteItem (m_hRules, nSel);
		RefreshList ();
		if(m_RuleListCtrl.SetCurSel(nSel) == LB_ERR)
			m_RuleListCtrl.SetCurSel(m_RuleListCtrl.GetCount() - 1);
	}	
}

struct DefPacketRuleItem
{
	LPCWSTR Name;
	WellKnownProtocol Proto;
	PacketDirection PacketDirection;
    StreamDirection StreamDirection;
	USHORT RemotePort;
	USHORT LocalPort;
};

const DefPacketRuleItem g_DefPacketRuleItemList[] =
{
	{ L"BOOTP Server (remote)", PROTO_UDP, pdBoth, sdBoth, 68, 0 }, // BOOTPC
	{ L"BOOTP Server (local)", PROTO_UDP, pdBoth, sdBoth, 0, 68 },
	{ L"DCOM (UDP)", PROTO_UDP, pdBoth, sdBoth, 135, 0}, // DCOM UDP
	{ L"DCOM (TCP)", PROTO_TCP, pdBoth, sdBoth, 135, 0}, // DCOM TCP
	{ L"Common Internet File System (UDP, remote)", PROTO_UDP, pdBoth, sdBoth, 445, 0 }, // CIFS UDP
	{ L"Common Internet File System (UDP, local)", PROTO_UDP, pdBoth, sdBoth, 0, 445 },
	{ L"Common Internet File System (TCP, remote)", PROTO_TCP, pdBoth, sdBoth, 445, 0 }, // CIFS TCP
	{ L"Common Internet File System (TCP, local)", PROTO_TCP, pdBoth, sdBoth, 0, 445 },
	{ L"LDAP (TCP)", PROTO_TCP, pdBoth, sdBoth, 389, 0 }, // LDAP TCP
	{ L"LDAP (TCP)", PROTO_UDP, pdBoth, sdBoth, 389, 0 },	// LDAP UDP
	{ L"LDAP SSL", PROTO_TCP, pdBoth, sdBoth, 636, 0 },	// LDAP SSL
	{ L"Kerberos (TCP)", PROTO_TCP, pdBoth, sdBoth, 88, 0 },  // KERBEROS TCP
	{ L"Kerberos (UDP)", PROTO_UDP, pdBoth, sdBoth, 88, 0 },	// KERBEROS UDP
	{ L"NetBIOS name service (remote)", PROTO_UDP, pdBoth, sdBoth, 137, 0 }, // NBNAME
	{ L"NetBIOS name service (local)", PROTO_UDP, pdBoth, sdBoth, 0, 137 },
	{ L"NetBIOS datagram (remote)", PROTO_UDP, pdBoth, sdBoth, 138, 0 }, // NBDATAGRAM
	{ L"NetBIOS datagram (local)", PROTO_UDP, pdBoth, sdBoth, 0, 138 },
	{ L"NetBIOS session (remote)", PROTO_TCP, pdBoth, sdBoth, 139, 0 }, // NBSESSION
	{ L"NetBIOS session (local)", PROTO_TCP, pdBoth, sdBoth, 0, 139 },
	{ L"DNS", PROTO_UDP, pdBoth, sdBoth, 53, 0 } // DNS
};

void CPacketRulesDlg::OnRestorePacketRule() 
{
	if (AfxMessageBox (IDS_PROMPT_RESTORE_DEFAULT, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		HPACKETRULES hRules = PacketRules_Create ();

		if (hRules)
		{
			for (int i = 0; i < sizeof (g_DefPacketRuleItemList) / sizeof (g_DefPacketRuleItemList[0]); ++i)
			{
				HPACKETRULE hRule = PacketRule_Create ();

				if (hRule)
				{
					PacketRule_SetIsEnabled (hRule, true);
					PacketRule_SetIsBlocking (hRule, false);
					
					PacketRule_SetName (hRule, g_DefPacketRuleItemList[i].Name);
					PacketRule_SetPacketDirection (hRule, g_DefPacketRuleItemList[i].PacketDirection);
					PacketRule_SetStreamDirection (hRule, g_DefPacketRuleItemList[i].StreamDirection);
					PacketRule_SetProtocol (hRule, g_DefPacketRuleItemList[i].Proto);

					if (g_DefPacketRuleItemList[i].RemotePort)
					{
						HRULEPORTS hPorts = RulePorts_Create ();
						if (hPorts)
						{
							HRULEPORT hPort = RulePort_Create (rptSingle);
							if (hPort)
							{
								RulePort_SetSinglePort (hPort, g_DefPacketRuleItemList[i].RemotePort);
								RulePorts_AddItemToBack (hPorts, hPort);
								RulePort_Delete (hPort);
							}
							PacketRule_SetTCPUDPRemotePorts (hRule, hPorts);
							RulePorts_Delete (hPorts);
						}
					}

					if (g_DefPacketRuleItemList[i].LocalPort)
					{
						HRULEPORTS hPorts = RulePorts_Create ();
						if (hPorts)
						{
							HRULEPORT hPort = RulePort_Create (rptSingle);
							if (hPort)
							{
								RulePort_SetSinglePort (hPort, g_DefPacketRuleItemList[i].LocalPort);
								RulePorts_AddItemToBack (hPorts, hPort);
								RulePort_Delete (hPort);
							}
							PacketRule_SetTCPUDPLocalPorts (hRule, hPorts);
							RulePorts_Delete (hPorts);
						}
					}
					PacketRules_AddItemToBack (hRules, hRule);
					PacketRule_Delete (hRule);
				}
			}
			PacketRules_Delete (m_hRules);
			PacketRules_Duplicate (hRules, &m_hRules);
			PacketRules_Delete (hRules);

			RefreshList ();
		}
	}	
}
