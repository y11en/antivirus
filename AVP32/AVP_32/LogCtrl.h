// LogCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogCtrl window

class CLogCtrl : public CListCtrl
{
// Construction
public:
	CLogCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLogCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLayout();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
