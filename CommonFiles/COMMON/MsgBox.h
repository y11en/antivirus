//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_MSGBOX_H
#define __COMMON_MSGBOX_H

#include "defs.h"

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxEx ( HWND hWnd, LPCTSTR pszText, LPCSTR pszCaption = NULL, UINT nType = MB_OK, HICON hIcon = NULL );

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxEx ( HWND hWnd, UINT nTextId, UINT nCaptionId = -1, UINT nType = MB_OK, HICON hIcon = NULL );

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxCBEx ( LPCTSTR pszCheckBoxString, bool & bCheckBoxState, HWND hWnd, LPCTSTR pszText, LPCSTR pszCaption = NULL, UINT nType = MB_OK, HICON hIcon = NULL );

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxCBEx ( LPCTSTR pszCheckBoxString, bool & bCheckBoxState, HWND hWnd, UINT nTextId, UINT nCaptionId = -1, UINT nType = MB_OK, HICON hIcon = NULL );

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int MsgBoxOkCancel ( HWND hWnd, LPCTSTR pszText, LPCTSTR pszOKText = NULL, LPCTSTR pszCancelText = NULL);


#endif // __COMMON_MSGBOX_H

//==============================================================================
