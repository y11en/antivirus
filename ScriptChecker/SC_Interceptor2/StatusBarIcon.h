#ifndef __status_bar_icon__
#define __status_bar_icon__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#ifndef SBA_LEFT
#define SBA_LEFT	0
#define SBA_CENTER	1
#define SBA_RIGHT	2
#endif

BOOL DrawStatusBarBitmap(HWND hWndStatusBar, HBITMAP hBitmap, UINT nPart, UINT nAlign, LPRECT pRect);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __status_bar_icon__
 