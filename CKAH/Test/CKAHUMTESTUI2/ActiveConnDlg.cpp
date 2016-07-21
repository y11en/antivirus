// ActiveConnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "ActiveConnDlg.h"
#include "winsock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActiveConnDlg dialog


CActiveConnDlg::CActiveConnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActiveConnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CActiveConnDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CActiveConnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActiveConnDlg)
	DDX_Control(pDX, IDCANCEL, m_CANCEL);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_OpenConnList, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActiveConnDlg, CDialog)
	//{{AFX_MSG_MAP(CActiveConnDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActiveConnDlg message handlers

BOOL CActiveConnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    m_List.InsertColumn(1, "PID",           LVCFMT_LEFT, 50);
    m_List.InsertColumn(2, "ProcessName",   LVCFMT_LEFT, 100);
    m_List.InsertColumn(3, "ImageName",     LVCFMT_LEFT, 100);
    m_List.InsertColumn(4, "CommandLine",   LVCFMT_LEFT, 100);
    m_List.InsertColumn(5, "Sent",   LVCFMT_LEFT, 100);
    m_List.InsertColumn(6, "Rcvd",   LVCFMT_LEFT, 100);

	FillDlg();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CActiveConnDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    if ( m_List.m_hWnd )    
        m_List.MoveWindow( 0, 0, cx - 87 , cy-8, TRUE );
	
    if ( m_OK.m_hWnd )
        m_OK.MoveWindow( cx - 79, 0, 75, 23 );

    if ( m_CANCEL.m_hWnd )
        m_CANCEL.MoveWindow( cx - 79, 33, 75, 23 );
	
}

VOID CActiveConnDlg::FillDlg()
{
    CKAHFW::HCONNLIST hConnectionList;

    CKAHUM::OpResult result = CKAHFW::GetConnectionList (&hConnectionList);

    if (result == CKAHUM::srOK)
    {
        int size;

		if ((result = CKAHFW::ConnectionList_GetSize (hConnectionList, &size)) == CKAHUM::srOK)
		{
            for (int j = 0; j < size; ++j)
            {
                CKAHFW::Connection Connection;
                
                if ((result = CKAHFW::ConnectionList_GetItem (hConnectionList, j, &Connection)) == CKAHUM::srOK)
                {
                    char buffer[MAX_PATH];
                    //in_addr addr_loc, addr_rem;
                    //addr_loc.s_addr = htonl (Connection.LocalIP);
                    //addr_rem.s_addr = htonl (Connection.RemoteIP);
                    
                    int item = m_List.InsertItem(0xFFFF, _T("gygy"));

                    m_List.SetItemText( j, 0,  _itoa( Connection.PID, buffer, 10) );
					m_List.SetItemText( j, 1,  "ProcessName" );
					
					_snprintf (buffer, MAX_PATH, "%ls", Connection.AppName );
					m_List.SetItemText( j, 2,  buffer );
					
                }
            }
        }
    }
}
