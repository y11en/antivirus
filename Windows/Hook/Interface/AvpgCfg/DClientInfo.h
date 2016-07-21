#if !defined(AFX_DCLIENTINFO_H__112A13F6_98EE_4CBF_BF4A_7E6B25FDA728__INCLUDED_)
#define AFX_DCLIENTINFO_H__112A13F6_98EE_4CBF_BF4A_7E6B25FDA728__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DClientInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDClientInfo dialog

class CDClientInfo : public CDialog
{
// Construction
public:
	CDClientInfo(HANDLE hDevice, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDClientInfo)
	enum { IDD = IDD_DIALOG_CLIENT_INFO };
	CComboBox	m_cbStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDClientInfo)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDClientInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonRefresh();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDropdownComboStatus();
	afx_msg void OnSelendokComboStatus();
	afx_msg void OnSelendcancelComboStatus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_hDevice;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCLIENTINFO_H__112A13F6_98EE_4CBF_BF4A_7E6B25FDA728__INCLUDED_)
