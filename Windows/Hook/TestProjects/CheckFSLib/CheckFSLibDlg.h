// CheckFSLibDlg.h : header file
//

#if !defined(AFX_CHECKFSLIBDLG_H__A7B5DCC1_56AB_4F94_A0AB_3508BA64D40B__INCLUDED_)
#define AFX_CHECKFSLIBDLG_H__A7B5DCC1_56AB_4F94_A0AB_3508BA64D40B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCheckFSLibDlg dialog

class CCheckFSLibDlg : public CDialog
{
// Construction
public:
	CCheckFSLibDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckFSLibDlg)
	enum { IDD = IDD_CHECKFSLIB_DIALOG };
	CButton	m_bPopup;
	CButton	m_bAddFilter;
	CButton	m_bInfo;
	BOOL	m_chStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckFSLibDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCheckFSLibDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnCheckStatus();
	afx_msg void OnButtonAddfilter();
	afx_msg void OnButtonAddinfofilter();
	afx_msg void OnButtonAddpopupfilter();
	afx_msg void OnButtonR3();
	afx_msg void OnButtonSetwritedetectfilter();
	afx_msg void OnButtonDelTheSame();
	afx_msg void OnButtonMaskList();
	afx_msg void OnButtonChangeParam();
	afx_msg void OnButtonResetTrace();
	afx_msg void OnButtonGetfilestat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	ULONG m_PopupFilterId;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKFSLIBDLG_H__A7B5DCC1_56AB_4F94_A0AB_3508BA64D40B__INCLUDED_)
