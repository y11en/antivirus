#if !defined(AFX_CODEGENWIZARD_H__0E24BC44_DAE9_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_CODEGENWIZARD_H__0E24BC44_DAE9_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <property/property.h>
#include "Resource.h"
#include "../PCGInterface.h"
#include <Stuff\FAutomat.h>

// CodeGenWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCodeGenWizard dialog

class CCodeGenWizard : public CDialog, public CFAutomaton	{
// Construction
public:
	CCodeGenWizard(HDATA hImpData, AVP_dword idSelectedIFace, BOOL i_bImplementation, CWnd* pParent = NULL);   // standard constructor
	~CCodeGenWizard();   // standard destructor

// Dialog Data
	//{{AFX_DATA(CCodeGenWizard)
	enum { IDD = IDD_CODEGENERATIONWIZARD_DLG };
	CButton	        m_FinishBtn;
	CButton					m_NextBtn;
	CButton					m_BackBtn;
	CStatic					m_ChildFrame;
	HDATA           m_hImpData;
	HDATA           m_hIntOptionsPattern;
	HDATA           m_hWorkOptTree;
	HDATA           m_hWorkIntRoot;
	int							m_nCurrOptIndex;
	CDialog				 *m_pPage;
	IPragueCodeGen *m_pInterface;
	BOOL            m_bInSingleFile;
	BOOL            m_bOldInSingleFile;
	CLSID           m_rCLSID;
	CLSID           m_rOldCLSID;
	int             m_nDirection;
	HDATA  				  m_hOptFileData;
	CString				  m_rcOptFileName;
	CString				  m_rcPublicDirName;
	CString				  m_rcPrivateDirName;
	//}}AFX_DATA


	BOOL m_bImplementation;
  static  PCFAState   *GetStatesTable();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodeGenWizard)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCodeGenWizard)
	virtual BOOL OnInitDialog();
	afx_msg void OnCgwNext();
	afx_msg void OnCgwBack();
	afx_msg void OnCgwFinish();
	afx_msg void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void GenerateWorkOptTree();
	void GetInterfaceOptionsPattern();
	bool PrepareAndSetGeneratorProjectInfo();
	void PrepareAndLaunchGenerator();
	HRESULT LaunchGenerator( HDATA hImpData, HDATA hOptData );
	void ShowGeneratorError( HRESULT hResult, CString &rError );
	HRESULT LoadTypeTable();
	void PrepareCodeGenOptions();
	void	LoadCodeGenOptionsPattern();
	HDATA GetOptFileOptions();

  virtual	bool  Reduce();
	virtual void  Stop();
	bool Enter0( int &rcNewState );
	bool Enter1( int &rcNewState );

	bool Leave0( int &rcNewState );
	bool Leave1( int &rcNewState );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEGENWIZARD_H__0E24BC44_DAE9_11D3_96B1_00D0B71DDDF6__INCLUDED_)
