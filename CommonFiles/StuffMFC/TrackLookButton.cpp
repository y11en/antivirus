// CTrackLookButton.cpp : implementation file
//

#include "stdafx.h"
#include "TrackLookButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RGB_BUTTON_BLACK    (GetSysColor(COLOR_WINDOWFRAME))
#define RGB_BUTTON_WHITE    (GetSysColor(COLOR_BTNHIGHLIGHT))
#define RGB_BUTTON_LIGHT    (GetSysColor(COLOR_BTNFACE))
#define RGB_BUTTON_DARK     (GetSysColor(COLOR_BTNSHADOW))

#define BORDER_CLEAR		0x0000L
#define BORDER_PUSHED		0x0001L
#define BORDER_NONPUSHED	0x0002L

static CTrackLookButton *gpCaptured = NULL;
static CTrackLookButton *gpFocused  = NULL;

/////////////////////////////////////////////////////////////////////////////
// CTrackLookButton
// ---
CTrackLookButton::CTrackLookButton() {
	m_bMouseCaptured = FALSE;
	m_bLButtonDown = FALSE;
	m_bLButtonWasDown = FALSE;
	m_bHasFocus = FALSE;
	m_bDisabled = FALSE;

	m_TextAlign = AlignRight;
	m_nBorder = BORDER_CLEAR;
	m_bRaised = FALSE;

	m_bBorderAlways = FALSE;
	m_bFocusAsRect = TRUE;
}

// ---
CTrackLookButton::~CTrackLookButton() {
}


// ---
void CTrackLookButton::SetBorderAlways( BOOL bSet ) {
	m_bBorderAlways = bSet;
}


// ---
void CTrackLookButton::SetFocusAsRect( BOOL bSet ) {
	m_bFocusAsRect = bSet;
}


// ---
void CTrackLookButton::Redraw() {
	Invalidate();
//	UpdateWindow();
}

BEGIN_MESSAGE_MAP(CTrackLookButton, CButton)
	//{{AFX_MSG_MAP(CTrackLookButton)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrackLookButton message handlers
// ---
void CTrackLookButton::OnMouseMove( UINT nFlags, CPoint point ) {
	CButton::OnMouseMove(nFlags,point);
	if ( GetCapture() == NULL || GetCapture() == this ) {
		if ( !m_bMouseCaptured || GetCapture() != this || m_nBorder == BORDER_CLEAR )	{
			if ( gpFocused ) {
				gpFocused->OnMouseLeave( 0, CPoint(-1, -1) );
				gpFocused = NULL;
			}
			SetCapture();
			m_bMouseCaptured = TRUE;
			OnMouseEnter( nFlags,point );
			gpCaptured = this;
		} 
		else {
			CRect rc;
			this->GetClientRect( &rc );
			if ( !rc.PtInRect(point) ) {
				OnMouseLeave( nFlags, point );
				if ( !(nFlags & MK_LBUTTON) ) {
					m_bMouseCaptured = FALSE;
					ReleaseCapture();
					gpCaptured = NULL;
				}
			}
			else
				if ( m_bLButtonWasDown )
					OnMouseEnter( nFlags,point );
		}
	}
}

// ---
void CTrackLookButton::OnKillFocus(CWnd* pNewWnd) {
	m_nBorder = BORDER_CLEAR;
	m_bHasFocus = FALSE;
	m_bRaised = FALSE;
	gpFocused = NULL;
	CButton::OnKillFocus( pNewWnd );
	Redraw();
}

// ---
void CTrackLookButton::OnSetFocus( CWnd* pOldWnd ) {
	if ( gpCaptured ) {
		gpCaptured->OnMouseLeave( 0, CPoint(-1, -1) );
		gpCaptured = NULL;
	}
	gpFocused = this;
	m_nBorder = m_bLButtonDown ? BORDER_PUSHED : BORDER_NONPUSHED;
	m_bHasFocus = TRUE;
	m_bRaised = TRUE;
	CButton::OnSetFocus( pOldWnd );
	Redraw();
}

