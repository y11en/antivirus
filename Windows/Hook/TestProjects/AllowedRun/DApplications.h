#if !defined(AFX_DAPPLICATIONS_H__C243A931_BEEB_49BF_9C8F_3DE47B50784A__INCLUDED_)
#define AFX_DAPPLICATIONS_H__C243A931_BEEB_49BF_9C8F_3DE47B50784A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DApplications.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDApplications dialog

class CDApplications : public CDialog
{
// Construction
public:
	CDApplications(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDApplications)
	enum { IDD = IDD_DIALOG_APP };
	CListCtrl	m_lsApps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDApplications)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDApplications)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnEnter();
	afx_msg long OnEdit(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnDblclkListApps(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg long OnAddnew(WPARAM wParam, LPARAM lparam);
	afx_msg long OnDelete(WPARAM wParam, LPARAM lparam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Refresh();
	HACCEL m_hAccel;
	BOOL m_bShow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAPPLICATIONS_H__C243A931_BEEB_49BF_9C8F_3DE47B50784A__INCLUDED_)
