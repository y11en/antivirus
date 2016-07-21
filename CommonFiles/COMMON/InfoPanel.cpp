// InfoPanel.cpp : implementation file
//

#include "stdafx.h"
#include "InfoPanel.h"
#include "RichText.h"
#include "ResourceHelper.h"
#include "util.h"
#include "resource.h"
#include "MemDC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INFO_SPACING	10
/////////////////////////////////////////////////////////////////////////////
// CInfoPanel
CInfoPanel::CInfoPanel(long nFlag /* = INFO_PANEL_FLAG_ICONED*/, bool bCustomRedraw/* = false*/) :
m_nInfoPanelFlag(nFlag),
m_nIconSize((nFlag & INFO_PANEL_FLAG_LARGE_ICON) ? 32 : ((nFlag & INFO_PANEL_FLAG_ICONED) ? 24 : 0)),
m_bCustomRedraw(bCustomRedraw)
{
	m_bCustomColor = false;
	m_bkColor = ::GetSysColor((nFlag & INFO_PANEL_FLAG_YELLOW)?COLOR_INFOBK:COLOR_3DFACE);
	m_bUpdateParent = false;
	
}

CInfoPanel::~CInfoPanel()
{
}


BEGIN_MESSAGE_MAP(CInfoPanel, CStatic)
//{{AFX_MSG_MAP(CInfoPanel)
ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE ()
	//}}AFX_MSG_MAP
ON_NOTIFY(EN_LINK, 1, OnLinkNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoPanel message handlers
//WS_VSCROLL

void CInfoPanel::CalcIconRect(CRect& rcIco)
{
	if(!m_nIconSize)
		return;

	GetClientRect  (rcIco);

	if (m_nInfoPanelFlag & INFO_PANEL_FLAG_YELLOW)
	{
		rcIco.left += INFO_SPACING;
		rcIco.top += INFO_SPACING;
	}
	rcIco.right = rcIco.left + m_nIconSize;
	rcIco.bottom = rcIco.top + m_nIconSize;
}

void CInfoPanel::CalcTextRect(CRect& rcText)
{
	GetClientRect  (rcText);

	if (m_nIconSize)
		rcText.left+= m_nIconSize + INFO_SPACING;


	
	if (m_nInfoPanelFlag & INFO_PANEL_FLAG_YELLOW)
	{
		rcText.left += INFO_SPACING;
		rcText.right -= INFO_SPACING;
		rcText.top += INFO_SPACING/2;
		rcText.bottom -= INFO_SPACING/2;
	}
	
	if ((rcText.right < rcText.left) || (rcText.bottom<rcText.top))
		rcText = CRect(0,0,0,0);
	
}

void CInfoPanel::PreSubclassWindow() 
{
	
	if (m_nIconSize)
	{
		CRect rcIco;
		CalcIconRect(rcIco);
		VERIFY( m_Ico.Create	(_T(""), WS_VISIBLE | WS_CHILD, rcIco, this) );
	}
	
	CRect rcText;
	CalcTextRect(rcText);
	
	VERIFY( m_Text.CreateEx	(WS_EX_TRANSPARENT, "RICHEDIT20A", _T(""), (m_nInfoPanelFlag & INFO_PANEL_FLAG_VSCROLL)?0x50200804:0x50000804, rcText, this, 1));

	CreateKAVFont(this, m_IPFont);

	if((HFONT)m_IPFont)
	{
		SetFont(&m_IPFont, TRUE);
		m_Text.SetFont(&m_IPFont, TRUE);
	}
}

void CInfoPanel::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	CRect rc;

	GetClientRect  (rc);
	
	if ( ::IsWindow(m_Text.GetSafeHwnd  ()) )
	{
		CRect rcText;
		CalcTextRect(rcText);

		if (m_nIconSize && ::IsWindow  (m_Ico.GetSafeHwnd  ()))
		{
			CRect rcIco;
			CalcIconRect(rcIco);
			m_Ico.MoveWindow  (rcIco.left, rcIco.top, rcIco.Width  (), rcIco.Height  (), FALSE);
			m_Text.MoveWindow  (rcText.left, rcText.top, rcText.Width  (), rcText.Height  (), TRUE);
		}
		else
			m_Text.MoveWindow  (rcText.left, rcText.top, rcText.Width  (), rcText.Height  (), TRUE);
	}
}