// ---
void CTrackLookButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) {
	int	 iSaveDC;
	CDC* pDC;
	CBrush brush(RGB_BUTTON_LIGHT);
	CRect rc;
	CString	strTitle;
	CString	strOutTitle;
	UINT	nFormat; //For DrawText
	UINT	nFlags;	//For DrawBitmap
	CBitmap*	pBitmap=NULL;
	
	pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	VERIFY(pDC);
	rc.CopyRect(&lpDrawItemStruct->rcItem);

	GetWindowText(strTitle);
	strOutTitle = strTitle;
	strTitle.Remove( _T('&') );

	nFormat = DT_SINGLELINE | DT_WORDBREAK;
	iSaveDC = pDC->SaveDC();
	
	switch ( m_TextAlign ) {
		case AlignAbove: 
			nFormat |= DT_CENTER|DT_TOP; 
			break;	
		case AlignBelow: 
			nFormat |= DT_CENTER|DT_BOTTOM;
			break;
		case AlignLeft:
		case AlignRight: 
			nFormat |= DT_LEFT|DT_VCENTER;
			break;
		default: 
			ASSERT(FALSE); // should not be called;
	}
	
	
	pDC->SetBkMode( TRANSPARENT );
	
	Draw3DBorder(pDC,rc,m_nBorder);
	CRect rcBorder(rc);
	
	if ( m_bHasFocus ) {
		if ( m_bFocusAsRect ) 
			pDC->SetTextColor(RGB_BUTTON_BLACK);
		else
			pDC->SetTextColor(RGB(0,0,255));
		pBitmap = &m_bitmapFocus;
		if ( pBitmap->m_hObject == NULL ) 
			pBitmap = &m_bitmap; //Simulate some bitmap;
	}
	else {
		pDC->SetTextColor(RGB_BUTTON_BLACK);
		pBitmap = &m_bitmap;
	}
	
	rc.InflateRect(-5,-3); //We assert we have a 5 points offset from border

	if ( m_bRaised ) 	{
		pBitmap = &m_bitmapFocus;
		if ( pBitmap->m_hObject == NULL ) 
			pBitmap=&m_bitmap; //Simulate some bitmap;
	} 
	
	if ( m_bDisabled = (::GetWindowLong(m_hWnd,GWL_STYLE) & WS_DISABLED) ) {
		pBitmap=&m_bitmapDisabled;
	}
	else 
		if (m_bLButtonDown) 
			rc.OffsetRect(1,1);
	
	CRect rcText(rc);

	if (pBitmap->m_hObject)	{
		CRect rcBitmap(rc);
		BITMAP	bmpInfo;			
		CSize	size;

		switch ( m_TextAlign ) {
			case AlignLeft:
				size = pDC->GetTextExtent(strTitle);
				rcBitmap.OffsetRect(size.cx+5,0);
				nFlags = DB_VCENTER;
				break;
			case AlignAbove:
				size=pDC->GetTextExtent(strTitle);
				rcBitmap.OffsetRect(0,size.cy+5);
				nFlags=DB_HCENTER;
				break;
			case AlignRight:
				pBitmap->GetBitmap(&bmpInfo);	
				rcText.OffsetRect(bmpInfo.bmWidth + 7,0);
				nFlags=DB_VCENTER;
				break;
			case AlignBelow:
				nFlags=DB_HCENTER;
				break;
			default: 
				ASSERT(FALSE);
				break;
		}

		DrawBitmap(pDC,rcBitmap,nFlags,pBitmap);
	}
	
	pDC->SetBkColor(RGB_BUTTON_LIGHT);
	rcText.right = rc.right - 2;
	::FillRect( *pDC, rcText, (HBRUSH)(COLOR_BTNFACE + 1) );

	CSize rcSize  = pDC->GetTabbedTextExtent(strTitle, _tcsclen(strTitle), 1, NULL );
	if ( rcSize.cx > rcText.Width() )
		nFormat &= ~DT_SINGLELINE;

	if ( m_bDisabled ) {
		rcText.OffsetRect(1,1);
		pDC->SetTextColor(RGB_BUTTON_WHITE);
		pDC->DrawText(strTitle,rcText,nFormat);
		rcText.OffsetRect(-1,-1);
		pDC->SetTextColor(RGB_BUTTON_DARK);
		pDC->DrawText(strOutTitle,rcText,nFormat);
	} 
	else
		pDC->DrawText( strOutTitle, rcText, nFormat );
	
	if ( m_bHasFocus && m_bFocusAsRect ) {
/*
		CRect rcFormat = rcText;
		pDC->DrawText( strTitle, rcFormat, nFormat | DT_CALCRECT );
		CSize rcSize( rcFormat.Width(), rcFormat.Height() );

		rcText.top   += (rcText.Height() - rcSize.cy) / 2;
		rcText.bottom = rcText.top + rcSize.cy;
		rcText.right  = rcText.left + rcSize.cx;
		rcText.InflateRect( 1, 2 );
		pDC->DrawFocusRect( rcText );
*/
		pDC->DrawText( strOutTitle, rcText, nFormat );

		rcBorder.InflateRect( -2, -2 );
		pDC->DrawFocusRect( rcBorder );
	}

	pDC->RestoreDC(iSaveDC);
}

