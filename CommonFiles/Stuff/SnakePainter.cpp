////////////////////////////////////////////////////////////////////
//
//  SnakePainter.cpp
//  SnakePainter implementation
//  AVP generic purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <Stuff\Thread.h>
#include <Stuff\SnakePainter.h>

static UINT nStepSnap	= ::RegisterWindowMessage( _T("AvpStepSnap") );

// ---
class CSnakePainterThread : public CThread {
	CEventSemaphore  m_rcStopEvent;
	HWND             m_hWndHost;
	DWORD					   m_dwTimeOut;
public :
	CSnakePainterThread( HWND hWndHost, DWORD dwTimeOut );

	virtual void	Terminate();
  virtual int		Run();
};



// ---
int CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam,  LPARAM lParam ) {
	CSnakePainter *pPainter = (CSnakePainter *)::GetWindowLong( hWnd, GWL_USERDATA );
	if ( pPainter ) {
		if ( uMsg == nStepSnap ) {
			pPainter->StepIt();
			return 0;
		}
		switch (uMsg ) {
			case WM_PAINT :
				pPainter->DoPaint();
				return 0;
		}
		if ( pPainter->m_pOldWndProc )
			return (int)::CallWindowProc( pPainter->m_pOldWndProc, hWnd, uMsg, wParam, lParam );
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CSnakePainter

CSnakePainter::CSnakePainter() {
	m_bReverse = FALSE;
	m_bReversable = FALSE;
	m_bTimeForTail = FALSE;
	m_bGradient = TRUE;
	m_nMax = 100;
	m_clrBk = RGB(255,255,255);
	m_nSize= 100;
	m_clrEnd = RGB(0,255,255);
	m_clrStart = RGB(255,0,0);
	m_nStep = 1;
	m_nPos = 0;
	m_nTail = 0;
	m_pThread = NULL;
	m_dwTimeout = 10;
	m_hImage = NULL;
	m_hWndHost = NULL;
	m_pOldWndProc = NULL;

	::ZeroMemory( &m_WndRect, sizeof(m_WndRect) );
}

CSnakePainter::~CSnakePainter() {
	if ( m_hImage )
		::DeleteObject( m_hImage );
}

// ---
void CSnakePainter::SetReversable( BOOL bReversable ) {
	m_bReversable = bReversable;
}


// ---
void CSnakePainter::SetSize( int nSize ) {
	m_nSize = nSize;
}

// ---
void CSnakePainter::SetBkColor( COLORREF clrFG ) {
	m_clrBk = clrFG;
}

// ---
void CSnakePainter::SetGradientFill( COLORREF clrStart, COLORREF clrEnd ) {
	m_clrCurStart = m_clrStart = clrStart;
	m_clrCurEnd = m_clrEnd = clrEnd;
}

// ---
void CSnakePainter::SetStep( int nStep ) {
	m_nStep = nStep;
}

// ---
void CSnakePainter::SetMax( int nMax ) {
  m_nMax = nMax;
}

// ---
void CSnakePainter::SetPos( int nPos ) {
	m_nPos = nPos;
	m_nTail = (m_nSize >= 100 ? 0 : (m_nPos - m_nSize));
	DoPaint();
}


// ---
int CSnakePainter::GetPos() {
	return m_nPos;
}

// ---
int CSnakePainter::GetStep() {
	return m_nStep;
}

// ---
int CSnakePainter::GetSize() {
	return m_nSize;
}

// ---
COLORREF CSnakePainter::GetBkColor() {
	return m_clrBk;
}

// ---
COLORREF CSnakePainter::GetGradientStartColor() {
	return m_clrStart;
}

// ---
COLORREF CSnakePainter::GetGradientEndColor() {
  return m_clrEnd;
}

// ---
int CSnakePainter::GetTail() {
	return m_nTail;
}

// ---
BOOL CSnakePainter::IsReverse() {
  return m_bReverse;
}

// ---
void CSnakePainter::SetWindow( HWND hWnd ) {
	if ( m_hWndHost != hWnd ) {
		if ( m_hWndHost ) {
			::SetWindowLong( m_hWndHost, GWL_WNDPROC,	(DWORD)m_pOldWndProc );
			::SetWindowLong( m_hWndHost, GWL_USERDATA, (DWORD)NULL );
		}
		m_hWndHost = hWnd;
		if ( m_hWndHost ) {
			m_pOldWndProc = (WNDPROC)::SetWindowLong( hWnd, GWL_WNDPROC,	(DWORD)WindowProc );
			::SetWindowLong( hWnd, GWL_USERDATA, (DWORD)this );
			RECT rcWndRect;
			::GetClientRect( m_hWndHost, &rcWndRect );
			m_WndRect = rcWndRect;
		}
	}
}

// ---
void CSnakePainter::SetTimeout( DWORD dwTimeout ) {
	m_dwTimeout = dwTimeout;
}

// ---
void CSnakePainter::SetWndRect( RECT rcWndRect ) {
	m_WndRect = rcWndRect;
}


// ---
BOOL CSnakePainter::SetImage( HINSTANCE hResInstance, UINT uiImageID ) {
	if ( m_hImage )
		::DeleteObject( m_hImage );

	m_bGradient = FALSE;
	m_hImage = ::LoadBitmap( hResInstance, MAKEINTRESOURCE(uiImageID) );
	if ( m_hImage ) {
		BITMAP    bmInfo;
		::GetObject( m_hImage, sizeof(bmInfo), &bmInfo );
		m_nSize = bmInfo.bmWidth * 100 / (m_WndRect.right - m_WndRect.left);
		return TRUE;
	}

	return FALSE;
}

// ---
void CSnakePainter::SetupReverse() {
	m_nPos = m_nMax;
	if( !m_bTimeForTail ) 
		m_bTimeForTail = TRUE;
	else 
		if( m_nTail >= m_nMax ) {
			m_bTimeForTail = FALSE;
			m_nPos = m_nMax - m_nStep;
			m_nTail = m_nMax;
			m_bReverse = TRUE;
			m_clrCurStart = m_clrEnd;
			m_clrCurEnd = m_clrStart;
		}
		else 
			SafeStepTail();
}

// ---
void CSnakePainter::SetupDirect() {
	m_nPos = 0;
	if ( !m_bTimeForTail ) 
		m_bTimeForTail = TRUE;
	else 
		if ( m_nTail <= 0 ) {
			m_bTimeForTail = FALSE;
			m_nPos = m_nStep;
			m_nTail = 0;
			m_bReverse = FALSE;
			m_clrCurStart = m_clrStart;
			m_clrCurEnd = m_clrEnd;
		}
		else
		  SafeStepTail();
}

// ---
int CSnakePainter::StepIt() {
	if( !m_bReverse )	{
		if( m_nPos >= m_nMax ) {
			if ( m_bReversable || m_nTail < m_nMax )
				SetupReverse();
			else
				ResetProgress();
		}
		else {
			SafeStepHead();
			SafeStepTail();
		}
	}
	else {
		if( m_nPos <= 0 )	{
			SetupDirect();
		}
		else {
			SafeStepHead();
			SafeStepTail();
		}
	}

	DoPaint();
	/*
	::InvalidateRect( m_hWndHost, NULL, FALSE );
	::UpdateWindow( m_hWndHost );
	*/
	return m_nPos;
}

// ---
int CSnakePainter::SafeStepHead() {
	int nStep;
	if ( m_bReverse ) {
		nStep = m_nPos - m_nStep;
		if ( nStep > 0 ) {
			m_nPos -= m_nStep;
			nStep = m_nStep;
		}
		else {
			nStep = m_nPos;
			m_nPos = 0;
		}
	}
	else {
		nStep = m_nPos + m_nStep;
		if ( nStep < m_nMax ) {
			m_nPos += m_nStep;
			nStep = m_nStep;
		}
		else {
			nStep = m_nMax-m_nPos;
			m_nPos = m_nMax;
		}
	}
	return nStep;
}

// ---
int CSnakePainter::SafeStepTail() {
	if ( !m_bTimeForTail && ((m_bReverse && m_nMax-m_nPos<m_nSize) || (!m_bReverse && m_nPos<m_nSize)) )
		return 0;

	int nStep;
	if ( m_bReverse ) {
		nStep = m_nTail - m_nStep;
		if ( nStep>0 )	{
			m_nTail -= m_nStep;
			nStep = m_nStep;
		}
		else {
			nStep = m_nTail;
			m_nTail=0;
		}
	}
	else {
		nStep = m_nTail + m_nStep;
		if ( nStep < m_nMax ) {
			m_nTail += m_nStep;
			nStep = m_nStep;
		}
		else {
			nStep = m_nMax - m_nTail;
			m_nTail = m_nMax;
		}
	}
  return nStep;
}

// ---
void CSnakePainter::ResetProgress() {
	m_bTimeForTail = FALSE;
	m_bReverse = FALSE;
	m_nTail = m_nPos = 0;
}

// ---
int CSnakePainter::GetMax() {
  return m_nMax;
}

// ---
void CSnakePainter::DrawImage( HDC hWndDC, RECT rcRect ) {
	if ( m_hImage ) {
		HDC				hMemDC = ::CreateCompatibleDC( hWndDC );
		HBITMAP		hOldBmp = (HBITMAP)::SelectObject( hMemDC, m_hImage );
		BITMAP    bmInfo;

		int iStartPos = 0;
		::GetObject( m_hImage, sizeof(bmInfo), &bmInfo );
		if ( !m_bReverse ) {
			int iShift = bmInfo.bmWidth - (rcRect.right-rcRect.left);
			if ( !m_bTimeForTail ) {
				iStartPos = bmInfo.bmWidth - (rcRect.right-rcRect.left);
				if ( iStartPos < 0 ) 
					iStartPos = 0;
			}
			
			if ( iShift < 0 )
				rcRect.left += (-iShift);
		}
		else {
			if ( m_bTimeForTail ) {
				iStartPos = bmInfo.bmWidth - (rcRect.right-rcRect.left);
			}
		}

		RECT rcFillRect = rcRect;

		UINT uiShift = ((rcRect.bottom - rcRect.top) - bmInfo.bmHeight) / 2;

		HBRUSH hBrush;			// Brush to fill in the bar	
		hBrush = ::CreateSolidBrush( m_clrBk );
		if ( hBrush ) {
			rcFillRect.bottom = rcRect.top + uiShift;
			::FillRect( hWndDC, &rcFillRect,  hBrush );
			rcFillRect.top = rcRect.bottom - uiShift - 1;
			rcFillRect.bottom = rcRect.bottom;
			::FillRect( hWndDC, &rcFillRect, hBrush );
			::DeleteObject( hBrush );
		}

		rcRect.top = rcRect.top + uiShift;
		rcRect.bottom = rcRect.top + bmInfo.bmHeight;//rcRect.bottom - uiShift;
		::BitBlt( hWndDC, rcRect.left, rcRect.top, rcRect.right-rcRect.left, rcRect.bottom-rcRect.top, hMemDC, iStartPos, 0, SRCCOPY );

		::SelectObject( hMemDC, hOldBmp );
		::DeleteDC( hMemDC );
	}
}

// ---
void CSnakePainter::DrawGradient( HDC hMemDC, RECT rcGrad )	{
	
	RECT   rectFill;	// Rectangle for filling band
	float  fStep;     // How wide is each band?
	HBRUSH hBrush;			// Brush to fill in the bar	
	
	// First find out the largest color distance between the start and end colors.  This distance
	// will determine how many steps we use to carve up the client region and the size of each
	// gradient rect.
	int r, g, b;							// First distance, then starting value
	float rStep, gStep, bStep;		// Step size for each color
	
	// Get the color differences
	r = (GetRValue(m_clrCurEnd) - GetRValue(m_clrCurStart));
	g = (GetGValue(m_clrCurEnd) - GetGValue(m_clrCurStart));
	b = (GetBValue(m_clrCurEnd) - GetBValue(m_clrCurStart));
	
	
	// Make the number of steps equal to the greatest distance
	int nSteps = max(abs(r), max(abs(g), abs(b)));
	
	// Determine how large each band should be in order to cover the
	// client with nSteps bands (one for every color intensity level)
	fStep = ((float)abs(rcGrad.right-rcGrad.left)) / (float)nSteps;
	
	// Calculate the step size for each color
	rStep = r/(float)nSteps * (float)(m_bReverse ? -1 : 1);
	gStep = g/(float)nSteps * (float)(m_bReverse ? -1 : 1);
	bStep = b/(float)nSteps * (float)(m_bReverse ? -1 : 1);
	
	COLORREF clrF = m_bReverse ? m_clrCurStart : m_clrCurEnd;

	// Reset the colors to the starting position
	r = GetRValue(clrF);
	g = GetGValue(clrF);
	b = GetBValue(clrF);
	
	// Start filling bands
	int iOnBand;
	for ( iOnBand = 0; iOnBand <= (nSteps>>1); iOnBand++ ) {
		::SetRect(&rectFill,
							(int)(rcGrad.left+iOnBand * fStep),       // Upper left X
							rcGrad.top,									 // Upper left Y
							rcGrad.left+(int)((iOnBand+1) * fStep),          // Lower right X
							rcGrad.bottom);			// Lower right Y

		if(rectFill.left<rcGrad.left || rectFill.right>rcGrad.right)
			break;
		// CDC::FillSolidRect is faster, but it does not handle 8-bit color depth
		int iPos = (iOnBand<<1);
		hBrush = ::CreateSolidBrush( clrF = RGB(r-rStep*iPos, g-gStep*iPos, b-bStep*iPos) );
		::FillRect( hMemDC, &rectFill, hBrush );
		::DeleteObject( hBrush );
	}
	
	// Reset the colors to the starting position
	r = GetRValue(clrF);
	g = GetGValue(clrF);
	b = GetBValue(clrF);
	
	for ( iOnBand; iOnBand < nSteps; iOnBand++ ) {
		::SetRect(&rectFill,
							(int)(rcGrad.left+iOnBand * fStep),       // Upper left X
							rcGrad.top,									 // Upper left Y
							rcGrad.left+(int)((iOnBand+1) * fStep),          // Lower right X
							rcGrad.bottom);			// Lower right Y

		if(rectFill.left<rcGrad.left || rectFill.right>rcGrad.right)
			break;
		// CDC::FillSolidRect is faster, but it does not handle 8-bit color depth
		int iPos = (iOnBand<<1);
		hBrush = ::CreateSolidBrush( RGB(r+rStep*iPos, g+gStep*iPos, b+bStep*iPos) );
		if ( !hBrush )
			break;
		::FillRect( hMemDC, &rectFill, hBrush );
		::DeleteObject( hBrush );
	}
}

// ---
void CSnakePainter::Start() {
	Stop();
//	ResetProgress();
	DoPaint();
	if ( m_hWndHost ) {
		m_pThread = new CSnakePainterThread( m_hWndHost, m_dwTimeout );
		if ( m_pThread )
			m_pThread->Start();
	}
}


// ---
void CSnakePainter::Stop() {
	if ( m_pThread && m_pThread->GetStatus() == CThread::Running ) {
		m_pThread->Terminate();
		delete m_pThread;
		m_pThread = NULL;
	}
}

// ---
void CSnakePainter::DoPaint() {

	if ( m_hWndHost ) {
		HDC hWndDC = ::GetDC( m_hWndHost );
		RECT rect,rcLeft,rcRight,rcSnake;

		::GetClientRect( m_hWndHost, &rect );

		HDC hMemDC = ::CreateCompatibleDC( hWndDC );
		HBITMAP hBmp = :: CreateCompatibleBitmap( hWndDC, rect.right-rect.left, rect.bottom-rect.top );
		HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, hBmp );

		::CopyRect( &rcLeft, &rect );
		::CopyRect( &rcRight, &rect );
		::CopyRect( &rcSnake, &rect ); 

		float nTailpos = (float)m_nTail / (float)m_nMax * (float)rect.right;
		float nHeadpos = (float)m_nPos / (float)m_nMax * (float)rect.right;
		
		rcSnake.left = (long)(m_bReverse ? nHeadpos : nTailpos );
		rcSnake.right = (long)(m_bReverse ? nTailpos : nHeadpos );

		rcLeft.right = rcSnake.left;
		rcRight.left = rcSnake.right;

		if ( m_bGradient )
			DrawGradient( hMemDC, rcSnake );
		else
			DrawImage( hMemDC, rcSnake );

		HBRUSH hBrush;			// Brush to fill in the bar	
		hBrush = ::CreateSolidBrush( m_clrBk );
		if ( hBrush ) {
			::FillRect( hMemDC, &rcLeft,  hBrush );
			::FillRect( hMemDC, &rcRight, hBrush );
			::DeleteObject( hBrush );
		}

		::BitBlt( hWndDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, hMemDC, 0, 0, SRCCOPY );

		::SelectObject( hMemDC, hOldBmp );
		::DeleteObject( hBmp );
		::DeleteDC( hMemDC );

		::ReleaseDC( m_hWndHost, hWndDC );		

		::ValidateRect( m_hWndHost, NULL );
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// --
CSnakePainterThread::CSnakePainterThread( HWND hWndHost, DWORD dwTimeOut ) : 
	CThread(),
	m_hWndHost( hWndHost ),
	m_dwTimeOut( dwTimeOut ) {
}


// --
int	CSnakePainterThread::Run() {
	BOOL bStop = FALSE;

	while ( !bStop ) {
		::SendNotifyMessage( m_hWndHost, nStepSnap, 0, 0 );
		//::PostMessage( m_hWndHost, nStepSnap, 0, 0 );
		bStop = ::WaitForSingleObject( m_rcStopEvent, m_dwTimeOut ) == WAIT_OBJECT_0;
	}
	
	return 0;
}

// ---
void CSnakePainterThread::Terminate() {
	CThread::Terminate();
	m_rcStopEvent.Set();
	//MsgWaitForExit( NULL );
	WaitForExit();
}


