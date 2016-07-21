#if !defined(AFX_MESSAGEBOX_H__5FFE7B73_DC34_4BCE_94CF_62A83E7DF893__INCLUDED_)
#define AFX_MESSAGEBOX_H__5FFE7B73_DC34_4BCE_94CF_62A83E7DF893__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// messagebox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog

class CMessageBox : public CDialog
{
// Construction
public:
	CMessageBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessageBox)
	enum { IDD = IDD_DLG_MESSAGE_BOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageBox)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEBOX_H__5FFE7B73_DC34_4BCE_94CF_62A83E7DF893__INCLUDED_)
