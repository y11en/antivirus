#if !defined(AFX_GENERALOPTIONSDLG_H__47821761_DAFF_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_GENERALOPTIONSDLG_H__47821761_DAFF_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <Stuff\FAutomat.h>

// GeneralOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneralOptionsDlg dialog

class CGeneralOptionsDlg : public CDialog, public CFAutoParam {
// Construction
public:
	CGeneralOptionsDlg(BOOL &rbInSingleFile, CWnd &rcFrame, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneralOptionsDlg)
	enum { IDD = IDD_CODEGENGENERAL_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL	&m_bInSingleFile;
	CWnd  &m_rcFrame;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void   SetState( int s );
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralOptionsDlg)
	afx_msg void OnSingleFile();
	afx_msg void OnDifferentFile();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALOPTIONSDLG_H__47821761_DAFF_11D3_96B1_00D0B71DDDF6__INCLUDED_)
