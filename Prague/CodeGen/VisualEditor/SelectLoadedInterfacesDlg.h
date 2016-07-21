#if !defined(AFX_SELECTLOADEDINTERFACESDLG_H__3169FB62_B23F_11D4_96B2_444553540000__INCLUDED_)
#define AFX_SELECTLOADEDINTERFACESDLG_H__3169FB62_B23F_11D4_96B2_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectLoadedInterfacesDlg.h : header file
//

#include <property/property.h>
#include <avpcontrols/controltree.h>

class CVisualEditorDlg;


/////////////////////////////////////////////////////////////////////////////
// CSelectLoadedInterfacesDlg dialog

class CSelectLoadedInterfacesDlg : public CDialog {
	HDATA							m_hLoadedContainer; 
	CVisualEditorDlg *m_pHost;
	BOOL              m_bFlushSelected;
// Construction
public:
	CSelectLoadedInterfacesDlg(HDATA hLoadedContainer, CVisualEditorDlg *pParent, BOOL bFlushSelected = TRUE );  

// Dialog Data
	//{{AFX_DATA(CSelectLoadedInterfacesDlg)
	enum { IDD = IDD_SELECTLOADEDINTERFACES_DLG };
	CControlTree	m_CTTControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectLoadedInterfacesDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectLoadedInterfacesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg LRESULT OnCTTWindowShutDown( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTCheckedChanging( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCTTDoExternalEditing( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void LoadInterfaceStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel );
	void LoadOneInterfaceStuff( HDATA hInterfaceData, CCTItemsArray &newItems, int nFirstItemLevel );
	void EnableDlgButtons();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTLOADEDINTERFACESDLG_H__3169FB62_B23F_11D4_96B2_444553540000__INCLUDED_)
