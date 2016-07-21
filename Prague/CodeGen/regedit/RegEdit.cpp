//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#define PR_IMPEX_DEF
#include "stdafx.h"
#include "RegEdit.h"

#include "MainFrm.h"
#include "RegEditDoc.h"
#include "LeftView.h"
#include "PersistentData.h"

#include <pr_loadr.h>
#include "iface/e_loader.h"
#include "iface/e_ktrace.h"

#include "plugin\p_win32_reg.h"
#include "plugin\p_dtreg.h"



//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

BEGIN_MESSAGE_MAP(CRegEditApp, CWinApp)
	//{{AFX_MSG_MAP(CRegEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW_KEY, OnNewKey)
END_MESSAGE_MAP()

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(imp_loader)
  #include "iface/e_loader.h"
  #include "iface/e_ktrace.h"
IMPORT_TABLE_END

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditApp construction

void AppGetModulePath(TCHAR* strBuf, DWORD nChars, HMODULE i_hModule)
{
	strBuf[0] = 0;
	TCHAR strModuleName[4000] = {0};

	if(!GetModuleFileName(i_hModule, strModuleName, sizeof(strModuleName)))
		return;
	for(int i=lstrlen(strModuleName);i>=0;i--) 
		if(strModuleName[i]=='\\') 
		{
			strModuleName[i] = 0;
			lstrcpyn(strBuf, strModuleName, nChars);
			return;
		}
}

