// ShellContextMenu.h: interface for the CShellContextMenu class.
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLCONTEXTMENU_H__B690453B_A97A_4D33_A6A5_0A0BDE5EB1A4__INCLUDED_)
#define AFX_SHELLCONTEXTMENU_H__B690453B_A97A_4D33_A6A5_0A0BDE5EB1A4__INCLUDED_

#include <shlobj.h>
#include "ShellWrappers.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShellContextMenu  
{
protected:
	SContextMenuPtr m_pCtxMenu;
	UINT m_nCmdFirstID;
	UINT m_nCmdLastID;
	CWnd* m_pOwner;		// NULL means no owner (no subclassing in act)
	WNDPROC m_oldWndProc;

public:
	BOOL Create(IShellFolder* pParentFolder, LPCITEMIDLIST pidl);
	CShellContextMenu();
	virtual ~CShellContextMenu();

	BOOL InvokeCommand(UINT nCmdID);

	BOOL FillMenu(CMenu *pMenu, UINT nStartIndex = 0, UINT nCmdFirstID = 1, UINT nCmdLastID = 0x7FFF, UINT nFlags = 0);
	
	BOOL HandleMenuMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pLResult);

	BOOL GetCommandDescription(UINT nCmdID, CString& rText);

	BOOL IsValid();

	// commands
	void GetCommandRange(UINT& nCmdFirstID, UINT& nCmdLastID);
	BOOL IsCommandInRange(UINT nCmdID);

	// owner
	void SetOwner(CWnd *pOwner);
	CWnd* GetOwner();

protected:
	void SubclassOwner();
	void UnsubclassOwner();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

// inline functions

inline CShellContextMenu::CShellContextMenu()
{
	m_nCmdFirstID = 0;
	m_nCmdLastID = 0;
	m_pOwner = NULL;
	m_oldWndProc = NULL;
}

inline CShellContextMenu::~CShellContextMenu()
{
	if (m_pOwner != NULL)
		UnsubclassOwner();
}

inline BOOL CShellContextMenu::IsValid()
{
	return (m_pCtxMenu.IsValid());
}

inline void CShellContextMenu::GetCommandRange(UINT &nCmdFirstID, UINT &nCmdLastID)
{
	nCmdFirstID = m_nCmdFirstID;
	nCmdLastID = m_nCmdLastID;
}

inline BOOL CShellContextMenu::IsCommandInRange(UINT nCmdID)
{
	return IsValid() && (nCmdID >= m_nCmdFirstID && nCmdID <= m_nCmdLastID);
}

inline CWnd* CShellContextMenu::GetOwner()
{
	return m_pOwner;
}

#endif // !defined(AFX_SHELLCONTEXTMENU_H__B690453B_A97A_4D33_A6A5_0A0BDE5EB1A4__INCLUDED_)
