//{{AFX_INCLUDES()
#include "controltreectrl.h"
//}}AFX_INCLUDES
#if !defined(AFX_SELECTINTERFACESDLG_H__518D8A25_EF57_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_SELECTINTERFACESDLG_H__518D8A25_EF57_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stuff/fautomat.h>
#include <stuff/regstorage.h>
#include <property/property.h>
#include <avpcontrols/controltree.h>
#include <avpcontrols/checklist.h>
#include "../pcginterface.h"

// SelectInterfacesDlg.h : header file
//

extern char *g_RegValueNameTable[][5];
	
/////////////////////////////////////////////////////////////////////////////
// CSelectInterfacesDlg dialog
// ---
class CSelectInterfacesDlg : public CDialog, public CFAutoParam {
// Construction
public:
	CSelectInterfacesDlg(
		HDATA            hImpData, 
		AVP_dword        idSelectedIFace, 
		HDATA&           hRestoredData, 
		CString&         rcFileName, 
		CString&         rcPublicDirName,
		CString&         rcPrivateDir,
		int              nRegime, 
		IPragueCodeGen*& pInterface, 
 		CLSID&           rCLSID, 
		CWnd&            rcFrame, 
		CWnd*            pParent = NULL );   // standard constructor

		virtual ~CSelectInterfacesDlg();
// Dialog Data
	//{{AFX_DATA(CSelectInterfacesDlg)
	enum { IDD = IDD_SELECTINTERFACES_DLG };
	CControlTree		m_CTTControl;
	CButton					m_BrowseBtn;
	CEdit						m_FileNameEdit;
	CString					m_FileName;
	CString					m_LoadStatus;
	CString					m_PublicDir;
	CString					m_PrivateDir;
	CCheckList	  	m_Objects;
	//}}AFX_DATA

	int							m_nSelected;
	int							m_nRegime;
	BOOL						m_bNewSession;
	CRegStorage   	m_RegStorage;
	HDATA         	m_hImpData;
	AVP_dword     	m_idSelectedIFace;
	
	CWnd&           m_rcFrame;
	CString&        m_rcFileName;
	CString&        m_rcPublicDir;
	CString&        m_rcPrivateDir;
	HDATA&          m_hRestoredData;
	IPragueCodeGen*& m_pInterface;
	CLSID&          m_rCLSID;
	
	virtual void   SetState( int s );
	BOOL	IsProtoMode();

	//{{AFX_VIRTUAL(CSelectInterfacesDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CSelectInterfacesDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCTTWindowShutDown( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTCheckedChanging( WPARAM wParam, LPARAM lParam );
	afx_msg void OnNewSession();
	afx_msg void OnUseFile();
	afx_msg void OnBrowse();
	afx_msg void OnOutputBrowse();
	afx_msg void OnIncludeBrowse();
	afx_msg void OnDestroy();
	afx_msg void OnChangeOutputDir();
	afx_msg void OnChangePrivateDir();
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddGenerator();
	afx_msg void OnUnregGenerator();
	afx_msg void OnCgUpdateGeneratorList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void LoadOneInterfaceStuff( HDATA hInterfaceData, CCTItemsArray &newItems, int nFirstItemLevel );
	void LoadPluginStuff( HDATA hInterfaceData, CCTItemsArray &newItems, int nFirstItemLevel );

	void OnNewOptionsFile( LPCTSTR i_strFileName ); 
	void FillGeneratorsList();
	bool SelectGeneratorModule( CString &rcFileName );
	void UpdateGeneratorsOutputDir();
	void UpdateCurrentGeneratorOutputDir();

	void ClearGeneratorsListData();
	void ClearInterfacesListData();	
	void UpdateButtonsState();
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SELECTINTERFACESDLG_H__518D8A25_EF57_11D3_96B1_00D0B71DDDF6__INCLUDED_)
