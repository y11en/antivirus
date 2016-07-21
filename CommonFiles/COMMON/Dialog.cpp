//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "Dialog.h"
#include "XPThemeHelper.h"
#include "util.h"
#include <windowsx.h>

#define UM_FORCE_SETWINDOWPLACEMENT	(WM_USER + 0x6D)

typedef std::map<HWND, std::basic_string<TCHAR> > HWND2STRING_MAP;

struct CCMapWrapper
{
	HWND2STRING_MAP m_TooltipMap;
};


//******************************************************************************
// class CCDialog
//******************************************************************************
IMPLEMENT_DYNAMIC(CCDialog, CDialog)

BEGIN_MESSAGE_MAP(CCDialog, CDialog)
    //{{AFX_MSG_MAP(CCDialog)
    ON_WM_LBUTTONDOWN()
    ON_WM_NCHITTEST()
    ON_WM_DESTROY()
    ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE(UM_FORCE_SETWINDOWPLACEMENT, OnSetWindowPlacement)
END_MESSAGE_MAP()

const char * CCDialog::s_pszWindowPlacementKey = "WindowPlacement";

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
CCDialog::CCDialog ( UINT nIDTemplate, CWnd * pParent, _dword Flags, LPCTSTR pszSectionName, CSize InitialSize )
:   CDialog ( nIDTemplate, pParent ),
    m_Flags ( Flags ),
	m_hWndParent(NULL),
	m_bEnableParent(FALSE),
	m_hAsyncDialogTemplate(NULL),
	m_bModalStyle(true),
	m_bModalAsync(false),
	m_bShow(true),
	TOOLTIP_TIMER_ID(0),
	m_hwndChild(NULL)
{
    if  ( pszSectionName )
    {
        m_SectionName = pszSectionName;
    }

	m_InitialSize = InitialSize;
    
    m_MinRect.SetRectEmpty ();

	m_TitleTip.SetWrapLongText ( TRUE );
	m_TitleTip.m_nDelayConst = 0;
	m_TitleTip.SetAutoHide(false);
	m_TitleTip.SetAbsolutePositionFlag(false);

	m_pTooltipMap = new CCMapWrapper;
    
    //{{AFX_DATA_INIT(CCDialog)
    //}}AFX_DATA_INIT
}

