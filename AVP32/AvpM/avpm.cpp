//  Ctrl95.cpp
//  AVP Monitor control application.


#define _WIN32_IE 0x0300
#include "stdafx.h"
#include "resource.h"
#include "vxd_msg.h"

#include <Version/ver_avp.h>

#include <AVPRegID.h>
#include <ScanAPI/Avpioctl.h>
#include <LoadKeys/key.h>
#include <ScanAPI/Scanobj.h>
#include <Stuff/LoadVxd.c>
#include <Stuff/calcsum.h>
#include "../avp_aler.h"
#include <LoadKeys/LoadKeys.h>
#include "../fn.h" 

#include <Sign/sign.h>
#define SIGN_BUF_LEN 0x2000
#define  SignCloseHandle	CloseHandle
#define  SignReadFile		ReadFile
#define  SignCreateFile		CreateFile 
#include <Sign/a_chfile.c>

//////////////////////////////////////////////////////////////////////////////////////
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include "pid.h"
#include "agentint.h"
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//////////////////////////////////////////////////////////////////////////////////////

#define DVPSUBDRV_AVP95             0x01            // AVP95.VXD
#define DVPSUBDRV_F95               0x02            // F95.VXD
#define DVPSUBDRVFN_LOAD            0x00            // load driver            
#define DVPSUBDRVFN_UNLOAD          0x01            // unload driver

// Win32 DeviceIoControl function IDs

#define DVPIOC_GETVERSION			0x00000777      // get version
#define DVPIOC_ENABLESCAN			0x00000200		// enable/disable scanning
#define DVPIOC_SETOPTION			0x00000210		// set option
#define DVPIOC_SETOPTION2			0x00000211		// set buffer/struct option
#define DVPIOC_SETTINGSCHANGED		0x00000400		// notify dvp.vxd about changed settings
#define DVPIOC_GETSCANCOUNTS		0x00000401		// retrieve counters of scanned objects
#define DVPIOC_SETLANGUAGE			0x00000501		// set language

#define	DVPIOC_ISSERVER				0x00000889	    // is the gatekeeper device a server version
#define	DVPIOC_SCANBOOTSECTOR		0x00000890	    // scan the desired bootsector



// option IDs, for both IOC and PM API

#define DVPOPTION_SCANFLOPPIES			0x0000		// scan floppies
#define DVPOPTION_SCANHDDS				0x0001		// scan local hard drives
#define DVPOPTION_SCANNETDRIVES			0x0002		// scan net drives
#define DVPOPTION_SCAN					0x0007		// enable scan
#define DVPOPTION_ACTION				0x0010		// action
#define DVPOPTION_PROMPTUSER			0x0018		// prompt user before denying access to infected file

#define DVPOPTION_SCANEXTENSIONS		0x8000		// extensions for executable (scanned) files
#define DVPOPTION_DRIVETYPES			0x8001		// drive type flags (not an option really...)
#define DVPOPTION_INSTPATH          0x8002          // set path to installation directory
#define DVPOPTION_SCANONCREATERENAME 0x8003          // scan on create/rename

#include "setopt.cpp"



#define MY_LRESULT int


static	OSVERSIONINFO	OsVersion;

/////////// APC stufff
BOOL UnderCC=0;
BOOL ResLimited=0;
HANDLE hApcDevice=INVALID_HANDLE_VALUE;
HANDLE doGetThreadHandle=INVALID_HANDLE_VALUE;
HANDLE callbackHandle=INVALID_HANDLE_VALUE;
DWORD doGetThreadId;

BOOL KillScanThread=0;
DWORDLONG TotalSpace;
DWORDLONG ProcessedSpace;
BOOL SpaceCritSec=0;
CRITICAL_SECTION TotalSpaceCritSec;
CRITICAL_SECTION ProcessedSpaceCritSec;


CRITICAL_SECTION callbackCriticalSection;
HANDLE pollingEvent=INVALID_HANDLE_VALUE;
DWORD __stdcall doGetCallbacks(void* );
BOOL APC_Init(HANDLE hDevice_);
BOOL APC_Destroy(HANDLE hDevice_);
void __stdcall APC_CallbackProc(DWORD lpData);

BOOL AlertAVPScanObject(AVPScanObject* so, BOOL crit=1);

static BOOL NT_CallbackProc(void)
{
	DWORD count;
	AVPScanObject so;
	if(DeviceIoControl(hApcDevice, IOCTL_AVP_CALLBACK_SYNC_DATA, 0, 0, &so, sizeof(AVPScanObject), &count, 0 ))
	{
		AlertAVPScanObject(&so);
		return TRUE;
	}
	return FALSE;
}

void __stdcall APC_CallbackProc(DWORD lpData)
{
	DWORD count;
	AlertAVPScanObject((AVPScanObject*)lpData);
	DeviceIoControl(hApcDevice, IOCTL_AVP_CALLBACK_SYNC_DATA, &lpData, 4, 0, 0, &count, 0 );
}

BOOL APC_Init(HANDLE hDevice_)
{
	hApcDevice = hDevice_;
	if(hApcDevice==INVALID_HANDLE_VALUE)return 0;
	InitializeCriticalSection(&callbackCriticalSection);
	pollingEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (pollingEvent==INVALID_HANDLE_VALUE)
		return 0;
	
	doGetThreadHandle=(HANDLE) CreateThread( NULL, 4096,
		(DWORD  (__stdcall *) (void *) ) doGetCallbacks,
		NULL, 0, (DWORD*) &doGetThreadId);

    if (doGetThreadHandle==INVALID_HANDLE_VALUE){
		SetEvent(pollingEvent);
		return 0;
	}
	return 1;
}

BOOL APC_Destroy(HANDLE hDevice_)
{
	DWORD count;
	if(hDevice_==INVALID_HANDLE_VALUE)return 0;
	if(callbackHandle!=INVALID_HANDLE_VALUE)
		DeviceIoControl(hDevice_, IOCTL_AVP_CALLBACK_DELETE, &callbackHandle, 4, 0, 0, &count, 0 );
	SetEvent(pollingEvent);
	if(doGetThreadHandle!=INVALID_HANDLE_VALUE)
		CloseHandle(doGetThreadHandle);
	if(hApcDevice!=INVALID_HANDLE_VALUE)
		DeleteCriticalSection(&callbackCriticalSection);
	return 1;
}
    

DWORD __stdcall doGetCallbacks(void*){
	DWORD count;
	PVOID initData;
	if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_NT)
			initData=pollingEvent;
	else	initData=&APC_CallbackProc;

	BOOL ret=DeviceIoControl((HANDLE)hApcDevice, IOCTL_AVP_CALLBACK_CREATE, &initData, 4, &callbackHandle, 4, &count, 0 );
	if(!ret)return 0;
	
	if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_NT){
		do{
			WaitForSingleObject(pollingEvent, INFINITE);
			while(NT_CallbackProc());
		}while(1);
	}
	else 
		while(WaitForSingleObjectEx(pollingEvent, INFINITE, TRUE) == WAIT_IO_COMPLETION );

	CloseHandle(pollingEvent);

	return 1;
}


// End of APC stuff

LRESULT WINAPI AlertDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_);

static SYSTEMTIME stStartTime;
static BOOL	Exitting=0;
static CPtrArray DetectArray;
static CPtrArray KeyHandleArray;
static int sign_check;
static DWORD functionality=0;
static BOOL TryToDisinfect=1;
static BOOL NeedAddIcon=0;

static	char	execName[MAX_PATH]="";
static	HBITMAP KLBmpH;
static	UINT	m_BaseDelay=14;
static	char	*m_Support;
static	char	m_Name[MAX_PATH];
static	char	m_Comp[MAX_PATH];

static	BOOL	startupMem=1;
static	BOOL	startupMbr=1;
static	BOOL	startupBoot=1;
static	BOOL	startupSelf=1;

static BOOL HelpCalled=0;
static HCURSOR hCursor;

const UINT WM_AVPVXD=RegisterWindowMessage(AVP_VXD_MSGSTRING);
const UINT WM_VXDAVP=RegisterWindowMessage(VXD_AVP_MSGSTRING);
UINT WM_AVPUPD=RegisterWindowMessage("UM_RELOADBASE");
const UINT WM_TASKBARCREATED = RegisterWindowMessage("TaskbarCreated");


#define MYWM_NOTIFYICON WM_USER+0xAD
#define MYWM_PROCICON	WM_USER+0xAE
#define WP_PROPERTIES   WM_USER+0xAF
#define MYWM_SELFSCAN   WM_USER+0xB0
#define MYWM_AFTERINIT  WM_USER+0xB1
#define MYWM_LOADBASE   WM_USER+0xB2
#define WM_PLAY_SOUND	WM_USER+0xC0

static HINSTANCE ghInstance;
static HICON hicon_, hdisicon_;

static LPSTR lpCmdLine=NULL;
static LPSTR StartPath=NULL;
static LPSTR FullINIFileName=NULL;
static LPSTR FullHELPFileName=NULL;
static LPSTR FullSETFileName=NULL;
static LPSTR FullKEYFileName=NULL;
static LPSTR IniFile=NULL;
static char* RFName=NULL;
static char* LimitSize=NULL;
static char* UserMasks=NULL;
static char* ExcludeMasks=NULL;
static char* LimitCompoundSize=NULL;
static DWORD LimitSizeInBytes=0;

static CPtrArray MessageArray;

DWORD propByTimer=0;
DWORD propByEvent=0;
DWORD propEvent=0;
DWORD propInterval=0;

#define MS(x) ((char*)MessageArray[x-IDS_ENABLE])

HWND hMainDlg=NULL;

#ifdef SPLASH_
HWND hSplash=NULL;
#endif

static HWND hPrShDlg=0;
static HWND hGeneralPage=0;
static HWND hAboutPage=0;

static int nStartPage=0;
static int Xpos=30, Ypos=30;

static BOOL disableMode=0;
static BOOL noExitMode=0;

static BOOL bSettings;
static UINT nIDEvent3; 
static UINT nIDEvent; 
static HANDLE hDevice=INVALID_HANDLE_VALUE;
static HANDLE hDeviceGK=INVALID_HANDLE_VALUE;
static HANDLE hDeviceIO=INVALID_HANDLE_VALUE;
static AVPStat sStat;
static DWORD ObjectsChecked;
static AVPBaseInfo BaseInfo;
static char LastUpdate[0x200];

static UINT EnableCheck,sEnableCheck,savedEnableCheck;
static UINT Packed,sPacked;
static UINT Archives,sArchives;
static UINT MailBases,sMailBases;
static UINT MailPlain,sMailPlain;
static UINT FileMask,sFileMask;	// 0 - smart;  1 - Programs; 2 - all files; 3 - User defined
static UINT Actions,sActions;
static UINT Analyser,sAnalyser;
static UINT Alert,sAlert;
static UINT Warnings,sWarnings;
static UINT Report,sReport;
static UINT Limit,sLimit;
static UINT LimitCompound,sLimitCompound;
static UINT Exclude,sExclude;
static DWORD MFlags;
static BOOL bApplied=0;

static void LoadLocalStrings();

static BOOL	GKServiceLoadedByMonitor=0;
static BOOL	AVPServiceLoadedByMonitor=1;
static RECT AlertRect;

void ApplyNow();
BOOL LoadSettings();
BOOL WriteSettings();
void UpdateSettings();
void RetrSettings();
void SaveMasks(char* str,DWORD ctlcode);
DWORD SetMFlags(DWORD& mf);
DWORD GetMFlags(DWORD& mf);
LRESULT WINAPI SupDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_);
void CreateMenuItem(MENUITEMINFO& mii, UINT wId, LPSTR str, UINT state=MFS_ENABLED);
BOOL CreateMenuBox(HWND wnd_, HMENU menu_);
//LoadKey(const char* filename);
extern DWORD unsqu (BYTE *i_buf,BYTE *o_buf);


BOOL scanThreadPaused=0;
HANDLE scanThread=NULL;

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

int SignReportMB(char* filename, int ret, HWND hwnd_=NULL, int mode=0);
int SignCheckFile(char* filename);

int SignReportMB(char* filename, int ret, HWND hwnd_, int mode)
{
		switch(ret)
	{
	default: 
	case SIGN_NOT_MATCH:
		char mess[0x200];
		sprintf(mess,MS(IDS_AUTH_FAILED),filename);
		if(mode==0)
		{
			strcat(mess,"\n\n");
			strcat(mess,MS(IDS_AUTH_CONTINUE));
		}
		if(IDYES==MessageBox(hwnd_,mess,MS(IDS_FULL_NAME),(mode?MB_OK:MB_YESNO)|MB_ICONEXCLAMATION|MB_TOPMOST|MB_DEFBUTTON2))
			ret=SIGN_OK;
		break;

	case SIGN_OK:
		break;
	}
	return ret;
}

int SignCheckFile(char* filename)
{
	char* name=filename;
	char buf[MAX_PATH];
	int ret=sign_check_file( name, 1, NULL,0, 0);
	if(ret==SIGN_CANT_READ){
		strcpy(buf,StartPath);
		strcat(buf,filename);
		name=buf;
		ret=sign_check_file( name, 1, NULL,0, 0);
	}
	return ret;
}

DWORD DriverFailedAndClose()
{
	if(Exitting) return 0;
	if(CCClientSendAlert)
		CCClientSendAlert(AVP_ALERT_ERROR_INT,MS(IDS_ERRLOADVXD));
	MessageBox(hMainDlg,MS(IDS_ERRLOADVXD),MS(IDS_AVPM_ERROR),MB_ICONSTOP|MB_TOPMOST);
	PostMessage(hMainDlg, WM_CLOSE, 0, 0);
	return 0;
}

static const void AddNextSring(char* str,const char* s)
{
	strcat(str,s);
	strcat(str,"\xd\xa");
}

static void MakeSendStatistics()
{
	char s[0x200];
	char str[0x800];
	*str=0;
	AVPStat Stat;
	DWORD count;
	if (!DeviceIoControl(hDevice, IOCTL_AVP_GET_STAT, 0, 0, (void*)&Stat, sizeof(AVPStat), &count, 0 ))
		return;
	memcpy(&sStat,&Stat,sizeof(AVPStat));

	char szDate[0x100], szTime[0x100];
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &stStartTime, NULL, szTime, 0x100);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &stStartTime, NULL, szDate, 0x100);

	sprintf(s,"%s %s",szDate,szTime);
	AddNextSring(str,s);
	sprintf(s,"%s %s\x9%s %s",MS( IDS_FULL_NAME ),MS( IDS_STARTED ),szDate,szTime);
	AddNextSring(str,s);
	memset(szTime,'_',70);szTime[70]=0;
	AddNextSring(str,szTime);
	AddNextSring(str,"");

	sprintf(s,"%s",Stat.Enable? MS( IDS_REP_ENABLED): MS( IDS_REP_DISABLED));
	AddNextSring(str,s);
	sprintf(s,"%s\x9%d",MS( IDS_CHECKED ),Stat.Objects);
	AddNextSring(str,s);
	sprintf(s,"%s\x9%d",MS( IDS_INFECTED ),Stat.Infected);
	AddNextSring(str,s);
	sprintf(s,"%s\x9%d",MS( IDS_DISINFECTED ),Stat.Disinfected);
	AddNextSring(str,s);
	sprintf(s,"%s\x9%d",MS( IDS_DELETED ),Stat.Deleted);
	AddNextSring(str,s);
	sprintf(s,"%s\x9%s",MS( IDS_LAST_CHOBJECT ),Stat.LastObjectName);
	AddNextSring(str,s);
	sprintf(s,"%s\x9%s",MS( IDS_LAST_INFOBJECT ),Stat.LastInfectedObjectName);
	AddNextSring(str,s);
	sprintf(s,"%s\x9%s",MS( IDS_LAST_VIRUSN ),Stat.LastVirusName);
	AddNextSring(str,s);

	if(CCClientSendStatistics)
		CCClientSendStatistics(str);
}

BOOL SendStatisticsByEvent(DWORD event)
{
	if(propByEvent && event<=propEvent)
	{
		MakeSendStatistics();
		return TRUE;
	}
	return FALSE;
}



void SaveWindowPos(){
	if(!hPrShDlg) return;
	RECT r;
	GetWindowRect(hPrShDlg,&r);
	Xpos=r.left;
	Ypos=r.top;

	char buf[20];
	WritePrivateProfileString(("Position"),("X"),_itoa(Xpos,buf,10),FullINIFileName);
	WritePrivateProfileString(("Position"),("Y"),_itoa(Ypos,buf,10),FullINIFileName);
}

typedef int (*fp_f)(HWND w = 0,long lParam = 0);


HBITMAP HDAVID=NULL;
HBITMAP HMEDIA1=NULL;
HBITMAP HMEDIA2=NULL;

typedef LRESULT (CALLBACK *MyProcPtr)(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_);

MyProcPtr oldDavidDlgProc;

LRESULT CALLBACK DavidDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	if(msg_==WM_PAINT){
		BITMAP bitmap;
		HBITMAP HMEDIA=(HBITMAP)GetWindowLong(hwnd_,GWL_USERDATA);
		if(HMEDIA)
		{
			PAINTSTRUCT ps;
			GetObject(HMEDIA,sizeof(BITMAP),&bitmap);
			HDC dc=BeginPaint(hwnd_,&ps);
			HDC memdc=CreateCompatibleDC(dc);
			HGDIOBJ oldobj=SelectObject(memdc,HMEDIA);
			BitBlt(dc,0,0,bitmap.bmWidth,bitmap.bmHeight,memdc,0,0,SRCCOPY);
			SelectObject(memdc,oldobj);
			DeleteDC(memdc);
			EndPaint(hwnd_,&ps);
	
			return 0;
		}
	}
	
	return oldDavidDlgProc(hwnd_, msg_, wparam_, lparam_);
}

