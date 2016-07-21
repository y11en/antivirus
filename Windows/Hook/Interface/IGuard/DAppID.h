#if !defined(AFX_DAPPID_H__E9CA211C_705B_43D7_9A06_D0F151B5CBBF__INCLUDED_)
#define AFX_DAPPID_H__E9CA211C_705B_43D7_9A06_D0F151B5CBBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DAppID.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDAppID dialog

class CDAppID : public CDialog
{
// Construction
public:
	CDAppID(ULONG* pAppID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDAppID)
	enum { IDD = IDD_DIALOG_APPID };
	DWORD	m_eAppID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDAppID)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDAppID)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	ULONG* m_pAppID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAPPID_H__E9CA211C_705B_43D7_9A06_D0F151B5CBBF__INCLUDED_)
