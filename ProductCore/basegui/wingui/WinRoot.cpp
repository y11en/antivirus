// WinRoot.cpp: implementation of the CWinRoot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinRoot.h"
#include <shellapi.h>
#include <shlobj.h>
#include <uxtheme.h>
#include <tmschema.h>
#include <locale>
#include <process.h>
#include <wtsapi32.h>
#include <dbt.h>
#include <Prague/plugin\p_win32_reg.h>
#include <Prague/iface\i_csect.h>
#include "SendMail.h"
#include "ShellCtrls.h"
#include "SysSink.h"
#include "..\SysSink.h"

#include <Objsel.h>
#include <Sddl.h>

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/getsystemmetrics.asp
#ifndef SM_MEDIACENTER
#define SM_MEDIACENTER 87
#endif

#ifndef SM_STARTER
#define SM_STARTER 88
#endif

#ifndef SM_TABLETPC
#define SM_TABLETPC 86
#endif

#ifndef SM_SERVERR2
#define SM_SERVERR2 89
#endif

#ifndef VER_SUITE_COMPUTE_SERVER
#define VER_SUITE_COMPUTE_SERVER 0x00004000
#endif

#ifndef VER_SUITE_STORAGE_SERVER
#define VER_SUITE_STORAGE_SERVER 0x00002000
#endif

//////////////////////////////////////////////////////////////////////

HINSTANCE    g_hInstance;
HCURSOR      g_curHandId;
HCURSOR      g_curTrackCol;
BOOL         g_bWOW64;

bool GetAppInfoByPid(CRootItem * pRoot, tDWORD nPID, CObjectInfo * pObjInfo);

//////////////////////////////////////////////////////////////////////
// CWinRoot

SINK_MAP_BEGINEX(CWinRoot)
	SINK_DYNAMIC_EX("regions",    GUI_ITEMT_COMBO,    CLocaleRegionsCombo())
	SINK_DYNAMIC_EX("cdrdrives",  GUI_ITEMT_COMBO,    CStdinRedirected())
	SINK_DYNAMIC_EX("procoutput", GUI_ITEMT_EDIT,     CStdinRedirected())
	SINK_DYNAMIC_EX("affinity",   GUI_ITEMT_LISTCTRL, CProcessorList())
	SINK_DYNAMIC_EX("shelltree",  GUI_ITEMT_TREE,     CShellTreeSink())
	SINK_DYNAMIC_EX("regtree",    GUI_ITEMT_TREE,     CRegTreeSink())
SINK_MAP_END(CRootItem)

//////////////////////////////////////////////////////////////////////

CWinRoot::CWinRoot(tDWORD nFlags, CRootSink * pSink) :
	CRootItem(nFlags, pSink),
	
	m_nThreadID(-1),
	
	m_Theme(_T("uxtheme.dll")),
	_OpenThemeData(NULL), _CloseThemeData(NULL), _DrawThemeBackground(NULL),
	_EnableThemeDialogTexture(NULL), _DrawThemeParentBackground(NULL),
	
	m_User(_T("user32.dll")),
	_SetLayeredWindowAttributes(NULL), _GetLastInputInfo(NULL),
	
	m_WinMM(_T("winmm.dll")),
	_PlaySound(NULL),
	m_hMsgHook(NULL),
	m_hFilterHook(NULL),

// 	m_VistaDWMLib(_T("dwmapi.dll")),
// 	_DwmIsCompositionEnabled(NULL), _DwmEnableBlurBehindWindow(NULL),
	
	m_hDesktopDC(false),
	
	m_nCharSet(DEFAULT_CHARSET),
	m_nCodePage(GetACP())
{
	memset(&m_osVer, 0, sizeof(m_osVer));
	m_osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( !::GetVersionEx((OSVERSIONINFO*)&m_osVer) )
	{
		m_osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx((OSVERSIONINFO*)&m_osVer);
	}

	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::OsVer: %u.%u, ProductType: %u, SuiteMask: 0x%04X", m_osVer.dwMajorVersion, m_osVer.dwMinorVersion, m_osVer.wProductType, m_osVer.wSuiteMask));

	m_nOsVer = GetSysVersion(tString());

	{
		DWORD dwVer[2];
		if( FAILED(AtlGetCommCtrlVersion(&dwVer[0], &dwVer[1])) )
		{
			dwVer[0] = 0;
			dwVer[1] = 0;
		}

		m_nComCtlVer = MAKELONG(dwVer[1], dwVer[0]);
	}

	m_wndAtom = NULL;
	m_nTimerSpin = 0;
	m_hToolTip = NULL;
	m_hBalloon = NULL;
	m_nThemed = 2;
	m_hThemeButton = NULL;
	m_hThemeScroll = NULL;
	m_hThemeHeader = NULL;
	m_hThemeHeader = NULL;

	if( !_SetLayeredWindowAttributes )
		m_User.GetFunc((FARPROC*)&_SetLayeredWindowAttributes, "SetLayeredWindowAttributes");
	if( !_GetLastInputInfo )
		m_User.GetFunc((FARPROC*)&_GetLastInputInfo, "GetLastInputInfo");
    
	CWindowL::Init(m_nOsVer);
}

CWinRoot::~CWinRoot()
{
}

LRESULT CWinRoot::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG pMsg = (PMSG)lParam;
	
	if( nCode == HC_ACTION && pMsg->message == WM_MOUSEWHEEL )
	{
		HWND hWnd = WindowFromPoint(pMsg->pt);
		if( hWnd && hWnd != pMsg->hwnd )
		{
			DWORD nPid;
			if( m_nThreadID == GetWindowThreadProcessId(hWnd, &nPid))
			{
				pMsg->message = WM_NULL;
				::PostMessage(hWnd, WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
			}
		}
	}
	return CallNextHookEx(m_hMsgHook, nCode, wParam, lParam);
}

LRESULT CWinRoot::MsgFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;
	
	if( nCode == HC_ACTION && pMsg->message == WM_DESTROY )
	{
		if( pMsg->hwnd == m_LoopStack.m_hWndLoopParent )
		{
			if( m_LoopStack.m_bMenuShowing )
				::EndMenu();
			
			if( m_LoopStack.m_pLoopItem && !m_LoopStack.m_pLoopItem->GetWindow() )
			{
				m_LoopStack.m_pLoopItem->m_pRoot = NULL;
				::PostMessage(m_LoopStack.m_hWndLoopParent, WM_NULL, 0, 0);
			}
		}
	}	
	return CallNextHookEx(m_hFilterHook, nCode, wParam, lParam);
}

static CTls g_pWinRoot;

LRESULT CALLBACK GetMsgProcLink(int code, WPARAM wParam, LPARAM lParam)
{
	CWinRoot * pRoot = (CWinRoot *)(tPTR)g_pWinRoot;
	return pRoot ? pRoot->GetMsgProc(code, wParam, lParam) : 0;
}

LRESULT CALLBACK GetMsgFilterLink(int code, WPARAM wParam, LPARAM lParam)
{
	CWinRoot * pRoot = (CWinRoot *)(tPTR)g_pWinRoot;
	return pRoot ? pRoot->MsgFilterProc(code, wParam, lParam) : 0;
}

bool CWinRoot::ActivateData(bool bActive)
{
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::ActivateData. this: %08x, thread: %08x, activate: %d", this, ::GetCurrentThreadId(), (tDWORD)bActive));

	bool bSetWindowsHook = m_nGuiFlags & GUIFLAG_OWNTHREAD && !(m_nGuiFlags & GUIFLAG_ADMIN);

	if( bActive )
	{
		m_nThreadID = ::GetCurrentThreadId();
		
		if( bSetWindowsHook )
		{
			g_pWinRoot = this;
			m_hMsgHook = ::SetWindowsHookEx(WH_GETMESSAGE, GetMsgProcLink, NULL, m_nThreadID);
			m_hFilterHook = ::SetWindowsHookEx(WH_CALLWNDPROC,  GetMsgFilterLink, NULL, m_nThreadID);
		}

		m_hDesktopDC.Create();
		m_dwScaleY = GetDeviceCaps(m_hDesktopDC, LOGPIXELSY);
		AdjustSize(DRAW_CHKRAD_SIZE);
		AdjustSize(DRAW_ICON_MARGIN);
		AdjustSize(LIST_ICON_SIZE);
		AdjustSize(LIST_CELL_MARGIN);
		AdjustSize(LIST_COL_DEFSIZE);

		::CoInitialize(NULL);

		{
			CWndClassInfo ci(GetWndClassInfo());
			
			tString strClass;
//			strClass.resize(100); _snprintf(&strClass[0], strClass.size(), "%I64u", (tQWORD)this);
//			strClass.resize(strlen(strClass.c_str()));
			
//#ifdef _DEBUG
			strClass.append("____AVP.Root");
//#endif // _DEBUG
			
			ci.m_wc.lpszClassName = strClass.c_str();
			m_wndAtom = ci.Register(&m_pfnSuperWindowProc);
		}

		CWindowImplBaseT<CWindow, CWinTraits<0, 0> >::Create(GetDesktopWindow(), CWindow::rcDefault, NULL,
			WS_POPUP, 0, (UINT)0, m_wndAtom, NULL);
		
		m_idTimerRefresh = SetTimer(1, TIMER_REFRESH_INT);

		if( !!m_hEvent )
			RegisterNotifications();

		OnActivate(this, true);
		return !!m_hWnd;
	}

	if( bSetWindowsHook )
	{
		::UnhookWindowsHookEx(m_hMsgHook);
		::UnhookWindowsHookEx(m_hFilterHook);
		g_pWinRoot = NULL;
	}

	OnActivate(this, false);

	CRootItem::Clear();
	ClearWinResources();

	if( ::IsWindow(m_hWnd) )
	{
		KillTimer(m_idTimerRefresh);
		KillTimer(m_idTimerAnimation);
		DestroyWindow();
	}
	else
		PR_TRACE((g_root, prtERROR, "GUI(Win)::ActivateData. deactivating without destroing window (already destroyed)"));
	
	m_hWnd = NULL;
	
	::UnregisterClass((LPCTSTR)m_wndAtom, GetWndClassInfo().m_wc.hInstance), m_wndAtom = NULL;
	::CoUninitialize();
	return true;
}

bool CWinRoot::IsOwnThread()
{
	return ::GetCurrentThreadId() == m_nThreadID;
}

bool CWinRoot::SetAnimationTimer(tDWORD nTimeout)
{
	if( nTimeout )
		m_idTimerAnimation = SetTimer(2, nTimeout);
	else
		KillTimer(m_idTimerAnimation);
	return true;
}

void CWinRoot::MessageLoop(CItemBase* pItem)
{
	CItemBase *pOwner = pItem ? pItem->GetOwner(true) : NULL;
	cLoopStack pOldParams(this, pOwner ? pOwner->GetWindow() : NULL, false, pItem);

	MSG msg;
	while( m_bActive )
	{ 
		GetMessage(&msg, NULL, 0, 0);
		if( msg.message == WM_QUIT && m_bActive )
			continue;

		if( !PreTranslateMessage(msg) )
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}

		if( m_bRefreshRequest )
		{
			if( !pItem )
			{
				RefreshDialog(NULL);
				m_bRefreshRequest = false;
			}
			else if( pItem->m_nFlags & STYLE_TOP_ITEM )
			{
				((CDialogItem*)pItem)->Close();
			}
		}

		if( pItem && (!pItem->m_pRoot) )
			break;
	}
}

//////////////////////////////////////////////////////////////////////

void CWinRoot::InitThemes()
{
	if( m_nThemed == 0 )
		return;
	
	if( m_nThemed == 1 && m_hThemeButton )
		return;

	if( m_nThemed == 2 )
	{
		DeinitThemes(false);
		// [14/2/2007 Lebedev]
		// ¬ Outlook на XP груз€тс€ 2 comctl32.dll, одна из WinSxS версии 6.0,
		// друга€ из System32 версии 5.82
		// ¬ этом случае m_nComCtlVer вычисл€етс€ как 5.82 и темы не работают
		/*
		m_nThemed = (m_nComCtlVer >= COMCTL_VER(6, 0)) ? 1 : 0;
		*/
		m_nThemed = 1;
	}
	
	if( m_nThemed )
	{
		m_Theme.GetFunc((FARPROC*)&_OpenThemeData,             "OpenThemeData");
		m_Theme.GetFunc((FARPROC*)&_CloseThemeData,            "CloseThemeData");
		m_Theme.GetFunc((FARPROC*)&_DrawThemeBackground,       "DrawThemeBackground");
		m_Theme.GetFunc((FARPROC*)&_DrawThemeParentBackground, "DrawThemeParentBackground");
		m_Theme.GetFunc((FARPROC*)&_EnableThemeDialogTexture,  "EnableThemeDialogTexture");

		if( _OpenThemeData )
		{
			m_hThemeButton = _OpenThemeData(m_hWnd, L"Button");
			m_hThemeScroll = _OpenThemeData(m_hWnd, L"Scrollbar");
			m_hThemeHeader = _OpenThemeData(m_hWnd, L"Header");
			m_hThemeGlobal = _OpenThemeData(m_hWnd, L"Global");
		}
	
		m_nThemed = _OpenThemeData && _DrawThemeBackground && m_hThemeButton && m_hThemeScroll;

	}

	if( !m_nThemed )
		DeinitThemes();
}

// void CWinRoot::InitDWM()
// {
// //Vista
// 	m_VistaDWMLib.GetFunc((FARPROC*)&_DwmIsCompositionEnabled,   "DwmIsCompositionEnabled");
// 	m_VistaDWMLib.GetFunc((FARPROC*)&_DwmEnableBlurBehindWindow, "DwmEnableBlurBehindWindow");
// 	m_VistaDWMLib.GetFunc((FARPROC*)&_DwmExtendFrameIntoClientArea, "DwmExtendFrameIntoClientArea");
// }


