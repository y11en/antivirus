#if !defined(AFX_DFOLDERS_H__FD888948_4AE7_422D_B601_95EA4FD8F141__INCLUDED_)
#define AFX_DFOLDERS_H__FD888948_4AE7_422D_B601_95EA4FD8F141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFolders.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DFolders dialog

class CDFolders : public CDialog
{
// Construction
public:
	CDFolders(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDFolders)
	enum { IDD = IDD_DIALOG_FOLDERS };
	CListCtrl	m_lsFolders;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFolders)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFolders)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnEnter();
	afx_msg long OnEdit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDblclkListFolders(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg long OnAddnew(WPARAM wParam, LPARAM lparam);
	afx_msg long OnDelete(WPARAM wParam, LPARAM lparam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Refresh();
	BOOL DeleteFiltersFromList(CString *pFolder);
	HACCEL m_hAccel;
	BOOL m_bShow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFOLDERS_H__FD888948_4AE7_422D_B601_95EA4FD8F141__INCLUDED_)
