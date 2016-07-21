// ScreenshotDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CScreenshotDlg dialog
class CScreenshotDlg : public CDialog
{
// Construction
public:
	CScreenshotDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCREENSHOT_DIALOG };

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
	afx_msg void OnNMCustomdrawView(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CStatic m_ImgCtrl;
public:
	afx_msg void OnBnClickedBtnshoot();
public:
	afx_msg void OnBnClickedTest2();
};
