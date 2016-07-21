// cust.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAvpCustomize dialog

class CAvpCustomize : public CDialog
{
// Construction
public:
	CAvpCustomize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAvpCustomize)
	enum { IDD = IDD_CUSTOMIZE };
	BOOL	m_CustDeleteAllMessage;
	BOOL	m_CustNextDiskPrompt;
	BOOL	m_CustPopupAfterFinish;
	BOOL	m_CustRedundantMessage;
	BOOL	m_CustSingleClick;
	BOOL	m_CustStatAfterBegin;
	BOOL	m_CustStatAfterFinish;
	BOOL	m_CustOtherMessages;
	BOOL	m_Sound;
	UINT	m_UpdateInterval;
	BOOL	m_UpdateCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvpCustomize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAvpCustomize)
	virtual BOOL OnInitDialog();
	afx_msg void OnCustUpdateCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
