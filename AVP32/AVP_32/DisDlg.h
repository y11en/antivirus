// DisDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisinfectDlg dialog

class CDisinfectDlg : public CDialog
{
// Construction
public:
	CDisinfectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisinfectDlg)
	enum { IDD = IDD_DISINFECT };
	CButton	m_sStop;
	BOOL	m_All;
	BOOL	m_CopyFolder;
	CString	m_sObject;
	CString	m_sVirus;
	int		m_Action;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisinfectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	  

	// Generated message map functions
	//{{AFX_MSG(CDisinfectDlg)
	afx_msg void OnStop();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
