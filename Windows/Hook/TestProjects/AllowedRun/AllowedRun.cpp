// AllowedRun.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AllowedRun.h"
#include "AllowedRunDlg.h"
#include "DChanges.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//+ ----------------------------------------------------------------------------------------
BOOL GetCmdLineParam(CString& sCommandLine, CString& sParamName, CString& sValue)
{
	BOOL exist = FALSE;

	CString sParam;
	
	sValue.Empty();

	int n = sCommandLine.Find(sParamName);
	if (n != -1)
	{
		exist = TRUE;

		sParam = sCommandLine.Mid(n);
		n = sParam.Find(_T("\""));
		if (n != -1)	// Name enclosed in '\'
		{
			sParam = sParam.Mid(n + 1);
			n = sParam.Find(_T("\""));
			if (n != -1)
				sValue = sParam.Left(n);
		}
		else			// Name 'as is'
		{
			n = sParam.Find(_T("="));
			if (n != -1)
			{
				sParam = sParam.Mid(n + 1);
				n = sParam.Find(_T(" "));
				if (n != -1)
					sValue = sParam.Left(n);
			}
		}
	}
	

	return exist;
}

//+ ----------------------------------------------------------------------------------------

CFiltersStorage* gpFilters = NULL;

void AddAppFilter(CString& strApp)
{
	DWORD dwSite = FLTCTL_ADD2TOP;
	char szProcess[PROCNAMELEN];

	PWCHAR pwsapp = strApp.GetBuffer(PROCNAMELEN);

	if (!WideCharToMultiByte(CP_ACP, 0, pwsapp, -1, (LPSTR) szProcess, sizeof(szProcess), NULL, NULL))
	{
		strApp.ReleaseBuffer();
		return;
	}
	
	strApp.ReleaseBuffer();


	CWnd* pWnd = AfxGetMainWnd();
	HWND hWnd = NULL;
	if (pWnd != NULL)
		hWnd = pWnd->m_hWnd;

	if (!AddFSFilterW(gpFilters->m_hDevice, gpFilters->m_AppID, _T("*"), szProcess, 0, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, &dwSite))
		::MessageBox(hWnd, _T("can't set filter for this application"), _T("Warning"), MB_ICONWARNING);
	else
		gpFilters->m_bIsModified = true;
}

void AddFolderFilter(CString& strFolder)
{
	if (strFolder.Right(1) != _T("*"))
		strFolder += _T("*");

	CString strtmp = strFolder;
	PWCHAR buffer = strFolder.GetBuffer(1024*32);
	ReparsePath(buffer, 1024*32);

	CWnd* pWnd = AfxGetMainWnd();
	HWND hWnd = NULL;
	if (pWnd != NULL)
		hWnd = pWnd->m_hWnd;

	if (strFolder != strtmp)
	{
		CDChanges DChanges(&strFolder, &strtmp, pWnd);
		DChanges.DoModal();
	}

	if (!AddFSFilterW(gpFilters->m_hDevice, gpFilters->m_AppID, buffer, "*", 0, FLT_A_DENY, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL))
		::MessageBox(hWnd, _T("can't set filter (deny access) for this folder"), _T("Warning"), MB_ICONWARNING);
	else
	{
		// filter on launch
		LPBYTE pBuf = (LPBYTE) HeapAlloc(GetProcessHeap(), 0, sizeof(FILTER_PARAM) + 1024*32);
		if (pBuf == NULL)
			::MessageBox(hWnd, _T("can't allocate memory"), _T("Warning"), MB_ICONWARNING);
		else
		{
			PFILTER_PARAM Param = (PFILTER_PARAM) pBuf;

			Param->m_ParamID = _PARAM_OBJECT_URL_W;				
			Param->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
			Param->m_ParamFlt = FLT_PARAM_WILDCARD;
			lstrcpy((WCHAR*) Param->m_ParamValue, buffer);
			Param->m_ParamSize = (lstrlen((WCHAR*) Param->m_ParamValue) + 1) * sizeof(WCHAR);
			
			if (!AddFSFilter2(gpFilters->m_hDevice, gpFilters->m_AppID, "*", 0, 
				FLT_A_DENY, FLTTYPE_SYSTEM, MJ_SYSTEM_CREATE_SECTION, 0, 0, PreProcessing, NULL, Param, NULL))
			{
				::MessageBox(hWnd, _T("can't set filter (deny launch) for this folder"), _T("Warning"), MB_ICONWARNING);
			}
			else
				gpFilters->m_bIsModified = true;

			HeapFree(GetProcessHeap(), 0, pBuf);
		}		
	}
	strFolder.ReleaseBuffer();
}

