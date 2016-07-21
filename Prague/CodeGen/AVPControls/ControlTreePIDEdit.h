#ifndef __CONTROLTREEPIDEDIT_H__
#define	__CONTROLTREEPIDEDIT_H__

#include <Windows.h>

//#pragma title("DT PID Custom Control Definitions")

// The structure we use to pass PID around
typedef struct tagPID {
	WORD	nPIDField1;	
	WORD	nPIDField2;
	WORD	nPIDField3;
	WORD	nPIDField4;
	UINT	nInError;		// Address WORD with erroroneous/missing data (1..4)
} PID_VALUE;

// IP Address control style bits

#define	PIDS_NOVALIDATE	0x0001	// Don't validate fields as they are input

// Messages to/from the PID control
enum PIDM {
	PIDM_GETPIDVALUE = WM_USER,
	PIDM_SETPIDVALUE,
	PIDM_SETREADONLY
};

// Resource IDs for our edit controls
enum PRID {
	IDC_PIDEDIT1 = 1,
	IDC_PIDEDIT2,
	IDC_PIDEDIT3,
	IDC_PIDEDIT4
};

class CControlTreePIDCtl;


/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDSpinButton window

class CControlTreePIDSpinButton : public CSpinButtonCtrl {
	CControlTreePIDCtl* m_pParent;
public :
	CControlTreePIDSpinButton();

	void SetParent(CControlTreePIDCtl* pParent);

protected :
	//{{AFX_MSG(CControlTreeSpinButton)
	afx_msg void OnDeltaPos(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDEdit window
// ---
class CControlTreePIDEdit : public CEdit {
protected :
	int m_nPosition;
// Construction
public:
	CControlTreePIDEdit();

// Attributes
public:
	friend class CControlTreePIDCtl;

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreePIDEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreePIDEdit();

	virtual int  GetPosition();
	virtual void SetPosition( int nPos );
	virtual void StepPosition( int nStep );

	// Generated message map functions
protected:
	CControlTreePIDCtl* m_pParent;
	void SetParent(CControlTreePIDCtl* pParent);
	//{{AFX_MSG(CControlTreePIDEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LONG OnSpinStep(UINT wParam, LONG lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

// ---
class CControlTreePIDTypeEdit : public CControlTreePIDEdit {
// Construction
public:
	CControlTreePIDTypeEdit();

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreePIDEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreePIDTypeEdit();

	virtual int  GetPosition();
	virtual void SetPosition( int nPos );
	virtual void StepPosition( int nStep );

	// Generated message map functions
protected:
	//{{AFX_MSG(CControlTreePIDEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LONG OnSpinStep(UINT wParam, LONG lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDCtl window

class CControlTreePIDCtl : public CWnd {
	bool									 m_bESC;
	HTREEITEM							 m_hItem;
// Construction
public:
	static BOOL Register();

	CControlTreePIDCtl( HTREEITEM hItem, CWnd *pLocWnd );

// Attributes
public:
	friend class CControlTreePIDEdit;

	void GetEncloseRect( CRect &rcEncloseRect );
	void EndWorkingProcessing();

// Operations
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwExStyle=0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlTreePIDCtl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlTreePIDCtl();
	BOOL GetPIDValue(BOOL bPrintErrors, PID_VALUE* lpPID)
		{ return (BOOL) ::SendMessage(m_hWnd, PIDM_GETPIDVALUE, (WPARAM) bPrintErrors, (LPARAM) lpPID); };
	BOOL SetPIDValue(PID_VALUE* lpPID)
		{ return (BOOL) ::SendMessage(m_hWnd, PIDM_SETPIDVALUE, 0, (LPARAM) lpPID); };
	BOOL SetReadOnly(BOOL bReadOnly = TRUE)
		{ return (BOOL) ::SendMessage(m_hWnd, PIDM_SETREADONLY, (WPARAM) bReadOnly, 0); };
	CControlTreePIDEdit* GetEditControl(int nIndex);

	// Generated message map functions
protected:
	virtual void OnChildChar(UINT nChar, UINT nRepCnt, UINT nFlags, CControlTreePIDEdit& child);
	//{{AFX_MSG(CControlTreePIDCtl)
	afx_msg LRESULT OnEditKillFocus( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetEncloseRect( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetPID( WPARAM wParam, LPARAM lParam );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg LONG OnSetFont(UINT wParam, LONG lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LONG OnGetPIDValue(UINT wParam, LONG lParam);
	afx_msg LONG OnSetPIDValue(UINT wParam, LONG lParam);
	afx_msg LONG OnSetReadOnly(UINT wParam, LONG lParam);
	afx_msg LONG OnChildInvalid(UINT wParam, LONG lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	CControlTreePIDEdit *m_PIDEdits[4];
	CRect								 m_rcPIDRects[4];
	CRect							   m_rcDot[3];
	CWnd                *m_pLocWnd;
	CControlTreePIDSpinButton		*m_pSpinForEdit;

	BOOL ParsePIDPart(int nIndex, CControlTreePIDEdit& edit, int& n);

	BOOL m_bEnabled;
	BOOL m_bReadOnly;
	BOOL m_bNoValidate;
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __CONTROLTREEPIDEDIT_H__
