// FindDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog
#ifndef __CFINDDLG_H_
#define __CFINDDLG_H_

class CFindDlg : public CDialog
{
// Construction
public:
	CFindDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_FIND };
	CString	m_String;
	BOOL	m_Case;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif// __CFINDDLG_H_
