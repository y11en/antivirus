// WinCtrls.h: interface for the CWinCtrls class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINCTRLS_H__5401AE0F_38DD_4399_883A_F072CC926781__INCLUDED_)
#define AFX_WINCTRLS_H__5401AE0F_38DD_4399_883A_F072CC926781__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <richedit.h>
#include <ExDispid.h>
#include "WinLang.h"

//////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;
extern HCURSOR   g_curHandId;
extern HCURSOR   g_curTrackCol;

void SetNearestFocusItem(CItemBase * pItem);
bool IsKeybBtnDownReal(bool bRadio = false);
bool IsLeftMouseDownReal();

//////////////////////////////////////////////////////////////////////

#define DECLARE_WND_AVPCLASS_EX(WndClassName, OrigWndClassName, Cursor, style) \
	virtual CWndClassInfo& GetWndClassInfo() \
	{ \
		static CWndClassInfo wc = \
		{ \
			{ sizeof(WNDCLASSEX), style, StartWindowProc, \
			  0, 0, NULL, NULL, NULL, NULL, NULL, WndClassName, NULL }, \
			OrigWndClassName, NULL, Cursor, TRUE, 0, _T("") \
		}; \
		return wc; \
	}

#define DECLARE_WND_AVPCLASS(WndClassName, OrigWndClassName, Cursor) \
	DECLARE_WND_AVPCLASS_EX(WndClassName, OrigWndClassName, Cursor, 0)

#define DECLARE_DLG_AVPCLASS(WndClassName) \
	virtual CWndClassInfo& GetWndClassInfo() \
	{ \
		static CWndClassInfo wc = \
		{ \
			{ sizeof(WNDCLASSEX), 0, NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, WndClassName, NULL }, \
			NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
		}; \
		return wc; \
	}

#define AVP_REFLECT_NOTIFICATIONS() \
	if( !(uMsg == WM_NOTIFY && \
		(((NMHDR *)lParam)->code == TTN_GETDISPINFOA || \
		((NMHDR *)lParam)->code == TTN_GETDISPINFOW)) ) \
		REFLECT_NOTIFICATIONS()

//////////////////////////////////////////////////////////////////////
// CItemWindowRtn

class CItemWindowRtn
{
public:
	virtual void wrtn_OnUpdateTextProp() = 0;
	virtual void wrtn_OnAdjustBordSize(RECT& rc) = 0;

public:
	CItemWindowRtn();
	
	virtual CItemBase * _item() = 0;
	virtual HWND _hwnd() = 0;

	HBRUSH   GetParentBackgroundBrush();
	COLORREF GetParentBackgroundColor();
	HWND     GetParentWindow();
	void     UpdateWindowProps(int flags, tDWORD nStateMask = 0);
	void     DrawParentContent(HDC dc, RECT *rc = NULL);

protected:
	bool     _MsgMenu(POINT& ptMenu);
	void     _MsgMouseMove(POINT& pt);
	void     _MsgPaint();

	void     StartTrackMouseLeave();

protected:
	DWORD    m_nStyles;
	DWORD    m_nStylesEx;
	
	bool     m_bTrackMouseEvent;
};

//////////////////////////////////////////////////////////////////////
// CItemWindowT

template <class TWinImpl, class TItemBase = CItemBase>
class ATL_NO_VTABLE CItemWindowT : public TWinImpl, public TItemBase, public CItemWindowRtn
{
public:
	typedef TWinImpl  WinImplClass;
	typedef TItemBase GuiImplClass;

	CItemBase * _item() { return this; }
	HWND        _hwnd() { return m_hWnd; }
	void        wrtn_OnUpdateTextProp()   { OnUpdateTextProp(); }
	void        wrtn_OnAdjustBordSize(RECT& rc) { OnAdjustBordSize(rc); }

	CItemWindowT() { m_nFlags |= STYLE_WINDOW; }

	void Cleanup()
	{
		if( IsWindow() ) DestroyWindow();
		TItemBase::Cleanup();
	}

	HWND     GetWindow()                { return m_hWnd; }
	HBRUSH   GetParentBackgroundBrush() { return CItemWindowRtn::GetParentBackgroundBrush(); }
	COLORREF GetParentBackgroundColor() { return CItemWindowRtn::GetParentBackgroundColor(); }
	HWND     GetParentWindow()          { return CItemWindowRtn::GetParentWindow(); }

	void InitItem(LPCSTR strType)
	{
		TItemBase::InitItem(strType);
		RECT rcFrame = {0, 0, 10, 10};
		TWinImpl::Create(m_pRoot, GetParentWindow(), GetWndClassInfo(), rcFrame, m_nStyles, m_nStylesEx, GetItemId());
	}

	void ApplyStyle(LPCSTR strStyle)
	{
		TItemBase::ApplyStyle(strStyle);
		sswitch(strStyle)
		scase(STR_PID_NOBORDER) m_nStylesEx &= ~WS_EX_CLIENTEDGE; sbreak;
		send
	}


	void OnUpdateTextProp()
	{
		TWinImpl::SetWindowTextW(m_pRoot, m_pRoot->LocalizeStr(TOR(cStrObj,()), OnGetDisplayText()));
		TItemBase::OnUpdateTextProp();
	}
	
	void OnUpdateProps(int flags = UPDATE_FLAG_ALL, tDWORD nStateMask = 0, RECT * rc = NULL)
	{
		UpdateWindowProps(flags, nStateMask);
		TItemBase::OnUpdateProps(flags, nStateMask, rc);
	}

	void UpdateWindowProps(int flags = UPDATE_FLAG_ALL, tDWORD nStateMask = 0)
	{
		CItemWindowRtn::UpdateWindowProps(flags, nStateMask);
	}

	void Focus(bool bFocus)
	{
		if( bFocus )
			SetFocus();
	}

	void InvalidateEx(RECT *rc)
	{
		InvalidateRect(rc);
	}

	void Update(RECT *rc)
	{
		TItemBase::Update(rc);
		if( m_hWnd && IsWindowVisible() )
		{
			InvalidateRect(rc);
			if( m_lockUpdate )
				m_nState |= ISTATE_NEED_UPDATE;
			else
				UpdateWindow();
		}
	}

	void DrawBackground(HDC dc, RECT& rcClient, RECT * prcClip)
	{
		if( m_pBackground )
			TItemBase::DrawBackground(dc, rcClient, prcClip);
		else
		{
			m_nState |= ISTATE_DRAW_CONTENT;
			TWinImpl::SendMessage(WM_ERASEBKGND, (WPARAM)dc, 0);
			m_nState &= ~ISTATE_DRAW_CONTENT;
		}
	}

	void SetTabOrder(CItemBase * pItem, bool bAfter)
	{
		HWND hAfter = pItem ? pItem->GetWindow() : NULL;
		if( !bAfter && hAfter )
			hAfter = ::GetWindow(hAfter, GW_HWNDPREV);
		SetWindowPos(hAfter, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOSIZE);
	}
	
	bool IsTabstop()
	{
		return !!(m_nStyles & WS_TABSTOP);
	}

