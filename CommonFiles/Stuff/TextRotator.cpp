////////////////////////////////////////////////////////////////////
//
//  TextRotator.cpp
//  TextRptator class implementation
//  AVP general purposess stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "TextRotator.h"

#define BEVELLINE_SPACE   6

// ---
struct CTRect : public RECT {
	CTRect() { memset(this, 0, sizeof(RECT)); }
	CTRect( RECT &rect ) { memcpy(this, &rect, sizeof(RECT)); }
	LONG Width() { return right - left; }
	LONG Height() { return bottom - top; }
};

/////////////////////////////////////////////////////////////////////////////
// class CTextRotator
// ---
CTextRotator::CTextRotator( HDC hSourceDC, LPCTSTR lpszText, UINT uiHorzAlignment, UINT uiVertAlignment ) {
  m_hSourceDC       = hSourceDC;
  m_hRotatedMemDC   = NULL;
  m_uiHorzAlignment = uiHorzAlignment;
  m_uiVertAlignment = uiVertAlignment;
  m_bDisabled       = FALSE;
  m_bDrawBevelLine  = FALSE;
  m_uiAngle         = 0;
  m_strText         = lpszText;
  m_clrBackground   = ::GetSysColor(COLOR_3DFACE);
  m_clrText         = ::GetSysColor(COLOR_WINDOWTEXT);
	m_hSourceBmp      = NULL;
	m_hFont						= NULL;

	HFONT hCurrFont = (HFONT)::GetCurrentObject( m_hSourceDC, OBJ_FONT );
	if ( hCurrFont ) {
		LOGFONT logFont;
		::GetObject( hCurrFont, sizeof(logFont), &logFont );
		m_hFont = ::CreateFontIndirect( &logFont );
	}

}

// ---
CTextRotator::~CTextRotator() {
	if ( m_hFont )
		::DeleteObject( m_hFont );
	
  Clear();
}

// ---
void CTextRotator::Clear() {
	if ( m_hSourceBmp )
		::DeleteObject( ::SelectObject(m_hRotatedMemDC, m_hSourceBmp) ); // Delete temporary empty bitmap 

  ::DeleteDC( m_hRotatedMemDC );

  m_hRotatedMemDC = NULL;

  memset( &m_rectRotated, 0, sizeof(m_rectRotated) );

  m_uiAngle = 0;
}

// ---
void CTextRotator::SetFont( HFONT hFont ) {
	if ( hFont ) {
		if ( m_hFont )
			::DeleteObject( m_hFont );

		HGDIOBJ hOldFont = ::SelectObject( m_hSourceDC, hFont ); 

		LOGFONT logFont;
		::GetObject( hFont, sizeof(logFont), &logFont );
		m_hFont = ::CreateFontIndirect( &logFont );

		::SelectObject( m_hSourceDC, hOldFont ); 
	}
}


// ---
SIZE CTextRotator::GetLengthlyTextSize( HDC hDC, CPArray<TCHAR> *parrayText ) {
  SIZE cMaxSize;

	cMaxSize.cx = 0;
	cMaxSize.cy = 0;

  for (int i=0,c=parrayText->Count(); i<c; i++ ) {
    TCHAR *str = (*parrayText)[i];
    SIZE    stSize;

    if (!::GetTextExtentPoint32(hDC, str, lstrlen(str), &stSize) ) 
			continue;

    if ( cMaxSize.cx < stSize.cx ) 
			cMaxSize.cx = stSize.cx;
    if ( cMaxSize.cy < stSize.cy ) 
			cMaxSize.cy = stSize.cy;
  }

  return cMaxSize;
}

// ---
BOOL CTextRotator::BitBltText( int X, int Y ) {
  if ( !m_hRotatedMemDC ) 
		return FALSE;

	CTRect rcRect( m_rectRotated );
  return ::BitBlt(m_hSourceDC,
									X,
									Y,
									rcRect.Width(),
									rcRect.Height(),
									m_hRotatedMemDC,
									0,
									0,
									SRCCOPY);
}

