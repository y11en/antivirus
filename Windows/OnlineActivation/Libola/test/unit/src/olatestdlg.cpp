// olatestDlg.cpp : implementation file
//

#include "stdafx.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <wininet.h>

#include "olatest.h"
#include "olatestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COlaTestDlg dialog

COlaTestDlg::COlaTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COlaTestDlg::IDD, pParent)
	, m_observer(this)
	, m_browser(this)
	, m_appId(0)
	, m_suppId(0)
	, m_actType(ACTIVATE_NEW)
	, m_proxy(_T("proxy.avp.ru:3128"))
	, m_user(_T("kl\\tester"))
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COlaTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_PROGRESS, m_progress);
	DDX_Text(pDX, IDC_EDIT_APPID, m_appId);
	DDX_Text(pDX, IDC_EDIT_SUPP, m_suppId);
	DDX_Text(pDX, IDC_EDIT_PASS, m_password);
	DDV_MaxChars(pDX, m_password, 8);
	DDX_Text(pDX, IDC_EDIT_CODE, m_actCode);
	DDX_Radio(pDX, IDC_RADIO_ACTYPE, reinterpret_cast<int&>(m_actType));
	DDX_Control(pDX, IDC_LIST_URLS, m_urls);
	DDX_Text(pDX, IDC_EDIT_PROXY, m_proxy);
	DDX_Text(pDX, IDC_EDIT_USER, m_user);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_pwd);
}

BEGIN_MESSAGE_MAP(COlaTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SUBMIT, OnButtonSubmit)
	ON_BN_CLICKED(IDC_BUTTON_ABORT, OnButtonAbort)
	ON_MESSAGE(WM_REQ_COMPLETED, OnRequestCompleted)
	ON_BN_CLICKED(IDC_BUTTON_REPLY, &COlaTestDlg::OnBnClickedButtonReply)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_URLS, &COlaTestDlg::OnLvnKeydownListUrls)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_URLS, &COlaTestDlg::OnLvnEndlabeleditListUrls)
	ON_BN_CLICKED(IDC_CHECK_PROXY, &COlaTestDlg::OnBnClickedCheckProxy)
	ON_BN_CLICKED(IDC_CHECK_IE, &COlaTestDlg::OnBnClickedCheckIe)
	ON_BN_CLICKED(IDC_CHECK_AUTH, &COlaTestDlg::OnBnClickedCheckAuth)
	ON_BN_CLICKED(IDC_RADIO_ACTYPE, &COlaTestDlg::OnBnClickedRadioActype)
	ON_BN_CLICKED(IDC_RADIO_REGISTERED, &COlaTestDlg::OnBnClickedRadioRegistered)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COlaTestDlg message handlers

