// SubclassedBase.cpp: implementation of the CSubclassedBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SubclassedBase.h"

LRESULT CSubclassedBase::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	PostMessage(WM_AFTER_INITDIALOG, 0, 0);
	
	bHandled = TRUE;
	return lRes;	
}

LRESULT CSubclassedBase::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BeforeDestroy();

	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	bHandled = TRUE;
	return lRes;	
}

LRESULT CSubclassedBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	
	bHandled = TRUE;
	return lRes;	
}

LRESULT CSubclassedBase::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	if (GetWindowLong(GWL_STYLE) & WS_VISIBLE)
//		SetWindowLong(GWL_STYLE, GetWindowLong(GWL_STYLE) & ~WS_VISIBLE);

	SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) | WS_EX_TRANSPARENT);
	
	return S_OK;	
}

LRESULT CSubclassedBase::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BeforeTimer((UINT)wParam);
	
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	bHandled = TRUE;
	return lRes;
}

LRESULT CSubclassedBase::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPWINDOWPOS lpwp = reinterpret_cast<LPWINDOWPOS>(lParam);
	lpwp->flags &= ~SWP_SHOWWINDOW;
	return S_OK;
}