void CInfoPanel::OnLinkNotify ( NMHDR * pNotifyHeader, LRESULT * pResult )
{
	*pResult = 0;
	HWND hwnd = GetSafeHwnd  ();
	HWND hParent = ::GetParent(hwnd);
	if (pNotifyHeader && ::IsWindow(hParent))
	{
		
		ENLINK enLink, *pOriginal;
		pOriginal = (ENLINK*)pNotifyHeader;
		
		
		enLink.nmhdr.hwndFrom = hwnd;
		enLink.nmhdr.idFrom = ::GetWindowLong  (hwnd, GWL_ID);
		enLink.nmhdr.code = pOriginal -> nmhdr.code;
		
		enLink.chrg = pOriginal -> chrg;
		enLink.msg = pOriginal -> msg;
		
		::SendMessage(hParent, WM_NOTIFY, (WPARAM)enLink.nmhdr.idFrom, (LPARAM)&enLink);
		
	}
}

void CInfoPanel::SetStatus(StatType stat, LPCTSTR szText)
{

	//ASSERT(m_nIconSize>0);
	
	//	SetRedraw (FALSE);
	
	VERIFY(CRichText::SetHyperText( m_Text.GetSafeHwnd  (), szText ));

//	if (m_bCustomRedraw)
//		m_Text.SetRedraw (FALSE);


	if ( m_nIconSize && ::IsWindow  (m_Ico.GetSafeHwnd  ()) )
	{
		switch ( stat )
		{
		case Stat_Error:
			{
				UINT nResID = (m_nIconSize==24)?IDR_COMMON_PNG_ERROR_SMALL:IDR_COMMON_PNG_ERROR_BIG;
				m_hResInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResID),"PNG");
				ASSERT (m_hResInst);
				m_PngRes = ::FindResource (m_hResInst, MAKEINTRESOURCE(nResID),"PNG");
				m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
			}
			break;
			
		case Stat_Warning:
			{
				UINT nResID = (m_nIconSize==24)?IDR_COMMON_PNG_WARN_SMALL:IDR_COMMON_PNG_WARN_BIG;
				m_hResInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResID),"PNG");
				ASSERT (m_hResInst);
				m_PngRes = ::FindResource (m_hResInst, MAKEINTRESOURCE(nResID),"PNG");
				m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
			}
			break;
			
		case Stat_Info:
			{
				UINT nResID = (m_nIconSize==24)?IDR_COMMON_PNG_INFO_SMALL:IDR_COMMON_PNG_INFO_BIG;
				m_hResInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResID),"PNG");
				ASSERT (m_hResInst);
				m_PngRes = ::FindResource (m_hResInst, MAKEINTRESOURCE(nResID),"PNG");
				m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
			}
			break;

		case Stat_Pause:
			{
				UINT nResID = (m_nIconSize==24)?IDR_COMMON_PNG_PAUSE_SMALL:IDR_COMMON_PNG_PAUSE_BIG;
				m_hResInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResID),"PNG");
				ASSERT (m_hResInst);
				m_PngRes = ::FindResource (m_hResInst, MAKEINTRESOURCE(nResID),"PNG");
				m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
			}
			break;
			
		case Stat_Ok:
			{
				UINT nResID = (m_nIconSize==24)?IDR_COMMON_PNG_OK_SMALL:IDR_COMMON_PNG_OK_BIG;
				m_hResInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResID),"PNG");
				ASSERT (m_hResInst);
				m_PngRes = ::FindResource (m_hResInst, MAKEINTRESOURCE(nResID),"PNG");
				m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
			}
			break;
			
		default: ASSERT(FALSE);
			
		}
	}

	