void CWinRoot::DeinitThemes(bool bFull)
{
	if( _CloseThemeData )
	{
		if( m_hThemeButton ) _CloseThemeData( m_hThemeButton ), m_hThemeButton = NULL;
		if( m_hThemeScroll ) _CloseThemeData( m_hThemeScroll ), m_hThemeScroll = NULL;
		if( m_hThemeHeader ) _CloseThemeData( m_hThemeHeader ), m_hThemeHeader = NULL;
		if( m_hThemeGlobal ) _CloseThemeData( m_hThemeGlobal ), m_hThemeGlobal = NULL;
	}

	if( !bFull )
		return;
	
	_OpenThemeData = NULL;
	_CloseThemeData = NULL;
	_DrawThemeBackground = NULL;
	_DrawThemeParentBackground = NULL;
	_EnableThemeDialogTexture = NULL;


	m_Theme.Close();
}

// void CWinRoot::DeinitDWM()
// {
// //Vista
// 	_DwmIsCompositionEnabled = NULL;
// 	_DwmEnableBlurBehindWindow = NULL;
// 	_DwmExtendFrameIntoClientArea = NULL;
// 	m_VistaDWMLib.Close();
// }

// bool CWinRoot::IsCompositionEnabled()
// {
// 	InitDWM();
// 	BOOL bEnabled = FALSE;
// 	return _DwmIsCompositionEnabled && _DwmEnableBlurBehindWindow && _DwmExtendFrameIntoClientArea && SUCCEEDED(_DwmIsCompositionEnabled(&bEnabled)) && bEnabled;
// }
// 
// HRESULT CWinRoot::EnableBlurBehindWindow(HWND window, bool enable, HRGN region, bool transitionOnMaximized)
// {
// 	if(!IsCompositionEnabled())
// 		return E_FAIL;
// 
//     DWM_BLURBEHIND blurBehind = { 0 };
//     
//     blurBehind.dwFlags = DWM_BB_ENABLE | DWM_BB_TRANSITIONONMAXIMIZED;
//     blurBehind.fEnable = enable;
//     blurBehind.fTransitionOnMaximized = transitionOnMaximized;
// 
//     if (enable && 0 != region)
//     {
//         blurBehind.dwFlags |= DWM_BB_BLURREGION;
//         blurBehind.hRgnBlur = region;
//     }
// 
// 	return _DwmEnableBlurBehindWindow(window, &blurBehind);
// }



bool CWinRoot::IsThemed()
{
	InitThemes();
	return !!m_nThemed;
}

bool CWinRoot::DrawCheckBox(HDC dc, RECT& rcRect, bool bChecked, tDWORD nState, bool bFlat)
{
	if( IsThemed() )
	{
		int iStateId;
		if( bChecked )
		{
			if( nState & ISTATE_DISABLED )
				iStateId = CBS_CHECKEDDISABLED;
			else if( nState & ISTATE_SELECTED )
				iStateId = CBS_CHECKEDPRESSED;
			else if( nState & ISTATE_HOTLIGHT )
				iStateId = CBS_CHECKEDHOT;
			else
				iStateId = CBS_CHECKEDNORMAL;
		}
		else
		{
			if( nState & ISTATE_DISABLED )
				iStateId = CBS_UNCHECKEDDISABLED;
			else if( nState & ISTATE_SELECTED )
				iStateId = CBS_UNCHECKEDPRESSED;
			else if( nState & ISTATE_HOTLIGHT )
				iStateId = CBS_UNCHECKEDHOT;
			else
				iStateId = CBS_UNCHECKEDNORMAL;
		}
		
		return !!_DrawThemeBackground(m_hThemeButton, dc, BP_CHECKBOX, iStateId, &rcRect, NULL);
	}
 
	RECT rcCheck = {0, 0, 13, 13};
	::OffsetRect(&rcCheck,
		rcRect.left + (RECT_CX(rcRect) - rcCheck.right)/2,
		rcRect.top + (RECT_CY(rcRect) - rcCheck.bottom)/2);
	
	UINT iState = DFCS_BUTTONCHECK|(bFlat ? DFCS_FLAT : 0)|(bChecked ? DFCS_CHECKED : 0);
	if( nState & ISTATE_DISABLED )
		iState |= DFCS_INACTIVE;
	if( nState & ISTATE_SELECTED )
		iState |= DFCS_PUSHED;
	if( nState & ISTATE_HOTLIGHT )
		iState |= DFCS_HOT;
	
	return !!::DrawFrameControl(dc, &rcCheck, DFC_BUTTON, iState);
}

bool CWinRoot::DrawRadioBox(HDC dc, RECT& rcRect, bool bChecked, tDWORD nState, bool bFlat)
{
	if( IsThemed() )
	{
		int iStateId;
		if( bChecked )
		{
			if( nState & ISTATE_DISABLED )
				iStateId = RBS_CHECKEDDISABLED;
			else if( nState & ISTATE_SELECTED )
				iStateId = RBS_CHECKEDPRESSED;
			else if( nState & ISTATE_HOTLIGHT )
				iStateId = RBS_CHECKEDHOT;
			else
				iStateId = RBS_CHECKEDNORMAL;
		}
		else
		{
			if( nState & ISTATE_DISABLED )
				iStateId = RBS_UNCHECKEDDISABLED;
			else if( nState & ISTATE_SELECTED )
				iStateId = RBS_UNCHECKEDPRESSED;
			else if( nState & ISTATE_HOTLIGHT )
				iStateId = RBS_UNCHECKEDHOT;
			else
				iStateId = RBS_UNCHECKEDNORMAL;
		}
		
		return !!_DrawThemeBackground(m_hThemeButton, dc, BP_RADIOBUTTON, iStateId, &rcRect, NULL);
	}
 
	RECT rcCheck = {0, 0, 12, 12};
	::OffsetRect(&rcCheck,
		rcRect.left + (RECT_CX(rcRect) - rcCheck.right)/2,
		rcRect.top + (RECT_CY(rcRect) - rcCheck.bottom)/2);
	
	UINT iState = DFCS_BUTTONRADIO|(bFlat ? DFCS_FLAT : 0)|(bChecked ? DFCS_CHECKED : 0);
	if( nState & ISTATE_DISABLED )
		iState |= DFCS_INACTIVE;
	if( nState & ISTATE_SELECTED )
		iState |= DFCS_PUSHED;
	if( nState & ISTATE_HOTLIGHT )
		iState |= DFCS_HOT;
	
	return !!::DrawFrameControl(dc, &rcCheck, DFC_BUTTON, iState);
}

bool CWinRoot::DrawDlgGripper(HDC dc, RECT& rcRect)
{
	if( IsThemed() )
		return !!_DrawThemeBackground(m_hThemeScroll, dc, SBP_SIZEBOX, SZB_RIGHTALIGN, &rcRect, NULL);
 
	return !!::DrawFrameControl(dc, &rcRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}

bool CWinRoot::DrawThemeBorder(HDC dc, RECT& rcRect)
{
	RECT rcExcl = rcRect; ::InflateRect(&rcExcl, -1, -1);
	::ExcludeClipRect(dc, rcExcl.left, rcExcl.top, rcExcl.right, rcExcl.bottom);
	::DrawEdge(dc, &rcRect, EDGE_SUNKEN, BF_FLAT|BF_RECT);
	::SelectClipRgn(dc, NULL);
	return true;
}

bool CWinRoot::DrawColumnHeader(HDC dc, RECT& rcRect, tDWORD flags)
{
	if( IsThemed() )
	{
		int iStateId, iPartId = HP_HEADERITEM;

		if( flags & drHotlight )
			iStateId = HIS_HOT;
		else if( flags & drPressed )
			iStateId = HIS_PRESSED;
		else
			iStateId = HIS_NORMAL;
		
		return !!_DrawThemeBackground(m_hThemeHeader, dc, iPartId, iStateId, &rcRect, NULL);
	}

	if( !(flags & (drSortUp|drSortDown)) ) 
		::DrawFrameControl(dc, &rcRect, DFC_BUTTON, DFCS_BUTTONPUSH);
	return true;
}

bool CWinRoot::DrawRect(HDC dc, RECT& rcRect, tDWORD flags, tDWORD state)
{
	if( flags & drCheckBox )
	{
		tDWORD nState = 0;
		if( flags & drDisabled )
			nState |= ISTATE_DISABLED;
		else if( flags & drHotlight )
			nState |= ISTATE_HOTLIGHT;

		return DrawCheckBox(dc, rcRect, !!(flags & drPressed), nState, !!(flags & drFlat));
	}

	if( flags & drColHeader )
		return DrawColumnHeader(dc, rcRect, flags);

	if( flags & drBorder )
	{
		if( IsThemed() )
			return !!_DrawThemeBackground(m_hThemeGlobal, dc, 1, 1, &rcRect, NULL);
		return false;
	}

	if( flags & drSelected )
	{
		if( state & ISTATE_FOCUSED )
			::FillRect(dc, &rcRect, GetSysColorBrush(COLOR_HIGHLIGHT));
		else
			::FillRect(dc, &rcRect, GetSysColorBrush(COLOR_BTNFACE));
	}						

	if( flags & drFocused )
	{
		::SetTextColor(dc, 0);
		::DrawFocusRect(dc, &rcRect);
	}

	if( flags & drInvert )
		::InvertRect(dc, &rcRect);

	return true;
}

bool CWinRoot::DragImage(CItemBase* pItem, CImageBase* pImage, void*& ctx, POINT& ptt, enDragType type, tDWORD ipos)
{
	struct CDragImage
	{
		HDC			m_cdc;
		HBITMAP		m_bmp_save;
		int			m_x, m_y;
		BOOL		m_erase;
	};

	CDragImage*& c = (CDragImage*&)ctx;
	if( type == dtEnd )
	{
		if( !c ) return true;
		DragImage(pItem, pImage, ctx, ptt, dtHide);

		if( c->m_bmp_save ) DeleteObject(c->m_bmp_save);
		if( c->m_cdc ) DeleteDC(c->m_cdc);
		delete c;
		c = NULL;
		return true;
	}

	HDC dc = GetDesktopDC();
	SIZE& sz = pImage->Size();
	long cx = sz.cx, cy = sz.cy;

	if( !c )
	{
		c = new CDragImage();
		c->m_cdc = CreateCompatibleDC(dc);
		c->m_bmp_save = CreateCompatibleBitmap(dc, cx, cy);
		c->m_erase = FALSE;
	}

	HGDIOBJ old = SelectObject(c->m_cdc, c->m_bmp_save);
	if( type == dtHide )
	{
		if( c->m_erase )
		{
			BitBlt(dc, c->m_x, c->m_y, cx, cy, c->m_cdc, 0, 0, SRCCOPY);
			c->m_erase = FALSE;
		}
	}
	else
	{
		POINT pt = ptt;
		HWND wnd = pItem->GetWindow();
		if( wnd )
			::ClientToScreen(wnd, &pt);

		long p_x = pt.x, p_y = pt.y;

		if( type == dtShow )
			p_x = c->m_x, p_y = c->m_y;
		else
			p_x -= cx/2, p_y -= cy/2;

		if( !c->m_erase )
		{
			BitBlt(c->m_cdc, 0, 0, cx, cy, dc, p_x, p_y, SRCCOPY);
			c->m_erase = TRUE;
		}
		else
		{
			ExcludeClipRect(dc, p_x, p_y, p_x + cx, p_y + cy);
			BitBlt(dc, c->m_x, c->m_y, cx, cy, c->m_cdc, 0, 0, SRCCOPY);
			SelectClipRgn(dc, NULL);

			int dx = c->m_x - p_x, dy = c->m_y - p_y;
			RECT rs = { 0, 0, cx, cy };
			ScrollDC(c->m_cdc, dx, dy, &rs, NULL, NULL, NULL);
			ExcludeClipRect(c->m_cdc, dx, dy, dx + cx, dy + cy);
			BitBlt(c->m_cdc, 0, 0, cx, cy, dc, p_x, p_y, SRCCOPY);
			SelectClipRgn(c->m_cdc, NULL);
		}

		RECT rc = {p_x, p_y, p_x+cx, p_y+cy};
		pImage->Draw(dc, rc, NULL, ipos);

		c->m_x = p_x;	c->m_y = p_y;
	}

	SelectObject(c->m_cdc, old);
	return true;
}

bool CWinRoot::EnableThemeDialogTexture(HWND hWnd, bool bForTab, bool bEnable)
{
	if( !_OpenThemeData || !_EnableThemeDialogTexture )
		return false;
	
	DWORD nFlags = (bEnable ? ETDT_ENABLE : ETDT_DISABLE);
	if( bForTab )
	{
		HTHEME hThemeTab = _OpenThemeData(hWnd, L"Tab");
		if( !hThemeTab )
			return false;
		
		if( _CloseThemeData )
			_CloseThemeData(hThemeTab);

		nFlags |= ETDT_USETABTEXTURE;
	}
	return !_EnableThemeDialogTexture(hWnd, nFlags);
}

bool CWinRoot::SetLayeredWindowAttributes(HWND hWnd, BYTE nAlpha)
{
	if( !_SetLayeredWindowAttributes )
		return false;
	if( !_SetLayeredWindowAttributes(hWnd, 0, nAlpha, LWA_ALPHA) )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CWinRoot::PreTranslateMessage(MSG& msg)
{
	// Test dialogs
	if( msg.message == WM_KEYDOWN && ( msg.wParam >= VK_NUMPAD0 && msg.wParam <= VK_NUMPAD9 ) && HIWORD(::GetKeyState(VK_CONTROL)) )
	{
		CItemBase::SendEvent(pmTEST_GUI + (tDWORD)msg.wParam - VK_NUMPAD0, NULL, false);
		return true;
	}

	if( msg.message == WM_KEYDOWN && msg.wParam == VK_F4 && ::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState(VK_SHIFT) < 0 )
	{
		CSkinDebugWindow * pSink = new CSkinDebugWindow();
		pSink->Show(m_pRoot);
		return true;
	}

	if( msg.message == WM_MOUSEMOVE && m_pDebugWnd )
	{
		if( CItemBase * pItem = GetItem(msg.hwnd) )
			if( CItemBase * pParent = pItem->GetOwner(true) )
			{
				POINT pt = {GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)};
				::GetCursorPos(&pt); ::ScreenToClient(pParent->GetWindow(), &pt);
				m_pRoot->m_pDebugWnd->ProcessPoint(pt, pParent);
			}
	}

	for(size_t i = 0; i < m_vecDialogs.size(); i++)
	{
		CDialogItem* pChild = m_vecDialogs[i];
		if( !(pChild->m_nFlags & STYLE_WINDOW) )
			continue;

		if( pChild->m_pUserData )
		{
			tSyncEventData* pData = (tSyncEventData*)pChild->m_pUserData;
			if( pData->errResult == errTIMEOUT )
				pChild->Close();
		}

		if( ((CDialogItemImpl*)pChild)->PreTranslateMessage(msg) )
			return true;
	}
	return false;
}

void CWinRoot::RegisterNotifications()
{
	HMODULE hMod = LoadLibrary("wtsapi32.dll");

	BOOL (WINAPI* fnWTSRegisterSessionNotification)(HWND hWnd, DWORD dwFlags);
	*(void**)&fnWTSRegisterSessionNotification = hMod ? GetProcAddress(hMod, "WTSRegisterSessionNotification") : NULL;

	if( fnWTSRegisterSessionNotification )
		fnWTSRegisterSessionNotification(m_hWnd, NOTIFY_FOR_THIS_SESSION);
}

void CWinRoot::PostAction(tWORD nActionId, CItemBase* pItem, void * pParam, bool bSync)
{
	bool bItem = !!pItem;
	if( bItem && !bSync )
		pItem->AddRef();
	
	WPARAM wParam = MAKELONG(nActionId, MAKEWORD(bItem, bSync));
	LPARAM lParam = bItem ? (LPARAM)pItem : (LPARAM)pParam;
	
	if( bSync )
		SendMessage(WM_ONACTION, wParam, lParam);
	else
		PostMessage(WM_ONACTION, wParam, lParam);
}

void CWinRoot::SwapMemory()
{
	::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);
}

