// KLGuard2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "KLGuard2.h"
#include "KLGuard2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//+ ------------------------------------------------------------------------------------------
CReestr *g_pReestr = NULL;
CProcessList* gpProcList = NULL;
//+ ------------------------------------------------------------------------------------------
#ifndef BIF_NEWDIALOGSTYLE
	#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
#endif

#ifndef BIF_NONEWFOLDERBUTTON 
	#define BIF_NONEWFOLDERBUTTON  0x0200   // Do not add the "New Folder" button to the dialog.  Only applicable with BIF_NEWDIALOGSTYLE.
#endif BIF_NONEWFOLDERBUTTON

BOOL BrowseForFolder(HWND hWnd, CString& strTitle, CString& strResult, BOOL bIncludeFiles, _tBrowseCallbackProc pCallback)
{
	BOOL bRet = FALSE;
    WCHAR szFolder[MAX_PATH * 2];
	
	ZeroMemory(szFolder, sizeof(szFolder));
	
    BROWSEINFO info;
    LPSHELLFOLDER shell;
    LPITEMIDLIST FolderID;
	
    SHGetDesktopFolder(&shell);
	
    info.hwndOwner = hWnd;
    info.pidlRoot = NULL;
    info.pszDisplayName = szFolder;
    info.lpszTitle = strTitle.GetBuffer(MAX_PATH);
    info.ulFlags = BIF_NEWDIALOGSTYLE;
	if (bIncludeFiles)
		info.ulFlags |= BIF_BROWSEINCLUDEFILES | BIF_NONEWFOLDERBUTTON;
    info.lpfn = pCallback;
	
    FolderID = SHBrowseForFolder(&info);
	
	if(FolderID != NULL)
	{
		if(SHGetPathFromIDList(FolderID, szFolder))
		{
			strResult= szFolder;
			bRet = TRUE;
		}
	}
	
    shell->Release();

	strTitle.ReleaseBuffer();
	
	return bRet;
}


BOOL TrayMessage(HWND hDlg, UINT uFlags, DWORD dwMessage, UINT uID, HICON hIcon, TCHAR *pszTip)
{
	BOOL res;
	NOTIFYICONDATA tnd;
	
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd=hDlg;
	tnd.uID = uID;
	tnd.uFlags=uFlags;
	tnd.uCallbackMessage = WM_NOTIFYICON;
	tnd.hIcon = hIcon;
	if(pszTip)
		lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip) / sizeof(TCHAR));
	else
		tnd.szTip[0] = '\0';
	res = Shell_NotifyIcon(dwMessage, &tnd);
	
	if(hIcon)
		DestroyIcon(hIcon);
	
	return res;
}

void ModifyTrayIcon(HWND hWnd, DWORD idi)
{
	TrayMessage(hWnd, NIF_ICON, NIM_MODIFY, _TRAY_ICON_ID, (HICON)LoadImage(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(idi), IMAGE_ICON, 16, 16, 0), _APP_NAME);
	SetClassLong(hWnd, GCL_HICON, (LONG)LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(idi)));
}


/////////////////////////////////////////////////////////////////////////////
// CKLGuard2App

BEGIN_MESSAGE_MAP(CKLGuard2App, CWinApp)
	//{{AFX_MSG_MAP(CKLGuard2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKLGuard2App construction

CKLGuard2App::CKLGuard2App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKLGuard2App object

CKLGuard2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CKLGuard2App initialization

BOOL CKLGuard2App::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CoInitialize(NULL);

	OSVERSIONINFO OsInfo;
	OsInfo.dwOSVersionInfoSize = sizeof(OsInfo);
	GetVersionEx(&OsInfo);

	if (OsInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		TRACE0("Wrong operation system. NT required.");
		return FALSE;
	}

	g_pReestr = new CReestr(_AVP_GUARD_V2);
	if (g_pReestr == NULL)
	{
		TRACE0("Can't create registry class.");
		return FALSE;
	}

	gpProcList = new CProcessList;
	if (gpProcList == NULL)
	{
		TRACE0("Can't create process list.");
		return FALSE;
	}


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CKLGuard2Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CKLGuard2App::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (gpProcList != NULL)
	{
		delete gpProcList;
		gpProcList = NULL;
	}

	if (g_pReestr != NULL)
	{
		delete g_pReestr;
		g_pReestr = NULL;
	}

	CoUninitialize();
	
	return CWinApp::ExitInstance();
}
