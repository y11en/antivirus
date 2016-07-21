//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "popup.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT BOOL _SetForegroundWindowEx ( HWND hWnd )
{
	if (hWnd == NULL)
		return FALSE;
	
	// determine current foreground window
	HWND hWndForeground = ::GetForegroundWindow ();

	if (hWndForeground == NULL)
		return ::SetForegroundWindow (hWnd);

	// check whether it is able to pump messages to avoid deadlocks
	DWORD dwResult = 0;

    if  ( SendMessageTimeout ( hWndForeground, WM_NULL, 0, 0, SMTO_NORMAL, 1000, &dwResult ) == 0 )
		return FALSE;

	int iTID = ::GetWindowThreadProcessId ( hWnd, 0 );
	int iForegroundTID  = ::GetWindowThreadProcessId ( hWndForeground, 0 );
	
	AttachThreadInput (iTID, iForegroundTID, TRUE);
	
	::SetForegroundWindow (hWnd);
	
	AttachThreadInput (iTID, iForegroundTID, FALSE);

    return TRUE;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT void PopupWindow ( CWnd * pWnd, bool bMakeTopmostOnFail )
{
	if  ( pWnd && IsWindow ( pWnd -> GetSafeHwnd () ) )
    {
        if  ( !_SetForegroundWindowEx ( pWnd -> GetSafeHwnd () ) )
        {
            pWnd ->SetForegroundWindow ();
            
            if  ( bMakeTopmostOnFail )
            {
                pWnd -> SetWindowPos ( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            }
        }
        pWnd -> BringWindowToTop ();
		pWnd -> RedrawWindow  (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
//        pWnd -> FlashWindow ( TRUE );
    }
}

//==============================================================================
