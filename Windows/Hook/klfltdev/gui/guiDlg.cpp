// guiDlg.cpp : implementation file
//
#include "stdafx.h"
#include "gui.h"
#include "guiDlg.h"
#include "winioctl.h"
#include "../ioctl.h"
#include "../../mklif/inc/hips_tools.h"
#include "GuidConst.h"
#include "control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DENY L"Deny"
#define ALLOW L"Allow"
#define DISABLE L"Disable"


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CguiDlg dialog



LONG gMemCounter = 0;

BOOL gbStop = FALSE;

void* __cdecl pfMemAlloc (
						  PVOID pOpaquePtr,
						  size_t size,
						  ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );

	return ptr;
};

void __cdecl pfMemFree (
						PVOID pOpaquePtr,
						void** pptr)
{
	if (!*pptr)
		return;

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};

DWORD
WINAPI
WorkingThread (
			   void* pParam
			   )
{
	HRESULT hResult = S_OK;
	PVOID pClientContext = pParam;

	PVOID pMessage;
	PMKLIF_EVENT_HDR pEventHdr;
	ULONG MessageSize;
	MKLIF_REPLY_EVENT Verdict;


	hResult = MKL_BuildMultipleWait( pClientContext, 1 );

	if ( SUCCEEDED( hResult ) )
		while(TRUE)
		{
			hResult = MKL_GetSingleMessage( pClientContext, &pMessage, &MessageSize, &pEventHdr, 1000, &gbStop );

			if (!SUCCEEDED( hResult) )
			{
				printf( "thread get message failed hresult 0x%x\n", hResult );
				break;
			}
			else
			{
				ULONG wcFileNameSize = 0;
				ULONG wcFullFileNameSize = 0;
				ULONG wcVirtFileNameSize = 0;
				ULONG wcVirtFullFileNameSize = 0;

				memset( &Verdict, 0, sizeof(Verdict) );
				Verdict.m_VerdictFlags = efVerdict_Default;
				Verdict.m_ExpTime = 0;

				PSINGLE_PARAM pGuid = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_URL_W, FALSE );
				PSINGLE_PARAM pDeviceType = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_VOLUME_NAME_W, FALSE );
				PSINGLE_PARAM pAccessMask = MKL_GetEventParam( pMessage, MessageSize, _PARAM_OBJECT_ACCESSATTR, FALSE );

				hResult = MKL_ReplyMessage( pClientContext, pMessage, &Verdict );

				ULONG mask = *(ULONG*)(pAccessMask->ParamValue);
				BOOLEAN isAllow = ( HIPS_GET_AM ( mask, HIPS_POS_KLFLTDEV ) == HIPS_FLAG_ALLOW ) ? TRUE : FALSE;
				BOOLEAN needLog = ( HIPS_GET_LOG ( mask, HIPS_POS_KLFLTDEV ) == HIPS_LOG_ON ) ? TRUE : FALSE;

				WCHAR str[256];

				wsprintfW ( str,L" %s - %s \n mask = %d (isAllow - %s needLog - %s)", 
					pGuid->ParamValue,
					pDeviceType->ParamValue,
					mask,
					( isAllow == TRUE )? L"TRUE" : L"FALSE",
					( needLog == TRUE )? L"TRUE" : L"FALSE"
					);

				MessageBoxW( NULL,str,L"LOG",MB_OK );
				
				MKL_FreeSingleMessage( pClientContext, &pMessage );
			}
		}

// 		if ( pClientContext )
// 		{
// 			MKL_ChangeClientActiveStatus( pClientContext, FALSE );
// 			MKL_DelAllFilters( pClientContext );
// 			MKL_ClientUnregister( (PVOID*)&pClientContext );
// 
// 			pClientContext = NULL;
// 		}

		return 0;
}


