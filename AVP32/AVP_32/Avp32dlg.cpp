// Avp32Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include "DlgProxy.h"
#include "Avp32Dlg.h"
#include "DisDlg.h"
#include "AVPRptID.h"
#include <mmsystem.h>
#include <io.h>

#include <Version/ver_avp.h>

#include <Bases/Retcode3.h>
#include <Bases/Format/Base.h>
#include <ScanAPI/avp_dll.h>
//#include "../key.h"
#include "../avp_aler.h" 
#include <Loadkeys/loadkeys.h>
#include "../fn.h" 
#include <AVPRegID.h>
#include <avpcomid.h>
#include <loadkeys/avpkeyid.h>
#include <property/property.h>
#include <Stuff/layout.h>

#include <Sign/sign.h>
#define SIGN_BUF_LEN 0x2000
#define  SignCloseHandle	CloseHandle
#define  SignReadFile		ReadFile
#define  SignCreateFile		CreateFile 
#include <Sign/a_chfile.c>
#include <WASCRes.rh>

#include <stuff/calcsum.h>
#include <bases/format/base.h>
#include <bases/format/records.h>
#include "../basework/workarea.h"
#include <fileformat/coff.h>

#ifdef AVE_SQL	
#include "../ave_sql/ave_fld.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Andrey
#include "agentint.h"
BOOL boProccessAndExit = FALSE;
BOOL should_reload=0;
BOOL delete_archives=0;

// Andrey
/////////////////////////////////////////////////////////////////////////////////////////

#include "FindDlg.h"

#ifndef ListView_SetExtendedListViewStyle
#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)
#define ListView_SetExtendedListViewStyle(hwndLV, dw)\
        (DWORD)SNDMSG((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)
#define LVM_GETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 55)
#define ListView_GetExtendedListViewStyle(hwndLV)\
        (DWORD)SNDMSG((hwndLV), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0)
#define LVS_EX_GRIDLINES        0x00000001
#define LVS_EX_SUBITEMIMAGES    0x00000002
#define LVS_EX_CHECKBOXES       0x00000004
#define LVS_EX_TRACKSELECT      0x00000008
#define LVS_EX_HEADERDRAGDROP   0x00000010
#define LVS_EX_FULLROWSELECT    0x00000020 // applies to report mode only
#define LVS_EX_ONECLICKACTIVATE 0x00000040
#define LVS_EX_TWOCLICKACTIVATE 0x00000080
#endif


CFindDlg FindDlg;

//CPtrArray KeyHandleArray;

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
#include <ScanAPI/avp_msg.h>
const UINT	 wm_AVP = RegisterWindowMessage( AVP_MSGSTRING );
const UINT WM_AVPUPD=RegisterWindowMessage("UM_RELOADBASE");

HWND	 h_AVP_KERNEL;

OSVERSIONINFO os;

static LOAD_PROGRESS_STRUCT* lps=NULL;

CString StartPath;
CString IniFile;
CString KeysPath;
static CString LocFile;
CString prof;
CStringArray LockedFiles;
BOOL needReboot=0;

int KillScan=1;


extern CRITICAL_SECTION TotalSpaceCriticalSection;
DWORDLONG TotalSpace = 0;

extern ExitCode;
extern char	execName[];

static int Xpos;
static int Ypos;
static int Xsize;
static int Ysize;

//extern CStringArray ExtArray;
//CStringArray Mess;

#include <winioctl.h>
#include <ScanAPI/avpioctl.h>


/* BOOL IsAdmin(void)

  returns TRUE if user is an admin
          FALSE if user is not an admin
*/
 
BOOL IsAdmin(void)
{
	HANDLE hAccessToken;
	UCHAR InfoBuffer[1024];
	PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS)InfoBuffer;
	DWORD dwInfoBufferSize;
	PSID psidAdministrators;
	SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
	UINT x;
	BOOL bSuccess;


	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_READ,&hAccessToken))
//		return(FALSE);
		return(TRUE);

	bSuccess =GetTokenInformation(hAccessToken,TokenGroups,InfoBuffer,
		1024, &dwInfoBufferSize);

	CloseHandle(hAccessToken);

	if( !bSuccess )
//		return FALSE;
		return(TRUE);

	if(!AllocateAndInitializeSid(&siaNtAuthority, 2,
	SECURITY_BUILTIN_DOMAIN_RID,
	DOMAIN_ALIAS_RID_ADMINS,
	0, 0, 0, 0, 0, 0,
	&psidAdministrators))
//	return FALSE;
		return(TRUE);

	// assume that we don't find the admin SID.
	bSuccess = FALSE;

	for(x=0;x<ptgGroups->GroupCount;x++)
	{
		if(EqualSid(psidAdministrators, ptgGroups->Groups[x].Sid) )
		{
			bSuccess = TRUE;
			break;
		}
	}
	FreeSid(psidAdministrators);
	return bSuccess;
}



BOOL AvpmRegisterThread(int on_ ,DWORD dwPlatformId)
{
	BOOL ret=FALSE;
	HANDLE hfile;
	DWORD count = 0L;
	
	if(dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		hfile=CreateFile("\\\\.\\FSAVPDevice0",
				GENERIC_READ|GENERIC_WRITE,	FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,OPEN_EXISTING,0,NULL);
	}else{
		hfile=CreateFile("\\\\.\\AVP95",
			0, FILE_SHARE_READ|FILE_SHARE_WRITE, 
			NULL,OPEN_EXISTING,FILE_FLAG_DELETE_ON_CLOSE,NULL);
	}
	if (hfile != INVALID_HANDLE_VALUE ){
		ret=DeviceIoControl(hfile, 
			on_ ? IOCTL_AVP_REGISTERTHREAD : IOCTL_AVP_UNREGISTERTHREAD, 
			NULL,0,NULL,0,&count,0);
		CloseHandle(hfile);
	}
	return ret;
}



/////////////////////////////////////////////////////////////////////////////
// CAvp32Dlg dialog
DWORD propByTimer=0;
DWORD propByEvent=0;
DWORD propEvent=0;
DWORD propInterval=0;

IMPLEMENT_DYNAMIC(CAvp32Dlg, CDialog);

CAvp32Dlg::CAvp32Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAvp32Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAvp32Dlg)
	m_sLine = _T("");
	m_Track = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_pAutoProxy = NULL;

	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	AvpmRegisterThread(1,os.dwPlatformId);

	functionality=0;
	MinSize.x=0;
	MinSize.y=0;

	hReportFile=INVALID_HANDLE_VALUE;
	ReportWithTiming=0;
	HiResCounterSupported=0;
/*	m_CA=0;
	m_Disinfect=0;
	m_Extract=0;
	m_MailBases=0;
	m_MailPlain=0;
	m_LicenceAlarm=0;
/*
/*	m_NOVELL=0;
	m_DOS=1;
	m_DOSLITE=1;
	m_OS2=0;
	m_WIN31=1;
	m_WIN95=1;
	m_WINNT=0;
*/
/*
	m_RemoteLaunch=0;
	m_RemoteScan=0;
	m_CopyInfo=_T("Evaluation copy");
	m_Unpack=0;
	m_LicenceDate= CTime(2038,1,1,0,0,0);
*/
	m_Name=_T("");
	m_Org=_T("");
/*
	m_Regnumber=_T("");
	m_Every=1;
	m_CRC=0;
	m_Monitor=0;
*/
	MethodDelete=0;
	SectorWarn=0;
	VirCount=0;	
	clStart=0;
	clQuit=0;
	clMinimize=0;
	clListDelete=0;

	char* p;
	GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,IniFile.GetBuffer(512),&p);
	*p=0;
	IniFile.ReleaseBuffer();
	StartPath=IniFile;
	IniFile+="AVP.INI";

	GetPrivateProfileString("AVP32","DefaultProfile","",prof.GetBuffer(0x200),0x200,IniFile);
	prof.ReleaseBuffer();
	if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
		WritePrivateProfileString("AVP32","DefaultProfile",prof,IniFile);

	if(prof.GetLength()){
		char buf[0x200];
		char* ptr;
		if(GetFullPathName(prof,0x500,buf,&ptr)) 
			prof=buf;
	}
	if(_access(prof,2))
		prof="AVP32.INI";

	// LOCALIZATION
	GetPrivateProfileString("AVP32","LocFile","AVP_LOC.DLL",LocFile.GetBuffer(0x500),0x500,IniFile);
	LocFile.ReleaseBuffer();
	WritePrivateProfileString("AVP32","LocFile",LocFile,IniFile);
	
	if(LocFile.GetLength() && SIGN_OK==SignCheckFile(LocFile))
		LoadLocLibrary("", LocFile);
	
	

//	AppName=_T("Kaspersky Anti-Virus");
	AppName=MS(IDS_PRODUCT_NAME);
	CommandLine=AfxGetApp()->m_lpCmdLine;
	CommandLine.MakeLower();
	CommandLine+=" ";

	BaseTime.GetCurrentTime();
	
	ScanThreadPtr=NULL;
	VirThreadPtr=NULL;
	BinMBR=(BYTE*)LockResource(LoadResource(NULL,FindResource(NULL,(const char*)IDR_BIN_M,"BIN")));
	BinBOOT=(BYTE*)LockResource(LoadResource(NULL,FindResource(NULL,(const char*)IDR_BIN_B,"BIN")));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CanCancel=0;

#ifdef AVE_SQL	
	const char* sw=_T("/ave_sql=");
	int i=CommandLine.Find(sw);
	if(-1!=i){
		DWORD level=atoi((LPCTSTR)CommandLine+i+strlen(sw));
		if(level && InitDB(malloc, free)){
			if(level & 0x10) ClearDatabase();
			AVP_SetCallbackRecords(level);
		}
	}
#endif

}

CAvp32Dlg::~CAvp32Dlg()
{
/*	while(KeyHandleArray.GetSize())
	{	
		DATA_Remove( (HDATA)KeyHandleArray[0], 0 );
		KeyHandleArray.RemoveAt(0);
	}
*/
#ifdef AVE_SQL
	DoneDB();
#endif
	AVP_Destroy();
	AvpmRegisterThread(0,os.dwPlatformId);
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;

}

void CAvp32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvp32Dlg)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_ANIMATE_SCAN, m_AnimateScan);
	DDX_Control(pDX, IDC_TAB1, m_Tab1);
	DDX_Text(pDX, IDC_STAT_LINE, m_sLine);
	DDX_Control(pDX, IDC_REPV, m_RepView);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		tPage1.GetPaths(PathArray);
	}
	else{
		tPage1.SetPaths(PathArray);
	}
		
//	tPage2.m_Packed &= !!(functionality & FN_UNPACK);
//	tPage2.m_Archives &= !!(functionality & FN_EXTRACT);
//	tPage2.m_MailBases &= !!(functionality & FN_MAIL);
//	tPage2.m_MailPlain &= !!(functionality & FN_MAIL);
//	tPage3.m_CA &= !!(functionality & FN_CA);
	if(!(functionality & FN_DISINFECT) && tPage6.m_Action<3)  tPage6.m_Action = 0;
}

BEGIN_MESSAGE_MAP(CAvp32Dlg, CDialog)
	//{{AFX_MSG_MAP(CAvp32Dlg)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_AVP_CHANGETAB, OnAvpChangetab)
	ON_COMMAND(ID_AVP_SCANEND, OnAvpScanEnd)
    ON_REGISTERED_MESSAGE( wm_AVP, OnAVPMessage )
	ON_REGISTERED_MESSAGE(WM_AVPUPD, OnAvpupd)
    ON_REGISTERED_MESSAGE( LM_UPDATE_DATA, OnUpdateData)
	ON_WM_CLOSE()
	ON_COMMAND(ID_AVP_RELOADBASES, OnAvpReloadbases)
	ON_COMMAND(ID_AVP_START, OnAvpStart)
	ON_COMMAND(ID_HELP_ABOUTAVP32, OnHelpAboutavp32)
	ON_WM_TIMER()
	ON_COMMAND(ID_SCAN_NOW, OnScanNow)
	ON_COMMAND(ID_STOP, OnStop)
	ON_BN_CLICKED(IDC_SCAN_NOW, OnScanNowButton)
	ON_COMMAND(ID_FILE_LOADPROFILE, OnFileLoadprofile)
	ON_COMMAND(ID_FILE_SAVEPROFILE, OnFileSaveprofile)
	ON_COMMAND(ID_FILE_SAVEPROFILEASDEFAULT, OnFileSaveprofileasdefault)
	ON_COMMAND(ID_FIND, OnFind)
	ON_COMMAND(ID_FINDAGAIN, OnFindagain)
	ON_COMMAND(ID_TRACK, OnTrack)
	ON_COMMAND(ID_ADD_DISKS, OnAddDisks)
	ON_COMMAND(ID_POP_MARK, OnPopMark)
	ON_COMMAND(ID_POP_SCAN, OnPopScan)
	ON_COMMAND(ID_POP_UNMARK, OnPopUnmark)
	ON_COMMAND(ID_TOOLS_UPDATENOW, OnToolsUpdatenow)
	ON_COMMAND(ID_HELP_AVPONTHEWEB, OnHelpAvpontheweb)
	ON_COMMAND(ID_CUSTOMIZE, OnCustomize)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_VIRLIST, OnVirlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvp32Dlg message handlers
CAvp32Dlg* Avp32Dlg;

BOOL CAvp32Dlg::OnInitDialog()
{
	CString s;
	long r;
	Avp32Dlg=this;
	CDialog::OnInitDialog();
	SetWindowText(AppName);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
//	m_AnimateScan.Open(IDR_AVI_SEARCH);
//	m_AnimateScan.Open(IDR_AVI1);

	r=GetWindowLong(GetDlgItem(IDC_STAT_LINE)->m_hWnd,GWL_STYLE);
	SetWindowLong(GetDlgItem(IDC_STAT_LINE)->m_hWnd,GWL_STYLE,SS_NOPREFIX | r);

	HINSTANCE hiold=AfxGetResourceHandle();
	if(hLocalizationLibrary>0)	AfxSetResourceHandle(hLocalizationLibrary);
	if(Menu.LoadMenu(IDD_AVP32_DIALOG)){
		GetMenu()->DestroyMenu();
		SetMenu(&Menu);
	}
    BOOL pml=popMenu.LoadMenu(IDR_POP_MENU);
	AfxSetResourceHandle(hiold);

    if(!pml)popMenu.LoadMenu(IDR_POP_MENU);
	


	char ExeName[0x200];
	char DllName[0x200];
	char* lpFilePart;
	GetModuleFileName(NULL,ExeName,0x200);
	sign_check=SignReportMB(ExeName,SignCheckFile(ExeName),0);
	GetFullPathName(ExeName,0x200,DllName,&lpFilePart);
	strcpy(lpFilePart,"AVPBASE.DLL");
	sign_check=SignReportMB(DllName,SignCheckFile(DllName),0);
	sign_check=sign_check_file(DllName,1,(void**)1,0,0);
	if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		strcpy(lpFilePart,"AVP_IONT.DLL");
		SignReportMB(DllName,SignCheckFile(DllName),0);
	}else
	{
		strcpy(lpFilePart,"AVP_IO.VXD");
		SignReportMB(DllName,SignCheckFile(DllName),0);
		strcpy(lpFilePart,"AVP_IO32.DLL");
		SignReportMB(DllName,SignCheckFile(DllName),0);
	}

	VirCountArray.SetSize(0,0x200);

	GetDlgItem(IDC_SCAN_NOW)->SetWindowText(MS( IDS_SCAN_NOW) );
//	GetDlgItem(IDC_TRACK)->SetWindowText(MS( IDS_TAB3_TRACK ));

	tPage1.Create(CTabPage1::IDD,&m_Tab1,this);
	tPage2.Create(CTabPage2::IDD,&m_Tab1,this);
	tPage3.Create(CTabPage3::IDD,&m_Tab1,this);
	tPage5.Create(CTabPage5::IDD,&m_Tab1,this);
	tPage6.Create(CTabPage6::IDD,&m_Tab1,this);

	tPage1.SetWindowText(MS( IDS_TAB_LOCATION ));
	tPage2.SetWindowText(MS( IDS_TAB_OBJECTS ));
	tPage3.SetWindowText(MS( IDS_TAB_OPTIONS ));
	tPage5.SetWindowText(MS( IDS_TAB_STATISTICS ));
	tPage6.SetWindowText(MS( IDS_TAB_ACTIONS ));

	tPage1.GetDlgItem(IDC_LOCAL)->SetWindowText(MS( IDS_TAB1_LOCAL ));
	tPage1.GetDlgItem(IDC_NETWORK)->SetWindowText(MS( IDS_TAB1_NETWORK ));
	tPage1.GetDlgItem(IDC_FLOPPY)->SetWindowText(MS( IDS_TAB1_FLOPPY ));
	tPage1.GetDlgItem(IDC_BUTTON_BROWSE)->SetWindowText(MS( IDS_TAB1_ADD_FOLDER ));

	tPage2.GetDlgItem(IDC_MEMORY)->SetWindowText(MS( IDS_TAB2_MEMORY ));
	tPage2.GetDlgItem(IDC_SECTORS)->SetWindowText(MS( IDS_TAB2_SECTORS ));
	tPage2.GetDlgItem(IDC_FILES)->SetWindowText(MS( IDS_TAB2_FILES ));
	tPage2.GetDlgItem(IDC_PACKED)->SetWindowText(MS( IDS_TAB2_PACKED ));
	tPage2.GetDlgItem(IDC_EXTRACT)->SetWindowText(MS( IDS_TAB2_EXTRACT ));
	tPage2.GetDlgItem(IDC_MAILBASES)->SetWindowText(MS( IDS_TAB2_MAILBASES ));
	tPage2.GetDlgItem(IDC_MAILPLAIN)->SetWindowText(MS( IDS_TAB2_MAILPLAIN ));
	tPage2.GetDlgItem(IDC_MASK_FRAME)->SetWindowText(MS( IDS_TAB2_MASK_FRAME ));
	tPage2.GetDlgItem(IDC_SMART)->SetWindowText(MS( IDS_TAB2_SMART ));
	tPage2.GetDlgItem(IDC_PROGRAMS)->SetWindowText(MS( IDS_TAB2_PROGRAMS ));
	tPage2.GetDlgItem(IDC_ALLFILES)->SetWindowText(MS( IDS_TAB2_ALLFILES ));
	tPage2.GetDlgItem(IDC_USERDEF)->SetWindowText(MS( IDS_TAB2_USERDEF ));
	tPage2.GetDlgItem(IDC_EXCLUDE)->SetWindowText(MS( IDS_TAB2_EXCLUDE ));

	tPage3.GetDlgItem(IDC_WARNINGS)->SetWindowText(MS( IDS_TAB3_WARNINGS ));
	tPage3.GetDlgItem(IDC_CA)->SetWindowText(MS( IDS_TAB3_CA ));
	tPage3.GetDlgItem(IDC_REDUNDANT)->SetWindowText(MS( IDS_TAB3_REDUNDANT ));
	tPage3.GetDlgItem(IDC_SHOWOK)->SetWindowText(MS( IDS_TAB3_SHOWOK ));
	tPage3.GetDlgItem(IDC_SHOWPACK)->SetWindowText(MS( IDS_TAB3_SHOWPACK ));
	tPage3.GetDlgItem(IDC_REPORT)->SetWindowText(MS( IDS_TAB3_REPORT ));
	tPage3.GetDlgItem(IDC_APPEND)->SetWindowText(MS( IDS_TAB3_APPEND ));
	tPage3.GetDlgItem(IDC_LIMIT)->SetWindowText(MS( IDS_TAB3_LIMIT ));
	tPage3.GetDlgItem(IDC_BUTTON_CUST)->SetWindowText(MS( IDS_CUSTOMIZE_BTN ));

	tPage5.GetDlgItem(IDC_SCANNED_FRAME)->SetWindowText(MS( IDS_TAB5_SCANNED_FRAME ));
	tPage5.GetDlgItem(IDC_FOUND_FRAME)->SetWindowText(MS( IDS_TAB5_FOUND_FRAME ));
	tPage5.GetDlgItem(IDC_STATIC_SECTORS)->SetWindowText(MS( IDS_TAB5_STATIC_SECTORS ));
	tPage5.GetDlgItem(IDC_STATIC_FILES)->SetWindowText(MS( IDS_TAB5_STATIC_FILES ));
	tPage5.GetDlgItem(IDC_STATIC_FOLDERS)->SetWindowText(MS( IDS_TAB5_STATIC_FOLDERS ));
	tPage5.GetDlgItem(IDC_STATIC_PACKED)->SetWindowText(MS( IDS_TAB5_STATIC_PACKED ));
	tPage5.GetDlgItem(IDC_STATIC_ARCHIVES)->SetWindowText(MS( IDS_TAB5_STATIC_ARCHIVES ));
	tPage5.GetDlgItem(IDC_STATIC_TIME)->SetWindowText(MS( IDS_TAB5_STATIC_TIME ));
	tPage5.GetDlgItem(IDC_STATIC_SPEED)->SetWindowText(MS( IDS_TAB5_STATIC_SPEED ));
	tPage5.GetDlgItem(IDC_STATIC_VIRUSES)->SetWindowText(MS( IDS_TAB5_STATIC_VIRUSES ));
	tPage5.GetDlgItem(IDC_STATIC_BODIES)->SetWindowText(MS( IDS_TAB5_STATIC_BODIES ));
	tPage5.GetDlgItem(IDC_STATIC_DISINFECTED)->SetWindowText(MS( IDS_TAB5_STATIC_DISINFECTED ));
	tPage5.GetDlgItem(IDC_STATIC_DELETED)->SetWindowText(MS( IDS_TAB5_STATIC_DELETED ));
	tPage5.GetDlgItem(IDC_STATIC_WARNINGS)->SetWindowText(MS( IDS_TAB5_STATIC_WARNINGS ));
	tPage5.GetDlgItem(IDC_STATIC_SUSPIC)->SetWindowText(MS( IDS_TAB5_STATIC_SUSPIC ));
	tPage5.GetDlgItem(IDC_STATIC_CORRUPTED)->SetWindowText(MS( IDS_TAB5_STATIC_CORRUPTED ));
	tPage5.GetDlgItem(IDC_STATIC_IOERRORS)->SetWindowText(MS( IDS_TAB5_STATIC_IOERRORS ));

	tPage6.GetDlgItem(IDC_INFECTED_OBJ)->SetWindowText(MS( IDS_TAB6_INFECTED_OBJ ));
	tPage6.GetDlgItem(IDC_SUSP_OBJ)->SetWindowText(MS( IDS_TAB6_SUSP_OBJ ));
	tPage6.GetDlgItem(IDC_REPORTONLY)->SetWindowText(MS( IDS_TAB6_REPORTONLY ));
	tPage6.GetDlgItem(IDC_DISINFECTDIALOG)->SetWindowText(MS( IDS_TAB6_DISINFECTDIALOG ));
	tPage6.GetDlgItem(IDC_DISINFECTAUTO)->SetWindowText(MS( IDS_TAB6_DISINFECTAUTO ));
	tPage6.GetDlgItem(IDC_DELETE)->SetWindowText(MS( IDS_TAB6_DELETE ));
	tPage6.GetDlgItem(IDC_COPYINFECTED)->SetWindowText(MS( IDS_TAB6_COPYINFECTED ));
	tPage6.GetDlgItem(IDC_COPYSUSPICIOUS)->SetWindowText(MS( IDS_TAB6_COPYSUSPICIOUS ));

	InsertTabPage(&m_Tab1,&tPage1);
	InsertTabPage(&m_Tab1,&tPage2);
	InsertTabPage(&m_Tab1,&tPage6);
	InsertTabPage(&m_Tab1,&tPage3);
	InsertTabPage(&m_Tab1,&tPage5);

	OnSelchangeTab1(NULL, &r);
