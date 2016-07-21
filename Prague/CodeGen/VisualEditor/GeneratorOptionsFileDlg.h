#if !defined(AFX_GENERATOROPTIONSFILEDLG_H__D1B3DE43_E393_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_GENERATOROPTIONSFILEDLG_H__D1B3DE43_E393_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include <property/property.h>
#include <Stuff\FAutomat.h>
#include <Stuff\RegStorage.h>

// GeneratorOptionsFile.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CGeneratorOptionsFileDlg dialog

class CGeneratorOptionsFileDlg : public CDialog, public CFAutoParam {
// Construction
public:
	CGeneratorOptionsFileDlg( HDATA &hRestoredData, 
		                        CString &rcFileName, 
														CString &rcPublicDirName,
														CString &rcPrivateDir,
														CWnd &rcFrame, 
														int nRegime, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneratorOptionsFileDlg)
	enum { IDD = IDD_OPTIONSFILE_DLG };
	CButton				m_BrowseBtn;
	CEdit					m_FileNameEdit;
	CString				m_FileName;
	BOOL					m_bNewSession;
	CRegStorage   m_RegStorage;
	CString				m_LoadStatus;
	int						m_nRegime;
	CString				m_PublicDir;
	CString				m_PrivateDir;
	//}}AFX_DATA


	CWnd		&m_rcFrame;
	CString	&m_rcFileName;
	CString &m_rcPublicDirName;
	CString &m_rcPrivateDir;
	HDATA		&m_hRestoredData;

	virtual void   SetState( int s );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneratorOptionsFileDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneratorOptionsFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNewSession();
	afx_msg void OnUseFile();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBrowse();
	afx_msg void OnOutputBrowse();
	afx_msg void OnIncludeBrowse();
	afx_msg void OnDestroy();
	afx_msg void OnChangeOutputDir();
	afx_msg void OnChangePrivateDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void  ReadOptionsFile( CString &rcFileName );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERATOROPTIONSFILE_H__D1B3DE43_E393_11D3_96B1_00D0B71DDDF6__INCLUDED_)
