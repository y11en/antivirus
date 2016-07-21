#if !defined(AFX_PORTDLG_H__9D022369_629B_4207_938A_976D09F54108__INCLUDED_)
#define AFX_PORTDLG_H__9D022369_629B_4207_938A_976D09F54108__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PortDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CPortDlg dialog

class CPortDlg : public CDialog
{
// Construction
public:
	CPortDlg (CKAHFW::HRULEPORT hPort, CWnd* pParent = NULL);   // standard constructor
	~CPortDlg ();

// Dialog Data
	//{{AFX_DATA(CPortDlg)
	enum { IDD = IDD_PORT };
	UINT	m_SinglePortValue;
	UINT	m_RangeLoValue;
	UINT	m_RangeHiValue;
	//}}AFX_DATA

	CKAHFW::HRULEPORT m_hPort;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPortDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PORTDLG_H__9D022369_629B_4207_938A_976D09F54108__INCLUDED_)