/*
	ListView_SetExtendedListViewStyle(m_List.m_hWnd, ListView_GetExtendedListViewStyle(m_List.m_hWnd)|LVS_EX_FULLROWSELECT);

	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	lvcolumn.pszText = (char*)(const char*)MS( IDS_LIST_ONAME );
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 290;
	m_List.InsertColumn(0, &lvcolumn);

	lvcolumn.pszText = (char*)(const char*)MS( IDS_LIST_RESULT );
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = 235;
	m_List.InsertColumn(1, &lvcolumn);
*/
	m_tbToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_TOOLTIPS);
	m_tbToolbar.LoadToolBar(IDR_TB_REP);
	m_tbToolbar.ShowWindow(SW_SHOWMAXIMIZED);
	m_tbToolbar.SetButtonStyle(0, TBBS_CHECKBOX);
	m_tbToolbar.GetToolBarCtrl().CheckButton(ID_TRACK, m_Track);



	CRect cr;
	GetWindowRect(&cr);
	MinSize.x=cr.right-cr.left;
	MinSize.y=cr.bottom-cr.top;

	GetPrivateProfileString("Configuration","KeysPath","",KeysPath.GetBuffer(0x500),0x500,IniFile);
	KeysPath.ReleaseBuffer();
	if(KeysPath.GetLength())
		if(KeysPath.Right(1)!="\\")KeysPath+="\\";
	if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
		WritePrivateProfileString("Configuration","KeysPath",KeysPath,IniFile);

	DWORD Level=0;
	BOOL Valid=0;
	BOOL AnyKeyFound=0;

	if(KeysPath.GetLength())
	AnyKeyFound|=LoadKeys(os.dwPlatformId==VER_PLATFORM_WIN32_NT?AVP_APID_SCANNER_WIN_NTWS:AVP_APID_SCANNER_WIN_95,
		KeysPath, &Level, &Valid, /*&KeyHandleArray*/NULL);

	AnyKeyFound|=LoadKeys(os.dwPlatformId==VER_PLATFORM_WIN32_NT?AVP_APID_SCANNER_WIN_NTWS:AVP_APID_SCANNER_WIN_95,
		StartPath, &Level, &Valid, /*&KeyHandleArray*/NULL);

	if(ExitCode!=7)
	{
		if(!AnyKeyFound)
		{
			MessageBox(MS(IDS_KEY_ABSENT),AppName,	MB_ICONINFORMATION|MB_OK);
		}
		else{
			if(Level==0){
				MessageBox((os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)?
					MS(IDS_KEY_PLATHFORM_95):MS(IDS_KEY_PLATHFORM_NT),
					AppName,
					MB_ICONINFORMATION|MB_OK);
			}
			else{
				if(Level) functionality |= FN_UPDATES;
				if(Level>=10) functionality |= FN_MINIMAL;
				if(Level>=20) functionality |= FN_OPTIONS;
				if(Level>=30) functionality |= FN_FULL;
				if(!Valid){
					functionality &= ~FN_UPDATES;
					MessageBox(MS(IDS_UPDATE_EXPIRED), AppName, MB_ICONINFORMATION|MB_OK);
				}
			}

		}

		if(!(functionality & FN_NETWORK)){
			char* p;
			CString s;
			DWORD i=GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,s.GetBuffer(512),&p);
			s.ReleaseBuffer();
			if(GetDriveType(s.Left(3)) == DRIVE_REMOTE){
				functionality=0;
				MessageBox(MS(IDS_REMOTE_LAUNCH),AppName,MB_ICONSTOP|MB_OK);
			}
		}

		if(os.dwPlatformId==VER_PLATFORM_WIN32_NT && !IsAdmin()
			&& (-1 == CommandLine.Find(_T("/a ")))
			&& !clStart
			&& cust.m_CustOtherMessages)
		{
			MessageBox(MS(IDS_ADMIN_RIGHTS),AppName,MB_ICONINFORMATION|MB_OK);
		}
	}

	HKEY hKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,AVP_REGKEY_INST_WS_FULL,&hKey) == ERROR_SUCCESS) 
	{
		DWORD n;
		n= MAX_PATH;
		RegQueryValueEx(hKey,AVP_REGVALUE_USERNAME,NULL,NULL,(UCHAR*)m_Name.GetBuffer(MAX_PATH),&n);
		m_Name.ReleaseBuffer();
		n= MAX_PATH;
		RegQueryValueEx(hKey,AVP_REGVALUE_USERCOMPANY,NULL,NULL,(UCHAR*)m_Org.GetBuffer(MAX_PATH),&n);
		m_Org.ReleaseBuffer();
		RegCloseKey(hKey);
	}


	if(m_Name.GetLength()==0){
		GetPrivateProfileString(_T("Registration"),_T("NAME"),_T(""),m_Name.GetBuffer(512),512,IniFile);
		m_Name.ReleaseBuffer();
	}
	if(m_Org.GetLength()==0){
		GetPrivateProfileString(_T("Registration"),_T("COMPANY"),_T(""),m_Org.GetBuffer(512),512,IniFile);
		m_Org.ReleaseBuffer();
	}
/*	if(m_Regnumber.GetLength()==0){
		GetPrivateProfileString(_T("Registration"),_T("NUMBER"),_T(""),m_Regnumber.GetBuffer(512),512,IniFile);
		m_Regnumber.ReleaseBuffer();
	}
*/
	if(-1 != CommandLine.Find(_T("/deletearchives "))) delete_archives=1;
	
	tPage1.AddDisks();

	
	char* ptr;
	char buf[0x500];
	int p;

	p=CommandLine.Find(_T("/p="));
	if(p!=-1){
		CString m=CommandLine.Mid(p+3);
		p=m.Find(" ");
		if(p!=-1)m=m.Left(p);
		if(GetFullPathName(m,0x500,buf,&ptr))
			if( 0==_access(buf,1))
				if(LoadProfile(buf)) goto nx2;
	}
	LoadProfile(prof);