HRESULT
CguiDlg::InitClientContext()
{
	HRESULT hResult = S_OK;

	pClientContext=NULL;
	ApiVersion = 0;
	AppId = 0;
	FltId = 0;
	ThHandle = NULL;

	hResult = MKL_ClientRegister (
		(PVOID*)&pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		0
		);

	if (IS_ERROR( hResult ))
		return hResult;


	hResult = MKL_GetDriverApiVersion( pClientContext, &ApiVersion );
	if (IS_ERROR( hResult ))
		return hResult;;

	hResult = MKL_GetAppId( pClientContext, &AppId );

	hResult = MKL_AddFilter (
		&FltId,
		pClientContext,
		"*",
		DEADLOCKWDOG_TIMEOUT, 
		FLT_A_POPUP, //FLT_A_NOTIFY, 
		FLTTYPE_SYSTEM,
		MJ_SYSTEM_KLFLTDEV, //MJ_SYSTEM_VIRT,
		0,
		0,
		PreProcessing,
		NULL,
		NULL
		);

	if (IS_ERROR( hResult ))
		goto end;

	hResult = MKL_ChangeClientActiveStatus( pClientContext, TRUE );
	if (IS_ERROR( hResult ))
		goto end;

	if (SUCCEEDED( hResult ))
	{
		// 		PVOID pMessage;
		// 		PMKLIF_EVENT_HDR pEventHdr;

		//int cou_max = 100;

		if (SUCCEEDED( hResult ))
		{
			ThHandle = CreateThread( NULL, 0, WorkingThread, pClientContext, 0, &dwThreadID );

			if (!ThHandle)
			{
				hResult = E_FAIL; 
				goto end;
			}



		}
	}

end:
	if ( IS_ERROR( hResult ) )
	{
		if ( pClientContext )
		{
			MKL_ChangeClientActiveStatus( pClientContext, FALSE );
			MKL_DelAllFilters( pClientContext );
			MKL_ClientUnregister( (PVOID*)&pClientContext );

			pClientContext = NULL;
		}

	}

	return hResult;
}


CguiDlg::CguiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CguiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pdev_rules = NULL;
}

void CguiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CguiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_APPL, &CguiDlg::OnBnClickedBtnAppl)
	ON_LBN_SELCHANGE(IDC_LIST_DEVICE, &CguiDlg::OnLbnSelchangeListDevice)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CguiDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_LOG, &CguiDlg::OnBnClickedLog)
	ON_BN_CLICKED(IDC_BTN_SEND, &CguiDlg::OnBnClickedBtnSend)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CguiDlg message handlers
#define DENY L"Deny"
#define ALLOW L"Allow"


HRESULT
CguiDlg::LoadDeviceList( CListBox *cmb_box )
{
	ULONG max_size;
	HRESULT hResult = E_FAIL;

	//hResult = GetRulesListSize(&max_size);
	hResult = MKL_KLFltDev_GetRulesSize(
		pClientContext,
		&max_size
		);
	if (!SUCCEEDED (hResult) )
		return hResult;

	
	pdev_rules = (PKLFLTDEV_RULES)new char [max_size];
	
	if ( !pdev_rules )
		return E_OUTOFMEMORY;

	//hResult = GetRulesList(max_size, pdev_rules);
	hResult = MKL_KLFltDev_GetRules( pClientContext, pdev_rules, max_size );
	if (!SUCCEEDED (hResult) )
	{
		delete [] pdev_rules;
		pdev_rules = NULL;
		return hResult;
	}
	
	
	ULONG item_size, size;
	item_size = 0; size = 0;
	PULONG pItemSize = (PULONG)((char*)pdev_rules + sizeof(KLFLTDEV_RULES));
	PULONG pmask = (PULONG)(pItemSize + 1);
	PULONG pStrSize = (PULONG)(pmask + 1);
	PWCHAR pStr = (PWCHAR)(pStrSize + 1);
	PWCHAR pGuid, pDevType;
	PULONG  pGuidSize, pDevTypeSize;

	ULONG i=0;
	WCHAR wc_tmp[256];
	
	int nIndex=0;

	while ( i < pdev_rules->m_RulesCount && size < pdev_rules->m_size )
	{
		item_size = *pItemSize;
		memset( wc_tmp, 0, 256*sizeof(WCHAR) );
		
		//Guid
		pGuid = pStr;
		pGuidSize = pStrSize;


		//DeviceType
		pStrSize = (PULONG) ((char*) pStr + *pStrSize);
		pStr = (PWCHAR)(pStrSize + 1);
		pDevType = pStr;
		pDevTypeSize = pStrSize;

		memcpy( wc_tmp, pGuid, *pGuidSize );
		memcpy( ((char*)wc_tmp + *pGuidSize), L" - ", sizeof(L" - ") - sizeof(WCHAR) );
		memcpy( ((char*)wc_tmp + *pGuidSize + sizeof(L" - ") - sizeof(WCHAR) ), pDevType, *pDevTypeSize );
				
		nIndex = cmb_box->AddString(wc_tmp);
		cmb_box->SetItemDataPtr( nIndex, pItemSize );
		
		pItemSize = (PULONG)((char*)pItemSize + item_size );
		pmask = (PULONG)(pItemSize + 1);
		pStrSize = (PULONG)(pmask + 1);
		pStr = (PWCHAR)(pStrSize + 1);
		
		size += item_size;
		i++;
	}
	
	cmb_box->SetCurSel(0);

	return hResult;
}


