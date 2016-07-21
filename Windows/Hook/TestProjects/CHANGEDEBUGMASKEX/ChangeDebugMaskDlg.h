// ChangeDebugMaskDlg.h : header file
//

#if !defined(AFX_CHANGEDEBUGMASKDLG_H__E74A734D_8E45_4649_92D0_79036DF7D0A7__INCLUDED_)
#define AFX_CHANGEDEBUGMASKDLG_H__E74A734D_8E45_4649_92D0_79036DF7D0A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChangeDebugMaskDlg dialog

class CChangeDebugMaskDlg : public CDialog
{
// Construction
public:
	CChangeDebugMaskDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeDebugMaskDlg)
	enum { IDD = IDD_CHANGEDEBUGMASK_DIALOG };
	CComboBox	m_cbDriver;
	CListCtrl	m_lMask;
	CComboBox	m_cbLevel;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeDebugMaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UnloadDebugPlugins();
	void LoadDebugPlugins();
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CChangeDebugMaskDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonUncheckall();
	afx_msg void OnButtonCheckall();
	afx_msg void OnSelchangeComboDriver();
	afx_msg void OnButtonApply();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ReloadSettings(DWORD dwID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEDEBUGMASKDLG_H__E74A734D_8E45_4649_92D0_79036DF7D0A7__INCLUDED_)
