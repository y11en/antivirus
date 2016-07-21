#if _MSC_VER < 1300

#define _WIN32_WINNT	0x0500
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <..\src\afximpl.h>
#include <afxpriv.h>

#include "FileDialogImpl.h"
#include "../StuffIO/Ioutil.h"

#include <dlgs.h>       // for standard control IDs for commdlg


AFX_COMDAT UINT _afxMsgLBSELCHANGE = 0;
AFX_COMDAT UINT _afxMsgSHAREVI = 0;
AFX_COMDAT UINT _afxMsgFILEOK = 0;
//AFX_COMDAT UINT _afxMsgCOLOROK = 0;
AFX_COMDAT UINT _afxMsgHELP = 0;
AFX_COMDAT UINT _afxMsgSETRGB = 0;

UINT CALLBACK
_AfxMFCCommDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (hWnd == NULL)
        return 0;
    
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
    if (pThreadState->m_pAlternateWndInit != NULL && CWnd::FromHandlePermanent(hWnd) == NULL)
    {
        ASSERT_KINDOF(CCFileDialogImpl,pThreadState->m_pAlternateWndInit);
        pThreadState->m_pAlternateWndInit->SubclassWindow(hWnd);
        pThreadState->m_pAlternateWndInit = NULL;
    }
    
    if (message == WM_INITDIALOG)
    {
        _afxMsgLBSELCHANGE = ::RegisterWindowMessage(LBSELCHSTRING);
        _afxMsgSHAREVI = ::RegisterWindowMessage(SHAREVISTRING);
        _afxMsgFILEOK = ::RegisterWindowMessage(FILEOKSTRING);
        //_afxMsgCOLOROK = ::RegisterWindowMessage(COLOROKSTRING);
        _afxMsgHELP = ::RegisterWindowMessage(HELPMSGSTRING);
        _afxMsgSETRGB = ::RegisterWindowMessage(SETRGBSTRING);
        return (UINT)AfxDlgProc(hWnd, message, wParam, lParam);
    }
    
    if (message == _afxMsgHELP ||
        (message == WM_COMMAND && LOWORD(wParam) == pshHelp))
    {
        // just translate the message into the AFX standard help command.
        SendMessage(hWnd, WM_COMMAND, ID_HELP, 0);
        return 1;
    }
    
    if (message < 0xC000)
    {
        // not a ::RegisterWindowMessage message
        return 0;
    }
    
    // assume it is already wired up to a permanent one
    CDialog* pDlg = (CDialog*)CWnd::FromHandlePermanent(hWnd);
    ASSERT(pDlg != NULL);
    ASSERT_KINDOF(CDialog, pDlg);
    
    if (pDlg->IsKindOf(RUNTIME_CLASS(CCFileDialogImpl)))
    {
        // If we're exploring then we are not interested in the Registered messages
        if (((CCFileDialogImpl*)pDlg)->m_ofn.Flags & OFN_EXPLORER)
            return 0;
    }
    
    // RegisterWindowMessage - does not copy to lastState buffer, so
    // CWnd::GetCurrentMessage and CWnd::Default will NOT work
    // while in these handlers
    
    // Dispatch special commdlg messages through our virtual callbacks
    if (message == _afxMsgSHAREVI)
    {
        ASSERT_KINDOF(CCFileDialogImpl, pDlg);
        return ((CCFileDialogImpl*)pDlg)->OnShareViolation((LPCTSTR)lParam);
    }
    else if (message == _afxMsgFILEOK)
    {
        ASSERT_KINDOF(CCFileDialogImpl, pDlg);
        
        if (afxData.bWin4)
            ((CCFileDialogImpl*)pDlg)->m_pofnTemp = (OPENFILENAME*)lParam;
        
        BOOL bResult = ((CCFileDialogImpl*)pDlg)->OnFileNameOK();
        
        ((CCFileDialogImpl*)pDlg)->m_pofnTemp = NULL;
        
        return bResult;
    }
    else if (message == _afxMsgLBSELCHANGE)
    {
        ASSERT_KINDOF(CCFileDialogImpl, pDlg);
        ((CCFileDialogImpl*)pDlg)->OnLBSelChangedNotify(wParam, LOWORD(lParam),
            HIWORD(lParam));
        return 0;
    }
    //else if (message == _afxMsgCOLOROK)
    //{
    //	ASSERT_KINDOF(CColorDialog, pDlg);
    //	return ((CColorDialog*)pDlg)->OnColorOK();
    //}
    else if (message == _afxMsgSETRGB)
    {
        // nothing to do here, since this is a SendMessage
        return 0;
    }
    return 0;
}

