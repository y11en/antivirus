#if !defined(AFX_DFOLDER_H__14C11160_721C_4A72_81EB_7F42062A9CF7__INCLUDED_)
#define AFX_DFOLDER_H__14C11160_721C_4A72_81EB_7F42062A9CF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFolder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDFolder dialog

class CDFolder : public CDialog
{
// Construction
public:
	CDFolder(CString* pFolder, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDFolder)
	enum { IDD = IDD_DIALOG_EDIT_FOLDER };
	CString	m_eFolder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFolder)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString* m_pFolder;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFOLDER_H__14C11160_721C_4A72_81EB_7F42062A9CF7__INCLUDED_)
