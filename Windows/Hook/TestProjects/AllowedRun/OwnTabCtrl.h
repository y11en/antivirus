#if !defined(AFX_OWNTABCTRL_H__797FA8C3_77E1_11D3_9C89_0000E8905EBC__INCLUDED_)
#define AFX_OWNTABCTRL_H__797FA8C3_77E1_11D3_9C89_0000E8905EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OwnTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COwnTabCtrl window

class COwnTabCtrl : public CTabCtrl
{
// Construction
public:
	COwnTabCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COwnTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	CRect GetPageRect();
	bool m_bCanErase;
	virtual ~COwnTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(COwnTabCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OWNTABCTRL_H__797FA8C3_77E1_11D3_9C89_0000E8905EBC__INCLUDED_)