void SaveFilters()
{
	CString FilterDescription;
	int Count = gpFilters->m_FiltersArray.GetSize();

	int nFolder = 0;
	int nApp = 0;

	WritePrivateProfileSection(_Section_Apps, NULL, _IniFileName);
	WritePrivateProfileSection(_Section_Folders, NULL, _IniFileName);

	if (Count == 0)
		return;

	CString ParamName;
	PFILTER_TRANSMIT pFilter;
	PFILTER_PARAM pParam;
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = gpFilters->m_FiltersArray.ElementAt(cou);
		if (pFilter == NULL)
			continue;
		
		pParam = IDriverGetFilterParam(pFilter, _PARAM_OBJECT_URL_W);
		if (pParam == NULL)
			continue;

		FilterDescription = (PWCHAR) pParam->m_ParamValue;
		
		if (FilterDescription == _T("*"))
		{
			//! Apps filter
			ParamName.Format(_T("A%d"), nApp++);
			FilterDescription = pFilter->m_ProcName;
			WritePrivateProfileString(_Section_Apps, ParamName, FilterDescription, _IniFileName);
		}
		else
		{
			if (pFilter->m_HookID == FLTTYPE_SYSTEM)
			{
				//! save folder
				ParamName.Format(_T("F%d"), nFolder++);
				WritePrivateProfileString(_Section_Folders, ParamName, FilterDescription, _IniFileName);
			}
		}
	}
}

void LoadFilters()
{
	CString ParamValue;
	PWCHAR wc = ParamValue.GetBuffer(32*1024);
	DWORD size;

	CString strtmp;
	PWCHAR pwsz;

	gpFilters->ReloadFiltersArray();
	int Count = gpFilters->m_FiltersArray.GetSize();
	if (Count != 0)
	{
		PFILTER_TRANSMIT pFilter;
		for (int cou = 0; cou < Count; cou++)
		{
			pFilter = gpFilters->m_FiltersArray.ElementAt(cou);
			if (pFilter == NULL)
				continue;

			gpFilters->DeleteFilter(pFilter, true);
		}
	}

	int pos;

	size = 32*1024;
	if ((size = GetPrivateProfileSection(_Section_Apps, wc, size, _IniFileName)) != 0)
	{
		pwsz = wc;
		while (TRUE)
		{
			strtmp = pwsz;
			if (strtmp.IsEmpty())
				break;

			pos = strtmp.Find(_T("="));
			if (pos != -1)
				AddAppFilter(strtmp.Mid(pos + 1));

			pwsz += (strtmp.GetLength() + 1);
		}
	}

	size = 32*1024;
	if ((size = GetPrivateProfileSection(_Section_Folders, wc, size, _IniFileName)) != 0)
	{
		pwsz = wc;
		while (TRUE)
		{
			strtmp = pwsz;
			if (strtmp.IsEmpty())
				break;

			pos = strtmp.Find(_T("="));
			if (pos != -1)
				AddFolderFilter(strtmp.Mid(pos + 1));

			pwsz += (strtmp.GetLength() + 1);
		}
	}
}

//+ ----------------------------------------------------------------------------------------

HANDLE g_hHeap;
long g_HeapUsage4 = 0;

void* _MM_Alloc(unsigned int nSize)
{
	InterlockedIncrement(&g_HeapUsage4);
	return HeapAlloc(g_hHeap, 0, nSize);
}

void _MM_Free(void* p)
{
	if (p == NULL)
	{
/*#ifdef _DEBUG
		OutputDebugString("Try to free memory with NULL pointer\n");
#endif*/
		return;
	}
	InterlockedDecrement(&g_HeapUsage4);
	HeapFree(g_hHeap, 0, p);
}

int _MM_Compare(void* p1, void* p2, int len)
{
	int ret = p1 == p2 ? 0 : 1;
	return ret;
}

