// WinDlg.cpp: implementation of the CWinCtrls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winroot.h"

//////////////////////////////////////////////////////////////////////

typedef std::vector<HWND>				   tDialogsList;
typedef std::map<tString, tDialogsList>    tDialogsMap;

static tDialogsMap	g_dlgMap;

//////////////////////////////////////////////////////////////////////
// CDialogItemImpl

CDialogItemImpl::CDialogItemImpl() :
	m_bGripperOn(0), m_bEndSession(0), m_bInited(0), m_bEnableParent(0), m_nCurShowState(-1)
{
	m_nStyles = WS_CAPTION|WS_SYSMENU|WS_OVERLAPPED|WS_POPUP;
}

void CDialogItemImpl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	sswitch(strStyle)
	scase("nocaption")   m_nStyles &= ~(WS_CAPTION|WS_SYSMENU); sbreak;
	scase("resizable")   m_nStyles |= WS_THICKFRAME|WS_MAXIMIZEBOX; sbreak;
	scase("minimizable") if( !m_pParent || m_pParent->GetOwner(true) == m_pRoot ) m_nStyles |= WS_MINIMIZEBOX; sbreak;
	scase("ontop")       if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) ) m_nStylesEx |= WS_EX_TOPMOST; sbreak;
	scase("layered")     if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) ) { m_nStylesEx |= WS_EX_TOPMOST|WS_EX_LAYERED|WS_EX_TOOLWINDOW; m_nStylesEx &= ~WS_EX_APPWINDOW; /*m_nFlags |= STYLE_NO_CLIPING;*/ } sbreak;
	scase(STR_PID_NOACTIVATE) m_nStylesEx |= WS_EX_NOACTIVATE; sbreak;
	send
}

void CDialogItemImpl::InitItem(LPCSTR strType)
{
	m_bSmartTransparency = (!m_pRoot->m_pSink || ((CRootSink *)m_pRoot->m_pSink)->IsTransparentEnabled());
	if( ((CWinRoot*)m_pRoot)->m_nOsVer < GUI_OSVER_WINXP )
		m_bSmartTransparency = false;

	if( (m_nStylesEx & WS_EX_LAYERED) && !m_bSmartTransparency )
		m_nStylesEx &= ~WS_EX_LAYERED;
	
	CDialogItem::InitItem(strType);
	RECT rcFrame = {0, 0, 0, 0};
	
	HWND hParentWnd = GetParentWindow();
	if( m_nStylesEx & WS_EX_TOOLWINDOW )
	{
		m_nStylesEx &= ~WS_EX_TOOLWINDOW;
		m_HiddenParent.Create(m_pRoot, NULL, m_HiddenParent.GetWndClassInfo(), rcFrame, WS_OVERLAPPED);
		// EXPLOIT (access violation on SW_SHOW for a parent) FIX:
		m_HiddenParent.m_pRoot = m_pRoot;
		hParentWnd = m_HiddenParent.m_hWnd;
	}
	
	Create(m_pRoot, hParentWnd, GetWndClassInfo(), rcFrame, m_nStyles & ~WS_VISIBLE, m_nStylesEx, (UINT)m_strSection.c_str());

	m_pOwner = NULL;
	((CWinRoot*)m_pRoot)->EnableThemeDialogTexture(m_hWnd, false, false);
}

void CDialogItemImpl::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	if( m_pDlgIcon )
	{
		CIcon* pBigIcon = m_pRoot->GetIcon(ICONID_DEFAULT".big");
		if( !pBigIcon )
			pBigIcon = m_pDlgIcon;

		CWindow::SetIcon(pBigIcon->IconHnd(), TRUE);
		CWindow::SetIcon(m_pDlgIcon->IconHnd(), FALSE);

		if( m_HiddenParent )
		{
			m_HiddenParent.CWindow::SetIcon(pBigIcon->IconHnd(), TRUE);
			m_HiddenParent.CWindow::SetIcon(m_pDlgIcon->IconHnd(), FALSE);
		}
	}
}

