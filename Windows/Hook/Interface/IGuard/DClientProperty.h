#if !defined(AFX_DCLIENTPROPERTY_H__3E53A06D_4E05_47A6_A04B_CD6CA28916EA__INCLUDED_)
#define AFX_DCLIENTPROPERTY_H__3E53A06D_4E05_47A6_A04B_CD6CA28916EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DClientProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDClientProperty dialog

class CDClientProperty : public CDialog
{
// Construction
public:
	CDClientProperty(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDClientProperty)
	enum { IDD = IDD_DIALOG_CLIENT_PROPERTY };
	CString	m_eLogPlace;
	BOOL	m_chAllowCacheDeadlock;
	BOOL	m_chClientType;
	BOOL	m_chSaveFilters;
	BOOL	m_chUseBlueScreen;
	BOOL	m_chUseDriverCache;
	BOOL	m_chWithoutCascade;
	BOOL	m_chWithoutWatchDog;
	int		m_eCacheSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDClientProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDClientProperty)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonApply();
	afx_msg void OnChangeEditLogPlace();
	afx_msg void OnCheckAllowCacheDeadlock();
	afx_msg void OnCheckClientPopup();
	afx_msg void OnCheckSaveFilters();
	afx_msg void OnCheckUseBlueScreenReq();
	afx_msg void OnCheckUsingDriverCache();
	afx_msg void OnCheckWithoutCascadeProcessing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void EnableApply(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCLIENTPROPERTY_H__3E53A06D_4E05_47A6_A04B_CD6CA28916EA__INCLUDED_)
