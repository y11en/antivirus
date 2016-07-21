// ScanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog
#include "FindDlg.h"

class CScanDialog : public CDialog
{
// Construction
public:
	void PutLogString( const char* objname, const char* message);
	void ObjectMessage(void* ScanObject,const char* s);
	CScanDialog(CWnd* pParent = NULL);   // standard constructor
	UINT ScanPath();
	UINT CheckMBRS(DWORD MFlags);
	UINT CheckBoot(CString& path, DWORD MFlags);
	
// Dialog Data
	//{{AFX_DATA(CScanDialog)
	enum { IDD = IDD_SCANDIALOG };
	CListCtrl	m_List;
	CString	m_Path;
	BOOL	m_CA;
	BOOL	m_AllWarnings;
	BOOL	m_AllEntry;
	BOOL	m_SavePages;
	BOOL	m_Redundant;
	BOOL	m_SavePacked;
	BOOL	m_SaveArchived;
	BOOL	m_Archived;
	BOOL	m_Mail;
	BOOL	m_Packed;
	int		m_Action;
	BOOL	m_Recurse;
	CString	m_PathSave;
	//}}AFX_DATA

	CWinThread* ScanThreadPtr;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFindDlg FindDlg;
	int KillScan;

	BOOL CanCancel;
	// Generated message map functions
	//{{AFX_MSG(CScanDialog)
	afx_msg void OnScan();
	afx_msg LONG OnAVPMessage( UINT uParam, LONG lParam);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindagain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
