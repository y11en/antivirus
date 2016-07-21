#if !defined(AFX_DCHANGEAPPID_H__9DCA41FC_55F0_4DD2_B1D1_52B2269ABA2B__INCLUDED_)
#define AFX_DCHANGEAPPID_H__9DCA41FC_55F0_4DD2_B1D1_52B2269ABA2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DChangeAppID.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDChangeAppID dialog

class CDChangeAppID : public CDialog
{
// Construction
public:
	CDChangeAppID(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDChangeAppID)
	enum { IDD = IDD_DIALOG_APPID };
	UINT	m_eAppID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDChangeAppID)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDChangeAppID)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCHANGEAPPID_H__9DCA41FC_55F0_4DD2_B1D1_52B2269ABA2B__INCLUDED_)
