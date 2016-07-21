#if !defined(AFX_RULETIMES_H__BACA7BB8_393C_48D0_A121_9963ECA5E17F__INCLUDED_)
#define AFX_RULETIMES_H__BACA7BB8_393C_48D0_A121_9963ECA5E17F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuleTimesDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CRuleTimesDlg dialog

class CRuleTimesDlg : public CDialog
{
// Construction
public:
	CRuleTimesDlg(CKAHFW::HRULETIMES hTimes = NULL, CWnd* pParent = NULL);   // standard constructor
    virtual ~CRuleTimesDlg ();

// Dialog Data
	//{{AFX_DATA(CRuleTimesDlg)
	enum { IDD = IDD_TIMES };
	CListBox	m_TimesList;
	//}}AFX_DATA

	CKAHFW::HRULETIMES m_hTimes;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuleTimesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void RefreshList();

	// Generated message map functions
	//{{AFX_MSG(CRuleTimesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddTime();
	afx_msg void OnEditTime();
	afx_msg void OnDeleteTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RULETIMES_H__BACA7BB8_393C_48D0_A121_9963ECA5E17F__INCLUDED_)
