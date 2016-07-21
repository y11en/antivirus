// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__13C51ECE_AE3D_11D3_A4CD_0000E8E1E96D__INCLUDED_)
#define AFX_DLGPROXY_H__13C51ECE_AE3D_11D3_A4CD_0000E8E1E96D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAvp32Dlg;

/////////////////////////////////////////////////////////////////////////////
// CAvp32DlgAutoProxy command target

class CAvp32DlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CAvp32DlgAutoProxy)

	CAvp32DlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CAvp32Dlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvp32DlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAvp32DlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CAvp32DlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CAvp32DlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAvp32DlgAutoProxy)
	afx_msg BOOL ProcessObject(long Type, long MFlags, LPCTSTR Name, long FAR* RFlags, BSTR FAR* VirusName);
	afx_msg BOOL ReloadBase();
	afx_msg BOOL CancelProcessObject(long Code);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__13C51ECE_AE3D_11D3_A4CD_0000E8E1E96D__INCLUDED_)