static UINT nIDEvent0; //timer identifier
static sPercent=-1;

SetGeneralButtons()
{
	if(hGeneralPage)
	{

		SetWindowText(GetDlgItem(hGeneralPage,IDC_SCAN), scanThread?MS(IDS_SCAN_STOP):MS(IDS_SCAN_START));
		SetWindowText(GetDlgItem(hGeneralPage,IDC_PAUSE), scanThreadPaused?MS(IDS_SCAN_CONTINUE):MS(IDS_SCAN_PAUSE));
		EnableWindow(GetDlgItem(hGeneralPage,IDC_PAUSE), scanThread!=NULL && !!(functionality));
		EnableWindow(GetDlgItem(hGeneralPage,IDC_SCAN), !!(functionality));

		EnableWindow(GetDlgItem(hGeneralPage,IDC_UPDATENOW ), !disableMode);
		EnableWindow(GetDlgItem(hGeneralPage,IDC_ENABLE), !disableMode && (scanThreadPaused || scanThread==NULL));
		EnableWindow(GetDlgItem(hGeneralPage,IDC_UNLOAD), !noExitMode );

		CheckDlgButton(hGeneralPage,IDC_ENABLE,EnableCheck);
		UpdateWindow(hGeneralPage);
	}
	char s[0x200];
	strcpy(s,MS(IDS_FULL_NAME));
	if(scanThread){
		strcat(s,": ");
		strcat(s,scanThreadPaused?MS(IDS_SCAN_PAUSED):MS(IDS_SCANNING));
	}
	SetWindowText(hPrShDlg,s);

	return 0;
}

BOOL ReportWriteLine(char* s_);
BOOL TrayMessage(HWND hwnd_, DWORD dwMessage, HICON hIcon, PSTR pszTip)
{
	if(pszTip==NULL) pszTip=MS( IDS_FULL_NAME );
    
	BOOL res;
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= hwnd_;
	tnd.uID			= IDR_MAINFRAME;
	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage	= MYWM_NOTIFYICON;
	tnd.hIcon		= hIcon;

	if (pszTip)	lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	else tnd.szTip[0] = '\0';
	
	res = Shell_NotifyIcon(dwMessage, &tnd);
	
	return res;
}


BOOL EnableStateChanged()
{
	DWORD count;
	if (!DeviceIoControl(hDevice,IOCTL_AVP_ENABLE,&EnableCheck,4,NULL,0,&count,0))
		return DriverFailedAndClose();

	ReportWriteLine(EnableCheck?MS(IDS_REP_ENABLED):MS(IDS_REP_DISABLED));
	SendMessage(hMainDlg, WM_SETICON, ICON_BIG, (LPARAM)(EnableCheck?hicon_:hdisicon_));
	SendMessage(hMainDlg, WM_SETICON, ICON_SMALL, (LPARAM)(EnableCheck?hicon_:hdisicon_));
	TrayMessage(hMainDlg, NIM_MODIFY, EnableCheck?hicon_:hdisicon_, EnableCheck?MS(IDS_TIP_ENABLE):MS(IDS_TIP_DISABLE));

	if(hGeneralPage){
		CheckDlgButton(hGeneralPage,IDC_ENABLE,EnableCheck);
		SendMessage(hPrShDlg, WM_SETICON, ICON_SMALL, (LPARAM)(EnableCheck?hicon_:hdisicon_));
	}
	return EnableCheck;
}


MY_LRESULT WINAPI SettingsDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch(msg_)
	{
		case WM_DESTROY:			
			KillTimer(hwnd_,nIDEvent0);
			hGeneralPage=NULL;
			break;

		case WM_INITDIALOG:			
			hGeneralPage=hwnd_;
			if(nStartPage==0) SetWindowPos(hPrShDlg,HWND_TOP,Xpos,Ypos,0,0,SWP_NOSIZE);

			if(HMEDIA1 || HDAVID)
			{
				SetWindowLong(GetDlgItem(hwnd_,IDC_DAVID),GWL_USERDATA,(LONG)(HMEDIA1?HMEDIA1:HDAVID));
				oldDavidDlgProc=(MyProcPtr)SetWindowLong(GetDlgItem(hwnd_,IDC_DAVID),GWL_WNDPROC,(LONG)DavidDlgProc);
			}
			
			SendMessage(hwnd_,WM_SETFONT,0,1);
			/////////////////////Localisation////////////////////////
			SetWindowText(GetDlgItem(hwnd_,IDC_ENABLE), MS(IDS_ENABLE));
			SetWindowText(GetDlgItem(hwnd_,IDC_UPDATENOW), MS(IDS_UPDATENOW));
			if(AVPServiceLoadedByMonitor)
				SetWindowText(GetDlgItem(hwnd_,IDC_UNLOAD), MS(IDS_UNLOAD));
			else
				SetWindowText(GetDlgItem(hwnd_,IDC_UNLOAD), MS(IDS_EXIT));

			SetGeneralButtons();
			SendMessage(GetDlgItem(hwnd_,IDC_PROGRESS), PBM_SETRANGE, 0,MAKELPARAM(0, 100));

			
			//////////////////////////////////////////////////////////
			
			sEnableCheck=EnableCheck;
			CheckDlgButton(hwnd_,IDC_ENABLE,sEnableCheck);

			return 0;

		case WM_TIMER:
			{
			if(wparam_!=nIDEvent0) return 1;

			int percent=0;
			if(scanThread)
			{
				EnterCriticalSection(&ProcessedSpaceCritSec);
				EnterCriticalSection(&TotalSpaceCritSec);
					if(TotalSpace!=0){
						percent=(int)((ProcessedSpace*100)/TotalSpace);
					}
				LeaveCriticalSection(&ProcessedSpaceCritSec);
				LeaveCriticalSection(&TotalSpaceCritSec);
			}
			if(percent>100)percent=100;
			if(percent==sPercent)	return 0;
			sPercent=percent;
			
			SendMessage(GetDlgItem(hwnd_,IDC_PROGRESS), PBM_SETPOS, percent,0);

			UpdateWindow(hwnd_);
			return 0;
			}


		case  WM_NOTIFY:
			switch (((LPNMHDR) lparam_)->code){
				case PSN_APPLY: 					
					SaveWindowPos();
					ApplyNow();
					break;

				case PSN_SETACTIVE: 
					nIDEvent0=1;
					SetTimer(hwnd_, nIDEvent0, 250, NULL);
					nStartPage=0; 
					break;

				case PSN_KILLACTIVE:
					KillTimer(hwnd_,nIDEvent0);
					SetWindowLong(hwnd_,DWL_MSGRESULT,FALSE);
					break;

				case PSN_HELP:
					WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0103); 
					HelpCalled=1;
					break;
				case PSN_QUERYCANCEL:{
					SaveWindowPos();
					RetrSettings();
					break;
				}
				default: break;
			}
			break;

		case WM_COMMAND:
			switch(wparam_){
			case IDC_UNLOAD:
				PostMessage(hMainDlg, WM_SYSCOMMAND, SC_CLOSE, 0);
				return 0;

			case IDC_UPDATENOW:
			case IDC_SCAN:
			case IDC_PAUSE:
				PostMessage(hMainDlg, msg_, wparam_,0);
				return 1;

			case IDC_ENABLE:
				sEnableCheck=(IsDlgButtonChecked(hwnd_, IDC_ENABLE)==BST_CHECKED);
				break;

			default: 
				break;
			}
			PropSheet_Changed(hPrShDlg, hwnd_);
			bApplied=0;
			break;
		
		case WM_KEYDOWN:
			if((int)wparam_!=VK_F1) break;
		case WM_HELP:
			WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0103); 
			HelpCalled=1;
			return 1;
	}
	return 0;
}


MY_LRESULT WINAPI SettingsDlgProc5(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch(msg_)
	{
		case WM_DESTROY:			
			hAboutPage=NULL;
			break;

		case WM_INITDIALOG:			
			hAboutPage=hwnd_;

			if(nStartPage==5) SetWindowPos(hPrShDlg,HWND_TOP,Xpos,Ypos,0,0,SWP_NOSIZE);

			SendMessage(hwnd_,WM_SETFONT,0,1);
			/////////////////////Localisation////////////////////////
			{
				char s[0x200];
				strcpy(s,MS(IDS_VERSION_NEW));
				strcat(s," " VER_PRODUCTVERSION_STR);
				SetWindowText(GetDlgItem(hwnd_,IDC_VERSION), s);
				SetWindowText(GetDlgItem(hwnd_,IDC_PRODUCTNAME), VER_PRODUCTNAME_STR );
				SetWindowText(GetDlgItem(hwnd_,IDC_COPYRIGHT), VER_LEGALCOPYRIGHT_STR "\n" VER_PORTIONCOPYRIGHT_STR VER_DF_STR ", " VER_LC_STR);
				
			}
			SetWindowText(GetDlgItem(hwnd_,IDC_S_REG), MS(IDS_REGISTERINFO));
			SetWindowText(GetDlgItem(hwnd_,IDC_SUPPORT), MS(IDS_SUPPORT_DLG));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_NAME), MS(IDS_NAME));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_ORG), MS(IDS_ORGANIZATION));
			//////////////////////////////////////////////////////////
			SetWindowText(GetDlgItem(hwnd_,IDC_NAME), m_Name);
			SetWindowText(GetDlgItem(hwnd_,IDC_ORG), m_Comp);

			SetWindowText(GetDlgItem(hwnd_,IDC_UPDATE), LastUpdate);

			HWND lv;
			lv=GetDlgItem(hwnd_,IDC_LIST_KEYS);
			ListView_SetExtendedListViewStyle(lv, ListView_GetExtendedListViewStyle(lv) | LVS_EX_FULLROWSELECT);

			LVITEM lvi;
			LVCOLUMN lvc;
			lvc.mask=LVCF_TEXT|LVCF_WIDTH;
			lvc.cchTextMax=100;
			lvc.cx=130;
			lvc.pszText=MS(IDS_LIST_KEYS_NUMBER);
			ListView_InsertColumn(lv,0,&lvc);
			lvc.cx=100;
			lvc.pszText=MS(IDS_LIST_KEYS_PLNAME);
			ListView_InsertColumn(lv,1,&lvc);
			lvc.cx=300;
			lvc.pszText=MS(IDS_LIST_KEYS_DESCRIPTION);
			ListView_InsertColumn(lv,2,&lvc);
			lvc.cx=100;
			lvc.pszText=MS(IDS_LIST_KEYS_EXPIRATION);
			ListView_InsertColumn(lv,3,&lvc);
			lvc.cx=100;
			lvc.pszText=MS(IDS_LIST_KEYS_FILE);
			ListView_InsertColumn(lv,4,&lvc);
			int i;
			for(i=0;i<KeyHandleArray.GetSize();i++)
			{
				char buf[0x200];
				HDATA hKey=(HDATA)KeyHandleArray[i];
				AVP_dword dseq[5];
				lvi.mask=LVIF_TEXT;
				lvi.iItem=i;
				lvi.pszText=buf;
				
				*buf=0;
				DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,PID_SERIALSTR,0), 0, buf, 0x200) ;
				lvi.iSubItem=0;
				ListView_InsertItem(lv,&lvi);

				*buf=0;
				DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYPLPOSNAME,0), 0, buf, 0x200);
				lvi.iSubItem=1;
				ListView_SetItem(lv,&lvi);

				*buf=0;
				if(DATA_Find(hKey, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYISTRIAL,0)))
					strcpy(buf,"TRIAL!  ");
				DATA_Get_Val(hKey,DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYLICENCEINFO,0), 0, buf+strlen(buf), 0x200);
				lvi.iSubItem=2;
				ListView_SetItem(lv,&lvi);

				SYSTEMTIME stExpir;
				memset(&stExpir,0,sizeof(SYSTEMTIME));
				*buf=0;
				if(DATA_Get_Val( hKey, DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYEXPDATE,0), 0, &stExpir, sizeof(AVP_date)) )
				{
					if(stStartTime.wYear > stExpir.wYear) goto time_bad;
					if(stStartTime.wYear < stExpir.wYear) goto time_ok;
					if(stStartTime.wMonth > stExpir.wMonth) goto time_bad;
					if(stStartTime.wMonth < stExpir.wMonth) goto time_ok;
					if(stStartTime.wDay > stExpir.wDay) goto time_bad;
					if(stStartTime.wDay < stExpir.wDay) goto time_ok;
		time_bad:
					strcpy(buf,"Expired");
					goto ok;
		time_ok:
					tm tm_exp;
					tm_exp.tm_mday=stExpir.wDay;
					tm_exp.tm_mon=stExpir.wMonth-1;
					tm_exp.tm_year=stExpir.wYear-1900;

					strftime(buf,0x200,"%x",&tm_exp);
		ok:			;
				}
				lvi.iSubItem=3;
				ListView_SetItem(lv,&lvi);

				*buf=0;
				DATA_Get_Val(hKey,DATA_Sequence(dseq,PID_FILENAME,0), 0, buf, 0x200);
				GetFullPathName(buf,0x200,buf,&lvi.pszText);
				lvi.iSubItem=4;
				ListView_SetItem(lv,&lvi);

			}
			lvi.iItem=0;
			lvi.iSubItem=0;
			lvi.mask=LVIF_STATE;
			lvi.state=LVIS_SELECTED|LVIS_FOCUSED;
			lvi.stateMask=LVIS_SELECTED|LVIS_FOCUSED;
			ListView_SetItem(lv,&lvi);

			return 0;

		case  WM_NOTIFY:
			switch (((LPNMHDR) lparam_)->code){
				case PSN_APPLY: 					
					SaveWindowPos();
					ApplyNow();
					break;
				case PSN_SETACTIVE: 
					nStartPage=5;
					DWORD count;
					if(DeviceIoControl(hDevice,IOCTL_AVP_GET_BASE_INFO,NULL,0,&BaseInfo,sizeof(AVPBaseInfo),&count,0)){

						sprintf(LastUpdate,MS(IDS_LAST_UPDATE),BaseInfo.LastUpdateDay,BaseInfo.LastUpdateMonth,
						BaseInfo.LastUpdateYear,BaseInfo.NumberOfRecords);
						SetWindowText(GetDlgItem(hAboutPage,IDC_UPDATE), LastUpdate);
					}
					else DriverFailedAndClose();
					
					
					return 1;
				case PSN_KILLACTIVE:
					SetWindowLong(hwnd_,DWL_MSGRESULT,FALSE);
					break;
				case PSN_HELP:
					WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0109); 
					HelpCalled=1;
					break;
				case PSN_QUERYCANCEL:{
					SaveWindowPos();
					RetrSettings();
					break;
				}
				default: break;
			}
			break;

		case WM_COMMAND:
			switch(wparam_){
			case IDC_SUPPORT:
				{
					m_Support="";
					int i=ListView_GetNextItem(GetDlgItem(hwnd_,IDC_LIST_KEYS),-1,LVNI_SELECTED);
					if(i!=-1) 
					{
						AVP_dword dseq[5];
						int l=DATA_Get_Val( (HDATA)KeyHandleArray[i], DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSUPPORTINFO,0), 0, 0, 0);
						if(l){
							char* p=new char[l];
							if(p){
								if(DATA_Get_Val( (HDATA)KeyHandleArray[i], DATA_Sequence(dseq,AVP_PID_KEYINFO,AVP_PID_KEYSUPPORTINFO,0), 0, p, l))
								{
									m_Support=p;
									DialogBox(ghInstance, MAKEINTRESOURCE(IDD_SUPPORT_DIALOG), hwnd_, (DLGPROC)SupDlgProc);
								}
								delete p;
							}
						}
					}
				}
				return 0;
			default: break;
			}
			break;
		
		case WM_KEYDOWN:
			if((int)wparam_!=VK_F1) break;
		case WM_HELP:
			WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0109); 
			HelpCalled=1;
			return 1;
	}
	return 0;
}

