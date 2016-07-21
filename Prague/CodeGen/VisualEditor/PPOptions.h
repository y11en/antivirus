#if !defined(AFX_PPOPTIONS_H__0F3F7B98_29AB_41E9_AE36_B4A55B35ABFA__INCLUDED_)
#define AFX_PPOPTIONS_H__0F3F7B98_29AB_41E9_AE36_B4A55B35ABFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPPOptions dialog

class CPPOptions : public CPropertyPage
{
	DECLARE_DYNCREATE(CPPOptions)

// Construction
public:
	CPPOptions();
	~CPPOptions();

// Dialog Data
	//{{AFX_DATA(CPPOptions)
	enum { IDD = IDD_OPTIONS };
	BOOL	m_bUseRegistryNotSql;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPPOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPPOptions)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PPOPTIONS_H__0F3F7B98_29AB_41E9_AE36_B4A55B35ABFA__INCLUDED_)
