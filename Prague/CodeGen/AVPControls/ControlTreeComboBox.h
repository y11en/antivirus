#if !defined(AFX_CONTROLLISTCOMBOBOX_H__3B155164_9BE3_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_CONTROLLISTCOMBOBOX_H__3B155164_9BE3_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <StuffMFC\ACComboBox.h>

// ControlListComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlTreeComboBox window

class CControlTreeComboBoxEdit;

class CControlTreeComboBox : public CACComboBox {
	CControlTreeComboBoxEdit *editCtrl;
	HTREEITEM 								m_hItem;	
	CStringList								m_lstItems;	
	int 											m_nSel;
	int                       m_additionXSpace;
	int												m_startupXSpace;
	bool											m_bESC;				// To indicate whether ESC key was pressed
// Construction
public:
	CControlTreeComboBox( HTREEITEM hItem, CStringList *plstItems, CString &text );

// Attributes
public:

// Operations
public:

	 void GetEncloseRect( CRect &encloseRect );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreeComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreeComboBox();

  friend class CControlTreeComboBoxEdit; 

private:
	void EndWorkingProcessing();
// Generated message map functions
protected:
	//{{AFX_MSG(CControlTreeComboBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditUpdate();
	afx_msg void OnNcDestroy();
	afx_msg void OnSelChange();
	afx_msg LRESULT OnGetEncloseRect( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CControlTreeComboBoxEdit window

class CControlTreeComboBoxEdit : public CEdit {
// Construction
public:
	CControlTreeComboBoxEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreeComboBoxEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreeComboBoxEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CControlTreeComboBoxEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnEmGetRect( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnPaste( WPARAM wParam, LPARAM lParam );
		//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLISTCOMBOBOX_H__3B155164_9BE3_11D2_96B0_00104B5B66AA__INCLUDED_)
