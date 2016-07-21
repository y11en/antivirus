// guiDlg.h : header file
//

#include "..\ioctl.h"
#include "../../mklif/mklapi/mklapi.h"

#pragma once


// CguiDlg dialog
class CguiDlg : public CDialog
{
// Construction
public:
	CguiDlg(CWnd* pParent = NULL);	// standard constructor

	
// Dialog Data
	enum { IDD = IDD_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	char* pClientContext;
	ULONG ApiVersion;
	ULONG AppId;
	ULONG FltId ;
	HANDLE ThHandle;
	DWORD dwThreadID;

	HRESULT InitClientContext();

	PKLFLTDEV_RULES pdev_rules; 
	
	HRESULT
	LoadDeviceList( CListBox *cmb_box );

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboboxex1();
	afx_msg void OnBnClickedBtnAppl();
	afx_msg void OnLbnSelchangeListDevice();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedLog();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnDestroy();
};