nx2:

	if(-1 != CommandLine.Find(_T("/w ")))
		tPage3.m_Report=1;

	if(-1 != CommandLine.Find(_T("/n ")) )
		ShowWindow(SW_MINIMIZE);

	if(-1 != CommandLine.Find(_T("/s ")) )
		clStart=1;

	hVirlist=INVALID_HANDLE_VALUE;
	char* key="/virlist=";
	p=CommandLine.Find(key);
	if(p!=-1){
		CString m=CommandLine.Mid(p+strlen(key));
		p=m.Find(" ");
		if(p!=-1)m=m.Left(p);
		if(GetFullPathName(m,0x500,buf,&ptr))
		{
			hVirlist=CreateFile(buf,
			GENERIC_WRITE,FILE_SHARE_READ,NULL,
			CREATE_ALWAYS,0,NULL);
		}
		if(hVirlist==INVALID_HANDLE_VALUE)
			SendMessage(wm_AVP,AVP_CALLBACK_ERROR_FILE_OPEN,(long)(const char*)buf);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Andrey
	CString sPolicyFileName;
	do
	{
		p = CommandLine.Find(_T("/policy"));
		if (p != -1)
		{
			sPolicyFileName = CommandLine.Mid(p);
/*			p = sPolicyFileName.Find(_T("\""));
			if (p != -1)	// Name enclosed in '\'
			{
				sPolicyFileName = sPolicyFileName.Mid(p + 1);
				p = sPolicyFileName.Find(_T("\""));
				if (p != -1)
				{
					sPolicyFileName = sPolicyFileName.Left(p);
					goto Next;
				}
			}
			else			// Name 'as is'
*/
			{
				p = sPolicyFileName.Find(_T("="));
				if (p != -1)
				{
					sPolicyFileName = sPolicyFileName.Mid(p + 1);
					p = sPolicyFileName.Find(_T(" "));
					if (p != -1)
					{
						sPolicyFileName = sPolicyFileName.Left(p);
						goto Next;
					}
				}
			}
		}
		break;
Next:;
	} while(FALSE);
// Andrey
/////////////////////////////////////////////////////////////////////////////////////////////////

	::SetWindowLong( m_RepView, GWL_ID, IDC_REPV );
	m_RepView.BlockStringLocalization( IDS_LST_NOITEMSINVIEW );

	AvpRPTCortegeFlags rcCFlags;
	AvpRPTDomainFlags  rcDFlags1;
	AvpRPTDomainFlags  rcDFlags2;

	rcCFlags.m_nFlags = 0;
	rcCFlags.m_nType = RPT_CRTG_HEADER;

	rcDFlags1.m_nFlags = 0;
	rcDFlags1.m_nHWidthPercent = 70;
	rcDFlags2.m_nFlags = 0;
	rcDFlags2.m_nHWidthPercent = 30;

	CString rcString;
	rcString.Format( "%s\t%s\t<%x.%x.%x>",MS( IDS_LIST_ONAME ),MS( IDS_LIST_RESULT ), rcCFlags.m_nFlags, rcDFlags1.m_nFlags, rcDFlags2.m_nFlags );

	m_RepView.SetReportCortege( rcString );
//	m_RepView.SetAutoTracking(m_Track);


/*	
	m_RepView.SetRefreshingTimeout(250);
	m_RepView.AcceptReportFile(tPage3.m_Reportfile);
*/		
	
	AVP_RegisterCallback(m_hWnd);

	PostMessage(WM_COMMAND,ID_AVP_START,0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAvp32Dlg::OnDestroy()
{
	FreeLocLibrary();
	if (CCClientDone) CCClientDone();
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

void CAvp32Dlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	lpMMI->ptMinTrackSize=MinSize;
	CDialog::OnGetMinMaxInfo( lpMMI );
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAvp32Dlg::OnPaint() 
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
HCURSOR CAvp32Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


#define DIR_LIST 
// string buffer should be 0x200 and contain backslash
BOOL CountDirSpace(char* string, DWORDLONG* data, BOOL* kill)
{
#ifdef DIR_LIST
	CPtrList DirList;
#endif
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	int l;
	l=strlen(string);
	char* namePtr=string+l;
	if(l>0x1FA) return 0;
	strcpy(namePtr,"*.*");

	hFind = FindFirstFile(string, &FindData);
	if(hFind == INVALID_HANDLE_VALUE) return 0;
	do{
		if(*kill)
			break;
		if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

	    	if(!(FindData.cFileName[0]=='.'
				&& (FindData.cFileName[1]==0
				|| (FindData.cFileName[1]=='.' && FindData.cFileName[2]==0))))
			{	
				strcpy(namePtr,FindData.cFileName);
				if(!strlen(FindData.cFileName)||(DWORD)-1==GetFileAttributes(string))
				{
					strcpy(namePtr,FindData.cAlternateFileName);
					if(!strlen(FindData.cAlternateFileName)||(DWORD)-1==GetFileAttributes(string)) 
						continue;
				}
#ifdef DIR_LIST
	    	    DirList.AddTail( new CString(namePtr));
#else
				strcat(namePtr,"\\");
				CountDirSpace(string, data, kill);
#endif
			}
		}
		else if (FindData.nFileSizeLow != 0xFFFFFFFF)
		{
			EnterCriticalSection(&TotalSpaceCriticalSection);
				*data += FindData.nFileSizeLow;
			LeaveCriticalSection(&TotalSpaceCriticalSection);
		}
	}while( FindNextFile(hFind, &FindData));

	FindClose(hFind);

#ifdef DIR_LIST
    while(!DirList.IsEmpty()){
    	CString* name = (CString*) DirList.RemoveHead();
		if(!*kill){
			strcpy(namePtr,(const char*)(*name));
			strcat(namePtr,"\\");
			CountDirSpace(string, data, kill);
		}
    	delete (CString*)name;
    }
#endif

	return !*kill;
}


UINT Count( LPVOID pParam ){
	char s[0x200];
	strncpy(s,(char*)pParam,0x200);
	if(s[strlen(s)-1]!='\\')
		strcat(s,"\\");
	AvpmRegisterThread(1,os.dwPlatformId);
	
	CountDirSpace(s,&TotalSpace,&KillScan);

	AvpmRegisterThread(0,os.dwPlatformId);
	
	return 1;
}


BOOL AvpMoveFileEx(const char* clean, const char* dest)
{
	if( os.dwPlatformId==VER_PLATFORM_WIN32_NT
	 ||(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS && ((os.dwMajorVersion>4)||(os.dwMajorVersion==4 && os.dwMinorVersion>=90)))
	)
	{
		MoveFileEx(clean, dest, MOVEFILE_DELAY_UNTIL_REBOOT);
		return 1;
	}

	static CString batName="avp_cure";
	static CString pause="pause";
	static CString autoexec="C:\\autoexec.bat";
	static CString autoexec_save="C:\\autoexec.avp";
	
	CString avpCommand;
	avpCommand.Format("@if exist %s.bat call %s.bat",batName,batName);
	CStringArray sArray;
	CString s;
	CStdioFile ae;
	if(!ae.Open(autoexec,CFile::modeRead))
		return 0;
	while(ae.ReadString(s)){
		if(s==avpCommand) goto ae_clo;
		sArray.Add(s);
	}
	ae.Close();
	SetFileAttributes(autoexec,FILE_ATTRIBUTE_NORMAL);
	CopyFile(autoexec_save,"C:\\autoexec.avp",1);
	if(!ae.Open(autoexec, CFile::modeWrite))goto ae_end;
	ae.SeekToBegin();
	ae.WriteString(avpCommand);
	ae.WriteString("\n");
	while(sArray.GetSize()){
		ae.WriteString(sArray[0]);
		ae.WriteString("\n");
		sArray.RemoveAt(0);
	}
ae_clo:	ae.Close();
ae_end:

	char buf[0x200];
	char avp[0x200];
	if(!ae.Open("C:\\avp_cure.bat", CFile::modeCreate | CFile::modeNoTruncate  | CFile::modeReadWrite))return 0;
	GetModuleFileName(NULL,avp,0x200);
	int l=0;
	GetShortPathName(avp, buf+l,0x200-l);
	CharToOem(buf+l,buf+l);
	if(ae.GetLength()==0){
		ae.WriteString("@echo off\n");
		ae.WriteString(buf);
		ae.WriteString(" /LOGO\n");
		ae.SetLength(ae.GetPosition());

		ae.WriteString(buf);
		ae.WriteString(" /MOVE ");
		ae.WriteString(autoexec_save);
		ae.WriteString(" ");
		ae.WriteString(autoexec);
		ae.SetLength(ae.GetPosition());
	}
	if(dest)
	{
		if(_access(clean,4)){
			l=strlen(lstrcat(buf," /DEL "));
			GetShortPathName(dest,buf+l,0x200-l);
			CharToOem(buf+l,buf+l);
		}
		else{
			l=strlen(lstrcat(buf," /MOVE "));
			GetShortPathName(clean,buf+l,0x200-l);
			CharToOem(buf+l,buf+l);
			l=strlen(lstrcat(buf," "));
			GetShortPathName(dest,buf+l,0x200-l);
			CharToOem(buf+l,buf+l);
		}
	}
	else{
		l=strlen(lstrcat(buf," /DEL "));
		GetShortPathName(clean,buf+l,0x200-l);
		CharToOem(buf+l,buf+l);
	}

	int pos;
nx:	pos=ae.GetPosition();
	if(ae.ReadString(s)){
		if(s==pause)	
			ae.Seek(pos,CStdioFile::begin);	
		else if(s==buf)	
			goto ab_clo;	
		else
			goto nx;
	}
	ae.WriteString(buf);
	ae.WriteString("\n");
	ae.WriteString(pause);
	ae.WriteString("\n");
	s.Format("copy %%0 %s.avp > nul\ndel %%0",batName);
	ae.WriteString(s);
	ae.SetLength(ae.GetPosition());
ab_clo:
	ae.Close();
	return 1;
}




UINT Scan( LPVOID pParam ){
	KillScan=0;
	CAvp32Dlg* mDlg=(CAvp32Dlg*)pParam;
	AvpmRegisterThread(1,os.dwPlatformId);
	mDlg->Timer=SetTimer(mDlg->m_hWnd,1,250,NULL);

	UINT ret= mDlg->ScanAllPaths();
	
	if(LockedFiles.GetSize()){
		AVPScanObject ScanObject;
		while(LockedFiles.GetSize())
		{
			DWORD attr;
			CString name=LockedFiles.GetAt(0);
			LockedFiles.RemoveAt(0);

			memset(&ScanObject,0,sizeof(ScanObject));
			strcpy(ScanObject.Name,name);
			strcat(ScanObject.Name,".avp");
			if(!CopyFile(name,ScanObject.Name,FALSE))goto dfail;
			if((DWORD)-1==(attr=GetFileAttributes(ScanObject.Name)))goto dfail;
			if(attr&=~FILE_ATTRIBUTE_READONLY){
				attr&=~FILE_ATTRIBUTE_READONLY;
				if(!SetFileAttributes(ScanObject.Name,attr))goto dfail;
			}
			ScanObject.Type=OT_FILE;
			ScanObject.MFlags=MF_MASK_ALL|MF_PACKED;
			ScanObject.MFlags=(ScanObject.MFlags&~MF_D_)|MF_D_CUREALL;

			AVP_ProcessObject(&ScanObject);

			if(!(ScanObject.RFlags & RF_CURE)){
				DeleteFile(ScanObject.Name);
				goto dfail;
			}
			needReboot=1;

			AvpMoveFileEx(ScanObject.Name,name);
dfail:
			continue;
		}

	}

	KillTimer(mDlg->m_hWnd,mDlg->Timer);
	mDlg->PostMessage(WM_COMMAND,ID_AVP_SCANEND);

	if(CCClientSendStatus)
		CCClientSendStatus(KillScan?TS_ABORTED:TS_DONE);

	AvpmRegisterThread(0,os.dwPlatformId);
	return ret;
}


void CAvp32Dlg::OnScanNow() 
{
	if(ScanThreadPtr || VirThreadPtr)return;
	UpdateData();

	int size=ScanList.GetSize();
	if(!size){
		size=PathArray.GetSize();
		for(int i=0; i<size; i++)
			if(PathArray[i][0]!='*')ScanList.Add(PathArray[i]);
		size=ScanList.GetSize();
	}
	
	if(tPage3.m_Redundant && !clStart){
		if(cust.m_CustRedundantMessage)
		if(IDYES!=MessageBox(MS(IDS_ASK_REDUNDANT),AppName,MB_YESNO|MB_ICONINFORMATION))
		{
			m_Tab1.SetCurSel(3);
			long r;
			OnSelchangeTab1(NULL, &r);
			return;
		}
	}

	if((tPage6.m_Action)==3){
		if(cust.m_CustDeleteAllMessage)
		if(IDYES!=MessageBox(MS(IDS_SHURE_DELETEALL),AppName,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION))
		{
			m_Tab1.SetCurSel(2);
			long r;
			OnSelchangeTab1(NULL, &r);
			return;
		}
	}

	if( !tPage2.m_Files && !tPage2.m_Sectors && !tPage2.m_Memory && !clStart)
	{
		if(cust.m_CustOtherMessages)
		{
			MessageBox(MS(IDS_NOTHING_FILES),AppName,MB_OK|MB_ICONINFORMATION);
			return;
		}
	}	
	if( !size && !clStart)
	{
		if(cust.m_CustOtherMessages)
		{
			MessageBox(MS(IDS_NOTHING_LOCATION),AppName,MB_OK|MB_ICONINFORMATION);
		}
	}	


	m_FPInfectFolder="";
	if(strnicmp((const char*)tPage6.m_sInfectFolder+1,":\\",2) && strnicmp(tPage6.m_sInfectFolder,"\\\\",2))
	{
		m_FPInfectFolder=StartPath;
		if(m_FPInfectFolder[m_FPInfectFolder.GetLength()-1]!='\\')m_FPInfectFolder+='\\';
	}
	m_FPInfectFolder+=tPage6.m_sInfectFolder;	
	if(tPage6.m_CopyInfected && _access(m_FPInfectFolder,00))
		CreateDirectory(m_FPInfectFolder,NULL);

	
	m_FPSuspFolder="";
	if(strnicmp((const char*)tPage6.m_sSuspFolder+1,":\\",2) && strnicmp(tPage6.m_sSuspFolder,"\\\\",2))
	{
		m_FPSuspFolder=StartPath;
		if(m_FPSuspFolder[m_FPSuspFolder.GetLength()-1]!='\\')m_FPSuspFolder+='\\';
	}
	m_FPSuspFolder+=tPage6.m_sSuspFolder;	
	if(tPage6.m_CopySuspicion && _access(m_FPSuspFolder,00))
		CreateDirectory(m_FPSuspFolder,NULL);



	tPage5.Flush();
	tPage5.UpdateData(FALSE);
	VirCountArray.RemoveAll();
//	m_List.DeleteAllItems();

	m_RepView.ClearReportView();
	m_RepView.SetAutoTracking(m_Track);
	m_tbToolbar.GetToolBarCtrl().CheckButton(ID_TRACK, m_Track);


	DisableControls(TRUE);
	ExitCode=1;
	StatusLine(MS( IDS_SCANPROC_SPAWN ));
	m_Progress.SetPos(0);
	LastAction=0;
	ApplyToAll=0;
	MethodDelete=0;
	SectorWarn=0;

	m_AnimateScan.Close();
	if(!m_AnimateScan.Open("search.avi"))
	m_AnimateScan.Open(IDR_AVI_SEARCH);
	m_AnimateScan.Play(0,(UINT)-1,(UINT)-1);
	m_AnimateScan.ShowWindow(SW_SHOW);

	if(cust.m_CustStatAfterBegin)
	{
		m_Tab1.SetCurSel(m_Tab1.GetItemCount()-1);
		long r;
		OnSelchangeTab1(NULL, &r);
//		if(m_List.GetItemCount())m_List.SetFocus();
//		m_RepView.Activate();
	}
	OpenReportFile();

	KillScan=0;
	EnterCriticalSection(&TotalSpaceCriticalSection);
		TotalSpace=0;
	LeaveCriticalSection(&TotalSpaceCriticalSection);
	if(tPage2.m_Memory){
		EnterCriticalSection(&TotalSpaceCriticalSection);
			TotalSpace+=1024;
		LeaveCriticalSection(&TotalSpaceCriticalSection);
	}
	for(int i=0; i< ScanList.GetSize(); i++){
		if(tPage2.m_Sectors){
			EnterCriticalSection(&TotalSpaceCriticalSection);
				TotalSpace+=512;
			LeaveCriticalSection(&TotalSpaceCriticalSection);
		}
		if(tPage2.m_Files && !ReportWithTiming)
			AfxBeginThread(Count,(LPVOID)(const char*)ScanList[i],THREAD_PRIORITY_NORMAL+tPage3.m_Priority-2);
	}

	ScanThreadPtr=AfxBeginThread(Scan,this,THREAD_PRIORITY_NORMAL+tPage3.m_Priority-2);

}



void CAvp32Dlg::OnAvpScanEnd() 
{
	CString s;
	ScanThreadPtr=NULL;
	if(should_reload)
		SendMessage(WM_COMMAND,ID_AVP_RELOADBASES,0);


	if(cust.m_CustStatAfterFinish)
	{
		m_Tab1.SetCurSel(m_Tab1.GetItemCount()-1);
		long r;
		OnSelchangeTab1(NULL, &r);
//		if(m_List.GetItemCount())m_List.SetFocus();
		m_RepView.Activate();
	}


	m_AnimateScan.Stop();
	if(tPage5.m_Bodies != tPage5.m_Disinfected){
		m_AnimateScan.Close();
		if(!m_AnimateScan.Open("infected.avi"))
		m_AnimateScan.Open(IDR_AVI_INFECTED);
		m_AnimateScan.Play(0,(UINT)-1,1);
	}
	else if(KillScan){
		m_AnimateScan.ShowWindow(SW_HIDE);
	}
	else{
		m_AnimateScan.Close();
		if(!m_AnimateScan.Open("ok.avi"))
		m_AnimateScan.Open(IDR_AVI_OK);
		m_AnimateScan.Play(0,(UINT)-1,1);
	}
	PutReportString(_T(""));

	if(!KillScan)
	{
		m_Progress.SetPos(100);
		StatusLine(MS( IDS_SCANPROC_COMPLETE ));
		PutReportString(MS( IDS_SCANPROC_COMPLETE ));
		ExitCode=0;
		if(tPage5.m_Corrupted)	ExitCode=8;
		if(tPage5.m_Suspicious)	ExitCode=3;
		if(tPage5.m_Warnings)	ExitCode=2;
		if(tPage5.m_Bodies){
			if(tPage5.m_Bodies == tPage5.m_Disinfected)
				ExitCode=5;
			else if(tPage5.m_Bodies == tPage5.m_Deleted)
				ExitCode=6;
			else
				ExitCode=4;
		}

	}else{
		StatusLine(MS( IDS_SCANPROC_INCOMPLETE ));
		PutReportString(MS( IDS_SCANPROC_INCOMPLETE ));
		ExitCode=1;
		if(tPage5.m_Bodies){
				ExitCode=4;
		}
	}


	MakeReport(s);
	if(CCClientSendStatistics)
		CCClientSendStatistics(s);
	if(hReportFile!=INVALID_HANDLE_VALUE)
	{
		ULONG bread;
		WriteFile(hReportFile,"\xd\xa",2,&bread,NULL);
		WriteFile(hReportFile,s,s.GetLength(),&bread,NULL);
		CloseHandle(hReportFile);
		hReportFile=INVALID_HANDLE_VALUE;
	}

	DisableControls(FALSE);
	UpdateData(FALSE);

	if(needReboot){
		needReboot=0;
		if(IDYES==GetDesktopWindow()->MessageBox(MS( IDS_NEED_REBOOT ),AppName,MB_YESNO|MB_ICONSTOP|MB_SYSTEMMODAL))
		{

			if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)
			{
				HANDLE hToken;              /* handle to process token */  
				TOKEN_PRIVILEGES tkp;        /* ptr. to token structure */ 
				if (!OpenProcessToken(GetCurrentProcess(), 
					TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
					goto ex;
				LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
 				tkp.PrivilegeCount = 1;  /* one privilege to set    */ 
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 				AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
			}
ex:
			PostMessage(WM_CLOSE);
			ExitWindowsEx(EWX_REBOOT/*|EWX_FORCE*/,0);
		}
	}
////////////////////////////////////////////////////////////////////////////////////
// Andrey
	if (boProccessAndExit)
	{
		PostMessage(WM_CLOSE);
	}
// Andrey
////////////////////////////////////////////////////////////////////////////////////

	if(-1 != CommandLine.Find(_T("/q "))){
		PostMessage(WM_CLOSE);
	}
	else{
		if(cust.m_CustPopupAfterFinish)
		{
			if(IsIconic())ShowWindow(SW_SHOWNORMAL);
			SetForegroundWindow();
		}
		if(cust.m_Sound){
			if(!sndPlaySound("finish.wav", SND_ASYNC))
				MessageBeep(MB_OK);
		}
	}

}


void CAvp32Dlg::OnStop() 
{
	if(VirThreadPtr){
		CancelProcess();
		return;
	}
	if(ScanThreadPtr==NULL)return;
	ScanThreadPtr->SuspendThread();
	if(CCClientSendStatus)
		CCClientSendStatus(TS_PAUSED);

	m_AnimateScan.Stop();
	if(IDYES==MessageBox(MS( IDS_SCANPROC_QUESTION ),AppName,MB_YESNO|MB_ICONSTOP))
		CancelProcess();

	m_AnimateScan.Play((UINT)-1,(UINT)-1,(UINT)-1);
	ScanThreadPtr->ResumeThread();
	if(CCClientSendStatus)
		CCClientSendStatus(TS_RUNNING);
}



void CAvp32Dlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TC_ITEM TabItem;
	CWnd* dlgWnd;
	int Sel = m_Tab1.GetCurSel();

	m_Tab1.SetFocus();

	for(int i=0; i<m_Tab1.GetItemCount(); i++){
		TabItem.mask=TCIF_PARAM;
		m_Tab1.GetItem(i,&TabItem);
		dlgWnd = (CWnd*)(TabItem.lParam);
		dlgWnd->UpdateData();
		dlgWnd->ModifyStyleEx( (Sel==i)?0:WS_EX_CONTROLPARENT, (Sel==i)?WS_EX_CONTROLPARENT:0 );
		if(Sel==i){
			dlgWnd->SetWindowPos( &wndTop,0,0,0,0,
			SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			UINT id;
			switch(Sel){
			case 1: id=IDD_TAB_PAGE2;break;
			case 2: id=IDD_TAB_PAGE6;break;
			case 3: id=IDD_TAB_PAGE3;break;
			case 4: id=IDD_TAB_PAGE5;break;
			default:id=IDD_TAB_PAGE1;break;
			}
			SetHelpID(id);
		}
		else{
			dlgWnd->SetWindowPos( &wndBottom,0,0,0,0,
			SWP_NOACTIVATE | SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		}
	}
	
	*pResult = 0;
}

#define LBT 5
#define LT 3

void CAvp32Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	static RULE rules[] = {

      // Action    Act-on                   Relative-to           Offset
      // ------    ------                   -----------           ------
//		{  lMOVE,    lLEFT  (IDC_ML),  lLEFT     (lPARENT)     ,0  },
		{  lSTRETCH, lRIGHT (IDC_ML),  lRIGHT    (lPARENT)     ,0  },
//		{  lMOVE,    lTOP   (IDC_ML),  lTOP      (lPARENT)     ,+0  },
//		{  lSTRETCH, lBOTTOM(IDC_ML),  lTOP      (lPARENT)     ,+1  },

		{  lMOVE,    lRIGHT (IDC_SCAN_NOW),   lRIGHT    (lPARENT),     -LBT  },
//		{  lMOVE,    lTOP   (IDC_SCAN_NOW),   lTOP      (lPARENT)     ,+10  },
//		{  lSTRETCH, lWIDTH (IDC_SCAN_NOW),   lWIDTHOF  (lPARENT, 15), +0  },

//		{  lMOVE,    lRIGHT (IDC_STOP),       lRIGHT    (IDC_SCAN_NOW),+0  },
//		{  lMOVE,    lTOP   (IDC_STOP),       lBOTTOM   (IDC_SCAN_NOW),+LBT  },
//		{  lSTRETCH, lWIDTH (IDC_STOP),       lWIDTH    (IDC_SCAN_NOW),+0  },

		{  lMOVE,    lLEFT  (IDC_TAB1),       lLEFT     (lPARENT)     ,+LT  },
//		{  lMOVE,    lTOP   (IDC_TAB1),       lTOP      (lPARENT)     ,+LBT  },
		{  lSTRETCH, lRIGHT (IDC_TAB1),       lLEFT     (IDC_SCAN_NOW),-LBT  },
//		{  lSTRETCH, lHEIGHT(IDC_TAB1),       lHEIGHTOF (lPARENT, 50), +0  },

		{  lMOVE,    lLEFT  (IDC_STAT_LINE),  lLEFT     (lPARENT)     ,+LT  },
		{  lSTRETCH, lRIGHT (IDC_STAT_LINE),  lRIGHT    (IDC_TAB1)    ,0  },
		{  lMOVE,    lBOTTOM(IDC_STAT_LINE),  lBOTTOM   (lPARENT)     ,-LT  },
//		{  lSTRETCH, lTOP   (IDC_STAT_LINE),  lBOTTOM   (IDC_TAB1)    ,-LT  },

		{  lMOVE,    lLEFT  (IDC_PROGRESS),      lRIGHT  (IDC_STAT_LINE)     ,+LT  },
		{  lMOVE,    lTOP   (IDC_PROGRESS),      lTOP   (IDC_STAT_LINE)    ,0  },
		{  lSTRETCH, lRIGHT (IDC_PROGRESS),      lRIGHT (lPARENT)    ,-LT  },

//		{  lMOVE,    lRIGHT (IDC_REPVIEW),    lRIGHT    (lPARENT),     -LBT  },
//		{  lMOVE,    lTOP   (IDC_REPVIEW),    lTOP    (IDC_PROGRESS),     0  },

		{  lMOVE,    lTOP   (IDC_TB),    lBOTTOM   (IDC_TAB1)    ,+LT  },
		{  lSTRETCH, lBOTTOM(IDC_TB),    lTOP    (IDC_STAT_LINE) ,-LT  },
		{  lMOVE,    lRIGHT (IDC_TB),    lRIGHT    (lPARENT)     ,-LT  },
/*
		{  lMOVE,    lTOP   (IDC_REPV),      lTOP    (IDC_TB)    ,0  },
		{  lSTRETCH, lBOTTOM(IDC_REPV),      lBOTTOM (IDC_TB) ,0  },
		{  lMOVE,    lLEFT  (IDC_REPV),      lLEFT     (lPARENT)     ,+LT  },
		{  lSTRETCH, lRIGHT (IDC_REPV),      lLEFT   (IDC_TB)     ,-LBT  },
*/
		{  lMOVE,    lLEFT (IDC_ANIMATE_SCAN),lLEFT   (IDC_SCAN_NOW), +2*LBT  },
		{  lMOVE,    lBOTTOM(IDC_ANIMATE_SCAN),lBOTTOM  (IDC_TAB1),	-3*LBT  },

//		{  lMOVE,    lLEFT	(IDC_TRACK),	lLEFT     (IDC_SCAN_NOW)  ,+0  },
//		{  lMOVE,    lBOTTOM(IDC_TRACK),	lBOTTOM   (IDC_TAB1)      ,+0 },
//		{  lSTRETCH, lRIGHT	(IDC_TRACK),	lRIGHT	  (lPARENT) 	  ,-LT },

		{  lEND                                                          }
   };

   // Place the dialog's child controls according to the rules. If there is
   // a problem with the rules, Layout_ComputeLayout will assert.

	Layout_ComputeLayout(m_hWnd, rules);

	CRect ca;
	m_Tab1.GetWindowRect(ca);
	ScreenToClient(ca);
	ca.left+=5;
	ca.right-=5;
	ca.bottom-=5;
	ca.top+=25;

	for(int i=0; i<m_Tab1.GetItemCount(); i++){
		TC_ITEM TabItem;
		CWnd* dlgWnd;
		TabItem.mask=TCIF_PARAM;
		m_Tab1.GetItem(i,&TabItem);
		dlgWnd = (CWnd*)(TabItem.lParam);
		dlgWnd->MoveWindow(ca,TRUE);
	}

	CRect R;
	if (m_tbToolbar.m_hWnd)
	{
		GetDlgItem(IDC_TB)->GetClientRect(&R);
		GetDlgItem(IDC_TB)->ClientToScreen(&R);
		ScreenToClient(&R);
		m_tbToolbar.SetWindowPos(this, R.left, R.top, R.right - R.left, R.bottom - R.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	if (m_RepView.m_hWnd)
	{
		CRect R1;
		GetDlgItem(IDC_TAB1)->GetClientRect(&R1);
		GetDlgItem(IDC_TAB1)->ClientToScreen(&R1);
		ScreenToClient(&R1);
		m_RepView.SetWindowPos(this, R1.left, R.top, R.left - R1.left - LBT, R.bottom - R.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}

}


void CAvp32Dlg::OnAvpChangetab() 
{
	m_Tab1.SetCurSel((m_Tab1.GetCurSel()+1)%m_Tab1.GetItemCount());
	long r;
	OnSelchangeTab1(NULL, &r);
}


void CAvp32Dlg::ObjectMessage(AVPScanObject* ScanObject,const char* s, BOOL noCheckDone, DWORD IpcMess) 
{
	if(ScanObject==NULL){
		PutLogString(s,"");
		return;
	}
	if(!noCheckDone && (ScanObject->RFlags & RF_REPORT_DONE)) return;
	ScanObject->RFlags|=RF_REPORT_DONE;
	CString Name;
	if(ScanObject->Entry){
		Name.Format(MS( IDS_ENTRY_FORMAT ),ScanObject->Name,ScanObject->Entry);
	}
	else{
		Name=ScanObject->Name;
	}
	if(IpcMess){
		CString mess=Name;
		mess+="   ";
		mess+=s;
		if(CCClientSendAlert)
			CCClientSendAlert(IpcMess,mess);
	}
	if(ReportWithTiming && ScanObject->RefData)
	{
		DWORD tick;
		if(HiResCounterSupported){
			LARGE_INTEGER PerfCount;
			QueryPerformanceCounter(&PerfCount);
			tick=PerfCount.LowPart;
		}else
			tick=GetTickCount();

		if(ScanObject->RefData > tick) tick+=(~ScanObject->RefData)+1;
		else tick-=ScanObject->RefData;
		CString cs;
		cs.Format("%s\x9%d",s,tick);
		PutLogString(Name,cs);
	}else
	{
		PutLogString(Name,s);
	}

}

LONG CAvp32Dlg::OnAvpupd( UINT uParam, LONG lParam)
{
	PostMessage(WM_COMMAND,ID_AVP_RELOADBASES,0);
	return 1;
}
CDisinfectDlg* pddlg=NULL;


DWORD COFF_ParseNames(char* s, char* src, void* buf, DWORD size)
{
	DWORD ret=0;
	*s=0;
	*src=0;

	COFF_Header* Header=(COFF_Header*)buf;
	COFF_Symbol* Symbol=(COFF_Symbol*)((BYTE*)buf+Header->P_SymbolTable);;
	
	for(DWORD i=0;i<Header->N_Symbols;i+=Symbol[i].Skip, i++){
		if(Symbol[i].Type == 0x67){
			strcpy(src,(char*)(Symbol+i+1));
			continue;
		}
			
		if(Symbol[i].Type != 0x02) continue; //Not code
		if(Symbol[i].Section == 0) continue; //External
		if((Symbol[i].Access & 0x20) == 0)continue; //Not function

		CString sn;
		if(Symbol[i].NameWord ==0)
			sn=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
		else{
			sn=Symbol[i].Name;
			if(sn.GetLength()>8)
				sn=sn.Left(8);
		}
		if(!strcmp(sn,"_decode"))
			ret|=1;
		if(!strcmp(sn,"_cure"))
			ret|=2;
		if(!strcmp(sn,"_jmp"))
			ret|=4;
		strcat(s,sn);
		strcat(s," ");
	}
	
	return ret;
}

#ifdef AVE_SQL
BOOL AddRecordCheck( DWORD idx1,DWORD idx2, DWORD type, const char* name)
{
	BOOL ret=AddRecord( idx1,idx2);
	if( !ret ){
		char s[0x200];
		sprintf(s,"Record already exist: %08x %08x %x %s\n",idx1,idx2,type,name);
		OutputDebugString(s);
#ifdef _DEBUG
///		__asm int 3;
#endif
	}
	return ret;
}
#endif

LONG CAvp32Dlg::OnAVPMessage( UINT uParam, LONG lParam){
	LONG ret=0;
	CString s;
	int i;
	char tempb[0x200];
	AVPScanObject* ScanObject=(AVPScanObject*)lParam;

	switch(uParam){
#ifdef AVE_SQL
	case AVP_CALLBACK_WA_RECORD:
//  AVP32.EXE /ave_sql=i      if i&1 - insert on load, if i&2 - insert signatures if i&16 - delete base before

		{
			char VName[0x200];
			char src[0x200];
			DWORD idx1;
			DWORD idx2;
			DWORD dw;
			CWorkArea* wa=(CWorkArea*)(lParam);
					
			switch(wa->CurRecordType){
			case RT_KERNEL:

				if(wa->CurRecordLinkObj)
				{
					R_Kernel* rp=(R_Kernel*)wa->CurRecord;
					dw=COFF_ParseNames(VName, src, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
					idx1=CalcSum((BYTE*)src,strlen(src));
					idx2=wa->CurRecordType;
					if(wa->CurRecordStatus == 1  && AddRecordCheck(idx1,idx2,dw=wa->CurRecordType,"kernel")){
						InsertField(idx1,idx2, FIELD_TYPE, (void*)&wa->CurRecordType, sizeof(wa->CurRecordType));
						InsertField(idx1,idx2, FIELD_LINK_SOURCE_NAME, (void*)src, strlen(src)+1);
						InsertField(idx1,idx2, FIELD_LINK_EXPORTS, (void*)VName, strlen(VName)+1);
						InsertField(idx1,idx2, FIELD_LINK_ENTRY, (void*)&dw, sizeof(dw));
						InsertField(idx1,idx2, FIELD_LINK_OBJ, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
					}
				}
				break;
			case RT_JUMP:
				{
					R_Jump* rp=(R_Jump*)wa->CurRecord;
					idx1=dw=CalcSum((BYTE*)wa->CurRecord, 15 );
					idx2=wa->CurRecordType;
					if(wa->CurRecordStatus == 1  && AddRecordCheck(idx1,idx2,dw=wa->CurRecordType,"jmp")){
						dw=wa->CurRecordType;
						InsertField(idx1,idx2, FIELD_TYPE, (void*)&dw, sizeof(dw));
						dw=rp->Len1;
						InsertField(idx1,idx2, FIELD_LENGTH_1, (void*)&dw, sizeof(dw));
						if(dw){
							dw=rp->BitMask1;
							InsertField(idx1,idx2, FIELD_J_BITMASK_1, (void*)&dw, sizeof(dw));
							dw=rp->BitMask2;
							InsertField(idx1,idx2, FIELD_J_BITMASK_2, (void*)&dw, sizeof(dw));
							dw=rp->Sum1;
							InsertField(idx1,idx2, FIELD_SUM_1, (void*)&dw, sizeof(dw));
						}
						if(dw=rp->Method)	InsertField(idx1,idx2, FIELD_J_METHOD, (void*)&dw, sizeof(dw));
						if(dw=rp->Data1)	InsertField(idx1,idx2, FIELD_J_DATA_1, (void*)&dw, sizeof(dw));
						if(dw=rp->Data2)	InsertField(idx1,idx2, FIELD_J_DATA_2, (void*)&dw, sizeof(dw));
						if(dw=rp->Data3)	InsertField(idx1,idx2, FIELD_J_DATA_3, (void*)&dw, sizeof(dw));

						if(wa->CurRecordLinkObj){
							dw=COFF_ParseNames(VName, src, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
							InsertField(idx1,idx2, FIELD_LINK_SOURCE_NAME, (void*)src, strlen(src)+1);
							InsertField(idx1,idx2, FIELD_LINK_EXPORTS, (void*)VName, strlen(VName)+1);
							InsertField(idx1,idx2, FIELD_LINK_ENTRY, (void*)&dw, sizeof(dw));
							InsertField(idx1,idx2, FIELD_LINK_OBJ, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
						}
					}
				}
				break;

			case RT_MEMORY:
				{
					R_Memory* rp=(R_Memory*)wa->CurRecord;
					idx1=CalcSum((BYTE*)wa->CurRecord, 11 );
					idx2=wa->CurRecordType ^ CalcSum((BYTE*)(const char*)rp->NameIdx, strlen((const char*)rp->NameIdx));
					if(wa->CurRecordStatus == 1  && AddRecord(idx1,idx2)){
						AVP_MakeFullName(VName,(const char*)rp->NameIdx);
						InsertField(idx1,idx2, FIELD_FULL_NAME, (void*)VName, strlen(VName)+1);
						InsertField(idx1,idx2, FIELD_NAME, (void*)rp->NameIdx, strlen((const char*)rp->NameIdx)+1);
						dw=wa->CurRecordType;
						InsertField(idx1,idx2, FIELD_TYPE, (void*)&dw, sizeof(dw));
						dw=rp->Method;
						InsertField(idx1,idx2, FIELD_M_METHOD, (void*)&dw, sizeof(dw));
						dw=rp->Len1;
						InsertField(idx1,idx2, FIELD_M_LENGTH, (void*)&dw, sizeof(dw));
						if(dw){
							dw=rp->Offset;
							InsertField(idx1,idx2, FIELD_M_OFFSET, (void*)&dw, sizeof(dw));
							dw=rp->Segment;
							InsertField(idx1,idx2, FIELD_M_SEGMENT, (void*)&dw, sizeof(dw));
							dw=rp->Sum1;
							InsertField(idx1,idx2, FIELD_M_SUM, (void*)&dw, sizeof(dw));
						}
						dw=rp->CureLength; InsertField(idx1,idx2, FIELD_M_CURE_LENGTH, (void*)&dw, sizeof(dw));
						if(dw){
							dw=rp->CureOffset; InsertField(idx1,idx2, FIELD_M_CURE_OFFSET, (void*)&dw, sizeof(dw));
							InsertField(idx1,idx2, FIELD_M_CURE_DATA, (void*)rp->CureData, rp->CureLength);
						}
							
						if(wa->CurRecordLinkObj){
							dw=COFF_ParseNames(VName, src, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
							InsertField(idx1,idx2, FIELD_LINK_SOURCE_NAME, (void*)src, strlen(src)+1);
							InsertField(idx1,idx2, FIELD_LINK_EXPORTS, (void*)VName, strlen(VName)+1);
							InsertField(idx1,idx2, FIELD_LINK_ENTRY, (void*)&dw, sizeof(dw));
							InsertField(idx1,idx2, FIELD_LINK_OBJ, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
						}
					}
				}
				break;
						
			case RT_SECTOR:
				{
					R_Sector* rp=(R_Sector*)wa->CurRecord;
					idx1=CalcSum((BYTE*)wa->CurRecord, 16 );
					idx2=wa->CurRecordType ^ CalcSum((BYTE*)(const char*)rp->NameIdx, strlen((const char*)rp->NameIdx));
					if(wa->CurRecordStatus == 2){
						if(InsertField(idx1,idx2, FIELD_DETECT_FILENAME, (void*)(wa->ScanObject->Name), strlen(wa->ScanObject->Name)+1))
						{
							if(rp->Len1)InsertField(idx1,idx2, FIELD_SIGNATURE_1, (void*)(wa->Header + rp->Off1), rp->Len1);
							if(rp->Len2)InsertField(idx1,idx2, FIELD_SIGNATURE_2, (void*)(wa->Header + rp->Off2), rp->Len2);
						}
					}
					if(wa->CurRecordStatus == 1  && AddRecordCheck(idx1,idx2,dw=wa->CurRecordType,(const char*)rp->NameIdx)){
						AVP_MakeFullName(VName,(const char*)rp->NameIdx);
						InsertField(idx1,idx2, FIELD_FULL_NAME, (void*)VName, strlen(VName)+1);
						InsertField(idx1,idx2, FIELD_NAME, (void*)rp->NameIdx, strlen((const char*)rp->NameIdx)+1);
						dw=wa->CurRecordType;
						InsertField(idx1,idx2, FIELD_TYPE, (void*)&dw, sizeof(dw));

						dw=rp->Len1;
						InsertField(idx1,idx2, FIELD_LENGTH_1, (void*)&dw, sizeof(dw));
						if(dw){
							dw=rp->Off1;
							InsertField(idx1,idx2, FIELD_OFFSET_1, (void*)&dw, sizeof(dw));
							dw=rp->Sum1;
							InsertField(idx1,idx2, FIELD_SUM_1, (void*)&dw, sizeof(dw));
						}
						dw=rp->Len2;
						InsertField(idx1,idx2, FIELD_LENGTH_2, (void*)&dw, sizeof(dw));
						if(dw){
							dw=rp->Off2;
							InsertField(idx1,idx2, FIELD_OFFSET_2, (void*)&dw, sizeof(dw));
							dw=rp->Sum2;
							InsertField(idx1,idx2, FIELD_SUM_2, (void*)&dw, sizeof(dw));
						}
						if(wa->CurRecordLinkObj){
							dw=COFF_ParseNames(VName, src, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
							InsertField(idx1,idx2, FIELD_LINK_SOURCE_NAME, (void*)src, strlen(src)+1);
							InsertField(idx1,idx2, FIELD_LINK_EXPORTS, (void*)VName, strlen(VName)+1);
							InsertField(idx1,idx2, FIELD_LINK_ENTRY, (void*)&dw, sizeof(dw));
							InsertField(idx1,idx2, FIELD_LINK_OBJ, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
						}
						
						if(dw= 0xF & rp->CureObjAndMethod1) InsertField(idx1,idx2, FIELD_S_CURE_METHOD_1, (void*)&dw, sizeof(dw));
						if(dw= 0xF0& rp->CureObjAndMethod1) InsertField(idx1,idx2, FIELD_S_CURE_OBJECT_1, (void*)&dw, sizeof(dw));
						if(dw=rp->CureAddr11)	InsertField(idx1,idx2, FIELD_S_CURE_ADDRESS_11, (void*)&dw, sizeof(dw));
						if(dw=rp->CureAddr12)	InsertField(idx1,idx2, FIELD_S_CURE_ADDRESS_12, (void*)&dw, sizeof(dw));
						if(dw=rp->CureData1)	InsertField(idx1,idx2, FIELD_S_CURE_DATA_1, (void*)&dw, sizeof(dw));
						if(dw= 0xF & rp->CureObjAndMethod2) InsertField(idx1,idx2, FIELD_S_CURE_METHOD_2, (void*)&dw, sizeof(dw));
						if(dw= 0xF0& rp->CureObjAndMethod2) InsertField(idx1,idx2, FIELD_S_CURE_OBJECT_2, (void*)&dw, sizeof(dw));
						if(dw=rp->CureAddr21)	InsertField(idx1,idx2, FIELD_S_CURE_ADDRESS_21, (void*)&dw, sizeof(dw));
						if(dw=rp->CureAddr22)	InsertField(idx1,idx2, FIELD_S_CURE_ADDRESS_22, (void*)&dw, sizeof(dw));
						if(dw=rp->CureData2)	InsertField(idx1,idx2, FIELD_S_CURE_DATA_2, (void*)&dw, sizeof(dw));
					}
				}
				break;
					
			case RT_FILE:
			case RT_CA:
			case RT_UNPACK:
			case RT_EXTRACT:
				{
					if(wa->CurRecordStatus == 2)
					{
						R_File_Cutted* rp=(R_File_Cutted*)wa->CurRecord;
						idx1=CalcSum((BYTE*)(&rp->Type), 12 );
						idx2=wa->CurRecordType ^ CalcSum((BYTE*)(const char*)rp->NameIdx, strlen((const char*)rp->NameIdx));
						if(wa->CurRecordType == RT_FILE){
							if(InsertField(idx1,idx2, FIELD_DETECT_FILENAME, (void*)(wa->ScanObject->Name), strlen(wa->ScanObject->Name)+1))
							{
								if(rp->Len1)InsertField(idx1,idx2, FIELD_SIGNATURE_1, (void*)(wa->Header + rp->Off1), rp->Len1);
								if(rp->Len2)InsertField(idx1,idx2, FIELD_SIGNATURE_2, (void*)(wa->Header + rp->Off2), rp->Len2);
							}
						}
						else if(rp->Len1 || rp->Len2){
							if(InsertField(idx1,idx2, FIELD_DETECT_FILENAME, (void*)(wa->ScanObject->Name), strlen(wa->ScanObject->Name)+1))
							{
								if(rp->Len1)InsertField(idx1,idx2, FIELD_SIGNATURE_1, (void*)(wa->Header + rp->Off1), rp->Len1);
								if(rp->Len2)InsertField(idx1,idx2, FIELD_SIGNATURE_2, (void*)(wa->Header + rp->Off2), rp->Len2);
							}
						}
					}
					else if(wa->CurRecordStatus == 1 )
					{
						R_File* rp=(R_File*)wa->CurRecord;
						idx1=CalcSum((BYTE*)(&rp->Type), 12 );
						idx2=wa->CurRecordType ^ CalcSum((BYTE*)(const char*)rp->NameIdx, strlen((const char*)rp->NameIdx));

						if(AddRecordCheck(idx1,idx2,dw=wa->CurRecordType,(const char*)rp->NameIdx))
						{
							AVP_MakeFullName(VName,(const char*)rp->NameIdx);
							InsertField(idx1,idx2, FIELD_FULL_NAME, (void*)VName, strlen(VName)+1);
							InsertField(idx1,idx2, FIELD_NAME, (void*)rp->NameIdx, strlen((const char*)rp->NameIdx)+1);
							dw=wa->CurRecordType;
							InsertField(idx1,idx2, FIELD_TYPE, (void*)&dw, sizeof(dw));
							dw=rp->Type;
							InsertField(idx1,idx2, FIELD_SUB_TYPE, (void*)&dw, sizeof(dw));
							
							dw=rp->Len1;
							InsertField(idx1,idx2, FIELD_LENGTH_1, (void*)&dw, sizeof(dw));
							if(dw){
								dw=rp->Off1;
								InsertField(idx1,idx2, FIELD_OFFSET_1, (void*)&dw, sizeof(dw));
								dw=rp->Sum1;
								InsertField(idx1,idx2, FIELD_SUM_1, (void*)&dw, sizeof(dw));
							}
							dw=rp->Len2;
							InsertField(idx1,idx2, FIELD_LENGTH_2, (void*)&dw, sizeof(dw));
							if(dw){
								dw=rp->Off2;
								InsertField(idx1,idx2, FIELD_OFFSET_2, (void*)&dw, sizeof(dw));
								dw=rp->Sum2;
								InsertField(idx1,idx2, FIELD_SUM_2, (void*)&dw, sizeof(dw));
							}
							if(wa->CurRecordLinkObj){
								dw=COFF_ParseNames(VName, src, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
								InsertField(idx1,idx2, FIELD_LINK_SOURCE_NAME, (void*)src, strlen(src)+1);
								InsertField(idx1,idx2, FIELD_LINK_EXPORTS, (void*)VName, strlen(VName)+1);
								InsertField(idx1,idx2, FIELD_LINK_ENTRY, (void*)&dw, sizeof(dw));
								InsertField(idx1,idx2, FIELD_LINK_OBJ, wa->CurRecordLinkObj, wa->CurRecordLinkObjSize);
							}
							if(wa->CurRecordType==RT_FILE){
								if(dw=rp->CureMethod)	InsertField(idx1,idx2, FIELD_F_CURE_METHOD, (void*)&dw, sizeof(dw));
								if(dw=rp->CureData1)	InsertField(idx1,idx2, FIELD_F_CURE_DATA_1, (void*)&dw, sizeof(dw));
								if(dw=rp->CureData2)	InsertField(idx1,idx2, FIELD_F_CURE_DATA_2, (void*)&dw, sizeof(dw));
								if(dw=rp->CureData3)	InsertField(idx1,idx2, FIELD_F_CURE_DATA_3, (void*)&dw, sizeof(dw));
								if(dw=rp->CureData4)	InsertField(idx1,idx2, FIELD_F_CURE_DATA_4, (void*)&dw, sizeof(dw));
								if(dw=rp->CureData5)	InsertField(idx1,idx2, FIELD_F_CURE_DATA_5, (void*)&dw, sizeof(dw));
							}
						}

					}
				}
				break;
			default:
				break;
			}
		}
		break;
#endif

	case AVP_CALLBACK: 
		h_AVP_KERNEL = (HWND)lParam;
		ret=1;
		break;

	case AVP_CALLBACK_MB_OK:
		ret = MessageBox((char*)lParam,AppName,MB_OK);
		break;

	case AVP_CALLBACK_MB_OKCANCEL:
		ret = MessageBox((char*)lParam,AppName,MB_OKCANCEL);
		break;

	case AVP_CALLBACK_MB_YESNO:
		ret = MessageBox((char*)lParam,AppName,MB_YESNO);
		break;

	case AVP_CALLBACK_PUT_STRING: 
		PutLogString((char*)lParam,"");
		break;

	case AVP_CALLBACK_ROTATE:
	case AVP_CALLBACK_ROTATE_OFF:
		break;
	case AVP_CALLBACK_LOAD_RECORD_NAME:
		AVP_MakeFullName(tempb,(char*)lParam);
		if(hVirlist!=INVALID_HANDLE_VALUE)
		{
			CString s;
			ULONG bread;
			s.Format(_T("%s\xd\xa"),tempb);
			WriteFile(hVirlist,s,s.GetLength(),&bread,NULL);
		}
		else PutLogString(lps->curName,tempb);
		break;
	case AVP_CALLBACK_LOAD_PROGRESS:
		{
			lps=(LOAD_PROGRESS_STRUCT*)lParam;
			
			VirCount=lps->recCount;
			if(lps->curLength!=0){
				s.Format(MS( IDS_LOADBASE_FORMAT ),lps->curName,lps->curPos*100/lps->curLength);
				StatusLine(s);
			}
		}
		break;

/*	case AVP_INTERNAL_MESSAGE:
		{
			OMS* oms=(OMS*)lParam;
			PutLogString(oms->Name,oms->Message);
			delete oms;
		}
		break;
*/
	case AVP_INTERNAL_CHECK_DIR:
		tPage5.m_Directories++;
		StatusLine((char*)lParam);
		break;

	case AVP_CALLBACK_GET_SECTOR_IMAGE:
		if(lParam==ST_MBR)	return (LONG)BinMBR;
		else				return (LONG)BinBOOT;

	case AVP_CALLBACK_ASK_DELETE:
		if(MethodDelete)return MethodDelete;
		s.Format(MS( IDS_ASK_DELETE ),
			ScanObject->Name,
			AVP_MakeFullName(tempb,ScanObject->VirusName));
		ret=MessageBox(s,AppName,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if(ret==IDYES){
			ret=2;
			s=MS( IDS_ASK_DELETE_ALL);
		}else{
			ret=1;
			s=MS( IDS_ASK_DELETE_SKIP_ALL );
		}
		if(IDYES==MessageBox(s,AppName,MB_YESNO|MB_ICONQUESTION))
			MethodDelete=ret;
		break;
	
	case AVP_CALLBACK_ASK_CURE:
		{
			BOOL CopyInfected=tPage6.m_CopyInfected;
			//if(ScanObject->InArc)ret=0;else

			switch(tPage6.m_Action){
			case 1:
				if(!ApplyToAll)
				{
					CDisinfectDlg dd;
					if(ScanObject->SType==OT_FILE)
						dd.m_sObject=MS( IDS_DISINF_OFILE );
					if(ScanObject->SType & OT_SECTOR)
						dd.m_sObject=MS( IDS_DISINF_OSECTOR );

					dd.m_sObject+="    ";
					dd.m_sObject+=ScanObject->Name;
					AVP_MakeFullName(dd.m_sVirus.GetBuffer(0x200),ScanObject->VirusName);
					dd.m_sVirus.ReleaseBuffer();
					dd.m_Action=LastAction;
					dd.m_All=0;
					dd.m_CopyFolder=CopyInfected;
					if(cust.m_Sound){
						if(!sndPlaySound("infected.wav", SND_ASYNC))
							MessageBeep(MB_ICONHAND);
					}
					
					if(IsIconic())ShowWindow(SW_SHOWNORMAL);
					SetForegroundWindow();

					pddlg=&dd;
					int r=dd.DoModal();
					pddlg=NULL;

					if(r==IDCANCEL){
						ret=0;break;
					}
					if(r==IDC_STOP){
						CancelProcess();
						ret=0;break;
					}
					LastAction=dd.m_Action;
					CopyInfected=dd.m_CopyFolder;
					ApplyToAll=dd.m_All;
					if(ApplyToAll)tPage6.m_CopyInfected=CopyInfected;

				}
				ret=LastAction;
				break;
			case 2:ret= 1; //disinfect
				break;
			case 3:ret= 2; //delete
				break;
			default:
			case 0:ret= 0; //test only
				break;
			}
			if(SIGN_BAD_PARAMETERS!=sign_check)ret=0;

			if(CopyInfected)CopyToFolder(ScanObject,m_FPInfectFolder);
		}
		if(ret && !SectorWarn && (ScanObject->SType & OT_SECTOR)){
			if(IDYES!=MessageBox(MS(IDS_CURE_SECTOR),AppName,MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2))
			{
				ret=0;
				CancelProcess();
			}
			SectorWarn=1;
		}

		break;

	case AVP_CALLBACK_OBJ_NAME:
		if(tPage6.m_CopyInfected 
			&&	!strnicmp((char*)lParam,m_FPInfectFolder,m_FPInfectFolder.GetLength()))
				return 17;
		if(tPage6.m_CopySuspicion 
			&&	!strnicmp((char*)lParam,m_FPSuspFolder,m_FPSuspFolder.GetLength()))
				return 17;

		StatusLine((char*)lParam);
		break;

	case AVP_CALLBACK_OBJECT_BEGIN:
		if(HiResCounterSupported)
		{
			LARGE_INTEGER PerfCount;
			QueryPerformanceCounter(&PerfCount);
			ScanObject->RefData=PerfCount.LowPart;
		}else
			ScanObject->RefData=GetTickCount();
		break;

	case AVP_CALLBACK_OBJECT_DONE:

		if(ScanObject->RFlags & RF_INTERFACE_FAULT)
			ObjectMessage(ScanObject,MS( IDS_INTERFACE_FAULT), 1);
		if(ScanObject->RFlags & RF_KERNEL_FAULT)
			ObjectMessage(ScanObject,MS( IDS_KERNEL_FAULT), 1);

		if(!ScanObject->Handle && !ScanObject->Entry)
			tPage5.m_CountSize+=ScanObject->Size;

		if(ScanObject->RFlags & RF_NOT_A_PROGRAMM) break;

		tPage5.m_RealCountSize+=ScanObject->Size;
		
		if(ScanObject->SType==OT_FILE && !ScanObject->InPack)
			tPage5.m_Files++;
		if(ScanObject->SType & OT_SECTOR)
		{
			if(!(ScanObject->RFlags & RF_REPORT_DONE))
				tPage5.m_Sectors++;
		}

		if((ScanObject->RFlags & RF_NOT_OPEN) 
			|| ((ScanObject->RFlags & RF_DELETE)
				 && !(ScanObject->RFlags & RF_DELETED) 
				 && !ScanObject->InArc)
		){
			if(ScanObject->RFlags & RF_DETECT)
			{
				HANDLE hfile=CreateFile(ScanObject->Name,
				GENERIC_READ|GENERIC_WRITE,	0,
				NULL,OPEN_EXISTING,0,NULL);
				if(hfile==INVALID_HANDLE_VALUE){
					
					switch(GetLastError()){
					case ERROR_LOCK_VIOLATION:
					case ERROR_SHARING_VIOLATION:
						ObjectMessage(ScanObject,MS( IDS_OBJ_LOCKED ),1);	
						if(ScanObject->RFlags & RF_DELETE){
							AvpMoveFileEx(ScanObject->Name,NULL);
							needReboot=1;
						}
						else{
							LockedFiles.Add(ScanObject->Name);
						}
						return KillScan;
					default:
						break;
					}
				}
				else 
					CloseHandle(hfile);

				tPage5.m_Errors++;
				ObjectMessage(ScanObject,MS( IDS_OBJ_IOERROR ),1,AVP_ALERT_ERROR_EXT);	
				SendStatisticsByEvent(0);
			}
			else 
				if(tPage3.m_ShowOK)
				ObjectMessage(ScanObject,MS( IDS_OBJ_NO_ACCESS ),1);	

			return KillScan;
		}

		if(ScanObject->RFlags & RF_DISK_OUT_OF_SPACE)
			ObjectMessage(ScanObject,MS( IDS_OBJ_DISKOUTOFSPACE ),1);

		if(ScanObject->RFlags & RF_CORRUPT){
			if(!(ScanObject->RFlags & RF_REPORT_DONE))
			{
				tPage5.m_Corrupted++;
				ObjectMessage(ScanObject,MS( IDS_OBJ_CORRUPTED ),1);
			}
		}
		if(ScanObject->RFlags & RF_IO_ERROR){
			tPage5.m_Errors++;
			ObjectMessage(ScanObject,MS( IDS_OBJ_IOERROR ),1,AVP_ALERT_ERROR_EXT);
		}
//  !! moved to AVP_CALLBACK_OBJECT_CURE_FAIL
//		if(ScanObject->RFlags & RF_CURE_FAIL){
//			s.Format(MS( IDS_OBJ_DISINF_FAILED ),AVP_MakeFullName(tempb,ScanObject->VirusName));
//			ObjectMessage(ScanObject,s,1);	
//		}

		if(ScanObject->RFlags & RF_PROTECTED){
//			if(tPage3.m_ShowOK)
				ObjectMessage(ScanObject,MS( IDS_OBJ_PROTECTED ),1);
		}

		if(!!(ScanObject->RFlags & (RF_DETECT|RF_WARNING|RF_SUSPIC)))
			ScanObject->RFlags |= RF_REPORT_DONE;

#ifdef _DEBUG
		if( !!(ScanObject->RFlags & ~(RF_CURE|RF_ARCHIVE|RF_PACKED)) ){
			s.Format(_T("RFlags: %08X"),ScanObject->RFlags);
			ObjectMessage(ScanObject,s);	
		}else
#endif
		if(tPage3.m_ShowOK)
			ObjectMessage(ScanObject,MS( IDS_OBJ_OK ));

		SendStatisticsByEvent(1);

		return KillScan;///!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	case AVP_CALLBACK_OBJECT_CURE_FAIL:
		s.Format(MS( IDS_OBJ_DISINF_FAILED ),AVP_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s,1);	
		break;

	case AVP_CALLBACK_OBJECT_CURE:
		s.Format(MS( IDS_OBJ_DISINFECTED ),AVP_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s,1,AVP_ALERT_OBJECT_CURE);	
		tPage5.m_Disinfected++;
		SendStatisticsByEvent(0);
		break;

	case AVP_CALLBACK_OBJECT_DELETE:
		s.Format(MS( IDS_OBJ_DELETED ),AVP_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s,1,AVP_ALERT_OBJECT_DELETE);	
		tPage5.m_Deleted++;
		SendStatisticsByEvent(0);
		break;

	case AVP_CALLBACK_OBJECT_DETECT:
		s.Format(MS( IDS_OBJ_INFECTED ),AVP_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s,1,AVP_ALERT_OBJECT_DETECT);	

		char* np;
		np=ScanObject->VirusName;
		while(*np=='#')
		{
			if(*(--np)=='\n') break;
			while(*(--np)) if(*np=='\n') break;
			np++;
		}
		
		for(i=VirCountArray.GetUpperBound();i>=0;i--)
			if(VirCountArray[i]==(DWORD)np)goto noadd;
		tPage5.m_Virus=1+VirCountArray.Add((DWORD)np);
noadd:	tPage5.m_Bodies++;
		SendStatisticsByEvent(0);
		break;
	
	case AVP_CALLBACK_OBJECT_PACKED:
		if(tPage3.m_ShowPack){
			if(ScanObject->RFlags & RF_ENCRYPTED)
				s.Format("%s %s", MS( IDS_OBJ_ENCRYPTED_BY ), AVP_MakeFullName(tempb,ScanObject->VirusName));
			else s.Format(MS( IDS_OBJ_PACKED ), AVP_MakeFullName(tempb,ScanObject->VirusName));
			ObjectMessage(ScanObject,s,1);
			ScanObject->RFlags&=~RF_REPORT_DONE;
		}
		ScanObject->RFlags&=~RF_ENCRYPTED;
		if(!ScanObject->InPack)		tPage5.m_Packed++;
		break;

	case AVP_CALLBACK_OBJECT_ARCHIVE:
		if(tPage3.m_ShowPack){
			s.Format(MS( IDS_OBJ_ARCHIVE ),AVP_MakeFullName(tempb,ScanObject->VirusName));
			ObjectMessage(ScanObject,s,1);
		}
		tPage5.m_Archives++;
		break;

	case AVP_CALLBACK_OBJECT_WARNING:
		if(tPage3.m_Warnings){
			s.Format(MS( IDS_OBJ_WARNING ),AVP_MakeFullName(tempb,ScanObject->WarningName));
			ObjectMessage(ScanObject,s,1,AVP_ALERT_OBJECT_WARNING);	
			tPage5.m_Warnings++;
			if(tPage6.m_CopySuspicion)CopyToFolder(ScanObject,m_FPSuspFolder);
			SendStatisticsByEvent(0);
		}
		break;

	case AVP_CALLBACK_OBJECT_SUSP:
		s.Format(MS( IDS_OBJ_SUSPICION ),AVP_MakeFullName(tempb,ScanObject->SuspicionName));
		ObjectMessage(ScanObject,s,1,AVP_ALERT_OBJECT_SUSP);	
		tPage5.m_Suspicious++;
		if(tPage6.m_CopySuspicion)CopyToFolder(ScanObject,m_FPSuspFolder);
		SendStatisticsByEvent(0);
		break;

	case AVP_CALLBACK_OBJECT_UNKNOWN:
		s.Format(MS( IDS_OBJ_UNKNOWN ),AVP_MakeFullName(tempb,ScanObject->VirusName));
		ObjectMessage(ScanObject,s,1);	
		break;

	case AVP_CALLBACK_OBJECT_MESS:
		ObjectMessage(ScanObject,ScanObject->VirusName,1);	
		break;

	case AVP_CALLBACK_ERROR_FILE_OPEN:
		if(CCClientSendAlert)
			CCClientSendAlert(AVP_ALERT_ERROR_INT,(char*)lParam);
		s.Format(MS( IDS_ERROR_FILE_OPEN ),(char*)lParam);
		return MessageBox(s,AppName,MB_ICONEXCLAMATION|MB_TOPMOST);

	case AVP_CALLBACK_ERROR_SET:
		if(CCClientSendAlert)
			CCClientSendAlert(AVP_ALERT_ERROR_INT,(char*)lParam);
		s.Format(MS( IDS_ERROR_SET ),(char*)lParam);
		return MessageBox(s,AppName,MB_ICONEXCLAMATION|MB_TOPMOST);

	case AVP_CALLBACK_ERROR_BASE:
		if(CCClientSendAlert)
			CCClientSendAlert(AVP_ALERT_ERROR_INT,(char*)lParam);
		s.Format(MS( IDS_ERROR_BASE ),(char*)lParam);
		return MessageBox(s,AppName,MB_ICONEXCLAMATION|MB_TOPMOST);

//	case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
// Supported in AVPBASE.DLL. Never comes here.
//		break;

	case AVP_CALLBACK_SETFILE_INTEGRITY_FAILED:
	case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
		ret=SignReportMB((char*)lParam,-1001,0);
		break;
	case AVP_CALLBACK_SETFILE_OPEN_FAILED_TRY_AGAIN:
	case AVP_CALLBACK_FILE_OPEN_FAILED_TRY_AGAIN:
#ifdef _DEBUG
		s.Format("Can't open file %s \n\nTry again?",(char*)lParam);
		return IDYES==MessageBox(s,AppName,MB_YESNO|MB_ICONEXCLAMATION|MB_TOPMOST);
#endif
		break;
	case AVP_CALLBACK_ERROR_SYMBOL:
	case AVP_CALLBACK_ERROR_FIXUPP:
	case AVP_CALLBACK_ERROR_KERNEL_PROC:
	case AVP_CALLBACK_ERROR_LINK:
		{
			static errorLink=0;
			if(ExitCode!=7)
			if(!errorLink){
				errorLink=1;
				if(CCClientSendAlert)
					CCClientSendAlert(AVP_ALERT_ERROR_INT,MS( IDS_ERROR_LINK ));
				return MessageBox(MS( IDS_ERROR_LINK ),AppName,MB_ICONEXCLAMATION|MB_TOPMOST);
			}
		}
		break;

	case AVP_CALLBACK_ERROR_SYMBOL_DUP:
		{
			static symDup=0;
			if(ExitCode!=7)
			if(!symDup){
				symDup=1;
				if(CCClientSendAlert)
					CCClientSendAlert(AVP_ALERT_ERROR_INT,MS( IDS_ERROR_DUP_NAME ));
				return MessageBox(MS( IDS_ERROR_DUP_NAME ),AppName,MB_ICONEXCLAMATION|MB_TOPMOST);
			}
		}
		break;

	}
	return ret;
}
	

void CAvp32Dlg::StatusLine( const char* newstring )
{
	m_sLine=" ";
	m_sLine+=newstring;
	SetDlgItemText( IDC_STAT_LINE, m_sLine );
}

/*
#define REP_BEG 50
#define REP_ADD 20
#define REP_MIN 3
		CString s=objname;
		int l=s.GetLength();
		if(l+REP_MIN > REP_BEG)	l=REP_BEG+((l-REP_BEG+REP_MIN)/REP_ADD+1)*REP_ADD-l;
		else		l=REP_BEG-l;
		while(l--)s+=" ";
		s+=message;
		s+="\x0d\x0a";
*/

int CAvp32Dlg::PutLogString( const char* objname, const char* message)
{
	
	CString s=objname;
	if(message && *message)
	{
		s+="\x9";
		s+=message;
	}

	PutReportString( s );
	m_RepView.SetReportCortege( s );

/*	
	
	LV_ITEM			lvitem;
	int i=m_List.GetItemCount();
	if((i%200)==0)m_List.SetItemCount(i+200);

	lvitem.mask = LVIF_TEXT | LVIF_STATE;// | (iSubItem == 0? LVIF_IMAGE : 0);
	lvitem.iItem = i;
	lvitem.iSubItem = 0;
	lvitem.state = i?0:LVIS_FOCUSED;
	if(m_Track)	lvitem.state |= LVIS_FOCUSED;
	lvitem.pszText = (char*)objname;
	i = m_List.InsertItem(&lvitem);
	
	if(message && *message)
	{
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 1;
		lvitem.pszText = (char*)message;
		m_List.SetItem(&lvitem);
	}
	
	if(m_Track)	m_List.EnsureVisible(i,FALSE);
	return i;
*/
	return 0;
}

BOOL CAvp32Dlg::InsertTabPage(CTabCtrl* _tab, CWnd* _page)
{
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
	CString m_sCaption;
	int i=_tab->GetItemCount();

	_page->GetWindowText(m_sCaption);
	TabCtrlItem.pszText = (char*)(const char*)m_sCaption;
	TabCtrlItem.lParam = (LPARAM) _page;
	_tab->InsertItem( i, &TabCtrlItem );

	return _page->ModifyStyle(WS_CAPTION|WS_BORDER,0,0);
}





BOOL CAvp32Dlg::DisableControls(BOOL dis)
{
	CWnd* wp=GetFocus();

//	GetDlgItem(IDC_TRACK)->ShowWindow(dis?SW_SHOW:SW_HIDE);

	GetMenu()->EnableMenuItem(ID_SCAN_NOW,dis?MF_GRAYED:MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_STOP,dis?MF_ENABLED:MF_GRAYED);
	
	GetMenu()->EnableMenuItem(ID_TOOLS_UPDATENOW,dis?MF_GRAYED:MF_ENABLED);
//	GetMenu()->EnableMenuItem(ID_FIND,(dis /*|| m_List.GetItemCount()*/)?MF_ENABLED:MF_GRAYED);
//	GetMenu()->DeleteMenu(ID_FIND,0);

	GetMenu()->EnableMenuItem(ID_VIRLIST,dis?MF_GRAYED:MF_ENABLED);


	GetDlgItem(IDC_SCAN_NOW)->SetWindowText(MS( (dis?IDS_STOP:IDS_SCAN_NOW) ));
	SetDefID(IDC_SCAN_NOW);

//	tPage1.GetDlgItem(IDC_LIST_DISK)->EnableWindow(!dis);
	tPage1.Enable(!dis);
	tPage1.GetDlgItem(IDC_LOCAL)->EnableWindow(!dis);
	tPage1.GetDlgItem(IDC_NETWORK)->EnableWindow(!dis && (functionality & FN_NETWORK));
	tPage1.GetDlgItem(IDC_FLOPPY)->EnableWindow(!dis);
	tPage1.GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(!dis);

	tPage2.GetDlgItem(IDC_MEMORY)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_SECTORS)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_FILES)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_PACKED)->EnableWindow(!dis /*&& (functionality & FN_OPTIONS)*/);
	tPage2.GetDlgItem(IDC_EXTRACT)->EnableWindow(!dis /*&& (functionality & FN_OPTIONS)*/);
	tPage2.GetDlgItem(IDC_MAILBASES)->EnableWindow(!dis /*&& (functionality & FN_OPTIONS)*/);
	tPage2.GetDlgItem(IDC_MAILPLAIN)->EnableWindow(!dis /*&& (functionality & FN_OPTIONS)*/);

/*
	if(!(functionality & FN_MAIL)){
		tPage2.GetDlgItem(IDC_MAILBASES)->ShowWindow(SW_HIDE);
		tPage2.GetDlgItem(IDC_MAILPLAIN)->ShowWindow(SW_HIDE);
	}
*/
	tPage2.GetDlgItem(IDC_SMART)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_PROGRAMS)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_ALLFILES)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_USERDEF)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_EDIT_EXT)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_EXCLUDE)->EnableWindow(!dis);
	tPage2.GetDlgItem(IDC_EDIT_EXCLUDE)->EnableWindow(!dis);

	tPage3.GetDlgItem(IDC_WARNINGS)->EnableWindow(!dis);
	tPage3.GetDlgItem(IDC_CA)->EnableWindow(!dis /*&& (functionality & FN_OPTIONS)*/);
	tPage3.GetDlgItem(IDC_REDUNDANT)->EnableWindow(!dis);
	tPage3.GetDlgItem(IDC_SHOWOK)->EnableWindow(!dis);
	tPage3.GetDlgItem(IDC_SHOWPACK)->EnableWindow(!dis);
	tPage3.GetDlgItem(IDC_REPORT)->EnableWindow(!dis);
//	tPage3.GetDlgItem(IDC_BUTTON_CUST)->EnableWindow(!dis);

	tPage6.GetDlgItem(IDC_REPORTONLY)->EnableWindow(!dis);
	tPage6.GetDlgItem(IDC_DISINFECTDIALOG)->EnableWindow(!dis && (functionality & FN_DISINFECT));
	tPage6.GetDlgItem(IDC_DISINFECTAUTO)->EnableWindow(!dis && (functionality & FN_DISINFECT));
	tPage6.GetDlgItem(IDC_DELETE)->EnableWindow(!dis );
	tPage6.GetDlgItem(IDC_COPYINFECTED)->EnableWindow(!dis);
	tPage6.GetDlgItem(IDC_COPYSUSPICIOUS)->EnableWindow(!dis);

	if(wp){
		while(!wp->IsWindowEnabled())	wp=GetNextDlgTabItem(wp);
		wp->SetFocus();
	}

//	UpdateData();
	return dis;
}

////////////////////////////This code inside new SCAN THREAD!!!!


CAvp32Dlg::CheckBoot(CString& path, DWORD type)
{
	AVPScanObject ScanObject;
	if(path[1]!=':')return 0;
	CString Name=MS( IDS_BOOT_SECTOR );
	Name+=" ";
	Name+=path.Left(2);

	memset(&ScanObject,0,sizeof(AVPScanObject));
	ScanObject.SType=OT_BOOT; 
	ScanObject.Size=512; 
	ScanObject.MFlags=tPage2.m_Mask;
	if(tPage3.m_Warnings)	ScanObject.MFlags|=MF_WARNINGS;
	if(tPage3.m_CA)			ScanObject.MFlags|=MF_CA;
	if(tPage3.m_Redundant)	ScanObject.MFlags|=MF_REDUNDANT;


	strcpy(ScanObject.Name,Name);
	ScanObject.Drive=(path[0]|0x20)-'a';
	ScanObject.Disk=ScanObject.Drive;

	switch(type)
	{
	case DRIVE_REMOTE:
		ScanObject.Disk=0x20; return 0;
	case DRIVE_RAMDISK:
		ScanObject.Disk=0x40; break;
	case DRIVE_CDROM:
		ScanObject.Disk=0x60 | ScanObject.Drive; break;
	case DRIVE_FIXED:
//		ScanObject.Disk=0x80; break;
		ScanObject.Disk=0xFF; break;
	case DRIVE_REMOVABLE:
	default:
		ScanObject.Disk=ScanObject.Drive;	break;
		return 0;
	}

	return AVP_ProcessObject(&ScanObject);
}

CAvp32Dlg::CheckMBRS()
{
	DWORD ret=0;
	EnterCriticalSection(&TotalSpaceCriticalSection);
		TotalSpace+=(16+4)*512;
	LeaveCriticalSection(&TotalSpaceCriticalSection);
	for(int i=0;i<16;i++)
	{
		CString Name;
		AVPScanObject ScanObject;
		memset(&ScanObject,0,sizeof(AVPScanObject));
		ScanObject.SType=OT_MBR;
		ScanObject.Size=512; 
		ScanObject.MFlags=tPage2.m_Mask;
//		ScanObject.MFlags|=MF_ALLWARNINGS;
		ScanObject.MFlags|=MF_ALLENTRY;
		if(tPage3.m_Warnings)	ScanObject.MFlags|=MF_WARNINGS;
		if(tPage3.m_CA)			ScanObject.MFlags|=MF_CA;
		if(tPage3.m_Redundant)	ScanObject.MFlags|=MF_REDUNDANT;

		ScanObject.Drive=2;
		
		Name.Format(MS( IDS_MBR_SECTOR ),i+1);
		strcpy(ScanObject.Name,Name);
		ScanObject.Disk=0x80+i;

		ret&=AVP_ProcessObject(&ScanObject);
		if(KillScan)break;
	}
	return ret;
}

static void SaveMasks(const char* str,DWORD ctlcode)
{
	if(ctlcode)	AVP_AddMask(NULL);
	else		AVP_AddExcludeMask(NULL);

	if(str==NULL)return;

	char s[0x200];
	strcpy(s,str);
	_strlwr(s);
	char* span=s;
	char end=*span;

	while(end && *span)
	{
		span+=strspn( span, ";, " );
		int l=strspn( span, "\"\'" );
		span+=l;
		switch(l)
		{
			case 0:		l=strcspn( span, ";, " );
				break;
			case 1:		l=strcspn( span, "\"\'" );
				break;
			default:
				break;
		}
		end=span[l];
		span[l]=0;
		if(*span)
		{
			if(ctlcode)	AVP_AddMask(span);
			else		AVP_AddExcludeMask(span);
			span+=l;
			span++;
		}
	}
}

UINT CAvp32Dlg::ScanAllPaths()   
{
	UINT ret=0;
	int i;
	int size=ScanList.GetSize();
	StartTime = CTime::GetCurrentTime();
	if(ReportWithTiming){
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}

	if(tPage2.m_Exclude)
	SaveMasks(tPage2.m_EditExclude,0);
	SaveMasks(tPage2.m_UserMask,1);

	if(tPage2.m_Memory){
		AVPScanObject ScanObject;
		memset(&ScanObject,0,sizeof(AVPScanObject));
		ScanObject.SType=OT_MEMORY;
		ScanObject.Size=1024; 
		ScanObject.MFlags=tPage2.m_Mask;
		strcpy(ScanObject.Name,MS( IDS_MEMORY ));
		AVP_ProcessObject(&ScanObject);
		if(KillScan)goto ret;
	}

	if(tPage2.m_Sectors){
		BOOL MBRS=FALSE;
		for(int i=0; i<size ; i++){
			CString Path=ScanList[i];
			if(Path.Right(1)==":"){
				DWORD type=GetDriveType(Path+"\\");
				switch(type)
				{
				case DRIVE_FIXED:
				case DRIVE_RAMDISK:
					MBRS=TRUE;
					if(KillScan)goto ret;
//					if(!CheckForSubst((Path[0]|0x20)-'a'+1))
						CheckBoot(Path,type);
				default:	break; //All other types check later.
				}
			}
		}
		if(KillScan)goto ret;
		if(MBRS)	CheckMBRS();
	}

	for(i=0; i<ScanList.GetSize();i++)
	{
		CString Path=ScanList[i];
		DWORD attr=GetFileAttributes(Path);

		if(attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString path=Path+"\\";
			DWORD type=GetDriveType(path);
			if(tPage2.m_Sectors && Path.Right(1)==":")
			switch(type)
			{
			case DRIVE_RAMDISK:
			case DRIVE_FIXED: //Already checked.
				break;
			default:
				if(KillScan)goto ret;
				CheckBoot(Path,type);
				break;
			}

			if(KillScan)goto ret;
			if(tPage2.m_Files)	
				ret=ScanPath(Path);

			if(KillScan)goto ret;
			if(size==1 && path.GetLength()==3 && isalpha(path[0]) && path[1]==__TEXT(':')
				&& (type==DRIVE_REMOVABLE || type==DRIVE_CDROM))
			{
				if(cust.m_CustNextDiskPrompt)
				if(IDYES==MessageBox(
					(const char*)((type==DRIVE_CDROM)?
					MS( IDS_NEXT_CDROM ):
					MS( IDS_NEXT_FLOPPY )),
					AppName,
					MB_YESNO|MB_ICONINFORMATION))
				{
					i--;
					if(tPage2.m_Sectors){
						EnterCriticalSection(&TotalSpaceCriticalSection);
							TotalSpace+=512;
						LeaveCriticalSection(&TotalSpaceCriticalSection);
					}
					if(tPage2.m_Files)
						AfxBeginThread(Count,(LPVOID)(const char*)Path,THREAD_PRIORITY_NORMAL+tPage3.m_Priority-2);
					continue;
				}
			}

		}
		else{
			AVPScanObject ScanObject;
			memset(&ScanObject,0,sizeof(AVPScanObject));
			ScanObject.SType=OT_FILE;
			ScanObject.MFlags=MF_MASK_ALL;
			if(tPage2.m_Packed)		ScanObject.MFlags|=MF_PACKED;
			if(tPage2.m_Archives)	ScanObject.MFlags|=MF_ARCHIVED;
			if(tPage2.m_MailBases)	ScanObject.MFlags|=MF_MAILBASES;
			if(tPage2.m_MailPlain)	ScanObject.MFlags|=MF_MAILPLAIN;
			if(tPage3.m_Warnings)	ScanObject.MFlags|=MF_WARNINGS;
			if(tPage3.m_CA)			ScanObject.MFlags|=MF_CA;
			if(tPage3.m_Redundant)	ScanObject.MFlags|=MF_REDUNDANT;
			if(tPage2.m_Exclude)	ScanObject.MFlags|=MF_MASK_EXCLUDE;
			if(delete_archives)		ScanObject.MFlags|=MF_DELETE_ARCHIVES;
			strcpy(ScanObject.Name, Path);
			AVP_ProcessObject(&ScanObject);
		}
	}

ret:	
	if(clListDelete && !KillScan)
	{
		DeleteFile(clList);
		clListDelete=0;
	}
	clStart=0;
	ScanList.RemoveAll();
	return ret;
}

#include "scandir.h"

UINT CAvp32Dlg::ScanPath( const char* path )
{
	UINT ret=0;
	StatusLine(path);

	CScanDir SD(path);
	
	DWORD MFlags=tPage2.m_Mask;

	if(tPage2.m_Packed)		MFlags|=MF_PACKED;
	if(tPage2.m_Archives)	MFlags|=MF_ARCHIVED;
	if(tPage2.m_MailBases)	MFlags|=MF_MAILBASES;
	if(tPage2.m_MailPlain)	MFlags|=MF_MAILPLAIN;

	if(tPage3.m_Warnings)	MFlags|=MF_WARNINGS;
	if(tPage3.m_CA)			MFlags|=MF_CA;
	if(tPage3.m_Redundant)	MFlags|=MF_REDUNDANT;
	if(tPage2.m_Exclude)	MFlags|=MF_MASK_EXCLUDE;
	
	if(delete_archives)		MFlags|=MF_DELETE_ARCHIVES;

//	MFlags|=MF_ALLWARNINGS;
//	MFlags|=MF_ALLENTRY;
	SD.Dispatch(this,&KillScan,MFlags,1);

	return ret;
}
/////////////////////////


void CAvp32Dlg::OnClose() 
{
	if(VirThreadPtr){
		if(!KillScan) OnStop();
		if(KillScan)PostMessage(WM_CLOSE);
		return;
	}

	if((DWORD)ScanThreadPtr > (DWORD)1){
		if(!KillScan) OnStop();
		if(KillScan)PostMessage(WM_CLOSE);
		ScanThreadPtr->ResumeThread();
		return;
	}
	CanCancel=TRUE;
	CDialog::OnClose();
}

void CAvp32Dlg::OnCancel()
{
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return;
	}
	if(CanCancel)	CDialog::OnCancel();
}



int ReadString(CString& s, HANDLE f)
{ 
	char buf[0x200];
	char* p=buf;
	DWORD bread;
	for(int i=0;i<0x200;i++ ,p++){
		if( !ReadFile(f,p,1,&bread,NULL) || !bread)break;
		if(*p==0x0D || *p==0x0A ){
			ReadFile(f,p,1,&bread,NULL);
			break;
		}
	}
	*p=0;
	s=buf;
	return i;

}


void CAvp32Dlg::OnAvpStart() 
{
	
	char* ptr;
	char buf[0x500];
	int p;

	SendMessage(WM_COMMAND,ID_AVP_RELOADBASES,0);
	
	CString TempLine=CommandLine;
	while(p=TempLine.GetLength()){
		char sc=' ';
nxt1:
	
		switch(TempLine[--p]){
		case '\\':
		case '/':
		case ' ':
		case 0x9:
		case 0xd:
		case 0xa:
			if(p)goto nxt1;
			--p;
			break;
		case '\'':
			sc='\'';
			--p;
			break;
		case '\"':
			sc='\"';
			--p;
			break;
		default:
			break;

		}
		if(! ++p) break;
		TempLine=TempLine.Left(p);

		p=TempLine.ReverseFind(sc)+1;
		if(!(sc==' ' && TempLine[p]=='/'))
		{
			clStart=1;
			ScanList.InsertAt(0,TempLine.Mid(p));
		}
		if(p==0)break;
		TempLine=TempLine.Left(p);
	}	


	p=CommandLine.Find(_T("/@"));
	if(p!=-1){
		p+=2;
		if(CommandLine[p]=='!')
		{
			p++;
			clListDelete=1;
		}
		if(CommandLine[p]=='='){
			p++;
			char sc;
			switch(CommandLine[p]){
			case '\'':
			case '\"':	sc=CommandLine[p];p++;break;
			default:	sc=' ';break;
			}
			CString m=CommandLine.Mid(p);
			p=m.Find(sc);
			if(p!=-1)m=m.Left(p);
			if(GetFullPathName(m,0x500,buf,&ptr)){
				clList=m;
				clStart=1;

				HANDLE f=CreateFile(clList,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
				if(f!=INVALID_HANDLE_VALUE)
				{
					CString s;
					while(ReadString(s,f)){
						if(s[0]==';')continue;
						if(s[0]==' ')continue;
						if(s[0]== 0 )continue;
						int i=s.GetLength();
nxt:
						switch(s[--i]){
						case '\\':
						case '/':
						case ' ':
						case 0x9:
						case 0xd:
						case 0xa:
							if(i)goto nxt;
							--i;
						default:
							break;
						}
						if(++i)ScanList.Add(s.Left(i));
					}
					CloseHandle(f);
				}
				else
				{
					SendMessage(wm_AVP,AVP_CALLBACK_ERROR_FILE_OPEN,(long)(const char*)clList);
				}
			}
		}
	}

	DisableControls(FALSE);
	UpdateWindow();
//	InitAgentInterface(1);
	if ((!CCClientLoadDLL(NULL)) || (!InitCCClientInterface(1))) p=0;

	if(clStart)SendMessage(WM_COMMAND,ID_SCAN_NOW,0);
}

int ReadString(char* buf,DWORD size, HANDLE f)
{
        DWORD bread;
		*buf=0;
        if( !ReadFile(f,buf,size-1,&bread,NULL) || bread>size )return 0;
        buf[bread]=0;
        for(DWORD i=0;i<bread;i++ ,buf++){
                if(*buf==0x0D || *buf==0x0A ){
                        *buf=0;
                        buf++;i++;
                        if(*buf==0x0D || *buf==0x0A || *buf==0)i++;
                        SetFilePointer(f,i-bread,NULL,FILE_CURRENT);
                        break;
                }
        }
        return i;
}

void CAvp32Dlg::OnAvpReloadbases() 
{
	int i=0;
	if(ScanThreadPtr){
		should_reload=1;
		return;
	}
	ScanThreadPtr=(CWinThread*)1; //disable scan while loading
	while(AfxGetApp()->OnIdle(i++));

	CString SetFile;
	GetPrivateProfileString("Configuration","SetFile","AVP.SET",SetFile.GetBuffer(0x500),0x500,IniFile);
	SetFile.ReleaseBuffer();

	CString BasePath;
	GetPrivateProfileString("Configuration","BasePath","",BasePath.GetBuffer(0x500),0x500,IniFile);
	BasePath.ReleaseBuffer();

	if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
	{
		WritePrivateProfileString("Configuration","SetFile",SetFile,IniFile);
		WritePrivateProfileString("Configuration","BasePath",BasePath,IniFile);
	}
	if(BasePath.GetLength()==0)
	{
		BasePath=StartPath;
	}
	if(BasePath.GetLength() && BasePath.Right(1)!="\\")	BasePath+="\\";

	
	BasePath+=SetFile;


	if(hVirlist!=INVALID_HANDLE_VALUE)
		AVP_SetCallbackNames(1);
	
	i=AVP_LoadBase(BasePath,0);

	AVP_SetCallbackNames(0);
	if(hVirlist!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hVirlist);
		hVirlist=INVALID_HANDLE_VALUE;
	}
	
	if(i){
/*
		CFileStatus SetFileStat;
		if(CFile::GetStatus(Name,SetFileStat))
			BaseTime=SetFileStat.m_mtime;
*/		
		if(lps)	
			BaseTime=CTime( 
			lps->LastUpdateYear, 
			lps->LastUpdateMonth, 
			lps->LastUpdateDay, 
			0, 0, 0);

		if(cust.m_CustOtherMessages)
			AVPRegistryChecker();


		AVPScanObject ScanObject;
		if(tPage2.m_Memory){
			memset(&ScanObject,0,sizeof(AVPScanObject));
			ScanObject.RFlags|=RF_REPORT_DONE;
			ScanObject.SType=OT_MEMORY;
			ScanObject.Size=1024; 
			ScanObject.MFlags=tPage2.m_Mask;
			strcpy(ScanObject.Name,MS( IDS_MEMORY ));
			AVP_ProcessObject(&ScanObject);
		}

		memset(&ScanObject,0,sizeof(ScanObject));
		ScanObject.RFlags|=RF_REPORT_DONE;
		GetModuleFileName(AfxGetInstanceHandle(),ScanObject.Name,0x200);
		ScanObject.Type=OT_FILE;
		ScanObject.MFlags=MF_MASK_ALL|MF_WARNINGS|MF_CA;
		ScanObject.MFlags=(ScanObject.MFlags&~MF_D_)|MF_D_CUREALL;
		AVP_ProcessObject(&ScanObject);
		if(ScanObject.RFlags & RF_DETECT && ScanObject.RFlags & RF_NOT_OPEN)
		{
			char buf[0x200];
			char vir[0x100];
			DWORD attr;
			strcpy(buf,ScanObject.Name);
			char* lp=strrchr(ScanObject.Name,'.');
			strcpy(lp,"_.EXE");
			if(!CopyFile(buf,ScanObject.Name,FALSE))goto dfail;
			if((DWORD)-1==(attr=GetFileAttributes(ScanObject.Name)))goto dfail;
			if(attr & FILE_ATTRIBUTE_READONLY)
			{
				attr&=~FILE_ATTRIBUTE_READONLY;
				if(!SetFileAttributes(ScanObject.Name,attr))goto dfail;
			}
			ScanObject.RFlags=0;
			AVP_ProcessObject(&ScanObject);
			AVP_MakeFullName(vir,ScanObject.VirusName);
			if(!(ScanObject.RFlags & RF_CURE))goto dfail;

			sprintf(buf,MS(IDS_SELF_DISINFECTED),vir);
			MessageBox(buf,AppName,MB_ICONEXCLAMATION|MB_TOPMOST);
			
			strcpy(execName,ScanObject.Name);
			PostMessage(WM_CLOSE, 0, 0);

			goto next4;
dfail:
			sprintf(buf,MS(IDS_SELF_DISINFECTION_FAILED),vir);
			MessageBox(buf,AppName,MB_ICONEXCLAMATION|MB_TOPMOST);
		}
next4:

		CString s;
//		s.Format(MS( IDS_BASES_LOADED ),VirCount);
		s.Format(MS( IDS_LAST_UPDATE ),BaseTime.Format(_T("%d.%m.%Y")) ,VirCount);

		StatusLine(s);
	}
	else
		StatusLine(MS( IDS_BASES_NOT_LOADED ));

	if(ExitCode!=7)
	if(cust.m_UpdateCheck && !clStart)
		if(BaseTime + CTimeSpan(cust.m_UpdateInterval,0,0,0) < CTime::GetCurrentTime())
			if(IDOK==MessageBox(MS( IDS_SET_OUTOFDATE ),AppName,MB_OKCANCEL|MB_ICONINFORMATION))
				PostMessage(WM_COMMAND,ID_TOOLS_UPDATENOW,0);

	DisableControls(0);
	ScanThreadPtr=0;
}
/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog


AboutDlg::AboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AboutDlg)
	//}}AFX_DATA_INIT
}


void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AboutDlg)
	DDX_Control(pDX, IDC_AVPABOUTVIEW, m_avcAbout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AboutDlg, CDialog)
	//{{AFX_MSG_MAP(AboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutDlg message handlers

//DEL void AboutDlg::OnSupport() 
//DEL {
//DEL 
//DEL 	int i=m_ListKeys.GetNextItem(-1,LVNI_SELECTED);
//DEL 	if(i!=-1) 
//DEL 	{
//DEL 		AVP_dword dseq[5];
//DEL 		int l=DATA_Get_Val( (HDATA)KeyHandleArray[i], DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSUPPORTINFO,0), 0, 0, 0);
//DEL 		if(l){
//DEL 			char* p=new char[l];
//DEL 			if(p){
//DEL 				if(DATA_Get_Val( (HDATA)KeyHandleArray[i], DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSUPPORTINFO,0), 0, p, l)){
//DEL 					CSupportDlg sd;
//DEL 					sd.m_Support=p;
//DEL 					sd.DoModal();
//DEL 					delete p;
//DEL 					return;
//DEL 				}
//DEL 				delete p;
//DEL 			}
//DEL 		}
//DEL 	}
//DEL 
//DEL 	CString n;
//DEL 	GetPrivateProfileString(_T("Configuration"),_T("SupportFile"),_T("Readme.txt"),n.GetBuffer(512),512,IniFile);
//DEL 	n.ReleaseBuffer();
//DEL 	if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
//DEL 		WritePrivateProfileString(_T("Configuration"),_T("SupportFile"),n,IniFile);
//DEL 	if(_access(n,4)){
//DEL 		CString t=StartPath;
//DEL 		t+="\\";
//DEL 		t+=n;
//DEL 		n=t;
//DEL 	}
//DEL 
//DEL 	ShellExecute(m_hWnd,_T("open"),n,NULL,NULL,SW_SHOWNORMAL);
//DEL }

extern HINSTANCE hLocalizationLibrary;

BOOL AboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(MS( IDS_ABOUT ));
	GetDlgItem(IDOK)->SetWindowText(LoadStr(IDS_OK_BUTTON));

	m_avcAbout.SetKeyFilesFolder(KeysPath);

	CString S;
	S= MS(IDS_VERSION);
	S+= " " VER_PRODUCTVERSION_STR;
	m_avcAbout.SetApplicationVersion(S);
	m_avcAbout.SetApplicationName( VER_PRODUCTNAME_STR );
	m_avcAbout.SetCopyRight1( VER_LEGALCOPYRIGHT_STR );
	m_avcAbout.SetCopyRight2( VER_PORTIONCOPYRIGHT_STR VER_LC_STR );

	S.Format(MS( IDS_LAST_UPDATE ),Avp32Dlg->BaseTime.Format(_T("%d.%m.%Y")) ,Avp32Dlg->VirCount);
	m_avcAbout.SetLastUpdate(S);
	m_avcAbout.SetRegisteredName(Avp32Dlg->m_Name);
	m_avcAbout.SetRegisteredOrganization(Avp32Dlg->m_Org);

	HINSTANCE hiold=AfxGetResourceHandle();
	if(hLocalizationLibrary>0)	AfxSetResourceHandle(hLocalizationLibrary);
	if(m_David.LoadBitmap(IDB_DAVID))	m_avcAbout.SetAVPBitmap((long)HBITMAP(m_David));
	AfxSetResourceHandle(hiold);

    HKEY hKey; 
	if (RegOpenKey(HKEY_LOCAL_MACHINE, AVP_REGKEY_INST_WS_FULL, &hKey) == ERROR_SUCCESS)
    {
		if(ERROR_SUCCESS==GetStrRegVal(hKey, AVP_REGVALUE_USERNAME, S) && S.GetLength())
			m_avcAbout.SetRegisteredName(S);

		if(ERROR_SUCCESS==GetStrRegVal(hKey, AVP_REGVALUE_USERCOMPANY, S) && S.GetLength())
			m_avcAbout.SetRegisteredOrganization(S);
		RegCloseKey(hKey);
    }
/*
	long nMinXSize, nMinYSize;
	m_avcAbout.GetMinSize(&nMinXSize, &nMinYSize);

	SetWindowPos(NULL,0,0,nMinXSize+10,nMinYSize+30,SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_NOMOVE);
*/
	return TRUE;

/*
	ListView_SetExtendedListViewStyle(m_ListKeys.m_hWnd, ListView_GetExtendedListViewStyle(m_ListKeys.m_hWnd)|LVS_EX_FULLROWSELECT);

	m_ListKeys.InsertColumn(0,MS(IDS_LIST_KEYS_NUMBER),LVCFMT_LEFT,130,0);
	m_ListKeys.InsertColumn(1,MS(IDS_LIST_KEYS_PLNAME),LVCFMT_LEFT,100,1);
	m_ListKeys.InsertColumn(2,MS(IDS_LIST_KEYS_DESCRIPTION),LVCFMT_LEFT,300,2);
	m_ListKeys.InsertColumn(3,MS(IDS_LIST_KEYS_EXPIRATION),LVCFMT_LEFT,100,3);
	m_ListKeys.InsertColumn(4,MS(IDS_LIST_KEYS_FILE),LVCFMT_LEFT,100,4);
	for(int i=0;i<KeyHandleArray.GetSize();i++)
	{
		char buf[0x200];
		HDATA hKey=(HDATA)KeyHandleArray[i];
		AVP_dword dseq[5];
		
		*buf=0;
		DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0), 0, buf, 0x200) ;
		m_ListKeys.InsertItem(i,buf);
		if(i==0)
			m_ListKeys.SetItem(i,0,LVIF_STATE,0,0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED,0);

		*buf=0;
		DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOSNAME,0), 0, buf, 0x200);
		m_ListKeys.SetItem(i,1,LVIF_TEXT,buf,0,0,0,0);

		*buf=0;
		if(DATA_Find(hKey, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))
			strcpy(buf,"TRIAL!  ");
		DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLICENCEINFO,0), 0, buf+strlen(buf), 0x200);
		m_ListKeys.SetItem(i,2,LVIF_TEXT,buf,0,0,0,0);

		SYSTEMTIME stCurrent;
		GetSystemTime(&stCurrent);
		SYSTEMTIME stExpir;
		memset(&stExpir,0,sizeof(SYSTEMTIME));
		*buf=0;
		if(DATA_Get_Val( hKey, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, &stExpir, sizeof(AVP_date)) )
		{
			if(stCurrent.wYear > stExpir.wYear) goto time_bad;
			if(stCurrent.wYear < stExpir.wYear) goto time_ok;
			if(stCurrent.wMonth > stExpir.wMonth) goto time_bad;
			if(stCurrent.wMonth < stExpir.wMonth) goto time_ok;
			if(stCurrent.wDay > stExpir.wDay) goto time_bad;
			if(stCurrent.wDay < stExpir.wDay) goto time_ok;
time_bad:
			strcpy(buf,"Expired");
			goto ok;
time_ok:
			strcpy(buf,CTime(stExpir).Format("%x"));
ok:			;
		}

		m_ListKeys.SetItem(i,3,LVIF_TEXT,buf,0,0,0,0);

		DATA_Get_Val(hKey,DATA_Sequence(dseq,PID_FILENAME,0), 0, buf, 0x200);
		char* p;
		GetFullPathName(buf,0x200,buf,&p);
		m_ListKeys.SetItem(i,4,LVIF_TEXT,p,0,0,0,0);

	}


	SetWindowText(MS( IDS_ABOUT ));
	GetDlgItem(IDC_SUPPORT)->SetWindowText(MS( IDS_SUPPORT ));

	{
		char s[0x200];
		strcpy(s,MS(IDS_VERSION));
		strcat(s," " VER_PRODUCTVERSION_STR);
		GetDlgItem(IDC_VERSION)->SetWindowText(s);
		GetDlgItem(IDC_PRODUCT_NAME)->SetWindowText(VER_PRODUCTNAME_STR );
		GetDlgItem(IDC_COPYRIGHT)->SetWindowText(VER_LEGALCOPYRIGHT_STR "\n" VER_PORTIONCOPYRIGHT_STR VER_LC_STR);
	}
	
	HINSTANCE hiold=AfxGetResourceHandle();
	HINSTANCE hinew=0;
	if(LocFile.GetLength()){
		hinew=LoadLibrary(LocFile);
		if(hinew>0){
			AfxSetResourceHandle(hinew);
			if(m_David.LoadBitmap(IDB_DAVID))
				((CStatic*)GetDlgItem(IDB_DAVID))->SetBitmap(HBITMAP(m_David));
			FreeLibrary(hinew);
			AfxSetResourceHandle(hiold);
		}
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
*/
}


void CAvp32Dlg::OnHelpAboutavp32() 
{
	AboutDlg ad;
	ad.DoModal();
}


BOOL CAvp32Dlg::UpdateData( BOOL bSaveAndValidate)
{
	BOOL ret=CDialog::UpdateData(bSaveAndValidate);
	ret&=tPage1.UpdateData(bSaveAndValidate);
	ret&=tPage2.UpdateData(bSaveAndValidate);
	ret&=tPage3.UpdateData(bSaveAndValidate);
	ret&=tPage5.UpdateData(bSaveAndValidate);
	ret&=tPage6.UpdateData(bSaveAndValidate);

	return ret;
}

void CAvp32Dlg::OnTimer(UINT nIDEvent) 
{
	CString s;
	static int op=-1;
	static int ico=0;
	static int rep=0;
	static int ipcrep=0;
	if(nIDEvent==Timer)
	{
		CTimeSpan ts=CTime::GetCurrentTime()-StartTime;
		tPage5.m_Time=_T("");
		if(ts.GetHours())tPage5.m_Time+=ts.Format(_T("%H:"));
		tPage5.m_Time+=ts.Format(_T("%M:%S"));
		tPage5.m_Speed=(int)(tPage5.m_RealCountSize/1024)/(ts.GetTotalSeconds()+1);

		int percent=0;
		EnterCriticalSection(&TotalSpaceCriticalSection);
			if(TotalSpace!=0){
				percent=(int)((tPage5.m_CountSize*100)/TotalSpace);
			}
		LeaveCriticalSection(&TotalSpaceCriticalSection);
		if(percent>100)percent=100;

		m_Progress.SetPos(percent);
		
		if(IsIconic()){ 
			ico=1;
			if(op!=percent){
				op=percent;
				s.Format(MS(IDS_MIN_CAPTION),percent);
				SetWindowText(s);
				UpdateWindow();
			}
		}else if(ico){
			ico=0;
			SetWindowText(AppName);
			UpdateWindow();
			op=-1;
		}

		tPage5.UpdateData(FALSE);

		if((++rep%=20)==0 && hReportFile!=INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(hReportFile);
		}

		if(propByTimer && (++ipcrep%=(propInterval*4))==0)
		{
			MakeReport(s);
			if(CCClientSendStatistics)
				CCClientSendStatistics(s);
		}

	}
	else CDialog::OnTimer(nIDEvent);
}


void CAvp32Dlg::OnScanNowButton() 
{
	if(ScanThreadPtr)
		PostMessage(WM_COMMAND,ID_STOP);
	else
		PostMessage(WM_COMMAND,ID_SCAN_NOW);
}



const char* GetProfileName(BOOL Open,HWND m_hWnd ,const char* title, const char* ext){
	CFileDialog* p_FDProfile;
	static CString name=_T("*.prf");

	p_FDProfile=new CFileDialog( Open ,_T("prf"), name,
		OFN_EXPLORER | OFN_HIDEREADONLY 
		|(Open?OFN_FILEMUSTEXIST:(OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST)),ext);

	p_FDProfile->m_ofn.hwndOwner=m_hWnd;
	p_FDProfile->m_ofn.lpstrTitle=title;
	
		CString cwd;
		GetCurrentDirectory(512,cwd.GetBuffer(512));
		cwd.ReleaseBuffer();

	int ret=p_FDProfile->DoModal();

		SetCurrentDirectory(cwd);

	if(ret==IDOK){
		name=p_FDProfile->GetPathName();
		delete p_FDProfile;
		return name;
	}else{
		delete p_FDProfile;
		return NULL;
	}
}



BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int val, LPCTSTR lpFileName)
{
	CString string;
	string.Format(_T("%d"),val);
	return WritePrivateProfileString(lpAppName,lpKeyName,string,lpFileName);
}

BOOL WritePrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, BOOL val, LPCTSTR lpFileName)
{
	return WritePrivateProfileString(lpAppName,lpKeyName,val?_T("Yes"):_T("No"),lpFileName);
}

BOOL GetPrivateProfileBool(LPCTSTR lpAppName, LPCTSTR lpKeyName, BOOL initval, LPCTSTR lpFileName)
{
	CString string;
	GetPrivateProfileString(lpAppName,lpKeyName,initval?_T("Yes"):_T("No"),string.GetBuffer(0x500),0x500,lpFileName);
	string.ReleaseBuffer();
	string.MakeUpper();
	if(string==_T("NO"))return FALSE;
	if(string==_T("FALSE"))return FALSE;
	if(string==_T("0"))return FALSE;
	if(string==_T(""))return FALSE;
	return TRUE;
}


BOOL CAvp32Dlg::LoadProfile(const char* name)
{
	if(name==NULL)return FALSE;
	BOOL ret=TRUE;
	CString s;
	GetPrivateProfileString(_T("Location"),_T("List"),"",s.GetBuffer(0x500),0x500,name);
	s.ReleaseBuffer();

	PathArray.RemoveAll();
	int i=0;
	int ii=0;
	while(i<s.GetLength()){
		CString ss=LPCTSTR(s)+i;
		ii=ss.Find(';');
		if(ii==-1)ii=s.GetLength()-i;
		PathArray.Add(ss.Left(ii));
		i+=++ii;
	}


	tPage2.m_Memory=	GetPrivateProfileBool(	_T("Objects"),_T("Memory"),1,name);
	tPage2.m_Sectors=	GetPrivateProfileBool(	_T("Objects"),_T("Sectors"),1,name);
	tPage2.m_Files=		GetPrivateProfileBool(	_T("Objects"),_T("Files"),1,name);
	tPage2.m_Packed=	GetPrivateProfileBool(	_T("Objects"),_T("Packed"),1,name);
	tPage2.m_Archives=	GetPrivateProfileBool(	_T("Objects"),_T("Archives"),1,name);
	tPage2.m_MailBases=	GetPrivateProfileBool(	_T("Objects"),_T("MailBases"),1,name);
	tPage2.m_MailPlain=	GetPrivateProfileBool(	_T("Objects"),_T("MailPlain"),1,name);

	tPage2.m_Mask=		GetPrivateProfileInt(	_T("Objects"),_T("FileMask"),0,name);
	GetPrivateProfileString(_T("Objects"),_T("UserMask"),"",tPage2.m_UserMask.GetBuffer(0x500),0x500,name);
	tPage2.m_UserMask.ReleaseBuffer();

	tPage2.m_Exclude=	GetPrivateProfileBool(	_T("Objects"),_T("Exclude"),0,name);
	GetPrivateProfileString(_T("Objects"),_T("ExcludeMask"),"",tPage2.m_EditExclude.GetBuffer(0x500),0x500,name);
	tPage2.m_EditExclude.ReleaseBuffer();

	tPage6.m_Action=GetPrivateProfileInt(	_T("Actions"),_T("InfectedAction"),1,name);
	tPage6.m_CopyInfected=GetPrivateProfileBool(	_T("Actions"),_T("InfectedCopy"),0,name);
	GetPrivateProfileString(_T("Actions"),_T("InfectedFolder"),_T("Infected"),tPage6.m_sInfectFolder.GetBuffer(0x500),0x500,name);
	tPage6.m_sInfectFolder.ReleaseBuffer();
	tPage6.m_CopySuspicion=GetPrivateProfileBool(	_T("Actions"),_T("SuspiciousCopy"),0,name);
	GetPrivateProfileString(_T("Actions"),_T("SuspiciousFolder"),_T("Suspicious"),tPage6.m_sSuspFolder.GetBuffer(0x500),0x500,name);
	tPage6.m_sSuspFolder.ReleaseBuffer();

	tPage3.m_Warnings=	GetPrivateProfileBool(	_T("Options"),_T("Warnings"),1,name);
	tPage3.m_CA=		GetPrivateProfileBool(	_T("Options"),_T("CodeAnalyzer"),1,name);
	tPage3.m_Redundant=	GetPrivateProfileBool(	_T("Options"),_T("RedundantScan"),0,name);
	tPage3.m_ShowOK=	GetPrivateProfileBool(	_T("Options"),_T("ShowOK"),0,name);
	tPage3.m_ShowPack=	GetPrivateProfileBool(	_T("Options"),_T("ShowPack"),0,name);
	cust.m_Sound=		GetPrivateProfileBool(	_T("Options"),_T("Sound"),1,name);
	m_Track=		GetPrivateProfileBool(	_T("Options"),_T("Tracking"),1,name);
	m_RepView.SetAutoTracking(m_Track);
	m_tbToolbar.GetToolBarCtrl().CheckButton(ID_TRACK, m_Track);

	tPage3.m_Report=	GetPrivateProfileBool(	_T("Options"),_T("Report"),1,name);
	GetPrivateProfileString(	_T("Options"),_T("ReportFileName"),_T("Report.txt"),tPage3.m_Reportfile.GetBuffer(0x500),0x500,name);
	tPage3.m_Reportfile.ReleaseBuffer();
	tPage3.m_Append=	GetPrivateProfileBool(	_T("Options"),_T("Append"),1,name);
	tPage3.m_Limit=		GetPrivateProfileBool(	_T("Options"),_T("ReportFileLimit"),0,name);
	tPage3.m_Size=		GetPrivateProfileInt(	_T("Options"),_T("ReportFileSize"),500,name);

	cust.m_Sound=		GetPrivateProfileBool(	_T("Customize"),_T("Sound"),0,name);
	cust.m_CustDeleteAllMessage=		GetPrivateProfileBool(	_T("Customize"),_T("DeleteAllMessage"),1,name);
	cust.m_CustNextDiskPrompt=		GetPrivateProfileBool(	_T("Customize"),_T("NextDiskPrompt"),1,name);
	cust.m_CustPopupAfterFinish=		GetPrivateProfileBool(	_T("Customize"),_T("PopupAfterFinish"),1,name);
	cust.m_CustRedundantMessage=		GetPrivateProfileBool(	_T("Customize"),_T("RedundantMessage"),1,name);
	cust.m_CustSingleClick=		GetPrivateProfileBool(	_T("Customize"),_T("SingleClick"),0,name);
	cust.m_CustStatAfterBegin=		GetPrivateProfileBool(	_T("Customize"),_T("StatAfterBegin"),0,name);
	cust.m_CustStatAfterFinish=		GetPrivateProfileBool(	_T("Customize"),_T("StatAfterFinish"),1,name);
	cust.m_CustOtherMessages=		GetPrivateProfileBool(	_T("Customize"),_T("OtherMessages"),1,name);
	cust.m_UpdateInterval=		GetPrivateProfileInt(	_T("Customize"),_T("UpdateInterval"),14,name);
	cust.m_UpdateCheck=		GetPrivateProfileBool(	_T("Customize"),_T("UpdateCheck"),1,name);

	RECT r;
	GetWindowRect(&r);
	Xpos=(UINT)GetPrivateProfileInt(("Position"),("Xpos"),r.left,name);
	Ypos=(UINT)GetPrivateProfileInt(("Position"),("Ypos"),r.top,name);
	Xsize=(UINT)GetPrivateProfileInt(("Position"),("Xsize"),r.right-r.left,name);
	Ysize=(UINT)GetPrivateProfileInt(("Position"),("Ysize"),r.bottom-r.top,name);
	GetDesktopWindow()->GetWindowRect(&r);
	if((r.right-r.left-10)<Xpos)Xpos=30;
	if((r.bottom-r.top-10)<Ypos)Ypos=30;
	if(Xsize<MinSize.x)Xsize=MinSize.x;
	if(Ysize<MinSize.y)Ysize=MinSize.y;
	SetWindowPos(NULL,Xpos,Ypos,Xsize,Ysize,SWP_NOZORDER);
	UpdateWindow();


	UpdateData(FALSE);
	return ret;
}

BOOL CAvp32Dlg::SaveProfile(const char* name)
{
	if(name==NULL)return FALSE;
	BOOL ret=TRUE;
	UpdateData();
	
	CString string="";
	for(int i=0;i<PathArray.GetSize();){
		string+=PathArray[i];
		i++;
		if(i==PathArray.GetSize())break;
		string+=";";
	}
	ret&=WritePrivateProfileString(_T("Location"),_T("List"),string,name);

	ret&=WritePrivateProfileBool(	_T("Objects"),_T("Memory"),tPage2.m_Memory,name);
	ret&=WritePrivateProfileBool(	_T("Objects"),_T("Sectors"),tPage2.m_Sectors,name);
	ret&=WritePrivateProfileBool(	_T("Objects"),_T("Files"),tPage2.m_Files,name);
	ret&=WritePrivateProfileBool(	_T("Objects"),_T("Packed"),tPage2.m_Packed,name);
	ret&=WritePrivateProfileBool(	_T("Objects"),_T("Archives"),tPage2.m_Archives,name);
	ret&=WritePrivateProfileBool(	_T("Objects"),_T("MailBases"),tPage2.m_MailBases,name);
	ret&=WritePrivateProfileBool(	_T("Objects"),_T("MailPlain"),tPage2.m_MailPlain,name);
	ret&=WritePrivateProfileInt(	_T("Objects"),_T("FileMask"),tPage2.m_Mask,name);
	ret&=WritePrivateProfileString(	_T("Objects"),_T("UserMask"),tPage2.m_UserMask,name);

	ret&=WritePrivateProfileBool(	_T("Objects"),_T("Exclude"),tPage2.m_Exclude,name);
	ret&=WritePrivateProfileString(	_T("Objects"),_T("ExcludeMask"),tPage2.m_EditExclude,name);

	ret&=WritePrivateProfileInt(	_T("Actions"),_T("InfectedAction"),tPage6.m_Action,name);
	ret&=WritePrivateProfileBool(	_T("Actions"),_T("InfectedCopy"),tPage6.m_CopyInfected,name);
	ret&=WritePrivateProfileString(	_T("Actions"),_T("InfectedFolder"),tPage6.m_sInfectFolder,name);
	ret&=WritePrivateProfileBool(	_T("Actions"),_T("SuspiciousCopy"),tPage6.m_CopySuspicion,name);
	ret&=WritePrivateProfileString(	_T("Actions"),_T("SuspiciousFolder"),tPage6.m_sSuspFolder,name);

	ret&=WritePrivateProfileBool(	_T("Options"),_T("Warnings"),tPage3.m_Warnings,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("CodeAnalyzer"),tPage3.m_CA,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("RedundantScan"),tPage3.m_Redundant,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("ShowOK"),tPage3.m_ShowOK,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("ShowPack"),tPage3.m_ShowPack,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("Tracking"),m_Track,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("Report"),tPage3.m_Report,name);
	ret&=WritePrivateProfileString(	_T("Options"),_T("ReportFileName"),tPage3.m_Reportfile,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("Append"),tPage3.m_Append,name);
	ret&=WritePrivateProfileBool(	_T("Options"),_T("ReportFileLimit"),tPage3.m_Limit,name);
	ret&=WritePrivateProfileInt(	_T("Options"),_T("ReportFileSize"),tPage3.m_Size,name);

	ret&=SaveCustom(name);

	return ret;
}


void CAvp32Dlg::OnFileSaveprofile() 
{
	const char* s=GetProfileName(FALSE,m_hWnd,MS( IDS_SAVE_PROFILE ),MS( IDS_PROFILE_EXT ));
	if(s)SaveProfile(s);
}

void CAvp32Dlg::OnFileLoadprofile() 
{
	const char* s=GetProfileName(TRUE,m_hWnd,MS( IDS_LOAD_PROFILE ),MS( IDS_PROFILE_EXT ));
	if(s)LoadProfile(s);
}

void CAvp32Dlg::OnFileSaveprofileasdefault() 
{
	if(_access(prof,2))
		 SaveProfile("AVP32.INI");
	else SaveProfile(prof);
}

BOOL CAvp32Dlg::CopyToFolder(AVPScanObject* ScanObject,const char* folder )
{
	CString name=ScanObject->Name;
	if(ScanObject->InArc)name=name.Left(name.Find("/"));
	char* ptr;
	char buf[0x500];
	if(GetFullPathName(name,0x500,buf,&ptr)){
		CString Dest=folder;
		if(Dest[Dest.GetLength()-1]!='\\')Dest+='\\';
		Dest+=ptr;
		if(0==_access(Dest,00)) return TRUE;
		if(CopyFile(name, Dest, FALSE)) return TRUE;
	}
	return FALSE;
}








//DEL BOOL CAvp32Dlg::LoadKey(const char* filename)
//DEL {
//DEL 	CFileStatus fstat;
//DEL 	CTime ct;
//DEL 	CFile f;
//DEL 	BOOL ret=FALSE;
//DEL 
//DEL 	CString s;
//DEL 
//DEL 	char CurDir[512];
//DEL 	GetCurrentDirectory(512,CurDir);
//DEL 
//DEL 	char Name[512];
//DEL 	char* p=Name;
//DEL 
//DEL 	if(_access(filename,0)!=0)
//DEL 	{
//DEL 		GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,Name,&p);
//DEL 		*p=0;
//DEL 		SetCurrentDirectory(Name);
//DEL 	}
//DEL 	strcpy(p,filename);
//DEL 	
//DEL 	if(f.Open(Name,CFile::modeRead)){
//DEL 		AVP_KeyHeader KeyHeader;
//DEL 		AVP_Key* Key;
//DEL 		BYTE *cbuf;
//DEL 		BYTE *buf;
//DEL 		UINT i;
//DEL 		f.Read(&KeyHeader,sizeof(AVP_KeyHeader));
//DEL 		if(KeyHeader.Magic!=(DWORD)AVP_KEY_MAGIC)goto clof;
//DEL 		cbuf=new BYTE[KeyHeader.CompressedSize];
//DEL 		buf=new BYTE[KeyHeader.UncompressedSize];
//DEL 
//DEL 		f.Read(cbuf,KeyHeader.CompressedSize);
//DEL 		if(KeyHeader.CompressedCRC!=CalcSum(cbuf,KeyHeader.CompressedSize))goto clo;
//DEL 		for(i=0;i<KeyHeader.CompressedSize;i++) cbuf[i]^=(BYTE)i;
//DEL 		if(KeyHeader.UncompressedSize!=unsqu(cbuf,buf))goto clof;
//DEL 		ret=TRUE;
//DEL 		Key=(AVP_Key*)buf;
//DEL 
//DEL 		m_Name=buf+Key->NameOffs;
//DEL 		m_Name.OemToAnsi();
//DEL 
//DEL 		m_Org=buf+Key->OrgOffs;
//DEL 		m_Org.OemToAnsi();
//DEL 
//DEL 		m_Regnumber=buf+Key->RegNumberOffs;
//DEL 		m_Regnumber.OemToAnsi();
//DEL 
//DEL 		m_CopyInfo=buf+Key->CopyInfoOffs;
//DEL 		m_CopyInfo.OemToAnsi();
//DEL 
//DEL 
//DEL 		KeyDescription* kd;
//DEL 		kd=new KeyDescription;
//DEL 		if(kd)
//DEL 		{
//DEL 			memset(kd,0,sizeof(KeyDescription));
//DEL 			KeyHandleArray.Add(kd);
//DEL 			kd->Number=new char[m_Regnumber.GetLength()+1];
//DEL 			strcpy(kd->Number,m_Regnumber);
//DEL 			kd->Description=new char[m_CopyInfo.GetLength()+1];
//DEL 			strcpy(kd->Description,m_CopyInfo);
//DEL 			GetFullPathName(filename,512,Name,&p);
//DEL 			kd->FileName=new char[strlen(p)+1];
//DEL 			strcpy(kd->FileName,p);
//DEL 			kd->ExpirationDate.wDay=Key->ExpirDay;
//DEL 			kd->ExpirationDate.wMonth=Key->ExpirMonth;
//DEL 			kd->ExpirationDate.wYear=Key->ExpirYear;
//DEL 		
//DEL 			CString m_Support=buf+Key->SupportOffs;
//DEL 			m_Support.OemToAnsi();
//DEL 			kd->Support=new char[m_Support.GetLength()+1];
//DEL 			strcpy(kd->Support,m_Support);
//DEL 		}
//DEL 
//DEL 
//DEL 		m_Every=((Key->Flags) & KEY_F_INFOEVERYLAUNCH)!=0;
//DEL 		if(m_Every)	MessageBox(m_CopyInfo,AppName,MB_OK);
//DEL 
//DEL 		m_RemoteLaunch=((Key->Options) & KEY_O_REMOTELAUNCH)!=0;
//DEL 
//DEL 		if(!m_RemoteLaunch){
//DEL 			char* p;
//DEL 			CString s;
//DEL 			DWORD i=GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,s.GetBuffer(512),&p);
//DEL 			s.ReleaseBuffer();
//DEL 			if(GetDriveType(s.Left(3)) == DRIVE_REMOTE){
//DEL 				MessageBox(MS(IDS_REMOTE_LAUNCH),AppName,MB_ICONSTOP|MB_OK);
//DEL 				goto clo;
//DEL 			}
//DEL 		}
//DEL 		if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS && !(Key->Platforms & KEY_P_WIN95)){
//DEL 			MessageBox(MS(IDS_KEY_PLATHFORM_95),AppName,MB_ICONINFORMATION|MB_OK);
//DEL 			goto clo;
//DEL 		}
//DEL 		if(os.dwPlatformId==VER_PLATFORM_WIN32_NT && !(Key->Platforms & KEY_P_WINNT)){
//DEL 			MessageBox(MS(IDS_KEY_PLATHFORM_NT),AppName,MB_ICONINFORMATION|MB_OK);
//DEL 			goto clo;
//DEL 		}
//DEL 
//DEL 		if(Key->ExpirYear<2038 && Key->ExpirMonth && Key->ExpirDay)
//DEL 			m_LicenceDate=CTime(Key->ExpirYear,Key->ExpirMonth,Key->ExpirDay,0,0,0);
//DEL 		else
//DEL 			m_LicenceDate=CTime(2038,1,1,0,0,0);
//DEL 		m_LicenceAlarm=Key->AlarmDays;
//DEL 
//DEL 		ct=CTime::GetCurrentTime();
//DEL 		if(ct>m_LicenceDate)
//DEL 		{
//DEL 			MessageBox(m_LicenceDate.Format(MS( IDS_EXPIRIED )),AppName,MB_OK|MB_ICONSTOP);
//DEL 			goto clo;
//DEL 		}
//DEL 		ct=ct+CTimeSpan(m_LicenceAlarm,0,0,0);
//DEL 		if(ct > m_LicenceDate)
//DEL 			MessageBox(m_LicenceDate.Format(MS( IDS_EXPIR_ALARM )),AppName,MB_OK|MB_ICONINFORMATION);
//DEL 		
//DEL 		m_Disinfect=((Key->Options) & KEY_O_DISINFECT)!=0;
//DEL 		m_Unpack=((Key->Options) & KEY_O_UNPACK)!=0;
//DEL 		m_Extract=((Key->Options) & KEY_O_EXTRACT)!=0;
//DEL 		m_MailBases=((Key->Options) & KEY_O_MAILBASES)!=0;
//DEL 		m_MailPlain=((Key->Options) & KEY_O_MAILPLAIN)!=0;
//DEL //		if(m_MailPlain && m_MailBases)	AppName+=" & MailCheck";
//DEL 
//DEL 		m_CA=((Key->Options) & KEY_O_CA)!=0;
//DEL 		m_RemoteScan=((Key->Options) & KEY_O_REMOTESCAN)!=0;
//DEL 		m_Monitor=((Key->Options) & KEY_O_MONITOR)!=0;
//DEL 		m_CRC=((Key->Options) & KEY_O_CRC)!=0;
//DEL 
//DEL 
//DEL 		m_DOSLITE=((Key->Platforms) & KEY_P_DOSLITE)!=0;
//DEL 		m_DOS=((Key->Platforms) & KEY_P_DOS)!=0;
//DEL 		m_WIN31=((Key->Platforms) & KEY_P_WIN31)!=0;
//DEL 		m_WIN95=((Key->Platforms) & KEY_P_WIN95)!=0;
//DEL 		m_WINNT=((Key->Platforms) & KEY_P_WINNT)!=0;
//DEL 		m_OS2=((Key->Platforms) & KEY_P_OS2)!=0;
//DEL 		m_NOVELL=((Key->Platforms) & KEY_P_NOVELL)!=0;
//DEL 
//DEL 		if(os.dwPlatformId==VER_PLATFORM_WIN32_NT && !IsAdmin()
//DEL 			&& (-1 == CommandLine.Find(_T("/a "))))
//DEL 			if(cust.m_CustOtherMessages)
//DEL 			MessageBox(MS(IDS_ADMIN_RIGHTS),AppName,MB_ICONINFORMATION|MB_OK);
//DEL 
//DEL 		UpdateData(FALSE);
//DEL clo:
//DEL 		delete cbuf;
//DEL 		delete buf;
//DEL clof:
//DEL 		f.Close();
//DEL 	}
//DEL 	else{
//DEL 		MessageBox(MS(IDS_KEY_ABSENT),AppName,MB_ICONINFORMATION|MB_OK);
//DEL 	}
//DEL 
//DEL 	SetCurrentDirectory(CurDir);
//DEL 	return ret;
//DEL }

/////////////////////////////////////////////////////////////////////////////
// CSupportDlg dialog


CSupportDlg::CSupportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSupportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSupportDlg)
	m_Support = _T("");
	//}}AFX_DATA_INIT
}


void CSupportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSupportDlg)
	DDX_Text(pDX, IDC_SUPPORT, m_Support);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSupportDlg, CDialog)
	//{{AFX_MSG_MAP(CSupportDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSupportDlg message handlers

BOOL CSupportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(MS( IDS_SUPPORT_DLG ));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CAvp32Dlg::PutReportString(const char* str)
{
	if(hReportFile==INVALID_HANDLE_VALUE) return FALSE;
	CString s;
	ULONG bread;
	if(tPage3.m_Limit && ((GetFileSize(hReportFile,NULL)/1024)>tPage3.m_Size)){
		SetFilePointer(hReportFile,0,0,FILE_BEGIN);
		SetEndOfFile(hReportFile);
		s.Format(_T("%s\xd\xa"),MS( IDS_REPORT_CUT ));
		WriteFile(hReportFile,s,s.GetLength(),&bread,NULL);
	}
	s.Format(_T("%s\xd\xa"),str);
	return WriteFile(hReportFile,s,s.GetLength(),&bread,NULL);
}

static int in_fdlg=0;
void CAvp32Dlg::OnFind() 
{
	while(IDOK==FindDlg.DoModal()){
		DWORD f=0;
		if(FindDlg.m_Case)f|=RFF_MATCHCASE;
		if(FindDlg.m_WWord)f|=RFF_MATCHWHOLEWORD;
		m_RepView.FindFirstItem(FindDlg.m_String, f);
		BOOL ok;
		m_RepView.GetLastFindResult(&ok);
		if(ok){
			if(m_Track && (ScanThreadPtr||VirThreadPtr)) OnTrack();
			break;
		}
	}

/*
	if(m_List.GetItemCount()==0)return;
	if(	in_fdlg )return;
	in_fdlg++;
	while(IDOK==FindDlg.DoModal()){
		if(FindDlg.m_String.GetLength()){
			PostMessage(WM_COMMAND,ID_FINDAGAIN);
//			OnFindagain();
			break;
		}
	}
	in_fdlg--;
*/
}