// ---
void	CTrackLookButton::OnMouseEnter( UINT nFlags,CPoint point ) {
	m_bLButtonDown = m_bLButtonWasDown && (nFlags & MK_LBUTTON);
	m_nBorder = m_bLButtonWasDown ? BORDER_PUSHED : BORDER_NONPUSHED;
	//if (m_bLButtonDown)
	//{
	m_bRaised=TRUE;
	Redraw();
}

// ---
void	CTrackLookButton::OnMouseLeave( UINT nFlags,CPoint point ) {
	m_nBorder = m_bLButtonWasDown ? BORDER_NONPUSHED : BORDER_CLEAR;
	m_bRaised = m_bLButtonWasDown ? TRUE : FALSE;
	m_bLButtonDown = FALSE;
	Redraw();
}

// ---
void CTrackLookButton::Draw3DBorder(CDC* pDC,CRect rc,UINT nOptions) {
	switch (nOptions)	{
		case BORDER_CLEAR:
			if ( m_bBorderAlways )
				pDC->Draw3dRect(rc,RGB_BUTTON_WHITE,RGB_BUTTON_DARK);
			else
				pDC->Draw3dRect(rc,RGB_BUTTON_LIGHT,RGB_BUTTON_LIGHT);
			break;
		case BORDER_PUSHED:
			pDC->Draw3dRect(rc,RGB_BUTTON_DARK,RGB_BUTTON_WHITE);
			break;
		case BORDER_NONPUSHED:
			pDC->Draw3dRect(rc,RGB_BUTTON_WHITE,RGB_BUTTON_DARK);
			break;
		default:
			break;
	}
}

void CTrackLookButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLButtonDown=TRUE;
	if (GetFocus()!=this)
	{
		this->SetFocus();
//		return;
	}
	m_bLButtonDown=TRUE;
	m_bLButtonWasDown=TRUE;
	m_bRaised=FALSE;
	m_nBorder=BORDER_PUSHED;

	CButton::OnLButtonDown(nFlags,point);
	Redraw();
}


void CTrackLookButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_bLButtonWasDown ) {
		m_bLButtonDown=FALSE;
		m_bLButtonWasDown=FALSE;
		m_bRaised=TRUE;
		if (GetFocus()!=this)
		{
			
			this->SetFocus();
		
		} else
		{
			m_nBorder=BORDER_NONPUSHED;
			Redraw();
		}
		
		m_bMouseCaptured = FALSE;
		ReleaseCapture();
		gpCaptured = NULL;

		CRect rc;
		this->GetClientRect( &rc );
		if ( rc.PtInRect(point) ) {
			SendMessage( WM_MOUSEMOVE, 0, MAKELPARAM(-1, -1) );
			GetParent()->PostMessage(WM_COMMAND,
						MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED),
						(LPARAM) m_hWnd);
		}
	}
}

void CTrackLookButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_bLButtonDown=TRUE;
	if (GetFocus()!=this)
	{
		this->SetFocus();
		return;
	}
	m_nBorder=BORDER_PUSHED;

	Redraw();
	CButton::OnLButtonDblClk(nFlags,point);	
	m_nBorder=BORDER_CLEAR;
}

int CTrackLookButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_bDisabled=(lpCreateStruct->style & WS_DISABLED);
	return 0;
}

void CTrackLookButton::OnEnable(BOOL bEnable) 
{
	m_bDisabled=!bEnable;
	CButton::OnEnable(bEnable);
}

BOOL CTrackLookButton::LoadBitmaps(UINT nBitmap, UINT nBitmapFocus, UINT nBitmapDisabled)
{
	return LoadBitmaps(MAKEINTRESOURCE(nBitmap),
						MAKEINTRESOURCE(nBitmapFocus),
						MAKEINTRESOURCE(nBitmapDisabled));
}

CTrackLookButton::TextAlign CTrackLookButton::GetTextAlignment() const
{
	return m_TextAlign;
}