	virtual CWndClassInfo& GetWndClassInfo(){ return *(CWndClassInfo*)NULL; }

protected:
	bool ctl_IsFocused() { return ::GetFocus() == m_hWnd; }
	void ctl_GetText(cStrObj& strText) { TWinImpl::GetWindowTextW(m_pRoot, strText); }
	void ctl_SetText(LPCWSTR strText) { TWinImpl::SetWindowTextW(m_pRoot, strText); }

protected:
	BEGIN_MSG_MAP(CItemWindowT)
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			((CWinRoot *)m_pRoot)->RelayTipEvent(m_pCurrentMsg);
			break;
		}
		
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONUP:
		case WM_NCMBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_KILLFOCUS:
			((CWinRoot *)m_pRoot)->RelayBalloonEvent(m_pCurrentMsg);
			break;
		}


		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMsgMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMsgMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMsgMouseActivate)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnMsgMenu)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKEYDOWN)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_QUERYENDSESSION, OnQueryEndSession)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)

		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnQueryEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::CItemWindowT(%s)::WM_QUERYENDSESSION(wParam=0x%X, lParam=0x%X)", m_strItemId.c_str(), wParam, lParam));
		return TRUE;
	}

	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		
		CItemBase * pSwitcer = m_pRoot->m_pTrackItem ? m_pRoot->m_pTrackItem->GetByFlagsAndState(STYLE_SHEET_ITEM|STYLE_ENUM, ISTATE_ALL, NULL, gbfasUpToParent) : NULL;
		if( !pSwitcer || (pSwitcer->m_nFlags & STYLE_TOP_ITEM) )
			return bHandled = FALSE, 0;

		if( zDelta != 0 )
			pSwitcer->SetNextPage(zDelta > 0);
		return 0;
	}
	
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMEASUREITEMSTRUCT lpInfo = (LPMEASUREITEMSTRUCT)lParam;
		if( lpInfo->CtlType == ODT_MENU )
			return CPopupMenu::MeasureItem(lpInfo);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPDRAWITEMSTRUCT lpInfo = (LPDRAWITEMSTRUCT)lParam;
		if( lpInfo->CtlType == ODT_MENU )
			return CPopupMenu::DrawItem(lpInfo);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnKEYDOWN(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = !!m_lockUpdate;
		return 0;
	}
	
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_MsgPaint();
		return S_OK;
	}

	LRESULT OnPaintEx(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( m_pOwner ) m_pOwner->m_nState |= ISTATE_DRAW_CONTENT;
		LRESULT nResult = DefWindowProc(uMsg, wParam, lParam);
		if( m_pOwner ) m_pOwner->m_nState &= ~ISTATE_DRAW_CONTENT;
		return nResult;
	}

	LRESULT OnPaintEmpty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(&ps);
		EndPaint(&ps);
		return S_OK;
	}

	LRESULT OnHookDraw(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		HookDrawBegin(/*(HBRUSH)::GetStockObject(WHITE_BRUSH)*//*GetParentBackgroundBrush()*/this);
		LRESULT bRet = DefWindowProc(uMsg, wParam, lParam);
		HookDrawEnd();
		return bRet;
	}

	LRESULT	OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( m_nState & ISTATE_DRAW_CONTENT )
			return bHandled = FALSE, FALSE;
		return TRUE;
	}

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		OnFocus(true);
		bHandled = FALSE;
		return S_OK;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		OnFocus(false);
		bHandled = FALSE;
		return S_OK;
	}

	LRESULT OnCtlBkColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return ( LRESULT )GetParentBackgroundBrush();
	}

	LRESULT OnCtlBkColorT( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		::SetBkMode((HDC)wParam, TRANSPARENT);
		if( m_pFont )
			::SetTextColor((HDC)wParam, m_pFont->GetColor());

		return ( LRESULT )GetParentBackgroundBrush();
	}

	LRESULT OnMsgMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT ptMenu = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if( !_MsgMenu(ptMenu) )
			bHandled = FALSE;
		return 0;
	}

	LRESULT OnMsgMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		_MsgMouseMove(pt);
		return 0;
	}
	
	LRESULT OnMsgMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		m_bTrackMouseEvent = false;
		m_pRoot->SetTrackItem(NULL);
		return bHandled = FALSE, 0;
	}

	LRESULT OnMsgMouseActivate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		if( m_nFlags & STYLE_NOACTIVATE )
			return MA_NOACTIVATE;
		return bHandled = FALSE, MA_ACTIVATE;
	}
	
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CItemBase *pTrackItem = m_pRoot->m_pTrackItem;
		if( pTrackItem && (pTrackItem->m_nState & ISTATE_DISABLED) )
			bHandled = FALSE;
		else if( pTrackItem && (pTrackItem->m_nFlags & STYLE_CLICKABLE) )
			SetCursor(g_curHandId);
		else
			bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CItemBase * pTrackItem = m_pRoot->m_pTrackItem;
 		m_pRoot->m_pClicking = pTrackItem;
		if( pTrackItem && (pTrackItem->m_nFlags & STYLE_CLICKABLE) )
		{
			if( pTrackItem->m_nFlags & STYLE_BUTTON_LIKE )
			{
				m_pRoot->m_pSelected = pTrackItem;
				pTrackItem->Select(true);
			}
			else if( pTrackItem->m_nFlags & STYLE_CHECKBOX_LIKE )
			{
				if( pTrackItem->m_nState & ISTATE_SELECTED )
				{
					m_pRoot->m_pSelected = NULL;
					pTrackItem->Select(false);
				}
				else
				{
					m_pRoot->m_pSelected = pTrackItem;
					pTrackItem->Select(true);
				}
			}
			else if( pTrackItem->m_nFlags & STYLE_ENUM_VALUE )
				pTrackItem->Select();
		}
		
		return bHandled = FALSE, 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( CItemBase *&pSelected = m_pRoot->m_pSelected )
			if( pSelected->m_nFlags & STYLE_BUTTON_LIKE )
				pSelected->Select(false), pSelected = NULL;
		
		if( CItemBase * pTrackItem = m_pRoot->m_pTrackItem )
			if( m_pRoot->m_pClicking == pTrackItem && pTrackItem->m_pRoot && pTrackItem->m_nFlags & STYLE_CLICKABLE )
				pTrackItem->Click();

		m_pRoot->m_pClicking = NULL;

		return bHandled = FALSE, 0;
	}

	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CItemBase *pTrackItem = m_pRoot->m_pTrackItem;
		if( pTrackItem && (pTrackItem->m_nFlags & STYLE_CLICKABLE) )
			SendMessage(WM_LBUTTONDOWN, wParam, lParam);

		CItemBase * pItem = pTrackItem ? pTrackItem : this;
		pItem->DblClick();
		return bHandled = FALSE, 0;
	}

	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		NMHDR * pHdr = (NMHDR *)lParam;
		switch( pHdr->code )
		{
		case TTN_SHOW:
		case TTN_GETDISPINFOW:
		case TTN_GETDISPINFOA:
			bHandled = TRUE;
			return ((CWinRoot *)m_pRoot)->ProcessTipEvent(pHdr, this);
		}
		
		return bHandled = FALSE, 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( !m_strTipText.empty() )
		{
			m_pRoot->EnableToolTip(this, false);
			m_strTipText.erase();
		}
		
		return bHandled = FALSE, 0;
	}
};

//////////////////////////////////////////////////////////////////////
// CDialogBase

class ATL_NO_VTABLE CDialogBase : public CDialogImplBaseT<CWindowL>
{
public:
	HWND Create(CRootItem * pRoot, HWND hWndParent, CWndClassInfo& pCI, RECT& rcPos, DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0);
};

template <class TWindow = CWindowL>
class ATL_NO_VTABLE CControlBaseT : public CWindowImplBaseT<TWindow>
{
public:
	HWND Create(CRootItem * pRoot, HWND hWndParent, CWndClassInfo& pCI, RECT& rcPos, DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0)
	{
		if( !(m_bAnsi ? RegisterWindowA(&pCI) : RegisterWindowW(&pCI)) )
			return NULL;

		if( pCI.m_lpszOrigName )
			m_pfnSuperWindowProc = pCI.pWndProc;
		else
			m_pfnSuperWindowProc = m_bAnsi ? ::DefWindowProcA : ::DefWindowProcW;

		_Module.AddCreateWndData(&m_thunk.cd, this);
		m_hWnd = CreateWnd(dwExStyle, (LPVOID)pCI.m_wc.lpszClassName, dwStyle, rcPos, hWndParent, nID);
		return m_hWnd;
	}

public:
	static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CControlBaseT* pThis = (CControlBaseT*)_Module.ExtractCreateWndData();
		if( !pThis && uMsg == WM_NCCREATE )
		{
			WNDPROC pProc;
			if( m_bAnsi )
			{
				CREATESTRUCTA &cs = *(CREATESTRUCTA *)lParam;
				WNDCLASSEXA wce; wce.cbSize = sizeof(wce);
				::GetClassInfoExA(NULL, cs.lpszClass, &wce);
				pProc = wce.lpfnWndProc;
				::SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG)pProc);
			}
			else
			{
				CREATESTRUCTW &cs = *(CREATESTRUCTW *)lParam;
				WNDCLASSEXW wce; wce.cbSize = sizeof(wce);
				GetClassInfoExW(NULL, cs.lpszClass, &wce);
				pProc = wce.lpfnWndProc;
				::SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG)pProc);
			}
			return pProc(hWnd, uMsg, wParam, lParam);
		}
		pThis->m_hWnd = hWnd;
		pThis->m_thunk.Init(WindowProc, pThis);
		DWORD dwOldProtect;
#if _ATL_VER >= 0x0800
		::VirtualProtect(pThis->m_thunk.thunk.pThunk, sizeof(*pThis->m_thunk.thunk.pThunk), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		WNDPROC pProc = (WNDPROC)(pThis->m_thunk.thunk.pThunk);
#else
		::VirtualProtect(&pThis->m_thunk.thunk, sizeof(pThis->m_thunk.thunk), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		WNDPROC pProc = (WNDPROC)&(pThis->m_thunk.thunk);
#endif
		if( m_bAnsi )
			(WNDPROC)::SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG)pProc);
		else
			(WNDPROC)::SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG)pProc);
		return pProc(hWnd, uMsg, wParam, lParam);
	}

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CControlBaseT* pThis = (CControlBaseT*)hWnd;

#if (_ATL_VER >= 0x0700)
		POINT pnt = { 0, 0 };
		ATL::_ATL_MSG msg( pThis->m_hWnd, uMsg, wParam, lParam, 0, pnt, TRUE );
		const ATL::_ATL_MSG* pOldMsg = pThis->m_pCurrentMsg;
#else
		MSG msg = { pThis->m_hWnd, uMsg, wParam, lParam, 0, { 0, 0 } };
		const MSG* pOldMsg = pThis->m_pCurrentMsg;
#endif

		LRESULT lRes;

		pThis->m_pCurrentMsg = &msg;
		BOOL bRet = pThis->ProcessWindowMessage(pThis->m_hWnd, uMsg, wParam, lParam, lRes, 0);
		pThis->m_pCurrentMsg = pOldMsg;

		// do the default processing if message was not handled
		if( bRet )
			return lRes;

		if( pThis->m_bAnsi )
			lRes = ::CallWindowProcA(pThis->m_pfnSuperWindowProc, pThis->m_hWnd, uMsg, wParam, lParam);
		else
			lRes = ::CallWindowProcW(pThis->m_pfnSuperWindowProc, pThis->m_hWnd, uMsg, wParam, lParam);

		if( uMsg == WM_NCDESTROY )
		{
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage(hWnd);
		}

		return lRes;
	}
};

typedef CControlBaseT<> CControlBase;

template <class TItemBase = CItemBase>
class ATL_NO_VTABLE CItemControl : public CItemWindowT<CControlBase, TItemBase>
{
};

//////////////////////////////////////////////////////////////////////
// CButtonImpl

