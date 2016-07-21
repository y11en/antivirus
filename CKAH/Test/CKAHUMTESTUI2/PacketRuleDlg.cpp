// PacketRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "PacketRuleDlg.h"
#include "RuleTimesDlg.h"
#include <atlconv.h>
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CPacketRuleDlg dialog


CPacketRuleDlg::CPacketRuleDlg (HPACKETRULE hRule, CWnd* pParent /*=NULL*/)
	: CDialog(CPacketRuleDlg::IDD, pParent),
	m_hRule (NULL)
{
	//{{AFX_DATA_INIT(CPacketRuleDlg)
	m_Name = _T("");
	m_Proto = 0;
	m_ICMPCode = 0;
	m_Block = FALSE;
	m_Notify = FALSE;
	m_Log = FALSE;
	m_Enable = FALSE;
	m_ICMPType = 0;
	//}}AFX_DATA_INIT

	PacketRule_Duplicate (hRule, &m_hRule);

	if (m_hRule)
	{
		ULONG nSize = PacketRule_GetName (m_hRule, NULL, 0);
		if (nSize)
		{
			WCHAR *pName = new WCHAR[nSize];
			if (pName && PacketRule_GetName (m_hRule, pName, nSize) == nSize - 1)
				m_Name = CString (pName);

			delete []pName;
		}

		PacketRule_GetProtocol (m_hRule, &m_Proto);
		PacketRule_GetICMPCode (m_hRule, &m_ICMPCode);
		PacketRule_GetICMPType (m_hRule, &m_ICMPType);
		bool bBlock = false;
		PacketRule_GetIsBlocking (m_hRule, &bBlock);
		m_Block = bBlock;
		bool bNotify = false;
		PacketRule_GetIsNotify (m_hRule, &bNotify);
		m_Notify = bNotify;
		bool bLog = false;
		PacketRule_GetIsLog (m_hRule, &bLog);
		m_Log = bLog;
		bool bEnable = false;
		PacketRule_GetIsEnabled (m_hRule, &bEnable);
		m_Enable = bEnable;
	}
}

CPacketRuleDlg::~CPacketRuleDlg ()
{
	PacketRule_Delete (m_hRule);
}


void CPacketRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPacketRuleDlg)
	DDX_Control(pDX, IDC_RemoteAddressList, m_RemoteAddressListCtrl);
	DDX_Control(pDX, IDC_LocalAddressList, m_LocalAddressListCtrl);
	DDX_Control(pDX, IDC_LocalPortList, m_LocalPortListCtrl);
	DDX_Control(pDX, IDC_RemotePortList, m_RemotePortListCtrl);
	DDX_Text(pDX, IDC_PacketRuleName, m_Name);
	DDX_Text(pDX, IDC_ProtoPacket, m_Proto);
	DDX_Text(pDX, IDC_ProtoPacket2, m_ICMPCode);
	DDX_Check(pDX, IDC_BlockPacketRule, m_Block);
	DDX_Check(pDX, IDC_NotifyPacket, m_Notify);
	DDX_Check(pDX, IDC_LogPacket, m_Log);
	DDX_Check(pDX, IDC_EnablePacket, m_Enable);
	DDX_Text(pDX, IDC_ProtoPacket3, m_ICMPType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPacketRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CPacketRuleDlg)
	ON_BN_CLICKED(IDC_AddRemotePort, OnAddRemotePort)
	ON_BN_CLICKED(IDC_DeleteRemotePort, OnDeleteRemotePort)
	ON_BN_CLICKED(IDC_AddLocalPort, OnAddLocalPort)
	ON_BN_CLICKED(IDC_DeleteLocalPort, OnDeleteLocalPort)
	ON_BN_CLICKED(IDC_AddLocalAddress, OnAddLocalAddress)
	ON_BN_CLICKED(IDC_DeleteLocalAddress, OnDeleteLocalAddress)
	ON_BN_CLICKED(IDC_AddRemoteAddress, OnAddRemoteAddress)
	ON_BN_CLICKED(IDC_DeleteRemoteAddress, OnDeleteRemoteAddress)
	ON_BN_CLICKED(IDC_EditLocalAddress, OnEditLocalAddress)
	ON_BN_CLICKED(IDC_EditRemoteAddress, OnEditRemoteAddress)
	ON_BN_CLICKED(IDC_EditLocalPort, OnEditLocalPort)
	ON_BN_CLICKED(IDC_EditRemotePort, OnEditRemotePort)
	ON_BN_CLICKED(IDC_Times, OnTimes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPacketRuleDlg message handlers

BOOL CPacketRuleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_hRule)
	{
		AfxMessageBox (IDS_NO_RULE, MB_OK | MB_ICONSTOP);
		DestroyWindow ();
		return TRUE;
	}

	PacketDirection packetdir;
	PacketRule_GetPacketDirection (m_hRule, &packetdir);

	CheckRadioButton (IDC_DirIncomingPacket, IDC_DirBothPacket, 
		packetdir == pdIncoming ? IDC_DirIncomingPacket : 
			(packetdir == pdOutgoing ? IDC_DirOutgoingPacket : IDC_DirBothPacket));

	StreamDirection streamdir;
	PacketRule_GetStreamDirection (m_hRule, &streamdir);

	CheckRadioButton (IDC_DirIncomingStream, IDC_DirBothStream, 
		streamdir == sdIncoming ? IDC_DirIncomingStream : 
			(streamdir == sdOutgoing ? IDC_DirOutgoingStream : IDC_DirBothStream));

	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPRemotePorts (m_hRule, &hPorts);
	RefreshPortList (m_RemotePortListCtrl, hPorts);
	RulePorts_Delete (hPorts);

	hPorts = NULL;
	PacketRule_GetTCPUDPLocalPorts (m_hRule, &hPorts);
	RefreshPortList (m_LocalPortListCtrl, hPorts);
	RulePorts_Delete (hPorts);

	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetRemoteAddresses (m_hRule, &hAddresses);
	RefreshAddressList (m_RemoteAddressListCtrl, hAddresses);
	RuleAddresses_Delete (hAddresses);

	hAddresses = NULL;
	PacketRule_GetLocalAddresses (m_hRule, &hAddresses);
	RefreshAddressList (m_LocalAddressListCtrl, hAddresses);
	RuleAddresses_Delete (hAddresses);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPacketRuleDlg::OnOK() 
{
	if (!UpdateData (TRUE))
		return;

	USES_CONVERSION;

	PacketRule_SetName (m_hRule, T2W ((LPTSTR)(LPCTSTR)m_Name));
	PacketRule_SetIsEnabled (m_hRule, !!m_Enable);
	PacketRule_SetIsBlocking (m_hRule, !!m_Block);

	int nDirPacketChecked = GetCheckedRadioButton (IDC_DirIncomingPacket, IDC_DirBothPacket);
	PacketDirection packetdir = nDirPacketChecked == IDC_DirIncomingPacket ? pdIncoming : 
		(nDirPacketChecked == IDC_DirOutgoingPacket ? pdOutgoing : pdBoth);
	PacketRule_SetPacketDirection (m_hRule, packetdir);

	int nDirStreamChecked = GetCheckedRadioButton (IDC_DirIncomingStream, IDC_DirBothStream);
	StreamDirection streamdir = nDirStreamChecked == IDC_DirIncomingStream ? sdIncoming : 
		(nDirStreamChecked == IDC_DirOutgoingStream ? sdOutgoing : sdBoth);
	PacketRule_SetStreamDirection (m_hRule, streamdir);

	PacketRule_SetProtocol (m_hRule, m_Proto);
	PacketRule_SetICMPCode (m_hRule, m_ICMPCode);
	PacketRule_SetICMPType (m_hRule, m_ICMPType);
	PacketRule_SetIsNotify (m_hRule, !!m_Notify);
	PacketRule_SetIsLog (m_hRule, !!m_Log);

	CDialog::OnOK();
}

