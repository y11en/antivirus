// ChooseObjectDialog.cpp: implementation of the CCChooseObjectDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common.h"
#include "ChooseObjectDialog.h"
#include <shlwapi.h>
#include "../KLUtil/UnicodeConv.h"
#include "..\StuffIO\Ioutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define BID_NOTE_TEXT_STATIC	0x3742
#define BID_STATE_TEXT_STATIC	0x3743
#define BID_EDIT_TEXT_EDT		0x3744
#define BID_TREECTRL			0x3741
 
#define ILSkip(pidl, cb)       ((LPITEMIDLIST)(((BYTE*)(pidl))+cb))
#define ILNext(pidl)           ILSkip(pidl, (pidl)->mkid.cb)

CSBHelper& CSBHelper::Instance()
{
	static CSBHelper Instance;
	return Instance;
}

CSBHelper::CSBHelper()
{
	m_hShellLib = LoadLibrary(_T("shell32.dll"));
	if (m_hShellLib)
		m_SHBindToParent = (fnSHBindToParent)GetProcAddress(m_hShellLib, "SHBindToParent");
	else
		m_SHBindToParent = NULL;
}

CSBHelper::~CSBHelper()
{
	m_SHBindToParent = NULL;
	if (m_hShellLib)
		FreeLibrary(m_hShellLib);
}

// Retrieves the UIObject interface for the specified full PIDL
HRESULT CSBHelper::SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
{
	LPCITEMIDLIST pidlChild;
	IShellFolder* psf;

	*ppv = NULL;

	if (m_SHBindToParent == NULL)
		return E_NOTIMPL;

	HRESULT hr = m_SHBindToParent(pidl, IID_IShellFolder, (void**)(&psf), &pidlChild);
	if (SUCCEEDED(hr))
	{
		hr = psf->GetUIObjectOf(hwnd, 1, &pidlChild, riid, NULL, ppv);
		psf->Release();
	}
	return hr;
}

HRESULT CSBHelper::SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidl)
{
	DWORD cbTotal = 0;

	if (pidl)
	{
		LPCITEMIDLIST pidl_temp = pidl;
		cbTotal += sizeof (pidl_temp->mkid.cb);

		while (pidl_temp->mkid.cb) 
		{
			cbTotal += pidl_temp->mkid.cb;
			pidl_temp = ILNext (pidl_temp);
		}
	}

	*ppidl = (LPITEMIDLIST)CoTaskMemAlloc(cbTotal);

	if (*ppidl)
		CopyMemory(*ppidl, pidl, cbTotal);

	return  *ppidl ? S_OK: E_OUTOFMEMORY;
}

// Get the target PIDL for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, etc.
HRESULT CSBHelper::SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl)
{
	IShellLink *psl;

	*ppidl = NULL;

	HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, IID_IShellLink, (void**)(&psl));

	if (SUCCEEDED(hr))
	{
		hr = psl->GetIDList(ppidl);
		psl->Release();
	}

	// It's not a folder shortcut so get the PIDL normally.
	if (FAILED(hr))
		hr = SHILClone(pidlFolder, ppidl);

	return hr;
}

// Get the target folder for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, the My Documents folder, etc.
HRESULT CSBHelper::GetTargetFolderPathFromIDListA(LPCITEMIDLIST pidlFolder, LPSTR pszPath)
{
	LPITEMIDLIST pidlTarget;

	*pszPath = 0;

	HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);

	if (SUCCEEDED(hr))
	{
		hr = ::SHGetPathFromIDListA(pidlTarget, pszPath);
		CoTaskMemFree(pidlTarget);
	}

	return hr;
}