#define UnicodePrefix _T("\\??\\")
#define UnicodePrefixLen 4

void ReparsePath(WCHAR* path, DWORD pathlen)
{
	WCHAR szPath[MAX_PATH * 2];

	if (pathlen < 3 || path[1] != _T(':') || path[2] != _T('\\')) // no drive letter
		return;

	DWORD dwDeviceNameLen;
	DWORD dwPathLen;

	path[2] = 0;
	dwDeviceNameLen = QueryDosDevice(path, szPath, sizeof(szPath) / sizeof(szPath[0]));
	path[2] = _T('\\');
	
	if (memcmp(szPath, _T("\\Device"), sizeof(_T("\\Device")) - sizeof(TCHAR)) == 0)
		return;

	if (dwDeviceNameLen)
	{
		WCHAR* newpath = szPath;
		dwPathLen = lstrlen(path+3);
		
		if(dwDeviceNameLen + dwPathLen + 1 <= sizeof(szPath)/sizeof(szPath[0]))
		{
			lstrcat(newpath, _T("\\"));
			lstrcat(newpath, path + 3);
		}
		
		if (memcmp(newpath, UnicodePrefix, UnicodePrefixLen) == 0)
		{
			newpath += UnicodePrefixLen;
			dwDeviceNameLen -= UnicodePrefixLen;
		}
		
		if(dwDeviceNameLen + dwPathLen + 1 <= pathlen)
			lstrcpy(path, newpath);
	}
}


BOOL BrowseForFolder(HWND hWnd, CString& strTitle, CString& strResult)
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
    info.ulFlags = 0;
    info.lpfn = NULL;
	
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

BOOL ExistNameInListCtrl(CListCtrl* pList, CString* pStrObject)
{
	CString strtmp;
	int Count = pList->GetItemCount();
	PWCHAR pws = strtmp.GetBuffer(32*1024);
	for (int cou = 0; cou < Count; cou++)
	{
		if (pList->GetItemText(cou, 0, pws, 32*1024) != 0)
		{
			if (strtmp == *pStrObject)
			{
				strtmp.ReleaseBuffer();
				return TRUE;
			}
		}
		
	}
	strtmp.ReleaseBuffer();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CAllowedRunApp

BEGIN_MESSAGE_MAP(CAllowedRunApp, CWinApp)
	//{{AFX_MSG_MAP(CAllowedRunApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllowedRunApp construction

CAllowedRunApp::CAllowedRunApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAllowedRunApp object

CAllowedRunApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAllowedRunApp initialization

BOOL CAllowedRunApp::InitInstance()
{
	g_hHeap = GetProcessHeap();
	if (FSDrvInterceptorInitEx(_CLIENT_FLAG_WITHOUTWATCHDOG, VirLabProtection, AVPG_INVISIBLEPRIORITY, 0) == FALSE)
	{
		::MessageBox(NULL, _T("Can't connect to interceptor (klif)"), _T("System error!"), MB_ICONERROR);
		return -1;

	}

	gpFilters = new CFiltersStorage(FSDrvGetDeviceHandle(), FSDrvGetAppID());
	if (gpFilters == NULL)
	{
		::MessageBox(NULL, _T("Internal init failed!"), _T("System error!"), MB_ICONERROR);
		return -1;
	}


	//+ ----------------------------------------------------------------------------------------
	PWCHAR cmd = GetCommandLine();
	if (cmd != NULL)
	{
		CString szCommandLine = cmd;
		CString autostart = _T("/autostart");
		CString value;
		if (GetCmdLineParam(szCommandLine, autostart, value))
		{
			LoadFilters();
			return TRUE;
		}
	}
	//- ----------------------------------------------------------------------------------------
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	FSDrvInterceptorSetActive(TRUE);

	CAllowedRunDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	if (gpFilters->m_bIsModified)
	{
		UINT bSave = GetProfileInt(_Section_Actions, _Ini_SaveFolters, 1);

		if (bSave)
			SaveFilters();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CAllowedRunApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (gpFilters != NULL)
	{
		delete gpFilters;
		gpFilters = NULL;
	}

	FSDrvInterceptorDone();	
	return CWinApp::ExitInstance();
}
