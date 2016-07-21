// CRConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "CRConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCRConsoleDlg dialog

DWORD WINAPI CCRConsoleDlg::SrvRecvThreadProc(LPVOID lpParameter)
{
    CCRConsoleDlg * Dlg = (CCRConsoleDlg *)lpParameter;

    SOCKET csocket = Dlg->m_csocket;
    SOCKET tsocket = Dlg->m_tsocket;

    char c;
    while (recv(tsocket, &c, sizeof(char), 0) == 1)
    {
        Dlg->SrvChar(c);
    }

    Dlg->m_tsocket = INVALID_SOCKET;
    closesocket(tsocket);

    Dlg->m_csocket = INVALID_SOCKET;
    closesocket(csocket);

    Dlg->m_connected = false;

    return TRUE;
}


DWORD WINAPI CCRConsoleDlg::CliRecvThreadProc(LPVOID lpParameter)
{
    CCRConsoleDlg * Dlg = (CCRConsoleDlg *)lpParameter;

    //sockaddr_in addr;
    //int size;

    SOCKET csocket = Dlg->m_csocket = accept(Dlg->m_lsocket, 0/*(sockaddr*)&addr*/, 0/*&size*/);

    if (csocket == INVALID_SOCKET)
    {
        int err = WSAGetLastError();
        return FALSE;
    }

    Dlg->m_connected = true;

    char init[500];
    int size = 0, r = 0;
    while( size != sizeof(init) && (r = recv(csocket, init+size, sizeof(init)-size, 0)) != 0)
    {
        size += r;
    }

    if (size != sizeof(init))
    {
        closesocket(csocket);
        Dlg->m_csocket = INVALID_SOCKET;

        Dlg->m_connected = false;

        return TRUE;
    }

    int ip[4];
    unsigned int tport, mport;
    unsigned int frompid;

    sscanf(init, "KAVSEND <%d.%d.%d.%d> <%d> <%d> <%d>", &ip[0],&ip[1],&ip[2],&ip[3], &tport, &frompid, &mport);

    SOCKET tsocket = Dlg->m_tsocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in taddr;

    taddr.sin_family        = AF_INET;
    taddr.sin_addr.s_addr   = ip[0] | (ip[1]<<8) | (ip[2]<<16) | (ip [3]<<24);
    taddr.sin_port          = htons(mport);

    if ( connect(tsocket, (struct sockaddr*)&taddr, sizeof(taddr)) == SOCKET_ERROR)
    {
        closesocket(csocket);
        Dlg->m_csocket = INVALID_SOCKET;
        closesocket(tsocket);
        Dlg->m_tsocket = INVALID_SOCKET;

        Dlg->m_connected = false;

        return TRUE;
    }

    Dlg->m_hsthread = CreateThread(NULL, 0, SrvRecvThreadProc, lpParameter, 0, NULL);

    char c;
    while (recv(csocket, &c, sizeof(char), 0) == 1)
    {
        Dlg->CliChar(c);
    }

    Dlg->m_csocket = INVALID_SOCKET;
    closesocket(csocket);

    Dlg->m_tsocket = INVALID_SOCKET;
    closesocket(tsocket);

    Dlg->m_connected = false;

    return TRUE;
}




