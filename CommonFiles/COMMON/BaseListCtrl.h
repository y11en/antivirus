#if !defined(AFX_BASELISTCTRL_H__9206AE1E_8145_4B2A_AAA8_E9C4E6FD7261__INCLUDED_)
#define AFX_BASELISTCTRL_H__9206AE1E_8145_4B2A_AAA8_E9C4E6FD7261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCBaseListCtrl window

class CCBaseListCtrl : public CListCtrl
{
// Construction
public:
	CCBaseListCtrl();

// Attributes
public:

	BOOL GetTextIconRect( int iRow, int iCol, CRect &rectText, CRect &rectIcon, CRect &rectItem );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCBaseListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCBaseListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCBaseListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASELISTCTRL_H__9206AE1E_8145_4B2A_AAA8_E9C4E6FD7261__INCLUDED_)