MY_LRESULT WINAPI SettingsDlgProc1(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch(msg_)
	{
		case WM_INITDIALOG:{			
			if(nStartPage==1) SetWindowPos(hPrShDlg,HWND_TOP,Xpos,Ypos,0,0,SWP_NOSIZE);
			SendMessage(hwnd_,WM_SETFONT,0,1);

			SetWindowText(GetDlgItem(hwnd_,IDC_COMPOUNDFILES), MS(IDS_COMPOUNDFILES));
			SetWindowText(GetDlgItem(hwnd_,IDC_LIMITCOMPOUND), MS(IDS_LIMITCOMPOUND));
			SetWindowText(GetDlgItem(hwnd_,IDC_LIMITCOMPOUNDSIZE), LimitCompoundSize);
			EnableWindow(GetDlgItem(hwnd_,IDC_LIMITCOMPOUNDSIZE), LimitCompound);
			if(LimitCompound)CheckDlgButton(hwnd_,IDC_LIMITCOMPOUND,1);

			SetWindowText(GetDlgItem(hwnd_,IDC_EXCLUDE), MS(IDS_EXCLUDE));
			SetWindowText(GetDlgItem(hwnd_,IDC_EDIT_EXCLUDE), ExcludeMasks);
			EnableWindow(GetDlgItem(hwnd_,IDC_EDIT_EXCLUDE), Exclude);
			if(Exclude)CheckDlgButton(hwnd_,IDC_EXCLUDE,1);

			SetWindowText(GetDlgItem(hwnd_,IDC_PACKED), MS(IDS_PACKED));
			SetWindowText(GetDlgItem(hwnd_,IDC_ARCHIVES), MS(IDS_ARCHIVE));
			SetWindowText(GetDlgItem(hwnd_,IDC_MAILBASES), MS(IDS_MAILBASES));
			SetWindowText(GetDlgItem(hwnd_,IDC_MAILPLAIN), MS(IDS_MAILPLAIN));
			SetWindowText(GetDlgItem(hwnd_,IDC_FILEMASK), MS(IDS_FILE_MASK));
			SetWindowText(GetDlgItem(hwnd_,IDC_SMARTMASK), MS(IDS_SMART));
			SetWindowText(GetDlgItem(hwnd_,IDC_PROGRAMS), MS(IDS_PROGRAMS));
			SetWindowText(GetDlgItem(hwnd_,IDC_ALLFILES), MS(IDS_ALLFILES));
			SetWindowText(GetDlgItem(hwnd_,IDC_USERDEF), MS(IDS_USERDEF));
			SetWindowText(GetDlgItem(hwnd_,IDC_USERMASK), UserMasks);

			//EnableWindow(GetDlgItem(hwnd_,IDC_ARCHIVES),  !!(functionality & FN_OPTIONS));
			//EnableWindow(GetDlgItem(hwnd_,IDC_PACKED),    !!(functionality & FN_OPTIONS));
			//EnableWindow(GetDlgItem(hwnd_,IDC_MAILBASES), !!(functionality & FN_OPTIONS));
			//EnableWindow(GetDlgItem(hwnd_,IDC_MAILPLAIN), !!(functionality & FN_OPTIONS));

			CheckDlgButton(hwnd_,IDC_ARCHIVES, /*!!(functionality & FN_EXTRACT) &&*/ Archives);
			CheckDlgButton(hwnd_,IDC_PACKED, /*!!(functionality & FN_UNPACK) &&*/ Packed);
			CheckDlgButton(hwnd_,IDC_MAILBASES, /*!!(functionality & FN_MAIL) &&*/ MailBases);
			CheckDlgButton(hwnd_,IDC_MAILPLAIN, /*!!(functionality & FN_MAIL) &&*/ MailPlain);

			BOOL b=FALSE;
			switch(FileMask)
			{
			case 0: CheckRadioButton(hwnd_, IDC_SMARTMASK, IDC_USERDEF, IDC_SMARTMASK);break;  
			case 1: CheckRadioButton(hwnd_, IDC_SMARTMASK, IDC_USERDEF, IDC_PROGRAMS);break;
			case 2: CheckRadioButton(hwnd_, IDC_SMARTMASK, IDC_USERDEF, IDC_ALLFILES);break; 
			case 3: CheckRadioButton(hwnd_, IDC_SMARTMASK, IDC_USERDEF, IDC_USERDEF);b=TRUE;break;
			}
			EnableWindow(GetDlgItem(hwnd_,IDC_USERMASK), b);

			
			if(disableMode){
				EnableWindow(GetDlgItem(hwnd_,IDC_PACKED), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_ARCHIVES), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_MAILPLAIN), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_MAILBASES), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_SMARTMASK), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_PROGRAMS), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_ALLFILES), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_USERDEF), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_USERMASK), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_LIMITCOMPOUNDSIZE), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_LIMITCOMPOUND), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_EXCLUDE), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_EDIT_EXCLUDE), FALSE);
			}

			return 0;
		}

		case  WM_NOTIFY:
			switch (((LPNMHDR) lparam_)->code){
				case PSN_APPLY: 					
					SaveWindowPos();
					ApplyNow();
					break;
				case PSN_SETACTIVE: nStartPage=1; break;
				case PSN_KILLACTIVE:
					GetWindowText(GetDlgItem(hwnd_,IDC_USERMASK),UserMasks,0x200);
					GetWindowText(GetDlgItem(hwnd_,IDC_EDIT_EXCLUDE),ExcludeMasks,0x200);
					GetWindowText(GetDlgItem(hwnd_,IDC_LIMITCOMPOUNDSIZE),LimitCompoundSize,0x40);
					UserMasks[0x1ff]=0;
					ExcludeMasks[0x1ff]=0;
					LimitCompoundSize[0x3f]=0;
					SetWindowLong(hwnd_,DWL_MSGRESULT,FALSE);
					break;
				case PSN_HELP:
					WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0104);
					HelpCalled=1;
					break;
				case PSN_QUERYCANCEL:{
					SaveWindowPos();
					RetrSettings();
					break;
				}
				default: break;
			}
			break;
		case WM_COMMAND:
			switch(wparam_){
				case IDC_PACKED:
					if(IsDlgButtonChecked(hwnd_, IDC_PACKED)==BST_CHECKED) sPacked=1;
					else sPacked=0;
					break;
				case IDC_ARCHIVES:
					if(IsDlgButtonChecked(hwnd_, IDC_ARCHIVES)==BST_CHECKED) sArchives=1;
					else sArchives=0;
					break;
				case IDC_MAILBASES:
					if(IsDlgButtonChecked(hwnd_, IDC_MAILBASES)==BST_CHECKED) sMailBases=1;
					else sMailBases=0;
					break;
				case IDC_MAILPLAIN:
					if(IsDlgButtonChecked(hwnd_, IDC_MAILPLAIN)==BST_CHECKED) sMailPlain=1;
					else sMailPlain=0;
					break;
				case IDC_SMARTMASK:	sFileMask=0; break;
				case IDC_PROGRAMS: sFileMask=1; break;
				case IDC_ALLFILES: sFileMask=2; break;
				case IDC_USERDEF: sFileMask=3; break;
				case IDC_EXCLUDE: 
					sExclude=(IsDlgButtonChecked(hwnd_, IDC_EXCLUDE)==BST_CHECKED);
					EnableWindow(GetDlgItem(hwnd_,IDC_EDIT_EXCLUDE), sExclude);
					break;
				case IDC_LIMITCOMPOUND:
					sLimitCompound=(IsDlgButtonChecked(hwnd_, IDC_LIMITCOMPOUND)==BST_CHECKED);
					EnableWindow(GetDlgItem(hwnd_,IDC_LIMITCOMPOUNDSIZE), sLimitCompound);
					break;
			}
			EnableWindow(GetDlgItem(hwnd_,IDC_USERMASK),sFileMask==3?TRUE:FALSE);
			EnableWindow(GetDlgItem(hwnd_,IDC_EDIT_EXCLUDE),sExclude);
			PropSheet_Changed(hPrShDlg, hwnd_);
			bApplied=0;
			break;

		case WM_KEYDOWN:
			if((int)wparam_!=VK_F1) break;
		case WM_HELP:
			WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0104); 
			HelpCalled=1;
			return 1;
	}
	return 0;
}

BOOL DeleteAuto;
MY_LRESULT WINAPI SettingsDlgProc2(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	int i;
	switch(msg_)
	{
		case WM_INITDIALOG:			
			if(nStartPage==2) SetWindowPos(hPrShDlg,HWND_TOP,Xpos,Ypos,0,0,SWP_NOSIZE);

			SendMessage(hwnd_,WM_SETFONT,0,1);

			SetWindowText(GetDlgItem(hwnd_,IDC_ALERT), MS(IDS_ALERT));
			SetWindowText(GetDlgItem(hwnd_,IDC_ACTIONS), MS(IDS_ACTIONS));
//			SetWindowText(GetDlgItem(hwnd_,IDC_ASK), MS(IDS_PROMPT_USER));
			SetWindowText(GetDlgItem(hwnd_,IDC_CURE), MS(IDS_DISINF_AUTO));
			SetWindowText(GetDlgItem(hwnd_,IDC_DELETE), MS(IDS_DELETE_AUTO));
			SetWindowText(GetDlgItem(hwnd_,IDC_DENY), MS(IDS_DENY));

			CheckDlgButton(hwnd_,IDC_ALERT,Alert);

			switch(Actions){
			default:
			case 0:i=IDC_DENY; break;
			case 1:i=IDC_CURE; break;
			case 2:i=IDC_DELETE; break;
			}
			if(!(functionality & FN_DISINFECT))
			{
				if(i==IDC_CURE)i=IDC_DENY;
				EnableWindow(GetDlgItem(hwnd_,IDC_CURE), FALSE);
			}
			if(disableMode){
//				EnableWindow(GetDlgItem(hwnd_,IDC_ALERT), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_DENY), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_CURE), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_DELETE), FALSE);
			}
			CheckRadioButton(hwnd_, IDC_DENY, IDC_DELETE, i);  

			
			return 0;

		case  WM_NOTIFY:
			switch (((LPNMHDR) lparam_)->code){
				case PSN_APPLY: 					
					SaveWindowPos();
					ApplyNow();
					break;
				case PSN_QUERYCANCEL:{
					SaveWindowPos();
					RetrSettings();
					break;
				}
				case PSN_SETACTIVE: nStartPage=2; break;
				case PSN_KILLACTIVE:
					if(sActions!=2){
						DeleteAuto=0;
						return 0;
					}
					if((sActions==2)&&(DeleteAuto==0)){ //2==delete
						if(IDNO==MessageBox(hwnd_, (char*)MS(IDS_SHURE_DELETEALL),
								MS( IDS_FULL_NAME ), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 |MB_TOPMOST))
						{
							SetWindowLong(hwnd_,DWL_MSGRESULT,TRUE);
							return TRUE;
						}
						DeleteAuto=1;
					}
					SetWindowLong(hwnd_,DWL_MSGRESULT,FALSE);
					return FALSE;//break;
				case PSN_HELP:
					WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0105); 
					HelpCalled=1;
					break;
				default: break;
			}
			break;

		case WM_COMMAND:
			switch(wparam_){
				case IDC_ALERT:
					if(IsDlgButtonChecked(hwnd_, IDC_ALERT)==BST_CHECKED)
						sAlert=1;
					else sAlert=0;
					break;
				default:
				case IDC_DENY: sActions=0; break;
				case IDC_CURE: sActions=1; break;
				case IDC_DELETE: sActions=2; break;
			}
			PropSheet_Changed(hPrShDlg, hwnd_);
			bApplied=0;
			break;

		case WM_KEYDOWN:
			if((int)wparam_!=VK_F1) break;
		case WM_HELP:
			WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0105); 
			HelpCalled=1;
			return 1;
	}
	return 0;
}

MY_LRESULT WINAPI SettingsDlgProc3(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch(msg_)
	{
		case WM_INITDIALOG:{			
			if(nStartPage==3) SetWindowPos(hPrShDlg,HWND_TOP,Xpos,Ypos,0,0,SWP_NOSIZE);

			SendMessage(hwnd_,WM_SETFONT,0,1);

			SetWindowText(GetDlgItem(hwnd_,IDC_ANALYSER), MS(IDS_CA));
			SetWindowText(GetDlgItem(hwnd_,IDC_WARNINGS), MS(IDS_WARNING));
			SetWindowText(GetDlgItem(hwnd_,IDC_REPORT), MS(IDS_REPORT));
			SetWindowText(GetDlgItem(hwnd_,IDC_LIMIT), MS(IDS_LIMIT));

			SetWindowText(GetDlgItem(hwnd_,IDC_REPORTFILE), RFName);
			SetWindowText(GetDlgItem(hwnd_,IDC_LIMITSIZE), LimitSize);

			CheckDlgButton(hwnd_,IDC_ANALYSER,/*!!(functionality & FN_CA) &&*/ Analyser);
			CheckDlgButton(hwnd_,IDC_WARNINGS,Warnings);

			//EnableWindow(GetDlgItem(hwnd_,IDC_ANALYSER), !!(functionality & FN_OPTIONS));

			if(Report){
				CheckDlgButton(hwnd_,IDC_REPORT,1);
				if(Limit)CheckDlgButton(hwnd_,IDC_LIMIT,1);
			}
			EnableWindow(GetDlgItem(hwnd_,IDC_REPORTFILE), Report);
			EnableWindow(GetDlgItem(hwnd_,IDC_LIMIT), Report);
			EnableWindow(GetDlgItem(hwnd_,IDC_LIMITSIZE), Report);
			EnableWindow(GetDlgItem(hwnd_,IDC_LIMITSIZE), Report && Limit);

			if(disableMode){
				EnableWindow(GetDlgItem(hwnd_,IDC_ANALYSER), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_WARNINGS), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_REPORT), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_REPORTFILE), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_LIMIT), FALSE);
				EnableWindow(GetDlgItem(hwnd_,IDC_LIMITSIZE), FALSE);
			}
			return 0;
		}

		case  WM_NOTIFY:
			switch (((LPNMHDR) lparam_)->code){
				case PSN_APPLY: 					
					SaveWindowPos();
					ApplyNow();
					break;
				case PSN_SETACTIVE: nStartPage=3; break;
				case PSN_KILLACTIVE: 
					GetWindowText(GetDlgItem(hwnd_,IDC_REPORTFILE),RFName,MAX_PATH);
					GetWindowText(GetDlgItem(hwnd_,IDC_LIMITSIZE),LimitSize,0x40);
					LimitSize[0x3f]=0;
					SetWindowLong(hwnd_,DWL_MSGRESULT,FALSE);
					break;
				case PSN_QUERYCANCEL:{
					SaveWindowPos();
					RetrSettings();
					break;
				}
				case PSN_HELP:
					WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0106); 
					HelpCalled=1;
					break;
				default: break;
			}
			break;

		case WM_COMMAND:
			switch(wparam_){
				case IDC_ANALYSER:
					if(IsDlgButtonChecked(hwnd_, IDC_ANALYSER)==BST_CHECKED)
						sAnalyser=1;
					else sAnalyser=0;
					break;
				case IDC_WARNINGS:
					if(IsDlgButtonChecked(hwnd_, IDC_WARNINGS)==BST_CHECKED)
						sWarnings=1;
					else sWarnings=0;
					break;
				case IDC_REPORT:
					sReport=(IsDlgButtonChecked(hwnd_, IDC_REPORT)==BST_CHECKED);
					EnableWindow(GetDlgItem(hwnd_,IDC_REPORTFILE), sReport);
					EnableWindow(GetDlgItem(hwnd_,IDC_LIMIT), sReport);
					EnableWindow(GetDlgItem(hwnd_,IDC_LIMITSIZE), sReport && sLimit);
					break;
				case IDC_LIMIT:
					sLimit=(IsDlgButtonChecked(hwnd_, IDC_LIMIT)==BST_CHECKED);
					EnableWindow(GetDlgItem(hwnd_,IDC_LIMITSIZE), sLimit);
					break;
			}
			PropSheet_Changed(hPrShDlg, hwnd_);
			bApplied=0;
			break;

		case WM_KEYDOWN:
			if((int)wparam_!=VK_F1) break;
		case WM_HELP:
			WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0106); 
			HelpCalled=1;
			return 1;
	}
	return 0;
}

MY_LRESULT WINAPI SettingsDlgProc4(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	DWORD  count;
	switch(msg_)
	{
		case WM_INITDIALOG:{			
			sStat.Objects=(DWORD)-1;
			if(nStartPage==4) SetWindowPos(hPrShDlg,HWND_TOP,Xpos,Ypos,0,0,SWP_NOSIZE);

			SendMessage(hwnd_,WM_SETFONT,0,1);

			SetWindowText(GetDlgItem(hwnd_,IDC_S_OBJECTS), MS(IDS_CHECKED));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_INFECTED), MS(IDS_INFECTED));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_DISINFECTED), MS(IDS_DISINFECTED));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_WARNINGS), MS(IDS_WARNINGS));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_SUSPICIONS), MS(IDS_SUSPICIONS));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_DELETED), MS(IDS_DELETED));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_LASTINFECTED), MS(IDS_LAST_INFOBJECT));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_LAST_OBJ), MS(IDS_LAST_CHOBJECT));
			SetWindowText(GetDlgItem(hwnd_,IDC_S_VIRNAME), MS(IDS_LAST_VIRUSN));
			SetWindowText(GetDlgItem(hwnd_,IDC_LASTFILE), "");
			SetWindowText(GetDlgItem(hwnd_,IDC_LASTINFECTED), "");
			
			ObjectsChecked=0;

			return 0;
		}
		case WM_DESTROY:
			KillTimer(hwnd_,nIDEvent);
			return 0;
	
		case WM_TIMER:
			{
			if(wparam_!=nIDEvent) return 1;
			if(nStartPage!=4) return 1;
			AVPStat Stat;
			if (!DeviceIoControl(hDevice, IOCTL_AVP_GET_STAT, 0, 0, (void*)&Stat, sizeof(AVPStat), &count, 0 ))
				return 0;
			if(memcmp(&sStat,&Stat,sizeof(AVPStat))==0)return 0;
			memcpy(&sStat,&Stat,sizeof(AVPStat));

			if(EnableCheck!=(BOOL)Stat.Enable)
				EnableStateChanged();
		
			//if(ObjectsChecked==sStat.Objects) return 0;
			
			char b[0x200];
			SendMessage(GetDlgItem(hwnd_, IDC_OBJECTS), WM_SETTEXT, 0, (LPARAM)_itoa(Stat.Objects,b,10));
			SendMessage(GetDlgItem(hwnd_, IDC_INFECTED), WM_SETTEXT, 0, (LPARAM)_itoa(Stat.Infected,b,10));
			SendMessage(GetDlgItem(hwnd_, IDC_WARNINGS1), WM_SETTEXT, 0, (LPARAM)_itoa(Stat.Warnings,b,10));
			SendMessage(GetDlgItem(hwnd_, IDC_SUSPICIONS), WM_SETTEXT, 0, (LPARAM)_itoa(Stat.Suspicions,b,10));
			SendMessage(GetDlgItem(hwnd_, IDC_DISINFECTED2), WM_SETTEXT, 0, (LPARAM)_itoa(Stat.Disinfected,b,10));
			SendMessage(GetDlgItem(hwnd_, IDC_DELETED2), WM_SETTEXT, 0, (LPARAM)_itoa(Stat.Deleted,b,10));
			SendMessage(GetDlgItem(hwnd_, IDC_LASTFILE), WM_SETTEXT, 0, (LPARAM)Stat.LastObjectName);
			SendMessage(GetDlgItem(hwnd_, IDC_LASTINFECTED), WM_SETTEXT, 0, (LPARAM)Stat.LastInfectedObjectName);
			SendMessage(GetDlgItem(hwnd_, IDC_VIRNAME), WM_SETTEXT, 0, (LPARAM)Stat.LastVirusName);

			ObjectsChecked=sStat.Objects;
			UpdateWindow(hwnd_);
			return 0;
			}

		case  WM_NOTIFY:
			switch (((LPNMHDR) lparam_)->code){
				case PSN_APPLY: 					
					SaveWindowPos();
					ApplyNow();
					break;
				case PSN_SETACTIVE: 
					nIDEvent=2;
					SetTimer(hwnd_, nIDEvent, 250, NULL);
					nStartPage=4; 
					break;
				case PSN_KILLACTIVE:
					KillTimer(hwnd_,nIDEvent);
					SetWindowLong(hwnd_,DWL_MSGRESULT,FALSE);
					break;
				case PSN_QUERYCANCEL:{
					SaveWindowPos();
					RetrSettings();
					break;
				}
				case PSN_HELP:
					WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0107); 
					HelpCalled=1;
					break;
				default: break;
			}
			break;

		case WM_KEYDOWN:
			if((int)wparam_!=VK_F1) break;
		case WM_HELP:
			WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0107); 
			HelpCalled=1;
			return 1;
	}
	return 0;
}

