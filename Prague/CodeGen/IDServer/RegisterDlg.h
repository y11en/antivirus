#if !defined(AFX_REGISTERDLG_H__76454981_0946_11D4_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_REGISTERDLG_H__76454981_0946_11D4_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDlg.h : header file
//
#include <afxcmn.h>
#include <property/property.h>

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

class CRegisterDlg : public CDialog
{
// Construction
public:
	CRegisterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_REGISTER_DLG };
	HDATA m_hRootData;
	CSpinButtonCtrl	m_VIdSpin;
	CSpinButtonCtrl	m_PRToSpin;
	CSpinButtonCtrl	m_IRToSpin;
	CSpinButtonCtrl	m_PRFromSpin;
	CSpinButtonCtrl	m_IRFromSpin;
	CString	m_VName;
	UINT	m_IRFrom;
	UINT	m_IRTo;
	UINT	m_PRFrom;
	UINT	m_PRTo;
	UINT	m_VId;
	CString	m_VMnemID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDLG_H__76454981_0946_11D4_96B1_00D0B71DDDF6__INCLUDED_)