void CAvp32Dlg::OnFindagain() 
{
	m_RepView.FindNextItem();
	BOOL ok;
	m_RepView.GetLastFindResult(&ok);
	if(!ok)OnFind();
	else if(m_Track && (ScanThreadPtr||VirThreadPtr)) OnTrack();



/*
	if(m_List.GetItemCount()==0)return;
	if(!FindDlg.m_String.GetLength()) {
		PostMessage(WM_COMMAND,ID_FIND);
//		OnFind();
		return;
	}

	CListCtrl& ctlList = (CListCtrl&) m_List;
	int fi=ctlList.GetNextItem(-1,LVNI_FOCUSED);
	if(fi == -1) fi=0;
	CString s;
	CString f=FindDlg.m_String;
	if(!FindDlg.m_Case)f.MakeLower();

	int i=fi;

	while(fi != (i=ctlList.GetNextItem(i,LVNI_ALL)))
	{
		for(int j=0;j<2;j++){
			s=ctlList.GetItemText(i,j);
			if(s.GetLength()){
				if(!FindDlg.m_Case)s.MakeLower();
				if(-1!=s.Find(f)){
					while(-1 != (j=ctlList.GetNextItem(-1,LVNI_FOCUSED)))
							ctlList.SetItemState(j,0,LVIS_FOCUSED);
					while(-1 != (j=ctlList.GetNextItem(-1,LVNI_SELECTED)))
							ctlList.SetItemState(j,0,LVIS_SELECTED);
							
					ctlList.SetItemState(i,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
					ctlList.EnsureVisible(i,FALSE);
					ctlList.SetFocus();
					return;
				}
			}
		}
	}
		PostMessage(WM_COMMAND,ID_FIND);
	return;
*/
}

