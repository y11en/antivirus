// TabPages.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabPage1 dialog

#define MAX_TYPES 10

class CAvp32Dlg;

class CTabPage1 : public CDialog
{
// Construction
public:
	int AddDisks();

	BOOL Enable(BOOL enable);
	void SetPaths(CStringArray& sap);
	void GetPaths(CStringArray& sap);
	int InsertPathString(const char*  vn, const char*  descr, int type, BOOL check, BOOL focus);
	CTabPage1(CWnd* pParent = NULL);   // standard constructor
	~CTabPage1();   // standard destructor
	BOOL Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd = NULL );

// Dialog Data
	//{{AFX_DATA(CTabPage1)
	enum { IDD = IDD_TAB_PAGE1 };
	CListCtrl	m_lDisk;
	int		m_cFloppy;
	int		m_cLocal;
	int		m_cNetwork;
	//}}AFX_DATA

//	int		m_cLocal;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabPage1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HIMAGELIST m_ShellImageList;
	BOOL m_Disabled;
	CTabCtrl* Tab;
	CAvp32Dlg* Parent;
	CImageList ImageList;
//	int count;
	int c_max[MAX_TYPES];
	int c_on[MAX_TYPES];
	int CheckDisks(int type,int status);
	void OnCheck_( int *var, int type);

	// Generated message map functions
	//{{AFX_MSG(CTabPage1)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonBrowse();
	afx_msg void OnCheckFloppy();
	afx_msg void OnCheckLocal();
	afx_msg void OnCheckNetwork();
	afx_msg void OnItemchangedListDisk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListDisk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListDisk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListDisk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListDisk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CTabPage2 dialog

class CTabPage2 : public CDialog
{
// Construction
public:
	CTabPage2(CWnd* pParent = NULL);   // standard constructor
	BOOL Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd = NULL );

// Dialog Data
	//{{AFX_DATA(CTabPage2)
	enum { IDD = IDD_TAB_PAGE2 };
	int		m_Mask;
	BOOL	m_Sectors;
	BOOL	m_Files;
	BOOL	m_Packed;
	BOOL	m_Memory;
	BOOL	m_Archives;
	CString	m_UserMask;
	BOOL	m_MailPlain;
	BOOL	m_MailBases;
	BOOL	m_Exclude;
	CString	m_EditExclude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabPage2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTabCtrl* Tab;
	CAvp32Dlg* Parent;

	// Generated message map functions
	//{{AFX_MSG(CTabPage2)
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg void OnChanged();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExclude();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CTabPage3 dialog

class CTabPage3 : public CDialog
{
// Construction
public:
	int m_Priority;
	CTabPage3(CWnd* pParent = NULL);   // standard constructor
	BOOL Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd = NULL );

// Dialog Data
	//{{AFX_DATA(CTabPage3)
	enum { IDD = IDD_TAB_PAGE3 };
	BOOL	m_Warnings;
	BOOL	m_ShowOK;
	BOOL	m_CA;
	BOOL	m_Limit;
	BOOL	m_Append;
	BOOL	m_Redundant;
	BOOL	m_Report;
	CString	m_Reportfile;
	UINT	m_Size;
	BOOL	m_ShowPack;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabPage3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTabCtrl* Tab;
	CAvp32Dlg* Parent;

	// Generated message map functions
	//{{AFX_MSG(CTabPage3)
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg void OnChanged();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonCust();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTabPage5 dialog

class CTabPage5 : public CDialog
{
// Construction
public:
	DWORDLONG m_CountSize;
	DWORDLONG m_RealCountSize;
	CTabPage5(CWnd* pParent = NULL);   // standard constructor
	BOOL Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd = NULL );
	void Flush();

// Dialog Data
	//{{AFX_DATA(CTabPage5)
	enum { IDD = IDD_TAB_PAGE5 };
	int	m_Archives;
	int	m_Bodies;
	int	m_Directories;
	int	m_Disinfected;
	int	m_Deleted;
	int	m_Files;
	int	m_Packed;
	int	m_Sectors;
	int	m_Suspicious;
	int	m_Virus;
	int	m_Warnings;
	int	m_Corrupted;
	int	m_Errors;
	CString	m_Time;
	int	m_Speed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabPage5)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTabCtrl* Tab;
	CAvp32Dlg* Parent;

	// Generated message map functions
	//{{AFX_MSG(CTabPage5)
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CTabPage6 dialog

class CTabPage6 : public CDialog
{
// Construction
public:
	CTabPage6(CWnd* pParent = NULL);   // standard constructor
	BOOL Create( UINT nIDTemplate, CTabCtrl* tab, CAvp32Dlg* pParentWnd = NULL );

// Dialog Data
	//{{AFX_DATA(CTabPage6)
	enum { IDD = IDD_TAB_PAGE6 };
	BOOL	m_CopyInfected;
	BOOL	m_CopySuspicion;
	CString	m_sInfectFolder;
	CString	m_sSuspFolder;
	int		m_Action;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabPage6)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTabCtrl* Tab;
	CAvp32Dlg* Parent;

	// Generated message map functions
	//{{AFX_MSG(CTabPage6)
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg void OnChanged();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
