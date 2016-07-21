#if !defined(AFX_APPLICATIONRULEDLG_H__11F60E87_94A9_4EB7_A600_943BF5D7178F__INCLUDED_)
#define AFX_APPLICATIONRULEDLG_H__11F60E87_94A9_4EB7_A600_943BF5D7178F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApplicationRuleDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CApplicationRuleDlg dialog

class CApplicationRuleDlg : public CDialog
{
// Construction
public:
	CApplicationRuleDlg (CKAHFW::HAPPRULE hRule = NULL, CWnd* pParent = NULL);   // standard constructor
	~CApplicationRuleDlg ();

// Dialog Data
	//{{AFX_DATA(CApplicationRuleDlg)
	enum { IDD = IDD_APPRULE };
	CListBox	m_Elements;
	CString	m_Name;
	BOOL	m_Block;
	BOOL	m_Notify;
	BOOL	m_Log;
	BOOL	m_Enable;
	CString	m_AppPath;
	CString	m_CmdLine;
	BOOL	m_IsCmdLine;
	//}}AFX_DATA

	CKAHFW::HAPPRULE m_hRule;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplicationRuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void RefreshElementList();
    /*
	void FillAppRuleElment (CKAHFW::HAPPRULEELEMENT element, BOOL &bCheck, CListBox &RemAddr, CListBox &RemPort, CListBox &LocPort);
    */

	// Generated message map functions
	//{{AFX_MSG(CApplicationRuleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseApp();
	virtual void OnOK();
	afx_msg void OnAddElement();
	afx_msg void OnEditElement();
	afx_msg void OnDeleteElement();
	afx_msg void OnTimes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLICATIONRULEDLG_H__11F60E87_94A9_4EB7_A600_943BF5D7178F__INCLUDED_)
