//==============================================================================
// PRODUCT: GUI Extension
//==============================================================================
// AUTHOR:  Timur I. Amirkhanov
//==============================================================================
#include "stdafx.h"

#include "FileDlg.h"
#include "FileDialogImpl.h"

#if _MSC_VER >= 1300

CCFileDialog::CCFileDialog(BOOL bOpenFileDialog,
		LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
		LPCTSTR lpszFilter, CWnd* pParentWnd, DWORD dwSize) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, dwSize)
{
}

void CCFileDialog::SetDefExts(LPCTSTR* lpDefExts, int nCount)
{
	m_aDefExts.RemoveAll();
	for (; nCount>0; nCount--, lpDefExts++)
		m_aDefExts.Add(*lpDefExts);
}

INT_PTR CCFileDialog::DoModal()
{
	if (m_aDefExts.GetSize() > 0)
		m_ofn.lpstrDefExt = m_aDefExts[0];
	return CFileDialog::DoModal();
}

void CCFileDialog::OnTypeChange()
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

#else

////////////////////////////////////////////////////////////////////////////
// FileOpen/FileSaveAs common dialog helper

CCFileDialog::CCFileDialog(BOOL bOpenFileDialog,
                           LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags,
                           LPCTSTR lpszFilter, CWnd* pParentWnd) : CCommonDialog(pParentWnd)
{
	memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
	pImpl = CCFileDialogImpl::CreateImpl(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd);
	GetImplOFN();
}

CCFileDialog::~CCFileDialog()
{
	CCFileDialogImpl::DeleteImpl(pImpl);
}

int CCFileDialog::DoModal()
{
	SetImplOFN();
	int nResult = pImpl->DoModal  ();
	GetImplOFN();

	return nResult;
}

CString CCFileDialog::GetPathName() const
{
	return pImpl->GetPathName  ();
}

CString CCFileDialog::GetFileName() const
{
	return pImpl->GetFileName  ();
}

CString CCFileDialog::GetFileExt() const
{
	return pImpl->GetFileExt  ();
}

CString CCFileDialog::GetFileTitle() const
{
	return pImpl->GetFileTitle  ();
}

POSITION CCFileDialog::GetStartPosition () const
{
	return pImpl->GetStartPosition();
}

CString CCFileDialog::GetNextPathName(POSITION& pos) const
{
	return pImpl->GetNextPathName(pos);
}

void CCFileDialog::SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID)
{
	pImpl->SetTemplate(lpWin3ID, lpWin4ID);
}

CString CCFileDialog::GetFolderPath() const
{
	return pImpl->GetFolderPath  ();
}

void CCFileDialog::SetControlText(int nID, LPCSTR lpsz)
{
	pImpl->SetControlText(nID, lpsz);
}

void CCFileDialog::HideControl(int nID)
{
	pImpl->HideControl(nID);
}

void CCFileDialog::SetDefExt(LPCSTR lpsz)
{
	pImpl->SetDefExt(lpsz);
}

void CCFileDialog::SetDefExts(LPCTSTR* lpDefExts, int nCount)
{
	pImpl->SetDefExts(lpDefExts, nCount);
}

void CCFileDialog::GetImplOFN()
{
	m_ofn.lStructSize = sizeof(m_ofn);
	m_ofn.hwndOwner = pImpl->m_ofn.hwndOwner;
	m_ofn.hInstance = pImpl->m_ofn.hInstance;
	m_ofn.lpstrFilter = pImpl->m_ofn.lpstrFilter;
	m_ofn.lpstrCustomFilter = pImpl->m_ofn.lpstrCustomFilter;
	m_ofn.nMaxCustFilter = pImpl->m_ofn.nMaxCustFilter;
	m_ofn.nFilterIndex = pImpl->m_ofn.nFilterIndex;
	m_ofn.lpstrFile = pImpl->m_ofn.lpstrFile;
	m_ofn.nMaxFile = pImpl->m_ofn.nMaxFile;
	m_ofn.lpstrFileTitle = pImpl->m_ofn.lpstrFileTitle;
	m_ofn.nMaxFileTitle = pImpl->m_ofn.nMaxFileTitle;
	m_ofn.lpstrInitialDir = pImpl->m_ofn.lpstrInitialDir;
	m_ofn.lpstrTitle = pImpl->m_ofn.lpstrTitle;
	m_ofn.Flags = pImpl->m_ofn.Flags;
	m_ofn.nFileOffset = pImpl->m_ofn.nFileOffset;
	m_ofn.nFileExtension = pImpl->m_ofn.nFileExtension;
	m_ofn.lpstrDefExt = pImpl->m_ofn.lpstrDefExt;
	m_ofn.lCustData = pImpl->m_ofn.lCustData;
	m_ofn.lpfnHook = pImpl->m_ofn.lpfnHook;
	m_ofn.lpTemplateName = pImpl->m_ofn.lpTemplateName;
}

void CCFileDialog::SetImplOFN()
{
	pImpl->m_ofn.hwndOwner = m_ofn.hwndOwner;
	pImpl->m_ofn.hInstance = m_ofn.hInstance;
	pImpl->m_ofn.lpstrFilter = m_ofn.lpstrFilter;
	pImpl->m_ofn.lpstrCustomFilter = m_ofn.lpstrCustomFilter;
	pImpl->m_ofn.nMaxCustFilter = m_ofn.nMaxCustFilter;
	pImpl->m_ofn.nFilterIndex = m_ofn.nFilterIndex;
	pImpl->m_ofn.lpstrFile = m_ofn.lpstrFile;
	pImpl->m_ofn.nMaxFile = m_ofn.nMaxFile;
	pImpl->m_ofn.lpstrFileTitle = m_ofn.lpstrFileTitle;
	pImpl->m_ofn.nMaxFileTitle = m_ofn.nMaxFileTitle;
	pImpl->m_ofn.lpstrInitialDir = m_ofn.lpstrInitialDir;
	pImpl->m_ofn.lpstrTitle = m_ofn.lpstrTitle;
	pImpl->m_ofn.Flags = m_ofn.Flags;
	pImpl->m_ofn.nFileOffset = m_ofn.nFileOffset;
	pImpl->m_ofn.nFileExtension = m_ofn.nFileExtension;
	pImpl->m_ofn.lpstrDefExt = m_ofn.lpstrDefExt;
	pImpl->m_ofn.lCustData = m_ofn.lCustData;
	pImpl->m_ofn.lpfnHook = m_ofn.lpfnHook;
	pImpl->m_ofn.lpTemplateName = m_ofn.lpTemplateName;
}

IMPLEMENT_DYNAMIC(CCFileDialog, CDialog)

//==============================================================================
#endif