void ApplyNow()
{
	if(bApplied)return;
	bApplied=TRUE;
	UpdateSettings();
	WriteSettings();
	SetMFlags(MFlags);
	SaveMasks(UserMasks,IOCTL_AVP_ADD_MASK);
	SaveMasks(ExcludeMasks,IOCTL_AVP_ADD_EXCLUDE_MASK);

	EnableStateChanged();

	if(hMainDlg)SendMessage(hMainDlg, WM_COMMAND, MI_PROPERTIES, 1);

	LimitSizeInBytes=atoi(LimitSize)*1024;

	DWORD count;
	DWORD CompoundSize=LimitCompound?atoi(LimitCompoundSize)*1024:0;

	if(!DeviceIoControl(hDevice,IOCTL_AVP_SET_MFLAGS,&MFlags,4,NULL,0,&count,0))
		goto err;

	if(!DeviceIoControl(hDevice,IOCTL_AVP_LIMITCOMPOUNDSIZE,&CompoundSize,4,NULL,0,&count,0)){
err:	DriverFailedAndClose();
		return;
	}
	SendStatisticsByEvent(0);

}


typedef struct _CMDSHOW{
	WORD p1;		//must always be set to two
	WORD p2;		//how the application window is to be shown
}CmdShow;

typedef struct tagLOADPARMS32 {  
    LPSTR lpEnvAddress;  // address of environment strings 
    LPSTR lpCmdLine;     // address of command line 
    LPSTR lpCmdShow;     // how to show new program 
    DWORD dwReserved;    // must be zero 
} LOADPARMS32; 

//int Wait=0;
int CALLBACK Ctrl_PSProc(HWND hwnd_, UINT msg_, LPARAM lparam_)
{
	switch(msg_)
	{
	case PSCB_INITIALIZED:
		hPrShDlg=hwnd_;
		break;
	case PSCB_PRECREATE:
		break;
	default:
		break;
	}

	return 0;
}

BOOL ReportWriteLine(char* s_)
{
	BOOL ret=0;

	if(Report && RFName){
		char s[0x400];
		AvpmRegisterThread(1 ,OsVersion.dwPlatformId);

		*s=0;
		if(strnicmp(RFName+1,":\\",2) && strnicmp(RFName,"\\\\",2))
		{
			strcpy(s,StartPath);
			if(s[strlen(s)-1]!='\\')
				strcat(s,"\\");	
		}
		strcat(s,RFName);


		HANDLE h=CreateFile(s,
			GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,
			OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if(h!=INVALID_HANDLE_VALUE){
			SYSTEMTIME st;
			GetLocalTime(&st);
			DWORD bytes;
			DWORD p=GetFileSize(h,NULL);
			if(Limit && (p>LimitSizeInBytes))
			{
				p=0;
				SetFilePointer(h,p,NULL,FILE_BEGIN);
				SetEndOfFile(h);
				sprintf(s,"%02d.%02d.%04d %02d:%02d:%02d\x9%s",
					st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,MS(IDS_REP_TRUNCATED));
				WriteFile(h,s,strlen(s),&bytes,NULL);
				WriteFile(h,"\xd\xa",2,&bytes,NULL);
			}
			else
				SetFilePointer(h,p,NULL,FILE_BEGIN);

			sprintf(s,"%02d.%02d.%04d %02d:%02d:%02d\x9%s",
				st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,s_);
			WriteFile(h,s,strlen(s),&bytes,NULL);
			WriteFile(h,"\xd\xa",2,&bytes,NULL);
			CloseHandle(h);
			ret=1;
		}
		AvpmRegisterThread(0 ,OsVersion.dwPlatformId);
	}
	return ret;
}

BOOL ReportWriteObject(AVPScanObject* so)
{ 
	char s[0x400];
	char *name=so->VirusName;
	char *what="";
	DWORD IpcMess=0;

	if(so->RFlags & RF_DELETED){
		what=MS(IDS_REP_DELETED);
		IpcMess=AVP_ALERT_OBJECT_DELETE;
	}
	else if(so->RFlags & (RF_IO_ERROR|RF_NOT_OPEN)){
		what=MS(IDS_REP_IO_ERROR);
		name=0;
		IpcMess=AVP_ALERT_ERROR_EXT;
	}
	else if(so->RFlags & RF_CURE_FAIL){
		what=MS(IDS_REP_DISINFECT_FAILED);
		IpcMess=AVP_ALERT_ERROR_EXT;
	}

	else if(so->RFlags & RF_CURE){
		what=MS(IDS_REP_DISINFECTED);
		IpcMess=AVP_ALERT_OBJECT_CURE;
	}

	else if(so->RFlags & RF_DETECT){
		what=MS(IDS_REP_DETECTED);
		IpcMess=AVP_ALERT_OBJECT_DETECT;
	}

	else if(so->RFlags & RF_WARNING){
		what=MS(IDS_REP_WARNING);
		name=so->WarningName;
		IpcMess=AVP_ALERT_OBJECT_WARNING;
	}
	else if(so->RFlags & RF_SUSPIC){
		what=MS(IDS_REP_SUSPICION);
		name=so->SuspicionName;
		IpcMess=AVP_ALERT_OBJECT_SUSP;
	}

	sprintf(s,"%s  %s  ",so->Name,what);
	DWORD count;
	if(name)
		if(!DeviceIoControl(hDevice, IOCTL_AVP_MAKE_FULL_NAME, &name, 4, s+strlen(s), 0x200, &count, 0 ))
			return FALSE;
	
	if(IpcMess)
		if(CCClientSendAlert)
			CCClientSendAlert(IpcMess,s);
	SendStatisticsByEvent(0);

	return ReportWriteLine(s);
}


HANDLE h_loadBase=NULL;

DWORD __stdcall loadBase(void* h){
	HWND hwnd_=(HWND)h;
	DWORD count;
	int ret=1;
		DeviceIoControl(hDevice,IOCTL_AVP_RELOAD_BASE,FullSETFileName,strlen(FullSETFileName)+1,0,0,&count,0);
		if(DeviceIoControl(hDevice,IOCTL_AVP_GET_BASE_INFO,NULL,0,&BaseInfo,sizeof(AVPBaseInfo),&count,0)){

			sprintf(LastUpdate,MS(IDS_LAST_UPDATE),BaseInfo.LastUpdateDay,BaseInfo.LastUpdateMonth,
			BaseInfo.LastUpdateYear,BaseInfo.NumberOfRecords);

			if(hAboutPage)
				SetWindowText(GetDlgItem(hAboutPage,IDC_UPDATE), LastUpdate);
		}
		else DriverFailedAndClose();
//ret:
		CloseHandle(h_loadBase);
		h_loadBase=NULL;
		return ret;
}


// string buffer should be 0x200 and contain backslash
BOOL CountDirSpace(char* string, DWORDLONG* data, BOOL* kill, CRITICAL_SECTION* CritSection, BOOL touch)
{
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
				strcat(namePtr,"\\");
				CountDirSpace(string, data, kill, CritSection, touch);
			}
		}
		else if (FindData.nFileSizeLow != 0xFFFFFFFF)
		{
			if(CritSection)EnterCriticalSection(CritSection);
			*data += FindData.nFileSizeLow;
			if(CritSection)LeaveCriticalSection(CritSection);


			if(touch)
			{
				strcpy(namePtr,FindData.cFileName);
				if(!strlen(FindData.cFileName)||(DWORD)-1==GetFileAttributes(string))
				{
					strcpy(namePtr,FindData.cAlternateFileName);
					if(!strlen(FindData.cAlternateFileName)||(DWORD)-1==GetFileAttributes(string)) 
						continue;
				}
				HANDLE h=CreateFile(string,
					GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL,OPEN_EXISTING,0,NULL);
				if(h!=INVALID_HANDLE_VALUE)
				{
					char buf[0x10];
					DWORD count;
					ReadFile(h,buf,0x10,&count,NULL);
					CloseHandle(h);
				}
			}
		}
	}while( FindNextFile(hFind, &FindData));

	FindClose(hFind);

	return !*kill;
}



DWORD __stdcall Scan(void*)
{
	char s[0x200];
	KillScanThread=0;
	TotalSpace=0;
	ProcessedSpace=0;

	//Unregister to scan!!!
	AvpmRegisterThread(0 ,OsVersion.dwPlatformId);

	strncpy(s,"C:\\",0x200);
	
	for(int i=0; i<32 ; i++){
		if(KillScanThread)break;
		sprintf(s,"%c:\\",'A'+i);
		DWORD type=GetDriveType(s);
		switch(type)
		{
		case DRIVE_FIXED:
		case DRIVE_RAMDISK:
			CountDirSpace(s,&TotalSpace,&KillScanThread,&TotalSpaceCritSec,0);
		default:	break; //All other types check later.
		}
	}

	for(i=0; i<32 ; i++){
		if(KillScanThread)break;
		sprintf(s,"%c:\\",'A'+i);
		DWORD type=GetDriveType(s);
		switch(type)
		{
		case DRIVE_FIXED:
		case DRIVE_RAMDISK:
			CountDirSpace(s,&ProcessedSpace,&KillScanThread,&ProcessedSpaceCritSec,1);
		default:	break; //All other types check later.
		}
	}

	scanThread=NULL;
	scanThreadPaused=0;
	EnableCheck=savedEnableCheck;
	bApplied=0;
	ApplyNow();

	PostMessage(hMainDlg, WM_COMMAND, IDC_SETGENERALBUTTONS, 0);

	sPercent=-1;
	if(hGeneralPage) PostMessage(GetDlgItem(hGeneralPage,IDC_PROGRESS), PBM_SETPOS, 0,0);

	return KillScanThread;
}
/*
BOOL CheckBootA()
{
	AVPScanObject so;
	DWORD count;
	memset(&so,0,sizeof(AVPScanObject));
	sprintf(so.Name,"%s A:",MS(IDS_BOOTSECTOR));
	so.Drive=0;
	so.Disk=0;
	so.SType=OT_BOOT; 
	so.MFlags=MF_CA|MF_WARNINGS;
	so.MFlags=(so.MFlags&~MF_D_)|MF_D_DENYACCESS;
	DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,&so,sizeof(AVPScanObject),&so,sizeof(AVPScanObject),&count,0);
	if(so.RFlags & RF_DETECT)
		return AlertAVPScanObject(&so,0);
//		TRUE;
	return FALSE;
}
*/
void StopScan()
{
	if(!scanThread) return;
	KillScanThread=1;
	if(scanThreadPaused)
	{
		savedEnableCheck=EnableCheck;
		EnableCheck=1;
		bApplied=0;
		ApplyNow();
		scanThreadPaused=0;
		ResumeThread(scanThread);
	}
	while(scanThread)Sleep(200);
}