//	SetRedraw (TRUE);

	CWnd *pParent = GetParent ();

	if ( !::IsWindow(pParent->GetSafeHwnd ()) )
		return;

	if (m_bCustomRedraw)
	{
		m_Ico.InvalidateRect (NULL);
		m_Ico.UpdateWindow ();
	}
	else
	{
		CRect rc;
		m_Text.GetWindowRect (rc);
		pParent->ScreenToClient (rc);
		pParent->InvalidateRect (rc);
		
		if ( m_nIconSize && ::IsWindow  (m_Ico.GetSafeHwnd  ()) )
		{
			m_Ico.GetWindowRect (rc);
			pParent->ScreenToClient (rc);
			pParent->InvalidateRect (rc);
		}
	}
}

bool CInfoPanel::SetText(LPCTSTR szText)
{


//	SetRedraw (FALSE);

	bool bResult = CRichText::SetHyperText( m_Text.GetSafeHwnd  (), szText );

//	if (m_bCustomRedraw)
//		m_Text.SetRedraw (FALSE);


//	SetRedraw (TRUE);

	if (bResult)
	{
		CWnd *pParent = GetParent ();
		if (pParent)
		{
			CRect rc;
			m_Text.GetWindowRect (rc);
			pParent->ScreenToClient (rc);
			pParent->InvalidateRect (rc);
		}
	}
	
	return bResult;
}

bool CInfoPanel::SetPngImage (LPCTSTR lpResName, LPCTSTR lpResType)
{
	if ( m_nIconSize && ::IsWindow  (m_Ico.GetSafeHwnd  ()) )
	{
		HINSTANCE hResInst = AfxFindResourceHandle(lpResName, lpResType);
		ASSERT (hResInst);
		return m_Ico.LoadImage  (hResInst, ::FindResource(hResInst, lpResName, lpResType), m_bkColor);
	}

	return false;
}

DWORD CInfoPanel::GetClickedLinkID(NMHDR * pNotifyHeader)
{
	return CRichText::GetLinkID(m_Text.GetSafeHwnd  (), 0, (LPARAM)pNotifyHeader);
}