void CDialogItemImpl::Cleanup()
{
	TBase::Cleanup();
	if( m_HiddenParent )
		m_HiddenParent.DestroyWindow();
}

void CDialogItemImpl::ReLoad()
{
	TBase::ReLoad();

	if( m_bEnableParent )
		ModalEnableParent(false);
}

tUINT CDialogItemImpl::_GetDlgPosOffset()
{
	size_t nDlgPos = 0;
	
	tString strDlgId; GetOwnerRelativePath(strDlgId, this, true);
	std::pair<tDialogsMap::iterator, bool> find =
		g_dlgMap.insert(tDialogsMap::value_type(strDlgId, tDialogsList()));
	
	tDialogsList& vecHwnd = find.first->second;
	
	nDlgPos = m_bInited ? 0 : -1;
	for(size_t i = 0; i < vecHwnd.size() && nDlgPos == -1; i++)
	{
		HWND& dlg = vecHwnd[i];
		if( !::IsWindow(dlg) )
		{
			nDlgPos = i;
			dlg = GetWindow();
		}
	}
	
	if( nDlgPos == -1 )
	{
		nDlgPos = vecHwnd.size();
		vecHwnd.push_back(m_hWnd);
	}

	return (tUINT)nDlgPos;
}

bool CDialogItemImpl::_CheckDlgPosOffset()
{
	tString strDlgId; GetOwnerRelativePath(strDlgId, this, true);
	tDialogsMap::iterator find = g_dlgMap.find(strDlgId);
	if( find == g_dlgMap.end() )
		return true;

	tDialogsList& vecHwnd = find->second;

	int nCount = 0;
	for(size_t i = 0; i < vecHwnd.size(); i++)
	{
		HWND &dlg = vecHwnd[i];
		if( dlg == GetWindow() )
			dlg = NULL;
		else if( ::IsWindow(dlg) )
			nCount++;
	}

	return !nCount;
}

void CDialogItemImpl::_InitDlgRect(RECT& rcFrame)
{
	int nFlags = SWP_NOACTIVATE|SWP_NOREPOSITION|SWP_NOZORDER;
	if( !(m_nDlgStyles & DLG_STYLE_RESIZABLE) || (m_nDlgStyles & DLG_STYLE_LAYERED) )
		nFlags |= SWP_NOSIZE;

	HWND hWndInsertAfter = NULL;
	if( (m_nStylesEx & WS_EX_TOPMOST) && (m_nDlgStyles & DLG_STYLE_NOAUTOFOCUS) )
	{
		POINT pt = {0, 0}; RECT rcTop;
		HWND hWnd = ::WindowFromPoint(pt);
		if( hWnd && ::GetWindowRect(hWnd, &rcTop) )
		{
			if( rcTop.bottom >= GetSystemMetrics(SM_CYSCREEN) &&
				rcTop.right >= GetSystemMetrics(SM_CXSCREEN) )
			{
				hWndInsertAfter = HWND_NOTOPMOST;
//				hWndInsertAfter = ::GetWindow(hWnd, GW_HWNDPREV);
				nFlags &= ~SWP_NOZORDER;
			}
		}
	}

	SetWindowPos(hWndInsertAfter, &rcFrame, nFlags);
	m_bInited = 1;
}

void CDialogItemImpl::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	UpdateWindowProps(flags, nStateMask & ~ISTATE_HIDE);
	
	int nUpdFlags = flags & ~(UPDATE_FLAG_TEXT|UPDATE_FLAG_FONT|UPDATE_FLAG_ICON|UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE);
	if( nUpdFlags )
		TBase::OnUpdateProps(nUpdFlags, nStateMask & ~ISTATE_HIDE, rc);

	if( nStateMask & ISTATE_HIDE )
	{
		if( m_nState & ISTATE_HIDE )
			::ShowWindow(m_hWnd, SW_HIDE);
		else if( m_pRoot->m_bRefreshRequest )
			::ShowWindow(m_hWnd, SW_SHOWNA);
	}
}