int __stdcall MainDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	DWORD  count;
    DWORD ret;
	static int ntry=0;

	if(msg_==WM_VXDAVP){
		switch(wparam_)
		{
		case WP_PROPERTIES:
			PostMessage(hMainDlg,WM_COMMAND,MI_PROPERTIES,0);
			return 1;
		case VXD_LOADED:
			break;
		case VXD_AVP_LOAD_ERR:
		case VXD_GK_LOAD_ERR:
			DriverFailedAndClose();
			break;
		}//end of switch
		return 1;
	}
	if(msg_==WM_AVPUPD)
	{
		PostMessage(hwnd_, MYWM_LOADBASE,0,0);
		return 1;
	}
	if(msg_==WM_TASKBARCREATED)
	{
		if(TrayMessage(hwnd_, NIM_ADD, EnableCheck?hicon_:hdisicon_, EnableCheck?MS(IDS_TIP_ENABLE):MS(IDS_TIP_DISABLE)))
			NeedAddIcon=0;
		return 1;
	}
	
	switch(msg_)
	{

    case WM_INITDIALOG:
		SendMessage(hwnd_, WM_SETICON, ICON_BIG, (LPARAM)hdisicon_);
		SendMessage(hwnd_, WM_SETICON, ICON_SMALL, (LPARAM)hdisicon_);
//		TrayMessage(hwnd_, NIM_ADD, hdisicon_, NULL);
		NeedAddIcon=1;
		nIDEvent3=3;
		SetTimer(hwnd_, nIDEvent3, 1000, NULL);
		return 1;

	case WM_USERCHANGED:
		NeedAddIcon=1;
		return 0;

/*
    case WM_QUERYENDSESSION:
		return !CheckBootA();
*/
    case WM_DESTROY:
		TrayMessage(hwnd_, NIM_DELETE, NULL, NULL);
		KillTimer(hwnd_,nIDEvent3);
		StopScan();
		Exitting=1;
		return 1;

	case WM_TIMER:
		{
			static ipcrep;

			if(wparam_!=nIDEvent3) return 1;
			if(propByTimer && (++ipcrep%=propInterval)==0)
				MakeSendStatistics();
			if(NeedAddIcon)
			{
				TrayMessage(hwnd_, NIM_DELETE, NULL, NULL);
				if(TrayMessage(hwnd_, NIM_ADD, EnableCheck?hicon_:hdisicon_, EnableCheck?MS(IDS_TIP_ENABLE):MS(IDS_TIP_DISABLE)))
					NeedAddIcon=0;
			}

			return 1;
		}

	case  MYWM_LOADBASE:
		if(h_loadBase==NULL)
		{
			DWORD ThreadId;
			ntry++;
			h_loadBase=CreateThread( NULL, 8192,
			(DWORD  (__stdcall *) (void *) ) loadBase,
			(DWORD*) hwnd_, 0, &ThreadId);
		}
	
/*

		DeviceIoControl(hDevice,IOCTL_AVP_RELOAD_BASE,FullSETFileName,strlen(FullSETFileName)+1,0,0,&count,0);
		if(DeviceIoControl(hDevice,IOCTL_AVP_GET_BASE_INFO,NULL,0,&BaseInfo,sizeof(AVPBaseInfo),&count,0)){

			sprintf(LastUpdate,MS(IDS_LAST_UPDATE),BaseInfo.LastUpdateDay,BaseInfo.LastUpdateMonth,
			BaseInfo.LastUpdateYear,BaseInfo.NumberOfRecords);

			if(hGeneralPage)
				SetWindowText(GetDlgItem(hGeneralPage,IDC_UPDATE), LastUpdate);
		}
		else{
			DriverFailedAndClose()
		}
*/
		return 1;

	case MYWM_AFTERINIT:
		DeviceIoControl(hDevice,IOCTL_AVP_GET_BASE_INFO,NULL,0,&BaseInfo,sizeof(AVPBaseInfo),&count,0);
		if(	h_loadBase || BaseInfo.Status & BI_LOADING){
			Sleep(20);
			PostMessage(hMainDlg,MYWM_AFTERINIT,0,0);
			return 1;
		}

		if(stricmp(FullSETFileName,BaseInfo.SetName)
			|| BaseInfo.Status & BI_ERROR)
		{
			if(!ntry){
				PostMessage(hwnd_,MYWM_LOADBASE,0,0);
				Sleep(20);
				PostMessage(hMainDlg,MYWM_AFTERINIT,0,0);
				return 1;
			}

		}
		
		MSG msg;
		if(PeekMessage(&msg,hwnd_,WM_CLOSE,WM_CLOSE,PM_NOREMOVE))	return 0;

		sprintf(LastUpdate,MS(IDS_LAST_UPDATE),BaseInfo.LastUpdateDay,BaseInfo.LastUpdateMonth,
		BaseInfo.LastUpdateYear,BaseInfo.NumberOfRecords);

		time_t curt;
		time(&curt);
		if(!disableMode)
		if(gmtime(&curt)){
			struct tm tbd=*gmtime(&curt);
			tbd.tm_year=BaseInfo.LastUpdateYear - 1900;
			tbd.tm_mon=BaseInfo.LastUpdateMonth - 1;
			tbd.tm_mday=BaseInfo.LastUpdateDay;
			curt-=mktime(&tbd);
			
			if(curt/(24*60*60) >= (time_t)m_BaseDelay)
				if(IDOK==MessageBox(hMainDlg,MS( IDS_SET_OUTOFDATE ),MS( IDS_FULL_NAME ),MB_OKCANCEL|MB_ICONINFORMATION|MB_TOPMOST))
					PostMessage(hMainDlg, WM_COMMAND, IDC_UPDATENOW,0);
		}
		if(Report){
			char s[0x200];
			sprintf(s,"%s %s  %s",MS(IDS_FULL_NAME),MS(IDS_REP_LOADED),LastUpdate);
			ReportWriteLine(s);
		}
		PostMessage(hwnd_,MYWM_SELFSCAN,0,0);
		EnableStateChanged();

		return 1;

	case MYWM_SELFSCAN:

		AVPScanObject so;
		char buf[0x100];

		if(startupMem){
			WritePrivateProfileString(("Startup"),("Memory"),_itoa(0,buf,10),FullINIFileName);
			memset(&so,0,sizeof(AVPScanObject));
			strcpy(so.Name,MS(IDS_MEMORY));
			so.SType=OT_MEMORY; 
			so.MFlags=MFlags;
			DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,&so,sizeof(AVPScanObject),&so,sizeof(AVPScanObject),&count,0);
			WritePrivateProfileString(("Startup"),("Memory"),_itoa(startupMem,buf,10),FullINIFileName);
		}

		if(startupMbr){
			WritePrivateProfileString(("Startup"),("MBR"),_itoa(0,buf,10),FullINIFileName);
			for(BYTE d=0;d<16;d++){
				memset(&so,0,sizeof(AVPScanObject));
				sprintf(so.Name,"%s HDD#%X",MS(IDS_MBR),d);
				so.Drive=2;
				so.Disk=0x80+d;
				so.SType=OT_MBR; 
				so.MFlags=MF_CA|MF_WARNINGS|MF_D_DENYACCESS;
				DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,&so,sizeof(AVPScanObject),&so,sizeof(AVPScanObject),&count,0);
				if(so.RFlags & RF_NOT_OPEN)break;
			}
			WritePrivateProfileString(("Startup"),("MBR"),_itoa(startupMbr,buf,10),FullINIFileName);
		}

		if(startupBoot){
			WritePrivateProfileString(("Startup"),("Boot"),_itoa(0,buf,10),FullINIFileName);
			memset(&so,0,sizeof(AVPScanObject));
			sprintf(so.Name,"%s C:",MS(IDS_BOOTSECTOR));
			so.Drive=2;
			so.Disk=0x80;
			so.SType=OT_BOOT; 
			so.MFlags=MF_CA|MF_WARNINGS;
			so.MFlags=(so.MFlags&~MF_D_)|MF_D_DENYACCESS;
			DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,&so,sizeof(AVPScanObject),&so,sizeof(AVPScanObject),&count,0);
			WritePrivateProfileString(("Startup"),("Boot"),_itoa(startupBoot,buf,10),FullINIFileName);
		}
  
		if(startupSelf){
			WritePrivateProfileString(("Startup"),("Self"),_itoa(startupSelf,buf,10),FullINIFileName);
			memset(&so,0,sizeof(AVPScanObject));
			GetModuleFileName(ghInstance,so.Name,0x200);
			so.SType=OT_FILE; 
			so.RFlags=RF_REPORT_DONE;
			so.MFlags=MF_CA|MF_WARNINGS|MF_PACKED;
			so.MFlags=(so.MFlags&~MF_D_)|MF_D_CUREALL;
			DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,&so,sizeof(AVPScanObject),&so,sizeof(AVPScanObject),&count,0);
			WritePrivateProfileString(("Startup"),("Self"),_itoa(startupSelf,buf,10),FullINIFileName);
			if(so.RFlags & RF_DETECT && so.RFlags & RF_NOT_OPEN)
			{
				char* lp;
				char buf[0x200];
				char vir[0x100]="";
				DWORD attr;
				DWORD enable=0;
				if(!DeviceIoControl(hDevice,IOCTL_AVP_ENABLE,&enable,4,NULL,0,&count,0))goto dfail;
				strcpy(buf,so.Name);
				lp=strrchr(so.Name,'.');
				strcpy(lp,"_.EXE");
				if(!CopyFile(buf,so.Name,FALSE))goto dfail;
				if((DWORD)-1==(attr=GetFileAttributes(so.Name)))goto dfail;
				attr&=~FILE_ATTRIBUTE_READONLY;
				if(!SetFileAttributes(so.Name,attr))goto dfail;
				so.RFlags=RF_REPORT_DONE;
				enable=1;
				if(!DeviceIoControl(hDevice,IOCTL_AVP_ENABLE,&enable,4,NULL,0,&count,0))goto dfail;
				if(!DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,&so,sizeof(AVPScanObject),&so,sizeof(AVPScanObject),&count,0))goto dfail;
				if(!DeviceIoControl(hDevice,IOCTL_AVP_MAKE_FULL_NAME,&so.VirusName,4,vir,0x100,&count,0))goto dfail;
				if(!(so.RFlags & RF_CURE))goto dfail;

				sprintf(buf,MS(IDS_SELF_DISINFECTED),vir);
				MessageBox(hwnd_,buf,MS( IDS_FULL_NAME ),MB_ICONEXCLAMATION|MB_TOPMOST);
				
				strcpy(execName,so.Name);
				PostMessage(hwnd_, WM_CLOSE, 0, 0);

				goto next4;
	dfail:
				sprintf(buf,MS(IDS_SELF_DISINFECTION_FAILED),vir);
				MessageBox(hwnd_,buf,MS( IDS_FULL_NAME ),MB_ICONEXCLAMATION|MB_TOPMOST);
			}
		}
next4:
#ifdef SPLASH_
	if(hSplash)
	{
		SetCursor(hCursor);
		DestroyWindow(hSplash);
		hSplash=NULL;
	}
#endif// SPLASH_

		return 1;
	
	case MYWM_NOTIFYICON:
		switch (lparam_)
		{
		case WM_RBUTTONUP:{
			POINT p;
			GetCursorPos(&p);
			PostMessage(hwnd_,MYWM_PROCICON,0,p.x<<16|p.y);
			return 1;
		}
//		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
			if(!ResLimited){
				if(hPrShDlg) SetForegroundWindow(hPrShDlg);
				else PostMessage(hwnd_, WM_COMMAND, MI_PROPERTIES, 0);
			}
			return 1;
		}
		return 0;
	
	case MYWM_PROCICON:{
			HMENU hmenu_=CreatePopupMenu();
			if(!CreateMenuBox(hwnd_, hmenu_)) return 0;
			SetForegroundWindow(hwnd_);
			TrackPopupMenu(hmenu_,TPM_RIGHTALIGN|TPM_LEFTBUTTON,lparam_>>16,lparam_&0xFFFF,0,hwnd_,NULL);
			SendMessage(hwnd_,WM_NULL,0,0);
			DestroyMenu(hmenu_);
			return 0;
	}

	case WM_SYSCOMMAND:
		if(wparam_==SC_CLOSE){
			if(noExitMode){
				return 1;
			}
			if(AVPServiceLoadedByMonitor)
				ret=MessageBox(hwnd_,MS(IDS_SHURE_EXIT),MS( IDS_FULL_NAME ),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1|MB_TOPMOST);
			else
				ret=IDYES;

			if(ret==IDYES){
				if(hPrShDlg)
					SendMessage(hPrShDlg,WM_CLOSE,0,0);
				PostMessage(hwnd_,WM_CLOSE,0,0);
			}
			return 1;
		}
		return 0;

	case WM_CLOSE:
/*		if(CheckBootA())
		{
				PostMessage(hwnd_,WM_CLOSE,0,0);
				return 0;
		}
*/
		DestroyWindow(hwnd_);
		PostQuitMessage(1);
		return 1;
	
	case WM_COMMAND:
		switch (wparam_)
        {
		case IDC_UPDATENOW:
			if(!disableMode)
			{
				if(!(functionality & FN_UPDATES))
				{
					MessageBox(NULL, MS(IDS_UPDATE_EXPIRED), MS( IDS_FULL_NAME ),	MB_ICONINFORMATION|MB_OK);
					break;
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
			break;

        case MI_EXIT:
			PostMessage(hMainDlg, WM_SYSCOMMAND, SC_CLOSE, 0);
			break;

		case MI_PROPERTIES:
			if(!ResLimited)
			{
			if(hPrShDlg) SetForegroundWindow(hPrShDlg);
			if(lparam_!=0) return 0;	
			if(bSettings) return 0;

			MFlags=0;
			if(!DeviceIoControl(hDevice, IOCTL_AVP_GET_MFLAGS,NULL,0,&MFlags,4,&count,0))
				return DriverFailedAndClose();

			GetMFlags(MFlags);
								
			bSettings=1;

			//////////////////////////
			PROPSHEETPAGE pp;
			pp.dwSize=sizeof(PROPSHEETPAGE); 
			pp.dwFlags=PSP_USETITLE | PSP_HASHELP; 
			pp.hInstance=ghInstance; 
			pp.pszTemplate=MAKEINTRESOURCE(IDD_GENERAL); 
			pp.pszTitle=MS(IDS_GENERAL);
			pp.pfnDlgProc=SettingsDlgProc; 
			pp.lParam=0; 
			pp.pfnCallback=NULL; 
			
			PROPSHEETPAGE pp1;
			pp1.dwSize=sizeof(PROPSHEETPAGE); 
			pp1.dwFlags=PSP_USETITLE | PSP_HASHELP; 
			pp1.hInstance=ghInstance; 
			pp1.pszTemplate=MAKEINTRESOURCE(IDD_OBJECTS); 
			pp1.pszTitle=MS(IDS_OBJECTS); 
			pp1.pfnDlgProc=SettingsDlgProc1; 
			pp1.lParam=0; 
			pp1.pfnCallback=NULL; 
						
			PROPSHEETPAGE pp2;
			pp2.dwSize=sizeof(PROPSHEETPAGE); 
			pp2.dwFlags=PSP_USETITLE | PSP_HASHELP; 
			pp2.hInstance=ghInstance; 
			pp2.pszTemplate=MAKEINTRESOURCE(IDD_ACTIONS); 
			pp2.pszTitle=MS(IDS_ACTIONS1); 
			pp2.pfnDlgProc=SettingsDlgProc2; 
			pp2.lParam=0; 
			pp2.pfnCallback=NULL; 

			PROPSHEETPAGE pp3;
			pp3.dwSize=sizeof(PROPSHEETPAGE); 
			pp3.dwFlags=PSP_USETITLE | PSP_HASHELP; 
			pp3.hInstance=ghInstance; 
			pp3.pszTemplate=MAKEINTRESOURCE(IDD_OPTIONS); 
			pp3.pszTitle=MS(IDS_OPTIONS);
			pp3.pfnDlgProc=SettingsDlgProc3; 
			pp3.lParam=0; 
			pp3.pfnCallback=NULL; 

			PROPSHEETPAGE pp4;
			pp4.dwSize=sizeof(PROPSHEETPAGE); 
			pp4.dwFlags=PSP_USETITLE | PSP_HASHELP; 
			pp4.hInstance=ghInstance; 
			pp4.pszTemplate=MAKEINTRESOURCE(IDD_STATISTICS); 
			pp4.pszTitle=MS(IDS_STATISTICS);
			pp4.pfnDlgProc=SettingsDlgProc4; 
			pp4.lParam=0; 
			pp4.pfnCallback=NULL; 

			PROPSHEETPAGE pp5;
			pp5.dwSize=sizeof(PROPSHEETPAGE); 
			pp5.dwFlags=PSP_USETITLE | PSP_HASHELP; 
			pp5.hInstance=ghInstance; 
			pp5.pszTemplate=MAKEINTRESOURCE(IDD_ABOUT); 
			pp5.pszTitle=MS(IDS_ABOUT); 
			pp5.pfnDlgProc=SettingsDlgProc5; 
			pp5.lParam=0; 
			pp5.pfnCallback=NULL; 

			PROPSHEETPAGE ar[6];

			PROPSHEETHEADER psh;
			psh.dwSize=sizeof(PROPSHEETHEADER); 
			psh.dwFlags=PSH_USECALLBACK|PSH_USEHICON|PSH_PROPSHEETPAGE|PSH_HASHELP; //|PSH_PROPTITLE
			if(disableMode)psh.dwFlags|=PSH_NOAPPLYNOW;
			psh.hwndParent=hwnd_; 
			psh.hInstance=ghInstance;
			if(EnableCheck) psh.hIcon=hicon_;
			else psh.hIcon=hdisicon_;
			psh.pszCaption=MS( IDS_FULL_NAME ); 
			psh.ppsp=(LPCPROPSHEETPAGE) &ar;
			psh.pfnCallback=Ctrl_PSProc; 
			psh.nStartPage=nStartPage; 

			psh.nPages=6;
			ar[0]=pp;
			ar[1]=pp1;
			ar[2]=pp2;
			ar[3]=pp3;
			ar[4]=pp4;
			ar[5]=pp5;

			if(ResLimited)
			{
				psh.nPages=3;
				ar[1]=pp4;
				ar[2]=pp5;
				if(nStartPage<4)	psh.nStartPage=0; 
				else if(nStartPage<5)	psh.nStartPage=1; 
				else psh.nStartPage=2; 
			}

			ret=PropertySheet(&psh);

			SetActiveWindow(NULL);

			hPrShDlg=0;
			if(HelpCalled){
				WinHelp(hwnd_,FullHELPFileName,HELP_QUIT,0);
				HelpCalled=0;
			}

			 
			//////////////////////////
			bSettings=0;
			if(ret==0) return 0;
			return 0;
		}
		return 0;

		case MI_DISABLE:
			sEnableCheck=EnableCheck=0;
			EnableStateChanged();

			SendStatisticsByEvent(0);
			return 0;

		case MI_ENABLE:
			sEnableCheck=EnableCheck=1;
			EnableStateChanged();

			SendStatisticsByEvent(0);
			return 0;


		case IDC_SETGENERALBUTTONS:
			SetGeneralButtons();
			return 0;

		case IDC_SCAN:
			if(scanThread)
				StopScan();
			else
			{
				savedEnableCheck=EnableCheck;
				EnableCheck=1;
				bApplied=0;
				ApplyNow();
				scanThreadPaused=0;

				DWORD ThreadId;
				scanThread=(HANDLE)CreateThread( NULL, 8192,
				(DWORD  (__stdcall *) (void *) ) Scan,
				NULL, 0, (DWORD*) &ThreadId);
				if(scanThread) SetThreadPriority(scanThread,THREAD_PRIORITY_IDLE);
			}

			SetGeneralButtons();
			return 0;
		
		case IDC_PAUSE:
			if(scanThread)
			{
				if(scanThreadPaused)
				{
					savedEnableCheck=EnableCheck;
					EnableCheck=1;
					bApplied=0;
					ApplyNow();
					ResumeThread(scanThread);
					scanThreadPaused=0;
				}
				else
				{
					SuspendThread(scanThread);
					scanThreadPaused=1;
					EnableCheck=savedEnableCheck;
					bApplied=0;
					ApplyNow();
				}
			}

			SetGeneralButtons();
			return 0;

		default:
			return 0;
        }//end of case WM_COMMAND: switch (wparam_){
		return 0;
		
	case WM_KEYDOWN:
		if((int)wparam_!=VK_F1) break;
	case WM_HELP:
		WinHelp(hwnd_,FullHELPFileName,HELP_CONTEXT,0x0104); 
		HelpCalled=1;
		return 1;

	default: 
		return 0;

	}//switch(msg_)
    return 0;
}




BOOL NTControlService(char* name_, int func_)
{
    SC_HANDLE schSCManager;
    SC_HANDLE  schService;
    SERVICE_STATUS ss;
    BOOL ret=FALSE;
	BOOL loaded=FALSE;

    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

    if(!schSCManager)      return FALSE;

//    schService=OpenService(schSCManager,name_,SERVICE_QUERY_STATUS);
    schService=OpenService(schSCManager,name_,SERVICE_ALL_ACCESS);
    if(!schService) goto retSCM;
    
	if (!QueryServiceStatus(schService, &ss))
		goto retS;

	if (ss.dwCurrentState == SERVICE_START_PENDING 
		|| ss.dwCurrentState == SERVICE_RUNNING 
		|| ss.dwCurrentState == SERVICE_CONTINUE_PENDING)
		loaded = TRUE;

	switch(func_)
	{
	case 1: //start service
		if(loaded) ret=TRUE;
		else	ret= StartService(schService,0,NULL);
		break;
	case 0: //stop service
		if(!loaded)ret=TRUE;
		else if (ControlService(schService,SERVICE_CONTROL_STOP,&ss))
		{
			int i = 0;
			do{
				Sleep(500);
				QueryServiceStatus(schService, &ss);
				i++;
			} while (ss.dwCurrentState == SERVICE_STOP_PENDING && i < 30); // Default go-out time for a service is 30 secs
		}
		break;
	default: //return service status
		ret=loaded;
		break;
    }

retS:
	CloseServiceHandle(schService);
retSCM:
	CloseServiceHandle(schSCManager);
    return ret;
}



#define RSP_SIMPLE_SERVICE        0x00000001
#define RSP_UNREGISTER_SERVICE    0x00000000

void ServiceRegister (void)
  {
    HINSTANCE hInstKernel;
    DWORD (__stdcall *pRegisterServiceProcess) (DWORD, DWORD);

    hInstKernel = LoadLibrary ("KERNEL32.DLL");

    if (hInstKernel)
      {
        pRegisterServiceProcess = (DWORD (__stdcall *) (DWORD, DWORD))
           GetProcAddress (hInstKernel, "RegisterServiceProcess");

        if (pRegisterServiceProcess)
          pRegisterServiceProcess (NULL, RSP_SIMPLE_SERVICE);

        FreeLibrary (hInstKernel);
      }
  }


