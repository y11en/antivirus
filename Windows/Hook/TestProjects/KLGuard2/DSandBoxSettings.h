#if !defined(AFX_DSANDBOXSETTINGS_H__033F92FF_D986_4F00_9A4D_3E9BD68E7D67__INCLUDED_)
#define AFX_DSANDBOXSETTINGS_H__033F92FF_D986_4F00_9A4D_3E9BD68E7D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSandBoxSettings.h : header file
//
#include "AppSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CDSandBoxSettings dialog

class CDSandBoxSettings : public CDialog
{
// Construction
public:
	CDSandBoxSettings(CSandBoxSettings* pSettings, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDSandBoxSettings)
	enum { IDD = IDD_DIALOG_SANDBOX_SETTINGS };
	CButton	m_btApply;
	BOOL	m_chAllowCreateProcess;
	BOOL	m_chAllowWrite_SandBoxFolder;
	BOOL	m_chAllowWrite_TempFolder;
	BOOL	m_chAllowWrite_Registry;
	BOOL	m_chAllowWrite_SystemFolder;
	BOOL	m_chLogEvent;
	BOOL	m_chShowWarning;
	BOOL	m_chTerminateProcess;
	CString	m_eSandBoxPath;
	BOOL	m_chAllowWriteProgramFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSandBoxSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDSandBoxSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	afx_msg void OnCheckAllowWriteSystemfolder();
	afx_msg void OnCheckAllowCreateProcess();
	afx_msg void OnCheckAllowWriteSandBoxfolder();
	afx_msg void OnCheckAllowWriteTempfolder();
	afx_msg void OnCheckDenywriteRegistry();
	afx_msg void OnCheckLogevent();
	afx_msg void OnCheckShowWarning();
	afx_msg void OnCheckTerminate();
	afx_msg void OnCheckAllowwriteProgramfiles();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSandBoxSettings* m_pSettings;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSANDBOXSETTINGS_H__033F92FF_D986_4F00_9A4D_3E9BD68E7D67__INCLUDED_)