void CDialogItemImpl::OnAdjustBordSize(RECT& rc)
{
	::AdjustWindowRect(&rc, ::GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
}

//////////////////////////////////////////////////////////////////////

bool CDialogItemImpl::IsActive()
{
	return m_hWnd == ::GetActiveWindow();
}

void CDialogItemImpl::Activate()
{
	if( !m_hWnd )
		return;

	for(size_t i = 0, n = m_aItems.size(); i < n; i++)
	{
		CDialogItem * pDlg = (CDialogItem *)m_aItems[i];
		if( (pDlg->m_nFlags & STYLE_TOP_ITEM) && pDlg->m_fModal )
		{
			pDlg->Activate();
			return;
		}
	}
	
	bool  bOwnInput = false;
	DWORD nFGThread = 0;
    DWORD nFGProcess = 0;
	HWND  hFGWnd = ::GetForegroundWindow();

	if( hFGWnd == m_hWnd )
		bOwnInput = true;
	else
	{
		if( hFGWnd && (nFGThread = GetWindowThreadProcessId(hFGWnd, &nFGProcess)) )
			if( nFGProcess == GetCurrentProcessId() )
				bOwnInput = true;
	}

	FlashWindow(FALSE);
	if( IsIconic() )
		ShowWindow(SW_RESTORE);
	else
	{
		_SetTransparent(IsActual() ? TRANS_FULL : TRANS_MED);

		if( m_nDlgStyles & DLG_STYLE_LAYERED )
		{
		}
		else if( !(m_nDlgStyles & DLG_STYLE_NOAUTOFOCUS) )
		{
			if( !bOwnInput )
				::AttachThreadInput(nFGThread, GetCurrentThreadId(), TRUE);

			::SetForegroundWindow(m_hWnd);

			if( !bOwnInput )
				::AttachThreadInput(nFGThread, GetCurrentThreadId(), FALSE);
		}
		
		if( !IsWindowVisible() )
		{
			tDWORD nShowFlag = m_nFlags & STYLE_NOACTIVATE ? SW_SHOWNOACTIVATE : (m_bMaximize ? SW_MAXIMIZE : SW_SHOWNOACTIVATE);
			ShowWindow(nShowFlag);
		}
		m_bMaximize = 0;
	}

	UpdateWindow();
	if( !(m_nDlgStyles & DLG_STYLE_NOAUTOFOCUS) )
	{
		// Это была попытка сделать асинхронный SetFocus из-за проблем с дедлоком из скриптчекера
		/*
		HWND hendPrev = ::GetFocus();
		::PostMessage(hendPrev, WM_KILLFOCUS, 0, 0);
		::PostMessage(m_hWnd, WM_SETFOCUS, 0, 0);
*/
		SetFocus();
	}
}

void CDialogItemImpl::GetParentRect(RECT& rc, CItemBase* pOwner)
{
	if( pOwner != m_pRoot )
	{
		TBase::GetParentRect(rc, pOwner);
		return;
	}

	POINT pt = {0, 0};
	::ClientToScreen(m_hWnd, &pt);

	rc.left = pt.x;
	rc.top = pt.y;
	rc.right = rc.left + m_szSize.cx;
	rc.bottom = rc.top + m_szSize.cy;
}

void CDialogItemImpl::Close(tDWORD nAction)
{
	if( !m_hWnd )
		return;
	
	m_nResult = nAction;
	m_bCloseDirect = true;
	
	if( ((CWinRoot *)m_pRoot)->m_bActive )
		PostMessage(WM_COMMAND, MAKELONG(IDCANCEL,0), 0);
	else
		_CloseDlg();
}

bool CDialogItemImpl::_CloseDlg()
{
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::CDialogItem(%s) closing: bUserClose = %u, nResult = %u", m_strItemId.c_str(), (int)m_bUserClose, m_nResult));
	
	bool bUserClose = m_bUserClose || m_bEndSession;
	bool bAppClose = !((CWinRoot *)m_pRoot)->m_bActive || m_bEndSession;
	bool bAbandoned = false;
	if( (!m_bUserClose || m_bEndSession) && (m_nResult == DLG_ACTION_CANCEL) )
		bAbandoned = true;
	m_bUserClose = 0;
	m_bEndSession = 0;
	
	if( m_pSink )
	{
		if( !m_bCloseDirect )
		{
			CItemBase * p = this;
			while( p )
			{
				if( p->m_pBinding )
				{
					cVar vRes; p->m_pBinding->ProcessReflection(CItemBinding::hOnClose, vRes);
					if( vRes.Type() == cVariant::vtInt && !vRes.ToDWORD() )
						return false;
				}
				
				p = EnumNextItem(p);
			}

			if( !m_pSink->OnCanClose(bUserClose) )
				return false;
		}
		
		if( !bAppClose && !((CDialogSink *)m_pSink)->OnClose(m_nResult) )
			return false;
	}
	
	if( bAbandoned )
		SetErrResult(errABANDONED);

	ModalEnableParent(true);
	DestroyWindow();
	return true;
}

void CDialogItemImpl::SetShowMode(eShow eShowMode)
{
	TBase::SetShowMode(eShowMode);

	switch( eShowMode )
	{
	case showNormal:    ::ShowWindow(m_hWnd, SW_RESTORE); break;
	case showMaximazed: ::ShowWindow(m_hWnd, SW_MAXIMIZE); break;
	case showMinimized: ::ShowWindow(m_hWnd, SW_MINIMIZE); break;
	}
}

bool CDialogItemImpl::SetNextDlgItem(bool bGroup, bool bBackward)
{
	CItemBase * pPrev = GetItem(::GetFocus());
	if( !pPrev )
		return false;
	
	bool bForce = false;
	CItemBase * pNext = pPrev;
	while( pNext = GetItem(::GetNextDlgTabItem(m_hWnd, pNext->GetWindow(), bBackward), pNext, bBackward) )
	{
		if( pNext == pPrev )
			break;
		
		bool bSameGroup = pNext->IsSameTabGroup(pPrev);
		if( bGroup )
		{
			if( bSameGroup )
			{
				bForce = true;
				continue;
			}
		}
		else
		{
			if( !bSameGroup )
			{
				bForce = true;
				continue;
			}
		}
		
		break;
	}
	
	if( !pNext || !bForce )
		return false;
	
	pNext->Focus(true);
	return true;
}

void CDialogItemImpl::ctl_SetTransparentValue(tBYTE nAlpha) { ((CWinRoot *)m_pRoot)->SetLayeredWindowAttributes(m_hWnd, nAlpha); Update(NULL); }
tDWORD CDialogItemImpl::ctl_SetTransparentTimer(tDWORD nInterval) { return (tDWORD)SetTimer(1, nInterval); }
void CDialogItemImpl::ctl_KillTransparentTimer(tDWORD nId) { KillTimer(nId); }

void CDialogItemImpl::_UpdateRgn()
{
	if( !(m_nDlgStyles & DLG_STYLE_ROUNDED) )
		return;

    // Выставляем скругленные углы только при наличии специального атрибута "rounded" и в случае включенных тем
	if( (!GUI_ISSTYLE_SIMPLE(m_pRoot) || m_pRoot->IsThemed()) )
	{
	    RECT rcFrame; GetWindowRect(&rcFrame);
    	
	    HRGN hRgn = ::CreateRectRgn(0, 0, 0, 0);
	    {
		    RECT rcRgn = {rcFrame.left, rcFrame.top, rcFrame.right + 1, rcFrame.bottom + 1};
    		
		    HRGN hRgnTop = ::CreateRectRgn(0, 0, RECT_CX(rcRgn), RECT_CY(rcRgn)/2);
		    HRGN hRgnBottom = ::CreateRectRgn(0, RECT_CY(rcRgn)/2, RECT_CX(rcRgn), RECT_CY(rcRgn));

		    HRGN hRgn1n = ::CreateRoundRectRgn(0, 0, RECT_CX(rcRgn), RECT_CY(rcRgn), 5, 5);
		    HRGN hRgn2n = ::CreateRoundRectRgn(0, 0, RECT_CX(rcRgn), RECT_CY(rcRgn), 11, 11);

		    HRGN hRgn1 = ::CreateRectRgn(0, 0, 0, 0);
		    HRGN hRgn2 = ::CreateRectRgn(0, 0, 0, 0);
		    ::CombineRgn(hRgn1, hRgn1n, hRgnBottom, RGN_DIFF);
		    ::CombineRgn(hRgn2, hRgn2n, hRgnTop, RGN_DIFF);
    		
		    ::DeleteObject(hRgn1n);
		    ::DeleteObject(hRgn2n);
		    ::DeleteObject(hRgnTop);
		    ::DeleteObject(hRgnBottom);

		    ::CombineRgn(hRgn, hRgn1, hRgn2, RGN_OR);
		    ::DeleteObject(hRgn1);
		    ::DeleteObject(hRgn2);
	    }
    	
	    SetWindowRgn(hRgn);
    }
    else
        SetWindowRgn(NULL);
}

void CDialogItemImpl::DrawBackground(HDC dc, RECT& rcClient, RECT * prcClip)
{
	TBase::DrawBackground(dc, rcClient, prcClip);
	
	if( m_nDlgStyles & DLG_STYLE_RESIZABLE )
	{
		RECT rcGripper = {rcClient.right - 16, rcClient.bottom - 16, rcClient.right, rcClient.bottom};

		if( GUI_ISSTYLE_SIMPLE(m_pRoot) )
		{
			((CWinRoot *)m_pRoot)->DrawDlgGripper(dc, rcGripper);
			return;
		}
		
		CIcon * pGripper = m_pRoot->GetIcon("gripper");
		if( pGripper )
			pGripper->Draw(dc, rcGripper);
	}
}

void CDialogItemImpl::ModalEnableParent(bool bEnable)
{
	HWND hParent = ::GetParent(m_hWnd);
	if( !hParent )
		return;

	if( bEnable )
	{
		if( m_bEnableParent )
			::EnableWindow(hParent, TRUE);

		if( ::GetActiveWindow() == m_hWnd )
			::SetActiveWindow(hParent);
	}
	else
	{
		if( ::IsWindowEnabled(hParent) )
		{
			::EnableWindow(hParent, FALSE);
			m_bEnableParent = 1;
		}
	}
}

bool CDialogItemImpl::PreTranslateMessage(MSG& msg)
{
	if( msg.message == WM_KEYDOWN && (m_hWnd == msg.hwnd || ::IsChild(m_hWnd, msg.hwnd)) )
	{
		bool bTabOrder = true, bBackward;
		switch(msg.wParam)
		{
		case VK_UP:    bBackward = true; break;
		case VK_DOWN:  bBackward = false; break;
		case VK_TAB:   bBackward = ::GetKeyState(VK_SHIFT) < 0; break;
		case VK_LEFT:  bBackward = true; break;
		case VK_RIGHT: bBackward = false; break;
		case VK_F5:
		case VK_F12:
			if( ::GetKeyState(VK_SHIFT) < 0 )
			{
				m_pRoot->Refresh(REFRESH_PARAMS|REFRESH_SKIN_DATA|(msg.wParam == VK_F12 ? REFRESH_DEFLOC : 0));
				((CWinRoot*)m_pRoot)->RefreshDialog(this);
			}

		default: bTabOrder = false;
		}

		if( msg.wParam == VK_TAB && ::GetKeyState(VK_CONTROL) < 0 )
		{
			if( m_pSheet && !(m_nFlags & STYLE_SHEET_ITEM) )
			{
				m_pSheet->SetNextPage(bBackward);
				return true;
			}
		}
		
		if( bTabOrder && !(::SendMessage(msg.hwnd, WM_GETDLGCODE, 0, (LPARAM)&msg) &
										(DLGC_WANTARROWS|DLGC_WANTTAB|DLGC_WANTALLKEYS)) )
			if( SetNextDlgItem(::GetKeyState(VK_CONTROL) < 0, bBackward) )
				return true;
	}

	return !!::IsDialogMessage(m_hWnd, &msg);
}

//////////////////////////////////////////////////////////////////////

LRESULT CDialogItemImpl::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WINDOWPOS& _Wp = *(WINDOWPOS *)lParam;

	if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) && (m_nState & ISTATE_INITED) &&
		(m_nDlgStyles & DLG_STYLE_BOTTOM) &&
		!(_Wp.flags & SWP_NOSIZE) )
	{
		RECT rcOld; GetWindowRect(&rcOld);
		if( _Wp.cx != RECT_CX(rcOld) || _Wp.cy != RECT_CY(rcOld) )
		{
			_Wp.flags &= ~SWP_NOMOVE;
			_Wp.x = rcOld.left - (_Wp.cx - RECT_CX(rcOld));
			_Wp.y = rcOld.top  - (_Wp.cy - RECT_CY(rcOld));
		}
	}
	
	// For that in SetWindowPos don't call WM_GETMINMAXINFO
	return 1;
}

