#if !defined(AFX_DCONFIG_H__3847092D_E984_4424_9C05_A992AB6CA548__INCLUDED_)
#define AFX_DCONFIG_H__3847092D_E984_4424_9C05_A992AB6CA548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DConfig.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDConfig dialog

class CDConfig : public CDialog
{
// Construction
public:
	CDConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDConfig)
	enum { IDD = IDD_DIALOG_CONFIG };
	COwnTabCtrl	m_tabMain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDConfig)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDConfig)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInsert();
	afx_msg void OnInsertCtrl();
	afx_msg void OnChangeAppid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRect m_TabRect;
	void TabResize();
	void SelectTab(int nNewTab);

	PHashTree m_PHash_TabWindow;

	void StoreWindowPosition();
	void RestoreWindowPosition();

	HACCEL m_hAccel;

	CDFilters* m_pDFilters;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCONFIG_H__3847092D_E984_4424_9C05_A992AB6CA548__INCLUDED_)