HDC CWinRoot::GetDesktopDC()
{
	return m_hDesktopDC;
}

HDC CWinRoot::GetTempDC(RECT& rcFrame, RECT* rcUpdate)
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

void CWinRoot::CopyTempDC(HDC hTempDC, HDC dc, RECT& rcFrame)
{
	::BitBlt(dc, rcFrame.left, rcFrame.top, RECT_CX(rcFrame), RECT_CY(rcFrame),
		hTempDC, rcFrame.left, rcFrame.top, SRCCOPY);
	::SelectClipRgn(hTempDC, NULL);
}

void CWinRoot::ClearResources(tDWORD nFlags)
{
	CRootItem::ClearResources(nFlags);
	if( nFlags == RES_ALL )
		ClearWinResources();
}

void CWinRoot::ClearWinResources()
{
	if( m_hTmpDC )
	{
		::SelectObject(m_hTmpDC, m_hTmpOldBmp);
		m_hTmpDC.Cleanup();
	}

	if( m_hTmpBmp )
		m_hTmpBmp.Cleanup();
	m_szTmpBmp.cx = m_szTmpBmp.cy = 0;

	DeinitThemes();

//	DeinitDWM();

	if( m_hToolTip )
		::DestroyWindow(m_hToolTip), m_hToolTip = NULL;

	if( m_hBalloon )
		::DestroyWindow(m_hBalloon), m_hBalloon = NULL;

	shell_FreeMalloc();
}

void CWinRoot::ClipRect(HDC dc, RECT* rc, bool bExclude)
{
	if( !rc )
		::SelectClipRgn(dc, NULL);
	else if( bExclude )
		::ExcludeClipRect(dc, rc->left, rc->top, rc->right, rc->bottom);
	else
		::IntersectClipRect(dc, rc->left, rc->top, rc->right, rc->bottom);
}

COLORREF CWinRoot::GetSysColor(LPCSTR strColor)
{
	int nIdx;
	sswitch( strColor )
		stcase( SYS_3DFACE ) nIdx = COLOR_3DFACE;	sbreak;
		stcase( SYS_WINDOW ) nIdx = COLOR_WINDOW;	sbreak;
		stcase( SYS_HIGHLIGHT ) nIdx = COLOR_HIGHLIGHT;	sbreak;
		sdefault() return -1; sbreak;
	send;
	return ::GetSysColor(nIdx);
}