int PASCAL WinMain(HINSTANCE hinst_, HINSTANCE hprevinst_, LPSTR cmdline_, int show_)
{
	DWORD count;
//	DWORD err;
	DWORD ret=0;
	int i;
	char* lp;
	
	OsVersion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVersion);
	GetLocalTime(&stStartTime);

	AvpmRegisterThread(1 ,OsVersion.dwPlatformId);

#ifdef _MBCS
	setlocale(LC_ALL, "");
	_setmbcp(_MB_CP_LOCALE);
#endif
	InitializeCriticalSection(&TotalSpaceCritSec);
	InitializeCriticalSection(&ProcessedSpaceCritSec);
	SpaceCritSec=1;

	size_t cmdlen;
	cmdlen=strlen(cmdline_);
	lpCmdLine=new char[cmdlen+4];
	strcpy(lpCmdLine,cmdline_);
	strcat(lpCmdLine," ");
	strlwr(lpCmdLine);

	GetModuleFileName(hinst_,execName,MAX_PATH);
	lp=strrchr(execName,'.');
	if(*(lp-1)=='_'){
		char src[MAX_PATH];
		strcpy(src,execName);
		strcpy(lp-1,".EXE");
		int i=100;
		while(!CopyFile(src,execName,FALSE))
		{
			if(!i--){
				char s[0x200];
				sprintf(s,"%s is locked. Can't overwrite.", execName);
				MessageBox(NULL,s,src,MB_ICONERROR|MB_OK);
				*execName=0;
				break;
			}
			Sleep(100);
		}
		goto exec_ret;
	}
	else{
		strcpy(lp,"_.EXE");
		DeleteFile(execName);
		*execName=0;
	}


	if(strstr(lpCmdLine,"/q ")){
		noExitMode=1;
		disableMode=1;
	}

	HANDLE MonitorMutex;
	MonitorMutex=CreateMutex(NULL,TRUE,"AVP Monitor");
	DWORD err;
	err=GetLastError();
	if(ERROR_ALREADY_EXISTS==err || ERROR_ACCESS_DENIED==err){
		if(show_!=SW_SHOWMINNOACTIVE)
			PostMessage(HWND_BROADCAST,WM_VXDAVP,WP_PROPERTIES,0);
		goto ret;
	}

	
	hicon_=LoadIcon(hinst_, MAKEINTRESOURCE(IDR_MAINFRAME));
	hdisicon_=LoadIcon(hinst_, MAKEINTRESOURCE(IDR_MAINICODIS));
    
	ghInstance=hinst_;

		RFName=new char[MAX_PATH];
		LimitSize=new char[0x40];
		LimitCompoundSize=new char[0x40];
		UserMasks=new char[0x200];
		ExcludeMasks=new char[0x200];
		ExcludeMasks[0]=0;
		UserMasks[0]=0;
		RFName[0]=0;
		

		StartPath=new char[MAX_PATH];
		GetModuleFileName(ghInstance,StartPath,MAX_PATH);
		lp=strrchr(StartPath,'\\');
		*(lp+1)=0;

		IniFile=new char[strlen(StartPath)+14];
		strcpy(IniFile,StartPath);
		strcat(IniFile,"AVP.INI");

		char buf[0x200];
		char* ptr;
		
		GetPrivateProfileString("AVPM","DefaultIniFile","",buf,0x200,IniFile);
		if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
			WritePrivateProfileString("AVPM","DefaultIniFile",buf,IniFile);
		FullINIFileName=new char[strlen(StartPath)+14];
		if(*buf!=0 || !GetFullPathName(buf,0x500,FullINIFileName,&ptr))
			strcpy(FullINIFileName,"AVPM.INI");

		FullHELPFileName=new char[strlen(StartPath)+14];
		strcpy(FullHELPFileName,StartPath);
		strcat(FullHELPFileName,"Avpm.hlp");

		FullSETFileName=new char[MAX_PATH];
		GetPrivateProfileString("Configuration","BasePath","",FullSETFileName,MAX_PATH,IniFile);
		if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
			WritePrivateProfileString("Configuration","BasePath",FullSETFileName,IniFile);
		int l;
		l=strlen(FullSETFileName);
		if(l==0){
			strcpy(FullSETFileName,StartPath);
			l=strlen(FullSETFileName);
		}
		if(l && *(FullSETFileName+l-1)!='\\')strcpy(FullSETFileName+l++,"\\");

		GetPrivateProfileString("Configuration","SetFile","AVP.SET",FullSETFileName+l,MAX_PATH-l,IniFile);
		if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
			WritePrivateProfileString("Configuration","SetFile",FullSETFileName+l,IniFile);



		FullKEYFileName=new char[MAX_PATH];
		GetPrivateProfileString("Configuration","KeysPath","",FullKEYFileName,MAX_PATH,IniFile);
		if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
			WritePrivateProfileString("Configuration","KeysPath",FullKEYFileName,IniFile);
		l=strlen(FullKEYFileName);
		if(l && *(FullKEYFileName+l-1)!='\\')strcpy(FullKEYFileName+l++,"\\");

		_chdir(StartPath);


		//Localisations//
		/////////////////
		LoadLocalStrings();

		char ExeName[MAX_PATH];
		GetModuleFileName(ghInstance,ExeName,MAX_PATH);
		sign_check=SignReportMB(ExeName,SignCheckFile(ExeName));
		if (sign_check)goto ret;
		sign_check=sign_check_file(ExeName,1,(void**)1,0,0);

		LoadSettings();

		DWORD pid;
		if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_NT)
		{
			HKEY hKey;
			DWORD server=0;
			if(RegOpenKey(HKEY_LOCAL_MACHINE,AVP_REGKEY_COMPONENTS_FULL "\\102",&hKey) == ERROR_SUCCESS) 
			{
				DWORD n = 4;
				RegQueryValueEx(hKey,"Server",NULL,NULL,(UCHAR*)&server,&n);
				RegCloseKey(hKey);
			}
			pid=server?AVP_APID_NT_SERVER:AVP_APID_MONITOR_WIN_NTWS;
		}
		else pid=AVP_APID_MONITOR_WIN_95;


	DWORD Level;
	BOOL Valid;
	BOOL AnyKeyFound;

	Level=0;
	Valid=0;
	AnyKeyFound=0;

	if(strlen(FullKEYFileName))
	AnyKeyFound|=LoadKeys(pid,
		FullKEYFileName, &Level, &Valid, &KeyHandleArray);

	AnyKeyFound|=LoadKeys(pid,
		FullHELPFileName, &Level, &Valid, &KeyHandleArray);


	if(!AnyKeyFound)
	{
		MessageBox(NULL, MS(IDS_KEY_ABSENT), MS( IDS_FULL_NAME ),	MB_ICONINFORMATION|MB_OK);
	}
	else{
		if(Level==0){
			char* m;
			switch(pid)
			{
			case AVP_APID_MONITOR_WIN_NTWS:
				m=MS(IDS_KEY_PLATHFORM_NT);
				break;
			case AVP_APID_NT_SERVER:
				m=MS(IDS_KEY_PLATHFORM_NT_SERVER);
				break;
			case AVP_APID_MONITOR_WIN_95:
			default:
				m=MS(IDS_KEY_PLATHFORM_95);
				break;
			}
			MessageBox(NULL,m, MS( IDS_FULL_NAME ),	MB_ICONINFORMATION|MB_TOPMOST);
		}
		else{
			if(Level) functionality |= FN_UPDATES;
			if(Level>=10) functionality |= FN_MINIMAL;
			if(Level>=20) functionality |= FN_OPTIONS;
			if(Level>=30) functionality |= FN_FULL;
			if(!Valid){
				functionality &= ~FN_UPDATES;
				MessageBox(NULL, MS(IDS_UPDATE_EXPIRED), MS( IDS_FULL_NAME ), MB_ICONINFORMATION|MB_OK);
			}
		}

	}
	if(!functionality){
		disableMode=1;
		Packed=1;
		FileMask=0;	// 0 - smart; ; 1 - programs; 2 - all files; 3 - user defined
		Actions=0;  // 0 - deny, 1 - disinf, 2 - delete, 
		Analyser=1;
		Warnings=1;
		Report=0;
		LimitCompound=0;
		Archives=0;
		MailBases=0;
		MailPlain=0;
		Exclude=0;
	}
	else
	{
		if(!(functionality & FN_OPTIONS))
			ResLimited=1;
	}
	RetrSettings();

	*m_Name=0;
	*m_Comp=0;
	HKEY hKey;
	if(RegOpenKey(HKEY_LOCAL_MACHINE,AVP_REGKEY_INST_WS_FULL,&hKey) == ERROR_SUCCESS) 
	{
		DWORD n;
		n= MAX_PATH;
		RegQueryValueEx(hKey,AVP_REGVALUE_USERNAME,NULL,NULL,(UCHAR*)m_Name,&n);
		n= MAX_PATH;
		RegQueryValueEx(hKey,AVP_REGVALUE_USERCOMPANY,NULL,NULL,(UCHAR*)m_Comp,&n);
		RegCloseKey(hKey);
	}

	if(*m_Name==0)	GetPrivateProfileString("Registration","NAME",   "",m_Name,MAX_PATH,IniFile);
	if(*m_Comp==0)	GetPrivateProfileString("Registration","COMPANY","",m_Comp,MAX_PATH,IniFile);

	if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		AVPServiceLoadedByMonitor=0;
ag:
		hDevice=CreateFile("\\\\.\\FSAVPDevice0",
			GENERIC_READ|GENERIC_WRITE,	FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
		if(hDevice == INVALID_HANDLE_VALUE ){
			if(!NTControlService("F-SECURE Gatekeeper",2)){
				if(!NTControlService("F-SECURE Recognizer",1))
					goto err_drv;
				if(!NTControlService("F-SECURE Filter",1))
					goto err_drv;
				if(!NTControlService("F-SECURE Gatekeeper",1))
					goto err_drv;
				GKServiceLoadedByMonitor=1;
			}

			if(!NTControlService("F-SECURE AVP",1))
				goto err_drv;

			AVPServiceLoadedByMonitor=1;
			goto ag;
		}
	}
	else  //VER_PLATFORM_WIN32_WINDOWS)
	{ 
		if(SignReportMB("AVP95.VXD",SignCheckFile("AVP95.VXD"))) goto ret;
		if(SignReportMB("AVP_IO.VXD",SignCheckFile("AVP_IO.VXD"))) goto ret;
		ServiceRegister();
		AVPServiceLoadedByMonitor=1;

		static char szGKOptKey[]="Software\\Data Fellows\\F-Secure\\Gatekeeper\\Options\\Scan";
		static char szGKAVPKey[]="Software\\Data Fellows\\F-Secure\\Gatekeeper\\Plug-Ins\\AVP";

		HKEY hKey;
		BOOL regErr=0;
		DWORD dwDisp;

        if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_LOCAL_MACHINE,szGKOptKey,0,0,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisp))
		{
				DWORD dwData=1;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"SilentMode",0,REG_DWORD,(LPBYTE)&dwData,sizeof(dwData)))
					regErr=1;
				RegCloseKey(hKey);
		}
		else	regErr=1;

        if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_LOCAL_MACHINE,szGKAVPKey,0,0,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisp))
		{
				DWORD dwData=1;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"ID",0,REG_DWORD,(LPBYTE)&dwData,sizeof(dwData)))
					regErr=1;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"Status",0,REG_DWORD,(LPBYTE)&dwData,sizeof(dwData)))
					regErr=1;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"Type",0,REG_DWORD,(LPBYTE)&dwData,sizeof(dwData)))
					regErr=1;
				dwData=0x50;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"Priority",0,REG_DWORD,(LPBYTE)&dwData,sizeof(dwData)))
					regErr=1;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"Driver",0,REG_SZ,(LPBYTE)"AVP95.VXD",10))
					regErr=1;
                if(ERROR_SUCCESS!=RegSetValueEx(hKey,"Name",0,REG_SZ,(LPBYTE)"AVP95   ",9))
					regErr=1;
				RegCloseKey(hKey);
		}
		else	regErr=1;
		if(regErr) goto err_drv;


		hDeviceIO=LoadVxd("AVP_IO.VXD",hinst_);
		if(hDeviceIO == INVALID_HANDLE_VALUE)
			goto err_drv;

		hDevice=LoadVxd("AVP95.VXD",hinst_);
		if(hDevice == INVALID_HANDLE_VALUE)
		{
			DWORD e=GetLastError();
			goto err_drv;
		}

		hDeviceGK=LoadVxd("GK95.VXD",hinst_);
		if(hDeviceGK == INVALID_HANDLE_VALUE)
			goto err_drv;

		char wd[MAX_PATH];
		char cd[MAX_PATH];
		char* lp=NULL;
		GetModuleFileName(NULL,cd,MAX_PATH);
		GetFullPathName(cd, MAX_PATH, wd, &lp);
		if(lp)*(lp-1)=0;
		GetCurrentDirectory(MAX_PATH,cd);
		SetCurrentDirectory(wd);

		if(!DvpSubDriverCtrl(hDeviceGK, DVPSUBDRVFN_LOAD, NULL))
		{
			SetCurrentDirectory(cd);
			goto err_drv;
		}
		SetCurrentDirectory(cd);


		SetDvpOption(hDeviceGK, DVPOPTION_ACTION, 0);
		SetDvpOption(hDeviceGK, DVPOPTION_PROMPTUSER, 0);
		SetDvpOption2(hDeviceGK, DVPOPTION_SCANEXTENSIONS, "*\0\0", 4);

		if(!strstr(lpCmdLine,"/nsoc "))
			SetDvpOption(hDeviceGK, DVPOPTION_SCANONCREATERENAME, 1);

		ret=DeviceIoControl(hDeviceGK, DVPIOC_ENABLESCAN, "1", 2, 0, 0, &count, 0);

	}
	
	if(!DeviceIoControl(hDevice,IOCTL_AVP_GET_VERSION,NULL,0,&ret,4,&count,0))
		goto err_drv;
	
	if(ret!=AVP_IOCTL_Version)
		goto err_drv;

	if(!APC_Init(hDevice))
		goto err_drv;

/*
	DWORD p[2];
	p[0]=0;
	p[1]=(DWORD)MS(IDS_ALERT_TEXT);
	if(!DeviceIoControl(hDevice,IOCTL_AVP_SET_MESSAGE_TEXT,p,8,NULL,0,&count,0))
		goto err_drv;
	p[0]=1;
	p[1]=(DWORD)MS(IDS_ALERT_SUSP);
	if(!DeviceIoControl(hDevice,IOCTL_AVP_SET_MESSAGE_TEXT,p,8,NULL,0,&count,0))
		goto err_drv;
	p[0]=2;
	p[1]=(DWORD)MS(IDS_ALERT_WARNING);
	if(!DeviceIoControl(hDevice,IOCTL_AVP_SET_MESSAGE_TEXT,p,8,NULL,0,&count,0))
		goto err_drv;
*/			
	SetMFlags(MFlags);

	ret=1;
	if(!DeviceIoControl(hDevice,IOCTL_AVP_STATIC_MFLAGS, &ret ,4,NULL,0,&count,0))
		goto err_drv;
	
	bApplied=0;
	ApplyNow();

	////////////////////////////////////////////////////////////////////////////////
	// Andrey
	if ((!CCClientLoadDLL(NULL)) || (!InitCCClientInterface(2))) 
			ret=0;
		
	// Andrey
	////////////////////////////////////////////////////////////////////////////////

	hMainDlg=CreateDialog(hinst_, MAKEINTRESOURCE(IDD_MAINDIAL), 0, (DLGPROC) &MainDlgProc);

	SetWindowText(hMainDlg,MS(IDS_FULL_NAME));
	SetActiveWindow(NULL);
	PostMessage(hMainDlg,MYWM_AFTERINIT,0,0);

