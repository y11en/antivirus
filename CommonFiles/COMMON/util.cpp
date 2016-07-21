#include "stdafx.h"
#include "util.h"
#include "resource.h"
#include <winsvc.h>
#include "common.h"
#include "ChooseObjectDialog.h"
#include "../../Windows/Personal 4.6/exenames.h"
#include "../../Windows/Personal 4.6/release_type.h"
#include "../KLUtil/UnicodeConv.h"
#include "../StuffIO/Ioutil.h"
#include <math.h>

#ifdef __WKS
	const TCHAR szAutoRunKeyName[] = _T("KAVWks50");
	const TCHAR szAutoRunCommandLine9x[] = _T("/minimize /chkas");
	const TCHAR szAutoRunCommandLineNt[] = _T("/minimize /chkas");
#else
	const TCHAR szAutoRunKeyName[] = _T("KAVPersonal50");
	const TCHAR szAutoRunCommandLine9x[] = _T("/minimize");
	const TCHAR szAutoRunCommandLineNt[] = _T("/minimize");
#endif

CCFileDialogEx::CCFileDialogEx (BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
								DWORD dwFlags, LPCTSTR lpszFilter, CWnd * pParentWnd) : 
	CCFileDialog (bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

void CCFileDialogEx::OnInitDone()
{
	SetControlText (IDOK, CString ((LPCTSTR)IDS_COMMON_BUTTON_CHOOSE_STR));
	SetControlText (IDCANCEL, CString ((LPCTSTR)IDS_COMMON_BUTTON_CANCEL_STR));
	SetControlText (stc2, CString ((LPCTSTR)IDS_COMMON_STATIC_FILE_TYPE_STR));
	SetControlText (stc3, CString ((LPCTSTR)IDS_COMMON_STATIC_FILE_NAME_STR));
	SetControlText (stc4, CString ((LPCTSTR)IDS_COMMON_STATIC_LOOK_IN_STR));

	CWnd *pParent = GetParent (), *pCtrl;
	
	if (pParent)
	{
		pCtrl = pParent->GetDlgItem (IDOK);
		if (pCtrl)
			pCtrl->ModifyStyle (0, BS_FLAT);

		pCtrl = pParent->GetDlgItem (IDCANCEL);
		if (pCtrl)
			pCtrl->ModifyStyle (0, BS_FLAT);
	}
}

void BrowseFile ( CWnd * pParentWnd, CString & strText, LPCTSTR pszInitialDir, DWORD dwFlags, LPCTSTR szTitle )
{
    CString strFilesMasks, strMasks;
    strFilesMasks.LoadString ( IDS_COM_ALL_FILE_MASK );
    strMasks.LoadString ( IDS_COM_ALL_MASKS );
    
    dwFlags &= ~OFN_ALLOWMULTISELECT;

    CCFileDialogEx dlg ( TRUE, NULL, strMasks, dwFlags, strFilesMasks, pParentWnd );
    dlg.m_ofn.lpstrTitle = szTitle;
    dlg.m_ofn.lpstrInitialDir = pszInitialDir;
    
    if  ( dlg.DoModal () == IDOK )
    {
        strText = dlg.GetPathName ();
    }
}

BOOL BrowseSaveFile ( CWnd * pParentWnd, CString & strText, DWORD dwFlags, LPCTSTR szTitle )
{
    CString strFilesMasks ( ( LPCTSTR ) IDS_COM_ALL_FILE_MASK );
    
    dwFlags &= ~OFN_ALLOWMULTISELECT;

	//TCHAR path[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	//_tsplitpath ( strText, drive, dir, fname, ext );
	//_tmakepath ( path, drive, dir, NULL, NULL );
    
    CCFileDialogEx dlg ( FALSE, NULL, strText, dwFlags, strFilesMasks, pParentWnd );
    dlg.m_ofn.lpstrTitle = szTitle;
	//dlg.m_ofn.lpstrInitialDir = path;
    
    if  ( dlg.DoModal () == IDOK )
    {
        strText = dlg.GetPathName ();
		return TRUE;
    }

	return FALSE;
}

void BrowseFileMulti (CWnd * pParentWnd,
						std::vector <std::string> &strFiles,
						DWORD dwFlags,
						LPCTSTR szTitle,
						LPCTSTR szInitialDir)
{
	CString strFilesMasks, strMasks;
//	CString strFileNames;
	strFilesMasks.LoadString ( IDS_COM_ALL_FILE_MASK );
//	strFileNames.LoadString ( IDS_COM_ALL_FILES );
	strMasks.LoadString ( IDS_COM_ALL_MASKS );
	
	dwFlags |= OFN_ALLOWMULTISELECT;
	dwFlags |= OFN_EXPLORER;
///	dwFlags |= OFN_NOVALIDATE; // see Q99338
	
	CCFileDialogEx dlg ( TRUE, NULL, strMasks, dwFlags, strFilesMasks, pParentWnd );
	
	TCHAR chFileBuffer[2048] = { _T('\0') }; // 2048 is the max size (see Q99338)
	
	dlg.m_ofn.lpstrTitle = szTitle;
	dlg.m_ofn.lpstrFile = chFileBuffer;
	dlg.m_ofn.nMaxFile = 2048;
	dlg.m_ofn.lpstrInitialDir = szInitialDir;
	
	strFiles.clear ();
	
	if  ( dlg.DoModal () == IDOK )
	{
		POSITION pos = dlg.GetStartPosition ();
		while (pos)
			strFiles.push_back ((LPCTSTR)dlg.GetNextPathName (pos));
	}
}

DWORD GetDllVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;
	
    hinstDll = LoadLibrary(lpszDllName);
	
    if(hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;
		
        pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");
		
        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;
			
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
			
            hr = (*pDllGetVersion)(&dvi);
			
            if(SUCCEEDED(hr))
            {
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }
        
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

BOOL BrowseFolder (CWnd *pParentWnd, 
					CStdStringW &strText,
					UINT nHelpId,
					LPCTSTR szTitle /*= NULL*/, LPCTSTR szNote /*= NULL*/, 
					CInputRestriction *pRestriction /*= NULL*/,
					long nType /*= 0*/,
					CStdStringW &szInitialPath /*= NULL*/)
{
	if (GetDllVersion(_T("shell32.dll")) >= PACKVERSION(4, 71) ||
			(!(nType & COD_EDITBOX) && !(nType & COD_FILEANDFOLDERS)))
	{
		CCChooseObjectDialog dlg (nHelpId);

		dlg.m_szNote = szNote;
		dlg.m_szTitle = szTitle;
		dlg.m_Edit.SetRestriction (pRestriction);
		dlg.SetObjectPath (szInitialPath);

		if (dlg.Browse (pParentWnd, nType))
		{
			dlg.GetObjectPath (strText);
			return TRUE;
		}
	}
	else
	{
		CBrowseDirectoryNT4 dlg (nHelpId, pParentWnd, pRestriction, __LPCTSTR(szInitialPath));

//		dlg.m_ofn.lpstrFilter = " \0.\0\0";

		dlg.m_szNote = szNote,
		dlg.m_ofn.lpstrTitle = (LPSTR)szTitle;
		dlg.m_bInitialEmpty = (nType & COD_INITIALEMPTY) != 0;

		if (IOSGetFileAttributes (__LPCTSTR(szInitialPath)) != INVALID_FILE_ATTRIBUTES)
			dlg.m_ofn.lpstrInitialDir = __LPCTSTR(szInitialPath);

		if (dlg.DoModal () == IDOK)
		{
			strText = dlg.m_StrFolderName;
			return TRUE;
		}
	}

	return FALSE;
}

BYTE GetCharsetByCodePage (int CodePage)
{
	BYTE Charset = DEFAULT_CHARSET;

	switch(CodePage)
	{
	case 1252:
		Charset = ANSI_CHARSET;
		break;
	case 1251:
		Charset = RUSSIAN_CHARSET;
		break;
	case 1250:
		Charset = EASTEUROPE_CHARSET;
		break;
	case 1253:
		Charset = GREEK_CHARSET;
		break;
	case 1254:
		Charset = TURKISH_CHARSET;
		break;
	case 1257:
		Charset = BALTIC_CHARSET;
		break;
	case 1255:
		Charset = HEBREW_CHARSET;
		break;
	case 1256:
		Charset = ARABIC_CHARSET;
		break;
	case 932:
		Charset = SHIFTJIS_CHARSET;
		break;
	case 949:
		Charset = HANGEUL_CHARSET;
		break;
	case 936:
		Charset = GB2312_CHARSET;
		break;
	case 950:
		Charset = CHINESEBIG5_CHARSET;
		break;
	}

	return Charset;
}

bool CanUserStartServicePart ()
{
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx (&info) || info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		return true;

	SC_HANDLE hSCManager = OpenSCManager (NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
	bool bRetValue = (hSCManager != NULL);
	if (hSCManager)
		CloseServiceHandle (hSCManager);

	return bRetValue;
}

bool CanRunProcessWithLogon()
{
	// shoud be Win2k+
	OSVERSIONINFO info;
	ZeroMemory (&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	return GetVersionEx(&info) && info.dwPlatformId == VER_PLATFORM_WIN32_NT && info.dwMajorVersion >= 5;
}

static AutoStartProduct::SetResult AddRemoveAppPathToRegistry (bool bAdd,
																 LPCTSTR szRegKeyName,
																 LPCTSTR szValueName,
																 LPCTSTR szPath)
{
	AutoStartProduct::SetResult Result = AutoStartProduct::asGeneralFail;

	HKEY hRunKey = NULL;
	LONG nResult = ERROR_SUCCESS;

	if ((nResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
					szRegKeyName,
					0,
					KEY_ALL_ACCESS,
					&hRunKey)) != ERROR_SUCCESS)
	{
		if (nResult	== ERROR_ACCESS_DENIED)
			Result = AutoStartProduct::asAccessDenied;
	}
	else
	{
		if (bAdd)
		{
			if ((nResult = RegSetValueEx (hRunKey,
											szValueName,
											0,
											REG_SZ,
											(BYTE *)szPath,
											_tcslen (szPath) + sizeof (_T('\0')))) == ERROR_SUCCESS)
			{
				Result = AutoStartProduct::asOK;
			}
		}
		else
		{
			if ((nResult = RegDeleteValue (hRunKey, szValueName)) != ERROR_SUCCESS &&
					nResult != ERROR_FILE_NOT_FOUND)
			{
				if (nResult	== ERROR_ACCESS_DENIED)
					Result = AutoStartProduct::asAccessDenied;
				else
					Result = AutoStartProduct::asGeneralFail;
			}
			else
			{
				Result = AutoStartProduct::asOK;
			}
		}
	}

	if (hRunKey)
		RegCloseKey (hRunKey);
	
	return Result;					
}

static AutoStartProduct::SetResult SetServiceAutoStart (bool bAutoStart)
{
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx (&info))
		return AutoStartProduct::asGeneralFail;
		
	if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		TCHAR szKAVSVCPath[_MAX_PATH];
		TCHAR szKAVSVCCmd[_MAX_PATH + 3];

		if (!GetModuleFileName (NULL, szKAVSVCPath, _MAX_PATH))
			return AutoStartProduct::asGeneralFail;

		TCHAR *pLastSlash = _tcsrchr (szKAVSVCPath, _T('\\'));
		if (pLastSlash == NULL)
			pLastSlash = szKAVSVCPath;
		else
			++pLastSlash;


		_tcscpy (pLastSlash, KAV_EXENAME_SERVICE);
		_tcscat (szKAVSVCPath, _T(".exe"));

		if (szKAVSVCPath[0]!=_T('\"'))
		{
			_tcscpy(szKAVSVCCmd, _T("\""));
			_tcscat (szKAVSVCCmd, szKAVSVCPath);
			_tcscat(szKAVSVCCmd, _T("\""));
		}
		else
			_tcscpy (szKAVSVCCmd, szKAVSVCPath);


		return AddRemoveAppPathToRegistry (bAutoStart, 
									_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"),
									KAV_EXENAME_SERVICE,
									szKAVSVCCmd);
	}
	else
	{
		AutoStartProduct::SetResult Result = AutoStartProduct::asGeneralFail;

		SC_HANDLE hSCManager = OpenSCManager (NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
		SC_HANDLE hService = NULL;

		if (!hSCManager)
		{
			if (GetLastError () == ERROR_ACCESS_DENIED)
				Result = AutoStartProduct::asAccessDenied;
		}
		else
		{
			hService = OpenService (hSCManager, KAV_EXENAME_SERVICE, SERVICE_CHANGE_CONFIG);

			if (!hService)
			{
				if (!bAutoStart)
					Result = AutoStartProduct::asOK;
				else if (GetLastError () == ERROR_ACCESS_DENIED)
					Result = AutoStartProduct::asAccessDenied;
			}
			else
			{
				if (!ChangeServiceConfig (hService,
					SERVICE_NO_CHANGE,
					bAutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
					SERVICE_NO_CHANGE,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL))
				{
					if (GetLastError () == ERROR_ACCESS_DENIED)
						Result = AutoStartProduct::asAccessDenied;
				}
				else
				{
					Result = AutoStartProduct::asOK;
				}
			}
		}


		if (hService)
			CloseServiceHandle (hService);
		if (hSCManager)
			CloseServiceHandle (hSCManager);

		return Result;
	}
}

static AutoStartProduct::SetResult SetGUIAutoStart (bool bAutoStart)
{
	TCHAR szAVPCCUPath[_MAX_PATH];
	TCHAR szAVPCCUCmdLine[_MAX_PATH + max (sizeof (szAutoRunCommandLine9x), sizeof (szAutoRunCommandLineNt))  + 4] = {0};  //2(\"\")+1(space)+1(NULL)

	if (!GetModuleFileName (NULL, szAVPCCUPath, _MAX_PATH))
		return AutoStartProduct::asGeneralFail;

	if(szAVPCCUPath[0]!=_T('\"'))
		_tcscat (szAVPCCUCmdLine, _T("\""));

	_tcscat (szAVPCCUCmdLine, szAVPCCUPath);

	if(szAVPCCUPath[0]!=_T('\"'))
		_tcscat (szAVPCCUCmdLine, _T("\""));

	_tcscat (szAVPCCUCmdLine, _T(" "));
	_tcscat (szAVPCCUCmdLine, IsWin9x () ? szAutoRunCommandLine9x : szAutoRunCommandLineNt);
	
	return AddRemoveAppPathToRegistry (bAutoStart, 
								_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
								szAutoRunKeyName,
								szAVPCCUCmdLine);
}

AutoStartProduct::SetResult AutoStartProduct::SetAutoStartProductOption (bool bAutoStart)
{
	AutoStartProduct::SetResult GUIResult = SetGUIAutoStart (bAutoStart);
	if (GUIResult != AutoStartProduct::asOK)
		return GUIResult;
	
	AutoStartProduct::SetResult SrvResult = SetServiceAutoStart (bAutoStart);
	if (SrvResult != AutoStartProduct::asOK)
		return SrvResult;
	
	return AutoStartProduct::asOK;
}


bool IsVersionXP()
{
	//check windows XP
	OSVERSIONINFO info;
	ZeroMemory(&info, sizeof(OSVERSIONINFO));
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx (&info))
		return ( (info.dwMajorVersion > 5) ||	(info.dwMajorVersion == 5 && info.dwMinorVersion > 0) );
	
	return false;
}

bool IsWin9x ()
{
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof ( osvi );
    GetVersionEx ( &osvi );
    
    return ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void AddStrToCombo ( CComboBox & ComboBox, UINT StrID, DWORD Value, int & CurSel, DWORD DefaultValue )
{
    CString str ( ( LPCSTR ) StrID );
    
    int index = ComboBox.AddString ( str );
    
    if  ( index >= 0 )
    {
        ComboBox.SetItemData ( index, Value );
        
        if  ( DefaultValue == Value )
        {
            CurSel = index;
        }
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int FindComboItemByData ( CComboBox & ComboBox, DWORD Value )
{
	for ( int i = 0; i < ComboBox.GetCount (); ++i )
	{
		if (ComboBox.GetItemData (i) == Value)
			return i;
	}

	return CB_ERR;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void RemoveSecondsFromDateTimeCtrl(HWND hDT)
{
	TCHAR format[MAX_PATH];
	int format_size = ::GetLocaleInfo  ( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, format, sizeof (format)/sizeof(TCHAR));
	
	TCHAR separator[4];
	int separator_size = ::GetLocaleInfo  ( LOCALE_USER_DEFAULT, LOCALE_STIME, separator, sizeof (separator)/sizeof(TCHAR));
	
	if ( format_size && separator_size )
	{
		std::basic_string<TCHAR> std_format = format;
		const std::basic_string<TCHAR> std_separator = separator;
		
		int nLSepCount = 0;
		int nRSepCount = 0;
		int nStdEraseIndex = 0;
		int nSizeToErase = 0;
		bool bLeftSide = true;
		
		for ( unsigned i = 0; i<std_format.size  (); )
		{
			if ( std_format[i] == _T('s') )
			{
				if (!nStdEraseIndex)
					nStdEraseIndex = i;
				
				bLeftSide = false;
				++nSizeToErase;
				++i;
			}
			else if (std_format[i] == std_separator[0])
			{
				if (!nStdEraseIndex)
					nStdEraseIndex = i;
				
				if (bLeftSide)
					++nLSepCount;
				else
					++nRSepCount;
				
				nSizeToErase += std_separator.size  ();
				i += std_separator.size  ();
			}
			else
			{
				if ( nSizeToErase && (!bLeftSide) )
				{
					std_format.erase  (nStdEraseIndex, nSizeToErase);
					i = nStdEraseIndex;
				}
				
				++i;
				
				nLSepCount = 0;
				nRSepCount = 0;
				nStdEraseIndex = 0;
				nSizeToErase = 0;
				bool bLeftSide = true;
			}
		}
		
		if ( nSizeToErase && (!bLeftSide) )
			std_format.erase  (nStdEraseIndex, nSizeToErase);
		
		::SendMessage  (hDT, DTM_SETFORMAT, 0, (LPARAM)std_format.c_str  ());
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool IsFontAvailable(HDC hDC, LPCTSTR lpszFamily)
{
	struct HideCallback
	{
		static int CALLBACK EnumProc(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *lpntm,
										DWORD FontType,	LPARAM lParam	)
		{
			return 0;
		}
	};
	
	return ::EnumFontFamilies(hDC, lpszFamily, (FONTENUMPROC)HideCallback::EnumProc, NULL) == 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool IsTahomaFontAvailable()
{
	static bool available = IsFontAvailable(CClientDC(NULL), TAHOMA_FONT_NAME);
	return available;
}

bool IsArialFontAvailable()
{
	static bool available = IsFontAvailable(CClientDC(NULL), ARIAL_FONT_NAME);
	return available;
}

bool IsSansSerifFontAvailable()
{
	static bool available = IsFontAvailable(CClientDC(NULL), SANS_SERIF_FONT_NAME);
	return available;
}

LPCTSTR GetKAVFontFaceName()
{
	if(IsTahomaFontAvailable())
		return TAHOMA_FONT_NAME;
	else if(IsSansSerifFontAvailable())
		return SANS_SERIF_FONT_NAME;
	else if(IsArialFontAvailable())
		return ARIAL_FONT_NAME;

	// get default GUI font
	LPCTSTR pszFace = SYSTEM_FONT_NAME;
	WORD wDefSize = 10;
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	if (hFont != NULL)
	{
		LOGFONT lf;
		if (::GetObject(hFont, sizeof(LOGFONT), &lf) != 0)
			pszFace = lf.lfFaceName;
	}
	return pszFace;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL CreateKAVFont(CWnd *pWnd,
				    CFont &font, 
					CDC *pDC /* = NULL */,
					bool bUnderline /* = false */,
					LONG nWeight /* = FW_NORMAL */,
					double fHeightPt /* = KAV_FONT_SIZE_DEFAULT */,
					bool bItalic /* = false */,
					LPCTSTR szFaceName /* = NULL */,
					BYTE nPrecision /* = OUT_DEFAULT_PRECIS */
					)
{
	if(!pWnd || (HFONT)font != NULL || (!pWnd->GetSafeHwnd() && !IsWindow(pWnd->GetSafeHwnd())))
		return false;
	
	std::auto_ptr<CClientDC> dc;
	if(!pDC)
	{
		dc = std::auto_ptr<CClientDC>(new CClientDC(pWnd));
		pDC = dc.get();
	}

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	
	CFont *pFont = pWnd->GetFont();
	if(pFont)
		pFont->GetLogFont(&lf);
	else
		lf.lfCharSet = GetCharsetByCodePage(GetACP());
	
	lf.lfHeight = (LONG)floor(fHeightPt * pDC->GetDeviceCaps(LOGPIXELSY) / 72. + .5);
	lf.lfWeight = nWeight;
	lf.lfUnderline = bUnderline;
	lf.lfItalic = bItalic;
	lf.lfOutPrecision = nPrecision;

	if(szFaceName && IsFontAvailable(pDC->GetSafeHdc(), szFaceName))
		_tcscpy(lf.lfFaceName, szFaceName);
	else
		_tcscpy(lf.lfFaceName, GetKAVFontFaceName());

	return font.CreateFontIndirect(&lf);
}