LRESULT CDialogItemImpl::OnGetDefId(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( CItemBase* pItem = GetDefItem() )
		return MAKELONG(pItem->GetItemId(), DC_HASDEFID);
	return 0;
}

LRESULT CDialogItemImpl::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( LOWORD(wParam) == IDCANCEL )
	{
		m_bUserClose = 1;
		if( !_CloseDlg() )
			return bHandled = FALSE, 0;
		return 0;
	}
	
	return bHandled = FALSE, 0;
}

LRESULT CDialogItemImpl::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE )
		m_bUserClose = 1;
	
	return bHandled = FALSE, 0;
}

LRESULT CDialogItemImpl::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !_CloseDlg() )
		return 0;
	return bHandled = FALSE, 0;
}

LRESULT CDialogItemImpl::OnQueryEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::CDialogItemImpl(%s)::WM_QUERYENDSESSION(wParam=0x%X, lParam=0x%X)", m_strItemId.c_str(), wParam, lParam));
	
	m_bEndSession = 1;
	if( !_CloseDlg() )
		return FALSE;
	return bHandled = FALSE, TRUE;
}

LRESULT CDialogItemImpl::OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( CItemBase * pHelp = GetItem("Help") )
	{
		if( !(pHelp->m_nState & ISTATE_HIDE) )
			pHelp->Click();
	}