template <class TItemBase = CItemBase>
class CButtonImpl : public CItemControl<TItemBase>
{
public:
	typedef CItemControl<TItemBase> TBase;
	DECLARE_WND_AVPCLASS_EX("AVP.Button", "BUTTON", NULL, CS_PARENTDC|CS_HREDRAW|CS_VREDRAW);

	CButtonImpl() : m_bRealLButtonDown(0) {}

protected:
	BEGIN_MSG_MAP(CButtonImpl)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(OCM_COMMAND, OnCommand)
		MESSAGE_HANDLER(OCM_CTLCOLORBTN, OnCtlBkColorT)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlBkColorT)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bRealLButtonDown = IsLeftMouseDownReal();
		return bHandled = FALSE, 0;
	}
	
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return DefWindowProc(uMsg, wParam, lParam);
	}

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( HIWORD(wParam) == BN_CLICKED &&
			::IsWindowEnabled(m_hWnd) &&
			::IsWindowVisible(m_hWnd) &&
			(m_bRealLButtonDown || IsKeybBtnDownReal(!!(m_nFlags & STYLE_ENUM_VALUE))) )
		{
			m_pRoot->m_pClicking = NULL;
			Click();
		}
		
		m_bRealLButtonDown = 0;
		return bHandled = FALSE, 0;
	}

	bool IsResizeble(bool bHorz){ return false; }

	bool DrawParentBackgroundT(HDC dc, RECT * rcDraw = NULL)
	{
		if( GUI_ISSTYLE_SIMPLE(m_pRoot) && m_pRoot->IsThemed() && ((CWinRoot *)m_pRoot)->_DrawThemeParentBackground )
			if( !((CWinRoot *)m_pRoot)->_DrawThemeParentBackground(m_hWnd, dc, rcDraw) )
				return true;
		
		return TBase::DrawParentBackground(dc, rcDraw);
	}

	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
	{
		if( (nStateMask & ISTATE_HIDE) && !(m_nState & ISTATE_HIDE) )
			SetWindowTextW(m_pRoot, m_pRoot->LocalizeStr(TOR(cStrObj,()), OnGetDisplayText()));
		
		TBase::OnUpdateProps(flags, nStateMask, rc);

		if( (nStateMask & ISTATE_HIDE) && (m_nState & ISTATE_HIDE) )
			SetWindowTextW(m_pRoot, L"");

		if( !(m_nStyles & BS_OWNERDRAW) && (nStateMask & ISTATE_DEFAULT) )
		{
			bool bOn = !!(m_nState & ISTATE_DEFAULT);
			
			DWORD nStyle = GetWindowLong(GWL_STYLE);
			DWORD nNewStyle = bOn ? (nStyle|BS_DEFPUSHBUTTON) : (nStyle & ~BS_DEFPUSHBUTTON);
			SendMessage(BM_SETSTYLE, nNewStyle, TRUE);
		}
	}

	unsigned m_bRealLButtonDown : 1;
};

//////////////////////////////////////////////////////////////////////
// CButton

class CButton : public CButtonImpl<>
{
public:
	typedef CButtonImpl<> TBase;

	BEGIN_MSG_MAP(CButton)
		MESSAGE_HANDLER(WM_PAINT, OnPaintEx)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	CButton()
	{
		m_nStyles = WS_CHILD|BS_PUSHBUTTON|WS_TABSTOP;
		m_nFlags |= STYLE_CLICKABLE|TEXT_SINGLELINE;
	}

	void GetAddSize(SIZE& szAdd){ szAdd.cx += 16; }
};

//////////////////////////////////////////////////////////////////////
// CBitmapButton

class CBitmapButton : public CButtonImpl<>
{
public:
	typedef CButtonImpl<> TBase;

	CBitmapButton()
	{
		m_nStyles = WS_CHILD|BS_PUSHBUTTON|WS_TABSTOP|BS_OWNERDRAW;
		m_nFlags |= STYLE_CLICKABLE|STYLE_TRANSPARENT;
	}

	BEGIN_MSG_MAP(CBitmapButton)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	void GetAddSize(SIZE& szAdd){}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DRAWITEMSTRUCT * pDi = (DRAWITEMSTRUCT *)lParam;
		
		if( pDi->itemState & ODS_SELECTED )
			m_nState |= ISTATE_SELECTED;
		else
			m_nState &= ~ISTATE_SELECTED;

		if( pDi->itemState & ODS_FOCUS )
			m_nState |= ISTATE_FOCUSED;
		else
			m_nState &= ~ISTATE_FOCUSED;

		DrawButton(pDi->hDC, pDi->rcItem);
		bHandled = false;
		return 0;
	}

	virtual void DrawButton(HDC dc, RECT& rcDraw)
	{
		if( !m_pBackground )
			return;
		
		HDC hDcTemp = NULL;
		if( m_pBackground->IsTransparent() )
		{
			//if( ((CWinRoot *)m_pRoot)->m_nOsVer < GUI_OSVER_WINXP )
				hDcTemp = m_pRoot->GetTempDC(rcDraw, NULL);
			DrawParentBackground(hDcTemp ? hDcTemp : dc, &rcDraw);
		}

		m_pBackground->Draw(hDcTemp ? hDcTemp : dc, rcDraw, NULL, m_pBackground.GetImage(GetStateImage()));
		
		if( hDcTemp )
			m_pRoot->CopyTempDC(hDcTemp, dc, rcDraw);

		if( m_nState & ISTATE_FOCUSED )
		{
			RECT rcFrame = rcDraw;
			InflateRect(&rcFrame, -3, -3);
			DrawFocusRect(dc, &rcFrame);
		}
	}
};

//////////////////////////////////////////////////////////////////////
// CXPButton

class CXPButton : public CBitmapButton, public CXPBrush
{
public:
	CXPButton(){ m_nStyles |= WS_TABSTOP; }
	void OnInit();
	void DrawButton(HDC dc, RECT& rcDraw);

	void GetAddSize(SIZE& szAdd){ szAdd.cx += 16; }
};

//////////////////////////////////////////////////////////////////////
// CItemWndFocusT

template <class WndImplT, class TItemBase = CItemBase>
class CItemWndFocusT : public TItemBase
{
public:
	typedef CItemWndFocusT<WndImplT, TItemBase> ThisT;
	
	class FocusWnd : public WndImplT
	{
	public:
		bool Create(HWND hWndParent, UINT nID = 0)
		{
			RECT rcFrame = {-100, -100, 0, 0};
			if( !WndImplT::Create(m_pRoot, hWndParent, GetWndClassInfo(), rcFrame, m_nStyles|WS_VISIBLE, m_nStylesEx, nID) )
				return false;

			OnUpdateProps(0, ISTATE_HIDE, NULL);
			return true;
		}

		void SetStyle(tDWORD nStyles) { m_nStyles = nStyles; }
		
		virtual void OnFocus(bool bFocusted) { OWNEROFMEMBER_PTR(ThisT, m_focus)->Focus(bFocusted); }
		virtual void Click(bool bForce) { OWNEROFMEMBER_PTR(ThisT, m_focus)->Click(bForce); }

		void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
		{
			WndImplT::OnUpdateProps(flags & ~(UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE), nStateMask, rc);
		}
	}; friend class FocusWnd;
	
	FocusWnd m_focus;
	
	void Cleanup()
	{
		if( m_focus.m_pRoot )
		{
			m_focus.m_pParent = NULL;
			m_focus.Cleanup();
			m_focus.m_pRoot = NULL;
		}
		
		TItemBase::Cleanup();
	}
	
	bool IsTabstop()                                 { return !!(m_nFlags & STYLE_CLICKABLE); }
	void SetTabOrder(CItemBase * pItem, bool bAfter) { if( m_focus.GetWindow() ) m_focus.SetTabOrder(pItem, bAfter); }
	HWND GetWindow()                                 { return m_focus.GetWindow(); }

	void InitItem(LPCSTR strType)
	{
		TItemBase::InitItem(strType);
		if( IsTabstop() && m_pOwner )
		{
			m_focus.m_pRoot = m_pRoot;
			m_focus.m_pOwner = m_pOwner;
			m_focus.m_pParent = m_pParent;
			m_focus.Create(m_pOwner->GetWindow(), GetItemId());
		}
	}

	void OnInited()
	{
		TItemBase::OnInited();
		m_focus.m_nState |= ISTATE_INITED;
	}

	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
	{
		TItemBase::OnUpdateProps(flags, nStateMask, rc);

		if( !m_focus.m_hWnd )
			return;
		
		if( nStateMask & ISTATE_DISABLED ) m_focus.Enable(!(m_nState & ISTATE_DISABLED));
		if( nStateMask & ISTATE_HIDE )     m_focus.Show(!(m_nState & ISTATE_HIDE));
	}

	void OnFocus(bool bFocusted)  { TItemBase::OnFocus(bFocusted); if( m_focus.m_hWnd ) m_focus.Focus(bFocusted); }
};

//////////////////////////////////////////////////////////////////////
// CHotlink

class CHotlink : public CItemWndFocusT<CButtonImpl<> >
{
public:
	typedef CItemWndFocusT<CButtonImpl<> > TBase;
	CHotlink()
	{
		m_focus.SetStyle(WS_CHILD|WS_TABSTOP|BS_GROUPBOX);
		m_nFlags |= STYLE_CLICKABLE;
	}

protected:
	void OnInit();
	void DrawItem(HDC dc, RECT& rcFrame);
	void OnFocus(bool bFocusted);
	void OnAdjustUpdateRect(RECT& rcUpdate);
};

