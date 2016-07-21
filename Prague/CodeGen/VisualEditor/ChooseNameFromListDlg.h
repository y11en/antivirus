#if !defined(AFX_CHOOSENAMEFROMLISTDLG_H__4D851BF6_8B14_4E39_A7AF_E59E2A1B107E__INCLUDED_)
#define AFX_CHOOSENAMEFROMLISTDLG_H__4D851BF6_8B14_4E39_A7AF_E59E2A1B107E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseNameFromListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseNameFromListDlg dialog

class CChooseNameFromListDlg : public CDialog
{
// Construction
public:
	CChooseNameFromListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseNameFromListDlg)
	enum { IDD = IDD_CHOOSE_NAME_FROM_LIST_DLG };
	CString	m_strNameChoosen;
	//}}AFX_DATA

	CString		m_strTitle;
	CStringList m_oNamesList;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseNameFromListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseNameFromListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSENAMEFROMLISTDLG_H__4D851BF6_8B14_4E39_A7AF_E59E2A1B107E__INCLUDED_)
