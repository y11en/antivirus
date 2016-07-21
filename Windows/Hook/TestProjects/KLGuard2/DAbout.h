#if !defined(AFX_DABOUT_H__DAE658F9_7B0C_434D_B35E_01EA97C1A2B3__INCLUDED_)
#define AFX_DABOUT_H__DAE658F9_7B0C_434D_B35E_01EA97C1A2B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DAbout.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDAbout dialog

class CDAbout : public CDialog
{
// Construction
public:
	CDAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDAbout)
	enum { IDD = IDD_DIALOG_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDAbout)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DABOUT_H__DAE658F9_7B0C_434D_B35E_01EA97C1A2B3__INCLUDED_)
