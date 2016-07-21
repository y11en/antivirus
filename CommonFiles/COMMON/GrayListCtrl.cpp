// CListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GrayListCtrl.h"
#include "LockedBitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define __TRACE_GRAY_LIST

/////////////////////////////////////////////////////////////////////////////
// CGrayListCtrl

CGrayListCtrl::CGrayListCtrl(bool bCheckSensitive, bool bGrayIcon ) : m_bCheckSensitive(bCheckSensitive), m_bGrayIcon(bGrayIcon)
{
	m_bDisableErasing = false;
}

CGrayListCtrl::~CGrayListCtrl()
{
}


BEGIN_MESSAGE_MAP(CGrayListCtrl, CCRoboListCtrl)
//{{AFX_MSG_MAP(CGrayListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(HDN_ENDDRAG, 0, OnHdrEnddrag)
	ON_MESSAGE_VOID(WU_DELAYEDFIT, OnDelayedFit)
	//}}AFX_MSG_MAP

	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)

#if defined(_DEBUG) && defined(__TRACE_GRAY_LIST)
	ON_WM_PAINT()
	ON_NOTIFY(HDN_ITEMCHANGINGA, 0, OnHdrChanging)
	ON_NOTIFY(HDN_ITEMCHANGINGW, 0, OnHdrChanging)
#endif

	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdrChanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdrChanged)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrayListCtrl message handlers

void CGrayListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
//for this notification, the structure is actually a
 // NMLVCUSTOMDRAW that tells you what's going on with the custom
 // draw action. So, we'll need to cast the generic pNMHDR pointer.
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

#ifdef __TRACE_GRAY_LIST
	TRACE("(0x%08x) CGrayListCtrl::OnCustomDraw: dwDrawStage = 0x%08x, hdc = 0x%08x, rc (l = %d, t = %d, r = %d, b = %d),\n"
			"\tdwItemSpec = 0x%08x, uItemState = 0x%08x, lItemlParam = 0x%08x,\n"
			"\tclrText = 0x%08x, clrTextBk = 0x%08x, iSubItem = 0x%08x\n",
			GetTickCount(),
			lplvcd->nmcd.dwDrawStage, lplvcd->nmcd.hdc,
			lplvcd->nmcd.rc.left, lplvcd->nmcd.rc.top, lplvcd->nmcd.rc.right, lplvcd->nmcd.rc.bottom,
			lplvcd->nmcd.dwItemSpec, lplvcd->nmcd.uItemState, lplvcd->nmcd.lItemlParam,
			lplvcd->clrText, lplvcd->clrTextBk, lplvcd->iSubItem);
#endif

	int iRow = lplvcd->nmcd.dwItemSpec;
	
    CWnd * pParent = GetParent();

	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
		
	case CDDS_ITEMPREPAINT:
		
		*pResult = m_bGrayIcon?CDRF_NOTIFYPOSTPAINT:CDRF_DODEFAULT;
		lplvcd ->nmcd.uItemState &= ~CDIS_FOCUS;
		

		if ( pParent && pParent->SendMessage(UM_PF2_QUERY_GRAY_TEXT, iRow) ||
			((GetExtendedStyle() & LVS_EX_CHECKBOXES) && m_bCheckSensitive && !GetCheck(iRow) ) )
		{
			lplvcd->clrText = GetSysColor(COLOR_GRAYTEXT);
		}

		break;
		
	case CDDS_ITEMPOSTPAINT:
		
		*pResult = CDRF_DODEFAULT;
		