//	else
//		m_pRoot->DoCommand(this, "help:");

	return bHandled = TRUE, 0;
}

LRESULT CDialogItemImpl::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	DeInit();
	return 0;
}

LRESULT CDialogItemImpl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_nCurShowState != wParam )
	{
		switch(wParam)
		{
		case SIZE_MAXIMIZED: OnChangeShowMode(showMaximazed); break;
		case SIZE_MINIMIZED: OnChangeShowMode(showMinimized); break;
		case SIZE_RESTORED:  OnChangeShowMode(showNormal); break;
		}

		m_nCurShowState = wParam;
	}
	
	if( wParam != SIZE_RESTORED && wParam != SIZE_MAXIMIZED )
		return( 1 );
		
	SIZE l_szNew = {LOWORD(lParam), HIWORD(lParam)};
	if( !l_szNew.cy || !l_szNew.cx )
		return 0;

	if( (l_szNew.cx - m_szSize.cx) || (l_szNew.cy - m_szSize.cy) )
	{
		LockUpdate(true);
		Resize(l_szNew.cx - m_szSize.cx, l_szNew.cy - m_szSize.cy, false);
		LockUpdate(false);
	}

	_UpdateRgn();
	return( 0 );
}

LRESULT CDialogItemImpl::OnSysColorChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_UpdateRgn();
	Update(NULL);
	return S_OK;
}

