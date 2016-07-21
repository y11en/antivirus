// OptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptDlg dialog

class COptDlg : public CDialog
{
// Construction
public:
	COptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptDlg)
	enum { IDD = IDD_OPTIONS };
	BOOL	m_LinkConfirm;
	BOOL	m_EscSaveRec;
	CString	m_SetFile;
	BOOL	m_ShowSourceFile;
	CString	m_SepLine;
	BOOL	m_SaveBackup;
	BOOL	m_SkipPag;
	CString	m_FontName;
	UINT	m_FontSize;
	BOOL	m_MakeFullName;
#if (defined (KL_MULTIPLATFORM_LINKS))
	CString	m_LocalCVSRoot;
#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