// ---
BOOL CTextRotator::BitBltText( RECT rSource ) {
  if ( !m_hRotatedMemDC ) 
		return FALSE;

  CTRect rectRotatedSave( m_rectRotated );
	CTRect rectSource( rSource );
	CTRect rectRotated( m_rectRotated );

  // Change rotated rectangle size to fit in source rectangle
  if ( rectSource.Width() < rectRotated.Width() ) 
		rectRotated.right = m_rectRotated.right = rectSource.Width();

  if ( rectSource.Height() < rectRotated.Height() ) 
		rectRotated.bottom = m_rectRotated.bottom = rectSource.Height();

  // Calculate left/top position to draw the text block
  int X = 0;
  int Y = 0;

	switch ( m_uiHorzAlignment ) {
		case HORZ_CENTER :
			X = (rectSource.Width() - rectRotated.Width()) / 2;
			break;
		case HORZ_RIGHT :
      X = rectSource.Width() - rectRotated.Width();
			break;
  }

  switch ( m_uiVertAlignment ) {
		case VERT_CENTER : // CENTER
			Y = (rectSource.Height() - rectRotated.Height()) / 2;
			break;
		case VERT_BOTTOM : // BOTTOM
      Y = rectSource.Height() - rectRotated.Height();
			break;
  }

	BOOL bRet = BitBltText( X, Y ); // Draw Text

  if (m_bDrawBevelLine) { // DRAW BEVEL LINES (If Wanted)
    RECT stRect;

    if (m_uiAngle == 0 || m_uiAngle == 180) {
			switch ( m_uiVertAlignment ) {
				case VERT_TOP : // TOP
					stRect.top = stRect.bottom = rectRotated.Height() / 2;
					break;
				case VERT_CENTER : // CENTER
          stRect.top = stRect.bottom = rectSource.Height() / 2;
					break;
				case VERT_BOTTOM : // BOTTOM
					stRect.top = stRect.bottom = rectSource.Height() - rectRotated.Height() / 2;
					break;
      }

			switch ( m_uiHorzAlignment ) {
				case HORZ_LEFT :
					stRect.left  = rectRotated.Width() + BEVELLINE_SPACE;
					stRect.right = rectSource.Width() - BEVELLINE_SPACE;

					::DrawEdge( m_hSourceDC, &stRect, EDGE_ETCHED, BF_TOP );
					break;
				case HORZ_CENTER :
					stRect.left  = BEVELLINE_SPACE;
					stRect.right = rectSource.Width() / 2 - rectRotated.Width() / 2 - BEVELLINE_SPACE;

					::DrawEdge(m_hSourceDC, &stRect, EDGE_ETCHED, BF_TOP);

					stRect.left  = rectSource.Width() / 2 + rectRotated.Width() / 2 + BEVELLINE_SPACE;
					stRect.right = rectSource.Width() - BEVELLINE_SPACE;

					::DrawEdge( m_hSourceDC, &stRect, EDGE_ETCHED, BF_TOP );
					break;
				case HORZ_RIGHT :
					stRect.left  = BEVELLINE_SPACE;
					stRect.right = rectSource.Width() - rectRotated.Width() - BEVELLINE_SPACE;

					::DrawEdge( m_hSourceDC, &stRect, EDGE_ETCHED, BF_TOP );
					break;
      }
    }
    else {
			switch ( m_uiHorzAlignment ) {
				case HORZ_LEFT :
					stRect.left = stRect.right = rectRotated.Width() / 2;
					break;
				case HORZ_CENTER :
					stRect.left = stRect.right = rectSource.Width() / 2;
					break;
				case HORZ_RIGHT :
					stRect.left = stRect.right = rectSource.Width() - rectRotated.Width() / 2;
					break;
			}

			switch ( m_uiVertAlignment ) {
				case VERT_TOP :// TOP
					stRect.top    = rectRotated.Height() + BEVELLINE_SPACE;
					stRect.bottom = rectSource.Height() - BEVELLINE_SPACE;

					::DrawEdge( m_hSourceDC, &stRect, EDGE_ETCHED, BF_LEFT );
					break;
				case VERT_CENTER : // CENTER
					stRect.top    = BEVELLINE_SPACE;
					stRect.bottom = rectSource.Height() / 2 - rectRotated.Height() / 2 - BEVELLINE_SPACE;
        
					::DrawEdge(m_hSourceDC, &stRect, EDGE_ETCHED, BF_LEFT);

					stRect.top    = rectSource.Height() / 2 + rectRotated.Height() / 2 + BEVELLINE_SPACE;
					stRect.bottom = rectSource.Height() - BEVELLINE_SPACE;

					::DrawEdge(m_hSourceDC, &stRect, EDGE_ETCHED, BF_LEFT);
					break;
				case VERT_BOTTOM : // BOTTOM
					stRect.top    = BEVELLINE_SPACE;
					stRect.bottom = rectSource.Height() - rectRotated.Height() - BEVELLINE_SPACE;

					::DrawEdge(m_hSourceDC, &stRect, EDGE_ETCHED, BF_LEFT);
					break;
      }
    } 
  }

  m_rectRotated = rectRotatedSave;

  return bRet;
}