LRESULT CDialogItemImpl::OnGetMinmaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szWndMin; GetMinSize(szWndMin);
	{
		RECT rcAdj = {0, 0, 0, 0}; ::AdjustWindowRect(&rcAdj, ::GetWindowLong(m_hWnd, GWL_STYLE), FALSE);
		szWndMin.cx += RECT_CX(rcAdj);
		szWndMin.cy += RECT_CY(rcAdj);
	}
	
	MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;
	pMinMax->ptMinTrackSize.x = szWndMin.cx;
	if( !IsResizeble(true) )
		pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
	pMinMax->ptMinTrackSize.y = szWndMin.cy;
	if( !IsResizeble(false) )
		pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
	return 0;
}

LRESULT CDialogItemImpl::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ctl_OnActivate(LOWORD(wParam) != WA_INACTIVE);
	return bHandled = FALSE, 0;
}

LRESULT CDialogItemImpl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_nDlgStyles & DLG_STYLE_RESIZABLE )
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		RECT rcClient = {m_szSize.cx-16, m_szSize.cy-16, m_szSize.cx, m_szSize.cy};

		if( m_bGripperOn = !!PtInRect(&rcClient, pt) )
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
	}

	if( m_nDlgStyles & DLG_STYLE_LAYERED )
	{
		if( !(m_nState & ISTATE_MOUSEOVER) )
		{
			StartTrackMouseLeave();
			m_nState |= ISTATE_MOUSEOVER;
		}
	}

	ctl_OnMouseMove();
	return bHandled = FALSE, S_OK;
}

