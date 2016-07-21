// virt_guiDlg.h : header file
//

#pragma once

#include "Virt.h"

// Cvirt_guiDlg dialog
class Cvirt_guiDlg : public CDialog
{
// Construction
public:
	Cvirt_guiDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VIRT_GUI_DIALOG };
	
	Virt virt_cl;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnInitVirt();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnAdd();
};