//////////////////////////////////////////////////////////////////////
// CGroup

class CGroup : public CItemWndFocusT<CButtonImpl<CGroupItem>, CGroupItem>
{
public:
	CGroup()
	{
		m_focus.SetStyle(WS_CHILD|WS_TABSTOP|BS_GROUPBOX);
	}
};

//////////////////////////////////////////////////////////////////////
// CChkRadLink

class CChkRadLink : public CBitmapButton
{
public:
	CChkRadLink();
	
	void DrawButton(HDC dc, RECT& rcDraw);
};

//////////////////////////////////////////////////////////////////////
// CChkRadBase

class CChkRadBase : public CButtonImpl<CRefsItem>
{
public:
	typedef CButtonImpl<CRefsItem>      TBase;
	typedef CRefsItemLinkT<CChkRadLink> TRef;

	CChkRadBase() : m_pLinkFont(NULL)
	{
		m_nStyles = WS_CHILD|WS_TABSTOP|BS_TOP|BS_LEFT|BS_MULTILINE;
		m_nFlags |= STYLE_CLICKABLE|STYLE_DRAW_CONTENT|STYLE_TRANSPARENT;
	}

	~CChkRadBase() { if( m_pLinkFont ) m_pLinkFont->Destroy(); }

	RefData * _RefCreate() { return new TRef(); }

public:
	BEGIN_MSG_MAP(CChkRadBase)
		AVP_REFLECT_NOTIFICATIONS()
		MESSAGE_HANDLER(WM_PAINT,         OnPaint)
		MESSAGE_HANDLER(WM_UPDATEUISTATE, OnUpdateUIState)
		MESSAGE_HANDLER(WM_ENABLE,        OnUpdateUIState)
		MESSAGE_HANDLER(BM_SETSTATE,      OnUpdateUIState)
		MESSAGE_HANDLER(BM_SETCHECK,      OnUpdateUIState)
		MESSAGE_HANDLER(WM_SETFOCUS,      OnSetFocus)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnUpdateUIState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	bool   Draw(HDC dc, RECT* rcUpdate);
	void   DrawItem(HDC dc, RECT& rcFrame);
	bool   DrawParentBackground(HDC dc, RECT * rcDraw);
	tDWORD GetFontFlags() { return TBase::GetFontFlags() & ~TEXT_HOTLIGHT; }
	void   GetAddSize(SIZE& szAdd);
	bool   IsOut(){ return true; }

	bool   OnClicked(CItemBase* pItem);
	void   OnAdjustClientRect(RECT& rcClient);
	void   OnFocus(bool bFocusted);
	void   OnApplyBlockStyles();

public:
	CFontStyle * m_pLinkFont;
};

//////////////////////////////////////////////////////////////////////
// CCheck

class CCheck : public CChkRadBase
{
public:
	CCheck();

	void     DrawContent(HDC dc, RECT& rcClient);

	bool     SetValue(const cVariant &pValue);
	bool     GetValue(cVariant &pValue);
	tTYPE_ID GetType(){ return tid_DWORD; }
	void     InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
};

//////////////////////////////////////////////////////////////////////
// CRadio

class CRadio : public CChkRadBase
{
public:
	CRadio();

	void     InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void     DrawContent(HDC dc, RECT& rcClient);
	void     OnInit();
	void     OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);

	tTYPE_ID GetType(){ return tid_UNK; }
	bool     SetValue(const cVariant& pValue) { return m_Val = pValue, true; }
	bool     GetValue(cVariant& pValue) { return pValue = m_Val, true; }

	cVariant m_Val;
};

//////////////////////////////////////////////////////////////////////
// CLockButton

class CLockButton : public CButtonImpl<>
{
public:
	typedef CButtonImpl<> TBase;
	
	BEGIN_MSG_MAP(CButtonImpl<>)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()
	
	CLockButton() : m_bLocked(false), m_bMandatoried(false)
	{
		m_nStyles = WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON|BS_OWNERDRAW;
		m_nFlags |= STYLE_CLICKABLE|TEXT_SINGLELINE;
	}
	
	void     GetAddSize(SIZE& szAdd){}
	bool     SetValue(const cVariant &pValue);
	bool     GetValue(cVariant &pValue);
	bool     IsOut(){ return true; }

	bool     OnClicked(CItemBase *);
	LRESULT  OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void     SetLock(bool bLock);
	void     SetMandatory(bool bMandatory);

protected:
	bool          m_bLocked;
	bool          m_bMandatoried;
};

//////////////////////////////////////////////////////////////////////
// CGroupBox

class CGroupBox : public CItemControl<CGroupItem>
{
public:
	typedef CItemControl<CGroupItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.GroupBox", "BUTTON", NULL);

	BEGIN_MSG_MAP(CGroupBox)
		MESSAGE_HANDLER(OCM_CTLCOLORBTN, OnBkColor)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnBkColor)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaintEmpty)
		MESSAGE_HANDLER(WM_ENABLE, OnUpdateUIState)
		
		MESSAGE_HANDLER(OCM_COMMAND, OnCommand)
		
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnBkColor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUpdateUIState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	CGroupBox();

	void InitItem(LPCSTR strType);
	void DrawItem(HDC dc, RECT& rcFrame);
};

//////////////////////////////////////////////////////////////////////
// CProgress

class CProgress : public CItemControl<>
{
public:
	typedef CItemControl<> TBase;
	DECLARE_WND_AVPCLASS("AVP.Progress", PROGRESS_CLASS, NULL);
	CProgress()
	{
		m_nStyles = WS_CHILD;
	}

protected:
	bool SetValue(const cVariant &pValue)
	{
		tDWORD nValue = pValue.ToDWORD();
		::SendMessage(m_hWnd, PBM_SETPOS, nValue == -1 ? 0 : nValue, 0);
		return true;
	}
	bool     GetValue(cVariant &pValue) { pValue = (tDWORD)::SendMessage(m_hWnd, PBM_GETPOS, 0, 0); return true; }
	tTYPE_ID GetType() { return tid_DWORD; }

	bool     IsResizeble(bool bHorz) { return bHorz; }
	bool     IsAdjustable(bool bHorz) { return false; }
};

//////////////////////////////////////////////////////////////////////
// CCombo

class CCombo : public CItemControl<CComboItem>
{
public:
	typedef CItemControl<CComboItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.ComboBox", "COMBOBOX", NULL);

	CCombo();

	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);
	void InitItem(LPCSTR strType);
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void GetAddSize(SIZE& szAdd) { szAdd.cy += 8; }

protected:
	BEGIN_MSG_MAP(CCombo)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { return DefWindowProc(); }

public:
	tSIZE_T ctl_GetCurSel() { return ::SendMessage(m_hWnd, CB_GETCURSEL, (WPARAM)0, ( LPARAM )0); }
	void    ctl_SetCurSel(tSIZE_T nIdx) { ::SendMessage(m_hWnd, CB_SETCURSEL, (WPARAM)nIdx, ( LPARAM )0); }
	tSIZE_T ctl_GetItemsCount() { return ::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0); }
	void    ctl_DeleteItem(tSIZE_T nIdx) { ::SendMessage(m_hWnd, CB_DELETESTRING, (WPARAM)nIdx, 0); }
	void    ctl_DeleteAllItems() { ::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0); }
	void    ctl_GetTextByPos(tSIZE_T nIdx, cStrObj& strText);
	tSIZE_T ctl_FindByName(LPCWSTR strName);
	tSIZE_T ctl_AddItem(LPCWSTR strName);
	tPTR    ctl_GetItemData(tSIZE_T nIdx);
	void    ctl_SetItemData(tSIZE_T nIdx, tPTR pData);

	tDWORD m_nDropLines;
};

//////////////////////////////////////////////////////////////////////
// CSlider

class CSlider : public CItemControl<CSliderItem>
{
public:
	typedef CItemControl<CSliderItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.Slider", TRACKBAR_CLASS, NULL);
	
public:
	CSlider();

	void ApplyStyle(LPCSTR strStyle);
	void OnInited();

protected:
	void ctl_SetRange(tSIZE_T nMin, tSIZE_T nMax);
	void ctl_SetPos(tSIZE_T nPos);
	tSIZE_T ctl_GetPos();
	bool ctl_IsVert();

protected:
	BEGIN_MSG_MAP(CSlider)
		MESSAGE_HANDLER(OCM_VSCROLL, OnScroll)
		MESSAGE_HANDLER(OCM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlBkColorT)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()
	
	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_DataChanged();
		return bHandled = FALSE, 0;
	}
};

//////////////////////////////////////////////////////////////////////
// CTab

class CTab : public CItemControl<CTabItem>, public CXPBrush
{
public:
	typedef CItemControl<CTabItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.Tab", WC_TABCONTROL, NULL);

protected:
	BEGIN_MSG_MAP( CTab )
		MESSAGE_HANDLER(OCM_NOTIFY,    OnNotifyCtrl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT,      OnPaint)
		
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

protected:
	LRESULT OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	tSIZE_T ctl_GetCurSel();
	void   ctl_SetCurSel(tSIZE_T nIdx);
	void   ctl_SetItem(tSIZE_T nIdx, LPCSTR strText, CItemBase * pItem);
	tSIZE_T ctl_GetItemCount();
	void   ctl_DeleteItem(tSIZE_T nIdx);
	void   ctl_DeleteAllItems();
	void   ctl_AdjustRect(RECT& rcClient);

protected:
	void OnInit();
	void Cleanup();
	CItemBase * CreateChild(LPCSTR strItemId, LPCSTR strItemType);
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void DrawItem(HDC dc, RECT& rcFrame);
	void DrawTabItem(HDC dc, RECT& rcItem, int nItem, int nState);

