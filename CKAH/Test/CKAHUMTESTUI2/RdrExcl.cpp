// RdrExcl.cpp : implementation file
//

#include "stdafx.h"
#include "ckahumtestui2.h"
#include "RdrExcl.h"
#include <stuff/StdString.h>
#include <winsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRdrExcl dialog

CRdrExcl::CRdrExcl(CWnd* pParent /*=NULL*/)
	: CDialog(CRdrExcl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRdrExcl)
	m_Host = _T("");
	m_Port = _T("");
	m_Path = _T("");
	//}}AFX_DATA_INIT
}

CRdrExcl::CRdrExcl(std::list<ExcludeItem>* pList, CWnd* pParent /*=NULL*/)
            : CDialog(CRdrExcl::IDD, pParent), m_pCrExludeList(pList) 
{
    //{{AFX_DATA_INIT(CRdrExcl)
	m_Host = _T("");
	m_Port = _T("");
	m_Path = _T("");
	//}}AFX_DATA_INIT
}

void CRdrExcl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRdrExcl)
	DDX_Control(pDX, IDC_REMOVE_EXCLUDE, m_REMOVE_EXCLUDE);
	DDX_Control(pDX, IDC_ADD_EXCLUDE, m_ADD_EXCLUDE);
	DDX_Control(pDX, IDCANCEL, m_CANCEL);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_EXCLUDES_LIST, m_List);
	DDX_Text(pDX, IDC_EDIT_HOST, m_Host);
	DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Text(pDX, IDC_EDIT_PATH, m_Path);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRdrExcl, CDialog)
	//{{AFX_MSG_MAP(CRdrExcl)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADD_EXCLUDE, OnAddExclude)
	ON_BN_CLICKED(IDC_REMOVE_EXCLUDE, OnRemoveExclude)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRdrExcl message handlers

BOOL CRdrExcl::OnInitDialog() 
{
    USES_CONVERSION;

	CDialog::OnInitDialog();
	
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    
    m_List.InsertColumn(1, "Application", LVCFMT_LEFT, 200);
    m_List.InsertColumn(2, "Remote Host", LVCFMT_LEFT, 100);
    m_List.InsertColumn(3, "Remote Port", LVCFMT_LEFT, 100);

    std::list<ExcludeItem>::iterator ii;
    char    buffer[10];
    int i;

    for ( i = 0, ii = m_pCrExludeList->begin(); ii != m_pCrExludeList->end(); i++,ii++ )
    {
        int item = m_List.InsertItem(0xFFFF, _T("gygy"));
        
        m_List.SetItemText( i, 0,  W2A(ii->m_Path) );
        
        //itoa(ii->m_RemoteHost, buffer, 10 );
         
        m_List.SetItemText( i, 1,  inet_ntoa(*(in_addr*)&ii->m_RemoteHost) );
        
        _itoa(ii->m_RemotePort, buffer, 10 );
        m_List.SetItemText( i, 2,  buffer );
    }
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRdrExcl::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    if ( m_List.m_hWnd )    
        m_List.MoveWindow( 0, 0, cx - 87 , cy-8, TRUE );
	
    if ( m_OK.m_hWnd )
        m_OK.MoveWindow             ( cx - 79, 0,   75, 23 );

    if ( m_CANCEL.m_hWnd )
        m_CANCEL.MoveWindow         ( cx - 79, 33,  75, 23 );

    if ( m_ADD_EXCLUDE.m_hWnd )
        m_ADD_EXCLUDE.MoveWindow    ( cx - 79, 66,  75, 23 );

    if ( m_REMOVE_EXCLUDE.m_hWnd )
        m_REMOVE_EXCLUDE.MoveWindow ( cx - 79, 99,  75, 23 );
}

void CRdrExcl::OnAddExclude() 
{
	// TODO: Add your control notification handler code here
    USES_CONVERSION;

    UpdateData ();

    LPHOSTENT	lpHostEntry = gethostbyname( T2A((LPTSTR)(LPCTSTR)m_Host) );
    if ( lpHostEntry == NULL )
    {
        if ( m_Host[0] != '0' )
            return;
    }
    ULONG   Ip = lpHostEntry ? ((LPIN_ADDR)*lpHostEntry->h_addr_list)->S_un.S_addr : 0;
    
    int count = m_List.GetItemCount();
    int item = m_List.InsertItem(0xFFFF, _T("gygy"));

    m_List.SetItemText( count, 0,  T2A((LPTSTR)(LPCTSTR)m_Path) );
    m_List.SetItemText( count, 1,  T2A((LPTSTR)(LPCTSTR)m_Host) );
    m_List.SetItemText( count, 2,  T2A((LPTSTR)(LPCTSTR)m_Port) );

    ExcludeItem Item;
    
    wcscpy ( Item.m_Path, T2W((LPTSTR)(LPCTSTR)m_Path) );
    
    Item.m_Handle       = NULL;
    Item.m_RemotePort   = htons ( atoi(T2A((LPTSTR)(LPCTSTR)m_Port)) );    
    Item.m_RemoteHost   = Ip;    

    m_pCrExludeList->push_back( Item );
}

void CRdrExcl::OnRemoveExclude() 
{
	// TODO: Add your control notification handler code here

    int CurSel = m_List.GetSelectionMark();
    int i = 0;
    
    m_List.DeleteItem( CurSel );
    
    std::list<ExcludeItem>::iterator ii;
    for ( i = 0, ii = m_pCrExludeList->begin(); ii != m_pCrExludeList->end(); i++,ii++ )
    {
        if ( i == CurSel )
        {
            m_pCrExludeList->erase(ii);
            break;
        }
    }
}
