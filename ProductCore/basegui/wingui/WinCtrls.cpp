// WinCtrls.cpp: implementation of the CWinCtrls class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winroot.h"
#include "vlistctrl.h"
#include "shellctrls.h"
#include "ChartCtrl.h"
#include "..\Graphics.h"

int DRAW_CHKRAD_SIZE = 13;

void SetNearestFocusItem(CItemBase * pItem)
{
	if( !pItem || !pItem->m_pParent )
		return;
	
	CItemBase * pFocus = NULL;
	CItemBase * pNext = pItem->m_pParent->GetNextChildTabstop(pItem, ENUMITEMS_TAB_ACTIVE);
	CItemBase * pPrev = pItem->m_pParent->GetNextChildTabstop(pItem, ENUMITEMS_TAB_ACTIVE|ENUMITEMS_BACKWARD);
	
	if( !pNext )
		pFocus = pPrev;
	else if( !pPrev )
		pFocus = pNext;
	else if( pNext == pPrev )
		pFocus = pNext;
	else
	{
		for(CItemBase * pParent = pItem->m_pParent; pParent; pParent = pParent->m_pParent)
		{
			if( pNext->IsParent(pParent) )
			{
				pFocus = pNext;
				break;
			}
			
			if( pPrev->IsParent(pParent) )
			{
				pFocus = pPrev;
				break;
			}
		}
	}
	
	if( pFocus && pFocus->GetWindow() )
		::SetFocus(pFocus->GetWindow());
	else
		::SetFocus(::GetNextDlgTabItem(::GetParent(pItem->GetWindow()), pItem->GetWindow(), FALSE));
}

bool IsLeftMouseDownReal()
{
	return true; // Убрана проверка от автокликеров в силу глюкавости данного механизма
/*
	return !!(::GetAsyncKeyState(::GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) & 0x8000);
*/
}

bool IsKeybBtnDownReal(bool bRadio)
{
	return true; // Убрана проверка от автокликеров в силу глюкавости данного механизма

/*
	bool bArrowsPressed = false;
	if( bRadio )
		bArrowsPressed = (::GetAsyncKeyState(VK_DOWN) & 0x8001) || (::GetAsyncKeyState(VK_UP) & 0x8001);
	
	bool bSpacePressed    = !!(::GetAsyncKeyState(VK_SPACE) & 0x0001);
	bool bReturnPressed   = !!(::GetAsyncKeyState(VK_RETURN) & 0x0001);
	
	bool bMnemonicPressed = !!(::GetAsyncKeyState(VK_MENU) & 0x8001);
	if( !bSpacePressed && !bReturnPressed && !bArrowsPressed && !bMnemonicPressed )
	{
		for(char vk_char = 'A'; vk_char <= 'Z'; vk_char++)
		{
			if( ::GetAsyncKeyState(vk_char) & 0x8001 )
			{
				bMnemonicPressed = true;
				break;
			}
		}

		if (!bMnemonicPressed)
		{
			// Bug 13270 fix: Дополнительные коды для русских хоткеев
			static const unsigned char aOemVk[] = { VK_OEM_1, VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_4, VK_OEM_6, VK_OEM_7 };
			for(int i=0; i < sizeof(aOemVk); ++i)
			{
				if( ::GetAsyncKeyState(aOemVk[i]) & 0x8001 )
				{
					bMnemonicPressed = true;
					break;
				}
			}
		}
	}
	
	return bSpacePressed || bReturnPressed || bMnemonicPressed || bArrowsPressed;
*/
}

//////////////////////////////////////////////////////////////////////
// CDialogControl

LRESULT CDialogControl::OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

//////////////////////////////////////////////////////////////////////
// CItemWindowRtn

CItemWindowRtn::CItemWindowRtn()
{
	m_bTrackMouseEvent = false;
	m_nStyles = WS_CHILD|WS_TABSTOP;
	m_nStylesEx = 0;
}

HBRUSH CItemWindowRtn::GetParentBackgroundBrush()
{
	CItemBase * pParent = NULL;
	if( _item()->m_pParent )
		pParent = _item()->m_pParent->GetItemWithBackground();
	if( !pParent || !pParent->m_pBackground )
		return NULL;
	HBRUSH hBrush = pParent->m_pBackground->BrushHnd();
	return hBrush ? hBrush : (HBRUSH)::GetStockObject(WHITE_BRUSH);
}

COLORREF CItemWindowRtn::GetParentBackgroundColor()
{
	CItemBase * pParent = _item()->m_pParent;
	if( pParent )
	{
		CItemBase * pBgItem = pParent->GetItemWithBackground();
		if( pBgItem && pBgItem->m_pBackground )
			return pBgItem->m_pBackground->BGColor();
	}
	
	return ::GetSysColor(COLOR_3DFACE);
}

HWND CItemWindowRtn::GetParentWindow()
{
	HWND hParent = NULL;
	if( _item()->m_pSink )
		hParent = _item()->m_pSink->OnGetParent();
	if( !hParent && _item()->m_pOwner )
		hParent = _item()->m_pOwner->GetWindow();
	return hParent;
}

void CItemWindowRtn::UpdateWindowProps(int flags, tDWORD nStateMask)
{
	CItemBase * pItem = _item();
	HWND hWnd = _hwnd();
	
	if( (flags & UPDATE_FLAG_TEXT) && (pItem->m_nState & ISTATE_CHANGED) )
	{
		wrtn_OnUpdateTextProp();
		pItem->m_nState &= ~ISTATE_CHANGED;
	}
	
	if( (flags & UPDATE_FLAG_FONT) && pItem->m_pFont )
		::SendMessage(_hwnd(), WM_SETFONT, (WPARAM)pItem->m_pFont->Handle(), MAKELPARAM(TRUE, 0));

	if( (flags & UPDATE_FLAG_ICON) && pItem->m_pIcon )
		::SendMessage(_hwnd(), WM_SETICON, FALSE, (LPARAM)pItem->m_pIcon->IconHnd());

	if( flags & (UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE) )
	{
		RECT rcFrame; pItem->GetParentRect(rcFrame);
		
		UINT nFlags = SWP_NOZORDER|SWP_NOACTIVATE;
		if( pItem->m_lockUpdate )
			nFlags |= SWP_NOREDRAW;
		if( !(flags & UPDATE_FLAG_POSITION) )
			nFlags |= SWP_NOMOVE;
		if( !(flags & UPDATE_FLAG_SIZE) )
			nFlags |= SWP_NOSIZE;
		
		wrtn_OnAdjustBordSize(rcFrame);
		::SetWindowPos(_hwnd(), NULL, rcFrame.left, rcFrame.top, RECT_CX(rcFrame), RECT_CY(rcFrame), nFlags);
	}

	if( nStateMask & ISTATE_DISABLED )
	{
		bool bOn = !(pItem->m_nState & ISTATE_DISABLED);
		if( !bOn && (::GetFocus() == hWnd) )
			SetNearestFocusItem(pItem);
		::EnableWindow(hWnd, !!bOn);
	}

	if( nStateMask & ISTATE_HIDE )
	{
		::ShowWindow(hWnd, !(pItem->m_nState & ISTATE_HIDE) ? SW_SHOWNOACTIVATE : SW_HIDE);
		if( pItem->m_lockUpdate && pItem->m_pOwner )
			pItem->m_pOwner->m_nState |= ISTATE_NEED_UPDATE;
	}
}

void CItemWindowRtn::DrawParentContent(HDC dc, RECT *rc)
{
	CItemBase * pOwner = _item()->GetOwner(true);
	if( !pOwner )
		return;

	CItemBase * pDrawExcl = _item()->m_pRoot->m_pDrawExcl;
	_item()->m_pRoot->m_pDrawExcl = _item();
	RECT rcItem; _item()->GetParentRect(rcItem, pOwner);

	RECT rcClip = {0, 0, RECT_CX(rcItem), RECT_CY(rcItem)};
	if( rc )
		rcClip = *rc;

	HRGN hRgn = ::CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
	::SelectClipRgn(dc, hRgn);
	::DeleteObject(hRgn);

	POINT prOrg; ::SetViewportOrgEx(dc, - rcItem.left, - rcItem.top, &prOrg);
	
	pOwner->Draw(dc, &rcItem);
	
	::SetViewportOrgEx(dc, prOrg.x, prOrg.y, NULL);

	::SelectClipRgn(dc, NULL);
	_item()->m_pRoot->m_pDrawExcl = pDrawExcl;
}

//////////////////////////////////////////////////////////////////////

bool CItemWindowRtn::_MsgMenu(POINT& ptMenu)
{
	if( ptMenu.x == -1 && ptMenu.y == -1 )
	{
		if( CItemBase * pFocus = _item()->GetByFlagsAndState(STYLE_ALL, ISTATE_FOCUSED) )
			pFocus->LoadItemMenu(NULL, LOAD_MENU_FLAG_SHOW|LOAD_MENU_FLAG_DESTROY|LOAD_MENU_FLAG_BOTTOMOFITEM);
		return true;
	}

	CItemBase * pItem = _item()->m_pRoot->m_pTrackItem;
	if( !pItem )
	{
		POINT pt = ptMenu;
		ScreenToClient(_item()->GetWindow(), &pt);
		pItem = _item()->HitTest(pt, _item(), false);
	}
	if( !pItem )
		pItem = _item();
	
	if( pItem->m_nState & ISTATE_DISABLED )
		return false;

	CPopupMenuItem * pMenu = pItem->LoadItemMenu(&ptMenu, LOAD_MENU_FLAG_SHOW);
	if( !pMenu )
		return false;
	
	bool bLoaded = !!pMenu->GetChildrenCount();
	pMenu->Destroy();
	return bLoaded;
}

void CItemWindowRtn::_MsgMouseMove(POINT& pt)
{
	if( !m_bTrackMouseEvent )
	{
		m_bTrackMouseEvent = true;
		StartTrackMouseLeave();
	}
		
	CItemBase * pHitItem = _item()->HitTest(pt, _item(), true);
	_item()->m_pRoot->SetTrackItem(pHitItem);
}

void CItemWindowRtn::_MsgPaint()
{
	PAINTSTRUCT ps;
	HDC dc = ::BeginPaint(_hwnd(), &ps);
	RECT rcUpdate = ps.rcPaint;
	
	CItemBase * pItem = _item();

	if( !pItem->m_lockUpdate )
	{
		bool bClip = pItem->Clip(dc, &rcUpdate);
		pItem->Draw(dc, &rcUpdate);
		pItem->m_pRoot->ClipRect(dc, NULL);
	}
	else
		pItem->m_nState |= ISTATE_NEED_UPDATE;
	
	::EndPaint(_hwnd(), &ps);
}

void CItemWindowRtn::StartTrackMouseLeave()
{
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = _hwnd();
	_TrackMouseEvent(&tme);
}

//////////////////////////////////////////////////////////////////////
// CLockButton

LRESULT CLockButton::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DRAWITEMSTRUCT * pDi = (DRAWITEMSTRUCT *)lParam;

	// Draw the button frame.
	UINT uStyle = DFCS_BUTTONPUSH;
	if(pDi->itemState & ODS_SELECTED || m_bLocked || m_bMandatoried)
		uStyle |= DFCS_PUSHED;
	::DrawFrameControl(pDi->hDC, &pDi->rcItem, DFC_BUTTON, uStyle);

	// Draw the lock image
	if(m_pBackground)
	{
		RECT rc = pDi->rcItem;
		rc.top += 2;
		rc.bottom -= 2;
		rc.left += 2;
		rc.right -= 2;
		m_pBackground->Draw(pDi->hDC, rc, NULL, m_pBackground.GetImage(m_bLocked ? 3 : (m_bMandatoried ? 1 : 0)));
	}
	
	// Draw the focus rect
	if(pDi->itemState & ODS_FOCUS)
	{
		RECT rc = pDi->rcItem;
		rc.top += 2;
		rc.bottom -= 2;
		rc.left += 2;
		rc.right -= 2;
		::DrawFocusRect(pDi->hDC, &rc);
	}

	return bHandled = FALSE, 0;
}

bool CLockButton::OnClicked(CItemBase *pItem)
{
	if( !m_bLocked )
	{
		SetMandatory(!m_bMandatoried);
		OnChangedData(this);

		return TBase::OnClicked(pItem);
	}
	return false;
}

bool CLockButton::SetValue(const cVariant &pValue)
{
	tDWORD nValue = pValue.ToDWORD();
	SetMandatory(!!(nValue & 1));
	SetLock(!!(nValue & 2));
	return true;
}

bool CLockButton::GetValue(cVariant &pValue)
{
	pValue = m_bMandatoried;
	return true;
}

void CLockButton::SetMandatory(bool bMandatory)
{
	if(m_bMandatoried != bMandatory)
	{
		m_bMandatoried = bMandatory;
		Invalidate();
	}
}

void CLockButton::SetLock(bool bLock)
{
	if(m_bLocked != bLock)
	{
		m_bLocked = bLock;
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////
// CChkRadLink

CChkRadLink::CChkRadLink()
{
	m_nStyles |= WS_TABSTOP;
	m_nFlags |= STYLE_DRAW_CONTENT;
	m_nFlags &= ~(TEXT_SINGLELINE|STYLE_DRAW_BACKGROUND|STYLE_TRANSPARENT);
}

void CChkRadLink::DrawButton(HDC dc, RECT& rcDraw)
{
	CItemBase::DrawItem(dc, rcDraw);
	CItemBase::DrawChildren(dc, &rcDraw);
//	if( m_nState & ISTATE_FOCUSED )
//		DrawFocusRect(dc, &rcDraw);
}

//////////////////////////////////////////////////////////////////////
// CChkRadBase

LRESULT CChkRadBase::OnUpdateUIState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( uMsg != WM_UPDATEUISTATE )
		DefWindowProc();
	
	RECT rcDraw = {0, 0, m_szSize.cx, m_szSize.cy};
	HDC dc = ::GetWindowDC(m_hWnd);
	Draw(dc, &rcDraw);
	::ReleaseDC(m_hWnd, dc);
	return 0;
}

LRESULT CChkRadBase::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TBase::OnSetFocus(uMsg, wParam, lParam, bHandled);
	return bHandled = TRUE, 0;
}

bool CChkRadBase::Draw(HDC dc, RECT* rcUpdate)
{
	if( !TBase::Draw(dc, rcUpdate) )
		return false;

	if( m_nState & ISTATE_FOCUSED )
	{
		RECT rcFocus; GetClientRectEx(rcFocus, false);
		InflateRect(&rcFocus, 1, 0);
		::DrawFocusRect(dc, &rcFocus);
	}

	return true;
}

void CChkRadBase::DrawItem(HDC dc, RECT& rcFrame)
{
	TBase::DrawItem(dc, rcFrame);
	
	for(size_t i = 0; i < m_aRefs.size(); i++)
	{
		TRef * pRef = (TRef *)m_aRefs[i];
		if( pRef->m_hWnd )
			pRef->Invalidate();
	}
}

bool CChkRadBase::DrawParentBackground(HDC dc, RECT * rcDraw)
{
//	return DrawParentContent(dc), true;
	return DrawParentBackgroundT(dc, rcDraw);
}

void CChkRadBase::GetAddSize(SIZE& szAdd)
{
	szAdd.cx += DRAW_CHKRAD_SIZE + 4 + 2;
	if( szAdd.cy < DRAW_CHKRAD_SIZE )
		szAdd.cy = DRAW_CHKRAD_SIZE;
}

bool CChkRadBase::OnClicked(CItemBase* pItem)
{
	if( pItem == this )
		OnChangedData(this);

	return TBase::OnClicked(pItem);
}

void CChkRadBase::OnAdjustClientRect(RECT& rcClient)
{
	rcClient.left += DRAW_CHKRAD_SIZE + 4;
	InflateRect(&rcClient, -1, 0);
}

void CChkRadBase::OnFocus(bool bFocusted)
{
	TBase::OnFocus(bFocusted);
	if( bFocusted )
		Update(NULL);
}

