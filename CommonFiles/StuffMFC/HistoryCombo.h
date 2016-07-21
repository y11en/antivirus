////////////////////////////////////////////////////////////////////////////
// File:	HistoryCombo.h
// Version:	1.0.0.0
// Created:	12-Apr-2001
//
// Author:	Paul S. Vickery
// E-mail:	paul@vickeryhome.freeserve.co.uk
//
// Implementation of CHistoryCombo which incorporates functionality to help
// with Loading and Saving of history in a combo box
//
// You are free to use or modify this code, with no restrictions, other than
// you continue to acknowledge me as the original author in this source code,
// or any code derived from it.
//
// If you use this code, or use it as a base for your own code, it would be 
// nice to hear from you simply so I know it's not been a waste of time!
//
// Copyright (c) 2001 Paul S. Vickery
//
// PLEASE LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORYCOMBO_H__EA3F7FA7_AA4D_11D4_A7CB_0000B48746CF__INCLUDED_)
#define AFX_HISTORYCOMBO_H__EA3F7FA7_AA4D_11D4_A7CB_0000B48746CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXADV_H__
#include "afxadv.h" // needed for CRecentFileList
#endif // ! __AFXADV_H__

/////////////////////////////////////////////////////////////////////////////
// CHistoryCombo window

class CHistoryCombo : public CComboBox
{
// Construction
public:
	CHistoryCombo();

// Attributes
public:

// Operations
public:
	int AddString(LPCTSTR lpszString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryCombo)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ClearHistory(BOOL bDeleteRegistryEntries = TRUE);
	void SetMaxHistoryItems(int nMaxItems) { m_nMaxHistoryItems = nMaxItems; }
	void SaveHistory(BOOL bAddCurrentItemtoHistory = TRUE);
	CString LoadHistory(LPCTSTR lpszSection, LPCTSTR lpszKeyPrefix, BOOL bSaveRestoreLastCurrent = TRUE, LPCTSTR lpszKeyCurItem = NULL);
	CString LoadHistory(CRecentFileList* pListMRU, BOOL bSelectMostRecent = TRUE);
	virtual ~CHistoryCombo();

	// Generated message map functions
protected:
	CString m_sSection;
	CString m_sKeyPrefix;
	CString m_sKeyCurItem;
	BOOL m_bSaveRestoreLastCurrent;
	int m_nMaxHistoryItems;

	//{{AFX_MSG(CHistoryCombo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYCOMBO_H__EA3F7FA7_AA4D_11D4_A7CB_0000B48746CF__INCLUDED_)
