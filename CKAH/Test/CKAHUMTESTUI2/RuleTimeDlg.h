#if !defined(AFX_RULETIME_H__F6A832E6_CB9B_45F6_A6F5_95B33CB54A64__INCLUDED_)
#define AFX_RULETIME_H__F6A832E6_CB9B_45F6_A6F5_95B33CB54A64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuleTimeDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CRuleTimeDlg dialog

class CRuleTimeDlg : public CDialog
{
// Construction
public:
	CRuleTimeDlg(CKAHFW::HRULETIME hTime = NULL, CWnd* pParent = NULL);   // standard constructor
    virtual ~CRuleTimeDlg ();

// Dialog Data
	//{{AFX_DATA(CRuleTimeDlg)
	enum { IDD = IDD_TIME };
	CListBox	m_TimeList;
	//}}AFX_DATA

	CKAHFW::HRULETIME m_hTime;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuleTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void RefreshList();

	// Generated message map functions
	//{{AFX_MSG(CRuleTimeDlg)
	afx_msg void OnAddTimeElt();
	afx_msg void OnEditTimeElt();
	afx_msg void OnDeleteTimeElt();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RULETIME_H__F6A832E6_CB9B_45F6_A6F5_95B33CB54A64__INCLUDED_)