tDWORD CWinRoot::GetSysVersion(tString& strVersion, tDWORD * nSystemInfo, tBOOL * bWinPE)
{
	tDWORD  nOSVer = 0;
	tString strOSName;
	tDWORD  nBuild = m_osVer.dwBuildNumber;
	bool    bX64 = false;
	
	{
		SYSTEM_INFO _sysInfo;
		_sysInfo.wProcessorArchitecture = PROCESSOR_ARCHITECTURE_UNKNOWN;

		VOID (WINAPI * _GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo) = NULL;
		CLibWrapper _Kernel32("kernel32.dll"); _Kernel32.GetFunc((FARPROC *)&_GetNativeSystemInfo, "GetNativeSystemInfo");
		
		if( _GetNativeSystemInfo )
			_GetNativeSystemInfo(&_sysInfo);
		else
			::GetSystemInfo(&_sysInfo);
		
		switch( _sysInfo.wProcessorArchitecture )
		{
		case PROCESSOR_ARCHITECTURE_IA64:
		case PROCESSOR_ARCHITECTURE_ALPHA64:
		case PROCESSOR_ARCHITECTURE_AMD64:
			bX64 = true;
			break;
		}

		if( nSystemInfo )
			*nSystemInfo = (tDWORD)_sysInfo.wProcessorArchitecture;
	}
	
	char strCSDVersion[countof(m_osVer.szCSDVersion)];
	memcpy(strCSDVersion, m_osVer.szCSDVersion, sizeof(strCSDVersion));

	switch(m_osVer.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		strOSName += _T("3.1");
		break;
	
	case VER_PLATFORM_WIN32_WINDOWS:
		nBuild = LOWORD(m_osVer.dwBuildNumber);
		switch(m_osVer.dwMajorVersion)
		{
		case 4:
			switch(m_osVer.dwMinorVersion)
			{
			case 0:  strOSName += _T("95"); nOSVer |= GUI_OSVER_WIN95; break;
			case 10: strOSName += _T("98"); nOSVer |= GUI_OSVER_WIN98; break;
			case 90: strOSName += _T("Me"); nOSVer |= GUI_OSVER_WINME; break;
			}
			
			if( strCSDVersion[1] == 'A' )
				strOSName += _T(" Second Edition");
			else if( strCSDVersion[1] == 'B' || strCSDVersion[1] == 'C' )
				strOSName += _T(" OSR2");

			*strCSDVersion = _T('\0');
			break;
		
		case 5:
			strOSName += _T("ME");
			break;
		}
		break;

	case VER_PLATFORM_WIN32_NT:
		switch(m_osVer.dwMajorVersion)
		{
		case 3:
			strOSName += _T("NT 3.51");
			nOSVer |= GUI_OSVER_WINNT351;
			break;
		case 4:
			strOSName += _T("NT 4.0");
			nOSVer |= GUI_OSVER_WINNT4;
			break;
		case 5:
			switch(m_osVer.dwMinorVersion)
			{
			case 0:
				strOSName += _T("2000");
				nOSVer |= GUI_OSVER_WIN2000;
				break;
			
			case 1:
				if( m_osVer.wProductType == VER_NT_WORKSTATION )
					strOSName += _T("XP");
				else
					strOSName += _T("2002");
				nOSVer |= GUI_OSVER_WINXP;
				break;
			
			case 2:
				if( m_osVer.wProductType == VER_NT_WORKSTATION )
					strOSName += _T("XP");
				else
				{
					strOSName += _T("2003");
					
					if( ::GetSystemMetrics(SM_SERVERR2) )
						strOSName += " R2";
				}
				
				nOSVer |= GUI_OSVER_WIN2003;
				break;
			}
			break;

		case 6:
			switch(m_osVer.dwMinorVersion)
			{
			case 0:
				if( m_osVer.wProductType == VER_NT_WORKSTATION )
					strOSName += _T("Vista");
				else
					strOSName += _T("\"Longhorn\"");
				nOSVer |= GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(m_osVer.dwMajorVersion, m_osVer.dwMinorVersion);
				break;
			}
			
			break;
		}

		if( strOSName.empty() )
		{
			char strUnknownVer[100];
			_snprintf(strUnknownVer, countof(strUnknownVer), "NT %u.%u", m_osVer.dwMajorVersion, m_osVer.dwMinorVersion);
			strOSName += strUnknownVer;
			
			nOSVer |= GUI_OSVER_FAMILY_NT|GUI_OSVER_VER_MAKE(m_osVer.dwMajorVersion, m_osVer.dwMinorVersion);
		}
		
		switch(m_osVer.wProductType)
		{
		case VER_NT_WORKSTATION:
			if( m_osVer.dwMajorVersion < 5 )
				strOSName += " Workstation";
			else if( m_osVer.dwMajorVersion == 5 )
			{
				if( m_osVer.wSuiteMask & VER_SUITE_PERSONAL )
					strOSName += " Home Edition";
				else if( m_osVer.wSuiteMask & VER_SUITE_EMBEDDEDNT )
					strOSName += " Embedded";
				else if( ::GetSystemMetrics(SM_MEDIACENTER) )
					strOSName += " Media Center Edition";
				else if( ::GetSystemMetrics(SM_STARTER) )
					strOSName += " Starter Edition";
				else if( ::GetSystemMetrics(SM_TABLETPC) )
					strOSName += " Tablet PC Edition";
				else
					strOSName += " Professional";
			}
			else if( m_osVer.dwMajorVersion == 6 )
			{
				if( m_osVer.wSuiteMask & VER_SUITE_PERSONAL )
					strOSName += " Home";
			}
			
			break;
		
		case VER_NT_DOMAIN_CONTROLLER:
		case VER_NT_SERVER:
			if( (nOSVer & GUI_OSVER_VER_MASK) < GUI_OSVER_VER_MAKE(5, 0) )
			{
				strOSName += " Server";
				
				if( m_osVer.wSuiteMask & VER_SUITE_ENTERPRISE )
					strOSName += " Enterprise Edition";
			}
			else if( (nOSVer & GUI_OSVER_VER_MASK) == GUI_OSVER_VER_MAKE(5, 0) )
			{
				if( m_osVer.wSuiteMask & VER_SUITE_DATACENTER )
					strOSName += " Datacenter";
				else if( m_osVer.wSuiteMask & VER_SUITE_ENTERPRISE )
					strOSName += " Advanced";
				else if( m_osVer.wSuiteMask & VER_SUITE_BLADE )
					strOSName += " Web";
				else
					strOSName += " Standart";
				
				strOSName += " Server";
			}
			else
			{
				if( m_osVer.wSuiteMask & VER_SUITE_STORAGE_SERVER )
					strOSName += " Storage";
				
				strOSName += " Server";

				if( m_osVer.wSuiteMask & VER_SUITE_ENTERPRISE )
					strOSName += " Enterprise Edition";
				else if( m_osVer.wSuiteMask & VER_SUITE_DATACENTER )
					strOSName += " Datacenter Edition";
				else if( m_osVer.wSuiteMask & VER_SUITE_BLADE )
					strOSName += " Web Edition";
				else if( m_osVer.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
					strOSName += " Compute Cluster Edition";
				else if( (m_osVer.wSuiteMask & (VER_SUITE_SMALLBUSINESS|VER_SUITE_SMALLBUSINESS_RESTRICTED)) == (VER_SUITE_SMALLBUSINESS|VER_SUITE_SMALLBUSINESS_RESTRICTED) )
					strOSName += " for Small Business Server";
				else
					strOSName += " Standart Edition";
			}

			if( m_osVer.wProductType == VER_NT_DOMAIN_CONTROLLER )
				strOSName += " Domain Controller";
			
			break;
		}
		
		break;
	}

	char strVerNumber[100], strVerBuild[100];
	if( strOSName.empty() )
		_snprintf(strVerNumber, countof(strVerNumber), "%u.%u", m_osVer.dwMajorVersion, m_osVer.dwMinorVersion);
	_snprintf(strVerBuild, countof(strVerBuild), "(build %u)", nBuild);
	
	strVersion = _T("Microsoft Windows");
	strVersion += " ";
	strVersion += strOSName.empty() ? strVerNumber : strOSName;
	
	if( bX64 )
		strVersion += " x64 Edition";
	
	if( strCSDVersion[0] )
	{
		strVersion += " ";
		strVersion += strCSDVersion;
	}
	
	strVersion += " ";
	strVersion += strVerBuild;
	
	if( bWinPE )
		*bWinPE = (tBOOL)(GetDriveType(NULL) == DRIVE_CDROM);
	
	return nOSVer;
}

bool CWinRoot::OpenShellObject(LPCWSTR strUrl, LPCWSTR strParams, LPCWSTR strDir, CItemBase * pParent, bool bSync)
{
	if( bSync )
	{
		SHELLEXECUTEINFOW _info; memset(&_info, 0, sizeof(_info));
		_info.cbSize = sizeof(_info);
		_info.fMask = SEE_MASK_NOCLOSEPROCESS;
		_info.nShow = SW_SHOWNORMAL;
		_info.lpFile = strUrl;
		_info.hwnd = pParent ? pParent->GetWindow() : NULL;
		_info.lpParameters = strParams;
		_info.lpDirectory = strDir;

		if( !::ShellExecuteExW(&_info) )
			return false;

		DWORD ec = 0;
		if( _info.hProcess )
		{
			::WaitForSingleObject(_info.hProcess, INFINITE);
			GetExitCodeProcess(_info.hProcess, &ec);
			::CloseHandle(_info.hProcess);
		}
		
		return ec == 0;
	}
	
	if( CWindowL::m_bAnsi )
	{
		CHAR BaseguiPath[MAX_PATH];
		if( !::GetModuleFileNameA(::GetModuleHandleA("basegui.ppl"), BaseguiPath, countof(BaseguiPath)) )
			return false;
		
		{
			CHAR * pLastSlash = strrchr(BaseguiPath, '\\');
			if( pLastSlash )
				*pLastSlash = 0;
		}

		USES_CONVERSION;
		tString str = "rundll32.exe basegui.ppl,ShellExecuter "; str += W2A(strUrl);

		STARTUPINFOA si = {sizeof(STARTUPINFOA), 0};
		PROCESS_INFORMATION pi;
		DWORD err = ERROR_SUCCESS;

		if( ::CreateProcessA(NULL, (LPSTR)str.c_str(), NULL, NULL, FALSE, 0, NULL, BaseguiPath, &si, &pi) )
		{
			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
		}
		else
			err = ::GetLastError();
		
		PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::OpenShellObject: CreateProcess(%s) err=0x%08x", str.c_str(), err));
		return err == ERROR_SUCCESS;
	}

	WCHAR BaseguiPath[MAX_PATH];
	if( !::GetModuleFileNameW(::GetModuleHandleW(L"basegui.ppl"), BaseguiPath, countof(BaseguiPath)) )
		return false;

	{
		WCHAR * pLastSlash = wcsrchr(BaseguiPath, L'\\');
		if( pLastSlash )
			*pLastSlash = 0;
	}

	cStrObj str = L"rundll32.exe basegui.ppl,ShellExecuter "; str += strUrl;

	STARTUPINFOW si = {sizeof(STARTUPINFOW), 0};
	PROCESS_INFORMATION pi;
	DWORD err = ERROR_SUCCESS;
	
	if( ::CreateProcessW(NULL, (LPWSTR)str.data(), NULL, NULL, FALSE, 0, NULL, BaseguiPath, &si, &pi) )
	{
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
	}
	else
		err = ::GetLastError();

	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::OpenShellObject: CreateProcess(%S) err=0x%08x", str.data(), err));
	return err == ERROR_SUCCESS;
}

bool CWinRoot::OpenURL(LPCSTR strURL, CItemBase * pParent)
{
	if( !strURL )
		return false;
	
	cStrObj szAddress;
	if( !strstr(strURL, "://") )
		szAddress += L"http://";
	szAddress += strURL;
	return OpenShellObject(szAddress.data(), NULL, NULL, pParent);
}

bool CWinRoot::OpenMailURL(LPCSTR strURL, CItemBase * pParent)
{
	cStringObj szAddress = L"mailto:"; szAddress += strURL;
	return OpenShellObject(szAddress.data(), NULL, NULL, pParent);
}

bool CWinRoot::GotoFile(LPCSTR strPath, CItemBase * pParent)
{
	tString sFolder = strPath;
	if( sFolder.empty() )
		return false;

	LPSTR strPrevPath = NULL;
	for(LPSTR strPathObj = &sFolder[0]; *strPathObj; strPathObj++)
	{
		if( *strPathObj != '/' && *strPathObj != '\\' )
			continue;

		*strPathObj = 0;
		if( !(::GetFileAttributes(sFolder.c_str()) & FILE_ATTRIBUTE_DIRECTORY) )
			break;

		*strPathObj = '\\';
		strPrevPath = strPathObj + 1;
	}

	if( !strPrevPath )
		return false;
	
	*strPrevPath = 0;
	// cCP_ANSI
	return OpenShellObject(cStringObj(sFolder.c_str()).data(), NULL, NULL, pParent);
}

bool CWinRoot::ExecProgram(LPCSTR strPath, LPCSTR strArgs)
{
	return !!_spawnlp(_P_NOWAIT, strPath, strPath, strArgs, NULL);
}

tERROR CWinRoot::SendMail(CSendMailInfo& pInfo, CItemBase * pParent)
{
	CMailSender * pSender = new CMailSender(this);
	if( !pSender )
		return errNOT_ENOUGH_MEMORY;
	
	HWND hWndParent = GetParentHWND(pParent, true);
	
	CItemBase _loop;
	_loop.m_pRoot = this;
	_loop.m_pParent = pParent;

	pSender->m_info = pInfo;
	if( hWndParent )
		pSender->m_loop = &_loop;

	if( !pSender->start() )
		return delete pSender, errNOT_OK;

	if( !hWndParent )
		return errOK;

	BOOL bIsParentEnabled = ::IsWindowEnabled(hWndParent);
	::EnableWindow(hWndParent, FALSE);
	
	m_pRoot->MessageLoop(&_loop);
	_loop.m_pParent = NULL;
	
	::EnableWindow(hWndParent, bIsParentEnabled);
	
	tERROR err = pSender->m_err;
	return delete pSender, err;
}

void CPlaySoundTask::do_work()
{
	if( !((CWinRoot *)m_pRoot)->_PlaySound )
	{
		((CWinRoot *)m_pRoot)->m_WinMM.GetFunc((FARPROC*)&((CWinRoot *)m_pRoot)->_PlaySound, CWindowL::m_bAnsi ? "PlaySoundA" : "PlaySoundW");
		if( !((CWinRoot *)m_pRoot)->_PlaySound )
			return;
	}

	LPVOID pszSound = (PVOID)m_strPath.data();
	if( CWindowL::m_bAnsi )
	{
		USES_CONVERSION;
		pszSound = (PVOID)W2A((LPCWSTR)pszSound);
	}

	((CWinRoot *)m_pRoot)->_PlaySound(NULL, 0, SND_PURGE); 
	((CWinRoot *)m_pRoot)->_PlaySound(pszSound, 0, SND_FILENAME|SND_ASYNC);
}

bool CWinRoot::DoSound(LPCSTR strSoundId)
{
	if( !strSoundId || !(*strSoundId) )
		return false;

	// bf 24836: Vista Logo: Test 8
	// If the application uses sounds, it must not be heard in another user's session
	// while logged on through the Fast User Switching method in order to pass this test case.
	if (!m_bGuiActive)
		return true;

	cStrObj strPath;
	{
		cStrObj strSoundPath;
		strSoundPath += L"sounds/"; strSoundPath += strSoundId;
		
		cAutoObj<cIO> pIO;
		if( !m_pSkinAlt.m_pSkinPtr || PR_FAIL(m_pSkinAlt.m_pSkinPtr->IOCreate(&pIO, cAutoString(strSoundPath), fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
			strPath.assign(m_pSkin.m_pSkinPtr, pgOBJECT_PATH);
		else
			strPath.assign(m_pSkinAlt.m_pSkinPtr, pgOBJECT_PATH);
		strPath += strSoundPath;
	}

	CPlaySoundTask * pTask = new CPlaySoundTask(this, strPath.data());
	return pTask->start();
}

bool CWinRoot::InitDrivesList()
{
	m_setDrives.clear();

	int nStrSize = ::GetLogicalDriveStrings(0, NULL);
	if( !nStrSize )
		return false;

	tString strDrives; strDrives.resize(nStrSize + 2);
	::GetLogicalDriveStrings(nStrSize, &strDrives[0]);

	for(LPSTR pDrv = &strDrives[0]; *pDrv; pDrv = pDrv + strlen(pDrv) + 1)
		m_setDrives.push_back(pDrv);

	return true;
}

bool CWinRoot::EnumDrives(tString& strDrive, tDWORD* pCookie)
{
	cAutoCS locker(m_accessor, true);

	tDWORD pos = *pCookie;
	if( !pos )
		InitDrivesList();

	if( pos >= m_setDrives.size() )
		return false;

	strDrive = m_setDrives[pos];
	(*pCookie)++;

	if( GetDriveType(strDrive.c_str()) == DRIVE_REMOTE )
		return EnumDrives(strDrive, pCookie);

	return true;
}

static bool VerQueryValue(PVOID pVersion, LPCSTR pCP, LPCSTR pInfoName, cStrObj& ret)
{
	if( !pVersion || !pInfoName )
		return false;

	WCHAR VerPath[2048];
	_snwprintf(VerPath, countof(VerPath), L"\\StringFileInfo\\%S\\%S", pCP, pInfoName);
	
	UINT nLen = 0;
	LPCWSTR pVerValue = NULL;
	if( !::VerQueryValueW(pVersion, VerPath,  (LPVOID *)&pVerValue, &nLen) )
		return false;
	ret.assign(pVerValue, cCP_UNICODE, nLen*sizeof(WCHAR));
	
	return true;
}

static CLocker g_csLock;

static BOOL SHGetFileInfoWrap(LPCWSTR strShellName, DWORD dwFileAttributes, DWORD dwFlags, SHFILEINFOW& shInfo)
{
	CoInitialize(NULL);

	BOOL bRes = FALSE;
	g_csLock.Enter();

	__try 
	{
		bRes = ::SHGetFileInfoW(strShellName, dwFileAttributes, &shInfo, sizeof(shInfo), dwFlags) != 0;
	}
	__except(1)
	{
	}

	g_csLock.Leave();
	return bRes;
}

static PSID _LdapADPAth2Sid(LPCWSTR pwzADsPath)
{
	if( !pwzADsPath || !*pwzADsPath )
		return NULL;

	LPCWSTR tmp = wcsstr(pwzADsPath, L"<SID=");
	if( !tmp )
		return NULL;

	pwzADsPath = tmp + 5;
	tmp = wcschr(pwzADsPath, L'>');
	if( !tmp )
		return NULL;

	size_t len = tmp - pwzADsPath;

	SID * pSid = NULL;
	LPCWSTR sAuth = len > (2 + 2 + 4 + 8) ? (pwzADsPath + 2 + 2 + 4 + 8) : NULL;

	{
		DWORD Revision = 0, SubAuthorityCount = 0;
		_snwscanf(pwzADsPath, 4, L"%02x%02x", &Revision, &SubAuthorityCount);

		if( SubAuthorityCount && !sAuth )
			return NULL;
		if( (pwzADsPath + len - sAuth)/8 != SubAuthorityCount )
			return NULL;
		
		SID_IDENTIFIER_AUTHORITY _sia; memset(&_sia, 0, sizeof(_sia)); _sia.Value[5] = (BYTE)SubAuthorityCount;
		if( !::AllocateAndInitializeSid(&_sia, (BYTE)SubAuthorityCount, 0, 0, 0, 0, 0, 0, 0, 0, (PSID *)&pSid) )
			return NULL;

		pSid->Revision = (BYTE)Revision;
		pSid->SubAuthorityCount = (BYTE)SubAuthorityCount;
	}
	
	BYTE i;
	
	// Get a SID identifier authority
	for(i = 0; i < countof(pSid->IdentifierAuthority.Value); i++)
	{
		DWORD dwVal = 0; _snwscanf(pwzADsPath + 4 + i*2, 2, L"%02x", &dwVal);
		pSid->IdentifierAuthority.Value[i] = (BYTE)dwVal;
	}
	
	// Get SID subauthorities
	for(i = 0; i < pSid->SubAuthorityCount; i++)
	{
		BYTE pbSrc[4];
		_snwscanf(sAuth, 8, L"%08x", pbSrc);

		PBYTE pbDst = (PBYTE)(pSid->SubAuthority + i);
		pbDst[0] = pbSrc[3];
		pbDst[1] = pbSrc[2];
		pbDst[2] = pbSrc[1];
		pbDst[3] = pbSrc[0];
		
		sAuth += 8;
	}

	return (PSID)pSid;
}

static PSID _Str2Sid(LPCWSTR pwzSid)
{
	SID * pSid = NULL;

	//::ConvertStringSidToSidW(pwzSid, (PSID *)&pSid);

	if( !pwzSid || !*pwzSid )
		return NULL;
	
	if( *pwzSid != L'S' )
		return NULL;

	size_t len = wcslen(pwzSid);

	LPCWSTR sIdAuth = NULL;
	LPCWSTR sAuth = NULL;
	{
		LPCWSTR sep;
		
		if( !(sep = wcschr(pwzSid, L'-')) )
			return NULL;
		if( !(sep = wcschr(sep + 1, L'-')) )
			return NULL;
		
		if( sep = wcschr(sep + 1, L'-') )
		{
			sep++;
			
			if( (pwzSid + len - sep >= 2) && (sep[0] == L'0') && (sep[1] == L'x') )
			{
				sIdAuth = sep + 2;

				LPCWSTR sep1 = wcschr(sep, L'-');
				if( sep1 )
				{
					sAuth = sep1 + 1;

					if( (sep1 - sIdAuth) != 2*countof(pSid->IdentifierAuthority.Value) )
						return NULL;
				}
				else
				{
					if( (pwzSid + len - sIdAuth) != 2*countof(pSid->IdentifierAuthority.Value) )
						return NULL;
				}
			}
			else
				sAuth = sep;
		}
	}
	
	{
		DWORD Revision = 0, SubAuthorityCount = 0;
		swscanf(pwzSid, L"S-%u-%u", &Revision, &SubAuthorityCount);

		SID_IDENTIFIER_AUTHORITY _sia; memset(&_sia, 0, sizeof(_sia)); _sia.Value[5] = (BYTE)SubAuthorityCount;
		if( !::AllocateAndInitializeSid(&_sia, (BYTE)SubAuthorityCount, 0, 0, 0, 0, 0, 0, 0, 0, (PSID *)&pSid) )
			return NULL;

		pSid->Revision = (BYTE)Revision;
		pSid->SubAuthorityCount = (BYTE)SubAuthorityCount;
	}
	
	BYTE i;

	// Get a SID identifier authority
	if( sIdAuth )
	{
		for(i = 0; i < countof(pSid->IdentifierAuthority.Value); i++)
		{
			DWORD dwVal = 0; _snwscanf(sIdAuth + 2*i, 2, L"%02x", &dwVal);
			pSid->IdentifierAuthority.Value[i] = (BYTE)dwVal;
		}
	}
	
	// Get SID subauthorities
	for(i = 0; i < pSid->SubAuthorityCount; i++)
	{
		if( !sAuth )
			break;
		
		LPCWSTR sAuthNext = NULL;
		size_t nAuthLen;
		{
			LPCWSTR sep = wcschr(sAuth, L'-');
			if( sep  )
			{
				sAuthNext = sep + 1;
				nAuthLen = sep - sAuth;
			}
			else
				nAuthLen = pwzSid + len - sAuth;
		}
		
		if( !nAuthLen )
			break;

		_snwscanf(sAuth, nAuthLen, L"%u", pSid->SubAuthority + i);
		sAuth = sAuthNext;
	}
	
	if( i < pSid->SubAuthorityCount )
		return ::FreeSid(pSid), NULL;
	
	return (PSID)pSid;
}

static void _Sid2Str(PSID pSid, cStrObj& strSid, bool bIdAuthForce = false)
{
/*
	{
		LPWSTR _wstr = NULL; ::ConvertSidToStringSidW(pSid, &_wstr);
		if( _wstr )
		{
			strSid = _wstr;
			::LocalFree((HLOCAL)_wstr);
		}
	}
*/
	
	tWCHAR buf[16];
	SID * psid = (SID *)pSid;

	_snwprintf(buf, countof(buf), L"S-%u-%u", psid->Revision, psid->SubAuthorityCount);
	strSid = buf;
	
	if( bIdAuthForce || psid->IdentifierAuthority.Value[0] || psid->IdentifierAuthority.Value[1] )
	{
		_snwprintf(buf, countof(buf), L"-0x%02x%02x%02x%02x%02x%02x",
			psid->IdentifierAuthority.Value[0],
			psid->IdentifierAuthority.Value[1],
			psid->IdentifierAuthority.Value[2],
			psid->IdentifierAuthority.Value[3],
			psid->IdentifierAuthority.Value[4],
			psid->IdentifierAuthority.Value[5]);
		strSid += buf;
	}
	
	for(UINT i = 0; (i < psid->SubAuthorityCount) && psid->SubAuthority[i]; i++)
	{
		_snwprintf(buf, countof(buf), L"-%u", psid->SubAuthority[i]);
		strSid += buf;
	}
}

bool _GetSidAccountName(PSID pSid, cStrObj& strUserName)
{
	SID_NAME_USE _eSnu;
	WCHAR szAccount[0x100] = { 0 };
	WCHAR szDomain[0x100] = { 0 };
	DWORD nAccountLen = countof(szAccount);
	DWORD nDomainLen = countof(szDomain);

	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::LookupAccountSid(S-%u-%u-...)...", ((SID *)pSid)->Revision, ((SID *)pSid)->SubAuthorityCount));

	bool ok = !!::LookupAccountSidW(NULL, pSid, szAccount, &nAccountLen, szDomain, &nDomainLen, &_eSnu);
	if( ok )
	{
		if( szDomain[0] )
		{
			strUserName += szDomain;
			strUserName += L"\\";
		}

		strUserName += szAccount;
	}
	
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::LookupAccountSid(S-%u-%u-...) result: %S, err=0x%08x.", ((SID *)pSid)->Revision, ((SID *)pSid)->SubAuthorityCount, strUserName.data(), ok ? 0 : ::GetLastError()));
	return ok;
}

bool CWinRoot::BrowseUserAccount(CItemBase * pParent, CObjectInfo& pObj)
{
	CComPtr<IDsObjectPicker>	pObjPick;		// semi-smart pointer to object
	CComPtr<IDataObject>		pDataObj;		// result data object
	DSOP_INIT_INFO				InitInfo;		// Init Info
	DSOP_SCOPE_INIT_INFO		aScopeInit[1];	// Scope Init Info
	HRESULT						hr;

	hr = ::CoCreateInstance(CLSID_DsObjectPicker, NULL, CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER, IID_IDsObjectPicker, (LPVOID *)&pObjPick);
	if( FAILED(hr) )
		return false;
	
	// init the DSOP_SCOPE_INIT_INFO
	memset(aScopeInit, 0, sizeof(aScopeInit)); aScopeInit[0].cbSize = sizeof(DSOP_SCOPE_INIT_INFO);
	aScopeInit[0].flType								= DSOP_SCOPE_TYPE_TARGET_COMPUTER|DSOP_SCOPE_TYPE_UPLEVEL_JOINED_DOMAIN|DSOP_SCOPE_TYPE_DOWNLEVEL_JOINED_DOMAIN|DSOP_SCOPE_TYPE_ENTERPRISE_DOMAIN|DSOP_SCOPE_TYPE_GLOBAL_CATALOG|DSOP_SCOPE_TYPE_EXTERNAL_UPLEVEL_DOMAIN|DSOP_SCOPE_TYPE_EXTERNAL_DOWNLEVEL_DOMAIN|DSOP_SCOPE_TYPE_USER_ENTERED_UPLEVEL_SCOPE|DSOP_SCOPE_TYPE_USER_ENTERED_DOWNLEVEL_SCOPE;
	aScopeInit[0].flScope								= DSOP_SCOPE_FLAG_STARTING_SCOPE|DSOP_SCOPE_FLAG_WANT_SID_PATH|DSOP_SCOPE_FLAG_WANT_PROVIDER_WINNT|DSOP_SCOPE_FLAG_DEFAULT_FILTER_USERS;
	aScopeInit[0].FilterFlags.Uplevel.flBothModes		= DSOP_FILTER_USERS|DSOP_FILTER_GLOBAL_GROUPS_DL;
	aScopeInit[0].FilterFlags.Uplevel.flMixedModeOnly	= 0;
	aScopeInit[0].FilterFlags.Uplevel.flNativeModeOnly	= 0;
	aScopeInit[0].FilterFlags.flDownlevel				= DSOP_DOWNLEVEL_FILTER_USERS;

	memset(&InitInfo, 0, sizeof(DSOP_INIT_INFO)); InitInfo.cbSize = sizeof(DSOP_INIT_INFO);
	InitInfo.pwzTargetComputer = NULL;
	InitInfo.cDsScopeInfos = countof(aScopeInit);
	InitInfo.aDsScopeInfos = aScopeInit;
	InitInfo.flOptions = DSOP_FLAG_SKIP_TARGET_COMPUTER_DC_CHECK/*|DSOP_FLAG_MULTISELECT*/;

	// make the call to tell the system what kind of dialog it should display
	hr = pObjPick->Initialize(&InitInfo);
	if( FAILED(hr) )
		return false;

	// make the call to show the dialog that we want
	hr = pObjPick->InvokeDialog(pParent->GetWindow(), (IDataObject **)&pDataObj);
	if( hr != S_OK )
		return false;

	// decode the results from the dialog
	STGMEDIUM				stm;		// the storage medium
	FORMATETC				fe;			// formatetc specifier
	PDS_SELECTION_LIST		pDsSelList;	// pointer to our results
	ULONG					i;			// loop iterator
	
	// Get the global memory block that contain the user's selections.
	fe.cfFormat = (CLIPFORMAT)::RegisterClipboardFormat(CFSTR_DSOP_DS_SELECTION_LIST);
	fe.ptd = NULL;
	fe.dwAspect = DVASPECT_CONTENT;
	fe.lindex = -1;
	fe.tymed = TYMED_HGLOBAL;

	// grab the data object
	hr = pDataObj->GetData(&fe, &stm);
	if( FAILED(hr) )
		return false;

	// Retrieve a pointer to DS_SELECTION_LIST structure.
	pDsSelList = (PDS_SELECTION_LIST)::GlobalLock(stm.hGlobal);
	if( !pDsSelList )
		return false;

	// Loop through DS_SELECTION array of selected objects.
	for(i = 0; i < pDsSelList->cItems; i++)
	{
		DS_SELECTION& _s = pDsSelList->aDsSelection[i];
		
		PSID pSid = _LdapADPAth2Sid(_s.pwzADsPath);
		if( !pSid )
			continue;
		
		_Sid2Str(pSid, pObj.m_strName);
		_GetSidAccountName(pSid, pObj.m_strDisplayName);
		::FreeSid(pSid);
	}
	::GlobalUnlock(stm.hGlobal);

	::ReleaseStgMedium(&stm);
	return true;
}

//////////////////////////////////////////////////////////////////////

bool _GetUserAccountInfo(CObjectInfo * pObjInfo)
{
	PSID pSid = _Str2Sid(pObjInfo->m_strName.data());
	if( !pSid )
		return false;
	
	bool res = _GetSidAccountName(pSid, pObjInfo->m_strDisplayName);
	::FreeSid(pSid);
	return res;
}

bool CWinRoot::GetObjectInfo(CObjectInfo * pObjInfo)
{
	if( CRootItem::GetObjectInfo(pObjInfo) )
		return true;
	
	tDWORD& nMask = pObjInfo->m_nQueryMask;
	tDWORD& nObjType = pObjInfo->m_nType;

	if( nObjType == SHELL_OBJTYPE_PROCESS )
		return m_pSink ? ((CRootSink *)m_pSink)->GetAppInfoByPid(pObjInfo->m_nCustomId, pObjInfo) : false;

	if( nObjType == SHELL_OBJTYPE_USERACCOUNT )
		return _GetUserAccountInfo(pObjInfo);

	if( !(m_nGuiFlags & GUIFLAG_ADMIN) )
		return _GetObjectInfo(pObjInfo);
	
	if( nObjType == SHELL_OBJTYPE_MYCOMPUTER ||
		!wcsncmp(pObjInfo->m_strName.data(), L"%personal%", 10) )
		return _GetObjectInfo(pObjInfo);
	
	if( nObjType == SHELL_OBJTYPE_UNKNOWN )
	{
		LPCWSTR strName = pObjInfo->m_strName.data();
		int len = pObjInfo->m_strName.size();
		
		nObjType = SHELL_OBJTYPE_MASK;

		if( len >= 2 && len <= 3 )
		{
			if( strName[1] == L':' )
				nObjType = SHELL_OBJTYPE_DRIVE;
		}
		else if( len && (strName[len - 1] == '\\' || strName[len - 1] == '/') )
			nObjType = SHELL_OBJTYPE_FOLDER;
	}

	if( nMask & OBJINFO_ICON )
	{
		SHFILEINFOW shInfo; ::ZeroMemory(&shInfo, sizeof(shInfo));
		DWORD       dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		DWORD       dwFlags = SHGFI_ICON|((nMask & OBJINFO_LARGEICON) ? SHGFI_LARGEICON : SHGFI_SMALLICON)|SHGFI_USEFILEATTRIBUTES;
		LPCWSTR     strShellName = pObjInfo->m_strName.data();
		int         nIcon = -1;
		
		HICON hIcon = NULL;
		switch( nObjType )
		{
		case SHELL_OBJTYPE_FOLDER:
			dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
			strShellName = L"";
			break;

		case SHELL_OBJTYPE_FILE:
			{
				tDWORD nPos = pObjInfo->m_strName.find_last_of(L"\\/");
				if( nPos != cStrObj::npos )
				{
					tDWORD nExtPos = pObjInfo->m_strName.find_last_of(L".");
					if( nExtPos != cStrObj::npos )
						strShellName += nExtPos;
					else
						strShellName += nPos + 1;
				}
			}
			break;
		
		case SHELL_OBJTYPE_DRIVE:
			if( nMask & OBJINFO_LARGEICON )
				::ExtractIconEx("shell32.dll", 8, &hIcon, NULL, 1);
			else
				::ExtractIconEx("shell32.dll", 8, NULL, &hIcon, 1);
			break;
		
		default:
			dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
			break;
		}
		
		if( !wcsncmp(pObjInfo->m_strName.data(), L"%personal%", 10) )
		{
			static CShellPidl pidlPersonal((UINT)CSIDL_PERSONAL, m_hWnd);
			strShellName = (LPCWSTR)(LPITEMIDLIST)pidlPersonal;
			dwFlags &= ~SHGFI_USEFILEATTRIBUTES;
			dwFlags |= SHGFI_PIDL;
		}

		if( !hIcon )
		{
			HIMAGELIST hSysImageList = (HIMAGELIST)SHGetFileInfoWrap(strShellName, dwFileAttributes, dwFlags|SHGFI_SYSICONINDEX, shInfo);
			if( nIcon != -1 )
				shInfo.hIcon = ImageList_GetIcon(hSysImageList, nIcon, ILD_NORMAL);
			else if( shInfo.iIcon && !shInfo.hIcon && hSysImageList )
				shInfo.hIcon = ImageList_GetIcon(hSysImageList, shInfo.iIcon, ILD_NORMAL);
		
			hIcon = shInfo.hIcon;
		}
		
		if( hIcon )
			pObjInfo->m_pIcon = CreateIcon(hIcon, true);
		else
			pObjInfo->m_pIcon = NULL;
	}
	
	pObjInfo->m_strDisplayName = pObjInfo->m_strName;
	return true;
}

bool ExpandEnv(LPCWSTR strSource, LPWSTR strResult, DWORD nSize, tDWORD &nObjType, tDWORD &nShellId, LPITEMIDLIST &pListId)
{
	*strResult = 0;
	if( !wcsncmp(strSource, L"%personal%", 10) )
	{
		nShellId = CSIDL_PERSONAL;
		::SHGetSpecialFolderLocation(NULL, nShellId, &pListId);

		if( pListId && nShellId == CSIDL_PERSONAL )
		{
			if( !SHGetPathFromIDListW(pListId, strResult) )
			{
				::CoTaskMemFree(pListId);
				pListId = NULL;
				return false;
			}
			wcscat(strResult, strSource + 10);
		}
		return true;
	}

	if( !wcsncmp(strSource, L"%sysdir%", 8) )
		return !!GetSystemDirectoryW(strResult, nSize);
	
	return !!ExpandEnvironmentStringsW(strSource, strResult, nSize - sizeof(WCHAR));
}

bool CWinRoot::_GetObjectInfo(CObjectInfo * pObjInfo)
{
	tDWORD &nMask = pObjInfo->m_nQueryMask;
	tDWORD &nObjType = pObjInfo->m_nType;
	BOOL   res;

	tDWORD  nShellId = 0;
	WCHAR   strBuff[0x1000];
	LPWSTR  strShellName = strBuff;
	bool    bAlreadyExpanded = false;
	LPITEMIDLIST pListId = NULL;

	USES_CONVERSION;

	if( nObjType == SHELL_OBJTYPE_UNKNOWN )
	{
		{
			cAutoCS locker(m_accessor, true);
			
			if( !m_setDrives.size() )
				InitDrivesList();
			
			for(size_t i = 0; i < m_setDrives.size(); i++ )
				if( !pObjInfo->m_strName.compare(m_setDrives[i].c_str(),
						fSTRING_COMPARE_CASE_INSENSITIVE|fSTRING_COMPARE_SLASH_INSENSITIVE) )
				{
					nObjType = SHELL_OBJTYPE_DRIVE;
					break;
				}
		}

		if( !wcscmp(pObjInfo->m_strName.data(), L"%mycomputer%") )
			nObjType = SHELL_OBJTYPE_MYCOMPUTER;

		if( nObjType == SHELL_OBJTYPE_UNKNOWN )
		{
			if( pObjInfo->m_strName.empty() )
				return false;
			
			if( !ExpandEnv(pObjInfo->m_strName.data(), strBuff, countof(strBuff), nObjType, nShellId, pListId) )
				return false;

			bAlreadyExpanded = true;
		
			tDWORD nAttr = ::GetFileAttributesW(strShellName);

			if( nAttr == INVALID_FILE_ATTRIBUTES )
			{
				cStrObj strPath(L"\\\\?\\");
				strPath += strShellName;
				nAttr = ::GetFileAttributesW(strPath.data());
			}
			
			if( nAttr == INVALID_FILE_ATTRIBUTES )
			{
				WIN32_FIND_DATAW _FileData;
				HANDLE hFind = ::FindFirstFileW(strShellName, &_FileData);
				if( hFind != INVALID_HANDLE_VALUE )
				{
					nAttr = _FileData.dwFileAttributes;
					if( !(nAttr & FILE_ATTRIBUTE_SYSTEM) )
						nAttr = INVALID_FILE_ATTRIBUTES;
					
					::FindClose(hFind);
				}
			}
			
			nObjType = nAttr & FILE_ATTRIBUTE_DIRECTORY ? SHELL_OBJTYPE_FOLDER : SHELL_OBJTYPE_FILE;
		}

		if( nObjType == SHELL_OBJTYPE_FOLDER || nObjType == SHELL_OBJTYPE_DRIVE )
		{
			size_t nSize = wcslen(strShellName);
			if( nSize && strShellName[nSize - 1] != '\\' )
			{
				strShellName[nSize] = '\\';
				strShellName[nSize + 1] = 0;
			}
		}
	}

	bool bRelativePath = false;
	if( nMask & (OBJINFO_DISPLAYNAME|OBJINFO_ICON) )
	{
		SHFILEINFOW shInfo; ::ZeroMemory(&shInfo, sizeof(shInfo));

		DWORD        dwFlags = 0;
		if( nMask & OBJINFO_ICON )
			dwFlags |= SHGFI_ICON|((nMask & OBJINFO_LARGEICON) ? SHGFI_LARGEICON : SHGFI_SMALLICON);

		switch( nObjType )
		{
		case SHELL_OBJTYPE_MYCOMPUTER:
			nShellId = CSIDL_DRIVES;
			break;

		case SHELL_OBJTYPE_DRIVE:
			strShellName = const_cast<LPWSTR>(pObjInfo->m_strName.data());
			dwFlags |= SHGFI_USEFILEATTRIBUTES|SHGFI_TYPENAME|SHGFI_DISPLAYNAME;
			break;

		default:
			if( !bAlreadyExpanded )
			{
				if( !ExpandEnv(pObjInfo->m_strName.data(), strBuff, countof(strBuff), nObjType, nShellId, pListId) )
					return false;
			}
		}

		LPCWSTR strSubPersonal = nShellId == CSIDL_PERSONAL ? pObjInfo->m_strName.data() + 10 : NULL;
		
		if( nShellId )
		{
			if( !pListId )
				::SHGetSpecialFolderLocation(NULL, nShellId, &pListId);

			if( pListId && strSubPersonal )
			{
				dwFlags &= ~(SHGFI_ICON|SHGFI_SMALLICON);
				bRelativePath = true;
			}

			dwFlags |= SHGFI_PIDL|SHGFI_USEFILEATTRIBUTES|SHGFI_TYPENAME|SHGFI_DISPLAYNAME;
		}

		res = SHGetFileInfoWrap((dwFlags & SHGFI_PIDL) ? (LPCWSTR)pListId : strShellName, FILE_ATTRIBUTE_NORMAL, dwFlags, shInfo);

		if( nMask & OBJINFO_DISPLAYNAME )
		{
			if( nObjType == SHELL_OBJTYPE_DRIVE )
			{
				if( pObjInfo->m_strName.size() )
				{
					CHAR szDrive[3] = {(CHAR)pObjInfo->m_strName.at(0), ':', 0};
					if( GetDriveType(szDrive) == DRIVE_NO_ROOT_DIR )
						pObjInfo->m_nType = SHELL_OBJTYPE_UNKNOWN;
				}

				if( !shInfo.szDisplayName || !*shInfo.szDisplayName )
				{
					pObjInfo->m_strDisplayName = pObjInfo->m_strName;
					pObjInfo->m_nType = SHELL_OBJTYPE_UNKNOWN;
				}
				else
				{
					pObjInfo->m_strDisplayName = shInfo.szDisplayName;
					pObjInfo->m_strDisplayName.replace_one((PWCHAR)"\xBD\x00\x00\x00", 0, L".5");
					pObjInfo->m_strDisplayName.replace_one((PWCHAR)"\x3F\x00\x00\x00", 0, L".5");
				}
			}
			else if( nShellId )
			{
				pObjInfo->m_strDisplayName = shInfo.szDisplayName;
				if( strSubPersonal )
				{
					pObjInfo->m_strDisplayName += strSubPersonal;
					strShellName = strBuff;
				}
			}
			else
				pObjInfo->m_strDisplayName = strShellName;
		}

		if( nMask & OBJINFO_ICON )
		{
			if( nObjType != SHELL_OBJTYPE_DRIVE && nShellId && strSubPersonal )
				strShellName = strBuff;
		
			if( !_wcsicmp(strShellName, L"system") )
			{
				CIcon* pIcon = CRootItem::GetIcon("startupobj");
				if( pIcon )
					shInfo.hIcon = pIcon->IconHnd();
			}

			HIMAGELIST hSysImageList = NULL;
			if( !shInfo.hIcon )
			{
				::ZeroMemory(&shInfo, sizeof(shInfo));
				hSysImageList = (HIMAGELIST)SHGetFileInfoWrap(strShellName, FILE_ATTRIBUTE_NORMAL, SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX, shInfo);
				if( !shInfo.hIcon && !shInfo.hIcon )
					hSysImageList = (HIMAGELIST)SHGetFileInfoWrap(strShellName, FILE_ATTRIBUTE_NORMAL, SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES, shInfo);
			}

			if( shInfo.iIcon && !shInfo.hIcon && hSysImageList )
				shInfo.hIcon = ImageList_GetIcon(hSysImageList, shInfo.iIcon, ILD_NORMAL);

			if( shInfo.hIcon )
				pObjInfo->m_pIcon = CreateIcon(shInfo.hIcon, true);
			else
				pObjInfo->m_pIcon = NULL;
		}

		if( pListId )
			::CoTaskMemFree(pListId);
	}

	if( nObjType == SHELL_OBJTYPE_FILE )
	{
		if( !bRelativePath )
			pObjInfo->m_strDisplayName = pObjInfo->m_strName;

		if( (nMask & OBJINFO_PRODUCTVERSION) || (nMask & OBJINFO_FILEVERSION) || (nMask & OBJINFO_COMPANYNAME) || (nMask & OBJINFO_DESCRIPTION) )
		{
			DWORD dwHandle;
			DWORD dwVersionSize = ::GetFileVersionInfoSizeW(pObjInfo->m_strName.data(), &dwHandle);
			if( dwVersionSize )
			{
				PVOID pVersionInfo = HeapAlloc(GetProcessHeap(), 0, dwVersionSize);
				if( pVersionInfo )
				{
					if( ::GetFileVersionInfoW(pObjInfo->m_strName.data(), NULL, dwVersionSize, pVersionInfo) )
					{
						UINT    nLen = 0;
						LPDWORD pnLangCp;
						if( ::VerQueryValueA(pVersionInfo, "\\VarFileInfo\\Translation", (LPVOID *)&pnLangCp, &nLen) )
						{
							#define __SWAPWORDS__(X) ( (X<<16) | (X>>16) )

							CHAR sLangCp[20];
							sprintf(sLangCp, "%08X", __SWAPWORDS__(*pnLangCp));
							
							if( nMask & OBJINFO_PRODUCTVERSION )
								VerQueryValue(pVersionInfo, sLangCp, "ProductVersion", pObjInfo->m_strProductVersion);
							
							if( nMask & OBJINFO_FILEVERSION )
								VerQueryValue(pVersionInfo, sLangCp, "FileVersion", pObjInfo->m_strFileVersion);
							
							if( nMask & OBJINFO_COMPANYNAME )
								VerQueryValue(pVersionInfo, sLangCp, "CompanyName", pObjInfo->m_strCompanyName);
							
							if( nMask & OBJINFO_DESCRIPTION )
								VerQueryValue(pVersionInfo, sLangCp, "FileDescription", pObjInfo->m_strDescription);
						}
					}
					
					HeapFree(GetProcessHeap(), 0, pVersionInfo);
				}
			}
		}
	}

	return true;
}

bool CWinRoot::ShowCpl(LPCSTR strAppletId)
{
	// ѕути по умолчанию
	tCHAR szCplPath[_MAX_PATH];
	tCHAR szCmdLine[_MAX_PATH * 2];
	LPCSTR strModule;

	if(m_osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		::GetWindowsDirectory(szCplPath, countof(szCplPath));
	else
		::GetSystemDirectory(szCplPath, countof(szCplPath));
	_tcscat(szCplPath, _T("\\control.exe"));

	PROCESS_INFORMATION stProcessInfo;
	STARTUPINFO stStartUpInfo; memset(&stStartUpInfo, 0, sizeof(stStartUpInfo));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	
	if( (DWORD)(LOBYTE(LOWORD(GetVersion()))) <= 5 )
	{
		int nAppletIdx = 0, nPageIdx = 0;

		sswitch(strAppletId)
			scase(CPLID_SYSTEMINFO) strModule = _T("sysdm.cpl"); sbreak;
			scase(CPLID_SOUNDS)
				strModule = _T("mmsys.cpl");
			if( ( m_osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && m_osVer.dwMajorVersion <= 4 && !m_osVer.dwMinorVersion ) ||
				( m_osVer.dwPlatformId == VER_PLATFORM_WIN32_NT && m_osVer.dwMajorVersion <= 4 ) )
				nAppletIdx = 1;
			else if( m_osVer.dwMajorVersion >= 5 && m_osVer.dwMinorVersion > 0 )
				nPageIdx = 1;
			sbreak;
			sdefault()              strModule = strAppletId; sbreak;
		send;

		_sntprintf(szCmdLine, countof(szCmdLine), _T("\"%s\" %s,@%u,%u"), szCplPath, strModule, nAppletIdx, nPageIdx);
	}
	else
	{
		sswitch(strAppletId)
			scase(CPLID_SYSTEMINFO) strModule = _T("System"); sbreak;
			sdefault()              strModule = strAppletId; sbreak;
		send;

		_sntprintf(szCmdLine, countof(szCmdLine), _T("\"%s\" /name Microsoft.%s"), szCplPath, strModule);
	}
	
	if( !::CreateProcess(szCplPath, szCmdLine, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &stStartUpInfo, &stProcessInfo) )
		return false;

	::CloseHandle(stProcessInfo.hThread);
	::CloseHandle(stProcessInfo.hProcess);
	return true;
}

bool CWinRoot::ShowContextHelp(tDWORD dwTopicID, CItemBase* pParent)
{
	PostMessage(WM_ONCTXHELP, (WPARAM)dwTopicID);
	return true;
}

///////////////////////////////////////////////////////
// class CFileFilterMaker

class CFileFilterMaker
{
public:
	CFileFilterMaker(tString sline, tCHAR sep) { Parse(sline, sep); }
	
	void GetFilter(CRootItem* pRoot, tCHAR* szFilter) 
	{
		tString sFilter;
		tCHAR szBuff[MAX_PATH];
		for(size_t i = 0; i < m_vExts.size(); i++)
		{
			tString& sExt = m_vExts[i];
			tString sExtDescr; pRoot->GetString(sExtDescr, PROFILE_LOCALIZE, "BrowseFileExt", sExt.c_str());
			
			if( sExt.find('.') == -1 )
			{
				if( sExtDescr.empty() )
					sprintf(szBuff, "*.%s\n*.%s\n", sExt.c_str(), sExt.c_str());
				else
					sprintf(szBuff, "%s (*.%s)\n*.%s\n", sExtDescr.c_str(), sExt.c_str(), sExt.c_str());
			}
			else
			{
				if( sExtDescr.empty() )
					sprintf(szBuff, "%s\n%s\n", sExt.c_str(), sExt.c_str());
				else
					sprintf(szBuff, "%s (%s)\n%s\n", sExtDescr.c_str(), sExt.c_str(), sExt.c_str());
			}
			
			sFilter += szBuff;
		}
		
		sprintf(szFilter, "%s\n", sFilter.c_str());
	}

private:
	cVector<tString> m_vExts;

	void Parse(tString sline, tCHAR sep)
	{
		if( sline.empty() ) 
			return;

		size_t pos_new = sline.find(sep);
		if( pos_new == -1 )
		{
			m_vExts.push_back(sline);
			return;
		}

		size_t pos_old = 0;
		while( pos_new >= pos_old )
		{
			tString one_val = sline.substr(pos_old, (pos_new - pos_old));
			m_vExts.push_back(one_val);
			pos_old = pos_new + 1;
			pos_new = sline.find(sep, pos_old);
			if( (tNATIVE_INT)pos_new < 0 ) pos_new = sline.length();
		}
	}
};

///////////////////////////////////////////////////////

struct CBrowseCallBackEx : public CBrowseCallBack
{
	virtual bool FilterObject(CObjectInfo& pObject)
	{
		if( pObject.m_nType != SHELL_OBJTYPE_FILE )
			return true;

		tDWORD pos = pObject.m_strName.find_last_of(".");
		if( pos == cStrObj::npos )
			return false;
		return !pObject.m_strName.compare(pos+1, cStrObj::whole, "exe", fSTRING_COMPARE_CASE_INSENSITIVE);
	}
} g_browse;

bool CWinRoot::BrowseFile(CItemBase* pParent, LPCSTR strTitle, LPCSTR strFileType, tObjPath& sPath, bool bSave)
{
	if( !strFileType )
		strFileType = "*";

	if( (m_nGuiFlags & GUIFLAG_ADMIN) && !strcmp(strFileType, "exe") )
	{
		cGuiParams _data;
		_data.m_strVal1 = sPath;
		if( CDialogBindingT<>(&_data).DoModal(pParent, "BrowseForAppSimpleEdit", DLG_ACTION_OKCANCEL) != DLG_ACTION_OK )
			return false;

		sPath = _data.m_strVal1;
		return true;
	}

	OPENFILENAMEA stOfn; memset(&stOfn, 0, sizeof(stOfn));
	stOfn.lStructSize  = m_osVer.dwMajorVersion == 4 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(stOfn);
	stOfn.hwndOwner    = GetParentHWND(pParent);
	stOfn.Flags        = OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_SHAREAWARE/*|OFN_SHOWHELP*/;
	stOfn.Flags       |= bSave ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST|OFN_NOTESTFILECREATE;
	
	if( g_bWOW64 )
		stOfn.Flags |= OFN_NODEREFERENCELINKS;
	
	CFileFilterMaker FFMaker(strFileType, ',');
	tCHAR strFilter[MAX_PATH];
	FFMaker.GetFilter(this, strFilter);

	if( strTitle && *strTitle )
		stOfn.lpstrTitle = strTitle;
	if( *strFileType != '*' )
		stOfn.lpstrDefExt = strFileType;

	stOfn.lpstrFilter = strFilter;
	stOfn.nFilterIndex = 0;

	cStrObj strObjTemp = sPath;
	if( strObjTemp.find_last_of("\\/") == cStrObj::npos )
	{
		cStrObj strLastPath; cAutoRegKey(m_cfg, cRegRoot, "OpenSave").get_strobj(strFileType, strLastPath);
		if( !strLastPath.empty() )
		{
			strLastPath.check_last_slash();
			strLastPath += strObjTemp;
			strObjTemp = strLastPath;
		}
	}
	
	bool l_res = !!CWindowL::BrowseFile(m_pRoot, &stOfn, strObjTemp, bSave);
	DWORD cdee = ::CommDlgExtendedError();
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::BrowseFile() try 1: CommDlgExtendedError=0x%08x", cdee));
	
	if( !l_res && cdee == FNERR_INVALIDFILENAME )
	{
		tDWORD pos = strObjTemp.find_last_of("\\/:");
		if( pos != cStrObj::npos )
		{
			strObjTemp.erase(0, pos + 1);
			l_res = !!CWindowL::BrowseFile(m_pRoot, &stOfn, strObjTemp, bSave);
			cdee = ::CommDlgExtendedError();
			PR_TRACE((g_root, prtIMPORTANT, "GUI(Win)::BrowseFile() try 2: CommDlgExtendedError=0x%08x", cdee));
		}
	}
	
	if( l_res )
		if( g_bWOW64 && strObjTemp.size() > 4 && !strObjTemp.compare(strObjTemp.size() - 4, 4, ".lnk", fSTRING_COMPARE_CASE_INSENSITIVE) )
			l_res = ResolveShortcat(strObjTemp);
		
	if( l_res )
	{
		sPath = strObjTemp;

		tDWORD nPathPos = strObjTemp.find_last_of("\\/");
		if( nPathPos != cStrObj::npos )
		{
			strObjTemp.erase(nPathPos);
			cAutoRegKey(m_cfg, cRegRoot, "OpenSave", cTRUE).set_strobj(strFileType, strObjTemp);
		}
	}
	
	return l_res;
}

//////////////////////////////////////////////////////////////////////

void CWinRoot::TimerRefresh(tDWORD nTimerSpin)
{
	if( GetTickCount() - m_tmLastTimer < TIMER_REFRESH_INT )
		return;

	CRootItem::TimerRefresh(nTimerSpin);

	if( !(m_nTimerSpin % 5) )
		if( _GetLastInputInfo )
		{
			LASTINPUTINFO lif = {sizeof(LASTINPUTINFO)};
			_GetLastInputInfo(&lif);
			
			if( lif.dwTime - m_tmLastUserInput > (1000 * 60 * 15) )
				CItemBase::SendEvent(EVENT_ID_END_LONG_USER_INACTIVITY, NULL, true);
			
			m_tmLastUserInput = lif.dwTime;
		}
		else
			m_tmLastUserInput = m_tmLastTimer;
}

void CWinRoot::SendEvent(tSyncEventData * pEvent)
{
	if( !pEvent->m_bOwnThread && (pEvent->nFlags & EVENT_SEND_SYNC) && !(pEvent->nFlags & EVENT_SEND_OWN_QUEUE) )
		SendMessage(WM_ONEVENT, (WPARAM)pEvent, 0);
	else
		CRootItem::SendEvent(pEvent);
}

void CWinRoot::PostProcessEventQueue()
{
	PostMessage(WM_ONEVENTQUEUE);
}

LRESULT CWinRoot::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRootItem::OnTimer(wParam);
	return S_OK;
}

LRESULT CWinRoot::OnEventHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRootItem::ProcessEvent((tSyncEventData*)wParam);
	return S_OK;
}

LRESULT CWinRoot::OnEventQueue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ProcessEventQueue();
	return S_OK;
}

LRESULT CWinRoot::OnActionHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bItem = !!LOBYTE(HIWORD(wParam));
	bool bSync = !!HIBYTE(HIWORD(wParam));
	tWORD nActionId = LOWORD(wParam);
	CItemBase * pItem = bItem ? (CItemBase *)lParam : NULL;
	void * pParam = bItem ? NULL : (void *)lParam;
	
	if( !pItem || pItem->m_pRoot )
		OnAction((tDWORD)nActionId, pItem, pParam);
	
	if( pItem && !bSync )
		pItem->Release();
	return S_OK;
}

LRESULT CWinRoot::OnContextHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::ShowContextHelp(this, (tDWORD)wParam);
	return 0;
}

