#if !defined(AFX_DSETTINGS_H__4361D0B2_9740_11D3_9CF4_0000E8905EBC__INCLUDED_)
#define AFX_DSETTINGS_H__4361D0B2_9740_11D3_9CF4_0000E8905EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSettings.h : header file
//
#include "DTreeBase.h"

/////////////////////////////////////////////////////////////////////////////
// CDTreeMode dialog

class CDTreeMode : public CDTreeBase
{
// Construction
public:
	CDTreeMode(CFiltersStorage* pFiltersStorage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDTreeMode)
	enum { IDD = IDD_DIALOG_TREEMODE };
	CControlTree	m_TreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDTreeMode)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDTreeMode)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg long OnRefreshTree(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTPreProcessContextMenu(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTDoContextMenuCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTItemSelChanging(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTRemoveItem(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	HWND m_hWndApply;
	HWND m_hWndCancel;
	void UpdateFilter(int TypeUpdate);
	void StateChanged(bool bReloadStorage = true);
private:
	bool m_bSettingChanged;

	void RefreshTree(void);

	CImageList m_ImageList;

	CFiltersStorage* m_pFiltersStorage;
protected:
	virtual void ItemChanged();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSETTINGS_H__4361D0B2_9740_11D3_9CF4_0000E8905EBC__INCLUDED_)
