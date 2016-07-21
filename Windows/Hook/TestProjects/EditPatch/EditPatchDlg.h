// EditPatchDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CEditPatchDlg dialog
class CEditPatchDlg : public CDialog
{
// Construction
public:
	CEditPatchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EDITPATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnpatch();
public:
	CEdit m_EditControl;
};