BOOL COlaTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// set buttons state
	GetDlgItem(IDC_BUTTON_SUBMIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_REPLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(FALSE);
	// populate activation URL list
	CRect rect;
	m_urls.GetClientRect(&rect);
	m_urls.SetExtendedStyle(m_urls.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	m_urls.InsertColumn(0, _T("Activation URL List"), LVCFMT_LEFT, rect.Width());
	m_urls.InsertItem(0, _T("https://auto-activation1.kaspersky.com/activate"));
	m_urls.SetCheck(0);
	m_urls.InsertItem(1, _T("https://auto-activation2.kaspersky.com/activate"));
	m_urls.SetCheck(1);
	m_urls.InsertItem(2, _T("https://auto-activation3.kaspersky.com/activate"));
	m_urls.SetCheck(2);
	m_urls.InsertItem(3, _T("https://auto-activation4.kaspersky.com/activate"));
	m_urls.SetCheck(3);
	m_urls.InsertItem(4, _T("https://auto-activation5.kaspersky.com/activate"));
	m_urls.SetCheck(4);
	m_urls.InsertItem(5, _T("https://auto-activation6.kaspersky.com/activate"));
	m_urls.SetCheck(5);
	// set proxy checks
	GetDlgItem(IDC_CHECK_PROXY)->SendMessage(BM_SETCHECK, 1, 0);
	GetDlgItem(IDC_CHECK_IE)->SendMessage(BM_SETCHECK, 1, 0);
	GetDlgItem(IDC_CHECK_AUTH)->SendMessage(BM_SETCHECK, 0, 0);
	OnBnClickedCheckProxy();
	OnBnClickedRadioActype();

	VERIFY(m_browser.CreateFromStatic(IDC_BROWSER, this));
	// ensures MSHTML is loaded
	m_browser.Navigate2(_T("about:blank"), NULL, NULL, NULL, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COlaTestDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COlaTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT COlaTestDlg::OnRequestCompleted(WPARAM wParam, LPARAM lParam)
{
	using namespace OnlineActivation;
	try
	{
		// get content object
		boost::shared_ptr<Content> pContent(m_pResponse->GetContent());
		// analyze content type
		switch(pContent->GetType())
		{
		case Content::CONTENT_HTML:
			m_browser.LoadFromBuffer(static_cast<const char*>(pContent->GetData()), pContent->GetDataSize());
			GetDlgItem(IDC_BUTTON_REPLY)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(TRUE);
			break;
		case Content::CONTENT_BINARY:
			{
				CFileDialog fd(FALSE);
				if (fd.DoModal() == IDOK)
				{
					std::ofstream os(fd.GetPathName(), std::ios::binary);
					os.write(static_cast<const char*>(pContent->GetData()), pContent->GetDataSize());
				}
			}
			GetDlgItem(IDC_BUTTON_SUBMIT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(FALSE);
			break;
		default:
			_ASSERTE(!"Incorrect content type");
			break;
		}

	}
	catch(const ActivationError& e)
	{
		GetDlgItem(IDC_BUTTON_SUBMIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(FALSE);
		WriteErrorMsg(e);
	}
	return S_OK;
}

void COlaTestDlg::WriteProgressMsg(LPCTSTR szProgress)
{
	m_progress.AddString(szProgress);
}

void COlaTestDlg::WriteErrorMsg(const OnlineActivation::ActivationError& error)
{
	std::basic_ostringstream<TCHAR> os;

	os	<< _T("Activation error occured. Type: ") << error.GetErrorType()
		<< _T(", Code: ") << error.GetErrorCode();

	m_progress.AddString(os.str().c_str());
}

void COlaTestDlg::OnButtonSubmit() 
{
	using namespace OnlineActivation;
	// update dialog data
	UpdateData(TRUE);
	// initialize browser object
	m_browser.Navigate2(_T("about:blank"), NULL, NULL, NULL, NULL);
	// clear progress content
	m_progress.ResetContent();

	try
	{
		// prepare activation URL list
		std::vector<std::string> urlbuf;
		for (int i = 0; i < m_urls.GetItemCount(); ++i)
		{
			if (m_urls.GetCheck(i))
				urlbuf.push_back(m_urls.GetItemText(i, 0).GetString());
		}
		std::vector<const char*> urls, proxies;
		for (std::vector<std::string>::const_iterator it = urlbuf.begin(); it != urlbuf.end(); ++it)
			urls.push_back(it->c_str());
		urls.push_back(0);
		ActivationConfig cfg = { 0 };
		cfg.url_list = &urls[0];

		// set proxy configuration
		if (GetDlgItem(IDC_CHECK_PROXY)->SendMessage(BM_GETCHECK, 0, 0))
		{
			if (GetDlgItem(IDC_CHECK_IE)->SendMessage(BM_GETCHECK, 0, 0))
			{
				cfg.inet_config.access_type = InternetConfig::PRECONFIG;
			}
			else
			{
				cfg.inet_config.access_type = InternetConfig::PROXY;
				proxies.push_back(m_proxy);
				proxies.push_back(0);
				cfg.inet_config.proxy_list = &proxies[0];
			}
		}
		else
		{
			cfg.inet_config.access_type = InternetConfig::DIRECT;
		}
		boost::shared_ptr<ActivationFactory> pFactory(CreateActivationFactory(cfg));
		// prepare activation data
		ApplicationId appId = { 0 };
		appId.id = m_appId;
		// compose activation code
		ActivationCode actCode = { 0 };
		if (sscanf(m_actCode, "%5c-%5c-%5c-%5c", actCode.part1, actCode.part2, actCode.part3, actCode.part4) != 4)
		{
			MessageBox("Invalid activation code format.", 0, MB_OK | MB_ICONERROR);
			return;
		}
		CustomerId custId = { 0 };
		custId.id = m_suppId;
		strncpy(custId.password, m_password.GetString(), 8);

		boost::shared_ptr<Request> pRequest;

		switch(m_actType)
		{
		case ACTIVATE_NEW:
			pRequest = pFactory->ComposeRequest(appId, actCode);
			break;
		case ACTIVATE_EXISTING:
			pRequest = pFactory->ComposeRequest(appId, actCode, custId);
			break;
		default:
			_ASSERTE(!"Unknown activation type");
			break;
		}
		// submit request
		m_pResponse = pRequest->Submit(m_observer);

		GetDlgItem(IDC_BUTTON_SUBMIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(TRUE);
	}
	catch(const OnlineActivation::ActivationError& e)
	{
		WriteErrorMsg(e);
	}
}

void COlaTestDlg::PostNcDestroy() 
{
	m_pResponse.reset();
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
}

void COlaTestDlg::OnButtonAbort() 
{
	// abort reply case
	if (GetDlgItem(IDC_BUTTON_REPLY)->IsWindowEnabled())
	{
		GetDlgItem(IDC_BUTTON_SUBMIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REPLY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(FALSE);
	}
	// request in progress case
	else if (m_pResponse)
		m_pResponse->Cancel();
}

void COlaTestDlg::PostReply(LPCTSTR szHeaders, PBYTE pbData, DWORD dwSize)
{
	_ASSERTE(m_pResponse && "Response must be already set");
	boost::shared_ptr<OnlineActivation::Request> pRequest
		(m_pResponse->ComposeReply(szHeaders, strlen(szHeaders), pbData, dwSize));
	m_browser.Navigate2(_T("about:blank"), NULL, NULL, NULL, NULL);
	m_pResponse = pRequest->Submit(m_observer);
}

void COlaTestDlg::OnBnClickedButtonReply()
{
	// TODO: Add your control notification handler code here
	m_browser.Submit();
}

void COlaTestDlg::HandleRequestStarted()
{
	GetDlgItem(IDC_BUTTON_SUBMIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REPLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ABORT)->EnableWindow(TRUE);
}

void COlaTestDlg::HandleRequestFinished(bool bSucceeded)
{
	PostMessage(WM_REQ_COMPLETED, 0 , 0);
}

void COlaTestDlg::OnLvnKeydownListUrls(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	switch (pLVKeyDow->wVKey)
	{
	case VK_INSERT:
		{
			int i = m_urls.InsertItem(m_urls.GetItemCount(), _T(""));
			if (i >= 0)
			{
				m_urls.SetCheck(i);
				m_urls.EditLabel(i);
			}
		}
		break;
	case VK_DELETE:
		{
			int i = m_urls.GetSelectionMark();
			if (i >= 0)
				m_urls.DeleteItem(i);
		}
		break;
	}
	*pResult = 0;
}

void COlaTestDlg::OnLvnEndlabeleditListUrls(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	m_urls.SetItem(&pDispInfo->item);
	m_urls.SetSelectionMark(pDispInfo->item.iItem);
	*pResult = 0;
}

void COlaTestDlg::OnBnClickedCheckProxy()
{
	if (GetDlgItem(IDC_CHECK_PROXY)->SendMessage(BM_GETCHECK, 0, 0))
	{
		GetDlgItem(IDC_CHECK_IE)->EnableWindow();
		GetDlgItem(IDC_CHECK_AUTH)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_CHECK_IE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_AUTH)->EnableWindow(FALSE);
	}
	OnBnClickedCheckIe();
	OnBnClickedCheckAuth();
}

void COlaTestDlg::OnBnClickedCheckIe()
{
	GetDlgItem(IDC_EDIT_PROXY)->EnableWindow
		(GetDlgItem(IDC_CHECK_PROXY)->SendMessage(BM_GETCHECK, 0, 0) && 
		!GetDlgItem(IDC_CHECK_IE)->SendMessage(BM_GETCHECK, 0, 0));
}

void COlaTestDlg::OnBnClickedCheckAuth()
{
	if (GetDlgItem(IDC_CHECK_PROXY)->SendMessage(BM_GETCHECK, 0, 0) &&
		GetDlgItem(IDC_CHECK_AUTH)->SendMessage(BM_GETCHECK, 0, 0))
	{
		GetDlgItem(IDC_EDIT_USER)->EnableWindow();
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_EDIT_USER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
	}
}

void COlaTestDlg::OnBnClickedRadioActype()
{
	GetDlgItem(IDC_EDIT_SUPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(FALSE);
}

void COlaTestDlg::OnBnClickedRadioRegistered()
{
	GetDlgItem(IDC_EDIT_SUPP)->EnableWindow();
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow();
}

bool COlaTestDlg::GetProxyAuth(std::string& user, std::string& pass) const
{
	if (GetDlgItem(IDC_CHECK_AUTH)->SendMessage(BM_GETCHECK, 0, 0))
	{
		user = m_user;
		pass = m_pwd;
		return true;
	}
	return false;
}
