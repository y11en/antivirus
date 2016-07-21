#if !defined(AFX_DFILTER_H__B33B8C81_5CC9_4D31_B53A_ED9C8F3CF2DC__INCLUDED_)
#define AFX_DFILTER_H__B33B8C81_5CC9_4D31_B53A_ED9C8F3CF2DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDFilter dialog

class CDFilter : public CDialog
{
// Construction
public:
	void SetAddPos(ULONG HookID, ULONG FuncMj, ULONG FuncMi);
	PFILTER_TRANSMIT m_FilterWork;
	CDFilter(PFILTER_TRANSMIT pFilter, CString* pCaption, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDFilter)
	enum { IDD = IDD_DIALOG_FILTER };
	CComboBox	m_cbFuncMi;
	CComboBox	m_cbFuncMj;
	CComboBox	m_cbHook;
	CComboBox	m_cbProcess;
	CListCtrl	m_lParameters;
	BOOL	m_chDontPass;
	BOOL	m_chDropCache;
	BOOL	m_chLogThis;
	BOOL	m_chProcessingType;
	BOOL	m_chResetCache;
	BOOL	m_chUseCache;
	BOOL	m_chSave2Cache;
	BOOL	m_chUserLog;
	int		m_Timeout;
	int		m_rFilterType;
	int		m_rVerdict;
	int		m_rPos;
	BOOL	m_chMiAll;
	BOOL	m_chMjAll;
	BOOL	m_chTouchFilter;
	int		m_eExpTime;
	BOOL	m_chProcessnameCacheable;
	BOOL	m_chTryNextFilter;
	CString	m_eFilterName;
	BOOL	m_chDbgBreak;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFilter)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboHookid();
	afx_msg void OnSelchangeComboFuncMj();
	afx_msg void OnSelchangeComboFuncMi();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCheckProcessingType();
	afx_msg void OnCheckAllMj();
	afx_msg void OnCheckAllMi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	PFILTER_TRANSMIT m_pFilter;
	BYTE m_pFilterBuffer[FILTER_BUFFER_SIZE];

	CString* m_pCaption;

	void FillMjList();
	void FillMiList();

	void RefreshParameters();

	ULONG m_AddPos_HookID;
	ULONG m_AddPos_FuncMj;
	ULONG m_AddPos_FuncMi;

	void TogglePos(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFILTER_H__B33B8C81_5CC9_4D31_B53A_ED9C8F3CF2DC__INCLUDED_)