void CPacketRuleDlg::OnAddRemotePort() 
{
	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPRemotePorts (m_hRule, &hPorts);
	AddPort (m_RemotePortListCtrl, hPorts, this);
	RulePorts_Delete (hPorts);
}

void CPacketRuleDlg::OnEditRemotePort() 
{
	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPRemotePorts (m_hRule, &hPorts);
	EditPort (m_RemotePortListCtrl, hPorts, this);
	RulePorts_Delete (hPorts);
}

void CPacketRuleDlg::OnDeleteRemotePort() 
{
	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPRemotePorts (m_hRule, &hPorts);
	DeletePort (m_RemotePortListCtrl, hPorts);
	RulePorts_Delete (hPorts);
}

void CPacketRuleDlg::OnAddLocalPort() 
{
	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPLocalPorts (m_hRule, &hPorts);
	AddPort (m_LocalPortListCtrl, hPorts, this);
	RulePorts_Delete (hPorts);
}

void CPacketRuleDlg::OnEditLocalPort() 
{
	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPLocalPorts (m_hRule, &hPorts);
	EditPort (m_LocalPortListCtrl, hPorts, this);
	RulePorts_Delete (hPorts);
}

void CPacketRuleDlg::OnDeleteLocalPort() 
{
	HRULEPORTS hPorts = NULL;
	PacketRule_GetTCPUDPLocalPorts (m_hRule, &hPorts);
	DeletePort (m_LocalPortListCtrl, hPorts);
	RulePorts_Delete (hPorts);
}

void CPacketRuleDlg::OnAddLocalAddress() 
{
	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetLocalAddresses (m_hRule, &hAddresses);
	AddAddress (m_LocalAddressListCtrl, hAddresses, this);
	RuleAddresses_Delete (hAddresses);
}

void CPacketRuleDlg::OnEditLocalAddress() 
{
	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetLocalAddresses (m_hRule, &hAddresses);
	EditAddress (m_LocalAddressListCtrl, hAddresses, this);
	RuleAddresses_Delete (hAddresses);
}

void CPacketRuleDlg::OnDeleteLocalAddress() 
{
	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetLocalAddresses (m_hRule, &hAddresses);
	DeleteAddress (m_LocalAddressListCtrl, hAddresses);
	RuleAddresses_Delete (hAddresses);
}

void CPacketRuleDlg::OnAddRemoteAddress() 
{
	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetRemoteAddresses (m_hRule, &hAddresses);
	AddAddress (m_RemoteAddressListCtrl, hAddresses, this);
	RuleAddresses_Delete (hAddresses);
}

void CPacketRuleDlg::OnEditRemoteAddress() 
{
	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetRemoteAddresses (m_hRule, &hAddresses);
	EditAddress (m_RemoteAddressListCtrl, hAddresses, this);
	RuleAddresses_Delete (hAddresses);
}

void CPacketRuleDlg::OnDeleteRemoteAddress() 
{
	HRULEADDRESSES hAddresses = NULL;
	PacketRule_GetRemoteAddresses (m_hRule, &hAddresses);
	DeleteAddress (m_RemoteAddressListCtrl, hAddresses);
	RuleAddresses_Delete (hAddresses);
}

void CPacketRuleDlg::OnTimes() 
{
    HRULETIMES hTimes = NULL;
    PacketRule_GetTimes (m_hRule, &hTimes);

	CRuleTimesDlg RuleTimesDlg (hTimes, this);

	if (RuleTimesDlg.DoModal () == IDOK)
	{
        PacketRule_SetTimes (m_hRule, RuleTimesDlg.m_hTimes);
	}

	RuleTimes_Delete (hTimes);
}
