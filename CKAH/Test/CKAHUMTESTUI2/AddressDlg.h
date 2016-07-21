#if !defined(AFX_ADDRESSDLG_H__3E702C8F_FAFC_4AF2_8CFE_D4AF54D8B1C3__INCLUDED_)
#define AFX_ADDRESSDLG_H__3E702C8F_FAFC_4AF2_8CFE_D4AF54D8B1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddressDlg.h : header file
//

#include <CKAH//ckahum.h>

/////////////////////////////////////////////////////////////////////////////
// CAddressDlg dialog

class CAddressDlg : public CDialog
{
// Construction
public:
	CAddressDlg	(CKAHFW::HRULEADDRESS hAddress, CWnd* pParent = NULL);   // standard constructor
	~CAddressDlg ();

// Dialog Data
	//{{AFX_DATA(CAddressDlg)
	enum { IDD = IDD_ADDRESS };
	CListBox	m_HostIPListCtrl;
	CIPAddressCtrl	m_SubnetMaskCtrl;
	CIPAddressCtrl	m_SubnetIPCtrl;
	CIPAddressCtrl	m_HiRangeIPCtrl;
	CIPAddressCtrl	m_LoRangeIPCtrl;
	CString	m_Name;
	//}}AFX_DATA

	CDWordArray m_HostIPs;

	CKAHFW::HRULEADDRESS m_hAddress;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void RefreshHostIPList ();

	// Generated message map functions
	//{{AFX_MSG(CAddressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHostIPListAdd();
	afx_msg void OnHostIPListClear();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDRESSDLG_H__3E702C8F_FAFC_4AF2_8CFE_D4AF54D8B1C3__INCLUDED_)
