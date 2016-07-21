//{{AFX_INCLUDES()
#include "reportviewctrl.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLOG_H__10D49936_DA16_11D3_9DBD_00D0B71C715A__INCLUDED_)
#define AFX_DLOG_H__10D49936_DA16_11D3_9DBD_00D0B71C715A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLog dialog

class CDLog : public CDialog
{
// Construction
public:
	CDLog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLog)
	enum { IDD = IDD_DIALOG_LOG };
	CComboBox	m_cbSessions;
	CReportViewCtrl	m_axReportViewCtrl;
	BOOL	m_chAutoTracking;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRVCGetCortegeIcon(long nIconID, long FAR* phIcon);
	afx_msg void OnRVCGetLocalisedString(long nLocaleID, BSTR FAR* pString);
	afx_msg void OnRVCGetFindContext(BSTR FAR* pContext, long FAR* pdwFlags, BOOL FAR* pbDone);
	afx_msg long OnStartInit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelchangeComboSessions();
	afx_msg void OnCheckAutotracking();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SaveColumnWidth(void);
	void RestoreColumnWidth(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLOG_H__10D49936_DA16_11D3_9DBD_00D0B71C715A__INCLUDED_)
