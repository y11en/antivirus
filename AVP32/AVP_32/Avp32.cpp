// avp32.cpp : Defines the class behaviors for the application.
//
#define __AVP32_CPP__


#include "stdafx.h"
#include <process.h>
#include "locale.h"
#include "avp32.h"
#include "avp32Dlg.h"
#include <io.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CRITICAL_SECTION TotalSpaceCriticalSection;

char	execName[0x200]="";

/////////////////////////////////////////////////////////////////////////////
// CAvp32App

BEGIN_MESSAGE_MAP(CAvp32App, CWinApp)
	//{{AFX_MSG_MAP(CAvp32App)
	ON_COMMAND( ID_HELP, OnHelp ) 
	ON_COMMAND( ID_HELP_INDEX, OnHelpIndex ) 
	ON_COMMAND( ID_CONTEXT_HELP, OnContextHelp ) 
	ON_COMMAND( ID_HELP_FINDER, OnHelpFinder ) 
	ON_COMMAND( ID_DEFAULT_HELP, OnHelpFinder ) 
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvp32App construction

CAvp32App::CAvp32App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAvp32App object

CAvp32App theApp;

/////////////////////////////////////////////////////////////////////////////
// CAvp32App initialization
int ExitCode=1;
HACCEL acc;
extern CString IniFile;
extern CString prof;

BOOL CAvp32App::InitInstance()
{
//	CoInitialize(0);
	if (!AfxOleInit())
	{
		AfxMessageBox("Ole initialization failed.");
		return FALSE;
	}

	AfxEnableControlContainer();

	InitializeCriticalSection(&TotalSpaceCriticalSection);
	CString CommandLine(m_lpCmdLine);
	CommandLine+=" ";
/*
	if(-1!=CommandLine.Find(moveKey)){
		ExitCode=16;
		return 0;
	}
*/

	GetModuleFileName(NULL,execName,0x200);
	char* lp=strrchr(execName,'.');

	if(*(lp-1)=='_'){
		char src[0x200];
		strcpy(src,execName);
		strcpy(lp-1,".EXE");
		int i=100;
		while(!CopyFile(src,execName,FALSE))
		{
			if(!i--){
				CString s;
				s.Format("%s is locked. Can't overwrite.",execName);
				AfxMessageBox(s);
				*execName=0;
				ExitCode=11;
				return 0;
			}
			Sleep(100);
		}
		return 0;
	}
	else{
		strcpy(lp,"_.EXE");
		DeleteFile(execName);
		*execName=0;
	}

#ifdef _MBCS
	setlocale ( LC_ALL, "" );
	_setmbcp(_MB_CP_LOCALE);
#endif

	acc=LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDD_AVP32_DIALOG));
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	char buf[0x200];
	char* ptr;


	char* HelpFilePath=(char*)malloc(0x200);
	GetFullPathName(m_pszHelpFilePath,0x200,HelpFilePath,&ptr);
	*ptr=0;
	SetCurrentDirectory(HelpFilePath);
	strcat(HelpFilePath,"AVP32.HLP");
	m_pszHelpFilePath=HelpFilePath;


	int ini=SearchPath(NULL,"AVP32.INI",NULL,0x200,buf,&ptr);
	CTime ct=CTime::GetCurrentTime();
	CFileStatus fs;
	
	if(CFile::GetStatus(buf,fs) && -1 != CommandLine.Find(_T("/d "))){
		if(fs.m_mtime.GetYear()==ct.GetYear()
		&& fs.m_mtime.GetMonth()==ct.GetMonth()
		&& fs.m_mtime.GetDay()==ct.GetDay())	return FALSE;
	}

	CAvp32Dlg dlg;
	m_pMainWnd = &dlg;

	int nResponse = dlg.DoModal();
	m_pMainWnd = NULL;

	if(	ini && ExitCode==0 ){
		fs.m_mtime=CTime::GetCurrentTime();
		CFile::SetStatus(buf,fs);
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
//#include <stdio.h>
int CAvp32App::ExitInstance() 
{
	CWinApp::ExitInstance();

	DeleteCriticalSection(&TotalSpaceCriticalSection);

	if(*execName){
		ExitCode=17;
		char buf[0x200];
		strcpy(buf,m_lpCmdLine);
		strcat(buf," ");
		_spawnl(_P_NOWAIT,execName,buf,NULL);
	}

	return ExitCode;
}

static in_acc=0;

BOOL CAvp32App::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_TAB){
		if(0x80&GetKeyState(VK_CONTROL)){
			m_pMainWnd->PostMessage(WM_COMMAND,ID_AVP_CHANGETAB);
			return TRUE;
		}
	}
	if(!in_acc){
		in_acc++;
		int ret=TranslateAccelerator(m_pMainWnd->m_hWnd,acc,pMsg);
		in_acc--;
		if(ret)	return TRUE;
	}
	
	return CWinApp::PreTranslateMessage(pMsg);
}