#ifdef SPLASH_
	hSplash=CreateDialog(hinst_, MAKEINTRESOURCE(IDD_SPLASH), hMainDlg, NULL);
	if(HMEDIA1 || HDAVID)
	{
		SetWindowLong(GetDlgItem(hSplash,IDC_DAVID),GWL_USERDATA,(LONG)(HMEDIA1?HMEDIA1:HDAVID));
		oldDavidDlgProc=(MyProcPtr)SetWindowLong(GetDlgItem(hSplash,IDC_DAVID),GWL_WNDPROC,(LONG)DavidDlgProc);
	}
	RedrawWindow(hSplash,NULL,0,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	SetWindowPos(hSplash,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	hCursor=SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
#endif

	MSG msg;
	while(GetMessage(&msg,NULL /*hMainDlg*/ ,0,0))
	{
//		if(IsDialogMessage(hMainDlg,&msg))continue;
		if(TranslateMessage(&msg))continue;
		DispatchMessage(&msg);
	}

	WriteSettings();
	ReportWriteLine(MS(IDS_REP_UNLOADED));

#ifdef SPLASH_
	if(hSplash)
	{
		SetCursor(hCursor);
		DestroyWindow(hSplash);
		hSplash=NULL;
	}
#endif

	goto ret;

err_drv:
	DriverFailedAndClose();

ret:

	if(CCClientDone)CCClientDone();

//	SendMessage(HWND_BROADCAST, WM_AVPVXD, UNLOAD_VXD, 0);
// now unload with closing.

	SendMessage(HWND_BROADCAST, WM_AVPVXD, CLOSE, 0);
	Sleep(500);

	if(hMainDlg!=NULL)
		DestroyWindow(hMainDlg);

//	if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
		APC_Destroy(hDevice);

	if(hDevice!=INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);

	if(hDeviceGK!=INVALID_HANDLE_VALUE)
	{
		DvpSubDriverCtrl(hDeviceGK, DVPSUBDRVFN_UNLOAD ,NULL);
		CloseHandle(hDeviceGK);
	}

	if(hDeviceIO!=INVALID_HANDLE_VALUE)
		CloseHandle(hDeviceIO);

	if(AVPServiceLoadedByMonitor)
		NTControlService("F-SECURE AVP",0);

	if(GKServiceLoadedByMonitor)
		NTControlService("F-SECURE Gatekeeper",0);


	while(KeyHandleArray.GetSize())
	{	
		DATA_Remove( (HDATA)KeyHandleArray[0], 0 );
		KeyHandleArray.RemoveAt(0);
	}


	if(IniFile) delete [] IniFile;
	if(StartPath) delete [] StartPath;
	if(FullINIFileName) delete [] FullINIFileName;
	if(FullHELPFileName) delete [] FullHELPFileName;
	if(FullSETFileName) delete [] FullSETFileName;
	if(FullKEYFileName) delete [] FullKEYFileName;

	if(RFName) delete [] RFName;
	if(LimitSize) delete [] LimitSize;
	if(LimitCompoundSize) delete [] LimitCompoundSize;
	if(UserMasks) delete [] UserMasks;
	if(ExcludeMasks) delete [] ExcludeMasks;

	if(SpaceCritSec)DeleteCriticalSection(&TotalSpaceCritSec);
	if(SpaceCritSec)DeleteCriticalSection(&ProcessedSpaceCritSec);

	for(i=MessageArray.GetSize();i;)
	{
		char* s;
		i--;
		if(s=(char*)MessageArray.GetAt(i)) 
			delete s;
	}
	MessageArray.RemoveAll();
	CloseHandle(MonitorMutex);


exec_ret:
	if(*execName)
		_spawnl(_P_NOWAIT,execName,lpCmdLine,NULL);

	if(lpCmdLine) delete lpCmdLine;
	AvpmRegisterThread(0 ,OsVersion.dwPlatformId);
	return 0;
}


LRESULT WINAPI SupDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch(msg_)
	{
		case WM_INITDIALOG:
			SetWindowText(hwnd_, MS(IDS_SUPPORT_DLG));
			SetWindowText(GetDlgItem(hwnd_,IDC_SUPPORT),m_Support);
			return 1;
		
		case WM_COMMAND:
			switch(wparam_){
				case IDOK:
					EndDialog(hwnd_, 1);
					break;
				case IDCANCEL:
					EndDialog(hwnd_, 1);
					break;
			}
		break;
		
		case WM_CLOSE:
			EndDialog(hwnd_, 1);
			return 0;
	}
	return 0;
}


void LoadLocalStrings()
{
	HINSTANCE hinew;
	char locfile[MAX_PATH];
	char buf[0x200];
	int len;
	int i=0;


	while((len=LoadString(ghInstance,i+IDS_ENABLE,buf,0x200))!=0){
		char* str=new char[len+1];
		strcpy(str,buf);
		MessageArray.SetAtGrow(i,str);
		i++;
	}
/*
HMEDIA1=LoadBitmap(ghInstance,MAKEINTRESOURCE(IDB_MEDIA1));
HMEDIA2=LoadBitmap(ghInstance,MAKEINTRESOURCE(IDB_MEDIA2));
*/
	
	GetPrivateProfileString("AVPM","LocFile","AVPM_LOC.DLL",locfile,MAX_PATH,IniFile);
	if(	_access(IniFile,0)!=0||_access(IniFile,2)==0)
		WritePrivateProfileString("AVPM","LocFile",locfile,IniFile);
	
	if(strlen(locfile)){
		hinew=LoadLibrary(locfile);
		if(hinew==NULL) return;
		i=0;
		while((len=LoadString(hinew,i+IDS_ENABLE,buf,0x200))!=0){
			char* str=new char[len+1];
			strcpy(str,buf);
			char* s=(char*)MessageArray.GetAt(i);
			if(s)delete s;
			MessageArray.SetAtGrow(i,str);
			i++;
		}
		HDAVID=LoadBitmap(hinew,MAKEINTRESOURCE(IDB_DAVID));
		HMEDIA1=LoadBitmap(hinew,MAKEINTRESOURCE(IDB_MEDIA1));
		HMEDIA2=LoadBitmap(hinew,MAKEINTRESOURCE(IDB_MEDIA2));

		if( HMEDIA1 || HMEDIA2 || LoadString(hinew,IDS_LIMITED_FLAG,buf,0x200))
			ResLimited=1;

		FreeLibrary(hinew);
	}
}

BOOL LoadSettings()
{
	EnableCheck=(UINT)GetPrivateProfileInt(("Settings"),("Enable"),1,FullINIFileName);
	Packed=(UINT)GetPrivateProfileInt(("Settings"),("Packed"),1,FullINIFileName);
	Archives=(UINT)GetPrivateProfileInt(("Settings"),("Archives"),0,FullINIFileName);
	MailPlain=(UINT)GetPrivateProfileInt(("Settings"),("MailPlain"),1,FullINIFileName);
	MailBases=(UINT)GetPrivateProfileInt(("Settings"),("MailBases"),0,FullINIFileName);
	Analyser=(UINT)GetPrivateProfileInt(("Settings"),("Analyser"),1,FullINIFileName);
	FileMask=(UINT)GetPrivateProfileInt(("Settings"),("FileMask"),0,FullINIFileName);
	Actions=(UINT)GetPrivateProfileInt(("Settings"),("Actions"),0,FullINIFileName);
	Alert=(UINT)GetPrivateProfileInt(("Settings"),("Alert"),1,FullINIFileName);
	Warnings=(UINT)GetPrivateProfileInt(("Settings"),("Warnings"),1,FullINIFileName);
	Report=(UINT)GetPrivateProfileInt(("Settings"),("Report"),0,FullINIFileName);
	Limit=(UINT)GetPrivateProfileInt(("Settings"),("Limit"),1,FullINIFileName);

	TryToDisinfect=(UINT)GetPrivateProfileInt(("Settings"),("TryToDisinfect"),1,FullINIFileName);

	GetPrivateProfileString(("Settings"),("LimitSize"),"500",LimitSize,0x40,FullINIFileName);
	GetPrivateProfileString(("Settings"),("ReportFileName"),"Avpm_rep.txt",RFName,MAX_PATH,FullINIFileName);
	GetPrivateProfileString(("Settings"),("Masks"),"",UserMasks,0x200,FullINIFileName);
	LimitSizeInBytes=1024*atoi(LimitSize);

	LimitCompound=(UINT)GetPrivateProfileInt(("Settings"),("LimitCompound"),0,FullINIFileName);
	GetPrivateProfileString(("Settings"),("LimitCompoundSize"),"2048",LimitCompoundSize,0x40,FullINIFileName);

	Exclude=(UINT)GetPrivateProfileInt(("Settings"),("Exclude"),0,FullINIFileName);
	GetPrivateProfileString(("Settings"),("ExcludeMasks"),"",ExcludeMasks,0x200,FullINIFileName);

	Xpos=(UINT)GetPrivateProfileInt(("Position"),("X"),0,FullINIFileName);
	Ypos=(UINT)GetPrivateProfileInt(("Position"),("Y"),0,FullINIFileName);
	RECT r;
	GetWindowRect(GetDesktopWindow(),&r);
	if((r.right-r.left)<Xpos+10)Xpos=30;
	if((r.bottom-r.top)<Ypos+10)Ypos=30;

	startupMem=(BOOL)GetPrivateProfileInt(("Startup"),("Memory"),1,FullINIFileName);
	startupMbr=(BOOL)GetPrivateProfileInt(("Startup"),("MBR"),1,FullINIFileName);
	startupBoot=(BOOL)GetPrivateProfileInt(("Startup"),("Boot"),1,FullINIFileName);
	startupSelf=(BOOL)GetPrivateProfileInt(("Startup"),("Self"),1,FullINIFileName);
	m_BaseDelay=(UINT)GetPrivateProfileInt(("Customize"),("UpdateInterval"),14,FullINIFileName);

	RetrSettings();
	return 1;
}