// Get the target folder for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, the My Documents folder, etc.
HRESULT CSBHelper::GetTargetFolderPathFromIDListW(LPCITEMIDLIST pidlFolder, LPWSTR pszPath)
{
	LPITEMIDLIST pidlTarget;

	*pszPath = 0;

	HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);

	if (SUCCEEDED(hr))
	{
		hr = ::SHGetPathFromIDListW(pidlTarget, pszPath);
		CoTaskMemFree(pidlTarget);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCChooseObjectDialog::CCChooseObjectDialog (UINT nHelpId) : 
CCWhiteBarDlg (0, nHelpId),
m_bInit(false),
m_bValidated (false),
m_bInitialEmpty (false)
{
	ZeroMemory(m_szObjectPath, sizeof(m_szObjectPath));
}

CCChooseObjectDialog::~CCChooseObjectDialog()
{
	
}

void CCChooseObjectDialog::GetObjectPath(CStdStringW& szOutPath)
{ 
	szOutPath = IsWin9x()?__LPCWSTR((LPCSTR)m_szObjectPath):(LPCWSTR)m_szObjectPath;
}

void CCChooseObjectDialog::SetObjectPath(CStdStringW& szPath)
{
	if (IsWin9x())
		strcpy((CHAR*)m_szObjectPath, __LPCSTR(szPath.c_str  ()));
	else
		wcscpy((WCHAR*)m_szObjectPath, szPath.c_str());
}

bool CCChooseObjectDialog::Browse(CWnd *pParentWnd, long nType/* = 0*/)
{
	m_nBrowseType = nType;

    if  ( IsWin9x () )
    {
        BROWSEINFO bi;
        ZeroMemory(&bi,sizeof(BROWSEINFO));
        bi.hwndOwner = pParentWnd->GetSafeHwnd();
        bi.lParam = (LPARAM)this;
        bi.lpfn = CCChooseObjectDialog::BrowseFolderCallbackProc;
        bi.ulFlags |= (nType & COD_EDITBOX)?BIF_EDITBOX | BIF_VALIDATE:0;
        bi.ulFlags |= (nType & COD_FILEANDFOLDERS)?BIF_BROWSEINCLUDEFILES:0;
        m_bInitialEmpty = (nType & COD_INITIALEMPTY) != 0;
        
        bi.lpszTitle = m_szNote;
        
        LPITEMIDLIST pidl;

        if  ( pidl = ::SHBrowseForFolder ( &bi ) )
        {

			if (!(nType & COD_EDITBOX) || strlen((LPCSTR)m_szObjectPath) == 0)
			{
				if  (SUCCEEDED (::SHGetPathFromIDListA ( pidl, (LPSTR)m_szObjectPath ) ) )
				{
					NormalizeFolderPathA ( (LPSTR)m_szObjectPath, MAX_PATH );
					m_bValidated = true;
				}
			}
             
            LPMALLOC pMalloc;
            if  ( SHGetMalloc ( &pMalloc ) == NOERROR )
            {
                pMalloc -> Free ( pidl );
                pMalloc -> Release ();
            }
        }
		
		return strlen((LPCTSTR)m_szObjectPath) && m_bValidated;
    }
    else
    {
        BROWSEINFOW bi;
        ZeroMemory ( &bi,sizeof ( bi ) );
        bi.hwndOwner = pParentWnd -> GetSafeHwnd ();
        bi.lParam = (LPARAM)this;
        bi.lpfn = CCChooseObjectDialog::BrowseFolderCallbackProc;
        bi.ulFlags |= (nType & COD_EDITBOX)?BIF_EDITBOX | BIF_VALIDATE:0;
        bi.ulFlags |= (nType & COD_FILEANDFOLDERS)?BIF_BROWSEINCLUDEFILES:0;
        m_bInitialEmpty = (nType & COD_INITIALEMPTY) != 0;
        
        bi.lpszTitle = __LPCWSTR(( LPCSTR )m_szNote);
        
        LPITEMIDLIST pidl;
		
        if  ( pidl = ::SHBrowseForFolderW ( &bi ) )
        {

			if (!(nType & COD_EDITBOX) || wcslen((LPCWSTR)m_szObjectPath) == 0)
			{
				if  (SUCCEEDED(
						CSBHelper::Instance().GetTargetFolderPathFromIDListW ( pidl, (LPWSTR)m_szObjectPath )
					))
				{
					NormalizeFolderPathW ( (LPWSTR)m_szObjectPath, MAX_PATH );
					m_bValidated = true;
				}
			}

            LPMALLOC pMalloc;
            if( SHGetMalloc(&pMalloc) == NOERROR )
            {
                pMalloc->Free(pidl);
                pMalloc->Release();
            }
        }
		
		return wcslen((LPCWSTR)m_szObjectPath) && m_bValidated;
    }
}

bool CCChooseObjectDialog::OnValidate(LPCSTR lpszObject)
{ 
	CInputRestriction* pInputRestriction = m_Edit.GetRestriction  ();
	if (pInputRestriction)
	{
		CString szErrTitle, szErrText;
		if( !pInputRestriction->Finalize  (lpszObject, szErrTitle, szErrText) )
		{
			if (!szErrText.IsEmpty  ())
			{
				CWnd* pParent = GetParent();
				HICON hIcon = pParent?(HICON)pParent->GetIcon  (true):NULL;
				MsgBoxEx(GetSafeHwnd(), szErrText, szErrTitle, MB_OK | MB_ICONERROR, hIcon);
			}
			
			return false;
		}
	}
	
	return true;
}

bool CCChooseObjectDialog::OnValidate(LPCWSTR lpszObject)
{ 
	CInputRestriction* pInputRestriction = m_Edit.GetRestriction  ();
	if (pInputRestriction)
	{
		CString szErrTitle, szErrText;
		if( !pInputRestriction->Finalize  (lpszObject, szErrTitle, szErrText) )
		{
			if (!szErrText.IsEmpty  ())
			{
				CWnd* pParent = GetParent();
				HICON hIcon = pParent?(HICON)pParent->GetIcon  (true):NULL;
				MsgBoxEx(GetSafeHwnd(), szErrText, szErrTitle, MB_OK | MB_ICONERROR, hIcon);
			}
			
			return false;
		}
	}
	
	return true;
}

int CALLBACK CCChooseObjectDialog::BrowseFolderCallbackProc ( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	if (!lpData)
		return 0;
	
	CCChooseObjectDialog* pThis = (CCChooseObjectDialog*)lpData;

	if (uMsg == BFFM_INITIALIZED)
	{
		if (!pThis->m_szTitle.IsEmpty  ())
			::SetWindowText (hwnd, (LPCSTR)pThis->m_szTitle);
		
		::SetDlgItemText (hwnd, IDOK, CString ((LPCTSTR)IDS_COMMON_BUTTON_CHOOSE_STR));
		::SetDlgItemText (hwnd, IDCANCEL, CString ((LPCTSTR)IDS_COMMON_BUTTON_CANCEL_STR));
//		::SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)LoadIconLR  (IDI_AVPCCU_MAINFRAME));
		
		::SetWindowLong ( hwnd, GWL_STYLE, ::GetWindowLong(hwnd, GWL_STYLE) & (~DS_CONTEXTHELP) );
		::SetWindowLong ( hwnd, GWL_EXSTYLE, ::GetWindowLong(hwnd, GWL_EXSTYLE) & (~WS_EX_CONTEXTHELP) );
		
		VERIFY(pThis->SubclassWindow  (hwnd));
		
//		pThis->ModifyStyle(DS_CONTEXTHELP, 0);
//		pThis->ModifyStyleEx(WS_EX_CONTEXTHELP, 0);

		CWnd* pTreeCtrl = pThis->GetDlgItem(BID_TREECTRL);

		if (pTreeCtrl)
		{
			CRect rcDlg, rcTreeCtrl;
			pThis->GetWindowRect(rcDlg);
			pTreeCtrl->GetWindowRect(rcTreeCtrl);
			
			rcDlg.bottom = rcTreeCtrl.bottom + MulDiv(7, HIWORD(::GetDialogBaseUnits  ()), 8);
			pThis->MoveWindow(rcDlg);
		}
		
		if (pThis->m_nBrowseType & COD_EDITBOX)
		{
			HWND hEditWnd = ::GetDlgItem(pThis->GetSafeHwnd  (), BID_EDIT_TEXT_EDT);
			
			if (hEditWnd)
			{
				CRect rcEdtWnd;
				::GetWindowRect  (hEditWnd, rcEdtWnd);
				pThis->ScreenToClient  (rcEdtWnd);
				VERIFY(pThis->m_Edit.Create  (pThis, rcEdtWnd));
				
				if (IsWin9x())
					::SetWindowText  (pThis->m_Edit.GetSafeHwnd  (), pThis->m_bInitialEmpty ? _T("") : (LPCSTR)pThis->m_szObjectPath);
				else
					::SetWindowTextW  (pThis->m_Edit.GetSafeHwnd  (), pThis->m_bInitialEmpty ? L"" : (LPCWSTR)pThis->m_szObjectPath);
				
				::ShowWindow(hEditWnd, SW_HIDE);
			}
		}
		
		
		pThis->OnInitDialog();
		
		pThis->m_bInit = true;

		if (IsWin9x())
			::SendMessage (hwnd, BFFM_SETSELECTIONA, TRUE, (LPARAM)pThis->m_szObjectPath);
		else
			::SendMessageW (hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)pThis->m_szObjectPath);
	}
	else if (uMsg == BFFM_VALIDATEFAILEDA )
	{
		if (!pThis->m_bInit)
			return 0;

		if (pThis->m_nBrowseType & COD_EDITBOX)
			return 0;
		
		if  ( pThis -> OnValidate ( ( LPCSTR ) lParam ) )
		{
//			strcpy((LPSTR)pThis->m_szObjectPath, (LPCSTR)lParam);
			pThis->m_bValidated = true;
			return 0;
		}
		else
			return 1;
		
	}
    else if (uMsg == BFFM_VALIDATEFAILEDW )
    {
        if (!pThis->m_bInit)
            return 0;

		if (pThis->m_nBrowseType & COD_EDITBOX)
			return 0;
		
        
        if  ( pThis -> OnValidate ( (LPCWSTR)lParam ) )
        {
//            wcscpy( (LPWSTR)pThis->m_szObjectPath, ( LPCWSTR ) lParam;
            pThis->m_bValidated = true;
            return 0;
        }
        else
            return 1;
        
    }
	else if (uMsg == BFFM_SELCHANGED )
	{
		if (!(pThis->m_bInit && (pThis->m_nBrowseType & COD_EDITBOX)))
			return 0;
		
        if  ( IsWin9x () )
        {
		    LPCITEMIDLIST pIDList = ( LPCITEMIDLIST ) lParam;

            CHAR szFolder [ MAX_PATH ];

            if  ( ::IsWindow ( pThis -> m_Edit.GetSafeHwnd () ) && SUCCEEDED ( ::SHGetPathFromIDListA ( pIDList, szFolder ) ) )
		    {
			    NormalizeFolderPathA ( szFolder, MAX_PATH );

				// Для файлов внутри zip-folder szFolder оказывается пустым. Вернём курсор на предыдущий элемент.
				if (strlen(szFolder) == 0
					&& 0 < ::GetWindowTextA(pThis->m_Edit.GetSafeHwnd(), szFolder, sizeof(szFolder)/sizeof(*szFolder)))
					::SendMessage(hwnd, BFFM_SETSELECTIONA, TRUE, (LPARAM) szFolder);
				else
					::SetWindowTextA ( pThis -> m_Edit.GetSafeHwnd (), szFolder );
		    }
        }
        else
        {
            LPCITEMIDLIST pIDList = ( LPCITEMIDLIST ) lParam;
            
            WCHAR szFolder [ MAX_PATH ];
            
            if  ( ::IsWindow ( pThis -> m_Edit.GetSafeHwnd () )
				&& SUCCEEDED (	CSBHelper::Instance().GetTargetFolderPathFromIDListW ( pIDList, szFolder ) ) )
            {
                NormalizeFolderPathW ( szFolder, MAX_PATH );

				// Для файлов внутри zip-folder szFolder оказывается пустым. Вернём курсор на предыдущий элемент.
				if (wcslen(szFolder) == 0
					&& 0 < ::GetWindowTextW(pThis->m_Edit.GetSafeHwnd(), szFolder, sizeof(szFolder)/sizeof(*szFolder)))
					::SendMessage(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM) szFolder);
				else
					::SetWindowTextW ( pThis -> m_Edit.GetSafeHwnd (), szFolder );
            }
        }
	}
	return 0;
}