	COLORREF m_clBg;
};

//////////////////////////////////////////////////////////////////////
// CIPAddressCtrl

class CIPAddressCtrl : public CItemControl<CIPAddrEditItem>
{
public:
	CIPAddressCtrl();
	virtual ~CIPAddressCtrl();

	typedef CItemControl<CIPAddrEditItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.IPAddress", WC_IPADDRESS, NULL);

protected:
	BEGIN_MSG_MAP(CIPAddressCtrl)
		MESSAGE_HANDLER(WM_GETFONT,  OnGetFont)
		MESSAGE_HANDLER(WM_SETFONT,  OnSetFont)
		MESSAGE_HANDLER(OCM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	void InitItem(LPCSTR strType);
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);

	LRESULT  OnGetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
public:
	void     GetAddSize(SIZE& szAdd);
	bool     SetValue(const cVariant &pValue);
	bool     GetValue(cVariant &pValue);

protected:
	HFONT m_hFont;
};

//////////////////////////////////////////////////////////////////////
// CEdit

class CEditRtn : public CControlBase
{
public:
	CEditRtn() : m_bInternalTextOp(0) {}
	
	BOOL  SetWindowTextW(CRootItem * pRoot, LPCWSTR strText)        { m_bInternalTextOp = 1; BOOL res = CControlBase::SetWindowTextW(pRoot, strText); m_bInternalTextOp = 0; return res; }
	void  GetWindowTextW(CRootItem * pRoot, cStrObj& strText) const { m_bInternalTextOp = 1; CControlBase::GetWindowTextW(pRoot, strText); m_bInternalTextOp = 0; }

	HWND Create(CRootItem * pRoot, HWND hWndParent, CWndClassInfo& pCI, RECT& rcPos, DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0);

	mutable unsigned  m_bInternalTextOp : 1;
};

class CEdit : public CItemWindowT<CEditRtn, CEditItem>
{
public:
	typedef CItemWindowT<CEditRtn, CEditItem> TBase;
	DECLARE_WND_AVPCLASS("EDIT", "EDIT", NULL);

	CEdit();

protected:
	BEGIN_MSG_MAP(CEdit)
		MESSAGE_HANDLER(OCM_COMMAND,         OnCommand)
		if( m_nCompress )
			MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlBkColorT)
		if( m_nStyles & ES_READONLY )
			MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlBkColor)

		MESSAGE_HANDLER(WM_MOVE,             OnMove)
		MESSAGE_HANDLER(WM_SIZE,             OnSize)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,      OnLButtonDown)
		MESSAGE_HANDLER(WM_NCCALCSIZE,       OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCPAINT,          OnNcPaint)
		
		MESSAGE_HANDLER(WM_GETTEXTLENGTH,    OnPswdProtectCheck)
		MESSAGE_HANDLER(WM_GETTEXT,          OnPswdProtectCheck)
		MESSAGE_HANDLER(WM_SETTEXT,          OnPswdProtectCheck)
		MESSAGE_HANDLER(EM_SETPASSWORDCHAR,  OnPswdProtectCheck)
		MESSAGE_HANDLER(WM_CHAR,	         OnChar)

		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMsgMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPswdProtectCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	void UpdateSpin();
	void SetRange(POINT& ptLim);

	void GetAddSize(SIZE& szAdd);
	void ApplyStyle(LPCSTR strStyle);
	
	void Cleanup();
	void InitItem(LPCSTR strType);
	void OnInited();
	void OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);

protected:
	void ctl_ReplaceText(LPCWSTR strText, tSIZE_T nFrom, tSIZE_T nTo = -1);
	void ctl_ReplaceSelText(LPCWSTR strText);
	void ctl_SetSelText(tSIZE_T nFrom, tSIZE_T nTo, bool bAsync = false);
	bool ctl_IsMultiline();
	bool ctl_IsNumber();
	bool ctl_IsReadonly();

protected:
	unsigned          m_bShowMenu : 1;
	unsigned          m_bAutoComplete : 1;
	unsigned          m_bUpdateSpin : 1;

	HWND              m_hSpin;
	SIZE              m_szSpin;
};

//////////////////////////////////////////////////////////////////////
// CTimeEditBox

class CTimeEditBox : public CItemControl<>
{
public:
	typedef CItemControl<> TBase;
	DECLARE_WND_AVPCLASS("AVP.TimeEdit", DATETIMEPICK_CLASS, NULL);

	CTimeEditBox();

protected:
	BEGIN_MSG_MAP(CTimeEditBox)
		MESSAGE_HANDLER(OCM_NOTIFY, OnNotify)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
public:
	void     OnInit();
	void     GetAddSize(SIZE& szAdd);
	void     ApplyStyle(LPCSTR strStyle);
	tTYPE_ID GetType() { return tid_DWORD; }
	bool     IsOut(){ return true; }
	bool     SetValue(const cVariant &pValue);
	bool     GetValue(cVariant &pValue);

private:
	tBOOL    m_bSeconds;
	tDWORD   m_nType;
};

//////////////////////////////////////////////////////////////////////
// CRichEditImpl

class CRichEditImpl : public CItemControl<CRichEditItem>
{
public:
	typedef CItemControl<CRichEditItem> TBase;
	
	CRichEditImpl();
	
	void InitItem(LPCSTR strType);
	void ApplyStyle(LPCSTR strStyle);

protected:
	DECLARE_WND_AVPCLASS("AVP.RichEdit", "RichEdit20A", NULL);

	BEGIN_MSG_MAP(CRichEditImpl)
		MESSAGE_HANDLER(OCM_NOTIFY,       OnNotifyCtrl)
		MESSAGE_HANDLER(WM_SETFOCUS,      OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS,     OnKillFocus)
		MESSAGE_HANDLER(WM_CREATE,        OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND,    OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT,         OnPaint)
		MESSAGE_HANDLER(WM_NCPAINT,       OnNcPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,   OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,     OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_KEYDOWN,       OnKeyDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE,     OnMouseMove)
		MESSAGE_HANDLER(WM_SHOWWINDOW,    OnShowWindow)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT	OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	void    OnApplyBlockStyles();
	void	ApplyRefStyles();
	void	ApplyBoldStyles();
	void    SetRichBlockStyle(DWORD dwMask, DWORD dwEffect, CHARRANGE& chrRange);
	void    OnCalcTextSize(HDC dc, SIZE& sz, tDWORD nFlags);
	void    OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc);
	
	void    UpdateRefsText(LPCSTR strText, bool bFull = true);

protected:
	void ConvertCharformat2LogFont(HDC hDc, const CHARFORMAT2& cf, LOGFONT& lf);
	void CalcTextSize(CHARRANGE& cr, SIZE& size);
	void CalcSingleLineRect(CHARRANGE& cr, RECT& rect);
	void DrawFocusRect();

public:
	unsigned  m_bNoFocus : 1;
	unsigned  m_bLBClick : 1;
	unsigned  m_bStylesApplied : 1;
	CHARRANGE m_curLink;
    size_t    m_indSelLink;
};

//////////////////////////////////////////////////////////////////////
// CGridCtrlImpl

class CGridCtrlImpl : public CItemControl<CListItem>
{
public:
	CGridCtrlImpl()
	{
		m_nFlags |= STYLE_DRAW_BACKGROUND|STYLE_DRAW_CONTENT|STYLE_TRANSPARENT/*|STYLE_CUSTOMCHILDREN*/;
		m_nStyles |= WS_VSCROLL|WS_HSCROLL|WS_TABSTOP|LVS_NOCOLUMNHEADER;
		m_nStylesEx = WS_EX_CLIENTEDGE;
	}

	typedef CItemControl<CListItem> TBase;
	DECLARE_WND_AVPCLASS_EX("AVP.List", "SysListView32", IDC_ARROW, CS_DBLCLKS);

	void OnUpdateScrollInfo(cScrollInfo& info)
	{
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.nMin = 0;
		si.fMask = SIF_RANGE|SIF_PAGE|SIF_POS;
		si.nPage = info.nPage;
		si.nPos = info.nPos;
		si.nMax = info.nMax;
		SetScrollInfo(info.nType == stVScroll ? SB_VERT : SB_HORZ, &si, TRUE);
	}

	bool OnColumnTrack(cColumn& col, tDWORD posX)
	{
		if(::GetCapture() != NULL) return false;
		::SetCapture(m_hWnd);

		// Цикл обработки сообщений
		long size = col.m_Size.cx, newsize;
		bool bContinue = true;
		while(bContinue)
		{
			MSG msg;
			::GetMessage(&msg, NULL, 0, 0);
			if(::GetCapture() != m_hWnd)
				break;

			switch(msg.message)
			{
			case WM_MOUSEMOVE:
				newsize = size + GET_X_LPARAM(msg.lParam) - posX;
				if( newsize < 0 ) newsize = 0;
				col.ChangeSize(newsize - col.m_Size.cx);
				::SetCursor(g_curTrackCol);
				Update(NULL);
				break;
			case WM_KEYDOWN:
				if(msg.wParam != VK_ESCAPE)
				   break;
			case WM_RBUTTONDOWN:
			case WM_LBUTTONUP:
				bContinue = false;
				break;

			default:
				::DispatchMessage(&msg);
			};
		}
	   ::ReleaseCapture();
		return true;

	}

