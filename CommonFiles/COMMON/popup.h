//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_POPUP_H
#define __COMMON_POPUP_H

#include "defs.h"

COMMONEXPORT BOOL _SetForegroundWindowEx ( HWND hWnd );

COMMONEXPORT void PopupWindow ( CWnd * pWnd, bool bMakeTopmostOnFail = true );

#endif // __COMMON_POPUP_H

//==============================================================================
