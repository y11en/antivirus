#if !defined(AFX_CONTROLLISTEDIT_H__DF056E06_9A4D_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_CONTROLLISTEDIT_H__DF056E06_9A4D_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ControlListEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlTreeEdit window

class CControlTreeEdit : public CEdit {
// Construction
public:
	CControlTreeEdit();

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreeEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
protected:
	//{{AFX_MSG(CControlTreeEdit)
	afx_msg void    OnNcDestroy();
	afx_msg LRESULT OnPaste( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLISTEDIT_H__DF056E06_9A4D_11D2_96B0_00104B5B66AA__INCLUDED_)
