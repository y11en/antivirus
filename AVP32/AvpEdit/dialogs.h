// Dialogs.h : header file
//

#include "TEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CDInsert dialog

class CDInsert : public CDialog
{
// Construction
public:
	CDInsert(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDInsert)
	enum { IDD = IDD_INSERT };
	int		m_iType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDInsert)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
		// Generated message map functions
	//{{AFX_MSG(CDInsert)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDForm dialog

class CDForm : public CDialog
{
// Construction
public:
	DWORD DataCRC;
	BOOL m_Modified;
	CDForm(UINT idd, CWnd* pParent = NULL);   // standard constructor

	CString	m_Link16;
	CString	m_Link32;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRecordEdit* REptr;

	// Generated message map functions
	//{{AFX_MSG(CDForm)
	afx_msg void OnButtonUnlink();
	afx_msg void OnButtonRelink();
	afx_msg void OnButtonLink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDSeparator dialog

class CDSeparator : public CDForm
{
// Construction
public:
	CDSeparator(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDSeparator)
	enum { IDD = IDD_R_SEPARATOR };
	CString	m_Comment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSeparator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDSeparator)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDKernel dialog

class CDKernel : public CDForm
{
// Construction
public:
	CDKernel(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDKernel)
	enum { IDD = IDD_R_KERNEL };
	CString	m_Comment;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDKernel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDKernel)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDJump dialog

class CDJump : public CDForm
{
// Construction
public:
	CDJump(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDJump)
	enum { IDD = IDD_R_JUMP };
	CString	m_Name;
	int		m_Method;
	CString	m_Comment;
	WORD	m_2Bytes;
	WORD	m_Data1;
	WORD	m_Data2;
	WORD	m_Data3;
	DWORD	m_Sum1;
	//}}AFX_DATA

	DWORD	m_Mask1;
	DWORD	m_Mask2;
	BYTE	m_Len1;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDJump)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDJump)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTemplateEdit	p_2Bytes;
	CTemplateEdit	p_Data1;
	CTemplateEdit	p_Data2;
	CTemplateEdit	p_Data3;
	CTemplateEdit	p_Sum;

};
/////////////////////////////////////////////////////////////////////////////
// CDMemory dialog

class CDMemory : public CDForm
{
// Construction
public:
	CDMemory(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDMemory)
	enum { IDD = IDD_R_MEMORY };
	CString	m_Name;
	CString	m_Comment;
	int		m_Method;
	DWORD	m_Sum1;
	WORD	m_Offset;
	BYTE	m_Byte;
	BYTE	m_Len1;
	BYTE	m_Len2;
	WORD	m_Off2;
	WORD	m_Segment;
	//}}AFX_DATA
	BYTE	m_Replace[5];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDMemory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDMemory)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMethod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTemplateEdit	p_Offset;
	CTemplateEdit	p_Byte;
	CTemplateEdit	p_Len;
	CTemplateEdit	p_Len2;
	CTemplateEdit	p_Off2;
	CTemplateEdit	p_Replace;
	CTemplateEdit	p_Segment;
	CTemplateEdit	p_Sum;
};
/////////////////////////////////////////////////////////////////////////////
// CDSector dialog