void CChkRadBase::OnApplyBlockStyles()
{
	if( !m_pLinkFont )
	{
		CFontStyle::INFO pFontInfo; m_pFont->GetInfo(pFontInfo);
		m_pLinkFont = m_pRoot->CreateFont();
		m_pLinkFont->Init(m_pRoot, m_pFont->GetName().c_str(), pFontInfo.m_nHeight, RGB(0, 0, 0xFF), pFontInfo.m_nBold, pFontInfo.m_bItalic, true, pFontInfo.m_nCharExtra);
	}
	
	if( m_aRefs.size() )
	{
		RECT rcCli; GetClientRectEx(rcCli, false);
		HDC dc = ::GetWindowDC(GetWindow());
		HFONT hOldFont = (HFONT)::SelectObject(dc, (HGDIOBJ)m_pFont->Handle());
		
		cStrObj strW;
		LPCSTR strDispText = GetDisplayText();
		for(size_t i = 0; i < m_aRefs.size(); i++)
		{
			CRefsItem::RefData * pRef = m_aRefs[i];
			CItemBase * pItem = pRef->_GetItem();
			if( !pItem->m_pFont || pItem->m_pFont == m_pRoot->GetFont(RES_DEF_FONT) )
				pItem->m_pFont = m_pLinkFont;
						
			RECT rcRef;
			
			{
				RECT rcText = {0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE};
				if( pRef->m_chrRange.x )
				{
					m_pRoot->LocalizeStr(strW, strDispText, pRef->m_chrRange.x);
					::DrawTextW(dc, strW.data(), strW.size(), &rcText, DT_SINGLELINE|DT_CALCRECT);
				}
				else
				{
					::DrawTextW(dc, L"W", 1, &rcText, DT_SINGLELINE|DT_CALCRECT);
					rcText.right = rcText.left;
				}
				
				rcRef.left = RECT_CX(rcText);
				rcRef.top = rcCli.top;
				rcRef.bottom = RECT_CY(rcText);
				rcRef.right = rcRef.left;
			}
			
			if( pRef->m_chrRange.y > pRef->m_chrRange.x )
			{
				RECT rcText = {0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE};
				m_pRoot->LocalizeStr(strW, strDispText + pRef->m_chrRange.x, pRef->m_chrRange.y - pRef->m_chrRange.x);
				::DrawTextW(dc, strW.data(), strW.size(), &rcText, DT_SINGLELINE|DT_CALCRECT);
				rcRef.right += RECT_CX(rcText);
			}
			
			pItem->m_szSize.cx = RECT_CX(rcRef);
			pItem->m_szSize.cy = RECT_CY(rcRef);
			pItem->m_ptOffset.x = rcRef.left;
			pItem->m_ptOffset.y = rcRef.top;
			::SetWindowPos(pItem->GetWindow(), NULL, rcCli.left + rcRef.left, rcCli.top + rcRef.top, RECT_CX(rcRef), RECT_CY(rcRef), SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
		}
		
		::SelectObject(dc, hOldFont);
		::ReleaseDC(GetWindow(), dc);
	}
}

//////////////////////////////////////////////////////////////////////
// CCheck

CCheck::CCheck()
{
	m_nStyles |= BS_AUTOCHECKBOX;
}

void CCheck::DrawContent(HDC dc, RECT& rcClient)
{
	CChkRadBase::DrawContent(dc, rcClient);
	
    CWinRoot* const pWinRoot = static_cast<CWinRoot*>(m_pRoot);
	RECT rcCheck = {0, 0, DRAW_CHKRAD_SIZE, DRAW_CHKRAD_SIZE};
	pWinRoot->DrawCheckBox(dc, rcCheck,
		SendMessage(BM_GETCHECK) == BST_CHECKED,
		m_nState|((SendMessage(BM_GETSTATE) & BST_PUSHED) ? ISTATE_SELECTED : 0), !GUI_ISSTYLE_SIMPLE(m_pRoot));
}

void CCheck::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);
	if( strProps.Get(STR_PID_TRISTATE).GetBool() )
		ModifyStyle(BS_AUTOCHECKBOX, BS_AUTO3STATE, 0);

	SetInt(_ttol(strProps.Get(STR_PID_VALUE).GetStr()));
}

bool CCheck::SetValue(const cVariant &pValue)
{
	SendMessage(BM_SETCHECK, pValue ? BST_CHECKED : BST_UNCHECKED);
	return true;
}

bool CCheck::GetValue(cVariant &pValue)
{
	pValue = SendMessage(BM_GETCHECK) == BST_CHECKED;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CRadio

CRadio::CRadio()
{
	m_nStyles |= BS_RADIOBUTTON;
	m_nFlags |= STYLE_ENUM_VALUE;
}

void CRadio::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	CChkRadBase::InitProps(strProps, pCtx);
	
	if( m_pParent )
		m_pParent->m_nFlags |= STYLE_ENUM;
}

void CRadio::DrawContent(HDC dc, RECT& rcClient)
{
	CChkRadBase::DrawContent(dc, rcClient);

    CWinRoot* pWinRoot = static_cast<CWinRoot*>(m_pRoot);
	RECT rcCheck = {0, 0, DRAW_CHKRAD_SIZE, DRAW_CHKRAD_SIZE};
	pWinRoot->DrawRadioBox(dc, rcCheck,
		SendMessage(BM_GETCHECK) == BST_CHECKED,
		(m_nState & ~ISTATE_SELECTED)|((SendMessage(BM_GETSTATE) & BST_PUSHED) ? ISTATE_SELECTED : 0), !GUI_ISSTYLE_SIMPLE(m_pRoot));
}

void CRadio::OnInit()
{
	TBase::OnInit();
	if( !IsInited() && m_pParent && (m_pParent->GetItem(NULL, GUI_ITEMT_RADIO) == this) )
		ModifyStyle(0, WS_GROUP);
}

void CRadio::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	CChkRadBase::OnUpdateProps(flags, nStateMask, rc);

	if( nStateMask & ISTATE_SELECTED )
		SendMessage(BM_SETCHECK, (m_nState & ISTATE_SELECTED) ? BST_CHECKED : BST_UNCHECKED);
}

//////////////////////////////////////////////////////////////////////
// CGroupBox

CGroupBox::CGroupBox()
{
	m_nStyles = WS_CHILD|BS_GROUPBOX;
	m_nFlags |= STYLE_NON_OWNER;
}

void CGroupBox::InitItem(LPCSTR strType)
{
	if( m_nFlags & STYLE_CLICKABLE )
		m_nStyles |= WS_TABSTOP;
	TBase::InitItem(strType);
}

void CGroupBox::DrawItem(HDC dc, RECT& rcFrame)
{
	RECT rcText = {rcFrame.left + 10, rcFrame.top, rcFrame.right, rcFrame.bottom};

	tDWORD nTextFlags = GetFontFlags();
	m_pFont->Draw(dc, OnGetDisplayText(), rcText, nTextFlags|TEXT_CALCSIZE);
	m_pFont->Draw(dc, OnGetDisplayText(), rcText, nTextFlags);

	rcText.left -= 3, rcText.right += 2;
	m_pRoot->ClipRect(dc, &rcText);

	POINT pt;
	::SetWindowOrgEx(dc, -rcFrame.left, -rcFrame.top, &pt);
	DefWindowProc(WM_PAINT, (WPARAM)dc, NULL);
	::SetWindowOrgEx(dc, pt.x, pt.y, NULL);

	CItemBase::DrawItem(dc, rcFrame);
}

LRESULT CGroupBox::OnBkColor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	::SetBkMode((HDC)wParam, TRANSPARENT);
	if( m_pFont )
		::SetTextColor((HDC)wParam, m_pFont->GetColor());
	return (LRESULT)GetStockObject(NULL_BRUSH);
}

LRESULT CGroupBox::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( HIWORD(wParam) == BN_CLICKED && (m_nFlags & STYLE_CLICKABLE) &&
		::IsWindowEnabled(m_hWnd) &&
		::IsWindowVisible(m_hWnd) )
		Click();

	return bHandled = FALSE, 0;
}

LRESULT CGroupBox::OnUpdateUIState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc();

	for(CItemBase * pOverlapped = NULL; pOverlapped = GetByFlagsAndState(STYLE_NOPARENTDISABLE, ISTATE_ALL, pOverlapped, gbfasNoRecurse); )
		pOverlapped->Update();
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CCombo

CCombo::CCombo() : m_nDropLines(10)
{
	m_nStyles = WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL;
}

LRESULT CCombo::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( HIWORD(wParam) == CBN_SELCHANGE )
		_SelChanged(ctl_GetCurSel());
	
	return bHandled = FALSE, 0;
}

void CCombo::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags & ~(UPDATE_FLAG_SIZE), nStateMask, rc);

	if( flags & UPDATE_FLAG_SIZE )
	{
		RECT rcFrame; GetParentRect(rcFrame);
			
		CFontStyle::INFO info; m_pFont->GetInfo(info);
		rcFrame.bottom += info.m_nHeight * m_nDropLines;

		SetWindowPos(NULL, &rcFrame, SWP_NOZORDER|SWP_NOMOVE);
	}
}

void CCombo::InitItem(LPCSTR strType)
{
	if( m_edit )
	{
		m_nStyles &= ~CBS_DROPDOWNLIST;
		m_nStyles |= CBS_DROPDOWN;
	}
	
	if( m_sorted )
		m_nStyles |= CBS_SORT;

	TBase::InitItem(strType);
}

void CCombo::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	CItemPropVals &prop = strProps.Get(STR_PID_DROPLINES);
	if( prop.IsValid() )
		m_nDropLines = (tDWORD)prop.GetLong();
}

tSIZE_T CCombo::ctl_AddItem(LPCWSTR strName)
{
	return (tSIZE_T)::SendMessageW(m_hWnd, CB_ADDSTRING, 0, (LPARAM)strName);
}

tPTR CCombo::ctl_GetItemData(tSIZE_T nIdx)
{
	return (tPTR)::SendMessageW(m_hWnd, CB_GETITEMDATA, (WPARAM)nIdx, (LPARAM)NULL);
}

void CCombo::ctl_SetItemData(tSIZE_T nIdx, tPTR pData)
{
	::SendMessageW(m_hWnd, CB_SETITEMDATA, (WPARAM)nIdx, (LPARAM)pData);
}

tSIZE_T CCombo::ctl_FindByName(LPCWSTR strName)
{
	if( !CWindowL::m_bAnsi )
		return (tDWORD)::SendMessageW(m_hWnd, CB_FINDSTRING, (WPARAM)-1, (LPARAM)strName);

	_USES_CONVERSION(((CWinRoot *)m_pRoot)->m_nCodePage);
	return ::SendMessageA(m_hWnd, CB_FINDSTRING, (WPARAM)-1, (LPARAM)W2A(strName));
}

void CCombo::ctl_GetTextByPos(tSIZE_T nIdx, cStrObj& strText)
{
	LRESULT nSize = SendMessage(CB_GETLBTEXTLEN, nIdx);
	if( nSize == -1 )
	{
		strText = L"";
		return;
	}

	if( !CWindowL::m_bAnsi )
	{
		strText.resize((tDWORD)nSize);
		if( strText.size() )
			::SendMessageW(m_hWnd, CB_GETLBTEXT, (WPARAM)nIdx, (LPARAM)strText.data());
	}
	else
	{
		tString strTmp;
		strTmp.resize(nSize);
		if( strTmp.size() )
			::SendMessageA(m_hWnd, CB_GETLBTEXT, (WPARAM)nIdx, (LPARAM)strTmp.c_str());
		
		strText = strTmp.c_str();
	}
}

//////////////////////////////////////////////////////////////////////
// CSlider

#define SLIDER_TICS_MINSIZE 6

CSlider::CSlider()
{
	m_nStyles = WS_CHILD|WS_TABSTOP|TBS_AUTOTICKS;
}

void CSlider::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_VERTICAL) m_nStyles |= TBS_VERT|TBS_BOTH; sbreak;
	send;
}

void CSlider::ctl_SetRange(tSIZE_T nMin, tSIZE_T nMax)
{
	SendMessage(TBM_SETRANGEMIN, FALSE, nMin);
	SendMessage(TBM_SETRANGEMAX, FALSE, nMax);
}

void CSlider::ctl_SetPos(tSIZE_T nPos) { ::SendMessage(m_hWnd, TBM_SETPOS, TRUE, nPos); }
tSIZE_T CSlider::ctl_GetPos() { return ::SendMessage(m_hWnd, TBM_GETPOS, 0, 0l); }
bool CSlider::ctl_IsVert() { return !!(GetStyle() & TBS_VERT); }

void CSlider::OnInited()
{
	TBase::OnInited();

	LRESULT l_nThics = SendMessage(TBM_GETRANGEMAX) - SendMessage(TBM_GETRANGEMIN) + 1;
	int l_size;
	{
		RECT rcChannel; SendMessage(TBM_GETCHANNELRECT, 0, (LPARAM)&rcChannel);
		l_size = RECT_CX(rcChannel);
	}

	if( l_size && ( (l_size < l_nThics) || (l_size - l_nThics) < (l_nThics - 1)*SLIDER_TICS_MINSIZE ) )
		SendMessage(TBM_SETTICFREQ, ::MulDiv((int)l_nThics, SLIDER_TICS_MINSIZE + 1, l_size + SLIDER_TICS_MINSIZE));
}

//////////////////////////////////////////////////////////////////////
// CXPButton

void CXPButton::OnInit()
{
	TBase::OnInit();

	COLORREF clFace = RGB(183,219,209);
	if( m_pBackground )
		clFace = m_pBackground->BGColor();

	CXPBrush::Init(clFace, GetParentBackgroundColor());
}

void CXPButton::DrawButton(HDC dc, RECT& rcDraw)
{
	RECT rcFrame = rcDraw;
	int nFontFlags = ALIGN_VCENTER|ALIGN_HCENTER|TEXT_SINGLELINE;

	if( m_nState & ISTATE_DISABLED )
	{
		FrameRect(dc, &rcFrame, CBrushHnd(CreateSolidBrush(m_clBg)));
		InflateRect(&rcFrame, -1, -1);
		FrameRect(dc, &rcFrame, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
		DrawGrayedConers(dc, rcDraw);
		InflateRect(&rcFrame, -1, -1);
		DrawGradint(dc, rcFrame, m_nState);

		nFontFlags |= TEXT_GRAYED;
	}
	else
	{
		InflateRect(&rcFrame, -1, -1);
		FrameRect(dc, &rcFrame, m_hFrame);

		InflateRect(&rcFrame, -1, -1);
		if( !(m_nState & (ISTATE_HOTLIGHT|ISTATE_DEFAULT|ISTATE_DISABLED)) )
			FrameRect(dc, &rcFrame, (HBRUSH)GetStockObject(WHITE_BRUSH));

		InflateRect(&rcFrame, -1, -1);
		DrawGradint(dc, rcFrame, m_nState, true); 

		DrawTopBorder(dc, rcDraw);
		DrawLeftBorder(dc, rcDraw);
		DrawBottomBorder(dc, rcDraw);
		DrawRightBorder(dc, rcDraw);

		if( m_nState & (ISTATE_HOTLIGHT|ISTATE_DEFAULT) )
		{
			tDWORD nState = m_nState; // For HT1, HT2, ...
			CAlphaBrush(HT1).Draw(dc, rcFrame.left, rcFrame.top-1, rcFrame.right, rcFrame.top);
			CAlphaBrush(HT2).Draw(dc, rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.top+1);
			CAlphaBrush(HT3).Draw(dc, rcFrame.left-1, rcFrame.bottom-1, rcFrame.right+1, rcFrame.bottom);
			CAlphaBrush(HT4).Draw(dc, rcFrame.left, rcFrame.bottom, rcFrame.right, rcFrame.bottom+1);
		}
	}

	if( m_pFont )
		m_pFont->Draw(dc, m_strText.c_str(), rcDraw, nFontFlags);

	if( m_nState & ISTATE_FOCUSED )
		DrawFocusRect(dc, &rcFrame); 
}

//////////////////////////////////////////////////////////////////////
// CHotlink

void CHotlink::OnInit()
{
	TBase::OnInit();
}

void CHotlink::DrawItem(HDC dc, RECT& rcFrame)
{
	TBase::DrawItem(dc, rcFrame);
	
/*	if( (m_nState & ISTATE_FOCUSED) && !m_aItems.size() )
	{
		RECT rcFocus = rcFrame;
		::InflateRect(&rcFocus, SINGLELINE_MARGSIZE, 0);
		::DrawFocusRect(dc, &rcFocus);
	}
*/
}

void CHotlink::OnFocus(bool bFocusted)
{
	TBase::OnFocus(bFocusted);

	for(size_t i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->Focus(bFocusted);
}

void CHotlink::OnAdjustUpdateRect(RECT& rcUpdate)
{
	TBase::OnAdjustUpdateRect(rcUpdate);
//	::InflateRect(&rcUpdate, SINGLELINE_MARGSIZE, 0);
}

//////////////////////////////////////////////////////////////////////
// CTab

void CTab::Cleanup()
{
	if( m_pRoot && !GUI_ISSTYLE_SIMPLE(m_pRoot) && m_pBackground )
		m_pBackground->Destroy();

	TBase::Cleanup();
}

void CTab::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);

	if( !GUI_ISSTYLE_SIMPLE(m_pRoot) )
	{
		m_clBg = m_pBackground ? m_pBackground->BGColor() : RGB(219,219,219);
		m_pBackground = m_pRoot->CreateBackground(RGB(255,255,255));
	}
}

CItemBase * CTab::CreateChild(LPCTSTR strItemId, LPCTSTR strItemType)
{
	if( strItemType == NULL || !*strItemType )
		return new CDialogControl();

	return NULL;
}

void CTab::OnInit()
{
	if( !GUI_ISSTYLE_SIMPLE(m_pRoot) )
		CXPBrush::Init(m_clBg, GetParentBackgroundColor());

	TBase::OnInit();
}

tSIZE_T CTab::ctl_GetCurSel() { return TabCtrl_GetCurSel(m_hWnd); }
void   CTab::ctl_SetCurSel(tSIZE_T nIdx) { TabCtrl_SetCurSel(m_hWnd, nIdx); }
tSIZE_T CTab::ctl_GetItemCount() { return TabCtrl_GetItemCount(m_hWnd); }
void   CTab::ctl_DeleteItem(tSIZE_T nIdx) { TabCtrl_DeleteItem(m_hWnd, nIdx); }
void   CTab::ctl_DeleteAllItems() { TabCtrl_DeleteAllItems(m_hWnd); }
void   CTab::ctl_AdjustRect(RECT& rcClient) { TabCtrl_AdjustRect(m_hWnd, FALSE, &rcClient); }

