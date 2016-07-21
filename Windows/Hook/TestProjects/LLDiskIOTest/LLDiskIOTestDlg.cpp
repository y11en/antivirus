// LLDiskIOTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LLDiskIOTest.h"
#include "LLDiskIOTestDlg.h"

#include "..\..\hook\avpgcom.h"
#include "..\..\hook\FSDrvLib.h"
#include "..\..\hook\IDriver.h"
#include "..\..\hook\hookspec.h"
#include "..\..\hook\funcs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLLDiskIOTestDlg dialog

CLLDiskIOTestDlg::CLLDiskIOTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLLDiskIOTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLLDiskIOTestDlg)
	m_BytesPerSector = 0;
	m_Cylinders = 0.0;
	m_MediaType = 0;
	m_SectorsPerTrack = 0;
	m_TracksPerCylinder = 0;
	m_StartingOffset = 0;
	m_PartitionLength = 0.0;
	m_HiddenSectors = 0;
	m_BootIndicator = 0;
	m_PartitionNumber = 0;
	m_PartitionType = 0;
	m_RecognizedPartition = 0;
	m_RewritePartition = 0;
	m_ReadThisSector = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLLDiskIOTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLLDiskIOTestDlg)
	DDX_Control(pDX, IDC_COMBO_DEVICE_TYPE, m_DeviceType);
	DDX_Control(pDX, IDC_COMBO_LETTER, m_cbLetter);
	DDX_Text(pDX, IDC_EDIT_BYTES_PER_SECTOR, m_BytesPerSector);
	DDV_MinMaxUInt(pDX, m_BytesPerSector, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_CYLINDERS, m_Cylinders);
	DDV_MinMaxDouble(pDX, m_Cylinders, 0., 99999999999.);
	DDX_Text(pDX, IDC_EDIT_MEDIA_TYPE, m_MediaType);
	DDV_MinMaxUInt(pDX, m_MediaType, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_SECTORS_PER_TRACKS, m_SectorsPerTrack);
	DDV_MinMaxUInt(pDX, m_SectorsPerTrack, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_TRAKS_PER_CYLINDER, m_TracksPerCylinder);
	DDV_MinMaxUInt(pDX, m_TracksPerCylinder, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_STARTINGOFFSET, m_StartingOffset);
	DDV_MinMaxDouble(pDX, m_StartingOffset, 0., 99999999999.);
	DDX_Text(pDX, IDC_EDIT_PARTITION_LENGTH, m_PartitionLength);
	DDV_MinMaxDouble(pDX, m_PartitionLength, 0., 9999999999999.);
	DDX_Text(pDX, IDC_EDIT_HIDDEN_SECTORS, m_HiddenSectors);
	DDV_MinMaxDWord(pDX, m_HiddenSectors, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_HIDDEN_BOOT_INDICATOR, m_BootIndicator);
	DDV_MinMaxDWord(pDX, m_BootIndicator, 0, 99);
	DDX_Text(pDX, IDC_EDIT_HIDDEN_PARTITION_NUMBER, m_PartitionNumber);
	DDV_MinMaxDWord(pDX, m_PartitionNumber, 0, 99999);
	DDX_Text(pDX, IDC_EDIT_HIDDEN_PARTITION_TYPE, m_PartitionType);
	DDV_MinMaxUInt(pDX, m_PartitionType, 0, 9999999);
	DDX_Text(pDX, IDC_EDIT_HIDDEN_RECOGNIZED_PARTITION, m_RecognizedPartition);
	DDV_MinMaxUInt(pDX, m_RecognizedPartition, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_HIDDEN_REWRITE_PARTITITON, m_RewritePartition);
	DDV_MinMaxUInt(pDX, m_RewritePartition, 0, 99999);
	DDX_Text(pDX, IDC_EDIT_READ_THIS_SECTOR, m_ReadThisSector);
	DDV_MinMaxUInt(pDX, m_ReadThisSector, 0, 4294967295);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLLDiskIOTestDlg, CDialog)
	//{{AFX_MSG_MAP(CLLDiskIOTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GETPARTITIONINFO, OnButtonGetpartitioninfo)
	ON_BN_CLICKED(IDC_BUTTON_READ_THIS_SECTOR, OnButtonReadThisSector)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLLDiskIOTestDlg message handlers

BOOL CLLDiskIOTestDlg::OnInitDialog()
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

	CString strtmp;
	for (int cou = 'A'; cou <= 'Z'; cou++)
	{
		strtmp = (char) cou;
		m_cbLetter.AddString(strtmp);
	}
	m_cbLetter.AddString("DR0");
	m_cbLetter.AddString("Floppy0");
	m_cbLetter.AddString("\\Device\\HarddiskVolume1");
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	if (FSDrvInterceptorInit() == FALSE)
		SetWindowText("Connection to driver failed");
	else
	{
		AddFSFilterW(FSDrvGetDeviceHandle(), FSDrvGetAppID(), L"*", "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_MOUNT_VOLUME, 0, PostProcessing, NULL);

		char pBuffer[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST)];
		PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) pBuffer;
		PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
		
		ULONG retsize;
		
		ZeroMemory(pRequest, sizeof(EXTERNAL_DRV_REQUEST) + sizeof(DISK_IO_REQUEST));
		pRequest->m_DrvID = FLTTYPE_DISK;
		pRequest->m_IOCTL = _AVPG_IOCTL_DISK_QUERYNAMES;
		pRequest->m_AppID = FSDrvGetAppID();
		pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

		pDiskRequest->m_Tag = -1;
		pDiskRequest->m_DataSize = 0;
		pDiskRequest->m_ByteOffset = 0;
		
		WCHAR pNames[0x1000];
		retsize = sizeof(pNames);
		ZeroMemory(pNames, sizeof(pNames));

		IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, pNames, &retsize);

	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLLDiskIOTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLLDiskIOTestDlg::OnPaint() 
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
HCURSOR CLLDiskIOTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CLLDiskIOTestDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	FSDrvInterceptorDone();

	return CDialog::DestroyWindow();
}

