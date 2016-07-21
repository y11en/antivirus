#if !defined(AFX_RULETIMEELT_H__77BC67E5_C9DD_4271_8730_AFA6256906E9__INCLUDED_)
#define AFX_RULETIMEELT_H__77BC67E5_C9DD_4271_8730_AFA6256906E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuleTimeEltDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CRuleTimeEltDlg dialog

class CRuleTimeEltDlg : public CDialog
{
// Construction
public:
	CRuleTimeEltDlg(CKAHFW::HRULETIMEELEMENT hTimeElt = NULL, CWnd* pParent = NULL);   // standard constructor
    virtual ~CRuleTimeEltDlg();

// Dialog Data
	//{{AFX_DATA(CRuleTimeEltDlg)
	enum { IDD = IDD_TIMEELT };
	DWORD	m_TimeMax;
	DWORD	m_TimeMin;
	//}}AFX_DATA

	CKAHFW::HRULETIMEELEMENT m_hTimeElt;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuleTimeEltDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRuleTimeEltDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RULETIMEELT_H__77BC67E5_C9DD_4271_8730_AFA6256906E9__INCLUDED_)