CCFileDialogImpl* CCFileDialogImpl::CreateImpl(BOOL bOpenFileDialog,
											   LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
											   LPCTSTR lpszFilter, CWnd* pParentWnd)
{
	return new CCFileDialogImpl(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd);
}

void CCFileDialogImpl::DeleteImpl  (CCFileDialogImpl* pImpl)
{
	delete pImpl;
}
////////////////////////////////////////////////////////////////////////////
// FileOpen/FileSaveAs common dialog helper

CCFileDialogImpl::CCFileDialogImpl(BOOL bOpenFileDialog,
								   LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
								   LPCTSTR lpszFilter, CWnd* pParentWnd) : CCommonDialog(pParentWnd), m_fnProcOrig(NULL)
{
    memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
    m_szFileName[0] = '\0';
    m_szFileTitle[0] = '\0';
    m_pofnTemp = NULL;
    
    m_bOpenFileDialog = bOpenFileDialog;
    m_nIDHelp = bOpenFileDialog ? AFX_IDD_FILEOPEN : AFX_IDD_FILESAVE;
    
    // Windows 95/98 and Windows NT 4.0: In an application intended
    // for Windows 95/98 or Windows NT 4.0 and that is compiled with
    // WINVER and _WIN32_WINNT >= 0x0500, use
    // OPENFILENAME_SIZE_VERSION_400 for this member. 
    
    m_ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
    
    OSVERSIONINFO OsVerInfo;
    ZeroMemory ( &OsVerInfo, sizeof ( OsVerInfo ) );
    OsVerInfo.dwOSVersionInfoSize = sizeof ( OsVerInfo );

    if   ( GetVersionEx ( &OsVerInfo ) )
    {
        if  ( OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
        {
            if  ( OsVerInfo.dwMajorVersion >= 5 )
            {
                m_ofn.lStructSize = sizeof ( m_ofn );
            }
        }
    }
    
    m_ofn.lpstrFile = m_szFileName;
    m_ofn.nMaxFile = _countof(m_szFileName);
    m_ofn.lpstrDefExt = lpszDefExt;
    m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
    m_ofn.nMaxFileTitle = _countof(m_szFileTitle);
    m_ofn.Flags |= dwFlags | OFN_ENABLEHOOK | OFN_ENABLESIZING;
    if (!afxData.bWin4 && AfxHelpEnabled())
        m_ofn.Flags |= OFN_SHOWHELP;
    if (afxData.bWin4)
    {
        m_ofn.Flags |= OFN_EXPLORER;
        m_ofn.hInstance = AfxGetResourceHandle();
    }
    m_ofn.lpfnHook = (COMMDLGPROC)_AfxMFCCommDlgProc;
    
    // setup initial file name
    if (lpszFileName != NULL)
        lstrcpyn(m_szFileName, lpszFileName, _countof(m_szFileName));
    
    // Translate filter into commdlg format (lots of \0)
    if (lpszFilter != NULL)
    {
        m_strFilter = lpszFilter;
        LPTSTR pch = m_strFilter.GetBuffer(0); // modify the buffer in place
        // MFC delimits with '|' not '\0'
        while ((pch = _tcschr(pch, '|')) != NULL)
            *pch++ = '\0';
        m_ofn.lpstrFilter = m_strFilter;
        // do not call ReleaseBuffer() since the string contains '\0' characters
    }

	if (m_ofn.Flags & OFN_ALLOWMULTISELECT)
		m_ofn.Flags |= OFN_EXPLORER;

}

int CCFileDialogImpl::DoModal()
{
	if (m_aDefExts.GetSize() > 0)
		m_ofn.lpstrDefExt = m_aDefExts[0];

    ASSERT_VALID(this);
    ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
    ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook
    
    // zero out the file buffer for consistent parsing later
    ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
    DWORD nOffset = lstrlen(m_ofn.lpstrFile)+1;
    ASSERT(nOffset <= m_ofn.nMaxFile);
    memset(m_ofn.lpstrFile+nOffset, 0, (m_ofn.nMaxFile-nOffset)*sizeof(TCHAR));
    
    // WINBUG: This is a special case for the file open/save dialog,
    //  which sometimes pumps while it is coming up but before it has
    //  disabled the main window.
    HWND hWndFocus = ::GetFocus();
    BOOL bEnableParent = FALSE;
    m_ofn.hwndOwner = PreModal();
    AfxUnhookWindowCreate();
    if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
    {
        bEnableParent = TRUE;
        ::EnableWindow(m_ofn.hwndOwner, FALSE);
    }
    
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
    ASSERT(pThreadState->m_pAlternateWndInit == NULL);
    
    if (m_ofn.Flags & OFN_EXPLORER)
        pThreadState->m_pAlternateWndInit = this;
    else
        AfxHookWindowCreate(this);
    
    int nResult;
    if (m_bOpenFileDialog)
        nResult = ::GetOpenFileName(&m_ofn);
    else
        nResult = ::GetSaveFileName(&m_ofn);
    
    if (nResult)
        ASSERT(pThreadState->m_pAlternateWndInit == NULL);
    pThreadState->m_pAlternateWndInit = NULL;
    
    // WINBUG: Second part of special case for file open/save dialog.
    if (bEnableParent)
        ::EnableWindow(m_ofn.hwndOwner, TRUE);
    if (::IsWindow(hWndFocus))
        ::SetFocus(hWndFocus);
    
    PostModal();
    return nResult ? nResult : IDCANCEL;
}

CString CCFileDialogImpl::GetPathName() const
{
    if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
    {
        ASSERT(::IsWindow(m_hWnd));
        CString strResult;
        if (GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH,
            (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
        {
            strResult.Empty();
        }
        else
        {
            strResult.ReleaseBuffer();
        }
        
        if (!strResult.IsEmpty())
        {
            if (GetParent()->SendMessage(CDM_GETFILEPATH, (WPARAM)MAX_PATH,
                (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
                strResult.Empty();
            else
            {
                strResult.ReleaseBuffer();
                return strResult;
            }
        }
    }
    return m_ofn.lpstrFile;
}

CString CCFileDialogImpl::GetFileName() const
{
    if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
    {
        ASSERT(::IsWindow(m_hWnd));
        CString strResult;
        if (GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH,
            (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
        {
            strResult.Empty();
        }
        else
        {
            strResult.ReleaseBuffer();
            return strResult;
        }
    }
    return m_ofn.lpstrFileTitle;
}

CString CCFileDialogImpl::GetFileExt() const
{
    if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
    {
        ASSERT(::IsWindow(m_hWnd));
        CString strResult;
        if (GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH,
            (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
            strResult.Empty();
        else
        {
            strResult.ReleaseBuffer();
            int pos = strResult.ReverseFind('.');
            if (pos >= 0)
                return strResult.Right(strResult.GetLength() - pos - 1);
            strResult.Empty();
        }
        return strResult;
    }
    
    if (m_pofnTemp != NULL)
        if (m_pofnTemp->nFileExtension == 0)
            return &afxChNil;
        else
            return m_pofnTemp->lpstrFile + m_pofnTemp->nFileExtension;
        
        if (m_ofn.nFileExtension == 0)
            return &afxChNil;
        else
            return m_ofn.lpstrFile + m_ofn.nFileExtension;
}

CString CCFileDialogImpl::GetFileTitle() const
{
    CString strResult = GetFileName();
    int pos = strResult.ReverseFind('.');
    if (pos >= 0)
        return strResult.Left(pos);
    return strResult;
}

CString CCFileDialogImpl::GetNextPathName(POSITION& pos) const
{
	if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) && !m_szFiles.IsEmpty  ())
		return GetNextPathNameMultiSelect  (pos);
	else
		return GetNextPathNameSingleSelect  (pos);
}

CString CCFileDialogImpl::GetNextPathNameMultiSelect  (POSITION& pos) const
{
	ASSERT (m_ofn.Flags & OFN_EXPLORER);
	ASSERT (m_ofn.Flags & OFN_ALLOWMULTISELECT);

	CString szFileName = GetNextFileName(pos);

	if (pos)
	{
		POSITION lookahead = pos;
		GetNextFileName(lookahead);
		
		if (lookahead==0)
			pos = 0;
	}

	bool bBackSlash = false;
	if ( !m_szFolderPath.IsEmpty  () )
		bBackSlash = (m_szFolderPath.GetAt  (m_szFolderPath.GetLength  ()-1) != _T('\\'));


	return m_szFolderPath + (bBackSlash?_T("\\"):_T("")) + szFileName;	

}

CString CCFileDialogImpl::GetNextFileName(POSITION& pos) const
{
	
	int nStartName = m_szFiles.Find  (_T('\"'), (int)pos==-1?0:(int)pos);
	int nEndName;
	if (nStartName!=-1)
	{
		++nStartName;
		nEndName = m_szFiles.Find  (_T('\"'), nStartName);
		
		if (nEndName != -1)
		{
			pos = (POSITION)(nEndName+1);
		}
		else
		{
			pos = 0;
			return _T("");
		}
	}
	else
	{
		pos = 0;
		return _T("");
	}

	return m_szFiles.Mid  (nStartName, nEndName - nStartName);
}


CString CCFileDialogImpl::GetNextPathNameSingleSelect(POSITION& pos) const
{
    BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
    TCHAR chDelimiter;
    if (bExplorer)
        chDelimiter = '\0';
    else
        chDelimiter = ' ';
    
    LPTSTR lpsz = (LPTSTR)pos;
    if (lpsz == m_ofn.lpstrFile) // first time
    {
        if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
        {
            pos = NULL;
            return m_ofn.lpstrFile;
        }
        
        // find char pos after first Delimiter
        while(*lpsz != chDelimiter && *lpsz != '\0')
            lpsz = _tcsinc(lpsz);
        lpsz = _tcsinc(lpsz);
        
        // if single selection then return only selection
        if (*lpsz == 0)
        {
            pos = NULL;
            return m_ofn.lpstrFile;
        }
    }
    
    CString strPath = m_ofn.lpstrFile;
    if (!bExplorer)
    {
        LPTSTR lpszPath = m_ofn.lpstrFile;
        while(*lpszPath != chDelimiter)
            lpszPath = _tcsinc(lpszPath);
        strPath = strPath.Left(lpszPath - m_ofn.lpstrFile);
    }
    
    LPTSTR lpszFileName = lpsz;
    CString strFileName = lpsz;
    
    // find char pos at next Delimiter
    while(*lpsz != chDelimiter && *lpsz != '\0')
        lpsz = _tcsinc(lpsz);
    
    if (!bExplorer && *lpsz == '\0')
        pos = NULL;
    else
    {
        if (!bExplorer)
            strFileName = strFileName.Left(lpsz - lpszFileName);
        
        lpsz = _tcsinc(lpsz);
        if (*lpsz == '\0') // if double terminated then done
            pos = NULL;
        else
            pos = (POSITION)lpsz;
    }
    
    // only add '\\' if it is needed
    if (!strPath.IsEmpty())
    {
        // check for last back-slash or forward slash (handles DBCS)
        LPCTSTR lpsz = _tcsrchr(strPath, '\\');
        if (lpsz == NULL)
            lpsz = _tcsrchr(strPath, '/');
        // if it is also the last character, then we don't need an extra
        if (lpsz != NULL &&
            (lpsz - (LPCTSTR)strPath) == strPath.GetLength()-1)
        {
            ASSERT(*lpsz == '\\' || *lpsz == '/');
            return strPath + strFileName;
        }
    }
    return strPath + '\\' + strFileName;
}

void CCFileDialogImpl::SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID)
{
    if (m_ofn.Flags & OFN_EXPLORER)
        m_ofn.lpTemplateName = lpWin4ID;
    else
        m_ofn.lpTemplateName = lpWin3ID;
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
}

CString CCFileDialogImpl::GetFolderPath() const
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT(m_ofn.Flags & OFN_EXPLORER);
    
    CString strResult;
    if (GetParent()->SendMessage(CDM_GETFOLDERPATH, (WPARAM)MAX_PATH, (LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
        strResult.Empty();
    else
        strResult.ReleaseBuffer();
    return strResult;
}

void CCFileDialogImpl::SetControlText(int nID, LPCSTR lpsz)
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT(m_ofn.Flags & OFN_EXPLORER);
    GetParent()->SendMessage(CDM_SETCONTROLTEXT, (WPARAM)nID, (LPARAM)lpsz);
}

void CCFileDialogImpl::HideControl(int nID)
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT(m_ofn.Flags & OFN_EXPLORER);
    GetParent()->SendMessage(CDM_HIDECONTROL, (WPARAM)nID, 0);
}

void CCFileDialogImpl::SetDefExt(LPCSTR lpsz)
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT(m_ofn.Flags & OFN_EXPLORER);
    GetParent()->SendMessage(CDM_SETDEFEXT, 0, (LPARAM)lpsz);
}

void CCFileDialogImpl::SetDefExts(LPCTSTR* lpDefExts, int nCount)
{
	m_aDefExts.RemoveAll();
	for (; nCount>0; nCount--, lpDefExts++)
		m_aDefExts.Add(*lpDefExts);
}

UINT CCFileDialogImpl::OnShareViolation(LPCTSTR)
{
    ASSERT_VALID(this);
    
    // Do not call Default() if you override
    return OFN_SHAREWARN; // default
}

BOOL CCFileDialogImpl::OnFileNameOK()
{
    ASSERT_VALID(this);
	
    // Do not call Default() if you override
    return FALSE;
}

void CCFileDialogImpl::OnLBSelChangedNotify(UINT, UINT, UINT)
{
    ASSERT_VALID(this);
    
    // Do not call Default() if you override
    // no default processing needed
}

LRESULT CALLBACK CCFileDialogImpl::fnWndProcFilter (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCFileDialogImpl* pThis = (CCFileDialogImpl*)GetWindowLong  (hWnd, GWL_USERDATA);
	ASSERT(pThis->GetParent()->GetSafeHwnd  () == hWnd);
	
	if (uMsg == WM_COMMAND && (WORD)wParam == IDOK && pThis -> m_bOpenFileDialog && (pThis->m_ofn.Flags & OFN_ALLOWMULTISELECT))
	{
		HWND hWndCombo = ::GetDlgItem(hWnd, cmb13);
			
		//old style edit, not combo (9x)
		if (!::IsWindow(hWndCombo))
			hWndCombo = ::GetDlgItem  (hWnd, edt1);
		
		if (::IsWindow  (hWndCombo))
		{
			CString szWndText;
			CString szFolderPath = pThis->GetFolderPath();

			int nLen = ::GetWindowTextLength(hWndCombo);
			::GetWindowText(hWndCombo, szWndText.GetBufferSetLength(nLen), nLen+1);
			szWndText.ReleaseBuffer();

			DWORD FileAttributes = IOSGetFileAttributes(szWndText);

			if  ( ( FileAttributes == INVALID_FILE_ATTRIBUTES ) )
				FileAttributes = IOSGetFileAttributes ( szFolderPath + ((szFolderPath[szFolderPath.GetLength() - 1]==_T('\\'))?szWndText:_T('\\')+szWndText) );
			
			if ( ( FileAttributes == INVALID_FILE_ATTRIBUTES ) || !(FileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				
				if (szWndText.Find  (_T('\"')) != -1)
				{
					pThis->m_szFiles = szWndText;
					pThis->m_szFolderPath = szFolderPath;
					::EndDialog(hWnd, IDOK);
					return 0;
				}
			}

		}

	}
	else if (uMsg == WM_DESTROY)
	{
		::SetWindowLong (hWnd, GWL_WNDPROC, (LONG)pThis->m_fnProcOrig);
		::SetWindowLong (hWnd, GWL_USERDATA, 0);
	}
	
	return pThis->m_fnProcOrig(hWnd, uMsg, wParam, lParam);
}
void CCFileDialogImpl::OnInitDone()
{
    ASSERT_VALID(this);
    GetParent()->CenterWindow();
	
	CWnd *pParent = GetParent();
	if (pParent)
	{
		m_fnProcOrig = (WNDPROC)::SetWindowLong (pParent->GetSafeHwnd  (), GWL_WNDPROC, (LONG)CCFileDialogImpl::fnWndProcFilter);
		::SetWindowLong (pParent->GetSafeHwnd  (), GWL_USERDATA, (LONG)this);
	}
	
    // Do not call Default() if you override
    // no default processing needed
}

void CCFileDialogImpl::OnFileNameChange()
{
    ASSERT_VALID(this);
    
    // Do not call Default() if you override
    // no default processing needed
}

void CCFileDialogImpl::OnFolderChange()
{
    ASSERT_VALID(this);
    
    // Do not call Default() if you override
    // no default processing needed
}

void CCFileDialogImpl::OnTypeChange()
{
    ASSERT_VALID(this);
    
    // Do not call Default() if you override
    // no default processing needed

	if (m_aDefExts.GetSize() > 0)
	{
		ASSERT(m_ofn.nFilterIndex <= (DWORD)m_aDefExts.GetSize());
		SetDefExt(m_aDefExts[m_ofn.nFilterIndex-1]);
	}
}

BOOL CCFileDialogImpl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    ASSERT(pResult != NULL);
    
    // allow message map to override
    if (CCommonDialog::OnNotify(wParam, lParam, pResult))
        return TRUE;
    
    OFNOTIFY* pNotify = (OFNOTIFY*)lParam;
    switch(pNotify->hdr.code)
    {
    case CDN_INITDONE:
        OnInitDone();
        return TRUE;
    case CDN_SELCHANGE:
        OnFileNameChange();
        return TRUE;
    case CDN_FOLDERCHANGE:
        OnFolderChange();
        return TRUE;
    case CDN_SHAREVIOLATION:
        *pResult = OnShareViolation(pNotify->pszFile);
        return TRUE;
    case CDN_HELP:
        if (!SendMessage(WM_COMMAND, ID_HELP))
            SendMessage(WM_COMMANDHELP, 0, 0);
        return TRUE;
    case CDN_FILEOK:
        *pResult = OnFileNameOK();
        return TRUE;
    case CDN_TYPECHANGE:
        OnTypeChange();
        return TRUE;
    }
    
    return FALSE;   // not handled
}

BOOL CCFileDialogImpl::GetReadOnlyPref () const
{
	return m_ofn.Flags & OFN_READONLY ? TRUE : FALSE;
}

// Enumerating multiple file selections
POSITION CCFileDialogImpl::GetStartPosition () const
{
	if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) && !m_szFiles.IsEmpty  ())
		return ( POSITION ) -1;
	else
		return ( POSITION ) m_ofn.lpstrFile;
}

// Helpers for custom templates
void CCFileDialogImpl::SetTemplate(UINT nWin3ID, UINT nWin4ID)
{
	SetTemplate ( MAKEINTRESOURCE ( nWin3ID ), MAKEINTRESOURCE ( nWin4ID ) );
}


IMPLEMENT_DYNAMIC(CCFileDialogImpl, CDialog)

//==============================================================================

#endif // #if _MSC_VER < 1300