CRegEditApp::CRegEditApp() : m_bFile(false)
{
	m_pPragueRoot = NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

CRegEditApp theApp;
CRegEditApp* GlobalGetApp() { return &theApp; }

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CString CRegEditApp::MakePragueLoaderParhname(LPCTSTR i_strPath)
{
	CString strRet(i_strPath);
	LPTSTR strBuf = strRet.GetBufferSetLength(1024);

	lstrcat( strBuf, _T("\\") );
	lstrcat( strBuf, PR_LOADER_MODULE_NAME );

	strRet.ReleaseBuffer();
	return strRet;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditApp::PragueLoad(LPCTSTR i_strPathname)
{
	ASSERT(NULL == m_pPragueRoot &&	NULL == m_hPragueLoader);

	hROOT* hPragueRoot = (hROOT*) & m_pPragueRoot;
	CString	strMsg;

	m_hPragueLoader = LoadLibrary( i_strPathname );

	if ( NULL == m_hPragueLoader ) 
	{
		strMsg.Format(_T("Error while loading Prague loader at '%s'!"), i_strPathname);
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}

	PragueLoadFuncEx plf = (PragueLoadFuncEx) GetProcAddress(m_hPragueLoader,PR_LOADER_LOAD_FUNC);

	if ( NULL == plf) 
	{
		strMsg.Format(_T("Loader doesn't export init function '%s'!"), i_strPathname);
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}

	tERROR nError = plf(hPragueRoot, 
			(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH | PR_LOADER_FLAG_USE_TRY_CATCH ),
			NULL, 0, 0);

	if ( PR_FAIL(nError) || NULL == m_pPragueRoot )
	{
		ReportPragueError(_T("Cannot initialize ROOT object."), nError);
		return FALSE;
	}
	
	nError = CALL_Root_ResolveImportTable(*hPragueRoot, NULL, imp_loader, PID_APPLICATION);

	if ( PR_FAIL(nError))
	{
		strMsg.Format(_T("Cannot import loader exports!"));
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}
				
	return TRUE;
} 

// ---
void CRegEditApp::PragueUnload() 
{
	hROOT hPragueRoot = (hROOT) m_pPragueRoot;

	if ( hPragueRoot ) 
	{
		if(m_hPragueLoader)
		{
			PragueUnloadFunc  puf = (PragueUnloadFunc)GetProcAddress(m_hPragueLoader, PR_LOADER_UNLOAD_FUNC);
			if ( puf )
				puf( hPragueRoot );
		}
		else
			CALL_SYS_ObjectClose( hPragueRoot );
	}
	
	if ( m_hPragueLoader )
		FreeLibrary( m_hPragueLoader );
	
	m_hPragueLoader = NULL;
	m_pPragueRoot = NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
cRegistry* CRegEditApp::OpenRegistry(LPCTSTR i_strRegKey, BOOL& o_bWinReg, BOOL& o_bReadOnly)
{
	if( !i_strRegKey || !*i_strRegKey )
		return NULL;

	//-------------------------------------------------------------//
	// find out wheather it is a Registry or Datatree format

  o_bReadOnly = FALSE;
	o_bWinReg = !m_bFile;

	//-------------------------------------------------------------//
	cRegistry* pRetVal = NULL;

	UINT uiImplemebation	= o_bWinReg ? PID_WIN32_REG : PID_DTREG;
	UINT uiPropId         = o_bWinReg ? pgROOT_POINT : pgOBJECT_NAME;

	tERROR nErr;

	tBOOL bReadOnly = cFALSE;
	if( FALSE == o_bWinReg ) {
		DWORD attr = GetFileAttributes( i_strRegKey );
		if ( attr == -1 ) {
			char msg[0x100+MAX_PATH];
			DWORD werr = GetLastError();
			if ( (werr == ERROR_FILE_NOT_FOUND) || (werr == ERROR_PATH_NOT_FOUND) )
				wsprintf( msg, _T("There is no file specified:\n%s"), i_strRegKey );
			else
				wsprintf( msg, _T("Cannot open file specified:\n%s"), i_strRegKey );
			AfxMessageBox( msg, MB_ICONINFORMATION );
			return 0;
		}
		if( attr & FILE_ATTRIBUTE_READONLY ) {
			bReadOnly = cTRUE;
			o_bReadOnly = TRUE;
			AfxMessageBox(_T("File specified has read only attribute. Open it for reading!"), MB_ICONINFORMATION);
		}
	}

	nErr = m_pPragueRoot->sysCreateObject( (hOBJECT*)&pRetVal, IID_REGISTRY, uiImplemebation );  
	if( PR_FAIL(nErr) ) {
		ReportPragueError(_T("Cannot create registry object."), nErr);
		return NULL;
	}

	nErr = pRetVal->propSetStr( 0, uiPropId, (void*)i_strRegKey, lstrlen(i_strRegKey) + 1);  
	if( PR_FAIL(nErr) ) {
		ReportPragueError(_T("Cannot set property value."), nErr);
		return NULL;
	}

	nErr = pRetVal->propSetBool( pgOBJECT_RO, bReadOnly);  
	if( PR_FAIL(nErr) ) {
		ReportPragueError(_T("Cannot set pgOBJECT_RO property value."), nErr);
		return NULL;
	}

	nErr = pRetVal->sysCreateObjectDone();   
	if( PR_FAIL(nErr) ) {
		ReportPragueError(_T("Create object done failed."), nErr);
		return NULL;
	}

	return pRetVal;
}						  



//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditApp::ReportPragueError(LPCTSTR i_strMsg, tERROR i_nErr)
{
	CString strMsg;
	if ( pr_sprintf ) 
	{
		pr_sprintf( strMsg.GetBufferSetLength(1024), 1024, _T("%s Error code: 0x%08x (%terr)."), 
			i_strMsg, (int)i_nErr, (int)i_nErr );
		strMsg.ReleaseBuffer();
	}
	else
		strMsg.Format(_T("%s Error code: 0x%08x (%d)."), i_strMsg, (int)i_nErr, (int)i_nErr);
	AfxMessageBox(strMsg, MB_ICONSTOP);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void	CRegEditApp::LoadParams()
{
	m_arrPersistentData.SetSize(DATA_CODE_MAX);

	for(int i=0; i<DATA_CODE_MAX; i++)
	{
		CString strItem;
		strItem.Format(_T("Item%d"), i);
		CString strVal = GetProfileString(_T("Settings"), strItem, 0);
		if(strVal.IsEmpty()) continue;
		m_arrPersistentData[i] = strVal;
	}
}

// --- 
void	CRegEditApp::SaveParams()
{
	for(int i=0; i<DATA_CODE_MAX; i++)
	{
		if(m_arrPersistentData[i].IsEmpty()) continue;
		CString strItem;
		strItem.Format(_T("Item%d"), i);
		WriteProfileString(_T("Settings"), strItem, m_arrPersistentData[i]);
	}
}

// ---
CString CRegEditApp::GetPersistentData(int i_nDataCode)
{
	ASSERT(DATA_CODE_MAX > i_nDataCode);
	return m_arrPersistentData[i_nDataCode];
}

// ---
void CRegEditApp::SetPersistentData(int i_nDataCode, CString& i_strData)
{
	ASSERT(DATA_CODE_MAX > i_nDataCode);
	m_arrPersistentData[i_nDataCode] = i_strData;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
UINT CRegEditApp::AppGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
	ASSERT(lpszTitle == NULL ||
		AfxIsValidAddress(lpszTitle, _MAX_FNAME));
	ASSERT(AfxIsValidString(lpszPathName));

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = (LPTSTR)lpszPathName;
	for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// remember last directory/drive separator
		if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
			lpszTemp = (LPTSTR)_tcsinc(lpsz);
	}

	// lpszTitle can be NULL which just returns the number of bytes
	if (lpszTitle == NULL)
		return lstrlen(lpszTemp)+1;

	// otherwise copy it into the buffer provided
	lstrcpyn(lpszTitle, lpszTemp, nMax);
	return 0;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// lpszCanon = C:\MYAPP\DEBUGS\C\TESWIN.C
//
// cchMax   b   Result
// ------   -   ---------
//  1- 7    F   <empty>
//  1- 7    T   TESWIN.C
//  8-14    x   TESWIN.C
// 15-16    x   C:\...\TESWIN.C
// 17-23    x   C:\...\C\TESWIN.C
// 24-25    x   C:\...\DEBUGS\C\TESWIN.C
// 26+      x   C:\MYAPP\DEBUGS\C\TESWIN.C

void CRegEditApp::AppAbbreviateName(LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName)
{
	int cchFullPath, cchFileName, cchVolName;
	const TCHAR* lpszCur;
	const TCHAR* lpszBase;
	const TCHAR* lpszFileName;

	lpszBase = lpszCanon;
	cchFullPath = lstrlen(lpszCanon);

	cchFileName = AppGetFileName(lpszCanon, NULL, 0) - 1;
	lpszFileName = lpszBase + (cchFullPath-cchFileName);

	// If cchMax is more than enough to hold the full path name, we're done.
	// This is probably a pretty common case, so we'll put it first.
	if (cchMax >= cchFullPath)
		return;

	// If cchMax isn't enough to hold at least the basename, we're done
	if (cchMax < cchFileName)
	{
		lstrcpy(lpszCanon, (bAtLeastName) ? lpszFileName : ""/*&afxChNil*/);
		return;
	}

	// Calculate the length of the volume name.  Normally, this is two characters
	// (e.g., "C:", "D:", etc.), but for a UNC name, it could be more (e.g.,
	// "\\server\share").
	//
	// If cchMax isn't enough to hold at least <volume_name>\...\<base_name>, the
	// result is the base filename.

	lpszCur = lpszBase + 2;                 // Skip "C:" or leading "\\"

	if (lpszBase[0] == '\\' && lpszBase[1] == '\\') // UNC pathname
	{
		// First skip to the '\' between the server name and the share name,
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
	}
	// if a UNC get the share name, if a drive get at least one directory
	ASSERT(*lpszCur == '\\');
	// make sure there is another directory, not just c:\filename.ext
	if (cchFullPath - cchFileName > 3)
	{
		lpszCur = _tcsinc(lpszCur);
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
	}
	ASSERT(*lpszCur == '\\');

	cchVolName = lpszCur - lpszBase;
	if (cchMax < cchVolName + 5 + cchFileName)
	{
		lstrcpy(lpszCanon, lpszFileName);
		return;
	}

	// Now loop through the remaining directory components until something
	// of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
	//
	// Assert that the whole filename doesn't fit -- this should have been
	// handled earlier.

	ASSERT(cchVolName + (int)lstrlen(lpszCur) > cchMax);
	while (cchVolName + 4 + (int)lstrlen(lpszCur) > cchMax)
	{
		do
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
		while (*lpszCur != '\\');
	}

	// Form the resultant string and we're done.
	lpszCanon[cchVolName] = '\0';
	lstrcat(lpszCanon, _T("\\..."));
	lstrcat(lpszCanon, lpszCur);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#define OBJECT_EXISTS(strPathname) (0xFFFFFFFF!=GetFileAttributes(strPathname))
void CRegEditApp::CheckInputParameter(CString& io_strKey)
{
	if(io_strKey.IsEmpty())
		return;

	// try to set up current dir at the beginning
	if(!OBJECT_EXISTS(io_strKey))
	{
		CString strCurDir;
		GetCurrentDirectory(1024, strCurDir.GetBufferSetLength(1024));
		strCurDir.ReleaseBuffer();
		strCurDir += _T("\\") + io_strKey;
		if(OBJECT_EXISTS(strCurDir))
			io_strKey = strCurDir;
	}
}	

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CDocument* CRegEditApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	m_bFile = true;
	CDocument* pDoc = CWinApp::OpenDocumentFile(lpszFileName);
	((CMainFrame*)m_pMainWnd)->OnUpdateFrameTitle(TRUE);
	return pDoc;
}


class CMyCLI : public CCommandLineInfo {
public:
	bool m_file;
	CMyCLI() : CCommandLineInfo(), m_file(false) {}
protected:
	virtual void ParseParam( const TCHAR* pszParam, BOOL bFlag, BOOL bLast ) {
		if ( bFlag && !lstrcmp(pszParam,"f") )
			m_file = true;
		else if ( bFlag && !lstrcmp(pszParam,"r") )
			m_file = false;
		else
			CCommandLineInfo::ParseParam( pszParam, bFlag, bLast );
	}
};


// ---
bool is_reg_key( const CString& str ) {
	if ( str.GetLength() < 2 )
		return false;
	char fi = str.GetAt(0);
	char se = str.GetAt(1);
	if ( fi != '\\' )
		return false;
	if ( se == '\\' )
		return false;
	int pos = str.Find( '\\', 1 );
	if ( pos == -1 )
		pos = str.Find( '/', 1 );
	if ( pos == -1 )
		return false;
	CString root = str.Mid( 1, pos-1 );
	if ( !root.CompareNoCase("hklm") )
		return true;
	if ( !root.CompareNoCase("hkcu") )
		return true;
	if ( !root.CompareNoCase("HKEY_CURRENT_USER") )
		return true;
	if ( !root.CompareNoCase("HKEY_CLASSES_ROOT") )
		return true;
	if ( !root.CompareNoCase("HKEY_USERS") )
		return true;
	if ( !root.CompareNoCase("HKEY_CURRENT_CONFIG") )
		return true;
	if ( !root.CompareNoCase("HKEY_DYNDATA") )
		return true;
	return false;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditApp::InitInstance()
{
	// -------------------------------------------------------------//
	AfxEnableControlContainer();

#ifdef _AFXDLL
	//Enable3dControls();			
#else
	//Enable3dControlsStatic();	
#endif
	
	// -------------------------------------------------------------//
	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("KasperskyLab"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	LoadParams();

	// -------------------------------------------------------------//
	// Register document templates

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRegEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	// -------------------------------------------------------------//
	CString strPraguePath; // = GetPersistentData(DATA_CODE_PRAGUE_PATH);
	if(strPraguePath.IsEmpty())
	{
		//m_strPraguePath = _T("o:\\Prague\\CodeGen\\RegEdit\\Pr");
		AppGetModulePath(strPraguePath.GetBufferSetLength(1024), 1024, NULL);
		strPraguePath.ReleaseBuffer();
		SetPersistentData(DATA_CODE_PRAGUE_PATH, strPraguePath);
	}

	CString strLoaderPathname = MakePragueLoaderParhname(strPraguePath);
	if(!PragueLoad(strLoaderPathname))
		return FALSE;

	// -------------------------------------------------------------//
	CMyCLI cmdInfo;
	ParseCommandLine(cmdInfo);

	m_bFile = cmdInfo.m_file;
	if ( !m_bFile && !is_reg_key(cmdInfo.m_strFileName) )
		m_bFile = TRUE;

	// -------------------------------------------------------------//
	CheckInputParameter(cmdInfo.m_strFileName);
	if(!pDocTemplate->OpenDocumentFile(cmdInfo.m_strFileName, FALSE))
		return FALSE;

	((CMainFrame*)m_pMainWnd)->OnUpdateFrameTitle(TRUE);
	((CMainFrame*)m_pMainWnd)->RestorePlacement (m_nCmdShow);
	m_pMainWnd->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

// ---
int CRegEditApp::ExitInstance() 
{
	SaveParams();
	PragueUnload();
	return CWinApp::ExitInstance();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRegEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditApp message handlers

void CRegEditApp::OnFileNew()
{
	AfxMessageBox(_T("Not implemented yet!"));
}

void CRegEditApp::OnFileOpen()
{
	CWinApp::OnFileOpen();
	((CMainFrame*)m_pMainWnd)->OnUpdateFrameTitle(TRUE);
}

void CRegEditApp::OnNewKey()
{
	((CMainFrame*)m_pMainWnd)->OnNewKey();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