LRESULT CWinRoot::OnSysColorChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_nThemed = 2;
	InitThemes();
	ReinitResources(RES_BACKGROUNDS);

	for(size_t i = 0; i < m_vecDialogs.size(); i++)
		::PostMessage(m_vecDialogs[i]->GetWindow(), OCM_SYSCOLORCHANGE, 0, 0);
	return S_OK;
}

LRESULT CWinRoot::OnDeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if( wParam != DBT_DEVICEARRIVAL && wParam != DBT_DEVICEREMOVECOMPLETE )
		return 0;

	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)lParam;
	if( pHdr->dbch_devicetype != DBT_DEVTYP_VOLUME )
		return 0;

	InitDrivesList();

	DEV_BROADCAST_VOLUME* pVolume = (DEV_BROADCAST_VOLUME*)pHdr;
	for(int i = 0; i < 32; i++)
		if( pVolume->dbcv_unitmask & (1 << i) )
		{
			CDeviceAction pInfo;
			pInfo.m_cDevice = 'A'+i;
			pInfo.m_nAction = (wParam == DBT_DEVICEARRIVAL) ? CDeviceAction::Add : CDeviceAction::Delete;

			CRootItem::SendEvent(EVENT_ID_DEVICE_CHANGED, &pInfo, EVENT_SEND_AFTER_TIMEOUT, NULL,
				(m_nOsVer & GUI_OSVER_FAMILY_NT) ? 100 : 3000);
		}
	return 0;
}

LRESULT CWinRoot::OnSessionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win):: WM_WTSSESSION_CHANGE(Status=0x%X)", wParam));

	switch( wParam )
	{
	case WTS_REMOTE_CONNECT:
	case WTS_CONSOLE_CONNECT: m_bGuiActive = true; break;
	case WTS_REMOTE_DISCONNECT:
	case WTS_CONSOLE_DISCONNECT: m_bGuiActive = false; break;
	}
	return S_OK;
}

LRESULT CWinRoot::OnTimeChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_tzset();
	return S_OK;
}

LRESULT CWinRoot::OnPower(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win):: WM_POWERBROADCAST(wParam=0x%X, lParam=0x%X)", wParam, lParam));
	return bHandled = TRUE, TRUE;
}

LRESULT CWinRoot::OnQueryEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PR_TRACE((g_root, prtIMPORTANT, "GUI(Win):: WM_QUERYENDSESSION(wParam=0x%X, lParam=0x%X)", wParam, lParam));
	return bHandled = TRUE, TRUE;
}

void CWinRoot::EnableToolTip(CItemBase* pItem, bool bEnable)
{
	if( !pItem )
		return;

	CItemBase* pToolItem = ((pItem->m_nFlags & STYLE_WINDOW) && !(pItem->m_nFlags & STYLE_NON_OWNER) && pItem->GetWindow()) ? pItem : pItem->m_pOwner;
	if( !pToolItem )
		return;

	HWND hToolWnd = pToolItem->GetWindow();
	if( !hToolWnd )
		return;

	if( !m_hToolTip )
	{
		RECT rcDef = {CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
		m_hToolTip = CWindowL::CreateWnd(WS_EX_TOPMOST|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW, (LPVOID)TOOLTIPS_CLASSW,
							  WS_POPUP|TTS_ALWAYSTIP|TTS_NOANIMATE/*|TTS_BALLOON*/,
							  rcDef, m_hWnd);
		if( m_hToolTip )
		{
			bool bTipTest = false;
			if( m_pSink )
				bTipTest = ((CRootSink *)m_pSink)->IsTooltipTest();
			
			SendMessageW(m_hToolTip, TTM_SETDELAYTIME, TTDT_INITIAL, bTipTest ? 0     : 1500);
			SendMessageW(m_hToolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, bTipTest ? 15000 : 10000);
			SendMessageW(m_hToolTip, TTM_SETDELAYTIME, TTDT_RESHOW,  bTipTest ? 0     : 1000);

            SendMessageW(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, 305);
		}
	}

	TOOLINFOW ti; memset(&ti, 0, sizeof(ti));
	ti.cbSize  = sizeof(ti);
	ti.uId     = (UINT)pItem;
	ti.hwnd    = hToolWnd;
	ti.hinst   = g_hInstance;
	ti.uFlags  = TTF_TRANSPARENT;
	
	if( bEnable )
	{
		if( !(pItem->m_nState & ISTATE_INITED) || (pItem->m_nState & ISTATE_HIDE) )
			return;

		if( pItem->m_nState & ISTATE_SHOWTIP )
			::SendMessageW(m_hToolTip, TTM_GETTOOLINFOW, 0, (LPARAM)&ti);

		pItem->GetParentRect(ti.rect, pToolItem);

		if( IsRectEmpty(&ti.rect) )
			return;

		DWORD nToolMsg = (pItem->m_nState & ISTATE_SHOWTIP) ? TTM_SETTOOLINFOW : TTM_ADDTOOLW;
		pItem->m_nState |= ISTATE_SHOWTIP;

		ti.lpszText = LPSTR_TEXTCALLBACKW;

//		PR_TRACE((g_root, prtERROR, "GUI(Win):: TTM_SETTOOLINFO begin (%d,%d)", ti.rect.left, ti.rect.top));
		::ShowWindow(m_hToolTip, SW_HIDE);
		::SendMessageW(m_hToolTip, nToolMsg, 0, (LPARAM)&ti);
//		PR_TRACE((g_root, prtERROR, "GUI(Win):: TTM_SETTOOLINFO done", ti.rect.left, ti.rect.top));
	}
	else
	{
		if( !(pItem->m_nState & ISTATE_SHOWTIP) )
			return;

		::SendMessageW(m_hToolTip, TTM_DELTOOLW, 0, (LPARAM)&ti);
		pItem->m_nState &= ~ISTATE_SHOWTIP;
	}
}

void CWinRoot::RelayTipEvent(const MSG * pMsg)
{
	if( m_hToolTip )
		::SendMessageW(m_hToolTip, TTM_RELAYEVENT, 0, (LPARAM)pMsg);
}

LRESULT CWinRoot::ProcessTipEvent(NMHDR * pHdr, CItemBase * pOwner)
{
	if( !m_hToolTip || pHdr->hwndFrom != m_hToolTip )
		return 0;

	HWND hwnd = pOwner->GetWindow();
	POINT pt;
	::GetCursorPos(&pt);

	if( hwnd )
		::ScreenToClient(hwnd, &pt);

	if( pHdr->code == TTN_SHOW )
	{
		DWORD flags = SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOCOPYBITS;
		RECT rcTip = {0, 0, 0, 0};
		bool ret = pOwner->GetTipInfo(pt, NULL, &rcTip);
		if( !ret )
		{
			flags |= SWP_NOSIZE|SWP_NOMOVE;
		}
		else if( hwnd )
		{
//			flags |= SWP_NOSIZE|SWP_NOMOVE; 

			::ClientToScreen(hwnd, (LPPOINT)&rcTip);
			::ClientToScreen(hwnd, (LPPOINT)&rcTip+1);
            ::SendMessage(m_hToolTip, TTM_ADJUSTRECT, TRUE, (LPARAM)&rcTip);

			if( rcTip.right > GetSystemMetrics(SM_CXSCREEN) )
			{
				RECT rcTipWnd;
				::GetWindowRect(m_hToolTip, &rcTipWnd);

				rcTip.right = rcTipWnd.right;
				if( rcTip.left > rcTipWnd.left )
					rcTip.left = rcTipWnd.left;

				rcTip.bottom = rcTip.top + RECT_CY(rcTipWnd);
			}
		}

//		PR_TRACE((g_root, prtERROR, "GUI(Win):: TTN_SHOW (%d,%d)", rcTip.left, rcTip.top));

		::SetWindowPos(m_hToolTip, HWND_TOPMOST,
			rcTip.left, rcTip.top, RECT_CX(rcTip), RECT_CY(rcTip), flags);
		return true;
	}

	if( pHdr->code != TTN_GETDISPINFOW && pHdr->code != TTN_GETDISPINFOA )
		return 0;

//	PR_TRACE((g_root, prtERROR, "GUI(Win):: TTN_GETDISPINFO"));

	bool ansi = pHdr->code == TTN_GETDISPINFOA;
	
	NMTTDISPINFOA * dia = (NMTTDISPINFOA *)pHdr;
	NMTTDISPINFOW * diw = (NMTTDISPINFOW *)pHdr;
	(ansi ? dia->hinst : diw->hinst) = NULL;
	
	if( (ansi ? dia->uFlags : diw->uFlags) & TTF_IDISHWND )
	{
		PR_TRACE((g_root, prtERROR, "GUI(Win):: ProcessTipEvent() TTF_IDISHWND exist!!!"));
		return 0;
	}
	
	CItemBase * pItem = (CItemBase *)pHdr->idFrom;
	if( pItem != pOwner )
	{
		CItemBase * pCheckItem;
		for(pCheckItem = NULL; pCheckItem = pOwner->EnumNextItem(pCheckItem); )
			if( pCheckItem == pItem )
				break;

		if( !pCheckItem )
		{
			PR_TRACE((g_root, prtERROR, "GUI(Win):: ProcessTipEvent() invalid item: %p!!!", pItem));
			return 0;
		}
	}

	tString strTipText = pItem->m_strTipText;
	pItem->GetTipInfo(pt, &strTipText, NULL);
	if( strTipText.empty() )
		return 0;
	
	LocalizeStr(m_bufItemTip, strTipText.c_str(), strTipText.size());
	if( ansi )
	{
		tString tipTextA;
		
		size_t cc = WideCharToMultiByte(CP_ACP, 0, m_bufItemTip.data(), m_bufItemTip.size(), NULL, 0, NULL, NULL);
		tipTextA.resize(cc);
		WideCharToMultiByte(CP_ACP, 0, m_bufItemTip.data(), m_bufItemTip.size(), const_cast<char *>(tipTextA.c_str()), cc, NULL, NULL);
		
		m_bufItemTip.resize(cc / 2 + 1);
		memcpy((char *)m_bufItemTip.data(), tipTextA.c_str(), cc);
	}	

	dia->lpszText = (LPSTR)m_bufItemTip.data();
	return 0;
}

bool CWinRoot::ShowBalloon(CItemBase* pItem, LPCSTR strTextId, LPCSTR strCaptionId)
{
	if( (m_nGuiFlags & GUIFLAG_ADMIN) || !pItem )
		return false;
	if( m_nComCtlVer < COMCTL_VER(6, 0) )
		return false;
	
	if( m_hBalloon )
		::DestroyWindow(m_hBalloon), m_hBalloon = NULL;

	CItemBase * pTop = pItem->GetOwner(true);

	cStrObj strText; GetFormatedText(strText, GetString(TOR_tString, PROFILE_LOCALIZE, NULL, strTextId), pItem->GetBinder());
	if( strText.empty() )
		strText = strTextId;

	tString strCaptionIdTmp;
	if( !strCaptionId )
	{
		strCaptionIdTmp = strTextId;
		strCaptionIdTmp += GUI_STR_AUTOSECT_SEP_STR "caption";
		strCaptionId = strCaptionIdTmp.c_str();
	}
	
	cStrObj strCaption; GetFormatedText(strCaption, GetString(TOR_tString, PROFILE_LOCALIZE, NULL, strCaptionId), pItem->GetBinder());
	if( strCaption.empty() && pTop )
		LocalizeStr(strCaption, pTop->GetDisplayText());

	if( pItem != pTop )
		for(CItemBase* p = pItem; p->m_pParent; p = p->m_pParent)
		{
			if( p->m_pParent->m_nFlags & STYLE_SHEET_PAGES )
				p->Select();

			if( p->m_pParent == pTop )
				break;
		}
	
	HWND hParent = pTop->GetWindow();

	RECT rcDef = {CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
	m_hBalloon = CWindowL::CreateWnd(/*WS_EX_TOPMOST|*/WS_EX_TOOLWINDOW, TOOLTIPS_CLASSW,
		WS_POPUP|WS_VISIBLE|TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_BALLOON, rcDef, m_hWnd);

	TOOLINFOW ti = {0, };
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_TRACK|TTF_TRANSPARENT;//|TTF_CENTERTIP;
	ti.hwnd = hParent;
	ti.uId = 0;
	ti.hinst = NULL;
	ti.lpszText = (LPWSTR)strText.data();
	
	pItem->GetParentRect(ti.rect, pItem->m_pRoot);

	POINT pt = {ti.rect.left + 38*RECT_CX(ti.rect)/100, ti.rect.top + (RECT_CY(ti.rect) ? (RECT_CY(ti.rect) - 6) : 0)};

	// For multiline tooltips !!!! Requres shell ver min 4.70 (Internet Explorer 3.x)
	::SendMessageW(m_hBalloon, TTM_SETMAXTIPWIDTH, 0, 305);
	::SendMessageW(m_hBalloon, TTM_ADDTOOLW, 0, (LPARAM)&ti);
	if( strCaption.size() )
		::SendMessageW(m_hBalloon, TTM_SETTITLEW, 2, (LPARAM)strCaption.data());
//	SendMessage(m_hBalloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, 1000);
//	SendMessage(m_hBalloon, TTM_POPUP, 0, 0);
//	SendMessage(m_hBalloon, TTM_ACTIVATE, (WPARAM)TRUE, 0);
	::SendMessageW(m_hBalloon, TTM_TRACKPOSITION, 0, (LPARAM)MAKELPARAM(pt.x, pt.y));
	::SendMessageW(m_hBalloon, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&ti);
	return true;
}

void CWinRoot::RelayBalloonEvent(const MSG * pMsg)
{
	if( m_hBalloon )
		::DestroyWindow(m_hBalloon), m_hBalloon = NULL;
}

//////////////////////////////////////////////////////////////////////

bool CWinRoot::RegisterHotKey(tDWORD nVK)
{
	tDWORD nKey = nVK >> 16;
	tDWORD nModifiers = nVK & 0xffff;
	tDWORD nID = ((nModifiers & 0xff) << 8) | (nVK & 0xff);
	return !!::RegisterHotKey(m_hWnd, nID, nModifiers, nKey);
}

LRESULT CWinRoot::OnHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	((CRootSink *)m_pSink)->OnHotkey(lParam);
	return bHandled = TRUE, TRUE;
}

//////////////////////////////////////////////////////////////////////

tERROR CWinRoot::SendSrvMgrRequest(LPCWSTR strService, tSrvMgrRequest nRequest)
{
	return ::SendSrvMgrRequest(strService, nRequest);
}

tERROR CWinRoot::GetUserInfo(CUserInfo *pUserInfo)
{
	tINT nRes;

	tCHAR *pSubkey = NULL;
	if( GetVersion() & 0x80000000 )
		pSubkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion";
	else
		pSubkey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

	HKEY hKey;
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, pSubkey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
	{
		DWORD nSize;
		
		nSize = 0;
		RegQueryValueEx(hKey, "RegisteredOwner", NULL, NULL, NULL, &nSize);
		pUserInfo->m_strName.resize(nSize);
		if(nSize)
			RegQueryValueEx(hKey, "RegisteredOwner", NULL, NULL, (tBYTE *)&pUserInfo->m_strName[0], &nSize);


		nSize = 0;
		RegQueryValueEx(hKey, "RegisteredOrganization", NULL, NULL, NULL, &nSize);
		pUserInfo->m_strCompany.resize(nSize);
		if(nSize)
			RegQueryValueEx(hKey, "RegisteredOrganization", NULL, NULL, (tBYTE *)&pUserInfo->m_strCompany[0], &nSize);

		RegCloseKey(hKey);
	}

	GEOID (WINAPI * _GetUserGeoID)(GEOCLASS) = NULL;
	int   (WINAPI * _GetGeoInfoA)(GEOID, GEOTYPE, LPSTR, int, LANGID) = NULL;
	
	CLibWrapper _Kernel32("KERNEL32.DLL");
	_Kernel32.GetFunc((FARPROC *)&_GetUserGeoID, "GetUserGeoID");
	_Kernel32.GetFunc((FARPROC *)&_GetGeoInfoA, "GetGeoInfoA");

	if( _GetUserGeoID && _GetGeoInfoA )
	{
		GEOID _userGeoId = _GetUserGeoID(GEOCLASS_NATION);
		
		nRes = _GetGeoInfoA(_userGeoId, GEO_ISO2, NULL, 0, NULL);
		pUserInfo->m_strCountryISO.resize(nRes ? (nRes - 1) : 0);
		_GetGeoInfoA(_userGeoId, GEO_ISO2, &pUserInfo->m_strCountryISO[0], (int)pUserInfo->m_strCountryISO.size() + 1, NULL);
	}
	else
	{
        nRes = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, NULL, 0);
        if( nRes )
		{
			pUserInfo->m_strCountryISO.resize(nRes - 1);
	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, &pUserInfo->m_strCountryISO[0], (int)pUserInfo->m_strCountryISO.size() + 1);
		}
		else
		{
            nRes = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SISO3166CTRYNAME, NULL, 0);
			if( nRes )
			{
				pUserInfo->m_strCountryISO.resize(nRes - 1);
				GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SISO3166CTRYNAME, &pUserInfo->m_strCountryISO[0], (int)pUserInfo->m_strCountryISO.size() + 1);
			}
		}
	}
	
	return errOK;
}