CCRConsoleDlg::CCRConsoleDlg(std::list<ExcludeItem>* pList, USHORT lport, USHORT rport, CWnd* pParent /* = NULL */)
	: CDialog(CCRConsoleDlg::IDD, pParent),
    m_pCrExludeList(pList),
	m_rport(rport),
	m_lport(lport),
    m_lsocket(INVALID_SOCKET),
    m_csocket(INVALID_SOCKET),
    m_connected(false),
    m_hcthread(INVALID_HANDLE_VALUE),
    m_hsthread(INVALID_HANDLE_VALUE),
    m_hproxy(0)
{
	//{{AFX_DATA_INIT(CCRConsoleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_lsocket = socket( AF_INET, SOCK_STREAM, 0 );

    if (m_lsocket== INVALID_SOCKET)
    {
        m_status = false;
        return;
    }

    const int   on      = 1;
    if  ( setsockopt ( m_lsocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof (on)) )
    {
        closesocket(m_lsocket);
        m_lsocket = INVALID_SOCKET;
        m_status = false;
        return;
    }

    sockaddr_in local;

    local.sin_family        = AF_INET;
    local.sin_addr.s_addr   = htonl ( INADDR_ANY );
    local.sin_port          = htons(lport);

    if  ( bind( m_lsocket,(struct sockaddr*)&local,sizeof(local) ) == SOCKET_ERROR )
    {
        closesocket(m_lsocket);
        m_lsocket = INVALID_SOCKET;
        m_status = false;
        return;
    }

    if (listen(m_lsocket, 1) != 0)
    {
        closesocket(m_lsocket);
        m_lsocket = INVALID_SOCKET;
        m_status = false;
        return;
    }

    if ( CKAHCR::RegisterProxy (m_lport, GetCurrentProcessId(), &m_hproxy) != CKAHUM::srOK)
    {
        closesocket(m_lsocket);
        m_lsocket = INVALID_SOCKET;
        m_status = false;
        return;
    }

    if ( CKAHCR::Proxy_AddPort (m_hproxy, m_rport) != CKAHUM::srOK)
    {
        CKAHCR::Proxy_Unregister(m_hproxy);
        m_hproxy = 0;
        
        closesocket(m_lsocket);
        m_lsocket = INVALID_SOCKET;
        m_status = false;
        return;
    }
    
    std::list<ExcludeItem>::iterator ii;

    for ( ii = m_pCrExludeList->begin(); ii != m_pCrExludeList->end(); ii++ )
    {        
        CKAHCR::Proxy_AddExclude( ii->m_RemoteHost, ii->m_RemotePort, ii->m_Path, wcslen(ii->m_Path) * 2 + 2,  &ii->m_Handle);
    }

    m_hcthread = CreateThread(NULL, 0, CliRecvThreadProc, this, 0, NULL);
}

CCRConsoleDlg::~CCRConsoleDlg()
{
    if (m_lsocket != INVALID_SOCKET) closesocket(m_lsocket);
    if (m_csocket != INVALID_SOCKET) closesocket(m_csocket);
    if (m_tsocket != INVALID_SOCKET) closesocket(m_tsocket);

    if (m_hcthread != INVALID_HANDLE_VALUE)
    {
        TerminateThread(m_hcthread, 0);
    }
    if (m_hsthread != INVALID_HANDLE_VALUE)
    {
        TerminateThread(m_hsthread, 0);
    }

    if (m_hproxy)
    {
        CKAHCR::Proxy_Unregister(m_hproxy);
        m_hproxy = 0;
    }
}

void CCRConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCRConsoleDlg)
	DDX_Control(pDX, IDC_Console, m_EdtConsole);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCRConsoleDlg, CDialog)
	//{{AFX_MSG_MAP(CCRConsoleDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCRConsoleDlg message handlers

void CCRConsoleDlg::PostNcDestroy() 
{
	delete this;
}

void CCRConsoleDlg::OnOK() 
{
	DestroyWindow();
}

BOOL CCRConsoleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_Console)->SetFocus();

	CString str;
	str.Format("Connection Redirector Console on Port %d", m_rport);

	SetWindowText(str);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CCRConsoleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

//	if(pWnd->GetSafeHwnd() == m_EdtConsole.GetSafeHwnd())
//		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	
	return hbr;
}

void CCRConsoleDlg::CliChar(UINT nChar) 
{
    if (m_connected)
    {
        char c = nChar;
        send(m_tsocket, &c, sizeof(c), 0);

        OutChar(nChar);
    }
}

void CCRConsoleDlg::SrvChar(UINT nChar) 
{
    if (m_connected)
    {
        char c = nChar;
        send(m_csocket, &c, sizeof(c), 0);

        OutChar(nChar);
    }
}

void CCRConsoleDlg::OutChar(UINT nChar) 
{
    char chstr[2] = "\0";
    chstr[0] = nChar;

    m_strText += chstr;

    m_EdtConsole.SetWindowText(m_strText);

    int len = m_strText.GetLength();
    m_EdtConsole.SetSel(len, -1);
}