void CAvp32Dlg::OnTrack() 
{
	UpdateData();
	m_Track=!m_Track;
	m_RepView.SetAutoTracking(m_Track);
	m_tbToolbar.GetToolBarCtrl().CheckButton(ID_TRACK, m_Track);
	UpdateData(FALSE);
}

void CAvp32Dlg::OnAddDisks()
{
	tPage1.AddDisks();
}

void CAvp32Dlg::OnPopMark() 
{
	int i=-1;
	while(-1 != (i=tPage1.m_lDisk.GetNextItem(i,LVNI_SELECTED))){
		tPage1.m_lDisk.SetItemState(i,INDEXTOOVERLAYMASK(1),LVIS_OVERLAY);
	}
}

void CAvp32Dlg::OnPopUnmark() 
{
	int i=-1;
	while(-1 != (i=tPage1.m_lDisk.GetNextItem(i,LVNI_SELECTED))){
		tPage1.m_lDisk.SetItemState(i,0,LVIS_OVERLAY);
	}
}

void CAvp32Dlg::OnPopScan() 
{
	int i=-1;
	while(-1 != (i=tPage1.m_lDisk.GetNextItem(i,LVNI_SELECTED)))
	{
		CString s=tPage1.m_lDisk.GetItemText(i,0);
		if(tPage1.m_lDisk.GetItemData(i)>1)	s=s.Left(2);
		ScanList.Add(s);
	}
	PostMessage(WM_COMMAND,ID_SCAN_NOW);
}