// ---
void CTextRotator::GetRotatedRect( RECT *pstRect ) {
  RECT rect = m_rectRotated;

  if ( pstRect ) 
		memcpy(pstRect, &rect, sizeof(RECT));
}

// ---
void CTextRotator::RotateBitmap( UINT degree, COLORREF clrBack, int Height, int Width, HDC sourceDC, HDC destDC ) {
	double Pi = 3.14159265359; 
	float radians = (float)(2 * Pi * degree) / 360;
	
	double value = cos(radians);
	if ( fabs(value) < FLT_EPSILON )
		value = 0.0;

	float cosine = (float)value;

	value = sin(radians);
	if ( fabs(value) < FLT_EPSILON )
		value = 0.0;

	float sine = (float)value;	
	
	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(-Height * sine);	
	int y1 = (int)(Height * cosine);
	int x2 = (int)(Width * cosine - Height * sine);
	int y2 = (int)(Height * cosine + Width * sine);
	int x3 = (int)(Width * cosine);	
	int y3 = (int)(Width * sine);
	int minx = min(0,min(x1, min(x2,x3)));	
	int miny = min(0,min(y1, min(y2,y3)));
	int maxx = max(0,max(x1, max(x2,x3)));	
	int maxy = max(0,max(y1, max(y2,y3)));
//	int maxx = max(x1, max(x2,x3));	
//	int maxy = max(y1, max(y2,y3));
	int w = maxx - minx;	
	int h = maxy - miny;	

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = ::CreateSolidBrush( clrBack );
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC, hbrBack );
	::PatBlt( destDC, 0, 0, w, h, PATCOPY );
	::DeleteObject( ::SelectObject( destDC, hbrOld ) );

	// Set mapping mode so that +ve y axis is upwords
	::SetMapMode( sourceDC, MM_ISOTROPIC );	
	::SetWindowExtEx(sourceDC,1,1,NULL);
	::SetViewportExtEx(sourceDC,1,1,NULL);	
	::SetViewportOrgEx(sourceDC,0, 0,NULL);
	
	::SetMapMode(destDC, MM_ISOTROPIC);	
	::SetWindowExtEx(destDC, 1,1,NULL);
	::SetViewportExtEx(destDC, 1,1,NULL);	
	::SetWindowOrgEx(destDC, minx, miny,NULL);

	// Now do the actual rotating - a pixel at a time
	// Computing the destination point for each source point
	// will leave a few pixels that do not get covered
	// So we use a reverse transform - e.i. compute the source point
	// for each destination point	
	for( int y = miny; y < maxy; y++ )	{
		for( int x = minx; x < maxx; x++ )		{
			int sourcex = (int)(x*cosine + y*sine);
			int sourcey = (int)(y*cosine - x*sine);
			if( sourcex >= 0 && sourcex < Width && sourcey >= 0 && sourcey < Height )
					::SetPixel(destDC,x,y,::GetPixel(sourceDC,sourcex,sourcey));
		}
	}	

	::SetViewportOrgEx( destDC, minx, miny, NULL );
}

// ---
static UINT NormalizeAngle( UINT uiAngle ) {
	uiAngle = abs(int(uiAngle));
	uiAngle %= 360;
	if ( abs(int(uiAngle) - 0) < 45 )
		uiAngle = 0;
	if ( abs(int(uiAngle) - 90) < 45 )
		uiAngle = 90;
	if ( abs(int(uiAngle) - 180) < 45 )
		uiAngle = 180;
	if ( abs(int(uiAngle) - 270) < 45 )
		uiAngle = 270;

	return uiAngle;
}

