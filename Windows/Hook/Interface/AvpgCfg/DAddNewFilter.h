#if !defined(AFX_DADDNEWFILTER_H__63CB9D2A_7148_41F9_959D_C66199719AB6__INCLUDED_)
#define AFX_DADDNEWFILTER_H__63CB9D2A_7148_41F9_959D_C66199719AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DAddNewFilter.h : header file
//
#include "DTreeBase.h"
/////////////////////////////////////////////////////////////////////////////
// CDAddNewFilter dialog

class CDAddNewFilter : public CDTreeBase
{
// Construction
public:
	PFILTER_TRANSMIT GetFilter();
	CDAddNewFilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDAddNewFilter)
	enum { IDD = IDD_DIALOG_ADDNEWFILTER };
	CControlTree	m_TreeCtrl;
	int		m_FilterPosition;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDAddNewFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDAddNewFilter)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
	char m_pFilterBuffer[FILTER_BUFFER_SIZE];
	PFILTER_TRANSMIT m_pFilter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DADDNEWFILTER_H__63CB9D2A_7148_41F9_959D_C66199719AB6__INCLUDED_)