void CTab::ctl_SetItem(tSIZE_T nIdx, LPCSTR strText, CItemBase* pItem)
{
	cStrObj strW; m_pRoot->LocalizeStr(strW, strText);
	
	TCITEMW l_stItem;
	l_stItem.mask = TCIF_TEXT;
	l_stItem.pszText = (LPWSTR)strW.data();

	tSIZE_T n = TabCtrl_GetItemCount(m_hWnd);
	UINT nMsg =  TCM_SETITEMW;
	if( nIdx >= n )
	{
		nIdx = n;
		nMsg = TCM_INSERTITEMW;
	}
	
	::SendMessageW(m_hWnd, nMsg, (WPARAM)nIdx, (LPARAM)&l_stItem);
}

LRESULT	CTab::OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	NMHDR * phdr = (NMHDR *)lParam;
		
	switch( phdr->code )
	{
	case TCN_SELCHANGING:
	{
		tSIZE_T nCurSel = TabCtrl_GetCurSel(m_hWnd);
		OnSelectItem(_Idx2Page(nCurSel), false);
	} break;
	
	case TCN_SELCHANGE:
	{
		tSIZE_T nCurSel = TabCtrl_GetCurSel(m_hWnd);
		OnSelectItem(_Idx2Page(nCurSel), true);
		m_nCurItem = nCurSel;
	} break;
	}
	
	return( S_OK );
}

LRESULT	CTab::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !GUI_ISSTYLE_SIMPLE(m_pRoot) )
		return TBase::OnEraseBkgnd(uMsg, wParam, lParam, bHandled);
	return 1;
}

LRESULT CTab::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !GUI_ISSTYLE_SIMPLE(m_pRoot) )
		return TBase::OnPaint(uMsg, wParam, lParam, bHandled);

	ValidateRect(NULL);

	RECT rc; GetClientRect(&rc);

	HDC hControlDC = GetDC();
	HDC hMemoryDC = ::CreateCompatibleDC(hControlDC);

	HBITMAP hBmp = ::CreateCompatibleBitmap(hControlDC, rc.right, rc.bottom);
	HGDIOBJ hBmpPrev = ::SelectObject(hMemoryDC, hBmp);

	// Draw into bitmap
	DefWindowProc(WM_ERASEBKGND, (WPARAM)hMemoryDC, 0);
	DefWindowProc(WM_PRINTCLIENT, (WPARAM)hMemoryDC, PRF_CLIENT);

	// Draw bitmap into window
	{
		RECT rcClient; GetClientRectEx(rcClient, true);
		m_pRoot->ClipRect(hControlDC, &rcClient);
		::BitBlt(hControlDC, 0, 0, rc.right, rc.bottom, hMemoryDC, 0, 0, SRCCOPY);
		m_pRoot->ClipRect(hControlDC, NULL);
	}

	::DeleteObject(::SelectObject(hMemoryDC, hBmpPrev));
	::DeleteDC(hMemoryDC);
	ReleaseDC(hControlDC);
	return 0;
}

void CTab::DrawItem(HDC dc, RECT& rcDraw)
{
	if( GUI_ISSTYLE_SIMPLE(m_pRoot) )
	{
		TBase::DrawItem(dc, rcDraw);
		return;
	}
	
	DrawParentBackground(dc, &rcDraw);

	RECT rcArea = rcDraw, rcSelItem;
	int nSelItem = TabCtrl_GetCurSel(m_hWnd);
	if( nSelItem != -1 )
	{
		TabCtrl_GetItemRect(m_hWnd, nSelItem, &rcSelItem);
		rcArea.top += rcSelItem.bottom-1;
	}

	RECT rcFrame = rcArea;
	InflateRect(&rcFrame, -1, -1);
	FillRect(dc, &rcFrame, (HBRUSH)GetStockObject(WHITE_BRUSH));
	FrameRect(dc, &rcFrame, m_hFrame);

	if( !nSelItem )
		/*GetLeftBrush().Draw(dc, rcArea.left, rcDraw.top+1, rcArea.left+1, rcDraw.bottom-1)*/;
	else
		DrawLeftBorder(dc, rcArea);
	DrawTopBorder(dc, rcArea);
	DrawBottomBorder(dc, rcArea);
	DrawRightBorder(dc, rcArea);

	int nCount = TabCtrl_GetItemCount(m_hWnd);
	for(int i = nCount-1; i >= 0; i--)
	{
		if( i == nSelItem )
			continue;

		RECT rcItem;
		TabCtrl_GetItemRect(m_hWnd, i, &rcItem);
		rcItem.bottom++;
		rcItem.right++;
		if( i == nSelItem + 1 )
			rcItem.left--;
		DrawTabItem(dc, rcItem, i,  0);
	}

	if( nSelItem != -1 )
	{
		InflateRect(&rcSelItem, 2, 2);
		rcSelItem.bottom--;
		DrawTabItem(dc, rcSelItem, nSelItem, ISTATE_SELECTED|ISTATE_HOTLIGHT);
	}
}

void CTab::DrawTabItem(HDC dc, RECT& rcItem, int nItem, int nState)
{
	RECT rcFrame = rcItem;
	InflateRect(&rcFrame, -1, -1);   rcFrame.bottom++;
	FrameRect(dc, &rcFrame, m_hFrame);

	if( !(nState & ISTATE_SELECTED) ) rcFrame.bottom--;
	InflateRect(&rcFrame, -1, -1);
	DrawGradint(dc, rcFrame, nState);

	DrawLeftBorder(dc, rcItem);
	DrawTopBorder(dc, rcItem);
	GetRightBrush().Draw(dc, rcItem.right-1, rcItem.top+4, rcItem.right, rcItem.bottom-1);

	InflateRect(&rcFrame, -1, -1);
	if( nState & ISTATE_HOTLIGHT )
	{
		CAlphaBrush(HT4).Draw(dc, rcFrame.left, rcFrame.top-1, rcFrame.right, rcFrame.top);
		CAlphaBrush(HT2).Draw(dc, rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.top+1);
	}

	if( m_pFont )
		m_pFont->Draw(dc, _Idx2Page(nItem)->GetDisplayText(), rcItem, ALIGN_VCENTER|ALIGN_HCENTER|TEXT_SINGLELINE);

/*	if( (m_nState & ISTATE_FOCUSED) && (nState & ISTATE_SELECTED) )
	{
		InflateRect(&rcFrame, -1, -1);
		DrawFocusRect(dc, &rcFrame);
	}
*/
}

//////////////////////////////////////////////////////////////////////
// CIPAddressCtrl

CIPAddressCtrl::CIPAddressCtrl() : 
	m_hFont(NULL)
{
}

CIPAddressCtrl::~CIPAddressCtrl()
{
	if( m_hFont )
		DeleteObject((HGDIOBJ)m_hFont), m_hFont = NULL;
}

void CIPAddressCtrl::InitItem(LPCSTR strType)
{
	GuiImplClass::InitItem(strType);
}

void CIPAddressCtrl::GetAddSize(SIZE& szAdd)
{
	szAdd.cx += ScaledSize(125); szAdd.cy += 8;
}

void CIPAddressCtrl::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);
	RECT rcFrame = {0, 0, ScaledSize(125), ScaledSize(20)};
	WinImplClass::Create(m_pRoot, GetParentWindow(), GetWndClassInfo(), rcFrame, m_nStyles, m_nStylesEx, GetItemId());
}

bool CIPAddressCtrl::SetValue(const cVariant &pValue)
{
	if( pValue )
	{
		tDWORD nIP, nValue = pValue.ToDWORD();
		BYTE *pIP = (BYTE *)&nIP;
		pIP[3] = FIRST_IPADDRESS(nValue);
		pIP[2] = SECOND_IPADDRESS(nValue);
		pIP[1] = THIRD_IPADDRESS(nValue);
		pIP[0] = FOURTH_IPADDRESS(nValue);
		//::SendMessage(m_hWnd, IPM_SETADDRESS, 0, nIP);
		// ip-address edit control has a bug:
		// IPM_SETADDRESS doesn't work if control created w/o WS_VISIBLE flag
		// and has never been visible ;) thanks to MS.
		::PostMessage(m_hWnd, IPM_SETADDRESS, 0, nIP);
	}
	else
		::SendMessage(m_hWnd, IPM_CLEARADDRESS, 0, 0);
	return true;
}

bool CIPAddressCtrl::GetValue(cVariant &pValue)
{
	tDWORD nIP, nValue = 0;
	::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (WPARAM)&nIP);
	BYTE *pIP = (BYTE *)&nValue;
	pIP[3] = FIRST_IPADDRESS(nIP);
	pIP[2] = SECOND_IPADDRESS(nIP);
	pIP[1] = THIRD_IPADDRESS(nIP);
	pIP[0] = FOURTH_IPADDRESS(nIP);
	pValue = nValue;
	return true;
}

//////////////////////////////////////////////////////////////////////

LRESULT CIPAddressCtrl::OnGetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !m_pFont )
		return NULL;
	return (LRESULT)m_pFont->Handle();
}

LRESULT CIPAddressCtrl::OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// ip-address edit control has a bug:
	// then it is destroying it is also destroying the font, given to it by WM_SETFONT
	if( m_hFont )
	{
		DeleteObject((HGDIOBJ)m_hFont);
		m_hFont = NULL;
	}
	if( (HFONT)wParam )
	{
		LOGFONT lf = {0,};
		if( GetObject((HFONT)wParam, sizeof(lf), &lf) )
			m_hFont = CreateFontIndirect(&lf);
	}
	return DefWindowProc(uMsg, (WPARAM)m_hFont, lParam);
}

LRESULT CIPAddressCtrl::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( HIWORD(wParam) == EN_CHANGE )
		_DataChanged();
	return bHandled = FALSE, 0;
}

//////////////////////////////////////////////////////////////////////
// CEditRtn

HWND CEditRtn::Create(CRootItem * pRoot, HWND hWndParent, CWndClassInfo& pCI, RECT& rcPos, DWORD dwStyle, DWORD dwExStyle, UINT nID)
{
	static bool bRegistered = false;

	if( !(m_bAnsi ? RegisterWindowA(&pCI, !bRegistered) : RegisterWindowW(&pCI, !bRegistered)) )
		return NULL;

	bRegistered = true;

	if( pCI.m_lpszOrigName )
		m_pfnSuperWindowProc = pCI.pWndProc;
	else
		m_pfnSuperWindowProc = m_bAnsi ? ::DefWindowProcA : ::DefWindowProcW;

	_Module.AddCreateWndData(&m_thunk.cd, this);
	m_hWnd = CreateWnd(dwExStyle, (LPVOID)pCI.m_wc.lpszClassName, dwStyle, rcPos, hWndParent, nID);
	return m_hWnd;
}

//////////////////////////////////////////////////////////////////////
// CEdit

CEdit::CEdit() :
	m_bShowMenu(0),
	m_bAutoComplete(0),
	m_hSpin(NULL),
	m_bUpdateSpin(0)
{
	m_nStyles = WS_CHILD|WS_TABSTOP|ES_AUTOVSCROLL|ES_AUTOHSCROLL|WS_HSCROLL|WS_VSCROLL;
	m_nStylesEx = WS_EX_CLIENTEDGE;
}

void CEdit::ctl_ReplaceText(LPCWSTR strText, tSIZE_T nFrom, tSIZE_T nTo)
{
	::SendMessageA(m_hWnd, EM_SETSEL, (WPARAM)nFrom, (LPARAM)nTo);
	
	if( !CWindowL::m_bAnsi )
		::SendMessageW(m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)strText);
	else
	{
		_USES_CONVERSION(((CWinRoot *)m_pRoot)->m_nCodePage);
		::SendMessageA(m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)W2A(strText));
	}
}

void CEdit::ctl_ReplaceSelText(LPCWSTR strText)
{
	if( !CWindowL::m_bAnsi )
		::SendMessageW(m_hWnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strText);
	else
	{
		_USES_CONVERSION(((CWinRoot *)m_pRoot)->m_nCodePage);
		::SendMessageA(m_hWnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)W2A(strText));
	}
}

void CEdit::ctl_SetSelText(tSIZE_T nFrom, tSIZE_T nTo, bool bAsync)
{
	if( bAsync )
		::PostMessageA(m_hWnd, EM_SETSEL, nFrom, nTo);
	else
		::SendMessageA(m_hWnd, EM_SETSEL, nFrom, nTo);
}

bool CEdit::ctl_IsMultiline() { return !!(m_nStyles & ES_MULTILINE); }
bool CEdit::ctl_IsNumber() { return !!(m_nStyles & ES_NUMBER); }
bool CEdit::ctl_IsReadonly() { return !!(m_nStyles & ES_READONLY); }

LRESULT CEdit::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( HIWORD(wParam) == EN_CHANGE )
		_DataChanged();
	
	return bHandled = FALSE, 0;
}

LRESULT CEdit::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdateSpin();
	return bHandled = FALSE, 0;
}

LRESULT CEdit::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( m_nCompress )
		OnUpdateTextProp();
	UpdateSpin();
	
	return bHandled = FALSE, 0;
}

LRESULT CEdit::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !(m_nFlags & TEXT_SINGLELINE) || (m_nStylesEx & WS_EX_CLIENTEDGE) )
		return bHandled = FALSE, 0;

	if( wParam )
	{
		NCCALCSIZE_PARAMS * pCs = (NCCALCSIZE_PARAMS *)lParam;
		::InflateRect(&pCs->rgrc[0], 0, -SINGLELINE_MARGSIZE);
	}
	
	return 0;
}

LRESULT CEdit::OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( m_nCompress && GetFocus() != m_hWnd )
		return SetFocus(), 0;
	return bHandled = FALSE, 0;
}

LRESULT CEdit::OnPswdProtectCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !(m_nStyles & ES_PASSWORD) || m_bInternalTextOp )
		bHandled = FALSE;
	return 0;
}

LRESULT CEdit::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !(m_nFlags & TEXT_SINGLELINE) || (m_nStylesEx & WS_EX_CLIENTEDGE) )
		return bHandled = FALSE, 0;

	HDC dc = ::GetWindowDC(m_hWnd);

	RECT rcFill = {0, 0, m_szSize.cx, m_szSize.cy};
	RECT rcNc = rcFill;
	::InflateRect(&rcNc, 0, -SINGLELINE_MARGSIZE);
	::ExcludeClipRect(dc, rcNc.left, rcNc.top, rcNc.right, rcNc.bottom);
	DrawParentBackground(dc, &rcFill);
	
	::ReleaseDC(m_hWnd, dc);
	return 0;
}

LRESULT CEdit::OnChar( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( ctl_IsNumber() && ::GetKeyState(VK_CONTROL) >= 0 && ::GetKeyState(VK_MENU) >= 0
		&& !iscntrl((int)wParam) && !isdigit((int)wParam) )
	{
		tObjPath strTitle, strText;

		m_pRoot->LocalizeStr(strTitle, LoadLocString(tString(), "InvalidSymbol"));
		m_pRoot->LocalizeStr(strText, LoadLocString(tString(), "OnlyNumbersAllowed"));

		EDITBALLOONTIP ebt;
		
		ebt.cbStruct = sizeof(EDITBALLOONTIP);
		ebt.pszTitle = strTitle.data();
		ebt.pszText  = strText.data();
		ebt.ttiIcon  = TTI_ERROR;

		SendMessage(EM_SHOWBALLOONTIP, 0, (LPARAM)&ebt);

		return bHandled = TRUE, 0;
	}

	return bHandled = FALSE, 0;
}

//////////////////////////////////////////////////////////////////////

void CEdit::GetAddSize(SIZE& szAdd)
{
	if( m_nStylesEx & WS_EX_CLIENTEDGE )
		szAdd.cy += 8;
	else
		TBase::GetAddSize(szAdd);
}

void CEdit::Cleanup()
{
	TBase::Cleanup();

	if( m_hSpin )
		::DestroyWindow(m_hSpin), m_hSpin = NULL;
}

void CEdit::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_READONLY)      m_nStyles |= ES_READONLY; m_nStyles &= ~WS_TABSTOP; sbreak;
	scase(STR_PID_PASSWORD)      m_nStyles |= ES_PASSWORD; sbreak;
	scase(STR_PID_PASSWORDSET)   m_nStyles |= ES_PASSWORD; m_bDummyPasswdShow = 1; m_bDummyPasswdSimpleShow = 1; sbreak;
	scase(STR_PID_PASSWORDSETEX) m_nStyles |= ES_PASSWORD; m_bDummyPasswdShow = 1; sbreak;
	scase(STR_PID_NUM)           m_nStyles |= ES_NUMBER; m_nStylesEx |= WS_EX_RIGHT; sbreak;
	scase(STR_PID_MULTILINE)     m_nStyles |= ES_MULTILINE|ES_WANTRETURN; sbreak;
	scase(STR_PID_RALIGN)        m_nStyles |= ES_RIGHT; sbreak;
	scase(STR_PID_NOSCROLL)      m_nStyles &= ~(WS_HSCROLL|WS_VSCROLL); sbreak;
	scase(STR_PID_VSCROLL)       m_nStyles &= ~WS_HSCROLL; m_nStyles |= WS_VSCROLL; sbreak;
	scase(STR_PID_HSCROLL)       m_nStyles &= ~WS_VSCROLL; m_nStyles |= WS_HSCROLL; sbreak;
	scase(STR_PID_AUTOCOMPLETE)  m_bAutoComplete = 1; sbreak;
	send;
}