	bool OnDragItem(cRowId* row, cColumn* col, POINT& pt)
	{
		if(::GetCapture() != NULL) return false;
		::SetCapture(m_hWnd);

		MSG	msg; bool bContinue = true, bSelect = false;
		while(bContinue)
		{
			::GetMessage(&msg, NULL, 0, 0);
			if(::GetCapture() != m_hWnd)
				break;

			switch(msg.message)
			{
			case WM_MOUSEMOVE:
				pt.x = GET_X_LPARAM(msg.lParam); pt.y = GET_Y_LPARAM(msg.lParam); 
				bContinue = DragItem(row, col, pt, false);
				break;
			case WM_KEYDOWN:
				if(msg.wParam != VK_ESCAPE)
				   break;
			case WM_LBUTTONUP:
				pt.x = GET_X_LPARAM(msg.lParam); pt.y = GET_Y_LPARAM(msg.lParam); 
				bSelect = true;
			case WM_RBUTTONDOWN:
				bContinue = false;
				break;
			case WM_TIMER:
				continue;
			default:
				::DispatchMessage(&msg);
			};
		}

		::ReleaseCapture();
		return DragItem(row, col, pt, true);
	}

	void OnSetCursor(enCursorType type)
	{
		if( type == ctResize )
		{
			if( !g_curTrackCol )
//				g_curTrackCol =	::LoadCursor(_Module.m_hInst, MAKEINTRESOURCE(IDC_TRACKCOL));
				g_curTrackCol =	::LoadCursor(NULL, IDC_SIZEWE);

			::SetCursor(g_curTrackCol);
		}
		else if( type == ctNo )
			::SetCursor(::LoadCursor(NULL, IDC_NO));
		else if( type == ctDragCol || type == ctHand )
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
		else if( type == ctDefault )
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}

	void OnGetViewSize(SIZE& szView)
	{
		RECT rcClient; GetClientRect(&rcClient);
		szView.cx = RECT_CX(rcClient); szView.cy = RECT_CY(rcClient);
	}

	void OnFocus(bool bFocusted)
	{
		TBase::OnFocus(bFocusted);
		TBase::Update(NULL);
	}

protected:
	BEGIN_MSG_MAP(CItemControl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnDummy)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnButtonDown)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnDlgCode)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnDummy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShow)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT	OnShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bWasLoad = FALSE;
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT	OnDummy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT	OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE sz = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		CListItem::OnSize(sz);
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_MsgPaint();
		bHandled = TRUE;
		return S_OK;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		OnOver(pt);
		bHandled = TRUE;
		LRESULT lResult = 0;
		TBase::ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, lResult);
		return TRUE;
	}

	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		Scroll(stVScroll, (((short)HIWORD(wParam)) > 0) ? sdPrev : sdNext, 0);
		return DLGC_WANTARROWS;
	}

	LRESULT OnDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return DLGC_WANTARROWS;
	}

	LRESULT OnButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( !(m_nState & ISTATE_FOCUSED) )
			SetFocus();

		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		tDWORD state = (wParam&MK_CONTROL)?fControl:0|(wParam&MK_SHIFT)?fShift:0;
		if( uMsg == WM_LBUTTONDBLCLK )
			state |= fDouble;
		else
			state |= (uMsg == WM_LBUTTONDOWN) ? fLButton : fRButton;

		OnCliked(pt, state);
		if( uMsg != WM_LBUTTONDBLCLK )
			TBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
		bHandled = TRUE;
		return 0;
	}
    RECT GetCheckRect(RECT rcCell, bool bCentered);

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		tDWORD state = 0;
		if( GetKeyState(VK_CONTROL) & (1<<15) ) state |= fControl;

		enUserAction act = sdUnknown;
		switch(wParam)
		{
		case VK_LEFT:   act = sdLeft; break;
		case VK_RIGHT:  act = sdRight; break;
		case VK_UP:     act = sdPrev; break;
		case VK_DOWN:   act = sdNext; break;
		case VK_PRIOR:  act = sdPagePrev; break;
		case VK_NEXT:   act = sdPageNext; break;
		case VK_HOME:   act = sdHome; break;
		case VK_END:    act = sdEnd; break;
		case VK_SPACE:  act = sdSpace; break;
		case VK_DELETE: act = sdDelete; break;
		case VK_RETURN: act = sdEnter; break;
		case 'A': if( !(m_flags & fSingleSel) && (state & fControl) ) { SelectRange(0, m_nAllSize); Update(NULL); } break;
		case 'F': if( state & fControl ) FindRows(eShowDlg); break;
		case 'C':
		case VK_INSERT: if( state & fControl ) CopyToClipboard(); break;
		case VK_F3: FindRows(!(GetKeyState(VK_SHIFT) & 0x80000000) ? 0 : eFindUp); break;
		case VK_ADD:    if( state & fControl ) AutoFitColumn(m_cols, true); break;
		}
		if( act != sdUnknown )
		{
			if( GetKeyState(VK_SHIFT) & (1<<15) ) state |= fShift;
			TBase::OnKeyDown(act, state);
		}
		return 0;
	}

	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		enScrollType type = (uMsg == WM_VSCROLL) ? stVScroll : stHScroll;
		enUserAction dir = sdExactPos;
		tDWORD pos = 0;
		switch(LOWORD(wParam))
		{
		case SB_TOP:         dir = sdHome; break;
		case SB_BOTTOM:      dir = sdEnd; break;
		case SB_LINEUP:      dir = sdPrev; break;
		case SB_LINEDOWN:    dir = sdNext; break;
		case SB_PAGEUP:      dir = sdPagePrev; break;
		case SB_PAGEDOWN:    dir = sdPageNext; break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:  
			 {
				SCROLLINFO si;
				si.cbSize = sizeof(si);
				si.fMask = SIF_TRACKPOS;
				 ::GetScrollInfo(m_hWnd, (uMsg == WM_VSCROLL) ? SB_VERT : SB_HORZ, &si);
				pos = si.nTrackPos;;
				 dir = sdExactPos;
				break;
			}

		case SB_ENDSCROLL: return 0;
		}
		Scroll(type, dir, pos);
		return bHandled = TRUE, 0;
	}

	void CopyToClipboard()
	{
		cStrObj sCopyText;
		cRowId nItem;
		while( GetSelectedItem(nItem) != invalidIndex )
			MakeCopyString(nItem, m_cols, sCopyText);

		if( sCopyText.empty() )
			return;

		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, sCopyText.memsize(cCP_UNICODE));
		if( !hglbCopy )
			return;

		void* lptstrCopy = GlobalLock(hglbCopy);
		wcscpy((wchar_t*)lptstrCopy, sCopyText.data());
		GlobalUnlock(hglbCopy);

		if( OpenClipboard() )
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hglbCopy);
			CloseClipboard();
		}
	}
};

//////////////////////////////////////////////////////////////////////
// CTreeImpl

class CTreeImpl : public CItemControl<CTreeItem>
{
public:
	typedef CItemControl<CTreeItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.Tree", WC_TREEVIEW, NULL);

public:
	CTreeImpl();

	cTreeItem * AppendItem(cTreeItem * pParent, cTreeItem * pItem, cTreeItem * pAfter = NULL);
	void        DeleteItem(cTreeItem * pItem);
	cTreeItem * GetParentItem(cTreeItem * pItem);
	cTreeItem * GetChildItem(cTreeItem * pItem);
	cTreeItem * GetNextItem(cTreeItem * pItem);
	cTreeItem * GetSelectionItem();
	cTreeItem * TreeHitTest(POINT &pt, tUINT * pnFlags);
	void        SelectItem(cTreeItem * pItem);
	void        ExpandItem(cTreeItem * pItem, bool bExpand);
	void        SetItemCheckState(cTreeItem * pItem, bool bChecked = true);
	void        SortChildren(cTreeItem * pItem, bool bRecurse);
	void        SetImageList(hImageList hImages);
	void        UpdateView(int nFlags);
	bool        AddStateIcon(CIcon * pIcon);
	void		DeleteChildItems(cTreeItem * pParent);

protected:
	BEGIN_MSG_MAP(CTreeImpl)
		MESSAGE_HANDLER(OCM_NOTIFY,     OnNotifyCtrl)
		MESSAGE_HANDLER(WM_KEYDOWN,     OnKeyDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()
	
	LRESULT OnNotifyCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM Param, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	static int CALLBACK _CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

protected:
	void      OnInit();
	void      Destroy();

	void      ApplyStyle(LPCSTR strStyle);
	
	void        _GetItemInfo(TVITEMA& item, bool bAnsi);
	cTreeItem * _GetItemData(hTreeItem hItem);
	void        _UpdateItemStateIconState(tDWORD &state, tDWORD &stateMask);
	bool        _CreateStateImageList();
	void        _DestroyStateImageList();
	void        _AddCheckboxIcon(HIMAGELIST hList, bool bChecked, tDWORD nState);

protected:
	HCURSOR             m_hOldCursor;
	BOOL                m_bExpanding;
	HIMAGELIST          m_hStateImgList;
	unsigned            m_fSetSel : 1;
};

//////////////////////////////////////////////////////////////////////
// CDialogControl

class CDialogControl : public CItemWindowT<CDialogBase, CDialogControlItem>
{
public:
	typedef CItemWindowT<CDialogBase, CDialogControlItem> TBase;
	DECLARE_DLG_AVPCLASS(NULL)

	CDialogControl()
	{
		m_nStyles = WS_CHILD|WS_TABSTOP;
		m_nStylesEx = WS_EX_CONTROLPARENT;
	}

