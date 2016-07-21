// KLGuard2Dlg.h : header file
//

#if !defined(AFX_KLGUARD2DLG_H__79EF7E83_A960_4743_876F_14C187B3B5FB__INCLUDED_)
#define AFX_KLGUARD2DLG_H__79EF7E83_A960_4743_876F_14C187B3B5FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Processes.h"

/////////////////////////////////////////////////////////////////////////////
// CKLGuard2Dlg dialog

class CKLGuard2Dlg : public CDialog
{
// Construction
public:
	CKLGuard2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKLGuard2Dlg)
	enum { IDD = IDD_KLGUARD2_DIALOG };
	CListCtrl	m_lProcesses;
	BOOL	m_chEducationMode;
	BOOL	m_chStartKnownProcesses;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKLGuard2Dlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKLGuard2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg long OnHyperLink(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg long OnTrayIocn(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	afx_msg void OnExit();
	afx_msg void OnDblclkListProcesses(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckEducationmode();
	afx_msg void OnCheckStartKnownProcesses();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg long OnAlert(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void TimerMain();
	void TimerTrayBlink();

	CHyperLink*	m_pSandBox;
	CHyperLink*	m_pGeneriSettings;
	CHyperLink*	m_pKnownProcesses;
	CHyperLink*	m_pAbout;

	CProcesses m_Processes;

	void SavePosition();
	void RestorePosition();

	LONG m_TrayBlinkCounter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KLGUARD2DLG_H__79EF7E83_A960_4743_876F_14C187B3B5FB__INCLUDED_)