//		if(GetExtendedStyle() & LVS_EX_CHECKBOXES)  // if we have a checkbox style,
		{
			LVITEM lvItem;
			ZeroMemory(&lvItem, sizeof (lvItem));
			
			lvItem.iItem = iRow;
			lvItem.iSubItem = 0;
			lvItem.mask = LVIF_IMAGE | LVIF_STATE;
			lvItem.stateMask = LVIS_FOCUSED; // 0xFFFFFFFF;
			
			VERIFY ( GetItem( &lvItem ) );

			GrayListLockedInfo ai;
			ai.m_bIsLocked = false;

			if ( pParent && pParent->SendMessage(UM_PF2_QUERY_GRAY_TEXT, iRow, (LPARAM)&ai) ||
				((GetExtendedStyle() & LVS_EX_CHECKBOXES) && m_bCheckSensitive && !GetCheck(iRow) ) )
			{
				CImageList* pImgLst = GetImageList  ( LVSIL_SMALL );

				if  ( pImgLst )
				{
					CRect rcIcon, rcHeader, rcIntersect;

					VERIFY ( GetItemRect  ( iRow, rcIcon, LVIR_ICON ) );
					
					GetHeaderCtrl  () -> GetItemRect  ( 0, rcHeader );
					
					rcHeader.top = rcIcon.top;
					rcHeader.bottom = rcIcon.bottom;
					
					rcIntersect.IntersectRect  ( rcHeader, rcIcon );
					
					
					HICON hIcon = ImageList_GetIcon(  pImgLst -> m_hImageList, lvItem.iImage, 0);
					
					if ( hIcon )
					{
						if ( rcIntersect.Width  () > 0 && rcIntersect.Height  () > 0 )
							DitherBlt  ( lplvcd->nmcd.hdc, rcIntersect.left, rcIntersect.top, rcIntersect.Width  (), rcIntersect.Height  (), hIcon, 0, 0, ai.m_bIsLocked);
						
						::DestroyIcon ( hIcon );
					}
					
				}
			}
			
			bool bListHasFocus = ( GetSafeHwnd  () == ::GetFocus() );
			
			if ( lvItem.state & LVIS_FOCUSED )
			{
				CRect rcLine, rcLabel;
				
				VERIFY ( GetItemRect  ( iRow, rcLabel, LVIR_LABEL ) );
				VERIFY ( GetItemRect  ( iRow, rcLine, LVIR_BOUNDS ) );
				
				rcLabel.right = rcLine.right;

				VERIFY(::DrawFocusRect  ( lplvcd->nmcd.hdc,  rcLabel ));
			
			}
		}
		
		break;
		
	default:// it wasn't a notification that was interesting to us.
		*pResult = CDRF_DODEFAULT;
	}
	
}


//
//      DitherBlt :     Draw a bitmap dithered (3D grayed effect like disabled buttons in toolbars) into a destination DC
//      Author :        Jean-Edouard Lachand-Robert (iamwired@geocities.com), June 1997.
//
//      hdcDest :       destination DC
//      nXDest :        x coordinate of the upper left corner of the destination rectangle into the DC
//      nYDest :        y coordinate of the upper left corner of the destination rectangle into the DC
//      nWidth :        width of the destination rectangle into the DC
//      nHeight :       height of the destination rectangle into the DC
//      hbm :           the bitmap to draw (as a part or as a whole)
//      nXSrc :         x coordinates of the upper left corner of the source rectangle into the bitmap
//      nYSrc :         y coordinates of the upper left corner of the source rectangle into the bitmap
//

