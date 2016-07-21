#if !defined(AFX_NEWINTERFACENAMEDLG_H__B976AF81_BEDD_11D4_96B3_444553540000__INCLUDED_)
#define AFX_NEWINTERFACENAMEDLG_H__B976AF81_BEDD_11D4_96B3_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewInterfaceNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewInterfaceNameDlg dialog
// ---
class CNewInterfaceNameDlg : public CDialog {
// Construction
public:
	CNewInterfaceNameDlg( CString &rcInterfaceName, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewInterfaceNameDlg)
	enum { IDD = IDD_INTERFACENAME_DLG };
	CString	m_IntName;
	//}}AFX_DATA

	CString &m_rcInterfaceName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewInterfaceNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewInterfaceNameDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWINTERFACENAMEDLG_H__B976AF81_BEDD_11D4_96B3_444553540000__INCLUDED_)
