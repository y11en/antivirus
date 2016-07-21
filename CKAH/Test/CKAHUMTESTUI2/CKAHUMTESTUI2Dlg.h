// CKAHUMTESTUI2Dlg.h : header file
//

#if !defined(AFX_CKAHUMTESTUI2DLG_H__225BB74E_9F45_43FD_A2D7_A47B1472FE93__INCLUDED_)
#define AFX_CKAHUMTESTUI2DLG_H__225BB74E_9F45_43FD_A2D7_A47B1472FE93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CKAH/ckahum.h>
#include <afxmt.h>
#include "RdrExcl.h"


/////////////////////////////////////////////////////////////////////////////
// CCKAHUMTESTUI2Dlg dialog

class CCKAHUMTESTUI2Dlg : public CDialog
{
// Construction
public:
	CCKAHUMTESTUI2Dlg(CWnd* pParent = NULL);	// standard constructor
	~CCKAHUMTESTUI2Dlg ();

	CKAHUM::OpResult UpdateNetList ();
	CKAHUM::OpResult UpdateOpenPortList ();
	CKAHUM::OpResult UpdateOpenConnectionList ();

	void AddStringToLog (LPCTSTR string);

    std::list<ExcludeItem> m_CrExludeList;

// Dialog Data
	//{{AFX_DATA(CCKAHUMTESTUI2Dlg)
	enum { IDD = IDD_CKAHUMTESTUI2_DIALOG };
	CListBox	m_TraceLog;
	CSliderCtrl	m_FirewallLevelSlider;
	CListBox	m_OpenConnectionListCtrl;
	CListBox	m_OpenPortListCtrl;
	CListBox	m_NetListCtrl;
	CIPAddressCtrl	m_UnbanMaskCtrl;
	CIPAddressCtrl	m_UnbanIPCtrl;
	CListBox	m_AttackList;
	CString	m_ReloadPath;
	UINT	m_SetParamID;
	UINT	m_SetParamNum;
	UINT	m_SetParamValue;
	UINT	m_GetParamID;
	UINT	m_GetParamNum;
	UINT	m_GetParamCountID;
	BOOL	m_Deny;
	BOOL	m_Notify;
	BOOL	m_Log;
	BOOL	m_Ban;
	UINT	m_SetIDSParamID;
	UINT	m_BanTime;
	UINT	m_nCRConsolePort;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCKAHUMTESTUI2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	void ClearResult ();
	void SetResult (CKAHUM::OpResult result);

	CKAHFW::HPACKETRULES m_hRules;
	CKAHFW::HCONNLIST m_hConnectionList;

	int m_nLastSliderPos;

	// Generated message map functions
	//{{AFX_MSG(CCKAHUMTESTUI2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnReload();
	afx_msg void OnStop();
	afx_msg void OnPause();
	afx_msg void OnResume();
	afx_msg void OnSetparam();
	afx_msg void OnGetparam();
	afx_msg void OnGetparamcount();
	afx_msg void OnClear();
	afx_msg void OnSetidsparam();
	afx_msg void OnUnban();
	afx_msg void OnStmStart();
	afx_msg void OnStmStop();
	afx_msg void OnSTMPause();
	afx_msg void OnSTMResume();
	afx_msg void OnFwStart();
	afx_msg void OnFwStop();
	afx_msg void OnFwPause();
	afx_msg void OnFwResume();
	afx_msg void OnCkahumpause();
	afx_msg void OnCkahumresume();
	afx_msg void OnUpdateNetList();
	afx_msg void OnUpdateOpenPortList();
	afx_msg void OnUpdateOpenConnectionList();
	afx_msg void OnBreakConnection();
	afx_msg void OnFwPacketrules();
	afx_msg void OnFwApprules();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCkahumenable();
	afx_msg void OnCkahumdisable();
	afx_msg LRESULT OnDelayedLog (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDelayedTrace (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowCreateRuleDlg (WPARAM wParam, LPARAM lParam);
	afx_msg void OnCrStart();
	afx_msg void OnCrStop();
	afx_msg void OnCRPause();
	afx_msg void OnCRResume();
	afx_msg void OnCRConsole();
	afx_msg void OnClear2();
	afx_msg void OnGetBannedList();
	afx_msg void OnHostStat();
	afx_msg void OnConnList();
	afx_msg void OnCRWatchdog();
	afx_msg void OnCRExcludes();
	afx_msg void OnStmChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CKAHUMTESTUI2DLG_H__225BB74E_9F45_43FD_A2D7_A47B1472FE93__INCLUDED_)
