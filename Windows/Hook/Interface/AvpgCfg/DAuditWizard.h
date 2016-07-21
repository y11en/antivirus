#if !defined(AFX_DAUDITWIZARD_H__AA1D6E20_CAC5_4045_90C8_DCD86992312F__INCLUDED_)
#define AFX_DAUDITWIZARD_H__AA1D6E20_CAC5_4045_90C8_DCD86992312F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DAuditWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDAuditWizard dialog

class CDAuditWizard : public CDialog
{
// Construction
public:
	CDAuditWizard(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDAuditWizard)
	enum { IDD = IDD_DIALOG_OAS };
	CString	m_eLogFilename;
	BOOL	m_fo_close;
	BOOL	m_fo_create;
	BOOL	m_fo_read;
	BOOL	m_fo_write;
	BOOL	m_reg_create;
	BOOL	m_reg_open;
	BOOL	m_reg_query;
	BOOL	m_reg_setval;
	CString	m_fo_mask;
	CString	m_reg_mask;
	CString	m_ProcName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDAuditWizard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDAuditWizard)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAUDITWIZARD_H__AA1D6E20_CAC5_4045_90C8_DCD86992312F__INCLUDED_)
