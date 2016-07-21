#if !defined(__KLUTIL_CLIPBOARD_H)
#define __KLUTIL_CLIPBOARD_H

#if _MSC_VER > 1000
#	pragma once
#endif // _MSC_VER > 1000

#include "defs.h"

namespace KLUTIL
{
	KLUTIL_API void EmptyClipboard(HWND hWnd = NULL);
	KLUTIL_API bool CopyToClipboard(LPCTSTR lpszText, HWND hWnd = NULL);
} // namespace KLUTIL

#endif // !defined(__KLUTIL_CLIPBOARD_H)
