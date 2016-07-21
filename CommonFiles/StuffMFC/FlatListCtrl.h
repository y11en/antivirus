#if !defined(AFX_FLATLISTCTRL_H__2162BEB5_A882_11D2_B18A_B294B34D6940__INCLUDED_)
#define AFX_FLATLISTCTRL_H__2162BEB5_A882_11D2_B18A_B294B34D6940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlatListCtrl.h : header file
//

#include "FlatHeaderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CFlatListCtrl window

class CFlatListCtrl : public CListCtrl
{
// Construction
public:
	CFlatListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetSortMode(int nColumn);

	void SetSortColumn(int nColumn, BOOL bAscending);
	void ResetSortColumn();

	virtual ~CFlatListCtrl();

protected:
	CFlatHeaderCtrl m_wndFlatHeader;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlatListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATLISTCTRL_H__2162BEB5_A882_11D2_B18A_B294B34D6940__INCLUDED_)
