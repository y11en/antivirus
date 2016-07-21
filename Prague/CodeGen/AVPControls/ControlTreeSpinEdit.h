#if !defined(AFX_CONTROLLISTSPINEDIT_H__DF056E06_9A4D_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_CONTROLLISTSPINEDIT_H__DF056E06_9A4D_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ControlListSpinEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlTreeSpinButton window

class CControlTreeSpinButton : public CSpinButtonCtrl {
	int m_nStepValue;
public :
	CControlTreeSpinButton( int nStepValue );

protected :
	//{{AFX_MSG(CControlTreeSpinButton)
	afx_msg void OnDeltaPos(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CControlTreeSpinEdit window

class CControlTreeSpinEdit : public CEdit {
	CControlTreeSpinButton *m_pSpinForEdit;
	bool										m_bOnSizeProcessing;
	int											m_nMinValue;
	int											m_nMaxValue;
	int											m_nStepValue;

// Construction
public:
	CControlTreeSpinEdit( int nMinValue, int nMsxValue, int nStepValue );

// Attributes
public:

// Operations
public:
	 void GetEncloseRect( CRect &encloseRect );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreeSpinEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreeSpinEdit();

	// Generated message map functions
protected:
	void ScrollParent();
	
	//{{AFX_MSG(CControlTreeSpinEdit)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg LRESULT OnGetEncloseRect( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnEmGetRect( WPARAM wParam, LPARAM lParam );
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLISTSPINEDIT_H__DF056E06_9A4D_11D2_96B0_00104B5B66AA__INCLUDED_)