	BEGIN_MSG_MAP(CDialogControl)
		MESSAGE_HANDLER(WM_HELP, OnHelp)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		if( uMsg != WM_CTLCOLORDLG )
			AVP_REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	void InitItem(LPCSTR strType)
	{
		TBase::InitItem(strType);
		if( m_pRoot->IsThemed() &&
			(!(m_pRoot->m_nGuiFlags & GUIFLAG_ADMIN) ||
			  (m_pParent && m_pParent->m_strItemType == GUI_ITEMT_TAB) ))
			static_cast<CWinRoot*>(m_pRoot)->EnableThemeDialogTexture(m_hWnd);

		if( !GUI_ISSTYLE_SIMPLE(m_pRoot) && m_pParent && m_pParent->m_pBackground )
			m_pBackground = m_pParent->m_pBackground;
	}
};

//////////////////////////////////////////////////////////////////////
// CPropertySheetPage

class CPropertySheetPage : public CDialogControl
{
public:
	CPropertySheetPage()
	{
		m_nFlags |= STYLE_TOP_ITEM;
	}

	BEGIN_MSG_MAP(CPropertySheetPage)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CDialogControl)
	END_MSG_MAP()
	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE l_szNew = {LOWORD(lParam), HIWORD(lParam)};

		if( (l_szNew.cx - m_szSize.cx) || (l_szNew.cy - m_szSize.cy) )
			Resize(l_szNew.cx - m_szSize.cx, l_szNew.cy - m_szSize.cy, false);
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////

class CDialogItemImpl : public CItemWindowT<CDialogBase, CDialogItem>
{
public:
	DECLARE_DLG_AVPCLASS("")

	typedef CItemWindowT<CDialogBase, CDialogItem> TBase;
	CDialogItemImpl();

	bool IsActive();
	void Activate();
	void GetParentRect(RECT& rcParent, CItemBase* pOwner = NULL);
	void ModalEnableParent(bool bEnable);

protected:
	BEGIN_MSG_MAP(CDialogItemImpl)
		MESSAGE_HANDLER(WM_HELP, OnHelp)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinmaxInfo)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_QUERYENDSESSION, OnQueryEndSession)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_NCMOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(DM_GETDEFID, OnGetDefId)

		MESSAGE_HANDLER(OCM_SYSCOLORCHANGE, OnSysColorChange)

		AVP_REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	void InitItem(LPCSTR strType);
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void Cleanup();
	void ApplyStyle(LPCSTR strStyle);
	void DrawBackground(HDC dc, RECT& rcClient, RECT * prcClip);
	void Close(tDWORD nAction);
	void SetShowMode(eShow eShowMode);
	bool SetNextDlgItem(bool bGroup, bool bBackward);
	bool PreTranslateMessage(MSG& msg);
	void ReLoad();

	void OnUpdateProps(int flags, tDWORD nStateMask = 0, RECT * rc = NULL);
	void OnAdjustBordSize(RECT& rc);

	LRESULT OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetDefId(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnQueryEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysColorChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinmaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){ return FALSE; }
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnFinalMessage(HWND);

protected:
	bool _CloseDlg();
	tUINT _GetDlgPosOffset();
	bool _CheckDlgPosOffset();
	void _InitDlgRect(RECT& rcFrame);
	void _UpdateRgn();

protected:
	void   ctl_SetTransparentValue(tBYTE nAlpha);
	tDWORD ctl_SetTransparentTimer(tDWORD nInterval);
	void   ctl_KillTransparentTimer(tDWORD nId);
	
protected:
	unsigned     m_bInited : 1;
	unsigned     m_bEndSession : 1;
	unsigned     m_bEnableParent : 1;
	unsigned     m_bMouseOver : 1;
	unsigned     m_bGripperOn : 1;
	tDWORD       m_nCurShowState;

protected:
	class CVoidWndImpl : public CItemControl<CItemBase> { public: DECLARE_WND_AVPCLASS("AVP.Void", NULL, NULL); };
	CVoidWndImpl m_HiddenParent;
};

//////////////////////////////////////////////////////////////////////
// CPopupMenu

#define MENU_ICON_MARGIN 23
#define MENU_LINE_HEIGHT 23

class CPopupMenu : public CPopupMenuItem
{
public:
	CPopupMenu() : m_hFont(NULL), m_pCheck(NULL){}
	~CPopupMenu();

	static void    CleanSeparators(HMENU hMenu);
	static LRESULT DrawItem(LPDRAWITEMSTRUCT lpInfo);
	static LRESULT MeasureItem(LPMEASUREITEMSTRUCT lpInfo);

protected:
	HMENU  ctl_GetRootItem();
	HMENU  ctl_AddItem(HMENU hMenu, CItemBase * pItem, bool bSub = false);
	tDWORD ctl_Track(POINT& pt);
	void   ctl_TrackEnd();
	void   ctl_CleanSeps(HMENU hMenu) { CleanSeparators(hMenu); }

protected:
	HFONT    m_hFont;
	CMenuHnd m_hMenu;
	CIcon*   m_pCheck;
};

//////////////////////////////////////////////////////////////////////
// CTrayIcon

extern const UINT WM_TASKBAR_CREATED;
extern const UINT KWS_TRAY_MESSAGE;

class CTrayIcon :
	public CTrayItem,
	public CWindowImpl<CTrayIcon, CWindowL, CWinTraits<WS_POPUP, 0> >
{
public:
	DECLARE_WND_CLASS("AVP.Tray");

	class CChanger : public CRootTaskBase
	{
	public:
		CChanger(CRootItem *pRoot, UINT uID);
		~CChanger();
		
		virtual void do_work();

		NOTIFYICONDATAW m_nid;
		bool *          m_pbEnd;
	};

public:
	CTrayIcon();
	
	HWND GetWindow(){ return m_hWnd; }
	void Cleanup();
	void InitItem(LPCSTR strType);
	void Click(bool bForce = false, bool rClick = false);

protected:
	void OnInit();
	void OnUpdateProps(int flags, tDWORD nStateMask = 0, RECT * rc = NULL);
	void TimerRefresh(tDWORD nTimerSpin);

protected:
	void ShowBalloon(LPCSTR szText, LPCSTR szTitle, LPCSTR szValue, tDWORD uTimeout, tDWORD dwIconType);
	LPCSTR GetBalloonText();

	BEGIN_MSG_MAP(CTrayIcon)
		MESSAGE_HANDLER(KWS_TRAY_MESSAGE, OnTrayMessage)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnMenu)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnMenu)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnSelDefItem)
		MESSAGE_HANDLER(WM_TASKBAR_CREATED, OnTaskbarCreated)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()

protected:
	LRESULT OnTrayMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Internal event handlers
	LRESULT OnMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSelDefItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	tString         m_strBalloonText;
	UINT			m_uID;

	static UINT		g_uTrayCount;
};

//////////////////////////////////////////////////////////////////////

class CWinCreditsCtrl : public CItemControl<>
{
	typedef CItemControl<> TBase;
	DECLARE_WND_AVPCLASS_EX("AVP.Credits", NULL, NULL, CS_DBLCLKS);

	BEGIN_MSG_MAP(CWinCreditsCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnInit();
	void DrawItem(HDC dc, RECT& rcFrame);
public:
	CWinCreditsCtrl();
	~CWinCreditsCtrl();

    void Scroll();
    bool IsCaptured() { return m_mousecaptured; }


private:
	struct CSItem
	{
		tString		m_sname;
		int			m_height;
		CFontStyle* m_pFont;
		SIZE		m_margin;

		CSItem() : m_sname(""), m_height(0), m_pFont(NULL) { m_margin.cx = m_margin.cy = 0; }
		CSItem(tString nm, CFontStyle* pF, int hi, SIZE sz) : m_sname(nm), m_pFont(pF), m_height(hi), m_margin(sz) {}
	};
	
	struct CreditSection
	{
		CSItem			m_csTitle;
		cVector<CSItem> m_vCredits;

		int CalcTotalHeight();
	};

	enum DCType { dc_scr = 0, dc_capt, dc_work, dc_mask }; 
	
	cVector<CreditSection>	m_vSections;
	cVector<int>			m_vStartSect;

	HDC			m_dc[dc_mask + 1];
	HBITMAP		m_hbmp[dc_mask + 1];
	HBITMAP		m_hbmp_old[dc_mask + 1];

	int			m_TitleHi, m_nScrollY, m_nBlendCaption, m_cur_section;
	SIZE		m_szWorkBmp;
	UINT_PTR	m_TimerID;

	typedef BOOL WINAPI AlphaBlend_FUNC(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, BLENDFUNCTION blendFunction);
	HMODULE				m_hIma32;
	AlphaBlend_FUNC*	m_pfnAlphaBlend;

    void SetScroll(int y);
    void NormalizeScroll();
    bool m_mousecaptured;
    int  m_mousey;
    int  m_mousetime;
    typedef double speed_t;
    speed_t m_speedy;
    speed_t m_scroll;
    bool    m_accmode;

    CImagePng m_png;
    bool m_pngact;

	////////////////////////////////////
	void DrawOneStep(HDC dc, RECT rcFrame);
	
	void CreateCompDC(HDC hDC, int nItem, int cx, int cy);
	void DeleteCompDC(int nItem);
	
	void SetCreditsTimer();
	void KillCreditsTimer();

	void UpdateCreditsDC(HDC dc, RECT rcFrame);
	void DrawCaption(HDC ParentDc, RECT rc, int isection);
	int	 CreditSection2WorkDC(const CreditSection& cs, int top);
	int	 GetSectionByScrollY(int ScrollY);
    bool GetItemByScrollY(int ScrollY, int& sect, int& item);
};

//////////////////////////////////////////////////////////////////////
// CWinHTMLCtrl

class CDocHostUIHandlerDispImpl;

class CWinHTMLCtrl : public CItemWindowT< CControlBaseT< CAxWindowT<CWindowL> >, CHTMLItem > 
	, public IDispEventImpl<0, CWinHTMLCtrl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
public:
	typedef CItemWindowT< CControlBaseT< CAxWindowT<CWindowL> >, CHTMLItem >  TBase;

#ifndef ATLAXWIN_CLASS
#define ATLAXWIN_CLASS	"AtlAxWin"
#endif
	DECLARE_WND_AVPCLASS("AVP.HTMLCtrl", ATLAXWIN_CLASS, NULL);

