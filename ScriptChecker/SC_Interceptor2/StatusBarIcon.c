#include "statusbaricon.h"
#include "commctrl.h"

/*
VOID CALLBACK RedrawStatusBar(
				   HWND hwnd,         // handle to window
				   UINT uMsg,         // WM_TIMER message
				   UINT idEvent,      // timer identifier
				   DWORD dwTime       // current system time
				   )
{
	if (idEvent != 0)
		KillTimer(hwnd, idEvent);
	//RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);
	//PostMessage(hwnd, WM_ERASEBKGND, 0, 0);
	//PostMessage(hwnd, WM_ERASEBKGND, 0, 0);
}
*/

// HOWTO: Drawing Transparent Bitmaps 
// Q79212
void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, long xStart,
                           long yStart, COLORREF cTransparentColor)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;
	
	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hBitmap);   // Select the bitmap
	if (cTransparentColor == -1)
		cTransparentColor = GetPixel(hdcTemp, 0, 0);
	
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Get width of bitmap
	ptSize.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device
	
	// to logical points
	
	// Create some DCs to hold temporary data.
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);
	
	// Create a bitmap for each DC. DCs are required for a number of
	// GDI functions.
	
	// Monochrome DC
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	
	// Monochrome DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	
	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	
	// Each DC must select a bitmap object to store pixel data.
	bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);
	
	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));
	
	// Save the bitmap sent here, because it will be overwritten.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);
	
	// Set the background color of the source DC to the color.
	// contained in the parts of the bitmap that should be transparent
	cColor = SetBkColor(hdcTemp, cTransparentColor);
	
	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);
	
	// Set the background color of the source DC back to the original
	// color.
	SetBkColor(hdcTemp, cColor);
	
	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);
	
	// Copy the background of the main DC to the destination.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,SRCCOPY);
	
	// Mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
	
	// Mask out the transparent colored pixels on the bitmap.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	
	// XOR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);
	
	// Copy the destination to the screen.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);
	
	// Place the original bitmap back into the bitmap sent here.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);
	
	// Delete the memory bitmaps.
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));
	
	// Delete the memory DCs.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
} 

#define MAX_PARTS	0x10
BOOL DrawStatusBarBitmap(HWND hWndStatusBar, HBITMAP hBitmap, UINT nPart, UINT nAlign, LPRECT pRect)
{
	HDC hdcDest;
//	HDC hdcMem = NULL;
	BITMAP bm;
	RECT partrect;

	LONG xsize;
	LONG ysize;
	LONG xdelta;
	LONG ydelta;
	BOOL bResult = FALSE;
	UINT parts[MAX_PARTS];
	INT	 nBorders[3];
//	UINT xedge = GetSystemMetrics(SM_CXEDGE);
//	UINT yedge = GetSystemMetrics(SM_CYEDGE);

	if (nPart > MAX_PARTS || nPart == 0)
		return FALSE;
	
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	xsize = bm.bmWidth;            // Get width of bitmap
	ysize = bm.bmHeight;           // Get height of bitmap

	SendMessage(hWndStatusBar, SB_GETPARTS, MAX_PARTS, (LPARAM)&parts);
	SendMessage(hWndStatusBar, SB_GETBORDERS, 0, (LPARAM)&nBorders);
	SendMessage(hWndStatusBar, SB_GETRECT, nPart, (LPARAM)&partrect);

	partrect.top++;
	partrect.bottom--;
	partrect.left++;
	partrect.right--;

	if (partrect.right - partrect.left < xsize)
		return FALSE;
	hdcDest = GetDC(hWndStatusBar);
	if (hdcDest == NULL)
		return FALSE;

	/*
	hdcMem = CreateCompatibleDC(hdcDest);
	if (hdcMem != NULL)
	{
		HBITMAP hbmOld;
		hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
		if (hbmOld != NULL)
		{
			ydelta = (partrect.bottom - partrect.top - ysize) / 2;
			xdelta = (partrect.right - partrect.left - xsize) / 2;
			pRect->top = partrect.top+ydelta;
			pRect->bottom = partrect.bottom-ydelta;
			switch (nAlign)
			{
			case SBA_LEFT:
				pRect->left = partrect.left;
				pRect->right = partrect.left + xsize;
				break;
			case SBA_CENTER:
				pRect->left = partrect.left + xdelta;
				pRect->right = partrect.right + xdelta;
				break;
			case SBA_RIGHT:
				pRect->left = partrect.right - xsize;
				pRect->right = partrect.right;
				break;
			}
			
			FillRect(hdcDest, &partrect, (HBRUSH) (COLOR_3DFACE+1));
			//SetBkMode(hdcMem, TRANSPARENT);
			//SetBkColor(hdcMem, GetPixel(hdcMem, 0, 0));
			//BitBlt(hdcDest, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, hdcMem, 0, 0, SRCAND);
			bResult = TRUE;
			hBitmap = (HBITMAP)SelectObject(hdcMem, hbmOld);
			DrawTransparentBitmap(hdcDest, hBitmap, pRect->top, pRect->right, GetPixel(hdcMem, 0, 0));
		}
		DeleteDC(hdcMem);
	}
	*/

	ydelta = (partrect.bottom - partrect.top - ysize) / 2;
	xdelta = (partrect.right - partrect.left - xsize) / 2;
	pRect->top = partrect.top+ydelta;
	pRect->bottom = partrect.bottom-ydelta;
	switch (nAlign)
	{
	case SBA_LEFT:
		pRect->left = partrect.left;
		pRect->right = partrect.left + xsize;
		break;
	case SBA_CENTER:
		pRect->left = partrect.left + xdelta;
		pRect->right = partrect.right + xdelta;
		break;
	case SBA_RIGHT:
		pRect->left = partrect.right - xsize;
		pRect->right = partrect.right;
		break;
	}

	FillRect(hdcDest, &partrect, (HBRUSH) (COLOR_3DFACE+1));
	DrawTransparentBitmap(hdcDest, hBitmap, pRect->left, pRect->top, (COLORREF)-1);
	
	ReleaseDC(hWndStatusBar, hdcDest);
	return bResult;
}