class CDSector : public CDForm
{
// Construction
public:
	CDSector(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDSector)
	enum { IDD = IDD_R_SECTOR };
	CString	m_Name;
	CString	m_Comment;
	BOOL	m_Aboot1;
	BOOL	m_Aboot2;
	BOOL	m_FDboot1;
	BOOL	m_FDboot2;
	BOOL	m_HDboot1;
	BOOL	m_HDboot2;
	BOOL	m_MBR1;
	BOOL	m_MBR2;
	int		m_Page2;
	int		m_Page1;
	int		m_Method1;
	int		m_Method2;
	int		m_Page_C1;
	int		m_Page_C2;
	WORD	m_2Bytes;
	WORD	m_Addr11;
	WORD	m_Addr12;
	WORD	m_Addr21;
	WORD	m_Addr22;
	BYTE	m_Data1;
	BYTE	m_Data2;
	BYTE	m_Len1;
	BYTE	m_Len2;
	WORD	m_Off1;
	WORD	m_Off2;
	DWORD	m_Sum1;
	DWORD	m_Sum2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDSector)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMethod1();
	afx_msg void OnSelchangeMethod2();
	afx_msg void OnButtonSumFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTemplateEdit	p_2Bytes;
	CTemplateEdit	p_Addr11;
	CTemplateEdit	p_Addr12;
	CTemplateEdit	p_Addr21;
	CTemplateEdit	p_Addr22;
	CTemplateEdit	p_Data1;
	CTemplateEdit	p_Data2;
	CTemplateEdit	p_Len1;
	CTemplateEdit	p_Len2;
	CTemplateEdit	p_Off1;
	CTemplateEdit	p_Off2;
	CTemplateEdit	p_Sum;
	CTemplateEdit	p_Sum2;

};
/////////////////////////////////////////////////////////////////////////////
// CDFile dialog

class CDFile : public CDForm
{
// Construction
public:
	CDFile(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDFile)
	enum { IDD = IDD_R_FILE };
	CString	m_Comment;
	CString	m_Name;
	int		m_Method;
	BOOL	m_Com;
	BOOL	m_Exe;
	BOOL	m_NE;
	BOOL	m_Ole2;
	int		m_Page_C;
	int		m_Page1;
	int		m_Page2;
	BOOL	m_Sys;
	WORD	m_2Bytes;
	WORD	m_Data1;
	WORD	m_Data2;
	WORD	m_Data3;
	WORD	m_Data4;
	WORD	m_Data5;
	BYTE	m_Len1;
	BYTE	m_Len2;
	WORD	m_Off1;
	WORD	m_Off2;
	DWORD	m_Sum1;
	DWORD	m_Sum2;
	BOOL	m_Script;
	BOOL	m_Hlp;
	//}}AFX_DATA



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFile)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMethod();
	virtual void OnOK();
	afx_msg void OnButtonSumFile();
	afx_msg void OnOle2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTemplateEdit	p_2Bytes;
	CTemplateEdit	p_Data1;
	CTemplateEdit	p_Data2;
	CTemplateEdit	p_Data3;
	CTemplateEdit	p_Data4;
	CTemplateEdit	p_Data5;
	CTemplateEdit	p_Len1;
	CTemplateEdit	p_Len2;
	CTemplateEdit	p_Off1;
	CTemplateEdit	p_Off2;
	CTemplateEdit	p_Sum;
	CTemplateEdit	p_Sum2;
};
/////////////////////////////////////////////////////////////////////////////
// CDCA dialog

class CDCA : public CDForm
{
// Construction
public:
	CDCA(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDCA)
	enum { IDD = IDD_R_CA };
	CString	m_Comment;
	CString	m_Name;
	BOOL	m_Com;
	BOOL	m_Exe;
	BOOL	m_NE;
	BOOL	m_Ole2;
	int		m_Page1;
	int		m_Page2;
	BOOL	m_Sys;
	BYTE	m_Len1;
	BYTE	m_Len2;
	WORD	m_Off1;
	WORD	m_Off2;
	DWORD	m_Sum1;
	DWORD	m_Sum2;
	WORD	m_2Bytes;
	BOOL	m_FalseAlarm;
	BOOL	m_Script;
	BOOL	m_Hlp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCA)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDCA)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnFalsealarm();
	virtual void OnOK();
	afx_msg void OnButtonSumFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTemplateEdit	p_2Bytes;
	CTemplateEdit	p_Len1;
	CTemplateEdit	p_Off1;
	CTemplateEdit	p_Sum;

	CTemplateEdit	p_Len2;
	CTemplateEdit	p_Off2;
	CTemplateEdit	p_Sum2;

};
/////////////////////////////////////////////////////////////////////////////
// CDUnpack dialog

