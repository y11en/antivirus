#if !defined(AFX_LARGECOMMENTDLG_H__9C76DAC1_C9A8_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_LARGECOMMENTDLG_H__9C76DAC1_C9A8_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LargeCommentDlg.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLargeCommentDlg dialog

class CLargeCommentDlg : public CDialog
{
// Construction
public:
	CLargeCommentDlg(CString &rcComment, DWORD dwTitleID, BOOL bReadOnly, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLargeCommentDlg)
	enum { IDD = IDD_LARGECOMMENT_DLG };
	CEdit	  m_CommentEdit;
	CString	m_Comment;
	CFont   m_EditFont;
	BOOL		m_bReadOnly;
	DWORD   m_dwTitleID;
	//}}AFX_DATA

	CString &m_rcSourceText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLargeCommentDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLargeCommentDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFontButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LARGECOMMENTDLG_H__9C76DAC1_C9A8_11D3_96B1_00D0B71DDDF6__INCLUDED_)
