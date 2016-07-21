// PortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "PortDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CPortDlg dialog

CPortDlg::CPortDlg (HRULEPORT hPort, CWnd* pParent /*=NULL*/)
	: CDialog(CPortDlg::IDD, pParent),
	m_hPort (NULL)
{
	//{{AFX_DATA_INIT(CPortDlg)
	m_SinglePortValue = 0;
	m_RangeLoValue = 0;
	m_RangeHiValue = 0;
	//}}AFX_DATA_INIT

	RulePort_Duplicate (hPort, &m_hPort);
}

CPortDlg::~CPortDlg ()
{
	RulePort_Delete (m_hPort);
}

void CPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPortDlg)
	DDX_Text(pDX, IDC_SingleValue, m_SinglePortValue);
	DDV_MinMaxUInt(pDX, m_SinglePortValue, 0, 65535);
	DDX_Text(pDX, IDC_RangeLoValue, m_RangeLoValue);
	DDX_Text(pDX, IDC_RangeHiValue, m_RangeHiValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPortDlg, CDialog)
	//{{AFX_MSG_MAP(CPortDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPortDlg message handlers

BOOL CPortDlg::OnInitDialog() 
{
	RulePortType port_type = rptSingle;
	if (RulePort_GetType (m_hPort, &port_type) == srOK)
	{
		if (port_type == rptSingle)
		{
			USHORT val;
			if (RulePort_GetSinglePort (m_hPort, &val) == srOK)
				m_SinglePortValue = val;
		}
		else if (port_type == rptRange)
		{
			USHORT loval, hival;
			if (RulePort_GetPortRange (m_hPort, &loval, &hival) == srOK)
			{
				m_RangeLoValue = loval;
				m_RangeHiValue = hival;
			}
		}
	}

	CDialog::OnInitDialog();
	
	CheckRadioButton (IDC_SinglePort, IDC_PortRange, port_type == rptSingle ? IDC_SinglePort : IDC_PortRange);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPortDlg::OnOK() 
{
	if (!UpdateData ())
		return;

	int nChecked = GetCheckedRadioButton (IDC_SinglePort, IDC_PortRange);

	RulePort_Delete (m_hPort);
	
	if (nChecked == IDC_SinglePort)
	{
		m_hPort = RulePort_Create (rptSingle);
		RulePort_SetSinglePort (m_hPort, m_SinglePortValue);
	}
	else if (nChecked == IDC_PortRange)
	{
		m_hPort = RulePort_Create (rptRange);
		RulePort_SetPortRange (m_hPort, m_RangeLoValue, m_RangeHiValue);
	}
	
	CDialog::OnOK();
}
