#if !defined(AFX_PACKETRULEDLG_H__EDDC8FB7_5A91_4004_8DB4_BC7CC033955A__INCLUDED_)
#define AFX_PACKETRULEDLG_H__EDDC8FB7_5A91_4004_8DB4_BC7CC033955A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PacketRuleDlg.h : header file
//

#include <CKAH/ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CPacketRuleDlg dialog

class CPacketRuleDlg : public CDialog
{
// Construction
public:
	CPacketRuleDlg (CKAHFW::HPACKETRULE hRule = NULL, CWnd* pParent = NULL);   // standard constructor
	~CPacketRuleDlg ();

// Dialog Data
	//{{AFX_DATA(CPacketRuleDlg)
	enum { IDD = IDD_PACKETRULE };
	CListBox	m_RemoteAddressListCtrl;
	CListBox	m_LocalAddressListCtrl;
	CListBox	m_LocalPortListCtrl;
	CListBox	m_RemotePortListCtrl;
	CString	m_Name;
	BYTE	m_Proto;
	BYTE	m_ICMPCode;
	BOOL	m_Block;
	BOOL	m_Notify;
	BOOL	m_Log;
	BOOL	m_Enable;
	BYTE	m_ICMPType;
	//}}AFX_DATA

	CKAHFW::HPACKETRULE	m_hRule;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPacketRuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPacketRuleDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAddRemotePort();
	afx_msg void OnDeleteRemotePort();
	afx_msg void OnAddLocalPort();
	afx_msg void OnDeleteLocalPort();
	afx_msg void OnAddLocalAddress();
	afx_msg void OnDeleteLocalAddress();
	afx_msg void OnAddRemoteAddress();
	afx_msg void OnDeleteRemoteAddress();
	afx_msg void OnEditLocalAddress();
	afx_msg void OnEditRemoteAddress();
	afx_msg void OnEditLocalPort();
	afx_msg void OnEditRemotePort();
	afx_msg void OnTimes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKETRULEDLG_H__EDDC8FB7_5A91_4004_8DB4_BC7CC033955A__INCLUDED_)