typedef int (*fp_f)(CWnd* w = 0,long lParam = 0);

void CAvp32Dlg::OnToolsUpdatenow() 
{
	if(ExitCode!=7)
	if(!(functionality & FN_UPDATES))
	{
		MessageBox(MS(IDS_UPDATE_EXPIRED),AppName);
		return;
	}


	HKEY hKey;
	char szFolder[MAX_PATH];
	DWORD n = MAX_PATH;

	if ((RegOpenKey(HKEY_LOCAL_MACHINE,AVP_REGKEY_SHAREDFILES_FULL,&hKey) != ERROR_SUCCESS) || 
		(RegQueryValueEx(hKey,AVP_REGVALUE_FOLDER,NULL,NULL,(UCHAR*)szFolder,&n) != ERROR_SUCCESS))
	{
		strcpy(szFolder,StartPath);
	}
	RegCloseKey(hKey);
	int l=sizeof(szFolder);
	if(l && szFolder[l-1]!='\\')strcat(szFolder,"\\");

	char bb[0x200];
#ifdef _DEBUG
	sprintf(bb,"\"%savpupd.exe\" /ini=\"%s\" /id=101",szFolder,IniFile);
#else
	sprintf(bb,"\"%savpupd.exe\"",szFolder);
#endif
	WinExec(bb,SW_SHOW);
}

void CAvp32Dlg::OnHelpAvpontheweb() 
{
	ShellExecute(m_hWnd, "open", MS(IDS_AVP_URL), NULL, NULL, SW_SHOWNORMAL);
}


