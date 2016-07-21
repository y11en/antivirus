#include "stdafx.h"

BOOL ScreenshotDemo1(HWND hWnd)
{
	BOOL bFinResult = FALSE;

	HDC hSrcDC = GetDC(NULL);
	if (hSrcDC)
	{
		HDC hDstDC = CreateCompatibleDC(hSrcDC)		;
		if (hDstDC)
		{
			DWORD dx = GetDeviceCaps(hSrcDC, HORZRES);
			DWORD dy = GetDeviceCaps(hSrcDC, VERTRES);

			HBITMAP hBmp = CreateCompatibleBitmap(hSrcDC, dx, dy);
			if (hBmp)
			{
				SelectObject(hDstDC, hBmp);

				BOOL bResult = BitBlt(hDstDC, 0, 0, dx, dy, hSrcDC, 0, 0, SRCCOPY);
				if (bResult)
				{
					HDC hDrawDC = GetWindowDC(hWnd);
					if (hDrawDC)
					{
						RECT wndRect;
						bResult = GetWindowRect(hWnd, &wndRect);
						if (bResult)
						{
							bResult = StretchBlt(hDrawDC, 0, 0,
								wndRect.right-wndRect.left, wndRect.bottom-wndRect.top, hDstDC, 0, 0, dx, dy, SRCCOPY);
							bFinResult = bResult;
						}

						ReleaseDC(hWnd, hDrawDC);
					}
				}
			}

			DeleteDC(hDstDC);
		}

		ReleaseDC(NULL, hSrcDC);
	}

	return bFinResult;
}

BOOL ScreenshotDemo2(HWND hWnd)
{
	BOOL bFinResult = FALSE;

	HDC hSrcDC = GetDC(NULL);
	if (hSrcDC)
	{
		DWORD dx = GetDeviceCaps(hSrcDC, HORZRES);
		DWORD dy = GetDeviceCaps(hSrcDC, VERTRES);

		HDC hDrawDC = GetWindowDC(hWnd);
		if (hDrawDC)
		{
			RECT wndRect;
			BOOL bResult = GetWindowRect(hWnd, &wndRect);
			if (bResult)
			{
				bResult = StretchBlt(hDrawDC, 0, 0,
					wndRect.right-wndRect.left, wndRect.bottom-wndRect.top, hSrcDC, 0, 0, dx, dy, SRCCOPY);
				bFinResult = bResult;
			}

			ReleaseDC(hWnd, hDrawDC);
		}

		ReleaseDC(NULL, hSrcDC);
	}

	return bFinResult;
}