	CWinHTMLCtrl();
	virtual ~CWinHTMLCtrl();

	virtual bool Navigate(LPCSTR szUrl);
	virtual bool Submit(LPCSTR szFormName = NULL);

	void InitItem(LPCSTR strType);
	void Cleanup();
	void OnInited();

	void ApplyStyle(LPCSTR strStyle);

	void OnUpdateProps(int flags = UPDATE_FLAG_ALL, tDWORD nStateMask = 0, RECT * rc = NULL);

	BEGIN_SINK_MAP(CWinHTMLCtrl)
		SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, BeforeNavigate2)
		SINK_ENTRY_EX(0, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, NavigateComplete2)
	END_SINK_MAP()

	STDMETHOD_(void, BeforeNavigate2)(
		IDispatch *pDisp,
		VARIANT *URL,
		VARIANT *Flags,
		VARIANT *TargetFrameName,
		VARIANT *PostData,
		VARIANT *Headers,
		VARIANT_BOOL *Cancel);

	STDMETHOD_(void, NavigateComplete2)(
		IDispatch *pDisp,
		VARIANT *URL);

protected:
	void RenderText();
	bool NeedScroll(CComQIPtr<IHTMLDocument2>& pDoc);
	void GetHTMLDocument(CComQIPtr<IHTMLDocument2>& pDoc);

protected:
	CComQIPtr<IWebBrowser2> m_pWB2;
	CDocHostUIHandlerDispImpl* m_pDocHostUI;
	bool m_bNavigateComplete;

	SIZE m_szScroll;
};

//////////////////////////////////////////////////////////////////////

class CSlideShowCtrl : public CItemBase
{

public:
	CSlideShowCtrl();	

	//override
	void OnInit();
	void TimerRefresh(tDWORD nTimerSpin);
	void DrawChildren(HDC dc, RECT* rcUpdate);
    bool OnClicked(CItemBase* pItem);

	SINK_MAP_BEGIN()
//		SINK_STATIC_ANY(_CmnSink)
		{ pItem->AttachSink(&_CmnSink, false); }
		SINK_MAP_END(CItemBase)

	class CMySink : public CItemSink
	{
	public:
		void OnInitProps(CItemProps& strProps)
		{
//			m_aItemsDelay[m_pItem->m_nPos]=(int)strProps.Get(STR_PID_DELAY).GetLong();
		}
	};

	CMySink	_CmnSink;

protected:
	virtual void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	bool IsAutoSlideShow(){return (m_nDelay && m_aItems.size ());}
	void GoNextSlide();

	struct GDIHelper
	{
		typedef BOOL WINAPI AlphaBlend_FUNC(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, BLENDFUNCTION blendFunction);
		GDIHelper()
		{
			SetNull();

			m_hIma32 = ::LoadLibrary(_T("Msimg32.dll"));
			if( m_hIma32 )
				m_pfnAlphaBlend = (AlphaBlend_FUNC *)::GetProcAddress(m_hIma32, "AlphaBlend");
		}
		~GDIHelper()
		{
			Clear();

			if (m_hIma32)
				::FreeLibrary(m_hIma32);

			m_hIma32 = NULL;
		}

		HMODULE				m_hIma32;
		AlphaBlend_FUNC*	m_pfnAlphaBlend;

		HDC m_DC;
		HBITMAP m_hBitmap;
		HBITMAP m_hPrevBitmap;
		void SetNull(){m_DC=NULL; m_hBitmap=NULL; m_hPrevBitmap=NULL;}
		GDIHelper& operator=(GDIHelper& src)
		{
			Clear();
			m_DC = src.m_DC;
			m_hBitmap = src.m_hBitmap;
			m_hPrevBitmap = src.m_hPrevBitmap;
			src.SetNull();
			return *this;
		}
		void Clear()
		{
			if(m_DC)
			{
				if (m_hPrevBitmap)
					::SelectObject(m_DC, m_hPrevBitmap);

				::DeleteDC(m_DC);
			}

			if (m_hBitmap)
				DeleteObject(m_hBitmap);

			m_DC = NULL;
			m_hBitmap = NULL;
			m_hPrevBitmap = NULL;
		}

		bool IsValid()
		{
			return m_DC!=NULL;
		}
		/*		HDC CWinRoot::GetTempDC(RECT& rcFrame, RECT* rcUpdate)
		{
		SIZE szNew = {RECT_CX(rcFrame), RECT_CY(rcFrame)};

		bool bInit = true;
		if( !m_hTmpDC )
		{
		m_szTmpBmp = szNew;
		m_hTmpDC = ::CreateCompatibleDC(GetDesktopDC());
		GetLastError();
		}
		else if( m_szTmpBmp.cx < szNew.cx || m_szTmpBmp.cy < szNew.cy )
		{
		m_szTmpBmp.cx = max(m_szTmpBmp.cx, szNew.cx);
		m_szTmpBmp.cy = max(m_szTmpBmp.cy, szNew.cy);
		::SelectObject(m_hTmpDC, m_hTmpOldBmp);
		m_hTmpBmp.Cleanup();
		}
		else
		bInit = false;

		if( bInit )
		{
		m_hTmpBmp = ::CreateCompatibleBitmap(GetDesktopDC(), m_szTmpBmp.cx, m_szTmpBmp.cy);
		m_hTmpOldBmp = (HBITMAP)::SelectObject(m_hTmpDC, m_hTmpBmp);
		}
		::SetViewportOrgEx(m_hTmpDC, -rcFrame.left, -rcFrame.top, NULL);
		if( rcUpdate )
		::IntersectClipRect(m_hTmpDC, rcUpdate->left, rcUpdate->top, rcUpdate->right, rcUpdate->bottom);
		else
		::IntersectClipRect(m_hTmpDC, 0, 0, szNew.cx, szNew.cy);
		return m_hTmpDC;
		}
		*/
		bool CreateTransparentDC(HDC dc, RECT& rcClient)
		{
			int iWidth = rcClient.right-rcClient.left;
			int iHeight = rcClient.bottom-rcClient.top;

			Clear();

			m_DC				= ::CreateCompatibleDC(dc);
			m_hBitmap			= ::CreateCompatibleBitmap(dc, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
			m_hPrevBitmap		= (HBITMAP)::SelectObject(m_DC, m_hBitmap);
			::SetViewportOrgEx (m_DC, -rcClient.left, -rcClient.top, NULL);
			::BitBlt(m_DC, rcClient.left,rcClient.top,iWidth, iHeight, dc, rcClient.left,rcClient.top,SRCCOPY);
			return IsValid();
		}

		void BlendTo(HDC dc, RECT& rcClient, BYTE iAlpha)
		{

			int iWidth = rcClient.right-rcClient.left;
			int iHeight = rcClient.bottom-rcClient.top;

// 			 			BitBlt(dc, rcClient.left,rcClient.top,iWidth, iHeight, m_DC, rcClient.left,rcClient.top,SRCCOPY);
// 			 			return;

			BLENDFUNCTION bf = { AC_SRC_OVER, 0, iAlpha, 0 };

			if (m_pfnAlphaBlend)
				m_pfnAlphaBlend(dc, rcClient.left,rcClient.top, iWidth, iHeight, m_DC, rcClient.left,rcClient.top, iWidth, iHeight, bf);
		}
	};




	GDIHelper m_MemDC, m_OldDC, m_ActiveDC;

	bool m_bAnimateInProgress;

	int m_iCurrentSlide;
	int m_iFrame;
	int m_nDelay;
	tDWORD m_dwLastTimeSpin;
};

//////////////////////////////////////////////////////////////////////
// CSplitCtrl

class CSplitCtrl : public CItemControl<CSplitterItem>
{
public:
	typedef CItemControl<CSplitterItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.Splitter", NULL, NULL);
	
	CSplitCtrl();
	~CSplitCtrl();

	// CItemBase
	void ApplyStyle(LPCSTR strStyle);

protected:
	BEGIN_MSG_MAP(CSplitCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()
	
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	HCURSOR m_hCursor;
	POINT	m_ptStart;
};

//////////////////////////////////////////////////////////////////////
// CHexViewCtrl

class CHexViewCtrl : public CItemControl<CHexViewItem>
{
public:
	typedef CItemControl<CHexViewItem> TBase;
	DECLARE_WND_AVPCLASS("AVP.HexViewer", NULL, NULL);
	
	CHexViewCtrl();

protected:
	BEGIN_MSG_MAP(CHexViewCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWhell)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnDlgCode)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWhell(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	// virtual overwrite CHexViewItem
	void UpdateScrollBars();
	void UpdateScrollBarPos();

};


/////////////////////////////////////////////

#endif // !defined(AFX_WINCTRLS_H__5401AE0F_38DD_4399_883A_F072CC926781__INCLUDED_)
