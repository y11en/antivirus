#if !defined(AFX_DFILTERS_H__58C1DA98_16B7_4E77_AD53_C6C941F9E0C5__INCLUDED_)
#define AFX_DFILTERS_H__58C1DA98_16B7_4E77_AD53_C6C941F9E0C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFilters.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDFilters dialog

class CDFilters : public CDialog
{
// Construction
public:
	CDFilters(CWnd* pParent = NULL);   // standard constructor
	void InsertNewFilter(void);

// Dialog Data
	//{{AFX_DATA(CDFilters)
	enum { IDD = IDD_DIALOG_FILTERS };
	CListCtrl	m_lFilters;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFilters)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFilters)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDel();
	afx_msg void OnDblclkListFilters(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Refresh(void);
	void Enable_DisableFilter();
	CFiltersStorage* m_pFiltersStorage;
	
	HACCEL m_hAccel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFILTERS_H__58C1DA98_16B7_4E77_AD53_C6C941F9E0C5__INCLUDED_)
