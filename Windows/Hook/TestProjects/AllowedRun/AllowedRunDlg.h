// AllowedRunDlg.h : header file
//

#if !defined(AFX_ALLOWEDRUNDLG_H__108A8C8D_5EA6_4D0C_9DFD_C0FCBD5D4C8C__INCLUDED_)
#define AFX_ALLOWEDRUNDLG_H__108A8C8D_5EA6_4D0C_9DFD_C0FCBD5D4C8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OwnTabCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CAllowedRunDlg dialog

class CAllowedRunDlg : public CDialog
{
// Construction
public:
	CAllowedRunDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAllowedRunDlg)
	enum { IDD = IDD_ALLOWEDRUN_DIALOG };
	COwnTabCtrl	m_tabMain;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllowedRunDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAllowedRunDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNexttab();
	afx_msg void OnPrevtab();
	afx_msg void OnEnter();
	afx_msg void OnButtonAddnew();
	afx_msg void OnButtonDelete();
	afx_msg void OnExit();
	afx_msg void OnButtonEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HACCEL m_hAccel;

	void RestorePosition();
	void SavePosition();

	CRect m_TabRect;
	PHashTree m_PHash_TabWindow;
	void TabResize();
	void SelectTab(int nNewTab);

	void SendMsgToCurrentPage(DWORD wuMessage);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLOWEDRUNDLG_H__108A8C8D_5EA6_4D0C_9DFD_C0FCBD5D4C8C__INCLUDED_)
