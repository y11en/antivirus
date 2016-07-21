// MultiThreadSwitchDlg.h : header file
//

#if !defined(AFX_MULTITHREADSWITCHDLG_H__267F49D6_7EC1_4772_A98A_B96EC80A2006__INCLUDED_)
#define AFX_MULTITHREADSWITCHDLG_H__267F49D6_7EC1_4772_A98A_B96EC80A2006__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMultiThreadSwitchDlg dialog

class CMultiThreadSwitchDlg : public CDialog
{
// Construction
public:
	CMultiThreadSwitchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMultiThreadSwitchDlg)
	enum { IDD = IDD_MULTITHREADSWITCH_DIALOG };
	int		m_eCount;
	int		m_RegsCnt;
	int		m_ThCnt;
	DWORD	m_RegSize;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiThreadSwitchDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMultiThreadSwitchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSs();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnOk();
	virtual void OnOK();
	afx_msg void OnButtonPause();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTITHREADSWITCHDLG_H__267F49D6_7EC1_4772_A98A_B96EC80A2006__INCLUDED_)