class CDUnpack : public CDForm
{
// Construction
public:
	CDUnpack(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDUnpack)
	enum { IDD = IDD_R_UNPACK };
	CString	m_Comment;
	CString	m_Name;
	BOOL	m_Com;
	BOOL	m_Exe;
	BOOL	m_NE;
	BOOL	m_Ole2;
	int		m_Page1;
	int		m_Page2;
	BOOL	m_Sys;
	WORD	m_2Bytes;
	BYTE	m_Len1;
	BYTE	m_Len2;
	WORD	m_Off1;
	WORD	m_Off2;
	DWORD	m_Sum1;
	DWORD	m_Sum2;
	BOOL	m_Script;
	BOOL	m_Hlp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDUnpack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDUnpack)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSumFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		
	CTemplateEdit	p_2Bytes;
	CTemplateEdit	p_Len1;
	CTemplateEdit	p_Len2;
	CTemplateEdit	p_Off1;
	CTemplateEdit	p_Off2;
	CTemplateEdit	p_Sum;
	CTemplateEdit	p_Sum2;

};
/////////////////////////////////////////////////////////////////////////////
// CDExtract dialog

class CDExtract : public CDForm
{
// Construction
public:
	CDExtract(CRecordEdit* reptr, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDExtract)
	enum { IDD = IDD_R_EXTRACT };
	CString	m_Comment;
	CString	m_Name;
	BOOL	m_Com;
	BOOL	m_Exe;
	BOOL	m_Ole2;
	int		m_Page1;
	int		m_Page2;
	BOOL	m_Sys;
	BOOL	m_NE;
	WORD	m_2Bytes;
	BYTE	m_Len1;
	BYTE	m_Len2;
	WORD	m_Off1;
	WORD	m_Off2;
	DWORD	m_Sum1;
	DWORD	m_Sum2;
	BOOL	m_Script;
	BOOL	m_Hlp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDExtract)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDExtract)
	afx_msg void OnButtonSum();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSumFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CTemplateEdit	p_2Bytes;
	CTemplateEdit	p_Len1;
	CTemplateEdit	p_Len2;
	CTemplateEdit	p_Off1;
	CTemplateEdit	p_Off2;
	CTemplateEdit	p_Sum;
	CTemplateEdit	p_Sum2;
};
/////////////////////////////////////////////////////////////////////////////
// CDSum dialog

class CDSum : public CDialog
{
// Construction
public:
//	long Filelen;
//	int c_Off2;
//	int c_Off1;
	CDSum(int len1, int len2, int off1, int off2, BYTE st=0 , CWnd* pParent = NULL, int autofill=1, DWORD mask1=(DWORD)-1, DWORD mask2=(DWORD)-1, DWORD disflags=0);   // standard constructor
	~CDSum();
	
	int m_Auto;
	DWORD m_Disflags;

// Dialog Data
	//{{AFX_DATA(CDSum)
	enum { IDD = IDD_SUM };
	//}}AFX_DATA
	DWORD m_Mask1;
	DWORD m_Mask2;
	int m_Off1;
	int m_Off2;
	int m_Len1;
	int m_Len2;
	BYTE* m_Buf1;
	BYTE* m_Buf2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BYTE SubType;

	// Generated message map functions
	//{{AFX_MSG(CDSum)
	afx_msg void OnAutofill();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CTemplateEdit	p_Area1;
	CTemplateEdit	p_Area2;
};
/////////////////////////////////////////////////////////////////////////////
// CDOffset dialog

class CDOffset : public CDialog
{
// Construction
public:
	CDOffset(CWnd* pParent = NULL);   // standard constructor
	int csno;
// Dialog Data
	//{{AFX_DATA(CDOffset)
	enum { IDD = IDD_OFFSET };
	DWORD	m_Offset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDOffset)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDOffset)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CTemplateEdit	p_Offset;
};
