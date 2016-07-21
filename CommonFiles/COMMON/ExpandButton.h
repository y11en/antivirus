#if !defined(AFX_EXPANDBUTTON_H__8A8CFF4D_0075_4BC8_8A4B_221AEDDC5BEB__INCLUDED_)
#define AFX_EXPANDBUTTON_H__8A8CFF4D_0075_4BC8_8A4B_221AEDDC5BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExpandButton.h : header file
//

#include "defs.h"

/////////////////////////////////////////////////////////////////////////////
// CExpandButton window

class COMMONEXPORT CExpandButton : public CButton
{
// Construction
public:
	CExpandButton();

// Attributes
public:

// Operations
public:
	void SetExpandState(bool bExpand);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpandButton)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExpandButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExpandButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void Initialize();

	bool m_bExpand;

	HICON m_hiMore, m_hiLess;
	CImageList m_ilMore, m_ilLess;
	bool m_bIsThemed;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPANDBUTTON_H__8A8CFF4D_0075_4BC8_8A4B_221AEDDC5BEB__INCLUDED_)
