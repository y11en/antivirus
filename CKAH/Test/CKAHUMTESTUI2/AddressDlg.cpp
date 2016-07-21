// AddressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "AddressDlg.h"
#include "winsock.h"
#include "NewHostAddrDlg.h"
#include <atlconv.h>
#include <CKAH/ckahiptoa.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace CKAHFW;
using namespace CKAHUM;

/////////////////////////////////////////////////////////////////////////////
// CAddressDlg dialog


CAddressDlg::CAddressDlg(CKAHFW::HRULEADDRESS hAddress, CWnd* pParent /*=NULL*/)
	: CDialog(CAddressDlg::IDD, pParent),
	m_hAddress (NULL)
{
	//{{AFX_DATA_INIT(CAddressDlg)
	m_Name = _T("");
	//}}AFX_DATA_INIT
	RuleAddress_Duplicate (hAddress, &m_hAddress);

	ULONG nSize = RuleAddress_GetName (hAddress, NULL, 0);
	if (nSize)
	{
		WCHAR *pName = new WCHAR[nSize];
		if (pName && RuleAddress_GetName (hAddress, pName, nSize) == nSize - 1)
			m_Name = CString (pName);

		delete []pName;
	}
}

CAddressDlg::~CAddressDlg ()
{
	RuleAddress_Delete (m_hAddress);
}

void CAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddressDlg)
	DDX_Control(pDX, IDC_HostIPList, m_HostIPListCtrl);
	DDX_Control(pDX, IDC_SubnetMask, m_SubnetMaskCtrl);
	DDX_Control(pDX, IDC_SubnetIP, m_SubnetIPCtrl);
	DDX_Control(pDX, IDC_RangeTo, m_HiRangeIPCtrl);
	DDX_Control(pDX, IDC_RangeFrom, m_LoRangeIPCtrl);
	DDX_Text(pDX, IDC_Name, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddressDlg, CDialog)
	//{{AFX_MSG_MAP(CAddressDlg)
	ON_BN_CLICKED(IDC_HostIPListAdd, OnHostIPListAdd)
	ON_BN_CLICKED(IDC_HostIPListClear, OnHostIPListClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressDlg message handlers

BOOL CAddressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RuleAddressType addr_type = ratHost;

	if (RuleAddress_GetType (m_hAddress, &addr_type) == srOK)
	{
		UINT idToCheck = 0;
		if (addr_type == ratHost)
		{
			idToCheck = IDC_HostAddress;
			
			ULONG nIPCount = 0;
			if (RuleAddress_GetAddressHostIPCount (m_hAddress, &nIPCount) == srOK)
			{
				for (ULONG i = 0; i < nIPCount; ++i)
				{
                    CKAHUM::IP ip (0);
					if (RuleAddress_GetAddressHostIPItem (m_hAddress, i, &ip) == srOK)
					{
                        m_HostIPs.Add (ip.v4);
					}
				}
			}

			RefreshHostIPList ();
		}
		else if (addr_type == ratRange)
		{
			idToCheck = IDC_AddressRange;

            CKAHUM::IP loaddr (0), hiaddr (0);
			if (RuleAddress_GetAddressRange (m_hAddress, &loaddr, &hiaddr) == srOK)
			{
                m_LoRangeIPCtrl.SetAddress (loaddr.v4);
				m_HiRangeIPCtrl.SetAddress (hiaddr.v4);
			}			
		}
		else if (addr_type == ratSubnet)
		{
			idToCheck = IDC_AddressSubnet;

            CKAHUM::IP addr (0), mask (0);
			if (RuleAddress_GetAddressSubnet (m_hAddress, &addr, &mask) == srOK)
			{
                m_SubnetIPCtrl.SetAddress (addr.v4);
				m_SubnetMaskCtrl.SetAddress (mask.v4);
			}

		}
		
		CheckRadioButton (IDC_HostAddress, IDC_AddressSubnet, idToCheck);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddressDlg::RefreshHostIPList ()
{
	m_HostIPListCtrl.SetRedraw (FALSE);
	m_HostIPListCtrl.ResetContent ();
	for (int i = 0; i < m_HostIPs.GetSize (); ++i)
	{
		CString strIP;

		in_addr addr;
		addr.s_addr = htonl (m_HostIPs[i]);
		strIP.Format (_T("%hs"), inet_ntoa (addr));

		m_HostIPListCtrl.AddString (strIP);
	}
	m_HostIPListCtrl.SetRedraw (TRUE);
}

void CAddressDlg::OnHostIPListAdd() 
{
	CNewHostAddrDlg NewHostDlg (this);	

	if (NewHostDlg.DoModal () == IDOK)
	{
		m_HostIPs.Append (NewHostDlg.m_IPs);
		m_Name = NewHostDlg.m_HostIPorName;

		RefreshHostIPList ();
		UpdateData (FALSE);
	}
}

void CAddressDlg::OnHostIPListClear() 
{
	m_HostIPs.RemoveAll ();
	RefreshHostIPList ();
}

void CAddressDlg::OnOK() 
{
	if (!UpdateData ())
		return;

	int nChecked = GetCheckedRadioButton (IDC_HostAddress, IDC_AddressSubnet);

	RuleAddress_Delete (m_hAddress);

	switch (nChecked)
	{
	case IDC_HostAddress:
		{
			m_hAddress = RuleAddress_Create (ratHost);

			if (m_HostIPs.GetSize () == 0)
				return;
			for (int i = 0; i < m_HostIPs.GetSize (); ++i)
            {
                CKAHUM::IP ip;
                ip.Setv4(m_HostIPs[i]);
				RuleAddress_AddHostIP (m_hAddress, &ip);
            }
		}
		break;
	case IDC_AddressRange:
		{
			m_hAddress = RuleAddress_Create (ratRange);
		
            CKAHUM::IP loaddr (0), hiaddr (0);
            if (m_LoRangeIPCtrl.GetAddress (loaddr.v4) == 0 || m_HiRangeIPCtrl.GetAddress (hiaddr.v4) == 0)
				return;
			
            m_Name.Format (_T("%s - %s"), CKAHUM_IPTOA(loaddr), CKAHUM_IPTOA(hiaddr));
			RuleAddress_SetAddressRange (m_hAddress, &loaddr, &hiaddr);
		}
		break;
	case IDC_AddressSubnet:
		{
			m_hAddress = RuleAddress_Create (ratSubnet);

            CKAHUM::IP ip (0), mask (0);
            if (m_SubnetIPCtrl.GetAddress (ip.v4) == 0 || m_SubnetMaskCtrl.GetAddress (mask.v4) == 0)
				return;
			

			m_Name.Format (_T("%s"), CKAHUM_IPNETTOA(ip,mask));
			RuleAddress_SetAddressSubnet (m_hAddress, &ip, &mask);
		}
		break;
	default:
		ASSERT (0);
		return;
	}
	
	USES_CONVERSION;
	RuleAddress_SetName (m_hAddress, T2W ((LPTSTR)(LPCTSTR)m_Name));

	CDialog::OnOK();
}
