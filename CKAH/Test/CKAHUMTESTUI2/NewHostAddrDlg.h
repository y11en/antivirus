#if !defined(AFX_NEWHOSTADDRDLG_H__E1215824_FF8B_4247_A2A8_7BC1F72445A9__INCLUDED_)
#define AFX_NEWHOSTADDRDLG_H__E1215824_FF8B_4247_A2A8_7BC1F72445A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewHostAddrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewHostAddrDlg dialog

class CNewHostAddrDlg : public CDialog
{
// Construction
public:
	CNewHostAddrDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewHostAddrDlg)
	enum { IDD = IDD_NEWADDRESS };
	CString	m_HostIPorName;
	//}}AFX_DATA

	CDWordArray m_IPs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewHostAddrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewHostAddrDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWHOSTADDRDLG_H__E1215824_FF8B_4247_A2A8_7BC1F72445A9__INCLUDED_)