void CEdit::InitItem(LPCSTR strType)
{
	if( m_nStyles & ES_MULTILINE )
	{
		if( !(m_nStyles & WS_HSCROLL) )
			m_nStyles &= ~ES_AUTOHSCROLL;
		
		m_nFlags &= ~TEXT_SINGLELINE;
	}
	else
		m_nStyles &= ~(ES_AUTOVSCROLL|ES_WANTRETURN|WS_HSCROLL|WS_VSCROLL);
	
	TBase::InitItem(strType);
}

void CEdit::OnInited()
{
	TBase::OnInited();

	// Create Spin
	if( m_nStyles & ES_NUMBER )
	{
		RECT rcCtl; ::GetWindowRect(m_hWnd, &rcCtl);

		m_hSpin = ::CreateWindow(UPDOWN_CLASS, _T(""),
			WS_CHILD|UDS_HOTTRACK|UDS_ALIGNRIGHT|UDS_NOTHOUSANDS|UDS_ARROWKEYS|UDS_SETBUDDYINT,
			0, 0, 0, 0, ::GetParent(m_hWnd), NULL, NULL, NULL);
		UpdateSpin();
		
		if( ::IsWindowVisible(m_hWnd) )
			::ShowWindow(m_hSpin, SW_SHOW);

		RECT rcCtlSpinned; ::GetWindowRect(m_hWnd, &rcCtlSpinned);

		m_szSpin.cx = RECT_CX(rcCtl) - RECT_CX(rcCtlSpinned);
		m_szSpin.cy = RECT_CY(rcCtl) - RECT_CY(rcCtlSpinned);

		SetRange(m_ptLim);
	}
	else if( m_ptLim.x )
		SendMessage(EM_SETLIMITTEXT, m_ptLim.x);

	if( !(m_nStylesEx & WS_EX_CLIENTEDGE) )
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, 0);

	if( m_nCompress )
		SetTipText(m_strText.c_str());

	if(m_bAutoComplete)
	{
		typedef HRESULT (WINAPI *fnSHAutoComplete)(HWND hwndEdit, DWORD dwFlags);
		HMODULE hShlwapi32 = ::LoadLibrary(_T("Shlwapi.dll"));
		if(hShlwapi32)
		{
			fnSHAutoComplete SHAutoComplete = (fnSHAutoComplete)::GetProcAddress(hShlwapi32, "SHAutoComplete");
			if(SHAutoComplete)
				SHAutoComplete(m_hWnd, SHACF_DEFAULT);
			::FreeLibrary(hShlwapi32);
		}
	}
}

void CEdit::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags, nStateMask, rc);

	if( (nStateMask & ISTATE_HIDE) && m_hSpin )
		::ShowWindow(m_hSpin, !(m_nState & ISTATE_HIDE) ? SW_SHOW : SW_HIDE);

	if( (nStateMask & ISTATE_DISABLED) && m_hSpin )
	{
		::EnableWindow(m_hSpin, !(m_nState & ISTATE_DISABLED));
		::InvalidateRect(m_hSpin, NULL, FALSE);
	}
}

void CEdit::UpdateSpin()
{
	if( !m_hSpin || m_bUpdateSpin )
		return;
	
	m_bUpdateSpin = 1;
	::SendMessage(m_hSpin, UDM_SETBUDDY, (WPARAM)m_hWnd, NULL);
	m_bUpdateSpin = 0;
}

void CEdit::SetRange(POINT& ptLim)
{
	::SendMessage(m_hSpin, UDM_SETRANGE32, (WPARAM)ptLim.x, (LPARAM)ptLim.y);
}

//////////////////////////////////////////////////////////////////////
// CTimeEditBox

CTimeEditBox::CTimeEditBox()
{
	m_nStyles = WS_CHILD|WS_TABSTOP|DTS_UPDOWN/*|DTS_TIMEFORMAT*/;
	m_nStylesEx = WS_EX_CLIENTEDGE;
	m_nType = DTT_TIMESHORT;
	m_bSeconds = false;
}

void CTimeEditBox::GetAddSize(SIZE& szAdd)
{
	if( m_nStylesEx & WS_EX_CLIENTEDGE )
		szAdd.cy += 8;
	else
		TBase::GetAddSize(szAdd);
}

void CTimeEditBox::ApplyStyle(LPCSTR strStyle)
{
	if( !strcmp(strStyle, "seconds") )
		m_bSeconds = true;
	if( !strcmp(strStyle, "date") )
		m_nType = DTT_DT;
}

LRESULT CTimeEditBox::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	NMHDR *pnmh = (NMHDR *)lParam;
	if(pnmh->code == DTN_DATETIMECHANGE &&  pnmh->hwndFrom == m_hWnd)
		OnChangedData(this);
	return bHandled = FALSE, 0;
}

void CTimeEditBox::OnInit()
{
	tString strFmt = GetDateTimeFormat((DTT_TYPE)m_nType);
	DateTime_SetFormat(m_hWnd, strFmt.c_str());
	TBase::OnInit();
}

bool CTimeEditBox::SetValue(const cVariant &pValue)
{
	time_t dwSec = pValue.ToDWORD();
	
	SYSTEMTIME st = {0, };
	GetSystemTime(&st);
	st.wHour   = (WORD)((dwSec/(60*60))%24);
	st.wMinute = (WORD)((dwSec/(60))%60);
	st.wSecond = 0/*(dwSec/(1))%60*/;

	bool bChanged = true;
	if( struct tm* tmTime = gmtime(&dwSec) )
		if( tmTime->tm_year != 70 )
		{
			st.wYear = tmTime->tm_year + 1900;
			st.wMonth = tmTime->tm_mon + 1;
			st.wDay = tmTime->tm_mday;
			bChanged = false;
		}

	bool bRet = !!::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, (LPARAM)GDT_VALID, (WPARAM)&st);

	if( bChanged )
		OnChangedData(this);

	return bRet;
}

bool CTimeEditBox::GetValue(cVariant &pValue)
{
	SYSTEMTIME st = {0, };
	if(::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (WPARAM)&st) == GDT_VALID)
	{
		if( m_bSeconds )
		{
			pValue = (tDWORD)((st.wHour * 60 + st.wMinute) * 60);
		}
		else
		{
			struct tm tmTime = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, 0, 0, 0};
			pValue = (tDWORD)(mktime(&tmTime) - _timezone);
		}
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// CRichEditImpl

CRichEditImpl::CRichEditImpl() :
	m_bNoFocus(0),
	m_bLBClick(0),
	m_bStylesApplied(0),
    m_indSelLink(0)
{
	m_nStyles = WS_CHILD|WS_TABSTOP|ES_AUTOVSCROLL|ES_MULTILINE|BS_FLAT/*|ES_NOOLEDRAGDROP*/;
	m_nStylesEx = WS_EX_CLIENTEDGE;
	m_curLink.cpMax = m_curLink.cpMin = 0;
}

void CRichEditImpl::InitItem(LPCTSTR strType)
{
	static CLibHnd g_hRTextLib;
	if( !g_hRTextLib )
	{
		if( !(g_hRTextLib = ::LoadLibrary("RICHED20.DLL")) )
		{
			OutputDebugString("GUI(Win)::Can't load library: RICHED20.DLL\n");
			return;
		}
	}

	if( !(m_nStylesEx & WS_EX_CLIENTEDGE) )
	{
		m_nStylesEx |= WS_EX_TRANSPARENT;
		m_nFlags |= STYLE_TRANSPARENT;
	}
	
	TBase::InitItem(strType);
	::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, (LPARAM)ENM_LINK);

	if( m_nStylesEx & WS_EX_CLIENTEDGE )
	{
		if( m_pRoot->IsThemed() )
			ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_STATICEDGE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, 5);
	}
	else
	{
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, 0);
		SendMessage(EM_SETBKGNDCOLOR, 0, GetParentBackgroundColor());
		SendMessage(EM_SETOPTIONS, ECOOP_OR, ECO_AUTOHSCROLL);
	}
}

void CRichEditImpl::ApplyStyle(LPCTSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	sswitch(strStyle)
	scase(STR_PID_VSCROLL)     m_nStyles |= ES_AUTOVSCROLL|WS_VSCROLL; sbreak;
	scase(STR_PID_HSCROLL)     m_nStyles |= ES_AUTOHSCROLL|WS_HSCROLL; sbreak;
	scase(STR_PID_READONLY)    m_nStyles |= ES_READONLY; sbreak;
	scase(STR_PID_RALIGN)      m_nStyles |= ES_RIGHT; sbreak;
	scase(STR_PID_NOBORDER)    m_nStylesEx &= ~WS_EX_CLIENTEDGE; sbreak;
	scase(STR_PID_NOFOCUS)     m_bNoFocus = 1; sbreak;
	send;
}

LRESULT CRichEditImpl::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
/*
	if( ((CWinRoot *)m_pRoot)->m_nOsVer >= GUI_OSVER_WIN2000 )
	{
		bHandled = FALSE;
		return 0;
	}
*/
	
	// Ричтекст некоторых версий не рюхает установку шрифта без установки текста до показа контрола
	DefWindowProc();
	
	if( (BOOL)wParam )
	{
		WinImplClass::SetFont(m_pFont->Handle());
		WinImplClass::SetWindowTextW(m_pRoot, m_pRoot->LocalizeStr(TOR_cStrObj, OnGetDisplayText()));
		m_bStylesApplied = 0, OnApplyBlockStyles();
	}
	
	return 0;
}

LRESULT	CRichEditImpl::OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	NMHDR * phdr = ( NMHDR * )lParam;
		
//	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "GUI(Win)::RichEdit::OnNotify(0x%08X)", phdr->code));

	switch(phdr->code)
	{
	case EN_LINK:
	{
		ENLINK * pENLink = (ENLINK *)phdr;
//		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "GUI(Win)::RichEdit::EN_LINK(0x%08X)", pENLink->msg));

		m_curLink = pENLink->chrg;

		if( pENLink->msg == WM_LBUTTONUP || pENLink->msg == WM_LBUTTONDBLCLK )
			for(size_t i = 0; i < m_aRefs.size(); i++)
			{
				RefData * pRef = m_aRefs[i];
				if( pRef->m_chrRangeEx.x == pENLink->chrg.cpMin )
				{
					pRef->_GetItem()->Click();
                    m_indSelLink = i;
           	        ApplyRefStyles();
					return 1;
				}
			}
	} break;
	}
	
	return 0;
}

LRESULT CRichEditImpl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch( wParam )
    {
    case VK_ESCAPE:
	    {
		    CItemBase * pOwner = GetOwner(true);
		    ::PostMessage(pOwner->GetWindow(), uMsg, wParam, lParam);
		    return 0;
	    }
        break;

    case VK_LEFT:
    case VK_UP:
        if( m_indSelLink > 0 )
        {
            --m_indSelLink;
           	ApplyRefStyles();
			return 0;
        }
        break;

    case VK_RIGHT:
    case VK_DOWN:
        if( m_indSelLink < (m_aRefs.size() - 1) )
        {
            ++m_indSelLink;
           	ApplyRefStyles();
            return 0;
        }
        break;
    case VK_SPACE:
    case VK_RETURN:
        if( m_indSelLink < m_aRefs.size() )
        {
			m_aRefs[m_indSelLink]->_GetItem()->Click();
            return 0;
        }
    }
	
	return bHandled = FALSE, S_OK;
}

LRESULT CRichEditImpl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !m_bNoFocus )
		bHandled = FALSE;

    TBase::OnFocus(true);
   	ApplyRefStyles();

	return S_OK;
}

LRESULT CRichEditImpl::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    TBase::OnFocus(false);
   	ApplyRefStyles();

	m_bLBClick = 0;
	return bHandled = FALSE, S_OK;
}

LRESULT CRichEditImpl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// RichEdit control ранних версий (например что содержится в NT 4.0)
	// содержит ошибку, и падает если в lpszClass структуре CREATESTRUCT содержится не имя класса а атом.
	CREATESTRUCT * pCr = (CREATESTRUCT *)lParam;
	pCr->lpszClass = GetWndClassInfo().m_wc.lpszClassName;
	return bHandled = FALSE, 0;
}

void CRichEditImpl::ConvertCharformat2LogFont(HDC hDc, const CHARFORMAT2& cf, LOGFONT& lf)
{
	DWORD dwFlags = 0;
	if (cf.dwMask & CFM_SIZE)
	{
		LONG yPerInch = ::GetDeviceCaps(hDc, LOGPIXELSY);
		lf.lfHeight = -(int) ((cf.yHeight * yPerInch) / 1440);
	}
	else
		lf.lfHeight = 0;

	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	
	if ((cf.dwMask & (CFM_ITALIC|CFM_BOLD)) == (CFM_ITALIC|CFM_BOLD))
	{
		lf.lfWeight = (cf.dwEffects & CFE_BOLD) ? FW_BOLD : FW_NORMAL;
		lf.lfItalic = (BYTE)((cf.dwEffects & CFE_ITALIC) ? TRUE : FALSE);
	}
	else
	{
		dwFlags |= CF_NOSTYLESEL;
		lf.lfWeight = FW_DONTCARE;
		lf.lfItalic = FALSE;
	}
	
	if ((cf.dwMask & (CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR)) ==
		(CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR))
	{
		dwFlags |= CF_EFFECTS;
		lf.lfUnderline = (BYTE)((cf.dwEffects & CFE_UNDERLINE) ? TRUE : FALSE);
		lf.lfStrikeOut = (BYTE)((cf.dwEffects & CFE_STRIKEOUT) ? TRUE : FALSE);
	}
	else
	{
		lf.lfUnderline = (BYTE)FALSE;
		lf.lfStrikeOut = (BYTE)FALSE;
	}
	
	if (cf.dwMask & CFM_CHARSET)
		lf.lfCharSet = cf.bCharSet;
	else
		dwFlags |= CF_NOSCRIPTSEL;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	if (cf.dwMask & CFM_FACE)
	{
		lf.lfPitchAndFamily = cf.bPitchAndFamily;
		lstrcpy(lf.lfFaceName, cf.szFaceName);
	}
	else
	{
		lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
		lf.lfFaceName[0] = (TCHAR)0;
	}
}

void CRichEditImpl::CalcTextSize(CHARRANGE& cr, SIZE& size)
{
	CDesktopDC desktopDC;

	if (desktopDC)
	{
		CHARFORMAT2 cf;
		cf.cbSize = sizeof (cf);
		SendMessage(m_hWnd, EM_SETSEL, cr.cpMin, cr.cpMin+1);
		SendMessage(m_hWnd, EM_GETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM)&cf);
		SendMessage(m_hWnd, EM_SETSEL, -1, -1);

		LOGFONT lf;
		ConvertCharformat2LogFont(desktopDC, cf, lf);
		
		HFONT hFont = ::CreateFontIndirect(&lf);
		
		if (hFont)
		{
			HFONT hOldFont = (HFONT)::SelectObject(desktopDC, hFont);
			RECT rc;
			
			cStrObj strW; m_pRoot->LocalizeStr(strW, m_strDisplayText.c_str() + cr.cpMin, cr.cpMax - cr.cpMin);
			::DrawTextW(desktopDC, strW.data(), (int)strW.size(), &rc, DT_CALCRECT);

			size.cx = rc.right - rc.left;
			size.cy = rc.bottom - rc.top;

			::SelectObject(desktopDC, hOldFont);

			::DeleteObject(hFont);
		}
	}
}

void CRichEditImpl::CalcSingleLineRect(CHARRANGE& cr, RECT& rect)
{
	LRESULT dwRet = ::SendMessage(m_hWnd, EM_POSFROMCHAR, (WPARAM)cr.cpMin, (LPARAM)0);
	rect.left = (short)LOWORD(dwRet);
	rect.top = (short)HIWORD(dwRet);

	SIZE sz;
	CalcTextSize(cr, sz);

	rect.right = rect.left + sz.cx;
	rect.bottom = rect.top + sz.cy;

	RECT rcClient;

	::GetClientRect(m_hWnd, &rcClient);

	if ( rect.left>=rcClient.right || rect.right<=0 || rect.bottom<=0 || rect.top>=rcClient.bottom )
	{
		rect.left = 0;
		rect.right = 0;
		rect.bottom = 0;
		rect.top = 0;
		
		return;
	}

	if (rect.left<0)
		rect.left = 0;

	if (rect.top<0)
		rect.top = 0;

	if (rect.right>rcClient.right)
		rect.right=rcClient.right;

	if (rect.bottom>rcClient.bottom)
		rect.bottom = rcClient.bottom;

	// Richedit, у которого установлен WS_EX_CLIENTEDGE, возвращает EM_POSFROMCHAR со смещением
	if( m_nStylesEx & WS_EX_CLIENTEDGE )
		::OffsetRect(&rect, 2, 2);
}

void CRichEditImpl::DrawFocusRect()
{
	RECT rc;
	CHARRANGE cr = {m_aRefs[m_indSelLink]->m_chrRangeEx.x, m_aRefs[m_indSelLink]->m_chrRangeEx.y};
	CalcSingleLineRect(cr, rc);
	::InflateRect(&rc, 2, 0);

	HDC dc = ::GetWindowDC(m_hWnd);
	::DrawFocusRect(dc, &rc);
	::ReleaseDC(m_hWnd, dc);
}

LRESULT	CRichEditImpl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_nStylesEx & WS_EX_CLIENTEDGE )
		return bHandled = FALSE, 0;
	
	DrawParentContent((HDC)wParam);
	return TRUE;
}

LRESULT CRichEditImpl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc();

