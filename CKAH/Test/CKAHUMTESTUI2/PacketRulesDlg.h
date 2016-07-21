#if !defined(AFX_PACKETRULESDLG_H__039B7F3C_0442_4031_B573_C18442C0BDE5__INCLUDED_)
#define AFX_PACKETRULESDLG_H__039B7F3C_0442_4031_B573_C18442C0BDE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PacketRulesDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CPacketRulesDlg dialog

class CPacketRulesDlg : public CDialog
{

// Construction
public:
	CPacketRulesDlg(CKAHFW::HPACKETRULES hRules, CWnd* pParent = NULL);   // standard constructor
	~CPacketRulesDlg ();

// Dialog Data
	//{{AFX_DATA(CPacketRulesDlg)
	enum { IDD = IDD_PACKETRULES };
	CListBox	m_RuleListCtrl;
	//}}AFX_DATA

	CKAHFW::HPACKETRULES m_hRules;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPacketRulesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void RefreshList ();

	// Generated message map functions
	//{{AFX_MSG(CPacketRulesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddPacketRule();
	afx_msg void OnEditPacketRule();
	afx_msg void OnDeletePacketRule();
	afx_msg void OnRestorePacketRule();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKETRULESDLG_H__039B7F3C_0442_4031_B573_C18442C0BDE5__INCLUDED_)
