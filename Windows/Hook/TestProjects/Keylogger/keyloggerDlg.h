// keyloggerDlg.h : header file
//

#if !defined(AFX_KEYLOGGERDLG_H__1E1F3078_698A_40C0_8A96_ADBFD5E5182B__INCLUDED_)
#define AFX_KEYLOGGERDLG_H__1E1F3078_698A_40C0_8A96_ADBFD5E5182B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerDlg dialog

class CKeyloggerDlg : public CDialog
{
// Construction
public:
	CKeyloggerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyloggerDlg)
	enum { IDD = IDD_KEYLOGGER_DIALOG };
	CButton	m_BtnKdSpliceStop;
	CButton	m_BtnKdSpliceStart;
	CButton	m_BtnKdWin32kStop;
	CButton	m_BtnKdWin32kStart;
	CButton	m_BtnKdDispatchStop;
	CButton	m_BtnKdDispatchStart;
	CButton	m_BtnKd4Stop;
	CButton	m_BtnKd4Start;
	CButton	m_BtnKd3Stop;
	CButton	m_BtnKd3Start;
	CButton	m_BtnKd1Stop;
	CButton	m_BtnKd1Start;
	CButton	m_BtnKd2Stop;
	CButton	m_BtnKd2Start;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyloggerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKeyloggerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnKd1Start();
	afx_msg void OnBtnKd1Stop();
	afx_msg void OnBtnKd2Start();
	afx_msg void OnBtnKd2Stop();
	afx_msg void OnBtnKd3Start();
	afx_msg void OnBtnKd3Stop();
	afx_msg void OnBtnKd4Stop();
	afx_msg void OnBtnKd4Start();
	afx_msg void OnBtnKdwin32kStart();
	afx_msg void OnBtnKdwin32kStop();
	afx_msg void OnBtnKddispatchStart();
	afx_msg void OnBtnKddispatchStop();
	afx_msg void OnClose();
	afx_msg void OnBtnKd1About();
	afx_msg void OnBtnKd2About();
	afx_msg void OnBtnKd3About();
	afx_msg void OnBtnKd4About();
	afx_msg void OnBtnKdwin32kAbout();
	afx_msg void OnBtnKddispatchAbout();
	afx_msg void OnBtnKdspliceStart();
	afx_msg void OnBtnKdspliceStop();
	afx_msg void OnBtnKdspliceAbout();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HMODULE m_hKD1DLL;

	PTCHAR GetAppPath();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYLOGGERDLG_H__1E1F3078_698A_40C0_8A96_ADBFD5E5182B__INCLUDED_)
