#if !defined(AFX_APPLICATIONRULEELEMENTDLG_H__CFF82751_B550_44ED_A971_8A30BDAAFA02__INCLUDED_)
#define AFX_APPLICATIONRULEELEMENTDLG_H__CFF82751_B550_44ED_A971_8A30BDAAFA02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApplicationRuleElementDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CApplicationRuleElementDlg dialog

class CApplicationRuleElementDlg : public CDialog
{
// Construction
public:
    CApplicationRuleElementDlg(CKAHFW::HAPPRULEELEMENT hElement = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CApplicationRuleElementDlg)
	enum { IDD = IDD_APPRULEELT };
	CListBox	m_LocalPorts;
	CListBox	m_RemotePorts;
	CListBox	m_RemoteAddresses;
	UINT	m_Proto;
	BOOL	m_Enabled;
	//}}AFX_DATA

    CKAHFW::HAPPRULEELEMENT m_hElement;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplicationRuleElementDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CApplicationRuleElementDlg)
    virtual BOOL OnInitDialog();
	afx_msg void OnRemoteAddress(UINT id);
	afx_msg void OnRemotePort(UINT id);
	afx_msg void OnLocalPort(UINT id);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPLICATIONRULEELEMENTDLG_H__CFF82751_B550_44ED_A971_8A30BDAAFA02__INCLUDED_)
