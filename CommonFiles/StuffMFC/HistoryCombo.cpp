////////////////////////////////////////////////////////////////////////////
// File:	HistoryCombo.cpp
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

#include "stdafx.h"
#include "HistoryCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_HISTORY_ITEMS 10

#include <atlbase.h>  // required for CRegKey

/////////////////////////////////////////////////////////////////////////////
// CHistoryCombo

CHistoryCombo::CHistoryCombo()
{
  m_nMaxHistoryItems = MAX_HISTORY_ITEMS;
  m_bSaveRestoreLastCurrent = TRUE;
}

CHistoryCombo::~CHistoryCombo()
{
}

BOOL CHistoryCombo::PreCreateWindow(CREATESTRUCT& cs) 
{
  // warn if creating with CBS_SORT style
  // (unfortunately we can't turn it off)
  if (cs.style & CBS_SORT)
    TRACE("WARNING: Creating History combo with CBS_SORT style\n");
  return CComboBox::PreCreateWindow(cs);
}

BEGIN_MESSAGE_MAP(CHistoryCombo, CComboBox)
	//{{AFX_MSG_MAP(CHistoryCombo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoryCombo message handlers

// this version of AddString adds a string only if it doesn't already exist
// in the list, and in any case, makes sure that the string is the first
// in the list (ie most recent in history)
// also makes sure number of items in the list doesn't exceed the maximum allowed
int CHistoryCombo::AddString(LPCTSTR lpszString)
{
  // if it's not set up as a history combo then call base class
  if (m_sSection.IsEmpty() || m_sKeyPrefix.IsEmpty())
    return CComboBox::AddString(lpszString);

  int nRet = -1;
  // don't add it already there
  CString sString(lpszString);
  sString.TrimLeft(" ");
  sString.TrimRight(" ");
  nRet = CComboBox::InsertString(0, sString);
  int nIndex = FindStringExact(0, sString);
  if (nIndex != -1 && nIndex != 0)
    DeleteString(nIndex);
  SetCurSel(nRet);
  return nRet;
}

// loads the history from the specified MRU list, and populates the combo list
// if bSelectMostRecent is TRUE, selects the most recent into the edit control
// returns the selected text
// if using this overload of LoadHistory then SaveHistory will not function
// but you should simply save the MRU in the normal way instead
// note that the MRU should have already been read from the profile before
// being passed to this function, as this function will not call MRU->ReadList()
CString CHistoryCombo::LoadHistory(CRecentFileList* pListMRU, BOOL bSelectMostRecent/*=TRUE*/)
{
  if (pListMRU == NULL)
    return "";

  int nNumItems = pListMRU->GetSize();
  for (int n = 0; n < nNumItems; n++)
    CComboBox::AddString((*pListMRU)[n]);
  if (bSelectMostRecent)
    SetCurSel(0);
  CString sText;
  GetWindowText(sText);
  return sText;
}

// loads the history from the specified profile area, and returns the 
// text selected
// the profile area is kept so that in doesn't need to specified again
// when saving the history
CString CHistoryCombo::LoadHistory(LPCTSTR lpszSection, LPCTSTR lpszKeyPrefix, 
				   BOOL bSaveRestoreLastCurrent/*=TRUE*/, 
				   LPCTSTR lpszKeyCurItem/*=NULL*/)
{
  if (lpszSection == NULL || lpszKeyPrefix == NULL || *lpszSection == '\0')
    return "";

  m_sSection = lpszSection;
  m_sKeyPrefix = lpszKeyPrefix;
  m_sKeyCurItem = lpszKeyCurItem == NULL ? "" : lpszKeyCurItem;
  m_bSaveRestoreLastCurrent = bSaveRestoreLastCurrent;
  CWinApp* pApp = AfxGetApp();

  int n = 0;
  CString sText;
  do
  {
    CString sKey;
    sKey.Format("%s%d", m_sKeyPrefix, n++);
    sText = pApp->GetProfileString(m_sSection, sKey);
    if (!sText.IsEmpty())
      CComboBox::AddString(sText);
  }while (!sText.IsEmpty());
  if (m_bSaveRestoreLastCurrent)
  {
    CString sKey;
    if (!m_sKeyCurItem.IsEmpty())
      sKey = m_sKeyCurItem;
    else if (m_sKeyPrefix.IsEmpty())
      sKey = "Last";
    else
      sKey = m_sKeyPrefix;
    sText = pApp->GetProfileString(m_sSection, sKey);
    if (!sText.IsEmpty())
    {
      int nIndex = FindStringExact(-1, sText);
      if (nIndex != -1)
	SetCurSel(nIndex);
      else if (GetStyle() & CBS_DROPDOWN)
	SetWindowText(sText);
    }
  }
  return sText;
}

// saves the history to the profile specified when calling LoadHistory
// if no profile information (ie LoadHistory() wasn't called with it) then
// this function does nothing
void CHistoryCombo::SaveHistory(BOOL bAddCurrentItemToHistory/*=TRUE*/)
{
  if (m_sSection.IsEmpty())
    return;

  CWinApp* pApp = AfxGetApp();

  if (bAddCurrentItemToHistory)
  {
    CString sCurItem;
    GetWindowText(sCurItem);
    // trim it, so we items which differ only by a leading/trailing space
    sCurItem.TrimLeft();
    sCurItem.TrimRight();
    if (! sCurItem.IsEmpty())
      AddString(sCurItem);
  }

  // save history to info cached earlier
  int nMax = min(GetCount(), m_nMaxHistoryItems + 1);
  for (int n = 0; n < nMax; n++)
  {
    CString sKey;
    sKey.Format("%s%d", m_sKeyPrefix, n);
    CString sText;
    GetLBText(n, sText);
    pApp->WriteProfileString(m_sSection, sKey, sText);
  }
  if (m_bSaveRestoreLastCurrent)
  {
    CString sText;
    GetWindowText(sText);
    CString sKey;
    if (!m_sKeyCurItem.IsEmpty())
      sKey = m_sKeyCurItem;
    else if (m_sKeyPrefix.IsEmpty())
      sKey = "Last";
    else
      sKey = m_sKeyPrefix;
    pApp->WriteProfileString(m_sSection, sKey, sText);
  }
}

// removes all the items from the history list, and optionally deletes
// the registry items. Note that if the history list is generated from
// a CRecentFileList, then registry entries will not be deleted
void CHistoryCombo::ClearHistory(BOOL bDeleteRegistryEntries/*=TRUE*/)
{
  ResetContent();
  if (! m_sSection.IsEmpty() && bDeleteRegistryEntries)
  {
    // get the actual reg key used
    CWinApp* pApp = AfxGetApp();
    CRegKey rk;
    CString sKey = "SOFTWARE\\";
    if (pApp->m_pszRegistryKey == NULL || pApp->m_pszAppName == NULL)
      return;
    sKey += pApp->m_pszRegistryKey + CString("\\");
    sKey += pApp->m_pszAppName + CString("\\");
    sKey += m_sSection;
    if (rk.Open(HKEY_CURRENT_USER, sKey) != ERROR_SUCCESS)
      return;
    // delete actual values
    int nMax = m_nMaxHistoryItems + 1;
    for (int n = 0; n < nMax; n++)
    {
      sKey.Format("%s%d", m_sKeyPrefix, n);
      rk.DeleteValue(sKey);
    }
    if (!m_sKeyCurItem.IsEmpty())
      sKey = m_sKeyCurItem;
    else if (m_sKeyPrefix.IsEmpty())
      sKey = "Last";
    else
      sKey = m_sKeyPrefix;
    rk.DeleteValue(sKey);
  }
}
