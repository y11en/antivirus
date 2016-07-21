#if !defined(AFX_HEADERGENWIZARD_H__518D8A24_EF57_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_HEADERGENWIZARD_H__518D8A24_EF57_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <property/property.h>
#include "Resource.h"
#include "../PCGInterface.h"
#include <Stuff\FAutomat.h>

// HeaderGenWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeaderGenWizard dialog
// ---
class CHeaderGenWizard : public CDialog, public CFAutomaton {
// Construction
public:
	CHeaderGenWizard(HDATA hImpData, AVP_dword idSelectedIFace, CWnd* pParent = NULL);   // standard constructor
	~CHeaderGenWizard();

// Dialog Data
	//{{AFX_DATA(CHeaderGenWizard)
	enum { IDD = IDD_HEADERGENERATIONWIZARD_DLG };
	CButton					m_NextBtn;
	CButton					m_BackBtn;
	CButton					m_FinishBtn;
	CStatic					m_ChildFrame;
	HDATA           m_hImpData;
	HDATA           m_hIntOptionsPattern;
	HDATA           m_hWorkOptTree;
	HDATA           m_hWorkIntRoot;
	int							m_nCurrOptIndex;
	CDialog				 *m_pPage;
	IPragueCodeGen *m_pInterface;
	DWORD           m_dwInterfaceCount;
	DWORD           m_dwPagesCount;
	CLSID           m_rCLSID;
	CLSID           m_rOldCLSID;
	int             m_nDirection;
	HDATA  				  m_hOptFileData;
	BOOL            m_bFinishEnable;
	CString				  m_rcOptFileName;
	CString				  m_rcPublicDirName;
	CString				  m_rcPrivateDirName;
	//}}AFX_DATA

  static  PCFAState   *GetStatesTable();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeaderGenWizard)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHeaderGenWizard)
	virtual BOOL OnInitDialog();
	afx_msg void OnHgwNext();
	afx_msg void OnHgwBack();
	afx_msg void OnHgwFinish();
	afx_msg void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void GenerateAndMergeWorkOptTree();
	void GenerateWorkOptTree();
	void GetInterfaceOptionsPattern();
	void FindCurrInterfaceData( int nIndex );
	bool PrepareAndSetGeneratorProjectInfo();
	void PrepareAndLaunchGenerator();
	void PrepareImpTree( HDATA hImpData, HDATA &hIntData, HDATA hWorkData, CString &rIntName );
	bool LaunchGenerator( HDATA hImpData, HDATA hOptData, CString &rIntName );
	void CopyCurrInterfaceData();
	void ShowGeneratorError( HRESULT hResult, CString &rError );
	HRESULT LoadTypeTable();

  virtual	bool  Reduce();
	bool Enter0( int &rcNewState );
	bool Enter1( int &rcNewState );

	bool Leave0( int &rcNewState );
	bool Leave1( int &rcNewState );
	//bool Leave2( int &rcNewState );
	//bool Leave4( int &rcNewState );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERGENWIZARD_H__518D8A24_EF57_11D3_96B1_00D0B71DDDF6__INCLUDED_)
