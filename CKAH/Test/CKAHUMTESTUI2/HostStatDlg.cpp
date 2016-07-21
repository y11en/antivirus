// HostStatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "HostStatDlg.h"
#include <CKAH/ckahiptoa.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHostStatDlg dialog


CHostStatDlg::CHostStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHostStatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHostStatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHostStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostStatDlg)
	DDX_Control(pDX, IDC_HostStat, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHostStatDlg, CDialog)
	//{{AFX_MSG_MAP(CHostStatDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostStatDlg message handlers

BOOL CHostStatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    
    m_List.InsertColumn(1, "Host", LVCFMT_LEFT, 50);
    m_List.InsertColumn(2, "Total Sent", LVCFMT_LEFT, 80);
    m_List.InsertColumn(3, "Total Rcvd", LVCFMT_LEFT, 80);

    m_List.InsertColumn(4,"TCP Sent", LVCFMT_LEFT, 80);
    m_List.InsertColumn(5, "TCP Rcvd", LVCFMT_LEFT, 80);

    m_List.InsertColumn(6, "UDP Sent", LVCFMT_LEFT, 80);
    m_List.InsertColumn(7 ,"UDP Rcvd", LVCFMT_LEFT, 80);

    m_List.InsertColumn(8, "Broadcast Sent", LVCFMT_LEFT, 100);
    m_List.InsertColumn(9, "Broadcast Rcvd", LVCFMT_LEFT, 100);

    FillDlg();

    
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHostStatDlg::FillDlg()
{
    CKAHFW::HHOSTSTATLIST   hlist;
    
    CKAHUM::OpResult or = CKAHFW::GetHostStatistics( &hlist );
    
    if ( or == CKAHUM::srOK )
    {
        int size;
        
        or = CKAHFW::HostStatistics_GetSize(hlist, &size);
        
        for ( int i = 0; i < size; i++ )
        {
			char buffer[100];
			int ii = m_List.InsertItem(0xFFFF, _T("gygy"));

            CKAHFW::HostStatItem item;

            CKAHFW::HostStatistics_GetItem(hlist, i, &item );
            
            m_List.SetItemText( i, 0, CKAHUM_IPTOA( item.HostIp ) ); 
			
			m_List.SetItemText( i, 1, _i64toa(item.TotalStat.outBytes, buffer, 10) ); 
			m_List.SetItemText( i, 2, _i64toa(item.TotalStat.inBytes, buffer, 10) );
			
			m_List.SetItemText( i, 3, _i64toa(item.TcpStat.outBytes, buffer, 10) ); 
			m_List.SetItemText( i, 4, _i64toa(item.TcpStat.inBytes, buffer, 10) ); 

			m_List.SetItemText( i, 5, _i64toa(item.UdpStat.outBytes, buffer, 10) ); 
			m_List.SetItemText( i, 6, _i64toa(item.UdpStat.inBytes, buffer, 10) ); 

			m_List.SetItemText( i, 7, _i64toa(item.BroadcastStat.outBytes, buffer, 10) ); 
			m_List.SetItemText( i, 8, _i64toa(item.BroadcastStat.inBytes, buffer, 10) ); 
			
        }
    }
}