void CTrackLookButton::SetTextAlignment(TextAlign nTextAlign)
{
	m_TextAlign=nTextAlign;
}

void CTrackLookButton::DrawBitmap(CDC * pDC, CRect rc,UINT nFlags,CBitmap * pBitmap)
{
	//Centers a bitmap in a given rectangle
	//If necessary clips the bitmap if outfits the rc
	CDC memDC;
	CBitmap* pOld=NULL;
	memDC.CreateCompatibleDC(pDC);
	BITMAP	bmpInfo;
	int Width;
	int Height;
	int xSrc=0;
	int ySrc=0;
	int xDesired;
	int yDesired;

	ASSERT(pBitmap->m_hObject!=NULL);
	pBitmap->GetBitmap(&bmpInfo);
	
	pOld=memDC.SelectObject((CBitmap*) pBitmap);
	if (pOld==NULL) return; //Destructors will clean up
	
	Width=(bmpInfo.bmWidth-rc.Width())/2;
	Height=(bmpInfo.bmHeight-rc.Height())/2;
	
	if ((nFlags & DB_HCENTER))
	{
		if (Width>0) //If the bitmap Width is larger then rc
		{
			xDesired=rc.left;
			xSrc=abs(Width);
		}
		else xDesired=rc.left+ abs(Width);
	}
		else xDesired=rc.left;
	
	if ((nFlags & DB_VCENTER))
	{
		if (Height>0) //If the bitmap Height is larger then rc
		{
			yDesired=rc.top;
			ySrc=abs(Height);
		}
		else yDesired=rc.top+abs(Height);
	} else
		yDesired=rc.top;
	
	pDC->BitBlt(xDesired,yDesired,rc.Width(),rc.Height(),&memDC,xSrc,ySrc,SRCCOPY);
	
	memDC.SelectObject(pOld);	
}

static void DitherBlt( CBitmap &srcBmp, CBitmap &dstBmp );

BOOL CTrackLookButton::LoadBitmaps(LPCTSTR lpszBitmap, LPCTSTR lpszBitmapFocus, LPCTSTR lpszBitmapDisabled)
{
	//Delete old ones
	m_bitmap.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapDisabled.DeleteObject();
	m_bitmapRaised.DeleteObject();
	
	if (!m_bitmap.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to Load Normal bitmap of CButton\n");
		return FALSE;
	}

	if (lpszBitmapFocus!=NULL)
	{
		if (!m_bitmapFocus.LoadBitmap(lpszBitmapFocus))
		{
			TRACE0("Failed to Load Focus bitmap of CButton\n");
			return FALSE;
		}
	}
	
	if (lpszBitmapDisabled!=NULL)
	{
		if (!m_bitmapDisabled.LoadBitmap(lpszBitmapDisabled))
		{
			TRACE0("Failed to Load Disabled bitmap of CButton\n");
			return FALSE;
		}
	}
	else {
		CBitmap bmp;
		bmp.LoadBitmap(lpszBitmap);
		::DitherBlt(bmp, m_bitmapDisabled);
	}

	return TRUE;
}