//	if( m_nState & ISTATE_FOCUSED && m_indSelLink < m_aRefs.size() )
//		DrawFocusRect();  bug fix 15138

	return 0;
}

LRESULT CRichEditImpl::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( !m_pRoot->IsThemed() || !(m_nStylesEx & WS_EX_CLIENTEDGE) )
		return bHandled = FALSE, 0;

	DefWindowProc();
	
	RECT rcBorder = {0, 0, m_szSize.cx, m_szSize.cy};
	
	HDC dc = ::GetWindowDC(m_hWnd);
	((CWinRoot *)m_pRoot)->DrawThemeBorder(dc, rcBorder);
	::ReleaseDC(m_hWnd, dc);
	return 0;
}

LRESULT CRichEditImpl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return bHandled = FALSE, 0;
}

LRESULT CRichEditImpl::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return bHandled = FALSE, 0;
}

LRESULT CRichEditImpl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	m_curLink.cpMin = m_curLink.cpMax = 0;
	return bHandled = FALSE, 0;
}

LRESULT CRichEditImpl::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return SendMessage(WM_LBUTTONDOWN, wParam, lParam);
}

void CRichEditImpl::UpdateRefsText(LPCSTR strText, bool bFull)
{
	m_bStylesApplied = 0;
	TBase::UpdateRefsText(strText, bFull);
	if( !m_strDisplayText.empty() && !(m_nFlags & AUTOSIZE_Y) )
		m_strDisplayText += "\n";
}

int __GetCharsLen(CRootItem * pRoot, LPSTR strText, int n)
{
	tCHAR c = strText[n];
	strText[n] = 0;
	
	cStrObj strUni; pRoot->LocalizeStr(strUni, strText);
	int nLen = strUni.size();
	
	strText[n] = c;
	return nLen;
}

void CRichEditImpl::ApplyRefStyles()
{
	LPSTR strDisplayText = &m_strDisplayText[0];

	for(size_t i = 0; i < m_aRefs.size(); i++)
	{
		POINT& _chrrRef = m_aRefs[i]->m_chrRange;
		POINT& _chrrRefEx = m_aRefs[i]->m_chrRangeEx;
		
		_chrrRefEx.x = __GetCharsLen(m_pRoot, strDisplayText, _chrrRef.x);
		_chrrRefEx.y = __GetCharsLen(m_pRoot, strDisplayText, _chrrRef.y);
		
		CHARRANGE _chrr;
		_chrr.cpMin = _chrrRefEx.x;
		_chrr.cpMax = _chrrRefEx.y;
	    SetRichBlockStyle(CFM_LINK, CFE_LINK, _chrr);
	}
}

void CRichEditImpl::ApplyBoldStyles()
{
	LPSTR strDisplayText = &m_strDisplayText[0];
	
	for(size_t i = 0; i < m_aBolds.size(); i++)
	{
		POINT& _chrrBold = m_aBolds[i];

		CHARRANGE _chrr;
		_chrr.cpMin = __GetCharsLen(m_pRoot, strDisplayText, _chrrBold.x);
		_chrr.cpMax = __GetCharsLen(m_pRoot, strDisplayText, _chrrBold.y);
		SetRichBlockStyle(CFM_BOLD, CFE_BOLD, _chrr);
	}
}

void CRichEditImpl::OnApplyBlockStyles()
{
	if( m_bStylesApplied )
		return;
	
	m_bStylesApplied = 1;

	ApplyRefStyles();
	ApplyBoldStyles();

	::SendMessage(m_hWnd, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
}

void CRichEditImpl::SetRichBlockStyle(DWORD dwMask, DWORD dwEffect, CHARRANGE& chrRange)
{
	if( CWindowL::m_bAnsi )
		::SendMessageA(m_hWnd, EM_SETSEL, (WPARAM)chrRange.cpMin, (LPARAM)chrRange.cpMax);
	else
		::SendMessageW(m_hWnd, EM_SETSEL, (WPARAM)chrRange.cpMin, (LPARAM)chrRange.cpMax);
	
	CHARFORMAT2 cf; cf.cbSize = sizeof(cf);
	cf.dwMask      = dwMask;
	cf.dwEffects   = dwEffect;

	::SendMessage(m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

void CRichEditImpl::OnCalcTextSize(HDC dc, SIZE& sz, tDWORD nFlags)
{
	LPCSTR strText = OnGetDisplayText();

	{
		RECT rcBound = {0, 0, sz.cx, sz.cy};
		m_pFont->Draw(dc, strText, rcBound, nFlags|TEXT_CALCSIZE);
		if( m_nFlags & AUTOSIZE_X )
			sz.cx = RECT_CX(rcBound) + 1;

		if( !strText || !*strText )
		{
			sz.cy = RECT_CY(rcBound);
			return;
		}
	}

	WinImplClass::SetFont(m_pFont->Handle());
	
	if( !m_bStylesApplied )
	{
		WinImplClass::SetWindowTextW(m_pRoot, m_pRoot->LocalizeStr(TOR_cStrObj, strText));
		OnApplyBlockStyles();
	}
	
	POINT lpDc = {::GetDeviceCaps(dc, LOGPIXELSX), ::GetDeviceCaps(dc, LOGPIXELSY)};
	
	FORMATRANGE fr;
	fr.hdc = dc;
	fr.hdcTarget = dc;
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;

	fr.rc.left = 0;
	fr.rc.top = 0;
	fr.rc.right = ::MulDiv(sz.cx, 1440, lpDc.x);
	fr.rc.bottom = ::MulDiv(sz.cy, 1440, lpDc.y);
	
	fr.rcPage = fr.rc;

	SendMessage(EM_FORMATRANGE, FALSE, (LPARAM)&fr);
	SendMessage(EM_FORMATRANGE, FALSE, NULL);

	WinImplClass::SetFont(m_pFont->Handle());
	m_bStylesApplied = 0, OnApplyBlockStyles();

	sz.cx = ::MulDiv(fr.rc.right, lpDc.x, 1440);
	sz.cy = ::MulDiv(fr.rc.bottom, lpDc.y, 1440);
}

void CRichEditImpl::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags, nStateMask, rc);
	
	if( flags & (UPDATE_FLAG_TEXT|UPDATE_FLAG_FONT) )
		m_bStylesApplied = 0;
	
	if( (nStateMask & ISTATE_HIDE) && !(m_nState & ISTATE_HIDE) )
		OnApplyBlockStyles();
}

//////////////////////////////////////////////////////////////////////
// CTreeImpl

CTreeImpl::CTreeImpl() :
	m_hStateImgList(NULL), m_fSetSel(0)
{
	m_nStyles = WS_CHILD|WS_TABSTOP|TVS_DISABLEDRAGDROP|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_SHOWSELALWAYS|TVS_TRACKSELECT;
	m_nStylesEx = WS_EX_CLIENTEDGE;
}

void CTreeImpl::OnInit()
{
	if( !CWindowL::m_bAnsi )
		TreeView_SetUnicodeFormat(m_hWnd, TRUE);

	if( m_nStyles & TVS_CHECKBOXES )
		_CreateStateImageList();

	TBase::OnInit();
}

void CTreeImpl::Destroy()
{
	_DestroyStateImageList();
	TBase::Destroy();
}

void CTreeImpl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);

	sswitch(strStyle)
	scase(STR_PID_NOSCROLL) m_nStyles |= TVS_NOSCROLL;     sbreak;
	stcase(checked)         m_nStyles |= TVS_CHECKBOXES;   sbreak;
	stcase(nobuttons)       m_nStyles &= ~TVS_HASBUTTONS;  sbreak;
	stcase(nolines)         m_nStyles &= ~TVS_HASLINES;    sbreak;
	stcase(nolinesatroot)   m_nStyles &= ~TVS_LINESATROOT; sbreak;
	send;
}

void CTreeImpl::SetImageList(hImageList hImages)
{
	SendMessage(TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)hImages);
}

cTreeItem * CTreeImpl::AppendItem(cTreeItem * pParent, cTreeItem * pItem, cTreeItem * pAfter)
{
	if( !pItem )
		return NULL;
	
	TVINSERTSTRUCT tvis; memset(&tvis, 0, sizeof(tvis));
	
	tvis.hParent = pParent ? (HTREEITEM)pParent->m_hItem : TVI_ROOT;
	tvis.hInsertAfter = pAfter ? (HTREEITEM)pAfter->m_hItem : TVI_LAST;
	
	tvis.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_CHILDREN;
	tvis.item.lParam = (LPARAM)pItem;
	tvis.item.iImage = I_IMAGECALLBACK;
	tvis.item.iSelectedImage = I_IMAGECALLBACK;
	tvis.item.pszText = LPSTR_TEXTCALLBACK;
	tvis.item.cChildren = I_CHILDRENCALLBACK;

	if( (m_nStyles & TVS_CHECKBOXES) && (pItem->m_stateMask & cTreeItem::fIconStateMask) )
	{
		tvis.item.mask |= TVIF_STATE;
		tvis.item.stateMask = pItem->m_stateMask;
		tvis.item.state = pItem->m_state;
	}
	
	pItem->m_hItem = (hTreeItem)TreeView_InsertItemL(m_pRoot, &tvis);
	return pItem;
}

void CTreeImpl::DeleteItem(cTreeItem * pItem)
{
	if( pItem ) TreeView_DeleteItem(m_hWnd, pItem->m_hItem);
}

void CTreeImpl::ExpandItem(cTreeItem * pItem, bool bExpand)
{
	TreeView_Expand(m_hWnd, pItem->m_hItem, bExpand ? TVE_EXPAND : TVE_COLLAPSE);
}

void CTreeImpl::SelectItem(cTreeItem * pItem)
{
	m_fSetSel = 1;
	if( pItem ) TreeView_SelectItem(m_hWnd, pItem->m_hItem);
}

void CTreeImpl::SetItemCheckState(cTreeItem * pItem, bool bChecked)
{
	if( pItem ) TreeView_SetCheckState(m_hWnd, (HTREEITEM)pItem->m_hItem, bChecked);
}

cTreeItem * CTreeImpl::GetParentItem(cTreeItem * pItem)
{
	if( !pItem )
		return NULL;
	return _GetItemData((hTreeItem)TreeView_GetParent(m_hWnd, pItem->m_hItem));
}

cTreeItem * CTreeImpl::GetChildItem(cTreeItem * pItem)
{
	return _GetItemData((hTreeItem)TreeView_GetChild(m_hWnd, pItem ? pItem->m_hItem : TVI_ROOT));
}

cTreeItem * CTreeImpl::GetNextItem(cTreeItem * pItem)
{
	return _GetItemData((hTreeItem)TreeView_GetNextSibling(m_hWnd, pItem ? pItem->m_hItem : TVI_ROOT));
}

cTreeItem * CTreeImpl::GetSelectionItem()
{
	return _GetItemData((hTreeItem)TreeView_GetSelection(m_hWnd));
}

cTreeItem * CTreeImpl::TreeHitTest(POINT &pt, UINT *pnFlags)
{
	*pnFlags = 0;

	TVHITTESTINFO ti = {0,};
	ti.pt = pt;
	if( TreeView_HitTest(m_hWnd, &ti) )
		*pnFlags = ti.flags;
	return _GetItemData((hTreeItem)ti.hItem);
}

void CTreeImpl::SortChildren(cTreeItem * pItem, bool bRecurse)
{
	if( !pItem )
		return;
	
	TVSORTCB sc = {(HTREEITEM)pItem->m_hItem, _CompareFunc, (LPARAM)this};
	TreeView_SortChildrenCB(m_hWnd, &sc, bRecurse);
}

void CTreeImpl::UpdateView(int nFlags)
{
	Invalidate(TRUE);
}

//////////////////////////////////////////////////////////////////////

int CTreeImpl::_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CTreeImpl * pImpl = (CTreeImpl *)lParamSort;
	if( !pImpl->m_pSink )
		return 0;
	return ((CTreeItemSink *)pImpl->m_pSink)->OnSortCompare((cTreeItem *)lParam1, (cTreeItem *)lParam2);
}

void CTreeImpl::_GetItemInfo(TVITEMA& item, bool bAnsi)
{
	cTreeItem * pItem = (cTreeItem *)item.lParam;
	if( !pItem )
		return;

	tDWORD flags = UPDATE_FLAG_STATE;
	
	if( item.mask & TVIF_TEXT && !(pItem->m_flags & UPDATE_FLAG_TEXT) )
	{
		RECT rcRect;
		if( TreeView_GetItemRect(m_hWnd, item.hItem, &rcRect, FALSE) )
			flags |= UPDATE_FLAG_TEXT;
	}

	if( item.mask & TVIF_IMAGE && !(pItem->m_flags & UPDATE_FLAG_ICON) )
		flags |= UPDATE_FLAG_ICON;
	if( item.mask & TVIF_SELECTEDIMAGE && !(pItem->m_flags & UPDATE_FLAG_ICONSELECTED) )
		flags |= UPDATE_FLAG_ICONSELECTED;
	if( item.mask & TVIF_CHILDREN && !(pItem->m_flags & UPDATE_FLAG_SUBCOUNT) )
		flags |= UPDATE_FLAG_SUBCOUNT;

	pItem->m_state = item.state;
	pItem->m_stateMask = item.stateMask;

	pItem->m_flags |= flags;
	if( m_pSink && flags )
		((CTreeItemSink *)m_pSink)->OnUpdateItemProps(pItem, flags);

	_UpdateItemStateIconState(pItem->m_state, pItem->m_stateMask);

	if( item.mask & TVIF_TEXT )
		item.pszText = bAnsi ? (LPSTR)m_pRoot->LocalizeStr(TOR_tString, pItem->m_strName.data(), pItem->m_strName.size()) : (LPSTR)pItem->m_strName.data();
	if( item.mask & TVIF_IMAGE )
		item.iImage = pItem->m_nImage;
	if( item.mask & TVIF_SELECTEDIMAGE )
		item.iSelectedImage = pItem->m_nSelectedImage;
	if( item.mask & TVIF_CHILDREN )
		item.cChildren = pItem->m_nChildren;

	item.mask |= TVIF_STATE;
	item.state = pItem->m_state;
	item.stateMask = pItem->m_stateMask;
}

cTreeItem * CTreeImpl::_GetItemData(hTreeItem hItem)
{
	if( !hItem )
		return NULL;
	
	TVITEM item = {0,};
	item.mask = TVIF_HANDLE|TVIF_PARAM;
	item.hItem = (HTREEITEM)hItem;
	TreeView_GetItem(m_hWnd, &item);
	return (cTreeItem *)item.lParam;
}

void CTreeImpl::_UpdateItemStateIconState(tDWORD &state, tDWORD &stateMask)
{
	if( stateMask & cTreeItem::fStateDisabled )
	{
		bool bChecked;
		switch(state & cTreeItem::fIconStateMask)
		{
		case cTreeItem::fIconStateUnchecked:
		case cTreeItem::fIconStateDisabledUnchecked:
			bChecked = false;
			break;
		case cTreeItem::fIconStateChecked:
		case cTreeItem::fIconStateDisabledChecked:
			bChecked = true;
			break;
		default:
			return;
		}

		state &= ~cTreeItem::fIconStateMask;
		stateMask |= cTreeItem::fIconStateMask;
		if( state & cTreeItem::fStateDisabled )
			state |= bChecked ? cTreeItem::fIconStateDisabledChecked : cTreeItem::fIconStateDisabledUnchecked;
		else
			state |= bChecked ? cTreeItem::fIconStateChecked : cTreeItem::fIconStateUnchecked;
	}
}

#define TREE_STATEICON_SIZE 16

bool CTreeImpl::_CreateStateImageList()
{
	if( m_hStateImgList )
		return true;
	
	m_hStateImgList = ::ImageList_Create(TREE_STATEICON_SIZE, TREE_STATEICON_SIZE, ILC_COLORDDB|ILC_MASK, 5, 1);
	
	_AddCheckboxIcon(m_hStateImgList, false, 0);
	_AddCheckboxIcon(m_hStateImgList, false, 0);
	_AddCheckboxIcon(m_hStateImgList, true,  0);
	_AddCheckboxIcon(m_hStateImgList, false, ISTATE_DISABLED);
	_AddCheckboxIcon(m_hStateImgList, true,  ISTATE_DISABLED);

	::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM)TVSIL_STATE, (LPARAM)m_hStateImgList);
	return true;
}

void CTreeImpl::_DestroyStateImageList()
{
	if( m_hStateImgList )
	{
		::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM)TVSIL_STATE, (LPARAM)NULL);
		::ImageList_Destroy(m_hStateImgList);
	}
}

void CTreeImpl::_AddCheckboxIcon(HIMAGELIST hList, bool bChecked, tDWORD nState)
{
	CWinRoot *pWinRoot = (CWinRoot *)m_pRoot;
	HDC hDesktopDC = pWinRoot->GetDesktopDC();

    CBitmapHnd hBm = ::CreateCompatibleBitmap(hDesktopDC, TREE_STATEICON_SIZE, TREE_STATEICON_SIZE);
    CBitmapHnd hBmMask = ::CreateCompatibleBitmap(hDesktopDC, TREE_STATEICON_SIZE, TREE_STATEICON_SIZE);
	{
		RECT rc = { 0, 0, TREE_STATEICON_SIZE, TREE_STATEICON_SIZE };

		CDcHnd hDC = ::CreateCompatibleDC(hDesktopDC);

		::SelectObject(hDC, hBmMask);
		::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
		
		::SelectObject(hDC, hBm);

		::FillRect(hDC, &rc, (HBRUSH)(COLOR_WINDOW + 1));

		rc.left += 2; rc.top += 2; rc.right -= 1; rc.bottom -= 1;
		pWinRoot->DrawCheckBox(hDC, rc, bChecked, nState);
	}

	::ImageList_Add((HIMAGELIST)hList, hBm, hBmMask);
}