BOOL WriteSettings()
{
	char buf[0x200];
	WritePrivateProfileString(("Settings"),("Enable"),_itoa(EnableCheck,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Packed"),_itoa(Packed,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Archives"),_itoa(Archives,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("MailPlain"),_itoa(MailPlain,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("MailBases"),_itoa(MailBases,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Analyser"),_itoa(Analyser,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("FileMask"),_itoa(FileMask,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Masks"),UserMasks,FullINIFileName);
	WritePrivateProfileString(("Settings"),("Actions"),_itoa(Actions,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Warnings"),_itoa(Warnings,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Report"),_itoa(Report,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("Limit"),_itoa(Limit,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("LimitSize"),LimitSize,FullINIFileName);
	WritePrivateProfileString(("Settings"),("ReportFileName"),RFName,FullINIFileName);

	WritePrivateProfileString(("Settings"),("Alert"),_itoa(Alert,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("TryToDisinfect"),_itoa(TryToDisinfect,buf,10),FullINIFileName);

	WritePrivateProfileString(("Settings"),("LimitCompound"),_itoa(LimitCompound,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("LimitCompoundSize"),LimitCompoundSize,FullINIFileName);

	WritePrivateProfileString(("Settings"),("Exclude"),_itoa(Exclude,buf,10),FullINIFileName);
	WritePrivateProfileString(("Settings"),("ExcludeMasks"),ExcludeMasks,FullINIFileName);

	WritePrivateProfileString(("Customize"),("UpdateInterval"),_itoa(m_BaseDelay,buf,10),FullINIFileName);

	return 1;
}

void UpdateSettings()
{
	EnableCheck=sEnableCheck;
	
	Packed=sPacked;
	Archives=sArchives;
	MailBases=sMailBases;
	MailPlain=sMailPlain;
	FileMask=sFileMask;	// 0 - smart; ; 1 - programs; 2 - all files; 3 - user defined
	Exclude=sExclude;
	
	Actions=sActions;  // 0 - ask, 1 - disinf, 2 - delete, 3 - deny
	Analyser=sAnalyser;
	Warnings=sWarnings;
	Alert=sAlert;
	Report=sReport;
	Limit=sLimit;
	LimitCompound=sLimitCompound;
}

void RetrSettings()
{
	sEnableCheck=EnableCheck;
	
	sPacked=Packed;
	sArchives=Archives;
	sMailBases=MailBases;
	sMailPlain=MailPlain;
	sFileMask=FileMask;	// 0 - smart; ; 1 - programs; 2 - all files; 3 - user defined
	sExclude=Exclude;

	sActions=Actions;  // 0 - ask, 1 - disinf, 2 - delete, 3 - deny

	sAnalyser=Analyser;
	sWarnings=Warnings;
	sAlert=Alert;
	sReport=Report;
	sLimit=Limit;
	sLimitCompound=LimitCompound;
	
}

static void SaveMasks(char* str,DWORD ctlcode)
{
	DWORD  count;
	//AVP_AddMask(NULL);
	if (!DeviceIoControl(hDevice,ctlcode,"",4,NULL,0,&count,0)){
		DriverFailedAndClose();
		return;
	}
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
			if (!DeviceIoControl(hDevice,ctlcode,span,l+1,NULL,0,&count,0)){
				DriverFailedAndClose();
				return;
			}
			span+=l;
			span++;
		}
	}
}


DWORD SetMFlags(DWORD& mf){
	mf=0;
	if(Warnings) mf |= MF_WARNINGS ;
	if(Analyser) mf |= MF_CA ;
	if(Packed) mf |= MF_PACKED ;
	if(Archives) mf |= MF_ARCHIVED ;
	if(MailBases) mf |= MF_MAILBASES ;
	if(MailPlain) mf |= MF_MAILPLAIN ;
	
	if(Actions==2) mf |= MF_D_DELETEALL ;
	else if(Actions==1) mf |= MF_D_CUREALL ;
	else mf |= MF_D_DENYACCESS ;
	
	switch(FileMask){
		case 0: mf |= MF_MASK_SMART ; break;
		case 1: mf |= MF_MASK_PROG ; break;
		case 2: mf |= MF_MASK_ALL ; break;
		case 3: mf |= MF_MASK_USER ; break;
		default: mf |= MF_MASK_SMART ; break;
	}
	if(Exclude) mf |= MF_MASK_EXCLUDE;

	if(sign_check!=SIGN_BAD_PARAMETERS){
			functionality&=~FN_DISINFECT;
			disableMode=1;
			mf=0;
	}

	return mf;
}

DWORD GetMFlags(DWORD& mf){
	Warnings=0;
	Analyser=0;
	Packed=0;
	Archives=0;
	MailBases=0;
	MailPlain=0;
	Exclude=0;

	if(mf & MF_WARNINGS)Warnings=1;
	if(/*!!(functionality & FN_CA) &&*/ mf & MF_CA) Analyser=1;
	if(/*!!(functionality & FN_UNPACK) &&*/ mf & MF_PACKED) Packed=1;
	if(/*!!(functionality & FN_EXTRACT) &&*/ mf & MF_ARCHIVED) Archives=1;
	if(/*!!(functionality & FN_MAIL) &&*/ mf & MF_MAILBASES) MailBases=1;
	if(/*!!(functionality & FN_MAIL) &&*/ mf & MF_MAILPLAIN) MailPlain=1;
	
	switch (mf & MF_D_){
		default:             
		case MF_D_ASKCURE: Actions=0; break;
		case MF_D_CUREALL: Actions=1; break;
		case MF_D_DELETEALL: Actions=2; break;
//		case MF_D_DENYACCESS: Actions=3;break;
	}

	switch( mf & MF_MASK_ ){
		case MF_MASK_SMART: FileMask=0; break;
		case MF_MASK_PROG: FileMask=1; break;
		case MF_MASK_ALL: FileMask=2; break;
		case MF_MASK_USER: FileMask=3; break;
		default: FileMask=0; break;
	}
	if(mf & MF_MASK_EXCLUDE) Exclude=1;
	
	RetrSettings();
	return mf;
}

void CreateMenuItem(MENUITEMINFO& mii, UINT wId, LPSTR str, UINT state)
{
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask= MIIM_DATA | MIIM_ID | MIIM_TYPE | MIIM_STATE;
	mii.fType=MFT_STRING;
	mii.fState=state;
	mii.wID=wId;
	mii.dwItemData=90;
	mii.dwTypeData=str;
	mii.cch=strlen(str);
}

BOOL CreateMenuBox(HWND wnd_, HMENU menu_)
{
	DWORD  count;
 	MENUITEMINFO msep;
	msep.cbSize=sizeof(MENUITEMINFO);
	msep.fMask=MIIM_TYPE;
	msep.fType=MFT_SEPARATOR;

	MENUITEMINFO mi;
	if (!DeviceIoControl(hDevice, IOCTL_AVP_GET_STAT, 0, 0, (void*)&sStat, sizeof(AVPStat), &count, 0 ))
	{
		//MessageBox(hwnd_, "timer DeviceIoControl failed\n", MS( IDS_FULL_NAME ), MB_ICONSTOP |MB_TOPMOST );
		return 0;
	}
	if(EnableCheck!=(BOOL)sStat.Enable)
		EnableStateChanged();

	if(!ResLimited)
	{
		CreateMenuItem(mi, EnableCheck?MI_DISABLE:MI_ENABLE, MS((EnableCheck?IDS_DISABLE:IDS_ENABLE)), 
			(noExitMode||disableMode||(scanThread && !scanThreadPaused))?MFS_DISABLED:MFS_ENABLED);
		InsertMenuItem(menu_, 2, 1, &mi);

		CreateMenuItem(mi, MI_PROPERTIES, MS(IDS_SETTINGS));
		InsertMenuItem(menu_, 1, 1, &mi);

		InsertMenuItem(menu_, 3, 1, &msep);
	}
	CreateMenuItem(mi, MI_EXIT, AVPServiceLoadedByMonitor?(MS(IDS_UNLOAD)):(MS(IDS_EXIT)), 
		noExitMode?MFS_DISABLED:MFS_ENABLED);
	InsertMenuItem(menu_, 6, 1, &mi);

	return 1;
}

/*	
LoadKey(const char* filename)
{
	BOOL ret=FALSE;
	HANDLE f;
	BOOL wrongKey=1;
	tm tstr;

	time_t tla;
	time_t curt;

	f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f!=INVALID_HANDLE_VALUE){
		AVP_KeyHeader KeyHeader;
		AVP_Key* Key;
		BYTE *cbuf=NULL;
		BYTE *buf=NULL;
		UINT i;
		DWORD bread;

		if(!ReadFile(f,&KeyHeader,sizeof(AVP_KeyHeader),&bread,NULL)) goto clof;
		if(KeyHeader.Magic!=(DWORD)AVP_KEY_MAGIC)goto clof;
		cbuf=new BYTE[KeyHeader.CompressedSize];
		buf=new BYTE[KeyHeader.UncompressedSize];

		if(!ReadFile(f,cbuf,KeyHeader.CompressedSize,&bread,NULL)) goto clo;
		if(KeyHeader.CompressedCRC!=CalcSum(cbuf,KeyHeader.CompressedSize))goto clo;
		for(i=0;i<KeyHeader.CompressedSize;i++) cbuf[i]^=(BYTE)i;
		if(KeyHeader.UncompressedSize!=unsqu(cbuf,buf))goto clo;

		wrongKey=0;

		Key=(AVP_Key*)buf;

		OemToChar((char*)(buf+Key->NameOffs), m_Name);
		OemToChar((char*)(buf+Key->OrgOffs), m_Comp);
		OemToChar((char*)(buf+Key->RegNumberOffs), m_Regnumber);

		if(strlen(m_Name)==0)
			GetPrivateProfileString("Registration","NAME","",m_Name,60,IniFile);
		if(strlen(m_Comp)==0)
			GetPrivateProfileString("Registration","COMPANY","",m_Comp,60,IniFile);
		if(strlen(m_Regnumber)==0)
			GetPrivateProfileString("Registration","NUMBER","",m_Regnumber,20,IniFile);

		OemToChar((char*)(buf+Key->CopyInfoOffs), m_CopyInfo);
		OemToChar((char*)(buf+Key->SellerOffs), m_Seller);
		OemToChar((char*)(buf+Key->SupportOffs), m_Support);

		m_Registered=((Key->Flags) & KEY_F_REGISTERED)!=0;
		m_BaseDelay=Key->BaseDelay;

		m_Every=((Key->Flags) & KEY_F_INFOEVERYLAUNCH)!=0;
		if(m_Every)	MessageBox(0,m_CopyInfo,MS( IDS_FULL_NAME ),MB_OK|MB_TOPMOST);

		m_RemoteLaunch=((Key->Options) & KEY_O_REMOTELAUNCH)!=0;
		if(!m_RemoteLaunch){
			char drv[4];
			strncpy(drv,StartPath,3);
			drv[3]=0;
			if(GetDriveType(drv) == DRIVE_REMOTE){
				MessageBox(0,MS(IDS_REMOTE_LAUNCH),MS( IDS_FULL_NAME ),MB_ICONSTOP|MB_TOPMOST);
				goto clo;
			}
		}
		if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS && !(Key->Platforms & KEY_P_WIN95)){
			MessageBox(0,MS(IDS_KEY_PLATHFORM_95),MS( IDS_FULL_NAME ),MB_ICONINFORMATION|MB_TOPMOST);
			goto clo;
		}
		if(OsVersion.dwPlatformId==VER_PLATFORM_WIN32_NT && !(Key->Platforms & KEY_P_WINNT)){
			MessageBox(0,MS(IDS_KEY_PLATHFORM_NT),MS( IDS_FULL_NAME ),MB_ICONINFORMATION|MB_TOPMOST);
			goto clo;
		}

		time(&curt);
		if(NULL==gmtime(&curt)) goto no_expir;
		tstr=*gmtime(&curt);

		if(Key->ExpirYear<2036 && (Key->ExpirMonth!=0) && (Key->ExpirDay!=0)){
			tstr.tm_mday=Key->ExpirDay;//	Day of month (1–31)
			tstr.tm_mon=Key->ExpirMonth;// (0–11; January = 0)
			tstr.tm_year=Key->ExpirYear-1900;	
			m_LicenceDate = mktime(&tstr);
		}
		else	goto no_expir;

		m_LicenceAlarm=Key->AlarmDays;

		char fs[100];
		if(difftime(curt,m_LicenceDate)>0)
		{
			sprintf(fs,MS( IDS_EXPIRIED ),tstr.tm_mday,tstr.tm_mon,tstr.tm_year+1900);
			MessageBox(0,fs,MS( IDS_FULL_NAME ),MB_ICONSTOP|MB_TOPMOST);
			goto clo;
		}
		sprintf(fs,MS( IDS_EXPIR_ALARM ),tstr.tm_mday,tstr.tm_mon,tstr.tm_year+1900);
		tstr.tm_mday-=m_LicenceAlarm;
		tla=mktime(&tstr);
		if(difftime(curt,tla)>0)  
			MessageBox(0,fs,MS( IDS_FULL_NAME ),MB_ICONINFORMATION|MB_TOPMOST);

no_expir:		
		m_Disinfect=((Key->Options) & KEY_O_DISINFECT)!=0;
		m_Unpack=((Key->Options) & KEY_O_UNPACK)!=0;
		m_Extract=((Key->Options) & KEY_O_EXTRACT)!=0;
		m_MailBases=((Key->Options) & KEY_O_MAILBASES)!=0;
		m_MailPlain=((Key->Options) & KEY_O_MAILPLAIN)!=0;
		m_CA=((Key->Options) & KEY_O_CA)!=0;
		m_RemoteScan=((Key->Options) & KEY_O_REMOTESCAN)!=0;
		m_Monitor=((Key->Options) & KEY_O_MONITOR)!=0;
		m_CRC=((Key->Options) & KEY_O_CRC)!=0;

		ret=TRUE;
clo:
		if(cbuf) delete [] cbuf;
		if(buf) delete [] buf;
clof:


		CloseHandle(f);
	}
	
	if(!m_Monitor)
	{
		wrongKey=1;
		ret=0;
	}

	if(wrongKey)
	{
		MessageBox(0,MS(IDS_KEY_ABSENT),MS( IDS_FULL_NAME ),MB_ICONINFORMATION|MB_TOPMOST);
	}

	if(!m_CA) Analyser=0;
	if(!m_Unpack) Packed=0;
	if(!m_Extract) Archives=0;
	if(!m_MailBases) MailBases=0;
	if(!m_MailPlain) MailPlain=0;

	if(!ret)
	{
		clippedMode=1;
		Packed=0;
		FileMask=0;	// 0 - smart; ; 1 - programs; 2 - all files; 3 - user defined
		Actions=0;  // 0 - deny, 1 - disinf, 2 - delete, 
		Analyser=0;
		Warnings=0;
		Report=0;
		LimitCompound=0;
		Archives=0;
		MailBases=0;
		MailPlain=0;
		Exclude=0;
//		Alert=0;
	}
	RetrSettings();

	return ret;
}
*/

static char* insert2name3(char* s,const char* s2,const char* Name,const char* s3)
{
	char* nam;
	strcat(s,s2);
	strcat(s,"\n");
	nam=s+strlen(s);
	strcat(s,Name);
	if(strlen(nam)>43)strncpy(nam+strlen(nam)-40,"... ...",7);
	strcat(s,"\n");
	strcat(s,s3);
	return s;
}

LRESULT WINAPI AlertDlgProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch(msg_)
	{
	case WM_DESTROY:
		GetWindowRect(hwnd_,&AlertRect);
		break;
	case WM_PLAY_SOUND:
		PlaySound("SystemHand",0,SND_ALIAS);
		break;
	case WM_INITDIALOG:
		{
			DWORD count;
			char s[0x400];

			if(HMEDIA2 || HDAVID)
			{
				SetWindowLong(GetDlgItem(hwnd_,IDC_DAVID),GWL_USERDATA,(LONG)(HMEDIA2?HMEDIA2:HDAVID));
				oldDavidDlgProc=(MyProcPtr)SetWindowLong(GetDlgItem(hwnd_,IDC_DAVID),GWL_WNDPROC,(LONG)DavidDlgProc);
			}
			
			AVPScanObject* so;
			so=(AVPScanObject*)lparam_;
			char* ptr;

			strcpy(s,MS(IDS_ALERT_MB_INTRO));
			strcat(s,"\n");
			if(so->RFlags & RF_DETECT){
				strcat(s,MS(IDS_ALERT_MB_1_VIRUS));
				ptr=so->VirusName;
			}
			else if(so->RFlags & RF_WARNING){
				strcat(s,MS(IDS_ALERT_MB_1_WARNING));
				ptr=so->WarningName;
			}
			else if(so->RFlags & RF_SUSPIC){
				strcat(s,MS(IDS_ALERT_MB_1_SUSPICION));
				ptr=so->SuspicionName;
			}
			DeviceIoControl(hDevice, IOCTL_AVP_MAKE_FULL_NAME, &ptr, 4, s+strlen(s), 0x200, &count, 0 );
			strcat(s,"\n");
			if(so->RFlags & RF_DELETED){
				insert2name3(s,
					MS(IDS_ALERT_MB_2_DELETED),
					so->Name,
					MS(IDS_ALERT_MB_3_DELETED));
			}
			else if(so->RFlags & (RF_IO_ERROR|RF_NOT_OPEN)){
				insert2name3(s,
					MS(IDS_ALERT_MB_2_FAILED),
					so->Name,
					MS(IDS_ALERT_MB_3_IO_ERROR));
			}
			else if(so->RFlags & RF_CURE_FAIL){
				insert2name3(s,
					MS(IDS_ALERT_MB_2_FAILED),
					so->Name,
					MS(IDS_ALERT_MB_3_FAILED));
			}
			else if(so->RFlags & RF_CURE){
				insert2name3(s,
					MS(IDS_ALERT_MB_2_DISINFECTED),
					so->Name,
					MS(IDS_ALERT_MB_3_DISINFECTED));
			}
			else{
				insert2name3(s,
					MS(IDS_ALERT_MB_2_DETECTED),
					so->Name,
					MS(IDS_ALERT_MB_3_DETECTED));
			}
			strcat(s,"\n\n");

			if(so->RFlags & RF_DETECT)
				strcat(s,MS(IDS_ALERT_MB_4_VIRUS));
			else 
				strcat(s,MS(IDS_ALERT_MB_4_DANGER));



			SetWindowText(GetDlgItem(hwnd_,IDC_ALERT_MESSAGE), s);
			SetWindowText(GetDlgItem(hwnd_,IDC_TRY_TO_DISINFECT), MS(IDS_TRY_TO_DISINFECT));
			strcpy(s,MS(IDS_FULL_NAME));
			strcat(s,MS(IDS_ALERT_MB_CAPTION));
			SetWindowText(hwnd_, s);
//
	TryToDisinfect=1;
//
			if( (so->RFlags & RF_DETECT)
				&& !(so->RFlags & (RF_IO_ERROR|RF_CURE_FAIL|RF_CURE|RF_DELETED|RF_ARCHIVE|RF_NOT_OPEN))
//				&& !(so->InArc)
				)
			{
				CheckDlgButton(hwnd_,IDC_TRY_TO_DISINFECT,TryToDisinfect && (functionality & FN_DISINFECT));
				EnableWindow(GetDlgItem(hwnd_,IDC_TRY_TO_DISINFECT), !disableMode && (functionality & FN_DISINFECT));
			}else{
				CheckDlgButton(hwnd_,IDC_TRY_TO_DISINFECT,0);
				EnableWindow(GetDlgItem(hwnd_,IDC_TRY_TO_DISINFECT),FALSE);
				ShowWindow(GetDlgItem(hwnd_,IDC_TRY_TO_DISINFECT),SW_HIDE);
			}
			PostMessage(hwnd_,WM_PLAY_SOUND,0,0);

			break;
		}
	case WM_COMMAND:
		switch(wparam_){
			case IDOK:
				if(IsWindowVisible(GetDlgItem(hwnd_, IDC_TRY_TO_DISINFECT)))
				{
					TryToDisinfect=IsDlgButtonChecked(hwnd_,IDC_TRY_TO_DISINFECT);
					if(TryToDisinfect){
						EndDialog(hwnd_, IDOK);
						break;
					}
				}
				EndDialog(hwnd_, IDCANCEL);
				break;
			case IDCANCEL:
				EndDialog(hwnd_, IDCANCEL);
				break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd_, IDCANCEL);
		break;
	}
	return 0;
}


BOOL AlertAVPScanObject(AVPScanObject* so, BOOL crit)
{
	BOOL ret=FALSE;
	if(!so ) return ret;
	if( so->Type==0x8001)
	{
		if(SignReportMB(so->Name,so->Connection))
			PostMessage(hMainDlg, WM_CLOSE, 0, 0);
		return ret;
	}
	if(so->RFlags & RF_REPORT_DONE)return ret;
	if(crit)EnterCriticalSection(&callbackCriticalSection);
	ReportWriteObject(so);
	if(Alert){
		int dret=DialogBoxParam(ghInstance, MAKEINTRESOURCE(IDD_ALERT), 
//			GetDesktopWindow(),
			hMainDlg,
			(DLGPROC)AlertDlgProc,(DWORD)so);

		RedrawWindow(0,&AlertRect,0,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
		
		if(dret==IDOK 
//			&& !(so->InArc) 
			&& !(so->RFlags &(RF_CURE|RF_CURE_FAIL|RF_IO_ERROR|RF_DELETED|RF_NOT_OPEN)))
		{
			if(so->InArc)
			{
				char* s=strchr(so->Name,'/');
				if(s)*s=0;
			}
			DWORD count;
			so->RFlags=0;
			so->MFlags=(so->MFlags&~MF_D_)|MF_D_CUREALL|MF_APC_CURE_REQUEST;
			DeviceIoControl(hDevice,IOCTL_AVP_PROCESS_OBJECT,so,sizeof(AVPScanObject),so,sizeof(AVPScanObject),&count,0);
			ret=TRUE;
		}
	}
	if(crit)LeaveCriticalSection(&callbackCriticalSection);
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int LoadSettings(LPPOLICY lpPolicy)
{
	LPPROPERTY p;
	BOOL boRestoreDlg = FALSE;
	if (lpPolicy)
	{
		if(hPrShDlg) 
		{
			disableMode=1;
			noExitMode=1;

			SaveWindowPos();
			SendMessage(hPrShDlg,WM_CLOSE,0,0);
			boRestoreDlg = TRUE;
			bSettings = 0;
			hPrShDlg = 0;
		}

		p = GetPropertyByID(lpPolicy, PROPID_DW_SCANPACK);
		if (p) Packed = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_SCANARCH);
		if (p) Archives = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_SCANMAILPLAIN);
		if (p) MailPlain = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_SCANMAIL);
		if (p) MailBases = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_CA);
		if (p) Analyser = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_WARNINGS);
		if (p) Warnings = *(DWORD*)p->Data;
// File mask
		FileMask = 0;
		p = GetPropertyByID(lpPolicy, PROPID_DW_MASKPROG);
		if ((p) && (*(DWORD*)p->Data)) FileMask = 1;
		p = GetPropertyByID(lpPolicy, PROPID_DW_MASKALL);
		if ((p) && (*(DWORD*)p->Data)) FileMask = 2;
		p = GetPropertyByID(lpPolicy, PROPID_DW_MASKUSER);
		if ((p) && (*(DWORD*)p->Data)) FileMask = 3;

		p = GetPropertyByID(lpPolicy,PROPID_DW_MASKEXC);
		if (p) Exclude = *((DWORD*)p->Data);
		p = GetPropertyByID(lpPolicy, PROPID_ST_MASKEXC);
		if (p) strcpy(ExcludeMasks, p->Data);

// File mask
// Actions		
		Actions = 0;
//		p = GetPropertyByID(lpPolicy, PROPID_DW_DENY);
		p = GetPropertyByID(lpPolicy, PROPID_DW_CURE);
		if ((p) && (*(DWORD*)p->Data)) Actions = 1;
		p = GetPropertyByID(lpPolicy, PROPID_DW_DELETE);
		if ((p) && (*(DWORD*)p->Data)) Actions = 2;
// Actions		
		p = GetPropertyByID(lpPolicy, PROPID_DW_MBALERT);
		if (p) Alert = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_REPORT);
		if (p) Report = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_LIMIT);
		if (p) Limit = *(DWORD*)p->Data;
		p = GetPropertyByID(lpPolicy, PROPID_DW_LIMITCOMP);
		if (p) LimitCompound = *(DWORD*)p->Data;

		p = GetPropertyByID(lpPolicy, PROPID_ST_REPORTFILE);
		if (p) strcpy(RFName, p->Data);
		p = GetPropertyByID(lpPolicy, PROPID_ST_LIMITSIZE);
		if (p) strcpy(LimitSize, p->Data);
		LimitSizeInBytes=1024*atoi(LimitSize);
		p = GetPropertyByID(lpPolicy, PROPID_ST_MASKINC);
		if (p) strcpy(UserMasks, p->Data);
		p = GetPropertyByID(lpPolicy, PROPID_ST_LIMITCOMPOUNDSIZE);
		if (p) strcpy(LimitCompoundSize, p->Data);
		p = GetPropertyByID(lpPolicy,PROPID_DW_ENABLE);
		if (p) EnableCheck = *(DWORD*)p->Data;

        p = GetPropertyByID(lpPolicy,PROPID_DW_REF_BYTIMER);
        if (p) propByTimer = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_REF_BYEVENT);
        if (p) propByEvent = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_EVENT);
        if (p) propEvent = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_REF_INTERVAL);
        if (p) propInterval = *((DWORD*)p->Data);


		TryToDisinfect=(UINT)GetPrivateProfileInt(("Settings"),("TryToDisinfect"),1,FullINIFileName);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
		Xpos=(UINT)GetPrivateProfileInt(("Position"),("X"),0,FullINIFileName);
		Ypos=(UINT)GetPrivateProfileInt(("Position"),("Y"),0,FullINIFileName);
		RECT r;
		GetWindowRect(GetDesktopWindow(),&r);
		if((r.right-r.left)<Xpos+10)Xpos=30;
		if((r.bottom-r.top)<Ypos+10)Ypos=30;

		startupMem=(BOOL)GetPrivateProfileInt(("Startup"),("Memory"),1,FullINIFileName);
		startupMbr=(BOOL)GetPrivateProfileInt(("Startup"),("MBR"),1,FullINIFileName);
		startupBoot=(BOOL)GetPrivateProfileInt(("Startup"),("Boot"),1,FullINIFileName);
		startupSelf=(BOOL)GetPrivateProfileInt(("Startup"),("Self"),1,FullINIFileName);

		RetrSettings();

		EnableStateChanged();


		bApplied = FALSE;
		ApplyNow();
		if (boRestoreDlg)
			PostMessage(hMainDlg, WM_COMMAND, MI_PROPERTIES,0);
	}
	return 0;
}