LRESULT CDialogItemImpl::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_nDlgStyles & DLG_STYLE_LAYERED )
	{
		POINT pt; GetCursorPos(&pt);
		RECT rcWindow; GetWindowRect(&rcWindow);

		m_nState &= ~ISTATE_MOUSEOVER;
	}

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDialogItemImpl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !m_pRoot->m_pTrackItem )
	{
		if( (m_nDlgStyles & DLG_STYLE_RESIZABLE) && m_bGripperOn )
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			PostMessage(WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, lParam);
		}
		else if( m_nDlgStyles & DLG_STYLE_DRAGABLE )
			PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
	}
	bHandled = FALSE;
	return S_OK;
}

LRESULT CDialogItemImpl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ctl_OnTransparentTimer();
	return 0;
}

void CDialogItemImpl::OnFinalMessage(HWND)
{
	if( m_pParent )
	{
		m_pParent->RemoveItem(this);
		m_nState |= ISTATE_CLOSING;
		m_pRoot->PostAction(ITEM_ACTION_DESTROY, this);
	}
}

//////////////////////////////////////////////////////////////////////

bool CDialogItem::_GetWindowPlacement(HWND hWnd, RECT& rcNormal, eShow& showCmd)
{
	WINDOWPLACEMENT wp; memset(&wp, 0, sizeof(wp));
	wp.length = sizeof(wp);
	if( !::GetWindowPlacement(hWnd, &wp) )
		return false;

	rcNormal = wp.rcNormalPosition;

	if( GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD )
	{
		POINT pt = {0, 0};
		ClientToScreen(hWnd, &pt);

		rcNormal.left += pt.x;
		rcNormal.right += pt.x;
		rcNormal.top += pt.y;
		rcNormal.bottom += pt.y;
	}
	
	showCmd = showNormal;
	if( wp.showCmd == SW_MAXIMIZE )
		showCmd = showMaximazed;
	return true;
}

bool CDialogItem::_GetGetWorkArea(RECT& rc)
{
	return !!::SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
}

void CDialogItem::_EnableSysClose(HWND hWnd, bool bEnable)
{
	HMENU hSysMenu = ::GetSystemMenu(hWnd, FALSE);
	if( !hSysMenu )
		return;

/**/
	MENUITEMINFO mi; ZeroMemory(&mi, sizeof(MENUITEMINFO)); mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE;
	if( !bEnable )
		mi.fState |= MFS_DISABLED;
	::SetMenuItemInfo(hSysMenu, SC_CLOSE, FALSE, &mi);
	::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_DRAWFRAME|SWP_FRAMECHANGED);
/*/

	::RemoveMenu(hSysMenu, SC_CLOSE, MF_BYCOMMAND);
	CPopupMenu::CleanSeparators(hSysMenu);
/**/
}

tDWORD CDialogItem::_GetSameWindowOffset()
{
	return ::GetSystemMetrics(SM_CYCAPTION);
}

//////////////////////////////////////////////////////////////////////