bool CTreeImpl::AddStateIcon(CIcon * pIcon)
{
	if( !pIcon || !m_hStateImgList )
		return false;
	
	::ImageList_AddIcon(m_hStateImgList, pIcon->IconHnd());
	return true;
}
 
//////////////////////////////////////////////////////////////////////

LRESULT	CTreeImpl::OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	NMHDR * phdr = (NMHDR *)lParam;
	NM_TREEVIEW * pNMTreeView = (NM_TREEVIEW *)phdr;
		
	switch( phdr->code )
	{
	case TVN_SELCHANGEDA:
	case TVN_SELCHANGEDW:
		if( pNMTreeView->action == TVC_BYMOUSE || pNMTreeView->action == TVC_BYKEYBOARD || m_fSetSel )
			OnSelect(this);
		break;

	case TVN_GETDISPINFOA:
	case TVN_GETDISPINFOW:
		_GetItemInfo(((LPNMTVDISPINFO)phdr)->item, phdr->code == TVN_GETDISPINFOA);
		break;

	case TVN_DELETEITEMA:
	case TVN_DELETEITEMW:
		if( m_pSink )
			((CTreeItemSink*)m_pSink)->OnItemDestroy((cTreeItem *)((LPNMTREEVIEW)phdr)->itemOld.lParam);
		break;

	case TVN_ITEMEXPANDINGA:
	case TVN_ITEMEXPANDINGW:
		if( pNMTreeView->action & TVE_EXPAND )
		{
			HTREEITEM hItem = (HTREEITEM)pNMTreeView->itemNew.hItem;
			cTreeItem * pItem = (cTreeItem *)pNMTreeView->itemNew.lParam;

			if( !TreeView_GetChild(m_hWnd, hItem) )
			{
				m_bExpanding = FALSE;
				m_hOldCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

				tUINT nSubCount = 0;
				if( !m_pSink || !((CTreeItemSink *)m_pSink)->OnItemExpanding(pItem, nSubCount) )
				{
					TVITEM item;
					item.hItem = hItem;
					item.mask = TVIF_CHILDREN;
					item.cChildren = pItem->m_nChildren = nSubCount;
					TreeView_SetItemL(m_pRoot, &item);

					if( !item.cChildren )
						Invalidate(TRUE);
//					else
//						SetRedraw(FALSE), m_bExpanding = TRUE;
				}
				
				::SetCursor(m_hOldCursor);
			}
		}
		break;

	case TVN_ITEMEXPANDEDA:
	case TVN_ITEMEXPANDEDW:
//		if( (pNMTreeView->action & TVE_EXPAND) && m_bExpanding )
//			SetRedraw(TRUE), m_bExpanding = FALSE;
		break;

	case NM_CUSTOMDRAW:
		{
			NMTVCUSTOMDRAW *pCD = (NMTVCUSTOMDRAW *)phdr;
			switch(pCD->nmcd.dwDrawStage)
			{
				case CDDS_PREPAINT:
					return CDRF_NOTIFYITEMDRAW;

				case CDDS_ITEMPREPAINT:
					if( m_pSink && pCD->nmcd.lItemlParam )
					{
						cTreeItem *pItem = (cTreeItem *)pCD->nmcd.lItemlParam;
						if( pItem->m_state & cTreeItem::fStateDisabled )
							pCD->clrText = GetSysColor(COLOR_GRAYTEXT);
					}
					return CDRF_DODEFAULT;
				
				default:
					return CDRF_DODEFAULT;
			}
		}
		break;
	}
	
	return S_OK;
}

LRESULT CTreeImpl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if( m_nStyles & TVS_CHECKBOXES )
	{
		UINT nFlags;
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		cTreeItem * pItem = TreeHitTest(pt, &nFlags);
		
		if( pItem && nFlags == TVHT_ONITEMSTATEICON )
		{
			SelectItem(pItem);

			tDWORD state = pItem->m_state & cTreeItem::fIconStateMask;
			if( state != cTreeItem::fIconStateUnchecked && state != cTreeItem::fIconStateChecked )
			{
				bHandled = TRUE;
				return S_OK;
			}

			if( !((CTreeItemSink *)m_pSink)->OnItemChecking(pItem) )
			{
				bHandled = TRUE;
				return S_OK;
			}

			state = state == cTreeItem::fIconStateUnchecked ? cTreeItem::fIconStateChecked : cTreeItem::fIconStateUnchecked;
			TreeView_SetItemState(m_hWnd, (HTREEITEM)pItem->m_hItem, state, cTreeItem::fIconStateMask);
			bHandled = TRUE;
		}
	}
	
	return S_OK;
}

LRESULT CTreeImpl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if( wParam == VK_SPACE )
	{
		cTreeItem * pItem = GetSelectionItem();
		if( pItem && m_pSink )
		{
			if( !((CTreeItemSink *)m_pSink)->OnItemChecking(pItem) )
				bHandled = TRUE;
		}
	}
	
	return S_OK;
}

void CTreeImpl::DeleteChildItems(cTreeItem * pParent)
{
	TBase::DeleteChildItems(pParent);

	TVITEM item;
	item.hItem = (HTREEITEM)pParent->m_hItem;
	item.mask = TVIF_CHILDREN;
	item.cChildren = I_CHILDRENCALLBACK;
	TreeView_SetItemL(m_pRoot, &item);
}

//////////////////////////////////////////////////////////////////////
// CPopupMenu

CPopupMenu::~CPopupMenu()
{
	if( m_hFont )
		DeleteObject(m_hFont);
}

HMENU CPopupMenu::ctl_GetRootItem()
{
	if( !m_hMenu )
		m_hMenu = CreatePopupMenu();
	return m_hMenu;
}

HMENU CPopupMenu::ctl_AddItem(HMENU hMenu, CItemBase * pItem, bool bSub)
{
	MENUITEMINFOW mi; memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);

	DWORD nPos = ::GetMenuItemCount(hMenu);

	mi.fMask = MIIM_FTYPE|MIIM_ID|MIIM_DATA|MIIM_STATE;
	mi.fType = m_pBackground ? MFT_OWNERDRAW : 0;

	cStrObj strW;
	
	if( pItem->m_strItemId != "Separator" && pItem->m_strItemId != "sep" )
	{
		if( pItem->m_nState & ISTATE_DISABLED )
			mi.fState |= MFS_DISABLED;
		if( pItem->m_nState & ISTATE_DEFAULT )
			mi.fState |= MFS_DEFAULT;
		if( pItem->m_nState & ISTATE_SELECTED )
			mi.fState |= MFS_CHECKED;

		if( !m_pBackground )
		{
			m_pRoot->LocalizeStr(strW, pItem->m_strText.c_str(), pItem->m_strText.size());

			mi.fMask |= MIIM_TYPE; mi.fMask &= ~(MIIM_DATA|MIIM_FTYPE);
			mi.fType       = MFT_STRING|(pItem->m_nFlags & STYLE_ENUM_VALUE) ? MFT_RADIOCHECK : 0;
			mi.cch         = (UINT)strW.size();
			mi.dwTypeData  = (LPWSTR)strW.data();
		}
		else
			mi.dwItemData = (ULONG_PTR)pItem;

		mi.wID = pItem->GetItemId();

		if( bSub )
		{
			mi.fMask |= MIIM_SUBMENU;
			mi.hSubMenu = CreatePopupMenu();
		}
	}
	else
	{
		mi.fType |= MFT_SEPARATOR;
		mi.dwItemData = (ULONG_PTR)this;
		mi.wID = 0;
	}

	::InsertMenuItemW(hMenu, nPos, TRUE, &mi);

	if( m_pBackground && !m_hFont )
	{
		NONCLIENTMETRICS info; info.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &info, 0);
		m_hFont = CreateFontIndirect(&info.lfMenuFont);
		m_pCheck = m_pRoot->GetIcon("MenuCheck");
	}

	return mi.hSubMenu;
}

tDWORD CPopupMenu::ctl_Track(POINT& pt)
{
	if( !m_hMenu )
		return -1;
	
	if( pt.x == 0 && pt.y == 0 )
		::GetCursorPos(&pt);

	HWND hParent = GetParentHWND(m_pParent);
	if( !hParent )
		hParent = ((CWinRoot *)m_pRoot)->m_hWnd;
	
	cLoopStack pOldParams((CWinRoot *)m_pRoot, hParent, true, NULL);

	CleanSeparators(m_hMenu);
	
	tDWORD nResult = TrackPopupMenu(m_hMenu,
		TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,
		pt.x, pt.y, 0, hParent, NULL);
	return nResult;
}

void CPopupMenu::ctl_TrackEnd()
{
	::EndMenu();
}

void CPopupMenu::CleanSeparators(HMENU hMenu)
{
	bool bRemove = true;
	int i;
	
	for(i = 0; ; i++)
	{
		MENUITEMINFO mi; memset(&mi, 0, sizeof(MENUITEMINFO));
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_TYPE|MIIM_SUBMENU;
		if( !::GetMenuItemInfo(hMenu, i, TRUE, &mi) )
			break;
		
		if( mi.fType & MFT_SEPARATOR )
		{
			if( bRemove || ((i + 1) == ::GetMenuItemCount(hMenu)) )
			{
				::RemoveMenu(hMenu, i, MF_BYPOSITION);
				i--;
			}
			else
				bRemove = true;
		}
		else
		{
			if( mi.hSubMenu )
				CleanSeparators(mi.hSubMenu);

			bRemove = false;
		}
	}

	for(i = ::GetMenuItemCount(hMenu); i > 0; i--)
	{
		MENUITEMINFO mi; memset(&mi, 0, sizeof(MENUITEMINFO));
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_TYPE;
		if( !::GetMenuItemInfo(hMenu, i - 1, TRUE, &mi) )
			break;
		
		if( !(mi.fType & MFT_SEPARATOR) )
			break;
		
		::RemoveMenu(hMenu, i - 1, MF_BYPOSITION);
	}
}

LRESULT CPopupMenu::MeasureItem(LPMEASUREITEMSTRUCT lpInfo)
{
	CItemBase* pItem = (CItemBase*)lpInfo->itemData;
	if( !lpInfo->itemID )
	{
		lpInfo->itemWidth = 10;
		
		if( pItem->m_pRoot->IsThemed() )
			lpInfo->itemHeight = 5;
		else
			lpInfo->itemHeight = 9;
	}
	else
	{
		HDC dc = pItem->m_pRoot->GetDesktopDC();
		CPopupMenu* pMenu = (CPopupMenu*)pItem->GetOwner();
		SelectObject(dc, (HFONT)pMenu->m_hFont);

		SIZE szText;

		cStrObj strW; pMenu->m_pRoot->LocalizeStr(strW, pItem->m_strText.c_str(), pItem->m_strText.size());
		GetTextExtentPoint32W(dc, strW.data(), (int)strW.size(), &szText); 

		lpInfo->itemWidth = szText.cx + MENU_ICON_MARGIN + 10;
		lpInfo->itemHeight = MENU_LINE_HEIGHT;
	}
	
	return TRUE;
}

LRESULT CPopupMenu::DrawItem(LPDRAWITEMSTRUCT lpInfo)
{
	CItemBase* pItem = NULL;
	CPopupMenu* pMenu = NULL;
	if( lpInfo->itemID )
	{
		pItem = (CItemBase *)lpInfo->itemData;
		pMenu = (CPopupMenu*)pItem->GetOwner();
	}
	else
		pMenu = (CPopupMenu*)lpInfo->itemData;

	RECT rc = lpInfo->rcItem;
	HDC dc = lpInfo->hDC;

	SelectObject(dc, (HFONT)pMenu->m_hFont);

	::FillRect(dc, &rc, GetSysColorBrush(COLOR_MENU));

	RECT rcBg = {rc.left, rc.top, rc.left + MENU_ICON_MARGIN, rc.top + MENU_LINE_HEIGHT};
	if( pMenu->m_pRoot->IsThemed() && pMenu->m_pBackground )
		pMenu->m_pBackground->Draw(dc, rcBg);

	RECT rcDraw = {(2*rc.left + MENU_ICON_MARGIN - 16)/2, (2*rc.top + MENU_LINE_HEIGHT - 16)/2};
	rcDraw.right = rcDraw.left + 16;
	rcDraw.bottom = rcDraw.top + 16;

	COLORREF clTextColor = COLOR_MENUTEXT;
	if( lpInfo->itemState & ODS_DISABLED )
		clTextColor = COLOR_GRAYTEXT;
	else if( lpInfo->itemState & ODS_SELECTED )
	{
		RECT rcSel = {rc.left, rc.top, rc.right, rc.bottom};
		if( pMenu->m_pRoot->IsThemed() )
			rcSel.left += 1 + MENU_ICON_MARGIN;
		
		::FillRect(dc, &rcSel, GetSysColorBrush(COLOR_HIGHLIGHT));
		clTextColor = COLOR_WINDOW;
	}

	if( pItem && pItem->m_pIcon )
		pItem->m_pIcon->Draw(dc, rcDraw, NULL, pItem->m_pIcon.GetImage(0));

	if( pMenu->m_pCheck && (lpInfo->itemState & MFS_CHECKED) )
	{
		InflateRect(&rcDraw, 2, 2);
		pMenu->m_pCheck->Draw(dc, rcDraw, NULL, 0);
		if( !pItem->m_pIcon )
			pMenu->m_pCheck->Draw(dc, rcDraw, NULL, 1);
	}

	rc.left += MENU_ICON_MARGIN + 5;
	rc.right -= 5;

	if( !pItem )
	{
		if( pMenu->m_pRoot->IsThemed() )
		{
			RECT rcSep = {rc.left, (rc.top + rc.bottom)/2};
			rcSep.right = rc.right;
			rcSep.bottom = rcSep.top + 1;
			::FillRect(dc, &rcSep, GetSysColorBrush(COLOR_GRAYTEXT));
		}
		else
		{
			RECT rcSep = {rc.left - MENU_ICON_MARGIN - 4, (rc.top + rc.bottom)/2 - 1};
			rcSep.right = rc.right + 4;
			rcSep.bottom = rcSep.top + 2;
			::DrawEdge(dc, &rcSep, BDR_SUNKENOUTER, BF_TOP|BF_BOTTOM);
		}
	}
	else
	{
		::SetBkMode(dc, TRANSPARENT);
		::SetTextColor(dc, GetSysColor(clTextColor));

		cStrObj strW; pMenu->m_pRoot->LocalizeStr(strW, pItem->m_strText.c_str(), pItem->m_strText.size());
		::DrawTextW(dc, strW.data(), (int)strW.size(),
				&rc, DT_END_ELLIPSIS|DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTrayIcon

CTrayIcon::CChanger::CChanger(CRootItem *pRoot, UINT uID) :
	CRootTaskBase(pRoot, 0xc7c6742e), m_pbEnd(NULL)
{
	memset(&m_nid, 0, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = uID;
	m_nid.uCallbackMessage = KWS_TRAY_MESSAGE;
}

CTrayIcon::CChanger::~CChanger()
{
	if( m_nid.hIcon )
		::DestroyIcon(m_nid.hIcon);

	if( m_pbEnd )
		*m_pbEnd = true;
}

void CTrayIcon::CChanger::do_work()
{
	if( (m_nid.uFlags & NIF_ICON) && !m_nid.hIcon )
	{
		::Shell_NotifyIconW(NIM_DELETE, &m_nid);
		return;
	}
	
	if( !::Shell_NotifyIconW(NIM_MODIFY, &m_nid) )
	{
		m_nid.uFlags |= NIF_MESSAGE;
		::Shell_NotifyIconW(NIM_ADD, &m_nid);
	}
}

//////////////////////////////////////////////////////////////////////

UINT const WM_TASKBAR_CREATED		= RegisterWindowMessage("TaskbarCreated");
UINT const KWS_TRAY_MESSAGE			= RegisterWindowMessage("2DC65F81-AD9F-491c-99F8-9274D7945644");

UINT CTrayIcon::g_uTrayCount = 0;

CTrayIcon::CTrayIcon()
{
	m_nFlags |= STYLE_CLICKABLE|STYLE_DESTROY_ICON|STYLE_WINDOW;
	m_uID = ++g_uTrayCount;
}

void CTrayIcon::InitItem(LPCSTR strType)
{
	RECT rcFrame = {0,0,0,0};
	Create(NULL, rcFrame);
}

void CTrayIcon::Cleanup()
{
	CTrayItem::SetIcon((CIcon *)NULL);

	CTrayItem::Cleanup();
	DestroyWindow();
}

void CTrayIcon::Click(bool bForce, bool rClick)
{
	if( !m_pBinding || !m_pBinding->GetHandler(CItemBinding::hOnClick) )
		LoadItemMenu(NULL, LOAD_MENU_FLAG_DEFITEM|LOAD_MENU_FLAG_DESTROY);
	CTrayItem::Click(bForce, rClick);
}

void CTrayIcon::OnInit()
{
	m_pFont = NULL;
}

void CTrayIcon::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	bool bSync = false;
	
	CChanger * pChanger = new CChanger(m_pRoot, m_uID);
	NOTIFYICONDATAW& nid = pChanger->m_nid;
	nid.hWnd = m_hWnd;
	
	if( flags & (UPDATE_FLAG_TEXT|UPDATE_FLAG_ICON) )
		flags |= UPDATE_FLAG_TEXT|UPDATE_FLAG_ICON;

	if( flags & UPDATE_FLAG_TEXT )
	{
		OSVERSIONINFOEX& l_osVer = ((CWinRoot *)m_pRoot)->m_osVer;

		bool bExtendedTip = 
			(l_osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ||
			l_osVer.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
			l_osVer.dwMajorVersion >= 5;

		nid.uFlags |= NIF_TIP;
		
		cStrObj strW; m_pRoot->LocalizeStr(strW, m_strText.c_str(), m_strText.size());
		CompressText(nid.szTip,
			bExtendedTip ? countof(nid.szTip) : min(64, countof(nid.szTip)),
			strW.data(), (int)strW.size());

		if( !bExtendedTip )
		{
			bool bPrevLF = false;
			for(int i = 0; i < countof(nid.szTip); i++)
				if( nid.szTip[i] == '\n' )
				{
					nid.szTip[i] = bPrevLF ? ' ' : '.';
					bPrevLF = true;
				}
				else
					bPrevLF = false;
		}

		m_nState &= ~ISTATE_CHANGED;
	}
	
	if( flags & UPDATE_FLAG_ICON )
	{
		nid.uFlags |= NIF_ICON;
		if( m_pIcon )
			nid.hIcon = ::CopyIcon(m_pIcon->IconHnd());
		else
			bSync = true;
	}

	if( !nid.uFlags )
	{
		delete pChanger;
		return;
	}

	bool bEndWait = false;
	if( bSync )
		pChanger->m_pbEnd = &bEndWait;
	
	if( !pChanger->start() )
		delete pChanger;

	if( bSync )
		while(!bEndWait)
			::Sleep(50);
}

void CTrayIcon::TimerRefresh(tDWORD nTimerSpin)
{
	if( GUI_TIMERSPIN_INTERVAL(15000) )
		OnUpdateProps(UPDATE_FLAG_ALL);
	
	CTrayItem::TimerRefresh(nTimerSpin);
}

void CTrayIcon::ShowBalloon(LPCSTR szText, LPCSTR szTitle, LPCSTR szValue, tDWORD uTimeout, tDWORD dwIconType)
{
/*
	nid.uFlags = NIF_INFO;
	nid.uTimeout = uTimeout;
	nid.dwInfoFlags = dwIconType;
	
	CompressText(nid.szInfo,      countof(nid.szInfo),      szText);
	CompressText(nid.szInfoTitle, countof(nid.szInfoTitle), szTitle);

	m_strBalloonText = szText ? szText : "";
	m_strCommand = szValue ? szValue : "";
	Shell_NotifyIcon(NIM_MODIFY, &nid);
*/
}

LPCSTR CTrayIcon::GetBalloonText()
{
	return m_strBalloonText.c_str();
}

LRESULT CTrayIcon::OnMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT ptMenu = {(LONG)(lParam >> 16), (LONG)(lParam & 0xFFFF)};
	SetForegroundWindow(m_hWnd);
	LoadItemMenu(&ptMenu);
	return TRUE;
}

LRESULT CTrayIcon::OnSelDefItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Click();
	return TRUE;
}

LRESULT CTrayIcon::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return bHandled = TRUE, TRUE;
}

LRESULT CTrayIcon::OnTrayMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( uMsg != KWS_TRAY_MESSAGE || wParam != m_uID )
		return S_OK;	// this is not our icon

	switch( lParam )
	{
	case WM_CONTEXTMENU:
	case WM_RBUTTONUP:
		POINT point;
		if( GetCursorPos(&point) )
		{
			SendMessage(lParam, 0, point.x<<16|point.y);
			return 1;
		}
		break;

	case WM_LBUTTONDOWN:
		SendMessage(WM_LBUTTONDOWN, 0, 0);
		return 1;

	default:
		break;
	}

	return S_OK;
}

