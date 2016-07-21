//{{AFX_INCLUDES()
#include "controltreectrl.h"
//}}AFX_INCLUDES
#if !defined(AFX_GENERATOROPTIONSDLG_H__0E24BC45_DAE9_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_GENERATOROPTIONSDLG_H__0E24BC45_DAE9_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Stuff\FAutomat.h>
#include "../PCGInterface.h"

// GeneratorOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneratorOptionsDlg dialog

class CGeneratorOptionsDlg : public CDialog, public CFAutoParam {
// Construction
public:
	CGeneratorOptionsDlg(IPragueCodeGen	*&pInterface, HDATA &hIntOptionsPattern, HDATA &hIntOptionsRoot, CWnd &rcFrame, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneratorOptionsDlg)
	enum { IDD = IDD_GENERATOROPTIONS_DLG };
	CControlTreeCtrl	m_CTTControl;
	HDATA							m_hIntOptionsData;
	CString	m_Title;
	//}}AFX_DATA

	HDATA						 &m_hIntOptionsPattern;
	HDATA            &m_hIntOptionsRoot;
	CWnd						 &m_rcFrame;
	IPragueCodeGen  *&m_pInterface;

	virtual void   SetState( int s );
	void GetGeneratorUpdatedOptions();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneratorOptionsDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneratorOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCTTDoExternalEditingEx(long hData, BSTR FAR* pItemText, BOOL FAR* pbCompleted);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//	BOOL PerformFileDlg( HDATA hData, CString &rItemText );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERATOROPTIONSDLG_H__0E24BC45_DAE9_11D3_96B1_00D0B71DDDF6__INCLUDED_)
