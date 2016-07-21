#if !defined(AFX_DSANDBOXWARNINGS_H__AFB0CDC2_24E5_4055_A889_E0D9D6B3D7F1__INCLUDED_)
#define AFX_DSANDBOXWARNINGS_H__AFB0CDC2_24E5_4055_A889_E0D9D6B3D7F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSandBoxWarnings.h : header file
//
#include "stdafx.h"
#include "resource.h"
#include "StrHistory.h"
/////////////////////////////////////////////////////////////////////////////
// CDSandBoxWarnings dialog

class CDSandBoxWarnings : public CDialog
{
// Construction
public:
	CDSandBoxWarnings(CStrHistory *pStrHistory, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDSandBoxWarnings)
	enum { IDD = IDD_DIALOG_SANDBOX_WARNINGS };
	CString	m_eWanring;
	UINT	m_WarningsRemain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSandBoxWarnings)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDSandBoxWarnings)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonAllok();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CStrHistory* m_pStrHistory;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSANDBOXWARNINGS_H__AFB0CDC2_24E5_4055_A889_E0D9D6B3D7F1__INCLUDED_)
