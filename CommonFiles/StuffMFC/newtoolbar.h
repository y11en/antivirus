#ifndef NEWTOOLBAR
#define NEWTOOLBAR

class CDlgToolBar : public CToolBar
{
// Construction
public:
	CDlgToolBar();
	virtual ~CDlgToolBar();

// Attributes

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgToolBar)
	//}}AFX_VIRTUAL

// Implementation

// Generated message map functions
protected:
	bool		m_init;
	CToolTipCtrl	m_TipCtrl;
	//{{AFX_MSG(CToolBarEx)
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif