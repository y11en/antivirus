//{{AFX_INCLUDES()
#include "coolbarctrl.h"
//}}AFX_INCLUDES
#if !defined(AFX_DCONFIG_H__10D49935_DA16_11D3_9DBD_00D0B71C715A__INCLUDED_)
#define AFX_DCONFIG_H__10D49935_DA16_11D3_9DBD_00D0B71C715A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DConfig.h : header file
//
#include "DDefaultFilters.h"
#include "DTreeMode.h"
//#include "DListMode.h"
#include "DLog.h"
/////////////////////////////////////////////////////////////////////////////
// CDConfig dialog

class CDConfig : public CDialog
{
// Construction
public:
	CDConfig(CWnd* pParent = NULL, HANDLE hDevice = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDConfig)
	enum { IDD = IDD_DIALOG_CONFIG };
	CButton	m_bCancel;
	CButton	m_bApply;
	CCoolBarCtrl	m_axCoolBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDConfig)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDConfig)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	virtual BOOL OnInitDialog();
	afx_msg void OnApplyNow();
	afx_msg void OnExit();
	afx_msg void OnCBRGetLocalisedStringAvpcoolbar(long nStringID, BSTR FAR* pString);
	afx_msg void OnCBRFolderChangeAvpcoolbar(long nFolderIndex);
	afx_msg void OnCBRItemClickAvpcoolbar(long nFolderIndex, long nItemIndex, long nItemID);
	afx_msg void OnSimpleWizard();
	afx_msg void OnWizard();
	afx_msg void OnChangeappid();
	afx_msg void OnClose();
	afx_msg void OnClientinfo();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPoint m_MinSize;

	void SavePosition();
	void RestorePosition();
	void TabResize();

	HANDLE m_hDevice;
	CFiltersStorage* m_pFiltersStorage;
// -----------------------------------------------------------------------------------------
	CDTreeMode* m_pDTreeMode;
//	CDListMode* m_pListMode;
//	CDLog* m_pLog;
	CWnd* m_pActivePage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCONFIG_H__10D49935_DA16_11D3_9DBD_00D0B71C715A__INCLUDED_)
