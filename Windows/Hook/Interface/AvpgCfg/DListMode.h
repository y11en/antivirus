#if !defined(AFX_DLISTMODE_H__4FE18AD3_127A_40C3_AF89_7816FB470A01__INCLUDED_)
#define AFX_DLISTMODE_H__4FE18AD3_127A_40C3_AF89_7816FB470A01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DListMode.h : header file
//

#include <CELC\CELCtrl.h>
/////////////////////////////////////////////////////////////////////////////
// CDListMode dialog

class CDListMode : public CDialog
{
// Construction
public:
	CDListMode(CFiltersStorage* pFiltersStorage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDListMode)
	enum { IDD = IDD_DIALOG_LISTMODE };
	CELCtrl m_lList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDListMode)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDListMode)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg long OnRefreshList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCLoadComboContents(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCPostLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCCheckChanging(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCPreProcessContextMenu(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCDoContextMenuCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCELCSelChanging(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	HWND m_hWndApply;
	HWND m_hWndCancel;
	void UpdateFilter(int TypeUpdate);
	void StateChanged(bool bReloadStorage = true);
private:
	CFiltersStorage* m_pFiltersStorage;
	bool m_bSettingChanged;
	void UpdateList(CELCMessageInfo *info);
	CListCtrlItem* GetItemWith(int Row, int nProperty, int* pnColumnIndex);
	void FreeUserData();

	char m_pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT m_pFilterTransmit;
	PFILTER_TRANSMIT m_pFilterTransmitSave;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLISTMODE_H__4FE18AD3_127A_40C3_AF89_7816FB470A01__INCLUDED_)
