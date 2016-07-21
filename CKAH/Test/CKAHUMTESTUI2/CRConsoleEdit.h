#if !defined(AFX_CRCONSOLEEDIT_H__CEC08E25_75B6_42A5_9A5F_C81F1757C37E__INCLUDED_)
#define AFX_CRCONSOLEEDIT_H__CEC08E25_75B6_42A5_9A5F_C81F1757C37E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CRConsoleEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCRConsoleEdit window

class CCRConsoleEdit : public CEdit
{
// Construction
public:
	CCRConsoleEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCRConsoleEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCRConsoleEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCRConsoleEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRCONSOLEEDIT_H__CEC08E25_75B6_42A5_9A5F_C81F1757C37E__INCLUDED_)