void CAvp32Dlg::OnCustomize() 
{
	if(IDOK==cust.DoModal())
	{
		if(_access(prof,2))
			 SaveCustom("AVP32.INI");
		else SaveCustom(prof);
	}

}

BOOL CAvp32Dlg::DisableControlsVirlist(BOOL dis)
{
//	GetDlgItem(IDC_TRACK)->ShowWindow(dis?SW_SHOW:SW_HIDE);
	GetMenu()->EnableMenuItem(ID_SCAN_NOW,dis?MF_GRAYED:MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_STOP,dis?MF_GRAYED:MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_TOOLS_UPDATENOW,dis?MF_GRAYED:MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_APP_EXIT,dis?MF_GRAYED:MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_VIRLIST,dis?MF_GRAYED:MF_ENABLED);
//	GetMenu()->EnableMenuItem(ID_FIND,(dis /*|| m_List.GetItemCount()*/)?MF_ENABLED:MF_GRAYED);
	GetDlgItem(IDC_SCAN_NOW)->EnableWindow(!dis);
	return dis;
}

UINT Virlist( LPVOID pParam ){
	CAvp32Dlg* mDlg=(CAvp32Dlg*)pParam;
	mDlg->DisableControlsVirlist(1);
	mDlg->OpenReportFile();
	AVP_SetCallbackNames(1);
	mDlg->OnAvpReloadbases();
	AVP_SetCallbackNames(0);
	CloseHandle(mDlg->hReportFile);
	mDlg->DisableControlsVirlist(0);
	KillScan=0;
	mDlg->VirThreadPtr=NULL;
	if(should_reload)
		mDlg->SendMessage(WM_COMMAND,ID_AVP_RELOADBASES,0);
	return 0;
}
void CAvp32Dlg::OnVirlist() 
{
	if(ScanThreadPtr||VirThreadPtr)return;
	StatusLine("");
	m_Progress.SetPos(0);
//	m_List.DeleteAllItems();
	m_RepView.ClearReportView();
	m_RepView.SetAutoTracking(m_Track);
	m_tbToolbar.GetToolBarCtrl().CheckButton(ID_TRACK, m_Track);

	VirThreadPtr=AfxBeginThread(Virlist,this);
}

BOOL CAvp32Dlg::OpenReportFile()
{
	if(hReportFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hReportFile);
		hReportFile=INVALID_HANDLE_VALUE;
	}

	if(tPage3.m_Report){
		CString rf="";
		if(strnicmp((const char*)tPage3.m_Reportfile+1,":\\",2) && strnicmp(tPage3.m_Reportfile,"\\\\",2))
		{
			rf=StartPath;
			if(rf[rf.GetLength()-1]!='\\')rf+='\\';
		}
		rf+=tPage3.m_Reportfile;	

		hReportFile=CreateFile(rf,
		GENERIC_WRITE,FILE_SHARE_READ,NULL,
		(tPage3.m_Append)?OPEN_ALWAYS:CREATE_ALWAYS,0,NULL);
		if(hReportFile==INVALID_HANDLE_VALUE)
			SendMessage(wm_AVP,AVP_CALLBACK_ERROR_FILE_OPEN,(long)(const char*)rf);
	}
	ReportWithTiming=0;

	if(hReportFile!=INVALID_HANDLE_VALUE){
		if(tPage3.m_Limit && (GetFileSize(hReportFile,NULL)/1024)>tPage3.m_Size){
			SetFilePointer(hReportFile,0,0,FILE_BEGIN);
			SetEndOfFile(hReportFile);
		}
		SetFilePointer(hReportFile,0,0,FILE_END);

		CTime t=CTime::GetCurrentTime();
		CString s=t.Format(MS( IDS_REPORT_BANNER ));
		PutReportString(s);

		HKEY hKey;
		DWORD Type,cbData;
        if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\KasperskyLab\\AVP32",0,KEY_QUERY_VALUE,&hKey))
        {
			Type=REG_DWORD;
			cbData=sizeof(ReportWithTiming);
			RegQueryValueEx(hKey,"ReportWithTiming",NULL,&Type,(LPBYTE)&ReportWithTiming,&cbData);
			RegCloseKey(hKey);
        }
		if(ReportWithTiming){
			LARGE_INTEGER Freq={0,0};
			HiResCounterSupported=0;
			if(QueryPerformanceFrequency(&Freq))
			{
				HiResCounterSupported=1;
				s.Format("High performance timing\x9tick=1/%d of second\n",Freq.LowPart);
			}else s.Format("Low performance timing\x9tick=1 millisecond\n");

			PutReportString(s);
		}

		return TRUE;
	}
	return FALSE;
}


/*
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Zones\0
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Zones\1
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Zones\2
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Zones\3
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Zones\4

HKEY_CURRENT_USER\Software\Microsoft\Office\8.0\Word\Options\EnableMacroVirusProtection
HKEY_CURRENT_USER\Software\Microsoft\Office\8.0\Excel\Microsoft Excel\Options6 (0,8)
HKEY_CURRENT_USER\Software\Microsoft\Office\9.0\Excel\Options\Options6

*/

//IE Security Keys
char szIERegKey[128]="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones";
char szIEBadScriptKey[]="1201";
char szIEBadActiveXKey[]="1004";
char szIEZoneName[]="DisplayName";

//Office Security Keys
char szOfficeBaseKey[]="Software\\Microsoft\\Office\\8.0";
char szWordKey[]="\\Word\\Options";
char szWordValue[]="EnableMacroVirusProtection";
char szExcel8Key[]="\\Excel\\Microsoft Excel";
char szExcel9Key[]="\\Excel\\Options";
char szExcelValue[]="Options6";
char szPPointKey[]="\\PowerPoint\\Options";
char szPPointValue[]="MacroVirusProtection";

// Security Hole Keys
char szSecureHole1RegKey[]="CLSID\\{06290BD5-48AA-11D2-8432-006008C3FBFC}\\Implemented Categories\\{7DD95801-9882-11CF-9FA9-00AA006C42C4}";


int CAvp32Dlg::AVPRegistryChecker(void)
{
HKEY hKey;
DWORD Type,cbData,dwData;
char szBuffer[MAX_PATH];
char szKeyName[MAX_PATH];
char res[0x200];
        
		if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CLASSES_ROOT,szSecureHole1RegKey,0,KEY_QUERY_VALUE,&hKey))
        {
                RegCloseKey(hKey);
				PutLogString(MS( IDS_REG_TYPELIB ),MS( IDS_REG_PATCH_NOT_INSTALLED ));
				PutLogString("",MS( IDS_REG_PATCH_AVAILABLE ));
				PutLogString("",MS( IDS_REG_PATCH_LOCATION ));
		}

//Try to check IE keys
        if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szIERegKey,0,KEY_QUERY_VALUE,&hKey))
        {
                RegCloseKey(hKey);
                lstrcat(szIERegKey,"\\0");
                for(char i='1';i<'5';i++)
                {
                        szIERegKey[lstrlen(szIERegKey)-1]=i;
                        if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szIERegKey,0,KEY_QUERY_VALUE,&hKey))
                        {
                                Type=REG_DWORD;
                                cbData=sizeof(dwData);
                                if(ERROR_SUCCESS==RegQueryValueEx(hKey,szIEBadScriptKey,NULL,&Type,(LPBYTE)&dwData,&cbData))
                                {
                                        if(dwData==0)
                                        {
                                                szBuffer[0]=0;
                                                Type=REG_SZ;
                                                cbData=sizeof(szBuffer);
                                                RegQueryValueEx(hKey,szIEZoneName,NULL,&Type,(LPBYTE)szBuffer,&cbData);
                                                sprintf(res,MS( IDS_REG_SECURITY_WEAK ),szBuffer);
												PutLogString(MS( IDS_REG_IE_OPTIONS ),res);
                                        }
                                }
                                Type=REG_DWORD;
                                cbData=sizeof(dwData);
                                if(ERROR_SUCCESS==RegQueryValueEx(hKey,szIEBadActiveXKey,NULL,&Type,(LPBYTE)&dwData,&cbData))
                                {
			                           if(dwData==0)
                                        {
                                                szBuffer[0]=0;
                                                Type=REG_SZ;
                                                cbData=sizeof(szBuffer);
                                                RegQueryValueEx(hKey,szIEZoneName,NULL,&Type,(LPBYTE)szBuffer,&cbData);
                                                sprintf(res,MS( IDS_REG_SECURITY_WEAK ),szBuffer);
												PutLogString(MS( IDS_REG_IE_OPTIONS ),res);
                                        }
                                }
                                RegCloseKey(hKey);
                        }
                }
        }
//Try to check Office 8 keys
        szOfficeBaseKey[lstrlen(szOfficeBaseKey)-3]='8';

        if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szOfficeBaseKey,0,KEY_QUERY_VALUE,&hKey))
        {
                RegCloseKey(hKey);
//Word 8
                lstrcpy(szKeyName,szOfficeBaseKey);
                lstrcat(szKeyName,szWordKey);
                if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_QUERY_VALUE,&hKey))
                {
                        Type=REG_SZ;
                        cbData=sizeof(szBuffer);
                        if(ERROR_SUCCESS==RegQueryValueEx(hKey,szWordValue,NULL,&Type,(LPBYTE)szBuffer,&cbData))
                        {
                                if(szBuffer[0]!='1')
								{
										PutLogString(MS( IDS_REG_W97_OPTIONS ),MS( IDS_REG_MACRO_PROTECTION_DISABLED ));
								}
                        }
                        RegCloseKey(hKey);
                }
//Excel
                lstrcpy(szKeyName,szOfficeBaseKey);
                lstrcat(szKeyName,szExcel8Key);
                if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_QUERY_VALUE,&hKey))
                {
                        Type=REG_DWORD;
                        cbData=sizeof(dwData);
                        if(ERROR_SUCCESS==RegQueryValueEx(hKey,szExcelValue,NULL,&Type,(LPBYTE)&dwData,&cbData))
                        {
                                if((dwData & 8)!=8)
										PutLogString(MS( IDS_REG_E97_OPTIONS ),MS( IDS_REG_MACRO_PROTECTION_DISABLED ));
                        }
                        RegCloseKey(hKey);
                }
//PPoint
                lstrcpy(szKeyName,szOfficeBaseKey);
                lstrcat(szKeyName,szPPointKey);
                if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_QUERY_VALUE,&hKey))
                {
                        Type=REG_DWORD;
                        cbData=sizeof(dwData);
                        if(ERROR_SUCCESS==RegQueryValueEx(hKey,szPPointValue,NULL,&Type,(LPBYTE)&dwData,&cbData))
                        {
                                if(dwData!=1)
										PutLogString(MS( IDS_REG_PP97_OPTIONS ),MS( IDS_REG_MACRO_PROTECTION_DISABLED ));
                        }
                        RegCloseKey(hKey);
                }
        }

        szOfficeBaseKey[lstrlen(szOfficeBaseKey)-3]++;
//Try to check Office 9 keys
        if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szOfficeBaseKey,0,KEY_QUERY_VALUE,&hKey))
        {
                RegCloseKey(hKey);
//Word 9
                lstrcpy(szKeyName,szOfficeBaseKey);
                lstrcat(szKeyName,szWordKey);
                if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_QUERY_VALUE,&hKey))
                {
                        Type=REG_DWORD;
                        cbData=sizeof(dwData);
                        if(ERROR_SUCCESS==RegQueryValueEx(hKey,szWordValue,NULL,&Type,(LPBYTE)&dwData,&cbData))
                        {
                                if(dwData!=1)
										PutLogString(MS( IDS_REG_W2000_OPTIONS ),MS( IDS_REG_MACRO_PROTECTION_DISABLED ));
                        }
                        RegCloseKey(hKey);
                }
//Excel
                lstrcpy(szKeyName,szOfficeBaseKey);
                lstrcat(szKeyName,szExcel9Key);
                if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_QUERY_VALUE,&hKey))
                {
                        Type=REG_DWORD;
                        cbData=sizeof(dwData);
                        if(ERROR_SUCCESS==RegQueryValueEx(hKey,szExcelValue,NULL,&Type,(LPBYTE)&dwData,&cbData))
                        {
                                if((dwData & 8)!=8)
										PutLogString(MS( IDS_REG_E2000_OPTIONS ),MS( IDS_REG_MACRO_PROTECTION_DISABLED ));
                        }
                        RegCloseKey(hKey);
                }
//PPoint
                lstrcpy(szKeyName,szOfficeBaseKey);
                lstrcat(szKeyName,szPPointKey);
                if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,szKeyName,0,KEY_QUERY_VALUE,&hKey))
                {
                        Type=REG_DWORD;
                        cbData=sizeof(dwData);
                        if(ERROR_SUCCESS==RegQueryValueEx(hKey,szPPointValue,NULL,&Type,(LPBYTE)&dwData,&cbData))
                        {
                                if(dwData != 1)
										PutLogString(MS( IDS_REG_PP2000_OPTIONS ),MS( IDS_REG_MACRO_PROTECTION_DISABLED ));
                        }
                        RegCloseKey(hKey);
                }
        }
        return 0;
}



BOOL CAvp32Dlg::SaveCustom(const char* name)
{
	BOOL ret=TRUE;
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("Sound"),cust.m_Sound,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("DeleteAllMessage"),cust.m_CustDeleteAllMessage,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("NextDiskPrompt"),cust.m_CustNextDiskPrompt,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("PopupAfterFinish"),cust.m_CustPopupAfterFinish,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("RedundantMessage"),cust.m_CustRedundantMessage,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("SingleClick"),cust.m_CustSingleClick,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("StatAfterBegin"),cust.m_CustStatAfterBegin,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("StatAfterFinish"),cust.m_CustStatAfterFinish,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("OtherMessages"),cust.m_CustOtherMessages,name);//
	ret&=WritePrivateProfileBool(	_T("Customize"),_T("UpdateCheck"),cust.m_UpdateCheck,name);//

	CRect r;
	char buf[20];
	GetWindowRect(&r);
	ret&=WritePrivateProfileString(("Position"),("Xpos"),_itoa(r.left,buf,10),name);
	ret&=WritePrivateProfileString(("Position"),("Ypos"),_itoa(r.top,buf,10),name);
	ret&=WritePrivateProfileString(("Position"),("Xsize"),_itoa(r.right-r.left,buf,10),name);
	ret&=WritePrivateProfileString(("Position"),("Ysize"),_itoa(r.bottom-r.top,buf,10),name);
	ret&=WritePrivateProfileString(	_T("Customize"),_T("UpdateInterval"),_itoa(cust.m_UpdateInterval,buf,10),name);//

	return ret;
}

LONG CAvp32Dlg::OnUpdateData(UINT uParam, LONG lParam)
{
	UpdateData(FALSE);
	return 1;
}

const void AddNextSring(CString &str,const char* s)
{
	str+=s; str+="\xd\xa";
}

const char* CAvp32Dlg::MakeReport(CString &str)
{
	str.Empty();
	CString s;

	char szDate[0x100], szTime[0x100];
	SYSTEMTIME st;
	StartTime.GetAsSystemTime(st);
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, szTime, 0x100);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szDate, 0x100);

	s.Format(_T("%s %s"),szDate,szTime);
	AddNextSring(str,s);
	s.Format(_T("%s %s\x9%s %s"),szDate,szTime,MS( IDS_FULL_NAME ),MS( IDS_STARTED ));
	AddNextSring(str,s);
	memset(szTime,'_',70);szTime[70]=0;
	AddNextSring(str,szTime);
	AddNextSring(str,_T(""));

	AddNextSring(str,MS( IDS_TAB5_SCANNED_FRAME ));
	AddNextSring(str,_T(""));

	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_SECTORS ),tPage5.m_Sectors);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_FILES ),tPage5.m_Files);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_FOLDERS ),tPage5.m_Directories);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_ARCHIVES ),tPage5.m_Archives);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_PACKED ),tPage5.m_Packed);
	AddNextSring(str,s);

	AddNextSring(str,_T(""));
	AddNextSring(str,MS( IDS_TAB5_FOUND_FRAME ));
	AddNextSring(str,_T(""));
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_VIRUSES ),tPage5.m_Virus);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_BODIES ),tPage5.m_Bodies);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_DISINFECTED ),tPage5.m_Disinfected);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_DELETED ),tPage5.m_Deleted);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_WARNINGS ),tPage5.m_Warnings);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_SUSPIC ),tPage5.m_Suspicious);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_CORRUPTED ),tPage5.m_Corrupted);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_IOERRORS ),tPage5.m_Errors);
	AddNextSring(str,s);

	AddNextSring(str,_T(""));
	s.Format(_T("%s\x9%d"),MS( IDS_TAB5_STATIC_SPEED ),tPage5.m_Speed);
	AddNextSring(str,s);
	s.Format(_T("%s\x9%s"),MS( IDS_TAB5_STATIC_TIME ),tPage5.m_Time);
	AddNextSring(str,s);

	if(!ScanThreadPtr){
	memset(szTime,'_',70);szTime[70]=0;
	AddNextSring(str,szTime);
	AddNextSring(str,_T(""));

	CTime t=CTime::GetCurrentTime();
	t.GetAsSystemTime(st);
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, szTime, 0x100);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szDate, 0x100);
	s.Format(_T("%s %s\x9%s %s"),szDate,szTime,MS( IDS_FULL_NAME ),MS( IDS_FINISHED ));
	AddNextSring(str,s);
	}

	return str;
}

BOOL CAvp32Dlg::SendStatisticsByEvent(DWORD event)
{
	if(propByEvent && event<=propEvent)
	{
		CString s;
		MakeReport(s);
		if(CCClientSendStatistics)
			CCClientSendStatistics(s);
		return TRUE;
	}
	return FALSE;
}

int CAvp32Dlg::SignReportMB(const char *filename, int ret, int mode)
{
	switch(ret)
	{
	case SIGN_NOTARY_NOT_FOUND:
//		ret=SIGN_OK;
//		break;

	case SIGN_NOT_MATCH:
		char mess[0x200];
		sprintf(mess,MS(IDS_AUTH_FAILED),filename);
		if(mode==0)
		{
			strcat(mess,"\n\n");
			strcat(mess,MS(IDS_AUTH_CONTINUE));
		}
		if(ExitCode!=7)
		if(IDYES==MessageBox(mess,AppName,(mode?MB_OK:MB_YESNO)|MB_ICONEXCLAMATION|MB_TOPMOST|MB_DEFBUTTON2))
			ret=SIGN_OK;
		break;

	case SIGN_OK:
	default: 
		break;
	}
	if(ret){
		ExitCode=7;
		PostMessage(WM_CLOSE);
	}
	return ret;
}

int CAvp32Dlg::SignCheckFile(const char *filename)
{
	const char* name=filename;
	char buf[_MAX_PATH];
	int ret=sign_check_file( name, 1, NULL,0, 0);
	if(ret==SIGN_CANT_READ){
		strcpy(buf,StartPath);
		strcat(buf,filename);
		name=buf;
		ret=sign_check_file( name, 1, NULL,0, 0);
	}
	return ret;
}

void CAvp32Dlg::CancelProcess()
{
	AVP_CancelProcessObject(1);
	KillScan=1;
	if(pddlg)pddlg->SendMessage(WM_CLOSE,0,0);
}

BEGIN_EVENTSINK_MAP(CAvp32Dlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CAvp32Dlg)
	ON_EVENT(CAvp32Dlg, IDC_REPV, 5 /* RVCGetLocalisedString */, OnRVCGetLocalisedString, VTS_I4 VTS_PBSTR)
	ON_EVENT(CAvp32Dlg, IDC_REPV, 6 /* RVCGetFindContext */, OnRVCGetFindContext, VTS_PBSTR VTS_PI4 VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CAvp32Dlg::OnRVCGetFindContext(BSTR FAR* pContext, long FAR* pdwFlags, BOOL FAR* pbDone) 
{
	OnFind();
	*pbDone = FALSE;
}

void CAvp32Dlg::OnRVCGetLocalisedString(long nLocaleID, BSTR FAR* pString) 
{
	CString S;
	switch(nLocaleID)
	{
	case IDS_WAS_FINDITEM:
		S=MS(IDS_FIND);
		break;
	case IDS_WAS_FINDNEXTITEM:
		S=MS(IDS_FINDNEXT);
		break;
	default: return;
	}
	SysFreeString(*pString);
	*pString = S.AllocSysString();

/*	CString S = LoadStr(nLocaleID);

	SysFreeString(*pString);
	*pString = S.AllocSysString();
	// TODO: Add your control notification handler code here
*/	
}


BEGIN_EVENTSINK_MAP(AboutDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(AboutDlg)
	ON_EVENT(AboutDlg, IDC_AVPABOUTVIEW, 1 /* ABTGetLocalisedString */, OnABTGetLocalisedStringAvpaboutview, VTS_I4 VTS_PBSTR)
	ON_EVENT(AboutDlg, IDC_AVPABOUTVIEW, 2 /* ABTGetGeneralSupportInfo */, OnABTGetGeneralSupportInfoAvpaboutview, VTS_PBSTR)
	ON_EVENT(AboutDlg, IDC_AVPABOUTVIEW, 3 /* ABTDisplayContextHelp */, OnABTDisplayContextHelpAvpaboutview, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void AboutDlg::OnABTGetLocalisedStringAvpaboutview(long nStringID, BSTR FAR* pString) 
{
	CString S;

	switch(nStringID)
	{
	case IDS_ABT_REGISTRATION_TITLE:
		S=MS( IDS_REGISTERINFO );
		break;
	case IDS_ABT_NAME_TITLE:
		S=MS( IDS_NAME );
		break;
	case IDS_ABT_ORGANIZATION_TITLE:
		S=MS( IDS_ORGANIZATION );
		break;
	case IDS_ABT_SUPPORT_BTN:
		S=MS(IDS_SUPPORT);
		break;
	case IDS_ABT_KEY_NUMBER: 
		S=MS(IDS_LIST_KEYS_NUMBER);
		break;
	case IDS_ABT_KEY_LICENCE_NAME:
		S=MS(IDS_LIST_KEYS_PLNAME);
		break;
	case IDS_ABT_KEY_INFO:
		S=MS(IDS_LIST_KEYS_DESCRIPTION);
		break;
	case IDS_ABT_KEY_EXPIRATION:
		S=MS(IDS_LIST_KEYS_EXPIRATION);
		break;
	case IDS_ABT_KEY_FILE:
		S=MS(IDS_LIST_KEYS_FILE);
		break;
	case IDS_ABT_KEY_EXPIRED:
		S=MS(IDS_EXPIRED);
		break;
	default: 
		return;
	}
	
	SysFreeString(*pString);
	*pString = S.AllocSysString();
}

void AboutDlg::OnABTGetGeneralSupportInfoAvpaboutview(BSTR FAR* pSupportInfo) 
{
	// TODO: Add your control notification handler code here
}
void AboutDlg::OnABTDisplayContextHelpAvpaboutview(LONG id)
{
//	WinHelp(0,HIDD(IDD_ABOUT_DIALOG));
	WinHelp(0,0x20000 & IDD_ABOUT_DIALOG);
}
