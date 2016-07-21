// Create_AuditDlg.h : header file
//

#if !defined(AFX_CREATE_AUDITDLG_H__BE4BF03E_9EDF_4E4B_A9D0_22D31FEAB6DF__INCLUDED_)
#define AFX_CREATE_AUDITDLG_H__BE4BF03E_9EDF_4E4B_A9D0_22D31FEAB6DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCreate_AuditDlg dialog

class CCreate_AuditDlg : public CDialog
{
// Construction
public:
	CCreate_AuditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCreate_AuditDlg)
	enum { IDD = IDD_CREATE_AUDIT_DIALOG };
	CString	m_ePath;
	BOOL	m_chAutoHide;
	BOOL	m_chAutoStart;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreate_AuditDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCreate_AuditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonStart();
	afx_msg long OnNewData(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExit();
	afx_msg void OnShow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HANDLE			m_hEvWaiterThread;
	THREAD_PARAM	m_ThreadParam;
	OSVERSIONINFO	m_OSVer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATE_AUDITDLG_H__BE4BF03E_9EDF_4E4B_A9D0_22D31FEAB6DF__INCLUDED_)
