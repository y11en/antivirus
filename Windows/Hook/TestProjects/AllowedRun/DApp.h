#if !defined(AFX_DAPP_H__21166420_60E8_47E6_A001_1BAD6EF59062__INCLUDED_)
#define AFX_DAPP_H__21166420_60E8_47E6_A001_1BAD6EF59062__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DApp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDApp dialog

class CDApp : public CDialog
{
// Construction
public:
	CDApp(CString* pApp, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDApp)
	enum { IDD = IDD_DIALOG_EDIT_APP };
	CComboBox	m_cbApp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDApp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDApp)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString* m_pApp;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAPP_H__21166420_60E8_47E6_A001_1BAD6EF59062__INCLUDED_)
