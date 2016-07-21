#if !defined(AFX_DINFO_H__A9F9F847_63F7_4CCC_B4DE_095F52395FE1__INCLUDED_)
#define AFX_DINFO_H__A9F9F847_63F7_4CCC_B4DE_095F52395FE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDInfo dialog

class CDInfo : public CDialog
{
// Construction
public:
	CDInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDInfo)
	enum { IDD = IDD_DIALOG_INFO };
	CCheckList	m_lKeysInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchangedListKeysinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DINFO_H__A9F9F847_63F7_4CCC_B4DE_095F52395FE1__INCLUDED_)
