#if !defined(AFX_BANOMETDLG_H__65ABC0C7_69EE_41BD_AA93_1FB82D8CD3EB__INCLUDED_)
#define AFX_BANOMETDLG_H__65ABC0C7_69EE_41BD_AA93_1FB82D8CD3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BanometDlg.h : header file
//

#include <CKAH/ckahum.h>

//#include "../../../../../windows/Hook/Plugins/Include/banomet_api.h"
#include "winsock.h"
//#include "../../ckahum/PluginUtils.h"

/////////////////////////////////////////////////////////////////////////////
// CBanometDlg dialog

class CBanometDlg : public CDialog
{
// Construction
public:
	void FillDlg(CKAHIDS::HBANNEDLIST  hList);
	CBanometDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBanometDlg)
	enum { IDD = IDD_BANOMET };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBanometDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBanometDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BANOMETDLG_H__65ABC0C7_69EE_41BD_AA93_1FB82D8CD3EB__INCLUDED_)
