// HyperStatic.cpp : implementation file
//
#include "stdafx.h"
#include "HyperStatic.h"
#include "ResourceHelper.h"
#include "resource.h"
#include "DragWnd.h"
#include "util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHyperStatic

CHyperStatic::CHyperStatic(DWORD dwID /*=0*/, DWORD dwFlags /*=0*/)
{
	m_dwMessageID = dwID;
	m_dwFlags = dwFlags;
	m_bUnderLine = TRUE;
	m_bOverControl = FALSE;
	
	m_crLinkColour      = RGB(  0,   0, 255);   // Blue
	m_crOverColour     = RGB(  0,   0, 255);//::GetSysColor(COLOR_HIGHLIGHT);

	
	m_hLinkCursor = ::LoadCursor ( AfxFindResourceHandle(MAKEINTRESOURCE ( IDC_KAP_LINKHAND ), RT_GROUP_CURSOR), MAKEINTRESOURCE ( IDC_KAP_LINKHAND ) );

	m_NormalCursor = ::LoadCursor ( NULL, MAKEINTRESOURCE ( IDC_ARROW ) );
}

CHyperStatic::~CHyperStatic()
{
	m_Font.DeleteObject ();
}


BEGIN_MESSAGE_MAP(CHyperStatic, CStatic)
//{{AFX_MSG_MAP(CHyperStatic)
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperStatic message handlers
HBRUSH CHyperStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	ASSERT(nCtlColor == CTLCOLOR_STATIC);

	if(m_dwFlags & HYPER_STATIC_OPAQUE)
	{
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH hbr = (HBRUSH)Default();
		pDC->SetTextColor(m_crLinkColour);
		return (HBRUSH)hbr;
	}
	else
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_crLinkColour);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
}

BOOL CHyperStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hLinkCursor && m_NormalCursor)
	{
		if ( HitTestLink() )
			::SetCursor(m_hLinkCursor);
		else
			::SetCursor(m_NormalCursor);
		
		return TRUE;
	}
	
	return FALSE;
}

void CHyperStatic::GetLinkRect (CRect & rcLink)
{
	CRect rcWndRect;
	GetWindowRect (rcWndRect);

	CRect rc;
	rc.SetRectEmpty ();
	GetNeedSize (rc);
	
	if ( GetStyle() & SS_RIGHT )
		rcWndRect.left = rcWndRect.right - rc.Width  ();
	else if (GetStyle() & SS_CENTER)
	{
		rcWndRect.left = rcWndRect.left + (rcWndRect.Width () - rc.Width  ())/2;
		rcWndRect.right = rcWndRect.right - (rcWndRect.Width () - rc.Width  ())/2;
	}
	else
		rcWndRect.right = rcWndRect.left + rc.Width  ();
	
	rcWndRect.bottom = rcWndRect.top + rc.Height  ();

	rcLink = rcWndRect;
}

bool CHyperStatic::HitTestLink()
{
	CRect rc;
	rc.SetRectEmpty ();
	GetLinkRect (rc);
	return (rc.PtInRect (CPoint(::GetMessagePos ())) == TRUE);
}

void CHyperStatic::GetNeedSize ( LPRECT prectMax )
{
	CClientDC dc (this);
	
	SetCtrlFont(&dc);
	
	CFont* pOldFont = dc.SelectObject(&m_Font);
	CString szText;
	GetWindowText( szText );
	
	dc.DrawText  ( szText, prectMax, DT_CALCRECT );
	
	dc.SelectObject(pOldFont);
}

void CHyperStatic::SetCtrlFont(CDC *pdc)
{
	if(m_Font.GetSafeHandle())
		return;

	CFont *pFont = GetFont();
	if(pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfUnderline = TRUE;
		if(m_Font.CreateFontIndirect(&lf))
			SetFont(&m_Font);
	}
	else if(CreateKAVFont(this, m_Font, pdc, true))
	{
		SetFont(&m_Font);
	}
}

void CHyperStatic::OnPaint() 
{
	SetCtrlFont();
	Default();
	return;
}

BOOL CHyperStatic::SendParentNotifyClick (BOOL bForceSend)
{
	CWnd * pParent = GetParent ();

	if  ( pParent && (bForceSend || HitTestLink ()) )
	{
		NMHDR nmhdr;
    
		nmhdr.hwndFrom = GetSafeHwnd ();
		nmhdr.idFrom = m_dwMessageID;
		nmhdr.code = EN_LINK;
    
		pParent -> SendMessage ( WM_NOTIFY, ( WPARAM ) GetWindowLong ( GetSafeHwnd (), GWL_ID ), ( LPARAM ) &nmhdr );

		return TRUE;
	}

	return FALSE;
}

void CHyperStatic::Initialize()
{
    // We want to get mouse messages and clicks via STN_CLICKED
    ::SetWindowLong ( GetSafeHwnd (), GWL_STYLE, GetStyle () | SS_NOTIFY );
}

int CHyperStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	Initialize();
	
	return 0;
}

void CHyperStatic::PreSubclassWindow() 
{
	Initialize();

    CStatic::PreSubclassWindow ();
}


void CHyperStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( !HitTestLink () )
		LButtonDragWnd(GetSafeHwnd  (), POINT_TO_LPARAM(point));

	CStatic::OnLButtonDown(nFlags, point);
}


void CHyperStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (SendParentNotifyClick ())
		return;
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CHyperStatic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_RETURN || nChar == VK_SPACE) && SendParentNotifyClick (TRUE))
		return;
	
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CHyperStatic::OnGetDlgCode() 
{
	return DLGC_BUTTON;
}

LRESULT CHyperStatic::OnPrintClient (WPARAM wParam, LPARAM lParam)
{
	SetCtrlFont();
	Default();
	return 0;
}