BOOL CguiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	HRESULT hResult;
	
	hResult = InitClientContext();
	if ( !SUCCEEDED (hResult) )
		MessageBox( L"InitClientContext Error", L"Error",MB_OK );

	int nIndex=0;
		
	nIndex = ((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(DENY);
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetItemData( nIndex, 0 );

	nIndex = ((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(ALLOW);
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetItemData( nIndex, 1 );

	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetCurSel(0);

	hResult = LoadDeviceList ( (CListBox*)GetDlgItem(IDC_LIST_DEVICE) );
	if ( !SUCCEEDED (hResult) )
		MessageBox( L"Load device List Error", L"Error",MB_OK );
	else
		OnLbnSelchangeListDevice();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CguiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CguiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CguiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CguiDlg::OnBnClickedBtnAppl()
{
	// TODO: Add your control notification handler code here
	//HRESULT hResult = ApplyRules();
	HRESULT hResult = MKL_KLFltDev_ApplyRules( pClientContext );
	if ( !SUCCEEDED (hResult) )
	{
		MessageBox( L"Load device List Error", L"Error",MB_OK );
	}
}

void CguiDlg::OnLbnSelchangeListDevice()
{
	// TODO: Add your control notification handler code here
	PULONG pRuleSize;
	PKLFLTDEV_RULE prule;
 	int nIndex=0;
	BOOLEAN isAllow;
	BOOLEAN needLog;
 	
	nIndex = ( (CListBox*)(GetDlgItem(IDC_LIST_DEVICE)) )->GetCurSel();
 	
	if ( CB_ERR == nIndex )
		return;
	
	pRuleSize = (PULONG) ((CListBox*)(GetDlgItem(IDC_LIST_DEVICE)))->GetItemDataPtr(nIndex);
	prule = (PKLFLTDEV_RULE)( pRuleSize +1 );

// 	isAllow = (BOOLEAN) ( (prule->m_mask) >> ACCESS_OFFSET )&0x3;
// 	needLog = (BOOLEAN) ( (prule->m_mask) >> LOG_OFFSET )&0x3;
	
	isAllow = ( HIPS_GET_AM ( prule->m_mask, HIPS_POS_KLFLTDEV ) == HIPS_FLAG_ALLOW ) ? TRUE : FALSE;
	needLog = ( HIPS_GET_LOG ( prule->m_mask, HIPS_POS_KLFLTDEV ) == HIPS_LOG_ON ) ? TRUE : FALSE;

	if ( isAllow )
		((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetCurSel(1);
	else
		((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetCurSel(0);


	if ( needLog )
		((CButton*)(GetDlgItem(IDC_LOG)))->SetCheck(BST_CHECKED);
	else
		((CButton*)(GetDlgItem(IDC_LOG)))->SetCheck(BST_UNCHECKED);
}


// ULONG
// _HIPS_SET_BLOCK(ULONG am, ULONG is_log, ULONG is_inh)
// {
// 	ULONG res = 0;
// 	res = ((is_inh << 3) | (is_log << 2) | am);
// 	return res;
// }

void CguiDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	PULONG pRuleSize;
	PKLFLTDEV_RULE prule;
	int nIndex=0;
	BOOLEAN isAllow;
	BOOLEAN needLog;
	WCHAR buf[256];
	ULONG mask = 0;
	memset ( buf, 0, 256*sizeof(WCHAR) );

	nIndex = ( (CListBox*)(GetDlgItem(IDC_LIST_DEVICE)) )->GetCurSel();
	pRuleSize = (PULONG) ((CListBox*)(GetDlgItem(IDC_LIST_DEVICE)))->GetItemDataPtr(nIndex);
	prule = (PKLFLTDEV_RULE)( pRuleSize +1 );

	( (CComboBox*)(GetDlgItem(IDC_COMBO1)) )->GetWindowTextW( buf,256 );
	
	if ( wcscmp( buf, ALLOW ) == 0 )
		isAllow = TRUE;
	else
		isAllow = FALSE;


	if (((CButton*)(GetDlgItem(IDC_LOG)))->GetCheck() == BST_CHECKED )
		needLog = TRUE;
	else
		needLog = FALSE;

	
	//mask =  ( (isAllow << ACCESS_OFFSET) | (needLog << LOG_OFFSET) );

	mask = HIPS_SET_BLOCK(
		((isAllow == TRUE) ? HIPS_FLAG_ALLOW : HIPS_FLAG_DENY), 
		((needLog == TRUE) ? HIPS_LOG_ON : HIPS_LOG_OFF), 
		0
		);
	
	prule->m_mask = mask;
}

void CguiDlg::OnBnClickedLog()
{
	// TODO: Add your control notification handler code here
	OnCbnSelchangeCombo1();
}

void CguiDlg::OnBnClickedBtnSend()
{
	// TODO: Add your control notification handler code here
	int i=0, count=0;
	HRESULT hResult;

	PULONG pRuleSize;
	PKLFLTDEV_RULE prule;
	REPLUG_STATUS replug_status = Need_Noting, tmp_replug_status = Need_Noting;
	
	count = ( (CListBox*)(GetDlgItem(IDC_LIST_DEVICE)) )->GetCount();

	while ( i < count )
	{
		pRuleSize = (PULONG) ((CListBox*)(GetDlgItem(IDC_LIST_DEVICE)))->GetItemDataPtr( i );
		prule = (PKLFLTDEV_RULE)( pRuleSize +1 );

		//SetRule2( prule, *pRuleSize, &tmp_replug_status );

//////////////////////////////////////////////////////////////////////////
		//PULONG pItemSize = (PULONG)((char*)pdev_rules + sizeof(KLFLTDEV_RULES));
		PULONG pmask = &prule->m_mask;
		PULONG pStrSize = (PULONG)(pmask + 1);
		PWCHAR pStr = (PWCHAR)(pStrSize + 1);
		PWCHAR pGuid, pDevType;
		PULONG pGuidSize, pDevTypeSize;
		
		PWCHAR wcGuid, wcDeviceType;

		//Guid
		pGuid = pStr;
		pGuidSize = pStrSize;
		
		wcGuid = (PWCHAR) new char [*pGuidSize + sizeof(WCHAR)];
		if (!wcGuid)
		{
			MessageBox(L"Out of memory wcGuid",L"Error", MB_OK);
			return;
		}
		
		memset( wcGuid, 0, *pGuidSize + sizeof(WCHAR) );
		memcpy (wcGuid, pGuid, *pGuidSize);
		
		//DeviceType
		pStrSize = (PULONG) ((char*) pStr + *pStrSize);
		pStr = (PWCHAR)(pStrSize + 1);
		pDevType = pStr;
		pDevTypeSize = pStrSize;
		
		wcDeviceType = (PWCHAR) new char [*pDevTypeSize + sizeof(WCHAR)];
		if (!wcDeviceType)
		{
			MessageBox(L"Out of memory wcDeviceType",L"Error", MB_OK);
			delete [] wcGuid;
			return;
		}

		memset( wcDeviceType, 0, *pDevTypeSize + sizeof(WCHAR) );
		memcpy( wcDeviceType, pDevType, *pDevTypeSize );
		
		hResult = MKL_KLFltDev_SetRule (
			pClientContext,
			wcGuid, 
			wcDeviceType, 
			*pmask, 
			&tmp_replug_status
			);
		if ( !SUCCEEDED(hResult) )
		{
			WCHAR buf[50];
			wsprintf ( buf , L"MKL_KLFltDev_SetRule Error %d hResult = %d ", GetLastError(), hResult );
			MessageBox( buf,L"Error", MB_OK );
		}


		delete [] wcGuid;
		delete [] wcDeviceType;

/////////////////////////////////////////////////////////////////////////

		if (tmp_replug_status > replug_status)
			replug_status = tmp_replug_status;

		i++;
	}
}

void CguiDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	

	//if (pClientContext)
	//	MKL_ClientUnregister ( (PVOID*)&pClientContext );
	
	if ( pClientContext )
	{
		MKL_ChangeClientActiveStatus( pClientContext, FALSE );
		gbStop = TRUE;
	
		WaitForSingleObject( ThHandle, INFINITE );
		CloseHandle( ThHandle );

		MKL_DelAllFilters( pClientContext );
		MKL_ClientUnregister( (PVOID*)&pClientContext );

		pClientContext = NULL;
	}

	

	if ( pdev_rules )
		delete [] pdev_rules;
}
