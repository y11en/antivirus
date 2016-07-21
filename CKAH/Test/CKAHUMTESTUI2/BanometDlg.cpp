// BanometDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "BanometDlg.h"
#include <CKAH/ckahiptoa.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBanometDlg dialog


CBanometDlg::CBanometDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBanometDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBanometDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBanometDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBanometDlg)
	DDX_Control(pDX, IDC_BANNED_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBanometDlg, CDialog)
	//{{AFX_MSG_MAP(CBanometDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBanometDlg message handlers

char* Month[] = {
    "January",
    "Febrary",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

BOOL CBanometDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    m_List.InsertColumn(1, "Host", LVCFMT_LEFT, 100);
    m_List.InsertColumn(2, "Banned from", LVCFMT_LEFT, 200);

    CKAHIDS::HBANNEDLIST  hList;
    CKAHUM::OpResult result = CKAHIDS::GetBannedList(&hList);

    if ( result == CKAHUM::srOK)
        FillDlg(hList);
	
	return TRUE;
}

void CBanometDlg::FillDlg(CKAHIDS::HBANNEDLIST  hList)
{
    int size = 0;

    CKAHUM::OpResult result = CKAHIDS::BannedList_GetSize (hList, &size);    

    if ( size )
    {
        for ( int i = 0; i < size; i++ )
        {
			int item = m_List.InsertItem(0xFFFF, _T("gygy"));

            CKAHIDS::BannedHost  host;
            CKAHIDS::BannedList_GetItem(hList, i, &host);

            m_List.SetItemText( i, 0, CKAHUM_IPTOA( host.IP ) );

            SYSTEMTIME st;
            FileTimeToSystemTime( (FILETIME*)&host.BanTime, &st );
            char    Time[50];
            _snprintf (Time, 50, "%d:%d:%d  %s, %d\0", st.wHour, st.wMinute, st.wSecond , Month[st.wMonth],st.wDay );
            
            m_List.SetItemText( i, 1, Time );
        }
    }
}
