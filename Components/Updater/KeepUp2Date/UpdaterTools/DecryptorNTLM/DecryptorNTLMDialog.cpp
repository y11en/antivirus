#include "stdafx.h"
#include "DecryptorNTLM.h"
#include "DecryptorNTLMDialog.h"

#include "NtlmMessages.h"

#include "helper/base64.h" // from Updater library


#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif


DecryptorNTLMDialog::DecryptorNTLMDialog(CWnd *pParent)
	: CDialog(DecryptorNTLMDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DecryptorNTLMDialog)
	m_input1 = _T("");
	m_input2 = _T("");
	m_input3 = _T("");
	m_input4 = _T("");
	m_output1 = _T("");
	m_output2 = _T("");
	m_output3 = _T("");
	m_output4 = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void DecryptorNTLMDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DecryptorNTLMDialog)
	DDX_Text(pDX, IDC_INPUT1, m_input1);
	DDX_Text(pDX, IDC_INPUT2, m_input2);
	DDX_Text(pDX, IDC_INPUT3, m_input3);
	DDX_Text(pDX, IDC_INPUT4, m_input4);
	DDX_Text(pDX, IDC_OUTPUT1, m_output1);
	DDX_Text(pDX, IDC_OUTPUT2, m_output2);
	DDX_Text(pDX, IDC_OUTPUT3, m_output3);
	DDX_Text(pDX, IDC_OUTPUT4, m_output4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DecryptorNTLMDialog, CDialog)
	//{{AFX_MSG_MAP(DecryptorNTLMDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DECRYPT, OnButtonDecrypt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL DecryptorNTLMDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void DecryptorNTLMDialog::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR DecryptorNTLMDialog::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void DecryptorNTLMDialog::OnButtonDecrypt() 
{
     CWnd::UpdateData(TRUE);
     // decoding Ntlm message type 1
     if(!m_input1.IsEmpty())
     {
        const std::vector<unsigned char> buffer = CBase64::decodeBuffer(m_input1.GetBuffer(0));
        if(!buffer.empty() && (buffer.size() < sizeof(Type1Message)))
        {
            Type1Message message;
            memcpy(&message, &buffer[0], buffer.size());
            m_output1 = message.toString();
        }
     }

     // decoding Ntlm message type 2
     if(!m_input2.IsEmpty())
     {
        const std::vector<unsigned char> buffer = CBase64::decodeBuffer(m_input2.GetBuffer(0));
        if(!buffer.empty() && (buffer.size() < sizeof(Type2Message)))
        {
            Type2Message message;
            memcpy(&message, &buffer[0], buffer.size());
            m_output2 = message.toString();
        }
     }

     // decoding Ntlm message type 3
     if(!m_input3.IsEmpty())
     {
        const std::vector<unsigned char> buffer = CBase64::decodeBuffer(m_input3.GetBuffer(0));
        if(!buffer.empty() && (buffer.size() < sizeof(Type3Message)))
        {
            Type3Message message;
            memcpy(&message, &buffer[0], buffer.size());
            m_output3 = message.toString();
        }
     }

     // decoding Basic Authentication Credentials
     if(!m_input4.IsEmpty())
     {
        const std::vector<unsigned char> buffer = CBase64::decodeBuffer(m_input4.GetBuffer(0));
        if(!buffer.empty())
            memcpy(m_output4.GetBufferSetLength(buffer.size()), reinterpret_cast<const char *>(&buffer[0]), buffer.size());
     }

     CWnd::UpdateData(FALSE);
}

