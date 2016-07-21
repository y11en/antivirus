#if !defined(AFX_GENERATORSUMMARYDLG_H__30632C61_DE1D_11D3_96B1_00D0B71DDDF6__INCLUDED_)
#define AFX_GENERATORSUMMARYDLG_H__30632C61_DE1D_11D3_96B1_00D0B71DDDF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include <property/property.h>
#include <Stuff\FAutomat.h>
#include <Stuff\RegStorage.h>

// GeneratorSummary.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneratorSummary dialog
class CGeneratorSummary : public CDialog, public CFAutoParam {
// Construction
public:
	CGeneratorSummary(HDATA hImpData, HDATA &hOptionsData, HDATA &hOptionsPattern, CString &rcOptFileName, GUID& quidClsId, CWnd &rcFrame, int nRegime, CWnd* pParent = NULL);   // standard constructor

// Dialog Data				 
	//{{AFX_DATA(CGeneratorSummary)
	enum { IDD = IDD_SUMMARYINFO_DLG };
	CEdit					m_EditView;
	CString				m_FileName;
	CRegStorage   m_RegStorage;
	int						m_nRegime;
	BOOL          m_bAutoSave;
	BOOL					m_bSaved;
	//}}AFX_DATA

	HDATA		 m_hImpData;
	GUID&		 m_quidClassId;
	HDATA		&m_hOptionsData;
	HDATA		&m_hOptionsPattern;
	CString &m_rcOptFileName;
	CWnd		&m_rcFrame;

	virtual void   SetState( int s );
	void	SetOptionsText();

	void	PerformAutoSave();
	//{{AFX_VIRTUAL(CGeneratorSummary)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CGeneratorSummary)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSaveOptions();
	afx_msg void OnAutoSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERATORSUMMARY_H__30632C61_DE1D_11D3_96B1_00D0B71DDDF6__INCLUDED_)
