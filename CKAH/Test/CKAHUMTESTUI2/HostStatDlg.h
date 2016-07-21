#if !defined(AFX_HOSTSTATDLG_H__F73DD5E9_07B4_4BDC_B98E_E2A97C6E7938__INCLUDED_)
#define AFX_HOSTSTATDLG_H__F73DD5E9_07B4_4BDC_B98E_E2A97C6E7938__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HostStatDlg.h : header file
//

#include <CKAH/ckahum.h>
#include "winsock.h"

/////////////////////////////////////////////////////////////////////////////
// CHostStatDlg dialog

class CHostStatDlg : public CDialog
{
// Construction
public:
	void FillDlg();
	CHostStatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHostStatDlg)
	enum { IDD = IDD_HostStat };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHostStatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHostStatDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOSTSTATDLG_H__F73DD5E9_07B4_4BDC_B98E_E2A97C6E7938__INCLUDED_)
