#if !defined(AFX_DWARNINGNOFILTERS_H__AEE51F8A_23E4_48FC_A810_30B963B86867__INCLUDED_)
#define AFX_DWARNINGNOFILTERS_H__AEE51F8A_23E4_48FC_A810_30B963B86867__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DWarningNoFilters.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDWarningNoFilters dialog

class CDWarningNoFilters : public CDialog
{
// Construction
public:
	CDWarningNoFilters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDWarningNoFilters)
	enum { IDD = IDD_DIALOG_WARNING_NO_FILTERS };
	BOOL	m_chDontShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWarningNoFilters)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDWarningNoFilters)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWARNINGNOFILTERS_H__AEE51F8A_23E4_48FC_A810_30B963B86867__INCLUDED_)