LRESULT CTrayIcon::OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OnUpdateProps(UPDATE_FLAG_ALL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CWinHTMLCtrl

class CDocHostUIHandlerDispImpl : public IDocHostUIHandlerDispatch
{
public:
// IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return E_NOINTERFACE;
	}
    
    ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return 1;
	}
    
    ULONG STDMETHODCALLTYPE Release( void)
	{
		return 1;
	}

// IDispatch
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ UINT *pctinfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Invoke( 
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr)
	{
		return E_NOTIMPL;
	}
	
// IDocHostUIHandler
	// MSHTML requests to display its context menu
	HRESULT STDMETHODCALLTYPE ShowContextMenu( 
		/* [in] */ DWORD dwID,
		/* [in] */ DWORD x,
		/* [in] */ DWORD y,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pdispReserved,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal)
	{
		return *dwRetVal = S_OK;
	}
        
	// Called at initialisation to find UI styles from container
	HRESULT STDMETHODCALLTYPE GetHostInfo( 
		/* [out][in] */ DWORD __RPC_FAR *pdwFlags,
		/* [out][in] */ DWORD __RPC_FAR *pdwDoubleClick)
	{
		*pdwFlags |= m_dwFlags;
		return S_OK;
	}
        
	// Allows the host to replace the IE4/MSHTML menus and toolbars. 
	HRESULT STDMETHODCALLTYPE ShowUI( 
		/* [in] */ DWORD dwID,
		/* [in] */ IUnknown __RPC_FAR *pActiveObject,
		/* [in] */ IUnknown __RPC_FAR *pCommandTarget,
		/* [in] */ IUnknown __RPC_FAR *pFrame,
		/* [in] */ IUnknown __RPC_FAR *pDoc,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal)
	{
		return *dwRetVal = S_OK;
	}

	// Called when IE4/MSHTML removes its menus and toolbars. 
	HRESULT STDMETHODCALLTYPE HideUI( void)
	{
		return S_OK;
	}

	// Notifies the host that the command state has changed. 
	HRESULT STDMETHODCALLTYPE UpdateUI( void)
	{
		return S_OK;
	}

	// Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::EnableModeless
	HRESULT STDMETHODCALLTYPE EnableModeless( 
		/* [in] */ VARIANT_BOOL fEnable)
	{
		return S_OK;
	}

	// Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnDocWindowActivate
	HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return S_OK;
	}

	// Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnFrameWindowActivate. 
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( 
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return S_OK;
	}

	// Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::ResizeBorder.
	HRESULT STDMETHODCALLTYPE ResizeBorder( 
		/* [in] */ long left,
		/* [in] */ long top,
		/* [in] */ long right,
		/* [in] */ long bottom,
		/* [in] */ IUnknown __RPC_FAR *pUIWindow,
		/* [in] */ VARIANT_BOOL fFrameWindow)
	{
		return S_OK;
	}
        
	// Called by IE4/MSHTML when IOleInPlaceActiveObject::TranslateAccelerator or IOleControlSite::TranslateAccelerator is called. 
	HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
		/* [in] */ DWORD_PTR hWnd,
		/* [in] */ DWORD nMessage,
		/* [in] */ DWORD_PTR wParam,
		/* [in] */ DWORD_PTR lParam,
		/* [in] */ BSTR bstrGuidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal)
	{
		return S_FALSE;
	}

	// Returns the registry key under which IE4/MSHTML stores user preferences. 
	// Returns S_OK if successful, or S_FALSE otherwise. If S_FALSE, IE4/MSHTML will default to its own user options.
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
		/* [out] */ BSTR __RPC_FAR *pbstrKey,
		/* [in] */ DWORD dw)
	{
		if (pbstrKey == NULL)
			return E_POINTER;
		return S_FALSE;
	}

	// Called by IE4/MSHTML when it is being used as a drop target to allow the host to supply an alternative IDropTarget
	HRESULT STDMETHODCALLTYPE GetDropTarget( 
		/* [in] */ IUnknown __RPC_FAR *pDropTarget,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDropTarget)
	{
		return E_NOTIMPL;
	}

	// Called by IE4/MSHTML to obtain the host's IDispatch interface
	HRESULT STDMETHODCALLTYPE GetExternal(
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
	{
		return E_NOINTERFACE;
	}

	// Called by IE4/MSHTML to allow the host an opportunity to modify the URL to be loaded
	HRESULT STDMETHODCALLTYPE TranslateUrl( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ BSTR bstrURLIn,
		/* [out] */ BSTR __RPC_FAR *pbstrURLOut)
	{
		return S_FALSE;
	}

	// Called on the host by IE4/MSHTML to allow the host to replace IE4/MSHTML's data object.
	// This allows the host to block certain clipboard formats or support additional clipboard formats. 
	HRESULT STDMETHODCALLTYPE FilterDataObject( 
		/* [in] */ IUnknown __RPC_FAR *pDO,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDORet)
	{
		return S_FALSE;
	}

	CDocHostUIHandlerDispImpl() : m_dwFlags(DOCHOSTUIFLAG_THEME|DOCHOSTUIFLAG_DISABLE_HELP_MENU) {}

	DWORD m_dwFlags;
};

CWinHTMLCtrl::CWinHTMLCtrl()
{
	AtlAxWinInit();
	m_pDocHostUI = new CDocHostUIHandlerDispImpl;
	m_szScroll.cx = m_szScroll.cy = 0;
	m_bNavigateComplete = false;
}

CWinHTMLCtrl::~CWinHTMLCtrl()
{
	delete m_pDocHostUI;
}

void CWinHTMLCtrl::InitItem(LPCSTR strType)
{
	TBase::InitItem(strType);

	CComPtr<IUnknown> punkCtrl;

    // Create the browser control using its CLSID.
    CreateControlEx( L"Shell.Explorer", NULL, NULL, &punkCtrl );

	m_pWB2 = punkCtrl;
	
	if( m_pWB2 )
	{
		SetExternalUIHandler(m_pDocHostUI);
		Navigate( "about:blank" );
		if( FAILED(DispEventAdvise(m_pWB2)) )
		{
			PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::InitItem: failed to advise for events"));
		}
	}
	else
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::InitItem: failed to create Shell.Explorer control"));
	}
}

void CWinHTMLCtrl::Cleanup()
{
	if( m_pWB2 )
		DispEventUnadvise(m_pWB2);
	TBase::Cleanup();
}

void CWinHTMLCtrl::OnInited()
{
	TBase::OnInited();
	RenderText();
}

void CWinHTMLCtrl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	sswitch(strStyle)
	scase(STR_PID_NOBORDER) m_pDocHostUI->m_dwFlags |= DOCHOSTUIFLAG_NO3DBORDER; sbreak;
	scase(STR_PID_NOSCROLL) m_pDocHostUI->m_dwFlags |= DOCHOSTUIFLAG_SCROLL_NO; sbreak;
	scase(STR_PID_NOFOCUS) m_pDocHostUI->m_dwFlags |= DOCHOSTUIFLAG_DIALOG; sbreak;
	send
}

void CWinHTMLCtrl::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	TBase::OnUpdateProps(flags, nStateMask, rc);

	if( flags & (UPDATE_FLAG_TEXT | UPDATE_FLAG_STATE | UPDATE_FLAG_SIZE) )
		RenderText();
}

bool CWinHTMLCtrl::Navigate(LPCSTR szUrl)
{
	if( !m_pWB2 )
		return false;

	m_bNavigateComplete = false;

	CComBSTR bstrUrl(szUrl);
	CComVariant varFlags(0, VT_I4);
	CComVariant varTargetFrameName("");
	CComVariant varPostData("");
	CComVariant varHeaders("");

	HRESULT hr = m_pWB2->Navigate(bstrUrl, &varFlags, &varTargetFrameName, &varPostData, &varHeaders);
	if( FAILED(hr) )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Navigate: failed to navigate, err = 0x%08X", hr));
	}

	return true;
}

bool CWinHTMLCtrl::Submit(LPCSTR szFormName)
{
	if( !m_pWB2 )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: no active browser control"));
		return false;
	}

	// получаем докмент, связанный с браузером
	CComQIPtr<IHTMLDocument2> pDoc;
	GetHTMLDocument(pDoc);
	if( !pDoc )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: failed to get HTMLDocument"));
		return false;
	}

	// получаем все формы
	CComPtr<IHTMLElementCollection> pForms;
	HRESULT hr = pDoc->get_forms(&pForms);
	if( FAILED(hr) || !pForms )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: failed to get any form from document, err = 0x%08X", hr));
		return false;
	}

	long length = 0;
	hr = pForms->get_length(&length);
	if( FAILED(hr) )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: failed to get length of forms collection, err = 0x%08X", hr));
		return false;
	}

	bool bFound(false);
	CComQIPtr<IHTMLFormElement> pForm;
	// перебираем все формы
	for( long i=0; i<length; ++i )
	{
		CComPtr<IDispatch> pDisp;
		CComVariant varIdx(i);
		hr = pForms->item(varIdx, varIdx, &pDisp);
		if( FAILED(hr) || !pDisp )
		{
			PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: failed to get form from collection, err = 0x%08X", hr));
			continue;
		}

		pForm = pDisp;
		if( szFormName )
		{
			// если имя формы задано, то проверяем на совпадение имени
			CComBSTR bstrName;
			if( SUCCEEDED(pForm->get_name(&bstrName)) && 
				bstrName == szFormName )
			{
				bFound = true;
				break;
			}
		}
		else
		{
			// если имя формы не задано, то берем первую из видимых -
			// для которых не задано style="display: none"
			CComQIPtr<IHTMLElement> pElem(pDisp);
			CComPtr<IHTMLStyle> pStyle;
			if( SUCCEEDED(pElem->get_style(&pStyle)) && pStyle )
			{
				CComBSTR bstrDyspaly;
				if( SUCCEEDED(pStyle->get_display(&bstrDyspaly)) && bstrDyspaly.m_str )
				{
					bstrDyspaly.ToLower();
					if( wcsstr(bstrDyspaly.m_str, L"none") )
					{
						// эта форма - невидимая
						continue;
					}
				}
			}
			bFound = true;
			break;
		}
	}

	if( !bFound || !pForm )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: failed to find needed form"));
		return false;
	}

	// сабмитим найденную форму
	hr = pForm->submit();
	if( FAILED(hr) )
	{
		PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::Submit: failed to submit found form, err = 0x%08X", hr));
		return false;
	}

	return true;
}

void CWinHTMLCtrl::GetHTMLDocument(CComQIPtr<IHTMLDocument2>& pDoc)
{
	CComPtr<IDispatch> pDisp;
	m_pWB2->get_Document(&pDisp);
	pDoc = pDisp;
}

bool CWinHTMLCtrl::NeedScroll(CComQIPtr<IHTMLDocument2>& pDoc)
{
	if (!pDoc)
		return false;

	CComPtr<IHTMLElement> lpBodyElement;
	pDoc->get_body(&lpBodyElement);
	if (!lpBodyElement)
		return false;

	CComPtr<IDispatch> lpDispatch1;
	lpBodyElement->get_all(&lpDispatch1);
	if (!lpDispatch1)
		return false;

	CComPtr<IHTMLElementCollection> pAll;
	lpDispatch1.QueryInterface(&pAll);
	if (!pAll)
		return false;

	RECT rcClient;
	GetClientRectEx(rcClient, true);
	int x = rcClient.right - rcClient.left, y = rcClient.bottom - rcClient.top;
	
	long length;
	pAll->get_length(&length);
	for (long i = 0; i < length; ++i)
	{
		VARIANT index; 
		::VariantInit(&index);
		index.vt = VT_I4;
		index.lVal = i;
		CComPtr<IHTMLElement> el;
		CComPtr<IDispatch> lpD;
		pAll->item(index, index, &lpD);
		lpD.QueryInterface(&el);
		long w, h, t, l;
		el->get_offsetWidth(&w);
		el->get_offsetHeight(&h);
		el->get_offsetTop(&t);
		el->get_offsetLeft(&l);
		if (t + h > y || l + h > x)
			return true;
	}

	return false;
}

void CWinHTMLCtrl::RenderText()
{
	if( !(m_nState & ISTATE_HIDE) && m_pWB2 )
	{
		CComQIPtr<IHTMLDocument2> pDoc;

		DWORD dwBeg = GetTickCount();
		while( !pDoc )
		{
			// В IE 5.0 для того, чтобы после навигации появился объект документа нужно разребать очередь сообщений
			MSG message;
			while( PeekMessage(&message, NULL, 0, 0, PM_REMOVE) )
			{
				TranslateMessage( &message );
				DispatchMessage( &message );
			}

			// bf 23911
			// m_pWB2->get_Document иногда падает где-то внутри IWebBrowser2,
			// предполагаем, что вызываем get_Document слишком рано, когда Navigate ещё
			// не отработал и поэтому внутри IWebBrowser2 ещё не всё проинициализировалось
			// или что-то подобное.
			// Как костыль, ждём прихода NavigateComplete или 30 секунд (чтобы не повиснуть)
			if (!m_bNavigateComplete && (GetTickCount() - dwBeg < 30000))
			{
				Sleep(10); // чтобы не грузить CPU на 100% если в очереди нет сообщений.
				continue;
			}

			GetHTMLDocument(pDoc);
		}

		CComBSTR bstrHTML(m_pRoot->LocalizeStr(TOR_cStrObj, m_strText.c_str(), m_strText.size()));

		SAFEARRAY* psa = ::SafeArrayCreateVector(VT_VARIANT, 0, 1);
		VARIANT *param;
		::SafeArrayAccessData(psa, (LPVOID*)&param);
		param->vt		= VT_BSTR;
		param->bstrVal	= bstrHTML.Detach();

		pDoc->write(psa);
		pDoc->close();
		bool bNeedScroll = NeedScroll(pDoc);
		if (bNeedScroll && m_pDocHostUI->m_dwFlags & DOCHOSTUIFLAG_SCROLL_NO)
		{
			m_pDocHostUI->m_dwFlags &= ~DOCHOSTUIFLAG_SCROLL_NO;
			m_pWB2->Refresh();
		}
		else if (!bNeedScroll && !(m_pDocHostUI->m_dwFlags & DOCHOSTUIFLAG_SCROLL_NO))
		{
			m_pDocHostUI->m_dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
			m_pWB2->Refresh();
		}

		::SafeArrayUnaccessData(psa);
		::SafeArrayDestroy(psa);
	}
}

