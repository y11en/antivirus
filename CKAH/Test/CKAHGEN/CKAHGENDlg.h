// CKAHGENDlg.h : header file
//

#if !defined(AFX_CKAHGENDLG_H__AB8C5A32_7C20_42DA_A977_D83DAD40A92E__INCLUDED_)
#define AFX_CKAHGENDLG_H__AB8C5A32_7C20_42DA_A977_D83DAD40A92E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCKAHGENDlg dialog

class CCKAHGENDlg : public CDialog
{
// Construction
public:
	CCKAHGENDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCKAHGENDlg)
	enum { IDD = IDD_CKAHGEN_DIALOG };
	CIPAddressCtrl	m_IPCtrl;
	UINT	m_AttackID;
	short	m_Proto;
	short	m_Port;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCKAHGENDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCKAHGENDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEmulate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CKAHGENDLG_H__AB8C5A32_7C20_42DA_A977_D83DAD40A92E__INCLUDED_)