void CInfoPanel::OnPaint() 
{
	CWnd* pParent = GetParent();

	if (!::IsWindow (pParent->GetSafeHwnd ()))
	{
		Default();
		return;
	}

	CRect rcUpdate;
	GetUpdateRect  (rcUpdate);
	{
		CPaintDC dc(this); // device context for painting
		
		CRect rcClient;
		GetClientRect  (rcClient);
		
		if(m_nInfoPanelFlag & INFO_PANEL_FLAG_YELLOW)
		{
			CBrush brushYellow(::GetSysColor(COLOR_INFOBK));
			CBrush* pOldBrush = dc.SelectObject(&brushYellow);
			
			// create and select a thick, black pen
			CPen penYellow;
			penYellow.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_MENUTEXT));
			CPen* pOldPen = dc.SelectObject(&penYellow);
			
			UpdateParentRect (dc, CRect (0, 0, 10, 10));
			UpdateParentRect (dc, CRect (0, rcClient.bottom - 10, 10, rcClient.bottom));
			UpdateParentRect (dc, CRect (rcClient.right - 10, 0, rcClient.right, 10));
			UpdateParentRect (dc, CRect (rcClient.right - 10, rcClient.bottom - 10, rcClient.right, rcClient.bottom));
			
			dc.RoundRect(rcClient, CPoint(10, 10));
			
			dc.SelectObject(pOldBrush);
			dc.SelectObject(pOldPen);	
		}
		else
		{
			if (m_bCustomRedraw)
			{

				CRect rcDrawOffset;
				GetWindowRect(rcDrawOffset);
				pParent->ScreenToClient (rcDrawOffset);

				CDC MemDC;
				MemDC.CreateCompatibleDC ( &dc );

				CBitmap bitmap;
				bitmap.CreateCompatibleBitmap ( &dc, rcDrawOffset.Width (), rcDrawOffset.Height () );
				CBitmap* pOldBitmap = MemDC.SelectObject ( &bitmap );

				CPoint pt = MemDC.SetViewportOrg (CPoint(-rcDrawOffset.left, -rcDrawOffset.top));



//				CPoint pt = dc.SetViewportOrg (CPoint(rcDrawOffset.left, rcDrawOffset.top));
//				dc.ExcludeClipRect (CRect(-rcDrawOffset.left, -rcDrawOffset.top, 10000, 0));
//				dc.ExcludeClipRect (CRect(-rcDrawOffset.left, rcClient.Height (), 10000, 10000));

				pParent->SendMessage(WM_ERASEBKGND, (WPARAM)MemDC.m_hDC, 0);

//				MemDC.SetViewportOrg (pt);


//				m_Ico.DrawCtrl(&MemDC);
				CRect rcIco;
				m_Ico.GetWindowRect(rcIco);
				ScreenToClient (rcIco);
				dc.ExcludeClipRect (rcIco);






//				FORMATRANGE fr;
//				fr.hdc = fr.hdcTarget = MemDC.m_hDC;
//				fr.chrg.cpMin = 0;
//				fr.chrg.cpMax = -1;
//				// all of the rects need to be in twips
//
//				CRect rcText;
//				m_Text.GetWindowRect (rcText);
//				pParent->ScreenToClient (rcText);
//
//
//				rcText.DeflateRect(1, 0, 1, 0);
//				
//				fr.rc.top = fr.rcPage.top = rcText.top * 1440/MemDC.GetDeviceCaps (LOGPIXELSY);
//				fr.rc.left = fr.rcPage.left = rcText.left * 1440/MemDC.GetDeviceCaps (LOGPIXELSX);
//				fr.rc.right = fr.rcPage.right = rcText.right * 1440/MemDC.GetDeviceCaps (LOGPIXELSX);
//				fr.rc.bottom = fr.rcPage.bottom = rcText.bottom * 1440/MemDC.GetDeviceCaps (LOGPIXELSY);
//				m_Text.SendMessage(EM_FORMATRANGE, TRUE, (LPARAM)&fr);
//				m_Text.SendMessage(EM_DISPLAYBAND, 0, (LPARAM)&rcText);


				// Copy the offscreen bitmap onto the screen.
				dc.BitBlt ( 0, 0, rcDrawOffset.Width (), rcDrawOffset.Height (),
                    &MemDC, rcDrawOffset.left, rcDrawOffset.top, SRCCOPY );
				
				// Swap back the original bitmap.
				MemDC.SelectObject ( pOldBitmap );


			}
			else
				UpdateParentRect (dc, rcUpdate);

		}
	}
}

void CInfoPanel::UpdateParentRect(HDC hdc, LPCRECT pRect)
{
	CWnd *pParentWnd = GetParent();
	
	if (pParentWnd && (m_bUpdateParent == false) )
	{
		HBRUSH hbr = (HBRUSH)pParentWnd->SendMessage (WM_CTLCOLORSTATIC, (WPARAM)hdc, (LPARAM)GetSafeHwnd());

		
		if ( hbr && hbr != (HBRUSH)::GetStockObject(HOLLOW_BRUSH) )
			::FillRect (hdc, pRect, hbr);
		else
		{
			CRect rc = pRect;
			ClientToScreen  (rc);
			pParentWnd->ScreenToClient  (rc);
			pParentWnd->InvalidateRect  (rc, TRUE);
			
			m_bUpdateParent = true;
			pParentWnd->UpdateWindow  ();
			m_bUpdateParent = false;
		}
	}
}

void CInfoPanel::OnSysColorChange()
{
	if (!m_bCustomColor)
		m_bkColor = ::GetSysColor((m_nInfoPanelFlag & INFO_PANEL_FLAG_YELLOW)?COLOR_INFOBK:COLOR_3DFACE);

	m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
}

void CInfoPanel::SysColorChange(COLORREF bkColor)
{
	SetBackColor  (bkColor);
	m_Ico.LoadImage  ( m_hResInst, m_PngRes, m_bkColor);
}

void CInfoPanel::SetBackColor(COLORREF bkColor)
{
	m_bkColor = bkColor;
	m_bCustomColor = true;
}

void CInfoPanel::DrawCtrl(CDC* pDC)
{
	if ( ::IsWindow  (m_Ico.GetSafeHwnd  ()) )
	{
		m_Ico.DrawCtrl (pDC);
	}

}