STDMETHODIMP_(void) CWinHTMLCtrl::BeforeNavigate2(
	IDispatch *pDisp,
	VARIANT *URL,
	VARIANT *Flags,
	VARIANT *TargetFrameName,
	VARIANT *PostData,
	VARIANT *Headers,
	VARIANT_BOOL *Cancel
)
{
	bool bCancel = false;

	_USES_CONVERSION(((CWinRoot *)m_pRoot)->m_nCodePage);

	tString strURL = URL->bstrVal ? OLE2A(URL->bstrVal) : _T("");
	tString strHeaders = Headers->bstrVal ? OLE2A(Headers->bstrVal) : _T("");

	VARIANT* vtPostedData = PostData->pvarVal;
	if (vtPostedData->vt & VT_ARRAY)
	{
		if( vtPostedData->parray->cDims != 1 && vtPostedData->parray->cbElements != 1 )
		{
			// must be a vector of bytes!
			PR_TRACE((g_root, prtDANGER, "GUI\tCWinHTMLCtrl::BeforeNavigate2: wrong posted data"));
			return;
		}

		tSIZE_T nPostDataLen = vtPostedData->parray->rgsabound[0].cElements;
		tPTR pPostData;

		::SafeArrayAccessData(vtPostedData->parray, &pPostData);
		
		OnNavigate(this, strURL, Flags->intVal, pPostData, nPostDataLen, strHeaders, bCancel);
		
		::SafeArrayUnaccessData(vtPostedData->parray);
	}


	*Cancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;

	if (!bCancel)
		m_bNavigateComplete = false;
};

STDMETHODIMP_(void) CWinHTMLCtrl::NavigateComplete2(
		   IDispatch *pDisp,
		   VARIANT *URL)
{
	m_bNavigateComplete = true;
}


//////////////////////////////////////////////////////////////////////

static struct
{
	BYTE Hide;
	BYTE Grow;
}alpha[] = 
{
	{255,0},
	{255,32},
	{255,64},
	{255,96},
	{255,128},
	{255,160},
	{255,192},
	{255,224},
	{255,255}
};

/*
{255,0},
{245,0},
{240,0},
{235,0},
{230,0},
{225,0},
{220,0},
{215,0},
{210,0},
{205,50},
{200,55},
//	{200,55},
{175,80},
{150,105},
{125,130},
{100,155},
{75,180},
//	{50,205},
{25,230},
{0,255}
*/

#define CYCLE_NEXT(current, container_size) ((current+1 >= container_size)?0:current+1)
#define CYCLE_PREV(current, container_size) (current?current-1:container_size-1)

CSlideShowCtrl::CSlideShowCtrl()
{
	m_iCurrentSlide = 0;
	m_iFrame = 0;

	m_nDelay = 0;
	m_bAnimateInProgress = false;
	m_dwLastTimeSpin = 0;
}

void CSlideShowCtrl::OnInit()
{
	if (_CmnSink.m_pItem)
		_CmnSink.m_pItem->AttachSink(NULL,false);
}

bool CSlideShowCtrl::OnClicked(CItemBase* pItem)
{
    if (!m_bAnimateInProgress)
    {
        return CItemBase::OnClicked(pItem);
        GoNextSlide();
    }
	return false;
}


void CSlideShowCtrl::GoNextSlide()
{
	if (!m_aItems.size ())
		return;

	m_iCurrentSlide = CYCLE_NEXT(m_iCurrentSlide, (int) m_aItems.size());
	m_iFrame = 0;
	m_OldDC.Clear();
	//= m_ActiveDC;
	m_ActiveDC.Clear();

	m_bAnimateInProgress = true;

	Update();

}



void CSlideShowCtrl::TimerRefresh(tDWORD nTimerSpin)
{
	if (m_bAnimateInProgress /*&& GUI_TIMERSPIN_INTERVAL(TIMER_REFRESH_INT)*/)
	{
		m_iFrame = CYCLE_NEXT(m_iFrame, sizeof(alpha)/sizeof(alpha[0]));
		m_bAnimateInProgress = !!m_iFrame;
		m_dwLastTimeSpin = nTimerSpin;

		Update();
		return;
	}


	if ( ((tUINT) m_nDelay <= (nTimerSpin-m_dwLastTimeSpin)*TIMER_REFRESH_INT) && IsAutoSlideShow())
		GoNextSlide();

}

void CSlideShowCtrl::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	CItemBase::InitProps(strProps, pCtx);
	m_nDelay = (int)strProps.Get(STR_PID_DELAY).GetLong();

	if (m_nDelay<0)
		m_nDelay = 0;
}


void CSlideShowCtrl::DrawChildren(HDC dc, RECT* rcUpdate)
{
	if (!m_aItems.size())
		return;

	RECT rcFrame;
	GetUpdateRectEx(rcFrame, rcUpdate);

	if (m_bAnimateInProgress)
	{
	    if (!m_ActiveDC.IsValid())
        {
	        m_ActiveDC.Clear();
	        m_ActiveDC.CreateTransparentDC(dc, rcFrame);
	        m_aItems[m_iCurrentSlide]->Draw(m_ActiveDC.m_DC, NULL);
        }

	    if (!m_OldDC.IsValid())
	    {
		    m_OldDC.Clear();
		    m_OldDC.CreateTransparentDC(dc, rcFrame);
		    m_aItems[CYCLE_PREV(m_iCurrentSlide, m_aItems.size())]->Draw(m_OldDC.m_DC, NULL);
	    }

		m_OldDC.BlendTo(dc, rcFrame, alpha[m_iFrame].Hide);
		m_ActiveDC.BlendTo(dc, rcFrame, alpha[m_iFrame].Grow);
	}
	else
		m_aItems[m_iCurrentSlide]->Draw(dc, &rcFrame);

}

//////////////////////////////////////////////////////////////////////
// CSplitCtrl

CSplitCtrl::CSplitCtrl() :
	m_hCursor(NULL) 
{
	m_nStyles = WS_CHILD;

	m_hCursor = ::LoadCursor(NULL, m_bVertical ? IDC_SIZEWE : IDC_SIZENS);

	CSplitterItem::SetSplitBarSize(::GetSystemMetrics(m_bVertical ? SM_CXSIZEFRAME : SM_CYSIZEFRAME));
	CSplitterItem::SetMinLength(30);
}

CSplitCtrl::~CSplitCtrl()
{
	if( m_hCursor )
		DestroyCursor(m_hCursor);
}

void CSplitCtrl::ApplyStyle(LPCSTR strStyle)
{
	TBase::ApplyStyle(strStyle);
	
	sswitch(strStyle)
	scase(STR_PID_VERTICAL)
	{
		m_bVertical = true;
		if( m_hCursor )
			DestroyCursor(m_hCursor);
		m_hCursor = ::LoadCursor(NULL, m_bVertical ? IDC_SIZEWE : IDC_SIZENS);
		sbreak;
	}
	send;
}

LRESULT CSplitCtrl::OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pBackground)	m_pBackground->Draw((HDC)wParam, m_rcSplitter);
	else				DrawParentContent((HDC)wParam, &m_rcSplitter);
	return TRUE;
}

LRESULT CSplitCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if( ::PtInRect(&m_rcSplitter, pt) )
	{
		SetCapture();
		::SetCursor(m_hCursor);
		m_ptStart = pt;
	}

	bHandled = FALSE;
	return 1;
}

LRESULT CSplitCtrl::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ReleaseCapture();
	bHandled = FALSE;
	return 1;
}

LRESULT CSplitCtrl::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if((wParam & MK_LBUTTON) && ::GetCapture() == m_hWnd)
	{
		float fRatio = 0.5f;
		
		if( m_bVertical ) { 
			if( pt.x < m_rcBorder.left + m_cxyMinLength )
				pt.x = m_rcBorder.left + m_cxyMinLength;
			if( pt.x > m_rcBorder.right - m_cxyMinLength )
				pt.x = m_rcBorder.right - m_cxyMinLength;

			fRatio = (float)pt.x/(float)RECT_CX(m_rcBorder);
		}
		else	{
			if( pt.y < m_rcBorder.top + m_cxyMinLength )
				pt.y = m_rcBorder.top + m_cxyMinLength;
			if( pt.y > m_rcBorder.bottom - m_cxyMinLength )
				pt.y = m_rcBorder.bottom - m_cxyMinLength;

			fRatio = (float)pt.y/(float)RECT_CY(m_rcBorder);
		}

		CSplitterItem::SetRatio(fRatio);
		Update(&m_rcBorder);
	}
	else	{		// not dragging, just set cursor
		if( ::PtInRect(&m_rcSplitter, pt) )
			::SetCursor(m_hCursor);
		bHandled = FALSE;
	}

	return 0;
}

LRESULT CSplitCtrl::OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if((HWND)wParam == m_hWnd && LOWORD(lParam) == HTCLIENT)
	{
		DWORD dwPos = ::GetMessagePos();
		POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) }; 
		ScreenToClient(&pt);

		if( ::PtInRect(&m_rcSplitter, pt) )
			return bHandled = TRUE, 1;
	}

	bHandled = FALSE;
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CHexViewCtrl

CHexViewCtrl::CHexViewCtrl()
{
	m_nFlags |= STYLE_DRAW_CONTENT|STYLE_DRAW_BACKGROUND|STYLE_TRANSPARENT; // STYLE_CLICKABLE|
	m_nStyles |= WS_TABSTOP|WS_VSCROLL;
	m_nStylesEx = WS_EX_CLIENTEDGE;
}

LRESULT CHexViewCtrl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_MsgPaint();
	return bHandled = TRUE, S_OK;
}

LRESULT CHexViewCtrl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RecalcLayouts();
	return bHandled = FALSE, 0;
}

LRESULT CHexViewCtrl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
		case VK_DOWN:	
			OnVScroll(0, SB_LINEDOWN, NULL, bHandled); 
			break;
		case VK_UP:		
			OnVScroll(0, SB_LINEUP, NULL, bHandled); 
			break;
		case VK_PRIOR:	
			OnVScroll(0, SB_PAGEUP, NULL, bHandled); 
			break;
		case VK_NEXT:	
			OnVScroll(0, SB_PAGEDOWN, NULL, bHandled); 
			break;
		case VK_HOME:	
			OnVScroll(0, MAKEWPARAM(SB_THUMBTRACK, 0), NULL, bHandled);	
			break;
		case VK_END:	
			OnVScroll(0, MAKEWPARAM(SB_THUMBTRACK, (DWORD)m_Data.size()), NULL, bHandled); 
			break;
		
		case VK_LEFT:
		case VK_RIGHT:
			{
				if( m_pSink)
					((CHexViewSink*)m_pSink)->OnNextPrev( wParam == VK_RIGHT ? true : false);
			}
			break;
	}

	return bHandled = TRUE, 0;
}

LRESULT CHexViewCtrl::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch( LOWORD(wParam) )
	{
		case SB_LINEDOWN: 
			TBase::SetTopIndex(m_topindex + m_BytesPerRow);
			break;
		
		case SB_LINEUP:
			TBase::SetTopIndex(m_topindex - m_BytesPerRow);
			break;
		
		case SB_PAGEDOWN:
			TBase::SetTopIndex(m_topindex + m_BytesPerRow * m_LinesPerPage);
			break;

		case SB_PAGEUP:
			TBase::SetTopIndex(m_topindex - m_BytesPerRow * m_LinesPerPage);
			break;

		case SB_THUMBTRACK: 
			TBase::SetTopIndex(HIWORD(wParam) * m_BytesPerRow);
			break;
	}
	
	return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT CHexViewCtrl::OnMouseWhell(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	short iDist = (short)HIWORD(wParam);
	if( iDist < 0 )	TBase::SetTopIndex(m_topindex + m_BytesPerRow);
	else			TBase::SetTopIndex(m_topindex - m_BytesPerRow);

	return bHandled = FALSE, TRUE;
}

LRESULT CHexViewCtrl::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetFocus();

	if( m_pSink) {
		POINT point = {	GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		((CHexViewSink*)m_pSink)->OnPosSelected(GetDataPos(point));
	}

	return bHandled = FALSE, TRUE;
}

LRESULT CHexViewCtrl::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetFocus();
	return bHandled = FALSE, TRUE;
}

LRESULT CHexViewCtrl::OnDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return DLGC_WANTALLKEYS|DLGC_WANTTAB;
}

LRESULT CHexViewCtrl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    TBase::OnFocus(true);
	return S_OK;
}

LRESULT CHexViewCtrl::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    TBase::OnFocus(false);
	return S_OK;
}

LRESULT CHexViewCtrl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	if( m_Data.size() > 0 && pt.x >= m_offHex )
		SetCursor(g_curHandId);
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	return bHandled = FALSE, 0;
}

void CHexViewCtrl::UpdateScrollBarPos()
{
	if( m_BytesPerRow > 0 )
		::SetScrollPos(m_hWnd, SB_VERT, m_topindex / m_BytesPerRow, TRUE);
}

void CHexViewCtrl::UpdateScrollBars()
{
	if( m_BytesPerRow == 0 )
		return;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);

	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = ((int)m_Data.size()) / m_BytesPerRow;
	si.nPage = m_LinesPerPage;
	si.nPos = m_topindex / m_BytesPerRow;

	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	if(si.nMax > (int)si.nPage)		::EnableScrollBar(m_hWnd, SB_VERT, ESB_ENABLE_BOTH);
	else							m_topindex = 0;
}


//////////////////////////////////////////////////////////////////////

CItemBase* CWinRoot::CreateItem(LPCSTR strId, LPCSTR strType)
{
	sswitch(strType)
	scase(GUI_ITEMT_BUTTON)
		if( GUI_ISSTYLE_SIMPLE(this) )
			return new CButton();
		return new CXPButton();
		sbreak;
	
	scase(GUI_ITEMT_GROUP)
		if( GUI_ISSTYLE_SIMPLE(this) )
			return new CGroupBox();
		return new CGroup();
		sbreak;

	scase(GUI_ITEMT_BOX)                   return new CGroup();                 sbreak;
	scase(GUI_ITEMT_BITMAPBUTTON)          return new CBitmapButton();          sbreak;
	scase(GUI_ITEMT_LOCKBUTTON)            return new CLockButton();            sbreak;
	scase(GUI_ITEMT_CHECK)                 return new CCheck();                 sbreak;
	scase(GUI_ITEMT_RADIO)                 return new CRadio();                 sbreak;
	scase(GUI_ITEMT_HOTLINK)               return new CHotlink();               sbreak;
	scase(GUI_ITEMT_PROGRESS)              return new CProgress();              sbreak;
	scase(GUI_ITEMT_COMBO)                 return new CCombo();                 sbreak;
	scase(GUI_ITEMT_TAB)                   return new CTab();                   sbreak;
	scase(GUI_ITEMT_LISTCTRL)              return new CGridCtrlImpl();          sbreak;
	scase(GUI_ITEMT_TREE)                  return new CTreeImpl();              sbreak;
	scase(GUI_ITEMT_PROPSHEETPAGE)         return new CPropertySheetPage();     sbreak;
	scase(GUI_ITEMT_EDIT)                  return new CEdit();                  sbreak;
	scase(GUI_ITEMT_TIMEEDIT)              return new CTimeEditBox();           sbreak;
	scase(GUI_ITEMT_RICHEDIT)              return new CRichEditImpl();          sbreak;
	scase(GUI_ITEMT_SLIDER)                return new CSlider();                sbreak;
	scase(GUI_ITEMT_IPADDRESS)             return new CIPAddressCtrl();         sbreak;
	scase(GUI_ITEMT_HTML)                  return new CWinHTMLCtrl();           sbreak;
	
	scase(GUI_ITEMT_DIALOG)                return new CDialogItemImpl();        sbreak;

	scase(GUI_ITEMT_MENU)                  return new CPopupMenu();             sbreak;
	scase(GUI_ITEMT_TRAY)                  return new CTrayIcon();              sbreak;

	scase(GUI_ITEMT_SLIDESHOW)             return new CSlideShowCtrl();         sbreak;
	scase(GUI_ITEMT_CREDITS)               return new CWinCreditsCtrl();        sbreak;
	scase(GUI_ITEMT_CHART)                 return new CChartCtrl();             sbreak;
	scase(GUI_ITEMT_SPLITTER)              return new CSplitCtrl();             sbreak;
	scase(GUI_ITEMT_HEXVIEWER)             return new CHexViewCtrl();           sbreak;
	send;

	return CRootItem::CreateItem(strId, strType);
}



//////////////////////////////////////////////////////////////////////
