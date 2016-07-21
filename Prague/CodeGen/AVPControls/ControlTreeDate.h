#if !defined(AFX_CONTROLTREEDATE_H__42720224_D309_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_CONTROLTREEDATE_H__42720224_D309_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlTreeDate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlTreeDate window

class CControlTreeDate : public CDateTimeCtrl {
	HTREEITEM m_hItem;
	bool			m_bESC;				// To indicate whether ESC key was pressed
// Construction
public:
	CControlTreeDate( HTREEITEM hItem );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreeDate)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreeDate();

	// Generated message map functions
protected:
	//{{AFX_MSG(CControlTreeDate)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg LRESULT OnGetEncloseRect( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLTREEDATE_H__42720224_D309_11D2_96B0_00104B5B66AA__INCLUDED_)
