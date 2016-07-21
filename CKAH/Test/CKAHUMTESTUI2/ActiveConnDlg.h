#if !defined(AFX_ACTIVECONNDLG_H__DF411908_22A7_43FD_96FA_30E8C38A0CF4__INCLUDED_)
#define AFX_ACTIVECONNDLG_H__DF411908_22A7_43FD_96FA_30E8C38A0CF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActiveConnDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CActiveConnDlg dialog

class CActiveConnDlg : public CDialog
{
// Construction
public:
	VOID FillDlg();
	CActiveConnDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CActiveConnDlg)
	enum { IDD = IDD_ConnDialog };
	CButton	m_CANCEL;
	CButton	m_OK;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActiveConnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CActiveConnDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVECONNDLG_H__DF411908_22A7_43FD_96FA_30E8C38A0CF4__INCLUDED_)
