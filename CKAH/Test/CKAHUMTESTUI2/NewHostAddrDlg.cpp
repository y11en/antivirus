// NewHostAddrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "NewHostAddrDlg.h"
#include <winsock.h>
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewHostAddrDlg dialog


CNewHostAddrDlg::CNewHostAddrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewHostAddrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewHostAddrDlg)
	m_HostIPorName = _T("");
	//}}AFX_DATA_INIT
}


void CNewHostAddrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewHostAddrDlg)
	DDX_Text(pDX, IDC_HostIPorName, m_HostIPorName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewHostAddrDlg, CDialog)
	//{{AFX_MSG_MAP(CNewHostAddrDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewHostAddrDlg message handlers

void CNewHostAddrDlg::OnOK() 
{
	if (!UpdateData () || m_HostIPorName.IsEmpty ())
		return;

	USES_CONVERSION;

	unsigned int addr = inet_addr (T2A ((LPTSTR)(LPCTSTR)m_HostIPorName));

	if (addr == INADDR_NONE)
	{
		struct hostent *hp = gethostbyname (T2A ((LPTSTR)(LPCTSTR)m_HostIPorName));

        if (hp == NULL)
		{
			AfxMessageBox (IDS_FAILEDTORESOLVEHOST, MB_OK | MB_ICONSTOP);
			return;
		}

		for (int i = 0; hp->h_addr_list[i]; ++i)
			m_IPs.Add (htonl (*(DWORD *)hp->h_addr_list[i]));
	}
	else
	{
		m_IPs.Add (htonl (addr));
	}
	
	CDialog::OnOK();
}