void CGrayListCtrl::DitherBlt (const HDC hdcDest, const int nXDest, const int nYDest, const int nWidth, 
							   const int nHeight, const HICON hIcon, const int nXSrc, const int nYSrc, bool bLocked) const
{
	ASSERT(hdcDest && hIcon);
	ASSERT(nWidth > 0 && nHeight > 0);

	ICONINFO icon_info;

	if (::GetIconInfo  ( hIcon, &icon_info ))
	{
		// Create a generic DC for all BitBlts
		HDC hDC = CreateCompatibleDC(hdcDest);
		ASSERT(hDC);
		
		if (hDC)
		{
			// Create a DC for the monochrome DIB section
			HDC bwDC = CreateCompatibleDC(hDC);
			ASSERT(bwDC);
			
			if (bwDC)
			{
				// Create the monochrome DIB section with a black and white palette
				struct {
					BITMAPINFOHEADER bmiHeader; 
					RGBQUAD 		 bmiColors[2]; 
				} RGBBWBITMAPINFO = {
					
					{		// a BITMAPINFOHEADER
						sizeof(BITMAPINFOHEADER),	// biSize 
							nWidth, 				// biWidth; 
							nHeight,				// biHeight; 
							1,						// biPlanes; 
							1,						// biBitCount 
							BI_RGB, 				// biCompression; 
							0,						// biSizeImage; 
							0,						// biXPelsPerMeter; 
							0,						// biYPelsPerMeter; 
							0,						// biClrUsed; 
							0						// biClrImportant; 
					},
					
					{
						{ 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
						} 
				};
				VOID *pbitsBW;
				HBITMAP hbmBW = CreateDIBSection(bwDC,
					(LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
				ASSERT(hbmBW);
				
				if (hbmBW)
				{
					// Attach the monochrome DIB section and the bitmap to the DCs
					SelectObject(bwDC, hbmBW);
					HGDIOBJ hobj = SelectObject(hDC, icon_info.hbmColor);
					
					// BitBlt the bitmap into the monochrome DIB section
					BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);

					SelectObject  ( hDC, hobj );
					
					// Paint the destination rectangle in gray
					FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
						nHeight - 1), GetSysColorBrush(COLOR_3DFACE)); //
					
					
					// BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
					// The magic ROP comes from the Charles Petzold's book
					HBRUSH hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
					HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
					BitBlt(hdcDest, nXDest + 1, nYDest + 1, nWidth-1, nHeight-1, bwDC, 0, 0, 0xB8074A);
					
					// BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
					hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
					DeleteObject(SelectObject(hdcDest, hb));
					BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight - 1, bwDC, 0, 0, 0xB8074A);

					HDC hMaskDC = CreateCompatibleDC(hdcDest);
					ASSERT(hMaskDC);

					HGDIOBJ oldMaskBitmap = SelectObject  (hMaskDC, icon_info.hbmMask);
					BitBlt (hdcDest, nXDest, nYDest, nWidth, nHeight, hMaskDC, 0, 0, SRCPAINT);
					SelectObject  (hMaskDC, oldMaskBitmap);
					DeleteDC  (hMaskDC);
					
					DeleteObject(SelectObject(hdcDest, oldBrush));

					DeleteObject  ( hbmBW );

					if (bLocked)
					{
						HDC hBitmapDC = ::CreateCompatibleDC(hdcDest);
						HGDIOBJ pOld = ::SelectObject(hBitmapDC, g_hLockBitmap);
						
						HDC hBitmapMaskDC = ::CreateCompatibleDC(hdcDest);
						HGDIOBJ pOldMask = ::SelectObject(hBitmapMaskDC, g_hLockBitmapMask);
						
						::BitBlt(hdcDest, nXDest + nWidth - g_LockBitmapSize.cx + 2, nYDest + nHeight - g_LockBitmapSize.cy, g_LockBitmapSize.cx - 2, g_LockBitmapSize.cy, hBitmapMaskDC, 0, 0, SRCAND);
						::BitBlt(hdcDest, nXDest + nWidth - g_LockBitmapSize.cx + 2, nYDest + nHeight - g_LockBitmapSize.cy, g_LockBitmapSize.cx - 2, g_LockBitmapSize.cy, hBitmapDC, 0, 0, SRCPAINT);
						
						::SelectObject(hBitmapDC, pOld);
						::DeleteDC(hBitmapDC);
						::SelectObject(hBitmapMaskDC, pOldMask);
						::DeleteDC(hBitmapMaskDC);

					}

				}
				
				VERIFY(DeleteDC(bwDC));
			}
			
			VERIFY(DeleteDC(hDC));
		}

	VERIFY(::DeleteObject  (icon_info.hbmColor));
	VERIFY(::DeleteObject  (icon_info.hbmMask));
	
	}
}

void CGrayListCtrl::OnDelayedFit()
{
	FitLastListCtrlColumn ();
}

void CGrayListCtrl::FitLastListCtrlColumn ()
{
	if (!m_HeaderCtrl.m_hWnd)
		return;

	int nHdrItem = m_HeaderCtrl.GetItemCount ();

	if ( nHdrItem < 1 )
		return;

//	SetColumnWidth ( nHdrItem - 1, LVSCW_AUTOSIZE_USEHEADER );

	// посчитаем ширину без последнего столбца
	int nWidth = 0, i;
	for ( i = 0; i < nHdrItem - 1; ++i )
	{
		nWidth += GetColumnWidth ( m_HeaderCtrl.OrderToIndex ( i ) );
	}

	// будем считать вручную
	CRect rcWindow, rcClient;
	// GetWindowRect - без скролбаров
	GetWindowRect ( &rcWindow );
	GetClientRect ( &rcClient );
	
	// определим, будет ли выведен вертикальный скролбар
	// GetStyle() & WS_VSCROLL не работает из-за возможно установленного SetRedraw(FALSE)
	CRect rcFirstItem, rcLastItem;
	rcFirstItem.SetRectEmpty();
	rcLastItem.SetRectEmpty();

	GetItemRect(0, &rcFirstItem, LVIR_BOUNDS);
	GetItemRect(GetItemCount() - 1, &rcLastItem, LVIR_BOUNDS);
	
	bool bHasVScroll = (!rcLastItem.IsRectEmpty() && rcLastItem.bottom > rcClient.bottom) ||
						(!rcFirstItem.IsRectEmpty() && rcFirstItem.top < rcClient.top);

	int nLastWidth = rcWindow.Width() - 
				 nWidth -
				 2 * GetSystemMetrics(SM_CXEDGE) -
				 ((bHasVScroll) ? GetSystemMetrics(SM_CXHSCROLL) : 0);

	if ( nLastWidth < 1 )
	{
		nLastWidth = 1;
	}

	m_bDisableErasing = true;

	int nLastColumnIndex = m_HeaderCtrl.OrderToIndex (nHdrItem - 1);
	
	CRect rcSubItem;

	// посчитаем максимальный размер строк в столбце
	// вообще-то здесь надо учитывать не только строки, но и иконки и чекбоксы
	int nExtraWidth = 16;
	if(GetImageList(LVSIL_SMALL) != NULL)
		nExtraWidth += 9;
	if(ListView_GetExtendedListViewStyle(GetSafeHwnd()) & LVS_EX_CHECKBOXES)
		nExtraWidth += 18;
	for ( i = 0; i < GetItemCount (); ++i )
	{
		int nStrWidth = GetStringWidth ( GetItemText(i, nLastColumnIndex) ) + nExtraWidth;
		nLastWidth = max (nLastWidth, nStrWidth);
	}

	if ( GetColumnWidth( nLastColumnIndex ) != nLastWidth)
	{
		SetColumnWidth ( nLastColumnIndex, nLastWidth );
	}

	m_bDisableErasing = false;
}

