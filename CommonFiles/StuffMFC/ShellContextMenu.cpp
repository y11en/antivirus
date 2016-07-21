// ShellContextMenu.cpp: implementation of the CShellContextMenu class.
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

#include "stdafx.h"
#include "ShellContextMenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CShellContextMenu::InvokeCommand(UINT nCmdID)
{
	if (!IsValid() || !IsCommandInRange(nCmdID))
		return FALSE;

	CMINVOKECOMMANDINFO cmi;
	ZeroMemory(&cmi, sizeof(CMINVOKECOMMANDINFO));
	cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
	cmi.hwnd = m_pOwner->GetSafeHwnd();
	cmi.lpVerb = MAKEINTRESOURCE(nCmdID - m_nCmdFirstID);
	cmi.nShow = SW_SHOWNORMAL;

	return (NOERROR == m_pCtxMenu->InvokeCommand(&cmi));
}

BOOL CShellContextMenu::FillMenu(CMenu *pMenu, UINT nStartIndex, UINT nCmdFirstID, UINT nCmdLastID, UINT nFlags)
{
	if (!IsValid())
		return FALSE;

	m_nCmdFirstID = nCmdFirstID;
	m_nCmdLastID = nCmdLastID;

	UINT nDefID = pMenu->GetDefaultItem(GMDI_USEDISABLED, TRUE);

	HRESULT hr = m_pCtxMenu->QueryContextMenu(pMenu->GetSafeHmenu(),
		nStartIndex, m_nCmdFirstID, m_nCmdLastID, nFlags);

	if (nFlags & CMF_NODEFAULT)
		pMenu->SetDefaultItem(nDefID, TRUE);

	if (FAILED(hr))
	{
		m_nCmdLastID = m_nCmdFirstID = 0;
		return FALSE;
	}

	m_nCmdLastID = m_nCmdFirstID + HRESULT_CODE(hr) - 1;
	return TRUE;
}

BOOL CShellContextMenu::HandleMenuMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pLResult)
{
	if (!IsValid())
		return FALSE;

	switch (message)
	{
	case WM_MENUSELECT:
		{
			// look for a parent frame, to update its status bar
			if (m_pOwner == NULL)
				return FALSE;
			CFrameWnd* pFrame = m_pOwner->GetTopLevelFrame();
			if (pFrame == NULL)
				return FALSE;

			// provide status bar text
			*pLResult = 0;
			CString sStatusText;

			UINT uItem = (UINT) LOWORD(wParam);
			UINT fuFlags = (UINT) HIWORD(wParam);

			if (fuFlags == 0xFFFF)	// menu was closed
				return FALSE;

			// get the first item of submenu, if popup item
			if (fuFlags & MF_POPUP)
				uItem = ::GetMenuItemID(::GetSubMenu((HMENU)lParam, uItem), 0);

			// check if it's a valid item
			if (!(fuFlags & MF_SEPARATOR)
				&& (uItem == 0 || !IsCommandInRange(uItem)))
				return FALSE;

			// get description
			if (!(fuFlags & MF_SEPARATOR))
				GetCommandDescription(uItem, sStatusText);

			// update status text, even if empty
			pFrame->SetMessageText(sStatusText);

			return TRUE;
		}
		break;

	case WM_INITMENUPOPUP:	// initialize sub-menus
	case WM_MEASUREITEM:	// handle owner-draw items
	case WM_DRAWITEM:
		{
			// prepare result in case we handle the message
			// (if we don't, it will be discarded)
			*pLResult = TRUE;
			if (message == WM_INITMENUPOPUP)
				*pLResult = 0;

			// check if it's a menu message
			if (message == WM_MEASUREITEM
				&& ((LPMEASUREITEMSTRUCT)lParam)->CtlType != ODT_MENU)
				return FALSE;
			if (message == WM_DRAWITEM
				&& ((LPDRAWITEMSTRUCT)lParam)->CtlType != ODT_MENU)
				return FALSE;

			// these messages need interface version 2
			SContextMenu2Ptr pCtxMenu2(m_pCtxMenu);
			if (!pCtxMenu2.IsValid())
				return FALSE;

			return (NOERROR == pCtxMenu2->HandleMenuMsg(message, wParam, lParam));
		}
	}
	return FALSE;
}

BOOL CShellContextMenu::GetCommandDescription(UINT nCmdID, CString& rText)
{
	if (!IsValid() || !IsCommandInRange(nCmdID))
		return FALSE;

	// make a buffer large enough to hold either a Unicode or Ansi
	// string and initialize it to zeroes, because some shell
	// extensions return success even when they don't copy anything
	// to the buffer. Conversion is achieved by a CString object.
	WCHAR szHelpText[MAX_PATH];
	ZeroMemory(szHelpText, sizeof(WCHAR)*MAX_PATH);

	// try with Unicode first (it seems Explorer does so)
	HRESULT hr = m_pCtxMenu->GetCommandString(nCmdID - m_nCmdFirstID,
		GCS_HELPTEXTW, NULL, (LPSTR)szHelpText, MAX_PATH);
	if (*(LPWSTR)szHelpText != 0)	// buffer was used
		rText = (LPWSTR)szHelpText;
	else
	{
		// otherwise try with Ansi
		hr = m_pCtxMenu->GetCommandString(nCmdID - m_nCmdFirstID,
			GCS_HELPTEXTA, NULL, (LPSTR)szHelpText, MAX_PATH);
		if (*(LPSTR)szHelpText != 0)	// buffer was used
			rText = (LPSTR)szHelpText;
	}

	return SUCCEEDED(hr);
}

BOOL CShellContextMenu::Create(IShellFolder *pParentFolder, LPCITEMIDLIST pidl)
{
	m_pCtxMenu = SContextMenuPtr(pParentFolder, pidl);
	return m_pCtxMenu.IsValid();
}

// a NULL pointer to unsubclass
void CShellContextMenu::SetOwner(CWnd *pOwner)
{
	if (m_pOwner != NULL || pOwner == NULL)
		UnsubclassOwner();

	m_pOwner = pOwner;
	if (m_pOwner != NULL)
		SubclassOwner();
}

const LPCTSTR SH_CTXMENU_OBJ = _T("ShellContextMenu");

void CShellContextMenu::SubclassOwner()
{
	ASSERT(::IsWindow(m_pOwner->GetSafeHwnd()));

	m_oldWndProc = (WNDPROC)::GetWindowLong(*m_pOwner, GWL_WNDPROC);
	::SetProp(*m_pOwner, SH_CTXMENU_OBJ, (HANDLE)this);
	::SetWindowLong(*m_pOwner, GWL_WNDPROC, (LONG)WindowProc);
}

LRESULT CALLBACK CShellContextMenu::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CShellContextMenu *pThis =
		(CShellContextMenu*)::GetProp(hwnd, SH_CTXMENU_OBJ);

	LRESULT lResult;
	if (pThis->HandleMenuMessage(message, wParam, lParam, &lResult))
		return lResult;	// success, no default processing

	if (message == WM_DESTROY)
		pThis->UnsubclassOwner();

	return ::CallWindowProc(pThis->m_oldWndProc, hwnd, message, wParam, lParam);
}

void CShellContextMenu::UnsubclassOwner()
{
	ASSERT(::IsWindow(m_pOwner->GetSafeHwnd()));

	::RemoveProp(*m_pOwner, SH_CTXMENU_OBJ);
	::SetWindowLong(*m_pOwner, GWL_WNDPROC, (LONG)m_oldWndProc);
	m_pOwner = NULL;
}
