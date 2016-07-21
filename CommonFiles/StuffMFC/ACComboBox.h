////////////////////////////////////////////////////////////////////
//
//  ACComboBox.h
//  The CComboBox derived class for automatically completion of text entered by user
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __ACCOMBOBOX_H__
#define __ACCOMBOBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ACComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CACComboBox window
class CACComboBox : public CComboBox {
// Construction
public:
	CACComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CACComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CACComboBox();

private :
	static BOOLEAN				m_bDisableAutoComplete;
	static WNDPROC				m_pOriginalEditProc;
	static LRESULT APIENTRY		AutoCompleteProc(HWND hWndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CACComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOMBOBOX_H__CEAD8763_E90A_11D3_96B1_00D0B71DDDF6__INCLUDED_)
