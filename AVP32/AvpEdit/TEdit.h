// TEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateEdit window

class CTemplateEdit : public CEdit
{
// Construction
public:
	void Check();
	CTemplateEdit();

// subclassed construction
	BOOL SubclassEdit(UINT nID, CWnd* pParent, char* formatString=NULL);

// Overridables

// Attributes
protected:
	BOOL CheckString(CString& s);
	CString Text;
	DWORD	Sel;
	CString	FormatString;


// Operations
protected:
	virtual void OnBadInput();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTemplateEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTemplateEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