CCDialog::~CCDialog()
{
	delete m_pTooltipMap;
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCDialog::OnLButtonDown ( UINT nFlags, CPoint point )
{
    CDialog::OnLButtonDown ( nFlags, point );
    
    if   ( m_Flags & DIALOG_FLAGS_MOVE_CONTEXT )
    {
        PostMessage ( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM ( point.x, point.y ) );
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
LRESULT CCDialog::OnNcHitTest ( CPoint point )
{
    UINT nHitTest = CDialog::OnNcHitTest ( point );
    
    if   ( m_Flags & DIALOG_FLAGS_GRIPPER )
    {
        // lets check gripper rectangle
        CRect rcGripper;
        GetWindowRect ( &rcGripper );
        rcGripper.left = rcGripper.right - ::GetSystemMetrics ( SM_CXHSCROLL );
        rcGripper.top = rcGripper.bottom - ::GetSystemMetrics ( SM_CYVSCROLL );
        
        if   ( rcGripper.PtInRect ( point ) )	
        {
            return HTBOTTOMRIGHT;
        }
    }
    //return ( ( nHitTest == HTCLIENT ) && ( m_Flags & DIALOG_FLAGS_MOVE_CONTEXT ) ) ? HTCAPTION : nHitTest;
    return nHitTest;
}

//------------------------------------------------------------------------------
// загружает положение окна
//      вызывающая функция должна удалить возвращаемый указатель
//------------------------------------------------------------------------------
WINDOWPLACEMENT * CCDialog::LoadWindowPlacement ()
{
    if   ( m_SectionName.IsEmpty () == FALSE )
    {
        WINDOWPLACEMENT * pwndpl;
        UINT nBytes;
        
        if  ( AfxGetApp () -> GetProfileBinary ( m_SectionName, s_pszWindowPlacementKey, ( LPBYTE * ) &pwndpl, &nBytes ) )
        {
            return pwndpl;
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
// сохраняет положение окна
//------------------------------------------------------------------------------
void CCDialog::SaveWindowPlacement ()
{
    if   ( m_SectionName.IsEmpty () == FALSE )
    {
        WINDOWPLACEMENT wndpl;
        wndpl.length = sizeof ( wndpl );
        
        if   ( GetWindowPlacement ( &wndpl ) )
        {
            AfxGetApp () -> WriteProfileBinary ( m_SectionName, s_pszWindowPlacementKey,
                ( LPBYTE ) &wndpl, wndpl.length );
        }
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL CCDialog::OnInitDialog ()
{
    CDialog::OnInitDialog ();
	
    CRect rect;
    GetWindowRect ( rect );
	MinRect  (rect);

	PostMessage(UM_FORCE_SETWINDOWPLACEMENT);

	if (m_Flags & DIALOG_FLAGS_MAKE_TOPMOST)
		SetWindowPos (&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	m_TitleTip.Create ( this );

    return TRUE;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCDialog::OnDestroy ()
{
	if (m_nFlags & WF_MODALLOOP)
	{
		// При сворачивании окон возможна ситуация, когда ::DestroyWindow
		// вызывается для диалога, который все еще находится в модальном цикле.
		// Чтобы выйти из цикла вызываем OnCancel
		OnCancel();
	}

	if (TOOLTIP_TIMER_ID)
		KillTimer(TOOLTIP_TIMER_ID);

	OnSaveSettings();
    SaveWindowPlacement ();
    CDialog::OnDestroy ();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCDialog::DrawGripper ( HDC hDC )
{
    if   ( !IsZoomed () && !IsIconic () )
    {
        CRect rc;
        GetClientRect ( &rc );
        rc.left = rc.right - ::GetSystemMetrics ( SM_CXHSCROLL );
        rc.top = rc.bottom - ::GetSystemMetrics ( SM_CYVSCROLL );
        
        if   ( CCXPThemeHelper::CanApplyXPStyle () )
        {
            HTHEME hTheme = CCXPThemeHelper::OpenThemeData ( GetSafeHwnd (), L"STATUS" );
            CCXPThemeHelper::DrawThemeBackground ( hTheme, hDC, SP_GRIPPER, 0, &rc, NULL );
            CCXPThemeHelper::CloseThemeData ( hTheme );
        }
        else
        {
            ::DrawFrameControl ( hDC, &rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
        }
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCDialog::OnPaint ()
{
    CPaintDC dc ( this );
    
    if   ( m_Flags & DIALOG_FLAGS_GRIPPER )
    {
        DrawGripper ( dc.m_hDC );
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCDialog::OnGetMinMaxInfo ( MINMAXINFO FAR * lpMMI )
{
    CDialog::OnGetMinMaxInfo ( lpMMI );
    
    if  ( m_MinRect.IsRectEmpty () == FALSE )
    {
        lpMMI -> ptMinTrackSize.x = m_MinRect.Width ();
        lpMMI -> ptMinTrackSize.y = m_MinRect.Height ();
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

UINT CCDialog::GetProfileInt( LPCTSTR lpszEntry, int nDefault )
{
	return AfxGetApp () -> GetProfileInt( (LPCTSTR)m_SectionName, lpszEntry, nDefault);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

BOOL CCDialog::WriteProfileInt( LPCTSTR lpszEntry, int nValue)
{
	return AfxGetApp () -> WriteProfileInt( (LPCTSTR)m_SectionName, lpszEntry, nValue);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

CString CCDialog::GetProfileString( LPCTSTR lpszEntry, LPCTSTR lpszDefault/* = NULL*/)
{
	return AfxGetApp () -> GetProfileString( (LPCTSTR)m_SectionName, lpszEntry, lpszDefault);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

BOOL CCDialog::WriteProfileString( LPCTSTR lpszEntry,	LPCTSTR lpszValue )
{
	return AfxGetApp () -> WriteProfileString( (LPCTSTR)m_SectionName, lpszEntry, lpszValue);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

BOOL CCDialog::GetProfileBinary( LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes)
{
    return AfxGetApp () -> GetProfileBinary( (LPCTSTR)m_SectionName, lpszEntry, ppData, pBytes );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
BOOL CCDialog::WriteProfileBinary( LPCTSTR lpszEntry,	LPBYTE pData, UINT nBytes)
{
    return AfxGetApp () -> WriteProfileBinary( (LPCTSTR)m_SectionName, lpszEntry, pData, nBytes);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

LRESULT CCDialog::OnSetWindowPlacement(WPARAM, LPARAM)
{
	TRACE("CCDialog::OnSetWindowPlacement\n");

	if ( !m_SectionName.IsEmpty () )
	{
		WINDOWPLACEMENT * pwndpl = LoadWindowPlacement ();
		
		if  ( pwndpl )
		{
			TRACE("CCDialog::OnSetWindowPlacement SetWindowPlacement\n");
			if ( pwndpl->showCmd == SW_SHOWMINIMIZED )
				pwndpl->showCmd = SW_SHOWNORMAL;

			if (!m_bShow)
				pwndpl->showCmd = SW_HIDE;
			
			if (m_Flags & DIALOG_FLAGS_PLACEMENT_NOSIZE)
			{
				WINDOWPLACEMENT wndpl;
				wndpl.length = sizeof ( wndpl );
				if   ( GetWindowPlacement ( &wndpl ) )
				{
					pwndpl->rcNormalPosition.right = pwndpl->rcNormalPosition.left + 
						(wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left);
					pwndpl->rcNormalPosition.bottom = pwndpl->rcNormalPosition.top + 
						(wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top);
				}
			}
			
			VERIFY(SetWindowPlacement ( pwndpl ));
			delete [] pwndpl;

			return 0;
		}

		if ( m_InitialSize.cx && m_InitialSize.cy )
		{
			WINDOWPLACEMENT wndpl;
			wndpl.length = sizeof ( wndpl );
			
			if   ( GetWindowPlacement ( &wndpl ) )
			{
				wndpl.showCmd = m_bShow?SW_SHOWNORMAL:SW_HIDE;

				CRect rcNormal (wndpl.rcNormalPosition);

				if (m_InitialSize.cx > rcNormal.Width ())
				{
					wndpl.rcNormalPosition.left -= (m_InitialSize.cx - rcNormal.Width ())/2;
					wndpl.rcNormalPosition.right += (m_InitialSize.cx - rcNormal.Width ())/2;

					if (m_InitialSize.cy > rcNormal.Height ())
					{
						wndpl.rcNormalPosition.top -= (m_InitialSize.cy - rcNormal.Height ())/2;
						wndpl.rcNormalPosition.bottom += (m_InitialSize.cy - rcNormal.Height ())/2;

						VERIFY(SetWindowPlacement ( &wndpl ));

						return 0;
					}

				}
				
			}
		}

		if (m_bShow)
		{
			ShowWindow(SW_SHOWNORMAL);
			UpdateWindow();
		}
	}
	
	return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

int CCDialog::DoModal()
{
	// can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
		m_lpDialogTemplate != NULL);

	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != NULL)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}
	
	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
			CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = m_SectionName.IsEmpty ()?MLF_SHOWONIDLE:0;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}
			
			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		//		DELETE_EXCEPTION(e);
		m_nModalResult = -1;
	}
	END_CATCH_ALL
		
		if (bEnableParent)
			::EnableWindow(hWndParent, TRUE);
		if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
			::SetActiveWindow(hWndParent);
		
		// destroy modal window
		DestroyWindow();
		PostModal();
		
		// unlock/free resources as necessary
		if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
			UnlockResource(hDialogTemplate);
		if (m_lpszTemplateName != NULL)
			FreeResource(hDialogTemplate);
		
		return m_nModalResult;
}

bool CCDialog::DoModalAsync()
{
	// can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
		m_lpDialogTemplate != NULL);

	ASSERT (m_bModalAsync == false);

	m_bModalAsync = true;
	
	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL m_hAsyncDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != NULL)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		m_hAsyncDialogTemplate = LoadResource(hInst, hResource);
	}
	if (m_hAsyncDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(m_hAsyncDialogTemplate);
	
	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return false;
	
	// disable parent (before creating dialog)
	m_hWndParent = PreModal();
	AfxUnhookWindowCreate();
	m_bEnableParent = FALSE;
	if (m_hWndParent != NULL && ::IsWindowEnabled(m_hWndParent))
	{
		::EnableWindow(m_hWndParent, FALSE);
		m_bEnableParent = TRUE;
	}
	
	bool bRet = false;
	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
			CWnd::FromHandle(m_hWndParent), hInst))
		{
			InvalidateRect (NULL);
			ShowWindow(SW_SHOW);

			UpdateWindow ();

			bRet = true;
/*
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = m_SectionName.IsEmpty ()?MLF_SHOWONIDLE:0;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}
			
			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
*/		}
	}
	CATCH_ALL(e)
	{
		m_nModalResult = -1;
	}
	END_CATCH_ALL
	
	return bRet;	
}

void CCDialog::NormalizeDlgFocus(CWnd* pWindow2Disable)
{
	if (::GetFocus() == pWindow2Disable->GetSafeHwnd  ())
	{
		CWnd* pNextItem = GetNextDlgTabItem(pWindow2Disable);
		
		if ( pNextItem && ::IsWindow(pNextItem->GetSafeHwnd  ()) )
		{
			pNextItem->SetFocus  ();
		}
	}
}


//==============================================================================
void CCDialog::OnClose() 
{
	CDialog::OnClose();

	if (m_bModalAsync)
		DestroyModelessDialog();

}

void CCDialog::DestroyModelessDialog()
{
	ASSERT (m_bModalAsync);
	
	if (!IsWindow (GetSafeHwnd ()))
		return;

	if (m_bEnableParent)
		::EnableWindow(m_hWndParent, TRUE);
	if (m_hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(m_hWndParent);

	AfxUnhookWindowCreate();   // just in case
	
	// re-enable windows
	if (::IsWindow(m_hWndTop))
		::EnableWindow(m_hWndTop, TRUE);
	m_hWndTop = NULL;
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL)
		pApp->EnableModeless(TRUE);

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(m_hAsyncDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(m_hAsyncDialogTemplate);

	DestroyWindow ();
}

void CCDialog::OnOK() 
{
	CDialog::OnOK();
	if (m_bModalAsync)
		DestroyModelessDialog();

}

void CCDialog::OnCancel() 
{
	CDialog::OnCancel();
	if (m_bModalAsync)
		DestroyModelessDialog();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CCDialog::PostNcDestroy ()
{
    CDialog::PostNcDestroy ();
    
    if  ( m_bModalStyle == false )
    {
		delete this;
    }
}

void CCDialog::EndDialog(int nResult)
{
	CDialog::EndDialog (nResult);

	if (m_bModalAsync)
		DestroyModelessDialog();
}

void CCDialog::SetCtrlTooltip(HWND hWndCtrl, LPCTSTR szTooltip)
{
	if (::IsWindow(hWndCtrl))
		m_pTooltipMap->m_TooltipMap[hWndCtrl] = szTooltip;
}

void CCDialog::SetCtrlTooltip(int nID, LPCTSTR szTooltip)
{
	SetCtrlTooltip (::GetDlgItem ( GetSafeHwnd(), nID ), szTooltip);
}

BOOL CCDialog::PreTranslateMessage(MSG* pMsg) 
{
    if  ( ::IsWindow ( m_TitleTip.GetSafeHwnd () ) )
    {
        m_TitleTip.PreTranslateMessage ( pMsg );
    }
	
	if (pMsg->message == WM_MOUSEMOVE)
	{
		CPoint point ( GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) );
		::ClientToScreen (pMsg->hwnd, &point);
		ScreenToClient(&point);

	    HWND hWndChild = ::ChildWindowFromPoint ( GetSafeHwnd (), point );
    
		if  ( ::IsWindow(hWndChild) && m_ptMouseClientPos != point)
		{
			HWND2STRING_MAP::iterator i = m_pTooltipMap->m_TooltipMap.find(hWndChild);
			if ( i != m_pTooltipMap->m_TooltipMap.end() )
			{
				if (TOOLTIP_TIMER_ID)
					KillTimer(TOOLTIP_TIMER_ID);

				TOOLTIP_TIMER_ID = SetTimer (17, GetDoubleClickTime (), NULL);
				TRACE("SetTimer(TOOLTIP_TIMER_ID)\n");
				
				m_ptMouseClientPos = point;
				m_hwndChild = hWndChild;
				m_szTooltipNext = i->second;

			}
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CCDialog::OnTimer(UINT nIDEvent) 
{
	if 	(nIDEvent == TOOLTIP_TIMER_ID)
	{
		TRACE("OnTimer(TOOLTIP_TIMER_ID)\n");
		KillTimer(TOOLTIP_TIMER_ID);
		TOOLTIP_TIMER_ID = 0;
		
		CPoint ptCur;
		VERIFY(::GetCursorPos(&ptCur));
		ScreenToClient(&ptCur);

		if (abs(ptCur.x - m_ptMouseClientPos.x) < GetSystemMetrics(SM_CXDOUBLECLK) &&
			abs(ptCur.y - m_ptMouseClientPos.y) < GetSystemMetrics(SM_CYDOUBLECLK) &&
			::ChildWindowFromPoint ( GetSafeHwnd (), ptCur ) == m_hwndChild)
		{
			CRect rect;
			::GetWindowRect (m_hwndChild, rect);
			ScreenToClient ( &rect );
			
			m_TitleTip.Show ( rect, m_szTooltipNext.c_str(), true );				
		}
	}

	CDialog::OnTimer(nIDEvent);
}