BOOL CGrayListCtrl::OnEraseBkgnd(CDC* pDC) 
{
#ifdef __TRACE_GRAY_LIST
	TRACE("(0x%08x) CGrayListCtrl::OnEraseBkgnd: m_bDisableErasing = %d\n", 
		GetTickCount(),	
		m_bDisableErasing);
#endif

	if (m_bDisableErasing)
	{
		return TRUE;
	}
	
	return CCRoboListCtrl::OnEraseBkgnd(pDC);
}

void CGrayListCtrl::PreSubclassWindow() 
{
	m_HeaderCtrl.SubclassWindow((HWND)SendMessage(LVM_GETHEADER));
	
	CCRoboListCtrl::PreSubclassWindow();
}

void CGrayListCtrl::OnHdrChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMHEADER *pHdr = (NMHEADER *)pNMHDR;
	
#ifdef __TRACE_GRAY_LIST
	TRACE("(0x%08x) CGrayListCtrl::OnHdrChanged: iItem = 0x%08x, iButton = 0x%08x, mask = 0x%08x, cxy = 0x%08x,\n"
			"\tpszText = 0x%08x, hbm = 0x%08x, cchTextMax = 0x%08x, fmt = 0x%08x,\n"
			"\tlParam = 0x%08x, iImage = 0x%08x, iOrder = 0x%08x\n",
			GetTickCount (),
			pHdr->iItem, pHdr->iButton, pHdr->pitem->mask, pHdr->pitem->cxy,
			pHdr->pitem->pszText, pHdr->pitem->hbm, pHdr->pitem->cchTextMax,
			pHdr->pitem->fmt, pHdr->pitem->lParam, pHdr->pitem->iImage,	pHdr->pitem->iOrder);
#endif

	*pResult = 0;

	if ( m_bDisableErasing || (pHdr->pitem->mask & HDI_WIDTH) == 0 )
	{
		Default();
		return;
	}

	SetRedraw (FALSE);

	Default();

	FitLastListCtrlColumn ();

	SetRedraw (TRUE);

}

void CGrayListCtrl::OnHdrChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMHEADER *pHdr = (NMHEADER *)pNMHDR;

#ifdef __TRACE_GRAY_LIST
	TRACE("(0x%08x) CGrayListCtrl::OnHdrChanging: iItem = 0x%08x, iButton = 0x%08x, mask = 0x%08x, cxy = 0x%08x,\n"
			"\tpszText = 0x%08x, hbm = 0x%08x, cchTextMax = 0x%08x, fmt = 0x%08x,\n"
			"\tlParam = 0x%08x, iImage = 0x%08x, iOrder = 0x%08x\n",
			GetTickCount (),
			pHdr->iItem, pHdr->iButton, pHdr->pitem->mask, pHdr->pitem->cxy,
			pHdr->pitem->pszText, pHdr->pitem->hbm, pHdr->pitem->cchTextMax,
			pHdr->pitem->fmt, pHdr->pitem->lParam, pHdr->pitem->iImage,	pHdr->pitem->iOrder);
#endif
	
	*pResult = 0;

	Default();
}

void CGrayListCtrl::OnPaint() 
{
#ifdef __TRACE_GRAY_LIST
	TRACE("(0x%08x) CGrayListCtrl::OnPaint\n", GetTickCount ());
#endif

	Default();
	
}

void CGrayListCtrl::OnHdrEnddrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	PostMessage (WU_DELAYEDFIT);

	Default();

	*pResult = 0;
}

void CGrayListCtrl::InvalidateRow(int iRow)
{
	CRect rc;
	GetItemRect  (iRow, rc, LVIR_BOUNDS);
	InvalidateRect  (rc);
}