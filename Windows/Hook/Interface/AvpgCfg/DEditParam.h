#if !defined(AFX_DEDITPARAM_H__B5E64616_8B93_495D_AFCE_58AB1FA3AB15__INCLUDED_)
#define AFX_DEDITPARAM_H__B5E64616_8B93_495D_AFCE_58AB1FA3AB15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DEditParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDEditParam dialog

class CDEditParam : public CDialog
{
// Construction
public:
	CDEditParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDEditParam)
	enum { IDD = IDD_DIALOG_EDITPARAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDEditParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDEditParam)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEDITPARAM_H__B5E64616_8B93_495D_AFCE_58AB1FA3AB15__INCLUDED_)
