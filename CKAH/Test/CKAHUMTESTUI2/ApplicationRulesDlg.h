#if !defined(AFX_APPLICATIONRULESDLG_H__4E673A21_132C_4454_B9BB_B4A3074D4CF9__INCLUDED_)
#define AFX_APPLICATIONRULESDLG_H__4E673A21_132C_4454_B9BB_B4A3074D4CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApplicationRulesDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CApplicationRulesDlg dialog

class CApplicationRulesDlg : public CDialog
{
// Construction
public:
	CApplicationRulesDlg (CKAHFW::HAPPRULES hRules, CWnd* pParent = NULL);   // standard constructor
	~CApplicationRulesDlg ();

// Dialog Data
	//{{AFX_DATA(CApplicationRulesDlg)
	enum { IDD = IDD_APPRULES };
	CListBox	m_RuleListCtrl;
	BOOL	m_RecalcHashes;
	//}}AFX_DATA

	CKAHFW::HAPPRULES m_hRules;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplicationRulesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void RefreshList ();

	// Generated message map functions
	//{{AFX_MSG(CApplicationRulesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddApplicationRule();
	afx_msg void OnEditApplicationRule();
	afx_msg void OnDeleteApplicationRule();
	afx_msg void OnRestoreApplicationRule();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLICATIONRULESDLG_H__4E673A21_132C_4454_B9BB_B4A3074D4CF9__INCLUDED_)