void CWinRoot::OnInitLocale()
{
	// ƒл€ английской локализации не мен€ем ситемный чарсет
	if( m_nLocaleId && m_nLocaleId != 1033 )
	{
		CHAR szLInfo[10];
		if( ::GetLocaleInfo(m_nLocaleId, LOCALE_IDEFAULTANSICODEPAGE, szLInfo, sizeof(szLInfo)) )
		{
			m_nCodePage = atoi(szLInfo);

			CHARSETINFO ci;
			if( ::TranslateCharsetInfo((DWORD*)m_nCodePage, &ci, TCI_SRCCODEPAGE) )
			{
				m_nCharSet = ci.ciCharset;
				PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "GUI(Win)\tInitLocale::trnaslate ok. new charset 0x%x", m_nCharSet));
			}
			else
			{
				PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "GUI(Win)\tInitLocale::trnaslate failed. stay with charset 0x%x", m_nCharSet));
			}
		}
		else
		{
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "GUI(Win)\tInitLocale::failed. locale 0x%x", m_nLocaleId));
		}
	}

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "GUI(Win)\tANSI CodePage: %u", m_nCodePage));
}

void CWinRoot::OnGetSysDefFont(tString& strFont)
{
	if( (m_nOsVer & GUI_OSVER_VER_MASK) > 0x00040A00 )
	{
		if( (m_nOsVer & GUI_OSVER_VER_MASK) < 0x00060000 )
			strFont = "MS Shell Dlg 2"; // Tahoma под 2k, XP
		else
			strFont = "Segoe UI"; // Segoe UI под Vista
	}
	else
		strFont = "MS Shell Dlg";
}

