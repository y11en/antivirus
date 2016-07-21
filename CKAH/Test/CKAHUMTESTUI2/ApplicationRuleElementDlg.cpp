// ApplicationRuleElementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "ApplicationRuleElementDlg.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CApplicationRuleElementDlg dialog


CApplicationRuleElementDlg::CApplicationRuleElementDlg(CKAHFW::HAPPRULEELEMENT hElement, CWnd* pParent /*=NULL*/)
	: CDialog(CApplicationRuleElementDlg::IDD, pParent)
{
    m_hElement = hElement;

	//{{AFX_DATA_INIT(CApplicationRuleElementDlg)
	m_Proto = 0;
	m_Enabled = FALSE;
	//}}AFX_DATA_INIT
}


void CApplicationRuleElementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CApplicationRuleElementDlg)
	DDX_Control(pDX, IDC_LocalPortList, m_LocalPorts);
	DDX_Control(pDX, IDC_RemotePortList, m_RemotePorts);
	DDX_Control(pDX, IDC_RemoteAddressList, m_RemoteAddresses);
	DDX_Text(pDX, IDC_ProtoPacket, m_Proto);
	DDX_Check(pDX, IDC_Enable, m_Enabled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CApplicationRuleElementDlg, CDialog)
	//{{AFX_MSG_MAP(CApplicationRuleElementDlg)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AddRemoteAddress, IDC_DeleteRemoteAddress, OnRemoteAddress)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AddRemotePort, IDC_DeleteRemotePort, OnRemotePort)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_AddLocalPort, IDC_DeleteLocalPort, OnLocalPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApplicationRuleElementDlg message handlers

BOOL CApplicationRuleElementDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    bool active;
    ApplicationRuleElement_GetIsActive(m_hElement, &active);
    m_Enabled = active;

    UCHAR proto = 0;
    ApplicationRuleElement_GetProtocol(m_hElement, &proto);
    m_Proto = proto;

	PacketDirection packetdir;
	ApplicationRuleElement_GetPacketDirection (m_hElement, &packetdir);

	CheckRadioButton (IDC_DirIncomingPacket, IDC_DirBothPacket, 
		packetdir == pdIncoming ? IDC_DirIncomingPacket : 
			(packetdir == pdOutgoing ? IDC_DirOutgoingPacket : IDC_DirBothPacket));

	StreamDirection streamdir;
	ApplicationRuleElement_GetStreamDirection (m_hElement, &streamdir);

	CheckRadioButton (IDC_DirIncomingStream, IDC_DirBothStream, 
		streamdir == sdIncoming ? IDC_DirIncomingStream : 
			(streamdir == sdOutgoing ? IDC_DirOutgoingStream : IDC_DirBothStream));


	HRULEPORTS hPorts = NULL;
	ApplicationRuleElement_GetRemotePorts (m_hElement, &hPorts);
	RefreshPortList (m_RemotePorts, hPorts);
	RulePorts_Delete (hPorts);

	hPorts = NULL;
	ApplicationRuleElement_GetLocalPorts (m_hElement, &hPorts);
	RefreshPortList (m_LocalPorts, hPorts);
	RulePorts_Delete (hPorts);

	HRULEADDRESSES hAddresses = NULL;
	ApplicationRuleElement_GetRemoteAddresses (m_hElement, &hAddresses);
	RefreshAddressList (m_RemoteAddresses, hAddresses);
	RuleAddresses_Delete (hAddresses);

	UpdateData (FALSE);
            
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CApplicationRuleElementDlg::OnRemoteAddress (UINT id) 
{
	HRULEADDRESSES hAddresses = NULL;
	ApplicationRuleElement_GetRemoteAddresses (m_hElement, &hAddresses);
	
	switch (id)
	{
	case IDC_AddRemoteAddress:
		AddAddress (m_RemoteAddresses, hAddresses, this);
		break;
	case IDC_EditRemoteAddress:
		EditAddress (m_RemoteAddresses, hAddresses, this);
		break;
	case IDC_DeleteRemoteAddress:
		DeleteAddress (m_RemoteAddresses, hAddresses);
		break;
	}

	RuleAddresses_Delete (hAddresses);
}

void CApplicationRuleElementDlg::OnRemotePort (UINT id) 
{
	HRULEPORTS hPorts = NULL;
	ApplicationRuleElement_GetRemotePorts (m_hElement, &hPorts);

	switch (id)
	{
	case IDC_AddRemotePort:
		AddPort (m_RemotePorts, hPorts, this);
		break;
	case IDC_EditRemotePort:
		EditPort (m_RemotePorts, hPorts, this);
		break;
	case IDC_DeleteRemotePort:
		DeletePort (m_RemotePorts, hPorts);
		break;
	}

	RulePorts_Delete (hPorts);
}

void CApplicationRuleElementDlg::OnLocalPort (UINT id) 
{
	HRULEPORTS hPorts = NULL;
	ApplicationRuleElement_GetLocalPorts (m_hElement, &hPorts);

	switch (id)
	{
	case IDC_AddLocalPort:
		AddPort (m_LocalPorts, hPorts, this);
		break;
	case IDC_EditLocalPort:
		EditPort (m_LocalPorts, hPorts, this);
		break;
	case IDC_DeleteLocalPort:
		DeletePort (m_LocalPorts, hPorts);
		break;
	}

	RulePorts_Delete (hPorts);
}

void CApplicationRuleElementDlg::OnOK() 
{
	if (!UpdateData (TRUE))
		return;

	ApplicationRuleElement_SetIsActive (m_hElement, !!m_Enabled);

	int nDirPacketChecked = GetCheckedRadioButton (IDC_DirIncomingPacket, IDC_DirBothPacket);
	PacketDirection packetdir = nDirPacketChecked == IDC_DirIncomingPacket ? pdIncoming : 
		(nDirPacketChecked == IDC_DirOutgoingPacket ? pdOutgoing : pdBoth);
	ApplicationRuleElement_SetPacketDirection (m_hElement, packetdir);

	int nDirStreamChecked = GetCheckedRadioButton (IDC_DirIncomingStream, IDC_DirBothStream);
	StreamDirection streamdir = nDirStreamChecked == IDC_DirIncomingStream ? sdIncoming : 
		(nDirStreamChecked == IDC_DirOutgoingStream ? sdOutgoing : sdBoth);
	ApplicationRuleElement_SetStreamDirection (m_hElement, streamdir);

	ApplicationRuleElement_SetProtocol (m_hElement, m_Proto);
    
	CDialog::OnOK();
}
