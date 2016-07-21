#include "stdafx.h"
#include "xpbrush.h"
#include "../Graphics.h"

//////////////////////////////////////////////////////////////////////
#define DP(cx, cy, cl)  SetPixel(dc, x+cx, y+cy, m_cl##cl)

void CXPBrush::Clear()
{
	if( m_hBrush ) ::DeleteObject(m_hBrush), m_hBrush = NULL;
	if( m_hFrame ) ::DeleteObject(m_hFrame), m_hFrame = NULL;
}

void CXPBrush::Init(COLORREF clDraw, COLORREF clBg, bool bEdged)
{
	m_clDraw = clDraw;
	m_clBg = clBg;
	m_clFrame = CAlphaBrush::Color(m_clDraw, m_clBg, bEdged ? 0.2 : 0.8);
	if( !bEdged )
		m_clFrame = RGB(GetRValue(m_clFrame)/2, GetGValue(m_clFrame)/2, GetBValue(m_clFrame)/2);
	m_hFrame = CreateSolidBrush(m_clFrame);
	m_hBrush = CreateSolidBrush(m_clDraw);

	m_cl0 = CAlphaBrush::Color(m_clFrame, 0x00FFFFFF, 0.8);
	m_cl1 = CAlphaBrush::Color(m_clFrame, 0x00FFFFFF, 0.6);
	m_cl2 = CAlphaBrush::Color(m_clFrame, m_clBg, 0.25);
	m_cl3 = CAlphaBrush::Color(m_clFrame, m_clBg, 0.6);
	m_cl4 = CAlphaBrush::Color(m_clFrame, m_clBg, 0.9);
	m_cl5 = CAlphaBrush::Color(0x00FFFFFF, m_clBg, 0.5);
	m_cl6 = CAlphaBrush::Color(0x00FFFFFF, m_clBg, 0.8);
	m_cl7 = CAlphaBrush::Color(0x00FFFFFF, m_clBg, 0.2);
}

bool CXPBrush::IsInited()
{
	return m_hBrush!=NULL;
}

void CXPBrush::DrawTopBorder(HDC dc, RECT& rc)
{
	GetTopBrush().Draw(dc, rc.left+1, rc.top, rc.right-1, rc.top+1);

	int x = rc.right, y = rc.top;
	DP(-3,3,1); DP(-4,2,1); 
	DP(-2,3,2); DP(-3,2,2); DP(-4,1,2); 
	DP(-2,2,3); DP(-3,1,3); 
	DP(-2,1,6); DP(-1,2,6); DP(-1,3,6); DP(-1,1,4);
	DP(-1,0,Bg);
}

void CXPBrush::DrawLeftBorder(HDC dc, RECT& rc)
{
	GetLeftBrush().Draw(dc, rc.left, rc.top+1, rc.left+1, rc.bottom-1);

	int x = rc.left, y = rc.top;
	DP(2,3,1); DP(3,2,1); 
	DP(1,3,2); DP(2,2,2); DP(3,1,2); 
	DP(1,2,3); DP(2,1,3); 
	DP(1,1,4);
	DP(0,0,Bg);
}

void CXPBrush::DrawBottomBorder(HDC dc, RECT& rc)
{
	GetBottomBrush().Draw(dc, rc.left+3, rc.bottom-1, rc.right-1, rc.bottom);

	int x = rc.left, y = rc.bottom;
	DP(2,-4,1); DP(3,-3,1); 
	DP(1,-4,2); DP(2,-3,2); DP(3,-2,2); 
	DP(1,-3,3); DP(2,-2,3); 
	DP(1,-2,5); DP(2,-1,5);
	DP(1,-1,6);
	DP(0,-1,Bg);
}

void CXPBrush::DrawRightBorder(HDC dc, RECT& rc)
{
	GetRightBrush().Draw(dc, rc.right-1, rc.top+4, rc.right, rc.bottom-4);

	int x = rc.right, y = rc.bottom;
	DP(-3,-4,1); DP(-4,-3,1); 
	DP(-2,-4,2); DP(-3,-3,2); DP(-4,-2,2); 
	DP(-2,-3,3); DP(-3,-2,3); 
	DP(-2,-2,7); DP(-1,-2,7); DP(-1,-3,5); DP(-1,-4,5);
	DP(-1,-1,Bg);
}

void CXPBrush::DrawGrayedConers(HDC dc, RECT& rc)
{
	int x = rc.left, y = rc.top;
	DP(1,1,4); DP(2,2,1); DP(3,2,0); DP(2,3,0);         y = rc.bottom;
	DP(1,-2,4); DP(2,-3,1); DP(3,-3,0); DP(2,-4,0);	    x = rc.right;
	DP(-2,-2,4); DP(-3,-3,1); DP(-4,-3,0); DP(-3,-4,0);	y = rc.top;
	DP(-2,1,4); DP(-3,2,1); DP(-4,2,0); DP(-3,3,0);
}

void CXPBrush::DrawGradint(HDC dc, RECT& rcDraw, int nState, bool bGradState)
{
	RECT rcGrad = rcDraw;

	COLORREF clDraw = m_clDraw;
	if( nState & ISTATE_DISABLED )
		clDraw = RGB(214,214,214);
	else
		rcGrad.bottom++;

	double nHeight = rcGrad.bottom - rcGrad.top;
	int    nStep = (int)nHeight/20; if( !nStep ) nStep = 1; 
	rcGrad.top = rcGrad.bottom - nStep;

	for(int i = 0; i < nHeight; i += nStep)
	{
		double d = ((nState & ISTATE_HOTLIGHT) ? 3.0 : 2.0)*(nHeight+i)/nHeight;
		double s = (nState & (ISTATE_SELECTED|ISTATE_DISABLED)) ? 1.3 : 0.5;
		double k = (1-sin(3.141592638*(i/nHeight-s)))/d;
		CAlphaBrush(0x00FFFFFF, clDraw, k).Draw(dc, rcGrad);

		if( bGradState && (nState & (ISTATE_HOTLIGHT|ISTATE_DEFAULT)) )
		{
			CAlphaBrush b1(HT1, HT4, k), b2(HT2, HT3, k);
			b1.Draw(dc, rcGrad.left-1, rcGrad.top, rcGrad.left, rcGrad.bottom);
			b1.Draw(dc, rcGrad.right+1, rcGrad.top, rcGrad.right, rcGrad.bottom);
			b2.Draw(dc, rcGrad.left, rcGrad.top, rcGrad.left+1, rcGrad.bottom);
			b2.Draw(dc, rcGrad.right-1, rcGrad.top, rcGrad.right, rcGrad.bottom);
		}

		OffsetRect(&rcGrad, 0, -nStep);
	}
}
