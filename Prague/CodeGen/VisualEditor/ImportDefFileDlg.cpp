// ImportDefFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImportDefFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportDefFileDlg

IMPLEMENT_DYNAMIC(CImportDefFileDlg, CFileDialog)

CImportDefFileDlg::CImportDefFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_bLinkToFile = TRUE;
}


BEGIN_MESSAGE_MAP(CImportDefFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(CImportDefFileDlg)
	ON_BN_CLICKED(IDC_IDF_LINKTOFILE_CBTN, OnLinkToFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ---
BOOL CImportDefFileDlg::OnInitDialog() {
	CFileDialog::OnInitDialog();
	
	CheckDlgButton( IDC_IDF_LINKTOFILE_CBTN, m_bLinkToFile ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// ---
void CImportDefFileDlg::OnLinkToFile() {
	m_bLinkToFile = !m_bLinkToFile;
}