void CCChooseObjectDialog::OnOK()
{
	if (m_nBrowseType & COD_EDITBOX)
	{
		if (IsWin9x())
		{
			::GetWindowText(m_Edit.GetSafeHwnd  (), (LPTSTR)m_szObjectPath, sizeof(m_szObjectPath)/sizeof(TCHAR));
			if  ( OnValidate ( (LPCTSTR)m_szObjectPath ) )
			{
				m_bValidated = true;
				Default ();
			}
		}
		else
		{
			::GetWindowTextW(m_Edit.GetSafeHwnd  (), (LPWSTR)m_szObjectPath, sizeof(m_szObjectPath)/sizeof(WCHAR));
			if  ( OnValidate ( (LPWSTR)m_szObjectPath ) )
			{
				m_bValidated = true;
				Default ();
			}
		}
	}
	else
		Default ();
}

void NormalizeFolderPathA ( LPSTR szFolder, int char_buffsize )
{
	DWORD FileAttributes = IOSGetFileAttributes( szFolder );

	if  ( ( FileAttributes != INVALID_FILE_ATTRIBUTES ) && ( FileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
	{
		int len = lstrlen(szFolder);

        if( len!= 0 && len < char_buffsize - 1 )
		{
			if( szFolder[ len - 1 ] != _T('\\') )
			{
				szFolder[ len ] = _T('\\');
				szFolder[ len + 1 ] = 0;
			}
		}
	}
}

void NormalizeFolderPathW ( LPWSTR szFolder, int char_buffsize )
{
    DWORD FileAttributes = IOSGetFileAttributesW ( szFolder );
    
    if  ( ( FileAttributes != INVALID_FILE_ATTRIBUTES ) && ( FileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
    {
        int len = wcslen ( szFolder );

        if  ( len && len < char_buffsize - 1 )
        {
            if  ( szFolder [ len - 1 ] != _T('\\') )
            {
                szFolder [ len ] = L'\\';
                szFolder [ len + 1 ] = 0;
            }
        }
    }
}

void NormalizeFolderPath (CString &szFolder)
{
	DWORD FileAttributes = IOSGetFileAttributes ( szFolder );

	if  ( ( FileAttributes != INVALID_FILE_ATTRIBUTES ) && ( FileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
	{
		int nLength = szFolder.GetLength ();

		if  ( szFolder [ nLength - 1 ] != _T('\\') )
        {
            szFolder += _T('\\');
        }
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseDirectoryNT4

#pragma warning (disable : 4355)

CBrowseDirectoryNT4::CBrowseDirectoryNT4 (UINT nHelpId, CWnd* pParentWnd, CInputRestriction *pRestriction, LPCTSTR szInitialStr) :
		CFileDialog (TRUE, NULL, NULL,
						OFN_HIDEREADONLY | OFN_NOTESTFILECREATE | 
						OFN_NOVALIDATE | OFN_NOCHANGEDIR,
						NULL, pParentWnd),
		m_bFirstUpdate (TRUE),
		m_ParentBrowseDirWnd (this, nHelpId),
		m_pRestriction (pRestriction),
		m_szInitialStr (szInitialStr),
		m_bInitialEmpty (false)
{
	m_ofn.Flags &= ~OFN_ENABLESIZING;
}


BEGIN_MESSAGE_MAP(CBrowseDirectoryNT4, CFileDialog)
	//{{AFX_MSG_MAP(CBrowseDirectoryNT4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBrowseDirectoryNT4::PreSubclassWindow()
{
	m_ParentBrowseDirWnd.SubclassWindow (GetParent()->GetSafeHwnd ());
	
	m_ParentBrowseDirWnd.ModifyStyle (DS_CONTEXTHELP, 0);
	m_ParentBrowseDirWnd.ModifyStyleEx(WS_EX_CONTEXTHELP, 0);
}

void CBrowseDirectoryNT4::OnInitDone()
{
	HideControl(cmb1);
	HideControl(stc2);
	HideControl(stc3);
	HideControl(edt1);

	CWnd *pParent = GetParent();

	if(pParent)
	{
		CWnd *pCtrl;

		CRect rc, rc1;

		// edt1
		pParent->GetDlgItem(edt1)->GetWindowRect(&rc);
		pParent->ScreenToClient(&rc);

		pParent->GetDlgItem(lst1)->GetWindowRect(&rc1);
		pParent->ScreenToClient(&rc1);

		rc.left = rc1.left;
		rc.right = rc1.right;

		// create new controls
		int nDeltaHeight = 0;

		if (!m_szNote.IsEmpty ())
		{
			rc.bottom += 8;

			m_Label.CreateEx(0x00000004, _T("STATIC"), NULL, 0x50020000,
				rc, pParent, stc32);
			m_Label.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			m_Label.SetFont(pParent->GetDlgItem(edt1)->GetFont());
			m_Label.SetWindowText (m_szNote);

			nDeltaHeight = rc.Height () + 2 * 5;
			rc.OffsetRect (0, nDeltaHeight);

			rc.bottom -= 8;
		}

		m_FolderName.CreateEx(0x00000204, _T("EDIT"), NULL, 0x50010080,
			rc, pParent, edt16);
		m_FolderName.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		m_FolderName.SetFont(pParent->GetDlgItem(edt1)->GetFont());
		m_FolderName.SetRestriction (m_pRestriction);

		if (!m_szNote.IsEmpty ())
			nDeltaHeight += 3;

		// cmb2
		pCtrl = pParent->GetDlgItem(cmb2);
		if (pCtrl)
		{
			pCtrl->GetWindowRect (&rc1);
			pParent->ScreenToClient (&rc1);
			rc1.left = rc.left;
			pCtrl->SetWindowPos (NULL, rc1.left, rc1.top, rc1.Width (), rc1.Height (), SWP_NOZORDER);
		}

		SetControlText (stc2, _T(""));
		SetControlText (stc3, _T(""));
		SetControlText (stc4, _T(""));

		pParent->GetWindowRect (&rc);
		pParent->SetWindowPos (NULL, 0, 0, rc.Width (), rc.Height () + nDeltaHeight - 30, SWP_NOMOVE | SWP_NOZORDER);

		pParent->SetDlgItemText (IDOK, CString ((LPCTSTR)IDS_COMMON_BUTTON_CHOOSE_STR));
		pParent->SetDlgItemText (IDCANCEL, CString ((LPCTSTR)IDS_COMMON_BUTTON_CANCEL_STR));

		typedef HRESULT ( WINAPI *fnSHAutoComplete) ( HWND hwndEdit, DWORD dwFlags );

		HMODULE hShlwapi32 = ::LoadLibrary (_T("Shlwapi.dll"));
		if (hShlwapi32)
		{
			fnSHAutoComplete SHAutoComplete = (fnSHAutoComplete)::GetProcAddress (hShlwapi32, _T("SHAutoComplete"));

			if (SHAutoComplete)
				SHAutoComplete (m_FolderName, SHACF_FILESYSTEM | SHACF_URLALL | SHACF_FILESYS_ONLY);

			::FreeLibrary (hShlwapi32);
		}

		m_ParentBrowseDirWnd.ExternalInit ();
		
		// OK, Cancel
		pCtrl = pParent->GetDlgItem(IDCANCEL);
		if (pCtrl)
		{
			pCtrl->GetWindowRect(&rc);
			pParent->ScreenToClient(&rc);
			rc.OffsetRect(-(rc.Width() + 5), 0);
			pCtrl = pParent->GetDlgItem(IDOK);
			if (pCtrl)
			{
				pCtrl->ModifyStyle (0, BS_FLAT);
				pCtrl->SetWindowPos (pParent->GetDlgItem (IDC_WHITEBAR), rc.left, rc.top, 0, 0, SWP_NOSIZE);
			}
		}
	}
}

void CBrowseDirectoryNT4::OnFolderChange()
{
	CWnd *pParent = GetParent();

	if (pParent)
	{
		CWnd *pEdit = pParent->GetDlgItem (edt1);
		if (pEdit)
		{
			pEdit->SetWindowText (_T("")); // reset file name
			m_StrFolderName = GetPathName ();
			NormalizeFolderPath ( m_StrFolderName );
			m_FolderName.SetWindowText(m_StrFolderName);
		}
	}
}

BOOL CBrowseDirectoryNT4::OnFileNameOK()
{
	return 0;
}

void CBrowseDirectoryNT4::OnFileNameChange()
{
	m_StrFolderName = GetPathName ();
	NormalizeFolderPath (m_StrFolderName);
	m_FolderName.SetWindowText(m_StrFolderName);

	DoInitialUpdate ();
}

void CBrowseDirectoryNT4::DoInitialUpdate ()
{
	if (m_bFirstUpdate)
	{
		if (m_bInitialEmpty)
			m_FolderName.SetWindowText (_T(""));
		else if (!m_szInitialStr.IsEmpty ())
			m_FolderName.SetWindowText (m_szInitialStr);
		m_FolderName.SetSel (0, -1);
		m_FolderName.SetFocus ();
		m_bFirstUpdate = FALSE;
	}
}

void CBrowseDirectoryNT4::OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode)
{
}

CString CBrowseDirectoryNT4::GetPathName() const
{
	if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		CString strResult;

		{
			if (GetParent()->SendMessage(CDM_GETFILEPATH, (WPARAM)MAX_PATH,
				(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
			{
				strResult.Empty();
				return m_ofn.lpstrFile;
			}
			else
			{
				strResult.ReleaseBuffer();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// BUG #8578 FIX BEGIN
		// Проблема в том, что не под WinXP CDM_GETFILEPATH возвращает предыдушее значение,
		// если была выбрана папка.
		// (см. MSDN Magazine: C++ Q&A: OpenDlg Fixes Preview Problems)
		// Указанное решение осложняется тем, что имя файла в ListCtrl может быть без расширения
		// (для зарегистрированных типов).
		// Поэтому на то, что выбрана папка указывает то, что strFileName пуст или то, что
		// значение выбранного элемента не находится в левой части strFileName ()
		
		// Достанем выбранный элемент из ListCtrl со списком папок и файлов.
		if (m_ParentBrowseDirWnd.GetSafeHwnd() != NULL)
		{
			CWnd* pSHELLDLL_DefView = m_ParentBrowseDirWnd.GetDlgItem(lst2);
			if (pSHELLDLL_DefView != NULL)
			{
				CListCtrl* pLC = (CListCtrl*) pSHELLDLL_DefView->GetDlgItem(1);
				if (pLC != NULL)
				{
					int nSelItem = pLC->GetNextItem(-1, LVNI_SELECTED);
					if (nSelItem != -1)
					{
						// Определим имя выбранного файла
						CString strFileName;
						if (GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH,
							(LPARAM)strFileName.GetBuffer(MAX_PATH)) < 0)
							strFileName.Empty();
						else
							strFileName.ReleaseBuffer();

						// Определим имя выбранного элемента в ListCtrl
						CString strSelItem = pLC->GetItemText(nSelItem, 0);

						// Если не сходятся, значит это папка
						if (strFileName.IsEmpty()
							|| strSelItem.CompareNoCase(strFileName.Left(strSelItem.GetLength())) != 0)
						{
							CString strFolder;
							if (GetParent()->SendMessage(CDM_GETFOLDERPATH , (WPARAM)MAX_PATH,
								(LPARAM)strFolder.GetBuffer(MAX_PATH)) > 0)
							{
								strFolder.ReleaseBuffer();
								strFolder.TrimRight(_T("\\/"));
								strResult = strFolder + _T("\\") + strSelItem;
							}
						}
					}
				}
			}
		}
		// BUG #8578 FIX END
		//////////////////////////////////////////////////////////////////////////

		return strResult;
	}
	return m_ofn.lpstrFile;
}

void CBrowseDirectoryNT4::OnEditChangeReflect ()
{
	m_FolderName.GetWindowText (m_StrFolderName);
}

void CBrowseDirectoryNT4::OnSizeReflect (UINT nType, int cx, int cy)
{
	
}

BOOL CBrowseDirectoryNT4::OnSelOKReflect ()
{
	NormalizeFolderPath (m_StrFolderName);

	if (m_pRestriction)
	{
		CString szErrTitle, szErrText;
		if(!m_pRestriction->Finalize  (m_StrFolderName, szErrTitle, szErrText) )
		{
			if (!szErrText.IsEmpty  ())
			{
				CWnd* pParent = GetParent();
				HICON hIcon = pParent?(HICON)pParent->GetIcon  (true):NULL;
				MsgBoxEx(GetSafeHwnd(), szErrText, szErrTitle, MB_OK | MB_ICONERROR, hIcon);
			}
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseDirectoryNT4Wnd

CBrowseDirectoryNT4Wnd::CBrowseDirectoryNT4Wnd (CBrowseDirectoryNT4 *pFileDlg, UINT nHelpId)
	: m_pFileDlg (pFileDlg),
		CCWhiteBarDlg (0, nHelpId, NULL, DIALOG_FLAGS_MOVE_CONTEXT)
{
	AddCustomButton (IDCANCEL);
}

CBrowseDirectoryNT4Wnd::~CBrowseDirectoryNT4Wnd ()
{
}


BEGIN_MESSAGE_MAP(CBrowseDirectoryNT4Wnd, CWnd)
	//{{AFX_MSG_MAP(CBrowseDirectoryNT4Wnd)
	ON_CONTROL (EN_CHANGE, edt16, OnEditChange)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBrowseDirectoryNT4Wnd message handlers

BOOL CBrowseDirectoryNT4Wnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == 1)
	{
		if (m_pFileDlg->OnSelOKReflect ())
			::EndDialog (m_hWnd, IDOK);

		return TRUE;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

void CBrowseDirectoryNT4Wnd::OnEditChange()
{
	m_pFileDlg->OnEditChangeReflect ();
}

void CBrowseDirectoryNT4Wnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	CWnd *pEdit = GetDlgItem (edt16);
	CWnd *pList = GetDlgItem (lst2);
	
	if (pEdit && pList)
	{
		CRect rc, rc1;

		pList->GetWindowRect (&rc);
		pEdit->GetWindowRect (&rc1);
		pEdit->SetWindowPos (NULL, 0, 0, rc.Width (), rc1.Height (), SWP_NOMOVE | SWP_NOZORDER);
	}

	m_pFileDlg->OnSizeReflect (nType, cx, cy);
}
