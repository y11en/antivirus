#if !defined(AFX_DDEFAULTFILTERS_H__6D2696BE_C902_4E29_AD4A_D9887DF56E09__INCLUDED_)
#define AFX_DDEFAULTFILTERS_H__6D2696BE_C902_4E29_AD4A_D9887DF56E09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DDefaultFilters.h : header file
//
#include <AVPCONTROLS\ControlTree.h>
#include "FiltersArray.h" 
/////////////////////////////////////////////////////////////////////////////
// CDDefaultFilters dialog

class CDDefaultFilters : public CDialog
{
// Construction
public:
	CDDefaultFilters(CFiltersStorage* pFiltersStorage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDDefaultFilters)
	enum { IDD = IDD_DIALOG_ADDDEFAULTFILTER };
	CControlTree	m_TreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDDefaultFilters)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDDefaultFilters)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg long OnCTTWindowShutdown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCTTPostLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCTTCheckChanging(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool IsFiltered(char* pchProcName, char* pchObjName, DWORD HookID, DWORD FuncMj, DWORD FuncMi, PFILTER_TRANSMIT pFilter, CFiltersArray* pFilterArray);
	void RemoveAllTreeInfoItems(void);
	void ReloadTree(void);
	void ItemChanged();

	CFiltersStorage* m_pFiltersStorage;
	bool m_bSettingChanged;
	CFiltersArray m_FiltersArray;
	CPoint m_MinSize;
	
	void UpdateFilters();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DDEFAULTFILTERS_H__6D2696BE_C902_4E29_AD4A_D9887DF56E09__INCLUDED_)