void CWinRoot::OnGetModulePath(tObjPath& strModulePath)
{
	if( (m_nOsVer & GUI_OSVER_FAMILY_MASK) != GUI_OSVER_FAMILY_NT )
	{
		tCHAR strModuleLPath[MAX_PATH];
		if( ::GetModuleFileNameA((HMODULE)g_hInstance, strModuleLPath, countof(strModuleLPath)) )
		{
			USES_CONVERSION;
			strModulePath = A2W(strModuleLPath);
		}
	}
	else
	{
		tWCHAR strModuleLPath[MAX_PATH];
		if( ::GetModuleFileNameW((HMODULE)g_hInstance, strModuleLPath, countof(strModuleLPath)) )
			strModulePath = strModuleLPath;
	}
}

//////////////////////////////////////////////////////////////////////

CComModule _Module;	// needed for ATL headers

void CRootItem::InitInstance(HINSTANCE hInstance)
{
	g_hInstance = hInstance;
	
	INITCOMMONCONTROLSEX cc; cc.dwSize = sizeof(cc);
	cc.dwICC = ICC_DATE_CLASSES | ICC_INTERNET_CLASSES;
	InitCommonControlsEx(&cc);

	// When using ActiveX controls under ATL 7.1, the framework will ASSERT inside atlcom.h on the following line:
	// ATLASSERT(!InlineIsEqualGUID(*m_plibid,GUID_NULL) && "Did you forget to pass the LIBID to CComModule::Init?");
	// Any guid as libid is needed to avoid this
	GUID guid = { 0x2f2a90b9, 0x19ca, 0x4070, { 0x9c, 0xb8, 0x11, 0xca, 0xcd, 0x74, 0xf0, 0x23 } };
	_Module.Init(NULL, hInstance, &guid);

	if( !g_curHandId )
		g_curHandId = LoadCursor(NULL, IDC_HAND);

	if( !g_curHandId )
		g_curHandId = LoadCursor(_Module.m_hInst, MAKEINTRESOURCE(IDC_LINKHAND));

	typedef BOOL WINAPI fnISWOW64PROCESS(HANDLE hProcess, PBOOL Wow64Process);
	fnISWOW64PROCESS * fnIsWow64Process = (fnISWOW64PROCESS *)GetProcAddress(GetModuleHandle("kernel32.dll"), "IsWow64Process");

	g_bWOW64 = FALSE;
	if( fnIsWow64Process && !fnIsWow64Process(GetCurrentProcess(), &g_bWOW64) )
		g_bWOW64 = FALSE;
}

void CRootItem::ExitInstance()
{
	_Module.Term();
}

//////////////////////////////////////////////////////////////////////

CRootItem * CRootItem::CreateInstance(tDWORD nFlags, CRootSink * pSink)
{
	CRootItem * pRoot = new CWinRoot(nFlags, pSink);
	if( !pRoot )
		return NULL;
	pRoot->OnInit();
	return pRoot;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