// --- 
static void DitherBlt( CBitmap &srcBmp, CBitmap &dstBmp ) {
	BITMAP bmp;
	srcBmp.GetBitmap( &bmp );
	int nWidth = bmp.bmWidth;
	int nHeight = bmp.bmHeight;
	int nXDest = 0;
	int nYDest = 0;
	int nXSrc = 0;
	int nYSrc = 0;

	CDC mDC;
	mDC.CreateCompatibleDC(0);
  CBitmap mskBmp;
	mskBmp.CreateCompatibleBitmap( &mDC, nWidth, nHeight );
	CBitmap * pmDCOldBmp = mDC.SelectObject( &mskBmp );

	CDC dDC;
	dDC.CreateCompatibleDC( 0 );
	CWindowDC winDC( CWnd::FromHandle(::GetDesktopWindow()) );
	dstBmp.CreateCompatibleBitmap( &winDC, nWidth, nHeight );
	CBitmap * pdDCOldBmp = dDC.SelectObject( &dstBmp );

  CDC sDC;
	sDC.CreateCompatibleDC( 0 );
  CBitmap * psDCOldBmp = sDC.SelectObject( &srcBmp );
	
	mDC.PatBlt( 0, 0, nWidth, nHeight, WHITENESS );
	sDC.SetBkColor( GetSysColor(COLOR_BTNFACE) );
	mDC.BitBlt( 0, 0, nWidth, nHeight, &sDC, nXSrc, nYSrc, SRCCOPY );
	sDC.SetBkColor( GetSysColor(COLOR_BTNHILIGHT) );
	mDC.BitBlt( 0, 0, nWidth, nHeight, &sDC, nXSrc, nYSrc, SRCPAINT );

	sDC.FillSolidRect( 0,0, nWidth, nHeight, GetSysColor(COLOR_3DFACE) );

	sDC.SetBkColor( RGB(0, 0, 0) );
	sDC.SetTextColor(RGB(255, 255, 255));

	CBrush brShadow, brHilight;
	brHilight.CreateSolidBrush(GetSysColor(COLOR_BTNHILIGHT));
	brShadow.CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
	CBrush * pOldBrush = sDC.SelectObject(&brHilight);
	sDC.BitBlt(0,0, nWidth, nHeight, &mDC, 0, 0, 0x00E20746L);
	
	dDC.BitBlt(nXDest+1,nYDest+1,nWidth, nHeight, &sDC,0,0,SRCCOPY);
	
	sDC.BitBlt(1,1, nWidth, nHeight, &mDC, 0, 0, 0x00E20746L);
	sDC.SelectObject(&brShadow);
	sDC.BitBlt(0,0, nWidth, nHeight, &mDC, 0, 0, 0x00E20746L);

	dDC.BitBlt(nXDest,nYDest,nWidth, nHeight, &sDC,0,0,SRCCOPY);

	sDC.SelectObject( psDCOldBmp );
	sDC.DeleteDC();

	mDC.SelectObject( pmDCOldBmp );
	mDC.DeleteDC();

	dDC.SelectObject( pdDCOldBmp );
	dDC.DeleteDC();
}

/*
void DitherBlt( CDC *drawdc, int nXDest, int nYDest, int nWidth, 
                        int nHeight, CBitmap &bmp, int nXSrc, int nYSrc)
{
	// create a monochrome memory DC
	CDC ddc;
	ddc.CreateCompatibleDC(0);
  CBitmap bwbmp;
	bwbmp.CreateCompatibleBitmap(&ddc, nWidth, nHeight);
	CBitmap * pddcOldBmp = ddc.SelectObject(&bwbmp);

  CDC dc;
	dc.CreateCompatibleDC(0);
  CBitmap * pdcOldBmp = dc.SelectObject(&bmp);
	
	// build a mask
	ddc.PatBlt(0, 0, nWidth, nHeight, WHITENESS);
	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	ddc.BitBlt(0, 0, nWidth, nHeight, &dc, nXSrc,nYSrc, SRCCOPY);
	dc.SetBkColor(GetSysColor(COLOR_BTNHILIGHT));
	ddc.BitBlt(0, 0, nWidth, nHeight, &dc, nXSrc,nYSrc, SRCPAINT);

	// Copy the image from the toolbar into the memory DC
	// and draw it (grayed) back into the toolbar.
	dc.FillSolidRect(0,0, nWidth, nHeight, GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU));
        //SK: Looks better on the old shell
	dc.SetBkColor(RGB(0, 0, 0));
	dc.SetTextColor(RGB(255, 255, 255));
	CBrush brShadow, brHilight;
	brHilight.CreateSolidBrush(GetSysColor(COLOR_BTNHILIGHT));
	brShadow.CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
	CBrush * pOldBrush = dc.SelectObject(&brHilight);
	dc.BitBlt(0,0, nWidth, nHeight, &ddc, 0, 0, 0x00E20746L);
	drawdc->BitBlt(nXDest+1,nYDest+1,nWidth, nHeight, &dc,0,0,SRCCOPY);
	dc.BitBlt(1,1, nWidth, nHeight, &ddc, 0, 0, 0x00E20746L);
	dc.SelectObject(&brShadow);
	dc.BitBlt(0,0, nWidth, nHeight, &ddc, 0, 0, 0x00E20746L);

	drawdc->BitBlt(nXDest,nYDest,nWidth, nHeight, &dc,0,0,SRCCOPY);
	// reset DCs
	ddc.SelectObject(pddcOldBmp);
	ddc.DeleteDC();
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pdcOldBmp);
	dc.DeleteDC();

	bwbmp.DeleteObject();
}
*/
