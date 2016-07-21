#if !defined(AFX_IMPORTDEFFILEDLG_H__AAD90E44_E473_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_IMPORTDEFFILEDLG_H__AAD90E44_E473_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"

// ImportDefFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportDefFileDlg dialog

class CImportDefFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CImportDefFileDlg)

public:
	BOOL  m_bLinkToFile;

	CImportDefFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CImportDefFileDlg)
	afx_msg void OnLinkToFile();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTDEFFILEDLG_H__AAD90E44_E473_11D3_96B1_00D0B71DDDF6__INCLUDED_)