// -----------------------------------------------------------------------------------------

void CLLDiskIOTestDlg::OnButtonGetpartitioninfo() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	__int64 longlong;
	m_BytesPerSector = 0;
	longlong = 0;
	m_Cylinders = (double) longlong;
	m_MediaType = 0;
	m_SectorsPerTrack = 0;
	m_TracksPerCylinder = 0;

	PDISK_IO_REQUEST	pDiskRequest;
	
	WCHAR pwName[MAX_PATH];

	int cursel = m_cbLetter.GetCurSel();
	int MaxIdx = 'Z' - 'A';

	if (cursel < MaxIdx)
	{
		pwName[0] = L'A' + cursel;
		pwName[1] = 0;
	}
	else
	{
//		lstrcpyW(pwName, L"\\Device\\Harddisk0\\DR0");
		lstrcpyW(pwName, L"\\Device\\HarddiskVolume1");
	}
	
	BYTE Buffer[512];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GET_GEOMETRY;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW((PWCHAR)pDiskRequest->m_DriveName, pwName);

	ULONG retsize;
	retsize = sizeof(Buffer);

	if (IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, Buffer, &retsize) == TRUE)
	{
		PDISK_GEOMETRY geom = (PDISK_GEOMETRY) Buffer;
		m_BytesPerSector = geom->BytesPerSector;
		longlong = *(__int64*) (&geom->Cylinders);
		m_Cylinders = (double) longlong;
		m_MediaType = geom->MediaType;
		m_SectorsPerTrack = geom->SectorsPerTrack;
		m_TracksPerCylinder = geom->TracksPerCylinder;
	}


	// -----------------------------------------------------------------------------------------
	m_StartingOffset = 0;
	m_PartitionLength = 0;
	m_HiddenSectors = 0;
	m_BootIndicator = 0;
	m_PartitionNumber = 0;
	m_PartitionType = 0;
	m_RecognizedPartition = 0;
	m_RewritePartition = 0;
	
	pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GET_PARTITION_INFO;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);
	
	pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW((PWCHAR)pDiskRequest->m_DriveName, pwName);

	retsize = sizeof(Buffer);
	
	if (IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, Buffer, &retsize) == TRUE)
	{
		PARTITION_INFORMATION* ppart = (PARTITION_INFORMATION*) Buffer;
		longlong = *(__int64*) (&ppart->StartingOffset);
		m_StartingOffset = (double) longlong;
		longlong = *(__int64*) (&ppart->PartitionLength);
		m_PartitionLength = (double) longlong;
		m_HiddenSectors = ppart->HiddenSectors;
		m_BootIndicator = ppart->BootIndicator;
		m_PartitionNumber = ppart->PartitionNumber;
		m_PartitionType = ppart->PartitionType;
		m_RecognizedPartition = ppart->RecognizedPartition;
		m_RewritePartition = ppart->RewritePartition;
	}

	pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_GET_DEVICE_TYPE;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);
	
	pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;
	lstrcpyW((PWCHAR)pDiskRequest->m_DriveName, pwName);
	
	retsize = sizeof(Buffer);
	
	if (IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, Buffer, &retsize) == TRUE)
	{
		DWORD* pDeviceType = (DWORD*) Buffer;
		m_DeviceType.SetCurSel(*pDeviceType);
	}

	UpdateData(FALSE);
}

void CLLDiskIOTestDlg::OnButtonReadThisSector() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if (m_BytesPerSector == 0)
		return;

	WCHAR wch = L'A' + m_cbLetter.GetCurSel();
	
	BYTE Buffer[1024];
	PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
	PDISK_IO_REQUEST pDiskRequest = (PDISK_IO_REQUEST) pRequest->m_Buffer;
	pDiskRequest->m_Tag = -1;

	pRequest->m_DrvID = FLTTYPE_DISK;
	pRequest->m_IOCTL = _AVPG_IOCTL_DISK_READ;
	pRequest->m_AppID = FSDrvGetAppID();
	pRequest->m_BufferSize = sizeof(DISK_IO_REQUEST);

	pDiskRequest->m_DataSize = m_BytesPerSector;
	pDiskRequest->m_DriveName[0] = wch;
	pDiskRequest->m_DriveName[1] = 0;
	
	pDiskRequest->m_ByteOffset = ((__int64) m_ReadThisSector) * m_BytesPerSector;
	
	ULONG retsize;
	retsize = sizeof(Buffer);
	
	if (IDriverExternalDrvRequest(FSDrvGetDeviceHandle(), pRequest, Buffer, &retsize) == TRUE)
	{
		OutputDebugString("read!!!\n");
	}
}
