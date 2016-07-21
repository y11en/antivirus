#if !defined(AFX_DCHANGES_H__0A1D0F60_2DE7_4824_9A33_6F547231F13D__INCLUDED_)
#define AFX_DCHANGES_H__0A1D0F60_2DE7_4824_9A33_6F547231F13D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DChanges.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDChanges dialog

class CDChanges : public CDialog
{
// Construction
public:
	CDChanges(CString* pNew, CString* pOld, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDChanges)
	enum { IDD = IDD_DIALOG_PATHCHANGED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDChanges)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDChanges)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_New;
	CString m_Old;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCHANGES_H__0A1D0F60_2DE7_4824_9A33_6F547231F13D__INCLUDED_)