// ---
BOOL CTextRotator::RotateText( UINT uiAngle ) {
  Clear(); // Reset all preview memory DC

  m_uiAngle = ::NormalizeAngle( uiAngle );

  // Split text into a list of text lines (for multiline support)
  int             iLineCounter = 0;
  CPArray<TCHAR>  arrayText( 0, 1, false );

  TCHAR *psz = _tcstok(m_strText, _T("\n"));

  while (psz) {
    arrayText.Add( psz );
    iLineCounter++;
    psz = _tcstok(NULL, _T("\n"));
  }

  HDC hMemDC = ::CreateCompatibleDC( m_hSourceDC );

  if ( !hMemDC ) {
    return FALSE;
  }

  HGDIOBJ hOldFont		= ::SelectObject( hMemDC, m_hFont ); 

  // Calculate the size of the lenghtly text of the text list
  SIZE sizeText = GetLengthlyTextSize( hMemDC, &arrayText );

  // Set Full rectangle size
  CTRect rectFull;

  rectFull.left   = 0;
  rectFull.top    = 0;
  rectFull.right  = sizeText.cx;
  rectFull.bottom = sizeText.cy * iLineCounter;

  // Create an empty memory DC for rotation operation
  int iMax = rectFull.Width();

  if (iMax < rectFull.Height() ) 
		iMax = rectFull.Height();

  HBITMAP hEmptyBitmap1 = ::CreateCompatibleBitmap( m_hSourceDC, iMax, iMax );

  if ( !hEmptyBitmap1 ) 
		return FALSE;

  HBITMAP hEmptyBitmap2 = ::CreateCompatibleBitmap( m_hSourceDC, iMax, iMax );

  if (!hEmptyBitmap2) {
    ::DeleteObject( hEmptyBitmap1 );
		::SelectObject( hMemDC, hOldFont );
    return FALSE;
  }

  m_hRotatedMemDC = ::CreateCompatibleDC( m_hSourceDC );

  if ( !m_hRotatedMemDC ) {
		::SelectObject( hMemDC, hOldFont );
    ::DeleteObject( m_hRotatedMemDC );
    ::DeleteDC( hMemDC );
    ::DeleteObject( hEmptyBitmap2 );
    ::DeleteObject( hEmptyBitmap1 );
    return FALSE;
  }

  HGDIOBJ hOldBitmap1 = ::SelectObject( hMemDC, hEmptyBitmap1 ); // Put the empty bitmap in memory DC

	if ( m_hSourceBmp )
		::DeleteObject( ::SelectObject(m_hRotatedMemDC, m_hSourceBmp) ); // Delete temporary empty bitmap 2
  m_hSourceBmp = ::SelectObject( m_hRotatedMemDC, hEmptyBitmap2 ); // Put the empty bitmap in rotated memory DC

  ::SetBkMode( hMemDC, TRANSPARENT );

  HBRUSH hBrush = ::CreateSolidBrush( m_clrBackground );

  HGDIOBJ hOldBrush = ::SelectObject( hMemDC, hBrush );
  
  ::FillRect( hMemDC, &rectFull, hBrush );

  ::FillRect( m_hRotatedMemDC, &rectFull, hBrush );

  ::SetTextColor(hMemDC, m_clrText);

  // Draw the list of text into memory DC
  for (int i=0,c=arrayText.Count(); i<c; i++) {
    RECT stRect;

    stRect.left   = 0;
    stRect.top    = i * sizeText.cy;
    stRect.right  = stRect.left + sizeText.cx;
    stRect.bottom = stRect.top  + sizeText.cy;

    if ( m_bDisabled ) {
      ::SetTextColor(hMemDC, GetSysColor(COLOR_3DHIGHLIGHT));

      RECT rect = stRect;

      rect.left   += 1;
      rect.top    += 1;
      rect.right  += 1;
      rect.bottom += 1;

      ::DrawText(hMemDC, arrayText[i], -1, &rect, DT_SINGLELINE | DT_VCENTER | m_uiHorzAlignment);

      ::SetTextColor(hMemDC, GetSysColor(COLOR_3DSHADOW));
     }

    ::DrawText(hMemDC, arrayText[i], -1, &stRect, DT_SINGLELINE | DT_VCENTER | m_uiHorzAlignment);
  }

	RotateBitmap( uiAngle, m_clrBackground, rectFull.Height(), rectFull.Width(), hMemDC, m_hRotatedMemDC );

  ::DeleteObject( ::SelectObject(hMemDC, hOldBitmap1) ); // Delete temporary empty bitmap 1
  ::DeleteObject( ::SelectObject(hMemDC, hOldBrush) );
  ::SelectObject( hMemDC, hOldFont );
  ::DeleteDC( hMemDC );

  if (uiAngle == 90 || uiAngle == 270) {
    m_rectRotated.left   = 0;
    m_rectRotated.top    = 0;
    m_rectRotated.right  = rectFull.Height();
    m_rectRotated.bottom = rectFull.Width();
  }
  else {
    m_rectRotated.left   = 0;
    m_rectRotated.top    = 0;
    m_rectRotated.right  = rectFull.Width();
    m_rectRotated.bottom = rectFull.Height();
  }

  return TRUE;
}


