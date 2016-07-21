#if !defined(AFX_DINFOFORUSER_H__4DB90808_4A1B_4D35_B84D_B3CD0F25E118__INCLUDED_)
#define AFX_DINFOFORUSER_H__4DB90808_4A1B_4D35_B84D_B3CD0F25E118__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DInfoForUser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDInfoForUser dialog

class CDInfoForUser : public CDialog
{
// Construction
public:
	CDInfoForUser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDInfoForUser)
	enum { IDD = IDD_DIALOG_INFOFORUSER };
	CEdit	m_eAdditionalInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDInfoForUser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDInfoForUser)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DINFOFORUSER_H__4DB90808_4A1B_4D35_B84D_B3CD0F25E118__INCLUDED_)
